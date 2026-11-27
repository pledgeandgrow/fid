#!/usr/bin/env bash
# SP9: the import closure parsed ahead in FID_PAR threads (compiler.fd
# pp_parse_ahead, parser.fd pp_parse_par / pp_predict / pp_valid) gives the
# sequential front end's output byte for byte, run after run.
#
#   FID=release/fid-dev.exe bash tests/par/parse_par_check.sh
#   FID=release/fid-dev.exe SELF=1 bash tests/par/parse_par_check.sh   # + compiler/main.fd
#   ... SELF=1 DUMPC_SELF=1 ...   # + dump-c compiler/main.fd (minutes)
#
# 1. tests/selfhost/test_parse_par.fd (ccompile and emit) prints the output
#    in its header: parses on worker threads equal this thread's, the
#    parser's globals untouched, in 1/2/4/8 threads.
# 2. tests/par/parse_ahead_probe.fd on compiler/main.fd: the loop could
#    take every parse ahead and none differs; with `bad` names it takes
#    none that differs, while some would ("blind" > 0).
# 3. `fid check` (import cache off) of programs with imports: the same
#    messages with FID_PAR unset, 2, 4 and 8, FID_PAR=8 five times, and
#    FID_PARSE_AHEAD=off; `dump-c` the same with and without FID_PAR=4.
# 4. FID_PP_VERIFY=1 FID_PAR=8 --time: every parse ahead the loop would
#    take is the loop's own parse ("0 differ", no "[pp] verify" line), and
#    the --time rows say how many were taken.
# 5. The import cache: a cold cache directory with FID_PAR=8 and without
#    gives the same output (the parse ahead runs, pp_imp_cold), and so does
#    the warm second build.
# 6. SELF=1: check compiler/main.fd sequential vs FID_PAR=8 (and verify);
#    DUMPC_SELF=1: dump-c compiler/main.fd too.
# Passes when it prints "parse_par_check: ok".
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-$(mktemp -d)}"
fail=0
export FID_HOME="${FID_HOME:-$(pwd)}"

norm() {
    grep -v -E '^\[prof\]|[0-9]+ ?ms\b|^\[time\]' "$1" || true
}

want_of() {
    sed -n '/^\/\/ Expected output:/,/^$/p' "$1" | sed '1d;$d' | sed 's|^// ||'
}

# 1. the pieces
want_of tests/selfhost/test_parse_par.fd > "$OUT/pp.want"
for how in ccompile emit; do
    if "$FID" $how tests/selfhost/test_parse_par.fd -o "$OUT/pp_$how.exe" > "$OUT/pp_$how.build" 2>&1; then
        ok=1
        for r in 1 2 3; do
            "$OUT/pp_$how.exe" > "$OUT/pp_$how.out$r" 2>&1
            if ! diff "$OUT/pp.want" "$OUT/pp_$how.out$r" > "$OUT/pp_$how.diff$r"; then
                echo "FAIL: test_parse_par ($how, run $r, see $OUT/pp_$how.diff$r)"; fail=1; ok=0
            fi
        done
        [ $ok -eq 1 ] && echo "test_parse_par ($how): as expected x3"
    else
        echo "FAIL: test_parse_par does not build with $how (see $OUT/pp_$how.build)"; fail=1
    fi
done

# 2. the probe on the compiler's own closure
if "$FID" ccompile tests/par/parse_ahead_probe.fd -o "$OUT/probe.exe" > "$OUT/probe.build" 2>&1; then
    "$OUT/probe.exe" compiler/main.fd 8 > "$OUT/probe.out" 2>&1
    grep -v '^time' "$OUT/probe.out"
    if ! grep -q 'differ 0, parsed again 0, blind 0' "$OUT/probe.out"; then
        echo "FAIL: probe: a parse ahead not taken or different (see $OUT/probe.out)"; fail=1
    fi
    "$OUT/probe.exe" compiler/main.fd 8 bad > "$OUT/probe_bad.out" 2>&1
    if ! grep -q 'could take 0, differ 0' "$OUT/probe_bad.out" || grep -q 'blind 0$' "$OUT/probe_bad.out"; then
        echo "FAIL: probe bad: wrong names not caught (see $OUT/probe_bad.out)"; fail=1
    else
        echo "probe bad: $(grep '^loop' "$OUT/probe_bad.out")"
    fi
    grep '^time' "$OUT/probe.out"
else
    echo "FAIL: parse_ahead_probe does not build (see $OUT/probe.build)"; fail=1
fi

# 3. programs with imports, import cache off
export FID_IMPCACHE=off
check_same() {
    local name="$1" f="$2"
    "$FID" check "$f" > "$OUT/$name.seq" 2>&1
    local n
    for n in 2 4 8; do
        FID_PAR=$n "$FID" check "$f" > "$OUT/$name.par$n" 2>&1
        if ! diff <(norm "$OUT/$name.seq") <(norm "$OUT/$name.par$n") > "$OUT/$name.diff$n"; then
            echo "FAIL: $name: FID_PAR=$n output differs (see $OUT/$name.diff$n)"; fail=1
        fi
    done
    for r in 1 2 3 4 5; do
        FID_PAR=8 "$FID" check "$f" > "$OUT/$name.rep$r" 2>&1
        if ! diff <(norm "$OUT/$name.seq") <(norm "$OUT/$name.rep$r") > "$OUT/$name.repdiff$r"; then
            echo "FAIL: $name: FID_PAR=8 run $r differs (see $OUT/$name.repdiff$r)"; fail=1
        fi
    done
    FID_PAR=8 FID_PARSE_AHEAD=off "$FID" check "$f" > "$OUT/$name.off" 2>&1
    if ! diff <(norm "$OUT/$name.seq") <(norm "$OUT/$name.off") > "$OUT/$name.offdiff"; then
        echo "FAIL: $name: FID_PARSE_AHEAD=off differs (see $OUT/$name.offdiff)"; fail=1
    fi
    echo "$name: the same with FID_PAR=2/4/8, 8 x5, parse ahead off"
}
check_same ark compiler/ark.fd
check_same par_escape tests/selfhost/test_par_escape.fd
check_same http2 examples/http2_server.fd
check_same parse_par tests/selfhost/test_parse_par.fd
for f in tests/selfhost/test_par_escape.fd examples/http2_server.fd; do
    b=$(basename "$f" .fd)
    "$FID" dump-c "$f" > "$OUT/$b.c.seq" 2>/dev/null
    FID_PAR=4 "$FID" dump-c "$f" > "$OUT/$b.c.par" 2>/dev/null
    if ! cmp -s "$OUT/$b.c.seq" "$OUT/$b.c.par"; then
        echo "FAIL: dump-c $f differs with FID_PAR=4"; fail=1
    else
        echo "dump-c $b: identical with FID_PAR=4"
    fi
done

# 4. every parse ahead taken is the loop's own parse
FID_PP_VERIFY=1 FID_PAR=8 "$FID" check examples/http2_server.fd --time > "$OUT/verify.out" 2>&1
grep -E 'imports: (parsed ahead|parses ahead used)' "$OUT/verify.out"
if grep -q '^\[pp\] verify' "$OUT/verify.out" || ! grep -q ' 0 differ' "$OUT/verify.out"; then
    echo "FAIL: FID_PP_VERIFY found a difference (see $OUT/verify.out)"; fail=1
fi

# 5. the import cache on: cold and warm, with and without the parse ahead
unset FID_IMPCACHE
mkdir -p "$OUT/ic_seq" "$OUT/ic_par"
FID_IMPCACHE_DIR="$OUT/ic_seq" "$FID" check examples/http2_server.fd --time > "$OUT/ic.seq1" 2>&1
FID_IMPCACHE_DIR="$OUT/ic_par" FID_PAR=8 "$FID" check examples/http2_server.fd --time > "$OUT/ic.par1" 2>&1
FID_IMPCACHE_DIR="$OUT/ic_seq" "$FID" check examples/http2_server.fd > "$OUT/ic.seq2" 2>&1
FID_IMPCACHE_DIR="$OUT/ic_par" FID_PAR=8 "$FID" check examples/http2_server.fd > "$OUT/ic.par2" 2>&1
for r in 1 2; do
    if ! diff <(norm "$OUT/ic.seq$r") <(norm "$OUT/ic.par$r") > "$OUT/ic.diff$r"; then
        echo "FAIL: import cache build $r differs with FID_PAR=8 (see $OUT/ic.diff$r)"; fail=1
    fi
done
if ! grep -q 'imports: parsed ahead' "$OUT/ic.par1"; then
    echo "FAIL: no parse ahead with a cold import cache (see $OUT/ic.par1)"; fail=1
fi
echo "import cache cold/warm: the same with FID_PAR=8"

# 6. the compiler itself
if [ "${SELF:-0}" = "1" ]; then
    export FID_IMPCACHE=off
    "$FID" check compiler/main.fd --time > "$OUT/self.seq" 2>&1
    FID_PAR=8 "$FID" check compiler/main.fd --time > "$OUT/self.par" 2>&1
    FID_PP_VERIFY=1 FID_PAR=8 "$FID" check compiler/main.fd --time > "$OUT/self.verify" 2>&1
    for v in par verify; do
        if ! diff <(norm "$OUT/self.seq" | grep -v '^\[pp\]') <(norm "$OUT/self.$v") > "$OUT/self.diff_$v"; then
            echo "FAIL: compiler/main.fd: FID_PAR=8 ($v) output differs (see $OUT/self.diff_$v)"; fail=1
        fi
    done
    if grep -q '^\[pp\] verify' "$OUT/self.verify" || ! grep -q ' 0 differ' "$OUT/self.verify"; then
        echo "FAIL: compiler/main.fd: FID_PP_VERIFY found a difference (see $OUT/self.verify)"; fail=1
    fi
    echo "sequential:"; grep -E '^\[time\] (  )?imports' "$OUT/self.seq"
    echo "FID_PAR=8:";  grep -E '^\[time\] (  )?imports' "$OUT/self.par"
    echo "verify:";     grep -E 'parses ahead used' "$OUT/self.verify"
    if [ "${DUMPC_SELF:-0}" = "1" ]; then
        "$FID" dump-c compiler/main.fd > "$OUT/self.c.seq" 2>/dev/null
        FID_PAR=8 "$FID" dump-c compiler/main.fd > "$OUT/self.c.par" 2>/dev/null
        if ! cmp -s "$OUT/self.c.seq" "$OUT/self.c.par"; then
            echo "FAIL: dump-c compiler/main.fd differs with FID_PAR=8"; fail=1
        else
            echo "dump-c compiler/main.fd: identical with FID_PAR=8"
        fi
    fi
fi

if [ $fail -eq 0 ]; then
    echo "parse_par_check: ok"
else
    echo "parse_par_check: FAILED (outputs in $OUT)"
    exit 1
fi
