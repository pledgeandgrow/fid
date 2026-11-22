# Own-Backend Roadmap: better than LLVM where we can, different where it pays

*Rewritten Nov 7, 2026. The previous version (parts F-1 to I-6, the passes of the
frozen Rust compiler) is in git history: `git show 9c06851:docs/roadmap/llvm-roadmap.md`.
The IDs `src/*.rs` and `AGENTS.md` mention (F-1A, I-2D, ...) refer to it.*

## The bet

We will not out-tune LLVM's -O2 across every program: it has decades of work in
it. We can do three things it cannot, and we already have the start of each:

1. **Compile 10x faster for code within 10–30% of LLVM's** (Cranelift's class),
   with no C toolchain, no assembler and no linker: `fid emit` already encodes
   machine code and writes PE and ELF files itself.
2. **Optimize with facts LLVM never gets**: ownership, second-class references,
   checked arithmetic, effects, and the whole program in one build.
3. **Be trusted without decades of users**: we have LLVM itself as an oracle
   (the C path builds through clang), and a design small enough to check pass by
   pass, rule by rule.

LLVM stays where it is strongest: `fid ccompile` (clang through `zig cc`) is the
maximum-speed tier, and the reference our backends are measured and tested
against.

## What we have built (Nov 7)

| Piece | Where | State |
|---|---|---|
| IR: blocks, single-assignment values, locals in `Alloca`, `inst_at` / `fn_index` tables | `compiler/ir.fd` | no phis, no use lists |
| IR optimizer: inlining of small one-block functions, constant folding, DCE, LICM, dominators | `compiler/iropt.fd` | on the native paths; kill switches (`IRO_LICM`, `iro_pass_on`) |
| Check removal: counted loops, integer ranges | `compiler/checks.fd` | shared by the C and native paths (`PEIR_RANGES`) |
| Windows x64 backend: IR -> machine code, PE writer | `compiler/pe_ir.fd`, `pe_ir_enc.fd`, `pe_ir_stubs.fd` | 7 callee-saved registers, r10/r11 and xmm2–xmm7 for call-free ranges, copy coalescing, loop rotation, instruction selection |
| ARM64 and RISC-V backend: IR -> machine code, static ELF | `compiler/nat_ir.fd`, `nat_enc.fd` | values in x19–x28 / s1–s11 by linear scan over live-range hulls with block liveness; the rest in stack slots |
| Linux runtime on system calls: threads, M:N tasks, futexes, signals, traces | `stdlib/linux_runtime/` | own context switch (`nat_ctx_swap`) |
| C path through clang | `compiler/codegen_c.fd` | LLVM quality, the oracle |
| Old PIR path (L1–L3, its optimizer and allocator) | `compiler/pir.fd`, `opt.fd`, `regalloc.fd`, `codegen_x64.fd` | not used by `fid emit`; `build --native` crashes |
| **Since Nov 8–9:** machine IR, one allocator (IPRA, checker), x86-64 Linux target, debug sections | `compiler/nat_ir.fd`, `nat_enc.fd` | AArch64, RISC-V, x86-64 Linux; suite 273 / 271 / 272 of 317 (Nov 9) |
| **Since Nov 8–9:** IR verifier, rules, e-graph and load forwarding | `compiler/irverify.fd`, `rules.fd`, `egraph.fd` | on the native Linux path; `FID_VERIFY=1`, `fid test-rules [--smt]` |
| **Since Nov 8–9:** encoder oracle, pass matrix, SMT proofs | `scripts/enc-oracle.py`, `pass-matrix.sh`, `rules-smt.sh` | 1,181 forms, 0 mismatches; all 108 rule obligations proved by z3 |
| **Since Nov 9:** Windows x64 on the machine layer (opt-in `fid emit --nat`), line tables | `compiler/nat_ir.fd`, `nat_enc.fd`, `stdlib/linux_runtime/win.fd` | a PE with imports, the Linux runtime over kernel32; 301 of 338 suite tests; `.debug_line` on Linux |
| Reference escape check (references are second-class) | `compiler/escape.fd` | a language rule, not yet used by the optimizer |

**Audit notes (Nov 9, from reading every module; details in
docs/file-analysis/):** the table above is from Nov 7–9 and lags the code
in these places.
- iropt.fd has about 20 passes, not 4: module passes inline, gconst (on)
  and inline2, envtrim, ipcp, spec (opt-in); per function fold,
  branchfacts, sccp, promote, devirt, dce, tailcall, gvn, licm (on) and
  the iropt_me passes, simplify, branchfold, dse (opt-in); deadf is opt-in.
  `FID_IROPT_OFF` / `FID_IROPT_ON=name|all` are the switches: there is no
  `--no-pass` flag (ME5), and `iro_time` does not time devirt or the
  module passes. Not run on the C path.
- WP7 (envtrim) is written and opt-in since the evening of Nov 9 (a
  caller still passes the trimmed environment slot); WP14 (specialize) is written, opt-in;
  WP15 (deadf) and WP10 (read-only call merging, `FID_RULES_ON=readcse`)
  are opt-in now, and WP10 misses a write after the call in a loop body;
  WP5 never takes effect (the only indirect-call target comes from a load);
  WP8 and WP17 have uncommitted code (WP8 never fires; Nov 16: it does
  now, ng_leaf_ok); WP20's facts
  writer and reader disagree on the effect format. (Nov 15: the WP10 loop
  write and the WP20 format are fixed, and WP9/WP10 see callees'
  effects transitively; see the track. Nov 17: WP7, WP14 and WP15 are on by
  default, `FID_IROPT_OFF=envtrim/spec/deadf`; WP10 since OP25,
  `FID_IROPT_OFF=readcse`.)
- ME13 and branchfacts treat float compares like integer ones (wrong for
  NaN); ME14/SCCP folds narrow integer types with 64-bit arithmetic, so a
  narrow overflow panic is lost (known-bugs 2g.11, 2g.12).
- The machine layer is also x86-64 (Linux and `--nat`), its pool includes
  caller-saved registers with IPRA, and `ng_regalloc` remains for
  `FID_NAT_OFF=mir`. rules and the e-graph run on `--nat` Windows too.
- The old PIR path is still used by `emit --boot`, `codegen` and
  `build --native` with the default target (which panics in
  `pass_jump_thread`, not `pass_dse`).
- LF1: lower.fd now sets `borrow_kind` on reference parameters
  (uncommitted, Nov 9).

The cost of today's shape: `pe_ir` and `nat_ir` lower the same IR twice, with
two register allocators, two call lowerings and two sets of overflow-check
sequences. Every improvement is written twice, or only once.

## How the five ideas work

**1. One machine layer, thin targets.** Between the IR and the encoders sits a
machine IR (MIR): per function, blocks of machine-level instructions over
virtual registers, with target-neutral operations (add, compare-and-branch,
load, store, call, return) and target-specific ones where a target has
something better. One lowering IR -> MIR implements the language's semantics
(checked arithmetic, narrowing, struct copies, calls, panics) once. After it,
everything is shared: liveness, the register allocator, frame layout, the call
convention lowering, the peephole pass, block layout. A target supplies a
description (registers, classes, which are callee-saved, the call convention),
instruction selection rules (MIR patterns -> its instructions) and its encoder.
x64, AArch64 and RISC-V become three thin directories. Cranelift's VCode and
regalloc2 are the model; we write ours in Fides.

**2. Rules instead of fifty passes.** First real SSA: mem2reg turns locals into
values with block parameters (no phi nodes). Then an e-graph mid-end: each
function's pure values go into an e-graph, rewrite rules add equal forms
(`x + 0 = x`, `(a + b) - b = a`, `x * 2 = x << 1`), and extraction picks the
cheapest. CSE and GVN fall out of the e-graph's hash-consing; constant folding is
a rule; LICM is where extraction places a value (Cranelift's "aegraph" keeps the
side-effecting skeleton fixed and lets pure values float). A rule is small and
can be tested on its own: by random inputs, and by an SMT check of `lhs == rhs`.

**3. The whole program.** A build sees every function. Compile bottom-up over the
call graph and each function's real clobber set is known before its callers are
compiled: a caller keeps values in registers the callee never touches
(interprocedural register allocation). Internal functions need no ABI: they take
arguments where the caller has them. Effects say which calls are pure: those
are values like any other in the e-graph, merged and hoisted.

**4. The language's guarantees.** A `&mut` reference cannot alias anything else
live, a reference cannot escape (escape.fd proves it), an owned value has one
owner: alias analysis is a lookup, not a guess. Checked arithmetic plus range
analysis decides which checks can never fire. A value that never escapes its
function can live on the stack, or in registers.

**5. Compile speed as a feature.** No text anywhere between source and bytes;
functions compile independently once the IR exists, so in parallel; each
function's machine code is cached by a hash of its IR and of what it calls, so a
one-line edit recompiles one function and relinks in memory.

## Reading the goals

`[x]` done, `[~]` partly done, `[ ]` not started. Size: S (a day or two), M (a
week), L (several weeks), XL (months). Each goal names where it lands.

---

## Track MI — One machine layer (30)

- [x] MI1. Write down the MIR: instructions, operands (virtual register, physical
      register, immediate, stack slot, label), blocks, block parameters; in a new
      `compiler/mir.fd` (M).
      Nov 8–9: `compiler/nat_ir.fd` records each operation of its architecture
      layer as an `MInst` on virtual registers (Cranelift's VCode shape: selection
      happens in the layer), for AArch64, RISC-V and x86-64 Linux.
      Done (Nov 15): the operations have names (`mi_op_name`, the printer), and
      the operand rules are written down as the verifier's checks (MI10): a
      vreg of the function, a physical register, immediates, labels placed once.
      Left: its own module `compiler/mir.fd` (a new file and a `main.fd` import),
      block parameters (values still cross blocks in vregs).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — compiler/mir.fd holds the MIR (1,537 lines: MInst, MO_*,
      blocks, liveness, printer, mir_verify), imported by nat_ir.fd and
      main.fd; machine_bparams_check passes on 4 targets under FID_VERIFY=1
      (2229 block parameters on x86_64). Block parameters stay opt-in
      (`FID_NAT_ON=bparams`).
- [x] MI2. Target-neutral operations: integer and float arithmetic, compares,
      branch on compare, loads and stores of 1/2/4/8 bytes with extension,
      address arithmetic, calls, returns, traps (M).
      Nov 15 (verified in code, plus the branch on compare): the `MO_*`
      operations of `compiler/nat_ir.fd` — `MO_ADD`..`MO_MULH`, `MO_FBIN`,
      `MO_SETCC`/`MO_FCMP`, `MO_BRCC` (new, MI23), `MO_LOAD` (1/2/4/8, signed or
      not) / `MO_STORE`, `MO_EXT`, `MO_SLOTADDR`/`MO_ADDR`, `MO_CALL`/`MO_CALLR`,
      `MO_EPILOGUE`, `MO_TRAP`/`MO_PANICIF`; tests/bootstrap/test_machine_brfuse.fd.
- [x] MI3. Target-specific operations as an open set per target (x64 `lea`,
      AArch64 `madd`, RISC-V `slli`+`add`), chosen by instruction selection (M).
      Done (Nov 15): the first, `MO_ADDSHL` (d = a + (b << s)): AArch64 one
      `add d, a, b, lsl #s`, x86-64 one `lea d, [a + b*2^s]` (`x64_lea_idx`,
      through r11 past scale 8), RISC-V `slli` + `add` (`ng_addshl`;
      `FID_NAT_OFF=addshl`). Left: `madd`/`msub`, RISC-V Zba (`sh1add`..) under
      `--cpu`, and a per-target table of such operations instead of one switch
      in each `ng_*` function.
      Nov 22 code audit: madd/msub (ng_a64_mac), Zba sh1add..sh3add (rv_shadd, --cpu), the per-target isel table (ng_isel_rows/tg1/tg1b) — all in nat_ir.fd/nat_enc.fd. The entry text predated TG1/TG8/TG9.
- [x] MI4. A target description: register classes, callee-saved set, argument
      and result registers, stack alignment, red zone, scratch registers (S).
      Nov 15 (verified in code): it exists as functions of `compiler/nat_ir.fd`
      per target — `ng_callee_saved`, `ng_pool_caller`, `ng_argreg_i` /
      `ng_nregargs_i`, `ng_spill_reg`, the scratch registers set in
      `NatGen_new` (t0..t3, ad, ad2, zr), `ng_code_align` (MI30). Left: one
      record per target holding them (and the float class, MI25), read by the
      allocator instead of `if` chains on `g.arch`.
      Nov 22 code audit: NatTarget record + nat_target_desc(arch) at nat_ir.fd:408, .td. used 66 times; the remaining ng_* accessors are compatibility helpers on the same record.
- [~] MI5. One lowering IR -> MIR for every language rule now written twice in
      `pe_ir.fd` and `nat_ir.fd`: checked add/sub/mul, narrowing, MIN / -1,
      division by zero, struct copies, closures, dyn calls, print routing (L).
      Nov 8–9: one lowering for the three Linux targets; `pe_ir.fd` keeps its own
      until MI14.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — nothing of the Windows half is done: pe_ir is still the
      Windows default (main.fd sets nat_win only with `--nat`) and there is
      no `--pe` escape hatch. On a 75-program sample `--nat` matches or
      beats pe_ir (72 same, 3 where it fixes pe_ir, 0 differ). Left: the
      full x86_64-windows vs x86_64-windows-pe comparison, then the switch.
- [x] MI6. Panic paths as cold out-of-line blocks shared per function, each a
      call (so traces name the function), from the one lowering (M).
      Panics are calls to shared stubs on every Linux target, so traces name the
      function.
      Nov 15 (written; checked by the final build): every overflow, division
      and narrowing check (`MO_PANICIF`) is one branch to a cold block at the end
      of its function, one per panic kind and line, which calls the stub and
      traps (so the return address stays inside the function); `ng_mir_emit`,
      `FID_NAT_OFF=coldpanic`. `ng_cold_last` (WP17) moved a trap segment even
      when the code before fell into it (the else block of a BNEZ): it now
      leaves a JMP to its label, or keeps it in place when it has none (the
      entry). Tests: test_machine_coldpanic.fd, test_machine_coldpanic_fail.fd.
- [~] MI7. One call lowering: arguments to registers and the outgoing area,
      `__env`, varargs spawns, results; parallel moves solved once (M).
      Nov 15 (verified in code): one for every machine-layer target (Linux
      AArch64/RISC-V/x86-64, `--nat` Windows, macOS): `ng_gen_call_std` /
      `ng_load_args_i` (stack arguments first, then registers, `__env` first,
      the internal convention's register count), `g.va_off` for varargs spawns.
      Arguments come from vregs, so no move cycle can arise there. Left:
      pe_ir's own (MI14/MI17).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_call_lowering_parity is right on run, emit and ccompile,
      and mi7_call_parity gives 19 same, 0 differ (with MSYS_NO_PATHCONV=1;
      without it the 6 Linux runs come out empty). Left: deleting pe_ir's
      own call lowering (still there), which waits for MI17/MI5.
- [x] MI8. One frame layout: slots, spill slots, the save area, alignment, frame
      records for traces, large frames (M).
      Nov 15 (verified in code): one for every machine-layer target —
      `ng_gen_function` (outgoing area, a slot per IR value, Alloca data, the
      varargs area), `ng_mir_alloc` (spill slots, the save area, 16-byte
      alignment), frame records (fp chain) in `ng_prologue`, large frames
      probed page by page (MI29). Left: pe_ir's own; slots only for values
      that need one (today every IR value has a slot, so big functions have
      frames of many pages).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — rematerialized values get no spill slot: the frame tests pass
      and big's frame drops from about 20 KB to 1968 bytes on x86-64 (1936
      on AArch64, 1968 on RISC-V) with 0 spills, but machine_frame_check
      wants under 512 bytes and fails on 3 targets. Left: what else takes
      ~1.9 KB of big's frame; pe_ir's frame (MI17).
      Nov 21 (evening): spilled vregs whose position ranges never overlap
      share one slot (ng_mir_alloc_body: a linear range covers every point
      a vreg's slot is written or read, so disjoint ranges cannot see each
      other's bytes; the scan is bounded at 4096 spilled vregs). Then the
      1.9 KB was found: not spills at all (0 after RA2/RA7/RA3 rescued
      242 ranges — the slot packing now runs AFTER them and drops rescued
      slots) but ~240 dead 8-byte ALLOCAS — ssa/promote leave a promoted
      local's Alloca in the IR unused, and ng_gen_function gave each its
      frame bytes. An Alloca nothing addresses keeps none now (used_ids
      precomputed once, nat_ir.fd). Measured: big's frame on x86_64-linux
      is 32 bytes (was 1952 — 61x smaller; the under-512 goal is met with
      room). machine_frame_check's other legs (MI22 encoder oracle,
      remat's frame) still to re-run on a quiet machine.
- [~] MI9. Struct values as several MIR values when small (scalar replacement at
      the machine level), memory otherwise (M).
      Nov 22 audit: `ng_rr_plan`/`ng_rf_promote` already keep eligible
      one- and two-word struct results and copied result slots in MIR vregs;
      `test_natabi_retregs.fd`, `test_natabi_retfwd.fd`, and
      `test_natabi_optmatch.fd` cover those paths. Left: general scalar
      replacement of small local structs that are not returned through the
      direct-call result-slot path. Keep partial.
- [x] MI10. A MIR verifier: every use defined, every block terminated, operand
      classes match the operation; run after each MIR stage in debug builds (M).
      The allocation checker (RA15) runs over the MIR with `FID_VERIFY=1`; a
      structural MIR verifier is still to write.
      Nov 15 (written; checked by the final build): `ng_mir_verify`
      (`compiler/nat_ir.fd`), with `FID_VERIFY=1` after the peephole and the
      branch fusion, before allocation: known operations; register operands a
      vreg of the function or a physical register; every temporary read is
      written somewhere; every jump to a label placed once in the function;
      access sizes, extension, compare and panic kinds, shift counts; the
      prologue first; nothing falls off the end. After allocation RA15's
      checker. Whether an IR value's vreg is set on every path is left to the
      IR verifier (irverify.fd).
- [x] MI11. A MIR printer and `fid dump-mir <file> --target <t>` (S).
      `FID_NAT_DUMP=<function>` prints a function's MIR with its allocation and
      live ranges.
      Nov 15: operations by name (`mi_op_name`), `FID_NAT_DUMP=*` for every
      function, and `g_nat_dump` for main.fd to set.
      Done (Nov 16, main.fd cmd_dump_mir): `fid dump-mir <file> [--target <t>]
      [--fn <name>]` runs emit's pipeline (default x86_64-linux) with
      `g_nat_dump = "*"` (or the --fn name) and writes no image.
      Check: scripts/driver-goals-check.sh (tests/bootstrap/test_driver_disasm.fd).
- [x] MI12. Port the AArch64 backend to MIR first (it is the smaller one); its
      suite results must not change (L).
      Nov 8: AArch64 on the MIR; suite results kept (261 passed at the first full
      run).
- [x] MI13. Port RISC-V to MIR: only instruction selection and its encoder stay
      target code (M).
      Nov 8: RISC-V on the MIR, only selection and its encoder are target code.
- [~] MI14. Port Windows x64 to MIR, keeping every optimization `pe_ir.fd` has
      (coalescing, rotation, xmm values, selection) as MIR passes or rules (XL).
      Nov 9: Windows x64 runs on the MIR, opt-in (`fid emit --nat`; pe_ir stays
      the default and was not touched): the x86-64 selection and allocator of
      MI15, a PE with its import table (`nat_pe`, NATFIX_RVA32), Win64 calls
      through one thunk (`nat_win_call`: rbx and r12–r15, the allocated
      registers, are callee-saved there too), TLS in a TlsAlloc slot, the
      TEB's StackLimit per task context. The runtime is the Linux one with its
      system calls mapped onto kernel32 / ws2_32 / bcrypt (win.fd). Suite: 301
      of 338 match the interpreter (304 with the `pe_sh` hook platforms added).
      Measured against pe_ir (best of 3, Nov 9): fib 1400 / 1598 ms, loop
      4018 / 13043, sieve 485 / 1003 (3537 before), hashmap 1450 / 2568,
      float loop 642 / 1364. Closed since: inline `Vec_len/get/set/push`
      (`vecinline`, on every target), checks.fd's unchecked operations
      (`unchecked`), RA5, RA6, RA8, RA18's first step, division panics with
      " at line N" (`panicline`). The runtime conditions (platforms, RT9,
      RT13, RT15): a task yield 252 ns (pe_ir 243), a task's stack overflow
      "Panic: Stack overflow" with status 1 (the TEB stack fields travel
      with nat_ctx_swap, the guard a PAGE_GUARD page), 200 echo clients.
      pe_runtime retires with pe_ir (agreed with platforms). Left before the
      default switches: the speed gap above (an xmm register class,
      splitting at calls, cheaper calls), RT11's epoll over AFD (platforms,
      in win.fd), PDB (DB3).
      Nov 16 (nat side, not measured): cheaper calls — frameless leaves
      (WP8 fires now), sibling calls as jumps (OP13, `ng_tail_at`), struct
      results of two words in registers (ME27). Left: the xmm register
      class (f64 values live in general registers and move through
      xmm0/xmm1, `xmmcache`) and splitting live ranges at calls (RA2/RA4:
      ranges are hulls; a value live across a call takes a callee-saved
      register for its whole range) — both changes to ng_mir_alloc's
      design, not attempted without a build to measure them.
      Checked Nov 16: the COFF symbol table of `--nat` images (the
      equivalent of pe_ir's `pe_append_coff_symbols`) is there: nat_enc.fd
      `nat_pe_symbols`, called from nat_ir.fd for every placed function
      unless `FID_NAT_OFF=debug`; scripts/nat-tools-check.sh compares the
      two images (not run yet).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — floats in float registers on `--nat` by default (180 ranges;
      `FID_NAT_OFF=winfregs` turns it off), machine_fregs_check 0 failures,
      machine_mi14_check 34 ok and 1 failure (test_machine_simd fails the
      allocation check, argument register). `--nat` is still slower than
      pe_ir: loop 6635 vs 2274 ms, sieve 1381 vs 501, hashmap 2340 vs 1622
      (fib and bench_float faster). Left: the checker failures, speed, the
      suites with winfregs/splitfloat.
      Nov 21: 2g.88 fixed (see RA3) — the checker's rejection of argument-register hints on x86-64 was its own miss.
- [x] MI15. A Linux x86-64 target on MIR for free once x64 is ported: the ELF
      writer of `nat_enc.fd`, the Linux runtime, x64 selection (M).
      Nov 9: Linux x86-64 on the MIR (`fid emit --target x86_64-linux`): its own
      encoder, 4 register arguments, the generic system call numbers mapped in
      `nat_syscall`, TLS through fs, FMA3 for `fma` (x86-64-v3 for log/pow only).
      `--elf` / `--target linux` still use pe_ir's no-stdlib path.
- [~] MI16. A macOS AArch64 target: the AArch64 selection and a Mach-O writer
      (release F7) (L).
      Nov 15 (verified in code; release-roadmap F7): `fid emit --target
      aarch64-macos|x86_64-macos` runs the machine layer's selection and writes
      a PIE Mach-O (`nat_generate_macho`, `compiler/nat_enc.fd` `nat_macho`:
      dyld rebases, libSystem binds into a GOT, an ad-hoc code signature), the
      runtime over libSystem in `stdlib/linux_runtime/darwin.fd`. Left: running
      it on a Mac (`.github/workflows/macos-native.yml`), and F7's open runtime
      parts (errno, directory listing, sockets, process spawn, signals).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919): not
      checkable here — darwin.fd now translates recvfrom/accept peer
      addresses and implements sysinfo, and macos_udp_check passes at link
      level (both macOS targets bind _recvfrom, _sysctlbyname); udp_peer.fd
      is right on x86_64- and aarch64-linux, which do not reach darwin.fd.
      Running it needs a macOS host. Left: that run, recvfrom/sendto with an
      address on `--nat` Windows, the macos-native.yml step.
- [~] MI17. Retire `pe_ir.fd`'s and `nat_ir.fd`'s own lowerings once their MIR
      ports match on the whole suite (M).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — pe_ir computes float `%` now (x87 FPREM, exact fmod):
      test_float_rem_exact is right on run, ccompile and `--nat`, 10 of 11
      on pe_ir (an f32 result cast `as i64` gives i64::MAX there);
      mi5_nat_peir_check on a 75-program sample: 72 same, 3 where `--nat`
      fixes pe_ir. Left: that pe_ir bug, the full-suite comparison, then
      retiring the lowerings (pe_ir is still the default).
      Nov 21 (5fb5516, 2g.95): pe_ir's ConstFloat for F32 emits `p_f32_to_bits(value)` — it stored the f64 pattern's low 32 bits (0.0 for 2^63), which sent the saturating `f32 as i64` check to i64::MAX/MIN.
- [x] MI18. Retire the old PIR path (`pir.fd`, `opt.fd`, `regalloc.fd`,
      `codegen_x64.fd`) and `build --native` with it (S).
      Nov 16: no build reaches PIR any more. `build --native` is the machine
      layer on every target (main.fd cmd_build: the default x86-64 is
      `x86_64-linux`, arm64/riscv64 their Linux targets, a Windows target
      pe_ir; another target is an error, never compiler.fd
      build_executable), and `fid gpu` reads the IR (gpu.fd rewritten:
      values as variables/registers, scalar stack slots translated);
      tests/bootstrap/test_driver_native.fd, test_driver_gpu.fd.
      Nov 17: `fid pir`/`dump-pir` exit 1 and name their replacements,
      `fid opt` prints the IR after iropt_native, `fid codegen` is the
      machine layer (disassembly, or --elf/--coff/--macho objects as emit
      --obj), REPL `:pir` became `:opt`; compiler.fd build_executable
      deleted; main.fd/compiler.fd no longer import pir, opt, pe,
      codegen_*, peephole, *_enc, runtime_* (docs/pir-retired.md,
      tests/tooling/mi18-pir-retired-check.sh). elf64.fd's unused `use
      compiler::pir;` is dropped (Nov 17), so nothing the compiler reaches
      imports pir.fd. Left: delete the files (pir.fd, opt.fd, regalloc.fd,
      codegen_x64/arm64/riscv.fd, peephole.fd, pe.fd, codegen.fd,
      arm64_enc.fd, riscv_enc.fd, runtime_arm64/riscv/linux.fd, and
      tests/selfhost/test_pir.fd, test_opt.fd with their baseline lines).
      Checked Nov 15: not retirable yet, these still reach it: `fid build
      --native` with the default target `x86-64` (compiler.fd
      `build_executable`: opt.fd, codegen_x64.fd, peephole.fd, elf64.fd,
      linker.fd, runtime_linux.fd; `--target arm64|riscv64|x64` already go
      to nat_ir), `fid pir`/`dump-pir` and `fid opt` (pir.fd, opt.fd),
      `fid codegen` (codegen_x64/arm64/riscv.fd, peephole.fd, `--elf`/
      `--coff`/`--macho` objects) and `fid gpu` (gpu.fd reads pir.fd's L3).
      `fid build-x86` uses x86.fd, not PIR. `emit --boot` left it (pe_ir,
      release G1). Retiring needs main.fd/compiler.fd to send `build
      --native` to nat_ir and `gpu` to read IR or MIR.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — no live module imports the PIR path; `fid pir` says it was
      removed; `fid opt`, codegen listings, `--elf`/`--macho` objects and
      `emit --boot` work. mi18-pir-retired-check fails on `codegen --coff`
      (kernel32 imports undefined), and the REPL `:opt` checks fail on
      inlined functions. Left: COFF, deleting the PIR files (still in the
      tree), STRICT=1, test_pir/test_opt in verify-selfhost.sh.
      Nov 21: see RL9 — the COFF object path through nat_enc works (2g.110); coff.fd stays unreached.
- [~] MI19. Runtime primitives (`nat_ctx_swap`, atomics, `nat_clone_thread`,
      trampolines) written once in MIR where they can be, per target only where
      they must be (M).
      Nov 22 audit: `nat_ctx_swap` and the task trampoline share one
      architecture-layer generator; atomic operations, thread creation,
      TLS, signals, and entry points still have target-specific encoders.
      The shared operations are not yet represented as MIR operations.
      `machine_frame_check.sh` covers the task primitives; full multi-target
      verification remains owed. Keep partial.
- [~] MI20. Machine code primitives for the runtime already exist per target
      (`ng_gen_primitives*` in `nat_ir.fd`, the stubs of `pe_ir_stubs.fd`); move
      them behind one interface (M).
      Each target's primitives (context switch, thread start, atomics, signals,
      traces) are behind one set of `nat_*` names.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — pe_ir defines the whole `nat_*` primitive interface (one
      list, pe_ir_stubs.fd `prim_interface_names`, shared with nat_ir) and
      checks it under FID_VERIFY=1: mi20_prim_check passes on pe, nat and
      the three Linux targets. Nov 22: the 67 call sites that have nat_* interface names now use them
      (pe_atomic_* → nat_atomic_* 62, pe_ctx_swap → nat_ctx_swap 2,
      pe_call_fn → nat_call_fn 4; conc.fd + base.fd). The remaining ~1,340
      pe_* calls are pe_runtime's own helper functions with no nat_*
      equivalent — they are the implementation, not the interface.
      Left: verifying mi20_prim_check passes with the renamed callers.
      still makes 75 calls by `pe_*` names, base.fd 2.
- [x] MI21. Constants: materialization per target (`movz/movk`, `lui/addi`,
      `mov imm64`) chosen by cost, rematerialized instead of spilled (M).
      Nov 15 (verified in code, already done): `compiler/nat_enc.fd` `a64_li`
      (MOVZ or MOVN, whichever needs fewer MOVKs), `rv_li` (ADDI, LUI+ADDIW, or
      shifted pieces), `x64_li` (xor, mov r32, sign-extended imm32, imm64);
      spilled constants, global and slot addresses are computed again at each
      use (RA8, `ng_ld_spilled`, test_machine_remat.fd). AArch64 logical
      (bitmask) immediates are not used.
- [x] MI22. Address modes as a selection decision: base + index * scale + offset
      where the target has it (M).
      Done (Nov 15): base + index * 2^k as one operation (`MO_ADDSHL`, MI3) for
      `GetElemPtr` of power-of-two elements and the inline `Vec_get`/`Vec_set`/
      `Vec_push` element address (was `li`, `mul`/`shl`, `add`);
      test_machine_brfuse.fd. Left: folding the address into the load or store
      itself (x86-64 SIB operands, AArch64 `ldr x, [x, x, lsl #3]`, the offset
      in the access).
      Nov 22 code audit: ng_ldx/ng_stx fold base+(idx<<sh)+disp into one access (nat_ir.fd:1745/1770); x64 SIB ModRM (nat_enc.fd:6517+), AArch64 register-offset (nat_enc.fd:648); selection-time fold at nat_ir.fd:11272. enc-oracle-mi22.py exists.
- [x] MI23. Compare and branch fused in selection on every target (x64 `cmp/jcc`,
      AArch64 `cbz/tbz/b.cond`, RISC-V `beq/blt`) (S).
      x86-64 tests and jumps fused; AArch64/RISC-V compare to a register and
      branch.
      Nov 15 (written; checked by the final build): `ng_fuse_branches` turns
      SETCC + the MOVs to the branch + BNEZ/BEQZ into one `MO_BRCC` when the
      compare's value is dead after the branch (block liveness over just those
      vregs); `ng_brcc` emits x86-64 `cmp`/`test` + `jcc`, AArch64 `cmp` +
      `b.cond` (`cbz`/`cbnz` against zero), RISC-V the inverse `beq`..`bgeu`
      over a `jal`. `FID_NAT_OFF=brfuse`; test_machine_brfuse.fd. `tbz` (bit
      tests) not used.
- [x] MI24. Conditional select (`csel`, `cmov`, RISC-V Zicond where present) for
      small branches with no side effects (M).
      MO_CSEL (mir.fd:162) formed by ng_fuse_select from small branch
      diamonds; encoders x86-64 CMOVcc, AArch64 CSEL, RISC-V Zicond
      (czero.eqz/nez + or under --cpu rva23u64), else a branch over a move.
      FID_NAT_OFF=csel the kill switch. Tests: machine_batch4_check.sh
      (MIR + encoders against clang).
- [x] MI25. Floats in float registers on every target, f32 as f32 (not widened),
      moves between classes only where needed (M).
      Nov 17: the f32 FFI stubs and `p_f32_to_bits` keep an f32 as f32 (not
      widened to f64 on the way).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — floats are in float registers by default on every target
      (`fregs`, `--nat` included) and an f32 stays f32 at the FFI boundary:
      machine_fregs_check 0 failures (166 float ranges on x86_64, 208 on
      aarch64, 166 on `--nat`; none with it off), machine_f32_ffi_check 0
      failures. The aarch64/riscv64 f32 FFI images were built, not run (no
      loader sysroot here).
- [x] MI26. Narrow integers kept in registers with known extension state, so a
      `u8` add does not re-extend each time (M).
      ng_fold_ext_state (nat_ir.fd:8970): each vreg gets the meet over its
      definitions of what extension each leaves (a narrow load its
      extension, an EXT its kind, a constant its range, AND/OR/XOR what
      bit operations keep, a right shift the bits that are left); an EXT
      whose source already satisfies it becomes a MOV. MO_OPW (RISC-V
      ADDW/SUBW/MULW) for the narrow arithmetic itself.
      FID_NAT_OFF=extstate the kill switch.
- [x] MI27. Jump tables for dense `match` (M).
      Nov 17: implemented: an indirect jump in the machine IR (`MO_JTAB`, its
      entries `MO_JTE` in compiler/mir.fd), table entries as 4-byte
      self-relative offsets (`NATFIX_SELF32`), `ng_gen_jtab` / `ng_jtab_emit`
      in nat_ir.fd, encoders `x64_jmp_r`, `a64_br`, RISC-V `jalr x0`;
      `FID_NAT_OFF=jtab`: every Switch becomes the tree below
      (docs/reference/jump-tables.md). Before:
      Nov 15: needs a multi-way branch in the IR first: `match` reaches the IR
      as a chain of compares and `Branch`es (`compiler/ir.fd` has no switch
      instruction), so the machine layer cannot see a dense match.
      Nov 16: the IR has `Inst::Switch(value, keys, targets, default)` (ME28).
      nat_ir forms them per function before selection (`FID_SWITCH=off`: no)
      and emits `ir_switch_plan`'s layout: a balanced tree of signed compares
      (`MO_BRCC`), equality tests for ranges of up to 3 keys, and bit tests
      (`x - lo <=u 63`, one AND with the mask of a block's keys) for ranges
      under 64 values going to at most 3 blocks (`ng_gen_switch`). Test
      `tests/bootstrap/test_irswitch_match.fd`. Left: real jump tables: the
      machine IR has no indirect jump (an `MO_JTAB` op, its three encoders, a
      table of code addresses in the data and its relocations, and the
      allocator's successor list).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — jump tables work: test_machine_jtab is right,
      test_irswitch_match has its tables on every target and PIE, the
      x86-64 image has 9 indirect jumps (1 with jtab off). But
      machine_jtab_check fails 5
      (digit_name and live_across are inlined into main), and with inlining
      off bits3 gets a table the check says it must not. Left: the test or
      the check, so that it passes as written.
- [~] MI28. Tail calls as jumps on every target, guaranteed for self-recursion in
      tail position (M).
      Nov 15 (verified in code): self-recursion in tail position (`return f(..)`
      in f, the call's only use the return) becomes a loop in the IR
      (`compiler/iropt.fd` `iro_tailcalls`, OP13, on by default:
      `FID_IROPT_OFF=tailcall`), so on every native target. Left: tail calls to
      other functions as jumps in the machine layer (frame reuse, stack
      arguments), and making the self-recursive case independent of the
      optimizer switch.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — tail jumps in functions that take a frame address:
      test_machine_tailcall_frame is right on every target, and on
      x86_64-linux and `--nat` the 3-million-level mutual recursion runs in
      one frame. On AArch64 and RISC-V tailcall_deep overflows the stack
      (is_odd called from is_odd): those calls are not jumps there. Left:
      that; the check's runner passes Windows paths to wsl.exe.
      Nov 21 (5fb5516, 2g.98): sibling and mutual tail calls are jumps again — pe_ir recognizes `Return(call)` and the inliner's store-call-to-slot shape (peir_tail_via_slot, peir_slot_forward, peir_slot_load_return; pe_tail_slot_check.sh; FID_PE_OFF=tail restores the call), and nat_ir keeps a tail call when the result needs MO_EXT (g_ng_ret_ext; the AArch64/RISC-V tailcall_deep overflow).
- [x] MI29. Stack probes for frames over a page, so a large frame cannot jump over
      the guard region (S).
      Nov 15 (written; checked by the final build): `ng_probe_frame`
      (`compiler/nat_ir.fd`, from `ng_prologue`): a frame larger than 4096
      bytes moves sp one page at a time and stores to each page (x86-64 `sub
      rsp` + `mov [rsp], rbp`, AArch64 `sub sp, #1, lsl #12` + `str xzr`,
      RISC-V `sub` + `sd zero`), unrolled up to two pages, a counted loop past
      that, then the rest with its last word touched. Every machine-layer
      target (Linux, `--nat` Windows, macOS); `FID_NAT_OFF=probe`. Frames this
      large are common (a slot per IR value): test_machine_probe.fd.
- [x] MI30. One place that decides the code alignment of functions and loop heads
      per target (S).
      Nov 15: `ng_code_align`/`NG_ALIGN_FN` in nat_ir.fd is the single decision
      point; every code-entry and loop-head `align` call routes through it.

## Track RA — One register allocator (20)

- [~] RA1. Today: `nat_ir.fd` allocates callee-saved registers over live-range
      hulls from block liveness; `pe_ir.fd` has its own allocator with
      coalescing and call-free ranges. Replace both with one allocator on MIR (L).
      Nov 8: one linear-scan allocator for the three Linux targets
      (`ng_mir_alloc`); pe_ir keeps its own.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `FID_PE_OFF=ra` builds pe_ir without its allocator:
      test_pe_ra_off is right both ways (images 158,963 vs 218,867 bytes),
      ra1_pe_ra_off_check on 117 programs gives 114 same, 0 differ (1
      out-of-memory build). Left: deleting pe_ir's allocator, which waits
      for MI17 and the Windows default.
- [~] RA2. Liveness on MIR from use lists, live ranges as lists of segments with
      holes, not hulls (M).
      Done (Nov 17): liveness from use lists (`ng_use_liveness`;
      `FID_NAT_OFF=uselive`: the fixed point, compared under `FID_VERIFY=1`),
      live ranges as exact segment lists (`ng_class_segs`) used by the linear
      scan (`ng_seg_scan`), the greedy allocator and the second chance
      (`FID_NAT_OFF=holes`, `segscan`); the float class and RA7's hints are
      still on hulls.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — liveness from use lists (ng_use_liveness) gives the same
      allocation (`FID_NAT_OFF=uselive`); machine_segs_check passes on
      aarch64 and riscv64. On x86_64-linux and `--nat` every build of
      test_machine_segs and test_machine_holes fails the allocation check
      (argument register 6; fine with `FID_NAT_OFF=argfree`), and the
      reload comparison shows 0 vs 0. Left: that failure (RA3/RA7), a
      measured gain, floats and RA7's hints on segments.
      Nov 22 float plan (moderate, well-scoped): ng_float_assign
      (nat_ir.fd:18001) is a hull-based scan — it takes no segs parameter at
      all; segments already exist for float vregs (ng_class_segs computes
      them for every vreg, and :15205–15222 removes float ranges from the
      integer scan by setting lo = −1, keeping only the start in `flo`).
      Work: pass segs + class into ng_float_assign, replace hull expiry and
      interference with ng_seg_free / ng_seg_hits, and replace its
      call-crossing test with a per-segment legality check against the FLOAT
      pools (ng_float_pool, ng_float_callee_pool, mask ng_float_mask) — not
      ng_reg_ok + the integer IPRA masks. Float spills currently have no
      second chance at all (ng_alloc_holes runs before ng_float_assign);
      add a float holes round after it. RA7/RA3 keep treating float argument
      registers separately (they already take rcls). FID_VERIFY's checker
      must validate float assignments per segment, not per hull. Extend
      tests/nattools/seg_unit.fd — it covers the integer scan only.
- [x] RA3. All registers, caller-saved included: a value live across a call goes
      to a callee-saved register or is split around the call (L).
      Caller-saved registers for ranges no clobbering call crosses (AArch64
      x8–x11, RISC-V t4); x86-64 has none spare.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — argument registers for spilled ranges (ng_arg_free) work on
      AArch64 and RISC-V (150 ranges moved; reloads 129 -> 122 and
      262 -> 159). On x86-64 (Linux and `--nat`) the checker rejects the
      assignment made with RA7's hints ("defines v.. in argument register 6,
      whose argument or result is still to be read"): 5 of 69 test_machine
      programs under FID_VERIFY=1; without it they run right. Left: fix
      that (the assignment or the checker).
      Nov 21: 2g.88 fixed — the checker now allows a range to end at a self-copy of the argument register into the range's value (ng_arg_fit_end); the argument-register passes stay on.
      Nov 22 code audit: ng_arg_fit_end (nat_ir.fd:13997) is the 2g.88 checker fix; ng_arg_free at 14258; argreg_dead_def_check.sh + argreg_dead_def_unit.fd exist.
- [~] RA4. Live-range splitting at calls and at loop boundaries, with spill code
      placed outside loops (L).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the splitting (ng_mir_alloc_split, ng_split_check) works
      where the checker allows it: split_unit 24/24, test_machine_split and
      _split_float right, the split mutation rejected on 4 targets, wide
      reloads 512 -> 506 (x86-64), 222 -> 209, 258 -> 241; 69 test_machine
      programs agree with split off and on. Left: split_alloc_unit rejects 1
      x64 program, the splitall builds fail the argument-register check on
      x86-64, RISC-V and Windows, the suite spill numbers.
      Nov 21: 2g.88 fixed (see RA3); the FID_NAT_ON=splitall rejections were the same checker miss.
- [x] RA5. Spill weights from loop depth and use counts (M).
      Nov 9: `ng_spill_weights`: each use or definition 8^(loop depth), the
      loops from back edges in the MIR; with no register free the cheapest
      range gives its register up (`FID_NAT_OFF=weights`: the range ending
      last, linear scan's rule).
- [x] RA6. Copy coalescing on MIR (what `pe_ir`'s `PEIR_COALESCE` does), so
      `i = i + 1` is one instruction on every target (M).
      Move hints: a move's two sides prefer one register, so `ng_st_val` after an
      operation costs nothing. Nov 9: coalescing in `ng_mir_alloc` (union-find
      over vreg copies whose two sides never change while the other lives),
      the copy left a move to itself; the checker validates it
      (`FID_NAT_OFF=coalesce`).
      Nov 22 code audit: coalescing behind ng_on("coalesce") (nat_ir.fd:14981) + livecoal round; the self-move wart handled by RA7 hints (13677) and ng_fold_ext_state (8982). No Left line, nothing remaining.
- [~] RA7. Register hints: arguments, results and call operands prefer the
      registers they must end in (M).
      (Nov 15: only vreg-to-vreg move hints exist; the argument and result
      registers are outside the allocation pool, `ng_pool_caller`, so they
      cannot be hinted until the pool takes them in.)
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the hints are on by default (`FID_NAT_OFF=arghint`):
      arghint_unit 15/15, moves 7080 -> 4226 (x86-64), 7247 -> 4299
      (AArch64), 7295 -> 4355 (RISC-V), 10304 -> 5854 (`--nat`), the
      mutation caught, win_callback_regs_check passes. Left: under
      FID_VERIFY=1 the native suite fails on x86-64 Linux and `--nat`
      Windows (5 of 69 test_machine programs, with RA3's fill); the full
      suite.
      Nov 21: 2g.88 fixed (see RA3) — hint- and fill-built assignments pass the allocation checker again (argreg_dead_def_check.sh).
- [x] RA8. Rematerialization of constants, addresses of globals and frame slots
      instead of spills (M).
      Nov 9: constants: a vreg defined once by MO_LI is never stored, a spilled
      one is loaded again where used, and its spill weight is 1
      (`FID_NAT_OFF=remat`). Nov 15: addresses of globals (MO_ADDR) and of
      frame slots (MO_SLOTADDR) too (`ng_remat_kind`, `ng_ld_spilled`), and
      the definition of a spilled rematerialized value emits nothing
      (`ng_mir_emit`); test_machine_remat.fd.
- [~] RA9. Second-chance or backtracking allocation in the style of regalloc2 /
      LLVM's greedy, behind a switch, compared with linear scan (XL).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the greedy allocator (`FID_NAT_ON=greedy`) passes
      FID_VERIFY=1 on x86-64, AArch64, RISC-V and `--nat`, greedy_unit
      12/12, and 14 benchmarks print what linear scan's print at 0.85–1.14
      of its time. It is not consistently better (AArch64 main spills
      214 -> 236), and machine_batch12's per-function checks fail (the
      functions are inlined). Left: the full suite, greedy_compare.sh.
- [x] RA10. Register classes: integer, float, and pairs where a target needs them
      (S).
      The three classes are NG_RC_INT / NG_RC_FLOAT / NG_RC_F32
      (nat_ir.fd:337), with ng_class_pool routing by class to each pool
      (caller-saved integers, callee-saved integers, float caller-saved,
      float callee-saved — RA18). MI25 put floats in float registers on
      every target by default; FID_NAT_OFF=fregs puts every float in a
      general register (the kill switch pass-matrix.sh checks), and
      FID_NAT_OFF=winfregs the --nat Windows variant (MI14). No target
      needs pairs (each value is one 64-bit register), so there is no
      pair class: NatTarget.pairs is false everywhere. Tests:
      test_machine_fregs.fd, machine_fregs_check.sh, ra18_float_bench.sh.
- [~] RA11. Parallel moves at block edges solved with cycle breaking through one
      scratch register (M).
      (Nov 15: there are no block-edge moves yet: a range keeps one register
      or slot everywhere, so this comes with splitting, RA4.)
      Nov 17 (with RA4; to be marked done once the suite validates RA4):
      nat_enc.fd `nat_par_moves` solves the moves of each split place (an
      ending jump, a fall-through, an edge block for a conditional branch's
      taken edge) as one parallel move (nat_ir.fd `ng_ls_emit`);
      `ng_split_check` in `ng_mir_check` verifies them; no phase can form a
      cycle, so no scratch register is taken; tests/nattools/par_moves_unit.fd,
      split_alloc_unit.fd, machine_split_check.sh.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — nat_par_moves serves the splitter and is checked:
      par_moves_unit 9/9, the checker rejects the split mutation on all 4
      targets. But the FID_VERIFY=1 splitall builds fail on x86-64, RISC-V
      and Windows (the argument-register check, not a move cycle), and
      split_alloc_unit rejects 1 x64 program. Left: those, so the suite
      validates RA4.
- [x] RA12. Callee-saved registers saved only on paths that use them (shrink
      wrapping) (M).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — shrink wrapping (nat_ir.fd ng_sw_plan,
      `FID_NAT_OFF=shrinkwrap`) with CFI rows for the deferred saves:
      test_machine_sw_unwind and _shrinkwrap2 right, sw_unwind_check unwinds
      10 probe stops on x86-64, AArch64 and RISC-V, the swcfi mutation is
      caught. Left: tailpath is not shrink-wrapped on AArch64 and RISC-V
      (it saves every callee-saved register in its prologue).
- [x] RA13. Save and restore with paired instructions (`stp/ldp` on AArch64) (S).
      Nov 15: `ng_prologue`/`ng_epilogue` save and restore the callee-saved
      registers two at a time (`a64_stp`/`a64_ldp` in nat_enc.fd, the same
      slots; `FID_NAT_OFF=pairs`: one at a time); test_machine_remat.fd. The
      two forms are not yet in scripts/enc-oracle.py.
- [~] RA14. A fast mode for debug builds: one pass, local allocation, under 10% of
      compile time (M).
      Done (Nov 15): `FID_NAT_FAST=1` (or `g_nat_fast`, for main.fd to set):
      `ng_mir_alloc` skips the liveness fixed point, coalescing and spill
      weights; values that cross blocks stay in their frame slots, the values
      local to a block share the registers by one linear scan;
      test_machine_fast.fd. Nov 16 (main.fd): `fid emit --fast-alloc` (or
      `--fast`) sets `g_nat_fast` with the mid-end on, `--debug` without it
      (SP13); also on `fid emit-ir`. The function cache's key now holds the
      allocator mode (it replayed code of the other allocator before);
      tests/bootstrap/test_driver_fast_alloc.fd. Left: measuring it against
      the 10% target.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — the check found this not done: in debug mode the allocation
      is 11–13% of the build, not under 10% (test_compiler_sized 11.2%,
      test_machine_probe 12.4%, compiler/main.fd 13.1%; `--fast-alloc` with
      the mid-end is 6.1–9.5%). The main.fd default and fast-alloc rows
      panic in iro_sccp (integer overflow on subtraction). Left: a faster
      fast path, that panic, a run on a quiet machine.
      Nov 21 (5fb5516): the FID_NAT_* switches are read once per image (ng_env_cache_load, g_ng_env_ok) instead of per move (copy hints) and per call (IPRA) — a large share of a debug build's allocation time. The SCCP panic the check hit (2g.89) is also fixed (SP11).
- [x] RA15. An allocation checker: replay the allocated code symbolically and
      prove every use reads the value it should (regalloc2's checker) (L).
      Nov 8: `ng_mir_check`, an independent exact liveness walk; it found a real
      bug at once (a vreg read before its block defines it was treated as local).
- [x] RA16. Run the checker on the whole suite in CI for every target (S).
      Run on the whole suite with `FID_VERIFY=1` on AArch64, RISC-V and x86-64.
      Nov 9: `scripts/nat-gate.sh`, the gate CI runs (one job per host after
      fid is built): the verifier and the checker on every native target,
      `fid test-rules`, the z3 proofs; exits non-zero on any failure.
      Nov 15 (checked): CI's `natgate` job (.github/workflows/ci.yml) runs it
      on x86_64-linux with the default subset of 16 tests. Nov 16: the job is
      a matrix over x86_64-linux, aarch64-linux and riscv64-linux (qemu-user
      installed), each `ALL=1 REQUIRE_Z3=1 scripts/nat-gate.sh <target>`,
      logs uploaded; scripts/ci-wiring-check.sh (the `gates` job) fails if
      it is dropped. Not yet seen green on GitHub's runners.
- [x] RA17. Statistics per function: spills, reloads, moves; `--stats` prints them
      (S).
      Nov 15: `fid emit --stats` sets `g_nat_stats`; `ng_mir_stats` prints
      per-function spill/reload/move counts after allocation.
- [x] RA18. Floats and integers allocated by the same algorithm, class-aware (M).
      Nov 9, a first step on x86-64: floats still live in general registers,
      but the emitter remembers which register xmm0/xmm1 mirror and does not
      move an operand again (`FID_NAT_OFF=xmmcache`): the float loop 1834 ->
      1364 ms. Left: an xmm register class in the allocator.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — float ranges a call crosses take callee-saved float
      registers on AArch64 (31 ranges) and RISC-V (40): the fcallee tests
      pass, the mutation is caught. ra18_float_bench: AArch64 0.89 of
      spilling, RISC-V 1.07 (slower, under qemu). Left: the RISC-V cost, the
      ratio table in docs/reference/callee-saved-floats.md, programs that
      start tasks; x86-64 has no callee-saved floats in this scheme.
      Nov 21 (evening): callee-saved float registers are off on RV64
      (ng_fcs_on): the measured 1.07 said twelve d-registers saved and
      restored in every prologue cost more than the spills they avoided;
      AArch64 (0.89) keeps them. A per-function cost model can bring them
      back; the fcallee tests and the mutation are unaffected (the pool is
      empty, ranges spill).
- [~] RA19. A value used only as a branch condition stays in flags where the
      target has them (M).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_machine_brflags and _brflags2 are right, and with
      `FID_IROPT_OFF=inline,inline2` not_if, and_if, or_if and the xb_n*
      cases lose their SETCCs. In a default build both check scripts fail
      (the functions are inlined); ext_if, xb_ext and xb keep theirs; the
      x86-64/`--nat` FID_VERIFY builds of brflags2 hit the argument-register
      check. Left: those, `while a && b` heads, `&&` with a call, RISC-V
      without Zicond.
- [x] RA20. The allocator honours IPRA clobber sets (Track WP) at each call (M).
      Calls only clobber what their callee was found to clobber (WP4).

## Track TG — Thin targets: selection and encoders (20)

- [x] TG1. Instruction selection as rules: MIR patterns -> target instructions,
      tried by cost, written as data in Fides (an ISLE-like table, no new
      language) (L).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the operations still hand-selected are table rows now
      (nat_ir.fd ng_isel_rows_tg1b: DIV/REM/MULH, ASR63, I2F/F2I, ANDN,
      BLSR, CSEL, TBNZ, OPW, ALUM): isel_tg1b_check gives the same image as
      the hand-written selection on x64, x64v3, a64, rv and rv23 with fregs
      on and off, the iseldiv mutant is caught; isel_table_check passes.
      Moves, calls and prologues stay hand-written, as documented.
- [x] TG2. Each rule carries its preconditions (immediate fits, register class)
      and is tested on its own by random operands against the generic lowering
      (M).
      Nov 22: every rule declares its preconditions as bit flags on RuRule
      (RP_INT, RP_SAME_TY, RP_SHIFT_LT_64, RP_DIV_NONZERO —
      compiler/rules.fd ru_precond_ok), checked before the shape match
      fires; the selftest (`fid test-rules`) verifies each rule on 309
      random operands AND that a rule's own preconditions do not
      contradict its shape (27 rules, 0 failed). Future machine-level
      rules state immediate-fit and register-class constraints here; the
      framework is already checking them.
- [x] TG3. Own encoders, no assembler: x64 (`pe_ir_enc.fd`), AArch64 and RV64IMFD
      (`nat_enc.fd`).
      x86-64 encoder added to `nat_enc.fd` (Nov 9).
- [x] TG4. Encoder oracle tests: for every instruction form we emit, encode random
      operands and compare the bytes with clang's integrated assembler
      (`zig cc -c x.s`, `llvm-objdump`) (M).
      Nov 8–9: `scripts/enc-oracle.py`, 1,181 forms of AArch64, RV64IMFD and
      x86-64 against clang's assembler: 0 mismatches (it found `clrex #0` and a
      longer `sub rax, imm32`).
- [~] TG5. A disassembler for our own output (`fid disasm`), table-driven from the
      same encoder descriptions, so the two cannot drift (L).
      Nov 16: compiler/disasm.fd decodes what nat_enc.fd (and nat_ir.fd's raw
      words) emit on x86-64 (prefixes, REX, VEX BMI2/FMA, ModRM/SIB/rip,
      ALU, shifts, mov/lea/movzx/movsx, setcc/jcc, SSE2/4.1), AArch64 and
      RISC-V 64 (with Zba/Zbb), anything else as `.word`/`.byte`;
      `fid disasm <file.fd> [--target <t>] [--fn f]` (no file written),
      `fid disasm <ELF or PE>` by symbol, `--raw --arch <a>`.
      scripts/disasm-check.sh compares it with llvm-objdump (x86-64:
      instruction boundaries; AArch64/RISC-V: no word left undecoded).
      Nov 17: the table is done: compiler/disasm.fd "The encoding forms:
      one table" (docs/disasm-encoding-table.md), checked by
      tests/nattools/enc_table_unit.fd; scripts/disasm-check.sh now pads
      with nops for llvm-objdump (tests/nattools/disasm_pad.py) and sends
      each listed instruction back through clang's assembler.
      Left: nat_enc.fd's encoders reading the same table (the EFW_*
      constants) instead of their own constants.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — compiler/disasm.fd decodes from one table (enc_table_unit
      ok); disasm-check: x86-64 instruction starts agree with objdump,
      AArch64 and RISC-V round trips all ok, but the x86-64 round trip fails
      on 3 tests (`shl rbx, 1` emitted as C1 /n 1 reassembles as D1 /n).
      Left: nat_enc.fd's encoders still use their own words (no EFW_ use),
      and that round trip (emit D1 or accept the alternative).
      Nov 22: the round trip half done — x64_shift_imm emits the D1 /ext
      form for n == 1 (one byte shorter, the canonical form clang gives
      `C1 /ext 1` back as); disasm already decodes D1. Validated with a
      built -O0 fid: enc-oracle 1192 forms 0 mismatches, and a 40-step
      shift loop prints the same on pe_ir, --nat and the interpreter.
      The EFW_ unification is still open (nat_enc.fd uses its own
      constants, disasm.fd has all 411 EFW_ references).
- [x] TG6. Branch relaxation: short forms where the target reaches, long forms
      only when needed (x64 `jcc rel8`, RISC-V `b` vs `jal` vs `auipc+jalr`) (M).
      Nov 15: `ng_mir_emit` (nat_ir.fd) emits a function with every branch
      long, then again from the same position with each branch whose
      target stays in reach however the layout changes (loop-head padding
      counted at its largest) short: x86-64 `jmp`/`jcc rel8`
      (`x64_jmp_short`/`x64_jcc_short`), RISC-V one `B<cc>` instead of
      `B<inverse>` over `JAL` (`rv_b_label`), RISC-V calls to an
      already-placed callee one `JAL ra` instead of `AUIPC+JALR`
      (`rv_call_near`). New fixups NATFIX_X64_REL8 / NATFIX_RV_B12 check
      the reach (out of reach is a build error, never a wrong jump).
      AArch64 needs none (every form is one word). `FID_NAT_OFF=relax`: all
      long. Test: tests/bootstrap/test_machine_relax.fd (a loop body of a
      few KB, short and long branches). Not yet run: the final build.
- [x] TG7. x64: memory operands in arithmetic, `test`, `lea` everywhere it pays,
      `cmov`, BMI1/BMI2 behind `--cpu` (M).
      Done (Nov 15): `test r, r` for every compare with zero (ng_bnez,
      ng_brcc, before); `lea` for element addresses (MO_ADDSHL, MI22) and
      now for every three-operand add (`ng_x64_bin`, no MOV first;
      `FID_NAT_OFF=lea`); BMI2 SHLX/SARX/SHRX for shifts by a register
      under an explicit `--cpu x86-64-v3|v4` (`ng_x64_bmi2`,
      `x64_shiftx`; the oracle forms in scripts/enc-oracle-tg.py). Test:
      tests/bootstrap/test_machine_isel.fd.
      Nov 17: memory operands for spilled values: ALU second operand
      (memop), compares (cmpslot: `cmp r, [rsp+off]`, ng_x64_cmp_slot),
      read-modify-write of a spilled destination (rmwslot:
      `op [rsp+off], r|imm`, ng_rmw_slot); cmov (MI24 MO_CSEL); BMI1 ANDN,
      BLSR, BLSMSK, BLSI under --cpu x86-64-v3|v4 (MO_BLSR kinds,
      ng_fuse_bits). Tests: tests/nattools/tg78_unit.fd,
      tests/bootstrap/test_machine_tg7.fd,
      tests/nattools/machine_tg78_check.sh, enc-oracle-tg78.py;
      docs/reference/slot-operands-and-writeback.md.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — cmpslot (90 slot compares) and rmwslot (56 slot
      read-modify-writes), none with them off; blsmsk/blsi under x86-64-v3,
      none on the baseline; the mutants are caught (they hang rather than
      print wrong output); tg78_unit ok on 3 targets, enc-oracle-tg78 2310
      forms 0 mismatches, test_machine_tg7 9/9.
- [x] TG8. AArch64: `madd/msub`, shifted and extended operands, `cbz/tbz`,
      `csel/csinc`, `ldp/stp`, pre/post-indexed addressing (M).
      Already in nat_ir/nat_enc (verified Nov 15): `msub` for remainders
      (ng_rem), `add ..., lsl #s` for element addresses (ng_addshl, MI22),
      extended-register `add/sub` for sp (`a64_add_ext`), `cbz/cbnz` for
      compares with zero (ng_brcc, MI23), `cset` (= `csinc`) for compares,
      `stp/ldp` for callee-saved registers (RA13), pre-/post-indexed
      `stp/ldp x29, x30` in prologue and epilogue. Done: madd/msub
      (ng_a64_mac, FID_NAT_OFF=madd), tbz/tbnz (MO_TBNZ, ng_fuse_tbz), csel
      (MO_CSEL, MI24), and (Nov 17) pre/post-indexed loads and stores in
      loops: ng_wb_pair moves a pointer step next to its access in the MIR,
      ng_a64_wb_fuse emits `ldr/str t, [n], #c` / `[n, #c]!` after
      allocation (a64_ldr_wb / a64_str_wb; FID_NAT_OFF=wbidx). Tests:
      tests/bootstrap/test_machine_tg8.fd, tests/nattools/tg78_unit.fd,
      machine_tg78_check.sh, enc-oracle-tg78.py.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — machine_tg78_check: 17 loads/stores with writeback on
      AArch64, 0 with `FID_NAT_OFF=wbidx`, the mutant caught, outputs right
      on aarch64, x86_64, riscv64 and `--nat`; enc-oracle-tg78 2310 forms 0
      mismatches; test_machine_tg8 8/8. Float-register accesses are not
      fused yet, as documented.
- [x] TG9. RISC-V: compressed instructions (RVC) for size, Zba/Zbb when present,
      `sext.w` elision (M).
      Done (Nov 15): Zba/Zbb under `--cpu rva22u64|rva23u64|rv64gc_zba_zbb`
      (`ng_rv_zb`; `nat_cpu_check` accepts the names): element addresses
      by one `sh1add/sh2add/sh3add` (ng_addshl), extensions by one
      `sext.b`/`sext.h`/`zext.h`/`zext.w` (ng_ext) where rv64gc takes two
      shifts; encoders `rv_shadd`, `rv_zext_w`, `rv_sext_b/h`, `rv_zext_h`,
      checked against clang by scripts/enc-oracle-tg.py. Left: RVC (the
      encoder writes 4-byte words only; NatEnc and every fixup assume it)
      and `sext.w` elision (the lowering has no 32-bit `*w` operations to
      elide after).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — RVC (nat_enc.fd nat_rvc_of/expand/compress, ng_rvc_pass):
      rvc_unit ok, enc-oracle-tg9 1238 forms and 36,206 RVC forms (2588 in 2
      bytes) with 0 mismatches, rvc_check 77 ok (its 3 failures were out of
      memory and right when rebuilt), test_machine_rvc 13/13; MO_OPW
      covers the `*w` operations. Not run: the riscv64 cross-test with RVC.
- [x] TG10. A `--cpu` switch per target with safe defaults (baseline x86-64-v2,
      armv8.0-a, rv64gc) (S).
      Nov 15: `fid emit --cpu <name>` is checked by `nat_cpu_check` —
      x86-64 requires v3 (the runtime's fma needs FMA3), AArch64 armv8.0-a..
      armv9-a, RISC-V rv64gc; `baseline`/`native` accepted; unknown names
      are a build error. `nat_cpu_default` records the safe default in
      `g_nat_cpu` for the selections that will differ.
      Nov 15: selections now differ: `x86-64-v3|v4` turns on BMI2 (TG7),
      `rva22u64`/`rva23u64`/`rv64gc_zba_zbb` (new names) Zba/Zbb (TG9).
- [~] TG11. SIMD as MIR vector operations: SSE2/AVX2, NEON, RVV, for the `simd`
      module and loops the vectorizer picks (XL).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — vector values get frame slots (the vectorizer spills to 4–6
      vector slots), stdlib/simd.fd has the integer intrinsics,
      enc-oracle-tg11 113 forms 0 mismatches, the sse/avx2/neon/rvv images
      are right when run by hand. Left: test_machine_simd case_n fails the
      allocation check on every x86-64 target under FID_VERIFY=1, the pe_ir
      build of test_simd_i64 crashes in its wrap section, reductions (red6,
      redk) are not vectorized.
      Nov 21 (5fb5516): the test_simd_i64 crash the check hit (2g.96) was an itoa scratch buffer under saved r14 in pe_ir_stubs, not SIMD codegen — fixed; the sse/avx2/neon/rvv images were right.
- [~] TG12. Object files (COFF, ELF, Mach-O) from the same writer as executables,
      for linking with C (release F4) (M).
      Nov 16, ELF: `fid emit <file> --obj --target <linux> -o x.o` (main.fd
      `nat_write_object` → nat_ir.fd `nat_emit_object`, which builds the
      image as for an executable, → nat_enc.fd `nat_elf_object`): an
      ET_REL with one page-aligned `.text` (code and data, as the
      executable's one segment), a RELA relocation per absolute word
      (R_X86_64_64 / R_AARCH64_ABS64 / R_RISCV_64 against the section),
      a local symbol per function, the global `fid_main` (the entry: argc
      and argv from the stack, main, exit) and `.note.GNU-stack`. Design
      (conservative): the functions keep the machine layer's own calling
      convention, so they stay local; `ld -e fid_main x.o` gives the
      program. A program without `main` is a library build (WP20). Test
      program test_nattools_object.fd, scripts/nat-tools-check.sh (not run
      yet). Nov 18: COFF for `--nat` (nat_enc.fd `nat_coff_object`) and
      Mach-O objects (`nat_macho_object`) wired in nat_ir.fd
      `nat_generate_image` / `nat_emit_object` (not run yet with a built
      compiler); checked by tests/nattools/tg12_objects_check.sh. All
      three writers make g_nat_obj_exports global symbols. Left: a run of
      those two, C-callable exports: a wrapper in the C convention per
      `@export` function (save the C callee-saved registers, C argument
      registers to the machine layer's, the result back to rax / x0) and
      g_nat_obj_exports / g_nat_obj_export_offs set around the object
      writer. Not written yet because the runtime's start-up cannot run
      inside a C host as the entry runs it: pe_rt_init sets the thread
      pointer (arch_prctl FS on x86-64 Linux, tpidr_el0, tp), which is
      glibc's in a C program, and installs signal handlers and the
      profiler. A wrapper needs the host's thread pointer swapped for the
      runtime's around each call (or a runtime TLS that leaves it alone)
      and a once-per-thread init.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — tg12_objects_check: the COFF object links with lld and
      prints what the PE prints, a C program links and calls fid_probe, the
      x86_64/aarch64 Mach-O objects link. `fid emit --obj --nat` on a real
      program fails (the runtime's LoadLibraryA/GetProcAddress/FreeLibrary
      undefined). Left: that; running the Mach-O objects (no Mac).
      Nov 21: see RL9 — `emit --obj --nat` and `codegen --coff` resolve the runtime's kernel32 imports now (2g.110).
- [x] TG13. One image writer for PE, ELF and Mach-O: sections, relocations, symbol
      tables, written once (L).
      compiler/image_writer.fd; every writer (nat_enc.fd nat_elf /
      nat_elf_debug / nat_elf_object / nat_pe / nat_pe_symbols / nat_macho /
      nat_macho_fat, TG12 nat_coff_object / nat_macho_object, pe_ir_enc.fd
      pe_build_file / pe_build_efi / pe_efi_relocs / COFF symbols / debug
      sections, coff.fd, elf64.fd, macho.fd, linker.fd) is a layout over it.
      Old bodies checked byte for byte and removed Nov 17 (FID_IMG_OLD is
      gone). Unit tests pin the bytes by digest:
      tests/nattools/image_writer_unit.fd, image_writer_obj_unit.fd; real
      programs: tests/nattools/image_same_check.sh. docs/image-writer.md.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — no old writer body is left (only a comment names
      FID_IMG_OLD); image_writer_unit and image_writer_obj_unit match their
      bytes; image_same_check on hello: 11 formats the same (pe-g differs
      only in the PDB path it names). The coff format is skipped there (the
      `codegen --coff` failure, RL9/TG12).
- [x] TG14. Position-independent executables on Linux and macOS (ASLR) (M).
      macOS: already PIE (verified Nov 15: `nat_macho` sets MH_PIE and dyld
      rebases every absolute word, F7). Linux, done (Nov 15) behind
      `FID_NAT_ON=pie`: an ET_DYN at link address 0 (`g_nat_pie`,
      `nat_elf_image_base` in nat_enc.fd), no dynamic loader — `_start`
      first adds the load bias to every absolute word from a table after
      the data (`ng_gen_pie_reloc`, `ng_gen_pie_table` in nat_ir.fd; x86-64,
      AArch64, RISC-V). Test program: tests/bootstrap/test_machine_pie.fd;
      check `FID_NAT_ON=pie NATIVE=1 bash scripts/cross-test.sh <target>`.
      Nov 17: PT_PHDR for a PIE with PT_INTERP (nat_enc.fd nat_elf_img),
      ready for lifting ng_ldyn_ok's PIE exclusion; .interp/.dynstr/.dynamic
      section headers in debug images. Nov 18: `fid emit --pie` /
      `--no-pie` (main.fd, g_nat_pie_req; neither: FID_NAT_ON=pie).
      Left: the ng_ldyn_ok hunk (PIE with dynamic imports,
      tests/nattools/pie_ldyn_prog.fd), the FID_NAT_ON=pie suite run on the
      three Linux targets, then PIE the default (nat_pie_wanted); the image
      is still one RWX segment.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — PIE on the three Linux targets: image_writer_unit's PIE
      lines and machine_batch7's TG14 lines pass (split ET_EXEC by default,
      pie ET_DYN), the images run (x86_64 in WSL, aarch64/riscv64 under
      qemu); a dynamically linked PIE (pie_ldyn_prog.fd) runs; `emit --pie`
      gives ET_DYN; 36 of a 40-test PIE sample pass, the other 4 fail
      without PIE too. The image is no longer one RWX segment. Left: PIE the
      default, the full `FID_NAT_ON=pie` suite.
- [x] TG15. Dynamic linking of system libraries where a platform needs it (libc
      on macOS, `kernel32` on Windows already), through import tables written by
      us (L).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — import tables we write (nat_enc.fd nat_pe_dll_for,
      nat_pe_win64_stub): tg15_win64_stub_check ok (13 Windows x64 and 12
      Apple arm64 stubs agree with clang), tg15_win_dll_check ok (92 imports
      from 7 DLLs, output right), an aarch64-macos binary binds _ldexp,
      _atoi and _hypot through libSystem, and a Linux libc/libm PIE runs.
      The Mach-O binaries were checked for structure only (no Mac here).
- [~] TG16. A WASM target on MIR: structured control flow from MIR's CFG
      (relooper / stackifier), our own module writer, replacing the invalid
      `wasm.fd` output (L).
      Done (Nov 15): `wasm.fd` is no longer invalid: rewritten as the
      machine layer's lowering on WASM's stack machine, its own module
      writer, i32 addresses, a funcref table, WASI preview 1 through
      stdlib/linux_runtime/wasi.fd; control flow is a "switch loop" (one
      loop, br_table to each IR block), not a stackifier. 316 of 344 suite
      tests match `fid run` (scripts/wasm-test.sh `ALL=1`). Float `%` and
      f32 <-> f64 casts fixed the same day (known-bugs 2g.13). Left: the
      port onto MIR itself (one lowering with the native targets) and a
      relooper/stackifier in place of the switch loop.
      Nov 18 (not yet run with a built compiler): control flow is
      structured. compiler/wasm.fd "Structured control flow" is a
      stackifier (Ramsey's dominator-tree translation: loop / block / if,
      br / br_if); the switch loop stays only for functions that may
      switch tasks and for irreducible CFGs; FID_WASM_SWITCH=1 forces it.
      Checked by tests/wasm/stackify-check.sh (valid modules, the same
      output as the switch loop), which CI's wasm32 lane runs;
      scripts/wasm-test.sh counts a module Node does not validate
      (tests/wasm/validate.mjs) as BUILD. Left: the port onto MIR (one
      lowering with nat_ir; it needs MIR target-neutral enough for a stack
      machine, e.g. an emit hook over NatFnRec.mir that wasm.fd walks
      instead of IR: not started).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the stackifier: stackify-check passes 10 of 10 (e.g.
      test_wasm_collections, 304 functions structured), the modules
      validate, both builds print the same, structured modules are about 9%
      smaller. Left: the target on MIR (wasm.fd still lowers from IR).
- [x] TG17. Peephole rules per target as data, after allocation: redundant moves,
      load after store, branch to next block (M).
      Already (verified Nov 15): branch to the next block, jump threading
      and redundant moves go in the shared MIR peephole before allocation
      (`ng_peep`, OP19); after allocation the emitter drops a move whose
      registers came out equal (`ng_mir_emit`). Left: rules as per-target
      data after allocation, load after store.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — per-target post-allocation rules (ng_post_rules) with load
      forwarding, also past a move (ldfwdmov): machine_tg17_check 0
      failures (x86_64 59 loads forwarded, 40 without ldfwdmov, none with
      ldfwd off; aarch64 9, riscv64 13, `--nat` ok); pass-matrix on a
      24-file sample: 0 switches differ on 3 targets.
- [x] TG18. A shared peephole pass on MIR before allocation: dead flags, double
      extension, constant compare (M).
      The pass is `ng_peep` (OP19: redundant and dead moves, load
      forwarding, branch to next block, jump threading). Nov 15:
      `ng_peep_fold` adds, block by block, a compare of two constants ->
      a constant, a branch on a constant -> nothing or a jump
      (`while true`), a check on a known zero -> nothing, an extension of
      an already extended value that keeps it -> a move
      (`FID_NAT_OFF=mirfold`). Dead flags: the MIR carries no flags between
      operations, so there is nothing to remove. Test:
      tests/bootstrap/test_machine_isel.fd. Not yet run: the final build.
- [x] TG19. Code size mode (`--size`): RVC, short encodings, no unrolling, shared
      panic blocks (S).
      Pieces already on by default (Nov 15): shared cold panic blocks per
      function (MI6), short branch encodings (TG6). Nov 16: `fid emit
      --size` (main.fd; also `emit-ir`) sets nat_ir's `g_nat_size`: loop
      heads are not padded to 16 bytes (ng_code_align) and Vec calls are not
      inlined (ng_gen_call); the mid-end's unrolling is off (checks.fd
      iro_cli_off: `--size` reads as `--no-pass unroll`); the function
      cache keys on it. tests/bootstrap/test_driver_fast_alloc.fd (the same
      output, an image no larger: scripts/driver-goals-check.sh). Left: RVC
      (compressed RISC-V encodings in nat_enc.fd).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — RVC is on by default for RISC-V Linux (NG_RVC_DEFAULT; off
      with `FID_NAT_OFF=rvc` or `--cpu rv64g`): tg19_size_check 0 failures;
      riscv64 images 1,119,752 -> 989,216 bytes by default (11% smaller),
      982,992 with `--size`; `fid disasm` lists the 2-byte forms. Not run:
      the riscv64 cross-test with RVC.
- [x] TG20. A new target costs under 3,000 lines: description, rules, encoder,
      image writer; measured by the RISC-V port (S).
      Measured Nov 15: the RISC-V 64 port is ~500 lines (encoder + target
      description in nat_ir/nat_enc), well under 3,000.

## Track ME — SSA and a rules mid-end (30)

- [x] ME1. Use lists in the IR, kept current by every pass (M).
      `IroUses`/`iro_uses` in `compiler/iropt.fd`: users per value and the
      block each use sits in; passes rebuild it where they need it (GVN,
      branch facts, forwarding, DCE).
- [~] ME2. Dominator tree and loop tree shared by every pass (`iro_cfg`,
      `iro_dominates` in `iropt.fd` are a start) (M).
      iropt's dominators are used by the IR verifier, the e-graph and forwarding.
      Done (Nov 15): `IroCfg`/`iro_cfg`/`iro_dominates` moved to
      `compiler/checks.fd`, which every mid-end module imports; iropt_me's
      own CFG builder (`ImeCfg`) is gone, its passes read `iro_cfg` and the
      O(1) interval dominance. A loop tree, `iro_loops` (headers innermost
      first, latches, innermost header per block, parent loop, depth),
      computed on demand; `iro_licm` (was its own CFG and DFS back edges)
      and `iro_iv` find their loops through it (test
      `tests/bootstrap/test_midend_loop_tree.fd`). Left: iropt_me's
      `ime_headers`/`ime_latches`/`ime_loop_body` still derive loops
      themselves (over the shared dominators), and checks.fd's counted-loop
      analysis, pe_ir and nat_ir keep CFGs of their own.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — ssa.fd's own loop walk is gone (ssa_exit_values takes
      iro_loop_body from the shared loop tree), ime_loop_body and pe_ir's
      successors use iro_cfg; the four loop-tree tests are right under
      FID_VERIFY=1 on pe_ir and `--nat`. Left: nat_ir's own IR-level
      successor walk (ng_block_succs), kept until ir_verify checks
      terminators.
- [~] ME3. mem2reg: locals whose address never escapes become SSA values with
      block parameters, on the IR every backend reads (L).
      Nov 8–9: scalar locals whose address is only loaded and stored live in
      registers in the backend, and a load becomes the stored value when that
      store is the local's only one and dominates it, or is earlier in its block
      (`compiler/egraph.fd`); no phis yet, the IR is unchanged.
      Nov 9: `iro_promote` (pass `promote` in `compiler/iropt.fd`) generalizes
      this in the mid-end: per-alloca reaching-store dataflow replaces every
      load that has a single reaching store across the whole CFG. Merges
      where predecessors disagree (or a store meets an uninitialized path)
      still need phis — Jump/Branch carry no edge arguments — and stay in
      memory. Nov 9 (later): two soundness bugs fixed — store operands are
      re-read live (`iro_store_val`) so an earlier promotion cannot leave a
      dangling id, and loads read by `TryUnwrap`/`Asm` are never dropped
      (`iro_unhandled_uses`); regression `tests/bootstrap/test_mem2reg_promote.fd`,
      log M8.
      Done (Nov 16): the merge case, without block parameters in any
      backend (decision: SSA lives only inside the mid-end). `compiler/ssa.fd`
      `ssa_round_trip` builds SSA from the slot form (dominance frontiers
      of the shared CFG, phis at iterated frontiers, renaming over the
      dominator tree), drops trivial and dead phis, and converts back before
      any backend reads the IR: each phi left gets a slot of its own, every
      predecessor stores into it before its terminator, the phi is a load at
      the top of its block (no edge splitting needed: only that block reads
      the slot). A local a read of which can see no store stays in memory.
      Opt-in, `FID_IROPT_ON=ssa` (run last in `ime_function_passes`, so
      fold/sccp/dce/gvn see the values; `FID_IROPT_OFF=ssa` the kill
      switch); test `tests/bootstrap/test_ssa_merge.fd`. Left: run the
      suite with it on and off, then make it the default (and retire
      `promote`, which it subsumes).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — ssa is on by default and promote is retired where ssa runs
      (ssa_takes): ssa-default-check passes, promote-retired-check passes 14
      of 15, but test_matrix_mul has about 2% more instructions without
      promote (6048 vs 5930 after iropt). Nov 21 (evening): promote runs
      after ssa too now (iro_promote_here no longer retires it where ssa
      took the function: the post-ssa slot form is exactly what promote's
      reaching-store dataflow works on, and the slots ssa left alone are
      its wins). Measured: matmul2x2 is 220 instructions either way —
      the 2% was matrix_mul's main, whose callers inline it; promote's
      win there is the reaching-store replacement ssa's phis now cover.
      Left: the rest of the
      NAT=1 file list.
- [x] ME4. An IR verifier after every pass in debug builds: types, dominance,
      terminators, block parameters (M).
      Nov 8: `compiler/irverify.fd` (`FID_VERIFY=1`), after lowering, iropt, the
      rules and the e-graph; it found 3 lowering bugs (2 fixed in lower.fd by the
      ownership session). Nov 9: wired into the Windows PE emit path too
      (`main.fd`, checks after lowering and after `iropt_native`) — it does
      not flag semantic miscompiles like M9 (structurally-valid-but-wrong IR).
- [x] ME5. One pass manager: order, per-pass timing, a switch to turn each off
      (`--no-pass=name`), and `iro_pass_on` folded into it (M).
      Switches per pass: `FID_IROPT_OFF`, `FID_NAT_OFF` (mir, promote, ipra, fold,
      hint, rules, purecse, egraph, forward, debug); timing in `[prof] native`
      lines. Nov 9: `iro_time` wraps every iropt pass; `FID_IROPT_STATS=1`
      prints `iropt: pass <name>: N ms (K calls)` most expensive first.
      Checked Nov 15: the `--no-pass` flag did not exist (only the
      variables) and devirt and the module passes were not timed. Now:
      `--no-pass=a,b` / `--no-pass a,b` on any command adds to the off lists
      of iropt, iropt_me, the rules and the e-graph (`iro_cli_off`/
      `iro_named_off` in `compiler/checks.fd`, read by `iro_pass_on`,
      `ime_on`, `ru_on`, `eg_on`; main.fd
      ignores flags it does not know, so it needs no change), and
      `iro_time_note` times closuredevirt, hofspec, inline, inline2,
      envtrim, ipcp, spec, gconst, devirt, deadf and the iropt_me passes
      (`midend`). nat_ir's own switches (`FID_NAT_OFF` mir, ipra, ...) do
      not read the flag.
- [x] ME6. An e-graph data structure: e-classes, hash-consed e-nodes, union-find,
      rebuilding (L).
      Nov 9: `compiler/egraph.fd`: e-classes, hash-consed e-nodes, union-find,
      rebuilding.
- [x] ME7. Pure values of a function go into the e-graph; side-effecting
      instructions stay a fixed skeleton (Cranelift's aegraph) (L).
      Pure integer and boolean values are e-nodes, everything else a leaf.
      Checked Nov 15: only constants and binary operations were nodes (the
      unary node kinds existed, nothing made them, so `!!b = b` never fired),
      and an operation with a parameter operand was left out (a parameter
      has no meta type). Done (Nov 15, `compiler/egraph.fd`): `-x`, `~x`,
      `!b` are nodes with their own cost, constants fold
      (`eg_fold_unary`: not the negation of the minimum, nothing outside a
      narrow or unsigned type), `!!b = b` and `~~x = x`, `-(-x) = x` in
      @wrapping code; parameters take part through `eg_value_ty` (test
      `tests/bootstrap/test_midend_egraph_unary.fd`). Left: integer casts and
      float values are still leaves; the skeleton is the IR's instruction
      order (no separate side-effect skeleton, see ME8).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the side-effect skeleton is written (egraph.fd eg_skeleton,
      opt-in `FID_IROPT_ON=egskel`): test_midend_egskel right, egskel-check
      9 PASS on `--nat` (1063 values floated, 0 undone), all 8 x86_64-linux
      images right, a 60-test sample 52 SAME, 0 differ. Left: the full
      suite on `--nat` and Linux, the pass matrix, then the default.
      Nov 21 (evening): on by default (egraph.fd: g_eg_skel = on_e &&
      eg_on("egskel"); FID_NAT_OFF=egskel turns it off) on the strength of
      those runs; the full suite with it on is still owed.
- [x] ME8. Extraction by cost, with each value placed at the latest dominating
      point that covers its uses: LICM and sinking fall out (L).
      Extraction by cost; an instruction is rewritten in place into its class's
      best form when that form's operands dominate it. Placement-based LICM is
      still to do.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — placement (egraph.fd eg_place) is on by default on the
      native paths: egplace-default-check passes (placed 21, hoisted 4; both
      off switches give identical images; 20 programs the same on, off and
      without the e-graph); a 60-test sample gives pe_ir 60 SAME, `--nat`
      53 SAME (2 builds failed for other reasons). Not run: the whole suite
      and a Linux target.
- [x] ME9. Rewrite rules as data: a pattern, a replacement, conditions on types
      and constants; a file of rules per area (arithmetic, comparisons, bits,
      floats) (M).
      Nov 8: `compiler/rules.fd`, 27 rules as data.
- [x] ME10. Constant folding as rules, with the language's overflow semantics (a
      fold that would panic is not folded) (S).
      Folding under the language's checked arithmetic (a fold that would panic is
      not folded).
- [x] ME11. CSE and GVN from hash-consing, across blocks by dominance (M).
      Through the e-graph: an earlier equal value that dominates becomes a move.
      Checked Nov 15, already done: `iro_gvn` (`compiler/iropt.fd`, pass
      `gvn`, on by default) hash-conses every pure instruction by a key
      (`iro_gvn_key`) in reverse postorder and reuses the earlier value whose
      block dominates (`iro_dominates`), loads of plain locals within a
      block included; the e-graph (`eg_member_before`) does the same for its
      classes after rewriting, so equal forms (`a + b` / `b + a`) merge too.
- [x] ME12. Algebraic simplification: identities, reassociation of constants,
      strength reduction of multiplication and division by constants (M).
      Identities as data in `rules.fd` plus `iro_simplify`: `x+0`, `x-0`,
      `x*1`, `x*0`, `x|0`, `x^0`, `x&0`, `x&x`, `x|x`, `x^x`, `x<<0`, `x>>0`
      (integers only — no float rules that would change NaN/-0.0, nothing
      that would drop a division or shift panic); `(x+c1)+c2` reassociation
      keeping overflow semantics; `*2^k`→`<<`, `/2^k`→`>>`, `%2^k`→`&` under
      the documented safety conditions. Nov 9: a native run of the suite with
      `simplify`, `branchfold` and `dse` on and off differed on some
      programs; turning one pass off at a time puts every difference on
      `branchfold` (10 programs, wrong booleans); `simplify` and `dse` change
      no output. The three are opt-in (`FID_IROPT_ON`); `branchfold` stays off
      (SCCP folds constant branches anyway).
- [x] ME13. Comparison and branch simplification: `!(a < b)` to `a >= b`, known
      results from dominating conditions (M).
      Comparisons flipped and same-operand comparisons folded. Nov 9: pass
      `branchfacts` — where a branch target has that branch as its only
      predecessor, uses of the condition and comparisons over the same
      operands are folded to constants through the dominated region (set-mask
      algebra over {lt,eq,gt}).
- [x] ME14. Sparse conditional constant propagation over the CFG, dead branches
      removed (M).
      `iro_sccp` (pass `sccp`): top/const/varying lattice over the reachable
      CFG in RPO (sound without iteration — single-def values, defs dominate
      uses); folds integer binops/unops/trunc/extend/bitcast under checked
      arithmetic (a fold that would panic stays varying); constant branch
      conditions become jumps, the dead edge's predecessor link is dropped
      and unreachable blocks removed.
- [x] ME15. Dead code and dead store elimination on SSA (M).
      `iro_dse`/`iro_dce_more` run on the current single-def IR; "on SSA"
      proper waits on ME3 phis.
      Done (Nov 16): on the SSA form of `compiler/ssa.fd` (ME3): phis nothing
      reads (directly or through other phis) are dropped, every store to a
      promoted local is removed (its value has a name), then `ssa_dce`
      drops the values left unread (constants, loads of locals, comparisons
      and bit operations; never what may panic). Opt-in with `ssa`. Left:
      the suite run with `ssa` on and off.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — on by default with `ssa` (`ssa`, `ssaexit` in
      IME_DEFAULT_ON; `FID_IROPT_OFF=ssa`): ssa-default-check 0 failing on
      pe_ir and `--nat`, test_ssa_merge right under FID_VERIFY=1, a 100-test
      sample with ssa on and off 97 SAME, 0 differ.
- [x] ME16. Each rule tested on its own: random inputs evaluated before and after
      the rewrite, in `fid test-rules` (M).
      `fid test-rules`: each rule on edge and random values.
- [x] ME17. Each integer rule proved by an SMT solver on 64-bit vectors (an
      offline check, run in CI; z3 as a tool, not a dependency) (L).
      `fid test-rules --smt <dir>` writes 108 obligations (27 rules at 8, 16, 32,
      64 bits), `scripts/rules-smt.sh` runs them. Nov 9: z3 5.3.3 (the
      z3-solver wheel, through its Python module) proves all 108; a rule
      broken on purpose gives `sat`. Nov 16: in CI. ci.yml's natgate job
      installs the z3-solver wheel and runs `scripts/rules-smt.sh` after the
      one-stage build (x86_64 entry); nat-gate.sh with `REQUIRE_Z3=1` counts
      a missing solver as a failure on every target.
- [x] ME18. Rule statistics: how often each fires on the suite; rules that never
      fire are deleted (S).
      `g_rule_hits` in `compiler/rules.fd`; `FID_RULE_STATS=1` prints
      `[rules] <name>: <hits>` per rule at the end of `rules_run`.
- [x] ME19. Saturation limits per function (nodes, iterations), so compile time
      stays linear in practice (S).
      Node and round limits (`EG_MAX_NODES`, `EG_MAX_ROUNDS`).
- [~] ME20. Inlining with a cost model across modules (iropt inlines one-block
      functions of up to 16 instructions today) (L).
      Nov 9: written, opt-in (`FID_IROPT_ON=inline2`), not run on the suite yet: callees of
      several blocks (the block is split at the call; returns store the
      result and jump on), up to 60 instructions, 400 for a callee called
      once, a caller kept under 4,000 (`compiler/iropt_me.fd` `ime_inline`).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — inline2 is on by default, its limits settable per build
      (FID_INLINE2_LIMITS) with a stats line: inline2-limits-check and
      inline2-check pass. inline2-tune.sh does not exit 0 (SLOWRUN, likely
      machine load), and with inline2 on test_regex_linear fails the
      `--nat` allocation check under FID_VERIFY=1 (only
      `FID_IROPT_OFF=inline2` fixes it). Left: those, the full suite.
      Nov 21: 2g.88 fixed (see RA3) — envtrim and inline2 exposed more cases of the checker miss, not new bugs.
- [~] ME21. Inlining decisions from the call graph bottom-up, with sizes known
      after the callee is optimized (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=inline2`), not run on the suite yet: post-order over
      the call graph: a callee is inlined into its callers after its own
      callees were.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the bottom-up inliner now resumes its scan after an inlined
      site and inlines the same sites: "resumed scan = rescan" on 5
      programs (e.g. test_lib_bigint 1178 vs 2800 weighings), same output.
      Left: the full suite on and off, the compile-time measurement
      (me-measure.sh), the `--nat` allocation-check failure of
      test_regex_linear with inline2.
- [~] ME22. Loop canonical form: preheaders, single latches, dedicated exits (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=loops`), not run on the suite yet: all three
      (`ime_canonical_loops`).
- [x] ME23. Induction variables: strength reduction, exit-value replacement,
      rotation shared by every backend (`PEIR_ROTATE` moves out of pe_ir) (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=ivsr`), not run on the suite yet: i * c in a
      loop becomes a second stepped variable, in @wrapping functions only
      (elsewhere the stepped copy could overflow where i * c is never
      computed). Exit values and rotation are not done.
      Done (Nov 16): exit values on the SSA form (`compiler/ssa.fd`
      `ssa_exit_values`, `FID_IROPT_ON=ssa,ssaexit`): for `i = c0; while i
      < N { ...; i = i + 1 }` (also `<=`, `!=`) with constants c0 and N, an
      i64 local, and the header's branch the loop's only exit to a block only
      the header enters, uses of i after the loop become the constant
      (max(c0, N); N + 1 for `<=`). Test `tests/bootstrap/test_ssa_merge.fd`.
      Left: exit values for bounds that are not constants (need a select or
      a guard); rotation stays `PEIR_ROTATE` in pe_ir (moving it is a
      backend change); the suite run with `ssaexit` on and off.
      Nov 17: exit values for bounds that are values (`ssa_exit_values`
      takes a start and a bound from before the loop; for `<` and `<=` a
      guard on the way in must prove start <= bound: `ssa_guard_le`). Test
      `tests/bootstrap/test_ssa_exit_bound.fd`, check
      tests/tooling/ssaexit-check.sh. IR loop rotation `rotate`
      (iropt_me.fd `ime_rotate`, run last on each function; the counted-loop
      proof knows the rotated shape: checks.fd `peir_rotated_test`), opt-in
      (`FID_IROPT_ON=rotate`) until a benchmark run says no program is
      slower (nat_ir's vectorizer looks for a Jump into a loop); pe_ir's
      `PEIR_ROTATE` stays meanwhile and does not fire twice. Test
      `tests/bootstrap/test_midend_rotate.fd`, check
      tests/tooling/rotate-check.sh; docs/perf/mid-end/me23-me25-rotate-exit-version.txt.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — exit values from bounds that are not constants work
      (ssaexit-check 0 failed on pe_ir and `--nat`), but rotate-check fails
      1: with rotation on, the counted-loop proof no longer removes
      sum_vec's bounds check. Rotation stays opt-in and PEIR_ROTATE stays in
      pe_ir. Left: that, SUITE=1.
      Nov 21 (evening): fixed — a rotated loop of one body block is a
      SELF-loop (the head ends in Branch(test, head, out), its own latch),
      which peir_counted_loop took down the normal path and then refused
      ("the step may be in the header"; checks.fd now treats a branch to
      the head itself as the rotated shape). With FID_IROPT_ON=rotate,
      sum_vec's bounds check is removed by the counted loop again
      ("removed bounds check (counted loop)"); rotate stays opt-in and
      PEIR_ROTATE unchanged until rotate-check's suite leg.
- [x] ME24. Loop unrolling by a small factor for short bodies with known trip
      counts (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=unroll`), not run on the suite yet: a one-block loop
      of at most 8 iterations known on entry is unrolled fully (`ime_unroll`);
      partial unrolling is not done.
      Done (Nov 15): partial unrolling in `ime_unroll_one`: a one-block body
      of at most 40 instructions whose known trip count is over 8 and a
      multiple of 4 (or 2) gets 4 (or 2) chained copies inside the loop, the
      header testing once per group (the skipped tests would all be true and
      only read); test `tests/bootstrap/test_midend_unroll_partial.fd`.
      Left: still opt-in (`FID_IROPT_ON=unroll`) until a suite run agrees
      with it on and off; no remainder loop (a trip count the factor does
      not divide is left alone), and trip counts must be constants.
      Nov 17 (competitive OP11): on by default (`unroll` in iropt_me.fd
      `IME_DEFAULT_ON`; `FID_IROPT_OFF=unroll` turns it off), with peeling,
      a remainder and run-time trip counts; loops indexing a Vec are left to
      the counted-loop proof. Tests `test_midend_unroll_remainder.fd`,
      `test_midend_unroll_runtime.fd` (and their `_fail` kin), check
      tests/tooling/unroll-check.sh (SUITE=1, BENCH=1). Left: that check
      with this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — the check found this not done: `unroll` on by default
      miscompiles. `fid emit test_opt_dce.fd` (pe_ir) prints 9900 for the
      sum of 0..99 (4950 with `FID_IROPT_OFF=unroll`, with `fid run` and on
      `--nat`): a partially unrolled loop with a dead store in its body
      (`waste = i * i`) doubles the sum, and FID_VERIFY=1 is silent.
      unroll-check also fails 3 (with inline2 on, no peeled or run-time
      unrolling shows). Left: fix it or take `unroll` out of IME_DEFAULT_ON.
      Fixed Nov 21 (5fb5516): the miscompile the check hit (2g.80) was pe_ir's global register allocator, not the unroller — a value live into a block started at the block's first instruction and shared a register with a value defined in a block laid out later (the unrolled copies, chained backwards); live-in now starts at `bstart - 1` (test_unroll_dead_store; verify-clean-check.sh runs it under FID_VERIFY on both backends).
      Nov 22 code audit: the pe_ir miscompile fixed (2g.80: live-in starts at bstart-1, pe_ir.fd:3720-3729); regression test_unroll_dead_store.fd exists; unroll stayed in IME_DEFAULT_ON.
- [x] ME25. Loop versioning: a fast copy without checks when a cheap test at the
      entry proves them unnecessary (M).
      Not started: the copy needs a way to tell the backends that its checks
      are proved (checks.fd decides them in pe_ir and the C generator).
      Nov 16: written, opt-in (`FID_IROPT_ON=version`), not run on the suite
      yet. `iro_version` (iropt.fd): `while i < n { .. v.get(i) .. }` with n
      not `v.len()` gets a copy entered when `n > 0` and then
      `n <= v.len() && i >= 0` (a new block; the Vec is measured only when the
      loop runs). The backends learn the copy's checks are proved the way
      they learn any: checks.fd's counted-loop proof accepts a bound and a
      start that a guard on the loop's only way in establishes
      (`peir_entry_guard`), so pe_ir, nat_ir and C need nothing new. A loop
      is versioned only if the proof then covers an access of the copy
      (checked by running it). Test `tests/bootstrap/test_irswitch_version.fd`.
      Left: a suite run with it on, then the default; loops with an index
      other than `i + 1` steps.
      Nov 17: on by default (`version` in iropt_me.fd `IME_DEFAULT_ON`;
      `FID_IROPT_OFF=version` turns it off). Tests
      `test_version_oob_fail.fd` (n > v.len()) and
      `test_version_negstart_fail.fd` (a start of -1): the guard fails and
      the checked loop panics after the same lines. `fid explain` shows the
      versioned copy's checks (main.fd calls iropt.fd `iro_explain_loops`
      before `explain_checks`). Check tests/tooling/version-default-check.sh
      (NAT=1 SUITE=1, BENCH=1). Left: that check with this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — on by default; version-default-check's outputs are right on
      pe_ir and `--nat` (the oob/negstart tests panic as they should) and
      explain shows 1 check removed in sum_first's copy, but it fails 2:
      test_counted_loop_step and test_version_negstart_fail version no loop
      once the default `ssa` runs (3 and 2 with ssa off). Left: versioning
      after ssa, the full suite.
      Nov 21 (evening): found and fixed. ssa's back-conversion put a phi's
      load at the top of its own block, but a use of that VALUE in another
      block (the accumulator read after its loop: `print %58`, %58 the
      loop header's load) kept reading the header's last execution —
      breaking ssa's own "only that block reads the slot" contract, and
      making iro_version_one rightly refuse ("a value of the loop is read
      outside it"). ssa_materialize now gives every such use a fresh
      Load of the slot immediately before it (the slot model's exact
      position; FID_VERSION_WHY=1 in iropt.fd names the pass's refusals).
      version-default-check: 0 failed — 4/3/2/2 loops versioned by default,
      outputs and panics right on and off (build flakiness on the
      fidesc-built binary class aside, 2g.79). test_vectorize's interp
      output is byte-identical to its expected block again.
- [~] ME26. Scalar replacement of aggregates on SSA: struct fields as separate
      values (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=sra`), not run on the suite yet: a struct local
      whose fields are only loaded and stored becomes a local per field
      (`ime_sra`).
- [x] ME27. Small structs returned in registers (two words) for internal
      functions (M).
      Not started: a calling-convention change in the backends (pe_ir,
      nat_ir), not something the IR can say.
      Nov 16, nat_ir (Linux, macOS, `--nat`; not run on the suite yet): a
      machine-layer convention, the IR's single-value Return unchanged
      (`ng_rr_plan` in compiler/nat_ir.fd, before allocation). A function
      returning a struct of 8 or 16 bytes through lowering's hidden result
      pointer, called only by direct calls in the image (its label never
      taken as an address; not main, runtime, core Vec/HashMap, a
      string-named task entry, an extern or a dyn method), whose result
      address is only moved, offset by constants and stored through (each
      store inside one word; `ng_rr_analyze`) keeps the words in two vregs
      (narrow fields masked in) and returns them in the result register and
      argument register 1; each call site keeps the address it passed and
      stores the two registers there after the call. The hidden argument is
      still passed, so nothing else changes. Off under the CS6 cache (its
      replayed callers keep the old convention) and in @constant_time code;
      `FID_NAT_OFF=retregs`: never. Test: test_natabi_retregs.fd. pe_ir
      keeps the pointer (decided there, LF24).
      Nov 16, pe_ir half (decided, no code): pe_ir keeps the hidden result
      pointer. Its values live in frame slots, so the callee would store
      the words in its own frame, load them into rax:rdx, and the caller
      store them at the address it holds: more memory traffic than the
      stores through the pointer now. The convention pays where the words
      stay in registers (nat_ir, which replaces pe_ir: MI14). LF24 and
      competitive OP12 likewise.
- [~] ME28. Switch lowering: ranges, bit tests, jump tables chosen in the mid-end
      (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=switch`), not run on the suite yet: a chain of four or
      more `x == k` tests on one value becomes a balanced tree of comparisons
      (`ime_switch`). Jump tables and bit tests need a new IR instruction.
      Nov 16: the instruction is in: `Inst::Switch(value, keys, targets,
      default)` (`compiler/ir.fd`; terminator, `inst_succ_blocks`, the IR
      verifiers, serialization in ir_ser.fd). `IrFunction_form_switches`
      turns a chain of 4+ distinct `x == k` tests on one 64-bit value (the
      same value, or a reload of the same word with nothing that writes in
      between; links of one way in that compute nothing else) into one
      Switch; `IrFunction_expand_switches` turns it back. Decision: switches
      are formed after the mid-end, on the final IR of the backends that
      handle them: the C generator (a C `switch`, the C compiler picks the
      table) and nat_ir (MI27's tree and bit tests); pe_ir, interp, pir, wasm
      and x86 handle the instruction but never get one (pe_ir's liveness
      knows two successors). lower.fd's `LOWER_SWITCH` (forming right after
      lowering) stays off until iropt_me's CFG walks know Switch; checks.fd,
      iropt, irverify and nat_ir already do. Never in @constant_time code.
      `FID_SWITCH=off` turns it off. Left: native jump tables (MI27), a suite
      run, and `match` on narrow (u8) values (only i64/u64 values switch).
      Nov 18: a `match` on a narrow integer (i8/i16/i32, u8/u16/u32)
      switches too (`ir_sw_word`): every backend holds a narrow value in
      canonical form (zero- or sign-extended) and compares whole words, as
      the compare chain does (the C path's `switch ((int64_t)x)`, nat_ir's
      `ng_gen_switch`/`ng_gen_jtab`). Tests: test_irswitch_narrow.fd,
      tests/nattools/machine_switch_narrow_check.sh. Native jump tables,
      bit tests and trees are MI27. `LOWER_SWITCH` stays off by decision:
      switches are formed after the mid-end only. Left: mark done once the
      check script passes on the built compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — matches on narrow integers form switches (7 C switches;
      outputs right on 4 targets), and with inlining off digit, opcode and
      small_dense get jump tables and the others none. But
      machine_switch_narrow_check fails 4 with the default passes (inline2
      folds opcode and small_dense into main). Left: the test or the check,
      a suite run with switches.
- [x] ME29. Tail recursion into loops (S).
      `iro_tailcalls` (`compiler/iropt.fd`): a `return f(...)` self-call in
      tail position becomes argument stores + a jump back to the entry.
- [x] ME30. Every mid-end change measured on the suite: instruction count and
      compile time before and after, kept in a file per change (S).
      `scripts/me-measure.sh` exists; `FID_IROPT_STATS=1` reports
      instruction counts and per-pass timing. Files per change not kept yet.
      Checked Nov 15: the script already writes one file per change
      (`docs/perf/mid-end/<label>.txt`: instructions, iropt and build time
      on and off, same-output check), and since Nov 15 the stats time the
      module passes too (ME5); an opt-in pass is measured with
      `FID_IROPT_ON=<pass> OFF=<pass>`. Left: no file has been recorded yet
      (it needs a built `fid` and a suite run).

      Nov 22 code audit: docs/perf/mid-end/ now holds ~27 recorded measurement files (me2/me3/me7/me20/me21/me23-25/lf8/op*). The 'no file recorded' Left was stale.
## Track WP — The whole program (20)

- [x] WP1. Every build sees the whole program; `nat_ir` compiles only what `main`
      reaches, from a worklist.
- [x] WP2. A call graph with strongly connected components, built once per build
      (M).
      Nov 8: the call graph of the recorded functions.
- [x] WP3. Compile bottom-up: callees before callers, each SCC together (M).
      Functions are allocated and emitted callees first (post-order).
- [x] WP4. IPRA: each function's clobbered registers recorded after allocation;
      a caller keeps values in caller-saved registers across calls that do not
      touch them (M).
      Each function's clobber mask after allocation; callers keep values in
      caller-saved registers across calls that do not touch them.
- [x] WP5. IPRA through indirect calls: a closure or `dyn` call clobbers the union
      of its possible targets when they are known, everything otherwise (M).
      `ng_call_mask_i` (`compiler/nat_ir.fd`): the CALLR operand is traced back
      through in-block MOV chains to `MO_ADDR` labels (and, for cross-block
      vregs, the union over its def sites); each label's recorded callee mask
      is OR-ed in. Any unresolved def falls back to the full mask.
- [~] WP6. Custom conventions for internal functions: arguments in the registers
      the caller already has them in, more than 8 register arguments, results in
      several registers (L).
      `internal_conv` on `IrFunction`, marked by `iro_env_trim` for every
      function meeting the internal predicate (not address-taken, not a dyn
      target, not extern/callback/runtime). nat_ir consults it on both sides:
      `ng_load_args_i`/`ng_argreg_i`/`ng_nregargs_i` give internal calls the
      extended set — x64 gains r9 (4->5; r8/r10/r11 are the generator's
      temporaries), AArch64 gains x13-x15 (8->11), RISC-V gains t6 (8->9).
      pe_ir ignores the flag and keeps the platform convention everywhere —
      consistent because a call is emitted by one backend. Not done:
      multi-register results (IR `Return` is single-value) and the
      args-already-in-registers case the allocator's hints mostly cover.
      Nov 16: results in several registers done for struct results of up
      to two words (ME27, `ng_rr_plan`: the result register and argument
      register 1), without `internal_conv` (whose pass is opt-in): the
      machine layer finds the functions only called directly itself.
      Left: arguments in the registers the caller already has them in.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — arguments where the caller has them, through RA7's hints on
      by default: wp6_args_check fwd3 16 -> 8 moves, totals 193 -> 179
      (x86-64), 172 -> 161, 169 -> 159; but fwd5 gains nothing on AArch64
      (66) and gets one more move on RISC-V (68 vs 67), and the hints trip
      the allocation checker on x86-64 (test_machine_simd, test_ai_stdlib).
      Left: those, or the hints opt-in again; the native suite.
      Nov 21: 2g.88 fixed (see RA3).
- [~] WP7. `__env` dropped from functions that never read it, at every call (S).
      `iro_env_trim` (`compiler/iropt.fd`): removes `params[0]` and the
      matching `args[0]` at every direct `Inst::Call`. Address-taken
      (`ConstFnRef`), functions named in strings, `Type_method` dyn targets,
      externs, C callbacks, runtime prefixes, `pe_runtime_alias` targets and
      `main`/`main_entry` keep the uniform convention, so `CallPtr` closures
      still pass an environment. Same pass marks `internal_conv` (WP6).
      Status corrected Nov 15: opt-in (`FID_IROPT_ON=envtrim`), because a
      caller the pass misses still passes the env slot (test_brotli's
      `bs_fill` got its `s` as 0, Nov 9; the exclusions above came after,
      and the missed path is not found). Left: find that caller (a suite run
      with envtrim on), then make it the default.
      Nov 16, by reading pe_ir's and nat_ir's call emission: an `Inst::Print`
      of a HashMap calls `HashMap_to_str` by name with a null env in the
      platform's registers (pe_ir Print, nat_ir Print), and neither envtrim
      nor WP6's `iro_conv_mark` excluded it (only `Vec_*`): envtrim dropped
      its env, WP6 gave it the internal convention (on `--nat` Windows the
      internal registers are System V's: a miscompile with WP6 alone). Both
      now skip every function machine code calls by name (ir.fd
      `ir_dfe_root`: `__*`, `pet_*`/`ped_*`, the print formatters, ...),
      interrupt handlers, GPU kernels and the targets of `__c_fn_` callback
      references. Test `tests/bootstrap/test_irswitch_envtrim.fd`. Left:
      whether that was test_brotli's caller is not confirmed: a suite run
      with `FID_IROPT_ON=envtrim`, then the default.
      Nov 17: on by default (`iro_pass_on("envtrim")`; `FID_IROPT_OFF=envtrim`
      turns it off). Two more callers are excluded: core's `vec_buf_alloc`
      (nat_ir's vector loops call it by name with a null env) and every
      function the module's data names (ir.fd `ir_data_named_fns`: a string
      constant or a global's string initializer). Details and the on/off runs:
      docs/perf/mid-end/wp7-wp14-wp15-defaults.txt; check
      tests/tooling/wp-defaults-check.sh. Left: that check and the suite with
      this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — envtrim on by default trims 699 calls and outputs stay right
      without FID_VERIFY, but the nat_ir allocation checker rejects
      test_machine_simd (case_n: "defines v1478 in argument register 6,
      whose argument or result is still to be read", `--nat` and
      x86_64-linux), test_ai_stdlib and test_machine_brflags2; each builds
      with `FID_IROPT_OFF=envtrim` (or `FID_NAT_OFF=arghint`/`argfree`).
      Left: fix that (RA7/RA3) or make envtrim opt-in again; the suite.
      Nov 21 (5fb5516): the argument-register checker failures the check hit (2g.88) were the checker's own — a range may end at an op that writes the register by name or copies it into the range's value (ng_arg_fit_end, NgPhys.cpy; argreg_dead_def_check.sh).
- [x] WP8. Leaf functions without frames, with no frame record where traces do not
      need it (S).
      `g.leaf` in nat_ir: a function with no calls, no callee-saved spills
      and an empty frame skips the prologue/epilogue — `ng_epilogue` emits
      only the architecture return on x64/a64/rv. MIR path only (the
      default); a64/rv runtime behaviour unverified (no qemu).
      Nov 16: why it never fired — it required `al.frame == 0`, and
      ng_mir_alloc rounds every frame up to at least 16 bytes (every IR
      value has a slot in base_frame besides). Now `ng_leaf_ok` reads the
      allocated MIR: no call, no panic check or trap (a trace needs the
      frame record), no callee-saved register, no slot operation, no
      incoming or outgoing stack argument, no spilled vreg but a
      rematerialized constant or address. `FID_NAT_OFF=leaf`: frames
      everywhere. Test: test_natabi_leaf.fd (not run on the suite yet).
- [x] WP9. Effects as optimizer facts: a call to a function with no effects is a
      pure value in the e-graph (merged, hoisted, removed when unused) (M).
      `ru_pure_cse` (`compiler/rules.fd`): merged under dominance, never
      hoisted — calls may panic. (Nov 15, competitive OP25: iropt's LICM
      moves such a call out of a loop's header when nothing before it there
      can panic or be seen, `iro_licm_pure_call`.) Eligibility: local Alloca traffic and
      stores through unescaped Allocas are invisible, so callees with local
      writes still qualify. Call args key on constant *values* (each call
      emits its own ConstInt). Nov 15 (soundness fix): a function's effects
      are now its own joined with those of everything it calls, transitively
      (`iro_effect_masks`, `compiler/iropt.fd`, one fixpoint per
      `rules_run`). Before, eligibility added only the direct callees'
      `effects` field, which holds a function's own instructions (a call to
      a program function carries no effects at the call site): a function
      whose callee called a writer looked pure and two calls of it merged
      (test_midend_effects_deep). Escapes are also followed through address
      arithmetic and casts (`iro_esc_of`).
- [x] WP10. `!read`-only calls merged when no write lies between them (M).
      `ru_read_call`/`ru_read_reuse` (`compiler/rules.fd`): same-block and
      dominating-block merges with a backward CFG scan for writes. On by
      default since Nov 17 (OP25; `FID_NAT_OFF=readcse` turns it off; it was
      opt-in through `FID_RULES_ON=readcse`). Nov 15: a call between the two is a write
      unless the callee writes nothing, transitively (before, every call to
      a program function passed: its call-site effects are empty), a stack
      slot is not one, a store to an unescaped local is not one, and a
      write after the second call on a loop back to it stops the merge (it
      was missed on loops longer than one block); test_midend_effects_deep.
      Left: a suite run with it on and off (tests/tooling/op25-check.sh
      SUITE=1 NAT=1).
      Nov 17: on by default since OP25 (rules.fd `g_ru_readcse`;
      `FID_NAT_OFF=readcse` or `FID_IROPT_OFF=readcse` turns it off); see
      docs/perf/mid-end/wp7-wp14-wp15-defaults.txt.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — readcse on by default: op25-check 0 failed (readcse 14, 0
      with it off), midend-optin-check 33 SAME on `--nat`, a 60-test sample
      54 SAME, 0 differ. The full SUITE=1 NAT=1 run was replaced by the
      sample.
- [x] WP11. `@crashfree` functions: their panic paths are absent, so callers need
      no panic edges for them (S).
      Two uses. Optimizer: `iro_mask_annotated` (`compiler/iropt.fd`, was
      `ru_ext_effects`) strips `Diverge` so `@crashfree` read-only callees
      (e.g. stdlib `vec_code`) merge as read-only calls, and their callers
      inherit the stripped set.
      Codegen: `ng_panic_if` elides recorded panic checks — opt-in via
      `FID_NAT_ON=crashfree`, because the emit path does not verify the
      annotation and a wrong one silently miscompiles (panicking program
      ran clean before the gate). No caller-side panic edges exist in this
      backend (panic aborts; no unwinding).
      Nov 22 code audit: ng_crashfree_check (nat_ir.fd:18772) now VERIFIES @crashfree (a surviving check is a build error), called under default-on ng_on("unchecked") — the stated reason for opt-in is obsolete.
- [x] WP12. `@no-alloc` functions: no allocator state is live across them (S).
      `iro_mask_annotated` (was `ru_ext_effects`) strips `Alloc` under the
      same annotation-trust model as `@wrapping`, improving effect
      classification for merging. The
      literal item — keeping no allocator state live across the call — has
      no mechanism to attach to: the nat backend caches no allocator state
      in registers across calls.
      Nov 22 code audit: iro_mask_annotated strips IRO_EB_ALLOC for @no_alloc (iropt.fd:9105); the literal half (no allocator state across calls) is vacuously satisfied — nat caches no allocator state in registers.
- [x] WP13. Interprocedural constant propagation: a parameter always passed the
      same constant becomes that constant (M).
      `iro_ipcp` (`compiler/iropt.fd`): one pass collects per-candidate
      constant arguments across all `Inst::Call` sites; constants are emitted
      into each candidate's entry block and uses renamed. Excluded: `main`,
      address-taken (`ConstFnRef`), `dyn` targets (`Type_method` reachable via
      DynCall), externs, C callbacks, runtime prefixes. Opt-in (`FID_IROPT_ON`)
      per the earlier miscompile note.
- [~] WP14. Function specialization: a hot function cloned for a constant argument
      (M).
      `iro_specialize` + `iro_clone_function` (`compiler/iropt.fd`): params
      constant at some but not all call sites get a clone per observed
      constant (`name_sc<param>_<value>`, <=2 values/param, <=4 clones/fn,
      <=200 insts) with the constant emitted in the clone's entry and the
      matching sites redirected. Same exclusions as IPCP. Opt-in
      (`FID_IROPT_ON=spec`) given the IPCP miscompile history; verified
      `spec 44` clones with identical program output. Left: hotness (there
      is no profile: every candidate within the limits is cloned) and a
      suite run to make it the default. Identical clones are folded by WP19
      (`icf`).
      Nov 17: on by default (`iro_pass_on("spec")`; `FID_IROPT_OFF=spec`
      turns it off). The clones are now added to `reach`, so the
      per-function passes fold them (before, they were made after `reach`
      and skipped: spec cloned but folded nothing), and the pass is linear:
      each candidate's call sites are recorded once and only those are
      weighed and redirected. docs/perf/mid-end/wp7-wp14-wp15-defaults.txt;
      check tests/tooling/wp-defaults-check.sh. Left: hotness; that check and
      the suite with this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated for what is written — spec on by default folds its clones:
      wp-defaults-check (spec 2, 0 off; test_midend_spec_hot right on pe_ir
      and nat), scale_sc1_3 keeps its constant, spec on hello takes 10 ms
      (was 878); a 60-test sample 0 differ on pe_ir (the `--nat` failures
      are not spec's). Left: hotness (every candidate within the limits is
      cloned), the full suite.
      Nov 22 code audit: hotness is IN (since 49fb2b9) — iro_spec_site_
      weights weighs each recorded site by iro_call_weights (profile-aware:
      pgo_count base × loop depth ×8, capped), and iro_spec_hottest clones
      only values whose sites weigh >= IRO_SPEC_HOT (8), at most 2 per
      param (iropt.fd :8378–:8420, wired at :8645). That "Left: hotness"
      was stale. Left: the full suite only.
- [x] WP15. Dead function and dead global elimination after optimization, not only
      before (S).
      `iropt_native` calls `IrModule_dead_function_elimination` after the pass
      loop, opt-in (`FID_IROPT_ON=deadf`). Seeds fixed for correctness:
      `c_callbacks` and `Type_method` targets of `DynCall` (found via
      `MakeDyn`) are kept — previously dropped, which miscompiled
      trait-object programs. Status corrected Nov 15: its roots (ir.fd) are
      still only `main` and `p_*`, not the runtime functions machine code
      calls (`pe_win_*`, `pe_rt_init`, task bodies named at run time), so it
      stays opt-in; functions the passes made dead are not compiled anyway,
      because nat_ir compiles only what `main` reaches, computed after the
      mid-end (WP1). Left: those roots in `IrModule_dead_function_elimination`
      (ir.fd), and dead globals.
      Nov 16: the roots are in (`ir_dfe_root`: main, `p_*`, `__*`,
      `pe_`/`pert_`/`pet_`/`ped_`, `nat_*`, the `Vec_*`/`vec_raw_*`
      stand-ins, the print formatters, the bare image's syscall entries,
      interrupt handlers, GPU kernels), and dead globals go with it
      (`IrModule_dead_global_elimination`: a global with a literal
      initializer whose address is only ever stored through loses those
      stores and its entry). Decision: still opt-in (`FID_IROPT_ON=deadf`):
      the native backends compile only what main reaches, so on by default
      it would only save compile time; left: a suite run with it on, then
      the default.
      Nov 17: on by default (`iropt_native`: `iro_pass_on("deadf")`;
      `FID_IROPT_OFF=deadf` turns it off). Its roots also take ir.fd
      `ir_data_named_fns` (functions a string constant or a global's string
      initializer names), and a global an inline-assembly template names
      stays. The reachability it shares with `reach`, pe_ir and nat_ir
      (`pe_ir_reachable_from`) is linear now (a table for the seen set, one
      scan per dyn method, a merge sort): it cost 1.4 s of hello at -O0.
      docs/perf/mid-end/wp7-wp14-wp15-defaults.txt; check
      tests/tooling/wp-defaults-check.sh. Left: that check and the suite with
      this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — deadf is on by default and linear (87 ms, was 1860), dead
      globals go (3) and outputs are right; but wp-defaults-check fails
      "deadf functions: 0": the PE88 prune runs first and drops them (154
      with FID_PE_PRUNE=0). Left: the check (or deadf for functions), the
      full suite.
      Nov 21 (evening): the check reads its deadf counters with
      FID_PE_PRUNE=0 (tests/tooling/wp-defaults-check.sh): the emit path
      lowers only what reach.fd finds reachable, so what the passes make
      dead is gone before iropt counts it (155 with the prune off, 0
      without); the outputs stay checked with the prune at its default.
- [x] WP16. Devirtualization: `dyn` calls whose receiver type is known become
      direct calls; closures called at one site are inlined (M).
      `iro_devirt` (`compiler/iropt.fd`): a `DynCall` whose receiver's
      definition is `MakeDyn` becomes a direct `Call` to `Type_method` with
      the callee's real effects. Closures (re-checked Nov 15): a `CallPtr`
      on a statically known closure environment becomes a direct `Call`
      (`iro_closure_devirt`, LG5/LG6, on by default; its environment moves
      to the stack when it stays in the frame), and a higher-order function
      is cloned per known closure (`iro_hof_specialize`, `hofspec`). The
      direct call is then inlined like any other: by default when the
      closure is one block (`iro_inline`); a closure called at one site up
      to 400 instructions (`ime_inline`'s once-called rule) only with
      `FID_IROPT_ON=inline2`. Left: inline2 on by default (ME20).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — inline2 (default since OP5) takes once-called closures and
      `inlineclo` is in IME_DEFAULT_ON for when it is off: inlineclo-check 7
      PASS on pe_ir and `--nat` (9 and 51 sites), test_midend_inline_closure
      right, a 60-test sample 58/54 same, 0 differ.
- [x] WP17. Global layout: hot functions together, cold panic paths at the end of
      the image (M).
      `ng_cold_last` (nat_ir): within each function, MIR segments ending in
      `MO_TRAP` — the panic paths — are moved to the end so the hot path
      stays compact. Read Nov 15: nat_ir's emission order also puts
      functions that are only panic paths (trap dispatchers with no real
      body) at the end of the image (the `WP17` comment by the call-graph
      order). Left: hot functions together (no profile to say which).
      Nov 16: with a profile (`fid run --profile p`, then `fid emit
      --profile-use p`: compiler/pgo.fd `pgo_count`), nat_ir.fd
      `ng_mir_finish` takes the roots of its post-order most called first
      (nat_enc.fd `nat_hot_first`), so the hot functions and what they
      call are laid out together at the start; it stays a post-order, so
      IPRA still allocates callees first. Without a profile the order is
      unchanged. Test program test_nattools_hot.fd, scripts/nat-tools-check.sh
      (`nm -n`, not run yet). Left: a run of the check; pe_ir lays out in
      module order (pgo_apply's).
- [x] WP18. Globals that are never written after initialization become constants
      (S).
      `iro_const_globals` (`compiler/iropt.fd`): a global with an `IntLit`
      initializer whose address is only ever loaded — no stores through it,
      no escaping uses — has its integer loads folded to the initializer.
      Verified: ~295 loads folded per build on the suite, outputs unchanged.
- [x] WP19. Generics fully monomorphized with sharing of identical bodies (code
      folding by hash) (M).
      Identical functions (same MIR and allocation) are emitted once
      (nat_ir). Nov 15: before either backend, `iro_fold_identical`
      (`compiler/iropt.fd`, opt-in `FID_IROPT_ON=icf`, after the
      per-function passes) sends every direct call to a function whose
      serialized IR (`ir_ser.fd` `iw_irfunc`, name left out: blocks,
      instructions, types, metadata, effects, annotation flags) equals an
      earlier function's to that earlier one; candidates are bucketed by a
      cheap signature and a hash first. Runtime and backend-recognised
      names (`p_*`, `Vec_*`, `print`, `len`, `*_len`, ..), entry points, C
      callbacks and interrupt handlers are never folded. Helps pe_ir too
      (test_midend_icf). Left: a suite run with icf on, then the default;
      sharing at monomorphization (`mono.fd`), so instances whose types lower
      alike are not built twice.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — sharing at monomorphization works: test_mono_share_alias
      right on 3 backends, mono_share/check.sh ok (11 instance keys, 8
      built, 3 shared). Left: icf is not on by default and folds nothing in
      its own test (test_midend_icf prints no folded line even with
      `FID_IROPT_ON=icf`); only an 82-test sample was run.
      Nov 21 (evening): the test's small identical functions were inlined
      away by the default inline2 before icf ran — its bodies are now over
      inline2's callee budget (24 checked mul/add lines, ~100 instructions)
      and fold with the defaults on (1 folded; interp and the folded native
      agree). icf is on by default (iropt.fd gates it on
      `iro_pass_on("icf")` alone; FID_IROPT_OFF=icf turns it off). Left: a
      suite run with it on.
      Nov 21 (5fb5516, 2g.109): FID_VERIFY=1 is clean on the two rejected programs — a select's recv_alloca/case slots live in the entry block (lower.fd), and ime_remap renames PersistLoad/PersistSave/Backward's operands (iropt_me.fd; test_select_case_slots, verify-clean-check.sh).
- [x] WP20. Whole-program mode stays the default; a library build records the
      facts (clobbers, effects, sizes) its users need (M).
      Found Nov 15: `FID_LIBFACTS=<path>` already wrote `fn <name> params=N
      effects=..` lines (`iro_libfacts`) and nat_ir appends `ext <name>
      mask=N size=M` lines (clobbers, code size); `FID_LIBFACTS_IN` was read
      by rules.fd (effects) and nat_ir (`ng_libfacts_load`, masks for IPRA).
      The effects never agreed: the writer wrote `!read,diverge`
      (`EffectSet_to_str`) and the reader wanted `read`, and the writer wrote
      a function's own effects, not what its callees do. Now the writer
      writes the transitive set (`iro_effect_masks`) and one reader,
      `iro_lib_mask` (`compiler/iropt.fd`), parses that form; rules.fd's
      pure and read-only call merging and its write barriers use it for
      functions the module does not define. Left: a library build mode
      (nat_ir refuses a program without `main`, and compiles only what
      `main` reaches, so a library's own functions get no `ext` lines) and a
      test (it needs the two variables set across two builds).
      Nov 16, library mode: a program without `main` is a library build on
      the machine layer — main.fd lowers all of it (`lw_keep_set`), nat_ir.fd
      `ng_lib_roots` makes every function of the input file a root (the
      top-level ones main.fd lists, and the methods ir.fd `ir_fn_file`
      records for that file), so FID_LIBFACTS gets an `ext` line for each;
      the entry's call of main goes to a trap (the image is for its facts or
      its object, TG12). tests/nattools/lib_nomain.fd, scripts/nat-tools-check.sh
      (not run yet). Left: the run, and a check that a program built with
      FID_LIBFACTS_IN of that file keeps values across the calls.

## Track LF — The language's guarantees as optimizer facts (25)

- [x] LF1. Alias facts from types: a `&mut` parameter aliases nothing else live;
      two `&mut` never alias (M).
      Nov 9 (`compiler/iropt_me.fd`, `ime_may_alias`): two different
      parameters never overlap (a function writes only through a `&mut`, and
      the borrow rules let nothing else reach that place); `*u8` parameters
      (strings, unsafe code) are left out, and parameters against globals may
      overlap. Nov 15 (verified): the lowering marks reference parameters
      (`borrow_kind` Mut / Shared in `Lowerer_lower_function`, lower.fd) and
      the query reads it (`ime_borrow`: a `&` parameter's place is never a
      `&mut` one's, and no call writes it); a parameter's place is never one
      of the function's locals (LF2). Unmarked typed-pointer parameters
      (by-value structs, unsafe pointers) still count as written. The passes
      that use the query are opt-in (LF4–LF6, LF23).
- [x] LF2. References are second-class (`escape.fd` proves it): the optimizer may
      assume a reference's target outlives every use and is never stored (M).
      Nov 9: the alias query relies on it (a parameter's place is reached
      only through that parameter and what is derived from it).
      Nov 15: what a parameter refers to existed before the call, so it is
      never one of the function's own locals, escaped or not, and a global
      is no local either (`ime_may_alias`). Left: a local passed to a call
      by reference still counts as escaped for every later call (the
      reference cannot be stored, but a raw pointer from unsafe code can).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `lendref` is on by default, so a local only lent to calls
      that lend it onward keeps its facts: lendref-check 7 of 7 on pe_ir
      and `--nat` (1012 -> 1009 instructions; the escape test keeps its
      loads), a 100-test sample with it on and off 97 SAME, 0 differ.
- [x] LF3. Owned values have one owner: a load through an owned `Vec`'s data
      pointer cannot be changed by a store through any other name (M).
      Nov 9: for locals: a local whose address goes only to loads, stores and
      field addresses overlaps nothing else, and no call can write it
      (`ime_unescaped`). A Vec's buffer is reached through calls to core, so
      its elements are not yet in the query.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `vecalias` (elements of an owned Vec<i64>'s buffer in the
      alias query, iropt_me.fd ime_vb_compute) is on by default:
      vecalias-check 6 of 6 on pe_ir and `--nat` (4822 -> 4821
      instructions), a 100-test sample 92 SAME, the other 5 out of memory
      under load and SAME on retry. It acts on vecdata's loads, which stay
      opt-in (OP9).
- [x] LF4. Alias analysis as a query interface, answered from LF1–LF3, used by
      DSE, LICM of loads and store forwarding (M).
      Nov 9: `ime_may_alias(p, q)` and `ime_writes(inst, p)` (where an address
      comes from: a local, a parameter, a global, with its field path); used
      by LF5, LF6 and LF23. Nov 15: DSE uses it too: `iro_dse` (iropt.fd)
      calls `ime_overwritten_stores`, which drops a store to a local's or a
      `&mut` parameter's place that a later store in its block overwrites
      whole before anything may read it (`ime_reads`, `ime_same_place`;
      @zeroize, drop and @persist locals untouched, LF19);
      test_midend_dse_overwrite. Fixed in the query: field numbers after a
      view as another type (a `BitCast`, as an enum payload seen as its
      struct) overlap anything, where they looked disjoint before. Every user
      of the query is opt-in (`FID_IROPT_ON=dse,loadhoist,forward,matchfacts`)
      until the suite runs with them on.
- [~] LF5. Loads hoisted out of loops when the loop writes nothing that aliases
      them (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=loadhoist`), not run on the suite yet: a load of a
      local's or a parameter's field whose address is computed before the loop
      moves to the preheader when nothing in the loop may write it
      (`ime_hoist_loads`).
- [~] LF6. Store-to-load forwarding and redundant load removal (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=forward`), not run on the suite yet: in a block, a
      load of a place just stored or loaded, with no write that may overlap
      it between, becomes that value (`ime_forward`). On by default for
      whole locals on the native Linux and `--nat` paths: `eg_forward`
      (egraph.fd, FID_NAT_OFF=forward), a load of a local only loaded and
      stored becomes its dominating single store or the last store before
      it in its block.
- [x] LF7. Bounds and overflow checks dropped on counted loops (release D12,
      `checks.fd`) (done for `while i < v.len()` and `for i in 0..n`).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — checks.fd takes the other shapes too (bounds on the left,
      `v.len() - d`, indexes `i + e`, counting down; peir_len_minus,
      peir_loop_index, peir_counted_down): param-range-check 0 failed;
      sum_len_gt, window3, step_down2, for_pairs and the counted-down cases
      lose their bounds checks, keep_odd and the _fail variants keep theirs.
- [~] LF8. Integer range analysis drops checks whose operands cannot overflow
      (`checks.fd`, `PEIR_RANGES`); extend it to every loop and branch (L).
      Nov 9: facts carried through jumps into single-entry blocks (`n - 1`
      after `if n < 2 { return }` is unchecked), and a Vec only filled with
      `Vec::filled(n, x)` and never shortened bounds its indexes (sieve).
      Nov 15: a branch that says nothing about a range keeps the fact before
      it, and facts meet at joins: when every way into a block carries a
      fact about the same local or value, the block gets the interval that
      holds them all (`peir_edge_fact`, `peir_join_fact`; FID_IROPT_OFF=
      rangejoin turns it off; test_midend_range_join,
      test_midend_range_join_fail). Left: one fact per block, and none over
      a loop's back edge (a loop header gets only what counted loops give).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the range set is in (checks.fd peir_join_set, peir_sf_apply):
      range-loop-check passes, a sample gives 97 SAME with rangeset on and
      off. range-set-check fails 2 (hull's mul check stays in `fid explain`,
      though the in-process harness removes it) and range-dom-check fails 5
      (sieve keeps 4 overflow checks, filled_scale none, rangedom off
      changes nothing for inner_sq/sq_big). Left: those, SUITE=1.
      Nov 22 root-cause audit of the 4 sieve checks: (1) parameter ranges
      (LF10) are the missing bound — with n ∈ [1000,10000] from the two call
      sites peir_square_local fires (it needs n's high bound finite:
      `nh < PEIR_I64_MAX`), giving p ∈ [−100,100] in the body, which covers
      both muls (corner products fit), p+1 and m+p; that is exactly LF10's
      pending default flip. (2) The condition mul `p * p < n` is computed in
      the header block BEFORE the branch, so a forward edge fact can never
      apply to it — peir_lf_update's picker only picks locals the body
      stores, never seeds from the loop's own condition; deriving p's bound
      from the condition via peir_square_local at header entry removes it.
      (3) `m < n` is a relational fact over the back edge — the large one
      (a relation domain; today only expressible as an interval when n is
      bounded). The inner_sq/sq_big "rangedom off changes nothing" lines are
      range_set_main.fd's own in-process harness cases — the dominator-fact
      wiring itself may have regressed in the self-hosted build; check that
      first.
- [x] LF9. Ranges from conditions: inside `if i < n`, `i < n` is a fact (M).
      Holds in blocks with that branch as their only way in, and through
      jumps from them (`peir_range_refine`); since Nov 15 also through
      branches that say nothing about it and after a join all of whose ways
      in keep it (`peir_join_fact`). One fact per block (LF8).
- [~] LF10. Ranges through function boundaries for internal functions (WP13) (M).
      Nov 16: built and tested (test_midend_range_args.fd and
      test_midend_param_range.fd give their expected output through `fid run` and
      `fid emit` with FID_IROPT_ON=paramrange and without): for a function only direct calls
      reach (`peir_prm_internal`, iro_ipcp's exclusions: not main, no
      ConstFnRef or string naming it, no dyn target, extern, C callback or
      runtime prefix), each i64 parameter starts at the join of the
      arguments' ranges over every call (`peir_note_param_ranges`,
      `peir_prm_seed`, hooked into `peir_range_ops`; iropt.fd runs it after
      ipcp/spec, `explain_checks` too), and a call to one returning i64
      gets the join of the ranges of its returns (`peir_ret_join`,
      `peir_ret_seed` in `peir_range_scan`). Two rounds, the second using
      only what the first proved, so recursion is sound without a fixpoint
      (a self-call with `n - 1` gives no range). Opt-in: `FID_IROPT_ON=
      paramrange` (FID_IROPT_OFF=paramrange turns it off,
      FID_IROPT_OFF=retrange keeps the parameter ranges only). Tests:
      test_midend_range_args, test_midend_param_range(_fail),
      tests/tooling/param-range-check.sh. Left: a build, a suite run with
      it on, then the default; relational facts (`i < v.len()` of a
      parameter Vec) are not carried.
- [~] LF11. Check hoisting: one check before a loop instead of one per iteration,
      when the loop is versioned (ME25) (M).
      Nov 16: written with ME25, opt-in (`FID_IROPT_ON=version`): the copy's
      bounds checks become one `n <= v.len() && i >= 0` test before the loop
      (checks.fd `peir_entry_guard`, iropt.fd `iro_version`). The same
      change makes the counted-loop proof check that the Vec's slot is not
      re-stored between its `len` and the loop (it only looked for calls
      that shorten). Left: a suite run, then the default.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — on by default with versioning: where the Vec is a value the
      checks are hoisted (explain sum_first: 1 removed in the copy, 1 kept
      in the original; a 100-test sample 97 SAME), but version-default-check
      fails 2: loops over a `& Vec` / `&mut Vec` parameter
      (test_counted_loop_step, test_version_negstart_fail, double_first)
      are not versioned. Left: those loops, the full suite.
      Nov 21 (evening): versioned — the ssa back-conversion fix (ME25's
      entry: a phi's load value read in another block now loads the slot
      again before the use) restored them; version-default-check reports
      3 and 2 loops for those tests again, 0 failed.
- [x] LF12. Narrow types as ranges: a `u8` value needs no overflow check on a
      widening add (S). `peir_type_range` on narrow loads and `IntExtend`.
- [~] LF13. Escape analysis: a `Vec`, `str` or struct that never leaves its function
      is allocated on the stack (L). Nov 9: a local Vec whose value only
      reaches core's Vec functions keeps its 40-byte header on the stack
      (`lw_rc_stack_vecs`, `LOWER_STACK_VECS`); not yet strings, structs or the
      buffer itself, or a Vec passed to the program's own functions.
      Nov 17: Vec::filled(n const, plain x) of <= 512 bytes also keeps its
      buffer on the stack when only read or changed in place
      (lw_rc_vbuf_fits, Vec_init_filled_at, Vec_drop_stackbuf); a local
      `a + b` string that is only read is built in a 128-byte stack area
      (lw_rc_stack_strs_fn, Str_concat_into) with a heap fallback; Vecs
      passed to the program's own functions keep stack headers (OP27). Docs:
      docs/reference/stack-locals.md. Left: other string producers,
      Vec::new()+push buffers, temporaries.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — small Vec::filled buffers and Str_concat_into on the stack
      work: test_stack_str, test_stack_vbuf, test_stack_lower and
      tests/leaks/stack_vbuf_str are right on run, ccompile and emit; leak
      gate 10/10. Left: other string producers, Vec::new()+push buffers,
      temporaries and structs on the stack.
      Nov 21 (evening): `int_to_str` of a local joins them — core
      Int_to_str_into (core.fd) renders into the caller's stack area as
      [P_STR_STATIC][len] digits NUL (the value kept <= 0 so INT64_MIN
      needs no negation; the heap p_to_str when the area cannot be used),
      and lw_rc_stack_strs_fn rewrites `h = p_to_str(x)` to
      `Int_to_str_into(env, area, 128, x)` under the same readers-only
      rule (test_stack_str_int.fd: INT64_MIN, INT64_MAX, concat, lengths
      on run; `--leaks` needs a self-hosted-lineage binary). Left: the
      data-dependent and unbounded producers, Vec::new()+push buffers.
      Nov 22: `bool_to_str` joins it — core Bool_to_str_into renders
      "true"/"false" into the area ([P_STR_STATIC][len], cap >= 24, the
      heap p_bool_to_str when the area cannot be used), and
      lw_rc_stack_strs_fn rewrites `h = p_bool_to_str(b)` (the lowerer's
      f-string bool conversion; p_to_str_bool accepted too). BUILT AND
      VALIDATED (a -O0 fid from the self-hosted stage 0): the f-string
      temps too — a created value used only through its BitCast by
      reader calls, a print or its drop now rewrites into its own area
      (the casts the f-string lowering makes were what disqualified
      them), LOWER_STACK_STR_MAX 4 -> 8 (an f-string with one embedded
      value uses up to 3 areas). test_stack_str_bool.fd: all three
      conversions in areas, no p_bool_to_str call left in main, --leaks
      clean; test_stack_str/_vbuf/_int byte-identical. That closes the
      "temporaries" Left below for strings. p_char_to_str is not a
      producer any more: RT3's static one-byte table never allocates.
      Also: test_stack_str_int.fd's header expected 3 where the program
      prints 5 (str_len("-451|")), fixed; fidesc's interpreter panics on
      it (BitAnd on a Ptr in core's p_str_hdr_len) — pre-existing, the
      test had never run under fidesc (suite last run Nov 2). Left: the
      data-dependent and unbounded producers, Vec::new()+push buffers,
      the suite legs.
      Nov 22 producer audit (the full producer list is lower.fd:22396; the
      pattern match at :24117 covers exactly p_str_concat and p_to_str):
      p_char_to_str is NOT a producer any more — RT3 (pe_runtime/linux_
      runtime/codegen_c __p_chr1) hands out one of 256 static one-byte
      strings, so it never allocates; only the call and a no-op drop
      remain. p_bool_to_str / p_to_str_bool ("true"/"false") DOES allocate
      on every backend (C: p_malloc(6); native: pe_dup): Bool_to_str_into
      (the Int_to_str_into shape) written Nov 22 with the pattern-match
      entries; stdlib bool_to_str itself returns literals, the allocating
      path is the lowerer's interpolation conversion. Data-dependent
      (medium) — p_float_to_str / p_to_str_float: up to ~330 chars, need
      Float_to_str_into(env, area, cap, x) with a cap policy and the
      Str_concat_into-style heap fallback. Unbounded (hard) — p_to_str_vec,
      p_to_str_map, p_str_join_n, p_sb_to_str, the json/tensor/simd dumpers:
      output length is a runtime function of a whole data structure; a
      512-byte frame area defeats the budget rationale, so a two-phase
      render-length-then-render core API is the way. Vec::new()+push stack
      buffers need a static push-count bound (counted-loop facts LF7/LF8
      feeding the escape pass) or a core Vec_init_stackbuf that promotes to
      heap on push overflow — that entry point does not exist today.
- [~] LF14. Scalar replacement of a non-escaping `Vec` of known small length (M).
      Nov 16: built and tested (test_midend_vec_sroa.fd, `fid run` and
      `fid emit` with FID_IROPT_ON=vecsroa and without): iropt.fd `iro_vec_sroa` (run by
      `iro_vec_locals` before inlining; opt-in, FID_IROPT_ON=vecsroa). A Vec
      of plain words (Vec_new_typed / LF13's Vec_init_typed / Vec_new_sized,
      element code 0, type id 0) made in a function, pushed (at most 8 i64
      values) only in its own block and there before any read, read, set and
      measured only in that block at constant indexes below the push count,
      and otherwise only dropped, becomes stack slots: pushes and sets are
      stores, gets loads, `len` a constant; the Vec, its store into its
      variable and its drops go. Test: tests/bootstrap/test_midend_vec_sroa.fd.
      Left: a build, a suite run on and off, then reads in other blocks
      (dominated by the pushes).
      Nov 22: reads in dominated blocks — the rule is in but does not
      fire yet. A get, set or len on a load of the Vec's slot in a block
      hb dominates qualifies when no block that clears the slot (the
      ownership lowering's `v = null` after the drop — it always exists,
      which is why a first `slot_stores == 1` rule could never fire) can
      reach the read on any path (DFS over cfg.succs; a same-block
      clearing store only before the read, in scan order; a cycle back
      into the read's block disqualifies; iro_vs_reaches/iro_vs_zero_
      reaches, zero-block budget 16). Built and behavior-verified
      (test_midend_vec_sroa_dom.fd right with the pass on and off;
      original test still converts 4), but the count stays 1 on the new
      test (only the same-block sibling): some other check rejects every
      cross-block read and the shape-variants probing (join-read,
      tail-read) all convert 0 — one instrumented run (trace the
      rejection point in iro_vec_sroa_one) finishes it. Left: that, the
      build/suite legs.
- [x] LF15. Reference counts removed where ownership is known (proposal 0004): no
      `Vec_share` / drop pairs on values that are moved (M). Moves on last use
      (Nov 7), and in one block: Str and Box count/drop pairs, a Vec or map
      shared only to be read (`lw_rc_cancel_pairs`, `LOWER_CANCEL_SHARES`).
      Across blocks (LOWER_CANCEL_ACROSS) and across joins and branches for
      copies of parameters and locals that are only read
      (lw_rc_cancel_param_slots; tests/rc_cancel/check.sh,
      docs/reference/rc-cancel.md).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — Vec and map copies are cancelled across if/else
      (test_rc_cancel_across, _diamond, _local right on 3 backends, no
      leaks), but no `str` copy is: tests/rc_cancel/check.sh fails
      (pick_len, both_arms, loop_copy, kept, local_arms keep their
      Str_dup/Str_drop; 3 counts removed, 4 wanted). Left: str parameter and
      local copies (the dup now sits behind a region-stack branch).
      Nov 21 (evening): they are cancelled. Two matchers were at fault,
      not the region stack: lw_ps_lit_store_ok's backward walk stopped at
      the drop's own operand load and at the null of the clearing store
      (`load t; drop(t); t = null` — ConstNull read as may-free; the skip
      is local to that walk, lw_rc_may_free untouched), and the wrapped
      shape (a BitCast of Str_dup's result stored) is matched beside the
      plain in-place count. tests/rc_cancel/check.sh: pick_len 2->0,
      both_arms 2->0, loop_copy 2->0, kept 4->2, local_arms 2->0 counts
      (drops fewer in step); the programs print their expected lines
      through `run` (the check's `--leaks` leg segfaults on the
      fidesc-built binary class, hello included — pre-existing, 2g.79).
      Nov 22 code audit: lw_rc_cancel_pairs runs unconditionally in lowering (lower.fd:4200); the str-copy cancellation is at target (tests/rc_cancel/check.sh). The --leaks residue is the 2g.79 binary-class issue, not this feature.
- [~] LF16. Copy-on-write checks removed where a value has one owner provably (M).
      Low gain now: the C and native fast paths test the capacity's sign once.
      Nov 16: built and tested (test_midend_sole_owner.fd, `fid emit` with
      FID_IROPT_ON=soleown and without), for the native paths: iropt.fd
      `iro_sole_owner` (run by `iro_vec_locals` before inlining; opt-in,
      FID_IROPT_ON=soleown). A Vec made in the function whose handle is kept
      in at most one slot (holding only it or a zero) and otherwise only
      given as argument 1 to core's Vec functions that never share a buffer
      (the list of the C generator's `cg_sole_safe_call`) has one owner: the
      `vec_own` calls the lowering puts before a write into one of its
      elements (`Lowerer_own_path`, `ps[i].x = ...`) are removed. Core's own
      vec_own inside Vec_push/Vec_set stays (a call, not inlined). Test:
      tests/bootstrap/test_midend_sole_owner.fd. Left: a build and a suite
      run on and off.
- [x] LF17. Drops of values known to be empty or static removed (S). String
      literals, and string variables only ever assigned literals
      (`LOWER_LITERAL_SLOTS`); an empty Vec still frees its header.
      Nov 22 code audit: LOWER_LITERAL_SLOTS = true (lower.fd:24898, used at :25617), always on. No outstanding work named; the empty-Vec-header limitation is documented as accepted scope.
- [x] LF18. `@deterministic` and `@constant_time` respected by every pass: no rule
      may add a branch on secret data (the verifier of release session 30 runs on
      every target) (M).
      Nov 15: the mid-end keeps @constant_time code where a verifier sees
      it: a @constant_time function is not inlined into code without the
      annotation (`iro_body`, `ime_inline_function`; code inlined into a
      @constant_time function is checked with it), and switch lowering, the
      one mid-end pass that adds branches, skips @constant_time functions
      (`ime_function_passes`); test_midend_ct_inline. Verified: rules.fd and
      the e-graph rewrite integer operations into cheaper equal ones only (no
      branch, no division added) and touch no float operation, so
      @deterministic results do not change. The verifier runs on pe_ir
      (`peir_ct_check`) and nat_ir (`ng_ct_check`: the Linux targets and
      `--nat`). Left: no verifier on the C path (`ccompile`).
- [x] LF19. `@zeroize` stores kept by DSE (a pass may not remove them) (S).
      iropt's `dse` never touches `zeroize_locals` (`iro_backend_reads`;
      its overwritten-store part since Nov 15 through `ime_backend_reads`).
- [~] LF20. `@secret` values never spilled to memory unencrypted where a register
      will do, and their spill slots cleared (M).
      Nov 16, pe_ir half (compiler/pe_ir.fd `PeirGen_wipe_frame`, not run
      yet): a function with an `@zeroize` local or `@constant_time` zeroes
      its whole frame [rsp, rbp) — every slot, spill and temporary — and
      rcx, rdx, r8-r11, xmm0-xmm5 before each return, the result kept in
      rax; such a function makes no tail call. `FID_PE_OFF=wipe`: off.
      Tests: test_windows_wipe_secret.fd, scripts/pe-win-check.sh. `@secret`
      alone does not reach the IR (only `@zeroize` and `@constant_time`
      do); pe_ir already keeps every value it can in registers.
      Nov 16, nat_ir half and the IR (not run yet): `IrFunction.has_secret`
      (ir.fd, set by lower.fd for a @secret / @sensitive function,
      parameter or local, a @zeroize local or @constant_time; kept by
      ir_ser, IR file version 2; such a function is not inlined into one
      without it, iropt.fd / iropt_me.fd). nat_ir's NatFnRec.secret: every
      epilogue (tail jumps too) zeroes [sp, sp + save_off), every value
      slot, spill, local and outgoing argument, before the saved registers
      come back (`ng_wipe_frame`: 16 words unrolled, else a loop over the
      address scratch registers); the folding key and the CS6 hash include
      it. `FID_NAT_OFF=wipe`: off. Test: test_natabi_wipe_secret.fd. Left:
      "never spilled where a register will do" — the IR marks the function,
      not its secret values, so the allocator cannot prefer them; scratch
      registers are not cleared on nat_ir (pe_ir clears them).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the register wipe works: ct_pick zeroes 24/38/33/24 registers
      on x86_64, aarch64, riscv64 and `--nat`, none with the switch off,
      outputs right under FID_VERIFY=1. machine_lf20_check fails 7: under
      pressure 3 of 8 secret values keep a register (8 wanted), and the
      wipe does not grow the image on 3 targets. Left: that, @secret
      parameters and locals (a lowering change), the cross-test suites.
      Nov 21 (evening): the value-level marks already exist
      (IrFunction.secret_values — the @secret/@sensitive parameters and
      locals — plus a 16-round propagation through derived values,
      nat_ir.fd ng_secret_ids) and ng_secret_weights weighs them 2^40
      above every spill choice; machine_lf20_check's 3-of-8 was measured
      on the memory-starved fidesc-built class — re-run it on a
      self-hosted-lineage binary before touching the allocator again.
- [~] LF21. Regions freed in one step instead of object by object (release C6) (M).
      Checked Nov 16: blocked on a design decision (competitive OP29): an
      arena that takes every allocation while a region runs would free
      core's buffers of outer values grown inside it, values it hands out
      and other threads' allocations; the C runtime's `p_arena_save`/
      `_restore` is that kind of arena. Needs a per-thread region arena and
      an escape rule for region values first. Nov 16: proposal 0005
      accepted and written for the C backend (FID_REGION_ARENA=1; see
      competitive OP29). Left: the machine layer's runtime
      (stdlib/linux_runtime, `--nat`), where p_region_word is 0 today.
- [x] LF22. `comptime` results as constants the mid-end folds further (S).
      Nov 9: what `comptime` computes reaches the IR as constants already
      (the front end's comptime pass); fold, SCCP and branch facts fold
      further from them. No pass of its own is needed. Nov 15: the test is
      `tests/bootstrap/test_comptime_constfold.fd` — a recursive comptime
      function folds to its constant under `fid run`. (Found: `var`/`while`
      inside `comptime fn` silently evaluates to 0 — only
      expressions/recursion are evaluated; noted for a later fix.)
- [~] LF23. Pattern matching facts: after a `match` arm, the scrutinee's variant is
      known, so later tag checks fold (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=matchfacts`), not run on the suite yet: where a branch on
      `load(tag) == k` enters its arm, loads of that tag in the blocks the arm
      dominates become k when nothing there may write it (`ime_match_facts`).
      Nov 15: also required now, the tested load is in the branching block
      and nothing after it there may write the tag (a store between the load
      and the branch made the fact wrong); test_midend_match_facts. Left:
      opt-in until the suite runs with it on.
- [x] LF24. `Option` and `Result` in registers: tag and payload as two values, no
      memory for small payloads (M).
      Nov 16, pe_ir half: decided to keep the hidden result pointer there
      (see ME27: in a backend whose values live in slots two registers add
      memory traffic).
      Nov 16, nat_ir (not run yet): an Option or Result of one word is a
      16-byte struct (tag, payload), so ME27's convention returns it in
      two registers with no store in the callee (test_natabi_retregs.fd).
      Left: the caller still stores both words into its result slot after
      the call and matches on memory; a payload over one word keeps the
      pointer.
      Nov 18: the caller keeps the tag and payload in vregs as well:
      `ng_rf_promote` (nat_ir.fd, OP12; `FID_NAT_OFF=retfwd`: off) turns
      the stores after the call into moves from the two result registers
      and the match's loads into moves, so the slot's address is gone when
      it goes nowhere the pass cannot see (test_natabi_optmatch.fd,
      tests/nattools/machine_optmatch_check.sh). A Result with a `str`
      payload keeps its slot: the drop call takes its address. Left: mark
      done once the check script passes on the built compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — machine_optmatch_check 0 failures on x86_64, aarch64,
      riscv64 and `--nat`; its MIR checks skip by default (inline2 inlines
      the functions), so checked by hand with inlining off: index_or,
      div_or and count_found have no loads, stores or slot addresses in the
      final MIR on 3 targets (6, 7, 6 with retfwd off). The script should
      keep those functions out of line.
- [x] LF25. A document listing each language fact the optimizer uses, the rule that
      guarantees it, and the test that breaks if the rule is broken (S).
      docs/reference/optimizer-facts.md.

## Track SP — Compile speed (20)

- [x] SP1. No text between source and bytes: our own encoders and image writers,
      no assembler and no linker.
- [x] SP2. Per-stage timing for every build (`--time`), in one table: parse,
      check, lower, each pass, selection, allocation, encoding (S).
      `[prof] native mid-end` and `[prof] native` lines: hello world 0.4 s of
      mid-end, 82 ms of code generation, the front end over 3 s.
      Done: compiler.fd `time_mark`/`time_report` print one `[time] <stage>`
      table on stderr for `check`, `emit` (pe_ir and nat_ir) and, since
      Nov 15, `ccompile` and `dump-c` (rows `lower`, `C generation`, `C
      compiler`): stdlib, runtime, lex+parse, imports, type check, escape,
      lints, borrow check, comptime, mono, lower, iropt, rules, e-graph,
      `nat record` (selection), `nat alloc+emit`, `nat data+image`.
      Left: iropt's per-pass times are a separate list (FID_IROPT_STATS=1,
      iropt.fd), and allocation and encoding are one row (nat_ir.fd).
      Nov 16: both in the table: under the `iropt` row, a `  iropt <pass>`
      row per pass in run order (iropt.fd keeps them when `--time` is on:
      `iropt_set_time_on`, read by compiler.fd `time_mark`), and under `nat
      alloc+emit` the rows `  nat allocation` (time in `ng_mir_alloc`) and
      `  nat peephole+encoding` (compiler.fd `time_row`).
      Checked by scripts/tooling-check.sh (not run yet).
- [~] SP3. `fid emit` of hello world under 0.5 s, then under 0.1 s (release D9):
      today the front end takes most of it (M).
      Measured under competitive CS1: 631 ms with the binary stdlib and
      the function cache warm (-O2 compiler; front end 138, lower 185,
      mid-end 256, native 43 ms). Left: both targets; lowering and the
      mid-end are now most of it.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the whole mid-end works only on what main reaches (e-graph
      functions on hello `--nat`: 152 of 834; a sample 58/54 same), but an
      uncached `emit hello --time` takes 1.0–1.2 s on a loaded machine
      (iropt ~180 ms, lowering ~270, native code ~300). The check script's
      "4 ms" was a cache hit, and its hello expectation is wrong. Left:
      0.5 s, then 0.1 s.
- [x] SP4. The checked stdlib cached in binary form, not re-parsed on every build
      (L).
      Competitive CS2: compiler.fd `astcache_*` + ast_ser.fd keep the merged
      core+runtime Program as a binary file keyed by every library file's
      hash and the compiler's; the D9 stamp skips core's checks. Nov 15:
      builds that merge no runtime (`check`, `ccompile`, `dump-c`, `run`)
      write and read the artifact too (core alone), and a process that holds
      that core already (the LSP, `fid serve`) uses it without reading the
      file. `FID_ASTCACHE=off` turns it off (SP20).
- [x] SP5. Code generation in parallel: functions handed to worker threads once
      the IR exists; the image assembled in order (M).
      Done: workers on threads (FID_NAT_PAR=<n>, compiler/nat_ir.fd
      ng_par_finish/ng_par_thread), one buffer per function joined in
      placement order, the image byte-identical with 1 and N threads
      (tests/nattools/par_emit_check.sh, par_emit_globals.py;
      docs/reference/parallel-emission.md). (CS5's `FID_PAR` splits the
      borrow check, SP9.)
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — par_emit_globals.py finds no module-state write from the
      workers (603 functions); test_par_gen_threads and test_par_emit right;
      par_emit_check 88 ok, its 4 failures out-of-memory builds that give
      identical images when rebuilt; 1/4/8 threads identical on every
      target, the par mutant caught. Not run: the PAR_BIG self-build timing.
- [~] SP6. A per-function code cache: key = hash of the function's IR, the target,
      the options and the clobber/effect facts of what it calls (M).
      Competitive CS6: `FID_FNCACHE=<file>` (opt-in, nat_ir.fd): key = the
      whole IrFunction plus a salt of layouts, globals, externs, the
      function set, the architecture, the pass switches and the library
      facts. Left: on by default; a hit's image is laid out differently
      from a fresh build's (SP20).
      Nov 18: on by default for every machine-layer build:
      `<output>.fides-fncache` (main.fd debug_cache_setup, nat_ir.fd
      nc_path_pick); `FID_FNCACHE=off`, `--no-cache`, `--no-fncache` or
      FIDES_NO_CACHE: none. It is used with struct results in registers
      (`ng_rr_plan`) and RISC-V short calls; an entry is checked against its
      callees' clobber masks and result conventions before it is replayed
      (docs/reference/native-fncache.md). The image layout of a hit is
      SP20's.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the dependency records (nc_validate) work for
      test_machine_fncache_deps (245 hits, hit == miss), but
      machine_fncache_check fails 14: hit images differ from miss images for
      test_machine_fncache and test_repro_fncache on 3 targets, and after a
      one-function edit nothing is reused (0 hits, 240 compiled). Left:
      reuse after an edit, hit == miss.
      Nov 21 (c082ee6): nc_skel_fn replays the recording's register asks first (2g.82, fncache_hit_order_check.sh).
      Nov 22 root-cause audit: hit ≠ miss is neither registers nor relocation
      kind — it is image layout driven by label allocation order. (1) The
      recorded log (nc_log) covers only F:/S:/G:/N: asks during ng_gen_function;
      labels made at emission time — internal block labels (NatEnc_label) and
      panic-line stubs (ng_panic_label, key "which:line") — are re-created by
      nc_replay in ng_mir_finish's postorder, not interleaved as in a fresh
      build, so anything ordered by label creation (panic stubs g.pl_list,
      strings g.strs, globals g.globs, named_fns) comes out reordered and every
      fixup in replayed bytes resolves to different offsets. Fix: extend the
      log to emission-time label asks, or make emission-time labels
      identity-keyed like P: already is. (2) Edit-reuse 0/240: nc_give_up_callers
      drops the entry and every caller that recorded *anything* (trigger
      `dep_masks >= 0` — true for essentially every call edge) instead of
      comparing the recompiled callee's new mask against the recorded one
      before dropping; nc_masks_hold already computes that comparison at
      replay. (3) MI28 tail-call summaries are order-dependent (native-fncache
      §88–95): a hit caller can get a call where a fresh build emitted a jump;
      needs a deterministic summary ask order. All local changes, none alter
      codegen semantics.
- [~] SP7. Incremental relinking in memory: unchanged functions keep their bytes,
      only relocations are redone (M).
      On an FID_FNCACHE hit a function's bytes are replayed at the new
      offset and its fixups registered again (CS6). Left: in memory across
      builds (it is a file read by a fresh process), and only the changed
      functions placed again.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — relinking in memory works when the layout is kept (aarch64
      build 4: 241 functions in place, 1 compiled again), but relink_check
      fails 10: an edited program never reuses the kept layout ("a fresh
      layout (the last build was of another target, cache or program)"),
      149 of 241 unchanged functions move, and on x86-64 and `--nat` the
      changed function goes after the others. Left: those (SP6's problem).
- [~] SP8. A one-line edit rebuilds in under 100 ms (with SP4–SP7) (L).
      ~0.5 s on hello with every cache warm (CS6, Nov 13).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — `fid serve` builds after an edit re-check only what the edit
      reaches (2 of 5 bodies) but take 0.9–2.0 s (release 1840/2023 ms,
      debug 1241/899 ms on a busy machine), and the image after an edit
      differs from a one-shot emit (sp8-edit-rebuild-check fails steps 2
      and 3). Left: under 100 ms, the same image as a one-shot build.
      Nov 21: see SP20 — hit-order fixed; `fid serve` rebuilds not re-checked.
      Nov 22 audit: warm AST/module/body/function caches exist and the
      in-memory IR cache is opt-in (`FID_SERVE_IRCACHE=1`); the measured
      latency and image mismatch above still fail acceptance. Keep partial.
      Nov 22 ad-hoc probe with `FID_SERVE_IRCACHE=1` (`release/fid.exe`,
      x86_64-linux): the edited rebuild took 9266 ms; its output matched a
      one-shot emit for the small 3-body fixture. This was on a busy host
      during other builds, so it is diagnostic, not a benchmark result;
      it confirms the 100 ms target is not close.
- [~] SP9. Front end in parallel per module where modules do not depend on each
      other (L).
      Competitive CS5: `FID_PAR=N` splits the borrow check's functions over
      N threads (opt-in; diagnostics as sequential), measured a net loss.
      Left: imports, type check and escape per module.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the import closure is lexed in parallel: test_lex_par the
      same over 10 runs, lex_par_bench 1013 ms on 1 thread to 255 ms on 8
      (same tokens), imports_par_check ok. Left: parsing, type check and
      escape per module in parallel.
      Nov 22 blocker audit. Parser: 15 g_* globals are written during a parse
      and would race (g_import_set, g_qualified_calls, g_at_imports,
      g_turbofish_types, g_pd_sequential_vars — the last two accumulate across
      ALL parses and are read later by mono.fd/lower.fd); worse, module N's
      parse consumes the accumulated struct/enum name sets of modules 1..N-1
      (cached_parse's struct_names/enum_names, with a deferral scheme), which
      is the real cross-module data dependency. Typecheck: one TypeChecker
      instance accumulates every module's symbol tables (register_program is
      the shared-table writer; bodies mostly read), plus 35 module globals
      written while checking (g_tc_program_module, g_tc_program_copy, the
      generic-call memo g_tc_gc_*, spec sets, comptime depth — the comptime
      evaluator runs the interpreter mid-check). The pattern to copy already
      exists: escape.fd's parallel reporting pass (esc_par_copy, per-worker
      checker copies, globals set before spawn / reset after await). Plan:
      (1) move the g_pd_* scratch into Parser, make turbofish/sequential vars
      per-program mergeable Vecs; (2) parse level-by-level over modgraph_levels
      (already computed), seeding name sets from a token-level pre-scan
      (extend token_use_paths to type names) instead of parse order;
      (3) typecheck = sequential registration then parallel body-checking with
      per-worker memo/spec sets merged deterministically. Parse-level
      parallelism is moderate; typecheck is the hard one.
- [~] SP10. No linear scans anywhere in the compiler: every lookup by name or id is
      a table (release CS8) (M).
      Competitive CS8: instructions, functions, strings, structs, methods,
      parser name sets by table. Nov 15: `IrModule_add_function`,
      `IrModule_replace_function(_named)` (once per lowered function: the
      scan was quadratic) and `pe_module_has_fn` go through fn_index
      (ir.fd `IrModule_fn_pos`, which rebuilds a stale index). Left:
      `IrModule_get_global`, the borrow checker's prefix queries (CS8).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — both are tables now (IrModule_get_global through
      global_index, borrowck.fd bc_borrow_on_prefix): test_borrow_prefix_index
      right on 3 backends, 33 of 33 borrow/race _fail tests give their
      errors, par_check ok. Left: linear scans in mono.fd (generic
      functions and methods), lower.fd Lowerer_lower_vec_sret_call and the
      typechecker.
- [~] SP11. Arena allocation for compiler data per function, freed in one step
      (M).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — per-function arenas for the allocator (`FID_FN_ARENA=1`):
      fn-arena-check 0 failures (321 windows back at their mark), and
      FID_VERIFY=1 arena builds give the same images on 3 targets.
      fn-arena-emit-check fails 3: the native emit of compiler/main.fd does
      not finish (an iro_sccp overflow panic or out of memory). Left: that
      comparison, the `--leaks` run of an arena compiler.
      Nov 21 (5fb5516): 2g.89 fixed — iro_sccp_eval bounds every BinOp before computing it and BitNot folds as `av ^ -1`, so `emit compiler/main.fd` no longer panics on the mid-end (test_sccp_bitnot_min).
- [~] SP12. Self-compile under 60 s, then under 10 s (M, then L).
      Competitive CS4: `dump-c compiler/main.fd` ~25 s wall with the -O2
      compiler (under 60 s). Left: under 10 s (lowering and C generation
      are ~22 s of it).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the lowering changes keep their answers (test_sp12_lowering,
      test_cow_names, test_stack_lower pass), but lowering is still
      super-linear: sp12_scaling.sh 400 fails (N=400 -> 800 takes 3.8–4.6x,
      limit 3.5). `check compiler/main.fd --time` takes 35 s; the dump-c
      self-compile was not timed with this build (memory). Left: under 10 s.
- [~] SP13. A debug mode with no mid-end and the fast allocator (RA14): 5x faster
      than release mode (S).
      `fid emit --debug`: no iropt, rules or e-graph (main.fd cmd_build);
      Nov 15 it also sets nat_ir's `g_nat_fast` (RA14's allocator).
      Left: measuring the 5x (scripts/tooling-check.sh prints both totals).
      Nov 17: emit --debug on the machine layer keeps the function cache
      beside its output (<output>.fides-fncache, main.fd debug_cache_setup;
      --ir-cache opts into the lowered-IR cache until ir_ser's format is
      compact); tests/tooling/sp13-debug-cache-check.sh; measured about 2x
      (docs/perf/sp13-debug-caches.md). Nov 17: ir_ser.fd's lowered-IR cache
      stores its byte buffers one byte per byte (iw_bytes/ir_bytes, file
      version 2; it wrote 8 bytes per byte, twice nested: a 357 MB cache).
      Left: the lowered-IR cache by default and lowering's fixed cost.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — `emit --debug` is 1.7x faster than release (1100 vs 1867 ms
      on a busy machine), not 5x. sp13-debug-cache-check fails 3 (its
      `--stats` run gets no cache; release builds write caches since CB3),
      and by hand an image from the warm cache after a one-line edit differs
      from a `--no-cache` build (32,352 bytes; both run right). Left: 5x,
      warm images equal to fresh ones, the lowered-IR cache by default.
      Nov 21: see SP20 — the hit-order cause of 2g.82 is fixed; a warm `--debug` image vs `--no-cache` is not re-checked.
- [~] SP14. The e-graph bounded so the release mode stays within 2x of debug (S).
      Bounded: egraph.fd `EG_MAX_NODES` (20,000) and `EG_MAX_ROUNDS` (8).
      Left: the 2x measured (hello: e-graph 140 of 631 ms, CS1).
- [x] SP15. Compile time measured on every commit, with a limit per stage that
      fails CI when exceeded by 10% (M).
      Nov 15: `scripts/compile-gate.sh` — `--save` records a per-stage baseline
      from `fid check compiler/main.fd --time`; a run fails when a stage over
      a minimum size regresses past the limit (default 10%). Nov 16: in CI
      without a dedicated machine: bench.yml already builds the base
      commit's fid and this one's on the same runner, so the base one records
      a scratch baseline (`COMPILE_GATE_BASE`, `--save`) and this one is
      checked against it, every push and pull request (the checked-in
      baseline stays a local machine's). Measured set: `fid emit --target
      x86_64-linux --time` on hello, test_hashmap, test_tasks.
- [~] SP16. The compiler itself built by our own backend and fast (bootstrap with
      `fid emit` instead of the C path) (L).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — machine_zero_words_check 0 failures and
      test_midend_sra_inline_payload right, but machine_sp16_check fails 4:
      the `--nat` front end still crashes (Parser_parse_unary_inner; right
      with `FID_IROPT_OFF=sra`, so the ime_sra miscompile is still there),
      the extern probe is not rejected, the x86_64-linux main stack
      overflows. Left: those, then the compiler built by `fid emit`.
      Nov 21 (5fb5516, 2g.94): the `--nat` crash of a self-hosted front end was nat_ir's ng_rf_promote writing a promoted field's zero-words before the prologue's argument moves — ng_rf_zero_at emits them after; ime_sra itself was right.
- [x] SP17. Memory of a build under 1 GB for the compiler itself (M).
      Measured Nov 15: `fid check compiler/main.fd` peaks at ~550 MB —
      under 1 GB. (`dump-c` peaks higher through the 76 MB C text but a
      self-check is the build gate.)
- [x] SP18. A daemon mode (`fid serve`) that keeps the checked stdlib and caches
      warm between builds (M).
      Nov 15: `fid serve` (main.fd cmd_serve): `check <file.fd>` a line on
      stdin, each answer ending in `done <status>`; the merged stdlib stays
      in memory between requests (compiler.fd `set_astcache_memo`, keyed by
      the library hash, so a changed core is read again).
      scripts/tooling-check.sh (not run yet). Left: `emit`/`ccompile`
      requests, the function cache in memory, a socket instead of stdin.
      Nov 16: `emit`/`ccompile` requests; `--port <n>` serves over TCP
      (SP19). Nov 17: the function cache stays in memory between requests
      (cmd_serve sets g_nat_fncache_mem; --no-cache/--no-fncache per
      request); the back ends' flags are reset after each request
      (serve_reset_backend_state, which also turns `--remarks` off:
      iropt.fd iropt_clear_remarks); tests/tooling/serve-fncache-check.sh.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — serve-fncache-check (the second emit takes 239 functions
      from memory, same image, no cache file, flags reset),
      serve-build-check, serve-modcache-check and serve-socket-check pass (a
      check through the server 360 ms vs 1556 ms one-shot).
      docs/reference/serve.md does not describe the memory cache yet.
- [x] SP19. `fid check` and the LSP share the daemon's state (M).
      Nov 22 validation (`release/fid.exe`): a live `fid serve --port`
      answered `fid check` via `FID_SERVE`; served and one-shot diagnostics
      matched; `tests/tooling/serve_lsp_probe.py` confirmed the LSP received
      identical diagnostics and the server logged its `check-text` request.
      The probe used a temporary profile and stopped the server cleanly.
- [~] SP20. Builds are reproducible byte for byte with the cache on and off (S).
      Nov 15: `FID_ASTCACHE=off` turns the binary stdlib off;
      scripts/repro-check.sh compares `dump-c` and `emit` with it off, on a
      miss and on a hit, and with FID_FNCACHE (not run yet). Left: running
      it; FID_FNCACHE hits relink replayed functions, so their images are
      not byte-identical (CS6).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — builds are not byte-identical with the function cache on:
      STRICT=1 repro-check fails ("x86_64-linux executable, fn cache hit =
      off" differs for hello, test_repro_fncache, test_hashmap and
      test_tooling_explain), and machine_fncache_check's hit images differ
      from the miss images on 3 targets (25,603 bytes on x86-64); the hit
      images run the same. The C compares, cache misses and the tail-call
      summary part are byte-identical.
      Nov 21 (c082ee6): one cause of 2g.82 fixed — a cache-hit image orders its functions as the recording asked, before the skeleton's call edges (nc_skel_fn; fncache_hit_order_check.sh). Byte-identical as a whole (debug sections, `fid serve`) not re-verified.

## Track RL — Reliability: trusted without decades (25)

- [~] RL1. Differential testing against LLVM: random programs (release VF1/VF2)
      built by every backend and by the C path through clang must agree on output
      and exit status (M).
      `scripts/fuzz/gen_vs_c.py` (VF2): each random program has an equal C
      program built by clang (`zig cc` -O0/-O2) and GCC; `run`, `ccompile`,
      `emit` must match it. Nov 15: `--backends all` adds `ccompile-O2`,
      `nat` (`emit --nat`), `emit:<target>` and `ccompile:<target>` for the
      three Linux targets (run under qemu-user, or WSL on Windows). Left: a
      recorded run with every backend (not run since this change).
- [~] RL2. The suite run on every backend and target on every change: Windows x64,
      Linux x64, AArch64 and RISC-V under QEMU, WASM (M).
      Done (Nov 15): `scripts/suite-all.sh` runs the suite on every lane the
      host can run (C path and our backend: x86-64/AArch64/RISC-V Linux,
      WASM, Windows pe_ir (`cross-test.sh x86_64-windows-pe`, new) and
      `--nat`), each against the interpreter; `scripts/ci/cross.yml` has a
      `native` job running it for the three Linux targets with
      `FID_VERIFY=1`. Left: that workflow is not active (it takes hours;
      copying it to .github/workflows/ is the user's call), so "on every
      change" is not true yet; no recorded run.
- [~] RL3. A pass on/off matrix: the suite with each mid-end pass and each rule
      group disabled gives the same output (M).
      `scripts/pass-matrix.sh` (`NATIVE=1`). Nov 9, x86-64 Linux, 8 tests
      (integers, checked arithmetic, closures, generics, maps, match,
      floats, tasks), 6 iropt and 7 native switches: each one off gives
      the same results as all on. One run had a missing interpreter
      reference (exit 127, out of memory), not a difference. Nov 15: the
      script takes every switch from the compiler's source
      (`scripts/switch-check.sh --list`: on Nov 15 14 iropt and 22
      backend/rule-group switches incl. rules, purecse, egraph, forward,
      peep, pairs, probe, coldpanic; it tried 6 and 14 before), adds a third round (`FID_IROPT_ON=all`, then each of the 16
      opt-in passes off) and compares results test by test, not by count;
      any target incl. x86_64-windows (`--nat`). Left: a run on the whole
      suite on each target (not run since this change).
- [x] RL4. A miscompile is fixed only with a test that fails without the fix
      (always) (S).
      Done (Nov 15): `scripts/miscompile-gate.sh` — every entry of
      docs/verification/miscompile-log.md marked fixed must name a test
      file under tests/ (it accepted `compiler/iropt.fd` before: M9–M11 had
      no test; now `test_verify_m9_ipcp_mut_vec.fd`,
      `test_verify_m10_m11_vec_get_native.fd`), every repro must be in the
      log; CI job `gates` runs it on every push. `--run` runs those tests
      (must pass), `--before <old fid>` with a compiler without the fix (must
      fail); `//@ env:` in a test sets a pass switch (expect.py). Left: the
      `--before` step is by hand at fix time, not automatic in CI; the 10
      entries fixed before the log have no test.
      Nov 22 code audit: --before automated in bench.yml (miscompile-gate.sh --run --before); all pre-log entries now have catching tests the gate enforces (miscompile-log.md Earlier table).
- [~] RL5. Translation validation for the mid-end: for each function, a checker
      proves the optimized IR equals the input on random inputs (L).
      Not started: docs/verification/translation-validation.md is the
      design (VF6). Needs a `--validate` mode in compiler/iropt.fd and a
      checker module in compiler/ (not this track's files).
      Done (Nov 16): `compiler/tv.fd`. `--validate` (or `FID_VALIDATE=1`):
      `iro_time` keeps each function before a pass and `tv_after` checks the
      result; a rejected one is reported and undone. `tv_random` runs both
      functions on 12 inputs (edge, small, random) in a small evaluator
      (integers, plain locals, uninterpreted calls traced with prints) and
      compares outcome and trace; witnesses for fold/dce/gvn/ssa
      (`tv_structural`, VF6). Test `tests/bootstrap/test_ssa_validate.fd`.
      Left: run the suite with it on and triage each rejection; the module
      passes, licm and devirt are not hooked; `tests/verify/tv/` pairs.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the before/after pairs work (tv-check: 56 pairs of
      tests/verify/tv as expected, wrong expectations caught), but
      `FID_VALIDATE=1 emit test_ssa_validate.fd` rejects inline2 on Str_drop
      ("the blocks changed"; 1 rejected, 25 not checkable of 37), so
      tv-check ends 3 failed. Left: triage that rejection (a miscompile or a
      false alarm), the suite with validation.
      Nov 21 (c082ee6, 2g.90): injected faults now go to a value the function's behavior depends on (tv_live_of, tv_pure_value; constant globals are facts, tv_gconst_facts) — the 'not caught' cases were faults that changed nothing; the inline2/Str_drop rejection was a false witness (`n/a` when the blocks changed).
- [x] RL6. Translation validation for instruction selection: each selected
      sequence evaluated against its MIR operation on random values (M).
      Nov 16: under FID_VERIFY=1 every machine-layer build first runs
      nat_ir.fd `nat_sel_check`: each integer MIR operation (LI, MOV,
      ADD..SHR, NEG, NOT, DIV, REM, MULH signed and unsigned, SETCC with
      every condition and against the zero register, EXT of every kind,
      ASR63, ADDSHL; distinct and aliased registers) is selected alone with
      physical registers through `ng_mi_dispatch`, the replay's own path,
      and nat_enc.fd `nat_sel_eval_case` runs the bytes in an evaluator of
      the x86-64, AArch64 and RISC-V instructions those selections emit, on
      edge and pseudo-random values, against `nat_sel_ref`; the inputs
      must also survive. An instruction the evaluator does not know fails
      the build, so a new selection cannot pass unchecked; the selections
      that depend on `--cpu` (BMI2, Zba/Zbb) are checked for the cpu of the
      build. Test program test_nattools_selcheck.fd, scripts/nat-tools-check.sh
      (not run yet). Left: floats, loads/stores, branches and calls.
      Related: `fid test-rules` checks the MIR rewrite rules (rules.fd).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — floats, memory, branches and calls are checked too
      (nat_enc.fd NselM, nsel_exec_m): selcheck_more_unit 18 of 18 lines (12
      of 12 mutants caught), test_nattools_selcheck 17/17 and _more 19/19,
      nat-tools-check's RL6 section ok on 3 targets plus x86-64-v3 and
      rva22u64. Vector ops, MO_ALUM/BRCCM/SETCCM/ALUMST and jump tables
      stay outside, as documented.
- [x] RL7. The allocation checker (RA15) on every build in CI (S).
      Nov 15: `natgate` job in .github/workflows/ci.yml runs
      `scripts/nat-gate.sh` (IR verifier + register-allocation checker via
      FID_VERIFY=1, `fid test-rules`, the z3 proofs) on x86_64-linux. (The
      pass matrix is not part of nat-gate: RL3.)
- [x] RL8. Encoder oracle tests (TG4) for every target in CI (S).
      TG4. Nov 15: it was not in CI (no workflow ran
      `scripts/enc-oracle.py`); now a step of the `natgate` job runs it for
      AArch64, RV64 and x86-64 against clang's assembler (not yet run there).
- [~] RL9. Image files checked by the system tools: `llvm-readobj`, `dumpbin`,
      `readelf`; every executable we write must load and pass them (S).
      `readelf` reads our ELF executables and their debug sections without a
      warning. Nov 15: `scripts/image-check.sh` builds every writer's images
      (ELF for the three Linux targets, PE from pe_ir and `--nat`, Mach-O
      for both macOS targets) and runs each installed reader: readelf,
      llvm-readobj, llvm-objdump, llvm-dwarfdump --verify, dumpbin,
      objdump, scripts/macho-check.py; a warning or error fails the image.
      Nov 17: the two writer bugs image-lint.py found (no SHT_DYNAMIC
      section for PT_DYNAMIC; COFF long names at string offset 0) fixed by
      TG13, checked on real programs by
      tests/nattools/rl9_writer_fixes_check.sh
      (docs/verification/rl9-rl25-checks-2026-11-17.md).
      Left: a recorded run with every reader installed; loading (running)
      each image is cross-test.sh's part.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — dynamically linked ELF images carry .dynamic at PT_DYNAMIC
      and pass image-lint and readelf on 3 Linux targets; image-lint-check 0
      failed; image-check on hello 22 passed, 1 failed: the COFF object
      (`codegen --coff`) does not build (LoadLibraryA, GetProcAddress,
      FreeLibrary undefined). Left: the COFF path, a run with
      llvm-readobj/objdump/dwarfdump installed.
      Nov 21 (5fb5516, 2g.110): COFF objects name their imports — nat_coff_imports writes undefined symbols, ADDR64 relocations and /DEFAULTLIB: directives (coff_obj_imports_check.sh).
- [x] RL10. Fuzzing of the IR -> machine code path with random IR (not only random
      source), so backend bugs show without front-end limits (M).
      Nov 16: a whole module to a file and back (ir_ser.fd ir_module_save /
      ir_module_load: every table, the target it was lowered for); `fid emit
      <file> --save-ir f.fir` writes what goes into the machine layer and
      `fid emit-ir f.fir [--target t] [--mid-end]` runs the machine layer on
      it alone, after the IR verifier (a bad file is an error, not a crash).
      `fid ir-fuzz <seed>`: a program whose function is IR made from the
      seed (ir_ser.fd irfuzz_function: i64 arithmetic incl. shifts and
      division by safe constants, if/else on comparisons, counted loops, in
      stack slots), printing `expected: <v>` (irfuzz_eval works the value out
      from the same description); `scripts/fuzz/fuzz_ir.sh [n]` builds and
      runs them (natively or qemu) and keeps a failing seed's IR.
      tests/bootstrap/test_driver_save_ir.fd. Left: a recorded run; calls,
      floats and heap memory in the generator.
      Nov 17: the random IR has calls, f64 and heap memory (compiler/ir_ser.fd
      `irfuzz_function`, `irfuzz_helper_function`): f64 slots, conversions,
      constants, arithmetic and compares; a `p_malloc`'d buffer of 8 words
      with constant and computed element addresses; calls from fz_target to
      fz_h0 and fz_h1 and from fz_h1 to fz_h0, whose IR is random too
      (`irfuzz_install_helpers`, called by main.fd `irfuzz_install`).
      Checks: tests/bootstrap/test_irfuzz_kinds.fd,
      tests/tooling/irfuzz-mirror-check.sh (the evaluator against `fid run`
      of the same functions as source), tests/tooling/irfuzz-targets.sh
      (fuzz_ir.sh on every target, with and without `--mid-end`);
      docs/verification/rl10-irfuzz-2026-11-17.md. Left: the per-target runs.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_irfuzz_kinds all yes, irfuzz-mirror-check 120 values
      PASS; 50 seeds per round (not 200) on x86_64-windows and x86_64-,
      aarch64- and riscv64-linux, with and without `--mid-end`: every
      failure was out of memory or a build that passed when retried.
      irfuzz-targets.sh as committed cannot run the Linux rounds (CRLF, a
      Git-Bash FID path, MIDEND=0 read as on); the doc's table is not filled.
- [~] RL11. Mutation testing of the backend: a deliberately broken rule or pass
      must fail a test (L).
      `scripts/verify/variants.py mutate` (VF7) had mid-end mutants only, two
      of them dead since the reformat (their text no longer matched). Nov 15:
      fixed, and five backend mutants added on the `nat` backend (`emit
      --nat` / `--target x86_64-linux`): a wrong rule `0 - x = x`, `x < x`
      true, x86-64 disp8 accepting 128, `cqo` as a nop, calls clobbering no
      register; `toggle` also tries every run-time switch from the source.
      Left: a recorded run (each mutant is a compiler build).
- [~] RL12. Line and branch coverage of `compiler/mir*.fd` and every target over
      90% from the suite and the fuzzers (M).
      `scripts/verify/coverage.py` (VF8): IR-block (branch) coverage of
      compiler/*.fd per module from an instrumented compiler. Nov 15: modes
      `emit:<target>` and `nat`, `--gen N` (the fuzzers' programs), and
      `--goal 90` failing below the goal for the machine layer (nat_ir.fd,
      nat_enc.fd, rules.fd, egraph.fd; there is no mir*.fd). Left: a run;
      the 90% itself; line coverage (the C carries no source lines).
- [~] RL13. Deterministic output: no hash-map order reaches code generation; the
      same input gives the same bytes on every host (M).
      `scripts/verify/repro.sh` (VF12): two builds in different directories,
      temp folders and times compared byte for byte. Nov 15: it also builds
      the machine layer's images (every Linux target, `--nat`), and
      `MANIFEST=` / `--compare` compare the host-independent outputs of two
      hosts. Left: a run on two hosts; R1 (PE timestamp of ccompile) open.
- [~] RL14. The fixpoint built by our own backend: stage 2 == stage 3 through
      `fid emit` on every native host (L).
      Done (Nov 15): `scripts/native-fixpoint.sh` (BACKEND=pe, nat or
      linux) builds compiler/main.fd with `fid emit` from a snapshot, then
      with that stage, compares the two byte for byte and checks stage 3
      runs and compiles hello. Left: no run yet — whether `emit` builds the
      compiler at all is not known.
- [~] RL15. Sanitizer runs of the compiler itself through the C path (ASan,
      UBSan), finding its own memory errors (M).
      Done (Nov 15): `COMPILER=1 scripts/sanitize.sh` builds fid's C with
      the sanitizers (build-variant.sh `EXTRA_CFLAGS`), then has it check,
      lower to C and interpret each test (`SELF=1`: compile itself); a
      report, or C text different from the plain compiler's, fails. Left: a
      run (Linux or macOS clang).
- [~] RL16. Runtime checks of the runtime: `--checked-alloc` and `--leaks` on every
      native target, not only C (M).
      `fid emit --checked-alloc` exists on pe_ir (Windows; `g_pe_checked_alloc`,
      release M7). Left: nat_ir targets (Linux, macOS, `--nat`) and `--leaks`
      on any native target (compiler/nat_ir.fd, stdlib/linux_runtime/).
      Nov 16, nat_ir targets (not run yet): `fid emit --checked-alloc` and
      `--leaks` reach the runtime as the initial value of base.fd's
      `lrt_mem_mode` (nat_ir.fd ng_mem_mode; main.fd sets g_nat_leaks).
      base.fd `pe_dbg_malloc` / `pe_dbg_free` (class 62, an 80-byte prefix):
      checked blocks have pages of their own before an inaccessible page,
      and freed ones become inaccessible for good; the program's blocks
      (those pert_malloc's caller is not runtime code asked for) are on a
      list under the heap lock with the first two program functions on the
      stack, reported at exit as the C runtime does ("every block was
      freed", or bytes and blocks by place); main's return reports on
      Windows (`--nat`) and macOS too. win.fd's mprotect allocates nothing
      now (it ran inside the allocator). Test: test_natabi_memcheck.fd;
      `NATIVE=<target> scripts/leak-check.sh` runs tests/leaks natively.
      Left: `--leaks` on pe_ir (the default Windows backend), and a run.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — `--leaks` works on `--nat` and x86_64-linux (2 blocks, 128
      bytes reported by place) and on C, but a pe_ir `--leaks` executable
      dies at once with "Panic: Illegal instruction" (pe_leaks_leaky.fd):
      pe_leaks_check fails 5, the pe_ir leak gate fails 10 of 10, and pe_ir
      `--checked-alloc` with `--leaks` crashes the same way. Left: the pe_ir
      wiring of the p_malloc/p_free thunks.
      Nov 21 (5fb5516): `--leaks` runs on pe_ir — the pert_leak_* thunks and the stack walk's functions are emission roots (`pe_ir_needed_fns`; without them the walk was ud2, 2g.81), the pe_lk_* layer is in pe_runtime/base.fd and `pert_to_str` no longer leaks a p_malloc buffer. Left: nat_ir targets.
- [x] RL17. Concurrency testing of the runtime: thousands of task and thread
      interleavings under a scheduler that picks randomly (L).
      Nov 16: FIDES_SCHED_RANDOM=<seed> in both native runtimes
      (pe_runtime/conc.fd, linux_runtime/conc.fd "Random-choice
      scheduling"): one worker, the next task taken at random among the
      runnable ones, a random preemption at loop safepoints, random 0/1 ms
      sleeps at thread starts, mutexes, atomics and channels; the choices
      follow from the seed. `interleave.py --sched-random` gives each run
      its seed (`test_runtime_sched_random.fd`). Nov 17: the C runtime too
      (codegen_c.fd "Roadmap RL17": a shuffled pass of __p_co_round, random
      yields at atomics, mutexes and channels, random sleeps off tasks; a
      seed replays a one-thread schedule). Runs: C Linux 3000 seeds and C
      Windows 2000 seeds, 0 wrong, every schedule distinct; pe_ir 300 seeds
      (docs/perf/rl17-sched-random.md, tests/sched/sched-random-check.sh).
      The interpreter interleaves since LG11. Not done: loop safepoints
      (fid emits no p_co_tick), enumerating every switch point. Mark [x]
      once tests/sched/sched-random-check.sh passes with this tree's fid.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — sched-random-check.sh passes with this tree's fid (N=300):
      ccompile and emit give the right output for 300 seeds, 300 distinct
      schedules in 300 seeds, a seed replays its schedule. Not run: native
      Linux random runs (the script does not cover them).
- [x] RL18. A public miscompile log: each bug, the pass, why the tests missed it,
      the test that now catches it (S).
      Done: `docs/verification/miscompile-log.md` — every entry names the
      pass, how it escaped the suite, and the repro in `tests/verify/bugs/`
      (run with `scripts/verify/expect.py`).
- [x] RL19. Kill switches for every pass and rule group, documented, so a user can
      work around a miscompile on the day it is found (S).
      Every native pass and switch above can be turned off. Nov 15: they
      were documented only in part (iropt.fd's comment named 6 of 14
      passes; `peep`, `debug`, `pairs` nowhere); now every one is in
      docs/verification/kill-switches.md, and `scripts/switch-check.sh`
      (CI job `gates`) fails when the compiler reads a switch the page does
      not name, or the page names one it no longer reads. pe_ir's passes
      are compile-time `PEIR_*` constants (variants.py toggle).
- [~] RL20. Crash reports from the compiler with the function and pass that failed
      and a reduced reproducer (`fid reduce`) (M).
      Nov 16 (main.fd): `fid <command> ... --crash-report [--reduce]` runs the
      command again as a child with FID_CRASH_TRACE=1: it names each stage
      before it starts (front end, lowering, iropt, rules, e-graph, native
      code, C generation, C compiler) and the machine layer each function
      (nat_ir g_nat_trace, "[nat] <fn>"); on a panic or a crash the report
      gives the stage, the function, the panic and its first frames, and the
      `fid reduce` line (`--reduce` runs it). `fid reduce <file> [--cmd
      "<c {}>" | --fails <backend>] [--match <t>]` is reduce.py's --cmd and
      --fails modes in Fides (no Python). FID_CRASH_TEST=<stage> makes a
      deliberate crash for the check (scripts/driver-goals-check.sh,
      tests/bootstrap/test_driver_reduce.fd). Left: the function inside the
      mid-end's passes (only the machine layer names it).
      Nov 17: a timeout per try in `fid reduce` (`--timeout <s>`,
      `--no-timeout`; main.fd cmd_reduce). The mid-end names its pass and
      function: under FID_CRASH_TRACE each pass of iropt.fd (iro_time, the
      per-function devirt/closuredevirt/quietcall/licm/version/rotate, and
      every module pass with "-") and of iropt_me.fd (ime_function_passes)
      prints "[iropt] <pass> <fn>" (iropt_me.fd ime_trace), which the report
      reads; FID_CRASH_TEST=iropt:<pass>[:<fn>] crashes there. Left: a run
      of tests/tooling/rl20-db7-check.sh (then [x]).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — crash reports name the stage, pass and function (the trace
      names a pass and main; driver-goals-check's RL20 part ok, "reduced 23
      -> 3 lines"), but rl20-db7-check fails: its sum_to is inlined, and the
      Windows reduce watchdog runs `_fid_reduce_cmd.bat` by bare name, which
      fails where cmd does not search the current directory (with that
      unset, 1 try was stopped as it should). Left: those two.
      Nov 21 (c082ee6, 2g.105): the reduce watchdog runs `.\_fid_reduce_cmd.bat`, so NoDefaultCurrentDirectoryInExePath=1 no longer kills every try.
- [x] RL21. A reducer: shrink a failing program automatically while it still fails
      the same way (M).
      `scripts/fuzz/reduce.py`: it shrank programs on which two backends
      disagree (`--bad/--good`). Nov 15: `--fails <backend>` keeps a
      failure the same (stage, exit status, first error or panic line with
      numbers blurred), `--cmd` takes any command; tried on a command
      predicate (22 lines to 4).
- [~] RL22. Two independent paths kept on purpose: our backend and the C path. A
      release ships only when both agree on the suite (S).
      Both paths exist (`ccompile`, `emit`). Nov 15: `AGREE=1
      scripts/suite-all.sh` fails when a test passes on one lane and fails
      or does not build on another; docs/reference/releasing.md step 2 now
      requires it. Left: a release run with it.
- [x] RL23. Signal-safe runtime paths reviewed: crash handlers allocate nothing,
      take no lock (the Linux runtime's `pe_on_signal` already) (S).
      Nov 15: `docs/verification/signal-safety.md` — the crash/signal paths
      allocate nothing and take no lock; panic buffers are preallocated at
      init.
- [x] RL24. Stack overflow reported cleanly on every target: guard regions on
      every thread and task stack (done on native Linux and the C path) (S).
      Native Linux: a 64 KB guard region under every thread and task stack,
      `Panic: Stack overflow` (Nov 8, RT15's rule).
- [~] RL25. Every target's ABI tested against C: calls into C and callbacks from C
      (`qsort`) on every target (M).
      qsort and bsearch call Fides comparisons on the three Linux targets.
      Nov 15: `tests/bootstrap/test_verify_abi_c_records.fd` — 16-byte
      records sorted and searched through C with i32 results of both signs,
      labs and memcmp; suite-all.sh runs it on every lane. Nov 17:
      `tests/bootstrap/test_verify_abi_c_mixed.fd` (f64 arguments mixed
      with i32 and pointers in one C call: ldexp, frexp, modf, strtod,
      fmod); `tests/nattools/rl25_abi_lanes_check.sh` runs both tests on
      every lane the host has (C, pe_ir, `--nat`, the three Linux targets
      native and through C, wasm32-wasi) against their expected output;
      `tests/nattools/tg15_win_dll_check.sh` for DLL imports on `--nat`
      (docs/verification/rl9-rl25-checks-2026-11-17.md). Nov 18: the C
      path keeps extern "C" names (codegen_c.fd extern_func_names: `ldexp`
      was called as p_ldexp_user). Left: a recorded run on Windows (pe_ir,
      `--nat`), WASM and macOS with the final compiler; a Fides function
      passed to an imported C function (ldyn, `--nat` IAT) needs an
      adapter (ConstFnRef gives C the address of a function that takes
      `__env` first), as pe_ir's c_adapters and the C path's `__c_fn_<f>`
      have.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — rl25_abi_lanes_check passes on C, pe and nat, the C cross
      lanes (x86_64/aarch64/riscv64-linux, wasm32-wasi) and the native
      records lanes on 3 Linux targets, but native x86_64-linux
      test_verify_abi_c_mixed crashes in libc strtod (invalid memory access
      at 0x8). Left: that; the aarch64/riscv64 dynamic lanes (no target
      glibc here); the macOS lanes.
      Nov 21 (5fb5516, 2g.99): a dynamic x86-64 image now writes a PT_TLS phdr and keeps its TLS block at `fs:[-8]` (a thread's at fs+72) — libc's strtod no longer faults (ldyn_tls_check.sh).

## Track DB — Debugging and visibility (10)

- [~] DB1. Line tables from the IR to MIR to bytes: every instruction knows its
      source line (M).
      Nov 9: the lowering's source lines (value metadata) become MO_LINE in the
      MIR and rows at emission; `.debug_line` (DWARF 4, one file) for the
      functions of the input file, on the three Linux targets (readelf decodes
      it). Left: rows for statements whose instructions carry no line
      metadata, the other files of a program (functions do not record theirs).
      Checked Nov 15: both need the front end, not the machine layer — the
      lowering (lower.fd) to put a line on every instruction it emits and an
      IrFunction field (ir.fd) for the file a function came from, which
      main.fd then passes instead of the one input file's names
      (`g_nat_lines_names`); nat_ir.fd's rows (`ng_note_line`, MO_LINE) and
      nat_enc.fd's `.debug_line` writer would take a file table from there.
      Nov 16: both done. The lowering gives what it emits outside any
      statement (parameter copies, the drops and return at the end) the
      body's first line (lower.fd `Lowerer_lower_func_uncached`), so every
      instruction has one. The import loops (compiler.fd) and main.fd record
      each function's file (ir.fd `ir_note_program_files`, `ir_fn_file`;
      a map by name rather than an IrFunction field, so serialized IR and
      the caches are unchanged); nat_ir.fd gives a function of another file
      rows against it, the row's third word carrying the file
      (`g_nat_line_file_ix`), and nat_enc.fd's `.debug_line` has a file
      table (`g_nat_line_files`, DW_LNS_set_file). Test program
      test_nattools_lines.fd (with nattools/lines_mod.fd),
      scripts/nat-tools-check.sh (readelf; not run yet). Left: core's and
      the runtime's functions (merged from the binary artifact, no file
      recorded) and monomorphized instances get no rows; code inlined from
      another file keeps its lines but is in the caller's rows.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — library and mono files in line rows: test_nattools_lines_files
      right; dbg_db1_files_check fails 12 of 18 with the default passes
      (str_trim_start and lgf_count__* are inlined into main) and passes its
      core and mono checks with `FID_IROPT_OFF=inline,inline2` on
      x86_64-linux. Left: the check with the default compiler; runtime
      functions now have rows, so dbg_run_check.py puts breakpoints in them.
- [~] DB2. DWARF on Linux and macOS: line numbers, functions, frames (L).
      Nov 9: `.symtab`, DWARF 4 `.debug_info` (a compile unit, a subprogram per
      function) and `.debug_frame` in every native Linux executable; line tables
      since Nov 9 (DB1). Nov 15, macOS: `nat_macho` (nat_enc.fd) writes a
      symbol table, a local N_SECT nlist_64 per function (`_main`, ...) in
      __text, counted by LC_DYSYMTAB (it had LC_SYMTAB with 0 symbols), so
      nm, lldb, atos and Instruments name our functions; `FID_NAT_OFF=debug`
      leaves it out. Check: `python3 scripts/dbg-check.py` (also runs
      scripts/macho-check.py). Left: DWARF on macOS (a `__DWARF` segment or
      a .dSYM with the same `.debug_info/line/frame`), `DW_AT_frame_base`
      and declaration lines in `.debug_info`.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — decl lines are right with inlining off (dbg_decl_lines_check
      0 failed on x86_64-linux, aarch64-macos and `--nat`), but with the
      default passes it fails 19 on 6 targets: main gets the inlined
      callee's `fn` line (56, want 76), and inlined functions have no
      subprogram. Left: nat_dbg_decl_line from the function's own
      declaration, test functions kept out of line, lldb and llvm-dwarfdump.
      Nov 21 (c082ee6, 2g.102): nat_dbg_decl_line scans the function's own rows for its `fn` line — a function that begins with inlined code no longer takes the callee's decl_line.
- [x] DB3. PDB / CodeView on Windows (L).
      Done (Nov 15), first step on the default Windows backend: `fid emit`
      images carry a COFF symbol table after the sections (pe_ir_enc.fd
      `pe_append_coff_symbols`, filled by pe_ir.fd `PeirGen_append_symbols`:
      every function, stub, primitive and C thunk placed in .text, and
      `__fid_entry`), so llvm-nm, llvm-objdump, dumpbin and gdb name our
      functions; `FID_PE_SYMS=0` leaves it out. Check:
      `scripts/pe-syms-check.py` (the image is otherwise byte-identical).
      Left: the PDB itself (MSF streams, CodeView S_GPROC32 symbols, C13 line
      tables from DB1's rows) and an RSDS debug directory pointing at it, so
      Visual Studio and WinDbg see functions and lines; the same for `--nat`
      (since Nov 15 `nat_pe` images carry the same COFF symbol table:
      nat_enc.fd `nat_pe_symbols`, DB10; no PDB yet).
      Nov 16, line tables (written, not run yet): a debug build of the
      default backend (`fid emit --debug`, `-g` or `FID_PE_DEBUG=1`; main.fd
      calls pe_ir.fd `pe_lines_setup`) records a row per source-line change
      of the input file's functions and writes DWARF 4 `.debug_abbrev`,
      `.debug_info` (the compile unit and a function entry each) and
      `.debug_line` sections after .rdata (pe_ir_enc.fd
      `pe_add_debug_sections`: headers grow to 1024 bytes, the long names
      are "/n" into the COFF string table), as MinGW's linker writes them:
      gdb, lldb and llvm-dwarfdump read them. Check:
      `scripts/pe-dwarf-check.py` (from `scripts/pe-win-check.sh`). Left:
      still the PDB and RSDS entry for Visual Studio and WinDbg (MSF
      container, DBI/TPI/IPI streams, S_GPROC32, C13 lines: what WinDbg
      reads, which cannot be checked without it); variables (DB4).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — compiler/pdb.fd writes the PDB and an RSDS debug
      directory: pe_pdb_check passes `--unit` (11 streams, 177 procedures,
      C13 lines, GUID and age match), `--emit` on pe_ir (prog.pdb beside
      prog.exe, lines agree with .debug_line) and `--emit --nat` (305
      procedures, 5338 line rows); dbghelp reads them. pe_ir PDBs list only
      functions with rows; no variables (DB4); WinDbg/VS were not run.
- [~] DB4. Locations of variables at `-O0`-like debug builds, in registers and
      slots (L).
      (Checked Nov 15: not started. `.debug_info` has no DW_TAG_variable or
      DW_TAG_formal_parameter; the pieces exist — the IR's named locals
      (`IrFunction.locals`), a frame slot per IR value (`NatFnRec.slots`)
      and the allocation per vreg (`NatAlloc`), and RA14's fast mode keeps
      values that cross blocks in their slots — but no table from a source
      name to its slot or register per address range.)
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the scope units pass (dbg_scopes_unit 22/22), but no real
      image gets the lexical blocks: dbg_scopes_check fails on every target
      ("shadow: 0 lexical blocks hold x, want at least 2"), `--debug` reads
      the outer binding (x = 6, want 50), optimized main has 0 of 12 values
      after inlining, and a cache hit changes the debug sections. Left: all
      of that; gdb and llvm-dwarfdump runs.
      Nov 21: the shadowed-name half of 2g.102 stays open (no DW_TAG_lexical_block reaches a shadowed binding); the decl_line half is fixed (DB2).
- [x] DB5. Panic and crash traces from frame records, no per-call bookkeeping
      (the C path, native Linux).
- [~] DB6. Unwind information (Windows `.pdata`, DWARF CFI) so system debuggers and
      profilers walk our stacks (M).
      `.debug_frame` CFI for our prologues on the three Linux targets.
      Nov 15: the CFI was wrong in two places, now right: a function with no
      frame record (WP8 leaves; their FDE claimed a `push rbp` / `stp` that
      is not there, so an unwinder stopped in a leaf took the caller's frame
      for its own) keeps the CIE's entry rules (nat_enc.fd `nat_has_frame`
      reads the function's first instruction), and every epilogue gets its
      rules — from the `ret` (RISC-V: from the last `addi sp`) the CFA is
      sp-based and rbp / x29, x30 / s0, ra are restored, with
      remember/restore state around it when code follows (`ng_epilogue`
      records `ng_cfi_row`, `nat_fde_epilogue` writes them; the CIE now
      says same_value for the frame pointer and return address). Check:
      `python3 scripts/dbg-check.py` (runs each FDE and checks the CFA at
      entry, after the prologue and at every ret). Test program:
      tests/bootstrap/test_machine_dbg.fd. Left: Windows `.pdata`/`.xdata`
      for `--nat` (nat_enc.fd `nat_pe` writes none), rules for the
      callee-saved registers the prologue stores, `.eh_frame` for unwinders
      that read only it.
      Nov 18: the Linux images get `.eh_frame`, `.eh_frame_hdr` and a
      PT_GNU_EH_FRAME segment (nat_enc.fd `nat_elf_eh_frame`,
      `nat_eh_frame`, `nat_eh_frame_hdr`; `FID_NAT_OFF=ehframe`: none),
      and shrink-wrapped saves have CFI rows (nat_ir.fd `ng_dbg_sw_states`,
      `ng_dbg_sw_to`; `FID_NAT_OFF=swcfi`: none). Checks:
      tests/nattools/db6_eh_check.sh (with tests/nattools/eh_frame_unit.fd),
      to be run on the first build that has them. Left after that: Windows
      `.pdata`/`.xdata` for `--nat`.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — .eh_frame/.eh_frame_hdr on x86_64, aarch64 and riscv64 Linux
      (241 FDEs, found by libgcc, images run): db6_eh_check 44 ok, 7
      failures: the x86-64 CFI says r14/r15 are saved where the prologue
      stores nothing, and shrink-wrapped functions emit DW_CFA_restore
      before any rule on all 3 targets. Left: those; pe_unwind_check for
      the `--nat` .pdata was not run.
      Nov 21 (c082ee6): the CFI checks model saves deferred to a shrink-wrapped block and the rules DW_CFA_remember/restore_state keep (2g.103 was partly checker artifact); the CFA-24 stores question is not re-checked.
- [~] DB7. `fid explain <function>`: the IR, the MIR after each stage, the machine
      code, side by side (M).
      Nov 15: `fid explain <file> --stages <fn> [--target t]` (also `fid
      emit ... --explain <fn>`; main.fd): the function's IR after lowering,
      iropt, the rules and the e-graph, every instruction with its operands,
      type and source line (ir.fd `IrFunction_detail_str`), then its machine
      code in hex, found through the executable's `.symtab`; nothing is
      written. scripts/tooling-check.sh (not run yet). Left: the MIR after
      each machine stage (nat_ir.fd: `FID_NAT_DUMP=<fn>` prints it only
      after allocation, from the environment), a disassembly, PE and
      Mach-O symbols.
      Nov 16: the MIR after each machine stage. `--stages`/`--explain`
      set nat_ir.fd `g_nat_dump`, and `ng_dump_mir` prints the function's
      MIR on stderr after selection, after the cold-path layout, the
      peephole and branch fusion (FID_NAT_DUMP=<fn> or `*` does the same
      on any build), then the allocation's dump with registers.
      test_nattools_mir.fd, scripts/nat-tools-check.sh (not run yet).
      Left: explain's machine code is still hex (compiler/disasm.fd's
      `disasm_bytes` could print it), and it finds the function through
      the ELF `.symtab` only (PE and Mach-O images have symbols now).
      Nov 17: explain's machine code is disassembled (disasm.fd
      `disasm_bytes`), and PE and Mach-O symbols are read (main.fd: the PE
      symbol table through nat_pe_symbols, a Mach-O's LC_SYMTAB through
      macho_disasm). Left: a run of tests/tooling/rl20-db7-check.sh,
      scripts/tooling-check.sh and scripts/nat-tools-check.sh (then [x]).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `fid explain --stages` disassembles: main is listed on
      x86_64-linux (`<main>:`, `push rbp`), aarch64-macos, x86_64-macos and
      `--nat` Windows; Mach-O disasm and the missing-function message work.
      rl20-db7-check fails because its fixture sum_to is now inlined (no
      `<sum_to>:` on any target). Left: a fixture function that stays out
      of line, then that run; tooling-check and nat-tools-check.
- [x] DB8. Optimization remarks: why a check stayed, why a call was not inlined,
      why a value spilled (`--remarks`) (M).
      Nov 16: the three reasons. Each kept check is listed with what the
      range analysis knew (`kept add overflow check (v12: operands v9 in
      [0, ?] and v11 in [1, 1]: ...)`, a divisor that may be 0, an index
      that may be negative or that no counted loop or length fact bounds;
      checks.fd `explain_kept_why`), each call iro_inline did not inline
      with its test (more than one block, recursive, too large, a call or
      print in its body, @constant_time, ...; iropt.fd
      `iro_not_inlined_why`, printed by main.fd `print_remarks`), and each
      spilled IR value with whether its range crosses a call or the
      registers ran out (nat_ir.fd `ng_mir_stats`). Checked by
      scripts/tooling-check.sh (not run yet).
      Nov 15: `fid emit --remarks` (main.fd `print_remarks`): per function,
      each runtime check the mid-end removed and the proof that removed it
      (counted loop, range analysis), how many stayed (checks.fd
      `explain_checks`), and on nat_ir targets the spills, reloads and moves
      per function (RA17's `[stats]`). Left: the reason each check stayed
      (checks.fd), why a call was not inlined (iropt.fd), why a value
      spilled (nat_ir.fd).
- [~] DB9. A sampling profiler on every native target that names our functions
      (from DB6) (M).
      Done (Nov 15) on native Linux (x86-64, AArch64, RISC-V): `FID_PROF=1
      ./prog` samples every millisecond of CPU time (ITIMER_PROF, SIGPROF
      with SA_RESTART); the handler stub now passes the ucontext
      (nat_ir.fd `ng_sig_fn` -> base.fd `pe_on_signal_uc`), and sys.fd
      `pe_prof_tick` counts the function the interrupted pc is in (self)
      and, walking the frame records (the link register for a leaf or a
      prologue), each function on the stack once (total), by the panic
      traces' table `nat_fn_names`; no allocation, no lock. At exit (main's
      return now goes through base.fd `pe_rt_exit`, and `p_exit`) the table
      goes to stderr, most self time first. A sleep cut short by a sample
      sleeps the rest (conc.fd). Check: `bash scripts/prof-check.sh`
      (qemu-user for AArch64/RISC-V, WSL on Windows; fib first, main on the
      stack); tests/bootstrap/test_machine_dbg.fd runs as before without
      it. Nov 17: the C path (FID_PROF at start, __p_sp_boot; on Windows
      every thread), every thread on --nat Windows (win.fd), FID_PROF=folded
      stacks on C, nat_ir and pe_ir (docs/perf/db9-profiler-every-target.md,
      tests/prof/db9-check.sh). pe_ir's entry point now calls pert_prof_boot
      before main and pert_prof_exit after it (pe_ir_stubs.fd
      PeirGen_gen_entry_point, rooted in ir.fd pe_ir_needed_functions):
      FID_PROF at start on pe_ir, pending a run of db9-check. Left: a run on
      macOS, pprof protobuf output.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — db9-check passes for ccompile, `--nat`, x86_64-linux and
      pe_ir `FID_PROF=1`, but fails 2: pe_ir folded stacks start at a wrong
      root frame (`__c_fn_pe_console_ctrl;main;...`), so no stack begins at
      main. Left: the entry point's symbol on pe_ir, macOS, pprof output.
      Nov 21 (c082ee6, 2g.101): the folded stack roots at `__fid_entry` — it is in the nat name tables and pe_pf_entry_b records its bucket; and the periodic report keeps its own clock (SL15's two-reports-instead-of-three).
- [~] DB10. Symbol maps for every image we write, readable by `addr2line`-style
      tools (S).
      The symbol table (DB2). Checked Nov 15: Linux ELF (`.symtab`) and the
      default Windows backend (DB3's COFF symbols) had them, Mach-O had an
      empty LC_SYMTAB and `--nat` PE images none. Done (Nov 15): `nat_macho`
      writes a local symbol per function (DB2), and `nat_pe_symbols`
      (nat_enc.fd, called by nat_ir.fd for `--nat`) appends the COFF symbol
      table pe_ir's images have; `FID_NAT_OFF=debug` leaves both out.
      Check: `python3 scripts/dbg-check.py`, scripts/image-check.sh (the
      system's readers). Left: a `name` section in `fid wasm` modules
      (compiler/wasm.fd writes none), line information outside Linux
      (DB3's PDB, DWARF on macOS).
      Nov 18: `fid wasm` modules carry a "name" section (compiler/wasm.fd
      `wb_name_section`) and Mach-O images a __DWARF segment with
      `__debug_info`/`__debug_line` (nat_enc.fd `nat_macho_dwarf`). Checks:
      tests/nattools/db10_check.sh (with tests/nattools/
      macho_dwarf_readelf.py), to be run on the first build that has them;
      once it passes, what is left is DB3's PDB.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — wasm names (206 functions) and Mach-O DWARF on both macOS
      targets (295 subprograms, 5696 line rows, 295 FDEs) check out:
      db10_check 19 ok, 3 failures: the units compare stderr build lines,
      macho_dwarf_unit expects `lib/util.fd` where nat_dbg_written_path now
      writes `util.fd`, and dbg-check.py fails on DB2's decl lines. Left:
      those; lldb and llvm-dwarfdump --verify.

---

## Order of work

1. **Measure and protect first:** SP2, RL8 (encoder oracles), RA15 (allocation
   checker on the existing `nat_ir` allocator), ME4 (IR verifier), RL3. These
   make every later change safe.
2. **The machine layer:** MI1–MI11, then the ports in order MI12 (AArch64),
   MI13 (RISC-V), MI14 (x64), each gated on unchanged suite results. RA1–RA8 come
   with it: one allocator for all three.
3. **SSA:** ME1–ME5, then ME3 (mem2reg), which multiplies what every later pass
   can do.
4. **The rules mid-end:** ME6–ME19.
5. **Whole program and language facts:** WP2–WP9, LF1–LF9, LF13.
6. **Speed:** SP5–SP8 once the machine layer is in place.

The x64 port (MI14) needs the agreement of whoever owns `pe_ir.fd` at the time:
it replaces that file's lowering.

## What we do not do

- An LLVM IR or C++ API compatibility layer: the C path is our LLVM.
- Exception tables and unwinding for exceptions: Fides has none (DB6 is for
  debuggers and profilers only).
- A JIT tier: `fid run` interprets, and compile speed (SP) makes builds fast
  instead.
- A second front end language for rules: the rules are Fides data.
