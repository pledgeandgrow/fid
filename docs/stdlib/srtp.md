[All modules](index.md)

# srtp -- Fides Documentation

## Contents

- [SRTP_AES128_CM_SHA1_80:](#SRTP_AES128_CM_SHA1_80:) (const)
- [SRTP_AES128_CM_SHA1_32:](#SRTP_AES128_CM_SHA1_32:) (const)
- [SRTP_AEAD_AES_128_GCM:](#SRTP_AEAD_AES_128_GCM:) (const)
- [SRTP_LABEL_RTP_ENC:](#SRTP_LABEL_RTP_ENC:) (const)
- [SRTP_LABEL_RTP_AUTH:](#SRTP_LABEL_RTP_AUTH:) (const)
- [SRTP_LABEL_RTP_SALT:](#SRTP_LABEL_RTP_SALT:) (const)
- [SRTP_LABEL_RTCP_ENC:](#SRTP_LABEL_RTCP_ENC:) (const)
- [SRTP_LABEL_RTCP_AUTH:](#SRTP_LABEL_RTCP_AUTH:) (const)
- [SRTP_LABEL_RTCP_SALT:](#SRTP_LABEL_RTCP_SALT:) (const)
- [srtp_profile_name](#srtp_profile_name) (function)
- [srtp_profile_known](#srtp_profile_known) (function)
- [srtp_profile_key_len](#srtp_profile_key_len) (function)
- [srtp_profile_salt_len](#srtp_profile_salt_len) (function)
- [srtp_profile_tag_len](#srtp_profile_tag_len) (function)
- [srtp_profile_rtcp_tag_len](#srtp_profile_rtcp_tag_len) (function)
- [srtp_aes_cm](#srtp_aes_cm) (function)
- [srtp_kdf](#srtp_kdf) (function)
- [SrtpContext](#SrtpContext) (struct)
- [SrtpResult](#SrtpResult) (struct)
- [srtp_context_session](#srtp_context_session) (function)
- [srtp_context_new](#srtp_context_new) (function)
- [srtp_set_roc](#srtp_set_roc) (function)
- [srtp_get_roc](#srtp_get_roc) (function)
- [srtcp_set_index](#srtcp_set_index) (function)
- [srtp_rtp_header_len](#srtp_rtp_header_len) (function)
- [srtp_estimate_roc](#srtp_estimate_roc) (function)
- [srtp_protect](#srtp_protect) (function)
- [srtp_unprotect](#srtp_unprotect) (function)
- [srtcp_protect](#srtcp_protect) (function)
- [srtcp_protect_opt](#srtcp_protect_opt) (function)
- [srtcp_unprotect](#srtcp_unprotect) (function)
- [srtp_is_rtcp](#srtp_is_rtcp) (function)

## SRTP_AES128_CM_SHA1_80:

DTLS-SRTP protection profile ids (RFC 5764 §4.1.2, RFC 7714 §14.2).

```fides
pub const SRTP_AES128_CM_SHA1_80: i64 = 1;
```

*Defined at line 49*

---

## SRTP_AES128_CM_SHA1_32:

```fides
pub const SRTP_AES128_CM_SHA1_32: i64 = 2;
```

*Defined at line 50*

---

## SRTP_AEAD_AES_128_GCM:

```fides
pub const SRTP_AEAD_AES_128_GCM: i64 = 7;
```

*Defined at line 51*

---

## SRTP_LABEL_RTP_ENC:

AES-CM PRF labels (RFC 3711 §4.3.1).

```fides
pub const SRTP_LABEL_RTP_ENC: i64 = 0;
```

*Defined at line 54*

---

## SRTP_LABEL_RTP_AUTH:

```fides
pub const SRTP_LABEL_RTP_AUTH: i64 = 1;
```

*Defined at line 55*

---

## SRTP_LABEL_RTP_SALT:

```fides
pub const SRTP_LABEL_RTP_SALT: i64 = 2;
```

*Defined at line 56*

---

## SRTP_LABEL_RTCP_ENC:

```fides
pub const SRTP_LABEL_RTCP_ENC: i64 = 3;
```

*Defined at line 57*

---

## SRTP_LABEL_RTCP_AUTH:

```fides
pub const SRTP_LABEL_RTCP_AUTH: i64 = 4;
```

*Defined at line 58*

---

## SRTP_LABEL_RTCP_SALT:

```fides
pub const SRTP_LABEL_RTCP_SALT: i64 = 5;
```

*Defined at line 59*

---

## srtp_profile_name

```fides
pub fn srtp_profile_name(p: i64) -> str {
```

**Parameters:**

- `p`: `i64`

**Returns:** `str`

*Defined at line 63*

---

## srtp_profile_known

```fides
pub fn srtp_profile_known(p: i64) -> bool {
```

**Parameters:**

- `p`: `i64`

**Returns:** `bool`

*Defined at line 76*

---

## srtp_profile_key_len

Master key length in bytes (16 for every profile here).

```fides
pub fn srtp_profile_key_len(p: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`

**Returns:** `i64`

*Defined at line 81*

---

## srtp_profile_salt_len

Master salt length: 14 bytes for AES-CM, 12 for GCM.

```fides
pub fn srtp_profile_salt_len(p: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`

**Returns:** `i64`

*Defined at line 86*

---

## srtp_profile_tag_len

SRTP authentication tag length in bytes.

```fides
pub fn srtp_profile_tag_len(p: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## srtp_profile_rtcp_tag_len

SRTCP authentication tag length (80 bits for both CM profiles).

```fides
pub fn srtp_profile_rtcp_tag_len(p: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`

**Returns:** `i64`

*Defined at line 105*

---

## srtp_aes_cm

```fides
pub fn srtp_aes_cm(@secret key: Vec<i64>, iv: Vec<i64>, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret key`: `Vec<i64>`
- `iv`: `Vec<i64>`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 167*

---

## srtp_kdf

The AES-CM PRF (RFC 3711 §4.3.1, §4.3.3): `n` bytes for `label` with
r = index DIV kdr (r = 0 when kdr is 0). The master salt is 14 bytes
(a 12-byte GCM salt is used as its first 12 bytes, zeros after, as
RFC 7714 implementations do).

```fides
pub fn srtp_kdf(master_key: Vec<i64>, master_salt: Vec<i64>, label: i64, index: i64, kdr: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `master_key`: `Vec<i64>`
- `master_salt`: `Vec<i64>`
- `label`: `i64`
- `index`: `i64`
- `kdr`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 204*

---

## SrtpContext

One direction of an SRTP session (what one sender protects, or what
one receiver unprotects): the session keys and per-SSRC state.

```fides
pub struct SrtpContext {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `profile` | `i64` |
| `rtp_key` | `Vec<i64>` |
| `rtp_auth` | `Vec<i64>` |
| `rtp_salt` | `Vec<i64>` |
| `rtcp_key` | `Vec<i64>` |
| `rtcp_auth` | `Vec<i64>` |
| `rtcp_salt` | `Vec<i64>` |
| `ssrcs` | `Vec<i64>` |
| `rocs` | `Vec<i64>` |
| `seqs` | `Vec<i64>` |
| `started` | `Vec<i64>` |
| `tops` | `Vec<i64>` |
| `wins` | `Vec<Vec<i64>>` |
| `rtcp_next` | `Vec<i64>` |
| `rtcp_tops` | `Vec<i64>` |
| `rtcp_wins` | `Vec<Vec<i64>>` |

*Defined at line 235*

---

## SrtpResult

The result of protecting or unprotecting one packet.

```fides
pub struct SrtpResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `data` | `Vec<i64>` |

*Defined at line 257*

---

## srtp_context_session

A context from session keys directly (what the RFC test vectors give).
For GCM the auth keys are unused (pass empty).

```fides
pub fn srtp_context_session(profile: i64, rtp_key: Vec<i64>, rtp_salt: Vec<i64>, rtp_auth: Vec<i64>, rtcp_key: Vec<i64>, rtcp_salt: Vec<i64>, rtcp_auth: Vec<i64>) -> SrtpContext {
```

**Parameters:**

- `profile`: `i64`
- `rtp_key`: `Vec<i64>`
- `rtp_salt`: `Vec<i64>`
- `rtp_auth`: `Vec<i64>`
- `rtcp_key`: `Vec<i64>`
- `rtcp_salt`: `Vec<i64>`
- `rtcp_auth`: `Vec<i64>`

**Returns:** `SrtpContext`

*Defined at line 277*

---

## srtp_context_new

A context from a master key and salt (RFC 3711 §4.3, kdr = 0): what
DTLS-SRTP hands over for each direction.

```fides
pub fn srtp_context_new(profile: i64, master_key: Vec<i64>, master_salt: Vec<i64>) -> SrtpContext {
```

**Parameters:**

- `profile`: `i64`
- `master_key`: `Vec<i64>`
- `master_salt`: `Vec<i64>`

**Returns:** `SrtpContext`

*Defined at line 293*

---

## srtp_set_roc

Set the rollover counter of an SSRC (signalled out of band, or to
resume a stream).

```fides
pub fn srtp_set_roc(c: &mut SrtpContext, ssrc: i64, roc: i64) {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `ssrc`: `i64`
- `roc`: `i64`

*Defined at line 331*

---

## srtp_get_roc

```fides
pub fn srtp_get_roc(c: &mut SrtpContext, ssrc: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `ssrc`: `i64`

**Returns:** `i64`

*Defined at line 336*

---

## srtcp_set_index

Set the next SRTCP index a sender uses for an SSRC.

```fides
pub fn srtcp_set_index(c: &mut SrtpContext, ssrc: i64, index: i64) {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `ssrc`: `i64`
- `index`: `i64`

*Defined at line 342*

---

## srtp_rtp_header_len

The length of an RTP packet's header (fixed part, CSRCs, extension),
or -1 when the packet is not RTP version 2 or is too short.

```fides
pub fn srtp_rtp_header_len(p: &Vec<i64>) -> i64 {
```

**Parameters:**

- `p`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 351*

---

## srtp_estimate_roc

The receiver's guess v of the ROC of a packet with sequence number
`seq`, from its ROC and highest sequence number s_l (RFC 3711
Appendix A). -1 when the packet would come from before ROC 0.

```fides
pub fn srtp_estimate_roc(roc: i64, s_l: i64, seq: i64) -> i64 {
```

**Parameters:**

- `roc`: `i64`
- `s_l`: `i64`
- `seq`: `i64`

**Returns:** `i64`

*Defined at line 371*

---

## srtp_protect

Protect an RTP packet: the SRTP packet, or ok false (bad packet or
context). The sender's ROC for the SSRC goes up when the sequence
number wraps.

```fides
pub fn srtp_protect(c: &mut SrtpContext, pkt: Vec<i64>) -> SrtpResult {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `pkt`: `Vec<i64>`

**Returns:** `SrtpResult`

*Defined at line 451*

---

## srtp_unprotect

Unprotect an SRTP packet: the RTP packet, or ok false with the reason
("authentication failed", "replayed", "too old", ...). State (ROC,
highest sequence number, replay window) changes only for a packet
that authenticates.

```fides
pub fn srtp_unprotect(c: &mut SrtpContext, pkt: Vec<i64>) -> SrtpResult {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `pkt`: `Vec<i64>`

**Returns:** `SrtpResult`

*Defined at line 496*

---

## srtcp_protect

Protect an RTCP compound packet with encryption (the usual case).

```fides
pub fn srtcp_protect(c: &mut SrtpContext, pkt: Vec<i64>) -> SrtpResult {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `pkt`: `Vec<i64>`

**Returns:** `SrtpResult`

*Defined at line 580*

---

## srtcp_protect_opt

Protect an RTCP packet; `encrypt` false sends it authenticated only
(E flag 0). The SRTCP index of the sender's SSRC goes up by one.

```fides
pub fn srtcp_protect_opt(c: &mut SrtpContext, pkt: Vec<i64>, encrypt: bool) -> SrtpResult {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `pkt`: `Vec<i64>`
- `encrypt`: `bool`

**Returns:** `SrtpResult`

*Defined at line 586*

---

## srtcp_unprotect

Unprotect an SRTCP packet (encrypted or authenticated only).

```fides
pub fn srtcp_unprotect(c: &mut SrtpContext, pkt: Vec<i64>) -> SrtpResult {
```

**Parameters:**

- `c`: `&mut SrtpContext`
- `pkt`: `Vec<i64>`

**Returns:** `SrtpResult`

*Defined at line 638*

---

## srtp_is_rtcp

RTP or RTCP? (RFC 5761 §4: payload types 192..223 are RTCP when RTP
and RTCP share a port.)

```fides
pub fn srtp_is_rtcp(pkt: &Vec<i64>) -> bool {
```

**Parameters:**

- `pkt`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 718*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
