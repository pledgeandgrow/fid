#!/usr/bin/env bash
# SP16: p_zero_words with a constant count becomes word stores on the
# machine layer (compiler/nat_ir.fd ng_zero_words_inline).
#   bash tests/nattools/machine_zero_words_check.sh [fid]   (default release/fid.exe)
# 1. tests/bootstrap/test_machine_zero_words.fd built natively (--nat on
#    Windows, x86_64-linux elsewhere on an x86-64 host) prints its expected
#    lines, with the inline on and with FID_NAT_OFF=zerowords.
# 2. main's machine IR (FID_NAT_DUMP=main) has fewer calls with the inline
#    on than with FID_NAT_OFF=zerowords: the calls to p_zero_words are gone.
# 3. tests/bootstrap/test_midend_sra_inline_payload.fd (an enum literal's
#    inline payload, the mid-end's SRA bug that stopped the natively built
#    parser) prints its expected lines natively, and through pe_ir on
#    Windows.
# 4. On Windows with WSL, both programs built for x86_64-linux run there too.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-zw"
mkdir -p "$OUT"
cd "$(dirname "$0")/../.." || exit 1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
if [ "$WIN" = 1 ]; then
    TGT=(--nat); EXT=.exe
else
    TGT=(--target x86_64-linux); EXT=""
    [ "$(uname -m)" = x86_64 ] || { echo "skip: native images need x86-64 here"; exit 0; }
fi

ZW=tests/bootstrap/test_machine_zero_words.fd
SRA=tests/bootstrap/test_midend_sra_inline_payload.fd

run_cmp() {  # label image test
    local label="$1" img="$2" t="$3"
    if "$img" > "$img.txt" 2>&1 && diff <(expected "$t") <(tr -d '\r' < "$img.txt") > /dev/null; then
        ok "$label"
    else
        bad "$label: $(tr -d '\r' < "$img.txt" | head -c 300)"
    fi
}

# 1. output, inline on and off
if "$FID" emit "$ZW" "${TGT[@]}" -o "$OUT/zw$EXT" > "$OUT/zw.log" 2>&1; then
    run_cmp "zero_words: expected lines (inline on)" "$OUT/zw$EXT" "$ZW"
else
    bad "zero_words does not build: $(tail -3 "$OUT/zw.log")"
fi
if FID_NAT_OFF=zerowords "$FID" emit "$ZW" "${TGT[@]}" -o "$OUT/zw_off$EXT" > "$OUT/zw_off.log" 2>&1; then
    run_cmp "zero_words: expected lines (FID_NAT_OFF=zerowords)" "$OUT/zw_off$EXT" "$ZW"
else
    bad "zero_words (zerowords off) does not build: $(tail -3 "$OUT/zw_off.log")"
fi

# 2. the calls are gone from main's machine IR
FID_NAT_DUMP=main "$FID" emit "$ZW" "${TGT[@]}" -o "$OUT/zw_d$EXT" > "$OUT/zw_on.mir" 2>&1
FID_NAT_DUMP=main FID_NAT_OFF=zerowords "$FID" emit "$ZW" "${TGT[@]}" -o "$OUT/zw_d2$EXT" > "$OUT/zw_off.mir" 2>&1
con=$(grep -c '^\[mir\] [0-9]*: call ' "$OUT/zw_on.mir")
coff=$(grep -c '^\[mir\] [0-9]*: call ' "$OUT/zw_off.mir")
if [ "$coff" -gt 0 ] && [ "$con" -lt "$coff" ]; then
    ok "main: $con calls with the inline, $coff with FID_NAT_OFF=zerowords"
else
    bad "main: $con calls with the inline, $coff without (expected fewer with it)"
fi

# 3. the SRA case
if "$FID" emit "$SRA" "${TGT[@]}" -o "$OUT/sra$EXT" > "$OUT/sra.log" 2>&1; then
    run_cmp "sra inline payload (machine layer)" "$OUT/sra$EXT" "$SRA"
else
    bad "sra test does not build natively: $(tail -3 "$OUT/sra.log")"
fi
if [ "$WIN" = 1 ]; then
    if "$FID" emit "$SRA" -o "$OUT/sra_pe.exe" > "$OUT/sra_pe.log" 2>&1; then
        run_cmp "sra inline payload (pe_ir)" "$OUT/sra_pe.exe" "$SRA"
    else
        bad "sra test does not build with pe_ir: $(tail -3 "$OUT/sra_pe.log")"
    fi
fi

# 4. the Linux images through WSL
if [ "$WIN" = 1 ] && command -v wsl.exe > /dev/null 2>&1; then
    for t in "$ZW" "$SRA"; do
        b=$(basename "$t" .fd)
        if "$FID" emit "$t" --target x86_64-linux -o "$OUT/${b}_linux" > "$OUT/${b}_linux.log" 2>&1; then
            p=$(MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -w "$OUT/${b}_linux")" 2> /dev/null | tr -d '\r')
            if MSYS_NO_PATHCONV=1 wsl.exe -e "$p" > "$OUT/${b}_linux.txt" 2>&1 && diff <(expected "$t") <(tr -d '\r' < "$OUT/${b}_linux.txt") > /dev/null; then
                ok "$b (x86_64-linux, WSL)"
            else
                bad "$b (x86_64-linux): $(tr -d '\r' < "$OUT/${b}_linux.txt" | head -c 200)"
            fi
        else
            bad "$b does not build for x86_64-linux: $(tail -3 "$OUT/${b}_linux.log")"
        fi
    done
fi

echo "machine_zero_words_check: $fails failure(s)"
[ $fails = 0 ]
