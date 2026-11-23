#!/bin/bash
# BG37 (docs/roadmap/bugs-roadmap.md): the lowered-IR cache (`emit
# --ir-cache`, FID_IRCACHE) is salted with the program's constants and
# type aliases, and with the compiler (compiler/ir_ser.fd irc_salt).
# Before, a function reading a `const` was replayed from the cache with the
# constant's old value after the constant changed.
#   1. emit a program printing a const with --ir-cache: it prints 1;
#   2. change the const (nothing else) and emit again: it prints 2;
#   3. change a type alias the body uses: the program follows it.
#   FID=release/fid.exe bash tests/tooling/ircache-const-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
unset FID_IRCACHE FIDES_NO_CACHE
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
write_prog() {
    cat > "$TMP/c.fd" <<EOF2
type Small = $2;
const K: i64 = $1;

fn show() -> i64 {
    val m: i64 = 0 - 1;
    val s: Small = m as Small;
    println(int_to_str(K) + " " + int_to_str(s as i64));
    return 0;
}

fn main() -> i64 {
    return show();
}
EOF2
}
build_run() {
    "$FID" emit "$TMP/c.fd" -o "$TMP/c.exe" --ir-cache > "$TMP/emit.out" 2>&1 || { echo "emit failed: $(tail -5 "$TMP/emit.out")"; return; }
    "$TMP/c.exe" | tr -d '\r'
}
write_prog 1 i64
got=$(build_run)
if [ "$got" = "1 -1" ]; then ok "1. first build: 1 -1"; else bad "1. first build: '$got'"; fi
write_prog 2 i64
got=$(build_run)
if [ "$got" = "2 -1" ]; then ok "2. the changed const is seen: 2 -1"; else bad "2. after the const changed: '$got' (the cached function?)"; fi
write_prog 2 u8
got=$(build_run)
if [ "$got" = "2 255" ]; then ok "3. a changed type alias is seen: 2 255"; else bad "3. after the alias changed: '$got'"; fi
if [ $fail = 0 ]; then echo "ircache-const-check: PASS"; else echo "ircache-const-check: FAIL"; fi
exit $fail
