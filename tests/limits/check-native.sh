#!/bin/bash
# tests/limits/check-native.sh [FID] [TARGET]: roadmap P12, the run-time
# resource limits of natively built programs (`fid emit`), the same checks
# as tests/limits/check.sh does for the C path:
#
#   FID_MEM_LIMIT=<bytes>[K|M|G]  live heap bytes; past it: exit 125 and
#                                 "fid: memory limit of N bytes exceeded"
#   FID_CPU_LIMIT=<seconds>       CPU time; past it: exit 124 and
#                                 "fid: CPU time limit of S seconds exceeded"
#
# TARGET: empty for the host's default `fid emit` (pe_ir on Windows), or a
# machine-layer target (x86_64-linux, aarch64-linux, riscv64-linux; on
# Windows these run in WSL, the last two under qemu-user), or `nat` for
# `fid emit --nat` (Windows x64 on the machine layer). Run from the project
# root.
#
#   bash tests/limits/check-native.sh release/fid.exe
#   bash tests/limits/check-native.sh release/fid.exe x86_64-linux
#   bash tests/limits/check-native.sh release/fid.exe aarch64-linux
#
# What each backend enforces (docs/reference/runtime-limits.md):
#   machine layer, Linux targets: memory and CPU time
#   --nat Windows:                memory and CPU time (a watcher thread
#                                 reading GetProcessTimes, win.fd)
#   macOS, WASI:                  memory only (the CPU check is skipped)
#   pe_ir (default Windows):      both, once its entry point calls
#                                 pert_limits_boot and its p_malloc/p_free
#                                 call pert_lim_take/pert_lim_give
set -u
FID="${1:-release/fid.exe}"
TARGET="${2:-}"
OUT=$(mktemp -d)
HOSTW=0
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*) HOSTW=1 ;;
esac
EMIT=()
EXE=""
RUNNER=""
INWSL=0
CPU=1
case "$TARGET" in
    "")             [ $HOSTW = 1 ] && EXE=".exe" ;;
    nat)            EMIT=(--nat); EXE=".exe" ;;
    x86_64-linux)   EMIT=(--target x86_64-linux); [ $HOSTW = 1 ] && INWSL=1 ;;
    aarch64-linux)  EMIT=(--target aarch64-linux); RUNNER=qemu-aarch64; [ $HOSTW = 1 ] && INWSL=1 ;;
    riscv64-linux)  EMIT=(--target riscv64-linux); RUNNER=qemu-riscv64; [ $HOSTW = 1 ] && INWSL=1 ;;
    *) echo "unknown target: $TARGET"; exit 2 ;;
esac
# a Linux host of the target's own architecture runs it without qemu
if [ $HOSTW = 0 ] && [ -n "$RUNNER" ]; then
    case "$(uname -m)-$RUNNER" in
        aarch64-qemu-aarch64|riscv64-qemu-riscv64) RUNNER="" ;;
    esac
fi
fail=0
ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }

# C:/x/y (or /c/x/y, a temporary folder) -> /mnt/c/x/y
wsl_path() {
    local p
    p=$(cygpath -m "$1" 2>/dev/null || echo "$1")
    local d=${p:0:1}
    echo "/mnt/${d,,}${p:2}"
}

# run <program> [VAR=value...]: stdout and stderr to $OUT/r.out, r.err
run() {
    local prog="$1"; shift
    if [ $INWSL = 1 ]; then
        MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e env "$@" $RUNNER "$(wsl_path "$prog")" > "$OUT/r.out" 2> "$OUT/r.err"
    else
        env "$@" $RUNNER "$prog" > "$OUT/r.out" 2> "$OUT/r.err"
    fi
    local code=$?
    nocr "$OUT/r.out" "$OUT/r.err"
    return $code
}

for p in mem_hog cpu_spin small; do
    if ! "$FID" emit "tests/limits/$p.fd" "${EMIT[@]}" -o "$OUT/$p$EXE" > "$OUT/$p.build" 2>&1; then
        bad "build $p" "$(tail -3 "$OUT/$p.build")"
    fi
done

# memory: 20000 strings of 1 KB do not fit in 8 MB
run "$OUT/mem_hog$EXE" FID_MEM_LIMIT=8M
code=$?
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 8388608 bytes exceeded (FID_MEM_LIMIT)" "$OUT/r.err"; then
    ok "memory limit stops the program (exit 125)"
else
    bad "memory limit" "exit $code: $(head -c 300 "$OUT/r.err")"
fi
if grep -q "^kept" "$OUT/r.out"; then
    bad "memory limit" "the program finished: $(cat "$OUT/r.out")"
fi

# the same program without a limit finishes
run "$OUT/mem_hog$EXE" FID_UNUSED=1
code=$?
if [ $code -eq 0 ] && grep -q "^kept 20000$" "$OUT/r.out"; then
    ok "no limit: the program finishes"
else
    bad "no limit" "exit $code: $(head -c 300 "$OUT/r.err")"
fi

# CPU time: a minute of work against a 1-second limit
if [ $CPU = 1 ]; then
    run "$OUT/cpu_spin$EXE" FID_CPU_LIMIT=1
    code=$?
    if [ $code -eq 124 ] && grep -q "^fid: CPU time limit of 1 seconds exceeded (FID_CPU_LIMIT)" "$OUT/r.err"; then
        ok "CPU time limit stops the program (exit 124)"
    else
        bad "CPU time limit" "exit $code: $(head -c 300 "$OUT/r.err")"
    fi
else
    echo "  skip  CPU time limit (not enforced on this target)"
fi

# under both limits: normal output and exit code
run "$OUT/small$EXE" FID_MEM_LIMIT=64M FID_CPU_LIMIT=30
code=$?
if [ $code -eq 0 ] && grep -q "^small ok 0$" "$OUT/r.out"; then
    ok "under the limits: runs normally"
else
    bad "under the limits" "exit $code: $(head -c 300 "$OUT/r.err")"
fi

# a malformed limit is an error, not ignored
run "$OUT/small$EXE" FID_MEM_LIMIT=lots
code=$?
if [ $code -eq 2 ] && grep -q "FID_MEM_LIMIT is not a size" "$OUT/r.err"; then
    ok "malformed FID_MEM_LIMIT rejected (exit 2)"
else
    bad "malformed FID_MEM_LIMIT" "exit $code"
fi

rm -rf "$OUT"
if [ $fail -ne 0 ]; then
    echo "limits (${TARGET:-emit}): $fail check(s) failed"
    exit 1
fi
echo "limits (${TARGET:-emit}): all checks passed"
