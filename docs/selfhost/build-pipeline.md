# Fides Self-Hosted Compiler — Build, Test, and Bootstrap Pipeline

> How to build, test, and verify the self-hosted compiler.
>
> **Last updated:** Sep 5, 2026 (session 15 — stack wrapper approach, 3-generation self-hosting)

---

## 1. Build Pipeline

### Step 1: Build the Rust host compiler

```bash
cargo build --release
```

Produces `target/release/fidesc.exe` and `target/release/pacte.exe`, etc.

For faster iteration (but slower execution):

```bash
cargo build
```

### Step 2: Generate the self-hosted compiler C source

```bash
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
```

Produces `release/fides-c.c`. The output is UTF-16 encoded on Windows and
may need conversion to UTF-8 before TCC can compile it:

```powershell
$content = [System.IO.File]::ReadAllText("release\fides-c.c", [System.Text.Encoding]::Unicode)
[System.IO.File]::WriteAllText("release\fides-c.c", $content, [System.Text.UTF8Encoding]::new($false))
```

### Step 3: Apply the stack wrapper

TCC produces executables with a 4KB default PE stack reserve. The Fides
compiler needs 256MB for its deeply recursive parser, type checker, and
codegen. The stack wrapper renames `main` → `fides_main` and adds a Windows
thread with a 256MB stack:

```powershell
$content = [System.IO.File]::ReadAllText("release\fides-c.c")
$content = $content -replace "int main\(int argc, char\*\* argv\)", "int fides_main(int argc, char** argv)"
$wrapper = @"

// === Stack wrapper ===
#include <windows.h>
typedef struct { int argc; char** argv; int ret; } MainArgs;
DWORD WINAPI thread_main(LPVOID param) {
    MainArgs* args = (MainArgs*)param;
    args->ret = fides_main(args->argc, args->argv);
    return 0;
}
int main(int argc, char** argv) {
    MainArgs args = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &args, 0, NULL);
    if (!h) return 1;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return args.ret;
}
"@
$content = $content + $wrapper
[System.IO.File]::WriteAllText("release\fides-c_wrapped.c", $content, [System.Text.Encoding]::ASCII)
```

**Why not PE header patching?** PE header patching (modifying
`SizeOfStackReserve` in the PE optional header) was attempted but was
fragile — some patched binaries were corrupted and crashed with
`0xC0000005`. The thread-based wrapper is more reliable and doesn't require
byte-level PE manipulation.

### Step 4: Compile the C to an executable with tcc

```bash
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release\fides-c.exe -lws2_32
```

**Why tcc?** It is fast, produces small binaries, and is what the bootstrap
pipeline currently targets. The Fides compiler can also target gcc/clang
when using the Rust host (`fidesc build`), but the self-hosted `main.fd`
currently hard-codes tcc.

**Why `-lws2_32`?** The runtime uses Windows sockets for some networking
primitives. Linking `ws2_32` satisfies those imports.

### Full build sequence (session 15)

```powershell
# 1. Build Rust compiler
cargo build --release

# 2. Generate first-gen C
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c

# 3. Convert UTF-16 → UTF-8
$content = [System.IO.File]::ReadAllText("release\fides-c.c", [System.Text.Encoding]::Unicode)
[System.IO.File]::WriteAllText("release\fides-c.c", $content, [System.Text.UTF8Encoding]::new($false))

# 4. Apply stack wrapper (see Step 3 above)
# → produces release\fides-c_wrapped.c

# 5. Compile with TCC
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release\fides-c.exe -lws2_32

# 6. Test
.\release\fides-c.exe dump-c release\test_simple.fd   # should EXIT 0

# 7. Self-host: gen 1 → gen 2
.\release\fides-c.exe dump-c compiler\main.fd > release\fides-c2.c
# (repeat steps 3-5 for fides-c2.c → fides-c2_new.exe)

# 8. Self-host: gen 2 → gen 3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# (repeat steps 3-5 for fides-c3.c → fides-c3.exe)

# 9. Verify gen 3 works
.\release\fides-c3.exe dump-c release\test_simple.fd   # should EXIT 0
```

---

## 1b. PE Emitter Pipeline (session 5+)

The self-hosted compiler now has a `build` command that generates native
Windows PE executables **directly from `.fd` source** — no tcc, no gcc, no
external linker. This uses the PE emitter ported in session 5
(`compiler/pe.fd`).

### Build a PE executable

```bash
.\release\fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe
.\release\hello_pe.exe
# → Hello from PE!
```

### How it works

1. The `build` command in `main.fd` compiles the `.fd` source through the
   normal pipeline (lex, parse, typecheck, lower, mono).
2. Instead of generating C and invoking tcc, it calls `PeCodeGen` from
   `compiler/pe.fd` to generate x86-64 machine code + PE file format.
3. The PE bytes are hex-encoded (because `p_file_write` truncates at null
   bytes) and written to a `.hex` file.
4. PowerShell decodes the hex file to binary and writes the `.exe`.

### Current limitations

The PE emitter currently supports:
- ✅ PE header generation, import table (GetStdHandle, WriteFile, ExitProcess)
- ✅ Entry point → main, function prologues/epilogues, stack frames
- ✅ `p_print_str`, `p_print_int`, `p_print_float`, `p_print_bool`, `p_panic`
- ✅ String literals (.rdata section)
- ✅ Basic arithmetic, local variables, control flow
- ✅ String concat (`p_str_concat`), int/bool/float→string
- ✅ Vec runtime (new/push/len/get/pop/set/first/last/...)
- ✅ HashMap runtime (new/insert/get/contains/...)

The PE emitter does NOT yet support:
- ⏳ Full struct/enum support (memcpy, SRET)
- ⏳ Float arithmetic operations (xmm ops)
- ⏳ Many-arg function calls (>4 args)
- ⏳ Full compiler source compilation
- ⏳ 20+ runtime stubs return placeholder values (see architecture.md pe.fd section)

See SH-13 sub-tasks in `docs/roadmap/selfhost-roadmap.md` for the full list.

---

## 2. Bootstrap Pipeline

### Goal: prove the self-hosted compiler is self-reproducing

```
stage0: target/release/fidesc.exe (Rust compiler)
   │
   ├─► ccompile compiler/main.fd → stage1.c (release/fides-c.c)
   │
stage1: release/fides-c.exe (C compiled from stage1.c)
   │
   ├─► ccompile compiler/main.fd → stage2.c
   │
stage2: release/stage2.exe
   │
   ├─► ccompile compiler/main.fd → stage3.c
   │
compare: diff stage2.c stage3.c  → must be identical
```

If `stage2.c` and `stage3.c` are identical, the compiler can reproduce
itself — this is the bootstrap fixpoint.

### Why the fixpoint matters

It catches "compiles but wrong output" bugs that assertion tests miss. If
the self-hosted compiler generates C that is functionally different from
the Rust compiler's C output, the fixpoint will diverge.

### Current status (session 15)

**BOOTSTRAP FIXPOINT ACHIEVED.** `fides-c2.c` is byte-identical to
`fides-c3.c` (SHA256-identical, 701,100 lines, 19.8MB). The self-hosted
compiler reproduces itself exactly across generations.

**Self-compilation works (session 4).** `fides-c.exe` successfully compiles
`compiler/main.fd` → `fides-c2.exe`. **PE emitter works (session 5).**
`fides-c.exe build tests\hello_pe.fd` → working `.exe` without tcc.
**Linux native codegen ported (session 8)** — `peephole.fd`, `regalloc.fd`,
`codegen_x64.fd` build OK but are not yet integrated. **`.exe` extension
auto-appended (session 8)** — no more manual `Copy-Item` needed.
**Category B codegen parity resolved (session 14)** — B5 (float bits) fixed,
B6/B7/B8 verified at parity.
**Category A crash blockers resolved (session 15)** — TCC 4KB stack overflow
fixed with 256MB stack wrapper; type_eq str_equal fix; large match split;
non-ASCII → ASCII.
**Category E fixpoint stability resolved (session 15)** — c2==c3 proves no
non-determinism.

The fixpoint verification command:

```bash
# fides-c2 (self-compiled) compiles the compiler → fides-c3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# Compare
diff release\fides-c2.c release\fides-c3.c
# Result: 0 differences out of 701,100 lines — BYTE IDENTICAL
```

---

## 3. Test Commands

### Full Rust compiler verification

```bash
./verify.sh
```

Runs `cargo test`, the interpreter, C codegen, and PE emitter over the
full `tests/` suite. **Run this before and after any session.**

Save a new baseline:

```bash
./verify.sh --save
```

Quick build check (no tests):

```bash
./verify.sh --quick
```

### Self-host verification

```bash
./verify-selfhost.sh
```

Runs every `tests/compiler/*.fd` on three backends:
- Interpreter (`fidesc run`)
- C Codegen (`fidesc run --backend c`) — in-process only
- PE Emitter (`fidesc run --backend pe`) — in-process only

Compares results against `tests/compiler/baseline/test_results.txt`.

Options:

```bash
./verify-selfhost.sh --save    # update baseline
./verify-selfhost.sh --quick   # build only
./verify-selfhost.sh --verbose # full output
./verify-selfhost.sh --list    # list tests
```

### Single test

```bash
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
./target/release/fidesc run tests/<name>.fd --backend pe
```

### Differential tests

```bash
.\verify-stdout-diff.ps1        # compare stdout across backends
.\verify-stdout-diff.ps1 -Quick # skip interpreter
.\verify-stdout-diff.ps1 -File test_borrowck
.\verify-stdout-diff.ps1 -Verbose

.\verify-stage-diff.ps1         # compare per-stage output
.\verify-stage-diff.ps1 -File test_borrowck
.\verify-stage-diff.ps1 -Stage c    # C output only
.\verify-stage-diff.ps1 -Stage ir   # IR output only
.\verify-stage-diff.ps1 -Stage tokens # tokens only
```

### Debug a broken layer

```bash
./scripts/bisect.sh <test_name>
```

Tells you which layer (lexer, IR, C codegen, etc.) first diverges.

---

## 4. What Each Test Suite Does

### `tests/`

The main language test suite, run against the Rust compiler's three backends:
- **Interpreter** (`run`): reference oracle
- **C Codegen** (`run --backend c`): C99 backend
- **PE Emitter** (`run --backend pe`): native x86-64 backend

Subdirectories:
- `tests/capability/`: capability tests
- `tests/effects/`: effect system tests
- `tests/regions/`: region tests
- `tests/safety/`: expected-fail safety tests (panic on OOB, div by zero, etc.)

### `tests/compiler/`

23 self-host tests for the Fides compiler modules:
- `test_ast.fd`
- `test_borrowck.fd`
- `test_codegen_c.fd`
- `test_compiler.fd`
- `test_diag.fd`
- `test_fmt.fd`
- `test_fmt_debug.fd`
- `test_fmt_diff.fd`
- `test_fmt_real.fd`
- `test_fmt_tok.fd`
- `test_ir.fd`
- `test_json.fd`
- `test_json_min2.fd`
- `test_json_ref.fd`
- `test_json_ref2.fd`
- `test_lower.fd`
- `test_mono.fd`
- `test_opt.fd`
- `test_pir.fd`
- `test_struct_lit.fd`
- `test_struct_simple.fd`
- `test_struct_str.fd`
- `test_token.fd`

### `tests/selfhost/`

Probe tests for self-hosting behavior:
- `probe2.fd`
- `probe_argc.fd`
- `probe_argc2.fd`
- `probe_import.fd`
- `probe_import2.fd`
- `probe_import3.fd`
- `probe_import4.fd`
- `probe_parser_direct.fd`
- `probe_parser_direct2.fd`
- `probe_parser_simple.fd`
- `test_fmt_harness.fd`
- `test_lexer_harness.fd`
- `test_mut.fd`
- `test_parser_harness.fd`
- `test_sl_debug.fd`
- `vec_expr_test.fd`
- `vec_fieldinit_test.fd`

---

## 5. Directory Layout

```
p-lang/
├── compiler/             # Self-hosted Fides compiler modules (25 .fd files, 57,021 LOC)
│   ├── main.fd           # CLI entry point (ccompile + build + lsp + test + fmt + ...)
│   ├── compiler.fd       # Pipeline driver
│   ├── token.fd          # Token kinds (133 variants)
│   ├── lexer.fd          # Tokenizer
│   ├── ast.fd            # AST types + Box<T> unbox helpers
│   ├── parser.fd         # Recursive-descent parser
│   ├── typechecker.fd    # Type/effect/region/capability/safety checker
│   ├── lower.fd          # AST → SSA IR lowering
│   ├── ir.fd             # IR types, instructions, modules
│   ├── pir.fd            # Multi-level PIR (L1→L2→L3)
│   ├── opt.fd            # PIR L2 optimizer (20+ passes)
│   ├── mono.fd           # Monomorphization
│   ├── borrowck.fd       # Borrow checker
│   ├── codegen_c.fd      # C99 code generator + runtime
│   ├── pe.fd             # PE emitter — native x86-64 + PE file writer (session 5)
│   ├── peephole.fd       # Peephole optimizer — 9 passes (session 8, not yet integrated)
│   ├── regalloc.fd       # Linear scan register allocator (session 8, not yet integrated)
│   ├── codegen_x64.fd    # PIR L3 → x86-64 AT&T asm, Linux (session 8, not yet integrated)
│   ├── comptime_eval.fd  # Comptime eval — full port (session 7)
│   ├── flat_ast.fd       # Flat AST serialization (session 9)
│   ├── lsp.fd            # LSP server — JSON-RPC over stdio (session 9)
│   ├── fmt.fd            # Source formatter
│   ├── diag.fd           # Structured diagnostics
│   ├── json.fd           # JSON parser/serializer
│   └── inc.fd            # Incremental compilation cache
├── docs/selfhost/        # This documentation (13 .md files)
├── release/              # Self-hosted build artifacts (gitignored)
├── src/                  # Rust reference compiler (52 .rs files, 84,447 LOC)
├── stdlib/               # Fides standard library (69 .fd modules, 16,090 LOC)
├── tests/                # Language tests (225+ .fd files)
├── tests/compiler/       # Self-host module tests (23 .fd files)
├── tests/selfhost/       # Bootstrap probe tests
├── target/               # Rust build artifacts
├── verify.sh             # Full Rust compiler verification
├── verify-selfhost.sh    # Self-host verification
├── verify-fixpoint.ps1   # Bootstrap fixpoint (Windows)
├── verify-stdout-diff.ps1
├── verify-stage-diff.ps1
└── build-selfhost.ps1    # Build the self-hosted compiler (Windows)
```

---

## 6. Windows-Specific Notes

### tcc location

TCC is installed at `C:\tcc\`:
- `C:\tcc\tcc.exe` — main compiler
- `C:\tcc\lib\ws2_32.def` — Windows sockets import library
- `C:\tcc\lib\libtcc1-64.a` — 64-bit TCC runtime
- `C:\tcc\include\` — C headers

### Why standalone PE/C binaries can fail

`verify-selfhost.sh` runs the C and PE backends in-process because standalone
binaries can segfault:
- C binaries can segfault on complex string concatenation when stdout is
  redirected.
- PE binaries can segfault when stdout is redirected.

In-process `run --backend c` / `run --backend pe` avoids these issues.

### Stack size

The self-hosted compiler needs a large stack. Two mechanisms:
1. **PE stack patching** after `tcc` links the executable (256 MB reserve).
2. **Windows 16 MB stack thread wrapper** in `src/codegen_c.rs` (`real_main` in a `CreateThread` with 16 MB) for Rust-compiled binaries.
