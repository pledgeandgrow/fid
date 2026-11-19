# SP13: `fid emit --debug` against release mode

Roadmap: docs/roadmap/llvm-roadmap.md SP13 ("a debug mode with no mid-end
and the fast allocator: 5x faster than release mode").

Recorded 2026-11-16, Windows 11 x64, with a one-stage `fid` built on Nov 15
23:01 (the agents' build `fidh1.exe`, not a release bootstrap). Program:
`tests/bootstrap/test_tooling_explain.fd`, the file `scripts/tooling-check.sh`
uses, built for `--target x86_64-linux` (the machine layer). Three runs of
each, the same numbers within 3%:

| Mode | Total (3 runs, ms) |
|---|---|
| `emit --time` (release) | 13755, 13387, 13663 |
| `emit --debug --time` | 8453, 8263, 8351 |

One run's table, stage by stage (ms):

| Stage | Release | Debug |
|---|---|---|
| stdlib | 774 | 781 |
| typecheck | 752 | 908 |
| lints | 339 | 216 |
| mono | 567 | 646 |
| front end (sum) | 2850 | 2971 |
| lower | 4436 | 4537 |
| iropt | 2866 | - |
| rules | 692 | - |
| egraph | 2037 | - |
| native code | 895 | 995 |
| **total** | **13785** | **8513** |

## Result

- Whole build: release / debug = **1.62x**. The 5x is **not** reached.
- After lowering (iropt + rules + e-graph + native code, the part `--debug`
  changes): 6490 ms against 995 ms, **6.5x**. The debug mode does what it
  should; it cannot reach 5x on the whole build while the front end (2.9 s)
  and lowering (4.4 s) are the same work in both modes and are 88% of the
  debug build.

So SP13 stays open. What is left for the whole-build 5x: lowering
(4.4 s for a small program: `emit` lowers what compiler/reach.fd finds
reachable, but core's and the runtime's functions dominate) and the front
end's fixed cost (the stdlib and type checking of core, D9's stamp
notwithstanding). With the back end as it is (6.49 s release,
1.0 s debug), 5x needs front end plus lowering `x` with
`x + 6.49 = 5 (x + 1.0)`, so `x` at most 0.38 s: about 20x less than the
7.4 s they take now (or cached between builds).

`scripts/tooling-check.sh` now prints both ratios after the two totals
(`ratio release/debug: whole build ...x, after lowering ...x`).
