[All modules](index.md)

# webrtc -- Fides Documentation

## Contents

- [SdpMedia](#SdpMedia) (struct)
- [Sdp](#Sdp) (struct)
- [sdp_new](#sdp_new) (function)
- [sdp_parse](#sdp_parse) (function)
- [sdp_to_str](#sdp_to_str) (function)
- [sdp_attr](#sdp_attr) (function)
- [sdp_has_attr](#sdp_has_attr) (function)
- [sdp_attrs](#sdp_attrs) (function)
- [sdp_direction](#sdp_direction) (function)
- [sdp_rtpmap](#sdp_rtpmap) (function)
- [sdp_codecs](#sdp_codecs) (function)
- [sdp_ice_ufrag](#sdp_ice_ufrag) (function)
- [sdp_ice_pwd](#sdp_ice_pwd) (function)
- [sdp_fingerprint](#sdp_fingerprint) (function)
- [SdpAnswerConfig](#SdpAnswerConfig) (struct)
- [sdp_answer](#sdp_answer) (function)
- [IceCandidate](#IceCandidate) (struct)
- [ice_parse_candidate](#ice_parse_candidate) (function)
- [ice_candidate_to_str](#ice_candidate_to_str) (function)
- [ice_type_pref](#ice_type_pref) (function)
- [ice_priority](#ice_priority) (function)
- [ice_pair_priority](#ice_pair_priority) (function)
- [IcePair](#IcePair) (struct)
- [ice_pairs](#ice_pairs) (function)
- [webrtc_sha1](#webrtc_sha1) (function)
- [webrtc_hmac_sha1](#webrtc_hmac_sha1) (function)
- [webrtc_crc32](#webrtc_crc32) (function)
- [STUN_BINDING:](#STUN_BINDING:) (const)
- [STUN_REQUEST:](#STUN_REQUEST:) (const)
- [STUN_INDICATION:](#STUN_INDICATION:) (const)
- [STUN_SUCCESS:](#STUN_SUCCESS:) (const)
- [STUN_ERROR:](#STUN_ERROR:) (const)
- [STUN_MAGIC:](#STUN_MAGIC:) (const)
- [STUN_MAPPED_ADDRESS:](#STUN_MAPPED_ADDRESS:) (const)
- [STUN_USERNAME:](#STUN_USERNAME:) (const)
- [STUN_MESSAGE_INTEGRITY:](#STUN_MESSAGE_INTEGRITY:) (const)
- [STUN_ERROR_CODE:](#STUN_ERROR_CODE:) (const)
- [STUN_XOR_MAPPED_ADDRESS:](#STUN_XOR_MAPPED_ADDRESS:) (const)
- [STUN_PRIORITY:](#STUN_PRIORITY:) (const)
- [STUN_USE_CANDIDATE:](#STUN_USE_CANDIDATE:) (const)
- [STUN_SOFTWARE:](#STUN_SOFTWARE:) (const)
- [STUN_FINGERPRINT:](#STUN_FINGERPRINT:) (const)
- [STUN_ICE_CONTROLLED:](#STUN_ICE_CONTROLLED:) (const)
- [STUN_ICE_CONTROLLING:](#STUN_ICE_CONTROLLING:) (const)
- [stun_type](#stun_type) (function)
- [stun_class](#stun_class) (function)
- [stun_method](#stun_method) (function)
- [StunMsg](#StunMsg) (struct)
- [stun_new](#stun_new) (function)
- [stun_add_attr](#stun_add_attr) (function)
- [stun_add_username](#stun_add_username) (function)
- [stun_add_software](#stun_add_software) (function)
- [stun_add_priority](#stun_add_priority) (function)
- [stun_add_use_candidate](#stun_add_use_candidate) (function)
- [stun_add_ice_role](#stun_add_ice_role) (function)
- [stun_add_error](#stun_add_error) (function)
- [webrtc_ip_bytes](#webrtc_ip_bytes) (function)
- [webrtc_ip_text](#webrtc_ip_text) (function)
- [stun_add_address](#stun_add_address) (function)
- [stun_encode](#stun_encode) (function)
- [stun_is_stun](#stun_is_stun) (function)
- [stun_decode](#stun_decode) (function)
- [stun_attr](#stun_attr) (function)
- [stun_has_attr](#stun_has_attr) (function)
- [stun_mapped_address](#stun_mapped_address) (function)
- [stun_error](#stun_error) (function)
- [stun_check_integrity](#stun_check_integrity) (function)
- [stun_check_fingerprint](#stun_check_fingerprint) (function)
- [stun_binding_response](#stun_binding_response) (function)
- [WEBRTC_NEW:](#WEBRTC_NEW:) (const)
- [WEBRTC_CHECKING:](#WEBRTC_CHECKING:) (const)
- [WEBRTC_CONNECTED:](#WEBRTC_CONNECTED:) (const)
- [WEBRTC_FAILED:](#WEBRTC_FAILED:) (const)
- [WEBRTC_CLOSED:](#WEBRTC_CLOSED:) (const)
- [WebrtcConfig](#WebrtcConfig) (struct)
- [WebrtcDatagram](#WebrtcDatagram) (struct)
- [WebrtcPeer](#WebrtcPeer) (struct)
- [webrtc_state_name](#webrtc_state_name) (function)
- [webrtc_config](#webrtc_config) (function)
- [webrtc_peer_new](#webrtc_peer_new) (function)
- [webrtc_peer_offer](#webrtc_peer_offer) (function)
- [webrtc_peer_answer](#webrtc_peer_answer) (function)
- [webrtc_peer_set_remote](#webrtc_peer_set_remote) (function)
- [webrtc_peer_drain](#webrtc_peer_drain) (function)
- [webrtc_peer_poll](#webrtc_peer_poll) (function)
- [webrtc_peer_receive](#webrtc_peer_receive) (function)
- [webrtc_peer_send_rtp](#webrtc_peer_send_rtp) (function)
- [webrtc_peer_read_rtp](#webrtc_peer_read_rtp) (function)
- [webrtc_peer_read_rtcp](#webrtc_peer_read_rtcp) (function)
- [webrtc_peer_selected](#webrtc_peer_selected) (function)
- [webrtc_peer_close](#webrtc_peer_close) (function)

## SdpMedia

```fides
pub struct SdpMedia {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `port` | `i64` |
| `proto` | `str` |
| `fmts` | `Vec<str>` |
| `conn` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |

*Defined at line 84*

---

## Sdp

```fides
pub struct Sdp {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `version` | `i64` |
| `origin` | `str` |
| `session_name` | `str` |
| `timing` | `str` |
| `conn` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `media` | `Vec<SdpMedia>` |

*Defined at line 94*

---

## sdp_new

```fides
pub fn sdp_new(origin: str) -> Sdp {
```

**Parameters:**

- `origin`: `str`

**Returns:** `Sdp`

*Defined at line 107*

---

## sdp_parse

Parse SDP text (CRLF or LF lines). ok false with `error` on a line that
is not "x=...", a missing v=, or a bad m= line.

```fides
pub fn sdp_parse(text: str) -> Sdp {
```

**Parameters:**

- `text`: `str`

**Returns:** `Sdp`

*Defined at line 121*

---

## sdp_to_str

SDP text with CRLF line ends, in the order RFC 8866 gives.

```fides
pub fn sdp_to_str(s: &Sdp) -> str {
```

**Parameters:**

- `s`: `&Sdp`

**Returns:** `str`

*Defined at line 227*

---

## sdp_attr

The first value of attribute `name` ("" when absent).

```fides
pub fn sdp_attr(names: Vec<str>, vals: Vec<str>, name: str) -> str {
```

**Parameters:**

- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `name`: `str`

**Returns:** `str`

*Defined at line 253*

---

## sdp_has_attr

```fides
pub fn sdp_has_attr(names: Vec<str>, name: str) -> bool {
```

**Parameters:**

- `names`: `Vec<str>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 264*

---

## sdp_attrs

Every value of attribute `name`.

```fides
pub fn sdp_attrs(names: Vec<str>, vals: Vec<str>, name: str) -> Vec<str> {
```

**Parameters:**

- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 276*

---

## sdp_direction

sendrecv, sendonly, recvonly or inactive (sendrecv by default).

```fides
pub fn sdp_direction(m: &SdpMedia) -> str {
```

**Parameters:**

- `m`: `&SdpMedia`

**Returns:** `str`

*Defined at line 289*

---

## sdp_rtpmap

The rtpmap of payload type `pt`: "opus/48000/2", or "".

```fides
pub fn sdp_rtpmap(m: &SdpMedia, pt: str) -> str {
```

**Parameters:**

- `m`: `&SdpMedia`
- `pt`: `str`

**Returns:** `str`

*Defined at line 302*

---

## sdp_codecs

The codec of each format: "111 opus/48000/2", ...

```fides
pub fn sdp_codecs(m: &SdpMedia) -> Vec<str> {
```

**Parameters:**

- `m`: `&SdpMedia`

**Returns:** `Vec<str>`

*Defined at line 329*

---

## sdp_ice_ufrag

ICE ufrag of a media section, else of the session.

```fides
pub fn sdp_ice_ufrag(s: &Sdp, m: &SdpMedia) -> str {
```

**Parameters:**

- `s`: `&Sdp`
- `m`: `&SdpMedia`

**Returns:** `str`

*Defined at line 340*

---

## sdp_ice_pwd

```fides
pub fn sdp_ice_pwd(s: &Sdp, m: &SdpMedia) -> str {
```

**Parameters:**

- `s`: `&Sdp`
- `m`: `&SdpMedia`

**Returns:** `str`

*Defined at line 348*

---

## sdp_fingerprint

```fides
pub fn sdp_fingerprint(s: &Sdp, m: &SdpMedia) -> str {
```

**Parameters:**

- `s`: `&Sdp`
- `m`: `&SdpMedia`

**Returns:** `str`

*Defined at line 356*

---

## SdpAnswerConfig

What the answerer accepts and its own identity.

```fides
pub struct SdpAnswerConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `origin` | `str` |
| `ufrag` | `str` |
| `pwd` | `str` |
| `fingerprint` | `str` |
| `codecs` | `Vec<str>` |
| `data_channels` | `bool` |

*Defined at line 365*

---

## sdp_answer

The answer to `offer` under `cfg` (JSEP-style, all sections bundled
when the offer bundles them).

```fides
pub fn sdp_answer(offer: &Sdp, cfg: SdpAnswerConfig) -> Sdp {
```

**Parameters:**

- `offer`: `&Sdp`
- `cfg`: `SdpAnswerConfig`

**Returns:** `Sdp`

*Defined at line 394*

---

## IceCandidate

```fides
pub struct IceCandidate {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `foundation` | `str` |
| `component` | `i64` |
| `transport` | `str` |
| `priority` | `i64` |
| `address` | `str` |
| `port` | `i64` |
| `typ` | `str` |
| `raddr` | `str` |
| `rport` | `i64` |
| `tcptype` | `str` |
| `ext` | `str` |

*Defined at line 515*

---

## ice_parse_candidate

Parse "candidate:..." (with or without "a=").

```fides
pub fn ice_parse_candidate(line: str) -> IceCandidate {
```

**Parameters:**

- `line`: `str`

**Returns:** `IceCandidate`

*Defined at line 531*

---

## ice_candidate_to_str

```fides
pub fn ice_candidate_to_str(c: &IceCandidate) -> str {
```

**Parameters:**

- `c`: `&IceCandidate`

**Returns:** `str`

*Defined at line 574*

---

## ice_type_pref

RFC 8445's recommended type preferences.

```fides
pub fn ice_type_pref(typ: str) -> i64 {
```

**Parameters:**

- `typ`: `str`

**Returns:** `i64`

*Defined at line 589*

---

## ice_priority

priority = 2^24 * type pref + 2^8 * local pref + (256 - component).

```fides
pub fn ice_priority(type_pref: i64, local_pref: i64, component: i64) -> i64 {
```

**Parameters:**

- `type_pref`: `i64`
- `local_pref`: `i64`
- `component`: `i64`

**Returns:** `i64`

*Defined at line 603*

---

## ice_pair_priority

Pair priority from the controlling (g) and controlled (d) candidates'
priorities.

```fides
pub fn ice_pair_priority(g: i64, d: i64) -> i64 {
```

**Parameters:**

- `g`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 609*

---

## IcePair

```fides
pub struct IcePair {
```

**Fields:**

| Name | Type |
|------|------|
| `local` | `i64` |
| `remote` | `i64` |
| `priority` | `i64` |
| `foundation` | `str` |

*Defined at line 627*

---

## ice_pairs

Candidate pairs, highest priority first. A local srflx candidate is
left out when the host candidate it comes from (raddr/rport) is there:
checks are sent from the base, so the pair would repeat the host's.

```fides
pub fn ice_pairs(locals: Vec<IceCandidate>, remotes: Vec<IceCandidate>, controlling: bool) -> Vec<IcePair> {
```

**Parameters:**

- `locals`: `Vec<IceCandidate>`
- `remotes`: `Vec<IceCandidate>`
- `controlling`: `bool`

**Returns:** `Vec<IcePair>`

*Defined at line 637*

---

## webrtc_sha1

SHA-1 (FIPS 180-4).

```fides
pub fn webrtc_sha1(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 687*

---

## webrtc_hmac_sha1

HMAC-SHA1 (RFC 2104).

```fides
pub fn webrtc_hmac_sha1(key: Vec<i64>, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `Vec<i64>`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 779*

---

## webrtc_crc32

CRC-32 (ISO-HDLC, the zlib one).

```fides
pub fn webrtc_crc32(b: Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 811*

---

## STUN_BINDING:

```fides
pub const STUN_BINDING: i64 = 1;
```

*Defined at line 832*

---

## STUN_REQUEST:

```fides
pub const STUN_REQUEST: i64 = 0;
```

*Defined at line 833*

---

## STUN_INDICATION:

```fides
pub const STUN_INDICATION: i64 = 1;
```

*Defined at line 834*

---

## STUN_SUCCESS:

```fides
pub const STUN_SUCCESS: i64 = 2;
```

*Defined at line 835*

---

## STUN_ERROR:

```fides
pub const STUN_ERROR: i64 = 3;
```

*Defined at line 836*

---

## STUN_MAGIC:

```fides
pub const STUN_MAGIC: i64 = 554869826;
```

*Defined at line 837*

---

## STUN_MAPPED_ADDRESS:

```fides
pub const STUN_MAPPED_ADDRESS: i64 = 1;
```

*Defined at line 839*

---

## STUN_USERNAME:

```fides
pub const STUN_USERNAME: i64 = 6;
```

*Defined at line 840*

---

## STUN_MESSAGE_INTEGRITY:

```fides
pub const STUN_MESSAGE_INTEGRITY: i64 = 8;
```

*Defined at line 841*

---

## STUN_ERROR_CODE:

```fides
pub const STUN_ERROR_CODE: i64 = 9;
```

*Defined at line 842*

---

## STUN_XOR_MAPPED_ADDRESS:

```fides
pub const STUN_XOR_MAPPED_ADDRESS: i64 = 32;
```

*Defined at line 843*

---

## STUN_PRIORITY:

```fides
pub const STUN_PRIORITY: i64 = 36;
```

*Defined at line 844*

---

## STUN_USE_CANDIDATE:

```fides
pub const STUN_USE_CANDIDATE: i64 = 37;
```

*Defined at line 845*

---

## STUN_SOFTWARE:

```fides
pub const STUN_SOFTWARE: i64 = 32802;
```

*Defined at line 846*

---

## STUN_FINGERPRINT:

```fides
pub const STUN_FINGERPRINT: i64 = 32808;
```

*Defined at line 847*

---

## STUN_ICE_CONTROLLED:

```fides
pub const STUN_ICE_CONTROLLED: i64 = 32809;
```

*Defined at line 848*

---

## STUN_ICE_CONTROLLING:

```fides
pub const STUN_ICE_CONTROLLING: i64 = 32810;
```

*Defined at line 849*

---

## stun_type

The 14-bit message type of a method and class.

```fides
pub fn stun_type(method: i64, class: i64) -> i64 {
```

**Parameters:**

- `method`: `i64`
- `class`: `i64`

**Returns:** `i64`

*Defined at line 852*

---

## stun_class

```fides
pub fn stun_class(mtype: i64) -> i64 {
```

**Parameters:**

- `mtype`: `i64`

**Returns:** `i64`

*Defined at line 856*

---

## stun_method

```fides
pub fn stun_method(mtype: i64) -> i64 {
```

**Parameters:**

- `mtype`: `i64`

**Returns:** `i64`

*Defined at line 860*

---

## StunMsg

A message: type, transaction id (12 bytes) and attributes in order.
After stun_decode: integrity_at / fingerprint_at are the byte offsets
of those attributes (-1 when absent).

```fides
pub struct StunMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `mtype` | `i64` |
| `txid` | `Vec<i64>` |
| `types` | `Vec<i64>` |
| `vals` | `Vec<Vec<i64>>` |
| `integrity_at` | `i64` |
| `fingerprint_at` | `i64` |
| `pad` | `i64` |

*Defined at line 867*

---

## stun_new

```fides
pub fn stun_new(method: i64, class: i64, txid: Vec<i64>) -> StunMsg {
```

**Parameters:**

- `method`: `i64`
- `class`: `i64`
- `txid`: `Vec<i64>`

**Returns:** `StunMsg`

*Defined at line 879*

---

## stun_add_attr

```fides
pub fn stun_add_attr(m: &mut StunMsg, t: i64, v: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `t`: `i64`
- `v`: `Vec<i64>`

*Defined at line 885*

---

## stun_add_username

```fides
pub fn stun_add_username(m: &mut StunMsg, user: str) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `user`: `str`

*Defined at line 900*

---

## stun_add_software

```fides
pub fn stun_add_software(m: &mut StunMsg, name: str) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `name`: `str`

*Defined at line 904*

---

## stun_add_priority

```fides
pub fn stun_add_priority(m: &mut StunMsg, prio: i64) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `prio`: `i64`

*Defined at line 908*

---

## stun_add_use_candidate

```fides
pub fn stun_add_use_candidate(m: &mut StunMsg) {
```

**Parameters:**

- `m`: `&mut StunMsg`

*Defined at line 912*

---

## stun_add_ice_role

ICE-CONTROLLING (controlling true) or ICE-CONTROLLED with a tiebreaker
given as 8 bytes.

```fides
pub fn stun_add_ice_role(m: &mut StunMsg, controlling: bool, tiebreaker: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `controlling`: `bool`
- `tiebreaker`: `Vec<i64>`

*Defined at line 919*

---

## stun_add_error

```fides
pub fn stun_add_error(m: &mut StunMsg, code: i64, reason: str) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `code`: `i64`
- `reason`: `str`

*Defined at line 927*

---

## webrtc_ip_bytes

IPv4 "a.b.c.d" or IPv6 text to bytes (empty when not an address).

```fides
pub fn webrtc_ip_bytes(ip: str) -> Vec<i64> {
```

**Parameters:**

- `ip`: `str`

**Returns:** `Vec<i64>`

*Defined at line 943*

---

## webrtc_ip_text

Address bytes as text (IPv6 per RFC 5952: lowercase, longest zero run
of two or more groups as "::").

```fides
pub fn webrtc_ip_text(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1021*

---

## stun_add_address

XOR-MAPPED-ADDRESS (xor true) or MAPPED-ADDRESS for an IP and port.

```fides
pub fn stun_add_address(m: &mut StunMsg, xor: bool, ip: str, port: i64) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `xor`: `bool`
- `ip`: `str`
- `port`: `i64`

*Defined at line 1086*

---

## stun_encode

Encode: attributes (padded with m.pad bytes), then MESSAGE-INTEGRITY
when `key` is not empty (short-term: the password's bytes), then
FINGERPRINT when asked.

```fides
pub fn stun_encode(m: &StunMsg, key: Vec<i64>, fingerprint: bool) -> Vec<i64> {
```

**Parameters:**

- `m`: `&StunMsg`
- `key`: `Vec<i64>`
- `fingerprint`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1149*

---

## stun_is_stun

Is this datagram STUN (first two bits 0, the magic cookie)?

```fides
pub fn stun_is_stun(b: Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1185*

---

## stun_decode

Decode a message; attributes after MESSAGE-INTEGRITY other than
FINGERPRINT are ignored (RFC 5389 15.4).

```fides
pub fn stun_decode(b: Vec<i64>) -> StunMsg {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `StunMsg`

*Defined at line 1191*

---

## stun_attr

The value of attribute t (empty when absent).

```fides
pub fn stun_attr(m: &StunMsg, t: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&StunMsg`
- `t`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1247*

---

## stun_has_attr

```fides
pub fn stun_has_attr(m: &StunMsg, t: i64) -> bool {
```

**Parameters:**

- `m`: `&StunMsg`
- `t`: `i64`

**Returns:** `bool`

*Defined at line 1259*

---

## stun_mapped_address

"ip:port" ("[ip]:port" for IPv6) of (XOR-)MAPPED-ADDRESS; "" if none.

```fides
pub fn stun_mapped_address(m: &StunMsg) -> str {
```

**Parameters:**

- `m`: `&StunMsg`

**Returns:** `str`

*Defined at line 1271*

---

## stun_error

ERROR-CODE as "code reason" ("" if none).

```fides
pub fn stun_error(m: &StunMsg) -> str {
```

**Parameters:**

- `m`: `&StunMsg`

**Returns:** `str`

*Defined at line 1310*

---

## stun_check_integrity

Check MESSAGE-INTEGRITY of the raw message `b` with `key`.

```fides
pub fn stun_check_integrity(b: Vec<i64>, key: Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`
- `key`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1325*

---

## stun_check_fingerprint

Check FINGERPRINT (it must be the last attribute).

```fides
pub fn stun_check_fingerprint(b: Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1348*

---

## stun_binding_response

A Binding success response to request `req`, from `ip`:`port`, with
integrity under `key` and a fingerprint (what an ICE agent answers).

```fides
pub fn stun_binding_response(req: &StunMsg, ip: str, port: i64, key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `req`: `&StunMsg`
- `ip`: `str`
- `port`: `i64`
- `key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1366*

---

## WEBRTC_NEW:

```fides
pub const WEBRTC_NEW: i64 = 0;
```

*Defined at line 1374*

---

## WEBRTC_CHECKING:

```fides
pub const WEBRTC_CHECKING: i64 = 1;
```

*Defined at line 1375*

---

## WEBRTC_CONNECTED:

```fides
pub const WEBRTC_CONNECTED: i64 = 2;
```

*Defined at line 1376*

---

## WEBRTC_FAILED:

```fides
pub const WEBRTC_FAILED: i64 = 3;
```

*Defined at line 1377*

---

## WEBRTC_CLOSED:

```fides
pub const WEBRTC_CLOSED: i64 = 4;
```

*Defined at line 1378*

---

## WebrtcConfig

One endpoint: certificate and P-256 key (PEM), ICE credentials, the
host address it receives on, its codecs ("111 opus/48000/2", in
preference order), entropy, the DTLS MTU and how long ICE may take.

```fides
pub struct WebrtcConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `cert_pem` | `str` |
| `key_pem` | `str` |
| `ufrag` | `str` |
| `pwd` | `str` |
| `ip` | `str` |
| `port` | `i64` |
| `codecs` | `Vec<str>` |
| `seed` | `Vec<i64>` |
| `mtu` | `i64` |
| `ice_timeout_ms` | `i64` |

*Defined at line 1391*

---

## WebrtcDatagram

A datagram to send to ip:port (from our host address).

```fides
pub struct WebrtcDatagram {
```

**Fields:**

| Name | Type |
|------|------|
| `ip` | `str` |
| `port` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 1405*

---

## WebrtcPeer

A peer connection. `history` lists the states it went through
("new > checking > connected").

```fides
pub struct WebrtcPeer {
```

**Fields:**

| Name | Type |
|------|------|
| `cfg` | `WebrtcConfig` |
| `state` | `i64` |
| `ice_state` | `i64` |
| `history` | `str` |
| `err` | `str` |
| `offerer` | `bool` |
| `has_remote` | `bool` |
| `fingerprint` | `str` |
| `remote_sdp` | `str` |
| `remote_ufrag` | `str` |
| `remote_pwd` | `str` |
| `remote_fingerprint` | `str` |
| `dtls_client` | `bool` |
| `locals` | `Vec<IceCandidate>` |
| `remotes` | `Vec<IceCandidate>` |
| `pairs` | `Vec<IcePair>` |
| `p_state` | `Vec<i64>` |
| `p_tries` | `Vec<i64>` |
| `p_next` | `Vec<i64>` |
| `p_rto` | `Vec<i64>` |
| `p_nominated` | `Vec<i64>` |
| `p_txid` | `Vec<Vec<i64>>` |
| `selected` | `i64` |
| `tiebreaker` | `Vec<i64>` |
| `rng_key` | `Vec<i64>` |
| `rng_ctr` | `i64` |
| `started_at` | `i64` |
| `next_check` | `i64` |
| `now` | `i64` |
| `dtls` | `DtlsConn` |
| `dtls_started` | `bool` |
| `dtls_ip` | `str` |
| `dtls_port` | `i64` |
| `media` | `bool` |
| `srtp_tx` | `SrtpContext` |
| `srtp_rx` | `SrtpContext` |
| `out` | `Vec<WebrtcDatagram>` |
| `rtp_in` | `Vec<Vec<i64>>` |
| `rtcp_in` | `Vec<Vec<i64>>` |
| `checks_sent` | `i64` |
| `srtp_dropped` | `i64` |

*Defined at line 1413*

---

## webrtc_state_name

```fides
pub fn webrtc_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 1457*

---

## webrtc_config

A configuration with Opus and PCMU, a 1200-byte MTU, a 30 s ICE
limit and a seed from the OS generator.

```fides
pub fn webrtc_config(cert_pem: str, key_pem: str, ufrag: str, pwd: str, ip: str, port: i64) -> WebrtcConfig {
```

**Parameters:**

- `cert_pem`: `str`
- `key_pem`: `str`
- `ufrag`: `str`
- `pwd`: `str`
- `ip`: `str`
- `port`: `i64`

**Returns:** `WebrtcConfig`

*Defined at line 1475*

---

## webrtc_peer_new

A peer connection; `offerer` true makes it the side that offers (and
the ICE controlling agent).

```fides
pub fn webrtc_peer_new(cfg: WebrtcConfig, offerer: bool) -> WebrtcPeer {
```

**Parameters:**

- `cfg`: `WebrtcConfig`
- `offerer`: `bool`

**Returns:** `WebrtcPeer`

*Defined at line 1528*

---

## webrtc_peer_offer

The offer: one bundled audio section (UDP/TLS/RTP/SAVPF, rtcp-mux)
with our codecs, ICE credentials, host candidate, fingerprint and
a=setup:actpass.

```fides
pub fn webrtc_peer_offer(p: &mut WebrtcPeer) -> str {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

**Returns:** `str`

*Defined at line 1567*

---

## webrtc_peer_answer

The answer to the offer given with webrtc_peer_set_remote: sdp_answer
with our codecs, credentials and fingerprint, plus our candidates.

```fides
pub fn webrtc_peer_answer(p: &mut WebrtcPeer) -> str {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

**Returns:** `str`

*Defined at line 1618*

---

## webrtc_peer_set_remote

Take the other side's offer (answerer) or answer (offerer): ICE
credentials, candidates, fingerprint and DTLS role of the first media
section; candidate pairs are formed. False (and failed) when the SDP
is unusable.

```fides
pub fn webrtc_peer_set_remote(p: &mut WebrtcPeer, text: str) -> bool {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`
- `text`: `str`

**Returns:** `bool`

*Defined at line 1656*

---

## webrtc_peer_drain

Datagrams to send, in order; the queue is emptied.

```fides
pub fn webrtc_peer_drain(p: &mut WebrtcPeer) -> Vec<WebrtcDatagram> {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

**Returns:** `Vec<WebrtcDatagram>`

*Defined at line 1723*

---

## webrtc_peer_poll

Run timers at `now` (milliseconds): paced and retransmitted ICE checks,
the ICE time limit, DTLS retransmission.

```fides
pub fn webrtc_peer_poll(p: &mut WebrtcPeer, now: i64) {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`
- `now`: `i64`

*Defined at line 1905*

---

## webrtc_peer_receive

A datagram from ip:port: STUN, DTLS or SRTP/SRTCP by its first byte
(RFC 7983).

```fides
pub fn webrtc_peer_receive(p: &mut WebrtcPeer, ip: str, port: i64, data: Vec<i64>, now: i64) {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`
- `ip`: `str`
- `port`: `i64`
- `data`: `Vec<i64>`
- `now`: `i64`

*Defined at line 1961*

---

## webrtc_peer_send_rtp

Protect and send an RTP or RTCP packet; false before connected.

```fides
pub fn webrtc_peer_send_rtp(p: &mut WebrtcPeer, pkt: Vec<i64>) -> bool {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`
- `pkt`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 2003*

---

## webrtc_peer_read_rtp

RTP packets received (decrypted), in order; the queue is emptied.

```fides
pub fn webrtc_peer_read_rtp(p: &mut WebrtcPeer) -> Vec<Vec<i64>> {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2028*

---

## webrtc_peer_read_rtcp

RTCP packets received (decrypted); the queue is emptied.

```fides
pub fn webrtc_peer_read_rtcp(p: &mut WebrtcPeer) -> Vec<Vec<i64>> {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2035*

---

## webrtc_peer_selected

The selected pair as "local ip:port -> remote ip:port" ("" before).

```fides
pub fn webrtc_peer_selected(p: &WebrtcPeer) -> str {
```

**Parameters:**

- `p`: `&WebrtcPeer`

**Returns:** `str`

*Defined at line 2042*

---

## webrtc_peer_close

Close: DTLS close_notify to the peer, state closed.

```fides
pub fn webrtc_peer_close(p: &mut WebrtcPeer) {
```

**Parameters:**

- `p`: `&mut WebrtcPeer`

*Defined at line 2053*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
