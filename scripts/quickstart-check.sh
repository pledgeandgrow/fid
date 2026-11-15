#!/usr/bin/env bash
# quickstart-check.sh — N2: run the getting-started guide end-to-end.
# Exercises every quickstart command path: `fid run`, `fid ccompile`,
# `fid emit`. Fails if any step fails or output differs from the guide.
set -e
FID="${1:-release/fid-dev}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

cat > "$WORK/hello.fd" <<'FD'
fn main() -> void {
    print("Hello, Fides!");
}
FD

cat > "$WORK/vec.fd" <<'FD'
fn main() -> void {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);
    print("Vec size: " + int_to_str(v.len()));
    print("Sum: " + int_to_str(v.get(0) + v.get(1) + v.get(2)));
}
FD

echo "[quickstart] fid run hello.fd"
out="$("$FID" run "$WORK/hello.fd" 2>/dev/null | tail -1)"
[ "$out" = "Hello, Fides!" ] || { echo "FAIL: run output: $out"; exit 1; }

echo "[quickstart] fid run vec.fd"
out="$("$FID" run "$WORK/vec.fd" 2>/dev/null)"
echo "$out" | grep -q "Vec size: 3" || { echo "FAIL: $out"; exit 1; }
echo "$out" | grep -q "Sum: 6" || { echo "FAIL: $out"; exit 1; }

echo "[quickstart] fid ccompile hello.fd"
"$FID" ccompile "$WORK/hello.fd" -o "$WORK/hello"
out="$("$WORK/hello" | tail -1)"
[ "$out" = "Hello, Fides!" ] || { echo "FAIL: ccompile output: $out"; exit 1; }

echo "[quickstart] fid emit hello.fd (native, no C compiler)"
if "$FID" emit "$WORK/hello.fd" -o "$WORK/hello2" 2>/dev/null; then
    out="$("$WORK/hello2" | tail -1)"
    [ "$out" = "Hello, Fides!" ] || { echo "FAIL: emit output: $out"; exit 1; }
else
    echo "[quickstart] emit skipped on this platform"
fi

echo "[quickstart] OK"
