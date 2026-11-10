# opt.fd — PIR L2 optimizer

> **Re-audited 2026-11-09.** The optimizer now panics on every program (`hello.fd`) in
> `pass_jump_thread`; it is reached only from `fid opt`, `fid pir --opt` and `fid build --native`
> with the default target, none of them a default path. Earlier findings re-checked below.

- **Path:** `compiler/opt.fd` — **7,764 LOC** (was 7,923 on Oct 23; Rust `src/opt.rs` = 8,573)
- **Role:** the L2 pass pipeline over PIR — 22 `Pass` variants, 19 in `Pass_all()` (:118),
  fixpoint driver `optimize_l2_with` (:243: `optimize_l2` 8 iterations, `optimize_l2_lto` 16),
  S-1A gating (`pass_relevant_for_module` :320), final `topo_sort_block` (:4396).

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` `build_executable` | :2147 / :2149 | `optimize_l2_lto` with `--lto`, else `optimize_l2` unless `--no-opt` |
| `main.fd` `cmd_opt` (`fid opt`) | :622 | `optimize_l2` then print L2 (the `[pass1,...]` argument is ignored) |
| `main.fd` `cmd_pir` (`fid pir <f> 2\|all --opt`) | :564, :581 | `optimize_l2` |

Importers: `compiler.fd`, `main.fd`. `build_executable` is reached by `fid build --native` only
when the target is not a nat_ir one: default `x86-64` (→ x86-64 ELF) or `windows` (→ iropt +
pe_ir; the L2 result is unused there). `--native --target arm64|riscv64|x64` now goes to nat_ir
(main.fd:3011). Not the default for any command.

## Findings

### Verified live

#### HIGH — `optimize_l2` panics on hello.fd

`s18.exe opt tests/bootstrap/hello.fd`, `pir <f> 2 --opt` and `build hello.fd --native`:
"Vec index out of bounds" in `Vec_get` ← `jump_thread_redirect` ← `jump_thread_function` ←
`pass_jump_thread`. `jump_thread_function` (:3159) passes `b.predecessors` entries (block ids)
as `pred_id`, and `jump_thread_redirect` (:3236-3237) uses it as an index:
`f.blocks.get(pred_id)`; it also passes the index `bi` as `from_block` and compares it with jump
targets, which are ids. Fix: map ids to indexes (or look the block up by id) in both places.
(AGENTS.md still names `pass_dse` and register allocation as the crash.)

### Code inspection

#### MEDIUM — `Licm` and `EscapeAnalysis` still absent — STILL TRUE

No `Pass` variants, impls or stats fields (grep: 0 hits). `Pass_all()` has 19 entries (:118-141)
against Rust's 21.

#### MEDIUM — sandbox inlining tier still missing — STILL TRUE

`inline_once` (:3975) builds one `eligible_names`/`eligible_funcs` set (:3987); no
`sandbox_eligible`. Sandbox devirt is present (`devirt_sandbox_function` :3061).

#### MEDIUM — alias analysis shortcuts — CHANGED

1. `borrow_kinds` still has no consumer in opt.fd (grep 0). The producer side changed: lower.fd
   now sets `meta.borrow_kind` on `&`/`&mut` parameters (lower.fd:4129, uncommitted), and pir.fd
   copies it into `L1Function.borrow_kinds` (pir.fd:1181-1188). Only the consumer is missing.
2. `is_lin` still hard-coded false (:5695-5697, "Would check linear_structs").

#### INFO

- Nov 3 (e655b58): helpers that pushed into a by-value `Vec` parameter (`used_push`,
  `copy_of_set`, ...) now take `&mut Vec`; under value semantics they had dropped their writes.
- Oct 24: `rpo_dfs` is iterative (deep CFGs overflowed the stack).
- `fid opt`'s usage text lists six passes and a pass-list argument; both are ignored.

## Fix order

1. Fix the id/index mix in jump threading (makes `opt`, `pir --opt`, `build --native` usable).
2. Consume `borrow_kinds` in `pass_alias_analysis` now that the lowering produces them.
3. Licm / EscapeAnalysis, sandbox tier, `linear_structs` only if the PIR path is kept.

## Verification state

- Live with s18: `opt` and `pir --opt` panic as above; `build --native --no-opt` gets past opt
  and fails at link ("Undefined symbols: main", codegen/linker, not this file).
- Grep-checked: Licm, EscapeAnalysis, sandbox_eligible, borrow_kinds, is_lin.
