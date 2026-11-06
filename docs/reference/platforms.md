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
| Windows x64 | interpreter, C, native (PE, `fid emit`) | the suite on all three, expected failures, every stdlib module, the native fixpoint, an installed `fid`. Native async/tasks crash (`test_async`, `test_async_float_struct`) |
| Linux x64 | interpreter, C | the suite, expected failures, `fid` bootstrapping itself, the release archive on a clean Ubuntu |

## Tier 2: builds, partly tested

The compiler builds, and programs compile; the suite does not run there on
every commit.

| Platform | Backends | Status |
|---|---|---|
| Linux x64 native (ELF) | `fidesc build` | works for the programs tried; not in CI; `fid` has no ELF backend yet (release F4) |
| macOS x64 and ARM64 | C | expected to work through the C backend; not tested in CI (release F5) |
| Linux ARM64 | C | through a C cross-compiler; not tested |

## Tier 3: present, not supported

| Platform | Backends | Status |
|---|---|---|
| ARM64, RISC-V native | assembly | runtime has stubs that do nothing (release F8) |
| WebAssembly | WASM | no globals, struct copies or collections yet (release F9) |
| macOS native | Mach-O objects | no executables yet (release F7) |
| Bare metal | `emit --bare`, `--boot` | `fid emit --bare` panics (release G2) |
| Android, iOS | none | not started (PL6) |

## What decides a platform's tier

- **Tier 1** needs CI that runs the suite on each commit and blocks on failure.
- **Tier 2** needs a build that is checked at release time.
- **Tier 3** is code in the tree that is not checked.
