#!/usr/bin/env bash
# disasm-forms-check.sh — `fid disasm --raw` on byte sequences, for rows no
# suite program reaches (docs/roadmap/bugs-roadmap.md):
#   BG233 and BG208 (x86_64): the 0x66 operand-size prefix, REX.B and F3 on
#     the one-byte forms the decoder names itself, and VEX 0F 77, which has
#     no ModRM byte (the decoder took the next byte for one);
#   BG232 (x86_64, aarch64, riscv64): instructions the encoders emit that
#     had no row. Their bytes are llvm-mc's (clang 21) for the text expected.
#
#   FID=release/fid.exe bash tests/tooling/disasm-forms-check.sh
#   OUT=<dir>   default build/disasm-forms-check
# Exit status 1 on a difference.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/disasm-forms-check}"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

# check <name> <bytes as hex pairs> <the listing, `|` between its lines>
check() {
    local name="$1" hex="$2" want="$3" arch="${4:-x86_64}" bin="$OUT/$1.bin" got esc
    esc=$(printf '%s' "$hex" | tr -d ' ' | sed -E 's/(..)/\\x\1/g')
    printf '%b' "$esc" > "$bin"
    got=$("$FID" disasm "$bin" --raw --arch "$arch" 2>/dev/null | tr -d '\r' \
          | sed -E 's/^ *[0-9a-fA-F]+: +//; s/  +/ /g' \
          | grep -v -E '^\([0-9]+ zero bytes of padding\)$' | tr '\n' '|' | sed 's/|$//')
    if [ "$got" = "$want" ]; then
        ok "$name"
    else
        bad "$name: got [$got], want [$want]"
    fi
}

# BG233
check bg233-operand-size-prefix "66 b8 34 12 c3" \
      "66 b8 34 12 mov ax, 0x1234|c3 ret"
check bg233-cwd "66 99 c3" "66 99 cwd|c3 ret"
check bg233-rex-b "49 90 c3" "49 90 xchg r8, rax|c3 ret"
check bg233-pause "f3 90 c3" "f3 90 pause|c3 ret"
# what these were before is unchanged
check bg233-as-before "b8 34 12 00 00 99 48 99 90 48 b8 01 00 00 00 00 00 00 00" \
      "b8 34 12 00 00 mov eax, 0x1234|99 cdq|48 99 cqo|90 nop|48 b8 01 00 00 00 00 00 00 00 mov rax, 0x1"
# BG208: VEX 0F 77 has no ModRM byte
check bg208-vzeroupper "c5 f8 77 4d 39 f4 c3" \
      "c5 f8 77 vzeroupper|4d 39 f4 cmp r12, r14|c3 ret"
check bg208-vzeroall "c5 fc 77 48 c7 c0 01 00 00 00 c3" \
      "c5 fc 77 vzeroall|48 c7 c0 01 00 00 00 mov rax, 0x1|c3 ret"

# BG232: rows for instructions the encoders emit (bytes by llvm-mc)
check bg232-blsmsk "c4 e2 f8 f3 d1 c3" "c4 e2 f8 f3 d1 blsmsk rax, rcx|c3 ret"
check bg232-blsi "c4 e2 f8 f3 d9 c3" "c4 e2 f8 f3 d9 blsi rax, rcx|c3 ret"
# AArch64: one register with writeback (the entry's own word first)
check bg232-str-post "28 85 00 f8" "28 85 00 f8 str x8, [x9], #8" aarch64
check bg232-ldr-pre "20 8c 40 f8" "20 8c 40 f8 ldr x0, [x1, #8]!" aarch64
check bg232-ldrsb-post "20 c4 9f 38" "20 c4 9f 38 ldrsb x0, [x1], #-4" aarch64
check bg232-ldrsh-pre "20 cc 9f 78" "20 cc 9f 78 ldrsh x0, [x1, #-4]!" aarch64
check bg232-ldrsw-post "20 44 80 b8" "20 44 80 b8 ldrsw x0, [x1], #4" aarch64
check bg232-strb-post "20 14 00 38" "20 14 00 38 strb w0, [x1], #1" aarch64
check bg232-strh-pre "20 ec 1f 78" "20 ec 1f 78 strh w0, [x1, #-2]!" aarch64
# AArch64: SIMD&FP loads, stores and pairs
check bg232-ldr-d "20 04 40 fd" "20 04 40 fd ldr d0, [x1, #8]" aarch64
check bg232-str-d "20 08 00 fd" "20 08 00 fd str d0, [x1, #16]" aarch64
check bg232-ldr-s "20 04 40 bd" "20 04 40 bd ldr s0, [x1, #4]" aarch64
check bg232-stp-d "e8 27 01 6d" "e8 27 01 6d stp d8, d9, [sp, #16]" aarch64
check bg232-ldp-d "e8 27 41 6d" "e8 27 41 6d ldp d8, d9, [sp, #16]" aarch64
# AArch64: vector
check bg232-bic-16b "20 1c 62 4e" "20 1c 62 4e bic v0.16b, v1.16b, v2.16b" aarch64
check bg232-dup-2d "20 04 18 4e" "20 04 18 4e dup v0.2d, v1.d[1]" aarch64
check bg232-mov-elem "20 04 18 5e" "20 04 18 5e mov d0, v1.d[1]" aarch64
# RISC-V: vector
check bg232-vsll-vi "d7 b0 21 96" "d7 b0 21 96 vsll.vi v1, v2, 3" riscv64
check bg232-vmseq-vi "d7 30 20 62" "d7 30 20 62 vmseq.vi v1, v2, 0" riscv64
check bg232-vmerge-vim "d7 b0 22 5c" "d7 b0 22 5c vmerge.vim v1, v2, 5, v0" riscv64
check bg232-vslidedown-vi "d7 b0 20 3e" "d7 b0 20 3e vslidedown.vi v1, v2, 1" riscv64
check bg232-vfmv-f-s "d7 10 20 42" "d7 10 20 42 vfmv.f.s f1, v2" riscv64
check bg232-vfmv-v-f "d7 50 01 5e" "d7 50 01 5e vfmv.v.f v1, f2" riscv64

if [ "$fails" -gt 0 ]; then
    echo "disasm-forms-check: $fails failed"
    exit 1
fi
echo "disasm-forms-check: all passed"
