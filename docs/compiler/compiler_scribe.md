[All modules](index.md)

# compiler::scribe -- Fides Documentation

## Contents

- [StringPair](#StringPair) (struct)
- [StringPair_new](#StringPair_new) (function)
- [StringTriple](#StringTriple) (struct)
- [StringTriple_new](#StringTriple_new) (function)
- [ParamsRet](#ParamsRet) (struct)
- [ParamsRet_new](#ParamsRet_new) (function)
- [DocItem](#DocItem) (struct)
- [DocItem_new](#DocItem_new) (function)
- [extract_docs](#extract_docs) (function)
- [OptionDocItem](#OptionDocItem) (enum)
- [ParseResult](#ParseResult) (struct)
- [ParseResult_new](#ParseResult_new) (function)
- [scribe_code_braces](#scribe_code_braces) (function)
- [extract_name_after](#extract_name_after) (function)
- [escape_html](#escape_html) (function)
- [generate_html](#generate_html) (function)
- [generate_html_ctx](#generate_html_ctx) (function)
- [generate_markdown](#generate_markdown) (function)
- [CSS:](#CSS:) (const)
- [SCRIBE_CSS_MORE:](#SCRIBE_CSS_MORE:) (const)
- [scribe_process_file](#scribe_process_file) (function)
- [process_directory](#process_directory) (function)
- [scribe_page_name](#scribe_page_name) (function)
- [scribe_collect](#scribe_collect) (function)
- [DocCtx](#DocCtx) (struct)
- [DocCtx_new](#DocCtx_new) (function)
- [scribe_register_types](#scribe_register_types) (function)
- [scribe_type_href](#scribe_type_href) (function)
- [scribe_link_types](#scribe_link_types) (function)
- [scribe_location_html](#scribe_location_html) (function)
- [scribe_source_page](#scribe_source_page) (function)
- [scribe_doc_line](#scribe_doc_line) (function)
- [scribe_example_marker](#scribe_example_marker) (function)
- [DocExample](#DocExample) (struct)
- [DocExample_new](#DocExample_new) (function)
- [scribe_doctests](#scribe_doctests) (function)
- [scribe_doctest_program](#scribe_doctest_program) (function)
- [scribe_doc_html](#scribe_doc_html) (function)
- [scribe_doc_md](#scribe_doc_md) (function)
- [scribe_dir_of](#scribe_dir_of) (function)
- [scribe_toml_value](#scribe_toml_value) (function)
- [scribe_doc_version](#scribe_doc_version) (function)
- [scribe_site](#scribe_site) (function)
- [scribe_print_help](#scribe_print_help) (function)
- [str_rfind_char](#str_rfind_char) (function)
- [str_trim_start_str](#str_trim_start_str) (function)
- [scribe_mkdir_all](#scribe_mkdir_all) (function)
- [scribe_help_text](#scribe_help_text) (function)
- [scribe_main](#scribe_main) (function)

## StringPair

```fides
pub struct StringPair {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `str` |

*Defined at line 20*

---

## StringPair_new

```fides
pub fn StringPair_new(a: str, b: str) -> StringPair {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `StringPair`

*Defined at line 25*

---

## StringTriple

```fides
pub struct StringTriple {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `str` |
| `c` | `str` |

*Defined at line 31*

---

## StringTriple_new

```fides
pub fn StringTriple_new(a: str, b: str, c: str) -> StringTriple {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`

**Returns:** `StringTriple`

*Defined at line 37*

---

## ParamsRet

```fides
pub struct ParamsRet {
```

**Fields:**

| Name | Type |
|------|------|
| `params` | `Vec<StringPair>` |
| `ret_type` | `str` |

*Defined at line 43*

---

## ParamsRet_new

```fides
pub fn ParamsRet_new(params: Vec<StringPair>, ret_type: str) -> ParamsRet {
```

**Parameters:**

- `params`: `Vec<StringPair>`
- `ret_type`: `str`

**Returns:** `ParamsRet`

*Defined at line 48*

---

## DocItem

```fides
pub struct DocItem {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `name` | `str` |
| `signature` | `str` |
| `doc` | `str` |
| `line` | `i64` |
| `fields` | `Vec<StringTriple>` |
| `params` | `Vec<StringPair>` |
| `ret_type` | `str` |
| `methods` | `Vec<DocItem>` |
| `variants` | `Vec<StringPair>` |
| `is_pub` | `bool` |

*Defined at line 56*

---

## DocItem_new

```fides
pub fn DocItem_new(kind: str, name: str, signature: str, doc: str, line: i64, is_pub: bool) -> DocItem {
```

**Parameters:**

- `kind`: `str`
- `name`: `str`
- `signature`: `str`
- `doc`: `str`
- `line`: `i64`
- `is_pub`: `bool`

**Returns:** `DocItem`

*Defined at line 78*

---

## extract_docs

Extract doc comments and items from source text

```fides
pub fn extract_docs(source: str) -> Vec<DocItem> {
```

**Parameters:**

- `source`: `str`

**Returns:** `Vec<DocItem>`

*Defined at line 87*

---

## OptionDocItem

```fides
pub enum OptionDocItem {
```

**Variants:**

- `Some`
- `None`

*Defined at line 199*

---

## ParseResult

```fides
pub struct ParseResult {
```

**Fields:**

| Name | Type |
|------|------|
| `item` | `OptionDocItem` |
| `end` | `i64` |

*Defined at line 206*

---

## ParseResult_new

```fides
pub fn ParseResult_new(item: OptionDocItem, end: i64) -> ParseResult {
```

**Parameters:**

- `item`: `OptionDocItem`
- `end`: `i64`

**Returns:** `ParseResult`

*Defined at line 211*

---

## scribe_code_braces

The `{` and `}` of each line that are code, in order: none inside a
string, an f-string, a raw string, a char literal or a comment (a block
comment, a string or a raw string may run over several lines). The item
parsers count these, not every brace of the line: `return "{";` or a
`// }` swallowed the items after it or cut one short (BG105).

```fides
pub fn scribe_code_braces(lines: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `lines`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 228*

---

## extract_name_after

```fides
pub fn extract_name_after(s: str, prefix: str) -> str {
```

**Parameters:**

- `s`: `str`
- `prefix`: `str`

**Returns:** `str`

*Defined at line 663*

---

## escape_html

```fides
pub fn escape_html(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 752*

---

## generate_html

```fides
pub fn generate_html(items: Vec<DocItem>, module_name: str) -> str {
```

**Parameters:**

- `items`: `Vec<DocItem>`
- `module_name`: `str`

**Returns:** `str`

*Defined at line 762*

---

## generate_html_ctx

TL11: a module's page; `ctx` names the types other pages document
(signatures link to them), the module's source page (each item links
to its line) and the version label.

```fides
pub fn generate_html_ctx(items: Vec<DocItem>, module_name: str, ctx: DocCtx) -> str {
```

**Parameters:**

- `items`: `Vec<DocItem>`
- `module_name`: `str`
- `ctx`: `DocCtx`

**Returns:** `str`

*Defined at line 769*

---

## generate_markdown

```fides
pub fn generate_markdown(items: Vec<DocItem>, module_name: str) -> str {
```

**Parameters:**

- `items`: `Vec<DocItem>`
- `module_name`: `str`

**Returns:** `str`

*Defined at line 1014*

---

## CSS:

```fides
pub const CSS: str = "body { font-family: -apple-system, sans-serif; max-width: 900px; margin: 0 auto; padding: 20px; color: #333; }\nnav.sidebar { float: right; width: 200px; padding: 10px; background: #f5f5f5; border-radius: 5px; }\nnav.sidebar ul { list-style: none; padding: 0; }\nnav.sidebar li { padding: 4px 0; }\n.kind { color: #999; font-size: 0.85em; }\nsection { border-bottom: 1px solid #eee; padding: 20px 0; }\nh1 { color: #2c3e50; }\nh2 { color: #34495e; margin-top: 30px; }\npre { background: #f8f8f8; padding: 10px; border-radius: 5px; overflow-x: auto; }\ncode { font-family: 'Fira Code', monospace; font-size: 0.9em; }\ntable { border-collapse: collapse; margin: 10px 0; }\ntd { border: 1px solid #ddd; padding: 6px 12px; }\n.line { color: #999; font-size: 0.85em; }\n.doc { margin: 10px 0; line-height: 1.5; }\n";
```

*Defined at line 1105*

---

## SCRIBE_CSS_MORE:

TL11: type links, source links, examples and the version label.

```fides
pub const SCRIBE_CSS_MORE: str = "a.ty { color: #2a6db0; text-decoration: none; }\na.ty:hover { text-decoration: underline; }\n.location a.src { color: #777; }\n.version { color: #666; font-size: 0.9em; margin-top: -10px; }\n.example-note { color: #555; font-size: 0.85em; margin: 8px 0 2px; }\npre.example { background: #f3f7f3; border-left: 3px solid #8b8; }\n";
```

*Defined at line 1107*

---

## scribe_process_file

```fides
pub fn scribe_process_file(input_path: str, output_dir: str, format: str) -> str {
```

**Parameters:**

- `input_path`: `str`
- `output_dir`: `str`
- `format`: `str`

**Returns:** `str`

*Defined at line 1110*

---

## process_directory

`fid doc <dir>` (TL11): a documentation site for the .fd files under
`input_dir`, as scribe_site writes it. A .fd file is one page; a file
that lists paths (one a line) documents those files.

```fides
pub fn process_directory(input_dir: str, output_dir: str, format: str) -> str {
```

**Parameters:**

- `input_dir`: `str`
- `output_dir`: `str`
- `format`: `str`

**Returns:** `str`

*Defined at line 1165*

---

## scribe_page_name

The file name of a module's page, without the extension: `a::b` is
`a_b` (`:` cannot be in a Windows file name).

```fides
pub fn scribe_page_name(module_name: str) -> str {
```

**Parameters:**

- `module_name`: `str`

**Returns:** `str`

*Defined at line 1204*

---

## scribe_collect

The .fd files under `dir` (recursively; not in hidden directories or
ark_modules/), appended to `out`.

```fides
pub fn scribe_collect(dir: str, out: &mut Vec<str>) {
```

**Parameters:**

- `dir`: `str`
- `out`: `&mut Vec<str>`

*Defined at line 1210*

---

## DocCtx

What a page needs to link outside one item: the types the site
documents (`types`: a type's name, and `<page>#<name>` for the page that
declares it, to an index into `pages`), this page's name, its source
page (`src_href`, "" for none) and file, and the version label.

```fides
pub struct DocCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `types` | `HashMap<str` |
| `pages` | `Vec<str>` |
| `page` | `str` |
| `src_href` | `str` |
| `src_label` | `str` |
| `version` | `str` |

*Defined at line 1268*

---

## DocCtx_new

A context that links nothing (a page on its own).

```fides
pub fn DocCtx_new() -> DocCtx {
```

**Returns:** `DocCtx`

*Defined at line 1279*

---

## scribe_register_types

Enter the types `items` declare (pub structs and enums, and caps, whose
sections render_* gives the id of their name) as sections of `page`.
The first page to declare a name is the one other pages link to.

```fides
pub fn scribe_register_types(ctx: &mut DocCtx, items: Vec<DocItem>, page: str) {
```

**Parameters:**

- `ctx`: `&mut DocCtx`
- `items`: `Vec<DocItem>`
- `page`: `str`

*Defined at line 1290*

---

## scribe_type_href

The link to the section of type `name` ("#Name" on this page,
"page.html#Name" on another), or "" when no page documents it.

```fides
pub fn scribe_type_href(ctx: DocCtx, name: str) -> str {
```

**Parameters:**

- `ctx`: `DocCtx`
- `name`: `str`

**Returns:** `str`

*Defined at line 1311*

---

## scribe_link_types

`text` (a signature or a type) as HTML in which each type name a page
documents links to its section. `self_name` (the item declared) stays
text, and so do string literals and a trailing `//` comment.

```fides
pub fn scribe_link_types(text: str, ctx: DocCtx, self_name: str) -> str {
```

**Parameters:**

- `text`: `str`
- `ctx`: `DocCtx`
- `self_name`: `str`

**Returns:** `str`

*Defined at line 1335*

---

## scribe_location_html

Where an item is defined: a link to its line on the module's source
page when there is one, else the line number.

```fides
pub fn scribe_location_html(line: i64, ctx: DocCtx) -> str {
```

**Parameters:**

- `line`: `i64`
- `ctx`: `DocCtx`

**Returns:** `str`

*Defined at line 1388*

---

## scribe_source_page

The page of a module's source (written to src/<page>.html): a line
each, `id="L<n>"`, so a location links to its line, and a link back
to the module's page.

```fides
pub fn scribe_source_page(source: str, label: str, module_page: str) -> str {
```

**Parameters:**

- `source`: `str`
- `label`: `str`
- `module_page`: `str`

**Returns:** `str`

*Defined at line 1398*

---

## scribe_doc_line

A `///` line's text as the doc keeps it: `rest` (what follows `///`)
trimmed, except for a line of an example as scripts/doctest.py reads
one (`///` in column 0, then two spaces or more), which keeps its
indentation so scribe_doc_html can find the example.

```fides
pub fn scribe_doc_line(raw: str, rest: str) -> str {
```

**Parameters:**

- `raw`: `str`
- `rest`: `str`

**Returns:** `str`

*Defined at line 1428*

---

## scribe_example_marker

-1 unless `l` (trimmed) is doctest.py's example marker: 0 for
`Example:`, 1 for `Example (native only):`.

```fides
pub fn scribe_example_marker(l: str) -> i64 {
```

**Parameters:**

- `l`: `str`

**Returns:** `i64`

*Defined at line 1437*

---

## DocExample

A `/// Example:` block `fid doctest` runs. `module_name` is what the
generated program's `use` imports; `native` marks an
`Example (native only):` block (skipped without --emit).

```fides
pub struct DocExample {
```

**Fields:**

| Name | Type |
|------|------|
| `module_name` | `str` |
| `native` | `bool` |
| `code` | `str` |

*Defined at line 1497*

---

## DocExample_new

```fides
pub fn DocExample_new(module_name: str, native: bool, code: str) -> DocExample {
```

**Parameters:**

- `module_name`: `str`
- `native`: `bool`
- `code`: `str`

**Returns:** `DocExample`

*Defined at line 1503*

---

## scribe_doctests

The `/// Example:` blocks of `source` (scripts/doctest.py's convention):
a doc line `Example:` or `Example (native only):`, then the doc lines
after it that start `///` followed by two spaces or more. `fallback_mod`
is what `use` imports when the file declares no `module`.

```fides
pub fn scribe_doctests(source: str, fallback_mod: str) -> Vec<DocExample> {
```

**Parameters:**

- `source`: `str`
- `fallback_mod`: `str`

**Returns:** `Vec<DocExample>`

*Defined at line 1513*

---

## scribe_doctest_program

The standalone program a doc example builds to (doctest.py's
build_prog): `use <module_name>;`, the example's own `use` lines
hoisted, the rest inside `fn main` (unless it declares one).

```fides
pub fn scribe_doctest_program(module_name: str, code: str) -> str {
```

**Parameters:**

- `module_name`: `str`
- `code`: `str`

**Returns:** `str`

*Defined at line 1573*

---

## scribe_doc_html

A doc comment as HTML: its lines joined with <br>, and each example
(doctest.py's convention) as a code block with a note that
scripts/doctest.py runs it.

```fides
pub fn scribe_doc_html(doc: str) -> str {
```

**Parameters:**

- `doc`: `str`

**Returns:** `str`

*Defined at line 1631*

---

## scribe_doc_md

A doc comment as Markdown, its examples as fenced code.

```fides
pub fn scribe_doc_md(doc: str) -> str {
```

**Parameters:**

- `doc`: `str`

**Returns:** `str`

*Defined at line 1666*

---

## scribe_dir_of

The directory part of `path` ("." for none).

```fides
pub fn scribe_dir_of(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 1693*

---

## scribe_toml_value

The value of `key = "…"` in the [package] section (or before any
section) of an ark.toml, or "".

```fides
pub fn scribe_toml_value(toml: str, key: str) -> str {
```

**Parameters:**

- `toml`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 1707*

---

## scribe_doc_version

The version label of the documentation of `dir`: $FID_DOC_VERSION when
set; else the name and version of the package (an ark.toml in `dir` or
its parent); else the fid version (and the commit a release build has).

```fides
pub fn scribe_doc_version(dir: str) -> str {
```

**Parameters:**

- `dir`: `str`

**Returns:** `str`

*Defined at line 1737*

---

## scribe_site

TL11: a documentation site for the .fd files under `input_dir`, written
to `output_dir`: a page per module (as `fid doc <file>` writes it, with a
link back to the index) and an index — every module with the first line
of its file's leading comment and its count of public items, and (HTML)
a search box over every public item's name, kind and signature that
links to the item's anchor. Markdown: index.md with the module list.

```fides
pub fn scribe_site(input_dir: str, output_dir: str, format: str) -> str {
```

**Parameters:**

- `input_dir`: `str`
- `output_dir`: `str`
- `format`: `str`

**Returns:** `str`

*Defined at line 1789*

---

## scribe_print_help

```fides
pub fn scribe_print_help() -> str {
```

**Returns:** `str`

*Defined at line 2012*

---

## str_rfind_char

```fides
pub fn str_rfind_char(s: str, ch: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `i64`

*Defined at line 2029*

---

## str_trim_start_str

```fides
pub fn str_trim_start_str(s: str, prefix: str) -> str {
```

**Parameters:**

- `s`: `str`
- `prefix`: `str`

**Returns:** `str`

*Defined at line 2064*

---

## scribe_mkdir_all

Make `path` and every parent it lacks (as fs::create_dir_all); an
existing directory is fine. Both separators count.

```fides
pub fn scribe_mkdir_all(path: str) {
```

**Parameters:**

- `path`: `str`

*Defined at line 2086*

---

## scribe_help_text

```fides
pub fn scribe_help_text() -> str {
```

**Returns:** `str`

*Defined at line 2113*

---

## scribe_main

Standalone `scribe.exe` entry; also serves `fid scribe` (skips the
leading "scribe" argument, and a "--" separator from `fid run`).
Arguments as the Rust scribe takes them (src/bin/scribe.rs, AD1):
options anywhere, `-o`/`-f` short forms, `--help`/`-h` (exit 0),
`--version`/`-v`, one input; an error goes to stderr with exit 1.

```fides
pub fn scribe_main() -> i64 {
```

**Returns:** `i64`

*Defined at line 2127*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
