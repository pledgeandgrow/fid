# native_debugger.fd — native (ptrace) debugger skeleton

- **Path:** `compiler/native_debugger.fd` — module `compiler::native_debugger`
- **LOC:** 544 (2026-11-09, unchanged). Rust `src/native_debugger.rs`: 311.
- **Role:** register structs (`Regs`, `UserRegs` = `user_regs_struct`),
  breakpoints, command parsing, `compile_to_native` (C via
  `ccompile_source_with_path`, then `gcc -g -O0`), and a command loop where
  every process-control command prints "ptrace not available".
- **Status:** DEAD. Nothing imports it; no `native-debug` / `autopsy`
  command in `fid`.
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::native_debugger` (grep 2026-11-09). main.fd
has only a comment header `cmd_native_debug` (:3348-3349) and no dispatch
entry. Not reachable from main.fd.

Imports: `core`, `vec`, `string`, `compiler::compiler`
(`ccompile_source_with_path`, `c_file_read`).

## Findings

### Verified live (probe)

1. **NEW, MEDIUM — command and breakpoint parsing never split.**
   `NativeDebugger_parse_bp_spec` (:176) and `parse_command` (:245) pass
   `":"` / `" "` to the local `str_find_char(s, ch: i64)` (:322), which then
   compares bytes with the literal's address: never found (probe gives -1;
   with `58` it gives the index). `break main:10` cannot be parsed.

### From code inspection

2. **STILL TRUE, HIGH — no process control.** No `ptrace` / `fork` /
   `waitpid` builtin exists; `run_ptrace` (:443) prints the limitation and
   loops over stdin. `NativeDebugger_get_regs` (:535) returns zeros.
3. **STILL TRUE, MEDIUM — `p_file_remove` does not exist** (:515, :517);
   the builtin is `p_file_delete`. The `.c` / `.bin` files would stay
   behind (and the program would not link).
4. **STILL TRUE, LOW — hard-coded `gcc`** (:384); no `CC`, no zig, and the
   check for Linux is never made before it shells out.
5. **NEW, LOW — `p_read_line()` used as `str`** (:457). It is a raw `i64`
   (0 on failure, AGENTS.md rule 25); `read_line` is the Option form.
   Type-checks only because `compiler::*` modules are checked leniently.
6. **NEW, LOW — stale text.** The header (:14-16) says the loop "is
   dispatched through compiler/main.fd"; it is not. :449 points users to
   `fidesc debug`, which `fid` does not have.
7. **INFO — type check.** `s18.exe check compiler/native_debugger.fd` did
   not finish in 120 s (it pulls in the compiler). A scratch copy with
   `use compiler::compiler` replaced by three stubs checks with no errors
   in 5 s. `p_file_remove` is not reported by `check`.
8. **STILL TRUE, INFO — salvageable parts:** `UserRegs` layout,
   `UserRegs_to_regs`, breakpoint bookkeeping.

## Fix order

1. Delete, unless ptrace/fork/waitpid builtins are planned.
2. If kept: bytes for `str_find_char`, `p_file_delete`, `read_line`,
   use the compiler's C toolchain choice instead of `gcc`.

## Verification state

- 2026-11-09: header, all function headers, `compile_to_native`,
  `run`, `run_ptrace` read; importers and dispatch grepped; stubbed type
  check run; `str_find_char` behaviour probed (same function as
  debugger.fd).
