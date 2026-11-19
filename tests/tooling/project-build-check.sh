#!/bin/bash
# L5B (docs/roadmap/roadmap.md, build system): `fid build` with no file in
# a directory with an ark.toml builds the project: compiler/main.fd
# project_build installs the dependencies ark.toml declares when they are
# not there yet (`ark install`: here a path dependency), then runs `ark
# build` with this same compiler, which builds every [[bin]] and [[lib]].
# The fixture (tests/projects/l5b_project, its path dependency
# tests/projects/l5b_dep) is copied to a temporary directory first, so the
# repository gets no ark_modules/ or build/.
#   FID=release/fid.exe bash tests/tooling/project-build-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
ROOT=$(pwd)
case "$FID" in
    /*|[A-Za-z]:*) ;;
    */*) FID="$ROOT/$FID" ;;
esac
# the copied project is outside the repository: the stdlib is the repo's
export FID_HOME="${FID_HOME:-$ROOT}"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cp -R tests/projects/l5b_project tests/projects/l5b_dep "$TMP/"
OUT="$TMP/out.txt"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
has() {
    if grep -qF -- "$2" "$OUT"; then ok "$1"; else bad "$1: no line with: $2"; fi
}

# 1. the first build installs the path dependency and builds both targets
( cd "$TMP/l5b_project" && "$FID" build ) > "$OUT" 2>&1
code=$?
if [ $code -eq 0 ]; then ok "fid build in the project succeeds"; else bad "fid build in the project: exit $code"; fi
has "the missing dependency is installed first" "Installing the dependencies of ark.toml"
if [ -f "$TMP/l5b_project/ark_modules/l5b_greet.fd" ]; then ok "the path dependency is in ark_modules/"; else bad "no ark_modules/l5b_greet.fd"; fi
has "the bin target is built" "Building bin l5b_app"
has "the lib target is checked" "Building lib l5b_lib"
has "both targets are built" "Built 2 target(s)"
app="$TMP/l5b_project/build/l5b_app"
[ -f "$app.exe" ] && app="$app.exe"
if [ -f "$app" ]; then
    got=$("$app" 2>&1 | tr -d '\r')
    if [ "$got" = "hello, project" ]; then ok "the built program runs"; else bad "the built program printed: $got"; fi
else
    bad "no executable build/l5b_app"
fi

# 2. a second build does not install again
( cd "$TMP/l5b_project" && "$FID" build ) > "$OUT" 2>&1
code=$?
if [ $code -eq 0 ]; then ok "a second fid build succeeds"; else bad "second fid build: exit $code"; fi
if grep -qF "Installing the dependencies" "$OUT"; then bad "the second build installed again"; else ok "installed dependencies are not installed again"; fi

# 3. --dry-run: the targets are built by this compiler, not a fid on PATH
( cd "$TMP/l5b_project" && "$FID" build --dry-run ) > "$OUT" 2>&1
fid_base=$(basename "$FID")
if grep -F " build src/main.fd -o build/l5b_app" "$OUT" | grep -qF "$fid_base"; then ok "the bin is built by this fid"; else bad "--dry-run does not show the bin built by $fid_base"; fi
if grep -F " check src/lib.fd" "$OUT" | grep -qF "$fid_base"; then ok "the lib is checked by this fid"; else bad "--dry-run does not show the lib checked by $fid_base"; fi

# 4. a project whose dependency cannot be installed fails before building
mkdir -p "$TMP/broken/src"
printf '[package]\nname = "broken"\nversion = "0.1.0"\n\n[dependencies]\nnothere = { path = "../no_such_dep.fd" }\n' > "$TMP/broken/ark.toml"
printf 'fn main() {\n    println("x");\n}\n' > "$TMP/broken/src/main.fd"
( cd "$TMP/broken" && "$FID" build ) > "$OUT" 2>&1
code=$?
if [ $code -ne 0 ]; then ok "a dependency that cannot be installed fails the build"; else bad "the broken project built"; fi

if [ $fail -ne 0 ]; then echo "--- last output ---"; cat "$OUT"; fi
exit $fail
