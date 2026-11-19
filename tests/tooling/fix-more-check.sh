#!/bin/bash
# CE5 (docs/roadmap/complaint-roadmap.md): `fid fix` applies more of the
# `Fix:` lines (compiler/lsp.fd fix_for_more): an unused variable or
# parameter named _x, a group of use lines sorted, a borrowed string
# returned or passed as an owned one copied with str_clone, and a missing
# annotation (`Fix: add @A to 'f'`) added. Fixtures: tests/projects/ce5_fix.
# Each is copied to build/ce5-fix/, `fid fix --check` must report a fix
# (status 1) and leave the file alone, `fid fix` must write the expected
# text, and the result must check (and lint) clean.
#   FID=release/fid.exe bash tests/tooling/fix-more-check.sh
# Run from the repository root. Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
SRC=tests/projects/ce5_fix
W=build/ce5-fix
mkdir -p "$W"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
# one fixture: name, the text the fixed file must hold, lint (1) or check (0)
one() {
    local name=$1 want=$2 lint=$3
    cp "$SRC/$name.fd" "$W/$name.fd"
    local out code
    out=$("$FID" fix "$W/$name.fd" --check 2>/dev/null | tr -d '\r'; exit ${PIPESTATUS[0]})
    code=$?
    if [ $code = 1 ] && echo "$out" | grep -q "^fixed  "; then ok "$name: fix --check reports the fix"; else bad "$name: fix --check (status $code): $out"; fi
    if cmp -s "$SRC/$name.fd" "$W/$name.fd"; then ok "$name: --check leaves the file"; else bad "$name: --check wrote the file"; fi
    "$FID" fix "$W/$name.fd" > /dev/null 2>&1
    if grep -qF -- "$want" "$W/$name.fd"; then ok "$name: fixed text"; else bad "$name: no line with: $want"; fi
    "$FID" check "$W/$name.fd" > /dev/null 2>&1
    if [ $? = 0 ]; then ok "$name: checks clean after the fix"; else bad "$name: errors remain after the fix"; fi
    if [ "$lint" = 1 ]; then
        if "$FID" lint "$W/$name.fd" 2>/dev/null | tr -d '\r' | grep -q "^No lint warnings"; then ok "$name: lints clean after the fix"; else bad "$name: lint warnings remain"; fi
    fi
}
one unused_var "var _leftover: i64 = 5;" 1
one unused_param "fn show(label: str, _extra: i64) {" 1
one use_order "use hash;" 1
one borrowed_return "return str_clone(m.text);" 0
one borrowed_pass "remember(1, str_clone(m.text))" 0
one add_audit "@audit" 0
# the sorted group: hash now comes first
if [ "$(grep -n '^use ' "$W/use_order.fd" | head -1 | cut -d: -f2)" = "use hash;" ]; then ok "use_order: hash first"; else bad "use_order: not sorted"; fi
# CE5, Nov 17: renames to snake_case with every use, fields a `S { .., .. }`
# literal leaves out, a by-value parameter of a method, a borrow conflict
# reported without a line
one snake_var "    var total_count: i64 = 0;" 1
one snake_param "fn clip(s: str, max_len: i64) -> str {" 1
one snake_fn "fn add_two(n: i64) -> i64 {" 1
one missing_field "Item { name: \"pen\", sold: false, count: 0, tags: Vec::new(), .. }" 0
one byvalue_method "        var extra = extra;" 0
one for_iter_clone "    for n in names.clone() {" 0
# what else the fixed files must (not) hold
has() { if grep -qF -- "$2" "$W/$1.fd"; then ok "$1: has $2"; else bad "$1: no line with: $2"; fi; }
hasnt() { if grep -qF -- "$2" "$W/$1.fd"; then bad "$1: still has $2"; else ok "$1: no $2"; fi; }
has snake_var "        total_count = total_count + i;"
has snake_var "Stats { totalCount: total_count }"
has snake_var "int_to_str(s.totalCount)"
has snake_var "println(\"totalCount \""
has snake_var "// totalCount stays in this comment"
has snake_param "return str_slice(s, 0, max_len);"
has snake_param "var max_len_other: i64 = 4;"
has snake_fn "val a: i64 = add_two(1);"
has snake_fn "println(\"addTwo \" + int_to_str(add_two(a) + 2));"
hasnt snake_fn "addTwo("
has missing_field "        count: 0,"
has missing_field "        tags: Vec::new(),"
hasnt missing_field "note: \"\""
# the fixed programs print what their headers say
runs() {
    local got
    got=$("$FID" run "$W/$1.fd" 2>/dev/null | tr -d '\r')
    if [ "$got" = "$2" ]; then ok "$1: prints what it should"; else bad "$1: printed: $got"; fi
}
runs snake_var "totalCount 6 6"
runs snake_param "abc 4"
runs snake_fn "addTwo 7"
runs missing_field "$(printf 'pen 0 false 0 -\nink 0 true 0 -')"
runs byvalue_method "3 1"
runs for_iter_clone "4 b!"
# a rename is not made when the new name is taken: the file stays as it is
cp "$SRC/snake_taken.fd" "$W/snake_taken.fd"
"$FID" fix "$W/snake_taken.fd" > /dev/null 2>&1
if cmp -s "$SRC/snake_taken.fd" "$W/snake_taken.fd"; then ok "snake_taken: not renamed onto a name in use"; else bad "snake_taken: renamed onto a name in use"; fi
if [ $fail = 0 ]; then echo "fix-more-check: PASS"; else echo "fix-more-check: FAIL"; fi
exit $fail
