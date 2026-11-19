# Tier checks

How the platform tiers of [platforms.md](platforms.md) are checked
(competitive-roadmap PL2, PL3): two scripts, the CI workflows that run them,
and how to run all of them from one Windows machine.

## Tier 1: `scripts/tier1.sh` (PL2)

Windows x64, Linux x64, Linux ARM64, macOS ARM64. On each, the whole suite
passes on every commit:

- **lanes**: every test through the C path and through our own backend, each
  compared with the interpreter (`fid run`: output, exit code, panic
  message), with `scripts/suite-all.sh AGREE=1` (a test that passes on one
  lane passes on all):
  - windows-x64: `c:x86_64-windows`, `native:x86_64-windows-pe` (pe_ir,
    `fid emit`), `native:x86_64-windows` (`emit --nat`)
  - linux-x64: `c:x86_64-linux`, `native:x86_64-linux`
  - linux-arm64: `c:aarch64-linux`, `native:aarch64-linux`
  - macos-arm64: `c:aarch64-macos`, `native:aarch64-macos`
- **rejected**: every expected-fail test is refused by `fid run`.
- **binaries**: every built binary is an executable of its target
  (`scripts/bin-target-check.py`); on macOS also a well-formed, signed
  Mach-O (`tools/macho_check.fd`).

CI: `.github/workflows/tier1.yml`, one job per platform (windows-latest,
ubuntu-latest, ubuntu-24.04-arm, macos-15), on every push and pull request.

From one Windows machine (`PLATFORM=` names another platform than the host):

```bash
FID=release/fid.exe bash scripts/tier1.sh                         # Windows x64, natively
FID=release/fid.exe PLATFORM=linux-x64 bash scripts/tier1.sh      # cross-built, run in WSL
FID=release/fid.exe PLATFORM=linux-arm64 bash scripts/tier1.sh    # run under qemu-aarch64 in WSL
FID=release/fid.exe PLATFORM=macos-arm64 bash scripts/tier1.sh    # built, Mach-O checked, not run
```

Each writes `build/tier1/summary.md` (set `OUT=` to keep the four apart) and
ends with `tier1: <platform> PASS` or `FAIL`. `TESTS="a.fd b.fd"` makes a
smoke run. A Linux platform from Windows needs WSL, and linux-arm64 needs
`qemu-aarch64` in it (`apt install qemu-user`); the script says so and stops
when they are missing.

### What a lane leaves out, and why

A lane's `results.txt` says `SKIP  <test> (<reason>)` for a test it does not
run; a SKIP is neither a pass nor a failure, and the lane summary counts
each kind:

- **Not for this target** (`scripts/lane-skips.txt`, read by
  `scripts/cross-test.sh`): `<target pattern> <test> <reason>`, a leading
  `!` for every target the pattern does not match. Only what a test cannot
  do on a target by its nature, never a bug: `test_cparser_dll_exports`
  reads the System32 DLLs, so it runs on the Windows lanes only. Tests that
  need an external library or a bare machine on every target stay in
  cross-test.sh's `skip_re`. `LANE_SKIPS=<file>` uses another list.
- **No dynamic loader for qemu-user**: a native Linux executable that calls
  C functions (`extern "C"`, `link "m"`) names the system's loader and is
  bound against the target's C library at load (nat_ir's CI2 path). Under
  qemu-aarch64 / qemu-riscv64 the loader and libraries come from
  `QEMU_LD_PREFIX`; `run-all.sh` sets it to `/usr/<arch>-linux-gnu` when that
  holds one (`apt install libc6-arm64-cross` or `libc6-riscv64-cross`).
  Without it such a test is `SKIP (dynamic: needs /lib/ld-linux-aarch64.so.1
  ...)`. On an ARM64 Linux host (the CI job) it runs natively. With WSL on
  this Windows machine, install `libc6-arm64-cross` in WSL for a run with
  no such skips. CI has none (CI4): ci.yml's natgate job installs
  `libc6-arm64-cross libc6-riscv64-cross` and runs `scripts/nat-gate.sh`
  with `REQUIRE_LOADER=1`, which fails the gate on any such SKIP;
  suite-all.yml's and tier2.yml's riscv64 jobs install the RISC-V one.
- A binary that imports one of the runtime's own names (`nat_*`, `pert_*`,
  `pe_*`, `p_*`) from a shared library is never skipped: that is a compiler
  bug (every executable defines those), the loader stops it with "symbol
  lookup error", and `bin-target-check.py` fails it in the binaries check.

`scripts/bin-target-check.py` reads the file names from stdin with `-` (a
lane's binaries are more than Windows' command line holds) and with
`--loader` prints each ELF's interpreter and runtime imports.

## Tier 2: `scripts/tier2.sh` (PL3)

Windows ARM64, macOS x64, FreeBSD x64, Linux RISC-V 64. The suite builds for
each through C (`fid ccompile --target`, `zig cc`), and runs where a machine
for it is at hand:

| Lane | `zig cc -target` | Runs |
|---|---|---|
| `c:aarch64-windows` | `aarch64-windows-gnu` | Windows ARM64 (CI: windows-11-arm) |
| `c:x86_64-macos` | `x86_64-macos` | an Intel Mac, or Rosetta 2 (CI: macos-15-intel) |
| `c:x86_64-freebsd` | `x86_64-freebsd` (dynamic, FreeBSD's libc) | FreeBSD (CI: a FreeBSD VM) |
| `c:riscv64-linux` | `riscv64-linux-musl` (static) | qemu-riscv64, in WSL on Windows |

`WITH_NATIVE=1` adds our own backend's `native:riscv64-linux` and
`native:x86_64-macos`. A riscv64 lane needs `qemu-riscv64` (in WSL on
Windows): the script says so and stops when it is missing
(`NO_RUN_CHECK=1` skips the check). A lane passes when every test builds, and, where it
ran, every test matches the interpreter. Every binary is checked by
`scripts/bin-target-check.py` (PE machine ARM64, an ELF tagged FreeBSD, ...)
and the Mach-O ones by `tools/macho_check.fd`.

```bash
FID=release/fid.exe bash scripts/tier2.sh                  # all four; riscv64 runs under qemu
FID=release/fid.exe WITH_NATIVE=1 bash scripts/tier2.sh    # plus the native lanes
```

A lane built where it cannot run lists `BUILT` per test and leaves
`build/tier2/lanes/<lane>/run-all.sh`. To finish it on the target: copy the
lane's directory there (under a checkout, same path), run
`bash <dir>/run-all.sh`, bring the directory back and compare:

```bash
COMPARE_ONLY=1 OUT=build/tier2/lanes/c-x86_64-freebsd bash scripts/cross-test.sh x86_64-freebsd
```

`.github/workflows/tier2.yml` does exactly that: job `build-only` builds the
Windows ARM64 and FreeBSD lanes on ubuntu-latest, `windows-arm64` runs them
on a windows-11-arm runner (`HOST_ARCH=aarch64` tells `cross-test.sh` the
host's architecture when an emulated shell cannot), and `freebsd` in a
FreeBSD VM (vmactions/freebsd-vm). `tests/tooling/tier2_compare_check.sh`
checks the compare-only step without a compiler.

## What the C runtime does per target

The generated C (compiler/codegen_c.fd) picks per platform:

- FreeBSD: no `<alloca.h>` (alloca is in `<stdlib.h>`); `malloc_usable_size`
  from `<malloc_np.h>`; the POSIX 2008 calls and `clock_gettime` as on
  Linux; task waits on sockets through kqueue, as on macOS; panic traces
  through `_Unwind_Backtrace` (zig links its libunwind: `-lunwind`).
- Windows ARM64: panic traces and the sampling profiler (`FID_PROF`) walk
  the ARM64 unwind tables (`RtlLookupFunctionEntry`/`RtlVirtualUnwind` over
  an ARM64 `CONTEXT`; a leaf's caller is in the link register); tasks run on
  fibers (the own context switch is x86-64 only on Windows).
- WASI (wasm32-wasi): no processes, signals or socket creation (those calls
  fail with `ENOSYS`); `SO_ERROR` and `SO_REUSEADDR` are defined when
  wasi-libc does not (known bug 2g.61: every `ccompile --target
  wasm32-wasi` failed on `SO_ERROR`).
- Every platform: a user function named like a type or macro of the
  runtime's headers (`byte`, `boolean`, `hyper` from rpcndr.h; `BYTE`,
  `DWORD`, `HANDLE`, `near`, `far`, `IN`, `ERROR` from windows.h; `FILE`,
  `DIR`, `stdin`, the `<stdint.h>` types; GNU C's `linux`, `unix`) gets a C
  name of its own, `p_<name>_user`, as one named like a libc function does
  (`cg_libc_clash` / `cg_c_header_name`, known bug 2g.69).

## Every lane on every change (RL2)

`scripts/suite-all.sh` knows ten lanes (`LIST_LANES=1 bash
scripts/suite-all.sh` prints them). Each runs on every push and pull
request to main:

| Lanes | Workflow, job |
|-------|---------------|
| `c:x86_64-windows native:x86_64-windows-pe native:x86_64-windows` | tier1.yml, windows-x64 |
| `c:x86_64-linux native:x86_64-linux` | tier1.yml, linux-x64 |
| `c:aarch64-linux native:aarch64-linux` | tier1.yml, linux-arm64 (ARM64 runner) |
| `c:riscv64-linux native:riscv64-linux` | suite-all.yml, riscv64 (qemu-riscv64, `FID_VERIFY=1`) |
| `c:wasm32-wasi` (and `fid wasm`, scripts/wasm-test.sh) | suite-all.yml, wasm32 (Node's WASI) |

`scripts/suite-lanes-check.sh` (run by ci-wiring-check.sh in ci.yml's
`gates` job) fails when a lane `suite-all.sh` lists is run by no workflow
triggered on both push and pull_request; `SELFTEST=1` checks that a
dropped lane and a push-only workflow are caught. Locally, `bash
scripts/suite-all.sh` runs every lane the host can run (Linux lanes in WSL
on Windows, `node` for wasm32).

## Tests of the tier machinery

- `tests/tooling/tier2_compare_check.sh`: the compare-only step of a lane
  built on one machine and run on another (no compiler needed).
- `tests/tooling/tier2-targets-check.sh` (`FID=...`): every Tier 2 target in
  both spellings (`x86_64-freebsd` and `freebsd-x64`, `aarch64-windows` and
  `windows-arm64`, ...) builds `hello`, `test_async` and `test_int_width`
  through C, each binary is checked by `bin-target-check.py`, and the
  riscv64 ones run under qemu-riscv64 against `fid run`.
- `tests/tooling/bin-target-check-selftest.py` (no compiler): ELF files made
  on the spot; a static and a C-importing dynamic one pass, one importing
  `nat_tls_set` fails; `--loader` and `-` (stdin).
- `tests/tooling/pl2-lane-skips-check.sh` (no compiler; runs itself in WSL
  on Windows): cross-test.sh with a stand-in fid and qemu-aarch64. A
  listed test is SKIP with its reason and never built; a C-only dynamic
  executable without a loader is SKIP and not started, and runs and passes
  with the loader under `QEMU_LD_PREFIX`; one importing `nat_tls_set` runs
  and FAILs.
- `tests/bootstrap/test_cfg_platforms.fd`: `@cfg` has exactly one variant
  for every tier-1 and tier-2 OS and architecture, `@cfg(freebsd)` among
  them, so the program builds on every lane.

## Tests that are not portable yet

Found while preparing the tier runs. Fixed since (Nov 17): `freebsd`,
`unix` and `wasi` are known `@cfg` conditions (compiler.fd `cfg_os_cond`);
`tests/bootstrap/test_cfg.fd` has one variant of each platform and
architecture function for every target; `test_c_callback_user_data.fd` has
a FreeBSD variant. Still open, outside the tier scripts:

- The native Linux lanes: an executable built by `fid emit --target
  x86_64-linux|aarch64-linux|riscv64-linux` imports the runtime primitives
  (`nat_tls_set`, `nat_frame_fp`, `nat_os`, ...) from libc through the
  dynamic linker (compiler/nat_ir.fd, the CI2 branch of the `extern "C"`
  call: the runtime's `extern "C"` block declares them, and only a
  `pert_c_`/`nat_c_` name was looked for before binding dynamically), so
  every test fails with "symbol lookup error". The binaries check now
  names this; the fix is in nat_ir.fd.
