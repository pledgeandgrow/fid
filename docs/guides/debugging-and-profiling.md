# Debugging and Profiling

> Consolidated Nov 22 from debugging-native.md, profiling-a-running-program.md and crash-reports-and-replay.md.

---

## Debugging a native build with gdb or lldb

A native Linux executable from `fid emit --target x86_64-linux`
(`aarch64-linux`, `riscv64-linux`) carries DWARF 4 debug information, in
optimized builds as well as in `--debug` builds:

| Section | What it holds | Written by |
|---|---|---|
| `.symtab` | every function's symbol | `compiler/nat_enc.fd` `nat_elf_debug` |
| `.debug_line` | a row per source line, with a file table for programs of several files (DB1) | `nat_debug_line` |
| `.debug_frame` | the frame of every function, prologues and epilogues, the callee-saved registers wherever they are saved (DB6) | `nat_elf_debug` |
| `.eh_frame`, `.eh_frame_hdr` | the same rules where unwinders that read only these look (libgcc, libunwind, perf), loaded with the image and named by `PT_GNU_EH_FRAME` (DB6) | `nat_elf_eh_frame` |
| `.debug_info` | the compile unit; per function its name as written, its symbol as linkage name, frame base, declaration line, parameters and variables (DB2, DB4, CT9); the program's struct types | `nat_elf_debug`, `nat_dbg_types`, `nat_dbg_put_vars` |
| `.debug_loc` | where a value held in a register is, as code ranges (CT6) | `nat_dbg_put_vars` |

The machine layer records what goes in them: `compiler/nat_ir.fd`
`ng_dbg_vars_note` (each source variable and where instruction selection put
it), `ng_dbg_mark` (where each machine instruction starts),
`ng_dbg_live_ranges` and `ng_dbg_vars_resolve` (the register, frame slot or
constant each one ends up in after allocation, and where in the code).

### Running under gdb

On Linux (or WSL) with qemu-user and gdb-multiarch:

```
fid emit prog.fd --target aarch64-linux -o prog
qemu-aarch64 -g 1234 ./prog &
gdb-multiarch prog -ex 'target remote :1234'
(gdb) break prog.fd:47
(gdb) continue
(gdb) info args
(gdb) info locals
(gdb) print p            # a struct variable: {x = 3, y = 4}
(gdb) ptype struct Seg
(gdb) break 'Point::len2'
(gdb) info functions pick   # pick<i64>, an instance of a generic function
```

An x86-64 build runs directly on an x86-64 Linux machine: `gdb ./prog`.

### macOS (`--target aarch64-macos`, `x86_64-macos`)

A Mach-O of the machine layer keeps the same DWARF in the executable, in a
`__DWARF` segment between `__DATA` and `__LINKEDIT` that the code signature
covers: `__debug_abbrev`, `__debug_info`, `__debug_line`, `__debug_frame`
and, when a value has a location list, `__debug_loc` (`compiler/nat_enc.fd`
`nat_macho_debug_cfi`, `nat_macho_dwarf_full`; the DWARF is
`nat_dwarf_build`, shared with the Linux and `--nat` images). lldb reads it
without a `.dSYM`:

```
fid emit prog.fd --target aarch64-macos -o prog
lldb ./prog
(lldb) breakpoint set -n 'Point::len2'
(lldb) run
(lldb) frame variable
(lldb) type lookup Seg
```

Without struct types and variables (`FID_NAT_OFF=dbgvars` on a program
with no structs) the segment has the functions and lines only (TL5). Check:
`bash tests/nattools/dbg_ct9_check.sh` (`tests/nattools/dbg_sections.py`
finds the DWARF sections of an ELF, a PE or a Mach-O, so
`dbg_names_check.py` and `dbg_vars_check.py` read all three; the unit test
`tests/nattools/macho_dbg_types_unit.fd` walks the sections as lldb does).

### Windows (`fid emit --nat`)

A debug build of the machine layer's Windows backend carries the same
DWARF (line table, functions, parameters and variables with their location
lists, struct types, frames) as COFF sections after `.data`, the way
MinGW's linker writes them: `.debug_abbrev`, `.debug_info`, `.debug_line`,
`.debug_frame` and `.debug_loc`, named through the COFF string table
(`compiler/nat_enc.fd` `nat_pe_debug`, `nat_pe_add_sections`,
`nat_pe_symbols_pre`; the DWARF itself is `nat_dwarf_build`, shared with
the Linux images). gdb and lldb on Windows read it:

```
fid emit prog.fd --nat --debug -o prog.exe      # or -g; FID_NAT_DWARF=1
gdb prog.exe
(gdb) break prog.fd:47
(gdb) run
(gdb) info locals
```

It is written for `--debug`, `-g` (main.fd sets `g_nat_win_dwarf`),
`FID_NAT_DWARF=1` or `FID_PE_DEBUG=1`; a plain `--nat` build keeps only its
COFF symbols, byte for byte as before (`compiler/nat_ir.fd`
`ng_dbg_win_dwarf`, `ng_dbg_lines_ok`). Visual Studio and WinDbg read PDB
files, not DWARF: `emit -g` also writes a PDB (`compiler/pdb.fd`,
[windows-pdb.md](windows-pdb.md)).

Check: `bash tests/nattools/pe_nat_dwarf_check.sh` (the unit test
`tests/nattools/pe_nat_dwarf_unit.fd`, then `--nat` builds of
`test_machine_dbg_vars.fd` and `test_machine_dbg_run.fd` read back by
`tests/nattools/pe_nat_dwarf_check.py`, and by
`x86_64-w64-mingw32-objdump --dwarf` when installed).

### Windows, the default backend (`fid emit --debug`, pe_ir)

A debug build of the default Windows backend (`fid emit prog.fd --debug`,
`-g` or `FID_PE_DEBUG=1`) carries, for the functions of the input file:

- `.debug_line`: a row at each function's first byte on its `fn` line,
  then a row wherever the source line changes, so `break <function>`
  stops after the prologue (at the second row, with the parameters
  stored) and `break prog.fd:<line>` stops on the line;
- `.debug_info`: each function with its declaration line, its frame base
  (`DW_OP_call_frame_cfa`) and its parameters and variables, each with its
  type (integers, floats, `bool`, `str` as text, structs with their fields,
  a struct held by its address as the struct) and its location;
- `.debug_frame`: an FDE per function (CFA = rbp + 16 after
  `push rbp; mov rbp, rsp`), so a debugger finds the frame and the caller.

A debug build keeps every value in its frame slot: pe_ir runs without its
register allocator (as `FID_PE_OFF=ra`) and every value a variable is bound
to keeps its slot for the whole function, so a variable is at one place,
`DW_OP_fbreg`, from its definition to the function's end; a constant is
`DW_OP_consts`. `-g` keeps the mid-end, which may inline a small function
or promote a variable away (it then shows as optimized out); `--debug` runs
no mid-end. The DWARF itself comes from the machine layer's writer
(`compiler/pe_ir_enc.fd` `pe_add_dwarf` over `compiler/nat_enc.fd`
`nat_dwarf_build`; `compiler/pe_ir.fd` `PeirGen_dbg_vars_note`,
`peir_dbg_type_descs`, `pe_lines_setup`).

```
fid emit prog.fd --debug -o prog.exe
gdb prog.exe                      # a MinGW gdb; or lldb prog.exe
(gdb) break prog.fd:29
(gdb) run
(gdb) info locals
(gdb) next
```

Switches: `FID_PE_OFF=dbgvars` writes the line tables and functions only
(the three sections of before, byte for byte); `FID_PE_DEBUG_RA=1` keeps the
register allocator in a debug build (variables held in registers then show
as optimized out).

Check: `FID=release/fid.exe bash tests/nattools/pe_dbg_vars_check.sh`. It
runs the unit test `tests/nattools/pe_dwarf_vars_unit.fd` (decoded by
`tests/nattools/pe_dwarf_vars_check.py`), then builds
`tests/bootstrap/test_pe_dbg_vars.fd` with `--debug` and `-g` and runs it
under `tests/nattools/pe_dbg_run_check.py`: a small debugger over the
Windows debugging API (no gdb needed) that breaks at the test's `// @name`
lines and at functions, steps over a line and reads every variable
through the DWARF, comparing it with the test's `// dbg` lines. binutils'
objdump (native or through WSL) must decode the sections, and gdb and lldb,
when installed, run the same session as a script.

### What a debugger sees

- **Names as written (CT9).** A method shows as `Point::len2` (its symbol,
  `Point_len2`, is the linkage name, so `break Point_len2` works too); an
  instance of a generic function as `pick<i64>` (symbol `pick__i64`), a
  generic method as `Stack::push<i64>`. Struct types have their fields by
  their Fides names, a struct held inline in another refers to its own
  entry; a `str` is the type `str`, a pointer to its characters, so `print s`
  shows the text. A struct a variable holds by its address (a struct value,
  `self`) is shown as the struct itself.
- **Optimized builds (CT6).** A value kept in a register has a location list:
  the code ranges where that register holds it. Outside them gdb says
  `<optimized out>`, it never shows another value that took the register. A
  value in a frame slot (spilled, or a local whose address is taken) is
  there for the whole function. A local the IR optimizer turned into SSA
  values has no one place and shows as optimized out.
- **Debug builds (DB4).** `fid emit --debug` runs no mid-end and the fast
  allocator, which keeps the values that cross blocks in their slots, so
  every parameter and variable is available at every line.
- **Lexical scopes (DB4).** A name bound more than once in a function (an
  inner `var x` that shadows the outer one, `var e` in two loops) gets a
  `DW_TAG_lexical_block` per binding, so `info locals` and `print x` show
  the binding in scope at the stop instead of both. A binding's scope runs
  from its declaration's line to the `}` that closes the block it is
  declared in (read from the source, `nat_enc.fd` `nat_dbg_scope_end`),
  and at least over every line that uses it; its code is each run of code
  whose line rows fall in those lines (code inlined from another file
  counts as the line before it), so a cold panic path moved to the end of
  the function is a second range of the same scope (`nat_ir.fd`
  `ng_dbg_scope_lines`, `ng_dbg_scope_blocks`; `nat_enc.fd`
  `nat_dbg_put_vars`). Names bound once stay variables of the whole
  function, as before.
- **The direct path (`FID_NAT_OFF=mir`, AArch64 and RISC-V).** It writes
  variables too (`ng_dbg_vars_direct`): an alloca at its frame data, a
  value in the callee-saved register `ng_regalloc` gave it (a location list
  from just after its first definition or read to the end of its last
  read: inside the register's range, so never another value's) or else in
  its frame slot.
- **The function cache (`FID_FNCACHE`).** A function served from the cache
  keeps the variables and scopes of the build that compiled it (its entry
  holds them, CS6/SP20), so a cached build has the same debug sections.

### Limits

- Windows: debug builds of the default backend (`fid emit --debug`, pe_ir)
  and of `--nat` have the DWARF (above); pe_ir's has no lexical scopes (a
  name bound twice is listed twice) and covers the input file's functions
  only, not methods of `impl` blocks. Visual Studio and WinDbg read PDB
  files, not DWARF (compiler/pdb.fd, llvm-roadmap DB3). macOS has the full DWARF
  (above); it has not been tried under lldb on a Mac yet.
- Only names bound more than once get lexical scopes; a variable whose
  name is unique is listed for the whole function (outside its live range
  it shows as optimized out). A scope is found from the source's braces, so
  a declaration on the same line as the `{` of its block takes the
  enclosing block's end.
- Enums show as a struct with `__tag` and `__data0..` members: the variant
  names are not in the IR the machine layer sees.
- core's and the runtime's functions have no line rows and no variables.
- A float in a float register (`FID_NAT_ON=fregs`, MI25) has a location
  when it is an f64; an f32 there shows as optimized out.

### Unwind tables (DB6)

`.debug_frame` (`nat_dwarf_build`) has one CIE and an FDE per function: the
frame record of the prologue, the callee-saved registers the prologue
stores (`nat_ir.fd` `ng_cfi_saves`), and each epilogue. Shrink wrapping
(RA12) saves some callee-saved registers later than the prologue and may
give them back before the exits; `ng_dbg_sw_states` works out which of
those saves have run where each machine instruction starts (a walk of the
blocks from the entry) and `ng_dbg_sw_to` writes a frame row wherever that
changes: after the deferred stores, after an early restore, and at the
label of a block reached in another state. A row with n = 0 is a restore
(`DW_CFA_restore`). `FID_NAT_OFF=swcfi` leaves those rows out (only the
prologue's saves are described, as before).

The same FDEs go to `.eh_frame` (`nat_eh_frame`: augmentation `zR`,
addresses pc-relative in 4 bytes), with `.eh_frame_hdr` (`nat_eh_frame_hdr`:
the binary search table sorted by address) before it. Both sit right after
the image, inside its last `PT_LOAD` (the data segment, which grows over
them), and a `PT_GNU_EH_FRAME` names the header. The number and kind of
loadable segments stay as they were. `FID_NAT_OFF=ehframe`: neither.

Checks: `bash tests/nattools/db6_eh_check.sh <fid>` (the three Linux
targets: `tests/nattools/eh_frame_check.py --readelf`, libgcc's
`_Unwind_Find_FDE` through `tests/nattools/eh_frame_libgcc.c`,
`tests/nattools/sw_cfi_check.py`, `cfi_saves_check.py`, and the programs
still run) and `fid run tests/nattools/eh_frame_unit.fd`.

### Switches and checks

- `FID_NAT_OFF=debug`: no debug sections at all; `FID_NAT_OFF=dbgvars`: no
  variables (the sections as before DB4); `FID_NAT_OFF=dbgranges`: a value
  in a register is described for the whole function (no `.debug_loc`);
  `FID_NAT_OFF=dbgscopes`: no lexical blocks (shadowed names appear twice,
  as before); `FID_NAT_OFF=dbgbraces`: scopes from the lines that use each
  binding only, without reading the source.
- `bash tests/nattools/dbg_scopes_check.sh` (FID=<fid>, in Linux or WSL):
  the scope unit tests (`tests/nattools/dbg_scopes_unit.fd`,
  `dbg_scope_blocks_unit.fd`), then `tests/bootstrap/test_machine_dbg_scopes.fd`
  on the three Linux targets, optimized and `--debug`, on the direct path,
  through the function cache and as Mach-O and `--nat` images:
  `tests/nattools/dbg_scopes_check.py` (blocks nest inside their parents,
  siblings do not overlap, the shadowed names are in blocks, each marker
  sees the innermost binding; `llvm-dwarfdump --verify` when installed) and
  `dbg_run_check.py` (values under qemu, innermost scope first).
- A function's declaration line (`DW_AT_decl_line`, what `info scope`,
  lldb's `image lookup -n` and IDEs show) is the line of its `fn` keyword:
  nat_enc.fd `nat_dbg_decl_line` reads it from the source file of the
  function's first line row, looking up past blank lines, comments and a
  signature over several lines (DB2). A generated function (a closure) and
  a source that cannot be read keep the first row's line;
  `FID_NAT_OFF=declline` gives every function that line, as before.
  Check: `python3 tests/nattools/dbg_decl_lines_check.py --fid <fid>` and
  `fid run tests/nattools/decl_line_unit.fd`.
- `python3 tests/nattools/dbg_run_check.py <exe> tests/bootstrap/test_machine_dbg_run.fd [--strict]`
  runs the program under qemu-user's gdb stub, stops on the lines the test
  marks and compares each variable with what the test says it holds (no gdb
  needed). `--strict` (for `--debug` builds): every value must be
  available.
- `bash tests/nattools/dbg_gdb_check.sh [target] [--debug]`: the same
  program under gdb itself.
- `python3 tests/nattools/dbg_vars_check.py <exe>` and
  `python3 tests/nattools/dbg_names_check.py <exe>`: the `.debug_info`
  entries of `test_machine_dbg_vars.fd` and `test_machine_dbg_names.fd`.

---

## Profiling a running program

`fid profile <file.fd>` builds a program with a sampling profiler, runs it
and reports where the time went. A service that is already running in
production cannot be restarted that way. `fid profile --pid` attaches to
it instead (complaint CO5, competitive SL15):

```
fid ccompile service.fd -o service      # any C build; no --profile needed
./service &                             # running, say as process 4242
fid profile --pid 4242 --seconds 30 -o service.folded
```

It samples every thread of the process, about once a millisecond, for the
given number of seconds (default 10; Ctrl-C stops it early) and leaves the
process running. The output is the same as for `fid profile <file.fd>`:
collapsed stacks, one line per sample (`main;handle;parse 1`), which
`flamegraph.pl`, `inferno-flamegraph` and speedscope read, and a table of
the functions with the most samples.

### What it needs

- A program built through C (`fid ccompile`, `fid build` through C) by a
  `fid` that writes `__p_prof_anchor` into the C. That is a small
  constant beside the table of Fides function names that panic traces
  already use (`__p_fns`): two magic words, then its own address, so the
  profiler can find the table in the running program's memory and name
  each frame. Programs from `fid emit` (the native backends) have no
  such table yet: the profiler says so and stops.
- A C compiler (`CC`, or `zig`, `gcc` or `clang` on the PATH): the
  profiler is a small C program that `fid` writes and builds each time
  (`_fid_prof_attach.c`, removed afterwards). On Windows it links
  `dbghelp`, so tcc cannot build it.
- Permission to read the process. On Windows any process of the same user.
  On Linux, ptrace: with Yama's `ptrace_scope` at 1 (the default on
  Ubuntu) only an ancestor may attach, so start the service with
  `FID_PROF_ATTACH=1` (its runtime then calls
  `prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY)`), or profile as root.

### How it samples

- **Windows** (x64, ARM64): every thread is suspended in turn
  (`SuspendThread`, `GetThreadContext`), its stack is walked over the
  program's unwind tables with dbghelp's `StackWalk64` (which reads the
  other process's memory), and the thread is resumed. Each frame's
  function start is named through the program's table. These stacks are
  exact.
- **Linux** (x86-64, AArch64): each thread is attached with
  `PTRACE_SEIZE` and stopped with `PTRACE_INTERRUPT`; its registers give
  the innermost function, and the stack is scanned for return addresses:
  a word is a frame when the instruction before it is a call, and a
  direct call must call the frame found below it. The function ranges come
  from the executable's symbol table (`/proc/<pid>/exe`); a stripped
  binary falls back to the Fides table alone and the profiler warns that
  stacks are approximate. A tail call, which leaves no return address,
  can hide a frame.

Samples are wall-clock: a thread that waits (a channel, a socket, a
sleep) is sampled where it waits. A thread with no Fides function on its
stack (an idle runtime thread) gives no sample.

### Checking it

`tests/tooling/co5-profile-pid-check.sh` builds
`tests/projects/co5_service/service.fd`, starts it in the background,
attaches for two seconds and checks the samples (the hot function takes
about three times the samples of the cold one, the stacks run from
`main`), that the service still runs afterwards, and the errors for a
process that is not a Fides program and for one that does not exist.

---

## Crash reports, task chains and replaying a run

Three tools for the bug that only shows up somewhere else: a crash report
you can read later, the chain of tasks behind a panic in a task, and a
recording of a run's inputs that the interpreter replays exactly.

### Crash reports (`fid inspect`)

A program writes a crash report when it panics or crashes and the
environment variable `FID_CRASH_REPORT` names a file:

```bash
FID_CRASH_REPORT=crash.txt ./server        # a program built through C
FID_CRASH_REPORT=crash.txt fid run app.fd  # the interpreter
fid run app.fd --crash-file crash.txt      # the same, as a flag
fid inspect crash.txt
```

`fid inspect` prints the program, the build that made it, the panic
message and the trace (innermost call first):

```
Crash of ./server
  build:   fid 1.0.0+dev (C)
  panic:   empty age field
  trace (innermost call first):
    - parse_age
    -| called from load_record
    -| called from main
```

The report is written for a `panic`, a failed check (index, overflow,
unwrap), a stack overflow, an invalid memory access and the signals the
runtime catches (SIGSEGV, SIGBUS, SIGABRT, SIGFPE; on Windows access
violations, stack overflows and illegal instructions). stderr says where
it went. A run that does not fail writes nothing.

The format is plain text, one field per line, so it can be attached to a
bug report as it is:

```
fid-crash-report 1
version: <the fid that built the program; a release build adds its commit and date>
program: <the program: argv[0], or the source file under fid run>
message: <a line of the panic message>        (one line each)
trace:
<the trace's lines, and the task chain of a panic in a task>
end
```

`--crash-file` is `fid run`'s flag. `--crash-report` without a file is
something else: it reports a crash of the compiler itself (the stage and
the function it failed in, and how to reduce the program).

Programs built natively (`fid emit`: pe_ir, `--nat` and the Linux targets)
write the report too, a stack overflow included (Nov 17; checked Nov 21
for pe_ir, `--nat` and x86_64-linux). Their traces are not right yet: an
inlined innermost function is missing from them and the last `called from`
line is malformed, and their version line is always `fid 1.0.0+dev
(native)`.

### The task chain of a panic in a task

A panic's trace shows the stack of the task that panicked. When that task
runs for others, the C runtime adds the chain of tasks that await it:

```
Panic: b failed
stack trace:
    - b
task chain: b <- a <- main
```

`b <- a` means `a` awaits `b`; the chain ends at `main` (or `(thread)` off
the main thread), or with `(started by f)` for a task nobody awaits yet.
The line is in the crash report too. Natively built programs print it as
well (`emit`, `emit --nat` and x86_64-linux, checked Nov 21).

### Recording and replaying a run (`fid run --record`, `fid replay`)

```bash
fid run app.fd --record run.trace      # run it, logging what its effects bring in
fid replay run.trace                   # run it again from the log
fid replay run.trace --steps           # each answered call on stderr
fid replay run.trace --until 7         # stop before the 7th call, show where it is
```

While recording, each call that brings something in from outside is made
and logged with its arguments and its result: files (read, read as bytes,
exists, size, hash, write, append, delete), directories, the environment,
the command line, the clock, the OS's random words, standard input,
`p_system` and processes run with captured output. The replay runs the
program in the interpreter from the source text stored in the trace and
answers each of those calls from the log instead of making it: the run
sees the same file contents, time, environment and random words, reads no
stdin and writes no file. Its output is the recorded run's.

A call that is not the next one of the log (another function, other
arguments) stops the replay with a `differs` message: the program changed,
or it ran differently. Fewer calls than recorded, or another exit code, is
reported as well, with exit status 1. `p_exit` and a panic in a task end
the recording with their exit code.

`--steps` prints `[replay k] name(args) -> result` for each call;
`--until k` stops before call k and prints the calls the program is in.

Limits: imported modules are read from disk again when replaying (only the
main file's text is in the trace); the interpreter's sockets are simulated
and need no log; recording works under `fid run` only, not in built
programs.
