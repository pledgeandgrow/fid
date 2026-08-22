# Fides Adoption Strategy

> Build it and they will come — only if switching is easy.

## The Adoption Problem

Every new language faces the chicken-and-egg problem:
- Users want ecosystems before they adopt
- Ecosystems form around users
- Nobody switches languages for marginal improvements

**Fides's strategy: target niches where Fides uniquely wins, then expand.**

---

## Phase 1: Niche Domination (Year 1-2)

### Niche 1: Embedded AI
- **Why Fides wins:** SIMD types + comptime shape verification + bare metal + memory safety
- **Why not Rust:** No SIMD first-class, compile times too slow for iteration
- **Why not Python:** Too slow, no bare metal, GC pauses
- **Target users:** AI researchers deploying models to edge devices
- **Killer feature:** Write AI inference code that runs on a microcontroller with memory safety

### Niche 2: Robotics
- **Why Fides wins:** Real-time effects + capability security + hot reload on running robots
- **Why not Rust:** Async doesn't support real-time well, no hot reload
- **Why not C:** No memory safety, no real-time guarantees
- **Target users:** Robotics startups, autonomous vehicle teams
- **Killer feature:** Hot-reload control code on a running robot without safety violations

### Niche 3: Safe Embedded Systems
- **Why Fides wins:** Memory safety + tiny binaries + deterministic memory + RISC-V support
- **Why not Rust:** Compile times too slow, borrow checker too rigid for embedded patterns
- **Why not C:** 70% of CVEs are memory safety issues
- **Target users:** IoT device manufacturers, medical device companies
- **Killer feature:** Memory-safe firmware with < 4KB binary size

> **Status update — what's WORKING now:**
> - ✅ **Bare-metal mode** — no-libc, no-OS targets are functional
> - ✅ **RISC-V target** — `codegen_riscv.rs` emits RISC-V assembly
> - ✅ **FixedBufferAllocator** — deterministic, no-allocation embedded memory management is operational (`src/embedded.rs`)
> - ✅ **Hot reload** — native JIT + function-level change detection works
> - ✅ **C interop** — C header parser → Fides FFI bindings (`src/cparser.rs`) is functional
> - ✅ **C codegen backend** — 101/127 test parity (near full parity with interpreter — test_phase8b fails: ECS __ecs_* not in C codegen)
>
> These niches are no longer aspirational — the foundational capabilities are built and running.

---

## Phase 2: Expansion (Year 2-3)

### Bridge: Systems Programming
- **Why Fides wins:** Memory safety without borrow checker pain + fast compilation + hot reload
- **Migration path:** C interop + C-to-Fides transpiler
- **Target users:** C developers tired of CVEs, Rust developers tired of compile times
- **Killer feature:** Gradually migrate C codebase to memory safety, one file at a time

### Bridge: Web Backends
- **Why Fides wins:** Effect handlers for async I/O + hot reload + fast compilation
- **Migration path:** HTTP server stdlib + JSON serialization (comptime)
- **Target users:** Startups wanting fast iteration + safety
- **Killer feature:** Hot-reload a running web server, see changes instantly

### Bridge: CLI Tools
- **Why Fides wins:** Fast compilation + single binary output + no runtime
- **Migration path:** Simple syntax, familiar to Rust/Go developers
- **Target users:** Go/Rust CLI tool developers
- **Killer feature:** Sub-second compile + hot reload during development

---

## Phase 3: Mainstream (Year 3-5)

### Bridge: Game Development
- **Why Fides wins:** Hot reload + SIMD + memory safety + fast compilation
- **Target users:** Game studios, indie developers
- **Killer feature:** Change game logic while the game is running

### Bridge: OS Development
- **Why Fides wins:** Bare metal + memory safety + inline assembly + capability security
- **Target users:** OS researchers, security-focused OS projects
- **Killer feature:** Write a memory-safe OS kernel with hot-reloadable drivers

### Bridge: Security-Critical Software
- **Why Fides wins:** Capability security + memory safety + effect system (auditable)
- **Target users:** Cryptography libraries, security tools, government software
- **Killer feature:** Prove no code can access the filesystem without explicit capability

---

## Migration Tools — Making Switching Easy

> **Note:** C-to-Fides and Rust-to-Fides transpilers are nice-to-have community
> goals, not on the critical path. See `docs/extra.md` for honest
> feasibility assessment. The FFI generator below is the only migration
> tool that exists today.

### FFI Generator (Working now)
```
// Input: C header (openssl.h)
SSL_CTX* SSL_CTX_new(const SSL_METHOD* method);

// Output: Fides FFI binding
extern "C" pf SSL_CTX_new(method: *const SSL_METHOD) -> *mut SSL_CTX;
```
- Parse C headers automatically
- Generate Fides FFI bindings
- No hand-written bindings needed
- ✅ **Working now** — `fidesc parse-header <file.h>` parses C headers and emits Fides FFI bindings (`src/cparser.rs`)

### C-to-Fides Transpiler (Nice-to-have — see `docs/extra.md`)
- Auto-translate C code to Fides code
- Feasible but moderate effort (real C has macros, goto, union punning)
- FFI already covers the interop case; transpiler would help porting logic

### Rust-to-Fides Transpiler (Aspirational — see `docs/extra.md`)
- Convert Rust code to Fides code
- Very high effort — lifetimes, macros, async, trait system don't map cleanly
- Practical alternative: use FFI via C ABI, port files manually

---

## Developer Experience — The Secret Weapon

### Sub-Second Compilation
```
$ fidesc build
Built in 0.3s   ← not 30 minutes
```

### Hot Reload
```
$ fidesc watch
Running... (PID 12345)
File changed: src/main.fd
Reloaded in 45ms   ← not a full rebuild
```

### Clear Error Messages
```
Error: use-after-free detected at compile time
  → src/main.fd:12:5
   |
12 |   print(buf[0]);
   |         ^^^
   |         buf was freed at line 10
   |
   = region r1 was freed at line 10
   = buf was allocated in region r1 at line 5
   = help: move the free_region call after the last use of buf
```

### AI-Assisted Development
```
$ fidesc ai "refactor this function to use effect handlers"
Analyzing...
Proposed change:
  - Replace async/await with effect handler
  - Add !IO effect annotation
  - Remove Send bound (not needed with effects)
Apply? [y/n]
```

---

## Community Building

### Open Source Strategy
- **MIT/Apache license** — permissive, no GPL contamination
- **GitHub** — where developers already are
- **Good first issues** — labeled for newcomers
- **Architecture docs** — help contributors understand the codebase

### Documentation Strategy
- **Fides Book** — free online book (like "The Rust Programming Language")
- **Interactive Tutorial** — learn by doing, in the browser
- **Examples Library** — real-world code for every use case
- **Playground** — try Fides without installing anything

### Conference Strategy
- **Talks at systems conferences** — OSDI, SOSP, PLDI
- **Talks at AI conferences** — NeurIPS, ICML (for the AI/ML angle)
- **Talks at embedded conferences** — Embedded World, IoT World
- **Blog posts** — technical deep dives, benchmarks, case studies

---

## Success Metrics

| Metric | Year 1 | Year 2 | Year 3 | Year 5 |
|---|---|---|---|---|
| GitHub stars | 1K | 10K | 50K | 100K |
| Active developers | 50 | 500 | 2K | 10K |
| Production users | 1 | 10 | 50 | 500 |
| Packages | 10 | 100 | 1K | 10K |
| Conference talks | 1 | 5 | 20 | 50 |
| University courses | 0 | 1 | 5 | 20 |

---

## The Adoption Funnel

```
Awareness (people hear about Fides)
    ↓ 10% convert
Interest (people try the playground)
    ↓ 20% convert
Trial (people install fidesc)
    ↓ 30% convert
Adoption (people use Fides for a project)
    ↓ 50% convert
Advocacy (people recommend Fides to others)
```

**Key conversion points:**
1. **Awareness → Interest:** Killer demos (hot reload on a robot, AI on a microcontroller)
2. **Interest → Trial:** Playground + one-command install
3. **Trial → Adoption:** Fast compilation + hot reload (instant gratification)
4. **Adoption → Advocacy:** Safety + performance (results speak for themselves)
