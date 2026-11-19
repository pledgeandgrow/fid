#!/bin/bash
# egraph-bound-check.sh — release mode stays within 2x of debug mode in
# compile time (docs/roadmap/llvm-roadmap.md SP14: the e-graph is bounded,
# compiler/egraph.fd EG_MAX_NODES and EG_MAX_ROUNDS, so that it cannot make
# a release build much slower than a debug one).
#
# For each program: `fid emit --target $TARGET --time` (release) and the same
# with `--debug` (no mid-end, the fast allocator), best of $RUNS each. The
# whole-build ratio release/debug must be at most $LIMIT; the e-graph's own
# share of the release build is printed next to it.
#
# Usage: scripts/egraph-bound-check.sh [program.fd ...]
#   FID=<fid>        the compiler (default release/fid.exe, else release/fid)
#   TARGET=<triple>  default x86_64-linux (the machine layer, where the
#                    e-graph runs)
#   RUNS=<n>         default 3
#   LIMIT=<x>        default 2.0
# Exit status: the number of programs over the limit (or that fail to build).
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
TARGET="${TARGET:-x86_64-linux}"
RUNS="${RUNS:-3}"
LIMIT="${LIMIT:-2.0}"
DIR="${OUT:-build/egraph-bound}"
mkdir -p "$DIR"
if [ $# -gt 0 ]; then PROGS=("$@"); else
  PROGS=(tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_tooling_explain.fd)
fi

ms_of() { grep -F "[time] $1:" "$2" | head -1 | grep -o '[0-9]* ms' | awk '{print $1}'; }

# best <mode> <prog> <name>: the lowest total of $RUNS builds; its log is kept
# as $DIR/<name>.<mode>.err. Prints "<total ms> <egraph ms>".
best() {
  local mode="$1" p="$2" name="$3" i t eg bt="" beg=0
  local flags=""
  [ "$mode" = debug ] && flags="--debug"
  for i in $(seq 1 "$RUNS"); do
    # shellcheck disable=SC2086
    "$FID" emit "$p" --target "$TARGET" $flags --time -o "$DIR/$name.$mode.out" \
      > /dev/null 2> "$DIR/$name.$mode.$i.err" || { echo "x 0"; return; }
    t=$(ms_of total "$DIR/$name.$mode.$i.err")
    eg=$(ms_of egraph "$DIR/$name.$mode.$i.err")
    [ -n "$t" ] || { echo "x 0"; return; }
    if [ -z "$bt" ] || [ "$t" -lt "$bt" ]; then
      bt=$t; beg=${eg:-0}
      cp "$DIR/$name.$mode.$i.err" "$DIR/$name.$mode.err"
    fi
  done
  echo "$bt $beg"
}

bad=0
printf '%-28s %10s %10s %7s %12s\n' program "release ms" "debug ms" ratio "e-graph ms"
for p in "${PROGS[@]}"; do
  name=$(basename "$p" .fd)
  read -r tr er < <(best release "$p" "$name")
  read -r td _ < <(best debug "$p" "$name")
  if [ "$tr" = x ] || [ "$td" = x ] || [ "$td" -le 0 ]; then
    echo "$name: FAIL a build failed or printed no [time] total (see $DIR/$name.*.err)"
    bad=$((bad + 1))
    continue
  fi
  ratio=$(awk "BEGIN{printf \"%.2f\", $tr/$td}")
  share=$(awk "BEGIN{printf \"%.0f\", 100*$er/$tr}")
  verdict=ok
  if awk "BEGIN{exit !($tr > $LIMIT * $td)}"; then verdict=OVER; bad=$((bad + 1)); fi
  printf '%-28s %10s %10s %6sx %8s (%s%%)  %s\n' "$name" "$tr" "$td" "$ratio" "$er" "$share" "$verdict"
done
if [ "$bad" = 0 ]; then
  echo "egraph-bound-check: release within ${LIMIT}x of debug on every program"
else
  echo "egraph-bound-check: $bad over the limit or failed"
fi
exit "$bad"
