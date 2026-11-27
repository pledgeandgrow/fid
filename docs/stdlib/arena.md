[All modules](index.md)

# arena -- Fides Documentation

## Contents

- [arena_new](#arena_new) (function)
- [arena_alloc](#arena_alloc) (function)
- [arena_save](#arena_save) (function)
- [arena_restore](#arena_restore) (function)
- [arena_free](#arena_free) (function)
- [region_new](#region_new) (function)
- [region_alloc](#region_alloc) (function)
- [region_alloc_zeroed](#region_alloc_zeroed) (function)
- [region_free](#region_free) (function)
- [region_free_one](#region_free_one) (function)
- [with_arena](#with_arena) (function)
- [mem_copy](#mem_copy) (function)
- [mem_zero](#mem_zero) (function)
- [mem_set](#mem_set) (function)
- [arena_used](#arena_used) (function)
- [arena_used_since](#arena_used_since) (function)
- [region_count](#region_count) (function)
- [frame_new](#frame_new) (function)
- [frame_begin](#frame_begin) (function)
- [frame_alloc](#frame_alloc) (function)
- [frame_end](#frame_end) (function)
- [frame_free](#frame_free) (function)
- [frame_alloc_count](#frame_alloc_count) (function)
- [http_request_arena_new](#http_request_arena_new) (function)
- [http_request_alloc](#http_request_alloc) (function)
- [http_request_arena_free](#http_request_arena_free) (function)

## arena_new

Create a new arena. Returns an arena handle (mark for restore).
The arena is process-global — all arenas share the same bump allocator.
Use arena_save/arena_restore to manage scopes within the arena.

```fides
pub fn arena_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 45*

---

## arena_alloc

Allocate `size` bytes from the arena. Returns a raw pointer (i64).
The memory is uninitialized. O(1) — just bumps a pointer.
The memory is freed when the arena is restored or freed.

```fides
pub fn arena_alloc(_arena_mark: i64, size: i64) -> i64 {
```

**Parameters:**

- `_arena_mark`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 52*

---

## arena_save

Save the current arena position. Returns a mark that can be passed
to arena_restore to free everything allocated after this point.

```fides
pub fn arena_save() -> i64 {
```

**Returns:** `i64`

*Defined at line 64*

---

## arena_restore

Restore the arena to a previously saved mark.
Frees all memory allocated after the mark was taken.
O(1) — just resets the bump pointer.

```fides
pub fn arena_restore(mark: i64) -> void {
```

**Parameters:**

- `mark`: `i64`

**Returns:** `void`

*Defined at line 71*

---

## arena_free

Free the entire arena. Equivalent to arena_restore(0).
After this, all pointers obtained from arena_alloc are invalid.

```fides
pub fn arena_free(arena_mark: i64) -> void {
```

**Parameters:**

- `arena_mark`: `i64`

**Returns:** `void`

*Defined at line 77*

---

## region_new

Region header — a 16-byte block {head, count}. Every allocation made
by region_alloc prepends a 16-byte node {next, size} to the list and
returns the payload just past it, so region_free only has to walk the
list and p_free each node — no side table, no Vec.

[header: head -> node -> node -> ...]
node = { next: i64, size: i64, payload[size] }
Create a new region. Returns a handle to use with region_alloc and
region_free.

```fides
pub fn region_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 100*

---

## region_alloc

Allocate `size` bytes within a region. Returns a raw pointer.
The memory is tracked and freed when region_free is called.

```fides
pub fn region_alloc(region: i64, size: i64) -> i64 {
```

**Parameters:**

- `region`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 122*

---

## region_alloc_zeroed

Allocate `size` zeroed bytes within a region.

```fides
pub fn region_alloc_zeroed(region: i64, size: i64) -> i64 {
```

**Parameters:**

- `region`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 151*

---

## region_free

Free all allocations in a region, then the region header itself.
O(n) where n is the number of live allocations — typically called once
at scope exit. Every pointer from this region is invalid afterwards.

```fides
pub fn region_free(region: i64) -> void {
```

**Parameters:**

- `region`: `i64`

**Returns:** `void`

*Defined at line 164*

---

## region_free_one

Free a single allocation within a region: unlinks its node and frees
it. Freeing an untracked or null pointer is a no-op — it is NOT freed,
so a pointer that belongs to another region stays safe.

```fides
pub fn region_free_one(region: i64, ptr: i64) -> void {
```

**Parameters:**

- `region`: `i64`
- `ptr`: `i64`

**Returns:** `void`

*Defined at line 201*

---

## with_arena

Run a function with a fresh arena, then free the arena.
All allocations made within the function via arena_alloc are
automatically freed when the function returns.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
with_arena(fn() -> i64 {
  var p = arena_alloc(0, 64);
  // ... use p ...
  return 0;
});
```

```fides
pub fn with_arena(f: fn() -> i64) -> i64 {
```

**Parameters:**

- `f`: `fn() -> i64`

**Returns:** `i64`

*Defined at line 261*

---

## mem_copy

```fides
pub fn mem_copy(dst: i64, src: i64, n: i64) -> void {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

**Returns:** `void`

*Defined at line 277*

---

## mem_zero

```fides
pub fn mem_zero(dst: i64, n: i64) -> void {
```

**Parameters:**

- `dst`: `i64`
- `n`: `i64`

**Returns:** `void`

*Defined at line 295*

---

## mem_set

```fides
pub fn mem_set(dst: i64, val: i64, n: i64) -> void {
```

**Parameters:**

- `dst`: `i64`
- `val`: `i64`
- `n`: `i64`

**Returns:** `void`

*Defined at line 303*

---

## arena_used

The current arena bump offset — equal to the total bytes the arena
has handed out since its last restore (a save mark is that offset).

```fides
pub fn arena_used() -> i64 {
```

**Returns:** `i64`

*Defined at line 311*

---

## arena_used_since

Bytes handed out since `mark` was saved (0 when the arena has since
been restored to or before the mark).

```fides
pub fn arena_used_since(mark: i64) -> i64 {
```

**Parameters:**

- `mark`: `i64`

**Returns:** `i64`

*Defined at line 317*

---

## region_count

Get the number of live allocations tracked by a region.

```fides
pub fn region_count(region: i64) -> i64 {
```

**Parameters:**

- `region`: `i64`

**Returns:** `i64`

*Defined at line 326*

---

## frame_new

Create a new frame allocator. Returns a FrameArena with mark=0.
Call frame_begin at the start of each iteration, frame_end at the end.

```fides
pub fn frame_new() -> FrameArena {
```

**Returns:** `FrameArena`

*Defined at line 368*

---

## frame_begin

Begin a new frame. Saves the current arena position.
All allocations between frame_begin and frame_end will be freed
when frame_end is called.

```fides
pub fn frame_begin(fa: &mut FrameArena) -> void {
```

**Parameters:**

- `fa`: `&mut FrameArena`

**Returns:** `void`

*Defined at line 377*

---

## frame_alloc

Allocate `size` bytes within the current frame.
The memory is freed when frame_end is called.
Must be called between frame_begin and frame_end.

```fides
pub fn frame_alloc(fa: &mut FrameArena, size: i64) -> i64 {
```

**Parameters:**

- `fa`: `&mut FrameArena`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 385*

---

## frame_end

End the current frame. Frees all memory allocated since frame_begin.
The backing memory is reused for the next frame (no actual free —
just resets the bump pointer).

```fides
pub fn frame_end(fa: FrameArena) -> void {
```

**Parameters:**

- `fa`: `FrameArena`

**Returns:** `void`

*Defined at line 397*

---

## frame_free

Free the frame allocator's backing memory.
Call this when you're done with the frame allocator (e.g. when
the game loop exits).

```fides
pub fn frame_free(fa: FrameArena) -> void {
```

**Parameters:**

- `fa`: `FrameArena`

**Returns:** `void`

*Defined at line 404*

---

## frame_alloc_count

Get the number of allocations in the current frame.

```fides
pub fn frame_alloc_count(fa: FrameArena) -> i64 {
```

**Parameters:**

- `fa`: `FrameArena`

**Returns:** `i64`

*Defined at line 409*

---

## http_request_arena_new

Create a new HTTP request arena. Returns a mark for later restore.
All allocations made via http_request_alloc will be freed when
http_request_arena_free is called.

```fides
pub fn http_request_arena_new() -> http_request_arena {
```

**Returns:** `http_request_arena`

*Defined at line 437*

---

## http_request_alloc

Allocate `size` bytes within an HTTP request arena.
The memory is freed when http_request_arena_free is called.

```fides
pub fn http_request_alloc(_arena: http_request_arena, size: i64) -> i64 {
```

**Parameters:**

- `_arena`: `http_request_arena`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 443*

---

## http_request_arena_free

Free an HTTP request arena. All allocations made via http_request_alloc
are freed at once. Call this after the response has been sent.

```fides
pub fn http_request_arena_free(arena: http_request_arena) -> void {
```

**Parameters:**

- `arena`: `http_request_arena`

**Returns:** `void`

*Defined at line 453*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
