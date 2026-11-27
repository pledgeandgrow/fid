[All modules](index.md)

# asn1 -- Fides Documentation

## Contents

- [ASN1_BOOLEAN:](#ASN1_BOOLEAN:) (const)
- [ASN1_INTEGER:](#ASN1_INTEGER:) (const)
- [ASN1_BIT_STRING:](#ASN1_BIT_STRING:) (const)
- [ASN1_OCTET_STRING:](#ASN1_OCTET_STRING:) (const)
- [ASN1_NULL:](#ASN1_NULL:) (const)
- [ASN1_OID:](#ASN1_OID:) (const)
- [ASN1_ENUMERATED:](#ASN1_ENUMERATED:) (const)
- [ASN1_UTF8_STRING:](#ASN1_UTF8_STRING:) (const)
- [ASN1_SEQUENCE:](#ASN1_SEQUENCE:) (const)
- [ASN1_SET:](#ASN1_SET:) (const)
- [ASN1_PRINTABLE_STRING:](#ASN1_PRINTABLE_STRING:) (const)
- [ASN1_T61_STRING:](#ASN1_T61_STRING:) (const)
- [ASN1_IA5_STRING:](#ASN1_IA5_STRING:) (const)
- [ASN1_UTC_TIME:](#ASN1_UTC_TIME:) (const)
- [ASN1_GENERALIZED_TIME:](#ASN1_GENERALIZED_TIME:) (const)
- [ASN1_BMP_STRING:](#ASN1_BMP_STRING:) (const)
- [ASN1_CLASS_UNIVERSAL:](#ASN1_CLASS_UNIVERSAL:) (const)
- [ASN1_CLASS_APPLICATION:](#ASN1_CLASS_APPLICATION:) (const)
- [ASN1_CLASS_CONTEXT:](#ASN1_CLASS_CONTEXT:) (const)
- [ASN1_CLASS_PRIVATE:](#ASN1_CLASS_PRIVATE:) (const)
- [Asn1Tlv](#Asn1Tlv) (struct)
- [asn1_read](#asn1_read) (function)
- [asn1_read_der](#asn1_read_der) (function)
- [asn1_is](#asn1_is) (function)
- [asn1_is_ctx](#asn1_is_ctx) (function)
- [asn1_children](#asn1_children) (function)
- [asn1_count](#asn1_count) (function)
- [asn1_slice](#asn1_slice) (function)
- [asn1_content](#asn1_content) (function)
- [asn1_raw](#asn1_raw) (function)
- [asn1_get_int](#asn1_get_int) (function)
- [asn1_int_minimal](#asn1_int_minimal) (function)
- [asn1_int_negative](#asn1_int_negative) (function)
- [asn1_get_uint_bytes](#asn1_get_uint_bytes) (function)
- [asn1_get_bool](#asn1_get_bool) (function)
- [asn1_get_oid](#asn1_get_oid) (function)
- [asn1_bits_unused](#asn1_bits_unused) (function)
- [asn1_get_bits](#asn1_get_bits) (function)
- [asn1_get_str](#asn1_get_str) (function)
- [asn1_days_from_civil](#asn1_days_from_civil) (function)
- [asn1_unix_time](#asn1_unix_time) (function)
- [asn1_civil_from_days](#asn1_civil_from_days) (function)
- [asn1_get_time](#asn1_get_time) (function)
- [asn1_cat](#asn1_cat) (function)
- [asn1_enc_len](#asn1_enc_len) (function)
- [asn1_enc_tag](#asn1_enc_tag) (function)
- [asn1_enc_tlv](#asn1_enc_tlv) (function)
- [asn1_enc_int](#asn1_enc_int) (function)
- [asn1_enc_uint](#asn1_enc_uint) (function)
- [asn1_enc_bool](#asn1_enc_bool) (function)
- [asn1_enc_null](#asn1_enc_null) (function)
- [asn1_oid_content](#asn1_oid_content) (function)
- [asn1_enc_oid](#asn1_enc_oid) (function)
- [asn1_enc_octets](#asn1_enc_octets) (function)
- [asn1_enc_bits](#asn1_enc_bits) (function)
- [asn1_enc_utf8](#asn1_enc_utf8) (function)
- [asn1_is_printable](#asn1_is_printable) (function)
- [asn1_enc_printable](#asn1_enc_printable) (function)
- [asn1_enc_ia5](#asn1_enc_ia5) (function)
- [asn1_enc_utctime](#asn1_enc_utctime) (function)
- [asn1_enc_gentime](#asn1_enc_gentime) (function)
- [asn1_enc_time](#asn1_enc_time) (function)
- [asn1_enc_seq](#asn1_enc_seq) (function)
- [asn1_enc_set](#asn1_enc_set) (function)
- [asn1_bytes_cmp](#asn1_bytes_cmp) (function)
- [asn1_enc_set_of](#asn1_enc_set_of) (function)
- [asn1_enc_explicit](#asn1_enc_explicit) (function)
- [asn1_enc_implicit](#asn1_enc_implicit) (function)
- [asn1_path](#asn1_path) (function)

## ASN1_BOOLEAN:

```fides
pub const ASN1_BOOLEAN: i64 = 1;
```

*Defined at line 34*

---

## ASN1_INTEGER:

```fides
pub const ASN1_INTEGER: i64 = 2;
```

*Defined at line 35*

---

## ASN1_BIT_STRING:

```fides
pub const ASN1_BIT_STRING: i64 = 3;
```

*Defined at line 36*

---

## ASN1_OCTET_STRING:

```fides
pub const ASN1_OCTET_STRING: i64 = 4;
```

*Defined at line 37*

---

## ASN1_NULL:

```fides
pub const ASN1_NULL: i64 = 5;
```

*Defined at line 38*

---

## ASN1_OID:

```fides
pub const ASN1_OID: i64 = 6;
```

*Defined at line 39*

---

## ASN1_ENUMERATED:

```fides
pub const ASN1_ENUMERATED: i64 = 10;
```

*Defined at line 40*

---

## ASN1_UTF8_STRING:

```fides
pub const ASN1_UTF8_STRING: i64 = 12;
```

*Defined at line 41*

---

## ASN1_SEQUENCE:

```fides
pub const ASN1_SEQUENCE: i64 = 16;
```

*Defined at line 42*

---

## ASN1_SET:

```fides
pub const ASN1_SET: i64 = 17;
```

*Defined at line 43*

---

## ASN1_PRINTABLE_STRING:

```fides
pub const ASN1_PRINTABLE_STRING: i64 = 19;
```

*Defined at line 44*

---

## ASN1_T61_STRING:

```fides
pub const ASN1_T61_STRING: i64 = 20;
```

*Defined at line 45*

---

## ASN1_IA5_STRING:

```fides
pub const ASN1_IA5_STRING: i64 = 22;
```

*Defined at line 46*

---

## ASN1_UTC_TIME:

```fides
pub const ASN1_UTC_TIME: i64 = 23;
```

*Defined at line 47*

---

## ASN1_GENERALIZED_TIME:

```fides
pub const ASN1_GENERALIZED_TIME: i64 = 24;
```

*Defined at line 48*

---

## ASN1_BMP_STRING:

```fides
pub const ASN1_BMP_STRING: i64 = 30;
```

*Defined at line 49*

---

## ASN1_CLASS_UNIVERSAL:

```fides
pub const ASN1_CLASS_UNIVERSAL: i64 = 0;
```

*Defined at line 51*

---

## ASN1_CLASS_APPLICATION:

```fides
pub const ASN1_CLASS_APPLICATION: i64 = 1;
```

*Defined at line 52*

---

## ASN1_CLASS_CONTEXT:

```fides
pub const ASN1_CLASS_CONTEXT: i64 = 2;
```

*Defined at line 53*

---

## ASN1_CLASS_PRIVATE:

```fides
pub const ASN1_CLASS_PRIVATE: i64 = 3;
```

*Defined at line 54*

---

## Asn1Tlv

The header of one element. `off`..`off + len` is the content; `end` is
the offset after the element (after the 00 00 of an indefinite length).
`ok` is false when the bytes do not hold a well-formed element, `der`
false when the element is valid BER but not DER.

```fides
pub struct Asn1Tlv {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `cls` | `i64` |
| `constructed` | `bool` |
| `tag` | `i64` |
| `start` | `i64` |
| `off` | `i64` |
| `len` | `i64` |
| `end` | `i64` |
| `indefinite` | `bool` |
| `der` | `bool` |

*Defined at line 60*

---

## asn1_read

The element at `at`, which must end by `limit` (BER: any length form).

```fides
pub fn asn1_read(buf: &Vec<i64>, at: i64, limit: i64) -> Asn1Tlv {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `at`: `i64`
- `limit`: `i64`

**Returns:** `Asn1Tlv`

*Defined at line 185*

---

## asn1_read_der

The element at `at` under DER's rules: anything only BER allows makes
the result not ok.

```fides
pub fn asn1_read_der(buf: &Vec<i64>, at: i64, limit: i64) -> Asn1Tlv {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `at`: `i64`
- `limit`: `i64`

**Returns:** `Asn1Tlv`

*Defined at line 191*

---

## asn1_is

Whether t is the universal element with tag number `tag`.

```fides
pub fn asn1_is(t: Asn1Tlv, tag: i64) -> bool {
```

**Parameters:**

- `t`: `Asn1Tlv`
- `tag`: `i64`

**Returns:** `bool`

*Defined at line 200*

---

## asn1_is_ctx

Whether t is the context-specific element [n].

```fides
pub fn asn1_is_ctx(t: Asn1Tlv, n: i64) -> bool {
```

**Parameters:**

- `t`: `Asn1Tlv`
- `n`: `i64`

**Returns:** `bool`

*Defined at line 205*

---

## asn1_children

The elements directly inside a constructed element, in order. A
malformed child ends the list with an element whose ok is false.

```fides
pub fn asn1_children(buf: &Vec<i64>, t: Asn1Tlv) -> Vec<Asn1Tlv> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Vec<Asn1Tlv>`

*Defined at line 211*

---

## asn1_count

The number of children, or -1 when one of them is malformed.

```fides
pub fn asn1_count(buf: &Vec<i64>, t: Asn1Tlv) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `i64`

*Defined at line 230*

---

## asn1_slice

Bytes buf[from..to].

```fides
pub fn asn1_slice(buf: &Vec<i64>, from: i64, to: i64) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 243*

---

## asn1_content

The content bytes of an element.

```fides
pub fn asn1_content(buf: &Vec<i64>, t: Asn1Tlv) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Vec<i64>`

*Defined at line 254*

---

## asn1_raw

The whole encoding of an element, header included.

```fides
pub fn asn1_raw(buf: &Vec<i64>, t: Asn1Tlv) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Vec<i64>`

*Defined at line 259*

---

## asn1_get_int

An INTEGER (or ENUMERATED) that fits in i64, two's complement.

```fides
pub fn asn1_get_int(buf: &Vec<i64>, t: Asn1Tlv) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `i64`

*Defined at line 266*

---

## asn1_int_minimal

Whether an INTEGER's content is minimal (DER: no redundant leading
00 or ff byte) and not empty.

```fides
pub fn asn1_int_minimal(buf: &Vec<i64>, t: Asn1Tlv) -> bool {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `bool`

*Defined at line 284*

---

## asn1_int_negative

Whether an INTEGER is negative.

```fides
pub fn asn1_int_negative(buf: &Vec<i64>, t: Asn1Tlv) -> bool {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `bool`

*Defined at line 303*

---

## asn1_get_uint_bytes

The magnitude bytes of a non-negative INTEGER, big-endian, without
leading zero bytes (at least one byte): RSA moduli, ECDSA r and s.

```fides
pub fn asn1_get_uint_bytes(buf: &Vec<i64>, t: Asn1Tlv) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Vec<i64>`

*Defined at line 309*

---

## asn1_get_bool

A BOOLEAN: any non-zero byte is true (DER requires 0xff).

```fides
pub fn asn1_get_bool(buf: &Vec<i64>, t: Asn1Tlv) -> bool {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `bool`

*Defined at line 318*

---

## asn1_get_oid

An OBJECT IDENTIFIER in dotted form ("1.2.840.113549.1.1.11"); "" when
malformed.

```fides
pub fn asn1_get_oid(buf: &Vec<i64>, t: Asn1Tlv) -> str {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `str`

*Defined at line 324*

---

## asn1_bits_unused

The number of unused bits in the last byte of a BIT STRING.

```fides
pub fn asn1_bits_unused(buf: &Vec<i64>, t: Asn1Tlv) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `i64`

*Defined at line 368*

---

## asn1_get_bits

The bytes of a BIT STRING (after the unused-bits count byte).

```fides
pub fn asn1_get_bits(buf: &Vec<i64>, t: Asn1Tlv) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Vec<i64>`

*Defined at line 376*

---

## asn1_get_str

A string type's bytes as a str (UTF8String, PrintableString,
IA5String, T61String: bytes as they are; BMPString: UCS-2 to UTF-8).

```fides
pub fn asn1_get_str(buf: &Vec<i64>, t: Asn1Tlv) -> str {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `str`

*Defined at line 385*

---

## asn1_days_from_civil

Days since 1970-01-01 of a civil date (H. Hinnant's algorithm).

```fides
pub fn asn1_days_from_civil(y_in: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y_in`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 417*

---

## asn1_unix_time

Unix seconds of a UTC date and time.

```fides
pub fn asn1_unix_time(y: i64, mo: i64, d: i64, h: i64, mi: i64, s: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `mo`: `i64`
- `d`: `i64`
- `h`: `i64`
- `mi`: `i64`
- `s`: `i64`

**Returns:** `i64`

*Defined at line 437*

---

## asn1_civil_from_days

The civil date of a day count: year * 10000 + month * 100 + day.

```fides
pub fn asn1_civil_from_days(z_in: i64) -> i64 {
```

**Parameters:**

- `z_in`: `i64`

**Returns:** `i64`

*Defined at line 442*

---

## asn1_get_time

A UTCTime (YYMMDDHHMM[SS]Z, years 50..99 are 19xx) or GeneralizedTime
(YYYYMMDDHHMMSS[.fff]Z) as Unix seconds. Offsets other than Z and
local times are refused (None), as RFC 5280 requires Z.

```fides
pub fn asn1_get_time(buf: &Vec<i64>, t: Asn1Tlv) -> Option<i64> {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Option<i64>`

*Defined at line 481*

---

## asn1_cat

a followed by b.

```fides
pub fn asn1_cat(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 542*

---

## asn1_enc_len

A DER length: short form below 128, else 0x80 + byte count, big-endian.

```fides
pub fn asn1_enc_len(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 558*

---

## asn1_enc_tag

An element with class, constructed flag and any tag number.

```fides
pub fn asn1_enc_tag(cls: i64, constructed: bool, tag: i64, content: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cls`: `i64`
- `constructed`: `bool`
- `tag`: `i64`
- `content`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 580*

---

## asn1_enc_tlv

A universal element: tag number below 31, constructed for SEQUENCE/SET.

```fides
pub fn asn1_enc_tlv(tag: i64, content: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `tag`: `i64`
- `content`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 621*

---

## asn1_enc_int

INTEGER of an i64, minimal two's complement.

```fides
pub fn asn1_enc_int(x: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 626*

---

## asn1_enc_uint

INTEGER of a non-negative big-endian magnitude (leading zeros dropped,
a 00 added when the top bit is set).

```fides
pub fn asn1_enc_uint(mag: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `mag`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 648*

---

## asn1_enc_bool

BOOLEAN (DER: true is 0xff).

```fides
pub fn asn1_enc_bool(b: bool) -> Vec<i64> {
```

**Parameters:**

- `b`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 669*

---

## asn1_enc_null

NULL.

```fides
pub fn asn1_enc_null() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 680*

---

## asn1_oid_content

The content bytes of a dotted OID; empty when the text is not an OID.

```fides
pub fn asn1_oid_content(oid: str) -> Vec<i64> {
```

**Parameters:**

- `oid`: `str`

**Returns:** `Vec<i64>`

*Defined at line 705*

---

## asn1_enc_oid

OBJECT IDENTIFIER from dotted text.

```fides
pub fn asn1_enc_oid(oid: str) -> Vec<i64> {
```

**Parameters:**

- `oid`: `str`

**Returns:** `Vec<i64>`

*Defined at line 745*

---

## asn1_enc_octets

OCTET STRING.

```fides
pub fn asn1_enc_octets(b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 750*

---

## asn1_enc_bits

BIT STRING of bytes whose last `unused` bits (0..7) are padding; DER
requires those bits to be zero, so they are cleared.

```fides
pub fn asn1_enc_bits(b: Vec<i64>, unused: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `Vec<i64>`
- `unused`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 756*

---

## asn1_enc_utf8

UTF8String.

```fides
pub fn asn1_enc_utf8(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 772*

---

## asn1_is_printable

Whether every byte is in PrintableString's alphabet (X.680 §41.4).

```fides
pub fn asn1_is_printable(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 777*

---

## asn1_enc_printable

PrintableString (the caller checks the alphabet with asn1_is_printable).

```fides
pub fn asn1_enc_printable(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 794*

---

## asn1_enc_ia5

IA5String (ASCII: e-mail addresses, DNS names).

```fides
pub fn asn1_enc_ia5(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 799*

---

## asn1_enc_utctime

UTCTime YYMMDDHHMMSSZ.

```fides
pub fn asn1_enc_utctime(unix: i64) -> Vec<i64> {
```

**Parameters:**

- `unix`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 844*

---

## asn1_enc_gentime

GeneralizedTime YYYYMMDDHHMMSSZ.

```fides
pub fn asn1_enc_gentime(unix: i64) -> Vec<i64> {
```

**Parameters:**

- `unix`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 849*

---

## asn1_enc_time

RFC 5280's Time: UTCTime through 2049, GeneralizedTime from 2050.

```fides
pub fn asn1_enc_time(unix: i64) -> Vec<i64> {
```

**Parameters:**

- `unix`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 854*

---

## asn1_enc_seq

SEQUENCE around already-encoded elements.

```fides
pub fn asn1_enc_seq(content: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `content`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 863*

---

## asn1_enc_set

SET around already-encoded elements (kept in the given order).

```fides
pub fn asn1_enc_set(content: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `content`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 868*

---

## asn1_bytes_cmp

Compare encodings as DER's SET OF does: byte by byte, the shorter one
padded with zeros at its end. Negative, 0 or positive.

```fides
pub fn asn1_bytes_cmp(a: &Vec<i64>, b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 874*

---

## asn1_enc_set_of

SET OF: the encoded elements sorted into DER order.

```fides
pub fn asn1_enc_set_of(elems: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `elems`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 898*

---

## asn1_enc_explicit

[n] EXPLICIT: a constructed context element around an encoded element.

```fides
pub fn asn1_enc_explicit(n: i64, inner: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`
- `inner`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 936*

---

## asn1_enc_implicit

[n] IMPLICIT: the content of an element under a context tag.

```fides
pub fn asn1_enc_implicit(n: i64, constructed: bool, content: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`
- `constructed`: `bool`
- `content`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 941*

---

## asn1_path

Follow a path of child indexes from the element at `at`: path [1, 0]
is the first child of the second child. Not ok when a step is missing.

```fides
pub fn asn1_path(buf: &Vec<i64>, at: i64, path: Vec<i64>) -> Asn1Tlv {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `at`: `i64`
- `path`: `Vec<i64>`

**Returns:** `Asn1Tlv`

*Defined at line 949*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
