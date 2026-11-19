#!/bin/bash
# wasm-c-2g61-check.sh -- known bug 2g.61: `fid ccompile --target
# wasm32-wasi` failed for every program: the C runtime's p_co_connect asks
# getsockopt(SOL_SOCKET, SO_ERROR) and wasi-libc declares no SO_ERROR
# ("use of undeclared identifier 'SO_ERROR'"). codegen_c.fd now defines it
# in the runtime's WASI block (where socket calls fail with ENOSYS).
#
# Checks:
#   1. the C of hello.fd defines SO_ERROR before its first use, inside the
#      `#ifdef __wasi__` block (no C compiler needed)
#   2. with zig: hello.fd, test_cwasm_libc_names.fd and
#      test_c_header_type_names.fd build for wasm32-wasi, and with node
#      they print what `fid run` prints and exit the same way
#
# Usage: tests/tooling/wasm-c-2g61-check.sh [fid]
#   default: release/fid-dev(.exe), else release/fid(.exe)
#   CC defaults to "zig cc"; without zig (or node) part 2 is skipped and
#   reported as such (exit 0 when part 1 passes).
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev.exe release/fid-dev release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "wasm-c-2g61: no compiler (give its path)"; exit 1; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
W="$ROOT/build/wasm-c-2g61"
rm -rf "$W"
mkdir -p "$W"
fail=0

# 1. SO_ERROR is defined in the WASI block, before p_co_connect uses it
"$FID" dump-c tests/bootstrap/hello.fd > "$W/hello.c" 2> "$W/dump.err"
if [ ! -s "$W/hello.c" ]; then
  echo "FAIL dump-c tests/bootstrap/hello.fd (see $W/dump.err)"
  exit 1
fi
awk '
  /^#ifdef __wasi__/ { inw = 1; depth = 0; next }
  inw && /^#if/ { depth++ }
  inw && /^#endif/ { if (depth == 0) inw = 0; else depth-- }
  inw && /^#define SO_ERROR / { def = NR }
  /SO_ERROR/ && !/^#/ && !use { use = NR }
  END {
    if (!def) { print "FAIL no #define SO_ERROR in the C runtime'"'"'s __wasi__ block"; exit 1 }
    if (use && use < def) { print "FAIL SO_ERROR used (line " use ") before it is defined (line " def ")"; exit 1 }
    print "ok   SO_ERROR defined for WASI (line " def "), first used at line " use
  }' "$W/hello.c" || fail=$((fail + 1))

# 2. Built for wasm32-wasi with zig and run under node
CC="${CC:-zig cc}"
if ! command -v zig > /dev/null 2>&1 || ! command -v node > /dev/null 2>&1; then
  echo "skip wasm32-wasi build and run (needs zig and node)"
  echo "wasm-c-2g61: $fail failed"
  exit $fail
fi
for t in hello test_cwasm_libc_names test_c_header_type_names; do
  src="tests/bootstrap/$t.fd"
  "$FID" run "$src" > "$W/$t.ref" 2> /dev/null
  ref_st=$?
  if ! CC="$CC" "$FID" ccompile "$src" --target wasm32-wasi -o "$W/$t.wasm" > "$W/$t.build" 2>&1; then
    echo "FAIL $t: ccompile --target wasm32-wasi"
    grep -a -m 5 "error" "$W/$t.build" | sed 's/^/     /'
    fail=$((fail + 1))
    continue
  fi
  node scripts/wasm-run.mjs "$W/$t.wasm" > "$W/$t.got" 2> "$W/$t.err"
  got_st=$?
  if ! cmp -s "$W/$t.ref" "$W/$t.got"; then
    echo "FAIL $t: output differs from fid run"
    diff "$W/$t.ref" "$W/$t.got" | head -10 | sed 's/^/     /'
    fail=$((fail + 1))
  elif [ "$ref_st" != "$got_st" ]; then
    echo "FAIL $t: exit $got_st, fid run exits $ref_st"
    fail=$((fail + 1))
  else
    echo "ok   $t: built for wasm32-wasi, same output and exit ($got_st) as fid run"
  fi
done
echo "wasm-c-2g61: $fail failed"
exit $fail
