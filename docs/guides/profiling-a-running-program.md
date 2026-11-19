# Profiling a running program

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

## What it needs

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

## How it samples

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

## Checking it

`tests/tooling/co5-profile-pid-check.sh` builds
`tests/projects/co5_service/service.fd`, starts it in the background,
attaches for two seconds and checks the samples (the hot function takes
about three times the samples of the cold one, the stacks run from
`main`), that the service still runs afterwards, and the errors for a
process that is not a Fides program and for one that does not exist.
