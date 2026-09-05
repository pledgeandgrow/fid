# Path to Self-Reproduction — Quick Reference

> Companion to `docs/roadmap/selfhost-roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 5, 2026 (session 15 — **BOOTSTRAP FIXPOINT ACHIEVED**: fides-c2.c == fides-c3.c, 0 diffs / 701,100 lines).

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
✅ Category A crash blockers RESOLVED (session 15):
  - A1 root cause: TCC 4KB stack → stack overflow → fixed with 256MB stack wrapper
  - type_eq string comparison: pointer == → str_equal (Named/Generic/Dyn/Assoc)
  - Large match expression codegen: split into helper functions (mono.fd, comptime_eval.fd)
✅ Self-hosting across 3 generations VERIFIED (session 15):
  - Gen 1 (fides-c.exe) → builds gen 2 C → runs test_simple.fd ✓
  - Gen 2 (fides-c2_new.exe) → builds gen 3 C → runs test_simple.fd ✓
  - Gen 3 (fides-c3.exe) → runs test_simple.fd ✓
✅ **BOOTSTRAP FIXPOINT ACHIEVED** (session 15): fides-c2.c == fides-c3.c (0 diffs / 701,100 lines)
⬜ Native codegen integration — ported but not wired into compiler pipeline
⬜ Mirror var feature to compiler/*.fd — NOT YET DONE (Rust-only so far)
⬜ Fix P0 bugs in parser.fd and lexer.fd — IDENTIFIED but not yet fixed
⬜ Fix inc.fd global_cache_* using var feature — IDENTIFIED but not yet done
```

**Session 15 summary:** **BOOTSTRAP FIXPOINT ACHIEVED.** Self-hosting works
across 3 generations with byte-identical output (fides-c2.c == fides-c3.c,
0 diffs out of 701,100 lines). Four bugs were fixed:
1. TCC 4KB stack overflow → 256MB stack thread wrapper
2. `type_eq` pointer `==` for strings → `str_equal` (Named/Generic/Dyn/Assoc)
3. Large match expressions in `mono.fd`/`comptime_eval.fd` → split into helpers
4. Non-ASCII characters in compiler source → replaced with ASCII equivalents
   (corrupted Unicode `─`/`═`/`→`/`—` in comments/strings caused different
   output between generations; replacing with `-`/`=`/`->`/`--` fixed it)

**MILESTONE (session 15):** Self-hosting achieved across 3 generations:
```
Gen 1: Rust → fides-c.c → TCC → fides-c.exe     → runs test_simple.fd ✓
Gen 2: Gen 1 → fides-c2.c → TCC → fides-c2.exe  → runs test_simple.fd ✓
Gen 3: Gen 2 → fides-c3.c → TCC → fides-c3.exe  → runs test_simple.fd ✓
```
Three bugs fixed:
1. TCC 4KB stack overflow → 256MB stack thread wrapper
2. type_eq pointer == for strings → str_equal (Named/Generic/Dyn/Assoc)
3. Large match expressions in mono.fd/comptime_eval.fd → split into helpers

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

### Step 7: Self-hosting across generations ✅ DONE (session 15)

```bash
# Gen 1: Rust → C → TCC → fides-c.exe
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
# (convert UTF-16→UTF-8, apply stack wrapper, compile with TCC)
.\release\fides-c.exe dump-c release\test_simple.fd   # ✓ EXIT 0

# Gen 2: Gen 1 → C → TCC → fides-c2_new.exe
.\release\fides-c.exe dump-c compiler\main.fd > release\fides-c2.c
# (convert, wrap, compile)
.\release\fides-c2_new.exe dump-c release\test_simple.fd   # ✓ EXIT 0

# Gen 3: Gen 2 → C → TCC → fides-c3.exe
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# (convert, wrap, compile)
.\release\fides-c3.exe dump-c release\test_simple.fd   # ✓ EXIT 0
```

**Three bugs were fixed in session 15 to achieve this:**
1. **TCC 4KB stack overflow** — TCC produces executables with a 4KB default
   PE stack reserve. The compiler's deeply recursive parser, type checker,
   and C codegen need far more stack. Fixed by wrapping `main` in a Windows
   thread with a 256MB stack (`CreateThread(NULL, 256*1024*1024, ...)`).
2. **`type_eq` string comparison** — `type_eq` in `compiler/typechecker.fd`
   used pointer `==` for string comparisons (`n1 == n2`), which compares
   addresses instead of content. This caused identical generic types
   (`Vec<Param>` vs `Vec<Param>`) to be rejected as incompatible. Fixed by
   replacing with `str_equal(n1, n2)` for Named, Generic, Dyn, and Assoc
   type variants.
3. **Large match expression codegen** — `subst_expr`/`subst_stmt` in
   `mono.fd` and comptime cases in `comptime_eval.fd` were split into helper
   functions (`subst_expr_part2`, `subst_stmt_part2`,
   `substitute_expr_comptime`, etc.) to avoid codegen issues with oversized
   match arms.

### Step 7b: Strict fixpoint ✅ DONE (session 15)

```bash
# Compare gen 2 vs gen 3
diff release\fides-c2.c release\fides-c3.c
# Result: 0 differences out of 701,100 lines — BYTE IDENTICAL
```

The fixpoint was achieved by replacing all non-ASCII characters in the 25
`compiler/*.fd` source files with ASCII equivalents. The corrupted Unicode
characters (box-drawing `─`/`═`, arrows `→`, em-dashes `—`, and their
mojibake variants) in comments and string literals were causing different
output between generations because the Rust compiler and self-hosted compiler
handled multi-byte UTF-8 sequences differently in string literals.

**Replacements made:**
- `─` (U+2500) → `-`
- `═` (U+2550) → `=`
- `—` (U+2014) → `--`
- `→` (U+2192) → `->`
- `↔` (U+2194) → `<->`
- `≤` (U+2264) → `<=`
- `×` (U+00D7) → `x`
- `²` (U+00B2) → `^2`
- `│` (U+2502) → `|`
- `▶` (U+25B6) → `>`
- Mojibake sequences (`ΓöÇ` etc.) → `-`

**Remaining work (not blocking fixpoint):**
1. **P0 bugs in parser.fd and lexer.fd** (session 11 static analysis):
   - `lexer.fd` — duplicate empty `str_filter_underscores` stub
   - `parser.fd` — `box_expr_deref` references 8 non-existent `Expr` variants
   - `parser.fd` — missing `return` in 4 parse functions
   - `parser.fd` — duplicate `var has_forward` in `parse_layer`/`parse_model`
   - `parser.fd` — `parse_use` version-pin always returns Err
2. **`var` feature not yet mirrored to `compiler/*.fd`** — the Rust compiler
   now supports module-level `var`/`pub var`, but the Fides self-hosted
   compiler files have not yet been updated to parse, type-check, lower, and
   emit globals.

**Resolved blockers:**
- ~~Category A crash blockers~~ — DONE (session 15: stack wrapper + type_eq fix)
- ~~`.exe` extension~~ — FIXED (session 8).
- ~~LSP port~~ — DONE (session 9).
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
# Generate first-gen C
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
# Convert UTF-16 → UTF-8, strip BOM
# Apply stack wrapper (256MB thread — see build-pipeline.md)
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release\fides-c.exe -lws2_32
# Self-host: gen 1 → gen 2
.\release\fides-c.exe dump-c compiler\main.fd > release\fides-c2.c
# Convert, wrap, compile → fides-c2_new.exe
# Self-host: gen 2 → gen 3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# Compare fides-c2.c vs fides-c3.c for fixpoint check
```

## What Success Looks Like

No crash ≠ working. The three failure modes after crashes stop:

1. **Wrong output** — program runs but prints wrong thing → compare C output against bootstrap compiler
2. **Fixpoint divergence** — stage2.c ≠ stage3.c → non-determinism (HashMap order, uninit memory)
3. **Correct + stable** — stage2.c == stage3.c → **fixpoint achieved**

**We are at stage 3** (session 15): **FIXPOINT ACHIEVED.** Self-hosting
works across 3 generations, all produce correct output, and stage2.c ==
stage3.c (byte-identical, 0 diffs out of 701,100 lines). The bootstrap
fixpoint is complete.
