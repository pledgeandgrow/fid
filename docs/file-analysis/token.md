# token.fd

- **Path:** `compiler/token.fd`
- **LOC:** 560 (Rust `src/token.rs`: 263 — the ~2× is manual `token_kind_tag`/`token_kind_eq`/`is_word_keyword`/`token_kind_to_str` functions replacing `#[derive(PartialEq)]` + `mem::discriminant`)
- **Role:** `TokenKind` enum + `Token` struct + helpers. The vocabulary every downstream phase speaks.

## Where called

| Caller | Mechanism |
|---|---|
| `compiler/lexer.fd` | Emits `Vec<Token>` — the module's raison d'être |
| `compiler/parser.fd` | `token_kind_eq`/`is_word_keyword`/`token_kind_to_str` on **every** token check (:341, :352, :4610, :4620, :4630…) |
| `compiler/comptime_eval.fd`, `compiler/compiler.fd`, `compiler/main.fd` | imports |

Live on every compile path — this is as upstream as code gets.

## Findings

### RESOLVED (2026-10-23) — `token_kind_tag` collision: `Spawn` and `Yield` both mapped to 25

**Fixed as suggested.** `token.fd:425-426` now reads:

```
TokenKind::Spawn => { return 25; }
TokenKind::Yield => { return 136; }   // was 25 — collided with Spawn
```

The original defect (for the record): `token_kind_eq` compared tags only →
`token_kind_eq(Spawn, Yield) == true`; parser checked `Spawn` first → `yield e`
silently parsed as `Expr::Spawn(e)` and the `Yield` arm was unreachable. Verified
at analysis time that 25 was the **only** collision (all tags uniq). `Expr::Yield`
can now actually be produced; re-check that the `Yield` parser arm (~:4620) is
reachable and behaves correctly end-to-end.

### MEDIUM — `Resume` variant missing (confirms parser run-37)

The only Rust variant absent from Fides. `resume` therefore can't even lex — it falls back to `Ident("resume")`, which is why `parser.fd` has no `Resume` arm. Lexer-level root cause of the `resume`/`spawn scope`/`cancel scope` gap.

### FD-only additions (not defects)

`Yield` (word-keyword — tag collision **fixed** 2026-10-23) and `DotDotDot` (`...` — lexed at `lexer.fd:610`) — forward-looking variants Rust lacks. `DotDotDot` is fully consistent (tag 105, `is_word_keyword` false, `to_str` "DotDotDot"); `Yield`'s wiring is now correct too.

### Verified faithful

- **Variant coverage**: 137 vs 136 — exhaustive diff; the only deltas are the three above.
- **`is_word_keyword`**: 62 `true` arms match Rust's 62 exactly (same set ± Yield/Resume).
- **`token_kind_tag`/`token_kind_eq`/`token_kind_to_str`**: exhaustive over all variants, `Eof`=135 terminal — matches `mem::discriminant` semantics (payload-ignoring).
- **`Token` struct + `token_new`**: parity.

## Suggested fix order

1. Renumber `Yield`'s tag to `136` (`token.fd:426`) — one-line fix that unbreaks `yield` parsing and revives the dead parser arm.
2. If `resume` is ever implemented (parser run-37 follow-up), `TokenKind::Resume` must be added first — it's the upstream blocker.
3. Consider asserting tag uniqueness in `token_kind_tag`'s unit tests — a collision like this is invisible until an expr-level diff.

## Verification

Inspection-only — no self-hosted binary. The collision is proven by the tag table itself (:425-426); the parser impact is proven by the dispatch order at `parser.fd:4610` vs `:4620` and `token_kind_eq`'s tag-only semantics (:558-559). Variant diff was computed mechanically over both enum bodies.
