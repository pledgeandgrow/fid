#!/bin/bash
# tests/memprof/check_more.sh [FID]: the memory profile beyond check.sh
# (roadmap L7C, "memory profiling: allocation tracking, region usage"),
# and run-time library loading on every backend (roadmap L6C).
#
#   regions  FID_REGION_ARENA=1 fid run --mem: the report's region usage
#            (regions by depth, their bytes and largest, the peak live in
#            regions, region bytes by function and line); without the
#            arenas no region line
#   native   fid emit (the machine layer: --nat on Windows, x86_64-linux
#            run under WSL / Linux) with FID_MEM_PROFILE=1: the same
#            report's totals, peak and bytes by function
#   dynlib   stdlib/dynlib.fd (tests/bootstrap/test_dynlib.fd) prints its
#            expected output under fid run, fid ccompile and fid emit
#            (pe_ir) on Windows; run and ccompile elsewhere
#
# Run from the project root. Needs a C compiler (CC, as for ccompile).
#
#   bash tests/memprof/check_more.sh                      # release/fid.exe
#   bash tests/memprof/check_more.sh release/fid-dev.exe
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;;
esac
fail=0

ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
skip() { echo "  skip  $1: $2"; }
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }
# a Windows file's path inside WSL (C:/x/y -> /mnt/c/x/y)
wsl_path() {
    local p d
    p=$(cygpath -m "$1")
    d=$(printf '%s' "${p:0:1}" | tr 'A-Z' 'a-z')
    printf '/mnt/%s%s' "$d" "${p:2}"
}

# --- regions: usage in the --mem report -----------------------------------
FID_REGION_ARENA=1 "$FID" run tests/memprof/region_profile.fd --mem > "$OUT/rg.out" 2> "$OUT/rg.err"
nocr "$OUT/rg.out" "$OUT/rg.err"
if grep -q "^built 314$" "$OUT/rg.out"; then
    ok "regions: program output"
else
    bad "regions: program output" "$(head -c 300 "$OUT/rg.out") $(head -c 300 "$OUT/rg.err")"
fi
line=$(grep -E "^fid --mem: regions: [0-9]+ region\(s\), [0-9]+ allocation\(s\), [0-9]+ bytes; peak [0-9]+ bytes live in regions$" "$OUT/rg.err" | head -1)
if [ -n "$line" ]; then
    ok "regions: totals and peak"
    nreg=$(echo "$line" | sed 's/.*regions: \([0-9]*\) region.*/\1/')
    rpeak=$(echo "$line" | sed 's/.*peak \([0-9]*\) bytes live in regions/\1/')
    # build() twice: two outer regions and two inner ones
    if [ "$nreg" -ge 4 ]; then ok "regions: $nreg regions >= 4"; else bad "regions: count" "$nreg"; fi
    # the Vec of 200 elements lives in the outer region: at least 1600 bytes
    if [ "$rpeak" -ge 1600 ]; then ok "regions: peak $rpeak >= 1600"; else bad "regions: peak" "$rpeak"; fi
else
    bad "regions: totals and peak" "$(grep -m3 'fid --mem' "$OUT/rg.err" || head -c 300 "$OUT/rg.err")"
fi
if grep -Eq "^  depth 1: 2 region\(s\), [0-9]+ allocation\(s\), [0-9]+ bytes; largest region [0-9]+ bytes$" "$OUT/rg.err"; then
    ok "regions: depth 1, two regions"
else
    bad "regions: depth 1" "$(grep -m3 'depth' "$OUT/rg.err")"
fi
if grep -Eq "^  depth 2: 2 region\(s\), [0-9]+ allocation\(s\), [0-9]+ bytes; largest region [0-9]+ bytes$" "$OUT/rg.err"; then
    ok "regions: depth 2, two regions"
else
    bad "regions: depth 2" "$(grep -m3 'depth' "$OUT/rg.err")"
fi
if grep -q "^  by function, in regions:$" "$OUT/rg.err" && grep -q "^  by line, in regions:$" "$OUT/rg.err"; then
    ok "regions: by function and by line"
else
    bad "regions: by function / line" "$(grep -m4 'in regions' "$OUT/rg.err")"
fi
# the push in the outer region (line 16) is a row of the region lines
if sed -n '/^  by line, in regions:$/,$p' "$OUT/rg.err" | grep -Eq "allocation\(s\): [^ ]*region_profile\.fd:16$"; then
    ok "regions: line 16 (the push)"
else
    bad "regions: line 16" "$(sed -n '/^  by line, in regions:$/,$p' "$OUT/rg.err" | head -c 300)"
fi
# without the arenas a region is a scope: no region usage
"$FID" run tests/memprof/region_profile.fd --mem > "$OUT/rg0.out" 2> "$OUT/rg0.err"
nocr "$OUT/rg0.out" "$OUT/rg0.err"
if grep -q "^built 314$" "$OUT/rg0.out" && grep -q "^fid --mem: " "$OUT/rg0.err" && ! grep -q "regions:" "$OUT/rg0.err"; then
    ok "regions: none without FID_REGION_ARENA"
else
    bad "regions: without arenas" "$(head -c 300 "$OUT/rg0.err")"
fi

# --- native: the machine layer's profile (FID_MEM_PROFILE) ---------------
native_check() {    # $1 label, $2 executable runner prefix ("" or "wsl"), $3 binary
    local label="$1" bin="$3"
    if [ "$2" = "wsl" ]; then
        local lp
        lp=$(wsl_path "$bin")
        MSYS_NO_PATHCONV=1 wsl -e sh -c "cp '$lp' /tmp/fid_mp_\$\$ && chmod +x /tmp/fid_mp_\$\$ && FID_MEM_PROFILE=1 /tmp/fid_mp_\$\$; r=\$?; rm -f /tmp/fid_mp_\$\$; exit \$r" > "$OUT/n.out" 2> "$OUT/n.err"
    else
        FID_MEM_PROFILE=1 "$bin" > "$OUT/n.out" 2> "$OUT/n.err"
    fi
    nocr "$OUT/n.out" "$OUT/n.err"
    if ! grep -q "^filled 5$" "$OUT/n.out"; then
        bad "$label: program output" "$(head -c 300 "$OUT/n.out") $(head -c 300 "$OUT/n.err")"
        return
    fi
    if grep -Eq "^fid --mem: [0-9]+ allocation\(s\), [0-9]+ bytes; [0-9]+ freed \([0-9]+ bytes\); peak [0-9]+ bytes live$" "$OUT/n.err"; then
        ok "$label: totals and peak"
    else
        bad "$label: totals and peak" "$(head -c 300 "$OUT/n.err")"
    fi
    # fill's five 100-byte blocks (keep may be inlined into main natively)
    if grep -Eq "^ +500 bytes in +5 allocation\(s\): fill$" "$OUT/n.err"; then
        ok "$label: row fill"
    else
        bad "$label: row fill" "$(grep -A6 'by function' "$OUT/n.err" | head -c 400)"
    fi
    local peak
    peak=$(sed -n 's/.*peak \([0-9]*\) bytes live.*/\1/p' "$OUT/n.err" | head -1)
    if [ -n "$peak" ] && [ "$peak" -ge 612 ]; then ok "$label: peak $peak >= 612"; else bad "$label: peak" "${peak:-none}"; fi
    # the rows add up to the totals (the report's own printing not counted)
    local tot rows
    tot=$(sed -n 's/^fid --mem: \([0-9]*\) allocation.*/\1/p' "$OUT/n.err" | head -1)
    rows=$(sed -n 's/^ *[0-9]* bytes in *\([0-9]*\) allocation(s): .*/\1/p' "$OUT/n.err" | awk '{s+=$1} END {print s+0}')
    if [ -n "$tot" ] && [ "$tot" = "$rows" ]; then ok "$label: rows add up ($rows)"; else bad "$label: rows add up" "total ${tot:-none}, rows $rows"; fi
    # without FID_MEM_PROFILE: no report
    if [ "$2" = "wsl" ]; then
        MSYS_NO_PATHCONV=1 wsl -e sh -c "cp '$(wsl_path "$bin")' /tmp/fid_mp0 && chmod +x /tmp/fid_mp0 && /tmp/fid_mp0; rm -f /tmp/fid_mp0" > "$OUT/n0.out" 2> "$OUT/n0.err"
    else
        "$bin" > "$OUT/n0.out" 2> "$OUT/n0.err"
    fi
    if grep -q "fid --mem" "$OUT/n0.err"; then bad "$label: off without FID_MEM_PROFILE" "$(head -c 200 "$OUT/n0.err")"; else ok "$label: off without FID_MEM_PROFILE"; fi
}
if [ "$WIN" = 1 ]; then
    if "$FID" emit tests/memprof/mem_profile.fd --nat -o "$OUT/mpn.exe" > "$OUT/mpn.build" 2>&1; then
        native_check "native --nat" "" "$OUT/mpn.exe"
    else
        bad "native --nat: build" "$(tail -c 300 "$OUT/mpn.build")"
    fi
    if command -v wsl > /dev/null 2>&1 && "$FID" emit tests/memprof/mem_profile.fd --target x86_64-linux -o "$OUT/mpl" > "$OUT/mpl.build" 2>&1; then
        native_check "native x86_64-linux (WSL)" "wsl" "$OUT/mpl"
    else
        skip "native x86_64-linux" "no WSL or build failed: $(tail -c 200 "$OUT/mpl.build" 2>/dev/null)"
    fi
elif [ "$(uname -s)" = "Linux" ] && [ "$(uname -m)" = "x86_64" ]; then
    if "$FID" emit tests/memprof/mem_profile.fd --target x86_64-linux -o "$OUT/mpl" > "$OUT/mpl.build" 2>&1; then
        native_check "native x86_64-linux" "" "$OUT/mpl"
    else
        bad "native x86_64-linux: build" "$(tail -c 300 "$OUT/mpl.build")"
    fi
fi

# --- dynlib: Library::open / symbol / close on each backend --------------
sed -n 's#^//     ##p' tests/bootstrap/test_dynlib.fd > "$OUT/dyn.expected"
dyn_check() {    # $1 label, $2 output file
    nocr "$2"
    if diff -q "$OUT/dyn.expected" "$2" > /dev/null; then
        ok "dynlib: $1"
    else
        bad "dynlib: $1" "$(diff "$OUT/dyn.expected" "$2" | head -c 300)"
    fi
}
"$FID" run tests/bootstrap/test_dynlib.fd > "$OUT/dyn_run.out" 2> "$OUT/dyn_run.err"
dyn_check "fid run" "$OUT/dyn_run.out"
if "$FID" ccompile tests/bootstrap/test_dynlib.fd -o "$OUT/dyn_c$EXE" > "$OUT/dyn_c.build" 2>&1; then
    "$OUT/dyn_c$EXE" > "$OUT/dyn_c.out" 2>&1
    dyn_check "ccompile" "$OUT/dyn_c.out"
else
    bad "dynlib: ccompile build" "$(tail -c 300 "$OUT/dyn_c.build")"
fi
if [ "$WIN" = 1 ]; then
    if "$FID" emit tests/bootstrap/test_dynlib.fd -o "$OUT/dyn_pe.exe" > "$OUT/dyn_pe.build" 2>&1; then
        "$OUT/dyn_pe.exe" > "$OUT/dyn_pe.out" 2>&1
        dyn_check "emit (pe_ir)" "$OUT/dyn_pe.out"
    else
        bad "dynlib: emit build" "$(tail -c 300 "$OUT/dyn_pe.build")"
    fi
    # the machine layer loads through the host's loader on --nat Windows
    # (linux_runtime/sys.fd pert_dl_load over the Windows loader; the old
    # row expected 'libc: none', which a static no-loader build gave)
    if "$FID" emit tests/bootstrap/test_dynlib.fd --nat -o "$OUT/dyn_nat.exe" > "$OUT/dyn_nat.build" 2>&1; then
        "$OUT/dyn_nat.exe" > "$OUT/dyn_nat.out" 2>&1
        nocr "$OUT/dyn_nat.out"
        dyn_check "emit --nat (the Windows loader)" "$OUT/dyn_nat.out"
    else
        bad "dynlib: emit --nat build" "$(tail -c 300 "$OUT/dyn_nat.build")"
    fi
fi

rm -rf "$OUT"
if [ "$fail" -gt 0 ]; then
    echo "$fail check(s) failed"
    exit 1
fi
echo "all checks passed"
