#!/usr/bin/env bash
# CS3 (competitive roadmap): borrow checking linear in program size.
# Generates borrow-dense synthetic programs of growing size (each function
# takes references to its parameters and locals, walks a Vec with `for`,
# builds and borrows a second one), runs `fid check --time` on each, and
# reads the "[time] borrowck" line. Linear means the time per 1000 lines
# stays flat: the check fails if the largest program's time per line is
# more than RATIO_MAX (default 3) times the smallest one's.
#
#   FID=release/fid.exe bash tests/tooling/cs3-borrowck-scaling.sh
#   SIZES="10000 100000 1000000" FID=... bash tests/tooling/cs3-borrowck-scaling.sh
#
# The default sizes (10k, 40k, 160k lines) keep the run short; the roadmap
# target (10k/100k/1M) is a SIZES= away.
set -u
FID=${FID:-release/fid.exe}
SIZES=${SIZES:-"10000 40000 160000"}
RATIO_MAX=${RATIO_MAX:-3}
TMP=${TMPDIR:-/tmp}/cs3-borrowck.$$
mkdir -p "$TMP"
trap 'rm -rf "$TMP"' EXIT

gen() { # gen <lines> <file>
    local lines=$1 out=$2
    local nf=$(( lines / 14 ))
    awk -v nf="$nf" 'BEGIN {
        for (i = 0; i < nf; i++) {
            printf "fn f_%d(v: Vec<i64>, s: str) -> i64 {\n", i
            printf "    val r1: & Vec<i64> = & v;\n"
            printf "    var total: i64 = 0;\n"
            printf "    for x in v {\n"
            printf "        total = total + x;\n"
            printf "    }\n"
            printf "    val n: i64 = r1.len();\n"
            printf "    var w: Vec<i64> = Vec::new();\n"
            printf "    w.push(n + %d);\n", i
            printf "    val r2: & Vec<i64> = & w;\n"
            printf "    val m: i64 = r2.len();\n"
            printf "    total = total + m + str_len(s);\n"
            printf "    return total;\n"
            printf "}\n"
        }
        printf "fn main() {\n"
        printf "    var v: Vec<i64> = Vec::new();\n"
        printf "    v.push(1);\n"
        printf "    println(int_to_str(f_0(v, \"x\")));\n"
        printf "}\n"
    }' > "$out"
}

first_rate=""
last_rate=""
for n in $SIZES; do
    f="$TMP/cs3_$n.fd"
    gen "$n" "$f"
    lines=$(wc -l < "$f")
    out=$("$FID" check "$f" --time 2>&1)
    if ! printf '%s\n' "$out" | grep -q "no errors"; then
        echo "FAIL: $lines-line program not accepted"
        printf '%s\n' "$out" | grep -v '^\[build\]' | head -20
        exit 1
    fi
    ms=$(printf '%s\n' "$out" | sed -n 's/^\[time\] borrowck: \([0-9]*\) ms.*/\1/p' | head -1)
    [ -z "$ms" ] && { echo "FAIL: no borrowck time in output"; exit 1; }
    # nanoseconds per line (integer)
    rate=$(( ms * 1000000 / lines ))
    echo "lines $lines  borrowck ${ms} ms  ${rate} ns/line"
    [ -z "$first_rate" ] && first_rate=$rate
    last_rate=$rate
done
# A tiny program's time is all fixed cost: compare against at least 1 ns.
[ "$first_rate" -lt 1 ] && first_rate=1
if [ $(( last_rate )) -gt $(( first_rate * RATIO_MAX )) ]; then
    echo "FAIL: borrowck time per line grew ${last_rate}/${first_rate} (> ${RATIO_MAX}x): not linear"
    exit 1
fi
echo "PASS: borrowck time per line stays within ${RATIO_MAX}x across sizes"
