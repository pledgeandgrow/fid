# tv.fd — translation validation of the mid-end

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap RL5. Design: docs/verification/translation-validation.md.

- **Path:** `compiler/tv.fd`
- **LOC:** 6,087 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** llvm-roadmap RL5, competitive-roadmap VF6. With `--validate`
  on the command line or `FID_VALIDATE` set (not 0), the pass manager keeps
  each function as it was before a pass (`tv_before`) and asks whether the
  result means the same (`tv_after`). A rejected result is reported on
  stderr and the function gets its IR from before the pass back. Two
  checks: witnesses for passes that keep value ids (fold, dce, gvn, the
  SSA round trip; since Nov 17 also LICM `tv_licm` and devirtualization
  `tv_devirt`), and runs of both functions on the same random inputs
  (`tv_random`) when they use only integers, plain locals, calls and
  prints. Module passes (inline, inline2, hofspec, ipcp, spec, gconst, icf)
  are checked around the whole pass (`tv_mod_before`/`tv_mod_after`); a
  rejected module pass is undone whole. `tv_report` prints the totals.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `checks`,
  `ssa`, `ir_ser`.

## Callers

| File | What |
|------|------|
| `iropt.fd` | `tv_on`, `tv_before`/`tv_after` around the per-function passes, `tv_mod_before`/`tv_mod_after` around the module passes; the iropt memo (FID_IROCACHE) is off while validating |
| `iropt_me.fd` | the same around the SSA round trip and its own passes |
| `inc.fd` | `FID_VALIDATE`, `FID_TV_PAIRS`, `FID_TV_INJECT`, `FID_TV_INJECT_IN` are part of the build key |

Reached from the builds that run the mid-end (see ssa.md), only when
validation is on.

## Switches

| Variable | Effect |
|----------|--------|
| `FID_VALIDATE=1` or `--validate` | validation on |
| `FID_VALIDATE_SMT=<dir>` | writes SMT-LIB obligations for straight-line pairs (tests/tooling/tv-check.sh runs z3 on them) |
| `FID_TV_INJECT=<passes>`, `FID_TV_INJECT_IN=<prefix>` | puts a fault into what those passes produce; the checks must reject it |
| `FID_TV_SELFTEST=1` | runs `tv_selftest` (pairs each check must accept or reject) |
| `FID_TV_PAIRS=<file or dir>` | runs the before/after pairs of tests/verify/tv (`*.tvp`, 6 files) |

## Status (Nov 21 check, RL5: partly)

tv-check: the 56 pairs of tests/verify/tv come out as expected and wrong
expectations are caught, but `FID_VALIDATE=1 emit test_ssa_validate.fd`
rejects inline2 on `Str_drop` ("the blocks changed"; 1 rejected, 25 not
checkable of 37), so tv-check ends with 3 failed (known-bugs 2g.90).

Fixed Nov 21 (c082ee6, 2g.90): the Str_drop rejection was a false alarm —
inline2 also copies multi-block callees and splits blocks, which sits
outside iro_inline's witness, so a result whose blocks changed is `n/a`,
not rejected (`tv_blocks_kept`). Injected faults now go to a value the
function's behavior depends on (`tv_live_of`, `tv_pure_value`; the
module's constant globals are facts, `tv_gconst_facts`), which is why
most "not caught" faults had rightly changed nothing. tv-check not re-run.

## Findings

- **MEDIUM — FIXED Nov 21 (2g.90):** the inline2 rejection on `Str_drop`
  was a false alarm of the check, fixed as above.
- **INFO:** the checks trust the IR's effect annotations (a call marked
  pure is a function of its arguments) and leave dominance to the IR
  verifier (irverify.fd, `FID_VERIFY=1`). Results neither check applies to
  are counted as not checkable.
