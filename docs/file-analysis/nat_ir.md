# nat_ir.fd — IR to native code: machine IR, register allocator, targets

Audited 2026-11-09 (new doc). Another session was editing this file during
the audit (5,548 lines when first read, 5,606 an hour later; `git diff`
showed about 240 uncommitted lines: WP5 indirect-call masks, WP8 leaf
frames, WP11 `@crashfree`, WP17 cold blocks). Line numbers below are from
the first read and shift.

- **Path:** `compiler/nat_ir.fd`
- **LOC:** 5,548 (first read, 2026-11-09)
- **Role:** the own native backend of `fid`. It lowers the IR (after
  iropt, rules and the e-graph) to machine code for AArch64, RISC-V 64 and
  x86-64, and writes a static Linux ELF or a Windows PE. Encoders and the
  file writers are in `nat_enc.fd`; the runtime is the Fides code in
  `stdlib/linux_runtime/`.

## Where it is called

| Caller | Site | What |
|--------|------|------|
| `compiler/main.fd` | `use compiler::nat_ir` (:44) | only importer |
| `compiler/main.fd` `cmd_build` | ~:3010-3110 | `nat_target_arch(target)`, `nat_generate_elf(mod, arch)`, `nat_generate_pe(mod)`; sets `g_nat_lines_file` / `g_nat_lines_names` for the line table |

CLI commands that reach it (`build` and `emit` are the same handler):

| Command | Target |
|---|---|
| `fid emit f.fd --target aarch64-linux` (also `arm64*`) | AArch64 ELF |
| `fid emit f.fd --target riscv64-linux` (also `riscv*`) | RISC-V 64 ELF |
| `fid emit f.fd --target x86_64-linux` (also `x64*`, `amd64*`) | x86-64 ELF |
| `fid emit f.fd --nat` (target empty, `*windows*`, or any non-linux target on a Windows host) | x86-64 PE |
| `fid emit f.fd --native --target arm64` / `riscv64` / `x64` | same as above (`--native` routes here when `nat_target_arch` knows the target) |

Not here: `--elf` / `--target linux` (pe_ir's no-stdlib ELF), `--bare`,
`--boot`, and `--native` with the default target `x86-64` (the hyphen:
`nat_target_arch("x86-64")` is 0, so that goes to the old PIR path in
`compiler.fd build_executable`).

Before it: `set_native_runtime("linux_runtime")` merges
`stdlib/linux_runtime/*.fd` (and `win.fd` for `--nat`), `g_cfg_os` /
`g_cfg_arch` set for `@cfg`, then `iropt_native`, `rules_run`, `egraph_run`
(skipped with `--debug`), with the IR verifier after each stage under
`FID_VERIFY=1`.

## What it does

- **Worklist from `main`** (WP1): only functions that `main` reaches (plus
  the panic stubs and functions named by a string constant, for thread and
  task entry by name) are compiled. A call to a function defined nowhere is
  collected and the build fails with "not available in native Linux
  executables yet: ...".
- **Architecture layer** (`ng_li`, `ng_add`, `ng_load`, `ng_call_label`,
  ...): one function per operation, with an AArch64, RISC-V and x86-64
  body. In recording mode each call appends an `MInst` instead of encoding.
- **Integer semantics as pe_ir and the interpreter**: overflow panics
  unless `@wrapping` (runtime `pe_*`/`pert_*` code always wraps), narrow
  results checked, division by zero and MIN / -1 panic. `checks.fd`
  (`peir_unchecked_ops`) removes checks on counted loops and proven ranges.
  Division panics carry " at line N" (per-line stubs).
- **Calling convention** (internal, nothing calls C): arguments in x0..x7 /
  a0..a7, on x86-64 rdi, rsi, rdx, rcx then the stack; result in argument
  register 0. `__env` first. A builtin `p_X` runs the runtime's `pert_X`
  with a null env. `Vec_len/get/set/push` are inlined (byte vectors and
  structs held inline included).
- **Machine IR (NG_MIR = true)**: registers < 64 are physical, >= 64 are
  vregs (IR value id + 64; four fresh scratch vregs per IR instruction).
  Scalar locals whose address is only loaded and stored are promoted to
  vregs (`ng_promotable`). After all functions are recorded,
  `ng_mir_finish` allocates and emits them callees first (post-order of
  the call graph, WP3).
- **Register allocator** (`ng_mir_alloc`): block liveness to a fixed point,
  live-range hulls, linear scan over caller-saved and callee-saved pools,
  move hints, copy coalescing (union-find), rematerialization of
  constants, spill weights 8^(loop depth). Pools: AArch64 x8-x11 +
  x19-x28; RISC-V t4 (x29) + s1, s2-s11; x86-64 rbx, r12-r15 only (no
  caller-saved pool). Spills go through three reserved scratch registers.
- **IPRA** (WP4): each function's clobber mask is recorded after
  allocation; a call to an already allocated callee clobbers only that
  mask. Unknown or recursive callees clobber the whole caller pool.
- **Allocation checker** (`ng_mir_check`, `FID_VERIFY=1`): exact backward
  liveness per instruction; fails the build if a definition or a call
  overwrites a register a live value holds.
- **Identical-function folding** (`ng_mir_key`, a double hash of the
  allocated MIR): later copies point their label at the first one.
- **Old direct path** (`FID_NAT_OFF=mir`, AArch64 / RISC-V only; x86-64
  always uses the MIR): `ng_regalloc` gives values callee-saved registers
  over hulls; encodes while lowering. So there are still two allocators.
- **Primitives in machine code**: `_start`, `nat_syscall` (x86-64 maps the
  generic Linux numbers through a table, `ng_x64_sysmap`), `nat_start_sp`,
  atomics, clone, thread pointer, signal entry, context switch, single
  instruction math. Windows: entry that builds a Linux-style start block,
  `nat_win_call(k, ...)` through an import table of kernel32, synch,
  bcrypt, ws2_32 and ntdll (`ng_win_imports`, order = `PE_W_*` in
  `win.fd`), TLS through the TEB, a vectored exception handler.
- **Debug data** (Linux only, `FID_NAT_OFF=debug` drops it): symbol table,
  DWARF `.debug_info`, `.debug_frame`, and `.debug_line` rows for the
  functions of the input file (DB1).
- **x86-64 xmm cache**: a chain of f64 operations keeps operands in
  xmm0/xmm1 instead of moving them again.
- **In the uncommitted working copy**: `FID_NAT_ON=crashfree` drops panic
  checks in `@crashfree` functions (WP11); `ng_call_mask_i` traces indirect
  call targets (WP5); leaf functions skip the frame (WP8); `ng_cold_last`
  moves segments that end in a trap to the end of the function (WP17).

## Switches

| Name | Effect |
|---|---|
| `NG_MIR` = true (const) | machine IR path; false = direct path |
| `NG_REGALLOC` = true (const) | direct path's allocator |
| `FID_NAT_OFF=mir` | direct path (AArch64 / RISC-V; ignored on x86-64) |
| `FID_NAT_OFF=promote` | locals stay in memory |
| `FID_NAT_OFF=ipra` | every call clobbers the full pool |
| `FID_NAT_OFF=fold` | no identical-function folding |
| `FID_NAT_OFF=hint` | no move hints |
| `FID_NAT_OFF=coalesce` | no copy coalescing |
| `FID_NAT_OFF=remat` | spilled constants are stored and reloaded |
| `FID_NAT_OFF=weights` | spill the range that ends last |
| `FID_NAT_OFF=vecinline` | Vec_len/get/set/push always call |
| `FID_NAT_OFF=unchecked` | every overflow and bounds check stays |
| `FID_NAT_OFF=panicline` | no " at line N" in division panics |
| `FID_NAT_OFF=xmmcache` | x86-64 floats always reloaded into xmm0/xmm1 |
| `FID_NAT_OFF=debug` | no symbols, DWARF or line table |
| `FID_NAT_ON=crashfree` | elide panic checks in `@crashfree` (working copy) |
| `FID_VERIFY=1` | allocation checker here; IR verifier in main.fd |
| `FID_NAT_DUMP=<fn>` | print that function's MIR with registers and ranges |

`rules`, `purecse`, `egraph`, `forward` are `FID_NAT_OFF` names read by
`rules.fd` / `egraph.fd`, not here.

## Findings

### Verified live

1. **INFO — the three Linux targets work on small programs.**
   `release/fid.exe emit p.fd --target x86_64-linux|aarch64-linux|riscv64-linux`
   built a loop + panic program; run in WSL (x86-64) and qemu-aarch64 the
   output, the panic trace and exit code 1 matched `fid run`. `return 3`
   from main gave exit code 3. `FID_VERIFY=1 FID_NAT_DUMP=check` printed
   the MIR and passed the checker on RISC-V.
2. **MEDIUM — the given probe binaries do not build these targets.**
   `s18.exe emit --target aarch64-linux` panicked "Integer overflow on
   multiplication" in `ng_mir_key`, which is `@wrapping`: that build of
   `fid` did not honour `@wrapping` on this function. `s18.exe emit --nat`
   (also `--target x86_64-windows --nat`) took the pe_ir path.
   `release/fid-dev.exe` (16:41) took the nat path for `--nat` but failed
   "the Windows layer of the runtime (win.fd) is not merged", and crashed
   in `rules_run` for the Linux targets. Both binaries were built while
   other sessions edit `rules.fd`, `win.fd` and `main.fd`; re-check with
   the next fixpoint binary before treating these as bugs here.

### Code inspection

3. **MEDIUM — WP8 leaf frames never happen (working copy).**
   `ng_mir_emit` sets `g.leaf = 0` when `al.frame > 0`, but `ng_mir_alloc`
   always makes `frame >= 16` (`if frame < 16 { frame = 16; }`). So every
   function keeps its prologue. If someone fixes the frame test, note that
   `g.leaf` is not reset after the last function, and `ng_prologue` /
   `ng_epilogue` also serve primitives emitted later.
4. **MEDIUM — WP17 `ng_cold_last` can break fall-through (working copy).**
   It moves any segment that contains `MO_TRAP` to the end of the function.
   Lowering elides a jump to the next block (`Jump`, and `Branch` whose
   then-block is next falls through). If the block after a fall-through
   holds a trap (an `Inst::Panic`, `Unreachable`, a DynCall with no
   matching type, a call to an undefined function), the fall-through edge
   then lands on whatever segment comes next. If the first segment holds a
   trap, the prologue moves away from the function's entry label. Today
   lowering emits panics as calls (no trap) and `Unreachable` / `Panic`
   rarely or never, so this is latent. Fix: only move a segment whose
   predecessor ends in an unconditional jump, epilogue or trap.
5. **INFO — WP5 indirect-call masks never resolve.** The only `MO_CALLR`
   is `CallPtr`, whose target register is loaded from the closure's word 0
   (`MO_LOAD`), so `ng_trace_label` returns -1 and the full mask is used.
   Safe, but the roadmap's [x] WP5 has no effect.
6. **LOW — two allocators.** `FID_NAT_OFF=mir` still runs `ng_regalloc`
   (callee-saved only, no checker) on AArch64 / RISC-V. Docs say "one
   allocator".
7. **LOW — file header is stale.** The comment at the top says AArch64 and
   RISC-V only and "every IR value has a stack slot"; x86-64 Linux,
   Windows and the MIR allocator are not mentioned.
8. **INFO — x86-64 has no caller-saved pool.** Values live only in rbx,
   r12-r15 (rax, rcx, rdx, r8-r11 are reserved scratch); IPRA has nothing
   to gain there.

## Fix order

1. Re-run the three Linux targets and `--nat` with the next fixpoint
   binary (finding 2).
2. Guard `ng_cold_last` against fall-through into a moved segment (4).
3. Decide WP8: allow `frame == 0` for leaves or drop the code (3).
4. Update the header comment (7).

## Verification state

- Read: header, NatGen, architecture layer samples, allocator, checker,
  emission, folding, `ng_mir_finish`, `ng_gen_function`, x86-64 and
  Windows primitives, `nat_generate_image`, `ng_cold_last`.
- Probes (scratch directory, not the repo): `release/fid.exe` builds of a
  loop/panic program and `return 3` for all three Linux targets, run in WSL
  and qemu; `s18.exe` and `release/fid-dev.exe` failed as in finding 2.
- No suite run.
