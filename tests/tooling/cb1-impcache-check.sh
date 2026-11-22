#!/bin/bash
# CB1 (complaint-roadmap): the import cache. compiler/compiler.fd keeps the
# parse of each imported module on disk (imp_load / imp_save, called from
# cached_parse), keyed by the module's text, the struct / enum / import
# names known when it is parsed that occur in its text, and the compiler.
# A later compile neither lexes nor parses an unchanged module. Checked
# here on tests/projects/cb1_impcache (two modules; a function name defined
# by both, so the qualified calls a parse records must come back on a hit;
# an alias import; a struct literal and enum of the other module):
#   1. the cache off, cold and warm give the same output, and dump-c gives
#      the same C byte for byte with the cache off and warm
#   2. the cold run parses and saves both modules, the warm run hits both
#   3. a program that knows another name occurring in calc.fd (struct
#      Gadget) parses calc.fd again (another key) and still reuses geo.fd
#   4. an edited module is parsed again and its new code runs
#   5. damaged cache files are ignored (parsed again, same output)
#   6. the core artifact (version 4: core stored once when nothing is
#      merged): a warm `check` hits it
# TIME=1 also times `fid check compiler/main.fd --time` cold and warm and
# requires the warm `[time] imports` to be lower (minutes; quiet machine).
#   FID=release/fid.exe bash tests/tooling/cb1-impcache-check.sh
# Run from the repository root. Exit 0 when every check passes.
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
mkdir -p "$TMP/cache"
export FID_IMPCACHE_DIR="$TMPW/cache"
unset FID_IMPCACHE
P=tests/projects/cb1_impcache
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
summary() { grep -o "import cache: [0-9]* hit(s), [0-9]* parsed, [0-9]* saved" "$1" | tail -1; }
expect_sum() {
    local got
    got=$(summary "$2")
    if [ "$got" = "import cache: $3" ]; then ok "$1: $3"; else bad "$1: expected 'import cache: $3', got '$got'"; fi
}
expect_out() {
    if [ "$(cat "$2" | tr -d '\r')" = "$3" ]; then ok "$1: output"; else bad "$1: output"; echo "  got:"; sed 's/^/    /' "$2"; echo "  expected:"; echo "$3" | sed 's/^/    /'; fi
}
MAIN_OUT=$(printf '16\n24\n24')

# 1-2. off, cold, warm
FID_IMPCACHE=off "$FID" run $P/main.fd > "$TMP/off.out" 2> "$TMP/off.err"
expect_out "cache off" "$TMP/off.out" "$MAIN_OUT"
if grep -q "import cache:" "$TMP/off.err"; then bad "FID_IMPCACHE=off still uses the cache"; else ok "FID_IMPCACHE=off: no cache"; fi
if ls "$TMP/cache" | grep -q .; then bad "FID_IMPCACHE=off wrote cache files"; else ok "FID_IMPCACHE=off: nothing written"; fi
"$FID" run $P/main.fd > "$TMP/cold.out" 2> "$TMP/cold.err"
expect_out "cold" "$TMP/cold.out" "$MAIN_OUT"
expect_sum "cold" "$TMP/cold.err" "0 hit(s), 2 parsed, 2 saved"
"$FID" run $P/main.fd > "$TMP/warm.out" 2> "$TMP/warm.err"
expect_out "warm" "$TMP/warm.out" "$MAIN_OUT"
expect_sum "warm" "$TMP/warm.err" "2 hit(s), 0 parsed, 0 saved"
FID_IMPCACHE=off "$FID" dump-c $P/main.fd > "$TMP/off.c" 2> /dev/null
"$FID" dump-c $P/main.fd > "$TMP/warm.c" 2> "$TMP/warmc.err"
expect_sum "warm dump-c" "$TMP/warmc.err" "2 hit(s), 0 parsed, 0 saved"
if [ -s "$TMP/off.c" ] && cmp -s "$TMP/off.c" "$TMP/warm.c"; then ok "dump-c: same C with the cache off and warm"; else bad "dump-c differs with the cache warm (or is empty)"; fi

# 3. another name known: calc.fd (holds the word Gadget) under another key
"$FID" run $P/gadget_main.fd > "$TMP/g1.out" 2> "$TMP/g1.err"
expect_out "Gadget program" "$TMP/g1.out" "161"
expect_sum "Gadget program" "$TMP/g1.err" "1 hit(s), 1 parsed, 1 saved"
"$FID" run $P/gadget_main.fd > "$TMP/g2.out" 2> "$TMP/g2.err"
expect_out "Gadget program again" "$TMP/g2.out" "161"
expect_sum "Gadget program again" "$TMP/g2.err" "2 hit(s), 0 parsed, 0 saved"
"$FID" run $P/main.fd > "$TMP/m3.out" 2> "$TMP/m3.err"
expect_out "first program after it" "$TMP/m3.out" "$MAIN_OUT"
expect_sum "first program after it" "$TMP/m3.err" "1 hit(s), 1 parsed, 1 saved"

# 4. an edited module
mkdir -p "$TMP/proj"
cp $P/main.fd $P/geo.fd $P/calc.fd "$TMP/proj/"
"$FID" run "$TMPW/proj/main.fd" > /dev/null 2>&1
sed -i 's/return w \* h;/return w * h + 1;/' "$TMP/proj/geo.fd"
"$FID" run "$TMPW/proj/main.fd" > "$TMP/e.out" 2> "$TMP/e.err"
expect_out "edited geo.fd" "$TMP/e.out" "$(printf '17\n24\n25')"
expect_sum "edited geo.fd" "$TMP/e.err" "1 hit(s), 1 parsed, 1 saved"

# 5. damaged files
for f in "$TMP"/cache/fid-imp-*.bin; do
    head -c 100 "$f" > "$f.cut" && mv "$f.cut" "$f"
done
"$FID" run $P/main.fd > "$TMP/d.out" 2> "$TMP/d.err"
expect_out "damaged cache" "$TMP/d.out" "$MAIN_OUT"
expect_sum "damaged cache" "$TMP/d.err" "0 hit(s), 2 parsed, 2 saved"

# 6. the core artifact, version 4
ASTF="$TMPW/core.bin"
FID_ASTCACHE="$ASTF" "$FID" check tests/bootstrap/hello.fd > /dev/null 2>&1
m1=$(stat -c %Y "$TMP/core.bin" 2>/dev/null)
sleep 2
FID_ASTCACHE="$ASTF" "$FID" check tests/bootstrap/hello.fd > "$TMP/ast.out" 2> "$TMP/ast.err"
code=$?
m2=$(stat -c %Y "$TMP/core.bin" 2>/dev/null)
# a hit reads the artifact and does not write it again
if [ $code -eq 0 ] && [ -s "$TMP/core.bin" ] && [ -n "$m1" ] && [ "$m1" = "$m2" ]; then ok "core artifact written once, then read"; else bad "core artifact (exit $code, mtime $m1 -> $m2)"; fi
if [ -s "$TMP/core.bin" ] && [ "$(od -An -tu1 -j8 -N1 "$TMP/core.bin" | tr -d ' ')" = "4" ]; then ok "core artifact is version 4"; else bad "core artifact is not version 4"; fi

# TIME=1: the self-compile's front end, cold and warm
if [ "${TIME:-0}" = "1" ]; then
    rm -f "$TMP"/cache/*
    "$FID" check compiler/main.fd --time > /dev/null 2> "$TMP/t1.err"
    "$FID" check compiler/main.fd --time > /dev/null 2> "$TMP/t2.err"
    c=$(grep "^\[time\] imports:" "$TMP/t1.err" | grep -o "[0-9]* ms" | head -1 | tr -dc 0-9)
    w=$(grep "^\[time\] imports:" "$TMP/t2.err" | grep -o "[0-9]* ms" | head -1 | tr -dc 0-9)
    echo "     check compiler/main.fd: imports cold ${c:-?} ms, warm ${w:-?} ms; $(summary "$TMP/t2.err")"
    grep "^\[time\]" "$TMP/t2.err" | sed 's/^/     warm /'
    if [ -n "$c" ] && [ -n "$w" ] && [ "$w" -lt "$c" ]; then ok "warm imports faster"; else bad "warm imports not faster"; fi
fi

if [ $fail -eq 0 ]; then echo "PASS cb1-impcache-check"; else echo "FAIL cb1-impcache-check"; fi
exit $fail
