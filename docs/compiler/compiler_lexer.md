[All modules](index.md)

# compiler::lexer -- Fides Documentation

## Contents

- [Lexer](#Lexer) (struct)
- [Lexer](#Lexer) (impl)
- [str_filter_underscores](#str_filter_underscores) (function)
- [keyword_or_ident](#keyword_or_ident) (function)
- [keyword_or_ident_own](#keyword_or_ident_own) (function)
- [lex_is_whitespace](#lex_is_whitespace) (function)
- [lex_is_alpha](#lex_is_alpha) (function)
- [lex_is_digit](#lex_is_digit) (function)
- [lex_is_alphanumeric](#lex_is_alphanumeric) (function)
- [lex_fstr_expr_end](#lex_fstr_expr_end) (function)
- [lex_fstr_lit_end](#lex_fstr_lit_end) (function)
- [lex_utf8_encode](#lex_utf8_encode) (function)
- [lex_is_hexdigit](#lex_is_hexdigit) (function)
- [parse_radix](#parse_radix) (function)
- [repeat_char](#repeat_char) (function)
- [lex_error](#lex_error) (function)
- [LexedText](#LexedText) (struct)
- [lex_text_alone](#lex_text_alone) (function)
- [lex_par_worker](#lex_par_worker) (function)
- [lex_par_plan](#lex_par_plan) (function)
- [lex_texts_par](#lex_texts_par) (function)
- [token_use_paths](#token_use_paths) (function)
- [ModScan](#ModScan) (struct)
- [token_module_scan](#token_module_scan) (function)
- [ModGraph](#ModGraph) (struct)
- [ModGraph_new](#ModGraph_new) (function)
- [ModScan_empty](#ModScan_empty) (function)
- [modgraph_levels](#modgraph_levels) (function)
- [modgraph_summary](#modgraph_summary) (function)

## Lexer

```fides
pub struct Lexer {
```

**Fields:**

| Name | Type |
|------|------|
| `source` | `str` |
| `src_len` | `i64` |
| `pos` | `i64` |
| `line` | `i64` |
| `col` | `i64` |
| `errors` | `Vec<str>` |
| `own_kw` | `i64` |

*Defined at line 22*

---

## Lexer

```fides
impl Lexer {
```

**Methods:**

### new

```fides
pub fn new(source: str) -> Lexer {
```

### tokenize

Tokenize the entire source. Returns Vec<Token> ending with an Eof token.

```fides
pub fn tokenize(self: &mut Lexer) -> Vec<Token> {
```

### peek

Peek the current byte, or 0 (NUL) at EOF.

```fides
fn peek(self: &Lexer) -> i64 {
```

### peek_at

Peek the byte at `offset` past the current position, or -1 if out of
bounds. (Rust returns Option<char>; we use -1 as the "None" sentinel.
Callers compare against specific byte values, never against 0, since 0
is a valid NUL byte -- using -1 avoids a false match at EOF.)

```fides
fn peek_at(self: &Lexer, offset: i64) -> i64 {
```

### advance

Consume and return the current byte, advancing position and updating
line/col. Multi-byte UTF-8 sequences count as a single column.

```fides
fn advance(self: &mut Lexer) -> i64 {
```

### read_utf8

Read the UTF-8 character at the current position as a string,
advancing past all its bytes. Unlike `byte_to_str(advance())`,
this captures multi-byte sequences (e.g. em-dash -- is 3 bytes).

```fides
fn read_utf8(self: &mut Lexer) -> str {
```

### read_codepoint

Decode one UTF-8 scalar at the cursor and advance past it.

```fides
fn read_codepoint(self: &mut Lexer) -> i64 {
```

### read_hex_value

Read N hex digits (N = 2 for \xHH) or up to '}' (for \u{...}).

```fides
fn read_hex_value(self: &mut Lexer, line: i64, col: i64, max_digits: i64, until_brace: bool) -> i64 {
```

### lifetime_ahead

At a `'`: whether a lifetime name follows -- a letter or `_`, then
letters, digits and `_` with no closing `'` after them (`'a'` is a
char literal, `'a` and `'static` are lifetimes).

```fides
fn lifetime_ahead(self: &Lexer) -> bool {
```

### read_lifetime

A lifetime name `'a`; the token's text keeps the quote.

```fides
fn read_lifetime(self: &mut Lexer) -> Token {
```

### read_char_lit

R1.7: char literal 'c' -> IntLit holding the Unicode scalar value, so it
compares directly against str_byte_at results. Same escapes as strings
plus \' (and \x / \u{...}).

```fides
fn read_char_lit(self: &mut Lexer) -> Token {
```

### skip_line_comment

```fides
fn skip_line_comment(self: &mut Lexer) {
```

### skip_block_comment

```fides
fn skip_block_comment(self: &mut Lexer) {
```

### read_identifier_or_keyword

```fides
fn read_identifier_or_keyword(self: &mut Lexer) -> Token {
```

### read_number

```fides
fn read_number(self: &mut Lexer) -> Token {
```

### check_int_fits

BG61: an integer literal of 2^64 or more is an error (it wrapped
silently: 18446744073709551616 was 0). `digits` are the literal's
digits in `radix` without its prefix or `_`, `text` as written.

```fides
fn check_int_fits(self: &mut Lexer, digits: str, radix: i64, text: str, line: i64, col: i64) {
```

### read_fstring

```fides
fn read_fstring(self: &mut Lexer) -> Token {
```

### read_string

```fides
fn read_string(self: &mut Lexer) -> Token {
```

### read_raw_string

Read a raw string: r"..." or r#"..."# (with N hash signs).
No escape sequences are processed -- content is taken literally.
The closing delimiter is '"' followed by the same number of '#'.

```fides
fn read_raw_string(self: &mut Lexer) -> Token {
```

### read_operator

```fides
fn read_operator(self: &mut Lexer) -> Token {
```

*Defined at line 37*

---

## str_filter_underscores

Remove underscore separators from a numeric string (e.g. "1_000" -> "1000")

```fides
pub fn str_filter_underscores(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 951*

---

## keyword_or_ident

```fides
pub fn keyword_or_ident(s: str) -> TokenKind {
```

**Parameters:**

- `s`: `str`

**Returns:** `TokenKind`

*Defined at line 978*

---

## keyword_or_ident_own

SP9: keyword_or_ident with the not-a-keyword words kept in `own` (a map
of one lexer, so lexers on several threads share nothing); `own` 0 is
keyword_or_ident itself. The same answer for every word.

```fides
pub fn keyword_or_ident_own(own: i64, s: str) -> TokenKind {
```

**Parameters:**

- `own`: `i64`
- `s`: `str`

**Returns:** `TokenKind`

*Defined at line 1008*

---

## lex_is_whitespace

```fides
pub fn lex_is_whitespace(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 1362*

---

## lex_is_alpha

```fides
pub fn lex_is_alpha(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 1367*

---

## lex_is_digit

```fides
pub fn lex_is_digit(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 1373*

---

## lex_is_alphanumeric

```fides
pub fn lex_is_alphanumeric(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 1377*

---

## lex_fstr_expr_end

The index of the `}` that closes the f-string expression starting at
`i` (just after its `{`) in `src` of length `n`; `n` if none does.

```fides
pub fn lex_fstr_expr_end(src: str, n: i64, i: i64) -> i64 {
```

**Parameters:**

- `src`: `str`
- `n`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 1399*

---

## lex_fstr_lit_end

Just past the literal that starts at `i` in `src` (length `n`): a
string `"..."`, an f-string `f"..."` (its braces too), a raw string
`r"..."`/`r#"..."#`, or a whole char literal `'c'`, `'\n'`, `'\u{..}'`.
Any other byte (a lone `'`, a lifetime's quote) is one byte: `i + 1`.

```fides
pub fn lex_fstr_lit_end(src: str, n: i64, i: i64) -> i64 {
```

**Parameters:**

- `src`: `str`
- `n`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 1439*

---

## lex_utf8_encode

UTF-8 encoding of the scalar `cp` (escape sequences in strings: Rust's
String::push(char) in src/lexer.rs stores the char as UTF-8).

```fides
pub fn lex_utf8_encode(cp: i64) -> str {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `str`

*Defined at line 1534*

---

## lex_is_hexdigit

```fides
pub fn lex_is_hexdigit(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 1547*

---

## parse_radix

```fides
pub fn parse_radix(s: str, radix: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `radix`: `i64`

**Returns:** `i64`

*Defined at line 1559*

---

## repeat_char

Build a string by repeating `ch` (given as a byte) `count` times.
Used for the "r###" prefix in raw-string error messages.

```fides
pub fn repeat_char(ch: i64, count: i64) -> str {
```

**Parameters:**

- `ch`: `i64`
- `count`: `i64`

**Returns:** `str`

*Defined at line 1588*

---

## lex_error

```fides
pub fn lex_error(line: i64, col: i64, msg: str) -> str {
```

**Parameters:**

- `line`: `i64`
- `col`: `i64`
- `msg`: `str`

**Returns:** `str`

*Defined at line 1601*

---

## LexedText

The tokens of one text and the lexer's messages (none: it lexed clean).

```fides
pub struct LexedText {
```

**Fields:**

| Name | Type |
|------|------|
| `tokens` | `Vec<Token>` |
| `errors` | `Vec<str>` |

*Defined at line 1675*

---

## lex_text_alone

What Lexer::new(text).tokenize() gives, and its messages, lexed with a
keyword cache of this lexer's own (nothing shared with another thread).

```fides
pub fn lex_text_alone(text: str) -> LexedText {
```

**Parameters:**

- `text`: `str`

**Returns:** `LexedText`

*Defined at line 1691*

---

## lex_par_worker

A worker of lex_texts_par: the texts of its plan, each lexed alone, in
the plan's order.

```fides
pub fn lex_par_worker(w: i64) -> Vec<LexedText> {
```

**Parameters:**

- `w`: `i64`

**Returns:** `Vec<LexedText>`

*Defined at line 1705*

---

## lex_par_plan

Which texts each of at most `nw` workers lexes: the texts by size,
largest first (the first of equal ones first), each to the worker with
the fewest bytes so far (the first of equal ones); a worker's list in
text order. One list per worker, min(nw, number of texts) lists.

```fides
pub fn lex_par_plan(sizes: Vec<i64>, nw: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `sizes`: `Vec<i64>`
- `nw`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 1720*

---

## lex_texts_par

The tokens and messages of each of `texts`, in order, lexed in up to
`nw` threads (FID_PAR); with fewer than two workers or texts, lexed
here one after another. The same tokens either way.

```fides
pub fn lex_texts_par(texts: Vec<str>, nw: i64) -> Vec<LexedText> {
```

**Parameters:**

- `texts`: `Vec<str>`
- `nw`: `i64`

**Returns:** `Vec<LexedText>`

*Defined at line 1786*

---

## token_use_paths

SP9: the modules a file imports, read from its tokens without a parse:
the path of each `use` outside braces, as parse_use puts it in
ImportDef.path (`a::b` for `use a::b;`, `use a::b as c;`,
`use a::b::{x, y};` and `use a::b::*;`), in order, each once. A `use` in
a body or an impl is not an import of the module.

```fides
pub fn token_use_paths(toks: Vec<Token>) -> Vec<str> {
```

**Parameters:**

- `toks`: `Vec<Token>`

**Returns:** `Vec<str>`

*Defined at line 1853*

---

## ModScan

SP9: what the import loop reads from a module's parse, read from its
tokens: the paths of its imports (token_use_paths: uses, each once;
uses_all as written, in order), the names of its structs and enums
(`struct N` / `enum N` outside braces), its `module a::b;` name ("" when
none), and the names its `use`s put in the parser's import set (alias,
else the path's last segment; in bodies too). A guess for the parallel
parse (compiler.fd pp_predict): a wrong one costs time, not output.

```fides
pub struct ModScan {
```

**Fields:**

| Name | Type |
|------|------|
| `uses` | `Vec<str>` |
| `uses_all` | `Vec<str>` |
| `structs` | `Vec<str>` |
| `enums` | `Vec<str>` |
| `module` | `str` |
| `import_names` | `Vec<str>` |

*Defined at line 1864*

---

## token_module_scan

```fides
pub fn token_module_scan(toks: Vec<Token>) -> ModScan {
```

**Parameters:**

- `toks`: `Vec<Token>`

**Returns:** `ModScan`

*Defined at line 1900*

---

## ModGraph

SP9: the modules of an import closure, in the order they were found
(breadth first from the program's own imports): each one's import path,
its file ("" when none was found) and the modules it imports (indexes
into names; "core" is left out, as the import loop skips it).
scans[i]: what module i's tokens say (token_module_scan; empty when
its file was not lexed).

```fides
pub struct ModGraph {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `files` | `Vec<str>` |
| `deps` | `Vec<Vec<i64>>` |
| `scans` | `Vec<ModScan>` |

*Defined at line 2053*

---

## ModGraph_new

```fides
pub fn ModGraph_new() -> ModGraph {
```

**Returns:** `ModGraph`

*Defined at line 2060*

---

## ModScan_empty

```fides
pub fn ModScan_empty() -> ModScan {
```

**Returns:** `ModScan`

*Defined at line 2066*

---

## modgraph_levels

Each module's level: 0 when it imports none of the others, else one
more than the highest level of those it imports. Modules of one level
do not import each other, directly or through others, so they can be
worked on at the same time once the levels below them are done. A
module on an import cycle, or importing one, gets -1 (those stay in the
import loop's order). An index out of range, or a module importing
itself, is no dependency.

```fides
pub fn modgraph_levels(deps: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `deps`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 2079*

---

## modgraph_summary

`<n> modules, <l> levels, widest <w>` and, when some are, `, <c> on a
cycle`: how many of the modules the levels of modgraph_levels let be
worked on together.

```fides
pub fn modgraph_summary(levels: Vec<i64>) -> str {
```

**Parameters:**

- `levels`: `Vec<i64>`

**Returns:** `str`

*Defined at line 2123*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
