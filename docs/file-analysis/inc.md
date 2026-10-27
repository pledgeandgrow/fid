# inc.fd — incremental compilation cache (166 LOC)

> **Re-verified 2026-10-23:** confirmed — cache key is still (path, main-src hash) only; dep edits and flag changes serve stale results. Per-process only.

## Role

In-memory cache of `IrModule`s keyed by `(source_path, source_hash)`:
FNV-1a hashing, `IncrementalCache` (Vec-linear-scan store), and the
`g_cache` module-global with lazy init. Port of `src/inc.rs` (171 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd` | :38 import, :1429 get, :1464 store | `ccompile_source_with_path` — hash → hit: skip to C codegen; miss: full pipeline then store. **Live.** |

## Findings

### F1 — Cache is blind to imports + flags — HIGH (inherited)

Key = `(path, hash_of_main_source)`:

- **Editing an imported module doesn't invalidate** — `use foo` dep
  edits leave `src_hash` unchanged → stale `IrModule` served →
  silent use of outdated code (already flagged in run 12).
- **`borrowck_mode`/flags not in key** — a first `ccompile
  --borrowck=off` caches the module; a later `--borrowck=on` hits
  the cache and **skips borrow-checking entirely**. Same for any
  config knob that changes the pipeline output.
- Rust has the identical key (inc.rs:18-22) — inherited design gap,
  but the flag-blindness is sharper in Fides where borrowck is a
  runtime parameter.

### F2 — Per-process only — INFO (acknowledged)

The cache dies with the process — each `fidesc ccompile` invocation
is a fresh process → always misses. Only helps in-process batch
compiles (the test harness). Header documents this honestly;
`hash_source`/`global_cache_get` overhead is then pure cost on the
CLI path.

### F3 — Port adaptations — LOW

- `pub var g_cache` struct global + `g_cache_init` lazy-init: the
  comment notes codegen_c emits struct-typed globals as zeroed
  pointers — the `ensure_cache_init` shim is the correct workaround.
  Both vars get re-verified on every call.
- `IncrementalCache_clear` pops in a loop (`Vec::new()` can't be
  reassigned — same Vec limitation family).
- FNV-1a constant written decimal (`8834263827529598795`) with the
  hex in a comment — i64-signed-safe ✓.
- Linear scan `get`/`store` — fine at this scale.
- Unbounded: every compiled module is retained forever — in a
  long-lived process (LSP) it's a leak-by-design.

## Verification state

- Call path verified: `ccompile_source_with_path` at :1427-1472 —
  get before pipeline, store inside `Ok` only.
- Hash/key semantics verified against Rust: identical FNV-1a +
  `(path, hash)` shape — all blind spots inherited.
- Module-var lazy init reviewed; can't be exercised end-to-end.

## Next

`interp.fd` — the 2,839-LOC IR interpreter, the reference oracle.
