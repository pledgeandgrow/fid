# Fides Self-Hosted Compiler — Known Bugs, Gotchas, and History

> Things that have broken before and how to avoid re-introducing them.
>
> Last updated: Sep 5, 2026 (session 14 — Category B codegen parity: B5 fixed, B6/B7/B8 verified at parity).

---

## 1. Fixed Bugs (Do Not Re-Introduce)

### 1.30 Float constant bit-pattern not preserved in C codegen (session 14 — B5)

**Symptom:** The self-hosted C codegen emitted `(double)float_to_str(value)`
for `Inst::ConstFloat`, which loses NaN payloads, -0.0 sign, and Inf bit
patterns. The Rust codegen uses a union compound literal
(`((union { uint64_t u; double f; }){ .u = <bits>ULL }).f`) to preserve
exact IEEE 754 bit patterns. This caused fixpoint divergence (stage2.c ≠
stage3.c) for any code containing float constants.

**Root cause:** The Fides `Inst::ConstFloat(f64, IrType)` stores the f64
value directly (not bits like Rust's `ConstFloat { bits: u64, ty }`).
The codegen had a TODO comment: "Add p_float_to_bits to runtime for exact
bit preservation." The `f64_to_bits` helper in `compiler/lower.fd` was a
stub returning 0.

**Fix (session 14):**
1. Added `p_f64_to_bits`/`p_f32_to_bits` C runtime functions to both
   `src/codegen_c.rs` and `compiler/codegen_c.fd` (forward declarations,
   signatures, and union-based implementations).
2. Registered them as externs in both `src/lower.rs` (`register_runtime`)
   and `compiler/lower.fd` (`Lowerer_register_runtime`).
3. Implemented `f64_to_bits`/`f32_to_bits` in `compiler/lower.fd` (were
   stubs returning 0) — they now call `p_f64_to_bits`/`p_f32_to_bits`.
4. Updated the `ConstFloat` handler in `compiler/codegen_c.fd` to emit
   union compound literals matching the Rust codegen, using
   `p_f64_to_bits(value)` / `p_f32_to_bits(value)` to get the bit pattern.

**Verification:** `fidesc dump-c` now shows
`vN = ((union { uint64_t u; double f; }){ .u = 9218868437227405311ULL }).f;`
for NaN constants (0x7FF8000000000000). `fidesc run` correctly prints
`NaN`, `inf`, `-inf`, `-0`, `3.141592653589793` for special float values.

**Lesson:** When the Fides IR stores a value differently than the Rust IR
(f64 vs bits:u64), the codegen needs a runtime helper to convert at
codegen time. The union compound literal approach preserves exact bit
patterns that decimal string conversion cannot.

### 1.26 `val` is a reserved keyword in Fides (session 8)

**Symptom:** Parse error `expected identifier but got 'val'` when building
`regalloc.fd`.

**Root cause:** `val` is a keyword in Fides (tokenized as `TokenKind::Val`
in `src/lexer.rs`). Using it as a variable or parameter name causes a parse
error.

**Fix (session 8):** Renamed all `val` variables/parameters to `value` in
`regalloc.fd` and `codegen_x64.fd`. Field accesses like `entry.val` are fine
(field names, not variable bindings).

**Lesson:** When porting Rust to Fides, check `src/lexer.rs` for the keyword
list. `val` is the main surprise — Rust uses it freely as a variable name.

### 1.27 `str_to_int` doesn't exist in Fides stdlib (session 8)

**Symptom:** Type error `undefined function 'str_to_int'` when building
`codegen_x64.fd`.

**Root cause:** The Fides stdlib function is named `str_parse_int`, not
`str_to_int`.

**Fix (session 8):** Replaced `str_to_int` with `str_parse_int` in
`codegen_x64.fd`.

### 1.28 `Vec::new()` type inference fails on field reassignment (session 8)

**Symptom:** Type error `cannot assign Vec<i64> to field 'block_labels' of
type Vec<BlockLabelEntry>` when building `codegen_x64.fd`.

**Root cause:** Fides cannot infer the element type when assigning
`self.field = Vec::new()` directly. The type inference defaults to
`Vec<i64>` instead of the field's declared type.

**Fix (session 8):** Use an intermediate typed variable:
```fides
var new_labels: Vec<BlockLabelEntry> = Vec::new();
self.block_labels = new_labels;
```

### 1.29 `x86.rs` is dead code — not needed for porting (session 8)

**Finding:** `src/x86.rs` (1377 lines) is a Windows MASM stack-machine
backend that is NOT called from any CLI command in `main.rs`. The only
callers are `build_source()` and `build_source_with_path()` in `compiler.rs`,
neither of which is invoked from the CLI. The actual Windows backend is
`pe.rs` (5704 lines), which is self-contained and does NOT use `x86.rs`.

**Decision:** Skip porting `x86.rs`. The three files that matter for native
codegen are `peephole.rs`, `regalloc.rs`, and `codegen_x64.rs` (Linux), all
now ported. For Windows, `pe.rs` is the relevant backend (already partially
ported as `pe.fd`).

### 1.22 I64Pair struct field mismatch — opt.fd vs compiler.fd (session 7)

**Symptom:** Build errors in `opt.fd` when accessing `I64Pair` fields —
field names didn't match between modules.

**Root cause:** `opt.fd` defined `I64Pair` with fields `key`/`val`, while
`compiler.fd` defined it with `first`/`second`. Code in `opt.fd` accessed
`.key`/`.val` but the struct layout from `compiler.fd` used `.first`/
`.second`.

**Fix (session 7):** Unified `opt.fd`'s `I64Pair` definition and all
usages to use `first`/`second`, matching `compiler.fd`.

### 1.23 Visibility errors — str_starts_with and binop_eq in opt.fd (session 7)

**Symptom:** `undefined function 'str_starts_with'` / `undefined function
'binop_eq'` when building `opt.fd`.

**Root cause:** `str_starts_with` and `binop_eq` were private functions
in `opt.fd` but needed to be called from other modules.

**Fix (session 7):** Made both functions `pub` in `opt.fd`.

### 1.24 comptime_eval.fd was a 39-line stub (session 7)

**Symptom:** No comptime evaluation was performed by the self-hosted
compiler — `comptime gen` blocks, `comptime <expr>` substitution, and
loop unrolling were all no-ops.

**Root cause:** `comptime_eval.fd` was a placeholder that only registered
the module name. The actual evaluation logic was never ported from
`src/comptime.rs`.

**Fix (session 7):** Full port of `src/comptime.rs` (1345 LOC) to
`compiler/comptime_eval.fd` (~1500 LOC). Includes `process_gen`,
`substitute`, `evaluate`, `evaluate_with_env`, `evaluate_block`,
`evaluate_reflect`, I-5A aggressive comptime, I-5D loop unrolling, and
reflection helpers.

### 1.25 pe.fd missing runtime stubs (session 7)

**Symptom:** PE emitter only had 5 runtime stubs (print_int, print_str,
panic, malloc, free). Programs using Vec, HashMap, string concat, bool/
float printing, or int-to-string conversion would crash.

**Root cause:** `pe.fd` was ported with only the minimum stubs needed for
simple test programs. The full compiler source requires ~25 runtime stubs.

**Fix (session 7):** Added 20+ runtime stubs to `pe.fd`:
- Print: `p_print_bool`, `p_print_float`
- String: `p_str_concat`, `p_to_str`, `p_to_str_str`, `p_to_str_bool`,
  `p_to_str_float`, `p_to_str_vec`
- Vec: `p_vec_new`, `p_vec_push`, `p_vec_len`, `p_vec_get`, `p_vec_pop`,
  `p_vec_set`, `p_vec_is_empty`, `p_vec_clear`, `p_vec_capacity`,
  `p_vec_first`, `p_vec_last`
- HashMap: `p_map_new`, `p_map_len`, `p_map_is_empty`, `p_map_contains`,
  `p_map_get`, `p_map_insert`, `p_map_remove`
- Encoder: `X86Encoder_div_r` (unsigned division)

**Note:** `p_to_str_float` is a simplified implementation (int part + 6
fractional digits). `p_to_str_vec` returns "[]" as a stub. Full
implementations can be added later.

### 1.21 Cross-module enum constructors parsed as MethodCall (session 6)

**Symptom:** `Type error (line 167): undefined variable 'OptStr'` / `cannot
call method 'Some' on type never` during self-compilation of `compiler/main.fd`.

**Root cause:** `OptStr::Some(input)` in `compiler/main.fd` was parsed as
`MethodCall(Ident("OptStr"), "Some", [input])` instead of
`EnumLit("OptStr", "Some", [input])` because the parser's `is_enum_name()`
check only knew about enums defined in the current module — not cross-module
enums like `OptStr` (defined in `compiler/ast.fd`). The re-parse step (which
runs after all imports are loaded) does produce `EnumLit`, but the typechecker
and lowerer still needed fallback handling for `MethodCall` on enum names.

**Fix (session 6):** Added enum name checks in three places:
1. `compiler/typechecker.fd` — `check_method_call`: if receiver is `Ident(name)`
   and `name` is in `self.enums`, return `Type::Named(name)` (enum constructor).
2. `compiler/lower.fd` — `MethodCall` lowering: if receiver is `Ident(name)` and
   `Lowerer_lookup_enum(self, name)` succeeds, delegate to
   `Lowerer_lower_expr(Expr::EnumLit(name, method, args))`.
3. `compiler/lower.fd` — `infer_expr_type` for `MethodCall`: if receiver is an
   enum name, return `IrType::Struct(name)`.

**Lesson:** The parser's `is_enum_name()` only knows current-module enums. The
re-parse step helps, but the typechecker and lowerer must also handle
`MethodCall` on enum names as a fallback for cross-module enum constructors.

### 1.22 Duplicate `p_file_write_vec` in C codegen (session 6)

**Symptom:** `tcc: error: redefinition of 'p_file_write_vec'` during self-compile.

**Root cause:** `emit_runtime_impl` in `compiler/codegen_c.fd` emitted the
`p_file_write_vec` function body, which was already emitted by
`emit_file_write_bin_impl`. Both functions wrote the same C function definition.

**Fix (session 6):** Removed the duplicate from `emit_runtime_impl`. The
function is only emitted once now (by `emit_file_write_bin_impl`).

### 1.23 O(N²) import resolution with linear name scans (session 6)

**Symptom:** Self-compile took ~238s just for import resolution, dominated by
`c_str_vec_contains` linear scans over `loaded_module_names` (18 modules),
`struct_names` (305+ names), and `enum_names` (305+ names).

**Root cause:** Every import resolution step called `c_str_vec_contains` to
check if a name was already loaded — O(n) per check, O(N²) overall with 18
modules and 305+ type names.

**Fix (session 6):** Added three HashMaps in `compiler/compiler.fd`:
`loaded_module_set`, `struct_name_set`, `enum_name_set`. Replaced
`c_str_vec_contains` calls with `HashMap_get` / `HashMap_insert` for O(1)
lookups. Import resolution time dropped from ~238s to negligible.

**Lesson:** Use HashMap (O(1)) instead of Vec linear scan (O(n)) for membership
checks when the collection grows beyond ~20 items.

### 1.17 `X86Encoder_mov_imm32` ModRM bug — memory store instead of register load (session 5)

**Symptom:** Generated PE executables crashed with `ACCESS_VIOLATION`.

**Root cause:** `X86Encoder_mov_imm32` in `compiler/pe.fd` used `X86Encoder_modrm(self, 0, 0, reg_code(r))` — `mod=0` is memory mode, which generates `mov [rcx], imm32` (a store to the address in RCX) instead of `mov rcx, imm32` (a register load). The Rust reference (`src/pe.rs`) correctly uses `mod=0b11` (register mode).

**Fix (session 5):** Changed to `X86Encoder_modrm(self, 3, 0, reg_code(r))` — `mod=3` is register mode.

**Lesson:** Always cross-check ModRM byte encoding against the Rust reference. The `mod` field is bits 7-6 of the ModRM byte: `00`=memory, `11`=register.

### 1.18 `Inst::Print` always called `p_print_int` regardless of type (session 5)

**Symptom:** Generated PE executables crashed when printing strings (called `p_print_int` with a string pointer, causing `ACCESS_VIOLATION`).

**Root cause:** The `Inst::Print` handler in `PeCodeGen_gen_print_inst` (`compiler/pe.fd`) unconditionally called `p_print_int` via `PeCodeGen_get_func_label(self, "p_print_int")`, ignoring the `ty: IrType` parameter.

**Fix (session 5):** Added a `match ty` dispatch:
- `IrType::Ptr(_)` → `p_print_str`
- `IrType::F64` → `p_print_float`
- `IrType::Bool` → `p_print_bool`
- default → `p_print_int`

### 1.19 Fides compiler silently drops statements in long functions (session 5)

**Symptom:** `X86Encoder_mov_imm32` calls vanished from the generated C code when inside long functions like `PeCodeGen_gen_print_str`, causing the PE emitter to produce broken machine code.

**Root cause:** The Fides compiler has a bug where it silently drops statements in long functions (likely a buffer overflow or index tracking issue in the lowerer/codegen). This is a **compiler bug**, not a code bug.

**Workaround (session 5):** Split long functions into smaller helper functions. `gen_print_str` was split into `emit_get_std_handle`, `emit_write_file`, `gen_print_str_strlen`, and `gen_print_str_write`. Each helper is short enough that all statements are preserved.

**Note:** This bug affects any long function in Fides source. If you see missing statements in generated C, check if the function is too long and split it.

### 1.20 `p_file_write` truncates binary data at null bytes (session 5)

**Symptom:** PE executables generated by the `build` command were truncated at the first null byte, producing corrupt files.

**Root cause:** `p_file_write` in the Fides runtime uses C string functions internally, which stop at null bytes. Binary PE data contains many null bytes.

**Fix (session 5):** Hex-encode the binary data as a string, write the hex string to a file, then use PowerShell to decode the hex file to binary:
```
[byte[]]$bytes = ($hex -split '(.{2})' | Where-Object { $_ } | ForEach-Object { [Convert]::ToByte($_, 16) })
[System.IO.File]::WriteAllBytes($output, $bytes)
```

### 1.11 Enum `==` comparison compares pointers not tags (session 4)

**Symptom:** `cannot assign i64 to field 'out' of type str` — the typechecker
was miscompiling because `op == BinOp::Add` silently produced wrong results.

**Root cause:** Enum literals lower to pointers (alloca addresses), while
pattern-matched enum values are tag values (integers). `==` on enums
compares pointer vs tag — always false (or always true, depending on layout).

**Fix (session 4):** Replaced all `== EnumVariant` in the compiler source with
manually-written `*_eq()` helper functions that do tag-based comparison via
`match`:
- `binop_eq` in `ast.fd`
- `ir_type_eq` in `ir.fd`
- `borrow_kind_eq` in `ir.fd`
- `region_ref_eq`, `mem_region_eq`, `pir_level_eq` in `pir.fd`

**Latent bug:** `==` on enums is still broken in the Fides language itself.
This is a language-level bug, not a compiler bug. All compiler source code
now avoids `==` on enums.

### 1.12 O(n²) string concatenation in C codegen (session 4)

**Symptom:** `p_malloc: OOM — failed to allocate new arena chunk` during
C codegen of the 39.5K LOC compiler source (generates ~400K lines of C).

**Root cause:** `wln()` in `compiler/codegen_c.fd` used `self.out = self.out + s + "\n"`.
Each concatenation allocates a new arena chunk and copies the entire previous
string. For 400K lines, this is O(n²) in memory.

**Fix (session 4):** Added `p_sb_new`/`p_sb_append`/`p_sb_to_str` mutable
string builder runtime functions that use `malloc`+`realloc`. The `wln()`
function now appends to a string builder instead of concatenating.

### 1.13 Duplicate function definitions in generated C (session 4)

**Symptom:** `tcc: error: incompatible types for redefinition of 'Vec_new'`

**Root cause:** `IrModule_add_function` pushed new entries without checking
for duplicates. Pre-registration added a stub `Vec_new()`, then lowering added
the real `Vec_new(void* env)`. Both survived → tcc saw two definitions.

**Fix (session 4):** `IrModule_add_function` now checks if a function with
the same name already exists and replaces it instead of pushing a duplicate.

### 1.14 `p_str_equal` called without `__env` parameter (session 4)

**Symptom:** `tcc: error: too few arguments to function 'p_str_equal'`

**Root cause:** `p_str_equal` is a user function (defined in `stdlib/core.fd`)
that takes `(env_ptr, a: i64, b: i64)`. The Fides lowerer was calling it with
only 2 arguments (a, b), missing the `__env` dummy pointer.

**Fix (session 4):** Added `dummy_env` (ConstInt(0, I64)) to all 3 call sites
in `compiler/lower.fd`:
- String equality: `str == str` → `p_str_equal`
- Match pattern: `Pattern::StrLit(s)` (2 sites)

### 1.15 C stdlib `remove` name collision (session 4)

**Symptom:** `tcc: error: too many arguments to function` for `remove`

**Root cause:** The Fides compiler source has a function named `remove` (for
removing elements from a collection). This collided with C's `remove()`
from `<stdio.h>` (which deletes a file). The generated C called the Fides
`remove` with 3 args, but tcc saw the C `remove` prototype (1 arg).

**Fix (session 4):** Added `remove` to the `sanitize_c_name` function in both
`compiler/codegen_c.fd` and `src/codegen_c.rs`. Fides `remove` is now emitted
as `p_remove_user` in the generated C.

### 1.16 Vec method mangling fails for I64-typed receivers (session 4)

**Symptom:** `tcc: error: undefined symbol 'len'` / `undefined symbol 'get'`

**Root cause:** Vec values are sometimes typed as `IrType::I64` at the IR
level (e.g. when loaded from a field access like `self.errors.len()`).
The method mangling code only handled `Struct`, `Opaque`, and `Generic`
types — `I64` fell through to the default case, leaving the method name
unmangled (`len` instead of `Vec_len`).

**Fix (session 4):** Added two fallbacks:
1. AST type lookup — if the receiver is `Expr::Ident(var_name)`, look up
   the variable's AST type to determine the collection type name.
2. Common-prefix function existence check — if mangling still fails, try
   `Vec_<method>`, `HashMap_<method>`, `String_<method>` and use the first
   one that exists as a user function.

### 1.1 `STATUS_ACCESS_VIOLATION` in `Lexer_tokenize`

**Symptom:** `release/fides-c.exe` crashed with 0xC0000005 when compiling any `.fd` file.

**Causes (plural):**
- `p_file_read("stdlib/core.fd")` could return `NULL` without a check.
- Deep recursion in the C codegen had no limit.
- `p_malloc` failures were not detected / did not abort.
- The Windows stack was too small.

**Fixes applied:**
- NULL check after `p_file_read` in `compiler.fd`.
- Recursion limit (`p_max_recursion = 10000`) + abort in `compiler/codegen_c.fd`.
- OOM abort on all `p_malloc` failures in `compiler/codegen_c.fd`.
- Vectored exception handler (Windows) / signal handler (Linux) in both `compiler/codegen_c.fd` and `src/codegen_c.rs`.
- Windows 16 MB stack thread wrapper in `src/codegen_c.rs`.
- PE stack patching to 256 MB after tcc links the bootstrap binary.

### 1.10 Enum variant extraction truncates struct payloads

**Symptom:** `Parse error: assignment target must be an identifier or array index at line 298 col 15` for `*p0 = 0;` in `stdlib/core.fd`.

**Root cause:** When matching `ParseResultExpr::Ok(e) => { ... }`, the lowerer used `Inst::Load` for `e` (which is `Expr`). Because the C backend treats `IrType::Struct(_)` as a pointer, `Load` only read 8 bytes (the `Expr` tag), not the full 80-byte `Expr`.

**Fix applied (Sep 3, 2026):**
- `compiler/lower.fd`: Added `Lowerer_load_enum_field()` helper — allocates a buffer and uses `Inst::Store` with `store_ty: Some(field_ty)` to copy the full inline struct data via `memcpy`.
- `compiler/lower.fd`: Fixed `Lowerer_pattern_binding_ir_types()` to return `IrType::Struct(_)` as-is (not `word_repr`) for struct-typed fields, so `load_enum_field` knows to copy the full data.
- `compiler/lower.fd`: Fixed `Lowerer_emit_struct_copy()` to use `load_enum_field` for struct-typed fields (was using `Load` which only reads 8 bytes).
- `compiler/codegen_c.fd`: Fixed `Inst::Store` handler to honor the explicit `store_ty` parameter and emit `memcpy` for `IrType::Struct(_)`. Also checks the pointer's pointee type (matching the Rust codegen behavior).

**Result:** The self-hosted compiler now parses `stdlib/core.fd` successfully. The `*p0 = 0;` deref assignment parse error is resolved.

### 1.3 Undersized enum layouts

**Symptom:** Struct-typed enum variants were being truncated during SRET copies.

**Root cause:** `Lowerer_make_enum_layout` computed `max_fields` (count of fields) and allocated 8 bytes per field. It did not account for the actual size of struct/enum-typed fields.

**Fix:** Compute `max_payload_size` from actual field sizes, looking up nested struct layouts. Use iterative fixpoint computation for nested enum/struct types.

### 1.4 Missing SRET for caller-side function calls

**Symptom:** Functions returning structs were called without passing the hidden SRET pointer, so the result was discarded / corrupted.

**Root cause:** `Expr::Call` and `Expr::MethodCall` lowering emitted `Inst::Call(callee, args, ret_ty, eff)` with `ret_ty = Struct` directly, instead of allocating a buffer, prepending it, and using `IrType::Void`.

**Fix:** Added SRET handling in three call lowering paths:
- `Expr::Call` in `compiler/lower.fd`
- `Expr::MethodCall` receiver path in `compiler/lower.fd`
- Associated function calls like `Vec::new()` in `compiler/lower.fd`

### 1.5 Pre-registration of impl method signatures

**Symptom:** Calls to `self.parse_unary()` returned the wrong / unknown type, so SRET handling was not triggered.

**Root cause:** `Lowerer_lookup_func_ret_ty_ir` only found functions pre-registered in the IR module. Free functions were pre-registered, but impl and trait impl methods were not.

**Fix:** Pre-register mangled impl and trait impl method signatures (`StructName_method` / `TypeName_method`) in the IR module before lowering function bodies.

### 1.6 `PIR L2 GetFieldPtr` offset bug

**Symptom:** Native codegen produced wrong struct field offsets.

**Root cause:** `pir.rs` previously used `field_index * 8` for all offsets, which was wrong once struct fields could be larger than 8 bytes (e.g. enum-typed fields).

**Fix:** `pir.rs` now looks up real offsets from `struct_layouts`.

### 1.7 Native codegen REX-prefix bug

**Symptom:** `p_json_at` produced a wrong instruction encoding (`0x49` instead of `0x4C`).

**Fix:** Corrected the REX prefix in the hand-coded x86-64 backend.

### 1.8 `opaque_values` stale string bug in interpreter

**Symptom:** Strings were lost during SRET operations.

**Root cause:** `Value::Str` entries in `opaque_values` were not cleared when a non-opaque value was stored to the same address.

**Fix:** Clear `self.opaque_values.remove(&addr)` before storing a non-opaque value.

### 1.9 Register allocator constant clobbering

**Symptom:** Constants were clobbered by intervening instructions.

**Fix:** Constant rematerialization (`Location::Remat`) instead of spilling/reloading.

---

## 2. Current Open Blockers

### 2.1 ~~Enum variant extraction truncates struct payloads~~ RESOLVED (Sep 3, 2026)

**Status:** Fixed. See section 1.10 above for details.

### 2.2 ~~Crash in `check_cfg` / `apply_cfg_filter`~~ RESOLVED (Sep 3, 2026)

### 2.3 ~~Crash in `box_expr_unbox` during `compute_func_effects`~~ RESOLVED (Sep 3, 2026)

### 2.4 ~~Crash in `map_str_str_get` → `p_str_equal`~~ RESOLVED (Sep 3, 2026)

### 2.5 ~~Fides lowerer gaps 1-5~~ RESOLVED (session 3)

All 5 gaps ported to the Fides lowerer. See `docs/roadmap/selfhost-roadmap.md`
for details.

### 2.6 ~~Dangling SRET alloca for enum returns~~ RESOLVED (session 3)

Ported to Fides lowerer as Gap 5.

### 2.7 ~~Windows `\r\n` doubling in C output~~ RESOLVED (session 2)

### 2.8 fides-c2.exe crashes on stdlib (PARTIALLY RESOLVED — session 13)

**Symptom:** `fides-c2_new.exe` (the second-generation compiler, built by
`fides-c.exe`) crashes with `STATUS_ACCESS_VIOLATION` (exit code -1073741819)
during type checking of `test_simple.fd`.

**Root cause:** Multiple behavioral differences between the self-hosted
compiler (`compiler/*.fd`) and the Rust compiler (`src/*.rs`) cause the
self-hosted compiler to generate slightly different IR/C, which leads to
null pointer dereferences in the second-generation binary.

**Session 12 progress:**
1. Split `keyword_or_ident_core` (51 if-statements, 933 C vars) into 5
   smaller functions (`_a` through `_e`, ~195 vars each) — fixed lexer crash.
2. Added scientific notation parsing to Rust lexer (`1.7976931348623157e308`
   in `codegen_c.fd`).
3. Made private lexer helper functions `pub` — fixed visibility errors.
4. Increased TCC stack reserve to 128MB (`-Wl,-stack,134217728`).
5. Added `-lws2_32` for socket function linking.

**Session 13 progress (4 parity fixes):**
1. **Removed DCE** from `ccompile_source_with_path` — self-hosted compiler
   was calling `IrModule_optimize` (dead function elimination) which Rust
   doesn't do. Removed ~303 missing functions. (1514 → 1814 functions)
2. **Fixed `TypeChecker_compute_func_effects`** — rewrote to avoid temporary
   `names`/`bodies` Vecs that caused extra boxing. Added `OptStmtList` type
   and `TypeChecker_find_body` helper.
3. **Fixed C codegen return type casts** — added `(int64_t)`/`(void*)`/
   `(double)`/`(float)`/`(bool)` casts on call results, matching Rust's
   `src/codegen_c.rs`.
4. **Fixed type alias resolution in `Lowerer_mangle_method`** — type aliases
   like `StrStack = Vec<str>` now resolve to `Vec` before mangling, fixing
   `StrStack_push` → `Vec_push` (was causing undefined symbols).

**Result:** fides-c2_new.exe now parses stdlib + user code successfully,
but crashes during type checking in `TypeChecker_lookup` (null pointer
from `Vec_get` on empty scopes).

**Current crash stack:**
```
[0] main
[1] cmd_dump_c
[2] ccompile_source_with_path
[3] build_program_with_path
[4] TypeChecker_check_bodies
[5] TypeChecker_check_func
[6] TypeChecker_check_stmt
[7] TypeChecker_check_stmt_inner
[8] TypeChecker_check_expr
[9] TypeChecker_check_expr_with_hint
[10] TypeChecker_check_expr_inner
[11] TypeChecker_check_expr
[12] TypeChecker_check_expr_with_hint
[13] TypeChecker_check_expr_inner
[14] TypeChecker_lookup
```

**Next steps:** See `docs/roadmap/selfhost-roadmap.md` § "Comprehensive Gap
Analysis" for the full list of remaining gaps. The immediate next fix is
adding null safety to `TypeChecker_lookup` (Category A1) and auditing
`TypeChecker_register_program` parity (Category A2).

### 2.9 ~~`.exe` extension missing in self-hosted compiler output~~ RESOLVED (session 8)

**Symptom:** `fides-c.exe ccompile file.fd -o output` produced `output`
without `.exe` extension on Windows. Manual `Copy-Item output output.exe`
was needed to run it.

**Root cause:** `cmd_ccompile` and `cmd_build` in `compiler/main.fd` used
the user-supplied output name as-is. The default for `ccompile` was `a.out`
(POSIX convention), and neither command appended `.exe` on Windows.

**Fix (session 8):** Added `ensure_exe_extension(s)` helper in
`compiler/main.fd` — returns `s` unchanged if it already ends with `.exe`,
otherwise appends `.exe`. Applied in both `cmd_ccompile` (default changed
from `a.out` → `a.exe`) and `cmd_build`. The C source path in `ccompile`
is now derived by replacing `.exe` with `.c` (matching `src/main.rs`
behavior). The `run`/`test`/`bench` commands already used hardcoded
`.exe` temp names, so no change was needed there.

**Lesson:** The self-hosted compiler currently targets Windows only (PE
emitter + tcc with `-lws2_32`). If Linux support is added later, this
should be gated on a platform check rather than always appending `.exe`.

### 2.10 Debug prints in compiler source (TODO)

38 `p_eprint_str` calls in `compiler.fd` (31) and `main.fd` (7) produce
noisy stderr output. These should be gated behind a debug flag before
fixpoint verification (they won't break C output comparison since they
go to stderr, but they make output noisy).

### 2.11 Pre-existing parse error at codegen_c.fd:973 (NEW — session 9)

**Symptom:** `fidesc ccompile compiler/main.fd -o release/fides-c_test.exe`
fails with:
```
error: Parse error at 973:34: expected identifier but got 'var'
Parse error at 974:9: expected fn but got 'Pattern'
```

**Root cause:** Unknown. The error occurs during import resolution when
building `compiler/main.fd` (which imports `compiler/codegen_c.fd` among
others). The line/column 973:34 points into the merged source (stdlib +
imports + user code), not the raw file. The error reproduces with the
prior commit's `codegen_c.fd` and `main.fd` (verified by checking out
commit `53df63b`), so it is **NOT caused by session 9 LSP work**.

**Impact:** Blocks rebuilding `fides-c.exe` with current code. The existing
`release/fides-c.exe` (built before this bug appeared) still works. This
must be fixed before the fixpoint check can be attempted with current code.

**Investigation needed:**
- Determine which file/module in the merge produces line 973
- Check if a recent stdlib change introduced a construct the parser can't handle
- Try building individual compiler modules to isolate which import triggers it

### 2.12 `--borrowck` flag parsed but not wired (CURRENT — session 11)

**Symptom:** `--borrowck=on` has no effect. Borrow checking is always `Off`.

**Root cause:** `compiler/main.fd` parses the `--borrowck` flag into a global
variable, but `compiler/compiler.fd` hardcodes `BorrowckMode::Off` when
constructing the `BorrowChecker`. The flag value is not passed through the
compilation pipeline.

**Why it wasn't fixed:** Threading the flag through `build_program_with_path`
and `ccompile_source_with_path` requires either (a) adding parameters to many
call sites, or (b) using module-level mutable globals — which Fides didn't
support until this session. The `var` feature was implemented specifically to
solve this, but the wiring has not yet been completed.

**Fix path:** Now that `var`/`pub var` is implemented in the Rust compiler,
mirror it in the Fides compiler (`compiler/*.fd`), then use a module-level
`pub var g_borrowck_mode: BorrowckMode = BorrowckMode::Off;` in `compiler.fd`
that `main.fd` sets before calling `build_program_with_path`.

**Impact:** Borrow checker is disabled in self-hosted compiler. No correctness
impact (borrow checking is opt-in), but `--borrowck=on` silently does nothing.

---

## 2b. Session 11 — Module-Level `var` Feature Bugs & Risks

> The `var`/`pub var` feature was implemented in the Rust compiler this session.
> It is NOT yet mirrored in the Fides self-hosted compiler (`compiler/*.fd`).
> Below are all bugs encountered during implementation, plus potential bugs
> that agents should watch for when debugging crashes or wrong output.

### 2b.1 Typechecker rejected assignments to globals (FIXED)

**Symptom:** `Type error: cannot assign to undefined variable 'g_counter'`

**Root cause:** `src/typechecker.rs` line ~3758 checked
`self.lookup(target).is_none() && !self.consts.contains_key(target)`
but did not check `self.globals.contains_key(target)`. Globals were registered
in `self.globals` but the assignment validator didn't know about them.

**Fix:** Added `&& !self.globals.contains_key(target)` to the condition.
Also added globals lookup in `check_expr` for `Expr::Ident(name)` (line ~4330).

**Lesson:** When adding a new category of named items (globals, types, etc.),
update ALL places that check whether a name exists: expression lookup,
assignment validation, and visibility checks.

### 2b.2 PE emitter: globals stored in .rdata section (WORKING but fragile)

**Symptom:** PE executables with globals crashed or produced wrong values.

**Root cause:** The PE emitter only has `.text` and `.rdata` sections. There is
no `.data` section. Globals need writable storage.

**Current approach:** Globals are appended to the `.rdata` section, which is
already marked `READ|WRITE` (0xC0000040) because the IAT needs write access.
`GlobalAddr` emits `lea_rip` to the global's offset within `.rdata`.

**Risk:** If the `.rdata` section characteristics are ever changed to read-only
(0x40000040), globals will silently become read-only and writes will crash with
`STATUS_ACCESS_VIOLATION`. The section flags are set in `build_pe_file()` in
`src/pe.rs` line ~1705.

**Better fix (future):** Add a proper `.data` section with its own section
header. This requires changing `NumberOfSections` from 2 to 3, adjusting
`SizeOfImage`, `SizeOfInitializedData`, and adding a third section header +
raw data block.

### 2b.3 ARM64/RISCV/WASM backends: GlobalAddr is a stub (KNOWN)

**Symptom:** Globals return 0 on ARM64, RISC-V, and WASM backends.

**Root cause:** These backends emit `mov #0` / `li 0` / `i64.const 0` as
placeholder stubs for `L3Inst::GlobalAddr` / `Inst::GlobalAddr`. The real
implementation would need platform-specific RIP-relative addressing:
- ARM64: `adrp x0, _g_name@PAGE` + `add x0, x0, _g_name@PAGEOFF`
- RISC-V: `lui x0, %hi(_g_name)` + `addi x0, x0, %lo(_g_name)`
- WASM: WASM globals or a linear memory offset table

**Impact:** Globals only work on x86-64 (PE, ELF, PIR codegen) and the
interpreter. If Fides targets ARM64/RISCV/WASM, globals must be properly
implemented.

### 2b.4 No initialization order guarantee for globals (POTENTIAL)

**Symptom:** A global whose initializer references another global declared
later in the file gets a zero/garbage value.

**Example:**
```fides
pub var g_a: i64 = g_b + 1;  // g_b not initialized yet
pub var g_b: i64 = 10;
```

**Root cause:** `init_globals()` in `src/interp.rs` iterates
`self.module.globals` (a HashMap, unordered) and initializes each global
independently. The PE emitter appends globals in HashMap iteration order too.
Neither checks whether an initializer references another global.

**Current behavior:** Non-literal initializer expressions (anything other than
`IntLit`, `FloatLit`, `BoolLit`, `StringLit`) are silently zero-initialized
in both the interpreter and PE emitter. So `g_a = g_b + 1` becomes `g_a = 0`.

**Fix needed:** The typechecker should reject forward references to other
globals in initializer expressions. Alternatively, topologically sort globals
by their dependencies and initialize in order.

### 2b.5 Optimizer purity analysis broken by globals (POTENTIAL)

**Symptom:** A function that reads a global is treated as `pure` by the
optimizer. If the global changes between two calls, the optimizer incorrectly
reuses the first result (CSE/GVN) or deletes the second call (DCE).

**Root cause:** The optimizer's purity analysis is based on function effects
(`EffectSet`). A function that only reads a global (no IO, no Alloc, no Write
to memory through a pointer) may be classified as pure. But global reads are
implicit memory reads not tracked by the current effect system.

**Current status:** Not yet observed in tests because the test suite doesn't
have functions that read globals and are called multiple times with global
mutation in between. But this is a latent bug.

**Fix needed:** Either (a) mark any function containing `GlobalAddr` + `Load`
as having a `Read` effect, or (b) teach the optimizer that `GlobalAddr` is not
pure (it depends on mutable global state).

### 2b.6 Complex global initializers silently zero-initialized (KNOWN)

**Symptom:** `var g_map: HashMap<str, i64> = HashMap::new();` produces an
empty/null map, not a working empty map.

**Root cause:** `eval_global_init()` in `src/interp.rs` and the PE emitter's
initializer code only handle literal expressions (`IntLit`, `FloatLit`,
`BoolLit`, `StringLit`). All other expressions (function calls, struct
constructors, etc.) fall through to `_ => Value::Int(0)` or
`_ => 0i64.to_le_bytes()`.

**Impact:** Globals with non-literal initializers (Vec::new(), HashMap::new(),
struct constructors) don't work. Only primitive-typed globals with literal
initializers are fully functional.

**Fix needed:** Evaluate complex initializer expressions at program startup
(either in a generated `__init_globals` function or by running them in the
entry point before `main`).

### 2b.7 Interpreter: string globals use opaque_values side-table (FRAGILE)

**Symptom:** String globals may lose their string value if the opaque_values
entry is overwritten.

**Root cause:** `init_globals()` in `src/interp.rs` allocates string globals
in memory and stores the pointer in the global slot. The actual `Value::Str`
is kept in `opaque_values` at the string address. If a subsequent store to
the same address doesn't clear the `opaque_values` entry, or if the address
is reused, the string can be lost.

**Status:** Not yet observed in tests. The 0x00400000 base address is far from
normal allocations, so collisions are unlikely. But this is fragile.

### 2b.8 Self-host regressions after var implementation (CURRENT)

**Symptom:** `verify.sh` shows 16 regressions after implementing `var`:
- `test_codegen_c` — interp + c codegen
- `test_lower`, `test_banking`, `test_deep_nesting`, `test_new_features`,
  `test_parallel`, `test_parallel_concurrent`, `test_phase8b`, `test_rc_arc`,
  `test_regression_collections`, `test_vec_math` — c codegen only
- `test_deep_nesting` — pe emit
- `test_token_full` — all 3 backends

**Root cause (test_token_full):** This is a library module (no `fn main`).
It was likely already failing before this session — the test count grew from
180 to 211 between baselines, suggesting many new tests were added.

**Root cause (others):** Unknown — needs investigation. Likely related to
the `var` changes in `lower.rs` (assignment handling, ident lookup) or
`typechecker.rs` (globals map). The C codegen-only failures suggest the
`emit_globals` function or `GlobalAddr` C codegen may interfere with
existing code paths.

**Investigation steps:**
1. Run `./scripts/bisect.sh test_codegen_c` to find which layer broke
2. Check if `emit_globals` emits anything when `module.globals` is empty
   (it should be a no-op for existing tests)
3. Check if the `lower.rs` assignment change (global check before alloca check)
   affects non-global assignments
4. Run `./target/release/fidesc run tests/test_codegen_c.fd` for the
   interpreter error message

### 2b.9 `git stash` incident (RESOLVED, process bug)

**Symptom:** Running `git stash` to compare baselines stashed all uncommitted
work (both agent's and user's changes), and the command was interrupted before
`git stash pop` could restore.

**Root cause:** Agent ran `git stash` carelessly to check baseline state.
This is a process error, not a code bug.

**Fix:** `git stash pop` restored all work. Added hard rule to `AGENTS.md`:
"NEVER run `git stash`, `git checkout -- .`, `git reset --hard`, or any
command that discards/hides uncommitted work."

**Lesson for all agents:** Use `git diff` or `git worktree add` to compare
baselines. NEVER touch the working tree.

---

## 2c. Session 11 — Static Analysis Findings (All `compiler/*.fd` Files)

> A comprehensive read-only static analysis was performed on all 24 self-hosted
> compiler modules. Below are the bugs and issues discovered, organized by
> priority. Line numbers refer to the `compiler/*.fd` files as they existed
> at the time of analysis.

### P0 — Critical (prevents compilation)

#### 2c.1 `lexer.fd:291` — Duplicate empty `str_filter_underscores` stub

**Symptom:** Numeric literals with underscores (e.g. `1_000_000`) won't have
underscores stripped.

**Root cause:** `str_filter_underscores` is defined twice:
1. Empty stub at line 291: `var result: str = "";` (function body ends immediately)
2. Real implementation at lines 572-583

Either the file fails to compile (duplicate function) or the empty stub
shadows the real one.

**Fix:** Remove the empty stub at line 291.

#### 2c.2 `parser.fd` — `box_expr_deref` references non-existent `Expr` variants

**Symptom:** Parser module won't compile.

**Root cause:** `box_expr_deref` (lines 4941-4971) matches `Expr` variants
that do not exist in `ast.fd`:
- `CharLit`, `Assign`, `Lambda`, `SizeOf`, `AlignOf`, `OffsetOf`, `Path`,
  `UnitLit` — none of these are in the `Expr` enum
- `ArrayLit(args)` — uses 1 field, but `ast.fd` defines `ArrayLit(Box<Type>, Vec<Expr>)` (2 fields)
- `Range(l, r)` — uses 2 fields, but `ast.fd` defines `Range(Box<Expr>, Box<Expr>, bool)` (3 fields)

**Fix:** Rewrite `box_expr_deref` to match the actual `ast.fd` `Expr` enum
variants and arities.

#### 2c.3 `parser.fd` — Missing `return` statements in 4 parse functions

**Symptom:** Functions fall off the end without returning a `ParseResult*`.

**Affected functions:**
- `parse_guard_stmt` (ends at line 2521)
- `parse_use_stmt` (ends at line 3024)
- `parse_match_expr` (ends at line 4002)
- `parse_comptime_expr` (line 3848 — only returns for `comptime { ... }`)

**Fix:** Add appropriate `return` statements at the end of each function.

#### 2c.4 `parser.fd` — Duplicate `var has_forward` in `parse_layer` and `parse_model`

**Symptom:** Duplicate variable declaration error.

**Root cause:** `var has_forward: bool = false;` is declared twice in:
- `parse_layer` (lines 1637-1639)
- `parse_model` (lines 1704-1706)

**Fix:** Remove the duplicate declaration.

#### 2c.5 `parser.fd:4684` — `parse_use` version-pin always returns Err

**Symptom:** `use foo = "1.0.0";` always produces a parse error.

**Root cause:** Lines 4684-4695: even when a valid version string is found
(`TokenKind::StringLit(s)`), the function immediately returns
`ParseResultImportDef::Err(...)`.

**Fix:** Only return `Err` when no valid string token is found.

### P1 — Important (correctness or feature gaps)

#### 2c.6 `compiler.fd:1006` — `--borrowck` hardcoded to Off

Already documented as bug 2.12 above. The `var` feature now makes this fixable.

#### 2c.7 `main.fd:71` — `--borrowck=` off-by-one in substring extraction

**Symptom:** `--borrowck=off` is parsed as `ff` instead of `off`.

**Root cause:** `str_substring_from(garg, 12)` for an 11-character prefix
(`--borrowck=`) drops the first value character if strings are 0-indexed.

**Fix:** Use index 11 instead of 12.

#### 2c.8 `inc.fd:134-145` — `global_cache_*` are no-ops

Already documented in architecture.md. Now fixable with the `var` feature
implemented in the Rust compiler. The Fides `inc.fd` can use `var` to
implement a real global cache once the `var` feature is mirrored to the
Fides self-hosted compiler.

#### 2c.9 `lower.fd:6005` — `f64_to_bits` always returns 0

**Symptom:** Float bitcast operations produce 0.

**Root cause:** `f64_to_bits(f: f64) -> i64` is a stub: `return 0;`

**Fix:** Implement proper IEEE 754 bit conversion.

#### 2c.10 `lower.fd:1767` — `lower_type` narrows I128/U128 to I64/U64

**Symptom:** 128-bit integer types silently become 64-bit in IR.

**Root cause:** `lower_type` maps `I128` → `I64` and `U128` → `U64` at lines
1767, 1772, 1833, 1838.

**Fix:** Add `IrType::I128`/`U128` support or reject 128-bit types with an error.

#### 2c.11 `opt.fd:221` — `is_lto_mode()` hardcoded false

**Symptom:** LTO mode detection always returns false.

**Root cause:** `is_lto_mode() -> bool { return false; }`

**Fix:** Read from a module-level flag (now possible with `var` feature).

#### 2c.12 `opt.fd:1649-1660` — `cse_key` unreachable duplicate arms

**Symptom:** Dead code (not a correctness issue, but confusing).

**Root cause:** Duplicate `ConstInt`/`ConstFloat`/`ConstStr`/`ConstBool` arms
appear after a catch-all `_` arm.

**Fix:** Remove the duplicate arms.

#### 2c.13 `pir.fd:2681` — `str_substring` returns original string

**Symptom:** Any caller of `str_substring` gets the full string instead of a
substring.

**Root cause:** `str_substring(s, start, len) -> str { return s; }` — ignores
`start` and `len`.

**Fix:** Implement actual substring extraction, or remove if unused.

#### 2c.14 `codegen_c.fd:2523,2555` — `DynCall` and `TryUnwrap` not implemented

**Symptom:** C codegen for `DynCall` and `TryUnwrap` emits only a comment,
leaving the destination variable uninitialized.

**Root cause:**
```fides
Inst::DynCall(_, _, _, _, _) => {
    wln(self, "    // DynCall not implemented in C backend");
}
Inst::TryUnwrap(_, _, _, _) => {
    wln(self, "    // TryUnwrap not implemented in C backend");
}
```

**Fix:** Implement proper C codegen for these instructions.

#### 2c.15 `pe.fd` — 20+ runtime stubs return wrong values

**Symptom:** PE executables that use these runtime functions produce wrong
results.

**Affected stubs:**
- `p_vec_insert`, `p_vec_remove`, `p_vec_reverse`, `p_vec_extend`,
  `p_vec_sort` — all return 0 (`xor rax; ret`)
- `p_string_contains`, `p_string_starts_with`, `p_string_ends_with` —
  return `true`
- `p_string_split` — returns empty Vec
- `p_string_find`, `p_string_rfind` — return 0
- `p_string_replace`, `p_string_to_uppercase`, `p_string_to_lowercase`,
  `p_string_trim`, `p_string_repeat` — return input unchanged
- `p_map_keys`, `p_map_values`, `p_map_clone` — return empty

**Fix:** Implement proper runtime functions for each.

#### 2c.16 `mono.fd:649` — `subst_gat_type_args` always uses args.get(0)

**Symptom:** Generics with more than one type parameter are mis-substituted.

**Root cause:** For single-character uppercase generic parameters, the function
always uses `args.get(0)` instead of the correct parameter index.

**Fix:** Use the correct parameter index based on the generic parameter name.

#### 2c.17 `comptime_eval.fd` — Return re-wrap omissions

**Symptom:** Comptime expressions inside `return` statements are not
substituted or evaluated.

**Root cause:** `substitute_expr` and `replace_ident_in_*` skip `Stmt::Return`
and `OptBoxExpr` re-wrapping. Comments say "Can't re-wrap into Box easily;
skip for now."

**Fix:** Implement proper Box re-wrapping for Return statements.

### P2 — Lower priority (not blocking bootstrap)

#### 2c.18 `typechecker.fd:1451-1505` — 13 safety verification stubs

All 13 `verify_*` functions for safety annotations (`@conserve`, `@gas`,
`@constant_time`, `@realtime`, `@differentiable`, `@quantized`, `@checkpoint`,
`@dp`, `@shard`, `@adapter`, `@tainted`, `@contracts`, `@allocator_params`)
are empty no-ops. Safety annotations are accepted but not enforced. Not
needed for bootstrap, but dangerous for production use.

#### 2c.19 `parser.fd:4847` — `parse_fstring_parts` is a stub

F-string interpolation is extracted as raw text, not parsed. The `Lexer::new`
call is commented out with "TODO: Re-enable when cross-module Lexer usage is
fixed."

#### 2c.20 `comptime_eval.fd` — `aggressive_comptime_expr` limited recursion

Only recurses into `Binary`, `Unary`, `Call`, `MethodCall`. Other expressions
(`If`, `Match`, `ArrayLit`, `StructLit`) are not traversed for nested comptime
calls.

#### 2c.21 `lower.fd:6155` — `infer_expr_ast_type` falls back to I64

Simplified inference returns `Type::I64` for most expression forms, making
closure return type inference unreliable for non-trivial closures.

#### 2c.22 `compiler.fd:1295` — `apply_cfg_filter` is a no-op

Explicit no-op: `// No-op — include all items.` CFG filtering is not
implemented.

#### 2c.23 `compiler.fd:1597` — `fill_default_trait_methods` incomplete

Creates default method functions but never pushes them into
`program.trait_impls`.

---

## 3. Gotchas

### 3.1 `Box<T>` in compiler source is not the same as language `Box`

- Language `Box::new(x)` is `p_malloc` + `Store`.
- Compiler-source `Box<T>` is a recursive pointer wrapper for AST/IR fields.
- Match patterns on `Box<T>` auto-unbox, but explicit `box_*_unbox()` helpers are needed for `Box<Expr>`, `Box<Type>`, `Box<IrType>`.

### 3.2 `Expr` variants in `lower.fd` are out of sync with `ast.fd`

`box_expr_unbox` in `compiler/lower.fd` (lines 71-111) lists variants that do not exist in `compiler/ast.fd`: `Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`.

**Action needed:** Reconcile `box_expr_unbox` with `ast.fd:244-283`.

### 3.3 `Str` is a pointer in IR

`Type::Str` lowers to `IrType::Ptr(Box::new(IrType::U8))`. Do not try to pass `str` as a struct.

### 3.4 `Inst::Load` on `IrType::Struct(_)` only reads 8 bytes

This is the source of the current bug. Any code that extracts a struct-typed field from an enum or struct with `GetFieldPtr` + `Load` is almost certainly wrong.

### 3.5 `Inst::Store` with `store_ty: Some(IrType::Struct(_))` must emit `memcpy`

If the C backend does not do this, `load_enum_field()` and `Lowerer_emit_sret_copy` will not actually copy full struct data.

### 3.6 `__env` dummy pointer

User functions receive a hidden `__env: *u8` first argument (after SRET for struct returns). It is always `0` and is used to make `CallPtr` uniform with direct calls.

**Do not pass `__env` to extern or `p_*` functions.**

### 3.7 Effects must be tracked or the optimizer will delete side-effecting calls

`runtime_effects()` in `compiler/lower.fd` sets `Write`, `Alloc`, `IO`, etc. on `p_*` builtins. Without this, the optimizer's DCE pass will remove calls it thinks are pure.

### 3.8 Vec of structs requires heap allocation

When pushing or inserting a struct into a `Vec`, the lowerer heap-allocates the struct before pushing the pointer. Skipping this produces a dangling SRET pointer.

### 3.9 `region` blocks affect allocation region

`Inst::Alloca` uses the current region from `Lowerer_current_region()`. Inside a `region` block, allocations go to the named region; otherwise they go to `Region::Stack`.

### 3.10 Match arm bindings do not leak

`Lowerer_lower_match` saves and truncates `var_ast_types` and other binding lists after each arm to prevent arm-local variables from leaking to later arms.

### 3.11 Progressive stage reporting uses `p_eprint_str`

`compiler/compiler.fd` and `compiler/main.fd` now call `p_eprint_str` to print each stage to stderr. When adding new stages, follow this pattern so we can still tell where a crash occurred.

### 3.12 Do not `git stash`, `git reset --hard`, `git clean`, or force-push

This can wipe user work and generated artifacts. Always commit or ask before destructive git operations.

### 3.13 Fides compiler silently drops statements in long functions

The Fides compiler has a bug where it silently drops statements in long
functions (likely a buffer overflow or index tracking issue). If you see
missing function calls or instructions in generated C code, check if the
source function is too long. **Workaround:** split long functions into
smaller helper functions (each <~30 statements).

**Session 12 update:** The root cause of the bootstrap crash was NOT
statement-dropping but rather **stack overflow**. The self-hosted compiler's
large functions (e.g. `Parser_parse_stmt` with 4672 C local variables =
~37KB per stack frame) overflow the default 1MB Windows stack during
recursive descent parsing. Fix: compile with TCC flag
`-Wl,-stack,134217728` (128MB stack reserve). The `keyword_or_ident_core`
splitting was still necessary to reduce per-function variable count, but
the stack size increase was the primary fix.

### 3.14 `p_file_write` truncates at null bytes

`p_file_write` uses C string functions internally and stops at null bytes.
For binary output (PE files, ELF files), hex-encode the data and decode
outside the Fides runtime (e.g., via PowerShell).

### 3.15 `src/pe.rs` and `compiler/pe.fd` must stay in sync

The PE emitter has been ported from Rust to Fides (session 5). If you change
`src/pe.rs`, the same change must be applied to `compiler/pe.fd`. This
includes instruction encoding, PE header layout, import table format, and
runtime stubs.

---

## 4. Files That Require Extra Care

From `AGENTS.md`, these are high/medium risk:

| File | Risk | Why |
|------|------|-----|
| `compiler/lower.fd` | **CRITICAL** | AST → IR; SRET, enum layout, match, try, defer, effects all live here |
| `compiler/ir.fd` | **HIGH** | Core IR; changes ripple to all backends |
| `compiler/codegen_c.fd` | **HIGH** | IR → C; ABI, casting, SRET, struct copy all here |
| `compiler/pe.fd` | **HIGH** | IR → x86-64 + PE file; ModRM/SIB bytes, REX prefixes, import table, section layout. Off-by-one = corrupt .exe |
| `compiler/parser.fd` | **MEDIUM** | Recursive descent; changes can subtly shift parse behavior |
| `compiler/ast.fd` | **MEDIUM** | Canonical AST; `lower.fd` unbox helper is already out of sync |
| `compiler/compiler.fd` | **MEDIUM** | Pipeline driver; import resolution is fragile |

Also high-risk in Rust reference compiler:
- `src/regalloc.rs`, `src/peephole.rs`, `src/elf64.rs`, `src/pe.rs`, `src/pir.rs`, `src/lower.rs`, `src/codegen_c.rs`, `src/codegen_x64.rs`

---

## 5. Common Failure Modes

| Symptom | Likely cause | Where to look |
|---------|--------------|---------------|
| `STATUS_ACCESS_VIOLATION` / segfault | Stack too small, null pointer, OOM, SRET mismatch | `codegen_c.fd` crash handler, PE stack, `lower.fd` SRET |
| `STATUS_ACCESS_VIOLATION` in PE emitter | Wrong ModRM byte, wrong call target, missing instruction | `pe.fd` — compare with `src/pe.rs`, check ModRM `mod` field |
| Parse errors on valid code | Parser unbox helper out of sync, enum variant truncation | `ast.fd` vs `lower.fd`, `load_enum_field` |
| Wrong output / fixpoint diverges | Enum layout, SRET copy, struct copy, C backend `memcpy` | `lower.fd` layout, `codegen_c.fd` `Store` |
| `p_*` calls deleted by optimizer | Missing `EffectSet` on call | `lower.fd` `runtime_effects()` |
| Vec of structs corrupt | Dangling SRET pointer on `push`/`insert` | `lower.fd` `Vec::push`/`insert` lowering |
| Import not found | Import path resolution order | `compiler.fd` `build_program_with_path()` |
| Missing statements in generated C | Fides compiler statement-dropping bug (long functions) | Split the source function into smaller helpers |
| PE file truncated/corrupt | `p_file_write` truncates at null bytes | Use hex encoding + external decode |
| Global variable always 0 | Non-literal initializer (Vec::new, HashMap::new, struct ctor) | `interp.rs:eval_global_init` / `pe.rs` initializer — only handles literals |
| Global variable crashes on write (PE) | `.rdata` section changed to read-only | `pe.rs:build_pe_file` section characteristics — must be 0xC0000040 (READ\|WRITE) |
| Global variable wrong on ARM64/RISCV/WASM | Backend stubs GlobalAddr as 0 | `codegen_arm64.rs`, `codegen_riscv.rs`, `wasm.rs` — need real RIP-relative addressing |
| `--borrowck=on` has no effect | Flag not wired through pipeline | `compiler.fd` hardcodes `BorrowckMode::Off`; see bug 2.12 |
| Optimizer deletes/reuses global-reading calls | Function reading global treated as pure | Effect system doesn't track global reads; see bug 2b.5 |
| `cannot assign to undefined variable` for global | Typechecker missing globals check | `typechecker.rs` assignment validation — see bug 2b.1 |
| Numeric literal with underscores not stripped | Duplicate `str_filter_underscores` stub | `lexer.fd:291` — see bug 2c.1 |
| Parser fails to compile | `box_expr_deref` references non-existent Expr variants | `parser.fd:4941` — see bug 2c.2 |
| Parse function returns nothing | Missing `return` at end of function | `parser.fd` — see bug 2c.3 |
| Duplicate variable error | `var has_forward` declared twice | `parser.fd:1637,1704` — see bug 2c.4 |
| `use foo = "1.0.0"` always errors | `parse_use` returns Err even on valid version | `parser.fd:4684` — see bug 2c.5 |
| `--borrowck=off` parsed as `ff` | Off-by-one in substring extraction | `main.fd:71` — see bug 2c.7 |
| Float bitcast returns 0 | `f64_to_bits` is a stub | `lower.fd:6005` — see bug 2c.9 |
| 128-bit integers truncated to 64-bit | `lower_type` narrows I128/U128 | `lower.fd:1767` — see bug 2c.10 |
| PE: vec_insert/remove/sort return 0 | Runtime stubs are `xor rax; ret` | `pe.fd` — see bug 2c.15 |
| PE: string_contains returns true | Runtime stub returns true always | `pe.fd` — see bug 2c.15 |
| Generic with >1 type param mis-substituted | `subst_gat_type_args` always uses args.get(0) | `mono.fd:649` — see bug 2c.16 |
| Comptime in `return` not evaluated | Return re-wrap omitted in substitute | `comptime_eval.fd` — see bug 2c.17 |
