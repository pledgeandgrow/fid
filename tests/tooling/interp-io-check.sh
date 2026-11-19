#!/bin/bash
# Known bugs 2g.59 and 2g.60 (docs/reference/known-bugs.md): a program's
# command line and standard input are the same under `fid run` as in a
# built program.
# - 2g.60: under `fid run args.fd a b`, arg_at(1) is "a" and args_len() 3,
#   as for `args.exe a b` (compiler/interp.fd interp_args_detect; the
#   interpreter saw fid's own command line, the first argument at 3).
# - 2g.59: read_line() gives each line without "\n" / "\r\n" and None at
#   the end of input: under `fid run` (interp.fd interp_read_input), through
#   C, and natively (`fid emit`: pe_ir on Windows, stdlib/pe_runtime/base.fd
#   pert_read_line, which returned "" at the end, so read_line never ended).
#   read_stdin(n) likewise (chunks.fd; pert_read_stdin).
# - natively, arg_at past the last argument is None (compiler/pe_ir_stubs.fd
#   p_argv returned the argument count as a pointer when there were
#   arguments: a crash).
#   FID=release/fid.exe bash tests/tooling/interp-io-check.sh
#   SKIP_NATIVE=1: leave out the `fid emit` build. CC: the C compiler for
#   ccompile (default "zig cc").
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
export CC=${CC:-zig cc}
P=tests/projects/interp_io
D=$(mktemp -d ./_interp_io.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
EXE=""
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;;
esac

ARGS_WANT="count 3
1 a
2 b c
program named"
LINES_WANT="line [one] 3
line [] 0
line [three] 5
line [four] 4
lines 4"

# the program's own output, without fid's build messages
clean() {
    tr -d '\r' | grep -v '^\[' | grep -v '^Built native executable'
}

check() {
    # check <what> <wanted> <got>
    if [ "$2" = "$3" ]; then
        echo "ok   $1"
    else
        echo "FAIL $1"
        echo "  wanted:"; echo "$2" | sed 's/^/    /'
        echo "  got:"; echo "$3" | sed 's/^/    /'
        fail=1
    fi
}

feed() {
    printf 'one\n\nthree\r\nfour'
}

# --- fid run ---------------------------------------------------------------
got=$("$FID" run "$P/args.fd" a "b c" 2>/dev/null | clean)
check "fid run: arguments start at arg_at(1)" "$ARGS_WANT" "$got"
got=$("$FID" run "$P/args.fd" 2>/dev/null | clean)
check "fid run: no arguments" "count 1
program named" "$got"
got=$(feed | timeout 60 "$FID" run "$P/lines.fd" 2>/dev/null | clean)
check "fid run: read_line ends with None" "$LINES_WANT" "$got"
got=$(printf '' | timeout 60 "$FID" run "$P/lines.fd" 2>/dev/null | clean)
check "fid run: read_line on empty input" "lines 0" "$got"
got=$(printf 'abcdefghij' | timeout 60 "$FID" run "$P/chunks.fd" 2>/dev/null | clean)
check "fid run: read_stdin ends with None" "chunks 3 bytes 10" "$got"

# --- through C ---------------------------------------------------------------
if "$FID" ccompile "$P/args.fd" -o "$D/args_c$EXE" > /dev/null 2>&1; then
    got=$("./$D/args_c$EXE" a "b c" | clean)
    check "C: arguments" "$ARGS_WANT" "$got"
else
    echo "FAIL C: build of args.fd"; fail=1
fi
if "$FID" ccompile "$P/lines.fd" -o "$D/lines_c$EXE" > /dev/null 2>&1; then
    got=$(feed | timeout 20 "./$D/lines_c$EXE" | clean)
    check "C: read_line ends with None" "$LINES_WANT" "$got"
else
    echo "FAIL C: build of lines.fd"; fail=1
fi
if "$FID" ccompile "$P/chunks.fd" -o "$D/chunks_c$EXE" > /dev/null 2>&1; then
    got=$(printf 'abcdefghij' | timeout 20 "./$D/chunks_c$EXE" | clean)
    check "C: read_stdin ends with None" "chunks 3 bytes 10" "$got"
else
    echo "FAIL C: build of chunks.fd"; fail=1
fi

# --- native (fid emit) -------------------------------------------------------
if [ "${SKIP_NATIVE:-0}" != "1" ]; then
    if "$FID" emit "$P/args.fd" -o "$D/args_n$EXE" > /dev/null 2>&1; then
        # plain words: pe_ir splits the Windows command line at spaces
        # without quotes ("b c" is two arguments there, a separate gap).
        # arg_at past the last argument is None (pe_ir_stubs p_argv
        # returned the argument count as a pointer: a crash)
        got=$("./$D/args_n$EXE" a b 2>&1 | clean)
        check "native: arguments" "count 3
1 a
2 b
program named" "$got"
        got=$("./$D/args_n$EXE" 2>&1 | clean)
        check "native: no arguments" "count 1
program named" "$got"
    else
        echo "FAIL native: build of args.fd"; fail=1
    fi
    if "$FID" emit "$P/lines.fd" -o "$D/lines_n$EXE" > /dev/null 2>&1; then
        got=$(feed | timeout 20 "./$D/lines_n$EXE" | clean)
        check "native: read_line ends with None" "$LINES_WANT" "$got"
        got=$(printf '' | timeout 20 "./$D/lines_n$EXE" | clean)
        check "native: read_line on empty input" "lines 0" "$got"
    else
        echo "FAIL native: build of lines.fd"; fail=1
    fi
    if "$FID" emit "$P/chunks.fd" -o "$D/chunks_n$EXE" > /dev/null 2>&1; then
        got=$(printf 'abcdefghij' | timeout 20 "./$D/chunks_n$EXE" | clean)
        check "native: read_stdin ends with None" "chunks 3 bytes 10" "$got"
        got=$(printf '' | timeout 20 "./$D/chunks_n$EXE" | clean)
        check "native: read_stdin on empty input" "chunks 0 bytes 0" "$got"
    else
        echo "FAIL native: build of chunks.fd"; fail=1
    fi
fi

if [ $fail -eq 0 ]; then
    echo "interp-io-check: all passed"
fi
exit $fail
