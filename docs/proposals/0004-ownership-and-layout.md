# 0004: One ownership rule, one data layout

- **Status:** accepted; implemented (Nov 7), not yet built or tested.
  Done: counted strings (core, C and native runtimes), Vec/HashMap element
  ownership by type id, the lowering's ownership pass with generated
  drop/dup functions (`LOWER_RC`), null-free builtins with `Option`
  wrappers, counted copies of strings from C, typed `HashMap.get`, the leak
  gate (`scripts/leak-check.sh`, `tests/leaks`). Left: structs inside a
  `Vec` stay one pointer per element (a heap copy each: a pointer into the
  buffer would dangle after a push, which nothing here can check), and the
  compiler's own maps are still `i64` handles (counted keys and values,
  never dropped).
- **Decision:** accepted on 2026-11-07. Replaces the memory parts of 0001:
  copy-on-write stays as the way a write to a shared buffer behaves, but who
  shares and who frees is decided by one rule, inserted by the compiler.

## Problem

Freeing was spread over three mechanisms that did not know about each other:

- manual `p_free`;
- a pass after lowering that frees locals it can prove are finished
  (`lw_auto_drop_vecs`, `lw_auto_drop_strs`: Vec, HashMap and some strings,
  elements only when every element was fresh);
- copy-on-write shares inserted at some copies (`LOWER_COW`), never at call
  arguments, `push`/`insert`/`set` arguments, enum payloads or `StructCopy`,
  and never dropped when the copy was held by a struct.

Leaks and double frees kept coming back (AGENTS.md known bugs 20–25), the
self-compile used 4.7 GB with copy-on-write on, and `var b = a` aliased with
it off. Data had the same problem: one machine word per element, strings
whose header was guessed from a magic number, and builtins returning a null
`str` that `s == 0` (a text comparison) could not detect.

## Change

### Ownership

1. **Every value of a managed type has exactly one owner.** Managed types:
   `str`, `Vec<T>`, `HashMap<K, V>`, and any struct, enum, tuple or generic
   instance with a managed field or payload.
2. **A variable owns its value.** Reading a variable borrows it: no count
   changes. Using the value where it is kept — stored in another variable, a
   field, an element (`push`, `insert`, `set`, a literal), returned — takes a
   copy (`dup`), or **moves** it when that was the variable's last use (the
   variable is then cleared, so its drop does nothing).
3. **A variable is dropped when its scope ends** (block end, `return`,
   `break`, `continue`, the `?` error path), and its old value is dropped
   when it is assigned.
4. **Temporaries** (a call's result, a concatenation, a literal struct) are
   owned by the statement that made them and dropped at its end unless they
   were kept.
5. **Parameters are borrowed** (Swift's default convention): the caller keeps
   ownership, the callee neither counts nor drops. A callee that assigns or
   changes a by-value parameter first takes its own copy (a `dup` at entry,
   dropped at its exit) — copy-on-write makes that copy O(1).
6. **Collections own their elements.** `push`/`insert`/`set` take an owned
   value; `pop`/`remove` give one back; dropping the last owner of a buffer
   drops its elements; replacing an element drops the old one.
7. **A write to a shared buffer copies it first** (copy-on-write, unchanged):
   `Vec` and `HashMap` keep a header per owner and count owners of the data.
8. Values sent to another thread are copied, so counts are never shared
   between threads and stay plain (non-atomic).

`dup` and `drop` are calls the lowering inserts; the old freeing passes and
the copy-on-write share heuristics are gone. `Rc`/`Arc` remain for state that
is meant to be shared and changed.

### Layout

- **Strings:** a heap string is `[count][P_STR_MAGIC][length] bytes NUL`
  (`s-24`, `s-16`, `s-8`); a literal is `[P_STR_STATIC][length] bytes NUL`
  and is never counted or freed. Every producer — core, the C runtime, the
  native runtime, the interpreter — allocates through the one function
  (`p_str_alloc`); strings that come from C are copied in at the boundary
  (`p_str_from_c`).
- **No null `str`.** Builtins that can fail have raw names typed `i64`
  (`p_file_read`, `p_getenv`, `p_argv`, `p_read_line`, `p_read_stdin`) and
  safe wrappers in core: `file_read(path) -> Option<str>`, `env_var(name) ->
  Option<str>`, `arg(i) -> Option<str>`, `argv(i) -> str` (panics when out of
  range, like indexing), `read_line() -> Option<str>`. Comparing a `str` with
  an integer is a type error. An `extern "C"` function declared to return
  `str` gets a copy of what C returned (`""` for null).
- **Vec header:** `{data, len, cap, code, tid}` (40 bytes). `code` is the
  element size (release D14: 1/2/4-byte integers and bools are stored at
  their size); `tid` says how to drop and copy an element: 0 nothing, 1
  `str`, 2 `Vec`, 3 `HashMap`, 16 and up a struct or enum the compiler
  numbers, dispatched through `__p_elem_drop`/`__p_elem_dup`, which core
  declares and the lowering fills in. A shared buffer's box is `{owners,
  cap}`.
- **HashMap header:** `{keys, vals, len, cap, shared, intkeys, ktid, vtid}`
  (64 bytes). `.get` gives the value type, not `i64`.

## What it breaks

- Code that read `p_file_read`/`p_getenv`/`p_argv` as `str`: use the
  wrappers.
- Code that relied on `var b = a` aliasing a `Vec` or a `HashMap`: pass
  `&mut` instead.
- `(s as i64) == 0` still compiles (it is an unsafe-looking cast), but no
  safe API returns a null `str` any more.

## Alternatives

- **A tracing GC:** simplest for users, but pauses break `@realtime`,
  `@firmware` and constant-time code.
- **Rust's ownership with lifetimes:** no counts at all, but every user pays
  in annotations, and Fides references are already second-class (they cannot
  be stored), which gives most of the safety; counting covers the rest.
  Because references cannot be stored, a heap value cannot reach itself, so
  counting frees everything without a cycle collector.

## Implementation

Core first (strings with counts, `Vec`/`HashMap` element ownership and
`tid`), then the runtimes (C, native, interpreter: every string producer
headered), then the lowering (type ids and generated drop/dup functions,
ownership insertion, the old passes removed), then the null-free builtins
and their callers, then typed HashMap in the compiler's own code. Checked at
the end: the 3-stage fixpoint, the suite, and every test run with the leak
report (`P_LEAKS`) showing no block left at exit.
