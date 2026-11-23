# What names a build of `fid`: version, build hash, cache key

`fid version` (also `fid --version`, `fid -V`) prints:

```
fid 1.0.0+dev (self-hosted Fides compiler)
build: exe:<16 hex digits>          the hash of the executable's bytes
commit: <commit>                    release builds only
built: <YYYY-MM-DD>                 release builds only
cache key: src:<hex> | exe:<hex>    what the build caches are keyed on
```

All of it comes from `compiler/build_info.fd`, whose values
`scripts/build-info.sh` writes before a release build
(`RELEASE=1 scripts/bootstrap.sh`) and puts back with `--reset` after it.

## Version (release N12)

`fid_version()` is the release's version when the build has one
(`fid_build_release`), else `<fid_version_base>+dev`. `build-info.sh` takes
the release version from, in order: `FIDES_VERSION` (what release.yml and
the installer use; a leading `v` is dropped), a `VERSION` file at the
repository root, or a tag `v<version>` on the commit built. Text that is not
a version (anything but letters, digits, `.`, `+`, `-`) is ignored.
`fid_version_base` is set by hand when a release is tagged.

The same version is in crash reports: `fid run --crash-file` (main.fd) and
programs built through C (`codegen_c.fd` `__p_build_version`).

## Cache key (release D9)

Every build cache (the checked-core stamp, the stdlib AST artifact, the
import cache, the mid-end and iropt memos, pe_ir's function cache, the
`--inc` units and the build stamps) is keyed on `inc.fd`
`inc_compiler_hash()`, so that a rebuilt compiler never reuses what an older
one wrote.

- A development build keys on the hash of its executable's bytes
  ("exe"). Finding that hash reads the whole executable (about 50 MB) on
  every command that consults a cache: 0.3 s per `emit` with a -O0 build.
- A release build keys on `fid_build_source` — the hash `build-info.sh`
  computes over every `.fd` file under `compiler/` and `stdlib/` (but
  `build_info.fd`), line endings ignored — together with the executable's
  size ("src"). Nothing of the executable is read.
- `FIDES_SELF_HASH=exe` makes a release build key on its bytes again (to
  keep the stages of a bootstrap strictly apart, or to compare).

The size separates the stages of a bootstrap, which all carry the same
source hash but are compiled by different compilers.

## Checking

```
FID=release/fid.exe bash tests/tooling/n12-version-check.sh
FID=release/fid.exe bash tests/tooling/d9-emit-time-check.sh
FID=release/fid.exe bash tests/tooling/crash-report-check.sh
```

`d9-emit-time-check.sh` also checks that the machine layer (`emit --nat`,
the Linux and macOS targets) drops the functions no code can call before
the mid-end, as pe_ir does (PE88), with the same output as without
(`FID_PE_PRUNE=0`), and times `emit` of hello world: under 0.5 s is the D9
target, enforced for a release build.
