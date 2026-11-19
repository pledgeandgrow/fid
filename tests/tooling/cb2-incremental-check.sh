#!/bin/bash
# CB2 (docs/roadmap/complaint-roadmap.md): `fid serve` checks again only the
# bodies an edit since its last clean check can reach (compiler/compiler.fd
# cb2_skip_set), and stops after the borrow check. docs/incremental-check.md.
#
# Part A (correctness, a three-module program in a temporary directory):
# every request's status and errors equal those of a server run with
# FID_CB2=off (full checks), and the notes say what was checked again:
#    1. first check: every body;          2. no edit: none;
#    3. a body of the program edited: that function and its caller;
#    4. a type error written there: reported;   5. reverted: none;
#    6. a signature in geo.fd changed: the unchanged caller in calc.fd
#       reports the wrong call;              7. reverted;
#    8. a field of geo's struct renamed: the unchanged literal in main.fd
#       reports it;                           9. reverted;
#   10. calc_lone (calc.fd) starts printing: the unchanged sandbox block of
#       main.fd that calls it reports the IO effect;   11. reverted;
#   12. a line comment and blank lines added above everything: none (the
#       item hashes leave out positions).
# Part B (timing, compiler/main.fd on a snapshot of compiler/): a first
# check, an unchanged one, a statement added in one function (no error),
# then a type error in it; prints each request's time and what it checked.
# The target of CB2 is under 50 ms from the edit to the diagnostics; the
# script prints whether it was met (STRICT_TIME=1: a miss fails it), and
# fails when an edit checks again more than a tenth of the bodies.
#   FID=release/fid.exe bash tests/tooling/cb2-incremental-check.sh
#   PART=A (or B): one part only.
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
export FID_HOME=${FID_HOME:-$(pwd)}
ROOT=$(pwd)
PART=${PART:-AB}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# ask <in-fd> <out-fd> <request>: the answer up to `done <status>`
ask() {
    echo "$3" >&"$1"
    local out="" l
    while IFS= read -r l <&"$2"; do
        l=${l%$'\r'}
        out="$out$l"$'\n'
        case "$l" in done*) break ;; esac
    done
    printf '%s' "$out"
}
status_of() { printf '%s' "$1" | grep '^done' | head -1 | sed 's/^done //'; }
# the error lines of an answer (not the note, not done)
errors_of() { printf '%s' "$1" | grep -v '^done' | grep -v ': no errors (' | grep -v ' error(s) (' ; }
note_of() { printf '%s' "$1" | grep -E ': no errors \(| error\(s\) \(' | head -1; }

if [[ $PART == *A* ]]; then
P="$TMP/a"
mkdir -p "$P"
cat > "$P/geo.fd" <<'EOF'
module geo;
pub struct Pt {
    pub x: i64,
    pub y: i64,
}
pub fn pt_sum(p: Pt) -> i64 {
    return p.x + p.y;
}
EOF
cat > "$P/calc.fd" <<'EOF'
module calc;
use geo;
pub fn calc_twice(p: Pt) -> i64 {
    return pt_sum(p) * 2;
}
pub fn calc_lone(n: i64) -> i64 {
    return n * 3;
}
EOF
cat > "$P/main.fd" <<'EOF'
use geo;
use calc;
fn helper(n: i64) -> i64 {
    return calc_lone(n) + 1;
}
fn other(n: i64) -> i64 {
    return n - 1;
}
fn boxed() -> i64 {
    var r: i64 = 0;
    sandbox {
        r = calc_lone(2);
    }
    return r;
}
fn main() -> i64 {
    val p = Pt { x: 1, y: 2 };
    println(int_to_str(calc_twice(p) + helper(2) + other(5) + boxed()));
    return 0;
}
EOF
cp "$P/geo.fd" "$P/geo.orig"; cp "$P/calc.fd" "$P/calc.orig"; cp "$P/main.fd" "$P/main.orig"
restore() { cp "$P/geo.orig" "$P/geo.fd"; cp "$P/calc.orig" "$P/calc.fd"; cp "$P/main.orig" "$P/main.fd"; }
# edit <step>: the files of step n (each from the original, or step 3's)
edit() {
    case "$1" in
        1|2) restore ;;
        3|5|7|9|11) restore; sed -i 's/return n - 1;/return n - 2;/' "$P/main.fd" ;;
        4) sed -i 's/return n - 2;/return "two";/' "$P/main.fd" ;;
        6) sed -i 's/pub fn pt_sum(p: Pt) -> i64 {/pub fn pt_sum(p: Pt, k: i64) -> i64 {/; s/return p.x + p.y;/return p.x + p.y + k;/' "$P/geo.fd" ;;
        8) sed -i 's/pub y: i64,/pub z: i64,/; s/return p.x + p.y;/return p.x + p.z;/' "$P/geo.fd" ;;
        10) sed -i 's/    return n \* 3;/    println("calc");\n    return n * 3;/' "$P/calc.fd" ;;
        12) restore; sed -i 's/return n - 1;/return n - 2;/' "$P/main.fd"
            { printf '// moved\n\n\n'; cat "$P/geo.fd"; } > "$P/geo.tmp" && mv "$P/geo.tmp" "$P/geo.fd"
            { printf '\n// moved too\n'; cat "$P/main.fd"; } > "$P/main.tmp" && mv "$P/main.tmp" "$P/main.fd" ;;
    esac
}
# run_steps <log>: one server, steps 1..12; answers in $P/ans.<n>.<tag>
run_steps() {
    local tag=$1
    coproc SRV { cd "$P" && "$FID" serve 2>"$P/serve.$tag.log"; }
    local n
    for n in 1 2 3 4 5 6 7 8 9 10 11 12; do
        edit $n
        ask "${SRV[1]}" "${SRV[0]}" "check main.fd" > "$P/ans.$n.$tag"
    done
    echo "quit" >&"${SRV[1]}"
    wait 2>/dev/null
}
FID_CB2=off run_steps off
FID_CB2= run_steps on
want_status=(x 0 0 0 1 0 1 0 1 0 1 0 0)
for n in 1 2 3 4 5 6 7 8 9 10 11 12; do
    A=$(cat "$P/ans.$n.on"); B=$(cat "$P/ans.$n.off")
    sa=$(status_of "$A"); sb=$(status_of "$B")
    if [ "$sa" != "${want_status[$n]}" ] || [ "$sb" != "${want_status[$n]}" ]; then
        bad "A$n: status $sa (FID_CB2=off: $sb), expected ${want_status[$n]}"
        printf '%s\n---\n%s\n' "$A" "$B" | sed 's/^/     /'
        continue
    fi
    if [ "$(errors_of "$A")" != "$(errors_of "$B")" ]; then
        bad "A$n: errors differ from a full check"
        printf '%s\n---\n%s\n' "$A" "$B" | sed 's/^/     /'
        continue
    fi
    ok "A$n: status $sa, same errors as a full check; $(note_of "$A" | sed 's/.*ms, //; s/)$//')"
done
# what each request checked again (the bodies are helper, other, boxed, main,
# pt_sum, calc_twice, calc_lone)
expect_note() {
    local n=$1 want=$2 got
    got=$(note_of "$(cat "$P/ans.$n.on")")
    case "$got" in *"$want"*) ok "A$n checked: $want" ;; *) bad "A$n checked: '$got', expected '$want'" ;; esac
}
expect_note 1 "all bodies checked: first check"
expect_note 2 "0 of 7 bodies checked"
expect_note 3 "2 of 7 bodies checked"
expect_note 4 "2 of 7 bodies checked"
expect_note 5 "0 of 7 bodies checked"
expect_note 6 "3 of 7 bodies checked"
expect_note 7 "0 of 7 bodies checked"
expect_note 8 "3 of 7 bodies checked"
expect_note 9 "0 of 7 bodies checked"
expect_note 10 "4 of 7 bodies checked"
expect_note 11 "0 of 7 bodies checked"
expect_note 12 "0 of 7 bodies checked"
# the errors the unchanged modules must report
grep -q "pt_sum" "$P/ans.6.on" && ok "A6 reports the call of pt_sum in calc.fd" || bad "A6: no pt_sum error: $(cat "$P/ans.6.on")"
grep -qi "sandbox" "$P/ans.10.on" && ok "A10 reports the sandbox error in main.fd" || bad "A10: no sandbox error: $(cat "$P/ans.10.on")"
fi

if [[ $PART == *B* ]]; then
B="$TMP/b"
mkdir -p "$B/compiler"
cp "$ROOT"/compiler/*.fd "$B/compiler/"
M="$B/compiler/main.fd"
cp "$M" "$B/main.orig"
grep -q '^fn serve_words(line: str) -> Vec<str> {$' "$M" || bad "B: compiler/main.fd has no 'fn serve_words(line: str) -> Vec<str> {' line to edit"
coproc SRV2 { cd "$B" && FID_SERVE_TIME=1 "$FID" serve 2>"$B/serve.log"; }
ms_of() { printf '%s' "$1" | grep -oE '\(([0-9]+) ms' | head -1 | grep -oE '[0-9]+'; }
B1=$(ask "${SRV2[1]}" "${SRV2[0]}" "check compiler/main.fd")
echo "     B1 first check:  $(note_of "$B1")"
[ "$(status_of "$B1")" = 0 ] || bad "B1: compiler/main.fd does not check clean: $(printf '%s' "$B1" | head -5)"
B2=$(ask "${SRV2[1]}" "${SRV2[0]}" "check compiler/main.fd")
echo "     B2 unchanged:    $(note_of "$B2")"
sed -i 's/^fn serve_words(line: str) -> Vec<str> {$/&\n    var cb2_probe: i64 = str_len(line) + 1;/' "$M"
B3=$(ask "${SRV2[1]}" "${SRV2[0]}" "check compiler/main.fd")
echo "     B3 edit:         $(note_of "$B3")"
sed -i 's/    var cb2_probe: i64 = str_len(line) + 1;/    var cb2_probe: i64 = cb2_no_such_fn(line);/' "$M"
B4=$(ask "${SRV2[1]}" "${SRV2[0]}" "check compiler/main.fd")
echo "     B4 type error:   $(note_of "$B4")"
cp "$B/main.orig" "$M"
B5=$(ask "${SRV2[1]}" "${SRV2[0]}" "check compiler/main.fd")
echo "     B5 reverted:     $(note_of "$B5")"
echo "quit" >&"${SRV2[1]}"
wait 2>/dev/null
[ "$(status_of "$B3")" = 0 ] && ok "B3: the edit checks clean" || bad "B3: $(printf '%s' "$B3" | head -5)"
if [ "$(status_of "$B4")" = 1 ] && grep -q "cb2_no_such_fn" <<<"$B4"; then ok "B4: the type error is reported"; else bad "B4: $(printf '%s' "$B4" | head -5)"; fi
[ "$(status_of "$B5")" = 0 ] && ok "B5: reverted, clean" || bad "B5: $(printf '%s' "$B5" | head -5)"
# what the edit checked again: a few bodies of main.fd, not the program
for x in "$B3" "$B4"; do
    k=$(note_of "$x" | grep -oE '[0-9]+ of [0-9]+ bodies' | head -1)
    kc=${k%% of*}; kt=$(echo "$k" | sed 's/.* of //; s/ bodies//')
    if [ -n "$kc" ] && [ -n "$kt" ] && [ "$kc" -le $((kt / 10)) ]; then ok "B: the edit checked $kc of $kt bodies"; else bad "B: the edit checked '$k'"; fi
done
t3=$(ms_of "$B3"); t4=$(ms_of "$B4")
echo "     edit to diagnostics: $t3 ms (clean), $t4 ms (with the error); target < 50 ms"
echo "     stage times of the last requests (FID_SERVE_TIME=1):"
grep '^\[time\]' "$B/serve.log" | tail -24 | sed 's/^/       /'
if [ -n "$t3" ] && [ -n "$t4" ] && [ "$t3" -lt 50 ] && [ "$t4" -lt 50 ]; then
    ok "B: under 50 ms from the edit to the diagnostics"
elif [ "${STRICT_TIME:-0}" = 1 ]; then
    bad "B: over 50 ms ($t3 ms, $t4 ms)"
else
    echo "NOTE B: the 50 ms target is not met ($t3 ms, $t4 ms); STRICT_TIME=1 makes this a failure"
fi
fi

if [ $fail = 0 ]; then echo "PASS cb2-incremental-check"; else echo "FAIL cb2-incremental-check"; fi
exit $fail
