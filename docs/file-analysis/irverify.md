# irverify.fd — the IR verifier (own-backend roadmap ME4)

> **New doc, 2026-11-09.**

**Path:** `compiler/irverify.fd` — **144 lines**. Imports `ast`, `ir`,
`checks`, `iropt` (for `iro_cfg`, `iro_dominates`). One constant:
`IV_MAX_ERRORS = 20`.

**Role:** `ir_verify(module, stage)` returns at most 20 messages. Per
function it checks: each block's instructions exist and appear in one block
only; every operand is a parameter or an instruction of the function;
`Jump`/`Branch` targets are blocks of the function; a value is defined
before its use (earlier in the same block, or in a dominating block).
Unreachable blocks are not checked for dominance. It does not check
terminators, instructions after a terminator, or types (`IrModule_verify` in
ir.fd checks terminators).

## Callers

| Caller | Site | When |
|--------|------|------|
| main.fd `nat_verified` | :2779 | printing wrapper |
| main.fd `cmd_build`, native Linux / `--nat` | :3062-3078 | `FID_VERIFY` set: after lowering, iropt, rules, the e-graph |
| main.fd `cmd_build`, Windows pe_ir | :3160-3166 | `FID_VERIFY` set: after lowering and iropt |

Not on the C path (`ccompile`, `dump-c`) or `run`. Status: live, opt-in.

## Findings — verified

### MEDIUM — false error on every `asm!` clobber

Operands come from `checks.fd` `peir_inst_operands`, which includes Asm
clobber inputs with id -1 (lowering's encoding). :116 reports
"%N uses %-1, which is defined nowhere". Verified by a harness that builds
one function with `Asm(... clobber ...)` and calls `ir_verify`: 1 error for
the clobber (plus the expected ones for two bad operands). With s18.exe,
`FID_VERIFY=1 emit` on a program with a clobber printed nothing, so it is
not confirmed that that binary runs the verifier on that path.

### MEDIUM — skipped when the stamp says up to date

`FID_VERIFY` is not in the build key (inc.fd): a second `emit` to the same
output reported "up to date" and verified nothing (verified live).

## Findings — code inspection

### INFO

- Asm outputs are not treated as uses (not in `peir_inst_operands`), so a
  bad output id goes unnoticed.
- Block params (`IrFunction_add_block_param`) have ids with no instruction
  and are not parameters: a use would be reported. Nothing creates them
  today.

## Fix order

1. Skip ids < 0 for Asm (or fix the clobber encoding).
2. Add `FID_VERIFY` to `inc_build_key`.

## Verification state

All 144 lines read. Live: harness run of `ir_verify`; stamp skip observed.
