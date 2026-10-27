# File Analysis: compiler/json.fd

> **Re-verified 2026-10-23:** confirmed — `is_valid_zero` fallback (:348-351) still accepts any digit/`-`-prefixed text → malformed numbers parse as 0.0.

**Run:** 27/52 | **LOC:** 666 (src/json.rs: 289, 2.3× — match-desugaring + helper shims) | **Status:** LIVE — LSP wire protocol

## Where it's called

`compiler/lsp.fd:22` — the only importer; `fidesc lsp` parses Content-Length-framed JSON-RPC with `json_parse`, reads with `json_get*`/`json_get_str`/`json_get_int` (helpers defined **in lsp.fd:175-197**, not here), responds via `Json::Obj` + `json_dump`. `runtime_linux.fd` mentions the module only in a comment.

## Verified live (fidesc run probe)

```
JsonObject::new + insert×3 → {"a":1,"b":2,"c":3}     ✓ (by-value `self` works:
parse {"x":1,"y":[1,2,3]}  → {"x":1,"y":[1,2,3]}       interp handle aliasing +
parse [1e, -, 5e+]         → [0,0,0]  ✗ WRONG        codegen lowers `self` as
                                                     void* into caller's struct)
```

## Findings

### MEDIUM — malformed numbers silently become `0.0` (verified live)

`parse_number` :332-358 — `str_parse_float(text)` returns `0.0` on failure; the `is_valid_zero` fallback then declares *any* text starting with a digit or `-` valid:

```fides
if (first_ch >= 48 && first_ch <= 57) || first_ch == 45 { is_valid_zero = true; }
```

So `-`, `1e`, `1e+`, `-.`, `12e-` all parse as `Json::Num(0.0)` instead of erroring. Rust uses `s.parse::<f64>()` which rejects all of these → `Err("Invalid number: …")` (json.rs:226-228). **Port regression** — the workaround for `str_parse_float`'s missing error signal turned malformed input into valid zeros. Fix: grammar-validate before converting (require ≥1 digit in the int part; if `e`/`E` consumed, require ≥1 exponent digit) — or reject when `text` isn't a canonical zero spelling *and* val==0.0.

### LOW — `\uXXXX` surrogate pairs not combined

`append_utf8` (:532-547) encodes any codepoint < 0x10000 as ≤3 UTF-8 bytes — `\uD83D\uDE00` emits two 3-byte CESU-8 encodings (technically invalid UTF-8) instead of the 4-byte combined emoji. Rust's version is *worse* — `char::from_u32` returns `None` for surrogates and it `push`es nothing, so the pair is **silently dropped** (json.rs:184-187). Different wrong answers; neither is conformant.

### LOW — unbounded recursion in `parse_value`

`parse_array`/`parse_object`/`parse_value` recurse with no depth cap — `[[[[…]]]]` × ~100k → stack overflow crash inside `fidesc lsp`. Inherited — Rust's `parse_value` is equally unguarded.

## Notable correctness wins (port is *better* than upstream)

- `\b` and `\f` escapes **handled** (:241-242) — Rust's escape match lacks both and errors "Invalid escape character". The port fixed an upstream gap.
- Sorted-key dump = BTreeMap parity (insertion sort — O(n²), fine for LSP payloads).
- `format_number` equivalent to Rust's `fract()==0 && |n|<1e15 → i64` rule.
- Byte-wise string parsing preserves UTF-8 correctly (passthrough append).
- `insert` updates-in-place for existing keys — matches `BTreeMap::insert` overwrite semantics.

## Verdict

The cleanest module audited so far — one real bug (number validation hole, verified live producing `[0,0,0]` from `[1e,-,5e+]`) plus two shared conformance gaps. The `is_valid_zero` heuristic should be replaced with grammar validation; everything else is serviceable as-is.
