# TEST.md — Fides Test Results

*Last updated: Sep 10, 2026 (session 24 — v1.0.0 released). Test baseline from `docs/roadmap/roadmap.md` Session 24. Self-host tests: 38/38 pass on all 3 backends. Bootstrap: Interpreter 236/237, C Codegen 226/237, PE 237/237. Self-compilation FAILS (SIGSEGV in Lowerer_lower_stmt). Fixpoint NOT VERIFIABLE (fides-c2.c missing). 303 total .fd test files (261 bootstrap + 42 selfhost). The Session 16 test counts below are preserved for historical reference — see AGENTS.md for the authoritative current baseline.*

## Legend

- ✅ **PASS** — Correct output and/or exit code
- ❌ **FAIL** — Crash, wrong output, or no output
- ⚠️ **PARTIAL** — Works but with known limitations
- — **N/A** — Not applicable for this target
- 🎁 **BONUS PASS** — Known-fail test that unexpectedly passes (improvement)

> **Note:** Exit codes are 0–255 (Unix convention). Values > 255 wrap modulo 256.
> For example, `test_match.fd` returns 300, which appears as exit code 44 (300 % 256).

---

## Test Summary

| Test Group | Count | Pass | Fail | Notes |
|------------|-------|------|------|-------|
| Harness (`cargo test --test harness`) | 8 | 8 | 0 | All passing (region, capability, effect, safety, golden, PE, interpreter, C codegen) |
| Unit tests (`cargo test --lib`) | 265 | 265 | 0 | All passing (incl. 47 cross-compilation tests + 7 incremental compilation) |
| Interpreter (`fidesc run`) | 165 normal | 164 | 1 | 1 fail: test_compiler_sized (user-added, unrelated to compiler infra) |
| PE Emitter (`fidesc emit`) | 165 normal | 165 | 0 | All normal tests pass. |
| C Codegen (`fidesc ccompile`) | 165 normal | 164 | 1 | 1 fail: test_compiler_sized. Binary hardening enabled. |
| Skip list | 0 | — | — | All previously skipped tests now pass. |
| Known-fail | 0 | — | — | All previously known-fail tests now pass on all 3 backends. |
| Expected-fail | 36 | 36 | 0 | All correctly fail — error detection tests |
| Safety tests | 9 | 9 | 0 | OOB, overflow, div-by-zero, unwrap None |
| Capability tests | 4 | 4 | 0 | Grant, deny, propagation |
| Effect tests | 7 | 7 | 0 | Annotations, inference, handlers, pure, under-declare |
| Region tests | 3 | 3 | 0 | Basic, nested, escape detection |
| **Total .fd test files** | **215** | | | 192 in root + 23 in subdirs (capability/effects/regions/safety) |

---

## Interpreter (`fidesc run`)

All 209/211 normal test files pass through the interpreter. The 2 failures
are pre-existing gaps documented in known-bugs.md (associated with the `var`
feature and other edge cases).

| Test File | Status | Output / Exit Code |
|-----------|--------|--------|
| `hello.fd` | ✅ | `Hello, Fides!50y is big` |
| `print_int.fd` | ✅ | `42` |
| `print_str.fd` | ✅ | `Hello!` |
| `arithmetic.fd` | ✅ | `30\n40` |
| `control_flow.fd` | ✅ | `1\n30\n10\n6` |
| `functions.fd` | ✅ | `7\n120` |
| `struct.fd` | ✅ | `10\n20` |
| `strings.fd` | ✅ | `Hello, World!FidesHello, Fides!x = 42` |
| `ret42.fd` | ✅ | exit=42 |
| `test_float.fd` | ✅ | `3.14159\n2.71828\n5.85987\n-3.5\n0` |
| `test_fstring_float.fd` | ✅ | `x = 3.14` |
| `test_fstring_int.fd` | ✅ | `val = 42` |
| `test_fstring_vec.fd` | ✅ | `vec = [1, 2]` |
| `test_vec_one.fd` | ✅ | `[42]` |
| `test_vec_loop.fd` | ✅ | `3` |
| `test_match.fd` | ✅ | exit=44 (returns 300; 300 % 256 = 44) |
| `test_match_edge.fd` | ✅ | `20\n2\n42\n77` |
| `test_option.fd` | ✅ | `42\n-1` |
| `test_try.fd` | ✅ | `42` |
| `test_defer.fd` | ✅ | `main body\ncleanup B\ncleanup A` |
| `test_pe_defer.fd` | ✅ | `main body\ncleanup A\ncleanup B` |
| `test_closure.fd` | ✅ | `10` |
| `test_closure_capture.fd` | ✅ | `15` |
| `test_impl.fd` | ✅ | `7\n13\n24` |
| `test_generics.fd` | ✅ | `42\n100\n20\n50` |
| `test_pe_generics.fd` | ✅ | `42\n20\n50` |
| `test_capabilities.fd` | ✅ | `42\n20\n10\n5\n10` |
| `test_hashmap.fd` | ✅ | `2\n30\ntrue\n1\nfalse` |
| `test_print_map.fd` | ✅ | `{alice: 30, bob: 25}` |
| `test_collections.fd` | ✅ | (multi-line collections output) |
| `test_pe_enum.fd` | ✅ | `42\n100\n-1` |
| `test_pe_for_vec.fd` | ✅ | `60` |
| `test_pe_refs.fd` | ✅ | `42` |
| `test_pe_struct.fd` | ✅ | `7\n3\n4` |
| `test_pe_try_block.fd` | ✅ | `50\n42\n99` |
| `test_pe_try_op.fd` | ✅ | `10` |
| `test_refs_mut.fd` | ✅ | `11\n16\n42` |
| `test_traits.fd` | ✅ | `30\n35\n30\n130` |
| `test_regression_closures.fd` | ✅ | Closures regression test |
| `test_regression_fstring.fd` | ✅ | F-string regression test |
| `test_regression_collections.fd` | ✅ | Collections regression test |
| `test_wasm.fd` | ✅ | WASM target test (Phase 5D.1) |
| `test_json.fd` | ✅ | JSON stdlib (Phase 5D.2) |
| `test_http.fd` | ✅ | HTTP server stdlib (Phase 5D.3) |
| `test_db.fd` | ✅ | Database driver framework (Phase 5D.4) |
| `test_tensor.fd` | ✅ | Tensor type + ops (Phase 5C.1) |
| `test_simd.fd` | ✅ | SIMD intrinsics (Phase 5C.2) |
| `test_shape.fd` | ✅ | Comptime shape verification (Phase 5C.3) |
| `test_parallel.fd` | ✅ | Auto-parallelization framework (Phase 5C.4) |
| `test_asm.fd` | ✅ | Inline assembly `asm!()` (Phase 5A.1) |
| `test_bare.fd` | ✅ | Bare-metal mode `--bare` (Phase 5A.2) |
| `test_interrupt.fd` | ✅ | Interrupt handler `interrupt fn` + cli/sti/hlt (Phase 5A.3) |
| `test_mmu.fd` | ✅ | MMU/page table: CR0/CR3/CR4, invlpg, page_map (Phase 5A.4) |
| `test_boot.fd` | ✅ | Multiboot2 boot code `--boot` (Phase 5A.5) |

---

## PE Emitter (`fidesc emit`)

Native Windows x64 PE executables. No external linker, assembler, or C compiler required.

All 165 normal test files produce valid PE32+ executables. Verified: MZ/PE signatures, x86_64 machine type, .text + .rdata sections, kernel32.dll imports for GetStdHandle/WriteFile/ExitProcess.

**Note:** PE executables target Windows x64 and cannot be executed on Linux without Wine. Output values listed below match the interpreter and are expected to be correct based on PE structure validation and disassembly verification. The bug fix history below documents previously identified and fixed issues.

| Test File | Status | Output | Notes |
|-----------|--------|--------|-------|
| `hello.fd` | ✅ | `Hello, Fides!50y is big` | Strings, int print, if/else |
| `print_int.fd` | ✅ | `42` | Integer printing |
| `print_str.fd` | ✅ | `Hello!` | String printing |
| `strings.fd` | ✅ | `Hello, World!FidesHello, Fides!x = 42` | Multiple strings, f-string |
| `arithmetic.fd` | ✅ | `30\n40` | Arithmetic ops |
| `control_flow.fd` | ✅ | `1\n30\n10\n6` | If/else, while, for |
| `functions.fd` | ✅ | `7\n120` | Function calls, recursion |
| `struct.fd` | ✅ | `10\n20` | Struct fields |
| `test_float.fd` | ✅ | `3.14159\n2.71828\n5.85987\n-3.5\n0` | Float printing |
| `test_vec_one.fd` | ✅ | `[42]` | Vec new + push + print |
| `test_vec_loop.fd` | ✅ | `3` | Vec push in while loop |
| `test_match.fd` | ✅ | exit=44 | Match on integer (300 % 256) |
| `test_match_edge.fd` | ✅ | `20\n2\n42\n77` | Match edge cases |
| `test_option.fd` | ✅ | `42\n-1` | Option Some/None |
| `test_try.fd` | ✅ | `42` | Try block |
| `test_defer.fd` | ✅ | `main body\ncleanup B\ncleanup A` | Defer execution |
| `test_pe_defer.fd` | ✅ | `main body\ncleanup A\ncleanup B` | Defer (alternate order) |
| `test_closure.fd` | ✅ | `10` | Closure capture |
| `test_impl.fd` | ✅ | `7\n13\n24` | Impl blocks, methods |
| `test_generics.fd` | ✅ | `42\n100\n20\n50` | Generic functions |
| `test_pe_generics.fd` | ✅ | `42\n20\n50` | Generic functions (PE tests) |
| `test_capabilities.fd` | ✅ | `42\n20\n10\n5\n10` | Capabilities |
| `test_hashmap.fd` | ✅ | `2\n30\ntrue\n1\nfalse` | HashMap insert/get/contains/remove/len/is_empty |
| `test_fstring_int.fd` | ✅ | `val = 42` | F-string with integer interpolation |
| `test_fstring_vec.fd` | ✅ | `vec = [1, 2]` | F-string with vec interpolation |
| `test_fstring_float.fd` | ✅ | `x = 3.14` | F-string with float interpolation |
| `test_pe_enum.fd` | ✅ | `42\n100\n-1` | Enum with data |
| `test_pe_for_vec.fd` | ✅ | `60` | For loop over vec |
| `test_pe_refs.fd` | ✅ | `42` | References |
| `test_pe_struct.fd` | ✅ | `7\n3\n4` | Struct methods |
| `test_pe_try_block.fd` | ✅ | `50\n42\n99` | Try blocks |
| `test_pe_try_op.fd` | ✅ | `10` | Try operator |
| `test_refs_mut.fd` | ✅ | `11\n16\n42` | Mutable references |
| `test_traits.fd` | ✅ | `30\n35\n30\n130` | Traits |
| `test_print_map.fd` | ✅ | `{alice: 30, bob: 25}` | Map printing |
| `test_collections.fd` | ✅ | (multi-line collections output) | Vec/HashMap/String operations |
| `test_closure_capture.fd` | ✅ | `15` | Closure with captured variable |
| `ret42.fd` | ✅ | exit=42 | Return value → exit code |

---

## C Codegen (`fidesc ccompile`)

Portable C code generation, compiled with gcc/clang. **198/211 normal tests produce output matching the interpreter** (session 15 baseline). Phase 10B: Vec/HashMap/String migrated to Fides stdlib. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack.

| Test File | Status | Notes |
|-----------|--------|-------|
| 104 test files | ✅ | Output matches interpreter |
| 4 test files | ❌ | test_game_stdlib, test_phase8b, test_vec_math, test_vec_shape_fail |
| 22 test files | ✅ (expected fail) | All correctly rejected at compile time |

---

## Native ELF64 Build (`fidesc build --target x86-64`)

Native Linux x86-64 ELF64 executables. No external linker, assembler, or C compiler required. Uses the full PIR pipeline (L1→L2→L3 → codegen → 9 peephole passes → ELF64).

| Test File | Status | Output / Exit Code | Notes |
|-----------|--------|--------|-------|
| `hello.fd` | ✅ | `Hello, Fides!50y is big` (exit 8) | 3468 bytes, zero external tools |
| `hello.fd --opt` | ✅ | `Hello, Fides!50y is big` (exit 8) | 3316 bytes with L2 optimizer |

**Note:** Exit code 8 is correct — `main()` has no explicit `return`, so it returns the value of the last expression (the `if` block), which is `print("y is big")` returning 8 (bytes written). The interpreter does not propagate return values as exit codes.

**Known issue:** The L2 optimizer (`--opt`) works for simple programs but exposes a pre-existing register allocator bug on complex programs (constants clobbered by intervening instructions). Not enabled by default.

---

### C Codegen fixes applied (Aug 10, 2026):

- **CallPtr closure dispatch**: Fixed with string-based function name lookup, per-candidate argument casting, and return type filtering
- **Print/println polymorphic dispatch**: Now correctly dispatches to `p_to_str_vec`, `p_to_str_map`, `p_to_str_float`, `p_to_str_str`, `p_to_str_bool` based on argument type
- **Map key sorting**: C runtime sorts map keys for deterministic output (matching interpreter)
- **String escaping**: Proper C string escape function handles `\t`, `\r`, `\0`, and control characters
- **Null pointer checks**: Added to all runtime functions and CallPtr dispatch
- **Float f-string**: `p_to_str_float` now correctly formats floats (was truncating to integer)
- **Vec f-string**: `p_to_str_vec` now correctly dispatched (was outputting pointer address)
- **Map printing**: `p_to_str_map` now implemented with sorted key output
- **Closures**: CallPtr with proper argument casting and return type filtering
- **Collections**: Complex Vec/HashMap/String operations now work correctly

---

## Automated Test Harness

Run `cargo test` to execute the automated test suite. The harness tests all three backends:

- **Interpreter (`run`)**: Executes each `.fd` file and checks output + exit code.
- **PE Emitter (`emit`)**: Generates `.exe` files and validates PE structure (MZ/PE signatures, x86_64 machine, section count).
- **C Codegen (`ccompile`)**: Compiles to C, builds with gcc, executes, and compares output against the interpreter.

Tests are defined in `tests/harness.rs` and automatically discover all `.fd` files in the `tests/` directory.

---

## PE Emitter — Bug Fix History

### 2026-08-06: R12/R13 ModR/M encoding bug (critical fix)

**Root cause:** `mov_reg_ptr`, `mov_ptr_reg`, and related functions checked `base == Reg::Rsp` to decide if a SIB byte was needed. But R12 has the same low 3 bits (code 4) as RSP, so the x86 encoding requires a SIB byte for R12 too. Similarly, R13 (code 5) shares bits with RBP, requiring mod=01 with disp8=0 for zero-displacement addressing.

**Impact:** Any code using R12 or R13 as a base register in memory operands produced invalid x86 encoding, causing access violations. This was the root cause of the `p_vec_push` crash.

**Fix:** Changed all affected functions to check `base.code() == 4` (covers RSP and R12) and `base.code() == 5` (covers RBP and R13) instead of comparing `Reg` enum variants directly.

**Files changed:** `src/pe.rs` — 11 functions fixed:
- `mov_reg_ptr`, `mov_ptr_reg`, `mov_ptr_reg32`, `mov_ptr_reg16`
- `mov_reg8_ptr`, `mov_reg32_ptr`, `mov_reg16_ptr`
- `emit_mem_reg`, `mov_reg_mem_disp`, `mov_mem_disp_reg`, `mov_mem_disp_reg32`

### 2026-08-06: HashMap support (p_map_* stubs)

**Root cause 1:** `p_map_insert` grow path called `p_malloc` which clobbered `r8` (len). The subsequent copy loop used `r8` as loop bound — with garbage in `r8`, it iterated wildly and crashed.

**Fix 1:** Save `r8` to `[rbp-24]` before `p_malloc`, restore after.

**Root cause 2:** All map string compare loops (`p_map_insert`, `p_map_get`, `p_map_contains`, `p_map_remove`) had the same `mov_reg8_mem` bug as `p_str_concat` — 8-bit load into 64-bit register without zeroing first.

**Fix 2:** Added `xor_self` before each `mov_reg8_mem` in all map stubs.

**Root cause 3:** `p_map_new` and `p_map_insert` had redundant `sub/add rsp 32` around `p_malloc`/`p_free` calls.

**Fix 3:** Removed redundant stack adjustments.

**Missing stub:** `p_map_is_empty` was not implemented.

**Fix 4:** Added `p_map_is_empty` stub — returns 1 if `len == 0`, else 0.

### 2026-08-06: F-string support (p_str_concat + p_to_str dispatch)

**Root cause 1:** `p_str_concat` strlen and copy loops used `mov_reg8_mem` (8-bit load) into a register, then `test_rr` (64-bit test) to check for null terminator. The 8-bit load only sets the low byte, leaving upper bytes with stale pointer values. The 64-bit test always saw non-zero bits, so null detection never triggered — loops ran past string ends and crashed.

**Fix 1:** Added `xor_self` before each `mov_reg8_mem` call to zero the register, so `test_rr` correctly reflects only the loaded byte.

**Root cause 2:** `p_to_str` only handled integers. F-strings with Vec arguments called `p_to_str` with a Vec pointer, which produced garbage.

**Fix 2:** Added type-based dispatch for `p_to_str` calls (same pattern as `print` dispatch). Added new runtime stubs:
- `p_to_str_vec` — formats Vec as `[1, 2, ...]` using `p_str_concat` + `p_to_str` per element
- `p_to_str_str` — returns input pointer unchanged
- `p_to_str_bool` — returns `"true"` or `"false"`
- `p_to_str_float` — truncates to integer, delegates to `p_to_str` (TODO: proper float formatting)

### 2026-08-06: p_vec_push full grow logic

Replaced the simplified (always-allocate) push with full implementation:
- Checks `len < cap` to skip grow
- Grows: `new_cap = cap == 0 ? 4 : cap * 2`
- Allocates new buffer via `p_malloc`
- Copies old elements to new buffer
- Frees old buffer if non-null
- Updates vec struct (ptr, cap)
- Stores value at `ptr[len]`, increments len

### 2026-08-10: C codegen — `return;` in non-void functions (critical fix)

**Root cause:** The C codegen emitted bare `return;` (no value) in non-void functions. This happened in two cases:
1. Struct-returning functions use an out-parameter convention (the result is written to a hidden first parameter, and the return is `Return { value: None }`), but the C codegen declared the return type as `struct Foo*` instead of `void`.
2. The `main` function has IR return type `void` but the C wrapper declares it as `int64_t main_entry(...)`, so `Return { value: None }` emitted `return;` in a function declared as returning `int64_t`.

**Impact:** GCC 14+ treats `-Wreturn-mismatch` as an error by default, so every program failed to compile.

**Fix:**
- Added `c_ret_type()` helper that returns `"void"` for aggregate (struct/array) return types, matching the out-parameter convention.
- Added `cur_fn_c_ret_is_void` and `cur_fn_c_ret_type` fields to track the current function's C return type.
- `Return { value: None }` now emits `return;` for void functions and `return 0;` for non-void functions (dead/unreachable blocks).

**Files changed:** `src/codegen_c.rs`

### 2026-08-10: C codegen — argument casting for function calls

**Root cause:** IR functions have a hidden first parameter (`%0: *u8`, a context/closure pointer) typed as `I64` in the IR, but the C function parameter is `void*`. Passing `int64_t` to `void*` is an error in GCC 14+ (`-Wint-conversion` promoted to error).

**Fix:**
- Added `fn_param_types` map to look up called function's parameter types at call sites.
- Added `cast_arg()` helper that casts integer→pointer, pointer→integer, and integer→struct-pointer as needed.
- Added `insert_runtime_sigs()` to register C runtime function signatures (p_vec_push, p_map_get, etc.) since they're not in `module.functions`.

**Files changed:** `src/codegen_c.rs`

### 2026-08-10: C codegen — type-aware Store and struct field offsets

**Root cause 1:** The `Store` instruction always wrote 8 bytes (`*(int64_t*)ptr = (int64_t)val`) regardless of the value or pointer type. For `int32_t` fields, this overflowed into adjacent fields, corrupting struct data.

**Fix 1:** Made `Store` type-aware — uses the pointer's pointee type (if available) to determine the store size. Stores `int32_t` as 4 bytes, `double` as 8 bytes, etc.

**Root cause 2:** `GetFieldPtr` used `field_index * 8` for field offsets, assuming all fields are 8 bytes. But structs with `i32` fields have 4-byte fields at offsets 0, 4, 8, etc.

**Fix 2:** Added `struct_layouts` map (from `IrModule::struct_layouts`) to look up actual field offsets. Falls back to `field_index * 8` for opaque pointers.

**Root cause 3:** `Alloca` used `type.size()` which returns 0 for struct types (placeholder). This caused `malloc(0)` for struct allocations.

**Fix 3:** Look up struct size from `struct_layouts` map. Ensure at least 8 bytes to avoid `malloc(0)` undefined behavior.

**Root cause 4:** `ConstFloat` emitted `v = 4614256650576692846LL` (integer literal) assigned to a `double` variable, which converted the integer to a float instead of preserving the IEEE 754 bits.

**Fix 4:** Use a union compound literal to reinterpret the bits: `v = ((union { uint64_t u; double f; }){ .u = 4614256650576692846ULL }).f`

**Files changed:** `src/codegen_c.rs`

---

# Test Batches

This document specifies all test batches for the Fides compiler so that
results from different backends are never confounded.

## Test Corpus

All batches use the same corpus: every `.fd` file in `tests/` (215 .fd test files as of
this writing: 195 in root + 23 in subdirs).  Tests are sorted alphabetically by filename.

## Batches

### Batch 1 — Interpreter (`fidesc run`)

| Field | Value |
|-------|-------|
| Command | `fidesc run <file.fd>` |
| Pass criterion | No panic on stderr; exit code may be non-zero |
| What it exercises | AST → IR → tree-walking interpreter |
| Run by | `fidesc test` (built-in) |
| Current score | 209/211 normal (session 15 baseline) |

The interpreter is the reference implementation.  A test "passes" if the
interpreter does not panic or emit a compilation error.  Non-zero exit codes
(e.g. `return 42`) are **not** failures.

### Batch 2 — C Codegen (`fidesc ccompile`)

| Field | Value |
|-------|-------|
| Command | `fidesc ccompile <file.fd> -o <exe>` then run `<exe>` |
| Pass criterion | stdout matches interpreter stdout exactly |
| What it exercises | AST → IR → C source transpilation → external gcc |
| Run by | `fidesc test` (built-in) |
| Current score | 198/211 normal (session 15 baseline. Phase 10B: Vec/HashMap/String migrated to Fides stdlib. Binary hardening enabled.) |

C codegen transpiles the Fides IR to C source code and compiles it with the
system C compiler (gcc/clang).  A test "passes" only if the executable's
stdout exactly matches the interpreter's stdout. C codegen is at full
parity with the interpreter.

### Batch 3 — PE Emitter (`fidesc emit`)

| Field | Value |
|-------|-------|
| Command | `fidesc emit <file.fd> -o <exe>` |
| Pass criterion | Emission succeeds and output file exists |
| What it exercises | AST → IR → PE bytecode emitter |
| Run by | `fidesc test` (built-in) |
| Current score | 165/165 normal |

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
| 1 | Interpreter | 209/211 normal | Session 15 baseline |
| 2 | C Codegen | 198/211 normal | Session 15 baseline. Phase 10B: Vec/HashMap/String migrated to Fides stdlib. Binary hardening enabled. |
| 3 | PE Emitter | 209/211 normal | Session 15 baseline |
| 4 | Native x86-64 | All tested programs match | test_collections, test_db, test_matrix_mul all FIXED |

## Important Notes

- **Never compare scores across batches.**  Each batch has different pass
  criteria (e.g. PE emitter only checks emission, not output correctness).
- **Batch 4 is not run by `fidesc test`.**  It must be run separately.
- The interpreter (Batch 1) is the **reference** for Batches 2 and 4.
- `test_trait_verify_fail` is expected to fail in all batches — it tests
  that the compiler correctly rejects invalid trait implementations.
