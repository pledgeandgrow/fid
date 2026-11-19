#!/bin/bash
# SP13 (docs/roadmap/llvm-roadmap.md): `fid emit --debug` on the machine
# layer keeps the machine code of each function beside its output
# (`<output>.fides-fncache`, compiler/main.fd debug_cache_setup through
# inc.fd env_override_set), so the next debug build after an edit only
# compiles the functions the edit changed. Checks, on a program in a
# temporary directory, built for x86_64-linux (any host):
#   1. a cold debug build writes the cache and the same image as a debug
#      build without it (--no-cache);
#   2. after an edit of one function, the next debug build takes the other
#      functions from the cache ("[fncache] entries from file, N hits",
#      N > 0) and writes the same image as a cache-less debug build of the
#      edited program;
#   3. --no-cache, --no-fncache and FID_FNCACHE=off write no cache;
#   4. a release build (no --debug) writes no cache (nothing changes there);
#   5. on a Linux x86-64 host, the cached build runs and prints what the
#      cache-less one prints.
# IRCACHE=1 also tries `--ir-cache` (the lowered-IR cache, opt-in): the
# same image warm as cold. TIME=1 prints the SP13 measurement: release,
# cold debug and warm debug totals (--time) and the ratios.
#   FID=release/fid.exe bash tests/tooling/sp13-debug-cache-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
T=x86_64-linux
write_prog() { # write_prog <greeting>
cat > "$TMP/p.fd" <<EOF2
fn greet() -> str {
    return "$1";
}
fn sum_to(n: i64) -> i64 {
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
    v.push(sum_to(10));
    v.push(sum_to(100));
    println(greet());
    println(int_to_str(v.get(0) + v.get(1)));
    return 0;
}
EOF2
}
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
hits_of() { grep -F "[fncache] entries from" "$1" | tail -1 | sed -n 's/.*, \([0-9]*\) hits.*/\1/p'; }
ms_of() { grep -F "[time] $1:" "$2" | head -1 | grep -o '[0-9]* ms' | awk '{print $1}'; }

write_prog "hello one"
"$FID" emit "$TMP/p.fd" --target $T --debug --stats --no-cache -o "$TMP/fresh1.out" > "$TMP/fresh1.log" 2>&1
"$FID" emit "$TMP/p.fd" --target $T --debug --stats --time -o "$TMP/d.out" > "$TMP/cold.log" 2>&1
if [ -s "$TMP/d.out.fides-fncache" ] && grep -qF "[fncache] entries from none" "$TMP/cold.log"; then
    ok "1. a cold debug build writes d.out.fides-fncache"
else
    bad "1. no function cache after a cold debug build: $(grep -F '[fncache]' "$TMP/cold.log" | head -2)"
fi
if [ -s "$TMP/fresh1.out" ] && cmp -s "$TMP/fresh1.out" "$TMP/d.out"; then
    ok "1. cold image = image without the cache"
else
    bad "1. the cold debug image differs from the cache-less one"
fi

write_prog "hello two"
"$FID" emit "$TMP/p.fd" --target $T --debug --stats --no-cache -o "$TMP/fresh2.out" > "$TMP/fresh2.log" 2>&1
"$FID" emit "$TMP/p.fd" --target $T --debug --stats --time -o "$TMP/d.out" > "$TMP/warm.log" 2>&1
h=$(hits_of "$TMP/warm.log")
if grep -qF "[fncache] entries from file" "$TMP/warm.log" && [ -n "$h" ] && [ "$h" -gt 0 ]; then
    ok "2. the debug build after an edit reuses $h functions"
else
    bad "2. no reuse after an edit: $(grep -F '[fncache]' "$TMP/warm.log" | head -2)"
fi
if [ -s "$TMP/fresh2.out" ] && cmp -s "$TMP/fresh2.out" "$TMP/d.out"; then
    ok "2. warm image = image without the cache"
else
    bad "2. the warm debug image differs from the cache-less one"
fi

"$FID" emit "$TMP/p.fd" --target $T --debug --no-cache -o "$TMP/n1.out" > /dev/null 2>&1
"$FID" emit "$TMP/p.fd" --target $T --debug --no-fncache -o "$TMP/n2.out" > /dev/null 2>&1
# (in $TMP: nat_ir took the word for a file name in the working directory)
(cd "$TMP" && FID_FNCACHE=off "$FID" emit p.fd --target $T --debug -o n3.out > /dev/null 2>&1)
if [ -s "$TMP/n1.out" ] && [ -s "$TMP/n2.out" ] && [ -s "$TMP/n3.out" ] && [ ! -e "$TMP/n1.out.fides-fncache" ] && [ ! -e "$TMP/n2.out.fides-fncache" ] && [ ! -e "$TMP/n3.out.fides-fncache" ] && [ ! -e "$TMP/off" ]; then
    ok "3. --no-cache, --no-fncache and FID_FNCACHE=off write no cache"
else
    bad "3. a cache was written although turned off: $(ls "$TMP" | grep -e fncache -e '^off$' | tr '\n' ' ')"
fi
"$FID" emit "$TMP/p.fd" --target $T --time -o "$TMP/r.out" > "$TMP/release.log" 2>&1
if [ -s "$TMP/r.out" ] && [ ! -e "$TMP/r.out.fides-fncache" ]; then
    ok "4. a release build writes no cache"
else
    bad "4. release build: $(ls "$TMP" | grep r.out | tr '\n' ' ')"
fi

if [ "$(uname -s)" = "Linux" ] && [ "$(uname -m)" = "x86_64" ]; then
    chmod +x "$TMP/d.out" "$TMP/fresh2.out"
    a=$("$TMP/d.out"); b=$("$TMP/fresh2.out")
    if [ "$a" = "$b" ] && [ "$a" = "hello two"$'\n'"5105" ]; then ok "5. the cached build prints the same"; else bad "5. outputs: '$a' vs '$b'"; fi
fi

if [ "${IRCACHE:-0}" = 1 ]; then
    write_prog "hello three"
    "$FID" emit "$TMP/p.fd" --target $T --debug --no-cache -o "$TMP/fresh3.out" > /dev/null 2>&1
    "$FID" emit "$TMP/p.fd" --target $T --debug --ir-cache -o "$TMP/i.out" > "$TMP/ir1.log" 2>&1
    write_prog "hello four"
    "$FID" emit "$TMP/p.fd" --target $T --debug --no-cache -o "$TMP/fresh4.out" > /dev/null 2>&1
    "$FID" emit "$TMP/p.fd" --target $T --debug --ir-cache --time -o "$TMP/i.out" > "$TMP/ir2.log" 2>&1
    sz=$(wc -c < "$TMP/i.out.fides-ircache" 2>/dev/null || echo 0)
    if [ "$sz" -gt 0 ] && cmp -s "$TMP/fresh4.out" "$TMP/i.out"; then
        ok "ir-cache: warm image = image without the caches (cache $sz bytes)"
    else
        bad "ir-cache: cache $sz bytes, images $(cmp "$TMP/fresh4.out" "$TMP/i.out" 2>&1)"
    fi
fi

if [ "${TIME:-0}" = 1 ]; then
    tr=$(ms_of total "$TMP/release.log"); tc=$(ms_of total "$TMP/cold.log"); tw=$(ms_of total "$TMP/warm.log")
    echo "time release:   $tr ms ($(grep -F '[time] lower:' "$TMP/release.log"), $(grep -F '[time] native code:' "$TMP/release.log"))"
    echo "time debug cold: $tc ms ($(grep -F '[time] lower:' "$TMP/cold.log"), $(grep -F '[time] native code:' "$TMP/cold.log"))"
    echo "time debug warm: $tw ms ($(grep -F '[time] lower:' "$TMP/warm.log"), $(grep -F '[time] native code:' "$TMP/warm.log"))"
    if [ -n "$tr" ] && [ -n "$tw" ] && [ "${tw:-0}" -gt 0 ] && [ "${tc:-0}" -gt 0 ]; then
        echo "ratio release/debug: cold $(awk "BEGIN{printf \"%.2f\", $tr/$tc}")x, warm $(awk "BEGIN{printf \"%.2f\", $tr/$tw}")x (SP13 asks 5x)"
    fi
    if [ "${IRCACHE:-0}" = 1 ]; then
        echo "time debug warm with --ir-cache: $(ms_of total "$TMP/ir2.log") ms ($(grep -F '[time] lower:' "$TMP/ir2.log"))"
    fi
fi
if [ $fail = 0 ]; then echo "sp13-debug-cache-check: PASS"; else echo "sp13-debug-cache-check: FAIL"; fi
exit $fail
