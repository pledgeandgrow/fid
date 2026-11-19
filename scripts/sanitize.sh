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
# On Windows, scripts/sanitize-wsl.sh does the same runs with the Windows
# fid and gcc/clang inside WSL (first runs and findings:
# docs/verification/infra-runs-2026-11-16-b7.md).
#
#   FID=release/fid.exe CC=clang ./scripts/sanitize.sh            # all tests
#   ./scripts/sanitize.sh test_vec_builtins test_mem_owned_drop    # some
#   SAN=undefined ./scripts/sanitize.sh                            # UBSan only
#
# Every run reads no input and stops after LIMIT seconds (default 300). An
# output that differs from `fid run` is told apart: a program that prints
# something different on each run (times, random values) is judged on the
# sanitizer reports alone; otherwise the program is built again without the
# sanitizers, and the failure says whether the difference is the C path's
# ("also built without sanitizers") or appears only under them. The summary
# is also written to $OUT/summary.san (uploaded by verify-nightly.yml).
#
# SAN=thread: ThreadSanitizer (roadmap VF14) instead of ASan/UBSan (the two
# runtimes cannot be linked together). With no test named, it runs the
# programs that start threads, tasks or channels (TSAN_TESTS below); a data
# race report ("WARNING: ThreadSanitizer") is a finding, like an ASan one.
#   SAN=thread FID=release/fid CC=clang ./scripts/sanitize.sh
#
# COMPILER=1: the compiler itself under the sanitizers (own-backend roadmap
# RL15). fid's C (`dump-c compiler/main.fd`) is built with the same flags by
# scripts/verify/build-variant.sh into $OUT/fid-san; that compiler then
# checks, lowers to C (`dump-c`) and interprets (`run`) each test. Any
# sanitizer report is a memory error or undefined behaviour in the compiler;
# its `dump-c` text must also equal that of $FID (same source, so any
# difference is the compiler reading memory it should not).
# SELF=1 also has fid-san compile compiler/main.fd (dump-c; slow: minutes).
#   COMPILER=1 FID=release/fid CC=clang ./scripts/sanitize.sh test_closure test_hashmap
set -u
cd "$(dirname "$0")/.."

FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
CC="${CC:-clang}"
SAN="${SAN:-address,undefined}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-sanitize}"
mkdir -p "$OUT"

case ",$SAN," in
    *,thread,*)
        case ",$SAN," in *,address,*|*,memory,*)
            echo "sanitize: ThreadSanitizer cannot be combined with ASan/MSan (SAN=$SAN)"; exit 2 ;;
        esac ;;
esac
# -fno-sanitize-recover only for UBSan: an UB report then stops the program
RECOVER=""
case ",$SAN," in *,undefined,*) RECOVER="-fno-sanitize-recover=undefined" ;; esac
export CC
export FIDES_CFLAGS="-fsanitize=$SAN $RECOVER -fno-omit-frame-pointer -g"
export FIDES_NO_CACHE=1
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=0:halt_on_error=1}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"
# second_deadlock_stack: both stacks of a lock-order report
export TSAN_OPTIONS="${TSAN_OPTIONS:-halt_on_error=1:second_deadlock_stack=1}"

# The programs that run code on more than one thread (or the task runtime
# and channels, which take locks): the default set under SAN=thread
TSAN_TESTS="real_threads test_threads test_tasks test_tasks_10k test_spawn_scope
  test_spawn_deep_copy test_spawn_vec_copy test_channel test_channels_select
  test_chan_send_copy test_parallel test_parallel_branches test_parallel_concurrent
  test_lib_atomic test_async test_async_executor test_async_threads
  test_async_interleave test_async_io test_async_state test_async_suspend
  test_async_float_struct"

if [ $# -gt 0 ]; then
    tests=()
    for t in "$@"; do tests+=("tests/bootstrap/${t%.fd}.fd"); done
elif [ "${SAN#*thread}" != "$SAN" ]; then
    tests=()
    for t in $TSAN_TESTS; do
        [ -f "tests/bootstrap/$t.fd" ] && tests+=("tests/bootstrap/$t.fd")
    done
else
    tests=(tests/bootstrap/test_*.fd)
fi

pass=0; fail=0; skip=0; nondet=0
failed=()
# LIMIT seconds per run (default 300; a hanging test must not hang the job);
# macOS has gtimeout (coreutils) or nothing
LIMIT="${LIMIT:-300}"
TMO=""
if command -v timeout > /dev/null 2>&1; then TMO="timeout $LIMIT"
elif command -v gtimeout > /dev/null 2>&1; then TMO="gtimeout $LIMIT"; fi
# a run reads no input: a test waiting on stdin would otherwise stop the job
run_quiet() { $TMO "$@" < /dev/null; }
san_hit() { grep -q 'ERROR: AddressSanitizer\|runtime error:\|ERROR: LeakSanitizer\|WARNING: ThreadSanitizer\|ERROR: ThreadSanitizer' "$1"; }

if [ -n "${COMPILER:-}" ]; then
    SANFID="$OUT/fid-san"
    echo "building the compiler with -fsanitize=$SAN into $SANFID ..."
    EXTRA_CFLAGS="$FIDES_CFLAGS" FID="$FID" bash scripts/verify/build-variant.sh . "$SANFID" || {
        echo "sanitize: could not build the sanitized compiler"; exit 1; }
    [ -x "$SANFID" ] || SANFID="$SANFID.exe"
    for f in "${tests[@]}"; do
        name=$(basename "$f" .fd)
        case "$name" in test_sqlite|test_gpu*|test_sdl*) skip=$((skip + 1)); continue ;; esac
        bad=""
        for step in check dump-c run; do
            log="$OUT/$name.fid-$step.san"
            timeout 900 "$SANFID" "$step" "$f" < /dev/null > "$OUT/$name.fid-$step.out" 2> "$log"
            if san_hit "$log"; then bad="$bad $step(sanitizer: $log)"; continue; fi
            # the same source through the unsanitized compiler must give
            # the same C text (`run` output is not compared: some tests
            # print times or random values)
            if [ "$step" = dump-c ]; then
                timeout 900 "$FID" "$step" "$f" < /dev/null > "$OUT/$name.ref-$step.out" 2> /dev/null
                cmp -s "$OUT/$name.fid-$step.out" "$OUT/$name.ref-$step.out" || bad="$bad $step(differs from $FID)"
            fi
        done
        if [ -n "$bad" ]; then
            fail=$((fail + 1)); failed+=("$name:$bad")
        else
            pass=$((pass + 1))
        fi
    done
    if [ -n "${SELF:-}" ]; then
        timeout 7200 "$SANFID" dump-c compiler/main.fd > "$OUT/self.c" 2> "$OUT/self.san"
        if san_hit "$OUT/self.san"; then
            fail=$((fail + 1)); failed+=("compiler/main.fd (sanitizer: $OUT/self.san)")
        else
            pass=$((pass + 1))
        fi
    fi
    echo "sanitize compiler ($SAN): $pass clean, $fail failed, $skip skipped"
    for x in "${failed[@]+"${failed[@]}"}"; do echo "  FAIL $x"; done
    [ "$fail" -eq 0 ]
    exit
fi

for f in "${tests[@]}"; do
    name=$(basename "$f" .fd)
    case "$name" in
        *_fail) skip=$((skip + 1)); continue ;;
        test_sqlite|test_gpu*|test_sdl*) skip=$((skip + 1)); continue ;;
    esac
    want=$(run_quiet "$FID" run "$f" 2>/dev/null)
    if ! "$FID" ccompile "$f" -o "$OUT/$name" > "$OUT/$name.build" 2>&1; then
        skip=$((skip + 1))   # does not build through C at all: not a sanitizer finding
        continue
    fi
    got=$(run_quiet "$OUT/$name" 2> "$OUT/$name.san")
    if san_hit "$OUT/$name.san"; then
        fail=$((fail + 1)); failed+=("$name (sanitizer: $OUT/$name.san)")
    elif [ "$got" != "$want" ]; then
        # Which difference is it? A program that prints times, addresses or
        # random values differs from itself: then only the sanitizers count.
        want2=$(run_quiet "$FID" run "$f" 2>/dev/null)
        if [ "$want2" != "$want" ]; then
            nondet=$((nondet + 1)); pass=$((pass + 1))
            continue
        fi
        # The same program built without the sanitizers: if it prints what
        # the sanitized one does, the C path differs from `fid run` (a bug,
        # but not a memory error); if not, the difference appears only
        # under the sanitizers (calloc'd blocks, ASan's layout): a finding.
        plain=""
        if FIDES_CFLAGS="" "$FID" ccompile "$f" -o "$OUT/$name.plain" > /dev/null 2>&1; then
            plain=$(run_quiet "$OUT/$name.plain" 2> /dev/null)
        fi
        if [ "$plain" = "$got" ]; then
            fail=$((fail + 1)); failed+=("$name (output differs from fid run, also built without sanitizers)")
        else
            fail=$((fail + 1)); failed+=("$name (output differs from fid run only under the sanitizers)")
        fi
    else
        pass=$((pass + 1))
    fi
done

{
    echo "sanitize ($SAN): $pass clean, $fail failed, $skip skipped" \
         "($nondet of the clean ones print different things each run: sanitizer reports only)"
    for x in "${failed[@]+"${failed[@]}"}"; do echo "  FAIL $x"; done
} | tee "$OUT/summary.san"
[ "$fail" -eq 0 ]
