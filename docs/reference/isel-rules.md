# Instruction selection as rules (machine layer)

llvm-roadmap TG1 and TG2. The machine layer (`compiler/nat_ir.fd`, the
Linux AArch64 / RISC-V 64 / x86-64 targets and `--nat` Windows) selects
target instructions for its machine IR (MIR) operations from a table of
rules written as Fides data, not from hand-written branches per target.

## The table

`ng_isel_rows` and `ng_isel_rows_tg1` build the rows; `ng_isel_build`
groups them by `(arch, op)` and sorts each group by cost (stable). A row is

    [arch, op, form, cost, pre, arg, parg]

- `form` names the target instruction or short sequence (`NG_IF_*`), and
  `arg` is the form's own operand (an ALU kind, a test-against-zero flag,
  the swap / unsigned / invert bits of a RISC-V compare).
- `pre` is the precondition (`NG_IP_*`) and `parg` its operand: an
  immediate class (`IMM_U32`, `IMM_S32`, `IMM_S12`, `IMM_MOVZ`,
  `IMM_MOVN`), a compare kind (`CC`), the second operand being the zero
  register (`B_ZERO`, `ZERO_CC`), a branch in short reach (`SHORT`), a
  displacement the target's load / store immediate holds (`A64_UIMM`,
  `DISP_S12`), an indexed form the target has (`INDEXED`), float
  registers (`FREG_MEM`, `FREG_OP`), an element scale (`SH_IDX`,
  `SH_ZERO`, `SH_ZBA`).
- `cost` orders the rules of an operation: instructions emitted, cheapest
  first. The first rule whose precondition holds and whose form does not
  decline is taken (`ng_isel_mi`).

`ng_mi_dispatch` tries the table first for every operation
`ng_isel_covers` names; the hand-written selection (`ng_li`, `ng_load`,
`ng_brcc`, `ng_fbin`, ...) runs when no rule applies and with
`FID_NAT_OFF=isel`.

## What the table covers

| Family | MIR | Rules |
|---|---|---|
| ALU | ADD, SUB, MUL, AND, OR, XOR, SHL, SAR, SHR, ADDI | x86-64 two-operand forms, `add r, imm` / LEA; AArch64 MADD / MSUB folding, three-register forms, ADD / SUB #imm; RISC-V R-type, ADDI |
| Constants | LI | x86-64 `xor r32` / `mov r32, imm32` / `mov r/m64, simm32` / `movabs`; AArch64 MOVZ / MOVN (one 16-bit piece) / MOVZ+MOVK sequence; RISC-V ADDI / LUI+ADDIW / longer chain |
| Shifts by a constant | SHI (new, code 56) | x86-64 `shl/sar/shr r, imm8`; AArch64 LSL / ASR / LSR #imm (UBFM / SBFM); RISC-V SLLI / SRAI / SRLI |
| Element addresses | ADDSHL | x86-64 LEA with a scaled index (or through r11 past scale 8); AArch64 ADD lsl; RISC-V ADD, Zba shNadd, SLLI+ADD |
| Compares | SETCC, BRCC | x86-64 TEST / CMP + SETcc / Jcc; AArch64 CMP + CSET, CBZ / CBNZ, CMP + B.cond; RISC-V SLT(U) with swap and XORI per kind, SUB + SLTIU / SLTU, B<cc> short or inverted over JAL |
| Loads, stores | LOAD, STORE, LDX, STX | float register forms; x86-64 `[base + disp32]`, SIB; AArch64 scaled unsigned offset or the displacement through a scratch register, register offset; RISC-V 12-bit offset or through a scratch register; ADDSHL then the access where no indexed form applies |
| Negation, extensions | NEG, NOT, EXT | x86-64 NEG / NOT (after a move), MOVSX / MOVZX / MOVSXD / `mov r32` by kind; AArch64 NEG, MVN, SXTB..UXTW; RISC-V SUB from zero, XORI -1, Zbb / Zba one-instruction extensions or ADDIW / ANDI, else SLLI + SRAI / SRLI |
| Floats | FBIN, FCMP | float registers (MI25), else the bits through the scratch pair: SSE2, AArch64 FP, RISC-V D |
| Vectors | VBIN | SSE2 / AVX2 / NEON / RVV (TG11, `docs/reference/vector-mir.md`) |
| Division, high product | DIV, REM, MULH (i0 != 0: unsigned) | x86-64 IDIV / DIV / IMUL / MUL over rax:rdx (CQO or rdx = 0 first, the other operand in r11); AArch64 SDIV / UDIV, SDIV + MSUB for the remainder, SMULH / UMULH; RISC-V DIV(U) / REM(U) / MULH(U) |
| Sign mask | ASR63 | x86-64 SAR 63 (after a move); AArch64 ASR #63; RISC-V SRAI 63 |
| Conversions | I2F, F2I (i0 != 0: f32) | float registers (MI25) first; x86-64 CVTSI2SD / CVTTSD2SI (xmm0 reused when it holds the source); AArch64 SCVTF / FCVTZS; RISC-V FCVT.D.L / FCVT.L.D |
| Fused bit operations | ANDN, BLSR (kinds BLSR / BLSMSK / BLSI) | x86-64 BMI1 under `--cpu x86-64-v3` / `v4`, else through r11; AArch64 BIC, NEG [+ MVN] then AND / EOR; RISC-V Zbb ANDN under `--cpu`, else XORI + AND; ADDI / SUB then AND / XOR |
| Select | CSEL | x86-64 TEST + CMOVcc; AArch64 CMP + CSEL; RISC-V Zicond (`--cpu rva23u64`) CZERO + OR, else a branch over a move; nothing when the destination is the value |
| Test bit and branch | TBNZ | x86-64 TEST imm / BT + Jcc (rel8 when short); AArch64 TBZ / TBNZ when short, else the inverse over B; RISC-V the bit to the sign then BLT / BGE, over JAL when long |
| 32-bit operations | OPW (RISC-V only) | ADDW / SUBW / MULW |
| Memory operand | ALUM (x86-64 only) | `op r, [base + disp]` after a move; through r11 when the destination is the base |

MO_SHI is formed by `ng_fuse_addi` from a SHL / SAR / SHR whose count is a
known constant (taken mod 64, as the register forms do). `FID_NAT_OFF=shi`
keeps the register forms.

## Checks (TG2)

Under `FID_VERIFY=1`, once per build (`nat_sel_check`, RL6):

- every case of `nat_enc.fd nat_sel_cases` (LI, the ALU, ADDI, SHI,
  ADDSHL, SETCC, NEG, NOT, EXT, ...) is selected and run by the selection evaluator
  on edge and random values against the operation's meaning, and its
  bytes must equal the hand-written selection's;
- `nat_isel_mem_check`: loads and stores (every size and signedness,
  displacements inside and outside each immediate, the stack pointer as a
  base, an index no indexed form takes), compare-and-branch (every kind,
  against a register and zero, short and long) and float operations (f64,
  f32, float registers under `FID_NAT_ON=fregs`) give the same bytes from
  the table as from the hand-written selection;
- `nat_isel_rule_check`: every rule of an operation the evaluator runs is
  tested on its own: random registers and constants are drawn until the
  rule is the one the table would select (its precondition holds and no
  cheaper rule's does), then that rule's form alone and the generic
  lowering (the hand-written selection) are both run on values against
  the operation's meaning. A rule no draw reaches is an error. `--stats`
  prints `[isel] TG2: N rules of the table checked alone on random
  operands`.

Mutations (`FID_NAT_MUTATE`, never in a real build) show the checks see
the table: `isel` (SUB's operands swapped), `iselimm` (the constant of
LI / ADDI / SHI rules off by one), `iselcc` (SETCC / BRCC rules on the
inverse condition), and `shi` (the fused count off by one).

Program-level tests: `tests/bootstrap/test_machine_isel.fd`,
`tests/bootstrap/test_machine_isel_tg1.fd`, the `shi`, `li`, `cmp` and
`mem` lines of `tests/bootstrap/test_machine_rules_random.fd`. Scripts:
`tests/nattools/isel_table_check.sh`, `tests/nattools/isel_rules_check.sh`,
`tests/nattools/rule_mutation_check.sh`.

The rows of the last families (`ng_isel_rows_tg1b`, forms in
`ng_isel_emit_tg1b`, Nov 17) are checked by `nat_isel_tg1b_check` under
`FID_VERIFY=1`: each against the hand-written selection, byte for byte,
for every register shape that changes the code (the destination equal to
an operand, x86-64's rax / rdx as operands, the base as the destination),
signed and unsigned, f64 and f32 in general and float registers, two F2I
of one source in a row (the xmm0 cache), each BLSR kind, both select
senses, bits 0, 7, 31, 32 and 63 short and long, and again under each
`--cpu` that changes a rule (x86-64-v3, rva23u64). The value evaluator
does not run these operations, so `nat_isel_rule_check` leaves them to
this comparison. Mutation `iseldiv` (the division rules take the other
result register or signedness) shows the comparison sees the rows.
Program test: `tests/bootstrap/test_machine_isel_tg1b.fd`; script:
`tests/nattools/isel_tg1b_check.sh` (byte identity of the images with
`FID_NAT_OFF=isel` on the machine tests, each target and `--cpu`, float
registers off and on).

## Not covered by rules yet

Moves, calls, jumps and jump tables, BNEZ / BEQZ, PANICIF / TRAP, the
prologue and epilogue, slot and incoming / outgoing argument accesses
(SLOTADDR, LDSLOT, STSLOT, LDIN, STOUT), COPY, F32BITS, OP18's BRCCM /
SETCCM / ALUMST and the vector operations other than VBIN: these keep
their hand-written selection. AArch64's MADD / MSUB rules are checked by
byte comparison only (they fold the multiply emitted before them, which a
rule run alone does not have).
