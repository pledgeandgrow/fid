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

## 2. Run the tests

The full suite through the Rust harness with `fid` as the compiler under
test, and the native suites:

```bash
./target/release/fidesc test --compiler release/fid.exe
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh x86_64-windows
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh aarch64-linux
```

Record the results in docs/reference/testing.md and the counts in
docs/reference/selfhost.md → Numbers.

## 3. Build, sign and checksum the installer

```bash
FIDES_VERSION=1.0.1 ./installer/build-installer.sh   # default: the version in installer/fides.nsi
```

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

1. Update `docs/reference/changelog.md` and the version in `Cargo.toml`
   (`fid version` prints it).
2. Tag the commit: `git tag -a v1.0.1 -m "Fides 1.0.1"` and push the tag.
3. Create the GitHub release from the tag. GitHub drafts the notes from the
   merged pull requests, grouped by label (`.github/release.yml`); add the
   highlights from the changelog.
4. Attach `Fides-Setup-<version>.exe` and `SHA256SUMS.txt`.
5. In the notes, say how the binary was checked: fixpoint, clean bootstrap,
   reproducible build, suite results.

Users check a download with `sha256sum -c SHA256SUMS.txt --ignore-missing`
(Linux, Git Bash) or `Get-FileHash Fides-Setup-<version>.exe -Algorithm
SHA256` (PowerShell), compared with the line in `SHA256SUMS.txt`.

## Security fixes

A fix for a privately reported vulnerability (SECURITY.md) is developed in a
GitHub security advisory's private fork, released as a patch version with
the steps above, and the advisory is published with the release.
