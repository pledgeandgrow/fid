#!/bin/bash
# BG54, BG36 (docs/roadmap/bugs-roadmap.md): paths in the command lines fid
# runs are quoted (compiler/main.fd shell_word), and a binary taken from
# the artifact store is executable.
#   1. `fid ccompile` of a program in a directory with a space, the output
#      in it too: the build passes and the program runs;
#   2. the same with `--inc` (each unit's compile and the link);
#   3. `fid run --leaks` there (fid runs `"<fid>" ccompile <file>` as a
#      child), twice: the second build reuses the cached binary, which must
#      still run (BG36: "Permission denied" on Linux and macOS before).
# Needs a C compiler (CC, or gcc/clang/tcc on the PATH); skipped without.
#   FID=release/fid.exe bash tests/tooling/path-spaces-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
if [ -z "$CC" ] && ! command -v gcc >/dev/null 2>&1 && ! command -v clang >/dev/null 2>&1 && ! command -v tcc >/dev/null 2>&1; then
    echo "path-spaces-check: SKIP (no C compiler)"
    exit 0
fi
BASE=$(mktemp -d)
trap 'rm -rf "$BASE"' EXIT
DIR="$BASE/with space"
mkdir -p "$DIR"
cat > "$DIR/hello.fd" <<'EOF2'
fn main() -> i64 {
    println("hello from a spaced path");
    return 0;
}
EOF2
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
EXE="$DIR/hello out.exe"
out=$("$FID" ccompile "$DIR/hello.fd" -o "$EXE" --no-cache 2>&1)
if [ -f "$EXE" ] && "$EXE" | grep -q "hello from a spaced path"; then ok "1. ccompile with spaces in the paths"; else bad "1. ccompile: $out"; fi
EXE2="$DIR/hello inc.exe"
out=$("$FID" ccompile "$DIR/hello.fd" -o "$EXE2" --inc --no-cache 2>&1)
if [ -f "$EXE2" ] && "$EXE2" | grep -q "hello from a spaced path"; then ok "2. ccompile --inc with spaces in the paths"; else bad "2. ccompile --inc: $out"; fi
cd "$DIR" || exit 1
r1=$("$FID" run "$DIR/hello.fd" --leaks 2>&1)
r2=$("$FID" run "$DIR/hello.fd" --leaks 2>&1)
if echo "$r1" | grep -q "hello from a spaced path"; then ok "3. run --leaks with a space in the path"; else bad "3. run --leaks: $r1"; fi
if echo "$r2" | grep -q "hello from a spaced path"; then ok "3. run --leaks again (the cached binary runs)"; else bad "3. run --leaks the second time: $r2"; fi
cd - > /dev/null
if [ $fail = 0 ]; then echo "path-spaces-check: PASS"; else echo "path-spaces-check: FAIL"; fi
exit $fail
