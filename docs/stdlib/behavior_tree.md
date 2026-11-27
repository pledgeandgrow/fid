[All modules](index.md)

# behavior_tree -- Fides Documentation

## Contents

- [BT_SUCCESS:](#BT_SUCCESS:) (const)
- [BT_FAILURE:](#BT_FAILURE:) (const)
- [BT_RUNNING:](#BT_RUNNING:) (const)
- [BT_IDLE:](#BT_IDLE:) (const)
- [BTK_SEQUENCE:](#BTK_SEQUENCE:) (const)
- [BTK_SEQUENCE_MEM:](#BTK_SEQUENCE_MEM:) (const)
- [BTK_REACTIVE_SEQUENCE:](#BTK_REACTIVE_SEQUENCE:) (const)
- [BTK_FALLBACK:](#BTK_FALLBACK:) (const)
- [BTK_REACTIVE_FALLBACK:](#BTK_REACTIVE_FALLBACK:) (const)
- [BTK_PARALLEL:](#BTK_PARALLEL:) (const)
- [BTK_INVERTER:](#BTK_INVERTER:) (const)
- [BTK_FORCE_SUCCESS:](#BTK_FORCE_SUCCESS:) (const)
- [BTK_FORCE_FAILURE:](#BTK_FORCE_FAILURE:) (const)
- [BTK_REPEAT:](#BTK_REPEAT:) (const)
- [BTK_RETRY:](#BTK_RETRY:) (const)
- [BTK_TIMEOUT:](#BTK_TIMEOUT:) (const)
- [BTK_ACTION:](#BTK_ACTION:) (const)
- [BTK_CONDITION:](#BTK_CONDITION:) (const)
- [BTK_ALWAYS_SUCCESS:](#BTK_ALWAYS_SUCCESS:) (const)
- [BTK_ALWAYS_FAILURE:](#BTK_ALWAYS_FAILURE:) (const)
- [bt_status_str](#bt_status_str) (function)
- [BB_INT:](#BB_INT:) (const)
- [BB_FLOAT:](#BB_FLOAT:) (const)
- [BB_STR:](#BB_STR:) (const)
- [BB_BOOL:](#BB_BOOL:) (const)
- [bb_new](#bb_new) (function)
- [bb_has](#bb_has) (function)
- [bb_kind](#bb_kind) (function)
- [bb_set_int](#bb_set_int) (function)
- [bb_set_float](#bb_set_float) (function)
- [bb_set_str](#bb_set_str) (function)
- [bb_set_bool](#bb_set_bool) (function)
- [bb_get_int](#bb_get_int) (function)
- [bb_get_float](#bb_get_float) (function)
- [bb_get_str](#bb_get_str) (function)
- [bb_get_bool](#bb_get_bool) (function)
- [BtLeaf](#BtLeaf) (struct)
- [bt_leaf_action](#bt_leaf_action) (function)
- [bt_leaf_action_halt](#bt_leaf_action_halt) (function)
- [bt_leaf_condition](#bt_leaf_condition) (function)
- [BtNode](#BtNode) (struct)
- [BTree](#BTree) (struct)
- [bt_new](#bt_new) (function)
- [bt_add_node](#bt_add_node) (function)
- [bt_set_root](#bt_set_root) (function)
- [bt_add_child](#bt_add_child) (function)
- [bt_register](#bt_register) (function)
- [bt_add_leaf](#bt_add_leaf) (function)
- [bt_set_port](#bt_set_port) (function)
- [bt_now](#bt_now) (function)
- [bt_port_has](#bt_port_has) (function)
- [bt_port_str](#bt_port_str) (function)
- [bt_port_int](#bt_port_int) (function)
- [bt_port_float](#bt_port_float) (function)
- [bt_port_key](#bt_port_key) (function)
- [bt_port_set_int](#bt_port_set_int) (function)
- [bt_port_set_float](#bt_port_set_float) (function)
- [bt_port_set_str](#bt_port_set_str) (function)
- [bt_halt](#bt_halt) (function)
- [bt_tick](#bt_tick) (function)
- [bt_tick_node](#bt_tick_node) (function)
- [bt_dump](#bt_dump) (function)
- [BtLoad](#BtLoad) (struct)
- [bt_from_xml](#bt_from_xml) (function)

## BT_SUCCESS:

```fides
pub const BT_SUCCESS: i64 = 0;
```

*Defined at line 54*

---

## BT_FAILURE:

```fides
pub const BT_FAILURE: i64 = 1;
```

*Defined at line 55*

---

## BT_RUNNING:

```fides
pub const BT_RUNNING: i64 = 2;
```

*Defined at line 56*

---

## BT_IDLE:

```fides
pub const BT_IDLE: i64 = 3;
```

*Defined at line 58*

---

## BTK_SEQUENCE:

```fides
pub const BTK_SEQUENCE: i64 = 1;
```

*Defined at line 60*

---

## BTK_SEQUENCE_MEM:

```fides
pub const BTK_SEQUENCE_MEM: i64 = 2;
```

*Defined at line 61*

---

## BTK_REACTIVE_SEQUENCE:

```fides
pub const BTK_REACTIVE_SEQUENCE: i64 = 3;
```

*Defined at line 62*

---

## BTK_FALLBACK:

```fides
pub const BTK_FALLBACK: i64 = 4;
```

*Defined at line 63*

---

## BTK_REACTIVE_FALLBACK:

```fides
pub const BTK_REACTIVE_FALLBACK: i64 = 5;
```

*Defined at line 64*

---

## BTK_PARALLEL:

```fides
pub const BTK_PARALLEL: i64 = 6;
```

*Defined at line 65*

---

## BTK_INVERTER:

```fides
pub const BTK_INVERTER: i64 = 10;
```

*Defined at line 66*

---

## BTK_FORCE_SUCCESS:

```fides
pub const BTK_FORCE_SUCCESS: i64 = 11;
```

*Defined at line 67*

---

## BTK_FORCE_FAILURE:

```fides
pub const BTK_FORCE_FAILURE: i64 = 12;
```

*Defined at line 68*

---

## BTK_REPEAT:

```fides
pub const BTK_REPEAT: i64 = 13;
```

*Defined at line 69*

---

## BTK_RETRY:

```fides
pub const BTK_RETRY: i64 = 14;
```

*Defined at line 70*

---

## BTK_TIMEOUT:

```fides
pub const BTK_TIMEOUT: i64 = 15;
```

*Defined at line 71*

---

## BTK_ACTION:

```fides
pub const BTK_ACTION: i64 = 20;
```

*Defined at line 72*

---

## BTK_CONDITION:

```fides
pub const BTK_CONDITION: i64 = 21;
```

*Defined at line 73*

---

## BTK_ALWAYS_SUCCESS:

```fides
pub const BTK_ALWAYS_SUCCESS: i64 = 22;
```

*Defined at line 74*

---

## BTK_ALWAYS_FAILURE:

```fides
pub const BTK_ALWAYS_FAILURE: i64 = 23;
```

*Defined at line 75*

---

## bt_status_str

```fides
pub fn bt_status_str(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 77*

---

## BB_INT:

```fides
pub const BB_INT: i64 = 1;
```

*Defined at line 92*

---

## BB_FLOAT:

```fides
pub const BB_FLOAT: i64 = 2;
```

*Defined at line 93*

---

## BB_STR:

```fides
pub const BB_STR: i64 = 3;
```

*Defined at line 94*

---

## BB_BOOL:

```fides
pub const BB_BOOL: i64 = 4;
```

*Defined at line 95*

---

## bb_new

A new, empty blackboard.

```fides
pub fn bb_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 106*

---

## bb_has

```fides
pub fn bb_has(bb: i64, key: str) -> bool {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`

**Returns:** `bool`

*Defined at line 136*

---

## bb_kind

The kind (BB_*) of an entry, 0 when absent.

```fides
pub fn bb_kind(bb: i64, key: str) -> i64 {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`

**Returns:** `i64`

*Defined at line 141*

---

## bb_set_int

```fides
pub fn bb_set_int(bb: i64, key: str, v: i64) {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `v`: `i64`

*Defined at line 149*

---

## bb_set_float

```fides
pub fn bb_set_float(bb: i64, key: str, v: f64) {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `v`: `f64`

*Defined at line 155*

---

## bb_set_str

```fides
pub fn bb_set_str(bb: i64, key: str, v: str) {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `v`: `str`

*Defined at line 161*

---

## bb_set_bool

```fides
pub fn bb_set_bool(bb: i64, key: str, v: bool) {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `v`: `bool`

*Defined at line 167*

---

## bb_get_int

An int entry (a float entry is truncated, a str entry parsed); `def`
when absent.

```fides
pub fn bb_get_int(bb: i64, key: str, def: i64) -> i64 {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 179*

---

## bb_get_float

A float entry (an int entry is converted, a str entry parsed).

```fides
pub fn bb_get_float(bb: i64, key: str, def: f64) -> f64 {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 195*

---

## bb_get_str

Any entry as text.

```fides
pub fn bb_get_str(bb: i64, key: str, def: str) -> str {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 211*

---

## bb_get_bool

A bool entry (an int is true when non-zero, a str when "true" or "1").

```fides
pub fn bb_get_bool(bb: i64, key: str, def: bool) -> bool {
```

**Parameters:**

- `bb`: `i64`
- `key`: `str`
- `def`: `bool`

**Returns:** `bool`

*Defined at line 233*

---

## BtLeaf

A registered leaf: its name, whether it is a condition, its tick and
its halt function.

```fides
pub struct BtLeaf {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `condition` | `bool` |
| `tick` | `fn(i64) -> i64` |
| `halt` | `fn(i64) -> i64` |

*Defined at line 252*

---

## bt_leaf_action

```fides
pub fn bt_leaf_action(name: str, tick: fn(i64) -> i64) -> BtLeaf {
```

**Parameters:**

- `name`: `str`
- `tick`: `fn(i64) -> i64`

**Returns:** `BtLeaf`

*Defined at line 263*

---

## bt_leaf_action_halt

```fides
pub fn bt_leaf_action_halt(name: str, tick: fn(i64) -> i64, halt: fn(i64) -> i64) -> BtLeaf {
```

**Parameters:**

- `name`: `str`
- `tick`: `fn(i64) -> i64`
- `halt`: `fn(i64) -> i64`

**Returns:** `BtLeaf`

*Defined at line 269*

---

## bt_leaf_condition

```fides
pub fn bt_leaf_condition(name: str, tick: fn(i64) -> i64) -> BtLeaf {
```

**Parameters:**

- `name`: `str`
- `tick`: `fn(i64) -> i64`

**Returns:** `BtLeaf`

*Defined at line 275*

---

## BtNode

```fides
pub struct BtNode {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `name` | `str` |
| `children` | `Vec<i64>` |
| `port_names` | `Vec<str>` |
| `port_vals` | `Vec<str>` |
| `leaf` | `i64` |
| `p1` | `i64` |
| `p2` | `i64` |
| `status` | `i64` |
| `idx` | `i64` |
| `start` | `f64` |
| `child_done` | `Vec<i64>` |

*Defined at line 281*

---

## BTree

```fides
pub struct BTree {
```

**Fields:**

| Name | Type |
|------|------|
| `nodes` | `Vec<BtNode>` |
| `leaves` | `Vec<BtLeaf>` |
| `root` | `i64` |
| `bb` | `i64` |
| `now` | `f64` |
| `log` | `Vec<str>` |
| `ticks` | `i64` |

*Defined at line 300*

---

## bt_new

```fides
pub fn bt_new(bb: i64) -> BTree {
```

**Parameters:**

- `bb`: `i64`

**Returns:** `BTree`

*Defined at line 311*

---

## bt_add_node

Add a node; returns its id. The first node added is the root unless
bt_set_root says otherwise.

```fides
pub fn bt_add_node(t: &mut BTree, kind: i64, name: str, p1: i64, p2: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut BTree`
- `kind`: `i64`
- `name`: `str`
- `p1`: `i64`
- `p2`: `i64`

**Returns:** `i64`

*Defined at line 319*

---

## bt_set_root

```fides
pub fn bt_set_root(t: &mut BTree, id: i64) {
```

**Parameters:**

- `t`: `&mut BTree`
- `id`: `i64`

*Defined at line 330*

---

## bt_add_child

Make `child` the last child of `parent`.

```fides
pub fn bt_add_child(t: &mut BTree, parent: i64, child: i64) {
```

**Parameters:**

- `t`: `&mut BTree`
- `parent`: `i64`
- `child`: `i64`

*Defined at line 335*

---

## bt_register

Register a leaf with the tree (by name; a second one replaces the first).

```fides
pub fn bt_register(t: &mut BTree, leaf: BtLeaf) -> i64 {
```

**Parameters:**

- `t`: `&mut BTree`
- `leaf`: `BtLeaf`

**Returns:** `i64`

*Defined at line 343*

---

## bt_add_leaf

A leaf node running the registered leaf `name` (-1 if none has it).

```fides
pub fn bt_add_leaf(t: &mut BTree, name: str) -> i64 {
```

**Parameters:**

- `t`: `&mut BTree`
- `name`: `str`

**Returns:** `i64`

*Defined at line 368*

---

## bt_set_port

Give a node a port (attribute).

```fides
pub fn bt_set_port(t: &mut BTree, id: i64, name: str, value: str) {
```

**Parameters:**

- `t`: `&mut BTree`
- `id`: `i64`
- `name`: `str`
- `value`: `str`

*Defined at line 385*

---

## bt_now

The current clock (ms) during a tick.

```fides
pub fn bt_now() -> f64 {
```

**Returns:** `f64`

*Defined at line 420*

---

## bt_port_has

```fides
pub fn bt_port_has(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 424*

---

## bt_port_str

A port as text: the blackboard entry for "{key}", the literal otherwise,
`def` when the node has no such port.

```fides
pub fn bt_port_str(name: str, def: str) -> str {
```

**Parameters:**

- `name`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 430*

---

## bt_port_int

```fides
pub fn bt_port_int(name: str, def: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 443*

---

## bt_port_float

```fides
pub fn bt_port_float(name: str, def: f64) -> f64 {
```

**Parameters:**

- `name`: `str`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 455*

---

## bt_port_key

The blackboard key an output port names ("" when it is not "{key}").

```fides
pub fn bt_port_key(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 468*

---

## bt_port_set_int

Write an output port; false when the port is not "{key}".

```fides
pub fn bt_port_set_int(name: str, v: i64) -> bool {
```

**Parameters:**

- `name`: `str`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 477*

---

## bt_port_set_float

```fides
pub fn bt_port_set_float(name: str, v: f64) -> bool {
```

**Parameters:**

- `name`: `str`
- `v`: `f64`

**Returns:** `bool`

*Defined at line 486*

---

## bt_port_set_str

```fides
pub fn bt_port_set_str(name: str, v: str) -> bool {
```

**Parameters:**

- `name`: `str`
- `v`: `str`

**Returns:** `bool`

*Defined at line 495*

---

## bt_halt

Halt a node and everything under it: running actions get their halt
function, and all state is reset to idle.

```fides
pub fn bt_halt(t: &mut BTree, id: i64) {
```

**Parameters:**

- `t`: `&mut BTree`
- `id`: `i64`

*Defined at line 521*

---

## bt_tick

Tick the whole tree at time now_ms; the root's status.

```fides
pub fn bt_tick(t: &mut BTree, now_ms: f64) -> i64 {
```

**Parameters:**

- `t`: `&mut BTree`
- `now_ms`: `f64`

**Returns:** `i64`

*Defined at line 562*

---

## bt_tick_node

Tick one node (and, through it, its children).

```fides
pub fn bt_tick_node(t: &mut BTree, id: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut BTree`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 574*

---

## bt_dump

A text picture of the tree: one node per line, indented, with its
last status.

```fides
pub fn bt_dump(t: &BTree) -> str {
```

**Parameters:**

- `t`: `&BTree`

**Returns:** `str`

*Defined at line 790*

---

## BtLoad

```fides
pub struct BtLoad {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `tree` | `BTree` |

*Defined at line 807*

---

## bt_from_xml

Build a tree from BehaviorTree.CPP XML with the given leaves.

```fides
pub fn bt_from_xml(text: str, leaves: &Vec<BtLeaf>, bb: i64) -> BtLoad {
```

**Parameters:**

- `text`: `str`
- `leaves`: `&Vec<BtLeaf>`
- `bb`: `i64`

**Returns:** `BtLoad`

*Defined at line 868*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
