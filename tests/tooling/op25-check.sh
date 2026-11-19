#!/bin/bash
# op25-check.sh — OP25 (competitive roadmap: effects drive optimization).
# What it checks, one line each (OK / FAIL), exit status the number of FAILs:
#
#  1. The parts do something (FID_IROPT_STATS=1, FID_RULE_STATS=1; inline
#     and inline2 off, or the calls they act on are inlined away):
#     - quietcall (compiler/iropt.fd iro_dead_calls, on by default):
#       "iropt: quietcall N quiet functions, dead calls removed in M" with
#       M > 0 for test_midend_quietcall.fd;
#     - guardcall (compiler/iropt_me.fd ime_guard_moves, with the opt-in
#       `licmguard`): "iropt: guardcall N calls" with N > 0 for
#       test_midend_guard_call.fd, and N = 0 with FID_IROPT_OFF=guardcall;
#     - nowritecall (ime_writes, on by default): "nowritecall N facts kept"
#       with N > 0 for test_midend_nowrite_facts.fd, 0 with
#       FID_IROPT_OFF=nowritecall;
#     - readcse (compiler/rules.fd, on by default, native machine layer
#       only: `emit --nat`): "[rules] readcse: N" with N > 0 for
#       test_midend_nowrite_facts.fd, 0 with FID_NAT_OFF=readcse.
#  2. test_midend_guard_call_fail.fd with the guard on panics after "0" and
#     "start", never before, and never prints "unreachable".
#  3. The three tests print their "Expected output:" lines (FID_VERIFY=1)
#     with everything on (licmguard named on), by default, and with every
#     OP25 switch named off; through pe_ir and `--nat`.
#  4. SUITE=1: every program of tests/bootstrap through
#     tests/tooling/midend-pass-suite-check.sh with the default-on parts
#     (quietcall,nowritecall,readcse) named off and on, then with
#     licmguard,guardcall named on and off (EXTRA, JOBS passed on; NAT=1:
#     both again with EXTRA=--nat). Each must end "0 differ".
#
# Usage: tests/tooling/op25-check.sh
#   FID=<compiler> (default release/fid.exe), OUT=<dir> (default build/op25),
#   SUITE=1, NAT=1, JOBS=<n>
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/op25}"
SUITE="${SUITE:-0}"
NAT="${NAT:-0}"
JOBS="${JOBS:-1}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK    $1"; }
no() { echo "FAIL  $1"; bad=$((bad + 1)); }

G=tests/bootstrap/test_midend_guard_call.fd
GF=tests/bootstrap/test_midend_guard_call_fail.fd
W=tests/bootstrap/test_midend_nowrite_facts.fd
Q=tests/bootstrap/test_midend_quietcall.fd
ALL_OFF=quietcall,guardcall,nowritecall,readcse,inline,inline2

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

# stat <label> <sed expression printing the number> <env...> -- <emit args>
stat() {
  local label="$1" expr="$2"; shift 2
  local exe="$OUT/$label.exe"
  if build "$exe" FID_IROPT_STATS=1 FID_RULE_STATS=1 "$@"; then
    sed -n "$expr" "$exe.log" | tr -d '\r' | head -1
  else
    echo "-1"
  fi
}
want_pos() {  # <what> <count>
  if [ -n "$2" ] && [ "$2" -gt 0 ] 2>/dev/null; then ok "stats $1: $2"; else no "stats $1: '$2' (expected > 0)"; fi
}
want_zero() {
  if [ "$2" = 0 ]; then ok "stats $1: 0"; else no "stats $1: '$2' (expected 0)"; fi
}

# 1. counters
QC='s/^iropt: quietcall [0-9]* quiet functions, dead calls removed in \([0-9]*\) functions.*/\1/p'
GC='s/^iropt: guardcall \([0-9]*\) calls.*/\1/p'
NW='s/^iropt: guardcall [0-9]* calls, nowritecall \([0-9]*\) facts kept.*/\1/p'
RC='s/^\[rules\] readcse: \([0-9]*\).*/\1/p'
want_pos "quietcall dead calls" "$(stat qc "$QC" FID_IROPT_OFF=inline,inline2 -- "$Q")"
want_pos "guardcall" "$(stat gc "$GC" FID_IROPT_ON=licmguard FID_IROPT_OFF=inline,inline2 -- "$G")"
want_zero "guardcall off" "$(stat gc_off "$GC" FID_IROPT_ON=licmguard FID_IROPT_OFF=inline,inline2,guardcall -- "$G")"
want_pos "nowritecall" "$(stat nw "$NW" FID_IROPT_OFF=inline,inline2 -- "$W")"
want_zero "nowritecall off" "$(stat nw_off "$NW" FID_IROPT_OFF=inline,inline2,nowritecall -- "$W")"
want_pos "readcse" "$(stat rc "$RC" FID_IROPT_OFF=inline,inline2 -- "$W" --nat)"
want_zero "readcse off" "$(stat rc_off "$RC" FID_IROPT_OFF=inline,inline2 FID_NAT_OFF=readcse -- "$W" --nat)"

# 2. the guarded call panics where the loop's first pass would
for tgt in pe_ir nat; do
  extra=""
  [ "$tgt" = nat ] && extra="--nat"
  exe="$OUT/guard_fail.$tgt.exe"
  if build "$exe" FID_IROPT_ON=licmguard FID_IROPT_OFF=inline,inline2 -- "$GF" $extra; then
    timeout 60 "$exe" > "$exe.out" 2>&1
    rc=$?
    if [ "$rc" -ne 0 ] && [ "$(head -2 "$exe.out" | tr -d '\r' | tr '\n' ' ')" = "0 start " ] && ! grep -q unreachable "$exe.out"; then
      ok "panic $tgt (rc $rc after \"start\")"
    else
      no "panic $tgt (rc $rc): $(head -3 "$exe.out" | tr -d '\r' | tr '\n' '|')"
    fi
  else
    no "build guard_fail $tgt: $(grep -m1 -i -E 'error|panic|invalid' "$exe.log")"
  fi
done

# 3. expected output in every configuration
for t in "$G" "$W" "$Q"; do
  n=$(basename "$t" .fd)
  want "$t" > "$OUT/$n.want"
  for tgt in pe_ir nat; do
    extra=""
    [ "$tgt" = nat ] && extra="--nat"
    for cfg in on default off; do
      case "$cfg" in
        on) envs=(FID_IROPT_ON=licmguard FID_IROPT_OFF=inline,inline2) ;;
        default) envs=(X=1) ;;
        off) envs=(FID_IROPT_OFF=$ALL_OFF FID_NAT_OFF=readcse) ;;
      esac
      exe="$OUT/$n.$tgt.$cfg.exe"
      if build "$exe" "${envs[@]}" FID_VERIFY=1 -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1
        if cmp -s <(tr -d '\r' < "$exe.out") "$OUT/$n.want"; then
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

# 4. the suite, on and off
if [ "$SUITE" = 1 ]; then
  suite() {  # <pass list> <extra>
    local log="$OUT/suite-$(echo "$1" | tr ',' '-')${2:+-nat}.txt"
    PASS="$1" FID="$FID" JOBS="$JOBS" EXTRA="$2" OUT="$OUT/suite-$(echo "$1" | tr ',' '-')${2:+-nat}" \
      bash tests/tooling/midend-pass-suite-check.sh > "$log" 2>&1
    local rc=$?
    if [ "$rc" = 0 ]; then
      ok "suite $1 ${2:-pe_ir}: $(tail -1 "$log" | tr -d '\r')"
    else
      no "suite $1 ${2:-pe_ir} (exit $rc; see $log): $(grep -E '^(DIFF|BUILD|VERIFY)' "$log" | head -5 | tr '\n' '|')"
    fi
  }
  suite quietcall,nowritecall,readcse ""
  suite licmguard,guardcall ""
  if [ "$NAT" = 1 ]; then
    suite quietcall,nowritecall,readcse --nat
    suite licmguard,guardcall --nat
  fi
fi

echo "op25-check: $bad failed"
exit "$bad"
