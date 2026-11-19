#!/usr/bin/env bash
# MI25: f32 values at the C boundary of the machine layer
# (compiler/nat_ir.fd ng_gen_ldyn_stubs / ng_gen_ldyn_stubs_risc, the
# p_f32_to_bits builtin), run after the compiler is built:
#   bash tests/nattools/machine_f32_ffi_check.sh [fid]   (default release/fid.exe)
#
# tests/nattools/machine_f32_ffi.fd calls libm's f32 functions
# (hypotf, copysignf, fminf, fmaxf, fmaf, ldexpf) through the dynamic
# imports the machine layer writes (CI2 / TG15) and prints the bits of
# f32 values. For x86_64-linux, aarch64-linux and riscv64-linux it is built
# with FID_VERIFY=1 three ways: by default (floats in float registers),
# FID_NAT_ON=fregs and FID_NAT_OFF=fregs (every float in a general
# register). Each build must pass tests/nattools/ldyn_check.py (the
# dynamic tables, the f32 functions imported) and, where this host can run
# it, print the file's expected output. x86-64 runs directly on Linux or
# through WSL on Windows; AArch64 / RISC-V run under qemu-aarch64 /
# qemu-riscv64 only when QEMU_LD_PREFIX names the target's glibc sysroot
# (they are dynamic executables), else they are built and checked only.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/nattools/machine_f32_ffi.fd
OUT="${TMPDIR:-/tmp}/fid-machine-f32-ffi"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <exe name> <target>: the program's output; status 99 when this
# host cannot run it
run_exe() {
    local exe="$1" t="$2" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
    if [ -n "$q" ]; then
        if [ "$(uname -m)" = "${t%%-*}" ] && [ -z "$WSL" ]; then
            q=""
        elif [ -z "${QEMU_LD_PREFIX:-}" ]; then
            return 99
        elif [ -n "$WSL" ]; then
            (cd "$OUT" && $WSL sh -c "command -v $q >/dev/null") || return 99
            (cd "$OUT" && $WSL env QEMU_LD_PREFIX="$QEMU_LD_PREFIX" $q "./$exe" 2>&1 | tr -d '\r')
            return 0
        elif ! command -v "$q" >/dev/null 2>&1; then
            return 99
        fi
    fi
    (cd "$OUT" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
}

for t in x86_64-linux aarch64-linux riscv64-linux; do
    for mode in default on off; do
        name="f32ffi-$t-$mode"
        sw=(FID_NAT_DEFAULT=1)
        [ "$mode" = on ] && sw=(FID_NAT_ON=fregs)
        [ "$mode" = off ] && sw=(FID_NAT_OFF=fregs)
        if ! env FID_VERIFY=1 "${sw[@]}" "$FID" emit "$SRC" --target "$t" -o "$OUT/$name" >"$OUT/$name.log" 2>&1; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$name.log" | tail -2)"
            continue
        fi
        if python3 tests/nattools/ldyn_check.py "$OUT/$name" hypotf copysignf fminf fmaxf fmaf ldexpf >"$OUT/$name.ldyn" 2>&1; then
            ok "$t $mode: dynamic imports of the f32 functions"
        else
            bad "$t $mode: ldyn_check: $(tail -2 "$OUT/$name.ldyn" | tr '\n' ' ')"
        fi
        got=$(run_exe "$name" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$t $mode: built with FID_VERIFY=1 (not run on this host)"
        elif [ "$got" = "$WANT" ]; then
            ok "$t $mode: output as expected"
        else
            bad "$t $mode: output differs"
            diff <(echo "$WANT") <(echo "$got") | head -10
        fi
    done
done

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
