# The fast register allocator (RA14)

The machine layer (`compiler/nat_ir.fd`) has two register allocators in
`ng_mir_alloc`:

- the default one: block liveness to a fixed point (from use lists, RA2),
  copy coalescing in two rounds (RA6), spill weights by loop depth (RA5),
  the segment scan with holes (RA2), argument-register hints and fills
  (RA7, RA3), a second chance for spilled ranges, the float class
  (MI25), and then live-range splitting (RA4, `ng_mir_alloc_split`), which
  runs the allocator two or three times for a function that spills;
- the fast one, for debug builds: one pass over the instructions for the
  positions, no liveness fixed point, no coalescing, no spill weights, no
  splitting. A value that lives in more than one block stays in its frame
  slot (each use loads it, each definition stores it); the values local to
  a block share the registers by one linear scan.

The fast one runs with `fid emit --fast-alloc` (or `--fast`: mid-end on),
with `fid emit --debug` (mid-end off, SP13), with `FID_NAT_FAST=1`, or when
a driver sets `g_nat_fast`. The function cache's key holds the mode, so a
cached function of one allocator is never replayed by the other.
`tests/bootstrap/test_machine_fast.fd` and
`tests/bootstrap/test_driver_fast_alloc.fd` check that programs print the
same either way.

## Measuring it

Every `fid emit` prints, after its `[prof] native:` line,

    [prof] native allocation: <ms>ms, fast allocator | default allocator

the time spent in `ng_mir_alloc_split` over every function that was not
replayed from the function cache (`g_nat_alloc_ms`, splitting included).
With `--time`, `[time] total: <ms> ms` is the whole build.

    bash tests/nattools/ra14_fast_alloc_time.sh $FID [--self] [--runs N]

builds several large tests (and, with `--self`, `compiler/main.fd`) for
x86_64-linux with the default allocator, `--fast-alloc` and `--debug
--no-cache` (a cold debug build, not one replayed from the debug build's
function cache), keeps the fastest of N runs of each, and prints the
allocation time, the total, the share and the fast allocator's speed-up.
It fails when the fast modes' allocation is 10% of the build or more.
Run it on a quiet machine: several fid processes at once make a build 10
to 200 times slower, unevenly.

## Numbers

Nov 17, with the compiler of Nov 16 (`stage0`, which has the fast
allocator but not the allocation-only row, so these are allocation and
emission together: an upper bound), on a machine running other builds at
the same time, `test_compiler_sized.fd` for x86_64-linux:

| mode | allocate and emit | total | share |
|------|------------------:|------:|------:|
| default allocator, mid-end on | 2523 ms | 25.1 s | 10.1% |
| `--fast-alloc`, mid-end on | 1832 ms | 24.9 s | 7.4% |
| `--debug` (mid-end off) | 2862 ms | 16.6 s | 17.3% |
| `--debug` with `FID_NAT_FAST=1` | 2470 ms | 15.1 s | 16.3% |

With the mid-end on, the fast allocator's allocation and emission together
are under 10% of the build, so its allocation is too. A debug build has no
mid-end, so the same work is a larger share: whether allocation alone is
under 10% there needs the allocation row, from the script above with the
final compiler. Those numbers, and the self-compile's (`--self`), go here
and into the roadmap's RA14 entry once measured. If the fast modes fail
the 10% bound, the next places to look in the fast path of
`ng_mir_alloc` are the call clobber masks (`ng_call_mask_i`, once per
call), the register classes (`ng_mir_classes_c`) and the per-vreg tables,
which are allocated for every vreg of the function even when most of them
go to frame slots.
