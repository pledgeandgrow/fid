#!/bin/bash
# pe_cpu_check.sh [FID]: competitive OP20 on the default Windows backend
# (pe_ir) and the C path, on a Windows host.
#
#   1. `fid emit --cpu bogus` and `fid ccompile --cpu bogus` fail with
#      nat_cpu_check's "Unknown --cpu 'bogus'" message.
#   2. tests/bootstrap/test_pe_cpu_bits.fd built with `fid emit` (the safe
#      default), `--cpu baseline` and `--cpu x86-64-v3` prints its Expected
#      output block each time.
#   3. The x86-64-v3 image has SHLX/SARX/SHRX and BLSR (VEX C4 E2 ..
#      F7 / F3 /1); the default image has fewer of each (the runtime may
#      hold none; data may look like one by chance, hence "fewer").
#   4. `fid ccompile --cpu x86-64-v3` builds and prints the same (the C
#      compiler got -march=x86-64-v3; with --trace the command shows it).
#
#   bash tests/nattools/pe_cpu_check.sh release/fid.exe
set -u
FID="${1:-release/fid.exe}"
export FIDES_NO_CACHE=1
SRC=tests/bootstrap/test_pe_cpu_bits.fd
OUT=$(mktemp -d)
fail=0

want_of() {
    awk '/^\/\/ Expected output/{f=1;next} f&&/^\/\/ /{sub(/^\/\/ /,"");print;next} f{exit}' "$1"
}
want_of "$SRC" > "$OUT/want"

# 1. unknown names
for cmd in emit ccompile; do
    "$FID" $cmd "$SRC" --cpu bogus -o "$OUT/bogus_$cmd.exe" > "$OUT/bogus_$cmd.log" 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then
        echo "  FAIL  $cmd --cpu bogus: accepted"
        fail=$((fail + 1))
    elif ! grep -q "Unknown --cpu 'bogus'" "$OUT/bogus_$cmd.log"; then
        echo "  FAIL  $cmd --cpu bogus: no \"Unknown --cpu\" message (see $OUT/bogus_$cmd.log)"
        fail=$((fail + 1))
    else
        echo "  ok    $cmd --cpu bogus: $(grep -m1 "Unknown --cpu" "$OUT/bogus_$cmd.log")"
    fi
done

# 2. builds and runs
build_run() {
    local name="$1"
    shift
    if ! "$FID" emit "$SRC" "$@" -o "$OUT/$name.exe" > "$OUT/$name.log" 2>&1; then
        echo "  FAIL  emit $name: does not build (see $OUT/$name.log)"
        fail=$((fail + 1))
        return 1
    fi
    "$OUT/$name.exe" > "$OUT/$name.got" 2>&1
    if diff -q --strip-trailing-cr "$OUT/want" "$OUT/$name.got" > /dev/null; then
        echo "  ok    emit $name"
    else
        echo "  FAIL  emit $name: output differs (see $OUT/$name.got)"
        fail=$((fail + 1))
    fi
}
build_run default
build_run baseline --cpu baseline
build_run v3 --cpu x86-64-v3
build_run v4 --cpu x86-64-v4

# 3. the forms in the images
count_vex() {
    # $1 image, $2 opcode (F7 shifts, F3 BLSR): C4, byte1 with map 0F38
    # (low five bits 00010), byte2 with W=1 and L=0, then the opcode; for
    # F3 ModRM.reg must be 1 (BLSR) and mod 11
    python3 - "$1" "$2" <<'EOF'
import sys
b = open(sys.argv[1], 'rb').read()
op = int(sys.argv[2], 16)
n = 0
for i in range(len(b) - 4):
    if b[i] != 0xC4 or (b[i+1] & 0x1F) != 0x02 or (b[i+2] & 0x84) != 0x80 or b[i+3] != op:
        continue
    m = b[i+4]
    if (m >> 6) != 3:
        continue
    if op == 0xF3 and ((m >> 3) & 7) != 1:
        continue
    if op == 0xF7 and (b[i+2] & 3) == 0:
        continue
    n += 1
print(n)
EOF
}
if [ -f "$OUT/default.exe" ] && [ -f "$OUT/v3.exe" ]; then
    d_sh=$(count_vex "$OUT/default.exe" F7)
    v_sh=$(count_vex "$OUT/v3.exe" F7)
    d_bl=$(count_vex "$OUT/default.exe" F3)
    v_bl=$(count_vex "$OUT/v3.exe" F3)
    if [ "$v_sh" -gt "$d_sh" ] && [ "$v_bl" -gt "$d_bl" ]; then
        echo "  ok    forms: shlx/sarx/shrx $d_sh -> $v_sh, blsr $d_bl -> $v_bl"
    else
        echo "  FAIL  forms: shlx/sarx/shrx $d_sh -> $v_sh, blsr $d_bl -> $v_bl (want more with --cpu x86-64-v3)"
        fail=$((fail + 1))
    fi
    if cmp -s "$OUT/default.exe" "$OUT/baseline.exe"; then
        echo "  ok    --cpu baseline gives the default image"
    else
        echo "  FAIL  --cpu baseline differs from the default image"
        fail=$((fail + 1))
    fi
fi

# 4. the C path
if "$FID" ccompile "$SRC" --cpu x86-64-v3 --trace -o "$OUT/c_v3.exe" > "$OUT/c_v3.log" 2>&1; then
    "$OUT/c_v3.exe" > "$OUT/c_v3.got" 2>&1
    if diff -q --strip-trailing-cr "$OUT/want" "$OUT/c_v3.got" > /dev/null; then
        echo "  ok    ccompile --cpu x86-64-v3"
    else
        echo "  FAIL  ccompile --cpu x86-64-v3: output differs (see $OUT/c_v3.got)"
        fail=$((fail + 1))
    fi
    if grep -q -- "-march=x86-64-v3\|/arch:AVX2" "$OUT/c_v3.log"; then
        echo "  ok    ccompile passed the CPU to the C compiler"
    else
        echo "  note  the C compiler's command line was not printed with -march (see $OUT/c_v3.log)"
    fi
else
    echo "  FAIL  ccompile --cpu x86-64-v3: does not build (see $OUT/c_v3.log)"
    fail=$((fail + 1))
fi

echo "pe_cpu_check: $fail failed (files in $OUT)"
[ $fail -eq 0 ]
