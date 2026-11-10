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
  WP8 and WP17 have uncommitted code (WP8 never fires); WP20's facts
  writer and reader disagree on the effect format.
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

- [~] MI1. Write down the MIR: instructions, operands (virtual register, physical
      register, immediate, stack slot, label), blocks, block parameters; in a new
      `compiler/mir.fd` (M).
      Nov 8–9: `compiler/nat_ir.fd` records each operation of its architecture
      layer as an `MInst` on virtual registers (Cranelift's VCode shape: selection
      happens in the layer), for AArch64, RISC-V and x86-64 Linux.
- [ ] MI2. Target-neutral operations: integer and float arithmetic, compares,
      branch on compare, loads and stores of 1/2/4/8 bytes with extension,
      address arithmetic, calls, returns, traps (M).
- [ ] MI3. Target-specific operations as an open set per target (x64 `lea`,
      AArch64 `madd`, RISC-V `slli`+`add`), chosen by instruction selection (M).
- [ ] MI4. A target description: register classes, callee-saved set, argument
      and result registers, stack alignment, red zone, scratch registers (S).
- [~] MI5. One lowering IR -> MIR for every language rule now written twice in
      `pe_ir.fd` and `nat_ir.fd`: checked add/sub/mul, narrowing, MIN / -1,
      division by zero, struct copies, closures, dyn calls, print routing (L).
      Nov 8–9: one lowering for the three Linux targets; `pe_ir.fd` keeps its own
      until MI14.
- [~] MI6. Panic paths as cold out-of-line blocks shared per function, each a
      call (so traces name the function), from the one lowering (M).
      Panics are calls to shared stubs on every Linux target, so traces name the
      function.
- [ ] MI7. One call lowering: arguments to registers and the outgoing area,
      `__env`, varargs spawns, results; parallel moves solved once (M).
- [ ] MI8. One frame layout: slots, spill slots, the save area, alignment, frame
      records for traces, large frames (M).
- [ ] MI9. Struct values as several MIR values when small (scalar replacement at
      the machine level), memory otherwise (M).
- [~] MI10. A MIR verifier: every use defined, every block terminated, operand
      classes match the operation; run after each MIR stage in debug builds (M).
      The allocation checker (RA15) runs over the MIR with `FID_VERIFY=1`; a
      structural MIR verifier is still to write.
- [~] MI11. A MIR printer and `fid dump-mir <file> --target <t>` (S).
      `FID_NAT_DUMP=<function>` prints a function's MIR with its allocation and
      live ranges.
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
- [x] MI15. A Linux x86-64 target on MIR for free once x64 is ported: the ELF
      writer of `nat_enc.fd`, the Linux runtime, x64 selection (M).
      Nov 9: Linux x86-64 on the MIR (`fid emit --target x86_64-linux`): its own
      encoder, 4 register arguments, the generic system call numbers mapped in
      `nat_syscall`, TLS through fs, FMA3 for `fma` (x86-64-v3 for log/pow only).
      `--elf` / `--target linux` still use pe_ir's no-stdlib path.
- [ ] MI16. A macOS AArch64 target: the AArch64 selection and a Mach-O writer
      (release F7) (L).
- [ ] MI17. Retire `pe_ir.fd`'s and `nat_ir.fd`'s own lowerings once their MIR
      ports match on the whole suite (M).
- [ ] MI18. Retire the old PIR path (`pir.fd`, `opt.fd`, `regalloc.fd`,
      `codegen_x64.fd`) and `build --native` with it (S).
- [ ] MI19. Runtime primitives (`nat_ctx_swap`, atomics, `nat_clone_thread`,
      trampolines) written once in MIR where they can be, per target only where
      they must be (M).
- [~] MI20. Machine code primitives for the runtime already exist per target
      (`ng_gen_primitives*` in `nat_ir.fd`, the stubs of `pe_ir_stubs.fd`); move
      them behind one interface (M).
      Each target's primitives (context switch, thread start, atomics, signals,
      traces) are behind one set of `nat_*` names.
- [ ] MI21. Constants: materialization per target (`movz/movk`, `lui/addi`,
      `mov imm64`) chosen by cost, rematerialized instead of spilled (M).
- [ ] MI22. Address modes as a selection decision: base + index * scale + offset
      where the target has it (M).
- [~] MI23. Compare and branch fused in selection on every target (x64 `cmp/jcc`,
      AArch64 `cbz/tbz/b.cond`, RISC-V `beq/blt`) (S).
      x86-64 tests and jumps fused; AArch64/RISC-V compare to a register and
      branch.
- [ ] MI24. Conditional select (`csel`, `cmov`, RISC-V Zicond where present) for
      small branches with no side effects (M).
- [ ] MI25. Floats in float registers on every target, f32 as f32 (not widened),
      moves between classes only where needed (M).
- [ ] MI26. Narrow integers kept in registers with known extension state, so a
      `u8` add does not re-extend each time (M).
- [ ] MI27. Jump tables for dense `match` (M).
- [ ] MI28. Tail calls as jumps on every target, guaranteed for self-recursion in
      tail position (M).
- [ ] MI29. Stack probes for frames over a page, so a large frame cannot jump over
      the guard region (S).
- [ ] MI30. One place that decides the code alignment of functions and loop heads
      per target (S).

## Track RA — One register allocator (20)

- [~] RA1. Today: `nat_ir.fd` allocates callee-saved registers over live-range
      hulls from block liveness; `pe_ir.fd` has its own allocator with
      coalescing and call-free ranges. Replace both with one allocator on MIR (L).
      Nov 8: one linear-scan allocator for the three Linux targets
      (`ng_mir_alloc`); pe_ir keeps its own.
- [~] RA2. Liveness on MIR from use lists, live ranges as lists of segments with
      holes, not hulls (M).
      Block liveness to a fixed point, live ranges as hulls (not yet segments with
      holes).
- [~] RA3. All registers, caller-saved included: a value live across a call goes
      to a callee-saved register or is split around the call (L).
      Caller-saved registers for ranges no clobbering call crosses (AArch64
      x8–x11, RISC-V t4); x86-64 has none spare.
- [ ] RA4. Live-range splitting at calls and at loop boundaries, with spill code
      placed outside loops (L).
- [x] RA5. Spill weights from loop depth and use counts (M).
      Nov 9: `ng_spill_weights`: each use or definition 8^(loop depth), the
      loops from back edges in the MIR; with no register free the cheapest
      range gives its register up (`FID_NAT_OFF=weights`: the range ending
      last, linear scan's rule).
- [~] RA6. Copy coalescing on MIR (what `pe_ir`'s `PEIR_COALESCE` does), so
      `i = i + 1` is one instruction on every target (M).
      Move hints: a move's two sides prefer one register, so `ng_st_val` after an
      operation costs nothing. Nov 9: coalescing in `ng_mir_alloc` (union-find
      over vreg copies whose two sides never change while the other lives),
      the copy left a move to itself; the checker validates it
      (`FID_NAT_OFF=coalesce`).
- [ ] RA7. Register hints: arguments, results and call operands prefer the
      registers they must end in (M).
- [~] RA8. Rematerialization of constants, addresses of globals and frame slots
      instead of spills (M).
      Nov 9: constants: a vreg defined once by MO_LI is never stored, a spilled
      one is loaded again where used, and its spill weight is 1
      (`FID_NAT_OFF=remat`). Left: addresses of globals and slots.
- [ ] RA9. Second-chance or backtracking allocation in the style of regalloc2 /
      LLVM's greedy, behind a switch, compared with linear scan (XL).
- [ ] RA10. Register classes: integer, float, and pairs where a target needs them
      (S).
- [ ] RA11. Parallel moves at block edges solved with cycle breaking through one
      scratch register (M).
- [ ] RA12. Callee-saved registers saved only on paths that use them (shrink
      wrapping) (M).
- [ ] RA13. Save and restore with paired instructions (`stp/ldp` on AArch64) (S).
- [ ] RA14. A fast mode for debug builds: one pass, local allocation, under 10% of
      compile time (M).
- [x] RA15. An allocation checker: replay the allocated code symbolically and
      prove every use reads the value it should (regalloc2's checker) (L).
      Nov 8: `ng_mir_check`, an independent exact liveness walk; it found a real
      bug at once (a vreg read before its block defines it was treated as local).
- [~] RA16. Run the checker on the whole suite in CI for every target (S).
      Run on the whole suite with `FID_VERIFY=1` on AArch64, RISC-V and x86-64.
      Nov 9: `scripts/nat-gate.sh`, the gate CI runs (one job per host after
      fid is built): the verifier and the checker on every native target,
      `fid test-rules`, the z3 proofs; exits non-zero on any failure.
- [ ] RA17. Statistics per function: spills, reloads, moves; `--stats` prints them
      (S).
- [~] RA18. Floats and integers allocated by the same algorithm, class-aware (M).
      Nov 9, a first step on x86-64: floats still live in general registers,
      but the emitter remembers which register xmm0/xmm1 mirror and does not
      move an operand again (`FID_NAT_OFF=xmmcache`): the float loop 1834 ->
      1364 ms. Left: an xmm register class in the allocator.
- [ ] RA19. A value used only as a branch condition stays in flags where the
      target has them (M).
- [x] RA20. The allocator honours IPRA clobber sets (Track WP) at each call (M).
      Calls only clobber what their callee was found to clobber (WP4).

## Track TG — Thin targets: selection and encoders (20)

- [ ] TG1. Instruction selection as rules: MIR patterns -> target instructions,
      tried by cost, written as data in Fides (an ISLE-like table, no new
      language) (L).
- [ ] TG2. Each rule carries its preconditions (immediate fits, register class)
      and is tested on its own by random operands against the generic lowering
      (M).
- [x] TG3. Own encoders, no assembler: x64 (`pe_ir_enc.fd`), AArch64 and RV64IMFD
      (`nat_enc.fd`).
      x86-64 encoder added to `nat_enc.fd` (Nov 9).
- [x] TG4. Encoder oracle tests: for every instruction form we emit, encode random
      operands and compare the bytes with clang's integrated assembler
      (`zig cc -c x.s`, `llvm-objdump`) (M).
      Nov 8–9: `scripts/enc-oracle.py`, 1,181 forms of AArch64, RV64IMFD and
      x86-64 against clang's assembler: 0 mismatches (it found `clrex #0` and a
      longer `sub rax, imm32`).
- [ ] TG5. A disassembler for our own output (`fid disasm`), table-driven from the
      same encoder descriptions, so the two cannot drift (L).
- [ ] TG6. Branch relaxation: short forms where the target reaches, long forms
      only when needed (x64 `jcc rel8`, RISC-V `b` vs `jal` vs `auipc+jalr`) (M).
- [ ] TG7. x64: memory operands in arithmetic, `test`, `lea` everywhere it pays,
      `cmov`, BMI1/BMI2 behind `--cpu` (M).
- [ ] TG8. AArch64: `madd/msub`, shifted and extended operands, `cbz/tbz`,
      `csel/csinc`, `ldp/stp`, pre/post-indexed addressing (M).
- [ ] TG9. RISC-V: compressed instructions (RVC) for size, Zba/Zbb when present,
      `sext.w` elision (M).
- [ ] TG10. A `--cpu` switch per target with safe defaults (baseline x86-64-v2,
      armv8.0-a, rv64gc) (S).
- [ ] TG11. SIMD as MIR vector operations: SSE2/AVX2, NEON, RVV, for the `simd`
      module and loops the vectorizer picks (XL).
- [ ] TG12. Object files (COFF, ELF, Mach-O) from the same writer as executables,
      for linking with C (release F4) (M).
- [ ] TG13. One image writer for PE, ELF and Mach-O: sections, relocations, symbol
      tables, written once (L).
- [ ] TG14. Position-independent executables on Linux and macOS (ASLR) (M).
- [ ] TG15. Dynamic linking of system libraries where a platform needs it (libc
      on macOS, `kernel32` on Windows already), through import tables written by
      us (L).
- [ ] TG16. A WASM target on MIR: structured control flow from MIR's CFG
      (relooper / stackifier), our own module writer, replacing the invalid
      `wasm.fd` output (L).
- [ ] TG17. Peephole rules per target as data, after allocation: redundant moves,
      load after store, branch to next block (M).
- [ ] TG18. A shared peephole pass on MIR before allocation: dead flags, double
      extension, constant compare (M).
- [ ] TG19. Code size mode (`--size`): RVC, short encodings, no unrolling, shared
      panic blocks (S).
- [ ] TG20. A new target costs under 3,000 lines: description, rules, encoder,
      image writer; measured by the RISC-V port (S).

## Track ME — SSA and a rules mid-end (30)

- [x] ME1. Use lists in the IR, kept current by every pass (M).
      `IroUses`/`iro_uses` in `compiler/iropt.fd`: users per value and the
      block each use sits in; passes rebuild it where they need it (GVN,
      branch facts, forwarding, DCE).
- [~] ME2. Dominator tree and loop tree shared by every pass (`iro_cfg`,
      `iro_dominates` in `iropt.fd` are a start) (M).
      iropt's dominators are used by the IR verifier, the e-graph and forwarding.
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
- [x] ME6. An e-graph data structure: e-classes, hash-consed e-nodes, union-find,
      rebuilding (L).
      Nov 9: `compiler/egraph.fd`: e-classes, hash-consed e-nodes, union-find,
      rebuilding.
- [~] ME7. Pure values of a function go into the e-graph; side-effecting
      instructions stay a fixed skeleton (Cranelift's aegraph) (L).
      Pure integer and boolean values are e-nodes, everything else a leaf.
- [~] ME8. Extraction by cost, with each value placed at the latest dominating
      point that covers its uses: LICM and sinking fall out (L).
      Extraction by cost; an instruction is rewritten in place into its class's
      best form when that form's operands dominate it. Placement-based LICM is
      still to do.
- [x] ME9. Rewrite rules as data: a pattern, a replacement, conditions on types
      and constants; a file of rules per area (arithmetic, comparisons, bits,
      floats) (M).
      Nov 8: `compiler/rules.fd`, 27 rules as data.
- [x] ME10. Constant folding as rules, with the language's overflow semantics (a
      fold that would panic is not folded) (S).
      Folding under the language's checked arithmetic (a fold that would panic is
      not folded).
- [~] ME11. CSE and GVN from hash-consing, across blocks by dominance (M).
      Through the e-graph: an earlier equal value that dominates becomes a move.
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
- [~] ME15. Dead code and dead store elimination on SSA (M).
      `iro_dse`/`iro_dce_more` run on the current single-def IR; "on SSA"
      proper waits on ME3 phis.
- [x] ME16. Each rule tested on its own: random inputs evaluated before and after
      the rewrite, in `fid test-rules` (M).
      `fid test-rules`: each rule on edge and random values.
- [~] ME17. Each integer rule proved by an SMT solver on 64-bit vectors (an
      offline check, run in CI; z3 as a tool, not a dependency) (L).
      `fid test-rules --smt <dir>` writes 108 obligations (27 rules at 8, 16, 32,
      64 bits), `scripts/rules-smt.sh` runs them. Nov 9: z3 5.3.3 (the
      z3-solver wheel, through its Python module) proves all 108; a rule
      broken on purpose gives `sat`. Left: running it in CI.
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
- [~] ME21. Inlining decisions from the call graph bottom-up, with sizes known
      after the callee is optimized (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=inline2`), not run on the suite yet: post-order over
      the call graph: a callee is inlined into its callers after its own
      callees were.
- [~] ME22. Loop canonical form: preheaders, single latches, dedicated exits (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=loops`), not run on the suite yet: all three
      (`ime_canonical_loops`).
- [~] ME23. Induction variables: strength reduction, exit-value replacement,
      rotation shared by every backend (`PEIR_ROTATE` moves out of pe_ir) (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=ivsr`), not run on the suite yet: i * c in a
      loop becomes a second stepped variable, in @wrapping functions only
      (elsewhere the stepped copy could overflow where i * c is never
      computed). Exit values and rotation are not done.
- [~] ME24. Loop unrolling by a small factor for short bodies with known trip
      counts (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=unroll`), not run on the suite yet: a one-block loop
      of at most 8 iterations known on entry is unrolled fully (`ime_unroll`);
      partial unrolling is not done.
- [ ] ME25. Loop versioning: a fast copy without checks when a cheap test at the
      entry proves them unnecessary (M).
      Not started: the copy needs a way to tell the backends that its checks
      are proved (checks.fd decides them in pe_ir and the C generator).
- [~] ME26. Scalar replacement of aggregates on SSA: struct fields as separate
      values (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=sra`), not run on the suite yet: a struct local
      whose fields are only loaded and stored becomes a local per field
      (`ime_sra`).
- [ ] ME27. Small structs returned in registers (two words) for internal
      functions (M).
      Not started: a calling-convention change in the backends (pe_ir,
      nat_ir), not something the IR can say.
- [~] ME28. Switch lowering: ranges, bit tests, jump tables chosen in the mid-end
      (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=switch`), not run on the suite yet: a chain of four or
      more `x == k` tests on one value becomes a balanced tree of comparisons
      (`ime_switch`). Jump tables and bit tests need a new IR instruction.
- [x] ME29. Tail recursion into loops (S).
      `iro_tailcalls` (`compiler/iropt.fd`): a `return f(...)` self-call in
      tail position becomes argument stores + a jump back to the entry.
- [~] ME30. Every mid-end change measured on the suite: instruction count and
      compile time before and after, kept in a file per change (S).
      `scripts/me-measure.sh` exists; `FID_IROPT_STATS=1` reports
      instruction counts and per-pass timing. Files per change not kept yet.

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
- [x] WP7. `__env` dropped from functions that never read it, at every call (S).
      `iro_env_trim` (`compiler/iropt.fd`): removes `params[0]` and the
      matching `args[0]` at every direct `Inst::Call`. Address-taken
      (`ConstFnRef`), `Type_method` dyn targets, externs, C callbacks,
      runtime prefixes, `main`/`main_entry` keep the uniform convention, so
      `CallPtr` closures still pass an environment. Same pass marks
      `internal_conv` (WP6).
- [~] WP8. Leaf functions without frames, with no frame record where traces do not
      need it (S).
      `g.leaf` in nat_ir: a function with no calls, no callee-saved spills
      and an empty frame skips the prologue/epilogue — `ng_epilogue` emits
      only the architecture return on x64/a64/rv. MIR path only (the
      default); a64/rv runtime behaviour unverified (no qemu).
- [x] WP9. Effects as optimizer facts: a call to a function with no effects is a
      pure value in the e-graph (merged, hoisted, removed when unused) (M).
      `ru_pure_cse` (`compiler/rules.fd`): merged under dominance, never
      hoisted — calls may panic. Eligibility uses `ru_ext_effects`: local
      Alloca traffic and stores through unescaped Allocas are invisible, so
      `!read`-annotated callees with local writes still qualify. Call args key
      on constant *values* (each call emits its own ConstInt).
- [x] WP10. `!read`-only calls merged when no write lies between them (M).
      `ru_read_call`/`ru_read_reuse`: same-block and dominating-block merges
      with a backward CFG scan for barriers — any side-effecting inst except
      a Store to an unescaped caller Alloca (invisible to the callee).
      Verified: two adjacent `!read` calls merge; a `poke()` between them
      blocks the merge.
- [~] WP11. `@crashfree` functions: their panic paths are absent, so callers need
      no panic edges for them (S).
      Two uses. Optimizer: `ru_ext_effects` strips `Diverge` so `@crashfree`
      `!read` callees (e.g. stdlib `vec_code`) merge as read-only calls.
      Codegen: `ng_panic_if` elides recorded panic checks — opt-in via
      `FID_NAT_ON=crashfree`, because the emit path does not verify the
      annotation and a wrong one silently miscompiles (panicking program
      ran clean before the gate). No caller-side panic edges exist in this
      backend (panic aborts; no unwinding).
- [~] WP12. `@no-alloc` functions: no allocator state is live across them (S).
      `ru_ext_effects` strips `Alloc` under the same annotation-trust model
      as `@wrapping`, improving effect classification for merging. The
      literal item — keeping no allocator state live across the call — has
      no mechanism to attach to: the nat backend caches no allocator state
      in registers across calls.
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
      `spec 44` clones with identical program output.
- [x] WP15. Dead function and dead global elimination after optimization, not only
      before (S).
      `iropt_native` calls `IrModule_dead_function_elimination` after the pass
      loop. Seeds fixed for correctness: `c_callbacks` and `Type_method`
      targets of `DynCall` (found via `MakeDyn`) are kept — previously dropped,
      which miscompiled trait-object programs.
- [~] WP16. Devirtualization: `dyn` calls whose receiver type is known become
      direct calls; closures called at one site are inlined (M).
      `iro_devirt` (`compiler/iropt.fd`): a `DynCall` whose receiver's
      definition is `MakeDyn` becomes a direct `Call` to `Type_method` with
      the callee's real effects. One-site closure inlining not done (closure
      {fn,env} pairs are built at runtime, not traceable in the IR).
- [~] WP17. Global layout: hot functions together, cold panic paths at the end of
      the image (M).
      `ng_cold_last` (nat_ir): within each function, MIR segments ending in
      `MO_TRAP` — the panic paths — are moved to the end so the hot path
      stays compact. Function-local reorder only; program-level hot/cold
      function ordering across the image not done.
- [x] WP18. Globals that are never written after initialization become constants
      (S).
      `iro_const_globals` (`compiler/iropt.fd`): a global with an `IntLit`
      initializer whose address is only ever loaded — no stores through it,
      no escaping uses — has its integer loads folded to the initializer.
      Verified: ~295 loads folded per build on the suite, outputs unchanged.
- [~] WP19. Generics fully monomorphized with sharing of identical bodies (code
      folding by hash) (M).
      Identical functions (same MIR and allocation) are emitted once.
- [ ] WP20. Whole-program mode stays the default; a library build records the
      facts (clobbers, effects, sizes) its users need (M).

## Track LF — The language's guarantees as optimizer facts (25)

- [~] LF1. Alias facts from types: a `&mut` parameter aliases nothing else live;
      two `&mut` never alias (M).
      Nov 9 (`compiler/iropt_me.fd`, `ime_may_alias`): two different
      parameters never overlap (a function writes only through a `&mut`, and
      the borrow rules let nothing else reach that place); `*u8` parameters
      (strings, unsafe code) are left out, and parameters against globals may
      overlap. The IR does not mark `&mut` (`borrow_kind` is never set), so
      the rule rests on the borrow checker.
- [~] LF2. References are second-class (`escape.fd` proves it): the optimizer may
      assume a reference's target outlives every use and is never stored (M).
      Nov 9: the alias query relies on it (a parameter's place is reached
      only through that parameter and what is derived from it).
- [~] LF3. Owned values have one owner: a load through an owned `Vec`'s data
      pointer cannot be changed by a store through any other name (M).
      Nov 9: for locals: a local whose address goes only to loads, stores and
      field addresses overlaps nothing else, and no call can write it
      (`ime_unescaped`). A Vec's buffer is reached through calls to core, so
      its elements are not yet in the query.
- [~] LF4. Alias analysis as a query interface, answered from LF1–LF3, used by
      DSE, LICM of loads and store forwarding (M).
      Nov 9: `ime_may_alias(p, q)` and `ime_writes(inst, p)` (where an address
      comes from: a local, a parameter, a global, with its field path); used
      by LF5, LF6 and LF23. DSE (`iro_dse`, iropt.fd) does not use it yet.
- [~] LF5. Loads hoisted out of loops when the loop writes nothing that aliases
      them (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=loadhoist`), not run on the suite yet: a load of a
      local's or a parameter's field whose address is computed before the loop
      moves to the preheader when nothing in the loop may write it
      (`ime_hoist_loads`).
- [~] LF6. Store-to-load forwarding and redundant load removal (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=forward`), not run on the suite yet: in a block, a
      load of a place just stored or loaded, with no write that may overlap
      it between, becomes that value (`ime_forward`).
- [~] LF7. Bounds and overflow checks dropped on counted loops (release D12,
      `checks.fd`) (done for `while i < v.len()` and `for i in 0..n`).
- [~] LF8. Integer range analysis drops checks whose operands cannot overflow
      (`checks.fd`, `PEIR_RANGES`); extend it to every loop and branch (L).
      Nov 9: facts carried through jumps into single-entry blocks (`n - 1`
      after `if n < 2 { return }` is unchecked), and a Vec only filled with
      `Vec::filled(n, x)` and never shortened bounds its indexes (sieve).
      Not yet: facts merged at joins.
- [~] LF9. Ranges from conditions: inside `if i < n`, `i < n` is a fact (M).
      Holds in blocks with that branch as their only way in, and through
      jumps from them (`peir_range_refine`); not after a join.
- [ ] LF10. Ranges through function boundaries for internal functions (WP13) (M).
- [ ] LF11. Check hoisting: one check before a loop instead of one per iteration,
      when the loop is versioned (ME25) (M).
- [x] LF12. Narrow types as ranges: a `u8` value needs no overflow check on a
      widening add (S). `peir_type_range` on narrow loads and `IntExtend`.
- [~] LF13. Escape analysis: a `Vec`, `str` or struct that never leaves its function
      is allocated on the stack (L). Nov 9: a local Vec whose value only
      reaches core's Vec functions keeps its 40-byte header on the stack
      (`lw_rc_stack_vecs`, `LOWER_STACK_VECS`); not yet strings, structs or the
      buffer itself, or a Vec passed to the program's own functions.
- [ ] LF14. Scalar replacement of a non-escaping `Vec` of known small length (M).
- [~] LF15. Reference counts removed where ownership is known (proposal 0004): no
      `Vec_share` / drop pairs on values that are moved (M). Moves on last use
      (Nov 7), and in one block: Str and Box count/drop pairs, a Vec or map
      shared only to be read (`lw_rc_cancel_pairs`, `LOWER_CANCEL_SHARES`).
      Not yet across blocks.
- [ ] LF16. Copy-on-write checks removed where a value has one owner provably (M).
      Low gain now: the C and native fast paths test the capacity's sign once.
- [~] LF17. Drops of values known to be empty or static removed (S). String
      literals, and string variables only ever assigned literals
      (`LOWER_LITERAL_SLOTS`); an empty Vec still frees its header.
- [ ] LF18. `@deterministic` and `@constant_time` respected by every pass: no rule
      may add a branch on secret data (the verifier of release session 30 runs on
      every target) (M).
- [x] LF19. `@zeroize` stores kept by DSE (a pass may not remove them) (S).
      iropt's `dse` never touches `zeroize_locals`.
- [ ] LF20. `@secret` values never spilled to memory unencrypted where a register
      will do, and their spill slots cleared (M).
- [ ] LF21. Regions freed in one step instead of object by object (release C6) (M).
- [~] LF22. `comptime` results as constants the mid-end folds further (S).
      Nov 9: what `comptime` computes reaches the IR as constants already
      (the front end's comptime pass); fold, SCCP and branch facts fold
      further from them. No pass of its own is needed; a test that checks it
      is still to write.
- [~] LF23. Pattern matching facts: after a `match` arm, the scrutinee's variant is
      known, so later tag checks fold (M).
      Nov 9: written, opt-in (`FID_IROPT_ON=matchfacts`), not run on the suite yet: where a branch on
      `load(tag) == k` enters its arm, loads of that tag in the blocks the arm
      dominates become k when nothing there may write it (`ime_match_facts`).
- [ ] LF24. `Option` and `Result` in registers: tag and payload as two values, no
      memory for small payloads (M).
- [x] LF25. A document listing each language fact the optimizer uses, the rule that
      guarantees it, and the test that breaks if the rule is broken (S).
      docs/reference/optimizer-facts.md.

## Track SP — Compile speed (20)

- [x] SP1. No text between source and bytes: our own encoders and image writers,
      no assembler and no linker.
- [~] SP2. Per-stage timing for every build (`--time`), in one table: parse,
      check, lower, each pass, selection, allocation, encoding (S).
      `[prof] native mid-end` and `[prof] native` lines: hello world 0.4 s of
      mid-end, 82 ms of code generation, the front end over 3 s.
- [ ] SP3. `fid emit` of hello world under 0.5 s, then under 0.1 s (release D9):
      today the front end takes most of it (M).
- [ ] SP4. The checked stdlib cached in binary form, not re-parsed on every build
      (L).
- [ ] SP5. Code generation in parallel: functions handed to worker threads once
      the IR exists; the image assembled in order (M).
- [ ] SP6. A per-function code cache: key = hash of the function's IR, the target,
      the options and the clobber/effect facts of what it calls (M).
- [ ] SP7. Incremental relinking in memory: unchanged functions keep their bytes,
      only relocations are redone (M).
- [ ] SP8. A one-line edit rebuilds in under 100 ms (with SP4–SP7) (L).
- [ ] SP9. Front end in parallel per module where modules do not depend on each
      other (L).
- [ ] SP10. No linear scans anywhere in the compiler: every lookup by name or id is
      a table (release CS8) (M).
- [ ] SP11. Arena allocation for compiler data per function, freed in one step
      (M).
- [ ] SP12. Self-compile under 60 s, then under 10 s (M, then L).
- [ ] SP13. A debug mode with no mid-end and the fast allocator (RA14): 5x faster
      than release mode (S).
- [ ] SP14. The e-graph bounded so the release mode stays within 2x of debug (S).
- [ ] SP15. Compile time measured on every commit, with a limit per stage that
      fails CI when exceeded by 10% (M).
- [ ] SP16. The compiler itself built by our own backend and fast (bootstrap with
      `fid emit` instead of the C path) (L).
- [ ] SP17. Memory of a build under 1 GB for the compiler itself (M).
- [ ] SP18. A daemon mode (`fid serve`) that keeps the checked stdlib and caches
      warm between builds (M).
- [ ] SP19. `fid check` and the LSP share the daemon's state (M).
- [ ] SP20. Builds are reproducible byte for byte with the cache on and off (S).

## Track RL — Reliability: trusted without decades (25)

- [~] RL1. Differential testing against LLVM: random programs (release VF1/VF2)
      built by every backend and by the C path through clang must agree on output
      and exit status (M).
- [ ] RL2. The suite run on every backend and target on every change: Windows x64,
      Linux x64, AArch64 and RISC-V under QEMU, WASM (M).
- [~] RL3. A pass on/off matrix: the suite with each mid-end pass and each rule
      group disabled gives the same output (M).
      `scripts/pass-matrix.sh` (`NATIVE=1`). Nov 9, x86-64 Linux, 8 tests
      (integers, checked arithmetic, closures, generics, maps, match,
      floats, tasks), 6 iropt and 7 native switches: each one off gives
      the same results as all on. One run had a missing interpreter
      reference (exit 127, out of memory), not a difference. Left: the
      whole suite, and AArch64/RISC-V.
- [ ] RL4. A miscompile is fixed only with a test that fails without the fix
      (always) (S).
- [ ] RL5. Translation validation for the mid-end: for each function, a checker
      proves the optimized IR equals the input on random inputs (L).
- [ ] RL6. Translation validation for instruction selection: each selected
      sequence evaluated against its MIR operation on random values (M).
- [ ] RL7. The allocation checker (RA15) on every build in CI (S).
- [x] RL8. Encoder oracle tests (TG4) for every target in CI (S).
      TG4.
- [~] RL9. Image files checked by the system tools: `llvm-readobj`, `dumpbin`,
      `readelf`; every executable we write must load and pass them (S).
      `readelf` reads our ELF executables and their debug sections without a
      warning.
- [ ] RL10. Fuzzing of the IR -> machine code path with random IR (not only random
      source), so backend bugs show without front-end limits (M).
- [ ] RL11. Mutation testing of the backend: a deliberately broken rule or pass
      must fail a test (L).
- [ ] RL12. Line and branch coverage of `compiler/mir*.fd` and every target over
      90% from the suite and the fuzzers (M).
- [ ] RL13. Deterministic output: no hash-map order reaches code generation; the
      same input gives the same bytes on every host (M).
- [ ] RL14. The fixpoint built by our own backend: stage 2 == stage 3 through
      `fid emit` on every native host (L).
- [ ] RL15. Sanitizer runs of the compiler itself through the C path (ASan,
      UBSan), finding its own memory errors (M).
- [ ] RL16. Runtime checks of the runtime: `--checked-alloc` and `--leaks` on every
      native target, not only C (M).
- [ ] RL17. Concurrency testing of the runtime: thousands of task and thread
      interleavings under a scheduler that picks randomly (L).
- [ ] RL18. A public miscompile log: each bug, the pass, why the tests missed it,
      the test that now catches it (S).
- [x] RL19. Kill switches for every pass and rule group, documented, so a user can
      work around a miscompile on the day it is found (S).
      Every native pass and switch above can be turned off.
- [ ] RL20. Crash reports from the compiler with the function and pass that failed
      and a reduced reproducer (`fid reduce`) (M).
- [ ] RL21. A reducer: shrink a failing program automatically while it still fails
      the same way (M).
- [ ] RL22. Two independent paths kept on purpose: our backend and the C path. A
      release ships only when both agree on the suite (S).
- [ ] RL23. Signal-safe runtime paths reviewed: crash handlers allocate nothing,
      take no lock (the Linux runtime's `pe_on_signal` already) (S).
- [x] RL24. Stack overflow reported cleanly on every target: guard regions on
      every thread and task stack (done on native Linux and the C path) (S).
      Native Linux: a 64 KB guard region under every thread and task stack,
      `Panic: Stack overflow` (Nov 8, RT15's rule).
- [~] RL25. Every target's ABI tested against C: calls into C and callbacks from C
      (`qsort`) on every target (M).
      qsort and bsearch call Fides comparisons on the three Linux targets.

## Track DB — Debugging and visibility (10)

- [~] DB1. Line tables from the IR to MIR to bytes: every instruction knows its
      source line (M).
      Nov 9: the lowering's source lines (value metadata) become MO_LINE in the
      MIR and rows at emission; `.debug_line` (DWARF 4, one file) for the
      functions of the input file, on the three Linux targets (readelf decodes
      it). Left: rows for statements whose instructions carry no line
      metadata, the other files of a program (functions do not record theirs).
- [~] DB2. DWARF on Linux and macOS: line numbers, functions, frames (L).
      Nov 9: `.symtab`, DWARF 4 `.debug_info` (a compile unit, a subprogram per
      function) and `.debug_frame` in every native Linux executable; line tables
      since Nov 9 (DB1).
- [ ] DB3. PDB / CodeView on Windows (L).
- [ ] DB4. Locations of variables at `-O0`-like debug builds, in registers and
      slots (L).
- [x] DB5. Panic and crash traces from frame records, no per-call bookkeeping
      (the C path, native Linux).
- [~] DB6. Unwind information (Windows `.pdata`, DWARF CFI) so system debuggers and
      profilers walk our stacks (M).
      `.debug_frame` CFI for our prologues on the three Linux targets.
- [ ] DB7. `fid explain <function>`: the IR, the MIR after each stage, the machine
      code, side by side (M).
- [ ] DB8. Optimization remarks: why a check stayed, why a call was not inlined,
      why a value spilled (`--remarks`) (M).
- [ ] DB9. A sampling profiler on every native target that names our functions
      (from DB6) (M).
- [x] DB10. Symbol maps for every image we write, readable by `addr2line`-style
      tools (S).
      The symbol table (DB2).

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
