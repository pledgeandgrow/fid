# Platforms

Where Fides runs, and how much each platform is tested (item PL1). A platform
moves up a tier only when the tests that tier names pass on it.

## Tier 1: tested on every commit

CI runs the whole suite, and a failure blocks the change.

Status (Nov 6, 2026): the repository has no CI workflow at the moment (the
`.github/workflows/` files were removed in commit 9f2ba40), so no platform
meets this definition today. The last full suite run is Nov 2 on Windows
(Rust harness, `fidesc` as compiler: interp 339/339, C 343/343, PE 345/345,
expected-fail 89/89); Linux CI last ran green in session 37. The Nov 4–6
changes were checked with `fid run`, `fid ccompile` and `fid emit` on their
tests. The table lists what Tier 1 covered when CI ran.

| Platform | Backends | What CI runs |
|---|---|---|
| Windows x64 | interpreter, C, native (PE, `fid emit`) | the suite on all three, expected failures, every stdlib module, the native fixpoint, an installed `fid`. Native async/tasks worked on Nov 6 and broke in builds of Nov 9 afternoon (iropt passes; fixed again in the evening's fid31, known-bugs 2f.2); the interpreters run an `async fn` to the end at the call (`test_async_suspend`) |
| Linux x64 | interpreter, C | the suite, expected failures, `fid` bootstrapping itself, the release archive on a clean Ubuntu |

## Tier 2: builds, partly tested

The compiler builds, and programs compile; the suite does not run there on
every commit.

| Platform | Backends | Status |
|---|---|---|
| Windows x64 native on the machine layer (PE) | `fid emit --nat` | opt-in; `fid emit` without it stays on pe_ir. The machine layer's x86-64 code (compiler/nat_ir.fd) in a PE it writes itself, the Linux runtime unchanged over kernel32, ws2_32 and bcrypt (stdlib/linux_runtime/win.fd): Windows threads, WaitOnAddress for futexes, a vectored exception handler for the panics ("Panic: Stack overflow", "Invalid memory access"), sockets with IPv6. 301 of 338 suite tests match the interpreter (Nov 9); processes (`system`, `process_run`) wait for a hook in `pe_sh` (sys.fd); no PDB yet |
| Linux x64 native (ELF) | `fid emit --target x86_64-linux` | static ELF, no C compiler or linker, the same machine-layer backend and Linux runtime as ARM64/RISC-V (compiler/nat_ir.fd); 272 suite tests match the interpreter (Nov 9, run natively in WSL); `log`/`pow` need FMA3 (x86-64-v3). `fid emit --elf` is pe_ir's earlier no-stdlib path |
| Linux ARM64 | C (`fid ccompile --target aarch64-linux`) | static musl executables from `zig cc` on any host; 270 of 286 suite tests matched `fid run` under qemu-aarch64 (Nov 7), the rest explained in release F8 |
| Linux RISC-V 64 | C (`--target riscv64-linux`) | same path as ARM64; sample programs run under qemu-riscv64; the full suite has not completed (release F8) |
| Linux ARM64, RISC-V 64 native | `fid emit --target aarch64-linux` / `riscv64-linux` | static ELF, no C compiler or linker (compiler/nat_ir.fd: a machine IR and one register allocator for the three Linux targets; stdlib/linux_runtime: threads, M:N tasks, channels, sockets, processes, musl's math, panics with traces, guard pages); under QEMU 273 (ARM64) and 271 (RISC-V) of 317 suite tests match the interpreter (Nov 9); most of the rest are references the 19:03 interpreter could not produce (release F8) |
| macOS x64 and ARM64 | C (`--target x86_64-macos`, `aarch64-macos`) | Mach-O executables build from any host with `zig cc`, and so does the compiler itself; not run (no Mac); CI job written in `scripts/ci/cross.yml` (release F5) |
| WebAssembly (WASI) | C (`--target wasm32-wasi`) | `.wasm` from `zig cc`, run with `node scripts/wasm-run.mjs`; sample programs run; tasks run to their end when entered, no processes, signals or socket creation; the full suite has not completed (release F9) |

Run a target's suite with `scripts/cross-test.sh <target>`: it builds every
test through C for the target, runs it (qemu-user in WSL or on Linux, Node for
WASI; macOS binaries are only built off a Mac) and compares the output and
exit code with the interpreter's. `NATIVE=1` builds with the native backend
(`fid emit --target aarch64-linux|riscv64-linux|x86_64-linux`, and `--nat`
for `x86_64-windows`) instead of C.

## Tier 3: present, not supported

| Platform | Backends | Status |
|---|---|---|
| WebAssembly native | `fid wasm` | its modules fail validation; no collections (release F9) |
| macOS native | Mach-O objects | no executables yet (release F7) |
| Bare metal | `emit --bare`, `--boot` | `fid emit --bare` writes flat binaries since Nov 6 (release G2); run on a hosted test runner, not yet under QEMU (G1) |
| Old PIR backends | `build --native` (default target), `build-x86`, `codegen --target x86-64\|arm64\|riscv`, `codegen --elf\|--coff\|--macho`, `emit --boot` | Nov 9: `build --native` panics in the PIR optimizer, `build-x86` writes an exe that segfaults, `emit --boot` panics; `codegen` prints assembly (ARM64/RISC-V assembly is wrong: `return` for `ret`); COFF objects are valid, Mach-O headers correct but never linked, x86-64 ELF objects have malformed section headers (known-bugs 2g.16) |
| Android, iOS | none | not started (PL6) |

## What decides a platform's tier

- **Tier 1** needs CI that runs the suite on each commit and blocks on failure.
- **Tier 2** needs a build that is checked at release time.
- **Tier 3** is code in the tree that is not checked.
