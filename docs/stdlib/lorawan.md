[All modules](index.md)

# lorawan -- Fides Documentation

## Contents

- [LW_JOIN_REQUEST:](#LW_JOIN_REQUEST:) (const)
- [LW_JOIN_ACCEPT:](#LW_JOIN_ACCEPT:) (const)
- [LW_UNCONFIRMED_UP:](#LW_UNCONFIRMED_UP:) (const)
- [LW_UNCONFIRMED_DOWN:](#LW_UNCONFIRMED_DOWN:) (const)
- [LW_CONFIRMED_UP:](#LW_CONFIRMED_UP:) (const)
- [LW_CONFIRMED_DOWN:](#LW_CONFIRMED_DOWN:) (const)
- [LW_PROPRIETARY:](#LW_PROPRIETARY:) (const)
- [LW_DIR_UP:](#LW_DIR_UP:) (const)
- [LW_DIR_DOWN:](#LW_DIR_DOWN:) (const)
- [LW_MAX_FCNT_GAP:](#LW_MAX_FCNT_GAP:) (const)
- [LW_ADR_ACK_LIMIT:](#LW_ADR_ACK_LIMIT:) (const)
- [LW_ADR_ACK_DELAY:](#LW_ADR_ACK_DELAY:) (const)
- [lw_aes_encrypt](#lw_aes_encrypt) (function)
- [lw_aes_sbox](#lw_aes_sbox) (function)
- [lw_aes_decrypt](#lw_aes_decrypt) (function)
- [lw_cmac_subkeys](#lw_cmac_subkeys) (function)
- [lw_cmac](#lw_cmac) (function)
- [lw_reverse](#lw_reverse) (function)
- [lw_mhdr](#lw_mhdr) (function)
- [lw_mtype](#lw_mtype) (function)
- [lw_join_request](#lw_join_request) (function)
- [lw_join_request_verify](#lw_join_request_verify) (function)
- [lw_join_request_eui](#lw_join_request_eui) (function)
- [lw_join_request_nonce](#lw_join_request_nonce) (function)
- [LwJoinAccept](#LwJoinAccept) (struct)
- [lw_join_accept_encode](#lw_join_accept_encode) (function)
- [lw_join_accept_decode](#lw_join_accept_decode) (function)
- [lw_session_key](#lw_session_key) (function)
- [lw_cflist_freqs](#lw_cflist_freqs) (function)
- [lw_cflist_decode](#lw_cflist_decode) (function)
- [LwFrame](#LwFrame) (struct)
- [lw_frame](#lw_frame) (function)
- [lw_dir_of](#lw_dir_of) (function)
- [lw_crypt_payload](#lw_crypt_payload) (function)
- [lw_data_mic](#lw_data_mic) (function)
- [lw_data_encode](#lw_data_encode) (function)
- [lw_data_decode](#lw_data_decode) (function)
- [lw_fcnt_extend](#lw_fcnt_extend) (function)
- [LW_CID_LINK_CHECK:](#LW_CID_LINK_CHECK:) (const)
- [LW_CID_LINK_ADR:](#LW_CID_LINK_ADR:) (const)
- [LW_CID_DUTY_CYCLE:](#LW_CID_DUTY_CYCLE:) (const)
- [LW_CID_RX_PARAM_SETUP:](#LW_CID_RX_PARAM_SETUP:) (const)
- [LW_CID_DEV_STATUS:](#LW_CID_DEV_STATUS:) (const)
- [LW_CID_NEW_CHANNEL:](#LW_CID_NEW_CHANNEL:) (const)
- [LW_CID_RX_TIMING_SETUP:](#LW_CID_RX_TIMING_SETUP:) (const)
- [LW_CID_TX_PARAM_SETUP:](#LW_CID_TX_PARAM_SETUP:) (const)
- [LW_CID_DL_CHANNEL:](#LW_CID_DL_CHANNEL:) (const)
- [LW_CID_DEVICE_TIME:](#LW_CID_DEVICE_TIME:) (const)
- [lw_mac_len](#lw_mac_len) (function)
- [LwMacCmd](#LwMacCmd) (struct)
- [lw_mac_parse](#lw_mac_parse) (function)
- [lw_mac_encode](#lw_mac_encode) (function)
- [lw_link_adr_req](#lw_link_adr_req) (function)
- [lw_link_adr_ans](#lw_link_adr_ans) (function)
- [lw_eu868_sf](#lw_eu868_sf) (function)
- [lw_eu868_bw_khz](#lw_eu868_bw_khz) (function)
- [lw_eu868_max_payload](#lw_eu868_max_payload) (function)
- [lw_required_snr10](#lw_required_snr10) (function)
- [lw_adr_network](#lw_adr_network) (function)
- [LwAdrDevice](#LwAdrDevice) (struct)
- [lw_adr_uplink](#lw_adr_uplink) (function)
- [lw_adr_downlink](#lw_adr_downlink) (function)
- [lw_time_on_air_us](#lw_time_on_air_us) (function)

## LW_JOIN_REQUEST:

```fides
pub const LW_JOIN_REQUEST: i64 = 0;
```

*Defined at line 34*

---

## LW_JOIN_ACCEPT:

```fides
pub const LW_JOIN_ACCEPT: i64 = 1;
```

*Defined at line 35*

---

## LW_UNCONFIRMED_UP:

```fides
pub const LW_UNCONFIRMED_UP: i64 = 2;
```

*Defined at line 36*

---

## LW_UNCONFIRMED_DOWN:

```fides
pub const LW_UNCONFIRMED_DOWN: i64 = 3;
```

*Defined at line 37*

---

## LW_CONFIRMED_UP:

```fides
pub const LW_CONFIRMED_UP: i64 = 4;
```

*Defined at line 38*

---

## LW_CONFIRMED_DOWN:

```fides
pub const LW_CONFIRMED_DOWN: i64 = 5;
```

*Defined at line 39*

---

## LW_PROPRIETARY:

```fides
pub const LW_PROPRIETARY: i64 = 7;
```

*Defined at line 40*

---

## LW_DIR_UP:

```fides
pub const LW_DIR_UP: i64 = 0;
```

*Defined at line 41*

---

## LW_DIR_DOWN:

```fides
pub const LW_DIR_DOWN: i64 = 1;
```

*Defined at line 42*

---

## LW_MAX_FCNT_GAP:

```fides
pub const LW_MAX_FCNT_GAP: i64 = 16384;
```

*Defined at line 43*

---

## LW_ADR_ACK_LIMIT:

```fides
pub const LW_ADR_ACK_LIMIT: i64 = 64;
```

*Defined at line 44*

---

## LW_ADR_ACK_DELAY:

```fides
pub const LW_ADR_ACK_DELAY: i64 = 32;
```

*Defined at line 45*

---

## lw_aes_encrypt

```fides
pub fn lw_aes_encrypt(key: &Vec<i64>, block: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `block`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 49*

---

## lw_aes_sbox

The AES S-box, computed: multiplicative inverse in GF(2^8), then the
affine map (FIPS 197 5.1.1).

```fides
pub fn lw_aes_sbox() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 77*

---

## lw_aes_decrypt

AES-128 inverse cipher of one block (FIPS 197 5.3).

```fides
pub fn lw_aes_decrypt(key: &Vec<i64>, block: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `block`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 144*

---

## lw_cmac_subkeys

The CMAC subkeys [K1 (16 bytes), K2 (16 bytes)].

```fides
pub fn lw_cmac_subkeys(key: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 214*

---

## lw_cmac

```fides
pub fn lw_cmac(key: &Vec<i64>, msg: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `msg`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 233*

---

## lw_reverse

A byte string reversed (EUIs are shown most significant byte first but
sent least significant first).

```fides
pub fn lw_reverse(b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 313*

---

## lw_mhdr

```fides
pub fn lw_mhdr(mtype: i64) -> i64 {
```

**Parameters:**

- `mtype`: `i64`

**Returns:** `i64`

*Defined at line 346*

---

## lw_mtype

```fides
pub fn lw_mtype(frame: &Vec<i64>) -> i64 {
```

**Parameters:**

- `frame`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 350*

---

## lw_join_request

Join-Request: MHDR | AppEUI | DevEUI | DevNonce | MIC. The EUIs are
given as shown (most significant byte first).

```fides
pub fn lw_join_request(app_key: &Vec<i64>, app_eui: &Vec<i64>, dev_eui: &Vec<i64>, dev_nonce: i64) -> Vec<i64> {
```

**Parameters:**

- `app_key`: `&Vec<i64>`
- `app_eui`: `&Vec<i64>`
- `dev_eui`: `&Vec<i64>`
- `dev_nonce`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 361*

---

## lw_join_request_verify

The network's check of a Join-Request's length, type and MIC (read
the EUIs and DevNonce with lw_join_request_eui / _nonce).

```fides
pub fn lw_join_request_verify(app_key: &Vec<i64>, f: &Vec<i64>) -> bool {
```

**Parameters:**

- `app_key`: `&Vec<i64>`
- `f`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 374*

---

## lw_join_request_eui

AppEUI (which = 0) or DevEUI (which = 1) of a Join-Request, as shown.

```fides
pub fn lw_join_request_eui(f: &Vec<i64>, which: i64) -> Vec<i64> {
```

**Parameters:**

- `f`: `&Vec<i64>`
- `which`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 382*

---

## lw_join_request_nonce

```fides
pub fn lw_join_request_nonce(f: &Vec<i64>) -> i64 {
```

**Parameters:**

- `f`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 386*

---

## LwJoinAccept

```fides
pub struct LwJoinAccept {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `app_nonce` | `i64` |
| `net_id` | `i64` |
| `dev_addr` | `i64` |
| `rx1_dr_offset` | `i64` |
| `rx2_dr` | `i64` |
| `rx_delay` | `i64` |
| `cf_list` | `Vec<i64>` |

*Defined at line 390*

---

## lw_join_accept_encode

Network side: the encrypted Join-Accept. cf_list is empty or 16 bytes.

```fides
pub fn lw_join_accept_encode(app_key: &Vec<i64>, ja: &LwJoinAccept) -> Vec<i64> {
```

**Parameters:**

- `app_key`: `&Vec<i64>`
- `ja`: `&LwJoinAccept`

**Returns:** `Vec<i64>`

*Defined at line 402*

---

## lw_join_accept_decode

Device side: decrypt (aes128_encrypt of each block), check the MIC and
read the fields; ok=false on a bad length or MIC.

```fides
pub fn lw_join_accept_decode(app_key: &Vec<i64>, f: &Vec<i64>) -> LwJoinAccept {
```

**Parameters:**

- `app_key`: `&Vec<i64>`
- `f`: `&Vec<i64>`

**Returns:** `LwJoinAccept`

*Defined at line 426*

---

## lw_session_key

NwkSKey (kind 1) or AppSKey (kind 2):
aes128_encrypt(AppKey, kind | AppNonce | NetID | DevNonce | pad16).

```fides
pub fn lw_session_key(app_key: &Vec<i64>, kind: i64, app_nonce: i64, net_id: i64, dev_nonce: i64) -> Vec<i64> {
```

**Parameters:**

- `app_key`: `&Vec<i64>`
- `kind`: `i64`
- `app_nonce`: `i64`
- `net_id`: `i64`
- `dev_nonce`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 459*

---

## lw_cflist_freqs

The CFList of EU868-like regions: up to five extra channel frequencies
(Hz), 3 bytes each in units of 100 Hz, then the CFListType byte 0.

```fides
pub fn lw_cflist_freqs(freqs: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `freqs`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 473*

---

## lw_cflist_decode

```fides
pub fn lw_cflist_decode(cf: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cf`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 488*

---

## LwFrame

```fides
pub struct LwFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `mtype` | `i64` |
| `dev_addr` | `i64` |
| `adr` | `bool` |
| `adr_ack_req` | `bool` |
| `ack` | `bool` |
| `fpending` | `bool` |
| `fcnt` | `i64` |
| `fopts` | `Vec<i64>` |
| `fport` | `i64` |
| `payload` | `Vec<i64>` |
| `mic_ok` | `bool` |
| `ok` | `bool` |

*Defined at line 506*

---

## lw_frame

```fides
pub fn lw_frame(mtype: i64, dev_addr: i64, fcnt: i64, fport: i64, payload: Vec<i64>) -> LwFrame {
```

**Parameters:**

- `mtype`: `i64`
- `dev_addr`: `i64`
- `fcnt`: `i64`
- `fport`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `LwFrame`

*Defined at line 521*

---

## lw_dir_of

```fides
pub fn lw_dir_of(mtype: i64) -> i64 {
```

**Parameters:**

- `mtype`: `i64`

**Returns:** `i64`

*Defined at line 527*

---

## lw_crypt_payload

FRMPayload encryption (symmetric): XOR with AES(K, A_i), A_i = 0x01 |
0^4 | Dir | DevAddr | FCnt (32) | 0x00 | i.

```fides
pub fn lw_crypt_payload(key: &Vec<i64>, dir: i64, dev_addr: i64, fcnt: i64, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `dir`: `i64`
- `dev_addr`: `i64`
- `fcnt`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 536*

---

## lw_data_mic

MIC of a data frame: CMAC(NwkSKey, B0 | msg)[0..4], B0 = 0x49 | 0^4 |
Dir | DevAddr | FCnt (32) | 0x00 | len(msg).

```fides
pub fn lw_data_mic(nwk_skey: &Vec<i64>, dir: i64, dev_addr: i64, fcnt: i64, msg: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `nwk_skey`: `&Vec<i64>`
- `dir`: `i64`
- `dev_addr`: `i64`
- `fcnt`: `i64`
- `msg`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 563*

---

## lw_data_encode

Encode a data frame. fcnt is the full 32-bit counter (its low 16 bits
go on the air); fport < 0 sends no port and no payload.

```fides
pub fn lw_data_encode(nwk_skey: &Vec<i64>, app_skey: &Vec<i64>, fr: &LwFrame) -> Vec<i64> {
```

**Parameters:**

- `nwk_skey`: `&Vec<i64>`
- `app_skey`: `&Vec<i64>`
- `fr`: `&LwFrame`

**Returns:** `Vec<i64>`

*Defined at line 578*

---

## lw_data_decode

Decode a data frame and decrypt its payload with the full counter
fcnt32 (from lw_fcnt_extend). mic_ok tells whether the MIC matched.

```fides
pub fn lw_data_decode(nwk_skey: &Vec<i64>, app_skey: &Vec<i64>, f: &Vec<i64>, fcnt32: i64) -> LwFrame {
```

**Parameters:**

- `nwk_skey`: `&Vec<i64>`
- `app_skey`: `&Vec<i64>`
- `f`: `&Vec<i64>`
- `fcnt32`: `i64`

**Returns:** `LwFrame`

*Defined at line 614*

---

## lw_fcnt_extend

The 32-bit counter a received 16-bit FCnt stands for, given the last
accepted one (-1 before any): the smallest value above `last` with
those low bits. -1 when it is a replay or beyond MAX_FCNT_GAP.

```fides
pub fn lw_fcnt_extend(last: i64, fcnt16: i64) -> i64 {
```

**Parameters:**

- `last`: `i64`
- `fcnt16`: `i64`

**Returns:** `i64`

*Defined at line 658*

---

## LW_CID_LINK_CHECK:

```fides
pub const LW_CID_LINK_CHECK: i64 = 0x02;
```

*Defined at line 675*

---

## LW_CID_LINK_ADR:

```fides
pub const LW_CID_LINK_ADR: i64 = 0x03;
```

*Defined at line 676*

---

## LW_CID_DUTY_CYCLE:

```fides
pub const LW_CID_DUTY_CYCLE: i64 = 0x04;
```

*Defined at line 677*

---

## LW_CID_RX_PARAM_SETUP:

```fides
pub const LW_CID_RX_PARAM_SETUP: i64 = 0x05;
```

*Defined at line 678*

---

## LW_CID_DEV_STATUS:

```fides
pub const LW_CID_DEV_STATUS: i64 = 0x06;
```

*Defined at line 679*

---

## LW_CID_NEW_CHANNEL:

```fides
pub const LW_CID_NEW_CHANNEL: i64 = 0x07;
```

*Defined at line 680*

---

## LW_CID_RX_TIMING_SETUP:

```fides
pub const LW_CID_RX_TIMING_SETUP: i64 = 0x08;
```

*Defined at line 681*

---

## LW_CID_TX_PARAM_SETUP:

```fides
pub const LW_CID_TX_PARAM_SETUP: i64 = 0x09;
```

*Defined at line 682*

---

## LW_CID_DL_CHANNEL:

```fides
pub const LW_CID_DL_CHANNEL: i64 = 0x0A;
```

*Defined at line 683*

---

## LW_CID_DEVICE_TIME:

```fides
pub const LW_CID_DEVICE_TIME: i64 = 0x0D;
```

*Defined at line 684*

---

## lw_mac_len

Payload length of a MAC command in a direction (-1: unknown CID).

```fides
pub fn lw_mac_len(cid: i64, dir: i64) -> i64 {
```

**Parameters:**

- `cid`: `i64`
- `dir`: `i64`

**Returns:** `i64`

*Defined at line 687*

---

## LwMacCmd

```fides
pub struct LwMacCmd {
```

**Fields:**

| Name | Type |
|------|------|
| `cid` | `i64` |
| `args` | `Vec<i64>` |

*Defined at line 730*

---

## lw_mac_parse

Split FOpts (or a port-0 payload) into commands; stops at an unknown
CID (the rest cannot be parsed, as the spec says).

```fides
pub fn lw_mac_parse(b: &Vec<i64>, dir: i64) -> Vec<LwMacCmd> {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `dir`: `i64`

**Returns:** `Vec<LwMacCmd>`

*Defined at line 737*

---

## lw_mac_encode

```fides
pub fn lw_mac_encode(cmds: &Vec<LwMacCmd>) -> Vec<i64> {
```

**Parameters:**

- `cmds`: `&Vec<LwMacCmd>`

**Returns:** `Vec<i64>`

*Defined at line 754*

---

## lw_link_adr_req

LinkADRReq: data rate, TX power index, channel mask, ChMaskCntl, NbTrans.

```fides
pub fn lw_link_adr_req(dr: i64, txpow: i64, ch_mask: i64, ch_mask_cntl: i64, nb_trans: i64) -> LwMacCmd {
```

**Parameters:**

- `dr`: `i64`
- `txpow`: `i64`
- `ch_mask`: `i64`
- `ch_mask_cntl`: `i64`
- `nb_trans`: `i64`

**Returns:** `LwMacCmd`

*Defined at line 767*

---

## lw_link_adr_ans

The device's LinkADRAns status bits: power ACK (bit 2), data rate ACK
(bit 1), channel mask ACK (bit 0).

```fides
pub fn lw_link_adr_ans(power_ok: bool, dr_ok: bool, mask_ok: bool) -> LwMacCmd {
```

**Parameters:**

- `power_ok`: `bool`
- `dr_ok`: `bool`
- `mask_ok`: `bool`

**Returns:** `LwMacCmd`

*Defined at line 779*

---

## lw_eu868_sf

EU868 data rate -> spreading factor (7..12), 0 for FSK (DR7).

```fides
pub fn lw_eu868_sf(dr: i64) -> i64 {
```

**Parameters:**

- `dr`: `i64`

**Returns:** `i64`

*Defined at line 800*

---

## lw_eu868_bw_khz

```fides
pub fn lw_eu868_bw_khz(dr: i64) -> i64 {
```

**Parameters:**

- `dr`: `i64`

**Returns:** `i64`

*Defined at line 810*

---

## lw_eu868_max_payload

EU868 maximum MACPayload size M (bytes) per data rate (repeater-
compatible N + 8 is the FRMPayload limit plus the FHDR/FPort).

```fides
pub fn lw_eu868_max_payload(dr: i64) -> i64 {
```

**Parameters:**

- `dr`: `i64`

**Returns:** `i64`

*Defined at line 819*

---

## lw_required_snr10

The SNR (0.1 dB) a LoRa spreading factor needs for demodulation.

```fides
pub fn lw_required_snr10(sf: i64) -> i64 {
```

**Parameters:**

- `sf`: `i64`

**Returns:** `i64`

*Defined at line 830*

---

## lw_adr_network

Network-side ADR step (Semtech's reference algorithm): from the best
SNR (0.1 dB) of recent uplinks at data rate dr and TX power index, the
new [dr, txpower index]. Each 3 dB of margin above required + margin
raises the data rate (up to max_dr), then lowers power (raises the
index up to max_tx_index); a negative margin raises power.

```fides
pub fn lw_adr_network(snr_max10: i64, dr: i64, txpow: i64, margin10: i64, max_dr: i64, max_tx_index: i64) -> Vec<i64> {
```

**Parameters:**

- `snr_max10`: `i64`
- `dr`: `i64`
- `txpow`: `i64`
- `margin10`: `i64`
- `max_dr`: `i64`
- `max_tx_index`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 839*

---

## LwAdrDevice

Device-side ADR backoff state.

```fides
pub struct LwAdrDevice {
```

**Fields:**

| Name | Type |
|------|------|
| `ack_cnt` | `i64` |
| `dr` | `i64` |
| `txpow` | `i64` |

*Defined at line 868*

---

## lw_adr_uplink

Count an uplink without any downlink; returns whether this uplink
sets ADRACKReq. After ADR_ACK_LIMIT + ADR_ACK_DELAY uplinks without an
answer the device first goes back to full power, then lowers the data
rate one step per ADR_ACK_DELAY.

```fides
pub fn lw_adr_uplink(a: &mut LwAdrDevice) -> bool {
```

**Parameters:**

- `a`: `&mut LwAdrDevice`

**Returns:** `bool`

*Defined at line 878*

---

## lw_adr_downlink

Any downlink resets the counter.

```fides
pub fn lw_adr_downlink(a: &mut LwAdrDevice) {
```

**Parameters:**

- `a`: `&mut LwAdrDevice`

*Defined at line 891*

---

## lw_time_on_air_us

LoRa time on air in microseconds (Semtech AN1200.13): payload bytes,
spreading factor, bandwidth in kHz, coding rate 1..4 (4/5..4/8),
preamble symbols, explicit header, CRC on; low data rate optimization
when the symbol lasts 16 ms or more.

```fides
pub fn lw_time_on_air_us(pl: i64, sf: i64, bw_khz: i64, cr: i64, preamble: i64, explicit_header: bool, crc: bool) -> i64 {
```

**Parameters:**

- `pl`: `i64`
- `sf`: `i64`
- `bw_khz`: `i64`
- `cr`: `i64`
- `preamble`: `i64`
- `explicit_header`: `bool`
- `crc`: `bool`

**Returns:** `i64`

*Defined at line 899*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
