# Serverless cold start (roadmap Phase 9, F32)

`@serverless` (compiler/typechecker.fd `TypeChecker_verify_serverless`)
checks that a handler can run cold: no writes to globals, no global whose
initializer runs code at start-up, no tasks. This page covers the build
side: a handler as the entry point, and how long such a process takes from
start to its first response.

## `--entry <handler>`

```
fid emit     handler.fd --entry handle_request -o handler      # native
fid build    handler.fd --entry handle_request -o handler      # the same
fid ccompile handler.fd --entry handle_request -o handler      # through C
```

The executable runs one request and exits. The request is `argv[1]`, or
one line of standard input when there is no argument. The response, which
is the handler's result, goes to standard output.

What the driver does (compiler/main.fd, section "F32 (serverless): a
handler as the entry point"):

1. `entry_source_for` reads `--entry` and calls `entry_wrap_source`. It
   parses the file and finds the handler among its top-level functions.
   A handler takes no parameter or one `str`, and returns `str`, an
   integer, `bool` or nothing; anything else is an error naming the
   handler. A handler not marked `@serverless` builds, with a note on
   stderr that its cold-start rules were not checked.
2. The program's own `main`, if any, is renamed `fid_entry_program_main`
   (`entry_rename_main`: a top-level `fn main(` or `pub fn main(` at the
   start of a line). Nothing calls it any more.
3. A generated `main` (`entry_main_text`) is appended. It reads the
   request and prints the handler's result. Line numbers of the file do
   not move, because the rename happens in place and the new text comes
   after the last line.
4. The rest of the build is unchanged. `emit` lowers only what
   compiler/reach.fd finds from `main` (the roots are main, core, the
   runtime and annotated functions), so the program's old `main` and what
   only it called are neither lowered nor in the image.
   tests/tooling/serverless-entry-check.sh checks this on the x86-64
   listing (`fid disasm --target x86_64-linux --entry ...`).

## The benchmark

```
FID=release/fid.exe RUNS=200 bash bench/coldstart.sh
```

It builds tests/bootstrap/test_serverless_entry.fd four ways:

- `fid emit --entry` (native: pe_ir on Windows, the machine layer elsewhere)
- `fid ccompile --entry` (through C, `CC`, by default `zig cc`)
- `fid emit` of the whole program
- the same handler written in C (bench/coldstart/handler.c, `zig cc -O2`)

Each one runs once as a warm-up and then `RUNS` times with the request
`/hello`. bench/coldstart/first_byte.py starts the clock before the process
is created and stops it at the first byte on stdout. The table gives each
build's size and the min, median and p90 milliseconds. A response that
differs from the C handler's is marked WRONG and the script exits 1.

## Numbers

**Preliminary (Nov 17, not yet the real measurement).** These were taken
on Windows 11 (x86-64, 16 threads) while other builds were running, with
stage0, a compiler from before `--entry` existed. The `--entry` builds were
emulated: I applied the exact source rewrite `entry_wrap_source` performs
to the file by hand. The medians are not reliable on a loaded machine.
Read the minimums.

| build | size (bytes) | min ms | median ms | p90 ms |
|---|---:|---:|---:|---:|
| fid emit, handler entry (pe_ir) | 46,434 | 8.5–10.3 | 25–38 | 54–84 |
| fid ccompile, handler entry (zig cc) | 343,552 | 9.7–13.0 | 22–43 | 54–89 |
| fid emit, whole program | 44,251 | 9.9–12.3 | 33–35 | 80–84 |
| C handler (zig cc -O2) | 190,976 | 10.1–14.0 | 19–49 | 33–103 |

On this machine, starting a process costs about the same whichever way the
handler was built: roughly 8 to 14 ms at best, almost all of it Windows
process creation. The native Fid build is the smallest image, at 45 KB
against 190 KB for the C handler and 340 KB through C.

The handler-entry image is 2 KB larger than the whole program here. It
adds argument and stdin reading (`arg_at`, `read_line`), and the whole
program had only one small function of its own to leave out. The trimming
pays off in programs whose `main` does more than the handler needs.

The real measurement still has to be taken with this run's final compiler
on a quiet machine, on Windows and on Linux (in WSL, where the machine
layer builds the handler). Use `RUNS=200 bash bench/coldstart.sh` and put
both tables here in place of the one above.
