[All modules](index.md)

# path -- Fides Documentation

## Contents

- [PATH_SEP:](#PATH_SEP:) (const)
- [path_sep](#path_sep) (function)
- [path_root](#path_root) (function)
- [path_join](#path_join) (function)
- [path_join_many](#path_join_many) (function)
- [path_split](#path_split) (function)
- [path_dirname](#path_dirname) (function)
- [path_basename](#path_basename) (function)
- [path_extension](#path_extension) (function)
- [path_stem](#path_stem) (function)
- [path_set_extension](#path_set_extension) (function)
- [path_normalize](#path_normalize) (function)
- [path_is_absolute](#path_is_absolute) (function)
- [path_is_relative](#path_is_relative) (function)
- [path_has_extension](#path_has_extension) (function)
- [path_components](#path_components) (function)
- [path_parent](#path_parent) (function)
- [path_depth](#path_depth) (function)

## PATH_SEP:

Path separator (always '/' for portability).

```fides
pub const PATH_SEP: str = "/";
```

*Defined at line 21*

---

## path_sep

Get the path separator character.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_sep(), "/");
```

```fides
pub fn path_sep() -> str {
```

**Returns:** `str`

*Defined at line 26*

---

## path_root

The root of a path: "/", "C:/", "C:", "//" (UNC), or "" (relative).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_root("/usr/bin"), "/");
assert_eq(path_root("C:\\Users"), "C:/");
assert_eq(path_root("docs/a.md"), "");
```

```fides
pub fn path_root(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 63*

---

## path_join

Join two path components with a separator.
path_join("foo", "bar") → "foo/bar"
path_join("foo/", "bar") → "foo/bar"
path_join("foo", "/bar") → "foo/bar"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = path_join("foo/", "bar");
println(p);
assert_eq(p, "foo/bar");
assert_eq(path_join("foo", "/bar"), "foo/bar");
```

```fides
pub fn path_join(a: str, b: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `str`

*Defined at line 93*

---

## path_join_many

Join multiple path components.
path_join_many(["a", "b", "c"]) → "a/b/c"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var parts: Vec<str> = Vec::new();
parts.push("a"); parts.push("b"); parts.push("c.txt");
assert_eq(path_join_many(parts), "a/b/c.txt");
```

```fides
pub fn path_join_many(parts: Vec<str>) -> str {
```

**Parameters:**

- `parts`: `Vec<str>`

**Returns:** `str`

*Defined at line 122*

---

## path_split

Split a path into (dirname, basename).
path_split("/a/b/c.txt") → ("/a/b", "c.txt")


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = path_split("/a/b/c.txt");
assert_eq(s.get(0), "/a/b");
assert_eq(s.get(1), "c.txt");
```

```fides
pub fn path_split(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 144*

---

## path_dirname

Get the directory name of a path.
path_dirname("/a/b/c.txt") → "/a/b"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_dirname("/a/b/c.txt"), "/a/b");
```

```fides
pub fn path_dirname(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 167*

---

## path_basename

Get the base name (file name) of a path.
path_basename("/a/b/c.txt") → "c.txt"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_basename("/a/b/c.txt"), "c.txt");
assert_eq(path_basename("C:\\dir\\x.fd"), "x.fd");
```

```fides
pub fn path_basename(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 192*

---

## path_extension

Get the file extension (including the dot).
path_extension("file.txt") → ".txt"
path_extension("file") → ""
path_extension("file.tar.gz") → ".gz"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_extension("file.tar.gz"), ".gz");
assert_eq(path_extension("Makefile"), "");
```

```fides
pub fn path_extension(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 214*

---

## path_stem

Get the file name without extension.
path_stem("file.txt") → "file"
path_stem("file.tar.gz") → "file.tar"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_stem("dir/file.tar.gz"), "file.tar");
```

```fides
pub fn path_stem(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 230*

---

## path_set_extension

Change the extension of a path.
path_set_extension("file.txt", ".md") → "file.md"
path_set_extension("file", ".txt") → "file.txt"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_set_extension("notes.txt", ".md"), "notes.md");
assert_eq(path_set_extension("notes", ".txt"), "notes.txt");
```

```fides
pub fn path_set_extension(path: str, ext: str) -> str {
```

**Parameters:**

- `path`: `str`
- `ext`: `str`

**Returns:** `str`

*Defined at line 246*

---

## path_normalize

Normalize a path by resolving "." and ".." components.
path_normalize("a/./b/../c") → "a/c"
path_normalize("/a/b/../c") → "/a/c"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_normalize("a/./b/../c"), "a/c");
assert_eq(path_normalize("/a/b/../../c"), "/c");
```

```fides
pub fn path_normalize(path0: str) -> str {
```

**Parameters:**

- `path0`: `str`

**Returns:** `str`

*Defined at line 265*

---

## path_is_absolute

Check if a path is absolute.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(path_is_absolute("/etc"));
assert(path_is_absolute("C:/Windows"));
assert(!path_is_absolute("src/main.fd"));
```

```fides
pub fn path_is_absolute(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 337*

---

## path_is_relative

Check if a path is relative.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(path_is_relative("src/main.fd"));
```

```fides
pub fn path_is_relative(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 353*

---

## path_has_extension

Check if a path has the given extension.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(path_has_extension("main.fd", ".fd"));
assert(!path_has_extension("main.fd", ".rs"));
```

```fides
pub fn path_has_extension(path: str, ext: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `ext`: `str`

**Returns:** `bool`

*Defined at line 362*

---

## path_components

Split a path into its components.
path_components("/a/b/c") → ["", "a", "b", "c"]


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = path_components("a/b/c");
assert_eq(c.len(), 3);
assert_eq(c.get(2), "c");
```

```fides
pub fn path_components(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 376*

---

## path_parent

Get the parent directory (same as dirname but handles trailing slashes).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_parent("/a/b/"), "/a");
```

```fides
pub fn path_parent(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 384*

---

## path_depth

Get the number of path components.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(path_depth("a/b/../c/d"), 3);
```

```fides
pub fn path_depth(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 396*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
