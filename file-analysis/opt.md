# opt.fd — PIR L2 optimizer

> **Re-verified 2026-10-23:** all findings confirmed — Licm+EscapeAnalysis still absent (0 matches), `borrow_non_aliasing`/`linear_ptrs` still have zero consumers, sandbox inline tier still single-set. Default list still 19 vs Rust's 21.

- **Path:** `compiler/opt.fd` — **7,923 LOC** (Rust `src/opt.rs` = 8,573, ~92%)
- **Role:** the L2 optimization pass pipeline — 22 `Pass` variants, fixpoint driver (`optimize_l2` = 8 iters, `optimize_l2_lto` = 16), S-1A relevance gating, then a final `topo_sort_block` on every block.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :1673 | `optimize_l2(module)` / `optimize_l2_lto(module)` on the `build` path (`--opt` default-on; `--lto` selects the LTO variant) |
| `compiler.fd` | :1684-1685 | `peephole_optimize` — backend-side, unrelated to this file's passes |
| `ir.fd` | :1952 | separate `IrModule_optimize` — **intentionally skipped** by `ccompile` to match Rust |

Only importer: `compiler.fd`. No `main.fd` subcommand passes a custom pass list (`--passes` plumbing exists in Rust's `Pass::parse` but there's no CLI surface on the Fides side either — parity).

## Verified faithful (code inspection)

- `Pass_all()` ordering is 1:1 with Rust's `Pass::all()` up to `Memoize`; `LoopRotate` commented out on **both** sides (documented `EffectReorder` interaction), `RegionPool` flag-gated on **both** sides (not in either default list).
- Fixpoint driver: stats-tracked, relevance-gated, stops on a zero-change iteration, final `topo_sort_block` sweep — same shape.
- S-1A `pass_relevant_for_module` gating ported (loops/calls/allocas/stores/branches/dyncalls/deterministic checks).
- F-3F inline cost model: `is_pure` +50%, `is_crashfree` +25%, `is_deterministic` +10% bonuses (:4025-4027) — ported.
- I-3C sandbox-aware **devirtualization** ported: `sandbox_impl_names`/`devirt_sandbox_function` at :3086-3183.
- I-4C gas-guided unrolling 1:1: `max_unroll` 8 → 64 when `f.gas_limit` is Some (:4895-4900).
- S-17: `L2Inst::Copy` handled at 16 traversal sites (Rust: 17 — covered).
- `topo_sort_block` (:942): Kahn's algorithm, terminator pinned last via depend-on-all-others (:987-996) — correct dependency-preserving reorder.
- `pass_dse` (:2574): alloca-escape analysis + GetFieldPtr→base-alloca mapping + reverse-walk overwritten-store elimination — structurally faithful.

## Findings

### MEDIUM — `Licm` and `EscapeAnalysis` passes dropped entirely *(code inspection)*

Rust's `Pass` enum has 24 variants; Fides has 22. The two missing are real, default-on passes:

- `Pass::Licm` (src/opt.rs:156,189) — loop-invariant code motion
- `Pass::EscapeAnalysis` (:158,190) — escape analysis

Absent at every level: no `Pass` variants, no `pass_licm`/`pass_escape_analysis` impls, no `OptStats` fields (`stats.licm`/`.escape_analysis` exist in Rust at :280-281), no name parsing (`"licm"`, `"escape-analysis"|"escape"|"ea"` at :247-248). Nothing else in `compiler/` implements them — confirmed by grep across all modules (only string-escape helpers match).

**Effect:** conservative direction — missed optimizations, not miscompiles. Loop-invariant loads/stores/computations stay inside loops; escaping-alloca promotions never fire.

### MEDIUM — I-3C sandbox-aware inlining dropped *(code inspection)*

Rust's `inline_once` (src/opt.rs:3792+) builds **two** eligibility sets:

- `eligible` — normal limits (3 blocks / 20 insts, LTO: 6/40)
- `sandbox_eligible` — looser limits (6 blocks / 40 insts) for call sites inside `sandbox {}` blocks, justified because sandbox code can only call pure functions

Fides's `inline_once` (:4113) has a single `eligible_names`/`eligible_funcs` set at normal limits (:4117-4122) — `sandbox_eligible`/`inline_sandbox_limits` don't exist (grep: zero hits). Sandbox call sites get the normal threshold.

**Effect:** less inlining inside `sandbox {}` blocks — missed-opt only. Note the *other* half of I-3C (sandbox devirt) **was** ported.

### MEDIUM — alias analysis weakened twice *(code inspection)*

1. **I-7A `borrow_non_aliasing` never consulted.** Rust's `pass_alias_analysis` calls it at four sites (:5166/:5261/:5354/:5381) to short-circuit alias queries via `BorrowKind`. `f.borrow_kinds` IS populated by `pir.fd:1184` — but has **zero consumers in opt.fd**. Compounds run-30's finding (lower.fd never tags `borrow_kind`, so the map is all-`None` anyway) — the borrow-aware path is absent end-to-end: producer dropped in lower, consumer dropped here.
2. **I-6B linear-type tracking stubbed.** `alias_analysis_function` (:5813-5818) hardcodes `is_lin = false` with the comment `// Would check linear_structs`. Rust checks `module.linear_structs` and tracks `linear_ptrs` (16 references) to prove stores through linear pointers can't invalidate other loads. The alloca-root tracing (Alloca + GetElemPtr/GetFieldPtr fixpoint) is ported — only the linear/borrow shortcuts are missing.

**Effect:** strictly weaker alias proofs → DSE/redundant-load eliminations that Rust performs are skipped. Conservative direction.

### Inherited / noted

- `Pass` name parsing surface presumably dropped with the two passes (no `licm`/`escape` strings) — moot since no `--passes` CLI flag exists on the Fides side.
- `IrModule_optimize` (ir.fd:1952) is dead on the `ccompile` path — intentional, matches Rust's main.rs:1434 call-site removal.

## Fix order

1. Port `pass_licm` + `pass_escape_analysis` (biggest real optimization loss — both default-on in Rust).
2. Restore the `borrow_non_aliasing` calls in `pass_alias_analysis` *and* the `borrow_kind` tagging in lower.fd:5671+ — one fix is inert without the other.
3. Add `sandbox_eligible` tier to `inline_once` if sandbox programs matter.
4. Wire `module.linear_structs` into the `is_lin` check (needs the propagation to exist upstream first — verify whether `L2Module` even carries `linear_structs`).

## Verification state

- No live probes run — all findings are structural absences confirmed by grep against both trees (`Licm`/`EscapeAnalysis`/`sandbox_eligible`/`borrow_non_aliasing`/`linear_ptrs` → zero hits in opt.fd where Rust has impls).
- `Pass_all()` vs `Pass::all()` compared line-by-line: Fides = 19 default passes; Rust = 21 (same 19 + Licm + EscapeAnalysis); LoopRotate excluded and RegionPool flag-gated identically.
- Spot-verified: inline cost bonuses, gas-limit unroll, sandbox devirt, topo_sort_block algorithm, DSE alloca tracing.
