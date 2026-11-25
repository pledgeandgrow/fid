# Diagnostic Codes

Every `fidesc` error carries a stable `Exxx` code. This index maps each code to
its meaning and the usual fix. `fid` prints the code of each error it can
classify (compiler/compiler.fd `classify_error` and `errcode_number`, applied
in `enrich_errors`) as `error[E044]: ...`; an error no rule classifies stays
plain `error: ...` (E000). Its diagnostics module, compiler/diag.fd, is still
not wired in (known-bugs 2g.22). Codes marked *fid* below are given only by
`fid`.

Each code `fid` gives has an entry in [error-codes.md](error-codes.md): its
meaning, a program that gets it, the fix and the test that checks it;
`fid explain-error E044` prints the entry. Every `fid` error also carries
a `Fix:` (or `Hint:`) line and, rendered by `enrich_errors`, a
`--> file:line:col` place with a caret (TL2).

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
| E058 | Reference escape (`fid`: "Reference error", compiler/escape.fd) | References are second-class — don't store or return them |
| E059 | Secrecy violation (`fid`: "Security error (@secret)" / "(@sensitive)", compiler/secrecy.fd) | `@secret`/`@sensitive` data cannot reach an output sink |
| E060 | Duplicate definition (*fid*) | Rename one, or import only one module that defines it |
| E061 | Taint violation (*fid*) | Pass the `@tainted` value through a sanitizer before a sink or a return |
| E062 | Non-exhaustive match (*fid*) | Add the missing variants or a `_` arm |
| E078 | Other check (*fid*) | An error of one of the other checks: `Event error`, `Redundancy error`, `Contract error`, `Thread safety error`, `Partition error`, `Transaction error`, `Serverless error`, `Differential privacy error`, `Comptime error`, `Shard error`, `Quantization error`, `Linear type error`, `Checkpoint error`, `Budget error`, `Visibility error`, `Privacy error`, `Unknown annotation`, `Misplaced annotation`, `Differentiability error`, `DP error`, `Adapter error`, `Hermetic error`, `Allocator error`, `KV cache region error`, `Injection error`, and inline assembly naming no 64-bit register (`asm! in f: ...`); the message carries the details and the fix (CE4) |
| E079 | Other type error (*fid*) | The message carries the details |

## Borrow checker (E080–E089)

| Code | Meaning | Fix |
|---|---|---|
| E080 | Mutate under active borrow | Use the borrow for the last time first, or scope it in `{ }` |
| E081 | Conflicting mutable borrows | Only one `&mut` may be live at a time |
| E082 | Data race (*fid*) | Guard the global with a lock, or keep it on one thread |
| E083 | Lock order (*fid*) | Take the locks in one order everywhere (CA4) |
| E084 | Task never awaited, joined or detached (*fid*) | A `Concurrency error`: await the handle, start the task in `spawn scope { }`, or write `spawn detached f(x)` (CA6, docs/reference/structured-concurrency.md) |

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
| E097 | Safety annotation violated (*fid*) | A `Safety error` / `Determinism error`: see docs/reference/safety-rules.md |
| E098 | `@constant_time` violation (*fid*) | No secret-dependent branch, early return, index or effect |
| E099 | `@realtime` violation (*fid*) | No blocking effect, `while` loop or recursion |

## Effects (E110–E119)

| Code | Meaning | Fix |
|---|---|---|
| E110 | Declared effects don't cover inferred effects | Widen the annotation, e.g. `!IO` → `!{IO, Net}`, or declare `!E` for a polymorphic row |

## Misc (E120+)

| Code | Meaning | Fix |
|---|---|---|
| E120 | No `main` function | Add `fn main()` |
| E999 | Internal compiler error (`fid`: the IR verifier, the C split's units) | Report a bug (`fid reduce` shrinks the program) |
| E000 | Uncategorized | Message carries the details |
