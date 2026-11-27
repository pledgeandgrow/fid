[All modules](index.md)

# compiler::cparser -- Fides Documentation

## Contents

- [CHeaderState](#CHeaderState) (struct)
- [CHeaderState_new](#CHeaderState_new) (function)
- [cparse_header_file](#cparse_header_file) (function)
- [cparse_header_file_with](#cparse_header_file_with) (function)
- [cparse_header_file_for](#cparse_header_file_for) (function)
- [cparse_dll_exports](#cparse_dll_exports) (function)
- [cparse_system_dll_of](#cparse_system_dll_of) (function)
- [CpEval](#CpEval) (struct)
- [CpExpr](#CpExpr) (struct)
- [CpxTy](#CpxTy) (struct)

## CHeaderState

The state of one parse: macros, headers read, the code, the types seen.

```fides
pub struct CHeaderState {
```

**Fields:**

| Name | Type |
|------|------|
| `macro_index` | `i64` |
| `macro_names` | `Vec<str>` |
| `macro_bodies` | `Vec<str>` |
| `macro_params` | `Vec<str>` |
| `macro_noise` | `Vec<i64>` |
| `included` | `Vec<str>` |
| `code` | `StringBuilder` |
| `toks` | `Vec<str>` |
| `kinds` | `Vec<i64>` |
| `td_index` | `i64` |
| `td_names` | `Vec<str>` |
| `td_base` | `Vec<str>` |
| `td_ptr` | `Vec<i64>` |
| `const_index` | `i64` |
| `const_names` | `Vec<str>` |
| `const_values` | `Vec<i64>` |
| `fn_index` | `i64` |
| `fn_lines` | `Vec<str>` |
| `skipped` | `Vec<str>` |
| `cpp` | `bool` |
| `tok_ns` | `Vec<str>` |
| `tok_c` | `Vec<bool>` |
| `inc_dirs` | `Vec<str>` |
| `inc_top` | `str` |
| `inc_depth` | `i64` |
| `inc_notes` | `Vec<str>` |
| `raw` | `Vec<str>` |
| `rawk` | `Vec<i64>` |
| `tok_raw` | `Vec<i64>` |
| `tok_sys` | `Vec<bool>` |
| `tok_nsq` | `Vec<str>` |
| `cx_lines` | `Vec<str>` |
| `cx_code` | `Vec<str>` |
| `cx_names` | `i64` |
| `cx_class_q` | `Vec<str>` |
| `cx_class_s` | `Vec<str>` |
| `cx_notes` | `Vec<str>` |
| `cx_tmpl_prefix` | `Vec<str>` |
| `cx_tmpl_q` | `Vec<str>` |
| `cx_tmpl_params` | `Vec<str>` |
| `cx_tmpl_at` | `Vec<i64>` |
| `cx_tmpl_len` | `Vec<i64>` |
| `cx_tt` | `Vec<str>` |
| `cx_tk` | `Vec<i64>` |

*Defined at line 40*

---

## CHeaderState_new

```fides
pub fn CHeaderState_new() -> CHeaderState {
```

**Returns:** `CHeaderState`

*Defined at line 113*

---

## cparse_header_file

Bindings for the header at `path` (and the headers it includes), as
Fides source. `link` is the library for the `extern` block; `module_name`
becomes the `module` line ("" for none).

```fides
pub fn cparse_header_file(path: str, link: str, module_name: str) -> str {
```

**Parameters:**

- `path`: `str`
- `link`: `str`
- `module_name`: `str`

**Returns:** `str`

*Defined at line 124*

---

## cparse_header_file_with

LG15: as cparse_header_file; `include` ("<stdio.h>", or "" for none)
becomes the extern block's `include`, so the C code includes the header
and calls its functions as it declares them (no prototype of ours).

```fides
pub fn cparse_header_file_with(path: str, link: str, module_name: str, include: str) -> str {
```

**Parameters:**

- `path`: `str`
- `link`: `str`
- `module_name`: `str`
- `include`: `str`

**Returns:** `str`

*Defined at line 131*

---

## cparse_header_file_for

LG16: as cparse_header_file_with; `program` is the source of the program
that imports the header ("" for none). A C++ header's function template
is instantiated for each name of the form `<ns>_<name>_<types>` the
program uses (`shapes_biggest_f64` is `shapes::biggest<double>`), besides
the instantiations the header names (`template int biggest<int>(int,
int);` is `shapes_biggest_i32`).

```fides
pub fn cparse_header_file_for(path: str, link: str, module_name: str, include: str, program: str) -> str {
```

**Parameters:**

- `path`: `str`
- `link`: `str`
- `module_name`: `str`
- `include`: `str`
- `program`: `str`

**Returns:** `str`

*Defined at line 141*

---

## cparse_dll_exports

The names the DLL (any PE image) at `path` exports, from its export
directory; none when the file is missing or not a PE image.

```fides
pub fn cparse_dll_exports(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 544*

---

## cparse_system_dll_of

LG15: the system DLL a native Windows program imports the C function
`c_name` from when its declaration names no library (a header's
functions): "msvcrt.dll" or "kernel32.dll", whichever exports it first,
"" when neither does. When neither DLL can be read (a build on another
system), "msvcrt.dll": the C library, which the program's start-up
loader checks (a missing function is reported when it is called).

```fides
pub fn cparse_system_dll_of(c_name: str) -> str {
```

**Parameters:**

- `c_name`: `str`

**Returns:** `str`

*Defined at line 605*

---

## CpEval

```fides
pub struct CpEval {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `value` | `i64` |
| `uns` | `bool` |
| `wide` | `bool` |

*Defined at line 2027*

---

## CpExpr

A parse position over a token list.

BG213: the value each cp_ex_* function returns has a C integer type,
left in `uns`/`wide` by the call: int (32 bits), unsigned int, long long
or unsigned long long (`long` is one of them by the system's ABI). The
operands of an operator are converted to their common type (C's usual
arithmetic conversions) and the result is that type's value: `-1u / 2`
is 2147483647, `~0u` 4294967295. An #if (`pp`) computes in 64 bits, as
C's preprocessor does (intmax_t, uintmax_t). `skip` > 0 while an operand
is not evaluated (the right of `0 &&`, the arm `?:` does not take): there
an unknown name or a division by zero is not an error.

```fides
pub struct CpExpr {
```

**Fields:**

| Name | Type |
|------|------|
| `toks` | `Vec<str>` |
| `kinds` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |
| `depth` | `i64` |
| `uns` | `bool` |
| `wide` | `bool` |
| `pp` | `bool` |
| `skip` | `i64` |

*Defined at line 2046*

---

## CpxTy

How a C++ type crosses the shim: `why` is "" when it can; `fides` is the
Fides type, `cty` the shim's C type, `cls` the class (CPX_OBJ*).

```fides
pub struct CpxTy {
```

**Fields:**

| Name | Type |
|------|------|
| `why` | `str` |
| `kind` | `i64` |
| `fides` | `str` |
| `cty` | `str` |
| `cls` | `str` |

*Defined at line 2946*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
