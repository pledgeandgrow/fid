# Infra runs, Nov 16 (batch 5): N2, N4, N11, M8, I6

The CI scripts of these goals, run on Windows 11 with a fid built on
Nov 15 at 23:01 (older than the tree: it has the ark.fd of HEAD), CC=zig cc.

## N2 `scripts/quickstart-check.sh`: PASS

All 6 steps of docs/reference/quickstart.md ran and printed what the guide
says (ccompile, the built hello, emit, run, the Vec example), 27 s. The
compiler must find stdlib/ next to it or above it (`release/fid-dev` in CI
does), or through FID_HOME.

## N11 `scripts/projects-check.sh`: 4 of 5 pass

- cli, game (run and ccompile), cbind: PASS.
- service: failed to type-check. Its request function was named `handle`,
  which is the keyword of an effect handler: `handle(line, ...)` parsed as a
  handler expression, typed as the function itself ("cannot assign
  fn(str, &mut Vec<str>, ...) -> str to variable 'reply' of type str").
  Renamed `reply_to` in tests/projects/service/main.fd: PASS (lint clean,
  exit status 1, output = expected.txt). The parser accepting `fn handle`
  as a name is a compiler bug (needs a "keyword" error).
- library (scripts/ark-check.sh): `ark install` of a path pact that is a
  directory fails with that compiler ("Path pact 'textkit' does not exist"):
  HEAD's ark.fd copies files only; the tree's ark.fd has
  `ark_install_path_in` (xcopy/cp -R of a directory). Not checkable without
  a build of the tree.

## N4 `scripts/fmt-check.sh`: FAIL, cause found

Step 1: 229 of 258 files of compiler/ and stdlib/ "would reformat"; steps
2-4: 7 of 20 formatted test copies are not stable. One bug causes the
instability and most of the tree's damage: compiler/fmt.fd ends a line at
`;` only inside braces (`if brace_depth > 0 && paren_depth == 0`), so at the
top level `module m; use core; use vec;` stay on one line, the comment on the
next line becomes a trailing comment, and after a blank line the next
declaration gets a leading space (the space is decided before the blank line
is written). A second run joins that line too. The tree shows an earlier
whole-tree run of that formatter: ` use vec;` with a leading space in 28
stdlib files, `module compiler::fmt; use core; use vec; use string;  // ---`
in fmt.fd itself, all of sdl2_h.fd's constants on one line.

With the condition changed to `if paren_depth == 0` (checked with a copy of
fmt.fd compiled as a program over the whole tree): unstable files 50 -> 0,
token changes 0, and the files the bug had not touched (arena, token) pass
`--check`. 229 files still need one `fid fmt` pass (repairs of the old run,
and new code written in another style: aligned trailing comments, one-line
`if ... { return x; }`).

New: step 0 of fmt-check.sh formats tests/fmt/toplevel_in.fd and compares
it with tests/fmt/toplevel.fd, which must pass `--check`; it fails with the
current fmt and passes with the fix.

## M8 `scripts/sanitize.sh`

Already run nightly (verify-nightly.yml `sanitize`, clang on Linux); now
also a sample of 6 programs on every push (ci.yml `selfhost`). The script
now stops each run after LIMIT seconds (default 300) with no input, tells a
program that prints something different on each run (judged on sanitizer
reports alone) from a C-path difference ("also built without sanitizers")
and from one only under the sanitizers, and writes $OUT/summary.san. Here:
UBSan through zig cc on hello, test_vec_filled, test_closure: 3 clean
(zig cc has no ASan runtime; ASan needs the Linux job).

## I6 socket soak

scripts/verify/soak_http.py + tests/verify/soak_http_service.fd, nightly
job `soak` (30 minutes under wrk). First runs: every request answered
correctly, but memory grows about 95-265 bytes per request on a kept-alive
connection (docs/verification/soak-http.md).
