#!/bin/bash
# TL3 (docs/roadmap/competitive-roadmap.md): `fid fix <file.fd> --edition N`
# upgrades a file to edition N (main.fd cmd_fix, compiler/lsp.fd
# fix_set_edition / fix_restore_edition and the fix engine's edition-2027
# deprecated case): it applies the fixes edition N asks for (in 2027 a call
# of a @deprecated function is an error; a call whose function names its
# replacement becomes a call of the replacement, plain or method), and
# writes `#[edition(N)]` only when the file then checks clean under N.
# Fixtures: tests/projects/tl3_edition, each copied to build/tl3-edition/.
#   upgrade.fd  2026 -> 2027: three calls replaced (one a method), edition
#               stated, checks clean under 2027, same output
#   stated.fd   `#[edition(2026)]` becomes `#[edition(2027)]` (one line)
#   plain.fd    nothing to change: the edition line alone is added
#   stuck.fd    a deprecated call without a replacement: the other call is
#               fixed, the edition is NOT stated, status 1, still runs
# `--check` must report and leave every file as it was.
#   FID=release/fid.exe bash tests/tooling/fix-edition-check.sh
# Run from the repository root. Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
SRC=tests/projects/tl3_edition
W=build/tl3-edition
mkdir -p "$W"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
# the "Expected output:" lines of a fixture's header
expected() { sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | grep '^// ' | grep -v 'Expected output:' | sed 's#^// ##'; }
runs_as_header() {
    local name=$1
    local got want
    got=$("$FID" run "$W/$name.fd" 2>/dev/null | tr -d '\r')
    want=$(expected "$SRC/$name.fd")
    if [ "$got" = "$want" ]; then ok "$name: prints what its header says"; else bad "$name: output [$got], want [$want]"; fi
}
# a fixture that reaches edition 2027: name, then lines that must be gone
reaches() {
    local name=$1; shift
    cp "$SRC/$name.fd" "$W/$name.fd"
    local out code
    out=$("$FID" fix "$W/$name.fd" --edition 2027 --check 2>/dev/null | tr -d '\r'; exit ${PIPESTATUS[0]})
    code=$?
    if [ $code = 1 ]; then ok "$name: --check reports a change (status 1)"; else bad "$name: --check status $code: $out"; fi
    if cmp -s "$SRC/$name.fd" "$W/$name.fd"; then ok "$name: --check leaves the file"; else bad "$name: --check wrote the file"; fi
    out=$("$FID" fix "$W/$name.fd" --edition 2027 2>&1 | tr -d '\r'; exit ${PIPESTATUS[0]})
    code=$?
    if [ $code = 0 ]; then ok "$name: fix --edition 2027 status 0"; else bad "$name: fix --edition 2027 status $code: $out"; fi
    if [ "$(grep -c '^#\[edition(2027)\]' "$W/$name.fd")" = 1 ]; then ok "$name: states #[edition(2027)] once"; else bad "$name: no single #[edition(2027)] line"; fi
    if grep -q '#\[edition(2026)\]' "$W/$name.fd"; then bad "$name: the 2026 line is still there"; else ok "$name: no 2026 line left"; fi
    local gone
    for gone in "$@"; do
        if grep -qF -- "$gone" "$W/$name.fd"; then bad "$name: still holds: $gone"; else ok "$name: no '$gone' left"; fi
    done
    "$FID" check "$W/$name.fd" > "$W/$name.check" 2>&1
    if [ $? = 0 ]; then ok "$name: checks clean in edition 2027"; else bad "$name: errors after the upgrade: $(head -3 "$W/$name.check")"; fi
    if grep -q "is deprecated" "$W/$name.check"; then bad "$name: a deprecated call is left"; else ok "$name: no deprecated call left"; fi
    runs_as_header "$name"
    # a second upgrade has nothing left to do
    out=$("$FID" fix "$W/$name.fd" --edition 2027 --check 2>/dev/null | tr -d '\r'; exit ${PIPESTATUS[0]})
    code=$?
    if [ $code = 0 ]; then ok "$name: a second upgrade changes nothing"; else bad "$name: second upgrade status $code: $out"; fi
}
reaches upgrade "plus(3, 4)" "times3(plus" "plus(1, 3)" ".old_value("
if grep -qF "println(triple(add(1, 3)));" "$W/upgrade.fd" && grep -qF "m.value()" "$W/upgrade.fd"; then ok "upgrade: both calls on one line and the method replaced"; else bad "upgrade: replacements missing"; fi
reaches stated "area(2"
reaches plain
# plain: the edition line is right before `fn main`
if [ "$(grep -A1 '^#\[edition(2027)\]' "$W/plain.fd" | tail -1)" = "fn main() -> i64 {" ]; then ok "plain: edition stated before the first line of code"; else bad "plain: edition line placed elsewhere"; fi
# stuck: one call cannot be fixed, so the file stays in its edition
cp "$SRC/stuck.fd" "$W/stuck.fd"
out=$("$FID" fix "$W/stuck.fd" --edition 2027 2>&1 | tr -d '\r'; exit ${PIPESTATUS[0]})
code=$?
if [ $code = 1 ]; then ok "stuck: status 1 (edition not reached)"; else bad "stuck: status $code: $out"; fi
if grep -q '#\[edition' "$W/stuck.fd"; then bad "stuck: an edition was written although errors remain under 2027"; else ok "stuck: no edition written"; fi
if grep -qF "half_of(10)" "$W/stuck.fd"; then ok "stuck: the call with a replacement is fixed"; else bad "stuck: half(10) not replaced"; fi
if grep -qF "twice(4)" "$W/stuck.fd"; then ok "stuck: the call without one is left"; else bad "stuck: twice(4) changed"; fi
if echo "$out" | grep -q "2027"; then ok "stuck: names the edition it could not reach"; else bad "stuck: no word about edition 2027: $out"; fi
"$FID" check "$W/stuck.fd" > /dev/null 2>&1
if [ $? = 0 ]; then ok "stuck: still checks clean in its own edition"; else bad "stuck: errors in 2026 after the fix"; fi
runs_as_header stuck
# an edition the compiler does not know
cp "$SRC/plain.fd" "$W/plain2030.fd"
"$FID" fix "$W/plain2030.fd" --edition 2030 > /dev/null 2>&1
code=$?
if [ $code = 1 ] && cmp -s "$SRC/plain.fd" "$W/plain2030.fd"; then ok "unknown edition: status 1, file left"; else bad "unknown edition: status $code or file changed"; fi
if [ $fail = 0 ]; then echo "fix-edition-check: PASS"; else echo "fix-edition-check: FAIL"; fi
exit $fail
