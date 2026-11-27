[All modules](index.md)

# ast_ser -- Fides Documentation

## Contents

- [AwBuf](#AwBuf) (struct)
- [ArBuf](#ArBuf) (struct)
- [aw_u8](#aw_u8) (function)
- [aw_i64](#aw_i64) (function)
- [aw_bool](#aw_bool) (function)
- [aw_str](#aw_str) (function)
- [aw_f64](#aw_f64) (function)
- [ar_u8](#ar_u8) (function)
- [ar_i64](#ar_i64) (function)
- [ar_bool](#ar_bool) (function)
- [ar_str](#ar_str) (function)
- [ar_f64](#ar_f64) (function)
- [aw_optstr](#aw_optstr) (function)
- [ar_optstr](#ar_optstr) (function)
- [aw_opti64](#aw_opti64) (function)
- [ar_opti64](#ar_opti64) (function)
- [aw_binop](#aw_binop) (function)
- [ar_binop](#ar_binop) (function)
- [aw_unop](#aw_unop) (function)
- [ar_unop](#ar_unop) (function)
- [aw_assignop](#aw_assignop) (function)
- [ar_assignop](#ar_assignop) (function)
- [aw_reflectop](#aw_reflectop) (function)
- [ar_reflectop](#ar_reflectop) (function)
- [aw_asmdir](#aw_asmdir) (function)
- [ar_asmdir](#ar_asmdir) (function)
- [aw_shapedim](#aw_shapedim) (function)
- [ar_shapedim](#ar_shapedim) (function)
- [aw_type](#aw_type) (function)
- [ar_type](#ar_type) (function)
- [aw_opttype](#aw_opttype) (function)
- [ar_opttype](#ar_opttype) (function)
- [aw_strpair](#aw_strpair) (function)
- [ar_strpair](#ar_strpair) (function)
- [aw_strs](#aw_strs) (function)
- [ar_strs](#ar_strs) (function)
- [aw_annotation](#aw_annotation) (function)
- [ar_annotation](#ar_annotation) (function)
- [aw_annotations](#aw_annotations) (function)
- [ar_annotations](#ar_annotations) (function)
- [aw_fieldinit](#aw_fieldinit) (function)
- [ar_fieldinit](#ar_fieldinit) (function)
- [aw_exprs](#aw_exprs) (function)
- [ar_exprs](#ar_exprs) (function)
- [aw_optexpr](#aw_optexpr) (function)
- [ar_optexpr](#ar_optexpr) (function)
- [aw_optstmts](#aw_optstmts) (function)
- [ar_optstmts](#ar_optstmts) (function)
- [aw_matcharm](#aw_matcharm) (function)
- [ar_matcharm](#ar_matcharm) (function)
- [aw_matcharms](#aw_matcharms) (function)
- [ar_matcharms](#ar_matcharms) (function)
- [aw_fstringpart](#aw_fstringpart) (function)
- [ar_fstringpart](#ar_fstringpart) (function)
- [aw_fstringparts](#aw_fstringparts) (function)
- [ar_fstringparts](#ar_fstringparts) (function)
- [aw_asmoperand](#aw_asmoperand) (function)
- [ar_asmoperand](#ar_asmoperand) (function)
- [aw_asmoperands](#aw_asmoperands) (function)
- [ar_asmoperands](#ar_asmoperands) (function)
- [aw_expr](#aw_expr) (function)
- [ar_expr](#ar_expr) (function)
- [aw_binding](#aw_binding) (function)
- [ar_binding](#ar_binding) (function)
- [aw_pattern](#aw_pattern) (function)
- [ar_pattern](#ar_pattern) (function)
- [aw_param](#aw_param) (function)
- [ar_param](#ar_param) (function)
- [aw_params](#aw_params) (function)
- [ar_params](#ar_params) (function)
- [aw_optparams](#aw_optparams) (function)
- [ar_optparams](#ar_optparams) (function)
- [aw_selectcase](#aw_selectcase) (function)
- [ar_selectcase](#ar_selectcase) (function)
- [aw_effecthandler](#aw_effecthandler) (function)
- [ar_effecthandler](#ar_effecthandler) (function)
- [aw_selectcases](#aw_selectcases) (function)
- [ar_selectcases](#ar_selectcases) (function)
- [aw_effecthandlers](#aw_effecthandlers) (function)
- [ar_effecthandlers](#ar_effecthandlers) (function)
- [aw_stmt](#aw_stmt) (function)
- [aw_stmts](#aw_stmts) (function)
- [ar_stmt](#ar_stmt) (function)
- [ar_stmts](#ar_stmts) (function)
- [aw_func](#aw_func) (function)
- [ar_func](#ar_func) (function)
- [aw_funcs](#aw_funcs) (function)
- [ar_funcs](#ar_funcs) (function)
- [aw_structdef](#aw_structdef) (function)
- [ar_structdef](#ar_structdef) (function)
- [aw_enumvariant](#aw_enumvariant) (function)
- [ar_enumvariant](#ar_enumvariant) (function)
- [aw_enumdef](#aw_enumdef) (function)
- [ar_enumdef](#ar_enumdef) (function)
- [aw_constdef](#aw_constdef) (function)
- [ar_constdef](#ar_constdef) (function)
- [aw_globalvar](#aw_globalvar) (function)
- [ar_globalvar](#ar_globalvar) (function)
- [aw_typealias](#aw_typealias) (function)
- [ar_typealias](#ar_typealias) (function)
- [aw_impldef](#aw_impldef) (function)
- [ar_impldef](#ar_impldef) (function)
- [aw_assoctypedecl](#aw_assoctypedecl) (function)
- [ar_assoctypedecl](#ar_assoctypedecl) (function)
- [aw_assoctypebinding](#aw_assoctypebinding) (function)
- [ar_assoctypebinding](#ar_assoctypebinding) (function)
- [aw_traitmethod](#aw_traitmethod) (function)
- [ar_traitmethod](#ar_traitmethod) (function)
- [aw_traitdef](#aw_traitdef) (function)
- [ar_traitdef](#ar_traitdef) (function)
- [aw_traitimpl](#aw_traitimpl) (function)
- [ar_traitimpl](#ar_traitimpl) (function)
- [aw_importdef](#aw_importdef) (function)
- [ar_importdef](#ar_importdef) (function)
- [aw_attribute](#aw_attribute) (function)
- [ar_attribute](#ar_attribute) (function)
- [aw_externdecl](#aw_externdecl) (function)
- [ar_externdecl](#ar_externdecl) (function)
- [aw_layerinst](#aw_layerinst) (function)
- [ar_layerinst](#ar_layerinst) (function)
- [aw_layerdef](#aw_layerdef) (function)
- [ar_layerdef](#ar_layerdef) (function)
- [aw_modeldef](#aw_modeldef) (function)
- [ar_modeldef](#ar_modeldef) (function)
- [aw_componentdef](#aw_componentdef) (function)
- [ar_componentdef](#ar_componentdef) (function)
- [aw_systemdef](#aw_systemdef) (function)
- [ar_systemdef](#ar_systemdef) (function)
- [aw_statetransition](#aw_statetransition) (function)
- [ar_statetransition](#ar_statetransition) (function)
- [aw_statedef](#aw_statedef) (function)
- [ar_statedef](#ar_statedef) (function)
- [aw_statemachinedef](#aw_statemachinedef) (function)
- [ar_statemachinedef](#ar_statemachinedef) (function)
- [aw_eventhandlerdef](#aw_eventhandlerdef) (function)
- [ar_eventhandlerdef](#ar_eventhandlerdef) (function)
- [aw_program](#aw_program) (function)
- [ar_program](#ar_program) (function)
- [ast_ser_program](#ast_ser_program) (function)
- [ast_deser_program](#ast_deser_program) (function)
- [ast_deser_ok](#ast_deser_ok) (function)
- [awbuf_new](#awbuf_new) (function)
- [arbuf_new](#arbuf_new) (function)
- [arbuf_ok](#arbuf_ok) (function)
- [awbuf_bytes](#awbuf_bytes) (function)
- [ast_bytes_to_str](#ast_bytes_to_str) (function)

## AwBuf

```fides
pub struct AwBuf {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |

*Defined at line 12*

---

## ArBuf

```fides
pub struct ArBuf {
```

**Fields:**

| Name | Type |
|------|------|
| `v` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |

*Defined at line 16*

---

## aw_u8

```fides
pub fn aw_u8(b: &mut AwBuf, v: i64) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `i64`

*Defined at line 22*

---

## aw_i64

```fides
pub fn aw_i64(b: &mut AwBuf, v: i64) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `i64`

*Defined at line 27*

---

## aw_bool

```fides
pub fn aw_bool(b: &mut AwBuf, v: bool) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `bool`

*Defined at line 37*

---

## aw_str

```fides
pub fn aw_str(b: &mut AwBuf, s: str) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `s`: `str`

*Defined at line 45*

---

## aw_f64

```fides
pub fn aw_f64(b: &mut AwBuf, v: f64) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `f64`

*Defined at line 54*

---

## ar_u8

```fides
pub fn ar_u8(r: &mut ArBuf) -> i64 {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `i64`

*Defined at line 58*

---

## ar_i64

```fides
pub fn ar_i64(r: &mut ArBuf) -> i64 {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `i64`

*Defined at line 72*

---

## ar_bool

```fides
pub fn ar_bool(r: &mut ArBuf) -> bool {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `bool`

*Defined at line 87*

---

## ar_str

```fides
pub fn ar_str(r: &mut ArBuf) -> str {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `str`

*Defined at line 91*

---

## ar_f64

```fides
pub fn ar_f64(r: &mut ArBuf) -> f64 {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `f64`

*Defined at line 113*

---

## aw_optstr

```fides
pub fn aw_optstr(b: &mut AwBuf, v: OptStr) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptStr`

*Defined at line 119*

---

## ar_optstr

```fides
pub fn ar_optstr(r: &mut ArBuf) -> OptStr {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptStr`

*Defined at line 131*

---

## aw_opti64

```fides
pub fn aw_opti64(b: &mut AwBuf, v: OptI64) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptI64`

*Defined at line 138*

---

## ar_opti64

```fides
pub fn ar_opti64(r: &mut ArBuf) -> OptI64 {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptI64`

*Defined at line 150*

---

## aw_binop

```fides
pub fn aw_binop(b: &mut AwBuf, op: BinOp) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `op`: `BinOp`

*Defined at line 159*

---

## ar_binop

```fides
pub fn ar_binop(r: &mut ArBuf) -> BinOp {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `BinOp`

*Defined at line 220*

---

## aw_unop

```fides
pub fn aw_unop(b: &mut AwBuf, op: UnaryOp) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `op`: `UnaryOp`

*Defined at line 276*

---

## ar_unop

```fides
pub fn ar_unop(r: &mut ArBuf) -> UnaryOp {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `UnaryOp`

*Defined at line 290*

---

## aw_assignop

```fides
pub fn aw_assignop(b: &mut AwBuf, op: AssignOp) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `op`: `AssignOp`

*Defined at line 301*

---

## ar_assignop

```fides
pub fn ar_assignop(r: &mut ArBuf) -> AssignOp {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `AssignOp`

*Defined at line 341*

---

## aw_reflectop

```fides
pub fn aw_reflectop(b: &mut AwBuf, op: ReflectOp) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `op`: `ReflectOp`

*Defined at line 376*

---

## ar_reflectop

```fides
pub fn ar_reflectop(r: &mut ArBuf) -> ReflectOp {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ReflectOp`

*Defined at line 419*

---

## aw_asmdir

```fides
pub fn aw_asmdir(b: &mut AwBuf, d: AsmDir) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `AsmDir`

*Defined at line 457*

---

## ar_asmdir

```fides
pub fn ar_asmdir(r: &mut ArBuf) -> AsmDir {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `AsmDir`

*Defined at line 471*

---

## aw_shapedim

```fides
pub fn aw_shapedim(b: &mut AwBuf, d: ShapeDim) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ShapeDim`

*Defined at line 482*

---

## ar_shapedim

```fides
pub fn ar_shapedim(r: &mut ArBuf) -> ShapeDim {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ShapeDim`

*Defined at line 495*

---

## aw_type

```fides
pub fn aw_type(b: &mut AwBuf, t: Type) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `t`: `Type`

*Defined at line 504*

---

## ar_type

```fides
pub fn ar_type(r: &mut ArBuf) -> Type {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Type`

*Defined at line 639*

---

## aw_opttype

```fides
pub fn aw_opttype(b: &mut AwBuf, v: OptType) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptType`

*Defined at line 769*

---

## ar_opttype

```fides
pub fn ar_opttype(r: &mut ArBuf) -> OptType {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptType`

*Defined at line 781*

---

## aw_strpair

```fides
pub fn aw_strpair(b: &mut AwBuf, p: StrPair) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `p`: `StrPair`

*Defined at line 790*

---

## ar_strpair

```fides
pub fn ar_strpair(r: &mut ArBuf) -> StrPair {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `StrPair`

*Defined at line 795*

---

## aw_strs

```fides
pub fn aw_strs(b: &mut AwBuf, v: Vec<str>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<str>`

*Defined at line 800*

---

## ar_strs

```fides
pub fn ar_strs(r: &mut ArBuf) -> Vec<str> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<str>`

*Defined at line 809*

---

## aw_annotation

```fides
pub fn aw_annotation(b: &mut AwBuf, a: Annotation) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `a`: `Annotation`

*Defined at line 820*

---

## ar_annotation

```fides
pub fn ar_annotation(r: &mut ArBuf) -> Annotation {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Annotation`

*Defined at line 825*

---

## aw_annotations

```fides
pub fn aw_annotations(b: &mut AwBuf, v: Vec<Annotation>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<Annotation>`

*Defined at line 829*

---

## ar_annotations

```fides
pub fn ar_annotations(r: &mut ArBuf) -> Vec<Annotation> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<Annotation>`

*Defined at line 838*

---

## aw_fieldinit

```fides
pub fn aw_fieldinit(b: &mut AwBuf, f: FieldInit) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `f`: `FieldInit`

*Defined at line 849*

---

## ar_fieldinit

```fides
pub fn ar_fieldinit(r: &mut ArBuf) -> FieldInit {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `FieldInit`

*Defined at line 854*

---

## aw_exprs

```fides
pub fn aw_exprs(b: &mut AwBuf, v: Vec<Expr>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<Expr>`

*Defined at line 858*

---

## ar_exprs

```fides
pub fn ar_exprs(r: &mut ArBuf) -> Vec<Expr> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<Expr>`

*Defined at line 867*

---

## aw_optexpr

```fides
pub fn aw_optexpr(b: &mut AwBuf, v: OptBoxExpr) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptBoxExpr`

*Defined at line 878*

---

## ar_optexpr

```fides
pub fn ar_optexpr(r: &mut ArBuf) -> OptBoxExpr {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptBoxExpr`

*Defined at line 890*

---

## aw_optstmts

```fides
pub fn aw_optstmts(b: &mut AwBuf, v: OptStmts) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptStmts`

*Defined at line 897*

---

## ar_optstmts

```fides
pub fn ar_optstmts(r: &mut ArBuf) -> OptStmts {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptStmts`

*Defined at line 909*

---

## aw_matcharm

```fides
pub fn aw_matcharm(b: &mut AwBuf, m: MatchArm) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `m`: `MatchArm`

*Defined at line 916*

---

## ar_matcharm

```fides
pub fn ar_matcharm(r: &mut ArBuf) -> MatchArm {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `MatchArm`

*Defined at line 922*

---

## aw_matcharms

```fides
pub fn aw_matcharms(b: &mut AwBuf, v: Vec<MatchArm>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<MatchArm>`

*Defined at line 926*

---

## ar_matcharms

```fides
pub fn ar_matcharms(r: &mut ArBuf) -> Vec<MatchArm> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<MatchArm>`

*Defined at line 935*

---

## aw_fstringpart

```fides
pub fn aw_fstringpart(b: &mut AwBuf, p: FStringPart) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `p`: `FStringPart`

*Defined at line 946*

---

## ar_fstringpart

```fides
pub fn ar_fstringpart(r: &mut ArBuf) -> FStringPart {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `FStringPart`

*Defined at line 959*

---

## aw_fstringparts

```fides
pub fn aw_fstringparts(b: &mut AwBuf, v: Vec<FStringPart>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<FStringPart>`

*Defined at line 966*

---

## ar_fstringparts

```fides
pub fn ar_fstringparts(r: &mut ArBuf) -> Vec<FStringPart> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<FStringPart>`

*Defined at line 975*

---

## aw_asmoperand

```fides
pub fn aw_asmoperand(b: &mut AwBuf, o: AsmOperand) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `o`: `AsmOperand`

*Defined at line 986*

---

## ar_asmoperand

```fides
pub fn ar_asmoperand(r: &mut ArBuf) -> AsmOperand {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `AsmOperand`

*Defined at line 992*

---

## aw_asmoperands

```fides
pub fn aw_asmoperands(b: &mut AwBuf, v: Vec<AsmOperand>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<AsmOperand>`

*Defined at line 996*

---

## ar_asmoperands

```fides
pub fn ar_asmoperands(r: &mut ArBuf) -> Vec<AsmOperand> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<AsmOperand>`

*Defined at line 1005*

---

## aw_expr

```fides
pub fn aw_expr(b: &mut AwBuf, e: Expr) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `e`: `Expr`

*Defined at line 1016*

---

## ar_expr

```fides
pub fn ar_expr(r: &mut ArBuf) -> Expr {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Expr`

*Defined at line 1217*

---

## aw_binding

```fides
pub fn aw_binding(b: &mut AwBuf, x: Binding) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `x`: `Binding`

*Defined at line 1355*

---

## ar_binding

```fides
pub fn ar_binding(r: &mut ArBuf) -> Binding {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Binding`

*Defined at line 1360*

---

## aw_pattern

```fides
pub fn aw_pattern(b: &mut AwBuf, p: Pattern) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `p`: `Pattern`

*Defined at line 1364*

---

## ar_pattern

```fides
pub fn ar_pattern(r: &mut ArBuf) -> Pattern {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Pattern`

*Defined at line 1404*

---

## aw_param

```fides
pub fn aw_param(b: &mut AwBuf, p: Param) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `p`: `Param`

*Defined at line 1436*

---

## ar_param

```fides
pub fn ar_param(r: &mut ArBuf) -> Param {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Param`

*Defined at line 1446*

---

## aw_params

```fides
pub fn aw_params(b: &mut AwBuf, v: Vec<Param>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<Param>`

*Defined at line 1455*

---

## ar_params

```fides
pub fn ar_params(r: &mut ArBuf) -> Vec<Param> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<Param>`

*Defined at line 1464*

---

## aw_optparams

```fides
pub fn aw_optparams(b: &mut AwBuf, v: OptParams) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `OptParams`

*Defined at line 1475*

---

## ar_optparams

```fides
pub fn ar_optparams(r: &mut ArBuf) -> OptParams {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `OptParams`

*Defined at line 1487*

---

## aw_selectcase

```fides
pub fn aw_selectcase(b: &mut AwBuf, c: SelectCase) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `c`: `SelectCase`

*Defined at line 1494*

---

## ar_selectcase

```fides
pub fn ar_selectcase(r: &mut ArBuf) -> SelectCase {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `SelectCase`

*Defined at line 1500*

---

## aw_effecthandler

```fides
pub fn aw_effecthandler(b: &mut AwBuf, h: EffectHandler) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `h`: `EffectHandler`

*Defined at line 1504*

---

## ar_effecthandler

```fides
pub fn ar_effecthandler(r: &mut ArBuf) -> EffectHandler {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `EffectHandler`

*Defined at line 1511*

---

## aw_selectcases

```fides
pub fn aw_selectcases(b: &mut AwBuf, v: Vec<SelectCase>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<SelectCase>`

*Defined at line 1515*

---

## ar_selectcases

```fides
pub fn ar_selectcases(r: &mut ArBuf) -> Vec<SelectCase> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<SelectCase>`

*Defined at line 1524*

---

## aw_effecthandlers

```fides
pub fn aw_effecthandlers(b: &mut AwBuf, v: Vec<EffectHandler>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<EffectHandler>`

*Defined at line 1535*

---

## ar_effecthandlers

```fides
pub fn ar_effecthandlers(r: &mut ArBuf) -> Vec<EffectHandler> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<EffectHandler>`

*Defined at line 1544*

---

## aw_stmt

```fides
pub fn aw_stmt(b: &mut AwBuf, s: Stmt) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `s`: `Stmt`

*Defined at line 1555*

---

## aw_stmts

```fides
pub fn aw_stmts(b: &mut AwBuf, v: Vec<Stmt>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<Stmt>`

*Defined at line 1765*

---

## ar_stmt

```fides
pub fn ar_stmt(r: &mut ArBuf) -> Stmt {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Stmt`

*Defined at line 1774*

---

## ar_stmts

```fides
pub fn ar_stmts(r: &mut ArBuf) -> Vec<Stmt> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<Stmt>`

*Defined at line 1920*

---

## aw_func

```fides
pub fn aw_func(b: &mut AwBuf, f: Func) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `f`: `Func`

*Defined at line 1933*

---

## ar_func

```fides
pub fn ar_func(r: &mut ArBuf) -> Func {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Func`

*Defined at line 1964*

---

## aw_funcs

```fides
pub fn aw_funcs(b: &mut AwBuf, v: Vec<Func>) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `Vec<Func>`

*Defined at line 1999*

---

## ar_funcs

```fides
pub fn ar_funcs(r: &mut ArBuf) -> Vec<Func> {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Vec<Func>`

*Defined at line 2008*

---

## aw_structdef

```fides
pub fn aw_structdef(b: &mut AwBuf, d: StructDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `StructDef`

*Defined at line 2019*

---

## ar_structdef

```fides
pub fn ar_structdef(r: &mut ArBuf) -> StructDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `StructDef`

*Defined at line 2027*

---

## aw_enumvariant

```fides
pub fn aw_enumvariant(b: &mut AwBuf, v: EnumVariant) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `v`: `EnumVariant`

*Defined at line 2036*

---

## ar_enumvariant

```fides
pub fn ar_enumvariant(r: &mut ArBuf) -> EnumVariant {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `EnumVariant`

*Defined at line 2041*

---

## aw_enumdef

```fides
pub fn aw_enumdef(b: &mut AwBuf, d: EnumDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `EnumDef`

*Defined at line 2045*

---

## ar_enumdef

```fides
pub fn ar_enumdef(r: &mut ArBuf) -> EnumDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `EnumDef`

*Defined at line 2057*

---

## aw_constdef

```fides
pub fn aw_constdef(b: &mut AwBuf, d: ConstDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ConstDef`

*Defined at line 2072*

---

## ar_constdef

```fides
pub fn ar_constdef(r: &mut ArBuf) -> ConstDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ConstDef`

*Defined at line 2079*

---

## aw_globalvar

```fides
pub fn aw_globalvar(b: &mut AwBuf, d: GlobalVar) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `GlobalVar`

*Defined at line 2083*

---

## ar_globalvar

```fides
pub fn ar_globalvar(r: &mut ArBuf) -> GlobalVar {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `GlobalVar`

*Defined at line 2090*

---

## aw_typealias

```fides
pub fn aw_typealias(b: &mut AwBuf, d: TypeAlias) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `TypeAlias`

*Defined at line 2094*

---

## ar_typealias

```fides
pub fn ar_typealias(r: &mut ArBuf) -> TypeAlias {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `TypeAlias`

*Defined at line 2100*

---

## aw_impldef

```fides
pub fn aw_impldef(b: &mut AwBuf, d: ImplDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ImplDef`

*Defined at line 2104*

---

## ar_impldef

```fides
pub fn ar_impldef(r: &mut ArBuf) -> ImplDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ImplDef`

*Defined at line 2110*

---

## aw_assoctypedecl

```fides
pub fn aw_assoctypedecl(b: &mut AwBuf, d: AssocTypeDecl) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `AssocTypeDecl`

*Defined at line 2114*

---

## ar_assoctypedecl

```fides
pub fn ar_assoctypedecl(r: &mut ArBuf) -> AssocTypeDecl {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `AssocTypeDecl`

*Defined at line 2119*

---

## aw_assoctypebinding

```fides
pub fn aw_assoctypebinding(b: &mut AwBuf, d: AssocTypeBinding) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `AssocTypeBinding`

*Defined at line 2123*

---

## ar_assoctypebinding

```fides
pub fn ar_assoctypebinding(r: &mut ArBuf) -> AssocTypeBinding {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `AssocTypeBinding`

*Defined at line 2128*

---

## aw_traitmethod

```fides
pub fn aw_traitmethod(b: &mut AwBuf, m: TraitMethod) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `m`: `TraitMethod`

*Defined at line 2132*

---

## ar_traitmethod

```fides
pub fn ar_traitmethod(r: &mut ArBuf) -> TraitMethod {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `TraitMethod`

*Defined at line 2153*

---

## aw_traitdef

```fides
pub fn aw_traitdef(b: &mut AwBuf, d: TraitDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `TraitDef`

*Defined at line 2172*

---

## ar_traitdef

```fides
pub fn ar_traitdef(r: &mut ArBuf) -> TraitDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `TraitDef`

*Defined at line 2190*

---

## aw_traitimpl

```fides
pub fn aw_traitimpl(b: &mut AwBuf, d: TraitImpl) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `TraitImpl`

*Defined at line 2213*

---

## ar_traitimpl

```fides
pub fn ar_traitimpl(r: &mut ArBuf) -> TraitImpl {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `TraitImpl`

*Defined at line 2226*

---

## aw_importdef

```fides
pub fn aw_importdef(b: &mut AwBuf, d: ImportDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ImportDef`

*Defined at line 2241*

---

## ar_importdef

```fides
pub fn ar_importdef(r: &mut ArBuf) -> ImportDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ImportDef`

*Defined at line 2251*

---

## aw_attribute

```fides
pub fn aw_attribute(b: &mut AwBuf, a: Attribute) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `a`: `Attribute`

*Defined at line 2255*

---

## ar_attribute

```fides
pub fn ar_attribute(r: &mut ArBuf) -> Attribute {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Attribute`

*Defined at line 2260*

---

## aw_externdecl

```fides
pub fn aw_externdecl(b: &mut AwBuf, d: ExternDecl) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ExternDecl`

*Defined at line 2264*

---

## ar_externdecl

```fides
pub fn ar_externdecl(r: &mut ArBuf) -> ExternDecl {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ExternDecl`

*Defined at line 2274*

---

## aw_layerinst

```fides
pub fn aw_layerinst(b: &mut AwBuf, d: LayerInst) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `LayerInst`

*Defined at line 2286*

---

## ar_layerinst

```fides
pub fn ar_layerinst(r: &mut ArBuf) -> LayerInst {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `LayerInst`

*Defined at line 2292*

---

## aw_layerdef

```fides
pub fn aw_layerdef(b: &mut AwBuf, d: LayerDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `LayerDef`

*Defined at line 2296*

---

## ar_layerdef

```fides
pub fn ar_layerdef(r: &mut ArBuf) -> LayerDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `LayerDef`

*Defined at line 2304*

---

## aw_modeldef

```fides
pub fn aw_modeldef(b: &mut AwBuf, d: ModelDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ModelDef`

*Defined at line 2313*

---

## ar_modeldef

```fides
pub fn ar_modeldef(r: &mut ArBuf) -> ModelDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ModelDef`

*Defined at line 2325*

---

## aw_componentdef

```fides
pub fn aw_componentdef(b: &mut AwBuf, d: ComponentDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `ComponentDef`

*Defined at line 2340*

---

## ar_componentdef

```fides
pub fn ar_componentdef(r: &mut ArBuf) -> ComponentDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `ComponentDef`

*Defined at line 2346*

---

## aw_systemdef

```fides
pub fn aw_systemdef(b: &mut AwBuf, d: SystemDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `SystemDef`

*Defined at line 2353*

---

## ar_systemdef

```fides
pub fn ar_systemdef(r: &mut ArBuf) -> SystemDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `SystemDef`

*Defined at line 2360*

---

## aw_statetransition

```fides
pub fn aw_statetransition(b: &mut AwBuf, d: StateTransition) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `StateTransition`

*Defined at line 2368*

---

## ar_statetransition

```fides
pub fn ar_statetransition(r: &mut ArBuf) -> StateTransition {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `StateTransition`

*Defined at line 2373*

---

## aw_statedef

```fides
pub fn aw_statedef(b: &mut AwBuf, d: StateDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `StateDef`

*Defined at line 2377*

---

## ar_statedef

```fides
pub fn ar_statedef(r: &mut ArBuf) -> StateDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `StateDef`

*Defined at line 2387*

---

## aw_statemachinedef

```fides
pub fn aw_statemachinedef(b: &mut AwBuf, d: StateMachineDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `StateMachineDef`

*Defined at line 2400*

---

## ar_statemachinedef

```fides
pub fn ar_statemachinedef(r: &mut ArBuf) -> StateMachineDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `StateMachineDef`

*Defined at line 2411*

---

## aw_eventhandlerdef

```fides
pub fn aw_eventhandlerdef(b: &mut AwBuf, d: EventHandlerDef) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `d`: `EventHandlerDef`

*Defined at line 2425*

---

## ar_eventhandlerdef

```fides
pub fn ar_eventhandlerdef(r: &mut ArBuf) -> EventHandlerDef {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `EventHandlerDef`

*Defined at line 2432*

---

## aw_program

```fides
pub fn aw_program(b: &mut AwBuf, p: Program) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `p`: `Program`

*Defined at line 2440*

---

## ar_program

```fides
pub fn ar_program(r: &mut ArBuf) -> Program {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `Program`

*Defined at line 2547*

---

## ast_ser_program

```fides
pub fn ast_ser_program(p: Program) -> Vec<i64> {
```

**Parameters:**

- `p`: `Program`

**Returns:** `Vec<i64>`

*Defined at line 2660*

---

## ast_deser_program

```fides
pub fn ast_deser_program(v: Vec<i64>) -> Program {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Program`

*Defined at line 2673*

---

## ast_deser_ok

```fides
pub fn ast_deser_ok(v: Vec<i64>) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 2690*

---

## awbuf_new

```fides
pub fn awbuf_new() -> AwBuf {
```

**Returns:** `AwBuf`

*Defined at line 2699*

---

## arbuf_new

```fides
pub fn arbuf_new(v: Vec<i64>) -> ArBuf {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `ArBuf`

*Defined at line 2704*

---

## arbuf_ok

```fides
pub fn arbuf_ok(r: &ArBuf) -> bool {
```

**Parameters:**

- `r`: `&ArBuf`

**Returns:** `bool`

*Defined at line 2709*

---

## awbuf_bytes

```fides
pub fn awbuf_bytes(b: &AwBuf) -> Vec<i64> {
```

**Parameters:**

- `b`: `&AwBuf`

**Returns:** `Vec<i64>`

*Defined at line 2712*

---

## ast_bytes_to_str

```fides
pub fn ast_bytes_to_str(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 2717*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
