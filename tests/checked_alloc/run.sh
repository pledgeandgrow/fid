#!/usr/bin/env bash
# `fid emit --checked-alloc` (release M7, complaint CM9) on native Windows.
#
#   bash tests/checked_alloc/run.sh            # the programs in this folder
#   SUITE=1 bash tests/checked_alloc/run.sh    # + tests/bootstrap through it
#
# Each program here says what the checked build must print:
#   // expect-checked: <text>   the build faults (nonzero exit) and prints <text>
#   // expect-output: <line>    the build exits 0 and prints <line>
# SUITE=1 also builds every tests/bootstrap/test_*.fd that is not an
# expected failure with --checked-alloc and compares its output and exit code
# with the same program built without it (BASE=emit, the default) or with
# `fid run` (BASE=run); LIMIT=n: the first n; ONLY=pattern: names matching;
# TIMEOUT=s per build and run (120); tests/checked_alloc/suite-skip.txt:
# names left out. A FAULT line is a --checked-alloc report in a suite test:
# a real use after free or overrun, see the SUITE section below.
# FID: the compiler (default release/fid.exe). Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-checked-alloc}"
mkdir -p "$OUT"
pass=0
fail=0

for src in tests/checked_alloc/*.fd; do
    name=$(basename "$src" .fd)
    exe="$OUT/$name.exe"
    if ! "$FID" emit "$src" --checked-alloc -o "$exe" > "$OUT/$name.build" 2>&1; then
        echo "FAIL $name: does not build"; tail -5 "$OUT/$name.build"; fail=$((fail + 1)); continue
    fi
    "$exe" > "$OUT/$name.out" 2>&1
    code=$?
    ok=1
    want_fault=0
    while IFS= read -r line; do
        case "$line" in
            "// expect-checked: "*)
                want_fault=1
                text="${line#// expect-checked: }"
                grep -qF -- "$text" "$OUT/$name.out" || { echo "FAIL $name: missing: $text"; ok=0; } ;;
            "// expect-output: "*)
                text="${line#// expect-output: }"
                grep -qxF -- "$text" "$OUT/$name.out" || { echo "FAIL $name: missing line: $text"; ok=0; } ;;
        esac
    done < "$src"
    if [ $want_fault = 1 ] && [ $code = 0 ]; then echo "FAIL $name: did not fault"; ok=0; fi
    if [ $want_fault = 0 ] && [ $code != 0 ]; then echo "FAIL $name: exit $code"; ok=0; fi
    if [ $ok = 1 ]; then pass=$((pass + 1)); echo "ok   $name"; else fail=$((fail + 1)); sed 's/^/     | /' "$OUT/$name.out"; fi
done

if [ "${SUITE:-0}" = 1 ]; then
    # The oracle (BASE): `emit` (default) is the same program built without
    # --checked-alloc, so a difference is the checked allocator's; `run` is
    # `fid run`, as before. Each build and run is under timeout TIMEOUT
    # (default 120 s), stdin from /dev/null. Names in
    # tests/checked_alloc/suite-skip.txt (one per line, then why) are left
    # out. Results, one `name|state|detail` line each, go to $OUT/suite.txt;
    # state is PASS, FAIL, FAULT (the checked build stopped with a
    # --checked-alloc report: a use after free or an overrun in the program,
    # core or the runtime, to be fixed, not skipped), BASE (the oracle itself
    # did not build or timed out: not counted), NOBUILD (the checked build
    # does not build while the plain one does: counted as a failure) or SKIP.
    BASE="${BASE:-emit}"
    TIMEOUT="${TIMEOUT:-120}"
    SKIPF=tests/checked_alloc/suite-skip.txt
    SKIPS=""
    [ -f "$SKIPF" ] && SKIPS="$(grep -v '^#' "$SKIPF" | tr -d '\r' | awk 'NF {print $1}')"
    : > "$OUT/suite.txt"
    n=0
    n_fault=0
    n_skip=0
    n_base=0
    for src in tests/bootstrap/test_*.fd; do
        name=$(basename "$src" .fd)
        case "$name" in *_fail) continue ;; esac
        if [ -n "${ONLY:-}" ] && ! echo "$name" | grep -q -- "$ONLY"; then continue; fi
        grep -q "expect-error" "$src" && continue
        if [ -n "$SKIPS" ] && echo "$SKIPS" | grep -qxF -- "$name"; then
            n_skip=$((n_skip + 1)); echo "$name|SKIP|suite-skip.txt" >> "$OUT/suite.txt"; continue
        fi
        n=$((n + 1))
        if [ -n "${LIMIT:-}" ] && [ $n -gt "$LIMIT" ]; then break; fi
        if [ "$BASE" = run ]; then
            timeout "$TIMEOUT" "$FID" run "$src" > "$OUT/$name.want" 2>/dev/null < /dev/null
            want_code=$?
        else
            if ! timeout "$TIMEOUT" "$FID" emit "$src" -o "$OUT/$name.plain.exe" > "$OUT/$name.plain.build" 2>&1; then
                n_base=$((n_base + 1)); echo "$name|BASE|plain emit does not build" >> "$OUT/suite.txt"
                echo "SKIP $name: does not build natively"; continue
            fi
            timeout "$TIMEOUT" "$OUT/$name.plain.exe" > "$OUT/$name.want" 2>/dev/null < /dev/null
            want_code=$?
        fi
        if [ $want_code = 124 ]; then
            n_base=$((n_base + 1)); echo "$name|BASE|oracle timed out" >> "$OUT/suite.txt"
            echo "SKIP $name: the $BASE oracle timed out"; continue
        fi
        if ! timeout "$TIMEOUT" "$FID" emit "$src" --checked-alloc -o "$OUT/$name.exe" > "$OUT/$name.build" 2>&1; then
            if [ "$BASE" = run ]; then
                echo "SKIP $name: does not build natively"; echo "$name|BASE|does not build natively" >> "$OUT/suite.txt"
                n_base=$((n_base + 1))
            else
                fail=$((fail + 1)); echo "FAIL $name: builds without --checked-alloc, not with it"
                echo "$name|NOBUILD|$(tail -1 "$OUT/$name.build" | tr -d '\r|')" >> "$OUT/suite.txt"
            fi
            continue
        fi
        timeout "$TIMEOUT" "$OUT/$name.exe" > "$OUT/$name.got" 2> "$OUT/$name.err" < /dev/null
        got_code=$?
        if cmp -s "$OUT/$name.want" "$OUT/$name.got" && [ $want_code = $got_code ]; then
            pass=$((pass + 1)); echo "$name|PASS|" >> "$OUT/suite.txt"
        elif grep -q -- "(--checked-alloc)" "$OUT/$name.got" "$OUT/$name.err"; then
            fail=$((fail + 1)); n_fault=$((n_fault + 1))
            why=$(grep -h -- "(--checked-alloc)" "$OUT/$name.got" "$OUT/$name.err" | head -1 | tr -d '\r|')
            echo "FAULT $name: $why"
            echo "$name|FAULT|$why" >> "$OUT/suite.txt"
        else
            fail=$((fail + 1)); echo "FAIL $name (exit $want_code vs $got_code)"
            diff "$OUT/$name.want" "$OUT/$name.got" | head -5
            echo "$name|FAIL|exit $want_code vs $got_code" >> "$OUT/suite.txt"
        fi
    done
    echo "checked-alloc suite (oracle: $BASE): $n_fault of the failures are --checked-alloc reports, $n_base not compared (the oracle did not build or timed out), $n_skip skipped; results in $OUT/suite.txt"
fi

echo "checked-alloc: $pass passed, $fail failed"
[ $fail = 0 ]
