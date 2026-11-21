#!/bin/bash
# ark-registry.sh -- checks of ark's sharing, registry and trust features
# (docs/reference/ark.md):
#   L5F  shared dependencies: two workspace members that need the same
#        pact get one version of it, resolved together, installed once in
#        the root's ark_modules/ and recorded once in the root's ark.lock;
#        `ark install` in a member runs the workspace's; `ark audit` in a
#        member reads the root's lock and copy
#   TL9  a git pact from a local repository that needs another git pact:
#        both installed, both in ark.lock (the second `via` the first),
#        `ark audit` passes
#   N6   `ark yank`: a yanked version is not chosen for a new lock, a lock
#        that records it keeps it (with a warning), `ark search` skips it,
#        `ark yank --undo` brings it back
#   CD1  [capabilities]: a pact that uses the network without a grant
#        fails `ark install`, `ark build` and `ark audit`, naming the call;
#        granting it passes; `ark update` prints the capability diff
# Everything is local (git repositories in a temporary directory).
#
# Usage: tests/ark/ark-registry.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, ...)
#   e.g.     tests/ark/ark-registry.sh release/fid.exe run tools/ark.fd --
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
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-registry: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac
# `fid run tools/ark.fd --`: the file from any directory
for ((k = 1; k < ${#ARK[@]}; k++)); do
  case "${ARK[$k]}" in *.fd) [ -f "$ROOT/${ARK[$k]}" ] && ARK[$k]="$ROOT/${ARK[$k]}" ;; esac
done
export FID_HOME="${FID_HOME:-$ROOT}"
unset ARK_FID

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-registry.$$")"
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
  if run "$@"; then ok "$label"; else bad "$label (exit $?):"; sed 's/^/        /' "$W/$2" | grep -v '^\s*\[' | head -20; fi
}
expect_fail() {
  local label="$1"
  shift
  if run "$@"; then bad "$label: succeeded"; sed 's/^/        /' "$W/$2" | grep -v '^\s*\[' | head -20; else ok "$label"; fi
}
has() {
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $(basename "$2")"; sed 's/^/        /' "$2" | grep -v '^\s*\[' | head -30; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $(basename "$2")"; else ok "$1"; fi
}
count() { # count <label> <file> <text> <n>
  local n
  n=$(grep -cF -- "$3" "$2" 2>/dev/null || true)
  if [ "$n" = "$4" ]; then ok "$1"; else bad "$1: '$3' $n times in $(basename "$2"), wanted $4"; fi
}
exists() { if [ -e "$W/$2" ]; then ok "$1"; else bad "$1: no $2"; fi; }
absent() { if [ -e "$W/$2" ]; then bad "$1: $2 exists"; else ok "$1"; fi; }
sha() { git -C "$W/$1" rev-parse "$2^{commit}"; }
mkrepo() { # mkrepo <dir> <tag>: commit everything in <dir>, tag it
  (cd "$W/$1" && { [ -d .git ] || git init -q; } && git add -A && git commit -q -m "$2" && git tag -f "$2" > /dev/null) || { echo "git failed in $1"; exit 1; }
}

echo "== ark-registry (${ARK[*]})"

# --- L5F: shared dependencies across workspace members -----------------------
echo "-- L5F shared dependencies"
mkdir -p "$W/ws/app" "$W/ws/libs/one" "$W/ws/libs/two" "$W/ws/shared"
printf '[workspace]\nmembers = ["app", "libs/*"]\n' > "$W/ws/ark.toml"
echo 'pub fn shared() -> i64 { return 5; }' > "$W/ws/shared/shared.fd"
printf '[package]\nname = "app"\n\n[dependencies]\nshared = { path = "../shared/shared.fd" }\n' > "$W/ws/app/ark.toml"
printf '[package]\nname = "one"\n\n[dependencies]\nshared = { path = "../../shared/shared.fd" }\n' > "$W/ws/libs/one/ark.toml"
printf '[package]\nname = "two"\n' > "$W/ws/libs/two/ark.toml"
expect_ok "ark install in the workspace root" ws ws-install.log install
has "one resolution for the workspace" "$W/ws-install.log" 'one ark.lock, one copy of each pact'
has "the pact shared by both members" "$W/ws-install.log" 'shared: shared by app, one'
count "installed once" "$W/ws-install.log" 'Installing shared' 1
exists "the one copy, in the root" ws/ark_modules/shared.fd
exists "the root's ark.lock" ws/ark.lock
count "one lock entry for it" "$W/ws/ark.lock" 'name = "shared"' 1
absent "no copy in app/" ws/app/ark_modules
absent "no ark.lock in app/" ws/app/ark.lock
absent "no copy in libs/one/" ws/libs/one/ark_modules
expect_ok "ark audit in the root (each member)" ws ws-audit.log audit
has "app audited against the root's lock" "$W/ws-audit.log" 'locked  fnv:'
expect_ok "ark install in a member" ws/app ws-member.log install
has "it runs the workspace's" "$W/ws-member.log" 'This is a member of the workspace at ..'
count "still one lock entry" "$W/ws/ark.lock" 'name = "shared"' 1
absent "still no ark.lock in app/" ws/app/ark.lock
expect_ok "ark audit in a member" ws/libs/one ws-audit1.log audit
has "it reads the root's copy" "$W/ws-audit1.log" 'every pact is installed as locked'
# two members that cannot share one copy: the conflict names both
mkdir -p "$W/ws/other"
echo 'pub fn shared() -> i64 { return 6; }' > "$W/ws/other/shared.fd"
printf '[package]\nname = "two"\n\n[dependencies]\nshared = { path = "../../other/shared.fd" }\n' > "$W/ws/libs/two/ark.toml"
expect_fail "members wanting two copies of a pact" ws ws-conflict.log install
has "the conflict names both members" "$W/ws-conflict.log" "conflicting requirements for 'shared'"
has "app's requirement" "$W/ws-conflict.log" 'app -> shared'
has "two's requirement" "$W/ws-conflict.log" 'two -> shared'

# --- TL9: a git pact with its own git pact -----------------------------------
echo "-- TL9 git pacts from local repositories"
mkdir -p "$W/hlib/src" "$W/glib/src" "$W/gapp"
printf '[package]\nname = "hlib"\nversion = "0.1.0"\n' > "$W/hlib/ark.toml"
printf 'pub fn h() -> i64 {\n    return 2;\n}\n' > "$W/hlib/src/lib.fd"
mkrepo hlib v0.1.0
printf '[package]\nname = "glib"\nversion = "0.1.0"\n\n[dependencies]\nhlib = { git = "%s" }\n' "$W/hlib" > "$W/glib/ark.toml"
printf 'use hlib;\n\npub fn g() -> i64 {\n    return h() + 1;\n}\n' > "$W/glib/src/lib.fd"
mkrepo glib v0.1.0
printf '[package]\nname = "gapp"\n\n[dependencies]\nglib = { git = "%s" }\n' "$W/glib" > "$W/gapp/ark.toml"
expect_ok "ark install (a git pact that needs a git pact)" gapp g-install.log install
exists "glib installed" gapp/ark_modules/glib/src/lib.fd
exists "hlib installed for it" gapp/ark_modules/hlib/src/lib.fd
has "glib in ark.lock" "$W/gapp/ark.lock" 'name = "glib"'
has "hlib in ark.lock" "$W/gapp/ark.lock" 'name = "hlib"'
has "hlib locked via glib" "$W/gapp/ark.lock" 'via = "glib"'
has "at hlib's commit" "$W/gapp/ark.lock" "resolved = \"$(sha hlib HEAD)\""
expect_ok "ark audit passes" gapp g-audit.log audit
has "both checked" "$W/g-audit.log" 'every pact is installed as locked'

# --- N6: yanking ------------------------------------------------------------
echo "-- N6 ark yank"
mkdir -p "$W/clib/src"
mkver() {
  printf '[package]\nname = "c"\nversion = "%s"\n' "$1" > "$W/clib/ark.toml"
  printf 'pub fn c_version() -> str {\n    return "%s";\n}\n' "$1" > "$W/clib/src/lib.fd"
  mkrepo clib "v$1"
}
git init -q --bare "$W/index.git"
git clone -q "$W/index.git" "$W/index-seed" 2> /dev/null
for v in 1.0.0 1.1.0; do mkver "$v"; echo "$v $W/clib v$v $(sha clib "v$v")" >> "$W/index-seed/c"; done
(cd "$W/index-seed" && git add -A && git commit -q -m c && git push -q origin HEAD 2> /dev/null) || { echo "cannot seed the index"; exit 1; }
mkdir -p "$W/old" "$W/new"
printf '[package]\nname = "old"\n\n[dependencies]\nc = "^1"\n' > "$W/old/ark.toml"
printf '[package]\nname = "new"\n\n[dependencies]\nc = "^1"\n' > "$W/new/ark.toml"
expect_ok "a project locks c 1.1.0" old y-old.log install
has "1.1.0 locked" "$W/old/ark.lock" 'version = "1.1.0"'
expect_ok "ark yank 1.1.0" clib y-yank.log yank 1.1.0
has "yanked" "$W/y-yank.log" 'Yanked c 1.1.0.'
git -C "$W/index-seed" pull -q 2> /dev/null
has "the index line marked" "$W/index-seed/c" "1.1.0 $W/clib v1.1.0 $(sha clib v1.1.0) yanked"
expect_ok "yanking again changes nothing" clib y-yank2.log yank 1.1.0
has "already yanked" "$W/y-yank2.log" 'already yanked'
expect_ok "a new project's ark install" new y-new.log install
has "it does not choose the yanked version" "$W/new/ark.lock" 'version = "1.0.0"'
rm -rf "$W/old/ark_modules"
expect_ok "the old project's ark install" old y-old2.log install
has "its lock keeps 1.1.0" "$W/old/ark.lock" 'version = "1.1.0"'
has "with a warning" "$W/y-old2.log" 'has been yanked from the index'
expect_ok "ark update in the old project" old y-upd.log update
has "update moves off the yanked version" "$W/old/ark.lock" 'version = "1.0.0"'
expect_ok "ark search" new y-search.log search c
has "search shows the newest version not yanked" "$W/y-search.log" 'c 1.0.0'
expect_fail "ark yank of a version not in the index" clib y-none.log yank 9.9.9
expect_ok "ark yank --undo" clib y-undo.log yank 1.1.0 --undo
has "unyanked" "$W/y-undo.log" 'Unyanked c 1.1.0.'
expect_ok "ark update after the undo" new y-upd2.log update
has "1.1.0 chosen again" "$W/new/ark.lock" 'version = "1.1.0"'

# --- CD1: per-dependency capabilities ----------------------------------------
echo "-- CD1 [capabilities]"
mkdir -p "$W/netpact/src" "$W/pure" "$W/capp/src"
printf '[package]\nname = "netpact"\nversion = "0.1.0"\n' > "$W/netpact/ark.toml"
printf 'pub fn fetch() -> i64 {\n    return 0;\n}\n' > "$W/netpact/src/lib.fd"
mkrepo netpact v0.1.0
echo 'pub fn add(a: i64, b: i64) -> i64 { return a + b; }' > "$W/pure/pure.fd"
printf 'fn main() -> i64 {\n    return 0;\n}\n' > "$W/capp/src/main.fd"
printf '[package]\nname = "capp"\n\n[dependencies]\nnetpact = { git = "%s" }\npure = { path = "../pure/pure.fd" }\n\n[capabilities]\nnetpact = []\npure = []\n' "$W/netpact" > "$W/capp/ark.toml"
expect_ok "ark install: both pacts use nothing" capp c-install.log install
expect_ok "ark audit" capp c-audit.log audit
# the pact starts using the network
printf 'pub fn fetch() -> i64 {\n    return p_tcp_connect("example.invalid", 80);\n}\n' > "$W/netpact/src/lib.fd"
mkrepo netpact v0.2.0
expect_fail "ark update to a version using the network without a grant" capp c-update.log update
has "the capability diff" "$W/c-update.log" 'netpact: +net'
has "the error names the pact" "$W/c-update.log" "pact 'netpact' uses capabilities ark.toml does not grant it"
has "and the call" "$W/c-update.log" 'net  ark_modules/netpact/src/lib.fd (p_tcp_)'
has "and the grant that would allow it" "$W/c-update.log" 'netpact = ["net"]'
expect_fail "ark build refuses" capp c-build.log build --dry-run
has "build: the same error" "$W/c-build.log" "pact 'netpact' uses capabilities"
expect_fail "ark audit fails" capp c-audit2.log audit
has "audit shows what it uses" "$W/c-audit2.log" 'uses: net'
expect_fail "ark install fails" capp c-install2.log install
sed -i 's/^netpact = \[\]$/netpact = ["net"]/' "$W/capp/ark.toml"
expect_ok "granted: ark install" capp c-install3.log install
expect_ok "granted: ark audit" capp c-audit3.log audit
expect_ok "granted: ark build --dry-run" capp c-build2.log build --dry-run
# no [capabilities] section: nothing is checked
sed -i '/^\[capabilities\]$/,$d' "$W/capp/ark.toml"
printf 'pub fn fetch() -> i64 {\n    return p_system("true");\n}\n' > "$W/netpact/src/lib.fd"
mkrepo netpact v0.3.0
expect_ok "without [capabilities]: ark update" capp c-update2.log update
has "the diff is still shown" "$W/c-update2.log" 'netpact: +process -net'

if [ $fail -ne 0 ]; then echo "ark-registry: FAILED"; exit 1; fi
echo "ark-registry: all checks passed"
