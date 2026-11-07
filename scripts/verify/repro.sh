#!/bin/bash
# repro.sh — reproducible builds (roadmap VF12): the same input must give the
# same bytes, whatever the directory, the temp folder, the time or the order.
#
#   FID=release/fid.exe scripts/verify/repro.sh [file.fd ...]
#
# Each program (default: a few tests of tests/bootstrap/) is copied into two
# directories with different paths, then built in each with a different
# TEMP/TMP and a pause between the builds:
#   dump-c   the generated C text
#   emit     the native executable (fid's own PE writer, no C compiler)
#   ccompile the executable through the C compiler (CC, default `zig cc`)
# and the two results are compared byte for byte. A difference is reported
# with the first differing offsets (cmp -l), so a timestamp, a path or a
# random value in the output can be found. COMPILER=1 also compares two
# `dump-c compiler/main.fd` runs (the fixpoint check compares stages, this
# compares one compiler with itself).
#
# Exit status: 0 when every output is reproducible, 1 otherwise.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
export CC="${CC:-zig cc}"
export FIDES_NO_CACHE=1
ROOT="$(pwd)"
WORK="${WORK:-build/repro}"
mkdir -p "$WORK"
WORK="$(cd "$WORK" && pwd)"
if [ $# -eq 0 ]; then
  set -- tests/bootstrap/hello.fd tests/bootstrap/test_struct_eq.fd \
         tests/bootstrap/test_closures.fd tests/bootstrap/test_hashmap.fd \
         tests/bootstrap/test_strings.fd
fi
bad=0
total=0

compare() {  # name a b
  total=$((total + 1))
  if cmp -s "$2" "$3"; then
    echo "  same      $1 ($(wc -c < "$2") bytes)"
  else
    bad=$((bad + 1))
    n=$(cmp -l "$2" "$3" 2>/dev/null | wc -l)
    echo "  DIFFERENT $1: $n bytes differ; first offsets (1-based, octal values):"
    cmp -l "$2" "$3" 2>/dev/null | head -8 | sed 's/^/      /'
  fi
}

for f in "$@"; do
  [ -f "$f" ] || { echo "missing: $f"; continue; }
  name=$(basename "$f" .fd)
  echo "$name"
  for side in a bb_longer_path; do
    d="$WORK/$side/$name"
    rm -rf "$d"; mkdir -p "$d/tmp"
    cp "$f" "$d/$name.fd"
    (
      cd "$ROOT"   # stdlib is found from the project root
      export TEMP="$d/tmp" TMP="$d/tmp" TMPDIR="$d/tmp"
      "$FID" dump-c "$d/$name.fd" > "$d/out.c" 2> /dev/null
      "$FID" emit "$d/$name.fd" -o "$d/emit.exe" > /dev/null 2>&1
      "$FID" ccompile "$d/$name.fd" -o "$d/cc.exe" > /dev/null 2>&1
    )
    sleep 2
  done
  A="$WORK/a/$name"; B="$WORK/bb_longer_path/$name"
  for out in out.c emit.exe cc.exe; do
    if [ -f "$A/$out" ] && [ -f "$B/$out" ]; then
      compare "$name/$out" "$A/$out" "$B/$out"
    else
      echo "  missing   $name/$out (build failed)"
    fi
  done
done

if [ -n "${COMPILER:-}" ]; then
  echo "compiler/main.fd"
  "$FID" dump-c compiler/main.fd > "$WORK/main1.c" 2> /dev/null
  "$FID" dump-c compiler/main.fd > "$WORK/main2.c" 2> /dev/null
  compare "dump-c compiler/main.fd" "$WORK/main1.c" "$WORK/main2.c"
fi

echo "$((total - bad)) of $total outputs reproducible"
[ "$bad" -eq 0 ]
