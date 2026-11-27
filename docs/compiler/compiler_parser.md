[All modules](index.md)

# compiler::parser -- Fides Documentation

## Contents

- [ParseResultExpr](#ParseResultExpr) (enum)
- [ParseResultExprs](#ParseResultExprs) (enum)
- [ParseResultStmt](#ParseResultStmt) (enum)
- [ParseResultStmts](#ParseResultStmts) (enum)
- [ParseResultType](#ParseResultType) (enum)
- [PatNest](#PatNest) (struct)
- [ParseResultPattern](#ParseResultPattern) (enum)
- [ParseResultStr](#ParseResultStr) (enum)
- [ParseResultFunc](#ParseResultFunc) (enum)
- [ParseResultStructDef](#ParseResultStructDef) (enum)
- [ParseResultEnumDef](#ParseResultEnumDef) (enum)
- [ParseResultConstDef](#ParseResultConstDef) (enum)
- [ParseResultGlobalVar](#ParseResultGlobalVar) (enum)
- [ParseResultTypeAlias](#ParseResultTypeAlias) (enum)
- [ParseResultTraitDef](#ParseResultTraitDef) (enum)
- [ParseResultImportDef](#ParseResultImportDef) (enum)
- [ParseResultAttribute](#ParseResultAttribute) (enum)
- [ParseResultAnnotation](#ParseResultAnnotation) (enum)
- [ParseResultParams](#ParseResultParams) (enum)
- [ParseResultLayerDef](#ParseResultLayerDef) (enum)
- [ParseResultModelDef](#ParseResultModelDef) (enum)
- [ParseResultComponentDef](#ParseResultComponentDef) (enum)
- [ParseResultSystemDef](#ParseResultSystemDef) (enum)
- [ParseResultStateMachineDef](#ParseResultStateMachineDef) (enum)
- [ParseResultEventHandlerDef](#ParseResultEventHandlerDef) (enum)
- [ParseResultExternDecl](#ParseResultExternDecl) (enum)
- [ParseResultMatchArms](#ParseResultMatchArms) (enum)
- [ParseResultProgram](#ParseResultProgram) (enum)
- [ParseResultUnit](#ParseResultUnit) (enum)
- [ImplOrTrait](#ImplOrTrait) (struct)
- [ParseResultImplOrTrait](#ParseResultImplOrTrait) (enum)
- [IfExpr](#IfExpr) (struct)
- [OpEntry](#OpEntry) (enum)
- [at_import_flags](#at_import_flags) (function)
- [turbofish_all](#turbofish_all) (function)
- [turbofish_types](#turbofish_types) (function)
- [str_vec_has](#str_vec_has) (function)
- [path_last](#path_last) (function)
- [take_qualified_calls](#take_qualified_calls) (function)
- [parser_import_add](#parser_import_add) (function)
- [parser_pcall_count](#parser_pcall_count) (function)
- [parser_qualified_calls](#parser_qualified_calls) (function)
- [parser_import_known](#parser_import_known) (function)
- [parser_import_names](#parser_import_names) (function)
- [ParseSide](#ParseSide) (struct)
- [ParseSide_empty](#ParseSide_empty) (function)
- [Parser_side](#Parser_side) (function)
- [parser_side_of](#parser_side_of) (function)
- [parser_side_free](#parser_side_free) (function)
- [parser_side_fits](#parser_side_fits) (function)
- [parser_side_apply](#parser_side_apply) (function)
- [Parser](#Parser) (struct)
- [pd_parser_make](#pd_parser_make) (function)
- [Parser_new](#Parser_new) (function)
- [Parser](#Parser) (impl)
- [Parser_with_names](#Parser_with_names) (function)
- [vec_str_join](#vec_str_join) (function)
- [pd_size_type_tok](#pd_size_type_tok) (function)
- [is_int_type_keyword](#is_int_type_keyword) (function)
- [pd_expected_name_err](#pd_expected_name_err) (function)
- [type_keyword_name](#type_keyword_name) (function)
- [starts_type](#starts_type) (function)
- [ParseResultIfExpr](#ParseResultIfExpr) (enum)
- [BinOpInfo](#BinOpInfo) (struct)
- [BinOpInfo_new](#BinOpInfo_new) (function)
- [reduce_one_op](#reduce_one_op) (function)
- [parser_saw_secret](#parser_saw_secret) (function)
- [parser_saw_splice](#parser_saw_splice) (function)
- [parse_expression_source](#parse_expression_source) (function)
- [pd_impl_narrow_self](#pd_impl_narrow_self) (function)
- [pd_impl_is_narrowed](#pd_impl_is_narrowed) (function)
- [pd_cross_module_specializations](#pd_cross_module_specializations) (function)
- [parser_sequential_vars](#parser_sequential_vars) (function)
- [PreParse](#PreParse) (struct)
- [pp_is_word](#pp_is_word) (function)
- [pp_text_words](#pp_text_words) (function)
- [pp_bits_of](#pp_bits_of) (function)
- [pp_parse_one](#pp_parse_one) (function)
- [pp_parse_worker](#pp_parse_worker) (function)
- [pp_parse_par](#pp_parse_par) (function)
- [PpPlan](#PpPlan) (struct)
- [pp_predict](#pp_predict) (function)
- [pp_begin](#pp_begin) (function)
- [pp_end](#pp_end) (function)
- [pp_count](#pp_count) (function)
- [pp_hits](#pp_hits) (function)
- [pp_misses](#pp_misses) (function)
- [pp_result](#pp_result) (function)
- [pp_valid](#pp_valid) (function)
- [pp_use](#pp_use) (function)

## ParseResultExpr

```fides
pub enum ParseResultExpr {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 37*

---

## ParseResultExprs

```fides
pub enum ParseResultExprs {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 42*

---

## ParseResultStmt

```fides
pub enum ParseResultStmt {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 47*

---

## ParseResultStmts

```fides
pub enum ParseResultStmts {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 52*

---

## ParseResultType

```fides
pub enum ParseResultType {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 57*

---

## PatNest

LG8: a synthetic binding for a nested pattern position. `Ok(x)` in
`Some(Ok(x))` records ("__np0", Ok(x)-pattern); the match arm's
desugar turns it into an inner match on __np0.

```fides
pub struct PatNest {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `pat` | `Pattern` |

*Defined at line 65*

---

## ParseResultPattern

```fides
pub enum ParseResultPattern {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 70*

---

## ParseResultStr

```fides
pub enum ParseResultStr {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 75*

---

## ParseResultFunc

```fides
pub enum ParseResultFunc {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 80*

---

## ParseResultStructDef

```fides
pub enum ParseResultStructDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 85*

---

## ParseResultEnumDef

```fides
pub enum ParseResultEnumDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 90*

---

## ParseResultConstDef

```fides
pub enum ParseResultConstDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 95*

---

## ParseResultGlobalVar

```fides
pub enum ParseResultGlobalVar {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 100*

---

## ParseResultTypeAlias

```fides
pub enum ParseResultTypeAlias {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 105*

---

## ParseResultTraitDef

```fides
pub enum ParseResultTraitDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 110*

---

## ParseResultImportDef

```fides
pub enum ParseResultImportDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 115*

---

## ParseResultAttribute

```fides
pub enum ParseResultAttribute {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 120*

---

## ParseResultAnnotation

```fides
pub enum ParseResultAnnotation {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 125*

---

## ParseResultParams

```fides
pub enum ParseResultParams {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 130*

---

## ParseResultLayerDef

```fides
pub enum ParseResultLayerDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 135*

---

## ParseResultModelDef

```fides
pub enum ParseResultModelDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 140*

---

## ParseResultComponentDef

```fides
pub enum ParseResultComponentDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 145*

---

## ParseResultSystemDef

```fides
pub enum ParseResultSystemDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 150*

---

## ParseResultStateMachineDef

```fides
pub enum ParseResultStateMachineDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 155*

---

## ParseResultEventHandlerDef

```fides
pub enum ParseResultEventHandlerDef {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 160*

---

## ParseResultExternDecl

```fides
pub enum ParseResultExternDecl {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 165*

---

## ParseResultMatchArms

```fides
pub enum ParseResultMatchArms {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 170*

---

## ParseResultProgram

```fides
pub enum ParseResultProgram {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 175*

---

## ParseResultUnit

```fides
pub enum ParseResultUnit {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 180*

---

## ImplOrTrait

```fides
pub struct ImplOrTrait {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `i64` |
| `struct_impl` | `ImplDef` |
| `trait_impl` | `TraitImpl` |

*Defined at line 187*

---

## ParseResultImplOrTrait

```fides
pub enum ParseResultImplOrTrait {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 194*

---

## IfExpr

```fides
pub struct IfExpr {
```

**Fields:**

| Name | Type |
|------|------|
| `cond` | `Expr` |
| `then_block` | `Vec<Stmt>` |
| `else_block` | `OptStmts` |

*Defined at line 200*

---

## OpEntry

```fides
pub enum OpEntry {
```

**Variants:**

- `Op`
- `Paren`

*Defined at line 207*

---

## at_import_flags

```fides
pub fn at_import_flags(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 225*

---

## turbofish_all

Every turbofish call's type arguments, by number.

```fides
pub fn turbofish_all() -> Vec<Type> {
```

**Returns:** `Vec<Type>`

*Defined at line 239*

---

## turbofish_types

The type arguments of the turbofish call numbered `k`.

```fides
pub fn turbofish_types(k: i64) -> Vec<Type> {
```

**Parameters:**

- `k`: `i64`

**Returns:** `Vec<Type>`

*Defined at line 244*

---

## str_vec_has

```fides
pub fn str_vec_has(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 259*

---

## path_last

The last `::` segment of a path.

```fides
pub fn path_last(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 271*

---

## take_qualified_calls

The `module::name` calls parsed since the last time this was called.

```fides
pub fn take_qualified_calls() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 283*

---

## parser_import_add

Put `name` in the import set (parse_use does, for `use a::name;`).

```fides
pub fn parser_import_add(name: str) {
```

**Parameters:**

- `name`: `str`

*Defined at line 299*

---

## parser_pcall_count

How many `__pcall_k` locals have been numbered.

```fides
pub fn parser_pcall_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 307*

---

## parser_qualified_calls

The `module::name` calls parsed since take_qualified_calls last took
them (still there).

```fides
pub fn parser_qualified_calls() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 313*

---

## parser_import_known

Whether `name` is in the import set.

```fides
pub fn parser_import_known(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 318*

---

## parser_import_names

The names in the import set, in the order they went in.

```fides
pub fn parser_import_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 323*

---

## ParseSide

What a parse on a worker thread changed besides its Program.

```fides
pub struct ParseSide {
```

**Fields:**

| Name | Type |
|------|------|
| `import_adds` | `Vec<str>` |
| `qcalls` | `Vec<str>` |
| `at_paths` | `Vec<str>` |
| `at_flags` | `Vec<i64>` |
| `turbofish` | `Vec<Type>` |
| `pcalls` | `i64` |
| `secret` | `bool` |
| `splice` | `bool` |

*Defined at line 328*

---

## ParseSide_empty

```fides
pub fn ParseSide_empty() -> ParseSide {
```

**Returns:** `ParseSide`

*Defined at line 344*

---

## Parser_side

A Parser for a worker thread: as Parser_with_names, with `imports` the
import set it starts from (a map of its own), a keyword cache of its own
for the f-strings it lexes, and every change besides its Program kept
in it. parser_side_free frees its maps.

```fides
pub fn Parser_side(tokens: Vec<Token>, struct_names: Vec<str>, enum_names: Vec<str>, imports: Vec<str>) -> Parser {
```

**Parameters:**

- `tokens`: `Vec<Token>`
- `struct_names`: `Vec<str>`
- `enum_names`: `Vec<str>`
- `imports`: `Vec<str>`

**Returns:** `Parser`

*Defined at line 354*

---

## parser_side_of

What the parse of `p` (a Parser_side) changed besides its Program.

```fides
pub fn parser_side_of(p: Parser) -> ParseSide {
```

**Parameters:**

- `p`: `Parser`

**Returns:** `ParseSide`

*Defined at line 368*

---

## parser_side_free

Free the maps of a Parser_side (after parser_side_of).

```fides
pub fn parser_side_free(p: &mut Parser) {
```

**Parameters:**

- `p`: `&mut Parser`

*Defined at line 375*

---

## parser_side_fits

Whether a parse that numbered from 0 gave the numbers a parse here
would: it numbered no turbofish call (no `__pcall_k` local), or none has
been numbered here yet.

```fides
pub fn parser_side_fits(sd: ParseSide) -> bool {
```

**Parameters:**

- `sd`: `ParseSide`

**Returns:** `bool`

*Defined at line 389*

---

## parser_side_apply

Leave the globals as the parse `sd` came from would have, run here.

```fides
pub fn parser_side_apply(sd: ParseSide) {
```

**Parameters:**

- `sd`: `ParseSide`

*Defined at line 400*

---

## Parser

```fides
pub struct Parser {
```

**Fields:**

| Name | Type |
|------|------|
| `tokens` | `Vec<Token>` |
| `pos` | `i64` |
| `struct_names` | `Vec<str>` |
| `enum_names` | `Vec<str>` |
| `struct_name_set` | `i64` |
| `enum_name_set` | `i64` |
| `alias_set` | `i64` |
| `errors` | `Vec<str>` |
| `expr_depth` | `i64` |
| `max_expr_depth` | `i64` |
| `nest_depth` | `i64` |
| `nest_hit` | `bool` |
| `lt_seen` | `Vec<str>` |
| `p_ct_type_params` | `Vec<str>` |
| `g_sequential_type` | `bool` |
| `pd_last_impl_self` | `Vec<Type>` |
| `pd_impl_selfs` | `Vec<Type>` |
| `pd_last_timpl_self` | `Vec<Type>` |
| `pd_timpl_selfs` | `Vec<Type>` |
| `pd_impl_lts` | `Vec<str>` |
| `own_side` | `bool` |
| `own_kw` | `i64` |
| `side_imports` | `i64` |
| `side_import_adds` | `Vec<str>` |
| `side_qcalls` | `Vec<str>` |
| `side_at_paths` | `Vec<str>` |
| `side_at_flags` | `Vec<i64>` |
| `side_turbofish` | `Vec<Type>` |
| `side_pcalls` | `i64` |
| `side_secret` | `bool` |
| `side_splice` | `bool` |

*Defined at line 434*

---

## pd_parser_make

SP9: a Parser over `tokens` knowing these struct and enum names (sn_set
and en_set: the same names as HashMaps), every other field as a new
parse starts. Parser_new, Parser::new and Parser_with_names all make
theirs here.

```fides
pub fn pd_parser_make(tokens: Vec<Token>, sn: Vec<str>, en: Vec<str>, sn_set: i64, en_set: i64) -> Parser {
```

**Parameters:**

- `tokens`: `Vec<Token>`
- `sn`: `Vec<str>`
- `en`: `Vec<str>`
- `sn_set`: `i64`
- `en_set`: `i64`

**Returns:** `Parser`

*Defined at line 501*

---

## Parser_new

```fides
pub fn Parser_new(tokens: Vec<Token>) -> Parser {
```

**Parameters:**

- `tokens`: `Vec<Token>`

**Returns:** `Parser`

*Defined at line 509*

---

## Parser

```fides
impl Parser {
```

**Methods:**

### new

```fides
pub fn new(tokens: Vec<Token>) -> Parser {
```

### peek

```fides
fn peek(self: &Parser) -> TokenKind {
```

### peek_token

```fides
fn peek_token(self: &Parser) -> Token {
```

### advance

```fides
fn advance(self: &mut Parser) -> Token {
```

### is_at_end

```fides
fn is_at_end(self: &Parser) -> bool {
```

### check

```fides
fn check(self: &Parser, kind: TokenKind) -> bool {
```

### match_tok

```fides
fn match_tok(self: &mut Parser, kind: TokenKind) -> bool {
```

### recover_to_sync_token

```fides
fn recover_to_sync_token(self: &mut Parser) {
```

### split_gt

```fides
fn split_gt(self: &mut Parser) -> bool {
```

### expect

```fides
fn expect(self: &mut Parser, kind: TokenKind, msg: str) -> ParseResultUnit {
```

### expect_ident

```fides
fn expect_ident(self: &mut Parser) -> ParseResultStr {
```

### expect_ident_or_keyword

```fides
fn expect_ident_or_keyword(self: &mut Parser) -> ParseResultStr {
```

### is_struct_name

```fides
fn is_struct_name(self: &Parser, name: str) -> bool {
```

### is_enum_name

```fides
fn is_enum_name(self: &Parser, name: str) -> bool {
```

### pd_import_known

Whether `name` is a module imported so far (`use` names it).

```fides
fn pd_import_known(self: &Parser, name: str) -> bool {
```

### pd_import_add

```fides
fn pd_import_add(self: &mut Parser, name: str) {
```

### pd_qcall

```fides
fn pd_qcall(self: &mut Parser, qcall: str) {
```

### pd_note_at_import

```fides
fn pd_note_at_import(self: &mut Parser, path: str, flags: i64) {
```

### pd_turbofish

The number of the turbofish call whose type arguments are `t`.

```fides
fn pd_turbofish(self: &mut Parser, t: Type) -> i64 {
```

### pd_next_pcall

The number of the next `__pcall_k` local.

```fides
fn pd_next_pcall(self: &mut Parser) -> i64 {
```

### pd_saw_secret

```fides
fn pd_saw_secret(self: &mut Parser) {
```

### pd_saw_splice

```fides
fn pd_saw_splice(self: &mut Parser) {
```

### read_type_name

```fides
fn read_type_name(self: &mut Parser) -> ParseResultStr {
```

### parse

```fides
pub fn parse(self: &mut Parser) -> ParseResultProgram {
```

### parse_top_level_item

```fides
fn parse_top_level_item(self: &mut Parser, program: &mut Program, is_pub: bool) -> ParseResultUnit {
```

### parse_extern_block

```fides
fn parse_extern_block(self: &mut Parser, program: &mut Program, _is_pub: bool) -> ParseResultUnit {
```

### parse_struct

```fides
fn parse_struct(self: &mut Parser) -> ParseResultStructDef {
```

### parse_enum

```fides
fn parse_enum(self: &mut Parser) -> ParseResultEnumDef {
```

### parse_const

```fides
fn parse_const(self: &mut Parser) -> ParseResultConstDef {
```

### parse_global_var

```fides
fn parse_global_var(self: &mut Parser) -> ParseResultGlobalVar {
```

### parse_type_alias

```fides
fn parse_type_alias(self: &mut Parser) -> ParseResultTypeAlias {
```

### parse_impl_or_trait_impl

```fides
fn parse_impl_or_trait_impl(self: &mut Parser) -> ParseResultImplOrTrait {
```

### parse_trait

```fides
fn parse_trait(self: &mut Parser) -> ParseResultTraitDef {
```

### parse_extern_func_decl

```fides
fn parse_extern_func_decl(self: &mut Parser, abi: str, include: OptStr, link_lib: OptStr) -> ParseResultExternDecl {
```

### parse_func

```fides
fn parse_func(self: &mut Parser) -> ParseResultFunc {
```

### parse_params

```fides
fn parse_params(self: &mut Parser) -> ParseResultParams {
```

### parse_layer

```fides
fn parse_layer(self: &mut Parser) -> ParseResultLayerDef {
```

### parse_model

```fides
fn parse_model(self: &mut Parser) -> ParseResultModelDef {
```

### parse_component

```fides
fn parse_component(self: &mut Parser) -> ParseResultComponentDef {
```

### parse_system

```fides
fn parse_system(self: &mut Parser) -> ParseResultSystemDef {
```

### parse_state_machine

```fides
fn parse_state_machine(self: &mut Parser) -> ParseResultStateMachineDef {
```

### parse_event_handler

```fides
fn parse_event_handler(self: &mut Parser) -> ParseResultEventHandlerDef {
```

### parse_type

```fides
fn parse_type(self: &mut Parser) -> ParseResultType {
```

### nest_too_deep

```fides
fn nest_too_deep(self: &mut Parser) -> str {
```

### push_parse_error

```fides
fn push_parse_error(self: &mut Parser, e: str) {
```

### parse_block

```fides
fn parse_block(self: &mut Parser) -> ParseResultStmts {
```

### parse_block_inner

```fides
fn parse_block_inner(self: &mut Parser) -> ParseResultStmts {
```

### parse_stmt

```fides
fn parse_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_assign_or_expr

```fides
fn parse_assign_or_expr(self: &mut Parser) -> ParseResultStmt {
```

### parse_while

```fides
fn parse_while(self: &mut Parser) -> ParseResultStmt {
```

### parse_guard_stmt

```fides
fn parse_guard_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_for

```fides
fn parse_for(self: &mut Parser) -> ParseResultStmt {
```

### parse_loop

```fides
fn parse_loop(self: &mut Parser) -> ParseResultStmt {
```

### parse_query_stmt

```fides
fn parse_query_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_match_stmt

```fides
fn parse_match_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_select_stmt

```fides
fn parse_select_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_parallel_stmt

```fides
fn parse_parallel_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_let

```fides
fn parse_let(self: &mut Parser) -> ParseResultStmt {
```

### parse_persist_let

```fides
fn parse_persist_let(self: &mut Parser) -> ParseResultStmt {
```

### parse_return

```fides
fn parse_return(self: &mut Parser) -> ParseResultStmt {
```

### parse_if_expr

```fides
fn parse_if_expr(self: &mut Parser) -> ParseResultIfExpr {
```

### parse_use_stmt

```fides
fn parse_use_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_handle_stmt

```fides
fn parse_handle_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_do_stmt

```fides
fn parse_do_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_resume_stmt

```fides
fn parse_resume_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_spawn_scope_stmt

```fides
fn parse_spawn_scope_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_cancel_scope_stmt

```fides
fn parse_cancel_scope_stmt(self: &mut Parser) -> ParseResultStmt {
```

### timeout_block_ahead

```fides
fn timeout_block_ahead(self: &Parser) -> bool {
```

### parse_timeout_stmt

```fides
fn parse_timeout_stmt(self: &mut Parser) -> ParseResultStmt {
```

### parse_region_block

```fides
fn parse_region_block(self: &mut Parser) -> ParseResultStmt {
```

### parse_sandbox_block

```fides
fn parse_sandbox_block(self: &mut Parser) -> ParseResultStmt {
```

### parse_unsafe_block

```fides
fn parse_unsafe_block(self: &mut Parser) -> ParseResultStmt {
```

### parse_txn_block

```fides
fn parse_txn_block(self: &mut Parser) -> ParseResultStmt {
```

### parse_grad_block

```fides
fn parse_grad_block(self: &mut Parser) -> ParseResultStmt {
```

### parse_pattern

```fides
fn parse_pattern(self: &mut Parser, nests: &mut Vec<PatNest>, ctr: &mut i64) -> ParseResultPattern {
```

### parse_pat_arg

One argument inside `Ctor(...)`: either a plain `name [: ty]` binding
or a nested pattern (`Ok(x)`, `E::V(y)`, `1`, `_`). A nested pattern
is bound to a fresh `__np` name and recorded in `nests`; the match
arm's desugar gives it an inner match of its own.

```fides
fn parse_pat_arg(self: &mut Parser, bindings: &mut Vec<Binding>, nests: &mut Vec<PatNest>, ctr: &mut i64) -> ParseResultUnit {
```

### pd_desugar_arms

Desugar a match's arms after parse: an arm whose pattern recorded
nested positions gets a body of inner `match __np { pat => body,
_ => <the remaining arms> }`, applied once per nested position.
`marks` — nests[marks[i] .. marks[i+1]] are arm i's positions;
marks.len() == arms.len() + 1. Recursive over the arm list.

```fides
fn pd_desugar_arms(self: &mut Parser, scrut: str, arms: Vec<MatchArm>, nests: Vec<PatNest>, marks: Vec<i64>, i: i64, line: i64, as_expr: bool) -> Vec<MatchArm> {
```

### parse_expr

```fides
fn parse_expr(self: &mut Parser) -> ParseResultExpr {
```

### parse_pipe_forward

```fides
fn parse_pipe_forward(self: &mut Parser) -> ParseResultExpr {
```

### parse_coalesce

```fides
fn parse_coalesce(self: &mut Parser) -> ParseResultExpr {
```

### parse_range

```fides
fn parse_range(self: &mut Parser) -> ParseResultExpr {
```

### parse_or

```fides
fn parse_or(self: &mut Parser) -> ParseResultExpr {
```

### parse_bit_or

```fides
fn parse_bit_or(self: &mut Parser) -> ParseResultExpr {
```

### parse_bit_xor

```fides
fn parse_bit_xor(self: &mut Parser) -> ParseResultExpr {
```

### parse_bit_and

```fides
fn parse_bit_and(self: &mut Parser) -> ParseResultExpr {
```

### parse_shift

```fides
fn parse_shift(self: &mut Parser) -> ParseResultExpr {
```

### parse_and

```fides
fn parse_and(self: &mut Parser) -> ParseResultExpr {
```

### parse_equality

```fides
fn parse_equality(self: &mut Parser) -> ParseResultExpr {
```

### parse_comparison

```fides
fn parse_comparison(self: &mut Parser) -> ParseResultExpr {
```

### parse_addition

```fides
fn parse_addition(self: &mut Parser) -> ParseResultExpr {
```

### parse_multiplication

```fides
fn parse_multiplication(self: &mut Parser) -> ParseResultExpr {
```

### parse_unary

```fides
fn parse_unary(self: &mut Parser) -> ParseResultExpr {
```

### parse_prefix_operand

```fides
fn parse_prefix_operand(self: &mut Parser) -> ParseResultExpr {
```

### parse_unary_inner

```fides
fn parse_unary_inner(self: &mut Parser) -> ParseResultExpr {
```

### parse_comptime_expr

```fides
fn parse_comptime_expr(self: &mut Parser, _line: i64) -> ParseResultExpr {
```

### parse_postfix

```fides
fn parse_postfix(self: &mut Parser) -> ParseResultExpr {
```

### parse_match_expr

```fides
fn parse_match_expr(self: &mut Parser) -> ParseResultExpr {
```

### parse_call_expr

```fides
fn parse_call_expr(self: &mut Parser, callee: str) -> ParseResultExpr {
```

### parse_struct_lit_expr

```fides
fn parse_struct_lit_expr(self: &mut Parser, name: str) -> ParseResultExpr {
```

### parse_expr_list

```fides
fn parse_expr_list(self: &mut Parser) -> ParseResultExprs {
```

### parse_tuple_rest

```fides
fn parse_tuple_rest(self: &mut Parser, first: Expr) -> ParseResultExpr {
```

### parse_primary

```fides
fn parse_primary(self: &mut Parser) -> ParseResultExpr {
```

### parse_asm

```fides
fn parse_asm(self: &mut Parser) -> ParseResultExpr {
```

### peek_bin_op

```fides
fn peek_bin_op(self: &Parser) -> BinOpInfo {
```

### parse_expr_iterative

```fides
fn parse_expr_iterative(self: &mut Parser) -> ParseResultExpr {
```

### pd_field_init_follows

`{ ident :` at the parser's position, after a name. BG257: of any
case: the structs of other modules are lowercase as often as not
(math's `vec2`, `mat4`), and no block starts with `ident :` either
way; the capital a name needed before kept their literals from
parsing.

```fides
fn pd_field_init_follows(self: &Parser, name: str) -> bool {
```

### pd_at_import

`@import(` at the parser's position.

```fides
fn pd_at_import(self: &Parser) -> bool {
```

### parse_at_import

`@import("path/foo.h")`: the import of `path/foo`, found as a header
(compiler.fd's import resolution: `.fd` first, then `.h`).

```fides
fn parse_at_import(self: &mut Parser) -> ParseResultImportDef {
```

### parse_use

```fides
fn parse_use(self: &mut Parser) -> ParseResultImportDef {
```

### parse_at_annotation

```fides
fn parse_at_annotation(self: &mut Parser) -> ParseResultAnnotation {
```

### parse_annotation_arg

```fides
fn parse_annotation_arg(self: &mut Parser) -> ParseResultStr {
```

### parse_attribute

```fides
fn parse_attribute(self: &mut Parser) -> ParseResultAttribute {
```

### parse_fstring_parts

The parts of an f-string: text, and each `{ ... }` as one expression
(reference §1.8). `line` and `col` are the literal's own: an error in a
part names the f-string.

```fides
fn parse_fstring_parts(self: &mut Parser, raw: str, line: i64, col: i64) -> ParseResultExpr {
```

*Defined at line 592*

---

## Parser_with_names

```fides
pub fn Parser_with_names(tokens: Vec<Token>, struct_names: Vec<str>, enum_names: Vec<str>) -> Parser {
```

**Parameters:**

- `tokens`: `Vec<Token>`
- `struct_names`: `Vec<str>`
- `enum_names`: `Vec<str>`

**Returns:** `Parser`

*Defined at line 9593*

---

## vec_str_join

```fides
pub fn vec_str_join(v: Vec<str>, sep: str) -> str {
```

**Parameters:**

- `v`: `Vec<str>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 9611*

---

## pd_size_type_tok

L8A: `usize` / `isize` after a const parameter (`const N: usize`):
words, not type keywords, read as i64.

```fides
pub fn pd_size_type_tok(kind: TokenKind) -> bool {
```

**Parameters:**

- `kind`: `TokenKind`

**Returns:** `bool`

*Defined at line 9746*

---

## is_int_type_keyword

```fides
pub fn is_int_type_keyword(kind: TokenKind) -> bool {
```

**Parameters:**

- `kind`: `TokenKind`

**Returns:** `bool`

*Defined at line 9757*

---

## pd_expected_name_err

The error for a name expected where a reserved type name stands
(`val f32: i64 = 1;`): says the word is reserved (bugs 13/26), rather
than "expected identifier but got 'F32'".

```fides
pub fn pd_expected_name_err(tok: Token) -> str {
```

**Parameters:**

- `tok`: `Token`

**Returns:** `str`

*Defined at line 9794*

---

## type_keyword_name

```fides
pub fn type_keyword_name(kind: TokenKind) -> str {
```

**Parameters:**

- `kind`: `TokenKind`

**Returns:** `str`

*Defined at line 9814*

---

## starts_type

```fides
pub fn starts_type(kind: TokenKind) -> bool {
```

**Parameters:**

- `kind`: `TokenKind`

**Returns:** `bool`

*Defined at line 9848*

---

## ParseResultIfExpr

```fides
pub enum ParseResultIfExpr {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 9916*

---

## BinOpInfo

```fides
pub struct BinOpInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `op` | `BinOp` |
| `prec` | `i64` |
| `found` | `bool` |

*Defined at line 9921*

---

## BinOpInfo_new

```fides
pub fn BinOpInfo_new(op: BinOp, prec: i64, found: bool) -> BinOpInfo {
```

**Parameters:**

- `op`: `BinOp`
- `prec`: `i64`
- `found`: `bool`

**Returns:** `BinOpInfo`

*Defined at line 9927*

---

## reduce_one_op

```fides
pub fn reduce_one_op(vals: &mut Vec<Expr>, ops: &mut Vec<OpEntry>) -> bool {
```

**Parameters:**

- `vals`: `&mut Vec<Expr>`
- `ops`: `&mut Vec<OpEntry>`

**Returns:** `bool`

*Defined at line 9933*

---

## parser_saw_secret

```fides
pub fn parser_saw_secret() -> bool {
```

**Returns:** `bool`

*Defined at line 9959*

---

## parser_saw_splice

```fides
pub fn parser_saw_splice() -> bool {
```

**Returns:** `bool`

*Defined at line 9966*

---

## parse_expression_source

LG9: the text a `comptime splice { }` block returned, parsed as exactly
one expression. The tokens count lines from `line` (the splice's), so an
error inside the generated expression points at the splice;
`struct_names` / `enum_names` are the program's, so `Name { .. }` and
`Name::V` in the text parse as they would in the source. Err: why the
text is not one expression.

```fides
pub fn parse_expression_source(source: str, line: i64, struct_names: Vec<str>, enum_names: Vec<str>) -> ParseResultExpr {
```

**Parameters:**

- `source`: `str`
- `line`: `i64`
- `struct_names`: `Vec<str>`
- `enum_names`: `Vec<str>`

**Returns:** `ParseResultExpr`

*Defined at line 9976*

---

## pd_impl_narrow_self

L8H: the narrower type the methods of an impl of `struct_name` (type
parameters `tps`) take `self` as: `Wrap<i64>` for `impl Wrap<i64>` or
`impl Show for Wrap<i64>` once their `self` is typed so
(pd_impl_specializations, pd_trait_impl_specializations). Void when the
impl is over the whole struct or has no method with `self`.

```fides
pub fn pd_impl_narrow_self(struct_name: str, tps: Vec<str>, methods: Vec<Func>) -> Type {
```

**Parameters:**

- `struct_name`: `str`
- `tps`: `Vec<str>`
- `methods`: `Vec<Func>`

**Returns:** `Type`

*Defined at line 10600*

---

## pd_impl_is_narrowed

L8H: whether `imp` is an impl for a narrower type than its struct's
(`impl Wrap<i64>`, `impl<U> Wrap<Vec<U>>`), seen from its methods'
`self`. merge_programs keeps a module's generic impl of the struct
beside it (it is no replacement of that impl).

```fides
pub fn pd_impl_is_narrowed(imp: ImplDef) -> bool {
```

**Parameters:**

- `imp`: `ImplDef`

**Returns:** `bool`

*Defined at line 10633*

---

## pd_cross_module_specializations

L8H across modules: a definition in one module specializes a generic
definition of a module it imports (`use shapes;` with `fn area(s:
Square)` beside shapes' `pub fn area<T>(s: T)`; `impl Wrap<i64> { fn
show(self) }` beside `impl<T> Wrap<T> { fn show(self) }`; `impl Show
for Wrap<i64>` beside `impl<T> Show for Wrap<T>`) when the generic one
takes its types and it is strictly more specific: it is renamed
`name__spec<k>` (k after every number of that name in any module), as
pd_name_specializations / pd_impl_specializations /
pd_trait_impl_specializations do within one file, so it is no duplicate
definition (compiler.fd's checks) and no shadowing
(cc_qualify_shadowed_imports): mono gives every instantiation the body
of the most specific definition, wherever it is called from. A
function's generic definition must be `pub`. `mods` are the imported
modules (their names in `names`), `user` the program; called by
compiler.fd before its duplicate checks. The errors found.

```fides
pub fn pd_cross_module_specializations(mods: &mut Vec<Program>, names: Vec<str>, user: &mut Program) -> Vec<str> {
```

**Parameters:**

- `mods`: `&mut Vec<Program>`
- `names`: `Vec<str>`
- `user`: `&mut Program`

**Returns:** `Vec<str>`

*Defined at line 10864*

---

## parser_sequential_vars

0008: the vars declared Vec<E..> (sequential) — the lowerer reads this
to route their Vec method calls to VecV_* (lower.fd copies it).

```fides
pub fn parser_sequential_vars() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 11144*

---

## PreParse

A module parsed on a worker thread: its Program (ok: the parse
succeeded), what it changed besides (ParseSide), and the distinct words
of its text with, for each, what it was known as when the parse began
(1 a struct, 2 an enum, 4 an import name).

```fides
pub struct PreParse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `prog` | `Program` |
| `side` | `ParseSide` |
| `words` | `Vec<str>` |
| `bits` | `Vec<i64>` |

*Defined at line 11172*

---

## pp_is_word

Whether `s` is one word as pp_text_words reads them.

```fides
pub fn pp_is_word(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 11204*

---

## pp_text_words

The distinct words of `src` (runs of letters, digits, `_` and bytes from
128 on: in code, comments and strings alike), first occurrence first.
Every name a parse of the text asks about is one of them.

```fides
pub fn pp_text_words(src: str) -> Vec<str> {
```

**Parameters:**

- `src`: `str`

**Returns:** `Vec<str>`

*Defined at line 11222*

---

## pp_bits_of

What `w` is to the parser `p` (before its parse): 1 a struct name, 2 an
enum name, 4 an import name (or'ed).

```fides
pub fn pp_bits_of(p: Parser, w: str) -> i64 {
```

**Parameters:**

- `p`: `Parser`
- `w`: `str`

**Returns:** `i64`

*Defined at line 11253*

---

## pp_parse_one

Parse module `k` of the g_pp_* tables, knowing these names.

```fides
pub fn pp_parse_one(k: i64, sn: Vec<str>, en: Vec<str>, inames: Vec<str>) -> PreParse {
```

**Parameters:**

- `k`: `i64`
- `sn`: `Vec<str>`
- `en`: `Vec<str>`
- `inames`: `Vec<str>`

**Returns:** `PreParse`

*Defined at line 11279*

---

## pp_parse_worker

A worker of pp_parse_par: the modules of its plan, in the plan's order.

```fides
pub fn pp_parse_worker(w: i64) -> Vec<PreParse> {
```

**Parameters:**

- `w`: `i64`

**Returns:** `Vec<PreParse>`

*Defined at line 11306*

---

## pp_parse_par

Each module of the tables parsed (in module order), in up to `nw`
threads: `toks[k]` the tokens of module k, `texts[k]` its text, and it
knows the first `sn_n[k]` of `snames` as struct names, `en_n[k]` of
`enames` as enum names and `in_n[k]` of `inames` as import names. With
one worker, parsed here one after another (as on a worker: nothing but
the results changes either way).

```fides
pub fn pp_parse_par(toks: Vec<Vec<Token>>, texts: Vec<str>, snames: Vec<str>, enames: Vec<str>, inames: Vec<str>, sn_n: Vec<i64>, en_n: Vec<i64>, in_n: Vec<i64>, nw: i64) -> Vec<PreParse> {
```

**Parameters:**

- `toks`: `Vec<Vec<Token>>`
- `texts`: `Vec<str>`
- `snames`: `Vec<str>`
- `enames`: `Vec<str>`
- `inames`: `Vec<str>`
- `sn_n`: `Vec<i64>`
- `en_n`: `Vec<i64>`
- `in_n`: `Vec<i64>`
- `nw`: `i64`

**Returns:** `Vec<PreParse>`

*Defined at line 11362*

---

## PpPlan

The names pp_predict expects the import loop to know: module i (an
index of the ModGraph) the first sn_n[i] of snames, en_n[i] of enames
and in_n[i] of inames (-1: a module whose file an earlier index has).

```fides
pub struct PpPlan {
```

**Fields:**

| Name | Type |
|------|------|
| `snames` | `Vec<str>` |
| `enames` | `Vec<str>` |
| `inames` | `Vec<str>` |
| `sn_n` | `Vec<i64>` |
| `en_n` | `Vec<i64>` |
| `in_n` | `Vec<i64>` |

*Defined at line 11454*

---

## pp_predict

compiler.fd's import loop run on what the tokens say instead of on
parses, from the program's imports `user_imports` (paths, in order) and
the names known before it (`struct_names`, `enum_names`, the import
set): the names it knows when it first parses each module of `g`
(cached_parse keeps that first parse, deferred or not). Keep in step
with the loop.

```fides
pub fn pp_predict(user_imports: Vec<str>, g: ModGraph, struct_names: Vec<str>, enum_names: Vec<str>) -> PpPlan {
```

**Parameters:**

- `user_imports`: `Vec<str>`
- `g`: `ModGraph`
- `struct_names`: `Vec<str>`
- `enum_names`: `Vec<str>`

**Returns:** `PpPlan`

*Defined at line 11482*

---

## pp_begin

Hand the loop the parses ahead: `results[k]` that of `files[k]`, begun
with `base_sn` struct names and `base_en` enum names known (the loop's
first ones) and the import set as it is now.

```fides
pub fn pp_begin(results: Vec<PreParse>, files: Vec<str>, base_sn: i64, base_en: i64) {
```

**Parameters:**

- `results`: `Vec<PreParse>`
- `files`: `Vec<str>`
- `base_sn`: `i64`
- `base_en`: `i64`

*Defined at line 11643*

---

## pp_end

No parse ahead to take any more; the tables freed.

```fides
pub fn pp_end() {
```

*Defined at line 11670*

---

## pp_count

How many parses ahead there are (0: none to take).

```fides
pub fn pp_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 11688*

---

## pp_hits

How many the loop took, and how many it parsed itself.

```fides
pub fn pp_hits() -> i64 {
```

**Returns:** `i64`

*Defined at line 11693*

---

## pp_misses

```fides
pub fn pp_misses() -> i64 {
```

**Returns:** `i64`

*Defined at line 11697*

---

## pp_result

Parse ahead `k` (for FID_PP_VERIFY).

```fides
pub fn pp_result(k: i64) -> PreParse {
```

**Parameters:**

- `k`: `i64`

**Returns:** `PreParse`

*Defined at line 11702*

---

## pp_valid

The index of the parse ahead of `path` the loop may take in place of
parsing it with these names (Parser_with_names(tokens, struct_names,
enum_names) and the import set as it is now): -1 when there is none or
the loop's parse could differ. Asked once per module (the loop keeps
its first parse).

```fides
pub fn pp_valid(path: str, struct_names: Vec<str>, enum_names: Vec<str>) -> i64 {
```

**Parameters:**

- `path`: `str`
- `struct_names`: `Vec<str>`
- `enum_names`: `Vec<str>`

**Returns:** `i64`

*Defined at line 11739*

---

## pp_use

Take parse ahead `k` (pp_valid said so): its other changes made as the
loop's parse would have made them, and its Program.

```fides
pub fn pp_use(k: i64) -> Program {
```

**Parameters:**

- `k`: `i64`

**Returns:** `Program`

*Defined at line 11787*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
