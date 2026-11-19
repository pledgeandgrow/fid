#!/bin/bash
# suite-lanes-check.sh — RL2 (llvm-roadmap): every lane scripts/suite-all.sh
# knows (LIST_LANES=1) is run by a workflow on every change, that is by a
# workflow under .github/workflows/ triggered on both push and
# pull_request. It reads the tree only; ci.yml's `gates` job runs it through
# scripts/ci-wiring-check.sh.
#
# Where lanes come from:
#   tier1.yml   each `platform: <p>` of its matrix runs scripts/tier1.sh,
#               whose DEF_LANES for <p> are the lanes
#   any other   a matrix entry `lanes: "<lane> ..."` run by
#   workflow    `bash scripts/suite-all.sh ${{ matrix.lanes }}`, or a literal
#               `bash scripts/suite-all.sh <lane> ...`
#
# Usage: scripts/suite-lanes-check.sh [workflow dir]   (.github/workflows)
#   SELFTEST=1  check the checker: a copy of the workflows without the
#               wasm32 lane, and one whose suite-all.yml runs on push only,
#               must both be reported
set -u
cd "$(dirname "$0")/.." || exit 1
ALL=$(LIST_LANES=1 bash scripts/suite-all.sh)
[ -n "$ALL" ] || { echo "suite-lanes-check: scripts/suite-all.sh lists no lanes"; exit 1; }

# lanes_of_dir <dir> -> the lanes its push+PR workflows run, one per line
lanes_of_dir() {
  local d="$1" f p lanes
  for f in "$d"/*.yml; do
    [ -f "$f" ] || continue
    # triggered on every change: both push and pull_request under `on:`
    awk '
      /^on:/ { on = 1; next }
      on && /^[^ #]/ { on = 0 }
      on && /^  push:/ { p = 1 }
      on && /^  pull_request:/ { r = 1 }
      END { exit !(p && r) }' <(tr -d '\r' < "$f") || continue
    if [ "$(basename "$f")" = tier1.yml ]; then
      for p in $(tr -d '\r' < "$f" | sed -n 's/^ *- platform: *\([a-z0-9-]*\).*/\1/p'); do
        lanes=$(tr -d '\r' < scripts/tier1.sh | sed -n "s/^ *$p) *DEF_LANES=\"\\([^\"]*\\)\".*/\\1/p")
        printf '%s\n' $lanes
      done
      continue
    fi
    if tr -d '\r' < "$f" | grep -qF 'bash scripts/suite-all.sh ${{ matrix.lanes }}'; then
      tr -d '\r' < "$f" | sed -n 's/^ *\(- \)\{0,1\}lanes: *"\([^"]*\)".*/\2/p' | tr ' ' '\n'
    fi
    tr -d '\r' < "$f" | sed -n 's/.*bash scripts\/suite-all\.sh \([^$]*\)$/\1/p' | tr ' ' '\n' | grep -E '^(c|native):'
  done | grep -v '^$' | sort -u
}

# check <dir> -> prints each lane no push+PR workflow runs; status = count
check() {
  local d="$1" have missing=0 lane
  have=$(lanes_of_dir "$d")
  for lane in $ALL; do
    if ! printf '%s\n' "$have" | grep -qxF "$lane"; then
      echo "MISSING RL2: lane $lane is run by no workflow on push and pull_request"
      missing=$((missing + 1))
    fi
  done
  return $missing
}

if [ "${SELFTEST:-}" = 1 ]; then
  T=$(mktemp -d)
  trap 'rm -rf "$T"' EXIT
  bad=0
  # 1. the wasm32 lane removed from suite-all.yml
  mkdir -p "$T/a"
  cp .github/workflows/*.yml "$T/a/"
  tr -d '\r' < .github/workflows/suite-all.yml | grep -v 'lanes: "c:wasm32-wasi"' > "$T/a/suite-all.yml"
  out=$(check "$T/a")
  echo "$out" | grep -q "lane c:wasm32-wasi " || { echo "selftest: a missing wasm32 lane was not reported"; bad=1; }
  # 2. suite-all.yml on push only: its lanes do not count
  mkdir -p "$T/b"
  cp .github/workflows/*.yml "$T/b/"
  tr -d '\r' < .github/workflows/suite-all.yml | awk '
    /^  pull_request:/ { skip = 1; next }
    skip && /^    / { next }
    { skip = 0; print }' > "$T/b/suite-all.yml"
  out=$(check "$T/b")
  echo "$out" | grep -q "lane c:riscv64-linux " || { echo "selftest: a push-only workflow was counted"; bad=1; }
  # 3. the real tree passes
  check .github/workflows > /dev/null || { echo "selftest: the tree itself fails"; bad=1; }
  [ "$bad" = 0 ] && echo "suite-lanes-check selftest: ok"
  exit $bad
fi

D="${1:-.github/workflows}"
check "$D"
n=$?
total=$(echo $ALL | wc -w)
if [ "$n" = 0 ]; then
  echo "suite-lanes-check: all $total lanes run on every change"
else
  echo "suite-lanes-check: $n of $total lanes not run on every change"
fi
exit $n
