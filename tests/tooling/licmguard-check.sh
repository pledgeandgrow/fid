#!/bin/bash
# licmguard-check.sh — OP9 (competitive-roadmap): the opt-in mid-end passes
# `licmguard` (trapping arithmetic out of a loop under a guard, from the
# first block of its body or a deeper one, compiler/iropt_me.fd
# ime_licm_guard / ime_guard_reaches) and `vecdata` (the data pointer of a
# Vec<i64> a loop only reads, loaded before the loop, ime_vec_data).
#
# Usage: tests/tooling/licmguard-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/licmguard
#   PASSES=...       default licmguard,vecdata
#   NAT=1            also each program through `emit --nat` (the machine
#                    layer) besides pe_ir
#   no test files: every tests/bootstrap/test_*.fd that is not _fail.fd
#
# 1. The passes do something: FID_IROPT_STATS=1 prints "iropt: licmguard N
#    operations, vecdata M reads"; N > 0 for test_midend_licm_guard.fd and
#    test_midend_licm_guard_deep.fd, M > 0 for test_midend_vec_data.fd.
# 2. The guarded division of the _fail tests panics after "start", never
#    before it, and "unreachable" is never printed.
# 3. Every program: built with the passes named off (FID_IROPT_OFF) and on
#    (FID_IROPT_ON, FID_VERIFY=1), the two executables print the same and
#    exit with the same status. SAME / DIFF / BUILD (built only with the
#    passes off) / SKIP (built with neither, or only with them on) lines.
# Exit status: the number of failed checks of 1-2 plus DIFF and BUILD lines.
# When it exits 0 on the whole suite, both passes can go into
# IME_DEFAULT_ON (compiler/iropt_me.fd).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/licmguard}"
PASSES="${PASSES:-licmguard,vecdata}"
NAT="${NAT:-0}"
mkdir -p "$OUT"
bad=0

# build <exe> <env...> -- <emit args...> : 0 when the executable was made
build() {
  local exe="$1"; shift
  local envs=()
  while [ "$1" != "--" ]; do envs+=("$1"); shift; done
  shift
  rm -f "$exe"
  env "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 && [ -f "$exe" ]
}

# 1. counters
stat_of() {  # <test> <field: licmguard|vecdata> -> the count
  local t="$1" what="$2" n
  n=$(basename "$t" .fd)
  build "$OUT/$n.stats.exe" FID_IROPT_ON="$PASSES" FID_IROPT_STATS=1 -- "$t" || { echo "-1"; return; }
  local line
  line=$(grep -m1 '^iropt: licmguard' "$OUT/$n.stats.exe.log")
  if [ "$what" = licmguard ]; then
    echo "$line" | sed -n 's/^iropt: licmguard \([0-9]*\) operations.*/\1/p'
  else
    echo "$line" | sed -n 's/.*vecdata \([0-9]*\) reads.*/\1/p'
  fi
}
check_stat() {  # <test> <field>
  local c
  c=$(stat_of "tests/bootstrap/$1.fd" "$2")
  if [ -n "$c" ] && [ "$c" -gt 0 ] 2>/dev/null; then
    echo "OK    stats   $1: $2 $c"
  else
    echo "FAIL  stats   $1: $2 '${c}' (expected > 0)"
    bad=$((bad + 1))
  fi
}
check_stat test_midend_licm_guard licmguard
check_stat test_midend_licm_guard_deep licmguard
check_stat test_midend_vec_data vecdata

# 2. the panics come where the loop's first pass would raise them
for n in test_midend_licm_guard_fail test_midend_licm_guard_deep_fail; do
  t="tests/bootstrap/$n.fd"
  [ -f "$t" ] || continue
  if build "$OUT/$n.on.exe" FID_IROPT_ON="$PASSES" -- "$t"; then
    timeout 60 "$OUT/$n.on.exe" > "$OUT/$n.on.out" 2>&1
    rc=$?
    if [ "$rc" -ne 0 ] && [ "$(head -2 "$OUT/$n.on.out" | tr -d '\r' | tr '\n' ' ')" = "0 start " ] && ! grep -q unreachable "$OUT/$n.on.out"; then
      echo "OK    panic   $n (rc $rc after \"start\")"
    else
      echo "FAIL  panic   $n (rc $rc):"
      head -4 "$OUT/$n.on.out" | sed 's/^/      /'
      bad=$((bad + 1))
    fi
  else
    echo "FAIL  build   $n: $(grep -m1 -i -E 'error|panic' "$OUT/$n.on.exe.log")"
    bad=$((bad + 1))
  fi
done

# 3. same output and exit status, passes on and off
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in tests/bootstrap/test_*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    TESTS+=("$t")
  done
fi
round() {  # <label> <test> <emit args...>
  local label="$1" t="$2"; shift 2
  local n a b
  n=$(basename "$t" .fd)
  local off="$OUT/$n.$label.off.exe" on="$OUT/$n.$label.on.exe"
  a=BUILD; b=BUILD
  if build "$off" FID_IROPT_OFF="$PASSES" -- "$t" "$@"; then
    timeout 60 "$off" > "$off.out" 2>&1; a=$?
  fi
  if build "$on" FID_IROPT_ON="$PASSES" FID_VERIFY=1 -- "$t" "$@"; then
    timeout 60 "$on" > "$on.out" 2>&1; b=$?
  fi
  if [ "$a" = BUILD ] && [ "$b" = BUILD ]; then
    printf 'SKIP  %-6s %s (builds with neither)\n' "$label" "$n"
  elif [ "$b" = BUILD ]; then
    printf 'BUILD %-6s %s (on: %s)\n' "$label" "$n" "$(grep -m1 -i -E 'error|panic|invalid' "$on.log")"
    return 1
  elif [ "$a" = BUILD ]; then
    printf 'SKIP  %-6s %s (builds only with the passes on)\n' "$label" "$n"
  elif [ "$a" = "$b" ] && cmp -s "$off.out" "$on.out"; then
    printf 'SAME  %-6s %s (rc %s)\n' "$label" "$n" "$a"
  else
    printf 'DIFF  %-6s %s (rc off %s, on %s)\n' "$label" "$n" "$a" "$b"
    diff "$off.out" "$on.out" | head -6 | sed 's/^/      /'
    return 1
  fi
  return 0
}
for t in "${TESTS[@]}"; do
  round pe_ir "$t" || bad=$((bad + 1))
  if [ "$NAT" = 1 ]; then
    round nat "$t" --nat || bad=$((bad + 1))
  fi
done 2>&1 | tee "$OUT/results.txt"
bad=$((bad + $(grep -c -E '^(DIFF|BUILD)' "$OUT/results.txt")))
echo "licmguard-check: ${#TESTS[@]} programs, $bad checks failed"
exit "$bad"
