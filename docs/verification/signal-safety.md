# Signal-safe runtime paths — review (RL23)

> Reviewed Nov 15, 2026 against `stdlib/linux_runtime/` (Linux, Windows via
> `win.fd`, macOS via `darwin.fd`). The claim under review: crash and signal
> handlers allocate nothing and take no lock, so they cannot deadlock on a
> heap or scheduler lock the faulting thread may have held.

## The handler: `pe_on_signal` (`stdlib/linux_runtime/base.fd`)

One handler serves Linux signals (installed by `pe_sig_install` with
`SA_SIGINFO | SA_ONSTACK`), Windows vectored exceptions
(`pe_win_on_exception` maps the code to a Linux signal first), and macOS
signals (the same sigaction path, `pe_mac_info` as the siginfo). What it
touches:

- `pe_ld` / `pe_st` / `pe_stb` — raw memory loads and stores.
- `pe_write_all` — a `write` syscall loop to fd 2, no userspace buffering.
- `pert_eprint_str`, `pe_eprint_int`, `pe_eprint_hex` — format into
  `pe_sigbuf`, a 32-byte buffer allocated **once in `pe_rt_init` before the
  handlers are installed**, so the handler never allocates.
- `pe_sig_flags` — a 1024-byte table allocated at init. The registered-
  signal path is one word store (`pe_st(pe_sig_flags + sig * 8, 1)`).
- `pe_trace_from` — walks the frame records (`nat_frame_fp`/`nat_frame_ret`/
  `nat_frame_next`) and the function-name table; bounded at 256 frames,
  alignment-checked (`fp & 7`), writes through `pe_write_all` only.
- `pe_sys` — a raw system call; on Windows `nat_win_call` calls kernel32
  (`WriteFile`, `TerminateProcess`), on macOS libc syscall wrappers.

**Verdict: no allocation, no lock, no userspace state mutation besides the
two preallocated tables.** A signal caught while the heap lock is held can
still print, trace and exit.

## Caveats and notes

- `pe_eprint_hex`/`pe_eprint_int` write into `pe_sigbuf` unconditionally.
  `pe_trace_from` guards `pe_sigbuf == 0`, they do not: safety rests on the
  init order in `pe_rt_init` (buffer allocated before `pe_sig_install`
  runs). Same on Windows and macOS: `pe_win_info` / `pe_mac_info` are
  allocated during startup, before the vectored handler or sigaction is
  registered. A signal *before* init cannot reach our handler — the
  handlers do not exist yet. Keep the init order; it is the proof.
- Stack-overflow detection reads the task block's guard range and the TLS
  guard window (`pe_ld(cur + 120)`, `pe_ld(t + 16/24)`), all reads — the
  handler runs on the alternate signal stack (`SA_ONSTACK`), so a faulted
  thread's stack is not needed.
- On Windows the "signal" is a vectored exception: the handler calls
  kernel32 (`WriteFile`, `TerminateProcess`). Those are not interrupted-
  thread-safe in the POSIX async-signal sense, but they take no lock the
  exception handler could deadlock on in the runtime's own code — VEHs run
  in the faulting thread with the loader lock not held by the runtime.
- The panic path ends in `pe_sys(EXIT_GROUP)` / `TerminateProcess` — exit
  status 1, no unwinding, per reference §8.2 and RT15.

## What is not covered

- The C path's handlers (POSIX `sigaction` + `sigaltstack`, a Windows
  vectored handler in `stdlib/pe_runtime`) follow the same shape —
  preallocated buffers, write-then-exit — but are a separate
  implementation; reviewing them is the same checklist over
  `stdlib/pe_runtime/*.fd`.
- Reentrant signals: a second fault inside the handler restarts it on the
  same signal stack (no recursion guard); the trace may duplicate. Cheap
  to add (a flag word in `pe_sig_flags`), not yet done.
