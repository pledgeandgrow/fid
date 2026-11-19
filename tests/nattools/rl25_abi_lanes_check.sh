#!/bin/bash
# rl25_abi_lanes_check.sh — RL25 (docs/roadmap/llvm-roadmap.md): the two
# C-ABI tests on every lane this host can run, each run's output compared
# with the "Expected output:" block of the test file (the interpreter cannot
# call qsort or ldexp, so scripts/cross-test.sh has no reference for them):
#   tests/bootstrap/test_verify_abi_c_records.fd  qsort/bsearch calling a
#       Fides comparison on 16-byte records, labs, memcmp
#   tests/bootstrap/test_verify_abi_c_mixed.fd    f64 arguments mixed with
#       i32 and pointer arguments in one C call (ldexp, frexp, modf,
#       strtod, fmod)
#
# Usage: bash tests/nattools/rl25_abi_lanes_check.sh [FID]
#   FID     the compiler (default release/fid.exe, else release/fid)
#   OUT     build/rl25-abi-lanes; the record of the run is $OUT/record.txt
#   LANES   which lanes (default all of them):
#             c          `ccompile` for the host (CC, default zig cc)
#             pe         `emit` (pe_ir, Windows only)
#             nat        `emit --nat` (the machine layer, Windows only)
#             native:<t> `emit --target <t>` for x86_64-linux,
#                        aarch64-linux, riscv64-linux (the machine layer)
#             c:<t>      `ccompile --target <t>` for the same three (static
#                        musl) and wasm32-wasi (run with node
#                        scripts/wasm-run.mjs)
#   WSL_DISTRO  where Linux binaries run on Windows; qemu-aarch64 and
#           qemu-riscv64 run the other architectures, with QEMU_LD_PREFIX
#           (default /usr/<arch>-linux-gnu) for a dynamically linked one
#   macOS lanes need a Mac (not run here).
#
# Each lane and test prints PASS, FAIL (build failed, wrong output or exit
# status non-zero) or SKIP (this host cannot run the lane). Exit status: the
# number of FAILs.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/rl25-abi-lanes}"
mkdir -p "$OUT"
REC="$OUT/record.txt"
: > "$REC"
CC="${CC:-zig cc}"
export CC
TESTS="tests/bootstrap/test_verify_abi_c_records.fd tests/bootstrap/test_verify_abi_c_mixed.fd"
ALL="c pe nat native:x86_64-linux native:aarch64-linux native:riscv64-linux c:x86_64-linux c:aarch64-linux c:riscv64-linux c:wasm32-wasi"
LANES="${LANES:-$ALL}"
bad=0; good=0; skipped=0
say() { echo "$1"; echo "$1" >> "$REC"; }

# expected <test.fd>: the lines after "// Expected output:" up to the
# first line that is not a comment, without the "// "
expected() {
  awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\// {sub(/^\/\/ ?/, ""); print; next} on {exit}' "$1"
}

ARCH=$(uname -m)
wsl_ok() { [ "$HOST" = windows ] && command -v wsl.exe > /dev/null 2>&1; }
in_wsl() { MSYS_NO_PATHCONV=1 wsl.exe ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e sh -c "$1"; }
wsl_path() {  # a Windows-side path -> /mnt/<drive>/...
  local p
  # cygpath -m gives C:/...: Git Bash's own /tmp, /c/... spellings are not
  # what WSL sees
  if command -v cygpath > /dev/null 2>&1; then
    p=$(cygpath -m "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")")
    echo "/mnt/$(echo "$p" | sed -E 's|^([a-zA-Z]):/|\L\1/|')"
    return
  fi
  p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
  echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

# runner <target>: how to start a Linux binary of that architecture here,
# "" when it cannot run
runner_for() {
  local a="${1%%-linux}" q=""
  [ "$a" = aarch64 ] && q=qemu-aarch64
  [ "$a" = riscv64 ] && q=qemu-riscv64
  if [ "$HOST" = linux ]; then
    if [ "$ARCH" = "$a" ]; then echo "direct"; return; fi
    [ -n "$q" ] && command -v "$q" > /dev/null 2>&1 && { echo "$q"; return; }
    echo ""; return
  fi
  if wsl_ok; then
    if [ -z "$q" ]; then echo "direct"; return; fi
    in_wsl "command -v $q" > /dev/null 2>&1 && { echo "$q"; return; }
  fi
  echo ""
}

# run_linux <target> <binary> <out>: its stdout in <out>, its exit status
run_linux() {
  local t="$1" bin="$2" out="$3" r a
  r=$(runner_for "$t")
  a="${t%%-linux}"
  local pre="if [ -z \"\${QEMU_LD_PREFIX:-}\" ] && [ -d /usr/$a-linux-gnu/lib ]; then export QEMU_LD_PREFIX=/usr/$a-linux-gnu; fi; "
  if [ "$HOST" = linux ]; then
    chmod +x "$bin"
    if [ "$r" = direct ]; then "$bin" > "$out" 2> "$out.err" < /dev/null; return $?; fi
    sh -c "$pre $r '$bin'" > "$out" 2> "$out.err" < /dev/null; return $?
  fi
  local wb
  wb=$(wsl_path "$bin")
  if [ "$r" = direct ]; then
    in_wsl "chmod +x '$wb'; '$wb' < /dev/null" > "$out" 2> "$out.err"; return $?
  fi
  in_wsl "chmod +x '$wb'; $pre $r '$wb' < /dev/null" > "$out" 2> "$out.err"; return $?
}

# judge <lane> <test> <run status> <out>
judge() {
  local lane="$1" tf="$2" rc="$3" out="$4" b
  b=$(basename "$tf" .fd)
  if [ "$rc" != 0 ]; then
    say "FAIL $lane $b: exit status $rc ($(tr -d '\r' < "$out.err" 2>/dev/null | head -2 | tr '\n' ' '))"
    bad=$((bad + 1)); return
  fi
  if cmp -s <(expected "$tf") <(tr -d '\r' < "$out"); then
    say "PASS $lane $b"
    good=$((good + 1))
  else
    say "FAIL $lane $b: output differs (diff <(expected) $out):"
    diff <(expected "$tf") <(tr -d '\r' < "$out") | head -8 | sed 's/^/    /' | tee -a "$REC"
    bad=$((bad + 1))
  fi
}
skip() { say "SKIP $1"; skipped=$((skipped + 1)); }
build() {  # build <lane> <test> <out binary> <fid args...>: 0 when built
  local lane="$1" tf="$2" bin="$3"; shift 3
  rm -f "$bin"
  if ! timeout 900 "$FID" "$@" > "$bin.build.log" 2>&1 || [ ! -s "$bin" ]; then
    say "FAIL $lane $(basename "$tf" .fd): build failed: $(grep -v '^\[' "$bin.build.log" | grep -v '^$' | tail -2 | tr '\n' ' ')"
    bad=$((bad + 1)); return 1
  fi
  return 0
}

say "rl25_abi_lanes_check with $FID on $HOST ($(date -u +%Y-%m-%dT%H:%MZ))"
for lane in $LANES; do
  for tf in $TESTS; do
    b=$(basename "$tf" .fd)
    case "$lane" in
      c)
        exe="$OUT/$b.c"; [ "$HOST" = windows ] && exe="$exe.exe"
        build "$lane" "$tf" "$exe" ccompile "$tf" -o "$exe" || continue
        "$exe" > "$exe.out" 2> "$exe.out.err" < /dev/null; judge "$lane" "$tf" $? "$exe.out" ;;
      pe|nat)
        if [ "$HOST" != windows ]; then skip "$lane $b (Windows only)"; continue; fi
        exe="$OUT/$b.$lane.exe"
        if [ "$lane" = pe ]; then build "$lane" "$tf" "$exe" emit "$tf" -o "$exe" || continue
        else build "$lane" "$tf" "$exe" emit "$tf" --nat -o "$exe" || continue; fi
        "$exe" > "$exe.out" 2> "$exe.out.err" < /dev/null; judge "$lane" "$tf" $? "$exe.out" ;;
      native:*|c:*-linux)
        t="${lane#*:}"
        if [ -z "$(runner_for "$t")" ]; then skip "$lane $b (no way to run $t here)"; continue; fi
        bin="$OUT/$b.${lane%%:*}.$t"
        if [ "${lane%%:*}" = native ]; then build "$lane" "$tf" "$bin" emit "$tf" --target "$t" -o "$bin" || continue
        else build "$lane" "$tf" "$bin" ccompile "$tf" --target "$t" -o "$bin" || continue; fi
        run_linux "$t" "$bin" "$bin.out"
        lrc=$?
        # a dynamically linked binary under qemu-user without the target's
        # glibc (libc6-arm64-cross / libc6-riscv64-cross, or QEMU_LD_PREFIX)
        if [ $lrc != 0 ] && grep -qa "Could not open '/lib" "$bin.out.err" 2>/dev/null; then
          skip "$lane $b (dynamic: $(tr -d '\r' < "$bin.out.err" | grep -a 'Could not open' | head -1 | sed 's/.*Could not open //'); install the target's glibc or set QEMU_LD_PREFIX)"
          continue
        fi
        judge "$lane" "$tf" $lrc "$bin.out" ;;
      c:wasm32-wasi)
        if ! command -v node > /dev/null 2>&1; then skip "$lane $b (node not installed)"; continue; fi
        w="$OUT/$b.wasm"
        build "$lane" "$tf" "$w" ccompile "$tf" --target wasm32-wasi -o "$w" || continue
        node scripts/wasm-run.mjs "$w" > "$w.out" 2> "$w.out.err" < /dev/null; judge "$lane" "$tf" $? "$w.out" ;;
      *) skip "$lane $b (unknown lane)" ;;
    esac
  done
done
say "rl25_abi_lanes_check: $good passed, $bad failed, $skipped skipped"
exit $bad
