#!/bin/bash
# bare-qemu-test.sh — the bare-metal tests under QEMU (release G1).
#
# Each test is built with `fid emit <test> --bare` (a Multiboot kernel:
# compiler/pe_ir_stubs.fd, "The bootable bare image") and booted with
#   qemu-system-x86_64 -kernel <bin> -serial stdio -display none -no-reboot
#       -device isa-debug-exit,iobase=0xf4,iosize=0x01
# COM1 is the program's output; the end of main (or a panic) writes the exit
# code to port 0xF4, which ends QEMU with status 2*code+1. The output and
# the exit code are compared with what the test expects:
#   tests/bootstrap/test_bare_*.fd   the lines after `// Expected output:`
#                                    (each `// ` stripped; a line ending in
#                                    `...` must only start with the rest),
#                                    and `// Expected exit: N` (default 0)
#   the kernel tests listed below    their expected output here
# A test with no `// Expected output:` block is not run.
#
# Usage:
#   FID=release/fid bash scripts/bare-qemu-test.sh [tests...]
# Environment:
#   FID      the compiler (default: release/fid, else release/fid-dev)
#   QEMU     default qemu-system-x86_64
#   OUT      where the .bin files go (default: a new temporary folder)
#   TIMEOUT  seconds per test (default 30)
#   BUILD=0  do not build: boot $OUT/<test>.bin as they are
#   RUN=0    only build (e.g. with a Windows fid, then RUN in WSL/Linux)
#   VERBOSE=1 print each test's output
# Exit status: 0 when every test passes.
set -u
cd "$(dirname "$0")/.." || exit 2

FID="${FID:-}"
if [ -z "$FID" ]; then
  for f in release/fid release/fid.exe release/fid-dev release/fid-dev.exe; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
QEMU="${QEMU:-qemu-system-x86_64}"
TIMEOUT="${TIMEOUT:-30}"
BUILD="${BUILD:-1}"
RUN="${RUN:-1}"
VERBOSE="${VERBOSE:-0}"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-/tmp}/bare-qemu.XXXXXX")}"
mkdir -p "$OUT"

if [ "$BUILD" = 1 ] && [ -z "$FID" ]; then
  echo "error: no fid (set FID)"; exit 2
fi
if [ "$RUN" = 1 ] && ! command -v "$QEMU" >/dev/null; then
  echo "error: $QEMU not found (apt install qemu-system-x86)"; exit 2
fi

# Kernel tests whose expected output is not in the file: name|exit|lines
# (lines separated by \n). Their main returns nothing (exit 0); `hlt`
# returns at the next timer tick (interrupts are on at boot).
KERNEL_TESTS=(
  "test_kernel_serial|0|OK"
  "test_kernel_hello|0|Hello from Fides OS!"
  "test_serial_hello|0|Hello from Fides!\n\r"
  "test_bare|0|"
  "test_interrupt|0|"
)

expected_of() {   # $1 = test file: prints the expected lines
  awk '
    { sub(/\r$/, "") }
    /^\/\/ Expected output:/ { on = 1; next }
    on && /^\/\/ Expected exit:/ { next }
    on && /^\/\// { sub(/^\/\/ ?/, ""); print; next }
    on { exit }
  ' "$1"
}
has_expected() { grep -q '^// Expected output:' "$1"; }
exit_of() {
  local e
  e=$(tr -d '\r' < "$1" | sed -n 's#^// Expected exit: *\([0-9][0-9]*\).*#\1#p' | head -1)
  echo "${e:-0}"
}

# compare actual ($1) with expected ($2), line by line; `...` = prefix
same_output() {
  awk -v exp_file="$2" '
    BEGIN { n = 0; while ((getline l < exp_file) > 0) { n++; e[n] = l } }
    { m++; a[m] = $0 }
    END {
      if (m != n) exit 1
      for (i = 1; i <= n; i++) {
        if (e[i] ~ /\.\.\.$/) {
          p = substr(e[i], 1, length(e[i]) - 3)
          if (substr(a[i], 1, length(p)) != p) exit 1
        } else if (a[i] != e[i]) exit 1
      }
    }
  ' "$1"
}

declare -a NAMES SRCS EXPS EXITS
add_test() {   # name src expected-file exit
  NAMES+=("$1"); SRCS+=("$2"); EXPS+=("$3"); EXITS+=("$4")
}

if [ $# -gt 0 ]; then
  CANDIDATES=("$@")
else
  CANDIDATES=(tests/bootstrap/test_bare_*.fd)
  for kt in "${KERNEL_TESTS[@]}"; do
    CANDIDATES+=("tests/bootstrap/${kt%%|*}.fd")
  done
fi
for src in "${CANDIDATES[@]}"; do
  name=$(basename "$src" .fd)
  exp="$OUT/$name.expected"
  found=0
  for kt in "${KERNEL_TESTS[@]}"; do
    if [ "${kt%%|*}" = "$name" ]; then
      rest="${kt#*|}"
      printf '%b' "${rest#*|}" > "$exp"
      [ -n "${rest#*|}" ] && echo >> "$exp"
      add_test "$name" "$src" "$exp" "${rest%%|*}"
      found=1
    fi
  done
  if [ $found = 0 ] && has_expected "$src"; then
    expected_of "$src" > "$exp"
    add_test "$name" "$src" "$exp" "$(exit_of "$src")"
  fi
done

pass=0; fail=0; failed=()
for i in "${!NAMES[@]}"; do
  name="${NAMES[$i]}"; src="${SRCS[$i]}"
  bin="$OUT/$name.bin"
  if [ "$BUILD" = 1 ]; then
    if ! "$FID" emit "$src" --bare -o "$bin" > "$OUT/$name.build.log" 2>&1; then
      echo "FAIL $name: build"; tail -5 "$OUT/$name.build.log" | sed 's/^/    /'
      fail=$((fail + 1)); failed+=("$name"); continue
    fi
  fi
  [ "$RUN" = 1 ] || { echo "built $name"; continue; }
  if [ ! -f "$bin" ]; then
    echo "FAIL $name: no $bin"; fail=$((fail + 1)); failed+=("$name"); continue
  fi
  timeout "$TIMEOUT" "$QEMU" -kernel "$bin" -serial stdio -display none \
      -no-reboot -nic none -m 128 \
      -device isa-debug-exit,iobase=0xf4,iosize=0x01 \
      < /dev/null > "$OUT/$name.out" 2> "$OUT/$name.err"
  rc=$?
  if [ $rc = 124 ]; then
    why="timeout after ${TIMEOUT}s"
    code=-1
  elif [ $((rc % 2)) = 1 ]; then
    code=$(( (rc - 1) / 2 ))
    why=""
  else
    code=-1
    why="QEMU exited with $rc (no debug exit: a triple fault resets, or QEMU failed)"
  fi
  if [ "$VERBOSE" = 1 ]; then sed 's/^/    | /' "$OUT/$name.out"; fi
  if [ -z "$why" ] && [ "$code" != "${EXITS[$i]}" ]; then
    why="exit code $code, expected ${EXITS[$i]}"
  fi
  if [ -z "$why" ] && ! same_output "$OUT/$name.out" "${EXPS[$i]}"; then
    why="output differs"
  fi
  if [ -z "$why" ]; then
    echo "ok   $name"
    pass=$((pass + 1))
  else
    echo "FAIL $name: $why"
    if [ "$why" = "output differs" ] || [ "$VERBOSE" != 1 ]; then
      diff "${EXPS[$i]}" "$OUT/$name.out" | head -20 | sed 's/^/    /'
      head -5 "$OUT/$name.err" | sed 's/^/    qemu: /'
    fi
    fail=$((fail + 1)); failed+=("$name")
  fi
done
[ "$RUN" = 1 ] || exit 0
echo "bare-metal under QEMU: $pass passed, $fail failed${failed:+ (${failed[*]})}"
[ $fail = 0 ]
