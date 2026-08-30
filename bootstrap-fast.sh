#!/bin/bash
# bootstrap-fast.sh — Fast iteration loop for bootstrap debugging
# Usage: ./bootstrap-fast.sh [test_file.fd] [--asan]
#
# Skips ASAN by default, uses -O0, only builds Rust compiler if src/*.rs changed.
# Typical iteration: ~28s (18s codegen + 10s gcc) instead of ~60s+

cd /home/medzer/pledge/p-lang

TEST_FILE="${1:-/tmp/hello.fd}"
USE_ASAN=false
if [ "$2" = "--asan" ]; then
    USE_ASAN=true
fi

OUTPUT="/tmp/fides-c"
C_FILE="/tmp/fides-c.c"

# Step 1: Check if Rust source changed — if so, rebuild
echo "=== Checking Rust compiler ==="
NEEDS_REBUILD=false
for f in src/lower.rs src/typechecker.rs src/codegen_c.rs src/main.rs src/parser.rs src/ast.rs src/ir.rs src/mono.rs src/borrowck.rs src/pir.rs src/opt.rs; do
    if [ "$f" -nt "target/release/fidesc" ] 2>/dev/null; then
        NEEDS_REBUILD=true
        break
    fi
done
if [ "$NEEDS_REBUILD" = true ]; then
    echo "Rust source changed, rebuilding..."
    cargo build --release 2>&1 | tail -1
else
    echo "Rust compiler up to date."
fi

# Step 2: Generate C from Fides compiler (cached if .fd files unchanged)
echo "=== Generating C (Rust → C) ==="
FD_HASH=$(cat compiler/*.fd stdlib/*.fd 2>/dev/null | md5sum | awk '{print $1}')
HASH_FILE="/tmp/fides-c.fdhash"
CACHED_HASH=""
if [ -f "$HASH_FILE" ]; then
    CACHED_HASH=$(cat "$HASH_FILE")
fi

if [ "$FD_HASH" = "$CACHED_HASH" ] && [ -f "$C_FILE" ]; then
    echo "  .fd files unchanged — reusing cached C ($C_FILE)"
else
    time ./target/release/fidesc ccompile compiler/main.fd -o "$OUTPUT" 2>&1 | head -1
    echo "$FD_HASH" > "$HASH_FILE"
fi

# Step 3: Compile C with tcc (0.25s — 80x faster than gcc)
echo "=== Compiling C (tcc) ==="
# Create tcc stdatomic.h stub
cat > /tmp/stdatomic.h << 'STUBEOF'
#include <stdint.h>
typedef int64_t atomic_int_least64_t;
typedef int atomic_int;
static inline void atomic_init(volatile int64_t* p, int64_t v) { *p = v; }
static inline int64_t atomic_load(volatile int64_t* p) { return *p; }
static inline void atomic_store(volatile int64_t* p, int64_t v) { *p = v; }
static inline int atomic_compare_exchange_strong(volatile int64_t* p, int64_t* expected, int64_t desired) { if (*p == *expected) { *p = desired; return 1; } *expected = *p; return 0; }
static inline int64_t atomic_fetch_add(volatile int64_t* p, int64_t v) { int64_t old = *p; *p += v; return old; }
static inline int64_t atomic_fetch_sub(volatile int64_t* p, int64_t v) { int64_t old = *p; *p -= v; return old; }
STUBEOF
time tcc -w -I/tmp -o "$OUTPUT" "$C_FILE" -lm 2>&1 | tail -3
if [ ! -f "$OUTPUT" ]; then
    echo "tcc failed, falling back to gcc -O0 -w..."
    gcc -O0 -w -o "$OUTPUT" "$C_FILE" -lm 2>&1 | tail -3
fi

# Step 4: Test
echo "=== Testing: $TEST_FILE ==="
"$OUTPUT" ccompile "$TEST_FILE" -o /tmp/test_output 2>&1
RET=$?
if [ $RET -eq 0 ]; then
    echo "SUCCESS!"
    /tmp/test_output 2>&1 | head -5
else
    echo "FAILED (exit $RET)"
    if [ "$USE_ASAN" = true ]; then
        echo "Rebuilding with ASAN (gcc) for crash details..."
        gcc -O0 -g -fsanitize=address -Wno-int-conversion -Wno-incompatible-pointer-types -o "${OUTPUT}-asan" "$C_FILE" -lm 2>/dev/null
        "${OUTPUT}-asan" ccompile "$TEST_FILE" -o /tmp/test_output 2>&1 | head -20
    else
        echo "Run with --asan for crash details"
    fi
fi
