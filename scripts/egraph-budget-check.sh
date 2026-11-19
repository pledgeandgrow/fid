#!/bin/bash
# egraph-budget-check.sh — the e-graph is bounded so that release mode stays
# within 2x of debug mode (docs/roadmap/llvm-roadmap.md SP14).
#
# compiler/egraph.fd gives each function a budget in proportion to its size
# (EG_GROW_PER_SEED nodes and EG_WORK_PER_SEED units of work per node made
# from its instructions, at most EG_MAX_NODES / EG_MAX_WORK, and at most
# EG_MAX_ROUNDS rounds), and finds a class's constant, the unary nodes and the
# additions through indexes instead of scanning every node, so the pass is
# linear in the size of the program. No time is read: the result is the
# same on every run (RL13). FID_EGRAPH_STATS=1 prints its counters:
#   [egraph] functions F, seed nodes S, nodes N, rounds R, work W,
#            stopped by rounds A, by nodes B, by work C, rewritten E, ...
#
# This script checks, for each program:
#   1. ratio   the release build (`emit`, the mid-end with the e-graph) takes
#              at most LIMIT (default 2.00) times the debug build
#              (`emit --debug`: no iropt, rules or e-graph), whole build, the
#              best of REPS (default 3) runs of each. The e-graph's own row
#              and the ratio with it off (FID_NAT_OFF=egraph,forward) are
#              printed so a failure can be put on the right stage.
#   2. stats   the [egraph] line is printed and identical on every run.
#   3. output  the release and debug executables print the same, and the
#              lines of the test's `// Expected output:` block when it has
#              one (when this host can run the image: `--nat` on Windows,
#              x86_64-linux on a Linux x86-64 host).
# and then, over generated programs of one function of SCALE_N and of
# 4 * SCALE_N lines of rewritable arithmetic:
#   4. linear  the e-graph's work grows at most 6x (linear: 4x, with room for
#              rounds that stop at a different point), not 16x (quadratic);
#              its time row is printed, and gated at 10x when the small one
#              takes at least 100 ms.
#
# Usage:
#   scripts/egraph-budget-check.sh [program.fd ...]
#   FID=<fid>      the compiler (default release/fid.exe, else release/fid)
#   LIMIT=<x>      the largest release/debug ratio (default 2.00)
#   REPS=<n>       timed builds of each mode per program (default 3)
#   EMIT_FLAGS=..  the native path (default: `--nat` on Windows,
#                  `--target x86_64-linux` elsewhere)
#   SCALE_N=<n>    lines of the small generated function (default 60;
#                  0 skips step 4)
# Exit status: 0 when every check passed, 1 otherwise.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
LIMIT="${LIMIT:-2.00}"
REPS="${REPS:-3}"
SCALE_N="${SCALE_N:-60}"
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) HOST=windows; DEF_FLAGS="--nat" ;;
  *) HOST=posix; DEF_FLAGS="--target x86_64-linux" ;;
esac
EMIT_FLAGS="${EMIT_FLAGS:-$DEF_FLAGS}"
can_run=0
if [ "$EMIT_FLAGS" = "--nat" ] && [ "$HOST" = windows ]; then can_run=1; fi
if [ "$EMIT_FLAGS" = "--target x86_64-linux" ] && [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then can_run=1; fi
DIR=build/egraph-budget
mkdir -p "$DIR"
fail=0
ok()  { echo "  ok    $*"; }
bad() { echo "  FAIL  $*"; fail=1; }

PROGS="$*"
if [ -z "$PROGS" ]; then
  PROGS="tests/bootstrap/hello.fd tests/bootstrap/test_egraph_budget.fd tests/bootstrap/test_tooling_explain.fd tests/bootstrap/test_midend_egraph_unary.fd tests/bootstrap/test_midend_egraph_cast.fd"
fi

ms_of() { # ms_of <row> <file>: the milliseconds of `[time] <row>:`
  grep -F "[time] $1:" "$2" | head -1 | grep -o '[0-9]* ms' | awk '{print $1}'
}
stat_of() { # stat_of <field> <file>: a number of the [egraph] line
  grep -F "[egraph] " "$2" | head -1 | sed -n "s/.*, $1 \([0-9][0-9]*\).*/\1/p"
}
expected() { # the "// Expected output:" block of a test
  awk '{ sub(/\r$/, "") } /^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
build() { # build <src> <out> <log> [extra flags...]: 0 when an image was written
  local src="$1" out="$2" log="$3"; shift 3
  rm -f "$out"
  # shellcheck disable=SC2086
  "$FID" emit "$src" $EMIT_FLAGS --time "$@" -o "$out" > /dev/null 2> "$log"
  [ -s "$out" ]
}
min_of() { # min_of <numbers...>
  local m=""
  for v in "$@"; do
    [ -z "$v" ] && continue
    if [ -z "$m" ] || [ "$v" -lt "$m" ]; then m=$v; fi
  done
  echo "${m:-0}"
}

for src in $PROGS; do
  [ -f "$src" ] || { bad "missing $src"; continue; }
  name=$(basename "$src" .fd)
  echo "== $name"
  rel_t=""; dbg_t=""; eg_t=""; off_t=""; stats0=""; stats_same=1
  r=1
  while [ "$r" -le "$REPS" ]; do
    if FID_EGRAPH_STATS=1 build "$src" "$DIR/$name.rel.exe" "$DIR/$name.rel.$r.err"; then
      rel_t="$rel_t $(ms_of total "$DIR/$name.rel.$r.err")"
      eg_t="$eg_t $(ms_of egraph "$DIR/$name.rel.$r.err")"
      s=$(grep -F "[egraph] " "$DIR/$name.rel.$r.err" | head -1)
      if [ "$r" = 1 ]; then stats0="$s"; elif [ "$s" != "$stats0" ]; then stats_same=0; fi
    else
      bad "$name: release build failed (see $DIR/$name.rel.$r.err)"
      break
    fi
    if build "$src" "$DIR/$name.dbg.exe" "$DIR/$name.dbg.$r.err" --debug; then
      dbg_t="$dbg_t $(ms_of total "$DIR/$name.dbg.$r.err")"
    else
      bad "$name: debug build failed (see $DIR/$name.dbg.$r.err)"
      break
    fi
    r=$((r + 1))
  done
  [ "$r" -le "$REPS" ] && continue
  # the release build with the e-graph and forwarding off, once: where the
  # rest of the release time goes
  if FID_NAT_OFF=egraph,forward build "$src" "$DIR/$name.off.exe" "$DIR/$name.off.err"; then
    off_t=$(ms_of total "$DIR/$name.off.err")
  fi
  # shellcheck disable=SC2086
  t_rel=$(min_of $rel_t); td=$(min_of $dbg_t); te=$(min_of $eg_t)
  if [ "$td" -le 0 ]; then
    bad "$name: no [time] total: row in the debug build"
    continue
  fi
  ratio=$(awk "BEGIN{printf \"%.2f\", $t_rel/$td}")
  noeg="?"
  [ -n "$off_t" ] && noeg=$(awk "BEGIN{printf \"%.2f\", $off_t/$td}")
  echo "  time  release ${t_rel} ms (e-graph ${te} ms), debug ${td} ms, release without the e-graph ${off_t:-?} ms"
  if awk "BEGIN{exit !($t_rel <= $LIMIT * $td)}"; then
    ok "ratio release/debug ${ratio}x <= ${LIMIT}x (without the e-graph ${noeg}x)"
  else
    bad "ratio release/debug ${ratio}x > ${LIMIT}x (without the e-graph ${noeg}x; e-graph ${te} ms)"
  fi
  # 2. the counters
  if [ -z "$stats0" ]; then
    bad "$name: no [egraph] line with FID_EGRAPH_STATS=1"
  else
    echo "  stats ${stats0#\[egraph\] }"
    if [ "$stats_same" = 1 ]; then ok "the [egraph] line is the same in all $REPS builds"; else bad "the [egraph] line differs between builds"; fi
  fi
  # 3. what the programs print
  if [ "$can_run" = 1 ]; then
    chmod +x "$DIR/$name.rel.exe" "$DIR/$name.dbg.exe" 2> /dev/null
    "$DIR/$name.rel.exe" > "$DIR/$name.rel.out" 2>&1
    "$DIR/$name.dbg.exe" > "$DIR/$name.dbg.out" 2>&1
    tr -d '\r' < "$DIR/$name.rel.out" > "$DIR/$name.rel.txt"
    tr -d '\r' < "$DIR/$name.dbg.out" > "$DIR/$name.dbg.txt"
    if cmp -s "$DIR/$name.rel.txt" "$DIR/$name.dbg.txt"; then ok "release and debug print the same"; else bad "release and debug print differently ($DIR/$name.rel.txt, $DIR/$name.dbg.txt)"; fi
    if grep -q '^// Expected output:' "$src"; then
      expected "$src" > "$DIR/$name.expected"
      if cmp -s "$DIR/$name.expected" "$DIR/$name.rel.txt"; then ok "the expected output"; else bad "not the expected output (diff $DIR/$name.expected $DIR/$name.rel.txt)"; fi
    fi
  else
    echo "  skip  running the images (this host cannot run '$EMIT_FLAGS' images)"
  fi
done

# 4. linear growth: one long @wrapping function of N and of 4N lines
gen() { # gen <lines> <file>
  {
    echo "@wrapping"
    echo "fn chain(x: i64, y: i64) -> i64 {"
    echo "    var t: i64 = 0;"
    local i=0
    while [ "$i" -lt "$1" ]; do
      echo "    t = t + ((x + $((i % 13 + 1))) + $((i % 7 + 2))) * 2 + (y * 1) + (x - x) + ~(~y) + -(-x) + (y & 0) + (x * 4);"
      i=$((i + 1))
    done
    echo "    return t;"
    echo "}"
    echo ""
    echo "fn main() -> i64 {"
    echo "    print(chain(7, 11));"
    echo '    print("\n");'
    echo "    return 0;"
    echo "}"
  } > "$2"
}
if [ "$SCALE_N" -gt 0 ]; then
  echo "== linear growth (one function of $SCALE_N and of $((4 * SCALE_N)) lines)"
  gen "$SCALE_N" "$DIR/scale_small.fd"
  gen "$((4 * SCALE_N))" "$DIR/scale_large.fd"
  if FID_EGRAPH_STATS=1 build "$DIR/scale_small.fd" "$DIR/scale_small.exe" "$DIR/scale_small.err" \
     && FID_EGRAPH_STATS=1 build "$DIR/scale_large.fd" "$DIR/scale_large.exe" "$DIR/scale_large.err"; then
    ws=$(stat_of work "$DIR/scale_small.err"); wl=$(stat_of work "$DIR/scale_large.err")
    ss=$(stat_of "seed nodes" "$DIR/scale_small.err"); sl=$(stat_of "seed nodes" "$DIR/scale_large.err")
    es=$(ms_of egraph "$DIR/scale_small.err"); el=$(ms_of egraph "$DIR/scale_large.err")
    echo "  small: seed ${ss:-?}, work ${ws:-?}, e-graph ${es:-?} ms"
    echo "  large: seed ${sl:-?}, work ${wl:-?}, e-graph ${el:-?} ms"
    if [ -z "$ws" ] || [ -z "$wl" ] || [ "$ws" -le 0 ]; then
      bad "no work counter in the [egraph] line"
    else
      # the module also holds the runtime's functions: compare the growth
      # of the whole work with 6x the growth of the seed
      wr=$(awk "BEGIN{printf \"%.2f\", $wl/$ws}")
      sr=$(awk "BEGIN{printf \"%.2f\", ${sl:-1}/${ss:-1}}")
      if awk "BEGIN{exit !($wl <= 1.5 * $sr * $ws)}"; then
        ok "work grew ${wr}x for ${sr}x the seed (linear)"
      else
        bad "work grew ${wr}x for ${sr}x the seed (more than 1.5x linear)"
      fi
    fi
    if [ -n "$es" ] && [ -n "$el" ] && [ "$es" -ge 100 ]; then
      if [ "$el" -le $((10 * es)) ]; then ok "e-graph time grew $(awk "BEGIN{printf \"%.2f\", $el/$es}")x"; else bad "e-graph time grew $(awk "BEGIN{printf \"%.2f\", $el/$es}")x (over 10x for 4x the code)"; fi
    fi
    if [ "$can_run" = 1 ]; then
      a=$("$DIR/scale_small.exe" | tr -d '\r'); b=$("$DIR/scale_large.exe" | tr -d '\r')
      # every line adds a positive amount: the longer chain prints more
      if [ -n "$a" ] && [ -n "$b" ] && [ "$a" -gt 0 ] && [ "$b" -gt "$a" ]; then ok "both run ($a, $b)"; else bad "the generated programs printed '$a' and '$b'"; fi
    fi
  else
    bad "a generated program did not build (see $DIR/scale_*.err)"
  fi
fi

if [ "$fail" = 0 ]; then echo "egraph-budget-check: all passed"; else echo "egraph-budget-check: FAILED"; fi
exit "$fail"
