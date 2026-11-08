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
| Reference escape check (references are second-class) | `compiler/escape.fd` | a language rule, not yet used by the optimizer |

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

- [ ] MI1. Write down the MIR: instructions, operands (virtual register, physical
      register, immediate, stack slot, label), blocks, block parameters; in a new
      `compiler/mir.fd` (M).
- [ ] MI2. Target-neutral operations: integer and float arithmetic, compares,
      branch on compare, loads and stores of 1/2/4/8 bytes with extension,
      address arithmetic, calls, returns, traps (M).
- [ ] MI3. Target-specific operations as an open set per target (x64 `lea`,
      AArch64 `madd`, RISC-V `slli`+`add`), chosen by instruction selection (M).
- [ ] MI4. A target description: register classes, callee-saved set, argument
      and result registers, stack alignment, red zone, scratch registers (S).
- [ ] MI5. One lowering IR -> MIR for every language rule now written twice in
      `pe_ir.fd` and `nat_ir.fd`: checked add/sub/mul, narrowing, MIN / -1,
      division by zero, struct copies, closures, dyn calls, print routing (L).
- [ ] MI6. Panic paths as cold out-of-line blocks shared per function, each a
      call (so traces name the function), from the one lowering (M).
- [ ] MI7. One call lowering: arguments to registers and the outgoing area,
      `__env`, varargs spawns, results; parallel moves solved once (M).
- [ ] MI8. One frame layout: slots, spill slots, the save area, alignment, frame
      records for traces, large frames (M).
- [ ] MI9. Struct values as several MIR values when small (scalar replacement at
      the machine level), memory otherwise (M).
- [ ] MI10. A MIR verifier: every use defined, every block terminated, operand
      classes match the operation; run after each MIR stage in debug builds (M).
- [ ] MI11. A MIR printer and `fid dump-mir <file> --target <t>` (S).
- [ ] MI12. Port the AArch64 backend to MIR first (it is the smaller one); its
      suite results must not change (L).
- [ ] MI13. Port RISC-V to MIR: only instruction selection and its encoder stay
      target code (M).
- [ ] MI14. Port Windows x64 to MIR, keeping every optimization `pe_ir.fd` has
      (coalescing, rotation, xmm values, selection) as MIR passes or rules (XL).
- [ ] MI15. A Linux x86-64 target on MIR for free once x64 is ported: the ELF
      writer of `nat_enc.fd`, the Linux runtime, x64 selection (M).
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
- [ ] MI21. Constants: materialization per target (`movz/movk`, `lui/addi`,
      `mov imm64`) chosen by cost, rematerialized instead of spilled (M).
- [ ] MI22. Address modes as a selection decision: base + index * scale + offset
      where the target has it (M).
- [ ] MI23. Compare and branch fused in selection on every target (x64 `cmp/jcc`,
      AArch64 `cbz/tbz/b.cond`, RISC-V `beq/blt`) (S).
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
- [ ] RA2. Liveness on MIR from use lists, live ranges as lists of segments with
      holes, not hulls (M).
- [ ] RA3. All registers, caller-saved included: a value live across a call goes
      to a callee-saved register or is split around the call (L).
- [ ] RA4. Live-range splitting at calls and at loop boundaries, with spill code
      placed outside loops (L).
- [ ] RA5. Spill weights from loop depth and use counts (M).
- [ ] RA6. Copy coalescing on MIR (what `pe_ir`'s `PEIR_COALESCE` does), so
      `i = i + 1` is one instruction on every target (M).
- [ ] RA7. Register hints: arguments, results and call operands prefer the
      registers they must end in (M).
- [ ] RA8. Rematerialization of constants, addresses of globals and frame slots
      instead of spills (M).
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
- [ ] RA15. An allocation checker: replay the allocated code symbolically and
      prove every use reads the value it should (regalloc2's checker) (L).
- [ ] RA16. Run the checker on the whole suite in CI for every target (S).
- [ ] RA17. Statistics per function: spills, reloads, moves; `--stats` prints them
      (S).
- [ ] RA18. Floats and integers allocated by the same algorithm, class-aware (M).
- [ ] RA19. A value used only as a branch condition stays in flags where the
      target has them (M).
- [ ] RA20. The allocator honours IPRA clobber sets (Track WP) at each call (M).

## Track TG — Thin targets: selection and encoders (20)

- [ ] TG1. Instruction selection as rules: MIR patterns -> target instructions,
      tried by cost, written as data in Fides (an ISLE-like table, no new
      language) (L).
- [ ] TG2. Each rule carries its preconditions (immediate fits, register class)
      and is tested on its own by random operands against the generic lowering
      (M).
- [x] TG3. Own encoders, no assembler: x64 (`pe_ir_enc.fd`), AArch64 and RV64IMFD
      (`nat_enc.fd`).
- [ ] TG4. Encoder oracle tests: for every instruction form we emit, encode random
      operands and compare the bytes with clang's integrated assembler
      (`zig cc -c x.s`, `llvm-objdump`) (M).
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

- [ ] ME1. Use lists in the IR, kept current by every pass (M).
- [~] ME2. Dominator tree and loop tree shared by every pass (`iro_cfg`,
      `iro_dominates` in `iropt.fd` are a start) (M).
- [ ] ME3. mem2reg: locals whose address never escapes become SSA values with
      block parameters, on the IR every backend reads (L).
- [ ] ME4. An IR verifier after every pass in debug builds: types, dominance,
      terminators, block parameters (M).
- [ ] ME5. One pass manager: order, per-pass timing, a switch to turn each off
      (`--no-pass=name`), and `iro_pass_on` folded into it (M).
- [ ] ME6. An e-graph data structure: e-classes, hash-consed e-nodes, union-find,
      rebuilding (L).
- [ ] ME7. Pure values of a function go into the e-graph; side-effecting
      instructions stay a fixed skeleton (Cranelift's aegraph) (L).
- [ ] ME8. Extraction by cost, with each value placed at the latest dominating
      point that covers its uses: LICM and sinking fall out (L).
- [ ] ME9. Rewrite rules as data: a pattern, a replacement, conditions on types
      and constants; a file of rules per area (arithmetic, comparisons, bits,
      floats) (M).
- [ ] ME10. Constant folding as rules, with the language's overflow semantics (a
      fold that would panic is not folded) (S).
- [ ] ME11. CSE and GVN from hash-consing, across blocks by dominance (M).
- [ ] ME12. Algebraic simplification: identities, reassociation of constants,
      strength reduction of multiplication and division by constants (M).
- [ ] ME13. Comparison and branch simplification: `!(a < b)` to `a >= b`, known
      results from dominating conditions (M).
- [ ] ME14. Sparse conditional constant propagation over the CFG, dead branches
      removed (M).
- [ ] ME15. Dead code and dead store elimination on SSA (M).
- [ ] ME16. Each rule tested on its own: random inputs evaluated before and after
      the rewrite, in `fid test-rules` (M).
- [ ] ME17. Each integer rule proved by an SMT solver on 64-bit vectors (an
      offline check, run in CI; z3 as a tool, not a dependency) (L).
- [ ] ME18. Rule statistics: how often each fires on the suite; rules that never
      fire are deleted (S).
- [ ] ME19. Saturation limits per function (nodes, iterations), so compile time
      stays linear in practice (S).
- [~] ME20. Inlining with a cost model across modules (iropt inlines one-block
      functions of up to 16 instructions today) (L).
- [ ] ME21. Inlining decisions from the call graph bottom-up, with sizes known
      after the callee is optimized (M).
- [ ] ME22. Loop canonical form: preheaders, single latches, dedicated exits (M).
- [ ] ME23. Induction variables: strength reduction, exit-value replacement,
      rotation shared by every backend (`PEIR_ROTATE` moves out of pe_ir) (M).
- [ ] ME24. Loop unrolling by a small factor for short bodies with known trip
      counts (M).
- [ ] ME25. Loop versioning: a fast copy without checks when a cheap test at the
      entry proves them unnecessary (M).
- [ ] ME26. Scalar replacement of aggregates on SSA: struct fields as separate
      values (M).
- [ ] ME27. Small structs returned in registers (two words) for internal
      functions (M).
- [ ] ME28. Switch lowering: ranges, bit tests, jump tables chosen in the mid-end
      (M).
- [ ] ME29. Tail recursion into loops (S).
- [ ] ME30. Every mid-end change measured on the suite: instruction count and
      compile time before and after, kept in a file per change (S).

## Track WP — The whole program (20)

- [x] WP1. Every build sees the whole program; `nat_ir` compiles only what `main`
      reaches, from a worklist.
- [ ] WP2. A call graph with strongly connected components, built once per build
      (M).
- [ ] WP3. Compile bottom-up: callees before callers, each SCC together (M).
- [ ] WP4. IPRA: each function's clobbered registers recorded after allocation;
      a caller keeps values in caller-saved registers across calls that do not
      touch them (M).
- [ ] WP5. IPRA through indirect calls: a closure or `dyn` call clobbers the union
      of its possible targets when they are known, everything otherwise (M).
- [ ] WP6. Custom conventions for internal functions: arguments in the registers
      the caller already has them in, more than 8 register arguments, results in
      several registers (L).
- [ ] WP7. `__env` dropped from functions that never read it, at every call (S).
- [ ] WP8. Leaf functions without frames, with no frame record where traces do not
      need it (S).
- [ ] WP9. Effects as optimizer facts: a call to a function with no effects is a
      pure value in the e-graph (merged, hoisted, removed when unused) (M).
- [ ] WP10. `!read`-only calls merged when no write lies between them (M).
- [ ] WP11. `@crashfree` functions: their panic paths are absent, so callers need
      no panic edges for them (S).
- [ ] WP12. `@no-alloc` functions: no allocator state is live across them (S).
- [ ] WP13. Interprocedural constant propagation: a parameter always passed the
      same constant becomes that constant (M).
- [ ] WP14. Function specialization: a hot function cloned for a constant argument
      (M).
- [ ] WP15. Dead function and dead global elimination after optimization, not only
      before (S).
- [ ] WP16. Devirtualization: `dyn` calls whose receiver type is known become
      direct calls; closures called at one site are inlined (M).
- [ ] WP17. Global layout: hot functions together, cold panic paths at the end of
      the image (M).
- [ ] WP18. Globals that are never written after initialization become constants
      (S).
- [ ] WP19. Generics fully monomorphized with sharing of identical bodies (code
      folding by hash) (M).
- [ ] WP20. Whole-program mode stays the default; a library build records the
      facts (clobbers, effects, sizes) its users need (M).

## Track LF — The language's guarantees as optimizer facts (25)

- [ ] LF1. Alias facts from types: a `&mut` parameter aliases nothing else live;
      two `&mut` never alias (M).
- [~] LF2. References are second-class (`escape.fd` proves it): the optimizer may
      assume a reference's target outlives every use and is never stored (M).
- [ ] LF3. Owned values have one owner: a load through an owned `Vec`'s data
      pointer cannot be changed by a store through any other name (M).
- [ ] LF4. Alias analysis as a query interface, answered from LF1–LF3, used by
      DSE, LICM of loads and store forwarding (M).
- [ ] LF5. Loads hoisted out of loops when the loop writes nothing that aliases
      them (M).
- [ ] LF6. Store-to-load forwarding and redundant load removal (M).
- [~] LF7. Bounds and overflow checks dropped on counted loops (release D12,
      `checks.fd`) (done for `while i < v.len()` and `for i in 0..n`).
- [~] LF8. Integer range analysis drops checks whose operands cannot overflow
      (`checks.fd`, `PEIR_RANGES`); extend it to every loop and branch (L).
- [ ] LF9. Ranges from conditions: inside `if i < n`, `i < n` is a fact (M).
- [ ] LF10. Ranges through function boundaries for internal functions (WP13) (M).
- [ ] LF11. Check hoisting: one check before a loop instead of one per iteration,
      when the loop is versioned (ME25) (M).
- [ ] LF12. Narrow types as ranges: a `u8` value needs no overflow check on a
      widening add (S).
- [ ] LF13. Escape analysis: a `Vec`, `str` or struct that never leaves its function
      is allocated on the stack (L).
- [ ] LF14. Scalar replacement of a non-escaping `Vec` of known small length (M).
- [ ] LF15. Reference counts removed where ownership is known (proposal 0004): no
      `Vec_share` / drop pairs on values that are moved (M).
- [ ] LF16. Copy-on-write checks removed where a value has one owner provably (M).
- [ ] LF17. Drops of values known to be empty or static removed (S).
- [ ] LF18. `@deterministic` and `@constant_time` respected by every pass: no rule
      may add a branch on secret data (the verifier of release session 30 runs on
      every target) (M).
- [ ] LF19. `@zeroize` stores kept by DSE (a pass may not remove them) (S).
- [ ] LF20. `@secret` values never spilled to memory unencrypted where a register
      will do, and their spill slots cleared (M).
- [ ] LF21. Regions freed in one step instead of object by object (release C6) (M).
- [ ] LF22. `comptime` results as constants the mid-end folds further (S).
- [ ] LF23. Pattern matching facts: after a `match` arm, the scrutinee's variant is
      known, so later tag checks fold (M).
- [ ] LF24. `Option` and `Result` in registers: tag and payload as two values, no
      memory for small payloads (M).
- [ ] LF25. A document listing each language fact the optimizer uses, the rule that
      guarantees it, and the test that breaks if the rule is broken (S).

## Track SP — Compile speed (20)

- [x] SP1. No text between source and bytes: our own encoders and image writers,
      no assembler and no linker.
- [ ] SP2. Per-stage timing for every build (`--time`), in one table: parse,
      check, lower, each pass, selection, allocation, encoding (S).
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
- [ ] RL3. A pass on/off matrix: the suite with each mid-end pass and each rule
      group disabled gives the same output (M).
- [ ] RL4. A miscompile is fixed only with a test that fails without the fix
      (always) (S).
- [ ] RL5. Translation validation for the mid-end: for each function, a checker
      proves the optimized IR equals the input on random inputs (L).
- [ ] RL6. Translation validation for instruction selection: each selected
      sequence evaluated against its MIR operation on random values (M).
- [ ] RL7. The allocation checker (RA15) on every build in CI (S).
- [ ] RL8. Encoder oracle tests (TG4) for every target in CI (S).
- [ ] RL9. Image files checked by the system tools: `llvm-readobj`, `dumpbin`,
      `readelf`; every executable we write must load and pass them (S).
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
- [ ] RL19. Kill switches for every pass and rule group, documented, so a user can
      work around a miscompile on the day it is found (S).
- [ ] RL20. Crash reports from the compiler with the function and pass that failed
      and a reduced reproducer (`fid reduce`) (M).
- [ ] RL21. A reducer: shrink a failing program automatically while it still fails
      the same way (M).
- [ ] RL22. Two independent paths kept on purpose: our backend and the C path. A
      release ships only when both agree on the suite (S).
- [ ] RL23. Signal-safe runtime paths reviewed: crash handlers allocate nothing,
      take no lock (the Linux runtime's `pe_on_signal` already) (S).
- [ ] RL24. Stack overflow reported cleanly on every target: guard regions on
      every thread and task stack (done on native Linux and the C path) (S).
- [ ] RL25. Every target's ABI tested against C: calls into C and callbacks from C
      (`qsort`) on every target (M).

## Track DB — Debugging and visibility (10)

- [ ] DB1. Line tables from the IR to MIR to bytes: every instruction knows its
      source line (M).
- [ ] DB2. DWARF on Linux and macOS: line numbers, functions, frames (L).
- [ ] DB3. PDB / CodeView on Windows (L).
- [ ] DB4. Locations of variables at `-O0`-like debug builds, in registers and
      slots (L).
- [x] DB5. Panic and crash traces from frame records, no per-call bookkeeping
      (the C path, native Linux).
- [ ] DB6. Unwind information (Windows `.pdata`, DWARF CFI) so system debuggers and
      profilers walk our stacks (M).
- [ ] DB7. `fid explain <function>`: the IR, the MIR after each stage, the machine
      code, side by side (M).
- [ ] DB8. Optimization remarks: why a check stayed, why a call was not inlined,
      why a value spilled (`--remarks`) (M).
- [ ] DB9. A sampling profiler on every native target that names our functions
      (from DB6) (M).
- [ ] DB10. Symbol maps for every image we write, readable by `addr2line`-style
      tools (S).

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
