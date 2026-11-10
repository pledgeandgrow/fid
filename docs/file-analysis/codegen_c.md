# codegen_c.fd — IR → C99 backend

> **Re-audited 2026-11-09** against the working tree (8,519 lines; 5,877
> at the 2026-10-22/23 audit). Probes with the latest self-hosted binary
> (s18) and `zig cc -fsyntax-only`.

- **Path:** `compiler/codegen_c.fd`
- **LOC:** 8,519 (about 4,400 of them are the C runtime as `wln` strings,
  `emit_runtime_impl` :4046-8418)
- **Role:** `IrModule` → one C translation unit plus its runtime. The
  path of `ccompile`, `dump-c`, `run --leaks/--checked-alloc`, `test`,
  `bench`, `profile`, `repl`, and `fid build` on POSIX. Imports ast, ir,
  checks (counted-loop analysis `peir_unchecked_ops`).

## Where it's called

| Caller | Site |
|---|---|
| `compiler.fd` `ccompile_source_with_path` | :1811-1812 (cache hit), :1843-1844 |
| `main.fd` `cmd_build` POSIX C path | :3227-3228 |
| `main.fd` `cmd_ccompile` | `set_c_unit_marks`, `set_c_profile`, `set_c_leaks`, `set_c_checked_alloc` (:1852-1855) |

Only compiler.fd imports it.

## What it emits (`CCodeGen_generate` :3949)

1. `#define P_PROFILE / P_LEAKS / P_CHECKED_ALLOC` when set.
2. `emit_runtime_header` :1135: FP contraction off, includes, Winsock or
   POSIX sockets, WASI shims (`__wasi__`: no processes/sockets,
   `P_NO_CTX`), `// PLINK: ws2_32`, `PTensor`.
3. `emit_extern_decls` :1243: `extern "C"` blocks, `// PLINK: lib` lines
   (main.fd links them), wrappers for externs taking/returning structs by
   value (`c_wrapped`).
4. `emit_runtime_types` :1482: PVec/PMap types, runtime prototypes.
5. String constants, struct definitions, module globals + runtime
   initializer, prototypes.
6. Each function (`gen_function` :2863), with `//@FN name` marks for
   `--inc` (split by compiler.fd `split_c_units`). Fast C bodies for
   core's hottest Vec functions (`fast_core_vec_body` :2754); inline typed
   Vec access (`cg_inline_typed_vec` :3017); integer arithmetic through
   helpers with overflow checks unless `@wrapping` or the counted-loop
   analysis proves them needless (`gen_arith_helpers` :8418,
   `gen_int_arith` :8490).
7. `emit_main_wrapper` :2516 (crash handlers, profiler start, thread
   registry), `emit_runtime_impl`, file byte I/O, and the `__p_fns`
   table (function address → name) used by traces, the profiler and the
   leak report.

### Runtime pieces (emit_runtime_impl)

- Vec/HashMap (`p_vec_*`, `p_map_*`) on core's layout; strings
  (`p_string_*`, `p_str_*`) with `__raw` C bodies wrapped by `p_str_own_c`
  so every producer returns a counted string (proposal 0004).
- Printing with one `fwrite` per print; files, dirs, env, argv, process
  capture (`p_process_run` via temp files), clocks, `p_os_random_u64`.
- Panic traces from the machine stack (`__p_frames`, `__p_print_trace_ctx`
  :4348): vectored handler + `SetThreadStackGuarantee` on Windows,
  `sigaction` + `sigaltstack` on POSIX.
- `P_PROFILE` sampler (:4450): a thread suspending the main thread every
  1 ms (Windows) or `SIGPROF`/`setitimer` (POSIX); writes collapsed stacks
  to `$FIDES_PROFILE_OUT` or `fides-profile.folded` at exit.
- Block allocator (:4553-4648): 4 MB arena chunks, 8-byte header
  `P_BLOCK_MAGIC | class`, power-of-two (and 3/4) classes up to 1 MB in
  per-thread free lists, larger blocks `calloc`; `p_free` panics on double
  or invalid free; ASan builds use calloc/free for everything.
  `p_arena_save`/`p_arena_restore`.
- Threads: real `CreateThread` shim on Windows (64 MB reserve), pthreads
  elsewhere; futures, executor, task poll, IO worker threads, atomics,
  Rc, mutex, TLS, channels.
- Coroutines (`p_co_*`, :5280+): own context switch in asm on x86-64,
  AArch64, RISC-V 64 (`P_OWN_CTX`), Windows fibers-like stacks, ucontext
  fallback; event loop (`__p_ev_*`), sleep, nonblocking accept/connect/
  recv/send.
- Sockets, readiness poll, signals, mmap; bare-metal builtins as no-op
  stubs; SIMD over Vec<f64>; tensors + autograd tape + KV cache +
  quantization + DP/TP stubs; JSON, HTTP, in-memory DB; simulated dlopen.

### Debug allocator (`--leaks`, `--checked-alloc`; :4589-4835)

Under `P_LEAKS` or `P_CHECKED_ALLOC` the block allocator above is renamed
`__p_block_malloc`/`__p_block_free` by `#define` and made `static`; a
second `p_malloc`/`p_free` after it records every live block in an
open-addressing table (spin lock) with the first 4 Fides frames of its
allocation site.
- `P_LEAKS`: at exit (`atexit`, registered on the first allocation), the
  blocks still live, grouped by site, largest first, top 20 (stderr).
- `P_CHECKED_ALLOC`: each block on its own pages, ending at a no-access
  page (`VirtualAlloc`/`mmap` + protect); a freed block's pages become
  no-access and are never reused.
- A pointer not in the table (e.g. `p_shared_alloc`) goes to
  `__p_block_free`.
Probe: a leaked 64-byte block was reported as "64 bytes in 1 block(s):
main".

## Findings — verified live

### MEDIUM — `f32 % f32` emits invalid C (STILL TRUE)

`is_float_mod` matches only `IrType::F64` (:3128-3136). Probe: `v9 = v7 %
v8;` on floats; zig cc: "invalid operands to binary expression ('float'
and 'float')". Fix: F32 → `fmodf`.

### MEDIUM — `BitCast` to/from a float becomes `(void*)` (NEW)

:3808-3812: `int_ptr_cast` returns "" for F32/F64, then `(void*)` is used.
`f32_value as f64` is lowered as BitCast (lower.fd), so `println(h as
f64)` gave `v13 = (void*)v12;` — zig cc: "operand of type 'float' cannot
be cast to a pointer type". (Root cause partly in lower.fd: f32→f64 should
be a float extend.)

### MEDIUM — `sanitize_c_name` denylist gaps (STILL TRUE)

:385-475. Probe: user `fn fopen(x: i64)` → "conflicting types for
'fopen'". Also missing: `system`, `abort`, `signal`, `write`, `read`,
`send`, `select`, `alloca`, `errno`, `index`, ... Prefixing user
functions would end the class.

## Findings — code inspection

### MEDIUM — thread/task tables have no bound (STILL TRUE)

`p_thread_handles[256]`, `p_task_results[256]`, `p_task_done_flags[256]`
(:5220-5222, :6053); `idx = p_thread_handle_count++` with no check and not
atomic (:6068, :6195, :6265). Spawn 257 writes out of bounds; concurrent
spawns race on the counter.

### LOW — spawned functions get at most 4 arguments (STILL TRUE)

`p_thread_wrapper` (:6040-6044): args 5-8 are read by `va_arg` and stored,
but the `default` case calls with 4.

### LOW — void DynCall miss is silent (STILL TRUE)

:3757-3797: non-void misses `abort()`, void ones do nothing. Candidates
are any `X_<method>` function; only the `type_name` strcmp separates them.

### LOW — silent no-ops (STILL TRUE)

`Inst::Asm` → comment + 0 (:3877; a template containing `*/` would end
the comment). `PersistLoad` → default, `PersistSave`/`Txn*`/`Unsafe*` →
nothing (:3894-3902). `Backward(_)` → `p_autograd_backward(NULL)` (:3909).

### LOW — runtime details (STILL TRUE)

`p_intern_str` keeps the caller's pointer, no copy (:5188).
`p_chan_try_recv` returns 0 on closed+empty (:6384) while `p_chan_recv`
returns `P_CHAN_PENDING` (:6368). `p_dl_open` returns fake sequential
handles (:6412).

### LOW — leak report silent for a program that never allocates (NEW)

`atexit(__p_mc_report)` is registered on the first `p_malloc` (:4809), so
"every block was freed" is never printed when nothing was allocated.

### LOW — latent (CHANGED)

- `GetElemPtr` on struct elements uses `ir_type_size(Struct) = 0`
  (:3271-3275, ir.fd:213). The type checker currently rejects array
  literals of structs, so not reached from source in a probe.
- `Inst::Panic` message is not escaped (:3874), but no IR producer emits
  `Inst::Panic` (only PIR L1/L3 have panics).
- `bb0` label is never emitted (:2952); nothing jumps to block 0 on the C
  path (no iropt runs before C).

### FIXED / CHANGED since 2026-10-23

- Windows threads: real `CreateThread` (:5196-5214), no longer inline.
- `find_inst` binary search (:2062); `alloca.h` included off Windows.
- Per-call `p_call_stack_push/pop` removed; traces from the machine stack.
- The 10-23 "What's good" note on call-stack push/pop is obsolete.

## Fix order

1. `fmodf` for F32 `%`; float-aware BitCast (and a real f32→f64 extend in
   lower.fd).
2. Prefix user C names (or extend the denylist).
3. Bound and lock the thread tables; pass 8 args.
4. Escape asm templates in comments; void DynCall abort.

## Verification state

Every function header read; gen_function, the arithmetic, cast, DynCall,
panic/asm arms, the allocator, debug allocator, threads, channels read.
Probes: f32 %, f32→f64 print, `fopen` name clash, `run --leaks`,
`run --checked-alloc` (runs). Not re-checked: tensor/autograd/JSON/HTTP/DB
runtime bodies, `emit_globals` escaping.
