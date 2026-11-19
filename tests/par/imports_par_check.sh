#!/usr/bin/env bash
# SP9: the import closure lexed ahead in FID_PAR threads (compiler.fd
# imp_prefetch_lex, compiler/lexer.fd lex_texts_par) gives the sequential
# output byte for byte, and how long the import phase takes either way.
#
#   FID=release/fid.exe bash tests/par/imports_par_check.sh
#   FID=release/fid.exe SELF=1 bash tests/par/imports_par_check.sh  # + compiler/main.fd
#
# 1. tests/selfhost/test_lex_par.fd built with ccompile prints the output in
#    its header (lexing in 1, 2 and 4 threads gives the plain lexer's tokens).
# 2. `fid check` (import cache off: FID_IMPCACHE=off) of programs with many
#    imports gives the same messages with FID_PAR unset, 2, 4 and 8 (timing
#    lines dropped), and with --time and FID_PAR=4 prints the
#    "imports: lexed ahead" row with the module graph.
# 3. A module with a lex error is reported the same way with and without it.
# 4. SELF=1: `check compiler/main.fd --time` with FID_PAR unset and 4: the
#    same messages, and the imports rows of both.
# Passes when it prints "imports_par_check: ok".
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-$(mktemp -d)}"
fail=0
export FID_HOME="${FID_HOME:-$(pwd)}"
export FID_IMPCACHE=off

norm() {
    grep -v -E '^\[prof\]|[0-9]+ ?ms\b|^\[time\]' "$1" || true
}

# 1. the lexing pieces
if "$FID" ccompile tests/selfhost/test_lex_par.fd -o "$OUT/lexpar.exe" > "$OUT/lexpar.build" 2>&1; then
    "$OUT/lexpar.exe" > "$OUT/lexpar.out" 2>&1
    sed -n '/^\/\/ Expected output:/,/^$/p' tests/selfhost/test_lex_par.fd | sed '1d;$d' | sed 's|^// ||' > "$OUT/lexpar.want"
    if ! diff "$OUT/lexpar.want" "$OUT/lexpar.out" > "$OUT/lexpar.diff"; then
        echo "FAIL: test_lex_par output (see $OUT/lexpar.diff)"; fail=1
    else
        echo "test_lex_par: as expected"
    fi
else
    echo "FAIL: test_lex_par does not build (see $OUT/lexpar.build)"; fail=1
fi

# 2. programs with imports
check_same() {
    local name="$1" f="$2"
    "$FID" check "$f" > "$OUT/$name.seq" 2>&1
    local n
    for n in 2 4 8; do
        FID_PAR=$n "$FID" check "$f" > "$OUT/$name.par$n" 2>&1
        if ! diff <(norm "$OUT/$name.seq") <(norm "$OUT/$name.par$n") > "$OUT/$name.diff$n"; then
            echo "FAIL: $name: FID_PAR=$n output differs (see $OUT/$name.diff$n)"
            fail=1
        fi
    done
    echo "$name: the same with FID_PAR=2/4/8"
}
check_same ark compiler/ark.fd
check_same par_escape tests/selfhost/test_par_escape.fd
FID_PAR=4 "$FID" check compiler/ark.fd --time > "$OUT/ark.time" 2>&1
if ! grep -q 'imports: lexed ahead, [0-9]* modules, [0-9]* levels, widest' "$OUT/ark.time"; then
    echo "FAIL: no 'imports: lexed ahead' row with FID_PAR=4 --time (see $OUT/ark.time)"; fail=1
else
    grep 'imports' "$OUT/ark.time"
fi

# 3. a lex error in an imported module: the same message either way
mkdir -p "$OUT/lexerr"
printf 'module badlex;\npub fn f() -> str { return "open\n' > "$OUT/lexerr/badlex.fd"
printf 'use badlex;\nfn main() -> i64 { return 0; }\n' > "$OUT/lexerr/main.fd"
"$FID" check "$OUT/lexerr/main.fd" > "$OUT/lexerr.seq" 2>&1
FID_PAR=4 "$FID" check "$OUT/lexerr/main.fd" > "$OUT/lexerr.par" 2>&1
if ! diff <(norm "$OUT/lexerr.seq") <(norm "$OUT/lexerr.par") > "$OUT/lexerr.diff"; then
    echo "FAIL: lex error reported differently with FID_PAR=4 (see $OUT/lexerr.diff)"; fail=1
else
    echo "lex error: the same message with FID_PAR=4"
fi

# 4. the compiler itself
if [ "${SELF:-0}" = "1" ]; then
    "$FID" check compiler/main.fd --time > "$OUT/self.seq" 2>&1
    FID_PAR=4 "$FID" check compiler/main.fd --time > "$OUT/self.par" 2>&1
    if ! diff <(norm "$OUT/self.seq") <(norm "$OUT/self.par") > "$OUT/self.diff"; then
        echo "FAIL: compiler/main.fd: FID_PAR=4 output differs (see $OUT/self.diff)"; fail=1
    fi
    echo "sequential:"; grep -E '^\[time\] (  )?imports' "$OUT/self.seq"
    echo "FID_PAR=4:";  grep -E '^\[time\] (  )?imports' "$OUT/self.par"
fi

if [ $fail -eq 0 ]; then
    echo "imports_par_check: ok"
else
    echo "imports_par_check: FAILED (outputs in $OUT)"
    exit 1
fi
