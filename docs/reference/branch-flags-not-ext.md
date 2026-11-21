# Branch conditions in the flags: `!` and EXT (RA19, Nov 17)

This page adds to `docs/reference/shrink-wrapping-and-branch-flags.md`.
RA19 keeps a value used only as a branch condition out of a register: the
compare is made at the branch, as one `BRCC` (`compiler/nat_ir.fd`).

- `ng_fuse_branches` handles a compare and its branch in one straight
  stretch, with up to 32 other instructions between (brgap) and `!` in the
  chain (brnot).
- `ng_fuse_andor` handles `&&` and `||` after select formation (brandor).
- `ng_fuse_xblock` handles a compare in a block that dominates the branch
  (brxblock).

## Added

- **EXT in the chain** (`FID_NAT_OFF=brext`). An extension of the compare's
  0 or 1 gives the same 0 or 1, whatever its width and sign. So an
  `EXT d, s` or an in-place `EXT v, v` is followed like a copy:
  - in `ng_fuse_branches`' walk back from the branch;
  - in `ng_fuse_xblock`.

  The lowering's normalization of a `bool` it keeps in a variable is such
  an in-place EXT.
- **`!` across blocks** (`ng_fuse_xblock`, under `brnot`). The chain from
  the branch back to the compare may now hold `!`, an XOR with an LI 1, in
  place or not. It may sit in the branch's own stretch, as in `if !c`, or
  in the compare's block A, as in `val c: bool = !(a < b)`.
  - Each `!` flips the branch's condition.
  - Its LI goes too when nothing else reads it.
  - `FID_NAT_MUTATE=brnot` flips it back (TG2).
- **In-place definitions.** An in-place EXT or `!` gives its vreg a second
  definition. `ng_fuse_xblock` follows it to the value's other definition
  (`prevdef`), which must sit earlier in the same block. Both definitions
  go with the chain, and the rule that every vreg of the chain is written
  only by the chain still holds.

## Still open

- `while a && b` loop heads that select formation (MI24) does not turn into
  a select.
- `&&` / `||` whose second operand contains a call: no select is formed,
  and the join keeps a bool. This needs jump threading of the bool's
  phi.
- RISC-V without Zicond has no select, so `&&` keeps the SETCC of its first
  compare there.

## Checks

`bash tests/nattools/machine_ra19_more_check.sh $FID` covers
`tests/bootstrap/test_machine_brflags2.fd`:

- fewer SETCC after branch fusion in `xb_not`, `xb_nval` and `xb_nn` on
  every target;
- the outputs, with the rules on and off;
- that the mutation is caught;
- that `test_machine_brflags.fd` still passes.

It also prints the spill effect of `ng_fuse_xblock`, whose longer ranges
of the compared values may spill. It sums `[stats] spilled / reloads`
over the brflags tests and `bench/fides/{fib,loop,sieve}.fd`, with and
without `FID_NAT_OFF=brxblock`.

Nov 21 (roadmap RA19 check, fid built from a self-hosted stage 0): the two
test programs print the right output, and with `FID_IROPT_OFF=inline,inline2`
`not_if`, `and_if`, `or_if` and the `xb_n*` cases lose their SETCCs. In a
default build both check scripts fail, because the functions are inlined;
`ext_if`, `xb_ext` and `xb` keep their SETCCs, and the x86-64 / `--nat`
`FID_VERIFY=1` builds of brflags2 hit the argument-register check
(known-bugs 2g.88).
