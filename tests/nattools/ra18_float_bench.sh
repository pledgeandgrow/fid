#!/usr/bin/env bash
# RA18 (docs/roadmap/llvm-roadmap.md): the measurement the goal asks for.
# The float loop (tests/nattools/bench_float.fd: a float loop with no
# call, then one with a call in it) built with floats in float registers
# (the default since MI25 / OP17 / MI14) against the xmm cache alone
# (FID_NAT_OFF=fregs: every float in a general register, the emitter
# remembering what xmm0/xmm1 mirror: the Nov 9 first step, 1834 -> 1364
# ms), on --nat Windows and x86_64-linux (through WSL on Windows); and on
# aarch64-linux / riscv64-linux (under qemu: the ratio, not the time, is
# what counts there) the callee-saved float registers against
# FID_NAT_OFF=fcallee (floats across the call spill).
#   bash tests/nattools/ra18_float_bench.sh [fid] [runs]   (default release/fid.exe 3)
# Prints one line per target and mode, best of N runs in ms:
#   <target> fregs <ms> xmmcache <ms> ratio <fregs/xmmcache>
#   <target> fcallee <ms> spill <ms> ratio <fcallee/spill> (fcallee ranges N)
# Every build must print the same two numbers (a difference is a failure,
# exit 1). Run it on a quiet machine; record the numbers in
# docs/reference/callee-saved-floats.md.
set -u
FID="${1:-release/fid.exe}"
N="${2:-3}"
SRC=tests/nattools/bench_float.fd
OUT="${TMPDIR:-/tmp}/fid-ra18-bench"
mkdir -p "$OUT"
fails=0
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
now_ms() { date +%s%3N; }

# runner <target>: the command prefix that runs an image of it ("" when
# none: the line is skipped)
runner() {
    case "$1" in
        windows) [ "$HOST" = windows ] && echo "direct" ;;
        x86_64-linux) if [ -n "$WSL" ]; then echo "$WSL"; elif [ "$HOST" = linux ] && [ "$(uname -m)" = x86_64 ]; then echo "direct"; fi ;;
        aarch64-linux) if [ -n "$WSL" ]; then $WSL sh -c "command -v qemu-aarch64 >/dev/null" && echo "$WSL qemu-aarch64"; elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64; elif [ "$(uname -m)" = aarch64 ]; then echo direct; fi ;;
        riscv64-linux) if [ -n "$WSL" ]; then $WSL sh -c "command -v qemu-riscv64 >/dev/null" && echo "$WSL qemu-riscv64"; elif command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64; elif [ "$(uname -m)" = riscv64 ]; then echo direct; fi ;;
    esac
}

# best <runner> <dir> <exe>: best wall time of N runs (output in <exe>.out)
best() {
    local r="$1" dir="$2" exe="$3" b="" k=0 t0 t1 d pre=""
    [ "$r" != direct ] && pre="$r"
    while [ "$k" -lt "$N" ]; do
        t0=$(now_ms)
        (cd "$dir" && $pre "./$exe" 2>&1 | tr -d '\r' >"$exe.out")
        t1=$(now_ms)
        d=$((t1 - t0))
        if [ -z "$b" ] || [ "$d" -lt "$b" ]; then b="$d"; fi
        k=$((k + 1))
    done
    echo "$b"
}

# pair <target> <label A> <env A> <label B> <env B>
pair() {
    local t="$1" la="$2" ea="$3" lb="$4" eb="$5" targ=(--target "$1") a="ra18-$1-$2" b="ra18-$1-$4"
    if [ "$t" = windows ]; then targ=(--nat); a="$a.exe"; b="$b.exe"; fi
    if ! env $ea "$FID" emit "$SRC" "${targ[@]}" --stats -o "$OUT/$a" >"$OUT/$a.log" 2>&1; then echo "FAIL $t $la build"; fails=$((fails + 1)); return; fi
    if ! env $eb "$FID" emit "$SRC" "${targ[@]}" --stats -o "$OUT/$b" >"$OUT/$b.log" 2>&1; then echo "FAIL $t $lb build"; fails=$((fails + 1)); return; fi
    local r
    r=$(runner "$t")
    if [ -z "$r" ]; then echo "skip $t: no runner here"; return; fi
    local ta tb
    ta=$(best "$r" "$OUT" "$a")
    tb=$(best "$r" "$OUT" "$b")
    if ! cmp -s "$OUT/$a.out" "$OUT/$b.out"; then echo "FAIL $t: $la and $lb print differently"; fails=$((fails + 1)); fi
    local ratio nfc
    ratio=$(awk -v x="$ta" -v y="$tb" 'BEGIN { if (y > 0) printf "%.2f", x / y; else print "-" }')
    nfc=$(sed -n 's/^\[fcallee\] [^:]*: \([0-9]*\) .*/\1/p' "$OUT/$a.log" | awk '{s += $1} END {print s + 0}')
    echo "$t $la $ta $lb $tb ratio $ratio (fcallee ranges $nfc)"
}

pair windows fregs "FID_NAT_DEFAULT=1" xmmcache "FID_NAT_OFF=fregs"
pair x86_64-linux fregs "FID_NAT_DEFAULT=1" xmmcache "FID_NAT_OFF=fregs"
pair aarch64-linux fcallee "FID_NAT_DEFAULT=1" spill "FID_NAT_OFF=fcallee"
pair riscv64-linux fcallee "FID_NAT_DEFAULT=1" spill "FID_NAT_OFF=fcallee"
if [ "$fails" -eq 0 ]; then echo "ra18_float_bench: all outputs equal"; else echo "ra18_float_bench: $fails failure(s)"; exit 1; fi
