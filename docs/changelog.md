# P Language Changelog

## [Unreleased] — Documentation Audit + Roadmap Accuracy Pass

### Documentation Updates
- **`docs/roadmap.md`**: Full rewrite for accuracy — fixed inconsistencies between overview status labels and section headers (Phase 3/4/5 said "MOSTLY COMPLETE" in overview but "Complete" in section headers). Added "What's Left" section to each phase. Added "Summary: Are Phases 1–5 Fully Done?" table. Added "Recommended Next Steps" priority list. Updated all test counts to verified Aug 2026 baseline.
- **`README.md`**: Updated test results table — 136/136 unit tests (was 133), 8/8 harness tests (was 7/8 — `golden_files_match` now passes), 78/78 interpreter + 1/1 expected-fail (was 78/79), 76/78 C codegen + 1/1 expected-fail (was 75/79). Updated phase status to reflect Phases 1–5 actual state.
- **`features.md`**: Updated test counts in header and C codegen row.
- **`TEST.md`**: Updated test summary table and per-backend sections with accurate counts. Documented `test_db` flakiness (HashMap ordering non-deterministic).

### Verified Test Baseline (Aug 2026)
- **Unit tests**: 136/136 pass
- **Harness tests**: 8/8 pass (golden_files_match now passes — was previously failing)
- **Interpreter**: 78/78 normal + 1/1 expected-fail
- **PE Emitter**: 78/78 normal + 1/1 expected-fail
- **C Codegen**: 76/78 normal + 1/1 expected-fail (2 real mismatches: test_asm by design, test_comptime_reflect interpreter-only; test_db flaky due to HashMap ordering)
- **Total .p test files**: 102 (79 root + 23 subdirs)
- **Source**: 35 .rs files, 47,397 LOC

### Phase Status Summary
- **Phase 1 — Foundation**: COMPLETE, nothing left
- **Phase 2 — Safety**: COMPLETE (interpreter), C codegen coverage gap for runtime allocator/effect-handler dispatch
- **Phase 3 — Power**: COMPLETE (interpreter), C codegen gap for comptime reflection
- **Phase 4 — Performance**: MOSTLY COMPLETE — regalloc bug blocks --opt by default, GetFieldPtr offset workaround
- **Phase 5 — Platform**: MOSTLY COMPLETE — all features work in interpreter, C codegen coverage partial, test_db flaky

## [Previous] — Optimization Improvements + Documentation Audit + Safety Net

### Verification Infrastructure
- **`verify.sh`**: Build + test + compare to baseline. Prevents "fix one thing, break everything" by diffing test results against a known-good baseline.
- **`scripts/gen_golden.sh`**: Generate golden files for 15 key tests at each compiler stage (IR, PIR L1/L2/L3, assembly, peephole, output).
- **`scripts/bisect.sh`**: Bisect which compiler layer introduced a regression by diffing each stage against golden files. Normalizes non-deterministic output (HashMap ordering, stack slot offsets, string constant numbering).
- **`AGENTS.md`**: Working guide for AI assistants and humans — build commands, high-risk files, known bugs, coding conventions, debugging workflow.
- **`docs/best-practices.md`**: 15 sections of hard-won insights from building the custom toolchain — register allocator traps, peephole correctness rules, ABI awareness, testing discipline, non-deterministic output issues.

### Register Allocator Improvements (regalloc.rs)
- **Pre-assigned arg register spilling**: When a function parameter is in a caller-saved register and a call clobbers it during the parameter's live range, spill the parameter to the stack. Updates `arg_locations` so the codegen prologue saves the arg register to the spill slot.
- **Spill offset alignment fix**: `spill_offset` now starts at `func.frame_size + 8` to match the codegen prologue's stack layout expectation.

### Peephole Optimizer Fixes (peephole.rs)
- **LEA coalescing across calls**: `pass_lea_coalesce` now clears tracking state on `call`/`callq` instructions. Calls clobber caller-saved registers, invalidating LEA→register forwarding.
- **Dead store elimination safety**: `pass_store_load_forward` now checks if the next instruction reads from the destination as a memory address before eliminating a store. Prevents incorrect elimination of stores needed for address-dependent loads.

### Codegen Improvements (codegen_x64.rs)
- **Closure environment passing**: `gen_call_ptr` now properly handles `has_env` flag — loads function pointer from `env[0]`, passes env pointer as first argument (RDI), shifts user args to start at RSI.
- **`Operand::ImmFunc` support**: Function addresses loaded via RIP-relative LEA for both register and stack slot destinations.

### Runtime (runtime_linux.rs)
- **`p_print_vec` implementation**: Hand-encoded x86-64 machine code for printing vectors in `[1, 2, 3]` format. Uses callee-saved registers (RBX, R12, R13) to preserve state across `write` syscalls and `p_print_int` calls.

### Documentation Updates
- **"Pledge" → "P" rename**: All documentation updated to use "P" as the language name (14 files). `pledgec` kept as compiler binary name. `PledgeOS` preserved as separate product name. `changelog.md` kept as "Pledge" for historical accuracy.
- **`docs/custom-toolchain.md`**: Full rewrite from aspirational Phase 1 to actual Phase 4D state — all 10 sections updated with real status.
- **`docs/full-stack-architecture.md`**: Full rewrite — architecture diagram, PIR levels, codegen backends, linker, runtime, stdlib, embedded/IoT, hot reload, C FFI, bootstrapping strategy.
- **`docs/syntax-spec.md`**: Targeted updates — version 0.5.0, keyword list from token.rs, stdlib types (removed non-existent Box/Rc/Arc), effect handlers status (partial), comptime reflection, attributes status (#[test]/#[derive] honest), concurrency status (parsed-only), capability security (actual syntax), hot reload (actual implementation).
- **C codegen test parity**: Improved from 53/79 to 75/79.

## [Previous] — Optimization Improvements + Documentation Audit

### Peephole Optimizer — 3 New Passes (peephole.rs)
- **Store-to-load forwarding**: `movq %src, -N(%rbp)` followed by `movq -N(%rbp), %dst` → replace load with `movq %src, %dst`. Eliminates redundant memory traffic.
- **LEA coalescing**: `leaq -N(%rbp), %reg` repeated with same address and destination → remove duplicates. Tracks last LEA and skips redundant recomputations.
- **Instruction selection (cmp+setcc+jcc)**: `cmpq %a, %b; sete %al; movzbq %al, %rax; testq %rax, %rax; jne .L` → `cmpq %a, %b; je .L`. Eliminates 4 instructions by using flags directly from cmpq. Supports all setcc variants (sete, setne, setl, setg, setle, setge, setb, seta, setbe, setae).
- **Total peephole passes**: 9 (up from 6), all always on in `build` command
- **Tests**: 26 peephole tests (up from 20), all pass

### L2 Optimizer Fixes (opt.rs)
- **Inliner dangling references**: Fixed bug where `replace_all_refs` ran before new instructions were fully integrated, leaving dangling references to removed call IDs. Fix: `inline_call_site_tracked` tracks all replacements across inlining rounds and applies them to newly created instructions.
- **Topological sort**: Added `topo_sort_block` to sort block instruction lists after inlining so definitions come before uses. Terminators forced to end.
- **`--opt` still not enabled by default**: Works for simple programs (hello.p: 3316 bytes vs 3468 without opt) but exposes pre-existing register allocator bug on complex programs (constants clobbered by intervening instructions).

### ELF64 Emitter Fix (elf64.rs)
- **movslq memory operand**: Fixed crash on `movslq (%rax), %rax` — extended encoder to handle memory operands via `try_parse_mem` + `encode_modrm_mem`.

### Hot Reload Implementation (hotreload.rs + main.rs)
- **Native JIT execution**: `compile_and_run()` compiles source to ELF64 executable via `build_executable` pipeline (PIR L1→L2→L3 → codegen → 9 peephole passes → ELF64), writes to temp file, executes as child process, captures stdout/stderr/exit code. Zero external tools.
- **Function-level change detection**: `FunctionSnapshot::from_l2()` hashes each L2 function's debug representation (name + params + blocks + values). `FunctionSnapshot::diff()` compares snapshots and reports added/modified/removed functions.
- **State preservation**: `PersistentState` stores key-value state in a temp file keyed by source path hash. State survives across reloads.
- **Watch command rewrite**: `pledgec watch <file.p> [--interval <ms>] [--opt] [--no-run]` — polls for file changes, recompiles to native ELF64, reports function diff (added/modified/removed), executes, shows compile/exec time + binary size. Initial compilation + reload both use native JIT.
- **Removed old stub**: The old `watch` command that just re-lowered to IR and printed "Hot reload complete" without executing anything has been replaced.
- **Tests**: 8 hot reload tests (function snapshot diff, no changes, removed functions, persistent state, compile_and_run hello, compile_and_run with opt, debugger, watcher detects changes). Total unit tests: 136 (up from 133).

### Documentation Audit
- **README.md**: Corrected all outdated claims — 35 source files (was 25), 45K LOC (was 35K), 133 unit tests (was 85), 7/8 harness (was 8/8), 78/79 interp/PE (was 52/52), 53/79 C codegen (was 38+), 102 .p files (was 87), 9 peephole passes (was 6), ELF64 executables work (was "WIP/segfault"), FFI/const generics/GATs work (were "not parsed"), GPU backend wired (was "dead code")
- **features.md**: Updated all status entries with verified information
- **docs/roadmap.md**: Updated phase status and test counts
- **docs/roadmap-limitations.md**: Updated gap analysis with verified results
- **docs/architecture.md**: Updated to show ELF64 as primary Linux backend, PIR pipeline in build
- **docs/changelog.md**: Updated test results to verified counts

## [Previous] — Phase 2: Safety (Complete) + Documentation Update

### Phase 2A — Region Inference
- **AST**: `Stmt::RegionBlock { name, body }` for `region r { ... }` blocks
- **Lexer/Parser**: `region` keyword, region block parsing
- **Typechecker**: region stack tracking, variable-to-region mapping, use-after-free detection, double-free detection, dangling pointer detection, region escape detection
- **IR**: `Region` enum (Stack, Heap, Static, Named), `RegionRef` in PIR (`NewRegion`, `FreeRegion`, `AllocaIn`)
- **Tests**: `tests/regions/test_region_basic.p`, `test_region_nested.p`, `test_region_escape.p` (3/3 pass)

### Phase 2B — Effect System
- **IR**: `Effect` enum expanded with `Net`, `Spawn`, `Pure` (in addition to `Alloc`, `IO`, `Diverge`, `Read`, `Write`)
- **Parser**: `!IO`, `!{IO, Net}`, `!{}` (pure) syntax in function signatures
- **Typechecker**: effect inference (fixpoint over call graph), effect checking (under-declaration errors), `has_effect_annotation` field on `Func`
- **Optimizer**: `is_pure` and `has_side_effects` in `opt.rs` leverage effect info for DCE/CSE
- **Effect handlers**: `handle { body } with { Effect.op(args) => handler }` and `do Effect.op(args)` syntax (AST, lexer, parser, typechecker)
- **Tests**: 7 tests — annotations, multi-effects, pure, under-declaration, pure violation, handlers, propagation (7/7 pass)

### Phase 2C — Capability Security (with Elision)
- **Capability elision**: all capability checks are compile-time only; `caps_for_call` in `pir.rs` returns empty `CapSet` for all calls, eliminating runtime capability overhead
- **Typechecker**: `cap_verified` and `current_func_name` fields track verified functions
- **Tests**: 4 tests — grant IO, deny IO, propagation, deny propagation (4/4 pass)

### Phase 2D — Bounds Safety
- **Runtime bounds checking**: Vec/array/string indexing checked in debug mode
- **Integer overflow checking**: debug mode panics on overflow, `--release` wraps
- **Division by zero detection**: runtime panic in debug mode
- **Option::unwrap panics on None**: calls `p_panic`
- **Tests**: 9 tests — OOB vec get/set, overflow add/mul/sub, div-by-zero, empty vec first/last, unwrap none (9/9 pass)

### Documentation Update
- **README.md**: Updated to reflect actual state — 35 source files, ~45,000 LOC, 133 unit tests, 7/8 harness tests, 78/79 interpreter/PE tests, 53/79 C codegen, 102 .p test files, 9 peephole passes, ELF64 executables work on Linux
- **docs/roadmap.md**: Updated test counts and phase status to reflect actual verified state
- **docs/architecture.md**: Updated to show ELF64 emitter as primary backend on Linux, PIR pipeline in build command
- **features.md**: Updated all sections with verified status, added ELF64/Mach-O/COFF/GPU backends, corrected const generics/GATs/FFI status
- **docs/roadmap-limitations.md**: Updated gap analysis with verified test results (133 unit, 7/8 harness, 78/79 interp/PE, 53/79 C codegen, 190 warnings)

### Test Results (Verified)
- **Unit tests**: 133/133 pass (elf64, pir, peephole: 26, codegen_x64, opt, regalloc, tensor)
- **Harness tests**: 7/8 pass (1 failure: golden_files_match — interpreter prints Rust debug format for Vec/Map/Ptr)
- **Interpreter**: 78/79 pass (1 expected failure: test_trait_verify_fail)
- **PE Emitter**: 78/79 valid (1 expected failure: test_trait_verify_fail)
- **C Codegen**: 53/79 match (26 mismatches: asm, comptime, const_generic, gat, json, tensor, etc.)
- **Total .p test files**: 102 (79 root + 9 safety + 4 capability + 7 effects + 3 regions)

## [Previous] — Phase 5A: Bare Metal

### Inline Assembly (`asm!()`) — 5A.1
- **AST**: `Expr::Asm` variant with template string, inputs, outputs, and clobbers
- **IR**: `Inst::Asm` instruction with operand handling
- **Parser**: `asm!("template" : outputs : inputs : clobbers)` syntax (Rust-style)
- **Typechecker**: infers result type from output operands (defaults to `void`)
- **Lowerer**: lowers to `Inst::Asm` with free variable collection for operands
- **Interpreter**: prints asm template and input/output counts (simulation mode)
- **PE codegen**: emits raw x86-64 bytes from asm template (nop, hlt, cli, sti, mfence, sfence, lfence, pause, etc.)
- **C codegen**: emits `__asm__()` blocks
- **WASM codegen**: panics (inline asm not supported in WASM)
- **Test**: `tests/test_asm.p` — 7 test cases covering nop, multi-instruction, inputs, outputs, in+out, fences, pause

### No-OS / Bare-Metal Mode (`--bare` flag) — 5A.2
- **`pledgec emit <file.p> --bare -o output.bin`**: produces a flat binary (no PE headers, no IAT, no OS calls)
- **Skips stdlib**: user code is compiled without `stdlib/core.p` — only built-in language features
- **`pe::generate_flat_binary`**: generates `[code][padding][rdata]` layout, load address 0x10000
- **Bare entry point**: calls `main()`, then `hlt; jmp -3` (infinite halt loop) — no `ExitProcess`
- **Runtime stubs skipped**: no print/malloc/vec stubs (user must use inline asm for I/O)
- **Test**: `tests/test_bare.p` — minimal bare-metal program using `asm!("hlt")`

### Interrupt Handler Support — 5A.3
- **`interrupt pf` keyword**: marks a function as an interrupt handler
- **AST**: `Func::is_interrupt` field, `TokenKind::Interrupt` token
- **Parser**: `interrupt pf name() { ... }` syntax
- **IR**: `IrFunction::is_interrupt` field propagated from AST
- **PE codegen**: interrupt handlers save all 14 GP registers on entry, restore on exit, use `iret` instead of `ret`
- **Builtins**: `p_cli()`, `p_sti()`, `p_hlt()`, `p_lidt(ptr)`, `p_idt_set_entry(vec, handler)`, `p_inb(port)`, `p_outb(port, val)`
- **Bare mode**: emits real x86 instructions (cli, sti, hlt, lidt, in/out)
- **Normal mode**: emits `ud2` (trap) since OS calls are meaningless
- **Test**: `tests/test_interrupt.p` — interrupt handler + cli/sti/hlt builtins

### MMU/Page Table Management — 5A.4
- **Control register access**: `p_cr0_read()`, `p_cr0_write(val)`, `p_cr3_read()`, `p_cr3_write(val)`, `p_cr4_read()`, `p_cr4_write(val)`
- **TLB management**: `p_invlpg(addr)` — invalidate TLB entry
- **Page mapping**: `p_page_map(page_table, virt, phys, flags)`, `p_page_unmap(page_table, virt)`
- **Physical memory**: `p_phys_read64(addr)`, `p_phys_write64(addr, val)`
- **Bare mode**: emits real x86 instructions (mov cr0/cr3/cr4, invlpg)
- **Test**: `tests/test_mmu.p` — CR0/CR3/CR4 read, TLB invalidation, page map/unmap

### Boot Code Generation — 5A.5
- **`pledgec emit <file.p> --boot -o output.bin`**: produces a Multiboot2-compatible kernel binary
- **Multiboot2 header**: magic 0xE85250D6, architecture 0 (i386), checksum, end tag — at offset 0
- **`pe::generate_boot_binary`**: generates `[mb2 header][boot code + main code][padding][rdata]` layout
- **Boot entry point**: sets up stack at 0x90000, calls `main()`, then halt loop
- **Load address**: 0x100000 (1MB — typical kernel load address for GRUB)
- **Test**: `tests/test_boot.p` — minimal kernel with cli, CR0 read, hlt

### Interpreter Fixes (Phase 5C/5D restoration)
- **Pre-register function signatures**: lowerer now pre-registers all function return types before lowering bodies, fixing type inference for calls to functions defined later in the file
- **`arg_string` helper**: interpreter can extract strings from both `Value::Str` and `Value::Ptr` (null-terminated string in memory)
- **JSON builtins**: all `p_json_*` builtins now use `arg_string` for robust string argument handling
- **`p_json_string`**: fixed to serialize a plain string as a JSON string literal (with quotes + escaping), not parse it
- **`p_http_parse_request`**: rewrote to actually parse HTTP request lines, headers, and body into a JSON string with proper escaping
- **`p_http_build_response`**: uses `arg_string` for body argument
- **Opaque store/load**: `Value::Tensor`, `Value::Json`, `Value::Dyn` now stored in interpreter side-table

### ELF64 Object File Emitter (`elf64.rs`) — New Module
- **ELF64 relocatable object file**: emits `.o` files with .text, .rodata, .symtab, .strtab, .rela sections
- **`--elf <output>` flag**: on `codegen` command, writes ELF64 object file
- **Linkable**: output can be linked with `ld`, `lld`, or `gold`

## [Previous] — Phase 5C: AI/ML

### Tensor Type (`tensor.rs` + `stdlib/tensor.p`) — New Module
- **N-dimensional arrays with stride info**: shape, strides, flat f64 storage
- **30 builtin functions**: `p_tensor_zeros`, `p_tensor_ones`, `p_tensor_filled`, `p_tensor_from_vec`, `p_tensor_get`, `p_tensor_set`, `p_tensor_shape`, `p_tensor_ndim`, `p_tensor_size`, `p_tensor_reshape`, `p_tensor_transpose`, `p_tensor_add`, `p_tensor_sub`, `p_tensor_mul`, `p_tensor_scale`, `p_tensor_matmul`, `p_tensor_dot`, `p_tensor_sum`, `p_tensor_mean`, `p_tensor_max`, `p_tensor_min`, `p_tensor_relu`, `p_tensor_sigmoid`, `p_tensor_softmax`, `p_tensor_to_str`, `p_tensor_to_json`, `p_tensor_clone`
- **Pledge stdlib wrapper**: clean API matching the builtin names
- **`Value::Tensor` variant**: opaque type in interpreter, stored in side-table like Vec/Map
- **Type system**: `Tensor` mapped to `IrType::Opaque("Tensor")` (not Struct, avoiding SRET)

### SIMD Intrinsics (`stdlib/simd.p`) — New Module
- **9 builtin functions**: `p_simd_add`, `p_simd_sub`, `p_simd_mul`, `p_simd_div`, `p_simd_dot`, `p_simd_sum`, `p_simd_scale`, `p_simd_from_vec`, `p_simd_to_str`
- **Pledge stdlib wrapper**: elementwise vector operations (f64x2, f64x4, etc.)
- **Interpreter**: SIMD vectors are Vec<f64> with elementwise operations
- **Future**: x86 backend will map to SSE/AVX instructions

### Comptime Shape Verification (`stdlib/shape.p`) — New Module
- **Runtime shape assertions**: `assert_ndim`, `assert_size`, `assert_same_shape`, `assert_matmul_compatible`, `assert_reshape`
- **Comptime shape constants**: `shape_1x1`, `shape_2x2`, `shape_2x3`, `shape_3x2`, `shape_3x4`, `shape_4x2`, `shape_4x4`, `shape_1x3`, `shape_1x4`, `shape_4x1`
- **Future**: when comptime evaluation (Phase 3A) lands, these become compile-time checks

### Auto-Parallelization Framework (`stdlib/parallel.p`) — New Module
- **Parallel operations**: `par_map`, `par_sum`, `par_dot`, `par_fill`, `par_range`
- **Parallelizability analysis**: `is_parallelizable(effect_mask)` — checks IO/State/Panic effect bits
- **Compiler directive**: `parallel_hint()` — marks loops for future auto-parallelization
- **Current**: runs sequentially; future effect system integration will enable true parallelism

### Neural Network Example (`examples/neural_network.p`)
- **2-layer MLP**: Input(3) → Hidden(4, ReLU) → Output(2, Softmax)
- **Pure Pledge**: no external library, uses only tensor stdlib
- **Success metric met**: "Write a neural network forward pass in pure Pledge"

### Interpreter (`interp.rs`) — New Value Type & Helpers
- **`Value::Tensor`**: `Rc<RefCell<Tensor>>` opaque value, stored in side-table
- **Manual `PartialEq` impl**: reference identity for opaque types (Vec, Map, Tensor, Dyn)
- **Helper methods**: `arg_vec_usize`, `arg_vec_i64`, `arg_vec_f64`, `simd_binary`
- **`MakeDyn` instruction handler**: creates trait objects from concrete values

### Lowerer (`lower.rs`) — Extern Registrations
- **30 tensor externs**: all tensor builtins registered with correct opaque types
- **9 SIMD externs**: all SIMD builtins registered with Vec opaque type
- **Function closures**: `t()` and `v()` return fresh `IrType::Opaque` instances

### Typechecker (`typechecker.rs`) — Return Type Inference
- **Float-returning builtins**: `p_tensor_get`, `p_tensor_dot`, `p_tensor_sum/mean/max/min`, `p_simd_dot/sum` → `Type::F64`
- **Tensor-returning builtins**: `p_tensor_*` (non-string) → `Type::Named("Tensor")`
- **String-returning builtins**: `p_tensor_to_str/to_json`, `p_simd_to_str` → `Type::Str`

### Test Results
- **Interpreter: 58/59 passed** (1 pre-existing test_trait_object failure)
- **PE Emitter: 59/59 valid**
- **WASM Target: 59/59 valid**
- **C Codegen: 51/59 matched** (test_float pre-existing + new builtins need C stubs)

### New Test Files
- `tests/test_tensor.p` — tensor creation, indexing, matmul, activations, transpose, reshape, JSON
- `tests/test_simd.p` — SIMD elementwise ops, dot product, sum, scale
- `tests/test_shape.p` — shape assertions (ndim, size, same_shape, matmul_compatible, reshape)
- `tests/test_parallel.p` — parallel map, sum, dot, fill, range, parallelizability check

### New Standard Library Modules
- `stdlib/tensor.p` — Tensor standard library
- `stdlib/simd.p` — SIMD intrinsics standard library
- `stdlib/shape.p` — Comptime shape verification
- `stdlib/parallel.p` — Auto-parallelization framework

### New Example
- `examples/neural_network.p` — 2-layer MLP forward pass (success metric)

---

## [Unreleased] — 2026-08-06 (Batch 11: Enhanced Built-in Types)

### Typechecker (`typechecker.rs`) — Comprehensive Method Sets
- **String methods**: Added `contains`, `starts_with`, `ends_with`, `trim`, `split`, `replace`, `char_at`, `to_uppercase`, `to_lowercase`, `parse_int`, `find`, `rfind`, `repeat`, `bytes`, `clone`
- **Vec methods**: Added `set`, `insert`, `remove`, `contains`, `reverse`, `first`, `last`, `extend`, `truncate`, `sort`, `clone`
- **HashMap methods**: Added `get_or`, `clone`

### Lowerer (`lower.rs`) — Extended Builtin Dispatch
- Extended method dispatch for all new String, Vec, and HashMap methods
- Correct return type inference including element type for Vec generics

### Interpreter (`interp.rs`) — Full Runtime Implementations
- **15 new String runtime functions**: `p_string_contains`, `p_string_starts_with`, `p_string_ends_with`, `p_string_trim`, `p_string_split`, `p_string_replace`, `p_string_char_at`, `p_string_to_uppercase`, `p_string_to_lowercase`, `p_string_parse_int`, `p_string_find`, `p_string_rfind`, `p_string_repeat`, `p_string_bytes`, `p_string_clone`
- **11 new Vec runtime functions**: `p_vec_set`, `p_vec_insert`, `p_vec_remove`, `p_vec_contains`, `p_vec_reverse`, `p_vec_first`, `p_vec_last`, `p_vec_extend`, `p_vec_truncate`, `p_vec_sort`, `p_vec_clone`
- **2 new HashMap runtime functions**: `p_map_get_or`, `p_map_clone`
- Added `PartialEq` derive to `Value` enum for `contains` comparisons

### PE Emitter (`pe.rs`) — Native Runtime Stubs
- Pre-allocated labels for all new runtime functions
- Functional x86-64 implementations for: `p_vec_is_empty`, `p_vec_clear`, `p_vec_first`, `p_vec_last`, `p_vec_contains`, `p_string_is_empty`, `p_string_len`, `p_string_parse_int`, `p_string_clone`, `p_string_starts_with`
- Simplified stubs (return default values) for complex operations: `insert`, `remove`, `reverse`, `extend`, `sort`, `split`, `replace`, `trim`, `to_uppercase`, `to_lowercase`, `find`, `rfind`, `repeat`

### Tests (`tests/test_collections.p`)
- Comprehensive test file covering all 49 operations across String, Vec, and HashMap
- All tests pass in interpreter backend

### Build Status
- Compiles successfully with warnings (unused variants — expected)
- Interpreter: all 49 test outputs verified correct
- PE emitter: stubs registered and compiling, full native implementations pending for complex methods

---

## [Unreleased] — 2026-08-04 (Batch 10: Direct PE Emitter)

### PE Emitter (`pe.rs`) — Direct Native Code Generation
- **New backend**: Encodes x86_64 instructions directly to machine code bytes and constructs a valid PE file — no `ml64.exe`, `link.exe`, or C compiler required
- **`emit` CLI command**: `pledgec emit <file.p> -o <out.exe>` — full pipeline: lex → parse → typecheck → monomorphize → lower → x86 machine code → PE file → `.exe`
- **Full x86_64 instruction encoder**: REX prefixes (W/R/X/B bits), ModR/M + SIB bytes, 8-bit/32-bit displacement encoding, immediate encoding
- **Register model**: All 16 x86_64 registers (RAX–R15) with correct encoding for extended registers (R8–R15)
- **Instruction set**: `mov_rr`, `mov_mem_disp_reg`, `mov_mem_disp_reg32` (32-bit store), `mov_r_imm`, `mov_r8_imm`, `mov_mem_reg8`, `add_rr`, `sub_rr`, `imul_rr`, `xor_rr`, `and_rr`, `or_rr`, `cmp_rr`, `test_rr`, `jcc_label`, `call_label`, `call_reg`, `call_rip`, `ret`, `push`, `pop`, `lea_rbp`, `sub_rsp_i32`, `add_rsp_i32`, `dec_r`, `inc_r`, `xor_self`
- **PE file format**: DOS header, PE header, section headers (.text + .rdata), import directory, IAT (Import Address Table) for `kernel32.dll`
- **Import resolution**: `GetStdHandle`, `WriteFile`, `ExitProcess` imported via IAT with RIP-relative `call [rip+offset]` addressing
- **Runtime stubs** (generated as machine code):
  - `p_print_int`: Integer to string (div/mod loop, reverse fill in stack buffer), `WriteFile` to stdout, newline
  - `p_print_str`: String + newline to stdout via `WriteFile`
  - `p_print_bool`: "true"/"false" to stdout
  - `p_print_float`: (stub, prints truncated int)
  - `p_panic`: Prints message + `ExitProcess`
  - `p_malloc` / `p_free`: Heap allocation (stubs)
- **Label relocation**: Deferred label resolution — `jcc` and `call` instructions emit placeholders that are patched after all code is generated
- **Stack frame**: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues, alloca slots at fixed `rbp` offsets, `r12`/`r13` saved/restored in runtime stubs
- **Read-only data**: String literals stored in `.rdata` section, referenced via RIP-relative addressing

### Bug Fixes
- **`mov_rr` REX prefix bug**: REX.R and REX.B bits were swapped in `mov_rr` (0x89 opcode). In `MOV r/m64, r64`, `reg` = source, `r/m` = destination, so R should extend `src` and B should extend `dst`. The old code had them backwards, causing wrong register selection for any `mov_rr` involving extended registers (R8–R15), leading to `ACCESS_VIOLATION` crashes
- **64-bit store overwriting adjacent stack data**: `mov_mem_disp_reg` was used to zero the `written` DWORD variable before `WriteFile` calls, but it performed a 64-bit store (REX.W), writing 8 bytes of zeros and overwriting adjacent stack data — the newline byte in `p_print_str` and digit bytes in `p_print_int`. Added `mov_mem_disp_reg32` for 32-bit stores and replaced all 5 `&written` zeroing calls
- **Integer print length off-by-one**: Buffer length was calculated as `r13 - r8` (buffer_end - current_pos), excluding the newline stored at `r13`. Added `+1` to include it

### Build Status
- `emit` verified: `hello.p` generates valid PE, produces correct output:
  ```
  Hello, Pledge!
  50
  y is big
  ```
- Output matches interpreter (`run` command) and MASM (`build` command) exactly
- Exit codes work correctly (`minimal.p` returns 42, `hello.p` returns 0)
- Compiles successfully with warnings (unused variants — expected)

---

## [Unreleased] — 2026-08-04 (Batch 9: Monomorphization & Module Imports)

### Monomorphization (`mono.rs`)
- **New module**: Generic function specialization
- Scans all call sites for concrete type arguments
- Generates specialized non-generic copies (e.g., `identity__i64`, `identity__bool`)
- Replaces generic call sites with calls to specialized versions
- Handles generic methods on structs (mangled as `StructName__method__T1_T2`)
- Integrates into pipeline: typecheck → monomorphize → lower

### Module Import Resolution (`compiler.rs`)
- `use foo::bar` imports now resolve files from disk
- Searches for `<path>.p` or `<path>/mod.p` relative to source file
- Also searches `pledge_modules/` subdirectory
- Module merging: stdlib + imported modules + user code (user wins on conflicts)
- Module-level visibility enforcement in typechecker
- Import error messages with suggestions (Levenshtein distance for "did you mean")
- `core` module auto-included (stdlib/core.p prepended to every program)

### Compiler Pipeline Updates
- All entry points now accept `source_path` for import resolution
- `build_program_with_path()` handles stdlib + imports + user merge
- `emit_source_with_path()` added for PE emission pipeline

---

## [Unreleased] — 2026-08-04 (Batch 8: C Codegen Backend)

### C Code Generator (`codegen_c.rs`)
- **New backend**: Translates SSA IR to portable C code (C99) that compiles with MSVC (`cl.exe`), GCC, or Clang
- **`ccompile` CLI command**: `pledgec ccompile <file.p> -o <out.exe>` — full pipeline: lex → parse → typecheck → lower → C codegen → compile with `cl.exe` → native `.exe`
- **MSVC environment setup**: `find_cl()` and `find_vcvarsall()` functions locate the MSVC compiler and set up its environment via `vcvarsall.bat`
- **Function renaming**: `main` → `main_entry` (avoids conflict with C `main` wrapper), `abs`/`max`/`min` → `p_abs`/`p_max`/`p_min` (avoids MSVC intrinsic/macro conflicts)
- **`#undef` directives**: `#undef max`/`#undef min`/`#undef abs` in generated C to avoid MSVC macro conflicts
- **SSA variable declarations**: All SSA values declared once at function start as `int64_t` (or `void*` for pointers), instructions generate assignments — no redefinitions
- **Void type handling**: `IrType::Void` and `IrType::Never` variables skipped in declarations; void functions get default `return;` statements
- **Polymorphic `print` dispatch**: `print` calls in IR are intercepted in codegen and dispatched to `p_print_str`, `p_print_int`, `p_print_bool`, or `p_print_float` based on argument type, followed by `p_print_newline()` (matching interpreter behavior where `print` adds a trailing newline)
- **Runtime implementations**: `p_print`, `p_print_str`, `p_print_int`, `p_print_bool`, `p_print_float`, `p_print_newline`, `p_panic`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str`, Vec/HashMap/String runtime functions
- **String constants**: Emitted as `static const char str_N[] = "...";` in generated C
- **Struct definitions**: Emitted as C `struct` typedefs with field layout
- **Block labels**: Basic blocks emitted as C labels (`bb0:`, `bb1:`, etc.) with `goto` for control flow
- **Main wrapper**: C `main()` function calls `main_entry()` and returns its result

### MSVC Integration (`main.rs`)
- **`find_cl()`**: Searches Visual Studio installation paths for `cl.exe`
- **`find_vcvarsall()`**: Searches for `vcvarsall.bat` to set up MSVC build environment
- **`ccompile` command**: Generates C file, invokes `cl.exe` via `vcvarsall.bat` environment, produces native executable

### Build Status
- Compiles successfully with warnings (unused imports/variants — expected)
- `ccompile` verified: `hello.p` generates valid C, compiles with MSVC `cl.exe`, and produces correct output:
  ```
  Hello, Pledge!
  50
  y is big
  ```
- Output matches interpreter (`run` command) exactly

---

## [Unreleased] — 2026-08-04 (Batch 7: Lowering Feature Implementation)

### AST → IR Lowering (`lower.rs`)
- **`alloca_types` tracking**: New `HashMap<ValueId, IrType>` field on `Lowerer` to track the actual type of each `alloca` instruction, enabling correct `Load` types instead of hardcoded `I64`
- **`Expr::Ident` fix**: Now uses `alloca_types` to determine the correct IR type for mutable variable loads (was hardcoded `I64`)
- **`Expr::Deref` fix**: Uses `alloca_types` for correct dereference load types
- **`Expr::ArrayLit` lowering**: Allocates array on stack via `Alloca`, stores each element via `GetElemPtr` + `Store`, returns the array pointer
- **`Expr::Index` lowering**: Uses `GetElemPtr` with correct element type from array type inference, loads element with correct type
- **`Stmt::IndexAssign` lowering**: `GetElemPtr` + `Store` for array element assignment, with compound assignment support (`+=`, `-=`, etc.)
- **Compound assignment fix**: `lower_compound_assign` and `Stmt::Assign` compound path now use `get_value_type()` / `alloca_types` for correct `BinOp` type instead of hardcoded `I64`
- **`Stmt::Return` defer emission**: `emit_defers` helper added — drains deferred statements from `defer_stack` and emits them before `Ret` instruction
- **`defer_stack` field**: New `Vec<Vec<Stmt>>` on `Lowerer` for tracking deferred statements per scope
- **`Expr::MethodCall` lowering**: Method name mangling (`__pledge_StructName_method`), SRET handling for struct returns, receiver passed as first argument
- **`Expr::Match` as expression**: `lower_match_expr` with result slot alloca, pattern matching branches, merge block for result
- **`infer_match_type`**: Infers the result type of a match expression from its arms
- **Match pattern lowering**: Extended to support `Pattern::StrLit` (string comparison), `Pattern::Range` (bounds check with `BinOp::Lt`/`GtEq`), `Pattern::EnumVariant` (variant discriminant check with bindings)
- **`Expr::TupleLit` lowering**: Allocates tuple struct on stack, stores each element via `GetFieldPtr`
- **`Expr::EnumLit` lowering**: Allocates enum struct, stores variant discriminant + payload fields
- **`Expr::Try` lowering**: `TryUnwrap` instruction with ok/err blocks, early return on error
- **`Expr::Closure` lowering**: Lambda lifting — creates a new `IrFunction` with mangled name, captures are simplified (no environment capture yet)
- **`for` loop array iterator**: `for x in arr { }` lowered to index-based while loop with `GetElemPtr` + `Load` for element access; handles both direct array values and pointer-to-array (from mutable variable loads)
- **`loop_stack` updated to 3-tuple**: `(cond_block, exit_block, Option<ValueId>)` to support loop-as-expression with break values
- **`enums` field**: `HashMap<String, EnumDef>` on `Lowerer` for enum variant lookup during pattern matching
- **`impl_methods` field**: `HashMap<String, HashMap<String, Func>>` on `Lowerer` for method resolution during `MethodCall` lowering

### Parser Fix (`parser.rs`)
- **Array literal element type**: Changed from hardcoded `Type::I32` to `Type::I64` to match `IntLit` lowering (which produces `I64` values)

### Test Files (Batch 7)
- `tests/features6.p` — array literals `[1, 2, 3, 4, 5]`, array indexing (read/write), compound assignment on array elements (`arr[2] += 100`), `for` loop over array with sum accumulation

### Build Status
- Compiles successfully with warnings (unused variants — expected)
- `run` command verified: `hello.p`, `struct_return.p`, `features6.p` all produce correct output in interpreter
- `features6.p` output: `1, 3, 5, 10, 20, 103, 142` (array reads, writes, compound assign, for-loop sum)

---

## [Unreleased] — 2026-08-04 (Batch 6: IR, x86_64 Backend, Interpreter)

### SSA IR Module (`ir.rs`)
- Typed Static Single Assignment (SSA) intermediate representation
- `IrType`: `I1`, `I8`, `I16`, `I32`, `I64`, `U8`, `U32`, `U64`, `F32`, `F64`, `Bool`, `Ptr`, `Void`, `Tuple`, `Struct`, `Enum`, `Func`
- `Inst`: `Const`, `ConstStr`, `BinOp`, `UnaryOp`, `Call`, `Load`, `Store`, `Alloca`, `Br`, `CondBr`, `Ret`, `Panic`, `Cast`, `ICmp`, `FCmp`, `GEP`, `StructField`, `StructConstruct`
- `ValueId`, `BlockId`, `IrFunction`, `IrModule` types
- `EffectSet` for tracking side effects
- `Display` impls for IR types, instructions, functions, modules (for `ir` CLI command)

### AST → IR Lowering (`lower.rs`)
- Full lowering pipeline: type-checked AST → SSA IR
- `Lowerer` struct with function-level lowering context
- Variable bindings via `alloca` + `load`/`store` (mutable variables)
- Control flow lowering:
  - `if`/`else` → `CondBr` with then/else/merge blocks
  - `while` loops → condition block + body block + back-edge
  - `for` loops over ranges → `alloca` for loop variable + comparison + increment + back-edge
- Function calls → `Inst::Call` with argument value IDs
- F-string lowering:
  - Literal parts → `ConstStr`
  - Expression parts → `__pledge_to_str` call (skipped if already string type)
  - All parts concatenated via `__pledge_str_concat`
  - Type check: if expression IR type is `Ptr`, skip `__pledge_to_str` (prevents treating string pointer as integer)
- Constants (`pc`) → `Inst::Const` with value inlined
- Binary ops, unary ops, comparisons → corresponding `Inst` variants
- Runtime function registration: `__pledge_print_int`, `__pledge_print_str`, `__pledge_print_bool`, `__pledge_to_str`, `__pledge_str_concat`, `__pledge_panic`

### IR Interpreter (`interp.rs`)
- Direct execution of SSA IR — no external assembler needed
- `run` CLI command for fast iteration
- Value types: `Int`, `Float`, `Bool`, `Str`, `Ptr`, `Unit`
- Runtime function handling:
  - `print` → dispatches to `__pledge_print_int`, `__pledge_print_str`, `__pledge_print_bool` based on value type
  - `__pledge_to_str` → converts `Int`, `Bool`, `Float` to `Str`; `Str` passes through
  - `__pledge_str_concat` → concatenates `Str` and `Ptr` values
- Verbose mode (`-v`) for debugging IR execution
- `RunResult::Return(Value)` and `RunResult::Panic(String)` result types

### x86_64 Code Generator (`x86.rs`)
- Generates MASM-compatible x86_64 assembly from SSA IR
- **Windows x64 calling convention**: RCX, RDX, R8, R9 for first 4 args; 32-byte shadow space; 16-byte stack alignment
- Stack frame: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues with pre-allocated alloca slots
- Non-volatile register preservation: `r12`/`r13` saved/restored in runtime stubs
- Runtime stubs (generated inline, no C runtime dependency):
  - `__pledge_print_int` — integer to string + `WriteFile` to stdout
  - `__pledge_print_str` — string + newline to stdout
  - `__pledge_print_bool` — "true"/"false" to stdout
  - `__pledge_to_str` — integer to null-terminated string in stack buffer
  - `__pledge_str_concat` — concatenates two strings into static buffer with temp buffer to avoid aliasing
  - `__pledge_panic` — prints message + `ExitProcess`
- Static data: string literals, `str_concat_buf`, `str_concat_tmp` in `.data` section (`BYTE 4096 DUP(?)`)
- MASM addressing: `mov reg, OFFSET label` for static data (avoids `ADDR32` relocation errors)
- External imports: `GetStdHandle`, `WriteFile`, `ExitProcess` from `kernel32.lib`
- Entry point: `main` / `_main` (both exported)

### CLI Commands
- `build <file.p> -o <out.exe>` — full pipeline to native Windows executable (ml64.exe + link.exe)
- `ccompile <file.p> -o <out.exe>` — full pipeline via C codegen + MSVC cl.exe (added in Batch 8)
- `run <file.p> [-v]` — interpret via IR (no external tools needed)
- `ir <file.p>` — print lowered SSA IR
- `compile <file.p> -o <out.s>` — legacy ARM64 codegen
- `tokens`, `ast`, `version` — unchanged

### Bug Fixes
- **For loop infinite loop**: `lower_for` now uses `alloca` for loop variable so increments persist across iterations
- **F-string only printing first literal**: `lower_fstring` now concatenates all parts (literals + expression-converted strings) via `__pledge_str_concat`
- **F-string string variables printing memory address**: Type check added — if expression is already `IrType::Ptr` (string), `__pledge_to_str` is skipped
- **Recursive function calls crashing in x86**: Non-volatile registers `r12`/`r13` now saved/restored in `__pledge_print_int`, `__pledge_print_str`, `__pledge_print_bool` stubs (Windows x64 ABI compliance)
- **Linker error `LNK2017: ADDR32`**: Changed `lea reg, [rip + label]` to `mov reg, OFFSET label` for static data buffers in MASM
- **MASM error `undefined symbol: rip`**: MASM does not use `rip` keyword; removed explicit `rip +` addressing
- **Buffer aliasing in `__pledge_str_concat`**: Added `str_concat_tmp` buffer — left string copied to temp first, then to output buffer

### Test Files (Batch 6)
- `tests/control_flow.p` — if/else if/else, for loops with `..` and `..=` ranges
- `tests/functions.p` — user-defined functions, recursive `factorial(5)`
- `tests/strings.p` — string literals, string variables, f-string interpolation with string and integer expressions
- `tests/struct_return.p` — struct return via SRET calling convention, field access on returned structs

### Build Status
- Compiles successfully
- `run` command verified: `hello.p`, `control_flow.p`, `functions.p`, `strings.p`, `features.p` all produce correct output in interpreter
- `build` command verified: `control_flow.p`, `functions.p`, `strings.p` all build and run correctly as native x86_64 executables

---

## [Unreleased] — 2026-08-03 (Batch 5)

### G16: Option/Result Types (Generic Types)
- Generic type syntax: `Option<T>`, `Result<T, E>` parsed as `Type::Generic { name, args }`
- Parser: `Name<T, U>` in `parse_type()` with nested generic args
- Typechecker: `Type::Generic` resolution for `Option` and `Result` in `?` operator
- Codegen: Generic types pass through as opaque values (simplified)

### G18: Generic Functions
- Generic function syntax: `pf identity<T>(x: T) -> T { ret x; }`
- Parser: `<T, U, ...>` type params after function name in `parse_func()`
- AST: `Func.type_params: Vec<String>` field added
- Typechecker: Type params registered as named types in function scope (simplified)

### G29: Error Handling (`?` Operator)
- Try operator: `expr?` unwraps `Result<T, E>` -> `T` or `Option<T>` -> `T`
- Parser: `?` as postfix operator in `parse_postfix()`
- AST: `Expr::Try { expr }` node
- Typechecker: Validates `Option<T>` or `Result<T, E>` and returns inner type `T`
- Codegen: Simplified passthrough (full impl would check discriminant and early-return)

### G30: Closures
- Block closure: `pf(x: i32) -> i32 { ret x + 1; }`
- Expression closure: `pf(x: i32) => x * 2`
- Parser: `pf(params)` in `parse_primary()` with `-> ret { body }` or `=> expr` forms
- AST: `Expr::Closure { params, body, ret_ty }` node
- Typechecker: Closure scope with param bindings, body checked
- Codegen: Simplified inline body generation (full impl would create trampoline function)

### G15: Tuple Types and Literals
- Tuple type: `(i32, bool)`, `(T, U, V)` parsed as `Type::Tuple(Vec<Type>)`
- Tuple literal: `(42, true, "hello")` parsed as `Expr::TupleLit(Vec<Expr>)`
- Parser: `(T, U, ...)` in `parse_type()`, `(expr, expr, ...)` in `parse_primary()`
- Typechecker: `Type::Tuple` of element types
- Codegen: Evaluate each element in sequence (simplified)

### G40: Standard Library (Initial)
- Test file demonstrates stdlib patterns: string operations, f-string formatting
- Full stdlib expansion is ongoing

### Bug Fix: Double-advance in `parse_type`
- Fixed `Ident` case in `parse_type()` that called `self.advance()` inside the match arm and then fell through to `self.advance()` at the end, causing the parser to skip the next token
- This was the root cause of generic function parsing failures

## [Unreleased] — 2026-08-02 (Batch 4)

### G17: Enum Variant Matching with Data
- Enum variants with anonymous data: `enum Shape { Circle(f64), Rectangle(f64, f64), Unit }`
- Enum variant construction: `Shape::Circle(3.14)`
- Enum variant pattern matching: `Shape::Circle(radius: f64) => { ... }`
- New `Expr::EnumLit { enum_name, variant, args }` AST node
- Parser: `EnumName::Variant(args)` in `parse_primary()`, anonymous types in `parse_enum()`
- `read_type_name()` helper accepts both identifiers and type keywords (i32, f64, etc.)
- Typechecker: `EnumLit` returns `Type::Named(enum_name)`
- Codegen: simplified variant index emission

### G28: References & Borrowing
- Reference types: `&i32`, `&mut T`, `&str`
- Reference expressions: `&x`, `&mut x`
- Dereference: `*expr`
- New `Type::Ref { inner, is_mut }` variant
- New `Expr::Ref { expr, is_mut }` and `Expr::Deref { expr }` AST nodes
- Parser: `&`/`&mut` in `parse_type()` and `parse_unary()`, `*` deref in `parse_unary()`
- Typechecker: `Ref` wraps inner type, `Deref` unwraps `Type::Ref`
- Codegen: `Ref` computes `add x0, sp, #offset`, `Deref` does `ldr x0, [x0]`

### G21: String Operations
- String concatenation with `+`: `"Hello" + " " + "World"`
- String literal patterns in match: `"hello" => { ... }`
- New `Expr::StrConcat { left, right }` AST node
- New `Pattern::StrLit(String)` variant
- Typechecker: `BinOp::Add` with `Str + Str` returns `Type::Str`
- Codegen: `StrConcat` calls `__pledge_str_concat` runtime function

### G31: Modules & Imports
- `use std::io;` — module import syntax
- `use path::item;` — path with `::` separator
- New `Stmt::Use { path, items }` AST node
- New `ImportDef` struct, `Program.imports` field
- Parser: `parse_use()` for top-level, `parse_use_stmt()` for statement-level
- Typechecker: `Stmt::Use` is a no-op (resolved at compile time)
- Codegen: `Stmt::Use` is a no-op

### G34: Attributes
- `#[test]` — simple attribute
- `#[name(arg1, arg2)]` — attribute with arguments
- New `Attribute { name, args }` struct, `Program.attributes` field
- Parser: `parse_attribute()` handles `#[...]` syntax with optional args
- Attributes parsed at top level alongside functions, structs, enums

### Test Files (Batch 4)
- `tests/features4.p` — tests for enums, references, string concat, imports, attributes

---

## [Unreleased] — 2026-08-02 (Batch 3)

### G14: Array Types and Literals
- Array type: `[i32; 5]` in type annotations
- Array literal: `[1, 2, 3, 4, 5]`
- Array indexing: `arr[0]`, `arr[i]`
- Array index assignment: `arr[0] = 42`, `arr[i] += 1`
- New `Type::Array { elem, size }` variant
- New `Expr::ArrayLit`, `Expr::Index` AST nodes
- New `Stmt::IndexAssign` for `arr[idx] op= expr`
- Parser: `[T; N]` in `parse_type()`, `[...]` in `parse_primary()`, `arr[idx]` in `parse_postfix()`
- Codegen: stack-allocated arrays (8 bytes per element), base+index*8 addressing

### G27: Pattern Matching (match / =>)
- `match expr { pattern => { body }, ... }` statement
- Pattern types: `IntLit`, `BoolLit`, `Ident` (binding), `Wildcard` (`_`), `EnumVariant` (`Enum::Variant(bindings)`)
- New `Pattern` enum, `MatchArm` struct
- New `Stmt::Match` and `Expr::Match` AST nodes
- Parser: `parse_match_stmt()`, `parse_pattern()`
- Typechecker: pattern variable binding in match arms
- Codegen: comparison-based arm matching with branch-to-arm-label pattern

### G32: F-String Interpolation
- `f"Hello, {name}!"` — first-class f-string with embedded expressions
- New `TokenKind::FString` token
- New `Expr::FString { parts }` and `FStringPart::Literal(String)` / `FStringPart::Expr(Expr)` AST nodes
- Lexer: `read_fstring()` handles `{expr}` interpolation with brace depth tracking
- Parser: `parse_fstring_parts()` re-lexes and parses embedded expressions
- Codegen: evaluates expression parts (full string formatting pending runtime support)

### G22: Float Codegen
- Float literals stored in `.rodata` section as `.double` constants
- New `intern_float()` method with `float_consts` map
- `gen_expr` for `FloatLit` loads via `adr` + `ldr d0`

### Test Files (Batch 3)
- `tests/features3.p` — tests for arrays, match, f-strings, floats

---

## [Unreleased] — 2026-08-02 (Batch 2)

### G5: For Loops with Ranges
- `for x in 0..10 { }` (exclusive range)
- `for x in 0..=10 { }` (inclusive range)
- New `Expr::Range` AST node with `start`, `end`, `inclusive` fields
- Parser: `parse_range()` at expression top level
- Codegen: proper counter-based loop with compare, increment, and branch

### G12: Type Aliases
- `ty Name = i32;` syntax (top-level)
- New `TypeAlias` struct, `type_aliases` field on `Program`
- Typechecker: `resolve_type()` recursively resolves aliases
- `types_compatible()` resolves aliases before comparison

### G23: Struct Methods (impl blocks)
- `impl StructName { pf method(self: StructName) -> i32 { } }` syntax
- New `ImplDef` struct, `impls` field on `Program`
- New `Expr::MethodCall` AST node: `receiver.method(args)`
- Parser: `parse_impl()`, method call parsing in `parse_postfix()`
- Typechecker: method registration per struct, arg count validation (self excluded)
- Codegen: mangled method names (`__pledge_StructName_method`), struct type tracking via `var_types`

### Block-as-Expression
- `{ stmt; stmt; expr }` returns final expression value
- New `Expr::Block(Vec<Stmt>)` AST node
- Parser: `LBrace` in `parse_primary()` triggers block expression parsing
- Typechecker: returns last expression type
- Codegen: generates block statements in new scope

### If-as-Expression
- Already partially supported via `Expr::If` in expression position
- Parser already handles `if` in `parse_primary()`

### Lexer Improvements (Batch 2)
- Hex literals: `0xFF`, `0xDEADBEEF`
- Binary literals: `0b1010`
- Octal literals: `0o777`
- Underscore separators: `1_000_000`, `0xFF_FF`

### Test Files (Batch 2)
- `tests/features2.p` — tests for ranges, type aliases, block expressions, struct methods

---

## [Unreleased] — 2026-08-02 (Batch 1)

### Language Syntax

#### Keyword Renames (Rust → Pledge)
- `fn` → `pf` (Pledge Function) — used for all functions
- `let` / `let mut` → `pv` / `pv mut` (Pledge Variable)
- `const` → `pc` (Pledge Constant)
- `trait` → `cap` (capability/trait)
- `return` → `ret`
- `type` → `ty` (type alias)
- `import` → `use`

#### New Keywords
- `comptime` — compile-time code blocks
- `defer` — Go-style cleanup
- `spawn` — task spawning
- `region` — explicit region (replaces lifetime annotations)

#### File Extension
- Changed from `.pl` to `.p` to avoid collision with Perl

### Compiler Features

#### G1: Compound Assignment Operators
- Added `+=`, `-=`, `*=`, `/=`, `%=`
- Added `&=`, `|=`, `^=`, `<<=`, `>>=`
- New `AssignOp` enum with 11 variants in AST
- Parser handles assignment via `parse_assign_or_expr()`
- Codegen for `+=`, `-=`, `*=` (others fall back to simple assign)

#### G2: Bitwise Operators
- Added `&` (bitwise AND), `|` (bitwise OR), `^` (bitwise XOR)
- Added `~` (bitwise NOT), `<<` (shift left), `>>` (shift right)
- New `BinOp` variants: `BitAnd`, `BitOr`, `BitXor`, `Shl`, `Shr`
- New `UnaryOp::BitNot` variant
- Operator precedence chain: `bit_or > bit_xor > bit_and > shift`
- Codegen: `and`, `orr`, `eor`, `lsl`, `asr`, `mvn`

#### G3: Boolean Literals
- `true` / `false` already in tokens; now fully supported in parser, typechecker, and codegen
- Codegen emits `mov x0, #1` (true) / `mov x0, #0` (false)

#### G4: While Loops
- `while cond { body }` syntax
- Parser: `parse_while()`
- Typechecker: validates condition is bool
- Codegen: proper label-based loop with `while_start` / `while_end`

#### G6: Loop / Break / Continue
- `loop { body }` — infinite loop
- `break;` / `break expr;` — exit loop
- `continue;` — skip to next iteration
- Parser: `parse_loop()`, break/continue handling in `parse_stmt()`
- Codegen: label-based jump instructions

#### G7: Mutable Variables
- `pv mut name = value;` syntax
- `is_mut` field added to `Stmt::Let`
- Assignment (`=`) and compound assignment store back to existing variable slots

#### G8: Constant Declarations
- `pc NAME: type = value;` syntax (top-level only)
- New `ConstDef` struct in AST
- `consts` field added to `Program`
- Typechecker tracks constants in `consts` map
- Constants resolved in identifier lookup

#### G9: Unary Not
- `!expr` for logical negation (bool)
- `~expr` for bitwise negation (integers)
- Both fully supported in parser, typechecker, and codegen

#### G10: Comparison Chains
- Comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`) work in nested expressions
- Proper precedence: `equality > comparison > shift > bitwise_and > bitwise_or`

#### G11: Type Inference for Variables
- `pv x = 42;` infers `i32` without explicit type annotation
- Typechecker infers from initializer expression when no type declared

#### G33: Defer
- `defer { body }` syntax
- Parser: `parse_stmt()` handles `defer` keyword
- Codegen: simplified (executes immediately; full scope-exit queueing pending)

### Lexer Improvements

#### Number Literals
- Hex literals: `0xFF`, `0xDEADBEEF`
- Binary literals: `0b1010`
- Octal literals: `0o777`
- Underscore separators: `1_000_000`, `0xFF_FF`

#### Comments
- Line comments: `//`
- Block comments: `/* */` (new)

#### Strings
- F-string lexing: `f"..."` (tokenized as string, interpolation parsing pending G32)

#### Operators
- All compound assignment operators: `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`
- Bitwise operators: `|`, `^`, `~`, `<<`, `>>`
- Range operators: `..`, `..=` (tokenized, parsing pending G5)
- Hash: `#` (for attributes, parsing pending G34)

### Token Changes
- Added: `Pipe`, `Caret`, `Tilde`, `Shl`, `Shr`, `DotDot`, `DotDotEq`
- Added: `PlusEq`, `MinusEq`, `StarEq`, `SlashEq`, `PercentEq`, `AmpEq`, `PipeEq`, `CaretEq`, `ShlEq`, `ShrEq`
- Added: `Hash`
- Removed duplicate delimiter definitions

### AST Changes
- `BinOp`: added `BitAnd`, `BitOr`, `BitXor`, `Shl`, `Shr`
- `UnaryOp`: added `BitNot`
- `Stmt`: added `Assign`, `While`, `For`, `Loop`, `Break`, `Continue`, `Defer`
- `Stmt::Let`: added `is_mut: bool` field
- New `AssignOp` enum (11 variants)
- New `ConstDef` struct
- `Program`: added `consts: Vec<ConstDef>` field

### Typechecker Changes
- Added `consts: HashMap<String, Type>` tracking
- Type checking for: `Assign`, `While`, `For`, `Loop`, `Break`, `Continue`, `Defer`
- Bitwise binary operator type checking (requires integer types)
- `BitNot` unary type checking
- Constant lookup in identifier resolution

### Codegen Changes
- Codegen for: `Assign` (with `+=`, `-=`, `*=`), `While`, `For`, `Loop`, `Break`, `Continue`, `Defer`
- Bitwise binary ops: `and`, `orr`, `eor`, `lsl`, `asr`
- `BitNot` unary: `mvn`
- Updated `count_lets()` to handle new statement types

### IDE Support
- `.vscode/settings.json` — associates `.p` files with Pledge language
- `.vscode/pledge.tmLanguage.json` — TextMate grammar for syntax highlighting
- Scopes: keywords, types, strings, comments, numbers, functions

### Documentation
- `README.md` — comprehensive rewrite with features, compiler pipeline, syntax overview, keyword reference, project structure, roadmap
- `docs/syntax-spec.md` — updated with new keywords and syntax

### Test Files
- `tests/hello.p` — updated to new syntax (`pf`, `pv`, semicolons)
- `tests/struct.p` — updated to new syntax
- `tests/arithmetic.p` — updated to new syntax
- `tests/features.p` — new test exercising compound assignment, bitwise ops, while loops, mutable vars, constants

### Build Status
- Compiles successfully with 5 warnings (all dead code — expected for unused AST fields)
- All 4 test files compile to ARM64 assembly

---

## [Initial] — Pre-2026-08-02

### Initial Compiler
- Lexer with basic tokens and keywords
- Recursive descent parser
- Basic type checker (integers, floats, strings, bools, structs, functions)
- ARM64 (AArch64) code generator
- CLI: `compile`, `tokens`, `ast`, `version` commands
- Basic structs, enums, functions, if/else expressions
- Binary arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- `print()` builtin
