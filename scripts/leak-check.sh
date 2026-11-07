#!/bin/bash
# leak-check.sh [FID] [FILE...]: ownership gate (proposal 0004).
#
# Builds each program through C with the leak report (fid run --leaks) and
# fails when its output differs from <name>.expected or when any block is
# still allocated at exit. With no files, runs every tests/leaks/*.fd.
# JOBS programs at once (default 1: `fid run --leaks` builds in a fixed
# temporary file in the current folder, which must be the project root).
#
#   ./scripts/leak-check.sh                       # release/fid.exe, tests/leaks
#   ./scripts/leak-check.sh release/fid-dev.exe tests/bootstrap/test_x.fd
set -u
FID="${1:-release/fid.exe}"
shift || true
JOBS="${JOBS:-1}"
FILES=("$@")
if [ ${#FILES[@]} -eq 0 ]; then FILES=(tests/leaks/*.fd); fi
OUT=$(mktemp -d)

check_one() {
    local fid="$1" f="$2" out="$3"
    local name; name=$(basename "$f" .fd)
    local dir; dir=$(dirname "$f")
    "$fid" run "$f" --leaks 2> "$out/$name.err" | grep -v -e '^Built native executable' -e '^Reused cached artifact' > "$out/$name.out"
    local status="ok"
    if [ -f "$dir/$name.expected" ] && ! diff -q --strip-trailing-cr "$dir/$name.expected" "$out/$name.out" > /dev/null; then
        status="output differs"
    fi
    if ! grep -q "every block was freed" "$out/$name.err"; then
        status="$status; leaks: $(grep -m1 'fid --leaks' "$out/$name.err" || echo 'no leak report')"
    fi
    echo "$status" > "$out/$name.status"
}
export -f check_one

printf '%s\n' "${FILES[@]}" | xargs -P "$JOBS" -I{} bash -c 'check_one "$0" "$1" "$2"' "$FID" {} "$OUT"

fail=0
for f in "${FILES[@]}"; do
    name=$(basename "$f" .fd)
    st=$(cat "$OUT/$name.status" 2>/dev/null || echo "did not run")
    if [ "$st" = "ok" ]; then
        echo "  ok    $name"
    else
        echo "  FAIL  $name: $st"
        fail=$((fail + 1))
    fi
done
echo "leak-check: ${#FILES[@]} program(s), $fail failed (reports in $OUT)"
[ $fail -eq 0 ]
