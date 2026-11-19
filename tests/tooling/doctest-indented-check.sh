#!/usr/bin/env bash
# doctest-indented-check.sh -- scripts/doctest.py reads `/// Example:` blocks
# whose `///` is indented, i.e. examples on methods inside `impl` blocks
# (competitive-roadmap SL19). Before, its marker regex matched `///` at
# column 0 only, so every method example was silently skipped.
#
# Part 1 (no compiler): doctest.examples() on a fixture finds the method
#   example, the column-0 example and the native-only one, with their code.
# Part 2 (FID set): doctest.py runs the method examples of stdlib modules
#   that have them (buffer, memo, sync) through `fid run`; none may fail and
#   at least 4 must run.
#
# Usage: FID=release/fid tests/tooling/doctest-indented-check.sh
set -u
cd "$(dirname "$0")/../.." || exit 1
PY=${PYTHON:-python3}
command -v "$PY" > /dev/null 2>&1 || PY=python
TMP=$(mktemp -d 2>/dev/null || echo "build/doctest-indented.$$")
mkdir -p "$TMP"
fail=0

cat > "$TMP/fixture.fd" <<'EOF'
module fixture;
/// A top-level function.
///
/// Example:
///   assert_eq(top(), 1);
pub fn top() -> i64 { return 1; }

pub struct Pt { pub x: i64 }

impl Pt {
    /// A method.
    ///
    /// Example:
    ///   val p = Pt::new(3);
    ///   assert_eq(p.x, 3);
    pub fn new(x: i64) -> Pt { return Pt { x: x }; }

    /// Example (native only):
    ///   assert_eq(Pt::new(4).x, 4);
    pub fn other(self: Pt) -> i64 { return self.x; }
}
EOF

"$PY" - "$TMP/fixture.fd" <<'EOF'
import importlib.util, sys
spec = importlib.util.spec_from_file_location("dt", "scripts/doctest.py")
dt = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dt)
ex = dt.examples(sys.argv[1])
got = [(m, n, c) for (m, n, c) in ex]
want = [
    ("fixture", False, ["assert_eq(top(), 1);"]),
    ("fixture", False, ["val p = Pt::new(3);", "assert_eq(p.x, 3);"]),
    ("fixture", True, ["assert_eq(Pt::new(4).x, 4);"]),
]
if got != want:
    print("FAIL part 1: examples() gave", got)
    sys.exit(1)
print("ok part 1: 3 examples, 2 of them indented in an impl block")
EOF
[ $? -eq 0 ] || fail=1

if [ -n "${FID:-}" ]; then
    mkdir -p "$TMP/std"
    cp stdlib/buffer.fd stdlib/memo.fd stdlib/sync.fd "$TMP/std/"
    out=$("$PY" scripts/doctest.py "$TMP/std" --fid "$FID" 2>&1)
    echo "$out"
    if ! echo "$out" | grep -q '| 0 failed |'; then
        echo "FAIL part 2: a method example failed"
        fail=1
    fi
    n=$(echo "$out" | sed -n 's/.*| \([0-9]*\) run |.*/\1/p')
    if [ -z "$n" ] || [ "$n" -lt 4 ]; then
        echo "FAIL part 2: expected at least 4 examples run, got '${n}'"
        fail=1
    fi
else
    echo "skip part 2 (set FID to run the stdlib method examples)"
fi

rm -rf "$TMP"
if [ $fail -ne 0 ]; then
    echo "doctest-indented-check: FAIL"
    exit 1
fi
echo "doctest-indented-check: PASS"
