#!/bin/bash
# SP8 (docs/roadmap/llvm-roadmap.md): a one-line edit rebuilds in a warm
# `fid serve` from what the server keeps between requests: the parsed core
# (astcache), the imported modules' parses (modcache), the checked bodies
# (CB2 in build requests: compiler.fd cb2_request_build), the machine code
# of every unchanged function (nat_ir.fd g_nat_fncache_mem). See
# docs/reference/incremental.md.
#
# On a program in a temporary directory, built through one `fid serve`
# for $T (default x86_64-linux, any host; T=nat: Windows x64 through the
# machine layer, `--nat`), in release mode and in debug mode (--debug: no
# mid-end, the fast allocator):
#   1. a first emit, then the same emit again (warm, no edit);
#   2. one line of one function edited: the image equals the one-shot
#      `fid emit` of the edited file, and the request checked again fewer
#      bodies than the program has ("N of M bodies checked", N < M);
#   3. one line of another function edited: as 2;
#   4. a type error written into that line: status 1 and the error;
#   5. the error taken out again (the file as in 3): status 0 and the
#      image of 3.
# Prints each request's time (the server's own measure on stderr, `fid
# serve: emit <file>: <ms> ms, ...`) and, for the edited rebuilds of 2 and
# 3, where it went (the request asks for --time; ROWS=0: not): the front
# end, lowering, the mid-end, the machine layer and, with STATS=1 (the
# request gets --stats too), how many functions the function cache
# replayed ("[fncache] ... N hits, M compiled": SP6 wants M to be the
# edited function and what inlined it). The SP8 target is under 100 ms for
# the edited rebuilds of 2 and 3; the last line says whether it was met
# (STRICT_TIME=1: a miss fails the script; correctness always does). The
# server is not started with --relink: an edited image must be the
# one-shot build's (SP7's kept layout is not; relink_check.sh opts in).
#   FID=release/fid.exe bash tests/tooling/sp8-edit-rebuild-check.sh
#   MODE=debug (or release): one mode only.  TARGET_MS=<n>: another target.
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
export FID_HOME=${FID_HOME:-$(pwd)}
T=${T:-x86_64-linux}
MODE=${MODE:-release debug}
TARGET_MS=${TARGET_MS:-100}
TMP=$(mktemp -d)
# the paths go inside request lines, which Git Bash does not convert
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
trap 'rm -rf "$TMP"' EXIT
if [ "$T" = nat ]; then
    TFLAGS="--nat"
    EXT=".exe"
else
    TFLAGS="--target $T"
    EXT=".out"
fi
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# the program: base, then edit 1 (a line of sum), edit 2 (a line of
# scale), edit 3 (a type error in that line)
write_prog() {
    local sum_line="$1" scale_line="$2"
    cat > "$TMP/prog.fd" <<EOF
struct Acc {
    total: i64,
    count: i64,
}
fn sum(n: i64) -> i64 {
    var s: i64 = 0;
    var i: i64 = 1;
    while i <= n {
$sum_line
        i = i + 1;
    }
    return s;
}
fn scale(x: i64) -> i64 {
$scale_line
}
fn acc_add(a: Acc, v: i64) -> Acc {
    return Acc { total: a.total + v, count: a.count + 1 };
}
fn label(k: i64) -> str {
    if k % 2 == 0 {
        return "even";
    }
    return "odd";
}
fn main() -> i64 {
    var a: Acc = Acc { total: 0, count: 0 };
    var k: i64 = 0;
    while k < 8 {
        a = acc_add(a, scale(sum(k * 10)));
        k = k + 1;
    }
    println(int_to_str(a.total) + " " + int_to_str(a.count) + " " + label(a.total));
    return 0;
}
EOF
}
BASE_SUM='        s = s + i;'
EDIT_SUM='        s = s + i * 2;'
BASE_SCALE='    return x * 3;'
EDIT_SCALE='    return x * 3 + 1;'
BAD_SCALE='    return label(x);'

coproc SRV { "$FID" serve 2>"$TMP/serve.err"; }
ask() {
    echo "$1" >&"${SRV[1]}"
    local out="" l
    while IFS= read -r l <&"${SRV[0]}"; do
        l=${l%$'\r'}
        out="$out$l"$'\n'
        case "$l" in done*) break ;; esac
    done
    printf '%s' "$out"
}
status_of() { printf '%s' "$1" | grep '^done' | tail -1 | sed 's/^done //'; }
# the server's time and CB2 note of its last build request
last_line() { grep -F "fid serve: emit " "$TMP/serve.err" | tail -1 | tr -d '\r'; }
ms_of() { printf '%s' "$1" | sed -n 's/.*: \([0-9][0-9]*\) ms.*/\1/p'; }
# "N of M bodies checked": 0 when N < M, 1 otherwise (or no note)
fewer_checked() {
    local n m
    n=$(printf '%s' "$1" | sed -n 's/.*, \([0-9][0-9]*\) of \([0-9][0-9]*\) bodies checked.*/\1/p')
    m=$(printf '%s' "$1" | sed -n 's/.*, \([0-9][0-9]*\) of \([0-9][0-9]*\) bodies checked.*/\2/p')
    [ -n "$n" ] && [ -n "$m" ] && [ "$n" -lt "$m" ]
}
# oneshot <out> [flags]: the one-shot image of prog.fd as it is now
oneshot() {
    local out="$1"; shift
    "$FID" emit "$TMP/prog.fd" $TFLAGS --no-cache "$@" -o "$out" > "$TMP/oneshot.log" 2>&1
}

slow=""
r=0
for mode in $MODE; do
    MF=""
    if [ "$mode" = debug ]; then MF="--debug"; fi
    echo "--- $mode mode ($T) ---"
    write_prog "$BASE_SUM" "$BASE_SCALE"
    r=$((r + 1))
    A=$(ask "emit $TMP/prog.fd $TFLAGS $MF -o $TMP/$mode-a$r$EXT")
    L=$(last_line)
    if [ "$(status_of "$A")" = 0 ]; then
        ok "1. first emit: $(ms_of "$L") ms"
    else
        bad "1. first emit: $(printf '%s' "$A" | tail -3 | tr '\n' ' ')"
    fi
    r=$((r + 1))
    A=$(ask "emit $TMP/prog.fd $TFLAGS $MF -o $TMP/$mode-a$r$EXT")
    L=$(last_line)
    if [ "$(status_of "$A")" = 0 ]; then
        ok "1. warm emit, no edit: $(ms_of "$L") ms ($L)"
    else
        bad "1. warm emit: $(printf '%s' "$A" | tail -3 | tr '\n' ' ')"
    fi
    step=2
    for edit in sum scale; do
        if [ $edit = sum ]; then
            write_prog "$EDIT_SUM" "$BASE_SCALE"
        else
            write_prog "$EDIT_SUM" "$EDIT_SCALE"
        fi
        r=$((r + 1))
        out="$TMP/$mode-e$r$EXT"
        before=$(wc -l < "$TMP/serve.err")
        TF=""
        if [ "${ROWS:-1}" = 1 ]; then TF="--time"; fi
        # (STATS=1: --stats too, for the function cache's line)
        if [ "${STATS:-0}" = 1 ]; then TF="$TF --stats"; fi
        A=$(ask "emit $TMP/prog.fd $TFLAGS $MF $TF -o $out")
        L=$(last_line)
        ms=$(ms_of "$L")
        if [ -n "$TF" ]; then
            # where this rebuild's time went (its own lines of the log)
            tail -n +$((before + 1)) "$TMP/serve.err" | tr -d '\r' | grep -E '^\[time\] (front end|typecheck|mono|lower|prune|iropt|rules|egraph|native code|  nat record|  nat alloc\+emit|write|total):|^\[fncache\] (entries|attempts)|^\[prof\] native:' | sed 's/^/     /'
        fi
        oneshot "$TMP/$mode-one$r$EXT" $MF
        if [ "$(status_of "$A")" = 0 ] && cmp -s "$out" "$TMP/$mode-one$r$EXT"; then
            ok "$step. one line of $edit edited: $ms ms, the image of the one-shot emit"
        else
            bad "$step. one line of $edit edited: status $(status_of "$A"); $(cmp "$out" "$TMP/$mode-one$r$EXT" 2>&1 | head -1)"
        fi
        if fewer_checked "$L"; then
            ok "$step. checked again: ${L##*, }"
        else
            bad "$step. CB2 did not narrow the checks: $L"
        fi
        if [ -n "$ms" ] && [ "$ms" -ge "$TARGET_MS" ]; then
            slow="$slow $mode/$edit=${ms}ms"
        fi
        if [ -z "$ms" ]; then
            slow="$slow $mode/$edit=?"
        fi
        step=$((step + 1))
    done
    good="$TMP/$mode-e$r$EXT"
    write_prog "$EDIT_SUM" "$BAD_SCALE"
    r=$((r + 1))
    A=$(ask "emit $TMP/prog.fd $TFLAGS $MF -o $TMP/$mode-x$r$EXT")
    if [ "$(status_of "$A")" = 1 ] && printf '%s' "$A" | grep -qi "error"; then
        ok "4. a type error in scale: status 1, reported"
    else
        bad "4. a type error in scale: status $(status_of "$A"): $(printf '%s' "$A" | tail -3 | tr '\n' ' ')"
    fi
    write_prog "$EDIT_SUM" "$EDIT_SCALE"
    r=$((r + 1))
    A=$(ask "emit $TMP/prog.fd $TFLAGS $MF -o $TMP/$mode-f$r$EXT")
    if [ "$(status_of "$A")" = 0 ] && cmp -s "$good" "$TMP/$mode-f$r$EXT"; then
        ok "5. the error taken out: status 0, the image of 3 ($(ms_of "$(last_line)") ms)"
    else
        bad "5. the error taken out: status $(status_of "$A"); $(cmp "$good" "$TMP/$mode-f$r$EXT" 2>&1 | head -1)"
    fi
done
echo "quit" >&"${SRV[1]}"
wait 2>/dev/null
if [ -z "$slow" ]; then
    echo "SP8 target met: every edited rebuild under $TARGET_MS ms"
else
    echo "SP8 target not met (under $TARGET_MS ms):$slow"
    if [ "${STRICT_TIME:-0}" = 1 ]; then fail=1; fi
fi
if [ $fail = 0 ]; then echo "sp8-edit-rebuild-check: PASS"; else echo "sp8-edit-rebuild-check: FAIL"; tail -20 "$TMP/serve.err"; fi
exit $fail
