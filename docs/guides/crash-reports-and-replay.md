# Crash reports, task chains and replaying a run

Three tools for the bug that only shows up somewhere else: a crash report
you can read later, the chain of tasks behind a panic in a task, and a
recording of a run's inputs that the interpreter replays exactly.

## Crash reports (`fid inspect`)

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

## The task chain of a panic in a task

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

## Recording and replaying a run (`fid run --record`, `fid replay`)

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
