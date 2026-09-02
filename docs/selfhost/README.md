# Fides Self-Hosted Compiler — Documentation

> **Read this before working on the self-hosted compiler.**
> This folder contains everything an agent (human or AI) needs to understand
> the architecture, constraints, build pipeline, and known gotchas.

## Files in this folder

| File | Purpose |
|------|---------|
| `architecture.md` | File-by-file breakdown of every `compiler/*.fd` module, what it does, and how modules connect |
| `language-constraints.md` | What the Fides language must respect: types, effects, regions, patterns, closures, FFI, defer, try |
| `calling-conventions.md` | SRET, enum layout, struct copy, Box semantics, Vec handling, the `Opt` pattern — the low-level machinery that must stay in sync |
| `build-pipeline.md` | How to build, test, bootstrap, and verify the self-hosted compiler |
| `known-bugs.md` | Known bugs, gotchas, and things that have been fixed (do not re-introduce) |
| `agent-guide.md` | Best practices and rules for agents working on the compiler |

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

## Current Status (Feb 2026)

- **Crash fixed** — the `STATUS_ACCESS_VIOLATION` crash is resolved
- **SRET fixed** — struct returns via out-parameter convention now work
- **Enum layout fixed** — payload sizes computed from actual field sizes
- **Remaining blocker** — enum variant extraction truncates struct payloads to 8 bytes; `load_enum_field()` must be ported from `src/lower.rs` to `compiler/lower.fd`
- **23/23 self-host tests pass** on interpreter + PE emitter
- **Bootstrap fixpoint not yet achieved** — blocked by the enum variant extraction bug

See `CURRENT.md` in the repo root for the latest state.
