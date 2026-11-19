#!/bin/bash
# inline2-suite-check.sh — OP5 (competitive roadmap), ME20/ME21 (llvm
# roadmap): inline2, the cost-model inliner (compiler/iropt_me.fd
# ime_inline_reached, on by default: IME_DEFAULT_ON; FID_IROPT_OFF=inline2
# turns it off), must not change what any program of the suite does.
#
# Usage: tests/tooling/inline2-suite-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/inline2-suite
#   JOBS=<n>         programs at a time (default 1; each is two builds)
#   ONENV="FID_IROPT_ON=inline2"  environment of the "on" build (a
#                    compiler older than OP5, where inline2 is opt-in)
#   EXTRA="--nat"    more `emit` arguments (another backend: --nat, or
#                    --target x86_64-linux / aarch64-linux, run under qemu
#                    by the caller's binfmt)
#   no test files: every tests/bootstrap/*.fd that is not expected to fail
#   (_fail.fd, `//! error`) and needs no outside library (sqlite, SDL, GL)
#
# For each program: `fid emit` with inline2 named off and with the default
# (inline2 on), both with FID_VERIFY=1 (the IR verifier runs after iropt)
# and FID_IROPT_STATS=1; each executable runs with stdin from /dev/null and
# a 60 s limit. One line per program:
#   SAME    same output and exit status
#   DIFF    output or exit status differ (the first lines of the diff follow)
#   BUILD   builds with inline2 off, not with it on
#   VERIFY  the IR verifier complains with inline2 on and not off
#   SKIP    builds with neither (or only on)
# then totals: iropt's instructions before its function passes (after
# inlining) and iropt's time, on vs off. Exit status: DIFF + BUILD + VERIFY.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2-suite}"
JOBS="${JOBS:-1}"
EXTRA="${EXTRA:-}"
ONENV="${ONENV:-X_INLINE2=default}"
mkdir -p "$OUT"
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in tests/bootstrap/*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    grep -q '^//! error' "$t" && continue
    grep -q -E 'sqlite3|SDL_|sdl2_h|glClear|use gl;|use sdl' "$t" && continue
    TESTS+=("$t")
  done
fi

# one <test>: writes $OUT/<name>.res (the line) and .num (counts)
one() {
  local t="$1" n
  n=$(basename "$t" .fd)
  local rcs=() m
  for m in off on; do
    local exe="$OUT/$n.$m.exe"
    rm -f "$exe" "$exe.out"
    if [ $m = off ]; then
      FID_IROPT_OFF=inline2 FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
    else
      env $ONENV FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
    fi
    if [ -f "$exe" ]; then
      timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
      rcs+=("$?")
    else
      rcs+=("BUILD")
    fi
  done
  local a="${rcs[0]}" b="${rcs[1]}" line
  local voff von
  voff=$(grep -c "IR verifier" "$OUT/$n.off.exe.log" 2>/dev/null)
  von=$(grep -c "IR verifier" "$OUT/$n.on.exe.log" 2>/dev/null)
  if [ "$a" = "BUILD" ] && [ "$b" = "BUILD" ]; then
    line=$(printf 'SKIP   %s (builds with neither)' "$n")
  elif [ "$a" = "BUILD" ]; then
    line=$(printf 'SKIP   %s (builds only with inline2 on)' "$n")
  elif [ "$b" = "BUILD" ]; then
    line=$(printf 'BUILD  %s (on: %s)' "$n" "$(grep -m1 -i -E 'error|panic|invalid|verifier' "$OUT/$n.on.exe.log")")
  elif [ "${von:-0}" -gt "${voff:-0}" ]; then
    line=$(printf 'VERIFY %s (%s)' "$n" "$(grep -m1 "IR verifier" "$OUT/$n.on.exe.log")")
  elif [ "$a" = "$b" ] && cmp -s <(tr -d '\r' < "$OUT/$n.off.exe.out") <(tr -d '\r' < "$OUT/$n.on.exe.out"); then
    line=$(printf 'SAME   %s (rc %s)' "$n" "$a")
  else
    line=$(printf 'DIFF   %s (rc off %s, on %s)\n%s' "$n" "$a" "$b" "$(diff "$OUT/$n.off.exe.out" "$OUT/$n.on.exe.out" | head -6 | sed 's/^/       /')")
  fi
  echo "$line" > "$OUT/$n.res"
  # iropt: instructions <before> -> <after> in <ms> ms
  local s m2
  : > "$OUT/$n.num"
  for m2 in off on; do
    s=$(tr -d '\r' < "$OUT/$n.$m2.exe.log" | sed -n 's/^iropt: instructions \([0-9]*\) -> \([0-9]*\) in \([0-9]*\) ms.*/\1 \2 \3/p' | tail -1)
    echo "$m2 ${s:-0 0 0}" >> "$OUT/$n.num"
  done
}
export -f one
export FID OUT EXTRA ONENV

printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$1"' _ {}

: > "$OUT/results.txt"
ib_off=0; ib_on=0; ms_off=0; ms_on=0
for t in "${TESTS[@]}"; do
  n=$(basename "$t" .fd)
  [ -f "$OUT/$n.res" ] && cat "$OUT/$n.res" >> "$OUT/results.txt"
  if [ -f "$OUT/$n.num" ]; then
    while read -r m b a ms; do
      if [ "$m" = off ]; then ib_off=$((ib_off + b)); ms_off=$((ms_off + ms)); else ib_on=$((ib_on + b)); ms_on=$((ms_on + ms)); fi
    done < "$OUT/$n.num"
  fi
done
cat "$OUT/results.txt"
bad=$(grep -c -E '^(DIFF|BUILD|VERIFY)' "$OUT/results.txt")
same=$(grep -c '^SAME' "$OUT/results.txt")
echo "inline2-suite-check: ${#TESTS[@]} programs, $same the same, $bad differ, fail to build or to verify with inline2 on"
echo "inline2-suite-check: iropt instructions after inlining $ib_off (off) -> $ib_on (on); iropt time $ms_off ms (off) -> $ms_on ms (on)"
exit "$bad"
