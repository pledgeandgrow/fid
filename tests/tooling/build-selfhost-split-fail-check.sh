#!/usr/bin/env bash
# build-selfhost-split-fail-check.sh — scripts/build-selfhost.sh does not
# hide a failed split unit (known-bugs 2g.86).
#
# Runs a copy of the script in a scratch tree with a fake stage 0 (dump-c
# prints a one-line C program, split-c writes the units) and a fake C
# compiler that fails unit 1 with exit 5 and no message:
#   1. once (the next try passes): the unit is named with its exit code,
#      built again alone, and the build ends "8 units at -O2", exit 0;
#   2. every time: the script names the unit, exits 1, and does not build
#      one unit at -O0. Before, both ended in an -O0 build with exit 0.
# Needs no real compiler; nothing is written outside the scratch tree.
#
# Usage: tests/tooling/build-selfhost-split-fail-check.sh
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
mkdir -p "$T/scripts" "$T/compiler"
cp "$ROOT/scripts/build-selfhost.sh" "$T/scripts/"
: > "$T/compiler/main.fd"

cat > "$T/stage0" <<'EOF'
#!/usr/bin/env bash
case "$1" in
  dump-c) echo 'int main(int argc, char** argv) { return 0; }' ;;
  split-c)
    base="$3"; n="$4"
    for k in $(seq 0 $((n - 1))); do echo "int u$k(void) { return $k; }" > "${base}_$k.c"; done
    : > "$base.h" ;;
  *) exit 2 ;;
esac
EOF
cat > "$T/cc" <<'EOF'
#!/usr/bin/env bash
# fake C compiler: writes its -o file; unit 1 fails (FAIL_MODE once|always)
out=""; unit1=0; o0=0; prev=""
for a in "$@"; do
  [ "$prev" = "-o" ] && out="$a"
  case "$a" in *_u_1.c) unit1=1 ;; -O0) o0=1 ;; esac
  prev="$a"
done
[ "$o0" = 1 ] && echo "O0 build" >> "$FAKE_DIR/o0.log"
if [ "$unit1" = 1 ]; then
  if [ "$FAIL_MODE" = always ]; then exit 5; fi
  if [ ! -f "$FAKE_DIR/failed_once" ]; then : > "$FAKE_DIR/failed_once"; exit 5; fi
fi
[ -n "$out" ] && : > "$out"
exit 0
EOF
chmod +x "$T/stage0" "$T/cc"

fail=0
run_case() {
  rm -f "$T/failed_once" "$T/o0.log"; rm -rf "$T/release"
  (cd "$T" && FAKE_DIR="$T" FAIL_MODE="$1" STAGE0="$T/stage0" CC="$T/cc" SPLIT=8 JOBS=4 \
     bash scripts/build-selfhost.sh) > "$T/out_$1.txt" 2>&1
  echo $?
}

rc=$(run_case once)
out=$(cat "$T/out_once.txt")
if [ "$rc" != 0 ] || ! grep -q "8 units at -O2" <<< "$out" || ! grep -q "_u_1.c failed at -O2 (exit 5)" <<< "$out" || [ -f "$T/o0.log" ]; then
  echo "FAIL once: exit $rc"; echo "$out"; fail=1
else
  echo "ok   a unit that fails once is named and rebuilt alone (8 units at -O2)"
fi

rc=$(run_case always)
out=$(cat "$T/out_always.txt")
if [ "$rc" != 1 ] || ! grep -q "error: unit .*_u_1.c failed at -O2 again" <<< "$out" || [ -f "$T/o0.log" ]; then
  echo "FAIL always: exit $rc"; echo "$out"; fail=1
else
  echo "ok   a unit that keeps failing stops the build (exit 1, no -O0 build)"
fi

[ "$fail" = 0 ] && echo "PASS build-selfhost-split-fail-check" || { echo "FAILED build-selfhost-split-fail-check"; exit 1; }
