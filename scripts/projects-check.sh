#!/bin/bash
# projects-check.sh — release-roadmap N11: five example projects built and
# run in CI, one of each kind (tests/projects/README.md):
#   cli      a command-line tool          fid ccompile; run with two files
#   library  a library pact and its app   scripts/ark-check.sh (ark add,
#                                         install, fid run, fid ccompile)
#   service  a line-protocol service      fid ccompile; requests on stdin
#   game     a game loop (Life)           fid run and fid ccompile
#   cbind    a C binding (libc, libm)     fid ccompile
# Each output is compared with tests/projects/<name>/expected.txt (CRs
# dropped), the exit status with the one the project documents, and each
# project's sources must lint clean ("No lint warnings.").
#
# Usage:
#   scripts/projects-check.sh [fid]   (default: release/fid-dev, else
#                                      release/fid.exe, else release/fid)
#   CC=<cc>   the C compiler fid ccompile uses (e.g. "zig cc")
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev release/fid-dev.exe release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "projects-check: no compiler (give its path)"; exit 1; fi
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
P="$ROOT/tests/projects"
WORK="$ROOT/build/projects-check"
rm -rf "$WORK"
mkdir -p "$WORK"
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }

# same <label> <expected file> <output file>
same() {
  if [ "$(tr -d '\r' < "$3")" = "$(tr -d '\r' < "$2")" ]; then
    ok "$1"
  else
    bad "$1: the output differs from ${2#$ROOT/}"
    diff <(tr -d '\r' < "$2") <(tr -d '\r' < "$3") | head -20 | sed 's/^/        /'
  fi
}

# lint_clean <file>: fid lint from the repository root (where stdlib/ is)
lint_clean() {
  local out
  out="$("$FID" lint "$1" 2>&1)"
  if echo "$out" | grep -q "No lint warnings"; then
    ok "lint ${1#$ROOT/}"
  else
    bad "lint ${1#$ROOT/}:"
    echo "$out" | head -10 | sed 's/^/        /'
  fi
}

# build <name>: fid ccompile tests/projects/<name>/main.fd -> $WORK/<name>/<name>
build() {
  mkdir -p "$WORK/$1"
  if "$FID" ccompile "$P/$1/main.fd" -o "$WORK/$1/$1$EXE" > "$WORK/$1/build.log" 2>&1 \
     && [ -f "$WORK/$1/$1$EXE" ]; then
    ok "$1: fid ccompile"
    return 0
  fi
  bad "$1: fid ccompile failed:"
  head -20 "$WORK/$1/build.log" | sed 's/^/        /'
  return 1
}

# status <label> <want> <got>
status() {
  if [ "$3" = "$2" ]; then ok "$1: exit status $3"; else bad "$1: exit status $3, expected $2"; fi
}

echo "== cli"
lint_clean "$P/cli/main.fd"
if build cli; then
  # the inputs are written here, so their bytes do not depend on how git
  # checks out line endings
  printf 'the quick brown fox\njumps over\nthe lazy dog\n' > "$WORK/cli/a.txt"
  printf 'hello fides\n\n  spaced   words here  \n' > "$WORK/cli/b.txt"
  (cd "$WORK/cli" && "./cli$EXE" a.txt b.txt) > "$WORK/cli/out.txt" 2> "$WORK/cli/err.txt"
  status "cli" 0 $?
  same "cli: wc a.txt b.txt" "$P/cli/expected.txt" "$WORK/cli/out.txt"
  (cd "$WORK/cli" && "./cli$EXE" missing.txt) > /dev/null 2> "$WORK/cli/err2.txt"
  status "cli: an unreadable file" 1 $?
  (cd "$WORK/cli" && "./cli$EXE") > /dev/null 2>&1
  status "cli: no arguments" 2 $?
fi

echo "== library (through ark)"
lint_clean "$P/library/textkit/src/lib.fd"
if bash "$ROOT/scripts/ark-check.sh" "$FID" > "$WORK/ark-check.log" 2>&1; then
  ok "library: scripts/ark-check.sh"
else
  bad "library: scripts/ark-check.sh failed:"
  grep -E "FAIL|^        " "$WORK/ark-check.log" | head -20 | sed 's/^/    /'
fi

echo "== service"
lint_clean "$P/service/main.fd"
if build service; then
  "$WORK/service/service$EXE" < "$P/service/requests.txt" > "$WORK/service/out.txt" 2> "$WORK/service/err.txt"
  # two of the requests are errors, and the service stops at QUIT
  status "service" 1 $?
  same "service: requests.txt" "$P/service/expected.txt" "$WORK/service/out.txt"
fi

echo "== game"
lint_clean "$P/game/main.fd"
"$FID" run "$P/game/main.fd" > "$WORK/game-run.txt" 2> "$WORK/game-run.err"
status "game: fid run" 0 $?
same "game: fid run" "$P/game/expected.txt" "$WORK/game-run.txt"
if build game; then
  "$WORK/game/game$EXE" > "$WORK/game/out.txt" 2> "$WORK/game/err.txt"
  status "game" 0 $?
  same "game: the built program" "$P/game/expected.txt" "$WORK/game/out.txt"
fi

echo "== cbind"
lint_clean "$P/cbind/main.fd"
if build cbind; then
  "$WORK/cbind/cbind$EXE" > "$WORK/cbind/out.txt" 2> "$WORK/cbind/err.txt"
  status "cbind" 0 $?
  same "cbind: the built program" "$P/cbind/expected.txt" "$WORK/cbind/out.txt"
fi

if [ "$fail" = 0 ]; then echo "projects-check: PASS"; else echo "projects-check: FAIL"; fi
exit "$fail"
