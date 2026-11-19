#!/bin/bash
# bench/history.sh — add a run's tables to the results page, newest first
# (competitive roadmap BM10). Nothing is dropped or edited: losses stay.
#
#   ./bench/run.sh > /tmp/run.md
#   ./bench/history.sh /tmp/run.md "Windows 11 x64, laptop, quiet"
#   ./bench/history.sh build/bench/run-linux.md "Linux x64, GitHub runner (verify-weekly)"
#
# Each call inserts one section into docs/benchmarks.md (the results page),
# right after its `## Results` heading, so the newest run comes first:
#   ### <date> — <machine>
#   <commit>, the file's tables as they are
# The tables come from bench/run.sh, bench/size.sh or
# bench/compile-speed.sh unchanged, with their footers (toolchain versions,
# load average, the spread of the runs).
set -u
cd "$(dirname "$0")/.." || exit 1
[ $# -ge 2 ] || { echo "usage: bench/history.sh <tables.md> <machine description>"; exit 2; }
SRC="$1"; MACHINE="$2"
# PAGE: another copy of the page (scripts/bench-history-check.sh uses a
# temporary one; the weekly job the checked-out docs/benchmarks.md)
PAGE="${PAGE:-docs/benchmarks.md}"
[ -f "$SRC" ] || { echo "no file $SRC"; exit 2; }
[ -f "$PAGE" ] || { echo "no $PAGE"; exit 2; }
grep -q '^## Results$' "$PAGE" || { echo "$PAGE has no '## Results' heading"; exit 2; }
COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
DIRTY=""
git diff --quiet HEAD 2>/dev/null || DIRTY=" (with uncommitted changes)"
SECTION=$(mktemp)
{
  echo
  echo "### $(date +%F) — $MACHINE"
  echo
  echo "Commit \`$COMMIT\`$DIRTY."
  echo
  tr -d '\r' < "$SRC"
} > "$SECTION"
TMP=$(mktemp)
awk -v f="$SECTION" '
  { print }
  $0 == "## Results" && !done { while ((getline l < f) > 0) print l; done = 1 }
' "$PAGE" > "$TMP" && cat "$TMP" > "$PAGE"
rm -f "$TMP" "$SECTION"
echo "added to $PAGE"
