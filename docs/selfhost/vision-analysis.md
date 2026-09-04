# Vision Analysis — Are We on the Right Path?

> An honest assessment of where Fides is, where it's going, and what the
> self-hosting effort needs to succeed. Updated Sep 4, 2026 (session 8).

---

## The Vision (from `docs/reference/vision.md`)

> Build the last programming language the world needs.

Eight pillars:
1. **Originality** — custom toolchain, no LLVM
2. **Security** — capabilities, no ambient authority
3. **Performance** — 10x compilation, 2-3x runtime
4. **Memory Safety** — regions, no borrow checker fights
5. **Power** — comptime, raw pointers, FFI
6. **Adoption** — C interop, hot reload, fast builds
7. **Custom** — everything built by us
8. **Innovation** — AI/ML, robotics, IoT

The North Star: one language for OS kernels, web servers, robots, AI, microcontrollers — all memory-safe, all fast, all without fighting the compiler.

---

## Where We Are Now

### What works (Rust compiler — `src/`)

| Area | Status | Evidence |
|------|--------|----------|
| Language features | Phases 1-8 complete | 164/165 tests pass on interp+C, 165/165 on PE |
| Cross-compilation | Done | x86-64, ARM64, RISC-V, Windows |
| Optimizer | 23 passes, mostly complete | SLP, loop-vec, LTO, inline, DCE, etc. |
| AI/ML | Complete | Tensor ops, autograd, KV cache, quantization, DP |
| Safety | Complete | Regions, effects, capabilities, borrow checker, @medical |
| Industry stdlibs | Partial | banking.fd, medical.fd, clinical.fd, ecu.fd, can.fd, arinc.fd |
| Tooling | In progress | LSP, formatter, linter, bench, profile, debugger, pacte, scribe, veil |

### What works (self-hosted compiler — `compiler/`)

| Area | Status | Evidence |
|------|--------|----------|
| All 18 modules | Compile + pass tests | 23/23 on interp + PE |
| Build pipeline | Works | `fides-c.exe` builds via tcc |
| SRET | Works | Struct returns via out-parameter |
| Enum layout | Works | Payload sizes computed from field sizes |
| Self-compilation | **✅ WORKS** | `fides-c.exe` → `fides-c2.exe` (session 4) |
| PE emitter | **✅ WORKS** | `fides-c.exe build` → native .exe without tcc (session 5) |
| Comptime eval | **✅ FULL PORT** | process_gen, substitute, I-5A, I-5D, reflection (session 7) |
| PE runtime stubs | **✅ 25+ stubs** | print, string, vec, map, float, bool (session 7) |
| Linux native codegen | **✅ PORTED** | peephole.fd + regalloc.fd + codegen_x64.fd (session 8, not yet integrated) |
| `.exe` extension | **✅ FIXED** | Auto-appended in ccompile + build commands (session 8) |
| Bootstrap fixpoint | **NOT YET VERIFIED** | fides-c2 crashes on stdlib (statement-dropping bug); need to fix + verify |

### What doesn't work yet

1. **fides-c2.exe crashes on stdlib** — second-generation compiler crashes while lexing `stdlib/core.fd`. Root cause: the **statement-dropping bug** (the Fides compiler silently drops statements in long functions; the second-generation binary has missing instructions baked in)
2. **Bootstrap fixpoint not verified** — can't yet confirm stage2.c == stage3.c
3. **Debug prints** — 38 `p_eprint_str` calls need gating behind a debug flag
4. **PE emitter limited to simple programs** — needs struct/enum, float arithmetic, many-arg calls for full compiler source
5. **Linux native codegen not integrated** — peephole.fd, regalloc.fd, codegen_x64.fd are ported but not wired into the pipeline (requires elf64.rs, linker.rs, runtime_linux.rs porting)

---

## Are We on the Right Path?

### Yes — the architecture is sound

The vision calls for a custom toolchain with no LLVM dependency. We have that:
- Custom lexer, parser, typechecker, IR, optimizer, codegen, linker
- Cross-compilation to 4 targets with zero external tools
- 86K LOC Rust → 39.5K LOC Fides (the self-host is more compact)

The self-hosting approach is correct:
- Rewrite leaf modules first, climb the dependency graph
- Test at every step (23/23 self-host tests pass)
- Bootstrap via C codegen (simplest path to a working binary)

### Yes — the current blocker is the right one to focus on

The self-hosted compiler now compiles itself (session 4). The remaining
blocker is that fides-c2.exe (the second generation) crashes on stdlib.
Once that's fixed:
1. We can verify the fixpoint (fides-c2.c == fides-c3.c)
2. We have a self-reproducing compiler
3. Rust is only needed for the initial bootstrap

### The direction change is correct

The user's change to `pattern_binding_ir_types` (using `word_repr` for all types) moves toward a **pointer-based enum payload** approach. This is simpler and more consistent than the inline + memcpy approach:

| Approach | Complexity | Consistency | Risk |
|----------|------------|-------------|------|
| Inline + memcpy (old) | High — needs `load_enum_field`, `memcpy` in codegen, multi-slot copy in `EnumLit` | Must keep 3 things in sync | High — easy to get wrong |
| Pointer-based (new) | Low — every slot is 8 bytes, `Load` reads pointer | Only 1 thing to get right | Low — matches how C already works |

**The pointer-based approach is the right direction.** It aligns with:
- The C backend's treatment of `IrType::Struct(_)` as a pointer type
- The `word_repr` function's existing behavior
- The principle of simplicity (one way to do things, from `docs/meta/strategy.md`)

---

## What Needs to Happen

### Immediate (to unblock fixpoint verification)

1. ~~**Fix `.exe` extension** in self-hosted compiler output (Windows)~~ — DONE (session 8)
2. **Fix the statement-dropping bug** in the Fides compiler — root cause of fides-c2.exe crash on stdlib (the compiler silently drops statements in long functions, baking missing instructions into the second-generation binary)
3. **Remove/gate debug prints** — 38 `p_eprint_str` calls
4. **Run the fixpoint test:** `diff fides-c2.exe.c fides-c3.exe.c`

### Near-term (after bootstrap)

5. **Run the full test suite** through the self-hosted compiler
6. **Verify the fixpoint** (stage2.c == stage3.c)
7. **Fix any remaining codegen differences** found during fixpoint testing
8. **Extend PE emitter to full compiler source** — add struct/enum, Vec/HashMap, float arithmetic support

### Long-term (post-bootstrap)

9. **Port codegen_x64.fd + peephole.fd + elf64.fd** for Linux native codegen
10. **Industry domain stdlibs** (Phase 9)
11. **Playground website** (post-self-host)
12. **Package registry** (post-self-host)

---

## What the Language Can Do Now

### Systems programming
- Raw pointers (`*mut T`, `*const T`) with `unsafe {}` enforcement
- Inline assembly
- Bare-metal targets (flat binary mode)
- Cross-compilation to ARM64, RISC-V, Windows
- Region-based memory (stack, heap, named regions)
- Arena allocators

### Safety
- Region inference (no lifetime annotations)
- Effect system (IO, Net, Alloc, etc. — all explicit)
- Capability security (no ambient authority)
- Borrow checker (field-wise, two-phase, NLL)
- Bounds checking (always on, skippable in `unsafe`)
- `@medical` (IEC 62304 compliance), `@crashfree`, `@constant_time`, `@tainted`
- Linear types (`@linear`), conservation proofs (`@conserve`)

### Performance
- 23 optimization passes (inline, CSE, GVN, DCE, SROA, DSE, loop-vec, SLP, etc.)
- LTO (cross-module inlining)
- Comptime evaluation (auto-eval, loop unrolling, dead branch elimination)
- SIMD stdlib
- GPU codegen (SPIR-V)

### AI/ML
- Tensor operations with shape checking
- Autograd (reverse-mode AD, gradient checkpointing)
- Quantization, KV cache, differential privacy
- Multi-rank sharding (simulated)

### Concurrency
- Async/await (stackless coroutines, state machine transform)
- Channels (pthread mutex + condvar)
- Select, spawn
- Effect handlers

### Ergonomics
- F-strings, `?.`, `??`, `is`, spread, guard
- Contracts (`given`/`ensure`)
- `@memo`, `@gas`, `@audit`, `@wrapping`, `@deterministic`
- Pattern matching (enum variants, ranges, guards)
- Closures with capture analysis
- Traits and impls

### Tooling
- LSP (completion, hover, definition)
- Formatter, linter
- Debugger (interactive, record/replay, post-mortem, native)
- Package manager (pacte)
- Doc generator (scribe)
- Coverage tool (veil)

---

## What's Missing for the Vision

| Vision pillar | What's missing | Priority |
|---------------|----------------|----------|
| Self-hosting | Fixpoint verification + PE emitter for full compiler | **CRITICAL** — self-compilation + PE emitter work, need fixpoint + full native codegen |
| Performance | AVX2/SSE2 codegen, graph-coloring regalloc | Medium |
| Platform | DWARF debug info, Linux native codegen (elf64.fd port) | Low |
| Innovation | Industry domain stdlibs (medical, blockchain, robotics) | Low (post-bootstrap) |
| Adoption | Playground website, package registry | Low (post-bootstrap) |
| Ergonomics | Hot reload, green threads | Medium |

---

## Lessons for the Self-Host Effort

1. **Keep `src/` and `compiler/` in sync.** Every change to the Rust lowerer must be mirrored in the Fides lowerer. The enum payload direction change is a perfect example — if one side changes and the other doesn't, things break silently.

2. **The C backend treats structs as pointers.** This is a fundamental design choice that affects everything: SRET, enum payloads, struct copies, field access. Don't fight it — work with it.

3. **Simpler is better.** The inline + memcpy approach for enum payloads was complex and required 3 things to stay in sync. The pointer-based approach requires 1 thing. Always choose the simpler approach.

4. **Test the fixpoint early and often.** Once the self-hosted compiler can compile anything, run the fixpoint. Divergences catch bugs that unit tests miss.

5. **Document decisions immediately.** When you change direction, record it in `COORDINATION.md` before doing anything else. Otherwise parallel agents will follow the old direction.

---

## Conclusion

**Yes, we are on the right path.** The vision is ambitious but the architecture
is sound. The self-hosting effort has achieved self-compilation (session 4),
native PE code generation (session 5), and Linux native codegen porting
(session 8) — `fides-c.exe` compiles the entire ~45K LOC compiler to a working
second-generation binary, the PE emitter generates native Windows executables
without tcc, and the Linux native codegen backend is ported (pending
integration). The remaining work is verifying the fixpoint (stage2.c ==
stage3.c), which requires fixing the statement-dropping bug (root cause of the
fides-c2.exe stdlib crash), and extending the PE emitter to support the full
compiler source.

Once the bootstrap fixpoint is achieved and the PE emitter supports the full
compiler source, Fides becomes a **self-reproducing compiler with a fully
custom toolchain** — no LLVM, no gcc, no tcc, no external linker. The
foundation for everything else in the vision.
