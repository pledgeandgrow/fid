[All modules](index.md)

# compiler::nat_ir -- Fides Documentation

## Contents

- [NatGen](#NatGen) (struct)
- [NatGen_new](#NatGen_new) (function)
- [NatTarget](#NatTarget) (struct)
- [NgDom](#NgDom) (struct)
- [NatFnRec](#NatFnRec) (struct)
- [NatAlloc](#NatAlloc) (struct)
- [nat_seg_unit](#nat_seg_unit) (function)
- [nat_seg_designed](#nat_seg_designed) (function)
- [NgPhys](#NgPhys) (struct)
- [nat_arghint_unit](#nat_arghint_unit) (function)
- [nat_arghint_check_unit](#nat_arghint_check_unit) (function)
- [nat_argfree_unit](#nat_argfree_unit) (function)
- [NgSplit](#NgSplit) (struct)
- [NgLsState](#NgLsState) (struct)
- [nat_split_unit](#nat_split_unit) (function)
- [nat_split_unit_mir](#nat_split_unit_mir) (function)
- [nat_split_unit_drop_mir](#nat_split_unit_drop_mir) (function)
- [nat_split_unit_drop](#nat_split_unit_drop) (function)
- [nat_split_alloc_unit_err](#nat_split_alloc_unit_err) (function)
- [nat_split_alloc_unit_why](#nat_split_alloc_unit_why) (function)
- [nat_split_alloc_unit](#nat_split_alloc_unit) (function)
- [nat_greedy_unit](#nat_greedy_unit) (function)
- [nat_vlive_unit_check](#nat_vlive_unit_check) (function)
- [nat_retfwd_unit_check](#nat_retfwd_unit_check) (function)
- [nat_set_time_on](#nat_set_time_on) (function)
- [nat_time_names](#nat_time_names) (function)
- [nat_time_ms](#nat_time_ms) (function)
- [nat_time_clear](#nat_time_clear) (function)
- [NatParState](#NatParState) (struct)
- [NatParBuf](#NatParBuf) (struct)
- [ng_par_thread](#ng_par_thread) (function)
- [nat_vsel_check](#nat_vsel_check) (function)
- [NgVz](#NgVz) (struct)
- [nat_pie_wanted](#nat_pie_wanted) (function)
- [nat_target_arch](#nat_target_arch) (function)
- [nat_cpu_default](#nat_cpu_default) (function)
- [nat_cpu_check](#nat_cpu_check) (function)
- [nat_select_mir](#nat_select_mir) (function)
- [nat_generate_elf](#nat_generate_elf) (function)
- [nat_generate_pe](#nat_generate_pe) (function)
- [nat_generate_macho](#nat_generate_macho) (function)
- [nat_generate_macho_universal](#nat_generate_macho_universal) (function)
- [NatLdyn](#NatLdyn) (struct)
- [ng_dbg_shown_pairs](#ng_dbg_shown_pairs) (function)
- [nat_dbg_scope_unit](#nat_dbg_scope_unit) (function)
- [ng_dbg_split_note](#ng_dbg_split_note) (function)
- [NcFix](#NcFix) (struct)
- [NcEntry](#NcEntry) (struct)
- [nat_fncache_source](#nat_fncache_source) (function)
- [nat_fncache_reemitted](#nat_fncache_reemitted) (function)
- [nat_relink_start](#nat_relink_start) (function)
- [nat_relink_counts](#nat_relink_counts) (function)
- [nat_emit_object](#nat_emit_object) (function)
- [nat_memop_unit_check](#nat_memop_unit_check) (function)
- [nat_tg78_unit_check](#nat_tg78_unit_check) (function)
- [nat_sel_check](#nat_sel_check) (function)
- [nat_sel_check_more](#nat_sel_check_more) (function)
- [nat_sel_check_more_part](#nat_sel_check_more_part) (function)
- [nat_isel_mem_check](#nat_isel_mem_check) (function)
- [nat_isel_tg1b_check](#nat_isel_tg1b_check) (function)
- [nat_isel_rule_check](#nat_isel_rule_check) (function)

## NatGen

```fides
pub struct NatGen {
```

**Fields:**

| Name | Type |
|------|------|
| `arch` | `i64` |
| `e` | `NatEnc` |
| `fn_labels` | `i64` |
| `ct_labels` | `i64` |
| `all_fns` | `i64` |
| `queue` | `Vec<str>` |
| `queued` | `i64` |
| `str_labels` | `i64` |
| `strs` | `Vec<str>` |
| `glob_labels` | `i64` |
| `globs` | `Vec<str>` |
| `panic_labels` | `Vec<i64>` |
| `start_sp_label` | `i64` |
| `entry_label` | `i64` |
| `unresolved` | `Vec<str>` |
| `unresolved_seen` | `i64` |
| `dyn_types` | `Vec<str>` |
| `named_fns` | `Vec<str>` |
| `named_seen` | `i64` |
| `fn_table_label` | `i64` |
| `c_names` | `i64` |
| `c_list` | `Vec<str>` |
| `tramp_label` | `i64` |
| `va_off` | `i64` |
| `vreg` | `Vec<i64>` |
| `save_regs` | `Vec<i64>` |
| `save_off` | `i64` |
| `names_label` | `i64` |
| `code_end_label` | `i64` |
| `placed_fns` | `Vec<str>` |
| `rec` | `bool` |
| `mir` | `Vec<MInst>` |
| `nvreg` | `i64` |
| `promoted` | `Vec<i64>` |
| `frame` | `i64` |
| `recs` | `Vec<NatFnRec>` |
| `label_fn` | `i64` |
| `fn_mask` | `i64` |
| `n_folded` | `i64` |
| `n_peep` | `i64` |
| `cur_name` | `str` |
| `cur_wrapping` | `bool` |
| `cur_crashfree` | `bool` |
| `td` | `NatTarget` |
| `cf_fns` | `i64` |
| `ld` | `NatLdyn` |
| `leaf` | `i64` |
| `wipe` | `i64` |
| `rr_size` | `i64` |
| `internal` | `i64` |
| `fact_path` | `str` |
| `lib_mask` | `i64` |
| `slots` | `Vec<i64>` |
| `alloca_data` | `Vec<i64>` |
| `block_labels` | `i64` |
| `next_block_label` | `i64` |
| `t0` | `i64` |
| `t1` | `i64` |
| `t2` | `i64` |
| `t3` | `i64` |
| `ad` | `i64` |
| `ad2` | `i64` |
| `zr` | `i64` |
| `arg0` | `i64` |
| `win` | `bool` |
| `mac` | `bool` |
| `mac_got_label` | `i64` |
| `win_slot_labels` | `Vec<i64>` |
| `win_slots_label` | `i64` |
| `win_tls_label` | `i64` |
| `win_imp_label` | `i64` |
| `win_imp_end` | `i64` |
| `win_iat_label` | `i64` |
| `win_iat_end` | `i64` |
| `line_fns` | `i64` |
| `lrows` | `Vec<i64>` |
| `lrow_open` | `bool` |
| `lines_cur_fn` | `bool` |
| `cur_line` | `i64` |
| `cfi_rows` | `Vec<i64>` |
| `unchecked` | `Vec<i64>` |
| `cur_inst_line` | `i64` |
| `pl_keys` | `i64` |
| `pl_list` | `Vec<i64>` |
| `x0_reg` | `i64` |
| `x1_reg` | `i64` |
| `br_short` | `bool` |
| `pie_table` | `i64` |
| `nc_on` | `bool` |
| `nc_path` | `str` |
| `nc_old` | `Vec<NcEntry>` |
| `nc_ix` | `i64` |
| `nc_new` | `Vec<NcEntry>` |
| `nc_hit` | `i64` |
| `nc_key` | `i64` |
| `nc_salt` | `i64` |
| `label_id` | `i64` |
| `fn_label_names` | `Vec<str>` |
| `pl_names` | `Vec<str>` |
| `nc_hit_names` | `Vec<str>` |
| `sw_pro` | `i64` |
| `sw_ep` | `i64` |
| `ldfwd_n` | `i64` |
| `dbg_mpos` | `Vec<i64>` |
| `dbg_mark_on` | `bool` |
| `tc_taint` | `Vec<i64>` |
| `par_w` | `bool` |
| `line_fix` | `i64` |
| `par_log` | `str` |
| `wipe_on` | `bool` |
| `wipe_regs` | `i64` |
| `wipe_keep1` | `bool` |

*Defined at line 52*

---

## NatGen_new

```fides
pub fn NatGen_new(arch: i64) -> NatGen {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `NatGen`

*Defined at line 305*

---

## NatTarget

```fides
pub struct NatTarget {
```

**Fields:**

| Name | Type |
|------|------|
| `arch` | `i64` |
| `int_caller` | `Vec<i64>` |
| `int_callee` | `Vec<i64>` |
| `int_caller_mask` | `i64` |
| `int_callee_mask` | `i64` |
| `float_regs` | `Vec<i64>` |
| `float_pool` | `Vec<i64>` |
| `float_scratch` | `Vec<i64>` |
| `float_class` | `i64` |
| `pairs` | `bool` |
| `argregs` | `Vec<i64>` |
| `nregargs` | `i64` |
| `nregargs_int` | `i64` |
| `result_reg` | `i64` |
| `spill_use0` | `i64` |
| `spill_use1` | `i64` |
| `spill_def` | `i64` |
| `t0` | `i64` |
| `t1` | `i64` |
| `t2` | `i64` |
| `t3` | `i64` |
| `ad` | `i64` |
| `ad2` | `i64` |
| `zr` | `i64` |
| `arg0` | `i64` |
| `stack_align` | `i64` |
| `red_zone` | `i64` |

*Defined at line 345*

---

## NgDom

Dominators of a graph whose nodes have two successors at most (RA12,
RA19), entered at node 0: idom (-1: not reachable; the entry is its
own), rnum (the reverse-postorder number, -1: not reachable), and pre /
post (the order the dominator tree is entered and left: a dominates b
when pre[a] <= pre[b] and post[b] <= post[a], ng_dom_has).

```fides
pub struct NgDom {
```

**Fields:**

| Name | Type |
|------|------|
| `idom` | `Vec<i64>` |
| `rnum` | `Vec<i64>` |
| `pre` | `Vec<i64>` |
| `post` | `Vec<i64>` |

*Defined at line 7222*

---

## NatFnRec

A recorded function, waiting for allocation and emission.

```fides
pub struct NatFnRec {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `mir` | `Vec<MInst>` |
| `nvreg` | `i64` |
| `base_frame` | `i64` |
| `slots` | `Vec<i64>` |
| `is_ct` | `bool,  // J7: @constant_time — ng_ct_check scans the final MIR` |
| `sret_v` | `i64` |
| `sret_size` | `i64` |
| `secret` | `bool` |

*Defined at line 9053*

---

## NatAlloc

```fides
pub struct NatAlloc {
```

**Fields:**

| Name | Type |
|------|------|
| `assign` | `Vec<i64>` |
| `remat` | `Vec<i64>` |
| `remat_val` | `Vec<i64>` |
| `spill` | `Vec<i64>` |
| `frame` | `i64` |
| `save_regs` | `Vec<i64>` |
| `save_off` | `i64` |
| `mask` | `i64` |
| `err` | `str` |
| `cls` | `Vec<i64>` |

*Defined at line 12737*

---

## nat_seg_unit

RA2: a test of the use-list liveness, the segments and the scan over
segments on a random function of target `arch` (NAT_A64, NAT_RV64,
NAT_X64): `nblk` blocks over `nvr` vregs, each block a label, a few
definitions, reads and calls, and a jump, a conditional branch or a fall
through (the last block returns), from the generator seeded with
`seed`. Checks that ng_use_liveness equals mi_liveness; that each vreg's
segments hold position 2k + 1 exactly when instruction k writes it or it
is live after k, and 2k exactly when it is live into k (k reads it, or
it is live after k without k writing it); and that the scan's
assignment passes the allocation checker's rules (no definition writes
a register another value live after it holds; no call clobbers a
register a value live after it holds; registers from the pool). Returns
"" or the first failure; with `stats`, "" is replaced by
"<ranges> <spilled> <evicted>" when everything passes. For
tests/nattools/seg_unit.fd.

```fides
pub fn nat_seg_unit(arch: i64, seed: i64, nblk: i64, nvr: i64, stats: bool) -> str {
```

**Parameters:**

- `arch`: `i64`
- `seed`: `i64`
- `nblk`: `i64`
- `nvr`: `i64`
- `stats`: `bool`

**Returns:** `str`

*Defined at line 13677*

---

## nat_seg_designed

RA2: a function of target `arch` whose ranges fit the registers only
with holes: `a` is made before a branch and read only on its far side,
while the near side keeps as many values live as the pool has
registers (K; with a running sum `s` written at each read). As hulls, K
+ 1 ranges meet in the near side; as segments, `a` is not live there.
Returns the spills of the scan over segments, of the greedy allocator
over segments and of the greedy allocator over hulls, separated by
spaces ("0 0 n", n >= 1, is the expected answer). For
tests/nattools/seg_unit.fd.

```fides
pub fn nat_seg_designed(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 13962*

---

## NgPhys

RA7: what each operation of a function does to the argument registers
(masks over registers 0..63, within the candidates).

```fides
pub struct NgPhys {
```

**Fields:**

| Name | Type |
|------|------|
| `rd` | `Vec<i64>` |
| `wr` | `Vec<i64>` |
| `imp` | `Vec<i64>` |
| `kill` | `Vec<i64>` |
| `live` | `Vec<i64>` |
| `cpy` | `Vec<i64>` |

*Defined at line 14469*

---

## nat_arghint_unit

RA7: the argument-register hints (switched on here) on target `arch`
(NAT_A64, NAT_RV64, NAT_X64) for the hand-made function `flat` with `nv`
values (vregs 64..64+nv-1), every one first given register 20 (a pool
register stand-in) and its hull from its appearances; `blind` ignores
the implicit uses. Each value's register, then how many moved, for
tests/nattools/arghint_unit.fd.

```fides
pub fn nat_arghint_unit(arch: i64, flat: &Vec<i64>, nv: i64, blind: bool) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `blind`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 15380*

---

## nat_arghint_check_unit

RA7: the checker's verdict (ng_mir_check_args: "" when sound) on the
hand-made function `flat` with the registers `assign` (one per value),
for tests/nattools/arghint_unit.fd.

```fides
pub fn nat_arghint_check_unit(arch: i64, flat: &Vec<i64>, nv: i64, assign: &Vec<i64>) -> str {
```

**Parameters:**

- `arch`: `i64`
- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `assign`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 15424*

---

## nat_argfree_unit

RA3: the free argument registers (ng_arg_free, with RA7's candidates
switched on) on target `arch` for the hand-made function `flat` with
`nv` values (vregs 64..64+nv-1), each starting in the register `init`
gives it (-1: spilled), its hull from its appearances; `blind` ignores
the implicit uses (as FID_NAT_MUTATE=argfree). Each value's register,
then the ranges filled, traded and the callee-saved registers freed,
for tests/nattools/argfree_unit.fd (nat_arghint_check_unit judges it).

```fides
pub fn nat_argfree_unit(arch: i64, flat: &Vec<i64>, nv: i64, init: &Vec<i64>, blind: bool) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `init`: `&Vec<i64>`
- `blind`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 15451*

---

## NgSplit

RA4: a function's machine IR after live-range splitting.

```fides
pub struct NgSplit {
```

**Fields:**

| Name | Type |
|------|------|
| `mir` | `Vec<MInst>` |
| `nvreg` | `i64` |
| `orig` | `Vec<i64>` |
| `src` | `Vec<i64>` |
| `n_split` | `i64` |
| `n_loops` | `i64` |
| `n_edges` | `i64` |
| `err` | `str` |

*Defined at line 17292*

---

## NgLsState

RA4: where the moves of a split go while it is planned: `slots` are
lists of [phase, destination, source]; pre_ix / post_ix map an original
index to the slot run before / after it, edge_ix a conditional branch
to its edge block (e_*: the branch, its slot, its old target label and
the index the block goes before).

```fides
pub struct NgLsState {
```

**Fields:**

| Name | Type |
|------|------|
| `slots` | `Vec<Vec<i64>>` |
| `pre_ix` | `i64` |
| `post_ix` | `i64` |
| `edge_ix` | `i64` |
| `e_t` | `Vec<i64>` |
| `e_slot` | `Vec<i64>` |
| `e_to` | `Vec<i64>` |
| `e_q` | `Vec<i64>` |

*Defined at line 17313*

---

## nat_split_unit

RA4 unit hook: "pieces N loops L edges E" and the checker's verdict
("check: ok", "check: <error>", "check: nothing split").

```fides
pub fn nat_split_unit(flat: &Vec<i64>, nv: i64, spilled: &Vec<i64>, mutate: bool) -> Vec<str> {
```

**Parameters:**

- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `spilled`: `&Vec<i64>`
- `mutate`: `bool`

**Returns:** `Vec<str>`

*Defined at line 18561*

---

## nat_split_unit_mir

RA4 unit hook: the split function as rows (the rows themselves when
nothing was split).

```fides
pub fn nat_split_unit_mir(flat: &Vec<i64>, nv: i64, spilled: &Vec<i64>, mutate: bool) -> Vec<i64> {
```

**Parameters:**

- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `spilled`: `&Vec<i64>`
- `mutate`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 18586*

---

## nat_split_unit_drop_mir

RA4 unit hook: the split of `flat` without its `nth` inserted move, as
rows (empty when there is no such move or nothing was split).

```fides
pub fn nat_split_unit_drop_mir(flat: &Vec<i64>, nv: i64, spilled: &Vec<i64>, nth: i64) -> Vec<i64> {
```

**Parameters:**

- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `spilled`: `&Vec<i64>`
- `nth`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 18641*

---

## nat_split_unit_drop

RA4 unit hook: the checker's verdict on the split of `flat` with the
`nth` inserted move (0: the first) left out, as a broken planner would
leave it ("check: ok", "check: <error>", "check: no such move" or
"check: nothing split").

```fides
pub fn nat_split_unit_drop(flat: &Vec<i64>, nv: i64, spilled: &Vec<i64>, nth: i64) -> str {
```

**Parameters:**

- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `spilled`: `&Vec<i64>`
- `nth`: `i64`

**Returns:** `str`

*Defined at line 18671*

---

## nat_split_alloc_unit_err

```fides
pub fn nat_split_alloc_unit_err() -> str {
```

**Returns:** `str`

*Defined at line 18694*

---

## nat_split_alloc_unit_why

RA4: why the planner split nothing in the last nat_split_alloc_unit
("" when it split something or did not run).

```fides
pub fn nat_split_alloc_unit_why() -> str {
```

**Returns:** `str`

*Defined at line 18700*

---

## nat_split_alloc_unit

RA4 unit hook: the whole allocation with live-range splitting
(ng_mir_alloc_split, as a build runs it, with the MIR verifier, the
allocation checker and the split checker on, as under FID_VERIFY=1) of
the hand-made function `flat` (rows of 7: op, r0, r1, r2, i0, i1, i2;
`nv` vregs from NG_VBASE) on target `arch` (NAT_A64, NAT_RV64,
NAT_X64). `mode`: 0 the default (kept when cheaper), 1 no splitting
(FID_NAT_OFF=split), 2 every value split and kept (FID_NAT_ON=splitall).
The result, for tests/nattools/split_alloc_unit.fd, which runs the
allocated function: [pieces, around loops, edge blocks, cost without,
cost with, kept (1/0) (zeros when nothing was split), the candidates
(values the first allocation spilled), the values the second split
left whole (splitprune), the registers a
call keeps (the callee-saved mask), the result register, the number of
rows, the rows of the function as allocated (split or not), the number
of vregs, then 4 per vreg: register (-1 spilled, -2 unused), remat kind
(0: none), remat value, slot]. The error: nat_split_alloc_unit_err().

```fides
pub fn nat_split_alloc_unit(arch: i64, flat: &Vec<i64>, nv: i64, mode: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `flat`: `&Vec<i64>`
- `nv`: `i64`
- `mode`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 18720*

---

## nat_greedy_unit

RA9: the greedy assignment of target `arch` (NAT_A64, NAT_RV64,
NAT_X64) on given hulls with no calls and no copy hints, for
tests/nattools/greedy_unit.fd: each range's register (-1 spilled), then
the number of evictions, then the size of the integer pool.

```fides
pub fn nat_greedy_unit(arch: i64, lo: &Vec<i64>, hi: &Vec<i64>, weight: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `lo`: `&Vec<i64>`
- `hi`: `&Vec<i64>`
- `weight`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 19495*

---

## nat_vlive_unit_check

TG11: the vector liveness check (ng_vec_live_check) on small MIR
sequences: a value read after a call, on a path that skips its write,
or at 32 bytes after VEND is caught; a loop over a splat, a lane read
after VEND, the zeroing idiom and a value through a frame slot around a
call (the vectorizer's spill slots) pass. Run with nat_sel_check
(FID_VERIFY=1).

```fides
pub fn nat_vlive_unit_check() -> str {
```

**Returns:** `str`

*Defined at line 20120*

---

## nat_retfwd_unit_check

OP12 / ME27 / LF24: both halves of struct results in two registers on
small MIR functions (FID_VERIFY=1, from nat_sel_check). The callee
`PROLOGUE; MOV s, a0; STORE [s], x; STORE [s + 8], y; EPILOGUE` keeps
the words in vregs and returns them in argument registers 0 and 1. The
caller `SLOTADDR p, 32; MOV a0, p; CALL f; LOAD [p]; LOAD [p + 8]`
reads them from those registers with no load or store left
(ng_rr_caller_rewrite, ng_rf_promote); a caller that also passes the
slot's address to another call keeps it in memory. Skipped when
FID_NAT_OFF names retregs or retfwd. "" when every case holds.

```fides
pub fn nat_retfwd_unit_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 23745*

---

## nat_set_time_on

```fides
pub fn nat_set_time_on() {
```

*Defined at line 23970*

---

## nat_time_names

The rows recorded so far (nat_time_clear empties them: a long-lived
process builds again).

```fides
pub fn nat_time_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 23976*

---

## nat_time_ms

```fides
pub fn nat_time_ms() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 23980*

---

## nat_time_clear

```fides
pub fn nat_time_clear() {
```

*Defined at line 23984*

---

## NatParState

What the driver keeps for each function ng_mir_finish allocated, in
placement order: the final MIR, its allocation, the job whose bytes it
shares (-1: placed itself), its line rows' file.

```fides
pub struct NatParState {
```

**Fields:**

| Name | Type |
|------|------|
| `recs` | `Vec<NatFnRec>` |
| `als` | `Vec<NatAlloc>` |
| `fold_to` | `Vec<i64>` |
| `file_ix` | `Vec<i64>` |
| `fold_ix` | `i64` |
| `n0` | `i64` |
| `n_redo` | `i64` |
| `taints` | `Vec<Vec<i64>>` |

*Defined at line 24064*

---

## NatParBuf

A worker's output: the function's bytes and fixups with offsets from
its start, the image's labels it placed ([label, offset] pairs), where
each label it made was placed (-1: not placed by it), the panic-line
stubs it asked for ([which, line, its label] and the keys), its line
and frame rows, and the generator's per-function state after it.

```fides
pub struct NatParBuf {
```

**Fields:**

| Name | Type |
|------|------|
| `code` | `Vec<i64>` |
| `fixups` | `Vec<NatFix>` |
| `placed` | `Vec<i64>` |
| `fresh` | `Vec<i64>` |
| `pl_list` | `Vec<i64>` |
| `pl_names` | `Vec<str>` |
| `lrows` | `Vec<i64>` |
| `cfi_rows` | `Vec<i64>` |
| `lrow_open` | `bool` |
| `last_place` | `i64` |
| `mul_end` | `i64` |
| `ldfwd` | `i64` |
| `save_regs` | `Vec<i64>` |
| `save_off` | `i64` |
| `frame` | `i64` |
| `leaf` | `i64` |
| `wipe` | `i64` |
| `x0_reg` | `i64` |
| `x1_reg` | `i64` |
| `br_short` | `bool` |
| `err` | `str` |
| `dbg_mpos` | `Vec<i64>` |
| `dbg_on` | `bool` |
| `log` | `str` |

*Defined at line 24084*

---

## ng_par_thread

SP5: a worker thread: jobs s..e with generator wi of g_ng_par_gens, one
buffer per job (a folded one's empty), in order. Its label table (n0
entries, every one unplaced between two jobs) is its own.

```fides
pub fn ng_par_thread(wi: i64, s: i64, e: i64) -> Vec<NatParBuf> {
```

**Parameters:**

- `wi`: `i64`
- `s`: `i64`
- `e`: `i64`

**Returns:** `Vec<NatParBuf>`

*Defined at line 24438*

---

## nat_vsel_check

TG11: MO_VBIN's selection table and its hand-written selection agree
byte for byte on every kind, width and a few register pairs of the
target (run with nat_sel_check under FID_VERIFY=1).

```fides
pub fn nat_vsel_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 27822*

---

## NgVz

OP22: a loop ng_vz_match looked at. Per instruction of the loop (the
header's, then the body blocks', terminators left out): its class
(NG_VZ_*), kind (a lane operation's or reduction's NVK_* kind; an
element access's index into vecs), vector register and, for a lane
operation, how its destination is had (0 the left operand's register,
1 the right one's, the operation commuted, 2 a fresh register holding a
copy of the left operand).

```fides
pub struct NgVz {
```

**Fields:**

| Name | Type |
|------|------|
| `head` | `i64` |
| `islot` | `i64` |
| `sslot` | `i64` |
| `sop` | `i64` |
| `bound` | `i64` |
| `ids` | `Vec<i64>` |
| `cls` | `Vec<i64>` |
| `kind` | `Vec<i64>` |
| `vreg` | `Vec<i64>` |
| `how` | `Vec<i64>` |
| `vecs` | `Vec<i64>` |
| `vst` | `Vec<i64>` |
| `splat` | `Vec<i64>` |
| `sreg` | `Vec<i64>` |
| `acc` | `i64` |
| `nget` | `i64` |
| `nset` | `i64` |
| `nsti` | `i64` |
| `nsts` | `i64` |
| `nred` | `i64` |
| `seen_set` | `bool` |
| `istored` | `bool` |
| `w` | `i64` |
| `spill` | `bool` |
| `pin` | `Vec<i64>` |
| `simd` | `bool` |
| `nchk` | `i64` |
| `scan` | `Vec<i64>` |
| `ichk` | `Vec<i64>` |
| `redchk` | `bool` |

*Defined at line 28446*

---

## nat_pie_wanted

TG14: a position-independent Linux executable? The driver's `--pie` /
`--no-pie` (g_nat_pie_req) first, else FID_NAT_ON=pie (FID_NAT_OFF=pie
wins over both, to compare).

```fides
pub fn nat_pie_wanted() -> bool {
```

**Returns:** `bool`

*Defined at line 33149*

---

## nat_target_arch

`target`: "aarch64-linux" or "riscv64-linux" (also "arm64-linux",
"riscv-linux", "arm64", "riscv64").

```fides
pub fn nat_target_arch(target: str) -> i64 {
```

**Parameters:**

- `target`: `str`

**Returns:** `i64`

*Defined at line 33272*

---

## nat_cpu_default

TG10: `--cpu <name>` per target, with safe defaults — x86-64-v3 (the
runtime's fma uses FMA3), armv8.0-a, rv64gc. Accepted names are recorded
in `g_nat_cpu`; selection consults it where a CPU adds instructions:
an explicit `--cpu x86-64-v3|v4` turns on BMI2 shifts (TG7,
ng_x64_bmi2), `--cpu rva22u64|rva23u64|rv64gc_zba_zbb` Zba/Zbb (TG9,
ng_rv_zb). Without --cpu (or with `baseline`/`native`) neither is used.

```fides
pub fn nat_cpu_default(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 33297*

---

## nat_cpu_check

"" when `cpu` is a name target `arch` accepts, an error message else.

```fides
pub fn nat_cpu_check(arch: i64, cpu: str) -> str {
```

**Parameters:**

- `arch`: `i64`
- `cpu`: `str`

**Returns:** `str`

*Defined at line 33311*

---

## nat_select_mir

```fides
pub fn nat_select_mir(module: &IrModule) -> NatGen {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `NatGen`

*Defined at line 33372*

---

## nat_generate_elf

```fides
pub fn nat_generate_elf(module: IrModule, arch: i64) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`
- `arch`: `i64`

**Returns:** `ResultBytes`

*Defined at line 33528*

---

## nat_generate_pe

A Windows x64 executable (PE) from an IR module that has the Linux
runtime and its Windows layer (stdlib/linux_runtime/win.fd) merged in.

```fides
pub fn nat_generate_pe(module: IrModule) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `ResultBytes`

*Defined at line 33539*

---

## nat_generate_macho

A macOS executable (Mach-O) for `arch` from an IR module that has the
Linux runtime and its macOS layer (stdlib/linux_runtime/darwin.fd) merged.

```fides
pub fn nat_generate_macho(module: IrModule, arch: i64) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`
- `arch`: `i64`

**Returns:** `ResultBytes`

*Defined at line 33545*

---

## nat_generate_macho_universal

L2B: `--target universal` (macOS): the x86-64 and the AArch64 Mach-O of
a program, each from its own IR module (built for that target, with
the macOS layer merged), in one fat binary (nat_enc.fd nat_macho_fat)
that `lipo -info` reports as "x86_64 arm64". The --cpu choice applies
per slice: one that is not a name of a slice's target is left out for
it (baseline there).

```fides
pub fn nat_generate_macho_universal(x64_module: IrModule, a64_module: IrModule) -> ResultBytes {
```

**Parameters:**

- `x64_module`: `IrModule`
- `a64_module`: `IrModule`

**Returns:** `ResultBytes`

*Defined at line 33555*

---

## NatLdyn

```fides
pub struct NatLdyn {
```

**Fields:**

| Name | Type |
|------|------|
| `c_libs` | `Vec<str>` |
| `c_fixed` | `Vec<i64>` |
| `names` | `Vec<str>` |
| `libs` | `Vec<str>` |
| `ix` | `i64` |
| `stub_keys` | `Vec<str>` |
| `stub_ix` | `i64` |
| `stub_labels` | `Vec<i64>` |
| `stub_imp` | `Vec<i64>` |
| `stub_sigs` | `Vec<str>` |
| `got_labels` | `Vec<i64>` |
| `interp_label` | `i64` |
| `dyn_label` | `i64` |
| `dyn_end_label` | `i64` |

*Defined at line 34172*

---

## ng_dbg_shown_pairs

CT9 for pe_ir (which does not import nat_ir): each function of
`module` whose debugger name (ng_dbg_shown_name) differs from its
symbol, as pairs [symbol, shown, symbol, shown, ...]. compiler/main.fd
gives them to pe_ir_enc.fd g_pe_dbg_shown_pairs before a debug build.

```fides
pub fn ng_dbg_shown_pairs(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 35293*

---

## nat_dbg_scope_unit

DB4: ng_dbg_scope_blocks for a unit test (tests/nattools/
dbg_scopes_unit.fd): function `name` at [fstart, fend) of a code
buffer whose line rows are `lrows` (g.lrows' [offset, line, file word];
word 0 the function's own file, 2k another file, odd a close row), its
resolved variables `lines`.

```fides
pub fn nat_dbg_scope_unit(name: str, fstart: i64, fend: i64, lrows: Vec<i64>, lines: Vec<str>) -> str {
```

**Parameters:**

- `name`: `str`
- `fstart`: `i64`
- `fend`: `i64`
- `lrows`: `Vec<i64>`
- `lines`: `Vec<str>`

**Returns:** `str`

*Defined at line 36002*

---

## ng_dbg_split_note

DB4: RA4 kept a split of function `name` with pieces `orig`
(NgSplit.orig), for ng_dbg_vars_resolve; only for a function with
variables (g_ng_dbg_pend).

```fides
pub fn ng_dbg_split_note(name: str, orig: &Vec<i64>) {
```

**Parameters:**

- `name`: `str`
- `orig`: `&Vec<i64>`

*Defined at line 36145*

---

## NcFix

```fides
pub struct NcFix {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `i64` |
| `kind` | `i64` |
| `tkind` | `i64` |
| `toff` | `i64` |
| `tstr` | `str,  // tkind 1/2/3: target identity` |

*Defined at line 38077*

---

## NcEntry

```fides
pub struct NcEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `key` | `i64` |
| `mir_len` | `i64` |
| `n_traps` | `i64` |
| `n_calls` | `i64` |
| `mask` | `i64` |
| `bytes` | `Vec<i64>` |
| `fixes` | `Vec<NcFix>` |
| `edges` | `Vec<str>` |
| `refs` | `Vec<str>` |
| `fold_key` | `str,  // ng_mir_key under the run's alloc — lets fresh fns fold onto the copy` |
| `regs` | `Vec<str>` |
| `lrows` | `Vec<i64>` |
| `cfi` | `Vec<i64>` |
| `dvars` | `str` |
| `deps` | `Vec<str>` |
| `dep_masks` | `Vec<i64>` |
| `dep_rr` | `Vec<i64>` |
| `rr_self` | `i64` |
| `addrs` | `Vec<str>` |
| `ft_self` | `i64` |
| `dep_ft` | `Vec<i64>` |
| `entry_run` | `Vec<str>` |
| `pos16` | `i64` |

*Defined at line 38089*

---

## nat_fncache_source

```fides
pub fn nat_fncache_source() -> i64 {
```

**Returns:** `i64`

*Defined at line 39829*

---

## nat_fncache_reemitted

CB3: the functions the last native build with the per-function cache
on emitted again (not replayed), for the driver to report.

```fides
pub fn nat_fncache_reemitted() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 39835*

---

## nat_relink_start

SP7: the kept layout's function `name`: its start, -1 when none (a
test reads it through nat_relink_start).

```fides
pub fn nat_relink_start(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 40727*

---

## nat_relink_counts

SP7: did the last native build of this process put its functions back
in the layout of the one before (nl_begin), and how many: [used (0/1),
in place, moved into place, placed after].

```fides
pub fn nat_relink_counts() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 40737*

---

## nat_emit_object

TG12: a relocatable object of the program for `arch`, for the driver's
object-file output (`fid emit --obj`, main.fd nat_write_object): ELF
(ET_REL) for the Linux targets (nat_enc.fd nat_elf_object), COFF for
`--nat` Windows (nat_coff_object) and Mach-O (MH_OBJECT) for macOS
(nat_macho_object); each writer says what is in its object. A program
without `main` is a library build (WP20): its functions are in the
object and the entry only traps.

```fides
pub fn nat_emit_object(module: IrModule, arch: i64, win: bool, mac: bool) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`
- `arch`: `i64`
- `win`: `bool`
- `mac`: `bool`

**Returns:** `ResultBytes`

*Defined at line 41516*

---

## nat_memop_unit_check

OP18: the memory-operand folds on small MIR sequences (FID_VERIFY=1,
from nat_sel_check): what ng_fuse_ldext, ng_fuse_memcmp and
ng_fuse_rmw make of each, and the sequences they must leave as they
are (a store between the load and its reader, a value read twice, a
constant out of the field's range, the load on the right of SUB).
Compares and read-modify-writes are formed on x86-64 only. A switch
that turns a fold off (FID_NAT_OFF) or mutates it (FID_NAT_MUTATE)
skips its cases. "" when every case holds, else what failed.

```fides
pub fn nat_memop_unit_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 41562*

---

## nat_tg78_unit_check

TG7/TG8: the selections of llvm-roadmap TG7 and TG8 on small cases
(tests/nattools/tg78_unit.fd runs it through `fid run`):
x86-64  BLSR's kinds from ng_fuse_bits under --cpu x86-64-v3 (a & (a
- 1), a ^ (a - 1), a & -a, a & (0 - a)), none without it;
ng_rmw_slot's decisions; the bytes of ng_x64_rmw_slot and
ng_x64_cmp_slot (the compare kind swapped when the slot is the
left side, `cmp qword [rsp + off], 0` against zero)
AArch64 ng_wb_pair's moves and refusals; ng_a64_wb_fuse's words (post-
and pre-indexed loads and stores, a step by a SUB) and
refusals (t = n, a label between, an offset that is neither
0 nor the step, a step past 9 bits, sp as the base)
"" when every case holds.

```fides
pub fn nat_tg78_unit_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 41816*

---

## nat_sel_check

```fides
pub fn nat_sel_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 42100*

---

## nat_sel_check_more

RL6: translation validation beyond the integer ALU (FID_VERIFY=1, from
nat_sel_check): float operations, loads and stores, branches and calls,
each selected alone with physical registers through ng_mi_dispatch (the
replay's own path), and the bytes run by nat_enc.fd's evaluator against
the operation's meaning (nat_sel_eval_float / _mem / _branch / _call).
"" or the first mismatch.

```fides
pub fn nat_sel_check_more(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 42207*

---

## nat_sel_check_more_part

RL6: one part of nat_sel_check_more (0 floats, 1 memory, 2 branches,
3 calls), for tests/nattools/selcheck_more_unit.fd.

```fides
pub fn nat_sel_check_more_part(arch: i64, part: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `part`: `i64`

**Returns:** `str`

*Defined at line 42229*

---

## nat_isel_mem_check

TG1: the rules the value evaluator cannot run (loads and stores, with
their addressing modes; compare-and-branch; float operations) compared
byte for byte with the hand-written selection on registers the replay
hands them (ng_isel_test_regs; the stack pointer as a base; x86-64's
rsp and AArch64's 31 as an index, which no indexed form takes), every
size and signedness, displacements inside and outside each target's
immediate, every compare kind against a register and against zero,
short and long branches, f64 and f32, and float registers when
FID_NAT_ON=fregs gives them. Run with nat_sel_check (FID_VERIFY=1).

```fides
pub fn nat_isel_mem_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 42865*

---

## nat_isel_tg1b_check

TG1: the rules of ng_isel_rows_tg1b against the hand-written selection,
byte for byte, under each `--cpu` that changes them (x86-64-v3: BMI1;
rva23u64: Zba, Zbb and Zicond): division, remainder and the high
product, signed and unsigned, with the destination equal to either
operand or neither and the operands in rax / rdx; the sign mask in
place and not; I2F / F2I for f64 and f32, in general registers and
(FID_NAT_ON=fregs) float registers, and two F2I of one source in a row
(x86-64's xmm0 cache); ANDN; each BLSR kind; the select both ways and
with its destination as its value; test-bit-and-branch on bits 0, 7,
31, 32 and 63, set and clear, short and long; RISC-V's ADDW / SUBW /
MULW; x86-64's memory-operand ALU of every kind with d the base, the
first operand or neither. Run by nat_sel_check (FID_VERIFY=1).

```fides
pub fn nat_isel_tg1b_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 43165*

---

## nat_isel_rule_check

TG2: every rule of the selection table for `arch` whose operation the
value evaluator runs (ng_isel_evaluable: LI, the ALU, ADDI, SHI,
ADDSHL, SETCC, NEG, NOT, EXT), alone, on random operands. Registers
(ng_isel_test_regs, sometimes d = a, a = b, or b the zero register)
and constants (ng_isel_rand_imm) are drawn until the rule's
precondition holds; then its form alone is emitted and run by
nat_enc.fd nat_sel_eval_case on edge and random values against the
operation's meaning, and so is the generic lowering of the same
operation (the hand-written selection, FID_NAT_OFF=isel). Each rule
must be met NG_ISEL_RULE_TRIALS times in 600 draws: a rule whose
precondition no draw meets is an error, so the generator keeps up with
the table (a rule always shadowed by a cheaper one on this target is
not counted as checked). Left to
nat_sel_check's byte comparison: AArch64's MADD / MSUB (they fold the
multiply emitted before them) and the vector rules (nat_vsel_check);
the memory, branch and float rules have nat_isel_mem_check.

```fides
pub fn nat_isel_rule_check(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 43617*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
