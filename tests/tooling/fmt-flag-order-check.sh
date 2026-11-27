#!/usr/bin/env bash
# fmt-flag-order-check.sh — `fid fmt` takes its flags before or after the
# file (BG237, docs/roadmap/bugs-roadmap.md): `fid fmt --check file.fd` read
# the flag as the file name ("could not read file: --check").
#
#   FID=release/fid.exe bash tests/tooling/fmt-flag-order-check.sh
#   OUT=<dir>   default build/fmt-flag-order-check
# Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/fmt-flag-order-check}"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

cat > "$OUT/plain.fd" <<'EOF'
fn main() -> i64 {
    val x = 1;
    return x;
}
EOF
# the same tokens, another spacing
cat > "$OUT/rough.fd" <<'EOF'
fn main() -> i64 {
    val  x=1;
    return x;
}
EOF

# check <name> <want rc> <want text in stderr> <command...>
check() {
    local name="$1" want_rc="$2" want="$3"; shift 3
    local rc err
    err=$("$@" 2>&1 >/dev/null); rc=$?
    err=$(printf '%s' "$err" | tr -d '\r')
    if [ "$rc" = "$want_rc" ] && printf '%s' "$err" | grep -q -F -- "$want"; then
        ok "$name"
    else
        bad "$name: exit $rc (want $want_rc), stderr [$err] (want it to hold [$want])"
    fi
}

check "flag before the file: formatted" 0 "already formatted" "$FID" fmt --check "$OUT/plain.fd"
check "flag before the file: would reformat" 1 "would reformat" "$FID" fmt --check "$OUT/rough.fd"
check "flag after the file: would reformat" 1 "would reformat" "$FID" fmt "$OUT/rough.fd" --check
check "no file: the usage" 1 "Usage: fid fmt" "$FID" fmt --check

# -w before the file writes it
cp "$OUT/rough.fd" "$OUT/copy.fd"
"$FID" fmt -w "$OUT/copy.fd" >/dev/null 2>&1
if cmp -s <(tr -d '\r' < "$OUT/copy.fd") <(tr -d '\r' < "$OUT/plain.fd"); then
    ok "-w before the file formats it"
else
    bad "-w before the file: the file was not formatted"
fi

# without a flag the formatted source goes to stdout
if "$FID" fmt "$OUT/rough.fd" 2>/dev/null | tr -d '\r' | cmp -s - <(tr -d '\r' < "$OUT/plain.fd"); then
    ok "no flag: the formatted source on stdout"
else
    bad "no flag: stdout is not the formatted source"
fi

if [ "$fails" -gt 0 ]; then
    echo "fmt-flag-order-check: $fails failed"
    exit 1
fi
echo "fmt-flag-order-check: all passed"
