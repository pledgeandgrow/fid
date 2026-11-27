[All modules](index.md)

# compiler::checks -- Fides Documentation

## Contents

- [peir_zeros](#peir_zeros) (function)
- [PeirLoopFacts](#PeirLoopFacts) (struct)
- [peir_inst_of](#peir_inst_of) (function)
- [peir_inst_pos](#peir_inst_pos) (function)
- [peir_load_of](#peir_load_of) (function)
- [peir_may_shorten](#peir_may_shorten) (function)
- [peir_same_vec](#peir_same_vec) (function)
- [peir_unchecked_ops](#peir_unchecked_ops) (function)
- [peir_is_len](#peir_is_len) (function)
- [peir_reach](#peir_reach) (function)
- [peir_add_edge](#peir_add_edge) (function)
- [peir_loop_facts](#peir_loop_facts) (function)
- [peir_counted_loop](#peir_counted_loop) (function)
- [peir_guard_ops](#peir_guard_ops) (function)
- [peir_inst_operands](#peir_inst_operands) (function)
- [PeirRanges](#PeirRanges) (struct)
- [PeirDomFacts](#PeirDomFacts) (struct)
- [PeirBackFacts](#PeirBackFacts) (struct)
- [peir_range_ops](#peir_range_ops) (function)
- [peir_prm_clear](#peir_prm_clear) (function)
- [peir_prm_wanted](#peir_prm_wanted) (function)
- [peir_note_param_ranges](#peir_note_param_ranges) (function)
- [explain_checks](#explain_checks) (function)
- [explain_allocs](#explain_allocs) (function)
- [explain_clone_reason](#explain_clone_reason) (function)
- [IroCfg](#IroCfg) (struct)
- [iro_cache_on](#iro_cache_on) (function)
- [iro_cfg_builds](#iro_cfg_builds) (function)
- [iro_cfg_reused](#iro_cfg_reused) (function)
- [iro_cfg](#iro_cfg) (function)
- [iro_dominates](#iro_dominates) (function)
- [IroLoops](#IroLoops) (struct)
- [iro_loop_body](#iro_loop_body) (function)
- [iro_iv_bounded](#iro_iv_bounded) (function)
- [iro_loops_builds](#iro_loops_builds) (function)
- [iro_loops_reused](#iro_loops_reused) (function)
- [iro_loops](#iro_loops) (function)
- [iro_cli_off](#iro_cli_off) (function)
- [iro_named_off](#iro_named_off) (function)
- [ct_source_param](#ct_source_param) (function)
- [peir_ct_check](#peir_ct_check) (function)

## peir_zeros

```fides
pub fn peir_zeros(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 18*

---

## PeirLoopFacts

The values of one function a counted loop looks at (peir_unchecked_ops).

```fides
pub struct PeirLoopFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `block_of` | `Vec<i64>` |
| `at` | `Vec<i64>` |
| `plain` | `Vec<i64>` |
| `st_ptr` | `Vec<i64>` |
| `st_block` | `Vec<i64>` |
| `st_at` | `Vec<i64>` |
| `st_value` | `Vec<i64>` |

*Defined at line 29*

---

## peir_inst_of

```fides
pub fn peir_inst_of(f: &IrFunction, id: i64) -> Inst {
```

**Parameters:**

- `f`: `&IrFunction`
- `id`: `i64`

**Returns:** `Inst`

*Defined at line 43*

---

## peir_inst_pos

The position of `id`'s instruction in f.insts, or -1 (a parameter, a
missing id). Hot loops match `f.insts.get(pos).inst` in place instead of
copying the instruction twice through peir_inst_of (CS4).

```fides
pub fn peir_inst_pos(f: &IrFunction, id: i64) -> i64 {
```

**Parameters:**

- `f`: `&IrFunction`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 60*

---

## peir_load_of

```fides
pub fn peir_load_of(f: &IrFunction, id: i64, ptr: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `id`: `i64`
- `ptr`: `i64`

**Returns:** `bool`

*Defined at line 67*

---

## peir_may_shorten

```fides
pub fn peir_may_shorten(f: &IrFunction, id: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 79*

---

## peir_same_vec

```fides
pub fn peir_same_vec(f: &IrFunction, a: i64, b: i64, body: Vec<i64>, facts: PeirLoopFacts) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `a`: `i64`
- `b`: `i64`
- `body`: `Vec<i64>`
- `facts`: `PeirLoopFacts`

**Returns:** `bool`

*Defined at line 104*

---

## peir_unchecked_ops

As src/pe.rs unchecked_ops: the checks a counted loop makes needless. In
`while i < n`, with `i` an unshared local that only the increment
`i = i + 1` changes in the loop, that increment cannot overflow. If also `n`
is `v.len()` (or a local set once to it before the loop), `i` starts at a
constant >= 0 and nothing between that `len` and the loop's end can shorten
`v`, then `v.get(i)` and `v.set(i, x)` are in bounds, when `i` is read
before the increment. Since Nov 16 (ME25) the increment may be `i = i +
c` for a constant c in 1..2^20: the accesses are proved as for 1, and
the increment then needs no overflow check only when the bound is a
length. Returns a flag per value. The C backend uses it too.
Per value: bit 1, an operation that needs no overflow or bounds check;
bit 2, a value known to be >= 0.

```fides
pub fn peir_unchecked_ops(f: &IrFunction, nvals: i64) -> Vec<i64> {
```

**Parameters:**

- `f`: `&IrFunction`
- `nvals`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 160*

---

## peir_is_len

```fides
pub fn peir_is_len(f: &IrFunction, id: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 193*

---

## peir_reach

The blocks outside the loop body reached from `start` along `edges`.

```fides
pub fn peir_reach(edges: Vec<Vec<i64>>, start: Vec<i64>, body: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `edges`: `Vec<Vec<i64>>`
- `start`: `Vec<i64>`
- `body`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 205*

---

## peir_add_edge

```fides
pub fn peir_add_edge(succs: &mut Vec<Vec<i64>>, preds: &mut Vec<Vec<i64>>, index: Vec<i64>, from: i64, target: i64) {
```

**Parameters:**

- `succs`: `&mut Vec<Vec<i64>>`
- `preds`: `&mut Vec<Vec<i64>>`
- `index`: `Vec<i64>`
- `from`: `i64`
- `target`: `i64`

*Defined at line 228*

---

## peir_loop_facts

```fides
pub fn peir_loop_facts(f: &IrFunction, nvals: i64) -> PeirLoopFacts {
```

**Parameters:**

- `f`: `&IrFunction`
- `nvals`: `i64`

**Returns:** `PeirLoopFacts`

*Defined at line 235*

---

## peir_counted_loop

`lbody`: the loop's blocks (flags per block index), the shared loop
tree's body of the loop headed by `head` (iro_loop_body, ME2).

```fides
pub fn peir_counted_loop(f: &IrFunction, head: i64, n: i64, succs: Vec<Vec<i64>>, preds: Vec<Vec<i64>>, lbody: Vec<i64>, index: Vec<i64>, facts: PeirLoopFacts, out: &mut Vec<i64>) {
```

**Parameters:**

- `f`: `&IrFunction`
- `head`: `i64`
- `n`: `i64`
- `succs`: `Vec<Vec<i64>>`
- `preds`: `Vec<Vec<i64>>`
- `lbody`: `Vec<i64>`
- `index`: `Vec<i64>`
- `facts`: `PeirLoopFacts`
- `out`: `&mut Vec<i64>`

*Defined at line 665*

---

## peir_guard_ops

```fides
pub fn peir_guard_ops(f: &IrFunction, n: i64, succs: Vec<Vec<i64>>, preds: Vec<Vec<i64>>, index: Vec<i64>, facts: PeirLoopFacts, out: &mut Vec<i64>) {
```

**Parameters:**

- `f`: `&IrFunction`
- `n`: `i64`
- `succs`: `Vec<Vec<i64>>`
- `preds`: `Vec<Vec<i64>>`
- `index`: `Vec<i64>`
- `facts`: `PeirLoopFacts`
- `out`: `&mut Vec<i64>`

*Defined at line 2136*

---

## peir_inst_operands

Generate integer binary operation.
Append every ValueId `inst` reads to `out` (used by the slot-reuse
analysis in gen_function — pe-roadmap items 84-85).

```fides
pub fn peir_inst_operands(inst: Inst, out: &mut Vec<i64>) -> void {
```

**Parameters:**

- `inst`: `Inst`
- `out`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 2363*

---

## PeirRanges

Ranges of one function: per value lo and hi, per local its bounds, and
the refinement in force in the block being scanned.

```fides
pub struct PeirRanges {
```

**Fields:**

| Name | Type |
|------|------|
| `lo` | `Vec<i64>` |
| `hi` | `Vec<i64>` |
| `loc_lo` | `Vec<i64>` |
| `loc_hi` | `Vec<i64>` |
| `plain` | `Vec<i64>` |
| `ref_v` | `i64` |
| `ref_loc` | `i64` |
| `ref_lo` | `i64` |
| `ref_hi` | `i64` |
| `ref_live` | `bool` |
| `vstable` | `Vec<i64>` |
| `vlen` | `Vec<i64>` |
| `cf_v` | `Vec<i64>` |
| `cf_lo` | `Vec<i64>` |
| `cf_hi` | `Vec<i64>` |
| `sf_on` | `bool` |
| `sf_loc` | `Vec<i64>` |
| `sf_lo` | `Vec<i64>` |
| `sf_hi` | `Vec<i64>` |
| `def_b` | `Vec<i64>` |
| `plain_cfg` | `bool` |

*Defined at line 2580*

---

## PeirDomFacts

```fides
pub struct PeirDomFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `on` | `bool` |
| `n` | `i64` |
| `idom` | `Vec<i64>` |
| `succs` | `Vec<Vec<i64>>` |
| `e_loc` | `Vec<i64>` |
| `e_lo` | `Vec<i64>` |
| `e_hi` | `Vec<i64>` |
| `st_head` | `Vec<i64>` |
| `st_next` | `Vec<i64>` |
| `st_b` | `Vec<i64>` |
| `st_k` | `Vec<i64>` |
| `rd` | `Vec<i64>` |
| `rs` | `Vec<i64>` |
| `rm` | `Vec<Vec<i64>>` |
| `budget` | `i64` |
| `e_loc2` | `Vec<i64>` |
| `e_lo2` | `Vec<i64>` |
| `e_hi2` | `Vec<i64>` |
| `fv_head` | `Vec<i64>` |
| `fv_next` | `Vec<i64>` |
| `fv_v` | `Vec<i64>` |
| `fv_lo` | `Vec<i64>` |
| `fv_hi` | `Vec<i64>` |

*Defined at line 4132*

---

## PeirBackFacts

```fides
pub struct PeirBackFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `on` | `bool` |
| `heads` | `Vec<i64>` |
| `bodies` | `Vec<Vec<i64>>` |
| `h_loc` | `Vec<i64>` |
| `h_lo` | `Vec<i64>` |
| `h_hi` | `Vec<i64>` |
| `h_grow` | `Vec<i64>` |
| `h_nar` | `Vec<i64>` |
| `tried_h` | `Vec<i64>` |
| `tried_p` | `Vec<i64>` |
| `multi` | `bool` |
| `is_head` | `Vec<i64>` |
| `m_h` | `Vec<i64>` |
| `m_p` | `Vec<i64>` |
| `m_lo` | `Vec<i64>` |
| `m_hi` | `Vec<i64>` |
| `m_grow` | `Vec<i64>` |
| `m_nar` | `Vec<i64>` |

*Defined at line 4411*

---

## peir_range_ops

```fides
pub fn peir_range_ops(f: &IrFunction, nvals: i64, out: &mut Vec<i64>) {
```

**Parameters:**

- `f`: `&IrFunction`
- `nvals`: `i64`
- `out`: `&mut Vec<i64>`

*Defined at line 4798*

---

## peir_prm_clear

LF10: drop every parameter fact.

```fides
pub fn peir_prm_clear() {
```

*Defined at line 5402*

---

## peir_prm_wanted

LF10: `paramrange` is on unless FID_IROPT_OFF names it.

```fides
pub fn peir_prm_wanted() -> bool {
```

**Returns:** `bool`

*Defined at line 5415*

---

## peir_note_param_ranges

LF10: fill the parameter facts for the whole program `module` (none
without a `main`). Returns how many parameters got a range.

```fides
pub fn peir_note_param_ranges(module: &IrModule) -> i64 {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `i64`

*Defined at line 5634*

---

## explain_checks

Per function: checks removed (with the proof that removed them) vs kept
(DB8: each with what the range analysis knew about its operands).
`filt` ("" = all) keeps only functions whose name contains it.

```fides
pub fn explain_checks(module: IrModule, filt: str) -> Vec<str> {
```

**Parameters:**

- `module`: `IrModule`
- `filt`: `str`

**Returns:** `Vec<str>`

*Defined at line 5840*

---

## explain_allocs

Per function: the instructions that allocate, and why the value is on the
heap. `filt` ("" = all) keeps only functions whose name contains it.

```fides
pub fn explain_allocs(module: IrModule, filt: str) -> Vec<str> {
```

**Parameters:**

- `module`: `IrModule`
- `filt`: `str`

**Returns:** `Vec<str>`

*Defined at line 6008*

---

## explain_clone_reason

CP4: for the instruction at index `k` of block `b`, when it is a second
owner the lowering inserted: "line N: clone required here because ...";
"" for any other instruction.

```fides
pub fn explain_clone_reason(f: &IrFunction, b: i64, k: i64) -> str {
```

**Parameters:**

- `f`: `&IrFunction`
- `b`: `i64`
- `k`: `i64`

**Returns:** `str`

*Defined at line 6345*

---

## IroCfg

A function's control flow: block id -> index, successors and
predecessors by index, reverse postorder of the reachable blocks, and
immediate dominators (the entry's is itself; -1 unreachable).

```fides
pub struct IroCfg {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `index` | `Vec<i64>` |
| `succs` | `Vec<Vec<i64>>` |
| `preds` | `Vec<Vec<i64>>` |
| `rpo` | `Vec<i64>` |
| `rpo_num` | `Vec<i64>` |
| `idom` | `Vec<i64>` |
| `tin` | `Vec<i64>` |
| `tout` | `Vec<i64>` |
| `gen` | `i64` |

*Defined at line 6396*

---

## iro_cache_on

OP3: are the mid-end's analyses cached (the CFG with its dominator tree
and loop tree here, the use lists in iropt.fd)? false with
FID_IROPT_OFF=cfgcache, which turns every one of these caches off.

```fides
pub fn iro_cache_on() -> bool {
```

**Returns:** `bool`

*Defined at line 6440*

---

## iro_cfg_builds

How many CFGs `iro_cfg` built, and how many it handed back unchanged.

```fides
pub fn iro_cfg_builds() -> i64 {
```

**Returns:** `i64`

*Defined at line 6451*

---

## iro_cfg_reused

```fides
pub fn iro_cfg_reused() -> i64 {
```

**Returns:** `i64`

*Defined at line 6455*

---

## iro_cfg

```fides
pub fn iro_cfg(f: &IrFunction) -> IroCfg {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `IroCfg`

*Defined at line 6516*

---

## iro_dominates

Does block `a` dominate block `b` (indexes)? O(1): the dominator
tree's DFS interval of `a` covers `b`'s.

```fides
pub fn iro_dominates(cfg: &IroCfg, a: i64, b: i64) -> bool {
```

**Parameters:**

- `cfg`: `&IroCfg`
- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 6746*

---

## IroLoops

The natural loops of a function, from its shared CFG (iro_cfg): a loop
is a header h with back edges b -> h (h dominates b); its body is h and
every reachable block that reaches a latch without passing through h.
Back edges to one header make one loop.

```fides
pub struct IroLoops {
```

**Fields:**

| Name | Type |
|------|------|
| `headers` | `Vec<i64>` |
| `latches` | `Vec<Vec<i64>>` |
| `head_of` | `Vec<i64>` |
| `parent` | `Vec<i64>` |
| `depth` | `Vec<i64>` |
| `found` | `Vec<i64>` |
| `bodies` | `Vec<Vec<i64>>` |
| `body_at` | `Vec<i64>` |

*Defined at line 6761*

---

## iro_loop_body

The body (flags per block index) of the loop headed by block index `h`
in `lt`; all zeros when `h` heads no loop.

```fides
pub fn iro_loop_body(lt: &IroLoops, h: i64) -> Vec<i64> {
```

**Parameters:**

- `lt`: `&IroLoops`
- `h`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 6788*

---

## iro_iv_bounded

OP10: may a shadow local j, set to L * c at the end of the preheader
`pre` and bumped by c * d right after the loop's one store to L
(`L = L + d`, in block index `sb`), be kept in code whose arithmetic
traps on overflow? It may when no value j ever holds overflows: L's
value on entry is a constant c0 (the last store to it on the way to
`pre`), the header `head` tests L against a constant K on its way out
(`L < K`, `L <= K` for d > 0; `>`, `>=` for d < 0, either branch order)
before any store to L in the iteration (the store is not in the
header), so L takes values between c0 and K -/+ 1 + d only; and the
magnitudes (c0, K at most 2^40, c and d at most 2^20) keep L * c for
each of them, c * d, and the original products, inside i64. Then the
shadow never traps, and neither did the multiply it replaces on any
iteration that ran: the shadow computes nothing the program could not.

```fides
pub fn iro_iv_bounded(f: &IrFunction, cfg: &IroCfg, body: Vec<i64>, head: i64, pre: i64, loc: i64, d: i64, sb: i64, c: i64) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`
- `cfg`: `&IroCfg`
- `body`: `Vec<i64>`
- `head`: `i64`
- `pre`: `i64`
- `loc`: `i64`
- `d`: `i64`
- `sb`: `i64`
- `c`: `i64`

**Returns:** `bool`

*Defined at line 6845*

---

## iro_loops_builds

How many loop trees `iro_loops` found, and how many it handed back from
its cache (FID_IROPT_STATS).

```fides
pub fn iro_loops_builds() -> i64 {
```

**Returns:** `i64`

*Defined at line 7032*

---

## iro_loops_reused

```fides
pub fn iro_loops_reused() -> i64 {
```

**Returns:** `i64`

*Defined at line 7036*

---

## iro_loops

The loop tree of the function `cfg` describes. Computed on demand (it
costs one walk per loop), so passes without loops do not pay for it;
the last one found is kept with its CFG's `gen` (OP3).

```fides
pub fn iro_loops(cfg: &IroCfg) -> IroLoops {
```

**Parameters:**

- `cfg`: `&IroCfg`

**Returns:** `IroLoops`

*Defined at line 7043*

---

## iro_cli_off

The passes `--no-pass=a,b` (or `--no-pass a,b`) names on the command
line, as ",a,b" to append to an off list (FID_IROPT_OFF, FID_NAT_OFF);
"" when there is none. iropt, iropt_me and the e-graph read it where
they read those variables, so the flag works for every command that
reaches the mid-end with no parsing in main.fd (its flag lookups skip
what they do not know).

```fides
pub fn iro_cli_off() -> str {
```

**Returns:** `str`

*Defined at line 7199*

---

## iro_named_off

Is pass `name` named by the off list `env` (a comma list, may be null or
empty) or by `--no-pass`?

```fides
pub fn iro_named_off(env: str, name: str) -> bool {
```

**Parameters:**

- `env`: `str`
- `name`: `str`

**Returns:** `bool`

*Defined at line 7224*

---

## ct_source_param

CF32: the index parameter `pi` of the lowered `func` has in its source
function (-1: none: it is the hidden result pointer of a function that
returns a struct or an array, or the environment). Lowering pushes the
result pointer, then the environment (the local "__env", which the
native paths drop from the functions that never read it, iro_env_trim),
then the source's parameters; the type checker puts @public down by the
source's index. Before Nov 27 the checks took the IR's index for it:
right after the trim, one too far on the C path, which checks the IR
as lowered (the marks of `pick(@public flag, a)` were looked up for
`a`).

```fides
pub fn ct_source_param(func: &IrFunction, pi: i64) -> i64 {
```

**Parameters:**

- `func`: `&IrFunction`
- `pi`: `i64`

**Returns:** `i64`

*Defined at line 7415*

---

## peir_ct_check

The violations of @constant_time function `func` (named `name`), one
message each; empty when it is clean. See the section comment above.

```fides
pub fn peir_ct_check(func: &IrFunction, name: str) -> Vec<str> {
```

**Parameters:**

- `func`: `&IrFunction`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 7448*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
