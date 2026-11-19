#!/usr/bin/env bash
# CT6 / DB4 / CT9 (docs/roadmap/complaint-roadmap.md, llvm-roadmap.md):
# what a debugger sees in native Linux builds, on the three targets, in
# the optimized build and in the debug build (`fid emit --debug`).
#
#   FID=release/fid.exe bash tests/nattools/dbg_ct6_check.sh
#
# Run from the repository root, in Linux or WSL (qemu-x86_64, qemu-aarch64,
# qemu-riscv64 and python3 on PATH; FID may be the Windows fid.exe).
# For each target:
#   - test_machine_dbg_run.fd, optimized: dbg_run_check.py (breakpoints
#     from .debug_line, values from .debug_info/.debug_loc; `!` values must
#     be available, every available value must be right);
#   - the same built with --debug: dbg_run_check.py --strict (every value
#     available and right);
#   - test_machine_dbg_names.fd: dbg_names_check.py (CT9);
#   - test_machine_dbg_vars.fd: dbg_vars_check.py (DB4 entries; location
#     lists included);
#   - with tests/nattools/dbg_gdb_check.sh when gdb-multiarch is installed.
# EXTRA_OFF=<passes> adds FID_NAT_OFF passes to every build (for example
# ldyn when the dynamic imports do not resolve in qemu).
# Exit 0 when every check holds.
set -u
FID="${FID:-release/fid.exe}"
OUT="$(mktemp -d)"
fail=0
off="${EXTRA_OFF:-}"
run_fid() {
    # a Windows fid.exe under WSL writes the -o path as a Windows path
    local args=()
    local next_o=0
    for a in "$@"; do
        if [ $next_o = 1 ] && [[ "$FID" == *.exe ]] && command -v wslpath >/dev/null 2>&1; then
            a="$(wslpath -w "$a")"
        fi
        next_o=0
        [ "$a" = "-o" ] && next_o=1
        args+=("$a")
    done
    if [ -n "$off" ]; then
        FID_NAT_OFF="$off" WSLENV="${WSLENV:+$WSLENV:}FID_NAT_OFF" "$FID" "${args[@]}"
    else
        "$FID" "${args[@]}"
    fi
}
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | grep '^// ' | grep -v 'Expected output' | sed 's#^// ##'; }
for t in x86_64-linux aarch64-linux riscv64-linux; do
    runner=qemu-x86_64
    case "$t" in aarch64-linux) runner=qemu-aarch64 ;; riscv64-linux) runner=qemu-riscv64 ;; esac
    for mode in opt debug; do
        flag=""
        strict=""
        if [ "$mode" = debug ]; then flag="--debug"; strict="--strict"; fi
        exe="$OUT/run-$t-$mode"
        if ! run_fid emit tests/bootstrap/test_machine_dbg_run.fd --target "$t" $flag -o "$exe" > "$exe.log" 2>&1; then
            echo "FAIL: build dbg_run $t $mode"; tail -3 "$exe.log"; fail=1; continue
        fi
        chmod +x "$exe"
        if ! diff <(expected tests/bootstrap/test_machine_dbg_run.fd) <($runner "$exe" 2>&1) > /dev/null; then
            echo "FAIL: dbg_run $t $mode: output differs"; fail=1
        fi
        echo "== dbg_run $t $mode"
        python3 tests/nattools/dbg_run_check.py "$exe" tests/bootstrap/test_machine_dbg_run.fd $strict || fail=1
    done
    exe="$OUT/names-$t"
    if run_fid emit tests/bootstrap/test_machine_dbg_names.fd --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        echo "== dbg_names $t"
        python3 tests/nattools/dbg_names_check.py "$exe" || fail=1
    else
        echo "FAIL: build dbg_names $t"; tail -3 "$exe.log"; fail=1
    fi
    exe="$OUT/vars-$t"
    if run_fid emit tests/bootstrap/test_machine_dbg_vars.fd --target "$t" -o "$exe" > "$exe.log" 2>&1; then
        echo "== dbg_vars $t"
        python3 tests/nattools/dbg_vars_check.py "$exe" tests/bootstrap/test_machine_dbg_vars.fd || fail=1
    else
        echo "FAIL: build dbg_vars $t"; tail -3 "$exe.log"; fail=1
    fi
    if command -v gdb-multiarch >/dev/null 2>&1 || { [ "$t" = x86_64-linux ] && command -v gdb >/dev/null 2>&1; }; then
        echo "== gdb $t"
        FID="$FID" bash tests/nattools/dbg_gdb_check.sh "$t" || fail=1
    fi
done
rm -rf "$OUT"
[ $fail = 0 ] && echo "CT6/DB4/CT9 debug checks OK"
exit $fail
