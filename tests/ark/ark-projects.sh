#!/bin/bash
# ark-projects.sh -- checks of ark's project features (docs/reference/ark.md):
#   L5A  dependency features: `ark add --features a,b` writes
#        `features = [...]` into the pact's line, every rewrite keeps it,
#        `{ version = "...", features = [...] }` is a registry pact;
#        `ark install` writes ark_modules/.features: each pact's features
#        (what every requirement on it asks for, its [features] default,
#        the features those name) and the project's own default;
#        `dep/feature` (and `dep?/feature`) entries in [features] turn on
#        a pact's feature; ark.lock records each pact's features
#   L5A  build targets: [target] bin/lib/test, [[bin]] (two), [[lib]],
#        [[test]], and the defaults (src/main.fd, src/lib.fd, tests/*.fd):
#        `ark targets`, `ark build --dry-run`, `ark test --dry-run`
#   L5C  `ark update`: the newest version within each constraint (^, ~),
#        not past it; ark.lock rewritten; `ark install` keeps the lock
#   L5F  [workspace] members (a list, `dir/*`, exclude): `ark list` and
#        `ark install` run in every member; a broken member fails the
#        command; --no-workspace
# With FID set to a compiler (FID=release/fid.exe), also builds for real:
#   two bins and a checked lib built by `ark build` (the bins run), `ark
#   test`, and a pact's @cfg(feature = "x") code chosen by its features
#   (needs compiler.fd check_cfg to read ark_modules/.features). `fid
#   test` compiles through C: set CC (CC="zig cc") when none is on PATH; a
#   fid outside the repository needs FID_HOME=<repo> for its stdlib.
# Everything is local (git repositories in a temporary directory).
#
# Usage: tests/ark/ark-projects.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, ...)
#   e.g.     FID=release/fid.exe tests/ark/ark-projects.sh ./ark.exe
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
if [ $# -gt 0 ]; then
  ARK=("$@")
else
  ARK=()
  # prefer the newest working compiler: release/fid.exe can be weeks old
  # and predate the very features these scripts check (2g.63's lesson)
  for f in release/fid-dev.exe release/fid.exe release/fid-dev release/fid; do
    if [ -x "$f" ]; then ARK=("$ROOT/$f" ark); break; fi
  done
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-projects: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac
FID="${FID:-}"
if [ -n "$FID" ]; then
  case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
  export ARK_FID="$(cygpath -m "$FID" 2>/dev/null || echo "$FID")"
else
  unset ARK_FID
fi

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-projects.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
W="$(cygpath -m "$WORK" 2>/dev/null || echo "$WORK")"
mkdir -p "$W/home"
export ARK_HOME="$W/home" HOME="$W/home" USERPROFILE="$W/home"
export ARK_INDEX="$W/index.git"
unset ARK_CACHE
export GIT_AUTHOR_NAME=ark-test GIT_AUTHOR_EMAIL=ark-test@example.invalid
export GIT_COMMITTER_NAME=ark-test GIT_COMMITTER_EMAIL=ark-test@example.invalid
export GIT_CONFIG_NOSYSTEM=1

fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
run() {
  local dir="$1" log="$2"
  shift 2
  (cd "$W/$dir" && "${ARK[@]}" "$@") > "$W/$log" 2>&1
}
expect_ok() {
  local label="$1"
  shift
  if run "$@"; then ok "$label"; else bad "$label (exit $?):"; sed 's/^/        /' "$W/$2" | head -20; fi
}
expect_fail() {
  local label="$1"
  shift
  if run "$@"; then bad "$label: succeeded"; sed 's/^/        /' "$W/$2" | head -20; else ok "$label"; fi
}
has() {
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $(basename "$2")"; sed 's/^/        /' "$2" | head -30; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $(basename "$2")"; else ok "$1"; fi
}
exists() { if [ -e "$W/$2" ]; then ok "$1"; else bad "$1: no $2"; fi; }
absent() { if [ -e "$W/$2" ]; then bad "$1: $2 exists"; else ok "$1"; fi; }
sha() { git -C "$W/$1" rev-parse "$2^{commit}"; }

echo "== ark-projects (${ARK[*]})"

# --- L5A: dependency features ------------------------------------------------
echo "-- L5A dependency features"
mkdir -p "$W/feat" "$W/mid" "$W/fapp"
printf '[package]\nname = "feat"\nversion = "0.1.0"\n\n[features]\ndefault = ["std"]\nfull = ["loud", "extra"]\n' > "$W/feat/ark.toml"
cat > "$W/feat/lib.fd" <<'EOF'
@cfg(feature = "loud")
pub fn feat_mode() -> str {
    return "loud";
}

@cfg(feature = "quiet")
pub fn feat_mode() -> str {
    return "quiet";
}
EOF
# mid needs feat with "extra": the one copy of feat gets both requests
printf '[package]\nname = "mid"\nversion = "0.1.0"\n\n[dependencies]\nfeat = { path = "../feat", features = ["extra"] }\n' > "$W/mid/ark.toml"
echo 'pub fn mid() -> i64 { return 1; }' > "$W/mid/lib.fd"
expect_ok "ark init" fapp init.log init fapp
printf '\n[features]\ndefault = ["fast"]\nfast = []\n' >> "$W/fapp/ark.toml"
expect_ok "ark add --features" fapp add.log add feat --path ../feat --features quiet,trace
has "the features in the pact's line" "$W/fapp/ark.toml" 'feat = { path = "../feat", features = ["quiet", "trace"] }'
expect_ok "ark add another pact (ark.toml rewritten)" fapp add2.log add mid --path ../mid
has "the features kept" "$W/fapp/ark.toml" 'feat = { path = "../feat", features = ["quiet", "trace"] }'
has "the project's [features] kept" "$W/fapp/ark.toml" 'default = ["fast"]'
expect_ok "ark list" fapp list.log list
has "ark list shows the features" "$W/list.log" 'features: quiet, trace'
expect_ok "ark install" fapp install.log install
exists "ark_modules/.features written" fapp/ark_modules/.features
has "feat: the project's, mid's and its default features" "$W/fapp/ark_modules/.features" 'feat = ["quiet", "trace", "extra", "default", "std"]'
has "the project's own default features" "$W/fapp/ark_modules/.features" 'fapp = ["default", "fast"]'
hasnt "mid has none" "$W/fapp/ark_modules/.features" 'mid ='
# -default: no default features; a feature naming others brings them in
printf '[package]\nname = "fapp2"\n\n[dependencies]\nfeat = { path = "../feat", features = ["-default", "full"] }\n' > "$W/fapp/ark.toml.2"
mkdir -p "$W/fapp2" && mv "$W/fapp/ark.toml.2" "$W/fapp2/ark.toml"
expect_ok "ark install (-default, full)" fapp2 install2.log install
has "full and what it names, no default" "$W/fapp2/ark_modules/.features" 'feat = ["full", "loud", "extra"]'
# a registry pact with features
mkdir -p "$W/fapp3"
printf '[package]\nname = "fapp3"\n\n[dependencies]\nc = { version = "^1.2", features = ["x"] }\n' > "$W/fapp3/ark.toml"
expect_ok "ark list of a { version, features } pact" fapp3 list3.log list
has "it is a registry pact at that version" "$W/list3.log" 'registry  ^1.2  features: x'
# ark.lock records each pact's features
has "ark.lock: feat's features" "$W/fapp/ark.lock" 'features = ["quiet", "trace", "extra", "default", "std"]'
# `dep/feature` (and `dep?/feature`) entries in [features]: fapp4's default
# `boost` turns on feat's "loud"; mid3's default `m` turns on feat's "full"
# (which names "loud" and "extra"); "nopact/x" names no pact: a warning
mkdir -p "$W/mid3" "$W/fapp4"
printf '[package]\nname = "mid3"\nversion = "0.1.0"\n\n[dependencies]\nfeat = { path = "../feat" }\n\n[features]\ndefault = ["m"]\nm = ["feat?/full"]\n' > "$W/mid3/ark.toml"
echo 'pub fn mid3() -> i64 { return 3; }' > "$W/mid3/lib.fd"
printf '[package]\nname = "fapp4"\n\n[dependencies]\nfeat = { path = "../feat", features = ["-default"] }\nmid3 = { path = "../mid3" }\n\n[features]\ndefault = ["boost"]\nboost = ["feat/loud", "nopact/x"]\n' > "$W/fapp4/ark.toml"
expect_ok "ark install (dep/feature entries)" fapp4 install5.log install
has "feat: loud (fapp4's boost), full (mid3's m) and what full names" "$W/fapp4/ark_modules/.features" 'feat = ["loud", "full", "extra"]'
has "mid3: its default and m" "$W/fapp4/ark_modules/.features" 'mid3 = ["default", "m"]'
has "fapp4: its own features, no dep/feature entries" "$W/fapp4/ark_modules/.features" 'fapp4 = ["default", "boost"]'
hasnt "no dep/feature entry in .features" "$W/fapp4/ark_modules/.features" 'feat/loud'
has "a dep/feature naming no pact: warned" "$W/install5.log" "'nopact/x', but 'nopact' is not one of the pacts installed"
hasnt "no warning for the weak feat?/full" "$W/install5.log" "feat?/full"
has "ark.lock: feat's resolved features" "$W/fapp4/ark.lock" 'features = ["loud", "full", "extra"]'
has "ark.lock: mid3's resolved features" "$W/fapp4/ark.lock" 'features = ["default", "m"]'
expect_ok "ark lock (from what is installed)" fapp4 lock5.log lock
has "ark lock keeps feat's features" "$W/fapp4/ark.lock" 'features = ["loud", "full", "extra"]'

# --- L5C: ark update within version constraints ------------------------------
echo "-- L5C ark update within the constraints"
mkdir -p "$W/clib/src"
printf '[package]\nname = "c"\nversion = "1.2.0"\n' > "$W/clib/ark.toml"
mkver() { # mkver <version>: c at that version, committed and tagged v<version>
  printf 'pub fn c_version() -> str {\n    return "%s";\n}\n' "$1" > "$W/clib/src/lib.fd"
  if [ ! -d "$W/clib/.git" ]; then (cd "$W/clib" && git init -q); fi
  (cd "$W/clib" && git add -A && git commit -q -m "$1" && git tag -f "v$1" > /dev/null) || { echo "git failed"; exit 1; }
}
publish() { # publish <version...>: those versions added to the index
  for v in "$@"; do mkver "$v"; echo "$v $W/clib v$v $(sha clib "v$v")" >> "$W/index-seed/c"; done
  (cd "$W/index-seed" && git add -A && git commit -q -m "c $*" && git push -q origin HEAD 2> /dev/null) || { echo "cannot update the index"; exit 1; }
}
git init -q --bare "$W/index.git"
git clone -q "$W/index.git" "$W/index-seed" 2> /dev/null
publish 1.2.0
mkdir -p "$W/up1" "$W/up2"
printf '[package]\nname = "up1"\n\n[dependencies]\nc = "^1.2"\n' > "$W/up1/ark.toml"
printf '[package]\nname = "up2"\n\n[dependencies]\nc = "~1.2"\n' > "$W/up2/ark.toml"
expect_ok "up1: ark install (only 1.2.0 published)" up1 up1-install.log install
has "up1: 1.2.0 locked" "$W/up1/ark.lock" 'version = "1.2.0"'
expect_ok "up2: ark install" up2 up2-install.log install
publish 1.2.3 1.3.0 2.0.0
expect_ok "up1: ark install after newer versions (the lock holds)" up1 up1-again.log install
has "up1: still 1.2.0" "$W/up1/ark.lock" 'version = "1.2.0"'
expect_ok "up1: ark update" up1 up1-update.log update
has "up1: ^1.2 -> 1.3.0 (the newest below 2.0.0)" "$W/up1/ark.lock" 'version = "1.3.0"'
hasnt "up1: not 2.0.0" "$W/up1/ark.lock" 'version = "2.0.0"'
has "up1: ark.lock has 1.3.0's commit" "$W/up1/ark.lock" "resolved = \"$(sha clib v1.3.0)\""
has "up1: 1.3.0 installed" "$W/up1/ark_modules/c/src/lib.fd" '"1.3.0"'
expect_ok "up2: ark update c" up2 up2-update.log update c
has "up2: ~1.2 -> 1.2.3 (the newest below 1.3.0)" "$W/up2/ark.lock" 'version = "1.2.3"'
has "up2: 1.2.3 installed" "$W/up2/ark_modules/c/src/lib.fd" '"1.2.3"'

# --- L5A: build targets ------------------------------------------------------
echo "-- L5A [target], [[bin]], [[lib]], [[test]]"
mkdir -p "$W/tapp/src" "$W/tapp/tests"
cat > "$W/tapp/ark.toml" <<'EOF'
[package]
name = "tapp"
version = "0.1.0"

[[bin]]
name = "hello"
path = "src/hello.fd"

[[bin]]
name = "bye"
path = "src/bye.fd"

[[lib]]
name = "util"
path = "src/util.fd"

[[test]]
name = "util_test"
path = "tests/util_test.fd"

[target]
dir = "out"
EOF
printf 'fn main() -> i64 {\n    print("hello from tapp\\n");\n    return 0;\n}\n' > "$W/tapp/src/hello.fd"
printf 'fn main() -> i64 {\n    print("bye from tapp\\n");\n    return 0;\n}\n' > "$W/tapp/src/bye.fd"
printf 'pub fn twice(x: i64) -> i64 {\n    return x * 2;\n}\n' > "$W/tapp/src/util.fd"
printf 'fn twice(x: i64) -> i64 {\n    return x * 2;\n}\n\nfn test_twice() {\n    assert(twice(2) == 4);\n}\n' > "$W/tapp/tests/util_test.fd"
expect_ok "ark targets" tapp targets.log targets
has "bin hello" "$W/targets.log" 'bin hello src/hello.fd'
has "bin bye" "$W/targets.log" 'bin bye src/bye.fd'
has "lib util (checked)" "$W/targets.log" 'lib util src/util.fd (check)'
has "test util_test" "$W/targets.log" 'test util_test tests/util_test.fd'
expect_ok "ark build --dry-run" tapp build-dry.log build --dry-run
has "builds hello into [target] dir" "$W/build-dry.log" ' build src/hello.fd -o out/hello'
has "builds bye" "$W/build-dry.log" ' build src/bye.fd -o out/bye'
has "checks util" "$W/build-dry.log" ' check src/util.fd'
hasnt "no test in a build" "$W/build-dry.log" 'util_test'
expect_ok "ark build --dry-run --release bye" tapp build-one.log build --dry-run --release bye
has "only bye, with --release" "$W/build-one.log" ' build src/bye.fd -o out/bye --release'
hasnt "not hello" "$W/build-one.log" 'hello'
expect_fail "ark build of an unknown target fails" tapp build-none.log build --dry-run nosuch
expect_ok "ark test --dry-run" tapp test-dry.log test --dry-run
has "fid test on the test target" "$W/test-dry.log" ' test tests/util_test.fd'
expect_ok "ark list shows the targets" tapp tlist.log list
has "TARGETS in ark list" "$W/tlist.log" 'bin hello  src/hello.fd'
expect_ok "ark add keeps [[bin]]" tapp tadd.log add util2 --path ../feat
n=$(grep -c '^\[\[bin\]\]' "$W/tapp/ark.toml")
if [ "$n" -eq 2 ]; then ok "both [[bin]] tables kept"; else bad "$n [[bin]] tables after ark add"; fi
expect_ok "ark targets after the rewrite" tapp targets2.log targets
has "bin bye still there" "$W/targets2.log" 'bin bye src/bye.fd'
# [target] keys and the defaults
mkdir -p "$W/tapp2/src" "$W/tapp3/src" "$W/tapp3/tests"
printf '[package]\nname = "tapp2"\n\n[target]\nbin = ["src/a.fd", "src/b.fd"]\nlib = "src/l.fd"\ntest = ["t1.fd"]\n' > "$W/tapp2/ark.toml"
expect_ok "ark targets ([target] keys)" tapp2 targets3.log targets
has "[target] bin a" "$W/targets3.log" 'bin a src/a.fd'
has "[target] bin b" "$W/targets3.log" 'bin b src/b.fd'
has "[target] lib" "$W/targets3.log" 'lib l src/l.fd (check)'
has "[target] test" "$W/targets3.log" 'test t1 t1.fd'
printf '[package]\nname = "tapp3"\n' > "$W/tapp3/ark.toml"
echo 'fn main() -> i64 { return 0; }' > "$W/tapp3/src/main.fd"
echo 'pub fn l() -> i64 { return 0; }' > "$W/tapp3/src/lib.fd"
echo 'fn test_x() { assert(true); }' > "$W/tapp3/tests/x.fd"
expect_ok "ark targets (defaults)" tapp3 targets4.log targets
has "default bin: src/main.fd named after the package" "$W/targets4.log" 'bin tapp3 src/main.fd'
has "default lib: src/lib.fd" "$W/targets4.log" 'lib tapp3 src/lib.fd (check)'
has "default tests: tests/*.fd" "$W/targets4.log" 'test x tests/x.fd'

# --- L5F: workspaces ---------------------------------------------------------
echo "-- L5F [workspace]"
mkdir -p "$W/ws/app/src" "$W/ws/libs/one" "$W/ws/libs/two" "$W/ws/libs/old" "$W/ws/shared"
printf '[workspace]\nmembers = [\n    "app",\n    "libs/*",\n]\nexclude = ["libs/old"]\n' > "$W/ws/ark.toml"
echo 'pub fn shared() -> i64 { return 5; }' > "$W/ws/shared/shared.fd"
printf '[package]\nname = "app"\n\n[dependencies]\nshared = { path = "../shared/shared.fd" }\n' > "$W/ws/app/ark.toml"
printf '[package]\nname = "one"\n\n[dependencies]\nshared = { path = "../../shared/shared.fd" }\n' > "$W/ws/libs/one/ark.toml"
printf '[package]\nname = "two"\n' > "$W/ws/libs/two/ark.toml"
printf '[package]\nname = "old"\n\n[dependencies]\nnothere = { path = "../../nothere" }\n' > "$W/ws/libs/old/ark.toml"
expect_ok "ark list in the workspace root" ws wslist.log list
has "the members" "$W/wslist.log" 'Workspace: 3 member(s): app, libs/one, libs/two'
has "app listed" "$W/wslist.log" 'app v0.1.0'
has "one listed" "$W/wslist.log" 'one v0.1.0'
has "two listed" "$W/wslist.log" 'two v0.1.0'
hasnt "the excluded member left out" "$W/wslist.log" 'old v0.1.0'
expect_ok "ark install in the workspace root" ws wsinstall.log install
# L5F shared dependencies (tests/ark/ark-registry.sh checks more): one
# resolution, one copy in the root, one ark.lock
exists "the members' pact installed once, in the root" ws/ark_modules/shared.fd
exists "the root's one ark.lock" ws/ark.lock
absent "no copy in app/" ws/app/ark_modules
absent "no ark.lock in libs/one/" ws/libs/one/ark.lock
expect_ok "ark targets in the root" ws wstargets.log targets
# a root that is a package too
printf '[package]\nname = "wsroot"\n\n[workspace]\nmembers = ["app"]\n' > "$W/ws/ark.toml"
expect_ok "ark list (root package + member)" ws wslist2.log list
has "the root's own list" "$W/wslist2.log" 'wsroot v0.1.0'
has "and the member's" "$W/wslist2.log" 'app v0.1.0'
expect_ok "ark list --no-workspace" ws wslist3.log list --no-workspace
hasnt "only the root" "$W/wslist3.log" 'app v0.1.0'
# a member that cannot install fails the command; one that fails a
# per-member command fails it after the others ran
printf '[workspace]\nmembers = ["libs/old", "libs/two"]\n' > "$W/ws/ark.toml"
expect_fail "ark install with a broken member fails" ws wsbad.log install
has "it names the pact" "$W/wsbad.log" "Path pact 'nothere' cannot be read"
expect_fail "ark audit with a broken member fails" ws wsbad2.log audit
has "it names the member" "$W/wsbad2.log" 'ark audit failed in libs/old'
has "the other member still ran" "$W/wsbad2.log" '== libs/two'

# --- with a compiler: real builds -------------------------------------------
if [ -n "$FID" ]; then
  echo "-- builds with $FID"
  expect_ok "ark build (two bins, one lib)" tapp build.log build
  has "it built three targets" "$W/build.log" 'Built 3 target(s)'
  out=$("$W/tapp/out/hello" 2>&1 || "$W/tapp/out/hello.exe" 2>&1)
  if [ "$out" = "hello from tapp" ]; then ok "bin hello runs"; else bad "bin hello: '$out'"; fi
  out=$("$W/tapp/out/bye" 2>&1 || "$W/tapp/out/bye.exe" 2>&1)
  if [ "$out" = "bye from tapp" ]; then ok "bin bye runs"; else bad "bin bye: '$out'"; fi
  printf 'pub fn twice(x: i64) -> i64 {\n    return nosuch(x);\n}\n' > "$W/tapp/src/util.fd"
  expect_fail "ark build fails when the lib does not check" tapp build-bad.log build util
  expect_ok "ark test" tapp test.log test
  # @cfg(feature) in a pact: fapp asks feat for "quiet"
  printf 'use feat;\n\nfn main() -> i64 {\n    print(feat_mode() + "\\n");\n    return 0;\n}\n' > "$W/fapp2/main.fd"
  printf '[package]\nname = "fapp2"\n\n[dependencies]\nfeat = { path = "../feat", features = ["-default", "quiet"] }\n' > "$W/fapp2/ark.toml"
  expect_ok "ark install (quiet)" fapp2 install4.log install
  expect_ok "ark build (default bin main.fd)" fapp2 build4.log build
  out=$("$W/fapp2/build/fapp2" 2>&1 || "$W/fapp2/build/fapp2.exe" 2>&1)
  if [ "$out" = "quiet" ]; then ok "@cfg(feature = \"quiet\") chosen by the pact's features"; else bad "@cfg(feature): '$out' (compiler.fd check_cfg reads ark_modules/.features?)"; fi
  # the same, with "quiet" turned on by a `feat/quiet` entry of the
  # project's own [features] default
  mkdir -p "$W/fapp5"
  cp "$W/fapp2/main.fd" "$W/fapp5/main.fd"
  printf '[package]\nname = "fapp5"\n\n[dependencies]\nfeat = { path = "../feat", features = ["-default"] }\n\n[features]\ndefault = ["q"]\nq = ["feat/quiet"]\n' > "$W/fapp5/ark.toml"
  expect_ok "ark install (feat/quiet)" fapp5 install6.log install
  expect_ok "ark build (feat/quiet)" fapp5 build6.log build
  out=$("$W/fapp5/build/fapp5" 2>&1 || "$W/fapp5/build/fapp5.exe" 2>&1)
  if [ "$out" = "quiet" ]; then ok "@cfg(feature = \"quiet\") chosen by a dep/feature entry"; else bad "@cfg(feature) through dep/feature: '$out'"; fi
fi

if [ "$fail" = 0 ]; then echo "ark-projects: PASS"; else echo "ark-projects: FAIL"; fi
exit "$fail"
