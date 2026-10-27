# codegen_c.fd — IR → C99 backend (5,877 LOC)

> **Re-verified 2026-10-23:** headline findings confirmed. The GetElemPtr "always elem 0" claim needs a functional test to fully prove (superficially `a + i*size` looks right — the missing struct-layout workaround is the suspected defect).

## Role

The **primary native backend**: `IrModule → C source` (with ~2.5K LOC of
embedded C runtime emitted as strings: PVec/PMap/string/channel/thread/
autograd/tensor/persist machinery). Port of `src/codegen_c.rs` (4,966 LOC).
This is the path every `ccompile`/`build`/`dump-c` flows through — the
highest-impact module analyzed so far, and correspondingly the most careful
(union type-puns for float returns, struct-memcpy stores, alloca sizing,
crash-stack traces, signal handlers).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd:1391-92` | `CCodeGen_generate(&mut cg, cached_module)` | cached-module C emit |
| `compiler/compiler.fd:1423-24` | `CCodeGen_generate(&mut cg, module)` | fresh-module C emit |
| `compiler/main.fd:1801-02, 1829-30` | `CCodeGen_generate(...)` | dump-c / ccompile commands |

Downstream: emitted `.c` compiled by a system C compiler — so every defect
below either produces invalid C or silently-wrong runtime behavior.

## Findings

### F1 — `GetElemPtr` on struct elements → every index is 0 — HIGH (inherited, inconsistent fix coverage)

`ir_type_size(IrType::Struct)` returns **0** (ir.fd:214 — "real size computed
during lowering"). `GetElemPtr` (:2636) uses it raw → `(char*)a + (i * 0)` →
**all indices land on element 0** for struct-typed arrays.

The file already carries the workaround in three places — `Alloca` (:2516),
`Store` (:2572), `StructCopy` (:3214) all look up `struct_layouts` — but
GetElemPtr was missed. Rust `elem_ty.size()` is also 0 (src/ir.rs:55) →
inherited root cause, but the port's fix coverage is incomplete.

### F2 — F32 `%` emits invalid C — MEDIUM (inherited)

`is_float_mod` (:2483-2494) only matches `IrType::F64`. `x % y` on `f32` →
`dst = l % r` → **C compile error** (`%` needs integers). Same gate in
src/codegen_c.rs:1630. Fix: include F32 → `fmodf(l, r)`.

### F3 — `Inst::Panic` message interpolated unescaped — MEDIUM (inherited)

:3170 `p_panic(\"" + message + "\")` — no `escape_c_string`. A panic message
containing `"` or `\n` produces invalid C (or worse, injects C code).
`ConstStr` escapes properly — Panic just doesn't call it.
Same in src/codegen_c.rs:2356.

### F4 — `sanitize_c_name` denylist gaps — MEDIUM

38 hardcoded renames (main→main_entry, malloc→p_malloc_user, C keywords…).
Missing: **`system`** (a user fn named `system` shadows libc inside
`p_system`'s `system(cmd)` → wrong call/recursion), `abort`, `signal`,
`fopen`, `write`, `read`, `send`, `select`, `alloca`, `stderr`, `errno`,
`index`, `basename`… Any user function with these names → collision with
runtime code. A denylist can't be complete — better: prefix ALL user fns
(`p_` convention already exists) or switch to collision *detection*.

### F5 — Thread-handle tables OOB — MEDIUM (inherited)

`p_executor_spawn`/`p_thread_spawn`: `idx = p_thread_handle_count++` into
`p_thread_handles[256]`/`p_task_results[256]`/`p_task_done_flags[256]` —
**no bound check** (the *entry* table is checked at :3786 `<2048`, handles
aren't). Spawn #257 → OOB write → silent heap corruption.
Identical in src/codegen_c.rs:2916.

### F6 — Windows pthread shim makes `spawn` synchronous — MEDIUM (inherited)

`_WIN32` shim (:3723-3739): `pthread_create` runs `fn(arg)` **inline** and
stores the return value as the "tid" — every spawned thread executes
synchronously inside `p_*_spawn`. Mutexes/conds are no-ops. Semantics
"work" (futures resolve on poll since already joined) but nothing is
concurrent — and a spawned fn that blocks on a channel the parent fills
**deadlocks** (parent is stuck inside spawn). Same in src:2813-2839.

### F7 — `bb0` label never emitted — LOW-MEDIUM

`gen_function` (:2394): labels only for `b > 0`. Any `Jump`/back-edge to
block 0 → `goto bb0` → **undefined label** C error. Loop headers are
normally non-zero so it rarely fires — until it does. Rust has the same
pattern (verify when this file is touched).

### F8 — Void DynCall misses silently — LOW-MEDIUM

`Inst::DynCall` (:3087-3096): `if(false) {} else if…` — for `void` returns
there's no `else` → unimplemented trait method on a type → **silent no-op**
(non-void arm has `else { abort(); }`). Also `str_ends_with(fname, "_"+m)`
makes *every* `Foo_method` a candidate for *any* trait's `method` — only
the `type_name` strcmp disambiguates; correct but fragile if two impls
share a struct name.

### F9 — Silent no-ops / dropped semantics — LOW-MEDIUM

- `Inst::Asm` → `/* asm */` + `dst = 0` (:3172-3178) — inline asm silently
  dropped (compounds the ast.fd clobber bug — asm is dead end-to-end).
- `PersistLoad` → always `default`; `PersistSave`/`TxnBegin/End`/
  `UnsafeBegin/End`/non-GPU `Region*` → nothing. `persist` never persists.
- `Inst::Backward(_)` → `p_autograd_backward(NULL)` — operand ignored; the
  C impl seeds the *last tape op* (sum/mean) instead of the named loss.
- `p_thread_wrapper` switch handles nargs 0-4; args 5-8 read by va_arg but
  never passed (:3803 `default` = f4).

### F10 — Runtime impl defects — LOW-MEDIUM (all inherited)

- `p_intern_str` stores the **caller's** pointer, no strdup (:3715) —
  intern table dangles if the caller frees/mutates the string.
- `p_chan_try_recv` on closed+empty returns `0` (:4105) — indistinguishable
  from a legit `0` value; blocking `recv` uses `P_CHAN_PENDING` instead —
  inconsistent sentinel.
- `p_dl_open` returns sequential fake handles (documented "matches interp").
- `p_map_*` is linear-probe (correct, O(n)).

### F11 — Misc — LOW

- `alloca()` needs `alloca.h`/MSVC `_alloca` on some toolchains.
- `find_inst` is O(n) per inst → O(n²) per function (same Vec-map style as
  the rest; fine until functions get big).
- `emit_main_wrapper` registers every non-internal function for
  `p_thread_spawn` by-name lookup — O(n) strcmps per spawn; fine.
- `emit_globals`: check `gen_global_runtime_expr` (:1915+) for non-escaping
  if user strings reach global initializers.

## What's good

- `Inst::Store` effective-ty chain (explicit ty → pointee → value ty) +
  per-width stores (:2568-2608) — careful.
- `ConstFloat` union-compound-literal preserves NaN/-0.0/Inf (:2424-2435).
- CallPtr name-dispatch with param-count + ret-type matching, abort on
  miss, honest fallback cast (:2893-3056).
- Crash machinery: vectored handler (Win) + SIGSEGV/BUS/ABRT/FPE handlers +
  call-stack printing + `p_call_stack_push/pop` per function.
- `escape_c_string` octal-escapes all non-printables — UTF-8-safe.
- String-const dedup table (`emit_string_consts`).

## Verification state

- Call sites + defects verified by direct read; **all runtime-impl issues
  (F5, F6, F10) confirmed identical in src/codegen_c.rs** — inherited, not
  port bugs. F1 is an *incomplete port fix* (siblings got the workaround,
  GetElemPtr didn't).
- Not executed E2E (parser blocked on ast.fd:344 `>>`; MSVC off PATH anyway).
