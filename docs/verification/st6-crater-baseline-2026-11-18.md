# ST6: the first crater baseline (Nov 18)

competitive-roadmap ST6: before each release, build every known Fides
project with the new compiler and fail on a file that built with the last
release and does not now (`scripts/crater.sh`, list `scripts/crater/projects.txt`,
the `crater` job of `.github/workflows/release.yml`).

## The baseline

`scripts/crater/baseline.txt` was recorded with the last release's compiler,
`release/fid.exe` (built Nov 10 15:24 with the 0.8.0 prerelease, commit
`9b9d70c`), and that release's own stdlib:

```bash
mkdir -p <dir> && git archive 9b9d70c stdlib | tar -x -C <dir>
FID_HOME=<dir> CC="zig cc" \
  CRATER_STDLIB_NOTE="stdlib/ of commit 9b9d70c (...)" \
  bash scripts/crater.sh --save release/fid.exe
```

Result: 16 of 17 files built. The one failure, `examples/http2_server.fd`,
imports `http2`, a module the 0.8.0 stdlib does not have; it is a correct
`fail` (a later compiler that builds it is reported as `fixed`).

Why the release's own stdlib: the copies are built outside the repository,
so the compiler takes the stdlib from `FID_HOME`. With the working tree's
stdlib the Nov 10 compiler failed 4 of 5 project builds on builtins it does
not know (`p_region_word` of the regions work in core.fd), and with the
Nov 15 HEAD stdlib on `vec_sz_reserve`: those are a compiler and a stdlib
of different days, not the release. `scripts/crater.sh --save` now writes a
three-line header (date, compiler and its `version`, the stdlib; comment
lines are skipped when comparing) so the baseline says what made it.

## The second run

The same command without `--save`: `crater: 16 built, 1 failed`, `crater:
PASS` (exit 0).

A run with the Nov 16 stage-0 compiler and the working tree's stdlib (a
preview of the release check) gave `fixed examples http2_server.fd` and
`FAIL projects-game game/main.fd` (`Panic: Unknown function 'p_region_word'`
in `rg_vec_fit`): that compiler predates the regions builtin the working
tree's core.fd calls. The compiler built from this tree has it; the check
must be repeated with it (below).

## Validation with the final compiler

```bash
CC="zig cc" bash scripts/crater.sh "$FID"      # from the repository root
```

Pass: `crater: PASS`, exit 0, no `FAIL` line (`fixed examples
http2_server.fd` is expected). A `FAIL projects-game` there is a real
regression of the regions work (the game project panics at run time).

## Left

No public Fides project outside this repository is known, so the list holds
the repository's examples and `tests/projects` (it needs outside users,
AD14). The baseline was recorded on Windows; the release job runs on Linux,
where a file that only builds on Windows would show as a regression.
