#!/bin/bash
# clean-bootstrap.sh — the bootstrap from a clean tree (roadmap VF13).
#
#   scripts/verify/clean-bootstrap.sh [--worktree] [out_dir]
#
# Exports the committed tree (`git archive HEAD`; with --worktree, the
# tracked and untracked files of the working tree that git does not ignore)
# into a new directory, so nothing built or left over in this checkout takes
# part. In that directory it then:
#   1. builds the frozen Rust compiler (cargo build --release, its own
#      target directory), unless STAGE0 names a compiler to start from;
#   2. runs scripts/bootstrap.sh from it: stage 1 from the Rust compiler (or
#      STAGE0), stages 2 and 3 from the previous stage; the fixpoint holds
#      when stages 2 and 3 emit the same C;
#   3. runs hello.fd with the stage-3 compiler.
# Before the long steps it checks that every module the tree's compiler/
# and stdlib/ import is in it (a file that was never committed), and it
# runs with FID_HOME unset and TEMP in <out_dir>/tmp (fid's caches live in
# TEMP), so nothing of this checkout takes part.
# The log of each step is kept in <out_dir> (default build/clean-bootstrap),
# and a summary of the run (date, host, commit, stage times, fixpoint,
# result) in <out_dir>/record.txt.
#
# Needs: git, cargo (unless STAGE0), a C compiler (CC, default `zig cc`).
# A stage needs several GB of memory (the generated C is ~50 MB).
set -u
cd "$(dirname "$0")/../.." || exit 1
MODE=head
if [ "${1:-}" = "--worktree" ]; then MODE=worktree; shift; fi
OUT="${1:-build/clean-bootstrap}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
TREE="$OUT/tree"
rm -rf "$TREE"; mkdir -p "$TREE"
export CC="${CC:-zig cc}"
# The record of the run (<out_dir>/record.txt): when, where, from what, and
# how it ended; the weekly job keeps it with the logs.
REC="$OUT/record.txt"
started=$(date +%s)
{
  echo "clean-tree bootstrap, $(date -u +%Y-%m-%dT%H:%M:%SZ), $(uname -srm)"
  echo "  tree: $MODE, commit $(git rev-parse --short HEAD 2>/dev/null)$( [ "$MODE" = worktree ] && ! git diff --quiet 2>/dev/null && echo ' + uncommitted changes')"
  echo "  CC: $CC; STAGE0: ${STAGE0:-cargo build of the Rust compiler}"
  echo "  load before: $(cut -d' ' -f1-3 /proc/loadavg 2>/dev/null || echo n/a)"
} > "$REC"
trap 'code=$?; { [ -f "$OUT/bootstrap.log" ] && grep -E "^\[stage|FIXPOINT" "$OUT/bootstrap.log" | sed "s/^/  /"; echo "  result: ${RESULT:-FAILED (exit $code)} after $(( $(date +%s) - started )) s"; } >> "$REC"' EXIT

if [ "$MODE" = head ]; then
  rev=$(git rev-parse HEAD)
  echo "[1/4] git archive $rev -> $TREE"
  git archive --format=tar HEAD | tar -x -C "$TREE" || exit 1
else
  echo "[1/4] working tree (tracked + untracked, not ignored) -> $TREE"
  # a tracked file deleted in the working tree is still listed: skip it
  git ls-files -z -co --exclude-standard | tar --null --ignore-failed-read -T - -cf - 2>/dev/null \
      | tar -x -C "$TREE" || exit 1
fi

# Nothing of this checkout or of earlier builds takes part: fid reads the
# stdlib in the current directory first (the new tree), then FID_HOME, and
# keeps its caches (imported modules, ASTs) in TEMP: a fresh folder here.
unset FID_HOME FID_LIB_PATH FID_IMPCACHE_DIR FID_ASTCACHE FID_FNCACHE FID_IRCACHE
rm -rf "$OUT/tmp"; mkdir -p "$OUT/tmp"
export TEMP="$OUT/tmp" TMP="$OUT/tmp" TMPDIR="$OUT/tmp" FIDES_NO_CACHE=1

# What a clean tree can lack: a module that a committed file imports but
# that was never committed (it builds here only because the file is in this
# checkout). Checked before the long steps: every `use compiler::m` of
# compiler/*.fd and every `use m` of compiler/ and stdlib/ must have its file.
missing=$( cd "$TREE" && cat compiler/*.fd stdlib/*.fd 2>/dev/null | sed 's://.*$::' \
  | grep -oE '(^|[;{ ])use [a-z_][a-z_0-9]*(::[a-z_][a-z_0-9]*)?;' \
  | sed -E 's/.*use //; s/;$//' | sort -u | while read -r m; do
      case "$m" in
        compiler::*) [ -f "compiler/${m#compiler::}.fd" ] || echo "compiler/${m#compiler::}.fd" ;;
        *::*) ;;
        compiler) ;;
        # a different module on each system (compiler.fd c_platform_module)
        tls) [ -f stdlib/tls_windows.fd ] && [ -f stdlib/tls_unix.fd ] || echo "stdlib/tls_windows.fd + tls_unix.fd" ;;
        *) [ -f "stdlib/$m.fd" ] || [ -f "compiler/$m.fd" ] || echo "stdlib/$m.fd" ;;
      esac
    done )
if [ -n "$missing" ]; then
  echo "FAILED: the clean tree lacks modules that its files import:"
  echo "$missing" | sed 's/^/  /'
  exit 1
fi
echo "      imports: every module the tree's compiler/ and stdlib/ use is in it"

case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; *) X="" ;; esac
if [ -z "${STAGE0:-}" ]; then
  echo "[2/4] cargo build --release (Rust bootstrap compiler)"
  ( cd "$TREE" && CARGO_TARGET_DIR="$OUT/cargo-target" cargo build --release --bin fidesc ) \
      > "$OUT/cargo.log" 2>&1 || { echo "cargo build failed (see $OUT/cargo.log)"; tail -20 "$OUT/cargo.log"; exit 1; }
  STAGE0="$OUT/cargo-target/release/fidesc$X"
else
  echo "[2/4] stage 0 given: $STAGE0"
  case "$STAGE0" in /*|?:*) ;; *) STAGE0="$(pwd)/$STAGE0" ;; esac
fi

echo "[3/4] scripts/bootstrap.sh (3 stages, fixpoint check)"
start=$(date +%s)
( cd "$TREE" && STAGE0="$STAGE0" ./scripts/bootstrap.sh "$OUT/stages" 3 ) > "$OUT/bootstrap.log" 2>&1
status=$?
echo "      $(( $(date +%s) - start )) s, exit $status"
tail -15 "$OUT/bootstrap.log" | sed 's/^/      /'
[ $status -eq 0 ] || { echo "FAILED: bootstrap (see $OUT/bootstrap.log)"; exit 1; }

echo "[4/4] stage 3 runs hello.fd"
S3=$(ls "$OUT"/stages/*3$X 2>/dev/null | head -1)
[ -n "$S3" ] || S3=$(ls -t "$OUT"/stages/* | grep -v '\.c$\|\.log$' | head -1)
[ -n "$S3" ] && [ -x "$S3" ] || { echo "FAILED: no stage-3 compiler in $OUT/stages"; exit 1; }
hello=$( cd "$TREE" && "$S3" run tests/bootstrap/hello.fd 2> "$OUT/hello.err" | tr -d '\r' )
# what stage 0 prints for it is the reference (as bootstrap.sh's RELEASE
# smoke test); `print` adds no newline: "Hello, Fides!50y is big"
want=$( cd "$TREE" && "$STAGE0" run tests/bootstrap/hello.fd 2>/dev/null | tr -d '\r' )
echo "      stage 3: '$hello'"
if [ -z "$hello" ] || [ "$hello" != "$want" ]; then
  echo "FAILED: stage 3 ($S3) printed '$hello' for hello.fd, stage 0 '$want' (see $OUT/hello.err)"
  exit 1
fi
RESULT="OK, stage 3 hello: '$hello'"
echo "clean-tree bootstrap: OK ($MODE; record in $REC)"
