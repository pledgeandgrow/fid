[All modules](index.md)

# html -- Fides Documentation

## Contents

- [h_escape](#h_escape) (function)
- [h_escape_attr](#h_escape_attr) (function)
- [h_is_void](#h_is_void) (function)
- [HAttrs](#HAttrs) (struct)
- [h_attrs](#h_attrs) (function)
- [h_attr](#h_attr) (function)
- [h_attr_bool](#h_attr_bool) (function)
- [h_attrs1](#h_attrs1) (function)
- [h_render_attrs](#h_render_attrs) (function)
- [h_el](#h_el) (function)
- [h_tag](#h_tag) (function)
- [h_text](#h_text) (function)
- [h_seq](#h_seq) (function)
- [h_document](#h_document) (function)
- [h_unescape](#h_unescape) (function)
- [H_ELEMENT:](#H_ELEMENT:) (const)
- [H_TEXT:](#H_TEXT:) (const)
- [H_COMMENT:](#H_COMMENT:) (const)
- [HtmlNode](#HtmlNode) (struct)
- [HtmlDoc](#HtmlDoc) (struct)
- [html_parse](#html_parse) (function)
- [html_attr](#html_attr) (function)
- [html_has_attr](#html_has_attr) (function)
- [html_by_tag](#html_by_tag) (function)
- [html_by_id](#html_by_id) (function)
- [html_by_class](#html_by_class) (function)
- [html_text_of](#html_text_of) (function)
- [html_element_count](#html_element_count) (function)
- [html_render](#html_render) (function)

## h_escape

Escape text content (& < >).

```fides
pub fn h_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 30*

---

## h_escape_attr

Escape an attribute value (& < > " ').

```fides
pub fn h_escape_attr(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 50*

---

## h_is_void

Is `tag` a void element (no end tag, no content)?

```fides
pub fn h_is_void(tag: str) -> bool {
```

**Parameters:**

- `tag`: `str`

**Returns:** `bool`

*Defined at line 74*

---

## HAttrs

An attribute list: parallel names and values. A value of "" with the
name marked boolean prints the bare name (checked, disabled...).

```fides
pub struct HAttrs {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `bare` | `Vec<bool>` |

*Defined at line 91*

---

## h_attrs

```fides
pub fn h_attrs() -> HAttrs {
```

**Returns:** `HAttrs`

*Defined at line 97*

---

## h_attr

Add a name=value attribute.

```fides
pub fn h_attr(a: &mut HAttrs, name: str, value: str) {
```

**Parameters:**

- `a`: `&mut HAttrs`
- `name`: `str`
- `value`: `str`

*Defined at line 104*

---

## h_attr_bool

Add a boolean attribute (printed as the bare name).

```fides
pub fn h_attr_bool(a: &mut HAttrs, name: str) {
```

**Parameters:**

- `a`: `&mut HAttrs`
- `name`: `str`

*Defined at line 111*

---

## h_attrs1

A one-attribute list (a common case).

```fides
pub fn h_attrs1(name: str, value: str) -> HAttrs {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `HAttrs`

*Defined at line 118*

---

## h_render_attrs

Render an attribute list (with a leading space when non-empty).

```fides
pub fn h_render_attrs(a: &HAttrs) -> str {
```

**Parameters:**

- `a`: `&HAttrs`

**Returns:** `str`

*Defined at line 125*

---

## h_el

An element with attributes and already-rendered inner HTML.

```fides
pub fn h_el(tag: str, attrs: HAttrs, inner: str) -> str {
```

**Parameters:**

- `tag`: `str`
- `attrs`: `HAttrs`
- `inner`: `str`

**Returns:** `str`

*Defined at line 139*

---

## h_tag

An element with no attributes.

```fides
pub fn h_tag(tag: str, inner: str) -> str {
```

**Parameters:**

- `tag`: `str`
- `inner`: `str`

**Returns:** `str`

*Defined at line 147*

---

## h_text

A text node (escaped).

```fides
pub fn h_text(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 152*

---

## h_seq

Join rendered children.

```fides
pub fn h_seq(parts: Vec<str>) -> str {
```

**Parameters:**

- `parts`: `Vec<str>`

**Returns:** `str`

*Defined at line 157*

---

## h_document

A complete document with a doctype.

```fides
pub fn h_document(head: str, body: str) -> str {
```

**Parameters:**

- `head`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 168*

---

## h_unescape

Decode HTML character references in `s` (named, decimal, hex).

```fides
pub fn h_unescape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 269*

---

## H_ELEMENT:

```fides
pub const H_ELEMENT: i64 = 0;
```

*Defined at line 346*

---

## H_TEXT:

```fides
pub const H_TEXT: i64 = 1;
```

*Defined at line 347*

---

## H_COMMENT:

```fides
pub const H_COMMENT: i64 = 2;
```

*Defined at line 348*

---

## HtmlNode

A node of the parse tree (flat arena, children by index).

```fides
pub struct HtmlNode {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `tag` | `str` |
| `text` | `str` |
| `attrs` | `HAttrs` |
| `parent` | `i64` |
| `children` | `Vec<i64>` |

*Defined at line 351*

---

## HtmlDoc

```fides
pub struct HtmlDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `nodes` | `Vec<HtmlNode>` |
| `root` | `i64` |

*Defined at line 360*

---

## html_parse

Parse HTML into a tree. The root is an element node named "#document".

```fides
pub fn html_parse(src: str) -> HtmlDoc {
```

**Parameters:**

- `src`: `str`

**Returns:** `HtmlDoc`

*Defined at line 484*

---

## html_attr

An attribute's value, "" when absent.

```fides
pub fn html_attr(node: &HtmlNode, name: str) -> str {
```

**Parameters:**

- `node`: `&HtmlNode`
- `name`: `str`

**Returns:** `str`

*Defined at line 635*

---

## html_has_attr

```fides
pub fn html_has_attr(node: &HtmlNode, name: str) -> bool {
```

**Parameters:**

- `node`: `&HtmlNode`
- `name`: `str`

**Returns:** `bool`

*Defined at line 646*

---

## html_by_tag

The indexes of every element with tag name `tag`.

```fides
pub fn html_by_tag(d: &HtmlDoc, tag: str) -> Vec<i64> {
```

**Parameters:**

- `d`: `&HtmlDoc`
- `tag`: `str`

**Returns:** `Vec<i64>`

*Defined at line 670*

---

## html_by_id

The first element whose id attribute is `id`, or -1.

```fides
pub fn html_by_id(d: &HtmlDoc, id: str) -> i64 {
```

**Parameters:**

- `d`: `&HtmlDoc`
- `id`: `str`

**Returns:** `i64`

*Defined at line 684*

---

## html_by_class

Every element carrying class `cls`.

```fides
pub fn html_by_class(d: &HtmlDoc, cls: str) -> Vec<i64> {
```

**Parameters:**

- `d`: `&HtmlDoc`
- `cls`: `str`

**Returns:** `Vec<i64>`

*Defined at line 697*

---

## html_text_of

The concatenated text of a node and its descendants.

```fides
pub fn html_text_of(d: &HtmlDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&HtmlDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 711*

---

## html_element_count

The number of element nodes in the tree.

```fides
pub fn html_element_count(d: &HtmlDoc) -> i64 {
```

**Parameters:**

- `d`: `&HtmlDoc`

**Returns:** `i64`

*Defined at line 726*

---

## html_render

Serialize a node back to HTML (re-escaping text and attributes).

```fides
pub fn html_render(d: &HtmlDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&HtmlDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 739*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
