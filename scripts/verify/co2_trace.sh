#!/usr/bin/env bash
# Complaint CO2: FIDES_TRACE=1 turns on the stdlib's spans with no code
# change. Runs tests/bootstrap/test_stdlib_trace.fd with and without it:
# with it, stderr must hold the program's own span and file_write's
# "file.write" span; without it, stderr must hold no span. Then
# tests/bootstrap/test_trace_spans.fd (FIDES_TRACE unset; it turns tracing
# on itself): a stdlib span's parent is the program's outer span; and
# tests/bootstrap/test_http_server.fd, compiled and run with FIDES_TRACE=1:
# an "http.server" span per request.
#
#   FID=release/fid.exe bash scripts/verify/co2_trace.sh
set -euo pipefail
FID="${FID:-release/fid.exe}"
TEST=tests/bootstrap/test_stdlib_trace.fd
err=$(FIDES_TRACE=1 "$FID" run "$TEST" 2>&1 >/dev/null | tr -d '\r')
for span in '"span":"test.span"' '"span":"file.write","detail":"_fides_test_trace.txt"'; do
    if ! printf '%s\n' "$err" | grep -qF "$span"; then
        echo "FAIL: no $span span with FIDES_TRACE=1; stderr was:"
        echo "$err"
        exit 1
    fi
done
err=$(env -u FIDES_TRACE "$FID" run "$TEST" 2>&1 >/dev/null | tr -d '\r')
if printf '%s\n' "$err" | grep -qF '"span":'; then
    echo "FAIL: spans printed without FIDES_TRACE:"
    echo "$err"
    exit 1
fi

# line_has <stderr> <fixed text> <fixed text>: one span line holds both
line_has() {
    printf '%s
' "$1" | grep -F -- "$2" | grep -qF -- "$3"
}

# Span ids and parents: test_trace_spans.fd switches tracing on itself, so
# FIDES_TRACE stays unset; a stdlib span begun inside the program's outer
# span names it (id 1) as its parent
TEST=tests/bootstrap/test_trace_spans.fd
err=$(env -u FIDES_TRACE "$FID" run "$TEST" 2>&1 >/dev/null | tr -d '')
if ! line_has "$err" '"span":"file.read","detail":"_fides_test_trace_spans.txt"' '"parent":1'; then
    echo "FAIL: no file.read span with parent 1 from $TEST; stderr was:"
    echo "$err"
    exit 1
fi
if ! line_has "$err" '"span":"test.outer"' '"id":1,"parent":0'; then
    echo "FAIL: no test.outer span with id 1, parent 0 from $TEST; stderr was:"
    echo "$err"
    exit 1
fi

# The HTTP server's span per request: the server and its client are tasks,
# so the test is compiled (the interpreter does not interleave tasks)
TEST=tests/bootstrap/test_http_server.fd
OUT="${OUT:-build/co2-trace}"
mkdir -p "$OUT"
EXE="$OUT/test_http_server"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE="$EXE.exe" ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile "$TEST" -o "$EXE" > "$OUT/http-build.log" 2>&1; then
    echo "FAIL: $TEST does not compile (see $OUT/http-build.log)"
    exit 1
fi
case "$EXE" in /*|[A-Za-z]:*) ;; *) EXE="./$EXE" ;; esac
err=$(FIDES_TRACE=1 timeout 120 "$EXE" 2>&1 >/dev/null | tr -d '') || true
if ! printf '%s
' "$err" | grep -qF '"span":"http.server","detail":"GET /echo"'; then
    echo "FAIL: no http.server span for GET /echo with FIDES_TRACE=1; stderr was:"
    echo "$err"
    exit 1
fi
echo "co2 trace: ok (spans with FIDES_TRACE=1, none without; ids and parents; http.server)"
