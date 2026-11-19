#!/usr/bin/env bash
# TG7 / TG8 (docs/roadmap/llvm-roadmap.md): the x86-64 and AArch64
# selections of compiler/nat_ir.fd added for them, run after the compiler
# is built:
#   bash tests/nattools/machine_tg78_check.sh [fid] [zig]
#   (defaults release/fid.exe, zig)
# - tests/nattools/tg78_unit.fd: the selections on small cases print "ok"
#   for every target.
# - tests/bootstrap/test_machine_tg7.fd (x86_64-linux, FID_VERIFY=1):
#   --cpu x86-64-v3 has BLSMSK and BLSI in the image, the baseline and
#   FID_NAT_OFF=blsmsk,blsi none; `cmp r, [rsp + d]` and `op [rsp + d], r|imm`
#   appear more often than with FID_NAT_OFF=cmpslot,rmwslot. Every image
#   prints the expected lines (WSL on Windows), and so does `--nat` (a
#   Windows host); FID_NAT_MUTATE=cmpslot, rmwslot and blsx (each rule
#   made wrong on purpose) change the output.
# - tests/bootstrap/test_machine_tg8.fd (aarch64-linux, FID_VERIFY=1, run
#   under qemu-aarch64): loads and stores with writeback in the image, none
#   with FID_NAT_OFF=wbidx; the expected lines both ways;
#   FID_NAT_MUTATE=wbidx changes the output. x86_64-linux and riscv64-linux
#   print the same lines.
# - tests/nattools/enc-oracle-tg78.py --only: the encoders against clang.
# A line "skip" (no WSL, no qemu, no zig) is not a failure. Exit 1 on a
# failure.
set -u
FID="${1:-release/fid.exe}"
ZIG="${2:-zig}"
OUT="${TMPDIR:-/tmp}/fid-machine-tg78"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
skip() { echo "skip $1"; }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d' | tr -d '\r'; }

wslpath_of() {
    local p d
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    d=$(echo "$p" | cut -c2 | tr 'A-Z' 'a-z')
    echo "/mnt/$d/$(echo "$p" | cut -c4-)"
}

# qemu_of <target>: the qemu-user program its images run under ("" none)
qemu_of() {
    case "$1" in
        aarch64-linux) echo qemu-aarch64 ;;
        riscv64-linux) echo qemu-riscv64 ;;
        *) echo "" ;;
    esac
}

# can_run <target>: images of the target run on this host
can_run() {
    local q
    q=$(qemu_of "$1")
    if [ "$1" = windows ]; then [ "$HOST" = windows ]; return; fi
    if [ -n "$WSL" ]; then return 0; fi
    if [ "$HOST" = linux ]; then
        if [ -n "$q" ]; then command -v "$q" >/dev/null 2>&1; return; fi
        return 0
    fi
    return 1
}

# run_img <image> <target>: the image's output
run_img() {
    local q
    q=$(qemu_of "$2")
    if [ "$2" = windows ]; then
        "$1" 2>&1
    elif [ -n "$WSL" ]; then
        MSYS_NO_PATHCONV=1 $WSL $q "$(wslpath_of "$1")" 2>&1
    elif [ -n "$q" ]; then
        "$q" "$1" 2>&1
    else
        "$1" 2>&1
    fi
}

# build <file> <target> <name> [emit args...] -> $OUT/<name>
build() {
    local f="$1" t="$2" name="$3"
    shift 3
    local targ="--target $t"
    if [ "$t" = windows ]; then targ="--nat"; fi
    if ! FID_VERIFY=1 "$FID" emit "$f" $targ "$@" -o "$OUT/$name" >"$OUT/$name.log" 2>&1; then
        bad "$name: emit failed: $(tail -2 "$OUT/$name.log" | tr '\n' ' ')"
        return 1
    fi
    return 0
}

# check_runs <image> <target> <test> <label>
check_runs() {
    local got
    if ! can_run "$2"; then
        skip "$4: cannot run $2 here"
        return
    fi
    got=$(run_img "$1" "$2" | tr -d '\r')
    if [ "$got" = "$(expected "$3")" ]; then ok "$4 output"; else bad "$4: output differs"; echo "$got" | head -12; fi
}

# check_mutant <image> <target> <test> <label>: the output must differ
check_mutant() {
    local got
    if ! can_run "$2"; then
        skip "$4: cannot run $2 here"
        return
    fi
    got=$(run_img "$1" "$2" | tr -d '\r')
    if [ "$got" != "$(expected "$3")" ]; then ok "$4 changes the output"; else bad "$4: the wrong rule went unnoticed"; fi
}

# count <image> <kind>: instructions of that kind in the executable segments
count() {
    "$PY" - "$1" "$2" <<'EOF'
import struct
import sys
data = open(sys.argv[1], 'rb').read()
kind = sys.argv[2]
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
if kind in ('blsmsk', 'blsi'):
    want = 0xD0 if kind == 'blsmsk' else 0xD8
    for i in range(len(data) - 4):
        if data[i] == 0xC4 and (data[i + 1] & 0x1F) == 2 and (data[i + 2] & 0x87) == 0x80 \
                and data[i + 3] == 0xF3 and (data[i + 4] & 0xF8) == want:
            n += 1
elif kind in ('cmpslot', 'rmwslot'):
    for i in range(len(data) - 4):
        rex, op, modrm, sib = data[i], data[i + 1], data[i + 2], data[i + 3]
        if rex not in (0x48, 0x4C) or (modrm & 7) != 4 or (modrm >> 6) not in (1, 2) or sib != 0x24:
            continue
        if kind == 'cmpslot' and op == 0x3B:
            n += 1
        if kind == 'cmpslot' and op == 0x83 and ((modrm >> 3) & 7) == 7:
            n += 1
        if kind == 'rmwslot' and op in (0x01, 0x29, 0x21, 0x09, 0x31):
            n += 1
        if kind == 'rmwslot' and op in (0x83, 0x81) and ((modrm >> 3) & 7) in (0, 1, 4, 5, 6):
            n += 1
elif kind == 'wb':
    for i in range(0, len(data) - 3, 4):
        w = int.from_bytes(data[i:i + 4], 'little')
        if (w & 0x3F200C00) in (0x38000400, 0x38000C00) and ((w >> 5) & 31) != 31:
            n += 1
print(n)
EOF
}

# --- the unit checks -------------------------------------------------------
u=$("$FID" run tests/nattools/tg78_unit.fd 2>&1 | grep -v '^\[' | tr -d '\r')
if [ "$u" = "$(printf 'a64 ok\nrv64 ok\nx64 ok')" ]; then ok "tg78_unit: every target"; else bad "tg78_unit: $(echo "$u" | tr '\n' ' ')"; fi

# --- TG7 on x86-64 -------------------------------------------------------------
T7=tests/bootstrap/test_machine_tg7.fd
if build $T7 x86_64-linux tg7-v3 --cpu x86-64-v3; then
    check_runs "$OUT/tg7-v3" x86_64-linux $T7 "TG7 x86-64 v3"
    a=$(count "$OUT/tg7-v3" blsmsk); b=$(count "$OUT/tg7-v3" blsi)
    if [ "$a" -gt 0 ] && [ "$b" -gt 0 ]; then ok "x86-64 v3: $a blsmsk, $b blsi"; else bad "x86-64 v3: blsmsk $a, blsi $b"; fi
fi
if build $T7 x86_64-linux tg7; then
    check_runs "$OUT/tg7" x86_64-linux $T7 "TG7 x86-64"
    a=$(count "$OUT/tg7" blsmsk); b=$(count "$OUT/tg7" blsi)
    if [ "$a" -eq 0 ] && [ "$b" -eq 0 ]; then ok "x86-64 baseline: no BMI1"; else bad "x86-64 baseline: blsmsk $a, blsi $b"; fi
    c_on=$(count "$OUT/tg7" cmpslot); r_on=$(count "$OUT/tg7" rmwslot)
    if FID_NAT_OFF=cmpslot,rmwslot build $T7 x86_64-linux tg7-off; then
        check_runs "$OUT/tg7-off" x86_64-linux $T7 "TG7 x86-64, cmpslot/rmwslot off"
        c_off=$(count "$OUT/tg7-off" cmpslot); r_off=$(count "$OUT/tg7-off" rmwslot)
        if [ "$c_on" -gt "$c_off" ]; then ok "cmpslot: $c_on slot compares ($c_off off)"; else bad "cmpslot: $c_on on, $c_off off"; fi
        if [ "$r_on" -gt "$r_off" ]; then ok "rmwslot: $r_on slot read-modify-writes ($r_off off)"; else bad "rmwslot: $r_on on, $r_off off"; fi
    fi
fi
if FID_NAT_OFF=blsmsk,blsi build $T7 x86_64-linux tg7-v3-off --cpu x86-64-v3; then
    check_runs "$OUT/tg7-v3-off" x86_64-linux $T7 "TG7 x86-64 v3, blsmsk/blsi off"
    a=$(count "$OUT/tg7-v3-off" blsmsk); b=$(count "$OUT/tg7-v3-off" blsi)
    if [ "$a" -eq 0 ] && [ "$b" -eq 0 ]; then ok "FID_NAT_OFF=blsmsk,blsi"; else bad "FID_NAT_OFF=blsmsk,blsi: blsmsk $a, blsi $b"; fi
fi
for mu in cmpslot rmwslot; do
    if FID_NAT_MUTATE=$mu build $T7 x86_64-linux tg7-mut-$mu; then
        check_mutant "$OUT/tg7-mut-$mu" x86_64-linux $T7 "FID_NAT_MUTATE=$mu"
    fi
done
if FID_NAT_MUTATE=blsx build $T7 x86_64-linux tg7-mut-blsx --cpu x86-64-v3; then
    check_mutant "$OUT/tg7-mut-blsx" x86_64-linux $T7 "FID_NAT_MUTATE=blsx"
fi
if [ "$HOST" = windows ]; then
    if build $T7 windows tg7-nat.exe; then check_runs "$OUT/tg7-nat.exe" windows $T7 "TG7 --nat"; fi
fi
for t in aarch64-linux riscv64-linux; do
    if build $T7 $t tg7-$t; then check_runs "$OUT/tg7-$t" $t $T7 "TG7 $t"; fi
done

# --- TG8 on AArch64 ------------------------------------------------------------
T8=tests/bootstrap/test_machine_tg8.fd
if build $T8 aarch64-linux tg8-a64; then
    check_runs "$OUT/tg8-a64" aarch64-linux $T8 "TG8 AArch64"
    w_on=$(count "$OUT/tg8-a64" wb)
    if [ "$w_on" -gt 0 ]; then ok "AArch64: $w_on loads / stores with writeback"; else bad "AArch64: no load or store with writeback"; fi
    if FID_NAT_OFF=wbidx build $T8 aarch64-linux tg8-a64-off; then
        check_runs "$OUT/tg8-a64-off" aarch64-linux $T8 "TG8 AArch64, wbidx off"
        w_off=$(count "$OUT/tg8-a64-off" wb)
        if [ "$w_off" -lt "$w_on" ]; then ok "FID_NAT_OFF=wbidx: $w_off with writeback"; else bad "FID_NAT_OFF=wbidx: $w_off with writeback ($w_on on)"; fi
    fi
fi
if FID_NAT_MUTATE=wbidx build $T8 aarch64-linux tg8-a64-mut; then
    check_mutant "$OUT/tg8-a64-mut" aarch64-linux $T8 "FID_NAT_MUTATE=wbidx"
fi
for t in x86_64-linux riscv64-linux; do
    if build $T8 $t tg8-$t; then check_runs "$OUT/tg8-$t" $t $T8 "TG8 $t"; fi
done
if [ "$HOST" = windows ]; then
    if build $T8 windows tg8-nat.exe; then check_runs "$OUT/tg8-nat.exe" windows $T8 "TG8 --nat"; fi
fi

# --- the encoders ----------------------------------------------------------------
if command -v "$ZIG" >/dev/null 2>&1; then
    if "$PY" tests/nattools/enc-oracle-tg78.py --only --fid "$FID" --zig "$ZIG" >"$OUT/oracle.log" 2>&1; then
        ok "enc-oracle-tg78 --only"
    else
        bad "enc-oracle-tg78 --only: $(tail -3 "$OUT/oracle.log" | tr '\n' ' ')"
    fi
else
    skip "enc-oracle-tg78: no $ZIG"
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
