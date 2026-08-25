#!/usr/bin/env bash
# Codegen Benchmark Suite — Fides vs gcc -O2
#
# Compares Fides-compiled code against equivalent hand-written C compiled
# with gcc -O2. Tests multiple workloads to identify codegen quality gaps.
#
# Backends tested:
#   1. Fides ccompile (C codegen → gcc)
#   2. Hand-written C with gcc -O2 (baseline)
#
# Usage: bash benchmarks/bench_codegen.sh

set -e

FIDESC=${FIDESC:-./target/release/fidesc}
TMPDIR=$(mktemp -d)

# Build the compiler if needed
if [ ! -f "$FIDESC" ]; then
    echo "Building fidesc (release)..."
    cargo build --release 2>/dev/null
fi

echo "=== Codegen Benchmark Suite: Fides vs gcc -O2 ==="
echo "Compiler: $FIDESC"
echo "Date: $(date -u '+%Y-%m-%d %H:%M:%S UTC')"
echo ""

# Helper: time a command and return milliseconds
time_ms() {
    local start end
    start=$(date +%s%N)
    "$@" > /dev/null 2>&1
    end=$(date +%s%N)
    echo $(( (end - start) / 1000000 ))
}

# Helper: run benchmark and print results
print_result() {
    local name="$1" fides_ms="$2" c_ms="$3" fides_res="$4" c_res="$5"
    local match="OK"
    if [ "$fides_res" != "$c_res" ]; then
        match="MISMATCH(fides=$fides_res c=$c_res)"
    fi
    local ratio="?"
    if [ "$c_ms" -gt 0 ] 2>/dev/null; then
        ratio=$(awk "BEGIN { printf \"%.1f\", $fides_ms / $c_ms }")
    fi
    printf "  %-30s Fides: %5dms  C: %5dms  Ratio: %5sx  [%s]\n" \
        "$name" "$fides_ms" "$c_ms" "$ratio" "$match"
}

# Helper: compile both Fides and C versions, run, and report
# $1 = name, $2 = fd file, $3 = c file
run_bench() {
    local name="$1" fd_file="$2" c_file="$3"
    local fides_exe="$TMPDIR/${name}_fides"
    local c_exe="$TMPDIR/${name}_c"

    $FIDESC ccompile "$fd_file" -o "$fides_exe" > /dev/null 2>&1
    gcc -O2 -o "$c_exe" "$c_file" 2>/dev/null

    local f_result c_result f_ms c_ms
    f_result=$("$fides_exe" 2>/dev/null)
    c_result=$("$c_exe" 2>/dev/null)
    f_ms=$(time_ms "$fides_exe")
    c_ms=$(time_ms "$c_exe")
    print_result "ccompile" "$f_ms" "$c_ms" "$f_result" "$c_result"
}

# ─── Benchmark 1: Fibonacci (recursive, call-heavy) ───────────────────
echo "--- Benchmark 1: fib(30) recursive ---"

cat > "$TMPDIR/fib.fd" << 'EOF'
pf fib(n: i64) -> i64 {
    if n <= 1 { ret n; }
    ret fib(n - 1) + fib(n - 2);
}
pf main() -> i64 !IO {
    pv result = fib(30);
    print(result);
    ret 0;
}
EOF

cat > "$TMPDIR/fib.c" << 'EOF'
#include <stdio.h>
#include <stdint.h>
int64_t fib(int64_t n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}
int main() {
    printf("%lld\n", (long long)fib(30));
    return 0;
}
EOF

run_bench "fib" "$TMPDIR/fib.fd" "$TMPDIR/fib.c"

# ─── Benchmark 2: Loop sum (tight loop, arithmetic) ───────────────────
echo "--- Benchmark 2: sum 1..10000000 ---"

cat > "$TMPDIR/sum.fd" << 'EOF'
pf main() -> i64 !IO {
    pv total = 0;
    pv i = 0;
    while i < 10000000 {
        total = total + i;
        i = i + 1;
    }
    print(total);
    ret 0;
}
EOF

cat > "$TMPDIR/sum.c" << 'EOF'
#include <stdio.h>
#include <stdint.h>
int main() {
    int64_t total = 0;
    for (int64_t i = 0; i < 10000000; i++) {
        total += i;
    }
    printf("%lld\n", (long long)total);
    return 0;
}
EOF

run_bench "sum" "$TMPDIR/sum.fd" "$TMPDIR/sum.c"

# ─── Benchmark 3: Nested loop (matrix-like) ───────────────────────────
echo "--- Benchmark 3: nested loop 1000x1000 ---"

cat > "$TMPDIR/nested.fd" << 'EOF'
pf main() -> i64 !IO {
    pv total = 0;
    pv i = 0;
    while i < 1000 {
        pv j = 0;
        while j < 1000 {
            total = total + i * j;
            j = j + 1;
        }
        i = i + 1;
    }
    print(total);
    ret 0;
}
EOF

cat > "$TMPDIR/nested.c" << 'EOF'
#include <stdio.h>
#include <stdint.h>
int main() {
    int64_t total = 0;
    for (int64_t i = 0; i < 1000; i++) {
        for (int64_t j = 0; j < 1000; j++) {
            total += i * j;
        }
    }
    printf("%lld\n", (long long)total);
    return 0;
}
EOF

run_bench "nested" "$TMPDIR/nested.fd" "$TMPDIR/nested.c"

# ─── Benchmark 4: Function call overhead ──────────────────────────────
echo "--- Benchmark 4: function call overhead (1M calls) ---"

cat > "$TMPDIR/calls.fd" << 'EOF'
pf compute(n: i64) -> i64 {
    pv a = n * 2;
    pv b = a + 1;
    pv c = b * 3;
    pv d = c - 2;
    pv e = d * 4;
    ret e;
}
pf main() -> i64 !IO {
    pv total = 0;
    pv i = 0;
    while i < 1000000 {
        total = total + compute(i);
        i = i + 1;
    }
    print(total);
    ret 0;
}
EOF

cat > "$TMPDIR/calls.c" << 'EOF'
#include <stdio.h>
#include <stdint.h>
int64_t compute(int64_t n) {
    int64_t a = n * 2;
    int64_t b = a + 1;
    int64_t c = b * 3;
    int64_t d = c - 2;
    int64_t e = d * 4;
    return e;
}
int main() {
    int64_t total = 0;
    for (int64_t i = 0; i < 1000000; i++) {
        total += compute(i);
    }
    printf("%lld\n", (long long)total);
    return 0;
}
EOF

run_bench "calls" "$TMPDIR/calls.fd" "$TMPDIR/calls.c"

# ─── Benchmark 5: Conditional branches ────────────────────────────────
echo "--- Benchmark 5: branch-heavy (Collatz 1..10000) ---"

cat > "$TMPDIR/collatz.fd" << 'EOF'
@wrapping
pf collatz_steps(n: i64) -> i64 {
    pv steps = 0;
    pv val = n;
    while val > 1 {
        if val % 2 == 0 {
            val = val / 2;
        } else {
            val = 3 * val + 1;
        }
        steps = steps + 1;
    }
    ret steps;
}
pf main() -> i64 !IO {
    pv total = 0;
    pv i = 1;
    while i <= 10000 {
        total = total + collatz_steps(i);
        i = i + 1;
    }
    print(total);
    ret 0;
}
EOF

cat > "$TMPDIR/collatz.c" << 'EOF'
#include <stdio.h>
#include <stdint.h>
int64_t collatz_steps(int64_t n) {
    int64_t steps = 0;
    int64_t val = n;
    while (val > 1) {
        if (val % 2 == 0) {
            val = val / 2;
        } else {
            val = 3 * val + 1;
        }
        steps++;
    }
    return steps;
}
int main() {
    int64_t total = 0;
    for (int64_t i = 1; i <= 10000; i++) {
        total += collatz_steps(i);
    }
    printf("%lld\n", (long long)total);
    return 0;
}
EOF

run_bench "collatz" "$TMPDIR/collatz.fd" "$TMPDIR/collatz.c"

# ─── Benchmark 6: Compilation speed ───────────────────────────────────
echo ""
echo "--- Benchmark 6: Compilation speed ---"

F_COMPILE_NS=$(date +%s%N)
$FIDESC ccompile "$TMPDIR/calls.fd" -o "$TMPDIR/calls_compile" > /dev/null 2>&1
F_COMPILE_NS=$(( $(date +%s%N) - F_COMPILE_NS ))
F_COMPILE_MS=$(( F_COMPILE_NS / 1000000 ))

C_COMPILE_NS=$(date +%s%N)
gcc -O2 -o "$TMPDIR/calls_compile_c" "$TMPDIR/calls.c" 2>/dev/null
C_COMPILE_NS=$(( $(date +%s%N) - C_COMPILE_NS ))
C_COMPILE_MS=$(( C_COMPILE_NS / 1000000 ))

echo "  Fides ccompile: ${F_COMPILE_MS}ms"
echo "  gcc -O2:        ${C_COMPILE_MS}ms"

# ─── Summary ───────────────────────────────────────────────────────────
echo ""
echo "=== Summary ==="
echo "Ratio = Fides time / C time (lower is better, 1.0x = parity)"
echo ""
echo "Notes:"
echo "  - ccompile: Fides generates C code, compiled with gcc (default opts)"
echo "  - C baseline: hand-written C compiled with gcc -O2"
echo "  - The ccompile backend benefits from gcc's optimizer"
echo "  - Compilation speed: Fides adds C generation time on top of gcc"

# Cleanup
rm -rf "$TMPDIR"
