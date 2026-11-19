#!/usr/bin/env bash
# CE8 (docs/roadmap/complaint-roadmap.md): a panic in a debug build prints
# the failing frame's word-sized parameters and locals before the trace.
#   FID=release/fid.exe bash tests/nattools/frame_values_check.sh
# Run from the repository root (Git Bash on Windows with WSL, or Linux).
# tests/bootstrap/test_crash_frame_values.fd is built
#   - with `fid ccompile -g` (C path, codegen_c.fd cg_dbg_prologue),
#   - with `fid emit` and FID_NAT_FRAMES=1, then with `-g`, for each
#     machine-layer target this machine can run: x86_64-linux (natively or
#     in WSL), aarch64-linux / riscv64-linux (qemu-user, natively or in WSL)
#     and `--nat` Windows (on Windows): nat_ir.fd ng_dbg_frames_data and
#     stdlib/linux_runtime/base.fd pe_dbg_frame_print,
#   - once without either (FID_NAT_FRAMES unset, no -g) on x86_64-linux:
#     no values.
# Each image, run plainly, must print the test's expected lines (exit 0);
# run with FID_FRAME_PANIC=1 it must exit 1 with, on stderr, "values in
# ratio:" before "stack trace:" and each of the lines below. A target
# that cannot run here is skipped. Exit 1 on a failure.
set -u
FID="${FID:-release/fid.exe}"
CC="${CC:-zig cc}"
export CC
PROG=tests/bootstrap/test_crash_frame_values.fd
OUT="${TMPDIR:-/tmp}/fid-frame-values"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=""
if [ "$WIN" = 1 ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
VALUES=(
    "    total = 10"
    "    parts = 0"
    "    label = \"share\""
    "    scale = 2.5"
    "    ok = true"
)

# runner <target>: the command prefix that runs an image of it, or "no"
runner() {
    case "$1" in
        c|nat) if [ "$WIN" = 1 ]; then echo "env"; else echo no; fi ;;
        x86_64-linux)
            if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo "env"
            elif [ -n "$WSL" ]; then echo "$WSL env"
            else echo no; fi ;;
        aarch64-linux|riscv64-linux)
            q=qemu-${1%%-linux}
            if [ "$(uname -s)" = Linux ] && command -v "$q" >/dev/null 2>&1; then echo "env $q"
            elif [ -n "$WSL" ] && wsl.exe -e sh -c "command -v $q" >/dev/null 2>&1; then echo "$WSL env $q"
            else echo no; fi ;;
        *) echo no ;;
    esac
}

# check <name> <image> <runner> <want values: 1|0>
check() {
    local name="$1" img="$2" run="$3" want="$4"
    local dir base
    dir=$(dirname "$img"); base=$(basename "$img")
    # shellcheck disable=SC2086
    (cd "$dir" && $run "./$base" >"$base.out" 2>"$base.err0")
    local rc=$?
    if [ "$rc" != 0 ] || ! diff -q --strip-trailing-cr <(expected "$PROG") "$img.out" >/dev/null; then
        bad "$name: the plain run differs (exit $rc, see $img.out)"
        return
    fi
    # shellcheck disable=SC2086
    (cd "$dir" && $run FID_FRAME_PANIC=1 "./$base" >"$base.out1" 2>"$base.err")
    rc=$?
    if [ "$rc" != 1 ]; then
        bad "$name: the panicking run exited $rc, not 1 (see $img.err)"
        return
    fi
    local err="$img.err"
    if [ "$want" = 0 ]; then
        if grep -q '^values in ' "$err"; then bad "$name: values printed without -g"; else ok "$name: no values without -g"; fi
        return
    fi
    if ! grep -q '^values in ratio:' "$err"; then
        bad "$name: no 'values in ratio:' line (stderr: $(head -3 "$err" | tr '\n' '|'))"
        return
    fi
    local vl tl
    vl=$(grep -n '^values in ratio:' "$err" | head -1 | cut -d: -f1)
    tl=$(grep -n '^stack trace:' "$err" | head -1 | cut -d: -f1)
    if [ -n "$tl" ] && [ "$tl" -lt "$vl" ]; then
        bad "$name: the values come after the trace"
        return
    fi
    local miss=0 v
    for v in "${VALUES[@]}"; do
        if ! tr -d '\r' < "$err" | grep -qxF "$v"; then
            bad "$name: missing '$v' (stderr: $(tr '\n' '|' < "$err"))"
            miss=1
        fi
    done
    [ "$miss" = 0 ] && ok "$name: the failing frame's values"
}

# C path
r=$(runner c)
if [ "$WIN" = 1 ]; then img="$OUT/cfv_c.exe"; else img="$OUT/cfv_c"; r=env; fi
if "$FID" ccompile "$PROG" -g -o "$img" >"$OUT/c.log" 2>&1; then
    check "ccompile -g" "$img" "$r" 1
else
    bad "ccompile -g: build failed (see $OUT/c.log)"
fi

# Machine layer
for tgt in x86_64-linux aarch64-linux riscv64-linux nat; do
    r=$(runner "$tgt")
    if [ "$r" = no ]; then
        echo "skip $tgt: cannot run its images here"
        continue
    fi
    for how in env g; do
        img="$OUT/cfv_${tgt}_$how"
        [ "$tgt" = nat ] && img="$img.exe"
        tflag=(--target "$tgt")
        [ "$tgt" = nat ] && tflag=(--nat)
        if [ "$how" = env ]; then
            FID_NAT_FRAMES=1 "$FID" emit "$PROG" "${tflag[@]}" -o "$img" >"$img.log" 2>&1
        else
            (unset FID_NAT_FRAMES; "$FID" emit "$PROG" "${tflag[@]}" -g -o "$img" >"$img.log" 2>&1)
        fi
        if [ $? != 0 ]; then
            bad "emit $tgt ($how): build failed (see $img.log)"
            continue
        fi
        check "emit $tgt ($how)" "$img" "$r" 1
    done
done

# Without -g: no values
r=$(runner x86_64-linux)
if [ "$r" != no ]; then
    img="$OUT/cfv_plain"
    if (unset FID_NAT_FRAMES; "$FID" emit "$PROG" --target x86_64-linux -o "$img" >"$img.log" 2>&1); then
        check "emit x86_64-linux (no -g)" "$img" "$r" 0
    else
        bad "emit x86_64-linux (no -g): build failed"
    fi
fi

[ "$fails" = 0 ] && echo "frame_values_check: ok" || echo "frame_values_check: $fails failure(s)"
[ "$fails" = 0 ]
