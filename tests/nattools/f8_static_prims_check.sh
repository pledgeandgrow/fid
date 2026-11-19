#!/usr/bin/env bash
# F8: a native Linux executable defines the machine layer's own primitives
# (nat_tls_set, nat_arch, nat_atomic_*, nat_clone_thread, ...) and never
# imports them from a shared library (compiler/nat_ir.fd ng_gen_call_std,
# ng_is_own_prim). stdlib/linux_runtime declares the primitives in an
# `extern "C"` block; CI2's dynamic imports took them for C functions, so
# every Linux executable named ld-linux and libc.so.6, imported nat_*, and
# stopped at load ("Could not open /lib/ld-linux-aarch64.so.1" under
# qemu-user, "symbol lookup error: undefined symbol: nat_tls_set" with a
# loader). Run after the compiler is built:
#   bash tests/nattools/f8_static_prims_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux and riscv64-linux:
#   - hello.fd, test_threads.fd and test_tasks.fd (threads, atomics, task
#     switching: every primitive group) are built with `fid emit --target`;
#     scripts/bin-target-check.py must pass them (no runtime name imported)
#     and its --loader report must give no interpreter ("-"): they call no
#     C function, so they are static;
#   - test_machine_ldyn_targets.fd (calls atoi/hypot/ldexp of the C
#     library) must still be dynamic: an interpreter, and no nat_* name
#     among its imports (bin-target-check passes it);
#   - where this host can run the target (Linux, or WSL on Windows, with
#     qemu-aarch64 / qemu-riscv64 for the other machines) the three static
#     programs must print what `fid run` prints, with its exit code.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-f8-static"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
PY=""
for p in python3 python; do
    command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null && { PY=$p; break; }
done
[ -n "$PY" ] || { echo "f8_static_prims_check: needs python 3"; exit 1; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe > /dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <exe name> <target>: runs it in $OUT, its output in <exe>.out and
# exit code in <exe>.rc; status 99 when this host cannot run it
run_exe() {
    local exe="$1" t="$2" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
    if [ -n "$q" ]; then
        if [ -n "$WSL" ]; then
            (cd "$OUT" && $WSL sh -c "command -v $q >/dev/null") || return 99
        elif ! command -v "$q" > /dev/null 2>&1; then
            if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
        fi
    elif [ "$HOST" = linux ] && [ "$(uname -m)" != x86_64 ]; then
        return 99
    fi
    (cd "$OUT" && timeout 120 $WSL $q "./$exe" < /dev/null 2> /dev/null | tr -d '\r' > "$exe.out"; echo "${PIPESTATUS[0]}" > "$exe.rc")
    return 0
}

STATIC="hello test_threads test_tasks"
for t in x86_64-linux aarch64-linux riscv64-linux; do
    for b in $STATIC; do
        exe="$b.$t"
        if ! "$FID" emit "tests/bootstrap/$b.fd" --target "$t" -o "$OUT/$exe" > "$OUT/$exe.log" 2>&1; then
            bad "$t $b: does not build ($OUT/$exe.log)"
            continue
        fi
        if "$PY" scripts/bin-target-check.py "$t" "$OUT/$exe" --quiet > "$OUT/$exe.chk" 2>&1; then
            ok "$t $b: imports none of the runtime's own names"
        else
            bad "$t $b: $(head -1 "$OUT/$exe.chk")"
        fi
        interp=$("$PY" scripts/bin-target-check.py --loader "$OUT/$exe" | tr -d '\r' | cut -f2)
        if [ "$interp" = "-" ]; then
            ok "$t $b: static (no program interpreter)"
        else
            bad "$t $b: names the loader $interp although it calls no C function"
        fi
        run_exe "$exe" "$t"
        if [ $? = 99 ]; then
            echo "skip $t $b: this host cannot run it"
            continue
        fi
        if [ ! -f "$OUT/$b.ref" ]; then
            timeout 120 "$FID" run "tests/bootstrap/$b.fd" < /dev/null 2> /dev/null | tr -d '\r' > "$OUT/$b.ref"
            echo "${PIPESTATUS[0]}" > "$OUT/$b.ref_rc"
        fi
        if cmp -s "$OUT/$exe.out" "$OUT/$b.ref" && [ "$(cat "$OUT/$exe.rc")" = "$(cat "$OUT/$b.ref_rc")" ]; then
            ok "$t $b: runs as \`fid run\` does"
        else
            bad "$t $b: output or exit code ($(cat "$OUT/$exe.rc"), fid run $(cat "$OUT/$b.ref_rc")) differs from \`fid run\` ($OUT/$exe.out)"
        fi
    done
    # A program that does call C functions stays dynamic, without nat_*
    exe="ldyn.$t"
    if ! "$FID" emit tests/bootstrap/test_machine_ldyn_targets.fd --target "$t" -o "$OUT/$exe" > "$OUT/$exe.log" 2>&1; then
        bad "$t test_machine_ldyn_targets: does not build ($OUT/$exe.log)"
        continue
    fi
    line=$("$PY" scripts/bin-target-check.py --loader "$OUT/$exe" | tr -d '\r')
    interp=$(echo "$line" | cut -f2)
    rt=$(echo "$line" | cut -f3)
    if [ "$interp" != "-" ] && [ "$rt" = "-" ]; then
        ok "$t test_machine_ldyn_targets: dynamic ($interp), no runtime name imported"
    else
        bad "$t test_machine_ldyn_targets: interpreter '$interp', runtime imports '$rt'"
    fi
done
if [ "$fails" = 0 ]; then
    echo "f8_static_prims_check: ok"
    exit 0
fi
echo "f8_static_prims_check: $fails failed"
exit 1
