# Fides Language — Design Decision Records

> Record of deliberate design choices, why they were made, and what the
> alternatives were. Future sessions should check here before questioning
> an existing design.

---

## ADR-001: Memory Model Hierarchy (Path B)

**Date:** Sep 2026
**Status:** Accepted
**Supersedes:** Vision's original "one memory model (regions only)" claim

### Context

The vision (`docs/reference/vision.md`) originally promised a single memory
model: regions, replacing Rust's ownership + borrowing + RC. In practice,
the implementation grew three mechanisms:

1. **Regions** — `region r { ... }`, compile-time escape detection, bulk free
2. **Borrow checker** — `&mut` exclusivity checking (opt-in via `--borrowck=on`)
3. **RC** — `Rc<T>`/`Arc<T>` runtime reference counting

This created a tension: the vision says "one way to do things," but the
implementation has three. We evaluated two paths:

- **Path A:** Commit to the vision — remove the borrow checker and RC, keep
  only regions. Bold, differentiated, but every graph/cache/callback pattern
  would require `unsafe` raw pointers.
- **Path B:** Keep all three, but establish a clear hierarchy — regions as
  default, borrow checker as opt-in, RC as the safe alternative to `unsafe`.

### Decision

**Path B.** Keep all three memory mechanisms with a documented hierarchy:

| Model | Role | When | Cost |
|-------|------|------|------|
| Regions | Default | Scoped data, predictable lifetime | Zero |
| References | Always available | Borrowed access | Zero (unchecked by default) |
| Borrow checker | Opt-in | Compile-time aliasing safety | Zero (opt-in complexity) |
| RC | Safe escape hatch | Shared ownership, dynamic lifetime | Runtime refcount |
| `unsafe` | Framework-only | Raw pointers, inline asm | N/A (banned in user code) |

### Rationale

The core insight: **RC replaces `unsafe` as the fallback for patterns regions
can't express.**

Patterns that are impossible with regions alone:
- Shared ownership (doubly-linked lists, graphs with cycles, parent/child links)
- Dynamic lifetime (caches, callbacks, observer registration)
- Thread-safe shared state (Arc across spawned threads)
- Self-referential structures

Without RC, these patterns force the user into `unsafe { }` raw pointers —
which violates the vision's "no `unsafe` in user code, ever." With RC, the
escape hatch is memory-safe. This reframes RC from "we copied Rust" to
"we eliminated the need for `unsafe` in user code."

### Consequences

- **Positive:** No `unsafe` needed for any memory-safe pattern. Regions stay
  zero-cost default. Borrow checker is available but not forced.
- **Negative:** Three mechanisms instead of one. Users must learn which to
  use when. Potential for inconsistency across codebases.
- **Mitigation:** Lint warning when RC is used where regions would suffice.
  First-party code (stdlib, compiler) should prefer regions. Document the
  hierarchy clearly in all guides.
- **Reversibility:** If we later choose Path A (regions only), the cost is
  low today: RC is used in 0 stdlib files, 1 test file, and runtime support
  is ~30 lines. The borrow checker is a single isolated pass. Removal is
  ~1-2 days of work. **This cost grows as more code adopts RC.**

### Reversibility guardrails

To keep the door open for Path A:
1. First-party code (stdlib, compiler, tests) must prefer regions over RC.
2. RC usage in stdlib/compiler requires justification in a comment.
3. The borrow checker must remain opt-in, never default.
4. A lint should warn on RC usage that could be replaced with regions.
