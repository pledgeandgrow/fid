# pe_ir.fd — IR → x86-64 machine code for native Windows executables

> **New 2026-11-09** (the module did not exist at the 10-22 audit). Written
> from the current source and live probes (`s18.exe`, `release/fid.exe`,
> test files in a scratch folder). Companion files: pe_ir_enc.md (encoder,
> imports, PE writer) and pe_ir_stubs.md (runtime stubs, Fides runtime,
> reachability).

> **Nov 21:** 9,010 lines (4,667 on Nov 9). It now imports `ast_ser`,
> `cparser`, `inc`, `ir_ser`, `lexer`, `parser`, `pdb`, `pgo` and `token`
> as well. `compiler.fd` `build_executable` is gone (MI18, Nov 17), so the
> MEDIUM "`build --native --target windows` crashes" finding no longer
> applies (`--native` is the machine layer). Debug builds carry DWARF line
> tables and a PDB (`emit -g`, DB3; pdb.md); cold blocks follow a
> `--profile-use` profile (pgo.md); `emit --boot` builds a Multiboot kernel
> here (`PEIR_BARE_BOOT`, Nov 15). The HIGH concurrency finding below was
> not re-checked. New on Nov 21, all fixed since (below and in c082ee6):
> a dead store in a partially unrolled loop (2g.80), `--leaks` executables
> crashed at once (2g.81), an f32 result cast `as i64` gave i64::MAX
> (2g.95), test_simd_i64 crashed in its `wrap:` case (2g.96, fixed in
> pe_ir_stubs), mutual tail calls overflowed the stack (2g.98),
> `FID_PROF=folded` stacks started at a wrong root (2g.101), wrong local
> values under `emit -g` (2g.104). Still open: fixed arrays past ~28 KB
> passed by value (2g.23, second entry). Fixed: `str_find`/`str_contains`
> needed `memchr` (2g.58, Nov 16). Line numbers below are from Nov 9.
>
> **Nov 21 (5fb5516) — 2g.80, 2g.81, 2g.95 and 2g.98 fixed.** 2g.81:
> `pe_ir_needed_fns` (:168) adds `pert_leak_malloc`/`pert_leak_free`/
> `pert_leak_exit` and everything reachable from them to the wanted roots
> under `--leaks` — `nat_frame_ret`/`nat_frame_next` had stayed `ud2`, so
> every `--leaks` image stopped with an illegal instruction at its first
> allocation. RL16: `__fid_entry` joins the nat tables as the COFF
> symbols' name for the entry point — a return address in it was named
> after the function placed before it, and `--leaks`' stack walk took the
> entry's blocks for the program's. 2g.95 (MI17): `ConstFloat` for `F32`
> stored the low 32 bits of the f64 pattern (`0.0` for 2^63), so the
> range checks of a saturating f32 `as i64` sent every value to
> i64::MAX/MIN; it now emits `p_f32_to_bits` (:4237). 2g.98 (OP13):
> `peir_tail_via_slot`/`peir_slot_load_return` (:2478/:2611) recognise a
> call in tail position through a result slot — `store call -> alloca`,
> `jump B`, and B (through blocks that only jump on, ≤8 hops) starts
> `load slot; return` — the shape the inliner gives `return g(x)`; the
> emitted jump skips the store and jump (`tail_skip = 2`), and mutual
> recursion no longer overflows the stack. 2g.80: `peir_global_ra`'s
> live-into-block lower bound is `bstart - 1`, not `bstart` — a value
> defined in a block laid out later (unrolled copies, chained backwards)
> had started exactly there and shared a register while live. Nov 21
> (c082ee6): 2g.101 and 2g.104 fixed — the folded profile roots at
> `__fid_entry` (`pe_pf_entry_b` in pe_runtime/conc.fd, SL15's report
> clock too), and `peir_dbg_live` counts only the instructions in a
> function's blocks (an inst the mid-end detached showed its stale slots
> for main's locals; FID_PE_DEBUG_RA=1 marks parameters no-register so
> they keep their frame slots). Still open: 2g.23 (second entry).

- **Path:** `compiler/pe_ir.fd`
- **LOC:** 9,010 (2026-11-21); 4,667 on Nov 9 (INDEX.md then said 3,102). Ported from `src/pe.rs`
  (`PeStackFrame`, `PeirGen::generate`, `gen_function`, `gen_inst`,
  `gen_int_binop`, `gen_float_binop`), then grown well past it (register
  allocation, inline Vec calls, loop rotation, bare/ELF output).
- **Role:** The backend of `fid emit` / `fid build` on a Windows host: SSA IR
  straight to machine code and a PE32+ file, no assembler, no C compiler. Also
  the flat bare-metal binary (`--bare`) and a no-stdlib static Linux ELF
  (`--elf`, `--target linux`).
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `pe_ir_enc`,
  `pe_ir_stubs`, `checks` (counted-loop check removal, `peir_inst_operands`,
  `peir_zeros`).

## Callers

| Caller | Site | What |
|--------|------|------|
| `main.fd` `cmd_build` (`build`, `emit`) | :3115-3194 | Windows default: `pe_ir_generate_pe` (:3168) |
| `main.fd` `cmd_build` | :2946-3001 | `--bare`: `pe_ir_generate_flat(ir, panic_handler)` (:2980) |
| `main.fd` `cmd_build` | :2907-2943 | `--elf` or `--target linux`: `pe_ir_generate_elf` (:2924) |
| `compiler.fd` `build_executable` | :2210-2217 | `build --native --target windows\|x64-windows\|x86-64-windows`: `iropt_native` then `pe_ir_generate_pe` |
| `compiler.fd`, `lower.fd` | :40, :35 | `use compiler::pe_ir`; lower.fd uses no pe_ir symbol (its `peir_inst_operands` is in checks.fd) |

`build` and `emit` are the same command (`main.fd:148-149` → `cmd_build`).

## How `fid emit` builds a Windows PE

1. `cmd_build` picks the path. `windows_pe` is true on a non-POSIX host when
   none of `--bare`, `--boot`, `--native` is given and the target is not
   arm64/aarch64/riscv64 (main.fd:3115-3116). A target containing `linux`,
   or `--nat`, goes to nat_ir first (:3010-3108); `--elf`/`--target linux`
   and `--bare` go to pe_ir's other two entry points.
2. `set_pe_runtime_merge(true)` (:3117): the front end merges
   `stdlib/pe_runtime/{base,conc,tensor,data}.fd` into core before the user
   program is parsed (compiler.fd:1150-1200; see pe_ir_stubs.md).
3. Incremental stamp: `<out>.fides-stamp`; an up-to-date output is not
   rebuilt (`--no-cache` / `FIDES_NO_CACHE` turn it off) (:3120-3131).
4. Front end → `lower` → `iropt_native` unless `--no-opt`/`--debug`
   (:3157-3162); `FID_VERIFY=1` runs the IR verifier before and after.
5. `pe_ir_generate_pe` (:4640): refuses a program with no `main`, runs
   `PeirGen_generate`, then refuses the program if any function reachable
   from `main` calls something nobody defines (the error names each one).

`PeirGen_generate` (:419-915):
- string constants (sorted), `link` DLL imports a reached `extern "C"` needs
  (:470-489), a warning for a DLL not on this machine (:490-507);
- `.rdata` (strings, import tables), then globals at their full type size
  (float bits through `p_f64_to_bits`, a `str` global patched to the string's
  address) (:532-605), then the 360-byte allocator state, 64-byte aligned
  (:607-619);
- labels for every function, every runtime stub (`pe_runtime_stub_names`
  :363), the machine primitives, the stub deps, C thunks for kernel32/msvcrt
  externs, `pert_c_*` overrides, stubs returning 0 for externs of a missing
  local header, C adapters for callbacks (:621-732);
- code for each function reachable from `main` (`PEIR_REACHABLE_ONLY`), and
  of the Fides runtime only what is needed; then runtime stubs, arithmetic
  panics, primitives, thunks, adapters, the DLL loader, the entry point, and
  any function referred to but not yet generated (`PeirGen_gen_on_demand`
  :1414) (:762-845);
- resolve labels and RIP references, patch import RVAs and `str` globals,
  write the PE (`pe_build_file`) (:847-914). `--bare` instead returns
  `[code][0xCC to 4 KB][data]` loaded at `g_pe_bare_load` (:850-881).

`PeirGen_gen_function` (:922-1407):
- per-block liveness, slot reuse for block-local temporaries
  (`peir_recyclable` :3640), permanent slots for the rest;
- whole-function linear-scan register allocation (`peir_global_ra` :1834,
  `peir_ra_scan` :1724): rbx, rsi, rdi, r12-r15 for any value; r10/r11 and
  xmm2-xmm5 for values no call crosses; xmm6/xmm7 for f64; locals (allocas)
  whose every access is a whole word; copy coalescing; spills by loop-weighted
  use count. A register value's "slot" is `PEIR_REG_SLOT + r`, so the
  emitting code is the same for both;
- callee-saved registers saved in the frame (xmm6/7 as 128 bits), one
  outgoing-argument area per frame, a stack probe for frames over 4 KB
  (:1285-1301);
- loop rotation (the header's test copied to the back edge, :1368-1396),
  compare+branch fusion (`pending_cmp_cc`), overflow checks with shared panic
  labels (`PeirGen_gen_arith_panics` :4430), division by a constant without
  `idiv` (:4304-4358), checks removed in counted loops (`peir_unchecked_ops`,
  checks.fd:109);
- `Vec_len`/`Vec_get`/`Vec_set`/`Vec_push` inlined with the slow cases
  (other element size, shared buffer, owned elements, full buffer) on cold
  paths after the function (`PeirGen_inline_vec_call` :4080,
  `PeirGen_emit_cold` :4199);
- interrupt handlers (bare): all GP registers saved, error code popped.

`gen_inst` handles every IR instruction; `Asm` is encoded only in bare builds
(`pe_asm_encode`, errors collected in `g_pe_asm_errors`); in a Windows build
it gives 0 (:3577-3618). `RegionBegin/End` do nothing.

## Switches

| Name | Value | Effect |
|------|-------|--------|
| `PEIR_REACHABLE_ONLY` (:1410) | true | only functions reachable from `main` (or referred to) get code |
| `PEIR_GLOBAL_RA` (:1632) | true | whole-function allocator; false = older per-block scheme |
| `PEIR_COALESCE` (:1634) | true | a local's loads/stores share its register |
| `PEIR_VOLATILE_RA` (:1636) | true | r10/r11, xmm2-5 for values no call crosses |
| `PEIR_XMM_RA` (:1638) | true | f64 values and locals in xmm registers |
| `PEIR_ROTATE` (:1640) | true | loop rotation |
| env `PE_DUMP_FN` (:930) | unset | prints each function's code offset |
| `do_opt` (`--no-opt`, `--debug`) | on | `iropt_native` before pe_ir |

## Findings

### Verified live

#### HIGH — the current build refuses every threaded or async program

With `s18.exe` (built 16:24 from the working copy), `emit` of `test_threads`,
`test_channel`, `test_async` and `test_async_float_struct` fails: "PE backend:
not available in native Windows executables: pe_ctx_swap, pe_ctx_tramp_addr,
pe_k_gqcs_ex, pe_k_iocp_create, pe_k_vfree". Same with `--no-opt` (not
iropt). `release/fid.exe` (13:47) emits `test_async` and its output equals
`fid run`. `pe_ctx_swap` gets a label only through `pe_stub_deps`
(:652-661) and the `pe_k_*` ones only as C thunks (:679-687), so a module
whose functions or extern signatures were pruned before pe_ir would give this.
The working copy is moving the reachability code from pe_ir_stubs.fd into
ir.fd (`IrModule_dead_function_elimination`, ir.fd:2344) and changing
lower.fd right now. Root cause not found.

#### MEDIUM — `build --native --target windows` crashes before pe_ir

`build_executable` lowers to L1/L2, runs `optimize_l2`, and builds L3
(compiler.fd:2142-2152) even for the Windows target, whose branch then uses
only the IR (:2210-2217). Probe: the PIR optimizer panics in
`pass_jump_thread` (Vec index out of bounds). With `--no-opt` the build
works (exit 42). The default `build --native` (target `x86-64`) dies the
same way on its ELF path.

#### Works (s18.exe, output equal to `fid run`)

`hello`, `test_vec_builtins`, `test_file`, `test_json`, `test_tensor`,
`test_sqlite` (winsqlite3.dll through the DLL loader). A double `p_free`
stops with "Panic: Double free", exit 1. `--bare` and `--elf` produce
binaries for a small program (not run: no QEMU/Linux here).

### Code inspection

#### LOW — the refusal message points at the frozen compiler

`pe_ir_generate_pe` says "(build with `fidesc ccompile` instead)" (:4663).
`fidesc` is frozen and lacks generics and other `fid` features; `fid
ccompile` is the working alternative.

#### LOW — `TryUnwrap` emits a `nop`

:3573-3576 "Not fully implemented". `lower` does not emit `TryUnwrap` today
(lower.fd only matches it, :3830), so this is latent.

#### LOW — a Windows build gives 0 for `asm!`

:3614-3618: no error and no warning; the template is silently dropped.

#### INFO — dead or stale bits

- The `p_vec_len` inline at :3182 never fires: `PeirGen_resolve_call` maps
  `p_vec_len` to `Vec_len` first (pe_runtime_alias), and `Vec_len` is
  inlined by `PeirGen_inline_vec_call`.
- `Inst::Panic` skips the call when `p_panic` has no label (:3566-3569);
  the label always exists (:366).
- The stack-probe comment (:1286-1289) still says "one slot per IR value, no
  reuse".
- `PE_DUMP_FN` is read with `p_getenv` once per generated function (:930).

## Fix order

1. Find why the working-copy build drops `pe_ctx_swap` and the conc.fd
   kernel32 thunks (compare `module.functions` / `extern_functions` reaching
   pe_ir in `s18.exe` and `release/fid.exe`).
2. In `build_executable`, skip L1/L2/L3 and the PIR optimizer for the
   Windows target.
3. Point the refusal message at `fid ccompile`.
4. Make `asm!` in a hosted build a compile error (or a warning).

## Verification state

- Live probes listed above (Nov 9). The full suite has not been run through
  `fid emit` with this compiler.
- pe-roadmap.md items 88-89 (speed targets) not re-measured.
