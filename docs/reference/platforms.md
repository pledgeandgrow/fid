# Platforms

Where Fides runs, and how much each platform is tested (item PL1). A platform
moves up a tier only when the tests that tier names pass on it.

## Tier 1: tested on every commit

CI runs the whole suite, and a failure blocks the change.

Status (Nov 6, 2026; history, see Nov 17–19 below): the repository had no
CI workflow then (the `.github/workflows/` files were removed in commit
9f2ba40), so no platform met this definition. The last full suite run is Nov 2 on Windows
(Rust harness, `fidesc` as compiler: interp 339/339, C 343/343, PE 345/345,
expected-fail 89/89); Linux CI last ran green in session 37. The Nov 4–6
changes were checked with `fid run`, `fid ccompile` and `fid emit` on their
tests. The table lists what Tier 1 covered when CI ran.

Nov 17–19: 13 workflows in `.github/workflows/`. tier1.yml runs
`scripts/tier1.sh` (lanes, expected failures, binaries) on
windows-latest, ubuntu-latest, ubuntu-24.04-arm and macos-15; tier2.yml
runs `scripts/tier2.sh` on the partly-tested targets; suite-all.yml,
macos.yml, macos-native.yml, bare-metal.yml, verify-nightly.yml and
verify-weekly.yml cover the rest; what each lane checks, and what a
target skips, is in [tier-checks.md](tier-checks.md). A platform counts
as Tier 1 once its job is green.

| Platform | Backends | What CI runs |
|---|---|---|
| Windows x64 | interpreter, C, native (PE, `fid emit`) | the suite on all three, expected failures, every stdlib module, the native fixpoint, an installed `fid`. Native async/tasks worked on Nov 6 and broke in builds of Nov 9 afternoon (iropt passes; fixed again in the evening's fid31, known-bugs 2f.2); the interpreters run an `async fn` to the end at the call (`test_async_suspend`) |
| Linux x64 | interpreter, C | the suite, expected failures, `fid` bootstrapping itself, the release archive on a clean Ubuntu |

## Tier 2: builds, partly tested

The compiler builds, and programs compile; the suite does not run there on
every commit.

| Platform | Backends | Status |
|---|---|---|
| Windows x64 native on the machine layer (PE) | `fid emit --nat` | opt-in; `fid emit` without it stays on pe_ir. The machine layer's x86-64 code (compiler/nat_ir.fd) in a PE it writes itself, the Linux runtime unchanged over kernel32, ws2_32 and bcrypt (stdlib/linux_runtime/win.fd): Windows threads, WaitOnAddress for futexes, a vectored exception handler for the panics ("Panic: Stack overflow", "Invalid memory access"), sockets with IPv6. 301 of 338 suite tests match the interpreter (Nov 9); processes (`system`, `process_run`) wait for a hook in `pe_sh` (sys.fd); PDB debug info since compiler/pdb.fd (`emit -g` writes `<out>.pdb`, DB3) |
| Linux x64 native (ELF) | `fid emit --target x86_64-linux` | static ELF, no C compiler or linker, the same machine-layer backend and Linux runtime as ARM64/RISC-V (compiler/nat_ir.fd); 272 suite tests match the interpreter (Nov 9, run natively in WSL); `log`/`pow` need FMA3 (x86-64-v3). `fid emit --elf` is pe_ir's earlier no-stdlib path |
| Linux ARM64 | C (`fid ccompile --target aarch64-linux`) | static musl executables from `zig cc` on any host; 270 of 286 suite tests matched `fid run` under qemu-aarch64 (Nov 7), the rest explained in release F8 |
| Linux RISC-V 64 | C (`--target riscv64-linux`) | same path as ARM64; sample programs run under qemu-riscv64; the full suite has not completed (release F8) |
| Linux ARM64, RISC-V 64 native | `fid emit --target aarch64-linux` / `riscv64-linux` | static ELF, no C compiler or linker (compiler/nat_ir.fd: a machine IR and one register allocator for the three Linux targets; stdlib/linux_runtime: threads, M:N tasks, channels, sockets, processes, musl's math, panics with traces, guard pages); under QEMU 273 (ARM64) and 271 (RISC-V) of 317 suite tests match the interpreter (Nov 9); most of the rest are references the 19:03 interpreter could not produce (release F8) |
| macOS x64 and ARM64 | C (`--target x86_64-macos`, `aarch64-macos`) | Mach-O executables build from any host with `zig cc`; macos.yml runs the whole suite on macos-15 and macos-15-intel runners (first reached the suite Nov 20; a bash-3.2 empty-array bug in cross-test.sh was the last blocker before it) |
| WebAssembly (WASI) | C (`--target wasm32-wasi`) | `.wasm` from `zig cc`, run with `node scripts/wasm-run.mjs`; sample programs run; tasks and `spawn` run to their end when entered; starting a process panics (`not available in WebAssembly: processes (...)`), no signals or socket creation; typed fixed-size array bindings compile (CI5); the full suite has not completed (release F9) |

Run a target's suite with `scripts/cross-test.sh <target>`: it builds every
test through C for the target, runs it (qemu-user in WSL or on Linux, Node for
WASI; macOS binaries are only built off a Mac) and compares the output and
exit code with the interpreter's. `NATIVE=1` builds with the native backend
(`fid emit --target aarch64-linux|riscv64-linux|x86_64-linux`, and `--nat`
for `x86_64-windows`) instead of C.

Which commands need a C toolchain to cross-compile (only `ccompile --target`,
through zig) and which need none (`fid build`/`emit --target`, any host):
docs/reference/cross-compilation.md.

## Tier 3: present, not supported

| Platform | Backends | Status |
|---|---|---|
| WebAssembly native | `fid wasm` | Nov 15: valid modules over WASI preview 1, no C compiler (compiler/wasm.fd, stdlib/linux_runtime/wasi.fd); 316 of 344 suite tests match `fid run` (scripts/wasm-test.sh `ALL=1`, Nov 15). Nov 18: tasks switch stacks (asyncify-style, compiler/wasm.fd "Tasks"; threads still run to their end), structured control flow (TG16), an index into a fixed-size array checked as the interpreter does; processes are a clear panic `not available in WebAssembly: processes (...)`, C functions without a WebAssembly version panic when called, bare-metal builtins do nothing (release F9) |
| macOS native | `fid emit --target x86_64-macos\|aarch64-macos` (machine layer) | Mach-O executables (PIE, dyld binds into a GOT, ad-hoc code signature, a symbol table; compiler/nat_enc.fd `nat_macho`, runtime stdlib/linux_runtime/darwin.fd) build from any host and pass scripts/macho-check.py; not run on a Mac yet (release F7) |
| Bare metal | `emit --bare`, `--boot` | `fid emit --bare` writes Multiboot flat binaries; bare-metal.yml boots every tests/bootstrap/test_bare_*.fd under `qemu-system-x86_64 -kernel` and compares COM1 output and the exit code (green again Nov 20, after the stage-0 bootstrap fix); the fides-os job tracks that kernel's own build (pledgeandgrow/fid-os issue 1) |
| Old PIR backends | `build-x86` | Removed from every other command on Nov 17 (llvm MI18): `build --native` and `codegen` (also `--elf\|--coff\|--macho` objects) run the machine layer, `emit --boot` builds pe_ir's bootable image, `fid pir` names the replacements. Nov 9: `build-x86` wrote an exe that segfaulted; the ELF object writer was fixed by TG13, the exe is untested since (known-bugs 2g.16) |
| Android | `fid emit --target aarch64-android[<api>]` (machine layer) | Nov 27 (target roadmap XT64): a static PIE (ET_DYN, no PT_INTERP) over the Linux runtime, `.note.android.ident` with the API level (34 by default); structure checked by tests/nattools/xt_elf_notes_check.sh, never run (`adb shell`: XT68). A program calling C functions is refused until bionic linking (XT65); cross-compilation.md § FreeBSD, OpenBSD and Android |
| FreeBSD native | `fid emit --target x86_64-freebsd\|aarch64-freebsd` (machine layer) | Nov 27 (XT44, XT50): the ELF branding (EI_OSABI 9, the ABI note `.note.tag` with `__FreeBSD_version` 1400097) is written, but there is no FreeBSD runtime yet (XT43), so the build stops with an error naming it; `FID_IMAGE_ONLY=1` writes the image over the Linux runtime for its headers only. The C route (`ccompile --target x86_64-freebsd`) is tier 2 above |
| OpenBSD | `fid emit --target x86_64-openbsd\|aarch64-openbsd` (machine layer) | Nov 27 (XT54): `.note.openbsd.ident`, `PT_OPENBSD_NOBTCFI`, code and data in separate PT_LOADs (W^X); OpenBSD takes system calls only from libc, so nothing runs before XT53 (dynamic libc) and XT55 (the runtime): the build stops with an error naming XT53, `FID_IMAGE_ONLY=1` writes the image for its headers |
| iOS (device and simulator) | `fid emit --target aarch64-ios\|aarch64-ios-simulator` (machine layer) | Nov 27 (target roadmap XT77): the macOS Mach-O with LC_BUILD_VERSION platform iOS (2) or iOS simulator (7), minimum iOS 15.0 or the one the target names (`aarch64-ios17.2`), the darwin runtime; structure checked by tests/nattools/xt_ios_target_check.sh, never run. A device also needs a real signature (XT81) and an app bundle (XT82); cross-compilation.md § iOS |
| Windows ARM64 | none | `--target aarch64-windows` is the machine layer's and stops with an error naming XT14 (no ARM64 PE writer yet, XT22); the C route (`ccompile --target aarch64-windows`, zig) builds it |

## What decides a platform's tier

- **Tier 1** needs CI that runs the suite on each commit and blocks on failure.
- **Tier 2** needs a build that is checked at release time.
- **Tier 3** is code in the tree that is not checked.
