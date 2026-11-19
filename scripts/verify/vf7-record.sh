#!/bin/bash
# vf7-record.sh — the recorded run of VF7 (mutation testing of the
# compiler, docs/roadmap/competitive-roadmap.md): every source mutant of
# scripts/verify/variants.py, each a compiler built by build-variant.sh,
# over the VF5 tests (test_opt_*), every test_verify_* test and 20
# generated programs of each fuzzer; the report goes to
# docs/verification/vf7-mutants-<date>.md with the survivors listed.
#
#   FID=release/fid.exe bash scripts/verify/vf7-record.sh
#
# Environment:
#   FID      the compiler the tree was built with (it builds the base and the
#            mutants; default release/fid.exe)
#   JOBS     tests compiled at once (default 4); mutant builds go one at a
#            time (each needs several GB)
#   ONLY     a comma list of mutant names (default every one; `python
#            scripts/verify/variants.py list` names them)
#   TESTS    test globs, comma separated (default the VF5 and test_verify_*
#            tests of tests/bootstrap/)
#   GEN      generated programs per fuzzer (default 20)
#   OUT      work directory (default build/vf7-record); built mutants are
#            kept there and reused by a second run
#   DOC      the report to write (default docs/verification/vf7-mutants-<date>.md)
#
# The tree is a snapshot of compiler/ and stdlib/ taken first, so files
# other sessions change during the hours this takes do not mix into the
# builds. Exit status: that of `variants.py mutate` (0 every mutant killed,
# 1 a mutant survived, 2 a mutant did not build).
set -u
cd "$(dirname "$0")/../.." || exit 2
FID="${FID:-release/fid.exe}"
JOBS="${JOBS:-4}"
GEN="${GEN:-20}"
OUT="${OUT:-build/vf7-record}"
TESTS="${TESTS:-tests/bootstrap/test_opt_*.fd,tests/bootstrap/test_verify_*.fd}"
DAY="$(date +%F)"
DOC="${DOC:-docs/verification/vf7-mutants-$DAY.md}"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
mkdir -p "$OUT"
SNAP="$OUT/tree"
if [ ! -d "$SNAP/compiler" ]; then
  mkdir -p "$SNAP"
  cp -r compiler stdlib "$SNAP/"
fi
ARGS=(mutate --fid "$FID" --tree "$SNAP" --tests "$TESTS" --gen "$GEN" -j "$JOBS" --out "$OUT")
[ -n "${ONLY:-}" ] && ARGS+=(--only "$ONLY")
START=$(date +%s)
"$PY" scripts/verify/variants.py "${ARGS[@]}" 2>&1 | tee "$OUT/mutate.log"
RC=${PIPESTATUS[0]}
END=$(date +%s)
REPORT="$OUT/mutate-report.txt"
NTEST=$(for g in ${TESTS//,/ }; do ls $g 2>/dev/null; done | wc -l)
{
  echo "# VF7 recorded mutation run, $DAY"
  echo
  echo "Made by \`scripts/verify/vf7-record.sh\` (roadmap VF7: a broken pass must"
  echo "fail a test). Each mutant is a compiler built from a snapshot of the tree"
  echo "with one deliberate bug (\`scripts/verify/variants.py\` MUTANTS, built by"
  echo "\`scripts/verify/build-variant.sh\`); each test is compiled by the base and"
  echo "the mutant compiler on the mutant's backends and, when the executables"
  echo "differ, both are run and compared. **killed**: some test's output or exit"
  echo "status changed; **SURVIVED**: no test noticed (a test is owed);"
  echo "**BUILD FAILED**: the mutant compiler did not build."
  echo
  echo "- compiler: \`$FID\` ($("$FID" --version 2>/dev/null | head -1))"
  echo "- tests: $NTEST files ($TESTS) and $GEN programs of each of gen_vs_c.py and gen_programs.py"
  echo "- time: $(( (END - START) / 60 )) minutes, JOBS=$JOBS"
  echo "- exit status: $RC"
  echo
  echo "## Verdicts"
  echo
  echo '```'
  cat "$REPORT" 2>/dev/null || echo "(no report: see $OUT/mutate.log)"
  echo '```'
  echo
  echo "## Survivors"
  echo
  if grep -q SURVIVED "$REPORT" 2>/dev/null; then
    grep SURVIVED "$REPORT" | awk '{print "- `" $1 "`: write a test whose output changes under it"}'
  else
    echo "None."
  fi
} > "$DOC"
echo "wrote $DOC"
exit "$RC"
