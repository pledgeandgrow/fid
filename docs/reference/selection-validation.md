# Translation validation of instruction selection (RL6)

The machine layer (`compiler/nat_ir.fd`) selects machine instructions for
each MIR operation. Under `FID_VERIFY=1` every machine-layer build first
runs `nat_sel_check(arch)`: each operation is selected alone, with
physical registers, through `ng_mi_dispatch` (the path the replay after
register allocation takes), and the bytes are run by a small evaluator of
the target's instructions (`compiler/nat_enc.fd`, section "RL6") on edge
and pseudo-random values. The result must be what the MIR operation
means. A mismatch, or an instruction the evaluator does not know, stops
the build with a message naming the operation, its registers and the
inputs.

## What is checked

| Part | Function | Operations | Meaning checked |
|------|----------|------------|-----------------|
| integer | `nat_sel_eval_case` | LI, MOV, ADD..SHR, NEG, NOT, DIV, REM, MULH, SETCC, EXT, ASR63, ADDSHL, ADDI, SHI | the destination's value; inputs unchanged |
| floats | `nat_sel_eval_float` | MO_FBIN (+ - * /), MO_FCMP (EQ..GE), MO_I2F, MO_F2I; f64 and f32 | IEEE result (any NaN for a NaN), compares false on NaN but `!=`; F2I only where defined (no NaN, inside the i64 range) |
| memory | `nat_sel_eval_mem` | MO_LOAD / MO_STORE / MO_LDX / MO_STX, sizes 1, 2, 4, 8, signed and unsigned | a load reads the bytes at base + (index << shift) + disp, extended, and writes nothing; a store writes exactly its bytes there |
| branches | `nat_sel_eval_branch` | MO_BRCC (every kind, against a register, the zero register, itself), MO_BNEZ, MO_BEQZ, MO_TBNZ (bits 0, 7, 31, 32, 63, set and clear) | taken exactly when the condition holds; short and long forms |
| calls | `nat_sel_eval_call` | argument setup (MO_LI / MO_MOV into the argument registers, MO_STOUT for stack arguments) then MO_CALL (short and long) or MO_CALLR | at the call, each argument is in its register or its outgoing slot, the target is the callee's, the stack pointer unchanged |

The cases come from `nat_sel_cases` (integer) and `nat_sel_check_more`
(`nat_sel_float_check`, `nat_sel_mem_eval_check`, `nat_sel_branch_check`,
`nat_sel_call_check`). Float cases put operands in general registers (an
f32 as its 32-bit pattern, zero-extended) and, with float registers on
(the default; `FID_NAT_OFF=fregs` turns them off), in float registers.
Memory cases use displacements inside and outside each target's
immediate, a register and the stack pointer as the base, float registers
as the value, every index scale and an index register no indexed form
takes. Call cases cover the platform convention and the internal one
(WP6), with 0, 1, all register and two stack arguments.

## The evaluator

The state is 64 registers (general 0..31, float `32 + f`, the low 64 bits
of xmm / d / f registers: x86-64 keeps the rest of a register an f32
operation writes, AArch64 clears it, RISC-V NaN-boxes an f32), the
flags, and a byte memory (`NselM`): a byte nothing wrote holds a pattern
of its address, or a fixed byte (0x80, 0x7F, 0xFF) so sign extension is
tested both ways; every write is kept, in order. A run starts at byte 0
and stops at a position (the code's end, or the fall-through of a branch,
whose target is placed after a gap), or at a call, whose position,
target and registers are kept (the callee is not run).

Covered instructions, besides the integer ones: x86-64 `jmp`/`jcc`
(rel8, rel32), `call` rel32 and through a register, `test` with an
immediate, `bt`, `mov`/`movzx`/`movsx`/`movsxd` loads and stores (ModRM,
SIB, displacements), SSE `movq`/`movd`, `movsd`/`movss`, `movaps`,
`addsd`..`divsd` and the single forms, `ucomisd`/`ucomiss` (with the
parity flag, `setp`/`setnp`), `cvtsi2sd`, `cvttsd2si`/`cvttss2si`,
`cvtss2sd`/`cvtsd2ss`, `pxor`; AArch64 `b`, `bl`, `b.cond`, `cbz`/`cbnz`,
`tbz`/`tbnz`, `blr`, `br`, `ldr`/`str` of every size and sign (unsigned
offset, register offset, float registers), `add` (extended register),
`fmov`, `fadd`..`fdiv`, `fcmp`, `fcvt`, `scvtf`, `fcvtzs`; RISC-V
`beq`..`bgeu`, `jal`, `jalr`, `auipc`, `lb`..`ld`, `lbu`..`lwu`,
`sb`..`sd`, `flw`/`fld`/`fsw`/`fsd`, `fadd`..`fdiv` (.d, .s), `feq`/`flt`/
`fle`, `fmv` between the register files, `fcvt` (f32/f64, from and to an
integer), `fsgnj`/`fsgnjx`. Float arithmetic is the host's f64 (an f32
operation is the f64 one on the widened operands rounded once to single,
the single-precision result for + - * /); compares and the f32 widening
work on the bits.

## Found by it

The first run found that an AArch64 `MO_LOAD` / `MO_STORE` with the
stack pointer as its base and a displacement outside the immediate
computed its address from `xzr` (ADD, shifted register, reads register 31
as zero): `nat_enc.fd a64_add_base` now uses ADD (extended register) for
a base of sp, in both the selection table (`ng_isel_emit_mem`) and the
hand-written selection (`ng_load`, `ng_store`, `ng_fmem`). The MIR the
lowering makes reaches the frame through `MO_LDSLOT` / `MO_STSLOT`
(correct already), so no program was miscompiled; the bytes for every
other base are unchanged.

## Running it

- Every `FID_VERIFY=1` build on the machine layer (`fid emit --target
  x86_64-linux|aarch64-linux|riscv64-linux`, `--nat`), e.g.
  `scripts/nat-tools-check.sh` and `FID_VERIFY=1 NATIVE=1
  scripts/cross-test.sh <target>`; `tests/bootstrap/
  test_nattools_selcheck_more.fd` uses the operations.
- Directly: `fid ccompile tests/nattools/selcheck_more_unit.fd -o
  selcheck_more && ./selcheck_more` prints each part per target, the
  whole check with `--cpu` x86-64-v3 and rva23u64, and that 12 mutated
  selections (a wrong operation, size, condition, sign, a missing
  argument, a wrong call target) are rejected and a correct one accepted.
  Run it also with `FID_NAT_OFF=fregs` and `FID_NAT_OFF=isel`.

Not covered: vector operations (TG11 checks their rows byte for byte,
`nat_vsel_check`), the memory-operand ALU (`MO_ALUM`, `MO_BRCCM`,
`MO_SETCCM`, `MO_ALUMST`; `nat_memop_unit_check` checks their formation),
jump tables, the prologue and epilogue, calls with float arguments.
