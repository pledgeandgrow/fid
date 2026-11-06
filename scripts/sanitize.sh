#!/usr/bin/env bash
# Run the test programs through the C backend under AddressSanitizer and
# UndefinedBehaviorSanitizer (release M8).
#
# Each tests/bootstrap/test_*.fd that is not an expected-fail test is built
# with `fid ccompile` and FIDES_CFLAGS="-fsanitize=..." and run; its output
# must match `fid run` and the sanitizers must stay quiet. Under ASan the C
# runtime takes every block from calloc (P_SAN_ALLOC in
# compiler/codegen_c.fd), so a use after free or an overrun of a Fides
# allocation is reported, not hidden in the block pools.
#
# Needs clang or gcc with the sanitizer runtimes: Linux or macOS. (zig cc
# has no ASan runtime; clang-cl on Windows has one but is not wired here.)
#
#   FID=release/fid.exe CC=clang ./scripts/sanitize.sh            # all tests
#   ./scripts/sanitize.sh test_vec_builtins test_mem_owned_drop    # some
#   SAN=undefined ./scripts/sanitize.sh                            # UBSan only
set -u
cd "$(dirname "$0")/.."

FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
CC="${CC:-clang}"
SAN="${SAN:-address,undefined}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-sanitize}"
mkdir -p "$OUT"

export CC
export FIDES_CFLAGS="-fsanitize=$SAN -fno-sanitize-recover=undefined -fno-omit-frame-pointer -g"
export FIDES_NO_CACHE=1
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=0:halt_on_error=1}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"

if [ $# -gt 0 ]; then
    tests=()
    for t in "$@"; do tests+=("tests/bootstrap/${t%.fd}.fd"); done
else
    tests=(tests/bootstrap/test_*.fd)
fi

pass=0; fail=0; skip=0
failed=()
for f in "${tests[@]}"; do
    name=$(basename "$f" .fd)
    case "$name" in
        *_fail) skip=$((skip + 1)); continue ;;
        test_sqlite|test_gpu*|test_sdl*) skip=$((skip + 1)); continue ;;
    esac
    want=$("$FID" run "$f" 2>/dev/null)
    if ! "$FID" ccompile "$f" -o "$OUT/$name" > "$OUT/$name.build" 2>&1; then
        skip=$((skip + 1))   # does not build through C at all: not a sanitizer finding
        continue
    fi
    got=$("$OUT/$name" 2> "$OUT/$name.san")
    if grep -q 'ERROR: AddressSanitizer\|runtime error:\|ERROR: LeakSanitizer' "$OUT/$name.san"; then
        fail=$((fail + 1)); failed+=("$name (sanitizer: $OUT/$name.san)")
    elif [ "$got" != "$want" ]; then
        fail=$((fail + 1)); failed+=("$name (output differs from fid run)")
    else
        pass=$((pass + 1))
    fi
done

echo "sanitize ($SAN): $pass clean, $fail failed, $skip skipped"
for x in "${failed[@]+"${failed[@]}"}"; do echo "  FAIL $x"; done
[ "$fail" -eq 0 ]
