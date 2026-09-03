# Fides Self-Hosted Compiler — Architecture

> File-by-file breakdown of the self-hosted compiler modules in `compiler/`.

## The Pipeline (Simplified)

```
main.fd
  └─ compiler.fd
       ├─ lexer.fd  ──► parser.fd  ──► ast.fd
       ├─ typechecker.fd ──► ast.fd
       ├─ borrowck.fd ──► ast.fd
       ├─ comptime_eval.fd ──► ast.fd
       ├─ lower.fd ──► ir.fd
       ├─ mono.fd ──► ast.fd
       ├─ pir.fd / opt.fd ──► ir.fd
       └─ codegen_c.fd ──► ir.fd  ──► C source
```

`compiler.fd` is the central hub. `ast.fd`, `ir.fd`, and `token.fd` are the
shared data-definition leaves that almost everything else consumes.

---

## `compiler/main.fd` — CLI entry point

- **Module:** `compiler::main`
- **Purpose:** Minimal command-line front-end. Currently supports `ccompile` for the C-codegen bootstrap.
- **Key functions:** `main()` dispatches commands; `cmd_ccompile()` parses CLI args and invokes the compiler pipeline.
- **Imports:** `core`, `vec`, `string`, `compiler::compiler`
- **Notes:** Top-level executable; no other compiler module imports it.

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

---

## `compiler/typechecker.fd` — Type, effect, region, capability, and safety check

- **Module:** `compiler::typechecker`
- **Purpose:** The largest semantic pass — inference, region/effect/capability checks, traits, generics, linear types, annotations (`@deterministic`, `@gas`, etc.).
- **Key types:** `TypeChecker`, `StringType`, `MethodInfo`, `TraitMethodInfo`, `FuncSig`, `ScopeEntry`, `LinearVar`, `RegionBudget`
- **Key functions:** `TypeChecker_register_program()`, `TypeChecker_check_bodies()`, `TypeChecker_compute_func_effects()`, `TypeChecker_verify_*` family
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

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
which only copies 8 bytes. This truncates the payload. The Rust lowerer has
`load_enum_field()` (buffer + `Store` which emits `memcpy`) to fix this. The
Fides lowerer needs the same.

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

---

## `compiler/pir.fd` — Multi-level PIR (L1→L2→L3)

- **Module:** `compiler::pir`
- **Purpose:** Lowers IR through PIR levels: L1 (named regions/effects/caps), L2 (SSA mid-level), L3 (virtual regs / stack slots).
- **Key types:** `CapSet`, `RegionRef`, `Region`, `L1Block`/`L1Function`/`L1Module`, `L2*` and `L3*` levels
- **Key functions:** `CapSet_new()`, `L1Function_new()`, `L2Function_new()`, `region_ref_eq()`, `region_eq()`, `l1inst_is_terminator()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `opt.fd`

---

## `compiler/opt.fd` — PIR L2 optimizer

- **Module:** `compiler::opt`
- **Purpose:** Core optimizer: pass enum, `optimize_l2` driver, and shared helpers; detailed passes live in `opt_*.fd` siblings.
- **Key types:** `Pass`, `OptStats`
- **Key functions:** `Pass_name()`, `Pass_all()`, `optimize_l2()`, `pass_const_fold()`, `pass_cse()`, `pass_dce()`, `pass_gvn()`, `is_pure()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`, `compiler::pir`
- **Imported by:** part of the broader build pipeline

---

## `compiler/mono.fd` — Monomorphization

- **Module:** `compiler::mono`
- **Purpose:** Clones generic functions for concrete type instantiations, resolves const generics, handles trait dispatch.
- **Key types:** `OptType`, `OptStr`, `SubstEntry`, `AssocBindingEntry`, `TraitAssocEntry`, `GenericFuncEntry`, `GenericMethodEntry`
- **Key functions:** `subst_get()`, `subst_set()`, `generic_funcs_get()`, `trait_assoc_get()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

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

## `compiler/comptime_eval.fd` — Compile-time evaluation stubs

- **Module:** `compiler::comptime_eval`
- **Purpose:** Placeholder for `comptime <expr>` / `comptime gen { }` evaluation.
- **Key functions:** `process_gen()`, `substitute()` — both currently pass through unchanged.
- **Imports:** `core`, `vec`, `compiler::ast`
- **Imported by:** `compiler.fd`

---

## `compiler/borrowck` (no `.fd` extension)

This is a compiled artifact / placeholder in `compiler/`. The actual borrow
checker is `compiler/borrowck.fd`.

## `compiler/codegen_c` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/codegen_c.fd`.

## `compiler/lower` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/lower.fd`.
