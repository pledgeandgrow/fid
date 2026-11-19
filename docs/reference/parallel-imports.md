# Imports lexed in parallel, and the module graph (SP9)

Roadmap SP9 (llvm-roadmap, "Front end in parallel per module where modules
do not depend on each other"). This page says what runs in parallel today,
why the output cannot change, what was measured, and what is left.

## What it does

With `FID_PAR=N` (2 to 16, as for the parallel checks in
[parallel-checks.md](parallel-checks.md)), the import phase first finds the
import closure of the program and lexes its files, a breadth-first wave at
a time, each wave in up to N threads. The import loop that follows is the
sequential one: it parses the modules one by one, in the same order and
with the same struct and enum names as without `FID_PAR`, and finds their
tokens already in its token cache.

| Piece | Where |
|-------|-------|
| Lex texts in N threads, same tokens and messages as `Lexer::new(t).tokenize()` | `compiler/lexer.fd` `lex_texts_par`, `lex_par_worker`, `lex_text_alone` |
| Spread the texts over the workers (largest first, to the least loaded) | `lexer.fd` `lex_par_plan` |
| A module's imports from its tokens, without a parse | `lexer.fd` `token_use_paths` |
| The module graph and its levels | `lexer.fd` `ModGraph`, `modgraph_levels`, `modgraph_summary` |
| The prefetch in the import phase (driver) | `compiler/compiler.fd` `imp_prefetch_lex` |

The prefetch runs only when the import cache is not in use (it is on in the
first build of a one-shot command unless `FID_IMPCACHE=off`; a cache hit
neither lexes nor parses, so lexing ahead would only add work) and not in a
long-lived process with the module cache (`fid serve`, `fid fix`, the LSP).
With `--time` it adds a row just before the `imports` row, a part of that
phase (its form; the numbers are those of the program):

```
[time]   imports: lexed ahead, <n> modules, <l> levels, widest <w>: <ms> ms
```

## Why the output is the same

* Lexing a file depends on its text alone. The one piece of shared state
  the lexer had, the cache of words that are no keyword (`g_lex_not_kw`,
  CS4), is replaced in a worker's lexer by a map of its own
  (`Lexer.own_kw`, `keyword_or_ident_own`); the cache only remembers the
  answer of a pure function, so it gives the same tokens.
* Only a file that lexed without an error is put in the token cache. A file
  with a lex error is left to the import loop, which lexes it again when it
  reaches it and reports the error exactly where it did before.
* The import loop, its order, its deferrals, the names it gives the parser
  and every message are unchanged; a file the prefetch lexed but the loop
  never reaches (a `use` it drops) is simply not used. The prefetch records
  nothing for incremental builds (`inc_record_*`): only the loop does.

## Why the threads share nothing they change

As for the parallel checks (owner counts are not atomic): a worker gets
its number as its only spawn argument, reads its list of texts from
`g_lex_par_plan` and the texts from `g_lex_par_texts` (set before the
spawns, reset after the last await, never changed while the workers run),
lexes a copy of each text in a buffer of its own (`lex_own_str`), and
returns its tokens; the caller puts the results back in text order.

## The module graph

`token_use_paths` reads the `use` paths at brace depth 0 from a file's
tokens, as `parse_use` names them (`a::b` for `use a::b;`, `use a::b as c;`,
`use a::b::{x, y};`, `use a::b::*;`). The prefetch builds the graph of the
closure from them (`ModGraph`: names, files, the indexes each imports;
`core` left out as the import loop skips it). `modgraph_levels` gives each
module its level: 0 when it imports none of the others, else one more than
the highest level of those it imports; modules of one level do not depend
on each other, so they are the units a per-module front end can work on
together. A module on an import cycle, or behind one, gets -1.

## Measured

`tests/par/lex_par_bench.fd` lexes the 80 `.fd` files of `compiler/`
(12.4 MB, 2.2 million tokens) with `lex_texts_par` (built with ccompile,
`zig cc`, Windows, Nov 17, a machine shared with other builds):

| Threads | 1 | 2 | 4 | 8 |
|---------|---|---|---|---|
| ms (run 1) | 736 | 434 | 385 | 248 |
| ms (run 2) | 747 | 400 | 325 | 219 |

The same tokens in every case. The gain is bounded by the largest file
(`typechecker.fd`, about a fifth of the bytes, is lexed by one thread).
Lexing is a small part of the import phase: the rest is the parse, which
stays sequential (below). The import rows of `check compiler/main.fd
--time` with and without `FID_PAR=4` are the measure to take on the built
compiler (`SELF=1 bash tests/par/imports_par_check.sh`).

## Not parallel yet (what SP9 still needs)

* **Parsing.** A module's parse depends on the struct and enum names of the
  modules parsed before it in the loop's order (not only those it imports),
  and on parser globals: the `module::name` calls (`take_qualified_calls`,
  read in parse order by `cc_qualify_shadowed_imports`), turbofish
  numbering (`turbofish_all`, numbered across modules), `parser_saw_secret`,
  the process-wide import set, `g_pd_impl_selfs`. To parse modules of one
  level together, the names each module would have been given must be
  known first (a token scan of `struct`/`enum` at depth 0 gives them, and a
  parse can check them afterwards), and each of those globals must become
  per-parser state merged in the loop's order.
* **Type check.** It runs on the merged program, not per module, and the
  body check writes shared state for every function: the inferred types,
  generic instances (CE9), the RT6 records, the effect tables and
  `self.errors` with TL2 placement. A per-module check needs those split
  into per-worker parts merged in function order, as the borrow and escape
  checks' workers do (their units are functions: parallel-checks.md).
* **Escape and `@secret`.** Their reporting passes already run in
  `FID_PAR` threads per function; the summary fixpoint before them is
  sequential. With the levels above, the summaries of one level could be
  computed together, since a module's summaries depend only on its own and
  those of lower levels (cycles excepted).
