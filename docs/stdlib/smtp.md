[All modules](index.md)

# smtp -- Fides Documentation

## Contents

- [SmtpReply](#SmtpReply) (struct)
- [smtp_parse_reply](#smtp_parse_reply) (function)
- [smtp_reply_text](#smtp_reply_text) (function)
- [SmtpCaps](#SmtpCaps) (struct)
- [smtp_caps_none](#smtp_caps_none) (function)
- [smtp_parse_caps](#smtp_parse_caps) (function)
- [smtp_caps_has_auth](#smtp_caps_has_auth) (function)
- [smtp_addr_spec](#smtp_addr_spec) (function)
- [smtp_auth_plain](#smtp_auth_plain) (function)
- [smtp_dot_stuff](#smtp_dot_stuff) (function)
- [SmtpTransport](#SmtpTransport) (struct)
- [smtp_tcp_transport](#smtp_tcp_transport) (function)
- [SmtpConfig](#SmtpConfig) (struct)
- [smtp_config](#smtp_config) (function)
- [SmtpResult](#SmtpResult) (struct)
- [smtp_send](#smtp_send) (function)
- [MailMessage](#MailMessage) (struct)
- [mail_new](#mail_new) (function)
- [mail_add_to](#mail_add_to) (function)
- [mail_add_cc](#mail_add_cc) (function)
- [mail_add_bcc](#mail_add_bcc) (function)
- [mail_set_text](#mail_set_text) (function)
- [mail_set_html](#mail_set_html) (function)
- [mail_attach](#mail_attach) (function)
- [mail_add_header](#mail_add_header) (function)
- [mail_envelope_from](#mail_envelope_from) (function)
- [mail_recipients](#mail_recipients) (function)
- [mail_encode_word](#mail_encode_word) (function)
- [mail_quoted_printable](#mail_quoted_printable) (function)
- [mail_base64_lines](#mail_base64_lines) (function)
- [mail_date](#mail_date) (function)
- [mail_build](#mail_build) (function)

## SmtpReply

A reply: code (0 while incomplete), its lines without the code, and the
enhanced status code of the first line ("" when none).

```fides
pub struct SmtpReply {
```

**Fields:**

| Name | Type |
|------|------|
| `complete` | `bool` |
| `ok` | `bool` |
| `code` | `i64` |
| `lines` | `Vec<str>` |
| `enhanced` | `str` |
| `used` | `i64` |
| `error` | `str` |

*Defined at line 50*

---

## smtp_parse_reply

Parse one reply at the start of `text` (lines end in CRLF; a bare LF
is taken too). complete false: more bytes are needed.

```fides
pub fn smtp_parse_reply(text: str) -> SmtpReply {
```

**Parameters:**

- `text`: `str`

**Returns:** `SmtpReply`

*Defined at line 103*

---

## smtp_reply_text

The reply text, lines joined by a space.

```fides
pub fn smtp_reply_text(r: &SmtpReply) -> str {
```

**Parameters:**

- `r`: `&SmtpReply`

**Returns:** `str`

*Defined at line 161*

---

## SmtpCaps

size: -1 without the SIZE extension, 0 with it and no limit.

```fides
pub struct SmtpCaps {
```

**Fields:**

| Name | Type |
|------|------|
| `esmtp` | `bool` |
| `starttls` | `bool` |
| `auth` | `Vec<str>` |
| `size` | `i64` |
| `eightbit` | `bool` |
| `pipelining` | `bool` |
| `smtputf8` | `bool` |
| `enhanced` | `bool` |

*Defined at line 168*

---

## smtp_caps_none

```fides
pub fn smtp_caps_none() -> SmtpCaps {
```

**Returns:** `SmtpCaps`

*Defined at line 179*

---

## smtp_parse_caps

The capabilities of a 250 reply to EHLO (its first line is the greeting).

```fides
pub fn smtp_parse_caps(r: &SmtpReply) -> SmtpCaps {
```

**Parameters:**

- `r`: `&SmtpReply`

**Returns:** `SmtpCaps`

*Defined at line 186*

---

## smtp_caps_has_auth

```fides
pub fn smtp_caps_has_auth(c: &SmtpCaps, mech: str) -> bool {
```

**Parameters:**

- `c`: `&SmtpCaps`
- `mech`: `str`

**Returns:** `bool`

*Defined at line 237*

---

## smtp_addr_spec

"<a@b>" from "Name <a@b>", "<a@b>" or "a@b".

```fides
pub fn smtp_addr_spec(addr: str) -> str {
```

**Parameters:**

- `addr`: `str`

**Returns:** `str`

*Defined at line 251*

---

## smtp_auth_plain

The AUTH PLAIN initial response: base64(authzid NUL user NUL pass).

```fides
pub fn smtp_auth_plain(user: str, pass: str) -> str {
```

**Parameters:**

- `user`: `str`
- `pass`: `str`

**Returns:** `str`

*Defined at line 272*

---

## smtp_dot_stuff

The DATA payload: lines starting with '.' get another '.', bare LF
become CRLF, and the terminating CRLF.CRLF is added.

```fides
pub fn smtp_dot_stuff(msg: str) -> str {
```

**Parameters:**

- `msg`: `str`

**Returns:** `str`

*Defined at line 293*

---

## SmtpTransport

```fides
pub struct SmtpTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `conn` | `i64` |
| `send` | `fn(i64, Vec<i64>) -> i64` |
| `recv` | `fn(i64, i64) -> Vec<i64>` |
| `starttls` | `fn(i64) -> bool` |

*Defined at line 327*

---

## smtp_tcp_transport

Plain TCP over a connected socket (stdlib net); STARTTLS is refused.

```fides
pub fn smtp_tcp_transport(sock: i64) -> SmtpTransport {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `SmtpTransport`

*Defined at line 339*

---

## SmtpConfig

```fides
pub struct SmtpConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `helo` | `str` |
| `user` | `str` |
| `pass` | `str` |
| `use_starttls` | `bool` |
| `require_tls` | `bool` |
| `prefer_login` | `bool` |

*Defined at line 345*

---

## smtp_config

EHLO name and credentials (user "" = no AUTH); STARTTLS when offered.

```fides
pub fn smtp_config(helo: str, user: str, pass: str) -> SmtpConfig {
```

**Parameters:**

- `helo`: `str`
- `user`: `str`
- `pass`: `str`

**Returns:** `SmtpConfig`

*Defined at line 355*

---

## SmtpResult

What happened. ok: the server took the message (code 250 after DATA).
step names the command that failed. accepted / rejected: recipients.

```fides
pub struct SmtpResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `step` | `str` |
| `code` | `i64` |
| `enhanced` | `str` |
| `message` | `str` |
| `accepted` | `Vec<str>` |
| `rejected` | `Vec<str>` |
| `tls` | `bool` |
| `auth` | `str` |
| `caps` | `SmtpCaps` |
| `transcript` | `Vec<str>` |

*Defined at line 363*

---

## smtp_send

Run a whole SMTP transaction: deliver `message` (RFC 5322 text, as
mail_build makes it) from `from` to each of `rcpts`.

```fides
pub fn smtp_send(tr: SmtpTransport, cfg: SmtpConfig, from: str, rcpts: Vec<str>, message: str) -> SmtpResult {
```

**Parameters:**

- `tr`: `SmtpTransport`
- `cfg`: `SmtpConfig`
- `from`: `str`
- `rcpts`: `Vec<str>`
- `message`: `str`

**Returns:** `SmtpResult`

*Defined at line 467*

---

## MailMessage

```fides
pub struct MailMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `from` | `str` |
| `to` | `Vec<str>` |
| `cc` | `Vec<str>` |
| `bcc` | `Vec<str>` |
| `reply_to` | `str` |
| `subject` | `str` |
| `text` | `str` |
| `html` | `str` |
| `att_names` | `Vec<str>` |
| `att_types` | `Vec<str>` |
| `att_data` | `Vec<Vec<i64>>` |
| `header_names` | `Vec<str>` |
| `header_vals` | `Vec<str>` |
| `message_id` | `str` |

*Defined at line 630*

---

## mail_new

```fides
pub fn mail_new(from: str, subject: str) -> MailMessage {
```

**Parameters:**

- `from`: `str`
- `subject`: `str`

**Returns:** `MailMessage`

*Defined at line 647*

---

## mail_add_to

```fides
pub fn mail_add_to(m: &mut MailMessage, addr: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `addr`: `str`

*Defined at line 653*

---

## mail_add_cc

```fides
pub fn mail_add_cc(m: &mut MailMessage, addr: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `addr`: `str`

*Defined at line 657*

---

## mail_add_bcc

A recipient who gets the mail but appears in no header.

```fides
pub fn mail_add_bcc(m: &mut MailMessage, addr: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `addr`: `str`

*Defined at line 662*

---

## mail_set_text

```fides
pub fn mail_set_text(m: &mut MailMessage, text: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `text`: `str`

*Defined at line 666*

---

## mail_set_html

```fides
pub fn mail_set_html(m: &mut MailMessage, html: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `html`: `str`

*Defined at line 670*

---

## mail_attach

```fides
pub fn mail_attach(m: &mut MailMessage, name: str, content_type: str, data: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `name`: `str`
- `content_type`: `str`
- `data`: `Vec<i64>`

*Defined at line 674*

---

## mail_add_header

```fides
pub fn mail_add_header(m: &mut MailMessage, name: str, value: str) {
```

**Parameters:**

- `m`: `&mut MailMessage`
- `name`: `str`
- `value`: `str`

*Defined at line 680*

---

## mail_envelope_from

The envelope sender (the From address).

```fides
pub fn mail_envelope_from(m: &MailMessage) -> str {
```

**Parameters:**

- `m`: `&MailMessage`

**Returns:** `str`

*Defined at line 686*

---

## mail_recipients

Every envelope recipient: To, Cc and Bcc.

```fides
pub fn mail_recipients(m: &MailMessage) -> Vec<str> {
```

**Parameters:**

- `m`: `&MailMessage`

**Returns:** `Vec<str>`

*Defined at line 691*

---

## mail_encode_word

```fides
pub fn mail_encode_word(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 747*

---

## mail_quoted_printable

Quoted-printable (RFC 2045 6.7) with CRLF line breaks and soft breaks
keeping lines at most 76 characters.

```fides
pub fn mail_quoted_printable(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 798*

---

## mail_base64_lines

Base64 in lines of 76 characters, CRLF after each.

```fides
pub fn mail_base64_lines(data: Vec<i64>) -> str {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 877*

---

## mail_date

An RFC 5322 date for Unix time `t` at a UTC offset in minutes:
"Tue, 17 Nov 2026 10:07:00 +0000".

```fides
pub fn mail_date(t: i64, offset_min: i64) -> str {
```

**Parameters:**

- `t`: `i64`
- `offset_min`: `i64`

**Returns:** `str`

*Defined at line 901*

---

## mail_build

The whole message (headers, CRLF line ends) for Unix time `t` at
`offset_min`; `seed` makes the boundaries and the Message-ID when none
is set.

```fides
pub fn mail_build(m: &MailMessage, t: i64, offset_min: i64, seed: i64) -> str {
```

**Parameters:**

- `m`: `&MailMessage`
- `t`: `i64`
- `offset_min`: `i64`
- `seed`: `i64`

**Returns:** `str`

*Defined at line 968*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
