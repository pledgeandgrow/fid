# ir_ser.fd — IR to bytes and back: build caches, `.fir` files, random IR

> **New doc 2026-11-21.** Read from the source (section headers and the
> cache comments); not every function was read.

- **Path:** `compiler/ir_ser.fd`
- **LOC:** 4,535 (2026-11-21). In git since Nov 13 (1a387e2). No `module`
  line; imported as `compiler::ir_ser`.
- **Role:** a binary codec for `IrFunction` and `IrType` (`iw_irfunc`,
  `ir_irfunc`, `iw_irtype`, `ir_irtype`) over the `AwBuf`/`ArBuf` byte
  buffers of ast_ser.fd. Instructions name functions, strings and globals
  by name, so a replayed function needs no fixups. On it:
  1. **Lowered-IR cache** (CS4; D9, `irc_*`): per function, keyed by a
     hash of its AST (`aw_func`), salted by every declaration of the
     program; a finish rewrites the file with only what the build used.
     OFF by default — measured Nov 22 (D9): replaying a lowered
     IrFunction costs about what lowering it does, so a warm hit saves
     nothing (hello 82 ms slower warm, best of 5; core's IR alone is
     11.4 MB). Opt-in: `FID_IRCACHE=<file>`, or `FID_IRCACHE=auto` for
     `<TEMP>/fid-irc-<hash>.bin` (full builds: run/dump-c/ccompile) and
     `<TEMP>/fid-ircp-<hash>.bin` (`fid emit`'s pruned builds, reach.fd)
     — separate, so the two modes do not thrash each other's entries.
     `FID_IRCACHE=off` (0, no, none, false), `FIDES_NO_CACHE=1` and
     `--no-cache` turn it off; `FID_IRCACHE_STATS=1` counts (stderr).
     Ver 3: a check word per entry (hash ^ key), so a torn or damaged
     file stops loading at the first bad entry instead of replaying
     silently wrong IR. `fid serve` keeps it in memory
     (`FID_SERVE_IRCACHE=1`). Gate: tests/tooling/ircache-check.sh.
  2. **Mid-end memo** (CS1, `mec_*`, `FID_MECACHE`): what the rules
     (rules.fd) and the e-graph (egraph.fd) changed in a function, keyed
     by the function's content hash, the run's switches, the callees'
     effect masks and the compiler; file `<TEMP>/fid-mec-<hash>.bin`.
     `FID_MECACHE=off`, `FIDES_NO_CACHE=1` or `--no-cache` turn it off;
     `FID_MECACHE_STATS=1` counts.
  3. **iropt memo** (CS6, `imc_*`, `FID_IROCACHE`): what iropt_native's
     per-function passes made of a function, keyed by its content hash,
     the transitive closure of the functions it names, the switches and
     the compiler. On by default for `fid emit -o <out>`
     (`<out>.fides-irocache`); `FID_IROCACHE=0` or `--no-irocache` off;
     `FID_IROCACHE_STATS=1`, `FID_IROCACHE_VERIFY=1` (run the passes on a
     hit too and count mismatches). Off with `--validate`, a profile,
     `FID_IROPT_STATS`, inline remarks, `FID_LIBFACTS_IN`.
  4. **Whole modules** (llvm-roadmap RL10, `ir_module_bytes`,
     `ir_module_save`, `ir_module_load`): `fid emit --save-ir <f.fir>`
     writes the module as it goes into the machine layer (magic "FIR1",
     version 3, the target); `fid emit-ir <f.fir> [--target <t>] [-o
     <out>] [--mid-end]` reads it and runs the machine layer alone.
  5. **Random IR** (RL10, `irfuzz_*`): `fid ir-fuzz <seed>` replaces a
     function of a small program with IR made from the seed; its printed
     value is known (`irfuzz_eval`), and scripts/fuzz/fuzz_ir.sh compares
     it with what the machine code prints.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `ast_ser`,
  `inc`.

## Callers

| File | What |
|------|------|
| `lower.fd` | the lowered-IR cache (`irc_begin`, `irc_lookup`, `irc_record`, `irc_finish`) |
| `rules.fd`, `egraph.fd` | the mid-end memo (`mec_*`, `mec_log_inst`, `mec_log_block`) |
| `iropt.fd` | the iropt memo (`imc_*`) |
| `main.fd` | `--save-ir`, `emit-ir`, `ir-fuzz` |
| `compiler.fd`, `pe_ir.fd`, `tv.fd` | the codec (function bytes and hashes) |

## Checks

tests/tooling/mecache-check.sh, tests/tooling/irocache-check.sh,
tests/tooling/ircache-check.sh.

## Findings

- **MEDIUM — OPEN (known-bugs 2g.93):** `FID_IROCACHE_VERIFY=1` reports
  mismatches on 80-85% of iropt memo hits (hello 18 of 21,
  test_matrix_mul 74 of 91, test_hashmap 98 of 116), although the
  executables are byte-identical: either the memo replays other IR or the
  verify comparison is wrong. The memo is on by default for `fid emit -o`.
- **FIXED (SP13):** byte buffers are written one byte per element
  (`iw_bytes`); `iw_vec_i64` wrote 8 bytes per byte, and the nested
  buffers of the lowered-IR cache grew to 357 MB, then an out-of-memory
  on the next build. A length past the end of the buffer now marks the
  read failed instead of allocating.
