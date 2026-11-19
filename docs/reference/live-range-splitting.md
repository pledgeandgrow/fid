# Live-range splitting in the machine layer (RA4)

The machine layer (`compiler/nat_ir.fd`: Linux x86-64, AArch64, RISC-V,
macOS, `--nat` Windows) gives each value one register or one frame slot for
its whole live range (`ng_mir_alloc`: linear scan over hulls, coalescing,
spill weights, rematerialization, second chance over holes, argument
register hints). A value that loses gets a slot everywhere: every read is a
load, every write a store, also inside a hot loop that has registers to
spare. Live-range splitting lets such a value have a register where it is
used most and the slot elsewhere.

## What is split

`ng_mir_alloc_split` allocates the function once. The values that spilled
(integer, f64 or f32 class, not rematerialized, not the hidden result
pointer) are the candidates of `ng_split_plan`:

- **Around loops.** For each candidate, the largest natural loops (one per
  head that some later block jumps back to: the blocks that reach such a
  jump without passing the head, so every way in from outside enters at
  the head; the entry block may not be in it) with no call, prologue or
  epilogue that read or write it, at most 128 loops a function. Inside such a loop the value is a new vreg, a
  piece. The piece is loaded on each edge that enters the head from
  outside (the preheader) when the value is live there, and, when the loop
  writes the value, stored on each edge that leaves the loop to a block
  where the value is live (the exits).
- **At calls.** Elsewhere (outside the value's own loop pieces), block by
  block, each run of two or more
  instructions that read or write the value with no call, prologue or
  epilogue between them is a piece: loaded before the first instruction
  when it reads the value, stored after the last write when the value is
  live after the run.

A piece is joined to its value by ordinary moves (`MO_MOV`). The second
allocation runs on the split function with these moves kept out of the
coalescer (`ng_split_pair`), so each piece takes a register or slot of its
own: a piece in a call-free loop can take a caller-saved register (AArch64
x8-x11, RISC-V t4), and its spill weight is its own loop's.

### Floats (MI14)

No float register is callee-saved on any target (the float pool, MI25:
x86-64 xmm2..xmm11, `--nat` Windows included since MI14; AArch64
v16..v27; RISC-V ft2..ft11), so a float value that a call falls inside
spills for its whole range (`ng_float_assign`). Since MI14 such values
are candidates too: the pieces between two calls and in a call-free loop
take float registers, and the value itself keeps the slot across the
calls. Each piece's class comes from its own operations
(`ng_mir_classes_c` counts the float operands of a copy class, moves not
counted), so a piece of a float value is a float, and the value, left
with only the moves, may become an integer; the moves between them cross
classes where they must (`ng_fmov_any`: `movq`/`movd`, `fmov`, `fmv`; an
f32 as its 32 bits zero-extended). `FID_NAT_OFF=splitfloat` keeps the
candidates to integers.

## Where the moves go

The moves of one place form one parallel move (`nat_enc.fd`
`nat_par_moves`), in four phases: a piece's store after its last write,
stores at loop exits, loads at loop entries, a piece's load before its
first read. For an edge from block P to block S:

- P ends in an unconditional jump to S: before the jump;
- P falls through into S: after P's last instruction (only that way runs
  it, since S's label comes after);
- P ends in a conditional branch to S: an edge block. The branch goes to a
  new label; the block (label, moves, jump to S) is put where nothing falls
  into it: right before S when the instruction before S never goes on (the
  block then falls into S, no jump), else after the nearest earlier such
  instruction, else after a later one (the function's end at the latest).

## Kept only when cheaper

`ng_split_cost` estimates both allocations: each slot access of a spilled
value counts 2, each move between two registers and each jump 1, each
weighted 8^(loop depth) as the spill weights are, plus 2 per callee-saved
register saved. The split is kept when it costs less; otherwise the first
allocation stays and nothing changes.

A value none of whose pieces took a register in the second allocation
gained only moves (the pieces of one loop crowd each other out of a small
caller-saved pool). The function is then split once more without such
values and allocated a third time; the cheaper of the two splits is the
one compared with the first allocation (`FID_NAT_OFF=splitprune`: the
first split only; never with `splitall`). `fid emit --stats` prints
`[split] <fn>: N pieces (L around loops), E edge blocks, [V values left
whole, ]cost A -> B, kept` (or `not kept`). The allocations that may be thrown away print nothing; the
one kept is run again when `--stats` or `FID_NAT_DUMP` asked for output.

## Checking

With `FID_VERIFY=1` the split function goes through the MIR verifier
(`ng_mir_verify`), the allocation checker (`ng_mir_check`, which sees the
pieces as ordinary vregs and their moves as ordinary moves) and the split
checker `ng_split_check`, run from `ng_mir_check` while a split function is
allocated. The split checker proves that the split function is the original
one but for renamed operands (each a piece of the operand's own value),
conditional branches sent to edge blocks that go on to the old target, and
inserted moves between a value and its pieces; and, by a forward
must-analysis over the split function's blocks, that every read names a
vreg holding the value's current contents on every way there (a write
makes its vreg the only holder, a move copies its source's state). On
entry to the function only the values themselves hold their contents (a
parameter, or nothing yet); a piece holds nothing until a move or a write
gives it the value, so a load missing before a loop or a run is caught as
well as a store missing at an exit.

## Switches

- `FID_NAT_OFF=split`: no splitting (the allocation of before).
- `FID_NAT_OFF=splitprune`: no second split without the values whose
  pieces all spilled.
- `FID_NAT_OFF=splitfloat`: integers only (MI14: by default float values
  that spilled are split too).
- `FID_NAT_ON=splitall`: every value is a candidate and the split is
  always kept: a stress test of the planner and the checker.
- `FID_NAT_OFF=splitfloat`: integer candidates only (MI14).
- `FID_NAT_MUTATE=split`: the stores at loop exits are left out; a
  `FID_VERIFY=1` build must then fail with the split checker's message.

## Tests

- `tests/nattools/split_unit.fd` (`fid ccompile`, then run the program:
  under `fid run` the interpreter runs all of `compiler::nat_ir` and the
  random programs take hours): the planner and the checker
  on hand-made machine IR, each function run before and after the split by
  a small interpreter; then 40 random programs (calls, nested loops, loops
  left from the middle, if and if/else) with every vreg a candidate: each
  split must pass the checker and compute the same, and each mutated split
  (no stores at loop exits) that computes something else must be rejected.
  `noload` / `nostore` leave the load before a loop or the store at its
  exit out of a split (`nat_split_unit_drop`): both must be rejected; then
  twelve more random programs, each split with one inserted move left out
  at a time: no such split that computes something else may be accepted.
  Before the compiler was rebuilt (Nov 17) this file was built with the
  previous compiler (`ccompile`, the tree's compiler/*.fd against the
  stdlib of the last commit) and printed its expected lines: 40 random
  programs split into 1252 pieces with 20 edge blocks, 23 mutants
  rejected, 72 of 72 dropped moves rejected. The planner, the checker and
  nat_par_moves have not yet run inside a native build.
- `tests/nattools/split_alloc_unit.fd` (`fid ccompile`, then run): the
  whole allocation (`nat_split_alloc_unit`: `ng_mir_alloc_split` with the
  MIR verifier, the allocation checker and the split checker on) of
  hand-made functions on AArch64, RISC-V and x86-64, three ways (no split,
  the default, `splitall`); each allocated function is run by a small
  interpreter over the registers and slots the allocation gave (a call
  overwrites every register that is not callee-saved) and must compute
  what the function computes over its vregs. Cases: `wide` (twenty values
  across two calls, changed in a call-free loop), `calls`, `midexit`, and
  24 random programs per target. Under `FID_NAT_MUTATE=split` some
  random allocations must be rejected and none may compute something else
  unnoticed.
- `tests/bootstrap/test_machine_split.fd`: sixteen values live across
  calls, a call-free loop over them, runs between calls, a loop left from
  its middle.
- `tests/nattools/machine_split_check.sh [fid]`: that program on every
  target with the switch on, off, `splitall` and the mutation; `FILES=...`
  for the whole suite under `splitall`.
- `tests/bootstrap/test_machine_split_float.fd` (MI14): floats live across
  calls, with runs of float operations between the calls, a call-free loop
  over values live across calls (`around`, which has no integer spill, so
  only the float split touches it) and more floats across a call than the
  pool holds. Every product is by a power of two, so a fused multiply-add
  gives the same bits. `tests/nattools/machine_mi14_check.sh [fid]`: that
  program on every target, `--nat` Windows included, by default, with
  `splitfloat` off, `split` off and `splitall`, all under `FID_VERIFY=1`;
  `around` must be split (and kept) by default and not with `splitfloat`
  off; it also checks MI14's Windows half (float registers by default,
  vectors beside them, the exception handler keeping xmm6..xmm15).

## Numbers

Measured Nov 17 with `tests/nattools/split_alloc_unit.fd` (the tree's
`compiler/nat_ir.fd`, built through C by the previous compiler; the cost
is `ng_split_cost`'s estimate, not a timing):

| target | `wide` (spilled values) | `wide` cost, split off -> on | 24 random programs: split kept | random cost, off -> on |
|---|---|---|---|---|
| AArch64 | 11 | 592 -> 466 (4 pieces kept, 7 values left whole) | 22 | 6371 -> 4934 (-23%) |
| RISC-V 64 | 10 | 542 (not kept: t4, its only caller-saved register, holds the loop's counter) | 22 | 6426 -> 5721 (-11%) |
| x86-64 | 17 | 892 (not kept: no caller-saved register in the pool) | 20 | 14225 -> 12937 (-9%) |

Without the second split (`FID_NAT_OFF=splitprune`) the random programs
cost 4956, 5775 and 13196, with 21, 20 and 15 splits kept, and AArch64's
`wide` 522. Every allocation, with the split off, on and under
`splitall`, passed the checkers and computed the same result.

The numbers of real programs (`[split]` and `[stats]` lines of
`fid emit --stats`, the suite's output with and without
`FID_NAT_OFF=split`) come from the validation of the built compiler:
`tests/nattools/machine_split_check.sh` and `scripts/cross-test.sh`.

### A coalescing bug found on the way

The first allocations of some random programs failed the allocation
checker on RISC-V and x86-64 even with the split off ("writes register N
while another live value holds it"), and one x86-64 split computed a
different result. The cause was in `ng_mir_alloc`'s copy coalescing, not
in the split: the first round joined `d <- s` when `s` has one definition,
`d` is not written inside `s`'s range and `d` is not read between `s`'s
definition and the copy in layout order. A branch between them can skip
the copy and reach a read of `d`'s older value, which `s`'s definition
then overwrites in the shared register. The rule now also requires `s`'s
definition and the copy to be in one block (every way on from the
definition reaches the copy); RA6's second round, over exact liveness,
still joins the copies that are safe across blocks.

## Limits

- Pieces are made only for values the first allocation spilled; a value in
  a callee-saved register across a loop that does not use it is not split
  to give the loop that register.
- A loop's pieces take caller-saved registers; where the pool has few or
  none (RISC-V t4, x86-64) they rarely get one. Putting the argument
  registers into the pool (RA7) is what would give them more.
- Loops are found from jumps back in the layout; a loop whose head is laid
  out after its latch, or a rotated loop entered in its middle, gets runs
  only.
- Debug information (DB4) keeps describing a split variable by its
  original vreg: inside a piece's range the location shows the slot, which
  may be stale.
