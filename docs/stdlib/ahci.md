[All modules](index.md)

# ahci -- Fides Documentation

## Contents

- [AHCI_CAP:](#AHCI_CAP:) (const)
- [AHCI_GHC:](#AHCI_GHC:) (const)
- [AHCI_IS:](#AHCI_IS:) (const)
- [AHCI_PI:](#AHCI_PI:) (const)
- [AHCI_VS:](#AHCI_VS:) (const)
- [AHCI_CCC_CTL:](#AHCI_CCC_CTL:) (const)
- [AHCI_CCC_PORTS:](#AHCI_CCC_PORTS:) (const)
- [AHCI_EM_LOC:](#AHCI_EM_LOC:) (const)
- [AHCI_EM_CTL:](#AHCI_EM_CTL:) (const)
- [AHCI_CAP2:](#AHCI_CAP2:) (const)
- [AHCI_BOHC:](#AHCI_BOHC:) (const)
- [AHCI_GHC_HR:](#AHCI_GHC_HR:) (const)
- [AHCI_GHC_IE:](#AHCI_GHC_IE:) (const)
- [AHCI_GHC_AE:](#AHCI_GHC_AE:) (const)
- [AHCI_CAP_SNCQ:](#AHCI_CAP_SNCQ:) (const)
- [AHCI_CAP_S64A:](#AHCI_CAP_S64A:) (const)
- [AHCI_CAP_SSS:](#AHCI_CAP_SSS:) (const)
- [AHCI_CAP_SCLO:](#AHCI_CAP_SCLO:) (const)
- [AHCI_PX_CLB:](#AHCI_PX_CLB:) (const)
- [AHCI_PX_CLBU:](#AHCI_PX_CLBU:) (const)
- [AHCI_PX_FB:](#AHCI_PX_FB:) (const)
- [AHCI_PX_FBU:](#AHCI_PX_FBU:) (const)
- [AHCI_PX_IS:](#AHCI_PX_IS:) (const)
- [AHCI_PX_IE:](#AHCI_PX_IE:) (const)
- [AHCI_PX_CMD:](#AHCI_PX_CMD:) (const)
- [AHCI_PX_TFD:](#AHCI_PX_TFD:) (const)
- [AHCI_PX_SIG:](#AHCI_PX_SIG:) (const)
- [AHCI_PX_SSTS:](#AHCI_PX_SSTS:) (const)
- [AHCI_PX_SCTL:](#AHCI_PX_SCTL:) (const)
- [AHCI_PX_SERR:](#AHCI_PX_SERR:) (const)
- [AHCI_PX_SACT:](#AHCI_PX_SACT:) (const)
- [AHCI_PX_CI:](#AHCI_PX_CI:) (const)
- [AHCI_PX_SNTF:](#AHCI_PX_SNTF:) (const)
- [AHCI_PX_FBS:](#AHCI_PX_FBS:) (const)
- [AHCI_PXCMD_ST:](#AHCI_PXCMD_ST:) (const)
- [AHCI_PXCMD_SUD:](#AHCI_PXCMD_SUD:) (const)
- [AHCI_PXCMD_POD:](#AHCI_PXCMD_POD:) (const)
- [AHCI_PXCMD_CLO:](#AHCI_PXCMD_CLO:) (const)
- [AHCI_PXCMD_FRE:](#AHCI_PXCMD_FRE:) (const)
- [AHCI_PXCMD_FR:](#AHCI_PXCMD_FR:) (const)
- [AHCI_PXCMD_CR:](#AHCI_PXCMD_CR:) (const)
- [AHCI_PXIS_DHRS:](#AHCI_PXIS_DHRS:) (const)
- [AHCI_PXIS_PSS:](#AHCI_PXIS_PSS:) (const)
- [AHCI_PXIS_DSS:](#AHCI_PXIS_DSS:) (const)
- [AHCI_PXIS_SDBS:](#AHCI_PXIS_SDBS:) (const)
- [AHCI_PXIS_DPS:](#AHCI_PXIS_DPS:) (const)
- [AHCI_PXIS_TFES:](#AHCI_PXIS_TFES:) (const)
- [ATA_SR_ERR:](#ATA_SR_ERR:) (const)
- [ATA_SR_DRQ:](#ATA_SR_DRQ:) (const)
- [ATA_SR_DF:](#ATA_SR_DF:) (const)
- [ATA_SR_DRDY:](#ATA_SR_DRDY:) (const)
- [ATA_SR_BSY:](#ATA_SR_BSY:) (const)
- [AHCI_SIG_ATA:](#AHCI_SIG_ATA:) (const)
- [AHCI_SIG_ATAPI:](#AHCI_SIG_ATAPI:) (const)
- [AHCI_SIG_SEMB:](#AHCI_SIG_SEMB:) (const)
- [AHCI_SIG_PM:](#AHCI_SIG_PM:) (const)
- [FIS_TYPE_REG_H2D:](#FIS_TYPE_REG_H2D:) (const)
- [FIS_TYPE_REG_D2H:](#FIS_TYPE_REG_D2H:) (const)
- [FIS_TYPE_DMA_ACT:](#FIS_TYPE_DMA_ACT:) (const)
- [FIS_TYPE_DMA_SETUP:](#FIS_TYPE_DMA_SETUP:) (const)
- [FIS_TYPE_DATA:](#FIS_TYPE_DATA:) (const)
- [FIS_TYPE_PIO_SETUP:](#FIS_TYPE_PIO_SETUP:) (const)
- [FIS_TYPE_DEV_BITS:](#FIS_TYPE_DEV_BITS:) (const)
- [ATA_CMD_READ_DMA_EXT:](#ATA_CMD_READ_DMA_EXT:) (const)
- [ATA_CMD_WRITE_DMA_EXT:](#ATA_CMD_WRITE_DMA_EXT:) (const)
- [ATA_CMD_IDENTIFY:](#ATA_CMD_IDENTIFY:) (const)
- [ATA_CMD_FLUSH_EXT:](#ATA_CMD_FLUSH_EXT:) (const)
- [ATA_CMD_READ_FPDMA:](#ATA_CMD_READ_FPDMA:) (const)
- [AHCI_CMD_HEADER_SIZE:](#AHCI_CMD_HEADER_SIZE:) (const)
- [AHCI_CMD_LIST_SIZE:](#AHCI_CMD_LIST_SIZE:) (const)
- [AHCI_RFIS_SIZE:](#AHCI_RFIS_SIZE:) (const)
- [AHCI_CMD_TABLE_PRDT:](#AHCI_CMD_TABLE_PRDT:) (const)
- [AHCI_PRD_SIZE:](#AHCI_PRD_SIZE:) (const)
- [AHCI_PRD_MAX:](#AHCI_PRD_MAX:) (const)
- [AHCI_RFIS_DSFIS:](#AHCI_RFIS_DSFIS:) (const)
- [AHCI_RFIS_PSFIS:](#AHCI_RFIS_PSFIS:) (const)
- [AHCI_RFIS_RFIS:](#AHCI_RFIS_RFIS:) (const)
- [AHCI_RFIS_SDBFIS:](#AHCI_RFIS_SDBFIS:) (const)
- [AHCI_RFIS_UFIS:](#AHCI_RFIS_UFIS:) (const)
- [ahci_port_base](#ahci_port_base) (function)
- [ahci_port_reg](#ahci_port_reg) (function)
- [ahci_cap_nports](#ahci_cap_nports) (function)
- [ahci_cap_nslots](#ahci_cap_nslots) (function)
- [ahci_cap_iss](#ahci_cap_iss) (function)
- [ahci_version_str](#ahci_version_str) (function)
- [ahci_ssts_active](#ahci_ssts_active) (function)
- [ahci_ssts_speed](#ahci_ssts_speed) (function)
- [ahci_sig_name](#ahci_sig_name) (function)
- [ahci_fis_h2d](#ahci_fis_h2d) (function)
- [ahci_fis_read_dma_ext](#ahci_fis_read_dma_ext) (function)
- [ahci_fis_write_dma_ext](#ahci_fis_write_dma_ext) (function)
- [ahci_fis_identify](#ahci_fis_identify) (function)
- [ahci_fis_flush](#ahci_fis_flush) (function)
- [ahci_fis_read_fpdma](#ahci_fis_read_fpdma) (function)
- [AhciD2h](#AhciD2h) (struct)
- [ahci_fis_parse_d2h](#ahci_fis_parse_d2h) (function)
- [ahci_cmd_header](#ahci_cmd_header) (function)
- [AhciCmdHeader](#AhciCmdHeader) (struct)
- [ahci_cmd_header_parse](#ahci_cmd_header_parse) (function)
- [ahci_prd](#ahci_prd) (function)
- [ahci_cmd_table](#ahci_cmd_table) (function)
- [ahci_prdt_len](#ahci_prdt_len) (function)
- [AhciIdentify](#AhciIdentify) (struct)
- [ahci_identify_parse](#ahci_identify_parse) (function)
- [AhciBus](#AhciBus) (struct)
- [AhciPort](#AhciPort) (struct)
- [AHCI_CT_STRIDE:](#AHCI_CT_STRIDE:) (const)
- [AHCI_POLL_LIMIT:](#AHCI_POLL_LIMIT:) (const)
- [ahci_rd](#ahci_rd) (function)
- [ahci_wr](#ahci_wr) (function)
- [ahci_wait](#ahci_wait) (function)
- [ahci_enable](#ahci_enable) (function)
- [ahci_reset](#ahci_reset) (function)
- [ahci_probe](#ahci_probe) (function)
- [ahci_port_stop](#ahci_port_stop) (function)
- [ahci_port_start](#ahci_port_start) (function)
- [ahci_port_setup](#ahci_port_setup) (function)
- [ahci_free_slot](#ahci_free_slot) (function)
- [ahci_exec](#ahci_exec) (function)
- [ahci_identify](#ahci_identify) (function)
- [ahci_read](#ahci_read) (function)
- [ahci_write](#ahci_write) (function)
- [ahci_blockdev](#ahci_blockdev) (function)

## AHCI_CAP:

```fides
pub const AHCI_CAP: i64 = 0;
```

*Defined at line 30*

---

## AHCI_GHC:

```fides
pub const AHCI_GHC: i64 = 4;
```

*Defined at line 31*

---

## AHCI_IS:

```fides
pub const AHCI_IS: i64 = 8;
```

*Defined at line 32*

---

## AHCI_PI:

```fides
pub const AHCI_PI: i64 = 12;
```

*Defined at line 33*

---

## AHCI_VS:

```fides
pub const AHCI_VS: i64 = 16;
```

*Defined at line 34*

---

## AHCI_CCC_CTL:

```fides
pub const AHCI_CCC_CTL: i64 = 20;
```

*Defined at line 35*

---

## AHCI_CCC_PORTS:

```fides
pub const AHCI_CCC_PORTS: i64 = 24;
```

*Defined at line 36*

---

## AHCI_EM_LOC:

```fides
pub const AHCI_EM_LOC: i64 = 28;
```

*Defined at line 37*

---

## AHCI_EM_CTL:

```fides
pub const AHCI_EM_CTL: i64 = 32;
```

*Defined at line 38*

---

## AHCI_CAP2:

```fides
pub const AHCI_CAP2: i64 = 36;
```

*Defined at line 39*

---

## AHCI_BOHC:

```fides
pub const AHCI_BOHC: i64 = 40;
```

*Defined at line 40*

---

## AHCI_GHC_HR:

```fides
pub const AHCI_GHC_HR: i64 = 1;
```

*Defined at line 42*

---

## AHCI_GHC_IE:

```fides
pub const AHCI_GHC_IE: i64 = 2;
```

*Defined at line 43*

---

## AHCI_GHC_AE:

```fides
pub const AHCI_GHC_AE: i64 = 2147483648;
```

*Defined at line 44*

---

## AHCI_CAP_SNCQ:

```fides
pub const AHCI_CAP_SNCQ: i64 = 1073741824;
```

*Defined at line 46*

---

## AHCI_CAP_S64A:

```fides
pub const AHCI_CAP_S64A: i64 = 2147483648;
```

*Defined at line 47*

---

## AHCI_CAP_SSS:

```fides
pub const AHCI_CAP_SSS: i64 = 134217728;
```

*Defined at line 48*

---

## AHCI_CAP_SCLO:

```fides
pub const AHCI_CAP_SCLO: i64 = 16777216;
```

*Defined at line 49*

---

## AHCI_PX_CLB:

```fides
pub const AHCI_PX_CLB: i64 = 0;
```

*Defined at line 53*

---

## AHCI_PX_CLBU:

```fides
pub const AHCI_PX_CLBU: i64 = 4;
```

*Defined at line 54*

---

## AHCI_PX_FB:

```fides
pub const AHCI_PX_FB: i64 = 8;
```

*Defined at line 55*

---

## AHCI_PX_FBU:

```fides
pub const AHCI_PX_FBU: i64 = 12;
```

*Defined at line 56*

---

## AHCI_PX_IS:

```fides
pub const AHCI_PX_IS: i64 = 16;
```

*Defined at line 57*

---

## AHCI_PX_IE:

```fides
pub const AHCI_PX_IE: i64 = 20;
```

*Defined at line 58*

---

## AHCI_PX_CMD:

```fides
pub const AHCI_PX_CMD: i64 = 24;
```

*Defined at line 59*

---

## AHCI_PX_TFD:

```fides
pub const AHCI_PX_TFD: i64 = 32;
```

*Defined at line 60*

---

## AHCI_PX_SIG:

```fides
pub const AHCI_PX_SIG: i64 = 36;
```

*Defined at line 61*

---

## AHCI_PX_SSTS:

```fides
pub const AHCI_PX_SSTS: i64 = 40;
```

*Defined at line 62*

---

## AHCI_PX_SCTL:

```fides
pub const AHCI_PX_SCTL: i64 = 44;
```

*Defined at line 63*

---

## AHCI_PX_SERR:

```fides
pub const AHCI_PX_SERR: i64 = 48;
```

*Defined at line 64*

---

## AHCI_PX_SACT:

```fides
pub const AHCI_PX_SACT: i64 = 52;
```

*Defined at line 65*

---

## AHCI_PX_CI:

```fides
pub const AHCI_PX_CI: i64 = 56;
```

*Defined at line 66*

---

## AHCI_PX_SNTF:

```fides
pub const AHCI_PX_SNTF: i64 = 60;
```

*Defined at line 67*

---

## AHCI_PX_FBS:

```fides
pub const AHCI_PX_FBS: i64 = 64;
```

*Defined at line 68*

---

## AHCI_PXCMD_ST:

```fides
pub const AHCI_PXCMD_ST: i64 = 1;
```

*Defined at line 70*

---

## AHCI_PXCMD_SUD:

```fides
pub const AHCI_PXCMD_SUD: i64 = 2;
```

*Defined at line 71*

---

## AHCI_PXCMD_POD:

```fides
pub const AHCI_PXCMD_POD: i64 = 4;
```

*Defined at line 72*

---

## AHCI_PXCMD_CLO:

```fides
pub const AHCI_PXCMD_CLO: i64 = 8;
```

*Defined at line 73*

---

## AHCI_PXCMD_FRE:

```fides
pub const AHCI_PXCMD_FRE: i64 = 16;
```

*Defined at line 74*

---

## AHCI_PXCMD_FR:

```fides
pub const AHCI_PXCMD_FR: i64 = 16384;
```

*Defined at line 75*

---

## AHCI_PXCMD_CR:

```fides
pub const AHCI_PXCMD_CR: i64 = 32768;
```

*Defined at line 76*

---

## AHCI_PXIS_DHRS:

```fides
pub const AHCI_PXIS_DHRS: i64 = 1;
```

*Defined at line 79*

---

## AHCI_PXIS_PSS:

```fides
pub const AHCI_PXIS_PSS: i64 = 2;
```

*Defined at line 80*

---

## AHCI_PXIS_DSS:

```fides
pub const AHCI_PXIS_DSS: i64 = 4;
```

*Defined at line 81*

---

## AHCI_PXIS_SDBS:

```fides
pub const AHCI_PXIS_SDBS: i64 = 8;
```

*Defined at line 82*

---

## AHCI_PXIS_DPS:

```fides
pub const AHCI_PXIS_DPS: i64 = 32;
```

*Defined at line 83*

---

## AHCI_PXIS_TFES:

```fides
pub const AHCI_PXIS_TFES: i64 = 1073741824;
```

*Defined at line 84*

---

## ATA_SR_ERR:

```fides
pub const ATA_SR_ERR: i64 = 1;
```

*Defined at line 87*

---

## ATA_SR_DRQ:

```fides
pub const ATA_SR_DRQ: i64 = 8;
```

*Defined at line 88*

---

## ATA_SR_DF:

```fides
pub const ATA_SR_DF: i64 = 32;
```

*Defined at line 89*

---

## ATA_SR_DRDY:

```fides
pub const ATA_SR_DRDY: i64 = 64;
```

*Defined at line 90*

---

## ATA_SR_BSY:

```fides
pub const ATA_SR_BSY: i64 = 128;
```

*Defined at line 91*

---

## AHCI_SIG_ATA:

```fides
pub const AHCI_SIG_ATA: i64 = 257;
```

*Defined at line 94*

---

## AHCI_SIG_ATAPI:

```fides
pub const AHCI_SIG_ATAPI: i64 = 3943956737;
```

*Defined at line 95*

---

## AHCI_SIG_SEMB:

```fides
pub const AHCI_SIG_SEMB: i64 = 3275489537;
```

*Defined at line 96*

---

## AHCI_SIG_PM:

```fides
pub const AHCI_SIG_PM: i64 = 2523463937;
```

*Defined at line 97*

---

## FIS_TYPE_REG_H2D:

```fides
pub const FIS_TYPE_REG_H2D: i64 = 39;
```

*Defined at line 100*

---

## FIS_TYPE_REG_D2H:

```fides
pub const FIS_TYPE_REG_D2H: i64 = 52;
```

*Defined at line 101*

---

## FIS_TYPE_DMA_ACT:

```fides
pub const FIS_TYPE_DMA_ACT: i64 = 57;
```

*Defined at line 102*

---

## FIS_TYPE_DMA_SETUP:

```fides
pub const FIS_TYPE_DMA_SETUP: i64 = 65;
```

*Defined at line 103*

---

## FIS_TYPE_DATA:

```fides
pub const FIS_TYPE_DATA: i64 = 70;
```

*Defined at line 104*

---

## FIS_TYPE_PIO_SETUP:

```fides
pub const FIS_TYPE_PIO_SETUP: i64 = 95;
```

*Defined at line 105*

---

## FIS_TYPE_DEV_BITS:

```fides
pub const FIS_TYPE_DEV_BITS: i64 = 161;
```

*Defined at line 106*

---

## ATA_CMD_READ_DMA_EXT:

```fides
pub const ATA_CMD_READ_DMA_EXT: i64 = 37;
```

*Defined at line 109*

---

## ATA_CMD_WRITE_DMA_EXT:

```fides
pub const ATA_CMD_WRITE_DMA_EXT: i64 = 53;
```

*Defined at line 110*

---

## ATA_CMD_IDENTIFY:

```fides
pub const ATA_CMD_IDENTIFY: i64 = 236;
```

*Defined at line 111*

---

## ATA_CMD_FLUSH_EXT:

```fides
pub const ATA_CMD_FLUSH_EXT: i64 = 234;
```

*Defined at line 112*

---

## ATA_CMD_READ_FPDMA:

```fides
pub const ATA_CMD_READ_FPDMA: i64 = 96;
```

*Defined at line 113*

---

## AHCI_CMD_HEADER_SIZE:

```fides
pub const AHCI_CMD_HEADER_SIZE: i64 = 32;
```

*Defined at line 116*

---

## AHCI_CMD_LIST_SIZE:

```fides
pub const AHCI_CMD_LIST_SIZE: i64 = 1024;
```

*Defined at line 117*

---

## AHCI_RFIS_SIZE:

```fides
pub const AHCI_RFIS_SIZE: i64 = 256;
```

*Defined at line 118*

---

## AHCI_CMD_TABLE_PRDT:

```fides
pub const AHCI_CMD_TABLE_PRDT: i64 = 128;
```

*Defined at line 119*

---

## AHCI_PRD_SIZE:

```fides
pub const AHCI_PRD_SIZE: i64 = 16;
```

*Defined at line 120*

---

## AHCI_PRD_MAX:

```fides
pub const AHCI_PRD_MAX: i64 = 4194304;
```

*Defined at line 121*

---

## AHCI_RFIS_DSFIS:

```fides
pub const AHCI_RFIS_DSFIS: i64 = 0;
```

*Defined at line 123*

---

## AHCI_RFIS_PSFIS:

```fides
pub const AHCI_RFIS_PSFIS: i64 = 32;
```

*Defined at line 124*

---

## AHCI_RFIS_RFIS:

```fides
pub const AHCI_RFIS_RFIS: i64 = 64;
```

*Defined at line 125*

---

## AHCI_RFIS_SDBFIS:

```fides
pub const AHCI_RFIS_SDBFIS: i64 = 88;
```

*Defined at line 126*

---

## AHCI_RFIS_UFIS:

```fides
pub const AHCI_RFIS_UFIS: i64 = 96;
```

*Defined at line 127*

---

## ahci_port_base

Offset of port `p`'s registers in ABAR.

```fides
pub fn ahci_port_base(p: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`

**Returns:** `i64`

*Defined at line 130*

---

## ahci_port_reg

```fides
pub fn ahci_port_reg(p: i64, reg: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 134*

---

## ahci_cap_nports

```fides
pub fn ahci_cap_nports(cap: i64) -> i64 {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `i64`

*Defined at line 140*

---

## ahci_cap_nslots

```fides
pub fn ahci_cap_nslots(cap: i64) -> i64 {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## ahci_cap_iss

Interface speed limit (CAP.ISS): 1 Gen1 1.5 Gb/s, 2 Gen2, 3 Gen3.

```fides
pub fn ahci_cap_iss(cap: i64) -> i64 {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `i64`

*Defined at line 149*

---

## ahci_version_str

"1.3.1"-style version from the VS register.

```fides
pub fn ahci_version_str(vs: i64) -> str {
```

**Parameters:**

- `vs`: `i64`

**Returns:** `str`

*Defined at line 154*

---

## ahci_ssts_active

Device present with Phy communication (DET = 3) and the link active
(IPM = 1).

```fides
pub fn ahci_ssts_active(ssts: i64) -> bool {
```

**Parameters:**

- `ssts`: `i64`

**Returns:** `bool`

*Defined at line 166*

---

## ahci_ssts_speed

```fides
pub fn ahci_ssts_speed(ssts: i64) -> i64 {
```

**Parameters:**

- `ssts`: `i64`

**Returns:** `i64`

*Defined at line 170*

---

## ahci_sig_name

```fides
pub fn ahci_sig_name(sig: i64) -> str {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `str`

*Defined at line 174*

---

## ahci_fis_h2d

Register FIS Host to Device (20 bytes) with the C bit set: a command.
`lba` is 48-bit, `count` 16-bit, `device` bit 6 = LBA mode.

```fides
pub fn ahci_fis_h2d(command: i64, features: i64, lba: i64, count: i64, device: i64) -> Vec<i64> {
```

**Parameters:**

- `command`: `i64`
- `features`: `i64`
- `lba`: `i64`
- `count`: `i64`
- `device`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 217*

---

## ahci_fis_read_dma_ext

READ DMA EXT of `count` sectors (1..65536; 65536 is sent as 0).

```fides
pub fn ahci_fis_read_dma_ext(lba: i64, count: i64) -> Vec<i64> {
```

**Parameters:**

- `lba`: `i64`
- `count`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 237*

---

## ahci_fis_write_dma_ext

```fides
pub fn ahci_fis_write_dma_ext(lba: i64, count: i64) -> Vec<i64> {
```

**Parameters:**

- `lba`: `i64`
- `count`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 241*

---

## ahci_fis_identify

```fides
pub fn ahci_fis_identify() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 245*

---

## ahci_fis_flush

```fides
pub fn ahci_fis_flush() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 249*

---

## ahci_fis_read_fpdma

READ FPDMA QUEUED (NCQ): the sector count goes in the features field,
the tag in count bits 7:3.

```fides
pub fn ahci_fis_read_fpdma(lba: i64, count: i64, tag: i64) -> Vec<i64> {
```

**Parameters:**

- `lba`: `i64`
- `count`: `i64`
- `tag`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 255*

---

## AhciD2h

```fides
pub struct AhciD2h {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `irq` | `bool` |
| `status` | `i64` |
| `error` | `i64` |
| `lba` | `i64` |
| `device` | `i64` |
| `count` | `i64` |

*Defined at line 259*

---

## ahci_fis_parse_d2h

Decode a Register FIS Device to Host (as found at RFIS+0x40).

```fides
pub fn ahci_fis_parse_d2h(f: &Vec<i64>, o: i64) -> AhciD2h {
```

**Parameters:**

- `f`: `&Vec<i64>`
- `o`: `i64`

**Returns:** `AhciD2h`

*Defined at line 270*

---

## ahci_cmd_header

A 32-byte command header: FIS length in dwords, flags, PRDT length and
the command table address.

```fides
pub fn ahci_cmd_header(fis_len: i64, atapi: bool, write: bool, prefetch: bool, clear_busy: bool, prdtl: i64, ctba: i64) -> Vec<i64> {
```

**Parameters:**

- `fis_len`: `i64`
- `atapi`: `bool`
- `write`: `bool`
- `prefetch`: `bool`
- `clear_busy`: `bool`
- `prdtl`: `i64`
- `ctba`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 291*

---

## AhciCmdHeader

```fides
pub struct AhciCmdHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `cfl` | `i64` |
| `atapi` | `bool` |
| `write` | `bool` |
| `prdtl` | `i64` |
| `prdbc` | `i64` |
| `ctba` | `i64` |

*Defined at line 314*

---

## ahci_cmd_header_parse

```fides
pub fn ahci_cmd_header_parse(h: &Vec<i64>, o: i64) -> AhciCmdHeader {
```

**Parameters:**

- `h`: `&Vec<i64>`
- `o`: `i64`

**Returns:** `AhciCmdHeader`

*Defined at line 323*

---

## ahci_prd

One PRD entry: data base address, byte count (even, 2..4 MiB), and
"interrupt on completion".

```fides
pub fn ahci_prd(dba: i64, nbytes: i64, irq: bool) -> Vec<i64> {
```

**Parameters:**

- `dba`: `i64`
- `nbytes`: `i64`
- `irq`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 332*

---

## ahci_cmd_table

The command table: the FIS at 0, ATAPI command at 0x40, then the PRDT
at 0x80 covering `nbytes` from `buf` in pieces of at most 4 MiB.

```fides
pub fn ahci_cmd_table(fis: &Vec<i64>, acmd: &Vec<i64>, buf: i64, nbytes: i64) -> Vec<i64> {
```

**Parameters:**

- `fis`: `&Vec<i64>`
- `acmd`: `&Vec<i64>`
- `buf`: `i64`
- `nbytes`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 346*

---

## ahci_prdt_len

```fides
pub fn ahci_prdt_len(nbytes: i64) -> i64 {
```

**Parameters:**

- `nbytes`: `i64`

**Returns:** `i64`

*Defined at line 376*

---

## AhciIdentify

```fides
pub struct AhciIdentify {
```

**Fields:**

| Name | Type |
|------|------|
| `model` | `str` |
| `serial` | `str` |
| `firmware` | `str` |
| `lba48` | `bool` |
| `sectors` | `i64` |
| `sector_size` | `i64` |
| `ncq` | `bool` |
| `queue_depth` | `i64` |

*Defined at line 382*

---

## ahci_identify_parse

```fides
pub fn ahci_identify_parse(d: &Vec<i64>) -> AhciIdentify {
```

**Parameters:**

- `d`: `&Vec<i64>`

**Returns:** `AhciIdentify`

*Defined at line 410*

---

## AhciBus

MMIO and host memory accessors (the platform seam).

```fides
pub struct AhciBus {
```

**Fields:**

| Name | Type |
|------|------|
| `mmio_rd` | `fn(i64) -> i64` |
| `mmio_wr` | `fn(i64, i64) -> i64` |
| `mem_rd` | `fn(i64, i64) -> Vec<i64>` |
| `mem_wr` | `fn(i64, Vec<i64>) -> i64` |

*Defined at line 436*

---

## AhciPort

Memory one port uses: the command list, the received-FIS area and the
command tables (one per slot, `ctba + slot * 256`).

```fides
pub struct AhciPort {
```

**Fields:**

| Name | Type |
|------|------|
| `port` | `i64` |
| `clb` | `i64` |
| `fb` | `i64` |
| `ctba` | `i64` |
| `nslots` | `i64` |
| `err` | `str` |
| `last_status` | `i64` |
| `last_error` | `i64` |

*Defined at line 445*

---

## AHCI_CT_STRIDE:

```fides
pub const AHCI_CT_STRIDE: i64 = 256;
```

*Defined at line 456*

---

## AHCI_POLL_LIMIT:

```fides
pub const AHCI_POLL_LIMIT: i64 = 100000;
```

*Defined at line 457*

---

## ahci_rd

```fides
pub fn ahci_rd(bus: &AhciBus, off: i64) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 459*

---

## ahci_wr

```fides
pub fn ahci_wr(bus: &AhciBus, off: i64, v: i64) {
```

**Parameters:**

- `bus`: `&AhciBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 464*

---

## ahci_wait

Wait until (reg & mask) == want; false after AHCI_POLL_LIMIT reads.

```fides
pub fn ahci_wait(bus: &AhciBus, off: i64, mask: i64, want: i64) -> bool {
```

**Parameters:**

- `bus`: `&AhciBus`
- `off`: `i64`
- `mask`: `i64`
- `want`: `i64`

**Returns:** `bool`

*Defined at line 480*

---

## ahci_enable

Turn AHCI mode on (GHC.AE) and return the implemented ports (PI).

```fides
pub fn ahci_enable(bus: &AhciBus) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`

**Returns:** `i64`

*Defined at line 492*

---

## ahci_reset

HBA reset (GHC.HR, cleared by the HBA when done), then AE again.

```fides
pub fn ahci_reset(bus: &AhciBus) -> bool {
```

**Parameters:**

- `bus`: `&AhciBus`

**Returns:** `bool`

*Defined at line 498*

---

## ahci_probe

Ports with an active SATA link and their signatures: [port, sig]...

```fides
pub fn ahci_probe(bus: &AhciBus) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&AhciBus`

**Returns:** `Vec<i64>`

*Defined at line 508*

---

## ahci_port_stop

Stop the command engine and FIS receive (clear ST, wait CR; clear FRE,
wait FR).

```fides
pub fn ahci_port_stop(bus: &AhciBus, p: i64) -> bool {
```

**Parameters:**

- `bus`: `&AhciBus`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 524*

---

## ahci_port_start

Start it again: wait CR clear, set FRE, then ST.

```fides
pub fn ahci_port_start(bus: &AhciBus, p: i64) -> bool {
```

**Parameters:**

- `bus`: `&AhciBus`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 535*

---

## ahci_port_setup

Point port `p` at its memory (aligned as required), clear errors and
interrupts, start it.

```fides
pub fn ahci_port_setup(bus: &AhciBus, p: i64, clb: i64, fb: i64, ctba: i64) -> AhciPort {
```

**Parameters:**

- `bus`: `&AhciBus`
- `p`: `i64`
- `clb`: `i64`
- `fb`: `i64`
- `ctba`: `i64`

**Returns:** `AhciPort`

*Defined at line 547*

---

## ahci_free_slot

A command slot neither in PxSACT nor PxCI, or -1.

```fides
pub fn ahci_free_slot(bus: &AhciBus, port: &AhciPort) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`
- `port`: `&AhciPort`

**Returns:** `i64`

*Defined at line 574*

---

## ahci_exec

Build the command in a free slot, issue it and wait for completion.
Returns 0 or -errno (EIO with the task file in last_status/last_error).

```fides
pub fn ahci_exec(bus: &AhciBus, port: &mut AhciPort, fis: Vec<i64>, write: bool, buf: i64, nbytes: i64) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`
- `port`: `&mut AhciPort`
- `fis`: `Vec<i64>`
- `write`: `bool`
- `buf`: `i64`
- `nbytes`: `i64`

**Returns:** `i64`

*Defined at line 588*

---

## ahci_identify

IDENTIFY DEVICE into the 512-byte buffer at `buf`.

```fides
pub fn ahci_identify(bus: &AhciBus, port: &mut AhciPort, buf: i64) -> AhciIdentify {
```

**Parameters:**

- `bus`: `&AhciBus`
- `port`: `&mut AhciPort`
- `buf`: `i64`

**Returns:** `AhciIdentify`

*Defined at line 626*

---

## ahci_read

```fides
pub fn ahci_read(bus: &AhciBus, port: &mut AhciPort, lba: i64, count: i64, buf: i64, sector: i64) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`
- `port`: `&mut AhciPort`
- `lba`: `i64`
- `count`: `i64`
- `buf`: `i64`
- `sector`: `i64`

**Returns:** `i64`

*Defined at line 635*

---

## ahci_write

```fides
pub fn ahci_write(bus: &AhciBus, port: &mut AhciPort, lba: i64, count: i64, buf: i64, sector: i64) -> i64 {
```

**Parameters:**

- `bus`: `&AhciBus`
- `port`: `&mut AhciPort`
- `lba`: `i64`
- `count`: `i64`
- `buf`: `i64`
- `sector`: `i64`

**Returns:** `i64`

*Defined at line 642*

---

## ahci_blockdev

A BlockDev over a set-up port: one sector per request through the DMA
bounce buffer at `buf` (sector-sized, physical, word aligned).

```fides
pub fn ahci_blockdev(bus: AhciBus, port: AhciPort, id: &AhciIdentify, buf: i64) -> BlockDev {
```

**Parameters:**

- `bus`: `AhciBus`
- `port`: `AhciPort`
- `id`: `&AhciIdentify`
- `buf`: `i64`

**Returns:** `BlockDev`

*Defined at line 691*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
