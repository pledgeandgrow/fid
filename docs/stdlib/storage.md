# storage — Fides Documentation

## Contents

- [MerkleTree](#merkletree) (struct)
- [merkle_tree_new](#merkle_tree_new) (function)
- [merkle_tree_add](#merkle_tree_add) (function)
- [merkle_tree_build](#merkle_tree_build) (function)
- [merkle_tree_root](#merkle_tree_root) (function)
- [merkle_tree_root_hex](#merkle_tree_root_hex) (function)
- [merkle_tree_leaf_count](#merkle_tree_leaf_count) (function)
- [merkle_tree_verify_data](#merkle_tree_verify_data) (function)
- [KVEntry](#kventry) (struct)
- [KVStore](#kvstore) (struct)
- [kv_store_new](#kv_store_new) (function)
- [kv_store_set](#kv_store_set) (function)
- [kv_store_get](#kv_store_get) (function)
- [kv_store_has](#kv_store_has) (function)
- [kv_store_delete](#kv_store_delete) (function)
- [kv_store_count](#kv_store_count) (function)
- [kv_store_state_hash](#kv_store_state_hash) (function)

## MerkleTree

A Merkle tree — uses parallel arrays for node data.

```fides
pub struct MerkleTree {
```

**Fields:**

| Name | Type |
|------|------|
| `hashes` | `Vec<i64>` |
| `left_children` | `Vec<i64>` |
| `right_children` | `Vec<i64>` |
| `is_leaf_flags` | `Vec<i64>` |
| `leaf_data` | `Vec<i64>` |
| `leaves` | `Vec<i64>` |
| `root_index` | `i64` |
| `node_count` | `i64` |

*Defined at line 84*

---

## merkle_tree_new

Create a new empty Merkle tree.

```fides
pub fn merkle_tree_new() -> MerkleTree {
```

**Returns:** `MerkleTree`

*Defined at line 119*

---

## merkle_tree_add

Add a data leaf to the tree. Must call merkle_tree_build() after
adding all leaves to compute the root.

```fides
pub fn merkle_tree_add(tree: &mut MerkleTree, data: str) -> i64 {
```

**Parameters:**

- `tree`: `&mut MerkleTree`
- `data`: `str`

**Returns:** `i64`

*Defined at line 137*

---

## merkle_tree_build

Build the tree from leaves and compute the root.
Must be called after adding all leaves.

```fides
pub fn merkle_tree_build(tree: &mut MerkleTree) -> i64 {
```

**Parameters:**

- `tree`: `&mut MerkleTree`

**Returns:** `i64`

*Defined at line 167*

---

## merkle_tree_root

Get the root hash of the tree. Returns empty Vec if tree is empty.

```fides
pub fn merkle_tree_root(tree: MerkleTree) -> Vec<i64> {
```

**Parameters:**

- `tree`: `MerkleTree`

**Returns:** `Vec<i64>`

*Defined at line 209*

---

## merkle_tree_root_hex

Get the root hash as a hex string.

```fides
pub fn merkle_tree_root_hex(tree: MerkleTree) -> str {
```

**Parameters:**

- `tree`: `MerkleTree`

**Returns:** `str`

*Defined at line 217*

---

## merkle_tree_leaf_count

Get the number of leaves in the tree.

```fides
pub fn merkle_tree_leaf_count(tree: MerkleTree) -> i64 {
```

**Parameters:**

- `tree`: `MerkleTree`

**Returns:** `i64`

*Defined at line 226*

---

## merkle_tree_verify_data

Verify that a piece of data is in the tree by recomputing its hash
and checking against leaf hashes.

```fides
pub fn merkle_tree_verify_data(tree: MerkleTree, data: str) -> bool {
```

**Parameters:**

- `tree`: `MerkleTree`
- `data`: `str`

**Returns:** `bool`

*Defined at line 232*

---

## KVEntry

A key-value entry.

```fides
pub struct KVEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `value` | `i64` |

*Defined at line 256*

---

## KVStore

A simple key-value store.

```fides
pub struct KVStore {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `values` | `Vec<i64>` |
| `count` | `i64` |

*Defined at line 262*

---

## kv_store_new

Create a new empty KV store.

```fides
pub fn kv_store_new() -> KVStore {
```

**Returns:** `KVStore`

*Defined at line 269*

---

## kv_store_set

Set a key-value pair. If key exists, updates the value.

```fides
pub fn kv_store_set(store: &mut KVStore, key: str, value: i64) -> void {
```

**Parameters:**

- `store`: `&mut KVStore`
- `key`: `str`
- `value`: `i64`

**Returns:** `void`

*Defined at line 276*

---

## kv_store_get

Get a value by key. Returns -1 if key not found.

```fides
pub fn kv_store_get(store: KVStore, key: str) -> i64 {
```

**Parameters:**

- `store`: `KVStore`
- `key`: `str`

**Returns:** `i64`

*Defined at line 289*

---

## kv_store_has

Check if a key exists.

```fides
pub fn kv_store_has(store: KVStore, key: str) -> bool {
```

**Parameters:**

- `store`: `KVStore`
- `key`: `str`

**Returns:** `bool`

*Defined at line 299*

---

## kv_store_delete

Delete a key. Returns true if key was found and deleted.

```fides
pub fn kv_store_delete(store: &mut KVStore, key: str) -> bool {
```

**Parameters:**

- `store`: `&mut KVStore`
- `key`: `str`

**Returns:** `bool`

*Defined at line 309*

---

## kv_store_count

Get the number of entries.

```fides
pub fn kv_store_count(store: KVStore) -> i64 {
```

**Parameters:**

- `store`: `KVStore`

**Returns:** `i64`

*Defined at line 329*

---

## kv_store_state_hash

Compute a hash of all key-value pairs (for state root verification).

```fides
pub fn kv_store_state_hash(store: KVStore) -> Vec<i64> {
```

**Parameters:**

- `store`: `KVStore`

**Returns:** `Vec<i64>`

*Defined at line 334*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
