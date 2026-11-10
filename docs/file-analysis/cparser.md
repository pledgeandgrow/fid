# cparser.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> The module was rewritten (release E5): 523 → 1,163 lines, a token-based parser. Most Oct 22
> findings are fixed; one changed.

- **Path:** `compiler/cparser.fd`
- **LOC:** 1,163 (Rust `src/cparser.rs`: 315)
- **Role:** C header to Fides FFI bindings. Reads the header and the `#include "..."` headers in
  the same folder, collects `#define`s, tokenizes, splits declarations at `;`, and writes:
  `pub const` for integer macros and enum values, one `extern "C" link "lib" { ... }` block, and a
  comment listing the functions it could not express.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/main.fd` | :192 dispatch, :2737 `cmd_parse_header` → `cparse_header_file` (:2768) | `fid parse-header <file.h> [-o out.fd] [--link lib]` |

Only public entry: `cparse_header_file(path, link, module_name)` (:100).

## What the module does now

- Macro table with function-like macros; "noise" macros (expanding only to attributes such as
  `SQLITE_API`, `__declspec(..)`) are dropped (:363-395).
- Constant-expression evaluator for `#define` bodies and enum values: casts, shifts, bit ops,
  function-like macros (:853-1110).
- Typedef table with pointer depth; MSVC `__intN`; `<stdint.h>` names (`cp_builtin`, :657).
- Types: `char*` → `str`; `T*` for int/float `T` → `*T`; deeper pointers → `*i64`; other pointers
  and function pointers → `i64`; enums → `i32`; `long` → `i32`; `_Bool` → `bool`. Integer
  parameters are widened to `i64` (:833-835). Struct by value, `va_list`, `...` are skipped and
  listed.
- Function bodies (`static inline ...{ }`) are skipped.

## Findings

### Verified live (`fid parse-header` on a scratch header)

- **FIXED — `extern` declarations skipped.** `extern int t_add(int a, int b);` →
  `fn t_add(a: i64, b: i64) -> i32;`.
- **FIXED — `void*`.** Result and parameter both `i64`.
- **FIXED — array parameters.** `char *argv[]` → `argv: *i64`.
- **FIXED — `restrict` / `_Bool`.** `int restricted_count(int *restrict p)` keeps its name;
  `_Bool` → `bool`.
- **MEDIUM — CHANGED — `long` is always 32 bits.** It was `i64` (wrong on Windows); now `i32`
  everywhere (:681-682), stated in the header comment. That is wrong for the LP64 targets fid now
  builds for (`x86_64-linux`, `aarch64-linux`, `riscv64-linux`, macOS), where `long` is 64 bits.
  There is no `--target` input to `parse-header`.
- **LOW — `#if`/`#ifdef` are not evaluated; the first definition wins.** `#ifdef _WIN32
  #define T_PLAT 1 #else 2` gives `T_PLAT = 1` on every host. Documented in the file header.
- **LOW — string macros are dropped silently.** `#define T_STR "hello"` produces nothing and is
  not in the "not expressible" list.
- **OK — skipped list works:** `t_printf` (variadic), `t_byval` (struct result).

### From code inspection

- **LOW — a returned `const char*` becomes `str`.** Fides then treats a C-owned pointer as a
  Fides string. Whether the ownership rules (proposal 0004) try to free it is not checked here.
- **LOW — declarations it does not understand vanish without a note**, e.g. a function returning
  a function pointer (`cp_function` returns early at :743-745, nothing pushed to `skipped`).
- **INFO — `#include <...>` is ignored** (only `"..."` is followed, :210).

## Fix order

1. Add `--target` (or `--lp64`) so `long`/`unsigned long` map to 64 bits on LP64.
2. Put every dropped declaration and string macro in the "not expressible" list.
3. Optionally emit `pub const NAME: str` for string macros.

## Verification state

- `fid check compiler/cparser.fd`: no errors.
- Live: `fid parse-header` on a 30-line scratch header plus an included one (constants, enum,
  typedefs, 14 prototypes); output read in full.
- `fid lint` reports `undefined function 'str_join'` here while `check` passes; lint resolves
  imports differently and is not reliable for this file.
