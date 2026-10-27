# main.fd — CLI Entry Point / Command Dispatch

**1,916 LOC** vs `src/main.rs` 3,372 LOC (~57%). `fn main` — nobody imports it; it IS the
binary. Arg-parse → global flags → ~30-command string dispatch → `cmd_*` functions that
mostly delegate into compiler.fd/lsp/scribe/veil/ark/fmt.

## Where it's called

- Itself the entry point. Imported by nothing.

## Command coverage — what's missing vs Rust (updated 2026-10-23)

| Rust command | Fides status |
|---|---|
| `codegen` | **restored** — `cmd_codegen` (:1657) w/ `--emit-coff`/`--macho`/`--emit-asm`/`--emit-obj` |
| `debug` | **dropped** — `cmd_debug` exists only as a comment header (:2258); debugger.fd still dead |
| `replay`, `native-debug`/`autopsy` | **dropped** — comment headers only (:2260); native_debugger.fd remains dead #5 |
| `parse-header` | **restored** — `cmd_parse_header` (:1932) |
| `gpu`, `spirv`, `ptx` | **restored** — `cmd_gpu` (:1785), spirv→.spv / ptx→.ptx |
| `build-x86` | **added** — `cmd_build_x86` (not in Rust — the stack-machine backend is now reachable) |
| `sourcemap`, `verify`/`audit` | **restored** — `cmd_sourcemap` (:1901), `cmd_verify` (:~1860, IR verification via `verify_module`) |
| `watch` | **aliased to `cmd_bench`** — decoy (dead #10), unchanged |
| `dump-ast-flat` | **aliased to `cmd_ast`** — summary, not the flat tree (flat_ast.md), unchanged |
| `diff-test` | **dropped** — golden-stage differential testing gone (`--stage`/`--golden`/`--update` infra) |
| `link` | **dropped** — linker.fd is only reachable inside `build` |
| `sbom`, `qualify`, `spdx` | dropped — supply-chain/qualification tooling absent |
| `repl`, `notebook`, `scribe`, `veil`, `ark`, `wasm`, `lint`, `lsp`, `fmt`, `test`, `bench`, `profile`, `opt`, `pir`, `ir`, `ast`, `tokens`, `dump-*`, `compile`/`ccompile`/`build`/`emit`/`run`/`version` | present (Fides also folds `scribe`/`veil`/`ark` in as commands — Rust keeps them as separate binaries) |

**Dropped targets are now ~8** (was ~12): `debug`, `diff-test`, `link`, `replay`,
`native-debug`/`autopsy`, `sbom`/`qualify`/`spdx`. `diff-test` remains the worst
operational loss — no self-hosted golden-file stage comparison.

## Flag-level findings

### MEDIUM — `--timeout` is parsed, printed, never enforced

Global flag → `g_timeout` → `run_timeout` at :598 — printed in verbose, then **dropped**;
`interpret_source_with_path` (:643) takes no timeout param. The :81 TODO admits it needs
a watcher thread. A documented flag that does nothing.

### MEDIUM — `run --release` and `run --trace` are decorative

Parsed (:589-590), echoed in verbose (:604-607), **not passed** to the interpreter —
`--trace` produces no per-stage timing (Rust's `run --trace` does). Dead stores.

### LOW — bare-metal flag regression

`--opt-level`, `--load-addr`, `--linker-script` (Rust :270-290) dropped; only
`--boot`/`--bare` survived → embedded/boot targets lose load-address/linker-script
control. Partly mitigated by linker-script parse being simple, but still a flag drop.

### LOW — REPL is compile-and-spawn, not interpret

`cmd_repl` (:1121) wraps input in `fn main(){println(x)}` → `ccompile_source_with_path` →
`p_system("tcc|gcc …")` → `p_system(exe)` per line. Rust's REPL uses `interpret_source`
(instant, no temp files). Works on the compiled binary only — needs `p_system`/`p_read_line`
(codegen_c-only builtins). `:ir` is advertised in help but unimplemented (in both — Rust's
help also lies).

### Inherited

- Positional args assumed `fides-c <cmd> <file> [flags]` — flag helpers scan `start+2`;
  `fides-c run --verbose file.fd` treats `--verbose` as the filename → "could not read
  file". Same convention as Rust (`args[2]` is the file).
- `cmd_test` shells `ls`/`dir` to enumerate tests via `p_system` (codegen_c-only builtin)
  and runs **2 backends** (tcc C + PE) — the comment admits "TODO: interpreter backend"
  — Rust's `test` runs interp+c+PE (3). Baseline metric parity is one backend short.
- `read_source_or_error`/`c_file_read` can't distinguish missing file from empty file
  (both → `""` → error path) — harmless edge.

## Assessment

Pure dispatch — the real logic lives in already-analyzed modules. The port's cost is
~12 dropped command surfaces (stranding every dead module we've catalogued) plus a few
dead flags. Nothing here miscompiles; the gaps are operational — you lose `diff-test`,
the debug/replay/native-debug family, `parse-header`, `codegen` asm output, linker control,
sbom/qualify/spdx, and a real `watch`. Most `cmd_*` bodies are thin shells over
`compiler.fd` and inherit whatever those modules do.
