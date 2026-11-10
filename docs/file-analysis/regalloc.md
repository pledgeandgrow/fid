# regalloc.fd — linear-scan register allocator (PIR path)

> **Re-audited 2026-11-09.** Same features as Oct 23. Nov 3 (e655b58) fixed a family of helpers
> that sorted or filled a by-value `Vec` (value semantics dropped the writes). Only the PIR x86-64
> path uses this file; the native backends have their own allocators (pe_ir, nat_ir).

- **Path:** `compiler/regalloc.fd` — **1,354 LOC** (Rust `src/regalloc.rs` = 1,509)
- **Role:** `allocate(func, pure_funcs, no_alloc_funcs)` (:283) and `allocate_module` (:555):
  live intervals over L3, argument preassignment, free pool and eviction, spill slots,
  rematerialized constants, callee-saved tracking.

## Where it is called

| Caller | Site | Command |
|---|---|---|
| `codegen_x64.fd` `generate_asm` | :40 | `fid codegen` (x86-64), `build --native` (default target), `emit --boot` |

Only importer: `codegen_x64.fd`. arm64/riscv codegens do not use it.

## Findings

### Code inspection

#### Features — STILL TRUE

- F-1A remat: `Location::Remat` (:184), applied at :465, :485, :500, :513;
  `compute_constant_vregs` (:573).
- F-1D / I-2D clobbers: `inst_clobbers` (:918), pure and `@no-alloc` calls clobber only RAX.
- `inst_def` (:1045) / `inst_uses` (:1089).

#### CHANGED — by-value Vec helpers

`compute_constant_vregs`, `record_def`, `sort_intervals_by_start`, `sort_pool`,
`sort_active_by_end`, `set_loc_in_vec`, `sort_vec_i64` now take `&mut Vec` (e655b58, Nov 3).
Before that, under value semantics, sorting and spill-slot recording were silently lost. Scan of
the file on Nov 9: no remaining by-value `Vec` parameter that is mutated.

#### LOW — tie-break order differs from Rust — STILL TRUE

`sort_intervals_by_start` (:1274) is a stable bubble sort on `start` only (no vreg-id key).
Deterministic; also O(n²) per function.

#### INFO (inherited) — whole-range clobber model — STILL TRUE

## Fix order

1. (Optional) vreg-id secondary key; a non-quadratic sort if the PIR path stays.

## Verification state

- Re-read cited lines and the e655b58 diff. Live: `codegen` (which allocates) runs on hello.fd.
