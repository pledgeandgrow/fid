# Shrink wrapping and branch conditions in the flags (RA12, RA19)

Two parts of the machine layer's allocator and selection in
`compiler/nat_ir.fd` (Linux AArch64, RISC-V and x86-64, and `--nat`
Windows). Roadmap entries: docs/roadmap/llvm-roadmap.md, RA12 and RA19.

## RA12: callee-saved registers saved where they are needed

`ng_sw_plan` decides, for each callee-saved register a function uses
(index k of its save area), where it is saved and which exits restore it.
The prologue still builds the frame record; only the register saves move.

- **Blocks.** The MIR's blocks (`mi_blocks`) plus an *edge block* on each
  fall-through into a labelled block. The code of an edge block goes right
  after the instruction that falls through, so a save or restore can sit
  on the way into a loop head without running on each turn.
- **Save point S_k.** The nearest common dominator of the blocks whose
  instructions name register k (any operand, after allocation), moved up
  the dominator tree until it qualifies. The entry block means the
  prologue, as before.
- **Without restores** (`ng_sw_ok`): S_k is on no cycle (the save runs at
  most once per call), and every exit S_k reaches is dominated by it. An
  exit is an epilogue, or a call that may become a tail jump. Each exit
  restores the registers whose S_k dominates it.
- **With restores** (`ng_sw_rest`): if S_k reaches an exit past the part
  of the function it dominates, as in `if rare { calls }` in the middle,
  register k is restored where each way leaves that part. The restore
  goes before the jump that ends the block, or on the fall-through edge.
  A forward walk over two states (saved, not saved) then checks that:
  - the state is "not saved" on reaching S_k;
  - it is "saved" on reaching each restore point and each block that
    names k;
  - it is one state or the other at each exit, which restores k or not.
- **Slots and CFI.** The slots do not move. The CFI (`.debug_frame`)
  describes the prologue's saves only, so a debugger shows a deferred
  register's caller value only between its save and restore points.
- **Switches.** `FID_NAT_OFF=shrinkwrap` saves everything in the
  prologue. `FID_NAT_OFF=swrestore` turns off the restore points, so only
  save points that need no early restore remain.
- **Statistics.** `fid emit --stats` prints one line per function that
  defers saves:
  `[shrinkwrap] <fn>: the prologue saves A of B callee-saved registers,
  the others at N later points, restored early at M`.
- **Limits.**
  - The plan applies to functions of up to 4096 blocks, with up to 64
    candidate checks per function.
  - A label that is address-taken or called stops the plan.
  - Without live-range splitting (RA4), a register that a value from the
    entry block shares with a later value is saved in the prologue.

Test: tests/bootstrap/test_machine_shrinkwrap2.fd (rare, inloop, tailpath,
twoguard, midrare). The older early-exit test is
tests/bootstrap/test_machine_shrinkwrap.fd.

## RA19: branch conditions kept in the flags

`ng_fuse_branches` runs on the MIR before allocation, after select
formation. It turns compares that only feed a branch into one
compare-and-branch (`BRCC x, y, cc, L`), so the 0/1 value is never
materialized:

- **Compare and branch in one stretch** (MI23, gap form RA19). `SETCC`,
  then copies, then `BNEZ`/`BEQZ`, with up to 32 unrelated instructions
  between them (`FID_NAT_OFF=brgap`). Before fusion, `ng_dead_mir` removes
  dead reads of the compare's value (`FID_NAT_OFF=deadmir`).
- **`!c`** (`ng_fb_not_of`). An XOR with an `LI 1` inside the copy chain
  flips the branch's condition, and the `LI` goes too
  (`FID_NAT_OFF=brnot`).
- **`a < b && c < d`, `a < b || c < d`** (`ng_fuse_andor`). On targets
  that have a select (x86-64, `--nat` Windows, AArch64, and RISC-V with
  Zicond), select formation first makes the condition one `CSEL` of two
  compares. It becomes two compare-and-branches instead. The `&&` with
  `BNEZ` and `||` with `BEQZ` forms use the label right after the branch
  (`FID_NAT_OFF=brandor`).
- **Compare in another block** (`ng_fuse_xblock`). For example,
  `val c: bool = a < b; if x > 0 { f(); } if c { ... }`. The branch
  compares the operands again; they live to the branch instead of the
  bool. All of the following must hold (`FID_NAT_OFF=brxblock`):
  - the compare's block dominates the branch;
  - the compare's operands are written once, in that block, before it;
  - its value and copies are written and read by the chain only.

`FID_NAT_MUTATE=brnot,brandor,brxblock` deliberately inverts each rule,
for the mutation check.

**Not covered.** A `&&`/`||` whose second operand has a call (no select is
formed, and the condition's join keeps a bool) and RISC-V without Zicond
keep a `SETCC` for the first compare.

Test: tests/bootstrap/test_machine_brflags.fd (random operands against
arithmetic references).

## Validation

`bash tests/nattools/machine_ra12_ra19_check.sh release/fid.exe` builds
both tests for every target with `FID_VERIFY=1`, runs the images where it
can, and checks the following:

- the `[shrinkwrap]` lines;
- the MIR after branch fusion: fewer SETCCs than with the switches off,
  and no CSEL left in `and_if`/`or_if`;
- that the mutations change the output.
