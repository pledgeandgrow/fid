# native_debugger.fd — Native (ptrace) Debugger Skeleton

> **Re-verified 2026-10-23:** still dead — `native-debug`/`autopsy` still absent from dispatch (comment headers only at main.fd:2260), zero importers.

**544 LOC** vs `src/native_debugger.rs` 311 LOC (~175% — comment-heavy honest skeleton +
local str helpers). **Twelfth dead module.**

## Where it's called

- **Nowhere** — zero importers; `native-debug`/`autopsy` were dropped from main.fd's
  dispatch (run 33). Rust wires `fidesc native-debug <file>` → real ptrace loop.

## What it is

An honest skeleton — the file *says* so at :419-449. Structure:

- `NativeDebugger_run` (:401) → `compile_to_native` (:345) — **actually runs**: reads
  source, `ccompile_source_with_path` → C → `gcc -g -O0 -o <src>.bin` via `p_system`.
  So the dead command still does real work (writes `<src>.c` + `<src>.bin` to disk)
  before discovering it can't trace.
- `run_ptrace` (:443) — interactive `(fidesc:native)>` REPL where `run`/`continue`/`step`/
  `regs` all print "ptrace not available"; only `break`/`info break`/`quit` function.
  No ptrace/fork/waitpid builtins exist in the Fides runtime — this is as far as it can go.
- `NativeDebugger_get_regs` (:535) returns `Regs_new()` — all-zeros stub with a
  documented 4-step recipe for when ptrace lands.

## Findings

- **Port typo — `p_file_remove` (:515, :517) doesn't exist.** Real builtin is
  `p_file_delete` (codegen_c:899/1619/3614, interp:3406). Cleanup of `.c`/`.bin` temp
  files silently fails — debris left in the source dir (same family as hotreload.fd).
- `compile_to_native` hardcodes `gcc` (:384) — no tcc fallback, no Windows path
  (acceptable: ptrace is Linux-only anyway; but inconsistent with cmd_repl's tcc choice).
- `is_linux()` isn't even consulted — on non-Linux the `gcc` shell-out still runs and
  only *then* fails.
- **Salvageable**: `UserRegs` (full `user_regs_struct` layout, :58-92), `UserRegs_to_regs`
  field mapping, `parse_bp_spec` (`func[:line]` parsing), `parse_command` — all real code.
  ~300 LOC of correct infrastructure around a syscall layer that doesn't exist.

## Assessment

Not a port defect — a platform-layer absence the file documents honestly. The only real
bug is the `p_file_remove` typo (would fail silently if the dead command ran). If a
`native-debug` command is ever wired, it needs `ptrace`/`fork`/`waitpid`/`kill` builtins
first; until then the file is dead weight alongside debugger.fd.
