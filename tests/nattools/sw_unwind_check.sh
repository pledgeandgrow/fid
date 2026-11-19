#!/usr/bin/env bash
# RA12 / DB6 (docs/roadmap/llvm-roadmap.md): unwinding through functions
# whose callee-saved registers shrink wrapping saves late, after the
# compiler is built:
#   bash tests/nattools/sw_unwind_check.sh [fid]   (default release/fid.exe)
# For x86_64-linux, aarch64-linux and riscv64-linux it builds
# tests/bootstrap/test_machine_sw_unwind.fd (FID_VERIFY=1, --stats) and:
#   - scripts/dbg-check.py on the test (each FDE's CFA rules);
#   - runs it under qemu-user's gdb stub with tests/nattools/
#     sw_unwind_check.py: at every call of `probe` from `mid` or `rare2`,
#     the registers .debug_frame unwinds for their caller must be the ones
#     it had (before, inside and after the deferred saves);
#   - where `[shrinkwrap] mid:` or `[shrinkwrap] rare2:` shows saves were
#     deferred: the same build with FID_NAT_OFF=swcfi (no rows for the
#     deferred saves) must FAIL that check (--expect-fail), so the check
#     sees the rows; with FID_NAT_OFF=shrinkwrap it must pass;
#   - the program's output must be the test's expected lines.
# The images run in WSL on Windows (qemu-x86_64 / qemu-aarch64 /
# qemu-riscv64 there; x86-64 needs qemu-x86_64 for the gdb stub). Exit 1
# on a failure; a target that cannot run here is reported and skipped.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-sw-unwind"
mkdir -p "$OUT"
T=tests/bootstrap/test_machine_sw_unwind.fd
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=0
if [ "$WIN" = 1 ] && command -v wsl.exe >/dev/null 2>&1; then WSL=1; fi
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
wslp() { MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -w "$1")" 2>/dev/null | tr -d '\r' | grep -v '^wsl: ' | tail -1; }

build() {
    local img="$1" tgt="$2"
    shift 2
    FID_VERIFY=1 "$FID" emit "$T" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
}

# unwind <image> [--expect-fail]: sw_unwind_check.py's output; exit 0 / 1,
# 2 when it cannot run here
unwind() {
    local img="$1" r rc
    shift
    if [ "$WSL" = 1 ]; then
        local wi wp
        wi=$(wslp "$img")
        wp=$(wslp tests/nattools/sw_unwind_check.py)
        [ -n "$wi" ] && [ -n "$wp" ] || { echo "no WSL path"; return 2; }
        r=$(MSYS_NO_PATHCONV=1 wsl.exe -e sh -c "cp '$wi' /tmp/fid_sw_unwind_img && chmod +x /tmp/fid_sw_unwind_img && python3 '$wp' /tmp/fid_sw_unwind_img $*" 2>&1 | tr -d '\r' | grep -v '^wsl: ')
        # (the pipeline's status is grep's: read the script's from its words)
        case "$r" in
            *"sw unwind OK"*|*"caught as expected"*) rc=0 ;;
            skip:*|"nothing checked"*) rc=2 ;;
            *) rc=1 ;;
        esac
    else
        chmod +x "$img"
        r=$($PY tests/nattools/sw_unwind_check.py "$img" "$@" 2>&1)
        rc=$?
    fi
    printf '%s\n' "$r"
    return $rc
}

run_img() {
    local img="$1" tgt="$2" q=""
    case "$tgt" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    if [ "$WSL" = 1 ]; then
        local w
        w=$(wslp "$img")
        if [ -n "$q" ]; then
            MSYS_NO_PATHCONV=1 wsl.exe -e "$q" "$w" 2>&1 | tr -d '\r' | grep -v '^wsl: '
        else
            MSYS_NO_PATHCONV=1 wsl.exe -e "$w" 2>&1 | tr -d '\r' | grep -v '^wsl: '
        fi
    elif [ "$(uname -s)" = Linux ]; then
        chmod +x "$img"
        if [ -n "$q" ] && [ "$(uname -m)" != "${tgt%%-*}" ]; then "$q" "$img" 2>&1; else "$img" 2>&1; fi
    else
        return 2
    fi
}

for tgt in ${SW_TARGETS:-x86_64-linux aarch64-linux riscv64-linux}; do
    img="$OUT/sw-$tgt"
    build "$img" "$tgt" --stats || { bad "$tgt builds: $(tail -3 "$img.log")"; continue; }
    ok "$tgt builds with FID_VERIFY=1"
    deferred=$(grep -E '^\[shrinkwrap\] (mid|rare2): ' "$img.log")
    if [ -n "$deferred" ]; then
        printf '%s\n' "$deferred" | sed "s/^/     $tgt /"
    else
        echo "note $tgt: no [shrinkwrap] line for mid or rare2 (nothing deferred there on this target)"
    fi
    got=$(run_img "$img" "$tgt")
    rrc=$?
    # (a dynamically linked image under qemu-user without a sysroot)
    case "$got" in *"Could not open '/lib/ld-linux"*) rrc=2 ;; esac
    if [ $rrc -eq 2 ]; then
        echo "skip $tgt run: cannot run here"
    elif [ "$got" = "$(expected "$T")" ]; then
        ok "$tgt runs"
    else
        bad "$tgt runs (output differs)"
        printf '%s\n' "$got" | head -10 | sed 's/^/     /'
    fi
    r=$(unwind "$img")
    rc=$?
    if [ $rc -eq 2 ]; then
        echo "skip $tgt unwind: $(echo "$r" | tail -1)"
        continue
    elif [ $rc -eq 0 ]; then
        ok "$tgt unwind: $(echo "$r" | tail -2 | head -1)"
    else
        bad "$tgt unwind"
        printf '%s\n' "$r" | head -12 | sed 's/^/     /'
    fi
    FID_NAT_OFF=shrinkwrap build "$img-off" "$tgt" || { bad "$tgt builds with FID_NAT_OFF=shrinkwrap"; continue; }
    r=$(unwind "$img-off")
    if [ $? -eq 0 ]; then ok "$tgt unwind with FID_NAT_OFF=shrinkwrap"; else bad "$tgt unwind with FID_NAT_OFF=shrinkwrap: $(echo "$r" | head -3)"; fi
    if [ -n "$deferred" ]; then
        FID_NAT_OFF=swcfi build "$img-nocfi" "$tgt" || { bad "$tgt builds with FID_NAT_OFF=swcfi"; continue; }
        r=$(unwind "$img-nocfi" --expect-fail)
        if [ $? -eq 0 ]; then ok "$tgt FID_NAT_OFF=swcfi is caught: $(echo "$r" | head -1 | cut -c1-150)"; else bad "$tgt FID_NAT_OFF=swcfi not caught: $(echo "$r" | head -3)"; fi
    fi
done

if r=$($PY scripts/dbg-check.py --fid "$FID" "$T" 2>&1); then ok "scripts/dbg-check.py $T"; else bad "scripts/dbg-check.py $T: $(echo "$r" | tail -3)"; fi

if [ "$fails" = 0 ]; then echo "all RA12 unwind checks passed"; exit 0; fi
echo "$fails RA12 unwind check(s) failed"
exit 1
