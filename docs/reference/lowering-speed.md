# Lowering speed: work that grew with a function's length (SP12)

SP12 (llvm-roadmap, the same goal as competitive-roadmap CS4) is a
self-compile under 10 s. Lowering (`compiler/lower.fd`) and C generation
were about 22 s of the 25 s. This page records what was found to grow
faster than linearly in the lowering, what was changed, how it was checked,
and what is left.

## How it was found

`tests/perf/sp12_gen.py N` writes a program whose one function has N groups
of ordinary statements (a string kept in a second variable, a string copied
from a place, a local Vec, a Vec copied and only read). On the Nov 16
compiler, `dump-c --time` of it gave:

| N | statements | lower |
|---|-----------|-------|
| 300 | 2,700 | 65 s |
| 600 | 5,400 | 249 s |

Twice the statements took four times as long: every pass below was
quadratic in the length of a function or of a block. `tests/perf/
sp12_lower_bench.fd` runs `lower` alone on a file inside one process; a
stack sampler on it showed where the time went, and each fix was measured
again the same way.

## What changed (all in the frontend group's files)

Every change gives the same answer the old code gave; only how it is found
differs.

| Where | Was | Now |
|-------|-----|-----|
| `Lowerer_last_use` (lower.fd) | for each kept variable, `cow_uses_stmt` on every later statement of each enclosing list, and `lw_has_defer` of the whole body | one name index per statement list (`fr_lu`, filled once by cowcheck's `cow_names_stmt`, `Lowerer_used_after`); `cur_has_defer` worked out once per function |
| `Lowerer_rc_move_last` | the last-use walk first | the cheap checks first (an owned slot, not a loop's condition) |
| `Lowerer_readonly_copy_in` (a copy of a place) | the rest of the block copied, then walked six times | only the statements that mention the copy or its root, each alone (`Lowerer_readonly_rest`); the full walk when the index cannot tell |
| `Lowerer_lookup_alloca`, `Lowerer_lookup_var_ast_type` | scan of every variable of the function | name index with links down the stack (`lw_ix_link`, `lw_ix_last_alloca`), rebuilt when the tables are replaced whole |
| `lw_rc_cancel_pairs` | `lw_rc_value_used` / `lw_rc_use_count` per candidate: a walk of the function each | use counts once (`lw_rc_use_counts`), kept up to date by `lw_rc_rewrite_arg` |
| `lw_rc_literal_slot_counts` | `lw_rc_value_used` per candidate | counts gathered in its own pass |
| `lw_rc_empty_drops` | per candidate, the block searched from the load forward and back | per block, each slot's previous and next reference (`lw_rc_empty_block_refs`) |
| `lw_rc_stack_vecs_fn` | each stack Vec rewrote its drops by a walk of the function | one pass for all (`lw_rc_stack_drops`) |
| `lw_detached` | the block's instruction list copied on every pop (the block was still in `f.blocks`, two owners) | the block taken out first |
| escape.fd `esc_declare_fn` | a copy of the block's variable list held while pushing: the push copied the whole list | the list read in place |
| escape.fd `esc_lookup`, `esc_add_roots` | every variable copied while searching | names compared in place |

## Measured

`sp12_lower_bench` built with stage0 (`dump-c`, then `zig cc -O1`), the same
generated programs, lowering only:

| N | midway (part of the changes) | after | after, with the ir.fd hunk |
|---|-----|-----|-----|
| 100 | 0.39 s | 0.22 s | 0.022 s |
| 200 | 1.8 s | 0.81 s | 0.068 s |
| 400 | 8.3 s | 3.7 s | 0.16 s |
| 800 | 32 s | 11.6 s | 0.53 s |
| 1600 | | 39 s | 1.8 s |

(Another job ran on the machine at the same time: the numbers are rough.)
Without the ir.fd hunk below, copying the block's instruction list on every
emitted instruction is most of what is left; with it, the long function
lowers about 20x faster than before and close to linearly (3.3x for 2x).
The first measurement, on stage0 itself, is the table further up.

## Left

- `compiler/ir.fd` `IrFunction_emit` (another group's file): it takes the
  block out of `self.blocks` with `get`, pushes the new instruction and sets
  it back, so the push finds the instruction list with two owners and
  copies it, once per instruction emitted: quadratic in a block's length,
  the largest single cost left (half the lowering time of the long
  function). Setting a fresh `BasicBlock_new(block_idx)` in its place right
  after the `get` makes the push change the list in place (checked on a
  stand-alone program: 20,000 pushes, 1,373 ms before, 2 ms after, same
  contents). The same in `IrFunction_add_block_param`.
- `lw_rc_cancel_param_slots` (`lw_ps_*`) and `Lowerer_lookup_alloca_type`
  still scan per candidate; they were 22% and 10% of the long-function
  bench after the changes above.
- C generation (codegen_c.fd, checks.fd: other groups) was not changed.
- Lowering or generating C per function in parallel, or from the
  per-function cache, is not done.
- Nothing here is measured on `dump-c compiler/main.fd`: the self-compile
  time with these changes is for the final build to record.

## Checking it

- `bash tests/perf/sp12_scaling.sh` (FID set): lower(2N) / lower(N) below
  3.0, and the generated program's output equal to what `sp12_gen.py
  --expect` computes.
- `tests/selfhost/test_cow_names.fd`: `cow_names_stmt` agrees with
  `cow_uses_stmt` for every statement and name of its fixture.
- `tests/bootstrap/test_sp12_lowering.fd`: moves, defers, closures, stack
  Vecs, shared Vecs, emptied Vecs and string pairs keep their meaning.
- During development each new table was compared with the old scan at
  every query (a copy of lower.fd with assertions) while lowering every
  program under tests/bootstrap, tests/leaks, tests/conformance and several
  compiler modules: no difference.
