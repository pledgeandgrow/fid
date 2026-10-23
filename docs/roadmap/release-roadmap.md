# Fides Release Roadmap — Goals Derived from Self-Hosted Code Analysis

> Every goal below is backed by direct evidence from `compiler/*.fd` (100K LOC)
> and `stdlib/*.fd` (30K LOC). Counts are measured, not estimated.
> Created: Oct 21, 2026. Baseline: v1.0.0 (released Sep 9, 2026).

---

## The Core Finding

The self-hosted compiler exercises only a **narrow safe subset** of the language:
`struct` / `enum` / `match` / `while` / `var` / `fn` / `Vec<T>` / `str_equal`.

| Feature in grammar | Uses in 100K LOC compiler |
|--------------------|---------------------------|
| `for x in …` loops | **0** (2,944 `while` loops) |
| `?` / `try` / `?.` / `??` operators | **0** |
| Generic functions `fn f<T>` | **0** |
| Closures | **0** |
| Traits / `dyn` / `impl X for Y` | **0** |
| `comptime` | **0** |
| `@annotations` (`@linear`, `@crashfree`, …) | **0** |
| `region` / `sandbox` / `defer` / `spawn` / `select` | **0** |
| Effect annotations `!IO` | **0** |
| f-strings (lexed, but stub — parser.fd:5148) | **0** |
| `+=` compound assignment | **0** (`x = x + 1` ×3,034) |
| Iterators `.map/.filter/.fold/.iter` | **0** |
| `if let` / let-else / or-patterns / tuple destructuring | **0** |

The goal of every release below: **close the gap between grammar coverage and
usable coverage** — measured by the compiler dogfooding its own features.

---

## Release v1.1 — Semantic Correctness

> Fix the places where the language silently does the wrong thing.
> These are correctness bugs, not missing features.

### R1.1 Fix `==` on strings

- **Evidence:** `str_equal(` called ~1,984× in compiler; ~513 `== "literal"` sites
  still live; bug 1.34 — `type_eq` pointer-compared `Type::Generic` names and
  rejected `Vec<Param> == Vec<Param>`.
- **Goal:** `==` on `str` operands lowers to content comparison in all type
  contexts (not only when both sides are statically `str`).
- **Acceptance:** delete all `str_equal` call sites in `compiler/*.fd`;
  `==` on string fields, enum-extracted strings, and `Vec<str>.get()` all work.

### R1.2 Fix `==` on enums

- **Evidence:** bug 1.11 — enum literals lower to pointers, match-bound values
  to tag ints; `==` compares across representations. ~24 hand-written `*_eq`
  functions + `token_kind_tag` (136-arm manual `mem::discriminant`) as workarounds.
- **Goal:** `==`/`!=` on enum values compares tag + payload correctly; derive
  `==` for enums via `@derive(Eq)` or make it automatic.
- **Acceptance:** delete `binop_eq`, `ir_type_eq`, `type_eq` (enum parts),
  `token_kind_eq`, `region_eq`, `pir_level_eq`, `value_eq`, etc. (~24 fns,
  ~464 call sites). Fix remaining `==` on enum literals (pe.fd:2843,
  parser.fd:4688).

### R1.3 Fix `Option<T>` payload layout

- **Evidence:** ast.fd:58 — "The builtin Option<T> has i64-sized payload slots,
  so it cannot hold non-i64 values." → **126 hand-rolled `Opt*`/`ParseResult*`/
  `Result*`/`Option*` enums**, ~2,523 `Some/None` uses; parser.fd alone has ~30.
- **Goal:** generic enum monomorphization produces correct per-instantiation
  layouts (struct payloads boxed or sized correctly).
- **Acceptance:** replace all `Opt*`/`ParseResult*`/`Option*` decls with
  `Option<T>`/`Result<T,E>`; `Option<Type>`/`Result<StructDef, str>` work.

### R1.4 Make `Vec<T>` hold real types

- **Evidence:** Vec stores i64 per slot — diag.fd:474 "Fides can't store
  structs in Vecs" (DiagList stores rendered strings); pe.fd:1759 bytes pushed
  as i64; Vec-of-struct push copied 8 bytes → `apply_cfg_filter` disabled
  (compiler.fd:2020); Vec\<f64> returned bit patterns (bug 3.19).
- **Goal:** `Vec` is layout-aware — `Vec<u8>`, `Vec<Struct>`, `Vec<f64>` store
  and return real values.
- **Acceptance:** `Vec<Diag>` usable; re-enable `apply_cfg_filter`; delete
  `word_repr` hacks; `Vec<u8>` for encoders (elf64/pe/arm64/riscv/wasm).

### R1.5 Fix `>>` in nested generics and expressions

- **Evidence:** `Vec<Vec<T> >` space-workaround ×16 sites + 6 named wrapper
  structs (`StmtVec`, `EffectHandlerVec`, `I64PairList`, …); `opt.fd:7086`
  `shr_i64` implements `>>` as a **division-by-2 loop**.
- **Goal:** `>>` tokenizes correctly in type position and expression position.
- **Acceptance:** write `Vec<Vec<i64>>`, `a >> 3` everywhere; delete wrappers.

### R1.6 Cross-module type visibility

- **Evidence:** enums from imports invisible to parser → per-file `Opt*`
  re-declaration + `Severity_error()` constructor fns (diag.fd:63); struct
  literals only for same-file structs → ~299 `*_new` constructors; enum ctors
  parse as `MethodCall` cross-module (bug 1.21, patched in 3 places).
- **Goal:** `use` makes enum variants + struct literal syntax work across
  modules.
- **Acceptance:** delete `*_new` constructor shims and `Severity_*()` helpers;
  `OptStr::Some(x)` works in any module that imports it.

### R1.7 Char literals and real `str`

- **Evidence:** no char literals — `c == 47  // '/'` at ~27 annotated sites,
  ~269 `str_byte_at` calls; `str` is `char*` — byte-based, 206 `unsafe {}`
  pointer-math blocks in stdlib.
- **Goal:** `'/'` char literal; `str` gets a safe API surface (or a `u8`/
  `byte` type) so lexers don't need magic numbers.
- **Acceptance:** lexer/parser code uses `c == '/'`; no `unsafe` needed for
  ordinary string work.

---

## Release v1.2 — Ergonomics (make the safe subset unnecessary)

### R2.1 Wire the `?` operator

- **Evidence:** `?`/`try`/`?.`/`??` parsed but 0 uses; every fallible call is a
  4-line `match { Ok => …, Err(e) => return Err(e) }` — parser.fd:872 `parse_struct`
  is 70 LOC vs ~30 with `?`.
- **Goal:** `expr?` works on `Result<T,E>`/`Option<T>` in all backends.
- **Acceptance:** parser rewrite uses `?`; ~30% LOC reduction in parse fns.

### R2.2 `for` loops + iteration

- **Evidence:** 0 `for` in compiler (works in stdlib ×55, `for i in 0..n`);
  2,944 manual `while i < v.len() { v.get(i); i = i + 1; }` loops.
- **Goal:** `for i in 0..n`, `for x in vec`, `for (k,v) in map` work in the
  self-hosted dialect on all backends.
- **Acceptance:** convert ≥50% of index-`while` loops to `for`; no regressions.

### R2.3 Iterators / destructuring / pattern power

- **Evidence:** no `.map/.filter/.fold`, no `let (a,b)`, no `if let`, no
  or-patterns ("Fides doesn't support |" — x86.fd:22), no struct spread.
- **Goal:** `if let Some(x) = e`, `A | B` patterns, tuple destructure,
  field init shorthand/`..rest` spread.
- **Acceptance:** `value_eq` (interp.fd:56, ~52 LOC) and `token_kind_tag`
  collapse to a few lines each.

### R2.4 Auto-deref, inference, struct init ergonomics

- **Evidence:** `&mut T` doesn't coerce → duplicated `*_ref` fns
  (typechecker.fd:373 `str_vec_contains_ref`); `self.f = Vec::new()` can't
  infer → ~1,056 explicit annotations (bug 1.28); struct literals need all
  fields → construct-then-assign dance (parser.fd:937).
- **Goal:** auto-deref at call sites; field-typed `Vec::new()` inference;
  `Struct { field: x, ..defaults }` partial init.
- **Acceptance:** delete `*_ref` duplicates and inference workaround vars.

### R2.5 String building & formatting

- **Evidence:** `+` concat is O(n²) (bug 1.12) → `p_sb_*` builder builtin;
  `+`/`int_to_str` everywhere; f-strings lexed but stubbed (parser.fd:5148).
- **Goal:** f-string `{expr}` interpolation works; `str` concat is amortized.
- **Acceptance:** codegen `wln`/`int_to_str` chains replaced by f-strings.

### R2.6 `HashMap<K,V>` as a real collection

- **Evidence:** HashMap is an opaque i64 handle storing i64 values —
  `HashMap_new() -> i64`, `HashMap_get(h,k) -> i64`; compiler code uses
  `Vec<SymIndex>` + linear scans instead (elf64.fd:183).
- **Goal:** typed `HashMap<str, T>` with real keys/values.
- **Acceptance:** symbol tables/maps use `HashMap<str, Sym>`; no i64 handles.

---

## Release v1.3 — Compiler Robustness

### R3.1 Fix the statement-dropping bug (for real)

- **Evidence:** bug 1.19 → ~35 extracted helpers: `keyword_or_ident_a.._ml` (8),
  ~23 `PeCodeGen_gen_inst_*` one-arm functions, `subst_expr_part2`,
  `Lowerer_lower_stmt_*` splits; `CHUNK_MARKER_5..11` residue in typechecker.fd.
  Partial root cause: 4KB/1MB stack vs 4,672 C locals (~37KB/frame).
- **Goal:** function size is not a correctness boundary — fix codegen to emit
  correct output regardless of function length, or split generated C
  automatically.
- **Acceptance:** merge the split helpers back; delete CHUNK_MARKERs; a single
  10K-line function compiles correctly.

### R3.2 Kill the latent landmines

- **Evidence (each independently fixable):**
  - `apply_cfg_filter` no-op'd to dodge Vec-of-struct bug (compiler.fd:2020)
  - Nested enum params corrupt → pir.fd:1835 does O(n) lookup-by-id instead
  - `p_file_write` truncates at NUL (bug 1.20/3.14)
  - `I128/U128` silently narrow to 64-bit (2c.10)
  - PE stubs return `xor rax; ret` wrong values (2c.15, 20+ stubs)
  - `pir.fd` str_substring returned whole string; `f64_to_bits` stub returned 0
  - Deliberate bug-for-bug parity: pe.fd:6498 "buggy first attempt (parity)"
- **Goal:** zero wrong-value stubs, zero no-op'd passes, zero known corruption
  paths.
- **Acceptance:** each item above has a regression test in tests/selfhost/.

### R3.3 Type system honesty

- **Evidence:** 13 `verify_*` functions in typechecker.fd are empty no-ops —
  `@constant_time`, `@gas`, `@dp`, `@conserve` **accepted but not enforced**;
  borrow checker hardcoded `Off` (compiler.fd); 5 unsafe blocks in compiler,
  206 in stdlib doing byte math; `str` coerces to i64 address (interp.fd:2099).
- **Goal:** annotations that claim enforcement actually enforce; borrowck
  wired through the pipeline (`--borrowck=on` default, flag plumbed via
  module `var` per known-bugs 2.12); `unsafe` audited and shrunk.
- **Acceptance:** negative tests — code violating each annotation fails to
  compile; `--borrowck` flag actually changes behavior.

### R3.4 Source hygiene & globals

- **Evidence:** ASCII-only policy (bug 1.36, fixpoint divergence) yet ~5,997
  non-ASCII chars regressed into 20 files; `compiler.fd.rej` committed;
  module-level mutable `var` officially unsupported (main.fd:35) but used in
  3 runtime files; globals broken on ARM64/RISC-V/WASM (2b.3); no global
  init order (2b.4); `val` keyword surprise (bug 1.26).
- **Goal:** ASCII lint in CI; support or forbid module `var` decisively;
  globals work on all backends or error loudly.
- **Acceptance:** `verify.sh` fails on non-ASCII; globals spec'd and tested.

### R3.5 Backend parity

- **Evidence:** C codegen 220/237 (17 fail: TLS, KV cache, threading segfault,
  ECS, `box_expr_unbox`); `emit` doesn't use regalloc; ARM64/RISC-V produce
  objects not executables; Mach-O executables unverified; WASM stubs
  `unreachable`.
- **Goal:** C codegen ≥230/237; one regalloc path for all emitters;
  ARM64/RISC-V `build` produces executables.
- **Acceptance:** test baseline updated; no backend has wrong-value stubs.

---

## Release v2.0 — Feature Hardening (dogfood the flagship features)

> The differentiators exist in the grammar but are unused by the compiler.
> v2.0 = the compiler itself uses them, proving they're production-real.

### R4.1 Compiler uses its own type system power

- Effects: annotate the compiler — `!IO` on file ops, `!{}` pure passes —
  and let the effect checker verify it. (0 annotations today.)
- Regions/budgets: parse/typecheck under `region` scopes instead of
  arena-everything.
- Borrow checking: self-host builds with `--borrowck=on` and stays green.

### R4.2 Compiler uses its own ergonomics

- Replace `ParseResult*` match-ladders with `?`.
- Replace manual `Opt*` with real `Option<T>`/`Result<T,E>`.
- Replace `while i < n` index loops with `for`/iterators.
- Use generics (`fn subst<T>`), traits (a real `Eq`/`Display` trait instead
  of `*_eq`/`*_to_str` free fns), and closures where they fit.
- Use `comptime` for constant tables (opcode tables, keyword maps).

### R4.3 Stdlib hardening

- **Evidence:** ~12 "Fides doesn't have X yet" comments (no deque — flowfield:107,
  no mutable Vec indexing — sprite:314, no u32 arrays — crypto:25, no globals —
  test:18, limited closures — http:270); 206 unsafe blocks in core string ops.
- **Goal:** stdlib needs no workaround comments; unsafe isolated behind safe
  APIs; missing containers (deque, byte buffer, typed map) added.

### R4.4 Performance reality check

- **Evidence:** O(n²) traps are easy: string concat (bug 1.12), linear-scan
  imports (bug 1.23 — 238s → negligible after HashMap), `Vec<SymIndex>` scans
  substituting for a real HashMap.
- **Goal:** benchmark suite vs Rust/C on compiler workloads; optimizer
  verified on the compiler's own IR; escape analysis; `?`-based code compiles
  to the same IR as manual match (zero-cost).

---

## Anti-Goals (explicitly not in these releases)

- New feature surface — the grammar already covers ~134 token kinds and 40+
  annotations; the releases above make the existing surface *true*.
- New backends — parity for existing ones first.
- Ecosystem/registry — comes after the language is self-consistent.

## Success Metric

**One number:** lines of `compiler/*.fd` required to express the same compiler.
Today: 100,934 LOC in the narrow dialect. Each release should shrink it —
target ~70–75K LOC by v2.0 — while *increasing* the fraction of the grammar
the compiler actually uses. Fixpoint + 38/38 selfhost tests must stay green
at every release.
