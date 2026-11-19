# Recorded check runs, 2026-11-16 (RL4, RL9, RL22, SP20)

Each run used the agents' one-stage `fid` of Nov 15 23:01 (`fidh1.exe`, not
a release bootstrap) on Windows 11 x64, with WSL (kali-linux) for the Linux
readers and executables and `zig cc` 0.16 as the C compiler. None of these is
the release run its roadmap entry asks for; each says what is left.
SP14's run is in docs/perf/sp14-egraph-bound.md.

## RL4: miscompile gate

`scripts/miscompile-gate.sh` now:

- checks the rows of "Earlier miscompiles" that name a test (the test must
  exist) and runs them with `--run`; `--before` skips them (no recent
  compiler has those bugs);
- `--new-since <rev>` limits `--run`/`--before` to the entries fixed now and
  not fixed at `<rev>`; with none, there is nothing to run;
- `MISCOMPILE_LOG=<file>` reads a copy of the log (to test the gate).

CI: .github/workflows/bench.yml, which already builds the base commit's fid
and head's on one runner, runs `miscompile-gate.sh --fid <head> --run
--before <base> --new-since <base sha>`, so the before-step runs on its own
for every change that marks an entry fixed. scripts/ci-wiring-check.sh keeps
that step wired.

Ten tests for the ten entries fixed before the log, each passing on `run`,
`ccompile` and `emit`:

| Entry | Test |
|---|---|
| AGENTS 4 | `tests/bootstrap/test_verify_legacy_vec_mut_arg.fd` |
| AGENTS 9 / known-bugs 1.6 | `tests/bootstrap/test_verify_legacy_enum_field_offset.fd` |
| AGENTS 10 / 1.7 | `tests/bootstrap/test_verify_legacy_native_values.fd` |
| AGENTS 12 | `tests/bootstrap/test_verify_legacy_zero_fields.fd` |
| AGENTS 13 | `tests/bootstrap/test_verify_legacy_first_slot_float.fd` |
| AGENTS 21 | `tests/bootstrap/test_verify_legacy_plain_str_free.fd` |
| known-bugs 1.11 | `tests/bootstrap/test_verify_legacy_enum_eq.fd` |
| known-bugs 1.18 | `tests/bootstrap/test_verify_legacy_print_types.fd` |
| known-bugs 1.19 | `tests/bootstrap/test_verify_legacy_long_function.fd` |
| known-bugs 1.30 | `tests/bootstrap/test_verify_legacy_float_consts.fd` |

Run, with a copy of the log that names these tests in the rows of
"Earlier miscompiles" (`MISCOMPILE_LOG=<copy> miscompile-gate.sh --fid
fidh1.exe --run`): "5 fixed entries checked, 0 without a test; 10 earlier
entries (before the log), 10 of them name a test", the 14 tests "14 of 14
files pass on run,ccompile,emit", the open repros 3 of 16 (informational),
"miscompile gate: ok". Left: the log itself names the tests (it is not this
group's file), and the CI step has not run yet on a change that fixes an
entry.

## RL9: image-check.sh

`scripts/image-check.sh` on Windows now finds dumpbin in Visual Studio's
MSVC tools without a developer prompt (and passes it `-nologo -headers
-imports`: Git Bash rewrote `/nologo` into a path, which made every dumpbin
check fail), and runs readelf, objdump and llvm-* in WSL when they are not
on PATH. `READERS_ONLY=1` lists the readers it would use.

Readers this host has: readelf (WSL), objdump with pei-x86-64 (WSL), dumpbin
(Visual Studio 18, MSVC 14.51), scripts/macho-check.py. Not installed:
llvm-readobj, llvm-objdump, llvm-dwarfdump.

Run on the default programs (hello, test_closure, test_hashmap, test_float,
test_tasks), every writer (ELF for x86_64/aarch64/riscv64-linux, PE from
pe_ir and `--nat`, Mach-O for both macOS targets): 45 checks passed, 0
failed (readelf 15 ELF images, dumpbin and objdump 10 PE images each,
macho-check 10 Mach-O images). Left: the same run with the LLVM readers
installed.

## RL22: AGREE=1 suite-all.sh

`scripts/suite-all.sh` takes `TESTS="a.fd b.fd"` (a smoke run on every
lane; the release check is still the whole suite).
`scripts/cross-test.sh` no longer exports `OS` (now `TOS`): on Windows it
overwrote `OS=Windows_NT`, which fid reads, so every pe_ir build of the
`native:x86_64-windows-pe` lane failed ("C compilation failed").

Smoke run, `AGREE=1 JOBS=1`, on hello, test_hashmap,
test_verify_legacy_enum_eq and test_verify_legacy_float_consts: all ten
lanes this host runs. c and native x86_64/aarch64/riscv64-linux (WSL with
qemu-user), c:x86_64-windows, native:x86_64-windows-pe and
native:x86_64-windows: 4 of 4 each. c:wasm32-wasi: no test builds — the C
runtime calls `getsockopt(..., SO_ERROR, ...)`, which WASI's headers do not
define (compiler/codegen_c.fd, the non-blocking connect). Left: that fix,
and the whole suite with `AGREE=1` on the release compiler.

## SP20: repro-check.sh

`scripts/repro-check.sh` now runs the x86-64 Linux executables in WSL on
Windows (so a fn-cache hit is checked to behave the same), says when no
artifact was written (the compare is then between two cache-off builds),
and `STRICT=1` fails on that and on a fn-cache hit that differs.

Run on hello, test_hashmap, test_tooling_explain (`emit --target
x86_64-linux`): the executables are identical with the artifact off, on a
miss and on a hit, and with FID_FNCACHE on a miss. On a fn-cache hit the
executable differs (hello: 47,896 bytes against 54,712) and runs the same
(output and exit 0, hello and test_hashmap). This compiler writes no
artifact for `dump-c` (the SP4 branch that keeps core alone for `dump-c` is
newer than it), so the C compares were vacuous. Left: the run with the
end-of-run build, and fn-cache hits that produce the same image
(compiler/nat_ir.fd, CS6).
