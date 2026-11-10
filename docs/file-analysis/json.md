# json.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> The number-validation bug is still open; it now keeps a prefix instead of returning 0.

- **Path:** `compiler/json.fd`
- **LOC:** 666 (Rust `src/json.rs`: 289)
- **Role:** leaf JSON module: `Json` enum (Null, Bool, Num(f64), Str, Arr, Obj), `JsonObject`
  (insertion-ordered keys, sorted on dump), `json_parse`, `json_dump` (compact / pretty),
  `json_escape_string`.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/lsp.fd` | whole file | JSON-RPC parse and responses (`fid lsp`) |
| `compiler/interp.fd` | :5807, :6378-6430, :6683 | the `p_json_*` builtins under `fid run` |
| `compiler/main.fd` | :2488, :2495 `cmd_qualify` | `json_escape_string` for `fid qualify` output |
| `tests/selfhost/test_json.fd`, `test_json_min2.fd` | tests |

Note: compiled programs (`ccompile`, C backend) use a separate C JSON parser written by
`codegen_c.fd` (:7849-7860), not this module. The two can disagree; not compared.

## Findings

### Verified live

- **MEDIUM — CHANGED — malformed numbers are accepted.** `p_json_parse("[1e,-,5e+,0]")` under
  `fid run` gives `[1,0,5,0]`. `parse_number` (:285-360) scans digits, then `str_parse_float`
  now parses the longest prefix (`1e` → 1, `5e+` → 5). For a 0 result the fallback at
  :344-351 accepts any text starting with a digit or `-` (`-` → 0). Rust rejects all three.
- **LOW — STILL TRUE — `\uXXXX` surrogate pairs are not combined.** `"😀"` is written as
  two 3-byte sequences (`ED A0 BD ED B8 80`, CESU-8) instead of one 4-byte emoji
  (`append_utf8`, :532-547).
- **OK — `\b`, `\f`, nested objects/arrays, sorted keys**: `{"b":1,"a":[1,2.5,{"c":null}]}` dumps
  as `{"a":[1,2.5,{"c":null}],"b":1}`.

### From code inspection

- **LOW — STILL TRUE — no recursion limit** in `parse_value` / `parse_array` / `parse_object`
  (:158-470). Deep nesting overflows the stack (`fid lsp` would crash on such a message).
- **LOW — `use string` unused.** `fid lint compiler/json.fd`: `unused import: 'string'`.
- **INFO — key sort is insertion sort** (`sort_keys`, :645): O(n²), fine for LSP payloads.

## Fix order

1. Validate the number grammar while scanning: at least one integer digit, at least one digit
   after `.` and after `e`/`E`. Return `Invalid number` otherwise. Remove the `is_valid_zero` guess.
2. Combine surrogate pairs in the `\u` escape.
3. Add a depth limit.

## Verification state

- `fid check compiler/json.fd`: no errors.
- Live probe through the interpreter builtins (`p_json_parse`) with a scratch file.
