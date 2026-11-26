#!/usr/bin/env bash
# tools/pass_matrix.fd (roadmap AD4) does what scripts/pass-matrix.sh does:
# both run with the same arguments and environment in a scratch copy of
# the tree (scripts/, compiler/egraph.fd and rules.fd, two tests), and
# their output, exit status and matrix.txt are compared (the OUT folder's
# name masked).
#
#   bash tests/tools/pass_matrix/check.sh        # about 8 min
#
# FID: the compiler (default release/fid.exe); FID_HOME tells a compiler
# outside the tree where stdlib/ is. OUT: where the tool, the copy and the
# outputs go. Cases: SELFTEST=1; no target (the header); a matrix on
# x86_64-windows (`emit --nat`; x86_64-linux on a Linux host) with
# IROPT="dce gvn" NAT=fold IME=dce EGRAPH=comm RULES=0 over test_hashmap
# and own_path.fd (it prints its own path, so every switch "differs" with
# an "output differs:" line); the same again with RESUME=1 (nothing run).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-pass-matrix-check}"
mkdir -p "$OUT"
X=""
TG=x86_64-linux
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe"; TG=x86_64-windows ;; esac
exe="$OUT/pass_matrix$X"
if ! "$FID" ccompile tools/pass_matrix.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/pass_matrix.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
export FID
T="$OUT/tree"
rm -rf "$T"
mkdir -p "$T/tests/bootstrap" "$T/compiler"
cp -r scripts "$T/"
cp compiler/egraph.fd compiler/rules.fd "$T/compiler/"
cp tests/bootstrap/test_hashmap.fd "$T/tests/bootstrap/"
cp tests/tools/pass_matrix/own_path.fd "$T/tests/bootstrap/test_own_path.fd"
pass=0
fail=0
mask() { tr -d '\r' < "$1" | sed -E 's#\(summary: .*\)#(summary: X)#'; }
compare() {   # name, keep (1: keep the first side's OUT for the second), then the arguments
    local name="$1" keep="$2"; shift 2
    [ "$keep" = 1 ] || rm -rf "$T/build"
    (cd "$T" && OUT= bash scripts/pass-matrix.sh "$@") > "$OUT/$name.sh" 2>&1
    echo "exit $?" >> "$OUT/$name.sh"
    cp "$T/build/pm/$TG/matrix.txt" "$OUT/$name.sh.matrix" 2>/dev/null || echo none > "$OUT/$name.sh.matrix"
    [ "$keep" = 1 ] || rm -rf "$T/build"
    (cd "$T" && OUT= "$exe" "$@") > "$OUT/$name.fd" 2>&1
    echo "exit $?" >> "$OUT/$name.fd"
    cp "$T/build/pm/$TG/matrix.txt" "$OUT/$name.fd.matrix" 2>/dev/null || echo none > "$OUT/$name.fd.matrix"
    if diff <(mask "$OUT/$name.sh") <(mask "$OUT/$name.fd") > "$OUT/$name.diff" \
       && diff <(mask "$OUT/$name.sh.matrix") <(mask "$OUT/$name.fd.matrix") >> "$OUT/$name.diff"; then
        pass=$((pass + 1)); echo "ok   $name ($(tail -1 "$OUT/$name.fd"), $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name"; head -20 "$OUT/$name.diff"
    fi
}
SELFTEST=1 compare selftest 0
compare usage 0
export IROPT="dce gvn" NAT=fold IME=dce EGRAPH=comm RULES=0
compare matrix 0 $TG tests/bootstrap/test_hashmap.fd tests/bootstrap/test_own_path.fd
RESUME=1 compare resume 1 $TG tests/bootstrap/test_hashmap.fd tests/bootstrap/test_own_path.fd
echo "pass_matrix port: $pass same, $fail differ"
[ $fail = 0 ]
