#!/usr/bin/env bash
# build-no-toolchain-check.sh — `fid build` needs no C compiler or linker
# (docs/roadmap/complaint-roadmap.md CB4, CB5).
#
# On a Linux or macOS host, with a PATH that holds no cc/gcc/clang/zig/ld:
#   1. `fid build hello.fd` (no --target) writes a running executable for
#      this machine (the machine layer: compiler/nat_ir.fd + nat_enc.fd)
#      and prints what `fid run` prints. Before CB4 it wrote C and called
#      gcc/clang/cc, so this failed with no C compiler.
#   2. `fid build --target windows-x64 hello.fd` writes a PE (pe_ir, CB5);
#      with wine on PATH it is run and compared too (TL6: ci.yml's selfhost
#      job runs this with wine and REQUIRE_WINE=1).
#   3. `--via-c` still takes the C route (and fails here: no C compiler).
# On Windows the script only checks 2 (the native default needs nothing).
#
# Usage: tests/tooling/build-no-toolchain-check.sh   (FID=<fid> to choose)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }

# A PATH with the shell tools only: links to sh, uname and the basics
BIN="$TMP/bin"; mkdir -p "$BIN"
for tool in sh bash uname cat env sed grep head tail rm mkdir chmod cp mv ls test true false printf echo dirname basename tr wc; do
  p=$(command -v "$tool" 2>/dev/null) && ln -s "$p" "$BIN/$tool" 2>/dev/null
done
bare_path="$BIN"
for t in cc gcc clang zig ld ld.lld; do
  if PATH="$bare_path" command -v "$t" >/dev/null 2>&1; then echo "  note  $t still on the bare PATH"; fi
done

ref=$("$FID" run tests/bootstrap/hello.fd 2>/dev/null)
case "$(uname -s 2>/dev/null)" in
  Linux|Darwin) posix=1 ;;
  *) posix=0 ;;
esac
# TL6: what runs the Windows PE of step 2 on Linux/macOS: WINE=<command>,
# else wine, else wine64 (REQUIRE_WINE=1: none is a failure, as in CI)
wine_cmd="${WINE:-}"
if [ -z "$wine_cmd" ]; then
  if command -v wine >/dev/null 2>&1; then wine_cmd=wine
  elif command -v wine64 >/dev/null 2>&1; then wine_cmd=wine64; fi
fi

if [ "$posix" = 1 ]; then
  if PATH="$bare_path" "$FID" build tests/bootstrap/hello.fd -o "$TMP/hello" >"$TMP/b1.log" 2>&1 && [ -x "$TMP/hello" ]; then
    got=$("$TMP/hello" 2>&1)
    if [ "$got" = "$ref" ]; then ok "1. fid build with no C compiler on PATH: $(grep -o 'Host target: [^ ]*' "$TMP/b1.log")"
    else bad "1. the executable prints '$got', fid run prints '$ref'"; fi
  else bad "1. fid build failed without a C compiler"; tail -10 "$TMP/b1.log"; fi
  if PATH="$bare_path" "$FID" build tests/bootstrap/hello.fd --via-c -o "$TMP/hello_c" >/dev/null 2>&1; then
    echo "  note  3. --via-c built with no C compiler on PATH?"
  else ok "3. --via-c takes the C route (no C compiler here: fails as expected)"; fi
  # 4. A program the machine layer rejects goes on through C by itself
  #    (FID_NAT_REJECT=1 makes up the rejection), with the normal PATH (a C
  #    compiler); an explicit --target never falls back
  if command -v cc >/dev/null 2>&1 || command -v gcc >/dev/null 2>&1 || command -v clang >/dev/null 2>&1; then
    if FID_NAT_REJECT=1 "$FID" build tests/bootstrap/hello.fd -o "$TMP/hello_fb" >"$TMP/b4.log" 2>&1 && [ -x "$TMP/hello_fb" ] \
       && grep -q 'falling back to the C compiler' "$TMP/b4.log"; then
      got=$("$TMP/hello_fb" 2>&1)
      if [ "$got" = "$ref" ]; then ok "4. rejected by the machine layer: built through C instead"
      else bad "4. the fallback's executable prints '$got', fid run prints '$ref'"; fi
    else bad "4. no fallback to C after a rejection"; tail -10 "$TMP/b4.log"; fi
    host_t=$(grep -o 'Host target: [^ ]*' "$TMP/b4.log" | head -1 | sed 's/Host target: //')
    if [ -n "$host_t" ]; then
      if FID_NAT_REJECT=1 "$FID" build tests/bootstrap/hello.fd --target "$host_t" -o "$TMP/hello_fb2" >"$TMP/b5.log" 2>&1; then
        if grep -q 'falling back' "$TMP/b5.log"; then bad "4. --target $host_t fell back to C"
        else ok "4. --target $host_t: FID_NAT_REJECT applies only to the host default (built natively)"; fi
      else bad "4. --target $host_t failed"; tail -5 "$TMP/b5.log"; fi
    fi
  else echo "  note  4. no C compiler on PATH: the fallback is not checked"; fi
fi

if PATH="$bare_path" "$FID" build tests/bootstrap/hello.fd --target windows-x64 -o "$TMP/hello_win.exe" >"$TMP/b2.log" 2>&1 \
   && [ "$(head -c 2 "$TMP/hello_win.exe")" = "MZ" ]; then
  if [ "$posix" = 0 ]; then
    got=$("$TMP/hello_win.exe" 2>&1 | tr -d '\r')
    if [ "$got" = "$ref" ]; then ok "2. --target windows-x64 runs"; else bad "2. windows-x64 prints '$got'"; fi
  elif [ -n "$wine_cmd" ]; then
    # TL6: wine quiet; its prefix is made on the first run
    got=$(WINEDEBUG="${WINEDEBUG:--all}" "$wine_cmd" "$TMP/hello_win.exe" 2>/dev/null | tr -d '\r')
    if [ "$got" = "$ref" ]; then ok "2. --target windows-x64 from $(uname -s) runs under $wine_cmd"; else bad "2. under $wine_cmd: '$got'"; fi
  elif [ "${REQUIRE_WINE:-}" = 1 ]; then
    bad "2. --target windows-x64 from $(uname -s) wrote a PE, but no wine to run it (REQUIRE_WINE=1)"
  else ok "2. --target windows-x64 from $(uname -s) wrote a PE (no wine to run it)"; fi
else bad "2. --target windows-x64 build failed"; tail -10 "$TMP/b2.log"; fi

if [ "$fail" = 0 ]; then echo "build-no-toolchain-check: PASS"; else echo "build-no-toolchain-check: FAIL"; fi
exit "$fail"
