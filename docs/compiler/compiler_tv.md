[All modules](index.md)

# compiler::tv -- Fides Documentation

## Contents

- [tv_on](#tv_on) (function)
- [tv_snapshot](#tv_snapshot) (function)
- [tv_before](#tv_before) (function)
- [tv_after](#tv_after) (function)
- [tv_report](#tv_report) (function)
- [tv_check](#tv_check) (function)
- [TvK](#TvK) (struct)
- [TvShape](#TvShape) (struct)
- [tv_note_globals](#tv_note_globals) (function)
- [tv_forget_globals](#tv_forget_globals) (function)
- [TvOut](#TvOut) (struct)
- [tv_random](#tv_random) (function)
- [tv_constk](#tv_constk) (function)
- [TvCtx](#TvCtx) (struct)
- [tv_structural](#tv_structural) (function)
- [TvPos](#TvPos) (struct)
- [tv_trust_call](#tv_trust_call) (function)
- [tv_licm](#tv_licm) (function)
- [tv_devirt](#tv_devirt) (function)
- [tv_after_devirt](#tv_after_devirt) (function)
- [TvSmt](#TvSmt) (struct)
- [TvSmtE](#TvSmtE) (struct)
- [tv_smt_text](#tv_smt_text) (function)
- [TvMod](#TvMod) (struct)
- [tv_mod_before](#tv_mod_before) (function)
- [TvWorld](#TvWorld) (struct)
- [TvRun](#TvRun) (struct)
- [TvFacts](#TvFacts) (struct)
- [tv_mod_after](#tv_mod_after) (function)
- [tv_smt_examples](#tv_smt_examples) (function)
- [tv_selftest](#tv_selftest) (function)
- [TvpP](#TvpP) (struct)
- [TvpPair](#TvpPair) (struct)
- [TvpArgs](#TvpArgs) (struct)
- [tv_is_module_pass](#tv_is_module_pass) (function)
- [tv_pair_check](#tv_pair_check) (function)
- [tv_pairs_text](#tv_pairs_text) (function)
- [tv_pairs_run](#tv_pairs_run) (function)

## tv_on

Is translation validation on (`--validate`, or FID_VALIDATE set and not 0)?

```fides
pub fn tv_on() -> bool {
```

**Returns:** `bool`

*Defined at line 93*

---

## tv_snapshot

A copy of `f` that the pass cannot change: every Vec a pass edits in
place is copied (instruction payloads are only ever replaced whole).

```fides
pub fn tv_snapshot(f: &IrFunction) -> IrFunction {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `IrFunction`

*Defined at line 119*

---

## tv_before

Before a pass: the function as it is when validation is on (an empty
function otherwise, which tv_after never reads).

```fides
pub fn tv_before(f: &IrFunction) -> IrFunction {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `IrFunction`

*Defined at line 175*

---

## tv_after

After pass `pass` changed `f` (from `pre`, a tv_before): true when the
result is accepted; false when it was rejected, and `f` is `pre` again.

```fides
pub fn tv_after(pname: str, pre: &IrFunction, f: &mut IrFunction) -> bool {
```

**Parameters:**

- `pname`: `str`
- `pre`: `&IrFunction`
- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 184*

---

## tv_report

The totals, once the mid-end is done (when validation is on); with
FID_TV_SELFTEST the self-test of the checks first (tv_selftest).

```fides
pub fn tv_report() {
```

*Defined at line 248*

---

## tv_check

Does `post` mean what `pre` meant, after pass `pass`? "" yes, "n/a" no
check applies, else why not.

```fides
pub fn tv_check(pname: str, pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `pname`: `str`
- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 288*

---

## TvK

A value, a panic, or a value this evaluator does not compute.

```fides
pub struct TvK {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `v` | `i64` |

*Defined at line 328*

---

## TvShape

Whether the evaluator can run `f`, its plain locals (allocas only loads
and stores touch, per value) and its blocks by id (-1: none).

```fides
pub struct TvShape {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `plain` | `Vec<i64>` |
| `bindex` | `Vec<i64>` |

*Defined at line 733*

---

## tv_note_globals

```fides
pub fn tv_note_globals(module: &IrModule) {
```

**Parameters:**

- `module`: `&IrModule`

*Defined at line 919*

---

## tv_forget_globals

```fides
pub fn tv_forget_globals() {
```

*Defined at line 933*

---

## TvOut

What one run did: kind 0 returned `val`, 1 panicked, 2 reached
Unreachable, 3 fell off a block, 4 ran out of steps, 5 met something it
does not evaluate; `trace` hashes the calls with effects and the prints.

```fides
pub struct TvOut {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `val` | `i64` |
| `trace` | `i64` |
| `steps` | `i64` |

*Defined at line 961*

---

## tv_random

RL5: run `pre` and `post` on the same inputs; "" when every run that
finished agrees, "n/a" when none could be run, else the first
disagreement.

```fides
pub fn tv_random(pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 1207*

---

## tv_constk

The value of `v` when it is a constant: a ConstInt or ConstBool, or an
operation on constants evaluated at its type. Kind 0 a value, 1 the
operation panics, 3 constants this evaluator does not compute, 2 not a
constant.

```fides
pub fn tv_constk(f: &IrFunction, v: i64, d: i64) -> TvK {
```

**Parameters:**

- `f`: `&IrFunction`
- `v`: `i64`
- `d`: `i64`

**Returns:** `TvK`

*Defined at line 1467*

---

## TvCtx

What one witness check knows: the pass, the plain locals of both
functions, where each instruction of the function before the pass is
(block index, position), its CFG, the comparisons left, and for each
value before the pass the value that stands for it after (+ 1).

```fides
pub struct TvCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `pname` | `str` |
| `plain_a` | `Vec<i64>` |
| `plain_b` | `Vec<i64>` |
| `blk_a` | `Vec<i64>` |
| `pos_a` | `Vec<i64>` |
| `cfg_a` | `IroCfg` |
| `budget` | `i64` |
| `rep` | `Vec<i64>` |

*Defined at line 1533*

---

## tv_structural

VF6: check the rewrites of a pass that keeps value ids (fold, dce, gvn,
ssa). "" accepted, "n/a" another pass, else what is wrong.

```fides
pub fn tv_structural(pname: str, pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `pname`: `str`
- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 1810*

---

## TvPos

Per value of a function (ids below `n`): the index of the block holding
it and its position there; -1 in no block, -2 in more than one place.

```fides
pub struct TvPos {
```

**Fields:**

| Name | Type |
|------|------|
| `blk` | `Vec<i64>` |
| `pos` | `Vec<i64>` |

*Defined at line 1966*

---

## tv_trust_call

Vouch for calls of `name` (kind 1 or 2, see g_tv_calls).

```fides
pub fn tv_trust_call(name: str, kind: i64) {
```

**Parameters:**

- `name`: `str`
- `kind`: `i64`

*Defined at line 2080*

---

## tv_licm

VF6: LICM's witness (iropt.fd iro_licm, OP9/OP25). The pass keeps every
value id and only moves instructions, each into the block that is the
way into a loop holding it (that block ends in a jump to the loop's
header). Checked: the same instructions on the same operands; in every
block those that stayed keep their order; and each one that moved may
run there: its operands come from outside the loop (or moved ahead of
it), and
- one that can neither panic nor be seen (tv_class 0) may come from any
block of the loop; a load of a local only if the loop never stores it;
- one that may panic (checked arithmetic, a call with no effect but a
panic, a load from memory nothing in the loop writes) only when it
ran, before the pass, first thing each time the loop was entered
(tv_licm_first);
- `len` of a Vec when nothing in the loop can resize one.

```fides
pub fn tv_licm(pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 2406*

---

## tv_devirt

VF6: devirtualization's witness (iropt.fd iro_devirt, WP16): a trait
call on a value made from a known type T is a direct call of T's method:
DynCall(trait, m, [MakeDyn(v, T, ..), a...]) -> Call(T_m, [0, v, a...]),
with the same result type, T_m a function of the module with that many
parameters, and the 0 (the env slot) a new constant just before it.
Nothing else may change.

```fides
pub fn tv_devirt(module: &IrModule, pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `module`: `&IrModule`
- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 2564*

---

## tv_after_devirt

tv_after for iro_devirt, which reads the module.

```fides
pub fn tv_after_devirt(module: &IrModule, pre: &IrFunction, f: &mut IrFunction) -> bool {
```

**Parameters:**

- `module`: `&IrModule`
- `pre`: `&IrFunction`
- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 2618*

---

## TvSmt

One function as SMT-LIB: the definitions of its values (named prefix +
id; parameter i is p<i>), what it returns, and when it panics. ok is
false when it is not one block of integer code without calls.

```fides
pub struct TvSmt {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `defs` | `str` |
| `ret` | `str` |
| `pan` | `str` |

*Defined at line 2896*

---

## TvSmtE

One operation: its term, and when it panics ("": never).

```fides
pub struct TvSmtE {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `term` | `str` |
| `cond` | `str` |

*Defined at line 2904*

---

## tv_smt_text

The SMT-LIB obligation that `post` means what `pre` meant: both as
bit-vector terms over the same parameters and the assertion that they
differ (in whether they panic, or in what they return when neither
does), so `unsat` proves them the same. `note` is the first line's
comment. "" when either is not one block of integer code without calls.

```fides
pub fn tv_smt_text(note: str, pre: &IrFunction, post: &IrFunction) -> str {
```

**Parameters:**

- `note`: `str`
- `pre`: `&IrFunction`
- `post`: `&IrFunction`

**Returns:** `str`

*Defined at line 3327*

---

## TvMod

The module before a module pass: a copy of every function, the hash of
each (ir_ser.fd mec_fn_hash), and where each name is (+ 1).

```fides
pub struct TvMod {
```

**Fields:**

| Name | Type |
|------|------|
| `on` | `bool` |
| `entries` | `Vec<IrFunctionEntry>` |
| `hashes` | `Vec<i64>` |
| `index` | `i64` |
| `has_main` | `bool` |

*Defined at line 3363*

---

## tv_mod_before

Before a module pass: the module as it is when validation is on (an
empty TvMod otherwise, which tv_mod_after does not read).

```fides
pub fn tv_mod_before(module: &IrModule) -> TvMod {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `TvMod`

*Defined at line 3393*

---

## TvWorld

The functions of one side of a module check: names, bodies, where each
name is (+ 1), whether each can be run (0 not known yet, 1 yes, 2 no)
and its shape, function pointers (tv_fnref_val, as a string) -> position
+ 1, and the constant globals (tv_gconst_facts).

```fides
pub struct TvWorld {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `fns` | `Vec<IrFunction>` |
| `index` | `i64` |
| `st` | `Vec<i64>` |
| `shapes` | `Vec<TvShape>` |
| `fref` | `i64` |
| `gc` | `i64` |

*Defined at line 3423*

---

## TvRun

What a run has done so far across the calls it followed: the trace of
effects, the calls with effects not followed, the steps, and the limit.

```fides
pub struct TvRun {
```

**Fields:**

| Name | Type |
|------|------|
| `trace` | `i64` |
| `ncalls` | `i64` |
| `steps` | `i64` |
| `limit` | `i64` |

*Defined at line 3585*

---

## TvFacts

WP13's fact, found here again: for each function that only direct calls
reach (not named by a function pointer, a string, a dyn method or a C
callback, not main, not one the runtime or the backends call by name),
per parameter the constant every call passes. at: name -> slot + 1; a
slot holds per parameter its state (1 one constant, 2 not) and value.

```fides
pub struct TvFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `at` | `i64` |
| `slots` | `Vec<Vec<i64>>` |

*Defined at line 3931*

---

## tv_mod_after

After module pass `pname` changed `module` (from `pre`, a tv_mod_before):
true when every function it changed is accepted; false when one was
rejected, and the module has its functions from before the pass again.

```fides
pub fn tv_mod_after(pname: str, pre: &TvMod, module: &mut IrModule) -> bool {
```

**Parameters:**

- `pname`: `str`
- `pre`: `&TvMod`
- `module`: `&mut IrModule`

**Returns:** `bool`

*Defined at line 4848*

---

## tv_smt_examples

VF6 (SMT for straight-line code): example pairs written as SMT-LIB to
`dir` (tests/tooling/tv-check.sh runs z3 on them and compares each
answer with the one its first line expects): rewrites a pass may make
(unsat) and mistakes (sat), the overflow and signedness traps among
them. The number written.

```fides
pub fn tv_smt_examples(dir: str) -> i64 {
```

**Parameters:**

- `dir`: `str`

**Returns:** `i64`

*Defined at line 5492*

---

## tv_selftest

Run every case: the number not as expected (each is printed).

```fides
pub fn tv_selftest() -> i64 {
```

**Returns:** `i64`

*Defined at line 5560*

---

## TvpP

```fides
pub struct TvpP {
```

**Fields:**

| Name | Type |
|------|------|
| `lines` | `Vec<str>` |
| `at` | `i64` |
| `err` | `str` |

*Defined at line 5641*

---

## TvpPair

```fides
pub struct TvpPair {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `pname` | `str` |
| `expect` | `str` |
| `why` | `str` |
| `check` | `str` |
| `an` | `Vec<str>` |
| `af` | `Vec<IrFunction>` |
| `bn` | `Vec<str>` |
| `bf` | `Vec<IrFunction>` |
| `globals` | `Vec<GlobalEntry>` |
| `side` | `i64` |

*Defined at line 5647*

---

## TvpArgs

The arguments (`%N` words) and effects (`!name` words) of a call, from
word `from` on.

```fides
pub struct TvpArgs {
```

**Fields:**

| Name | Type |
|------|------|
| `args` | `Vec<i64>` |
| `eff` | `EffectSet` |

*Defined at line 5901*

---

## tv_is_module_pass

Is `pname` a module pass (checked by tv_mod_after in the mid-end)?

```fides
pub fn tv_is_module_pass(pname: str) -> bool {
```

**Parameters:**

- `pname`: `str`

**Returns:** `bool`

*Defined at line 6271*

---

## tv_pair_check

RL5: the verdict the mid-end gives on function `fname` after pass
`pname`, for a module whose functions were `an`/`af` (and globals
`globals`) before the pass and are `bn`/`bf` after it: "" accepted,
"n/a" no check applies, else why it is rejected. The same checks as
tv_after (function passes), tv_after_devirt and tv_mod_after (module
passes) make.

```fides
pub fn tv_pair_check(pname: str, an: Vec<str>, af: Vec<IrFunction>, bn: Vec<str>, bf: Vec<IrFunction>, globals: Vec<GlobalEntry>, fname: str) -> str {
```

**Parameters:**

- `pname`: `str`
- `an`: `Vec<str>`
- `af`: `Vec<IrFunction>`
- `bn`: `Vec<str>`
- `bf`: `Vec<IrFunction>`
- `globals`: `Vec<GlobalEntry>`
- `fname`: `str`

**Returns:** `str`

*Defined at line 6281*

---

## tv_pairs_text

Run the pairs of file text `text` (named `src` in the report).

```fides
pub fn tv_pairs_text(src: str, text: str, each: bool, counts: &mut Vec<i64>) {
```

**Parameters:**

- `src`: `str`
- `text`: `str`
- `each`: `bool`
- `counts`: `&mut Vec<i64>`

*Defined at line 6373*

---

## tv_pairs_run

Run the pairs of `path`: a .tvp file, or every .tvp file of a
directory. Prints "tv pairs: N pairs, M as expected" on stderr (with
`each` a line per pair before it) and returns the number not as
expected (a file that cannot be read counts as one).

```fides
pub fn tv_pairs_run(path: str, each: bool) -> i64 {
```

**Parameters:**

- `path`: `str`
- `each`: `bool`

**Returns:** `i64`

*Defined at line 6445*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
