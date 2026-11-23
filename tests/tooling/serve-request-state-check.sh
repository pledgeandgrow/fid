#!/bin/bash
# BG50, BG70 (docs/roadmap/bugs-roadmap.md): one `fid serve` request does
# not change how the next one builds.
#   1. BG50: after a `ccompile --leaks` request, a `ccompile` request
#      without it generates C without the debug allocator (P_LEAKS), and
#      after a `ccompile -g` request, C without #line directives (the C
#      generator's switches are reset: main.fd serve_reset_backend_state);
#   2. BG70: a `ccompile --target` request for another OS than the previous
#      request's gets the program lowered for that OS (@cfg), not the
#      in-memory IR cache's entry of the previous request (compiler.fd
#      ir_cache_key). Only the C is looked at: no C compiler or cross
#      compiler is needed (the C is written before it runs).
#   FID=release/fid.exe bash tests/tooling/serve-request-state-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
# the paths go inside request lines, which Git Bash does not convert
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
trap 'rm -rf "$TMP"' EXIT
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
cat > "$TMP/os.fd" <<'EOF2'
@cfg(linux)
fn os_name() -> str {
    return "built-for-linux";
}
@cfg(windows)
fn os_name() -> str {
    return "built-for-windows";
}
@cfg(macos)
fn os_name() -> str {
    return "built-for-macos";
}
fn main() -> i64 {
    println(os_name());
    return 0;
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
# 1. BG50
ask "ccompile $TMP/sum.fd -o $TMP/leaks.exe --no-cache --leaks" > /dev/null
ask "ccompile $TMP/sum.fd -o $TMP/plain.exe --no-cache" > /dev/null
if grep -q "define P_LEAKS 1" "$TMP/leaks.c" 2>/dev/null; then ok "1. the --leaks request's C has the debug allocator"; else bad "1. the --leaks request's C has no P_LEAKS (the check cannot tell)"; fi
if [ -s "$TMP/plain.c" ] && ! grep -q "define P_LEAKS 1" "$TMP/plain.c"; then ok "1. the next request's C has no debug allocator"; else bad "1. the request after --leaks still has P_LEAKS (or no C)"; fi
ask "ccompile $TMP/sum.fd -o $TMP/dbg.exe --no-cache -g" > /dev/null
ask "ccompile $TMP/sum.fd -o $TMP/plain2.exe --no-cache" > /dev/null
if grep -q "^#line " "$TMP/dbg.c" 2>/dev/null; then ok "1. the -g request's C has #line"; else bad "1. the -g request's C has no #line (the check cannot tell)"; fi
if [ -s "$TMP/plain2.c" ] && ! grep -q "^#line " "$TMP/plain2.c"; then ok "1. the next request's C has no #line"; else bad "1. the request after -g still has #line (or no C)"; fi
# 2. BG70: the host's build first (the cache holds it), then another OS
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) other=x86_64-linux; want=built-for-linux; host_want=built-for-windows ;;
    Darwin*) other=x86_64-linux; want=built-for-linux; host_want=built-for-macos ;;
    *) other=x86_64-windows; want=built-for-windows; host_want=built-for-linux ;;
esac
ask "ccompile $TMP/os.fd -o $TMP/host.exe --no-cache" > /dev/null
ask "ccompile $TMP/os.fd -o $TMP/cross.exe --no-cache --target $other" > /dev/null
if grep -q "$host_want" "$TMP/host.c" 2>/dev/null; then ok "2. the host request's C is the host's program"; else bad "2. the host request's C does not name $host_want"; fi
if grep -q "$want" "$TMP/cross.c" 2>/dev/null && ! grep -q "$host_want" "$TMP/cross.c"; then ok "2. the --target $other request's C is lowered for $other"; else bad "2. the --target $other request's C is not $want (the cached host IR?)"; fi
echo "quit" >&"${SRV[1]}"
wait 2>/dev/null
if [ $fail = 0 ]; then echo "serve-request-state-check: PASS"; else echo "serve-request-state-check: FAIL"; tail -20 "$TMP/serve.err"; fi
exit $fail
