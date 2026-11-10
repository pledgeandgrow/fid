# pe_ir_stubs.fd — runtime stubs, Fides runtime hookup and reachability (pe_ir)

> **New 2026-11-09** (not in the 10-22 audit). Written from the source and
> live probes with `s18.exe` (see pe_ir.md). **The file is being edited by
> another session right now:** HEAD (3215fe7) has 7,071 lines; the working
> copy went 7,071 → 6,927 → 6,882 → 6,755 while this was written, moving the
> name-resolution and reachability helpers (`PeirStrPair`,
> `pe_strs_contains`, `pe_sort_strs`, `pe_stub_deps`, `pe_print_builtins`,
> `is_pe_runtime_fn`, `pert_name`, `pe_runtime_alias`, `pe_module_has_fn`,
> `pe_ir_c_override`, `pe_ir_resolve_callee`, `pe_ir_print_formatter`,
> `pe_ir_autograd_builtin`, `pe_ir_reachable_from`/`needed`/`reachable_functions`)
> into `compiler/ir.fd` (:2019-2340), next to the new
> `IrModule_dead_function_elimination`. Line numbers below are **HEAD's**.

- **Path:** `compiler/pe_ir_stubs.fd`
- **LOC:** 7,071 at HEAD (INDEX.md says 5,539); 6,755 in the working copy.
- **Role:** The `PeirGen` state (:144) and everything pe_ir emits that is not
  a user instruction: 79 `PeirGen_gen_*` machine-code generators (print,
  panic, allocator, strings, StringBuilder, files, argv, `p_system`, Vec and
  HashMap stubs, bare-metal stubs), the Fides runtime hookup (`pert_X`,
  primitives, C thunks and adapters, the `link` DLL loader), the entry point,
  and an Intel-syntax `asm!` encoder for bare builds (`pe_asm_encode` :7056).
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `pe_ir_enc`.
  It also uses `PeOffEnt` and `PEIR_PANIC_*` from pe_ir.fd without a `use`.

## Callers

| Caller | What |
|--------|------|
| `pe_ir.fd` | `PeirGen_*`, `pe_runtime_stub_names` labels, `pe_primitive_names`, `pe_stub_deps`, reachability |
| `main.fd` (:25 `use`) | `g_pe_checked_alloc` (`--checked-alloc`, :2816) |
| `iropt.fd` (:35 `use`) | `pe_ir_reachable_functions` (:101) |
| `nat_ir.fd` (:30 `use`) | `pe_runtime_alias`, `pert_name` (:876-881), `is_pe_runtime_fn`, `pe_ir_autograd_builtin`, `pe_ir_print_formatter` — the Linux / `--nat` backend resolves builtins the same way |

Reached from every pe_ir command (Windows `build`/`emit`, `--bare`,
`--elf`), and, through the shared helpers, from nat_ir builds and `iropt`.

## How the Fides runtime is merged and called

1. **Merge** (compiler.fd:1150-1200): when `g_pe_runtime_merge` is set
   (Windows `emit`/`build`, `build --native --target windows`, and every
   nat_ir build through `set_native_runtime`), the front end parses the
   runtime files and merges them into core before the user program
   (`merge_programs(rt_program, stdlib)`). pe_runtime: `base.fd`, `conc.fd`,
   `tensor.fd`, `data.fd`. linux_runtime (nat_ir): its own `base`, `conc`,
   `sys`, `math`, plus **pe_runtime's `data.fd` and `tensor.fd`**, plus
   `win.fd` for `--nat`. Files are found by `pe_runtime_path`
   (compiler.fd:965: `stdlib_dir()/<dir>/<file>`, legacy `src/<dir>` fallback
   that no longer exists). Not merged for `--bare`, `--elf`, `--boot`, or the
   C path.
2. **Resolution** (`PeirGen_resolve_call` :706, `pe_ir_resolve_callee` :517):
   a call to `name` runs, in order: the `pert_c_<C name>` override of an
   `extern "C"` function (putchar, puts: :501); `pe_runtime_alias(name)`;
   `pert_X` for builtin `p_X` or `__X` when the program has it (`pert_name`
   :451); else `name` itself (a machine stub, a C thunk, a program function).
   A `pert_*` stand-in takes a null `__env` first (`PeirGen_call_builtin`
   :723).
3. **`pe_runtime_alias`** (:462-485): every `p_vec_*` builtin runs core's
   code: `p_vec_new/push/len/get/set/pop/is_empty/clear/capacity/first/last/
   contains/truncate/clone` → `Vec_*`; `p_vec_insert/remove/reverse/extend/
   sort` → `vec_raw_*` (all in stdlib/core.fd:1540-1990). This is checked
   before `pert_X`, so no runtime file can override a Vec builtin.
4. **Pruning**: the runtime's own functions (`pert_`, `pe_`, `pet_`, `ped_`:
   `is_pe_runtime_fn` :444) get code only if `pe_ir_needed_functions` (:670)
   reaches them, and, with `PEIR_REACHABLE_ONLY`, only if reachable from
   `main` (pe_ir.fd:781-800). `pe_ir_reachable_from` (:573) follows calls,
   stub deps, print dispatch, autograd builtins, function references, string
   constants that name a function (thread/task entries) and, for a `dyn`
   call, every function ending in `_<method>`.
5. **Primitives and deps**: `pe_primitive_names` (:353: atomics, float bits,
   `pe_fn_lookup`, `pe_call_fn`) are machine code the runtime declares
   `extern "C"`; `pe_stub_deps` (:364) pairs machine code with the Fides
   function it serves (`p_thread_spawn` ↔ `pert_thread_spawn_v`,
   `pe_ctx_swap` ↔ `pe_ctx_switch`, ...). The task context switch is fixed
   bytes (`pe_ctx_swap_code` :387: callee-saved regs, xmm6-15 and the TEB
   stack fields).
6. **C calls**: `CThunk` (:51) moves Fides arguments into the Windows x64 C
   convention (floats as bits, structs by value: `pe_c_by_value` :76);
   `CAdapter` (:64) is the reverse for callbacks. `link` DLLs are loaded at
   start-up by `__pe_load_dlls` (`PeirGen_gen_dll_loader` :6147); a missing
   DLL or function gets a stub that stops the program with a message on
   first call.

## Other parts

- **Allocator** (`PeirGen_gen_malloc` :2806, `gen_free` :3062): size classes
  with per-thread free lists (TLS index in the allocator state, release H8),
  a header `PEIR_BLOCK_MAGIC | class`; `p_free` panics on a double free or a
  pointer that was never a block (live: "Panic: Double free"); large blocks
  from `HeapAlloc`. `--checked-alloc` (:2636): every block on its own pages
  ending at a guard page, freed pages made inaccessible.
- **Bare metal** (`PeirGen_gen_bare_runtime` :846): real interrupt, MMU, GDT,
  user-mode and COM1 stubs; bump `p_malloc` from `g_pe_bare_heap` (0x300000);
  print to COM1; panic calls the `@panic_handler` or prints and halts; any
  other stub traps (`PeirGen_trap_unplaced` :1042). With `g_pe_elf` the same
  code uses Linux system calls (mmap heap, write, exit_group).
- **Entry point** (`PeirGen_gen_entry_point` :6063): Windows: vectored
  exception handler (CPU faults become panics), DLL loader, `main`,
  `ExitProcess(main's result)`.
- **Hosted stubs** (`PeirGen_gen_runtime_stubs` :1060): the functions that
  "only pretended to work" (string search/replace/case/trim, vec sort/insert,
  map keys) are no longer generated; a call to one is refused by name.

## Switches

| Name | Value | Effect |
|------|-------|--------|
| `g_pe_checked_alloc` | false (`--checked-alloc`) | guard-page allocator |
| `g_pe_bare_heap` | 0x300000 | bare heap start |
| `g_pe_bare_load` | 0x10000 (0x401000 for `--elf`) | flat image load address |
| `g_pe_elf` | false (`--elf`) | bare code on Linux system calls |
| `g_pe_bare_panic_handler` | "" | the program's `@panic_handler` |
| `PEIR_ALLOC_STATE_SIZE` | 360 | allocator state bytes in .rdata |

## Findings

### Verified live

#### HIGH — threaded/async programs refused by the current build

See pe_ir.md: with `s18.exe` the labels for `pe_ctx_swap` (a stub dep, :364)
and the conc.fd kernel32 thunks are missing, so `test_threads`,
`test_channel`, `test_async`, `test_async_float_struct` are refused;
`release/fid.exe` builds `test_async` correctly. The move of this file's
reachability code into ir.fd is in progress; root cause not found.

### Code inspection

#### MEDIUM — the machine `p_vec_*` stubs are still emitted, with an old layout

`gen_runtime_stubs` (:1060-1152) still calls `gen_vec_new`, `push`, `len`,
`get`, `pop`, `set`, `is_empty`, `clear`, `capacity`, `first`, `last`,
`contains`, `truncate`, `clone`, and `pe_runtime_stub_names`
(pe_ir.fd:374-378) gives them labels. Every call resolves to core first
(alias), and no stub calls them by label, so they are dead bytes in every
exe. `p_vec_new` builds a 32-byte header (:3964-3986), not the 40-byte one
(AGENTS.md #25): anyone who wires a call to the label gets a broken Vec.

#### LOW — `p_map_*` stubs have their own HashMap layout

`gen_map_new/insert/get/...` build a map that is not core's 56-byte
HashMap. `lower` never emits `p_map_*` calls (only a name check at
lower.fd:382), so only a program that calls `p_map_*` by name reaches them,
and mixing such a map with core's `HashMap_*` would break.

#### LOW — stale comments

- :1155-1160 (working copy: about :839-844) says the bare-metal bodies "are not ported; the bare_mode guard
  panics". They are ported (`gen_bare_runtime` :846) and `--bare` builds.
- stdlib/pe_runtime/*.fd headers say `src/pe_runtime/` and that the backend
  has machine code for Vec (base.fd:1-12); both are out of date.

#### INFO — reachability over-approximates

Any string constant equal to a function name (:633-635) and every function
ending in `_<method>` for each `dyn` call (:646-654) are kept. Safe, but
executables carry more code than they run.

## Fix order

1. Fix the conc.fd refusal in the working copy (see pe_ir.md).
2. Delete the dead `p_vec_*` generators and their labels.
3. Either alias `p_map_*` to core's `HashMap_*` like Vec, or refuse them.
4. Fix the stale comments once the ir.fd move settles.

## Verification state

- Live (Nov 9, `s18.exe`): runtime merge works for base/data/tensor programs
  (file, JSON, tensor, SQLite outputs equal `fid run`); double free panics;
  conc.fd programs refused (regression vs `release/fid.exe`).
- pe-roadmap items 31-41 and 61-79 describe the Oct 30-31 state; see the
  report for which are stale.
