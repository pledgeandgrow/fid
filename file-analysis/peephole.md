# peephole.fd — asm-level peephole optimizer

> **Re-verified 2026-10-23:** `notq`/`notl` flag-misclassification mechanism confirmed — NOT does not affect flags yet the pass treats it as flag-setting → adjacent `test` removal can leave a jcc reading stale flags. Inherited from Rust (:431).

- **Path:** `compiler/peephole.fd` — **1,150 LOC** (Rust `src/peephole.rs` = 994, ~116%)
- **Role:** the 9-pass string-level peephole over generated x86-64 asm — the *last* transform before object emission. AGENTS.md: CRITICAL — always on in `build`, a wrong transform silently corrupts all output.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :1685 | `asm = peephole_optimize(asm)` — the x86-64 ELF `build` path, unconditional |

`peephole_optimize_pass` (single-pass runner, :72) has **zero callers** — same dead API as Rust's `optimize_pass`.

## Verified faithful (code inspection)

- Driver 1:1 — same 9 passes in the same order, same 5-iteration fixpoint on `lines.len()` convergence (:51-66 vs Rust :33-49).
- `AsmLine` parser + classification (`is_label`/`is_directive`/`is_other`) matches; labels parse to `is_other=false` → they **stop** every lookahead scan.
- `pass_dead_store` (:511) — lookahead breaks on the first non-comment line including labels → a label between `movq %rax,%rbx` and `movq %rcx,%rbx` keeps the first mov. Safe.
- `pass_branch_threading` (:380) — `resolve_jump_target` carries a `visited` set; label→jmp chains and cycles (`L1→L2→L1`) terminate. Same as Rust :312.
- `pass_cmp_setcc_jcc` (:857) — the `cmpq; setX %al; movzbq %al,%rax; testq %rax,%rax; jne` → `cmpq; jX` fusion is literal-matched on all five instruction forms; `setcc_to_jcc` table is all 11 entries identical to Rust, including the same `jne`-only restriction (no `je` arm either side).
- `pass_redundant_test` (:599) — flag-state resets on labels *and* directives (`!is_other` → reset); requires exact dst==operand match before dropping `testq %r,%r`.

## Findings

### MEDIUM — `notq`/`notl` misclassified as flag-setting *(inherited from Rust)*

`is_flag_setting` (:674-695) includes `notq`/`notl` — but x86 `NOT` is the one bitwise op that **does not** modify flags. Rust has the identical bug (:431-432).

**Reachable:** `~x` lowers to `notq %r` (codegen_x64.fd:1222) and `!y` lowers to `testq %r,%r; sete` (:1215). So `!(~x)` can emit `notq %r; testq %r,%r` adjacent — the pass drops the `testq` → `sete`/`jz` read flags from whatever ran *before* the `not` → wrong result. Narrow — needs no spill/reload between the two ops — but real.

### LOW — `pass_dead_store`'s explicit `(%r…)` memory checks are dead logic

:545-558 mark `reads_dst` when the *next* mov's src contains `(%rax)`…`(%r15)` — but the actual hazard (reading through `dst`) is already caught by `str_contains(next_src, dst)`. The reg list misses `(%rsp)`/`(%rbp)` anyway — so the block only *reduces* optimization on `(%rax)`-form sources while adding nothing for correctness. Harmless noise.

## Fix order

1. Remove `notq`/`notl`/`negl`? — no: only `notq`/`notl` come out (`neg` *does* set flags). One-line fix, shared with upstream Rust.
2. (Optional) delete the redundant `(%r…)` contains-block in `pass_dead_store`.

## Verification state

- No live runs — verified by pass-by-pass source audit against `src/peephole.rs`: driver, parser classification, all three scan-safety properties (label-stop, cycle guard, boundary reset), the full cc-mapping table, and the flag-setter table.
- The `not` flag semantics are per Intel SDM — `NOT` is documented as affecting no flags.
