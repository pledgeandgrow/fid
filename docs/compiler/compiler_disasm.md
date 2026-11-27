[All modules](index.md)

# compiler::disasm -- Fides Documentation

## Contents

- [DSM_A64:](#DSM_A64:) (const)
- [DSM_RV64:](#DSM_RV64:) (const)
- [DSM_X64:](#DSM_X64:) (const)
- [DsmInsn](#DsmInsn) (struct)
- [disasm_arch_of](#disasm_arch_of) (function)
- [DsmModrm](#DsmModrm) (struct)
- [EFW_A64_ADD:](#EFW_A64_ADD:) (const)
- [EFW_A64_ADDS:](#EFW_A64_ADDS:) (const)
- [EFW_A64_SUB:](#EFW_A64_SUB:) (const)
- [EFW_A64_SUBS:](#EFW_A64_SUBS:) (const)
- [EFW_A64_AND:](#EFW_A64_AND:) (const)
- [EFW_A64_ORR:](#EFW_A64_ORR:) (const)
- [EFW_A64_EOR:](#EFW_A64_EOR:) (const)
- [EFW_A64_ANDS:](#EFW_A64_ANDS:) (const)
- [EFW_A64_BIC:](#EFW_A64_BIC:) (const)
- [EFW_A64_ORN:](#EFW_A64_ORN:) (const)
- [EFW_A64_EON:](#EFW_A64_EON:) (const)
- [EFW_A64_BICS:](#EFW_A64_BICS:) (const)
- [EFW_A64_ADD_IMM:](#EFW_A64_ADD_IMM:) (const)
- [EFW_A64_ADDS_IMM:](#EFW_A64_ADDS_IMM:) (const)
- [EFW_A64_SUB_IMM:](#EFW_A64_SUB_IMM:) (const)
- [EFW_A64_SUBS_IMM:](#EFW_A64_SUBS_IMM:) (const)
- [EFW_A64_ADD_EXT:](#EFW_A64_ADD_EXT:) (const)
- [EFW_A64_SUB_EXT:](#EFW_A64_SUB_EXT:) (const)
- [EFW_A64_MADD:](#EFW_A64_MADD:) (const)
- [EFW_A64_MSUB:](#EFW_A64_MSUB:) (const)
- [EFW_A64_SMULH:](#EFW_A64_SMULH:) (const)
- [EFW_A64_UMULH:](#EFW_A64_UMULH:) (const)
- [EFW_A64_UDIV:](#EFW_A64_UDIV:) (const)
- [EFW_A64_SDIV:](#EFW_A64_SDIV:) (const)
- [EFW_A64_LSLV:](#EFW_A64_LSLV:) (const)
- [EFW_A64_LSRV:](#EFW_A64_LSRV:) (const)
- [EFW_A64_ASRV:](#EFW_A64_ASRV:) (const)
- [EFW_A64_RORV:](#EFW_A64_RORV:) (const)
- [EFW_A64_CSEL:](#EFW_A64_CSEL:) (const)
- [EFW_A64_CSINC:](#EFW_A64_CSINC:) (const)
- [EFW_A64_SBFM:](#EFW_A64_SBFM:) (const)
- [EFW_A64_UBFM:](#EFW_A64_UBFM:) (const)
- [EFW_A64_MOVN:](#EFW_A64_MOVN:) (const)
- [EFW_A64_MOVZ:](#EFW_A64_MOVZ:) (const)
- [EFW_A64_MOVK:](#EFW_A64_MOVK:) (const)
- [EFW_A64_B:](#EFW_A64_B:) (const)
- [EFW_A64_BL:](#EFW_A64_BL:) (const)
- [EFW_A64_BCOND:](#EFW_A64_BCOND:) (const)
- [EFW_A64_CBZ_W:](#EFW_A64_CBZ_W:) (const)
- [EFW_A64_CBNZ_W:](#EFW_A64_CBNZ_W:) (const)
- [EFW_A64_CBZ:](#EFW_A64_CBZ:) (const)
- [EFW_A64_CBNZ:](#EFW_A64_CBNZ:) (const)
- [EFW_A64_TBZ:](#EFW_A64_TBZ:) (const)
- [EFW_A64_TBNZ:](#EFW_A64_TBNZ:) (const)
- [EFW_A64_ADRP:](#EFW_A64_ADRP:) (const)
- [EFW_A64_ADR:](#EFW_A64_ADR:) (const)
- [EFW_A64_BLR:](#EFW_A64_BLR:) (const)
- [EFW_A64_BR:](#EFW_A64_BR:) (const)
- [EFW_A64_RET:](#EFW_A64_RET:) (const)
- [EFW_A64_NOP:](#EFW_A64_NOP:) (const)
- [EFW_A64_SVC0:](#EFW_A64_SVC0:) (const)
- [EFW_A64_BRK:](#EFW_A64_BRK:) (const)
- [EFW_A64_DMB_ISH:](#EFW_A64_DMB_ISH:) (const)
- [EFW_A64_CLREX:](#EFW_A64_CLREX:) (const)
- [EFW_A64_MRS_TPIDR:](#EFW_A64_MRS_TPIDR:) (const)
- [EFW_A64_MSR_TPIDR:](#EFW_A64_MSR_TPIDR:) (const)
- [EFW_A64_STRB:](#EFW_A64_STRB:) (const)
- [EFW_A64_LDRB:](#EFW_A64_LDRB:) (const)
- [EFW_A64_LDRSB:](#EFW_A64_LDRSB:) (const)
- [EFW_A64_STRH:](#EFW_A64_STRH:) (const)
- [EFW_A64_LDRH:](#EFW_A64_LDRH:) (const)
- [EFW_A64_LDRSH:](#EFW_A64_LDRSH:) (const)
- [EFW_A64_STR_W:](#EFW_A64_STR_W:) (const)
- [EFW_A64_LDR_W:](#EFW_A64_LDR_W:) (const)
- [EFW_A64_LDRSW:](#EFW_A64_LDRSW:) (const)
- [EFW_A64_STR:](#EFW_A64_STR:) (const)
- [EFW_A64_LDR:](#EFW_A64_LDR:) (const)
- [EFW_A64_STR_Q:](#EFW_A64_STR_Q:) (const)
- [EFW_A64_LDR_Q:](#EFW_A64_LDR_Q:) (const)
- [EFW_A64_STP_POST:](#EFW_A64_STP_POST:) (const)
- [EFW_A64_LDP_POST:](#EFW_A64_LDP_POST:) (const)
- [EFW_A64_STP:](#EFW_A64_STP:) (const)
- [EFW_A64_LDP:](#EFW_A64_LDP:) (const)
- [EFW_A64_STP_PRE:](#EFW_A64_STP_PRE:) (const)
- [EFW_A64_LDP_PRE:](#EFW_A64_LDP_PRE:) (const)
- [EFW_A64_LDAXR:](#EFW_A64_LDAXR:) (const)
- [EFW_A64_STLXR:](#EFW_A64_STLXR:) (const)
- [EFW_A64_LDAR:](#EFW_A64_LDAR:) (const)
- [EFW_A64_STLR:](#EFW_A64_STLR:) (const)
- [EFW_A64_FMOV_DX:](#EFW_A64_FMOV_DX:) (const)
- [EFW_A64_FMOV_XD:](#EFW_A64_FMOV_XD:) (const)
- [EFW_A64_FMOV_SW:](#EFW_A64_FMOV_SW:) (const)
- [EFW_A64_FMOV_WS:](#EFW_A64_FMOV_WS:) (const)
- [EFW_A64_FCVT_DS:](#EFW_A64_FCVT_DS:) (const)
- [EFW_A64_FCVT_SD:](#EFW_A64_FCVT_SD:) (const)
- [EFW_A64_SCVTF:](#EFW_A64_SCVTF:) (const)
- [EFW_A64_FCVTZS:](#EFW_A64_FCVTZS:) (const)
- [EFW_A64_FSQRT:](#EFW_A64_FSQRT:) (const)
- [EFW_A64_FABS:](#EFW_A64_FABS:) (const)
- [EFW_A64_FRINTM:](#EFW_A64_FRINTM:) (const)
- [EFW_A64_FRINTP:](#EFW_A64_FRINTP:) (const)
- [EFW_A64_FRINTZ:](#EFW_A64_FRINTZ:) (const)
- [EFW_A64_FRINTA:](#EFW_A64_FRINTA:) (const)
- [EFW_A64_FMOV_DD:](#EFW_A64_FMOV_DD:) (const)
- [EFW_A64_FADD:](#EFW_A64_FADD:) (const)
- [EFW_A64_FSUB:](#EFW_A64_FSUB:) (const)
- [EFW_A64_FMUL:](#EFW_A64_FMUL:) (const)
- [EFW_A64_FDIV:](#EFW_A64_FDIV:) (const)
- [EFW_A64_FCMP:](#EFW_A64_FCMP:) (const)
- [EFW_A64_FMADD:](#EFW_A64_FMADD:) (const)
- [EFW_RV_ADD:](#EFW_RV_ADD:) (const)
- [EFW_RV_SUB:](#EFW_RV_SUB:) (const)
- [EFW_RV_SLL:](#EFW_RV_SLL:) (const)
- [EFW_RV_SLT:](#EFW_RV_SLT:) (const)
- [EFW_RV_SLTU:](#EFW_RV_SLTU:) (const)
- [EFW_RV_XOR:](#EFW_RV_XOR:) (const)
- [EFW_RV_SRL:](#EFW_RV_SRL:) (const)
- [EFW_RV_SRA:](#EFW_RV_SRA:) (const)
- [EFW_RV_OR:](#EFW_RV_OR:) (const)
- [EFW_RV_AND:](#EFW_RV_AND:) (const)
- [EFW_RV_MUL:](#EFW_RV_MUL:) (const)
- [EFW_RV_MULH:](#EFW_RV_MULH:) (const)
- [EFW_RV_MULHU:](#EFW_RV_MULHU:) (const)
- [EFW_RV_DIV:](#EFW_RV_DIV:) (const)
- [EFW_RV_DIVU:](#EFW_RV_DIVU:) (const)
- [EFW_RV_REM:](#EFW_RV_REM:) (const)
- [EFW_RV_REMU:](#EFW_RV_REMU:) (const)
- [EFW_RV_ADDI:](#EFW_RV_ADDI:) (const)
- [EFW_RV_SLTIU:](#EFW_RV_SLTIU:) (const)
- [EFW_RV_XORI:](#EFW_RV_XORI:) (const)
- [EFW_RV_ANDI:](#EFW_RV_ANDI:) (const)
- [EFW_RV_SLLI:](#EFW_RV_SLLI:) (const)
- [EFW_RV_SRLI:](#EFW_RV_SRLI:) (const)
- [EFW_RV_SRAI:](#EFW_RV_SRAI:) (const)
- [EFW_RV_ADDIW:](#EFW_RV_ADDIW:) (const)
- [EFW_RV_ADDW:](#EFW_RV_ADDW:) (const)
- [EFW_RV_SUBW:](#EFW_RV_SUBW:) (const)
- [EFW_RV_MULW:](#EFW_RV_MULW:) (const)
- [EFW_RV_SH1ADD:](#EFW_RV_SH1ADD:) (const)
- [EFW_RV_ZEXT_W:](#EFW_RV_ZEXT_W:) (const)
- [EFW_RV_ZEXT_H:](#EFW_RV_ZEXT_H:) (const)
- [EFW_RV_SEXT_B:](#EFW_RV_SEXT_B:) (const)
- [EFW_RV_SEXT_H:](#EFW_RV_SEXT_H:) (const)
- [EFW_RV_ANDN:](#EFW_RV_ANDN:) (const)
- [EFW_RV_CZERO_EQZ:](#EFW_RV_CZERO_EQZ:) (const)
- [EFW_RV_CZERO_NEZ:](#EFW_RV_CZERO_NEZ:) (const)
- [EFW_RV_LUI:](#EFW_RV_LUI:) (const)
- [EFW_RV_AUIPC:](#EFW_RV_AUIPC:) (const)
- [EFW_RV_JAL:](#EFW_RV_JAL:) (const)
- [EFW_RV_JALR:](#EFW_RV_JALR:) (const)
- [EFW_RV_ECALL:](#EFW_RV_ECALL:) (const)
- [EFW_RV_EBREAK:](#EFW_RV_EBREAK:) (const)
- [EFW_RV_FENCE_RW:](#EFW_RV_FENCE_RW:) (const)
- [EFW_RV_LD:](#EFW_RV_LD:) (const)
- [EFW_RV_SD:](#EFW_RV_SD:) (const)
- [EFW_RV_FLD:](#EFW_RV_FLD:) (const)
- [EFW_RV_FSD:](#EFW_RV_FSD:) (const)
- [EFW_RV_BEQ:](#EFW_RV_BEQ:) (const)
- [EFW_RV_FMV_DX:](#EFW_RV_FMV_DX:) (const)
- [EFW_RV_FMV_XD:](#EFW_RV_FMV_XD:) (const)
- [EFW_RV_FMV_WX:](#EFW_RV_FMV_WX:) (const)
- [EFW_RV_FMV_XW:](#EFW_RV_FMV_XW:) (const)
- [EFW_RV_FADD_D:](#EFW_RV_FADD_D:) (const)
- [EFW_RV_FSUB_D:](#EFW_RV_FSUB_D:) (const)
- [EFW_RV_FMUL_D:](#EFW_RV_FMUL_D:) (const)
- [EFW_RV_FDIV_D:](#EFW_RV_FDIV_D:) (const)
- [EFW_RV_FSQRT_D:](#EFW_RV_FSQRT_D:) (const)
- [EFW_RV_FSGNJ_D:](#EFW_RV_FSGNJ_D:) (const)
- [EFW_RV_FSGNJN_D:](#EFW_RV_FSGNJN_D:) (const)
- [EFW_RV_FSGNJX_D:](#EFW_RV_FSGNJX_D:) (const)
- [EFW_RV_FLE_D:](#EFW_RV_FLE_D:) (const)
- [EFW_RV_FLT_D:](#EFW_RV_FLT_D:) (const)
- [EFW_RV_FEQ_D:](#EFW_RV_FEQ_D:) (const)
- [EFW_RV_FCVT_D_L:](#EFW_RV_FCVT_D_L:) (const)
- [EFW_RV_FCVT_D_W:](#EFW_RV_FCVT_D_W:) (const)
- [EFW_RV_FCVT_L_D:](#EFW_RV_FCVT_L_D:) (const)
- [EFW_RV_FCVT_W_D:](#EFW_RV_FCVT_W_D:) (const)
- [EFW_RV_FCVT_D_S:](#EFW_RV_FCVT_D_S:) (const)
- [EFW_RV_FCVT_S_D:](#EFW_RV_FCVT_S_D:) (const)
- [EFW_RV_FMADD_D:](#EFW_RV_FMADD_D:) (const)
- [EFW_RV_LR_D:](#EFW_RV_LR_D:) (const)
- [EFW_RV_SC_D:](#EFW_RV_SC_D:) (const)
- [EFW_RV_AMOADD_D:](#EFW_RV_AMOADD_D:) (const)
- [EFW_RV_AMOSWAP_D:](#EFW_RV_AMOSWAP_D:) (const)
- [ef_init](#ef_init) (function)
- [ef_id](#ef_id) (function)
- [ef_count](#ef_count) (function)
- [ef_arch](#ef_arch) (function)
- [ef_template](#ef_template) (function)
- [ef_base](#ef_base) (function)
- [ef_mask](#ef_mask) (function)
- [ef_word](#ef_word) (function)
- [ef_decode_word](#ef_decode_word) (function)
- [ef_x64_rr](#ef_x64_rr) (function)
- [ef_check](#ef_check) (function)
- [dsm_a64](#dsm_a64) (function)
- [dsm_rv](#dsm_rv) (function)
- [dsm_x64](#dsm_x64) (function)
- [disasm_bytes](#disasm_bytes) (function)
- [disasm_image](#disasm_image) (function)

## DSM_A64:

```fides
pub const DSM_A64: i64 = 1;
```

*Defined at line 32*

---

## DSM_RV64:

```fides
pub const DSM_RV64: i64 = 2;
```

*Defined at line 33*

---

## DSM_X64:

```fides
pub const DSM_X64: i64 = 3;
```

*Defined at line 34*

---

## DsmInsn

An instruction: its length in bytes and its text.

```fides
pub struct DsmInsn {
```

**Fields:**

| Name | Type |
|------|------|
| `len` | `i64` |
| `text` | `str` |

*Defined at line 37*

---

## disasm_arch_of

The architecture of a --arch name, 0 when it is not one.

```fides
pub fn disasm_arch_of(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 49*

---

## DsmModrm

A ModRM operand: `len` bytes (ModRM, SIB, displacement), the register
field, the r/m register when mod is 3 (else -1) and the memory text
([base + index*scale + disp]); `rip`: rip-relative with `disp`.

```fides
pub struct DsmModrm {
```

**Fields:**

| Name | Type |
|------|------|
| `len` | `i64` |
| `reg` | `i64` |
| `rm_reg` | `i64` |
| `mem` | `str` |
| `rip` | `bool` |
| `disp` | `i64` |

*Defined at line 221*

---

## EFW_A64_ADD:

```fides
pub const EFW_A64_ADD: i64 = 0x8B000000;
```

*Defined at line 353*

---

## EFW_A64_ADDS:

```fides
pub const EFW_A64_ADDS: i64 = 0xAB000000;
```

*Defined at line 354*

---

## EFW_A64_SUB:

```fides
pub const EFW_A64_SUB: i64 = 0xCB000000;
```

*Defined at line 355*

---

## EFW_A64_SUBS:

```fides
pub const EFW_A64_SUBS: i64 = 0xEB000000;
```

*Defined at line 356*

---

## EFW_A64_AND:

```fides
pub const EFW_A64_AND: i64 = 0x8A000000;
```

*Defined at line 357*

---

## EFW_A64_ORR:

```fides
pub const EFW_A64_ORR: i64 = 0xAA000000;
```

*Defined at line 358*

---

## EFW_A64_EOR:

```fides
pub const EFW_A64_EOR: i64 = 0xCA000000;
```

*Defined at line 359*

---

## EFW_A64_ANDS:

```fides
pub const EFW_A64_ANDS: i64 = 0xEA000000;
```

*Defined at line 360*

---

## EFW_A64_BIC:

```fides
pub const EFW_A64_BIC: i64 = 0x8A200000;
```

*Defined at line 361*

---

## EFW_A64_ORN:

```fides
pub const EFW_A64_ORN: i64 = 0xAA200000;
```

*Defined at line 362*

---

## EFW_A64_EON:

```fides
pub const EFW_A64_EON: i64 = 0xCA200000;
```

*Defined at line 363*

---

## EFW_A64_BICS:

```fides
pub const EFW_A64_BICS: i64 = 0xEA200000;
```

*Defined at line 364*

---

## EFW_A64_ADD_IMM:

```fides
pub const EFW_A64_ADD_IMM: i64 = 0x91000000;
```

*Defined at line 365*

---

## EFW_A64_ADDS_IMM:

```fides
pub const EFW_A64_ADDS_IMM: i64 = 0xB1000000;
```

*Defined at line 366*

---

## EFW_A64_SUB_IMM:

```fides
pub const EFW_A64_SUB_IMM: i64 = 0xD1000000;
```

*Defined at line 367*

---

## EFW_A64_SUBS_IMM:

```fides
pub const EFW_A64_SUBS_IMM: i64 = 0xF1000000;
```

*Defined at line 368*

---

## EFW_A64_ADD_EXT:

```fides
pub const EFW_A64_ADD_EXT: i64 = 0x8B206000;
```

*Defined at line 369*

---

## EFW_A64_SUB_EXT:

```fides
pub const EFW_A64_SUB_EXT: i64 = 0xCB206000;
```

*Defined at line 370*

---

## EFW_A64_MADD:

```fides
pub const EFW_A64_MADD: i64 = 0x9B000000;
```

*Defined at line 371*

---

## EFW_A64_MSUB:

```fides
pub const EFW_A64_MSUB: i64 = 0x9B008000;
```

*Defined at line 372*

---

## EFW_A64_SMULH:

```fides
pub const EFW_A64_SMULH: i64 = 0x9B407C00;
```

*Defined at line 373*

---

## EFW_A64_UMULH:

```fides
pub const EFW_A64_UMULH: i64 = 0x9BC07C00;
```

*Defined at line 374*

---

## EFW_A64_UDIV:

```fides
pub const EFW_A64_UDIV: i64 = 0x9AC00800;
```

*Defined at line 375*

---

## EFW_A64_SDIV:

```fides
pub const EFW_A64_SDIV: i64 = 0x9AC00C00;
```

*Defined at line 376*

---

## EFW_A64_LSLV:

```fides
pub const EFW_A64_LSLV: i64 = 0x9AC02000;
```

*Defined at line 377*

---

## EFW_A64_LSRV:

```fides
pub const EFW_A64_LSRV: i64 = 0x9AC02400;
```

*Defined at line 378*

---

## EFW_A64_ASRV:

```fides
pub const EFW_A64_ASRV: i64 = 0x9AC02800;
```

*Defined at line 379*

---

## EFW_A64_RORV:

```fides
pub const EFW_A64_RORV: i64 = 0x9AC02C00;
```

*Defined at line 380*

---

## EFW_A64_CSEL:

```fides
pub const EFW_A64_CSEL: i64 = 0x9A800000;
```

*Defined at line 381*

---

## EFW_A64_CSINC:

```fides
pub const EFW_A64_CSINC: i64 = 0x9A800400;
```

*Defined at line 382*

---

## EFW_A64_SBFM:

```fides
pub const EFW_A64_SBFM: i64 = 0x93400000;
```

*Defined at line 383*

---

## EFW_A64_UBFM:

```fides
pub const EFW_A64_UBFM: i64 = 0xD3400000;
```

*Defined at line 384*

---

## EFW_A64_MOVN:

```fides
pub const EFW_A64_MOVN: i64 = 0x92800000;
```

*Defined at line 385*

---

## EFW_A64_MOVZ:

```fides
pub const EFW_A64_MOVZ: i64 = 0xD2800000;
```

*Defined at line 386*

---

## EFW_A64_MOVK:

```fides
pub const EFW_A64_MOVK: i64 = 0xF2800000;
```

*Defined at line 387*

---

## EFW_A64_B:

```fides
pub const EFW_A64_B: i64 = 0x14000000;
```

*Defined at line 388*

---

## EFW_A64_BL:

```fides
pub const EFW_A64_BL: i64 = 0x94000000;
```

*Defined at line 389*

---

## EFW_A64_BCOND:

```fides
pub const EFW_A64_BCOND: i64 = 0x54000000;
```

*Defined at line 390*

---

## EFW_A64_CBZ_W:

```fides
pub const EFW_A64_CBZ_W: i64 = 0x34000000;
```

*Defined at line 391*

---

## EFW_A64_CBNZ_W:

```fides
pub const EFW_A64_CBNZ_W: i64 = 0x35000000;
```

*Defined at line 392*

---

## EFW_A64_CBZ:

```fides
pub const EFW_A64_CBZ: i64 = 0xB4000000;
```

*Defined at line 393*

---

## EFW_A64_CBNZ:

```fides
pub const EFW_A64_CBNZ: i64 = 0xB5000000;
```

*Defined at line 394*

---

## EFW_A64_TBZ:

```fides
pub const EFW_A64_TBZ: i64 = 0x36000000;
```

*Defined at line 395*

---

## EFW_A64_TBNZ:

```fides
pub const EFW_A64_TBNZ: i64 = 0x37000000;
```

*Defined at line 396*

---

## EFW_A64_ADRP:

```fides
pub const EFW_A64_ADRP: i64 = 0x90000000;
```

*Defined at line 397*

---

## EFW_A64_ADR:

```fides
pub const EFW_A64_ADR: i64 = 0x10000000;
```

*Defined at line 398*

---

## EFW_A64_BLR:

```fides
pub const EFW_A64_BLR: i64 = 0xD63F0000;
```

*Defined at line 399*

---

## EFW_A64_BR:

```fides
pub const EFW_A64_BR: i64 = 0xD61F0000;
```

*Defined at line 400*

---

## EFW_A64_RET:

```fides
pub const EFW_A64_RET: i64 = 0xD65F03C0;
```

*Defined at line 401*

---

## EFW_A64_NOP:

```fides
pub const EFW_A64_NOP: i64 = 0xD503201F;
```

*Defined at line 402*

---

## EFW_A64_SVC0:

```fides
pub const EFW_A64_SVC0: i64 = 0xD4000001;
```

*Defined at line 403*

---

## EFW_A64_BRK:

```fides
pub const EFW_A64_BRK: i64 = 0xD4200000;
```

*Defined at line 404*

---

## EFW_A64_DMB_ISH:

```fides
pub const EFW_A64_DMB_ISH: i64 = 0xD5033BBF;
```

*Defined at line 405*

---

## EFW_A64_CLREX:

```fides
pub const EFW_A64_CLREX: i64 = 0xD5033F5F;
```

*Defined at line 406*

---

## EFW_A64_MRS_TPIDR:

```fides
pub const EFW_A64_MRS_TPIDR: i64 = 0xD53BD040;
```

*Defined at line 407*

---

## EFW_A64_MSR_TPIDR:

```fides
pub const EFW_A64_MSR_TPIDR: i64 = 0xD51BD040;
```

*Defined at line 408*

---

## EFW_A64_STRB:

```fides
pub const EFW_A64_STRB: i64 = 0x39000000;
```

*Defined at line 409*

---

## EFW_A64_LDRB:

```fides
pub const EFW_A64_LDRB: i64 = 0x39400000;
```

*Defined at line 410*

---

## EFW_A64_LDRSB:

```fides
pub const EFW_A64_LDRSB: i64 = 0x39800000;
```

*Defined at line 411*

---

## EFW_A64_STRH:

```fides
pub const EFW_A64_STRH: i64 = 0x79000000;
```

*Defined at line 412*

---

## EFW_A64_LDRH:

```fides
pub const EFW_A64_LDRH: i64 = 0x79400000;
```

*Defined at line 413*

---

## EFW_A64_LDRSH:

```fides
pub const EFW_A64_LDRSH: i64 = 0x79800000;
```

*Defined at line 414*

---

## EFW_A64_STR_W:

```fides
pub const EFW_A64_STR_W: i64 = 0xB9000000;
```

*Defined at line 415*

---

## EFW_A64_LDR_W:

```fides
pub const EFW_A64_LDR_W: i64 = 0xB9400000;
```

*Defined at line 416*

---

## EFW_A64_LDRSW:

```fides
pub const EFW_A64_LDRSW: i64 = 0xB9800000;
```

*Defined at line 417*

---

## EFW_A64_STR:

```fides
pub const EFW_A64_STR: i64 = 0xF9000000;
```

*Defined at line 418*

---

## EFW_A64_LDR:

```fides
pub const EFW_A64_LDR: i64 = 0xF9400000;
```

*Defined at line 419*

---

## EFW_A64_STR_Q:

```fides
pub const EFW_A64_STR_Q: i64 = 0x3D800000;
```

*Defined at line 420*

---

## EFW_A64_LDR_Q:

```fides
pub const EFW_A64_LDR_Q: i64 = 0x3DC00000;
```

*Defined at line 421*

---

## EFW_A64_STP_POST:

```fides
pub const EFW_A64_STP_POST: i64 = 0xA8800000;
```

*Defined at line 422*

---

## EFW_A64_LDP_POST:

```fides
pub const EFW_A64_LDP_POST: i64 = 0xA8C00000;
```

*Defined at line 423*

---

## EFW_A64_STP:

```fides
pub const EFW_A64_STP: i64 = 0xA9000000;
```

*Defined at line 424*

---

## EFW_A64_LDP:

```fides
pub const EFW_A64_LDP: i64 = 0xA9400000;
```

*Defined at line 425*

---

## EFW_A64_STP_PRE:

```fides
pub const EFW_A64_STP_PRE: i64 = 0xA9800000;
```

*Defined at line 426*

---

## EFW_A64_LDP_PRE:

```fides
pub const EFW_A64_LDP_PRE: i64 = 0xA9C00000;
```

*Defined at line 427*

---

## EFW_A64_LDAXR:

```fides
pub const EFW_A64_LDAXR: i64 = 0xC85FFC00;
```

*Defined at line 428*

---

## EFW_A64_STLXR:

```fides
pub const EFW_A64_STLXR: i64 = 0xC800FC00;
```

*Defined at line 429*

---

## EFW_A64_LDAR:

```fides
pub const EFW_A64_LDAR: i64 = 0xC8DFFC00;
```

*Defined at line 430*

---

## EFW_A64_STLR:

```fides
pub const EFW_A64_STLR: i64 = 0xC89FFC00;
```

*Defined at line 431*

---

## EFW_A64_FMOV_DX:

```fides
pub const EFW_A64_FMOV_DX: i64 = 0x9E670000;
```

*Defined at line 432*

---

## EFW_A64_FMOV_XD:

```fides
pub const EFW_A64_FMOV_XD: i64 = 0x9E660000;
```

*Defined at line 433*

---

## EFW_A64_FMOV_SW:

```fides
pub const EFW_A64_FMOV_SW: i64 = 0x1E270000;
```

*Defined at line 434*

---

## EFW_A64_FMOV_WS:

```fides
pub const EFW_A64_FMOV_WS: i64 = 0x1E260000;
```

*Defined at line 435*

---

## EFW_A64_FCVT_DS:

```fides
pub const EFW_A64_FCVT_DS: i64 = 0x1E22C000;
```

*Defined at line 436*

---

## EFW_A64_FCVT_SD:

```fides
pub const EFW_A64_FCVT_SD: i64 = 0x1E624000;
```

*Defined at line 437*

---

## EFW_A64_SCVTF:

```fides
pub const EFW_A64_SCVTF: i64 = 0x9E620000;
```

*Defined at line 438*

---

## EFW_A64_FCVTZS:

```fides
pub const EFW_A64_FCVTZS: i64 = 0x9E780000;
```

*Defined at line 439*

---

## EFW_A64_FSQRT:

```fides
pub const EFW_A64_FSQRT: i64 = 0x1E61C000;
```

*Defined at line 440*

---

## EFW_A64_FABS:

```fides
pub const EFW_A64_FABS: i64 = 0x1E60C000;
```

*Defined at line 441*

---

## EFW_A64_FRINTM:

```fides
pub const EFW_A64_FRINTM: i64 = 0x1E654000;
```

*Defined at line 442*

---

## EFW_A64_FRINTP:

```fides
pub const EFW_A64_FRINTP: i64 = 0x1E64C000;
```

*Defined at line 443*

---

## EFW_A64_FRINTZ:

```fides
pub const EFW_A64_FRINTZ: i64 = 0x1E65C000;
```

*Defined at line 444*

---

## EFW_A64_FRINTA:

```fides
pub const EFW_A64_FRINTA: i64 = 0x1E664000;
```

*Defined at line 445*

---

## EFW_A64_FMOV_DD:

```fides
pub const EFW_A64_FMOV_DD: i64 = 0x1E604000;
```

*Defined at line 446*

---

## EFW_A64_FADD:

```fides
pub const EFW_A64_FADD: i64 = 0x1E602800;
```

*Defined at line 447*

---

## EFW_A64_FSUB:

```fides
pub const EFW_A64_FSUB: i64 = 0x1E603800;
```

*Defined at line 448*

---

## EFW_A64_FMUL:

```fides
pub const EFW_A64_FMUL: i64 = 0x1E600800;
```

*Defined at line 449*

---

## EFW_A64_FDIV:

```fides
pub const EFW_A64_FDIV: i64 = 0x1E601800;
```

*Defined at line 450*

---

## EFW_A64_FCMP:

```fides
pub const EFW_A64_FCMP: i64 = 0x1E602000;
```

*Defined at line 451*

---

## EFW_A64_FMADD:

```fides
pub const EFW_A64_FMADD: i64 = 0x1F400000;
```

*Defined at line 452*

---

## EFW_RV_ADD:

```fides
pub const EFW_RV_ADD: i64 = 0x00000033;
```

*Defined at line 455*

---

## EFW_RV_SUB:

```fides
pub const EFW_RV_SUB: i64 = 0x40000033;
```

*Defined at line 456*

---

## EFW_RV_SLL:

```fides
pub const EFW_RV_SLL: i64 = 0x00001033;
```

*Defined at line 457*

---

## EFW_RV_SLT:

```fides
pub const EFW_RV_SLT: i64 = 0x00002033;
```

*Defined at line 458*

---

## EFW_RV_SLTU:

```fides
pub const EFW_RV_SLTU: i64 = 0x00003033;
```

*Defined at line 459*

---

## EFW_RV_XOR:

```fides
pub const EFW_RV_XOR: i64 = 0x00004033;
```

*Defined at line 460*

---

## EFW_RV_SRL:

```fides
pub const EFW_RV_SRL: i64 = 0x00005033;
```

*Defined at line 461*

---

## EFW_RV_SRA:

```fides
pub const EFW_RV_SRA: i64 = 0x40005033;
```

*Defined at line 462*

---

## EFW_RV_OR:

```fides
pub const EFW_RV_OR: i64 = 0x00006033;
```

*Defined at line 463*

---

## EFW_RV_AND:

```fides
pub const EFW_RV_AND: i64 = 0x00007033;
```

*Defined at line 464*

---

## EFW_RV_MUL:

```fides
pub const EFW_RV_MUL: i64 = 0x02000033;
```

*Defined at line 465*

---

## EFW_RV_MULH:

```fides
pub const EFW_RV_MULH: i64 = 0x02001033;
```

*Defined at line 466*

---

## EFW_RV_MULHU:

```fides
pub const EFW_RV_MULHU: i64 = 0x02003033;
```

*Defined at line 467*

---

## EFW_RV_DIV:

```fides
pub const EFW_RV_DIV: i64 = 0x02004033;
```

*Defined at line 468*

---

## EFW_RV_DIVU:

```fides
pub const EFW_RV_DIVU: i64 = 0x02005033;
```

*Defined at line 469*

---

## EFW_RV_REM:

```fides
pub const EFW_RV_REM: i64 = 0x02006033;
```

*Defined at line 470*

---

## EFW_RV_REMU:

```fides
pub const EFW_RV_REMU: i64 = 0x02007033;
```

*Defined at line 471*

---

## EFW_RV_ADDI:

```fides
pub const EFW_RV_ADDI: i64 = 0x00000013;
```

*Defined at line 472*

---

## EFW_RV_SLTIU:

```fides
pub const EFW_RV_SLTIU: i64 = 0x00003013;
```

*Defined at line 473*

---

## EFW_RV_XORI:

```fides
pub const EFW_RV_XORI: i64 = 0x00004013;
```

*Defined at line 474*

---

## EFW_RV_ANDI:

```fides
pub const EFW_RV_ANDI: i64 = 0x00007013;
```

*Defined at line 475*

---

## EFW_RV_SLLI:

```fides
pub const EFW_RV_SLLI: i64 = 0x00001013;
```

*Defined at line 476*

---

## EFW_RV_SRLI:

```fides
pub const EFW_RV_SRLI: i64 = 0x00005013;
```

*Defined at line 477*

---

## EFW_RV_SRAI:

```fides
pub const EFW_RV_SRAI: i64 = 0x40005013;
```

*Defined at line 478*

---

## EFW_RV_ADDIW:

```fides
pub const EFW_RV_ADDIW: i64 = 0x0000001B;
```

*Defined at line 479*

---

## EFW_RV_ADDW:

```fides
pub const EFW_RV_ADDW: i64 = 0x0000003B;
```

*Defined at line 480*

---

## EFW_RV_SUBW:

```fides
pub const EFW_RV_SUBW: i64 = 0x4000003B;
```

*Defined at line 481*

---

## EFW_RV_MULW:

```fides
pub const EFW_RV_MULW: i64 = 0x0200003B;
```

*Defined at line 482*

---

## EFW_RV_SH1ADD:

```fides
pub const EFW_RV_SH1ADD: i64 = 0x20002033;
```

*Defined at line 483*

---

## EFW_RV_ZEXT_W:

```fides
pub const EFW_RV_ZEXT_W: i64 = 0x0800003B;
```

*Defined at line 484*

---

## EFW_RV_ZEXT_H:

```fides
pub const EFW_RV_ZEXT_H: i64 = 0x0800403B;
```

*Defined at line 485*

---

## EFW_RV_SEXT_B:

```fides
pub const EFW_RV_SEXT_B: i64 = 0x60401013;
```

*Defined at line 486*

---

## EFW_RV_SEXT_H:

```fides
pub const EFW_RV_SEXT_H: i64 = 0x60501013;
```

*Defined at line 487*

---

## EFW_RV_ANDN:

```fides
pub const EFW_RV_ANDN: i64 = 0x40007033;
```

*Defined at line 488*

---

## EFW_RV_CZERO_EQZ:

```fides
pub const EFW_RV_CZERO_EQZ: i64 = 0x0E005033;
```

*Defined at line 489*

---

## EFW_RV_CZERO_NEZ:

```fides
pub const EFW_RV_CZERO_NEZ: i64 = 0x0E007033;
```

*Defined at line 490*

---

## EFW_RV_LUI:

```fides
pub const EFW_RV_LUI: i64 = 0x00000037;
```

*Defined at line 491*

---

## EFW_RV_AUIPC:

```fides
pub const EFW_RV_AUIPC: i64 = 0x00000017;
```

*Defined at line 492*

---

## EFW_RV_JAL:

```fides
pub const EFW_RV_JAL: i64 = 0x0000006F;
```

*Defined at line 493*

---

## EFW_RV_JALR:

```fides
pub const EFW_RV_JALR: i64 = 0x00000067;
```

*Defined at line 494*

---

## EFW_RV_ECALL:

```fides
pub const EFW_RV_ECALL: i64 = 0x00000073;
```

*Defined at line 495*

---

## EFW_RV_EBREAK:

```fides
pub const EFW_RV_EBREAK: i64 = 0x00100073;
```

*Defined at line 496*

---

## EFW_RV_FENCE_RW:

```fides
pub const EFW_RV_FENCE_RW: i64 = 0x0330000F;
```

*Defined at line 497*

---

## EFW_RV_LD:

```fides
pub const EFW_RV_LD: i64 = 0x00003003;
```

*Defined at line 498*

---

## EFW_RV_SD:

```fides
pub const EFW_RV_SD: i64 = 0x00003023;
```

*Defined at line 499*

---

## EFW_RV_FLD:

```fides
pub const EFW_RV_FLD: i64 = 0x00003007;
```

*Defined at line 500*

---

## EFW_RV_FSD:

```fides
pub const EFW_RV_FSD: i64 = 0x00003027;
```

*Defined at line 501*

---

## EFW_RV_BEQ:

```fides
pub const EFW_RV_BEQ: i64 = 0x00000063;
```

*Defined at line 502*

---

## EFW_RV_FMV_DX:

```fides
pub const EFW_RV_FMV_DX: i64 = 0xF2000053;
```

*Defined at line 503*

---

## EFW_RV_FMV_XD:

```fides
pub const EFW_RV_FMV_XD: i64 = 0xE2000053;
```

*Defined at line 504*

---

## EFW_RV_FMV_WX:

```fides
pub const EFW_RV_FMV_WX: i64 = 0xF0000053;
```

*Defined at line 505*

---

## EFW_RV_FMV_XW:

```fides
pub const EFW_RV_FMV_XW: i64 = 0xE0000053;
```

*Defined at line 506*

---

## EFW_RV_FADD_D:

```fides
pub const EFW_RV_FADD_D: i64 = 0x02000053;
```

*Defined at line 507*

---

## EFW_RV_FSUB_D:

```fides
pub const EFW_RV_FSUB_D: i64 = 0x0A000053;
```

*Defined at line 508*

---

## EFW_RV_FMUL_D:

```fides
pub const EFW_RV_FMUL_D: i64 = 0x12000053;
```

*Defined at line 509*

---

## EFW_RV_FDIV_D:

```fides
pub const EFW_RV_FDIV_D: i64 = 0x1A000053;
```

*Defined at line 510*

---

## EFW_RV_FSQRT_D:

```fides
pub const EFW_RV_FSQRT_D: i64 = 0x5A000053;
```

*Defined at line 511*

---

## EFW_RV_FSGNJ_D:

```fides
pub const EFW_RV_FSGNJ_D: i64 = 0x22000053;
```

*Defined at line 512*

---

## EFW_RV_FSGNJN_D:

```fides
pub const EFW_RV_FSGNJN_D: i64 = 0x22001053;
```

*Defined at line 513*

---

## EFW_RV_FSGNJX_D:

```fides
pub const EFW_RV_FSGNJX_D: i64 = 0x22002053;
```

*Defined at line 514*

---

## EFW_RV_FLE_D:

```fides
pub const EFW_RV_FLE_D: i64 = 0xA2000053;
```

*Defined at line 515*

---

## EFW_RV_FLT_D:

```fides
pub const EFW_RV_FLT_D: i64 = 0xA2001053;
```

*Defined at line 516*

---

## EFW_RV_FEQ_D:

```fides
pub const EFW_RV_FEQ_D: i64 = 0xA2002053;
```

*Defined at line 517*

---

## EFW_RV_FCVT_D_L:

```fides
pub const EFW_RV_FCVT_D_L: i64 = 0xD2200053;
```

*Defined at line 518*

---

## EFW_RV_FCVT_D_W:

```fides
pub const EFW_RV_FCVT_D_W: i64 = 0xD2000053;
```

*Defined at line 519*

---

## EFW_RV_FCVT_L_D:

```fides
pub const EFW_RV_FCVT_L_D: i64 = 0xC2200053;
```

*Defined at line 520*

---

## EFW_RV_FCVT_W_D:

```fides
pub const EFW_RV_FCVT_W_D: i64 = 0xC2000053;
```

*Defined at line 521*

---

## EFW_RV_FCVT_D_S:

```fides
pub const EFW_RV_FCVT_D_S: i64 = 0x42000053;
```

*Defined at line 522*

---

## EFW_RV_FCVT_S_D:

```fides
pub const EFW_RV_FCVT_S_D: i64 = 0x40100053;
```

*Defined at line 523*

---

## EFW_RV_FMADD_D:

```fides
pub const EFW_RV_FMADD_D: i64 = 0x02000043;
```

*Defined at line 524*

---

## EFW_RV_LR_D:

```fides
pub const EFW_RV_LR_D: i64 = 0x1000302F;
```

*Defined at line 525*

---

## EFW_RV_SC_D:

```fides
pub const EFW_RV_SC_D: i64 = 0x1800302F;
```

*Defined at line 526*

---

## EFW_RV_AMOADD_D:

```fides
pub const EFW_RV_AMOADD_D: i64 = 0x0000302F;
```

*Defined at line 527*

---

## EFW_RV_AMOSWAP_D:

```fides
pub const EFW_RV_AMOSWAP_D: i64 = 0x0800302F;
```

*Defined at line 528*

---

## ef_init

Build the table once.

```fides
pub fn ef_init() {
```

*Defined at line 751*

---

## ef_id

The id of the row whose template is `tmpl` (first in table order), -1
when none. For tools and tests; the encoders use the EFW_* words.

```fides
pub fn ef_id(arch: i64, tmpl: str) -> i64 {
```

**Parameters:**

- `arch`: `i64`
- `tmpl`: `str`

**Returns:** `i64`

*Defined at line 1301*

---

## ef_count

```fides
pub fn ef_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 1313*

---

## ef_arch

```fides
pub fn ef_arch(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 1318*

---

## ef_template

```fides
pub fn ef_template(id: i64) -> str {
```

**Parameters:**

- `id`: `i64`

**Returns:** `str`

*Defined at line 1323*

---

## ef_base

```fides
pub fn ef_base(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 1328*

---

## ef_mask

```fides
pub fn ef_mask(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 1333*

---

## ef_word

```fides
pub fn ef_word(id: i64, a: i64, b: i64, c: i64, d: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 1530*

---

## ef_decode_word

A 32-bit word through the table: its text, "" when no row has it.

```fides
pub fn ef_decode_word(arch: i64, w: i64, pc: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `w`: `i64`
- `pc`: `i64`

**Returns:** `str`

*Defined at line 1877*

---

## ef_x64_rr

x86-64: the bytes of row `id` between registers (ModRM mod 3): reg in
ModRM.reg (the row's digit when it has one), rm in ModRM.rm (for a
VEX row, vvvv = v and `l256` VEX.L); `w` sets REX.W / VEX.W. A +r row
takes the register in rm. Immediates are not included. The encoder
side of the x86-64 rows; the two-byte VEX (C5) when it can say it, as
assemblers (and nat_enc.fd x64_vex) pick.

```fides
pub fn ef_x64_rr(id: i64, reg: i64, rm: i64, v: i64, w: bool, l256: bool) -> Vec<i64> {
```

**Parameters:**

- `id`: `i64`
- `reg`: `i64`
- `rm`: `i64`
- `v`: `i64`
- `w`: `bool`
- `l256`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1891*

---

## ef_check

```fides
pub fn ef_check() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 2010*

---

## dsm_a64

```fides
pub fn dsm_a64(w: i64, pc: i64) -> str {
```

**Parameters:**

- `w`: `i64`
- `pc`: `i64`

**Returns:** `str`

*Defined at line 2083*

---

## dsm_rv

```fides
pub fn dsm_rv(w: i64, pc: i64) -> str {
```

**Parameters:**

- `w`: `i64`
- `pc`: `i64`

**Returns:** `str`

*Defined at line 2094*

---

## dsm_x64

```fides
pub fn dsm_x64(b: &Vec<i64>, at: i64, addr: i64) -> DsmInsn {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`
- `addr`: `i64`

**Returns:** `DsmInsn`

*Defined at line 2291*

---

## disasm_bytes

```fides
pub fn disasm_bytes(b: &Vec<i64>, from: i64, to: i64, arch: i64, addr: i64) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`
- `arch`: `i64`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 2510*

---

## disasm_image

An executable's code by function: an ELF (x86-64, AArch64, RISC-V 64)
or a PE from the machine layer; `name` "" for every function.

```fides
pub fn disasm_image(bytes: Vec<i64>, name: str) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `name`: `str`

**Returns:** `str`

*Defined at line 2737*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
