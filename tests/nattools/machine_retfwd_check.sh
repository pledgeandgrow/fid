#!/usr/bin/env bash
# OP12 (competitive-roadmap), llvm ME27 / LF24: the caller's result slot of a
# struct returned in two registers kept in vregs (compiler/nat_ir.fd
# ng_rf_promote, after ng_rr_caller_rewrite). Run after the compiler is
# built:
#   bash tests/nattools/machine_retfwd_check.sh [fid]   (default release/fid.exe)
# - On x86_64-linux, aarch64-linux and riscv64-linux (FID_VERIFY=1: the MIR
#   verifier and the allocation checker run too), main of
#   tests/bootstrap/test_natabi_retfwd.fd and of test_natabi_retregs.fd
#   keeps fewer loads, stores and copies in its final MIR than with
#   FID_NAT_OFF=retfwd (the slots stay in memory), and the function
#   `pair_sum` below none at all.
# - The images print the tests' expected lines with the pass on, with
#   FID_NAT_OFF=retfwd and with FID_NAT_OFF=retregs (x86-64 natively or in
#   WSL, AArch64 / RISC-V under qemu-user; --nat on a Windows host).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-retfwd"
mkdir -p "$OUT"
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

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

# memory operations of function $2 in the final MIR dump $1
memops() {
    awk -v fn="$2" '
        index($0, "[mir] " fn ": ") == 1 && index($0, " operations, frame ") > 0 { p = 1; next }
        p && /^\[mir\] [^0-9]/ { p = 0 }
        p && /^\[mir\] [0-9]+: (load|store|copy|ldx|stx|alum|alumst|brccm|setccm) / { n++ }
        END { print n + 0 }' "$1"
}

# A small program: the struct is only read back where it comes back
P="$OUT/pair_sum.fd"
cat > "$P" <<'EOF'
struct Pair {
    a: i64,
    b: i64,
}

fn make_pair(a: i64, b: i64) -> Pair {
    return Pair { a: a * 2, b: b + 1 };
}

// (two blocks: not inlined into main)
fn pair_sum(x: i64) -> i64 {
    if x > 1000000 {
        return 0;
    }
    val p: Pair = make_pair(x, x + 3);
    return p.a + p.b;
}

fn main() {
    println(pair_sum(5));
    println(pair_sum(0 - 4));
}
// Expected output:
// 19
// -8
EOF

for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    for T in tests/bootstrap/test_natabi_retfwd.fd tests/bootstrap/test_natabi_retregs.fd; do
        b=$(basename "$T" .fd)
        FID_VERIFY=1 FID_NAT_DUMP=main "$FID" emit "$T" --target $tgt -o "$OUT/$b-$tgt-on" >"$OUT/$b-$tgt-on.log" 2>&1 || bad "$tgt $b: build failed (see $OUT/$b-$tgt-on.log)"
        FID_NAT_OFF=retfwd FID_NAT_DUMP=main "$FID" emit "$T" --target $tgt -o "$OUT/$b-$tgt-off" >"$OUT/$b-$tgt-off.log" 2>&1 || bad "$tgt $b: build with retfwd off failed"
        FID_NAT_OFF=retregs "$FID" emit "$T" --target $tgt -o "$OUT/$b-$tgt-rr" >"$OUT/$b-$tgt-rr.log" 2>&1 || bad "$tgt $b: build with retregs off failed"
        on=$(memops "$OUT/$b-$tgt-on.log" main)
        off=$(memops "$OUT/$b-$tgt-off.log" main)
        if [ "$on" -lt "$off" ]; then
            ok "$tgt $b: main has $on memory operations, $off with the slots in memory"
        else
            bad "$tgt $b: main has $on memory operations, $off with FID_NAT_OFF=retfwd"
        fi
        WANT=$(expected "$T" | tr -d '\r')
        for mode in on off rr; do
            got=$(run_img "$OUT/$b-$tgt-$mode" $tgt | tr -d '\r')
            if [ -z "$got" ]; then
                echo "skip $tgt $b $mode: cannot run here"
            elif [ "$got" = "$WANT" ]; then
                ok "$tgt $b $mode: expected output"
            else
                bad "$tgt $b $mode: got $(echo "$got" | tr '\n' ' ')"
            fi
        done
    done
    FID_VERIFY=1 FID_NAT_DUMP=pair_sum "$FID" emit "$P" --target $tgt -o "$OUT/pair-$tgt" >"$OUT/pair-$tgt.log" 2>&1 || bad "$tgt pair_sum: build failed"
    n=$(memops "$OUT/pair-$tgt.log" pair_sum)
    if ! grep -q "pair_sum: [0-9]* operations, frame" "$OUT/pair-$tgt.log"; then
        echo "skip $tgt pair_sum: no MIR dump (inlined?)"
    elif ! grep -q "^\[mir\] [0-9]*: call .*" "$OUT/pair-$tgt.log"; then
        echo "skip $tgt pair_sum: make_pair was inlined"
    elif [ "$n" = 0 ]; then
        ok "$tgt pair_sum: no load, store or copy left"
    else
        bad "$tgt pair_sum: $n memory operations left (see $OUT/pair-$tgt.log)"
    fi
    got=$(run_img "$OUT/pair-$tgt" $tgt | tr -d '\r')
    if [ -n "$got" ]; then
        if [ "$got" = "$(printf '19\n-8')" ]; then ok "$tgt pair_sum: output"; else bad "$tgt pair_sum: got $(echo "$got" | tr '\n' ' ')"; fi
    fi
done

if [ "$HOST" = windows ]; then
    for T in tests/bootstrap/test_natabi_retfwd.fd tests/bootstrap/test_natabi_retregs.fd; do
        b=$(basename "$T" .fd)
        WANT=$(expected "$T" | tr -d '\r')
        for mode in on off; do
            img="$OUT/$b-nat-$mode.exe"
            if [ $mode = on ]; then
                FID_VERIFY=1 "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1
            else
                FID_NAT_OFF=retfwd "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1
            fi
            got=$("$img" 2>&1 | tr -d '\r')
            if [ "$got" = "$WANT" ]; then ok "--nat $b $mode: expected output"; else bad "--nat $b $mode: got $(echo "$got" | tr '\n' ' ')"; fi
        done
    done
fi

echo "$fails failure(s)"
[ $fails -eq 0 ]
