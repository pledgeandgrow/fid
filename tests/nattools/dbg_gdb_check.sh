#!/usr/bin/env bash
# CT6 / DB4 / CT9: what gdb itself shows for a native Linux build of
# tests/bootstrap/test_machine_dbg_run.fd (dbg_run_check.py checks the same
# without gdb, by speaking the remote protocol). Run in Linux or WSL with
# qemu-user and gdb-multiarch (or gdb for the host's architecture):
#
#   FID=release/fid.exe bash tests/nattools/dbg_gdb_check.sh [target] [--debug]
#
# target: x86_64-linux (default), aarch64-linux, riscv64-linux. With
# --debug the build is `fid emit --debug` (DB4: every value available);
# without it, the optimized build (CT6). Builds the test, starts it under
# `qemu-<arch> -g PORT`, and runs gdb in batch mode: a breakpoint on the
# line marked `// @loop` (scale's loop body), `info args`, `info locals`,
# `ptype struct Seg`, `info functions pick` and a breakpoint on
# 'Point::len2'. Passes when gdb shows base = 6 and factor = 7 (and, for
# the optimized build too, acc = 0 and step = 0 on the first stop), the
# struct Seg with its members a, b and tag, the function pick<i64> and
# the method Point::len2. Exit 0 when every check holds; 2 when gdb or
# qemu is missing (nothing checked).
set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
FID="${FID:-$ROOT/release/fid.exe}"
TARGET="x86_64-linux"
DEBUG=""
for a in "$@"; do
    case "$a" in
        --debug) DEBUG="--debug" ;;
        *) TARGET="$a" ;;
    esac
done
case "$TARGET" in
    x86_64-linux) QEMU=qemu-x86_64; ARCH=i386:x86-64 ;;
    aarch64-linux) QEMU=qemu-aarch64; ARCH=aarch64 ;;
    riscv64-linux) QEMU=qemu-riscv64; ARCH=riscv:rv64 ;;
    *) echo "unknown target $TARGET"; exit 2 ;;
esac
GDB=""
for g in gdb-multiarch gdb; do
    if command -v "$g" >/dev/null 2>&1; then GDB="$g"; break; fi
done
if [ -z "$GDB" ] || ! command -v "$QEMU" >/dev/null 2>&1; then
    echo "SKIP: needs gdb-multiarch (or gdb) and $QEMU"
    exit 2
fi
SRC="$ROOT/tests/bootstrap/test_machine_dbg_run.fd"
OUT="$(mktemp -d)"
EXE="$OUT/t"
EXE_ARG="$EXE"
if [[ "$FID" == *.exe ]] && command -v wslpath >/dev/null 2>&1; then
    # a Windows fid.exe under WSL: Windows paths
    EXE_ARG="$(wslpath -w "$EXE")"
fi
(cd "$ROOT" && "$FID" emit tests/bootstrap/test_machine_dbg_run.fd --target "$TARGET" $DEBUG -o "$EXE_ARG") >"$OUT/build.log" 2>&1 || {
    echo "FAIL: build"; tail -5 "$OUT/build.log"; exit 1; }
chmod +x "$EXE"
LOOP=$(grep -n '// @loop$' "$SRC" | head -1 | cut -d: -f1)
PORT=$((20000 + RANDOM % 30000))
"$QEMU" -g "$PORT" "$EXE" >"$OUT/run.log" 2>&1 &
QPID=$!
sleep 0.5
"$GDB" -batch -nx \
    -ex "set architecture $ARCH" \
    -ex "file $EXE" \
    -ex "target remote 127.0.0.1:$PORT" \
    -ex "break test_machine_dbg_run.fd:$LOOP" \
    -ex "continue" \
    -ex "info args" \
    -ex "info locals" \
    -ex "ptype struct Seg" \
    -ex "info functions pick" \
    -ex "break 'Point::len2'" \
    -ex "continue" \
    -ex "info args" \
    -ex "print self" \
    -ex "kill" >"$OUT/gdb.log" 2>&1
kill "$QPID" 2>/dev/null
wait "$QPID" 2>/dev/null
cat "$OUT/gdb.log"
fails=0
want() {
    if ! grep -Eq "$1" "$OUT/gdb.log"; then
        echo "FAIL: gdb output has no match for: $1"
        fails=$((fails + 1))
    fi
}
want 'base = 6'
want 'factor = 7'
want 'acc = 0'
want 'step = 0'
want 'struct Seg \{'
want 'tag;'
want 'pick<i64>'
want "Breakpoint 2 at .*"
want 'x = 3'
if [ "$fails" -eq 0 ]; then
    echo "dbg gdb OK ($TARGET${DEBUG:+ debug})"
    exit 0
fi
exit 1
