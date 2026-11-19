#!/bin/bash
# SP18 (docs/roadmap/llvm-roadmap.md): `fid serve` keeps the machine
# layer's per-function cache in memory between build requests
# (compiler/main.fd cmd_serve sets nat_ir.fd g_nat_fncache_mem), and puts
# back after each request what its flags set in the back ends
# (main.fd serve_reset_backend_state). Checks, on a program in a temporary
# directory, built for x86_64-linux (any host) through `fid serve`:
#   1. the first emit compiles every function ("[fncache] entries from
#      none") and writes the image the one-shot `fid emit` writes;
#   2. the second emit (another -o, so no build stamp skips it) takes its
#      functions from memory ("entries from memory, N hits", N > 0), writes
#      no cache file, and writes the same image;
#   3. a request with --no-fncache builds without the cache (no [fncache]
#      line) and writes the same image;
#   4. after a --stats request, a request without --stats prints no
#      [fncache] line (g_nat_stats is reset);
#   5. after a --debug request (the fast allocator), a request without
#      --debug writes the release image of the one-shot run (g_nat_fast
#      is reset);
#   6. --no-pass and --size in a request reach the mid-end (checks.fd
#      iro_cli_off reads the request's words): `--size` through serve
#      writes the image of the one-shot `emit --size`.
#   FID=release/fid.exe bash tests/tooling/serve-fncache-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
# the paths go inside request lines, which Git Bash does not convert
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
trap 'rm -rf "$TMP"' EXIT
T=x86_64-linux
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
    var v: Vec<i64> = Vec::new();
    var k: i64 = 0;
    while k < 8 {
        v.push(sum(k * 10));
        k = k + 1;
    }
    println(int_to_str(v.get(7)));
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
nlines() { grep -cF "[fncache]" "$TMP/serve.err"; }
# the one-shot images
"$FID" emit "$TMP/sum.fd" --target $T --stats --no-cache -o "$TMP/one.out" > /dev/null 2>&1
"$FID" emit "$TMP/sum.fd" --target $T --no-cache -o "$TMP/rel.out" > /dev/null 2>&1
"$FID" emit "$TMP/sum.fd" --target $T --size --no-cache -o "$TMP/size.out" > /dev/null 2>&1

A=$(ask "emit $TMP/sum.fd --target $T --stats -o $TMP/s1.out")
if echo "$A" | grep -q "^done 0" && grep -qF "[fncache] entries from none" "$TMP/serve.err" && cmp -s "$TMP/one.out" "$TMP/s1.out"; then
    ok "1. first emit: every function compiled, same image as fid emit"
else
    bad "1. first emit: $(echo "$A" | tail -1); $(grep -F '[fncache]' "$TMP/serve.err" | head -2); $(cmp "$TMP/one.out" "$TMP/s1.out" 2>&1)"
fi
A=$(ask "emit $TMP/sum.fd --target $T --stats -o $TMP/s2.out")
h=$(grep -F "[fncache] entries from memory" "$TMP/serve.err" | tail -1 | sed -n 's/.*, \([0-9]*\) hits.*/\1/p')
if echo "$A" | grep -q "^done 0" && [ -n "$h" ] && [ "$h" -gt 0 ]; then
    ok "2. second emit: $h functions from memory"
else
    bad "2. second emit: no hits from memory: $(grep -F '[fncache]' "$TMP/serve.err" | tail -2)"
fi
if cmp -s "$TMP/one.out" "$TMP/s2.out" && ! ls "$TMP" | grep -q fides-fncache; then
    ok "2. same image, no cache file"
else
    bad "2. image or cache file: $(cmp "$TMP/one.out" "$TMP/s2.out" 2>&1) $(ls "$TMP" | grep fncache | tr '\n' ' ')"
fi
before=$(nlines)
A=$(ask "emit $TMP/sum.fd --target $T --stats --no-fncache -o $TMP/s3.out")
if echo "$A" | grep -q "^done 0" && [ "$(nlines)" = "$before" ] && cmp -s "$TMP/one.out" "$TMP/s3.out"; then
    ok "3. --no-fncache: built without the cache, same image"
else
    bad "3. --no-fncache: $(grep -F '[fncache]' "$TMP/serve.err" | tail -1); $(cmp "$TMP/one.out" "$TMP/s3.out" 2>&1)"
fi
before=$(nlines)
A=$(ask "emit $TMP/sum.fd --target $T -o $TMP/s4.out")
if echo "$A" | grep -q "^done 0" && [ "$(nlines)" = "$before" ] && cmp -s "$TMP/rel.out" "$TMP/s4.out"; then
    ok "4. after --stats, a request without it prints no [fncache] line"
else
    bad "4. --stats stayed on or the image differs: $(grep -F '[fncache]' "$TMP/serve.err" | tail -1); $(cmp "$TMP/rel.out" "$TMP/s4.out" 2>&1)"
fi
A=$(ask "emit $TMP/sum.fd --target $T --debug -o $TMP/d5.out")
A2=$(ask "emit $TMP/sum.fd --target $T -o $TMP/s5.out")
if echo "$A2" | grep -q "^done 0" && cmp -s "$TMP/rel.out" "$TMP/s5.out"; then
    ok "5. after --debug, a request without it writes the release image"
else
    bad "5. after --debug: $(cmp "$TMP/rel.out" "$TMP/s5.out" 2>&1)"
fi
A=$(ask "emit $TMP/sum.fd --target $T --size -o $TMP/s6.out")
if echo "$A" | grep -q "^done 0" && cmp -s "$TMP/size.out" "$TMP/s6.out"; then
    ok "6. --size in a request: the image of emit --size"
else
    bad "6. --size: $(cmp "$TMP/size.out" "$TMP/s6.out" 2>&1)"
fi
echo "quit" >&"${SRV[1]}"
wait 2>/dev/null
if [ $fail = 0 ]; then echo "serve-fncache-check: PASS"; else echo "serve-fncache-check: FAIL"; tail -20 "$TMP/serve.err"; fi
exit $fail
