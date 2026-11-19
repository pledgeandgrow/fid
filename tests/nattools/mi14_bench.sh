#!/usr/bin/env bash
# MI14 (docs/roadmap/llvm-roadmap.md): the speed of `--nat` (the machine
# layer, compiler/nat_ir.fd) against pe_ir (the default Windows `emit`),
# best of N runs, on a Windows host:
#   bash tests/nattools/mi14_bench.sh [fid] [runs]   (default release/fid.exe 3)
# Programs: bench/fides/fib.fd, loop.fd, sieve.fd, hashmap.fd and
# tests/nattools/bench_float.fd. Each is built three ways:
#   pe_ir    `fid emit` (the default)
#   nat      `fid emit --nat`
#   nat-old  `fid emit --nat` with MI14's changes off
#            (FID_NAT_OFF=winfregs,splitfloat: no float registers on
#            Windows, no splitting of floats)
# and every image must print the same as pe_ir's (a difference is a
# failure: exit 1). Prints one line per program:
#   <name> pe_ir <ms> nat <ms> nat-old <ms> nat/pe_ir <ratio>
# Times are wall-clock milliseconds, the best of the runs; run it on a
# quiet machine (other fid builds make every number noise). The speed is
# recorded, not judged: MI17 (the default switch) reads these numbers.
set -u
FID="${1:-release/fid.exe}"
N="${2:-3}"
OUT="${TMPDIR:-/tmp}/fid-mi14-bench"
mkdir -p "$OUT"
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) ;;
    *) echo "mi14_bench: needs a Windows host (pe_ir and --nat build Windows images)"; exit 0 ;;
esac
fails=0
now_ms() { date +%s%3N; }

# best <exe>: the best wall time of N runs in ms (and the output in <exe>.out)
best() {
    local exe="$1" b="" k=0 t0 t1 d
    while [ "$k" -lt "$N" ]; do
        t0=$(now_ms)
        "$exe" >"$exe.out" 2>&1
        t1=$(now_ms)
        d=$((t1 - t0))
        if [ -z "$b" ] || [ "$d" -lt "$b" ]; then b="$d"; fi
        k=$((k + 1))
    done
    echo "$b"
}

for src in bench/fides/fib.fd bench/fides/loop.fd bench/fides/sieve.fd bench/fides/hashmap.fd tests/nattools/bench_float.fd; do
    name=$(basename "$src" .fd)
    pe="$OUT/$name-pe.exe"
    nat="$OUT/$name-nat.exe"
    old="$OUT/$name-natold.exe"
    if ! "$FID" emit "$src" -o "$pe" >"$pe.log" 2>&1; then echo "FAIL $name: pe_ir build"; fails=$((fails + 1)); continue; fi
    if ! "$FID" emit "$src" --nat -o "$nat" >"$nat.log" 2>&1; then echo "FAIL $name: --nat build"; fails=$((fails + 1)); continue; fi
    if ! env FID_NAT_OFF=winfregs,splitfloat "$FID" emit "$src" --nat -o "$old" >"$old.log" 2>&1; then echo "FAIL $name: --nat (MI14 off) build"; fails=$((fails + 1)); continue; fi
    tp=$(best "$pe")
    tn=$(best "$nat")
    to=$(best "$old")
    if ! cmp -s "$pe.out" "$nat.out"; then echo "FAIL $name: --nat prints differently from pe_ir"; diff "$pe.out" "$nat.out" | head -5; fails=$((fails + 1)); fi
    if ! cmp -s "$pe.out" "$old.out"; then echo "FAIL $name: --nat (MI14 off) prints differently from pe_ir"; fails=$((fails + 1)); fi
    ratio=$(awk -v a="$tn" -v b="$tp" 'BEGIN { if (b > 0) printf "%.2f", a / b; else print "-" }')
    echo "$name pe_ir $tp nat $tn nat-old $to nat/pe_ir $ratio"
done
if [ "$fails" -eq 0 ]; then echo "mi14_bench: all outputs equal"; else echo "mi14_bench: $fails failure(s)"; exit 1; fi
