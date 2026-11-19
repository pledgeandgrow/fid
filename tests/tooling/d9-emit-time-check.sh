#!/usr/bin/env bash
# D9 (docs/roadmap/release-roadmap.md): `fid emit` of hello world in under
# 0.5 s. What this checks:
#   1. the cache key (inc.fd inc_compiler_hash). A release build (scripts/
#      build-info.sh wrote compiler/build_info.fd fid_build_source) keys the
#      caches on the source's hash and the executable's size, reading nothing
#      of the executable; FIDES_SELF_HASH=exe gives the old key (the hash of
#      the executable's bytes, which `fid version` prints as "build: exe:").
#      A development build uses the old key.
#   2. the machine layer (`emit --nat` on Windows, the host target on Linux)
#      drops what no code can call before the mid-end, as pe_ir does (PE88):
#      "[time] prune" in --time, the same output with FID_PE_PRUNE=0 and as
#      `fid run`.
#   3. the time of `emit` of tests/bootstrap/hello.fd: a fresh build (a new
#      output, the stdlib caches warm), and a rebuild after an edit (the
#      output's own caches warm); best of RUNS (default 3). With a release
#      build (cache key "src:") the fresh build must take under LIMIT_MS
#      (default 500); a development build is only reported.
#   FID=release/fid.exe bash tests/tooling/d9-emit-time-check.sh
# Run from the repository root, on a quiet machine. Exit 0 when every check
# passes.
set -u
FID=${FID:-release/fid.exe}
RUNS=${RUNS:-3}
LIMIT_MS=${LIMIT_MS:-500}
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
note() { echo "     $1"; }
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;; esac
D=$(mktemp -d ./_d9_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
now_ms() { echo $(( $(date +%s%N) / 1000000 )); }

# --- 1. the cache key -----------------------------------------------------
"$FID" version > "$D/v.txt" 2>&1
key=$(tr -d '\r' < "$D/v.txt" | sed -n 's/^cache key: \([a-z]*\):\([0-9a-f]*\)$/\1 \2/p')
build=$(tr -d '\r' < "$D/v.txt" | sed -n 's/^build: exe:\([0-9a-f]\{16\}\).*/\1/p')
mode=${key%% *}
khex=${key#* }
if [ -z "$key" ]; then bad "fid version prints no 'cache key:' line"; cat "$D/v.txt"; mode=none; fi
if [ -z "$build" ]; then bad "fid version prints no 'build: exe:<hash>' line"; fi
case "$mode" in
  exe)
    ok "cache key: exe (a development build)"
    if [ -n "$build" ] && [ "$khex" = "$build" ]; then ok "the exe key is the executable's hash"; else bad "exe key $khex, build $build"; fi ;;
  src)
    ok "cache key: src (a release build: no read of the executable)"
    if [ "$khex" = "$build" ]; then bad "the src key equals the executable's hash"; else ok "the src key is not the executable's hash"; fi
    FIDES_SELF_HASH=exe "$FID" version > "$D/v2.txt" 2>&1
    k2=$(tr -d '\r' < "$D/v2.txt" | sed -n 's/^cache key: exe:\([0-9a-f]*\)$/\1/p')
    if [ -n "$k2" ] && [ "$k2" = "$build" ]; then ok "FIDES_SELF_HASH=exe: the executable's hash"; else bad "FIDES_SELF_HASH=exe gave '$k2', build $build"; fi
    # the key is the same on every run (no per-run part)
    "$FID" version > "$D/v3.txt" 2>&1
    k3=$(tr -d '\r' < "$D/v3.txt" | sed -n 's/^cache key: src:\([0-9a-f]*\)$/\1/p')
    if [ "$k3" = "$khex" ]; then ok "the src key is stable"; else bad "the src key changed: $khex then $k3"; fi ;;
esac

# --- 2. the machine layer's prune ------------------------------------------
if [ $WIN = 1 ]; then NATFLAG="--nat"; else NATFLAG=""; fi
nat_ok=1
for t in hello test_vec_builtins test_closure test_drop test_hashmap; do
  src=tests/bootstrap/$t.fd
  [ -f "$src" ] || continue
  "$FID" run "$src" > "$D/$t.run" 2>/dev/null
  if ! "$FID" emit "$src" $NATFLAG --no-cache --time -o "$D/$t.p$EXE" > "$D/$t.p.log" 2>&1; then
    if [ -z "$NATFLAG" ] && [ "$(uname -s)" != Linux ]; then note "$t: no machine-layer host build here"; continue; fi
    bad "$t: emit failed"; grep -m3 -i -E 'error|panic' "$D/$t.p.log"; nat_ok=0; continue
  fi
  if ! grep -q '^\[time\] prune:' "$D/$t.p.log"; then bad "$t: no prune stage on the machine layer"; nat_ok=0; fi
  if grep -q 'failed on the pruned module' "$D/$t.p.log"; then note "$t: built again unpruned (a function the prune dropped was needed): $(grep -m1 'failed on the pruned' "$D/$t.p.log")"; fi
  FID_PE_PRUNE=0 "$FID" emit "$src" $NATFLAG --no-cache -o "$D/$t.u$EXE" > "$D/$t.u.log" 2>&1 || { bad "$t: unpruned emit failed"; nat_ok=0; continue; }
  "./$D/$t.p$EXE" > "$D/$t.p.out" 2>/dev/null
  "./$D/$t.u$EXE" > "$D/$t.u.out" 2>/dev/null
  if [ ! -s "$D/$t.run" ]; then
    # `fid run` printed nothing (it failed): compare the two builds only
    if cmp -s "$D/$t.p.out" "$D/$t.u.out"; then ok "$t: pruned = unpruned (fid run gave no output)"; else bad "$t: pruned and unpruned outputs differ"; nat_ok=0; fi
  elif cmp -s "$D/$t.p.out" "$D/$t.u.out" && cmp -s "$D/$t.p.out" "$D/$t.run"; then ok "$t: pruned = unpruned = fid run"; else bad "$t: outputs differ (pruned / unpruned / run)"; diff "$D/$t.p.out" "$D/$t.run" | head -5; nat_ok=0; fi
done
[ $nat_ok = 1 ] && ok "machine-layer prune"

# --- 3. emit time of hello --------------------------------------------------
# warm the stdlib caches (checked core, the AST artifact) once
"$FID" emit tests/bootstrap/hello.fd -o "$D/warm$EXE" > /dev/null 2>&1
best=999999
i=1
while [ $i -le "$RUNS" ]; do
  s=$(now_ms)
  "$FID" emit tests/bootstrap/hello.fd -o "$D/fresh$i$EXE" --time > "$D/fresh$i.log" 2>&1
  e=$(now_ms)
  ms=$((e - s))
  [ $ms -lt $best ] && best=$ms && cp "$D/fresh$i.log" "$D/best.log"
  i=$((i + 1))
done
out=$("./$D/fresh1$EXE" 2>/dev/null | tr -d '\r')
case "$out" in "Hello, Fides!"*) ok "hello runs" ;; *) bad "hello printed '$out'" ;; esac
note "fresh emit of hello: best of $RUNS: $best ms; stages of that run:"
grep '^\[time\]' "$D/best.log" | grep -v '^\[time\]   ' | sed 's/^/       /'
# a rebuild after an edit: same output, its caches warm
cp tests/bootstrap/hello.fd "$D/h.fd"
"$FID" emit "$D/h.fd" -o "$D/h$EXE" > /dev/null 2>&1
rbest=999999
i=1
while [ $i -le "$RUNS" ]; do
  printf '// edit %s\n' "$i" >> "$D/h.fd"
  s=$(now_ms)
  "$FID" emit "$D/h.fd" -o "$D/h$EXE" > /dev/null 2>&1
  e=$(now_ms)
  ms=$((e - s))
  [ $ms -lt $rbest ] && rbest=$ms
  i=$((i + 1))
done
note "rebuild of hello after an edit: best of $RUNS: $rbest ms"
s=$(now_ms); "$FID" emit "$D/h.fd" -o "$D/h$EXE" > /dev/null 2>&1; e=$(now_ms)
note "up-to-date emit (nothing changed): $((e - s)) ms"
if [ "$mode" = src ]; then
  if [ $best -lt "$LIMIT_MS" ]; then ok "D9: fresh emit of hello $best ms < $LIMIT_MS ms"; else bad "D9: fresh emit of hello $best ms, target < $LIMIT_MS ms"; fi
else
  note "D9 target ($LIMIT_MS ms) not enforced: not a release build (cache key '$mode'); build with RELEASE=1 scripts/bootstrap.sh"
fi
if [ $fail -eq 0 ]; then echo "D9 check: all passed"; else echo "D9 check: FAILED"; fi
exit $fail
