# Fides Self-Hosted Compiler — Build, Test, and Bootstrap Pipeline

> How to build, test, and verify the self-hosted compiler.

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

### Step 2: Compile the self-hosted compiler to C

```bash
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
```

Produces `release/fides-c.c`.

### Step 3: Compile the C to an executable with tcc

```bash
tcc -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def
```

**Why tcc?** It is fast, produces small binaries, and is what the bootstrap
pipeline currently targets. The Fides compiler can also target gcc/clang
when using the Rust host (`fidesc build`), but the self-hosted `main.fd`
currently hard-codes tcc.

**Why `ws2_32.def`?** The runtime uses Windows sockets for some networking
primitives. Linking `ws2_32.def` satisfies those imports.

### Step 4: Patch the PE stack to 256 MB

The self-hosted compiler needs a large stack (the parser is deeply recursive
and `alloca` is used throughout). TCC does not provide a way to set the
initial stack via a flag, so the PE header is patched:

```powershell
$bytes = [System.IO.File]::ReadAllBytes("release\fides-c.exe")
$peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
$stackOffset = $peOffset + 0x18 + 0x48
$newStack = [BitConverter]::GetBytes([int64]268435456)   # 256 MB
[Array]::Copy($newStack, 0, $bytes, $stackOffset, 8)
$newCommit = [BitConverter]::GetBytes([int64]16777216)   # 16 MB initial commit
[Array]::Copy($newCommit, 0, $bytes, $stackOffset + 8, 8)
[System.IO.File]::WriteAllBytes("release\fides-c.exe", $bytes)
```

### One-liner (PowerShell)

```powershell
$c = Get-Content build-selfhost.ps1 -Raw; Invoke-Expression $c
```

**Note:** `build-selfhost.ps1` was previously missing but has been recreated.
It performs all four steps in order.

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

### Current status

Not yet achieved. The current blocker is the enum variant extraction bug
(see `calling-conventions.md`). Once fixed, the fixpoint can be run with:

```bash
.\verify-fixpoint.ps1
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
├── compiler/           # Self-hosted Fides compiler modules (*.fd)
│   ├── main.fd         # CLI entry point
│   ├── compiler.fd     # Pipeline driver
│   ├── token.fd
│   ├── lexer.fd
│   ├── ast.fd
│   ├── parser.fd
│   ├── typechecker.fd
│   ├── lower.fd
│   ├── ir.fd
│   ├── pir.fd
│   ├── opt.fd
│   ├── mono.fd
│   ├── borrowck.fd
│   ├── codegen_c.fd
│   ├── fmt.fd
│   ├── diag.fd
│   ├── json.fd
│   └── comptime_eval.fd
├── docs/selfhost/      # This documentation
├── release/            # Self-hosted build artifacts (ignored by git, except .gitkeep)
├── src/                # Rust reference compiler
├── stdlib/             # Fides standard library (69 .fd modules)
├── tests/              # Language tests
├── tests/compiler/     # Self-host module tests
├── tests/selfhost/     # Bootstrap probe tests
├── target/             # Rust build artifacts
├── verify.sh           # Full Rust compiler verification
├── verify-selfhost.sh  # Self-host verification
├── verify-fixpoint.ps1 # Bootstrap fixpoint (Windows)
├── verify-stdout-diff.ps1
├── verify-stage-diff.ps1
└── build-selfhost.ps1  # Build the self-hosted compiler (Windows)
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
