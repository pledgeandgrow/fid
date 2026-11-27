[All modules](index.md)

# nvme -- Fides Documentation

## Contents

- [NVME_REG_CAP:](#NVME_REG_CAP:) (const)
- [NVME_REG_VS:](#NVME_REG_VS:) (const)
- [NVME_REG_INTMS:](#NVME_REG_INTMS:) (const)
- [NVME_REG_INTMC:](#NVME_REG_INTMC:) (const)
- [NVME_REG_CC:](#NVME_REG_CC:) (const)
- [NVME_REG_CSTS:](#NVME_REG_CSTS:) (const)
- [NVME_REG_NSSR:](#NVME_REG_NSSR:) (const)
- [NVME_REG_AQA:](#NVME_REG_AQA:) (const)
- [NVME_REG_ASQ:](#NVME_REG_ASQ:) (const)
- [NVME_REG_ACQ:](#NVME_REG_ACQ:) (const)
- [NVME_REG_DBS:](#NVME_REG_DBS:) (const)
- [NVME_CC_EN:](#NVME_CC_EN:) (const)
- [NVME_CSTS_RDY:](#NVME_CSTS_RDY:) (const)
- [NVME_CSTS_CFS:](#NVME_CSTS_CFS:) (const)
- [NVME_PAGE:](#NVME_PAGE:) (const)
- [NVME_SQE_SIZE:](#NVME_SQE_SIZE:) (const)
- [NVME_CQE_SIZE:](#NVME_CQE_SIZE:) (const)
- [NVME_POLL_LIMIT:](#NVME_POLL_LIMIT:) (const)
- [NVME_ADMIN_DELETE_SQ:](#NVME_ADMIN_DELETE_SQ:) (const)
- [NVME_ADMIN_CREATE_SQ:](#NVME_ADMIN_CREATE_SQ:) (const)
- [NVME_ADMIN_GET_LOG_PAGE:](#NVME_ADMIN_GET_LOG_PAGE:) (const)
- [NVME_ADMIN_DELETE_CQ:](#NVME_ADMIN_DELETE_CQ:) (const)
- [NVME_ADMIN_CREATE_CQ:](#NVME_ADMIN_CREATE_CQ:) (const)
- [NVME_ADMIN_IDENTIFY:](#NVME_ADMIN_IDENTIFY:) (const)
- [NVME_ADMIN_ABORT:](#NVME_ADMIN_ABORT:) (const)
- [NVME_ADMIN_SET_FEATURES:](#NVME_ADMIN_SET_FEATURES:) (const)
- [NVME_ADMIN_GET_FEATURES:](#NVME_ADMIN_GET_FEATURES:) (const)
- [NVME_CMD_FLUSH:](#NVME_CMD_FLUSH:) (const)
- [NVME_CMD_WRITE:](#NVME_CMD_WRITE:) (const)
- [NVME_CMD_READ:](#NVME_CMD_READ:) (const)
- [NVME_CNS_NAMESPACE:](#NVME_CNS_NAMESPACE:) (const)
- [NVME_CNS_CONTROLLER:](#NVME_CNS_CONTROLLER:) (const)
- [NVME_CNS_NS_LIST:](#NVME_CNS_NS_LIST:) (const)
- [NVME_FEAT_NUM_QUEUES:](#NVME_FEAT_NUM_QUEUES:) (const)
- [NVME_SC_SUCCESS:](#NVME_SC_SUCCESS:) (const)
- [NVME_SC_INVALID_OPCODE:](#NVME_SC_INVALID_OPCODE:) (const)
- [NVME_SC_INVALID_FIELD:](#NVME_SC_INVALID_FIELD:) (const)
- [NVME_SC_LBA_RANGE:](#NVME_SC_LBA_RANGE:) (const)
- [NvmeCap](#NvmeCap) (struct)
- [nvme_cap_decode](#nvme_cap_decode) (function)
- [nvme_version_str](#nvme_version_str) (function)
- [nvme_sq_doorbell](#nvme_sq_doorbell) (function)
- [nvme_cq_doorbell](#nvme_cq_doorbell) (function)
- [nvme_cc_value](#nvme_cc_value) (function)
- [nvme_aqa_value](#nvme_aqa_value) (function)
- [NvmeCmd](#NvmeCmd) (struct)
- [nvme_cmd](#nvme_cmd) (function)
- [nvme_sqe](#nvme_sqe) (function)
- [nvme_sqe_parse](#nvme_sqe_parse) (function)
- [nvme_cmd_identify](#nvme_cmd_identify) (function)
- [nvme_cmd_create_cq](#nvme_cmd_create_cq) (function)
- [nvme_cmd_create_sq](#nvme_cmd_create_sq) (function)
- [nvme_cmd_delete_sq](#nvme_cmd_delete_sq) (function)
- [nvme_cmd_delete_cq](#nvme_cmd_delete_cq) (function)
- [nvme_cmd_set_num_queues](#nvme_cmd_set_num_queues) (function)
- [nvme_cmd_rw](#nvme_cmd_rw) (function)
- [nvme_cmd_flush](#nvme_cmd_flush) (function)
- [nvme_prps](#nvme_prps) (function)
- [nvme_prp_list_bytes](#nvme_prp_list_bytes) (function)
- [NvmeCqe](#NvmeCqe) (struct)
- [nvme_cqe_parse](#nvme_cqe_parse) (function)
- [nvme_cqe](#nvme_cqe) (function)
- [nvme_status_str](#nvme_status_str) (function)
- [NvmeIdCtrl](#NvmeIdCtrl) (struct)
- [nvme_id_ctrl_parse](#nvme_id_ctrl_parse) (function)
- [nvme_max_transfer](#nvme_max_transfer) (function)
- [NvmeIdNs](#NvmeIdNs) (struct)
- [nvme_id_ns_parse](#nvme_id_ns_parse) (function)
- [NvmeBus](#NvmeBus) (struct)
- [NvmeQueue](#NvmeQueue) (struct)
- [nvme_queue](#nvme_queue) (function)
- [nvme_rd](#nvme_rd) (function)
- [nvme_wr](#nvme_wr) (function)
- [nvme_wr64](#nvme_wr64) (function)
- [nvme_sq_full](#nvme_sq_full) (function)
- [nvme_submit](#nvme_submit) (function)
- [nvme_poll](#nvme_poll) (function)
- [nvme_exec](#nvme_exec) (function)
- [nvme_ok](#nvme_ok) (function)
- [NvmeCtrl](#NvmeCtrl) (struct)
- [nvme_init](#nvme_init) (function)
- [nvme_create_io_queues](#nvme_create_io_queues) (function)
- [nvme_rw](#nvme_rw) (function)
- [nvme_blockdev](#nvme_blockdev) (function)

## NVME_REG_CAP:

```fides
pub const NVME_REG_CAP: i64 = 0;
```

*Defined at line 31*

---

## NVME_REG_VS:

```fides
pub const NVME_REG_VS: i64 = 8;
```

*Defined at line 32*

---

## NVME_REG_INTMS:

```fides
pub const NVME_REG_INTMS: i64 = 12;
```

*Defined at line 33*

---

## NVME_REG_INTMC:

```fides
pub const NVME_REG_INTMC: i64 = 16;
```

*Defined at line 34*

---

## NVME_REG_CC:

```fides
pub const NVME_REG_CC: i64 = 20;
```

*Defined at line 35*

---

## NVME_REG_CSTS:

```fides
pub const NVME_REG_CSTS: i64 = 28;
```

*Defined at line 36*

---

## NVME_REG_NSSR:

```fides
pub const NVME_REG_NSSR: i64 = 32;
```

*Defined at line 37*

---

## NVME_REG_AQA:

```fides
pub const NVME_REG_AQA: i64 = 36;
```

*Defined at line 38*

---

## NVME_REG_ASQ:

```fides
pub const NVME_REG_ASQ: i64 = 40;
```

*Defined at line 39*

---

## NVME_REG_ACQ:

```fides
pub const NVME_REG_ACQ: i64 = 48;
```

*Defined at line 40*

---

## NVME_REG_DBS:

```fides
pub const NVME_REG_DBS: i64 = 4096;
```

*Defined at line 41*

---

## NVME_CC_EN:

```fides
pub const NVME_CC_EN: i64 = 1;
```

*Defined at line 43*

---

## NVME_CSTS_RDY:

```fides
pub const NVME_CSTS_RDY: i64 = 1;
```

*Defined at line 44*

---

## NVME_CSTS_CFS:

```fides
pub const NVME_CSTS_CFS: i64 = 2;
```

*Defined at line 45*

---

## NVME_PAGE:

```fides
pub const NVME_PAGE: i64 = 4096;
```

*Defined at line 47*

---

## NVME_SQE_SIZE:

```fides
pub const NVME_SQE_SIZE: i64 = 64;
```

*Defined at line 48*

---

## NVME_CQE_SIZE:

```fides
pub const NVME_CQE_SIZE: i64 = 16;
```

*Defined at line 49*

---

## NVME_POLL_LIMIT:

```fides
pub const NVME_POLL_LIMIT: i64 = 100000;
```

*Defined at line 50*

---

## NVME_ADMIN_DELETE_SQ:

```fides
pub const NVME_ADMIN_DELETE_SQ: i64 = 0;
```

*Defined at line 53*

---

## NVME_ADMIN_CREATE_SQ:

```fides
pub const NVME_ADMIN_CREATE_SQ: i64 = 1;
```

*Defined at line 54*

---

## NVME_ADMIN_GET_LOG_PAGE:

```fides
pub const NVME_ADMIN_GET_LOG_PAGE: i64 = 2;
```

*Defined at line 55*

---

## NVME_ADMIN_DELETE_CQ:

```fides
pub const NVME_ADMIN_DELETE_CQ: i64 = 4;
```

*Defined at line 56*

---

## NVME_ADMIN_CREATE_CQ:

```fides
pub const NVME_ADMIN_CREATE_CQ: i64 = 5;
```

*Defined at line 57*

---

## NVME_ADMIN_IDENTIFY:

```fides
pub const NVME_ADMIN_IDENTIFY: i64 = 6;
```

*Defined at line 58*

---

## NVME_ADMIN_ABORT:

```fides
pub const NVME_ADMIN_ABORT: i64 = 8;
```

*Defined at line 59*

---

## NVME_ADMIN_SET_FEATURES:

```fides
pub const NVME_ADMIN_SET_FEATURES: i64 = 9;
```

*Defined at line 60*

---

## NVME_ADMIN_GET_FEATURES:

```fides
pub const NVME_ADMIN_GET_FEATURES: i64 = 10;
```

*Defined at line 61*

---

## NVME_CMD_FLUSH:

```fides
pub const NVME_CMD_FLUSH: i64 = 0;
```

*Defined at line 63*

---

## NVME_CMD_WRITE:

```fides
pub const NVME_CMD_WRITE: i64 = 1;
```

*Defined at line 64*

---

## NVME_CMD_READ:

```fides
pub const NVME_CMD_READ: i64 = 2;
```

*Defined at line 65*

---

## NVME_CNS_NAMESPACE:

```fides
pub const NVME_CNS_NAMESPACE: i64 = 0;
```

*Defined at line 67*

---

## NVME_CNS_CONTROLLER:

```fides
pub const NVME_CNS_CONTROLLER: i64 = 1;
```

*Defined at line 68*

---

## NVME_CNS_NS_LIST:

```fides
pub const NVME_CNS_NS_LIST: i64 = 2;
```

*Defined at line 69*

---

## NVME_FEAT_NUM_QUEUES:

```fides
pub const NVME_FEAT_NUM_QUEUES: i64 = 7;
```

*Defined at line 71*

---

## NVME_SC_SUCCESS:

```fides
pub const NVME_SC_SUCCESS: i64 = 0;
```

*Defined at line 73*

---

## NVME_SC_INVALID_OPCODE:

```fides
pub const NVME_SC_INVALID_OPCODE: i64 = 1;
```

*Defined at line 74*

---

## NVME_SC_INVALID_FIELD:

```fides
pub const NVME_SC_INVALID_FIELD: i64 = 2;
```

*Defined at line 75*

---

## NVME_SC_LBA_RANGE:

```fides
pub const NVME_SC_LBA_RANGE: i64 = 128;
```

*Defined at line 76*

---

## NvmeCap

```fides
pub struct NvmeCap {
```

**Fields:**

| Name | Type |
|------|------|
| `mqes` | `i64` |
| `cqr` | `bool` |
| `timeout_ms` | `i64` |
| `dstrd` | `i64` |
| `css_nvm` | `bool` |
| `mpsmin` | `i64` |
| `mpsmax` | `i64` |

*Defined at line 78*

---

## nvme_cap_decode

Decode CAP from its two 32-bit halves.

```fides
pub fn nvme_cap_decode(lo: i64, hi: i64) -> NvmeCap {
```

**Parameters:**

- `lo`: `i64`
- `hi`: `i64`

**Returns:** `NvmeCap`

*Defined at line 89*

---

## nvme_version_str

```fides
pub fn nvme_version_str(vs: i64) -> str {
```

**Parameters:**

- `vs`: `i64`

**Returns:** `str`

*Defined at line 95*

---

## nvme_sq_doorbell

The SQ y tail doorbell offset.

```fides
pub fn nvme_sq_doorbell(qid: i64, dstrd: i64) -> i64 {
```

**Parameters:**

- `qid`: `i64`
- `dstrd`: `i64`

**Returns:** `i64`

*Defined at line 104*

---

## nvme_cq_doorbell

The CQ y head doorbell offset.

```fides
pub fn nvme_cq_doorbell(qid: i64, dstrd: i64) -> i64 {
```

**Parameters:**

- `qid`: `i64`
- `dstrd`: `i64`

**Returns:** `i64`

*Defined at line 109*

---

## nvme_cc_value

CC for enabling: NVM command set, 2^(12+mps) pages, 64-byte SQ and
16-byte CQ entries, round robin.

```fides
pub fn nvme_cc_value(mps: i64) -> i64 {
```

**Parameters:**

- `mps`: `i64`

**Returns:** `i64`

*Defined at line 115*

---

## nvme_aqa_value

```fides
pub fn nvme_aqa_value(sq_entries: i64, cq_entries: i64) -> i64 {
```

**Parameters:**

- `sq_entries`: `i64`
- `cq_entries`: `i64`

**Returns:** `i64`

*Defined at line 119*

---

## NvmeCmd

```fides
pub struct NvmeCmd {
```

**Fields:**

| Name | Type |
|------|------|
| `opc` | `i64` |
| `cid` | `i64` |
| `nsid` | `i64` |
| `prp1` | `i64` |
| `prp2` | `i64` |
| `cdw10` | `i64` |
| `cdw11` | `i64` |
| `cdw12` | `i64` |
| `cdw13` | `i64` |
| `cdw14` | `i64` |
| `cdw15` | `i64` |

*Defined at line 157*

---

## nvme_cmd

```fides
pub fn nvme_cmd(opc: i64, nsid: i64) -> NvmeCmd {
```

**Parameters:**

- `opc`: `i64`
- `nsid`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 171*

---

## nvme_sqe

The 64-byte submission queue entry (PRPs, no fused operation).

```fides
pub fn nvme_sqe(c: &NvmeCmd) -> Vec<i64> {
```

**Parameters:**

- `c`: `&NvmeCmd`

**Returns:** `Vec<i64>`

*Defined at line 178*

---

## nvme_sqe_parse

```fides
pub fn nvme_sqe_parse(e: &Vec<i64>, o: i64) -> NvmeCmd {
```

**Parameters:**

- `e`: `&Vec<i64>`
- `o`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 193*

---

## nvme_cmd_identify

```fides
pub fn nvme_cmd_identify(cns: i64, nsid: i64, buf: i64) -> NvmeCmd {
```

**Parameters:**

- `cns`: `i64`
- `nsid`: `i64`
- `buf`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 200*

---

## nvme_cmd_create_cq

Create I/O Completion Queue `qid` of `entries` at `base`, physically
contiguous, interrupts on vector `iv` when `ien`.

```fides
pub fn nvme_cmd_create_cq(qid: i64, entries: i64, base: i64, iv: i64, ien: bool) -> NvmeCmd {
```

**Parameters:**

- `qid`: `i64`
- `entries`: `i64`
- `base`: `i64`
- `iv`: `i64`
- `ien`: `bool`

**Returns:** `NvmeCmd`

*Defined at line 209*

---

## nvme_cmd_create_sq

Create I/O Submission Queue `qid` of `entries` at `base` completing
into `cqid`, priority `prio` (0 urgent .. 3 low).

```fides
pub fn nvme_cmd_create_sq(qid: i64, entries: i64, base: i64, cqid: i64, prio: i64) -> NvmeCmd {
```

**Parameters:**

- `qid`: `i64`
- `entries`: `i64`
- `base`: `i64`
- `cqid`: `i64`
- `prio`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 222*

---

## nvme_cmd_delete_sq

```fides
pub fn nvme_cmd_delete_sq(qid: i64) -> NvmeCmd {
```

**Parameters:**

- `qid`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 230*

---

## nvme_cmd_delete_cq

```fides
pub fn nvme_cmd_delete_cq(qid: i64) -> NvmeCmd {
```

**Parameters:**

- `qid`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 236*

---

## nvme_cmd_set_num_queues

Set Features: Number of Queues (counts are 1-based here, 0-based on
the wire).

```fides
pub fn nvme_cmd_set_num_queues(nsq: i64, ncq: i64) -> NvmeCmd {
```

**Parameters:**

- `nsq`: `i64`
- `ncq`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 244*

---

## nvme_cmd_rw

Read or Write of `nlb` blocks (1..65536) at `slba`.

```fides
pub fn nvme_cmd_rw(write: bool, nsid: i64, slba: i64, nlb: i64, prp1: i64, prp2: i64) -> NvmeCmd {
```

**Parameters:**

- `write`: `bool`
- `nsid`: `i64`
- `slba`: `i64`
- `nlb`: `i64`
- `prp1`: `i64`
- `prp2`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 252*

---

## nvme_cmd_flush

```fides
pub fn nvme_cmd_flush(nsid: i64) -> NvmeCmd {
```

**Parameters:**

- `nsid`: `i64`

**Returns:** `NvmeCmd`

*Defined at line 266*

---

## nvme_prps

The pages a buffer touches: [prp1, prp2, list entries...]. prp2 is 0
for one page, the second page for two, and for more the address
`list` where the remaining page addresses must be written (one page
of list holds 512; longer lists are chained through their last entry,
which this layout also does).

```fides
pub fn nvme_prps(buf: i64, nbytes: i64, list: i64) -> Vec<i64> {
```

**Parameters:**

- `buf`: `i64`
- `nbytes`: `i64`
- `list`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 277*

---

## nvme_prp_list_bytes

The PRP list bytes for `nvme_prps` (entries after the first two).

```fides
pub fn nvme_prp_list_bytes(prps: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `prps`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 322*

---

## NvmeCqe

```fides
pub struct NvmeCqe {
```

**Fields:**

| Name | Type |
|------|------|
| `dw0` | `i64` |
| `sqhd` | `i64` |
| `sqid` | `i64` |
| `cid` | `i64` |
| `phase` | `i64` |
| `sc` | `i64` |
| `sct` | `i64` |
| `dnr` | `bool` |

*Defined at line 339*

---

## nvme_cqe_parse

```fides
pub fn nvme_cqe_parse(e: &Vec<i64>, o: i64) -> NvmeCqe {
```

**Parameters:**

- `e`: `&Vec<i64>`
- `o`: `i64`

**Returns:** `NvmeCqe`

*Defined at line 350*

---

## nvme_cqe

A completion entry (what a controller writes).

```fides
pub fn nvme_cqe(dw0: i64, sqhd: i64, sqid: i64, cid: i64, phase: i64, sc: i64, sct: i64) -> Vec<i64> {
```

**Parameters:**

- `dw0`: `i64`
- `sqhd`: `i64`
- `sqid`: `i64`
- `cid`: `i64`
- `phase`: `i64`
- `sc`: `i64`
- `sct`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 359*

---

## nvme_status_str

```fides
pub fn nvme_status_str(c: &NvmeCqe) -> str {
```

**Parameters:**

- `c`: `&NvmeCqe`

**Returns:** `str`

*Defined at line 367*

---

## NvmeIdCtrl

```fides
pub struct NvmeIdCtrl {
```

**Fields:**

| Name | Type |
|------|------|
| `vid` | `i64` |
| `serial` | `str` |
| `model` | `str` |
| `firmware` | `str` |
| `mdts` | `i64` |
| `cntlid` | `i64` |
| `version` | `i64` |
| `sqes` | `i64` |
| `cqes` | `i64` |
| `nn` | `i64` |

*Defined at line 398*

---

## nvme_id_ctrl_parse

```fides
pub fn nvme_id_ctrl_parse(d: &Vec<i64>) -> NvmeIdCtrl {
```

**Parameters:**

- `d`: `&Vec<i64>`

**Returns:** `NvmeIdCtrl`

*Defined at line 421*

---

## nvme_max_transfer

Largest transfer in bytes (MDTS; 0 means no limit, given as -1).

```fides
pub fn nvme_max_transfer(id: &NvmeIdCtrl, mpsmin: i64) -> i64 {
```

**Parameters:**

- `id`: `&NvmeIdCtrl`
- `mpsmin`: `i64`

**Returns:** `i64`

*Defined at line 428*

---

## NvmeIdNs

```fides
pub struct NvmeIdNs {
```

**Fields:**

| Name | Type |
|------|------|
| `nsze` | `i64` |
| `ncap` | `i64` |
| `nuse` | `i64` |
| `nlbaf` | `i64` |
| `flbas` | `i64` |
| `lba_size` | `i64` |
| `ms` | `i64` |

*Defined at line 435*

---

## nvme_id_ns_parse

```fides
pub fn nvme_id_ns_parse(d: &Vec<i64>) -> NvmeIdNs {
```

**Parameters:**

- `d`: `&Vec<i64>`

**Returns:** `NvmeIdNs`

*Defined at line 445*

---

## NvmeBus

```fides
pub struct NvmeBus {
```

**Fields:**

| Name | Type |
|------|------|
| `mmio_rd` | `fn(i64) -> i64` |
| `mmio_wr` | `fn(i64, i64) -> i64` |
| `mem_rd` | `fn(i64, i64) -> Vec<i64>` |
| `mem_wr` | `fn(i64, Vec<i64>) -> i64` |

*Defined at line 455*

---

## NvmeQueue

```fides
pub struct NvmeQueue {
```

**Fields:**

| Name | Type |
|------|------|
| `qid` | `i64` |
| `sq` | `i64` |
| `cq` | `i64` |
| `size` | `i64` |
| `sq_tail` | `i64` |
| `sq_head` | `i64` |
| `cq_head` | `i64` |
| `phase` | `i64` |
| `next_cid` | `i64` |
| `dstrd` | `i64` |

*Defined at line 462*

---

## nvme_queue

```fides
pub fn nvme_queue(qid: i64, sq: i64, cq: i64, size: i64, dstrd: i64) -> NvmeQueue {
```

**Parameters:**

- `qid`: `i64`
- `sq`: `i64`
- `cq`: `i64`
- `size`: `i64`
- `dstrd`: `i64`

**Returns:** `NvmeQueue`

*Defined at line 475*

---

## nvme_rd

```fides
pub fn nvme_rd(bus: &NvmeBus, off: i64) -> i64 {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 481*

---

## nvme_wr

```fides
pub fn nvme_wr(bus: &NvmeBus, off: i64, v: i64) {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 486*

---

## nvme_wr64

```fides
pub fn nvme_wr64(bus: &NvmeBus, off: i64, v: i64) {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 491*

---

## nvme_sq_full

Is the submission queue full (one slot always stays empty)?

```fides
pub fn nvme_sq_full(q: &NvmeQueue) -> bool {
```

**Parameters:**

- `q`: `&NvmeQueue`

**Returns:** `bool`

*Defined at line 507*

---

## nvme_submit

Put `c` on the queue with the next command id and ring the doorbell.
Returns the command id, or -EBUSY when the queue is full.

```fides
pub fn nvme_submit(bus: &NvmeBus, q: &mut NvmeQueue, cmd: NvmeCmd) -> i64 {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `q`: `&mut NvmeQueue`
- `cmd`: `NvmeCmd`

**Returns:** `i64`

*Defined at line 513*

---

## nvme_poll

Take one completion if the controller posted one (its phase tag equals
the queue's current phase): cid -1 when there is none.

```fides
pub fn nvme_poll(bus: &NvmeBus, q: &mut NvmeQueue) -> NvmeCqe {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `q`: `&mut NvmeQueue`

**Returns:** `NvmeCqe`

*Defined at line 528*

---

## nvme_exec

Submit and wait for that command's completion.

```fides
pub fn nvme_exec(bus: &NvmeBus, q: &mut NvmeQueue, cmd: NvmeCmd) -> NvmeCqe {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `q`: `&mut NvmeQueue`
- `cmd`: `NvmeCmd`

**Returns:** `NvmeCqe`

*Defined at line 546*

---

## nvme_ok

```fides
pub fn nvme_ok(c: &NvmeCqe) -> bool {
```

**Parameters:**

- `c`: `&NvmeCqe`

**Returns:** `bool`

*Defined at line 565*

---

## NvmeCtrl

```fides
pub struct NvmeCtrl {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `cap` | `NvmeCap` |
| `version` | `i64` |
| `admin` | `NvmeQueue` |

*Defined at line 569*

---

## nvme_init

Reset and enable the controller with an admin queue pair of `entries`
entries at `asq` and `acq` (page aligned).

```fides
pub fn nvme_init(bus: &NvmeBus, asq: i64, acq: i64, entries: i64) -> NvmeCtrl {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `asq`: `i64`
- `acq`: `i64`
- `entries`: `i64`

**Returns:** `NvmeCtrl`

*Defined at line 593*

---

## nvme_create_io_queues

Create the I/O queue pair `qid` (completion queue first, as required).

```fides
pub fn nvme_create_io_queues(bus: &NvmeBus, ctl: &mut NvmeCtrl, qid: i64, sq: i64, cq: i64, entries: i64) -> NvmeQueue {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `ctl`: `&mut NvmeCtrl`
- `qid`: `i64`
- `sq`: `i64`
- `cq`: `i64`
- `entries`: `i64`

**Returns:** `NvmeQueue`

*Defined at line 630*

---

## nvme_rw

Read or write `nlb` blocks between `slba` and the buffer `buf`; `list`
is a page for the PRP list when the buffer spans more than two pages.

```fides
pub fn nvme_rw(bus: &NvmeBus, q: &mut NvmeQueue, write: bool, nsid: i64, slba: i64, nlb: i64, lba_size: i64, buf: i64, list: i64) -> NvmeCqe {
```

**Parameters:**

- `bus`: `&NvmeBus`
- `q`: `&mut NvmeQueue`
- `write`: `bool`
- `nsid`: `i64`
- `slba`: `i64`
- `nlb`: `i64`
- `lba_size`: `i64`
- `buf`: `i64`
- `list`: `i64`

**Returns:** `NvmeCqe`

*Defined at line 649*

---

## nvme_blockdev

A BlockDev over namespace `nsid` through I/O queue `q`; `buf` is two
pages of DMA memory (a bounce page and a PRP list page).

```fides
pub fn nvme_blockdev(bus: NvmeBus, q: NvmeQueue, nsid: i64, ns: &NvmeIdNs, buf: i64) -> BlockDev {
```

**Parameters:**

- `bus`: `NvmeBus`
- `q`: `NvmeQueue`
- `nsid`: `i64`
- `ns`: `&NvmeIdNs`
- `buf`: `i64`

**Returns:** `BlockDev`

*Defined at line 705*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
