#!/bin/bash
# r1-ccompile-repro-check.sh — the test of miscompile-log entry R1 (VF12):
# `fid ccompile` of the same source gives the same executable bytes whatever
# the directory it is built from and the time it is built at.
#
# R1's fix is compiler/main.fd `repro_win_flags`/`repro_env_prefix` (a fixed
# SOURCE_DATE_EPOCH for the C compiler, no debug directory / PDB path).
# FIDES_NO_REPRO=1 turns the fix off, so the same compiler also shows that
# this check fails without it.
#
# Usage: FID=release/fid.exe tests/tooling/r1-ccompile-repro-check.sh
#   CC   the C compiler fid uses (default `zig cc`)
#   OUT  work directory (default build/r1-repro-check)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. hello.fd built with ccompile in two directories of different path
#     lengths, two seconds apart, with different TEMP: the same bytes
#  2. the same with FIDES_NO_REPRO=1 (the fix off): the bytes differ, so
#     check 1 would fail without the fix (SKIP when the compiler has no
#     switch: then the bytes agree there too and nothing is shown)
# A build that fails is a FAIL (no C compiler: say so, exit 2).
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}" ;; *) FID="${FID:-release/fid}" ;; esac
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
export FIDES_NO_CACHE=1
export FID_HOME="${FID_HOME:-$ROOT}"
OUT="${OUT:-build/r1-repro-check}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
SRC=tests/bootstrap/hello.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

if ! "$FID" version > /dev/null 2>&1; then
  echo "r1 check: $FID does not run" >&2
  exit 2
fi
case "$CC" in *zig*) command -v zig > /dev/null 2>&1 || { echo "r1 check: no zig for CC=$CC" >&2; exit 2; } ;; esac

# build <dir> <tmp> [env...]: copy hello.fd into <dir>, ccompile it there by
# its file name (as a user does) with TEMP/TMP=<tmp>; prints the exe's path
build() {
  local d="$1" t="$2"; shift 2
  mkdir -p "$d" "$t"
  cp "$ROOT/$SRC" "$d/hello.fd"
  rm -f "$d/hello.exe" "$d/hello.pdb"
  ( cd "$d" && env TEMP="$t" TMP="$t" TMPDIR="$t" "$@" "$FID" ccompile hello.fd -o hello.exe ) > "$d/build.log" 2>&1
  [ -s "$d/hello.exe" ] || return 1
  echo "$d/hello.exe"
}

# pair <name> [env...]: two builds; sets SAME=1 when the bytes agree
pair() {
  local name="$1"; shift
  local a b
  a=$(build "$OUT/$name/a" "$OUT/$name/tmp-a" "$@") || { fail "$name: build a failed ($(tail -n 3 "$OUT/$name/a/build.log" | tr '\n' ' '))"; return 1; }
  sleep 2   # the PE time stamp has a resolution of one second
  b=$(build "$OUT/$name/a-much-longer-directory-name/b" "$OUT/$name/other-tmp-b" "$@") || { fail "$name: build b failed ($(tail -n 3 "$OUT/$name/a-much-longer-directory-name/b/build.log" | tr '\n' ' '))"; return 1; }
  if cmp -s "$a" "$b"; then SAME=1; else SAME=0; NDIFF=$(cmp -l "$a" "$b" 2>/dev/null | wc -l); fi
  SIZE=$(wc -c < "$a")
  return 0
}

# 1
SAME=0; NDIFF=0; SIZE=0
if pair repro; then
  if [ "$SAME" = 1 ]; then
    pass "ccompile from two directories, two seconds apart: the same $SIZE bytes"
  else
    fail "ccompile from two directories: $NDIFF bytes differ (R1 is back)"
  fi
fi

# 2
SAME=0; NDIFF=0; SIZE=0
if pair norepro FIDES_NO_REPRO=1; then
  if [ "$SAME" = 0 ]; then
    pass "with FIDES_NO_REPRO=1 (the fix off) $NDIFF bytes differ: check 1 catches R1"
  else
    echo "SKIP 2 (FIDES_NO_REPRO=1 gives the same bytes too: this compiler has no such switch)"
  fi
fi
exit $bad
