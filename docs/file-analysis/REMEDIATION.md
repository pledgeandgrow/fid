# Section A remediation — pure rewiring pass

Scope: the "A" list from the dead-module triage — items fixable by rewiring
existing code, with no new OS/runtime primitives. Items needing `ptrace`,
`readdir`, `mkdir`, file-error returns, the interpreter debug hook, or
`p_file_read_bytes` are deferred to Section B.

## Status summary

| # | Item | Status | Verification |
|---|------|--------|--------------|
| A1 | `elf64::encode_asm_block(runtime_asm)` + merge in `runtime_linux.fd` | Done | Merges + typechecks in full-program compile |
| A2 | `granted_caps` threaded through `build_executable` → `strip_ungranted_caps` + `runtime_parts_with_caps{,_arm64,_riscv}` | Done | Typecheck; per-bit `cap_granted*` (`FS/IO`=1, `Net`=2, `Spawn`=4) |
| A3 | `interp.fd` `p_tensor_*`/`p_quantize_*`/`p_kv_*` → `tensor.fd` `Tensor_*`/`KvCache_*` | Done | tensor.fd runtime-verified; found + fixed the `Tensor` name collision (below) |
| A4 | `veil_run` → `run_with_coverage` | Done | Typecheck; note covered-set still needs the Section-B debug hook to go above 0% |
| A5 | `Yield` tag 25 → 136 (`token.fd`) | Done | `token_kind_tag` now injective over all 137 variants |
| A6 | `build-x86` command (x86.fd backend) + `codegen` command (x86-64/arm64/riscv asm + ELF/COFF/Mach-O emit) | Done | Dispatch + handlers typecheck; usage text updated |
| A7 | Dropped `main.fd` commands | Partially restored | `build-x86`, `codegen`, `gpu`, `verify`/`audit`, `sourcemap` wired. Deferred: `link` (needs `p_file_read_bytes`), `sbom`/`spdx`/`qualify` (helpers never ported), `debug`/`replay`/`native-debug`/`autopsy` (ptrace + interp debug hook), real `watch` (inotify/poll — currently a `bench` alias, pre-existing), `diff-test` (Rust-side differential harness by design) |

## Cross-cutting fixes discovered during verification

### Duplicate-definition sweep (~60 names → 0)

`fidesc`'s new `find_module_conflicts` check exposed ~72 duplicate
fn/type/const names across the 52-module merge — pre-existing latent
miscompiles (module merge silently kept the last definition; callers in other
modules bound whichever copy survived). Resolved by keeping the canonical name
in one module and prefixing copies elsewhere:

- `runtime_linux.fd`: `pb1`–`pb9` → `rl_pb*` (909 call sites)
- `coff.fd`: `emit_object_file` → `emit_coff_file`, string helpers → `co_*`
- `macho.fd`: `emit_object_file` → `emit_macho_file`, string helpers + `emit_u*` → `mo_*`
- `cparser.fd`: `Param` → `CParam` (different shape than `ast::Param`), `str_*` helpers → `cp_*`
- `x86.fd`: `str_*` helpers → `x86_*` (collided with codegen_x64/codegen_c)
- misc: `str_vec_contains`, `vec_contains_str`, `str_starts_with`, `I64Pair_new`, `c_str_vec_contains` deduped across mono/typechecker/codegen_c/opt/lower/compiler

Result: full 52-module merge now typechecks clean (was previously unverifiable).

### `Tensor` → `TensorData` rename (new defect found by runtime probing)

`Tensor` is a language-builtin type name (`lower.fd:1195`, resolves to
`Opaque("Tensor")`), so `compiler::tensor::Tensor` — the ported Vec-struct —
could never be used through `&self` methods: `&Tensor` params bound the
builtin. Renamed the struct to `TensorData` (42 sites in tensor.fd, 9 in
interp.fd). Runtime-verified: filled/set/get/ndim/size/sum/matmul/KvCache all
return correct values under `fidesc run`. See `tensor.md` addendum.

### `module` is a reserved keyword

New `main.fd` handlers initially bound `ResultIrModule::Ok(module)` — parse
errors. Renamed to `ir_mod`.

## Verification state

- **Full merge + typecheck**: clean — `fidesc run` on a `use compiler::main`
  probe merges all 52 modules with zero duplicate/parse/type errors.
- **interp closure**: `use compiler::interp` probe merges + runs clean.
- **tensor.fd**: runtime-verified correct values.
- **C build of the modified tree**: `fidesc ccompile` generates ~30MB C with
  zero frontend errors. `cl.exe` fails environmentally (no SDK env);
  `zig cc` compiles+links it (needs `-lws2_32`, `-fno-sanitize=undefined`,
  `-Wno-int-conversion` family) but the resulting binary segfaults in
  `Parser_new`/`HashMap` — **unresolved**, possibly zig-codegen on the
  pointer-punning C, possibly a latent bug. Baseline comparison not available
  (no `release/fides-c3.exe` in workspace to diff against).
- **Meta-interp end-to-end** (`interpret_source` on a `p_tensor_*` program
  through the Fides interpreter under the Rust interpreter): launched, still
  running — the full frontend under meta-interpretation is very slow.

## Deferred (Section B prerequisites)

- Interpreter debug hook → veil covered-set, `debug`/`replay`
- `ptrace`/`fork`/`waitpid` → `native-debug`/`autopsy`
- `readdir`/`mkdir`/file-error returns → `scribe <dir>`, output-dir creation
- `p_file_read_bytes` → `link`
- inotify/poll → real `watch`
- Verify-layer helpers (`count_gas_cost`, taint propagation, contract eval) →
  `@gas`/`@secret`/`@dp` enforcement
- f-string sub-lex/parse → `f"{expr}"` interpolation
- `TokenKind::Resume` + parse arms → resumable green threads
