#!/usr/bin/env bash
# llvm LF24: an Option or Result of one word returned in two registers (ME27,
# compiler/nat_ir.fd ng_rr_plan) is kept by the caller in two vregs: the
# result slot's stores, the match's tag and payload loads and the slot's
# address all go away (ng_rr_caller_rewrite, ng_rf_promote; the LF24 step
# turns the slot's MO_SLOTADDR into a constant). Run after the compiler is
# built:
#   bash tests/nattools/machine_optmatch_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux and riscv64-linux, built with
# FID_VERIFY=1 and FID_NAT_DUMP='*':
# - the final MIR of index_or, div_or and count_found
#   (tests/bootstrap/test_natabi_optmatch.fd) holds no load, store, copy,
#   indexed or memory-operand operation and no slotaddr;
# - with FID_NAT_OFF=retfwd each of them holds at least one (the check
#   sees the difference);
# - the images print the file's expected output with the pass on, with
#   FID_NAT_OFF=retfwd and with FID_NAT_OFF=retregs (x86-64 natively or in
#   WSL, AArch64 / RISC-V under qemu-user; a target without a runner is
#   built and checked only).
# On a Windows host the same for --nat (output only).
# A function the inliner folded into its caller has no final MIR of its
# own: reported as a skip, not a failure.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-optmatch"
mkdir -p "$OUT"
SRC=tests/bootstrap/test_natabi_optmatch.fd
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC" | tr -d '\r')"

wslpath_of() {
    local p
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

# run_img <image> <target>: prints the image's output, "" when it cannot run here
run_img() {
    local q=""
    case "$2" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    if [ -n "$WSL" ]; then
        if [ -n "$q" ]; then
            $WSL sh -c "command -v $q >/dev/null" || return 0
        fi
        MSYS_NO_PATHCONV=1 $WSL $q "$(wslpath_of "$1")" 2>&1
    elif [ "$HOST" = linux ]; then
        if [ -n "$q" ]; then
            command -v "$q" >/dev/null 2>&1 || return 0
            "$q" "$1" 2>&1
        else
            "$1" 2>&1
        fi
    fi
}

# has_final <dump> <function>: the function has a final MIR listing
has_final() {
    awk -v fn="$2" 'index($0, "[mir] " fn ": ") == 1 && index($0, " operations, frame ") > 0 { f = 1 } END { exit !f }' "$1"
}

# memops <dump> <function>: memory operations and slot addresses in the
# function's final MIR (spill code, ldslot/stslot, is not counted)
memops() {
    awk -v fn="$2" '
        index($0, "[mir] " fn ": ") == 1 && index($0, " operations, frame ") > 0 { p = 1; next }
        p && /^\[mir\] [^0-9]/ { p = 0 }
        p && /^\[mir\] [0-9]+: (load|store|copy|ldx|stx|alum|alumst|brccm|setccm|slotaddr) / { n++ }
        END { print n + 0 }' "$1"
}

for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    on="$OUT/om-$tgt-on"
    if ! FID_VERIFY=1 FID_NAT_DUMP='*' "$FID" emit "$SRC" --target $tgt -o "$on" >"$on.out" 2>"$on.log"; then
        bad "$tgt: build failed: $(grep -v '^\[' "$on.log" | tail -2)"
        continue
    fi
    off="$OUT/om-$tgt-off"
    FID_NAT_OFF=retfwd FID_NAT_DUMP='*' "$FID" emit "$SRC" --target $tgt -o "$off" >"$off.out" 2>"$off.log" || bad "$tgt: build with retfwd off failed"
    rr="$OUT/om-$tgt-rr"
    FID_NAT_OFF=retregs "$FID" emit "$SRC" --target $tgt -o "$rr" >"$rr.out" 2>"$rr.log" || bad "$tgt: build with retregs off failed"
    for fn in index_or div_or count_found; do
        if ! has_final "$on.log" "$fn"; then
            echo "skip $tgt $fn: no MIR of its own (inlined?)"
            continue
        fi
        n_on=$(memops "$on.log" "$fn")
        n_off=$(memops "$off.log" "$fn")
        if [ "$n_on" -eq 0 ] && [ "$n_off" -gt 0 ]; then
            ok "$tgt $fn: tag and payload in vregs, no slot ($n_off memory operations with FID_NAT_OFF=retfwd)"
        else
            bad "$tgt $fn: $n_on memory operations or slot addresses left ($n_off with FID_NAT_OFF=retfwd; see $on.log)"
        fi
    done
    for mode in on off rr; do
        got=$(run_img "$OUT/om-$tgt-$mode" $tgt | tr -d '\r')
        if [ -z "$got" ]; then
            echo "skip $tgt $mode: cannot run here"
        elif [ "$got" = "$WANT" ]; then
            ok "$tgt $mode: expected output"
        else
            bad "$tgt $mode: got $(echo "$got" | tr '\n' ' ')"
        fi
    done
done

if [ "$HOST" = windows ]; then
    for mode in on off; do
        img="$OUT/om-nat-$mode.exe"
        if [ $mode = on ]; then
            FID_VERIFY=1 "$FID" emit "$SRC" --nat -o "$img" >"$img.log" 2>&1
        else
            FID_NAT_OFF=retfwd "$FID" emit "$SRC" --nat -o "$img" >"$img.log" 2>&1
        fi
        got=$("$img" 2>&1 | tr -d '\r')
        if [ "$got" = "$WANT" ]; then ok "--nat $mode: expected output"; else bad "--nat $mode: got $(echo "$got" | tr '\n' ' ')"; fi
    done
fi

echo "$fails failure(s)"
[ $fails -eq 0 ]
