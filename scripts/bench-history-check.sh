#!/bin/bash
# scripts/bench-history-check.sh — the benchmark results page is kept current
# without a hand step (competitive roadmap BM10). Checks, on copies (the
# real docs/benchmarks.md is not touched):
#   1. bench/history.sh puts a run right after `## Results`, newest first,
#      with its tables unchanged, and leaves every other line of the page
#   2. the weekly history file grows by one section per run and
#      bench/page-merge.sh puts its sections on the page, newest first, the
#      page's own runs after them, none twice when merged again
#   3. the wiring: verify-weekly.yml's bench job adds each run with
#      history.sh and uploads `bench-weekly-history`; pages.yml merges the
#      newest one and builds the site with tools/site.fd, after the weekly
#      job too
# No compiler needed. Exit 1 on the first failed check.
set -u
cd "$(dirname "$0")/.." || exit 2
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

# 1. history.sh on a copy of the page
cp docs/benchmarks.md "$T/page.md"
printf '| program | fides | c |\n|---|---|---|\n| fib | 0.81 | 0.80 |\n' > "$T/run1.md"
printf '| program | fides | c |\n|---|---|---|\n| fib | 0.79 | 0.80 |\n' > "$T/run2.md"
PAGE="$T/page.md" bash bench/history.sh "$T/run1.md" "machine ONE" > /dev/null || bad "history.sh run 1"
PAGE="$T/page.md" bash bench/history.sh "$T/run2.md" "machine TWO" > /dev/null || bad "history.sh run 2"
r=$(grep -n '^## Results$' "$T/page.md" | cut -d: -f1)
two=$(grep -n -- '— machine TWO$' "$T/page.md" | cut -d: -f1)
one=$(grep -n -- '— machine ONE$' "$T/page.md" | cut -d: -f1)
if [ -n "$two" ] && [ -n "$one" ] && [ "$two" -eq $((r + 2)) ] && [ "$one" -gt "$two" ]; then
    ok "history.sh: newest run first, right after ## Results"
else
    bad "history.sh: order (Results $r, TWO $two, ONE $one)"
fi
grep -qF '| fib | 0.79 | 0.80 |' "$T/page.md" && grep -qF '| fib | 0.81 | 0.80 |' "$T/page.md" \
    && ok "history.sh: tables kept as they are" || bad "history.sh: a table is missing"
# the page minus the two inserted sections is the page as it was
awk -v r="$r" -v one="$one" 'NR > r && NR <= one + 6 { next } { print }' "$T/page.md" \
    | tr -d '\r' > "$T/rest.md"
tr -d '\r' < docs/benchmarks.md > "$T/orig.md"
cmp -s "$T/rest.md" "$T/orig.md" && ok "history.sh: nothing else on the page changed" \
    || bad "history.sh: the rest of the page changed ($(diff "$T/orig.md" "$T/rest.md" | head -3 | tr '\n' ' '))"
PAGE="$T/none.md" bash bench/history.sh "$T/run1.md" "x" > /dev/null 2>&1 \
    && bad "history.sh accepted a missing page" || ok "history.sh: a missing page is an error"

# 2. the weekly history and page-merge.sh
printf '## Results\n' > "$T/hist.md"
PAGE="$T/hist.md" bash bench/history.sh "$T/run1.md" "Linux weekly A" > /dev/null
PAGE="$T/hist.md" bash bench/history.sh "$T/run2.md" "Linux weekly B" > /dev/null
[ "$(grep -c '^### ' "$T/hist.md")" -eq 2 ] && ok "history file: one section per run" \
    || bad "history file: $(grep -c '^### ' "$T/hist.md") sections, want 2"
cp docs/benchmarks.md "$T/site.md"
PAGE="$T/site.md" bash bench/page-merge.sh "$T/hist.md" > "$T/merge.log" || bad "page-merge.sh failed"
grep -q '2 weekly run' "$T/merge.log" && ok "page-merge.sh: two runs added" || bad "page-merge.sh: $(cat "$T/merge.log")"
r=$(grep -n '^## Results$' "$T/site.md" | cut -d: -f1)
b=$(grep -n -- '— Linux weekly B$' "$T/site.md" | cut -d: -f1)
a=$(grep -n -- '— Linux weekly A$' "$T/site.md" | cut -d: -f1)
own=$(tr -d '\r' < docs/benchmarks.md | awk '/^## Results$/ { s = 1; next } s && /^### / { print; exit }')
o=$(tr -d '\r' < "$T/site.md" | grep -n -F -x -- "$own" | head -1 | cut -d: -f1)
if [ -n "$b" ] && [ -n "$a" ] && [ -n "$o" ] && [ "$b" -eq $((r + 2)) ] && [ "$a" -gt "$b" ] && [ "$o" -gt "$a" ]; then
    ok "page-merge.sh: weekly runs newest first, then the page's own runs"
else
    bad "page-merge.sh: order (Results $r, B $b, A $a, own $o)"
fi
before=$(grep -c '^### ' "$T/site.md")
PAGE="$T/site.md" bash bench/page-merge.sh "$T/hist.md" > /dev/null
[ "$(grep -c '^### ' "$T/site.md")" -eq "$before" ] && ok "page-merge.sh: merging again adds nothing" \
    || bad "page-merge.sh: merging again duplicated sections"

# 3. the wiring
W=.github/workflows/verify-weekly.yml; P=.github/workflows/pages.yml
job=$(awk '/^  bench:$/ { s = 1; next } s && /^  [a-z][a-z_-]*:$/ { exit } s { print }' "$W")
echo "$job" | grep -q 'bash bench/history.sh build/bench/run-all.md' && ok "weekly bench job runs history.sh" \
    || bad "weekly bench job does not run bench/history.sh"
echo "$job" | grep -q 'name: bench-weekly-history' && ok "weekly bench job uploads bench-weekly-history" \
    || bad "weekly bench job does not upload bench-weekly-history"
echo "$job" | grep -q 'actions: read' && ok "weekly bench job may read artifacts" || bad "weekly bench job lacks actions: read"
if [ -f "$P" ]; then
    grep -q 'bench/page-merge.sh' "$P" && ok "pages.yml merges the weekly runs" || bad "pages.yml does not run page-merge.sh"
    grep -q 'tools/site.fd' "$P" && grep -q -- '--build _site' "$P" && ok "pages.yml builds the site" || bad "pages.yml does not build tools/site.fd"
    grep -q 'workflows: \["Verify (weekly)"\]' "$P" && ok "pages.yml runs after the weekly job" || bad "pages.yml is not triggered by Verify (weekly)"
    grep -q '^name: Verify (weekly)$' "$W" || bad "verify-weekly.yml is no longer named 'Verify (weekly)' (pages.yml's trigger)"
    grep -q 'actions/deploy-pages' "$P" && ok "pages.yml deploys" || bad "pages.yml does not deploy"
else
    bad "no $P"
fi

[ "$fails" -eq 0 ] && echo "bench-history-check: all passed" || echo "bench-history-check: $fails failed"
[ "$fails" -eq 0 ]
