# The machine IR (MIR)

`compiler/mir.fd` (roadmap MI1). The machine layer, `compiler/nat_ir.fd`,
selects each IR instruction into MIR operations on virtual registers,
allocates registers over them and emits code for AArch64, RISC-V 64 and
x86-64 (Linux, macOS, `--nat` Windows). The MIR itself has no target in it
and lives in its own module, which `nat_ir.fd` imports:

| What | Where in mir.fd |
|------|-----------------|
| an operation: `op`, register fields `r0 r1 r2`, immediates `i0 i1 i2` | `MInst`, `mi_new` |
| the operation codes, with what each takes | `MO_LI` .. `MO_BARG`, `MO_LAST` |
| operand kinds: compares, extensions, panics | `NCC_*`, `NEXT_*`, `NP_*` |
| virtual registers start at | `NG_VBASE` (64; below it a physical register) |
| what an operation reads (`1` r0, `2` r1, `4` r2) and whether it defines r0 | `mi_use_mask`, `mi_defines` |
| branches, calls, block ends | `mi_is_br`, `mi_is_cond_br`, `mi_is_call`, `mi_ends_block` |
| blocks and successors | `MBlocks`, `mi_blocks` |
| block liveness over a chosen set of vregs | `mi_liveness` |
| the printer | `mi_op_name`, `mi_text`, `mir_listing` |
| the structural verifier | `mir_verify` (nat_ir's `ng_mir_verify` adds the target's checks: x86-64's zero operand 60, vector widths and registers) |
| block parameters | `MO_BPARAM`, `MO_BARG`, `mir_verify_bparams`, `mir_bparams_form`, `mir_bparams_ssa_check`, `mir_bparams_lower`, `mir_par_moves` |

Operands: a register field holds a vreg (`NG_VBASE` and up, below the
function's vreg count) or a physical register (0..31; x86-64 also 60, the
zero operand of a compare). Immediates hold constants, labels (a jump's
target, a placed label: `MO_PLACE`), stack slots (frame offsets: `MO_LDSLOT`,
`MO_STSLOT`, `MO_SLOTADDR`) and the operation's kinds. A new operation is
added in mir.fd (its code, `MO_LAST`, `mi_use_mask` / `mi_defines`, its
name in `mi_op_name`, its operand checks in `mir_verify`) and selected and
emitted in nat_ir.fd.

## Block parameters

A block may take parameters, and each edge into it passes arguments:

```
place 100            the block's label
bparam v69 0         r0 = parameter i0 of the block (right after the place,
bparam v70 1         numbered 0, 1, ..)
...
barg v72 100 0       the edge to label i0 passes r0 as parameter i1
barg v71 100 1       (right before the block's jump, or at the end of a
bnez v72 100         block that falls into a labeled block)
```

An edge's arguments are assigned at once (a parallel copy). The arguments
before a conditional branch name their edge by label: those for the
branch's target hold when it is taken, those for the label of the block it
falls into when it is not; when the two are the same label, one set serves
both. The verifier (`mir_verify_bparams`, run by `mir_verify` whenever the
function has either operation) checks that parameters are numbered from 0
right after a label, that a parameter's vreg is written by its `bparam`
only, that arguments come together at the block's end, each for a label
the block goes to and a parameter that label's block has, and that every
edge passes each parameter of its target exactly once. The liveness reads
a `bparam` as a definition at the block's start and a `barg` as a use at
the jump.

`mir_bparams_form(mir, nvreg, keep)` puts a function in this form: every
vreg that more than one operation writes (but `keep`, the struct-result
pointer) becomes one vreg per write, and every labeled block takes a
parameter for each such value live into it, which each edge passes. A
value written once still crosses blocks by dominance, as in Cranelift. An
operation that reads and writes r0 (`csel`) first gets a move of the old
value into the new name; a read of a value no path wrote (a read the IR
verifier judges) reads a fresh vreg set to 0. The result is SSA
(`mir_bparams_ssa_check`). Nothing changes when no vreg is written twice,
when the entry block has a label, or when the function takes the address
of one of its own labels (`MO_ADDR`: an edge the blocks do not show).

`mir_bparams_lower(mir, nvreg, label0)` takes the form out again: each
edge's arguments become moves into its target's parameters (sequenced by
`mir_par_moves`, a cycle through one fresh vreg) before an unconditional
jump, at the end of a block that falls through, and after a conditional
branch for the edge it falls through. The edge a conditional branch takes
goes through an edge block placed after the function's end (`place E`,
the moves, `jmp L`); the branch goes to `E`. The edge labels are `label0`,
`label0 + 1`, ..; the caller creates them (`n_labels`).

## In the machine layer

`FID_NAT_ON=bparams` (off by default) runs the round trip on every
function, right before allocation (nat_ir.fd `ng_bparams_round`; not in
`@constant_time` code). With `FID_VERIFY=1` the form is verified (the
verifier, SSA) and so is the lowered MIR; the allocator, its liveness and
the allocation checker (`ng_mir_check`) then work on ordinary MIR, so the
images must compute what they compute without it. `FID_NAT_DUMP=<function>`
shows the MIR "after block parameters" and "after block parameters taken
out"; `--stats` prints a `[bparams]` line per function. Debug information
of a variable whose vreg the form renamed does not follow it (DB4 reads the
vreg the variable had).

## Checks

- `fid run tests/nattools/bparam_unit.fd`: a loop's form (its listing),
  a cyclic parallel copy, verifier mutants (an argument left out, a wrong
  index, a parameter written elsewhere, a parameter mid-block, parameters
  on a block without a label, a second write) and 300 random functions run
  by a small MIR interpreter before the form, in it and after the lowering.
- `bash tests/nattools/machine_bparams_check.sh [fid]`: the unit test, then
  `tests/bootstrap/test_machine_bparams.fd` built for each machine-layer
  target with `FID_NAT_ON=bparams FID_VERIFY=1` (and without), each image
  run where the host can; `FILES=...` for more programs (the suite).
