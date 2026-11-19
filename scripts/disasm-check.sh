#!/usr/bin/env bash
# disasm-check.sh — `fid disasm` against llvm-objdump (llvm-roadmap TG5).
#
#   scripts/disasm-check.sh [file.fd ...]     (default: a few suite tests)
#     FID=<fid>            the compiler (default release/fid.exe, else release/fid)
#     OBJDUMP=<objdump>    default llvm-objdump (GNU objdump works for x86-64)
#     TARGETS="x86_64-linux aarch64-linux riscv64-linux"
#     ZIG=<zig>            for the round trip (default zig; ROUNDTRIP=0: off)
#     PY=<python>          default python3, else python
#
# For each program and target: `fid emit` writes the image, `fid disasm`
# lists it by function (compiler/disasm.fd), llvm-objdump -d lists the same
# image. Checked per function:
#   x86-64           the instruction start addresses are the same (the
#                    length decoder agrees with LLVM's on every byte).
#                    objdump reads a copy whose padding (the zeros `fid
#                    disasm` shows as `(N zero bytes of padding)`: loop-head
#                    alignment after a jump, and between functions) is nops
#                    (tests/nattools/disasm_pad.py), with -z, so that it
#                    does not run from the zeros into the next instruction;
#   AArch64/RISC-V   every word LLVM decodes is decoded by us too (no
#                    `.word` where LLVM names an instruction).
# Then, every target, the round trip (tests/nattools/disasm_roundtrip.py):
# each instruction of the listing goes back through clang's assembler
# (`zig cc -c`) and must give its own bytes: the operands are checked, not
# only the lengths. Mnemonic spelling is not compared with objdump (aliases
# differ: movz/mov, li/addi); the round trip checks it is valid assembly.
# Exit status 1 on any difference; without llvm-objdump, the objdump step
# is skipped; without zig, the round trip.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OBJDUMP="${OBJDUMP:-llvm-objdump}"
TARGETS="${TARGETS:-x86_64-linux aarch64-linux riscv64-linux}"
ZIG="${ZIG:-zig}"
ROUNDTRIP="${ROUNDTRIP:-1}"
PY="${PY:-}"
if [ -z "$PY" ]; then
  if command -v python3 > /dev/null && python3 -c 'import sys' > /dev/null 2>&1; then PY=python3; else PY=python; fi
fi
have_zig=1
command -v "$ZIG" > /dev/null || have_zig=0
[ "$ROUNDTRIP" = 0 ] && have_zig=0
[ "$have_zig" = 0 ] && echo "disasm-check: no $ZIG (or ROUNDTRIP=0): no round trip through the assembler"
OUT=build/disasm-check
mkdir -p "$OUT"
files=("$@")
if [ ${#files[@]} -eq 0 ]; then
  files=(tests/bootstrap/test_driver_disasm.fd tests/bootstrap/test_driver_fast_alloc.fd tests/bootstrap/test_machine_fast.fd)
fi
have_objdump=1
command -v "$OBJDUMP" > /dev/null || have_objdump=0
[ "$have_objdump" = 0 ] && echo "disasm-check: no $OBJDUMP: no comparison with objdump"
fail=0
# the round trip of one listing: arch name for disasm_roundtrip.py
roundtrip() {
  [ "$have_zig" = 0 ] && return 0
  local arch="aarch64"
  case "$2" in
    x86_64*) arch="x86_64" ;;
    riscv64*) arch="riscv64" ;;
  esac
  if "$PY" tests/nattools/disasm_roundtrip.py "$1" "$arch" --cc "$ZIG cc" > "$1.rt" 2>&1; then
    echo "  ok    $(basename "$1" .ours) round trip: $(head -1 "$1.rt" | sed -E 's/^disasm_roundtrip: [^:]*: //')"
  else
    echo "  FAIL  $(basename "$1" .ours) round trip (see $1.rt):"
    sed -n '1,12p' "$1.rt" | sed 's/^/        /'
    fail=1
  fi
}
for f in "${files[@]}"; do
  base="$(basename "$f" .fd)"
  for t in $TARGETS; do
    img="$OUT/$base-$t"
    if ! "$FID" emit "$f" --target "$t" -o "$img" > "$img.log" 2>&1; then
      echo "  FAIL  $base $t: emit failed (see $img.log)"; fail=1; continue
    fi
    "$FID" disasm "$img" > "$img.ours" 2>&1
    if ! grep -q '^[0-9a-f]* <main>:' "$img.ours"; then
      echo "  FAIL  $base $t: fid disasm lists no main (see $img.ours)"; fail=1; continue
    fi
    roundtrip "$img.ours" "$t"
    [ "$have_objdump" = 0 ] && { echo "  ok    $base $t (disassembles)"; continue; }
    case "$t" in
      x86_64*)
        # objdump on a copy with the padding as nops (see the header)
        if ! "$PY" tests/nattools/disasm_pad.py "$img" "$img.ours" "$img.nopad" > "$img.pad.log" 2>&1; then
          echo "  FAIL  $base $t: padding that is not zeros (see $img.pad.log)"; fail=1; continue
        fi
        "$OBJDUMP" -d -z --no-show-raw-insn "$img.nopad" > "$img.llvm" 2>&1 || true
        # instruction start addresses of the code both list
        grep -E '^  [0-9a-f]{8}:  ' "$img.ours" | grep -v 'zero bytes of padding' | sed -E 's/^  0*([0-9a-f]+):.*/\1/' | sort -u > "$img.ours.addr"
        grep -E '^ *[0-9a-f]+:' "$img.llvm" | sed -E 's/^ *([0-9a-f]+):.*/\1/' | sed -E 's/^0*//' | sort -u > "$img.llvm.addr"
        missing="$(comm -23 "$img.ours.addr" "$img.llvm.addr" | wc -l)"
        if [ "$missing" -gt 0 ]; then
          echo "  FAIL  $base $t: $missing instruction starts LLVM does not have (first: $(comm -23 "$img.ours.addr" "$img.llvm.addr" | head -3 | tr '\n' ' '))"
          fail=1
        else
          echo "  ok    $base $t ($(wc -l < "$img.ours.addr") instructions agree)"
        fi ;;
      *)
        "$OBJDUMP" -d --no-show-raw-insn "$img" > "$img.llvm" 2>&1 || true
        unknown="$(grep -cE ' \.word 0x' "$img.ours")"
        llvm_unknown="$(grep -cE '<unknown>|\.word|\.inst' "$img.llvm")"
        if [ "$unknown" -gt "$llvm_unknown" ]; then
          echo "  FAIL  $base $t: $unknown words we do not decode, LLVM $llvm_unknown (grep '.word' $img.ours)"
          fail=1
        else
          echo "  ok    $base $t ($unknown undecoded words, LLVM $llvm_unknown)"
        fi ;;
    esac
  done
done
exit "$fail"
