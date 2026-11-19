# One interface for the runtime's machine-code primitives (MI20)

Every native backend defines the same set of `nat_*` primitives, the
machine-code pieces a runtime cannot write in Fides: system calls and
threads, the thread word, signals, atomics, the task context switch,
calls by address, traces (frame records and the function tables), the
start block, and a few target facts. The list is
`prim_interface_names()` in `compiler/pe_ir_stubs.fd`.

| Group | Names | Contract |
|---|---|---|
| system | `nat_syscall(n, a..f)` | a Linux system call by generic number; `-ENOSYS` on pe_ir |
| threads | `nat_clone_thread(entry, arg, stack_top, ctid, tls)` | starts `entry(arg)` on a new thread whose thread word is `tls`; `*ctid` becomes 0 when it ends; the thread id, or `-EAGAIN` |
| | `nat_tls_get()`, `nat_tls_set(v)` | the thread word |
| signals | `nat_sig_handler()`, `nat_sig_restorer()` | the handler's address (Windows: the vectored exception handler) and the restorer's (0 on Windows) |
| atomics | `nat_atomic_load/store/add/cas` | 8-byte, sequentially consistent; `add` returns the old value, `cas` 1 or 0 |
| tasks | `nat_ctx_swap(save, sp)`, `nat_ctx_tramp()` | switch contexts; the trampoline a new context starts in (its saved layout belongs to the runtime of the target) |
| calls | `nat_call_fn(addr, nargs, argv)` | `addr(env 0, argv[0..8])` |
| traces | `nat_fn_table()` | `[name, address]` per function named by a string, then `[0, 0]` |
| | `nat_fn_names()` | `[address, name]` per function in address order, then `[end of code, 0]` |
| | `nat_frame_fp()`, `nat_frame_ret(fp)`, `nat_frame_next(fp)` | the caller's frame record, its return address, the next record |
| start | `nat_start_sp()` | `[argc][argv...][0][envp...][0]` |
| target | `nat_arch()`, `nat_os()`, `nat_fma(a, b, c)` | 1 AArch64, 2 RISC-V, 3 x86-64; 0 Linux, 1 Windows, 2 macOS; `a * b + c` rounded once on f64 bits |

## Who defines them

- The machine layer (`compiler/nat_ir.fd`): Linux x86-64, AArch64,
  RISC-V, `emit --nat` Windows and macOS. `ng_prim_check` fails a build
  under `FID_VERIFY=1` when one is missing.
- pe_ir, the default Windows `fid emit` (`compiler/pe_ir_stubs.fd`,
  `PeirGen_gen_nat_prims`; the tables in `compiler/pe_ir.fd`,
  `PeirGen_gen_nat_tables`). Arguments come in the C convention (rcx,
  rdx, r8, r9, then the stack). The atomics, `nat_call_fn`,
  `nat_ctx_swap` and `nat_ctx_tramp` share the bytes of their `pe_*`
  twins. A primitive is made only when a reached `extern "C"`
  declaration names it, so images that use none are unchanged except for
  a 2-byte trap per unused name. Under `FID_VERIFY=1` all of them are
  made and `PeirGen_nat_check` fails the build ("the runtime primitives
  of this target lack ...") if one is missing. The entry point allocates
  the TLS index and builds the start block from the command line. The
  tables hold absolute addresses, because the image loads at its preferred
  base and has no relocations.

## Checking

`bash tests/nattools/mi20_prim_check.sh <fid>` builds
`tests/nattools/mi20_prims.fd`, which calls every group of primitives by
these names, with pe_ir and `--nat`, both with and without `FID_VERIFY=1`.
With `LINUX=1` it also builds for the Linux targets. Each build must print
the same lines.

## Still to do

`stdlib/pe_runtime` still calls the `pe_*` names (`pe_atomic_*`,
`pe_call_fn`, `pe_ctx_swap`, `pe_ctx_tramp_addr`, `pe_fn_lookup`,
`pe_thread_entry_addr`). The next steps are to move it to the `nat_*`
names, and then to drop the `pe_*` twins from `pe_primitive_names` and
`pe_stub_deps`.
