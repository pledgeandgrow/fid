#!/bin/bash
# L7G (docs/roadmap/roadmap.md, REPL improvements): `fid repl` keeps one-line
# definitions and inspects them and expressions without running anything:
# :type, :ast, :ir, :effects, :caps, :opt (MI18: it replaced :pir, which
# now only says it was removed) (compiler/main.fd cmd_repl,
# compiler/compiler.fd repl_*). No C compiler is needed: no line is run.
#   FID=release/fid.exe bash tests/tooling/repl-inspect-check.sh
# Run from the repository root. Exit 0 when every expected line is there.
FID=${FID:-release/fid.exe}
# results go to stdout (OUT); prompts and errors to stderr (ERR)
OUT=$(mktemp)
ERR=$(mktemp)
fail=0
printf '%s\n' \
    'fn greet(n: i64) -> i64 { println("hi"); return n + 1; }' \
    'fn add(a: i64, b: i64) -> i64 { return a + b; }' \
    'fn load() -> i64 { var s: i64 = p_file_read("x.txt"); return s; }' \
    'fn uses_load() -> i64 { return load(); }' \
    'fn sq(n: i64) -> i64 { var r: i64 = n * n; return r; }' \
    'struct Pt { x: i64 }' \
    'impl Pt { fn show(self: Pt) -> i64 { println("pt"); return self.x; } }' \
    ':effects sq' \
    ':effects Pt::show' \
    ':type add(1, 2)' \
    ':type 1 + 2' \
    ':type 70000 * 2' \
    ':type -5' \
    ':type 1 < 2' \
    ':type 1.5' \
    ':ast greet(1) + 2' \
    ':ir add' \
    ':ir greet(1)' \
    ':effects greet' \
    ':effects add' \
    ':caps greet' \
    ':caps add' \
    ':caps load' \
    ':caps uses_load' \
    ':opt add' \
    ':opt greet' \
    ':pir add' \
    ':effects nosuch' \
    ':defs' \
    ':quit' | "$FID" repl > "$OUT" 2> "$ERR"
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
hasn() {
    if [ "$(grep -cxF -- "$2" "$OUT")" -ge "$3" ]; then echo "ok   $1"; else echo "FAIL $1: fewer than $3 lines: $2"; fail=1; fi
}
hasx() {
    hasn "$1" "$2" 1
}
hasn "seven definitions kept" "defined" 7
has ":type sees a session function" "add(1, 2) : i64"
# integer literals are i64, as in a `val` without a type (not i8 / i32)
hasx ":type of literals added" "1 + 2 : i64"
hasx ":type of a literal product above i16" "70000 * 2 : i64"
hasx ":type of a negative literal" "-5 : i64"
hasx ":type of a literal comparison" "1 < 2 : bool"
hasx ":type of a float literal" "1.5 : f64"
# :ast prints the parse tree, two spaces per level
has ":ast root" "Binary +"
has ":ast call" "  Call greet"
has ":ast argument" "    Int 1"
has ":ast right operand" "  Int 2"
# :ir of a session function, then of the main that prints an expression
has ":ir add header" "fn add("
has ":ir add body" "add %"
has ":ir expression main" "fn main("
has ":ir expression call" "call greet("
# :effects and :caps of greet both start "greet : IO"; :caps is that alone
if [ "$(grep -c '^greet : IO' "$OUT")" -ge 2 ]; then echo "ok   :effects of a printing function"; else echo "FAIL :effects of a printing function: no IO"; fail=1; fi
hasx ":effects of a pure function" "add : pure"
# the language's effects (type checker), not the IR's: a local is no
# Alloc/Write/Read effect; a method is named Type::method
hasx ":effects of a pure function with a local" "sq : pure"
hasx ":effects of a printing method" "Pt::show : IO"
hasx ":caps of a printing function" "greet : IO"
hasx ":caps of a pure function" "add : none"
hasx ":caps of a file read" "load : FS"
hasx ":caps through a session function" "uses_load : FS"
has ":opt header of a pure function" "; --- function add after iropt (effects: pure) ---"
has ":opt header names the effects" "; --- function greet after iropt (effects: IO"
if [ "$(grep -c '^fn add(' "$OUT")" -ge 2 ]; then echo "ok   :opt prints the function's IR (as :ir does)"; else echo "FAIL :opt prints the function's IR: fewer than two 'fn add(' lines"; fail=1; fi
if grep -qF ":pir was removed with the PIR path" "$ERR"; then echo "ok   :pir says it was removed"; else echo "FAIL :pir says it was removed"; fail=1; fi
if grep -qF "no function 'nosuch'" "$ERR"; then echo "ok   :effects of an unknown function"; else echo "FAIL :effects of an unknown function: no error"; fail=1; fi
has ":defs lists the definitions" "fn uses_load() -> i64 { return load(); }"
if [ $fail -ne 0 ]; then
    echo "--- repl output ---"
    cat "$OUT"
    echo "--- repl stderr ---"
    cat "$ERR"
fi
rm -f "$OUT" "$ERR"
exit $fail
