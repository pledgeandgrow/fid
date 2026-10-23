# regalloc.fd — linear-scan register allocator

> **Re-verified 2026-10-23:** still the cleanest file — no defects found. All AGENTS-flagged features (F-1A remat, F-1D/I-2D clobber sets, S-17 determinism) verified ported.

- **Path:** `compiler/regalloc.fd` — **1,354 LOC** (Rust `src/regalloc.rs` = 1,509, ~90%)
- **Role:** `allocate(func, pure_funcs, no_alloc_funcs)` — linear-scan over L3: live-interval computation, arg preassignment, free-pool/eviction, spill slots, remat, callee-saved tracking. Feeds `ModuleAllocation` into codegen.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `codegen_x64.fd` | :40 | `allocate_module(module)` — the **only** real consumer; x86-64 path |

Only `codegen_x64.fd` imports it (`use compiler::regalloc` at :31). arm64/riscv headers *say* "L3Module → (regalloc)" but never call it — they use their own simpler assignment.

## Verified faithful — all AGENTS-flagged features present

- **F-1A rematerialization**: `Location::Remat(i64)` (:184), `compute_constant_vregs` finds `Mov src=ImmI` defs, remat applied on assign/evict/spill at :462-513 — same sites as Rust (:377-416, :638-651).
- **F-1D + I-2D effect-aware clobbering**: `inst_clobbers` (:918) is **1:1** — pure/`@no-alloc` calls clobber only `RAX`; `CallPtr`/`DynCall`/`Panic`/non-pure `Call` clobber full caller-saved; `Div`/`Mod` → `RAX+RCX+RDX`, other bins → `RCX+RDX`; `Neg`→`RAX+RCX`, `Not`→`RAX`. Every arm's clobber set matches Rust :942+ exactly.
- **S-17 determinism — preserved by construction**: Rust needs `(start, vreg_id)` sort because HashMap order was the bug. Fides has no HashMap — intervals are built first-seen into parallel Vecs (params → blocks → insts, all ordered) and `sort_intervals_by_start` is a *stable* bubble sort on `start` only. Deterministic; ties resolve in first-seen order vs Rust's vreg-id order → different allocation on ties but repeatable. Cosmetic divergence, not a defect.
- `inst_def`/`inst_uses` cover all 22 `L3Inst` arms each — parity with Rust's :1032/:1060.
- Arg preassignment (arg_regs → `Location::Reg`, spill after N regs at `16 + (i-N)*8`), clobber-in-range check before honoring a preassigned reg (:361), eviction remat, callee-saved usage tracking — all present.

## Findings

### LOW — tie-break order differs from Rust *(cosmetic)*

Missing the explicit `(start, vreg_id)` secondary key — bubble sort is stable, so ties keep first-seen order rather than vreg-id order. Deterministic either way; produces *different-but-valid* allocations vs the reference on equal-start ties. If exact-parity output ever matters (golden diffs), add the secondary key — otherwise harmless.

### Inherited — clobber model granularity

`inst_clobbers` is instruction-granular only — a `Call` anywhere in a live range blocks the reg for the whole range (conservative). Same model as Rust; noted for completeness.

## Fix order

1. (Optional) add `vreg_id` secondary key to `sort_intervals_by_start` for byte-parity with Rust's allocation choices.

## Verification state

- No live runs — the audit here was feature-by-feature against the AGENTS.md risk list: Remat sites, clobber sets per arm, sort stability + interval production order (the S-17 mechanism), `inst_def`/`inst_uses` arm coverage (22/22), preassignment + clobber-check path.
