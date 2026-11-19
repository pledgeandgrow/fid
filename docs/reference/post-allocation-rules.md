# Post-allocation rules (TG17)

The machine layer (`compiler/nat_ir.fd`) runs a few rewrites after register
allocation, while it emits the allocated MIR (`ng_mir_emit_pass`). Which
rules a target runs is data: `ng_post_rules` returns each target's row of
rule names, and `ng_post_rule(g, name)` tests the row and the rule's switch.

| Target | Rules |
|--------|-------|
| x86-64 (Linux, `--nat` Windows) | ldfwd, ldfwdmov, memop, cmpslot, rmwslot |
| AArch64 | ldfwd, ldfwdmov, wbidx |
| RISC-V 64 | ldfwd, ldfwdmov |

- **ldfwd**: load after store. Suppose an instruction stores a spilled vreg
  to its slot, and the next emitting instruction reads that vreg. The read
  then becomes a register move from the register that was stored, or
  nothing when that is the register it wanted (`ng_ld_fwd`).
  - What is remembered is dropped at a label, a branch, a call, or a
    deferred save or early restore of RA12. It is also dropped when a
    scratch load in the same instruction writes the stored register.
  - `MO_LINE` keeps it.
- **ldfwdmov** (Nov 17): the same forward survives a register-to-register
  `MO_MOV` that does not write the stored register. A move whose registers
  came out equal emits nothing and keeps it too. Such moves are the ones
  that set up a call's arguments, or the copies at a block's end. The move
  must:
  - read no spilled vreg;
  - write no slot;
  - not cross register classes.

  Every target emits such a move as one instruction with no temporaries,
  which is why this is safe.
- **memop**, **cmpslot**, **rmwslot** (x86-64, TG7): a spilled operand is
  used in place from its slot: an ALU operand, a compare, or a
  read-modify-write.
- **wbidx** (AArch64, TG8): a load or store and the `add` of its base become
  one access with writeback.

Two more rewrites hold on every target and are not in the table:

- After allocation, the emitter drops a move whose registers came out
  equal.
- Before allocation, the shared MIR peephole (`ng_peep`, OP19) removes
  branches to the next block, threads jumps and drops redundant moves.

## Switches

`FID_NAT_OFF=<rule>` turns one rule off. `ng_post_rule_switch` spells each
rule's `ng_on("...")` out, so that `scripts/switch-check.sh --list nat`, and
therefore `scripts/pass-matrix.sh`, see every rule.

`FID_NAT_MUTATE=ldfwdmov` keeps the forward even when the move writes the
stored register. It is a wrong rule on purpose, for TG2's mutation checks,
and is never set in a real build.

## Checks

`--stats` prints `[ldfwd] N slot loads after a store became register moves`.

`bash tests/nattools/machine_tg17_check.sh $FID [--matrix]` builds
`tests/bootstrap/test_machine_ldfwd.fd` and `test_machine_ldfwd2.fd` with
`FID_VERIFY=1` on x86_64-linux, aarch64-linux, riscv64-linux and `--nat`,
in three modes: all rules on, `ldfwd` off, and `ldfwdmov` off. It checks:

- the forward counts;
- the outputs;
- that the image changes with `ldfwd` off.

With `--matrix`, it also runs `scripts/pass-matrix.sh` on each target over
the post-allocation switches.
