# File Analysis: compiler/interp.fd

**Run:** 25/52 | **LOC:** 3,967 (src/interp.rs: 6,358, ~62%) | **Status:** LIVE — the `run` path


> C3/C4 fixed 2026-10-23 (v3): checked add/sub/mul via pre-checks + @wrapping honored (`is_wrapping_func` live); MIN/-1 div/mod edge; txn rollback now deep-copies Memory + restores opaque_values.

## Where it's called

- `compiler/compiler.fd:1604,1626,1647` — `interpret_ir_module` / `interpret_ir_module_with_options` (the `fidesc run` execution path, `--max-recursion`/`--max-steps`/`--max-memory` options)
- `compiler/hotreload.fd:333` — `interpret_ir_module` (but hotreload itself is dead, run 23)
- Imported by `debugger.fd` (`set_debug_hook` → dead `debug_mode` flag, run 15) and `veil.fd` (coverage instrumentation)

The reference oracle for the interp leg of the selfhost test matrix. Core execution model is a faithful port: `Value`/`Memory`/`ExecOutcome`/`RunResult`, side tables (`vec_side_table`, `opaque_values`, `array_bounds`), `__env` dummy-param insertion (:751-776, same as Rust), call-site line tracking, per-inst step/recursion/memory limits, panic stack traces. Instruction coverage is **1:1** — all 42 `Inst` variants handled (Fides adds `Some`/`None` arms Rust lacks).

The divergence is not *what* it handles — it's how much of the semantics inside each handler, and the builtin surface, was ported.

## Findings

### HIGH — ~100 builtins still missing, and the miss is still silent (updated 2026-10-23)

`is_runtime_func` (:1843-1908) claims the same prefix families as Rust (`p_json_`, `p_tensor_`, `p_simd_`, `p_socket_`, `p_http_`, `p_db_`, `p_net_`, `p_thread_`, `p_atomic_`, `p_mutex_`, `p_shared_`, `p_future_`, `p_grad_`, `p_dp_`, `p_tp_`, `p_checkpoint_`, `p_volatile_`, `p_sb_`). **Since this analysis: `p_tensor_*`, `p_quantize_*`, `p_kv_cache_*` are now wired** to `compiler::tensor` (~35 call sites at :2701-2987, via `Interpreter_alloc_tensor`). Still missing: channels (see below), autograd/`p_grad_*`, simd, socket, http, sqlite3/db (~15 fns), net, executor/future/task, dl, hardware access (`p_inb/p_outb/p_hlt/p_cr0-4/...`), atomics, mutexes, shared mem, rc, regex, `p_system`, `p_getenv`, `p_exit`, `p_argc/p_argv`, `p_f64_to_bits`/`p_f64_from_bits`, checkpoint, most threads.

The killer is the fallthrough (:3425-3433): unhandled → **`ExecOutcome::Value(Value::Int(0))`** with only a `--verbose` eprint. Rust's fallthrough is `ExecOutcome::Panic("Unknown runtime function: {name}")` (src/interp.rs:6165). Every missing builtin fabricates `0` instead of erroring — `f64_to_bits(x)` → 0, `getenv` → 0, `socket_listen` → 0. Programs "run" and produce wrong results.

### HIGH — explicitly-stubbed builtins return plausible garbage (updated 2026-10-23)

Channels (:2674-2691, still stubs): `p_chan_create` → `Int(0)`, `p_chan_recv` → `Int(0)`, `p_chan_try_recv` → 0, `p_chan_is_closed` → `Bool(false)` — a recv loop consumes infinite zeros forever. `p_grad_get` → `Int(0)`, `p_grad_clear` → Void (:2693-2699). ~~`p_kv_cache_*`~~ — **now wired** to `KvCache_*` (moved into the tensor block, :2920+). Rust has a real `channels: HashMap<i64, ChannelState>` and `kv_caches` — only kv was restored; channels and the autograd tape remain stubs. `spawn` + channel code still silently computes nothing under `fidesc run`.

### HIGH — checked arithmetic dropped (F64 feature)

`eval_binop` (:1639-1648) does `a + b` / `a - b` / `a * b` unconditionally — "Checked addition (simplified: wrapping for now)". `Interpreter_is_wrapping_func` exists (:3487) but is **never called** — verified by grep. The caller's "Integer overflow on addition/subtraction/multiplication" panic paths (:1028-1030) are dead code. Rust uses `checked_add/sub/mul` → `Value::Void` → panic, wrapping only under `@wrapping` (src/interp.rs:2383-2415). `i64::MAX + 1` silently goes negative under `fidesc run` while it panics on every other backend.

### HIGH — `Inst::StructCopy` aliases instead of copying

:1619-1624 — `values.set(dst, get_value(values, src))` copies the *pointer Value* into dst's slot. Rust memcpys `type_size(ty)` bytes between the two addresses (src/interp.rs:1599-1615). Every struct-level copy in the interpreter makes dst and src share memory — writes through one are visible in the other. (The x64/C backends' real copies make this an oracle-vs-backend divergence.)

### HIGH — `Inst::Store` always writes 8 bytes for Int

:1172-1184 — `Value::Int` → `Memory_store_i64` regardless of type. Rust sizes the store from `store_ty`/deref-detection: i8/u8→1 byte, i16→2, i32/u32→4, f32→4 (src/interp.rs:1495-1570). `*p_i8 = x` in the interpreter writes 8 bytes into a 1-byte slot, clobbering the 7 bytes after it. Load reads the right sizes (:1093-1136) — the asymmetry makes it invisible on the immediate load and corrupts neighbors.

### HIGH — txn rollback is fiction

`txn_mem_snapshots.push(self.memory)` (:1593) copies the `Memory` *struct* — `data` is a `Vec` whose copy shares the backing buffer. Writes after the snapshot land in the same buffer the "snapshot" points to; on panic (:880-886) `self.memory = snap_mem` restores `next_alloc` but the bytes are already post-txn. `opaque_values` is never snapshotted ("simplified" comment). Rust clones memory + opaque_values + kv_caches (src/interp.rs:2113-2124, rollback :1218-1224).

### MEDIUM — autograd engine absent

Rust's interp has a real tape (`autograd_tape: Vec<AutogradEntry>`, `Backward` walks it in reverse computing grads, :2142-2200). The Fides `Interpreter` has **no tape field at all** — `GradBegin`/`GradEnd`/`Backward` → `Continue` (:1616-1618). Together with `p_grad_*` → 0, autograd under `fidesc run` produces nothing. Consistent with runs 4/8 — the semantic engine lives in codegen_c and Rust's interp; the port has neither.

### MEDIUM — vec side-table conversion stubs

`Interpreter_arg_to_vec_i64` (:3643-3657) and `args_to_f64_vec` (:3695-3697): the `Value::Vec` arm says "Simplified — returns empty for now" / "would extract elements". Any builtin receiving an interpreter-level `Value::Vec` gets an empty vec. The two parallel representations (`Value::Int(addr)` memory-model vs `Value::Vec(handle)` side-table) lose identity at every conversion — same family as the run-2 `&mut Vec` no-op.

### MEDIUM — Vec/HashMap printing dropped

`Inst::Print` (:1512-1555) has no `is_opaque_named` path — Rust calls `Vec_to_str`/`HashMap_to_str` to format (`println(v)` → `[1, 2, 3]`). Fides prints `ptr(0x…)`.

### MEDIUM — debug-mode safety checks dropped

`interp.debug_mode` is a dead store (run 15) — but in Rust it's live: `freed_addresses` UAF checks on Load/Store (:1402/:1468/:2053-2081) and `region_addresses` per-region tracking in Alloca (:1378-1383). The port has neither; `region_stack` stores names only (:1569-1576). Region alloc/free still works (names popped, memory bump-allocated forever) — but `use after region-end` is undetected.

### Inherited from Rust (same defects upstream)

- **`Inst::TryUnwrap` → unconditional `ok_block` jump** (:1509-1511) — never inspects the Result tag, `err_block` ignored. Rust identical ("Simplified — just continue to ok_block", :1907-1911). Sharper here: x64 and C backends DO check the tag — the oracle and the compiled backends disagree on `try`/`?` error paths.
- `PersistLoad` → default value, `PersistSave` → no-op (:1581-1588).
- `Inst::Asm` → `Int(0)`/`Continue` (:1560-1566) — asm dead end-to-end in interp too.
- `U32` load uses sign-extending `Memory_load_i32` (:1118-1120) — `u32 ≥ 0x80000000` loads negative. Rust identical (`load_i32(addr) as i64`, :1412).
- `Memory_load_byte`/`store_byte` OOB → silent 0/no-op — same as Rust.
- `Value::Str` binops: `str == int` is `len==0 && int==0` (:1733) — quirky but matches Rust.

## Adaptation notes (sound, not bugs)

- HashMap→Vec side tables with linear scan (documented in header) — O(n) lookups on `vec_side_table`/`opaque_values`/`map_values`; fine for tests, quadratic on big heaps.
- `Memory` byte-vector with 4096-byte growth steps — same model as Rust.
- `call_stack`/`call_site_lines`/`pending_call_site` for panic traces — faithful.
- Panic captures stack trace once (:873-879), restores `region_stack` depth on exit (:910-913) — correct cleanup.

## Verdict

The interpreter is a **working core-language executor with a fabricated perimeter**. Ints/floats/bools/strings/vec/map/structs-via-memory/calls/closures/dispatch all work — which is why 38/38 selfhost tests pass — but step outside that subset and every missing feature returns `0` instead of erroring. The port's single worst decision is the `Int(0)` fallthrough; it converts ~140 missing builtins from crashes into silent wrong answers, which is worse than not running at all for a reference oracle.

**Fix priority:** (1) change the unknown-builtin fallthrough to `Panic` — one line, immediately honest; (2) `StructCopy` memcpy — silent aliasing corrupts any struct-copying program; (3) store sizing via `store_ty`; (4) checked arithmetic reusing the dead `is_wrapping_func`; (5) `arg_to_vec_i64`/`args_to_f64_vec` real extraction. The 137-builtin gap is a port-size problem — implement by subsystem (chan/kv/grad real, hw/dl/net stubs with Panic).
