#!/usr/bin/env bash
# op20-zig-cpu-check.sh — `ccompile --cpu <name>` with CC='zig cc' builds
# (known-bugs 2g.111, OP20). zig cc rejects the GCC/clang spelling
# -march=x86-64-v3 ("unknown CPU: 'x86'"); main.fd zig_march_name gives it
# zig's own names (x86_64_v3, generic+v8_2a, generic_rv64+m+a+f+d+c).
#
# 1. x86-64 host: --cpu x86-64-v3 builds and runs hello (when the CPU has
#    AVX2/FMA, else it is only built); --cpu x86-64-v4 builds.
# 2. --target aarch64-linux --cpu armv8.2-a and --target riscv64-linux
#    --cpu rv64gc build (cross, not run).
# Skips (exit 0) when zig is not on PATH.
#
# Usage: tests/tooling/op20-zig-cpu-check.sh   (FID=<fid> to choose)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-$ROOT/release/fid.exe}"
[ -x "$FID" ] || FID="$ROOT/release/fid"
export FID_HOME="${FID_HOME:-$ROOT}"
if ! command -v zig > /dev/null 2>&1; then
  echo "SKIP op20-zig-cpu-check: no zig on PATH"
  exit 0
fi
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
cat > "$T/hello.fd" <<'EOF'
fn main() -> i64 {
    var s: f64 = 0.0;
    var i: i64 = 0;
    while i < 10 {
        s = s + 1.5;
        i = i + 1;
    }
    println("cpu ok " + int_to_str(s as i64));
    return 0;
}
EOF
fail=0
build() {
  # $1 label, rest: arguments after the file
  local label="$1"; shift
  if CC="zig cc" "$FID" ccompile "$T/hello.fd" "$@" > "$T/$label.log" 2>&1 && [ -f "$T/$label" -o -f "$T/$label.exe" ]; then
    echo "ok   $label: built ($*)"
    return 0
  fi
  echo "FAIL $label: ($*)"; tail -5 "$T/$label.log"; fail=1
  return 1
}

case "$(uname -m)" in
  x86_64|amd64|AMD64)
    if build v3 --cpu x86-64-v3 -o "$T/v3"; then
      exe="$T/v3"; [ -f "$exe.exe" ] && exe="$exe.exe"
      if grep -q -i "avx2" /proc/cpuinfo 2> /dev/null || [ "$(uname -s | cut -c1-5)" != "Linux" ]; then
        out=$("$exe" 2>&1 | tr -d '\r')
        if [ "$out" = "cpu ok 15" ]; then echo "ok   v3 runs: $out"; else echo "FAIL v3 runs: '$out'"; fail=1; fi
      fi
    fi
    build v4 --cpu x86-64-v4 -o "$T/v4" || true
    ;;
esac
build a64 --target aarch64-linux --cpu armv8.2-a -o "$T/a64" || true
build rv64 --target riscv64-linux --cpu rv64gc -o "$T/rv64" || true

[ "$fail" = 0 ] && echo "PASS op20-zig-cpu-check" || { echo "FAILED op20-zig-cpu-check"; exit 1; }
