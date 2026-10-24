# fmt.fd — source formatter (1,179 LOC)

> **Re-verified 2026-10-23:** corruption mechanism confirmed — `generic_depth` handling at :985-1015 still suppresses whitespace inside fake generics, `>>` never closes it, prefix-op spacing rules still dead. Exact output bytes not re-run.

## Role

Token-based (not AST) formatter: `fmt_tokenize` → `need_space` →
`format_source`, plus `is_formatted` for `--check`. Preserves comments
and blank lines; 4-space indent via brace tracking. Port of
`src/fmt.rs` (1,236 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `main.fd` | :23 import, :144 `cmd_fmt` | `fides-c fmt <file> [--check]` — **live**, user-facing. |

## Findings

### F1 — `word < word` poisons `generic_depth` → file collapses — CRITICAL (verified live, inherited)

`is_generic_open` (:651-668) treats `word < word` as a generic open —
it only excludes `is_cond_or_match_kw`/`is_type_word` as *prev*. Any
comparison `a < b` (both idents) opens a "generic" that never closes:

```
in :  if a < b { return 1; } return 0;
out:  ifa<b{return1;}return0;
```

`generic_depth` stays ≥1 forever → the `:1004` override forces
`need_space_before=false` for every later token except after `:`/`,`
→ **all whitespace in the rest of the function disappears** → `ifa`,
`return1`, `return0` — single identifiers. Output doesn't parse.

Verified live with `fidesc fmt` (the Rust binary — **inherited**:
`src/fmt.rs:513-532` has the identical heuristic + single-`>` close).

### F2 — `>>` never decrements `generic_depth` — CRITICAL (verified live, inherited)

`try_multi_punct` lexes `>>` as one token; `cur_is_generic_close`
(:981-991) only fires on single `>`. `Vec<Vec<i64>>` leaves
`generic_depth=2` permanently → everything after the type collapses:

```
in :  var x: Vec<Vec<i64>> = Vec::new(); if a < b { ...
out:  var x: Vec<Vec<i64>>=Vec::new(); ifa<b&&b>0{ ...
```

Same file-poisoning mechanism as F1 — combined, they make `fmt`
unsafe on **any** file using generics or `<` comparisons, i.e.
virtually all real code. `--check` (`is_formatted`) then reports
canonical source as "unformatted" — CI usage fails both ways.

### F3 — All prefix-operator "no space" rules are dead code — HIGH (verified live, inherited)

`is_binop` (:436) contains `&`, `*`, `-`, `!`… and the check at
:734-735 (`is_binop(cur)||is_binop(prev)`) returns `true` **before**
the prefix rules at :752-799 (`!x`, `~x`, `&x`, `*x`, `-5` contexts)
are reached. Verified live:

```
in :  foo(&y, &mut y);  var y: i64 = -5;  return *x + 1;
out:  foo(& y, & mut y);  var y: i64 = - 5;  return * x + 1;
```

Parses the same (whitespace-insensitive) but every borrow/deref/
negative is defaced — the exact output `--check` then demands.
Rust is identical (:1002 before :1040 — same dead rule, comment even
rationalizes it: *"we already returned true for binop above"* —
the reasoning is wrong: `foo(&x)` hits the binop arm too).

### F4 — Additional gaps — LOW/MEDIUM

- `try_multi_punct` lacks `..` — `0..5` survives only because two
  `.` tokens also get no space. `0..-1` → `0..- 1` (`-` prefix).
- Missing prefix contexts for `-`: `??`, `|>`, `~>`, compound
  assigns (`x += -1` → `x += - 1`).
- `is_generic_open` can't be fixed heuristically — needs lookahead
  to a matching `>`/`(` or an AST-aware pass.
- `str_starts_with(prev,"f")` at :811 — any `f*`-ident + `{` gets
  a space (intended: f-strings; redundant with :806).

## Verification state

- **All three findings reproduced live** on `fidesc fmt` (Rust build,
  same logic both sides — inherited, not port regressions).
- `fmt` on `if a < b` or `Vec<Vec<T>>` produces **non-compiling
  output** — the tool is unsafe to run on the codebase that ships it.
- Positive: comments/blank-lines/raw-strings/f-strings handled;
  simple programs are idempotent (`already formatted` verified).

## Next

`gpu.fd`.
