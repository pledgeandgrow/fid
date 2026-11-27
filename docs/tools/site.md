[All modules](index.md)

# site -- Fides Documentation

## Contents

- [site_repo_url](#site_repo_url) (function)
- [site_sections](#site_sections) (function)
- [site_top_pages](#site_top_pages) (function)
- [site_join](#site_join) (function)
- [site_is_section](#site_is_section) (function)
- [site_is_source](#site_is_source) (function)
- [site_has_page](#site_has_page) (function)
- [site_html_path](#site_html_path) (function)
- [site_prefix](#site_prefix) (function)
- [site_pct_decode](#site_pct_decode) (function)
- [site_clean_path](#site_clean_path) (function)
- [SiteRoute](#SiteRoute) (struct)
- [site_route](#site_route) (function)
- [site_cells](#site_cells) (function)
- [site_strip_external_img](#site_strip_external_img) (function)
- [site_render_md](#site_render_md) (function)
- [site_resolve](#site_resolve) (function)
- [site_link](#site_link) (function)
- [site_rewrite_links](#site_rewrite_links) (function)
- [site_text](#site_text) (function)
- [site_slug](#site_slug) (function)
- [site_anchor_headings](#site_anchor_headings) (function)
- [site_title_of](#site_title_of) (function)
- [site_section_pages](#site_section_pages) (function)
- [site_layout](#site_layout) (function)
- [site_render_page](#site_render_page) (function)
- [site_render_index](#site_render_index) (function)
- [site_all_pages](#site_all_pages) (function)
- [site_render_pages](#site_render_pages) (function)
- [site_render_missing](#site_render_missing) (function)
- [site_css](#site_css) (function)
- [site_respond](#site_respond) (function)
- [site_build](#site_build) (function)

## site_repo_url

```fides
pub fn site_repo_url() -> str {
```

**Returns:** `str`

*Defined at line 35*

---

## site_sections

Directories whose *.md files (one level) are pages.

```fides
pub fn site_sections() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 42*

---

## site_top_pages

Pages outside the sections.

```fides
pub fn site_top_pages() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 51*

---

## site_join

The parts joined, pairwise (no quadratic copying; core's StringBuilder
keeps its buffer for good, which a server cannot afford).

```fides
pub fn site_join(parts: Vec<str>) -> str {
```

**Parameters:**

- `parts`: `Vec<str>`

**Returns:** `str`

*Defined at line 63*

---

## site_is_section

```fides
pub fn site_is_section(dir: str) -> bool {
```

**Parameters:**

- `dir`: `str`

**Returns:** `bool`

*Defined at line 88*

---

## site_is_source

May repository path `rel` be shown as a page? (It need not exist.)

```fides
pub fn site_is_source(rel: str) -> bool {
```

**Parameters:**

- `rel`: `str`

**Returns:** `bool`

*Defined at line 93*

---

## site_has_page

A page that exists.

```fides
pub fn site_has_page(rel: str) -> bool {
```

**Parameters:**

- `rel`: `str`

**Returns:** `bool`

*Defined at line 109*

---

## site_html_path

The site path of page `src`: README.md is index.html, a.md is a.html.

```fides
pub fn site_html_path(src: str) -> str {
```

**Parameters:**

- `src`: `str`

**Returns:** `str`

*Defined at line 114*

---

## site_prefix

"../" once per directory of site path `html`: from that page to the root.

```fides
pub fn site_prefix(html: str) -> str {
```

**Parameters:**

- `html`: `str`

**Returns:** `str`

*Defined at line 120*

---

## site_pct_decode

Percent-decoding of a request path ('+' stays '+'; a '%' not followed
by two hex digits stays, and site_clean_path then refuses it).

```fides
pub fn site_pct_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 141*

---

## site_clean_path

The decoded request path, still starting with '/', or "" when it is
refused: not starting with '/', too long, "%00", a control byte, a
backslash, ':', "//", a '%' left after decoding (double encoding), or a
segment that is "..", "." or starts with '.'.

```fides
pub fn site_clean_path(raw: str) -> str {
```

**Parameters:**

- `raw`: `str`

**Returns:** `str`

*Defined at line 162*

---

## SiteRoute

Where a request goes. `kind`: "page" (target: the Markdown file),
"index" (target: a section), "pages", "css", "svg" (target: the file),
"redirect" (target: the Location), "missing" (404), "refused" (403).

```fides
pub struct SiteRoute {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `kind` | `str` |
| `target` | `str` |

*Defined at line 183*

---

## site_route

Route a raw request path (no query).

```fides
pub fn site_route(raw: str) -> SiteRoute {
```

**Parameters:**

- `raw`: `str`

**Returns:** `SiteRoute`

*Defined at line 194*

---

## site_cells

The cells of a table row: split at '|' not escaped by '\', outer pipes
dropped, "\|" read as '|' (GitHub's rule, also inside `code`).

```fides
pub fn site_cells(line: str) -> Vec<str> {
```

**Parameters:**

- `line`: `str`

**Returns:** `Vec<str>`

*Defined at line 243*

---

## site_strip_external_img

Raw HTML from the repository's own Markdown is passed through, except an
<img> from another site (a badge), which becomes its alt text: the site
loads nothing from elsewhere.

```fides
pub fn site_strip_external_img(html: str) -> str {
```

**Parameters:**

- `html`: `str`

**Returns:** `str`

*Defined at line 330*

---

## site_render_md

Markdown to an HTML fragment: tables and HTML blocks here, the rest by
markdown.fd's md_to_html.

```fides
pub fn site_render_md(text0: str) -> str {
```

**Parameters:**

- `text0`: `str`

**Returns:** `str`

*Defined at line 357*

---

## site_resolve

`rel` resolved against directory `dir` ("" is the root), with "." and
".." taken out; "" when it climbs above the root.

```fides
pub fn site_resolve(dir: str, rel: str) -> str {
```

**Parameters:**

- `dir`: `str`
- `rel`: `str`

**Returns:** `str`

*Defined at line 414*

---

## site_link

A link of page `src` (a repository path) as the site writes it: a page
becomes its .html, relative to this page; the logo stays here; any other
file of the repository goes to GitHub; other sites and #anchors are kept.

```fides
pub fn site_link(href: str, src: str) -> str {
```

**Parameters:**

- `href`: `str`
- `src`: `str`

**Returns:** `str`

*Defined at line 450*

---

## site_rewrite_links

Every href="..." and src="..." of `html` through site_link.

```fides
pub fn site_rewrite_links(html: str, src: str) -> str {
```

**Parameters:**

- `html`: `str`
- `src`: `str`

**Returns:** `str`

*Defined at line 479*

---

## site_text

Text of an HTML fragment: tags dropped, the entities md_escape makes read.

```fides
pub fn site_text(html: str) -> str {
```

**Parameters:**

- `html`: `str`

**Returns:** `str`

*Defined at line 506*

---

## site_slug

GitHub's anchor of a heading: lower case, spaces to '-', other
punctuation dropped (also the common non-ASCII marks: — – → § “ ”).

```fides
pub fn site_slug(text: str) -> str {
```

**Parameters:**

- `text`: `str`

**Returns:** `str`

*Defined at line 527*

---

## site_anchor_headings

Give each <hN> of md_to_html an id (GitHub's anchor; a repeat gets -1,
-2, ...). The first <h1>'s text comes back through `title`.

```fides
pub fn site_anchor_headings(html: str, title: &mut str) -> str {
```

**Parameters:**

- `html`: `str`
- `title`: `&mut str`

**Returns:** `str`

*Defined at line 555*

---

## site_title_of

The first "# " heading of a Markdown file, or its name.

```fides
pub fn site_title_of(src: str) -> str {
```

**Parameters:**

- `src`: `str`

**Returns:** `str`

*Defined at line 596*

---

## site_section_pages

The pages of a section, README.md first, then by file name.

```fides
pub fn site_section_pages(dir: str) -> Vec<str> {
```

**Parameters:**

- `dir`: `str`

**Returns:** `Vec<str>`

*Defined at line 625*

---

## site_layout

The page around `body`. `html` is the page's own site path (it sets the
relative prefix), `src` its Markdown file ("" for a generated page),
`section` the sidebar to show ("" for none).

```fides
pub fn site_layout(title: str, body: str, html: str, src: str, section: str) -> str {
```

**Parameters:**

- `title`: `str`
- `body`: `str`
- `html`: `str`
- `src`: `str`
- `section`: `str`

**Returns:** `str`

*Defined at line 655*

---

## site_render_page

The HTML page of Markdown file `src`.

```fides
pub fn site_render_page(src: str) -> str {
```

**Parameters:**

- `src`: `str`

**Returns:** `str`

*Defined at line 703*

---

## site_render_index

The generated list of a section without a README.md.

```fides
pub fn site_render_index(dir: str) -> str {
```

**Parameters:**

- `dir`: `str`

**Returns:** `str`

*Defined at line 736*

---

## site_all_pages

Every page of the site.

```fides
pub fn site_all_pages() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 743*

---

## site_render_pages

```fides
pub fn site_render_pages() -> str {
```

**Returns:** `str`

*Defined at line 765*

---

## site_render_missing

The 404 page. Its links start at `base` (the site's root URL path): it
is shown for a path of any depth.

```fides
pub fn site_render_missing(base: str) -> str {
```

**Parameters:**

- `base`: `str`

**Returns:** `str`

*Defined at line 789*

---

## site_css

```fides
pub fn site_css() -> str {
```

**Returns:** `str`

*Defined at line 799*

---

## site_respond

The response to `method` on raw request path `path` (no query).

```fides
pub fn site_respond(method: str, path: str) -> str {
```

**Parameters:**

- `method`: `str`
- `path`: `str`

**Returns:** `str`

*Defined at line 843*

---

## site_build

Write every page, the stylesheet, the logo, a 404.html and .nojekyll to
`out` (GitHub Pages serves such a directory as it is). Files written.

```fides
pub fn site_build(out: str) -> i64 {
```

**Parameters:**

- `out`: `str`

**Returns:** `i64`

*Defined at line 891*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
