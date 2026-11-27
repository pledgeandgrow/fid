[All modules](index.md)

# xml -- Fides Documentation

## Contents

- [XmlNode](#XmlNode) (struct)
- [xml_unescape](#xml_unescape) (function)
- [xml_escape](#xml_escape) (function)
- [XmlDoc](#XmlDoc) (struct)
- [XmlOptions](#XmlOptions) (struct)
- [xml_options](#xml_options) (function)
- [XML_MAX_DEPTH:](#XML_MAX_DEPTH:) (const)
- [xml_max_depth](#xml_max_depth) (function)
- [xml_parse](#xml_parse) (function)
- [xml_parse_fragment](#xml_parse_fragment) (function)
- [xml_load](#xml_load) (function)
- [xml_load_with](#xml_load_with) (function)
- [xml_attr](#xml_attr) (function)
- [xml_attr_ns](#xml_attr_ns) (function)
- [xml_child_ns](#xml_child_ns) (function)
- [xml_children_ns](#xml_children_ns) (function)
- [xml_child](#xml_child) (function)
- [xml_has](#xml_has) (function)
- [xml_children](#xml_children) (function)
- [xml_find_all](#xml_find_all) (function)
- [xml_text](#xml_text) (function)
- [xml_to_str](#xml_to_str) (function)
- [xml_elem](#xml_elem) (function)
- [xml_with_attr](#xml_with_attr) (function)
- [xml_with_child](#xml_with_child) (function)

## XmlNode

An XML element. `text` accumulates character data found directly
inside the element (concatenated, entity-decoded). Empty `tag`
marks a parse failure / not-found sentinel.

```fides
pub struct XmlNode {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `str` |
| `attr_names` | `Vec<str>` |
| `attr_vals` | `Vec<str>` |
| `text` | `str` |
| `children` | `Vec<XmlNode>` |
| `ns` | `str` |
| `local` | `str` |
| `attr_ns` | `Vec<str>` |

*Defined at line 65*

---

## xml_unescape

Decode &amp; &lt; &gt; &quot; &apos; &#NN; &#xHH; entities.

```fides
pub fn xml_unescape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 94*

---

## xml_escape

Escape & < > " in text content.

```fides
pub fn xml_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 162*

---

## XmlDoc

A strictly parsed document: `root` is the document element, or ok =
false and `error` says what is not well-formed and where.

```fides
pub struct XmlDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `root` | `XmlNode` |

*Defined at line 191*

---

## XmlOptions

Options of xml_load_with; xml_options() gives the defaults (all off).
namespaces  XML Namespaces 1.0 errors: an unbound prefix, a name with
two colons, xmlns:p="", the xml and xmlns prefixes and
names misused, two attributes with one expanded name.
(Prefixes are resolved into XmlNode.ns either way.)
validate    validity against the DOCTYPE (XML 1.0 VCs): every element
and attribute declared, content models (EMPTY, ANY, mixed,
children with , | ? * +), #REQUIRED and #FIXED attributes,
enumerations, ID (a name, unique), IDREF(S) (match an ID),
NMTOKEN(S), ENTITY/ENTITIES (unparsed entities), the root
element named by the DOCTYPE
external    load external parsed entities (<!ENTITY e SYSTEM "uri">),
external parameter entities and the external DTD subset
(<!DOCTYPE r SYSTEM "uri">) through `resolver`. Off by
default: nothing is fetched (no XXE), a reference to an
external entity in text is skipped, as XML 1.0 4.4.3
allows a processor that does not read them
resolver    fn(system_id, public_id) -> the entity's text ("" when it
cannot be had: an error)

```fides
pub struct XmlOptions {
```

**Fields:**

| Name | Type |
|------|------|
| `namespaces` | `bool` |
| `validate` | `bool` |
| `external` | `bool` |
| `resolver` | `fn(str, str) -> str` |

*Defined at line 216*

---

## xml_options

Default options: no namespace errors, no validation, no external
entities.

```fides
pub fn xml_options() -> XmlOptions {
```

**Returns:** `XmlOptions`

*Defined at line 229*

---

## XML_MAX_DEPTH:

Elements nested deeper than this are an error in a strict parse, and in
a lenient one are kept without their content, instead of overflowing
the stack. 128 and not JSON's 512: a tree of nodes is destroyed
recursively too (about five calls a level), and the interpreter's
recursion limit is 1000.

```fides
pub const XML_MAX_DEPTH: i64 = 128;
```

*Defined at line 538*

---

## xml_max_depth

The deepest nesting this build accepts: XML_MAX_DEPTH, or 48 on
WebAssembly, where the parser's frames overflow the stack at 64 (a
document at the limit must still parse on every backend).

```fides
pub fn xml_max_depth() -> i64 {
```

**Returns:** `i64`

*Defined at line 543*

---

## xml_parse

Parse an XML document leniently (malformed parts are skipped or kept
as text). Returns the root element; a node with tag "" when no element
is found.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val root = xml_parse("<cfg v=\"2\"><item>a &amp; b</item><item>c</item></cfg>");
assert_eq(root.tag, "cfg");
assert_eq(xml_attr(root, "v"), "2");
assert_eq(xml_children(root, "item").len(), 2);
assert_eq(xml_text(xml_child(root, "item")), "a & b");
assert(!xml_has(root, "other"));
assert_eq(xml_escape("<a&b>"), "&lt;a&amp;b&gt;");
assert_eq(xml_unescape("&lt;x&gt;"), "<x>");
```

```fides
pub fn xml_parse(text: str) -> XmlNode {
```

**Parameters:**

- `text`: `str`

**Returns:** `XmlNode`

*Defined at line 2668*

---

## xml_parse_fragment

Parse a fragment allowing multiple top-level elements.

```fides
pub fn xml_parse_fragment(text: str) -> Vec<XmlNode> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<XmlNode>`

*Defined at line 2678*

---

## xml_load

Parse a document strictly: it must be well-formed XML 1.0 (see the
module header); otherwise ok = false and `error` gives the line and
column of the first problem.

```fides
pub fn xml_load(text: str) -> XmlDoc {
```

**Parameters:**

- `text`: `str`

**Returns:** `XmlDoc`

*Defined at line 2687*

---

## xml_load_with

xml_load with options (XmlOptions: namespace errors, validation against
the DTD, external entities through a resolver).

```fides
pub fn xml_load_with(text: str, opts: XmlOptions) -> XmlDoc {
```

**Parameters:**

- `text`: `str`
- `opts`: `XmlOptions`

**Returns:** `XmlDoc`

*Defined at line 2693*

---

## xml_attr

Attribute value or "" when absent.

```fides
pub fn xml_attr(node: XmlNode, name: str) -> str {
```

**Parameters:**

- `node`: `XmlNode`
- `name`: `str`

**Returns:** `str`

*Defined at line 2709*

---

## xml_attr_ns

Value of the attribute `local` in namespace `ns` ("" for an unprefixed
attribute), or "" when absent.

```fides
pub fn xml_attr_ns(node: XmlNode, ns: str, local: str) -> str {
```

**Parameters:**

- `node`: `XmlNode`
- `ns`: `str`
- `local`: `str`

**Returns:** `str`

*Defined at line 2722*

---

## xml_child_ns

First direct child named `local` in namespace `ns`; tag "" when absent.

```fides
pub fn xml_child_ns(node: XmlNode, ns: str, local: str) -> XmlNode {
```

**Parameters:**

- `node`: `XmlNode`
- `ns`: `str`
- `local`: `str`

**Returns:** `XmlNode`

*Defined at line 2738*

---

## xml_children_ns

All direct children named `local` in namespace `ns`.

```fides
pub fn xml_children_ns(node: XmlNode, ns: str, local: str) -> Vec<XmlNode> {
```

**Parameters:**

- `node`: `XmlNode`
- `ns`: `str`
- `local`: `str`

**Returns:** `Vec<XmlNode>`

*Defined at line 2751*

---

## xml_child

First direct child with `tag`; tag "" sentinel when absent.

```fides
pub fn xml_child(node: XmlNode, tag: str) -> XmlNode {
```

**Parameters:**

- `node`: `XmlNode`
- `tag`: `str`

**Returns:** `XmlNode`

*Defined at line 2765*

---

## xml_has

True when node has a direct child with `tag`.

```fides
pub fn xml_has(node: XmlNode, tag: str) -> bool {
```

**Parameters:**

- `node`: `XmlNode`
- `tag`: `str`

**Returns:** `bool`

*Defined at line 2777*

---

## xml_children

All direct children with `tag`.

```fides
pub fn xml_children(node: XmlNode, tag: str) -> Vec<XmlNode> {
```

**Parameters:**

- `node`: `XmlNode`
- `tag`: `str`

**Returns:** `Vec<XmlNode>`

*Defined at line 2782*

---

## xml_find_all

All descendant elements with `tag` (document order).

```fides
pub fn xml_find_all(node: XmlNode, tag: str) -> Vec<XmlNode> {
```

**Parameters:**

- `node`: `XmlNode`
- `tag`: `str`

**Returns:** `Vec<XmlNode>`

*Defined at line 2795*

---

## xml_text

All descendant-or-self text concatenated (entity-decoded).

```fides
pub fn xml_text(node: XmlNode) -> str {
```

**Parameters:**

- `node`: `XmlNode`

**Returns:** `str`

*Defined at line 2813*

---

## xml_to_str

Serialize a node tree back to XML text (no pretty indentation).

```fides
pub fn xml_to_str(node: XmlNode) -> str {
```

**Parameters:**

- `node`: `XmlNode`

**Returns:** `str`

*Defined at line 2830*

---

## xml_elem

Build an element node.

```fides
pub fn xml_elem(tag: str, text: str) -> XmlNode {
```

**Parameters:**

- `tag`: `str`
- `text`: `str`

**Returns:** `XmlNode`

*Defined at line 2851*

---

## xml_with_attr

Add an attribute to a node (returns the node for chaining).

```fides
pub fn xml_with_attr(node: XmlNode, name: str, value: str) -> XmlNode {
```

**Parameters:**

- `node`: `XmlNode`
- `name`: `str`
- `value`: `str`

**Returns:** `XmlNode`

*Defined at line 2858*

---

## xml_with_child

Append a child (returns the parent for chaining).

```fides
pub fn xml_with_child(node: XmlNode, child: XmlNode) -> XmlNode {
```

**Parameters:**

- `node`: `XmlNode`
- `child`: `XmlNode`

**Returns:** `XmlNode`

*Defined at line 2867*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
