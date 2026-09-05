# Fides Self-Hosted Compiler — Architecture

> File-by-file breakdown of the self-hosted compiler modules in `compiler/`.
>
> **Last updated:** Sep 5, 2026 (session 15 — **BOOTSTRAP FIXPOINT ACHIEVED**: fides-c2.c == fides-c3.c)
>
> **Note on line numbers:** Source line numbers in "Known issues (session 11
> analysis)" sections were accurate as of session 11 but may have shifted in
> sessions 12–14. Treat them as approximate guides, not exact references.
> Re-verify with `grep` before editing based on these numbers.
>
> **Module count:** 25 `.fd` files in `compiler/` (57,021 LOC total).
> The "18 modules" count in older session notes is obsolete.

## The Pipeline (Simplified)

```
main.fd
  ├─ compiler.fd
  │    ├─ lexer.fd  ──► parser.fd  ──► ast.fd
  │    ├─ typechecker.fd ──► ast.fd
  │    ├─ borrowck.fd ──► ast.fd
  │    ├─ comptime_eval.fd ──► ast.fd
  │    ├─ flat_ast.fd ──► ast.fd (dump-ast-flat serialization)
  │    ├─ lower.fd ──► ir.fd
  │    ├─ mono.fd ──► ast.fd
  │    ├─ pir.fd / opt.fd ──► ir.fd
  │    ├─ inc.fd ──► ir.fd (incremental compilation cache)
  │    ├─ codegen_c.fd ──► ir.fd  ──► C source ──► tcc ──► .exe
  │    ├─ pe.fd ──► ir.fd  ──► x86-64 machine code + PE file ──► .exe (no tcc!)
  │    └─ [Linux backend — ported but NOT yet integrated]
  │         ├─ peephole.fd ──► L3Module (9 optimization passes)
  │         ├─ regalloc.fd ──► L3Module (linear scan register allocator)
  │         └─ codegen_x64.fd ──► L3Module ──► x86-64 AT&T asm ──► ELF64 (future)
  ├─ lsp.fd ──► compiler.fd (get_diagnostics) + json.fd ──► JSON-RPC over stdio
  ├─ fmt.fd ──► source formatter
  └─ diag.fd ──► structured diagnostics
```

`compiler.fd` is the central hub. `ast.fd`, `ir.fd`, and `token.fd` are the
shared data-definition leaves that almost everything else consumes.

The `ccompile` command uses `codegen_c.fd` (C codegen + tcc). The `build`
command uses `pe.fd` (native PE emitter, no tcc). The `lsp` command uses
`lsp.fd` (JSON-RPC over stdio, diagnostics via `compiler.fd`). The Linux
native backend (`peephole.fd` + `regalloc.fd` + `codegen_x64.fd`) is ported
but not yet wired into the pipeline — integration requires porting
`elf64.rs`, `linker.rs`, and `runtime_linux.rs` first.

---

## `compiler/main.fd` — CLI entry point

- **Module:** `compiler::main`
- **Purpose:** Minimal command-line front-end. Supports `ccompile` (C codegen + tcc) and `build` (native PE emitter, no tcc).
- **Key functions:** `main()` dispatches commands; `cmd_ccompile()` parses CLI args and invokes the C codegen pipeline; `cmd_build()` invokes the PE emitter pipeline.
- **Imports:** `core`, `vec`, `string`, `compiler::compiler`, `compiler::pe`
- **Notes:** Top-level executable; no other compiler module imports it. The `build` command was added in session 5.

### Known issues (session 11 analysis)

- **`--borrowck` off-by-one** — line 71: `str_substring_from(garg, 12)` for
  `--borrowck=` (11-char prefix) drops the first value character. **P1.**
- **`g_borrowck` parsed but not passed through** — read from CLI but only
  printed, not forwarded to the compiler pipeline. **P1.**
- **`cmd_lint` is a stub** — line 1009: always prints "No lint warnings."
- **`cmd_run` has no interpreter backend** — line 547: compiles to C and
  executes the binary; `--max-recursion`, `--max-steps`, `--max-memory`,
  `--trace` are parsed but not wired.
- **`cmd_test` missing backends** — line 666: only C codegen + PE compile;
  no interpreter backend, no PE run check.
- **`cmd_build` unimplemented flags** — `--no-opt`, `--lto`, `--caps`,
  non-x86-64 `--target` are parsed but not implemented.
- **`read_source_or_error` silent failure** — returns `""` for unreadable
  files, indistinguishable from empty source.

## `compiler/compiler.fd` — Pipeline driver

- **Module:** `compiler::compiler`
- **Purpose:** Ties the whole compiler together: lex, parse, typecheck, lower, C codegen, plus path helpers, error enrichment, capability stripping.
- **Key types:** `ResultStr`, `ResultProgram`, `ResultIrModule`, `CompileResult`, `ErrCode`, `I64Pair`, `StrTriple`, `Option`
- **Key functions:** `build_program()`, `build_program_with_path()`, `ccompile_source_with_path()`, `lower_to_ir_module()`, `enrich_errors()`, `errcode_to_str()`
- **Imports:** `compiler::ast`, `token`, `lexer`, `parser`, `ir`, `typechecker`, `lower`, `mono`, `borrowck`, `codegen_c`, `comptime_eval`
- **Imported by:** `main.fd`

### Import resolution (`build_program_with_path`)

1. Parse stdlib (`core` and dependencies).
2. Parse the user source.
3. Resolve imports LIFO from a queue.
4. `core` is skipped (already loaded).
5. Each import path `foo::bar` is converted to `foo/bar` and tried in several candidate locations:
   - `base_dir/foo/bar.fd` and `foo/bar/mod.fd`
   - `stdlib/foo/bar.fd` and `stdlib/foo/bar/mod.fd`
   - project-root variants
6. A pre-scan parse extracts transitive imports and struct/enum names.
7. Modules with unloaded dependencies are deferred; dependencies are loaded first.
8. Once all dependencies are ready, the module is fully parsed and added to `imported_modules`.
9. All loaded modules are merged (`merge_programs`) with user code last.
10. Pipeline continues: cfg → derive → desugar → comptime → typecheck → borrowck → mono.

**Critical rule:** there is no standalone `parse_import` helper. Import loading and parsing are all inline inside `build_program_with_path`. Refactor with extreme care.

### Known issues (session 11 analysis)

- **`--borrowck` not wired** — `BorrowChecker_new(BorrowckMode::Off)` is
  hardcoded at line 1006 (TODO at 1003). The CLI flag is parsed in `main.fd`
  but never reaches the pipeline.
- **`apply_cfg_filter` is a no-op** — line 1295: `// No-op — include all items.`
- **`fill_default_trait_methods` incomplete** — line 1597: creates default
  method functions but never pushes them into `program.trait_impls`.
- **`c_file_read` suspicious comparison** — line 1960: compares `str` result
  to integer `0` (`if content == 0`), which likely never matches.
- **`c_os_cwd` hardcoded** — line 1985: always returns `"."`.
- **`inc.fd` global cache is a no-op** — `global_cache_get`/`global_cache_store`
  are stubs (see `inc.fd` section). Now fixable with the `var` feature.

---

## `compiler/token.fd` — Token types

- **Module:** `compiler::token`
- **Purpose:** Defines every token kind and the `Token` struct.
- **Key types:** `TokenKind` (133 variants), `Token`
- **Key functions:** `token_new()`, `token_kind_to_str()`, `is_word_keyword()`, `token_kind_eq()`
- **Imports:** none
- **Imported by:** `lexer.fd`, `parser.fd`, `compiler.fd`

## `compiler/lexer.fd` — Tokenizer

- **Module:** `compiler::lexer`
- **Purpose:** Byte-based tokenizer. Produces `Vec<Token>` with line/column info and collects lex errors.
- **Key types:** `Lexer`
- **Key functions:** `Lexer::new()`, `Lexer::tokenize()`, `keyword_or_ident()`, `read_identifier_or_keyword()`, `read_number()`, `read_string()`, `read_operator()`
- **Imports:** `core`, `vec`, `compiler::token`
- **Imported by:** `parser.fd`, `compiler.fd`

### Important lexer implementation details

- `src_len` is cached on `Lexer` to avoid calling `str_len()` on every `peek()`.
- `peek()` returns `0` (NUL) at EOF; `peek_at(offset)` returns `-1` at EOF.
- Identifiers and numbers are extracted with `str_slice(self.source, start, end)` to avoid O(n²) `str_concat`.
- UTF-8 multi-byte sequences are counted as one column; continuation bytes are skipped by `advance()`.
- `read_utf8()` extracts 2/3/4 byte sequences and returns a `Str` token.

### Known issues (session 11 analysis)

- **Duplicate `str_filter_underscores`** — empty stub at line 291 shadows the
  real implementation at lines 572-583. Numeric literals with underscores
  (e.g. `1_000_000`) will not have underscores stripped. **P0 — must fix.**

---

## `compiler/ast.fd` — Abstract Syntax Tree

- **Module:** `compiler::ast`
- **Purpose:** All AST node definitions used by parser, typechecker, lowerer, and codegen.
- **Key types:** `Program`, `Func`, `Stmt`, `Expr`, `Type`, `Pattern`, `BinOp`, `UnaryOp`, `ImplDef`, `TraitImpl`, `ExternDecl`, and many small helpers.
- **Key functions:** `program_new()`, `type_to_str()`, `binop_eq()`, `unaryop_eq()`, many `*_new` constructors.
- **Imports:** `core`, `vec`, `string`
- **Imported by:** `parser`, `lower`, `ir`, `typechecker`, `codegen_c`, `pir`, `opt`, `mono`, `borrowck`, `comptime_eval`, `compiler`

### Canonical AST enums

**`Expr` (lines 244-283):**

```fides
pub enum Expr {
    IntLit(i64), FloatLit(f64), StringLit(str), BoolLit(bool),
    Ident(str),
    Binary(BinOp, Box<Expr>, Box<Expr>),
    Unary(UnaryOp, Box<Expr>),
    Call(str, Vec<Expr>),
    FieldAccess(Box<Expr>, str), OptionalFieldAccess(Box<Expr>, str),
    StructLit(str, Vec<FieldInit>, OptBoxExpr),
    If(Box<Expr>, Vec<Stmt>, OptStmts),
    Range(Box<Expr>, Box<Expr>, bool),
    Block(Vec<Stmt>),
    MethodCall(Box<Expr>, str, Vec<Expr>),
    ArrayLit(Box<Type>, Vec<Expr>),
    Index(Box<Expr>, Box<Expr>),
    FString(Vec<FStringPart>),
    Match(Box<Expr>, Vec<MatchArm>),
    EnumLit(str, str, Vec<Expr>),
    Ref(Box<Expr>, bool),
    Deref(Box<Expr>),
    StrConcat(Box<Expr>, Box<Expr>),
    Try(Box<Expr>), Coalesce(Box<Expr>, Box<Expr>),
    IsCheck(Box<Expr>, str),
    TryBlock(Vec<Stmt>),
    TupleLit(Vec<Expr>),
    Closure(Vec<Param>, Vec<Stmt>, OptType),
    Cast(Box<Expr>, Type),
    Asm(str, Vec<AsmOperand>),
    Comptime(Box<Expr>),
    ComptimeBlock(Vec<Stmt>),
    ComptimeReflect(ReflectOp, Type),
    PipeForward(Box<Expr>, Box<Expr>),
    QuantCast(Box<Expr>, Type),
    Await(Box<Expr>),
    Spawn(Box<Expr>),
}
```

**`Stmt` (lines 312-342):** Let, LetTuple, LetUninit, Assign, IndexAssign, DerefAssign, FieldAssign, Expr, Return, If, While, For, Loop, Break, Continue, Defer, Match, Use, Handle, Do, RegionBlock, Sandbox, Unsafe, Txn, ComptimeGen, GradBlock, Backward, Query, Select.

**`Type` (lines 103-132):** I8/16/32/64/128, U8/16/32/64/128, F32, F64, Bool, Str, Void, Never, Infer, Named, Array, ArrayConst, Ref, RawPtr, Generic, Tuple, Func, Assoc, Dyn, ShapedTensor.

**`BinOp` (lines 136-155):** Add, Sub, Mul, Div, Mod, Eq, Neq, Lt, Gt, LtEq, GtEq, And, Or, BitAnd, BitOr, BitXor, Shl, Shr.

**`Pattern` (lines 287-295):** Wildcard, IntLit, BoolLit, Ident, EnumVariant, StrLit, Range.

### Important warning

`compiler/lower.fd` contains a `box_expr_unbox` helper that enumerates an
*out-of-sync* set of `Expr` variants (e.g. `Lambda`, `Throw`, `TryUnwrap`,
`Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`,
`Spawn`). These **do not exist** in `compiler/ast.fd`. The self-hosted compiler
must use the `ast.fd` canonical enums; `lower.fd` needs to be reconciled to them.

---

## `compiler/parser.fd` — Recursive-descent parser

- **Module:** `compiler::parser`
- **Purpose:** Turns `Vec<Token>` into an AST `Program`. Returns custom `ParseResult*` enums because Fides has no `?` operator.
- **Key types:** `Parser`, `ParseResultExpr`, `ParseResultStmt`, `ParseResultStmts`, `ParseResultType`, `ParseResultProgram`, `OpEntry`, `BinOpInfo`, `IfExpr`
- **Key functions:** `Parser_new()`, `Parser_with_names()`, `parse()` → `ParseResultProgram`, `parse_expr()`, `parse_stmt()`, `parse_pattern()`, `parse_match()`
- **Imports:** `core`, `vec`, `compiler::token`, `compiler::ast`, `compiler::lexer`
- **Imported by:** `compiler.fd`

### Parser conventions

- `ParseResultX::Ok(value)` / `ParseResultX::Err(str)` because Fides lacks `?`.
- Calls like `self.parse_expr()` must be matched explicitly every time.
- `peek_token()` and `peek()` inspect the current token.
- `advance()` consumes and advances.
- `match_tok(k)` consumes `k` if it is the current token and returns `true`.
- `expect(k, msg)` reports a parse error if the current token is not `k`.

### Known issues (session 11 analysis)

- **`box_expr_deref` out-of-sync with `ast.fd`** — references 8 non-existent
  `Expr` variants (`CharLit`, `Assign`, `Lambda`, `SizeOf`, `AlignOf`,
  `OffsetOf`, `Path`, `UnitLit`) and uses wrong arities for `ArrayLit` (1 arg
  vs 2 in ast.fd) and `Range` (2 args vs 3 in ast.fd). **P0 — will fail to
  compile.**
- **Missing `return` statements** — `parse_guard_stmt` (line 2521),
  `parse_use_stmt` (line 3024), `parse_match_expr` (line 4002), and
  `parse_comptime_expr` (line 3848) fall off the end without returning a
  `ParseResult*`. **P0.**
- **Duplicate `var has_forward`** — declared twice in `parse_layer`
  (lines 1637-1639) and `parse_model` (lines 1704-1706). **P0.**
- **`parse_use` version-pin always errors** — lines 4684-4695: even when a
  valid version string is found, the function returns `Err`. **P1.**
- **`parse_fstring_parts` is a stub** — line 4847: f-string interpolation is
  extracted as raw text, not parsed. **P2.**
- **Non-exhaustive `match` patterns** — several helper functions
  (`expect_ident`, `read_type_name`, `parse_type` extraction, `parse_pattern`
  extraction) may need `_ => {}` default arms. **P1.**

---

## `compiler/typechecker.fd` — Type, effect, region, capability, and safety check

- **Module:** `compiler::typechecker`
- **Purpose:** The largest semantic pass — inference, region/effect/capability checks, traits, generics, linear types, annotations (`@deterministic`, `@gas`, etc.).
- **Key types:** `TypeChecker`, `StringType`, `MethodInfo`, `TraitMethodInfo`, `FuncSig`, `ScopeEntry`, `LinearVar`, `RegionBudget`
- **Key functions:** `TypeChecker_register_program()`, `TypeChecker_check_bodies()`, `TypeChecker_compute_func_effects()`, `TypeChecker_verify_*` family
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

### Known issues (session 11 analysis)

- **13 empty safety verification stubs** — lines 1451-1505: `verify_conserve`,
  `verify_gas`, `verify_safety_annotations`, `verify_constant_time`,
  `verify_realtime`, `verify_differentiable`, `verify_quantized`,
  `verify_checkpoint`, `verify_dp`, `verify_shard`, `verify_adapter`,
  `verify_tainted`, `verify_contracts`, `verify_allocator_params` are all
  no-ops. Safety annotations are accepted but not enforced. **P2** (not needed
  for bootstrap, but dangerous for production use).
- **`type_size` returns 8 for most composites** — line 2317: coarse sizing
  for ref, raw-pointer, and many composite types regardless of actual layout.
- **`verify_deterministic` is partial** — line 1407: rejects IO/Net/Spawn/
  Diverge and floating-point ops but does not reject randomness or time.

---

## `compiler/borrowck.fd` — Borrow checker

- **Module:** `compiler::borrowck`
- **Purpose:** Separate pass after typechecking that catches `&mut` vs shared borrow conflicts and illegal mutations while borrows are active.
- **Key types:** `SharedBorrowEntry`, `MutBorrowEntry`, `BorrowOfEntry`, `ReservedMutBorrow`, `BorrowChecker`, `BorrowckMode`
- **Key functions:** `BorrowChecker_new()`, `BorrowChecker.check()`, `BorrowChecker.check_func()`, `pattern_bindings()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

---

## `compiler/ir.fd` — Typed SSA intermediate representation

- **Module:** `compiler::ir`
- **Purpose:** Core IR types: types, effects, regions, instructions, basic blocks, functions, and modules.
- **Key types:** `IrType`, `Effect`, `EffectSet`, `Region`, `Inst`, `BasicBlock`, `IrFunction`, `IrModule`, `ValueId`, `ValueIdType`, `StringIrType`
- **Key functions:** `ir_type_size()`, `ir_type_align()`, `ir_type_is_int()`, `ir_type_is_float()`, `ir_type_is_ptr()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `lower`, `codegen_c`, `pir`, `opt`, `compiler`

### `Opt*` pattern

```fides
pub enum OptIrType { Some(IrType), None }
pub enum OptValueId { Some(i64), None }
pub enum OptU64     { Some(i64), None }
pub enum OptString  { Some(str),  None }
pub enum OptInst    { Some(Inst), None }
pub enum OptValueMeta { Some(ValueMeta), None }
pub enum OptStructLayout { Some(StructLayout), None }
pub enum OptIrFunction { Some(IrFunction), None }
```

These are consumed with explicit `match X { Some(v) => ..., None => ... }`.
Fides has no `Option` combinators.

---

## `compiler/lower.fd` — AST → SSA IR lowering

- **Module:** `compiler::lower`
- **Purpose:** Lowers typed AST into Fides SSA IR (`IrModule`). Handles control flow, expressions, structs, enums, effects, defer, try, closures, regions.
- **Key types:** `Lowerer`, `StringStructDefEntry`, `StringEnumDefEntry`, `I64IrTypeEntry`, `OptFunc`, `OptStructDef`, `OptValueId`
- **Key functions:** `Lowerer_new()`, `Lowerer_lower_program()`, `lower()`, `Lowerer_compute_struct_layout()`, `Lowerer_compute_enum_layout()`, `Lowerer_lower_type()`, `Lowerer_lower_expr()`, `Lowerer_lower_match()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `compiler.fd`

### Key responsibilities

- Pre-register all function signatures so calls can look up return types before bodies are lowered.
- Pre-register impl methods and trait impl methods under mangled names (`StructName_method`, `TypeName_method`).
- Compute struct and enum layouts with iterative fixpoint for nested types.
- Implement the SRET (struct return via hidden out-parameter) convention for both callees and callers.
- Lower match expressions with tag checks and payload extraction.
- Track `current_sret`, `defer_stack`, `try_stack`, `region_stack`, `loop_stack`.

### Currently known issue

Enum variant extraction for struct-typed payloads uses a plain `Inst::Load`,
which only copies 8 bytes. Under the pointer-based enum payload approach
(COORDINATION.md [D1]), this is correct — each payload slot is an 8-byte
pointer. The old `load_enum_field()` workaround is no longer needed.

### Session 14 changes (Category B)

- **`f64_to_bits` / `f32_to_bits` implemented** — were stubs returning 0;
  now call `p_f64_to_bits` / `p_f32_to_bits` runtime functions. Used by
  `ConstFloat` codegen to preserve NaN/Inf/-0.0 bit patterns.
- **`Lowerer_resolve_type` recursively resolves generic type arguments** —
  `Type::Generic(name, args)` now resolves each arg via `Lowerer_resolve_type`
  before returning. This fixes nested generic type alias expansion.
- **`p_f64_to_bits` / `p_f32_to_bits` externs registered** in
  `Lowerer_register_runtime`.

### Known issues (session 11 analysis — line numbers approximate)

- **~~`f64_to_bits` always returns 0~~** — **FIXED (session 14)**.
- **`lower_type` narrows I128/U128 to I64/U64** — 128-bit integers silently
  truncated to 64-bit. **P1.**
- **`runtime_effects` is a hardcoded whitelist** — any `p_*` function not in
  the list returns `None` and is treated as pure, risking the optimizer
  deleting side-effecting calls. **P1.**
- **`Lowerer_infer_expr_ast_type` falls back to I64** — simplified inference
  returns `Type::I64` for most expression forms, making closure return type
  inference unreliable. **P2.**
- **`box_expr_unbox` enumerates out-of-sync variants** — references
  `Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`,
  `ComptimeGen`, `QuantCast`, `Await`, `Spawn` which do not exist in
  `ast.fd`. (Already noted in the warning above.)

---

## `compiler/codegen_c.fd` — C99 code generator

- **Module:** `compiler::codegen_c`
- **Purpose:** Translates `IrModule` into C99 source, relying on a C runtime for `p_vec_*`, `p_map_*`, `p_string_*`.
- **Key types:** `ValueNameEntry`, `StringConstEntry`, `FnParamTypesEntry`, `FnRetTypeEntry`, `CCodeGen`
- **Key functions:** `CCodeGen_new()`, `c_type()`, `CCodeGen_generate()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `compiler.fd`

### Key responsibilities

- Map `IrType` to C types. `IrType::Struct(_)` becomes `void*` in C.
- Detect aggregate returns (`Struct` / `Array`) and emit `void` C return types.
- At call sites, emit `fname(&sret, ...)` for aggregate returns and set the result pointer.
- Emit `Inst::Store` as `memcpy` for struct-typed values (this is essential for SRET copy correctness).
- Cast pointer args to `void*` / `int64_t` as needed.
- Sanitize Fides function names that collide with C keywords.

### Session 14 changes (Category B)

- **`ConstFloat` now preserves exact bit patterns** — emits union compound
  literals (`((union { uint64_t u; double f; }){ .u = <bits>ULL }).f`) instead
  of decimal casts. NaN/Inf/-0.0 are now bit-exact. Added `p_f64_to_bits` and
  `p_f32_to_bits` C runtime functions (decls + implementations).
- **Polymorphic print/println dispatch verified at parity** with Rust codegen
  (Vec/HashMap/Tensor/F64/Bool/Ptr/Generic/any/default cases all match).

### Known issues (session 11 analysis — line numbers approximate)

- **`DynCall` not implemented** — emits only a comment
  `// DynCall not implemented in C backend`, leaving the destination variable
  uninitialized. **P1.** (Category B2)
- **`TryUnwrap` not implemented** — emits only a comment, same
  issue. **P1.** (Category B2)
- **~~`ConstFloat` TODO~~** — **FIXED (session 14)**. Now uses union compound
  literals with `p_f64_to_bits`/`p_f32_to_bits` for exact bit preservation.

---

## `compiler/pe.fd` — PE emitter (native x86-64 + PE file writer)

- **Module:** `compiler::pe`
- **Purpose:** Self-contained x86-64 code generator + Windows PE file writer.
  Translates `IrModule` directly into machine code and emits a valid PE
  executable — no tcc, no gcc, no external linker needed.
- **Key types:** `X86Encoder` (instruction encoder), `PeCodeGen` (IR → codegen),
  `PeStackFrame` (stack frame management), `PeFuncEntry`, `PeImportEntry`
- **Key functions:** `X86Encoder_*` (instruction encoding), `PeCodeGen_new()`,
  `PeCodeGen_generate()` (main entry), `PeCodeGen_gen_print_str()`,
  `PeCodeGen_gen_print_int()`, `PeCodeGen_gen_panic()`,
  `PeCodeGen_emit_entry_point()`, `PeCodeGen_write_pe_file()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `main.fd` (via `build` command)
- **Ported from:** `src/pe.rs` (6,345 lines Rust → 4,845 lines Fides, session 5)

### Key responsibilities

- Encode x86-64 instructions: REX prefixes, ModRM/SIB bytes, immediates,
  rip-relative addressing for calls and string literals.
- Generate PE file format: DOS header, PE header, section table, .text
  section (code), .rdata section (string literals + import table), IAT.
- Import Windows API functions: `GetStdHandle`, `WriteFile`, `ExitProcess`.
- Generate runtime stubs: `p_print_str` (strlen + WriteFile to stdout),
  `p_print_int` (itoa + WriteFile), `p_print_float`, `p_print_bool`,
  `p_panic` (ExitProcess with error code).
- Manage stack frames: prologue (`push rbp; mov rbp, rsp; sub rsp, N`),
  epilogue (`mov rsp, rbp; pop rbp; ret`), local variable access via
  `[rbp - offset]`.
- Label resolution: function labels, jump labels (for control flow),
  call targets resolved via rip-relative addressing.

### Current capabilities (session 14)

| Feature | Status |
|---------|--------|
| PE header + section layout | ✅ |
| Import table (IAT) | ✅ |
| Entry point → main | ✅ |
| Function prologues/epilogues | ✅ |
| Stack frame management | ✅ |
| Print stubs (str/int/float/bool) | ✅ |
| Panic stub | ✅ |
| String literals (.rdata) | ✅ |
| Basic arithmetic | ✅ |
| Local variables | ✅ |
| Control flow (if/while/loop) | ✅ |
| String concat (p_str_concat) | ✅ |
| Int→string (p_to_str) | ✅ |
| Bool→string (p_to_str_bool) | ✅ |
| Float→string (p_to_str_float) | ✅ (simplified) |
| Vec runtime (new/push/len/get/pop/set/...) | ✅ |
| HashMap runtime (new/insert/get/contains/...) | ✅ |
| Full struct/enum support | ⏳ TODO |
| Float arithmetic (xmm ops) | ⏳ TODO |
| Many-arg calls (>4 args) | ⏳ TODO |
| Full compiler source | ⏳ TODO |

### Known issues

- **`p_file_write` truncates at null bytes** — binary PE data is hex-encoded
  and decoded via PowerShell.
- **Fides compiler statement-dropping bug** — long functions in `pe.fd` may
  lose statements. Worked around by splitting into smaller helpers.
- **`src/pe.rs` and `compiler/pe.fd` must stay in sync** — if one changes,
  the other needs the same change.

### Known issues (session 11 analysis)

- **20+ runtime stubs return wrong values** — `p_vec_insert`, `p_vec_remove`,
  `p_vec_reverse`, `p_vec_extend`, `p_vec_sort` all return 0 (`xor rax; ret`).
  `p_string_contains`, `p_string_starts_with`, `p_string_ends_with` return
  `true`. `p_string_split` returns empty Vec. `p_string_find`/`rfind` return
  0. `p_string_replace`/`to_uppercase`/`to_lowercase`/`trim`/`repeat` return
  input unchanged. `p_map_keys`/`values`/`clone` return empty. **P1** — any
  test using these via the PE backend will see wrong behavior.
- **Missing REX prefixes** — `X86Encoder` does not emit REX for all 8-bit/
  16-bit partial register operations. **P2.**

---

## `compiler/peephole.fd` — Peephole optimizer (Linux backend, NOT yet integrated)

- **Module:** `compiler::peephole`
- **Purpose:** 9 peephole optimization passes that run on L3 (post-regalloc)
  x86-64 instructions. Always enabled in the Rust `build` command.
- **Key types:** `PeepholeOptimizer`, `PeepInst`, `PeepOperand`
- **Key functions:** `PeepholeOptimizer_new()`, `PeepholeOptimizer_optimize()`,
  9 passes: redundant move elimination, zero idioms, dead store elimination,
  constant folding, branch folding, lea-to-add, strength reduction, address
  mode folding, jump threading
- **Imports:** `core`, `vec`, `compiler::pir` (for L3 types)
- **Ported from:** `src/peephole.rs` (903 lines Rust → 1,064 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`. Integration requires the full Linux backend
  (`elf64.fd`, `linker.fd`, `runtime_linux.fd`).

---

## `compiler/regalloc.fd` — Linear scan register allocator (Linux backend, NOT yet integrated)

- **Module:** `compiler::regalloc`
- **Purpose:** Converts virtual registers in L3 IR to physical x86-64
  registers. Handles spill slots and constant rematerialization
  (`Location::Remat`).
- **Key types:** `ModuleAllocation`, `FuncAllocation`, `LiveInterval`,
  `Location` (Reg/Spill/Remat), `PhysReg` constants
- **Key functions:** `allocate_module()`, `allocate_function()`,
  `linear_scan()`, `spill()`, `rematerialize()`
- **Imports:** `core`, `vec`, `compiler::pir` (for L3 types)
- **Ported from:** `src/regalloc.rs` (1,088 lines Rust → 1,181 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`.
- **Note:** `val` is a reserved keyword in Fides — all `val` variables in the
  Rust source were renamed to `value` during porting (see known-bugs.md §1.26).

---

## `compiler/codegen_x64.fd` — PIR L3 → x86-64 assembly (Linux backend, NOT yet integrated)

- **Module:** `compiler::codegen_x64`
- **Purpose:** Generates AT&T-syntax x86-64 assembly from PIR L3 modules.
  Uses the System V AMD64 ABI (RDI, RSI, RDX, RCX, R8, R9 for arguments).
  Output is suitable for `gcc -c` or `as`.
- **Key types:** `X64CodeGen`, `Operand` (Slot/Reg/Imm/Label/Const),
  `BlockLabelEntry`, `PanicStub`, `StringConstEntry`
- **Key functions:** `generate_asm()` (main entry), `X64CodeGen_gen_module()`,
  `X64CodeGen_gen_function()`, `X64CodeGen_gen_inst()`,
  `X64CodeGen_gen_call_abi()`, `X64CodeGen_gen_call_ptr()`,
  `X64CodeGen_gen_load()`, `X64CodeGen_gen_store()`
- **Imports:** `core`, `vec`, `string`, `compiler::pir`, `compiler::regalloc`
- **Ported from:** `src/codegen_x64.rs` (1,940 lines Rust → 1,942 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`. Integration requires `elf64.fd` (object file
  emitter) + `linker.fd` + `runtime_linux.fd`.
- **Note:** `x86.rs` (1377 lines) is dead code and was NOT ported — see
  known-bugs.md §1.29. The actual Windows backend is `pe.rs`/`pe.fd`, which
  is self-contained and does not use this file.

---

## `compiler/lsp.fd` — LSP server (JSON-RPC over stdio)

- **Module:** `compiler::lsp`
- **Purpose:** Language Server Protocol implementation for editor integration.
  Reads JSON-RPC 2.0 messages from stdin (Content-Length framing), dispatches
  to handlers, writes responses/notifications to stdout.
- **Key types:** `DocEntry` (uri + text), `OptI64Pair` (definition search
  result), `I64Triple` (occurrence line/col/length)
- **Key functions:** `lsp_run()` (main message loop), `handle_initialize()`,
  `handle_completion()`, `handle_hover()`, `handle_definition()`,
  `handle_rename()`, `publish_diagnostics()`, `send_notification()`,
  `send_response()`, `send_error()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::json`,
  `compiler::compiler`
- **Ported from:** `src/lsp.rs` (916 lines Rust → 1,082 lines Fides, session 9)
- **Runtime dependencies (added to `codegen_c.fd`):** `p_read_line`,
  `p_read_stdin(n)`, `p_stdin_eof`, `p_flush_stdout`
- **Status:** Module parses correctly. `lsp` command wired into
  `main.fd`. Full self-hosted build works (session 15: bootstrap fixpoint achieved).
- **Features:**
  - `initialize` / `shutdown` / `exit` lifecycle
  - `textDocument/didOpen` / `didChange` / `didClose` — document tracking
  - `textDocument/publishDiagnostics` — via `get_diagnostics()` from `compiler.fd`
  - `textDocument/completion` — keywords + functions + structs + enums +
    consts + type aliases + traits + dot-access methods/fields
  - `textDocument/hover` — function/struct/enum/const/type/trait/method signatures
  - `textDocument/definition` — source text search for definition location
  - `textDocument/rename` — whole-word occurrence search + WorkspaceEdit

---

## `compiler/pir.fd` — Multi-level PIR (L1→L2→L3)

- **Module:** `compiler::pir`
- **Purpose:** Lowers IR through PIR levels: L1 (named regions/effects/caps), L2 (SSA mid-level), L3 (virtual regs / stack slots).
- **Key types:** `CapSet`, `RegionRef`, `Region`, `L1Block`/`L1Function`/`L1Module`, `L2*` and `L3*` levels
- **Key functions:** `CapSet_new()`, `L1Function_new()`, `L2Function_new()`, `region_ref_eq()`, `region_eq()`, `l1inst_is_terminator()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `opt.fd`

### Known issues (session 11 analysis)

- **`str_substring` is a stub** — line 2681: returns the original string `s`,
  ignoring `start` and `len` parameters. Semantically wrong if any caller
  uses it. **P1.**

---

## `compiler/opt.fd` — PIR L2 optimizer

- **Module:** `compiler::opt`
- **Purpose:** Core optimizer: pass enum, `optimize_l2` driver, and shared helpers; detailed passes live in `opt_*.fd` siblings.
- **Key types:** `Pass`, `OptStats`
- **Key functions:** `Pass_name()`, `Pass_all()`, `optimize_l2()`, `pass_const_fold()`, `pass_cse()`, `pass_dce()`, `pass_gvn()`, `is_pure()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`, `compiler::pir`
- **Imported by:** part of the broader build pipeline

### Known issues (session 11 analysis)

- **`is_lto_mode()` hardcoded false** — line 221: always returns `false`,
  LTO detection is broken (LTO is inferred from `max_iters` in the driver
  instead).
- **`cse_key` unreachable duplicate arms** — lines 1649-1660: duplicate
  `ConstInt`/`ConstFloat`/`ConstStr`/`ConstBool` arms after a catch-all `_`
  are dead code.
- **`effect_reorder_block` changed detection bug** — line 5926: compares
  `insts.get(pi2)` vs `pure_insts.get(pi2)` but `pi2` iterates over
  `pure_insts`, not `insts`, making the position check meaningless.

---

## `compiler/mono.fd` — Monomorphization

- **Module:** `compiler::mono`
- **Purpose:** Clones generic functions for concrete type instantiations, resolves const generics, handles trait dispatch.
- **Key types:** `OptType`, `OptStr`, `SubstEntry`, `AssocBindingEntry`, `TraitAssocEntry`, `GenericFuncEntry`, `GenericMethodEntry`
- **Key functions:** `subst_get()`, `subst_set()`, `generic_funcs_get()`, `trait_assoc_get()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

### Known issues (session 11 analysis)

- **`subst_gat_type_args` always uses args.get(0)** — line 649: for
  single-character uppercase generic parameters, always uses the first type
  argument regardless of parameter index. Mis-substitutes any generic with
  more than one type parameter. **P1.**
- **`resolve_assoc_types` falls back to I64** — lines 711, 751, 789:
  unresolved associated types silently become `Type::I64` instead of
  producing an error. **P2.**
- **`infer_expr_type` is crude** — line 2565: returns `Type::I64` for many
  unhandled expression forms. **P2.**

---

## `compiler/fmt.fd` — Source formatter

- **Module:** `compiler::fmt`
- **Purpose:** Token-based pretty-printer for `.fd` files; preserves comments and normalizes spacing/indentation.
- **Key types:** `FTok`
- **Key functions:** `fmt_tokenize()`, `ftok_text()`, `format_source()`, `is_formatted()`
- **Imports:** `core`, `vec`, `string`
- **Notes:** Used via `fidesc fmt` CLI, not imported by the compiler pipeline.

---

## `compiler/diag.fd` — Diagnostic / error reporting

- **Module:** `compiler::diag`
- **Purpose:** Structured errors/warnings with severity, source context, color, and error codes.
- **Key types:** `Severity`, `ErrCode`, `Diag`
- **Key functions:** `Severity.label()`, `color_code()`, `ErrCode_*()` constructors
- **Imports:** `vec`
- **Notes:** The active compiler pipeline mostly uses `StrTriple` / string errors, but `diag` is the eventual replacement.

---

## `compiler/json.fd` — JSON parser/serializer

- **Module:** `compiler::json`
- **Purpose:** Self-contained JSON handling; used for LSP/config/dumps.
- **Key types:** `Json`, `JsonResult`, `JsonObject`
- **Key functions:** `json_parse()`, `json_dump()`, `json_escape_string()`
- **Imports:** `core`, `vec`, `string`
- **Notes:** Leaf utility module.

---

## `compiler/inc.fd` — Incremental compilation cache

- **Module:** `compiler::inc`
- **Purpose:** Source-hash-based incremental compilation cache. Stores
  `IrModule` results keyed by source path + hash to avoid recompiling
  unchanged modules.
- **Key types:** `IncCacheEntry`, `IncrementalCache`, `OptIrModule`
- **Key functions:** `hash_source()`, `hash_str()` (FNV-1a 64-bit),
  `IncrementalCache_new()`, `IncrementalCache_get()`,
  `IncrementalCache_store()`, `global_cache_store()`, `global_cache_get()`,
  `global_cache_clear()`
- **Imports:** `core`, `vec`, `string`, `compiler::ir`
- **Imported by:** `compiler.fd`

### Known issues (session 11 analysis)

- **`global_cache_*` are all no-ops** — lines 134-145:
  `global_cache_store` does nothing, `global_cache_get` always returns
  `OptIrModule::None`, `global_cache_clear` does nothing. The file comments
  say these will be replaced "when Fides supports module-level mutable
  state." **Now fixable** — the `var` feature was implemented in the Rust
  compiler in session 11. The Fides `inc.fd` can now use `var` to implement
  a real global cache. **P1.**

---

## `compiler/comptime_eval.fd` — Compile-time evaluation engine

- **Module:** `compiler::comptime_eval`
- **Purpose:** Full comptime evaluation engine — evaluates `comptime <expr>`,
  `comptime gen { }` blocks, aggressive comptime (I-5A), and loop unrolling
  (I-5D). Injects generated functions/structs into the program and substitutes
  comptime expressions with their evaluated values.
- **Key functions:**
  - `process_gen()` — evaluates `comptime gen` blocks, parses result as Fides
    source, injects generated functions/structs
  - `substitute()` — recursively substitutes `comptime` expressions with
    evaluated values (8 iterations for fixpoint)
  - `evaluate()` / `evaluate_with_env()` / `evaluate_block()` — full AST
    interpreter (binary ops, unary ops, calls, if/else, blocks, for loops,
    string concat, f-strings, identifiers, comptime functions)
  - `evaluate_reflect()` — reflection: `TypeName`, `IsStruct`, `IsEnum`,
    `FieldCount`, `FieldNames`, `FieldTypes`, `EnumVariants`
  - `box_expr_unbox()` / `box_expr_new()` — boxing helpers (`box_expr_unbox`
    is centralized in `ast.fd` per [D10]; `box_expr_new` is local)
- **ComptimeEnv:** Uses parallel Vecs (no HashMap at compile time)
- **I-5A:** Auto-evaluates calls to `comptime fn` functions when all
  arguments are literals (no explicit `comptime` wrapper needed)
- **I-5D:** Unrolls `for` loops with literal bounds (≤16 iterations, ≤16
  body stmts) at AST level; eliminates dead `if` branches with literal
  conditions
- **Imports:** `core`, `vec`, `compiler::ast`
- **Imported by:** `compiler.fd`

### Known issues (session 11 analysis)

- **Return re-wrap omissions** — `substitute_expr` and `replace_ident_in_*`
  skip `Stmt::Return` and `OptBoxExpr` re-wrapping (comments say "Can't
  re-wrap into Box easily; skip for now"). Comptime expressions inside
  `return` statements are not substituted or evaluated. **P1.**
- **`aggressive_comptime_expr` limited recursion** — only recurses into
  `Binary`, `Unary`, `Call`, `MethodCall`; other expressions (If, Match,
  ArrayLit, StructLit) are not traversed for nested comptime calls. **P2.**
- **`evaluate` falls through to `IntLit(0)`** — line 1077: unsupported
  expression forms silently become 0. **P2.**
- **`box_expr_unbox` defaults to `IntLit(0)`** — line 64: unhandled `Box<Expr>`
  variants silently become zero. **P2.**

---

## `compiler/flat_ast.fd` — Flat AST serialization

- **Module:** `compiler::flat_ast`
- **Purpose:** Produces a line-based serialization of the AST for
  `dump-ast-flat` command and self-host parser differential testing.
  Format: 2-space indent per depth, node type first, then fields
  (strings double-quoted, numbers bare, bools as true/false).
- **Key functions:**
  - `program_flat(p: Program) -> str` — main entry point, serializes entire program
  - `func_flat(f: Func, depth) -> str` — serializes a function with params, body
  - `expr_flat(e: Expr, depth) -> str` — serializes all 39 Expr variants
  - `stmt_flat(s: Stmt, depth) -> str` — serializes all 28 Stmt variants
  - `type_flat(t: Type, depth) -> str` — serializes all 29 Type variants
  - `pattern_flat(p: Pattern, depth) -> str` — serializes all 7 Pattern variants
  - `esc(s: str) -> str` — escapes strings for output (\\n, \\r, \\t, \\", \\\\, \\u{XX})
- **Box<T> handling:** Uses `box_expr_unbox()`, `box_type_unbox()`,
  `box_pattern_unbox()` from `ast.fd` to unbox `Box<Expr>`/`Box<Type>`/
  `Box<Pattern>` fields for recursive calls (Fides has no `*` deref for `Box<T>`)
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** Not yet integrated into `compiler.fd` (standalone module)
- **Ported from:** `src/flat_ast.rs` (741 LOC Rust → 1,259 LOC Fides)

---

## `compiler/borrowck` (no `.fd` extension)

This is a compiled artifact / placeholder in `compiler/`. The actual borrow
checker is `compiler/borrowck.fd`.

## `compiler/codegen_c` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/codegen_c.fd`.

## `compiler/lower` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/lower.fd`.
