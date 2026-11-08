# Verification tools (roadmap track VF)

What each tool checks, how to run it, and its last recorded run. Bugs found
are in [miscompile-log.md](miscompile-log.md), each with a repro in
`tests/verify/bugs/`. The roadmap lines are in
[competitive-roadmap.md § Track VF](../roadmap/competitive-roadmap.md#track-vf--verification-in-place-of-years).

All tools take `--fid <compiler>` (default `release/fid.exe`) and use
`CC="zig cc"` for the C path. Python tools need Python 3.10+; `soak.py` and
`interleave.py` need `psutil`. On Windows the Linux and GCC parts run in WSL.

| VF | Tool | Run |
|----|------|-----|
| VF2 | `scripts/fuzz/gen_vs_c.py`: random programs, each with an equivalent C program built by Clang (`zig cc` -O0/-O2) and GCC (WSL, -O0/-O2); all C builds must agree, then `run`, `ccompile`, `emit` must match them. `--avoid` steps around known bugs; `scripts/fuzz/triage.py` groups failures; `scripts/fuzz/reduce.py` shrinks one | `python scripts/fuzz/gen_vs_c.py -n 100 --seed 1` |
| VF3 | `scripts/fuzz/fuzz_frontend.py`: mutants of the suite (token edits, splices, cuts, deep nesting, odd bytes) through `fid check`; a crash, internal panic or hang is a finding; with `--cov-fid` (a compiler built with counters) it keeps inputs that reach new IR blocks | `python scripts/fuzz/fuzz_frontend.py --fid release/fid.exe -n 2000` |
| VF4 | `scripts/verify/variants.py toggle`: each optimizer pass off in turn (iropt passes at run time through `FID_IROPT_OFF`, `PEIR_*` constants in rebuilt compilers); every test's output must not change | `python scripts/verify/variants.py toggle --tree . --sample 60 --gen 20` |
| VF5 | `tests/bootstrap/test_opt_*.fd`: one test per pass (fold, inline, dce, gvn, tailcall, licm, ranges, register allocation), checked against the interpreter (no optimizer) | `python scripts/verify/expect.py tests/bootstrap/test_opt_*.fd` |
| VF6 | design only: [translation-validation.md](translation-validation.md) | — |
| VF7 | `scripts/verify/variants.py mutate`: compilers with one deliberate bug in a pass; some test must notice | `python scripts/verify/variants.py mutate --tree . --tests 'tests/bootstrap/test_opt_*.fd'` |
| VF8 | `scripts/verify/cov_instrument.py` (counters at every function and IR block of the compiler's C) + `scripts/verify/coverage.py` (runs the suite, maps blocks back to `compiler/*.fd`) | see the header of `coverage.py` |
| VF9 | [semantics.md](semantics.md): big-step semantics of the integer core | — |
| VF10 | `tests/conformance/`: one test per sentence of the reference (file name = section), run by `scripts/verify/expect.py` | `python scripts/verify/expect.py tests/conformance` |
| VF11 | [memory-safety-core.md](memory-safety-core.md): calculus, invariant, proof sketch | — |
| VF12 | `scripts/verify/repro.sh`: each program built twice in different directories, temp folders and times; `dump-c`, `emit` and `ccompile` outputs compared byte for byte (`COMPILER=1` also compares two `dump-c compiler/main.fd`) | `FID=release/fid.exe scripts/verify/repro.sh` |
| VF13 | `scripts/verify/clean-bootstrap.sh`: `git archive HEAD` (or `--worktree`) into a new directory, Rust bootstrap from cargo, `scripts/bootstrap.sh` 3 stages, fixpoint | `scripts/verify/clean-bootstrap.sh` |
| VF15 | `scripts/fuzz/fuzz_parsers.py` + `tests/fuzz/parsers.fd`: json, regex, http, csv, toml, xml, url, uuid on mutated input; any panic, crash or hang | `python scripts/fuzz/fuzz_parsers.py -n 5000` |
| VF16 | `scripts/verify/soak.py` + `tests/verify/soak_service.fd`: a request handler run for `--duration` s; memory slope over the second half must be flat | `python scripts/verify/soak.py --duration 86400` |
| VF17 | `scripts/verify/chaos.py` + `tests/verify/chaos.fd`: dropped/reset/half/silent servers, a reader that goes away, missing files, stack exhaustion; with `--linux` full disk (`ulimit -f`) and out of memory (`ulimit -v`) | `python scripts/verify/chaos.py --linux` |
| VF18 | `scripts/verify/interleave.py` + `tests/verify/interleave.fd`: atomics, a mutex, a channel pipeline, parallel branches; thousands of runs on random CPU sets with and without load must print one result | `python scripts/verify/interleave.py -n 2000` |
| VF19 | [miscompile-log.md](miscompile-log.md) | — |

`scripts/verify/build-variant.sh <tree> <out.exe> [FILE:FROM=>TO ...]` builds
a compiler from a copy of a tree with edits; VF4, VF7, VF8 use it.
`scripts/verify/expect.py` checks `//>` (an output line), `//! panic`,
`//! error: words`, `//! exit: N` and `//@ backends:` in a test file.

## Recorded runs (Nov 7 2026, Windows x64, parent's Nov 7 20:50 `fid`)

See the VF lines of the roadmap for the numbers; each line there names the
run it reports.
