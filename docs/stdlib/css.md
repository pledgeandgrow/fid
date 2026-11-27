[All modules](index.md)

# css -- Fides Documentation

## Contents

- [CSS_IDENT:](#CSS_IDENT:) (const)
- [CSS_HASH:](#CSS_HASH:) (const)
- [CSS_STRING:](#CSS_STRING:) (const)
- [CSS_NUMBER:](#CSS_NUMBER:) (const)
- [CSS_DELIM:](#CSS_DELIM:) (const)
- [CSS_LBRACE:](#CSS_LBRACE:) (const)
- [CSS_RBRACE:](#CSS_RBRACE:) (const)
- [CSS_LPAREN:](#CSS_LPAREN:) (const)
- [CSS_RPAREN:](#CSS_RPAREN:) (const)
- [CSS_LBRACKET:](#CSS_LBRACKET:) (const)
- [CSS_RBRACKET:](#CSS_RBRACKET:) (const)
- [CSS_COLON:](#CSS_COLON:) (const)
- [CSS_SEMI:](#CSS_SEMI:) (const)
- [CSS_COMMA:](#CSS_COMMA:) (const)
- [CSS_ATKEYWORD:](#CSS_ATKEYWORD:) (const)
- [CSS_WS:](#CSS_WS:) (const)
- [CSS_EOF:](#CSS_EOF:) (const)
- [CssTok](#CssTok) (struct)
- [css_tokenize](#css_tokenize) (function)
- [CssDecl](#CssDecl) (struct)
- [CssRule](#CssRule) (struct)
- [CssSheet](#CssSheet) (struct)
- [css_parse](#css_parse) (function)
- [css_specificity](#css_specificity) (function)
- [css_specificity_str](#css_specificity_str) (function)
- [css_minify](#css_minify) (function)
- [css_minify_str](#css_minify_str) (function)
- [css_pretty](#css_pretty) (function)
- [css_get](#css_get) (function)

## CSS_IDENT:

```fides
pub const CSS_IDENT: i64 = 0;
```

*Defined at line 28*

---

## CSS_HASH:

```fides
pub const CSS_HASH: i64 = 1;  // #id
```

*Defined at line 29*

---

## CSS_STRING:

```fides
pub const CSS_STRING: i64 = 2;
```

*Defined at line 30*

---

## CSS_NUMBER:

```fides
pub const CSS_NUMBER: i64 = 3;  // includes units / %: the text holds it
```

*Defined at line 31*

---

## CSS_DELIM:

```fides
pub const CSS_DELIM: i64 = 4;  // one punctuation char
```

*Defined at line 32*

---

## CSS_LBRACE:

```fides
pub const CSS_LBRACE: i64 = 5;
```

*Defined at line 33*

---

## CSS_RBRACE:

```fides
pub const CSS_RBRACE: i64 = 6;
```

*Defined at line 34*

---

## CSS_LPAREN:

```fides
pub const CSS_LPAREN: i64 = 7;
```

*Defined at line 35*

---

## CSS_RPAREN:

```fides
pub const CSS_RPAREN: i64 = 8;
```

*Defined at line 36*

---

## CSS_LBRACKET:

```fides
pub const CSS_LBRACKET: i64 = 9;
```

*Defined at line 37*

---

## CSS_RBRACKET:

```fides
pub const CSS_RBRACKET: i64 = 10;
```

*Defined at line 38*

---

## CSS_COLON:

```fides
pub const CSS_COLON: i64 = 11;
```

*Defined at line 39*

---

## CSS_SEMI:

```fides
pub const CSS_SEMI: i64 = 12;
```

*Defined at line 40*

---

## CSS_COMMA:

```fides
pub const CSS_COMMA: i64 = 13;
```

*Defined at line 41*

---

## CSS_ATKEYWORD:

```fides
pub const CSS_ATKEYWORD: i64 = 14;
```

*Defined at line 42*

---

## CSS_WS:

```fides
pub const CSS_WS: i64 = 15;
```

*Defined at line 43*

---

## CSS_EOF:

```fides
pub const CSS_EOF: i64 = 16;
```

*Defined at line 44*

---

## CssTok

```fides
pub struct CssTok {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `text` | `str` |

*Defined at line 46*

---

## css_tokenize

Tokenize CSS (comments are dropped; one whitespace token per run).

```fides
pub fn css_tokenize(src: str) -> Vec<CssTok> {
```

**Parameters:**

- `src`: `str`

**Returns:** `Vec<CssTok>`

*Defined at line 70*

---

## CssDecl

```fides
pub struct CssDecl {
```

**Fields:**

| Name | Type |
|------|------|
| `property` | `str` |
| `value` | `str` |
| `important` | `bool` |

*Defined at line 198*

---

## CssRule

A rule. kind 0: a style rule (selectors + decls). kind 1: an at-rule
(`name`, `prelude`); when it has a block, `decls` or `rules` hold it.

```fides
pub struct CssRule {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `selectors` | `Vec<str>` |
| `decls` | `Vec<CssDecl>` |
| `at_name` | `str` |
| `prelude` | `str` |
| `rules` | `Vec<CssRule>` |
| `has_block` | `bool` |

*Defined at line 206*

---

## CssSheet

```fides
pub struct CssSheet {
```

**Fields:**

| Name | Type |
|------|------|
| `rules` | `Vec<CssRule>` |

*Defined at line 216*

---

## css_parse

Parse a stylesheet.

```fides
pub fn css_parse(src: str) -> CssSheet {
```

**Parameters:**

- `src`: `str`

**Returns:** `CssSheet`

*Defined at line 417*

---

## css_specificity

Specificity of one compound/complex selector as a << 16 | b << 8 | c,
where a = #id count, b = .class/[attr]/:pseudo-class, c = type/::pseudo-
element. The universal selector and combinators add nothing.

```fides
pub fn css_specificity(sel: str) -> i64 {
```

**Parameters:**

- `sel`: `str`

**Returns:** `i64`

*Defined at line 439*

---

## css_specificity_str

The (a, b, c) triple of a specificity value, as "a,b,c".

```fides
pub fn css_specificity_str(spec: i64) -> str {
```

**Parameters:**

- `spec`: `i64`

**Returns:** `str`

*Defined at line 491*

---

## css_minify

Minified text of a parsed stylesheet.

```fides
pub fn css_minify(sheet: &CssSheet) -> str {
```

**Parameters:**

- `sheet`: `&CssSheet`

**Returns:** `str`

*Defined at line 568*

---

## css_minify_str

Minify CSS source directly.

```fides
pub fn css_minify_str(src: str) -> str {
```

**Parameters:**

- `src`: `str`

**Returns:** `str`

*Defined at line 579*

---

## css_pretty

Pretty-printed text of a parsed stylesheet.

```fides
pub fn css_pretty(sheet: &CssSheet) -> str {
```

**Parameters:**

- `sheet`: `&CssSheet`

**Returns:** `str`

*Defined at line 641*

---

## css_get

A declaration's value for a property in a rule, "" when absent.

```fides
pub fn css_get(r: &CssRule, property: str) -> str {
```

**Parameters:**

- `r`: `&CssRule`
- `property`: `str`

**Returns:** `str`

*Defined at line 652*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
