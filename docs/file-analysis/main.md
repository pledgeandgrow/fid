# main.fd — CLI entry point / command dispatch

> **Re-audited 2026-11-09** against the working tree (3,349 lines). Earlier
> audit: 2026-10-22/23 (1,916 LOC then). Probes run with the latest
> self-hosted binary (s18) on scratch files.

- **Path:** `compiler/main.fd`
- **LOC:** 3,349
- **Role:** `pub fn main` of `fid`. Reads global flags, then a string
  dispatch to `cmd_*` functions. Most commands are thin shells over
  `compiler.fd` (pipeline), `codegen_c.fd` (via compiler.fd), `nat_ir.fd`,
  `pe_ir.fd`, `iropt.fd`/`rules.fd`/`egraph.fd`, `csplit.fd`, `wasm.fd`,
  `gpu.fd`, and the tool modules (scribe, veil, ark, lsp, fmt, cparser).
  It also drives the C compiler (`ccompile`), the build cache (`inc.fd`)
  and the test/bench/profile harnesses through `p_system`.

## Where it's called

Nothing imports it; it is the binary. It imports 40 modules (main.fd:11-50).

## Commands (main.fd:146-193)

| Command | Function | What it does |
|---|---|---|
| `compile` | `cmd_compile` :1755 | Alias of `ccompile` (Rust's `compile` emitted asm). |
| `ccompile` | `cmd_ccompile` :1784 | Fides → C (`ccompile_source_with_path`) → C compiler → executable. Flags: `-o`, `--target <arch-os>` (zig cc cross, else `<target>-gcc/clang`), `--cc-opt 0..3|s`, `--debug` (-O0), `--release` (-O2), `--inc` (per-module units, `build_c_units`), `--no-cache`, `--profile`, `--leaks`, `--checked-alloc`, `--trace` (prints one line only). Build stamp + artifact cache (`inc.fd`). Patches the PE stack to 256 MB on Windows. |
| `build`, `emit` | `cmd_build` :2789 | One function for both. Order: `--boot` → multiboot2 kernel (`pe.fd generate_boot_binary`, no std); `--elf` or `--target linux` → static x86-64 ELF from `pe_ir_generate_elf` (no std); `--bare` → flat binary (`pe_ir_generate_flat`, no std); `--target *linux*` (or `--native` with arm64/riscv64/x64) → machine layer (`iropt_native` → `rules_run` → `egraph_run` → `nat_generate_elf`); `--nat` → Windows PE on the machine layer (`nat_generate_pe`); Windows host default → `iropt_native` + `pe_ir_generate_pe`; `--native` otherwise → `build_executable` (PIR path); POSIX default → C via `CCodeGen` + `gcc/clang/cc -O2`. Flags also: `--no-opt`, `--debug` (no mid-end), `--lto`, `--caps`, `--time`, `--checked-alloc` (`g_pe_checked_alloc`). |
| `build-x86` | `cmd_build_x86` :2168 | Linux ELF from the x86 stack-machine backend (`build_x86_linux`). |
| `run` | `cmd_run` :687 | IR interpreter (`interpret_source_with_path`). `--leaks`/`--checked-alloc` instead re-invoke `fid ccompile --debug` with the debug allocator and run the result. Other flags: `-v`, `--max-recursion`, `--max-steps`, `--max-memory`, `--release`, `--trace`, `--timeout`. |
| `tokens`, `dump-tokens` | `cmd_tokens` :365 | Token list (line:col kind). |
| `ast`, `dump-ast`, `dump-ast-raw`, `dump-ast-flat` | `cmd_ast` :402 | All four print the same summary (struct/enum/function counts and function names) of the merged program. |
| `ir`, `dump-ir` | `cmd_ir` :449 | `IrModule_to_str` of `lower_to_ir_module(source)` (no import resolution). |
| `dump-c` | `cmd_dump_c` :478 | Generated C to stdout (`ccompile_source_with_path`, borrowck "on"). |
| `split-c` | `cmd_split_c` :509 | `fid split-c <in.c> <prefix> <n>` → `cs_split_files` (csplit.fd). Used by scripts/bootstrap.sh. |
| `pir`, `dump-pir` | `cmd_pir` :522 | PIR L1/L2/L3/all, `--opt` runs `optimize_l2`. |
| `opt` | `cmd_opt` :602 | PIR L2 after `optimize_l2`. The pass-list argument in its usage text is ignored. |
| `fmt` | `cmd_fmt` :638 | `format_source`, rewrites the file; `--check`. |
| `version` | `cmd_version` :358 | Prints "fid 0.1.0". |
| `test` | `cmd_test` :897 / `cmd_test_file` :802 | With a `.fd`: builds a harness (`fn test_*()` each in its own process, `fn bench_*()` timed to 200 ms) via `fid ccompile`. Without: scans tests/bootstrap, C-compiles and runs each, "PE" = front end only; `--fail` runs the expected-fail list (hard-coded :943-1021). |
| `bench` | `cmd_bench` :1233 | ccompile, then run the exe `-n` times (prints exit codes, no timing). |
| `watch` | `cmd_bench` :180 | Same as `bench`. |
| `profile` | `cmd_profile` :1319 | `fid ccompile --profile`, run, read `fides-profile.folded`, print the top 20 self-sample functions. |
| `lint` | `cmd_lint` :1541 | `lint_source`: unused functions/imports, unwrap() outside tests. |
| `check` | `cmd_check` :1512 | Front end only (`build_program_with_path_borrowck`, "on"), `--time`. |
| `test-rules` | inline :171 | `rules_selftest()`; `--smt <dir>` adds `rules_smt`. |
| `doc`, `scribe` | `cmd_scribe` :3316 | `scribe_process_file` (`--format`, `--output`). |
| `veil` | `cmd_veil` :3332 | Coverage (`veil_run`). |
| `wasm` | `cmd_wasm` :1584 | `compile_to_wasm` → `.wasm`, or `--wat` section dump. |
| `lsp` | `lsp_run` | LSP server. |
| `ark` | `ark_main` | Package manager (reads argv itself). |
| `repl` | `cmd_repl` :1390 | Each line wrapped in `fn main(){ println(x) }`, C-compiled with tcc/gcc, run. |
| `codegen` | `cmd_codegen` :2203 | PIR L3 → asm for x86-64 (`--peephole`, `--elf/--coff/--macho <o>`), arm64, riscv. |
| `gpu` | `cmd_gpu` :2303 | PIR L3 → SPIR-V (`--text`) or PTX. |
| `verify` | `cmd_verify` :2665 | `IrModule_verify` on the lowered module. |
| `audit` | `cmd_audit` :2570 | Text scan of the file and its `use` modules for `unsafe {`, `@unsafe`, `extern "C"` and their `// SAFETY:` comments; `--stdlib`, `--strict`. |
| `qualify` | `cmd_qualify` :2425 | JSON safety evidence: rule table (`qualify_rules` :2387), tests present, annotated functions; always "NOT_QUALIFIED". |
| `sourcemap` | `cmd_sourcemap` :2706 | `IrModule_source_map`. |
| `parse-header` | `cmd_parse_header` :2737 | C header → Fides bindings (`cparse_header_file`), `-o`, `--link`. |
| `notebook` | `cmd_notebook` :1447 | Writes an HTML page: `///` lines as markdown cells, each other line as a code cell. Nothing is evaluated (the usage text says it is). |

Not present (in Rust, not here): `debug`, `replay`, `autopsy`/`native-debug`
(only comment headers at :3346-3349), `diff-test`, `link`, `sbom`, `spdx`.

## Global flags and environment

- `--borrowck=off|warn|on` (:93) and `--timeout N` / `--timeout=N` (:112-124),
  anywhere in argv. Leading `--x` args before the command are skipped (:129).
- Interpreter mode: args after `--` (`fidesc run main.fd -- ccompile ...`).
- Environment read here: `CC`, `FIDES_CC_OPT`, `FIDES_CFLAGS`,
  `FIDES_NO_CACHE`, `FID_VERIFY` (IR verifier after each native mid-end
  stage), `OS` (host detection, `host_os` :317). Via compiler.fd:
  `FID_HOME`, `PATH`, `FIDES_COW_CHECK`, `FIDES_INC_FLAT`. In generated C:
  `FIDES_PROFILE_OUT`.
- Globals set here: `g_cfg_os`/`g_cfg_arch` (compiler.fd, @cfg target),
  `g_native_rt_win`, `g_nat_lines_names`/`g_nat_lines_file` (line table),
  `g_pe_checked_alloc`; setters `set_time_on`, `set_c_*`,
  `set_pe_runtime_merge`, `set_native_runtime`.

## Findings — verified live (s18 probes)

### MEDIUM — `build --native` with the default target crashes (CHANGED)

`fid build x.fd --native` → `build_executable` (PIR, compiler.fd:2124) →
`optimize_l2` panics "Vec index out of bounds" in `jump_thread_redirect`
(opt.fd). With `--no-opt` it ran past 120 s and was stopped. The default
target is `"x86-64"`, which `nat_target_arch` does not accept (it wants
`x86_64`/`x64`), so `--native` does not reach the working machine layer.
On a Windows host this path would also write a Linux ELF named `.exe`.

## Findings — code inspection

### MEDIUM — `--timeout` is parsed, never enforced (STILL TRUE)

main.fd:139-142 (TODO), :726: `run_timeout` is only printed with `-v`.

### MEDIUM — `fid build` on Linux/macOS takes a weaker C path (NEW)

main.fd:3225-3251: `gcc -O2 -o out out.c -lm 2>/dev/null`, then clang, cc.
Unlike `ccompile` it has no `-fno-strict-aliasing`/`-fno-strict-overflow`
(the generated C type-puns), no `-lpthread`, no `// PLINK:` libraries, no
`-lunwind`, ignores `CC`, and hides compiler errors.

### MEDIUM — `fid test` without a file is broken on POSIX (NEW)

`ls tests/bootstrap/*.fd` (:909) returns paths with the directory, then
:1090 builds `"tests\\bootstrap\\" + fname` (backslashes and the directory
twice). Stems then never match the expected-fail list. The exe is run as
`_fides_test_tmp` without `./` (:1136). The "PE" column is only
`build_program_with_path` succeeding (:1145-1157); no interpreter backend.

### LOW — `run --release` and `run --trace` do nothing (STILL TRUE)

:717-718, printed only under `-v`; not passed to the interpreter.
`ccompile --trace` likewise prints one line (:1835).

### LOW — `ir`, `pir`, `opt` ignore imports (NEW)

They call `lower_to_ir_module(source)` (:462, :546, :617) with no path, so
`use` modules are not resolved (compiler.fd:1285 needs a source path).

### LOW — `run --leaks` / `--checked-alloc` leave files behind (NEW)

Only the exe is deleted (:711). `_fides_memcheck_tmp.c`, `.fides-stamp`
and `.pdb` stay in the working directory (seen in a probe).

### LOW — decoys and stubs (STILL TRUE)

`watch` = `bench` (:180); `dump-ast-flat` = `cmd_ast` summary (:157);
`notebook` evaluates nothing; REPL compiles each line with hard-coded
`tcc`/`gcc` (:1422-1426, ignores `CC`) and advertises `:ir`, which does
not exist; `bench` prints exit codes, not times.

### LOW — positional convention (STILL TRUE)

The file is `argv[start+1]`; flag helpers scan from `start+2` (:211-261).
`fid run --verbose x.fd` treats `--verbose` as the file.
`c_file_read` cannot tell a missing file from an empty one.

### LOW — bare-metal flags (STILL TRUE)

No `--opt-level`, `--load-addr`, `--linker-script`. The `--boot`, `--elf`,
`--bare` paths call `build_program_no_std`, which forces borrowck "off"
(compiler.fd:677) whatever `--borrowck` says.

### FIXED since 2026-10-23

- `codegen`, `parse-header`, `gpu`, `sourcemap`, `verify` restored;
  `audit` and `qualify` added (Nov 4); `check`, `doc`, `test-rules`,
  `split-c`, `profile` (sampling), `test <file>` added.
- Cross targets (`ccompile --target`), WASI via zig cc, `--inc` unit
  builds, build stamps and artifact cache.

## Fix order

1. Map `--native` to the machine layer (accept `x86-64`) or remove the PIR
   fallback; fix `fid build` on POSIX to share `ccompile`'s C command.
2. Fix `fid test` directory mode on POSIX (strip the directory, `./`).
3. Enforce or drop `--timeout`, `run --release/--trace`.
4. Clean up the memcheck temp files; pass a path from `ir`/`pir`/`opt`.

## Verification state

All dispatch lines and `cmd_*` bodies read. Probes: `build --native`
(panic, hang), `run --leaks` (works, leaves files), `run --borrowck=warn`,
`dump-c`, `wasm`, `gpu`, `split-c`. Not run: `test` directory mode,
`repl`, `profile`, `lsp`, `ark`.
