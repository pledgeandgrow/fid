# compiler.fd — pipeline orchestrator

> **Re-audited 2026-11-09** against the working tree (4,502 lines; 2,841 at
> the 2026-10-22/23 audit). Probes with the latest self-hosted binary (s18).

- **Path:** `compiler/compiler.fd`
- **LOC:** 4,502
- **Role:** the front-end driver and the backend entry points. Owns stdlib
  and runtime loading, import resolution, program merging, the AST passes
  (@cfg, @derive, `==` synthesis, desugaring, default trait methods,
  turbofish), the checker sequence, error enrichment, the in-process IR
  cache hook, `--time` stage timing, and `split_c_units` for `ccompile --inc`.

## Where it's called

| Caller | What |
|---|---|
| `main.fd` | every command (see main.md) |
| `lsp.fd` | `get_diagnostics` :2419 |
| `veil.fd`, `hotreload.fd` | `lower_to_ir_module` :2009 |
| `native_debugger.fd` | imports (module itself is dead) |

Imports 39 modules (:23-61), among them `codegen` (dead, see codegen.md),
`codegen_c`, `wasm`, `iropt`, `cowcheck`, `x86`, the PIR-era backends.

## Pipelines

### Front end: `build_program_with_path_borrowck` :1112 (every hosted command)

1. Record core.fd as a build input; `get_cached_stdlib` parses
   `stdlib_dir()/core.fd` once per process (:1085). `stdlib_dir` (:985):
   ./stdlib, `$FID_HOME/stdlib`, `<exe dir>/stdlib`, `<exe dir>/../stdlib`.
2. Native runtime merge when `g_pe_runtime_merge` (:1150): pe_runtime
   (base, conc, tensor, data) or linux_runtime (base, conc, sys, math,
   pe_runtime/data, pe_runtime/tensor, + win.fd for `--nat`); core wins.
3. Lex + parse the user file with stdlib type names.
4. Resolve `use` imports (:1285-1515): candidates base dir, stdlib,
   project root (`find_project_root`), `name.fd` or `name/mod.fd`;
   `tls` → tls_unix/tls_windows (`c_platform_module`). A module whose deps
   are not loaded is deferred; each file is read/lexed/parsed once
   (`cached_*`). Every found and every missing candidate is recorded for
   the build stamp (`inc_record_file`/`inc_record_absent`). User code is
   re-parsed with the full name set.
5. `cc_qualify_shadowed_imports` (user fn with the same name as an
   imported one: both kept as `module__name`), supertrait inheritance
   (`cc_inherit_supertraits`), duplicate functions per module and
   cross-module conflicts (`cc_find_duplicate_funcs`,
   `cc_find_module_conflicts`).
6. Merge stdlib → imports → user (`merge_programs` :2544, later wins;
   `record_func_modules` notes each function's module for `--inc`).
7. `apply_cfg_filter` → `process_derive` → `synthesize_enum_eq` →
   `synthesize_struct_eq` → `desugar_layers_models` → `desugar_phase8b` →
   `process_gen` (comptime gen) → `fill_default_trait_methods` →
   `mono_resolve_turbofish` (proposal 0002).
8. Type check: register core, imports, user; trait impls, generated decls;
   core bodies skipped when this core was checked clean before
   (`inc_core_checked`, release D9); `TypeChecker_check_bodies`.
9. `escape_check`, `secrecy_check`, lints (`g_lint_warnings`), borrow
   check (mode on/warn/off), optional `FIDES_COW_CHECK` report
   (`cow_check_program`), mark core checked, `substitute` (comptime),
   `monomorphize`.

### `run` → `interpret_source_with_path` :2081

Front end → `lower` → `interpret_ir_module_with_options` (interp.fd).

### `ccompile` / `dump-c` → `ccompile_source_with_path` :1804

1. `global_cache_get(path, hash)` (inc.fd: in-process IR cache, checks the
   recorded deps); hit → C generation only.
2. `lower_to_ir_module_with_path` (front end + `lower`); no IR optimizer
   (matches Rust's ccompile).
3. `global_cache_store`, `CCodeGen_new` + `CCodeGen_generate`.
   main.fd then writes the C and runs the C compiler.

### `emit` / `build` (front end via main.fd)

main.fd calls `build_program_with_path_borrowck` or `build_program_no_std`
itself and then `lower` + the backend (nat_ir, pe_ir, C). compiler.fd
provides `build_executable` :2124 for `--native` (PIR: IR → L1 → L2 →
optimize → L3 → codegen_x64 + peephole + elf64 + runtime_linux + linker;
arm64/riscv analogues; `windows` → `iropt_native` + `pe_ir_generate_pe`;
`strip_ungranted_caps` with `--caps`), `build_x86_linux` :2381
(`build-x86`), `compile_to_wasm` :1858 (`wasm`).

### No-std: `build_program_no_std_borrowck` :682

Parse, resolve imports (no stdlib), desugar, `process_gen`, default trait
methods, merge, type check, escape, secrecy, borrow check, monomorphize.

## Findings — verified live (s18 probes)

### HIGH — `--borrowck=warn` prints nothing (STILL TRUE)

compiler.fd:1734-1748 (and :908-921 in no-std) read `bc.errors`;
borrowck.fd:1357 puts warn-mode violations in `bc.warnings`. Probe:
`run tests/bootstrap/test_borrowck_fail.fd --borrowck=warn` runs with
exit 0 and no warning; the same file without the flag is rejected.

### MEDIUM — unconditional `[build]`/`[prof]`/`[ccompile]` stderr lines (STILL TRUE)

E.g. :1131, :1204-1234, :1291-1311 (one line per import), :1617-1794,
:1809-1849. Seen on every probe. `--time` (`time_mark`/`time_report`,
:1053-1081) is the gated replacement, but the old lines stay.

### LOW — `@cfg(debug)` never holds (NEW)

`check_cfg` is always called with `is_debug = false` (:2737-2756):
`@cfg(debug)` items are always dropped and `@cfg(release)` kept, also for
`ccompile --debug`. Probe: a debug/release pair printed "release".
@cfg filters only functions and structs (not enums, impls, consts); the
parser rejects annotations on impl methods and enums anyway.

## Findings — code inspection

### MEDIUM — re-parse failure swallowed (STILL TRUE)

:1506-1514: `ParseResultProgram::Err(_) => { }` keeps the first parse.

### MEDIUM — `lint_source` skips import resolution (STILL TRUE)

:1913 "simplified — no import resolution": stdlib + user only, so a file
with `use` gets false errors.

### MEDIUM — no-std path skips passes (CHANGED)

`build_program_no_std` (main.fd's `--boot`, `--elf`, `--bare`) passes
borrowck "off" (:677). The no-std body now has escape and secrecy checks
and honours the mode it is given, but still skips `apply_cfg_filter`,
`process_derive`, `synthesize_*_eq`, `mono_resolve_turbofish`,
`substitute` and core-free name checks (:828-932).

### LOW — no-std merge precedence reversed (NEW)

:837 `merge_programs(program, imported)`: the second argument wins, so an
imported module's definition replaces the user's on a name clash (the
comment says "user wins").

### LOW — plain `impl` blocks merge by struct name only (STILL TRUE)

merge_programs :2629-2642: a user `impl S` hides every earlier `impl S`
block (other methods included). Trait impls merge on (trait, type).

### LOW — import edge cases (STILL TRUE)

Cyclic import: a warning and `continue` (:1388-1393), so the failure shows
later as unknown names. `cached_parse` keys on path only (:226-246).

### LOW — dead plumbing (CHANGED)

- `g_lint_warnings` (:1038) is written (:1713, :1888) and never read.
- `set_borrowck_mode` / `g_borrowck_mode` (:1042-1048): no callers.
- `interpret_source`, `interpret_source_with_options`: no callers.
- `strip_ungranted_caps` is now called (from `build_executable` :2139,
  `--native --caps`), a path that crashes (main.md).
- File header (:1-10) still says every pass is "fully functional".

### INFO — derives (CHANGED)

`Serialize` now builds real JSON (`cc_serialize_field`), `Hash` added
(:3084), `Eq` aliases `PartialEq`. `Debug` still returns literal
`"S(f: ?)"` (:2978-2994), `Clone` returns `self`, `Deserialize` ignores
its input and returns defaults (:3038-3051).

### FIXED since 2026-10-23

- `fill_default_trait_methods` stores the impl back (:3593).
- `apply_cfg_filter` works (:2724), target-aware via `g_cfg_os`/
  `g_cfg_arch` (set by `ccompile --target` and native builds).
- The IR cache checks the recorded dependency files (inc.fd:352-363),
  so a changed import is a miss.
- No-std path runs escape/secrecy and the borrow checker when asked.
- Windows target of `build_executable` uses pe_ir, not `generate_pe`.

## Fix order

1. Warn mode: print `bc.warnings`.
2. Gate the `[build]`/`[prof]` lines (or drop them for `--time`).
3. Report re-parse errors; resolve imports in `lint_source`.
4. Pass the real borrowck mode to no-std; run the missing passes; fix the
   merge order; give `check_cfg` the build mode.

## Verification state

All function headers read; the pipeline functions, merge, cfg, derive,
default methods and split_c_units read in full. Probes: borrowck warn,
@cfg debug/release, test_cfg, test_trait_default (pass). Not re-checked:
`desugar_phase8b`, `desugar_layers_models`, `cc_find_module_conflicts`
details.
