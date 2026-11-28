#!/bin/bash
# BG362 (docs/roadmap/bugs-roadmap.md): `fid run`'s panic trace and the
# --crash-file report `fid inspect` prints name each function with the line
# it is at (compiler/interp.fd Interpreter_format_call_stack_at): the
# innermost frame at the panicking line, each caller at the line of its
# call. Before, every line was shifted one frame up ("c at line 18", the
# line in b where c is called) and main had none.
#   FID=release/fid.exe bash tests/tooling/bg362-trace-lines-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
P=tests/bootstrap/test_bg362_trace_lines.fd
D=$(mktemp -d ./tests/_bg362_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
"$FID" run "$P" --crash-file "$D/r.txt" > "$D/out.txt" 2> "$D/err.txt"
code=$?
if [ $code -ne 0 ]; then echo "ok   the run fails"; else echo "FAIL the run exits 0"; fail=1; fi
has() {
    if tr -d '\r' < "$2" | grep -qF -- "$3"; then echo "ok   $1"; else echo "FAIL $1: no line with: $3"; fail=1; fi
}
hasnt() {
    if tr -d '\r' < "$2" | grep -qF -- "$3"; then echo "FAIL $1: a line with: $3"; fail=1; else echo "ok   $1"; fi
}
has "panic line" "$D/err.txt" "Division by zero at line 14"
has "c at its line" "$D/err.txt" "c at line 14"
has "b at its line" "$D/err.txt" "called from b at line 18"
has "a at its line" "$D/err.txt" "called from a at line 22"
has "main at its line" "$D/err.txt" "called from main at line 27"
hasnt "no caller's line on c" "$D/err.txt" "c at line 18"
if [ -s "$D/r.txt" ]; then
    "$FID" inspect "$D/r.txt" > "$D/insp.txt" 2>&1
    has "report: c at its line" "$D/insp.txt" "c at line 14"
    has "report: main at its line" "$D/insp.txt" "main at line 27"
else
    echo "FAIL no crash report written"; fail=1
fi
exit $fail
