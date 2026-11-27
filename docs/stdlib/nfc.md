[All modules](index.md)

# nfc -- Fides Documentation

## Contents

- [TNF_EMPTY:](#TNF_EMPTY:) (const)
- [TNF_WELL_KNOWN:](#TNF_WELL_KNOWN:) (const)
- [TNF_MEDIA:](#TNF_MEDIA:) (const)
- [TNF_ABSOLUTE_URI:](#TNF_ABSOLUTE_URI:) (const)
- [TNF_EXTERNAL:](#TNF_EXTERNAL:) (const)
- [TNF_UNKNOWN:](#TNF_UNKNOWN:) (const)
- [TNF_UNCHANGED:](#TNF_UNCHANGED:) (const)
- [NDEF_MB:](#NDEF_MB:) (const)
- [NDEF_ME:](#NDEF_ME:) (const)
- [NDEF_CF:](#NDEF_CF:) (const)
- [NDEF_SR:](#NDEF_SR:) (const)
- [NDEF_IL:](#NDEF_IL:) (const)
- [NdefRecord](#NdefRecord) (struct)
- [ndef_record](#ndef_record) (function)
- [ndef_type_is](#ndef_type_is) (function)
- [ndef_encode](#ndef_encode) (function)
- [NdefMessage](#NdefMessage) (struct)
- [ndef_decode](#ndef_decode) (function)
- [nfc_uri_prefixes](#nfc_uri_prefixes) (function)
- [ndef_uri_record](#ndef_uri_record) (function)
- [ndef_uri](#ndef_uri) (function)
- [ndef_text_record](#ndef_text_record) (function)
- [NdefText](#NdefText) (struct)
- [ndef_text](#ndef_text) (function)
- [ndef_mime_record](#ndef_mime_record) (function)
- [ndef_external_record](#ndef_external_record) (function)
- [ndef_android_app_record](#ndef_android_app_record) (function)
- [NFC_ACTION_NONE:](#NFC_ACTION_NONE:) (const)
- [NFC_ACTION_DO:](#NFC_ACTION_DO:) (const)
- [NFC_ACTION_SAVE:](#NFC_ACTION_SAVE:) (const)
- [NFC_ACTION_EDIT:](#NFC_ACTION_EDIT:) (const)
- [SmartPoster](#SmartPoster) (struct)
- [smart_poster_new](#smart_poster_new) (function)
- [smart_poster_title](#smart_poster_title) (function)
- [ndef_smart_poster_record](#ndef_smart_poster_record) (function)
- [ndef_smart_poster](#ndef_smart_poster) (function)
- [nfc_tlv_wrap](#nfc_tlv_wrap) (function)
- [nfc_tlv_find](#nfc_tlv_find) (function)
- [nfc_type2_cc](#nfc_type2_cc) (function)
- [NfcCall](#NfcCall) (struct)
- [nfc_android_calls](#nfc_android_calls) (function)
- [nfc_ios_calls](#nfc_ios_calls) (function)

## TNF_EMPTY:

```fides
pub const TNF_EMPTY: i64 = 0;
```

*Defined at line 30*

---

## TNF_WELL_KNOWN:

```fides
pub const TNF_WELL_KNOWN: i64 = 1;
```

*Defined at line 31*

---

## TNF_MEDIA:

```fides
pub const TNF_MEDIA: i64 = 2;
```

*Defined at line 32*

---

## TNF_ABSOLUTE_URI:

```fides
pub const TNF_ABSOLUTE_URI: i64 = 3;
```

*Defined at line 33*

---

## TNF_EXTERNAL:

```fides
pub const TNF_EXTERNAL: i64 = 4;
```

*Defined at line 34*

---

## TNF_UNKNOWN:

```fides
pub const TNF_UNKNOWN: i64 = 5;
```

*Defined at line 35*

---

## TNF_UNCHANGED:

```fides
pub const TNF_UNCHANGED: i64 = 6;
```

*Defined at line 36*

---

## NDEF_MB:

```fides
pub const NDEF_MB: i64 = 0x80;
```

*Defined at line 38*

---

## NDEF_ME:

```fides
pub const NDEF_ME: i64 = 0x40;
```

*Defined at line 39*

---

## NDEF_CF:

```fides
pub const NDEF_CF: i64 = 0x20;
```

*Defined at line 40*

---

## NDEF_SR:

```fides
pub const NDEF_SR: i64 = 0x10;
```

*Defined at line 41*

---

## NDEF_IL:

```fides
pub const NDEF_IL: i64 = 0x08;
```

*Defined at line 42*

---

## NdefRecord

```fides
pub struct NdefRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `tnf` | `i64` |
| `rtype` | `Vec<i64>` |
| `id` | `Vec<i64>` |
| `payload` | `Vec<i64>` |

*Defined at line 44*

---

## ndef_record

```fides
pub fn ndef_record(tnf: i64, rtype: str, payload: Vec<i64>) -> NdefRecord {
```

**Parameters:**

- `tnf`: `i64`
- `rtype`: `str`
- `payload`: `Vec<i64>`

**Returns:** `NdefRecord`

*Defined at line 51*

---

## ndef_type_is

```fides
pub fn ndef_type_is(r: &NdefRecord, tnf: i64, t: str) -> bool {
```

**Parameters:**

- `r`: `&NdefRecord`
- `tnf`: `i64`
- `t`: `str`

**Returns:** `bool`

*Defined at line 89*

---

## ndef_encode

Encode a message. `chunk` > 0 splits payloads longer than it into
chunked records of at most `chunk` bytes.

```fides
pub fn ndef_encode(records: Vec<NdefRecord>, chunk: i64) -> Vec<i64> {
```

**Parameters:**

- `records`: `Vec<NdefRecord>`
- `chunk`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 124*

---

## NdefMessage

A decoded message: `ok` false (and `error`) on malformed bytes.

```fides
pub struct NdefMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `records` | `Vec<NdefRecord>` |

*Defined at line 174*

---

## ndef_decode

```fides
pub fn ndef_decode(b: Vec<i64>) -> NdefMessage {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `NdefMessage`

*Defined at line 186*

---

## nfc_uri_prefixes

The URI identifier code table (NFC Forum URI RTD).

```fides
pub fn nfc_uri_prefixes() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 288*

---

## ndef_uri_record

```fides
pub fn ndef_uri_record(uri: str) -> NdefRecord {
```

**Parameters:**

- `uri`: `str`

**Returns:** `NdefRecord`

*Defined at line 329*

---

## ndef_uri

The URI of a URI record (or of an absolute-URI record); "" otherwise.

```fides
pub fn ndef_uri(r: &NdefRecord) -> str {
```

**Parameters:**

- `r`: `&NdefRecord`

**Returns:** `str`

*Defined at line 347*

---

## ndef_text_record

A Text record: UTF-8, or UTF-16 big-endian when `utf16`.

```fides
pub fn ndef_text_record(text: str, lang: str, utf16: bool) -> NdefRecord {
```

**Parameters:**

- `text`: `str`
- `lang`: `str`
- `utf16`: `bool`

**Returns:** `NdefRecord`

*Defined at line 367*

---

## NdefText

```fides
pub struct NdefText {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `lang` | `str` |
| `text` | `str` |
| `utf16` | `bool` |

*Defined at line 402*

---

## ndef_text

```fides
pub fn ndef_text(r: &NdefRecord) -> NdefText {
```

**Parameters:**

- `r`: `&NdefRecord`

**Returns:** `NdefText`

*Defined at line 409*

---

## ndef_mime_record

```fides
pub fn ndef_mime_record(mime_type: str, data: Vec<i64>) -> NdefRecord {
```

**Parameters:**

- `mime_type`: `str`
- `data`: `Vec<i64>`

**Returns:** `NdefRecord`

*Defined at line 469*

---

## ndef_external_record

An external type "domain:type" (NFC Forum external RTD, lower case).

```fides
pub fn ndef_external_record(domain_type: str, data: Vec<i64>) -> NdefRecord {
```

**Parameters:**

- `domain_type`: `str`
- `data`: `Vec<i64>`

**Returns:** `NdefRecord`

*Defined at line 474*

---

## ndef_android_app_record

The Android application record that makes the tag open `package`.

```fides
pub fn ndef_android_app_record(package: str) -> NdefRecord {
```

**Parameters:**

- `package`: `str`

**Returns:** `NdefRecord`

*Defined at line 479*

---

## NFC_ACTION_NONE:

```fides
pub const NFC_ACTION_NONE: i64 = -1;
```

*Defined at line 485*

---

## NFC_ACTION_DO:

```fides
pub const NFC_ACTION_DO: i64 = 0;
```

*Defined at line 486*

---

## NFC_ACTION_SAVE:

```fides
pub const NFC_ACTION_SAVE: i64 = 1;
```

*Defined at line 487*

---

## NFC_ACTION_EDIT:

```fides
pub const NFC_ACTION_EDIT: i64 = 2;
```

*Defined at line 488*

---

## SmartPoster

```fides
pub struct SmartPoster {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `uri` | `str` |
| `titles` | `Vec<str>` |
| `langs` | `Vec<str>` |
| `action` | `i64` |
| `size` | `i64,  // -1 none` |
| `mime` | `str` |

*Defined at line 490*

---

## smart_poster_new

```fides
pub fn smart_poster_new(uri: str) -> SmartPoster {
```

**Parameters:**

- `uri`: `str`

**Returns:** `SmartPoster`

*Defined at line 500*

---

## smart_poster_title

```fides
pub fn smart_poster_title(sp: &mut SmartPoster, lang: str, title: str) {
```

**Parameters:**

- `sp`: `&mut SmartPoster`
- `lang`: `str`
- `title`: `str`

*Defined at line 506*

---

## ndef_smart_poster_record

```fides
pub fn ndef_smart_poster_record(sp: &SmartPoster) -> NdefRecord {
```

**Parameters:**

- `sp`: `&SmartPoster`

**Returns:** `NdefRecord`

*Defined at line 511*

---

## ndef_smart_poster

```fides
pub fn ndef_smart_poster(r: &NdefRecord) -> SmartPoster {
```

**Parameters:**

- `r`: `&NdefRecord`

**Returns:** `SmartPoster`

*Defined at line 538*

---

## nfc_tlv_wrap

The NDEF message TLV (0x03, length, value) followed by a terminator
TLV (0xFE), as written to a Type 2 tag's data area.

```fides
pub fn nfc_tlv_wrap(message: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `message`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 578*

---

## nfc_tlv_find

The first NDEF message TLV of a data area (NULL and other TLVs are
skipped); empty when there is none.

```fides
pub fn nfc_tlv_find(area: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `area`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 596*

---

## nfc_type2_cc

The capability container of a Type 2 tag with `data_bytes` of NDEF
memory: magic E1, version 1.0, size / 8, read/write access 00.

```fides
pub fn nfc_type2_cc(data_bytes: i64) -> Vec<i64> {
```

**Parameters:**

- `data_bytes`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 631*

---

## NfcCall

```fides
pub struct NfcCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |

*Defined at line 642*

---

## nfc_android_calls

```fides
pub fn nfc_android_calls() -> Vec<NfcCall> {
```

**Returns:** `Vec<NfcCall>`

*Defined at line 648*

---

## nfc_ios_calls

```fides
pub fn nfc_ios_calls() -> Vec<NfcCall> {
```

**Returns:** `Vec<NfcCall>`

*Defined at line 671*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
