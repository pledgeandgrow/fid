#!/bin/bash
# cd8-no-install-code.sh -- CD8 (complaint roadmap): installing a pact runs
# none of its code. npm's postinstall, Cargo's build.rs and pip's setup.py
# run what a package ships at install time; ark does not: `ark install`
# copies (path) or checks out (git) the pact's files and nothing else. Each
# pact here ships every hook another tool would run -- an [scripts] table
# with postinstall/install/build entries in ark.toml, a build.fd and
# build.rs with a main, install.sh, setup.py, and git hooks in .githooks
# with core.hooksPath set in its own repository -- and each would leave a
# marker file. After the installs no marker exists.
# (The other half of CD8, comptime with no I/O, is checked by
# tests/bootstrap/test_comptime_*_fail.fd.)
#
# Usage: tests/ark/cd8-no-install-code.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, release/fid ark)
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
  if [ ${#ARK[@]} -eq 0 ]; then echo "cd8-no-install-code: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/cd8-ark.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
W="$(cygpath -m "$WORK" 2>/dev/null || echo "$WORK")"
mkdir -p "$W/home" "$W/markers"
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
exists() { if [ -e "$W/$2" ]; then ok "$1"; else bad "$1: no $2"; fi; }

# a pact <dir> <name> that ships every kind of install-time hook
hooked_pact() {
  local d="$W/$1" n="$2" m="$W/markers"
  mkdir -p "$d/src" "$d/.githooks"
  cat > "$d/ark.toml" <<TOML
[package]
name = "$n"
version = "1.0.0"

[scripts]
preinstall = "echo pre > $m/$n-preinstall"
install = "echo install > $m/$n-install"
postinstall = "echo post > $m/$n-postinstall"
build = "echo build > $m/$n-build"
TOML
  echo "pub fn ${n}_value() -> i64 { return 1; }" > "$d/src/lib.fd"
  printf 'fn main() -> i64 {\n    p_file_write("%s/%s-build-fd", "ran");\n    return 0;\n}\n' "$m" "$n" > "$d/build.fd"
  printf 'fn main() { std::fs::write("%s/%s-build-rs", "ran").unwrap(); }\n' "$m" "$n" > "$d/build.rs"
  printf '#!/bin/sh\necho ran > "%s/%s-install-sh"\n' "$m" "$n" > "$d/install.sh"
  printf 'open("%s/%s-setup-py", "w").write("ran")\n' "$m" "$n" > "$d/setup.py"
  for h in post-checkout post-merge post-clone post-rewrite; do
    printf '#!/bin/sh\necho ran > "%s/%s-git-%s"\n' "$m" "$n" "$h" > "$d/.githooks/$h"
  done
  chmod +x "$d/install.sh" "$d/.githooks/"*
}

echo "== cd8-no-install-code (${ARK[*]})"
hooked_pact plib plib
hooked_pact glib glib
(cd "$W/glib" && git init -q && git config core.hooksPath .githooks && git add -A && git commit -q -m init) || { echo "git failed"; exit 1; }
mkdir -p "$W/app"
expect_ok "ark init" app init.log init app
expect_ok "ark add a path pact" app addp.log add plib --path ../plib
expect_ok "ark add a git pact" app addg.log add glib --git "$W/glib"
expect_ok "ark install" app install.log install
exists "the path pact is installed" app/ark_modules/plib/src/lib.fd
exists "the git pact is checked out" app/ark_modules/glib/src/lib.fd
rm -rf "$W/app/ark_modules"
expect_ok "ark install again (from ark.lock)" app install2.log install
n=$(ls "$W/markers" | wc -l)
if [ "$n" -eq 0 ]; then
  ok "no install-time code ran (no marker file)"
else
  bad "install-time code ran: $(ls "$W/markers" | tr '\n' ' ')"
fi

if [ "$fail" = 0 ]; then echo "cd8-no-install-code: PASS"; else echo "cd8-no-install-code: FAIL"; fi
exit "$fail"
