#!/bin/bash
# ssa-default-check.sh — the mid-end's SSA round trip (compiler/ssa.fd,
# competitive-roadmap OP2) is on by default: `ssa` and `ssaexit` are in
# compiler/iropt_me.fd IME_DEFAULT_ON.
#
# For each test: `emit` with no switch, with FID_IROPT_ON=ssa,ssaexit and
# with FID_IROPT_OFF=ssa,ssaexit. The default executable must be
# byte-identical to the "on" one (the passes run without being named) and
# differ from the "off" one on test_ssa_merge (they do something there);
# all three must print the same and exit the same.
#
# Usage: tests/tooling/ssa-default-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/ssadefault
#   NAT=1            also through `emit --nat`
# Exit status: the number of failing tests.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/ssadefault}"
NAT="${NAT:-0}"
mkdir -p "$OUT"
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  TESTS=(tests/bootstrap/test_ssa_merge.fd tests/bootstrap/test_mem2reg_promote.fd)
fi
bad=0

# one <label> <test> <emit args...>
one() {
  local label="$1" t="$2"; shift 2
  local n; n=$(basename "$t" .fd)
  local d="$OUT/$n.$label.def.exe" on="$OUT/$n.$label.on.exe" off="$OUT/$n.$label.off.exe"
  rm -f "$d" "$on" "$off"
  env -u FID_IROPT_ON -u FID_IROPT_OFF FID_VERIFY=1 "$FID" emit "$t" "$@" -o "$d" > "$d.log" 2>&1
  env -u FID_IROPT_OFF FID_IROPT_ON=ssa,ssaexit FID_VERIFY=1 "$FID" emit "$t" "$@" -o "$on" > "$on.log" 2>&1
  env -u FID_IROPT_ON FID_IROPT_OFF=ssa,ssaexit FID_VERIFY=1 "$FID" emit "$t" "$@" -o "$off" > "$off.log" 2>&1
  if [ ! -f "$d" ] || [ ! -f "$on" ] || [ ! -f "$off" ]; then
    echo "BUILD $label $n (default $([ -f "$d" ] && echo ok || echo fails), on $([ -f "$on" ] && echo ok || echo fails), off $([ -f "$off" ] && echo ok || echo fails))"
    return 1
  fi
  local r1 r2 r3
  timeout 60 "$d" > "$d.out" 2>&1; r1=$?
  timeout 60 "$on" > "$on.out" 2>&1; r2=$?
  timeout 60 "$off" > "$off.out" 2>&1; r3=$?
  if [ "$r1" != "$r2" ] || [ "$r1" != "$r3" ] || ! cmp -s "$d.out" "$on.out" || ! cmp -s "$d.out" "$off.out"; then
    echo "DIFF  $label $n (output or exit status: default $r1, on $r2, off $r3)"
    return 1
  fi
  if ! cmp -s "$d" "$on"; then
    echo "OFF   $label $n (the default build is not the one with ssa,ssaexit on: not on by default)"
    return 1
  fi
  if [ "$n" = "test_ssa_merge" ] && cmp -s "$d" "$off"; then
    echo "NOOP  $label $n (ssa,ssaexit changed nothing where they must)"
    return 1
  fi
  echo "OK    $label $n (rc $r1)"
  return 0
}

for t in "${TESTS[@]}"; do
  one pe "$t" || bad=$((bad + 1))
  if [ "$NAT" = "1" ]; then
    one nat "$t" --nat || bad=$((bad + 1))
  fi
done
echo "ssa-default-check: ${#TESTS[@]} tests, $bad failing"
exit "$bad"
