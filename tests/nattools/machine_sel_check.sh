#!/usr/bin/env bash
# Checks of the machine-layer goals MI3 / OP20 (docs/roadmap/llvm-roadmap.md,
# docs/roadmap/competitive-roadmap.md), OP23 and L2B (docs/roadmap/roadmap.md),
# run after the compiler is built:
#   bash tests/nattools/machine_sel_check.sh [fid]   (default release/fid.exe)
# - tests/bootstrap/test_machine_bits.fd built for each Linux target with
#   FID_VERIFY=1, with and without the new selections; x86-64 runs (WSL on
#   Windows) and must print its expected output every way.
# - the selected instructions are in the image when on, absent when off:
#   x86-64 --cpu x86-64-v3: ANDN and BLSR (VEX 0F38 F2 / F3 /1);
#   AArch64: MADD (Ra != xzr) and BIC; RISC-V --cpu rva22u64: Zbb ANDN.
# - tests/bootstrap/test_runtime_memops.fd (OP23) on x86-64 Linux.
# - tests/nattools/macho_fat_unit.fd (L2B's fat writer) through `fid run`,
#   and, when the driver accepts it, `fid emit --target universal`
#   (header checked by tests/nattools/macho_fat_check.py).
# - tests/nattools/enc-oracle-mi3.py when zig is on PATH.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-sel"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# count <file> <kind>: how many of the instruction kind are in the image
count() {
    python3 - "$1" "$2" <<'EOF'
import sys
import struct
data = open(sys.argv[1], 'rb').read()
kind = sys.argv[2]
# only the executable segments of the ELF (data words could look like code)
if data[:4] == b'\x7fELF':
    phoff = struct.unpack_from('<Q', data, 0x20)[0]
    phsz, phn = struct.unpack_from('<HH', data, 0x36)
    code = b''
    for k in range(phn):
        p_type, p_flags, p_off = struct.unpack_from('<IIQ', data, phoff + k * phsz)
        p_filesz = struct.unpack_from('<Q', data, phoff + k * phsz + 0x20)[0]
        if p_type == 1 and (p_flags & 1):
            code += b'\0' * ((-len(code)) % 4) + data[p_off:p_off + p_filesz]
    data = code
n = 0
if kind in ('andn', 'blsr'):
    for i in range(len(data) - 4):
        if data[i] != 0xC4 or (data[i + 1] & 0x1F) != 2 or (data[i + 2] & 0x87) != 0x80:
            continue
        if kind == 'andn' and data[i + 3] == 0xF2 and (data[i + 4] & 0xC0) == 0xC0:
            n += 1
        if kind == 'blsr' and data[i + 3] == 0xF3 and (data[i + 4] & 0xF8) == 0xC8:
            n += 1
else:
    for i in range(0, len(data) - 3, 4):
        w = int.from_bytes(data[i:i + 4], 'little')
        if kind == 'madd' and (w & 0xFFE08000) == 0x9B000000 and ((w >> 10) & 31) != 31:
            n += 1
        if kind == 'bic' and (w & 0xFF200000) == 0x8A200000:
            n += 1
        if kind == 'rvandn' and (w & 0xFE00707F) == 0x40007033:
            n += 1
print(n)
EOF
}

# build <file> <target> <name> [extra emit args...] -> $OUT/<name>
build() {
    local f="$1" t="$2" name="$3"
    shift 3
    if ! FID_VERIFY=1 "$FID" emit "$f" --target "$t" "$@" -o "$OUT/$name" >"$OUT/$name.log" 2>&1; then
        bad "$name: emit failed: $(tail -2 "$OUT/$name.log")"
        return 1
    fi
    return 0
}

runs_right() {  # exe file label
    local got
    got=$($RUN "$1" 2>&1 | tr -d '\r')
    if [ "$got" = "$(expected "$2")" ]; then ok "$3 output"; else bad "$3: output differs"; echo "$got" | head -5; fi
}

BITS=tests/bootstrap/test_machine_bits.fd

# x86-64: BMI1 under --cpu x86-64-v3 only
if build $BITS x86_64-linux bits-x64-v3 --cpu x86-64-v3; then
    runs_right "$OUT/bits-x64-v3" $BITS "x86-64 v3"
    a=$(count "$OUT/bits-x64-v3" andn); b=$(count "$OUT/bits-x64-v3" blsr)
    if [ "$a" -gt 0 ] && [ "$b" -gt 0 ]; then ok "x86-64 v3: $a andn, $b blsr"; else bad "x86-64 v3: andn $a, blsr $b"; fi
fi
if build $BITS x86_64-linux bits-x64; then
    runs_right "$OUT/bits-x64" $BITS "x86-64 baseline"
    a=$(count "$OUT/bits-x64" andn); b=$(count "$OUT/bits-x64" blsr)
    if [ "$a" -eq 0 ] && [ "$b" -eq 0 ]; then ok "x86-64 baseline: no BMI1"; else bad "x86-64 baseline: andn $a, blsr $b"; fi
fi
if FID_NAT_OFF=andn,blsr build $BITS x86_64-linux bits-x64-v3-off --cpu x86-64-v3; then
    runs_right "$OUT/bits-x64-v3-off" $BITS "x86-64 v3, andn/blsr off"
    a=$(count "$OUT/bits-x64-v3-off" andn); b=$(count "$OUT/bits-x64-v3-off" blsr)
    if [ "$a" -eq 0 ] && [ "$b" -eq 0 ]; then ok "FID_NAT_OFF=andn,blsr"; else bad "FID_NAT_OFF=andn,blsr: andn $a, blsr $b"; fi
fi

# AArch64: MADD/MSUB and BIC
if build $BITS aarch64-linux bits-a64; then
    m=$(count "$OUT/bits-a64" madd); c=$(count "$OUT/bits-a64" bic)
    if [ "$m" -gt 0 ] && [ "$c" -gt 0 ]; then ok "AArch64: $m madd, $c bic"; else bad "AArch64: madd $m, bic $c"; fi
fi
if FID_NAT_OFF=madd,andn build $BITS aarch64-linux bits-a64-off; then
    m=$(count "$OUT/bits-a64-off" madd); c=$(count "$OUT/bits-a64-off" bic)
    if [ "$m" -eq 0 ] && [ "$c" -eq 0 ]; then ok "FID_NAT_OFF=madd,andn (AArch64)"; else bad "AArch64 off: madd $m, bic $c"; fi
fi

# RISC-V: Zbb ANDN under --cpu rva22u64
if build $BITS riscv64-linux bits-rv-zb --cpu rva22u64; then
    r=$(count "$OUT/bits-rv-zb" rvandn)
    if [ "$r" -gt 0 ]; then ok "RISC-V rva22u64: $r andn"; else bad "RISC-V rva22u64: no andn"; fi
fi
if build $BITS riscv64-linux bits-rv; then
    r=$(count "$OUT/bits-rv" rvandn)
    if [ "$r" -eq 0 ]; then ok "RISC-V rv64gc: no andn"; else bad "RISC-V rv64gc: $r andn"; fi
fi

# OP23: the runtime's word-wide copy / compare / search / fill
if build tests/bootstrap/test_runtime_memops.fd x86_64-linux memops-x64; then
    runs_right "$OUT/memops-x64" tests/bootstrap/test_runtime_memops.fd "OP23 memops x86-64"
fi
build tests/bootstrap/test_runtime_memops.fd aarch64-linux memops-a64 && ok "OP23 memops AArch64 (built with FID_VERIFY=1)"
build tests/bootstrap/test_runtime_memops.fd riscv64-linux memops-rv && ok "OP23 memops RISC-V (built with FID_VERIFY=1)"

# L2B: the fat writer, then the driver's --target universal if it has it
got=$("$FID" run tests/nattools/macho_fat_unit.fd 2>/dev/null | tr -d '\r' | grep -v '^\[')
if [ "$got" = "$(expected tests/nattools/macho_fat_unit.fd)" ]; then ok "L2B nat_macho_fat"; else bad "L2B nat_macho_fat"; echo "$got" | head -5; fi
if "$FID" emit tests/bootstrap/hello.fd --target universal -o "$OUT/hello-universal" >"$OUT/universal.log" 2>&1; then
    if python3 tests/nattools/macho_fat_check.py "$OUT/hello-universal"; then ok "L2B --target universal"; else bad "L2B --target universal: header"; fi
else
    echo "skip L2B --target universal: the driver does not take it yet ($(tail -1 "$OUT/universal.log"))"
fi

if command -v zig >/dev/null 2>&1; then
    if python3 tests/nattools/enc-oracle-mi3.py --fid "$FID" >"$OUT/oracle.log" 2>&1; then ok "enc-oracle-mi3"; else bad "enc-oracle-mi3: $(tail -3 "$OUT/oracle.log")"; fi
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
