# Test Batches

This document specifies all test batches for the Fides compiler so that
results from different backends are never confounded.

## Test Corpus

All batches use the same corpus: every `.fd` file in `tests/` (149 files as of
this writing: 127 normal + 22 expected-fail).  Tests are sorted alphabetically by filename.

## Batches

### Batch 1 — Interpreter (`fidesc run`)

| Field | Value |
|-------|-------|
| Command | `fidesc run <file.fd>` |
| Pass criterion | No panic on stderr; exit code may be non-zero |
| What it exercises | AST → IR → tree-walking interpreter |
| Run by | `fidesc test` (built-in) |
| Current score | 102/127 + 22/22 expected-fail |

The interpreter is the reference implementation.  A test "passes" if the
interpreter does not panic or emit a compilation error.  Non-zero exit codes
(e.g. `ret 42`) are **not** failures.  `test_trait_verify_fail`,
`test_budget_fail`, and `test_sandbox_fail` are expected-fail
tests (correctly rejected at compile time).

### Batch 2 — C Codegen (`fidesc ccompile`)

| Field | Value |
|-------|-------|
| Command | `fidesc ccompile <file.fd> -o <exe>` then run `<exe>` |
| Pass criterion | stdout matches interpreter stdout exactly |
| What it exercises | AST → IR → C source transpilation → external gcc |
| Run by | `fidesc test` (built-in) |
| Current score | 101/127 + 22/22 expected-fail (near full parity — test_phase8b fails: ECS __ecs_* not in C codegen) |

C codegen transpiles the Fides IR to C source code and compiles it with the
system C compiler (gcc/clang).  A test "passes" only if the executable's
stdout exactly matches the interpreter's stdout. C codegen is at near full
parity with the interpreter — test_phase8b fails (ECS __ecs_* not in C codegen).

### Batch 3 — PE Emitter (`fidesc emit`)

| Field | Value |
|-------|-------|
| Command | `fidesc emit <file.fd> -o <exe>` |
| Pass criterion | Emission succeeds and output file exists |
| What it exercises | AST → IR → PE bytecode emitter |
| Run by | `fidesc test` (built-in) |
| Current score | 102/127 + 22/22 expected-fail |

The PE (Portable Executable) emitter produces a standalone native x86-64
executable. A test "passes" if emission succeeds and the output file is
created.  It does **not** run the executable or compare output (PE binaries
cannot run on Linux without Wine).

### Batch 4 — Native x86-64 (`fidesc build`)

| Field | Value |
|-------|-------|
| Command | `fidesc build <file.fd> -o <exe>` then run `<exe>` |
| Pass criterion | stdout matches interpreter stdout exactly |
| What it exercises | PIR → L2 → L3 → x86-64 asm → ELF object → linker → native exe |
| Run by | Custom script (`/tmp/test_native.sh`) — **not** `fidesc test` |
| Current score | All tested programs match interpreter (test_collections, test_db, test_matrix_mul all FIXED) |

The native x86-64 backend is a self-contained toolchain: it generates x86-64
assembly, encodes it into an ELF relocatable object, and links it with a
pre-compiled Linux runtime (raw machine code stubs).  No external assembler
or linker is used.

A test "passes" only if the native executable's stdout exactly matches the
interpreter's stdout.

#### Known issue categories (Batch 4 only)

1. **Missing runtime stubs** — `p_vec_push`, `p_vec_len`, `p_vec_get`,
   `p_hashmap_*`, `p_json_*`, `p_http_*`, `p_db_*`, etc. are stubbed as
   no-ops or return 0.  Tests using these produce empty or wrong output.

2. **Register allocator** — caller-saved registers clobbered across calls,
   pre-assigned arg registers not spilled.  (Partially fixed.)

3. **ELF assembler** — `imulq` operand swap (fixed), `movzbq` 8-bit
   register handling (fixed), other encoding edge cases may remain.

4. **Peephole optimizer** — `pass_lea_coalesce` not clearing state across
   `call` instructions (fixed), `pass_store_load_forward` matching
   register-to-register moves (fixed).

5. **Struct/enum layout** — field offsets now use 8-byte uniform slots
   (fixed), but enum variant data access may still have issues.

## Running Each Batch

### Batches 1–3 (built-in)

```bash
cargo build --release
./target/release/fidesc test
```

### Batch 4 (native x86-64)

```bash
#!/bin/bash
cd /path/to/p-lang
pass=0; fail=0
for f in tests/*.fd; do
    name=$(basename "$f" .fd)
    interp_out=$(cargo run --quiet -- run "$f" 2>/dev/null)
    cargo run --quiet -- build "$f" -o "/tmp/test_$name" 2>/dev/null
    [ -f "/tmp/test_$name" ] || { fail=$((fail+1)); echo "FAIL: $name (build)"; continue; }
    chmod +x "/tmp/test_$name"
    native_out=$("/tmp/test_$name" 2>/dev/null)
    if [ "$interp_out" == "$native_out" ]; then
        pass=$((pass+1)); echo "PASS: $name"
    else
        fail=$((fail+1)); echo "FAIL: $name"
    fi
done
echo "=== Results: $pass pass, $fail fail ==="
```

## Scoreboard

| Batch | Backend | Score | Notes |
|-------|---------|-------|-------|
| 1 | Interpreter | 102/127 + 22/22 expected-fail | Pre-existing failures: test_game_stdlib, test_vec_math, test_vec_shape_fail |
| 2 | C Codegen | 101/127 + 22/22 expected-fail | Near full parity — test_phase8b fails (ECS __ecs_* not in C codegen). test_db no longer flaky. |
| 3 | PE Emitter | 102/127 + 22/22 expected-fail | Same pre-existing failures as interpreter |
| 4 | Native x86-64 | All tested programs match | test_collections, test_db, test_matrix_mul all FIXED (was 63/66) |

## Important Notes

- **Never compare scores across batches.**  Each batch has different pass
  criteria (e.g. PE emitter only checks emission, not output correctness).
- **Batch 4 is not run by `fidesc test`.**  It must be run separately.
- The interpreter (Batch 1) is the **reference** for Batches 2 and 4.
- `test_trait_verify_fail` is expected to fail in all batches — it tests
  that the compiler correctly rejects invalid trait implementations.
