#!/bin/bash
# bench/page-merge.sh — put the weekly runs into the results page before the
# website is built (competitive roadmap BM10).
#
#   bash bench/page-merge.sh build/bench/page/weekly-history.md
#
# The weekly job (.github/workflows/verify-weekly.yml, job `bench`) keeps its
# runs in a file of their own, weekly-history.md: a `## Results` line and
# below it one section per run, newest first, each added by bench/history.sh.
# That file travels from week to week as the artifact `bench-weekly-history`,
# so no commit is needed to keep it. This script copies its sections, as they
# are, into docs/benchmarks.md right after the page's own `## Results`
# heading: the weekly runs come first, then the runs written by hand. A
# section already on the page (same `### ` heading line) is not added twice.
# .github/workflows/pages.yml runs it, then builds the site.
# PAGE: another page than docs/benchmarks.md (scripts/bench-history-check.sh).
set -u
cd "$(dirname "$0")/.." || exit 1
[ $# -ge 1 ] || { echo "usage: bench/page-merge.sh <weekly-history.md>"; exit 2; }
HIST="$1"
PAGE="${PAGE:-docs/benchmarks.md}"
[ -f "$HIST" ] || { echo "no file $HIST"; exit 2; }
[ -f "$PAGE" ] || { echo "no $PAGE"; exit 2; }
grep -q '^## Results$' "$PAGE" || { echo "$PAGE has no '## Results' heading"; exit 2; }
grep -q '^## Results$' "$HIST" || { echo "$HIST has no '## Results' heading"; exit 2; }
SECTIONS=$(mktemp)
# the history's sections: everything after its `## Results` line, minus the
# ones whose heading the page already has
tr -d '\r' < "$HIST" | awk -v page="$PAGE" '
  BEGIN { while ((getline l < page) > 0) { sub(/\r$/, "", l); if (l ~ /^### /) have[l] = 1 } }
  started && /^### / { keep = !($0 in have) }
  started && keep { print }
  $0 == "## Results" { started = 1; keep = 1 }
' > "$SECTIONS"
n=$(grep -c '^### ' "$SECTIONS")
TMP=$(mktemp)
awk -v f="$SECTIONS" '
  { print }
  $0 == "## Results" && !done { while ((getline l < f) > 0) print l; done = 1 }
' "$PAGE" > "$TMP" && cat "$TMP" > "$PAGE"
rm -f "$TMP" "$SECTIONS"
echo "page-merge: $n weekly run(s) added to $PAGE"
