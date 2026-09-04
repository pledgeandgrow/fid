# Path to Self-Reproduction — Quick Reference

> Companion to `docs/roadmap/selfhost-roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 4, 2026 (session 8).

---

## Where We Are

```
✅ Lexer — works (stdlib/core.fd tokenized)
✅ Parser — works (stdlib + user code parsed)
✅ TypeChecker — works (hello.fd type checks ok)
✅ Lowerer — works (hello.fd lowered to IR)
✅ CCodeGen — works (hello.fd → C code → tcc → correct output)
✅ Self-compilation — WORKS (fides-c.exe → fides-c2_test.exe, EXIT 0)
✅ PE emitter — WORKS (fides-c.exe build → native .exe without tcc)
✅ HashMap import resolution — O(1) lookups (session 6)
✅ Cross-module enum constructors — MethodCall→EnumLit fallback (session 6)
✅ comptime_eval.fd — FULL PORT (process_gen, substitute, I-5A, I-5D, reflection)
✅ pe.fd runtime stubs — 25+ stubs (print, string, vec, map, float, bool)
✅ peephole.fd — PORTED (985 lines, from src/peephole.rs 903 lines)
✅ regalloc.fd — PORTED (1086 lines, from src/regalloc.rs 1096 lines)
✅ codegen_x64.fd — PORTED (1780 lines, from src/codegen_x64.rs 1923 lines)
⬜ Fixpoint — NOT YET VERIFIED (fides-c2 crashes on stdlib lexing)
⬜ Native codegen integration — ported but not wired into compiler pipeline
```

**MILESTONE (session 8):** Linux native codegen backend ported:
```
.\target\release\fidesc.exe build compiler\peephole.fd      → Built (OK)
.\target\release\fidesc.exe build compiler\regalloc.fd       → Built (OK)
.\target\release\fidesc.exe build compiler\codegen_x64.fd    → Built (OK)
```
Three files ported from Rust to Fides, all compile successfully:
- `peephole.fd` (985 lines) — 9 peephole optimization passes
- `regalloc.fd` (1086 lines) — linear scan register allocator
- `codegen_x64.fd` (1780 lines) — PIR L3 → x86-64 assembly (System V AMD64 ABI)
These form the Linux ELF64 native codegen pipeline. NOT yet integrated into
`compiler.fd`/`main.fd`. The Windows PE backend (`pe.rs`, 5704 lines) is a
separate self-contained codegen that does NOT use these files.

**MILESTONE (session 7):** comptime_eval.fd fully ported + pe.fd runtime stubs expanded:
```
.\target\release\fidesc.exe build compiler\comptime_eval.fd  → Built (OK)
.\target\release\fidesc.exe build compiler\pe.fd             → Built (OK)
.\target\release\fidesc.exe build compiler\compiler.fd       → Built (OK)
.\target\release\fidesc.exe build compiler\main.fd           → Built (OK)
.\target\release\fidesc.exe run tests\compiler\test_compiler.fd → All tests passed!
```

**MILESTONE (session 6):** Self-compile succeeds with all fixes:
```
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2_test.exe
→ [prof] typecheck: 4000ms
→ [prof] total build: 262000ms
→ [prof] codegen: 38000ms
→ Built native executable: release\fides-c2_test.exe
→ EXIT: 0  ✅
```

**MILESTONE (session 5):** The self-hosted compiler generates native PE executables:
```
.\release\fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe
→ Built native executable: release\hello_pe.exe
.\release\hello_pe.exe
→ Hello from PE!    ✅ CORRECT OUTPUT, EXIT CODE 0
```

**MILESTONE (session 4):** The self-hosted compiler compiles itself:
```
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2.exe
→ Built native executable: release\fides-c2.exe
```

The generated `fides-c2.exe` correctly compiles `test_strconcat.fd` → "hello world".

## The Steps to Fixpoint

### Step 1: Fix Vec-of-struct truncation ✅ DONE

Fixed `map_str_str_get` crash. All Vec paths now handle struct elements.

### Step 2: Port Fides lowerer gaps 1-5 ✅ DONE (session 3)

All 5 gaps ported to the Fides lowerer. Plus Gaps 6-11 ported in sessions 2-3.

### Step 3: hello.fd compiles without crash ✅ DONE

### Step 4: hello.fd output is correct ✅ DONE

Output: `Hello, Fides!50y is big`

### Step 5: Self-compilation (stage1) ✅ DONE (session 4)

```bash
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2.exe
→ Built native executable: release\fides-c2.exe
```

**Six bugs were fixed in session 4 to achieve this:**
1. Enum `==` comparison bug — replaced with `*_eq()` helpers
2. O(n²) string concat in C codegen — added `p_sb_*` string builder
3. Duplicate function definitions — `IrModule_add_function` deduplicates
4. `p_str_equal` env_ptr mismatch — added `dummy_env` to call sites
5. C stdlib `remove` collision — added to `sanitize_c_name`
6. Vec method mangling for I64 receivers — AST type lookup + prefix fallback

### Step 6: stage1 produces correct output ✅ DONE (session 4)

```bash
.\release\fides-c.exe ccompile test_strconcat.fd -o test_strconcat2
Copy-Item test_strconcat2 test_strconcat2.exe -Force
.\test_strconcat2.exe
→ hello world    ✅ CORRECT
```

### Step 6b: PE emitter generates native executables ✅ DONE (session 5)

```bash
.\release\fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe
.\release\hello_pe.exe
→ Hello from PE!    ✅ CORRECT
```

**Three bugs were fixed in session 5 to achieve this:**
1. `X86Encoder_mov_imm32` ModRM bug — `mod=0` (memory) → `mod=3` (register)
2. `Inst::Print` type dispatch — always called `p_print_int` → now dispatches by IR type
3. Statement-dropping bug workaround — split long functions into smaller helpers

### Step 6c: OptStr type resolution + HashMap imports ✅ DONE (session 6)

Three bugs were fixed to make self-compile succeed (EXIT 0):

1. **OptStr type resolution** — `OptStr::Some(input)` was parsed as `MethodCall`
   instead of `EnumLit` because the parser didn't know `OptStr` was a cross-module
   enum. Fixed in typechecker (`check_method_call` checks enum names), lowerer
   (`MethodCall` on enum name → delegate to `EnumLit`), and `infer_expr_type`.
2. **Duplicate `p_file_write_vec`** — `emit_runtime_impl` emitted a function body
   already in `emit_file_write_bin_impl`. Removed the duplicate.
3. **HashMap for import resolution** — replaced O(n) `c_str_vec_contains` scans for
   `loaded_module_names`, `struct_names`, `enum_names` with O(1) `HashMap_get`
   lookups. Import resolution dropped from ~238s to negligible.

### Step 7: Fixpoint ⏳ NEXT (blocked by statement-dropping bug)

```bash
# fides-c2 (self-compiled) compiles the compiler → fides-c3
.\release\fides-c2_test.exe ccompile compiler\main.fd -o release\fides-c3.exe
# Compare
diff release\fides-c2_test.exe.c release\fides-c3.exe.c
# Must be identical
```

**Current blockers for Step 7:**
1. **fides-c2.exe STATUS_ACCESS_VIOLATION** — second-generation compiler crashes
   during stdlib lexing. Root cause is the **statement-dropping bug** (bug 1.19
   in known-bugs.md): the Fides compiler silently drops statements in long
   functions. The second-generation compiler has this bug baked into its
   generated C code, so it crashes on complex inputs.
2. **Debug prints** — 38 `p_eprint_str` calls produce noisy stderr output

**Resolved blockers:**
- ~~`.exe` extension~~ — FIXED (session 8). `ensure_exe_extension()` in
  `compiler/main.fd` now appends `.exe` to output names in both `ccompile`
  and `build` commands. No more manual `Copy-Item` needed.

### Step 8: Native codegen integration (future)

Three Linux native codegen files are ported but not yet integrated:
- `compiler/peephole.fd` (985 lines) — peephole optimizer
- `compiler/regalloc.fd` (1086 lines) — register allocator
- `compiler/codegen_x64.fd` (1780 lines) — PIR L3 → x86-64 AT&T asm

**To integrate the Linux backend:**
1. Port `src/elf64.rs` → `compiler/elf64.fd` (ELF64 object file emitter)
2. Port `src/linker.rs` → `compiler/linker.fd` (linker with runtime)
3. Port `src/runtime_linux.rs` → `compiler/runtime_linux.fd` (Linux runtime)
4. Wire `codegen_x64::generate_asm()` + `peephole::optimize()` +
   `elf64::emit_object_file()` + `linker::link_with_runtime()` into
   `compiler.fd` as the `"x86-64"` target path
5. Add `--target x86-64` support to `main.fd`

**For Windows native codegen:**
- `pe.rs` (5704 lines) is the actual Windows backend — self-contained, does
  NOT use `codegen_x64.rs`/`regalloc.rs`/`peephole.rs`
- `pe.fd` is already partially ported (~2600 lines) with 25+ runtime stubs
- To complete: port remaining `pe.rs` functionality to `pe.fd`
- `x86.rs` (1377 lines) is DEAD CODE — skip it (see session 8 analysis)

## Rebuild Cycle

Every fix requires a full rebuild:

```bash
cargo build --release
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c.exe
# (patch PE stack to 256MB — see build-pipeline.md)
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2_test.exe
```

## What Success Looks Like

No crash ≠ working. The three failure modes after crashes stop:

1. **Wrong output** — program runs but prints wrong thing → compare C output against bootstrap compiler
2. **Fixpoint divergence** — stage2.c ≠ stage3.c → non-determinism (HashMap order, uninit memory)
3. **Correct + stable** — stage2.c == stage3.c → **fixpoint achieved**

We're at stage 1 (self-compilation works, EXIT 0, output is correct for simple
files) + PE emitter works for simple programs. The fixpoint check (stage 2-3)
is next, blocked by the fides-c2 stdlib crash.
