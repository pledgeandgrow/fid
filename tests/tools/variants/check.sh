#!/usr/bin/env bash
# tools/variants.fd (roadmap AD4) prints and writes what
# scripts/verify/variants.py does: both run with the same arguments, each
# with its own --out, and their stdout+stderr, exit status,
# <out>/<cmd>-report.txt and --resume results (sorted: children finish in
# any order) are compared, the --out directories' names masked ("\r\n"
# from Python on Windows counts as "\n").
#
#   bash tests/tools/variants/check.sh       # a few minutes
#
# Nothing here builds a compiler (build-variant.sh takes minutes and
# gigabytes each): the run-time switches run on $FID itself, and the
# compilers of the constant switches and mutants are put in <out> before
# the run, where both look for them first — a copy of $FID (a mutant that
# survives) or tests/tools/variants/fakecc.fd (every test's output changes:
# killed, or DIFFER for a toggle) — but one, whose build from a tree with
# no sources fails at once (BUILD FAILED, exit 2). The tests are sample/
# (one the passes change without changing its output, one with an
# `//@ env:` line, one whose output changes from run to run — flaky — and
# one that does not compile) and 30 tiny generated ones for --sample.
# The cases: list and check on the tree and on an empty one; toggles on
# emit and nat with -j 2; --prune-stats with --resume, twice (the second
# run resumes everything); --sample (both of random.sample's ways) and
# --no-fail-tests; mutants killed, surviving and not built; a constant
# toggle that differs; an --only that matches nothing; argparse's errors.
#
# FID: the compiler (default release/fid.exe; a path Python can start too,
# so on Windows not a /c/... one); it also builds the tool and the stand-in.
# Run from the repository root; needs python3 (or python) and bash.
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-variants-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
X=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
exe="$OUT/variants$X"
fake="$OUT/fakecc$X"
if ! "$FID" ccompile tools/variants.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/variants.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
"$FID" ccompile tests/tools/variants/fakecc.fd -o "$fake" > "$OUT/build-fake.txt" 2>&1 \
    || { echo "FAIL: fakecc.fd does not build"; tail -5 "$OUT/build-fake.txt"; exit 1; }
printf 'fn main() -> i64 {\n    println("a mutant wrote this");\n    return 0;\n}\n' > "$OUT/mutant.fd"
"$FID" emit "$OUT/mutant.fd" -o "$OUT/mutant$X" > /dev/null 2>&1 || { echo "FAIL: mutant.fd"; exit 1; }
export FAKE_EXE="$OUT/mutant$X"
# a tree with no sources (list finds no switches, check finds no edits,
# a build fails at once)
mkdir -p "$OUT/emptytree/compiler" "$OUT/emptytree/stdlib"
# 30 tiny tests for --sample's set-based draw (more than 21 files)
mkdir -p "$OUT/many"
for k in $(seq -w 0 29); do
    printf 'fn main() -> i64 {\n    println(%d);\n    return 0;\n}\n' "$((10#$k * 7))" > "$OUT/many/t$k.fd"
done
# the folder as the tools see it: Git Bash does not turn a /c/... path
# holding a * into a Windows one
MANY="$OUT/many"
command -v cygpath > /dev/null 2>&1 && MANY="$(cygpath -m "$OUT/many")"
pass=0
fail=0
# compare <name> <args...>: both get the args and --out <OUT>/<name>.py.d
# or <name>.fd.d (made fresh unless KEEP=1; PRE="<file>:<source> ..." are
# copied in first)
compare() {
    local name="$1"; shift
    local d
    for d in py fd; do
        if [ "${KEEP:-0}" != 1 ]; then rm -rf "$OUT/$name.$d.d"; fi
        mkdir -p "$OUT/$name.$d.d"
        for p in ${PRE:-}; do cp "${p#*:}" "$OUT/$name.$d.d/${p%%:*}"; done
    done
    "$PY" scripts/verify/variants.py "$@" --out "$OUT/$name.py.d" > "$OUT/$name.py" 2>&1
    local pc=$?
    "$exe" "$@" --out "$OUT/$name.fd.d" > "$OUT/$name.fd" 2>&1
    local fc=$?
    local bad=0
    norm() { sed -e "s#$name\.py\.d#OUTDIR#g; s#$name\.fd\.d#OUTDIR#g" -e 's/\r$//' "$1"; }
    diff <(norm "$OUT/$name.py") <(norm "$OUT/$name.fd") > "$OUT/$name.diff" || bad=1
    [ $pc = $fc ] || bad=1
    local f
    for f in toggle-report.txt mutate-report.txt; do
        if [ -f "$OUT/$name.py.d/$f" ] || [ -f "$OUT/$name.fd.d/$f" ]; then
            diff <(norm "$OUT/$name.py.d/$f") <(norm "$OUT/$name.fd.d/$f") >> "$OUT/$name.diff" 2>&1 \
                || { bad=1; echo "  ($f differs)" >> "$OUT/$name.diff"; }
        fi
    done
    for f in toggle-results.jsonl mutate-results.jsonl; do
        if [ -f "$OUT/$name.py.d/$f" ] || [ -f "$OUT/$name.fd.d/$f" ]; then
            diff <(norm "$OUT/$name.py.d/$f" | sed 's#\\\\#/#g' | sort) <(norm "$OUT/$name.fd.d/$f" | sed 's#\\\\#/#g' | sort) \
                >> "$OUT/$name.diff" 2>&1 || { bad=1; echo "  ($f differs)" >> "$OUT/$name.diff"; }
        fi
    done
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
T='tests/tools/variants/sample/*.fd'
compare list list
compare check check
compare check-empty check --tree "$OUT/emptytree"
compare list-empty list --tree "$OUT/emptytree"
compare toggle toggle --fid "$FID" --tests "$T" --only 'iropt-fold,iropt-dce,iropt-gvn' --backends emit -j 2
compare toggle-nat toggle --fid "$FID" --tests "$T" --only 'nat-rules,iropt-licm' --backends nat -j 2
compare prune toggle --fid "$FID" --tests "$T" --only 'iropt-sccp,iropt-tailcall,iropt-dse' --backends emit \
    --prune-stats --prune-check 0.5 --resume -j 3
KEEP=1 compare prune toggle --fid "$FID" --tests "$T" --only 'iropt-sccp,iropt-tailcall,iropt-dse' --backends emit \
    --prune-stats --prune-check 0.5 --resume -j 3
compare sample toggle --fid "$FID" --tests "$MANY/*.fd" --sample 2 --only iropt-licm --backends emit --resume
compare sample-pool toggle --fid "$FID" --tests "$T,$MANY/t0*.fd" --sample 4 --no-fail-tests \
    --only iropt-licm --backends emit --resume -j 4
PRE="fid-base.exe:$FID fid-fold-lt-as-le.exe:$fake fid-gvn-ignores-rhs.exe:$FID" \
    compare mutate mutate --fid "$FID" --tree "$OUT/emptytree" --tests "$T" \
    --only 'fold-lt-as-le,gvn-ignores-rhs,dce-all-pure' --backends emit -j 3
PRE="fid-base.exe:$FID fid-peir-rotate.exe:$fake" \
    compare const-toggle toggle --fid "$FID" --tree "$OUT/emptytree" --tests "$T" --only 'peir-*' --backends emit
compare nomatch toggle --fid "$FID" --tests "$T" --only 'zzz-*' --backends emit
compare noargs
compare help -h
compare badcmd bogus
compare badint toggle -j x
compare extra list extra
# the port's TOGGLES and MUTANTS are copies: every field (the edits' `to`
# text included, which neither list nor check shows) must be the script's
if "$PY" - <<'PY'
import re, sys
sys.path.insert(0, "scripts/verify")
import variants as v
src = open("tools/variants.fd", encoding="utf-8").read()
lit = r'"((?:[^"\\]|\\.)*)"'
def un(s):
    return re.sub(r"\\(.)", lambda m: {"n": "\n", "t": "\t"}.get(m.group(1), m.group(1)), s)
def bl(b1, b2):
    return [un(b1)] + ([un(b2)] if b2 else [])
blist = r'va_b[12]\(' + lit + r'(?:, ' + lit + r')?\)'
env = [(un(n), "env", un(h), bl(b1, b2)) for n, h, b1, b2 in
       re.findall(r'va_env\(' + lit + ', ' + lit + ', ' + blist + r'\)\);', src)]
const = [(un(n), (un(f), un(fr), un(to)), bl(b1, b2)) for n, f, fr, to, b1, b2 in
         re.findall(r'va_const\(' + lit + ', ' + lit + r',\s*' + lit + r',\s*' + lit + ', ' + blist + r'\)\);', src)]
toggles = [t for t in v.TOGGLES]
want_t = [(n, k, h, b) for n, k, h, b in toggles]
got_t = []
ei = ci = 0
for n, k, h, b in toggles:
    if k == "env":
        got_t.append(env[ei]); ei += 1
    else:
        c = const[ci]; ci += 1
        got_t.append((c[0], "const", c[1], c[2]))
got_m = [tuple(c) for c in const[ci:]]
ok = got_t == want_t and got_m == [tuple(m) for m in v.MUTANTS] and ei == len(env)
sys.exit(0 if ok else 1)
PY
then
    pass=$((pass + 1)); echo "ok   tables (TOGGLES and MUTANTS are the script's)"
else
    fail=$((fail + 1)); echo "FAIL tables: tools/variants.fd's TOGGLES or MUTANTS differ from the script's"
fi
echo "variants port: $pass same, $fail differ"
[ $fail = 0 ]
