#!/bin/bash
# TL3 (docs/roadmap/competitive-roadmap.md): `fid fix` repairs two more
# diagnostics (compiler/lsp.fd fix_for_more, fix_param_copy, fix_for_clone):
#   a by-value parameter changed   -> `var p = p;` first in the body
#   a Vec changed while a `for` loop iterates over it (a borrow error)
#                                  -> `for x in v.clone() {`
# Fixtures: tests/projects/tl3_fix. Each is copied to build/tl3-fix/;
# `fid fix --check` must report the fix (status 1) and leave the file,
# `fid fix` must write the expected line, and the fixed program must check
# clean and print what its header says.
#   FID=release/fid.exe bash tests/tooling/fix-tl3-check.sh
# Run from the repository root. Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
SRC=tests/projects/tl3_fix
W=build/tl3-fix
mkdir -p "$W"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
# one fixture: name, a line the fixed file must hold, the expected output
one() {
    local name=$1 want=$2 expect=$3
    cp "$SRC/$name.fd" "$W/$name.fd"
    local out code
    out=$("$FID" fix "$W/$name.fd" --check 2>/dev/null | tr -d '\r'; exit ${PIPESTATUS[0]})
    code=$?
    if [ $code = 1 ] && echo "$out" | grep -q "^fixed  "; then ok "$name: fix --check reports the fix"; else bad "$name: fix --check (status $code): $out"; fi
    if cmp -s "$SRC/$name.fd" "$W/$name.fd"; then ok "$name: --check leaves the file"; else bad "$name: --check wrote the file"; fi
    "$FID" fix "$W/$name.fd" > "$W/$name.fix.out" 2>&1
    if grep -qF -- "$want" "$W/$name.fd"; then ok "$name: fixed text"; else bad "$name: no line with: $want"; fi
    if "$FID" check "$W/$name.fd" > /dev/null 2>&1; then ok "$name: checks clean after the fix"; else bad "$name: errors remain after the fix"; fi
    local got
    got=$("$FID" run "$W/$name.fd" 2>/dev/null | tr -d '\r')
    if [ "$got" = "$expect" ]; then ok "$name: the fixed program prints what it should"; else bad "$name: printed: $got"; fi
}
one param_copy "    var items = items;" "$(printf '3\n2')"
one for_clone "    for x in v.clone() {" "$(printf '5\n1 2 3 20 30')"
if [ $fail = 0 ]; then echo "fix-tl3-check: PASS"; else echo "fix-tl3-check: FAIL"; fi
exit $fail
