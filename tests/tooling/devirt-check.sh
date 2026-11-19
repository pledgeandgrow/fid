#!/bin/bash
# devirt-check.sh — OP14 (competitive-roadmap): the devirtualization passes
# of compiler/iropt.fd that are on by default and work across functions:
# `dynspec` (iro_dyn_specialize: a function cloned per concrete type of a
# `dyn` argument) and `fielddevirt` (iro_field_devirt: a trait call or a
# closure call on a value read from a struct field every store of which
# stores one dyn type / one closure target).
#
# Usage: tests/tooling/devirt-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/devirt
#   NAT=1            also each program through `emit --nat` (the machine
#                    layer) besides pe_ir (the default `emit` on Windows)
#   no test files: every tests/bootstrap/test_*.fd that is not _fail.fd
#
# 1. The passes do something: FID_IROPT_STATS=1 prints "iropt: dynspec N
#    clones, fielddevirt D dyn calls, C closure calls"; N > 0 for
#    test_midend_devirt_param.fd, D >= 3 and C >= 1 for
#    test_midend_devirt_field.fd.
# 2. The OP14 tests (test_midend_devirt_param, test_midend_devirt_field)
#    print their "Expected output" block, with the
#    passes on and with them off.
# 3. Every program: built with FID_IROPT_OFF=dynspec,fielddevirt and with
#    the default passes (FID_VERIFY=1), the two executables print the same
#    and exit with the same status. SAME / DIFF / BUILD (built only with the
#    passes off) / SKIP (built with neither, or only with them on) lines.
# Exit status: failed checks of 1-2 plus DIFF and BUILD lines. A DIFF or
# BUILD line on the suite means the pass it comes from leaves the default
# (FID_IROPT_OFF=dynspec or =fielddevirt alone tells which).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/devirt}"
NAT="${NAT:-0}"
PASSES="dynspec,fielddevirt"
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
t=tests/bootstrap/test_midend_devirt_field.fd
if build "$OUT/field.stats.exe" FID_IROPT_STATS=1 -- "$t"; then
  line=$(grep -m1 '^iropt: dynspec' "$OUT/field.stats.exe.log")
  d=$(echo "$line" | sed -n 's/.*fielddevirt \([0-9]*\) dyn calls.*/\1/p')
  c=$(echo "$line" | sed -n 's/.* \([0-9]*\) closure calls.*/\1/p')
  if [ -n "$d" ] && [ -n "$c" ] && [ "$d" -ge 3 ] && [ "$c" -ge 1 ]; then
    echo "OK    stats   test_midend_devirt_field: $line"
  else
    echo "FAIL  stats   test_midend_devirt_field: '$line' (expected >= 3 dyn calls, >= 1 closure call)"
    bad=$((bad + 1))
  fi
else
  echo "FAIL  build   test_midend_devirt_field: $(grep -m1 -i -E 'error|panic' "$OUT/field.stats.exe.log")"
  bad=$((bad + 1))
fi
t=tests/bootstrap/test_midend_devirt_param.fd
if build "$OUT/param.stats.exe" FID_IROPT_STATS=1 -- "$t"; then
  line=$(grep -m1 '^iropt: dynspec' "$OUT/param.stats.exe.log")
  n=$(echo "$line" | sed -n 's/^iropt: dynspec \([0-9]*\) clones.*/\1/p')
  if [ -n "$n" ] && [ "$n" -gt 0 ]; then
    echo "OK    stats   test_midend_devirt_param: $line"
  else
    echo "FAIL  stats   test_midend_devirt_param: '$line' (expected > 0 clones)"
    bad=$((bad + 1))
  fi
else
  echo "FAIL  build   test_midend_devirt_param: $(grep -m1 -i -E 'error|panic' "$OUT/param.stats.exe.log")"
  bad=$((bad + 1))
fi

# 2. the expected output, passes on and off
expected() {  # <test.fd> -> the lines of its "// Expected output:" block
  awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
for n in test_midend_devirt_param test_midend_devirt_field; do
  t="tests/bootstrap/$n.fd"
  [ -f "$t" ] || continue
  expected "$t" > "$OUT/$n.expected"
  for mode in on off; do
    exe="$OUT/$n.exp.$mode.exe"
    if [ "$mode" = on ]; then
      build "$exe" FID_VERIFY=1 -- "$t"; ok=$?
    else
      build "$exe" FID_IROPT_OFF="$PASSES" -- "$t"; ok=$?
    fi
    if [ "$ok" -ne 0 ]; then
      echo "FAIL  build   $n ($mode): $(grep -m1 -i -E 'error|panic|invalid' "$exe.log")"
      bad=$((bad + 1))
      continue
    fi
    timeout 60 "$exe" 2>&1 | tr -d '\r' > "$exe.out"
    if [ -s "$OUT/$n.expected" ] && cmp -s "$OUT/$n.expected" "$exe.out"; then
      echo "OK    output  $n ($mode)"
    else
      echo "FAIL  output  $n ($mode):"
      diff "$OUT/$n.expected" "$exe.out" | head -6 | sed 's/^/      /'
      bad=$((bad + 1))
    fi
  done
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
  if build "$on" FID_VERIFY=1 -- "$t" "$@"; then
    timeout 60 "$on" > "$on.out" 2>&1; b=$?
  fi
  if [ "$a" = BUILD ] && [ "$b" = BUILD ]; then
    printf 'SKIP  %-6s %s (builds with neither)\n' "$label" "$n"
  elif [ "$b" = BUILD ]; then
    printf 'BUILD %-6s %s (on: %s)\n' "$label" "$n" "$(grep -m1 -i -E 'error|panic|invalid' "$on.log")"
  elif [ "$a" = BUILD ]; then
    printf 'SKIP  %-6s %s (builds only with the passes on)\n' "$label" "$n"
  elif [ "$a" = "$b" ] && cmp -s "$off.out" "$on.out"; then
    printf 'SAME  %-6s %s (rc %s)\n' "$label" "$n" "$a"
  else
    printf 'DIFF  %-6s %s (rc off %s, on %s)\n' "$label" "$n" "$a" "$b"
    diff "$off.out" "$on.out" | head -6 | sed 's/^/      /'
  fi
}
: > "$OUT/results.txt"
for t in "${TESTS[@]}"; do
  round pe_ir "$t" | tee -a "$OUT/results.txt"
  if [ "$NAT" = 1 ]; then
    round nat "$t" --nat | tee -a "$OUT/results.txt"
  fi
done
nbad=$(grep -c -E '^(DIFF|BUILD)' "$OUT/results.txt")
bad=$((bad + nbad))
echo "devirt-check: ${#TESTS[@]} programs, $(grep -c '^SAME' "$OUT/results.txt") same, $nbad DIFF/BUILD, $bad checks failed"
exit "$bad"
