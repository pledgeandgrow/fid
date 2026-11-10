# fmt.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> Unchanged since Oct 22 (no commits). All three corruption bugs reproduce; they also spread
> across function boundaries.

- **Path:** `compiler/fmt.fd`
- **LOC:** 1,179 (Rust `src/fmt.rs`: 1,236)
- **Role:** token-based (not AST) formatter: `fmt_tokenize` → `need_space` → `format_source`.
  Keeps comments and blank lines, 4-space indent from brace depth.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/main.fd` | :165 dispatch, :638 `cmd_fmt` | `fid fmt <file> [--check]` |
| `tests/selfhost/test_fmt*.fd` (6 files) | tests |

`cmd_fmt` writes the result **over the input file** (`main.fd:669`) unless `--check`.
`is_formatted` (:1177) has no caller; `cmd_fmt` compares strings itself.

## Findings

### Verified live

Input (scratch copy) and the file `fid fmt` wrote back:

```
fn f(a: i64, b: i64) -> i64 { if a < b { return 1; } return 0; }
fn g(x: &i64) -> i64 { var y: i64 = -5; return *x + y; }
--->
    if a<b{
        return1;
    ...
fng(x: &i64)->i64{
    vary: i64=-5;
    return*x+y;
```

- **CRITICAL — STILL TRUE — `a < b` between two words opens a "generic" that never closes.**
  `is_generic_open` (:651-669) returns true for word `<` word. `generic_depth` (:857) then stays
  above 0 and the override at :1004 removes spaces from every later token. It is never reset at
  a function boundary, so the rest of the **file** collapses: `return1`, `fng`, `vary`, `ifn`.
  The output does not compile, and it replaced the input.
- **CRITICAL — STILL TRUE — `>>` never closes generics.** `Vec<Vec<i64>>` leaves depth 2
  (the close check at :981-991 only sees a single `>`). Same collapse.
- **HIGH — STILL TRUE — prefix operators get spaces or lose them wrongly.** `is_binop` (:420) is
  checked at :734-735 before the prefix rules, so `&x`, `*x`, `-5` are treated as binary. In
  the collapsed output this shows as `=-5`, `return*x`; in a clean file as `& y`, `- 5`.

### From code inspection

- **LOW — STILL TRUE — `..` is not a multi-char punct** (`try_multi_punct`, :50); `-` after `??`,
  `|>`, `~>` and compound assignments is not seen as prefix.
- **LOW — `is_cond_kw` is unused** (`fid lint`), `use string` unused.

## Fix order

1. Stop writing the file when the formatted text does not lex to the same tokens as the input
   (cheap safety net: compare `fmt_tokenize` of both, ignoring whitespace).
2. Reset `generic_depth` at `;`, `{`, `}`; treat `>>` as two closes.
3. Decide prefix vs binary from the previous significant token before the `is_binop` rule.
4. Longer term: format from the AST.

## Verification state

- `fid check compiler/fmt.fd`: no errors.
- Live: `fid fmt` on a scratch file (copy kept as `fm_orig.fd` in the session scratch folder).
- `fid lint compiler/fmt.fd`: `unused function 'is_cond_kw'`, `unused import: 'string'`.
