# Releasing Fides

The steps to cut a release, in order. Each step names the script that does
it; none of them needs anything outside this repository except NSIS (for the
installer), a C compiler (`zig cc`) and, for signing, a code-signing
certificate.

## 1. Build and check the compiler

```bash
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh       # 3 stages, fixpoint checked
scripts/verify/clean-bootstrap.sh                  # same from a clean `git archive HEAD`
FID=release/fid.exe scripts/verify/repro.sh        # same input, same bytes
bash scripts/leak-check.sh                         # tests/leaks: every block freed
```

The fixpoint (stage 2 C == stage 3 C) and the clean bootstrap show that the
shipped `fid.exe` is built from the tagged source and nothing else. Run them
one at a time on a quiet machine: parallel builds slow each other down by
10x or more.

Nov 21: started from `fidesc` (no `release/stage0/fid` pinned), the
bootstrap cannot pass stage 2 on the current tree: a `fid` built by
`fidesc` runs out of memory self-compiling it (known-bugs 2g.79). Start it
from a self-hosted `fid` instead (`STAGE0=<fid.exe>`; `PIN=1` after a
fixpoint keeps the last stage as the next stage 0); the Nov 18 `fid` built
the tree that way.

## 2. Run the tests

The full suite through the Rust harness with `fid` as the compiler under
test, and the native suites:

```bash
./target/release/fidesc test --compiler release/fid.exe
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh x86_64-windows
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh aarch64-linux
AGREE=1 FID=release/fid.exe JOBS=4 bash scripts/suite-all.sh   # every lane; C and our backend agree
bash scripts/miscompile-gate.sh --run                          # every fixed miscompile's test passes
bash scripts/release-fresh.sh release/fid.exe                  # the stamped build of this source, not a stale one
CC="zig cc" bash scripts/crater.sh release/fid.exe             # every known project still builds (ST6)
bash scripts/install-layout-check.sh release/fid.exe           # run / ccompile / emit from an installed layout
```

Pushing the tag runs `.github/workflows/release.yml`, which does the
bootstrap, the installer, a clean-machine install, update and uninstall,
the Linux archive and crater on GitHub's runners, then attaches the files,
`SHA256SUMS` and a provenance attestation per file to a draft release.
What CI checks on every push besides (`.github/workflows/ci.yml`): the
own-backend gate on the whole suite for x86_64, AArch64 and RISC-V 64 Linux
(`ALL=1 scripts/nat-gate.sh`, qemu-user; llvm-roadmap RA16), the rewrite
rules proved by z3 (`scripts/rules-smt.sh`, ME17); in bench.yml, the
per-stage compile-time gate against the base commit (`scripts/compile-gate.sh`,
SP15). Weekly (verify-weekly.yml): the suite with `fid` as the compiler
(A4) and the whole-compiler tests under load (A12, below).

**Whole-compiler tests and the 30 s limit (A12).** `fidesc test` gives each
compiled test 30 s (the frozen src/main.rs). The tests listed in
`scripts/long-tests.txt` (today `test_spawn_gt`) run the self-hosted
compiler inside themselves and can need more on a loaded machine;
`verify.sh` reruns a C or PE failure of one of them with
`scripts/long-tests.sh` (300 s, `LIMIT=`) and counts it only if that run
fails too. A new test of that kind goes into `scripts/long-tests.txt`.

After a release, record its crater results as the next baseline:
`scripts/crater.sh --save release/fid.exe`.

A release ships only when the two independent paths agree on the suite
(llvm-roadmap RL22): `scripts/suite-all.sh` runs it through the C path and
through our own backend on every target the host can run, and with
`AGREE=1` fails when a test passes on one lane and fails, or does not build,
on another.

Record the results in docs/reference/testing.md and the counts in
docs/reference/selfhost.md → Numbers.

## 3. Build, sign and checksum the installer

```bash
FIDES_VERSION=1.0.1 ./installer/build-installer.sh   # default: the version in installer/fides.nsi
```

- **The compiler it ships** is checked first (release-roadmap A10):
  `scripts/release-fresh.sh release/fid.exe` must say it is the stamped
  release build of this source. A missing, unstamped or stale `fid.exe` is
  rebuilt with `RELEASE=1 scripts/bootstrap.sh` (3 stages, fixpoint) and
  checked again; the installer is refused if the rebuild fails.
  `FIDES_NO_REBUILD=1` refuses instead of rebuilding (release.yml sets it:
  it has just built and checked `fid.exe`).
- **The companion tools** `ark.exe`, `scribe.exe` and `veil.exe` are the
  Fides ports, built by that `fid.exe` from `tools/<tool>.fd`
  (`release/fid.exe ccompile`, with `CC`, default `zig cc`; N8), so they
  always match it. `FIDES_RUST_TOOLS=1` ships the frozen Rust builds from
  `target/release` instead. `fidesc.exe` is the Rust compiler, from Cargo.
- The version comes from `FIDES_VERSION` (default: the `!define VERSION` in `installer/fides.nsi`) and is
  passed to NSIS (`-DVERSION=...`), so the script and `installer/fides.nsi`
  cannot disagree.
- **Signing:** with `FIDES_SIGN_PFX=<certificate.pfx>` and `FIDES_SIGN_PASS`
  set, the installer is signed with Authenticode (`signtool` on Windows,
  `osslsigncode` elsewhere; timestamp server `FIDES_SIGN_TS`, default
  DigiCert). An unsigned installer works but Windows SmartScreen warns every
  user. Never commit the certificate or its password.
- **Checksums:** the script ends with `scripts/release-checksums.sh`, which
  writes `SHA256SUMS.txt` (installer and shipped binaries) at the repository
  root. Run it by hand for other files: `scripts/release-checksums.sh <files>`.

## 4. Publish

1. Update `docs/reference/changelog.md`, the version in `Cargo.toml`
   (what `fidesc` reports) and `fid_version_base` in
   `compiler/build_info.fd` (a dev build of `fid` prints it with `+dev`; a
   release build prints the version `scripts/build-info.sh` writes from
   `FIDES_VERSION`, a `VERSION` file or the tag).
2. Tag the commit: `git tag -a v1.0.1 -m "Fides 1.0.1"` and push the tag.
3. Create the GitHub release from the tag. GitHub drafts the notes from the
   merged pull requests, grouped by label (`.github/release.yml`); add the
   highlights from the changelog.
4. Attach `Fid-Setup-<version>.exe` and `SHA256SUMS.txt`, and the
   standard library's documentation: `scripts/stdlib-docs.sh --zip`
   regenerates `docs/stdlib/` with scribe (`fid doc`, a page per module and
   `index.md`) and packs it as `build/fides-stdlib-docs.zip`; commit the
   regenerated pages with the release (`scripts/stdlib-docs.sh --check`
   fails while they are stale). A tag's release.yml run does this itself:
   the `linux` job generates the zip with the release `fid` and `publish`
   attaches it, with its checksum and provenance, to the draft release
   (N10).
5. In the notes, say how the binary was checked: fixpoint, clean bootstrap,
   reproducible build, suite results.

Users check a download with `sha256sum -c SHA256SUMS.txt --ignore-missing`
(Linux, Git Bash) or `Get-FileHash Fid-Setup-<version>.exe -Algorithm
SHA256` (PowerShell), compared with the line in `SHA256SUMS.txt`.

## Security fixes

A fix for a privately reported vulnerability (SECURITY.md) is developed in a
GitHub security advisory's private fork, released as a patch version with
the steps above, and the advisory is published with the release.
