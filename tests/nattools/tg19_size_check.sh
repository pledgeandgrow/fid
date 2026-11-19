#!/usr/bin/env bash
# TG19: code size mode. `fid emit --size` (nat_ir g_nat_size: no loop-head
# padding, no inline Vec bodies, the mid-end's unrolling off) and RISC-V's
# 2-byte instructions (TG9's RVC, compiler/nat_enc.fd nat_rvc_compress via
# compiler/nat_ir.fd ng_rvc_pass), which TG19 turns on by default on rv64gc
# (ng_rvc_wanted, NG_RVC_DEFAULT) and under --size. Run after the compiler
# is built:
#   bash tests/nattools/tg19_size_check.sh [fid]   (default release/fid.exe)
#
# For each program, built for riscv64-linux:
#   off      FID_NAT_OFF=rvc            e_flags 4, no [rvc] line
#   default  no switch                  e_flags 5 (EF_RISCV_RVC), no [rvc]
#                                       line (a default build is quiet), an
#                                       image smaller than `off`
#   on       FID_NAT_ON=rvc             an [rvc] line, the default's bytes
#   size     --size                     an [rvc] line, e_flags 5, an image
#                                       no larger than the default's and
#                                       smaller than `off`
#   rv64g    --cpu rv64g                e_flags 4, no [rvc] line (no C
#                                       extension)
# Every build runs under qemu-riscv64 (through WSL on Windows) where the
# `off` build runs, and prints the file's expected output. On x86_64-linux,
# --size gives an image no larger than the plain one (as
# scripts/driver-goals-check.sh checks). A size table and the total saving
# are printed. Exit 1 on a failure.
set -u
FID="${1:-${FID:-release/fid.exe}}"
OUT="${TMPDIR:-/tmp}/fid-tg19"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() {
    awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
RUNNER=""
if [ -n "$WSL" ]; then
    if $WSL sh -c "command -v qemu-riscv64 >/dev/null"; then RUNNER="qemu-riscv64"; fi
elif [ "$HOST" = linux ]; then
    if [ "$(uname -m)" = riscv64 ]; then RUNNER="native"
    elif command -v qemu-riscv64 >/dev/null 2>&1; then RUNNER="qemu-riscv64"; fi
fi

# run <exe>: its output; status 99 when this host cannot run it
run() {
    [ -n "$RUNNER" ] || return 99
    local q="$RUNNER"
    [ "$q" = native ] && q=""
    (cd "$OUT" && $WSL $q "./$1" 2>&1 | tr -d '\r')
}

# the ELF's e_flags (offset 48, 4 bytes)
eflags() { od -An -tu4 -j48 -N4 "$1" | tr -d ' '; }
fsize() { wc -c < "$1" | tr -d ' '; }

PROGS="tests/bootstrap/test_driver_fast_alloc.fd tests/bootstrap/test_machine_rvc.fd
tests/bootstrap/test_machine_jtab.fd tests/bootstrap/test_machine_bparams.fd"

tot_off=0
tot_def=0
tot_size=0
printf '%-30s %10s %10s %10s\n' program rvc-off default --size > "$OUT/sizes.txt"
for src in $PROGS; do
    [ -f "$src" ] || { echo "skip $src (missing)"; continue; }
    base=$(basename "$src" .fd)
    WANT="$(expected "$src")"
    built=1
    for mode in off default on size rv64g; do
        exe="$base-$mode"
        sw=(FID_VERIFY=1)
        extra=()
        case "$mode" in
            off) sw+=(FID_NAT_OFF=rvc) ;;
            on) sw+=(FID_NAT_ON=rvc) ;;
            size) extra=(--size) ;;
            rv64g) extra=(--cpu rv64g) ;;
        esac
        if ! env "${sw[@]}" "$FID" emit "$src" --target riscv64-linux "${extra[@]+"${extra[@]}"}" -o "$OUT/$exe" >"$OUT/$exe.out" 2>"$OUT/$exe.log"; then
            bad "$base $mode: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2)"
            built=0
            continue
        fi
        line=$(grep '^\[rvc\]' "$OUT/$exe.log" | tail -1)
        fl=$(eflags "$OUT/$exe")
        case "$mode" in
            off|rv64g)
                if [ -z "$line" ] && [ "$fl" = 4 ]; then ok "$base $mode: not compressed (e_flags 4)"; else bad "$base $mode: e_flags $fl, ${line:-no [rvc] line}"; fi ;;
            default)
                if [ -z "$line" ] && [ "$fl" = 5 ]; then ok "$base default: compressed (e_flags 5), quietly"; else bad "$base default: e_flags $fl (want 5), ${line:-no [rvc] line}"; fi ;;
            on|size)
                case "$line" in
                    *"instructions in 2 bytes"*)
                        if [ "$fl" = 5 ]; then ok "$base $mode: $line"; else bad "$base $mode: e_flags $fl (want 5)"; fi ;;
                    *) bad "$base $mode: ${line:-no [rvc] line} (e_flags $fl)" ;;
                esac ;;
        esac
        out=$(run "$exe")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$base $mode: built (no qemu-riscv64 here to run it)"
        elif [ "$mode" = off ] && [ "$out" != "$WANT" ]; then
            # the uncompressed build does not run here: a host matter
            # (a dynamically linked program without a riscv64 sysroot)
            echo "skip $base: the FID_NAT_OFF=rvc build does not print its expected output here either"
            echo "skip" > "$OUT/$base.norun"
        elif [ -f "$OUT/$base.norun" ]; then
            :
        elif [ "$out" = "$WANT" ]; then
            ok "$base $mode: output as expected"
        else
            bad "$base $mode: output differs"
            diff <(echo "$WANT") <(echo "$out") | head -8
        fi
    done
    rm -f "$OUT/$base.norun"
    [ $built -eq 1 ] || continue
    if cmp -s "$OUT/$base-default" "$OUT/$base-on"; then
        ok "$base: FID_NAT_ON=rvc gives the default image"
    else
        bad "$base: the FID_NAT_ON=rvc image differs from the default one"
    fi
    so=$(fsize "$OUT/$base-off")
    sd=$(fsize "$OUT/$base-default")
    ss=$(fsize "$OUT/$base-size")
    if [ "$sd" -lt "$so" ]; then ok "$base: default image $sd bytes < $so without RVC"; else bad "$base: default image $sd bytes, $so without RVC"; fi
    if [ "$ss" -le "$sd" ] && [ "$ss" -lt "$so" ]; then ok "$base: --size image $ss bytes <= $sd, < $so"; else bad "$base: --size image $ss bytes (default $sd, without RVC $so)"; fi
    tot_off=$((tot_off + so))
    tot_def=$((tot_def + sd))
    tot_size=$((tot_size + ss))
    printf '%-30s %10s %10s %10s\n' "$base" "$so" "$sd" "$ss" >> "$OUT/sizes.txt"
done

# x86-64: --size no larger (no loop-head padding, no inline Vec bodies)
F=tests/bootstrap/test_driver_fast_alloc.fd
if "$FID" emit "$F" --target x86_64-linux -o "$OUT/x64-plain" >"$OUT/x64-plain.log" 2>&1 &&
   "$FID" emit "$F" --target x86_64-linux --size -o "$OUT/x64-size" >"$OUT/x64-size.log" 2>&1; then
    p=$(fsize "$OUT/x64-plain"); s=$(fsize "$OUT/x64-size")
    if [ "$s" -le "$p" ]; then ok "x86_64 --size image $s bytes <= $p"; else bad "x86_64 --size image $s bytes > $p"; fi
else
    bad "x86_64 emit failed (see $OUT/x64-plain.log, $OUT/x64-size.log)"
fi

cat "$OUT/sizes.txt"
if [ "$tot_off" -gt 0 ]; then
    echo "riscv64 images: $tot_off -> $tot_def bytes by default ($(( (tot_off - tot_def) * 100 / tot_off ))% smaller), $tot_size with --size ($(( (tot_off - tot_size) * 100 / tot_off ))% smaller)"
fi
echo "$fails failure(s)"
[ "$fails" -eq 0 ]
