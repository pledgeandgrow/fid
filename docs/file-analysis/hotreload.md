# hotreload.fd — watch-mode / hot-reload scaffold

- **Path:** `compiler/hotreload.fd` — module `compiler::hotreload`
- **LOC:** 636 (2026-11-09, unchanged). Rust `src/hotreload.rs`: 559.
- **Role:** per-function change detection over IR (`FunctionSnapshot`,
  `FunctionDiff`, FNV-style hashes), `compile_and_run` (lower, then run in
  the interpreter), `PersistentState` (key=value file between reloads),
  `Watcher` (poll file times), and a small `Debugger` call-stack struct.
- **Status:** DEAD. Nothing imports it; `fid watch` runs `bench`.
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::hotreload` (grep 2026-11-09). `fid watch`
dispatches to `cmd_bench` (main.fd:180), which builds through C and times
the run; nothing watches files. Not reachable from main.fd.

Imports: `core`, `vec`, `string`, `compiler::ir`, `compiler::compiler`
(`lower_to_ir_module`), `compiler::interp` (`interpret_ir_module`).

## Findings

### Verified live (probe)

1. **NEW, MEDIUM — `PersistentState_load` never finds `=`.** :377 calls the
   local `str_find_char(line, "=")`, which takes a byte (`ch: i64`) and so
   compares each byte with the literal's address. A probe of the same
   function returns -1 for `":"` and the right index for `58`. Every saved
   line is skipped on load.

### From code inspection

2. **STILL TRUE, HIGH — `watch` is a decoy.** `watch` → `cmd_bench`
   (main.fd:180). docs/reference/features.md:123 and architecture.md:642
   say so too.
3. **STILL TRUE, HIGH — `p_file_modified_ms` does not exist.** Called at
   :449 and :459; not defined in core, lower.fd's extern list
   (:2115-2124 has read/write/exists/append/delete/size/hash), the
   interpreter, or the C runtime (grep). `Watcher_*` cannot link.
4. **STILL TRUE, MEDIUM — `p_file_remove` does not exist** (:433); the
   builtin is `p_file_delete`.
5. **STILL TRUE, MEDIUM — `Watcher_run` spins.** 1000 polls with no sleep,
   `interval_ms` unused (:484-505). A sleep exists now for tasks
   (`task_sleep`, stdlib/task.fd), so "no sleep facility" (:483) is stale.
6. **STILL TRUE, MEDIUM — state file path is `/tmp/pledge_state_<hash>.json`**
   (:359): not valid on Windows; the content is key=value, not JSON.
7. **STILL TRUE, LOW — `JitResult` stdout/stderr are always ""** for a run
   (:338); the interpreter prints straight to the process streams.
8. **STILL TRUE, LOW — second `pub struct Debugger`** (:509) would clash
   with debugger.fd's if both were imported; local copies of
   `int_to_str`, `char_from_digit`, `str_lines`, `str_find_char`
   (:590-636).
9. **NEW, LOW — `p_file_read` used as `str`** (:370, :494), a raw `i64` now
   (AGENTS.md rule 25).
10. **INFO — type check.** `s18.exe check compiler/hotreload.fd` did not
    finish in 125 s (it pulls in the compiler). A scratch copy with the
    two compiler imports replaced by stubs: as `module compiler::hotreload`
    no errors; renamed out of `compiler::`, three errors (`p_file_read`
    i64 → str at :370 and :494, `str_find_char` str for i64 at :377). The
    missing `p_file_modified_ms` / `p_file_remove` are not reported by
    `check`; they would fail at C compile or link (not run).
11. **STILL TRUE, INFO — the good part.** `FunctionSnapshot_from_ir`
    (:157-217) and `FunctionSnapshot_diff` (:72) are sound and reusable.

## Fix order

1. Delete, or wire a real `watch` (mtime builtin, sleep, re-run on change)
   that reuses `FunctionSnapshot_*`.
2. If kept: `p_file_delete`, a byte for `str_find_char`, `read_file`, a
   temp path from the environment.

## Verification state

- 2026-11-09: header, all function headers, `compile_and_run`,
  `PersistentState`, `Watcher` read; importers and dispatch grepped;
  stubbed type checks run; `str_find_char` behaviour probed.
