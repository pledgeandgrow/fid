# SP14: release mode within 2x of debug mode

Roadmap: docs/roadmap/llvm-roadmap.md SP14 ("the e-graph bounded so the
release mode stays within 2x of debug"). The bound is in compiler/egraph.fd:
`EG_MAX_NODES` (20,000) and `EG_MAX_ROUNDS` (8).

Check: `scripts/egraph-bound-check.sh` builds each program with
`fid emit --target x86_64-linux --time` (release) and with `--debug` (no
mid-end, the fast allocator), best of 3 per mode, and fails when the
whole-build ratio release/debug is over `LIMIT` (2.0). CI runs it in
.github/workflows/bench.yml on head's fid ("Release within 2x of debug
compile time (SP14)"); scripts/ci-wiring-check.sh keeps the step wired.

## Recorded run

2026-11-16, Windows 11 x64, the agents' one-stage `fid` of Nov 15 23:01
(`fidh1.exe`, not a release bootstrap), other builds running on the machine
at the same time (absolute times are high; the ratio is what is checked):

| Program | Release (ms) | Debug (ms) | Ratio | E-graph (ms, share of release) |
|---|---|---|---|---|
| hello | 12146 | 6676 | 1.82x | 1904 (16%) |
| test_hashmap | 12244 | 6656 | 1.84x | 1716 (14%) |
| test_tooling_explain | 11749 | 6990 | 1.68x | 1665 (14%) |

Result: release stays within 2x of debug on all three (1.68x-1.84x). The
e-graph is 14-16% of a release build; without it the ratio would be about
1.5x. SP13's run (docs/perf/sp13-debug-vs-release.md, a quiet machine) found
1.62x on test_tooling_explain, with the e-graph at 2.0 s of 13.8 s.

The margin is small (1.84x on test_hashmap): an e-graph or mid-end change
that adds ~10% to a release build of a small program crosses the limit, which
is what the CI step is for.
