# debugger.fd — interactive IR debugger (977 LOC)

> **Re-verified 2026-10-23:** still dead — `debug` is still not dispatched (cmd_debug is a bare comment header at main.fd:2258), zero importers, `interp.debug_mode` still a dead store.

## Role

Interactive debugger state: breakpoints, watches, `parse_command`, source
listing, IR/effect printers, trace save/load/replay. Port of
`src/debugger.rs` (965 LOC) — which is a real stdin-driven REPL in Rust.

## Where it's called

| Caller | Site | What |
|--------|------|------|
| — | — | **Nothing.** No `use compiler::debugger` anywhere; main.fd's dispatch (31 commands) has no `debug`, no `replay`, no `autopsy`. `cmd_debug` is an empty comment block (:1913-1916). |

Fifth dead module (autograd, codegen, coff, cparser, this).

## Findings

### F1 — Whole module unreachable + gutted — HIGH

Even if a `debug` command were wired, `Debugger_run` (:773-830) is
**not a debugger**: it builds the program, calls
`Interpreter_set_debug_hook` (`debug_mode = true` — a no-op), runs
`Interpreter_run`, prints the result. **No REPL, no stdin loop, no
breakpoint checking** — it's `fidesc run` with a banner. The comment
admits: *"simplified: no callback hook yet"*.

Rust's `run()` (src:251+) reads stdin in a loop, calls `should_break` per
step, and pauses for interactive commands — none of that was ported; it
can't be, since there's no stdin-read path in the self-hosted library.

### F2 — `debug_mode` flag is a dead store — HIGH (supporting evidence)

`interp.fd`: field at :413, set true by default at :448 AND by
`set_debug_hook` at :3841 — **never read anywhere** — the flag changes
nothing. The interpreter has no per-instruction break check.

### F3 — Dead debugging vocabulary — INFO

All of it compiles but is uncallable: `Breakpoint`/`should_break`/
`add_watch`/`update_watch`/`list_source`/`parse_command`/`format_value`/
`print_debugger_help`/`print_locals`/`print_var`/`show_ir`/`show_effects`/
`list_functions`/`DebuggerState`/`TraceEntry`/`save_trace`/`load_trace`/
`replay_trace` (~900 LOC). `replay_trace` degraded to non-interactive
trace-dump ("no stdin in library code" — :905-909) — but `fidesc replay`
doesn't exist in main.fd anyway.

### F4 — Trace format asymmetry — LOW

`save_trace` writes `func_name block_id inst_id source_line [value|-]`;
`load_trace` requires `parts.len() >= 5` — a line without a value column
(4 parts) is dropped silently even though "-" is the writer's placeholder
for None (consistent, but brittle to hand-edited traces). `str_to_int`
has no error path — malformed numbers → 0.

## Verification state

- Dead-module status verified: zero importers, zero dispatch entries.
- Rust is a real REPL — the Fides port is the *skeleton without the
  interactivity*; can't be exercised at all.
- Recommendation: this needs stdin (`p_read_line`-style runtime support)
  before it's worth keeping — until then it's 977 LOC of scaffolding.
  Alternative: delete and re-port when stdin lands.

## Next

`diag.fd`.
