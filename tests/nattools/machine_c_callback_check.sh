#!/usr/bin/env bash
# Bugs roadmap BG166: C calling program functions on the machine layer
# (compiler/nat_ir.fd ng_cbx_label, ng_gen_cbx_adapters: an entry adapter
# per function a C function the dynamic linker binds is handed), run after
# the compiler is built:
#   bash tests/nattools/machine_c_callback_check.sh [fid]   (default release/fid.exe)
#
# tests/nattools/machine_c_callback.c is built as libcbk.so for each of
# x86_64-linux, aarch64-linux and riscv64-linux (zig cc, or ZIGCC), and
# tests/nattools/machine_c_callback.fd is emitted for it with FID_VERIFY=1
# (VERIFY=0: without; Nov 24 the x86-64 instruction selection check, RL6,
# stops on MO_SHI before and after this fix), by default and with
# FID_NAT_OFF=fregs. Where this host can run it (Linux
# x86-64 or WSL; AArch64 / RISC-V under qemu-user with QEMU_LD_PREFIX
# naming the target's glibc sysroot) each build must print the file's
# expected output; FID_NAT_OFF=cbadapt (no adapters, the address of the
# function itself, as before the fix) must not. On a Windows host the same
# program is also built against cbk.dll with `emit --nat` (x86_64-windows,
# the Win64 adapters) and with pe_ir, and run. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/nattools/machine_c_callback.fd
CSRC=tests/nattools/machine_c_callback.c
ZIGCC="${ZIGCC:-zig cc}"
VERIFY="${VERIFY:-1}"
OUT="${TMPDIR:-/tmp}/fid-machine-c-callback"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
# (wsl.exe with no distribution installed runs nothing: not a Linux host)
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e true >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <dir> <exe name> <target>: the program's output; status 99 when
# this host cannot run it
run_exe() {
    local dir="$1" exe="$2" t="$3" q=""
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
            (cd "$dir" && $WSL sh -c "command -v $q >/dev/null") || return 99
            (cd "$dir" && $WSL env QEMU_LD_PREFIX="$QEMU_LD_PREFIX" LD_LIBRARY_PATH=. $q "./$exe" 2>&1 | tr -d '\r')
            return 0
        elif ! command -v "$q" >/dev/null 2>&1; then
            return 99
        fi
    fi
    (cd "$dir" && $WSL env LD_LIBRARY_PATH=. $q "./$exe" 2>&1 | tr -d '\r')
}

for t in x86_64-linux aarch64-linux riscv64-linux; do
    dir="$OUT/$t"
    mkdir -p "$dir"
    # the soname without a directory (zig on Windows records the path given)
    if ! $ZIGCC -target "${t%%-*}-linux-gnu" -shared -fPIC -O1 -Wl,-soname,libcbk.so -o "$dir/libcbk.so" "$CSRC" >"$dir/lib.log" 2>&1; then
        bad "$t: building libcbk.so failed: $(tail -2 "$dir/lib.log")"
        continue
    fi
    for mode in default nofregs nocbadapt; do
        name="cbk-$mode"
        sw=(FID_NAT_DEFAULT=1)
        [ "$mode" = nofregs ] && sw=(FID_NAT_OFF=fregs)
        [ "$mode" = nocbadapt ] && sw=(FID_NAT_OFF=cbadapt)
        # (any FID_VERIFY value turns the checks on)
        [ "$VERIFY" = 1 ] && sw+=(FID_VERIFY=1)
        if ! env "${sw[@]}" "$FID" emit "$SRC" --target "$t" -o "$dir/$name" >"$dir/$name.log" 2>&1; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$dir/$name.log" | tail -2)"
            continue
        fi
        got=$(run_exe "$dir" "$name" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$t $mode: built, FID_VERIFY=$VERIFY (not run on this host)"
        elif [ "$mode" = nocbadapt ]; then
            if [ "$got" = "$WANT" ]; then
                bad "$t $mode: the address of the function itself gave the right output (the check sees nothing)"
            else
                ok "$t $mode: without adapters the output differs, as before the fix"
            fi
        elif [ "$got" = "$WANT" ]; then
            ok "$t $mode: output as expected"
        else
            bad "$t $mode: output differs"
            diff <(echo "$WANT") <(echo "$got") | head -10
        fi
    done
done

# x86_64-windows (`emit --nat`: the Win64 adapters) and pe_ir (`emit`, the
# default Windows backend), on a Windows host: cbk.dll beside the image.
# Nov 27, fid of Nov 25: default and nofregs print the expected output,
# nocbadapt prints 5 50000000 0 ... (the bare addresses), pe_ir as expected.
if [ "$HOST" = windows ]; then
    t=x86_64-windows
    dir="$OUT/$t"
    mkdir -p "$dir"
    if ! $ZIGCC -target x86_64-windows-gnu -shared -O1 -o "$dir/cbk.dll" "$CSRC" >"$dir/lib.log" 2>&1; then
        bad "$t: building cbk.dll failed: $(tail -2 "$dir/lib.log")"
    else
        for mode in default nofregs nocbadapt peir; do
            name="cbk-$mode.exe"
            sw=(FID_NAT_DEFAULT=1)
            flags=(--nat)
            [ "$mode" = nofregs ] && sw=(FID_NAT_OFF=fregs)
            [ "$mode" = nocbadapt ] && sw=(FID_NAT_OFF=cbadapt)
            [ "$mode" = peir ] && flags=()
            [ "$VERIFY" = 1 ] && sw+=(FID_VERIFY=1)
            if ! env "${sw[@]}" "$FID" emit "$SRC" "${flags[@]}" -o "$dir/$name" >"$dir/$mode.log" 2>&1; then
                bad "$t $mode: emit failed: $(grep -v '^\[' "$dir/$mode.log" | tail -2)"
                continue
            fi
            got=$(cd "$dir" && "./$name" 2>&1 | tr -d '\r')
            if [ "$mode" = nocbadapt ]; then
                if [ "$got" = "$WANT" ]; then
                    bad "$t $mode: the address of the function itself gave the right output (the check sees nothing)"
                else
                    ok "$t $mode: without adapters the output differs, as before the fix"
                fi
            elif [ "$got" = "$WANT" ]; then
                ok "$t $mode: output as expected"
            else
                bad "$t $mode: output differs"
                diff <(echo "$WANT") <(echo "$got") | head -10
            fi
        done
    fi
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
