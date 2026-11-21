# pgo.fd — profiles from a real run (profile-guided optimization)

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of competitive-roadmap OP30.

- **Path:** `compiler/pgo.fd`
- **LOC:** 444 (2026-11-21). In git since Nov 15 (1c12c26). No `module`
  line; imported as `compiler::pgo`.
- **Role:** competitive-roadmap OP30. `fid run <file.fd> --profile <out>`
  makes the interpreter count calls and block entries and writes them
  (`pgo_write`: a line `fides-profile 2`, then `<calls> <function>` per
  function, most called first, each followed by `<1 + times entered>
  <function>@<block id>` per block). `--profile-use <out>` on `emit`,
  `ccompile` or `build` loads it: `pgo_apply` puts the most-called
  functions first in the module, and the passes ask `pgo_count`,
  `pgo_block_entries`, `pgo_block_cold`, `pgo_block_weight` and
  `pgo_site_calls`. A version 1 profile (calls only) is still read.
  It also holds the `--trace f,g` list of `fid run` (CT10:
  `g_trace_fns`, `trace_set`, `trace_wanted`), because main.fd and
  interp.fd both import it.
- **Imports:** `core`, `vec`, `string`, `compiler::ir`.

## Callers

| File | What |
|------|------|
| `main.fd` | `--profile-use <file>` sets `g_pgo_use_path`; `run --profile <out>` sets `g_pgo_record` and calls `pgo_write`; `--trace` calls `trace_set` |
| `lower.fd` | `pgo_apply(&mut module)` at the end of `lower`; when recording, `pgo_apply` calls `pgo_instrument` (one empty probe call `<function>@<block id>` per block) |
| `interp.fd` | `pgo_note_call` for each call while recording; `trace_wanted` |
| `iropt.fd`, `iropt_me.fd` | the inliners weigh a call by its block (`pgo_site_calls`); unrolling skips a loop the run never entered |
| `nat_ir.fd` | never-entered blocks laid out last (`ng_pgo_cold_labels`, `FID_NAT_OFF=pgolayout`), spill weights (`FID_NAT_OFF=pgoweights`); the profile's hash is part of the function-cache key |
| `pe_ir.fd` | cold blocks laid out with the panic blocks (`pgo_block_cold`) |
| `nat_enc.fd` | a comment only (function order) |

Switch: `FID_PGO_BLOCKS=0` records calls only (no probes).

## Status (Nov 21 check, OP30: partly)

tests/tooling/pgo-block-check.sh passes 18 checks;
`test_pgo_block_profile` prints its expected values;
tests/tooling/pgo-bench.sh measured 1,349 ms without the profile and
1,149 ms with it (one micro-benchmark, busy machine). Not done: a suite run
with a profile, and benchmark gains on a quiet machine.

## Findings

- **INFO:** block ids are the lowering's. A block the mid-end makes has a
  new id the profile does not know, and counts as hot.
- **INFO:** function names are the IR's after monomorphization: a profile
  is for the program as it was when profiled; a function it does not know
  counts 0.
