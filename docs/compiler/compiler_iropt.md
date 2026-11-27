[All modules](index.md)

# compiler::iropt -- Fides Documentation

## Contents

- [iropt_set_time_on](#iropt_set_time_on) (function)
- [iropt_pass_time_names](#iropt_pass_time_names) (function)
- [iropt_pass_time_ms](#iropt_pass_time_ms) (function)
- [iropt_native](#iropt_native) (function)
- [IroImc](#IroImc) (struct)
- [iro_mid_reach](#iro_mid_reach) (function)
- [iro_in_reach](#iro_in_reach) (function)
- [iro_inl_sites_reset](#iro_inl_sites_reset) (function)
- [IroBody](#IroBody) (struct)
- [iropt_set_remarks](#iropt_set_remarks) (function)
- [iropt_clear_remarks](#iropt_clear_remarks) (function)
- [iropt_inline_remarks](#iropt_inline_remarks) (function)
- [iro_inline](#iro_inline) (function)
- [iro_inline_in](#iro_inline_in) (function)
- [IroUses](#IroUses) (struct)
- [iro_uses](#iro_uses) (function)
- [iro_uses_builds](#iro_uses_builds) (function)
- [iro_uses_reused](#iro_uses_reused) (function)
- [iro_uses_arm](#iro_uses_arm) (function)
- [iro_uses_disarm](#iro_uses_disarm) (function)
- [iro_uses_forget](#iro_uses_forget) (function)
- [iro_ipcp](#iro_ipcp) (function)
- [iro_hof_specialize](#iro_hof_specialize) (function)
- [iro_hof_specialize_in](#iro_hof_specialize_in) (function)
- [iro_dyn_specialize](#iro_dyn_specialize) (function)
- [iro_field_devirt](#iro_field_devirt) (function)
- [iro_const_globals](#iro_const_globals) (function)
- [iro_const_globals_in](#iro_const_globals_in) (function)
- [iro_conv_mark](#iro_conv_mark) (function)
- [iro_env_trim](#iro_env_trim) (function)
- [iro_specialize](#iro_specialize) (function)
- [iro_eff_bit](#iro_eff_bit) (function)
- [iro_eff_mask](#iro_eff_mask) (function)
- [iro_mask_effects](#iro_mask_effects) (function)
- [iro_mask_pure](#iro_mask_pure) (function)
- [iro_mask_read_only](#iro_mask_read_only) (function)
- [iro_mask_no_write](#iro_mask_no_write) (function)
- [iro_mem_base](#iro_mem_base) (function)
- [iro_esc_of](#iro_esc_of) (function)
- [iro_local_addr](#iro_local_addr) (function)
- [iro_absent_call_mask](#iro_absent_call_mask) (function)
- [iro_effect_masks](#iro_effect_masks) (function)
- [iro_effect_masks_in](#iro_effect_masks_in) (function)
- [iro_call_mask](#iro_call_mask) (function)
- [iro_quiet_fns](#iro_quiet_fns) (function)
- [iro_quiet_fns_in](#iro_quiet_fns_in) (function)
- [iro_parse_effects_field](#iro_parse_effects_field) (function)
- [iro_lib_mask](#iro_lib_mask) (function)
- [iro_libfacts](#iro_libfacts) (function)
- [iro_fold_identical](#iro_fold_identical) (function)
- [iro_explain_loops](#iro_explain_loops) (function)

## iropt_set_time_on

Keep iropt's per-pass times for the --time table.

```fides
pub fn iropt_set_time_on() {
```

*Defined at line 53*

---

## iropt_pass_time_names

The passes timed by the last iropt_native (with --time), in run order.

```fides
pub fn iropt_pass_time_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 58*

---

## iropt_pass_time_ms

Their milliseconds, in the same order.

```fides
pub fn iropt_pass_time_ms() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 63*

---

## iropt_native

Run every pass on `module` (the pass manager: see iro_pass_on).

```fides
pub fn iropt_native(module: &mut IrModule) {
```

**Parameters:**

- `module`: `&mut IrModule`

*Defined at line 68*

---

## IroImc

What the memo knows about the module before the loop, by position: the
content hash before the passes (0: not yet; a function the loop does not
run is hashed when a key needs it, its IR is the same then), the
positions of the functions it names, a hash of the names it uses that
the module does not define, and 1 when the loop runs the passes on it.

```fides
pub struct IroImc {
```

**Fields:**

| Name | Type |
|------|------|
| `pre_h` | `Vec<i64>` |
| `refs` | `Vec<Vec<i64>>` |
| `ext` | `Vec<i64>` |
| `runs` | `Vec<i64>` |
| `mark` | `Vec<i64>` |
| `gen` | `i64` |
| `budget` | `i64` |

*Defined at line 862*

---

## iro_mid_reach

SP3: the functions the mid-end works on: a HashMap of the names main
reaches (pe_ir_reachable_functions), or 0 (every function) when the
module has no main or FID_IROPT_OFF names `midreach`.

```fides
pub fn iro_mid_reach(module: &IrModule) -> i64 {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `i64`

*Defined at line 1222*

---

## iro_in_reach

SP3: is `name` one of the functions `reach` (iro_mid_reach) holds?
Every function when `reach` is 0.

```fides
pub fn iro_in_reach(reach: i64, name: str) -> bool {
```

**Parameters:**

- `reach`: `i64`
- `name`: `str`

**Returns:** `bool`

*Defined at line 1231*

---

## iro_inl_sites_reset

BG243: forget the records (a new module is about to be optimized).

```fides
pub fn iro_inl_sites_reset() {
```

*Defined at line 1358*

---

## IroBody

The instructions of `callee` to copy (its Return excluded) and the value
it returns, when it can be inlined: one block, at most iro_inline_limit
plain instructions, a scalar result. ret = -1 when it cannot.

```fides
pub struct IroBody {
```

**Fields:**

| Name | Type |
|------|------|
| `insts` | `Vec<i64>` |
| `ret` | `i64` |

*Defined at line 1591*

---

## iropt_set_remarks

Record why calls are not inlined (main.fd, for --remarks).

```fides
pub fn iropt_set_remarks() {
```

*Defined at line 1682*

---

## iropt_clear_remarks

SP18: stop recording remarks (main.fd serve_reset_backend_state, so a
`--remarks` request of `fid serve` does not leave them on for the next).

```fides
pub fn iropt_clear_remarks() {
```

*Defined at line 1688*

---

## iropt_inline_remarks

The inlining remarks of the last iropt_native whose caller's name
contains `filt` ("" = all).

```fides
pub fn iropt_inline_remarks(filt: str) -> Vec<str> {
```

**Parameters:**

- `filt`: `str`

**Returns:** `Vec<str>`

*Defined at line 1694*

---

## iro_inline

Inline small functions at their call sites, in every function.

```fides
pub fn iro_inline(module: &mut IrModule) {
```

**Parameters:**

- `module`: `&mut IrModule`

*Defined at line 1797*

---

## iro_inline_in

SP3: iro_inline into the functions `reach` (iro_mid_reach) holds only;
`reach` 0: every function.

```fides
pub fn iro_inline_in(module: &mut IrModule, reach: i64) {
```

**Parameters:**

- `module`: `&mut IrModule`
- `reach`: `i64`

*Defined at line 1803*

---

## IroUses

ME1: the use lists of a function. `count` is how many operands read
each value, `users` the instructions that read it, `block_of` the block
(index) each instruction is in, `pos_of` its place there (-1: in none).
Built in one pass over the blocks; a pass that changes instructions
rebuilds it (iro_uses) or keeps it current with iro_uses_drop.

```fides
pub struct IroUses {
```

**Fields:**

| Name | Type |
|------|------|
| `count` | `Vec<i64>` |
| `users` | `Vec<Vec<i64>>` |
| `block_of` | `Vec<i64>` |
| `pos_of` | `Vec<i64>` |

*Defined at line 3685*

---

## iro_uses

```fides
pub fn iro_uses(f: &IrFunction) -> IroUses {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `IroUses`

*Defined at line 3692*

---

## iro_uses_builds

How many use lists `iro_uses` built, and how many `iro_uses_entry`
handed back from the cache (FID_IROPT_STATS).

```fides
pub fn iro_uses_builds() -> i64 {
```

**Returns:** `i64`

*Defined at line 3820*

---

## iro_uses_reused

```fides
pub fn iro_uses_reused() -> i64 {
```

**Returns:** `i64`

*Defined at line 3824*

---

## iro_uses_arm

The pass manager starts on a function: nothing cached yet, and
iro_uses_entry may cache (unless FID_IROPT_OFF=cfgcache).

```fides
pub fn iro_uses_arm() {
```

*Defined at line 3830*

---

## iro_uses_disarm

The pass manager is done with its functions: every request builds again.

```fides
pub fn iro_uses_disarm() {
```

*Defined at line 3836*

---

## iro_uses_forget

A pass changed the function: the cached use lists no longer hold.

```fides
pub fn iro_uses_forget() {
```

*Defined at line 3921*

---

## iro_ipcp

WP13: a parameter every call site passes the same constant for is replaced
by that constant inside the callee. One pass collects per-candidate
constant arguments (flattened, at most 8 parameters); a second pass emits
the constants into each candidate's entry block and renames the uses.

```fides
pub fn iro_ipcp(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 6045*

---

## iro_hof_specialize

LG5 (full): `v.map(fn ..)` hands the closure env to a higher-order
function that then CallPtr-dispatches on the env's name string per
element. When the caller's env is a locally built, statically known
closure, clone the HOF once per (hof, closure target) and rewrite the
CallPtr on that parameter to a direct Call — after inlining the clone
is the plain loop LG5 wants. The generic HOF stays for callers whose
env is not statically known.

```fides
pub fn iro_hof_specialize(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 6889*

---

## iro_hof_specialize_in

SP3: iro_hof_specialize for the calls in the functions `reach`
(iro_mid_reach) holds; `reach` 0: every function. The exclusions are
still collected over every function. Name sets are tables, and only a
function that calls one with a CallPtr is looked at closely (its use
lists built): the same clones and rewrites as a scan of everything,
without a list search per call or use lists for every function.

```fides
pub fn iro_hof_specialize_in(module: &mut IrModule, reach: i64) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`
- `reach`: `i64`

**Returns:** `i64`

*Defined at line 6899*

---

## iro_dyn_specialize

OP14 (Nov 16): a call that passes `x as dyn T` (directly, or through a
local stored once just before) to a function that makes trait calls on
that parameter calls a clone of the function instead,
`callee__dyn<pos>_<T>`, which takes `x` as one more parameter (last):
in the clone each of those trait calls is a direct call to `T_method`
with `x` as the receiver (what the type dispatch would pick: the dyn
object holds T's name and `x`). The dyn value is still passed, for the
clone's other uses. Same exclusions as iro_hof_specialize: never a
runtime, extern, name-called, address-taken or trait-target function;
callees of at most 200 instructions. One clone per (callee, position,
T); the original stays for every other caller. FID_IROPT_OFF=dynspec.

```fides
pub fn iro_dyn_specialize(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 7256*

---

## iro_field_devirt

OP14 (Nov 17): a trait call or a closure call on a value read from a
struct field, where every store of that field in the module stores
the same kind of thing. A field all of whose stores store `x as dyn T`
(one T) holds a dyn value of type T wherever it is read, so `h.sh.m()`
(h a parameter, a Vec element, anything typed as the struct; directly
or through a local stored once with the read) is a direct call to
`T_m` with the receiver read from the dyn value (its word at +8, what
the backends' dispatch passes). A field all of whose stores store a
closure of one target F (an env Lowerer_make_fn_env built: a named
function or a lambda) makes a call through a read of it a direct call
to F with that env. A store that copies the field from the same field
of another value keeps the fact; any other store loses it, as does a
field address used other than to load or store (passed to a call,
`&mut h.sh`), a struct a global holds, and, for the field's index, a
dyn or closure store or opaque field address on a pointer of no known
struct. `counts` gets the dyn calls ([0]) and closure calls ([1]) made
direct; returns their sum. FID_IROPT_OFF=fielddevirt.

```fides
pub fn iro_field_devirt(module: &mut IrModule, counts: &mut Vec<i64>) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`
- `counts`: `&mut Vec<i64>`

**Returns:** `i64`

*Defined at line 7915*

---

## iro_const_globals

WP18: a global with a constant initializer whose address is only ever
loaded — never stored through and never escaping — has its loads folded
to the initializer. cval per global: 0 not an int constant, -1 excluded.

```fides
pub fn iro_const_globals(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 8020*

---

## iro_const_globals_in

SP3: iro_const_globals folding loads in the functions `reach`
(iro_mid_reach) holds only (`reach` 0: every function); which globals
are never written is still decided over every function. A function
that takes the address of no candidate global costs one scan, no tables.

```fides
pub fn iro_const_globals_in(module: &mut IrModule, reach: i64) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`
- `reach`: `i64`

**Returns:** `i64`

*Defined at line 8028*

---

## iro_conv_mark

WP6: mark every function that may use the internal calling convention.
The predicate is the same one env-trim uses for safety: not address-taken,
not a dyn trait target, not an extern or C callback, not a runtime
stand-in — functions only ever reached through direct Inst::Call sites,
which all read the same flag when picking a register count. Runs
unconditionally (unlike env trimming): the flag only selects a wider
arg-register set in the backend; it changes no signatures.

```fides
pub fn iro_conv_mark(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 8265*

---

## iro_env_trim

WP7: drop the synthetic __env parameter (params[0]) from functions that
never read it, and the matching args[0] at every direct call to them.
Address-taken functions (ConstFnRef), trait-impl targets of DynCall,
externs, C callbacks and runtime helpers keep the uniform convention —
CallPtr always passes an environment.

```fides
pub fn iro_env_trim(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 8396*

---

## iro_specialize

WP14: specialize functions on the constant arguments some — not all —
call sites pass. Params constant at every site are IPCP's job; here we
clone per observed constant value and redirect just those sites, for
the values whose call sites are hot (iro_spec_hottest). Only call sites
in functions `reach` holds (what main reaches, a table of names) count;
every clone made is added to `reach`, so the per-function passes
optimize it with its constant.

```fides
pub fn iro_specialize(module: &mut IrModule, reach: i64) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`
- `reach`: `i64`

**Returns:** `i64`

*Defined at line 8623*

---

## iro_eff_bit

The bit of one effect in an effect mask.

```fides
pub fn iro_eff_bit(e: Effect) -> i64 {
```

**Parameters:**

- `e`: `Effect`

**Returns:** `i64`

*Defined at line 8938*

---

## iro_eff_mask

The mask of an effect set.

```fides
pub fn iro_eff_mask(s: &EffectSet) -> i64 {
```

**Parameters:**

- `s`: `&EffectSet`

**Returns:** `i64`

*Defined at line 8975*

---

## iro_mask_effects

The effect set of a mask, in the order effect_to_str's names are listed.

```fides
pub fn iro_mask_effects(m: i64) -> EffectSet {
```

**Parameters:**

- `m`: `i64`

**Returns:** `EffectSet`

*Defined at line 8986*

---

## iro_mask_pure

No effects at all: two calls with the same arguments give the same value.

```fides
pub fn iro_mask_pure(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 9022*

---

## iro_mask_read_only

Only reads (EffectSet_is_read_only): the result depends on the arguments
and on memory, and changes nothing.

```fides
pub fn iro_mask_read_only(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 9028*

---

## iro_mask_no_write

Changes no memory another call could read: reads, a panic, or nothing.
Such a call between two read-only calls does not stop their merging.

```fides
pub fn iro_mask_no_write(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 9034*

---

## iro_mem_base

The memory a value addresses: its id chased back through field and
element pointers and casts (an Alloca id for a local's address; the
value itself for anything else).

```fides
pub fn iro_mem_base(f: &IrFunction, id0: i64) -> i64 {
```

**Parameters:**

- `f`: `&IrFunction`
- `id0`: `i64`

**Returns:** `i64`

*Defined at line 9041*

---

## iro_esc_of

The escape set of `f`, per value id (1: escaped): the Allocas whose
address leaves the function: call arguments, stored values, struct
copies, persisted values, returns. Loads and stores through any other
Alloca touch only the function's own frame.

```fides
pub fn iro_esc_of(f: &IrFunction) -> Vec<i64> {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `Vec<i64>`

*Defined at line 9145*

---

## iro_local_addr

Is `p` an address into a local of `f` that nothing outside it can see?

```fides
pub fn iro_local_addr(f: &IrFunction, esc: &Vec<i64>, p: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `esc`: `&Vec<i64>`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 9195*

---

## iro_absent_call_mask

The effect mask of a call to `name`, a function the module does not
define, with the call site's own effects `site`: a runtime builtin is
what lowering said it is (plus a write when that is more than reading);
a library function is what its facts file says (WP20); anything else may
write.

```fides
pub fn iro_absent_call_mask(name: str, site: &EffectSet) -> i64 {
```

**Parameters:**

- `name`: `str`
- `site`: `&EffectSet`

**Returns:** `i64`

*Defined at line 9212*

---

## iro_effect_masks

The effect mask a caller sees for every function of `module`, by
position in `module.functions`: own effects joined with every callee's,
transitively. Callees are visited first (post-order over the call
graph), so an acyclic program settles in one round and the next round
confirms it; recursion takes a round per step around a cycle.

```fides
pub fn iro_effect_masks(module: &IrModule) -> Vec<i64> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<i64>`

*Defined at line 9309*

---

## iro_effect_masks_in

SP3: iro_effect_masks for the functions `reach` (iro_mid_reach) holds
and their callees, transitively (iro_call_closure): the same masks as
over the whole module for those; IRO_EB_ALL for the others, which no
function of the set calls. `reach` 0: every function.

```fides
pub fn iro_effect_masks_in(module: &IrModule, reach: i64) -> Vec<i64> {
```

**Parameters:**

- `module`: `&IrModule`
- `reach`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 9321*

---

## iro_call_mask

The effect mask of a call to `name` with call-site effects `site`, given
the module's masks (iro_effect_masks): a module function's transitive
mask, or iro_absent_call_mask.

```fides
pub fn iro_call_mask(module: &IrModule, masks: &Vec<i64>, name: str, site: &EffectSet) -> i64 {
```

**Parameters:**

- `module`: `&IrModule`
- `masks`: `&Vec<i64>`
- `name`: `str`
- `site`: `&EffectSet`

**Returns:** `i64`

*Defined at line 9406*

---

## iro_quiet_fns

OP25: per function of `module` (by position), 1 when every call of it
ends without a panic: its own code does (iro_quiet_local) and so does
every function it calls. A least fixpoint: a function counts only once
all its callees do, so recursion is never proved (a call cycle could run
for ever, or overflow the stack). What it reads or writes is not asked
here: iro_effect_masks says that.

```fides
pub fn iro_quiet_fns(module: &IrModule) -> Vec<i64> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<i64>`

*Defined at line 9637*

---

## iro_quiet_fns_in

SP3: iro_quiet_fns for the functions `reach` (iro_mid_reach) holds and
their callees, transitively (iro_call_closure): the same answer as over
the whole module for those, 0 (not proved) for the others, which no
function of the set calls. `reach` 0: every function.

```fides
pub fn iro_quiet_fns_in(module: &IrModule, reach: i64) -> Vec<i64> {
```

**Parameters:**

- `module`: `&IrModule`
- `reach`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 9645*

---

## iro_parse_effects_field

The mask of an `effects=` field: `pure`, or `!` and names joined by
commas (EffectSet_to_str, as iro_libfacts writes it).

```fides
pub fn iro_parse_effects_field(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 9832*

---

## iro_lib_mask

The effect mask the facts file gives function `name`, or -1 when there
is no file or it does not list `name`.

```fides
pub fn iro_lib_mask(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 9921*

---

## iro_libfacts

WP20: library-facts artifact: with FID_LIBFACTS=<path>, write one line
per optimized function: name, parameter count, the effects its callers
must assume (iro_effect_masks: transitive, in EffectSet_to_str's form,
which iro_lib_load reads back) and annotation flags, for consumers that
build against the library without whole-program mode. The machine layer
appends its `ext` lines (clobbers, sizes) to the same file.

```fides
pub fn iro_libfacts(module: &IrModule) {
```

**Parameters:**

- `module`: `&IrModule`

*Defined at line 9938*

---

## iro_fold_identical

WP19: send every direct call to a function whose IR is the same as an
earlier function's to that earlier one. Returns the functions folded.
Only functions that share a cheap signature (instruction, block,
parameter and value counts) with another are serialized.

```fides
pub fn iro_fold_identical(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 10053*

---

## iro_explain_loops

ME25/ME23: for `fid explain` (CP3), which reads the IR as lowered: each
function's loops versioned (when `version` is on: the default since
Nov 17) and rotated (when `rotate` is), as iropt_native leaves them, so
the report lists a versioned copy's checks as removed (counted loop)
and the original loop's as kept. Returns how many loops were versioned.

```fides
pub fn iro_explain_loops(module: &mut IrModule) -> i64 {
```

**Parameters:**

- `module`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 10740*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
