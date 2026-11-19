#!/usr/bin/env bash
# MI28 (docs/roadmap/llvm-roadmap.md): tail calls as jumps in functions
# that take their frame's address (compiler/nat_ir.fd ng_tail_taint,
# ng_ft_run, ng_ft_settle; docs/reference/tail-calls.md). Run after the
# compiler is built:
#   bash tests/nattools/machine_tailcall_frame_check.sh [fid]   (default release/fid.exe)
# On every machine-layer target (x86_64, aarch64 and riscv64 Linux; --nat
# Windows on Windows), each build with FID_VERIFY=1:
# 1. tests/bootstrap/test_machine_tailcall_frame.fd prints its expected
#    lines (a tail call passing the frame's address stays a call, so
#    `peek` and `relay` read their struct intact), with the defaults, with
#    FID_IROPT_OFF=tailcall, FID_NAT_OFF=tailframe and FID_NAT_OFF=tailcall.
# 2. tests/nattools/tailcall_frame_deep.fd (three million levels of self
#    and mutual recursion, a struct local handed to a helper by address
#    in each) prints its expected lines with the defaults and with
#    FID_IROPT_OFF=tailcall: the jumps reuse one frame.
# 3. The deep test does not fit the stack with FID_NAT_OFF=tailframe
#    FID_IROPT_OFF=tailcall (the old rule: no jumps in such a function),
#    nor with FID_NAT_OFF=tailcall: the change is what makes it run.
# 4. tests/nattools/tailcall_deep.fd and
#    tests/bootstrap/test_machine_tailcall_stack.fd (the stack-argument
#    tail calls, no frame address) still run with both iropt settings.
# Images run where they can: x86-64 Linux through WSL (or natively on
# Linux), aarch64 / riscv64 under qemu-user in WSL or on PATH, --nat on
# Windows. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-tailcall-frame"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=""
if command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

runner() {
    case "$1" in
        nat) [ "$WIN" = 1 ] && echo "" || echo no ;;
        x86_64-linux)
            if [ -n "$WSL" ]; then echo "$WSL"
            elif [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo ""
            else echo no; fi ;;
        aarch64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-aarch64"
            elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64
            else echo no; fi ;;
        riscv64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-riscv64"
            elif command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64
            else echo no; fi ;;
        *) echo no ;;
    esac
}

build() {   # build <image> <test> <target>: FID_VERIFY=1
    if [ "$3" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$2" --nat -o "$1" >"$1.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$2" --target "$3" -o "$1" >"$1.log" 2>&1
    fi
}

# runs <image> <target>: its output (stderr too), or nothing when it
# cannot run here (return 2)
runs() {
    local r
    r=$(runner "$2")
    [ "$r" = no ] && return 2
    timeout 300 $r "$1" 2>&1 | tr -d '\r'
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
[ "$WIN" = 1 ] && TARGETS="$TARGETS nat"

for tgt in $TARGETS; do
    ext=""
    [ "$tgt" = nat ] && ext=".exe"
    # 1. the bootstrap test under every switch
    T=tests/bootstrap/test_machine_tailcall_frame.fd
    for sw in default iropt tailframe tailcall; do
        img="$OUT/frame-$tgt-$sw$ext"
        case $sw in
            default) build "$img" "$T" "$tgt" ;;
            iropt) FID_IROPT_OFF=tailcall build "$img" "$T" "$tgt" ;;
            tailframe) FID_NAT_OFF=tailframe build "$img" "$T" "$tgt" ;;
            tailcall) FID_NAT_OFF=tailcall build "$img" "$T" "$tgt" ;;
        esac
        if [ $? -ne 0 ]; then
            bad "MI28 $tgt $sw: build failed: $(tail -1 "$img.log")"
            continue
        fi
        got=$(runs "$img" "$tgt")
        if [ $? -eq 2 ]; then continue; fi
        if [ "$got" = "$(expected "$T")" ]; then ok "MI28 $tgt $sw: test_machine_tailcall_frame runs"
        else bad "MI28 $tgt $sw: test_machine_tailcall_frame output differs"; fi
    done
    # 2. three million levels deep
    D=tests/nattools/tailcall_frame_deep.fd
    for io in on off; do
        img="$OUT/deep-$tgt-$io$ext"
        if [ $io = on ]; then build "$img" "$D" "$tgt"
        else FID_IROPT_OFF=tailcall build "$img" "$D" "$tgt"; fi
        if [ $? -ne 0 ]; then
            bad "MI28 $tgt deep (iropt tailcall $io): build failed: $(tail -1 "$img.log")"
            continue
        fi
        got=$(runs "$img" "$tgt")
        if [ $? -eq 2 ]; then continue; fi
        if [ "$got" = "$(expected "$D")" ]; then ok "MI28 $tgt deep (iropt tailcall $io): three million levels in one frame"
        else bad "MI28 $tgt deep (iropt tailcall $io): output differs (overflow?)"; fi
    done
    # 3. without the change it does not fit
    for sw in tailframe tailcall; do
        img="$OUT/deep-$tgt-no$sw$ext"
        FID_NAT_OFF=$sw FID_IROPT_OFF=tailcall build "$img" "$D" "$tgt" || continue
        got=$(runs "$img" "$tgt")
        if [ $? -eq 2 ]; then continue; fi
        if [ "$got" != "$(expected "$D")" ]; then ok "MI28 $tgt: with FID_NAT_OFF=$sw the deep test does not fit the stack"
        else bad "MI28 $tgt: the deep test fits with FID_NAT_OFF=$sw (it proves nothing)"; fi
    done
    # 4. the stack-argument tests still run
    for S in tests/bootstrap/test_machine_tailcall_stack.fd tests/nattools/tailcall_deep.fd; do
        b=$(basename "$S" .fd)
        for io in on off; do
            img="$OUT/$b-$tgt-$io$ext"
            if [ $io = on ]; then build "$img" "$S" "$tgt"
            else FID_IROPT_OFF=tailcall build "$img" "$S" "$tgt"; fi
            if [ $? -ne 0 ]; then
                bad "MI28 $tgt $b (iropt tailcall $io): build failed"
                continue
            fi
            got=$(runs "$img" "$tgt")
            if [ $? -eq 2 ]; then continue; fi
            if [ "$got" = "$(expected "$S")" ]; then ok "MI28 $tgt $b (iropt tailcall $io) runs"
            else bad "MI28 $tgt $b (iropt tailcall $io): output differs"; fi
        done
    done
done

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all MI28 frame-address tail-call checks passed"
