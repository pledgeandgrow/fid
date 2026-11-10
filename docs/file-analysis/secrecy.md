# secrecy.fd — `@secret` / `@sensitive` information-flow check

> **First audit 2026-11-09** (no earlier doc; the 10-22 typechecker audit
> described the old `verify_secret`, which this module replaced). Whole-file
> read plus live probes (`s18.exe run`).

- **Path:** `compiler/secrecy.fd`
- **LOC:** 953 (Rust `src/secrecy.rs`: 774)
- **Role:** A secret never reaches an output. Sources: `@secret` parameters and locals, results of `@secret` functions. Sinks: output and logging calls, network and file writes, globals, writes through reference parameters, `spawn`, calls through function values. `@sensitive` is the same check one notch down, released by `privacy::release` or `@declassify`.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::parser` (`parser_saw_secret`), `compiler::escape` (`esc_idents_stmts`).

## Callers

| Caller | Site | What |
|--------|------|------|
| `compiler.fd` main pipeline | :1692-1701 | `secrecy_check(program)` after `escape_check`; errors fail the build |
| `compiler.fd` no-std path | :885-892 | same |
| `compiler.fd` `lint_source` | :1934 | same |

Reached from every command that builds a program, and from `lint`. Skipped
entirely unless the parser saw `@secret` or `@sensitive` anywhere
(`parser_saw_secret`, parser.fd:5422/:5708; :933 here) — only
`stdlib/privacy.fd` in the stdlib has one.

## What it does

- **Labels** are sorted `Vec<i64>`: -1 secret, -2 sensitive, `i >= 0` "parameter i" (:17-19). Per function, labels are flow-insensitive and only grow (`sec_assign` :289).
- **Per-function summaries** (`SecSummary` :32): which parameters the function outputs (`leaks`), which reach its result (`returns`), whether it outputs at all (`outputs`). Found to a fixpoint over all functions (:935-944), then a reporting pass (:945-951). Inside a function, up to 8 passes until labels stop changing (:413-422).
- **Implicit flows:** `if`, `while`, `for` and `match` push their condition's label as the context (`pc`, :305-317); an output under a secret context is an error (`sec_sink` :334); a function that outputs, called under one, too (:865-877).
- **Sinks:** the names in `sec_is_output_sink` (:114-122); a global (`sec_store` :517); a write through a reference parameter unless the function is `@secret`/`@declassify` (:521-527); `spawn` (:719-728); a call through a function value (:829-833); `.send(..)` on an unknown method (:912).
- **Results:** returning a secret needs `@secret` or `@declassify` on the function (`sec_ret` :353); `@declassify` clears the result label (:887).
- **Methods:** found by name over every `Type_method` suffix (:176-195); a method that is not read-only stores its arguments' labels into its receiver (:907-910).

## Findings

### Verified live

#### MEDIUM — a `match` guard on a secret does not taint the arm

`sec_arms` evaluates the guard (:492-495) but does not add its label to the
context of the arm body. Probe: `match 1 { _ if k > 3 => { print(1); } _ => {} }`
with `@secret k` compiles and prints 1 — one bit of `k` reaches the output.

#### MEDIUM — the sink list is hand-written and incomplete

`sec_is_output_sink` (:114-122) names 22 builtins. A call to any other
function outside the program returns its arguments' label and is not a sink
(:834-837). Probe: `p_file_write_vec(path, v)` with a secret pushed into `v`
compiles and writes the file. Also missing: `p_write*` (lower.fd:377 treats
`p_write` as output), other socket/HTTP/process builtins. The effect table in
typechecker.fd (`IO`/`Net` builtins) is the list to derive sinks from.

#### LOW — early return and loop exit under a secret are not tracked

A `return` or `break` inside a branch on a secret ends the context with the
branch (:603-612): what runs after it is not under the secret. Probe:
`if k > 3 { return; } print(1);` compiles; whether 1 is printed reveals
`k > 3`. `loop` pushes no context (:631). This is the usual termination /
early-exit channel; SR-13 does not mention it.

### Code inspection

#### LOW — labels are per name, not per variable

`var_index` maps a name to one label for the whole function (:278-303). Two
variables with the same name in different blocks share a label: a secret in
one taints the other (false positive, never a missed leak).

#### LOW — at most 8 passes per function

`sec_analyze_func` stops after 8 passes even if labels still change (:413-422).
A chain of more than 8 assignments around a loop back edge could leave a
label short in the summary pass; the outer fixpoint re-runs the function, so
the reporting pass likely converges, but nothing checks that it did.

#### LOW — `spawn` of a method call

`Expr::Spawn` adds argument labels only for `Expr::Call` tasks (:722-725).
For a method call it uses the call's result label, which for a known method
is its summary's `returns` (not every argument).

## Fix order

1. Add the guard's label to the arm's context in `sec_arms`.
2. Derive sinks from the `IO`/`Net` builtin effect table instead of a name list; treat an unknown builtin with a secret argument as a sink, or list the pure ones.
3. Decide on early exits (a `return`/`break` under a secret context taints what follows in the function) and document the choice in safety-rules.md.
4. Per-scope labels; check that the 8-pass cap is reached only on convergence.

## Verification

Tests: `tests/bootstrap/test_secret.fd`, `test_secret_*_fail.fd` (output,
global, branch, helper, return), `test_sensitive.fd`,
`test_sensitive_fail.fd`. Whole-file read. Live probes: `let t = k + 1;
print(t)` rejected (propagation works), `p_print_str(k)` rejected; guard,
early return and `p_file_write_vec` accepted.
