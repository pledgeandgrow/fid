#!/bin/bash
# ark-binary.sh -- L5E (roadmap.md, Registry): binary packages
# (docs/reference/ark.md, "Binary packages"):
#   ark prebuild    the lib target's exports (@export, else pub fn), only
#                   integers/floats/bool (a str function stops it), the
#                   build command (--dry-run), prebuilt/<target>/ with the
#                   library and prebuilt.toml (hashes, source, fn lines)
#   ark add --binary / binary = true, ark list
#   ark install     checks the library hash and the source hash, writes
#                   ark_modules/<pact>.fd (the binary interface: extern
#                   "C" link + a pub fn over each) and ark_modules/.binaries;
#                   --source: no interface; a stale prebuilt (source changed
#                   since) falls back to the source with a warning; a
#                   library that does not match its hash fails the install
#   ark build       copies the library beside what it builds (--dry-run),
#                   FID_LIB_PATH (-L, $ORIGIN rpath), off Windows through
#                   `fid ccompile`
#   ark test        FID_LIB_PATH, and PATH / LD_LIBRARY_PATH for the test
#                   programs (with FID: the tests call the library)
#   ark audit       the library against its hash
# The library: with FID set to a compiler that has `fid build --lib` (CI7)
# and the ark_modules/ lookup (TL9), `ark prebuild` builds it and the
# project's `ark build` links it; its program runs and prints what the
# library computes, and fails without the library. Without FID, a C
# library with the same exports (zig cc, when on PATH) or stand-in bytes
# is packaged with `ark prebuild --from`.
# Everything is local (git repositories in a temporary directory).
#
# Usage: tests/ark/ark-binary.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, ...)
#   e.g.     FID=release/fid.exe CC="zig cc" tests/ark/ark-binary.sh
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
if [ $# -gt 0 ]; then
  ARK=("$@")
else
  ARK=()
  # prefer the newest working compiler: release/fid.exe can be weeks old
  # and predate the very features these scripts check (2g.63's lesson)
  for f in release/fid-dev.exe release/fid.exe release/fid-dev release/fid; do
    if [ -x "$f" ]; then ARK=("$ROOT/$f" ark); break; fi
  done
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-binary: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac
FID="${FID:-}"
if [ -n "$FID" ]; then
  case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
  export ARK_FID="$(cygpath -m "$FID" 2>/dev/null || echo "$FID")"
  export FID_HOME="${FID_HOME:-$(cygpath -m "$ROOT" 2>/dev/null || echo "$ROOT")}"
else
  unset ARK_FID
fi

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-binary.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
W="$(cygpath -m "$WORK" 2>/dev/null || echo "$WORK")"
mkdir -p "$W/home"
export ARK_HOME="$W/home" HOME="$W/home" USERPROFILE="$W/home"
export ARK_INDEX="$W/index.git"
unset ARK_CACHE
export GIT_AUTHOR_NAME=ark-test GIT_AUTHOR_EMAIL=ark-test@example.invalid
export GIT_COMMITTER_NAME=ark-test GIT_COMMITTER_EMAIL=ark-test@example.invalid
export GIT_CONFIG_NOSYSTEM=1

# the target: this machine's, in ark's spelling
case "$(uname -s 2>/dev/null)" in
  MINGW*|MSYS*|CYGWIN*) TOS=windows ;;
  Darwin*) TOS=macos ;;
  *) TOS=linux ;;
esac
ARCH="$(uname -m 2>/dev/null)"
case "$ARCH" in arm64|aarch64) ARCH=aarch64 ;; riscv64) ;; *) ARCH=x86_64 ;; esac
export ARK_TARGET="$ARCH-$TOS"
T="$ARK_TARGET"
case "$TOS" in
  windows) LIB=mathx.dll; EXE=.exe ;;
  macos) LIB=libmathx.dylib; EXE= ;;
  *) LIB=libmathx.so; EXE= ;;
esac

fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
run() {
  local dir="$1" log="$2"
  shift 2
  (cd "$W/$dir" && "${ARK[@]}" "$@") > "$W/$log" 2>&1
}
expect_ok() {
  local label="$1"
  shift
  if run "$@"; then ok "$label"; else bad "$label (exit $?):"; sed 's/^/        /' "$W/$2" | head -20; fi
}
expect_fail() {
  local label="$1"
  shift
  if run "$@"; then bad "$label: succeeded"; sed 's/^/        /' "$W/$2" | head -20; else ok "$label"; fi
}
has() {
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $(basename "$2")"; sed 's/^/        /' "$2" | head -30; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $(basename "$2")"; else ok "$1"; fi
}
exists() { if [ -e "$W/$2" ]; then ok "$1"; else bad "$1: no $2"; fi; }
absent() { if [ -e "$W/$2" ]; then bad "$1: $2 exists"; else ok "$1"; fi; }
commit() { git -C "$W/$1" add -A >/dev/null && git -C "$W/$1" commit -q -m "$2" && git -C "$W/$1" tag -f "$3" >/dev/null; }

echo "== ark-binary (${ARK[*]}; target $T)"

# --- the package --------------------------------------------------------------
echo "-- ark prebuild"
mkdir -p "$W/mathx/src"
printf '[package]\nname = "mathx"\nversion = "0.1.0"\n' > "$W/mathx/ark.toml"
cat > "$W/mathx/src/lib.fd" <<'EOF'
// mathx: a small library shipped prebuilt (tests/ark/ark-binary.sh)

@export
pub fn mx_add(a: i64, b: i64) -> i64 {
    return a + b;
}

@export
pub fn mx_scale(x: f64) -> f64 {
    return x * 2.5;
}

@export
pub fn mx_is_even(n: i64) -> bool {
    return n % 2 == 0;
}

// not exported: a str does not cross a prebuilt library
pub fn mx_name() -> str {
    return "mathx";
}
EOF
expect_ok "ark prebuild --dry-run" mathx dry.log prebuild --dry-run
has "the shim is built as a library" "$W/dry.log" "build src/.ark_prebuild_mathx.fd --lib --export ark_bin_mathx_mx_add,ark_bin_mathx_mx_scale,ark_bin_mathx_mx_is_even -o prebuilt/$T/$LIB"
has "an export and its C name" "$W/dry.log" "fn mx_add(a: i64, b: i64) -> i64  (C name ark_bin_mathx_mx_add)"
has "a float export" "$W/dry.log" "fn mx_scale(x: f64) -> f64"
hasnt "the str function is not exported" "$W/dry.log" "mx_name"
absent "--dry-run writes nothing" mathx/prebuilt
expect_ok "ark prebuild --dry-run --target linux-arm64" mathx dry2.log prebuild --dry-run --target linux-arm64
has "a cross target: its directory and library name" "$W/dry2.log" "-o prebuilt/aarch64-linux/libmathx.so --target aarch64-linux"

mkdir -p "$W/strlib/src"
printf '[package]\nname = "strlib"\nversion = "0.1.0"\n' > "$W/strlib/ark.toml"
printf 'pub fn greet(name: str) -> str {\n    return "hi " + name;\n}\n\npub fn twice(n: i64) -> i64 {\n    return n * 2;\n}\n' > "$W/strlib/src/lib.fd"
expect_fail "ark prebuild: a pub fn with a str stops it" strlib str.log prebuild --dry-run
has "it says which and why" "$W/str.log" "greet: parameter name: str cannot cross a prebuilt library"
has "it suggests @export" "$W/str.log" "Mark the functions to ship with @export"
printf '\n@export\npub fn shout(s: str) -> i64 {\n    return 1;\n}\n' >> "$W/strlib/src/lib.fd"
expect_fail "ark prebuild: an @export fn with a str stops it" strlib str2.log prebuild --dry-run
has "the @export fn named" "$W/str2.log" "shout: parameter s: str"
mkdir -p "$W/nolib"
printf '[package]\nname = "nolib"\nversion = "0.1.0"\n' > "$W/nolib/ark.toml"
expect_fail "ark prebuild without a lib target" nolib nolib.log prebuild

# the library itself
REAL=0
if [ -n "$FID" ]; then
  expect_ok "ark prebuild (fid build --lib)" mathx pre.log prebuild
  REAL=1
else
  if command -v zig >/dev/null 2>&1; then
    cat > "$W/mx.c" <<'EOF'
#include <stdint.h>
#ifdef _WIN32
#define X __declspec(dllexport)
#else
#define X __attribute__((visibility("default")))
#endif
X int64_t ark_bin_mathx_mx_add(int64_t a, int64_t b) { return a + b; }
X double ark_bin_mathx_mx_scale(double x) { return x * 2.5; }
X _Bool ark_bin_mathx_mx_is_even(int64_t n) { return n % 2 == 0; }
EOF
    if (cd "$W" && zig cc -shared -fPIC -o "$LIB" mx.c) >/dev/null 2>&1; then
      ok "a C library with the same exports (zig cc)"
    else
      printf 'stand-in library bytes\n' > "$W/$LIB"
    fi
  else
    printf 'stand-in library bytes\n' > "$W/$LIB"
  fi
  expect_ok "ark prebuild --from <library>" mathx pre.log prebuild --from "$W/$LIB"
fi
exists "the library in prebuilt/<target>/" "mathx/prebuilt/$T/$LIB"
exists "prebuilt.toml" "mathx/prebuilt/$T/prebuilt.toml"
absent "the shim is removed" "mathx/src/.ark_prebuild_mathx.fd"
P="$W/mathx/prebuilt/$T/prebuilt.toml"
has "prebuilt.toml: the target" "$P" "target = \"$T\""
has "prebuilt.toml: the library" "$P" "library = \"$LIB\""
has "prebuilt.toml: the link name" "$P" 'link = "mathx"'
has "prebuilt.toml: the library hash" "$P" 'library-hash = "fnv:'
has "prebuilt.toml: the source it was built from" "$P" 'source = "src"'
has "prebuilt.toml: the source hash" "$P" 'source-hash = "fnv:'
has "prebuilt.toml: each function" "$P" 'fn = "mx_add(a: i64, b: i64) -> i64"'
has "prebuilt.toml: a bool function" "$P" 'fn = "mx_is_even(n: i64) -> bool"'
hasnt "prebuilt.toml: no str function" "$P" "mx_name"
if [ -n "$FID" ]; then
  cp "$W/mathx/prebuilt/$T/$LIB" "$W/lib.before"
  expect_ok "ark prebuild --check (a rebuild gives the same library)" mathx check.log prebuild --check
  has "--check says so" "$W/check.log" "same: the library built now matches"
  if cmp -s "$W/lib.before" "$W/mathx/prebuilt/$T/$LIB"; then ok "--check leaves the recorded library as it was"; else bad "--check changed the recorded library"; fi
  # other source: another library, and --check says so (and restores it)
  cp "$W/mathx/src/lib.fd" "$W/lib.fd.keep"
  printf '\n@export\npub fn mx_three() -> i64 {\n    return 3;\n}\n' >> "$W/mathx/src/lib.fd"
  expect_fail "ark prebuild --check after the source changed" mathx check2.log prebuild --check
  has "--check: DIFFERENT" "$W/check2.log" "DIFFERENT: the library built now is"
  cp "$W/lib.fd.keep" "$W/mathx/src/lib.fd"
  if cmp -s "$W/lib.before" "$W/mathx/prebuilt/$T/$LIB"; then ok "a failed --check restores the recorded library"; else bad "a failed --check left another library"; fi
  absent "--check leaves no shim" "mathx/src/.ark_prebuild_mathx.fd"
fi
git -C "$W/mathx" init -q -b main 2>/dev/null || git -C "$W/mathx" init -q
commit mathx "mathx 0.1.0 with a prebuilt library" v0.1.0

# --- the project --------------------------------------------------------------
echo "-- a project with binary = true"
mkdir -p "$W/app/src"
expect_ok "ark init" app init.log init app
cat > "$W/app/src/main.fd" <<'EOF'
use mathx;

fn main() -> i64 {
    println(mx_add(40, 2));
    println(mx_scale(2.0));
    println(mx_is_even(10));
    return 0;
}
EOF
expect_ok "ark add --binary" app add.log add mathx --git "$W/mathx" --binary
has "binary = true in the pact's line" "$W/app/ark.toml" "mathx = { git = \"$W/mathx\", binary = true }"
expect_ok "ark list" app list.log list
has "ark list shows it" "$W/list.log" "binary"
expect_ok "ark install" app install.log install
has "the prebuilt library is used" "$W/install.log" "mathx: prebuilt $T library $LIB (its source is not compiled)"
I="$W/app/ark_modules/mathx.fd"
exists "the binary interface ark_modules/mathx.fd" app/ark_modules/mathx.fd
has "interface: extern \"C\" link" "$I" 'extern "C" link "mathx" {'
has "interface: the C name" "$I" "fn ark_bin_mathx_mx_add(a: i64, b: i64) -> i64;"
has "interface: the Fides function" "$I" "pub fn mx_add(a: i64, b: i64) -> i64 {"
has "interface: it calls the library" "$I" "return ark_bin_mathx_mx_add(a, b);"
hasnt "interface: no source function" "$I" "return a + b"
has "ark_modules/.binaries lists the library" "$W/app/ark_modules/.binaries" "mathx = \"mathx/prebuilt/$T/$LIB\""
expect_ok "ark build --dry-run" app bdry.log build --dry-run
has "the library is copied beside the program" "$W/bdry.log" "copy ark_modules/mathx/prebuilt/$T/$LIB build/$LIB"
expect_ok "ark audit" app audit.log audit
has "audit: the prebuilt library" "$W/audit.log" "prebuilt  ark_modules/mathx/prebuilt/$T/$LIB"
# the build command: the library's directory in FID_LIB_PATH (fid's C path:
# -L, and the $ORIGIN rpath on Linux); off Windows through C (fid ccompile),
# on Windows fid build (pe_ir imports the DLL by name)
has "ark build: FID_LIB_PATH for the link" "$W/bdry.log" "FID_LIB_PATH="
has "ark build: the library's directory in it" "$W/bdry.log" "ark_modules/mathx/prebuilt/$T"
if [ "$TOS" = windows ]; then
  has "Windows: fid build (pe_ir)" "$W/bdry.log" " build src/main.fd -o build/app"
else
  has "off Windows: fid ccompile (the C path links the library)" "$W/bdry.log" " ccompile src/main.fd -o build/app"
fi
# ark test: the test programs link the library and find it when they run
mkdir -p "$W/app/tests"
printf 'use mathx;\n\nfn test_mx_add() {\n    assert(mx_add(2, 3) == 5);\n}\n\nfn test_mx_is_even() {\n    assert(mx_is_even(4));\n}\n' > "$W/app/tests/mx_test.fd"
expect_ok "ark test --dry-run" app tdry.log test --dry-run
has "ark test: FID_LIB_PATH for the link" "$W/tdry.log" "FID_LIB_PATH="
has "ark test: the library's directory" "$W/tdry.log" "ark_modules/mathx/prebuilt/$T"
if [ "$TOS" = windows ]; then
  has "ark test: the library on PATH when the tests run" "$W/tdry.log" 'set "PATH='
else
  has "ark test: the library on LD_LIBRARY_PATH when the tests run" "$W/tdry.log" "LD_LIBRARY_PATH="
fi
has "ark test: fid test on the test file" "$W/tdry.log" " test tests/mx_test.fd"

if [ "$REAL" = 1 ]; then
  echo "-- the program links the prebuilt library"
  expect_ok "ark build" app build.log build
  exists "the library beside the program" "app/build/$LIB"
  if [ -x "$W/app/build/app$EXE" ]; then
    out=$(cd "$W/app/build" && LD_LIBRARY_PATH=. DYLD_LIBRARY_PATH=. "./app$EXE" 2>&1 | tr -d '\r')
    if [ "$out" = "$(printf '42\n5\ntrue')" ]; then ok "the program runs the library's code"; else bad "the program printed '$out'"; fi
    mv "$W/app/build/$LIB" "$W/app/build/$LIB.off"
    if (cd "$W/app/build" && "./app$EXE") > "$W/nolib-run.log" 2>&1; then
      bad "the program ran without the library"
    else
      ok "without the library the program stops (it is linked, not compiled in)"
    fi
    mv "$W/app/build/$LIB.off" "$W/app/build/$LIB"
  else
    bad "ark build made no build/app$EXE"
  fi
  expect_ok "ark test (the tests call the prebuilt library)" app test.log test
  has "ark test: the test target passed" "$W/test.log" "1 test target(s), 0 failed"
fi

echo "-- checks of the library"
cp "$W/app/ark_modules/mathx/prebuilt/$T/$LIB" "$W/lib.keep"
printf 'x' >> "$W/app/ark_modules/mathx/prebuilt/$T/$LIB"
expect_fail "ark audit: a changed library" app audit2.log audit
has "audit names it" "$W/audit2.log" "PREBUILT CHANGED: ark_modules/mathx/prebuilt/$T/$LIB"
cp "$W/lib.keep" "$W/app/ark_modules/mathx/prebuilt/$T/$LIB"
expect_ok "ark audit again" app audit3.log audit

expect_ok "ark install --source" app src.log install --source
absent "--source: no interface (use mathx reads the source)" app/ark_modules/mathx.fd
hasnt "--source: no library listed" "$W/app/ark_modules/.binaries" "mathx ="
expect_ok "ark install (binary again)" app again.log install
exists "the interface is back" app/ark_modules/mathx.fd

# the source changes and the prebuilt is not rebuilt: stale
printf '\n@export\npub fn mx_sub(a: i64, b: i64) -> i64 {\n    return a - b;\n}\n' >> "$W/mathx/src/lib.fd"
commit mathx "mathx: a new function, prebuilt not rebuilt" v0.1.1
expect_ok "ark update (a stale prebuilt)" app stale.log update
has "a stale prebuilt falls back to the source" "$W/stale.log" "warning: pact 'mathx' is built from source: the prebuilt library was built from other source"
absent "no interface for a stale prebuilt" app/ark_modules/mathx.fd
# prebuilt again: used again
if [ "$REAL" = 1 ]; then
  run mathx pre2.log prebuild
else
  run mathx pre2.log prebuild --from "$W/$LIB"
fi
commit mathx "mathx: prebuilt again" v0.1.2
expect_ok "ark update (prebuilt again)" app fresh.log update
has "the new prebuilt is used" "$W/fresh.log" "mathx: prebuilt $T library $LIB"
has "its new function is in the interface" "$W/app/ark_modules/mathx.fd" "pub fn mx_sub(a: i64, b: i64) -> i64 {"

# ark remove: the interface goes with the pact
expect_ok "ark remove" app rm.log remove mathx
absent "ark remove drops the interface" app/ark_modules/mathx.fd
expect_ok "ark add --binary again" app add2.log add mathx --git "$W/mathx" --binary
expect_ok "ark install" app install2.log install
exists "the interface" app/ark_modules/mathx.fd

# a library that does not match its recorded hash fails the install
printf 'tampered' >> "$W/mathx/prebuilt/$T/$LIB"
commit mathx "mathx: a library that does not match prebuilt.toml" v0.1.3
expect_fail "ark update: a library that does not match its hash" app tamper.log update
has "it says so" "$W/tamper.log" "does not match the hash prebuilt.toml records"
absent "no interface over a library that fails its check" app/ark_modules/mathx.fd

if [ "$fail" = 0 ]; then echo "ark-binary: PASS"; else echo "ark-binary: FAIL"; fi
exit $fail
