#!/bin/bash
# pe_fncache_check.sh [FID]: inc-roadmap B5, the per-function cache of the
# default Windows backend (compiler/pe_ir.fd PeirGen_gen_cached,
# PeirGen_fc_*), on a Windows host where `fid emit` uses pe_ir.
#
# For every program (PROGS, default: tests/bootstrap/*.fd but the _fail
# tests) that pe_ir builds:
#   ref    FID_FNCACHE=0: no cache (the reference image)
#   cold   FID_FNCACHE=<dir>/fc: an empty cache, filled
#   warm   the same cache again: FID_PE_FNCACHE_STATS=1 must report hits
#   dflt   no FID_FNCACHE at all: on by default, the file
#          <output>.fides-fncache.pe beside the output
#   dflt2  the same output again (its build stamp removed): hits
# Every image must be byte-identical to ref. Then the edit pair
# (tests/nattools/pe_fncache_edit_a.fd, then _b.fd with one function
# changed, one cache) must give _b's uncached image, with hits and misses.
# `--no-fncache` and FID_FNCACHE=off must write no cache file.
#
#   bash tests/nattools/pe_fncache_check.sh release/fid.exe
#   JOBS=4 PROGS="tests/bootstrap/hello.fd" bash tests/nattools/pe_fncache_check.sh
set -u
FID="${1:-release/fid.exe}"
JOBS="${JOBS:-4}"
OUT=$(mktemp -d)
if [ -z "${PROGS:-}" ]; then
    PROGS=$(ls tests/bootstrap/*.fd | grep -v '_fail\.fd$')
fi

one() {
    local p="$1" out="$2" fid="$3"
    local n
    n=$(basename "$p" .fd)
    local d="$out/$n"
    mkdir -p "$d"
    if ! FIDES_NO_CACHE=1 FID_FNCACHE=0 "$fid" emit "$p" -o "$d/ref.exe" > "$d/ref.log" 2>&1; then
        echo "skip $n"
        return 0
    fi
    if [ -f "$d/ref.exe.fides-fncache.pe" ]; then
        echo "FAIL $n: FID_FNCACHE=0 wrote a cache file"
        return 0
    fi
    FIDES_NO_CACHE=1 FID_FNCACHE="$d/fc" "$fid" emit "$p" -o "$d/cold.exe" > "$d/cold.log" 2>&1
    FIDES_NO_CACHE=1 FID_FNCACHE="$d/fc" FID_PE_FNCACHE_STATS=1 "$fid" emit "$p" -o "$d/warm.exe" > "$d/warm.log" 2>&1
    "$fid" emit "$p" -o "$d/dflt.exe" > "$d/dflt.log" 2>&1
    rm -f "$d/dflt.exe.fides-stamp"
    FID_PE_FNCACHE_STATS=1 "$fid" emit "$p" -o "$d/dflt.exe" > "$d/dflt2.log" 2>&1
    local bad=""
    for k in cold warm dflt; do
        if ! cmp -s "$d/ref.exe" "$d/$k.exe"; then
            bad="$bad $k"
        fi
    done
    if [ ! -f "$d/fc.pe" ]; then
        bad="$bad no-cache-file"
    fi
    if [ ! -f "$d/dflt.exe.fides-fncache.pe" ]; then
        bad="$bad no-default-cache-file"
    fi
    if ! grep -q "fncache: [1-9][0-9]* hits" "$d/warm.log"; then
        bad="$bad warm-no-hits"
    fi
    if ! grep -q "fncache: [1-9][0-9]* hits" "$d/dflt2.log"; then
        bad="$bad default-no-hits"
    fi
    if [ -n "$bad" ]; then
        echo "FAIL $n:$bad"
    else
        echo "ok $n $(grep -m1 -o 'fncache: [0-9]* hits, [0-9]* misses' "$d/warm.log")"
    fi
}
export -f one

printf '%s\n' $PROGS | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {} "$OUT" "$FID" > "$OUT/results.txt"
ok=$(grep -c '^ok ' "$OUT/results.txt")
skip=$(grep -c '^skip ' "$OUT/results.txt")
nfail=$(grep -c '^FAIL ' "$OUT/results.txt")
grep '^FAIL ' "$OUT/results.txt" | head -40

# The edit pair
e="$OUT/edit"
mkdir -p "$e"
efail=0
FIDES_NO_CACHE=1 FID_FNCACHE=0 "$FID" emit tests/nattools/pe_fncache_edit_b.fd -o "$e/b_ref.exe" > "$e/b_ref.log" 2>&1
FIDES_NO_CACHE=1 FID_FNCACHE="$e/fc" "$FID" emit tests/nattools/pe_fncache_edit_a.fd -o "$e/a.exe" > "$e/a.log" 2>&1
FIDES_NO_CACHE=1 FID_FNCACHE="$e/fc" FID_PE_FNCACHE_STATS=1 "$FID" emit tests/nattools/pe_fncache_edit_b.fd -o "$e/b.exe" > "$e/b.log" 2>&1
if ! cmp -s "$e/b_ref.exe" "$e/b.exe"; then
    echo "FAIL edit: the edited program's image differs from its uncached build"
    efail=1
elif ! grep -q "fncache: [1-9][0-9]* hits, [1-9][0-9]* misses" "$e/b.log"; then
    echo "FAIL edit: want hits and misses: $(grep -m1 'fncache:' "$e/b.log")"
    efail=1
else
    echo "ok edit $(grep -m1 -o 'fncache: [0-9]* hits, [0-9]* misses' "$e/b.log")"
    "$e/b.exe" > "$e/b.got" 2>&1
    printf '7\nsmall\n15\nbig\n' > "$e/b.want"
    if ! diff -q --strip-trailing-cr "$e/b.want" "$e/b.got" > /dev/null; then
        echo "FAIL edit: the edited program prints $(tr '\n' ' ' < "$e/b.got")"
        efail=1
    fi
fi

# The off switches
o="$OUT/off"
mkdir -p "$o"
ofail=0
"$FID" emit tests/bootstrap/hello.fd --no-fncache -o "$o/h1.exe" > "$o/h1.log" 2>&1
FID_FNCACHE=off "$FID" emit tests/bootstrap/hello.fd -o "$o/h2.exe" > "$o/h2.log" 2>&1
if [ -f "$o/h1.exe.fides-fncache.pe" ] || [ -f "$o/h2.exe.fides-fncache.pe" ]; then
    echo "FAIL off: --no-fncache or FID_FNCACHE=off wrote a cache file"
    ofail=1
else
    echo "ok off switches"
fi

echo "pe_fncache_check: $ok identical, $nfail failed, $skip not built by pe_ir; edit $([ $efail -eq 0 ] && echo ok || echo FAILED); off $([ $ofail -eq 0 ] && echo ok || echo FAILED) (files in $OUT)"
[ $nfail -eq 0 ] && [ $efail -eq 0 ] && [ $ofail -eq 0 ]
