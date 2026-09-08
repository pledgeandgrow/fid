# swift — Fides Documentation

## Contents

- [SWIFT_MT103:](#swift_mt103:) (const)
- [SWIFT_MT202:](#swift_mt202:) (const)
- [SWIFT_MT900:](#swift_mt900:) (const)
- [SWIFT_MT910:](#swift_mt910:) (const)
- [SWIFT_MT940:](#swift_mt940:) (const)
- [SWIFT_MT942:](#swift_mt942:) (const)
- [SWIFT_MT199:](#swift_mt199:) (const)
- [SWIFT_MT299:](#swift_mt299:) (const)
- [SWIFT_MT300:](#swift_mt300:) (const)
- [SWIFT_MT320:](#swift_mt320:) (const)
- [SWIFT_MT700:](#swift_mt700:) (const)
- [SWIFT_MT760:](#swift_mt760:) (const)
- [swift_mt_name](#swift_mt_name) (function)
- [swift_bic_validate](#swift_bic_validate) (function)
- [swift_bic_bank_code](#swift_bic_bank_code) (function)
- [swift_bic_country](#swift_bic_country) (function)
- [swift_bic_location](#swift_bic_location) (function)
- [swift_bic_branch](#swift_bic_branch) (function)
- [swift_iban_validate_length](#swift_iban_validate_length) (function)
- [swift_iban_country](#swift_iban_country) (function)
- [swift_iban_check_digits](#swift_iban_check_digits) (function)
- [swift_iban_bban](#swift_iban_bban) (function)
- [MT103](#mt103) (struct)
- [swift_mt103_new](#swift_mt103_new) (function)
- [swift_mt103_set_ordering](#swift_mt103_set_ordering) (function)
- [swift_mt103_set_beneficiary](#swift_mt103_set_beneficiary) (function)
- [swift_mt103_set_remittance](#swift_mt103_set_remittance) (function)
- [swift_mt103_set_value_date](#swift_mt103_set_value_date) (function)
- [swift_mt103_serialize](#swift_mt103_serialize) (function)
- [swift_mt103_amount_major](#swift_mt103_amount_major) (function)
- [swift_mt103_amount_formatted](#swift_mt103_amount_formatted) (function)
- [swift_parse_field](#swift_parse_field) (function)
- [swift_extract_field](#swift_extract_field) (function)

## SWIFT_MT103:

```fides
pub const SWIFT_MT103: i64  = 103;  // Customer Credit Transfer
```

*Defined at line 27*

---

## SWIFT_MT202:

```fides
pub const SWIFT_MT202: i64  = 202;  // General Financial Institution Transfer
```

*Defined at line 28*

---

## SWIFT_MT900:

```fides
pub const SWIFT_MT900: i64  = 900;  // Confirmation of Debit
```

*Defined at line 29*

---

## SWIFT_MT910:

```fides
pub const SWIFT_MT910: i64  = 910;  // Confirmation of Credit
```

*Defined at line 30*

---

## SWIFT_MT940:

```fides
pub const SWIFT_MT940: i64  = 940;  // Statement Message
```

*Defined at line 31*

---

## SWIFT_MT942:

```fides
pub const SWIFT_MT942: i64  = 942;  // Interim Transaction Report
```

*Defined at line 32*

---

## SWIFT_MT199:

```fides
pub const SWIFT_MT199: i64  = 199;  // Free Format Message
```

*Defined at line 33*

---

## SWIFT_MT299:

```fides
pub const SWIFT_MT299: i64  = 299;  // Free Format Message (FIN)
```

*Defined at line 34*

---

## SWIFT_MT300:

```fides
pub const SWIFT_MT300: i64  = 300;  // Foreign Exchange Confirmation
```

*Defined at line 35*

---

## SWIFT_MT320:

```fides
pub const SWIFT_MT320: i64  = 320;  // Loan/Deposit Confirmation
```

*Defined at line 36*

---

## SWIFT_MT700:

```fides
pub const SWIFT_MT700: i64  = 700;  // Issue of a Documentary Credit
```

*Defined at line 37*

---

## SWIFT_MT760:

```fides
pub const SWIFT_MT760: i64  = 760;  // Guarantee
```

*Defined at line 38*

---

## swift_mt_name

Get SWIFT message type name.

```fides
pub fn swift_mt_name(mt: i64) -> str {
```

**Parameters:**

- `mt`: `i64`

**Returns:** `str`

*Defined at line 41*

---

## swift_bic_validate

Validate a BIC code (8 or 11 characters).

```fides
pub fn swift_bic_validate(bic: str) -> bool {
```

**Parameters:**

- `bic`: `str`

**Returns:** `bool`

*Defined at line 67*

---

## swift_bic_bank_code

Get the bank code (first 4 characters) from a BIC.

```fides
pub fn swift_bic_bank_code(bic: str) -> str {
```

**Parameters:**

- `bic`: `str`

**Returns:** `str`

*Defined at line 83*

---

## swift_bic_country

Get the country code (characters 5-6) from a BIC.

```fides
pub fn swift_bic_country(bic: str) -> str {
```

**Parameters:**

- `bic`: `str`

**Returns:** `str`

*Defined at line 88*

---

## swift_bic_location

Get the location code (characters 7-8) from a BIC.

```fides
pub fn swift_bic_location(bic: str) -> str {
```

**Parameters:**

- `bic`: `str`

**Returns:** `str`

*Defined at line 93*

---

## swift_bic_branch

Get the branch code (characters 9-11) from a BIC.
Returns "XXX" if not present (primary office).

```fides
pub fn swift_bic_branch(bic: str) -> str {
```

**Parameters:**

- `bic`: `str`

**Returns:** `str`

*Defined at line 99*

---

## swift_iban_validate_length

Validate IBAN length by country code.

```fides
pub fn swift_iban_validate_length(iban: str) -> bool {
```

**Parameters:**

- `iban`: `str`

**Returns:** `bool`

*Defined at line 113*

---

## swift_iban_country

Get country code from IBAN.

```fides
pub fn swift_iban_country(iban: str) -> str {
```

**Parameters:**

- `iban`: `str`

**Returns:** `str`

*Defined at line 140*

---

## swift_iban_check_digits

Get check digits from IBAN (characters 3-4).

```fides
pub fn swift_iban_check_digits(iban: str) -> str {
```

**Parameters:**

- `iban`: `str`

**Returns:** `str`

*Defined at line 145*

---

## swift_iban_bban

Get BBAN (Basic Bank Account Number) from IBAN (characters 5+).

```fides
pub fn swift_iban_bban(iban: str) -> str {
```

**Parameters:**

- `iban`: `str`

**Returns:** `str`

*Defined at line 150*

---

## MT103

An MT103 message structure.

```fides
pub struct MT103 {
```

**Fields:**

| Name | Type |
|------|------|
| `sender_bic` | `str` |
| `receiver_bic` | `str` |
| `message_id` | `str` |
| `amount` | `i64,        // in cents` |
| `currency` | `str,      // 3-letter ISO 4217` |
| `ordering_customer` | `str` |
| `beneficiary_customer` | `str` |
| `ordering_institution` | `str` |
| `beneficiary_institution` | `str` |
| `remittance_info` | `str` |
| `value_date` | `str,    // YYMMDD` |

*Defined at line 160*

---

## swift_mt103_new

Create a new MT103 message.

```fides
pub fn swift_mt103_new(sender_bic: str, receiver_bic: str, amount_cents: i64,
```

**Parameters:**

- `sender_bic`: `str`
- `receiver_bic`: `str`
- `amount_cents`: `i64`

*Defined at line 175*

---

## swift_mt103_set_ordering

Set ordering customer (party initiating the payment).

```fides
pub fn swift_mt103_set_ordering(msg: MT103, customer: str, institution: str) -> void {
```

**Parameters:**

- `msg`: `MT103`
- `customer`: `str`
- `institution`: `str`

**Returns:** `void`

*Defined at line 193*

---

## swift_mt103_set_beneficiary

Set beneficiary customer (party receiving the payment).

```fides
pub fn swift_mt103_set_beneficiary(msg: MT103, customer: str, institution: str) -> void {
```

**Parameters:**

- `msg`: `MT103`
- `customer`: `str`
- `institution`: `str`

**Returns:** `void`

*Defined at line 199*

---

## swift_mt103_set_remittance

Set remittance information (payment reference).

```fides
pub fn swift_mt103_set_remittance(msg: MT103, info: str) -> void {
```

**Parameters:**

- `msg`: `MT103`
- `info`: `str`

**Returns:** `void`

*Defined at line 205*

---

## swift_mt103_set_value_date

Set value date (YYMMDD format).

```fides
pub fn swift_mt103_set_value_date(msg: MT103, date: str) -> void {
```

**Parameters:**

- `msg`: `MT103`
- `date`: `str`

**Returns:** `void`

*Defined at line 210*

---

## swift_mt103_serialize

Serialize an MT103 message to SWIFT text format.

```fides
pub fn swift_mt103_serialize(msg: MT103) -> str {
```

**Parameters:**

- `msg`: `MT103`

**Returns:** `str`

*Defined at line 215*

---

## swift_mt103_amount_major

Get the amount in major currency units (e.g. dollars, not cents).

```fides
pub fn swift_mt103_amount_major(msg: MT103) -> f64 {
```

**Parameters:**

- `msg`: `MT103`

**Returns:** `f64`

*Defined at line 256*

---

## swift_mt103_amount_formatted

Get the amount as a formatted string (e.g. "1000.00 USD").

```fides
pub fn swift_mt103_amount_formatted(msg: MT103) -> str {
```

**Parameters:**

- `msg`: `MT103`

**Returns:** `str`

*Defined at line 261*

---

## swift_parse_field

Parse a field tag and value from a SWIFT text line.
Format: ":TAG:VALUE" (e.g. ":20:REF12345")
Returns (tag, value) as a Vec [tag_str, value_str].

```fides
pub fn swift_parse_field(line: str) -> Vec {
```

**Parameters:**

- `line`: `str`

**Returns:** `Vec`

*Defined at line 277*

---

## swift_extract_field

Extract a specific field from a SWIFT text block.
Returns the field value, or "" if not found.

```fides
pub fn swift_extract_field(text: str, tag: str) -> str {
```

**Parameters:**

- `text`: `str`
- `tag`: `str`

**Returns:** `str`

*Defined at line 311*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
