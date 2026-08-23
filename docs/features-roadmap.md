# Fides Features Roadmap — New Language Features

> This roadmap tracks **new features** added to Fides — things that don't
> exist in any other language and leverage Fides's unique combination of
> capabilities + effects + regions + comptime + custom toolchain.
>
> Separate from `docs/roadmap.md` which tracks the core compiler phases
> (1–11). This file is about *differentiating features*.

---

## Design Philosophy: Syntax vs Stdlib

Fides follows the Rust/Zig philosophy: **small language, rich stdlib, infinite extensibility.**

Not every useful feature should be a language keyword. The test is simple:

> **Can a user build this in Fides itself? If yes, it belongs in the stdlib. If no, it belongs in the compiler.**

### Why This Matters

| Syntax (compiler) | Stdlib (Fides code) |
|-------------------|-----------------|
| Parser + typechecker + lowering + codegen needed | Just a `.fd` file — no compiler changes |
| Users **cannot** modify how it works | Users **can** read, modify, extend, replace |
| Works with zero imports | Requires `use` import |
| Compiler can optimize across it | Opaque to compiler (just function calls) |
| Adds a keyword — language grows | No keyword — language stays small |
| Every user pays the learning cost | Only users who need it pay the cost |
| One implementation for everyone | Multiple competing implementations possible |

### The Rule

```
SYNTAX  = things the COMPILER must understand (can't be a library)
STDLIB  = things USERS can build themselves (just .fd files)
CUT     = things EXISTING features already cover (redundant)
```

Only make it syntax when the compiler must do something a library can't:
- **Instruction emission** (SIMD, GPU codegen)
- **Call graph analysis** (crash safety, contracts)
- **Type narrowing** (optional chaining, type checks)
- **Memory layout control** (ECS SoA, object pools)
- **Thread management** (parallel fork-join)
- **IR transformation** (autograd tracing)

Everything else — tilemaps, sprites, flowfields, streaming, scenes — is stdlib.

---

## Status

```
─── Implemented (F1–F5) ──────────────────────────────────────────
F1: sandbox { }              ✅ IMPLEMENTED — typechecker enforces, zero runtime cost
F2: persist variables        ✅ IMPLEMENTED — interpreter + IR + lowerer + PIR
F3: Memory budgets           ✅ IMPLEMENTED — compile-time check, zero runtime cost
F4: watch --diff             ✅ IMPLEMENTED — PIR L2 line diff + binary size delta
F5: Capability-aware linking ✅ IMPLEMENTED --caps flag, strips ungranted functions

─── Language Syntax — Tier A: Compiler Must Understand ───────────
F16: vec2/3/4 + mat4 types    🔲 PLANNED — SIMD instruction emission needs compiler
F17: Vector swizzle (.xyz)    🔲 PLANNED — parser + typechecker for component access
F8:  ?. optional chaining     ✅ IMPLEMENTED — null-safe navigation, None short-circuits to 0
F10: given/ensure contracts   ✅ IMPLEMENTED — preconditions verified at call sites with constant args (Design by Contract)
F18: ECS (component/system)   ✅ IMPLEMENTED — component/system/query desugar to structs/functions/loops, __ecs_* runtime in interpreter
F13: parallel { | } fork-join 🔲 PLANNED — compiler emits thread spawn/join
F14: @differentiable          🔲 PLANNED — compiler traces IR for autograd
F21: @shader (vert/frag)      🔲 PLANNED — compiler emits SPIR-V for GPU
F23: @simd                    🔲 PLANNED — compiler vectorizes loops to SSE/AVX/NEON
F24: @crashfree               ✅ IMPLEMENTED — compile-time panic prevention: rejects unwrap, p_panic, non-const div/mod, while loops, non-crashfree calls
F25: @pool                    🔲 PLANNED — compiler manages pool memory layout, zero alloc
F26: @deterministic           ✅ IMPLEMENTED — compiler rejects FP/random/IO/time (medical+blockchain)
F27: @realtime                🔲 PLANNED — compiler analyzes WCET, rejects malloc (robotics)
F28: @gas                     ✅ IMPLEMENTED — compile-time gas budgets, AST node cost model, transitive call graph analysis (blockchain)
F29: @interrupt               🔲 PLANNED — compiler generates ISR code, vector table (robotics+IoT)
F30: @firmware                🔲 PLANNED — compiler strips OS deps, bare metal startup (IoT)
F31: @lowpower                🔲 PLANNED — compiler inserts sleep, gates clocks (IoT)
F32: @serverless              🔲 PLANNED — compiler tree-shakes, pre-init, WASM output (serverless)

─── Language Syntax — Tier B: Ergonomic (worth the keyword) ──────
F6:  guard =>                 ✅ IMPLEMENTED — desugars to if !cond { action }, works in all backends
F7:  .. spread operator       ✅ IMPLEMENTED — struct field copy + override, works in all backends
F9:  ?? coalescing            ✅ IMPLEMENTED — Option ?? default, works in all backends
F11: is type check            ✅ IMPLEMENTED — runtime enum variant check + compile-time type check
F12: @memo annotation         ✅ IMPLEMENTED (as stdlib) — memoization via stdlib/memo.fd (Memo::new, get_or_compute), not a compiler annotation
F20: on event handlers        ✅ IMPLEMENTED — `on event_name(params) { body }` desugars to `__on_<event_name>` function
F19: state machines           ✅ IMPLEMENTED — `state_machine` desugars to enum + initial/handle functions, transition table
F15: @no_grad scope           ❌ CUT — unnecessary in compile-time opt-in autograd (grad blocks are opt-in, nothing to disable)
F33: @audit                   ✅ IMPLEMENTED — entry/exit logging with parameter values (IR injection), type-aware printing
F34: @emit                    🔲 PLANNED — blockchain event emission with signature verification
F35: @watchdog                🔲 PLANNED — compiler auto-inserts watchdog kicks (robotics+IoT)
F36: @handler                 🔲 PLANNED — auto-generates routing + OpenAPI docs (serverless)
F37: @ota                     🔲 PLANNED — compiler generates binary diff for OTA updates (IoT)

─── Banking & Finance — Tier A: Compile-Time Financial Safety ──
F38: money type               ✅ IMPLEMENTED — fixed-point i64 cents, @linear struct, no FP, no overflow
F39: @conserve                ✅ IMPLEMENTED — compiler proves money in = money out, no mint/IO/float inside @conserve functions
F40: txn { } blocks           ✅ IMPLEMENTED — parsing + typechecking + interpreter rollback (memory snapshot on TxnBegin, restore on panic, commit on TxnEnd)
F41: linear money             ✅ IMPLEMENTED — @linear annotation on structs, move semantics, no copies, loop-aware (can't consume in loops)
F42: Account<State> types     🔲 PLANNED — state-encoded account types (Active/Frozen/Closed can't be mixed)
F43: LogicalTime type         ✅ IMPLEMENTED — stdlib/time.fd: timezone-free, clock-free, @deterministic date math (Howard Hinnant civil algorithm)

─── Production Primitives — Tier B: Ergonomic ───────────────────
F61: tuple destructuring       ✅ IMPLEMENTED — `pv (a, b) = expr` multi-return unpacking
F62: file I/O                  ✅ IMPLEMENTED — stdlib/file.fd: read/write/append/exists
F63: loop-aware linear types   ✅ IMPLEMENTED — linear vars can't be consumed inside loops
F95: raw pointer types         ✅ IMPLEMENTED — *mut T / *const T with unsafe {} blocks, typechecker enforces unsafe_depth, &T coerces to *mut T

─── Cross-Industry Safety — Tier A: Compiler-Verified Domain Guarantees ──
F44: @safety-critical         ✅ IMPLEMENTED — composable safety primitives: @no-alloc + @no-recursion + @no-unbounded + @no-panic + @no-indirect (DO-178C/ISO 26262)
F45: @redundant               🔲 PLANNED — compiler generates dual-channel redundant execution + comparison (aerospace)
F46: @bounded                 🔲 PLANNED — compiler proves all loops terminate, no unbounded iteration (safety-critical)
F47: @partitioned             🔲 PLANNED — compiler enforces spatial+temporal partitioning between components (avionics)
F48: @medical                 ✅ IMPLEMENTED — IEC 62304 rules: no dynamic alloc, no recursion, no while, no unwrap, must be @audit, transitive
F49: @isolated                🔲 PLANNED — compiler proves no shared mutable state between components (multi-core safety)
F50: @signed                  🔲 PLANNED — compiler requires cryptographic signatures on all state transitions (blockchain+medical)
F51: @reproducible            🔲 PLANNED — compiler verifies bit-identical output across runs/builds (regulatory+science)

─── Automotive — Tier A: Compiler-Verified Vehicle Safety ───────
F52: @autosar                 ✅ IMPLEMENTED — composable safety primitives: @no-alloc + @no-recursion + @no-unbounded + @no-panic (AUTOSAR coding guidelines)
F53: @can-bus                 🔲 PLANNED — compiler generates CAN frame encoding/decoding with compile-time ID verification
F54: @diagnostic              🔲 PLANNED — compiler auto-generates UDS/OBD diagnostic service handlers (automotive)
F55: @calibratable            🔲 PLANNED — compiler marks tunable parameters, generates calibration data format (automotive)

─── Aerospace & Defense — Tier A: Compiler-Verified Flight Safety ──
F56: @dal-a                   ✅ IMPLEMENTED — composable safety primitives: @no-alloc + @no-recursion + @no-unbounded + @no-panic + @requires-audit (DAL-A aerospace)
F57: @arinc                   🔲 PLANNED — compiler generates ARINC 653 partition scheduling + port communication (avionics)
F58: @bit-exact               🔲 PLANNED — compiler guarantees bit-identical FP results across hardware (flight control)

─── Energy & Grid — Tier A: Compiler-Verified Grid Safety ───────
F59: @grid-safe               🔲 PLANNED — compiler enforces IEC 61850 rules: bounded response, no blocking, deterministic dispatch
F60: @scada                   🔲 PLANNED — compiler generates SCADA protocol stubs with compile-time point verification (energy)

─── Cybersecurity — Tier A: Compiler-Verified Exploit Prevention ──
F61: @no-alias                🔲 PLANNED — compiler proves no two mutable refs to same data (memory safety)
F62: bounds checking lock     ✅ IMPLEMENTED — bounds checks ALWAYS fire in safe mode, skippable in unsafe {} (memory safety)
F63: init guarantee           ✅ IMPLEMENTED — `pm` keyword for uninitialized mutable vars, definite assignment analysis, compile error on read before assignment (memory safety)
F64: checked arithmetic lock  ✅ IMPLEMENTED — overflow = panic (not wrap), always on, @wrapping opt-out (integer safety)
F65: @wrapping                ✅ IMPLEMENTED — explicit opt-in for wrap-around arithmetic (hashing, crypto)
F67: typed strings            ✅ IMPLEMENTED — SQLString, ShellString, HTMLString, JSONString, PathString distinct types, injection prevention via type system
F68: parameterized queries    ✅ IMPLEMENTED — no string concatenation for typed strings (SQLString/ShellString/etc), use bind/escape methods (injection prevention)
F69: @escaped                 🔲 PLANNED — compiler verifies output is escaped before rendering (XSS prevention)
F70: @atomic-check            🔲 PLANNED — check and act are one atomic operation (TOCTOU prevention)
F72: @constant-time           🔲 PLANNED — compiler verifies no data-dependent branches or memory access (side-channel)
F74: @secret type             🔲 PLANNED — information flow control, secrets can't be printed/logged/sent (info leak)
F75: @cfi                     🔲 PLANNED — control flow integrity: shadow stack, CFI checks, canaries (ROP prevention)
F76: vtable protection        🔲 PLANNED — vtables read-only, pointer-signed, no user modification (JOP prevention)
F79: @pinned                  🔲 PLANNED — dependency pinned with content hash, tamper detection (supply chain)
F80: @sandbox-import          🔲 PLANNED — imported modules sandboxed by default, zero capabilities (supply chain)
F82: no auto-deserialize      🔲 PLANNED — all parsing explicit + schema-validated, no reflection-based creation (deserialization)
F85: @thread-safe             🔲 PLANNED — compiler proves no data races, message passing only (race conditions)
F88: @zeroize                 🔲 PLANNED — secrets wiped from memory on scope exit (info leak prevention)
F90: @no-leak                 🔲 PLANNED — compiler verifies no secret data in error messages or logs (info leak)
F91: @crypto-approved         🔲 PLANNED — only vetted algorithms (no MD5, SHA1, custom crypto) (crypto weakness)
F92: @nonce-unique            🔲 PLANNED — compiler verifies nonce never reused (crypto weakness)
F94: @least-privilege         🔲 PLANNED — compiler verifies minimum capabilities per function (privilege escalation)

─── Cybersecurity — Tier B: Ergonomic Security Annotations ─────
F66: money overflow protection🔲 PLANNED — money type overflow = transaction abort, never silent (integer safety)
F71: FS race protection       🔲 PLANNED — FS operations capability-gated + atomic (TOCTOU prevention)
F73: @no-branch-on-secret     🔲 PLANNED — reject branches based on secret-typed data (side-channel)
F77: stack canaries           🔲 PLANNED — codegen inserts canaries on all function frames (stack overflow)
F78: shadow stack             🔲 PLANNED — hardware shadow stack where supported (ARM64, x86-64 CET)
F81: @audited-dependency      🔲 PLANNED — dependency must have @audit trail (supply chain)
F83: @schema-validated        🔲 PLANNED — compiler verifies parser against schema (deserialization)
F84: no runtime reflection    🔲 PLANNED — reflection is comptime only, no runtime type instantiation (deserialization)
F86: message passing only     🔲 PLANNED — no shared mutable state between threads (race conditions)
F87: @race-free               🔲 PLANNED — compiler verifies all shared access is synchronized (race conditions)
F89: @csprng                  🔲 PLANNED — only cryptographically secure random in crypto contexts (crypto weakness)
F93: @capability-scope        🔲 PLANNED — capabilities can't be escalated at runtime (privilege escalation)

─── Stdlib Modules — Built in Fides, Shipped with Compiler ──────────
S1: stdlib/tilemap.fd          ✅ DONE — tile maps, collision, auto-tiling (gaming)
S2: stdlib/sprite.fd           ✅ DONE — animation, atlases, palette swap (gaming)
S3: stdlib/flowfield.fd        ✅ DONE — RTS pathfinding, vector fields (gaming)
S4: stdlib/streaming.fd        ✅ DONE — chunk load/unload for open worlds (gaming)
S5: stdlib/scene.fd            ✅ DONE — declarative entity spawning (was F22)
S6: stdlib/physics.fd          ✅ DONE — collision detection, rigid bodies (gaming+robotics)
S7: stdlib/audio.fd            ✅ DONE — audio playback via FFI (gaming, stub — no actual audio hardware)
S8: stdlib/input.fd            ✅ DONE — input handling on top of `on` events (gaming, stub — no actual input hardware)
S9: stdlib/crypto.fd           🔲 PLANNED — hashing, signatures, encryption (blockchain+medical)
S10: stdlib/consensus.fd       🔲 PLANNED — PoW, PoS, PBFT, Raft (blockchain)
S11: stdlib/storage.fd         🔲 PLANNED — Merkle trees, KV storage, state roots (blockchain)
S12: stdlib/hal.fd             🔲 PLANNED — GPIO, I2C, SPI, UART, PWM (robotics+IoT)
S13: stdlib/sensor.fd          🔲 PLANNED — IMU, temperature, distance, camera (robotics+IoT)
S14: stdlib/motor.fd           🔲 PLANNED — PWM, encoder feedback, PID (robotics)
S15: stdlib/protocol.fd        🔲 PLANNED — MQTT, CoAP, BLE, LoRaWAN (IoT)
S16: stdlib/ota.fd             🔲 PLANNED — OTA protocol: download, verify, apply, rollback (IoT)
S17: stdlib/http_server.fd     🔲 PLANNED — HTTP server, routing, middleware, TLS (serverless+web)
S18: stdlib/wasm_api.fd        🔲 PLANNED — WASM browser bindings: DOM, fetch, localStorage (serverless+web)
S19: stdlib/banking.fd         🔲 PLANNED — core banking: accounts, ledger, transfers, interest, clearing (finance)
S20: stdlib/swift.fd           🔲 PLANNED — SWIFT message format (MT/MX), BIC lookup, transaction routing (finance)
S21: stdlib/payments.fd        🔲 PLANNED — payment rails: ACH, SEPA, FedNow, UPI, Pix adapters (finance)
S22: stdlib/can.fd             🔲 PLANNED — CAN bus frame encoding, J1939, OBD-II, UDS diagnostics (automotive)
S23: stdlib/ecu.fd             🔲 PLANNED — ECU abstractions: sensors, actuators, control loops, calibration (automotive)
S24: stdlib/arinc.fd           🔲 PLANNED — ARINC 653 partitions, ports, health monitoring (aerospace)
S25: stdlib/ada.fd             🔲 PLANNED — Ada-style safety patterns: ranged types, invariant checking (aerospace+defense)
S26: stdlib/grid.fd            🔲 PLANNED — IEC 61850 data models, GOOSE/SV/MMS protocols (energy)
S27: stdlib/scada.fd           🔲 PLANNED — SCADA: Modbus, DNP3, IEC 60870-5-104, point database (energy)
S28: stdlib/medical.fd         🔲 PLANNED — HL7/FHIR parsing, DICOM imaging, ICD coding, FDA audit trails (medical)
S29: stdlib/clinical.fd        🔲 PLANNED — clinical calculations: drug dosage, BMI, GFR, risk scores (medical)

─── Stdlib Modules — Implemented (Phase 11) ─────────────────────
S30: stdlib/math.fd            ✅ IMPLEMENTED — sqrt, pow, floor, ceil, sin, cos, fabs, constants (pi, e)
S31: stdlib/env.fd             ✅ IMPLEMENTED — env_get, env_exists (environment variable access)
S32: stdlib/args.fd            ✅ IMPLEMENTED — args_len, args_get, args_program_name (command-line args)
S33: stdlib/process.fd         ✅ IMPLEMENTED — process_exit, process_system (process control)
S34: stdlib/regex.fd           ✅ IMPLEMENTED — regex_matches (?/* wildcards), regex_find, regex_contains, regex_starts_with, regex_ends_with

─── Cut — Redundant, Build with Existing Features ───────────────
C1: fogofwar                  ❌ CUT — build with tilemap (S1) + query (F18)
C2: selection                 ❌ CUT — build with query (F18) + formation math
C3: traffic                   ❌ CUT — build with ECS (F18) + flowfield (S3) + state (F19)
C4: lod                       ❌ CUT — build with distance check + model swap
C5: snapshot                  ❌ CUT — merge into persist (F2): persist every 30s { }
C6: scene { } keyword         ❌ CUT — moved to stdlib/scene.fd (S5)
C7: @hipaa                    ❌ CUT — just a capability tag (HealthData), use existing cap system
C8: @contract                 ❌ CUT — compose sandbox(F1) + @deterministic(F26) + @gas(F28) + persist(F2)
C9: @view                     ❌ CUT — just !{} pure effect, already exists
C10: @constructor             ❌ CUT — just a function called from init, no keyword needed
C11: @coldstart               ❌ CUT — merge into @realtime(phase=init, deadline=10ms)
C12: @payable                 ❌ CUT — just a function with a value parameter
C13: @warm                    ❌ CUT — runtime concern (keep-alive ping), not compile-time
C14: mesh networking          ❌ CUT — just a protocol, build with stdlib/protocol.fd (S15)
C15: @validated               ❌ CUT — already covered by given/ensure contracts (F10)
```

### Classification Summary

| Tier | Count | What | Why |
|------|-------|------|-----|
| **Implemented** | 30+ | F1–F11, F18–F20, F24, F26, F28, F33, F38–F41, F43, F44, F48, F52, F56, F61–F65, F67–F68, F95 | Done and tested (interpreter + C codegen where applicable) |
| **Syntax Tier A** | 39 | F10, F13, F14, F16–F18, F21, F23–F60 | Compiler must understand — can't be a library |
| **Syntax Tier B** | 12 | F6, F7, F9, F11, F12, F19, F20, F33–F37 | Ergonomic — could be library but worth the keyword |
| **Stdlib** | 29 | S1–S29 | Built in Fides, users can read/modify/replace |
| **Cut** | 16 | C1–C15, F15 | Redundant — existing features already cover |

**Total: 30+ implemented + 52 syntax + 29 stdlib + 15 cut = 105 features analyzed.**

### Industry Coverage

| Industry | Syntax Features | Stdlib Modules | Key Guarantee |
|----------|----------------|----------------|---------------|
| **Banking/Finance** | F38–F43 (money, @conserve, txn, linear, Account<State>, LogicalTime) | S19–S21 (banking, SWIFT, payments) | Compile-time money conservation proof |
| **Automotive** | F44, F52–F55 (@safety-critical, @autosar, @can-bus, @diagnostic, @calibratable) | S22–S23 (CAN, ECU) | ISO 26262 / AUTOSAR compliance at compile time |
| **Aerospace/Defense** | F44–F47, F56–F58 (@safety-critical, @redundant, @bounded, @partitioned, @dal-a, @arinc, @bit-exact) | S24–S25 (ARINC, Ada patterns) | DO-178C DAL-A compliance at compile time |
| **Medical** | F26, F33, F48, F50, F51 (@deterministic, @audit, @medical, @signed, @reproducible) | S28–S29 (medical, clinical) | IEC 62304 compliance + FDA audit trail |
| **Energy/Grid** | F59–F60 (@grid-safe, @scada) | S26–S27 (grid, SCADA) | IEC 61850 compliance + deterministic dispatch |
| **Robotics/IoT** | F27, F29–F31, F35, F37 (@realtime, @interrupt, @firmware, @lowpower, @watchdog, @ota) | S12–S16 (HAL, sensor, motor, protocol, OTA) | WCET proof + no-malloc proof + bare metal |
| **Blockchain** | F28, F34, F50 (@gas, @emit, @signed) | S9–S11 (crypto, consensus, storage) | Compile-time gas verification + determinism |
| **Gaming** | F16–F18, F21, F23–F25 (vec/mat, swizzle, ECS, @shader, @simd, @crashfree, @pool) | S1–S8 (tilemap, sprite, etc.) | Crash-free game loop proof |
| **AI/ML** | F14 (@differentiable) | — (tensor.fd exists) | Compile-time shape checking + autograd |
| **Serverless/Edge** | F32, F36 (@serverless, @handler) | S17–S18 (HTTP server, WASM API) | Tiny binaries + capability-stripped |

**The key insight:** all these industries share the same 3 core safety systems (capabilities, effects, regions). Each industry's annotations are different *configurations* of those systems. Building @deterministic for medical makes @gas for blockchain cheaper. Building @bounded for aerospace makes @crashfree for gaming stronger. This is Fides's moat — one language, every industry, compile-time proofs.

---

## F1 — `sandbox { }` — Capability Revocation Blocks

**Status:** ✅ Implemented
**Files:** token.rs, lexer.rs, ast.rs, parser.rs, typechecker.rs, lower.rs, mono.rs, comptime.rs, codegen.rs
**Test:** `tests/test_sandbox.fd` (passes), `tests/test_sandbox_fail.fd` (expected fail)
**Impact:** High — security
**Novelty:** Genuinely impossible in Rust/C/Go (no compile-time capability system)

### Concept

`sandbox` blocks *remove* all capabilities for their scope. Any IO/FS/Net/Spawn
call inside the block is a compile error. This lets you run untrusted or
plugin code with zero authority — verified at compile time, zero runtime cost.

```p
use core with { IO, FS, Net };

pf main() {
    print("I can print");  // OK — IO granted

    sandbox {
        // Compile error: print requires IO capability
        // Compile error: file_read requires FS capability
        // Compile error: socket_connect requires Net capability
        load_plugin();
    }

    print("I can print again");  // OK — capabilities restored
}
```

### Implementation

1. Add `Sandbox` to AST (`ast.rs`) — a block expression
2. Parse `sandbox { }` in `parser.rs`
3. In `typechecker.rs`: push an empty `CapabilitySet` onto the capability stack
   when entering a `sandbox` block, pop on exit. Existing `check_capability`
   calls will reject any IO/FS/Net/Spawn inside the block.
4. Lowering: `sandbox` is transparent — just lower the inner block. No runtime
   code needed. The safety is entirely compile-time.

### Why Other Languages Can't Do This

- **Rust/C/Go:** No capability system. Authority is ambient — any code can
  call `open()` anytime. You can't remove what was never tracked.
- **Java:** Had `SecurityManager` but deprecated it — it was runtime, not
  compile-time, and too coarse.
- **Wasm:** Has capability-based imports but at module level, not scope level.

### Use Cases

- Plugin systems — load third-party code, prove it can't touch the filesystem
- Supply chain security — wrap untrusted dependencies in `sandbox`
- Sandboxed evaluators — run user-submitted code with zero authority
- Audit — "this function provably doesn't do IO" is a one-line `sandbox` wrap

### Tests

- `test_sandbox.fd` — IO call inside sandbox → compile error
- `test_sandbox_nested.fd` — sandbox inside sandbox, capabilities restored after
- `test_sandbox_safe.fd` — pure function inside sandbox → OK

---

## F2 — `persist` Variables

**Status:** ✅ Implemented
**Files:** token.rs, lexer.rs, ast.rs, parser.rs, ir.rs, lower.rs, interp.rs, pir.rs, codegen_c.rs, pe.rs, wasm.rs
**Test:** `tests/test_persist.fd` (passes)
**Impact:** High — developer experience
**Novelty:** No language has scope-level state persistence across hot reloads

### Concept

`persist` variables survive hot reloads. Their values are saved when the
program exits and restored when it restarts. This makes hot reload useful for
real applications — tweak rendering code without resetting game state, edit
data processing without losing loaded data.

```p
pf main() {
    persist pv counter: i64 = 0;

    counter += 1;
    print(f"Called {counter} times");

    // Edit this function, save → hot reload
    // counter is NOT reset to 0, it keeps its value
}
```

### Implementation

1. Add `Persist` modifier to `LetStmt` in AST (`ast.rs`)
2. Parse `persist pv` in `parser.rs`
3. In `lower.rs`: mark the variable, emit a save call on function exit and
   a restore call on function entry
4. In `hotreload.rs`: `PersistentState` already exists (key-value store in
   temp file). Use the variable name as the key, serialize/deserialize the
   value. The infrastructure is already built and tested.

### Why Other Languages Can't Do This

- **Erlang/Elixir:** Hot reload preserves *process* state, not *variable*
  state. You don't mark individual variables.
- **Rust/Go/C:** No hot reload at all.
- **Python:** `importlib.reload()` re-executes modules from scratch — no
  state preservation.
- **Smalltalk/Lisp:** Persistent images preserve *everything* — no opt-in.

### Use Cases

- Game development — tweak rendering without resetting game state
- Data analysis / REPL — keep loaded data while editing code
- Embedded — tune parameters without losing sensor calibration
- Server dev — reload handler logic without dropping connections

### Tests

- `test_persist.fd` — persist counter, increment, verify value survives reload
- `test_persist_struct.fd` — persist a struct, verify fields survive
- `test_persist_vec.fd` — persist a Vec, verify contents survive

---

## F3 — `region r with budget(N)` — Compile-Time Memory Budgets

**Status:** ✅ Implemented
**Files:** ast.rs, parser.rs, typechecker.rs, lower.rs, mono.rs
**Test:** `tests/test_budget.fd` (passes), `tests/test_budget_fail.fd` (expected fail)
**Impact:** High — embedded/safety-critical
**Novelty:** Genuinely impossible in Rust/Zig (runtime panic only, no compile-time guarantee)

### Concept

Regions can declare a memory budget. The compiler tracks all allocations
inside the region and produces a **compile error** if the total exceeds the
budget. This is a hard guarantee — not a runtime check, not a hope.

```p
pf sensor_loop() !RealTime {
    region r with budget(2048) {
        pv buf = Vec::new();
        for i in 0..256 {
            buf.push(read_sensor(i));  // OK — 256 * 8 = 2048 bytes
        }
        buf.push(0);  // COMPILE ERROR: budget exceeded (2049 > 2048)
    }
}
```

### Implementation

1. Parse `with budget(N)` on `region` declarations (`parser.rs`)
2. In `typechecker.rs`: track cumulative allocation size per region. The
   compiler already knows every `allocator_alloc` call and its type size.
   Sum allocations per region, compare to budget at region end.
3. For dynamic allocations (Vec::push in a loop): use the loop's constant
   trip count × element size. If trip count is not constant, require a
   `max_iterations(N)` annotation on the loop or reject.
4. Zero runtime cost — this is a compile-time check only.

### Why Other Languages Can't Do This

- **Rust:** Has lifetimes but doesn't track allocation sizes in the type
  system. The borrow checker knows *when* memory is freed, not *how much*.
- **Zig:** Has explicit allocators and `FixedBufferAllocator` that panics
  at runtime. No compile-time guarantee.
- **C/C++:** `malloc` is opaque. No memory model at language level.
- **Ada/SPARK:** Can prove memory bounds but requires manual formal proofs.

### Use Cases

- Embedded/firmware — guarantee ≤ 16KB heap usage for 16KB RAM targets
- Real-time systems — prove no dynamic allocation in real-time paths
- Safety-critical (aerospace, medical) — prove memory bounds before deployment
- Server — cap memory per request handler

### Tests

- `test_budget.fd` — region with budget, allocations under budget → OK
- `test_budget_exceed.fd` — allocations exceed budget → compile error
- `test_budget_loop.fd` — constant trip count loop, budget calculated → OK
- `test_budget_dynamic.fd` — dynamic loop without max_iterations → compile error

---

## F4 — `fidesc watch --diff` — Optimization Diff

**Status:** ✅ Implemented
**Files:** main.rs
**Usage:** `fidesc watch <file.fd> --diff [--opt]`
**Impact:** High — developer experience, performance tuning
**Novelty:** Custom toolchain exclusive — impossible with LLVM-based compilers

### Concept

When `fidesc watch` detects a file change, it shows what the optimizer
*did* — PIR changes, assembly changes, binary size delta. Like `git diff`
but for compiler optimizations.

```bash
$ fidesc watch app.fd --diff
[change detected] function `render` modified

  PIR L2 changes:
    + inlined `compute_matrix` (3 instructions removed)
    + CSE eliminated 2 redundant multiplies
    - removed dead store to %v12

  Assembly changes:
    - movq %rax, -24(%rbp)     # dead store eliminated
    + leaq -8(%rbp), %rdi      # inlined argument

  Binary: 312 bytes → 289 bytes (-23)
```

### Implementation

1. Before recompilation: capture current PIR L2 output and assembly output
   for each function (Fides already prints these via `fidesc pir` and
   `fidesc codegen`)
2. After recompilation: capture new output
3. Diff the two (line-based diff, similar to `git diff` output)
4. Print per-function summary: what was inlined, what was eliminated,
   what changed in assembly, binary size delta
5. The watch command already detects which functions changed
   (`FunctionSnapshot::diff`). Just add the output capture + diff.

### Why Other Languages Can't Do This

- **Rust/GCC/Clang:** Use LLVM/GCC as a black box. Optimization remarks
  (`-Rpass`) are a stream of text, not a diff. And they're per-compilation,
  not per-change. You can't easily get "what changed" out of LLVM.
- **Zig:** Doesn't show what the optimizer did.
- **Go:** Same — compiler is opaque.

Fides can do this because it has its *own* optimizer (`opt.rs`) and *own* PIR
representation. The optimizer output is structured and accessible.

### Use Cases

- Performance tuning — see exactly what the optimizer did when you change code
- Teaching — learn how optimizations work on *your* code, not textbook examples
- Debugging — "why did my binary get 200 bytes bigger?" → see the diff
- Optimization verification — confirm that refactor actually helped

### Tests

- Unit test: diff two PIR outputs, verify correct additions/removals reported
- Unit test: diff two assembly outputs, verify correct line changes
- Integration: `fidesc watch --diff` on a test file, verify output format

---

## F5 — Capability-Aware Linking

**Status:** ✅ Implemented
**Files:** main.rs, compiler.rs
**Usage:** `fidesc build <file.fd> --caps "IO,FS"`
**Impact:** High — security + binary size
**Novelty:** No linker in any language removes code based on capabilities

### Concept

When building with `--caps "IO,FS"`, the linker removes all code that
requires capabilities you didn't grant. If you don't grant `Net`, the
linker strips all Net-dependent functions from the binary. Smaller binary,
smaller attack surface, and it's *proven* — the code is not in the binary.

```bash
$ fidesc build app.fd --caps "IO"
  [linker] module `network.fd` requires Net — not granted, excluding
  [linker] removed 12 functions (Net-dependent)
  [linker] removed 3 import entries (socket syscalls)
  Built: 8KB (was 47KB with all modules)
```

### Implementation

1. The typechecker already knows which functions require which capabilities
   (stored in function metadata)
2. The linker already does dead code elimination via BFS from entry point
   (`linker.rs::dead_code_elimination`)
3. Add `--caps` flag to `fidesc build` — parse the granted capability list
4. In the DCE pass: treat "requires ungranted capability" as "unreachable"
   — don't traverse into functions that need capabilities you didn't grant
5. Report what was removed (function names, import entries, byte savings)

### Why Other Languages Can't Do This

- **Rust/C/Go:** No capabilities. Linker does DCE based on reachability,
  not authority.
- **Wasm:** Linker strips unused imports, but based on reachability, not
  capabilities.
- No language has *compile-time capabilities* that the linker could use.

### Use Cases

- Minimal binaries — strip unused network/FS code for embedded targets
- Attack surface reduction — prove network code is not in the binary
- Supply chain audit — "this binary provably cannot make network calls"
- Container images — smaller binaries = smaller images = faster deploys

### Tests

- `test_link_caps.fd` — build with `--caps "IO"`, verify Net code excluded
- `test_link_caps_full.fd` — build with `--caps "IO,FS,Net"`, verify all included
- Unit test: DCE with capability filter, verify correct functions removed

---

## F6 — `guard cond => action` — Early Exit Without Nesting

**Classification:** SYNTAX — Tier B (ergonomic; could be `guard(cond, || return)` but used too often for function syntax)
**Status:** 🔲 Planned
**Impact:** High — developer experience, readability
**Novelty:** Swift has `guard`; Fides adds the `=>` arrow for explicit action, making the exit path unmissable

### Concept

`guard` is an early-exit statement that flattens code. Instead of wrapping
the happy path in nested `if` blocks, you state the precondition upfront and
exit if it fails. The `=>` arrow makes the exit action explicit and visible.

```p
pf process(data: Vec<i64>) -> i64 {
    guard data.len() > 0 => return 0;
    guard data.len() < 100 => return -1;

    // Happy path — no nesting, full width available
    pv sum = 0;
    for x in data { sum += x; }
    sum
}

// Without guard — the pyramid of doom:
pf process_bad(data: Vec<i64>) -> i64 {
    if data.len() > 0 {
        if data.len() < 100 {
            pv sum = 0;
            for x in data { sum += x; }
            sum
        } else { -1 }
    } else { 0 }
}
```

### Implementation

1. Add `Guard` keyword to `token.rs` / `lexer.rs`
2. Add `Stmt::Guard { cond, action, line }` to `ast.rs`
3. Parse `guard <expr> => <stmt>` in `parser.rs`
4. Typechecker: verify `action` is a valid exit (return, break, continue, or panic)
5. Lowering: desugar to `if !cond { action }` — zero new IR needed

### Why It's Better Than Swift's guard

- Swift's `guard` requires `else { return }` — two keywords for the error path
- Fides's `guard cond => return` uses the arrow to visually separate condition from action
- The arrow is consistent with match arms (`=>`) and closures (`=>`)

### Use Cases

- Input validation at function entry
- Early returns from loops
- Precondition checks before expensive operations
- Reducing cognitive load — the happy path is always at the top level

---

## F7 — `..` Spread Operator — Zero-Boilerplate Updates

**Classification:** SYNTAX — Tier B (ergonomic; could be `struct::copy(p, hp=70)` but used too often for function syntax)
**Status:** 🔲 Planned
**Impact:** High — developer experience, immutability ergonomics
**Novelty:** Rust has struct update syntax (`..base`) but no array spread; JS has both but no type safety. Fides has both with compile-time verification.

### Concept

The `..` operator spreads collections and struct fields into new values.
This makes immutable updates concise — no manual field copying, no loop to
clone arrays.

```p
// ─── Struct spread: update one field, copy the rest ───────────
pv player = Player { name: "Hero", hp: 100, pos: vec3(0, 0, 0), inventory: Vec::new() };

// Take damage — copy everything except hp
pv hurt = Player { ..player, hp: player.hp - 10 };

// Move — copy everything except pos
pv moved = Player { ..player, pos: vec3(10, 0, 0) };

// ─── Array spread: combine arrays without loops ───────────────
pv a = [1, 2, 3];
pv b = [4, 5, 6];
pv combined = [..a, ..b, 7];  // [1, 2, 3, 4, 5, 6, 7]

// ─── Function arguments: spread a Vec into varargs ────────────
pv args = [1, 2, 3];
pv result = sum(..args);  // equivalent to sum(1, 2, 3)
```

### Implementation

1. Add `Spread` token to `token.rs` (reuse `DotDot` or add `DotDotDot`)
2. Add `Expr::Spread { expr }` to `ast.rs`
3. Parse `..expr` in array literals, struct literals, and call argument positions
4. Typechecker: verify spread type matches context (array element type, struct type, function param type)
5. Lowering: for arrays → emit element-by-element copy; for structs → emit field-by-field copy with overrides; for calls → unpack into individual args

### Why Other Languages Fall Short

- **Rust:** Has `..base` for structs but no array spread. `vec![..a, ..b]` doesn't work.
- **JavaScript:** Has spread but no type checking — `[...a, ...b]` can silently produce wrong types.
- **Go:** No spread at all. Manual copying everywhere.
- **Python:** `*args` for function calls but no struct spread.

### Use Cases

- Immutable state updates (game entities, UI state, config)
- Combining arrays without `extend()` chains
- Passing Vec elements as function arguments
- Cloning with modifications — the most common operation in data-heavy code

---

## F8 — `?.` Optional Chaining — Zero-Cost Null Safety

**Classification:** SYNTAX — Tier A (typechecker must narrow types through the chain — a library can't do this)
**Status:** 🔲 Planned
**Impact:** High — developer experience, safety
**Novelty:** Fides already has `Option<T>`; this adds the chaining syntax that makes it painless. Combined with Fides's comptime, chains can be fully evaluated at compile time when possible.

### Concept

`?.` short-circuits a chain of field/method accesses if any intermediate
value is `None`. Instead of writing nested `match` blocks, you chain `?.`
and the compiler inserts the null checks automatically.

```p
// Without ?. — pyramid of match blocks
pv zip = match user {
    Some(u) => match u.address {
        Some(a) => match a.zip_code {
            Some(z) => z,
            None => "unknown",
        },
        None => "unknown",
    },
    None => "unknown",
};

// With ?. — one line
pv zip = user?.address?.zip_code ?? "unknown";

// Method chaining with ?.
pv label = user?.get_profile()?.display_name ?? "Anonymous";

// Assignment with ?. — only sets if the chain is valid
user?.address?.city = "Portland";  // no-op if user or address is None
```

### Implementation

1. Add `QuestionDot` token (`?.`) to `token.rs` / `lexer.rs`
2. Extend `Expr::FieldAccess` and `Expr::MethodCall` with an `optional: bool` flag
3. Parse `?.` as optional variant of `.` in `parser.rs`
4. Typechecker: if `expr` is `Option<T>`, unwrap to `T` for the field access; result type is `Option<U>` where U is the field type
5. Lowering: desugar `a?.b` to `match a { Some(x) => Some(x.b), None => None }`

### Why It Matters

- **C#/TypeScript:** Have `?.` but no compile-time type safety on the chain
- **Rust:** Has `Option` but no `?.` — you write `and_then()` chains or nested `match`
- **Swift:** Has `?.` but optional is implicit, not explicit like Fides's `Option<T>`
- **Fides's advantage:** Explicit `Option<T>` + `?.` chaining + comptime evaluation = zero-cost when the value is known at compile time

### Use Cases

- Navigating nested data structures (JSON, configs, database records)
- Chaining method calls that might return None
- Game code: `entity?.get_component(Health)?.hp ?? 0`
- API responses: `response?.data?.user?.email ?? "no email"`

---

## F9 — `??` Coalescing — Default Values

**Classification:** SYNTAX — Tier B (ergonomic; could be `coalesce(x, default)` but used too often for function syntax)
**Status:** 🔲 Planned
**Impact:** Medium — developer experience
**Novelty:** Pairs with `?.` — provides the "else" branch for optional chains. Fides's version also works for zero-value defaults on primitives.

### Concept

`??` returns the left side if it's `Some(x)` (or non-zero for primitives),
otherwise the right side. It's the companion to `?.` — the chain gives you
an `Option`, and `??` gives you a concrete value.

```p
// Option coalescing
pv name = user?.name ?? "Anonymous";
pv port = config?.port ?? 8080;

// Primitive coalescing — 0 / "" / false are treated as "absent"
pv timeout = config.timeout ?? 30;        // if timeout == 0, use 30
pv label = config.label ?? "default";     // if label == "", use "default"
pv enabled = config.enabled ?? true;       // if enabled == false, use true

// Chaining with ?? for fallbacks
pv server = primary ?? backup ?? local ?? "localhost:8080";
```

### Implementation

1. Add `QuestionQuestion` token (`??`) to `token.rs` / `lexer.rs`
2. Add `Expr::Coalesce { left, right }` to `ast.rs`
3. Parse `??` as a low-precedence binary operator in `parser.rs`
4. Typechecker: if left is `Option<T>`, right must be `T`, result is `T`; if left is a primitive, right must be same type, result is same type
5. Lowering: for `Option<T>` → `match left { Some(x) => x, None => right }`; for primitives → `if left != zero { left } else { right }`

### Use Cases

- Default values for optional configuration
- Fallback chains (primary → backup → last resort)
- Game defaults: `save_game?.level ?? 1`
- Eliminating `unwrap_or()` boilerplate

---

## F10 — `given`/`ensure` — Design by Contract

**Classification:** SYNTAX — Tier A (typechecker must verify contracts at all call sites — a library can't see call sites)
**Status:** ✅ Implemented
**Impact:** High — safety, correctness
**Novelty:** Eiffel had contracts but they were runtime-checked. Fides's version leverages comptime to verify simple contracts at compile time. No other systems language has native contract syntax.

### Concept

`given` declares preconditions (must be true on entry). `ensure` declares
postconditions (must be true on exit). The compiler verifies simple
contracts at compile time; complex ones become runtime assertions in debug
builds and are elided in release builds.

```p
pf sqrt(x: f64) -> f64
    given x >= 0.0
    ensure result >= 0.0
{
    // Compiler verifies: x >= 0 is checked at call sites with known values
    // Runtime: assertion in debug mode, elided in release
    if x == 0.0 { ret 0.0; }
    pv r = x;
    for _ in 0..20 {
        r = (r + x / r) / 2.0;
    }
    r
}

pf binary_search(arr: Vec<i64>, target: i64) -> i64
    given arr.is_sorted()           // precondition: array must be sorted
    ensure result >= 0 || result == -1  // postcondition: valid index or -1
{
    pv lo = 0;
    pv hi = arr.len() - 1;
    while lo <= hi {
        pv mid = (lo + hi) / 2;
        if arr[mid] == target { ret mid; }
        if arr[mid] < target { lo = mid + 1; }
        else { hi = mid - 1; }
    }
    -1
}

// Contracts on generic functions with trait bounds
pf max<T: Ord>(a: T, b: T) -> T
    ensure result >= a && result >= b
{
    if a >= b { a } else { b }
}
```

### Implementation

1. Add `Given` and `Ensure` keywords to `token.rs` / `lexer.rs`
2. Add `preconditions: Vec<Expr>` and `postconditions: Vec<Expr>` to `Func` in `ast.rs`
3. Parse `given <expr>+` and `ensure <expr>+` between function signature and body
4. Typechecker: for `given` — at each call site, if argument values are known at compile time, evaluate the precondition. If it fails → compile error. If unknown → emit a runtime assertion (debug only). For `ensure` — analyze the function body to verify the postcondition holds for all return paths.
5. Lowering: in debug mode, emit `assert()` calls. In release mode, elide runtime checks (compile-time checks still apply).
6. `result` is a special identifier in `ensure` expressions — bound to the return value.

### Why Other Languages Can't Do This

- **Rust:** No contract syntax. Debug assertions are manual (`assert!`). No compile-time verification.
- **D:** Has contracts but they're always runtime-checked, never compile-time.
- **Ada/SPARK:** Can prove contracts but requires external proof tools and manual effort.
- **C++:** `[[expects:]]` / `[[ensures:]]` attributes exist but are ignored by most compilers.
- **Fides's advantage:** Comptime evaluation + capability system + effect system = contracts that are actually verified, not just documented.

### Use Cases

- API design — make preconditions explicit and enforced
- Safety-critical code — prove input ranges at compile time
- Library authoring — `given ptr != null` eliminates an entire class of bugs
- Game development — `ensure entity.health >= 0` (health can't go negative)
- Mathematical functions — `given x >= 0` for sqrt, `given n > 0` for log

---

## F11 — `is` Type Check — Clean Type Narrowing

**Classification:** SYNTAX — Tier B (type narrowing in branches needs typechecker, but could work as a library with explicit casts)
**Status:** 🔲 Planned
**Impact:** Medium — developer experience
**Novelty:** C# has `is` but it doesn't narrow types. TypeScript has `typeof` but it's string-based. Fides's `is` narrows the type in the branch, eliminating redundant casts.

### Concept

`is` checks if a value is of a specific type and narrows the type in the
branch body. No more `as` casts after a type check — the compiler knows
the type is narrowed.

```p
pv value: dyn Display = get_value();

// is + type narrowing — no cast needed inside the branch
if value is i64 {
    // value is i64 here — no cast needed
    println(value + 1);
} else if value is str {
    // value is str here
    println(value.len());
}

// is with enum variants
pv shape = Shape::Circle(5.0);
if shape is Circle {
    println(shape.radius);  // field access without match binding
}

// is in match arms
match value {
    is i64 => println("number"),
    is str => println("string"),
    _ => println("unknown"),
}

// is not — negated form
if value is not null {
    process(value);
}
```

### Implementation

1. Add `Is` keyword to `token.rs` / `lexer.rs` (already exists as `Is`? check)
2. Add `Expr::Is { expr, ty }` to `ast.rs`
3. Parse `expr is Type` in `parser.rs`
4. Typechecker: evaluate `expr` type, compare to `Type`. In the `then` branch, narrow the variable's type to `Type`. Return `bool`.
5. Lowering: for concrete types → emit a type tag check. For trait objects → check the vtable type. For enums → check the variant tag.

### Use Cases

- Type-safe handling of `dyn` trait objects
- Pattern matching without full `match` blocks
- Runtime type checking with compile-time narrowing
- Game code: `if entity is Player { ... }` vs `if entity is Enemy { ... }`

---

## F12 — `@memo` — Zero-Boilerplate Auto-Memoization

**Classification:** SYNTAX — Tier B (could be `memoize(|n| { ... })` wrapper, but annotation is cleaner; purity verified by effect system)
**Status:** 🔲 Planned
**Impact:** High — performance, developer experience
**Novelty:** No systems language has annotation-based memoization. Python has `@lru_cache` but it's dynamic. Fides's version uses comptime to generate the cache lookup code, with zero runtime overhead for cache misses on pure functions.

### Concept

`@memo` automatically caches function results. The compiler generates a
hash map keyed on the function arguments. On subsequent calls with the same
arguments, the cached result is returned without re-executing the function.

```p
@memo
pf fib(n: i64) -> i64 {
    if n <= 1 { ret n; }
    fib(n - 1) + fib(n - 2)
}

// Without @memo: fib(40) takes ~1 billion calls
// With @memo: fib(40) takes ~40 calls

@memo(max_size=1024)
pf expensive_hash(data: Vec<i64>) -> i64 {
    // Cache up to 1024 unique argument combinations
    // LRU eviction when cache is full
    pv h = 0;
    for x in data { h = h * 31 + x; }
    h
}

// @memo requires pure functions — the effect system verifies this
@memo
pf impure_func(x: i64) -> i64 {
    print("side effect");  // COMPILE ERROR: @memo requires !{} (pure)
    x * 2
}
```

### Implementation

1. Recognize `@memo` annotation in `parser.rs` (already parses annotations)
2. In `typechecker.rs`: verify the function is pure (effect set is `!{}`). If not → compile error.
3. In `lower.rs` or a new `memo.rs` pass: generate a static cache (HashMap) keyed on the argument tuple. Wrap the function body: check cache → if hit, return cached; if miss, execute body, store result, return.
4. For `max_size=N`: generate an LRU eviction check.
5. The cache is per-monomorphization — each generic instantiation gets its own cache.

### Why Other Languages Fall Short

- **Python:** `@lru_cache` works but Python is dynamic — no compile-time purity check, no type-safe cache keys.
- **Rust:** No built-in memoization. You write `HashMap` boilerplate by hand or use a crate.
- **C++:** No memoization. Manual `std::unordered_map` caching.
- **Fides's advantage:** Effect system proves purity at compile time. Comptime generates the cache code. Zero boilerplate, zero runtime overhead for the purity check.

### Use Cases

- Recursive algorithms (fibonacci, ackermann, edit distance)
- Expensive pure computations (hashing, parsing, normalization)
- Game AI: memoized pathfinding heuristics
- ML: memoized feature extraction

---

## F13 — `parallel { | }` — Structured Fork-Join Concurrency

**Classification:** SYNTAX — Tier A (compiler must emit thread spawn/join — a library can't manage threads this way)
**Status:** 🔲 Planned
**Impact:** High — performance, developer experience
**Novelty:** Rayon (Rust) uses `join()` function calls. OpenMP uses `#pragma`. Fides uses native syntax with `|` as the task separator — the most readable form of structured parallelism.

### Concept

`parallel { }` runs all `|`-separated tasks concurrently and automatically
joins at the closing `}`. Each task can return a value. The result is a
tuple of all task results. If any task panics, the others are cancelled.

```p
// Run three computations in parallel, auto-join at }
pv (a, b, c) = parallel {
    | heavy_computation_1()
    | heavy_computation_2()
    | heavy_computation_3()
};

// Parallel data processing
pv results = parallel {
    | process_batch(batch_1)
    | process_batch(batch_2)
    | process_batch(batch_3)
    | process_batch(batch_4)
};

// Parallel with shared read-only state (no locks needed)
pv data = load_data();  // immutable after this point
pv (min, max, avg) = parallel {
    | data.iter().min()
    | data.iter().max()
    | data.iter().sum() / data.len()
};

// Error propagation — if any task fails, all are cancelled
pv result = parallel {
    | fetch_from_primary()    // if this fails, secondary is cancelled
    | fetch_from_secondary()
};
```

### Implementation

1. Add `Parallel` keyword to `token.rs` / `lexer.rs`
2. Add `Stmt::Parallel { tasks: Vec<Vec<Stmt>>, line }` to `ast.rs`
3. Parse `parallel { | task1 | task2 | ... }` in `parser.rs` — `|` separates tasks
4. Typechecker: each task must have the same effect set (no task can have effects another doesn't — for safety). Result type is a tuple of task return types.
5. Lowering: emit `spawn` calls for each task, then `join` at the end. Leverage existing `spawn` infrastructure.
6. Capability checking: all tasks inherit the enclosing scope's capabilities. No task can escalate privileges.

### Why Other Languages Fall Short

- **Rust/Rayon:** `rayon::join(|| a(), || b())` — function-based, not syntax. No tuple destructuring.
- **OpenMP:** `#pragma omp parallel sections` — preprocessor, not language. No type safety.
- **Go:** `go func() { ... }()` + `sync.WaitGroup` — manual goroutine management, no auto-join.
- **Fides's advantage:** Native syntax, auto-join, tuple destructuring, effect system prevents data races, capability system prevents privilege escalation.

### Use Cases

- Parallel data processing (map-reduce style)
- Game engine subsystems (physics, AI, rendering in parallel)
- ML pipeline parallelism (data loading + preprocessing + training)
- Any embarrassingly parallel computation

---

## F14 — `@differentiable` — Mark Functions for Autograd

**Classification:** SYNTAX — Tier A (compiler must trace IR operations for autograd — a library can't inspect IR)
**Status:** 🔲 Planned
**Impact:** High — AI/ML
**Novelty:** JAX uses `jax.grad()` transformations. PyTorch uses runtime autograd graphs. Fides uses a compile-time annotation that tells the autograd pass to trace the function and generate backward code.

### Concept

`@differentiable` marks a function as eligible for autograd. When called
inside a `grad { }` block, the autograd pass traces the function's
operations and generates gradient code. Functions without `@differentiable`
are treated as opaque (no gradients computed through them).

```p
@differentiable
pf relu(x: f64) -> f64 {
    if x > 0.0 { x } else { 0.0 }
}

@differentiable
pf loss(pred: Tensor, target: Tensor) -> f64 {
    pv diff = pred - target;
    pv squared = diff * diff;
    mean(squared)  // MSE loss
}

// Inside a grad block, @differentiable functions are traced
pf train(model: Model, data: Tensor, labels: Tensor) {
    grad(model.weights) {
        pv pred = model.forward(data);
        pv l = loss(pred, labels);
        backward(l);  // gradients computed through @differentiable functions
    }
}
```

### Implementation

1. Recognize `@differentiable` annotation in `parser.rs`
2. In `typechecker.rs`: verify the function only uses `@differentiable` operations (or primitives the autograd knows how to differentiate)
3. In `autograd.rs`: when tracing a grad block, follow calls to `@differentiable` functions and trace their bodies too. Non-differentiable functions are treated as constants.
4. Generate backward code for each `@differentiable` function, composing them via the chain rule.

### Use Cases

- Custom loss functions in ML training
- Custom activation functions
- Differentiable physics simulations
- Gradient-based optimization

---

## F15 — `@no_grad` — CUT

**Status:** ❌ Cut — unnecessary in Fides's autograd model

### Why cut

PyTorch needs `torch.no_grad()` because gradient tracking is **on by default** —
you need a way to disable it for inference. Fides's autograd is **opt-in** —
gradient tracking only happens inside `grad(var) { }` blocks. Outside those
blocks, there's nothing to disable. The default IS no-grad.

The only theoretical use case (skip tracing for a helper function inside a
`grad` block) is handled by the **@differentiable opt-in model**: only
`@differentiable` functions get traced. If you don't mark a function as
`@differentiable`, it won't be traced. `@no_grad` is the inverse of an opt-in
that already exists.

---

## F16 — `vec2`/`vec3`/`vec4` + `mat4` — First-Class Math Types

**Classification:** SYNTAX — Tier A (compiler must emit SIMD instructions for vector operators — a library has function call overhead)
**Status:** 🔲 Planned
**Impact:** High — game development, graphics, ML
**Novelty:** GLSL/HLSL have these but are shader-only languages. Fides brings them to a general-purpose systems language with full operator overloading, swizzle syntax, and compile-time shape checking.

### Concept

Vector and matrix types are first-class citizens in Fides. They support all
arithmetic operators, are type-checked at compile time, and generate
efficient SIMD code on supported hardware.

```p
// ─── Vector types ─────────────────────────────────────────────
pv a: vec3 = vec3(1.0, 2.0, 3.0);
pv b: vec3 = vec3(4.0, 5.0, 6.0);

pv sum = a + b;              // vec3(5, 7, 9)
pv diff = a - b;             // vec3(-3, -3, -3)
pv scaled = a * 2.0;         // vec3(2, 4, 6) — scalar mult
pv dot_product = dot(a, b);  // 32.0 — scalar
pv cross_product = cross(a, b);  // vec3 — perpendicular vector
pv len = length(a);          // 3.741...
pv unit = normalize(a);      // unit vector
pv lerp_result = lerp(a, b, 0.5);  // vec3(2.5, 3.5, 4.5)

// ─── Matrix types ─────────────────────────────────────────────
pv model = translate(vec3(10, 0, 0)) * rotate_y(0.5) * scale(2.0);
pv view = look_at(eye, target, up);
pv proj = perspective(radians(60), 16.0/9.0, 0.1, 100.0);
pv mvp = proj * view * model;

pv clip_pos = mvp * vec4(local_pos, 1.0);  // matrix × vector
pv screen = clip_pos.xyz / clip_pos.w;     // perspective divide

// ─── 2D game helpers ──────────────────────────────────────────
pv pos: vec2 = vec2(100, 200);
pv size: vec2 = vec2(32, 32);
pv rect = rect_from(pos, size);
if rect.contains(mouse_pos) { /* mouse is over button */ }

// ─── Quaternion rotation ──────────────────────────────────────
pv q = quat_from_axis_angle(vec3(0, 1, 0), radians(90));
pv rotated = q * vec3(1, 0, 0);  // rotate vector by quaternion
pv combined = q1 * q2;           // compose rotations
pv smooth = slerp(q1, q2, 0.5);  // smooth interpolation
```

### Implementation

1. Add `vec2`, `vec3`, `vec4`, `mat2`, `mat3`, `mat4`, `quat` as type aliases for fixed-shape tensors: `vec3 = Tensor<f32, [3]>`, `mat4 = Tensor<f32, [4, 4]>`
2. Add operator overloading via traits: `cap VecOps { op_add, op_sub, op_mul, ... }` with impls for each vec/mat type
3. Built-in functions: `dot`, `cross`, `length`, `normalize`, `lerp`, `translate`, `rotate_*`, `scale`, `perspective`, `look_at`, `slerp`
4. Codegen: on x86-64 → SSE/AVX instructions; on ARM → NEON; on GPU → native vector ops
5. Shape checking: `vec3 + vec2` → compile error (shape mismatch)

### Why Other Languages Fall Short

- **C++:** Needs GLM or DirectXMath library. No native syntax.
- **Rust:** Needs `nalgebra` or `cgmath` crate. No swizzle.
- **C#:** Unity's `Vector3` is a class with method calls, not operators.
- **Fides's advantage:** Native types + operators + swizzle + SIMD codegen + shape checking. Game devs and ML engineers get shader-quality math in a systems language.

### Use Cases

- Game physics (positions, velocities, forces)
- 3D graphics (transforms, projections, lighting)
- ML (vectorized operations, embeddings)
- Simulation (particle systems, fluid dynamics)
- UI (2D layouts, animations)

---

## F17 — Vector Swizzle — Shader-Style Component Access

**Classification:** SYNTAX — Tier A (parser must recognize `.xyz` patterns; typechecker must verify component validity; zero-cost desugaring)
**Status:** 🔲 Planned
**Impact:** High — game development, graphics
**Novelty:** GLSL/HLSL have swizzle but are shader-only. Fides brings it to a general-purpose language. The `.xyz` syntax desugars to `vec3(self.x, self.y, self.z)` at compile time — zero runtime cost.

### Concept

Swizzle allows accessing and reordering vector components using `.xy`,
`.xyz`, `.rgb`, `.xyzw` notation. Any combination of `x/y/z/w` or
`r/g/b/a` can be used. Components can be repeated, reordered, or omitted.

```p
pv color: vec4 = vec4(1.0, 0.5, 0.2, 1.0);

// Read swizzles — any combination of x/y/z/w or r/g/b/a
pv rgb = color.rgb;       // vec3(1.0, 0.5, 0.2) — color interpretation
pv xy = color.xy;         // vec2(1.0, 0.5)     — position interpretation
pv zz = color.zz;         // vec2(0.2, 0.2)     — replication
pv yzx = color.yzx;       // vec3(0.5, 0.2, 1.0) — reordering
pv xxx = color.xxx;       // vec3(1.0, 1.0, 1.0) — splat

// Write swizzles — assign multiple components at once
pv pos: vec3 = vec3(0.0, 0.0, 0.0);
pos.xz = vec2(10.0, 20.0);     // modify x and z, y stays 0.0
pos.rgb = vec3(1.0, 0.0, 0.0); // same thing, color syntax

// Chaining with operators
pv v: vec4 = vec4(1.0, 2.0, 3.0, 4.0);
pv result = v.xyz * 2.0 + v.w;  // vec3(4, 6, 8) + 4 = vec3(8, 10, 12)

// In game code
pv to_enemy = enemy.pos - player.pos;
pv direction = normalize(to_enemy);
pv forward = vec3(0, 0, -1);
pv is_in_front = dot(direction, forward) > 0;
pv right = cross(forward, up);  // perpendicular vector
```

### Implementation

1. In `parser.rs`: when parsing `.identifier`, check if the identifier matches swizzle pattern (`[xyzw]+` or `[rgba]+`, length 1-4)
2. If it matches, create `Expr::Swizzle { expr, components: Vec<char> }` instead of `Expr::FieldAccess`
3. Typechecker: verify the swizzle is valid for the vector type (e.g., `.xyz` requires vec3+, `.xyzw` requires vec4)
4. Lowering: `.xyz` on a vec4 desugars to `vec3(self.x, self.y, self.z)` — direct field access, zero cost
5. Write swizzle: `pos.xz = vec2(a, b)` desugars to `pos.x = a; pos.z = b;`

### Why It Matters

- **GLSL/HLSL:** Have swizzle but only in shader code. Can't use for game logic.
- **C++ (GLM):** Has swizzle via macros but it's opt-in and verbose.
- **Rust (cgmath):** No swizzle. You write `vec3(v.x, v.y, v.z)`.
- **Fides's advantage:** Swizzle in a general-purpose language. Same syntax for shaders and game logic. Zero cost via compile-time desugaring.

### Use Cases

- Color manipulation (`.rgb`, `.alpha`)
- Position/rotation extraction (`.xyz`, `.xz` for 2D in 3D)
- Data packing/unpacking (shuffle components for SIMD)
- Shader-like math in game logic code

---

## F18 — ECS (Entity-Component-System) — Native Game Architecture

**Classification:** SYNTAX — Tier A (compiler must control SoA memory layout + optimize query iteration — a library can't rearrange memory)
**Status:** 🔲 Planned
**Impact:** Very High — game development
**Novelty:** Unity bolted ECS onto C# with attributes. Unreal uses C++ macros. Bevy uses Rust traits. Fides makes ECS native language syntax with `component`, `system`, and `query` keywords — the first language to do so.

### Concept

ECS is the dominant architecture for game engines. Fides makes it native syntax:
- `component` declares a data container (pure data, no logic)
- `system` declares a function that runs over all entities matching a query
- `query` iterates all entities that have the specified components

```p
// ─── Components: pure data ────────────────────────────────────
component Position { x: f32, y: f32, z: f32 }
component Velocity { dx: f32, dy: f32, dz: f32 }
component Health   { hp: i32, max_hp: i32 }
component Sprite   { texture: str, frame: i32 }
component Collider { radius: f32 }

// ─── Systems: logic over matching entities ────────────────────
system Move(dt: f32) {
    query(Position, mut Velocity) {
        entity.position.x += entity.velocity.dx * dt;
        entity.position.y += entity.velocity.dy * dt;
        entity.position.z += entity.velocity.dz * dt;
    }
}

system Gravity(dt: f32) {
    query(mut Velocity) {
        entity.velocity.dy -= 9.81 * dt;
    }
}

system Render {
    query(Position, Sprite) {
        draw_sprite(entity.sprite.texture, entity.sprite.frame,
                    entity.position);
    }
}

// ─── System dependencies: control execution order ─────────────
system Combat {
    after Gravity, Move;  // runs after both
    query(Health, Collider) {
        // collision detection + damage
    }
}

// ─── Spawning entities at runtime ─────────────────────────────
pf spawn_enemy(x: f32, y: f32) -> Entity {
    pv e = Entity::new();
    e.add(Position { x, y, z: 0.0 });
    e.add(Velocity { dx: 0.0, dy: 0.0, dz: 0.0 });
    e.add(Health { hp: 100, max_hp: 100 });
    e.add(Sprite { texture: "enemy.png", frame: 0 });
    e.add(Collider { radius: 16.0 });
    e
}

// ─── System scheduling: parallel execution ────────────────────
// Systems with no dependencies run in parallel automatically
schedule {
    parallel { Move | Gravity },  // these can run together
    Combat,                        // runs after both
    Render,                        // runs last
}
```

### Implementation

1. Add `Component`, `System`, `Query`, `Schedule` keywords to `token.rs` / `lexer.rs`
2. Add `ComponentDef`, `SystemDef`, `QueryExpr` to `ast.rs`
3. Parse `component Name { fields }`, `system Name(params) { body }`, `query(Components) { body }` in `parser.rs`
4. Typechecker: verify components are pure data (no methods). Verify queries only access declared components. Verify system dependencies form a DAG (no cycles).
5. Lowering: desugar to struct definitions + iteration functions. `query(A, B) { body }` becomes a loop over all entities with both A and B components.
6. Memory layout: the compiler chooses Structure-of-Arrays (SoA) for cache efficiency. Components are stored in contiguous arrays indexed by entity ID.
7. Parallel scheduling: systems with no dependencies are parallelized using `parallel { }` (F13).

### Why Other Languages Fall Short

| Feature | Unity (C#) | Unreal (C++) | Bevy (Rust) | **Fides** |
|---------|-----------|-------------|-------------|-------|
| ECS syntax | Attributes | Macros | Traits | **Native keywords** |
| Query syntax | LINQ-like | Manual | Trait bounds | **`query()` keyword** |
| Memory layout | Manual | Manual | Auto (SoA) | **Auto (SoA)** |
| Parallel systems | Job system | Task graph | Parallel ECS | **`parallel { }`** |
| Compile-time checks | Partial | None | Partial | **Full (no invalid queries)** |

### Use Cases

- Game engines (the primary use case)
- Simulation systems (particle systems, physics)
- Data-oriented design (cache-friendly data processing)
- Any system with many entities + many components + many processors

---

## F19 — `state` — Native State Machine Syntax

**Classification:** SYNTAX — Tier B (could be a library with enums + match, but compile-time transition verification needs typechecker)
**Status:** 🔲 Planned
**Impact:** High — game development, UI, protocol handling
**Novelty:** No language has native state machine syntax. Libraries exist (XState for JS, smach for Rust) but they're external. Fides makes state machines first-class language constructs with compile-time transition verification.

### Concept

`state` blocks define a state machine: states, transitions, and per-state
event handlers. The compiler verifies that all transitions point to valid
states, that all states are reachable, and that all events are handled.

```p
state Player {
    // ─── Idle: standing still ────────────────────────────────
    Idle {
        on input(Move(dir: vec3)) {
            player.velocity = dir * speed;
        } => Running

        on input(Attack) {
            player.attack_timer = 0.3;
        } => Attacking

        on input(Damage(amount: i32)) {
            player.health -= amount;
        } => Hurt

        on tick(dt: f32) {
            player.sprite.frame = 0;
        }
    }

    // ─── Running: moving ─────────────────────────────────────
    Running {
        on input(Move(dir: vec3)) {
            player.velocity = dir * speed;
        }  // stay in Running

        on input(Stop) => Idle

        on tick(dt: f32) {
            player.position += player.velocity * dt;
            player.sprite.frame = (player.sprite.frame + 1) % 8;
        }
    }

    // ─── Attacking: mid-swing ────────────────────────────────
    Attacking {
        on tick(dt: f32) {
            player.attack_timer -= dt;
            if player.attack_timer < 0.15 && !player.has_hit {
                deal_damage_in_range(player.position, 2.0, 50);
                player.has_hit = true;
            }
        }

        on timer(player.attack_timer) => Idle
    }

    // ─── Hurt: invulnerable briefly ──────────────────────────
    Hurt {
        on tick(dt: f32) {
            player.invuln_timer -= dt;
            player.sprite.visible = (player.invuln_timer % 0.1) < 0.05;
        }

        on timer(player.invuln_timer) => Idle

        on input(Damage(_)) { }  // ignore damage while invulnerable
    }
}

// ─── Using the state machine ──────────────────────────────────
pv player_state = Player::Idle;

// In game loop:
player_state.handle(input_event);  // dispatches to current state
player_state.handle(tick(dt));     // runs tick handler
```

### Implementation

1. Add `State` keyword to `token.rs` / `lexer.rs`
2. Add `StateMachineDef`, `StateDef`, `Transition` to `ast.rs`
3. Parse `state Name { StateA { on event => target } StateB { ... } }` in `parser.rs`
4. Typechecker: verify all `=> target` references point to defined states. Verify all states are reachable from the initial state. Warn on unhandled events.
5. Lowering: desugar to an enum (one variant per state) + a `handle` method with a match block. The `on event` handlers become match arms.
6. `on timer(expr) => target` desugars to: when `expr <= 0`, transition to `target`.

### Why It's Better Than Libraries

- **XState (JS):** JSON config, no compile-time checks. Invalid transitions are runtime errors.
- **smach (Rust):** Builder pattern, verbose. No syntax-level support.
- **Unity Animator:** Visual editor, not code. Hard to version control.
- **Fides's advantage:** Native syntax, compile-time verified transitions, readable as code, version-controllable, integrates with `on` event handlers (F20).

### Use Cases

- Game character states (idle, run, attack, hurt, dead)
- UI state machines (loading, ready, error, empty)
- Network protocol state machines (connecting, connected, disconnecting)
- AI behavior states (patrol, chase, attack, flee)
- Tutorial/quest progression states

---

## F20 — `on` Event Handlers — Unified Event System

**Classification:** SYNTAX — Tier B (could be a callback registry library, but unified dispatch + compile-time parameter verification is cleaner as syntax)
**Status:** 🔲 Planned
**Impact:** High — game development, UI, async
**Novelty:** Unity uses magic method names (`OnCollisionEnter`, `Update`). Node.js uses `addEventListener`. Fides uses a single `on` keyword for all event types — input, collision, physics, lifecycle — with type-safe parameters.

### Concept

`on` declares an event handler. The compiler connects it to the appropriate
event source based on the event type. One keyword, all events.

```p
// ─── Input events ─────────────────────────────────────────────
on key_down(key: Key) {
    match key {
        Key::Space => player.jump(),
        Key::E => player.interact(),
        Key::Escape => game.pause(),
    }
}

on key_up(key: Key) {
    if key == Key::Space { player.stop_jump(); }
}

on mouse_move(pos: vec2, delta: vec2) {
    camera.rotate(delta * sensitivity);
}

// ─── Collision events ─────────────────────────────────────────
on collision(other: Entity, point: vec3, normal: vec3) {
    if other.has(Health) {
        pv push = normal * 0.5;
        entity.position += push;
        other.position -= push;
    }
}

on trigger_enter(zone: Entity) {
    if zone.name == "death_zone" { entity.kill(); }
    if zone.name == "checkpoint" { game.save(entity.position); }
}

// ─── Tick / frame events ──────────────────────────────────────
on tick(dt: f32) {
    entity.position += entity.velocity * dt;
}

on fixed_tick(dt: f32) {
    physics_step(entity, dt);  // fixed physics timestep
}

// ─── Lifecycle events ─────────────────────────────────────────
on spawn() {
    entity.health = 100;
    print(f"Enemy spawned at {entity.position}");
}

on death() {
    spawn_particles(entity.position, 20);
    drop_loot(entity.position, entity.loot_table);
}

on destroy() {
    entity.collider.remove();
}
```

### Implementation

1. Add `On` keyword to `token.rs` / `lexer.rs`
2. Add `EventHandler { event_type: String, params: Vec<Param>, body: Vec<Stmt> }` to `ast.rs`
3. Parse `on event_name(params) { body }` in `parser.rs`
4. Typechecker: register the handler with the event system. Verify parameter types match the event definition.
5. Lowering: desugar to a callback registration. `on tick(dt) { body }` becomes a function + registration call.
6. Built-in event types: `key_down`, `key_up`, `mouse_move`, `mouse_click`, `collision`, `trigger_enter`, `trigger_exit`, `tick`, `fixed_tick`, `spawn`, `death`, `destroy`, `custom(name)`.

### Why It's Better Than Alternatives

- **Unity:** Magic method names (`OnCollisionEnter`, `Update`, `Start`). No type safety on parameters. Can't add custom events without writing a system.
- **Node.js:** `addEventListener('click', handler)` — string-based, no type checking.
- **Rust:** No built-in event system. You use channels or observer crates.
- **Fides's advantage:** One keyword, type-safe parameters, compile-time verification, works with ECS (F18) and state machines (F19).

### Use Cases

- Game input handling (keyboard, mouse, gamepad)
- Physics callbacks (collisions, triggers)
- Game loop integration (tick, fixed_tick)
- Entity lifecycle (spawn, death, destroy)
- Custom game events (level complete, quest accepted, item picked up)
- UI events (click, hover, focus, blur)

---

## F21 — `@shader` — Same Language for CPU and GPU

**Classification:** SYNTAX — Tier A (compiler must emit SPIR-V/PTX for GPU — a library can't generate GPU bytecode)
**Status:** 🔲 Planned
**Impact:** Very High — game development, graphics, compute
**Novelty:** No general-purpose language lets you write shaders in the same language as game logic. You write GLSL/HLSL strings embedded in C++/Rust/C#. Fides compiles the same code to both x86 and SPIR-V, with full type checking and error messages.

### Concept

`@vertex`, `@fragment`, and `@kernel` annotations tell the compiler to
generate GPU code (SPIR-V for Vulkan, GLSL for OpenGL, PTX for CUDA)
instead of CPU code. The shader code has full type checking, autocompletion,
and error messages — because it's just Fides code.

```p
// ─── Vertex shader ────────────────────────────────────────────
@vertex
pf vert(in_pos: vec3, in_uv: vec2, in_normal: vec3) -> (vec4, vec2, vec3) {
    pv world_pos = model * vec4(in_pos, 1.0);
    pv clip_pos = view_proj * world_pos;
    pv world_normal = normalize(mat3(model) * in_normal);
    ret (clip_pos, in_uv, world_normal);
}

// ─── Fragment shader ──────────────────────────────────────────
@fragment
pf frag(uv: vec2, normal: vec3) -> vec4 {
    pv tex_color = texture(diffuse_map, uv);
    pv light_dir = normalize(vec3(0.5, 1.0, 0.3));
    pv diffuse = max(dot(normal, light_dir), 0.0);
    pv ambient = 0.2;
    pv color = tex_color.rgb * (diffuse + ambient);
    ret vec4(color, tex_color.a);
}

// ─── Compute shader: particle simulation on GPU ───────────────
@kernel(workgroup=[64])
pf particle_update(particles: Buffer<Particle>, dt: f32) {
    pv i = thread_id(0);
    if i >= particles.len { ret; }
    pv p = particles[i];
    p.velocity.y -= 9.81 * dt;
    p.position += p.velocity * dt;
    if p.position.y < 0.0 {
        p.position.y = 0.0;
        p.velocity.y *= -0.5;  // bounce
    }
    particles[i] = p;
}
```

### Implementation

1. Recognize `@vertex`, `@fragment`, `@kernel` annotations (parser already supports annotations)
2. In `typechecker.rs`: verify shader functions only use GPU-compatible operations (no file IO, no heap allocation, no recursive calls)
3. In `gpu.rs`: extend SPIR-V codegen to handle vertex/fragment shader stages (currently only compute shaders)
4. Generate SPIR-V for Vulkan, with GLSL fallback for OpenGL via SPIR-V cross-compilation
5. Shader uniforms (model, view_proj, diffuse_map) are auto-extracted from the function's free variables
6. Buffer types: `Buffer<T>` maps to GPU SSBO (Shader Storage Buffer Object)

### Why It's a Game Changer

- **C++ (OpenGL/Vulkan):** Shaders are GLSL strings. No type checking. No autocompletion. Errors at runtime.
- **Rust (wgpu):** Same — WGSL strings. Type errors are runtime.
- **Unity (Shader Graph):** Visual editor, not code. Limited expressiveness.
- **Fides's advantage:** One language, one type system, one set of error messages. Refactoring a struct used in both CPU and GPU code just works. No more "shader compile error at runtime" — it's a compile-time error.

### Use Cases

- 3D rendering (vertex + fragment shaders)
- Compute shaders (particle systems, image processing, physics)
- GPU-accelerated ML (custom kernels)
- Post-processing effects
- Procedural generation on GPU

---

## F22 — `scene { }` — Declarative Scene Hierarchy

**Classification:** ~~SYNTAX~~ → **STDLIB (S5)** — moved to `stdlib/scene.fd`. It's just entity spawning code (`Entity::new()` + `.add()` calls). No compiler support needed. Users can read, modify, and extend it. Keeping it as a keyword would bloat the language for no compiler benefit.
**Impact:** Medium — game development, UI
**Novelty:** Unity uses YAML (.unity files). Godot uses XML (.tscn files). Unreal uses binary (.umap). Fides uses code — the scene is a code block with embedded logic, type checking, and hot reload support.

### Concept

`scene` blocks declare a scene hierarchy declaratively. Entities are
nested, components are assigned, and event handlers can be embedded
directly. The scene desugars to entity spawning code at compile time.

```p
scene GameWorld {
    Camera {
        pos: vec3(0.0, 10.0, 20.0),
        target: vec3(0.0, 0.0, 0.0),
        fov: 60.0,
    }

    Light(Sun) {
        dir: normalize(vec3(-0.5, -1.0, -0.3)),
        color: vec3(1.0, 0.95, 0.8),
        intensity: 1.5,
    }

    Entity(Player) {
        Position { x: 0.0, y: 0.0, z: 0.0 },
        Velocity { dx: 0.0, dy: 0.0, dz: 0.0 },
        Health { hp: 100, max_hp: 100 },
        Sprite { texture: "player.png", frame: 0 },

        children: [
            Entity(Sword) {
                Position { x: 0.5, y: 0.0, z: 0.0 },
                Sprite { texture: "sword.png", frame: 0 },
            }
        ]

        on tick(dt: f32) {
            // Embedded logic — runs every frame for this entity
            if input.is_pressed(Key::Space) {
                player.velocity.dy = 5.0;
            }
        }
    }

    Entity(EnemySpawner) {
        Position { x: 50.0, y: 0.0, z: 50.0 },
        on tick(dt: f32) {
            if self.timer <= 0.0 {
                spawn_enemy(self.position);
                self.timer = 3.0;
            }
            self.timer -= dt;
        }
    }
}
```

### Implementation

1. Add `Scene` keyword to `token.rs` / `lexer.rs`
2. Add `SceneDef`, `SceneNode` to `ast.rs`
3. Parse `scene Name { NodeName { props, components, children, on handlers } }` in `parser.rs`
4. Typechecker: verify component assignments match component definitions. Verify child entities have valid types.
5. Lowering: desugar to a function that spawns entities, assigns components, and registers event handlers. Children inherit parent transforms.
6. Hot reload: when the scene block changes, re-spawn only the modified entities (leveraging Fides's existing hot reload infrastructure).

### Why Code Is Better Than Data Files

- **Type checking:** A typo in a component field is a compile error, not a silent runtime bug
- **Refactoring:** Renaming a component updates all scene references automatically
- **Logic embedding:** Event handlers can be inline — no separate script file
- **Version control:** Scenes are code → meaningful diffs, not binary blobs
- **Hot reload:** Change a scene value → see it instantly, no editor restart

### Use Cases

- Game level definition (entities, props, lights, cameras)
- UI layout (panels, buttons, labels with embedded event handlers)
- Prefab/template definition (reusable entity configurations)
- Procedural scene generation (code generates scene blocks)

---

## F23 — `@simd` — Auto-Vectorization Annotation

**Classification:** SYNTAX — Tier A (compiler must emit SSE/AVX/NEON vector instructions — a library can't generate machine code)
**Status:** 🔲 Planned
**Impact:** High — performance
**Novelty:** No annotation-based SIMD in any systems language. Rust has `std::simd` but it's a library with explicit types. Fides's `@simd` lets the compiler auto-vectorize existing loops.

### Concept

`@simd` tells the compiler to vectorize a loop — process multiple elements per CPU instruction using SSE (4x float), AVX (8x float), or NEON (ARM). No manual intrinsics needed.

```p
// ─── Vectorize a vertex transform loop ────────────────────────
@simd
pf transform_vertices(verts: Buffer<vec3>, mvp: mat4) {
    for i in 0..verts.len {
        verts[i] = (mvp * vec4(verts[i], 1.0)).xyz;
    }
    // Compiler generates SSE/AVX: processes 4 (or 8) vertices per iteration
}

// ─── SIMD with reduction (dot product) ────────────────────────
@simd(reduce=+)
pf dot_product(a: Buffer<f32>, b: Buffer<f32>) -> f32 {
    pv sum = 0.0;
    for i in 0..a.len {
        sum += a[i] * b[i];
    }
    sum  // compiler generates horizontal add at end
}

// ─── Game use: particle update (10,000 particles) ─────────────
@simd
pf update_particles(particles: Buffer<Particle>, dt: f32) {
    for i in 0..particles.len {
        particles[i].velocity.y -= 9.81 * dt;
        particles[i].position += particles[i].velocity * dt;
    }
    // 4x faster than scalar loop — no manual intrinsics
}
```

### Implementation

1. Recognize `@simd` annotation in `parser.rs`
2. In `opt.rs` or a new `vectorize.rs` pass: analyze the loop body for vectorizability (no loop-carried dependencies except explicit `reduce=`)
3. In `codegen_x64.rs`: emit SSE/AVX instructions (addps, mulps, etc.) for vectorized operations
4. In `codegen_arm64.rs`: emit NEON instructions
5. Fallback: if loop can't be vectorized, emit scalar code with a warning

### Why It Must Be Syntax

A library can't emit machine code. `@simd` requires the compiler to:
- Analyze loop dependencies (needs IR access)
- Rewrite the loop to process N elements per iteration (needs IR transformation)
- Emit vector instructions (needs codegen access)

### Use Cases

- Vertex transformation (3D rendering)
- Particle systems (10,000+ particles per frame)
- Image processing (pixel manipulation)
- Physics (broad-phase collision)
- ML inference (batched matrix multiply)
- Any hot loop over contiguous data

---

## F24 — `@crashfree` — Prove a Function Can't Panic

**Classification:** SYNTAX — Tier A (compiler must analyze entire call graph for panic safety — a library can't inspect itself)
**Status:** 🔲 Planned
**Impact:** Very High — Fides's killer feature for gaming
**Novelty:** No language can prove a function is crash-free at compile time. This is Fides's unique advantage for game development.

### Concept

`@crashfree` tells the compiler to verify that a function can never panic. The compiler checks:
- No `unwrap()` calls (must use `match` or `?.`)
- No array indexing without bounds check
- No division without a zero guard
- No calls to non-`@crashfree` functions
- No `panic!` or `assert!` in debug mode

If any of these are found, it's a **compile error**.

```p
// ─── Prove the game loop can't crash ──────────────────────────
@crashfree
pf update_position(entity: Entity, dt: f32) -> Entity {
    guard dt > 0.0 => return entity;  // guard prevents divide-by-zero
    Entity { ..entity, pos: entity.pos + entity.vel * dt }
}

// @crashfree propagates: calling a non-crashfree function = compile error
@crashfree
pf game_loop(dt: f32) {
    update_position(player, dt);  // OK: update_position is @crashfree

    // COMPILE ERROR: load_texture can panic (file not found)
    // load_texture("player.png");
    // Error: @crashfree function calls non-crashfree 'load_texture'
}

// The main loop is PROVABLY crash-free
@crashfree
pf main_loop() {
    loop {
        game_loop(get_delta_time());
    }
}
```

### Implementation

1. Recognize `@crashfree` annotation in `parser.rs`
2. In `typechecker.rs` or a new `crashfree.rs` pass: build call graph from the function
3. For each function in the call graph, check:
   - No `unwrap()` / `expect()` calls
   - No array indexing without preceding bounds check
   - No division where divisor might be zero (unless guarded by `given` or `guard`)
   - No `panic!` / `assert!` / `unreachable!`
   - All called functions are also `@crashfree`
4. If any violation → compile error with location

### Why It Must Be Syntax

A library can't inspect the call graph. `@crashfree` requires the compiler to:
- Trace every function call transitively (needs call graph)
- Analyze IR for potential panic points (needs IR access)
- Verify contracts (`given x != 0`) at call sites (needs typechecker)

### The "No Crash" Promise

| Crash Cause | How Fides Prevents It |
|-------------|-------------------|
| Null pointer dereference | No nulls — `Option<T>` + `?.` chaining (F8) |
| Array out of bounds | Compile-time bounds checking + runtime check |
| Use-after-free | Region system — compile-time detection |
| Double-free | Region system — compile-time detection |
| Data race in threads | Effect system — pure functions can't race |
| Divide by zero | `given x != 0` contracts (F10) |
| Unwrap on None | `@crashfree` forbids unwrap (F24) |
| Out of memory | `region with budget(N)` — compile-time guarantee (F3) |
| Subsystem crash | `supervise` — recover instead of dying (see below) |

**No other game engine language provides this.** Unity crashes from null refs. Unreal crashes from C++ memory errors. Godot crashes from GDScript type errors. Fides makes crashes a compile-time problem.

### Use Cases

- Game loop (prove it can't crash)
- Real-time systems (prove no panic in time-critical code)
- Embedded (prove no crash in firmware)
- Server (prove request handler can't crash the server)
- Any code where "crash" means "lost player progress"

---

## F25 — `@pool` — Zero-Allocation Object Pools

**Classification:** SYNTAX — Tier A (compiler must manage pool memory layout for zero overhead — a library has allocation overhead)
**Status:** 🔲 Planned
**Impact:** High — performance
**Novelty:** No language has compiler-managed object pools. Rust's `bumpalo` is a library. Fides's `@pool` lets the compiler manage the pool, eliminating allocation in hot loops.

### Concept

`@pool` pre-allocates a fixed-size pool of objects. Acquiring and releasing from the pool is O(1) with zero allocation — just flipping an `active` flag. This eliminates malloc/free overhead in hot loops (bullets, particles, enemies).

```p
// ─── Pre-allocate 10,000 bullets — no runtime allocation ──────
@pool(max_size=10000)
struct Bullet {
    pos: vec3,
    vel: vec3,
    damage: i32,
    active: bool,
}

// Spawn from pool — O(1), no malloc
pf fire_bullet(pos: vec3, dir: vec3) {
    pv bullet = Bullet::acquire();  // reuse dead bullet or allocate from pre-pool
    bullet.pos = pos;
    bullet.vel = dir * 100.0;
    bullet.active = true;
}

// Release back to pool — O(1), no free
system UpdateBullets(dt: f32) {
    query(Bullet) {
        entity.pos += entity.vel * dt;
        if entity.pos.y < 0 || entity.lifetime <= 0 {
            entity.active = false;
            Bullet::release(entity);  // back to pool, NOT freed
        }
    }
}
// 10,000 bullets, zero allocations after warmup
```

### Implementation

1. Recognize `@pool(max_size=N)` annotation in `parser.rs`
2. In `lower.rs` or a new `pool.rs` pass: generate a static pool buffer of `max_size` entries
3. Generate `acquire()` — find next inactive slot, mark active, return reference
4. Generate `release()` — mark slot inactive (no deallocation)
5. In `codegen`: emit the pool as a static array in the data section
6. Compile-time check: if `acquire()` is called when pool is full → compile warning (or runtime panic in debug, degrade in release)

### Why It Must Be Syntax

A library can do pooling (`bumpalo`, `typed-arena`), but:
- Library pools have allocation overhead for the pool itself
- Library pools can't be laid out in the binary's data section
- Library pools require explicit setup/teardown
- `@pool` lets the compiler place the pool in static memory, zero setup cost

### Use Cases

- Bullets (thousands per second, spawn/die constantly)
- Particles (10,000+ per frame)
- Enemies (spawn/despawn waves)
- Network packets (high-frequency send/receive)
- Any object with high spawn/despawn rate

---

## Stdlib Modules — Built in Fides, Shipped with Compiler

These modules are **not language keywords** — they're `.fd` files that ship with the compiler. Users can read the source, modify them, extend them, or replace them entirely. The compiler treats them like any other Fides code.

> **Philosophy:** A low-level language should give you sharp tools, not pre-built furniture. `vec3` + `@simd` + ECS + `@crashfree` are sharp tools. `tilemap` as a keyword is pre-built furniture. Provide the tools. Ship good furniture in the stdlib. Let users build their own if they want.

### S1 — `stdlib/tilemap.fd` — Tile Maps for 2D Games

**Classification:** STDLIB — built on `Vec<i32>` + `vec2` (F16) + ECS query (F18)
**Status:** ✅ Done

```p
use tilemap;

pv map = tilemap::Tilemap::new(256, 256, tile_size: 16);
map.fill("grass");
map.set(10, 5, "stone");     // place a stone tile
map.set_rect(0, 0, 10, 10, "water");  // fill a region

// Collision — built on the map data
if map.is_solid_at(player.pos) { /* can't walk here */ }

// Rendering — cull to camera view
pv visible = map.tiles_in_rect(camera.view_rect);
for tile in visible { draw_tile(tile); }
```

**Why stdlib not syntax:** It's arrays of tile IDs + collision checks. A `struct Tilemap { tiles: Vec<i32> }` with methods. Users can add custom tile types, auto-tiling rules, or replace it with their own implementation. No compiler support needed.

### S2 — `stdlib/sprite.fd` — Animation and Atlases

**Classification:** STDLIB — built on `struct` + `Vec<i32>` + timers
**Status:** ✅ Done

```p
use sprite;

pv sprite = sprite::Sprite::new("player.png", frame_size: vec2(32, 32));
sprite.add_anim("idle", frames: 0..=3, fps: 4, loop: true);
sprite.add_anim("walk", frames: 4..=11, fps: 8, loop: true);
sprite.play("walk");
sprite.flip_x = true;
sprite.update(dt);  // advance animation frame
draw_sprite(sprite);
```

**Why stdlib not syntax:** It's frame index math + a timer. A `struct Sprite { frames: Vec<i32>, fps: f32, current_frame: i32 }` with methods. Users can add palette swapping, skeletal animation, or custom blending. No compiler support needed.

### S3 — `stdlib/flowfield.fd` — RTS Pathfinding

**Classification:** STDLIB — built on `vec2` (F16) + grid math
**Status:** ✅ Done

```p
use flowfield;

pv field = flowfield::FlowField::compute(
    target: vec2(50, 50),
    obstacles: collision_grid,
    cost_grid: terrain_costs,
);

// 1000 units follow the same field — O(1) per unit
system MoveUnits(dt: f32) {
    query(Position, Velocity, Orders) {
        if entity.orders.current == MoveTo {
            pv flow = field.sample(entity.position);
            entity.velocity = flow.direction * entity.speed;
        }
    }
}
```

**Why stdlib not syntax:** It's grid math + vector field computation. A `struct FlowField { grid: Vec<vec2> }` with a compute method. Users can optimize for their specific game (different grid sizes, terrain weights, dynamic obstacles). No compiler support needed.

### S4 — `stdlib/streaming.fd` — Open World Chunk Loading

**Classification:** STDLIB — built on ECS (F18) + `HashMap` + `query`
**Status:** ✅ Done

```p
use streaming;

pv world = streaming::ChunkManager::new(
    chunk_size: 256,
    load_radius: 3,
    unload_radius: 5,
);

system UpdateStreaming {
    world.update(player.pos);  // load/unload chunks around player

    for chunk in world.newly_loaded {
        for spawn in chunk.npc_spawns { spawn_npc(spawn); }
    }
    for chunk in world.newly_unloaded {
        query(NPC, Position) {
            if world.chunk_of(entity.position) == chunk {
                entity.despawn();
            }
        }
    }
}
```

**Why stdlib not syntax:** It's chunk load/unload logic with a `HashMap<vec2, Chunk>`. Users can tune chunk sizes, add priority loading, implement async loading, or add custom chunk formats. No compiler support needed.

### S5 — `stdlib/scene.fd` — Declarative Scene Hierarchy

**Classification:** STDLIB — built on ECS (F18) + `on` events (F20). Was F22 (syntax), moved to stdlib.
**Status:** ✅ Done

```p
use scene;

// Scene is just a function that spawns entities
pv level = scene::load("levels/forest.scn");

// Or build programmatically
pv player = scene::entity("Player")
    .with(Position { x: 0, y: 0, z: 0 })
    .with(Health { hp: 100, max_hp: 100 })
    .with(Sprite { texture: "player.png", frame: 0 })
    .on_tick(|dt| { player.update(dt); })
    .spawn();
```

**Why moved from syntax to stdlib:** `scene { }` as a keyword was just syntactic sugar for `Entity::new()` + `.add()` calls. No compiler transformation needed. As a stdlib module, users can customize scene loading, add their own scene formats, or integrate with streaming (S4). The keyword would have locked everyone into one scene model.

### S6 — `stdlib/physics.fd` — Collision Detection

**Classification:** STDLIB — built on `vec3` (F16) + `struct` + ECS (F18)
**Status:** ✅ Done

```p
use physics;

pv world = physics::World::new(gravity: vec3(0, -9.81, 0));
world.add_body(player, physics::Body::dynamic(collider: physics::Capsule(0.5, 1.8)));
world.add_body(ground, physics::Body::static(collider: physics::Box(100, 1, 100)));
world.step(dt);  // integrate physics

pv contacts = world.contacts_for(player);
for contact in contacts {
    on_collision(contact.other, contact.point, contact.normal);
}
```

### S7 — `stdlib/audio.fd` — Audio Playback

**Classification:** STDLIB — built on FFI (`extern "C"`) to OS audio APIs
**Status:** ✅ Done (stub)

```p
use audio;

audio::init();
pv bgm = audio::load("music/theme.ogg");
bgm.play(loop: true);
bgm.set_volume(0.5);

pv sfx = audio::load("sfx/sword.wav");
sfx.play();  // one-shot
```

### S8 — `stdlib/input.fd` — Input Handling

**Classification:** STDLIB — built on `on` events (F20) + FFI to OS input APIs
**Status:** ✅ Done (stub)

```p
use input;

input::init();
input::bind_key(Key::W, "move_forward");
input::bind_key(Key::Space, "jump");

on input::event("move_forward") { player.move_forward(); }
on input::event("jump") { player.jump(); }
```

---

## Cut Features — Redundant, Build with Existing Features

These features were considered and rejected. Each one can be built using existing planned features with minimal code. Adding them as keywords would bloat the language for no compiler benefit.

### C1 — `fogofwar` — CUT

**Why cut:** Build with tilemap (S1) + ECS query (F18) in ~20 lines.

```p
// Fog of war = tilemap visibility grid + distance check per unit
pf update_fog(map: Tilemap, units: Vec<Entity>) {
    for y in 0..map.height {
        for x in 0..map.width {
            map.fog[x][y] = Explored;
            for unit in units {
                if distance(vec2(x, y), unit.pos) < unit.vision_range {
                    map.fog[x][y] = Visible;
                }
            }
        }
    }
}
```

### C2 — `selection` — CUT

**Why cut:** Build with ECS query (F18) + formation math. Group selection is just a rect query; formations are offset calculations.

```p
pv selected = query(Position, Owner) {
    where entity.owner == current_player
    where select_box.contains(entity.position)
};
pv formation = Formation::grid(selected.len(), spacing: 2.0);
```

### C3 — `traffic` — CUT

**Why cut:** Build with ECS (F18) + flowfield (S3) + state machines (F19). Cars are just entities with AI state machines on a road flowfield.

```p
component Car { route: FlowField, speed: f32 }
system Traffic(dt) {
    query(Position, Velocity, Car) {
        pv flow = entity.car.route.sample(entity.position);
        entity.velocity = flow.dir * entity.car.speed;
    }
}
```

### C4 — `lod` — CUT

**Why cut:** Build with a distance check + model swap. 4 lines of code, not a keyword.

```p
system UpdateLOD {
    query(Position, Renderable) {
        pv dist = distance(entity.position, camera.pos);
        entity.model = if dist < 50 { "high" } else if dist < 200 { "med" } else { "low" };
    }
}
```

### C5 — `snapshot` — CUT

**Why cut:** Merge into `persist` (F2). Instead of a separate snapshot feature, extend persist with auto-save intervals: `persist every 30s { player, world }`. One feature, not two.

### C6 — `scene { }` keyword — CUT

**Why cut:** Moved to stdlib/scene.fd (S5). It's just entity spawning code — `Entity::new()` + `.add()` calls. No compiler transformation needed. As a stdlib module, users can customize it. As a keyword, it would lock everyone into one scene model.

---

## Summary

### Implemented Features

| Feature | Status | Impact | What It Enables |
|---------|--------|--------|----------------|
| F1: `sandbox { }` | ✅ Done | High | Compile-time untrusted code sandboxing |
| F2: `persist` | ✅ Done | High | State preservation across hot reloads |
| F3: Memory budgets | ✅ Done | High | Compile-time memory guarantees for embedded |
| F4: `watch --diff` | ✅ Done | High | Visible optimizer behavior |
| F5: Cap-aware linking | ✅ Done | High | Smaller binaries, proven attack surface |

### Language Syntax — Tier A (Compiler Must Understand)

| Feature | Status | Impact | Why Syntax |
|---------|--------|--------|------------|
| F8: `?.` chaining | ✅ Implemented | High | Typechecker narrows types through chain |
| F10: `given`/`ensure` | 🔲 Planned | High | Typechecker verifies at all call sites |
| F13: `parallel { \| }` | 🔲 Planned | High | Compiler emits thread spawn/join |
| F14: `@differentiable` | 🔲 Planned | High | Compiler traces IR for autograd |
| F16: vec/mat types | 🔲 Planned | High | Compiler emits SIMD instructions |
| F17: swizzle `.xyz` | 🔲 Planned | High | Parser + typechecker for components |
| F18: ECS syntax | 🔲 Planned | Very High | Compiler controls SoA memory layout |
| F21: `@shader` | 🔲 Planned | Very High | Compiler emits SPIR-V for GPU |
| F23: `@simd` | 🔲 Planned | High | Compiler vectorizes loops to SSE/AVX |
| F24: `@crashfree` | ✅ Implemented | Very High | Compile-time panic prevention: rejects unwrap, p_panic, non-const div/mod, while loops, non-crashfree calls |
| F25: `@pool` | 🔲 Planned | High | Compiler manages pool memory, zero alloc |

### Language Syntax — Tier B (Ergonomic, Worth the Keyword)

| Feature | Status | Impact | Why Syntax |
|---------|--------|--------|------------|
| F6: `guard =>` | ✅ Implemented | High | Used constantly, ugly as function |
| F7: `..` spread | ✅ Implemented | High | Used constantly, ugly as function |
| F9: `??` coalescing | ✅ Implemented | Medium | Used constantly, ugly as function |
| F11: `is` type check | ✅ Implemented | Medium | Type narrowing in branches |
| F12: `@memo` | 🔲 Planned | High | Purity verified by effect system |
| F15: `@no_grad` | ❌ Cut | — | Unnecessary — autograd is opt-in |
| F19: `state` machines | 🔲 Planned | High | Compile-time transition verification |
| F20: `on` events | 🔲 Planned | High | Unified dispatch + parameter verification |

### Stdlib Modules (Built in Fides, Shipped with Compiler)

| Module | Status | What It Provides |
|--------|--------|-----------------|
| S1: `tilemap.fd` | ✅ Done | Tile maps, collision, auto-tiling |
| S2: `sprite.fd` | ✅ Done | Animation, atlases, palette swap |
| S3: `flowfield.fd` | ✅ Done | RTS pathfinding, vector fields |
| S4: `streaming.fd` | ✅ Done | Chunk load/unload for open worlds |
| S5: `scene.fd` | ✅ Done | Declarative entity spawning (was F22) |
| S6: `physics.fd` | ✅ Done | Collision detection, rigid bodies |
| S7: `audio.fd` | ✅ Done (stub) | Audio playback via FFI (no actual hardware) |
| S8: `input.fd` | ✅ Done (stub) | Input handling on top of `on` events (no actual hardware) |

### Cut Features (Redundant)

| Feature | Status | Why Cut |
|---------|--------|---------|
| C1: fogofwar | ❌ Cut | Build with tilemap (S1) + query (F18) |
| C2: selection | ❌ Cut | Build with query (F18) + formation math |
| C3: traffic | ❌ Cut | Build with ECS (F18) + flowfield (S3) + state (F19) |
| C4: lod | ❌ Cut | Build with distance check + model swap |
| C5: snapshot | ❌ Cut | Merge into persist (F2): `persist every 30s { }` |
| C6: scene keyword | ❌ Cut | Moved to stdlib/scene.fd (S5) |

### The Story

> Fides is the only language where you can sandbox untrusted code at compile
> time, guarantee memory budgets for embedded, persist state across hot
> reloads, see what the optimizer changed, and strip untrusted code from
> your binary — all verified by the compiler, not hoped for at runtime.
>
> With Tier A syntax (F8, F10, F13–F18, F21, F23–F25), Fides gives the compiler
> the power to emit SIMD instructions, manage ECS memory layout, trace
> autograd graphs, generate GPU shaders, vectorize loops, prove crash-free
> code, and manage object pools — things no library can do.
>
> With Tier B syntax (F6, F7, F9, F11, F12, F19, F20), Fides makes common
> patterns beautiful: guard clauses, spread operators, optional chaining,
> contracts, memoization, state machines, and event handlers — all native
> syntax, not library boilerplate.
>
> With the stdlib (S1–S8), Fides ships game systems as readable Fides code:
> tilemaps, sprites, flowfields, streaming, scenes, physics, audio, input.
> Users can read every line, modify anything, or replace it entirely.
>
> Fides follows the Rust/Zig philosophy: small language, rich stdlib, infinite
> extensibility. Only make it syntax when the compiler must do something
> a library can't. Everything else is Fides code that users control.

### Implementation Details

All 5 original features (F1–F5) were implemented in a single session. Test results (current state):
- Interpreter: 105/130 passed (+ 22/22 expected-fail)
- PE Emitter: 105/130 valid (+ 22/22 expected-fail)
- C Codegen: 104/130 matched — near full parity (+ 22/22 expected-fail)
- Total: 152 .fd test files (130 normal + 22 expected-fail)

**Planned for implementation:**
- 39 Tier A syntax features (F10, F13, F14, F16–F18, F21, F23–F60) — need compiler support
- 7 Tier B syntax features (F12, F19, F20) — ergonomic keywords
- 29 stdlib module slots (S1–S29) — Fides code, no compiler changes (34 actual .fd files as of Aug 2026)
- 15 features cut (C1–C15) — redundant, build with existing features

---

## F38 — `money` Type — First-Class Financial Type

**Status:** 🔲 Planned
**Impact:** Critical — prevents overflow, truncation, floating-point errors in financial code
**Novelty:** No mainstream language has a dedicated money type with conservation rules
**Industries:** Banking, finance, payments, blockchain, insurance

### Concept

`money` is a first-class type with compile-time safety rules that make
financial bugs impossible at the type level:

```p
// money is NOT just a number — it's a TYPE with dimensional rules
pv balance: money(18, 2) = $1000.00    // 18 digits precision, 2 decimal places

// Legal: money + money = money
balance + $500.00

// Legal: money × scalar = money
balance * 3

// Legal: money ÷ scalar = money (with explicit rounding mode)
balance / 12  // requires rounding annotation: balance / 12 round_down

// ILLEGAL (compile error): money × money = money² (dimensionally wrong)
balance * balance    // COMPILE ERROR: cannot multiply money × money

// ILLEGAL (compile error): money ÷ money = dimensionless
balance / balance    // COMPILE ERROR: use money_to_ratio() for explicit conversion

// ILLEGAL (compile error): no implicit float conversion
balance as f64       // COMPILE ERROR: money cannot be implicitly converted to float

// Overflow: runtime abort (NOT silent truncation like COBOL)
pv result: money(18, 2) = balance + $99999999999999.99
// If overflow → transaction aborts, NOT silent corruption
```

### Why It Matters

| Bug class | COBOL | Fides with `money` type |
|-----------|-------|---------------------|
| Silent truncation | `PIC 9(8)` overflows → wraps to 0 | Compile error or runtime abort |
| Float rounding | `COMP` float → 0.1 + 0.2 = 0.30000000000000004 | `money` uses integer cents, no FP |
| Dimensional errors | `BALANCE * RATE` (money × money?) | `balance * rate_bps` (money × scalar = money) |
| Field too small | `PIC X(20)` truncates long names | `money` precision is explicit, overflow = error |

### Implementation

1. Add `Money` type to type system (`typechecker.rs`) — parameterized by precision and scale
2. Lower to `i128` (128-bit integer) in IR — cents representation
3. Typechecker enforces: money ± money = money, money × scalar = money, money ÷ scalar = money
4. Typechecker rejects: money × money, money ÷ money, money → f64
5. Overflow check: runtime abort on overflow (not silent)
6. C codegen: emit `__int128` with overflow checks
7. Interpreter: track money type, abort on overflow

---

## F39 — `@conserve` — Compile-Time Money Conservation Proof

**Status:** 🔲 Planned
**Impact:** Critical — the killer feature for banking
**Novelty:** No language in the world can prove money conservation at compile time
**Industries:** Banking, finance, payments, blockchain, insurance

### Concept

`@conserve` makes the compiler verify that money is conserved across every
code path — like Rust's borrow checker, but for money:

```p
@conserve  // Compiler proves: money in = money out
pf transfer(
    from: &mut Account,
    to: &mut Account,
    fee_account: &mut Account,
    amount: money,
) -> Result<money, Error> {
    let fee = calculate_fee(amount);    // fee is money
    
    // Compiler tracks:
    //   Input:  amount (parameter)
    //   Output: from -= (amount + fee), to += amount, fee_account += fee
    //   Check:  -(amount + fee) + amount + fee = 0  ✓ CONSERVED
    
    from.debit(amount + fee)?;
    to.credit(amount)?;
    fee_account.credit(fee)?;
    
    ret fee    // return value is money, tracked in conservation
}

// COMPILE ERROR — money not conserved:
@conserve
pf bad_transfer(from: &mut Account, to: &mut Account, amount: money) {
    from.debit(amount)?;
    to.credit(amount / 2)?;    // ERROR: debited amount, credited amount/2
                                // Missing: amount/2 — where did it go?
}
```

### Why It Matters

This is **double-entry bookkeeping enforced at compile time.** Every debit
must have a matching credit. The salami attack (stealing rounding errors)
becomes impossible because the compiler notices the missing half-cent.

| Guarantee | COBOL | Fides with `@conserve` |
|-----------|-------|---------------------|
| Debits = credits | Manual (programmer must remember) | Compiler-enforced (can't forget) |
| Rounding theft | Possible (salami attack) | Impossible (compiler detects missing amount) |
| Copy-paste bugs | Common (forgot to credit one side) | Impossible (compiler detects imbalance) |

### Implementation

1. Track money flow in typechecker: every `money` parameter, return, debit, credit
2. Build a flow graph: where does each money unit enter and exit?
3. For each code path: verify sum(inputs) = sum(outputs)
4. If any path doesn't conserve → compile error with explanation
5. This is analogous to Rust's borrow checker but tracks money, not memory

---

## F40 — `txn { }` — Language-Level Atomic Transactions

**Status:** 🔲 Planned
**Impact:** Critical — prevents partial transactions (the #1 cause of financial inconsistency)
**Novelty:** Most languages rely on database transactions; Fides makes it a language construct
**Industries:** Banking, finance, payments, blockchain, any transactional system

### Concept

```p
pf transfer(from: &mut Account, to: &mut Account, amount: money) -> Result<(), Error> {
    txn {
        // Everything inside txn { } is atomic:
        // - If ANY operation fails → ALL changes roll back
        // - If the program CRASHES → all changes roll back
        // - The compiler verifies every operation is reversible
        
        from.debit(amount)?;      // if this fails → nothing happened
        to.credit(amount)?;       // if this fails → from.debit is rolled back
        log.transfer(from.id, to.id, amount)?;  // if this fails → both rolled back
    }
    // After txn { } block completes → changes are committed atomically
    ret Ok(())
}

// COMPILE ERROR — irreversible operation inside txn:
fn bad_transfer(from: &mut Account, to: &mut Account, amount: money) {
    txn {
        from.debit(amount);
        send_email(from.email, "Transfer sent");  // ERROR: email is irreversible
        to.credit(amount);
    }
}
```

### Why It Matters

| Bug class | COBOL + CICS | Fides with `txn { }` |
|-----------|-------------|-------------------|
| Partial transaction (crash between debit and credit) | CICS rolls back (runtime) | Compiler prevents irreversible ops + auto-rollback |
| Irreversible side effect in transaction | No check (programmer must know) | Compile error |
| Transaction left open | CICS timeout (minutes) | `txn { }` scope-bounded (can't be left open) |

### Implementation

1. Add `Txn` block to AST (`ast.rs`)
2. Typechecker: verify every operation inside `txn { }` is reversible (no IO, no Net, no irreversible side effects)
3. Lowering: emit write-ahead log entries before each modification
4. Runtime: on panic/error, replay log in reverse to restore state
5. Commit: flush log atomically (fsync)

---

## F41 — Linear Money — Money Can't Be Duplicated

**Status:** 🔲 Planned
**Impact:** High — prevents accidental money duplication at the type level
**Novelty:** Rust has ownership for memory; Fides has linear types for money
**Industries:** Banking, finance, blockchain

### Concept

```p
// Money is LINEAR — can be moved, not copied
pv balance: money = $1000.00

pv payment1: money = balance    // OK — balance is MOVED (consumed)
pv payment2: money = balance    // COMPILE ERROR: balance already moved

// For account balances (need repeated reads):
struct Account {
    balance: money_ref,    // reference to money, can be read but not copied
}

pf debit(account: &mut Account, amount: money) -> Result<(), Error> {
    // amount is CONSUMED here — caller can't use it again
    account.balance -= amount;
    ret Ok(())
}
```

### Why It Matters

Prevents the class of bugs where money is accidentally duplicated by copying
a balance variable and using both copies. In COBOL, `MOVE BALANCE TO TEMP`
creates a copy — both `BALANCE` and `TEMP` hold the same value, and using
both means the money was effectively doubled.

### Implementation

1. Add `Linear` annotation to type system for `money` values
2. Typechecker: `money` variables are move-only (like Rust's `Copy` vs `Move`)
3. `money_ref` (account balance) is a read-only view that can't be moved
4. Only `debit`/`credit` can modify `money_ref`, and they consume their `money` parameter

---

## F42 — `Account<State>` — State-Safe Account Types

**Status:** 🔲 Planned
**Impact:** High — prevents operations on accounts in wrong state
**Novelty:** Type-state pattern applied to banking (originally from Rust's type-state)
**Industries:** Banking, finance, insurance

### Concept

```p
enum AccountState { Pending, Active, Frozen, Closed }

struct Account<state: AccountState> {
    id: i64,
    balance: money,
}

// transfer only works on Active accounts
pf transfer(from: &mut Account<Active>, to: &mut Account<Active>, amount: money) -> Result<(), Error> { ... }

// COMPILE ERROR: can't transfer from closed account
pf bad_transfer(from: &mut Account<Closed>) {
    transfer(from, ...);  // ERROR: expected Account<Active>, got Account<Closed>
}

// State transitions are explicit and verified
pf close_account(account: Account<Active>) -> Account<Closed> {
    guard account.balance == $0.00 => ret Error("Balance must be zero");
    ret Account<Closed> { ...account }
}
```

### Why It Matters

In COBOL, account status is a field (`STATUS = 'C'`). The programmer must
check it before every operation. If they forget, you can transfer from a
closed account. With `Account<State>`, the TYPE prevents it — you can't
even call `transfer` on a `Closed` account.

### Implementation

1. Requires const generics (already partially implemented in Fides)
2. Typechecker: `Account<Active>` and `Account<Closed>` are different types
3. State transition functions consume old state, produce new state
4. No runtime check needed — type system prevents misuse

---

## F43 — `LogicalTime` — Clock-Free Time Type

**Status:** 🔲 Planned (stdlib deliverable, not a compiler feature)
**Impact:** High — prevents timezone, clock-skew, and leap-year bugs
**Novelty:** Most languages use system clock; Fides provides logical time as explicit parameter
**Industries:** Banking, finance, medical, aerospace, blockchain
**Implementation note:** This is a **stdlib struct** (`struct LogicalTime { nanos: i64 }` with methods), not a compiler feature. It will be implemented in Phase 7.5A alongside `@deterministic`, since its only purpose is to give `@deterministic` functions a way to do time math without wall clock access. A type alias (`ty LogicalTime = i64`) would provide zero safety — the compiler must treat it as a distinct type.

### Concept

```p
// LogicalTime is a pure time type — no timezone, no system clock
@deterministic
pf calculate_interest(
    principal: money,
    rate_bps: i64,
    start: LogicalDate,
    end: LogicalDate,
) -> money {
    let days = date_diff(start, end);  // exact, timezone-free
    ret (principal * rate_bps * days) / (10000 * 365)
}

// ILLEGAL inside @deterministic:
let now = system_clock();  // COMPILE ERROR: system clock not allowed
```

### Why It Matters

| Bug class | COBOL | Fides with `LogicalTime` |
|-----------|-------|----------------------|
| Timezone error | System clock is local time → off by one day | `LogicalDate` has no timezone |
| Clock skew | Different servers have different clocks | Time is a parameter, not read from OS |
| Leap year | Manual handling (often wrong) | `date_diff` handles leap years correctly |
| Reproducibility | Same calculation may differ across servers | Same inputs → same output, always |

### Implementation

1. Add `LogicalDate` and `LogicalTime` types to stdlib
2. `date_diff` uses proleptic Gregorian calendar (handles leap years)
3. No timezone — dates are absolute
4. `@deterministic` rejects `system_clock()`, `system_now()`, etc.
5. Time must be passed as parameter, not read from environment

---

## F44 — `@safety-critical` — DO-178C / ISO 26262 Coding Rules

**Status:** 🔲 Planned
**Impact:** Critical for aerospace and automotive
**Novelty:** No language enforces safety-critical coding standards at compile time
**Industries:** Aerospace, automotive, medical, nuclear, rail

### Concept

```p
@safety-critical  // Compiler enforces DO-178C / ISO 26262 rules
pf flight_control_loop(
    sensor_data: &SensorData,
    controls: &mut Controls,
) -> Result<(), Error> {
    // Rules enforced by compiler:
    // 1. No dynamic memory allocation (no heap, no malloc, no Vec::push)
    // 2. No recursion (all loops must have bounded iteration count)
    // 3. No unbounded loops (compiler proves termination)
    // 4. No floating point unless explicitly approved (@bit-exact required)
    // 5. No pointers to pointers (max 1 level of indirection)
    // 6. No function pointers (no indirect calls)
    // 7. All variables initialized before use
    // 8. All errors handled (no ignored Result)
    // 9. No IO except through approved HAL functions
    // 10. Cyclomatic complexity ≤ 15 per function
    
    let altitude = sensor_data.altitude;
    guard altitude > 0 => ret Error("Invalid altitude");
    
    // Bounded loop — compiler verifies max iterations
    for i in 0..10 {
        controls.adjust(sensor_data.channels[i]);
    }
    
    ret Ok(())
}
```

### Why It Matters

Today, DO-178C and ISO 26262 compliance is verified by **external tools**
(LDRA, Polyspace, Coverity) that run AFTER coding. They find violations
late in the process. Fides enforces the rules **at compile time** — violations
are compile errors, not late-stage audit findings.

| Standard | Domain | Fides enforcement |
|----------|--------|---------------|
| DO-178C | Aerospace | @safety-critical + @dal-a |
| ISO 26262 | Automotive | @safety-critical + @autosar |
| IEC 62304 | Medical | @medical |
| IEC 60880 | Nuclear | @safety-critical + @redundant |
| EN 50128 | Rail | @safety-critical + @bounded |

### Implementation

1. `@safety-critical` activates a set of typechecker restrictions
2. No heap allocation (region must be Stack or Static)
3. No recursion (typechecker detects call cycles)
4. All loops must have constant or provably-bounded trip count
5. No function pointers (typechecker rejects indirect calls)
6. All `Result` must be handled (no `let _ = ...`)
7. Cyclomatic complexity counter in typechecker

---

## F45 — `@redundant` — Dual-Channel Redundant Execution

**Status:** 🔲 Planned
**Impact:** Critical for aerospace flight control
**Novelty:** No language generates redundant execution automatically
**Industries:** Aerospace, defense, nuclear, rail

### Concept

```p
@redundant  // Compiler generates TWO copies of this function on separate cores
@crashfree
pf compute_pitch_command(
    sensor_data: &SensorData,
) -> i32 {
    let pitch = sensor_data.alpha * 100 + sensor_data.q * 50;
    ret pitch.clamp(-1000, 1000)
}

// Compiler generates:
// 1. Core A executes compute_pitch_command_channel_a()
// 2. Core B executes compute_pitch_command_channel_b() (identical code)
// 3. Voter compares outputs: if they match → output. If not → fail-safe.
// 4. If either channel crashes → @crashfree prevents this, but voter handles it
```

### Why It Matters

Flight control computers use dual/triple redundant channels. Today this is
done with **two separate codebases** written by **two separate teams** (DO-178C
requires this for DAL-A). Fides can generate both channels from one codebase,
with the compiler ensuring they're identical but independently scheduled.

### Implementation

1. `@redundant` duplicates the function in codegen
2. Each copy runs on a different core (compiler emits core affinity)
3. Voter function is auto-generated: compares outputs, triggers fail-safe on mismatch
4. Requires multi-core target support in codegen

---

## F46 — `@bounded` — Prove All Loops Terminate

**Status:** 🔲 Planned
**Impact:** Critical for safety-critical systems
**Novelty:** Most languages can't prove loop termination; Fides can
**Industries:** Aerospace, automotive, medical, nuclear, rail, robotics

### Concept

```p
@bounded  // Compiler proves every loop in this function terminates
pf process_packets(buffer: &Buffer) -> Result<(), Error> {
    // OK — constant trip count
    for i in 0..10 {
        process(buffer.get(i)?);
    }
    
    // OK — provably decreasing iterator
    let mut n = buffer.len();
    while n > 0 {
        process(buffer.get(buffer.len() - n)?);
        n -= 1;  // compiler proves n decreases
    }
    
    // COMPILE ERROR — unbounded loop
    while buffer.has_data() {
        process(buffer.read()?);
        // ERROR: cannot prove termination — has_data() may never return false
    }
}
```

### Why It Matters

Unbounded loops are the #1 cause of missed deadlines in safety-critical
systems. WCET (Worst-Case Execution Time) analysis requires that all loops
have a known maximum iteration count. Today this is verified by external
tools (aiT, Bound-T). Fides enforces it at compile time.

### Implementation

1. Typechecker analyzes every loop: for, while
2. `for` with constant range → automatically bounded
3. `while` with decreasing iterator → prove termination via ranking function
4. `while` with external condition → compile error unless `max_iterations` annotation
5. Combined with `@gas` for total execution cost bound

---

## F47 — `@partitioned` — Spatial and Temporal Partitioning

**Status:** 🔲 Planned
**Impact:** Critical for avionics (ARINC 653)
**Novelty:** No language enforces partitioning at compile time
**Industries:** Aerospace, automotive, medical, multi-core safety

### Concept

```p
@partitioned  // Compiler enforces no shared mutable state between components
pf avionics_system() {
    partition flight_control {
        // Flight control code — own memory region, own time slice
        // Cannot access navigation's memory
        // Cannot run during navigation's time slice
        control_loop();
    }
    
    partition navigation {
        // Navigation code — own memory region, own time slice
        // Cannot access flight_control's memory
        // Cannot run during flight_control's time slice
        nav_update();
    }
    
    // Communication only through typed ports (ARINC 653 queuing/sampling ports)
    port nav_to_control: Port<NavData>;
}
```

### Why It Matters

ARINC 653 requires that avionics components are spatially partitioned (can't
access each other's memory) and temporally partitioned (can't run during
each other's time slices). Today this is enforced by the RTOS (VxWorks,
INTEGRITY). Fides can enforce it at compile time — the compiler prevents any
shared mutable state between partitions.

### Implementation

1. Add `partition` blocks to AST
2. Typechecker: each partition gets its own region (spatial isolation)
3. No shared mutable references between partitions (effect system + region system)
4. Communication only through `Port<T>` (typed, copy-only, no shared memory)
5. Codegen: emit partition scheduling table for ARINC 653 RTOS

---

## F48 — `@medical` — IEC 62304 Medical Device Rules

**Status:** 🔲 Planned
**Impact:** Critical for medical devices
**Novelty:** No language enforces IEC 62304 at compile time
**Industries:** Medical devices, healthcare

### Concept

```p
@medical  // Compiler enforces IEC 62304 rules
@audit    // Auto-instrument every function for FDA audit trail
pf drug_dosage_calculator(
    patient_weight_kg: i64,    // integer, not float (no FP in dosage calc)
    drug_mg_per_kg: i64,
) -> Result<i64, Error> {
    // IEC 62304 rules enforced:
    // 1. No dynamic memory allocation (static allocation only)
    // 2. No recursion (all paths bounded)
    // 3. No floating point (integer math only for dosage)
    // 4. Every function call logged (@audit)
    // 5. Every error handled (no ignored Result)
    // 6. No uninitialized variables
    // 7. No undefined behavior
    // 8. All inputs range-checked
    
    guard patient_weight_kg > 0 && patient_weight_kg < 500 => ret Error("Invalid weight");
    guard drug_mg_per_kg > 0 && drug_mg_per_kg < 1000 => ret Error("Invalid dosage");
    
    let dose_mg = patient_weight_kg * drug_mg_per_kg;
    guard dose_mg < 10000 => ret Error("Dose exceeds safety limit");
    
    ret dose_mg
}
```

### Why It Matters

Medical device software must comply with IEC 62304 (US) or MDR (EU).
Today, compliance is verified by external auditors who manually review
code. Fides enforces the rules at compile time — violations are compile errors,
not audit findings months later.

| IEC 62304 Requirement | Fides Enforcement |
|----------------------|---------------|
| No dynamic allocation in safety-critical | `@medical` rejects heap allocation |
| All errors handled | Typechecker rejects ignored `Result` |
| No uninitialized data | Typechecker requires initialization |
| Traceability (every requirement → code) | `@audit` auto-generates traceability log |
| No undefined behavior | Fides has no UB (safe language by design) |
| Risk analysis | `@deterministic` proves no unexpected side effects |

### Implementation

1. `@medical` activates: no heap, no recursion, no FP, all errors handled, all inputs range-checked
2. Combined with `@audit` for automatic FDA-compliant audit trail
3. Combined with `@deterministic` for provably reproducible calculations
4. Combined with `@crashfree` for proving no panics in life-critical paths

---

## F52 — `@autosar` — AUTOSAR Coding Guidelines

**Status:** 🔲 Planned
**Impact:** Critical for automotive ECU software
**Novelty:** No language enforces AUTOSAR C++14 guidelines at compile time
**Industries:** Automotive

### Concept

```p
@autosar  // Compiler enforces AUTOSAR coding guidelines
pf engine_control(
    sensors: &SensorData,
    actuators: &mut Actuators,
) -> Result<(), Error> {
    // AUTOSAR rules enforced:
    // 1. No dynamic allocation after startup (static allocation only)
    // 2. No recursion (bounded call depth)
    // 3. No multiple inheritance (not applicable in Fides — no classes)
    // 4. No exceptions (Fides uses Result — already compliant)
    // 5. No goto (Fides has no goto — already compliant)
    // 6. All parameters named and typed
    // 7. No magic numbers (use pc constants)
    // 8. Cyclomatic complexity ≤ 15
    // 9. No pointer arithmetic (Fides has no raw pointers in safe mode)
    // 10. Fixed-point math preferred over FP
    
    let rpm = sensors.rpm;
    guard rpm < MAX_RPM => ret Error("RPM exceeds limit");
    
    actuators.set_throttle(calculate_throttle(rpm, sensors.load));
    ret Ok(())
}
```

### Why It Matters

AUTOSAR is the standard for automotive ECU software. Today, compliance is
checked by static analysis tools (Axivion, Helix QAC) that run AFTER coding.
Fides enforces AUTOSAR rules at compile time.

### Implementation

1. `@autosar` activates: no heap post-startup, no recursion, complexity ≤ 15, no magic numbers
2. Combined with `@safety-critical` for ISO 26262 compliance
3. Combined with `@can-bus` for CAN frame generation
4. Combined with `@calibratable` for tunable parameters

---

## F53 — `@can-bus` — CAN Frame Encoding with Compile-Time ID Verification

**Status:** 🔲 Planned
**Impact:** High for automotive
**Novelty:** No language generates CAN frames with compile-time ID verification
**Industries:** Automotive, heavy machinery, robotics

### Concept

```p
@can-bus  // Compiler generates CAN frame encoding/decoding
pf send_engine_telemetry(ecu: &mut ECU, rpm: i32, temp: i32) -> Result<(), Error> {
    // Compiler generates CAN frame:
    // - ID: 0x0C0 (engine telemetry, verified at compile time — no collision)
    // - DLC: 8 bytes
    // - Byte 0-3: rpm (i32, little-endian)
    // - Byte 4-7: temp (i32, little-endian)
    can_send(0x0C0, struct { rpm, temp });
    ret Ok(())
}

// COMPILE ERROR — CAN ID collision:
@can-bus
pf send_brake_status(ecu: &mut ECU, pressure: i32) -> Result<(), Error> {
    can_send(0x0C0, struct { pressure });  // ERROR: ID 0x0C0 already used by send_engine_telemetry
}
```

### Why It Matters

CAN bus IDs must be unique across the vehicle. Today, ID collisions are
found during integration testing (when two ECUs talk over the same ID).
Fides verifies uniqueness at compile time.

### Implementation

1. `@can-bus` registers CAN IDs in a compile-time table
2. Typechecker verifies no ID collisions across all `@can-bus` functions
3. Codegen generates CAN frame encoding (pack struct into 8-byte frame)
4. Stdlib `can.fd` provides CAN HAL (socketCAN on Linux, direct register on bare metal)

---

## F56 — `@dal-a` — DO-178C Design Assurance Level A

**Status:** 🔲 Planned
**Impact:** Critical for aerospace (highest safety level)
**Novelty:** No language enforces DAL-A at compile time
**Industries:** Aerospace, defense

### Concept

```p
@dal-a  // Highest safety level — compiler enforces strictest rules
@redundant  // Dual-channel execution
@crashfree  // Proven no panic
@bounded    // All loops terminate
pf flight_control_primary(
    sensors: &SensorData,
    controls: &mut FlightControls,
) -> Result<(), Error> {
    // DAL-A rules enforced:
    // 1. No dynamic allocation (ever — not even at startup)
    // 2. No recursion (ever)
    // 3. No interrupts (polling only — deterministic timing)
    // 4. No floating point (unless @bit-exact is also applied)
    // 5. No function pointers (no indirect calls)
    // 6. No heap (all memory static)
    // 7. All loops bounded (@bounded)
    // 8. All errors handled
    // 9. Dual-channel execution (@redundant)
    // 10. Full traceability (@audit)
    // 11. MC/DC coverage required (compiler generates test obligations)
    
    let pitch = sensors.alpha * 100;
    let roll = sensors.beta * 50;
    
    controls.set_elevator(pitch.clamp(-1000, 1000));
    controls.set_ailerons(roll.clamp(-1000, 1000));
    
    ret Ok(())
}
```

### Why It Matters

DAL-A is the highest safety level in DO-178C — required for flight control
systems where failure causes catastrophic loss. Today, DAL-A compliance
requires millions of dollars in verification effort. Fides enforces the coding
rules at compile time, reducing verification cost.

### Implementation

1. `@dal-a` is the strictest combination: `@safety-critical` + `@redundant` + `@crashfree` + `@bounded` + `@audit`
2. Additional rules: no interrupts (polling only), no FP without `@bit-exact`
3. MC/DC (Modified Condition/Decision Coverage) — compiler generates test obligations
4. Requires all 10 rules satisfied simultaneously

---

## F59 — `@grid-safe` — IEC 61850 Grid Safety

**Status:** 🔲 Planned
**Impact:** High for energy/smart grid
**Novelty:** No language enforces IEC 61850 timing rules at compile time
**Industries:** Energy, power grid, smart grid, renewable energy

### Concept

```p
@grid-safe  // Compiler enforces IEC 61850 rules
@realtime   // WCET analysis, no malloc
pf protection_relay(
    current: &SensorData,
    voltage: &SensorData,
    breaker: &mut Breaker,
) -> Result<(), Error> {
    // IEC 61850 rules enforced:
    // 1. Bounded response time (≤ 4ms for trip decision)
    // 2. No blocking operations (no mutex, no IO wait)
    // 3. Deterministic dispatch (no dynamic dispatch)
    // 4. No heap allocation in protection path
    // 5. GOOSE message priority verified
    
    let fault_detected = current.value > FAULT_THRESHOLD || voltage.value < VOLTAGE_MIN;
    
    if fault_detected {
        breaker.trip()?;  // Must complete within 4ms
        goose_send(TRIP_MESSAGE, breaker.id)?;
    }
    
    ret Ok(())
}
```

### Why It Matters

Power grid protection relays must respond within 4 milliseconds. Missing
the deadline means equipment damage or power outage. Today, timing
verification is done with external WCET tools. Fides enforces it at compile time.

### Implementation

1. `@grid-safe` activates: no blocking, no dynamic dispatch, no heap in protection path
2. Combined with `@realtime` for WCET analysis
3. Combined with `@bounded` for loop termination proof
4. `@gas(4000)` to verify ≤ 4ms execution (at 1M ops/sec budget)
