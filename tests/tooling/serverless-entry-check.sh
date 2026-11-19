#!/usr/bin/env bash
# Roadmap Phase 9 (serverless, F32), the cold-start side:
# `fid emit|build|ccompile <file> --entry <handler>` makes a handler the
# entry point (compiler/main.fd entry_wrap_source, entry_source_for): the
# program's main is set aside, a generated main runs one request (argv[1],
# else one line of stdin) through the handler, and `emit` lowers only what
# compiler/reach.fd finds from it.
#   FID=release/fid.exe bash tests/tooling/serverless-entry-check.sh
# Run from the repository root. Exit 0 when every check passes. The C part
# runs when a C compiler is there (CC, zig, tcc, gcc or cc).
set -u
FID=${FID:-release/fid.exe}
SRC=tests/bootstrap/test_serverless_entry.fd
export MSYS_NO_PATHCONV=1
T=$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/fid_entry_$$")
mkdir -p "$T"
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# 1. without --entry the program is unchanged
want_whole=$(printf '200 /hello 11\n404 /missing 0\nreport: 2 requests')
got=$("$FID" run "$SRC" 2>/dev/null | tr -d '\r')
[ "$got" = "$want_whole" ] && ok "fid run: the whole program" || bad "fid run: got '$got'"

# 2. the handler as the entry point: request from argv[1], else stdin
if "$FID" emit "$SRC" --entry handle_request -o "$T/h$X" >"$T/emit.log" 2>&1; then
    got=$("$T/h$X" /hello | tr -d '\r')
    [ "$got" = "200 /hello 11" ] && ok "emit --entry: argv[1] request" || bad "emit --entry: argv[1] request: '$got'"
    got=$(echo /missing | "$T/h$X" | tr -d '\r')
    [ "$got" = "404 /missing 0" ] && ok "emit --entry: stdin request" || bad "emit --entry: stdin request: '$got'"
    grep -qF "Entry point: handle_request" "$T/emit.log" && ok "emit --entry: says the entry point" || bad "emit --entry: no 'Entry point' line"
    if grep -qF "not marked @serverless" "$T/emit.log"; then bad "emit --entry: @serverless handler reported as unmarked"; else ok "emit --entry: @serverless handler, no note"; fi
else
    cat "$T/emit.log"
    bad "emit --entry handle_request"
fi

# 3. only what the handler reaches is lowered (x86-64 Linux listing on any host)
"$FID" disasm "$SRC" --target x86_64-linux --entry handle_request >"$T/entry.s" 2>/dev/null
"$FID" disasm "$SRC" --target x86_64-linux >"$T/whole.s" 2>/dev/null
grep -q "<main_only_report>:" "$T/whole.s" && ok "whole program has main_only_report" || bad "whole program: no <main_only_report> in the listing"
grep -q "<handle_request>:" "$T/entry.s" && grep -q "<route>:" "$T/entry.s" && ok "--entry keeps handle_request and route" || bad "--entry: handle_request or route missing"
if grep -q "<main_only_report>:" "$T/entry.s" || grep -q "<fid_entry_program_main>:" "$T/entry.s"; then
    bad "--entry: main's own code was lowered"
else
    ok "--entry: main and main_only_report not lowered"
fi

# 4. errors
if "$FID" emit "$SRC" --entry no_such_handler -o "$T/x$X" >"$T/e1.log" 2>&1; then
    bad "--entry no_such_handler accepted"
else
    grep -qF "has no top-level function" "$T/e1.log" && ok "unknown handler refused" || bad "unknown handler: $(cat "$T/e1.log" | tail -1)"
fi
if "$FID" emit "$SRC" --entry main -o "$T/x$X" >"$T/e2.log" 2>&1; then
    bad "--entry main accepted"
else
    ok "--entry main refused"
fi
cat >"$T/two.fd" <<'EOF'
fn add(a: i64, b: i64) -> i64 {
    return a + b;
}
fn ping() -> i64 {
    return 7;
}
fn main() {
    println(int_to_str(add(1, 2)));
}
EOF
if "$FID" emit "$T/two.fd" --entry add -o "$T/x$X" >"$T/e3.log" 2>&1; then
    bad "--entry with two parameters accepted"
else
    grep -qF "2 parameters" "$T/e3.log" && ok "two-parameter handler refused" || bad "two-parameter handler: $(tail -1 "$T/e3.log")"
fi
if "$FID" emit "$T/two.fd" --entry ping -o "$T/p$X" >"$T/e4.log" 2>&1; then
    got=$("$T/p$X" | tr -d '\r')
    [ "$got" = "7" ] && ok "handler without a request, i64 result" || bad "ping: '$got'"
    grep -qF "not marked @serverless" "$T/e4.log" && ok "unmarked handler noted" || bad "unmarked handler: no note"
else
    tail -3 "$T/e4.log"
    bad "--entry ping"
fi

# 5. the same handler through C
have_cc=0
if [ -n "${CC:-}" ]; then have_cc=1; fi
for c in zig tcc gcc cc; do command -v $c >/dev/null 2>&1 && have_cc=1; done
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
if [ $have_cc -eq 1 ]; then
    if "$FID" ccompile "$SRC" --entry handle_request -o "$T/hc$X" >"$T/c.log" 2>&1; then
        got=$("$T/hc$X" /hello | tr -d '\r')
        [ "$got" = "200 /hello 11" ] && ok "ccompile --entry" || bad "ccompile --entry: '$got'"
    else
        tail -3 "$T/c.log"
        bad "ccompile --entry"
    fi
else
    echo "skip ccompile --entry: no C compiler"
fi
rm -rf "$T"
exit $fail
