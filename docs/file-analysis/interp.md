# File Analysis: compiler/interp.fd

- **Path:** `compiler/interp.fd` — module `compiler::interp`
- **LOC:** 7,456 (2026-11-09; was 3,967 on 2026-10-22). `src/interp.rs`: 6,358.
- **Role:** the IR interpreter behind `fid run`. It walks the SSA IR of a
  lowered module (core included), one instruction at a time, over a
  simulated byte memory, and runs the `p_*` builtins itself.
- **Status:** LIVE.
- **Re-audited:** 2026-11-09 (static read of the whole dispatch plus probes
  with the current compiler, `s18.exe run`).

## Where it is called

| Caller | Site | What |
|---|---|---|
| `compiler/compiler.fd` | :42 import; :2051, :2074, :2096 | `interpret_source*` call `interpret_ir_module` / `interpret_ir_module_with_options` |
| `compiler/main.fd` | :151 dispatch → `cmd_run` (:687) → :771 `interpret_source_with_path` | `fid run` (limits `--max-recursion`, `--max-steps`, `--max-memory`, `-v`) |
| `compiler/veil.fd` | :8 import; :322-325 | `fid veil` runs the program with `Interpreter_new` / `Interpreter_run` |
| `compiler/debugger.fd`, `compiler/hotreload.fd` | :43 / :22 | import it, but both modules are dead (nothing imports them) |
| `tests/selfhost/test_spawn_gt.fd` | | drives `Interpreter_new` / `Interpreter_run` directly |

Not reached by: `fid run --leaks` / `--checked-alloc` (main.fd:694-713 builds
through C and runs the binary instead), `fid test` (C only; the comment at
main.fd ~1079 "the self-hosted compiler does not yet have an interpreter" is
stale), `bench`, `profile`, `repl`, `notebook` (all C).

Imports: `core`, `vec`, `string`, `math`, `compiler::ast`, `compiler::ir`,
`compiler::green_thread` (the `p_gt_*` scheduler), `compiler::tensor`
(`TensorData`, `KvCache`), `compiler::json`.

## What it does now

### Execution model

- `Interpreter_run_function` (:1324) binds parameters into a `Vec<Value>`
  indexed by value id, inserts a dummy `__env` when the first parameter is
  `*u8` and the caller passed one argument fewer (:1362-1387), then walks
  blocks until a `Return` or `Panic`. Limits: recursion depth (default
  1000, :906), step count, memory (checked at `Alloca` only).
- Panics capture a stack trace once, append " at line N" from the
  instruction's meta, and restore the region stack on exit.
- Name lookups go through hash tables: `fn_cache` (function name → index),
  `rt_cache` (memo of `is_runtime_func`), `I64Table` for the opaque, dyn and
  tensor side tables (:547-650).

### Memory model

- `Memory` (:153) is a `Vec<i64>` with **one simulated byte per element**
  (8x host overhead), 1 MB pre-allocated (:211-220), bump allocation from
  0x1000 (:222). Globals live at 0x400000, 8 bytes each (:1072).
- **Nothing is ever reused.** `Memory_free` (:240) only marks the block in
  `blocks`; it reports "Invalid free" and "Double free" as native code does,
  but loads and stores never check for a freed block (no use-after-free
  detection). `p_region_free` is a no-op (:3207); `p_arena_save/restore`
  do nothing (:3209).
- Values: `Int`, `Float`, `Bool`, `Str`, `Ptr`, `Void`, `Vec(handle)`,
  `Map(handle)` (:40). A `Value::Str` is the interpreter's own opaque
  string; it only gets bytes in memory when it has to:
  - passed to an `i64`/`u64`/pointer parameter (:1397-1416),
  - stored through a pointer (:1983-1987), stored in an opaque slot, pushed
    into a raw Vec (`Interpreter_value_word`, :1584), or loaded through.
  `Memory_store_cstr` (:435) writes it the way core keeps one: a 16-byte
  header `[0x4649444553545201 magic][length]`, the bytes, a NUL, and
  registers the address as a block so a free of it is legal. The address is
  also put in `opaque_values` with the original `Value::Str`, so a later
  word load at that address gives the string back (:1567, :1817).
  Every materialization allocates again, and none is freed: a loop that
  passes a string to a `str` parameter grows memory by the string's size
  per call (code inspection).
- Reading a string back (`Interpreter_value_to_string`, :1116;
  `Memory_load_cstr`, :459) uses the header length when the word at
  `addr - 16` is one of core's string headers, else reads to the NUL.
- `str == str` on two `Ptr(U8)` operands compares contents (:1650-1698).
- Raw Vecs from `p_vec_new` are real 40-byte headers in simulated memory
  with element codes (`interp_vec_ld/st`, :162-209). The `p_vec_*` builtins
  are the interpreter's own code over that layout (:3370-3520, :4574-4800),
  not core's `Vec_*` as AGENTS.md rule 16 describes for the other backends;
  `tests/bootstrap/test_vec_builtins.fd` agrees with them (probe).
  Programs that call `v.push(x)` run core's `Vec_push` as IR.

### Which calls are builtins (`is_runtime_func`, :2988)

Order matters:
1. Any `extern` function of the module.
2. **Forced builtins**, chosen even when core or the program defines a Fides
   function of that name: `print`, prefixes `p_vec_`, `p_map_`, `p_string_`,
   `p_json_`, `p_tensor_`, `p_simd_`, `p_socket_`, `p_http_`, `p_db_`,
   `p_file_`, `p_dir_`, `p_process_`, `p_thread_`, `p_atomic_`, `p_mutex_`,
   `p_shared_`, `p_future_`, `p_grad_`, `__ecs_`, `p_dp_budget_`, `p_tp_`,
   `p_checkpoint_`, `p_volatile_`, `p_signal_`, and the names `p_mkdir`,
   `p_rmdir`, `p_rename`, `p_is_dir`, `p_getpid`, `p_str_concat_int`,
   `p_str_append` (:2998-3028).
3. If a Fides function of that name exists, it runs as IR (:3033).
4. Fallback builtins for names nothing defines: `p_sb_*`, PRNG/noise,
   print, alloc/free, string conversions, clocks, `p_memset`, zeroize
   (:3037-3074).

`call_runtime` (:3087) tries `call_tensor` (:7039) first, then its own
chain, then `call_runtime2` (:5937). About 270 distinct builtin names are
handled. An unknown name now **panics** (:5114, "Unknown runtime function");
an unknown `extern "C"` function goes to `Interpreter_libc_call` first and
otherwise panics with "FFI function ... is not available in the
interpreter ... build it with `fid ccompile`" (:5096-5111).

### Emulations

- **libc** (`Interpreter_libc_call`, :7412): `getenv`, `printf`/`puts`,
  `system` (through `p_process_run`, output copied), `exit`, `getpid`. A few
  more C names are in `call_runtime2` (`abs`, `sqrt`, `sin`, `pow`, `strlen`,
  `strcmp`, ... :6203+). Nothing else from C.
- **Signals** (:3220-3238): `p_signal_register/raise/check` keep two global
  bit masks (`g_interp_sig_registered/pending`, :7447). Only signals the
  program raises itself; no OS signal is ever delivered; numbers 0-63.
- **Threads, futures, executors** (:5939-6000): `p_thread_spawn` and
  `p_executor_spawn` run the function to its end at the spawn
  (`Interpreter_run_eager`) and keep the result; join/poll return it.
- **Coroutines / tasks** (`p_co_*`, :5992-6021): one call stack, so
  `p_co_go` runs the task to its end at once; `p_co_yield` and `p_co_sleep`
  do nothing; `p_co_wait_fd` says "ready"; `p_co_done` is always 1.
- **Green threads** (`p_gt_*`, :3849-3936) over
  `compiler::green_thread`'s scheduler; `p_gt_run_all` runs each queued
  thread to its end in spawn order.
- **Channels** (:6162-6201): an unbounded queue per channel (the capacity
  is stored, never enforced). `recv` on an empty channel returns the
  sentinel `-9223372036854775807` instead of blocking.
- **Autograd** (:6837-7017): a real tape now (`ag_tape`), `GradBegin`
  clears it, `Backward` runs `Interpreter_run_backward`; `p_grad_get` /
  `p_grad_zero` are live in `call_tensor` (:7162-7177). Checkpoint mode keeps
  inputs for relu/sigmoid.
- **Tensors / KV cache / quantization / DP noise**: `call_tensor` plus the
  `p_tensor_*` block, on `compiler::tensor` (`Interpreter_alloc_tensor`,
  :5414). Sigmoid/softmax use the real `exp` (`interp_tensor_sigmoid`,
  :7221) because tensor.fd's `tensor_exp` is a short series.
- Also emulated: JSON, sqlite-like `p_db_*`/`p_sqlite3_*` rows, ECS, string
  builders, TLS keys, mutexes and atomics (single thread), `dl` handles,
  simulated `p_net_*` futures, `p_phys_*`/`p_cr*`/`p_serial_*` stubs,
  persist variables (in memory only), transactions (deep snapshot).

### What `fid run` cannot run

- **`extern "C"` calls** other than the libc list above: they panic with
  the FFI message. This includes **`sysconf`**: `use sys; sys_cpu_count()`
  panics at `stdlib/sys.fd:49` (verified live). Same for `sys_mem_*`,
  `sqlite3_*` through FFI, SDL, any library binding.
- **Interleaving tasks.** A program whose tasks must take turns gets a
  different result. Probe: two `task_go` workers that log, `task_yield()`,
  log again: `fid run` gives `1122` (each task ran to its end); the
  scheduled runtime interleaves them (expected `1212`, not run natively
  here). A consumer task that waits for a producer started after it sees an
  empty channel and the sentinel value.
- **Blocking**: `p_co_sleep`, channel recv, `p_co_wait_fd` never wait.
- **Real signals, real threads, inline asm** (`Inst::Asm` → `Int(0)`,
  :2426), anything that reads machine state (`p_cr*`, ports).
- **Use-after-free detection**: not here; `fid run --leaks` /
  `--checked-alloc` go through C.

## Findings

Status key: FIXED, STILL TRUE (current line), CHANGED.

### Verified live (probes with s18.exe, 2026-11-09)

1. **FIXED (was HIGH) — unknown builtin fabricated `Int(0)`.** Now
   `Panic("Unknown runtime function: …")` (:5114), FFI gets its own message.
   Probe: `sys_cpu_count()` panics with the FFI message.
2. **FIXED (was HIGH) — checked arithmetic dropped.**
   `Interpreter_eval_int_binop` (:2622-2677) checks overflow at the
   operation's width, wraps under `@wrapping` (`cur_wrapping`, set per call
   at :1339). Probe: `i64::MAX + 1` → "Integer overflow on addition: … at
   line 3".
3. **FIXED (was HIGH) — `Store` always wrote 8 bytes.** Stores now size by
   the pointee/value type (:1958-1969). Probe: struct `{a: i8, b: i8, c:
   i64}`, `s.a = 100` leaves `b == 2`, `c == 3`.
4. **FIXED (was HIGH) — `StructCopy` aliased.** It copies `type_size`
   bytes now (:2523-2545); struct `Store`s with a struct type copy too
   (:1862-1880, :1945-1957). Probe: `var t = s; t.b = 7` leaves `s.b == 2`.
5. **FIXED (was MEDIUM) — Vec/HashMap printing.** `Print` formats through
   the program's `Vec_to_str` / `HashMap_to_str` (:2346-2361). Probe:
   `[1, 2, 3]`, `{a: 1}`.
6. **FIXED (was HIGH) — channels were stubs.** Real queue (:6162-6201);
   `tests/bootstrap/test_channel.fd` and `test_channels_select.fd` run.
7. **FIXED (was MEDIUM) — no autograd tape.** Tape and backward pass exist
   (:6837-7017); `tests/bootstrap/test_autodiff_grad.fd` runs and prints
   the expected gradients.
8. **NEW, MEDIUM — `printf` ignores its format.** `Interpreter_libc_call`
   prints one argument as plain text (:7421-7426). Probe:
   `printf("value=%d\n", 42)` prints `value=%d`; a native build prints
   `value=42`.
9. **NEW, MEDIUM — tasks and coroutines run eagerly** (:5992-6021). Probe
   result `1122` above. This is by design (one call stack) and is stated in
   `stdlib/task.fd`'s header, but `fid run` output differs from the binary
   for any program that depends on the order tasks run in.
10. **NEW, HIGH — `p_gt_yield` twice in one green thread loses the next
    thread.** `GreenThreadScheduler_yield_current` (green_thread.fd:114)
    pops the next thread from `ready_queue` and marks it RUNNING, but never
    runs it; `p_gt_run_all` (:3872-3916) then skips any thread that is not
    READY/SUSPENDED. Probe: `w1` yields twice, `w2` never runs:
    `is_done(b) == false`, `get_result(b) == 0`. With one yield the queue
    happens to hand back the same thread, so the selfhost test passes.
11. **STILL TRUE, LOW — `TryUnwrap` jumps to its second operand without
    looking at the tag** (:2334-2336). Not reachable today: `lower.fd`
    never emits `Inst::TryUnwrap` (`?` lowers to a match). Probe: `f(x)?`
    on an `Err` returns the error correctly.

### From code inspection

12. **STILL TRUE, MEDIUM — `debug_mode` is a dead store.** Set at :896 and
    by `Interpreter_set_debug_hook` (:5593-5595), read nowhere. No hook is
    stored or called per instruction, so `fid veil` collects no coverage
    (see veil.md) and debugger.fd could not step.
13. **NEW, MEDIUM — memory only grows.** No reuse after free; every
    `Value::Str` → address conversion allocates a fresh block (:435,
    :1409, :1591, :1984). Long loops over string-taking core functions grow
    the simulated heap without bound; `--max-memory` only counts at
    `Alloca` (:1729), so heap growth is not limited.
14. **STILL TRUE, LOW — no use-after-free or use-after-region check.**
    Loads/stores do not consult `blocks`; `region_stack` holds names only.
15. **CHANGED, LOW — txn rollback.** `TxnBegin` now deep-copies memory and
    the opaque table (:2475-2481; was a shared-buffer copy). Rollback runs
    in the `Panic` arm of every `run_function` the panic passes through
    (:1501-1511), so each frame pops one more snapshot; harmless today
    because a panic ends the program except inside `p_gt_run_all`, which
    logs and continues.
16. **STILL TRUE, LOW — `PersistLoad/Save`** keep values in memory for the
    run only; `Interpreter_save_persist_state` / `with_persist_file` are
    no-ops (:5598-5606).
17. **STILL TRUE, INFO — `Inst::Asm`** → `Int(0)` / `Continue` (:2426-2432).
18. **FIXED — `U32` load sign-extended.** Now masked (:1789-1791).
19. **STILL TRUE, INFO — out-of-range byte load → 0, store below 0 →
    ignored** (:264-280). A store past the end grows memory instead.
20. **STILL TRUE, INFO — `str == int`** is true only for `""` and `0`
    (:2787, :2841); matches Rust.
21. **NEW, INFO — dead duplicate handlers.** `call_tensor` runs first
    (:3096) and answers (or panics for) these names, so the later copies in
    `call_runtime` are unreachable: `p_grad_get` / `p_grad_zero`
    (:3939-3944), `p_tensor_get` / `p_tensor_set` (:3975-3992),
    `p_tensor_add/sub/mul/scale/matmul/sum/mean/relu/sigmoid/softmax`
    (:4054-4144), `p_tensor_to_json` (:4288). `p_str_concat_int` is listed
    twice in `is_runtime_func` (:3027, :3058).
22. **NEW, INFO — stale comments.** :799-800 ("advanced features … stubbed
    or omitted"), :3085 ("advanced builtins return defaults"), :2504
    ("Autograd (stubbed)"), :5592 ("no-op"), :5296 ("Vec is 32 bytes"; it
    is 40) no longer describe the code.
23. **CHANGED, INFO — side-table lookups.** The opaque/dyn/tensor tables are
    hash tables now; `vec_side_table` is still searched linearly in
    `Interpreter_lookup_vec_value` (:5258-5293).
24. **INFO — inherited from Rust: wrong gradients for `mul` and `matmul`.**
    `run_backward` passes `grad_out` unchanged to both inputs of `mul`
    (:6956-6958) and all-ones to both inputs of `matmul` (:6965-6967);
    src/interp.rs:2337-2350 does the same ("without forward values, use
    identity"). Add/sub/scale/relu/sigmoid/sum/mean are right.

## Fix order

1. Green-thread yield (finding 10): make `p_gt_yield` not pick a next
   thread, or make `run_all` run the RUNNING one.
2. A real per-instruction hook (finding 12) so `veil` can count coverage.
3. `printf` formatting (8), or panic with the FFI message instead of
   printing the format string.
4. Free materialized strings / reuse freed blocks; count heap growth
   against `--max-memory` (13).
5. `mul`/`matmul` gradients using the forward values the tape could keep
   (24).
6. Remove the dead handlers and stale comments (21, 22).

## Verification state

- Read on 2026-11-09: whole of :1-3100 (memory, tables, run loop,
  `execute_inst`, binops, `is_runtime_func`), the dispatch heads and the
  autograd/tensor/libc parts; the middle of `call_runtime` (:3100-5080) by
  function and builtin name.
- Probes (scratch files, `s18.exe run`): overflow, i8 store, struct copy,
  `sysconf`, tasks with yield, Vec/HashMap print, `?` operator, green
  threads with one and two yields, `printf`/`puts`; repo tests
  `test_autodiff_grad`, `test_channel`, `test_channels_select`,
  `test_vec_builtins`, `test_tensor`, `test_kv_cache`, `test_quantization`,
  `test_tensor_shape_vec` (all exit 0).
- Not re-checked: the full selfhost/bootstrap suites under `fid run`.
