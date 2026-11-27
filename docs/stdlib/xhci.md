[All modules](index.md)

# xhci -- Fides Documentation

## Contents

- [XHCI_CAPLENGTH:](#XHCI_CAPLENGTH:) (const)
- [XHCI_HCIVERSION:](#XHCI_HCIVERSION:) (const)
- [XHCI_HCSPARAMS1:](#XHCI_HCSPARAMS1:) (const)
- [XHCI_HCSPARAMS2:](#XHCI_HCSPARAMS2:) (const)
- [XHCI_HCSPARAMS3:](#XHCI_HCSPARAMS3:) (const)
- [XHCI_HCCPARAMS1:](#XHCI_HCCPARAMS1:) (const)
- [XHCI_DBOFF:](#XHCI_DBOFF:) (const)
- [XHCI_RTSOFF:](#XHCI_RTSOFF:) (const)
- [XHCI_HCCPARAMS2:](#XHCI_HCCPARAMS2:) (const)
- [XHCI_USBCMD:](#XHCI_USBCMD:) (const)
- [XHCI_USBSTS:](#XHCI_USBSTS:) (const)
- [XHCI_PAGESIZE:](#XHCI_PAGESIZE:) (const)
- [XHCI_DNCTRL:](#XHCI_DNCTRL:) (const)
- [XHCI_CRCR:](#XHCI_CRCR:) (const)
- [XHCI_DCBAAP:](#XHCI_DCBAAP:) (const)
- [XHCI_CONFIG:](#XHCI_CONFIG:) (const)
- [XHCI_PORTS:](#XHCI_PORTS:) (const)
- [USBCMD_RS:](#USBCMD_RS:) (const)
- [USBCMD_HCRST:](#USBCMD_HCRST:) (const)
- [USBCMD_INTE:](#USBCMD_INTE:) (const)
- [USBCMD_HSEE:](#USBCMD_HSEE:) (const)
- [USBSTS_HCH:](#USBSTS_HCH:) (const)
- [USBSTS_HSE:](#USBSTS_HSE:) (const)
- [USBSTS_EINT:](#USBSTS_EINT:) (const)
- [USBSTS_PCD:](#USBSTS_PCD:) (const)
- [USBSTS_CNR:](#USBSTS_CNR:) (const)
- [USBSTS_HCE:](#USBSTS_HCE:) (const)
- [PORTSC_CCS:](#PORTSC_CCS:) (const)
- [PORTSC_PED:](#PORTSC_PED:) (const)
- [PORTSC_OCA:](#PORTSC_OCA:) (const)
- [PORTSC_PR:](#PORTSC_PR:) (const)
- [PORTSC_PP:](#PORTSC_PP:) (const)
- [PORTSC_LWS:](#PORTSC_LWS:) (const)
- [PORTSC_CSC:](#PORTSC_CSC:) (const)
- [PORTSC_PEC:](#PORTSC_PEC:) (const)
- [PORTSC_WRC:](#PORTSC_WRC:) (const)
- [PORTSC_OCC:](#PORTSC_OCC:) (const)
- [PORTSC_PRC:](#PORTSC_PRC:) (const)
- [PORTSC_PLC:](#PORTSC_PLC:) (const)
- [PORTSC_CEC:](#PORTSC_CEC:) (const)
- [PORTSC_WPR:](#PORTSC_WPR:) (const)
- [PORTSC_RW1C:](#PORTSC_RW1C:) (const)
- [XHCI_SPEED_FULL:](#XHCI_SPEED_FULL:) (const)
- [XHCI_SPEED_LOW:](#XHCI_SPEED_LOW:) (const)
- [XHCI_SPEED_HIGH:](#XHCI_SPEED_HIGH:) (const)
- [XHCI_SPEED_SUPER:](#XHCI_SPEED_SUPER:) (const)
- [XHCI_SPEED_SUPER_PLUS:](#XHCI_SPEED_SUPER_PLUS:) (const)
- [TRB_NORMAL:](#TRB_NORMAL:) (const)
- [TRB_SETUP:](#TRB_SETUP:) (const)
- [TRB_DATA:](#TRB_DATA:) (const)
- [TRB_STATUS:](#TRB_STATUS:) (const)
- [TRB_ISOCH:](#TRB_ISOCH:) (const)
- [TRB_LINK:](#TRB_LINK:) (const)
- [TRB_EVENT_DATA:](#TRB_EVENT_DATA:) (const)
- [TRB_NOOP:](#TRB_NOOP:) (const)
- [TRB_ENABLE_SLOT:](#TRB_ENABLE_SLOT:) (const)
- [TRB_DISABLE_SLOT:](#TRB_DISABLE_SLOT:) (const)
- [TRB_ADDRESS_DEVICE:](#TRB_ADDRESS_DEVICE:) (const)
- [TRB_CONFIGURE_EP:](#TRB_CONFIGURE_EP:) (const)
- [TRB_EVALUATE_CTX:](#TRB_EVALUATE_CTX:) (const)
- [TRB_RESET_EP:](#TRB_RESET_EP:) (const)
- [TRB_STOP_EP:](#TRB_STOP_EP:) (const)
- [TRB_SET_TR_DEQ:](#TRB_SET_TR_DEQ:) (const)
- [TRB_RESET_DEVICE:](#TRB_RESET_DEVICE:) (const)
- [TRB_NOOP_CMD:](#TRB_NOOP_CMD:) (const)
- [TRB_TRANSFER_EVENT:](#TRB_TRANSFER_EVENT:) (const)
- [TRB_CMD_COMPLETION:](#TRB_CMD_COMPLETION:) (const)
- [TRB_PORT_STATUS:](#TRB_PORT_STATUS:) (const)
- [TRB_HC_EVENT:](#TRB_HC_EVENT:) (const)
- [TRB_CYCLE:](#TRB_CYCLE:) (const)
- [TRB_TC:](#TRB_TC:) (const)
- [TRB_ENT:](#TRB_ENT:) (const)
- [TRB_ISP:](#TRB_ISP:) (const)
- [TRB_CH:](#TRB_CH:) (const)
- [TRB_IOC:](#TRB_IOC:) (const)
- [TRB_IDT:](#TRB_IDT:) (const)
- [TRB_DIR_IN:](#TRB_DIR_IN:) (const)
- [CC_SUCCESS:](#CC_SUCCESS:) (const)
- [CC_DATA_BUFFER:](#CC_DATA_BUFFER:) (const)
- [CC_BABBLE:](#CC_BABBLE:) (const)
- [CC_TRANSACTION:](#CC_TRANSACTION:) (const)
- [CC_TRB:](#CC_TRB:) (const)
- [CC_STALL:](#CC_STALL:) (const)
- [CC_NO_SLOTS:](#CC_NO_SLOTS:) (const)
- [CC_SHORT_PACKET:](#CC_SHORT_PACKET:) (const)
- [CC_PARAMETER:](#CC_PARAMETER:) (const)
- [CC_CONTEXT_STATE:](#CC_CONTEXT_STATE:) (const)
- [CC_STOPPED:](#CC_STOPPED:) (const)
- [xhci_cc_name](#xhci_cc_name) (function)
- [Trb](#Trb) (struct)
- [trb_type](#trb_type) (function)
- [trb_cycle](#trb_cycle) (function)
- [trb_cc](#trb_cc) (function)
- [trb_slot](#trb_slot) (function)
- [trb_ep](#trb_ep) (function)
- [trb_residual](#trb_residual) (function)
- [trb_port](#trb_port) (function)
- [trb_normal](#trb_normal) (function)
- [trb_setup](#trb_setup) (function)
- [trb_data](#trb_data) (function)
- [trb_status](#trb_status) (function)
- [trb_link](#trb_link) (function)
- [trb_noop_cmd](#trb_noop_cmd) (function)
- [trb_enable_slot](#trb_enable_slot) (function)
- [trb_disable_slot](#trb_disable_slot) (function)
- [trb_address_device](#trb_address_device) (function)
- [trb_configure_ep](#trb_configure_ep) (function)
- [trb_evaluate_ctx](#trb_evaluate_ctx) (function)
- [trb_reset_ep](#trb_reset_ep) (function)
- [trb_stop_ep](#trb_stop_ep) (function)
- [trb_set_tr_deq](#trb_set_tr_deq) (function)
- [trb_reset_device](#trb_reset_device) (function)
- [xhci_trb_write](#xhci_trb_write) (function)
- [xhci_trb_read](#xhci_trb_read) (function)
- [XhciRing](#XhciRing) (struct)
- [xhci_ring_new](#xhci_ring_new) (function)
- [xhci_ring_space](#xhci_ring_space) (function)
- [xhci_ring_enq_ptr](#xhci_ring_enq_ptr) (function)
- [xhci_ring_push](#xhci_ring_push) (function)
- [xhci_ring_consumed](#xhci_ring_consumed) (function)
- [xhci_crcr](#xhci_crcr) (function)
- [XhciEventRing](#XhciEventRing) (struct)
- [xhci_event_ring_new](#xhci_event_ring_new) (function)
- [xhci_event_ptr](#xhci_event_ptr) (function)
- [xhci_event_pending](#xhci_event_pending) (function)
- [xhci_event_pop](#xhci_event_pop) (function)
- [xhci_erdp](#xhci_erdp) (function)
- [EP_ISOCH_OUT:](#EP_ISOCH_OUT:) (const)
- [EP_BULK_OUT:](#EP_BULK_OUT:) (const)
- [EP_INTR_OUT:](#EP_INTR_OUT:) (const)
- [EP_CONTROL:](#EP_CONTROL:) (const)
- [EP_ISOCH_IN:](#EP_ISOCH_IN:) (const)
- [EP_BULK_IN:](#EP_BULK_IN:) (const)
- [EP_INTR_IN:](#EP_INTR_IN:) (const)
- [xhci_dci](#xhci_dci) (function)
- [xhci_ep_type](#xhci_ep_type) (function)
- [xhci_ep0_max_packet](#xhci_ep0_max_packet) (function)
- [xhci_slot_ctx](#xhci_slot_ctx) (function)
- [xhci_ep_ctx](#xhci_ep_ctx) (function)
- [xhci_ctx_size](#xhci_ctx_size) (function)
- [xhci_input_ctx_address](#xhci_input_ctx_address) (function)
- [xhci_ctx_dword](#xhci_ctx_dword) (function)
- [XhciBus](#XhciBus) (struct)
- [xhci_rd](#xhci_rd) (function)
- [xhci_wr](#xhci_wr) (function)
- [xhci_wr64](#xhci_wr64) (function)
- [xhci_rd64](#xhci_rd64) (function)
- [XhciCaps](#XhciCaps) (struct)
- [xhci_caps](#xhci_caps) (function)
- [xhci_portsc_off](#xhci_portsc_off) (function)
- [xhci_ir_off](#xhci_ir_off) (function)
- [xhci_db_off](#xhci_db_off) (function)
- [xhci_ring_doorbell](#xhci_ring_doorbell) (function)
- [xhci_reset](#xhci_reset) (function)
- [xhci_start](#xhci_start) (function)
- [PortStatus](#PortStatus) (struct)
- [xhci_port_decode](#xhci_port_decode) (function)
- [xhci_portsc_preserve](#xhci_portsc_preserve) (function)
- [xhci_port_reset](#xhci_port_reset) (function)
- [xhci_port_ack](#xhci_port_ack) (function)
- [XECP_LEGACY:](#XECP_LEGACY:) (const)
- [XECP_PROTOCOL:](#XECP_PROTOCOL:) (const)
- [xhci_ext_caps](#xhci_ext_caps) (function)
- [xhci_legacy_handoff](#xhci_legacy_handoff) (function)
- [XhciProtocol](#XhciProtocol) (struct)
- [xhci_protocol](#xhci_protocol) (function)

## XHCI_CAPLENGTH:

```fides
pub const XHCI_CAPLENGTH: i64 = 0x00;
```

*Defined at line 31*

---

## XHCI_HCIVERSION:

```fides
pub const XHCI_HCIVERSION: i64 = 0x02;
```

*Defined at line 32*

---

## XHCI_HCSPARAMS1:

```fides
pub const XHCI_HCSPARAMS1: i64 = 0x04;
```

*Defined at line 33*

---

## XHCI_HCSPARAMS2:

```fides
pub const XHCI_HCSPARAMS2: i64 = 0x08;
```

*Defined at line 34*

---

## XHCI_HCSPARAMS3:

```fides
pub const XHCI_HCSPARAMS3: i64 = 0x0C;
```

*Defined at line 35*

---

## XHCI_HCCPARAMS1:

```fides
pub const XHCI_HCCPARAMS1: i64 = 0x10;
```

*Defined at line 36*

---

## XHCI_DBOFF:

```fides
pub const XHCI_DBOFF: i64 = 0x14;
```

*Defined at line 37*

---

## XHCI_RTSOFF:

```fides
pub const XHCI_RTSOFF: i64 = 0x18;
```

*Defined at line 38*

---

## XHCI_HCCPARAMS2:

```fides
pub const XHCI_HCCPARAMS2: i64 = 0x1C;
```

*Defined at line 39*

---

## XHCI_USBCMD:

```fides
pub const XHCI_USBCMD: i64 = 0x00;
```

*Defined at line 42*

---

## XHCI_USBSTS:

```fides
pub const XHCI_USBSTS: i64 = 0x04;
```

*Defined at line 43*

---

## XHCI_PAGESIZE:

```fides
pub const XHCI_PAGESIZE: i64 = 0x08;
```

*Defined at line 44*

---

## XHCI_DNCTRL:

```fides
pub const XHCI_DNCTRL: i64 = 0x14;
```

*Defined at line 45*

---

## XHCI_CRCR:

```fides
pub const XHCI_CRCR: i64 = 0x18;
```

*Defined at line 46*

---

## XHCI_DCBAAP:

```fides
pub const XHCI_DCBAAP: i64 = 0x30;
```

*Defined at line 47*

---

## XHCI_CONFIG:

```fides
pub const XHCI_CONFIG: i64 = 0x38;
```

*Defined at line 48*

---

## XHCI_PORTS:

```fides
pub const XHCI_PORTS: i64 = 0x400;
```

*Defined at line 49*

---

## USBCMD_RS:

```fides
pub const USBCMD_RS: i64 = 0x1;
```

*Defined at line 51*

---

## USBCMD_HCRST:

```fides
pub const USBCMD_HCRST: i64 = 0x2;
```

*Defined at line 52*

---

## USBCMD_INTE:

```fides
pub const USBCMD_INTE: i64 = 0x4;
```

*Defined at line 53*

---

## USBCMD_HSEE:

```fides
pub const USBCMD_HSEE: i64 = 0x8;
```

*Defined at line 54*

---

## USBSTS_HCH:

```fides
pub const USBSTS_HCH: i64 = 0x1;
```

*Defined at line 56*

---

## USBSTS_HSE:

```fides
pub const USBSTS_HSE: i64 = 0x4;
```

*Defined at line 57*

---

## USBSTS_EINT:

```fides
pub const USBSTS_EINT: i64 = 0x8;
```

*Defined at line 58*

---

## USBSTS_PCD:

```fides
pub const USBSTS_PCD: i64 = 0x10;
```

*Defined at line 59*

---

## USBSTS_CNR:

```fides
pub const USBSTS_CNR: i64 = 0x800;
```

*Defined at line 60*

---

## USBSTS_HCE:

```fides
pub const USBSTS_HCE: i64 = 0x1000;
```

*Defined at line 61*

---

## PORTSC_CCS:

```fides
pub const PORTSC_CCS: i64 = 0x1;
```

*Defined at line 64*

---

## PORTSC_PED:

```fides
pub const PORTSC_PED: i64 = 0x2;
```

*Defined at line 65*

---

## PORTSC_OCA:

```fides
pub const PORTSC_OCA: i64 = 0x8;
```

*Defined at line 66*

---

## PORTSC_PR:

```fides
pub const PORTSC_PR: i64 = 0x10;
```

*Defined at line 67*

---

## PORTSC_PP:

```fides
pub const PORTSC_PP: i64 = 0x200;
```

*Defined at line 68*

---

## PORTSC_LWS:

```fides
pub const PORTSC_LWS: i64 = 0x10000;
```

*Defined at line 69*

---

## PORTSC_CSC:

```fides
pub const PORTSC_CSC: i64 = 0x20000;
```

*Defined at line 70*

---

## PORTSC_PEC:

```fides
pub const PORTSC_PEC: i64 = 0x40000;
```

*Defined at line 71*

---

## PORTSC_WRC:

```fides
pub const PORTSC_WRC: i64 = 0x80000;
```

*Defined at line 72*

---

## PORTSC_OCC:

```fides
pub const PORTSC_OCC: i64 = 0x100000;
```

*Defined at line 73*

---

## PORTSC_PRC:

```fides
pub const PORTSC_PRC: i64 = 0x200000;
```

*Defined at line 74*

---

## PORTSC_PLC:

```fides
pub const PORTSC_PLC: i64 = 0x400000;
```

*Defined at line 75*

---

## PORTSC_CEC:

```fides
pub const PORTSC_CEC: i64 = 0x800000;
```

*Defined at line 76*

---

## PORTSC_WPR:

```fides
pub const PORTSC_WPR: i64 = 0x80000000;
```

*Defined at line 77*

---

## PORTSC_RW1C:

Write-1-to-clear change bits (and PED, which a 1 would disable).

```fides
pub const PORTSC_RW1C: i64 = 0x00FE0002;
```

*Defined at line 79*

---

## XHCI_SPEED_FULL:

```fides
pub const XHCI_SPEED_FULL: i64 = 1;
```

*Defined at line 82*

---

## XHCI_SPEED_LOW:

```fides
pub const XHCI_SPEED_LOW: i64 = 2;
```

*Defined at line 83*

---

## XHCI_SPEED_HIGH:

```fides
pub const XHCI_SPEED_HIGH: i64 = 3;
```

*Defined at line 84*

---

## XHCI_SPEED_SUPER:

```fides
pub const XHCI_SPEED_SUPER: i64 = 4;
```

*Defined at line 85*

---

## XHCI_SPEED_SUPER_PLUS:

```fides
pub const XHCI_SPEED_SUPER_PLUS: i64 = 5;
```

*Defined at line 86*

---

## TRB_NORMAL:

```fides
pub const TRB_NORMAL: i64 = 1;
```

*Defined at line 90*

---

## TRB_SETUP:

```fides
pub const TRB_SETUP: i64 = 2;
```

*Defined at line 91*

---

## TRB_DATA:

```fides
pub const TRB_DATA: i64 = 3;
```

*Defined at line 92*

---

## TRB_STATUS:

```fides
pub const TRB_STATUS: i64 = 4;
```

*Defined at line 93*

---

## TRB_ISOCH:

```fides
pub const TRB_ISOCH: i64 = 5;
```

*Defined at line 94*

---

## TRB_LINK:

```fides
pub const TRB_LINK: i64 = 6;
```

*Defined at line 95*

---

## TRB_EVENT_DATA:

```fides
pub const TRB_EVENT_DATA: i64 = 7;
```

*Defined at line 96*

---

## TRB_NOOP:

```fides
pub const TRB_NOOP: i64 = 8;
```

*Defined at line 97*

---

## TRB_ENABLE_SLOT:

```fides
pub const TRB_ENABLE_SLOT: i64 = 9;
```

*Defined at line 98*

---

## TRB_DISABLE_SLOT:

```fides
pub const TRB_DISABLE_SLOT: i64 = 10;
```

*Defined at line 99*

---

## TRB_ADDRESS_DEVICE:

```fides
pub const TRB_ADDRESS_DEVICE: i64 = 11;
```

*Defined at line 100*

---

## TRB_CONFIGURE_EP:

```fides
pub const TRB_CONFIGURE_EP: i64 = 12;
```

*Defined at line 101*

---

## TRB_EVALUATE_CTX:

```fides
pub const TRB_EVALUATE_CTX: i64 = 13;
```

*Defined at line 102*

---

## TRB_RESET_EP:

```fides
pub const TRB_RESET_EP: i64 = 14;
```

*Defined at line 103*

---

## TRB_STOP_EP:

```fides
pub const TRB_STOP_EP: i64 = 15;
```

*Defined at line 104*

---

## TRB_SET_TR_DEQ:

```fides
pub const TRB_SET_TR_DEQ: i64 = 16;
```

*Defined at line 105*

---

## TRB_RESET_DEVICE:

```fides
pub const TRB_RESET_DEVICE: i64 = 17;
```

*Defined at line 106*

---

## TRB_NOOP_CMD:

```fides
pub const TRB_NOOP_CMD: i64 = 23;
```

*Defined at line 107*

---

## TRB_TRANSFER_EVENT:

```fides
pub const TRB_TRANSFER_EVENT: i64 = 32;
```

*Defined at line 108*

---

## TRB_CMD_COMPLETION:

```fides
pub const TRB_CMD_COMPLETION: i64 = 33;
```

*Defined at line 109*

---

## TRB_PORT_STATUS:

```fides
pub const TRB_PORT_STATUS: i64 = 34;
```

*Defined at line 110*

---

## TRB_HC_EVENT:

```fides
pub const TRB_HC_EVENT: i64 = 37;
```

*Defined at line 111*

---

## TRB_CYCLE:

```fides
pub const TRB_CYCLE: i64 = 0x1;
```

*Defined at line 113*

---

## TRB_TC:

```fides
pub const TRB_TC: i64 = 0x2;
```

*Defined at line 114*

---

## TRB_ENT:

```fides
pub const TRB_ENT: i64 = 0x2;
```

*Defined at line 116*

---

## TRB_ISP:

```fides
pub const TRB_ISP: i64 = 0x4;
```

*Defined at line 117*

---

## TRB_CH:

```fides
pub const TRB_CH: i64 = 0x10;
```

*Defined at line 118*

---

## TRB_IOC:

```fides
pub const TRB_IOC: i64 = 0x20;
```

*Defined at line 119*

---

## TRB_IDT:

```fides
pub const TRB_IDT: i64 = 0x40;
```

*Defined at line 120*

---

## TRB_DIR_IN:

```fides
pub const TRB_DIR_IN: i64 = 0x10000;
```

*Defined at line 121*

---

## CC_SUCCESS:

```fides
pub const CC_SUCCESS: i64 = 1;
```

*Defined at line 123*

---

## CC_DATA_BUFFER:

```fides
pub const CC_DATA_BUFFER: i64 = 2;
```

*Defined at line 124*

---

## CC_BABBLE:

```fides
pub const CC_BABBLE: i64 = 3;
```

*Defined at line 125*

---

## CC_TRANSACTION:

```fides
pub const CC_TRANSACTION: i64 = 4;
```

*Defined at line 126*

---

## CC_TRB:

```fides
pub const CC_TRB: i64 = 5;
```

*Defined at line 127*

---

## CC_STALL:

```fides
pub const CC_STALL: i64 = 6;
```

*Defined at line 128*

---

## CC_NO_SLOTS:

```fides
pub const CC_NO_SLOTS: i64 = 9;
```

*Defined at line 129*

---

## CC_SHORT_PACKET:

```fides
pub const CC_SHORT_PACKET: i64 = 13;
```

*Defined at line 130*

---

## CC_PARAMETER:

```fides
pub const CC_PARAMETER: i64 = 17;
```

*Defined at line 131*

---

## CC_CONTEXT_STATE:

```fides
pub const CC_CONTEXT_STATE: i64 = 19;
```

*Defined at line 132*

---

## CC_STOPPED:

```fides
pub const CC_STOPPED: i64 = 26;
```

*Defined at line 133*

---

## xhci_cc_name

```fides
pub fn xhci_cc_name(cc: i64) -> str {
```

**Parameters:**

- `cc`: `i64`

**Returns:** `str`

*Defined at line 135*

---

## Trb

```fides
pub struct Trb {
```

**Fields:**

| Name | Type |
|------|------|
| `param` | `i64` |
| `status` | `i64` |
| `control` | `i64` |

*Defined at line 201*

---

## trb_type

```fides
pub fn trb_type(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 208*

---

## trb_cycle

```fides
pub fn trb_cycle(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 212*

---

## trb_cc

Completion code of an event TRB.

```fides
pub fn trb_cc(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 217*

---

## trb_slot

Slot id of an event or command TRB.

```fides
pub fn trb_slot(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 222*

---

## trb_ep

Endpoint id (DCI) of a transfer event.

```fides
pub fn trb_ep(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 227*

---

## trb_residual

Bytes not transferred, from a transfer event.

```fides
pub fn trb_residual(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 232*

---

## trb_port

Port id of a port status change event.

```fides
pub fn trb_port(t: &Trb) -> i64 {
```

**Parameters:**

- `t`: `&Trb`

**Returns:** `i64`

*Defined at line 237*

---

## trb_normal

Normal TRB (bulk / interrupt data).

```fides
pub fn trb_normal(buf: i64, len: i64, td_size: i64, flags: i64) -> Trb {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`
- `td_size`: `i64`
- `flags`: `i64`

**Returns:** `Trb`

*Defined at line 246*

---

## trb_setup

```fides
pub fn trb_setup(bm_request_type: i64, b_request: i64, w_value: i64, w_index: i64, w_length: i64, trt: i64) -> Trb {
```

**Parameters:**

- `bm_request_type`: `i64`
- `b_request`: `i64`
- `w_value`: `i64`
- `w_index`: `i64`
- `w_length`: `i64`
- `trt`: `i64`

**Returns:** `Trb`

*Defined at line 255*

---

## trb_data

```fides
pub fn trb_data(buf: i64, len: i64, dir_in: bool) -> Trb {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`
- `dir_in`: `bool`

**Returns:** `Trb`

*Defined at line 262*

---

## trb_status

Status stage: direction opposite to the data stage (IN when there is
no data stage), interrupt on completion.

```fides
pub fn trb_status(dir_in: bool) -> Trb {
```

**Parameters:**

- `dir_in`: `bool`

**Returns:** `Trb`

*Defined at line 274*

---

## trb_link

```fides
pub fn trb_link(target: i64, toggle: bool) -> Trb {
```

**Parameters:**

- `target`: `i64`
- `toggle`: `bool`

**Returns:** `Trb`

*Defined at line 284*

---

## trb_noop_cmd

```fides
pub fn trb_noop_cmd() -> Trb {
```

**Returns:** `Trb`

*Defined at line 294*

---

## trb_enable_slot

```fides
pub fn trb_enable_slot(slot_type: i64) -> Trb {
```

**Parameters:**

- `slot_type`: `i64`

**Returns:** `Trb`

*Defined at line 300*

---

## trb_disable_slot

```fides
pub fn trb_disable_slot(slot: i64) -> Trb {
```

**Parameters:**

- `slot`: `i64`

**Returns:** `Trb`

*Defined at line 306*

---

## trb_address_device

Address Device; bsr = block SET_ADDRESS (only enable the slot).

```fides
pub fn trb_address_device(input_ctx: i64, slot: i64, bsr: bool) -> Trb {
```

**Parameters:**

- `input_ctx`: `i64`
- `slot`: `i64`
- `bsr`: `bool`

**Returns:** `Trb`

*Defined at line 313*

---

## trb_configure_ep

Configure Endpoint; dc = deconfigure.

```fides
pub fn trb_configure_ep(input_ctx: i64, slot: i64, dc: bool) -> Trb {
```

**Parameters:**

- `input_ctx`: `i64`
- `slot`: `i64`
- `dc`: `bool`

**Returns:** `Trb`

*Defined at line 324*

---

## trb_evaluate_ctx

```fides
pub fn trb_evaluate_ctx(input_ctx: i64, slot: i64) -> Trb {
```

**Parameters:**

- `input_ctx`: `i64`
- `slot`: `i64`

**Returns:** `Trb`

*Defined at line 334*

---

## trb_reset_ep

```fides
pub fn trb_reset_ep(slot: i64, dci: i64, preserve: bool) -> Trb {
```

**Parameters:**

- `slot`: `i64`
- `dci`: `i64`
- `preserve`: `bool`

**Returns:** `Trb`

*Defined at line 340*

---

## trb_stop_ep

```fides
pub fn trb_stop_ep(slot: i64, dci: i64, suspend: bool) -> Trb {
```

**Parameters:**

- `slot`: `i64`
- `dci`: `i64`
- `suspend`: `bool`

**Returns:** `Trb`

*Defined at line 350*

---

## trb_set_tr_deq

Set TR Dequeue Pointer: new dequeue address with its cycle state.

```fides
pub fn trb_set_tr_deq(slot: i64, dci: i64, ptr: i64, dcs: i64) -> Trb {
```

**Parameters:**

- `slot`: `i64`
- `dci`: `i64`
- `ptr`: `i64`
- `dcs`: `i64`

**Returns:** `Trb`

*Defined at line 361*

---

## trb_reset_device

```fides
pub fn trb_reset_device(slot: i64) -> Trb {
```

**Parameters:**

- `slot`: `i64`

**Returns:** `Trb`

*Defined at line 367*

---

## xhci_trb_write

Write a TRB at pa: parameter and status first, the control dword (with
the cycle bit) last, so the controller never sees a half-written TRB.

```fides
pub fn xhci_trb_write(m: &mut PhysMem, pa: i64, t: Trb) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `t`: `Trb`

*Defined at line 375*

---

## xhci_trb_read

```fides
pub fn xhci_trb_read(m: &PhysMem, pa: i64) -> Trb {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`

**Returns:** `Trb`

*Defined at line 383*

---

## XhciRing

```fides
pub struct XhciRing {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `size` | `i64` |
| `enq` | `i64` |
| `pcs` | `i64` |
| `deq` | `i64` |
| `queued` | `i64` |

*Defined at line 393*

---

## xhci_ring_new

A one-segment ring of `size` TRBs at pa (64-byte aligned): zeroed,
with a link TRB back to the start that toggles the cycle state.

```fides
pub fn xhci_ring_new(m: &mut PhysMem, pa: i64, size: i64) -> XhciRing {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `size`: `i64`

**Returns:** `XhciRing`

*Defined at line 407*

---

## xhci_ring_space

TRBs that can still be queued (one slot stays empty to tell full from
empty).

```fides
pub fn xhci_ring_space(r: &XhciRing) -> i64 {
```

**Parameters:**

- `r`: `&XhciRing`

**Returns:** `i64`

*Defined at line 421*

---

## xhci_ring_enq_ptr

Physical address of the next enqueue slot.

```fides
pub fn xhci_ring_enq_ptr(r: &XhciRing) -> i64 {
```

**Parameters:**

- `r`: `&XhciRing`

**Returns:** `i64`

*Defined at line 427*

---

## xhci_ring_push

Queue a TRB with the producer cycle bit; returns its address, -1 when
the ring is full. Crossing the link TRB hands it to the controller
(its cycle bit and chain bit set from the TRB before it) and toggles
the producer cycle state.

```fides
pub fn xhci_ring_push(m: &mut PhysMem, r: &mut XhciRing, t: Trb) -> i64 {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `r`: `&mut XhciRing`
- `t`: `Trb`

**Returns:** `i64`

*Defined at line 435*

---

## xhci_ring_consumed

The controller reported it finished the TRB at pa: everything up to
and including it is free again.

```fides
pub fn xhci_ring_consumed(r: &mut XhciRing, pa: i64) {
```

**Parameters:**

- `r`: `&mut XhciRing`
- `pa`: `i64`

*Defined at line 458*

---

## xhci_crcr

CRCR value: ring address with the ring cycle state.

```fides
pub fn xhci_crcr(r: &XhciRing) -> i64 {
```

**Parameters:**

- `r`: `&XhciRing`

**Returns:** `i64`

*Defined at line 478*

---

## XhciEventRing

```fides
pub struct XhciEventRing {
```

**Fields:**

| Name | Type |
|------|------|
| `erst` | `i64` |
| `seg` | `i64` |
| `size` | `i64` |
| `deq` | `i64` |
| `ccs` | `i64` |

*Defined at line 484*

---

## xhci_event_ring_new

One-segment event ring: segment of `size` TRBs at seg, ERST at erst.

```fides
pub fn xhci_event_ring_new(m: &mut PhysMem, erst: i64, seg: i64, size: i64) -> XhciEventRing {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `erst`: `i64`
- `seg`: `i64`
- `size`: `i64`

**Returns:** `XhciEventRing`

*Defined at line 494*

---

## xhci_event_ptr

```fides
pub fn xhci_event_ptr(er: &XhciEventRing) -> i64 {
```

**Parameters:**

- `er`: `&XhciEventRing`

**Returns:** `i64`

*Defined at line 508*

---

## xhci_event_pending

Whether an event is waiting (its cycle bit matches CCS).

```fides
pub fn xhci_event_pending(er: &XhciEventRing, m: &PhysMem) -> bool {
```

**Parameters:**

- `er`: `&XhciEventRing`
- `m`: `&PhysMem`

**Returns:** `bool`

*Defined at line 513*

---

## xhci_event_pop

Take the next event; control = -1 when none is pending.

```fides
pub fn xhci_event_pop(er: &mut XhciEventRing, m: &PhysMem) -> Trb {
```

**Parameters:**

- `er`: `&mut XhciEventRing`
- `m`: `&PhysMem`

**Returns:** `Trb`

*Defined at line 518*

---

## xhci_erdp

ERDP value after handling events: dequeue pointer with EHB (bit 3,
write 1 to clear the busy flag).

```fides
pub fn xhci_erdp(er: &XhciEventRing) -> i64 {
```

**Parameters:**

- `er`: `&XhciEventRing`

**Returns:** `i64`

*Defined at line 535*

---

## EP_ISOCH_OUT:

```fides
pub const EP_ISOCH_OUT: i64 = 1;
```

*Defined at line 541*

---

## EP_BULK_OUT:

```fides
pub const EP_BULK_OUT: i64 = 2;
```

*Defined at line 542*

---

## EP_INTR_OUT:

```fides
pub const EP_INTR_OUT: i64 = 3;
```

*Defined at line 543*

---

## EP_CONTROL:

```fides
pub const EP_CONTROL: i64 = 4;
```

*Defined at line 544*

---

## EP_ISOCH_IN:

```fides
pub const EP_ISOCH_IN: i64 = 5;
```

*Defined at line 545*

---

## EP_BULK_IN:

```fides
pub const EP_BULK_IN: i64 = 6;
```

*Defined at line 546*

---

## EP_INTR_IN:

```fides
pub const EP_INTR_IN: i64 = 7;
```

*Defined at line 547*

---

## xhci_dci

Device context index of an endpoint: EP0 = 1, epN OUT = 2N, IN = 2N+1.

```fides
pub fn xhci_dci(ep_num: i64, dir_in: bool) -> i64 {
```

**Parameters:**

- `ep_num`: `i64`
- `dir_in`: `bool`

**Returns:** `i64`

*Defined at line 550*

---

## xhci_ep_type

xHCI endpoint type from a USB endpoint descriptor's bmAttributes and
direction.

```fides
pub fn xhci_ep_type(bm_attributes: i64, dir_in: bool) -> i64 {
```

**Parameters:**

- `bm_attributes`: `i64`
- `dir_in`: `bool`

**Returns:** `i64`

*Defined at line 562*

---

## xhci_ep0_max_packet

Initial EP0 max packet size for a port speed.

```fides
pub fn xhci_ep0_max_packet(speed: i64) -> i64 {
```

**Parameters:**

- `speed`: `i64`

**Returns:** `i64`

*Defined at line 574*

---

## xhci_slot_ctx

Slot context dword 0..3.

```fides
pub fn xhci_slot_ctx(speed: i64, entries: i64, root_port: i64, route: i64) -> Vec<i64> {
```

**Parameters:**

- `speed`: `i64`
- `entries`: `i64`
- `root_port`: `i64`
- `route`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 588*

---

## xhci_ep_ctx

```fides
pub fn xhci_ep_ctx(ep_type: i64, max_packet: i64, burst: i64, interval: i64, ring: i64, dcs: i64, avg_trb: i64) -> Vec<i64> {
```

**Parameters:**

- `ep_type`: `i64`
- `max_packet`: `i64`
- `burst`: `i64`
- `interval`: `i64`
- `ring`: `i64`
- `dcs`: `i64`
- `avg_trb`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 601*

---

## xhci_ctx_size

Context size in bytes (HCCPARAMS1.CSZ: 64-byte contexts).

```fides
pub fn xhci_ctx_size(csz64: bool) -> i64 {
```

**Parameters:**

- `csz64`: `bool`

**Returns:** `i64`

*Defined at line 624*

---

## xhci_input_ctx_address

Input context for Address Device at pa: add flags A0 (slot) and A1
(EP0), the slot context and the EP0 context on ring ep0_ring.

```fides
pub fn xhci_input_ctx_address(m: &mut PhysMem, pa: i64, csz64: bool, root_port: i64, speed: i64, ep0_ring: i64) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `csz64`: `bool`
- `root_port`: `i64`
- `speed`: `i64`
- `ep0_ring`: `i64`

*Defined at line 633*

---

## xhci_ctx_dword

Dword d of context index `ctx` (0 = input control / slot, ...) in a
context block at pa.

```fides
pub fn xhci_ctx_dword(m: &PhysMem, pa: i64, csz64: bool, ctx: i64, d: i64) -> i64 {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`
- `csz64`: `bool`
- `ctx`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 647*

---

## XhciBus

```fides
pub struct XhciBus {
```

**Fields:**

| Name | Type |
|------|------|
| `rd` | `fn(i64) -> i64` |
| `wr` | `fn(i64, i64) -> void` |

*Defined at line 653*

---

## xhci_rd

```fides
pub fn xhci_rd(b: &XhciBus, off: i64) -> i64 {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 658*

---

## xhci_wr

```fides
pub fn xhci_wr(b: &XhciBus, off: i64, v: i64) {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 663*

---

## xhci_wr64

64-bit register as two dword writes, low first.

```fides
pub fn xhci_wr64(b: &XhciBus, off: i64, v: i64) {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 669*

---

## xhci_rd64

```fides
pub fn xhci_rd64(b: &XhciBus, off: i64) -> i64 {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 675*

---

## XhciCaps

```fides
pub struct XhciCaps {
```

**Fields:**

| Name | Type |
|------|------|
| `caplength` | `i64` |
| `version` | `i64` |
| `max_slots` | `i64` |
| `max_intrs` | `i64` |
| `max_ports` | `i64` |
| `erst_max` | `i64` |
| `scratchpads` | `i64` |
| `ac64` | `bool` |
| `csz64` | `bool` |
| `ppc` | `bool` |
| `xecp` | `i64` |
| `op` | `i64` |
| `db` | `i64` |
| `rt` | `i64` |

*Defined at line 679*

---

## xhci_caps

```fides
pub fn xhci_caps(b: &XhciBus) -> XhciCaps {
```

**Parameters:**

- `b`: `&XhciBus`

**Returns:** `XhciCaps`

*Defined at line 698*

---

## xhci_portsc_off

Byte offset of PORTSC of port n (1-based).

```fides
pub fn xhci_portsc_off(c: &XhciCaps, n: i64) -> i64 {
```

**Parameters:**

- `c`: `&XhciCaps`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 710*

---

## xhci_ir_off

Byte offset of a register of interrupter i: IMAN 0, IMOD 4, ERSTSZ 8,
ERSTBA 0x10, ERDP 0x18.

```fides
pub fn xhci_ir_off(c: &XhciCaps, i: i64, reg: i64) -> i64 {
```

**Parameters:**

- `c`: `&XhciCaps`
- `i`: `i64`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 716*

---

## xhci_db_off

Doorbell register of a slot (0 = the host controller / command ring).

```fides
pub fn xhci_db_off(c: &XhciCaps, slot: i64) -> i64 {
```

**Parameters:**

- `c`: `&XhciCaps`
- `slot`: `i64`

**Returns:** `i64`

*Defined at line 721*

---

## xhci_ring_doorbell

```fides
pub fn xhci_ring_doorbell(b: &XhciBus, c: &XhciCaps, slot: i64, target: i64, stream: i64) {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`
- `slot`: `i64`
- `target`: `i64`
- `stream`: `i64`

*Defined at line 725*

---

## xhci_reset

Stop, then reset the controller: clear R/S, wait for HCHalted, set
HCRST, wait for HCRST and CNR to clear.

```fides
pub fn xhci_reset(b: &XhciBus, c: &XhciCaps, spins: i64) -> bool {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`
- `spins`: `i64`

**Returns:** `bool`

*Defined at line 742*

---

## xhci_start

Program and start a reset controller: slots enabled, DCBAA, command
ring, interrupter 0 (ERSTSZ, ERDP, then ERSTBA last), interrupts on,
run. Returns false if the controller does not leave the halted state.

```fides
pub fn xhci_start(b: &XhciBus, c: &XhciCaps, dcbaa: i64, cmd: &XhciRing, er: &XhciEventRing, slots: i64, spins: i64) -> bool {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`
- `dcbaa`: `i64`
- `cmd`: `&XhciRing`
- `er`: `&XhciEventRing`
- `slots`: `i64`
- `spins`: `i64`

**Returns:** `bool`

*Defined at line 758*

---

## PortStatus

```fides
pub struct PortStatus {
```

**Fields:**

| Name | Type |
|------|------|
| `connected` | `bool` |
| `enabled` | `bool` |
| `powered` | `bool` |
| `reset` | `bool` |
| `speed` | `i64` |
| `link_state` | `i64` |
| `changes` | `i64,  // CSC..CEC bits (17..23)` |

*Defined at line 779*

---

## xhci_port_decode

```fides
pub fn xhci_port_decode(v: i64) -> PortStatus {
```

**Parameters:**

- `v`: `i64`

**Returns:** `PortStatus`

*Defined at line 789*

---

## xhci_portsc_preserve

Value to write to PORTSC that changes nothing: RW1C bits and PED
cleared so the write does not acknowledge or disable anything.

```fides
pub fn xhci_portsc_preserve(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 797*

---

## xhci_port_reset

Start a port reset (warm reset for USB 3 ports when warm).

```fides
pub fn xhci_port_reset(b: &XhciBus, c: &XhciCaps, port: i64, warm: bool) {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`
- `port`: `i64`
- `warm`: `bool`

*Defined at line 802*

---

## xhci_port_ack

Acknowledge change bits (write 1 to the ones in `bits`).

```fides
pub fn xhci_port_ack(b: &XhciBus, c: &XhciCaps, port: i64, bits: i64) {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`
- `port`: `i64`
- `bits`: `i64`

*Defined at line 813*

---

## XECP_LEGACY:

```fides
pub const XECP_LEGACY: i64 = 1;
```

*Defined at line 820*

---

## XECP_PROTOCOL:

```fides
pub const XECP_PROTOCOL: i64 = 2;
```

*Defined at line 821*

---

## xhci_ext_caps

Offsets of all extended capabilities, packed id * 2^32 + offset.

```fides
pub fn xhci_ext_caps(b: &XhciBus, c: &XhciCaps) -> Vec<i64> {
```

**Parameters:**

- `b`: `&XhciBus`
- `c`: `&XhciCaps`

**Returns:** `Vec<i64>`

*Defined at line 824*

---

## xhci_legacy_handoff

BIOS -> OS handoff on the USB legacy support capability at off: set
the OS-owned semaphore, wait for the BIOS-owned one to clear.

```fides
pub fn xhci_legacy_handoff(b: &XhciBus, off: i64, spins: i64) -> bool {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`
- `spins`: `i64`

**Returns:** `bool`

*Defined at line 843*

---

## XhciProtocol

```fides
pub struct XhciProtocol {
```

**Fields:**

| Name | Type |
|------|------|
| `major` | `i64` |
| `minor` | `i64` |
| `name` | `str` |
| `first_port` | `i64` |
| `port_count` | `i64` |

*Defined at line 856*

---

## xhci_protocol

Supported protocol capability at off ("USB " 2.0 or 3.x and its ports).

```fides
pub fn xhci_protocol(b: &XhciBus, off: i64) -> XhciProtocol {
```

**Parameters:**

- `b`: `&XhciBus`
- `off`: `i64`

**Returns:** `XhciProtocol`

*Defined at line 865*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
