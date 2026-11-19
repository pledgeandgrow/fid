# The sampling profiler of a running program (competitive SL15)

A program can be profiled while it runs, without being rebuilt for it and
without stopping it, on every backend:

| Backend | Started by | Sampler | Report on request |
|---|---|---|---|
| native Linux, macOS (`fid emit`, nat_ir) | `FID_PROF=1`, or `prof_start()` | SIGPROF every ms of CPU time (all threads) | `kill -USR2 <pid>`, `FID_PROF_EVERY=<s>` |
| native Windows (`fid emit`, pe_ir) | `FID_PROF=1`, or `prof_start()` | a thread that suspends each registered thread every ms it ran | the named event `Local\fid-prof-<pid>`, Ctrl-Break, `FID_PROF_EVERY=<s>` |
| native Windows `--nat` | `FID_PROF=1`, or `prof_start()` | a thread that suspends the main thread | `FID_PROF_EVERY=<s>` |
| C (`fid ccompile`) | `prof_start()` (and `fid profile --pid` from outside) | Windows: a thread that suspends the thread that started it; POSIX: SIGPROF | `prof_report()` |
| `fid run` | `prof_start()` | none: one sample per call | `prof_report()` |

The report is the same table everywhere, on stderr, most self time first:

```
profile (FID_PROF): 228 samples, one per ms of CPU time
   self%     self  total%    total  function
   100.0      228   100.0      228  prof_burn
     0.0        0   100.0      228  main
```

"self" counts the samples whose pc was in the function (on the C backend:
the innermost Fides function on the stack, so time in the C runtime and
libraries counts to the Fides function that called them); "total" the
samples with the function anywhere on the stack, once per sample.

## From the program: stdlib/profiler.fd

```
use profiler;

prof_start();                     // false where this build cannot sample
handle_requests();
val n = prof_stop();              // the samples taken
prof_report();                    // the table so far, to stderr
val hot = prof_samples("parse");  // samples with parse on the stack
```

`prof_start` drops earlier samples; `prof_report` can be called while
sampling goes on (a service can print it from a control endpoint). The
builtins behind them are `p_prof_start`, `p_prof_stop`, `p_prof_report` and
`p_prof_samples(name)`:

- `fid run` (compiler/interp.fd, "The profiler a program drives"): the
  interpreter has no clock to sample by, so each call of a Fides function
  while it is on is a sample (`interp_prof_count`, called by
  `Interpreter_run_function`). The table is headed "counting calls".
- C (compiler/codegen_c.fd `emit_prof_api_runtime`): a fixed table of 4096
  functions keyed by the name `__p_fn_name` gives (the panic traces' table),
  filled by the SIGPROF handler (atomic adds) or by the Windows sampler
  thread after it has resumed the thread (nothing that can take a lock runs
  while a thread is suspended). tcc and WASI builds have no sampler:
  `prof_start()` is false.
- native Linux/macOS/`--nat` (stdlib/linux_runtime/sys.fd
  `pert_prof_start` ...): the FID_PROF sampler (DB9) started and stopped
  from the program.
- native Windows, pe_ir (stdlib/pe_runtime/conc.fd, "The sampling
  profiler"): below.

## Native Windows (pe_ir)

pe_ir images name functions through `nat_fn_names` (MI20: an
address-to-name table after the code, emitted when the runtime reaches
it). Every OS thread the runtime starts registers itself as it starts
(`pert_thread_main` → `pe_pf_add_self`: a handle of its own, opened with
OpenThread, and its TEB): the task workers, the poller, the pool, and the
threads of `p_thread_spawn`; the thread that starts the profiler too (the
main thread under FID_PROF). Up to 64 threads.

The sampler thread raises the timer resolution to 0.5 ms and, every
millisecond while the profiler is on, looks at each registered thread:
when its cycle count moved since the last look (QueryThreadCycleTime: one
sample per ms of its CPU time, so an idle worker costs nothing), it
suspends it, reads rip, rsp and rbp (GetThreadContext), counts the
function rip is in as "self" and walks the rbp chain (every pe_ir function
starts `push rbp; mov rbp, rsp`) for "total", then resumes it. The walk
stays inside the stack the thread is on: StackLimit..StackBase of its TEB,
which a task switch keeps current; a sample taken in the middle of a
switch counts the pc only. Nothing is allocated while a thread is
suspended.

Control, as on Linux:

- `FID_PROF=1 prog.exe`: the profiler starts before main
  (`pert_prof_boot`, called by the entry point) and the table is printed
  at exit (`pert_prof_exit` after main, `pert_exit` for `exit()`).
- `FID_PROF_EVERY=<s>`: also every s seconds while it runs.
- In place of SIGUSR2: set the named event `Local\fid-prof-<pid>` from any
  process of the same session, for instance from PowerShell:

  ```
  [System.Threading.EventWaitHandle]::OpenExisting('Local\fid-prof-4242').Set()
  ```

  or press Ctrl-Break in the program's console. A watcher thread prints
  the table so far within 100 ms; the program runs on.

## Checks

- `tests/bootstrap/test_prof_sampling.fd`: the stdlib API on each backend
  (expected output in the file).
- `tests/prof/sl15-check.sh`: that test through `fid run`, `ccompile`,
  `emit` and (from Windows) `emit --target x86_64-linux` under WSL, with
  the table checked on stderr; and on Windows `tests/prof/sl15_service.fd`
  under FID_PROF, the named event (set by the program and from PowerShell)
  and FID_PROF_EVERY.

The C runtime's code was checked on Nov 17 by patching it into the C a
stage-0 `fid dump-c` wrote for the test: on Windows (zig cc, 228 samples,
prof_burn on top) and on Linux under WSL (gcc, SIGPROF, 91 samples,
prof_spin self, prof_burn total).
