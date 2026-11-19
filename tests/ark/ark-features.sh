#!/bin/bash
# ark-features.sh -- checks of ark's package features (docs/reference/ark.md):
#   L5A  ark init writes [package] authors, description and license; every
#        rewrite of ark.toml keeps them, and keeps keys and sections ark
#        does not know
#   L5A  [dev-dependencies]: `ark add --dev` puts a pact there; `ark install`
#        puts it in ark_modules/.dev/ (not ark_modules/), ark.lock says
#        `dev = true`; `ark install --no-dev` leaves it out
#   CD3  two projects with the same git pact share one copy of its
#        repository (the cache, ARK_HOME/.ark/cache/git): each checkout
#        borrows its objects; a third installs with the repository gone;
#        the checkout too: one per commit in the cache, ark_modules/<name>
#        a link to it (`ark update` moves one project's link only;
#        ARK_LINK=0 gives a checkout of its own)
#   CD4  a -> c "1" and b -> c "1.2": one version of c, the newest both
#        accept (1.2.3); a -> c "1" and b2 -> c "2": the install fails and
#        names both paths; two paths to e: the same
#   CU5  ark publish: a minor version that removes or changes a pub fn is
#        refused; one that only adds is published; a new major version may
#        remove; `ark api --against` shows the change
# Everything is local (git repositories in a temporary directory): no
# network.
#
# Usage: tests/ark/ark-features.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, release/fid ark)
#   e.g.     tests/ark/ark-features.sh release/fid-dev.exe ark
#            tests/ark/ark-features.sh ./ark.exe     (tools/ark.fd built)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
if [ $# -gt 0 ]; then
  ARK=("$@")
else
  ARK=()
  for f in release/fid.exe release/fid-dev.exe release/fid release/fid-dev; do
    if [ -x "$f" ]; then ARK=("$ROOT/$f" ark); break; fi
  done
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-features: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-features.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
# a path git, cmd.exe and ark all read the same way on Windows
W="$(cygpath -m "$WORK" 2>/dev/null || echo "$WORK")"
mkdir -p "$W/home"
export ARK_HOME="$W/home" HOME="$W/home" USERPROFILE="$W/home"
export ARK_INDEX="$W/index.git"
unset ARK_CACHE ARK_LINK
export GIT_AUTHOR_NAME=ark-test GIT_AUTHOR_EMAIL=ark-test@example.invalid
export GIT_COMMITTER_NAME=ark-test GIT_COMMITTER_EMAIL=ark-test@example.invalid
export GIT_CONFIG_NOSYSTEM=1

fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
# run <dir> <log> <args...>: ark in <dir>, output in $W/<log>; its exit code
run() {
  local dir="$1" log="$2"
  shift 2
  (cd "$W/$dir" && "${ARK[@]}" "$@") > "$W/$log" 2>&1
}
expect_ok() { # expect_ok <label> <dir> <log> <args...>
  local label="$1"
  shift
  if run "$@"; then ok "$label"; else bad "$label (exit $?):"; sed 's/^/        /' "$W/$2" | head -20; fi
}
expect_fail() { # expect_fail <label> <dir> <log> <args...>
  local label="$1"
  shift
  if run "$@"; then bad "$label: succeeded"; sed 's/^/        /' "$W/$2" | head -20; else ok "$label"; fi
}
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $(basename "$2")"; sed 's/^/        /' "$2" | head -30; fi
}
hasnt() { # hasnt <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $(basename "$2")"; else ok "$1"; fi
}
exists() { if [ -e "$W/$2" ]; then ok "$1"; else bad "$1: no $2"; fi; }
absent() { if [ -e "$W/$2" ]; then bad "$1: $2 exists"; else ok "$1"; fi; }
repo() { # repo <dir>: a git repository with what is in it committed
  (cd "$W/$1" && git init -q && git add -A && git commit -q -m init) || { echo "git failed in $1"; exit 1; }
}
commit_tag() { # commit_tag <dir> <tag>: commit everything, tag it (moving the tag)
  (cd "$W/$1" && git add -A && git commit -q -m "$2" && git tag -f "$2" > /dev/null) || { echo "git failed in $1"; exit 1; }
}
sha() { git -C "$W/$1" rev-parse "$2^{commit}"; }

echo "== ark-features (${ARK[*]})"

# --- the package index and a registry pact c with four versions ----------
mkdir -p "$W/clib/src"
printf '[package]\nname = "c"\nversion = "1.2.0"\n' > "$W/clib/ark.toml"
for v in 1.2.0 1.2.3 1.3.0 2.0.0; do
  printf 'pub fn c_version() -> str {\n    return "%s";\n}\n' "$v" > "$W/clib/src/lib.fd"
  if [ "$v" = 1.2.0 ]; then repo clib; git -C "$W/clib" tag v1.2.0; else commit_tag clib "v$v"; fi
done
git init -q --bare "$W/index.git"
git clone -q "$W/index.git" "$W/index-seed" 2> /dev/null
for v in 1.2.0 1.2.3 1.3.0 2.0.0; do
  echo "$v $W/clib v$v $(sha clib "v$v")" >> "$W/index-seed/c"
done
(cd "$W/index-seed" && git add -A && git commit -q -m c && git push -q origin HEAD 2> /dev/null) || { echo "cannot seed the index"; exit 1; }

# --- L5A: [package] fields ---------------------------------------------------
echo "-- L5A [package] authors, description, license"
mkdir -p "$W/app1" "$W/lib1"
echo 'pub fn one() -> i64 { return 1; }' > "$W/lib1/lib1.fd"
expect_ok "ark init with --author/--description/--license" app1 init.log init app1 --author "Ann <ann@example.org>" --author Bob --description 'A "demo" app' --license MIT
has "authors written" "$W/app1/ark.toml" 'authors = ["Ann <ann@example.org>", "Bob"]'
has "description written (escaped)" "$W/app1/ark.toml" 'description = "A \"demo\" app"'
has "license written" "$W/app1/ark.toml" 'license = "MIT"'
# a key and a section ark does not know
sed -i 's/^license = .*/&\nrepository = "https:\/\/example.org\/app1"/' "$W/app1/ark.toml"
printf '\n[profile]\nopt = 2\n' >> "$W/app1/ark.toml"
expect_ok "ark add rewrites ark.toml" app1 add.log add lib1 --path ../lib1
has "authors kept" "$W/app1/ark.toml" 'authors = ["Ann <ann@example.org>", "Bob"]'
has "description kept" "$W/app1/ark.toml" 'description = "A \"demo\" app"'
has "license kept" "$W/app1/ark.toml" 'license = "MIT"'
has "other [package] key kept" "$W/app1/ark.toml" 'repository = "https://example.org/app1"'
has "other section kept" "$W/app1/ark.toml" '[profile]'
has "its key kept" "$W/app1/ark.toml" 'opt = 2'
has "the pact added" "$W/app1/ark.toml" 'lib1 = { path = "../lib1" }'
expect_ok "ark list" app1 list.log list
has "ark list shows the description" "$W/list.log" 'description: A "demo" app'
has "ark list shows the authors" "$W/list.log" 'authors: Ann <ann@example.org>, Bob'
has "ark list shows the license" "$W/list.log" 'license: MIT'

# --- L5A: [dev-dependencies] -------------------------------------------------
echo "-- L5A [dev-dependencies]"
mkdir -p "$W/testkit/src"
echo 'pub fn tk() -> i64 { return 7; }' > "$W/testkit/src/lib.fd"
expect_ok "ark add --dev" app1 adddev.log add testkit --path ../testkit --dev
has "a [dev-dependencies] section" "$W/app1/ark.toml" '[dev-dependencies]'
has "the dev pact in it" "$W/app1/ark.toml" 'testkit = { path = "../testkit" }'
expect_ok "ark install" app1 install.log install
exists "the dev pact in ark_modules/.dev/" app1/ark_modules/.dev/testkit/src/lib.fd
absent "not in ark_modules/ (a normal build does not see it)" app1/ark_modules/testkit
exists "the normal pact in ark_modules/" app1/ark_modules/lib1/lib1.fd
has "ark.lock marks it" "$W/app1/ark.lock" 'dev = true'
expect_ok "ark audit with a dev pact" app1 audit.log audit
expect_ok "ark list" app1 list2.log list
has "ark list marks the dev pact" "$W/list2.log" 'path (dev)'
rm -rf "$W/app1/ark_modules" "$W/app1/ark.lock"
expect_ok "ark install --no-dev" app1 nodev.log install --no-dev
absent "no dev pact installed" app1/ark_modules/.dev/testkit
exists "the normal pact installed" app1/ark_modules/lib1/lib1.fd
expect_ok "ark remove of a dev pact" app1 rmdev.log remove testkit
hasnt "the section is gone" "$W/app1/ark.toml" '[dev-dependencies]'

# --- CD3: one copy per machine -----------------------------------------------
echo "-- CD3 shared cache"
mkdir -p "$W/gitlib/src" "$W/p1" "$W/p2" "$W/p3"
echo 'pub fn g() -> i64 { return 3; }' > "$W/gitlib/src/lib.fd"
repo gitlib
for p in p1 p2; do
  expect_ok "$p: ark init" "$p" init.log init "$p"
  expect_ok "$p: ark add --git" "$p" add.log add gitlib --git "$W/gitlib"
  expect_ok "$p: ark install" "$p" "$p-install.log" install
  exists "$p: the pact checked out" "$p/ark_modules/gitlib/src/lib.fd"
  exists "$p: the checkout borrows the cache's objects" "$p/ark_modules/gitlib/.git/objects/info/alternates"
done
n=$(ls -d "$W/home/.ark/cache/git/"gitlib-*.git 2> /dev/null | wc -l)
if [ "$n" -eq 1 ]; then ok "one copy of the repository in the cache"; else bad "$n copies in $W/home/.ark/cache/git"; fi
has "p1 borrows from the cache" "$W/p1/ark_modules/gitlib/.git/objects/info/alternates" "/.ark/cache/git/gitlib-"
has "p2 borrows from the same copy" "$W/p2/ark_modules/gitlib/.git/objects/info/alternates" "$(cat "$W/p1/ark_modules/gitlib/.git/objects/info/alternates" 2> /dev/null)"
# the repository gone: a third project still installs from the cache
mv "$W/gitlib" "$W/gitlib.gone"
expect_ok "p3: ark init" p3 init.log init p3
expect_ok "p3: ark add --git" p3 add.log add gitlib --git "$W/gitlib"
expect_ok "p3: ark install with the repository gone (the cache's copy)" p3 p3-install.log install
exists "p3: the pact checked out" p3/ark_modules/gitlib/src/lib.fd
mv "$W/gitlib.gone" "$W/gitlib"
# CD3 shared checkout: one working tree per commit, every project's
# ark_modules/gitlib a link to it (a junction on Windows)
n=$(ls -d "$W/home/.ark/cache/checkouts/"gitlib-*/* 2> /dev/null | wc -l)
if [ "$n" -eq 1 ]; then ok "one shared checkout of the commit in the cache"; else bad "$n checkouts in $W/home/.ark/cache/checkouts"; fi
echo probe > "$W/p1/ark_modules/gitlib/.ark-probe"
for p in p2 p3; do
  exists "$p sees the file written through p1's ark_modules/gitlib (one working tree)" "$p/ark_modules/gitlib/.ark-probe"
done
rm -f "$W/p1/ark_modules/gitlib/.ark-probe"
expect_ok "p1: ark install again (the link kept)" p1 p1-again.log install
hasnt "p1: no warning about the shared checkout" "$W/p1-again.log" "warning:"
# a program in p1 builds through the link (when the command is fid ark)
if [ "${#ARK[@]}" -ge 2 ] && [ "${ARK[1]}" = ark ]; then
  printf 'use gitlib;\n\nfn main() -> i64 {\n    println(g());\n    return 0;\n}\n' > "$W/p1/main.fd"
  out=$(cd "$W/p1" && "${ARK[0]}" run main.fd 2> /dev/null | tr -d '\r')
  if [ "$out" = 3 ]; then ok "p1: fid run main.fd uses the linked pact (3)"; else bad "p1: fid run main.fd printed [$out], want 3"; fi
fi
# a new commit: `ark update` moves p1's link only; p2 stays at the old commit
echo 'pub fn g() -> i64 { return 4; }' > "$W/gitlib/src/lib.fd"
(cd "$W/gitlib" && git commit -q -am two) || { echo "git failed in gitlib"; exit 1; }
expect_ok "p1: ark update" p1 p1-update.log update
has "p1: the new commit's source" "$W/p1/ark_modules/gitlib/src/lib.fd" "return 4;"
has "p2: still the old commit's source" "$W/p2/ark_modules/gitlib/src/lib.fd" "return 3;"
n=$(ls -d "$W/home/.ark/cache/checkouts/"gitlib-*/* 2> /dev/null | wc -l)
if [ "$n" -eq 2 ]; then ok "two shared checkouts, one per commit"; else bad "$n checkouts after the update, want 2"; fi
echo probe > "$W/p1/ark_modules/gitlib/.ark-probe"
absent "p2 no longer shares p1's working tree" p2/ark_modules/gitlib/.ark-probe
rm -f "$W/p1/ark_modules/gitlib/.ark-probe"
# ARK_LINK=0: a checkout of its own (which still borrows the cache's objects)
mkdir -p "$W/p4"
expect_ok "p4: ark init" p4 init.log init p4
expect_ok "p4: ark add --git" p4 add.log add gitlib --git "$W/gitlib"
export ARK_LINK=0
expect_ok "p4: ark install with ARK_LINK=0" p4 p4-install.log install
unset ARK_LINK
has "p4: the pact checked out" "$W/p4/ark_modules/gitlib/src/lib.fd" "return 4;"
echo probe > "$W/p1/ark_modules/gitlib/.ark-probe"
absent "p4 has a working tree of its own" p4/ark_modules/gitlib/.ark-probe
rm -f "$W/p1/ark_modules/gitlib/.ark-probe"
exists "p4: the checkout borrows the cache's objects" "p4/ark_modules/gitlib/.git/objects/info/alternates"

# --- CD4: diamond dependencies -----------------------------------------------
echo "-- CD4 one version per pact"
for d in a:'"1"' b:'"1.2"' b2:'"2"'; do
  n="${d%%:*}"; spec="${d#*:}"
  mkdir -p "$W/$n/src"
  printf '[package]\nname = "%s"\nversion = "0.1.0"\n\n[dependencies]\nc = %s\n' "$n" "$spec" > "$W/$n/ark.toml"
  echo "pub fn $n() -> i64 { return 1; }" > "$W/$n/src/lib.fd"
done
mkdir -p "$W/app2" "$W/app3"
expect_ok "app2: ark init" app2 init.log init app2
expect_ok "app2: ark add a" app2 add.log add a --path ../a
expect_ok "app2: ark add b" app2 add.log add b --path ../b
expect_ok "app2: ark install (a -> c 1, b -> c 1.2)" app2 app2-install.log install
has "one version chosen, the newest both accept" "$W/app2-install.log" "c 1.2.3: the newest version for app2 -> a -> c (1) and app2 -> b -> c (1.2)"
n=$(grep -c '^name = "c"' "$W/app2/ark.lock")
if [ "$n" -eq 1 ]; then ok "ark.lock has c once"; else bad "ark.lock has c $n times"; fi
has "ark.lock records its version" "$W/app2/ark.lock" 'version = "1.2.3"'
has "ark.lock records who needs it" "$W/app2/ark.lock" 'via = "a"'
has "that version installed" "$W/app2/ark_modules/c/src/lib.fd" '"1.2.3"'
expect_ok "app2: ark audit" app2 audit2.log audit
expect_ok "app2: ark install again (locked)" app2 app2-again.log install
has "the same version kept" "$W/app2/ark.lock" 'version = "1.2.3"'
expect_ok "app3: ark init" app3 init.log init app3
expect_ok "app3: ark add a" app3 add.log add a --path ../a
expect_ok "app3: ark add b2" app3 add.log add b2 --path ../b2
expect_fail "app3: ark install (a -> c 1, b2 -> c 2) fails" app3 app3-install.log install
has "the error names the first path" "$W/app3-install.log" "app3 -> a -> c  wants 1  (newest: 1.3.0)"
has "the error names the second path" "$W/app3-install.log" "app3 -> b2 -> c  wants 2  (newest: 2.0.0)"
has "the error says no version satisfies both" "$W/app3-install.log" "no version of 'c' satisfies all of them"
# two paths to one pact
mkdir -p "$W/e1" "$W/e2" "$W/d1" "$W/d2" "$W/app4"
echo 'pub fn e() -> i64 { return 1; }' > "$W/e1/e.fd"
echo 'pub fn e() -> i64 { return 2; }' > "$W/e2/e.fd"
printf '[package]\nname = "d1"\n\n[dependencies]\ne = { path = "../e1/e.fd" }\n' > "$W/d1/ark.toml"
printf '[package]\nname = "d2"\n\n[dependencies]\ne = { path = "../e2/e.fd" }\n' > "$W/d2/ark.toml"
echo 'pub fn d() -> i64 { return 1; }' > "$W/d1/lib.fd"
echo 'pub fn d() -> i64 { return 2; }' > "$W/d2/lib.fd"
expect_ok "app4: ark init" app4 init.log init app4
expect_ok "app4: ark add d1" app4 add.log add d1 --path ../d1
expect_ok "app4: ark add d2" app4 add.log add d2 --path ../d2
expect_fail "app4: two paths to e fail" app4 app4-install.log install
has "the error names d1's path" "$W/app4-install.log" "app4 -> d1 -> e  wants path ../e1/e.fd"
has "the error names d2's path" "$W/app4-install.log" "app4 -> d2 -> e  wants path ../e2/e.fd"

# --- CU5: the public API of a minor version ----------------------------------
echo "-- CU5 public API check on publish"
mkdir -p "$W/pkg/src"
lib() { # lib <version> <fn...>: pkg's ark.toml and src/lib.fd with those pub fns
  local v="$1"
  shift
  printf '[package]\nname = "pkg"\nversion = "%s"\n' "$v" > "$W/pkg/ark.toml"
  : > "$W/pkg/src/lib.fd"
  for f in "$@"; do
    case "$f" in
      greet2) printf 'pub fn greet(name: str, times: i64) -> str {\n    return name;\n}\n' >> "$W/pkg/src/lib.fd" ;;
      *) printf '/// %s\npub fn %s(name: str) -> str {\n    // not "pub fn hidden()" {\n    return name;\n}\n' "$f" "$f" >> "$W/pkg/src/lib.fd" ;;
    esac
  done
  printf 'fn private_helper() -> i64 {\n    return 0;\n}\n' >> "$W/pkg/src/lib.fd"
}
lib 1.0.0 greet shout
repo pkg
git -C "$W/pkg" tag v1.0.0
expect_ok "publish 1.0.0" pkg pub100.log publish --git "$W/pkg"
expect_ok "ark api" pkg api.log api
has "ark api lists a pub fn" "$W/api.log" "pub fn greet(name: str) -> str  (src/lib.fd)"
hasnt "not a private fn" "$W/api.log" "private_helper"
hasnt "not a fn named in a comment or string" "$W/api.log" "hidden"
lib 1.1.0 greet
commit_tag pkg v1.1.0
expect_fail "publish 1.1.0 removing pub fn shout is refused" pkg pub110a.log publish --git "$W/pkg"
has "it names the removed fn" "$W/pub110a.log" "removed: pub fn shout(name: str) -> str  (src/lib.fd)"
lib 1.1.0 greet2 shout
commit_tag pkg v1.1.0
expect_fail "publish 1.1.0 changing greet's parameters is refused" pkg pub110b.log publish --git "$W/pkg"
has "it names the changed fn" "$W/pub110b.log" "changed: pub fn greet(name: str) -> str  ->  pub fn greet(name: str, times: i64) -> str"
lib 1.1.0 greet shout whisper
commit_tag pkg v1.1.0
expect_ok "ark api --against v1.0.0 (additive)" pkg apidiff.log api --against v1.0.0
has "it shows the added fn" "$W/apidiff.log" "added: pub fn whisper(name: str) -> str"
expect_ok "publish 1.1.0 adding pub fn whisper" pkg pub110c.log publish --git "$W/pkg"
has "the check passed" "$W/pub110c.log" "API check against 1.0.0: 1 item(s) added, none removed or changed"
lib 2.0.0 shout whisper
commit_tag pkg v2.0.0
expect_ok "publish 2.0.0 removing greet (a new major version)" pkg pub200.log publish --git "$W/pkg"
has "a major version may change the API" "$W/pub200.log" "is a new major version after 1.1.0"

if [ "$fail" = 0 ]; then echo "ark-features: PASS"; else echo "ark-features: FAIL"; fi
exit "$fail"
