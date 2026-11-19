#!/bin/bash
# CB2 (docs/roadmap/complaint-roadmap.md): a long-lived checker (`fid
# serve`, also `fid fix` and `fid lsp`) does not lex or parse again an
# imported module whose file has not changed since the last request
# (compiler/compiler.fd modcache_get); a changed one is parsed again.
# Checks, on a two-module program in a temporary directory:
#   1. the first check is clean and reuses nothing;
#   2. the second check (only main.fd edited) is clean and reports reused
#      module parses;
#   3. after a type error is written into the imported module, the next
#      check reports it (the stale parse is not used);
#   4. after the module is fixed again, the check is clean.
#   FID=release/fid.exe bash tests/tooling/serve-modcache-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
# the paths go inside request lines, which Git Bash does not convert
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/cbhelp.fd" <<'EOF2'
module cbhelp;
pub fn cbhelp_twice(x: i64) -> i64 {
    return x * 2;
}
EOF2
cat > "$TMP/main.fd" <<'EOF2'
use cbhelp;
fn main() -> i64 {
    println(int_to_str(cbhelp_twice(3)));
    return 0;
}
EOF2
# The requests go one at a time: each edit is made after the answer before
# it (`done <status>`), so the files are read in the intended order.
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
coproc SRV { "$FID" serve 2>/dev/null; }
ask() {
    echo "check $TMP/main.fd" >&"${SRV[1]}"
    local out="" l
    while IFS= read -r l <&"${SRV[0]}"; do
        l=${l%$'\r'}
        out="$out$l"$'\n'
        case "$l" in done*) break ;; esac
    done
    printf '%s' "$out"
}
A1=$(ask)
if echo "$A1" | grep -q "no errors" && ! echo "$A1" | grep -q "reused"; then ok "1. first check clean, nothing reused"; else bad "1. first check: $A1"; fi
sed -i 's/cbhelp_twice(3)/cbhelp_twice(4)/' "$TMP/main.fd"
A2=$(ask)
if echo "$A2" | grep -q "no errors" && echo "$A2" | grep -q "module parses reused"; then ok "2. second check reuses the module's parse"; else bad "2. second check: $A2"; fi
sed -i 's/return x \* 2;/return "two";/' "$TMP/cbhelp.fd"
A3=$(ask)
if echo "$A3" | grep -q "^done 1"; then ok "3. an edit of the imported module is seen"; else bad "3. after editing the module: $A3"; fi
sed -i 's/return "two";/return x * 2;/' "$TMP/cbhelp.fd"
A4=$(ask)
if echo "$A4" | grep -q "no errors"; then ok "4. the module fixed again checks clean"; else bad "4. after fixing the module: $A4"; fi
echo "quit" >&"${SRV[1]}"
wait 2>/dev/null
if [ $fail = 0 ]; then echo "serve-modcache-check: PASS"; else echo "serve-modcache-check: FAIL"; fi
exit $fail
