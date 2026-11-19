#!/bin/bash
# CD3 (docs/roadmap/complaint-roadmap.md): only the functions a program
# uses end up in the binary, on the C path too. `fid emit` lowers only what
# compiler/reach.fd finds reachable; `fid ccompile` / `fid dump-c` must do
# the same (compiler.fd lower_to_ir_module_with_path, by default for the C
# path; FID_LOWER_ALL=1 or FID_C_REACH=0 lowers everything, as before).
#
# A project with a pact in ark_modules/ (as `ark install` leaves it) whose
# lib.fd has a used and an unused function, each returning a string that
# names it. The C of the program must hold the used one's string and not
# the unused one's; with FID_LOWER_ALL=1 both are there (the check sees the
# difference). With a C compiler (CC, else zig cc), the program is built
# and run as well.
#   FID=release/fid.exe bash tests/tooling/cd3-reach-c-check.sh
# Run from the repository root. Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
case "$FID" in
  /*|[A-Za-z]:*) ;;
  */*) FID="$(pwd)/$FID" ;;
esac
export FID_HOME="${FID_HOME:-$(pwd)}"
W=build/cd3-reach-c
rm -rf "$W"
mkdir -p "$W/ark_modules/cd3dep/src"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
cat > "$W/ark_modules/cd3dep/src/lib.fd" <<'EOF'
pub fn cd3_used() -> str {
    return "CD3-USED-BODY";
}

pub fn cd3_never_called() -> str {
    return "CD3-UNUSED-BODY";
}

pub fn cd3_never_called_either(n: i64) -> i64 {
    println("CD3-UNUSED-TOO");
    return n * 7;
}
EOF
printf '[package]\nname = "cd3app"\nversion = "0.1.0"\n\n[dependencies]\ncd3dep = { path = "../cd3dep" }\n' > "$W/ark.toml"
cat > "$W/main.fd" <<'EOF'
use cd3dep;

fn main() -> i64 {
    println(cd3_used());
    return 0;
}
EOF
unset FID_C_REACH FID_LOWER_ALL
(cd "$W" && "$FID" dump-c main.fd > reach.c 2> reach.err)
if [ $? = 0 ] && [ -s "$W/reach.c" ]; then ok "dump-c of a program with a pact"; else bad "dump-c failed: $(head -5 "$W/reach.err")"; fi
if grep -q "CD3-USED-BODY" "$W/reach.c"; then ok "the used pact function is in the C"; else bad "the used function's body is missing"; fi
if grep -q "CD3-UNUSED-BODY" "$W/reach.c"; then bad "the unused pact function's body is in the C (the C path lowers everything)"; else ok "the unused pact function's body is not in the C"; fi
if grep -q "CD3-UNUSED-TOO" "$W/reach.c"; then bad "the second unused function's body is in the C"; else ok "the second unused function's body is not in the C"; fi
(cd "$W" && FID_LOWER_ALL=1 "$FID" dump-c main.fd > all.c 2> all.err)
if grep -q "CD3-UNUSED-BODY" "$W/all.c"; then ok "FID_LOWER_ALL=1: everything lowered (the unused body is there)"; else bad "FID_LOWER_ALL=1 did not lower the unused function"; fi
a=$(wc -c < "$W/all.c"); r=$(wc -c < "$W/reach.c")
if [ "$r" -lt "$a" ]; then ok "the reachable C is smaller ($r < $a bytes)"; else bad "the reachable C is not smaller ($r vs $a bytes)"; fi
# built and run, when there is a C compiler
cc="${CC:-}"
if [ -z "$cc" ] && command -v zig > /dev/null 2>&1; then cc="zig cc"; fi
if [ -n "$cc" ]; then
  (cd "$W" && CC="$cc" "$FID" ccompile main.fd -o prog.exe > cc.log 2>&1)
  if [ $? = 0 ]; then
    out=$(cd "$W" && ./prog.exe | tr -d '\r')
    if [ "$out" = "CD3-USED-BODY" ]; then ok "ccompile: the program runs (CD3-USED-BODY)"; else bad "ccompile: printed [$out]"; fi
    if grep -aq "CD3-UNUSED-BODY" "$W/prog.exe"; then bad "the unused function's string is in the binary"; else ok "the unused function's string is not in the binary"; fi
  else
    bad "ccompile failed: $(tail -5 "$W/cc.log")"
  fi
else
  echo "skip ccompile: no C compiler (CC or zig)"
fi
if [ $fail = 0 ]; then echo "cd3-reach-c-check: PASS"; else echo "cd3-reach-c-check: FAIL"; fi
exit $fail
