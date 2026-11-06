# Diagnostic Codes

Every compiler error carries a stable `Exxx` code. This index maps each code to
its meaning and the usual fix.

## Lexer (E001–E019)

| Code | Meaning | Fix |
|---|---|---|
| E001 | Unterminated string literal | Close the `"` |
| E002 | Unterminated f-string | Close the `f"` |
| E003 | Unknown escape sequence | Use `\n`, `\t`, `\`, `\"`, `\xNN` |
| E004 | Unexpected character | Remove or quote the character |

## Parser (E020–E039)

| Code | Meaning | Fix |
|---|---|---|
| E020 | Expected token | Check the syntax — usually a missing `)`, `}`, or `,` |
| E021 | Expected identifier | Use a name where a keyword or literal appears |
| E022 | Expected type | Add a type after `:` or `->` |
| E023 | Invalid assignment target | Assign to a `var`, field, or `*ptr`, not a value |

## Type errors (E040–E079)

| Code | Meaning | Fix |
|---|---|---|
| E040 | Undefined variable | Declare with `val`/`var` or check the spelling (a suggestion is printed) |
| E041 | Undefined function | Import it, define it, or fix the name |
| E042 | Undefined struct | Define or import the struct |
| E043 | Undefined field | Check the struct's fields |
| E044 | Type mismatch | Convert explicitly (`x as i64`, `p_int_to_float`, ...) |
| E045 | `if`/`while` condition not `bool` | Compare or use `!= 0` |
| E046 | Argument count mismatch | Match the signature's parameter count |
| E047 | Missing return | Add `return` on every path of a non-void fn |
| E048 | Use before init | Initialize the `var` before reading it |
| E049 | Non-linear value used where linear required | Use `@linear` type or copy the value |
| E050 | Linear value already consumed | Linear values move once — don't reuse |
| E051 | Linear value captured in loop | Restructure so the linear value isn't re-used per iteration |
| E052 | Value escapes its region | Keep the use inside the `region` block or allocate outside |
| E053 | Use-after-free of region value | Move the access inside the `region` block |
| E054 | Region budget exceeded | Reduce allocations or raise `with budget(N)` |
| E055 | GPU region violation | GPU-region values can't leave the region or go to CPU memory |
| E056 | KV-cache region violation | KV-cache values stay inside the `region kv_cache` block |
| E057 | Misplaced annotation | `@annotation` only valid on the documented target (fn/struct/field) |
| E058 | Reference escape | References are second-class — don't store or return them |
| E059 | Secrecy violation | `@secret`/`@sensitive` data cannot reach an output sink |

## Borrow checker (E080–E089)

| Code | Meaning | Fix |
|---|---|---|
| E080 | Mutate under active borrow | Use the borrow for the last time first, or scope it in `{ }` |
| E081 | Conflicting mutable borrows | Only one `&mut` may be live at a time |

## Capabilities & safety (E090–E109)

| Code | Meaning | Fix |
|---|---|---|
| E090 | Capability denied | Grant it: `use mod with { IO }` |
| E091 | Sandbox violation | All capabilities are revoked in `sandbox {}`, granted or not — move the call out |
| E092 | Conservation violation | A `@conserve` function created money, used floats or effects, or called a non-`@conserve` function |
| E093 | Gas exhausted | Increase `gas(N)` budget or reduce work |
| E094 | `@crashfree` function may panic | Remove the fallible op or drop the annotation |
| E095 | Contract violation | A call with constant arguments makes a `given` precondition false (`ensure` is not checked) |
| E096 | `@medical` violation | Medical-mode code hit a forbidden operation |

## Effects (E110–E119)

| Code | Meaning | Fix |
|---|---|---|
| E110 | Declared effects don't cover inferred effects | Widen the annotation, e.g. `!IO` → `!{IO, Net}`, or declare `!E` for a polymorphic row |

## Misc (E120+)

| Code | Meaning | Fix |
|---|---|---|
| E120 | No `main` function | Add `fn main()` |
| E999 | Internal compiler error | Report a bug |
| E000 | Uncategorized | Message carries the details |
