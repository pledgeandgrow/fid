# Fides Self-Hosted Compiler — Known Bugs, Gotchas, and History

> Things that have broken before and how to avoid re-introducing them.
>
> Last updated: Oct 25, 2026 (session 30: standalone-binary stdlib crash, `cast_arg` `Ptr→Generic` ABI repair, nondeterministic self-hosted typecheck, `cmd /C` quoting fix — see §2d).

---

## 1. Fixed Bugs (Do Not Re-Introduce)

### 1.37 Accidental `git checkout -- compiler/pe.fd` lost uncommitted work (session 16)

**Symptom:** Running `git checkout -- compiler/pe.fd` to "discard changes"
destroyed uncommitted UEFI/bare-metal runtime stubs (p_hlt, p_cli, p_sti,
p_inb, p_outb, p_serial) that had been added to `compiler/pe.fd` in the
same session. No stash or backup was available.

**Root cause:** Violated AGENTS.md hard rule: "NEVER run `git stash`,
`git checkout -- .`, `git reset --hard`, or any command that discards/hides
uncommitted work." The command was run to "clean up" the file but destroyed
in-progress work.

**Fix (session 16):** Re-added the cpu_local fields, GS helpers, inline asm
backend, and GlobalAddr cpu_local handling to `compiler/pe.fd`. The UEFI
bindings were recreated in a separate `stdlib/uefi.fd` module instead of
in `compiler/pe.fd`. The bare runtime stubs (p_hlt, p_cli, etc.) were NOT
recovered and need to be re-added in a future session.

**Lesson:** NEVER run `git checkout -- <file>` on files with uncommitted
changes. If you need to compare against a baseline, use `git diff` or a
separate worktree. This is documented in AGENTS.md as a hard rule.

### 1.36 Non-ASCII characters in compiler source cause fixpoint divergence (session 15)

**Symptom:** The generated C was behaviorally correct across all 3 generations,
but `fides-c2.c` ≠ `fides-c3.c` — there were 34 diff lines out of 701,100.
All 34 diffs were Unicode corruption in comments and string literals (e.g.,
`// ─── Runtime types ───` appeared as `?????` in gen 2 and as garbled
multi-byte sequences in gen 3).

**Root cause:** The 25 `compiler/*.fd` source files contained non-ASCII
characters (box-drawing `─` U+2500, `═` U+2550, arrows `→` U+2192, em-dashes
`—` U+2014, and their mojibake variants) in comments and string literals.
The Rust compiler and self-hosted compiler handled these multi-byte UTF-8
sequences differently in string literals, producing different output.

**Fix (session 15):** Replaced all non-ASCII characters in all 25
`compiler/*.fd` files with ASCII equivalents:
- `─` (U+2500) → `-`    (15,245 occurrences)
- `═` (U+2550) → `=`    (11,678 occurrences)
- `ΓöÇ` (mojibake for `─`) → `-`  (~2,617 sequences)
- `—` (U+2014) → `--`   (348 occurrences)
- `→` (U+2192) → `->`   (210 occurrences)
- `↔` (U+2194) → `<->`  (2 occurrences)
- `≤` (U+2264) → `<=`   (6 occurrences)
- `×` (U+00D7) → `x`    (5 occurrences)
- Others → appropriate ASCII

**Result:** `fides-c2.c` == `fides-c3.c` (0 diffs out of 701,100 lines).
**Bootstrap fixpoint achieved.**

**Lesson:** Never use non-ASCII characters in compiler source files. The
compiler's string literal handling may not be Unicode-safe across
generations, causing fixpoint divergence. Use ASCII-only comments and
string literals (`---` instead of `───`, `->` instead of `→`, etc.).

### 1.33 TCC 4KB stack overflow (session 15 — A1 root cause)

**Symptom:** The self-hosted compiler (`fides-c.exe`, `fides-c2_new.exe`)
crashed with `0xC00000FD` (stack overflow) or `0xC0000005` (access violation)
during C code generation of large programs (the compiler source itself).

**Root cause:** TCC produces Windows executables with a 4KB default PE stack
reserve (`SizeOfStackReserve = 0x1000`). The Fides compiler's parser, type
checker, and C codegen are deeply recursive and use `alloca` extensively.
4KB is nowhere near enough — the codegen phase alone needs hundreds of MB.

**Previous misdiagnosis:** This was originally diagnosed as a null pointer
dereference in `TypeChecker_lookup` (Category A1). The actual root cause was
stack overflow corrupting the stack frame, which manifested as spurious null
accesses in various functions.

**Fix (session 15):** Instead of patching the PE header (which was fragile
and sometimes corrupted the executable), wrap the compiler's `main` in a
Windows thread with a 256MB stack:
```c
int fides_main(int argc, char** argv) { /* original main */ }
DWORD WINAPI thread_main(LPVOID param) {
    MainArgs* args = (MainArgs*)param;
    args->ret = fides_main(args->argc, args->argv);
    return 0;
}
int main(int argc, char** argv) {
    MainArgs args = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &args, 0, NULL);
    WaitForSingleObject(h, INFINITE);
    return args.ret;
}
```
This is applied as a post-processing step on the generated C file. See
`selfhost.md` § Build Pipeline, "Stack Wrapper" for the full script.

**Lesson:** TCC's default stack reserve is 4KB, which is absurdly small for
any non-trivial program. Always use a stack wrapper or PE header patch when
building Fides compiler output with TCC. The thread-based wrapper is more
reliable than PE header patching.

### 1.34 type_eq uses pointer == for string comparisons (session 15)

**Symptom:** The self-hosted type checker rejected identical generic type
assignments with errors like:
```
Type error: cannot assign Vec<Param> to field 'params' of type Vec<Param>
Type error: cannot assign Vec<Stmt> to field 'body' of type Vec<Stmt>
Type error: cannot assign Vec<StrPair> to field 'const_params' of type Vec<StrPair>
```

**Root cause:** `type_eq` in `compiler/typechecker.fd` compared string fields
(type names) using `==`, which in the generated C code compares pointer
addresses rather than string content. Two `Type::Generic("Vec", ...)` values
created at different source locations have different string allocations, so
`n1 == n2` returns false even when both are `"Vec"`.

**Fix (session 15):** Replaced all string `==` comparisons in `type_eq` with
`str_equal()`:
- `Type::Named`: `n1 == n2` → `str_equal(n1, n2)`
- `Type::Generic`: `n1 != n2` → `!str_equal(n1, n2)`
- `Type::Dyn`: `n1 == n2` → `str_equal(n1, n2)`
- `Type::Assoc`: `t1 != t2` / `n1 != n2` / `f1 != f2` → `!str_equal(...)`

**Lesson:** In Fides, strings are `char*` pointers. The `==` operator on
strings compares addresses, not content. Always use `str_equal()` for string
value comparisons. The Rust reference compiler uses `#[derive(PartialEq)]`
which does content comparison automatically — the Fides port must explicitly
call `str_equal`.

### 1.35 Large match expression codegen issues (session 15)

**Symptom:** Very large `match` expressions in `mono.fd` (`subst_expr`,
`subst_stmt`) and `comptime_eval.fd` (`substitute_expr`) caused codegen
issues or crashes.

**Fix (session 15):** Split the large match expressions into helper functions:
- `subst_expr` → delegates remaining arms to `subst_expr_part2`
- `subst_stmt` → delegates remaining arms to `subst_stmt_part2`
- `substitute_expr` comptime cases → `substitute_expr_comptime`,
  `substitute_expr_comptime_block`, `substitute_expr_comptime_reflect`

**Lesson:** When a match expression has too many arms, the generated C function
becomes very large and can cause stack issues or codegen problems. Splitting
into helper functions keeps each function manageable.

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
  (Removed Nov 2, 2026: compiled code now relies on the OS guard page, with a
  handler on reserved stack that reports the overflow.)
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

All 5 gaps ported to the Fides lowerer. See `docs/roadmap/roadmap.md` § Self-Host Details
for details.

### 2.6 ~~Dangling SRET alloca for enum returns~~ RESOLVED (session 3)

Ported to Fides lowerer as Gap 5.

### 2.7 ~~Windows `\r\n` doubling in C output~~ RESOLVED (session 2)

### 2.8 fides-c2.exe crashes on stdlib (RESOLVED — session 15)

**Symptom:** `fides-c2_new.exe` (the second-generation compiler, built by
`fides-c.exe`) crashes with `STATUS_ACCESS_VIOLATION` (exit code -1073741819)
during type checking of `test_simple.fd`.

**Root cause:** The primary root cause was **TCC's 4KB default PE stack
reserve** (not 1MB as previously believed). The compiler's deeply recursive
parser, type checker, and codegen need far more stack. The stack overflow
corrupted stack frames and manifested as spurious null pointer accesses
in `TypeChecker_lookup`. A secondary cause was `type_eq` using pointer `==`
for string comparisons instead of `str_equal`.

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

**Session 15 progress (FINAL FIX):**
1. **TCC 4KB stack overflow** — replaced PE header patching with a Windows
   thread wrapper providing 256MB stack (`CreateThread(NULL, 256*1024*1024, ...)`).
2. **type_eq string comparison** — replaced `==` with `str_equal` for
   `Type::Named`, `Type::Generic`, `Type::Dyn`, and `Type::Assoc`.
3. **Large match expression codegen** — split `subst_expr`/`subst_stmt` in
   `mono.fd` and comptime cases in `comptime_eval.fd` into helper functions.
4. **Non-ASCII characters** — replaced all Unicode chars in compiler source
   with ASCII equivalents.

**Result:** **BOOTSTRAP FIXPOINT ACHIEVED.** fides-c2.c == fides-c3.c
(SHA256-identical, 701,100 lines, 19.8MB). Three generations verified.

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
(Session 34: it now has a test driver `main` and passes on all backends.)

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
4. Run `./target/release/fidesc run tests/bootstrap/test_codegen_c.fd` for the
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

## 2d. Session 30 — Standalone-binary crash + ABI/tooling bugs (Oct 25, 2026)

| # | Bug | Status |
|---|-----|--------|
| 2d.1 | `release/fid.exe` crashes on any program that includes stdlib (`run`, `dump-c`, `ccompile` → access violation at `0x1` in `HashMap_insert`/`Parser_new`/`p_str_equal`). `version` works. `verify-selfhost.sh` already documented standalone-binary instability; in-process `fidesc run`/`fidesc ccompile` are reliable. | **FIXED** (no longer reproduces, Oct 25): the self-hosted binary compiles the stdlib and itself; see §2e |
| 2d.2 | `Vec` param promotion: a `Vec<T>` parameter that is mutated (or passed `&mut`) may be promoted to `Ptr(Vec)` while its callers were lowered against the `Generic(Vec)` signature — so `find(ids)` forwarded a slot pointer where a header was expected → `Vec_len` garbage → `regalloc` crash. | **PARTIAL** — `cast_arg` now emits `*(void**)` when `Ptr(inner)` → `Generic`/`Opaque` with `inner == dest` (both `src/codegen_c.rs` and `compiler/codegen_c.fd`); heals the observed case but promotion itself is still inconsistent. |
| 2d.3 | Self-hosted typecheck/borrowck is **nondeterministic**: identical `compiler/*.fd` input fails with different errors per run (e.g. `Vec<i64>` vs `Vec<Vec<EscVar>>` assignment at `main.fd:500`, or a reference-escape error at `interp.fd:3061`) or succeeds. Suspected: HashMap iteration order in inference. | **OPEN** — retry builds before assuming a regression |
| 2d.4 | `cmd /C` quote stripping: `cl.exe` link path failed silently on Windows (empty stderr) because the `vcvarsall.bat && cl.exe` string lost its first quote when it began with `"` and contained `&&`/`>`. | **FIXED** — `src/main.rs` wraps `cmd_str` in an outer quote pair |
| 2d.5 | `interp_reinterpret_word` (session-29 `Vec<f64>` call-result punning) appears **removed** in the current working tree (`Inst::Call` ignores `call_ret_ty` again) — verify this is intentional before relying on `Vec<f64>` reads under the self-hosted interp. | **VERIFY** |

## 2e. Sessions 33–34 — Self-hosted parity with the Rust compiler (Oct 25–27, 2026)

The self-hosted `run` now matches `fidesc run` on all 302 test programs, and
`ccompile` matches on 242 of 243 (all 59 expected-fail tests rejected).
Bugs fixed on the way, so they are not re-introduced:

| # | Bug | Status |
|---|-----|--------|
| 2e.1 | Interpreter (`compiler/interp.fd`): a jump between basic blocks ended the function (every `if`/loop/`match` stopped it). | **FIXED** |
| 2e.2 | Interpreter: a byte load at a side-table string's address returned the whole string, so string loops never ended. | **FIXED** |
| 2e.3 | Interpreter: a runtime builtin of the same name replaced core's own `p_str_equal`, `p_str_length`, … (map lookups failed). | **FIXED** |
| 2e.4 | Comptime loop unroller put the first iteration's value in every iteration. | **FIXED** |
| 2e.5 | `merge_programs` (`compiler/compiler.fd`): `merged` shares its `impls` Vec with `user`, so a second `impl` block of a struct from an imported module was dropped (it looked like a user impl). The user's impl names are now taken before pushing. | **FIXED** |
| 2e.6 | Typechecker: a second `impl` block's methods were not added to the struct's method map. | **FIXED** |
| 2e.7 | `@conserve` only checked effects; calls to non-@conserve functions, money-creating calls and floats were not checked. | **FIXED** (ported from `src/typechecker.rs`) |
| 2e.8 | Self-hosted `ccompile` did not link `extern "C" link "lib"` libraries (`// PLINK:` lines). | **FIXED** |
| 2e.9 | `p_gt_is_done` typed i64 by the typechecker (lowering says bool). | **FIXED** |
| 2e.10 | Native backend (`build --native`: PIR → x86-64 → `compiler/pe.fd`) crashes on every program: `pass_dse` calls the wrong equality function, then register allocation reads null. `build`/`ccompile` go through C instead. | **OPEN** |

Not bugs: `test_sqlite` needs libsqlite3 and `examples/minimal_game` needs
SDL2 (both compilers fail the same way without them).

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

### 3.16 Global `Vec::new()` in struct literals crashes at runtime (session 17) — FIXED (session 22)

**Status:** FIXED. The C backend now emits an `__init_globals()` function
that is called from the generated `main()` entry point. Complex global
initializers (struct literals containing `Vec::new()` and other method calls)
are generated as runtime initialization code rather than being reduced to `0`.

**Symptom (historical):** Native binaries crashed with SIGSEGV during
initialization of any module that declares a global variable containing
`Vec::new()` inside a struct literal:

```fides
var g_state: MyState = MyState {
    items: Vec::new(),   // <-- crashed here
    ...
};
```

**Root cause:** The C backend's global variable initialization only handled
literal expressions (integers, floats, booleans, strings). Complex expressions
such as struct literals and `Vec::new()` calls were reduced to `0`, producing
a null pointer that was dereferenced on first use.

**Fix:** `compiler/codegen_c.fd` now generates an `__init_globals()` function
containing runtime initialization code for complex global initializers. This
function is called at the start of the generated C `main()` wrapper. The
`gen_global_runtime_expr` helper handles struct literals and `Vec::new()`
method calls.

**Affected modules (workarounds no longer needed):** `profiler.fd`,
`replay.fd`, `touch.fd`, `editor.fd`, `shader.fd` (MaterialCache).

### 3.17 `Vec::set()` on same index twice with struct containing `str` causes buffer overflow (session 17) — FIXED (session 22)

**Status:** FIXED. The C backend now allocates the full struct size for
`alloca` slots used in struct vector operations, and `memcpy` uses the
correct struct size. Repeated `Vec::set()` on the same index with struct
values containing `str` fields no longer overflows.

**Symptom (historical):** Native binary aborted with
`*** buffer overflow detected ***` (SIGABRT) when calling `Vec::set()`
twice on the same index with a struct value that contains a `str`
(heap-allocated) field.

**Root cause:** The generated C code allocated an 8-byte `alloca` slot for
a struct pointer, but then `memcpy`'d the full struct size into it on
reassignment, overflowing the buffer.

**Fix:** `compiler/codegen_c.fd` now uses correct struct allocation sizes
for all `alloca` slots involved in struct vector operations, and `memcpy`
uses the actual struct layout size.

**Affected modules (workarounds no longer needed):** `editor.fd`.

### 3.18 Struct return value reassignment causes `memcpy` overflow (session 17) — FIXED (session 22)

**Status:** FIXED. The lowerer now uses the inner struct type (not
`Ptr(Struct)`) for `alloca` slots when an inferred `let` variable receives
a struct-returning function call via SRET. This ensures the destination
has the full struct layout size.

**Symptom (historical):** Native binary aborted with
`*** buffer overflow detected ***` when reassigning a variable with a
struct return value from a function, especially on the second assignment:

```fides
var frame = replay_get_frame(&rp);   // OK
frame = replay_get_frame(&rp);       // OVERFLOW on second call
```

**Root cause:** The lowerer allocated an 8-byte `alloca` for the variable
because the SRET convention returns a `Ptr(Struct)`, but the codegen then
`memcpy`'d the full struct size (e.g., 88 bytes) into it, overflowing.

**Fix:** `compiler/lower.fd` `Stmt::Let` lowering now checks if the
inferred value is a pointer to a struct returned through SRET. If so, the
`alloca` uses the inner struct type rather than `Ptr(Struct)`, ensuring
the destination has the complete layout size. Existing struct store logic
then performs a correctly-sized copy.

**Affected modules (workarounds no longer needed):** `replay.fd`.

### 3.19 `Vec<f64>` stores integer bit patterns instead of floats (session 17) — FIXED (session 22)

**Status:** FIXED. The C backend now uses a type-punning union
(`{ double d; int64_t i; }`) for `Vec_push` and `Vec_get` when the
element type is `f64`, preserving the float bit pattern through the
`int64_t` word representation used by the runtime vector.

**Symptom (historical):** A `Vec<f64>` returned integer bit patterns
(e.g., `4621819117588971520`) instead of the stored float values
(e.g., `10.0`).

**Root cause:** The runtime's generic pointer-vector stores all elements
as `void*` pointers. When `f64` values were pushed, they were
bit-cast to `int64_t` and stored as pointers. On retrieval, the
generated code cast the `int64_t` return value directly to `double`,
which did not preserve the float bit pattern correctly.

**Fix:** `compiler/codegen_c.fd` now generates type-punning unions for
`Vec_push` arguments and `Vec_get` return values when the element type is
`f64`, ensuring the float value is preserved through the `int64_t` word
representation used by the runtime vector.

**Affected modules (workarounds no longer needed):** `editor.fd`.

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
| Global Vec crashes on init | `Vec::new()` in global struct literal not initialized | FIXED in session 22 — see 3.16 |
| `Vec::set()` overflow on structs with `str` | 8-byte alloca + full struct memcpy | FIXED in session 22 — see 3.17 |
| Struct return reassignment overflow | 8-byte alloca + full struct memcpy on 2nd call | FIXED in session 22 — see 3.18 |
| `Vec<f64>` returns integer bit patterns | Generic pointer-vector stores f64 as int64 | FIXED in session 22 — see 3.19 |

---

## v1.0.0 Release Notes (Sep 9, 2026)

### Bugs fixed for the release

- **Visibility errors** — `escape_html`, `str_rfind_char` in `compiler/scribe.fd`
  and `char_from_byte` in `compiler/elf64.fd` were private, blocking
  cross-module access. Fixed by adding `pub`.
- **PRNG** — `src/interp.rs` `prng_next_f64` had an unnecessary `mut` on `s0`
  (harmless but cleaned up).

### Known limitations at v1.0.0

- **C codegen segfault on full compiler** — c3 type-checks and borrow-checks
  the full 75K LOC compiler successfully, but segfaults during C codegen of
  the very large output. This is a memory issue, not a correctness issue.
  Normal Fides programs compile and run correctly.
- **Installer ships Rust bootstrap compiler** — The Windows installer ships
  `fidesc.exe` (the Rust bootstrap compiler) renamed as `fides.exe`. This
  is fully functional for compiling Fides programs. The self-hosted compiler
  (`release/fides-c3.exe`) is available from source builds.
- **4 runtime bugs from Session 21** (3.16–3.19) — all FIXED in session 22.
  See bugs 3.16–3.19 above for details.


## Appendix: Port Gap Analysis (Historical)

*From `port-gap-analysis.md` — HISTORICAL document, last updated Session 16 (Sep 6, 2026). Many listed gaps have been resolved in Sessions 17-24; see `AGENTS.md` → "Current Baseline" for the authoritative state.*

> **HISTORICAL DOCUMENT — Last updated Session 16 (Sep 6, 2026).**
> Many gaps listed below have been resolved in Sessions 17-24.
> See `AGENTS.md` → "Current Baseline" for the authoritative current state.
> This document is preserved for reference but should not be used to assess
> current port completeness.
>
> Systematic comparison of every ported module's Rust source vs Fides port.
> Identifies incomplete ports — functionality present in Rust but missing in Fides.
>
> Line counts are current as of session 15. Source line numbers in detailed
> sections below may be from session 11 and are approximate.

---

### Summary Table

| Module | Rust LOC | Fides LOC | LOC Ratio | Completion | Status |
|--------|----------|-----------|-----------|------------|--------|
| main | 2,787 | 1,388 | 0.50 | **60%** | PARTIAL — global flags added, ~10 more commands ported |
| pe | 6,345 | 4,845 | 0.76 | **75%** | PARTIAL — Tier 1 bugs fixed, string runtime + vec/map helpers ported |
| typechecker | 7,163 | 5,521 | 0.77 | **70%** | PARTIAL — resolve_type recursion, generics, compute_func_effects done |
| lower | 6,460 | 6,420 | 0.99 | **95%** | OK — B5 float bits fixed, recursive generic arg resolution (session 14) |
| opt | 7,517 | 7,695 | 1.02 | **95%** | OK — dominator infra, devirt, GVN/alias_analysis upgraded |
| compiler | 2,141 | 1,994 | 0.93 | **90%** | OK — pipeline orchestration complete |
| inc | 146 | 146 | 1.00 | **80%** | PARTIAL — global_cache_* are no-ops (fixable with var feature) |
| codegen_x64 | 1,940 | 1,942 | 1.00 | **90%** | OK — not yet integrated |
| fmt | 1,068 | 1,109 | 1.04 | **95%** | OK |
| regalloc | 1,088 | 1,181 | 1.09 | **98%** | OK |
| peephole | 903 | 1,064 | 1.18 | **98%** | OK |
| lsp | 916 | 1,082 | 1.18 | **95%** | OK |
| codegen_c | 4,632 | 5,204 | 1.12 | **95%** | OK — B5 float bits + union literals (session 14) |
| ast | 904 | 1,144 | 1.27 | **98%** | OK — Box<T> unbox helpers centralized (session 9) |
| pir | 2,049 | 2,704 | 1.32 | **95%** | OK |
| comptime_eval | 1,299 | 1,615 | 1.24 | **90%** | OK |
| lexer | 582 | 832 | 1.43 | **90%** | PARTIAL — duplicate str_filter_underscores stub |
| ir | 1,626 | 2,524 | 1.55 | **98%** | OK |
| diag | 412 | 659 | 1.60 | **98%** | OK |
| parser | 3,275 | 5,145 | 1.57 | **85%** | PARTIAL — box_expr_deref out-of-sync, missing returns, duplicate vars |
| borrowck | 1,158 | 1,830 | 1.58 | **95%** | OK |
| flat_ast | 741 | 1,259 | 1.70 | **95%** | OK |
| mono | 1,422 | 2,569 | 1.81 | **90%** | PARTIAL — subst_gat_type_args bug, crude infer_expr_type |
| token | 250 | 554 | 2.22 | **100%** | OK |
| json | 273 | 666 | 2.44 | **100%** | OK |

**Legend:** OK = no significant gaps. PARTIAL = some features missing but functional.
MAJOR GAPS = significant functionality missing, may affect correctness.

**Overall self-host port completion: ~85%** (weighted by importance; up from
~82% in session 11 due to session 14 Category B codegen parity fixes in
lower.fd and codegen_c.fd).

---

### 1. main.rs → main.fd (completion: ~30%) — MAJOR GAPS

#### Command coverage: 7 complete, 12 partial, 1 stub, 17 missing

| Command | Status | Notes |
|---------|--------|-------|
| `version` | COMPLETE | |
| `ir` / `dump-ir` | COMPLETE | |
| `dump-c` | COMPLETE | |
| `dump-pir` / `pir` | COMPLETE | Levels + `--opt` supported |
| `fmt` | COMPLETE | Supports `--check` |
| `tokens` / `dump-tokens` | PARTIAL | Prints `line:col kind`, no token values |
| `ast` / `dump-ast` | PARTIAL | Prints summary only, not full AST |
| `build` | PARTIAL | PE emitter only; missing `--target`, `--no-opt`, `--lto`, `--caps` |
| `emit` | PARTIAL | Aliased to `build`; missing `--bare`, `--boot` modes |
| `run` | PARTIAL | C/tcc only; missing interpreter, `--release`, `--trace`, `--max-*` limits |
| `ccompile` | PARTIAL | Hardcoded `tcc`; missing `--target`, `--trace`, CC detection |
| `test` | PARTIAL | C/tcc backend only; missing interp/PE backends, `--fail` |
| `opt` | PARTIAL | Ignores `[pass1,pass2,...]` argument — always runs default set |
| `bench` | PARTIAL | Missing `--ccompile` flag and timing statistics |
| `lsp` | PARTIAL | Calls `lsp_run()` — completeness not verified |
| `lint` | STUB | Always prints "No lint warnings." |
| `parse-header` | MISSING | C header → FFI bindings |
| `compile` | MISSING | Alias for ccompile |
| `debug` | MISSING | Interactive debugger |
| `replay` | MISSING | Trace replay |
| `autopsy` | MISSING | Native binary debug (Linux only) |
| `dump-ast-raw` | MISSING | Raw AST dump |
| `dump-ast-flat` | MISSING | Flat AST dump (flat_ast.fd ported but not wired) |
| `diff-test` | MISSING | Differential testing vs golden files |
| `sourcemap` | MISSING | Source map generation |
| `audit` / `verify` | MISSING | Security audit |
| `repl` | MISSING | Interactive REPL |
| `wasm` | MISSING | WASM backend |
| `codegen` | MISSING | Show assembly |
| `gpu` | MISSING | GPU codegen |
| `watch` | MISSING | File watcher + recompile |
| `link` | MISSING | Standalone linker |
| `profile` | MISSING | Per-function profiling |

#### Missing global flags
- `--borrowck=off|warn|on`
- `--timeout` / `--timeout=N`

#### Missing helper functions
- `parse_target_triple`, `find_ml64`, `find_cl`, `find_vcvarsall`, `find_link`, `find_kernel32_lib`

---

### 2. pe.rs → pe.fd (completion: ~50%) — MAJOR GAPS

#### Instruction encoders

| Category | Completion | Details |
|----------|------------|---------|
| Integer/basic | **85%** | mov, add, sub, imul, idiv, div, and, or, xor, not, neg, cmp, test, shl, sar, cqo, setcc, inc, dec, lea, call, jmp, jcc, push, pop, ret, nop, ud2 |
| Sign/zero extension | **90%** | movzx, movsxd, movsx |
| Scalar SSE2 double | **80%** | addsd, subsd, mulsd, divsd, ucomisd, cvtsi2sd, cvttsd2si, movsd (mem), xorpd |
| Scalar SSE single | **30%** | addss, ucomiss present; missing subss, mulss, divss |
| Packed SSE2 double | **0%** | Missing: addpd, subpd, mulpd, divpd, movapd (all forms) |
| AVX | **0%** | Missing: vex2, vaddpd, vmulpd |
| 16-bit forms | **0%** | Missing: mov_reg16_ptr, mov_ptr_reg16 |
| Raw byte emit | **0%** | Missing: bare-mode opcodes (cli/sti/hlt) |

#### Runtime stubs

| Category | Completion | Missing functions |
|----------|------------|-------------------|
| Print (int/str/bool) | **40%** | `p_print_int` broken (negative unreachable, digit loop wrong); `p_print_vec`/`p_print_map` MISSING |
| String conversion | **70%** | `p_to_str_vec` returns `"[]"` always; `p_to_str_float` doesn't zero-pad |
| Vec runtime | **60%** | Missing: contains, truncate, clone, insert, remove, reverse, extend, sort |
| Map runtime | **70%** | Missing: clear, keys, values, get_or, clone; `p_map_remove` is no-op |
| String runtime | **0%** | ALL 21 missing: from, len, is_empty, push_str, slice, contains, starts_with, ends_with, trim, split, replace, char_at, to_uppercase, to_lowercase, parse_int, find, rfind, repeat, bytes, clone |
| Interrupt/MMU | **0%** | ALL missing: cli, sti, hlt, lidt, idt_set_entry, inb, outb, cr3/cr0/cr4 read/write, invlpg, page_map/unmap, phys_read64/write64 |

#### Critical bugs in existing stubs

| Bug | Location | Details |
|-----|----------|---------|
| **`p_malloc` allocates 0 bytes** | pe.fd:2767 | Sets `R8 = 0` before `HeapAlloc` — every allocation returns zero-size buffer |
| **`p_free` is a no-op** | pe.fd:2803 | Plain `ret` — memory leak |
| **`p_print_int` broken** | pe.fd:2447 | Negative branch after `ret` (unreachable); digit loop writes to buffer start, not indexed position |
| **`p_to_str_vec` returns `"[]"`** | pe.fd:3853 | Stub — no element iteration |
| **`p_to_str_float` no zero-pad** | pe.fd:3753 | Multiplies by 1M, loses precision for small fractions |
| **`p_print_vec`/`p_print_map`** | — | Registered as labels but no generator body — calls land on unplaced labels |

#### Missing PE/binary output features
- `generate_flat_binary` — bare-metal flat binary output (0%)
- `build_multiboot2_header` — Multiboot2 boot header (0%)
- `generate_boot_binary` — bootable image generation (0%)
- `gen_boot_entry_point` — bare-mode entry point (0%)
- `bare_mode` field declared but never used

#### PE header / import table: COMPLETE (90%)
- PE32+ header, 2-section layout, import table with 6 kernel32 imports all match Rust

---

### 3. typechecker.rs → typechecker.fd (completion: ~35%) — MAJOR GAPS

#### Effect/annotation verification — ALL STUBBED (0%)

| Function | Status | Impact |
|----------|--------|--------|
| `compute_func_effects` | **NO-OP** | func_effects map never populated — makes verify_declared_effects and verify_deterministic inert |
| `verify_conserve` | Stub | No @conserve enforcement |
| `verify_gas` | Stub | No @gas enforcement |
| `verify_safety_annotations` | Stub | No @safety enforcement |
| `verify_constant_time` | Stub | No @constant_time enforcement |
| `verify_realtime` | Stub | No @realtime enforcement |
| `verify_differentiable` | Stub | No @differentiable enforcement |
| `verify_quantized` | Stub | No @quantized enforcement |
| `verify_checkpoint` | Stub | No @checkpoint enforcement |
| `verify_dp` | Stub | No @dp enforcement |
| `verify_shard` | Stub | No @shard enforcement |
| `verify_adapter` | Stub | No @adapter enforcement |
| `verify_tainted` | Stub | No @tainted enforcement |
| `verify_contracts` | Stub | No given/ensure enforcement |
| `verify_allocator_params` | Stub | No @allocator enforcement |
| `check_visibility` | **DISABLED** | `return;` immediately — no pub/private enforcement |

#### Missing analysis functions (entire suites)

| Function group | Status | Purpose |
|---------------|--------|---------|
| `suggest_closest` + edit_distance | MISSING | Did-you-mean diagnostics |
| `is_vec_type` + swizzle checks | MISSING | SIMD vector type checking |
| `resolve_pattern_enum` | MISSING | Enum shorthand pattern resolution |
| `subst_type_params` | MISSING | Generic type substitution |
| `resolve_assoc_in_type` | MISSING | Associated type resolution |
| `type_from_name` | MISSING | Primitive name → Type mapping |
| `promote_for_arith` | MISSING | Mixed-type arithmetic promotion |
| `get_tensor_shape` + `shapes_compatible` | MISSING | Tensor shape checking |
| `collect_captured_vars` | MISSING | Closure capture analysis |
| `count_gas_cost_*` | MISSING | @gas cost accounting |
| `scan_for_calls` | MISSING | Call graph computation |
| `scan_for_division` | MISSING | Division safety verification |
| `scan_for_while_loops` | MISSING | Loop scanning for @realtime |
| `scan_for_early_returns` | MISSING | Early return detection |
| `scan_for_call_sites` | MISSING | Call-site extraction |
| `eval_contract` | MISSING | given/ensure contract evaluation |

#### Shallow type resolution
- `resolve_type` only handles `Type::Named` — does NOT recurse into `Ref`, `Array`, `Generic`, `Tuple`, `RawPtr`, `Func`, `ShapedTensor`
- `types_compatible` has no rules for `Func`, `ShapedTensor`, `Map`, `Set`, `Channel` (matches Rust gap)
- `type_size` returns 8 for everything except primitives and Named structs

#### Region budget tracking: COMPLETE
- Region stack, budget allocation, usage tracking, and enforcement all implemented

#### Generic type resolution: PARTIAL
- Basic generic parameter tracking works
- Missing `subst_type_params` and `resolve_pattern_enum` for full substitution

---

### 4. opt.rs → opt.fd (completion: ~88%) — PARTIAL

All 23 optimization passes are present. 17 are complete, 4 are partial, 1 is stub.

#### Per-pass status

| Pass | Status | What's missing |
|------|--------|----------------|
| `pass_const_fold` | **COMPLETE** | Float folding added (session 10) |
| `pass_cse` | **COMPLETE** | Intra-block CSE with @deterministic call CSE |
| `pass_gvn` | PARTIAL | Entry-block seeding only; no dominator/RPO-based GVN; no `resolve_copy` |
| `pass_sroa` | **COMPLETE** | alloca escape detection |
| `pass_dse` | **COMPLETE** | Dead store elimination with escape tracking |
| `pass_dce` | **COMPLETE** | `remove_unreachable_blocks` added (session 10) |
| `pass_devirt` | STUB | Single-impl devirt works; sandbox devirt infrastructure only (needs module trait_impls) |
| `pass_jump_thread` | PARTIAL | Simpler form set than Rust |
| `pass_loop_rotate` | **COMPLETE** | |
| `pass_loop_vectorize` | PARTIAL | Only widens induction step 1→4 for constant trip counts; missing `vectorize_loop`, `is_array_load`, `has_loop_carried_dep` |
| `pass_inline` | PARTIAL | Multi-block inlining added (session 10); LTO limits added; still missing sandbox-aware limits |
| `pass_unroll` | **COMPLETE** | remap_inst + ValueId cloning |
| `pass_tco` | **COMPLETE** | Param-to-alloca replacement with loads added (session 10) |
| `pass_alias_analysis` | PARTIAL | Within-block only; no dominator tree, no borrow-based aliasing |
| `pass_effect_reorder` | **COMPLETE** | Memory-dependency safety + terminator ordering fixed (session 10) |
| `pass_partial_eval` | **COMPLETE** | Pure-call / built-in / user-function eval |
| `pass_slp_vectorize` | **COMPLETE** | F64 adjacent load/store/binop vectorization |
| `pass_effect_optimize` | **COMPLETE** | Dead pure-call elimination + @crashfree hoisting |
| `pass_strength_reduce` | **COMPLETE** | x*2^n → x<<n, x*0, etc. |
| `pass_region_pool` | **COMPLETE** | p_alloc replacement + p_region_free insertion |
| `pass_reassociate` | **COMPLETE** | Add/Mul constant folding + canonicalization |
| `pass_memoize` | **COMPLETE** | Per-block @deterministic call memoization |

#### Missing helper functions
`compute_dominators`, `compute_rpo`, `compute_predecessors`, `vectorize_loop`,
`is_array_load`, `has_loop_carried_dep`, `borrow_non_aliasing`, `resolve_copy`,
`new_const_int`

#### Helpers already ported (session 10)
`remap_inst`, `inst_uses_value`, `remove_unreachable_blocks`, `const_float_value`,
`eval_binop_float`, `inline_multi_block`, `max_block_id`, `remap_block_id`,
`clone_inst_remapped_blocks`, `terminator_targets`

---

### 5. lower.rs → lower.fd (completion: ~85%) — PARTIAL

#### Expression lowering: ALL ARMS PRESENT

| Expression | Status | Notes |
|-----------|--------|-------|
| `Expr::Asm` | **COMPLETE** | Added session 10 — Inst::Asm with input/output operands |
| `Expr::Cast` to `Dyn` | **COMPLETE** | Added session 10 — MakeDyn instruction |
| `Expr::QuantCast` | **COMPLETE** | Added session 10 — proper cast lowering |
| `Expr::PipeForward` | PARTIAL | Call/Ident/FieldAccess added (session 10); MethodCall desugaring may be wrong (left becomes receiver, not first arg) |
| `Expr::Spawn` | **COMPLETE** | Added session 10 — p_thread_spawn call |
| `Expr::TryBlock` | **COMPLETE** | Added session 10 — Result alloca/error/continue |
| `Expr::FString` | **COMPLETE** | Vec_to_str formatting added (session 10) |
| `Expr::OptionalFieldAccess` | PARTIAL | Rewritten to FieldAccess, missing None unwrap branch |
| `Expr::ComptimeReflect` | STUB | Returns ConstInt(0) |
| All other Expr arms | **COMPLETE** | IntLit, FloatLit, BoolLit, StringLit, Ident, Binary, Unary, Call, MethodCall, FieldAccess, Index, StructLit, If, Block, TupleLit, Range, Ref, Deref, StrConcat, EnumLit, Match, Try, Coalesce, IsCheck, Closure, ArrayLit, Comptime, ComptimeBlock, Await |

#### Statement lowering: ALL ARMS PRESENT

| Statement | Status | Notes |
|-----------|--------|-------|
| `Stmt::Select` | **COMPLETE** | Added session 10 — p_chan_try_recv polling chain |
| `Stmt::Do`/`Handle` | **COMPLETE** | Added session 10 — effect_handler_stack |
| `for c in str` | **COMPLETE** | Added session 10 — p_str_length + String_byte_at |
| `@persist` saves | **COMPLETE** | Added session 10 — emit_persist_saves before returns |
| All other Stmt arms | **COMPLETE** | Let, LetUninit, LetTuple, Assign, Expr, Return, If, While, For, Loop, Break, Continue, Defer, Match, IndexAssign, DerefAssign, FieldAssign, Unsafe, RegionBlock, Sandbox, Txn, ComptimeGen, GradBlock, Backward, Query, Use |

#### Helper functions

| Function | Status | Notes |
|----------|--------|-------|
| `emit_persist_saves` | **COMPLETE** | Added session 10 |
| `infer_vec_elem_type` | **COMPLETE** | Added session 10 |
| `infer_map_types` | **COMPLETE** | Added session 10 |
| `infer_closure_ret_ty` | **COMPLETE** | Added session 10 (simplified) |
| `ir_type_to_ast_type` | **COMPLETE** | Added session 10 |
| `lower_swizzle` | PARTIAL | Added session 10; only xyzw group, missing rgba/stpq |
| `lower_vec_sret_call` | PARTIAL | Added session 10; passes empty EffectSet instead of looking up function effects |
| `pattern_binding_ast_types` | MISSING | Not yet ported — affects generic Vec/HashMap type recovery in match arms |
| `pattern_binding_ir_types` | **COMPLETE** | IR payload types for match bindings |

#### Remaining gaps

| Gap | Impact |
|-----|--------|
| **Capture types all I64** | `find_captures` returns `Vec<str>` not `Vec<(str, Type)>`; closure environments untyped |
| **Swizzle groups** | Only xyzw; missing rgba/stpq |
| **SRET effects** | `lower_vec_sret_call` passes empty EffectSet |
| **Pattern binding AST types** | `var_ast_types` not updated for match arm bindings |
| **OptionalFieldAccess** | No None unwrap branch |
| **PipeForward MethodCall** | left becomes receiver instead of first arg |
| **infer_expr_ast_type** | Simplified — returns I64 for most expressions |

---

### Priority Ranking — What to Fix First

#### Tier 1: Blocks self-hosting fixpoint (critical bugs)
1. **pe.fd `p_malloc` bug** — passes 0 to HeapAlloc (breaks all PE programs)
2. **pe.fd `p_print_int` broken** — negative numbers unreachable, digit loop wrong
3. **pe.fd ungenerated labels** — `p_print_vec`/`p_print_map` registered but no body
4. **pe.fd `p_free` no-op** — memory leak
5. **lexer.fd duplicate `str_filter_underscores`** — empty stub at line 291 shadows real impl at 572; numeric literals with underscores won't be stripped
6. **parser.fd `box_expr_deref` out-of-sync** — references 8 non-existent Expr variants + wrong arities for ArrayLit/Range; will fail to compile
7. **parser.fd missing returns** — `parse_guard_stmt`, `parse_use_stmt`, `parse_match_expr`, `parse_comptime_expr` fall off end without returning
8. **parser.fd duplicate `var has_forward`** — in `parse_layer` and `parse_model`; duplicate variable declaration
9. **parser.fd `parse_use` version-pin** — always returns Err even when valid version string found

#### Tier 2: Correctness gaps (affects output)
5. **typechecker effect verification** — all 16 verify_* functions stubbed
6. **typechecker `compute_func_effects`** — no-op, makes effect checks inert
7. **typechecker `check_visibility`** — disabled
8. **typechecker `resolve_type`** — doesn't recurse into Ref/Array/Generic/Tuple
9. **lower capture types** — all captures stored as I64
10. **lower `pattern_binding_ast_types`** — missing, affects generic type recovery
11. **lower `f64_to_bits` returns 0** — float bitcast broken (lower.fd:6005)
12. **lower `lower_type` narrows I128/U128** — to I64/U64 (lower.fd:1767)
13. **compiler.fd `--borrowck` not wired** — hardcoded Off (line 1006)
14. **main.fd `--borrowck` off-by-one** — str_substring_from(garg, 12) drops first char
15. **inc.fd global_cache_* no-ops** — fixable now that var feature exists in Rust compiler
16. **opt.fd `is_lto_mode()` hardcoded false** — LTO detection broken
17. **opt.fd `cse_key` duplicate arms** — dead code after catch-all (lines 1649-1660)
18. **pir.fd `str_substring` stub** — returns original string, ignores start/len
19. **codegen_c `DynCall`/`TryUnwrap`** — emit comments only, leave dest uninitialized
20. **pe.fd 20+ runtime stubs wrong** — vec_insert/remove/reverse/sort/extend return 0; string_contains/starts_with/ends_with return true; string_split returns empty; string_find/rfind return 0
21. **mono `subst_gat_type_args`** — always uses args.get(0) for single-char generics
22. **comptime_eval Return re-wrap** — substitute/replace skip Return statements

#### Tier 3: Feature completeness
23. **pe.fd missing string runtime** — 21 string functions entirely missing
24. **pe.fd missing SIMD/AVX encoders** — packed double, AVX, scalar single
25. **pe.fd missing vec/map helpers** — contains, clone, insert, remove, sort, keys, values
26. **main.fd missing commands** — 17 of 37 commands missing
27. **main.fd `lint` stub** — always prints "No lint warnings"
28. **main.fd `test` runner** — only C backend, no interp/PE
29. **opt `pass_loop_vectorize`** — only step widening, no real vectorization
30. **opt `pass_gvn`** — no dominator/RPO-based GVN
31. **opt `pass_alias_analysis`** — only within-block, no dominator tree
32. **opt `pass_devirt`** — sandbox devirt infrastructure only

#### Tier 4: Nice to have
33. **pe.fd flat/boot modes** — bare-metal output
34. **typechecker did-you-mean** — typo suggestions
35. **typechecker gas/contract verification** — @gas, given/ensure
36. **lower swizzle groups** — rgba/stpq support
37. **lower OptionalFieldAccess** — None unwrap branch
38. **opt dominator tree infrastructure** — compute_dominators, compute_rpo, compute_predecessors
39. **main.fd global flags** — --borrowck, --timeout
40. **parser.fd f-string interpolation** — parse_fstring_parts is a stub
41. **comptime_eval aggressive recursion** — only recurses into Binary/Unary/Call/MethodCall

---

### Session 10 Changes (this session)

#### lower.fd improvements (12 items)
- Added Expr::Asm lowering (inline assembly)
- Added Expr::Cast to Dyn (MakeDyn instruction)
- Fixed Expr::QuantCast (proper cast lowering)
- Expanded Expr::PipeForward (MethodCall, Ident, FieldAccess)
- Added Expr::Spawn (p_thread_spawn call)
- Added Expr::TryBlock (Result alloca/error/continue)
- Fixed Expr::FString (Vec_to_str formatting)
- Added Stmt::Select (channel select with p_chan_try_recv)
- Added Stmt::Do/Handle (effect_handler_stack)
- Added for c in str (string iteration)
- Added @persist saves (emit_persist_saves before returns)
- Added helpers: infer_vec_elem_type, infer_map_types, infer_closure_ret_ty, ir_type_to_ast_type, lower_swizzle, lower_vec_sret_call

#### opt.fd improvements (6 items)
- Fixed pass_tco (replace param uses with loads)
- Added pass_dce remove_unreachable_blocks
- Added pass_const_fold float folding
- Fixed pass_inline (multi-block inlining, LTO limits)
- Fixed pass_effect_reorder (memory-dependency safety, terminator ordering)
- Added pass_devirt sandbox infrastructure

#### pir.fd changes
- Added lto_enabled field to L2Module

#### Verification
- All 3 modified files compile successfully
- Self-host tests: 22/23 pass (1 pre-existing failure: test_codegen_c.fd)
- Key tests pass: test_lower.fd, test_opt.fd, test_pir.fd, test_ir.fd, test_compiler.fd

---

### Session 11 Changes (this session)

#### Target: Achieve recommended completion levels across all 5 modules

#### pe.fd improvements (Tier 1 bug fixes + runtime porting)
- Fixed `p_malloc` — was allocating 0 bytes (R8=0 before HeapAlloc); now saves RCX to R12 first
- Fixed `p_free` — was a no-op (just `ret`); now calls HeapFree properly
- Rewrote `p_print_int` — negative branch was unreachable (after ret); digit loop was writing to buffer start instead of indexed position
- Fixed `p_to_str_vec` — was returning "[]" for every vector; now iterates elements
- Fixed `p_to_str_float` — was not zero-padding fractional part
- Fixed `p_map_remove` — was a no-op; now implements linear search + shift removal
- Added `p_print_vec` and `p_print_map` generator functions (labels were registered but had no body)
- Ported 21 string runtime functions: p_str_from, p_str_len, p_str_is_empty, p_str_push_str, p_str_slice, p_str_contains, p_str_starts_with, p_str_ends_with, p_str_trim, p_str_split, p_str_replace, p_str_char_at, p_str_to_uppercase, p_str_to_lowercase, p_str_parse_int, p_str_find, p_str_rfind, p_str_repeat, p_str_bytes, p_str_clone
- Ported 8 vec helper functions: p_vec_contains, p_vec_truncate, p_vec_clone, p_vec_insert, p_vec_remove, p_vec_reverse, p_vec_extend, p_vec_sort
- Ported 5 map helper functions: p_map_clear, p_map_keys, p_map_values, p_map_get_or, p_map_clone
- Completion: 50% → 75%

#### lower.fd improvements (correctness gaps)
- Fixed typed closure captures — `find_captures` now returns `Vec<(str, Type)>` instead of `Vec<str>`; captures use actual types instead of hardcoded `Type::I64`
- Added `pattern_binding_ast_types` — records `var_ast_types` for match-arm bindings (affects generic Vec/HashMap type recovery)
- Fixed SRET effects in `lower_vec_sret_call` — now looks up function effects instead of passing empty EffectSet
- Fixed `OptionalFieldAccess` lowering — now properly checks for None and returns None, otherwise accesses field from Some payload
- Fixed `PipeForward` MethodCall desugaring — `left` is now the first argument, not the receiver
- Fixed swizzle groups — added rgba and stpq support (was only xyzw)
- Completion: 85% → 95%

#### typechecker.fd improvements (core functionality)
- Fixed `resolve_type` recursion — now recurses through Ref, Array, ArrayConst, Generic, Tuple, RawPtr, Func, ShapedTensor (was only handling Named aliases)
- Implemented `subst_type_params` — substitutes type parameters with concrete types in a type tree
- Implemented `resolve_pattern_enum` — resolves enum shorthand patterns to full variant patterns
- Implemented `resolve_assoc_in_type` — resolves associated types within a type
- Implemented `compute_func_effects` — scans function bodies for IO/Spawn/Diverge/memory effects and populates `func_effects` map (was a no-op)
- Re-enabled `check_visibility` — performs real pub/private/field checks (was disabled with `return;`)
- 16 `verify_*` annotation functions intentionally skipped (not needed for bootstrap)
- Completion: 35% → 70%

#### opt.fd improvements (dominator infrastructure + pass upgrades)
- Ported `compute_dominators` — iterative dataflow algorithm for immediate dominators
- Ported `compute_rpo` — reverse post-order traversal of CFG blocks
- Ported `compute_predecessors` — builds predecessor map from terminator analysis
- Completed `pass_devirt` — wired module-level `trait_impls` list for sandbox single-impl devirtualization
- Upgraded `pass_gvn` — uses RPO traversal and dominator-based value availability; added `resolve_copy` for copy-chain resolution
- Upgraded `pass_alias_analysis` — uses dominator tree for cross-block analysis; ported `borrow_non_aliasing`
- Completion: 88% → 95%

#### main.fd improvements (CLI flags + commands)
- Added global flag parsing: `--borrowck=off|warn|on` and `--timeout N` / `--timeout=N`
- Ported `compile` command (alias for ccompile with full flags)
- Improved `build` command: added `--target`, `--no-opt`, `--lto`, `--caps` flags
- Improved `run` command: added `--release`, `--max-recursion`, `--max-steps`, `--max-memory`, `--timeout`, `--trace` flags
- Improved `ccompile` command: added `--target`, `--trace`, better CC detection (gcc, clang, cl.exe)
- Improved `test` command: added multi-backend support (interpreter, PE, C), `--fail` flag
- Improved `opt` command: added optional pass list argument
- Replaced `lint` stub with real analysis (unused variables, unreachable code)
- Ported `dump-ast-raw` and `dump-ast-flat` commands
- Improved `bench` command: added `--ccompile` flag and timing statistics
- Ported `watch` command with `--interval`, `--opt`, `--no-run`, `--diff` flags
- Completion: 30% → 60%

#### pir.fd changes
- Added `trait_impls` field to `L2Module` for devirt support

#### codegen_c.fd changes
- Added float modulo support (fmod for F64 Mod)

#### Regression fixes
- Fixed `StructLayoutEntry.key` → `.name` in lower.fd:4510 (field doesn't exist)
- Fixed duplicate `I64Pair` struct in opt.fd (conflicting field names a/b vs first/second)

#### Verification (session 11)
- Rust compiler builds successfully (cargo build --release)
- Self-host tests: 22/23 pass on all 3 backends (interp, C codegen, PE emitter)
  - 1 pre-existing failure: test_codegen_c.fd (missing p_sb_new runtime function)
  - 3 new tests now pass: test_compiler.fd, test_opt.fd, test_lower.fd (were failing due to regressions, now fixed)
- Baseline saved to tests/selfhost/baseline/test_results.txt

#### Module-level `var` feature (session 11)
- Implemented `var`/`pub var` at module level in the Rust compiler (src/ast.rs,
  ir.rs, parser.rs, typechecker.rs, lower.rs, codegen_c.rs, pe.rs, interp.rs,
  mono.rs, opt.rs, pir.rs, wasm.rs, x86.rs, codegen_arm64.rs, codegen_riscv.rs)
- New `GlobalVar` AST node, `GlobalAddr` IR instruction, `global_vars` field on
  `IrModule` and `Program`
- PE emitter: globals stored in `.rdata` section (writable), accessed via
  `lea_rip` to global offset
- C backend: globals emitted as C file-scope variables
- Interpreter: globals stored in a dedicated global environment
- Dedicated test `test_globals.fd` passes on interp, C, and PE backends
- **16 regressions** in full test suite after var implementation (see known-bugs.md §11)

#### Static analysis of all `compiler/*.fd` files (session 11)
A comprehensive read-only static analysis was performed on all 24 self-hosted
compiler modules. Key findings are recorded below. See `known-bugs.md` §11 for
the full bug list with line numbers.

**Critical (P0) — prevents compilation:**
- `lexer.fd:291` — duplicate empty `str_filter_underscores` stub (real impl at 572)
- `parser.fd` — `box_expr_deref` references non-existent `Expr` variants
  (CharLit, Assign, Lambda, SizeOf, etc.) and wrong arities (ArrayLit, Range)
- `parser.fd` — missing `return` in `parse_guard_stmt`, `parse_use_stmt`,
  `parse_match_expr`, `parse_comptime_expr`
- `parser.fd` — duplicate `var has_forward` in `parse_layer` and `parse_model`
- `parser.fd:4684` — `parse_use` version-pin always returns Err

**Important (P1) — correctness or feature gaps:**
- `compiler.fd:1006` — `--borrowck` hardcoded to Off (TODO at 1003)
- `main.fd:71` — `str_substring_from(garg, 12)` off-by-one for `--borrowck=`
- `inc.fd:134-145` — `global_cache_*` are no-ops (NOW FIXABLE with var feature!)
- `lower.fd:6005` — `f64_to_bits` always returns 0
- `lower.fd:1767` — `lower_type` narrows I128/U128 to I64/U64
- `opt.fd:221` — `is_lto_mode()` hardcoded false
- `opt.fd:1649` — `cse_key` unreachable duplicate Const* arms
- `pir.fd:2681` — `str_substring` returns original string (stub)
- `codegen_c.fd:2523` — `DynCall` emits comment only (not implemented)
- `codegen_c.fd:2555` — `TryUnwrap` emits comment only (not implemented)
- `pe.fd` — 20+ runtime stubs return wrong values (vec_insert, string_contains
  returns true, string_split returns empty, etc.)
- `mono.fd:649` — `subst_gat_type_args` always uses args.get(0)
- `typechecker.fd:1451-1505` — 13 safety verification functions are empty stubs
- `comptime_eval.fd` — Return re-wrap omissions in substitute/replace functions
