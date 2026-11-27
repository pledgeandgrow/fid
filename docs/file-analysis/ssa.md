# ssa.fd — SSA form inside the mid-end (mem2reg, dead phis, loop exit values)

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap ME3.

- **Path:** `compiler/ssa.fd`
- **LOC:** 1,829 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** llvm-roadmap ME3 (mem2reg with merges), ME15 (dead phis and
  stores), ME23 (exit values of counted loops). For one function: find the
  scalar allocas only loaded and stored at their own type, place phis at
  the iterated dominance frontiers of their stores (CFG from `iro_cfg`,
  compiler/checks.fd), rename over the dominator tree, simplify on SSA
  (trivial phis, dead phis, a counted loop's exit value as a constant or
  the loop's bound), then go back to the slot form: each phi left gets an
  Alloca of its own, its predecessors store into it, and the phi becomes a
  Load. The backends never see SSA. Entry point `ssa_round_trip(f,
  exit_values)`; also `ssa_takes` (may the function be converted),
  `ssa_dce`, `ssa_remap`, `ssa_vty`, `ssa_exit_stats_line`.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `checks`.

## Callers

| File | What |
|------|------|
| `iropt_me.fd` `ime_function_passes` | `ssa_round_trip(f, ime_on("ssaexit"))` when `ime_on("ssa")` |
| `iropt.fd` | `ssa_takes` decides where the older `promote` pass still runs (promote is retired where ssa runs) |
| `tv.fd` | checks each round trip against the function before it (`--validate`) |
| `egraph.fd` | imports it |

Reached by every build that runs the mid-end (`iropt_native`): `fid
emit`/`build` on pe_ir (Windows) and the machine layer (Linux, macOS,
`--nat`), `fid opt`, `emit-ir --mid-end`, `:opt` in `fid repl`. Not the C
path (`ccompile`, `dump-c` run no IR optimizer), `fid wasm` or `fid run`.

Switches: `ssa` and `ssaexit` are in iropt_me.fd `IME_DEFAULT_ON` (on by
default since Nov 16); `FID_IROPT_OFF=ssa` or `FID_IROPT_OFF=ssaexit` turn
them off.

## What it leaves alone

A candidate whose load can see no store on some path (a read of an
uninitialized local) stays in memory. Functions with `TryUnwrap` or `Asm`,
`@constant_time` functions and functions whose entry block has
predecessors are not converted. A function with more than `SSA_MAX_PHIS`
(20,000) phis is left as it is.

## Status (Nov 27 check, ME3: done)

On by default. `ssa-default-check` passes with `NAT=1` (2/2). The remaining
15-program machine-layer file list passes `PASS=ssa,ssaexit` with
`FID_VERIFY=1`: 15/15 the same, with no output, build or verifier failure.
The pe_ir list is clean too; one parallel off-build of
`test_mem2reg_promote` was a host out-of-memory artifact and passed when
retried serially. Tests: `test_ssa_merge`, `test_ssa_exit_bound`,
`test_ssa_exit_loop_tree`, `test_ssa_validate` (tests/bootstrap).

`promote` is not retired: since the Nov 21 evening batch it intentionally
runs after `ssa` on the slots SSA leaves in memory. Therefore the old
`promote-retired-check` RETIRED expectation is obsolete; nonzero promote
counts are not failures.

## Findings

- **INFO:** the promoted local's Alloca stays (unused) so `locals` and the
  debug info still name a value that exists.

## Nov 21 evening (optimizer batch, 5bb97b3)

- Fixed: step 5's contract "only that block reads the slot" was broken —
  a phi's load VALUE could be used in another block (the accumulator read
  after its loop), reading the header's last execution forever.
  `ssa_materialize` now gives every such use a fresh `Load` of the slot
  immediately before the use itself (the slot model's exact position; a
  block-top placement was tried first and was unsound when a store to the
  slot precedes the use in that block — test_vectorize caught it).
- Finding: **FIXED (was HIGH)** — loop versioning (iropt.fd) had rightly
  refused those functions ("a value of the loop is read outside it");
  version-default-check is 0 failed with ssa on again.
- Also: ssa's keyword trap for editors — identifiers `q4`/`q8` are i8
  aliases in the language; fid accepts them as names, frozen fidesc does
  not.