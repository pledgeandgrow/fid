# Parallel front-end checks (`FID_PAR`)

`FID_PAR=N` (2 to 16; unset, 1 or anything else: off) runs three passes of
the front end in up to N threads. It is opt-in (roadmap CS5) and changes
no output: the messages are the sequential ones, in the same order, byte for
byte.

| Pass | Split | Module |
|------|-------|--------|
| Borrow check | every function body | `compiler/borrowck.fd` `check`, `bc_par_worker` |
| Escape (reference) check | the reporting pass, after the summary fixpoint | `compiler/escape.fd` `esc_par_report`, `esc_par_worker` |
| `@secret`/`@sensitive` check | the reporting pass, after the summary fixpoint | `compiler/secrecy.fd` `sec_par_report`, `sec_par_worker` |

A pass goes parallel only when it has at least 64 functions to check
(the borrow check also only in its default `on` mode: warnings stay
sequential).

## Why the output is the same

* Each pass checks a contiguous chunk of the functions per worker
  (`par_chunk_bounds`), and the chunks' messages are joined in order.
* The escape and `@secret` checks first find, sequentially, what each
  function's result borrows or leaks (the fixpoint). The reporting pass
  reads those summaries and writes none, so a function's messages do not
  depend on which functions were checked before it. (Before CS5 the
  escape check's reporting pass recorded the summary of a function whose
  result holds no reference as it went, so a call checked after such a
  function could see a non-empty summary the fixpoint never used; the
  summary of such a function is now always the empty one the fixpoint
  gives it.)
* A message already given is not given again (`esc_error`, `sec_error`);
  joining the chunks applies the same rule.

## Why the threads share nothing they change

Owner counts of strings, Vecs and Boxes are not atomic (core.fd, "Copies
for another thread"), so two threads must never copy or drop the same one
at once. Keeping a value (a `let`, a field, an element, a return) copies
it; reading it or passing it as an argument does not. So:

* each worker makes its own checker and copies the pass's tables into
  buffers of its own (`par_own_*` in escape.fd), only reading the
  master's; the HashMaps are raw handles that are only looked up;
* no analysis keeps another function's `Func`: a call reads what it needs
  of its callee from tables (`param_names`, `param_rt`, `param_has_ref`,
  `param_tys`, `fn_anns`), and a worker passes its units to the analysis
  without keeping them;
* a function's AST is touched by the one worker that checks it. Before
  these passes, AST pieces are shared between functions only inside one
  impl block (the type name of its `self` parameters) and among trait
  impls (every impl that lacks a method gets the same default body), so a
  chunk never splits an impl block, and all trait impls' methods are one
  chunk.

## Checking it

```bash
FID=release/fid.exe bash tests/par/par_check.sh          # outputs equal
FID=release/fid.exe TIME=1 bash tests/par/par_check.sh   # + check compiler/main.fd --time
```

`tests/selfhost/test_par_escape.fd` runs the three checks on a parsed
program of 271 functions with errors of each kind;
`tests/selfhost/test_par_chunks.fd` checks the chunking.

## Not parallel

The type check of function bodies and import resolution stay sequential:
the type checker changes shared state for every body (its tables, the
inferred types, generic instances, RT6 records), and parsing a module
depends on the struct and enum names of the modules parsed before it and
on parser globals (`take_qualified_calls`, turbofish numbering,
`parser_saw_secret`).
