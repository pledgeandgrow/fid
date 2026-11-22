#!/bin/bash
# pl2-lane-skips-check.sh — what scripts/cross-test.sh leaves out of a lane
# and why (PL2), checked with a stand-in fid and a stand-in qemu-aarch64
# (no compiler is built or run). The stand-in fid's `emit` writes small
# ELF files (tests/tooling/bin-target-check-selftest.py --make):
#   1. a test scripts/lane-skips.txt-style list names for the target is
#      not built, and results.txt says "SKIP  <test> (<reason>)"; a `!`
#      pattern keeps it on the targets it names
#   2. a dynamic executable that imports only C functions, with no loader
#      for qemu (QEMU_LD_PREFIX without one), is SKIP ("dynamic: needs
#      /lib/ld-linux-aarch64.so.1"), and qemu is not started for it
#   3. with the loader under QEMU_LD_PREFIX it runs and passes
#   4. a dynamic executable that imports nat_tls_set (a runtime name: the
#      compiler's bug) is not spared: it runs and FAILS
#   5. a static executable runs and passes
# On Windows (Git Bash) the check runs itself in WSL; it needs python3.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    W=$(cygpath -m "$ROOT" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
    MSYS_NO_PATHCONV=1 exec wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$W/tests/tooling/pl2-lane-skips-check.sh" ;;
esac
command -v python3 > /dev/null 2>&1 || { echo "pl2-lane-skips-check: needs python3"; exit 1; }
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }

# The tests: their names tell the stand-in fid what to write
for t in t_static t_cdyn t_rtdyn t_skipme; do echo 'fn main() { println("hi"); }' > "$TMP/$t.fd"; done
mkdir -p "$TMP/bin"
cat > "$TMP/bin/fid" << EOF
#!/bin/bash
echo "\$*" >> "\$FAKE_LOG"
case "\$1" in
  run) echo hi; exit 0 ;;
  emit)
    out=""; prev=""; src="\$2"
    for a in "\$@"; do [ "\$prev" = -o ] && out="\$a"; prev="\$a"; done
    kind=static
    case "\$src" in *t_cdyn.fd) kind=c ;; *t_rtdyn.fd) kind=runtime ;; esac
    python3 "$ROOT/tests/tooling/bin-target-check-selftest.py" --make "\$out" \$kind aarch64; exit \$? ;;
esac
exit 1
EOF
# The stand-in qemu-aarch64: a binary importing nat_tls_set stops as the
# real loader does; anything else prints the test's output
cat > "$TMP/bin/qemu-aarch64" << 'EOF'
#!/bin/bash
echo "qemu $1" >> "$FAKE_LOG"
if grep -aq nat_tls_set "$1"; then echo "$1: symbol lookup error: undefined symbol: nat_tls_set" >&2; exit 127; fi
echo hi
EOF
chmod +x "$TMP/bin/fid" "$TMP/bin/qemu-aarch64"
export FAKE_LOG="$TMP/calls.log"
printf '# test list\n!*-windows*  t_skipme  needs the System32 DLLs\nx86_64-linux  t_static  only an x86-64 skip\n' > "$TMP/skips.txt"
mkdir -p "$TMP/noloader" "$TMP/sysroot/lib"
: > "$TMP/sysroot/lib/ld-linux-aarch64.so.1"

lane() {  # lane <name> <QEMU_LD_PREFIX>: the four tests on aarch64-linux
  : > "$FAKE_LOG"
  # HOST_ARCH=x86_64: the check is of the qemu path; on an aarch64 host
  # (ubuntu-24.04-arm, PL2) cross-test.sh would run the fake ELFs itself
  PATH="$TMP/bin:$PATH" QEMU_LD_PREFIX="$2" NATIVE=1 FID="$TMP/bin/fid" OUT="$TMP/out-$1" JOBS=1 \
    HOST_ARCH=x86_64 LANE_SKIPS="$TMP/skips.txt" bash scripts/cross-test.sh aarch64-linux \
    "$TMP/t_static.fd" "$TMP/t_cdyn.fd" "$TMP/t_rtdyn.fd" "$TMP/t_skipme.fd" > "$TMP/$1.log" 2>&1
}
res() { grep -a "^[A-Z]* *$2\b" "$TMP/out-$1-native/results.txt" 2>/dev/null; }

lane noload "$TMP/noloader"
if res noload t_skipme | grep -q '^SKIP  t_skipme (needs the System32 DLLs)' && ! grep -q 't_skipme' "$FAKE_LOG"; then
  ok "1. listed test skipped with its reason, never built"
else bad "1. t_skipme: '$(res noload t_skipme)'; calls: $(grep -c t_skipme "$FAKE_LOG")"; fi
if res noload t_static | grep -q '^PASS'; then
  ok "1. a pattern for another target (x86_64-linux) does not skip on aarch64-linux"
else bad "1. t_static: '$(res noload t_static)'"; fi
if res noload t_cdyn | grep -q '^SKIP  t_cdyn (dynamic: needs /lib/ld-linux-aarch64.so.1' && ! grep -q '^qemu .*t_cdyn' "$FAKE_LOG"; then
  ok "2. C-only dynamic executable without a loader: SKIP, not run"
else bad "2. t_cdyn: '$(res noload t_cdyn)'"; fi
if res noload t_rtdyn | grep -q '^FAIL' && grep -q '^qemu .*t_rtdyn' "$FAKE_LOG"; then
  ok "4. importing nat_tls_set: run, FAIL"
else bad "4. t_rtdyn: '$(res noload t_rtdyn)'"; fi
if grep -q '^aarch64-linux: 1 passed, 1 failed, 0 did not build, 0 without reference, 1 skipped on this target, 1 dynamic not run' "$TMP/noload.log"; then
  ok "summary counts the skips"
else bad "summary: $(grep '^aarch64-linux:' "$TMP/noload.log")"; fi

lane withloader "$TMP/sysroot"
if res withloader t_cdyn | grep -q '^PASS' && grep -q '^qemu .*t_cdyn' "$FAKE_LOG"; then
  ok "3. with the loader under QEMU_LD_PREFIX: run, PASS"
else bad "3. t_cdyn: '$(res withloader t_cdyn)'"; fi
if res withloader t_static | grep -q '^PASS'; then
  ok "5. static executable: PASS"
else bad "5. t_static: '$(res withloader t_static)'"; fi

if [ "$fail" = 0 ]; then echo "pl2-lane-skips-check: PASS"; else echo "pl2-lane-skips-check: FAIL"; fi
exit "$fail"
