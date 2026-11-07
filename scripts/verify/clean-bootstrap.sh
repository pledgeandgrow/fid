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
# The log of each step is kept in <out_dir> (default build/clean-bootstrap).
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

if [ "$MODE" = head ]; then
  rev=$(git rev-parse HEAD)
  echo "[1/4] git archive $rev -> $TREE"
  git archive --format=tar HEAD | tar -x -C "$TREE" || exit 1
else
  echo "[1/4] working tree (tracked + untracked, not ignored) -> $TREE"
  git ls-files -z -co --exclude-standard | tar --null -T - -cf - | tar -x -C "$TREE" || exit 1
fi

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
( cd "$TREE" && "$S3" run tests/bootstrap/hello.fd ) 2>/dev/null | tail -3
echo "clean-tree bootstrap: OK ($MODE)"
