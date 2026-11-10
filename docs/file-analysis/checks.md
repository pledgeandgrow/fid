# checks.fd — runtime checks a function does not need

> **New doc, 2026-11-09.** Working tree has a small uncommitted edit
> (`peir_inst_of` returns `Unreachable` for a missing id, M9).

**Path:** `compiler/checks.fd` — **1,332 lines**. Imports `ast`, `ir`.

**Role:** per-function analysis that marks operations needing no overflow
or bounds check. `peir_unchecked_ops(f, nvals)` returns a flag per value:
bit 1 = no check needed, bit 2 = value known >= 0. Three analyses:

1. **Counted loops** (port of src/pe.rs `unchecked_ops`, :109-520): in
   `while i < n` where only `i = i + 1` changes `i`, the increment cannot
   overflow; if `n` is `v.len()` (or a local set once to it), `i` starts at
   a constant >= 0 and nothing can shorten `v`, then `v.get(i)`/`v.set(i, x)`
   before the increment are in bounds.
2. **Integer ranges** (OP24, `PEIR_RANGES`, :576-1172): an interval per i64
   value from constants, `% c`, `/ c`, `& c`, `>> c`, narrow types, lengths,
   local stores and branch refinement; add/sub/mul whose interval stays in
   i64 are unchecked.
3. **Filled Vecs** (:1175-1332): a Vec local only ever holding a
   `Vec_filled_typed(n, ...)` result (or zero) and only used by get, set,
   push, len, share, drop never has fewer than the least `n`.

Also shared helpers used across the backends and optimizer: `peir_zeros`,
`peir_inst_of`, `peir_inst_operands` (operands of an instruction),
`peir_add_edge`, `peir_reach`.

**Switch:** `PEIR_RANGES = true` (:587) turns analysis 2 (and 3, which
runs inside it) on.

## Callers

| Module | Uses | Reached from |
|--------|------|--------------|
| codegen_c.fd | `peir_unchecked_ops` | `ccompile`, `compile`, `dump-c`, `split-c`, `run --leaks` |
| pe_ir.fd | `peir_unchecked_ops`, `peir_inst_operands`, `peir_zeros` | `emit`/`build` (Windows PE, ELF) |
| nat_ir.fd | `peir_unchecked_ops` | `emit --target *-linux`, `--nat` |
| iropt.fd, iropt_me.fd, rules.fd, egraph.fd | `peir_inst_of`, `peir_inst_operands`, `peir_zeros`, `peir_add_edge` | native mid-end |
| irverify.fd | `peir_inst_operands` | `FID_VERIFY=1` |
| lower.fd | `peir_inst_operands` (through `use compiler::pe_ir`) | every lowering |

Not reached by `run` (the interpreter).

## Findings — verified live

### HIGH — an `asm!` clobber in a loop crashes the analysis

`peir_inst_operands` pushes every Asm input id (:563), and lowering gives a
clobber id -1 (lower.fd:10186). `peir_loop_facts` then runs
`facts.plain.set(op, 0)` with op = -1 (:278): "Vec index out of bounds".
The loop facts are built only for functions with a back edge, so straight
code is fine. Probe: `dump-c`, `emit` and `emit --nat` all panic in
`peir_loop_facts`. Any consumer indexing by operand id has the same risk.
Fix: skip ids < 0 in `peir_inst_operands` (or fix the encoding in lower).

## Findings — code inspection

### LOW (perf) — ownership calls defeat the counted-loop proof

`peir_may_shorten` (:54) treats every call not on its short list as able to
shorten a Vec. With `LOWER_RC` on, loop bodies hold `Str_drop`,
`Vec_drop`, `Str_dup`, `Vec_share` calls, so analysis 1 gives up in those
loops. Analyses 2 and 3 may still cover them.

### INFO — soundness notes

- Analysis 3 rejects any other use of the slot or of a loaded value (stored
  elsewhere, passed by pointer, a second argument position), which keeps it
  sound. `Vec_share` counts as keeping the length because the share is
  copy-on-write.
- `peir_same_vec` accepts loads of the same plain slot or parameter with no
  store to it in the loop body.

## Fix order

1. Ignore negative operand ids in `peir_inst_operands`.
2. Teach `peir_may_shorten` the ownership calls that cannot shorten a Vec
   (`Str_dup`, `Str_drop`, `Vec_share` of another Vec).

## Verification state

Read: all of analysis 1 and 3, the range section headers and helpers. Live:
the asm crash on three paths. Range rules (analysis 2) not audited line by
line.
