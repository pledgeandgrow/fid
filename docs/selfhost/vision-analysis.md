# Vision Analysis — Are We on the Right Path?

> An honest assessment of where Fides is, where it's going, and what the
> self-hosting effort needs to succeed. Written Sep 3, 2026.

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
| Enum layout | Works (with caveat) | Payload sizes computed from field sizes |
| Bootstrap fixpoint | **NOT YET** | Blocked by enum variant extraction bug |

### What doesn't work yet

1. **Self-hosted compiler can't parse `stdlib/core.fd`** — enum variant extraction truncates struct payloads
2. **Bootstrap fixpoint not achieved** — can't verify the compiler reproduces itself
3. **PE emitter not rewritten in Fides** — deferred until C codegen bootstrap lands
4. **`box_expr_unbox` in `compiler/lower.fd` is out of sync** with `ast.fd`
5. **Fides lowerer `EnumLit` doesn't do multi-slot copy** — loses struct data on construction

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

The enum variant extraction bug is the **last** codegen bug between us and the bootstrap fixpoint. Once it's fixed:
1. The self-hosted compiler can parse `stdlib/core.fd`
2. It can compile `compiler/main.fd` to C
3. We can verify the fixpoint (stage2.c == stage3.c)
4. We have a self-reproducing compiler

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

### Immediate (to unblock bootstrap)

1. **Complete the pointer-based enum approach [D1]:**
   - `src/lower.rs` `compute_enum_layout`: 8 bytes per field (not struct size)
   - `src/lower.rs` `EnumLit`: store struct args as pointers (single slot)
   - `src/lower.rs` `load_enum_field`: remove or simplify
   - `compiler/lower.fd` `Lowerer_make_enum_layout`: 8 bytes per field
   - `compiler/lower.fd` `EnumLit`: store struct args as pointers
   - `compiler/lower.fd` match extraction: already uses `Load` — no change needed

2. **Rebuild and test:**
   - `cargo build --release`
   - `.\build-selfhost.ps1`
   - `.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test`
   - If that works: `.\release\fides-c.exe ccompile stdlib\core.fd`
   - If that works: run the bootstrap fixpoint

3. **Sync `box_expr_unbox`** in `compiler/lower.fd` with `ast.fd`

### Near-term (after bootstrap)

4. **Run the full test suite** through the self-hosted compiler
5. **Verify the fixpoint** (stage2.c == stage3.c)
6. **Fix any remaining codegen differences** found during fixpoint testing

### Long-term (post-bootstrap)

7. **Rewrite PE emitter in Fides** (Phase 10B)
8. **Industry domain stdlibs** (Phase 9)
9. **Playground website** (post-self-host)
10. **Package registry** (post-self-host)

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
| Self-hosting | Bootstrap fixpoint | **CRITICAL** — blocks everything |
| Performance | AVX2/SSE2 codegen, graph-coloring regalloc | Medium |
| Platform | DWARF debug info | Low |
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

**Yes, we are on the right path.** The vision is ambitious but the architecture is sound. The self-hosting effort is 95% complete — all 18 modules are rewritten and pass tests. The only remaining blocker is the enum variant extraction bug, which the pointer-based approach [D1] will fix cleanly.

Once the bootstrap fixpoint is achieved, Fides becomes a self-reproducing compiler — the foundation for everything else in the vision.
