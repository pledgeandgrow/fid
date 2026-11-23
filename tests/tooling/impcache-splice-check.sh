#!/bin/bash
# BG35 (docs/roadmap/bugs-roadmap.md): the import cache (CB1) keeps no
# parse of a module with a `comptime splice` (compiler/compiler.fd
# cached_parse). The parser flag that tells process_gen to expand splices
# (parser.fd g_saw_splice) is set only by a parse, so a second build that
# took the module from the cache left its splice unexpanded.
#   1. a program importing such a module prints the same twice in a row
#      with the cache on (cold, then warm), and as with the cache off;
#   2. the module with the splice is parsed both times (not saved).
#   FID=release/fid.exe bash tests/tooling/impcache-splice-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
ROOT=$(pwd)
case "$FID" in
    /*|[A-Za-z]:*) ;;
    */*) FID="$ROOT/$FID" ;;
esac
export FID_HOME=${FID_HOME:-$ROOT}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
TMPW="$TMP"
if command -v cygpath >/dev/null 2>&1; then TMPW=$(cygpath -m "$TMP"); fi
mkdir -p "$TMP/cache" "$TMP/p"
export FID_IMPCACHE_DIR="$TMPW/cache"
unset FID_IMPCACHE
cat > "$TMP/p/gen.fd" <<'EOF2'
module gen;

pub fn thirty() -> i64 {
    return comptime splice { return "3 * 10"; };
}
EOF2
cat > "$TMP/p/main.fd" <<'EOF2'
use gen;

fn main() -> i64 {
    println(thirty());
    return 0;
}
EOF2
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
FID_IMPCACHE=off "$FID" run "$TMP/p/main.fd" > "$TMP/off.out" 2> "$TMP/off.err"
"$FID" run "$TMP/p/main.fd" > "$TMP/cold.out" 2> "$TMP/cold.err"
"$FID" run "$TMP/p/main.fd" > "$TMP/warm.out" 2> "$TMP/warm.err"
want=30
for run in off cold warm; do
    got=$(tr -d '\r' < "$TMP/$run.out")
    if [ "$got" = "$want" ]; then ok "1. $run: $want"; else bad "1. $run: got '$got'"; tail -5 "$TMP/$run.err"; fi
done
if grep -q "import cache: 0 hit(s)" "$TMP/warm.err" || ! grep -q "import cache:" "$TMP/warm.err"; then ok "2. the warm build took nothing with a splice from the cache"; else bad "2. warm: $(grep -o 'import cache: [0-9]* hit(s), [0-9]* parsed, [0-9]* saved' "$TMP/warm.err" | tail -1)"; fi
if [ $fail = 0 ]; then echo "impcache-splice-check: PASS"; else echo "impcache-splice-check: FAIL"; fi
exit $fail
