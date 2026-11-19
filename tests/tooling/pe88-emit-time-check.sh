#!/bin/bash
# pe-roadmap 88: `fid emit` (pe_ir, the Windows default) of a typical test
# in under 1 second, with an -O2 compiler on a quiet machine, caches warm.
#
# compiler/main.fd pe88_prune drops, between lowering and the mid-end, the
# functions no code of a pe_ir build can call (core functions neither main
# nor the runtime reaches), so iropt's module-wide passes and pe_ir's
# module-wide work go over fewer functions. Checked here:
#   1. the build says what it dropped ("[build] pe88: N of M functions ..."),
#      N > 0 for hello.fd
#   2. each program below, built with the prune (the default) and without
#      it (FID_PE_PRUNE=0), prints the same and exits the same
#   3. TIME (default 1): the best of RUNS (default 5) `emit --no-cache --time`
#      runs of hello.fd and of a mid-size test (test_collections.fd), after
#      one warm-up run (D9's checked-library stamp, the import and AST
#      caches); each total must be under LIMIT_MS (default 1000). The stage
#      table of the best run is printed: record it in pe-roadmap 88.
#      `--no-cache` only skips the "up to date" answer of an unchanged
#      output, so every run builds. The time of the prune itself is its own
#      row ("prune"), and the same runs with FID_PE_PRUNE=0 show what it
#      saves.
# ALL=1 compares the two builds over every test of tests/bootstrap that is
# not expected to fail (long).
#   FID=release/fid.exe bash tests/tooling/pe88-emit-time-check.sh
#   TIME=0 FID=release/fid.exe bash tests/tooling/pe88-emit-time-check.sh
# Run from the repository root on Windows (pe_ir writes a Windows .exe).
# Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
ROOT=$(pwd)
case "$FID" in
    /*|[A-Za-z]:*) ;;
    */*) FID="$ROOT/$FID" ;;
esac
export FID_HOME=${FID_HOME:-$ROOT}
unset FID_PE_PRUNE FID_LOWER_ALL
TIME=${TIME:-1}
RUNS=${RUNS:-5}
LIMIT_MS=${LIMIT_MS:-1000}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# 1. hello.fd: the prune runs and drops something
if "$FID" emit tests/bootstrap/hello.fd -o "$TMP/hello.exe" --no-cache > "$TMP/hello.log" 2> "$TMP/hello.err"; then
    line=$(grep -o "\[build\] pe88: [0-9]* of [0-9]* functions" "$TMP/hello.err" | tail -1)
    n=$(echo "$line" | sed -n 's/.*pe88: \([0-9]*\) of.*/\1/p')
    if [ -n "$n" ] && [ "$n" -gt 0 ]; then ok "hello.fd: $line dropped"; else bad "hello.fd: no '[build] pe88: N of M' line with N > 0 (got '$line')"; fi
    if grep -q "pe_ir failed on the pruned module" "$TMP/hello.err"; then bad "hello.fd: the pruned build failed and was made again"; fi
    out=$("$TMP/hello.exe" | tr -d '\r')
    if [ "$out" = "$(printf 'Hello, Fides!\n50\ny is big')" ] || [ "$out" = "Hello, Fides!50y is big" ]; then ok "hello.fd: output"; else bad "hello.fd: output '$out'"; fi
else
    bad "hello.fd: emit failed"; sed 's/^/    /' "$TMP/hello.log" | head -20
fi
FID_PE_PRUNE=0 "$FID" emit tests/bootstrap/hello.fd -o "$TMP/hello0.exe" --no-cache > /dev/null 2> "$TMP/hello0.err"
if grep -q "\[build\] pe88:" "$TMP/hello0.err"; then bad "FID_PE_PRUNE=0 still prunes"; else ok "FID_PE_PRUNE=0: no prune"; fi

# 2. the same programs with and without the prune
compare() {
    local t=$1 base
    base=$(basename "$t" .fd)
    if ! "$FID" emit "$t" -o "$TMP/$base.p.exe" --no-cache > "$TMP/$base.p.log" 2> "$TMP/$base.p.err"; then
        if FID_PE_PRUNE=0 "$FID" emit "$t" -o "$TMP/$base.f.exe" --no-cache > /dev/null 2>&1; then
            bad "$base: builds without the prune, not with it"; sed 's/^/    /' "$TMP/$base.p.log" | head -10
        else
            echo "skip $base: pe_ir does not build it either way"
        fi
        return
    fi
    if grep -q "pe_ir failed on the pruned module" "$TMP/$base.p.err"; then bad "$base: the pruned build failed and was made again"; fi
    if ! FID_PE_PRUNE=0 "$FID" emit "$t" -o "$TMP/$base.f.exe" --no-cache > /dev/null 2>&1; then
        bad "$base: builds with the prune only"; return
    fi
    (cd "$(dirname "$t")" && timeout 60 "$TMP/$base.p.exe" < /dev/null > "$TMP/$base.p.out" 2> /dev/null); local ep=$?
    (cd "$(dirname "$t")" && timeout 60 "$TMP/$base.f.exe" < /dev/null > "$TMP/$base.f.out" 2> /dev/null); local ef=$?
    if [ $ep -ne $ef ]; then bad "$base: exit $ep with the prune, $ef without"; return; fi
    if cmp -s <(tr -d '\r' < "$TMP/$base.p.out") <(tr -d '\r' < "$TMP/$base.f.out"); then
        ok "$base: same output and exit ($ef) with and without the prune"
    else
        bad "$base: output differs with the prune"; diff <(tr -d '\r' < "$TMP/$base.f.out") <(tr -d '\r' < "$TMP/$base.p.out") | head -10 | sed 's/^/    /'
    fi
}
if [ "${ALL:-0}" = "1" ]; then
    LIST=$(ls tests/bootstrap/*.fd | grep -v '_fail\.fd$')
else
    LIST="tests/bootstrap/hello.fd tests/bootstrap/test_pe88_prune_reach.fd tests/bootstrap/test_collections.fd tests/bootstrap/test_matrix_mul.fd tests/bootstrap/test_traits.fd tests/bootstrap/test_drop.fd tests/bootstrap/test_vec_builtins.fd tests/bootstrap/test_closure.fd tests/bootstrap/test_closure_devirt.fd tests/bootstrap/test_frontend_dyn_default.fd tests/bootstrap/test_async.fd tests/bootstrap/test_async_threads.fd"
fi
for t in $LIST; do
    [ -f "$t" ] && compare "$t"
done
# test_pe88_prune_reach.fd: what only a trait object, a function value, a
# closure or a scope exit reaches is still there (its expected lines)
R=test_pe88_prune_reach
if [ -f "$TMP/$R.p.out" ]; then
    want=$(sed -n '/^\/\/ Expected output:/,/^$/p' tests/bootstrap/$R.fd | sed -n 's|^// ||p' | grep -v '^Expected output:')
    if [ "$(tr -d '\r' < "$TMP/$R.p.out")" = "$want" ]; then ok "$R: expected output with the prune"; else bad "$R: output with the prune"; sed 's/^/    /' "$TMP/$R.p.out"; fi
fi

# 3. the time
best_total() {
    # best_total <file.fd> <tag>: the smallest "[time] total" of RUNS runs
    local t=$1 tag=$2 i best=999999 ms
    "$FID" emit "$t" -o "$TMP/time.exe" --no-cache --time > /dev/null 2> "$TMP/warm.err"
    for i in $(seq 1 "$RUNS"); do
        "$FID" emit "$t" -o "$TMP/time.exe" --no-cache --time > /dev/null 2> "$TMP/run.err"
        ms=$(grep "^\[time\] total:" "$TMP/run.err" | sed 's/[^0-9]//g' | tail -1)
        if [ -n "$ms" ] && [ "$ms" -lt "$best" ]; then
            best=$ms
            cp "$TMP/run.err" "$TMP/best-$tag.err"
        fi
    done
    echo "$best"
}
if [ "$TIME" = "1" ]; then
    others=$(tasklist 2>/dev/null | grep -i -c -E "^(fid|fid-dev|stage[0-9]|zig|clang|cc1)[^ ]*\.exe" || true)
    if [ "${others:-0}" -gt 1 ]; then
        echo "note: $others compiler processes are running: the times below are not those of a quiet machine"
    fi
    for t in tests/bootstrap/hello.fd tests/bootstrap/test_collections.fd; do
        base=$(basename "$t" .fd)
        ms=$(best_total "$t" "$base")
        ms0=$(FID_PE_PRUNE=0 best_total "$t" "${base}-noprune")
        echo "--- $base.fd: best of $RUNS: ${ms} ms (FID_PE_PRUNE=0: ${ms0} ms) ---"
        if [ -f "$TMP/best-$base.err" ]; then
            grep "^\[time\]" "$TMP/best-$base.err" | grep -v "^\[time\]   iropt" | sed 's/^/    /'
        fi
        if [ "$ms" -lt "$LIMIT_MS" ]; then ok "$base.fd: emit in $ms ms (< $LIMIT_MS)"; else bad "$base.fd: emit in $ms ms (limit $LIMIT_MS)"; fi
    done
fi

if [ $fail -eq 0 ]; then echo "PASS"; else echo "FAILED"; fi
exit $fail
