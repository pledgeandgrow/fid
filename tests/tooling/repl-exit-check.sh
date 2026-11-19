#!/bin/bash
# L7 exit criterion (docs/roadmap/roadmap.md, "REPL with type/effect/
# capability inspection"): every L7G REPL command in ONE scripted session of
# `fid repl` (compiler/main.fd cmd_repl): a multi-line definition, one-line
# definitions, :type, :ast, :ir (of a function and of an expression),
# :effects, :caps, :opt (MI18: it replaced :pir, which now only says it was
# removed), :defs, :help, an unknown command
# and :quit. Nothing here runs a line through C, so no C compiler is needed.
#   FID=release/fid.exe bash tests/tooling/repl-exit-check.sh
# Run from the repository root. Exit 0 when every expected line is there.
FID=${FID:-release/fid.exe}
OUT=$(mktemp)
ERR=$(mktemp)
fail=0
printf '%s\n' \
    'fn tri(n: i64) -> i64 {' \
    '    var s: i64 = 0;' \
    '    var i: i64 = 1;' \
    '    while i <= n {' \
    '        s = s + i;' \
    '        i = i + 1;' \
    '    }' \
    '    return s;' \
    '}' \
    'fn shout(n: i64) -> i64 { println("tri"); return tri(n); }' \
    'fn slurp() -> i64 { var s: i64 = p_file_read("in.txt"); return s; }' \
    ':type tri(4)' \
    ':type shout(2) > 3' \
    ':ast tri(3) * 2' \
    ':ir tri' \
    ':ir shout(1)' \
    ':effects tri' \
    ':effects shout' \
    ':caps tri' \
    ':caps shout' \
    ':caps slurp' \
    ':opt tri' \
    ':opt shout' \
    ':pir tri' \
    ':defs' \
    ':help' \
    ':nosuch' \
    ':quit' | "$FID" repl > "$OUT" 2> "$ERR"
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
hasx() {
    if grep -qxF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line: $2"; fail=1; fi
}
haserr() {
    if grep -qF -- "$2" "$ERR"; then echo "ok   $1"; else echo "FAIL $1: no stderr line with: $2"; fail=1; fi
}
if [ "$(grep -cxF defined "$OUT")" -eq 3 ]; then echo "ok   three definitions kept (one over nine lines)"; else echo "FAIL three definitions kept"; fail=1; fi
has ":type of a multi-line function's call" "tri(4) : i64"
has ":type of a comparison" "shout(2) > 3 : bool"
has ":ast root" "Binary *"
has ":ast call" "  Call tri"
has ":ast argument" "    Int 3"
has ":ir of a function" "fn tri("
has ":ir of an expression" "call shout("
hasx ":effects of a pure function" "tri : pure"
if grep -q '^shout : IO' "$OUT"; then echo "ok   :effects of a printing function"; else echo "FAIL :effects of a printing function"; fail=1; fi
hasx ":caps of a pure function" "tri : none"
hasx ":caps of a printing function" "shout : IO"
hasx ":caps of a file read" "slurp : FS"
has ":opt of a pure function" "; --- function tri after iropt (effects: pure) ---"
has ":opt names the effects" "; --- function shout after iropt (effects: IO"
haserr ":pir says it was removed" ":pir was removed with the PIR path"
# continuation lines are kept as typed, indentation included
has ":defs keeps the multi-line definition" "    while i <= n {"
has ":defs keeps the one-line definitions" 'fn shout(n: i64) -> i64 { println("tri"); return tri(n); }'
haserr ":help lists :effects" ":effects <fn>"
haserr ":help lists :caps" ":caps <fn>"
haserr ":help lists :opt" ":opt <fn>"
haserr "an unknown command is named" "Unknown command: :nosuch"
haserr ":quit ends the session" "Bye!"
if [ $fail -ne 0 ]; then
    echo "--- repl output ---"
    cat "$OUT"
    echo "--- repl stderr ---"
    cat "$ERR"
fi
rm -f "$OUT" "$ERR"
if [ $fail = 0 ]; then echo "repl-exit-check: PASS"; else echo "repl-exit-check: FAIL"; fi
exit $fail
