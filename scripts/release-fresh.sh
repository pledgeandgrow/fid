#!/bin/bash
# release-fresh.sh — release-roadmap A10: is release/fid(.exe) the release
# bootstrap's build of the source as it is now, or a stale one?
#
#   scripts/release-fresh.sh --write [exe]   record a stamp beside the binary
#                                            (scripts/bootstrap.sh does it
#                                            with RELEASE=1)
#   scripts/release-fresh.sh [exe]           check it: exit 0 when the stamp
#                                            matches the binary and the
#                                            source, 1 when stale or unstamped
#
# The stamp, <exe>.stamp, holds the binary's SHA-256, a hash of the source it
# was built from (compiler/, stdlib/ and scripts/bootstrap.sh, line endings
# dropped so a Windows and a Linux checkout agree) and the commit. A release
# job (.github/workflows/release.yml) and the installer build should refuse a
# binary that fails the check: rebuild it with RELEASE=1 scripts/bootstrap.sh.
set -u
cd "$(dirname "$0")/.." || exit 1
MODE=check
if [ "${1:-}" = "--write" ]; then MODE=write; shift; fi
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
EXE="${1:-release/fid$X}"
STAMP="$EXE.stamp"

sha() { if command -v sha256sum > /dev/null; then sha256sum | cut -d' ' -f1; else shasum -a 256 | cut -d' ' -f1; fi; }
source_hash() {
  # every .fd of the compiler and the library (runtimes included), in a fixed
  # order, each with its name, then the bootstrap script. Not
  # compiler/build_info.fd: bootstrap.sh stamps the release values into it,
  # writes this stamp, and its EXIT trap resets the file, so with it in the
  # hash every release build read as stale (BG284; build-info.sh's own
  # source hash leaves it out the same way)
  local files
  files=$(find compiler stdlib -name '*.fd' | LC_ALL=C sort | grep -v '^compiler/build_info\.fd$')
  { printf '%s\n' "$files"; printf '%s\n' "$files" | xargs cat; cat scripts/bootstrap.sh; } | tr -d '\r' | sha
}

if [ ! -f "$EXE" ]; then echo "release-fresh: no $EXE"; exit 1; fi
bin=$(sha < "$EXE")
src=$(source_hash)
commit="$(git rev-parse --short HEAD 2> /dev/null || echo unknown)$(git diff --quiet HEAD -- compiler stdlib ':!compiler/build_info.fd' 2> /dev/null || echo '+changes')"

if [ "$MODE" = write ]; then
  {
    echo "binary $bin"
    echo "source $src"
    echo "commit $commit"
    echo "built $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  } > "$STAMP"
  echo "release-fresh: wrote $STAMP ($commit)"
  exit 0
fi

if [ ! -f "$STAMP" ]; then
  echo "release-fresh: $EXE has no stamp: not built by RELEASE=1 scripts/bootstrap.sh (rebuild it)"
  exit 1
fi
want_bin=$(sed -n 's/^binary //p' "$STAMP")
want_src=$(sed -n 's/^source //p' "$STAMP")
if [ "$bin" != "$want_bin" ]; then
  echo "release-fresh: $EXE is not the binary its stamp records (replaced by another build)"
  exit 1
fi
if [ "$src" != "$want_src" ]; then
  echo "release-fresh: $EXE is stale: compiler/ or stdlib/ changed since it was built ($(sed -n 's/^commit //p' "$STAMP"), $(sed -n 's/^built //p' "$STAMP"))"
  exit 1
fi
echo "release-fresh: $EXE is the release build of this source ($(sed -n 's/^commit //p' "$STAMP"))"
exit 0
