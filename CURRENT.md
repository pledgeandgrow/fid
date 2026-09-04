# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Sep 4, 2026 (session 7).

## Where We Stopped (Sep 4 session 7 — pick up here)

### MILESTONE: comptime_eval.fd full port + pe.fd runtime stubs + visibility fixes

Three major pieces of work completed in session 7:

1. **`comptime_eval.fd` full port** — Replaced the 39-line stub with a
   ~1500-line port of `src/comptime.rs` (1345 LOC). Features: `process_gen`,
   `substitute`, I-5A aggressive comptime, I-5D loop unrolling, full AST
   interpreter, reflection (TypeName, IsStruct, IsEnum, FieldCount,
   FieldNames, FieldTypes, EnumVariants), ComptimeEnv with parallel Vecs.

2. **`pe.fd` runtime stubs** — Added 20+ missing runtime stubs to the PE
   emitter: print (bool, float), string (concat, to_str variants), Vec
   (new/push/len/get/pop/set/is_empty/clear/capacity/first/last), HashMap
   (new/len/is_empty/contains/get/insert/remove). Also added
   `X86Encoder_div_r` encoder helper.

3. **Visibility + struct fixes** — Made `str_starts_with` and `binop_eq` in
   `opt.fd` public. Fixed `I64Pair` struct field mismatch between `opt.fd`
   (key/val) and `compiler.fd` (first/second).

**Build verification:** All 18 self-host modules build successfully with the
Rust bootstrap compiler. Self-host tests pass (test_compiler, test_ast,
test_lower all exit 0 with "All tests passed!").

### Previous milestone (session 6): Self-compile succeeds + HashMap import optimization

```
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2_test.exe
→ [prof] typecheck: 4000ms
→ [prof] borrowck: 1000ms
→ [prof] mono: 1000ms
→ [build] Done. Program built successfully.
→ [prof] total build: 262000ms
→ [prof] lower: 4000ms
→ [prof] codegen: 38000ms
→ Built native executable: release\fides-c2_test.exe
→ EXIT: 0  ✅
```

**The self-hosted compiler (`fides-c.exe`) compiles itself to a working
second-generation binary (`fides-c2_test.exe`, 8.9MB).** All pipeline stages
pass: typecheck, borrowck, mono, lower, C codegen, tcc link.

The fixpoint (fides-c2 compiling main.fd → fides-c3) is blocked by a
pre-existing `STATUS_ACCESS_VIOLATION` crash in the self-compiled binary
during stdlib lexing — same bug as session 4/5, unrelated to session 6 fixes.

### Session 7 work completed:
1. **comptime_eval.fd full port** — `process_gen`, `substitute`, `evaluate`, `evaluate_with_env`, `evaluate_block`, `evaluate_reflect`, I-5A aggressive comptime, I-5D loop unrolling, reflection helpers, `box_expr_unbox`/`box_expr_new`
2. **pe.fd runtime stubs** — 20+ stubs added (print, string, vec, map, float, bool)
3. **Visibility fixes** — `str_starts_with` and `binop_eq` made `pub` in `opt.fd`
4. **I64Pair struct fix** — unified field names to `first`/`second` across `opt.fd` and `compiler.fd`

### What works:
1. **Self-compilation** — `fides-c.exe` compiles `compiler/main.fd` → `fides-c2_test.exe` ✅ (EXIT 0)
2. **All 18 self-host modules compile + pass tests** (23/23 on tests/compiler/ via interpreter + PE)
3. **Build pipeline complete** — `compiler/*.fd` compiles to C via bootstrap, links via tcc
4. **All pipeline stages pass** during self-compilation: lex, parse, typecheck, borrowck, mono, lower, C codegen, file write, tcc link
5. **SRET calling convention** — struct-returning functions use hidden out-parameter
6. **Enum layout** — payload sizes computed from actual field sizes with fixpoint iteration
7. **Enum `==` comparison** — all enum comparisons use `*_eq()` helper functions (not broken `==`)
8. **String builder (`p_sb_*`)** — mutable string builder eliminates O(n²) memory in C codegen
9. **Vec-of-struct handling** — all Vec paths handle struct elements correctly
10. **Safety hardening** — recursion limit, OOM abort, crash handlers, Windows stack
11. **PE stack patching** — 256MB stack reserve for tcc-linked binaries
12. **All 12 lowerer/codegen gaps ported** (Gaps 1-11 done, Gap 12 deferred)
13. **Cross-module enum constructors** — `MethodCall` on enum names is handled in typechecker + lowerer (session 6)
14. **HashMap-based import resolution** — O(1) module/struct/enum name lookups (session 6)
15. **comptime_eval.fd full port** — process_gen, substitute, I-5A, I-5D, reflection (session 7)
16. **pe.fd runtime stubs** — 25+ stubs for print, string, vec, map, float, bool (session 7)

### What's next:
1. **Debug fides-c2.exe STATUS_ACCESS_VIOLATION** — second-generation compiler crashes during stdlib lexing. This is the #1 blocker for fixpoint verification. Likely a pre-existing lexer/recursion bug that only manifests in the self-compiled binary.
2. **Fix `.exe` extension** — self-hosted compiler doesn't add `.exe` on Windows
3. **Remove/gate debug prints** — 38 `p_eprint_str` calls in compiler.fd need gating
4. **Fixpoint verification** — compare fides-c2.c against fides-c3.c (the ultimate goal)
5. **pe.fd full struct/enum support** — needed for PE emitter to handle full compiler source
6. **pe.fd float arithmetic** — xmm operations for float math
7. **pe.fd many-arg calls** — stack-based argument passing for >4 args

### Performance (session 6):
- **Self-compile time: ~305s (5:05)** — import resolution is no longer the bottleneck
- HashMap optimization reduced import resolution from ~238s to negligible
- Remaining time is dominated by `total build` (262s) which includes parsing all 18 modules
- Profiling breakdown: stdlib parse 6s, typecheck 4s, borrowck 1s, mono 1s, lower 4s, codegen 38s

See `docs/roadmap/selfhost-roadmap.md` for the full plan.

## Temporary workarounds (to remove after fixpoint):
- `apply_cfg_filter` is a no-op in `compiler/compiler.fd`
- `compute_func_effects` is a no-op in `compiler/typechecker.fd`
- Debug prints in `compiler/parser.fd` (DEBUG parse_unary, DEBUG assign)
- 38 `p_eprint_str` calls in `compiler.fd` and `main.fd`

## Self-Host Module Status

All 18 modules rewritten in Fides. 23/23 self-host tests pass on interpreter + PE.

| Module | LOC | Status |
|--------|-----|--------|
| token.fd | 517 | DONE |
| diag.fd | 594 | DONE |
| json.fd | 628 | DONE |
| fmt.fd | 1007 | DONE |
| ast.fd | 920 | DONE |
| lexer.fd | 673 | DONE |
| parser.fd | 4800 | DONE |
| ir.fd | 2139 | DONE |
| typechecker.fd | 4848 | DONE (effects no-op) |
| lower.fd | 4444 | DONE |
| mono.fd | 2422 | DONE |
| borrowck.fd | 1731 | DONE |
| pir.fd | 2432 | DONE |
| opt.fd | 6022 | DONE |
| codegen_c.fd | 4529 | DONE |
| compiler.fd | 1641 | DONE (cfg no-op) |
| comptime_eval.fd | ~1500 | DONE (full port, session 7) |
| main.fd | 120 | DONE |
| **Total** | **39,502** | **18/18** |

## Deferred (Do Not Start)

- PE emitter rewrite (after C codegen self-host lands)
- Playground website (post-self-host)
- Package registry (post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (medical, blockchain, robotics)

---

## Build Commands

```bash
# Build bootstrap compiler (Rust)
cargo build --release

# Build self-hosted compiler (Fides → C → .exe)
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
# (tcc link is now automatic via the ccompile command)

# Patch PE stack to 256MB (REQUIRED for tcc-linked binaries)
$bytes = [System.IO.File]::ReadAllBytes("release\fides-c.exe")
$peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
$stackOffset = $peOffset + 0x18 + 0x48
[Array]::Copy([BitConverter]::GetBytes([int64]268435456), 0, $bytes, $stackOffset, 8)
[Array]::Copy([BitConverter]::GetBytes([int64]268435456), 0, $bytes, $stackOffset + 8, 8)
[System.IO.File]::WriteAllBytes("release\fides-c.exe", $bytes)

# Self-compilation test
.\release\fides-c.exe ccompile compiler\main.fd -o release\fides-c2.exe
# → Built native executable: release\fides-c2.exe

# Test on a file
.\release\fides-c.exe ccompile test_strconcat.fd -o test_strconcat2
Copy-Item test_strconcat2 test_strconcat2.exe -Force
.\test_strconcat2.exe
# → hello world
```
