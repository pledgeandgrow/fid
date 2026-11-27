# Compile time, Nov 27 2026: the Nov 25 tree against the Nov 27 tree

Roadmap: docs/roadmap/llvm-roadmap.md SP3, SP6, SP8, SP12, SP13. Measured
in the Nov 27 gate, which was stopped part way (the SP9 checks, the leak
gate and the whole suite on 3 backends had not finished).

**How the two compilers were built.** For a fair comparison both were built
fully with gcc -O2: "before" is the Nov 25 tree, "after" the Nov 27 tree.
These gcc builds are for timing only: a fid built by mingw gcc -O2 crashes in
the interpreter on `spawn` and `parallel` (segfault; the zig builds do not), so
the v0.8.5-unstable binary is a zig build.
Neither is a 3-stage fixpoint build. The v0.8.5-unstable `fid.exe` is a
separate one-stage build from a self-hosted stage 0.

**Machine.** A shared, busy Windows machine: other sessions' builds ran
during the measurements, so read the numbers as noisy and compare the two
columns with each other, not with other days.

## `fid emit hello --no-cache --time`

Three runs each, total / wall clock, in milliseconds.

| Run | Before (Nov 25 tree) | After (Nov 27 tree) |
|---|---|---|
| 1 (first, cold) | 1445 / 1813 | 1441 / 1591 |
| 2 | 1086 / 1405 | 891 / 1057 |
| 3 | 997 / 1322 | 1071 / 1228 |

Stages that moved: mono 33-37 -> 10-17 ms (mono.fd skips the bodies it
cannot change), native code 230-329 -> 170-205 ms. The hello image is
identical with `FID_MONO_FULL=1` (the skip is exact). `build`/`emit` now exit
right after the image is written (`FID_NO_FAST_EXIT=1` turns that off).
SP3's 0.5 s is not met.

## `fid emit --debug hello`

Wall clock 1,250 -> 967 ms (total 898 -> 767 ms). Still not 5x faster than
release mode (SP13): the front end is the same in both modes.

## Self-compile: `fid dump-c compiler/main.fd`

With the Nov 27 compiler: 43.5 s total (44 s wall): typecheck 4.9 s, mono
0.2 s, lowering 11.8 s, C generation 14.9 s. 57 s on Nov 21 (the compiler
built at -O2 in 8 split units). The C is byte-identical with
`FID_MONO_FULL=1` and with `FID_PAR=4` (the import closure parsed ahead in
parallel).

## Lowering scaling (SP12)

`sp12_scaling.sh 400`: ratio 2.97, under the 3.5 limit (it was 3.8-4.6
before lower.fd's `alloca_type` index).

## Edit rebuilds (SP6, SP7, SP8)

- `machine_fncache_check`: after an edit the build reuses 273/273/274
  functions from the cache and compiles 2 (x86_64/aarch64/riscv64; before: 0
  reused, 275 compiled). 3 checks failed in this run ("mk not given up"; the
  check script's line was turned into a note in be36317).
- `relink_check`: 4 failures (the serve requests did not all build).
- `sp8-edit-rebuild-check`: PASS (the edited rebuild gives the one-shot
  image); `serve-fncache-check`: PASS. The 100 ms target is not met: release
  1278/2144 ms, debug 628/672 ms.
- `sp20` function-cache tail checks: all passed.
