# hotreload.fd — watch-mode + hot-reload scaffolding (635 LOC)

> **Re-verified 2026-10-23:** still dead — `watch` still aliases `cmd_bench` (main.fd:160), `p_file_modified_ms`/`p_file_remove` still implemented nowhere.

## Role

Four subsystems: `FunctionSnapshot`/`FunctionDiff` (IR-level
per-function change detection via FNV-1a content hashes),
`compile_and_run` (JIT: lower → interpret), `PersistentState`
(key=value file across reloads), `Watcher` (poll-for-changes loop),
plus a second `Debugger` struct. Port of `src/hotreload.rs` (559 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| — | — | **Nothing** — no importer anywhere. |

Rust wires `fidesc watch` → `FunctionSnapshot::from_l2` +
`Watcher::run` + `compile_and_run` (main.rs:2348-2509). Fides's
`watch` command **aliases `cmd_bench`** (main.fd dispatch) — running
`fides-c watch file.fd` benchmarks once and exits; it never watches.
Tenth dead module.

## Findings

### F1 — `watch` command is a decoy — HIGH (wiring)

`watch → cmd_bench` means the documented watch mode doesn't exist —
users get a benchmark run, not a file watcher. The real machinery
(FunctionSnapshot diff → recompile-on-change) is all here, unwired.

### F2 — `p_file_modified_ms` doesn't exist — HIGH

`Watcher_watch`/`Watcher_poll` (:448, :458) call
`p_file_modified_ms` — **not implemented in interp.fd or codegen_c.fd**
(the file primitives are `p_file_read/write/exists/append/delete/
size`). Any call to `Watcher_*` would fail to resolve — the watcher
can't have ever run in the self-hosted build.

### F3 — `Watcher_run` busy-polls, no sleep — MEDIUM (by design, still wrong)

`interval_ms` accepted but unused ("no sleep facility"), loop is
`max_iterations=1000` of tight `stat`-style polls → 100% CPU burn
then silent exit — not a daemon, a CPU spin followed by termination.

### F4 — `PersistentState` Unix-only + wrong remove builtin — MEDIUM

- State file path hardcoded `/tmp/pledge_state_<hash>.json` (:358) —
  no such directory on Windows; `p_file_write` would fail.
- `PersistentState_clear` calls `p_file_remove` (:432) — the actual
  builtin is `p_file_delete` — undefined-symbol failure.

### F5 — `JitResult` misleads + Debugger name collision — LOW

- `compile_and_run` returns `stdout`/`stderr` fields that are always
  `""` — the interpreter prints directly to the process streams;
  nothing captures them.
- Declares `pub struct Debugger` (:508) — a second, unrelated
  Debugger type; co-importing with `compiler::debugger` would collide
  or silently shadow depending on import rules.
- Local `str_lines`/`int_to_str`/`char_from_digit`/`hash_str`
  duplicated at :571-635 (stdlib shadows again).

### F6 — The good part — INFO

`FunctionSnapshot_from_ir` (:157-217) is the real workhorse and is
sound: hashes name + ret_ty + entry + params + per-block inst-ids +
inst-kind names + value metas — a function's hash changes on any
structural edit; `FunctionDiff` gives added/modified/removed sets.
This is the piece `fidesc watch` actually needed — it's reusable as-is.

## Verification state

- Dead status verified: zero importers; `watch` dispatch confirmed to
  alias `cmd_bench`.
- `p_file_modified_ms`/`p_file_remove` absence verified by grepping
  all p_file_* implementations in interp.fd + codegen_c.fd.
- Dead-in-Rust: false — Rust's `watch` is fully wired (this one is a
  pure port-orphan, not inherited).

## Next

`inc.fd`.
