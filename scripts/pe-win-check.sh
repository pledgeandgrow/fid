#!/bin/bash
# pe-win-check.sh [FID]: the Nov 16 work on the default Windows backend
# (compiler/pe_ir.fd), on a Windows host where `fid emit` uses pe_ir.
#
#   LG13  asm! runs in a process: scripts/pe-win/asm_process.fd prints its
#         expected output; tests/bootstrap/test_windows_asm_regs.fd (the
#         templates overwrite callee-saved registers) prints its own.
#   OP13  sibling tail calls: scripts/pe-win/tail_deep.fd (100 million
#         calls in tail position) runs; test_windows_tail_sibling.fd prints
#         its expected output with and without the jumps (FID_PE_OFF=tail).
#   OP15  cold blocks last: test_windows_cold_layout.fd prints the same
#         with and without (FID_PE_OFF=layout), and the code differs.
#   LF20  cleared frames: test_windows_wipe_secret.fd prints the same with
#         and without (FID_PE_OFF=wipe), and the image is larger with it.
#   B5    the per-function cache: two builds with FID_FNCACHE give the
#         same bytes as a build without it, the second from the cache.
#   DB3   a debug build (FID_PE_DEBUG=1) has DWARF line tables
#         (scripts/pe-dwarf-check.py) and prints the same.
#
#   ./scripts/pe-win-check.sh                     # release/fid.exe
#   ./scripts/pe-win-check.sh release/fid-dev.exe
set -u
FID="${1:-release/fid.exe}"
# Builds that differ only in environment switches the build cache does not
# see: never reuse a cached build.
export FIDES_NO_CACHE=1
OUT=$(mktemp -d)
fail=0

want_of() {
    awk '/^\/\/ Expected output/{f=1;next} f&&/^\/\/ /{sub(/^\/\/ /,"");print;next} f&&/^\/\/$/{print "";next} f{exit}' "$1"
}

# build_run <name> <file> [env assignments...]: emit and run; output in $OUT/<name>.got
build_run() {
    local name="$1" file="$2"
    shift 2
    if ! env "$@" "$FID" emit "$file" -o "$OUT/$name.exe" > "$OUT/$name.log" 2>&1; then
        echo "  FAIL  $name: does not build (see $OUT/$name.log)"
        fail=$((fail + 1))
        return 1
    fi
    "$OUT/$name.exe" > "$OUT/$name.got" 2>&1
    return 0
}

# expect <name> <file>: the run's output is the file's Expected output block
expect() {
    want_of "$2" > "$OUT/$1.want"
    if diff -q --strip-trailing-cr "$OUT/$1.want" "$OUT/$1.got" > /dev/null; then
        echo "  ok    $1"
    else
        echo "  FAIL  $1: output differs from the Expected output block (see $OUT/$1.got)"
        fail=$((fail + 1))
    fi
}

# LG13
build_run asm_process scripts/pe-win/asm_process.fd && expect asm_process scripts/pe-win/asm_process.fd
build_run asm_regs tests/bootstrap/test_windows_asm_regs.fd && expect asm_regs tests/bootstrap/test_windows_asm_regs.fd

# OP13
build_run tail_deep scripts/pe-win/tail_deep.fd && expect tail_deep scripts/pe-win/tail_deep.fd
build_run tail tests/bootstrap/test_windows_tail_sibling.fd && expect tail tests/bootstrap/test_windows_tail_sibling.fd
build_run tail_off tests/bootstrap/test_windows_tail_sibling.fd FID_PE_OFF=tail && expect tail_off tests/bootstrap/test_windows_tail_sibling.fd

# OP15
build_run layout tests/bootstrap/test_windows_cold_layout.fd && expect layout tests/bootstrap/test_windows_cold_layout.fd
build_run layout_off tests/bootstrap/test_windows_cold_layout.fd FID_PE_OFF=layout && expect layout_off tests/bootstrap/test_windows_cold_layout.fd
if [ -f "$OUT/layout.exe" ] && [ -f "$OUT/layout_off.exe" ] && cmp -s "$OUT/layout.exe" "$OUT/layout_off.exe"; then
    echo "  FAIL  layout: the image is the same with FID_PE_OFF=layout (no block moved)"
    fail=$((fail + 1))
fi

# LF20
build_run wipe tests/bootstrap/test_windows_wipe_secret.fd && expect wipe tests/bootstrap/test_windows_wipe_secret.fd
build_run wipe_off tests/bootstrap/test_windows_wipe_secret.fd FID_PE_OFF=wipe && expect wipe_off tests/bootstrap/test_windows_wipe_secret.fd
if [ -f "$OUT/wipe.exe" ] && [ -f "$OUT/wipe_off.exe" ] && cmp -s "$OUT/wipe.exe" "$OUT/wipe_off.exe"; then
    echo "  FAIL  wipe: the image is the same with FID_PE_OFF=wipe (no frame cleared)"
    fail=$((fail + 1))
fi

# B5
fc="$OUT/fncache"
build_run fc_none tests/bootstrap/test_windows_cold_layout.fd
build_run fc_cold tests/bootstrap/test_windows_cold_layout.fd FID_FNCACHE="$fc"
build_run fc_warm tests/bootstrap/test_windows_cold_layout.fd FID_FNCACHE="$fc" FID_PE_FNCACHE_STATS=1
if [ ! -f "$fc.pe" ]; then
    echo "  FAIL  fncache: no cache file $fc.pe"
    fail=$((fail + 1))
elif ! cmp -s "$OUT/fc_none.exe" "$OUT/fc_cold.exe" || ! cmp -s "$OUT/fc_none.exe" "$OUT/fc_warm.exe"; then
    echo "  FAIL  fncache: a build with the cache differs from one without"
    fail=$((fail + 1))
elif ! grep -q "fncache: [1-9][0-9]* hits" "$OUT/fc_warm.log"; then
    echo "  FAIL  fncache: the second build took nothing from the cache (see $OUT/fc_warm.log)"
    fail=$((fail + 1))
else
    echo "  ok    fncache: $(grep -m1 'fncache:' "$OUT/fc_warm.log")"
fi

# DB3
build_run dbg tests/bootstrap/test_windows_cold_layout.fd FID_PE_DEBUG=1 && expect dbg tests/bootstrap/test_windows_cold_layout.fd
if [ -f "$OUT/dbg.exe" ]; then
    if python3 scripts/pe-dwarf-check.py "$OUT/dbg.exe" test_windows_cold_layout.fd; then
        echo "  ok    dwarf"
    else
        echo "  FAIL  dwarf: see the lines above"
        fail=$((fail + 1))
    fi
fi

echo "pe-win-check: $fail failed (logs in $OUT)"
[ $fail -eq 0 ]
