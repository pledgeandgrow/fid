# ark: the project file, dependencies and publishing

`fid ark <command>` (compiler/ark.fd). This page covers the manifest
fields, dev-dependencies, the per-machine cache, how a build gets one
version of each pact, and the API check on publish. The command list is in
docs/reference/tool.md; `fid ark help` prints it too.

Checked by `tests/ark/ark-features.sh [command that runs ark]`, which uses
local git repositories only (no network), and by `scripts/ark-check.sh`
(the new-user workflow).

## ark.toml

```toml
[package]
name = "app"
version = "0.1.0"
edition = "2026"
authors = ["Ann <ann@example.org>", "Bob"]
description = "A demo app"
license = "MIT"

[dependencies]
textkit = { path = "../textkit" }
mylib = { git = "https://example.org/mylib.git", tag = "v1.2.0" }
c = "^1.2"                 # a package of the index (ARK_INDEX)

[dev-dependencies]
testkit = { path = "../testkit" }
```

- `ark init [name] [--author <a>]... [--description <d>] [--license <spdx>]`
  writes every `[package]` field. `authors` is an array of strings (on one
  line or over several). `description` and `license` are free text: ark
  escapes `"` and `\` when it writes them.
- When ark rewrites ark.toml (`add`, `remove`), it keeps the `[package]`
  keys it does not interpret (`repository = ...`, say) and every other
  section (`[profile]`, `[[bin]]`, ...), with their comments, as written.
  Comments in `[package]`, `[dependencies]` and `[dev-dependencies]` are
  not kept.
- `ark list` shows the description, authors and license.

## Dev-dependencies

`ark add <name> ... --dev` puts a pact under `[dev-dependencies]` (a pact
is in one section or the other: adding it again moves it). `ark install`
puts dev-dependencies, and the pacts only they need, in
`ark_modules/.dev/` instead of `ark_modules/`, so a normal build, which
looks in `vendor/` and `ark_modules/`, does not see them. ark.lock marks
them `dev = true`. `ark install --no-dev` leaves them out. `ark vendor`
does not copy them. `ark audit` and `ark lock` cover them.

`fid test` also has to look in `ark_modules/.dev/`, after `ark_modules/`.
That is one line in `compiler/compiler.fd` (`ark_push_candidates`) and
has not been made yet. Until then a test that does `use testkit;` does
not find a dev pact.

## One copy of each repository per machine (CD3)

A git or registry pact's repository is cloned once per machine, into
`$ARK_CACHE/git/<name>-<hash>.git` (default `<ARK_HOME, USERPROFILE or
HOME>/.ark/cache`). The clone is a mirror and its objects are never pruned.
The checkout is shared too: the cache keeps one checkout of each commit,
`<ARK_CACHE or ~/.ark/cache>/checkouts/<name>-<hash>/<commit>` (made with
`git clone --shared`, so it borrows the mirror's objects), and a project's
`ark_modules/<name>` is a link to it: a junction on Windows, a symbolic
link elsewhere. `use` follows it like any directory, so a second project
that needs the same commit of a pact fetches and copies nothing.

- Do not edit a pact through `ark_modules/<name>`: every project that uses
  that commit sees the change (ark warns when the shared checkout has
  local changes).
- `ARK_LINK=0` (or `no`, `false`, `off`) gives a project a checkout of its
  own, which still borrows the cache's objects. A directory of its own
  already at `ark_modules/<name>` is kept.
- Old checkouts are not pruned yet: the cache keeps every commit any
  project checked out.

ark fetches into the cache only when it needs a commit the cache does not
have, or when nothing is locked (`ark update`, a first install). If that
fetch fails (offline, or the repository is gone), ark warns and uses the
copy it has. A repository on this machine is keyed by its absolute path,
so a relative `git = "../lib"` from two projects shares one copy. With no
home directory there is no cache, and ark clones into the project as it
did before.

Binary size: `fid emit` lowers only what a program reaches
(compiler/reach.fd), so a pact's unused functions add nothing to a native
binary. The C path (`ccompile`, `dump-c`) does the same (CD3);
`FID_C_REACH=0` or `FID_LOWER_ALL=1` lowers everything.

## One version of each pact in a build (CD4)

`ark install` reads the `[dependencies]` of every pact it installs as well
as the project's own: a path pact's `ark.toml` from its directory, and a
git or registry pact's from the cached repository at the commit being
installed. Each pact is installed once, in `ark_modules/`. Every pact a
build reaches is listed in ark.lock. One that another pact asks for has
`via = "<that pact>"`. A registry pact also has `version = "x.y.z"`.

- **Registry pacts:** the version chosen is the newest that every
  requirement on the pact accepts. A version already locked is kept as
  long as every requirement accepts it. If a requirement found later rules
  out the version chosen first, resolution starts over with that
  requirement included from the beginning. When several pacts agree on a
  version, ark says which version it chose:

  ```
  c 1.2.3: the newest version for app -> a -> c (1) and app -> b -> c (1.2)
  ```

- **Conflicts:** if no version satisfies every requirement, or two
  requirements name different git repositories or refs, or different paths,
  the install stops before it changes anything. The error lists each path
  that asks for the pact and what it wants. For a version spec, it also
  shows the newest version that spec accepts on its own:

  ```
  ark: conflicting requirements for 'c':
    app -> a -> c  wants 1  (newest: 1.3.0)
    app -> b2 -> c  wants 2  (newest: 2.0.0)
    no version of 'c' satisfies all of them; a build uses one version of each pact
  ```

- A git or registry pact cannot ask for a pact by `path` (the path would
  only exist inside that pact's own checkout). ark reports this as an error.
- Paths in a pact's `ark.toml` are relative to that pact's directory.
  `ark_modules/` is flat: one directory per pact name.

### Version specs

Clauses separated by commas, all of which must hold:

| Spec | Matches |
|------|---------|
| `""`, `*` | any version |
| `1.2`, `=1.2`, `1.2.x`, `1.2.*` | 1.2 and any 1.2.z |
| `1.2.3` | 1.2.3 (and 1.2.3.z) |
| `^1.2.3` | ≥1.2.3 and <2.0.0. For 0.y: `^0.2.3` is <0.3.0, `^0.0.3` is <0.0.4 |
| `~1.2.3` | ≥1.2.3, <1.3.0 |
| `>=1.2`, `>1.2`, `<=2`, `<2` | the comparison |

A leading `v` is ignored. Before Nov 15, `^` was ignored, so `^1.2` meant
`1.2`. Now it means ≥1.2 and <2.

## The public API a minor version keeps (CU5)

`ark publish` compares the public API of the commit tagged `v<version>`
with the API of the newest release below that version in the index:

- **A new major version** (x+1, or 0.y+1 while the major is 0) may change
  anything.
- **A minor or patch version** has to keep every item of the earlier
  release unchanged. New items are allowed. If an item was removed or
  changed, publish is refused and each such item is listed:

  ```
  ark: pkg 1.1.0 is a minor version after 1.0.0, but its public API does not keep 1.0.0's:
    removed: pub fn shout(name: str) -> str  (src/lib.fd)
  Keep those items, or publish a new major version.
  ```

The API is every `pub` item in the package's `.fd` files, except files
under `tests/`, `examples/`, `bench/`, `ark_modules/` and `vendor/`:

- `pub fn` signatures (a method in an `impl` block is named with its
  type);
- `pub struct`, `pub enum` and `pub trait`, each with its whole body (a
  new field or variant can break code that builds the struct or matches on
  the enum);
- `pub const` and `pub static` with their types;
- `pub type`.

Comments, strings and whitespace do not count. The earlier release's
commit has to be in the local repository. If it is not, ark fetches tags
from `origin`, and refuses to publish if it still cannot read it.

`ark api` prints the API of the working tree. `ark api --against <rev>`
lists what changed since `<rev>` (`removed:`, `changed:`, `added:`) and
exits 1 if anything breaks, so you can check before tagging.

## Dependency features (L5A)

```toml
[dependencies]
feat = { path = "../feat", features = ["quiet", "trace"] }
c = { version = "^1.2", features = ["simd"] }   # a registry pact

[features]            # this package's own
default = ["fast"]
fast = []
full = ["fast", "extra"]
```

- `ark add <name> ... --features a,b` (or `--feature a`, again for more)
  writes the `features = [...]` of the pact's line. Every rewrite of
  ark.toml keeps it.
- `ark install` writes `ark_modules/.features` (and
  `ark_modules/.dev/.features` for the dev pacts). It has one line
  `name = ["a", "b"]` for each package that has features on:
  - for each pact: the features every requirement on it asks for (from
    the project's ark.toml and from the pacts' own), its `[features]`
    `default` (left out when a request lists `"-default"`), and the
    features those name in its `[features]` table, followed until nothing
    new is added;
  - for the project itself: its `default` and the features that names.
  A `dep/feature` entry (or `dep?/feature`) turns `feature` on in pact
  `dep`, whose own [features] are then followed; an entry naming a
  package that is not a pact gets a warning (not for `dep?/`). ark.lock
  records each pact's resolved features (`features = [...]`), and `ark
  lock` keeps them. `ark list` shows each pact's requested features.
- A build keeps an item marked `@cfg(feature = "x")` only when `x` is
  listed: compiler.fd `cfg_ark_features` reads `ark_modules/.features`.
  The file lists features per package, but the check uses all the listed
  names together.

## Build targets (L5A)

```toml
[[bin]]
name = "hello"
path = "src/hello.fd"

[[bin]]
name = "bye"
path = "src/bye.fd"

[[lib]]
name = "util"
path = "src/util.fd"
kind = "check"        # or "shared": a shared library (fid build --lib)

[[test]]
name = "util_test"
path = "tests/util_test.fd"

[target]
dir = "out"           # where ark build writes; default "build"
# or, by path only (the name is the file's stem):
# bin = ["src/a.fd", "src/b.fd"]
# lib = "src/lib.fd"
# test = ["tests/a.fd"]
```

With no target declared, the defaults are: bin `src/main.fd` (or
`main.fd`), named after the package; lib `src/lib.fd`; tests
`tests/*.fd`. A missing path is filled in too: a `[[bin]]` named after the
package is `src/main.fd`, any other bin is `src/bin/<name>.fd`, a
`[[lib]]` is `src/lib.fd`, and a `[[test]]` is `tests/<name>.fd`.

- `ark build [name...] [--release|--debug] [--target <triple>] [--dry-run]`
  builds every bin with `fid build <path> -o <dir>/<name>` and checks
  every lib with `fid check <path>`. Other packages `use` a lib's source,
  so a lib needs no output file unless `kind = "shared"`, which runs `fid
  build <path> --lib`. If you give names, only those targets are built.
  `--dry-run` prints the commands without running them. ark goes on after
  a target fails and exits 1 at the end.
- `ark test [name...] [--dry-run]` runs `fid test <path>` on each test
  target.
- `ark targets` lists the targets, and `ark list` shows them under
  TARGETS.
- ark runs the compiler named by `ARK_FID`. Without it, ark uses the fid
  that is running it (`fid ark build`), or `fid` from PATH for a
  standalone `ark`.

`fid build` itself still takes one file. It does not read ark.toml
(roadmap L5B).

## ark update within the version constraints (L5C)

`ark install` keeps the version that ark.lock records, as long as every
requirement still accepts it. `ark update` ignores the lock and takes the
newest version in the index that every requirement accepts. For `"^1.2"`
that is the newest below 2.0.0, and for `"~1.2"` the newest below 1.3.0.
A git pact gets its newest commit. ark then installs the result and
rewrites ark.lock. `ark update <name>` updates only that pact, and the
others keep their locked versions.

## Workspaces (L5F)

```toml
[workspace]
members = ["app", "libs/*"]
exclude = ["libs/old"]
```

An ark.toml with `[workspace] members` is a workspace root. A member is a
directory that has its own ark.toml. `dir/*` stands for every such
directory in `dir`, and `exclude` takes members out again. In the root,
`list`, `audit`, `vendor`, `build`, `test` and `targets` run in each
member in turn. If the root also has a `[package]`, they run in the root
first. When any member fails, the command exits 1, but only after every
member has run, and the failing members are named. `--no-workspace` runs
the command in the root alone.

**Shared dependencies.** `ark install`, `ark update [name]` and `ark lock`
in the root resolve the pacts of the root and of every member together:
one version of each pact for the whole workspace (a member's path pact is
read relative to that member). Each pact is installed once, in the root's
`ark_modules/`, and recorded once, in the root's ark.lock; ark prints
`<pact>: shared by a, b` for each pact several members need. Two members
that want different copies of a pact are a conflict, reported with both
paths (`app -> shared wants path shared/shared.fd`, `two -> shared wants
path other/shared.fd`). The members get no `ark_modules/` and no ark.lock
of their own. Run in a member, `install`, `update` and `lock` run the
workspace's at the root, and `audit`, `list` and `build` read the root's
ark.lock and copies. `[capabilities]` (CD1) is read from the root.

A build in a member finds the root's copies once compiler.fd's
`ark_push_candidates` also looks in the workspace root's `vendor/` and
`ark_modules/` (the nearest ark.toml above with a `[workspace]` that
lists the member). Until then, `fid build` run in a member does not find
the shared pacts. `--no-workspace` in a member installs it on its own, as
before.

## Yanking a version (N6)

```
ark yank 1.1.0               # this package's 1.1.0 (ark.toml's name)
ark yank 1.1.0 --package c   # another package's
ark yank 1.1.0 --undo
```

`ark yank` marks a published version in the index: its line gets a fifth
word, `version git-url tag commit yanked`. The change is committed and
pushed like `ark publish`. If the push fails, the commit stays in the
local index for a pull request. A yanked version is never chosen for a
new lock, whether by `ark add --version`, by `ark install` for a pact
that ark.lock does not record yet, or by `ark update`. `ark search` shows
the newest version that is not yanked. A project whose ark.lock already
records the yanked commit keeps installing it, with a warning, until
`ark update` moves it to another version. `--undo` removes the mark. The
line itself is never removed, and a yanked version cannot be published
again, because versions never change.

## Capabilities of each dependency (CD1)

```toml
[capabilities]
json = []                    # pure code: nothing
http_client = ["net"]
tool = ["fs", "process"]     # "*" grants everything
```

When ark.toml has a `[capabilities]` section, every pact the project uses
is checked against it, including the pacts other pacts need. A pact that
is not listed is granted nothing. `ark install`, `ark build`, `ark test`
and `ark audit` fail when a pact uses a capability it is not granted, and
the error names the file and the call:

```
ark: pact 'netpact' uses capabilities ark.toml does not grant it:
  net  ark_modules/netpact/src/lib.fd (p_tcp_)
  grant them under [capabilities] (netpact = ["net"]) or drop the pact
```

ark scans each pact's `.fd` files to see what it uses: `net` (sockets,
TCP/UDP, HTTP, DNS, `use net`, `use http...`, `use ws...`, `use dns`,
`use tls_...`), `fs` (`p_file_*`, `p_dir_*`, `read_file(`, `write_file(`,
`use fs`, `use file`, `use mmap`), `process` (`p_system(`, `p_process_*`,
`use process`), `env` (`p_getenv`, `env_var(`, `use env`), `ffi`
(`extern`, `link "..."`), `unsafe` (`unsafe {`), and whatever its imports
ask for (`use core with { IO, Crypto }` gives `io` and `crypto`). The scan
reads text, so a call in a comment counts as well. `ark audit` prints
`uses: ...` for each pact. `ark update` prints what changed, for example
`netpact: +net` when a pact now uses something it did not use before. It
prints the diff even when there is no `[capabilities]` section, and then
checks the grants.

The check runs in ark only. `fid build` run directly does not check
`[capabilities]` yet. That needs the driver to call the same check.

## Binary packages (L5E)

A package can ship a prebuilt library for each target. A project that
asks for it links the library and does not compile the package's source.

In the package, `ark prebuild` builds the lib target (`src/lib.fd`, or
the `[[lib]]` path) with `fid build --lib` into `prebuilt/<target>/`:

```
prebuilt/x86_64-windows/mathx.dll       # lib<name>.so on Linux, lib<name>.dylib on macOS
prebuilt/x86_64-windows/mathx.h         # the C header fid writes
prebuilt/x86_64-windows/prebuilt.toml
```

```toml
# prebuilt.toml -- written by ark prebuild
name = "mathx"
version = "0.1.0"
target = "x86_64-windows"
library = "mathx.dll"
link = "mathx"
library-hash = "fnv:...:789000"   # the library's bytes
source = "src"
source-hash = "fnv:..."           # the .fd files it was built from (CR ignored)
fn = "mx_add(a: i64, b: i64) -> i64"
fn = "mx_scale(x: f64) -> f64"
```

The library exports the lib's `@export` functions. When the lib has none,
it exports every top-level `pub fn`. Only integers (`i8`..`u64`), floats
and `bool` cross the library, because the library and the program each
have their own Fides runtime and allocator. A `str`, `Vec`, struct or
generic function stops `ark prebuild` and names the function. Ship the
other functions as source, or wrap them in `@export` functions with
scalar types. ark writes a shim beside the source for the build and
removes it after. The shim gives each export the C name
`ark_bin_<package>_<fn>`, so the library's symbols cannot collide with
Fides names.

- `--target <t>`: another target (`aarch64-linux`, `x86_64-macos`, and
  also `linux-arm64` or `x64-windows`). fid cross-compiles through
  `zig cc`.
- `--from <library>`: package a library built another way. It must export
  the `ark_bin_` names that `--dry-run` lists.
- `--check`: build again and compare with the recorded hash. The builds
  are reproducible (complaint CB6), so the bytes should be the same.
- `--dry-run`: print the build command and the functions.

The package then commits `prebuilt/` and publishes as usual, with a tag
or `ark publish`.

In a project, set `binary = true` on the pact's line (`ark add <name> ...
--binary`):

```toml
[dependencies]
mathx = { git = "https://example.org/mathx", binary = true }
```

`ark install` (and `ark update`) look in the installed pact for
`prebuilt/<target>/`. The target is `ARK_TARGET`, else this machine. The
library must match `library-hash`, or the install fails. The installed
source must match `source-hash`. If it does not, the prebuilt library is
stale and ark prints a warning and uses the source. When both match, ark
writes `ark_modules/<name>.fd`, the binary interface:

```
// ark_modules/mathx.fd -- the binary interface of pact mathx 0.1.0 (x86_64-windows)
// DO NOT EDIT -- generated by ark install (L5E) from mathx/prebuilt/x86_64-windows/mathx.dll:
extern "C" link "mathx" {
    fn ark_bin_mathx_mx_add(a: i64, b: i64) -> i64;
    ...
}

pub fn mx_add(a: i64, b: i64) -> i64 {
    unsafe {
        return ark_bin_mathx_mx_add(a, b);
    }
}
```

`use mathx` finds `ark_modules/mathx.fd` before `ark_modules/mathx/src/lib.fd`,
because it is the first place compiler.fd `ark_push_candidates` looks. So
the program calls the library and the source is not compiled. ark
generates the interface from the `fn` lines, not from a file in the
package, so the interface holds nothing but the extern block and one call
per function. ark writes the interface for pacts in `ark_modules/` only.
A vendored pact (`vendor/`, CD9) is found first and is always compiled
from source.

- `ark install --binary`: use every pact's prebuilt library when it ships
  one. `ark install --source`: use none. Without `binary = true` or
  `--binary`, a pact is compiled from source, as before.
- `ark_modules/.binaries` lists the chosen libraries. `ark build` copies
  each one beside what it builds, which is where Windows looks for a DLL.
  `ark build` and `ark test` set `FID_LIB_PATH` to the libraries'
  directories: fid's C path passes them as `-L`, and on Linux adds the
  `$ORIGIN` rpath, so the program finds the library beside itself. Off
  Windows, a bin that links a prebuilt library is built with `fid
  ccompile` (and the directories also go on `LIBRARY_PATH`). `ark test`
  also puts the libraries on `PATH` (Windows) or on `LD_LIBRARY_PATH` and
  `DYLD_LIBRARY_PATH`.
- `ark prebuild --check` rebuilds to the same file, compares, and puts the
  recorded library back.
- `ark audit` checks each library against its recorded hash and prints
  `prebuilt <path> <hash>`, or `PREBUILT CHANGED`.
- `ark remove` removes the interface with the pact. `ark list` shows
  `binary`.

What is not done: the capability scan (CD1) reads the pact's source.
The binary is trusted to be built from that source, which is the meaning
of the source hash, and `ark prebuild --check` is the way to confirm it.
Prebuilt IR is not shipped, so generic functions, methods and non-scalar
types still need the source. In a workspace, `binary = true` is read from
the root's ark.toml only, or use `--binary`. The hashes are FNV-1a like
ark.lock's, not signatures.

## Checks

All four parts above about project features are checked by
`tests/ark/ark-projects.sh [command that runs ark]`. Shared workspace
dependencies, git pacts that need git pacts (TL9), yanking and
capabilities are checked by `tests/ark/ark-registry.sh [command]`.
`tests/ark/ark-selfbuild.sh` builds `tools/ark.fd` with fid through C
and runs every `tests/ark/*.sh` against the ark it built (AD1). Set
`FID=<compiler>` to make ark-projects.sh build the bins too, run them,
run `ark test`, and check `@cfg(feature)`. The `@cfg(feature)` check
fails until compiler.fd reads `ark_modules/.features`.
`tests/ark/ark-binary.sh [command]` checks binary packages (L5E):
`ark prebuild`, `binary = true`, the interface, the hash checks, stale
and tampered libraries, and `--source`. Without `FID`, it packages a C
library made by `zig cc` (or stand-in bytes) with `--from`. With
`FID=<compiler>` (one that has `fid build --lib` and the `ark_modules/`
lookup), `ark prebuild` builds the library, and the project's program is
built with `ark build` and run. The check confirms that it prints the
library's results and that it stops when the library is missing.
