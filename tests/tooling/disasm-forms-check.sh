#!/usr/bin/env bash
# disasm-forms-check.sh — `fid disasm --raw --arch x86_64` on byte sequences
# the compiler does not emit, so no image reaches them (BG233 and BG208,
# docs/roadmap/bugs-roadmap.md): the 0x66 operand-size prefix, REX.B and F3
# on the one-byte forms the decoder names itself, and VEX 0F 77, which has
# no ModRM byte (the decoder took the next byte for one).
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
    local name="$1" hex="$2" want="$3" bin="$OUT/$1.bin" got esc
    esc=$(printf '%s' "$hex" | tr -d ' ' | sed -E 's/(..)/\\x\1/g')
    printf '%b' "$esc" > "$bin"
    got=$("$FID" disasm "$bin" --raw --arch x86_64 2>/dev/null | tr -d '\r' \
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

if [ "$fails" -gt 0 ]; then
    echo "disasm-forms-check: $fails failed"
    exit 1
fi
echo "disasm-forms-check: all passed"
