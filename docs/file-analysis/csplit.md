# csplit.fd — split one generated C file into units

> **First audit 2026-11-09.** Probes with the latest self-hosted binary (s18).

- **Path:** `compiler/csplit.fd`
- **LOC:** 482 (no `module` line; imported as `compiler::csplit`)
- **Role:** `fid split-c <in.c> <prefix> <n>`. Reads a C file as top-level
  items (preprocessor lines, declarations, function definitions; strings,
  chars and comments skipped while counting braces; only the first branch
  of an `#if` inside an item is counted). Writes `<prefix>.h` (every item
  in declaration form: types/macros/`static inline` as is, function
  definitions as prototypes, mutable globals as `extern`, `static const`
  as is), `<prefix>_0.c` (the whole file with definitions given away
  replaced by prototypes, `static` removed from non-inline functions and
  mutable globals), and `<prefix>_K.c` (the header include and unit K's
  definitions). Only definitions outside every `#if` move; units get runs
  of about equal size. Top-level `__asm__` stays in unit 0.

## Where it's called

| Caller | What |
|---|---|
| `main.fd` `cmd_split_c` :509 | `cs_split_files` :457 |
| `scripts/bootstrap.sh` :117-125 | splits each stage's C into `SPLIT` (8) units compiled at -O2 in parallel; the release `fid.exe` is built this way |

Separate from `split_c_units` in compiler.fd (`ccompile --inc`, splits by
source module using `//@FN` marks).

## Findings — verified live

### LOW — `static` is removed from function-local variables (NEW)

`cs_strip_static` (:258) removes the first word `static` anywhere in the
text, and the whole definition is passed to it (:411). When the function
head has no `static`, the first local `static` loses it. Probe on a
hello-world C file split in 4: `p_process_run`'s `static int ctr = 0;`
became `int ctr = 0;` (temp file names `.fides_proc_<pid>_0` for every
call, so concurrent calls collide), and `p_net_connect`'s `static int ws`
became a local (WSAStartup on every call). This is in the shipped,
split-built `fid.exe`. Fix: strip only within the head (`it.start` ..
`it.body`).

## Findings — code inspection

### LOW — word search ignores literals and comments

`cs_strip_static` and `cs_has_word` (:242) do not skip strings or
comments: a head or global containing the word in a comment or string is
changed or misclassified.

### LOW — quadratic helpers

`cs_has_word`/`cs_strip_static` slice a new string at every position;
fine for short items, slow on long ones.

### INFO

- An unbalanced item makes the rest of the file one kept item (:233-236).
- `cs_decl_kind` returns 0 ("kept as is", duplicated into header and unit
  0) for anything not understood; a mutable global it misses would be
  defined twice.

## Fix order

1. Strip `static` only in the definition head.
2. Skip literals/comments in the word helpers.

## Verification state

Whole file read. Probe: `split-c` on a 31K-line generated C into 4 units
(rc 0; the `static` change seen in `u_3.c`). The units were not compiled
in the probe.
