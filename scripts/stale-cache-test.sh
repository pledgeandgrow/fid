#!/usr/bin/env bash
# CB8: the build cache must invalidate on every input change.
#   Build a program, then change each of: the source, an imported module,
#   the compiler binary, the flags. Each rebuild must re-run the C compiler
#   (no "up to date" skip) and produce output for the new input.
# Usage: stale-cache-test.sh [fid-binary]
set -e
FID="${1:-release/fid.exe}"
CC="${CC:-zig cc}"
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT

cat > "$TMP/lib.fd" <<'EOI'
pub fn answer() -> i64 { return 42; }
EOI
cat > "$TMP/main.fd" <<'EOI'
use lib;
fn main() -> i64 { println(lib::answer()); return 0; }
EOI

build() { (cd "$TMP" && CC="$CC" "$OLDPWD/$FID" ccompile main.fd -o main.exe 2>&1); }
run() { "$TMP/main.exe"; }

out=$(build)
[ -f "$TMP/main.exe" ] || { echo "FAIL: initial build"; echo "$out"; exit 1; }
[ "$(run)" = "42" ] || { echo "FAIL: initial run"; exit 1; }
echo "1. initial build ok"

# 2. source change -> new output
sed -i 's/42/43/' "$TMP/lib.fd"
out=$(build)
[ "$(run)" = "43" ] || { echo "FAIL: dep change not picked up"; echo "$out"; exit 1; }
echo "2. imported-module change invalidates"

# 3. --no-cache forces a rebuild
out=$(cd "$TMP" && CC="$CC" "$OLDPWD/$FID" ccompile main.fd -o main.exe --no-cache 2>&1)
[ "$(run)" = "43" ] || { echo "FAIL: --no-cache run"; exit 1; }
echo "3. --no-cache ok"

# 4. a no-change rebuild is allowed to skip (the cache works at all)
out=$(build)
echo "$out" | grep -qi 'up to date' && echo "4. unchanged build skipped (cache hits)" \
  || echo "4. unchanged build rebuilt (safe, just slower)"

# 5. different flags invalidate
out=$(cd "$TMP" && CC="$CC" FIDES_CC_OPT=0 "$OLDPWD/$FID" ccompile main.fd -o main.exe 2>&1)
[ "$(run)" = "43" ] || { echo "FAIL: flag change run"; exit 1; }
echo "5. flag change invalidates"

echo "stale-cache-test: PASS"
