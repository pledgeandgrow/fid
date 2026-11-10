# veil.fd

- **Path:** `compiler/veil.fd` — module `compiler::veil`
- **LOC:** 516 (2026-11-09). Rust `src/veil.rs`: 367.
- **Role:** the `veil` coverage tool: function / line / instruction
  coverage of one program run in the interpreter, as a text or HTML report.
- **Status:** reached from the CLI, but **broken**: it crashes before it
  writes a report, and it could not count coverage even if it did not.
- **Re-audited:** 2026-11-09.

## Where it is called

| Caller | Line | What |
|---|---|---|
| `compiler/main.fd` | 29 | `use compiler::veil` |
| `compiler/main.fd` | 183 | `fid veil` → `cmd_veil` (:3332) |
| `compiler/main.fd` | 3334, 3340 | `veil_print_help()`, `veil_run(input, format, output_dir)` |

Imports: `core`, `vec`, `string`, `compiler::ir`, `compiler::compiler`
(`lower_to_ir_module`), `compiler::interp`.

## What it does

- `collect_all_coverage` (:233) lists every function, every instruction and
  every `(function, source line)` of the lowered module. The module includes
  core, so the report lists core's functions too.
- `run_with_coverage` (:298) lowers the source, collects the structure,
  creates an `Interpreter`, calls `Interpreter_set_debug_hook`, runs `main`,
  takes the panic trace, computes per-function line counts.
- `veil_run` (:341) writes `<output>/coverage.txt` or `coverage.html`
  (default output dir `coverage`, main.fd:3339) and returns the text.
- `CoverageData_mark_*` (:179-197) would record what ran; nothing calls them.

## Findings

### Verified live (2026-11-09, `release/fid.exe` and `s18.exe`)

1. **NEW, HIGH — `fid veil` crashes with "Double free" after the program
   runs.** Both binaries, on a hello-world and on a struct test: the
   program's output appears, then
   `Panic: Double free` in `Vec_drop` ← `__own_drop_17` ←
   `run_with_coverage` ← `veil_run`. No report file is written; exit 1.
   Cause (reduced in a scratch probe): a local declared without a value
   (`var module: IrModule;`, :301) and assigned from a `match` payload
   binding (:303, `module = m;`) is freed twice when the function ends
   (likely once as `module` and once inside `module_result`; the exact pair
   was not traced). The 15-line repro
   (`var m: M; match r { ResM::Ok(mm) => { m = mm; } … }`) gives the same
   "Double free" under `fid run`; giving `m` an initial value makes it pass.
   This is an ownership bug in `lower.fd` (proposal 0004 assignment
   ownership), hit here first. Workaround in veil: initialize `module` or
   use `m` inside the match arm.
2. **STILL TRUE, HIGH — coverage is always 0%.** `Interpreter_set_debug_hook`
   (interp.fd:5593-5595) only sets `debug_mode`, which nothing reads; no
   per-instruction callback exists, so `covered_lines`,
   `covered_instructions`, `called_functions` stay empty and
   `compute_func_counts` (:199) reports `0/N` for every function. The
   comments at :292-297 and :319-321 say so (their line reference
   "interp.fd:2712" is stale; it is :5593 now).
3. **STILL TRUE, LOW — a missing input reads as "Cannot read"** (:342-345).
   Probe: a nonexistent path prints `Cannot read <path>` and exit code 0
   (`cmd_veil` always returns 0, main.fd:3343).

### From code inspection

4. **STILL TRUE, MEDIUM — output directory is not created, write not
   checked.** `p_file_write(output_dir + "/coverage.txt", …)` (:361, :366);
   no `mkdir`, result ignored, success message printed regardless. (Not
   reached today because of finding 1.)
5. **NEW, LOW — `p_file_read` used as `str`** (:342). AGENTS.md rule 25 says
   `p_file_read` is a raw `i64` (0 on failure) and code should use
   `read_file`. It type-checks because `compiler::*` modules are checked
   leniently (a scratch copy renamed out of `compiler::` fails with "cannot
   assign i64 to … str" for the same call in hotreload.fd).
6. **NEW, LOW — the report covers core.** `collect_all_coverage` walks
   `module.functions`, which holds every core function the program pulls
   in; the per-function table would be mostly core.
7. **Faithful to Rust (unchanged):** the structure walk, `CoverageData`
   shape, the three percentages, and both report generators.

## Fix order

1. Fix the double free (finding 1): in lower.fd's ownership of
   assignment from a pattern binding; or, locally, initialize `module` at
   :301.
2. Give the interpreter a real per-instruction hook and call
   `CoverageData_mark_*` from it (finding 2).
3. `mkdir -p` the output directory, check the write, return 1 on failure
   (findings 3, 4); read with `read_file`.
4. Filter core functions out of the report (finding 6).

## Verification state

- 2026-11-09: whole file read. `fid veil` run with `release/fid.exe` and
  `s18.exe` on two scratch programs (both crash as above); nonexistent input
  probed; the double free reduced to a 15-line program under `fid run`.
