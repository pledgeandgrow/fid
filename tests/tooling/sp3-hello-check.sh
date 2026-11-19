#!/bin/bash
# sp3-hello-check.sh — llvm-roadmap SP3: `fid emit` of hello world under
# 0.5 s, then under 0.1 s. The mid-end's share (Nov 17): the module passes
# of compiler/iropt.fd iropt_native (closuredevirt, hofspec, inline,
# gconst's folding, the effect masks and quiet functions), the rules
# (rules.fd rules_run) and the e-graph (egraph.fd egraph_run) work only on
# what main reaches (iro_mid_reach) and, for the facts about calls, its
# callees (iro_call_closure). FID_IROPT_OFF=midreach: every function, as
# before. hofspec also looks only at callers of a function with a CallPtr
# and keeps its name sets in tables.
#
# What it checks, one line each (OK / FAIL / INFO), exit status the number
# of FAILs:
#  1. Same output: tests/bootstrap/test_sp3_midend_reach.fd prints its
#     "Expected output:" lines, and hello prints "Hello, Fides!", 50,
#     "y is big", built with FID_VERIFY=1 by default and with
#     FID_IROPT_OFF=midreach, through pe_ir and `--nat`.
#  2. The set is smaller: FID_EGRAPH_STATS=1 on hello `--nat` prints
#     "[egraph] functions N": N by default < N with FID_IROPT_OFF=midreach.
#  3. TIME=1: `$FID emit hello.fd --time`, RUNS times (default 5) after one
#     warm-up (binary stdlib, function caches and memos warm), the best
#     total; FAIL when it is not under LIMIT ms (default 500, the first
#     target); INFO lines with the rows of the best run (front end, lower,
#     iropt and its passes, native code) and the same with
#     FID_IROPT_OFF=midreach. Time on a quiet machine with a release (-O2)
#     compiler: other fid processes make a stage 10-200x slower.
#  4. SUITE=1: every program of tests/bootstrap through
#     tests/tooling/midend-pass-suite-check.sh with PASS=midreach (off:
#     every function; on: the default), JOBS passed on; NAT=1: again with
#     --nat. Must end "0 differ" (exit 0).
#
# Usage: tests/tooling/sp3-hello-check.sh
#   FID=<compiler> (default release/fid.exe), OUT=<dir> (default
#   build/sp3-hello), TIME=1, RUNS=<n>, LIMIT=<ms>, SUITE=1, NAT=1, JOBS=<n>
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/sp3-hello}"
TIME="${TIME:-0}"
RUNS="${RUNS:-5}"
LIMIT="${LIMIT:-500}"
SUITE="${SUITE:-0}"
NAT="${NAT:-0}"
JOBS="${JOBS:-1}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK    $1"; }
no() { echo "FAIL  $1"; bad=$((bad + 1)); }
info() { echo "INFO  $1"; }

H=tests/bootstrap/hello.fd
T=tests/bootstrap/test_sp3_midend_reach.fd

# build <exe> <env...> -- <emit args...>: 0 when the executable was made
build() {
  local exe="$1"; shift
  local envs=()
  while [ "$1" != "--" ]; do envs+=("$1"); shift; done
  shift
  rm -f "$exe"
  env "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 && [ -f "$exe" ]
}

# The "Expected output:" lines of a test's header comment
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}

# 1. output, default and with every function
want "$T" > "$OUT/reach.want"
printf 'Hello, Fides!\n50\ny is big\n' > "$OUT/hello.want"
for t in "$T" "$H"; do
  n=$(basename "$t" .fd)
  w="$OUT/reach.want"
  [ "$t" = "$H" ] && w="$OUT/hello.want"
  for tgt in pe_ir nat; do
    extra=""
    [ "$tgt" = nat ] && extra="--nat"
    for cfg in default all; do
      case "$cfg" in
        default) envs=(X=1) ;;
        all) envs=(FID_IROPT_OFF=midreach) ;;
      esac
      exe="$OUT/$n.$tgt.$cfg.exe"
      if build "$exe" "${envs[@]}" FID_VERIFY=1 -- "$t" $extra; then
        timeout 60 "$exe" < /dev/null > "$exe.out" 2>&1
        if cmp -s <(tr -d '\r' < "$exe.out") "$w"; then
          ok "output $n $tgt $cfg"
        else
          no "output $n $tgt $cfg (see $exe.out)"
        fi
      else
        no "build $n $tgt $cfg: $(grep -m1 -i -E 'error|panic|invalid|verif' "$exe.log")"
      fi
    done
  done
done

# 2. the e-graph sees fewer functions
EGF='s/^\[egraph\] functions \([0-9]*\),.*/\1/p'
nd=-1
na=-1
if build "$OUT/eg_default.exe" FID_EGRAPH_STATS=1 -- "$H" --nat; then
  nd=$(sed -n "$EGF" "$OUT/eg_default.exe.log" | tr -d '\r' | head -1)
fi
if build "$OUT/eg_all.exe" FID_EGRAPH_STATS=1 FID_IROPT_OFF=midreach -- "$H" --nat; then
  na=$(sed -n "$EGF" "$OUT/eg_all.exe.log" | tr -d '\r' | head -1)
fi
if [ -n "$nd" ] && [ -n "$na" ] && [ "$nd" -ge 0 ] 2>/dev/null && [ "$nd" -lt "$na" ] 2>/dev/null; then
  ok "e-graph functions on hello --nat: $nd (every function: $na)"
else
  no "e-graph functions on hello --nat: '$nd', every function '$na' (expected fewer by default)"
fi

# 3. the time of hello
if [ "$TIME" = 1 ]; then
  # best_total <tag> <env...>: the best "[time] total" of RUNS runs; the
  # log of that run in $OUT/time.<tag>.best
  best_total() {
    local tag="$1"; shift
    local best=-1 r t
    env "$@" "$FID" emit "$H" -o "$OUT/time.$tag.exe" --time > "$OUT/time.$tag.warm" 2>&1
    for ((r = 0; r < RUNS; r++)); do
      env "$@" "$FID" emit "$H" -o "$OUT/time.$tag.exe" --time > "$OUT/time.$tag.$r" 2>&1
      t=$(sed -n 's/^\[time\] total: \([0-9]*\) ms.*/\1/p' "$OUT/time.$tag.$r" | tr -d '\r' | head -1)
      [ -z "$t" ] && continue
      if [ "$best" -lt 0 ] || [ "$t" -lt "$best" ]; then
        best=$t
        cp "$OUT/time.$tag.$r" "$OUT/time.$tag.best"
      fi
    done
    echo "$best"
  }
  rows() {  # the main rows of a --time log, on one line
    grep -E '^\[time\] (front end|lower|iropt|rules|egraph|native code|total)' "$1" | tr -d '\r' | sed 's/^\[time\] //' | tr '\n' ',' | sed 's/,$//'
  }
  bd=$(best_total default X=1)
  ba=$(best_total all FID_IROPT_OFF=midreach)
  if [ "$bd" -ge 0 ] 2>/dev/null && [ "$bd" -lt "$LIMIT" ]; then
    ok "hello emit: $bd ms (under $LIMIT ms)"
  else
    no "hello emit: $bd ms (not under $LIMIT ms)"
  fi
  [ -f "$OUT/time.default.best" ] && info "default: $(rows "$OUT/time.default.best")"
  [ -f "$OUT/time.default.best" ] && info "iropt passes: $(grep -E '^\[time\]   iropt ' "$OUT/time.default.best" | tr -d '\r' | sed 's/^\[time\]   iropt //' | tr '\n' ',' | sed 's/,$//')"
  info "every function (FID_IROPT_OFF=midreach): $ba ms"
  [ -f "$OUT/time.all.best" ] && info "every function: $(rows "$OUT/time.all.best")"
  if [ "$bd" -ge 0 ] 2>/dev/null && [ "$bd" -lt 100 ]; then
    info "the second target (under 100 ms) is met too"
  fi
fi

# 4. the suite, reach-only and every function
if [ "$SUITE" = 1 ]; then
  suite() {  # <extra>
    local tag="pe_ir"
    [ -n "$1" ] && tag="nat"
    local log="$OUT/suite-$tag.txt"
    PASS=midreach FID="$FID" JOBS="$JOBS" EXTRA="$1" OUT="$OUT/suite-$tag" \
      bash tests/tooling/midend-pass-suite-check.sh > "$log" 2>&1
    local rc=$?
    if [ "$rc" = 0 ]; then
      ok "suite midreach $tag: $(tail -1 "$log" | tr -d '\r')"
    else
      no "suite midreach $tag (exit $rc; see $log): $(grep -E '^(DIFF|BUILD|VERIFY)' "$log" | head -5 | tr '\n' '|')"
    fi
  }
  suite ""
  if [ "$NAT" = 1 ]; then
    suite --nat
  fi
fi

echo "sp3-hello-check: $bad failed"
exit "$bad"
