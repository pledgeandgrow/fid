# Fides Self-Hosted Compiler — Known Bugs, Gotchas, and History

> Things that have broken before and how to avoid re-introducing them.
>
> Last updated: Sep 4, 2026 (session 9).

---

## 1. Fixed Bugs (Do Not Re-Introduce)

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

### 2.8 fides-c2.exe crashes on stdlib (CURRENT — session 6, root cause confirmed session 8)

**Symptom:** `fides-c2_test.exe` (the second-generation compiler, built by
`fides-c.exe`) crashes with `STATUS_ACCESS_VIOLATION` (exit code -1073741819)
while lexing `stdlib/core.fd`.

**Root cause:** The **statement-dropping bug** (bug 1.19 in known-bugs.md).
The Fides compiler silently drops statements in long functions. When
`fides-c.exe` compiles the compiler source, some statements are dropped
from the generated C code. The resulting `fides-c2_test.exe` has missing
instructions baked into its binary, causing it to crash on complex inputs
like `stdlib/core.fd` (16K LOC).

**Status:** The workaround (splitting long functions into smaller helpers)
was applied to `pe.fd` in session 5, but the bug affects ALL long functions
in the compiler source. The real fix is to find and fix the statement-dropping
bug in the Fides compiler itself (in `lower.rs` or `codegen_c.rs`).

**Impact:** Blocks fixpoint verification. We need fides-c2 to compile the
compiler to produce fides-c3, then compare C output.

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
