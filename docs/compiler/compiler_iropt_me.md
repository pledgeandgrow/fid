[All modules](index.md)

# compiler::iropt_me -- Fides Documentation

## Contents

- [ime_on](#ime_on) (function)
- [ime_trace](#ime_trace) (function)
- [ime_inline_stats_line](#ime_inline_stats_line) (function)
- [ime_inline](#ime_inline) (function)
- [ime_inline_reached](#ime_inline_reached) (function)
- [ime_inline_closures](#ime_inline_closures) (function)
- [ime_canonical_loops](#ime_canonical_loops) (function)
- [ime_strength_reduce](#ime_strength_reduce) (function)
- [ime_loop_stats_line](#ime_loop_stats_line) (function)
- [ime_unroll](#ime_unroll) (function)
- [ime_rotate_count](#ime_rotate_count) (function)
- [ime_rotate](#ime_rotate) (function)
- [ime_sra](#ime_sra) (function)
- [ImeCase](#ImeCase) (struct)
- [ime_switch](#ime_switch) (function)
- [ime_rotate_pass](#ime_rotate_pass) (function)
- [ime_function_passes](#ime_function_passes) (function)
- [ime_set_call_facts](#ime_set_call_facts) (function)
- [ime_guardcall_count](#ime_guardcall_count) (function)
- [ime_nowrite_count](#ime_nowrite_count) (function)
- [ime_licm_guard](#ime_licm_guard) (function)
- [ime_licmguard_count](#ime_licmguard_count) (function)
- [ime_vecdata_count](#ime_vecdata_count) (function)
- [ime_vec_data](#ime_vec_data) (function)
- [ImeAddr](#ImeAddr) (struct)
- [ime_unescaped](#ime_unescaped) (function)
- [ime_forget_ref_params](#ime_forget_ref_params) (function)
- [ime_note_ref_params](#ime_note_ref_params) (function)
- [ime_may_alias](#ime_may_alias) (function)
- [ime_vecalias_count](#ime_vecalias_count) (function)
- [ime_overwritten_stores](#ime_overwritten_stores) (function)
- [ime_hoist_loads](#ime_hoist_loads) (function)
- [ime_forward](#ime_forward) (function)
- [ime_match_facts](#ime_match_facts) (function)

## ime_on

Is mid-end pass `name` on? Only when FID_IROPT_ON names it (or "all")
and FID_IROPT_OFF does not. The passes of IME_DEFAULT_ON are on unless
FID_IROPT_OFF names them.

```fides
pub fn ime_on(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 76*

---

## ime_trace

```fides
pub fn ime_trace(pass: str, fn_name: str) {
```

**Parameters:**

- `pass`: `str`
- `fn_name`: `str`

*Defined at line 1014*

---

## ime_inline_stats_line

ME20/ME21: inline2's line of FID_IROPT_STATS (read by
tests/tooling/inline2-tune.sh and inline2-limits-check.sh).

```fides
pub fn ime_inline_stats_line() -> str {
```

**Returns:** `str`

*Defined at line 1117*

---

## ime_inline

ME20, ME21: inline across the module, callees before their callers
(post-order over the call graph), so a callee's size is its size after
its own inlining.

```fides
pub fn ime_inline(module: &mut IrModule) {
```

**Parameters:**

- `module`: `&mut IrModule`

*Defined at line 1308*

---

## ime_inline_reached

ME20: ime_inline into the functions named in `reach` (a HashMap from
name; 0: every function) only: a function main never reaches is not
worth growing (iropt optimizes only what main reaches, CS1).

```fides
pub fn ime_inline_reached(module: &mut IrModule, reach: i64) {
```

**Parameters:**

- `module`: `&mut IrModule`
- `reach`: `i64`

*Defined at line 1315*

---

## ime_inline_closures

WP16: the once-called rule of ime_inline for closures alone: a closure
(`__closure_N`) that closure devirtualization made a direct call, called
at one site in the module, is copied into its caller up to
IME_INLINE_ONCE_MAX instructions. A step towards inline2 by default
(ME20): the closure's own body is all that moves (FID_IROPT_ON=inlineclo).

```fides
pub fn ime_inline_closures(module: &mut IrModule) {
```

**Parameters:**

- `module`: `&mut IrModule`

*Defined at line 1324*

---

## ime_canonical_loops

Give every loop a preheader, one latch and dedicated exits. True when
anything changed (the CFG is then recomputed by the caller).

```fides
pub fn ime_canonical_loops(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 1451*

---

## ime_strength_reduce

In an @wrapping function (where i * c and its stepped copy wrap alike),
a product `load(i) * c` in a loop whose only store to i is
`i = i + step` becomes a load of a second local j kept equal to i * c:
j = i * c in the preheader, j = j + step * c after each step.
OP10 (Nov 17): outside @wrapping code too, when the loop's test bounds
i by a constant and i starts at one (checks.fd iro_iv_bounded, the
proof iropt.fd's `iv` uses): then no value j takes can overflow, so j
traps nowhere the multiply would not. FID_IROPT_OFF=ivbound keeps it to
@wrapping functions. Exit values of the loop's variable are ssaexit's
(compiler/ssa.fd, on by default since OP2).

```fides
pub fn ime_strength_reduce(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 1619*

---

## ime_loop_stats_line

The line FID_IROPT_STATS prints for OP10/OP11 (compiler/iropt.fd);
tests/tooling/ivsr-check.sh and unroll-check.sh read it.

```fides
pub fn ime_loop_stats_line() -> str {
```

**Returns:** `str`

*Defined at line 1874*

---

## ime_unroll

A loop of one header and one body block, `while i < C { body; i = i +
s }` with i a constant c0 when the loop is entered, runs a known number
of times; when that is at most IME_UNROLL_TRIPS (and the copies stay
small) the body is copied that many times and the loop goes. A longer
loop whose trip count is a multiple of 4 or 2 keeps its header and gets
that many copies of its body per trip around it (partial unrolling). The
header's test only reads (loads and a comparison), so leaving it out
changes nothing; every operation of the body runs as before, in order.

```fides
pub fn ime_unroll(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 1892*

---

## ime_rotate_count

How many loops `rotate` rotated in this build.

```fides
pub fn ime_rotate_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 2498*

---

## ime_rotate

ME23: rotate the loops of `f` (see above). True when one changed.

```fides
pub fn ime_rotate(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 2554*

---

## ime_sra

A struct local whose every use is a field address used only by loads
and stores becomes one local per field: each field address is replaced
by that field's own local, and the struct's allocation goes.

```fides
pub fn ime_sra(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 2958*

---

## ImeCase

The value `x == k` compares (a load of a plain local counts as that
local: -2 - slot) and k; ok false when `c` is not such a comparison.

```fides
pub struct ImeCase {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |
| `k` | `i64` |
| `ok` | `bool` |

*Defined at line 3128*

---

## ime_switch

A chain of tests `x == k1 -> T1 else x == k2 -> T2 else ... -> D` on one
value becomes a balanced tree of `x < k` tests ending in `x == k`
tests: log2(n) comparisons instead of n.

```fides
pub fn ime_switch(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 3214*

---

## ime_rotate_pass

ME23: loop rotation, run by iropt_native last on each function (after
licm and versioning, which want the test in the header); with
--validate the result is checked (compiler/tv.fd).

```fides
pub fn ime_rotate_pass(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 3408*

---

## ime_function_passes

```fides
pub fn ime_function_passes(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 3412*

---

## ime_set_call_facts

OP25: the two sets above (iropt_native, before the per-function passes;
0 for either: nothing known).

```fides
pub fn ime_set_call_facts(pure_calls: i64, nowrite_calls: i64) {
```

**Parameters:**

- `pure_calls`: `i64`
- `nowrite_calls`: `i64`

*Defined at line 3629*

---

## ime_guardcall_count

OP25: how many calls ime_licm_guard moved, and how many times a no-write
call kept a memory fact (FID_IROPT_STATS; tests/tooling/op25-check.sh).

```fides
pub fn ime_guardcall_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 3654*

---

## ime_nowrite_count

```fides
pub fn ime_nowrite_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 3658*

---

## ime_licm_guard

The guard transform above, loop by loop (the CFG is computed again
after each change). True when anything moved.

```fides
pub fn ime_licm_guard(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 3664*

---

## ime_licmguard_count

```fides
pub fn ime_licmguard_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 3934*

---

## ime_vecdata_count

```fides
pub fn ime_vecdata_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 3938*

---

## ime_vec_data

The pass above over every loop of `f`. A read is changed for the
innermost loop that holds it: that loop's preheader jumps straight to
its header, and the proof read v's length there or before the loop, so
v is a live Vec when P loads its word 0. True when a read changed.

```fides
pub fn ime_vec_data(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 4230*

---

## ImeAddr

Where an address comes from: kind 1 a local, 2 a parameter written
through (`&mut`, or unmarked and not `*u8`), 4 a `&` parameter, 3 a
global, 5 an element of a Vec<i64>'s buffer (OP26, LF3: `base` the owner
of its header, ime_vb_header_owner; FID_IROPT_OFF=vecalias: never), 0
unknown; `base` the local or parameter (the global's GlobalAddr id);
`path` the fields and constant elements from it ("f2.e5.", "e?." for an
element not known).

```fides
pub struct ImeAddr {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `base` | `i64` |
| `path` | `str` |

*Defined at line 4538*

---

## ime_unescaped

Locals whose address goes nowhere but loads, stores (as the address)
and field or element addresses that do the same: 1. A local whose
address also goes to reference parameters of direct calls that never
let it go (LF2, ime_lends_ref) is 2: only a call it is passed to can
read or write it, the reference ends with that call.

```fides
pub fn ime_unescaped(f: &IrFunction) -> Vec<i64> {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `Vec<i64>`

*Defined at line 4688*

---

## ime_forget_ref_params

LF2 off (or another module): nothing is lent.

```fides
pub fn ime_forget_ref_params() {
```

*Defined at line 4895*

---

## ime_note_ref_params

LF2: fill g_ime_ref_params from the module (whole program: the callee
of a direct call is the module's function of that name). Run once
before the per-function passes (iropt_native).

```fides
pub fn ime_note_ref_params(module: &IrModule) {
```

**Parameters:**

- `module`: `&IrModule`

*Defined at line 4902*

---

## ime_may_alias

LF4: may the places at addresses p and q overlap?

```fides
pub fn ime_may_alias(f: &IrFunction, unesc: &Vec<i64>, p: i64, q: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `unesc`: `&Vec<i64>`
- `p`: `i64`
- `q`: `i64`

**Returns:** `bool`

*Defined at line 5123*

---

## ime_vecalias_count

OP26: element reads of a Vec<i64> that ime_forward reused, since the
compiler started (FID_IROPT_STATS; tests/tooling/vecalias-check.sh).

```fides
pub fn ime_vecalias_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 5687*

---

## ime_overwritten_stores

LF4 in DSE (iro_dse calls it): in a block, a store to a local's or a
written (`&mut`) parameter's place that a later store in the block
overwrites whole (the same place, the same type), with nothing between
that may read it (ime_reads) or leave the block, is dead. Locals the
backend reads by itself (drops, @zeroize, @persist: LF19) and globals
are left alone. The stored values stay; only the stores go.

```fides
pub fn ime_overwritten_stores(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 6157*

---

## ime_hoist_loads

In a loop with a preheader, a load whose address is computed before the
loop and whose place nothing in the loop may write (LF4) moves to the
preheader: it reads the same value on every iteration.

```fides
pub fn ime_hoist_loads(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 6291*

---

## ime_forward

In each block, a load of a place whose value is known (just stored, or
loaded and not written since, per LF4) becomes that value. Only values
of the load's own type are forwarded.

```fides
pub fn ime_forward(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 6426*

---

## ime_match_facts

Where `Branch(load(tag) == k, T, E)` enters T (T's only predecessor),
every load of that same address in the blocks T dominates reads k, as
long as nothing in that area may write it (LF4): it becomes the
constant, so a later test of the same tag folds.

```fides
pub fn ime_match_facts(f: &mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 6573*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
