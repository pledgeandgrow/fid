#!/bin/bash
# mi20_prim_check.sh [FID]: MI20 (llvm-roadmap), one interface for the
# runtime's machine-code primitives. tests/nattools/mi20_prims.fd calls every
# group of them by their nat_* names (atomics, nat_call_fn and the function
# table, frame records and the address -> name table, the thread word, a
# thread, the start block, the signal handler, the task trampoline, fma,
# arch/os) and must print its Expected block on each native backend, plus
# a last line naming the target.
#
#   1. pe_ir (plain `fid emit`, the default Windows backend): builds and
#      prints the block, then "target arch=3 os=1". Before MI20 pe_ir refused
#      the program ("not available in native Windows executables: nat_arch,
#      ...").
#   2. The same under FID_VERIFY=1: every primitive of the interface is made
#      and PeirGen_nat_check passes (a missing one fails the build with "the
#      runtime primitives of this target lack ..."); tests/bootstrap/hello.fd
#      and test_threads.fd built that way print what `fid run` prints.
#   3. `fid emit --nat` (the machine layer's Windows), plain and FID_VERIFY=1.
#   4. LINUX=1: x86_64-linux in WSL, aarch64-linux and riscv64-linux under
#      qemu-user in WSL (on a Linux host: natively / qemu directly); the last
#      line is arch=3 / 1 / 2, os=0.
#
#   bash tests/nattools/mi20_prim_check.sh release/fid.exe
#   LINUX=1 bash tests/nattools/mi20_prim_check.sh release/fid.exe
set -u
FID="${1:-release/fid.exe}"
export FIDES_NO_CACHE=1
SRC=tests/nattools/mi20_prims.fd
OUT="${OUT:-build/mi20}"
mkdir -p "$OUT"
fail=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=linux ;; esac

want_of() {
    awk '/^\/\/ Expected output/{f=1;next} f&&/^\/\/ /{sub(/^\/\/ /,"");print;next} f{exit}' "$1"
}
want_of "$SRC" > "$OUT/want"

# compare NAME TARGET_LINE: $OUT/NAME.got against the block + the line
compare() {
    local name="$1" tline="$2"
    { cat "$OUT/want"; echo "$tline"; } > "$OUT/$name.want"
    if diff -q --strip-trailing-cr "$OUT/$name.want" "$OUT/$name.got" > /dev/null; then
        echo "  ok    $name"
    else
        echo "  FAIL  $name: output differs"
        diff --strip-trailing-cr "$OUT/$name.want" "$OUT/$name.got" | head -20
        fail=$((fail + 1))
    fi
}

# build NAME [env...] -- args...: fid emit SRC args -o $OUT/NAME
build() {
    local name="$1"
    shift
    if ! env "$@" > "$OUT/$name.log" 2>&1; then
        echo "  FAIL  $name: does not build (see $OUT/$name.log)"
        tail -5 "$OUT/$name.log"
        fail=$((fail + 1))
        return 1
    fi
    return 0
}

if [ "$HOST" = windows ]; then
    # 1-3. pe_ir and --nat, plain and FID_VERIFY=1
    for mode in pe pe_verify nat nat_verify; do
        flags=""
        verify=""
        case "$mode" in nat*) flags="--nat" ;; esac
        case "$mode" in *_verify) verify="FID_VERIFY=1" ;; esac
        if build "$mode" $verify "$FID" emit "$SRC" $flags -o "$OUT/$mode.exe"; then
            "$OUT/$mode.exe" > "$OUT/$mode.got" 2>&1
            compare "$mode" "target arch=3 os=1"
        fi
    done
    # 2. other programs with every primitive made (the tables cover the
    # whole program, the entry point builds the start block)
    for t in tests/bootstrap/hello.fd tests/bootstrap/test_threads.fd; do
        b=$(basename "$t" .fd)
        "$FID" run "$t" > "$OUT/$b.ref" 2> /dev/null
        for mode in pe nat; do
            flags=""
            if [ "$mode" = nat ]; then flags="--nat"; fi
            if build "${b}_${mode}_verify" FID_VERIFY=1 "$FID" emit "$t" $flags -o "$OUT/${b}_${mode}_verify.exe"; then
                "$OUT/${b}_${mode}_verify.exe" > "$OUT/${b}_${mode}_verify.got" 2> /dev/null
                if diff -q --strip-trailing-cr "$OUT/$b.ref" "$OUT/${b}_${mode}_verify.got" > /dev/null; then
                    echo "  ok    $b ($mode, FID_VERIFY=1)"
                else
                    echo "  FAIL  $b ($mode, FID_VERIFY=1): output differs from fid run"
                    fail=$((fail + 1))
                fi
            fi
        done
    done
fi

# 4. The Linux targets
if [ "${LINUX:-}" = 1 ]; then
    for spec in "x86_64-linux::3" "aarch64-linux:qemu-aarch64:1" "riscv64-linux:qemu-riscv64:2"; do
        tgt="${spec%%:*}"
        rest="${spec#*:}"
        runner="${rest%%:*}"
        arch="${rest#*:}"
        name="linux_${tgt%%-*}"
        if build "$name" "$FID" emit "$SRC" --target "$tgt" -o "$OUT/$name"; then
            if [ "$HOST" = windows ]; then
                (cd "$OUT" && wsl -e $runner "./$name") > "$OUT/$name.got" 2> "$OUT/$name.err"
            else
                (cd "$OUT" && $runner "./$name") > "$OUT/$name.got" 2> "$OUT/$name.err"
            fi
            compare "$name" "target arch=$arch os=0"
        fi
    done
fi

if [ $fail -eq 0 ]; then
    echo "mi20_prim_check: all passed"
    exit 0
fi
echo "mi20_prim_check: $fail failed"
exit 1
