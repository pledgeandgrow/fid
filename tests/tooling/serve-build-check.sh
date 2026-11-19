#!/bin/bash
# SP18 (docs/roadmap/llvm-roadmap.md): `fid serve` answers `emit` and
# `ccompile` requests with the same output as the one-shot commands
# (compiler/main.fd serve_build: the request's words stand for argv,
# inc.fd argv_at; compiler.fd serve_reset_build_state after each one).
# Checks, on two programs in a temporary directory:
#   1. `emit` through the server writes the image `fid emit` writes, byte
#      for byte, twice in a row (the second with the stdlib warm);
#   2. `ccompile` through the server writes the C `fid ccompile` writes
#      (out.c beside the executable; no C compiler is needed for that);
#   3. a `check` after an emit is as clean as before it (the runtime the
#      emit merged is not left merged), and an error is still reported.
#   FID=release/fid.exe bash tests/tooling/serve-build-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
# the paths go inside request lines, which Git Bash does not convert
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/hello.fd" <<'EOF2'
fn main() -> i64 {
    println("hello from serve");
    return 0;
}
EOF2
cat > "$TMP/sum.fd" <<'EOF2'
fn sum(n: i64) -> i64 {
    var s: i64 = 0;
    var i: i64 = 1;
    while i <= n {
        s = s + i;
        i = i + 1;
    }
    return s;
}
fn main() -> i64 {
    println(int_to_str(sum(10)));
    return 0;
}
EOF2
cat > "$TMP/bad.fd" <<'EOF2'
fn main() -> i64 {
    var x: i64 = "text";
    return x;
}
EOF2
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
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
for prog in hello sum; do
    A=$(ask "emit $TMP/$prog.fd -o $TMP/$prog.exe --no-cache")
    cp "$TMP/$prog.exe" "$TMP/$prog.serve.exe" 2>/dev/null
    A2=$(ask "emit $TMP/$prog.fd -o $TMP/$prog.exe --no-cache")
    cp "$TMP/$prog.exe" "$TMP/$prog.serve2.exe" 2>/dev/null
    "$FID" emit "$TMP/$prog.fd" -o "$TMP/$prog.exe" --no-cache > /dev/null 2>&1
    if echo "$A" | grep -q "^done 0" && cmp -s "$TMP/$prog.exe" "$TMP/$prog.serve.exe"; then ok "1. emit $prog: same image as fid emit"; else bad "1. emit $prog: $A"; fi
    if echo "$A2" | grep -q "^done 0" && cmp -s "$TMP/$prog.exe" "$TMP/$prog.serve2.exe"; then ok "1. emit $prog again (warm): same image"; else bad "1. emit $prog again: $A2"; fi
done
A=$(ask "ccompile $TMP/sum.fd -o $TMP/sumc.exe --no-cache")
cp "$TMP/sumc.c" "$TMP/sumc.serve.c" 2>/dev/null
"$FID" ccompile "$TMP/sum.fd" -o "$TMP/sumc.exe" --no-cache > /dev/null 2>&1
if [ -s "$TMP/sumc.serve.c" ] && cmp -s "$TMP/sumc.c" "$TMP/sumc.serve.c"; then ok "2. ccompile: same C as fid ccompile"; else bad "2. ccompile: $A"; fi
A=$(ask "check $TMP/sum.fd")
if echo "$A" | grep -q "no errors" && echo "$A" | grep -q "^done 0"; then ok "3. check after an emit is clean"; else bad "3. check after an emit: $A"; fi
A=$(ask "check $TMP/bad.fd")
if echo "$A" | grep -q "^done 1" && echo "$A" | grep -q "error"; then ok "3. check after an emit still reports an error"; else bad "3. check of a bad file: $A"; fi
A=$(ask "emit -o $TMP/x.exe")
if echo "$A" | grep -q "^done 2"; then ok "a request without a file is refused"; else bad "a request without a file: $A"; fi
echo "quit" >&"${SRV[1]}"
wait 2>/dev/null
if [ $fail = 0 ]; then echo "serve-build-check: PASS"; else echo "serve-build-check: FAIL"; cat "$TMP/serve.err" | tail -20; fi
exit $fail
