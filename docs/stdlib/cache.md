[All modules](index.md)

# cache -- Fides Documentation

## Contents

- [CacheStats](#CacheStats) (struct)
- [cache_stats_str](#cache_stats_str) (function)
- [LruCache](#LruCache) (struct)
- [lru_new](#lru_new) (function)
- [lru_evict](#lru_evict) (function)
- [lru_get](#lru_get) (function)
- [lru_peek](#lru_peek) (function)
- [lru_has](#lru_has) (function)
- [lru_put](#lru_put) (function)
- [lru_remove](#lru_remove) (function)
- [lru_len](#lru_len) (function)
- [lru_keys](#lru_keys) (function)
- [LfuCache](#LfuCache) (struct)
- [lfu_new](#lfu_new) (function)
- [lfu_evict](#lfu_evict) (function)
- [lfu_get](#lfu_get) (function)
- [lfu_has](#lfu_has) (function)
- [lfu_count](#lfu_count) (function)
- [lfu_put](#lfu_put) (function)
- [lfu_len](#lfu_len) (function)
- [TtlCache](#TtlCache) (struct)
- [ttl_new](#ttl_new) (function)
- [ttl_get](#ttl_get) (function)
- [ttl_sweep](#ttl_sweep) (function)
- [ttl_put](#ttl_put) (function)
- [ttl_put_for](#ttl_put_for) (function)
- [ttl_remaining](#ttl_remaining) (function)
- [ttl_len](#ttl_len) (function)

## CacheStats

Counters common to the caches.

```fides
pub struct CacheStats {
```

**Fields:**

| Name | Type |
|------|------|
| `hits` | `i64` |
| `misses` | `i64` |
| `evictions` | `i64` |
| `expirations` | `i64` |

*Defined at line 31*

---

## cache_stats_str

"hits=3 misses=1 evictions=0 expired=0 ratio=75%"

```fides
pub fn cache_stats_str(s: &CacheStats) -> str {
```

**Parameters:**

- `s`: `&CacheStats`

**Returns:** `str`

*Defined at line 45*

---

## LruCache

Slots in parallel vectors; `prev`/`next` link them most recent first
(head) to least recent (tail); free slots are chained through `next`.

```fides
pub struct LruCache {
```

**Fields:**

| Name | Type |
|------|------|
| `capacity` | `i64` |
| `index` | `HashMap<str` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `prev` | `Vec<i64>` |
| `next` | `Vec<i64>` |
| `head` | `i64` |
| `tail` | `i64` |
| `free` | `i64` |
| `size` | `i64` |
| `stats` | `CacheStats` |

*Defined at line 57*

---

## lru_new

```fides
pub fn lru_new(capacity: i64) -> LruCache {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `LruCache`

*Defined at line 72*

---

## lru_evict

Remove and return the least recently used key ("" when empty).

```fides
pub fn lru_evict(c: &mut LruCache) -> str {
```

**Parameters:**

- `c`: `&mut LruCache`

**Returns:** `str`

*Defined at line 138*

---

## lru_get

The value of `key` ("" on a miss); a hit makes it most recent.

```fides
pub fn lru_get(c: &mut LruCache, key: str) -> str {
```

**Parameters:**

- `c`: `&mut LruCache`
- `key`: `str`

**Returns:** `str`

*Defined at line 151*

---

## lru_peek

Look without counting or reordering.

```fides
pub fn lru_peek(c: &LruCache, key: str) -> str {
```

**Parameters:**

- `c`: `&LruCache`
- `key`: `str`

**Returns:** `str`

*Defined at line 164*

---

## lru_has

```fides
pub fn lru_has(c: &LruCache, key: str) -> bool {
```

**Parameters:**

- `c`: `&LruCache`
- `key`: `str`

**Returns:** `bool`

*Defined at line 171*

---

## lru_put

Insert or replace; the entry becomes most recent. Returns the evicted
key ("" when nothing was evicted).

```fides
pub fn lru_put(c: &mut LruCache, key: str, value: str) -> str {
```

**Parameters:**

- `c`: `&mut LruCache`
- `key`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 177*

---

## lru_remove

Remove `key`; whether it was present.

```fides
pub fn lru_remove(c: &mut LruCache, key: str) -> bool {
```

**Parameters:**

- `c`: `&mut LruCache`
- `key`: `str`

**Returns:** `bool`

*Defined at line 197*

---

## lru_len

```fides
pub fn lru_len(c: &LruCache) -> i64 {
```

**Parameters:**

- `c`: `&LruCache`

**Returns:** `i64`

*Defined at line 207*

---

## lru_keys

Keys from most to least recently used.

```fides
pub fn lru_keys(c: &LruCache) -> Vec<str> {
```

**Parameters:**

- `c`: `&LruCache`

**Returns:** `Vec<str>`

*Defined at line 212*

---

## LfuCache

Per slot: key, value, use count, and links in its count's list (most
recent first). `bhead`/`btail` map a count to its list's ends.

```fides
pub struct LfuCache {
```

**Fields:**

| Name | Type |
|------|------|
| `capacity` | `i64` |
| `index` | `HashMap<str` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `freq` | `Vec<i64>` |
| `prev` | `Vec<i64>` |
| `next` | `Vec<i64>` |
| `bhead` | `HashMap<i64` |
| `btail` | `HashMap<i64` |
| `min_freq` | `i64` |
| `free` | `i64` |
| `size` | `i64` |
| `stats` | `CacheStats` |

*Defined at line 226*

---

## lfu_new

```fides
pub fn lfu_new(capacity: i64) -> LfuCache {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `LfuCache`

*Defined at line 245*

---

## lfu_evict

Remove and return the least frequently used key (least recent among
equals); "" when empty.

```fides
pub fn lfu_evict(c: &mut LfuCache) -> str {
```

**Parameters:**

- `c`: `&mut LfuCache`

**Returns:** `str`

*Defined at line 305*

---

## lfu_get

```fides
pub fn lfu_get(c: &mut LfuCache, key: str) -> str {
```

**Parameters:**

- `c`: `&mut LfuCache`
- `key`: `str`

**Returns:** `str`

*Defined at line 325*

---

## lfu_has

```fides
pub fn lfu_has(c: &LfuCache, key: str) -> bool {
```

**Parameters:**

- `c`: `&LfuCache`
- `key`: `str`

**Returns:** `bool`

*Defined at line 336*

---

## lfu_count

The use count of `key` (0 when absent). A put counts as the first use.

```fides
pub fn lfu_count(c: &LfuCache, key: str) -> i64 {
```

**Parameters:**

- `c`: `&LfuCache`
- `key`: `str`

**Returns:** `i64`

*Defined at line 341*

---

## lfu_put

Insert or replace (a replace counts as a use). Returns the evicted key.

```fides
pub fn lfu_put(c: &mut LfuCache, key: str, value: str) -> str {
```

**Parameters:**

- `c`: `&mut LfuCache`
- `key`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 349*

---

## lfu_len

```fides
pub fn lfu_len(c: &LfuCache) -> i64 {
```

**Parameters:**

- `c`: `&LfuCache`

**Returns:** `i64`

*Defined at line 382*

---

## TtlCache

An LRU whose entries expire `ttl` time units after their last write.

```fides
pub struct TtlCache {
```

**Fields:**

| Name | Type |
|------|------|
| `lru` | `LruCache` |
| `ttl` | `i64` |
| `expires` | `HashMap<str` |
| `stats` | `CacheStats` |

*Defined at line 389*

---

## ttl_new

```fides
pub fn ttl_new(capacity: i64, ttl: i64) -> TtlCache {
```

**Parameters:**

- `capacity`: `i64`
- `ttl`: `i64`

**Returns:** `TtlCache`

*Defined at line 397*

---

## ttl_get

The value of `key` at time `now` ("" when missing or expired).

```fides
pub fn ttl_get(c: &mut TtlCache, key: str, now: i64) -> str {
```

**Parameters:**

- `c`: `&mut TtlCache`
- `key`: `str`
- `now`: `i64`

**Returns:** `str`

*Defined at line 410*

---

## ttl_sweep

Remove every entry expired at `now`; how many went.

```fides
pub fn ttl_sweep(c: &mut TtlCache, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut TtlCache`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 425*

---

## ttl_put

Insert or replace with the default ttl; expired entries are swept
before an LRU eviction. Returns the key evicted for room ("").

```fides
pub fn ttl_put(c: &mut TtlCache, key: str, value: str, now: i64) -> str {
```

**Parameters:**

- `c`: `&mut TtlCache`
- `key`: `str`
- `value`: `str`
- `now`: `i64`

**Returns:** `str`

*Defined at line 441*

---

## ttl_put_for

Insert or replace with its own lifetime.

```fides
pub fn ttl_put_for(c: &mut TtlCache, key: str, value: str, now: i64, ttl: i64) -> str {
```

**Parameters:**

- `c`: `&mut TtlCache`
- `key`: `str`
- `value`: `str`
- `now`: `i64`
- `ttl`: `i64`

**Returns:** `str`

*Defined at line 446*

---

## ttl_remaining

Time left for `key` at `now` (0 when missing or expired).

```fides
pub fn ttl_remaining(c: &TtlCache, key: str, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&TtlCache`
- `key`: `str`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 462*

---

## ttl_len

```fides
pub fn ttl_len(c: &TtlCache) -> i64 {
```

**Parameters:**

- `c`: `&TtlCache`

**Returns:** `i64`

*Defined at line 473*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
