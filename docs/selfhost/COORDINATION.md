# Coordination — Parallel Agent Sessions

> **READ THIS FIRST, BEFORE ANY OTHER FILE.**
> This is the single source of truth for what is being worked on, by whom,
> and what architectural decisions have been made. Without this, parallel
> agents will conflict and undo each other's work.
>
> **Last updated:** Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings. Bootstrap fixpoint preserved.)

---

## How to Use This File

### Before starting work
1. Read this file completely.
2. Check "Active Sessions" — are any sessions working on files you need?
3. Check "Architectural Decisions" — has a direction changed since you last read the docs?
4. Add your session to "Active Sessions" with the files you plan to touch.
5. Read `CURRENT.md` for the current blocker.

### During work
1. If you make an architectural decision, add it to "Architectural Decisions" immediately.
2. If you change which files you're editing, update your session entry.
3. If you discover something important, add it to "Key Findings".

### After finishing
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `CURRENT.md` with what you did.
3. Remove your session from "Active Sessions".
4. Update relevant docs in `docs/selfhost/` if you learned something new.

---

## Active Sessions

> Format: `[Session ID] — [Task] — [Files being edited] — [Status]`
> Add your session when you start, remove when you finish.

| Session | Task | Files | Status |
|---------|------|-------|--------|
| (none) | — | — | — |

> **If a file is listed here, DO NOT edit it in another session.**
> Wait for the session to finish, or coordinate with the user.

---

## Architectural Decisions

> **Every architectural decision must be recorded here with a date.**
> If a decision is reversed, mark it as `[REVERSED]` and add the new decision.
> This is what prevents agents from following outdated docs.

### [D1] Enum payload storage — Sep 3, 2026

**Decision:** Enum payload fields that are structs/enums are stored as **pointers** (8 bytes each), not inline data.

**Rationale:**
- The C backend treats `IrType::Struct(_)` as a pointer type (`struct Foo_s*`).
- `Inst::Load` with a struct type reads only 8 bytes (the pointer).
- Storing inline struct data requires `memcpy` via `Inst::Store` with explicit `store_ty`, which is complex and error-prone.
- Storing pointers is simpler: every payload slot is 8 bytes, `Load` reads the pointer, field access works through the pointer.

**What this means for the code:**
1. `compute_enum_layout` / `Lowerer_make_enum_layout`: every field gets 8 bytes, regardless of type. `payload_slots = max_variant_arity` (number of fields), not `max_payload_size / 8`.
2. `EnumLit`: struct-typed args are heap-allocated (or already pointers from SRET), and the pointer is stored in the payload slot.
3. `pattern_binding_ir_types`: returns `word_repr(lower_type(t))` for all fields — `Struct(_)` becomes `Ptr(Struct(_))`. **[DONE in `src/lower.rs` by user]**
4. Match extraction: `Inst::Load` reads the 8-byte pointer. No `load_enum_field` needed.
5. `load_enum_field` in `src/lower.rs`: can be simplified to just `Inst::Load` for all types, or removed.

**Status:** `src/lower.rs` `pattern_binding_ir_types` changed to use `word_repr` for all types. `compute_enum_layout` and `EnumLit` in `src/lower.rs` still use inline storage — **INCONSISTENT — needs to be updated**.

**What needs to change:**
- `src/lower.rs` `compute_enum_layout`: change to 8 bytes per field (not struct size)
- `src/lower.rs` `EnumLit`: store struct args as pointers (single slot), not multi-slot inline copy
- `src/lower.rs` `load_enum_field`: remove or simplify (just `Load`)
- `compiler/lower.fd` `Lowerer_make_enum_layout`: change to 8 bytes per field
- `compiler/lower.fd` `EnumLit`: store struct args as pointers
- `compiler/lower.fd` match extraction: use `Load` (already does — no change needed)

**Previous direction [REVERSED]:** Store struct data inline in enum payloads, use `load_enum_field()` + `memcpy` to extract. This was documented in `docs/selfhost/calling-conventions.md` and `docs/selfhost/known-bugs.md` but is now outdated.

### [D2] SRET calling convention — Feb 2026

**Decision:** Functions returning structs use the SRET (struct return via hidden out-parameter) convention.

**Status:** Implemented in both `src/lower.rs` and `compiler/lower.fd`. Working correctly.

### [D3] `box_expr_unbox` in `compiler/lower.fd` is out of sync — Feb 2026

**Decision:** Must be reconciled with `compiler/ast.fd` canonical `Expr` enum.

**Status:** Not yet done. `box_expr_unbox` lists variants that don't exist in `ast.fd` (`Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`).

### [D4] PE emitter ported to Fides — Sep 10, 2026 (session 5)

**Decision:** The PE emitter (`src/pe.rs`) has been ported to Fides as
`compiler/pe.fd` (~2600 LOC). The `build` command in `main.fd` uses it to
generate native Windows PE executables without tcc.

**Rationale:**
- Eliminates the tcc dependency for Windows executable generation.
- The PE emitter is self-contained: own x86-64 encoder, own PE file writer,
  own import table, own runtime stubs (p_print_str, p_print_int, etc.).
- Brings Fides closer to the vision of a fully custom toolchain with zero
  external tools.

**What this means for the code:**
1. `compiler/pe.fd` contains `X86Encoder` (instruction encoder), `PeCodeGen`
   (IR → x86-64 code generation), and PE file writing logic.
2. `compiler/main.fd` has a `build` command that uses `PeCodeGen` instead of
   `ccompile` (which generates C and invokes tcc).
3. Binary output uses hex encoding + PowerShell decode (workaround for
   `p_file_write` truncating at null bytes).
4. `src/pe.rs` and `compiler/pe.fd` must stay in sync — if one changes, the
   other needs the same change.

**Status:** Working for simple programs (print, arithmetic, strings). Full
compiler source support is pending (struct/enum, Vec/HashMap, floats).

### [D5] comptime_eval.fd fully ported — Sep 4, 2026 (session 7)

**Decision:** The comptime evaluation engine (`src/comptime.rs`, 1345 LOC) has
been fully ported to Fides as `compiler/comptime_eval.fd` (~1500 LOC).

**Rationale:**
- The previous `comptime_eval.fd` was a 39-line stub that only registered
  the module — no actual comptime evaluation was performed.
- The full port enables `comptime gen` blocks, `comptime <expr>` substitution,
  aggressive comptime evaluation (I-5A), and loop unrolling (I-5D) in the
  self-hosted compiler.

**What this means for the code:**
1. `compiler/comptime_eval.fd` contains `process_gen`, `substitute`,
   `evaluate`, `evaluate_with_env`, `evaluate_block`, and reflection helpers.
2. `compiler/compiler.fd` calls `process_gen` and `substitute` during the
   compilation pipeline (after typecheck, before monomorphization).
3. `ComptimeEnv` uses parallel Vecs (no HashMap at compile time).
4. `box_expr_unbox` and `box_expr_new` are local to `comptime_eval.fd` since
   they were local to other modules in the Rust source.

**Status:** Builds successfully. Self-host tests pass.

### [D6] pe.fd runtime stubs expanded — Sep 4, 2026 (session 7)

**Decision:** The PE emitter's runtime stubs were expanded from 5 stubs
(print_int, print_str, panic, malloc, free) to 25+ stubs covering the
full runtime surface needed by the compiler source.

**Rationale:**
- The 5-stub minimum was sufficient for simple test programs but not for
  the full compiler source, which uses Vec, HashMap, string concatenation,
  bool/float printing, and int-to-string conversion.
- Without these stubs, the PE emitter would crash on any program using
  these runtime functions.

**Stubs added:**
- Print: `p_print_bool`, `p_print_float`
- String: `p_str_concat`, `p_to_str`, `p_to_str_str`, `p_to_str_bool`,
  `p_to_str_float`, `p_to_str_vec`
- Vec: `p_vec_new`, `p_vec_push`, `p_vec_len`, `p_vec_get`, `p_vec_pop`,
  `p_vec_set`, `p_vec_is_empty`, `p_vec_clear`, `p_vec_capacity`,
  `p_vec_first`, `p_vec_last`
- HashMap: `p_map_new`, `p_map_len`, `p_map_is_empty`, `p_map_contains`,
  `p_map_get`, `p_map_insert`, `p_map_remove`
- Encoder helper: `X86Encoder_div_r` (unsigned division)

**Status:** Builds successfully. `p_to_str_float` and `p_to_str_vec` are
simplified implementations (float uses int part + 6 fractional digits;
vec returns "[]" stub). Full implementations can be added later.

### [D7] Linux native codegen ported to Fides — Sep 4, 2026 (session 8)

**Decision:** Three Rust source files forming the Linux ELF64 native
codegen pipeline were ported to Fides:
- `compiler/peephole.fd` (985 lines, from `src/peephole.rs` 903 lines)
- `compiler/regalloc.fd` (1086 lines, from `src/regalloc.rs` 1096 lines)
- `compiler/codegen_x64.fd` (1780 lines, from `src/codegen_x64.rs` 1923 lines)

**Rationale:**
- These three files form the Linux native codegen pipeline (PIR L3 →
  register allocation → peephole optimization → x86-64 AT&T assembly).
- Porting them is a prerequisite for eliminating tcc on Linux and for
  achieving a fully self-hosted Linux backend.
- `x86.rs` (1377 lines) was identified as dead code (not called from any
  CLI command) and was NOT ported. See known-bugs.md §1.29.

**What this means for the code:**
1. All three files build successfully with the bootstrap compiler.
2. They are NOT yet integrated into `compiler.fd`/`main.fd`.
3. Integration requires porting `elf64.rs`, `linker.rs`, and
   `runtime_linux.rs` to Fides, then wiring the pipeline.
4. The Windows PE backend (`pe.rs`/`pe.fd`) is separate and self-contained
   — it does NOT use these three files.

**Adaptations made during porting:**
- `val` is a reserved keyword in Fides → renamed to `value` everywhere
  (see known-bugs.md §1.26)
- `str_to_int` → `str_parse_int` (Fides stdlib naming, see §1.27)
- `Vec::new()` field reassignment requires intermediate typed variable
  (see §1.28)
- `PhysReg` enum → `i64` constants with `physreg_name()` helper
- `Option<T>` → `Opt*` enums (`OptStr`, `OptI64`, `OptLocation`, etc.)
- Char literals → i64 byte values
- `HashMap`/`HashSet` → Vec-based maps with helper structs

**Status:** All three files build OK. Not yet wired into the pipeline.

### [D8] `.exe` extension auto-appended on Windows — Sep 4, 2026 (session 8)

**Decision:** The `ccompile` and `build` commands in `compiler/main.fd`
now automatically append `.exe` to output filenames on Windows if not
already present.

**Rationale:**
- Previously, `fides-c ccompile file.fd -o hello` produced `hello` (no
  extension), requiring a manual `Copy-Item hello hello.exe` to run it.
- The bootstrap compiler (`src/main.rs`) already defaults to `{stem}.exe`
  and preserves `.exe` in user-supplied output names.
- The self-hosted compiler currently targets Windows only (PE emitter +
  tcc with `-lws2_32`), so always appending `.exe` is correct.

**What this means for the code:**
1. New helper `ensure_exe_extension(s)` in `compiler/main.fd` — returns
   `s` unchanged if it ends with `.exe`, otherwise appends `.exe`.
2. `cmd_ccompile`: default changed from `a.out` → `a.exe`, output
   normalized via `ensure_exe_extension`. C source path derived by
   replacing `.exe` with `.c` (matching `src/main.rs` behavior).
3. `cmd_build`: output normalized via `ensure_exe_extension`.
4. `run`/`test`/`bench` commands already used hardcoded `.exe` temp
   names — no change needed.

**Status:** Done. Bug #2.9 in known-bugs.md marked as RESOLVED.

**Future note:** If Linux support is added to the self-hosted compiler,
this should be gated on a platform check rather than always appending
`.exe`.

### [D9] LSP server ported to Fides — Sep 4, 2026 (session 9)

**Decision:** The LSP server (`src/lsp.rs`, 996 LOC) has been ported to
Fides as `compiler/lsp.fd` (1084 lines). The `lsp` command has been
added to `compiler/main.fd`, dispatching to `lsp_run()`.

**What was ported:**
- JSON-RPC 2.0 over stdio with Content-Length header framing
- `initialize` / `shutdown` / `exit` lifecycle
- `textDocument/didOpen` / `didChange` / `didClose` — document tracking
- `textDocument/publishDiagnostics` — via `get_diagnostics()` from `compiler.fd`
- `textDocument/completion` — keywords + functions + structs + enums +
  consts + type aliases + traits + dot-access methods/fields
- `textDocument/hover` — function/struct/enum/const/type/trait/method signatures
- `textDocument/definition` — source text search for definition location
- `textDocument/rename` — whole-word occurrence search + WorkspaceEdit

**Runtime I/O functions added to `compiler/codegen_c.fd`:**
- `p_read_line` — read a line from stdin (NULL on EOF)
- `p_read_stdin(n)` — read exactly N bytes from stdin (for LSP body)
- `p_stdin_eof` — feof check (detect client disconnect)
- `p_flush_stdout` — fflush(stdout) for immediate message delivery

**Status:** Module parses correctly. Full self-hosted build works
(session 15: bootstrap fixpoint achieved). The `lsp` command and
`lsp_run()` entry point are wired into `main.fd`.

---

### [D10] flat_ast.fd ported + Box<T> unbox helpers centralized — Sep 4, 2026 (session 9)

**Decision:** `src/flat_ast.rs` (755 LOC) has been ported to Fides as
`compiler/flat_ast.fd` (1260 lines). This module provides flat AST
serialization for the `dump-ast-flat` command and self-host parser
differential testing.

**Box<T> unbox helpers centralized in ast.fd:**
- Added `pub fn box_expr_unbox`, `box_type_unbox`, `box_pattern_unbox` to
  `compiler/ast.fd`
- These were previously duplicated across 6+ modules (typechecker.fd,
  comptime_eval.fd, parser.fd, lower.fd, ir.fd, codegen_c.fd)
- Now available as shared public functions from `ast.fd`

**Key finding: Box<T> deref gap**
- `Box<T>` is `Generic("Box", [T])` in Fides — not a pointer type
- `*` (Deref) only works on `Type::Ref` and `Type::RawPtr`
- Workaround: `box_*_unbox()` helpers (match on all variants, return unboxed)
- Proper fix: `Deref` trait system (future work)

**interp.rs NOT ported:**
- The self-hosted compiler doesn't need an interpreter
- `run` command uses C compilation + tcc
- Comptime evaluation handled by `comptime_eval.fd` (AST-level)
- `interp.rs` (5782 lines) has features not needed for self-hosting

**Status:** `flat_ast.fd` builds successfully. `ast.fd` changes verified —
all 22/23 self-host tests still pass (1 pre-existing failure in
`test_codegen_c.fd` due to missing `p_sb_new` runtime function).

---

### [D11] Category B codegen parity gaps resolved — Sep 5, 2026 (session 14)

**Decision:** The Category B code generation parity gaps between the Rust
compiler and the self-hosted compiler have been resolved:

- **B5 (Float constant bit-pattern) — FIXED:** Added `p_f64_to_bits`/
  `p_f32_to_bits` C runtime functions to both `src/codegen_c.rs` and
  `compiler/codegen_c.fd`. Registered as externs in both `src/lower.rs`
  and `compiler/lower.fd`. Updated `ConstFloat` handler in
  `compiler/codegen_c.fd` to emit union compound literals
  (`((union { uint64_t u; double f; }){ .u = <bits>ULL }).f`) matching
  the Rust codegen. Implemented `f64_to_bits`/`f32_to_bits` in
  `compiler/lower.fd` (were stubs returning 0). NaN/Inf/-0.0 bit patterns
  are now preserved.

- **B6 (Polymorphic print/println dispatch) — AT PARITY:** Verified via
  side-by-side comparison. All type cases (Vec, HashMap, Tensor, F64, Bool,
  Ptr/Opaque, Generic, any, default) match between Rust and Fides codegen.
  No changes needed.

- **B7 (Missing runtime sigs) — AT PARITY:** `p_malloc_persistent`,
  `p_arena_save`, `p_arena_restore` are registered in `compiler/codegen_c.fd`
  with matching parameter types. No changes needed.

- **B8 (Vec method word_repr) — AT PARITY:** `Lowerer_lower_method_call`
  in `compiler/lower.fd` uses `word_repr(vec_elem_ty)` for
  get/pop/first/last/remove, matching Rust. The `infer_expr_type` path
  returns raw `elem_ty` in both compilers — this is intentional parity
  by design. No changes needed.

**Files changed:**
- `src/codegen_c.rs` — added `p_f64_to_bits`/`p_f32_to_bits` (sigs, decls, impls)
- `src/lower.rs` — registered `p_f64_to_bits`/`p_f32_to_bits` externs
- `compiler/codegen_c.fd` — added runtime functions + updated ConstFloat handler
- `compiler/lower.fd` — registered externs + implemented `f64_to_bits`/`f32_to_bits`

**Remaining Category B gaps (B1-B4, B9-B10):** Low priority — not used by
the compiler source. See `docs/roadmap/selfhost-roadmap.md` Category B table.

---

## Key Findings

> Discoveries that affect the direction. Add new findings at the top.

### [F11] BOOTSTRAP FIXPOINT ACHIEVED — Sep 5, 2026 (session 15)

**fides-c2.c == fides-c3.c** (SHA256-identical, 701,100 lines each). The
self-hosted compiler compiles itself to byte-for-byte identical output
across generations. This is the ultimate goal of self-hosting.

**Bootstrap chain verified:**
```
Rust compiler → fides-c.c (706,318 lines) → fides-c.exe
fides-c.exe   → fides-c2.c (701,100 lines) → fides-c2.exe
fides-c2.exe  → fides-c3.c (701,100 lines) → fides-c3.exe  ← IDENTICAL to c2
fides-c3.exe  → compiles test_simple.fd correctly (EXIT 0)
```

**Four bugs were fixed in session 15 to achieve this:**
1. **TCC 4KB stack overflow** (bug 1.33) — TCC's default stack was 4KB,
   not 1MB as previously believed. Fixed by adding `-Wl,-stack,268435456`
   (256MB) to the TCC compile command.
2. **type_eq pointer == on strings** (bug 1.34) — `type_eq` used `==` for
   string comparison in `Type::Named`, `Type::Generic`, `Type::Dyn`, and
   `Type::Assoc`. Replaced with `str_equal`.
3. **Large match expression codegen** (bug 1.35) — `subst_expr` and
   `subst_stmt` in `mono.fd` had large match expressions that caused
   codegen issues. Split into helper functions.
4. **Non-ASCII characters in compiler source** (bug 1.36) — Unicode
   characters (em-dash, arrows, box-drawing) in comments and string
   literals caused fixpoint divergence. Replaced with ASCII equivalents.

**What's NOT yet done:**
- Rust vs self-hosted parity gap (c1=706K lines vs c2=701K lines, ~5K
  line difference). This does NOT block the self-hosted fixpoint but
  means the Rust and self-hosted compilers produce slightly different C.
- 287 `p_eprint_str` debug prints still in compiler source (go to stderr,
  don't affect C output or fixpoint).
- Category C/D pipeline features (interpretation, native build, WASM,
  safety verification) — not needed for fixpoint.

### [F10] Large-function body truncation analysis — RESOLVED Sep 5, 2026 (session 15)

**Initial finding:** Second-generation C output (`stage2.c`) was truncated
at 361,955 lines (vs 701,100 expected). 1278 of 1881 function bodies were
missing. Truncation occurred at `void* v3` in `subst_expr` variable
declarations.

**Resolution:** The truncation was caused by the three bugs above (TCC
stack overflow, type_eq pointer comparison, large match codegen). After
fixing all three, the second-generation compiler produces full 701,100-line
output with 1795 function bodies (105 missing = expected external functions).

The earlier hypothesis of "value_metas Vec corruption" was incorrect —
the actual cause was the TCC 4KB stack overflow causing silent corruption
during code generation.

### [F9] debugger.rs porting assessment — NOT feasible without interp.rs — Sep 4, 2026 (session 9)

**Assessment:** Porting `src/debugger.rs` (914 LOC) to Fides is **not
feasible** as a standalone task because it has a hard dependency on the
interpreter (`src/interp.rs`, 5782 LOC).

**Dependency chain:**
```
debugger.rs (914 LOC)
  └─ imports from interp.rs:
     ├─ Interpreter (struct with 50+ fields)
     ├─ RunResult (enum: Return/Pause/Panic)
     ├─ DebugAction (enum: Continue/Pause/InjectReturn/InjectPanic)
     ├─ Value (enum: Int/Float/Bool/Str/Ptr/Void/Vec/Map/Dyn/Tensor/Json/...)
     └─ TraceEntry (struct: func_name, block_id, inst_id, source_line, value_repr)
  └─ uses:
     ├─ Interpreter::new(module) — constructs interpreter from IrModule
     ├─ interp.set_debug_hook(closure) — installs callback before each instruction
     ├─ interp.run() — executes main function
     └─ closure receives (func_name, block, source_line, inst_id, values, func)
```

**What interp.rs contains (5782 LOC):**
- `Value` enum (14 variants, 130 LOC) — runtime values
- `Memory` struct (110 LOC) — simulated heap with byte addressing
- `Interpreter` struct (155 LOC, 50+ fields) — execution state
- `Interpreter::new()` (65 LOC) — initialization
- `Interpreter::run()` (10 LOC) — entry point
- `run_function()` (225 LOC) — function call dispatch + block iteration
- `execute_inst()` (888 LOC) — IR instruction evaluator (all Inst variants)
- `call_runtime()` (3247 LOC) — 251 runtime function handlers (p_print_*, p_vec_*,
  p_map_*, p_malloc, p_tensor_*, p_json_*, p_db_*, p_chan_*, p_mutex_*, p_tls_*,
  p_future_*, p_spawn, p_autograd_*, p_dp_*, p_quant_*, p_kv_cache_*, etc.)
- `eval_binop()` (186 LOC), `eval_unaryop()` (26 LOC), `cast_value()` (13 LOC)
- Autograd tape, tensor ops, futures, threads, channels, ECS, databases,
  DP noise, TP simulation, KV cache, quantization

**Why it's not needed for self-hosting:**
1. The self-hosted compiler's `run` command uses C compilation + tcc, not
   the interpreter
2. Comptime evaluation is handled by `comptime_eval.fd` (AST-level
   interpreter, already ported)
3. The `debug` and `replay` commands are developer tools, not required
   for bootstrap fixpoint
4. The self-hosted compiler's `main.fd` has no `debug`/`replay` commands

**What would be needed to port it:**
1. Port `interp.rs` (5782 LOC) → `compiler/interp.fd` — massive effort,
   ~8000+ LOC Fides. Would need:
   - `Value` enum with all 14 variants
   - `Memory` struct with byte-level load/store
   - `Interpreter` struct with 50+ fields
   - `execute_inst()` — all IR instruction handling
   - `call_runtime()` — all 251 runtime function handlers
   - Closures for debug hooks (Fides has no closures — would need a
     different mechanism, e.g. function pointers or a callback struct)
2. Port `debugger.rs` (914 LOC) → `compiler/debugger.fd`
3. Add `debug`/`replay` commands to `main.fd`

**Estimated effort:** 8000+ LOC Fides for interp.fd + 1200 LOC for
debugger.fd = ~9200 LOC total. Multiple sessions.

**Recommendation:** **Skip for now.** The debugger is a developer
convenience tool, not a self-hosting requirement. Port it only if:
- The self-hosted compiler needs interactive debugging capability
- Or if a future `run` command redesign switches from C compilation to
  a native interpreter (unlikely — C compilation is faster)

### [F8] Pre-existing parse error blocks self-hosted rebuild — RESOLVED Sep 5, 2026 (session 15)

The original parse error at `codegen_c.fd:973` ("expected identifier but got
'var'") was resolved in earlier sessions. The self-hosted build pipeline
builds `fides-c.exe` successfully.

**RESOLVED (session 15):** All Category A crash blockers resolved. The
"TypeChecker_lookup null deref" was actually a TCC 4KB stack overflow
(bug 1.33), not a null pointer issue. Fixed by adding
`-Wl,-stack,268435456` to the TCC compile command. The bootstrap fixpoint
is now achieved (fides-c2.c == fides-c3.c).

### [F7] Statement-dropping bug and fides-c2 crash — RESOLVED Sep 5, 2026 (session 15)

The `STATUS_ACCESS_VIOLATION` crash in `fides-c2_test.exe` while lexing
`stdlib/core.fd` was initially attributed to the **statement-dropping bug**
(bug 1.19 in known-bugs.md). Session 12 investigation found the primary
cause was **stack overflow** — the TCC-linked binary had a 4KB default stack
(not 1MB as previously believed). Fix: increased TCC stack reserve to 256MB
(`-Wl,-stack,268435456`) and split `keyword_or_ident_core` (51 if-statements,
933 C vars) into 5 smaller functions.

The statement-dropping bug (1.19) was also addressed in session 15 by
splitting `subst_expr` and `subst_stmt` in `mono.fd` into helper functions.
After all fixes, the second-generation compiler produces full 701,100-line
output with 1795 function bodies (105 missing = expected external functions).

**RESOLVED (session 15):** Bootstrap fixpoint achieved. No more crashes.
fides-c2.c == fides-c3.c (SHA256-identical).

### [F6] Linux native codegen ported to Fides — Sep 4, 2026 (session 8)

Three files ported from Rust to Fides, all compile successfully:
- `compiler/peephole.fd` (985 lines) — 9 peephole optimization passes
- `compiler/regalloc.fd` (1086 lines) — linear scan register allocator
- `compiler/codegen_x64.fd` (1780 lines) — PIR L3 → x86-64 AT&T assembly
  (System V AMD64 ABI)

These form the Linux ELF64 native codegen pipeline. NOT yet integrated into
`compiler.fd`/`main.fd` — requires porting `elf64.rs`, `linker.rs`, and
`runtime_linux.rs` first.

**Key finding:** `x86.rs` (1377 lines) is dead code — not called from any
CLI command in `main.rs`. The actual Windows backend is `pe.rs` (5704 lines),
which is self-contained and does NOT use `codegen_x64.rs`/`regalloc.rs`/
`peephole.rs`. See known-bugs.md §1.29.

### [F5] PE emitter ported to Fides — Sep 10, 2026 (session 5)

`compiler/pe.fd` (~2600 LOC) is a self-contained x86-64 code generator + PE
file writer, ported from `src/pe.rs` (5704 lines of Rust). The `build`
command in `main.fd` uses it to generate native Windows PE executables
directly from `.fd` source — no tcc, no gcc, no external linker.

**Working:** `fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe`
→ prints "Hello from PE!", exit code 0.

**Three bugs were fixed during the port:**
1. `X86Encoder_mov_imm32` ModRM bug — `mod=0` (memory) instead of `mod=3`
   (register), generating `mov [rcx], imm32` instead of `mov rcx, imm32`.
2. `Inst::Print` type dispatch — always called `p_print_int` regardless of
   argument type. Fixed to dispatch by IR type (Ptr→str, F64→float, Bool→bool).
3. Fides compiler statement-dropping bug — long functions silently lose
   statements. Worked around by splitting `gen_print_str` into smaller helpers.

**Binary file output:** `p_file_write` truncates at null bytes. Solved via
hex encoding the PE bytes and using PowerShell to decode to binary.

**Remaining for full compiler source:** struct/enum support, Vec/HashMap
runtime, float arithmetic, many-arg function calls. See SH-13 sub-tasks in
`docs/roadmap/selfhost-roadmap.md`.

### [F4] Self-compilation works — Sep 9, 2026 (session 4)

`fides-c.exe` successfully compiles `compiler/main.fd` → `fides-c2.exe`.
Six bugs were fixed to achieve this (see `docs/selfhost/known-bugs.md`
sections 1.11-1.16). The generated `fides-c2.exe` correctly compiles
`test_strconcat.fd` → "hello world".

**Remaining for fixpoint:** fides-c2.exe crashes on stdlib (statement-dropping
bug — root cause confirmed session 8), debug prints need gating.
~~`.exe` extension missing~~ — FIXED (session 8, see [D8]).

### [F3] Fides lowerer `EnumLit` does not do multi-slot copy — Sep 3, 2026

`compiler/lower.fd` line 3337 stores each enum arg at field index `ai + 1` as a single 8-byte value:
```fides
var data_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(alloca, ai + 1, IrType::I64));
var _s2: i64 = Lowerer_emit_inst(self, Inst::Store(data_ptr, arg_val, OptIrType::None));
```

The Rust lowerer (`src/lower.rs` lines 4650-4668) does multi-slot copy for struct-typed args. The Fides lowerer does not. This means the Fides lowerer already loses struct data on enum construction, not just extraction.

**With the pointer-based approach [D1], this bug goes away** — every arg is a single 8-byte pointer, so single-slot store is correct.

### [F2] `c_type(IrType::Struct(name))` returns a pointer type — Feb 2026

`compiler/codegen_c.fd` line 146:
```fides
IrType::Struct(name) => { return "struct " + name + "_s*"; }
```

This means `Inst::Load` with `IrType::Struct(_)` reads 8 bytes (a pointer), not the full struct. This is the root cause of the enum variant extraction bug.

**With the pointer-based approach [D1], this is correct** — the payload slot stores a pointer, and `Load` reads it.

### [F1] `Inst::Store` in Fides codegen does not emit `memcpy` for structs — Feb 2026

`compiler/codegen_c.fd` line 1961 ignores the `store_ty` parameter and treats all stores as scalar. The Rust codegen (`src/codegen_c.rs` lines 1498-1556) handles `IrType::Struct(name)` with `memcpy`.

**With the pointer-based approach [D1], this doesn't matter** — struct stores are always 8-byte pointer stores.

---

## Pre-Flight Checklist (Every Session)

Before touching any file:

- [ ] Read this file (`docs/selfhost/COORDINATION.md`)
- [ ] Read `CURRENT.md`
- [ ] Check "Active Sessions" — no conflicts with your planned files
- [ ] Check "Architectural Decisions" — no direction changes since last time
- [ ] Run `./verify.sh` to confirm baseline
- [ ] Add your session to "Active Sessions"

---

## File Ownership Map

> Which files are "owned" by which concern. If you're working on one of these
> files, you need to understand the full context.

| File | Concern | Coordinated by |
|------|---------|----------------|
| `src/lower.rs` | Enum layout, SRET, match extraction | This file |
| `compiler/lower.fd` | Fides lowerer — must mirror `src/lower.rs` | This file |
| `src/codegen_c.rs` | C codegen — struct store, SRET | This file |
| `compiler/codegen_c.fd` | Fides C codegen — must mirror `src/codegen_c.rs` | This file |
| `src/pe.rs` | Rust PE emitter — x86-64 codegen + PE file format | This file |
| `compiler/pe.fd` | Fides PE emitter — must mirror `src/pe.rs` | This file |
| `src/peephole.rs` | Rust peephole optimizer — 9 passes | This file |
| `compiler/peephole.fd` | Fides peephole — must mirror `src/peephole.rs` | This file |
| `src/regalloc.rs` | Rust register allocator — linear scan | This file |
| `compiler/regalloc.fd` | Fides register allocator — must mirror `src/regalloc.rs` | This file |
| `src/codegen_x64.rs` | Rust x86-64 codegen (Linux, AT&T syntax) | This file |
| `compiler/codegen_x64.fd` | Fides x86-64 codegen — must mirror `src/codegen_x64.rs` | This file |
| `src/lsp.rs` | Rust LSP server — JSON-RPC over stdio | This file |
| `compiler/lsp.fd` | Fides LSP server — must mirror `src/lsp.rs` | This file |
| `compiler/main.fd` | CLI entry point — `.exe` extension, command dispatch, `lsp` command | This file |
| `compiler/ast.fd` | Canonical AST — changes require updating `box_*_unbox` everywhere | This file |
| `CURRENT.md` | Current status — update after every milestone | Session that completes a milestone |

---

## Session Protocol

### Starting a session
1. Read this file + `CURRENT.md` + `AGENTS.md`.
2. Pick a task from `CURRENT.md` "Next steps" or from the roadmap.
3. Add your session to "Active Sessions" with the files you'll touch.
4. Run `./verify.sh`.

### Making architectural decisions
1. Add the decision to "Architectural Decisions" with a date and rationale.
2. If it reverses a previous decision, mark the old one as `[REVERSED]`.
3. Update any docs that reference the old direction.
4. Commit the decision before proceeding with implementation.

### Finishing a session
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `CURRENT.md` with what you did and the new state.
3. Update relevant docs in `docs/selfhost/`.
4. Remove your session from "Active Sessions".
5. Commit your changes.

---

## Common Mistakes to Avoid

1. **Following outdated docs.** Always check "Architectural Decisions" first. The docs in `docs/selfhost/` may be stale.

2. **Editing files another session is working on.** Check "Active Sessions" first.

3. **Changing direction without recording it.** If you reverse an architectural decision, record it here immediately so other agents don't follow the old direction.

4. **Not running `./verify.sh` before and after.** You might be starting from a broken state, or you might break something without knowing.

5. **Updating `src/lower.rs` without updating `compiler/lower.fd`.** These must stay in sync. If you change one, the other needs the same change.

6. **Updating `src/codegen_c.rs` without updating `compiler/codegen_c.fd`.** Same — these must stay in sync.

7. **Updating `src/pe.rs` without updating `compiler/pe.fd`.** Same — these must stay in sync. The PE emitter is now ported to Fides (session 5).
