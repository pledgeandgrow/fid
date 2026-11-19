#!/usr/bin/env bash
# fuzz_ir.sh — random IR through the machine layer (llvm-roadmap RL10).
#
#   scripts/fuzz/fuzz_ir.sh [count] [first_seed]
#     FID=<fid>          the compiler (default release/fid.exe, else release/fid)
#     TARGET=<t>         x86_64-linux (default), aarch64-linux, riscv64-linux,
#                        or x86_64-windows (--nat, on a Windows host)
#     MIDEND=1           run the mid-end on the random IR too (--mid-end)
#     FAST=1             RA14's fast allocator (--fast-alloc)
#     SIZE=1             TG19's --size
#
# For each seed, `fid ir-fuzz <seed>` builds a program whose function is IR
# made from the seed (compiler/ir_ser.fd irfuzz_function and
# irfuzz_helper_function: integer and f64 arithmetic, if/else on
# comparisons, counted loops, stack slots, a p_malloc'd heap buffer, and
# calls between the generated functions fz_target, fz_h0 and fz_h1) and prints
# `expected: <v>`, the value the IR computes (worked out by irfuzz_eval).
# The program is run (natively when the host matches the target, else
# under qemu-<arch>; without either the build alone is checked) and must
# print v. A failing seed keeps its IR: build/fuzz-ir/fail-<seed>.fir,
# replayed with `fid emit-ir build/fuzz-ir/fail-<seed>.fir`, and shrunk
# by hand (the description is in the seed). Exit status 1 on any failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
COUNT="${1:-100}"
FIRST="${2:-1}"
TARGET="${TARGET:-x86_64-linux}"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT=build/fuzz-ir
mkdir -p "$OUT"
flags=()
case "$TARGET" in
  *windows*) flags+=(--nat) ;;
  *) flags+=(--target "$TARGET") ;;
esac
[ -n "${MIDEND:-}" ] && flags+=(--mid-end)
[ -n "${FAST:-}" ] && flags+=(--fast-alloc)
[ -n "${SIZE:-}" ] && flags+=(--size)

# How to run an image of the target here ("" = build only)
runner=""
host_arch="$(uname -m 2>/dev/null)"
case "$TARGET" in
  x86_64-linux)
    if [ "$(uname -s)" = Linux ] && [ "$host_arch" = x86_64 ]; then runner="direct"
    elif command -v qemu-x86_64 > /dev/null; then runner="qemu-x86_64"; fi ;;
  aarch64-linux)
    if [ "$(uname -s)" = Linux ] && [ "$host_arch" = aarch64 ]; then runner="direct"
    elif command -v qemu-aarch64 > /dev/null; then runner="qemu-aarch64"; fi ;;
  riscv64-linux)
    if command -v qemu-riscv64 > /dev/null; then runner="qemu-riscv64"; fi ;;
  *windows*)
    case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) runner="direct" ;; esac ;;
esac
[ -z "$runner" ] && echo "fuzz_ir: no way to run $TARGET images here: checking the builds only"

pass=0
fail=0
seed="$FIRST"
last=$((FIRST + COUNT - 1))
while [ "$seed" -le "$last" ]; do
  exe="$OUT/ir-$seed"
  case "$TARGET" in *windows*) exe="$exe.exe" ;; esac
  log="$OUT/ir-$seed.log"
  if ! "$FID" ir-fuzz "$seed" "${flags[@]}" -o "$exe" > "$log" 2>&1; then
    echo "  FAIL  seed $seed: the build failed (see $log)"
    "$FID" ir-fuzz "$seed" "${flags[@]}" --save-ir "$OUT/fail-$seed.fir" -o "$exe" > /dev/null 2>&1
    fail=$((fail + 1))
    seed=$((seed + 1))
    continue
  fi
  want="$(grep -m1 '^expected: ' "$log" | sed 's/^expected: //' | tr -d '\r')"
  if [ -z "$runner" ]; then
    pass=$((pass + 1))
  else
    if [ "$runner" = direct ]; then got="$("$exe" 2>&1 | tr -d '\r')"; else got="$("$runner" "$exe" 2>&1 | tr -d '\r')"; fi
    if [ "$got" = "$want" ]; then
      pass=$((pass + 1))
      rm -f "$exe" "$log"
    else
      echo "  FAIL  seed $seed: printed '$got', the IR computes '$want'"
      "$FID" ir-fuzz "$seed" "${flags[@]}" --save-ir "$OUT/fail-$seed.fir" -o "$exe" > /dev/null 2>&1
      echo "        IR kept: $OUT/fail-$seed.fir ($FID emit-ir $OUT/fail-$seed.fir ${flags[*]})"
      fail=$((fail + 1))
    fi
  fi
  seed=$((seed + 1))
done
echo "fuzz_ir: $pass passed, $fail failed ($TARGET, seeds $FIRST..$last)"
[ "$fail" -eq 0 ]
