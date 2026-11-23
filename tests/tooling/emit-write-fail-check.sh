#!/bin/bash
# BG52 (docs/roadmap/bugs-roadmap.md): `fid emit` and `fid build` fail when
# the output cannot be written (here: -o into a directory that does not
# exist), with an error naming the file, instead of printing that they
# built it (compiler/main.fd write_output_vec).
#   1. `fid emit x.fd -o <missing dir>/x.exe`: exit status not 0, the
#      message "could not write", and no "Built native executable";
#   2. the same with `fid build`;
#   3. a writable -o still builds (exit 0, the file is there).
#   FID=release/fid.exe bash tests/tooling/emit-write-fail-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/hello.fd" <<'EOF2'
fn main() -> i64 {
    println("hello");
    return 0;
}
EOF2
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
for cmd in emit build; do
    out=$("$FID" $cmd "$TMP/hello.fd" -o "$TMP/no/such/dir/hello.exe" --no-cache 2>&1)
    rc=$?
    if [ $rc -ne 0 ] && echo "$out" | grep -q "could not write" && ! echo "$out" | grep -q "Built native executable"; then
        ok "$cmd: an output that cannot be written is an error (status $rc)"
    else
        bad "$cmd: status $rc: $out"
    fi
done
"$FID" emit "$TMP/hello.fd" -o "$TMP/hello.exe" --no-cache > "$TMP/ok.out" 2>&1
rc=$?
if [ $rc -eq 0 ] && [ -s "$TMP/hello.exe" ]; then ok "a writable output still builds"; else bad "a writable output: status $rc: $(cat "$TMP/ok.out")"; fi
if [ $fail = 0 ]; then echo "emit-write-fail-check: PASS"; else echo "emit-write-fail-check: FAIL"; fi
exit $fail
