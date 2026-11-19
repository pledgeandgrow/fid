#!/usr/bin/env bash
# TG17 (docs/roadmap/llvm-roadmap.md): the post-allocation rules as
# per-target data (compiler/nat_ir.fd ng_post_rules: x86-64 ldfwd,
# ldfwdmov, memop, cmpslot, rmwslot; AArch64 ldfwd, ldfwdmov, wbidx;
# RISC-V ldfwd, ldfwdmov), load after store of spill slots (ldfwd) and its
# extension past register moves (ldfwdmov). Run after the compiler is built:
#   bash tests/nattools/machine_tg17_check.sh [fid] [--matrix]
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows, each
# of tests/bootstrap/test_machine_ldfwd.fd and test_machine_ldfwd2.fd is
# built with FID_VERIFY=1 and --stats three ways: every rule on,
# FID_NAT_OFF=ldfwd, FID_NAT_OFF=ldfwdmov. Passing:
#   - "[ldfwd] N slot loads after a store became register moves": N > 0
#     with the rules on, N = 0 with FID_NAT_OFF=ldfwd, and N with
#     ldfwdmov off <= N with it on (summed over both programs, on at
#     least one target strictly less: ldfwdmov forwards past a move)
#   - the three images print the file's expected output wherever they run
#     (WSL / qemu on Windows; --nat natively)
#   - the image with ldfwd off differs from the one with it on (bytes)
# With --matrix it then runs scripts/pass-matrix.sh on each target with
# NAT="ldfwd ldfwdmov memop cmpslot rmwslot wbidx" and FID_VERIFY=1 over
# the whole suite (long): every switch must give the same results.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
MATRIX="${2:-}"
OUT="${TMPDIR:-/tmp}/fid-machine-tg17"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

run_exe() {
    local dir="$1" exe="$2" t="$3" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        *-linux)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$dir" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$dir" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$dir" && "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

ldfwd_n() { sed -n 's/^\[ldfwd\] \([0-9]*\) .*/\1/p' "$1" | tail -1; }

strict=0
for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    tot_on=0; tot_off=0; tot_nomov=0
    for src in tests/bootstrap/test_machine_ldfwd.fd tests/bootstrap/test_machine_ldfwd2.fd; do
        base=$(basename "$src" .fd)
        want="$(expected "$src")"
        targ=(--target "$t"); ext=""
        if [ "$t" = windows ]; then targ=(--nat); ext=".exe"; fi
        for mode in on off nomov; do
            sw=(FID_VERIFY=1)
            [ "$mode" = off ] && sw=(FID_VERIFY=1 FID_NAT_OFF=ldfwd)
            [ "$mode" = nomov ] && sw=(FID_VERIFY=1 FID_NAT_OFF=ldfwdmov)
            img="$base-$t-$mode$ext"
            if ! env "${sw[@]}" "$FID" emit "$src" "${targ[@]}" --stats -o "$OUT/$img" >"$OUT/$img.log" 2>&1; then
                bad "$t $base ($mode) builds: $(tail -3 "$OUT/$img.log")"
                continue
            fi
            n=$(ldfwd_n "$OUT/$img.log"); n=${n:-0}
            case "$mode" in on) tot_on=$((tot_on + n)) ;; off) tot_off=$((tot_off + n)) ;; nomov) tot_nomov=$((tot_nomov + n)) ;; esac
            got=$(run_exe "$OUT" "$img" "$t"); s=$?
            if [ "$s" = 99 ]; then
                :
            elif [ "$got" = "$want" ]; then
                ok "$t $base ($mode): expected output"
            else
                bad "$t $base ($mode): output differs"; diff <(echo "$want") <(echo "$got") | head -5
            fi
        done
        if [ -f "$OUT/$base-$t-on$ext" ] && [ -f "$OUT/$base-$t-off$ext" ]; then
            if cmp -s "$OUT/$base-$t-on$ext" "$OUT/$base-$t-off$ext"; then bad "$t $base: the image is the same with ldfwd off"; else ok "$t $base: ldfwd changes the image"; fi
        fi
    done
    if [ "$tot_on" -gt 0 ]; then ok "$t: $tot_on loads forwarded"; else bad "$t: no load forwarded"; fi
    if [ "$tot_off" -eq 0 ]; then ok "$t: none with FID_NAT_OFF=ldfwd"; else bad "$t: $tot_off forwarded with FID_NAT_OFF=ldfwd"; fi
    if [ "$tot_nomov" -le "$tot_on" ]; then ok "$t: $tot_nomov without ldfwdmov <= $tot_on with it"; else bad "$t: more forwards without ldfwdmov ($tot_nomov > $tot_on)"; fi
    [ "$tot_nomov" -lt "$tot_on" ] && strict=$((strict + 1))
done
if [ "$strict" -gt 0 ]; then ok "ldfwdmov forwards past a move on $strict target(s)"; else bad "ldfwdmov forwarded nothing past a move on any target"; fi

if [ "$MATRIX" = --matrix ]; then
    for t in x86_64-linux aarch64-linux riscv64-linux x86_64-windows; do
        if NAT="ldfwd ldfwdmov memop cmpslot rmwslot wbidx" IROPT="" IME="" EGRAPH="" RULES="" FID_VERIFY=1 FID="$FID" bash scripts/pass-matrix.sh "$t"; then
            ok "pass-matrix $t: the post-allocation rules off give the same results"
        else
            bad "pass-matrix $t: see build/pm/$t/matrix.txt"
        fi
    done
fi

if [ "$fails" -eq 0 ]; then echo "machine_tg17_check: 0 failure(s)"; else echo "machine_tg17_check: $fails failure(s)"; exit 1; fi
