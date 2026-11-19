#!/usr/bin/env bash
# OP28 (competitive roadmap): comptime evaluation of whole tables and
# parsers. tests/bootstrap/test_comptime_table.fd builds a CRC table and
# tests/bootstrap/test_comptime_parser.fd parses constant text (core string
# functions and plain helper functions inside a comptime fn); in both, the
# compiler computes the values, `main` only builds the result from them (no
# call to the builder or the parser is left in it), nothing falls back to
# run time ("warning: comptime" absent) and the output is the expected one.
#
# usage: tests/tooling/op28-comptime-parser.sh [fid]   (default:
# release/fid.exe, run from the repository root)
set -u
FID="${1:-release/fid.exe}"
fail=0
tmp="${TMPDIR:-/tmp}/op28.$$"
mkdir -p "$tmp"
check() { # check <label> <text> <fixed string> <want: 1 present, 0 absent>
    if printf '%s\n' "$2" | grep -qF -- "$3"; then got=1; else got=0; fi
    if [ "$got" = "$4" ]; then
        echo "ok    $1"
    else
        echo "FAIL  $1"
        printf '%s\n' "$2" | sed 's/^/      /' | head -20
        fail=1
    fi
}
main_of() { # main_of <c file>: the body of main_entry
    awk '/^int64_t main_entry\(void\* v0\) \{/{p=1} p{print} p&&/^}/{exit}' "$1"
}
expected() { # the "// Expected output:" block of a test
    awk '/^\/\/ Expected output:/{p=1; next} p&&/^\/\/ /{sub(/^\/\/ /,""); print; next} p{exit}' "$1"
}
for t in test_comptime_table test_comptime_parser; do
    src="tests/bootstrap/$t.fd"
    out=$("$FID" run "$src" 2>"$tmp/$t.err" | grep -v '^\[')
    err=$(cat "$tmp/$t.err")
    want=$(expected "$src")
    if [ "$out" = "$want" ]; then echo "ok    $t: output"; else
        echo "FAIL  $t: output"; printf 'got:\n%s\nwant:\n%s\n' "$out" "$want" | sed 's/^/      /'; fail=1; fi
    check "$t: nothing left to run time" "$err" "warning: comptime" 0
    "$FID" dump-c "$src" >"$tmp/$t.c" 2>/dev/null
    body=$(main_of "$tmp/$t.c")
    check "$t: main_entry found" "$body" "main_entry" 1
done
tbody=$(main_of "$tmp/test_comptime_table.c")
check "table: main calls no crc_table" "$tbody" "crc_table(" 0
check "table: the entry 1996959894 is in main" "$tbody" "1996959894" 1
pbody=$(main_of "$tmp/test_comptime_parser.c")
check "parser: main calls no parse_nums" "$pbody" "parse_nums((" 0
check "parser: main calls no config_keys" "$pbody" "config_keys((" 0
check "parser: main calls no config_values" "$pbody" "config_values((" 0
check "parser: main still calls the run-time parser" "$pbody" "parse_nums_rt((" 1
check "parser: the parsed 300 is in main" "$pbody" "300LL" 1
rm -rf "$tmp"
exit $fail
