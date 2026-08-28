s c# Panic/Unwrap Audit — Goal 0.2

> Per-call-site catalog of every `panic!`/`unwrap()`/`expect()` in the Rust
> compiler code, with corrected counts and recommended actions for the Fides
> self-hosting rewrite.
>
> **Audited:** Aug 28, 2026
> **Method:** `grep` for `\.unwrap\(\)`, `\.expect\(`, `panic!` in each target
> file, then manual categorization of every hit with surrounding context.

## Headline Finding — The Roadmap's Counts Were Wrong

The `selfhost-roadmap.md` Goal 0.2 section claimed **285** panicking calls,
with the breakdown:

| File | Roadmap claim | Reality |
|------|---------------|---------|
| parser.rs | 225 ("mostly `self.peek().unwrap()`") | **1 `unwrap()` + 223 custom `expect()` + 0 `panic!`** |
| interp.rs | 47 | 45 `unwrap()` + 2 `panic!` = 47 ✓ |
| typechecker.rs | 9 | 8 `unwrap()` = 8 |
| lower.rs | 3 | 3 ✓ |
| ir.rs | 1 | 1 ✓ |
| **Total** | **285** | **55 real panicking calls** (see breakdown below) |

### Why the parser count collapsed from 225 → 1

The roadmap author confused Rust's panicking `Option::expect` / `Result::expect`
with the parser's **custom `expect()` method** defined at
`src/parser.rs:332`:

```rust
fn expect(&mut self, kind: TokenKind, msg: &str) -> Result<Token, String> {
    if self.check(&kind) {
        Ok(self.advance())
    } else {
        let tok = self.peek_token();
        Err(format!(
            "Parse error at {}:{}: expected {} but got '{}'",
            tok.line, tok.col, msg, tok.kind
        ))
    }
}
```

This returns `Result<Token, String>` and is always called with `?` propagation
(e.g. `self.expect(TokenKind::LBrace, "{")?`). It is **proper error handling**,
not a panic. The 223 `self.expect(...)` call sites are exactly the pattern the
roadmap *wanted* — they already do what the "Key insight" says the Fides rewrite
would achieve. There is nothing to fix here.

The roadmap's "Key insight" ("the parser's 225 unwraps are mostly
`self.peek().unwrap()`") is factually incorrect: parser.rs has **zero**
`peek().unwrap()` calls and exactly **one** `unwrap()` in the entire file.

## Real Panicking Calls: 55 total

After removing the 223 false positives, the actual panicking surface is:

| Category | Count | Files | Risk |
|----------|-------|-------|------|
| `Mutex::lock().unwrap()` (poisoning) | 42 | interp.rs | LOW — only panics if a thread panicked while holding the lock |
| Intentional `panic!` (debug assertions) | 2 | interp.rs | NONE — deliberate use-after-free detection |
| Guarded `unwrap()` (provably safe by local check) | 8 | typechecker.rs (6), lower.rs (1), interp.rs (1) | NONE |
| Logic-assumption `unwrap()` (could panic on bad state) | 3 | typechecker.rs (1), lower.rs (1), ir.rs (1) | LOW |

---

## Per-File Catalog

### parser.rs — 1 real call (was claimed 225)

| Line | Code | Category | Safe? | Action |
|------|------|----------|-------|--------|
| 1334 | `types.into_iter().next().unwrap()` | Guarded — preceded by `if types.len() == 1` | Yes | Replace with `types.remove(0)` or `swap_remove(0)` for clarity; no correctness fix needed |

**Note on the 223 `self.expect(...)` calls:** these call the custom
`expect(kind, msg) -> Result<Token, String>` method (line 332), not Rust's
`Option::expect`. They are already `Result`-based with `?` propagation. **No
action required** — they are the model the Fides rewrite should follow.

### interp.rs — 47 calls (45 `unwrap()` + 2 `panic!`)

#### Mutex poisoning (42 calls) — LOW risk

All 42 are `something.lock().unwrap()`. `Mutex::lock()` returns `Err` only when
the mutex is **poisoned** — i.e., another thread panicked while holding it.
These are in the async/channel/shared-memory simulation code (lines 4648–5280).

| Lines | Object | Context |
|-------|--------|---------|
| 4648, 4677, 4682, 4713, 4718, 4741, 4770, 4775, 4803, 4813, 4817, 4844, 4854, 4859, 4889, 4896, 4917, 4923, 4942, 4946, 4962, 4968 | `self.task_done` / `task_done` | async future completion tracking |
| 4988, 4999–5000, 5016, 5033, 5052, 5067, 5113, 5125, 5142 | `self.shared_memory` | shared memory read/write/CAS |
| 5161, 5177, 5182, 5207, 5212, 5235, 5240, 5261, 5265, 5276, 5280 | `self.channels` / `ch` | channel send/recv/close/status |

**Action for Fides rewrite:** Fides has no `Mutex` type — concurrency is via
`channel.fd` and `sync.fd`. These 42 calls disappear entirely in the rewrite
(replaced by channel operations). No Rust-side fix needed unless we want to
harden the interpreter against poisoned-mutex panics today, which is low value.

#### Intentional panics (2 calls) — deliberate

| Line | Code | Purpose |
|------|------|---------|
| 1181 | `panic!("Use-after-free: loading from address 0x{:x}...")` | Debug-mode use-after-free detection |
| 1247 | `panic!("Use-after-free: storing to address 0x{:x}...")` | Debug-mode use-after-free detection |

Both are inside `if self.debug_mode && self.freed_addresses.contains(&addr)`
guards. They are **intentional safety checks**, not accidental unwraps.

**Action for Fides rewrite:** Translate to Fides `assert` / `panic` with the
same guard. Keep as deliberate runtime checks.

#### Other unwraps (3 calls)

| Line | Code | Category | Safe? | Action |
|------|------|----------|-------|--------|
| 86 | `m.get(*key).unwrap()` | Display formatting (debug print of map) | Mostly — key came from iterating the same map | Replace with `if let Some(v) = m.get(*key)` or `unwrap_or(&"<missing>")` |
| 3075 | `s.chars().nth(idx as usize).unwrap()` | String char indexing | Yes — `idx` is bounds-checked earlier in the function | Replace with `nth_or_err(idx)` returning `Result` |
| 5595 | `star_s.unwrap()` | Glob/wildcard matching | Yes — only reached when `star_s` is `Some` per matching logic | Replace with `if let Some(s) = star_s` for clarity |

### typechecker.rs — 8 calls

| Line | Code | Category | Safe? | Action |
|------|------|----------|-------|--------|
| 33 | `field.chars().next().unwrap()` | Swizzle field access — `field` is a validated non-empty swizzle mask | Yes | Replace with `field.chars().next_or_err()` or guard with `if field.is_empty()` |
| 832 | `self.methods.get(&imp.struct_name).unwrap()` | In `else` branch — entry existence guaranteed by the `if` | Yes | Replace with `if let Some(existing) = self.methods.get(...)` |
| 2157 | `prims.unwrap()` after `prims.is_none()` check | Redundant double-check pattern | Yes | Collapse into `if let Some(prims) = func_primitives.get(...)` |
| 2160 | `prims.unwrap()` | Same block, after the `is_none` guard passed | Yes | Same — use `if let Some` |
| 3198 | `self.scopes.last_mut().unwrap()` | Assumes a scope is on the stack | **FIXED** — replaced with `if let Some` + defensive scope creation | Done |
| 4564 | `a.last().unwrap()` | Guarded by `a.len() >= 2` | Yes | Replace with `a[a.len()-1]` or keep (provably safe) |
| 4565 | `b.first().unwrap()` | Guarded by `b.len() >= 2` | Yes | Replace with `b[0]` or keep |
| 4769 | `name.chars().next().unwrap()` | Guarded by `name.len() == 1` | Yes | Replace with `name.chars().next_or_err()` |

### lower.rs — 3 calls

| Line | Code | Category | Safe? | Action |
|------|------|----------|-------|--------|
| 140 | `field.chars().next().unwrap()` | Swizzle field — non-empty (same as typechecker:33) | Yes | Replace with `next_or_err()` |
| 1108 | `self.current_func.as_mut().unwrap()` | Assumes lowering inside a function | **FIXED** — restructured to compute `region` before mutable borrow, use `if let Some(f)` directly | Done |
| 4349 | `alloca_ty.unwrap()` | `alloca_ty` set earlier in the same path | Yes (by construction) | Replace with `if let Some(ty) = alloca_ty` for defensiveness |

### ir.rs — 1 call

| Line | Code | Category | Safe? | Action |
|------|------|----------|-------|--------|
| 1670 | `block.instructions.last().unwrap()` | Validation pass checking "block ends with terminator" | **FIXED** — added `is_empty()` guard that pushes a validation error for empty blocks | Done |

---

## Summary of Real Action Items

Only **3 calls** represented genuine latent bugs (could panic on real input).
**All 3 have been fixed** (Aug 28, 2026) — `./verify.sh` confirms no regressions:

1. **`typechecker.rs:3198`** — `self.scopes.last_mut().unwrap()` — panics if
   `declare()` is called before any `push_scope()`.
   **Fixed:** Replaced with `if let Some(scope) = self.scopes.last_mut()` +
   defensive `else` that creates a scope if none exists. No unwrap, no panic.

2. **`lower.rs:1108`** — `self.current_func.as_mut().unwrap()` — panics if
   lowering happens outside a function.
   **Fixed:** Restructured to compute `region` before the mutable borrow, then
   use `if let Some(f) = self.current_func.as_mut()` directly. No unwrap, no
   panic.

3. **`ir.rs:1670`** — `block.instructions.last().unwrap()` — panics on an empty
   block during validation.
   **Fixed:** Added `if block.instructions.is_empty()` guard that pushes a
   validation error ("block has no instructions") instead of panicking. The
   existing terminator check runs in the `else` branch.

The remaining 52 calls are either:
- **42 mutex-poisoning unwraps** (interp.rs) — disappear in the Fides rewrite
  (no `Mutex` type; channels replace them).
- **2 intentional panics** (interp.rs) — deliberate use-after-free checks to
  preserve as-is.
- **8 provably-safe guarded unwraps** — cosmetic improvements only.

## Recommendation for the Roadmap

The Goal 0.2 section in `selfhost-roadmap.md` should be corrected to reflect
that the parser is **already** `Result`-based (via the custom `expect()`), and
the real audit work is the 3 latent-bug fixes above plus the cosmetic cleanup of
8 guarded unwraps. The "285 calls" headline overstates the work by ~5×.
