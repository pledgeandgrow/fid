# Compiler crash reports, `fid reduce`, and `fid explain --stages`

This page covers two goals in docs/roadmap/llvm-roadmap.md: RL20 (crash
reports and the reducer) and DB7 (one function, stage by stage). The code
is in compiler/main.fd. The checks are in `tests/tooling/rl20-db7-check.sh`.

## `--crash-report`

`fid <command> ... --crash-report` runs the same command again as a child
process with `FID_CRASH_TRACE=1` set. The child writes a trace on stderr:

- `[stage] <name>`: the front end, lowering, iropt, rules, e-graph, native
  code, C generation and the C compiler, each written before the stage
  starts (`crash_stage`).
- `[iropt] <pass> <function>`: each mid-end pass and the function it is
  about to run on. A pass over the whole module writes `-` in place of the
  function. iropt.fd and iropt_me.fd read `FID_CRASH_TRACE` themselves
  (`ime_trace`).
- `[nat] <function>`: each function the machine layer compiles (nat_ir
  `g_nat_trace`).

If the child panics or dies, the parent prints a report from the last trace
lines. It gives the stage, the pass and function (or the machine layer's
function), the panic message and its first frames. It also gives the
`fid reduce` line that reproduces the failure. `--reduce` runs that line
too.

To test the report, make a deliberate crash with `FID_CRASH_TEST`:

- `FID_CRASH_TEST=<text>` panics at the start of the first stage whose name
  contains the text, for example `lowering` or `native`.
- `FID_CRASH_TEST=iropt:<pass>` panics in that pass, on the first function
  it runs on.
- `FID_CRASH_TEST=iropt:<pass>:<fn>` panics in that pass on function `<fn>`.

## `fid reduce`

```
fid reduce <in.fd> [-o <out.fd>] [--cmd "<command, {} = the file>" |
           --fails <run|check|ccompile|emit|nat|emit:<target>>]
           [--match <text>] [--timeout <s> | --no-timeout]
```

`fid reduce` removes statements and blocks from the input while the failure
stays the same. "The same" depends on the mode:

- `--cmd`: the command's exit status.
- `--fails`: the stage (build or run), the exit status, and the first error
  or panic line, with numbers and paths blurred.

Each try is a child process with a time limit. The default is 300 seconds,
the same as scripts/fuzz/reduce.py. Set another limit with `--timeout <s>`,
or remove it with `--no-timeout`.

Timeouts matter because removing a loop's step makes a program that never
ends. A try that runs past the limit is killed together with every process
it started:

- POSIX: a watchdog shell kills the process tree (`pgrep -P`).
- Windows: PowerShell `WaitForExit`, then `taskkill /T`.

The killed try counts as the failure `timeout`. It is kept only when the
input itself timed out, so `fid reduce` can also shrink a program that
hangs. At the end, the reducer prints how many tries were stopped.

## `fid explain <file> --stages <fn>` (also `fid emit ... --explain <fn>`)

This command prints function `<fn>` at each stage, in order:

1. Its IR after lowering, after iropt, after the rules and after the
   e-graph.
2. Its machine IR after each machine stage, on stderr as `[mir]` lines.
3. Its machine code, disassembled by compiler/disasm.fd `disasm_bytes`.
   Each line holds the address, the bytes and the instruction.

The function is found through the image's own symbol table, so every
executable format the machine layer writes works:

- ELF (Linux targets): `.symtab`.
- PE (`--nat`): the COFF symbol table from `nat_pe_symbols`.
- Mach-O (`--target x86_64-macos|aarch64-macos`): `LC_SYMTAB`, read by
  main.fd `macho_disasm`. Names have C's leading underscore, and a function
  ends where the next symbol of its section starts. For a universal binary,
  the first slice is read.

`fid disasm <Mach-O> [--fn <name>]` uses the same reader.
