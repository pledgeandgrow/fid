# veil.fd

- **Path:** `compiler/veil.fd`
- **LOC:** 527 (Rust `src/veil.rs`: 367)
- **Role:** `veil` code-coverage tool — function/line/instruction coverage via an interpreter debug hook, text + HTML reports.

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/main.fd` | 26 | `use compiler::veil` |
| `compiler/main.fd` | 154 | `veil` command → `cmd_veil` (:1899) |
| `compiler/main.fd` | 1907 | `veil_run(input, format, output_dir)` |

Live command — but see below.

## Findings

### HIGH — veil still reports 0% on everything (partial fix 2026-10-23)

Two stacked drops — **one now fixed:**

1. ~~**`veil_run` never executes the program.**~~ **FIXED** — `veil_run` now calls `run_with_coverage` (:347), which builds IR, runs `interpret_ir_module`, populates the `VeilCoverage` tables, and prints program output (:298-338). Program execution is back.
2. **Still broken: the hook is a no-op.** `Interpreter_set_debug_hook` (`interp.fd:4212-4214`) only sets `interp.debug_mode = true` — it **does not store or register a callback**, and `debug_mode` itself is a dead store (nothing reads it; run-15 confirmed). So `run_with_coverage` runs the program but every covered-lines/instructions table stays empty anyway. Rust installs a real `FnMut` hook (`src/veil.rs:117`, invoked per-instruction at `src/interp.rs:1167`).

**Net result is unchanged:** `compute_func_counts` still walks an always-empty `covered_lines` → `covered=0` for every function → the report emits `0/N (0.0%)` + `[X]` on every function. The program now *runs* (output is shown) but coverage data is still never collected. Fides veil remains a **structural enumerator wearing a coverage report's clothes**.

### MEDIUM — same output-dir/write problems as scribe

`p_file_write(output_dir + "/coverage.txt")` — no `create_dir_all` (Rust :316 has it), unchecked result. Missing `coverage/` dir → silent no-write + "Coverage report: …" success message.

### LOW — read-failure conflation

`str_len(source) == 0` → "Cannot read" (:343) — the usual `p_file_read` ambiguity.

### Verified faithful

- `collect_all_coverage` — function/block/instruction enumeration with `source_line` meta extraction — matches Rust :62.
- `compute_func_counts`, `CoverageData` shape, the three `*_pct` computations — 1:1.
- Both report generators (`generate_text_report`, `generate_html_report`) ported with the ` OK`/` X` marker scheme.
- `run_with_coverage`'s structure (build → collect → run → counts → status-string) matches Rust's :88 shape — now actually called; still hookless.

## Suggested fix order

1. Wire `Interpreter_set_debug_hook` to actually store+invoke a hook (interp needs a debug-hook field + call site per instruction). **This is now the sole blocker** — everything else in the chain works.
2. ~~Route `veil_run` through `run_with_coverage`~~ — **DONE (2026-10-23)**.
3. `mkdir -p` + check `p_file_write` result (same fix as scribe).

## Verification

Re-verified 2026-10-23 against current source: `run_with_coverage` is now called
(veil.fd:347) but `Interpreter_set_debug_hook` remains a flag-only no-op
(interp.fd:4212-4214 — sets `debug_mode`, stores no hook, and `debug_mode` is
itself never read). The 0%-always conclusion still holds mechanically:
`covered_lines` is never populated because no per-instruction hook fires.
