# DB9: the sampling profiler on every target, every thread, folded stacks

llvm-roadmap DB9 asks for a sampling profiler on every native target that
names our functions. This note covers what was added on Nov 17, 2026 on top of
the Linux/macOS/`--nat` sampler of Nov 15 and the SL15 work (a profiler the
program drives, `stdlib/profiler.fd`).

## Use

```
FID_PROF=1 ./prog            # at exit: the table on stderr, most self time first
FID_PROF=folded ./prog 2> prog.folded
flamegraph.pl prog.folded > prog.svg      # or load prog.folded in speedscope
FID_PROF_EVERY=5 ./prog      # also the table every 5 s (native targets)
```

`FID_PROF=folded` prints one line per distinct stack, root first, then its
number of samples (`main;main_work;fib;fib 43`). Nothing else goes to stderr
in that mode (no header), so the file is the input of `flamegraph.pl` and
speedscope's folded importer. A program's own stderr output is
mixed in, as for the table.

## What each backend does

| Backend | Sampler | Threads | `FID_PROF` at start | Folded |
|---|---|---|---|---|
| C (`ccompile`, POSIX) | `ITIMER_PROF` + `SIGPROF`, `_Unwind_Backtrace` | all (the signal falls on the running thread) | yes (`__p_sp_boot` in `main`) | yes |
| C (`ccompile`, Windows x64/ARM64) | a thread: `SuspendThread`, `GetThreadContext`, unwind tables | the thread that started it and every thread `p_thread_wrapper` starts after | yes | yes |
| pe_ir (`emit`, Windows default) | `pe_runtime/conc.fd` sampler thread (SL15) | every runtime thread | after SL15 needsOther A+B (entry point hook); `prof_start()` now | yes |
| nat_ir Linux x86-64/AArch64/RISC-V, macOS | `SIGPROF` (`linux_runtime/sys.fd`) | all | yes (`pe_rt_init`) | yes |
| nat_ir `--nat` Windows | `win.fd` sampler thread | main + every thread `pe_win_clone` starts once it is up | yes | yes |
| WASI, tcc | none (`prof_start()` returns false) | | | |

Code:

- C: `compiler/codegen_c.fd` `emit_prof_api_runtime`. The folded table is
  `__p_sf_*` (4096 stacks of up to 64 frames, made with `calloc` before
  sampling starts; a new stack claims its slot with a compare-and-swap of its
  hash, is filled, then counted). Windows: `__p_sp_th[]` (up to 64 threads),
  `__p_sp_add_current` from `__p_sp_begin` and `__p_sp_thread_start`
  (`p_thread_wrapper`); the sampler looks at each thread whose cycle count
  moved since the last look. `__p_sp_boot` (called by `main` before
  `main_entry`) reads `FID_PROF` and registers `__p_sp_atexit`, which stops
  the sampler and prints the report. Not in `fid profile` builds (`P_PROFILE`
  has its own sampler).
- Native Linux/macOS/`--nat`: `stdlib/linux_runtime/sys.fd` `pe_prof_fold_init`,
  `pe_prof_chain` (the same walk as `pe_prof_tick_regs`, twice: once for the
  hash, once to fill a new slot, so the signal handler needs no buffer),
  `pe_prof_fold`, `pe_prof_print_folded`; `win.fd` `pe_win_prof_add`,
  `pe_win_prof_thread`, `pe_win_prof_main` over all records.
- pe_ir: `stdlib/pe_runtime/conc.fd` `pe_pf_fold_init`, `pe_pf_chain`,
  `pe_pf_fold`, `pe_pf_print_folded` (one writer: the sampler thread).

A fix found on the way (`sys.fd` `pert_prof_start`): on `--nat` Windows, a
program that called `prof_start()` while `FID_PROF` had already started the
sampler could lose it (the sampler thread ends when it sees the profiler off
and `pe_prof_keep` 0, and `pert_prof_start` turned it off before setting
`pe_prof_keep`): 4 runs in 12 got 0 samples; now 0 in 15.

## Checked (Nov 17, with the Nov 16 compiler and this runtime text patched in)

- C, Windows (zig cc): `tests/prof/db9_threads.fd` under `FID_PROF=1`: 481
  samples, fib 100 % self, `main` 50 %, `thread_work` 50 % total (the second
  thread was not sampled before). `FID_PROF=folded`: stacks rooted at `main`
  and at `thread_work`.
- C, Linux (WSL, gcc): the same, 177 samples (SIGPROF), both threads.
- `--nat` Windows: `db9_threads` and `db9_api` under `FID_PROF=1` and
  `FID_PROF=folded`: both threads (`pe_win_thread_main;pe_thread_main;
  pe_thread_body;...`); 15 of 15 runs sampled.
- pe_ir and native Linux: type-check only (the Nov 16 compiler lacks pe_ir's
  `nat_fn_names` and builds Linux executables that ask for a loader).

Run `FID=$FID bash tests/prof/db9-check.sh` with the rebuilt compiler for every
backend at once, and `scripts/prof-check.sh`,
`tests/nattools/machine_batch8_check.sh` for the earlier checks.

## Limits

- Stacks deeper than 64 frames (C) or 96 (native) lose their outermost
  frames: the innermost ones (where the time is) are kept.
- At most 4096 distinct stacks; more go to one line
  `(more stacks than the table holds) N`.
- C on Windows and `--nat`: threads started before the profiler are not
  sampled (except the one that started it); under `FID_PROF` that is none.
  At most 64 threads.
- pprof's protobuf format is not written.
