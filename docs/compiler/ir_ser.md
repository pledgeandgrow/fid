[All modules](index.md)

# ir_ser -- Fides Documentation

## Contents

- [iw_irtype](#iw_irtype) (function)
- [ir_irtype](#ir_irtype) (function)
- [iw_irfunc](#iw_irfunc) (function)
- [ir_irfunc](#ir_irfunc) (function)
- [IrcEntry](#IrcEntry) (struct)
- [IrcEntry_new](#IrcEntry_new) (function)
- [irc_fn_key](#irc_fn_key) (function)
- [irc_begin](#irc_begin) (function)
- [irc_lookup](#irc_lookup) (function)
- [irc_record](#irc_record) (function)
- [irc_reintern](#irc_reintern) (function)
- [irc_replay](#irc_replay) (function)
- [irc_finish](#irc_finish) (function)
- [MEC_RULES:](#MEC_RULES:) (const)
- [MEC_EGRAPH:](#MEC_EGRAPH:) (const)
- [MecEntry](#MecEntry) (struct)
- [MecEntry_new](#MecEntry_new) (function)
- [mec_mix](#mec_mix) (function)
- [mec_b](#mec_b) (function)
- [mec_fn_hash](#mec_fn_hash) (function)
- [mec_key](#mec_key) (function)
- [mec_begin](#mec_begin) (function)
- [mec_pass_begin](#mec_pass_begin) (function)
- [mec_pass_end](#mec_pass_end) (function)
- [mec_lookup](#mec_lookup) (function)
- [mec_entry](#mec_entry) (function)
- [mec_log_begin](#mec_log_begin) (function)
- [mec_log_end](#mec_log_end) (function)
- [mec_log_inst](#mec_log_inst) (function)
- [mec_log_block](#mec_log_block) (function)
- [mec_apply_at](#mec_apply_at) (function)
- [mec_note](#mec_note) (function)
- [mec_chain_clear](#mec_chain_clear) (function)
- [mec_chain_push](#mec_chain_push) (function)
- [mec_chain_len](#mec_chain_len) (function)
- [mec_chain_get](#mec_chain_get) (function)
- [mec_finish](#mec_finish) (function)
- [MEC_IROPT:](#MEC_IROPT:) (const)
- [ImcEntry](#ImcEntry) (struct)
- [ImcEntry_new](#ImcEntry_new) (function)
- [imc_hs](#imc_hs) (function)
- [imc_env_h](#imc_env_h) (function)
- [imc_env_set](#imc_env_set) (function)
- [imc_verify_on](#imc_verify_on) (function)
- [imc_begin](#imc_begin) (function)
- [imc_ran](#imc_ran) (function)
- [imc_lookup](#imc_lookup) (function)
- [imc_same](#imc_same) (function)
- [imc_post](#imc_post) (function)
- [imc_use](#imc_use) (function)
- [imc_nokey_note](#imc_nokey_note) (function)
- [imc_take](#imc_take) (function)
- [imc_note](#imc_note) (function)
- [imc_verify_note](#imc_verify_note) (function)
- [imc_verify_post](#imc_verify_post) (function)
- [imc_finish](#imc_finish) (function)
- [IR_FILE_MAGIC:](#IR_FILE_MAGIC:) (const)
- [IR_FILE_VERSION:](#IR_FILE_VERSION:) (const)
- [ir_module_bytes](#ir_module_bytes) (function)
- [ir_module_save](#ir_module_save) (function)
- [IrModuleLoad](#IrModuleLoad) (struct)
- [ir_module_load](#ir_module_load) (function)
- [IRFUZZ_VARS:](#IRFUZZ_VARS:) (const)
- [IRFUZZ_FVARS:](#IRFUZZ_FVARS:) (const)
- [IRFUZZ_MEM:](#IRFUZZ_MEM:) (const)
- [IRFUZZ_HELPERS:](#IRFUZZ_HELPERS:) (const)
- [IRFUZZ_FCONSTS:](#IRFUZZ_FCONSTS:) (const)
- [irfuzz_program](#irfuzz_program) (function)
- [irfuzz_helper_program](#irfuzz_helper_program) (function)
- [irfuzz_init](#irfuzz_init) (function)
- [irfuzz_arg](#irfuzz_arg) (function)
- [irf_eval_unit](#irf_eval_unit) (function)
- [irfuzz_eval](#irfuzz_eval) (function)
- [irfuzz_helper_source](#irfuzz_helper_source) (function)
- [irfuzz_unit_source](#irfuzz_unit_source) (function)
- [irfuzz_template](#irfuzz_template) (function)
- [irfuzz_function](#irfuzz_function) (function)
- [irfuzz_helper_function](#irfuzz_helper_function) (function)
- [irfuzz_install_helpers](#irfuzz_install_helpers) (function)

## iw_irtype

```fides
pub fn iw_irtype(b: &mut AwBuf, t: IrType) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `t`: `IrType`

*Defined at line 73*

---

## ir_irtype

```fides
pub fn ir_irtype(r: &mut ArBuf) -> IrType {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `IrType`

*Defined at line 143*

---

## iw_irfunc

Serialize one lowered function (post-Lowerer_lower_func, pre auto-drop
passes — the auto-drop passes run on all functions uniformly after).

```fides
pub fn iw_irfunc(b: &mut AwBuf, f: IrFunction) {
```

**Parameters:**

- `b`: `&mut AwBuf`
- `f`: `IrFunction`

*Defined at line 1088*

---

## ir_irfunc

Deserialize an IrFunction; r.ok is false on any truncation/desync.

```fides
pub fn ir_irfunc(r: &mut ArBuf) -> IrFunction {
```

**Parameters:**

- `r`: `&mut ArBuf`

**Returns:** `IrFunction`

*Defined at line 1174*

---

## IrcEntry

```fides
pub struct IrcEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |
| `name` | `str` |
| `bytes` | `Vec<i64>` |

*Defined at line 1272*

---

## IrcEntry_new

```fides
pub fn IrcEntry_new(k: i64, n: str, b: Vec<i64>) -> IrcEntry {
```

**Parameters:**

- `k`: `i64`
- `n`: `str`
- `b`: `Vec<i64>`

**Returns:** `IrcEntry`

*Defined at line 1277*

---

## irc_fn_key

Content key for one function: its whole AST.

```fides
pub fn irc_fn_key(f: Func) -> i64 {
```

**Parameters:**

- `f`: `Func`

**Returns:** `i64`

*Defined at line 1403*

---

## irc_begin

Initialize the cache for one lower() call; returns immediately when
FID_IRCACHE names no file (the default), or the salt differs (the
file is rewritten at finish either way).

```fides
pub fn irc_begin(program: Program, pruned: bool) {
```

**Parameters:**

- `program`: `Program`
- `pruned`: `bool`

*Defined at line 1478*

---

## irc_lookup

Look up a cached function by (key,name); returns its serialized bytes
or an empty Vec. The name check keeps same-key collisions honest.
Hits re-record into g_irc_new so irc_finish keeps used entries alive.

```fides
pub fn irc_lookup(key: i64, name: str) -> Vec<i64> {
```

**Parameters:**

- `key`: `i64`
- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1540*

---

## irc_record

Record a just-lowered function for the finish write. `spawned` holds
the functions its body-lower synthesized (__own_drop/dup_N and friends):
on a later hit they are replayed back into the module too.

```fides
pub fn irc_record(key: i64, f: IrFunction, spawned: Vec<IrFunction>, types: Vec<Type>) {
```

**Parameters:**

- `key`: `i64`
- `f`: `IrFunction`
- `spawned`: `Vec<IrFunction>`
- `types`: `Vec<Type>`

*Defined at line 1554*

---

## irc_reintern

Re-intern the string constants a fresh lower would have registered.
(Lowerer interns ConstStr operands at emit; Panic/Asm strings stay
inside their insts and are not interned.)

```fides
pub fn irc_reintern(module: &mut IrModule, f: IrFunction) {
```

**Parameters:**

- `module`: `&mut IrModule`
- `f`: `IrFunction`

*Defined at line 1580*

---

## irc_replay

Replay a cached record: returns the main IrFunction after pushing any
spawned helpers into the module. Sets r.ok false on desync.

```fides
pub fn irc_replay(r: &mut ArBuf, module: &mut IrModule, l: &mut Lowerer) -> IrFunction {
```

**Parameters:**

- `r`: `&mut ArBuf`
- `module`: `&mut IrModule`
- `l`: `&mut Lowerer`

**Returns:** `IrFunction`

*Defined at line 1596*

---

## irc_finish

Write back the records produced this build (fresh captures replace stale
hits; dead functions drop out).

```fides
pub fn irc_finish() {
```

*Defined at line 1633*

---

## MEC_RULES:

```fides
pub const MEC_RULES: i64 = 1;
```

*Defined at line 1700*

---

## MEC_EGRAPH:

```fides
pub const MEC_EGRAPH: i64 = 2;
```

*Defined at line 1701*

---

## MecEntry

```fides
pub struct MecEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |
| `name_h` | `i64` |
| `post` | `i64` |
| `c1` | `i64` |
| `c2` | `i64` |
| `c3` | `i64` |
| `delta` | `Vec<i64>` |

*Defined at line 1706*

---

## MecEntry_new

```fides
pub fn MecEntry_new(key: i64, name_h: i64, post: i64, c1: i64, c2: i64, c3: i64, delta: Vec<i64>) -> MecEntry {
```

**Parameters:**

- `key`: `i64`
- `name_h`: `i64`
- `post`: `i64`
- `c1`: `i64`
- `c2`: `i64`
- `c3`: `i64`
- `delta`: `Vec<i64>`

**Returns:** `MecEntry`

*Defined at line 1716*

---

## mec_mix

```fides
pub fn mec_mix(h: i64, v: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `v`: `i64`

**Returns:** `i64`

*Defined at line 1747*

---

## mec_b

```fides
pub fn mec_b(b: bool) -> i64 {
```

**Parameters:**

- `b`: `bool`

**Returns:** `i64`

*Defined at line 1752*

---

## mec_fn_hash

The content hash of a function: every field iw_irfunc writes.

```fides
pub fn mec_fn_hash(f: IrFunction) -> i64 {
```

**Parameters:**

- `f`: `IrFunction`

**Returns:** `i64`

*Defined at line 2181*

---

## mec_key

The key of pass `pass` on a function hashing `pre`, with the run's
switches and other inputs folded into `salt`. Never one of the two
integers core's HashMap reserves (core.fd hm_store_i).

```fides
pub fn mec_key(pass: i64, pre: i64, salt: i64) -> i64 {
```

**Parameters:**

- `pass`: `i64`
- `pre`: `i64`
- `salt`: `i64`

**Returns:** `i64`

*Defined at line 2248*

---

## mec_begin

Start a pass with the memo: false when it is off. Loads the file on
the first pass of the process (or when FID_MECACHE names another);
a long-lived process (`fid serve`) keeps the entries in memory.

```fides
pub fn mec_begin() -> bool {
```

**Returns:** `bool`

*Defined at line 2404*

---

## mec_pass_begin

```fides
pub fn mec_pass_begin() {
```

*Defined at line 2415*

---

## mec_pass_end

```fides
pub fn mec_pass_end(name: str) {
```

**Parameters:**

- `name`: `str`

*Defined at line 2421*

---

## mec_lookup

The entry for `key` made on a function named `name`: its position + 1,
or 0.

```fides
pub fn mec_lookup(key: i64, name: str) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2430*

---

## mec_entry

```fides
pub fn mec_entry(at: i64) -> MecEntry {
```

**Parameters:**

- `at`: `i64`

**Returns:** `MecEntry`

*Defined at line 2444*

---

## mec_log_begin

```fides
pub fn mec_log_begin() {
```

*Defined at line 2448*

---

## mec_log_end

```fides
pub fn mec_log_end() {
```

*Defined at line 2454*

---

## mec_log_inst

A pass replaced f.insts[at] (eg_set_inst, ru_set_inst).

```fides
pub fn mec_log_inst(at: i64) {
```

**Parameters:**

- `at`: `i64`

*Defined at line 2459*

---

## mec_log_block

A pass replaced f.blocks[bi] (eg_place).

```fides
pub fn mec_log_block(bi: i64) {
```

**Parameters:**

- `bi`: `i64`

*Defined at line 2466*

---

## mec_apply_at

A hit: replay entry `at` (mec_lookup) on `f`. False, `f` unchanged,
when it does not fit: the caller runs the pass.

```fides
pub fn mec_apply_at(at: i64, f: &mut IrFunction) -> bool {
```

**Parameters:**

- `at`: `i64`
- `f`: `&mut IrFunction`

**Returns:** `bool`

*Defined at line 2756*

---

## mec_note

A miss: the pass ran on `f`, which was `pre` (hashing `pre_h`) and
is now `f`; `changed` as the pass reported (when it reported nothing
the caller keeps `pre`). Records the entry when its replay gives `f`
exactly; returns f's hash.

```fides
pub fn mec_note(key: i64, pre: IrFunction, pre_h: i64, f: IrFunction, changed: bool, c1: i64, c2: i64, c3: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `pre`: `IrFunction`
- `pre_h`: `i64`
- `f`: `IrFunction`
- `changed`: `bool`
- `c1`: `i64`
- `c2`: `i64`
- `c3`: `i64`

**Returns:** `i64`

*Defined at line 2770*

---

## mec_chain_clear

```fides
pub fn mec_chain_clear() {
```

*Defined at line 2792*

---

## mec_chain_push

```fides
pub fn mec_chain_push(name: str, n_insts: i64, h: i64) {
```

**Parameters:**

- `name`: `str`
- `n_insts`: `i64`
- `h`: `i64`

*Defined at line 2798*

---

## mec_chain_len

```fides
pub fn mec_chain_len() -> i64 {
```

**Returns:** `i64`

*Defined at line 2804*

---

## mec_chain_get

The hash the rules left for function `i` of the module (named `name`,
`n_insts` instructions), 0 when unknown.

```fides
pub fn mec_chain_get(i: i64, name: str, n_insts: i64) -> i64 {
```

**Parameters:**

- `i`: `i64`
- `name`: `str`
- `n_insts`: `i64`

**Returns:** `i64`

*Defined at line 2810*

---

## mec_finish

After the last pass of a build (egraph_run): write the entries when the
build made any (used ones, then new ones, then the rest), and keep them
for the process's next build.

```fides
pub fn mec_finish() {
```

*Defined at line 2860*

---

## MEC_IROPT:

```fides
pub const MEC_IROPT: i64 = 3;
```

*Defined at line 2948*

---

## ImcEntry

```fides
pub struct ImcEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |
| `name_h` | `i64` |
| `post` | `i64` |
| `same` | `i64` |
| `off` | `i64` |
| `len` | `i64` |
| `chk` | `i64` |
| `bytes` | `Vec<i64>` |

*Defined at line 2956*

---

## ImcEntry_new

```fides
pub fn ImcEntry_new(key: i64, name_h: i64, post: i64, same: i64, off: i64, len: i64, chk: i64, bytes: Vec<i64>) -> ImcEntry {
```

**Parameters:**

- `key`: `i64`
- `name_h`: `i64`
- `post`: `i64`
- `same`: `i64`
- `off`: `i64`
- `len`: `i64`
- `chk`: `i64`
- `bytes`: `Vec<i64>`

**Returns:** `ImcEntry`

*Defined at line 2970*

---

## imc_hs

hash_source for iropt.fd (which does not import compiler::inc).

```fides
pub fn imc_hs(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 3002*

---

## imc_env_h

The hash of environment variable `name`'s value ("" when unset).

```fides
pub fn imc_env_h(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 3007*

---

## imc_env_set

Is environment variable `name` set to something?

```fides
pub fn imc_env_set(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 3012*

---

## imc_verify_on

FID_IROCACHE_VERIFY: run the passes on a hit too and compare.

```fides
pub fn imc_verify_on() -> bool {
```

**Returns:** `bool`

*Defined at line 3017*

---

## imc_begin

Start iropt_native's loop with the memo: false when it is off. Loads
the file the first time (or when FID_IROCACHE names another); a
long-lived process (`fid serve`) keeps the entries in memory.

```fides
pub fn imc_begin() -> bool {
```

**Returns:** `bool`

*Defined at line 3159*

---

## imc_ran

CB3: the functions the last build with the memo ran the passes on.

```fides
pub fn imc_ran() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 3177*

---

## imc_lookup

The entry for `key` made on a function named `name`: its position + 1,
or 0.

```fides
pub fn imc_lookup(key: i64, name: str) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 3183*

---

## imc_same

Did the passes leave the function of entry `at` as it was?

```fides
pub fn imc_same(at: i64) -> bool {
```

**Parameters:**

- `at`: `i64`

**Returns:** `bool`

*Defined at line 3198*

---

## imc_post

The content hash entry `at` recorded after the passes.

```fides
pub fn imc_post(at: i64) -> i64 {
```

**Parameters:**

- `at`: `i64`

**Returns:** `i64`

*Defined at line 3203*

---

## imc_use

A hit on entry `at` is used.

```fides
pub fn imc_use(at: i64) {
```

**Parameters:**

- `at`: `i64`

*Defined at line 3208*

---

## imc_nokey_note

A function the loop runs on that has no key (iro_imc_key gave 0).

```fides
pub fn imc_nokey_note(name: str) {
```

**Parameters:**

- `name`: `str`

*Defined at line 3214*

---

## imc_take

The function entry `at` recorded (named `name`), when its bytes are
whole: None when they are not (a damaged file), and the caller runs the
passes. Marks the entry used.

```fides
pub fn imc_take(at: i64, name: str) -> OptIrFunction {
```

**Parameters:**

- `at`: `i64`
- `name`: `str`

**Returns:** `OptIrFunction`

*Defined at line 3222*

---

## imc_note

A miss: the passes made `f` (named `name`) of a function hashing
`pre_h`; `changed` as they reported. Records the entry; returns f's
content hash.

```fides
pub fn imc_note(key: i64, name: str, pre_h: i64, f: IrFunction, changed: bool) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `name`: `str`
- `pre_h`: `i64`
- `f`: `IrFunction`
- `changed`: `bool`

**Returns:** `i64`

*Defined at line 3246*

---

## imc_verify_note

FID_IROCACHE_VERIFY: the passes ran on a hit of entry `at` and gave a
function hashing `post`.

```fides
pub fn imc_verify_note(at: i64, name: str, post: i64) {
```

**Parameters:**

- `at`: `i64`
- `name`: `str`
- `post`: `i64`

*Defined at line 3271*

---

## imc_verify_post

2g.93 (CS6): the content hash of what the passes made of a verified
hit's function, in the hash imc_note recorded for entry `at`: the hash
of iw_irfunc's bytes for a changed function (imc_note's `post`),
`pre_h` (mec_fn_hash) for one left as it was. (Comparing mec_fn_hash
with a recorded bytes hash reported every changed function as a
mismatch.) For an entry recorded "unchanged", a result the passes
report as changed is compared by mec_fn_hash, which is pre_h when the
IR is the same.

```fides
pub fn imc_verify_post(at: i64, f: IrFunction, changed: bool, pre_h: i64) -> i64 {
```

**Parameters:**

- `at`: `i64`
- `f`: `IrFunction`
- `changed`: `bool`
- `pre_h`: `i64`

**Returns:** `i64`

*Defined at line 3288*

---

## imc_finish

After iropt_native's loop: print the counts (FID_IROCACHE_STATS), and
write the entries when the build made any (used ones, then new ones,
then the rest); the process keeps them in memory for its next build.

```fides
pub fn imc_finish() {
```

*Defined at line 3316*

---

## IR_FILE_MAGIC:

```fides
pub const IR_FILE_MAGIC: i64 = 0x31524946;
```

*Defined at line 3414*

---

## IR_FILE_VERSION:

```fides
pub const IR_FILE_VERSION: i64 = 3;  // 2 (Nov 16): IrFunction.has_secret (LF20); 3 (Nov 18): IrFunction.secret_values (LF20)
```

*Defined at line 3415*

---

## ir_module_bytes

The serialized module (with the target it was lowered for).

```fides
pub fn ir_module_bytes(m: &IrModule, target: str) -> Vec<i64> {
```

**Parameters:**

- `m`: `&IrModule`
- `target`: `str`

**Returns:** `Vec<i64>`

*Defined at line 3418*

---

## ir_module_save

Write the module to `path`; false when the file cannot be written.

```fides
pub fn ir_module_save(path: str, m: &IrModule, target: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `m`: `&IrModule`
- `target`: `str`

**Returns:** `bool`

*Defined at line 3530*

---

## IrModuleLoad

A module read back: `ok` false (and `err` why) on a file that is not
one, is cut short, or is of another version.

```fides
pub struct IrModuleLoad {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `target` | `str` |
| `module` | `IrModule` |

*Defined at line 3537*

---

## ir_module_load

Read a module that ir_module_save wrote.

```fides
pub fn ir_module_load(path: str) -> IrModuleLoad {
```

**Parameters:**

- `path`: `str`

**Returns:** `IrModuleLoad`

*Defined at line 3551*

---

## IRFUZZ_VARS:

```fides
pub const IRFUZZ_VARS: i64 = 4;
```

*Defined at line 3706*

---

## IRFUZZ_FVARS:

The f64 variables w0, w1.

```fides
pub const IRFUZZ_FVARS: i64 = 2;
```

*Defined at line 3708*

---

## IRFUZZ_MEM:

Words of fz_target's heap buffer (a power of two).

```fides
pub const IRFUZZ_MEM: i64 = 8;
```

*Defined at line 3710*

---

## IRFUZZ_HELPERS:

The generated functions fz_h0.. that the others call.

```fides
pub const IRFUZZ_HELPERS: i64 = 2;
```

*Defined at line 3712*

---

## IRFUZZ_FCONSTS:

Entries of irf_fconst.

```fides
pub const IRFUZZ_FCONSTS: i64 = 8;
```

*Defined at line 3714*

---

## irfuzz_program

The description of seed `seed`'s fz_target.

```fides
pub fn irfuzz_program(seed: i64) -> Vec<i64> {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3947*

---

## irfuzz_helper_program

The description of seed `seed`'s fz_h<k>.

```fides
pub fn irfuzz_helper_program(seed: i64, k: i64) -> Vec<i64> {
```

**Parameters:**

- `seed`: `i64`
- `k`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3952*

---

## irfuzz_init

```fides
pub fn irfuzz_init(seed: i64, which: i64) -> i64 {
```

**Parameters:**

- `seed`: `i64`
- `which`: `i64`

**Returns:** `i64`

*Defined at line 4087*

---

## irfuzz_arg

```fides
pub fn irfuzz_arg(seed: i64, which: i64) -> i64 {
```

**Parameters:**

- `seed`: `i64`
- `which`: `i64`

**Returns:** `i64`

*Defined at line 4093*

---

## irf_eval_unit

What unit `unit` of seed `seed` (-1 fz_target, k fz_h<k>) returns
for the arguments a and b.

```fides
pub fn irf_eval_unit(seed: i64, unit: i64, a: i64, b: i64) -> i64 {
```

**Parameters:**

- `seed`: `i64`
- `unit`: `i64`
- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 4213*

---

## irfuzz_eval

What fz_target returns for seed `seed` (arguments irfuzz_arg).

```fides
pub fn irfuzz_eval(seed: i64) -> i64 {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `i64`

*Defined at line 4235*

---

## irfuzz_helper_source

Source of seed `seed`'s fz_h<k>: it computes what its description
says (irf_eval_unit), as irfuzz_helper_function's IR does.

```fides
pub fn irfuzz_helper_source(seed: i64, k: i64) -> str {
```

**Parameters:**

- `seed`: `i64`
- `k`: `i64`

**Returns:** `str`

*Defined at line 4367*

---

## irfuzz_unit_source

Source of unit `unit` (-1 fz_target, k fz_h<k>) as a function named
`name` that computes what irf_eval_unit does; fz_target's buffer is a
Vec<i64> here (tests/tooling/irfuzz-mirror-check.sh runs these
against irfuzz_eval).

```fides
pub fn irfuzz_unit_source(seed: i64, unit: i64, name: str) -> str {
```

**Parameters:**

- `seed`: `i64`
- `unit`: `i64`
- `name`: `str`

**Returns:** `str`

*Defined at line 4375*

---

## irfuzz_template

The program `fid ir-fuzz` builds: fz_target's body is replaced by
irfuzz_function, each fz_h<k>'s by irfuzz_install_helpers (its source
here computes the same value). fz_target's template body calls every
helper (so the lowering keeps them) and allocates (so main's call to
it is not taken for a pure one, which the random body is not).

```fides
pub fn irfuzz_template(seed: i64) -> str {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `str`

*Defined at line 4400*

---

## irfuzz_function

`old` (the lowered fz_target: its parameters `__env, a, b` are kept)
with the body of seed `seed`'s description.

```fides
pub fn irfuzz_function(old: IrFunction, seed: i64) -> IrFunction {
```

**Parameters:**

- `old`: `IrFunction`
- `seed`: `i64`

**Returns:** `IrFunction`

*Defined at line 4676*

---

## irfuzz_helper_function

`old` (the lowered fz_h<k>) with the body of seed `seed`'s description
of fz_h<k>.

```fides
pub fn irfuzz_helper_function(old: IrFunction, seed: i64, k: i64) -> IrFunction {
```

**Parameters:**

- `old`: `IrFunction`
- `seed`: `i64`
- `k`: `i64`

**Returns:** `IrFunction`

*Defined at line 4682*

---

## irfuzz_install_helpers

Each fz_h<k> of the template, lowered, replaced by its random IR (call
after replacing fz_target: the value stays irfuzz_eval's). False when
the module lacks one.

```fides
pub fn irfuzz_install_helpers(m: &mut IrModule, seed: i64) -> bool {
```

**Parameters:**

- `m`: `&mut IrModule`
- `seed`: `i64`

**Returns:** `bool`

*Defined at line 4689*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
