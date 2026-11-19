#!/bin/bash
# tooling-goals-check.sh -- the tool goals of docs/roadmap/release-roadmap.md
# (track N) and docs/roadmap/complaint-roadmap.md done on Nov 15:
#   N7   fid lsp: references, formatting, signature help (scripts/tl/lsp_probe.py)
#   N12  fid --version (and fid version) names the exact build: exe:<hash>
#   CT10 fid run --trace f,g: each call of f and g with its arguments and
#        result on stderr (tests/bootstrap/test_tooling_trace.fd)
#   CT2  fid test --filter <text> and --jobs <n> (scripts/tooling/ct2_tests.fd)
#   CT3  fid test: FIDES_TEST_NOW / FIDES_TEST_SEED fixed per test, unless
#        --real-clock or a test named *_real_clock (scripts/tooling/ct3_clock_tests.fd)
#   CB9  ccompile: a symbol no library provides is a Fides error with the
#        line of its extern "C" declaration (scripts/tooling/cb9_link.fd)
#   CU3  a @deprecated call warns on a run; fid fix replaces it with the
#        named function (tests/bootstrap/test_tooling_fix_deprecated.fd)
#   CD6  ark add warns about a name one edit away from a known one
#   CD9  ark vendor: vendor/ + vendor/vendor.lock, `use` finds vendor/ with
#        no ark_modules/, ark install checks the copy, ark audit sees a change
#   CD7  ark audit lists the C libraries and capabilities of a pact
#   N6   ark add --version / install / search / publish against a local
#        Git index (ARK_INDEX, ARK_HOME in a scratch directory); ark yank
#        through tests/ark/ark-registry.sh
#   CA10 fid lint warns of a blocking call in a green task
#        (tests/tooling/ca10-blocking-check.sh)
#   CD8  ark install runs none of a pact's code (tests/ark/cd8-no-install-code.sh)
#
# Usage:
#   scripts/tooling-goals-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
# Needs git (N6, a local index) and python3 (N7); each is skipped without.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
DIR="$ROOT/build/tooling-goals-check"
mkdir -p "$DIR"
fail=0
# ark keeps its index copy under ARK_HOME: a scratch one, not the user's
export ARK_HOME="$DIR/ark-home"
mkdir -p "$ARK_HOME"

ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $2"; fi
}
hasnt() { # hasnt <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $2"; else ok "$1"; fi
}
same() { # same <label> <file> <text>
  if [ "$(tr -d '\r' < "$2")" = "$3" ]; then ok "$1"; else bad "$1 (see $2)"; fi
}

echo "== N7 fid lsp (references, formatting, signature help)"
if command -v python3 > /dev/null; then
  python3 scripts/tl/lsp_probe.py "$FID" > "$DIR/lsp.out" 2>&1 || fail=1
  cat "$DIR/lsp.out"
else
  echo "  skip  python3 not found"
fi

echo "== N12 fid --version names the exact build"
"$FID" --version > "$DIR/version.out" 2>&1
has "--version works" "$DIR/version.out" "fid "
has "the build's hash" "$DIR/version.out" "build: exe:"
"$FID" version > "$DIR/version2.out" 2>&1
if cmp -s "$DIR/version.out" "$DIR/version2.out"; then ok "same as fid version"; else bad "fid --version and fid version differ"; fi

echo "== CT10 fid run --trace"
T=tests/bootstrap/test_tooling_trace.fd
"$FID" run "$T" --trace add,Point::norm2,greet > "$DIR/trace.out" 2> "$DIR/trace.err"
same "output unchanged" "$DIR/trace.out" "$(printf '7\n25\nhi ann')"
has "call with arguments" "$DIR/trace.err" "add(3, 4)"
has "its result" "$DIR/trace.err" "add = 7"
has "a method by T::m" "$DIR/trace.err" "Point_norm2("
has "a string argument quoted" "$DIR/trace.err" "greet(\"ann\")"
has "a string result quoted" "$DIR/trace.err" "greet = \"hi ann\""
hasnt "main is not traced" "$DIR/trace.err" "[trace] main("
"$FID" run "$T" > /dev/null 2> "$DIR/notrace.err"
hasnt "no trace without --trace" "$DIR/notrace.err" "[trace]"

echo "== CT2 fid test --filter / --jobs"
C=scripts/tooling/ct2_tests.fd
"$FID" test "$C" --filter add > "$DIR/ct2-filter.out" 2> "$DIR/ct2-filter.err"
has "filter runs test_add_small" "$DIR/ct2-filter.out" "ok    test_add_small"
has "filter runs test_add_big" "$DIR/ct2-filter.out" "ok    test_add_big"
hasnt "filter leaves out test_sub" "$DIR/ct2-filter.out" "test_sub"
has "filtered count" "$DIR/ct2-filter.out" "2 passed, 0 failed"
"$FID" test "$C" --jobs 3 > "$DIR/ct2-jobs.out" 2> "$DIR/ct2-jobs.err"
echo "status=$?" >> "$DIR/ct2-jobs.out"
has "parallel: a test's output" "$DIR/ct2-jobs.out" "add small"
has "parallel: the failing test fails" "$DIR/ct2-jobs.out" "FAIL  test_mul_fails"
has "parallel: the count" "$DIR/ct2-jobs.out" "3 passed, 1 failed"
has "parallel: the run fails" "$DIR/ct2-jobs.out" "status=1"
if [ "$(grep -n 'add small' "$DIR/ct2-jobs.out" | cut -d: -f1)" -lt "$(grep -n 'ok    test_add_small' "$DIR/ct2-jobs.out" | cut -d: -f1)" ]; then
  ok "parallel: output before its verdict"
else
  bad "parallel: output not before its verdict (see $DIR/ct2-jobs.out)"
fi

echo "== CT3 fid test fixes the clock and the seed"
K=scripts/tooling/ct3_clock_tests.fd
env -u FIDES_TEST_NOW -u FIDES_TEST_SEED "$FID" test "$K" > "$DIR/ct3.out" 2> "$DIR/ct3.err"
has "fixed clock and seed, real clock by name" "$DIR/ct3.out" "3 passed, 0 failed"
env -u FIDES_TEST_NOW -u FIDES_TEST_SEED "$FID" test "$K" --real-clock > "$DIR/ct3-real.out" 2> "$DIR/ct3-real.err"
has "--real-clock: the fixed-clock test fails" "$DIR/ct3-real.out" "FAIL  test_clock_is_fixed"
has "--real-clock: the real-clock test passes" "$DIR/ct3-real.out" "ok    test_clock_real_clock"
env -u FIDES_TEST_NOW -u FIDES_TEST_SEED "$FID" test "$K" --jobs 3 > "$DIR/ct3-jobs.out" 2> "$DIR/ct3-jobs.err"
has "the same with --jobs 3" "$DIR/ct3-jobs.out" "3 passed, 0 failed"

echo "== CB9 a link error as a Fides error"
L=scripts/tooling/cb9_link.fd
"$FID" ccompile "$L" -o "$DIR/cb9.exe" > "$DIR/cb9.out" 2> "$DIR/cb9.err"
echo "status=$?" >> "$DIR/cb9.out"
if grep -qiE "undefined|unresolved" "$DIR/cb9.err"; then
  has "names the missing function" "$DIR/cb9.err" "error: link: the C function 'fides_cb9_missing' is not in any library the program links"
  has "and its declaration" "$DIR/cb9.err" "cb9_link.fd:9: declared in an extern \"C\" block that names no library"
  has "the build fails" "$DIR/cb9.out" "status=1"
else
  echo "  skip  no C compiler reached the link (see $DIR/cb9.err)"
fi

echo "== CU3 @deprecated: warning on run, fid fix applies the replacement"
D=tests/bootstrap/test_tooling_fix_deprecated.fd
"$FID" run "$D" > "$DIR/dep-run.out" 2> "$DIR/dep-run.err"
has "a run warns" "$DIR/dep-run.err" "'area' is deprecated: use \`area_of\` instead"
cp "$D" "$DIR/dep_me.fd"
"$FID" fix "$DIR/dep_me.fd" > "$DIR/dep-fix.out" 2> "$DIR/dep-fix.err"
has "fix names the replacement" "$DIR/dep-fix.out" "call 'area_of' instead of the deprecated 'area'"
if sed -n '/^fn main/,$p' "$DIR/dep_me.fd" | grep -q "area("; then
  bad "a call of area( is left in main (see $DIR/dep_me.fd)"
else
  ok "every call in main replaced"
fi
"$FID" run "$DIR/dep_me.fd" > "$DIR/dep-fixed.out" 2> "$DIR/dep-fixed.err"
same "the fixed program prints the same" "$DIR/dep-fixed.out" "$(printf '6\n20')"
hasnt "and warns no more" "$DIR/dep-fixed.err" "is deprecated"

echo "== CD6 / CD7 / CD9 ark: typosquat warning, audit, vendor"
P="$DIR/ark"
rm -rf "$P"
mkdir -p "$P/app" "$P/pacts"
cat > "$P/pacts/greet.fd" << 'EOF'
module greet;

pub fn greet_twice(n: i64) -> i64 {
    return n * 2;
}
EOF
# a pact main.fd does not use, with a C library for the audit to list
cat > "$P/pacts/clib.fd" << 'EOF'
module clib;

extern "C" link "m" {
    fn cos(x: f64) -> f64;
}
EOF
cat > "$P/app/main.fd" << 'EOF'
use greet;

fn main() -> i64 {
    print(greet_twice(21));
    print("\n");
    return 0;
}
EOF
(
  cd "$P/app" || exit 1
  "$FID" ark init app > ../init.out 2>&1
  "$FID" ark add greet --path ../pacts/greet.fd > ../add.out 2>&1
  "$FID" ark add clib --path ../pacts/clib.fd > /dev/null 2>&1
  "$FID" ark add greeet --path ../pacts/greet.fd > ../add2.out 2>&1
  "$FID" ark remove greeet > /dev/null 2>&1
  "$FID" ark install > ../install.out 2>&1
  "$FID" ark audit > ../audit.out 2>&1
  "$FID" ark vendor > ../vendor.out 2>&1
  echo "vendor=$?" >> ../vendor.out
  rm -f ark_modules/greet.fd
  "$FID" run main.fd > ../run-vendored.out 2> ../run-vendored.err
  "$FID" ark install > ../install-vendored.out 2>&1
  "$FID" ark audit > ../audit-vendored.out 2>&1
  echo "audit=$?" >> ../audit-vendored.out
  echo "// changed" >> vendor/greet.fd
  "$FID" ark audit > ../audit-changed.out 2>&1
  echo "audit=$?" >> ../audit-changed.out
  "$FID" ark install > ../install-changed.out 2>&1
  echo "install=$?" >> ../install-changed.out
)
has "typosquat warning" "$P/add2.out" "'greeet' is one edit away from 'greet'"
hasnt "no warning for the first name" "$P/add.out" "one edit away"
has "audit lists the C library" "$P/audit.out" "C libraries: m"
has "vendor ran" "$P/vendor.out" "vendor=0"
if [ -f "$P/app/vendor/greet.fd" ]; then ok "vendor/greet.fd written"; else bad "no vendor/greet.fd"; fi
has "vendor.lock has the hash" "$P/app/vendor/vendor.lock" "resolved = \"fnv:"
same "use finds vendor/ without ark_modules/" "$P/run-vendored.out" "42"
has "install takes the vendored copy" "$P/install-vendored.out" "OK (vendored)"
has "audit of the vendored copy" "$P/audit-vendored.out" "vendored  fnv:"
has "audit clean" "$P/audit-vendored.out" "audit=0"
has "audit sees a changed copy" "$P/audit-changed.out" "CHANGED: vendor/vendor.lock has"
has "audit fails on it" "$P/audit-changed.out" "audit=1"
has "install refuses it" "$P/install-changed.out" "install=1"

echo "== CA10 fid lint: a blocking call in a green task"
if bash tests/tooling/ca10-blocking-check.sh "$FID" > "$DIR/ca10.out" 2>&1; then
  ok "tests/tooling/ca10-blocking-check.sh"
else
  bad "tests/tooling/ca10-blocking-check.sh (see $DIR/ca10.out)"
  grep -v "^ok" "$DIR/ca10.out" | head -10
fi

echo "== CD8 ark install runs none of a pact's code"
if command -v git > /dev/null; then
  if bash tests/ark/cd8-no-install-code.sh "$FID" ark > "$DIR/cd8.out" 2>&1; then
    ok "tests/ark/cd8-no-install-code.sh"
  else
    bad "tests/ark/cd8-no-install-code.sh (see $DIR/cd8.out)"
    grep "FAIL" "$DIR/cd8.out" | head -10
  fi
else
  echo "  skip  git not found"
fi

echo "== N6 ark registry on a local index"
if command -v git > /dev/null; then
  R="$DIR/registry"
  rm -rf "$R"
  mkdir -p "$R/home" "$R/pkg" "$R/app"
  export ARK_HOME="$R/home"
  export ARK_INDEX="$R/index.git"
  G="git -c user.name=ark-check -c user.email=ark-check@example.invalid"
  # the package: a git repository tagged v1.0.0
  (
    cd "$R/pkg" || exit 1
    git init -q .
    printf 'module shapes;\n\npub fn sq(x: i64) -> i64 {\n    return x * x;\n}\n' > lib.fd
    printf '[package]\nname = "shapes"\nversion = "1.0.0"\nedition = "2026"\n' > ark.toml
    git add . && $G commit -q -m v1 && git tag v1.0.0
  )
  # the index: a bare repository with one commit
  git init -q --bare "$R/index.git"
  (
    cd "$R" || exit 1
    git clone -q index.git seed 2> /dev/null
    cd seed || exit 1
    printf '# ark index\n' > README
    git add README && $G commit -q -m init && git push -q origin HEAD 2> /dev/null
  )
  # publish v1.0.0 from the package (the push goes to the bare index)
  (cd "$R/pkg" && "$FID" ark publish --git "$R/pkg" > ../publish.out 2>&1; echo "publish=$?" >> ../publish.out)
  has "publish" "$R/publish.out" "Published shapes 1.0.0."
  (cd "$R/pkg" && "$FID" ark publish --git "$R/pkg" > ../publish2.out 2>&1)
  has "a published version is never changed" "$R/publish2.out" "already published"
  (cd "$R/app" && "$FID" ark search sha > ../search.out 2>&1)
  has "search finds it" "$R/search.out" "shapes 1.0.0"
  cat > "$R/app/main.fd" << 'EOF'
use shapes;

fn main() -> i64 {
    print(sq(7));
    print("\n");
    return 0;
}
EOF
  (
    cd "$R/app" || exit 1
    "$FID" ark init app > /dev/null 2>&1
    "$FID" ark add shapes --version 1.0 > ../radd.out 2>&1
    "$FID" ark install > ../rinstall.out 2>&1
    "$FID" run main.fd > ../rrun.out 2> ../rrun.err
    "$FID" ark audit > ../raudit.out 2>&1
    echo "audit=$?" >> ../raudit.out
  )
  has "registry install" "$R/rinstall.out" "OK (registry"
  has "locked as registry" "$R/app/ark.lock" "kind = \"registry\""
  same "use finds the registry pact" "$R/rrun.out" "49"
  has "audit of a registry pact" "$R/raudit.out" "audit=0"
  # a tag moved off the published commit is refused
  (
    cd "$R/pkg" || exit 1
    echo "// moved" >> lib.fd
    git add . && $G commit -q -m moved && git tag -f v1.0.0 > /dev/null
  )
  rm -rf "$R/app/ark_modules" "$R/app/ark.lock"
  (cd "$R/app" && "$FID" ark install > ../rmoved.out 2>&1; echo "install=$?" >> ../rmoved.out)
  has "a moved tag is refused" "$R/rmoved.out" "refusing to install"
  has "and the install fails" "$R/rmoved.out" "install=1"
  unset ARK_HOME ARK_INDEX
  # N6 ark yank: a yanked version is not chosen for a new lock, a lock that
  # records it keeps it (with a warning), search skips it, --undo restores
  # it; tests/ark/ark-registry.sh holds those steps (and L5F, TL9, CD1)
  if bash tests/ark/ark-registry.sh "$FID" ark > "$DIR/ark-registry.out" 2>&1; then
    ok "ark yank (tests/ark/ark-registry.sh)"
  else
    bad "ark yank (tests/ark/ark-registry.sh): see $DIR/ark-registry.out"
    grep "FAIL" "$DIR/ark-registry.out" | head -10
  fi
else
  echo "  skip  git not found"
fi

if [ "$fail" = 0 ]; then echo "tooling-goals-check: all passed"; else echo "tooling-goals-check: FAILED"; fi
exit "$fail"
