# Incremental checks in `fid serve` (CB2)

A long-lived `fid serve` (and an LSP process, which turns the module
cache on) checks a program again after an edit without checking every
function body again. Only the bodies the edit can reach are checked. The
code is in `compiler/compiler.fd`, section "CB2: incremental body checks in
`fid serve`". It is driven from `compiler/main.fd` `serve_request` and the
socket `check-text` request, and from `check_text_errors` when the module
cache is on.

## What is kept

After a check request that passes (type check, reference escapes, secrecy,
borrow check), the server keeps the modules of the program: the file of
each one, its text, and its top-level items. An item is a function, an
`impl` or trait-impl method, a declaration, or an import or module line or
`#[..]` attribute. Each item is kept with:

- a hash of its tokens without their positions, so adding a line above it
  leaves it unchanged;
- the names it defines: the function or method name, the name after
  `struct`, `enum`, `cap`, `type`, `var`, `const` and so on, and the variants
  of an enum;
- the words it mentions: its identifiers, and the words of its f-strings.

## What the next request checks

The next request compares each module with that clean state:

1. An item whose hash changed, a new item and a removed item are
   *changed*. The names they define go into a pool.
2. An item that mentions a pooled name is *affected*: a signature, a
   struct, or a fact about a callee (its effects, whether it allocates or
   reaches C code, an escape summary) may have changed what its check sees.
   The names it defines go into the pool too, and this repeats to a
   fixpoint, because those facts flow up the call graph. `main` counts only
   where it is called or passed (`main(`, `(main)`), because `main` is a
   common word in programs.
3. A changed import or module line, or a `#[..]` attribute, changes every
   item of its module.
4. Every body of the program is checked when any of these changed: the
   program's own import lines or attributes (its capabilities), the
   settings (core and runtime, the borrow-check mode, the `@cfg` target and
   features, the edition), or a module without a file (a C header).
5. When any module spawns threads and the program's module is registered
   as `main`, every body of that module is checked. The race check reads
   the global writes the checker records for `main` while it checks those
   bodies.

Every other body whose key (`fn:f`, `impl:T:m`, `trait:T:m`) the program
defines only once goes into the skip set. The checker, the escape and
secrecy passes and the borrow checker already take this skip set for core
(release D9). The passes over the whole program still run on every request:
effects, the `verify_*` passes, lock order, races, escape summaries and
lifetime ties. Only the per-body checks are skipped.

A check request ends after the borrow check. Comptime substitution and
monomorphization run in a build but report no errors, so they are skipped.
Lints are skipped too, because the answer holds errors only. The request's
note says what was checked:

```
main.fd: no errors (41 ms, 2 module parses reused, 2 of 7 bodies checked)
main.fd: 1 error(s) (39 ms, 2 module parses reused, 2 of 7 bodies checked)
main.fd: no errors (812 ms, all bodies checked: first check)
```

## Switches

- `FID_CB2=off`: every request is a full build, as before CB2.
- `FID_CB2_DEBUG=1`: the server prints the changed, removed and checked
  keys on stderr (`cb2: changed main.fd fn:other`).
- `FID_SERVE_TIME=1`: the server prints each check's stage times on
  stderr (`[time] typecheck: 12 ms`). The `cb2 plan` row is the comparison
  itself.

## Checking it

`tests/tooling/cb2-incremental-check.sh` (with `FID=<compiler>`):

- **Part A** runs a three-module program through twelve edits, once with
  `FID_CB2=off` and once with CB2 on. Each request must give the same
  status and errors in both runs. The notes must show what was checked
  again. The edits include a signature change and a struct field rename in
  `geo.fd`, which make the unchanged `calc.fd` and `main.fd` report
  errors, and a callee that starts printing, which makes an unchanged
  `sandbox` block report an IO effect.
- **Part B** times `compiler/main.fd` on a snapshot of `compiler/`: a first
  check, an unchanged check, a statement added in one function, and a
  type error there. It prints each request's time, the bodies it checked
  and the stage times. The CB2 target is under 50 ms from the edit to the
  diagnostics. The script reports whether the target is met;
  `STRICT_TIME=1` turns a miss into a failure.

## Limits

- The passes over the whole program still run on every request: imports
  and the merge, type registration, the effect and `verify_*` passes, and
  escape summaries. On a program as large as the compiler they are what
  is left of a request's time. Making them incremental needs state kept
  across requests inside the type checker.
- An edit to a widely used function's body checks all of its transitive
  callers, because their facts may have changed. The checker's facts are
  not visible from outside it, so the server cannot see that they did not
  change.
- Warnings (lints, deprecated calls) are not part of a check request's
  answer.
