#!/bin/bash
# BG71 (docs/roadmap/bugs-roadmap.md): `fid ccompile --inc --target <t>`
# compiles and links its units for <t> (the one-unit build's flags and
# system libraries, compiler/main.fd inc_link_mode / inc_system_libs), not
# for the host: a Linux target from a Windows host got -lws2_32 and no
# -static, and its ELF was patched as a PE.
#   1. `--inc --target <other OS>` builds, and the file is that OS's format
#      (an ELF for x86_64-linux, a PE for x86_64-windows);
#   2. it is the same format as the one-unit build's.
# Needs zig (zig cc cross-compiles); skipped without.
#   FID=release/fid.exe bash tests/tooling/inc-target-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
if ! command -v zig >/dev/null 2>&1; then
    echo "inc-target-check: SKIP (no zig)"
    exit 0
fi
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/hello.fd" <<'EOF2'
fn main() -> i64 {
    println("hello");
    return 0;
}
EOF2
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*|Darwin*) other=x86_64-linux; magic='7f454c46' ;;
    *) other=x86_64-windows; magic='4d5a' ;;
esac
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
head_hex() { od -An -tx1 -N4 "$1" 2>/dev/null | tr -d ' \n'; }
out=$(env -u CC "$FID" ccompile "$TMP/hello.fd" -o "$TMP/inc.exe" --inc --no-cache --target $other 2>&1)
rc=$?
h=$(head_hex "$TMP/inc.exe")
case "$h" in
    $magic*) ok "1. --inc --target $other: status $rc, the output is a $other executable" ;;
    *) bad "1. --inc --target $other: status $rc, magic '$h': $out" ;;
esac
env -u CC "$FID" ccompile "$TMP/hello.fd" -o "$TMP/one.exe" --no-cache --target $other > /dev/null 2>&1
h1=$(head_hex "$TMP/one.exe")
if [ -n "$h" ] && [ "${h:0:4}" = "${h1:0:4}" ]; then ok "2. the same format as the one-unit build"; else bad "2. --inc '$h', one unit '$h1'"; fi
if [ $fail = 0 ]; then echo "inc-target-check: PASS"; else echo "inc-target-check: FAIL"; fi
exit $fail
