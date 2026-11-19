#!/bin/bash
# CT7 (docs/roadmap/complaint-roadmap.md): record what a run's effects
# bring in, replay the run exactly in the interpreter. `fid run <file>
# --record <trace>` logs each effect call (compiler/interp.fd
# Interpreter_rr_call); `fid replay <trace>` runs the program again with
# each of those calls answered from the trace. The fixture
# (tests/projects/ct7_replay/inputs.fd) reads a file named by an
# environment variable, the clock, OS randomness and a line of stdin, and
# writes a file. The replay runs with the input file gone, the variables
# unset and other stdin: it must print the same lines and write nothing.
#   FID=release/fid.exe bash tests/tooling/replay-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
ROOT=$(pwd)
case "$FID" in
    /*|[A-Za-z]:*) ;;
    */*) FID="$ROOT/$FID" ;;
esac
PROG=tests/projects/ct7_replay/inputs.fd
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
echo "recorded input" > "$TMP/in.txt"

# 1. the recorded run
echo "first line" | CT7_INPUT="$TMP/in.txt" CT7_OUTPUT="$TMP/out.txt" "$FID" run "$PROG" --record "$TMP/run.trace" > "$TMP/rec.out" 2> "$TMP/rec.err"
code=$?
if [ $code -eq 0 ]; then ok "the recorded run succeeds"; else bad "the recorded run: exit $code"; cat "$TMP/rec.err"; fi
if grep -qF "read: recorded input" "$TMP/rec.out" && grep -qF "stdin: first line" "$TMP/rec.out"; then ok "the recorded run read its inputs"; else bad "the recorded run did not read its inputs"; fi
if [ -f "$TMP/out.txt" ]; then ok "the recorded run wrote its file"; else bad "the recorded run wrote no file"; fi
if head -1 "$TMP/run.trace" 2>/dev/null | grep -qF "fid-replay 1"; then ok "a trace is written"; else bad "no trace"; fi
for ev in p_getenv p_file_read p_clock_now_ms p_os_random_u64 p_read_line p_file_write; do
    if grep -q "^ev $ev " "$TMP/run.trace"; then ok "the trace holds $ev"; else bad "the trace has no $ev call"; fi
done
if grep -q "^exit 0" "$TMP/run.trace"; then ok "the trace holds the exit code"; else bad "no exit line in the trace"; fi

# 2. the replay: no input file, no variables, other stdin, a later clock
rm -f "$TMP/in.txt" "$TMP/out.txt"
echo "other line" | "$FID" replay "$TMP/run.trace" > "$TMP/rep.out" 2> "$TMP/rep.err"
code=$?
if [ $code -eq 0 ]; then ok "the replay succeeds"; else bad "the replay: exit $code"; cat "$TMP/rep.err"; fi
grep -v '^\[' "$TMP/rec.out" | tr -d '\r' > "$TMP/rec.lines"
grep -v '^\[' "$TMP/rep.out" | tr -d '\r' > "$TMP/rep.lines"
if [ -s "$TMP/rec.lines" ] && cmp -s "$TMP/rec.lines" "$TMP/rep.lines"; then ok "the replay prints what the recorded run printed"; else bad "the replay's output differs"; diff "$TMP/rec.lines" "$TMP/rep.lines"; fi
if [ -f "$TMP/out.txt" ]; then bad "the replay wrote the file (real I/O)"; else ok "the replay wrote no file"; fi

# 3. --steps shows each answered call
"$FID" replay "$TMP/run.trace" --steps < /dev/null > /dev/null 2> "$TMP/steps.err"
if grep -q '^\[replay [0-9]*\] p_getenv("CT7_INPUT") -> ' "$TMP/steps.err" && grep -q "^\[replay [0-9]*\] p_clock_now_ms() -> " "$TMP/steps.err"; then ok "--steps prints each answered call"; else bad "--steps did not print the calls"; cat "$TMP/steps.err"; fi

# 4. a run that differs from the recording is stopped and reported
grep -v "^ev p_clock_now_ms " "$TMP/run.trace" > "$TMP/bad.trace"
"$FID" replay "$TMP/bad.trace" < /dev/null > /dev/null 2> "$TMP/bad.err"
code=$?
if [ $code -ne 0 ] && grep -qF "differs" "$TMP/bad.err"; then ok "a run that differs from the recording is reported"; else bad "a differing replay was not reported (exit $code)"; cat "$TMP/bad.err"; fi

# 5. not a trace
echo "hello" > "$TMP/no.trace"
"$FID" replay "$TMP/no.trace" > /dev/null 2> "$TMP/no.err"
if [ $? -ne 0 ] && grep -qF "not a trace" "$TMP/no.err"; then ok "a file that is not a trace is refused"; else bad "a non-trace was not refused"; fi

# 6. --until <k> stops before effect call k and shows where the program is
"$FID" replay "$TMP/run.trace" --until 2 < /dev/null > "$TMP/until.out" 2> "$TMP/until.err"
code=$?
if [ $code -eq 0 ] && grep -qF "[replay] stopped before effect call 2 of " "$TMP/until.err" && grep -qF "main" "$TMP/until.err"; then ok "--until stops at the call and shows the stack"; else bad "--until (exit $code)"; cat "$TMP/until.err"; fi
if grep -qF "done" "$TMP/until.out"; then bad "--until ran past the stop"; else ok "--until runs no further"; fi

# 7. bytes, a captured process and p_exit (tests/projects/ct7_replay/exit.fd)
PROG2=tests/projects/ct7_replay/exit.fd
printf 'A\000B\377' > "$TMP/in.bin"
CT7_BYTES="$TMP/in.bin" CT7_CMD="echo from-process" "$FID" run "$PROG2" --record "$TMP/exit.trace" < /dev/null > "$TMP/exit_rec.out" 2> "$TMP/exit_rec.err"
code=$?
if [ $code -eq 3 ]; then ok "the recorded run exits 3 (p_exit)"; else bad "the recorded run: exit $code"; cat "$TMP/exit_rec.err"; fi
if grep -qF "bytes: 4 sum: 386 zeros: 1" "$TMP/exit_rec.out" && grep -qF "process: from-process" "$TMP/exit_rec.out"; then ok "the recorded run read the bytes and the process output"; else bad "the recorded run's output"; cat "$TMP/exit_rec.out"; fi
if grep -q "^ev p_file_read_bytes .* y " "$TMP/exit.trace" && grep -q "^ev p_process_run " "$TMP/exit.trace" && grep -q "^ev p_process_stdout " "$TMP/exit.trace"; then ok "the trace holds the bytes and the process calls"; else bad "the trace lacks the bytes or the process calls"; fi
if grep -q "^exit 3" "$TMP/exit.trace"; then ok "p_exit ends the trace with its code"; else bad "no 'exit 3' line after p_exit"; fi
rm -f "$TMP/in.bin"
"$FID" replay "$TMP/exit.trace" < /dev/null > "$TMP/exit_rep.out" 2> "$TMP/exit_rep.err"
code=$?
if [ $code -eq 3 ]; then ok "the replay exits 3"; else bad "the replay: exit $code"; cat "$TMP/exit_rep.err"; fi
grep -v '^\[' "$TMP/exit_rec.out" | tr -d '\r' > "$TMP/exit_rec.lines"
grep -v '^\[' "$TMP/exit_rep.out" | tr -d '\r' > "$TMP/exit_rep.lines"
if [ -s "$TMP/exit_rec.lines" ] && cmp -s "$TMP/exit_rec.lines" "$TMP/exit_rep.lines"; then ok "the replay prints the same (bytes, process output from the trace)"; else bad "the replay's output differs"; diff "$TMP/exit_rec.lines" "$TMP/exit_rep.lines"; fi
# a replay that stops short of the recording's p_exit is reported
grep -v "^ev p_process_close " "$TMP/exit.trace" > "$TMP/exit_bad.trace"
"$FID" replay "$TMP/exit_bad.trace" < /dev/null > /dev/null 2> "$TMP/exit_bad.err"
if grep -qF "differs" "$TMP/exit_bad.err" || grep -qF "after the last recorded" "$TMP/exit_bad.err"; then ok "a replay that differs before p_exit is reported"; else bad "a differing replay before p_exit was not reported"; cat "$TMP/exit_bad.err"; fi

exit $fail
