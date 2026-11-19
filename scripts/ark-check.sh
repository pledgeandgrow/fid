#!/bin/bash
# ark-check.sh — release-roadmap N1: the package workflow of a new user,
# on a clean machine. In a fresh directory outside the repository, with a
# fresh HOME and FID_HOME unset (the compiler finds its library next to
# itself), it
#   1. creates a project:        fid ark init app
#   2. adds a library pact:      fid ark add textkit --path ../textkit
#                                (tests/projects/library/textkit, copied)
#   3. installs it:              fid ark install   (ark_modules/, ark.lock)
#   4. lists it:                 fid ark list
#   5. runs the app:             fid run main.fd           (`use textkit;`)
#   6. builds it and runs that:  fid ccompile main.fd -o app
# and compares both outputs with tests/projects/library/expected.txt.
# CI runs it on Linux and on Windows (.github/workflows/ci.yml).
#
# Usage:
#   scripts/ark-check.sh [fid]   (default: release/fid-dev, else release/fid.exe,
#                                 else release/fid)
#   CC=<cc>      the C compiler fid ccompile uses (e.g. "zig cc")
#   NO_BUILD=1   skip step 6 (no C compiler on this machine)
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev release/fid-dev.exe release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then
  echo "ark-check: no compiler (give its path)"; exit 1
fi
if [ ! -e "$FID" ] && [ ! -e "$FID.exe" ]; then
  echo "ark-check: $FID not found"; exit 1
fi
# an absolute path: the steps run in another directory
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-check.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$WORK/home" "$WORK/textkit" "$WORK/app"
cp -R "$ROOT/tests/projects/library/textkit/." "$WORK/textkit/"
cp "$ROOT/tests/projects/library/app/main.fd" "$WORK/app/main.fd"
tr -d '\r' < "$ROOT/tests/projects/library/expected.txt" > "$WORK/expected.txt"

fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
step() { # step <label> <log> <command...>: run in the app directory
  local label="$1" log="$2"
  shift 2
  if (cd "$WORK/app" && HOME="$WORK/home" USERPROFILE="$WORK/home" FID_HOME= "$@") > "$WORK/$log" 2>&1; then
    ok "$label"
  else
    bad "$label (exit $?):"
    sed 's/^/        /' "$WORK/$log" | head -20
  fi
}
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $(basename "$2")"; fi
}
same() { # same <label> <file>: the file (CRs dropped) is the expected output
  if [ "$(tr -d '\r' < "$2")" = "$(cat "$WORK/expected.txt")" ]; then
    ok "$1"
  else
    bad "$1: the output differs from tests/projects/library/expected.txt"
    tr -d '\r' < "$2" | diff "$WORK/expected.txt" - | head -20 | sed 's/^/        /'
  fi
}

echo "== ark-check ($FID)"
step "ark init" init.log "$FID" ark init app
if [ -f "$WORK/app/ark.toml" ]; then ok "ark.toml written"; else bad "no ark.toml"; fi
step "ark add --path" add.log "$FID" ark add textkit --path ../textkit
has "the manifest names the pact" "$WORK/app/ark.toml" "textkit"
step "ark install" install.log "$FID" ark install
has "installed as a path pact" "$WORK/install.log" "OK (path)"
if [ -f "$WORK/app/ark_modules/textkit/src/lib.fd" ]; then
  ok "ark_modules/textkit/src/lib.fd"
else
  bad "the pact is not in ark_modules/textkit/"
fi
if [ -f "$WORK/app/ark.lock" ]; then
  has "ark.lock records its hash" "$WORK/app/ark.lock" "resolved = \"fnv:"
else
  bad "no ark.lock"
fi
step "ark list" list.log "$FID" ark list
has "the list shows the pact" "$WORK/list.log" "textkit"

(cd "$WORK/app" && HOME="$WORK/home" USERPROFILE="$WORK/home" FID_HOME= "$FID" run main.fd) > "$WORK/run.out" 2> "$WORK/run.err"
if [ $? -eq 0 ]; then same "fid run main.fd" "$WORK/run.out"; else bad "fid run main.fd failed:"; head -20 "$WORK/run.err" | sed 's/^/        /'; fi

if [ -n "${NO_BUILD:-}" ]; then
  echo "  skip  fid ccompile (NO_BUILD)"
else
  step "fid ccompile main.fd" build.log "$FID" ccompile main.fd -o "app$EXE"
  if [ -f "$WORK/app/app$EXE" ]; then
    (cd "$WORK/app" && "./app$EXE") > "$WORK/app.out" 2> "$WORK/app.err"
    same "the built program" "$WORK/app.out"
  else
    bad "fid ccompile wrote no app$EXE"
  fi
fi

if [ "$fail" = 0 ]; then echo "ark-check: PASS"; else echo "ark-check: FAIL"; fi
exit "$fail"
