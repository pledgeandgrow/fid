# debugger.fd — interactive IR debugger (scaffold)

- **Path:** `compiler/debugger.fd` — module `compiler::debugger`
- **LOC:** 977 (2026-11-09, unchanged). Rust `src/debugger.rs`: 965.
- **Role:** debugger state and helpers: breakpoints, watches, command
  parsing, source listing, IR/effect/locals printers, trace save/load/replay,
  and `Debugger_run`, which builds and runs the program in the interpreter.
- **Status:** DEAD. Nothing imports it; `fid` has no `debug` or `replay`
  command.
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::debugger` anywhere (grep 2026-11-09). The
main.fd dispatch (:146-192) has no `debug`, `replay` or `autopsy`; only a
comment header `cmd_debug` remains (main.fd:3346-3347). `fid debug x.fd`
prints "Unknown command". Not reachable from main.fd.

Imports: `core`, `vec`, `string`, `compiler::interp`, `compiler::ir`,
`compiler` (no module of that name exists; the compiler module is
`compiler::compiler`), `compiler::lower`.

## What it has

- `Breakpoint`, `Debugger` (:50-93), `Debugger_should_break` (:111),
  add/delete/list breakpoints and watches, `Debugger_list_source` (:227).
- `format_value` (:254, knows all eight `Value` variants), help text,
  `parse_command` (:297).
- `DebuggerState`, `TraceEntry`, `print_locals`, `print_var`, `show_ir`,
  `show_ir_with_current`, `show_effects`, `list_functions` (:402-760).
- `Debugger_run` (:773): `build_program_with_path` → `lower` →
  `Interpreter_new` → `Interpreter_set_debug_hook` → `Interpreter_run`,
  then prints the result. No command loop, no stepping.
- `save_trace` / `load_trace` / `replay_trace` (:839-950): text trace,
  printed non-interactively.

## Findings

### Verified live (probe)

1. **NEW, MEDIUM — command and breakpoint parsing never split.**
   `parse_command` (:299) and `Debugger_parse_bp_spec` (:96) call the local
   `str_find_char(s, ch: i64)` with a string literal (`" "`, `":"`). The
   byte is compared with the literal's address, so nothing is ever found.
   Probe (a `compiler::` module, so it compiles the same way):
   `str_find_char("main:10", ":")` → `-1`; with `58` → `4`. So
   `break main:10` would be one command word `break main:10`, and
   `main:10` would be a function name with line -1. Same bug in
   native_debugger.fd (:176, :245) and hotreload.fd (:377).

### From code inspection

2. **STILL TRUE, HIGH — dead and not a debugger.** `Debugger_run` is
   `fid run` with a banner. The interpreter has no per-instruction hook
   (`debug_mode` is set and never read: interp.fd:896, :5594), so
   `Debugger_should_break` has nowhere to be called from.
3. **CHANGED — "no stdin in library code".** The comments at :30-36,
   :905-909 say stepping needs stdin, which library code lacks. Stdin now
   exists (`read_line` in core; native_debugger.fd's loop already calls
   `p_read_line`). The real missing piece is the interpreter hook.
4. **NEW, LOW — the header says the run loop "is wired through
   compiler/main.fd's `debug` command"** (:34-36). There is no such
   command.
5. **STILL TRUE, LOW — trace format.** `load_trace` (:866) keeps only the
   fifth word as the value, so a value with spaces is cut; lines with
   fewer than five words are dropped; `str_to_int` has no error path
   (bad numbers become 0).
6. **NEW, LOW — `p_file_read` used as `str`** (:867). It is a raw `i64`
   now (AGENTS.md rule 25); it type-checks only because `compiler::*`
   modules are checked leniently.
7. **INFO — type check.** `s18.exe check compiler/debugger.fd` did not
   finish in 125 s (it pulls in compiler::compiler and lower). A scratch
   copy with `use compiler;` / `use compiler::lower;` replaced by three
   stubs (`ResultProgram`, `build_program_with_path`, `lower`) checks with
   no errors in 19 s. Whether the real `use compiler;` resolves was not
   checked.

## Fix order

1. Decide: delete (the Rust tool is a REPL; this is a skeleton), or add a
   per-instruction hook to interp.fd first.
2. If kept: pass byte values to `str_find_char` (`58`, `32`), add the
   `debug` command, read input with `read_line`, use `read_file`.

## Verification state

- 2026-11-09: header, all function headers, `Debugger_run`, trace code
  read; importers and dispatch grepped; `str_find_char` behaviour probed;
  stubbed type check run.
