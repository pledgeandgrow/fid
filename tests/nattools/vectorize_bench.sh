#!/usr/bin/env bash
# competitive-roadmap OP22: times of tests/nattools/vectorize_bench.fd built
# by `fid emit` with the loop vectorizer and without (FID_NAT_OFF=vectorize),
# best of RUNS runs each, on the targets that can run here:
#   bash tests/nattools/vectorize_bench.sh [fid] [runs]
# x86-64 Linux (SSE2, and AVX2 when the CPU has it) under WSL or natively,
# AArch64 Linux under qemu-user (emulated: the ratio is indicative only),
# --nat Windows natively. The two outputs (checksums) must be the same;
# a difference is a failure (exit 1). Record the table in
# docs/reference/loop-vectorization.md.
set -u
FID="${1:-release/fid.exe}"
RUNS="${2:-3}"
OUT="${TMPDIR:-/tmp}/fid-vectorize-bench"
mkdir -p "$OUT"
B=tests/nattools/vectorize_bench.fd
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0

ms() { date +%s%N | cut -c1-13; }

best() {
    # best <runner...>: the fastest of RUNS runs in ms; the output to $OUT/last
    local b=999999999 k=0 t0 t1 d
    while [ $k -lt "$RUNS" ]; do
        t0=$(ms)
        "$@" >"$OUT/last" 2>&1
        t1=$(ms)
        d=$((t1 - t0))
        [ $d -lt $b ] && b=$d
        k=$((k + 1))
    done
    echo $b
}

one() {
    # one <name> <runner prefix> <emit args...>
    local name="$1" pre="$2"
    shift 2
    local img="$OUT/$name" ext=""
    [ "$name" = nat ] && ext=".exe"
    if ! "$FID" emit "$B" "$@" -o "$img$ext" >"$img.log" 2>&1; then echo "$name: build failed"; fails=$((fails + 1)); return; fi
    if ! env FID_NAT_OFF=vectorize "$FID" emit "$B" "$@" -o "$img-off$ext" >"$img-off.log" 2>&1; then echo "$name: build (off) failed"; fails=$((fails + 1)); return; fi
    local on off
    on=$(best $pre "$img$ext"); cp "$OUT/last" "$img.out"
    off=$(best $pre "$img-off$ext"); cp "$OUT/last" "$img-off.out"
    if ! cmp -s "$img.out" "$img-off.out"; then echo "$name: OUTPUT DIFFERS"; fails=$((fails + 1)); fi
    printf "%-8s vectorized %6d ms   scalar %6d ms   output %s\n" "$name" "$on" "$off" "$(head -c 60 "$img.out")"
}

if [ -n "$RUN" ] || { [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; }; then
    one sse "$RUN" --target x86_64-linux
    if $RUN grep -q avx2 /proc/cpuinfo 2>/dev/null; then one avx2 "$RUN" --target x86_64-linux --cpu x86-64-v3; fi
fi
if $RUN sh -c 'command -v qemu-aarch64' >/dev/null 2>&1; then one neon "$RUN qemu-aarch64" --target aarch64-linux; fi
if [ "$WIN" = 1 ]; then one nat "" --nat; fi
if [ "$fails" = 0 ]; then echo "vectorize_bench: done"; else echo "vectorize_bench: $fails failed"; exit 1; fi
