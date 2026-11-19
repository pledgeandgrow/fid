#!/bin/bash
# tl6-cross-route-check.sh — scripts/cross-test.sh on a Linux host builds
# the Windows lanes for Windows and runs them under wine (TL6), checked with
# a stand-in fid and a stand-in wine (no compiler is built or run):
#   1. NATIVE=1 x86_64-windows-pe passes `--target x86_64-windows` to
#      `fid emit` (before TL6: no target, so a Linux host wrote an ELF)
#   2. NATIVE=1 x86_64-windows passes `--target x86_64-windows --nat`
#   3. with WINE set, the executables run under it and are compared with
#      `fid run` (before TL6: "built only" on every non-Windows host)
#   4. BUILD_ONLY=1 still only builds
# On Windows (Git Bash) the check runs itself in WSL.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    W=$(cygpath -m "$ROOT" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
    MSYS_NO_PATHCONV=1 exec wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$W/tests/tooling/tl6-cross-route-check.sh" ;;
esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }

# The stand-in fid: `run` prints hello's output; `emit ... -o X` records its
# arguments and writes X as "MZ" and a shell script printing the same
cat > "$TMP/fid" << 'EOF'
#!/bin/bash
echo "$*" >> "$FAKE_LOG"
case "$1" in
  run) printf 'Hello, Fides!\n50\ny is big\n'; exit 0 ;;
  emit)
    out=""; prev=""
    for a in "$@"; do [ "$prev" = -o ] && out="$a"; prev="$a"; done
    printf 'MZ\nprintf "Hello, Fides!\\r\\n50\\r\\ny is big\\r\\n"\n' > "$out"; exit 0 ;;
esac
exit 1
EOF
# The stand-in wine: `wineboot` does nothing; an .exe is the script after "MZ"
cat > "$TMP/wine" << 'EOF'
#!/bin/bash
[ "$1" = wineboot ] && exit 0
echo "wine $1" >> "$FAKE_LOG"
tail -n +2 "$1" | bash
EOF
chmod +x "$TMP/fid" "$TMP/wine"
export FAKE_LOG="$TMP/calls.log"

lane() {  # lane <target> <extra env...>: run cross-test.sh on hello.fd
  local t="$1"; shift
  : > "$FAKE_LOG"
  env "$@" NATIVE=1 FID="$TMP/fid" OUT="$TMP/out-$t" JOBS=1 bash scripts/cross-test.sh "$t" tests/bootstrap/hello.fd > "$TMP/$t.log" 2>&1
}

lane x86_64-windows-pe WINE="$TMP/wine"
if grep -q -- "^emit tests/bootstrap/hello.fd --target x86_64-windows -o " "$FAKE_LOG"; then
  ok "1. x86_64-windows-pe: emit --target x86_64-windows"
else bad "1. x86_64-windows-pe: emit was called as: $(grep '^emit' "$FAKE_LOG")"; fi
if grep -q "^x86_64-windows-pe: 1 passed, 0 failed" "$TMP/x86_64-windows-pe.log" && grep -q "^wine " "$FAKE_LOG"; then
  ok "3. the PE ran under WINE and matched fid run"
else bad "3. not run under WINE: $(grep '^x86_64-windows-pe:' "$TMP/x86_64-windows-pe.log")"; fi

lane x86_64-windows WINE="$TMP/wine"
if grep -q -- "^emit tests/bootstrap/hello.fd --target x86_64-windows --nat -o " "$FAKE_LOG"; then
  ok "2. x86_64-windows: emit --target x86_64-windows --nat"
else bad "2. x86_64-windows: emit was called as: $(grep '^emit' "$FAKE_LOG")"; fi

lane x86_64-windows-pe WINE="$TMP/wine" BUILD_ONLY=1
if grep -q "1 built only" "$TMP/x86_64-windows-pe.log" && ! grep -q "^wine " "$FAKE_LOG"; then
  ok "4. BUILD_ONLY=1: built, not run"
else bad "4. BUILD_ONLY=1: $(grep '^x86_64-windows-pe:' "$TMP/x86_64-windows-pe.log")"; fi

if [ "$fail" = 0 ]; then echo "tl6-cross-route-check: PASS"; else echo "tl6-cross-route-check: FAIL"; fi
exit "$fail"
