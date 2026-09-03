# Fides Self-Hosted Compiler — Documentation

> **Read `COORDINATION.md` FIRST, before any other file in this folder.**
> It tracks active sessions, architectural decisions, and file ownership.
> Without it, parallel agents will conflict and undo each other's work.

## Files in this folder

| File | Purpose | Read order |
|------|---------|------------|
| `COORDINATION.md` | **READ FIRST.** Active sessions, architectural decisions, file ownership, pre-flight checklist | 1 |
| `vision-analysis.md` | Are we on the right path? What the language can do now, what's missing | 2 |
| `capability-comparison.md` | What Fides can do today + honest comparison vs C/Rust/Zig/Go/Python | 3 |
| `path-to-fixpoint.md` | **The 7 steps from current state to self-reproduction** | 4 |
| `CURRENT.md` (repo root) | What are we doing right now (not in this folder) | 5 |
| `architecture.md` | File-by-file breakdown of every `compiler/*.fd` module | 4 |
| `language-constraints.md` | What the Fides language must respect: types, effects, regions, patterns, closures, FFI, defer, try | 5 |
| `calling-conventions.md` | SRET, enum layout, struct copy, Box semantics, Vec handling — the low-level machinery | 6 |
| `build-pipeline.md` | How to build, test, bootstrap, and verify the self-hosted compiler | 7 |
| `known-bugs.md` | Known bugs, gotchas, and things that have been fixed (do not re-introduce) | 8 |
| `agent-guide.md` | Best practices and rules for agents working on the compiler | 9 |

> **Note:** `calling-conventions.md` and `known-bugs.md` contain some outdated
> content about the inline + memcpy approach for enum payloads. The current
> direction is the **pointer-based approach** — see `COORDINATION.md` [D1].
> Those docs will be updated when the approach is finalized.

## Quick Start

```bash
# 1. Build the Rust host compiler
cargo build --release

# 2. Use it to compile the self-hosted compiler (Fides → C → .exe)
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
tcc -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def
# (patch PE stack to 256MB — see build-pipeline.md)

# 3. Test the self-hosted compiler
./verify-selfhost.sh

# 4. Verify the Rust compiler's test suite still passes
./verify.sh
```

## Current Status (Sep 2026)

- **Crash fixed** — the `STATUS_ACCESS_VIOLATION` crash is resolved
- **SRET fixed** — struct returns via out-parameter convention now work
- **Direction changed** — enum payloads will store **pointers** to structs, not inline data. See `COORDINATION.md` [D1].
- **23/23 self-host tests pass** on interpreter + PE emitter
- **Bootstrap fixpoint not yet achieved** — blocked by enum payload approach being incomplete
- **`src/lower.rs` partially updated** — `pattern_binding_ir_types` uses `word_repr` for all types, but `compute_enum_layout` and `EnumLit` still use inline storage. These need to be updated to complete the pointer-based approach.

See `CURRENT.md` in the repo root for the latest state.

## For Parallel Sessions

If you run multiple agent sessions simultaneously:

1. **Every session reads `COORDINATION.md` first.**
2. **Every session adds itself to "Active Sessions"** with the files it's editing.
3. **No two sessions edit the same file.**
4. **Architectural decisions are recorded immediately** in `COORDINATION.md`.
5. **`src/lower.rs` and `compiler/lower.fd` must stay in sync** — if one changes, the other needs the same change.
6. **`src/codegen_c.rs` and `compiler/codegen_c.fd` must stay in sync.**
