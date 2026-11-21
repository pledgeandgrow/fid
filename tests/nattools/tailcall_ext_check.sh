#!/usr/bin/env bash
# 2g.98 (docs/reference/known-bugs.md; MI28 in docs/roadmap/llvm-roadmap.md),
# the machine layer's half: mutual and sibling tail calls three million
# levels deep must run in one frame on every Linux target of the machine
# layer and on --nat Windows. Before, AArch64 and RISC-V overflowed the
# stack in tests/nattools/tailcall_deep.fd (is_odd called from is_odd):
# after inlining, the extension of a bool result into a promoted local
# stood between the call and the return, and compiler/nat_ir.fd ng_tail_at
# took only moves there (ng_tail_ext_kept: an extension the callee's
# narrow result already satisfies is a move).
#   bash tests/nattools/tailcall_ext_check.sh [fid]   (default release/fid.exe)
# For tests/nattools/tailcall_deep.fd and tailcall_narrow_deep.fd, each
# target (x86_64-linux, aarch64-linux, riscv64-linux, --nat Windows), with
# the IR's own self-recursion loop on and off (FID_IROPT_OFF=tailcall): the
# build must succeed and the run print the file's expected output. Linux
# images run directly on Linux (AArch64 / RISC-V under qemu-aarch64 /
# qemu-riscv64), through WSL on Windows (`wsl.exe -e`, from the image's
# folder: no Windows path is passed); a target whose runner is missing is
# built only. tests/bootstrap/test_machine_tailcall_narrow.fd is the
# shallow form every backend runs. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-tailcall-ext"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

run_exe() {
    local dir="$1" exe="$2" t="$3" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$dir" && "./$exe" 2>&1 | tr -d '\r')
            ;;
        *)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$dir" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$dir" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

for src in tests/nattools/tailcall_deep.fd tests/nattools/tailcall_narrow_deep.fd; do
    base=$(basename "$src" .fd)
    want="$(expected "$src")"
    for t in x86_64-linux aarch64-linux riscv64-linux windows; do
        for mode in iropt notail; do
            exe="$base-$t-$mode"
            targ=(--target "$t")
            [ "$t" = windows ] && { exe="$exe.exe"; targ=(--nat); }
            envs=(FIDES_NO_CACHE=1)
            [ "$mode" = notail ] && envs+=(FID_IROPT_OFF=tailcall)
            if ! env "${envs[@]}" "$FID" emit "$src" "${targ[@]}" -o "$OUT/$exe" >"$OUT/$exe.log" 2>&1; then
                bad "$base $t $mode: build failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2 | tr '\n' ' ')"
                continue
            fi
            got=$(run_exe "$OUT" "$exe" "$t")
            rc=$?
            if [ $rc -eq 99 ]; then
                ok "$base $t $mode: built (not run on this host)"
            elif [ "$got" = "$want" ]; then
                ok "$base $t $mode: three million levels in one frame"
            else
                bad "$base $t $mode: output differs"
                diff <(echo "$want") <(echo "$got") | head -6
            fi
        done
    done
done

echo "tailcall_ext_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
