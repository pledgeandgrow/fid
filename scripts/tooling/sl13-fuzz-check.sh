#!/usr/bin/env bash
# sl13-fuzz-check.sh -- SL13 (docs/roadmap/competitive-roadmap.md):
# `fid test --fuzz` is coverage-guided and keeps its failures, as Go does.
# Input: scripts/tooling/sl13_fuzz_tests.fd (copied into the work
# directory, so its testdata/ is made there and not in the repository).
#
#   1. --fuzz -run fuzz_magic --seeds 20000: the harness is built with
#      `ccompile --fuzz-cov` ("[build] fuzz coverage: N blocks"); the
#      mutator (bit flips, byte sets, inserts, deletes, copies, splices,
#      truncation and growth, the file's literals) keeps each input that
#      reaches a new block, so it gets one byte further into the "FUZ"
#      check at a time and finds it (blind inputs would need ~16 million).
#      The run fails and the input is saved as
#      testdata/fuzz/fuzz_magic/<hash>, starting with "FUZ".
#   2. The same run again, in a fresh directory: the same file name (the
#      mutator is seeded: --fuzz-seed 1 by default).
#   3. `fid test <file>` (no --fuzz): the saved input runs as the test
#      fuzz_magic/<hash> and fails until the bug is fixed; test_plain runs.
#   4. `-run fuzz_magic/<hash>`: that saved input alone.
#   5. The bug fixed (the panic made a return): every test passes, the
#      saved input stays as a regression test.
#   6. --fuzz -run fuzz_magic --seeds 100 after the fix: the saved input
#      seeds the run, so it reaches the blocks behind "FUZ" at once
#      (>= 7 blocks; from the plain seeds alone 100 inputs reach 4).
#   7. --fuzz -run fuzz_shapes --seeds 300: passes, the corpus grows past
#      the first input.
#
# Usage: FID=<fid> bash scripts/tooling/sl13-fuzz-check.sh
#   (needs a C compiler for `fid ccompile`: CC, zig or cl)
# Exit 0 = every check passed.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
[ -x "$FID" ] || { echo "sl13-fuzz-check: no compiler at $FID (set FID)"; exit 1; }
export FID_HOME="${FID_HOME:-$ROOT}"
W="$ROOT/build/sl13-fuzz-check"
rm -rf "$W"   # this script's own output of an earlier run
mkdir -p "$W/a" "$W/b"
SRC="$ROOT/scripts/tooling/sl13_fuzz_tests.fd"
cp "$SRC" "$W/a/sl13_fuzz_tests.fd"
cp "$SRC" "$W/b/sl13_fuzz_tests.fd"
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $2"; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $2"; else ok "$1"; fi
}
ftest() { # ftest <dir> <out> <args...>: fid test in <dir>, status in $rc
  local d="$1" o="$2"; shift 2
  (cd "$d" && "$FID" test "$@") > "$o" 2>&1
  rc=$?
}

echo "== 1. coverage-guided: the magic input is found and saved"
ftest "$W/a" "$W/1.out" sl13_fuzz_tests.fd --fuzz -run fuzz_magic --seeds 20000
[ "$rc" != 0 ] && ok "the fuzz run fails (exit $rc)" || bad "the fuzz run passed: the bug was not found (see $W/1.out)"
has "the harness has block coverage" "$W/1.out" "[build] fuzz coverage: "
has "the target is reported" "$W/1.out" "FAIL  fuzz_magic (within 20000 seeds)"
has "the input is saved" "$W/1.out" "failing input ("
CDIR="$W/a/testdata/fuzz/fuzz_magic"
saved=""
if [ -d "$CDIR" ]; then saved="$(ls "$CDIR" | head -1)"; fi
if [ -n "$saved" ]; then
  ok "testdata/fuzz/fuzz_magic/$saved"
  n_saved=$(ls "$CDIR" | wc -l)
  [ "$n_saved" = 1 ] && ok "one saved input" || bad "$n_saved saved inputs"
  head3="$(head -c 3 "$CDIR/$saved")"
  [ "$head3" = "FUZ" ] && ok "the saved input starts with FUZ" || bad "the saved input starts with '$head3'"
  case "$saved" in
    [0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ok "named by its hash" ;;
    *) bad "the saved input's name '$saved' is not 16 hex digits" ;;
  esac
else
  bad "nothing in $CDIR"
fi
[ -f "$W/a/_fides_fuzz_cur_fuzz_magic.bin" ] && bad "the current-input file was left behind" || ok "the current-input file is removed"

echo "== 2. the same run, the same input"
ftest "$W/b" "$W/2.out" sl13_fuzz_tests.fd --fuzz -run fuzz_magic --seeds 20000
saved_b=""
if [ -d "$W/b/testdata/fuzz/fuzz_magic" ]; then saved_b="$(ls "$W/b/testdata/fuzz/fuzz_magic" | head -1)"; fi
if [ -n "$saved" ] && [ "$saved_b" = "$saved" ]; then ok "deterministic: $saved"; else bad "second run saved '$saved_b', first '$saved'"; fi

if [ -n "$saved" ]; then
  echo "== 3. fid test runs the saved input again"
  ftest "$W/a" "$W/3.out" sl13_fuzz_tests.fd
  [ "$rc" != 0 ] && ok "fid test fails while the bug is there" || bad "fid test passed (see $W/3.out)"
  has "the plain test runs" "$W/3.out" "ok    test_plain"
  has "the saved input is a test" "$W/3.out" "FAIL  fuzz_magic/$saved"
  has "the count" "$W/3.out" "1 passed, 1 failed"
  hasnt "no fuzzing without --fuzz" "$W/3.out" "seeds)"

  echo "== 4. -run <target>/<saved input>"
  ftest "$W/a" "$W/4.out" sl13_fuzz_tests.fd -run "fuzz_magic/$saved"
  has "the saved input alone" "$W/4.out" "0 passed, 1 failed"
  hasnt "test_plain is not run" "$W/4.out" "test_plain"

  echo "== 5. the bug fixed: the saved input passes"
  sed 's/panic("fuzz_magic: the input starts with FUZ");/return;/' "$SRC" > "$W/a/sl13_fuzz_tests.fd"
  grep -q 'return;' "$W/a/sl13_fuzz_tests.fd" || bad "could not fix the copy"
  ftest "$W/a" "$W/5.out" sl13_fuzz_tests.fd
  [ "$rc" = 0 ] && ok "fid test passes" || bad "fid test failed after the fix (see $W/5.out)"
  has "the saved input passes" "$W/5.out" "ok    fuzz_magic/$saved"
  has "the count" "$W/5.out" "2 passed, 0 failed"
  [ -f "$CDIR/$saved" ] && ok "the saved input is kept" || bad "the saved input is gone"

  echo "== 6. the saved input seeds the next fuzz run"
  ftest "$W/a" "$W/6.out" sl13_fuzz_tests.fd --fuzz -run fuzz_magic --seeds 100
  [ "$rc" = 0 ] && ok "the fuzz run passes" || bad "the fuzz run failed (see $W/6.out)"
  has "ok" "$W/6.out" "ok    fuzz_magic (100 seeds)"
  blocks=$(sed -n 's/.* corpus [0-9]*, \([0-9]*\) blocks reached.*/\1/p' "$W/6.out" | head -1)
  if [ -n "$blocks" ] && [ "$blocks" -ge 7 ]; then ok "$blocks blocks reached (the ones behind FUZ too)"; else bad "blocks reached: '${blocks}' (see $W/6.out)"; fi
fi

echo "== 7. a target that passes: the corpus grows"
ftest "$W/b" "$W/7.out" sl13_fuzz_tests.fd --fuzz -run fuzz_shapes --seeds 300
[ "$rc" = 0 ] && ok "passes" || bad "fuzz_shapes failed (see $W/7.out)"
has "300 inputs" "$W/7.out" "300 inputs, corpus "
corpus=$(sed -n 's/.* inputs, corpus \([0-9]*\), .*/\1/p' "$W/7.out" | head -1)
if [ -n "$corpus" ] && [ "$corpus" -gt 1 ]; then ok "corpus $corpus"; else bad "corpus '${corpus}' (see $W/7.out)"; fi
[ -d "$W/b/testdata/fuzz/fuzz_shapes" ] && bad "a passing target saved an input" || ok "nothing saved for a passing target"

if [ "$fail" -eq 0 ]; then echo "sl13-fuzz-check: ok"; else echo "sl13-fuzz-check: FAILED"; fi
exit "$fail"
