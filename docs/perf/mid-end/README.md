# Mid-end measurements (ME30)

One file per change, written by `scripts/me-measure.sh LABEL FID [FILE...]`
(docs/roadmap/llvm-roadmap.md ME30): each program built natively twice by
the same compiler, the pass on and off, with the IR instructions after
iropt, iropt's time, the whole build's time, and whether both programs print
the same.

`optin-<pass>.txt` are the opt-in passes (the names compiler/*.fd tests with
`ime_on`, i.e. `FID_IROPT_ON`), all made by `scripts/me-measure-all.sh`.

## 2026-11-16 run

Compiler: a one-stage `fid` built Nov 15 23:01 (`fidh1.exe`), Windows x64,
`fid emit` (pe_ir). Programs: `tests/bootstrap/test_counted_loop.fd` and
`tests/bootstrap/test_irswitch_match.fd` only, **not the suite** (a suite
run is ~2 x 500 builds of 10-20 s per pass; with 20 passes, days on this
machine):

```
FID=... ./scripts/me-measure-all.sh "$FID" tests/bootstrap/test_counted_loop.fd tests/bootstrap/test_irswitch_match.fd
```

Totals over the two programs (instructions after iropt, iropt ms, build ms;
on vs off). Every pass printed the same on and off.

| Pass | Instructions on / off | iropt ms on / off | Build ms on / off |
|---|---|---|---|
| branchfold | 5050 / 5050 | 1814 / 1771 | 19706 / 19733 |
| deadf | 5050 / 5050 | 3332 / 1680 | 18352 / 19318 |
| envtrim | 4838 / 5050 | 3266 / 1755 | 21144 / 19384 |
| forward | 5049 / 5050 | 1750 / 1759 | 19436 / 19713 |
| icf | 5050 / 5050 | 1717 / 1780 | 19572 / 19599 |
| inline2 | 12936 / 5050 | 21182 / 1801 | 58150 / 19884 |
| ipcp | 5050 / 5050 | 2026 / 1863 | 20454 / 20426 |
| ivsr | 5050 / 5050 | 1983 / 1991 | 21320 / 21404 |
| licmguard | 5050 / 5050 | 1841 / 1886 | 19882 / 19597 |
| loadhoist | 5029 / 5050 | 1834 / 1830 | 20486 / 20882 |
| loops | 5051 / 5050 | 1912 / 2013 | 21052 / 21157 |
| matchfacts | 5050 / 5050 | 1958 / 1916 | 20639 / 20934 |
| simplify | 5032 / 5050 | 2133 / 1939 | 20879 / 20675 |
| spec | 5050 / 5050 | 7239 / 1737 | 26177 / 20162 |
| sra | 5050 / 5050 | 1913 / 1853 | 20425 / 20325 |
| ssa | 5050 / 5050 | 1830 / 1838 | 20572 / 20697 |
| ssaexit | 5050 / 5050 | 1878 / 1917 | 20922 / 21013 |
| switch | 5073 / 5050 | 1959 / 1885 | 20714 / 20277 |
| unroll | 5081 / 5050 | 2198 / 2148 | 24605 / 24458 |
| version | 5050 / 5050 | 2059 / 1953 | 22575 / 21117 |

What stands out:

- `inline2` (ME20/21) multiplies `test_counted_loop`'s IR by 3.2
  (3526 -> 11163 instructions) and its iropt time by 18 (1.1 s -> 20 s):
  its budget does not hold on this program.
- `spec` costs 5.5 s of iropt for no change in instructions; `deadf` and
  `envtrim` double iropt's time (`envtrim` removes 4% of the instructions).
- Most passes change nothing on these two programs: they need programs that
  have what they look for (the per-pass tests, `tests/bootstrap/test_midend_*`),
  and the suite run that ME30 asks for.

Build times move by up to ±1 s between runs of the same build (other
processes on the machine): only differences above that mean something.

## Suite runs (since Nov 18)

The run ME30 asks for, every pass on the whole suite, is
`scripts/me-measure-all.sh` with no files, which since Nov 18 runs
`scripts/me-measure-suite.py`:

```
JOBS=4 CHECK_SHARE=3 ./scripts/me-measure-all.sh release/fid.exe
# or: python scripts/me-measure-suite.py --fid release/fid.exe --jobs 4 --check-share 3
```

- It writes the same `optin-<pass>.txt` files as before (one line per
  program: instructions on/off, iropt ms on/off, build ms on/off,
  same/DIFF; a totals line), now for every program of the suite (the
  programs `scripts/me-measure.sh` takes with no files), and the table of
  all passes to `optin-suite.md`. Copy that table here under a dated
  heading when a run is finished.
- The default build (no `FID_IROPT_ON`/`OFF`) is shared: it is the "off"
  side of an opt-in pass and the "on" side of a pass in `IME_DEFAULT_ON`
  (compiler/iropt_me.fd `ime_on`), so a pass costs one build per program,
  not two. On Nov 18 the suite is 1,158 programs and there are 27 pass
  names: 1,158 + 27 x 1,158 = 32,424 builds instead of 62,532. A build of
  a test program took 3.5-4 s with the Nov 16 stage 0 on a quiet machine,
  so about 36 h at `--jobs 1`, 5-9 h at `--jobs 4`-`8`.
  `--check-share K` checks that claim on K programs per pass by also
  building them with `FID_IROPT_ON=<p> FID_IROPT_OFF=<p>` (what the old
  loop built); a difference is listed at the end of `optin-suite.md`.
- Each build's result is cached in `build/me-measure/<compiler hash>/`: a
  stopped run goes on where it stopped; `--report-only` writes the files from
  the cache. A rebuilt compiler has another hash and starts over.
- With `--jobs N` the build times include the other builds running at the
  same time: compare them within one run only. Instruction counts and the
  same-output check do not depend on it.
- `SERIAL=1 ./scripts/me-measure-all.sh ...` runs the old loop (two builds
  per program and pass through `scripts/me-measure.sh`).

No suite run has been recorded yet: it needs this tree's built `fid`
(the 2026-11-16 files above are from the two-program run). The script was
checked on Nov 18 with the Nov 16 stage 0 on `test_counted_loop` and
`test_irswitch_match`, passes `branchfold` (opt-in) and `unroll` (on by
default), `--check-share 1`: 8 builds in 32 s, both files and the table
written, the shared-build check ok, same output on both sides (that stage 0
predates `unroll` being on by default, so neither pass changed anything
there; the files were written to a scratch directory, not here).
