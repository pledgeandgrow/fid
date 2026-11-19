#!/usr/bin/env bash
# Checks of the machine-layer goals MI4/RA10, WP11, CT9 and CI2
# (docs/roadmap/llvm-roadmap.md, docs/roadmap/complaint-roadmap.md), run
# after the compiler is built:
#   bash tests/nattools/machine_goals_check.sh [fid]   (default release/fid.exe)
# Linux executables run directly on Linux, through WSL on Windows
# (RUN="wsl.exe -e" is picked when wsl.exe exists). Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-goals"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

run_linux() {  # file target
    local exe="$OUT/$(basename "$1" .fd)-$2"
    if ! FID_VERIFY=1 "$FID" emit "$1" --target "$2" -o "$exe" >"$exe.log" 2>&1; then
        bad "$1 $2: emit failed: $(tail -2 "$exe.log")"; return 1
    fi
    if [ "$2" = x86_64-linux ]; then
        local got
        got=$($RUN "$exe" 2>&1 | tr -d '\r')
        if [ "$got" = "$(expected "$1")" ]; then ok "$1 $2"; else bad "$1 $2: output differs"; echo "$got" | head -5; fi
    else
        ok "$1 $2 (built with FID_VERIFY=1)"
    fi
}

# MI4/RA10: the target description and the class check (FID_VERIFY=1 runs
# the allocation checker, which now checks each register's class)
for t in x86_64-linux aarch64-linux riscv64-linux; do
    run_linux tests/bootstrap/test_machine_remat.fd "$t"
    run_linux tests/bootstrap/test_machine_crashfree.fd "$t"
done

# WP11: a wrong @crashfree is rejected on the emit path
for t in x86_64-linux aarch64-linux riscv64-linux; do
    if "$FID" emit tests/bootstrap/test_machine_crashfree_fail.fd --target "$t" -o "$OUT/cf_fail" >"$OUT/cf_fail.log" 2>&1; then
        bad "WP11 $t: test_machine_crashfree_fail.fd built"
    elif grep -q "@crashfree violated: function 'bump'" "$OUT/cf_fail.log"; then
        ok "WP11 $t: wrong @crashfree rejected"
    else
        bad "WP11 $t: rejected for another reason: $(tail -2 "$OUT/cf_fail.log")"
    fi
done

# CT9: Fides names and struct types in .debug_info
if "$FID" emit tests/bootstrap/test_machine_dbg_names.fd --target x86_64-linux -o "$OUT/dbg" >"$OUT/dbg.log" 2>&1 \
   && python3 tests/nattools/dbg_names_check.py "$OUT/dbg"; then
    ok "CT9 debug names"
else
    bad "CT9 debug names"
fi

# CI2: libm/libc calls with no C compiler (needs a glibc x86-64 Linux)
run_linux tests/bootstrap/test_machine_cimport.fd x86_64-linux

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
