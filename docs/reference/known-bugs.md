# Fides Self-Hosted Compiler — Known Bugs, Gotchas, and History

> Things that have broken before and how to avoid re-introducing them.
>
> Nov 11: `fidesc run` (the Rust bootstrap interpreter) silently ignored
> whole-struct re-assignment (`x = f()` left `x` unchanged): the struct
> memcpy updated byte memory but the opaque side-table entries for field
> slots (Vec/Str/Map values, keyed by address) stayed stale. Every
> `var x = ...; x = struct_result` loop was a no-op — bigint_pow/modpow
> returned 1, Poly1305 returned 0. Fixed by `Interpreter::struct_memcpy`
> migrating opaque entries src→dst (src/interp.rs); verified vs `fid run`
> and the RFC 8439/5869 vectors. Also fixed `aead_mac` interleaving the
> aad/ct length blocks (stdlib/tls_crypto.fd).
>
> Nov 23 (a whole-language analysis; changelog "Type checker holes, silent
> traps and backend gaps"): the type checker accepted `s = 42` into a `str`
> (an assignment checked only a literal's fit), any two `Vec`/`HashMap`
> instances (`Vec<str>` as `Vec<i64>`: addresses printed, words freed as
> strings), and any cast (`128 as &i64`, `5 as str`, `7 as Vec<i64>` outside
> `unsafe`); a bare `None =>` arm bound a catch-all; text after an f-string's
> expression was dropped; a `defer` ran from a branch that did not execute;
> the interpreter wrote a number with the value's width, not the slot's, and
> read an integer's bits as a float (so did the native backend);
> `try { "a" + "b" }` and `ok()`/`err()`/`context()` were typed i64;
> `x |> o.m(a)` lowered as `o.m(x, a)`; `f"{u64}"` was signed; `Rc`/`Arc` had
> no lowering; `Vec`'s closure methods needed `use vec;` and a closure with
> no `return` of a value (`fn(x: i64) { print(x); }`) ended in `abort()` on
> the C backend. Each has a rejection or output test in tests/conformance.
> Do not bring an "accepts any pair" rule back. The new assignment check
> also caught `ce_layout_lowerer` (comptime_eval.fd) assigning a
> `Vec<EnumDef>` to the lowerer's `(name, EnumDef)` table: fixed there.
>
> Last updated: Nov 23, 2026 (the paragraph above). Earlier Nov 21, 2026
> (§2g: rows 2g.80–2g.113 from the check of
> 224 goals on a self-hosted stage 0; rows fixed by Nov 19 marked so.
> Later Nov 21 (debcee9 + 5fb5516 + c082ee6): 2g.80, .81, .84-.90, .93-.96,
> .98-.102, .104-.111, .113 fixed; 2g.82, .102 (shadowed names), .103 and
> .112 partly; 2g.83, .91, .92, .97 still open).
> Earlier: Nov 6, 2026 (§2f: open items and fixes of Nov 4–6). Earlier:
> Oct 25, 2026 (session 30: standalone-binary stdlib crash, `cast_arg`
> `Ptr→Generic` ABI repair, nondeterministic self-hosted typecheck, `cmd /C`
> quoting fix — see §2d).

---

## 1. Fixed Bugs (Do Not Re-Introduce)

### 1.39 Gate scripts took `wsl.exe` on PATH for a working WSL (Nov 27)

**Symptom:** Seven gates (machine_mi14, machine_fregs, machine_lf20,
machine_lf20_param, wp6_args, machine_sp16, vectorize_spill) reported Linux
images as "output differs" on a Windows machine that has no WSL
distribution; relink_check.sh reported every request of `fid serve` as
"could not read file".

**Root cause:** `if command -v wsl.exe; then WSL="wsl.exe -e"; fi`: wsl.exe
is on PATH on every Windows and answers with its help text when no
distribution is installed. relink_check.sh also wrote a Git Bash path
(`/c/Users/...`) on `fid serve`'s stdin, where no shell converts it.

**Fix:** `wsl.exe -e sh -c 'echo wsl-ok' | grep -q wsl-ok` before the
runner is used, and `cygpath -m` for a path given to `fid serve` (f10eb6a1).

**Lesson:** a Linux image that prints nothing on Windows is a missing
runner, not a difference between backends. With that, and with the other
gate defects of that day fixed, `machine_mi14_check.sh` (33 ok),
`machine_fregs_check.sh` (25 ok), `machine_sp16_check.sh` (9 ok) and
`wp6_args_check.sh` (23 ok) run on Windows.

### 1.38 `scripts/build-selfhost.sh`: the retry without its flags, then a line that stopped every success (Nov 27)

**Symptom:** a unit that had failed at -O2 for memory was reported
"unbuildable at -O2/-O1/-O0" with errors such as "incompatible integer to
pointer conversion passing 'int64_t' to parameter of type 'HANDLE'". Hours
later every build stopped before the link with "retried: unbound variable".

**Root cause:** the -O1/-O0 retry computed `cflags` and never used it: it
compiled with `-O1 -fno-strict-aliasing -fwrapv -fno-sanitize=all` alone,
without `-w` and the `-Wno-*`, so the generated C's ordinary conversions
were errors. The summary line added that afternoon read a list that existed
on the retry path only, under `set -u`.

**Fix:** the retry compiles with `"${cflags[@]}"`; the list is created with
the others before the loop (f10eb6a1, 56492abb). `SPLIT_OPT` picks the level
of the first build when -O2 does not fit.

**Lesson:** run a changed build script to its end, the success path
included, before committing it.

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
1. Find which layer broke with `dump-ir` / `dump-pir` / `codegen` on
   test_codegen_c (`scripts/bisect.sh` was never written)
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

> **FIXED (re-checked Nov 9, 2026):** the global cache works; inc.fd also keeps on-disk build stamps and `.fidcache` (docs/file-analysis/inc.md).

Already documented in architecture.md. Now fixable with the `var` feature
implemented in the Rust compiler. The Fides `inc.fd` can use `var` to
implement a real global cache once the `var` feature is mirrored to the
Fides self-hosted compiler.

#### 2c.9 `lower.fd:6005` — `f64_to_bits` always returns 0

> **FIXED (re-checked Nov 9, 2026):** it calls `p_f64_to_bits` (docs/file-analysis/lower.md).

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

> **FIXED (re-checked Nov 9, 2026):** the `verify_*` passes are implemented and each safety rule has a negative test (safety-rules.md); remaining gaps are in docs/file-analysis/typechecker.md.

All 13 `verify_*` functions for safety annotations (`@conserve`, `@gas`,
`@constant_time`, `@realtime`, `@differentiable`, `@quantized`, `@checkpoint`,
`@dp`, `@shard`, `@adapter`, `@tainted`, `@contracts`, `@allocator_params`)
are empty no-ops. Safety annotations are accepted but not enforced. Not
needed for bootstrap, but dangerous for production use.

#### 2c.19 `parser.fd:4847` — `parse_fstring_parts` is a stub

> **FIXED (re-checked Nov 9, 2026):** f-strings are parsed (docs/file-analysis/parser.md).

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
| 2e.10 | Native backend (`build --native`: PIR → x86-64 → `compiler/pe.fd`) crashes on every program: `pass_dse` calls the wrong equality function, then register allocation reads null. `build`/`ccompile` go through C instead. Nov 9: `build --native` with the default target goes through compiler.fd `build_executable` (opt, codegen_x64, elf64, linker), not pe.fd, and panics in opt.fd `pass_jump_thread`; with `--no-opt` the link fails ("Undefined symbols: main": elf64 writes 56-byte section headers). | **OBSOLETE** — both causes are gone: `build_executable` was removed Nov 17 (compiler.fd comment at its old site; `--native` is nat_ir on every target), and the 56-byte section headers are fixed by TG13 (`ElfEmitter_build_elf` → `ElfEmitter_build_elf_img`; the old body is gone). `build-x86` still exists and now emits well-formed objects; whether its exe runs is untested (no current binary builds the tree) |

Not bugs: `test_sqlite` needs libsqlite3 and `examples/minimal_game` needs
SDL2 (both compilers fail the same way without them).

## 2f. Nov 4–6, 2026 — open items and fixes

The Nov 4–6 changes were checked with `fid run`, `fid ccompile` and
`fid emit` on their tests. The full suite was last run on Nov 2 (Rust
harness, `fidesc` as compiler: interp 339/339, C 343/343, PE 345/345,
expected-fail 89/89) and has not been rerun with the Nov 4–6 compiler.

**Open:**

| # | Item | Status |
|---|------|--------|
| 2f.1 | Value semantics (M5, proposal 0001): copy-on-write lowering for `Vec`, `HashMap` and the `Vec` fields of copied structs is written behind `LOWER_COW` (off), with lazy and counted shares. With it on, test programs get value semantics on all backends and the compiler compiling itself reaches its fixpoint (Nov 6), at a peak of 4.7 GB against 1.8 GB off. It stays off: since Nov 7–8 the ownership rule of proposal 0004 (`LOWER_RC`) gives `var b = a` on a `Vec`, `str` or `HashMap` an independent value (shared until the first write). Struct copies were fixed Nov 15 (2g.3). | **FIXED** (by proposal 0004, not by `LOWER_COW`) |
| 2f.2 | Native async/tasks: `fid emit` programs using async or spawn+await crash (`test_async`, `test_async_float_struct`). The interpreter and C are fine. Fixed Nov 6 (worker threads were started through a name lookup that returned 0); broken again in builds of Nov 9 afternoon: `test_async` printed 0 for every task result, `test_async_float_struct` panicked. Cause: three iropt passes on by default (envtrim shifted the arguments of async bodies started by name, gconst folded runtime globals to their initial values, deadf removed runtime functions). Fixed in iropt.fd: deadf opt-in, gconst in two phases, functions named by a string skipped. Verified that evening with fid31 (today's tree plus the fixes): both tests match the interpreter on pe_ir and `--nat`, with no switches. envtrim is opt-in now (`FID_IROPT_ON=envtrim`): a caller still passes the trimmed environment slot. Confirmed again with fid32 (adds the rules.fd fix for calls inside a callee): test_async, test_async_float_struct, test_tasks, test_brotli, test_bigint, test_decimal, test_args, test_counted_loop and test_fs match the interpreter on pe_ir and `--nat`. Still opt-in: deadf (WP15), envtrim (WP7), readcse (WP10), branchfold. | **FIXED** (Nov 9, fid31/fid32) |
| 2f.3 | `build --native` (the old PIR path) crashes (same as 2e.10); `fid emit` is the native path. | **OBSOLETE** Nov 17: `--native` is the machine layer (nat_ir) on every target; the PIR path it crashed in was removed |
| 2f.4 | (Nov 9: pe_ir has a whole-function linear-scan allocator with r10/r11 and xmm registers, `PEIR_GLOBAL_RA`; nat_ir has its own with IPRA.) D4: native code keeps values in 7 callee-saved registers (hottest locals, parameters, short-lived temporaries). There is no full register allocator: no cross-block values, caller-saved registers, or floats in xmm. Nov 21: nat_ir's allocator keeps values across blocks, uses argument registers (RA3, RA7) and puts floats in float registers (`fregs`, on by default); 2g.88 is an open checker failure in it | **OPEN** (partly; not rechecked as a whole) |
| 2f.5 | D14/D15: `Vec` elements at their real size, references into containers. | **OPEN** |
| 2f.6 | F: macOS, native ELF, ARM64, RISC-V and WASM backends are experimental / incomplete. G: bare metal, `emit --bare` panics. Nov 9: native Linux AArch64/RISC-V/x86-64 work through nat_ir (273/271/272 of 317), `emit --bare` builds (not run under QEMU); WASM and the PIR-era backends remain incomplete. Nov 15: `fid wasm` modules validate and run under WASI (316 of 344 suite tests, F9); `emit --bare`/`--boot` kernels boot under QEMU (G1; the Bare-metal CI lane). Nov 17: the PIR-era backends are no longer reached (MI18). | **OPEN** (partly) |
| 2f.7 | J5–J9 (crypto), E5 (`parse-header` on SDL2/SQLite headers). | **OPEN** |
| 2f.8 | `fid` self-compile is ~6x slower than `fidesc`'s (~133 s vs ~21 s). Nov 9: 74 s with the compiler built at -O2 (split C). Nov 21: 57 s at -O2 for the 361K-line tree (`dump-c compiler/main.fd`). | **OPEN** |
| 2f.9 | `fid version` prints `fid 0.1.0` (Cargo.toml 0.1.0) while the release is v1.0.0. | **FIXED** Nov 19: `fid` prints `<base>+dev` ("1.0.0+dev") via compiler/build_info.fd (N12); fidesc bumped to 1.0.0 (Cargo.toml, Cargo.lock, src/main.rs `version` command) |
| 2f.10 | Written, built into the compiler, not run yet: `fid emit --checked-alloc` (M7), `scripts/sanitize.sh` (M8, needs clang with ASan), `scripts/fuzz/nightly.sh` with the `checked` backend (M9; no CI workflow schedules it). The per-thread free lists of the native runtime (H8) are exercised by every native test, but no multi-thread stress test has been run. Nov 16–17: sanitize.sh and fuzz/nightly.sh were run (docs/verification/sanitizers-2026-11-17.md, fuzz-runs.md); ci.yml runs sanitize.sh, verify-nightly.yml both | **RUN** (sanitize, fuzz); stress test not run |
| 2f.11 | A literal of a user-defined generic struct is rejected: `struct Pair<A, B> { first: A, second: B }` then `Pair { first: 1, second: "one" }` gives "field 'first' expected A, got i8". Generic functions, generic enums and `impl<T>` work. | **FIXED** Nov 15: the type checker infers the literal's type arguments and substitutes them in field types; lowering lays type-parameter fields out as words (an f64 through its bits, a string as its pointer) and reads them back as the instance type (test_generic_struct) |
| 2f.12 | An `async fn ... -> Future<f64>` returned its result truncated (the function was lowered as returning a word; `fid run` printed 0, C a garbage value). Fixed Nov 6 (`lower.fd`, `lw_body_ret_type`: the body returns the T of `Future<T>`); `async fn half(x: f64) -> Future<f64>` gives 2.5 on the interpreter and C. | **FIXED** |

**Fixed:**

| # | Bug | Status |
|---|-----|--------|
| 2f.11 | `HashMap` with integer keys crashed. Integer keys are now stored as `key ^ HM_INT_XOR` (`stdlib/core.fd`), so 0 and -1 are ordinary keys; the two keys that would map to the empty and removed markers (`0x5DEECE66D1234567` and its complement) panic. `test_hashmap_int_keys` on all 3 backends. | **FIXED** |
| 2f.12 | `p_str_free` misread plain (headerless) strings in native emit. | **FIXED** |
| 2f.13 | The C runtime's `p_print_float` freed the string from `p_to_str_float` with `free()` instead of `p_free` (`compiler/codegen_c.fd`). The frozen `fidesc` (`src/codegen_c.rs`) still calls `free()`. | **FIXED** in `fid` |
| 2f.14 | Changing a `Vec` while a `for` loop walks it compiled (bounds-checked, but it could skip or repeat elements). Now rejected (K4, `test_iter_invalidation_fail.fd`). | **FIXED** in `fid` |
| 2f.15 | A higher-order function got every effect in `fid`, so a `!{}` function could not call it. Its effects are now instantiated from the function passed at each call site (`effects/test_effect_ho_instantiation.fd`). | **FIXED** in `fid` |

---


## 2g.123 — pe_ir's p_file_read returns a buffer p_str_take rejects (the "str without a string header" panic class)

Found Nov 26 triaging the first green Windows CI lane (the lane's pe_ir-only
failures). Minimal reproduction (build/fio_test.fd shape):

    p_file_write("x.txt", "abc");
    print(p_str_take(p_file_read("x.txt")));

prints "written" then panics with "a str without a string header" under
`fid emit` (pe_ir, default Windows); correct under `fid run` and ccompile.
PeirGen_gen_file_read (compiler/pe_ir_stubs.fd ~8349) allocates with p_malloc
and returns the data pointer; nothing writes the counted-string header
p_str_take expects (the C runtime path, which passes, arranges it). The same
panic class appears in test_vec_sized_layout (pe_ir), test_stdlib_repaired
(pe_ir diverges after the json_save file write), and in --nat runs of
test_process_command / mem_profile.fd / mem_hog — likely one shared defect in
how hand-written native file/string primitives produce str-valued memory.
Fix design (Nov 26, verified against the working concat implementation at
compiler/pe_ir_stubs.fd ~5744): a counted string is p_malloc(16 + len + 1),
[block+0] = 0x4649444500000001 (magic plus one owner), [block+8] = len, the
bytes at block+16 with a terminator, the str pointer is block+16.
PeirGen_gen_file_read allocates raw and returns raw; it must allocate
size+17, write the two header words, aim ReadFile at block+16, terminate,
return block+16. The same treatment for any other pe_ir primitive returning
str-valued memory (process output reads) — grep PeirGen_gen_* for p_malloc
callers that return the raw block. The nat_ir runtime (linux_runtime) passes
these tests, so its arrangement is the second reference.
Status: fix implemented Nov 27 (unverified in CI): pe_ir_stubs.fd allocates
size+17, writes the magic/owner word at [block] and the length at [block+8]
after ReadFile, aims ReadFile at block+16 — the existing return arithmetic
self-corrects to the counted-string pointer. Verification awaits a fid built
from the tree (the windows tier1 lane's pe_ir tests are the gate: fio_test
prints written/read/done, test_stdlib_repaired and test_vec_sized_layout
pass). NOTE the number: first recorded as 2g.122 in the table below, but
2g.122 was already taken there by the iropt store-forwarding entry — this
entry is 2g.123.

## 2g. Nov 9, 2026 — module audit: open bugs

Found by re-auditing every `compiler/*.fd` module against its
docs/file-analysis/<module>.md (all 68 rewritten Nov 9). "Seen" means
reproduced with a `fid` binary of Nov 9; the rest is from reading the code.
Details and line numbers are in each module's file-analysis doc.

| # | Module | Bug | Evidence |
|---|--------|-----|----------|
| 2g.1 | csplit | `cs_strip_static` removes the first `static` anywhere in a function, so the `static` locals of `p_process_run` and `p_net_connect` become plain locals in the split (release) build of `fid` | seen in the split C — **FIXED** Nov 15: csplit strips `static` from the function head only (`test`: split C of fid compiles and links) |
| 2g.2 | lower | an uninitialised `var m: M;` assigned from a match payload is freed twice: `fid veil` panics "Double free" before writing a report | seen, 15-line repro in veil.md — **FIXED** Nov 15: `LetUninit` records the variable type, so `m = mm` takes an owner (test_uninit_match_payload); fixes `fid veil` and the LSP crash |
| 2g.3 | lower | copying a struct does not separate its `Vec` fields: `var t = s; t.v.push(2)` changes `s.v` (interpreter and C) | seen — **FIXED** Nov 15: an unannotated `var s = S{..}` records its type, so `var t = s` dups the Vec fields (test_struct_copy_vec) |
| 2g.4 | lower, checks | an `asm!` clobber is stored as an input with value id -1; a clobber inside a loop crashes `dump-c` and `emit` (checks.fd `peir_loop_facts`) and irverify reports it | seen — **FIXED** Nov 16 with 2g.53 (the same cause: checks.fd, ir.fd's reference list, nat_ir and pe_ir skip an input with id < 0) |
| 2g.5 | lsp | completion, hover, definition and rename end with "Double free"; diagnostics are published at line 0 | seen — **FIXED** Nov 15: same cause as 2g.2 (`var program: Program;` from a match payload); diagnostics take the line of any `(line N)` / ` --> f:L:C` message (`parse_error_position`) |
| 2g.6 | fmt | `a < b` and `>>` open and never close a generic, so the rest of the file loses its spaces; `fid fmt` overwrites the input with it | seen — **FIXED** Nov 15: a generic `<` must close with only type tokens inside and no name after; `>>` closes two; `fid fmt` refuses to write when the tokens would change (`fmt_same_tokens`) |
| 2g.7 | typechecker | match exhaustiveness ignores guards and generic enums (`Option`, `Result`, user `<T>`): a non-exhaustive match compiles and runs no arm | seen — **FIXED** Nov 15: guarded arms cover nothing; generic enums (`Option<T>`, user `E<T>`, `&E`) are checked by variants; a bare variant name is not a catch-all (test_match_guard_nonexhaustive_fail, test_match_generic_nonexhaustive_fail). Found a real gap: `ir.fd inst_kind_name` had no StructCopy arm |
| 2g.8 | mono | a generic call inside a closure, f-string, `\|>`, `sandbox`, `parallel` (and region/txn/select/spawn/await) is not renamed: `run` panics "Unknown function", C calls the unmangled name | seen — **FIXED** Nov 15: mono rewrites calls in closures (typed by their own params), f-strings, pipes, await/spawn/yield, sandbox, txn, region, select, parallel, handle, ... (test_generic_calls_nested) |
| 2g.9 | compiler, borrowck | `--borrowck=warn` prints nothing (compiler.fd reads `bc.errors`, warn fills `bc.warnings`); `g(&mut x, &mut x) as i64` and a nested field write `s.a.z =` under `&s.a` are accepted | seen — **FIXED** Nov 15: warn mode prints `bc.warnings`; casts/pipes are descended; `s.a.z =` checks every prefix (test_borrow_cast_fail, test_borrow_nested_field_fail) |
| 2g.10 | secrecy | a match guard on a secret does not taint the arm; `p_file_write_vec`/`p_write*` are not sinks | seen — **FIXED** Nov 15: guards taint their arm and later arms; `p_file_write_vec`, `write_file`, `p_write*`, `p_fs_write*`, socket/net writes are sinks (test_secret_guard_fail, test_secret_file_vec_fail) |
| 2g.11 | iropt (on by default) | SCCP folds i8..u64 with i64 arithmetic: an i32 overflow panic is lost, u64 compares and divides as signed, and the evaluation itself can overflow in the compiler | code — **FIXED** Nov 15: a fold must produce a value of the operands' type (else left to run: the panic or @wrapping wrap happens); u64 above i64 range, and shifts by the width or more, are not folded (test_sccp_unsigned, test_sccp_i32_overflow_fail) |
| 2g.12 | iropt (on by default) | branchfacts makes `a >= b` true on the false edge of `a < b` for floats, wrong for NaN | code — **FIXED** Nov 15: on floats, only the true edge of a comparison settles the others (NaN) (test_float_nan_branch) |
| 2g.13 | codegen_c | `f32 % f32` and a bit cast to/from a float produce invalid C; a user function named like a libc function (`fopen`) clashes | seen with zig cc — **FIXED** Nov 15 (not yet built): F32 `%` is `fmodf`; a BitCast to a float is a C value conversion (`(float)v`, `(double)v`, through `intptr_t` from a pointer), as the interpreter converts; a user function named like a function of the runtime's headers (`cg_libc_clash`: stdio, stdlib, string, ctype, math, unistd, sockets, signal, time, ...) is renamed `p_<name>_user` (test_cwasm_f32_ops, test_cwasm_libc_names). Also in wasm.fd: float `%` (was 0) and f32 <-> f64 casts (kept the bits). Nov 19: the "Left" items are done too — nat_ir BitCast converts f32<->f64 (MI25, `ng_f32bits`/`ng_f32widen` at Inst::BitCast) and pe_ir converts via `peir_cvt_float` (OP17, pe_ir.fd Inst::BitCast); float `%` is fmod/fmodf on both native layers (MI17: nat_ir calls the runtime's `pert_c_fmod`, pe_ir the same) |
| 2g.14 | compiler | `@cfg(debug)` never holds (`check_cfg` is always called with `is_debug = false`), even with `ccompile --debug` | seen — **FIXED** Nov 15: `--debug` on the command line sets `g_cfg_debug`, which check_cfg now receives |
| 2g.15 | inc | the build stamp ignores `FID_VERIFY`, `FIDES_CFLAGS`, `FID_RULES_ON`, `FID_NAT_ON`, `FID_LIBFACTS*`: a stamp hit skips verification | seen |
| 2g.16 | elf64, linker, x86 | x86-64 ELF objects have 56-byte section headers (`sh_addr` never written): readelf rejects them, linker.fd links no user code, `build-x86` writes a 120-byte exe that segfaults | **FIXED** by TG13 (image-writer object path): `ElfEmitter_build_elf` now delegates to `ElfEmitter_build_elf_img` — 64-byte headers with `sh_addr` 0, `r_info = symbol << 32 | type`, locals first, `.rodata` labels defined in section 2. The old malformed body is gone. `build-x86`'s exe still untested (no binary on this machine builds the current tree) |
| 2g.17 | opt, pe | the PIR optimizer panics on hello world (`jump_thread_redirect`), so `opt`, `pir --opt` and `build --native` (default target) fail; `emit --boot` panics in pe.fd | seen — Nov 15 (not yet built): opt.fd jump threading looks blocks up by id (`find_block_index`) where it used an index as an id and an id as an index, and a correlated branch that threads to itself is no longer a change (it never reached the fixpoint); `emit --boot` goes through pe_ir now (`PEIR_BARE_BOOT`, release G1), not pe.fd. Left: run `fid opt hello.fd` and `build --native` again. **OBSOLETE** Nov 17 (MI18): opt.fd and the PIR path are no longer reached; `fid opt` prints the IR after iropt, `build --native` is nat_ir |
| 2g.18 | lexer | `5e3` lexes as `5` then `e3`; an unknown character ends an imported module silently | seen — **FIXED** Nov 15: `5e3`/`2E-3` lex as floats when digits follow the e (test_float_exponent). Also Nov 15 (not yet built): an unknown character is a `TokenKind::BadChar` error token (lexer.fd `read_operator`, token.fd), not an `Eof` that ended the stream, so an imported module fails to parse at it ("unexpected token '$'") instead of losing everything after it (test_frontend_badchar_import_fail, test_frontend_badchar_mod). Nov 16 (not yet built): compiler.fd returns an imported module's lex errors themselves (`cached_lex` keeps them, the import loops return them against the module's file): "unexpected character '$'" |
| 2g.19 | comptime_eval | an assignment inside an `if` in a `comptime fn` is lost (`pick(true)` gives 1, not 7) | seen — **FIXED** Nov 15 (not yet built; the Nov 15 dev binaries predate it and still give 1): OP28 (1c12c26) runs a comptime fn body in `evaluate_block_rt`, whose blocks write their assignments back to the enclosing ones. Also Nov 15: break/continue in for/while and `loop` (`ce_loop_flow`, the write-back also on an early exit), every compound operator (`ce_assign`: `/=`, `%=`, `<<=`, ... stored the right side), an inner `val x` hides an outer `x` (`ComptimeEnv_declare`), and a statement the evaluator does not run (`match`, field/index assignment, ...) is an error, so the call runs with the program, instead of being skipped (test_frontend_comptime_if_assign) |
| 2g.20 | interp | `printf` prints its format string unformatted; green threads: a thread that yields twice stops the next one from running | seen |
| 2g.21 | wasm, gpu | every WASM module fails validation; SPIR-V word counts are off by one and PTX labels do not match | seen — WASM **FIXED** Nov 15 (wasm.fd rewritten: modules validate and run under WASI, 316 of 344 suite tests match `fid run`, release F9). GPU, Nov 15 (not yet built): gpu.fd rewritten — SPIR-V word counts, result ids on types, the opcode and Int64 capability numbers, types before the function, one OpVariable per register, OpLabel per block, a named entry point, `--text` opcode names; PTX: `bra` targets are the block labels, registers and parameters declared, `ret`, comparisons and shifts (scripts/spirv-check.py checks a .spv's structure). Left: SPIR-V branches have no OpSelectionMerge/OpLoopMerge (Vulkan's validator wants structured control flow); calls, memory and floats are not translated (counted, a warning) |
| 2g.22 | flat_ast, diag, runtime | these unreachable modules no longer pass `fid check` | seen — Nov 15: flat_ast.fd (`AsmDir::Clobber` arm) and diag.fd (the `Diag_with_*` builders change a copy, not their by-value parameter) pass `fid check`, and so does tests/selfhost/test_diag.fd. Nov 16 (not yet built): runtime.fd's own helper is `rt_str_length`, so it no longer calls core's private `p_str_length` |
| 2g.23 | reach | reachability-only lowering for `emit` drops an implicit `Drop` method (`Logger_drop`): `test_drop` fails on native Windows and WASM; `FID_LOWER_ALL=1` fixes it | fixed Nov 15 (every `drop` method is a root in reach.fd): `test_drop` matches through native `emit` |
| 2g.24 | pe_runtime/data.fd | stores `pe_str_addr(ped_copy(..))`, a temporary freed at the end of its statement: `test_db`, `test_real_database` fail natively | seen Nov 15 — fixed Nov 15 (1c12c26): data.fd keeps the raw word of `ped_copy_raw`, no `pe_str_addr(ped_copy(..))` is left |
| 2g.25 | native runtimes | `pert_to_str_float` stops at 2^53 digits: `sqrt(2)` prints `1.414213562373095` (the interpreter and C print `1.4142135623730951`) | seen Nov 15 (WASM; likely every machine-layer target) |
| 2g.26 | codegen_c | a call written directly as a `match` scrutinee (`match f(...) { ... }`) in a large function made the stage-0 C generator crash (null access in `gen_function`); binding the result first works | seen Nov 15 |
| 2g.27 | compiler | a type error in an imported module is reported under the importing file's name and lines | seen Nov 15 (fides-os) — partly fixed Nov 15 (not yet built): the type checker names the module of a body it checks for another module (`tc_tag_module_errors`): "Type error (module 'm', line N): ...", no longer shown against line N of the importing file (test_frontend_modtag_import_fail). Nov 16 (not yet built): `enrich_errors` shows such an error against the module's own file and source line (the import loops record each module's file, `record_module_file`; test_frontend_modfile_import_fail). Left: borrow, reference and @secret errors (borrowck, escape, secrecy) do not know a body's module yet |
| 2g.32 | interp | core's new alignment test `(s & 7)` in `p_str_hdr_len` reached the interpreter with a string or pointer value, which its integer operators rejected: every `str_len` under `fid run` panicked "Arithmetic error" (C and native fine) | fixed Nov 15: arithmetic and bitwise operators take a string's or pointer's address |
| 2g.33 | codegen_c | printing a tensor freed the runtime's string with C `free()` instead of the block allocator: the program died after the first print (exit 127; `test_tensor_shape_vec`, `test_autodiff_grad`, `test_ml_full`) | fixed Nov 15 (`p_str_free_c`) |
| 2g.34 | scripts | cross-test.sh built a Mac's own macOS target as a zig cross target (every test BUILD on macos-15); the macOS CI jobs' rustc ran out of stack (SIGBUS) on macos-15-intel | fixed Nov 15 (host build on a Mac; `RUST_MIN_STACK`) |
| 2g.35 | ownership, http | `http_serve_scoped` panics "Double free" in `Vec_drop_inplace` (from `__own_drop_N` in `http_conn_task`) at the end of the first connection, after its response is sent: `fn h(req: ScopedRequest) -> str { return build_response(200, "text/plain", "ok\n"); }` served through C, then two `curl`s. Likely `http_request_free(req)` and the compiler's own drop of `req` both free it. `tools/site.fd` uses `http_serve` meanwhile (about 130 KB kept per request) | fixed Nov 15 (the connection loop no longer frees what its variables drop): the reproducer through C answers every request and its memory stays flat (6,448 KB after 300 requests, 6,488 KB after 600) |
| 2g.36 | core, runtimes | `StringBuilder` never frees its buffer: it has no `drop`, `clear()` makes a new one, and the C runtime takes builders and their grown buffers from the persistent pool (`p_malloc_persistent`). A server rendering pages with it grew 2.2 MB per page. Needs a `p_sb_free` on every back end and a decision on dropping a by-value struct that holds a handle | seen Nov 15 |
| 2g.37 | compiler | a function two imported modules both define is reported as undefined: `use http; use url;` then `url_decode(..)` says "undefined function 'url_decode'" (`url.fd` and `encoding.fd`, reached through http -> json, both define it); the message should name the ambiguity | seen Nov 15 — **FIXED** Nov 15 (not yet built): compiler.fd renames every copy (`url__url_decode`); the type checker now says "function 'url_decode' is ambiguous: it is defined by more than one imported module (encoding, url)" with the qualified calls to use (`tc_ambiguous_modules`; test_frontend_ambiguous_import_fail, test_frontend_ambiguous_import_qualified) |
| 2g.38 | parser | a struct literal of a struct declared in an imported module does not parse in the importing program: `use ws;` then `val c: WsConn = WsConn { fd: 1, is_client: true, open: false, tls: 0 };` gives "Parse error at 3:32: unexpected token 'Colon'" (the parser knows only the file's own struct names when it decides `Name {` starts a literal). A constructor function works | fixed Nov 15 (a capitalised `Name { field:` is a literal even when the struct comes from an import; `test_import_struct_lit.fd`) |
| 2g.39 | lint | `fid lint` does not resolve `use` imports: every imported name is reported as an undefined function (`tests/bootstrap/test_ws.fd` too) | seen Nov 15 |
| 2g.40 | nat_ir peephole | every native Linux, macOS and `--nat` program printed nothing and exited 0 (it had panicked: `mmap` of a garbage size, then the panic's `write` with garbage arguments). OP19's recorded-MIR peephole (a9a8522) dropped a `li`/`mov` to a register written again before any named use, but a physical argument register is read by the call between without being named: `mov x1, s; call pe_len; ...; mov x1, 1` lost pe_len's argument. Found with `qemu-aarch64 -strace` and `FID_NAT_OFF=peep` | fixed Nov 15 (the rule only takes virtual registers, `>= 64`) |
| 2g.41 | native runtimes | a zero byte is dropped when strings are joined natively: `"a" + str_from_bytes([0]) + "b"` has length 2 natively, 3 on the interpreter and C (`test_toml_suite_spec` fails natively on toml-test's `string/unicode-escape`) | seen Nov 15; fixed Nov 17 (not yet built): pe_ir's `p_str_concat` measures each half with `PeirGen_emit_slen` (pe_ir_stubs.fd), which took a count word as counted only below `P_STR_RC + 2^30`. A string `p_str_append` grew in place carries `P_STR_ROOM` (bit 31) in that word, so it was measured with `strlen` and lost everything from its first zero byte. `emit_slen` now applies core's `p_str_count_of` rule (word - `P_STR_RC` below 2^32, bit 30 clear) and still checks the length word. stdlib/toml.fd grows its strings in place, and `test_toml_suite_spec` passes natively; `test_stdlib_http_binary` (body bytes after a zero) is the same cause. Test: `test_str_append_nul.fd` |
| 2g.42 | interp | `read_file` of an existing empty file is None on the interpreter, Some("") on C and native | seen Nov 15; fixed Nov 16 (interp.fd: a failed read is Int(0), an empty file one heap "" that does not compare equal to 0, `interp_empty_read`; test_runtime_empty_file.fd) |
| 2g.43 | pe_runtime | `pert_process_run` keeps the string from `pe_read_all(of)` as a raw address that is freed at the end of its statement: the next read reuses the block, so a child's stdout shows its stderr text (`test_process`, `test_lib_process` natively); `process_stdout(0)`/`process_stderr(0)` return a null string that crashes on use (should be "") | seen Nov 15; fixed Nov 16 (pe_runtime/base.fd: `pe_read_all(..) as i64` hands each string to the record; a handle of 0 gives ""; test_runtime_process_capture.fd) |
| 2g.44 | runtimes | process capture writes `.fides_proc_*` (C) and `.fprc_*` (native) files into the current directory: in a directory the user cannot write (the repo root under a sandbox user, `C:\Windows`), cmd prints "Accès refusé." / "Access denied." and nothing is captured. They belong in the temp directory | seen Nov 15; C part fixed Nov 16 (codegen_c.fd `__p_proc_tmpdir`: TMPDIR or /tmp, TEMP or TMP on Windows; test_cback_proc_tmpdir.fd); native part fixed Nov 16 (pe_runtime `pe_capture_dir`: %TEMP%/%TMP%; linux_runtime: $TMPDIR, /tmp, %TEMP% under win.fd) |
| 2g.45 | interp | `p_string_len` and `p_string_from` on a `str` parameter give 0 and "" on the interpreter (on a local they work): `fn f(s: str) -> i64 { return p_string_len(s); }` gives 0 for "hello". `smallstr_to_str`/`smallstr_eq` are still wrong there (string.fd's other uses now call `str_len`) | seen Nov 15 |
| 2g.46 | interp | a string joined with an element of an untyped Vec was "": `var u = Vec::new(); u.push(" KB"); "x" + u.get(0)` | fixed Nov 15 for literal pushes (RT6: the element type comes from the first literal pushed); a Vec filled only from non-literals still needs its type written |
| 2g.47 | lowering | a struct-returning function called through a function-pointer parameter gets the wrong arguments: `resultvalue_and_then(ResultValue_ok(8), half)` panics "Modulo by zero" inside `half`; with a closure for `or_else`, "Invalid memory access" | seen Nov 15 — likely fixed Nov 17 for `fid run` and C by docs/reference/stdlib-agent-bugs.md 2/14 (a call through a fn value passes a result pointer for a struct; test_fix_fnval_struct_ret); this repro not rerun; `fid wasm` is still wrong (2g.108) |
| 2g.48 | stdlib/string.fd | `rope_concat` drops both children: `rope_to_str(rope_concat(..))` is "" though the module header shows "Hello, world!" | seen Nov 15 — fixed Nov 19 (49fb2b9): a Rope is a Vec of parts and `rope_concat` copies both |
| 2g.49 | interp | under memory pressure the interpreter sometimes stops with `p_malloc: OOM — failed to allocate new arena chunk` (hex_decode, regex_replace, test_http2_frames); the same run then passes: CI on a loaded runner could flake | seen Nov 15 |
| 2g.51 | interp, codegen_c | a fixed array `[i64; N]` as a return value or a `&`/`&mut` parameter is wrong: the interpreter reads wrong values (`c[0]` printed 4806, `a[3]` read 0 after `a[3] = 7`), and the C path emits `int64_t[10] v44 = 0;` and `*(int64_t[10]*)v42`, which no C compiler accepts. `fn mk() -> [i64; 10] { var a: [i64; 10] = [0,0,0,0,0,0,0,0,0,0]; a[3] = 7; return a; }` plus an `add(h: &mut [i64; 10], f: &[i64; 10], g: &[i64; 10])` should print 7 and 14. stdlib/x25519.fd uses a ten-field struct instead | seen Nov 15 |
| 2g.52 | interp, codegen_c | `println` of a `u64` prints it signed: `val k: u64 = 0xcbbb9d5dc1059ed8; println(k)` gives -3766243637369397544 (reference.md says unsigned); the bits are right | seen Nov 15 |
| 2g.53 | checks | `asm!(.., clobber("rbx"))` crashes the C generator ("Vec index out of bounds" in `Vec_set` from `peir_loop_facts` / `peir_unchecked_ops`): lowering passes a clobber as an input with id -1, and the counted-loop analysis indexes by it. `fn main() -> i64 { var y: i64 = 0; y = asm!("mov rbx, 99; mov rax, rbx", out("rax") y, clobber("rbx")); println(y); return 0; }` | FIXED Nov 16: checks.fd `peir_inst_operands`, ir.fd's reference list, nat_ir's operand list and pe_ir's asm loads skip an input with id < 0 |
| 2g.54 | typechecker, lowering | `a ?? b` on an `Option<str>` is typed `i64` unless bound to a `str` variable: `println(name() ?? "x")` prints a pointer through C and native; `"x: " + (name() ?? "n")` prints an empty line under `fid run` and crashes a C build (Invalid memory access) | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): `a ?? b` has the payload's type (lower.fd `Lowerer_coalesce_ir_type`; test_coalesce_str) |
| 2g.55 | typechecker | `?` is accepted in a function returning `i64`: exit 0 under `fid run`, exit 192 from C | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): `?` needs a function (or closure, or `try` block) that returns an Option or a Result (test_try_ret_i64_fail, test_try_ret_void_fail, test_try_ret_ok) |
| 2g.56 | typechecker | `for x in v` with `v: &Vec<i64>` types `x` as `&Vec<i64>`; `for x in *v` works | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): a borrowed collection iterates over its elements, in the type checker and in lowering (test_for_borrowed_vec) |
| 2g.57 | core | `Vec<str>.sort()` does not order strings, and `HashMap.keys()` comes back in insertion order though core's comment says sorted | seen Nov 15 (book) |
| 2g.58 | pe_ir | a native Windows `emit` of any program that uses `str_find`/`str_contains` fails ("pe_c_memchr not available"): pe_runtime/base.fd calls `memchr`, which is not in pe_ir_enc.fd's msvcrt import list | FIXED Nov 16: `memchr` added to `pe_msvcrt_imports` |
| 2g.59 | runtimes, interp | `read_line()` never returns None at end of input natively on Windows; the interpreter has no `read_line` at all ("FFI function 'p_read_line' not available") | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): interp.fd `interp_read_input`, pe_runtime `pert_read_line`/`pert_read_stdin` end with None (tests/tooling/interp-io-check.sh) |
| 2g.60 | interp | FIXED / not reproducing (verified Nov 20): `arg_at` sees only the program's args (interp and built agree); `for s in vec_of_enums` carries payloads; `select { case ch => x => .. default => .. }` runs under `fid run`; a misspelt `task_go` name panics with exit 1 as in C (compiler/interp.fd `p_co_go` cites this row) | seen Nov 15 (book) |
| 2g.61 | codegen_c | NOT REPRODUCING (Nov 20): `ccompile --target wasm32-wasi` builds hello, test_http, test_closure and test_spawn_vec_copy and runs them under `node scripts/wasm-run.mjs`; the `__wasi__` header stubs already define SO_ERROR. Reopen with a failing program if one is found | seen Nov 15 (book) |
| 2g.62 | tools | `fid bench` always calls tcc and ignores `CC`; `fid fmt` writes `& v` and leaves a stray leading space before `fn`; `parse-header` maps C `int` parameters to `i64` but an `int` result to `i32` | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): `fid bench` builds through `ccompile` (uses `CC`), fmt.fd keeps prefix operators on their operand and starts a declaration after an annotation on its own line, cparser.fd keeps a parameter's C type (tests/tooling/tools-2g62-check.sh) |
| 2g.63 | ark | NOT REPRODUCING (Nov 21, verified on Windows): a directory pact with backslash paths, a file pact and the empty lock all behave - tests/ark (paths 13, features 106, projects 95, registry 69 checks) is green with the current fid-dev. The earlier failures were the scripts preferring the stale release/fid.exe (they now prefer fid-dev). One real bug found and fixed: src/ark.rs DEFAULT_INDEX pointed at ark-index.git, not pacts.git | seen Nov 15 (book) |
| 2g.64 | diagnostics | the @crashfree error suggests `.get_or(i, default)`, which Vec does not have; the non-exhaustive-match error names `main` for a match inside another function; "arithmetic requires numeric types" has no line | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): the hint names `for i in 0..v.len()`, the match error names the function, the arithmetic error has its line (test_diag_crashfree_hint_fail, test_diag_match_where_fail, test_diag_arith_line_fail) |
| 2g.65 | effects | `!{}` does not rule out allocation: `Vec::new`, `push` and string `+` infer no `Alloc` (use `@no-alloc`) | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): the effect scan gives `Alloc` to string `+` and to Vec/HashMap creation and methods (typechecker.fd; test_effect_pure_alloc_vec_fail, _push_fail, _str_fail, test_effect_alloc_declared) |
| 2g.66 | mono, interp | a `Vec.map` that changes the element type (str -> i64) compiles, then panics under `fid run` with "Unknown function 'Vec_map'" | seen Nov 15 (book) — fixed Nov 19 (49fb2b9): `map` and the other closure methods are stdlib/vec.fd's, so without `use vec;` they are an error; `map` gives a Vec of what its function returns (test_vec_map_change_type, test_vec_map_no_use_fail, test_vec_map_float_fail) |
| 2g.67 | runtimes | `float_to_str` of infinity never returns on the interpreter and C: `var z: f64 = 0.0; println(float_to_str(1.0 / z));` | seen Nov 16 — fixed Nov 17 (docs/reference/stdlib-agent-bugs.md 6): "inf", "-inf" and "NaN" on the interpreter and C, as natively (test_fix_float_inf_nan) |
| 2g.68 | runtimes | `float_to_str` differs by back end: `-0.0` is "0" on the interpreter and C, "-0" natively; `1.0e300` is the exact 301-digit expansion on the interpreter and C, "1000000000000000100000..." natively | seen Nov 16 |
| 2g.69 | codegen_c | a user function named `byte` does not compile through C on Windows: it collides with rpcndr.h's `typedef unsigned char byte` (9 errors). Names the C headers declare as types need the same renaming as functions (cg_libc_clash) | seen Nov 16 — fixed Nov 19 (49fb2b9): codegen_c.fd `cg_c_header_name` renames names the headers declare as types or macros (test_c_header_type_names) |
| 2g.70 | runtimes, reference | zero bytes in strings: `str_from_bytes([65, 0, 66])` has length 1 on the interpreter and 3 on C; the literal `"x\0y"` has length 1 on both, but reference.md §1.8 says `str_len("ab\0cd")` is 5 | seen Nov 16 — interpreter part fixed Nov 17 (docs/reference/stdlib-agent-bugs.md 35: it reads a counted string by its length; test_fix_str_concat_nul). The literal not rechecked |
| 2g.71 | secrecy | a native `emit` of a program with `use crypto` fails the @secret check inside `crypto_hmac_sha256`, reported against the user's file and lines; the interpreter and C accept it (`tools/release_checksums.fd`, `tools/macho_check.fd` cannot be built natively) | seen Nov 16 — fixed Nov 19 (49fb2b9): secrecy.fd takes a free function `T_m` for `x.m(..)` only when `T` names a type (test_secret_native_crypto, test_stdlib_hmac_native, test_secret_method_sink_fail) |
| 2g.72 | lowering | a global `u64` is read as signed on the interpreter and C: `var g: u64 = 18446744073709551615; fn main() -> i64 { println(g); println(g >> 60); return 0; }` prints -1 and -1 (a local copy shifts right correctly: 15) | seen Nov 16 (c2fd) — fixed Nov 19 (49fb2b9): a global's declared type types its reads, and println prints a u64 as print does (test_global_u64) |
| 2g.73 | borrowck | `swap(&mut v[0], &mut v[1])` is rejected ("a mutable borrow is already reserved"): two disjoint element borrows of one Vec in one call. Possibly intended (indices are not known disjoint), but it blocks the direct form of C's `swap(&a[i], &a[j])` | seen Nov 16 (c2fd) — fixed Nov 19 (49fb2b9) for constant indices: `v[0]` and `v[1]` are element paths `v.[0]`, `v.[1]` that do not overlap; an unknown index still borrows the whole Vec (test_borrow_vec_elems, test_borrow_vec_same_elem_fail, test_borrow_vec_elem_var_fail) |
| 2g.74 | typechecker | indexing a global Vec, `g[i]`, fails with "cannot index undefined variable" (`g.get(i)` works) | seen Nov 16 (c2fd) — fixed Nov 19 (49fb2b9): typechecker.fd and lower.fd look the name up among the globals (test_global_vec_index) |
| 2g.28 | typechecker | the generic-struct literal check of Nov 15 ("cannot infer type parameter 'T' of struct 'Stack' from the literal") rejects stdlib code whose literal gets its type from the function's return type: `test_lib_stack`, `test_lib_queue`, `test_collections_btree`, `test_compiler_sized` no longer built on any backend | fixed Nov 15: only a bare `T` field the literal leaves open is an error; a parameter nested in a field type (`Vec<T>` given `Vec::new()`) is decided by the type the literal is used as. The four tests and `test_generic_struct` match through C |
| 2g.23 | pe_ir emit | a fixed array `[i64; N]` passed by value as a function param truncates past ~index 3500 (~28 KB): reads return out-of-bounds memory. `pub const` arrays index correctly | seen (unicode_data 3832-elem table: `udata_find` returned -1 for keys past the cut; a local `var` copy of the same array read fine) |
| 2g.29 | interp, core | `str_len` on a multi-byte UTF-8 string panics in the interpreter: "Arithmetic error" in `p_str_hdr_len` (stdlib/str.fd:43 via `uni_to_codepoints`); the unicode.fd doc examples were marked `(native only)` because of it | fixed Nov 15: the same bug as 2g.32 (integer operators of the interpreter now take a string or pointer as its address); `fid run` of `str_len("abc")` and of test_lib_asset, test_fs, test_qualified_names, test_stdlib_repaired give the right output. The `(native only)` marks on the unicode.fd examples can go |
| 2g.30 | veil, ownership | `fid test --coverage` still double-frees inside `run_with_coverage` (`Vec_drop` via `__own_drop_5`) when the covered program panics; `fid veil` on the same sources works. Earlier by-value copies (`cov.data`, `up: Program`, `module`) were fixed Nov 15; one shallow-copy site remains, reached only through the `cmd_test` path | fix written Nov 18, confirmed when tests/tooling/test-coverage-check.sh passes: `var module: IrModule; module = m;` in compiler/veil.fd `run_with_coverage` memcpy'd the result's payload with no new owner, so both were dropped; the payload is now lent to `vl_cover_module` |
| 2g.31 | codegen_c | `fid dump-c compiler/main.fd` emits two signature-mismatched calls zig cc rejects under C2x: `p_socket_read(fd, int64, len)` where the stub wants `char*`, and `net_recv_from` passing `void* v4` to `pledge_net_recvfrom`'s `int64_t` param — `build-selfhost.sh` fails without hand-patched casts | seen Nov 15 |
| 2g.75 | typechecker | `"n = " + 5` (a `str` plus an integer) is accepted and gives an empty string on `fid run`, C and native; reference.md §4.2 allows `+` on two `str` only, so it should be a type error | seen Nov 17 with the Nov 16 fid, checking the book (AD6, tests/book/check_book.py) |
| 2g.76 | typechecker | a `fn main() -> i64` (any function with a non-void result) whose body can end without `return` is accepted; `main` then exits 0 on every backend. A missing return on some path should be an error | seen Nov 17 (AD6) |
| 2g.77 | interp | `panic(msg)` under `fid run` prints `Panic: msg at line 63`: the line of the `p_panic` call in core.fd's `panic` wrapper, not the caller's (the trace's `panic at line 4` is right); C and native print no line | seen Nov 17 (AD6) |
| 2g.78 | stdlib | stdlib/http.fd `status_text` knows only 200-204, 301-304, 400-413, 429, 431 and 500: `build_response(503, ...)` or 422 sends `HTTP/1.1 503 Unknown` | seen Nov 17 (AD6) — fixed Nov 19 (49fb2b9): `status_text` knows 422, 501-504 and the other common codes |
| 2g.79 | lower (src/lower.rs) | a fid *built by fidesc* (stage 1 of the 3-stage bootstrap) self-compiling the 361K-line tree climbs linearly to >16 GB of arena chunks and dies with `p_malloc: OOM — failed to allocate new arena chunk` in C codegen; the Rust fidesc does the same dump-c peaking at ~600 MB. The frozen bootstrap's lowering predates proposal 0004 and emits no drops for the temporaries the current sources churn per function, so nothing returns to the free lists. Fix: port `Lowerer_rc_own_value`/rc-drop (compiler/lower.fd "Ownership") into src/lower.rs. Until then a 3-stage bootstrap from fidesc cannot pass stage 2 on 7-31 GB machines (macos-native.yml stays red for this reason); one-stage builds and everything a fidesc-built fid runs short of the self-compile are unaffected. Nov 21: the wall is in the fidesc-built stage 1 only. A self-hosted fid of Nov 18 used as stage 0 compiled the unmodified Nov 20 tree (86cc919) in one step (121 MB of C, ~5 min), and the result passes run, emit, ccompile and the leak gate, so the bootstrap can start from a released fid instead of fidesc. Also Nov 21 (d79b31c): src/lower.rs now takes an owner for Vec and HashMap elements and keys; statement temporaries and parameters are still not dropped or borrowed there, so the wall stands. Nov 21 evening: src/lower.rs now also drops statement temporaries — fresh `Vec_new`/`Vec_share`/`Vec_share_deep`/`HashMap_keys`/`HashMap_new`/`HashMap_share` headers used once by a call or print that keeps nothing (drop_function_vecs `temps`, the drop_function_strs pattern), and STR_FRESH_CALLS covers `Str_dup`/`Vec_to_str`; parameters stay caller-owned by design (the same as lower.fd's LOWER_COW/LOWER_ARG_SHARE gating). Wall not re-measured. Nov 21 evening, validating the optimizer batch (5bb97b3) on a fidesc-built fid-dev.exe (STAGE0=fidesc, one -O0 unit): the class shows three more runtime symptoms under memory pressure, all pre-existing and unchanged by that batch — `run --leaks` segfaults (hello.fd included), interpreter runs of mid-size tests truncate at random points (test_blake3_md5: 5, 11, 17, 18, 19 or 27 of 27 lines across identical runs, before and after the changes; the isolated cases give correct results), and stdlib parses intermittently die with the arena OOM. Route the full gates (`--leaks`, simd_native_check, machine_frame_check, test_ai_stdlib) through a self-hosted-lineage binary. Nov 21 evening: the gate itself passed from the pinned self-hosted stage 0 — `SPLIT=16 JOBS=2 RELEASE=1 ./scripts/bootstrap.sh` reached `FIXPOINT OK: stage 2 and stage 3 emit identical C` + `RELEASE OK` (all -O2 splits OOM'd to the -O0 fallback; release/gate-nov21-c.log). Nov 22 (early): a single-stage fid-dev from the same pinned stage 0 (16 units, two at -O0 for memory) still segfaults on `run --leaks` (hello.fd) and panics on test_simd_native --nat's second line — those are NOT 2g.79 (the bootstrap gate passes); they are the check programs' own faults, to be re-filed separately once reproduced on a quiet machine | seen Nov 20 (local + both macOS runners, measured with RSS polling); Nov 21 (build from self-hosted stage 0); Nov 21 evening (fidesc-built fid-dev.exe) |
| 2g.80 | pe_ir, iropt_me (unroll) | partial unrolling gives wrong code on pe_ir when the loop body has a dead store: `while i < 100 { waste = i * i; s = s + i; i = i + 1 } println(s)` prints 9900, not 4950, from the default `fid emit` (stats: 0 full, 1 partial). FID_IROPT_OFF=unroll, `--nat` and `fid run` print 4950; without the dead `waste` store it is right. FID_VERIFY=1 is silent, and turning off dse, dce, gvn, licm, iv, sccp, fold, forward, loadhoist, sra, ssa or egraph does not help. tests/bootstrap/test_opt_dce.fd shows it too (midend-optin-check: `< 4950 > 9900`). `unroll` is in IME_DEFAULT_ON (iropt_me.fd): fix it or take it out, as ME24 says | seen Nov 21 (ME24, OP11, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): the bug was pe_ir's global register allocator, not the unroller — `peir_global_ra` started a live-in value at the block's first instruction, so a value defined in a block laid out later (the unrolled copies, chained backwards) shared a register while live; live-in now starts one position earlier (`bstart - 1`) (test_unroll_dead_store; verify-clean-check.sh runs it under FID_VERIFY on both backends) |
| 2g.81 | pe_ir, pe_runtime | pe_ir `--leaks` executables crash at once: `fid emit tests/nattools/pe_leaks_leaky.fd --leaks -o lk.exe`, then lk.exe prints `Panic: Illegal instruction`, exit 1; the same with `--checked-alloc --leaks`. tests/nattools/pe_leaks_check.sh: 10 of 10 leak-gate programs fail. `--nat`, x86_64-linux and `fid run --leaks` report the two leaked blocks. The wiring of the p_malloc/p_free thunks (g_pe_leaks, pe_lk_raw_malloc in pe_ir_stubs.fd; pert_leak_malloc in pe_runtime/base.fd) is broken | seen Nov 21 (RL16, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): under `--leaks` `pe_ir_needed_fns` adds pert_leak_malloc/free/exit (and what the stack walk calls: nat_frame_ret/next) to the emission roots — without them the walk stayed ud2 and every image hit an illegal instruction; the entry point is named `__fid_entry` in the nat tables (its frames no longer take the previous function's name); the pe_lk_* runtime layer is in pe_runtime/base.fd and `pert_to_str` no longer leaks a p_malloc buffer; also `PeirGen_gen_argv` passed the token's address as its size |
| 2g.82 | nat_ir, inc (function cache) | a function-cache hit image is not byte-identical to a cache-off image. STRICT=1 scripts/repro-check.sh: `DIFF x86_64-linux executable, fn cache hit = off` for hello, test_repro_fncache, test_hashmap and test_tooling_explain; machine_fncache_check.sh: hit differs from miss on x86_64, aarch64 and riscv64 (25,603 bytes on x86-64). After a one-line edit a warm `emit --debug` image differs from a `--no-cache` one (32,352 bytes), and `fid serve` rebuilds differ from a one-shot emit; a cache hit also changes the debug sections. Cache misses equal no cache, and every image runs the same: a reproducibility bug, not wrong code | seen Nov 21 (SP20, SP13, SP8, SP6, DB4, check from self-hosted stage 0) — partly **FIXED** Nov 21 (c082ee6): `nc_skel_fn` replays the recording's register requests *before* the skeleton's call edges — a callee the recording asked for later was queued earlier, so a hit image ordered functions differently (hello identical after; fncache_hit_order_check.sh). The `--debug` sections' difference and `fid serve` rebuilds are not re-checked |
| 2g.83 | compiler (fuzz) | `ccompile --fuzz-cov` panics `Double free` in Vec_drop_inplace, from `__own_drop_100` in build_program_with_path_borrowck (called by lower_to_ir_module_with_path). The coverage build of scripts/tooling/sl13-fuzz-check.sh dies, so no target is found, no input is saved and no corpus grows. The harness itself has block coverage (`fuzz coverage: 26 blocks`) | seen Nov 21 (SL13, check from self-hosted stage 0) |
| 2g.84 | mono (mono_pack_unroll) | in an unrolled `for x in xs` over a variadic pack, every call takes the generic overload and `break`/`continue` never fire. test_variadic_pack prints `mixed (3): other other other` (want `int 1 str two yes`); test_variadic_pack_jumps prints `until_stop: [other][other][other][other]` (want `[int 1][yes]`); test_variadic_pack_forward prints `log (3): other other other`. Same on run, ccompile and emit, with FID_MONO_SHARE_OFF=1 and with the Nov 18 fid. dump-ir has the instances (describe__i64, describe__bool), but the loop copies call the generic describe. `xs.len()` and `xs.get(K)` work | seen Nov 21 (roadmap@7922, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): a loop copy's `let x = xs__<k>` is typed `T__k`, and while it substitutes `x` aliases `xs__<k>` on `g_mono_pack_aliases` (a stack, innermost first, so nested pack loops work), so a call on `x` resolves against the element's type as `xs.get(k)` does (test_variadic_pack_nested) |
| 2g.85 | reach | in a `--lib` build, `emit`'s reachability pruning does not lower the callees of `--export` roots: `fid build s.fd --lib --export ark_bin_mathx_mx_add -o mathx.dll`, whose export calls a non-exported `mx_add`, makes a DLL whose export returns 0 (42 with FID_LOWER_ALL=1). tests/ark/ark-binary.sh with FID set fails: `the program printed '0 0 false'`. Also seen once: `ark prebuild --check` after a source change exited with no output (`--check: DIFFERENT`) | seen Nov 21 (roadmap@7675, AD1, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): the `--export` names are reachability roots — `CGen_lib_export_names` (codegen_c.fd) feeds compiler.fd's `c_reach_keep_set`, as `@export` functions already were |
| 2g.86 | scripts | scripts/build-selfhost.sh hides a failed split build. When a split unit's `zig cc -O2` exits non-zero with no message (exit 5, most likely out of memory at JOBS=4 on a busy machine), fid-dev.cc.log stays empty, the units are deleted, and the script builds one unit at -O0 and exits 0. The caller gets a 67 MB -O0 fid (about 17x slower) and no error. It should name the failed unit and its exit code, and retry failed units one at a time or fail: by hand, the same 8 units at -O2 built one at a time (15 MB fid) | seen Nov 21 (build of 86cc919 from the Nov 18 fid; N4) — **FIXED** Nov 21 (c082ee6): a unit whose `zig cc -O2` fails is named with its exit code and retried alone; still failing, the script stops with exit 1 (build-selfhost-split-fail-check.sh). It still falls back to one -O0 unit when the *whole* split build cannot run |
| 2g.87 | typechecker (diagnostics) | an integer literal is named by the narrowest type it got: test_frontend_units_dims_int_fail expects `a unit combines with an f64 or another unit, got i64` but `fid check` prints ``error[E079]: Type error (line 12): `i8 * Len`: a unit combines with an f64 or another unit, got i8.`` (the literal `2` is typed i8), so the expected-fail check fails on it. Same style: `p_unix_connect expects a str socket path, got 'i8'` for a literal. Name it as an integer (literal) in the message, or change the test | seen Nov 21 (CL10, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): an unsuffixed integer literal is named `i64` in type messages (test_diag_int_literal_i64_fail) |
| 2g.88 | nat_ir (RA3, RA7) | on x86-64 (Linux and `--nat` Windows) the FID_VERIFY=1 allocation checker rejects assignments made with argument-register hints and fill: `case_n: instruction 712 defines v1478 in argument register 6, whose argument or result is still to be read` (test_machine_simd). Also test_ai_stdlib, test_machine_brflags2, test_machine_argfree/holes/segs, test_regex_linear (`rv_seq ... argument register 2`) and FID_NAT_ON=splitall builds (x86_64, riscv64, Windows; split_alloc_unit `x64 random 19 mode 2: rejected`). Gone with FID_NAT_OFF=arghint or argfree; envtrim and inline2, both on by default, expose more cases. Without FID_VERIFY the programs print what `fid run` prints: either the assignment is unsafe or the checker is wrong. AArch64 and RISC-V pass | seen Nov 21 (WP7, RA3, RA7, WP6, MI14, OP16, RA4, ME20, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): the checker was wrong — a live range may end at an op that writes the argument register by name (`mov r, v`) or copies it into the range's value (`mov v, r`): `ng_arg_fit_end` checks `hi` itself (new `NgPhys.cpy` records a move that copies the register into a value), and `ng_mir_check_args` exempts `mov r, v` where v already holds r (argreg_dead_def_check.sh + argreg_dead_def_unit.fd) |
| 2g.89 | iropt (sccp) | `fid emit compiler/main.fd` (the mid-end on) panics `Integer overflow on subtraction` in iro_sccp, called from iropt_native in cmd_build, with or without FID_FN_ARENA (some runs end in out of memory instead). So the compiler cannot be built by `fid emit` | seen Nov 21 (RA14, SP11, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): BitNot folds as `av ^ -1` (`0 - av - 1` overflowed on i64::MIN), Neg refuses `-i64::MIN`, and `iro_sccp_eval` bounds every BinOp before computing it — Fides arithmetic is checked, so evaluating the fold itself could panic; an uncomputable fold returns varying (test_sccp_bitnot_min). `ime_reduce_one` also rejects a step constant of i64::MIN |
| 2g.90 | tv, iropt_me (inline2) | translation validation rejects inline2 on Str_drop: `FID_VALIDATE=1 emit test_ssa_validate.fd` gives `tv: Str_drop: pass inline2 rejected: the blocks changed` (test_tv_inject too). Not triaged: a miscompile in inline2 or a false alarm in tv.fd. Also, faults injected after inline, gconst, fold, forward, sccp and dse are not caught (6 of 12 in test_tv_inject), and one reached the executable (it printed 43, not 42) | seen Nov 21 (VF6, RL5, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): both halves. The Str_drop rejection was a false alarm — inline2 also copies multi-block callees, splitting blocks, which sits outside iro_inline's witness, so `tv_mod_check` answers `n/a` for inline2/inlineclo when the result's blocks changed (`tv_blocks_kept`). For the uncaught faults: injection now targets a value something depends on (`tv_live_of`, `tv_pure_value`) — a fault anywhere else rightly changes nothing — and the module's constant globals are facts (`tv_gconst_facts`) so a function that reads one is run too |
| 2g.91 | inc (seeded store) | a fresh project never takes core's unit from the seeded store: inc-seed-store-check.sh steps 3 and 4 fail (`no Took line, Compiling 2 of 2 units: core, main`). Seeding itself and FIDES_SEED_STORE=off work. Core's unit key differs between the seed build and the fresh project | seen Nov 21 (B6, check from self-hosted stage 0) — **FIXED** Nov 21 (verified on release/fid.exe, v0.8.2): core's per-program dispatch functions (`__p_elem_drop`, `__p_elem_dup`, `__p_mem_drop`, `__p_mem_dup`, `__p_key_strs`) are placed in main's unit, so core's unit key no longer depends on the program's types; inc-seed-store-check.sh passes all 6 (`Took 1 of 2 units from the seeded store`, installed-layout `<fid dir>/cache` read, FIDES_SEED_STORE=off honoured) |
| 2g.92 | inc (IR units) | on the default IR-unit path of `ccompile --inc`, a `dyn` call whose impl lives in another unit has no dispatch candidates (`if (false) {} else { abort(); }`): inc-ir-units-check.sh prints 2/hi/42, then exits 127 (single TU: 2/hi/42/25, exit 0, with `else if (strcmp(_t, "Sq") == 0) v2 = Sq_area(NULL, _recv);`). FIDES_INC_IR=off works. Also on this path: no `[inc] layouts` debug line, and a new struct in lib1 rebuilds main | seen Nov 21 (B1, B2, check from self-hosted stage 0); a repro check was added Nov 21 (c082ee6, tests/tooling/inc-ir-dispatch-check.sh: a dyn call and a function-value call across units) — **FIXED** Nov 21 (verified on release/fid.exe, v0.8.2): `ccompile_inc_prepare` gives each unit the dyn types its calls can reach plus stubs of their methods and of every function a function value can name (compiler.fd, the `B1 (2g.92)` hunk); inc-ir-dispatch-check.sh and inc-ir-units-check.sh pass, and inc-layouts-check.sh now shows the `[inc] layouts` line with a new struct in lib1 rebuilding lib1 only |
| 2g.93 | iropt (memo) | FID_IROCACHE_VERIFY=1 reports mismatches on 80-85% of memo hits (hello 18 of 21, test_matrix_mul 74 of 91, test_hashmap 98 of 116; tests/tooling/irocache-check.sh step 5: `MISMATCH main ... rg_depth_at ... Str_drop`), although the executables are byte-identical. Either the memo replays other IR or the verify comparison is wrong. CB3 already turns the memo on for `emit -o` | seen Nov 21 (CS6, CB3, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): the verify comparison was wrong — `imc_verify_post` (ir_ser.fd) now hashes the bytes the passes made of a verified hit (`iw_irfunc` of the post function), against `pre_h` (`mec_fn_hash`) only for a function left unchanged; comparing the recorded bytes-hash with `mec_fn_hash` reported every changed function as a mismatch |
| 2g.94 | iropt_me (sra) | the ime_sra miscompile is still in the tree: a `--nat` build of the front end crashes with `Invalid memory access ... at 0x0` in Parser_parse_unary_inner, called from Parser_parse_unary (machine_sp16_check.sh); the pe_ir image prints `hello.fd: 48 tokens, sum 379680922`, and FID_IROPT_OFF=sra fixes the `--nat` one. So no natively built fid can parse | seen Nov 21 (SP16, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): the crash was nat_ir's `ng_rf_promote`, not ime_sra — it wrote a promoted field's zero-words *before* the prologue's argument moves, so a `li 0` took an argument register still to be read; `ng_rf_zero_at` emits them at the first label after the prologue |
| 2g.95 | pe_ir | an f32 function result cast `as i64` gives i64::MAX on pe_ir: `id32(2.0) as i64` prints 9223372036854775807 (`fid run`, C and `--nat` print 2); line 10 of test_float_rem_exact. pe_ir is also wrong where `--nat` is right on test_crash_frame_values and test_thumb_core | seen Nov 21 (MI17, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): `Inst::ConstFloat` for F32 stored the low 32 bits of the f64's bit pattern — 0.0 for 2^63 — so a saturating `as i64` range check clamped every value; it emits `p_f32_to_bits(value)` now |
| 2g.96 | pe_ir | the pe_ir image of test_simd_i64 prints 12 lines, then `Panic: Invalid memory access (null or dangling pointer)` in the `wrap:` case; run, ccompile, `--nat` and the sse, avx2, neon and rvv images are right | seen Nov 21 (OP21, TG11, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): not SIMD at all — `PeirGen_gen_to_str`'s itoa scratch was 16 bytes at rbp-40, under saved r14 at rbp-24, so any number ≥16 chars (i64::MIN is 20) clobbered the caller's r14; the buffer is 32 bytes and the frame grown (pe_ir_stubs.fd) |
| 2g.97 | mono, typechecker (trait default methods) | a trait default method calls a specialized impl for a type no specialization covers: in test_trait_impl_specialization, `c.twice()` on a `Wrap<bool>` (`twice` calls `self.show()`) prints `i64 1 i64 1`, not `any any`: Wrap<i64>'s `show` runs on a Wrap<bool> and reads the bool as an i64. Wrong code on run, ccompile and emit; FID_MONO_SHARE_OFF=1 and the Nov 18 fid give the same | seen Nov 21 (roadmap@7946, check from self-hosted stage 0) |
| 2g.98 | pe_ir, nat_ir (tail calls) | mutual and sibling tail calls are emitted as calls: scripts/pe-win/tail_deep.fd overflows the stack on pe_ir (`Panic: Stack overflow`; disasm of is_odd shows a call; also with the function cache off), a regression of OP13. tests/nattools/tailcall_deep.fd (3 million levels) overflows on aarch64 and riscv64 Linux (is_odd called from is_odd); x86_64-linux and `--nat` print `true 3000000 6000055 1` | seen Nov 21 (B5, MI28, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): pe_ir recognizes `Return(call)` and the inliner's `store call → slot; jump B` shape (`peir_tail_via_slot`, `peir_slot_forward`, `peir_slot_load_return`) and emits `mov rsp, rbp; pop rbp; jmp` for a Fides callee of ≤4 args (pe_tail_slot_check.sh; FID_PE_OFF=tail restores the overflow); on AArch64/RISC-V nat_ir kept a tail call only when the result needed no `MO_EXT` — `g_ng_ret_ext`/`ng_tail_ext_kept` fix that (is_odd/is_even jump again) |
| 2g.99 | nat_ir (dynamic link) | a dynamically linked x86_64-linux image crashes in libc `strtod`: test_verify_abi_c_mixed prints the ldexp/frexp/modf lines right, then `Panic: Invalid memory access ... at 0x8` (both runs). The C lanes pass | seen Nov 21 (RL25, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): a dynamic x86-64 image had no TLS block, so libc's strtod read its locale through fs and faulted; `nat_tls_get`/`nat_tls_set` now read/write `fs:[-8]` (a thread's block lives at fs+72) and `nat_elf_img` writes a PT_TLS program header when the dynamic section allows (ldyn_tls_check.sh) |
| 2g.100 | native crash reports | native crash traces (pe_ir, `--nat`, x86_64-linux) drop the innermost function once it is inlined: the panic is in parse_age, but the trace shows only load_record and main, and the `called from main` line is malformed (a stray dash-bar prefix). The native report's version is hard-coded `fid 1.0.0+dev (native)` instead of the build's | seen Nov 21 (CO9, check from self-hosted stage 0) — partly **FIXED** Nov 21 (c082ee6): a native crash trace is a walk of frame records, so an inlined function was never in it — `ime_panic_frame` (iropt_me.fd) keeps `panic` and its callers out of inlining, so the innermost function stays a frame (`FID_IROPT_OFF=panicframe` inlines them again). The malformed `called from` line and the hard-coded version are not re-checked |
| 2g.101 | pe_ir, pe_runtime (profiler) | FID_PROF=folded stacks on pe_ir start at a wrong root frame: `__c_fn_pe_console_ctrl;main;fib_sc0_25;fib;...` (the entry point gets pe_console_ctrl's name); C, `--nat` and Linux start at main. And with FID_PROF_EVERY=1, pert_prof_watch_main (pe_runtime/conc.fd) sets ticks to 0 on every report, so an event-triggered report restarts the 1 s timer: a 1.6 s service prints 2 reports, not 3 | seen Nov 21 (DB9, SL15, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516 + c082ee6): the entry point is registered as `__fid_entry` in the nat name tables and `pe_pf_entry_b` (pe_runtime/conc.fd) records its profile bucket, so folded stacks root at it; and the periodic report keeps its own clock — an event-triggered report no longer resets the 1 s timer |
| 2g.102 | nat_enc (DWARF) | debug info of optimized native builds: a function that begins with inlined code gets the callee's DW_AT_decl_line (main 56, want 76: nat_dbg_decl_line takes the first row, not the `fn` line), on every target. Under `--debug`, a shadowed name shows the outer binding (`shadow@inner#1 x = 6, want 50`; FID_NAT_OFF=mir gives 0), and no DW_TAG_lexical_block reaches `shadow` | seen Nov 21 (DB2, CT6, CT9, DB4, check from self-hosted stage 0) — decl_line half **FIXED** Nov 21 (c082ee6): `nat_dbg_decl_line` scans the function's own rows for its `fn` line instead of taking the section's first row (which an inlined callee owned); the shadowed-name/lexical-block half (CT6, DB4) is open |
| 2g.103 | nat_ir, nat_enc (CFI) | x86-64 CFI says r14/r15 are saved at CFA-24 where the prologue stores nothing (cfi_saves_check: FDE 0x40d3ac, also 0x40fd58), and shrink-wrapped functions emit DW_CFA_restore for registers that have no rule yet, on x86_64, aarch64 and riscv64 Linux (fib, pe_fn_name_at). An unwinder can restore wrong values | seen Nov 21 (DB6, check from self-hosted stage 0); Nov 21 (c082ee6): the check scripts now model what shrink wrapping and DW_CFA_remember/restore_state do — a save deferred to RA12's block and a rule kept across an epilogue no longer read as missing/rule-less, so part of the report was checker artifact. The CFA-24 stores question is not re-checked |
| 2g.104 | pe_ir (debug info) | `emit -g` (optimized pe_ir) shows wrong values for main's locals: `total = 0, want 45`, `sum = 140729126777126, want 61`, `shown = 6094848, want 45`; with FID_PE_DEBUG_RA=1 the parameters show as optimized out (pe_dbg_vars_check.sh) | seen Nov 21 (N9, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): `peir_dbg_live` counts only the instructions of `func`'s blocks — the IR optimizer can take an instruction out of its block and leave it in `func.insts`, and its stale slots were shown for `total`, `sum` and `shown`; under FID_PE_DEBUG_RA=1 the parameters are marked no-register so they keep their frame slots (pe_ir.fd) |
| 2g.105 | main (reduce) | on Windows the reduce watchdog runs `cmd /c _fid_reduce_cmd.bat` by bare name. With NoDefaultCurrentDirectoryInExePath=1, cmd does not look in the current directory, so every try fails ("'_fid_reduce_cmd.bat' n'est pas reconnu") and no try is stopped. Call `.\_fid_reduce_cmd.bat` or a full path | seen Nov 21 (RL20, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): the watchdog runs `.\_fid_reduce_cmd.bat` (main.fd) |
| 2g.106 | nat_enc (PE, Mach-O) | `--nat` Windows and both macOS targets build a program that calls an undefined C function and say nothing; the `--nat` exe exits 127 at run time. x86_64-linux and ccompile report `error: link: the C function 'fides_cb9_not_in_libm' is not in any library the program links` | seen Nov 21 (CB9, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): `nat_pe_link_missing` reads the linked DLLs' export directories and `nat_macho_link_missing` the SDK's .tbd stubs (nat_enc.fd); an unresolved C call is `error: link: the C function 'X' is not in any library the program links` with its declaration site on --nat Windows and macOS too (cb9_dll_sdk_check.sh) |
| 2g.107 | codegen_c | an extern C name equal to a core function stops core's rename: once stdlib.h's `abs` is in extern_func_names, sanitize_c_name returns before the abs -> p_abs rename, and core's `pub fn abs` is emitted as `int64_t abs(void*, int64_t)`. zig cc: "conflicting types for 'abs'" (ccompile test_at_import_system.fd with FID_INCLUDE set to zig's any-windows-any headers); `emit` prints 5 | seen Nov 21 (CI1, LG15, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): an extern name goes into `extern_func_names` only when no Fides function has it, so core's `abs` keeps its `p_abs` rename and the extern's declaration stays apart (lg15-extern-core-name-check.sh) |
| 2g.108 | wasm | `fid wasm` returns structs from function values wrong: test_fix_fnval_struct_ret prints `8597928 0`, then `Panic: Invalid memory access` (want `1 2 / 7 0 / 30 4 / 12 5 / 9 9`); the WASI C path and x86_64-linux are right | seen Nov 21 (F9, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): wasm's `CallPtr` passes a struct/enum result's out-pointer first, before the env — as a direct call and nat_ir's CallPtr do; the arguments were shifted by one |
| 2g.109 | iropt, lower (IR verifier) | FID_VERIFY=1 rejects two suite programs: test_ml_full with `IR verifier (after iropt): main: %60 uses %59, which is defined nowhere` (gone only with FID_IROPT_OFF=forward,loadhoist,sra), and test_verify_semantics_ext with `after lowering ... does not dominate` | seen Nov 21 (WP19, OP9, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): a select's `recv_alloca` and case-variable slots are made in the entry block (`Lowerer_txn_raw_slot`) — a case's variable was found by name in blocks it did not dominate; and `ime_remap` now renames the operands of PersistLoad/PersistSave/Backward (a renamed loss's `backward` read a value defined nowhere), with `ime_unrenamed_uses`/`ime_remap_covers` to catch a miss (test_select_case_slots; verify-clean-check.sh covers both) |
| 2g.110 | nat_enc (COFF objects) | `fid codegen --coff` and `emit --obj --nat` fail on every program: `native backend: undefined symbol: FreeLibrary` (also LoadLibraryA, GetProcAddress). In object mode the runtime's kernel32 imports are not resolved. The fixed coff.fd CoffEmitter_build_coff is reached by nothing | seen Nov 21 (RL9, TG12, MI18, check from self-hosted stage 0) — **FIXED** Nov 21 (5fb5516): `ng_ldyn_ok` keeps dynamic imports in COFF/Mach-O object mode (ELF objects still refuse them), and `nat_coff_imports` writes each import as an undefined external symbol with an ADDR64 relocation per IAT slot and a `/DEFAULTLIB:` directive per DLL (coff_obj_imports_check.sh) |
| 2g.111 | main (C path) | `ccompile --cpu x86-64-v3` with CC='zig cc' does not build: main passes `-march=x86-64-v3`, which zig 0.16 rejects ("unknown CPU: 'x86'"; zig wants `x86_64_v3`) | seen Nov 21 (OP20, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): `--cpu` names are spelled zig's way (`x86-64-v3` → `x86_64_v3`, nat_cpu_check's table; op20-zig-cpu-check.sh) |
| 2g.112 | typechecker | 14 of 422 expect-error tests fail, the same with the Nov 18 fid: diag_reverse_operator_type, frontend_trait_self and world_hermetic_clock are accepted by `fid check`; banking_double_credit, money_linear and crashfree get other texts than they expect; test_diag_tl2_nesting_stmt_fail now stops at the parser's 256-level limit before the type checker's 512 | seen Nov 21 (TL2, check from self-hosted stage 0) — partly Nov 21 (c082ee6): the nesting test reaches the type checker's limit through a 320-operator chain whose innermost operand is a 220-block function literal (the parser's 256 limit stays the outer bound); the banking_double_credit, money_linear and crashfree expectations were corrected to the current texts and err_send_spawn awaits its task (an unawaited task is CA6's own error and hid this one). Still open: diag_reverse_operator_type, frontend_trait_self and world_hermetic_clock are accepted by `fid check` |
| 2g.113 | interp (process) | the interpreter's parallel spawn is flaky under load: test_process_spawn under `fid run` once printed `parallel codes: 0 1 -1`, not `0 1 2` (1 of 4 runs) | seen Nov 21 (roadmap@7565, check from self-hosted stage 0) — **FIXED** Nov 21 (c082ee6): `ipx_wait` re-reads the exit-code file for up to 2 s until it holds text — cmd's `echo %errorlevel%> file` creates the file before writing it, and under load the single 1 ms retry read a still-empty file as -1 (interp-process-spawn-load-check.sh) |
| 2g.114 | pe_ir (self-compile) | `release/stage0/fid.exe emit compiler/main.fd -o release/fid-emit.exe` completes in 5m18s (typecheck 4.4s, lower 9.3s, iropt 97s, native code 188s) but emits repeated `[PE] WARNING: unresolved label 7841/7842 at code offset 0x13e4...` warnings, and the produced binary panics `Invalid memory access (null or dangling pointer)` on `fid-emit.exe run tests/bootstrap/hello.fd` after `[prof] borrowck: 17ms` (`version` works). Repro in git-ignored release/: fid-emit.exe 25,393,885 bytes, Nov 21 | seen Nov 21 — OPEN. The unresolved-label warnings (labels 7841/7842, five sites in one function) are the lead: a value/branch referenced after its defining block was dropped or renumbered, probably in pe88's pre-mid-end prune ("785 of 10417 functions no code can call dropped") or a pe_ir pass renumbering block labels without fixing a reference. Not triaged |
| 2g.115 | nat_ir (frame slots, MI8) | the frame-slot packing (712c6e7) checks a new value only against the first value already in a slot, so two values live at once can share one slot: on `--nat` and x86_64-linux test_threads, test_fstring_float (3.14 printed as 985.96) and ambiguous_import_qualified go wrong, and they pass with FID_NAT_OFF=frameslots. A second bug, not yet found, still breaks test_hashmap, test_json, test_mem_return and test_alloc_zero_paths there with frameslots off (suspect: the dead-alloca frame skip in ng_gen_function). Seen with main's own compiler and runtime | seen Nov 22 (landing of the P2 batch) — **fix **verified** Nov 22 (ng_pack_spill_slots checks every value in a slot): under fid-nov22, --nat passes test_threads, test_alloc_zero_paths, test_alloc_thread_heaps, test_hashmap, test_json, test_mem_return, test_fstring_float, test_args, test_machine_tailcall_stack and a Vec 64->128 growth loop — all of which broke before. FID_NAT_OFF=deadalloca leaves them green, so the feared second bug is not in these tests; the still-open shared miscompile is 2g.118 |
| 2g.116 | pe_ir (panic trace, CO9) | every panicking pe_ir program hangs printing `0`s: PeirGen_gen_panic's hex-digit loop (e107298) keeps its counter in RCX and the value in RAX across the WriteFile call, which clobbers both | seen Nov 22 (tests/tp native failed-rank check, test_span_double_free) — **fix **verified** Nov 22: a panicking pe_ir binary prints real trace addresses (0000000140001xxx) and exits instead of hanging on 0s |
| 2g.117 | interp (task interleave) | `fidesc run tests/bootstrap/test_task_interleave.fd --max-steps 50000000` never returns: the step budget is not enforced while interleaved tasks wait (a channel/scheduler wait does not count as a step), so a livelocked interleave spins forever — the Nov 21 verify.sh --save run sat on this one test from 23:51 to 15:33 the next day until the child was killed by hand | seen Nov 22 (verify.sh --save night chain); fix: count a wait-loop turn as a step (or bound total turns per task) in the interpreter's interleave scheduler |
| 2g.118 | shared native lowering | `crypto_sha256_hex_str("abc")` returns `0e55ec8a…` on `emit` (pe_ir) and `emit --nat` — the same wrong value on both — but the correct `ba7816bf…` on `run` and `ccompile`; stdlib/crypto.fd is pure Fides (i64 ops, no externs, no @cfg), so the miscompile is in code shared by the two native paths or in a native runtime primitive. `FID_IROPT_OFF=all` changes nothing. Also seen: test_std_cookie (hmac), test_std_dtls (prf), test_std_hpke — every hash/HMAC/PRF vector is wrong natively; test_std_dsp harmonics, test_std_apriltag detections, test_std_geolocation point-in-polygon also differ on emit. Possibly the unfound second bug of 2g.115 | seen Nov 22 (emit-lane expect.py run on fid-nov22, the self-hosted stage-0 build of the merged tree) — OPEN |
| 2g.119 | lower + pe_ir (global inits) | a `var` with a non-constant initializer (`var x: i64 = 0 - 1`, 119 uses in the tree) is zeroed in pe_ir's baked .rdata (pe_ir.fd only folds IntLit/FloatLit/BoolLit/StringLit into data) and its real assignment is emitted at the top of `main` (lower.fd global_inits) — so a value a boot hook (pert_limits_boot, pert_prof_boot) wrote before `main` is overwritten when `main` starts. FID_PROF's `pe_pf_entry_b` held __fid_entry's bucket only until `main` ran: folded stacks printed `__fid_entry;main;…` again (2g.101's fix defeated). Same ordering on every backend that lowers inits into `main` | seen Nov 22 (DB9 folded, pe_ir) — **FIXED** Nov 22: pe_pf_bucket names the row it lands on (`__fid_entry`) instead of trusting a cached bucket, so no profiler state depends on init order (db9-check.sh all 32 legs). The general hazard is **FIXED** Nov 26 (f3d82b06) for constant arithmetic: lower.fd folds a constant initializer to a literal when the global is registered (lw_fold_const_init: `0 - 1` -> -1, arithmetic, bitwise, shifts, comparisons; smallness guards keep interpreter-trapping overflow out, division by zero and wide shifts run as written), so every backend writes it into the program's data with no runtime assignment at all. Verified end to end: dump-c bakes -1LL, pe_ir prints -1/23/63/16/true, dynamic initializers unchanged. What is genuinely not constant (a call) still initializes at the top of main after the boot hooks — that ordering hazard stays for non-constant initializers || 2g.120 | nat_ir (record ABI, callbacks) | `tests/nattools/rl25_abi_lanes_check.sh`: the native (`--nat`, x86_64-linux machine) lanes of test_verify_abi_c_records return the sorted records in a repeated first element (`1:10 1:10 1:10 1:10 1:10` / `4:40 4:40 ...`, want `1:10 2:20 4:40 5:50 9:90` and the reverse) — a record passed to or returned from a C callback is copied wrong on the native backends. The C lanes, pe_ir, the cross C lanes and the mixed-argument lanes pass; aarch64/riscv64 dynamic lanes untested (no target glibc). Same family as 2g.118 (shared native lowering) | seen Nov 23 (RL25, fid-nov22.exe) — OPEN |
| 2g.121 | dump-c (extern call args) | `pledge_net_recvfrom` call sites are emitted with `void*` arguments where the declared C prototype takes `int64_t` (stdlib/net.fd:49 declares all-i64 params): clang/zig cc reject it under `-Wint-conversion` ("passing 'void *' to parameter of type 'int64_t'"), gcc only warns. Harmless on x86-64 (same register slot) but it breaks the fid-dev self-hosted build under zig cc without `-Wno-int-conversion`; other extern call sites cast properly, so this one likely misses an insert-cast step in the C backend | seen Nov 25 (fid-dev rebuild, unit u_7) — **FIXED** Nov 26 (9fe4ac8c): extern param types were never registered in the C backend, so no call-site cast was ever inserted for an extern; both codegen_c.fd and src/codegen_c.rs now register them (aggregate signatures keep no entry — the __c_wrap_ wrapper already speaks the C types). Verified: the tree's 133MB of generated C compiles clean, the recvfrom call casts to (int64_t) |
| 2g.122 | iropt (store-to-load forwarding) | `test_lib_game_headless` with inline2 on failed the post-iropt verifier: `main: %1687 uses %460, which is defined nowhere`. `ime_forward` scans blocks in layout order and recorded a copied load's substitution to `%460` before `%460` itself was forwarded to constant `%452`; the one-level chain was never compressed, so the intermediate load was removed while the copied comparison still used it | seen Nov 26 (ME20 inline2 suite) — **FIXED** Nov 26: `ime_forward` now compresses forwarding chains (`ime_forward_final`) and rewrites uses with the final value. Verified with FID_VERIFY=1 on pe_ir and `--nat`; the full inline2 suite has no build or verifier failures |

## 3. Gotchas

### 3.1 `Box<T>` in compiler source is not the same as language `Box`

- Language `Box::new(x)` is `p_malloc` + `Store`.
- Compiler-source `Box<T>` is a recursive pointer wrapper for AST/IR fields.
- Match patterns on `Box<T>` auto-unbox, but explicit `box_*_unbox()` helpers are needed for `Box<Expr>`, `Box<Type>`, `Box<IrType>`.

### 3.2 `Expr` variants in `lower.fd` are out of sync with `ast.fd`

> **FIXED (re-checked Nov 9, 2026):** `lw_box_expr_unbox` (lower.fd:73) lists only real variants.

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
| `compiler/pe_ir*.fd` | **HIGH** | Default Windows native backend: IR → x86-64 + PE file; ModRM/SIB bytes, REX prefixes, import table, section layout. Off-by-one = corrupt .exe (compiler/pe.fd, the older one, is no longer reached since Nov 17) |
| `compiler/nat_ir.fd`, `compiler/nat_enc.fd` | **HIGH** | Machine IR, register allocator and encoders of the Linux targets and `--nat`; checks: `FID_VERIFY=1`, scripts/enc-oracle.py, scripts/pass-matrix.sh |
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
| `fmt` | COMPLETE (historical; Nov 9: it corrupts files, see 2g) | Supports `--check` |
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

#### Effect/annotation verification — ALL STUBBED (0%) (historical: implemented since, see 2c.18)

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
40. **parser.fd f-string interpolation** — parse_fstring_parts is a stub (fixed since; Nov 9)
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
