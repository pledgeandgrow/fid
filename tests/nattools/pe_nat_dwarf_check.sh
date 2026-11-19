#!/usr/bin/env bash
# CT6 (docs/roadmap/complaint-roadmap.md; llvm DB3/DB4, release N9): DWARF
# in `--nat` Windows images, so gdb and lldb on Windows show lines,
# functions and variables of an optimized build (compiler/nat_enc.fd
# nat_pe_debug; compiler/nat_ir.fd ng_dbg_win_dwarf).
#
#   FID=release/fid.exe bash tests/nattools/pe_nat_dwarf_check.sh
#
# Run from the repository root in Git Bash on Windows (the images run
# there; python3 and, when installed, x86_64-w64-mingw32-objdump are taken
# from PATH, else from WSL). Checks, each printed as ok / FAIL / skip:
#   1. tests/nattools/pe_nat_dwarf_unit.fd (built with `ccompile`, CC
#      default zig cc) prints its expected lines; the image it writes passes
#      pe_nat_dwarf_check.py;
#   2. test_machine_dbg_vars.fd and test_machine_dbg_run.fd built with
#      `emit --nat` and FID_NAT_DWARF=1, optimized and with --debug: each
#      prints its expected lines and passes pe_nat_dwarf_check.py (the
#      markers' lines have rows; dbg_vars' parameters, variables and
#      locations through dbg_vars_check.py);
#   3. `emit --nat -g` (needs the main.fd hunk that sets g_nat_win_dwarf;
#      FAIL until then);
#   4. a plain `emit --nat` (no switch): no debug sections, headers of 512
#      bytes, as before CT6.
# Exit 1 on a failure.
set -u
FID="${FID:-release/fid.exe}"
CC="${CC:-zig cc}"
export CC
OUT="${TMPDIR:-/tmp}/fid-pe-nat-dwarf"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# python3 (with mingw objdump when there) here, or in WSL
PY=""
for p in python3 python; do
    if command -v "$p" >/dev/null 2>&1 && "$p" -c 'import sys; sys.exit(0 if sys.version_info >= (3, 6) else 1)' >/dev/null 2>&1; then
        PY="$p"
        break
    fi
done
in_wsl=0
if command -v wsl.exe >/dev/null 2>&1 && ! command -v x86_64-w64-mingw32-objdump >/dev/null 2>&1; then
    in_wsl=1
fi
check() {
    # check <image> [args...]: pe_nat_dwarf_check.py on it
    local img="$1"
    shift
    if [ "$in_wsl" = 1 ]; then
        local wimg wroot
        wimg="$(cygpath -w "$img")"
        wroot="$(cygpath -w "$PWD")"
        wsl.exe -e bash -lc "cd \"\$(wslpath -u '$wroot')\" && python3 tests/nattools/pe_nat_dwarf_check.py \"\$(wslpath -u '$wimg')\" $*"
    else
        "$PY" tests/nattools/pe_nat_dwarf_check.py "$img" "$@"
    fi
}

# 1. the unit test
if $FID ccompile tests/nattools/pe_nat_dwarf_unit.fd -o "$OUT/pdu.exe" > "$OUT/pdu.log" 2>&1; then
    if diff <(expected tests/nattools/pe_nat_dwarf_unit.fd) <(FID_PE_DWARF_OUT="$OUT/pdu_img.exe" "$OUT/pdu.exe" | tr -d '\r') > "$OUT/pdu.diff"; then
        ok "pe_nat_dwarf_unit output"
    else
        bad "pe_nat_dwarf_unit output differs:"; head -20 "$OUT/pdu.diff"
    fi
    if check "$OUT/pdu_img.exe" > "$OUT/pdu.chk" 2>&1; then
        ok "pe_nat_dwarf_unit image: $(tail -1 "$OUT/pdu.chk")"
    else
        bad "pe_nat_dwarf_unit image"; cat "$OUT/pdu.chk"
    fi
else
    bad "build pe_nat_dwarf_unit.fd"; tail -5 "$OUT/pdu.log"
fi

# 2./3./4. real builds
build_check() {
    # build_check <label> <test> <env> <flags> <checker args>
    local label="$1" t="$2" envs="$3" flags="$4" cargs="$5"
    local exe="$OUT/$label.exe"
    if ! env -u FID_NAT_DWARF -u FID_PE_DEBUG -u FID_NAT_FRAMES $envs $FID emit "$t" --nat $flags -o "$exe" > "$OUT/$label.log" 2>&1; then
        bad "$label: build"; tail -5 "$OUT/$label.log"; return
    fi
    if ! diff <(expected "$t") <("$exe" 2>&1 | tr -d '\r') > "$OUT/$label.diff"; then
        bad "$label: output differs"; head -10 "$OUT/$label.diff"
    fi
    if check "$exe" $cargs > "$OUT/$label.chk" 2>&1; then
        ok "$label: $(tail -1 "$OUT/$label.chk")"
    else
        bad "$label"; cat "$OUT/$label.chk"
    fi
}
VARS=tests/bootstrap/test_machine_dbg_vars.fd
RUN=tests/bootstrap/test_machine_dbg_run.fd
build_check vars-opt "$VARS" "FID_NAT_DWARF=1" "" "$VARS --vars"
build_check vars-debug "$VARS" "FID_NAT_DWARF=1" "--debug" "$VARS --vars"
build_check run-opt "$RUN" "FID_NAT_DWARF=1" "" "$RUN"
build_check run-debug "$RUN" "FID_NAT_DWARF=1" "--debug" "$RUN"
build_check run-g "$RUN" "FID_NAT_FRAMES=0" "-g" "$RUN"
build_check vars-plain "$VARS" "FID_NAT_FRAMES=0" "" "--plain"

[ "$fails" = 0 ] && echo "pe-nat-dwarf-check: ok"
exit $(( fails > 0 ))
