# mir.fd — the machine IR of the machine layer

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap MI1.

- **Path:** `compiler/mir.fd`
- **LOC:** 1,579 (2026-11-21). In git since Nov 19 (49fb2b9); before that
  the same code was inside `nat_ir.fd`.
- **Role:** llvm-roadmap MI1. The MIR with no target in it: the operation
  codes (`MO_*`) and operand kinds (`NCC_*`, `NEXT_*`, `NP_*`), the
  operation `MInst` (op, registers r0..r2, immediates i0..i2; a register
  is a vreg from `NG_VBASE` up or a physical register below), the uses and
  definitions of each operation (`mi_use_mask`, `mi_defines`), blocks
  (`mi_blocks`), block liveness (`mi_liveness`), the printer
  (`mi_op_name`, `mi_text`, `mir_listing`) and the structural verifier
  (`mir_verify`). Also block parameters (`BPARAM`/`BARG`):
  `mir_bparams_form` puts a function in that SSA form,
  `mir_bparams_ssa_check` checks it, `mir_bparams_lower` takes it out again
  before allocation (parallel copies on the edges, `mir_par_moves`).
  docs/reference/mir.md describes it.
- **Imports:** `core`, `vec`, `string`.

## Callers

| File | What |
|------|------|
| `nat_ir.fd` | records every selected operation as an `MInst`; `ng_mir_verify` runs `mir_verify` and adds the target's own checks (`FID_VERIFY=1`); block parameters when `FID_NAT_ON=bparams` |
| `main.fd` | imported only for load order: `image_writer` and `mir` come before the modules that build their structs, because the stage-0 compiler (fidesc) parses a struct literal only of a type it has already loaded (main.fd header, release A4) |

Commands: every machine-layer build (`emit`/`build` for the Linux and
macOS targets, `--nat`, `thumbv7m-*` is separate: nat_thumb.fd), `fid
dump-mir`, `fid disasm <file.fd>`.

## Status (Nov 21 check, MI1: validated)

tests/nattools/machine_bparams_check.sh passes on 4 targets under
`FID_VERIFY=1`; tests/nattools/bparam_unit.fd runs the form and the
lowering on random functions. Block parameters stay opt-in
(`FID_NAT_ON=bparams`, `ng_opt_in`).

## Findings

- No bugs found in this module. Open machine-layer bugs are in
  nat_ir.md (known-bugs 2g.88: the allocation checker rejects
  argument-register hints on x86-64).
