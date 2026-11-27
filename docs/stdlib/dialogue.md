[All modules](index.md)

# dialogue -- Fides Documentation

## Contents

- [DLG_EQ:](#DLG_EQ:) (const)
- [DLG_NE:](#DLG_NE:) (const)
- [DLG_LT:](#DLG_LT:) (const)
- [DLG_LE:](#DLG_LE:) (const)
- [DLG_GT:](#DLG_GT:) (const)
- [DLG_GE:](#DLG_GE:) (const)
- [DLG_SET:](#DLG_SET:) (const)
- [DLG_ADD:](#DLG_ADD:) (const)
- [DLG_LINE:](#DLG_LINE:) (const)
- [DLG_CHOICE:](#DLG_CHOICE:) (const)
- [DLG_END:](#DLG_END:) (const)
- [DlgCond](#DlgCond) (struct)
- [DlgEffect](#DlgEffect) (struct)
- [DlgChoice](#DlgChoice) (struct)
- [DlgNode](#DlgNode) (struct)
- [DlgGraph](#DlgGraph) (struct)
- [dlg_graph](#dlg_graph) (function)
- [dlg_node](#dlg_node) (function)
- [dlg_node_line](#dlg_node_line) (function)
- [dlg_choice](#dlg_choice) (function)
- [dlg_add_node](#dlg_add_node) (function)
- [dlg_find](#dlg_find) (function)
- [dlg_check](#dlg_check) (function)
- [DlgRunner](#DlgRunner) (struct)
- [dlg_runner](#dlg_runner) (function)
- [dlg_get](#dlg_get) (function)
- [dlg_set](#dlg_set) (function)
- [dlg_cond_holds](#dlg_cond_holds) (function)
- [dlg_start](#dlg_start) (function)
- [dlg_interpolate](#dlg_interpolate) (function)
- [dlg_speaker](#dlg_speaker) (function)
- [dlg_line_text](#dlg_line_text) (function)
- [dlg_advance](#dlg_advance) (function)
- [dlg_options](#dlg_options) (function)
- [dlg_option_text](#dlg_option_text) (function)
- [dlg_choose](#dlg_choose) (function)
- [DlgParse](#DlgParse) (struct)
- [dlg_parse](#dlg_parse) (function)

## DLG_EQ:

```fides
pub const DLG_EQ: i64 = 0;
```

*Defined at line 41*

---

## DLG_NE:

```fides
pub const DLG_NE: i64 = 1;
```

*Defined at line 42*

---

## DLG_LT:

```fides
pub const DLG_LT: i64 = 2;
```

*Defined at line 43*

---

## DLG_LE:

```fides
pub const DLG_LE: i64 = 3;
```

*Defined at line 44*

---

## DLG_GT:

```fides
pub const DLG_GT: i64 = 4;
```

*Defined at line 45*

---

## DLG_GE:

```fides
pub const DLG_GE: i64 = 5;
```

*Defined at line 46*

---

## DLG_SET:

```fides
pub const DLG_SET: i64 = 0;
```

*Defined at line 48*

---

## DLG_ADD:

```fides
pub const DLG_ADD: i64 = 1;
```

*Defined at line 49*

---

## DLG_LINE:

```fides
pub const DLG_LINE: i64 = 0;
```

*Defined at line 51*

---

## DLG_CHOICE:

```fides
pub const DLG_CHOICE: i64 = 1;
```

*Defined at line 52*

---

## DLG_END:

```fides
pub const DLG_END: i64 = 2;
```

*Defined at line 53*

---

## DlgCond

```fides
pub struct DlgCond {
```

**Fields:**

| Name | Type |
|------|------|
| `var_name` | `str` |
| `op` | `i64` |
| `value` | `i64` |

*Defined at line 55*

---

## DlgEffect

```fides
pub struct DlgEffect {
```

**Fields:**

| Name | Type |
|------|------|
| `var_name` | `str` |
| `kind` | `i64` |
| `value` | `i64` |

*Defined at line 61*

---

## DlgChoice

```fides
pub struct DlgChoice {
```

**Fields:**

| Name | Type |
|------|------|
| `text` | `str` |
| `target` | `str` |
| `conds` | `Vec<DlgCond>` |
| `effects` | `Vec<DlgEffect>` |

*Defined at line 67*

---

## DlgNode

```fides
pub struct DlgNode {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `speakers` | `Vec<str>` |
| `lines` | `Vec<str>` |
| `effects` | `Vec<DlgEffect>` |
| `choices` | `Vec<DlgChoice>` |
| `next` | `str,  // "" or "END": the dialogue ends after the lines` |

*Defined at line 75*

---

## DlgGraph

```fides
pub struct DlgGraph {
```

**Fields:**

| Name | Type |
|------|------|
| `nodes` | `Vec<DlgNode>` |

*Defined at line 84*

---

## dlg_graph

```fides
pub fn dlg_graph() -> DlgGraph {
```

**Returns:** `DlgGraph`

*Defined at line 88*

---

## dlg_node

```fides
pub fn dlg_node(id: str) -> DlgNode {
```

**Parameters:**

- `id`: `str`

**Returns:** `DlgNode`

*Defined at line 94*

---

## dlg_node_line

```fides
pub fn dlg_node_line(n: &mut DlgNode, speaker: str, text: str) {
```

**Parameters:**

- `n`: `&mut DlgNode`
- `speaker`: `str`
- `text`: `str`

*Defined at line 100*

---

## dlg_choice

```fides
pub fn dlg_choice(text: str, target: str) -> DlgChoice {
```

**Parameters:**

- `text`: `str`
- `target`: `str`

**Returns:** `DlgChoice`

*Defined at line 105*

---

## dlg_add_node

```fides
pub fn dlg_add_node(g: &mut DlgGraph, n: DlgNode) {
```

**Parameters:**

- `g`: `&mut DlgGraph`
- `n`: `DlgNode`

*Defined at line 111*

---

## dlg_find

Index of node `id`, -1 when there is none.

```fides
pub fn dlg_find(g: &DlgGraph, id: str) -> i64 {
```

**Parameters:**

- `g`: `&DlgGraph`
- `id`: `str`

**Returns:** `i64`

*Defined at line 116*

---

## dlg_check

Every target (choice or next) that names no node, as "from -> to".

```fides
pub fn dlg_check(g: &DlgGraph) -> Vec<str> {
```

**Parameters:**

- `g`: `&DlgGraph`

**Returns:** `Vec<str>`

*Defined at line 128*

---

## DlgRunner

```fides
pub struct DlgRunner {
```

**Fields:**

| Name | Type |
|------|------|
| `var_names` | `Vec<str>` |
| `var_vals` | `Vec<i64>` |
| `node` | `i64` |
| `line` | `i64` |
| `state` | `i64` |
| `visited` | `Vec<i64>,  // times each node was entered` |

*Defined at line 151*

---

## dlg_runner

```fides
pub fn dlg_runner(g: &DlgGraph) -> DlgRunner {
```

**Parameters:**

- `g`: `&DlgGraph`

**Returns:** `DlgRunner`

*Defined at line 160*

---

## dlg_get

```fides
pub fn dlg_get(r: &DlgRunner, name: str) -> i64 {
```

**Parameters:**

- `r`: `&DlgRunner`
- `name`: `str`

**Returns:** `i64`

*Defined at line 172*

---

## dlg_set

```fides
pub fn dlg_set(r: &mut DlgRunner, name: str, value: i64) {
```

**Parameters:**

- `r`: `&mut DlgRunner`
- `name`: `str`
- `value`: `i64`

*Defined at line 183*

---

## dlg_cond_holds

```fides
pub fn dlg_cond_holds(r: &DlgRunner, c: &DlgCond) -> bool {
```

**Parameters:**

- `r`: `&DlgRunner`
- `c`: `&DlgCond`

**Returns:** `bool`

*Defined at line 196*

---

## dlg_start

Start at node `id` ("" = the first node).

```fides
pub fn dlg_start(r: &mut DlgRunner, g: &DlgGraph, id: str) {
```

**Parameters:**

- `r`: `&mut DlgRunner`
- `g`: `&DlgGraph`
- `id`: `str`

*Defined at line 268*

---

## dlg_interpolate

Replace every `{name}` with the variable's value.

```fides
pub fn dlg_interpolate(r: &DlgRunner, text: str) -> str {
```

**Parameters:**

- `r`: `&DlgRunner`
- `text`: `str`

**Returns:** `str`

*Defined at line 281*

---

## dlg_speaker

The speaker of the current line ("" when none or not on a line).

```fides
pub fn dlg_speaker(r: &DlgRunner, g: &DlgGraph) -> str {
```

**Parameters:**

- `r`: `&DlgRunner`
- `g`: `&DlgGraph`

**Returns:** `str`

*Defined at line 305*

---

## dlg_line_text

The current line with variables filled in ("" when not on a line).

```fides
pub fn dlg_line_text(r: &DlgRunner, g: &DlgGraph) -> str {
```

**Parameters:**

- `r`: `&DlgRunner`
- `g`: `&DlgGraph`

**Returns:** `str`

*Defined at line 313*

---

## dlg_advance

Go past the current line.

```fides
pub fn dlg_advance(r: &mut DlgRunner, g: &DlgGraph) {
```

**Parameters:**

- `r`: `&mut DlgRunner`
- `g`: `&DlgGraph`

*Defined at line 321*

---

## dlg_options

Indexes (into the node's choices) of the choices offered now.

```fides
pub fn dlg_options(r: &DlgRunner, g: &DlgGraph) -> Vec<i64> {
```

**Parameters:**

- `r`: `&DlgRunner`
- `g`: `&DlgGraph`

**Returns:** `Vec<i64>`

*Defined at line 330*

---

## dlg_option_text

The text of option `k` of `dlg_options` with variables filled in.

```fides
pub fn dlg_option_text(r: &DlgRunner, g: &DlgGraph, k: i64) -> str {
```

**Parameters:**

- `r`: `&DlgRunner`
- `g`: `&DlgGraph`
- `k`: `i64`

**Returns:** `str`

*Defined at line 357*

---

## dlg_choose

Pick option `k` of `dlg_options`: run its effects, go to its target.
False (nothing happens) when `k` is not offered.

```fides
pub fn dlg_choose(r: &mut DlgRunner, g: &DlgGraph, k: i64) -> bool {
```

**Parameters:**

- `r`: `&mut DlgRunner`
- `g`: `&DlgGraph`
- `k`: `i64`

**Returns:** `bool`

*Defined at line 367*

---

## DlgParse

```fides
pub struct DlgParse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `graph` | `DlgGraph` |

*Defined at line 380*

---

## dlg_parse

Parse a dialogue script (format in the header). Targets that name no
node are an error.

```fides
pub fn dlg_parse(src: str) -> DlgParse {
```

**Parameters:**

- `src`: `str`

**Returns:** `DlgParse`

*Defined at line 586*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
