#!/usr/bin/env bash
# DB4 (docs/roadmap/llvm-roadmap.md): variables of native builds, every
# path that writes them, and their lexical scopes.
#
#   FID=release/fid.exe bash tests/nattools/dbg_scopes_check.sh
#
# Run from the repository root in Linux or WSL (qemu-x86_64, qemu-aarch64,
# qemu-riscv64 and python3 on PATH; FID may be the Windows fid.exe). Checks:
#   - unit: tests/nattools/dbg_scopes_unit.fd (nat_enc.fd's lexical-block
#     DIEs) and dbg_scope_blocks_unit.fd (nat_ir.fd's scopes from line
#     rows, nat_dbg_scope_end), through `fid ccompile` (CC, default
#     "zig cc") or, without a C compiler, `fid run`;
#   - for x86_64-linux, aarch64-linux, riscv64-linux, optimized and with
#     --debug: test_machine_dbg_scopes.fd prints its expected lines,
#     dbg_scopes_check.py (blocks nest, the shadowed names are in blocks,
#     each marker sees the innermost binding) and dbg_run_check.py under
#     qemu (--strict for --debug: every value available and right);
#     test_machine_dbg_vars.fd passes dbg_vars_check.py;
#   - the direct path (FID_NAT_OFF=mir, aarch64-linux and riscv64-linux):
#     the same three checks, now that it writes variables too;
#   - RA4's live-range splitting (FID_NAT_ON=splitall, x86-64 and
#     AArch64): test_machine_dbg_run.fd under dbg_run_check.py, every
#     value shown right (a split value's slot is not read where a piece
#     holds a newer value);
#   - the function cache (FID_FNCACHE): a second build served from the
#     cache has the first build's .debug_info, .debug_loc and
#     .debug_abbrev byte for byte, and passes dbg_scopes_check.py;
#   - FID_NAT_OFF=dbgscopes: no lexical blocks (dbg_scopes_check.py must
#     then report the shadowed names: the check fails without DB4);
#   - aarch64-macos and --nat Windows (FID_NAT_DWARF=1) images:
#     dbg_scopes_check.py on their DWARF.
# EXTRA_OFF=<passes> adds FID_NAT_OFF passes to every build (ldyn when the
# dynamic imports do not resolve in qemu). Exit 0 when every check holds.
set -u
FID="${FID:-release/fid.exe}"
OUT="$(mktemp -d)"
fail=0
off="${EXTRA_OFF:-}"
SRC=tests/bootstrap/test_machine_dbg_scopes.fd
VARS=tests/bootstrap/test_machine_dbg_vars.fd
winpath() {
    if [[ "$FID" == *.exe ]] && command -v wslpath >/dev/null 2>&1; then wslpath -w "$1"; else echo "$1"; fi
}
# fid with extra FID_NAT_OFF passes ($1, may be empty) and FID_FNCACHE ($2)
run_fid() {
    local noff="$1"; shift
    local cache="$1"; shift
    local args=()
    local next_o=0
    for a in "$@"; do
        [ $next_o = 1 ] && a="$(winpath "$a")"
        next_o=0
        [ "$a" = "-o" ] && next_o=1
        args+=("$a")
    done
    local all="$off"
    if [ -n "$noff" ]; then all="${all:+$all,}$noff"; fi
    local wenv="${WSLENV:+$WSLENV:}FID_NAT_OFF:FID_FNCACHE/p:FID_NAT_DWARF:FID_NAT_ON"
    if [ -n "$cache" ]; then
        FID_NAT_OFF="$all" FID_FNCACHE="$cache" FID_NAT_DWARF="${NAT_DWARF:-}" FID_NAT_ON="${NAT_ON:-}" WSLENV="$wenv" "$FID" "${args[@]}"
    else
        FID_NAT_OFF="$all" FID_NAT_DWARF="${NAT_DWARF:-}" FID_NAT_ON="${NAT_ON:-}" WSLENV="$wenv" "$FID" "${args[@]}"
    fi
}
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | grep '^// ' | grep -v 'Expected output' | sed 's#^// ##'; }
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
qemu_of() {
    case "$1" in aarch64-linux) echo qemu-aarch64 ;; riscv64-linux) echo qemu-riscv64 ;; *) echo qemu-x86_64 ;; esac
}

# --- unit tests -------------------------------------------------------------
# (a Windows fid.exe under WSL compiles them into the Windows temp folder:
# a C compiler there cannot write to the WSL file system)
UDIR="$OUT"
if [[ "$FID" == *.exe ]] && command -v cmd.exe >/dev/null 2>&1 && command -v wslpath >/dev/null 2>&1; then
    wt="$(cmd.exe /c 'echo %TEMP%' 2>/dev/null | tr -d '\r')"
    if [ -n "$wt" ]; then
        UDIR="$(wslpath "$wt")/dbg-scopes-$$"
        mkdir -p "$UDIR"
    fi
fi
for u in dbg_scopes_unit dbg_scope_blocks_unit; do
    f="tests/nattools/$u.fd"
    got="$OUT/$u.out"
    if CC="${CC:-zig cc}" WSLENV="${WSLENV:+$WSLENV:}CC" "$FID" ccompile "$f" -o "$(winpath "$UDIR/$u.exe")" > "$OUT/$u.log" 2>&1 && [ -f "$UDIR/$u.exe" ]; then
        "$UDIR/$u.exe" 2>&1 | tr -d '\r' > "$got"
    else
        "$FID" run "$f" 2>/dev/null | tr -d '\r' | grep -v '^\[' > "$got"
    fi
    if diff <(expected "$f") "$got" > "$OUT/$u.diff"; then ok "unit $u"; else bad "unit $u"; head -20 "$OUT/$u.diff"; fi
done

# --- one build: output, scopes, values ------------------------------------
# check_build <label> <target> <exe> <strict flag>
check_build() {
    local label="$1" t="$2" exe="$3" strict="$4"
    chmod +x "$exe" 2>/dev/null
    local q; q="$(qemu_of "$t")"
    if diff <(expected "$SRC") <($q "$exe" 2>&1 | tr -d '\r') > /dev/null; then ok "$label output"; else bad "$label output differs"; fi
    python3 tests/nattools/dbg_scopes_check.py "$exe" "$SRC" > "$exe.sc" 2>&1 && ok "$label scopes" || { bad "$label scopes"; grep FAIL "$exe.sc"; }
    timeout 300 python3 tests/nattools/dbg_run_check.py "$exe" "$SRC" $strict > "$exe.run" 2>&1 && ok "$label values" || { bad "$label values"; grep -E "FAIL|available" "$exe.run"; }
}

for t in x86_64-linux aarch64-linux riscv64-linux; do
    for mode in opt debug; do
        flag=""; strict=""
        if [ "$mode" = debug ]; then flag="--debug"; strict="--strict"; fi
        exe="$OUT/sc-$t-$mode"
        if run_fid "" "" emit "$SRC" --target "$t" $flag -o "$exe" > "$exe.log" 2>&1; then
            check_build "$t $mode" "$t" "$exe" "$strict"
        else
            bad "build $t $mode"; tail -3 "$exe.log"
        fi
    done
    exe="$OUT/vars-$t"
    if run_fid "" "" emit "$VARS" --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        python3 tests/nattools/dbg_vars_check.py "$exe" "$VARS" > "$exe.vc" 2>&1 && ok "$t vars" || { bad "$t vars"; grep FAIL "$exe.vc"; }
    else
        bad "build vars $t"; tail -3 "$exe.log"
    fi
done

# --- the direct path (no machine IR) ----------------------------------------
for t in aarch64-linux riscv64-linux; do
    exe="$OUT/sc-$t-direct"
    if run_fid mir "" emit "$SRC" --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        check_build "$t direct" "$t" "$exe" ""
    else
        bad "build $t direct"; tail -3 "$exe.log"
    fi
    exe="$OUT/vars-$t-direct"
    if run_fid mir "" emit "$VARS" --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        python3 tests/nattools/dbg_vars_check.py "$exe" "$VARS" > "$exe.vc" 2>&1 && ok "$t direct vars" || { bad "$t direct vars"; grep FAIL "$exe.vc"; }
    else
        bad "build vars $t direct"; tail -3 "$exe.log"
    fi
done

# --- RA4's split values (FID_NAT_ON=splitall: every value split) ------------
# A value whose loop piece holds a newer value than its slot must not be
# shown from the slot there (ng_dbg_split_loc; needs ng_mir_alloc_split to
# call ng_dbg_split_note): every value dbg_run_check.py can read is right.
RUN=tests/bootstrap/test_machine_dbg_run.fd
for t in x86_64-linux aarch64-linux; do
    exe="$OUT/run-$t-split"
    if NAT_ON=splitall run_fid "" "" emit "$RUN" --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        chmod +x "$exe" 2>/dev/null
        if diff <(expected "$RUN") <($(qemu_of "$t") "$exe" 2>&1 | tr -d '\r') > /dev/null; then ok "$t splitall output"; else bad "$t splitall output differs"; fi
        timeout 300 python3 tests/nattools/dbg_run_check.py "$exe" "$RUN" > "$exe.run" 2>&1 && ok "$t splitall values" || { bad "$t splitall values"; grep -E "FAIL|available" "$exe.run"; }
    else
        bad "build $t splitall"; tail -3 "$exe.log"
    fi
done

# --- the function cache -----------------------------------------------------
cache="$UDIR/fncache.bin"
a="$OUT/sc-cache-1"; b="$OUT/sc-cache-2"
if run_fid "" "$cache" emit "$SRC" --target x86_64-linux -o "$a" --stats > "$a.log" 2>&1 && run_fid "" "$cache" emit "$SRC" --target x86_64-linux -o "$b" --stats > "$b.log" 2>&1; then
    if grep -q "hits" "$b.log" && ! grep -q " 0 hits" "$b.log"; then ok "cache hit"; else echo "note: no [fncache] hit line in the second build's log"; fi
    if python3 - "$a" "$b" <<'EOF'
import sys
sys.path.insert(0, "tests/nattools")
import dbg_vars_check as dv
a = open(sys.argv[1], "rb").read()
b = open(sys.argv[2], "rb").read()
bad = [s for s in (".debug_info", ".debug_loc", ".debug_abbrev") if dv.section(a, s) != dv.section(b, s)]
print("differ: " + ", ".join(bad) if bad else "same")
sys.exit(1 if bad else 0)
EOF
    then ok "cache: same debug sections"; else bad "cache: debug sections differ"; fi
    python3 tests/nattools/dbg_scopes_check.py "$b" "$SRC" > "$b.sc" 2>&1 && ok "cache scopes" || { bad "cache scopes"; grep FAIL "$b.sc"; }
else
    bad "build with FID_FNCACHE"; tail -3 "$a.log" "$b.log"
fi

# --- scopes off: the shadowing is back (the check must see it) -------------
exe="$OUT/sc-noscopes"
if run_fid dbgscopes "" emit "$SRC" --target x86_64-linux -o "$exe" > "$exe.log" 2>&1; then
    if python3 tests/nattools/dbg_scopes_check.py "$exe" "$SRC" > "$exe.sc" 2>&1; then
        bad "FID_NAT_OFF=dbgscopes: dbg_scopes_check.py did not see the shadowed names"
    else
        ok "FID_NAT_OFF=dbgscopes: shadowing reported"
    fi
else
    bad "build dbgscopes"; tail -3 "$exe.log"
fi

# --- other images: Mach-O, --nat Windows --------------------------------
exe="$OUT/sc-mac"
if run_fid "" "" emit "$SRC" --target aarch64-macos -g -o "$exe" > "$exe.log" 2>&1; then
    python3 tests/nattools/dbg_scopes_check.py "$exe" "$SRC" > "$exe.sc" 2>&1 && ok "aarch64-macos scopes" || { bad "aarch64-macos scopes"; grep FAIL "$exe.sc"; }
else
    bad "build aarch64-macos"; tail -3 "$exe.log"
fi
exe="$OUT/sc-nat.exe"
if NAT_DWARF=1 run_fid "" "" emit "$SRC" --nat -o "$exe" > "$exe.log" 2>&1; then
    python3 tests/nattools/dbg_scopes_check.py "$exe" "$SRC" > "$exe.sc" 2>&1 && ok "--nat scopes" || { bad "--nat scopes"; grep FAIL "$exe.sc"; }
else
    bad "build --nat"; tail -3 "$exe.log"
fi

[ "$UDIR" != "$OUT" ] && rm -rf "${UDIR:?}"
rm -rf "$OUT"
[ $fail = 0 ] && echo "dbg-scopes-check: ok"
exit $fail
