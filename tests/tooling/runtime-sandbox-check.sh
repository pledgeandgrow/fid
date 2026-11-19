#!/bin/bash
# Roadmap Phase 12 "Runtime sandbox -- capability revocation in interpreter":
# `fid run` (compiler/interp.fd) revokes capabilities while the program runs
# (p_cap_revoke, p_cap_revoked, p_sandbox_enter/exit), so a call the type
# checker cannot see through -- a function value -- is stopped too.
#   FID=release/fid.exe bash tests/tooling/runtime-sandbox-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
T=tests/bootstrap/test_runtime_sandbox_fail.fd
OUT=$(mktemp)
ERR=$(mktemp)
fail=0
"$FID" run "$T" > "$OUT" 2> "$ERR"
code=$?
expected=$(printf '0\n1\n0\n0\n1\n-1\nstill printing')
if [ "$(tr -d '\r' < "$OUT")" = "$expected" ]; then echo "ok   output up to the revoked call"; else echo "FAIL output up to the revoked call"; fail=1; fi
if [ $code -ne 0 ]; then echo "ok   the run fails"; else echo "FAIL the run fails: exit 0"; fail=1; fi
if grep -qF "Sandbox error: 'p_file_exists' needs capability 'FS', which was revoked at run time" "$ERR"; then echo "ok   sandbox error names the call and the capability"; else echo "FAIL no sandbox error"; fail=1; fi
# a normal program is unchanged
P=$(mktemp --suffix=.fd 2>/dev/null || echo "${TMPDIR:-/tmp}/sandbox_plain_$$.fd")
printf '%s\n' 'fn main() {' '    println(p_file_exists("no_such_file_for_sandbox_test.txt"));' '    println("plain");' '}' > "$P"
if [ "$("$FID" run "$P" 2>/dev/null | tr -d '\r')" = "$(printf 'false\nplain')" ] || [ "$("$FID" run "$P" 2>/dev/null | tr -d '\r')" = "$(printf '0\nplain')" ]; then echo "ok   program without revocation unchanged"; else echo "FAIL program without revocation"; fail=1; fi
if [ $fail -ne 0 ]; then
    echo "--- stdout ---"; cat "$OUT"
    echo "--- stderr ---"; cat "$ERR"
fi
rm -f "$OUT" "$ERR" "$P"
exit $fail
