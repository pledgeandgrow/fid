#!/usr/bin/env bash
# RL16 (docs/roadmap/llvm-roadmap.md): `--leaks` on the native backends.
#   bash tests/nattools/pe_leaks_check.sh [fid]   (default release/fid.exe)
# From the project root, after the compiler is built.
# 1. pe_ir (the default Windows backend; Windows host): tests/leaks/*.fd
#    built with `fid emit --leaks` print their .expected output and "fid
#    --leaks: every block was freed" (scripts/leak-check.sh with
#    NATIVE=x86_64-windows), and tests/nattools/pe_leaks_leaky.fd reports
#    its two blocks: "fid --leaks: 2 block(s), 128 bytes, never freed" and a
#    place line naming leak_here (or main, when inlined). Built without
#    --leaks it prints no report.
# 2. The same leaky program through C (`fid run --leaks`) and, when WSL or
#    a Linux host is there, x86_64-linux (`fid emit --target x86_64-linux
#    --leaks`): the same first line.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-pe-leaks"
mkdir -p "$OUT"
P=tests/nattools/pe_leaks_leaky.fd
WANT="fid --leaks: 2 block(s), 128 bytes, never freed; by where they were allocated:"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

check_report() {   # label, stderr file
    local label="$1" err="$2"
    if grep -q -F "$WANT" "$err" && grep -E -q '^ +128 bytes in +2 block\(s\): (leak_here|main)' "$err"; then
        ok "$label: the two leaked blocks, by place"
    else
        bad "$label: report differs:"
        cat "$err"
    fi
}

case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*|Windows_NT) WINHOST=1 ;;
    *) WINHOST=0 ;;
esac

if [ "$WINHOST" = 1 ]; then
    # 1a. pe_ir, the leak gate's programs
    if NATIVE=x86_64-windows bash scripts/leak-check.sh "$FID" > "$OUT/gate.log" 2>&1; then
        ok "pe_ir: tests/leaks clean ($(tail -1 "$OUT/gate.log"))"
    else
        bad "pe_ir: leak gate: $(tail -1 "$OUT/gate.log")"
        grep FAIL "$OUT/gate.log"
    fi
    # 1b. pe_ir, the leaky program
    if "$FID" emit "$P" --leaks -o "$OUT/leaky.exe" > "$OUT/leaky.build" 2>&1; then
        "$OUT/leaky.exe" > "$OUT/leaky.out" 2> "$OUT/leaky.err"
        if [ "$(tr -d '\r' < "$OUT/leaky.out")" = "true" ]; then ok "pe_ir: leaky program output"
        else bad "pe_ir: leaky program output: $(cat "$OUT/leaky.out")"; fi
        check_report "pe_ir" "$OUT/leaky.err"
    else
        bad "pe_ir: emit --leaks failed: $(tail -2 "$OUT/leaky.build")"
    fi
    # 1c. without --leaks: no report, same output
    if "$FID" emit "$P" -o "$OUT/plain.exe" > "$OUT/plain.build" 2>&1; then
        "$OUT/plain.exe" > "$OUT/plain.out" 2> "$OUT/plain.err"
        if grep -q "fid --leaks" "$OUT/plain.err"; then bad "pe_ir: a report without --leaks"
        else ok "pe_ir: no report without --leaks"; fi
    else
        bad "pe_ir: plain emit failed: $(tail -2 "$OUT/plain.build")"
    fi
    # 1d. --leaks with --checked-alloc together
    if "$FID" emit "$P" --leaks --checked-alloc -o "$OUT/both.exe" > "$OUT/both.build" 2>&1; then
        "$OUT/both.exe" > "$OUT/both.out" 2> "$OUT/both.err"
        check_report "pe_ir --checked-alloc" "$OUT/both.err"
    else
        bad "pe_ir: emit --leaks --checked-alloc failed: $(tail -2 "$OUT/both.build")"
    fi
fi

# 2a. C
"$FID" run "$P" --leaks > "$OUT/c.out" 2> "$OUT/c.err"
check_report "C (fid run --leaks)" "$OUT/c.err"

# 2b. the machine layer's x86_64-linux runtime
if [ "$(uname -s)" = Linux ] || { [ "$WINHOST" = 1 ] && command -v wsl >/dev/null 2>&1; }; then
    if "$FID" emit "$P" --target x86_64-linux --leaks -o "$OUT/leaky-linux" > "$OUT/linux.build" 2>&1; then
        if [ "$(uname -s)" = Linux ]; then
            chmod +x "$OUT/leaky-linux"
            "$OUT/leaky-linux" > "$OUT/linux.out" 2> "$OUT/linux.err"
        else
            # wslpath run with -e (no shell: `wsl wslpath C:\...` lost the
            # backslashes) on a path with forward slashes (cygpath -m)
            lin=$(MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -m "$OUT/leaky-linux")" 2> /dev/null | tr -d '\r' | grep -v '^wsl: ' | tail -1)
            MSYS_NO_PATHCONV=1 wsl.exe -e bash -c "cp '$lin' /tmp/fid-leaky-linux && chmod +x /tmp/fid-leaky-linux && /tmp/fid-leaky-linux" > "$OUT/linux.out" 2> "$OUT/linux.err"
        fi
        check_report "x86_64-linux" "$OUT/linux.err"
    else
        bad "x86_64-linux: emit --leaks failed: $(tail -2 "$OUT/linux.build")"
    fi
fi

if [ "$fails" -ne 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all RL16 --leaks checks passed"
