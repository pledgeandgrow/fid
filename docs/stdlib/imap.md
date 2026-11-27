[All modules](index.md)

# imap -- Fides Documentation

## Contents

- [IMAP_ATOM:](#IMAP_ATOM:) (const)
- [IMAP_NUM:](#IMAP_NUM:) (const)
- [IMAP_STR:](#IMAP_STR:) (const)
- [IMAP_NIL:](#IMAP_NIL:) (const)
- [IMAP_LIST:](#IMAP_LIST:) (const)
- [ImapDoc](#ImapDoc) (struct)
- [imap_kind](#imap_kind) (function)
- [imap_text](#imap_text) (function)
- [imap_num](#imap_num) (function)
- [imap_count](#imap_count) (function)
- [imap_child](#imap_child) (function)
- [imap_string](#imap_string) (function)
- [imap_to_str](#imap_to_str) (function)
- [ImapResponse](#ImapResponse) (struct)
- [imap_parse_response](#imap_parse_response) (function)
- [imap_quote](#imap_quote) (function)
- [imap_astring](#imap_astring) (function)
- [ImapTransport](#ImapTransport) (struct)
- [imap_tcp_transport](#imap_tcp_transport) (function)
- [ImapClient](#ImapClient) (struct)
- [imap_client_new](#imap_client_new) (function)
- [imap_next_tag](#imap_next_tag) (function)
- [imap_read_response](#imap_read_response) (function)
- [ImapResult](#ImapResult) (struct)
- [imap_command_lit](#imap_command_lit) (function)
- [imap_command](#imap_command) (function)
- [imap_greeting](#imap_greeting) (function)
- [imap_has_cap](#imap_has_cap) (function)
- [imap_capability](#imap_capability) (function)
- [imap_login](#imap_login) (function)
- [ImapMailbox](#ImapMailbox) (struct)
- [imap_mailbox_from](#imap_mailbox_from) (function)
- [imap_select](#imap_select) (function)
- [imap_examine](#imap_examine) (function)
- [imap_search_results](#imap_search_results) (function)
- [imap_search](#imap_search) (function)
- [imap_uid_search](#imap_uid_search) (function)
- [imap_store](#imap_store) (function)
- [imap_logout](#imap_logout) (function)
- [imap_idle_start](#imap_idle_start) (function)
- [imap_idle_next](#imap_idle_next) (function)
- [imap_idle_done](#imap_idle_done) (function)
- [ImapAddress](#ImapAddress) (struct)
- [ImapEnvelope](#ImapEnvelope) (struct)
- [ImapPart](#ImapPart) (struct)
- [ImapFetch](#ImapFetch) (struct)
- [imap_decode_words](#imap_decode_words) (function)
- [imap_envelope](#imap_envelope) (function)
- [imap_body_parts](#imap_body_parts) (function)
- [imap_fetch_items](#imap_fetch_items) (function)
- [imap_fetch](#imap_fetch) (function)
- [imap_uid_fetch](#imap_uid_fetch) (function)
- [imap_fetch_section](#imap_fetch_section) (function)

## IMAP_ATOM:

```fides
pub const IMAP_ATOM: i64 = 1;
```

*Defined at line 47*

---

## IMAP_NUM:

```fides
pub const IMAP_NUM: i64 = 2;
```

*Defined at line 48*

---

## IMAP_STR:

```fides
pub const IMAP_STR: i64 = 3;
```

*Defined at line 49*

---

## IMAP_NIL:

```fides
pub const IMAP_NIL: i64 = 4;
```

*Defined at line 50*

---

## IMAP_LIST:

```fides
pub const IMAP_LIST: i64 = 5;
```

*Defined at line 51*

---

## ImapDoc

Parsed values as a flat tree: node i has kind[i], text[i] (atom or
string bytes), num[i], and for a list its children.

```fides
pub struct ImapDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>` |
| `text` | `Vec<str>` |
| `num` | `Vec<i64>` |
| `first` | `Vec<i64>` |
| `nkids` | `Vec<i64>` |
| `kids` | `Vec<i64>` |

*Defined at line 57*

---

## imap_kind

```fides
pub fn imap_kind(d: &ImapDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 81*

---

## imap_text

```fides
pub fn imap_text(d: &ImapDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 85*

---

## imap_num

```fides
pub fn imap_num(d: &ImapDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 89*

---

## imap_count

```fides
pub fn imap_count(d: &ImapDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## imap_child

```fides
pub fn imap_child(d: &ImapDoc, i: i64, k: i64) -> i64 {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 97*

---

## imap_string

A string value (STR, ATOM or NUM as text), "" for NIL or a list.

```fides
pub fn imap_string(d: &ImapDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 102*

---

## imap_to_str

IMAP syntax of node i (strings quoted), for logs and tests.

```fides
pub fn imap_to_str(d: &ImapDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 111*

---

## ImapResponse

One server response. tag: "*" untagged, "+" continuation, or the
command's tag. For "* 12 FETCH (...)": num 12, word "FETCH". For
status responses (OK, NO, BAD, PREAUTH, BYE): word is it, code the
bracketed resp-text code ("UIDVALIDITY 3857529045"), text the rest.
values: the line's values after the word (a list node for FETCH).

```fides
pub struct ImapResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `complete` | `bool` |
| `ok` | `bool` |
| `error` | `str` |
| `used` | `i64` |
| `tag` | `str` |
| `num` | `i64` |
| `word` | `str` |
| `code` | `str` |
| `text` | `str` |
| `doc` | `ImapDoc` |
| `values` | `Vec<i64>` |

*Defined at line 141*

---

## imap_parse_response

Parse the response starting at byte `at`. complete false (and ok true)
when more bytes are needed.

```fides
pub fn imap_parse_response(bytes: Vec<i64>, at: i64) -> ImapResponse {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `at`: `i64`

**Returns:** `ImapResponse`

*Defined at line 388*

---

## imap_quote

A quoted string ("a \"b\"").

```fides
pub fn imap_quote(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 485*

---

## imap_astring

How to send `s` as an astring: itself when it is a safe atom, quoted
when it can be, else "" (a literal is needed).

```fides
pub fn imap_astring(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 501*

---

## ImapTransport

```fides
pub struct ImapTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `conn` | `i64` |
| `send` | `fn(i64, Vec<i64>) -> i64` |
| `recv` | `fn(i64, i64) -> Vec<i64>` |

*Defined at line 530*

---

## imap_tcp_transport

```fides
pub fn imap_tcp_transport(sock: i64) -> ImapTransport {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `ImapTransport`

*Defined at line 536*

---

## ImapClient

```fides
pub struct ImapClient {
```

**Fields:**

| Name | Type |
|------|------|
| `tr` | `ImapTransport` |
| `inbuf` | `Vec<i64>` |
| `next_tag` | `i64` |
| `prefix` | `str` |
| `ok` | `bool` |
| `error` | `str` |
| `caps` | `Vec<str>` |
| `literal_plus` | `bool` |
| `idle_tag` | `str` |

*Defined at line 542*

---

## imap_client_new

```fides
pub fn imap_client_new(tr: ImapTransport) -> ImapClient {
```

**Parameters:**

- `tr`: `ImapTransport`

**Returns:** `ImapClient`

*Defined at line 554*

---

## imap_next_tag

The next tag: A001, A002, ...

```fides
pub fn imap_next_tag(c: &mut ImapClient) -> str {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `str`

*Defined at line 561*

---

## imap_read_response

Read one whole response from the connection.

```fides
pub fn imap_read_response(c: &mut ImapClient) -> ImapResponse {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `ImapResponse`

*Defined at line 583*

---

## ImapResult

A command's outcome: the tagged status (OK/NO/BAD), its code and text,
and every untagged response that came before it.

```fides
pub struct ImapResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `status` | `str` |
| `code` | `str` |
| `text` | `str` |
| `untagged` | `Vec<ImapResponse>` |

*Defined at line 621*

---

## imap_command_lit

Send a command (without tag) and read up to its tagged completion.
`literals` are sent as {n} literals where the command has "{}" (each
after the server's "+").

```fides
pub fn imap_command_lit(c: &mut ImapClient, cmd: str, literals: Vec<str>) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `cmd`: `str`
- `literals`: `Vec<str>`

**Returns:** `ImapResult`

*Defined at line 632*

---

## imap_command

```fides
pub fn imap_command(c: &mut ImapClient, cmd: str) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `cmd`: `str`

**Returns:** `ImapResult`

*Defined at line 687*

---

## imap_greeting

Read the server greeting (OK or PREAUTH; BYE refuses).

```fides
pub fn imap_greeting(c: &mut ImapClient) -> bool {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `bool`

*Defined at line 692*

---

## imap_has_cap

```fides
pub fn imap_has_cap(c: &ImapClient, cap: str) -> bool {
```

**Parameters:**

- `c`: `&ImapClient`
- `cap`: `str`

**Returns:** `bool`

*Defined at line 722*

---

## imap_capability

```fides
pub fn imap_capability(c: &mut ImapClient) -> Vec<str> {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `Vec<str>`

*Defined at line 733*

---

## imap_login

```fides
pub fn imap_login(c: &mut ImapClient, user: str, pass: str) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `user`: `str`
- `pass`: `str`

**Returns:** `ImapResult`

*Defined at line 762*

---

## ImapMailbox

What SELECT / EXAMINE report.

```fides
pub struct ImapMailbox {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `exists` | `i64` |
| `recent` | `i64` |
| `unseen` | `i64` |
| `uidvalidity` | `i64` |
| `uidnext` | `i64` |
| `flags` | `Vec<str>` |
| `permanent_flags` | `Vec<str>` |
| `read_write` | `bool` |
| `text` | `str` |

*Defined at line 772*

---

## imap_mailbox_from

The mailbox state from a SELECT/EXAMINE result.

```fides
pub fn imap_mailbox_from(r: &ImapResult) -> ImapMailbox {
```

**Parameters:**

- `r`: `&ImapResult`

**Returns:** `ImapMailbox`

*Defined at line 814*

---

## imap_select

```fides
pub fn imap_select(c: &mut ImapClient, mailbox: str) -> ImapMailbox {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `mailbox`: `str`

**Returns:** `ImapMailbox`

*Defined at line 847*

---

## imap_examine

```fides
pub fn imap_examine(c: &mut ImapClient, mailbox: str) -> ImapMailbox {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `mailbox`: `str`

**Returns:** `ImapMailbox`

*Defined at line 853*

---

## imap_search_results

The numbers of "* SEARCH ..." responses in a result.

```fides
pub fn imap_search_results(r: &ImapResult) -> Vec<i64> {
```

**Parameters:**

- `r`: `&ImapResult`

**Returns:** `Vec<i64>`

*Defined at line 860*

---

## imap_search

SEARCH with criteria as IMAP text ("UNSEEN FROM \"ada\" SINCE 1-Feb-2026").

```fides
pub fn imap_search(c: &mut ImapClient, criteria: str) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `criteria`: `str`

**Returns:** `Vec<i64>`

*Defined at line 880*

---

## imap_uid_search

```fides
pub fn imap_uid_search(c: &mut ImapClient, criteria: str) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `criteria`: `str`

**Returns:** `Vec<i64>`

*Defined at line 885*

---

## imap_store

STORE: e.g. imap_store(c, "2:4", "+FLAGS.SILENT", "(\\Deleted)").

```fides
pub fn imap_store(c: &mut ImapClient, set: str, op: str, flags: str) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `set`: `str`
- `op`: `str`
- `flags`: `str`

**Returns:** `ImapResult`

*Defined at line 891*

---

## imap_logout

```fides
pub fn imap_logout(c: &mut ImapClient) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `ImapResult`

*Defined at line 895*

---

## imap_idle_start

Start IDLE: true once the server's continuation came.

```fides
pub fn imap_idle_start(c: &mut ImapClient) -> bool {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `bool`

*Defined at line 900*

---

## imap_idle_next

The next untagged response during IDLE ("* 23 EXISTS", "* 4 EXPUNGE").

```fides
pub fn imap_idle_next(c: &mut ImapClient) -> ImapResponse {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `ImapResponse`

*Defined at line 917*

---

## imap_idle_done

End IDLE (DONE) and read the tagged completion.

```fides
pub fn imap_idle_done(c: &mut ImapClient) -> ImapResult {
```

**Parameters:**

- `c`: `&mut ImapClient`

**Returns:** `ImapResult`

*Defined at line 922*

---

## ImapAddress

An address of an envelope: "Name <mailbox@host>" in `full`.

```fides
pub struct ImapAddress {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `mailbox` | `str` |
| `host` | `str` |
| `full` | `str` |

*Defined at line 946*

---

## ImapEnvelope

```fides
pub struct ImapEnvelope {
```

**Fields:**

| Name | Type |
|------|------|
| `date` | `str` |
| `subject` | `str` |
| `from` | `Vec<ImapAddress>` |
| `sender` | `Vec<ImapAddress>` |
| `reply_to` | `Vec<ImapAddress>` |
| `to` | `Vec<ImapAddress>` |
| `cc` | `Vec<ImapAddress>` |
| `bcc` | `Vec<ImapAddress>` |
| `in_reply_to` | `str` |
| `message_id` | `str` |

*Defined at line 953*

---

## ImapPart

One part of a body structure: section "1", "2.1" (or "TEXT" for a
single-part message: section "1"), type/subtype lowercase, parameters
"name=value;..." and the transfer encoding, size in bytes, lines for text.

```fides
pub struct ImapPart {
```

**Fields:**

| Name | Type |
|------|------|
| `section` | `str` |
| `mime` | `str` |
| `params` | `str` |
| `id` | `str` |
| `description` | `str` |
| `encoding` | `str` |
| `size` | `i64` |
| `lines` | `i64` |
| `multipart` | `bool` |

*Defined at line 969*

---

## ImapFetch

The items of one FETCH response (seq: its message number).

```fides
pub struct ImapFetch {
```

**Fields:**

| Name | Type |
|------|------|
| `seq` | `i64` |
| `uid` | `i64` |
| `flags` | `Vec<str>` |
| `size` | `i64` |
| `internal_date` | `str` |
| `has_envelope` | `bool` |
| `envelope` | `ImapEnvelope` |
| `parts` | `Vec<ImapPart>` |
| `section_names` | `Vec<str>` |
| `section_data` | `Vec<str>` |

*Defined at line 982*

---

## imap_decode_words

Decode RFC 2047 encoded-words (=?charset?B|Q?text?=) in a header value;
UTF-8, US-ASCII and ISO-8859-1 charsets; whitespace between two
encoded-words is dropped. Unknown charsets are left as they are.

```fides
pub fn imap_decode_words(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 1028*

---

## imap_envelope

An ENVELOPE value (a list of 10 fields).

```fides
pub fn imap_envelope(d: &ImapDoc, i: i64) -> ImapEnvelope {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `ImapEnvelope`

*Defined at line 1133*

---

## imap_body_parts

The parts of a BODY / BODYSTRUCTURE value, depth first.

```fides
pub fn imap_body_parts(d: &ImapDoc, i: i64) -> Vec<ImapPart> {
```

**Parameters:**

- `d`: `&ImapDoc`
- `i`: `i64`

**Returns:** `Vec<ImapPart>`

*Defined at line 1225*

---

## imap_fetch_items

The items of a "* n FETCH (...)" response.

```fides
pub fn imap_fetch_items(r: &ImapResponse) -> ImapFetch {
```

**Parameters:**

- `r`: `&ImapResponse`

**Returns:** `ImapFetch`

*Defined at line 1232*

---

## imap_fetch

FETCH set items: imap_fetch(c, "1:3", "(UID FLAGS ENVELOPE)").

```fides
pub fn imap_fetch(c: &mut ImapClient, set: str, items: str) -> Vec<ImapFetch> {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `set`: `str`
- `items`: `str`

**Returns:** `Vec<ImapFetch>`

*Defined at line 1285*

---

## imap_uid_fetch

```fides
pub fn imap_uid_fetch(c: &mut ImapClient, set: str, items: str) -> Vec<ImapFetch> {
```

**Parameters:**

- `c`: `&mut ImapClient`
- `set`: `str`
- `items`: `str`

**Returns:** `Vec<ImapFetch>`

*Defined at line 1290*

---

## imap_fetch_section

The section data of a fetch by its name ("BODY[TEXT]"), or "".

```fides
pub fn imap_fetch_section(f: &ImapFetch, name: str) -> str {
```

**Parameters:**

- `f`: `&ImapFetch`
- `name`: `str`

**Returns:** `str`

*Defined at line 1296*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
