[All modules](index.md)

# compiler::mir -- Fides Documentation

## Contents

- [NCC_EQ:](#NCC_EQ:) (const)
- [NCC_NE:](#NCC_NE:) (const)
- [NCC_LT:](#NCC_LT:) (const)
- [NCC_LE:](#NCC_LE:) (const)
- [NCC_GT:](#NCC_GT:) (const)
- [NCC_GE:](#NCC_GE:) (const)
- [NCC_LTU:](#NCC_LTU:) (const)
- [NCC_LEU:](#NCC_LEU:) (const)
- [NCC_GTU:](#NCC_GTU:) (const)
- [NCC_GEU:](#NCC_GEU:) (const)
- [NEXT_SB:](#NEXT_SB:) (const)
- [NEXT_SH:](#NEXT_SH:) (const)
- [NEXT_SW:](#NEXT_SW:) (const)
- [NEXT_ZB:](#NEXT_ZB:) (const)
- [NEXT_ZH:](#NEXT_ZH:) (const)
- [NEXT_ZW:](#NEXT_ZW:) (const)
- [NP_ADD:](#NP_ADD:) (const)
- [NP_SUB:](#NP_SUB:) (const)
- [NP_MUL:](#NP_MUL:) (const)
- [NP_DIV_OVF:](#NP_DIV_OVF:) (const)
- [NP_DIV_ZERO:](#NP_DIV_ZERO:) (const)
- [NP_MOD_ZERO:](#NP_MOD_ZERO:) (const)
- [NG_VBASE:](#NG_VBASE:) (const)
- [MO_LI:](#MO_LI:) (const)
- [MO_MOV:](#MO_MOV:) (const)
- [MO_SLOTADDR:](#MO_SLOTADDR:) (const)
- [MO_LDSLOT:](#MO_LDSLOT:) (const)
- [MO_STSLOT:](#MO_STSLOT:) (const)
- [MO_LOAD:](#MO_LOAD:) (const)
- [MO_STORE:](#MO_STORE:) (const)
- [MO_ADDR:](#MO_ADDR:) (const)
- [MO_CALL:](#MO_CALL:) (const)
- [MO_CALLR:](#MO_CALLR:) (const)
- [MO_JMP:](#MO_JMP:) (const)
- [MO_BNEZ:](#MO_BNEZ:) (const)
- [MO_BEQZ:](#MO_BEQZ:) (const)
- [MO_PANICIF:](#MO_PANICIF:) (const)
- [MO_TRAP:](#MO_TRAP:) (const)
- [MO_ADD:](#MO_ADD:) (const)
- [MO_SUB:](#MO_SUB:) (const)
- [MO_MUL:](#MO_MUL:) (const)
- [MO_AND:](#MO_AND:) (const)
- [MO_OR:](#MO_OR:) (const)
- [MO_XOR:](#MO_XOR:) (const)
- [MO_SHL:](#MO_SHL:) (const)
- [MO_SAR:](#MO_SAR:) (const)
- [MO_SHR:](#MO_SHR:) (const)
- [MO_NEG:](#MO_NEG:) (const)
- [MO_NOT:](#MO_NOT:) (const)
- [MO_DIV:](#MO_DIV:) (const)
- [MO_REM:](#MO_REM:) (const)
- [MO_MULH:](#MO_MULH:) (const)
- [MO_SETCC:](#MO_SETCC:) (const)
- [MO_EXT:](#MO_EXT:) (const)
- [MO_FBIN:](#MO_FBIN:) (const)
- [MO_FCMP:](#MO_FCMP:) (const)
- [MO_I2F:](#MO_I2F:) (const)
- [MO_F2I:](#MO_F2I:) (const)
- [MO_PROLOGUE:](#MO_PROLOGUE:) (const)
- [MO_EPILOGUE:](#MO_EPILOGUE:) (const)
- [MO_LDIN:](#MO_LDIN:) (const)
- [MO_STOUT:](#MO_STOUT:) (const)
- [MO_COPY:](#MO_COPY:) (const)
- [MO_PLACE:](#MO_PLACE:) (const)
- [MO_ASR63:](#MO_ASR63:) (const)
- [MO_LINE:](#MO_LINE:) (const)
- [MO_F32BITS:](#MO_F32BITS:) (const)
- [MO_BRCC:](#MO_BRCC:) (const)
- [MO_ADDSHL:](#MO_ADDSHL:) (const)
- [MO_ANDN:](#MO_ANDN:) (const)
- [MO_BLSR:](#MO_BLSR:) (const)
- [MO_LDX:](#MO_LDX:) (const)
- [MO_STX:](#MO_STX:) (const)
- [MO_CSEL:](#MO_CSEL:) (const)
- [MO_ADDI:](#MO_ADDI:) (const)
- [MO_TBNZ:](#MO_TBNZ:) (const)
- [MO_OPW:](#MO_OPW:) (const)
- [MO_ALUM:](#MO_ALUM:) (const)
- [MO_SHI:](#MO_SHI:) (const)
- [MO_VLD:](#MO_VLD:) (const)
- [MO_VST:](#MO_VST:) (const)
- [MO_VBIN:](#MO_VBIN:) (const)
- [MO_VSPLAT:](#MO_VSPLAT:) (const)
- [MO_VLANE:](#MO_VLANE:) (const)
- [MO_VZMASK:](#MO_VZMASK:) (const)
- [MO_VEND:](#MO_VEND:) (const)
- [MO_VBTEST:](#MO_VBTEST:) (const)
- [MO_BRCCM:](#MO_BRCCM:) (const)
- [MO_SETCCM:](#MO_SETCCM:) (const)
- [MO_ALUMST:](#MO_ALUMST:) (const)
- [MO_BPARAM:](#MO_BPARAM:) (const)
- [MO_BARG:](#MO_BARG:) (const)
- [MO_JTAB:](#MO_JTAB:) (const)
- [MO_JTE:](#MO_JTE:) (const)
- [MO_LAST:](#MO_LAST:) (const)
- [MInst](#MInst) (struct)
- [mi_new](#mi_new) (function)
- [mi_defines](#mi_defines) (function)
- [mi_use_mask](#mi_use_mask) (function)
- [mi_is_call](#mi_is_call) (function)
- [mi_is_cond_br](#mi_is_cond_br) (function)
- [mi_is_br](#mi_is_br) (function)
- [mi_nop](#mi_nop) (function)
- [mi_ends_block](#mi_ends_block) (function)
- [mi_has_jtab](#mi_has_jtab) (function)
- [mi_field](#mi_field) (function)
- [mi_set_field](#mi_set_field) (function)
- [MBlocks](#MBlocks) (struct)
- [mi_blocks](#mi_blocks) (function)
- [mi_liveness](#mi_liveness) (function)
- [mi_op_name](#mi_op_name) (function)
- [mi_text](#mi_text) (function)
- [mir_listing](#mir_listing) (function)
- [mir_reg_ok](#mir_reg_ok) (function)
- [mir_verify_at](#mir_verify_at) (function)
- [mir_verify](#mir_verify) (function)
- [mir_verify_bparams](#mir_verify_bparams) (function)
- [mir_bparams_ssa_check](#mir_bparams_ssa_check) (function)
- [MirBp](#MirBp) (struct)
- [mir_bparams_form](#mir_bparams_form) (function)
- [mir_par_moves](#mir_par_moves) (function)
- [mir_bparams_lower](#mir_bparams_lower) (function)

## NCC_EQ:

```fides
pub const NCC_EQ: i64 = 0;
```

*Defined at line 62*

---

## NCC_NE:

```fides
pub const NCC_NE: i64 = 1;
```

*Defined at line 63*

---

## NCC_LT:

```fides
pub const NCC_LT: i64 = 2;
```

*Defined at line 64*

---

## NCC_LE:

```fides
pub const NCC_LE: i64 = 3;
```

*Defined at line 65*

---

## NCC_GT:

```fides
pub const NCC_GT: i64 = 4;
```

*Defined at line 66*

---

## NCC_GE:

```fides
pub const NCC_GE: i64 = 5;
```

*Defined at line 67*

---

## NCC_LTU:

```fides
pub const NCC_LTU: i64 = 6;
```

*Defined at line 68*

---

## NCC_LEU:

```fides
pub const NCC_LEU: i64 = 7;
```

*Defined at line 69*

---

## NCC_GTU:

```fides
pub const NCC_GTU: i64 = 8;
```

*Defined at line 70*

---

## NCC_GEU:

```fides
pub const NCC_GEU: i64 = 9;
```

*Defined at line 71*

---

## NEXT_SB:

```fides
pub const NEXT_SB: i64 = 1;
```

*Defined at line 73*

---

## NEXT_SH:

```fides
pub const NEXT_SH: i64 = 2;
```

*Defined at line 74*

---

## NEXT_SW:

```fides
pub const NEXT_SW: i64 = 3;
```

*Defined at line 75*

---

## NEXT_ZB:

```fides
pub const NEXT_ZB: i64 = 4;
```

*Defined at line 76*

---

## NEXT_ZH:

```fides
pub const NEXT_ZH: i64 = 5;
```

*Defined at line 77*

---

## NEXT_ZW:

```fides
pub const NEXT_ZW: i64 = 6;
```

*Defined at line 78*

---

## NP_ADD:

```fides
pub const NP_ADD: i64 = 0;
```

*Defined at line 80*

---

## NP_SUB:

```fides
pub const NP_SUB: i64 = 1;
```

*Defined at line 81*

---

## NP_MUL:

```fides
pub const NP_MUL: i64 = 2;
```

*Defined at line 82*

---

## NP_DIV_OVF:

```fides
pub const NP_DIV_OVF: i64 = 3;
```

*Defined at line 83*

---

## NP_DIV_ZERO:

```fides
pub const NP_DIV_ZERO: i64 = 4;
```

*Defined at line 84*

---

## NP_MOD_ZERO:

```fides
pub const NP_MOD_ZERO: i64 = 5;
```

*Defined at line 85*

---

## NG_VBASE:

```fides
pub const NG_VBASE: i64 = 64;
```

*Defined at line 89*

---

## MO_LI:

```fides
pub const MO_LI: i64 = 1;
```

*Defined at line 91*

---

## MO_MOV:

```fides
pub const MO_MOV: i64 = 2;
```

*Defined at line 92*

---

## MO_SLOTADDR:

```fides
pub const MO_SLOTADDR: i64 = 3;
```

*Defined at line 93*

---

## MO_LDSLOT:

```fides
pub const MO_LDSLOT: i64 = 4;
```

*Defined at line 94*

---

## MO_STSLOT:

```fides
pub const MO_STSLOT: i64 = 5;
```

*Defined at line 95*

---

## MO_LOAD:

```fides
pub const MO_LOAD: i64 = 6;
```

*Defined at line 96*

---

## MO_STORE:

```fides
pub const MO_STORE: i64 = 7;
```

*Defined at line 97*

---

## MO_ADDR:

```fides
pub const MO_ADDR: i64 = 8;
```

*Defined at line 98*

---

## MO_CALL:

```fides
pub const MO_CALL: i64 = 9;
```

*Defined at line 99*

---

## MO_CALLR:

```fides
pub const MO_CALLR: i64 = 10;
```

*Defined at line 100*

---

## MO_JMP:

```fides
pub const MO_JMP: i64 = 11;
```

*Defined at line 101*

---

## MO_BNEZ:

```fides
pub const MO_BNEZ: i64 = 12;
```

*Defined at line 102*

---

## MO_BEQZ:

```fides
pub const MO_BEQZ: i64 = 13;
```

*Defined at line 103*

---

## MO_PANICIF:

```fides
pub const MO_PANICIF: i64 = 14;
```

*Defined at line 104*

---

## MO_TRAP:

```fides
pub const MO_TRAP: i64 = 15;
```

*Defined at line 105*

---

## MO_ADD:

```fides
pub const MO_ADD: i64 = 16;
```

*Defined at line 106*

---

## MO_SUB:

```fides
pub const MO_SUB: i64 = 17;
```

*Defined at line 107*

---

## MO_MUL:

```fides
pub const MO_MUL: i64 = 18;
```

*Defined at line 108*

---

## MO_AND:

```fides
pub const MO_AND: i64 = 19;
```

*Defined at line 109*

---

## MO_OR:

```fides
pub const MO_OR: i64 = 20;
```

*Defined at line 110*

---

## MO_XOR:

```fides
pub const MO_XOR: i64 = 21;
```

*Defined at line 111*

---

## MO_SHL:

```fides
pub const MO_SHL: i64 = 22;
```

*Defined at line 112*

---

## MO_SAR:

```fides
pub const MO_SAR: i64 = 23;
```

*Defined at line 113*

---

## MO_SHR:

```fides
pub const MO_SHR: i64 = 24;
```

*Defined at line 114*

---

## MO_NEG:

```fides
pub const MO_NEG: i64 = 25;
```

*Defined at line 115*

---

## MO_NOT:

```fides
pub const MO_NOT: i64 = 26;
```

*Defined at line 116*

---

## MO_DIV:

```fides
pub const MO_DIV: i64 = 27;
```

*Defined at line 117*

---

## MO_REM:

```fides
pub const MO_REM: i64 = 28;
```

*Defined at line 118*

---

## MO_MULH:

```fides
pub const MO_MULH: i64 = 29;
```

*Defined at line 119*

---

## MO_SETCC:

```fides
pub const MO_SETCC: i64 = 30;
```

*Defined at line 120*

---

## MO_EXT:

```fides
pub const MO_EXT: i64 = 31;
```

*Defined at line 121*

---

## MO_FBIN:

```fides
pub const MO_FBIN: i64 = 32;
```

*Defined at line 122*

---

## MO_FCMP:

```fides
pub const MO_FCMP: i64 = 33;
```

*Defined at line 123*

---

## MO_I2F:

```fides
pub const MO_I2F: i64 = 34;
```

*Defined at line 124*

---

## MO_F2I:

```fides
pub const MO_F2I: i64 = 35;
```

*Defined at line 125*

---

## MO_PROLOGUE:

```fides
pub const MO_PROLOGUE: i64 = 36;
```

*Defined at line 126*

---

## MO_EPILOGUE:

```fides
pub const MO_EPILOGUE: i64 = 37;
```

*Defined at line 127*

---

## MO_LDIN:

```fides
pub const MO_LDIN: i64 = 38;
```

*Defined at line 128*

---

## MO_STOUT:

```fides
pub const MO_STOUT: i64 = 39;
```

*Defined at line 129*

---

## MO_COPY:

```fides
pub const MO_COPY: i64 = 40;
```

*Defined at line 130*

---

## MO_PLACE:

```fides
pub const MO_PLACE: i64 = 41;
```

*Defined at line 131*

---

## MO_ASR63:

```fides
pub const MO_ASR63: i64 = 42;
```

*Defined at line 132*

---

## MO_LINE:

```fides
pub const MO_LINE: i64 = 43;  // i0: a source line starts here (no code)
```

*Defined at line 133*

---

## MO_F32BITS:

```fides
pub const MO_F32BITS: i64 = 44;  // r0 = zero-extended f32 pattern of widened-f64 r1
```

*Defined at line 134*

---

## MO_BRCC:

```fides
pub const MO_BRCC: i64 = 45;
```

*Defined at line 138*

---

## MO_ADDSHL:

```fides
pub const MO_ADDSHL: i64 = 46;
```

*Defined at line 141*

---

## MO_ANDN:

```fides
pub const MO_ANDN: i64 = 47;
```

*Defined at line 147*

---

## MO_BLSR:

```fides
pub const MO_BLSR: i64 = 48;
```

*Defined at line 148*

---

## MO_LDX:

```fides
pub const MO_LDX: i64 = 49;
```

*Defined at line 155*

---

## MO_STX:

```fides
pub const MO_STX: i64 = 50;
```

*Defined at line 156*

---

## MO_CSEL:

```fides
pub const MO_CSEL: i64 = 51;
```

*Defined at line 162*

---

## MO_ADDI:

```fides
pub const MO_ADDI: i64 = 52;
```

*Defined at line 168*

---

## MO_TBNZ:

```fides
pub const MO_TBNZ: i64 = 53;
```

*Defined at line 173*

---

## MO_OPW:

```fides
pub const MO_OPW: i64 = 54;
```

*Defined at line 178*

---

## MO_ALUM:

```fides
pub const MO_ALUM: i64 = 55;
```

*Defined at line 184*

---

## MO_SHI:

```fides
pub const MO_SHI: i64 = 56;
```

*Defined at line 190*

---

## MO_VLD:

```fides
pub const MO_VLD: i64 = 57;
```

*Defined at line 213*

---

## MO_VST:

```fides
pub const MO_VST: i64 = 58;
```

*Defined at line 214*

---

## MO_VBIN:

```fides
pub const MO_VBIN: i64 = 59;
```

*Defined at line 215*

---

## MO_VSPLAT:

```fides
pub const MO_VSPLAT: i64 = 60;
```

*Defined at line 216*

---

## MO_VLANE:

```fides
pub const MO_VLANE: i64 = 61;
```

*Defined at line 217*

---

## MO_VZMASK:

```fides
pub const MO_VZMASK: i64 = 62;
```

*Defined at line 218*

---

## MO_VEND:

```fides
pub const MO_VEND: i64 = 63;
```

*Defined at line 219*

---

## MO_VBTEST:

```fides
pub const MO_VBTEST: i64 = 71;
```

*Defined at line 226*

---

## MO_BRCCM:

```fides
pub const MO_BRCCM: i64 = 64;
```

*Defined at line 244*

---

## MO_SETCCM:

```fides
pub const MO_SETCCM: i64 = 65;
```

*Defined at line 245*

---

## MO_ALUMST:

```fides
pub const MO_ALUMST: i64 = 66;
```

*Defined at line 246*

---

## MO_BPARAM:

```fides
pub const MO_BPARAM: i64 = 67;
```

*Defined at line 251*

---

## MO_BARG:

```fides
pub const MO_BARG: i64 = 68;
```

*Defined at line 252*

---

## MO_JTAB:

```fides
pub const MO_JTAB: i64 = 69;
```

*Defined at line 269*

---

## MO_JTE:

```fides
pub const MO_JTE: i64 = 70;
```

*Defined at line 270*

---

## MO_LAST:

```fides
pub const MO_LAST: i64 = 71;
```

*Defined at line 272*

---

## MInst

```fides
pub struct MInst {
```

**Fields:**

| Name | Type |
|------|------|
| `op` | `i64` |
| `r0` | `i64` |
| `r1` | `i64` |
| `r2` | `i64` |
| `i0` | `i64` |
| `i1` | `i64` |
| `i2` | `i64` |

*Defined at line 274*

---

## mi_new

```fides
pub fn mi_new(op: i64, r0: i64, r1: i64, r2: i64, i0: i64, i1: i64, i2: i64) -> MInst {
```

**Parameters:**

- `op`: `i64`
- `r0`: `i64`
- `r1`: `i64`
- `r2`: `i64`
- `i0`: `i64`
- `i1`: `i64`
- `i2`: `i64`

**Returns:** `MInst`

*Defined at line 284*

---

## mi_defines

r0 of `op` is the register it defines.

```fides
pub fn mi_defines(op: i64) -> bool {
```

**Parameters:**

- `op`: `i64`

**Returns:** `bool`

*Defined at line 291*

---

## mi_use_mask

The fields of `op` it reads: 1 r0, 2 r1, 4 r2.

```fides
pub fn mi_use_mask(op: i64) -> i64 {
```

**Parameters:**

- `op`: `i64`

**Returns:** `i64`

*Defined at line 314*

---

## mi_is_call

```fides
pub fn mi_is_call(op: i64) -> bool {
```

**Parameters:**

- `op`: `i64`

**Returns:** `bool`

*Defined at line 348*

---

## mi_is_cond_br

A jump inside the function to label i0: unconditional (MO_JMP) or
conditional (it falls through to the next instruction otherwise).

```fides
pub fn mi_is_cond_br(op: i64) -> bool {
```

**Parameters:**

- `op`: `i64`

**Returns:** `bool`

*Defined at line 354*

---

## mi_is_br

```fides
pub fn mi_is_br(op: i64) -> bool {
```

**Parameters:**

- `op`: `i64`

**Returns:** `bool`

*Defined at line 358*

---

## mi_nop

```fides
pub fn mi_nop() -> MInst {
```

**Returns:** `MInst`

*Defined at line 362*

---

## mi_ends_block

Ends a machine block: control does not go on to the next instruction
only (it may jump, or not continue at all).

```fides
pub fn mi_ends_block(op: i64) -> bool {
```

**Parameters:**

- `op`: `i64`

**Returns:** `bool`

*Defined at line 370*

---

## mi_has_jtab

MI27: the function holds a jump table (MO_JTAB). The passes that put
code on a fall-through or after a block's last instruction (live-range
splitting, shrink-wrapping, block parameters) or move blocks (the cold
layout) leave it alone: nothing may come between a dispatch and its
entries.

```fides
pub fn mi_has_jtab(mir: &Vec<MInst>) -> bool {
```

**Parameters:**

- `mir`: `&Vec<MInst>`

**Returns:** `bool`

*Defined at line 379*

---

## mi_field

```fides
pub fn mi_field(m: MInst, f: i64) -> i64 {
```

**Parameters:**

- `m`: `MInst`
- `f`: `i64`

**Returns:** `i64`

*Defined at line 390*

---

## mi_set_field

m with register field f set to v.

```fides
pub fn mi_set_field(m: MInst, f: i64, v: i64) -> MInst {
```

**Parameters:**

- `m`: `MInst`
- `f`: `i64`
- `v`: `i64`

**Returns:** `MInst`

*Defined at line 401*

---

## MBlocks

Machine blocks: starts, ends, the block of each instruction, successors.

```fides
pub struct MBlocks {
```

**Fields:**

| Name | Type |
|------|------|
| `start` | `Vec<i64>` |
| `end` | `Vec<i64>` |
| `of` | `Vec<i64>` |
| `succs` | `Vec<Vec<i64>>` |

*Defined at line 416*

---

## mi_blocks

```fides
pub fn mi_blocks(mir: &Vec<MInst>) -> MBlocks {
```

**Parameters:**

- `mir`: `&Vec<MInst>`

**Returns:** `MBlocks`

*Defined at line 423*

---

## mi_liveness

Block liveness over the vregs whose index has sel[vi] >= 0 (their bit is
sel[vi]); live_in and live_out as bit sets of `words` words per block.
(MI1: a BPARAM defines its vreg at the block's start, a BARG reads its
argument at the jump; both are ordinary definitions and uses here.)

```fides
pub fn mi_liveness(mir: &Vec<MInst>, mb: &MBlocks, sel: &Vec<i64>, words: i64, live_in: &mut Vec<i64>, live_out: &mut Vec<i64>) {
```

**Parameters:**

- `mir`: `&Vec<MInst>`
- `mb`: `&MBlocks`
- `sel`: `&Vec<i64>`
- `words`: `i64`
- `live_in`: `&mut Vec<i64>`
- `live_out`: `&mut Vec<i64>`

*Defined at line 490*

---

## mi_op_name

The name of a machine IR operation (the MIR printer, the verifier).

```fides
pub fn mi_op_name(op: i64) -> str {
```

**Parameters:**

- `op`: `i64`

**Returns:** `str`

*Defined at line 567*

---

## mi_text

One operation as the MIR listings print it (FID_NAT_DUMP, `fid explain
--stages`): its name, the three register fields (`v` a vreg, `p` a
physical register) and i0, i1.

```fides
pub fn mi_text(m: MInst) -> str {
```

**Parameters:**

- `m`: `MInst`

**Returns:** `str`

*Defined at line 684*

---

## mir_listing

A whole function, one operation a line ("k: text").

```fides
pub fn mir_listing(mir: &Vec<MInst>) -> str {
```

**Parameters:**

- `mir`: `&Vec<MInst>`

**Returns:** `str`

*Defined at line 700*

---

## mir_reg_ok

v names a register a MIR operand may hold: a vreg of the function, a
physical register, or `zero_reg` (x86-64 60, the zero operand of a
compare; -1 on targets whose zero register is an ordinary number).

```fides
pub fn mir_reg_ok(v: i64, nvreg: i64, zero_reg: i64) -> bool {
```

**Parameters:**

- `v`: `i64`
- `nvreg`: `i64`
- `zero_reg`: `i64`

**Returns:** `bool`

*Defined at line 715*

---

## mir_verify_at

The verifier's prefix for an error at MIR position k.

```fides
pub fn mir_verify_at(name: str, k: i64, op: i64) -> str {
```

**Parameters:**

- `name`: `str`
- `k`: `i64`
- `op`: `i64`

**Returns:** `str`

*Defined at line 726*

---

## mir_verify

MI10: the structural MIR verifier, without the target's own checks
(nat_ir.fd ng_mir_verify adds the vector operations'): every operation
known; every register operand a vreg of the function (nvreg vregs, the
first nslots of them the IR values') or a physical register; every
temporary (a vreg past the IR values') read somewhere written somewhere
(a temporary belongs to one IR instruction's lowering, so one read with
no write is a lowering bug: an IR value's vreg may be read on a path
that never sets it, which is the IR verifier's to judge); every jump to
a label the function places, each placed once; operand classes (access
sizes 1/2/4/8, extension kinds, compare kinds, shift counts, panic
kinds); block parameters and arguments well formed (mir_verify_bparams);
the prologue first and nothing falling off the end.

```fides
pub fn mir_verify(name: str, mir: &Vec<MInst>, nvreg: i64, nslots: i64, zero_reg: i64) -> str {
```

**Parameters:**

- `name`: `str`
- `mir`: `&Vec<MInst>`
- `nvreg`: `i64`
- `nslots`: `i64`
- `zero_reg`: `i64`

**Returns:** `str`

*Defined at line 742*

---

## mir_verify_bparams

MI1: block parameters and arguments well formed. A block's BPARAMs come
right after its PLACE, numbered 0, 1, ..; a parameter is a vreg defined
there only. A block's BARGs come together right before its jump (or at
its end when it falls through), each naming a label the block's edges
go to (the jump's target, or the block it falls into) and a parameter
that label's block has; every edge passes each of its target's
parameters exactly once.

```fides
pub fn mir_verify_bparams(name: str, mir: &Vec<MInst>) -> str {
```

**Parameters:**

- `name`: `str`
- `mir`: `&Vec<MInst>`

**Returns:** `str`

*Defined at line 944*

---

## mir_bparams_ssa_check

MI1: the formed MIR is SSA: every vreg but `keep` written by at most
one operation (an operation that updates its r0 in place, MO_CSEL, not
counted: it follows the move that defines its name), and every vreg
written more than once in `orig` (the MIR before the form) no longer
read or written. "" when it holds.

```fides
pub fn mir_bparams_ssa_check(name: str, orig: &Vec<MInst>, mir: &Vec<MInst>, nvreg: i64, keep: i64) -> str {
```

**Parameters:**

- `name`: `str`
- `orig`: `&Vec<MInst>`
- `mir`: `&Vec<MInst>`
- `nvreg`: `i64`
- `keep`: `i64`

**Returns:** `str`

*Defined at line 1089*

---

## MirBp

A function's MIR with block parameters (mir_bparams_form) or without
them again (mir_bparams_lower).

```fides
pub struct MirBp {
```

**Fields:**

| Name | Type |
|------|------|
| `mir` | `Vec<MInst>` |
| `nvreg` | `i64` |
| `n_params` | `i64` |
| `n_labels` | `i64` |
| `err` | `str` |

*Defined at line 1142*

---

## mir_bparams_form

MI1: the MIR in block-parameter form. Every vreg (but `keep`, the
struct-result pointer) that more than one operation writes becomes one
vreg per write, and each labeled block takes a parameter for each such
value live into it, which every edge into the block passes. The form
computes what the original computes: each name holds, wherever it is
read, the value the original vreg held there. Nothing changes (n_params
0) when no vreg is written twice, when the entry is a labeled block, or
when the function takes the address of one of its own labels (MO_ADDR:
an edge the blocks do not show).

```fides
pub fn mir_bparams_form(mir: &Vec<MInst>, nvreg: i64, keep: i64) -> MirBp {
```

**Parameters:**

- `mir`: `&Vec<MInst>`
- `nvreg`: `i64`
- `keep`: `i64`

**Returns:** `MirBp`

*Defined at line 1177*

---

## mir_par_moves

The parallel copy dst[i] = src[i] (all read before any is written) as
moves; a cycle goes through a fresh vreg. Returns the vreg count after.

```fides
pub fn mir_par_moves(out: &mut Vec<MInst>, dst: &Vec<i64>, src: &Vec<i64>, nvreg: i64) -> i64 {
```

**Parameters:**

- `out`: `&mut Vec<MInst>`
- `dst`: `&Vec<i64>`
- `src`: `&Vec<i64>`
- `nvreg`: `i64`

**Returns:** `i64`

*Defined at line 1404*

---

## mir_bparams_lower

MI1: block parameters out again, before allocation: each edge's
arguments become moves into the parameters' vregs (a parallel copy,
mir_par_moves) — before an unconditional jump, at the end of a block
that falls into the labeled block, after a conditional branch for the
edge it falls through, and in an edge block for the edge it takes (the
branch goes to a new label label0, label0 + 1, .., placed after the
function's end with the moves and a jump to the old target; n_labels of
them, which the caller creates). BPARAMs go away (one no edge passes a
value, of a block nothing but itself enters, becomes a 0).

```fides
pub fn mir_bparams_lower(mir: &Vec<MInst>, nvreg: i64, label0: i64) -> MirBp {
```

**Parameters:**

- `mir`: `&Vec<MInst>`
- `nvreg`: `i64`
- `label0`: `i64`

**Returns:** `MirBp`

*Defined at line 1506*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
