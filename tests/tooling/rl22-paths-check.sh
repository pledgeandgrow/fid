#!/bin/bash
# rl22-paths-check.sh — RL22 (llvm-roadmap: two independent paths, the C
# path and our backend; a release ships only when both agree on the suite).
# Checks the tooling, one line each (OK / FAIL / SKIP); the exit status is
# the number of FAILs:
#
#  1. scripts/verify/rl22-paths.py --selftest: made-up lane folders; the
#     pairs (each Linux target, c:x86_64-windows with both pe_ir and --nat),
#     a lane with no partner, the causes, recorded / not recorded / stale /
#     malformed record lines, the exit status.
#  2. scripts/verify/rl22-recorded.txt has no malformed line, and every
#     line's pair pattern matches a pair rl22-paths.py can form.
#  3. scripts/verify/rl22-release-run.sh and scripts/suite-all.sh parse, and
#     docs/reference/releasing.md still runs `AGREE=1 ... suite-all.sh`.
#  4. FID=<compiler> given: a smoke release run (hello.fd and
#     test_hashmap.fd) on the host's C-path lane and own-backend lane;
#     both must pass and agree, and the record page must be written.
#
# Usage: tests/tooling/rl22-paths-check.sh   (FID=..., CC, OUT=<dir>)
set -u
cd "$(dirname "$0")/../.." || exit 1
OUT="${OUT:-build/rl22-check}"
mkdir -p "$OUT"
bad=0
ok()   { echo "OK   $*"; }
fail() { echo "FAIL $*"; bad=$((bad + 1)); }
skip() { echo "SKIP $*"; }
PY=""
for p in python3 python; do
    if command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then PY=$p; break; fi
done

# 1
if [ -z "$PY" ]; then
    fail "1 no Python 3"
elif "$PY" scripts/verify/rl22-paths.py --selftest > "$OUT/selftest.log" 2>&1; then
    ok "1 $(tail -n 1 "$OUT/selftest.log")"
else
    fail "1 rl22-paths.py --selftest"; sed 's/^/     /' "$OUT/selftest.log"
fi

# 2
if [ -n "$PY" ] && "$PY" - scripts/verify/rl22-recorded.txt > "$OUT/recorded.log" 2>&1 << 'EOF'
import fnmatch, sys
lanes = ["c:x86_64-linux", "c:aarch64-linux", "c:riscv64-linux", "c:x86_64-windows",
         "c:aarch64-macos", "c:x86_64-macos"]
pairs = []
for c in lanes:
    t = c[2:]
    pairs += [c + "|native:" + t, c + "|native:" + t + "-pe"]
bad = 0
for i, line in enumerate(open(sys.argv[1], encoding="utf-8"), 1):
    s = line.strip()
    if not s or s.startswith("#"):
        continue
    p = s.split(None, 2)
    if len(p) < 3:
        print("line %d: needs <pair> <test> <reason>" % i); bad += 1; continue
    if not any(fnmatch.fnmatchcase(x, p[0]) for x in pairs):
        print("line %d: pair %s matches no C-path/own-backend pair" % (i, p[0])); bad += 1
sys.exit(1 if bad else 0)
EOF
then
    ok "2 scripts/verify/rl22-recorded.txt: every line well formed"
else
    fail "2 scripts/verify/rl22-recorded.txt"; sed 's/^/     /' "$OUT/recorded.log"
fi

# 3
if bash -n scripts/verify/rl22-release-run.sh && bash -n scripts/suite-all.sh &&
   grep -q 'AGREE=1 .*scripts/suite-all.sh' docs/reference/releasing.md; then
    ok "3 scripts parse; releasing.md runs AGREE=1 suite-all.sh"
else
    fail "3 rl22-release-run.sh / suite-all.sh / releasing.md"
fi

# 4
if [ -z "${FID:-}" ]; then
    skip "4 FID not set (a smoke release run on hello.fd and test_hashmap.fd)"
else
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) L=(c:x86_64-windows native:x86_64-windows-pe) ;;
        *) L=(c:x86_64-linux native:x86_64-linux) ;;
    esac
    if OUT="$OUT/run" MD="$OUT/run/page.md" JOBS=1 TESTS="tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd" \
         bash scripts/verify/rl22-release-run.sh "${L[@]}" > "$OUT/run.log" 2>&1 &&
       [ -s "$OUT/run/page.md" ] && grep -q 'release gate passes' "$OUT/run/page.md"; then
        ok "4 smoke release run on ${L[*]}: both pass, they agree ($OUT/run/page.md)"
    else
        fail "4 smoke release run on ${L[*]} ($OUT/run.log)"; tail -n 12 "$OUT/run.log" | sed 's/^/     /'
    fi
fi
exit $bad
