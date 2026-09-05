# Path to Self-Reproduction — Quick Reference

> Companion to `docs/roadmap/selfhost-roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 5, 2026 (session 14 — Category B codegen parity resolved).

---

## Where We Are

```
✅ Lexer — works (stdlib/core.fd tokenized)
✅ Parser — works (stdlib + user code parsed)
✅ TypeChecker — works (hello.fd type checks ok)
✅ Lowerer — works (hello.fd lowered to IR)
✅ CCodeGen — works (hello.fd → C code → tcc → correct output)
✅ Self-compilation — WORKS (fides-c.exe → fides-c2.c, EXIT 0)
✅ PE emitter — WORKS (fides-c.exe build → native .exe without tcc)
✅ HashMap import resolution — O(1) lookups (session 6)
✅ Cross-module enum constructors — MethodCall→EnumLit fallback (session 6)
✅ comptime_eval.fd — FULL PORT (process_gen, substitute, I-5A, I-5D, reflection)
✅ pe.fd runtime stubs — 25+ stubs (print, string, vec, map, float, bool)
✅ peephole.fd — PORTED (1,064 lines, from src/peephole.rs 903 lines)
✅ regalloc.fd — PORTED (1,181 lines, from src/regalloc.rs 1,088 lines)
✅ codegen_x64.fd — PORTED (1,942 lines, from src/codegen_x64.rs 1,940 lines)
✅ lsp.fd — PORTED (1,082 lines, from src/lsp.rs 916 lines) — JSON-RPC over stdio
✅ flat_ast.fd — PORTED (1,259 lines, from src/flat_ast.rs 741 lines)
✅ Module-level `var`/`pub var` — IMPLEMENTED in Rust compiler (session 11)
✅ Full static analysis of all 25 compiler/*.fd files — COMPLETED (session 11)
✅ DCE removed from self-hosted ccompile — 1814 functions (matches Rust's 1813)
✅ TypeChecker_compute_func_effects rewritten — no extra boxing
✅ C codegen return type casts — (int64_t)/(void*)/(double) added
✅ Type alias resolution in Lowerer_mangle_method — StrStack→Vec resolved
✅ Category B codegen parity — B5 FIXED, B6/B7/B8 AT PARITY (session 14)
✅ Float bit-pattern preservation — p_f64_to_bits/p_f32_to_bits + union literals
✅ Lowerer_resolve_type recursive generic arg resolution (session 14)
⬜ Fixpoint — NOT YET ACHIEVED (fides-c2 crashes in TypeChecker_lookup)
⬜ Native codegen integration — ported but not wired into compiler pipeline
⬜ Mirror var feature to compiler/*.fd — NOT YET DONE (Rust-only so far)
⬜ Fix P0 bugs in parser.fd and lexer.fd — IDENTIFIED but not yet fixed
⬜ Fix inc.fd global_cache_* using var feature — IDENTIFIED but not yet done
⬜ Fix Category A crash blockers — TypeChecker_lookup null deref
```

**Session 14 summary:** Category B codegen parity gaps resolved. B5 (float
bit-pattern) fixed via `p_f64_to_bits`/`p_f32_to_bits` runtime functions +
union compound literals in ConstFloat codegen. B6 (print/println dispatch),
B7 (runtime sigs), B8 (Vec word_repr) verified at parity. The current
blocker is Category A: `fides-c2_new.exe` crashes during type checking in
`TypeChecker_lookup` (null pointer from `Vec_get` on empty scopes). See
`docs/roadmap/selfhost-roadmap.md` § "Comprehensive Gap Analysis" for the
full list of remaining gaps organized by priority (A: crash blockers,
B: codegen parity, C: pipeline features, D: safety verification,
E: fixpoint stability, F: language features).

**MILESTONE (session 14):** Category B codegen parity resolved:
```
B5 (float bits)    — FIXED: p_f64_to_bits/p_f32_to_bits + union compound literals
B6 (print dispatch) — AT PARITY: all type cases match Rust codegen
B7 (runtime sigs)  — AT PARITY: p_malloc_persistent/p_arena_save/restore registered
B8 (Vec word_repr) — AT PARITY: lower_method_call uses word_repr(elem_ty)
Files changed: src/codegen_c.rs, src/lower.rs, compiler/codegen_c.fd, compiler/lower.fd
```

**MILESTONE (session 9):** LSP server ported to Fides:
```
compiler/lsp.fd (1,082 lines) — full LSP server ported from src/lsp.rs (916 lines)
  - JSON-RPC 2.0 over stdio (Content-Length framing)
  - initialize / shutdown / exit
  - textDocument/didOpen / didChange / didClose
  - textDocument/publishDiagnostics (via get_diagnostics)
  - textDocument/completion (keywords + symbols + dot-access methods/fields)
  - textDocument/hover (function/struct/enum/const/type/trait/method signatures)
  - textDocument/definition (source search for fn/struct/enum/const/type/cap)
  - textDocument/rename (whole-word occurrence search + WorkspaceEdit)
Runtime support added to codegen_c.fd:
  - p_read_line (stdin line read, NULL on EOF)
  - p_read_stdin (exact N-byte read for LSP body)
  - p_stdin_eof (feof check for client disconnect)
  - p_flush_stdout (fflush for immediate message delivery)
'lsp' command added to compiler/main.fd → lsp_run()
```

**MILESTONE (session 8):** Linux native codegen backend ported:
```
.\target\release\fidesc.exe build compiler\peephole.fd      → Built (OK)
.\target\release\fidesc.exe build compiler\regalloc.fd       → Built (OK)
.\target\release\fidesc.exe build compiler\codegen_x64.fd    → Built (OK)
```
Three files ported from Rust to Fides, all compile successfully:
- `peephole.fd` (1,064 lines) — 9 peephole optimization passes
- `regalloc.fd` (1,181 lines) — linear scan register allocator
- `codegen_x64.fd` (1,942 lines) — PIR L3 → x86-64 assembly (System V AMD64 ABI)
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

### Step 7: Fixpoint ⏳ NEXT (blocked by Category A crash + var feature)

```bash
# fides-c2 (self-compiled) compiles the compiler → fides-c3
.\release\fides-c2_test.exe ccompile compiler\main.fd -o release\fides-c3.exe
# Compare
diff release\fides-c2_test.exe.c release\fides-c3.exe.c
# Must be identical
```

**Current blockers for Step 7 (session 14):**
1. **Category A: TypeChecker_lookup null deref** — `fides-c2_new.exe` crashes
   during type checking of `stdlib/core.fd`. `Vec_get` on an empty scope
   returns null, which is dereferenced. Fix: add null checks after `Vec_get`
   in `TypeChecker_lookup` and related functions.
2. **Category A: Statement-dropping bug** (bug 1.19) — still latent; may
   affect long functions. Session 12 found the primary crash cause was stack
   overflow (fixed via TCC stack patch), but statement-dropping remains a
   correctness risk.
3. **Debug prints** — 38 `p_eprint_str` calls produce noisy stderr output
   (Category E6).
4. **P0 bugs in parser.fd and lexer.fd** (session 11 static analysis):
   - `lexer.fd` — duplicate empty `str_filter_underscores` stub
   - `parser.fd` — `box_expr_deref` references 8 non-existent `Expr` variants
   - `parser.fd` — missing `return` in 4 parse functions
   - `parser.fd` — duplicate `var has_forward` in `parse_layer`/`parse_model`
   - `parser.fd` — `parse_use` version-pin always returns Err
   These must be fixed before the self-hosted compiler can compile its own
   parser and lexer modules correctly.
5. **`var` feature not yet mirrored to `compiler/*.fd`** — the Rust compiler
   now supports module-level `var`/`pub var`, but the Fides self-hosted
   compiler files (`ast.fd`, `parser.fd`, `typechecker.fd`, `lower.fd`,
   `codegen_c.fd`, `pe.fd`, `ir.fd`, etc.) have not yet been updated to
   parse, type-check, lower, and emit globals. This blocks using `var` in
   `inc.fd` to implement the global cache.

**Resolved blockers:**
- ~~`.exe` extension~~ — FIXED (session 8). `ensure_exe_extension()` in
  `compiler/main.fd` now appends `.exe` to output names in both `ccompile`
  and `build` commands. No more manual `Copy-Item` needed.
- ~~LSP port~~ — DONE (session 9). `compiler/lsp.fd` (1,082 lines) implements
  JSON-RPC over stdio with diagnostics, completion, hover, definition, rename.
  `lsp` command added to `main.fd`. Runtime I/O functions added to `codegen_c.fd`.
- ~~Pre-existing parse error at codegen_c.fd:973~~ — RESOLVED in earlier sessions.
- ~~Category B codegen parity gaps~~ — DONE (session 14: B5 fixed, B6/B7/B8 at parity).

### Step 8: Native codegen integration (future)

Three Linux native codegen files are ported but not yet integrated:
- `compiler/peephole.fd` (1,064 lines) — peephole optimizer
- `compiler/regalloc.fd` (1,181 lines) — register allocator
- `compiler/codegen_x64.fd` (1,942 lines) — PIR L3 → x86-64 AT&T asm

**To integrate the Linux backend:**
1. Port `src/elf64.rs` → `compiler/elf64.fd` (ELF64 object file emitter)
2. Port `src/linker.rs` → `compiler/linker.fd` (linker with runtime)
3. Port `src/runtime_linux.rs` → `compiler/runtime_linux.fd` (Linux runtime)
4. Wire `codegen_x64::generate_asm()` + `peephole::optimize()` +
   `elf64::emit_object_file()` + `linker::link_with_runtime()` into
   `compiler.fd` as the `"x86-64"` target path
5. Add `--target x86-64` support to `main.fd`

**For Windows native codegen:**
- `pe.rs` (6,345 lines) is the actual Windows backend — self-contained, does
  NOT use `codegen_x64.rs`/`regalloc.rs`/`peephole.rs`
- `pe.fd` is already partially ported (4,845 lines) with 25+ runtime stubs
- To complete: port remaining `pe.rs` functionality to `pe.fd`
- `x86.rs` (1,381 lines) is DEAD CODE — skip it (see session 8 analysis)

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
files) + PE emitter works for simple programs + LSP ported + var feature
implemented in Rust compiler + full static analysis completed. The fixpoint
check (stage 2-3) is next, blocked by the fides-c2 stdlib crash, a pre-existing
parse error in codegen_c.fd, and 5 P0 bugs in parser.fd/lexer.fd identified by
the session 11 static analysis.
