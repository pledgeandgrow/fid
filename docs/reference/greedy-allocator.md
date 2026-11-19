# The greedy register allocator (RA9)

The machine layer (`compiler/nat_ir.fd`: Linux x86-64, AArch64, RISC-V,
macOS and `--nat` Windows) allocates registers with a linear scan by
default: RA2's scan over segments with holes (`ng_seg_scan`). RA9 adds a
second allocator in the style of LLVM's greedy allocator and regalloc2's
backtracking one, behind a switch, to compare the two.

```
FID_NAT_ON=greedy fid emit prog.fd --target x86_64-linux -o prog
```

## What it does

`ng_greedy_assign` replaces only the assignment step of `ng_mir_alloc`.
It sees what the scan sees and fills the same `assign` vector (a register,
-1 spilled, -2 not a range), so everything after the scan runs unchanged:
RA2's second allocation over segments (`ng_alloc_holes`), RA7's argument
hints, RA3's free argument registers, RA10's classes, the coalesced
members, frame slots, RA4's live-range splitting (which calls
`ng_mir_alloc` again on the split function, with the greedy assignment)
and the allocation checker under `FID_VERIFY=1`.

- **Ranges.** The representatives of the copy classes after coalescing,
  as segments with holes (RA2, `ng_class_segs`) or, with
  `FID_NAT_OFF=holes` or `FID_NAT_OFF=segscan`, as hulls `[lo, hi]`.
- **Order.** A binary max-heap: the longest hull first, then the earlier
  start, then the lower vreg (deterministic).
- **Choice.** In order: the copy partner's register (the scan's hint,
  by representative), a free caller-saved register, a free callee-saved
  one. "Free" means no range already holding it shares a position with
  this one, and no call this range is live across clobbers it
  (`ng_reg_ok` / `ng_seg_reg_ok`, with IPRA's call masks).
- **Eviction.** When no register is free, the register whose
  interfering ranges are all lighter than this one (RA5's spill weights;
  the hull's length when `FID_NAT_OFF=weights`), the heaviest of them the
  lightest, is taken; the ranges it had go back into the queue. A range is
  evicted at most `NG_GR_EVICT_MAX` (4) times and only by a heavier one, so
  the queue empties.
- **Spill.** Otherwise the range spills; RA2's allocation over segments
  then gives it a second chance, and RA4 may split it.

The linear scan also evicts by weight (RA5), but only among the ranges
active at the current start, and an evicted range is spilled for good. The
greedy allocator puts the evicted range back, so it can still take another
register later.

What it does not do: it has no splitting of its own (LLVM's greedy splits
a range around its interference before spilling; here RA4's splitter runs
after allocation, on spilled values, for both allocators), and no
per-region costs beyond the spill weights.

Compile time: over segments, a register's ranges overlap as hulls, so the
interference test walks that register's list from its first range (the
scan, which goes in order of start, drops ranges that ended; the greedy
order is by length, so it cannot), and each assignment copies the list
(`ng_gr_insert`). Segment comparisons stop at 8 million per function
(then overlapping first and last positions count as interference: sound,
less precise), but the walk itself is not capped: on functions with
thousands of ranges the greedy allocator is slower than the scan. Not
measured yet; every `fid emit` prints `[prof] native allocation: N ms,
default allocator` (`greedy allocator` with the switch), so building a
large program both ways shows it.

## Switches and output

| Switch | Effect |
|---|---|
| `FID_NAT_ON=greedy` | the greedy allocator in place of the scan (not with the fast allocator of debug builds, RA14) |
| `FID_NAT_ON=greedycmp` with `--stats` | the scan allocates; the greedy assignment is computed on a copy and an `[ra9]` line compares them |

`fid emit --stats` prints, per function:

- with `greedy`: `[greedy] f: N ranges, E evictions, S spilled before holes`;
- with `greedycmp`: `[ra9] f: N ranges; linear scan spills L (weight WL),
  greedy G (weight WG, E evictions)`: the spills of each assignment on the
  same ranges, before RA2's second chance, and the sum of their spill
  weights (each use or definition 8^loop depth: the cost the program pays);
- in both cases the usual `[stats] f: spilled N, reloads R, moves M, frame
  F` of the final allocation.

The function cache (`FID_FNCACHE`) keys on `FID_NAT_ON`, so a cached
linear-scan function is never reused for a greedy build.

## Tests

- `tests/nattools/greedy_unit.fd` (`nat_greedy_unit`): the assignment on
  hand-made hulls, on AArch64, RISC-V and x86-64: a full span (the
  lightest range evicted, then spilled), a long light range evicted by a
  short heavy one, ranges that only touch end to start (no spill), and 120
  random ranges checked for soundness (no two interfering ranges share a
  register, every register from the pool) and determinism.
- `tests/nattools/seg_unit.fd` (`nat_seg_unit`, RA2's test): random
  functions; the scan and the greedy allocator over segments must both
  pass the allocation checker's rules (no definition into a register a
  live value holds, no caller-saved register across a call).
- `tests/bootstrap/test_machine_greedy.fd`: 20 values live across a loop
  with calls, and other pressure shapes; output from `fid run`.
- `tests/nattools/machine_batch12_check.sh [fid]`: the unit test; the
  program on every target with `FID_VERIFY=1`, linear and greedy; the
  `[greedy]` lines; the images' output where they run; a spill table.
  With `FILES="$(ls tests/bootstrap/*.fd)"` it builds the whole suite
  both ways on every target under the checker (a program whose linear
  build fails is not counted) and compares the greedy image's output and
  exit code with the linear one's where it runs.
- `tests/nattools/greedy_compare.sh [fid]`: the suite on the Linux
  targets under the checker with `greedy`, then the `[ra9]` totals per
  target and the programs where the spill weights differ most.
- `tests/nattools/greedy_bench.sh [fid]`: the benchmark programs
  (`bench/fides`, `bench/game/fides`) both ways, best of 5 runs, with the
  final spills and reloads: a markdown table for the section below.

## Results

Not measured yet. On Nov 17 the allocator was checked by its unit tests
only, built with the previous compiler against this tree's
`compiler/nat_ir.fd` (`fid ccompile`): `tests/nattools/greedy_unit.fd`
(hulls) and `tests/nattools/seg_unit.fd` (segments: random functions,
the greedy assignment judged by the allocation checker's rules) printed
all their expected lines on every target. No program has been built with
`FID_NAT_ON=greedy` by a compiler that has it. The tables below are to be
filled from the commands that follow, run with the compiler built from
this tree.

| target | ranges | linear spills | linear weight | greedy spills | greedy weight | evictions |
|---|---:|---:|---:|---:|---:|---:|
| x86_64-linux | | | | | | |
| aarch64-linux | | | | | | |
| riscv64-linux | | | | | | |

(from `bash tests/nattools/greedy_compare.sh $FID`: the suite's totals)

| program | target | linear (s) | greedy (s) | greedy / linear | spilled lin / gr | reloads lin / gr | alloc ms lin / gr |
|---|---|---:|---:|---:|---:|---:|---:|
| | | | | | | | |

(from `bash tests/nattools/greedy_bench.sh $FID`)

What to look for: a lower greedy weight means fewer loads and stores in
loops; x86-64, with five allocatable registers (rbx, r12..r15) and no
caller-saved pool, is where eviction should matter most; AArch64 (14
registers) and RISC-V (12) rarely spill, so the two allocators should
mostly agree there. The greedy allocator becomes the default only if it
passes the checker on the whole suite on every target, never costs more
weight than the scan in total, and is not slower on the benchmarks.
