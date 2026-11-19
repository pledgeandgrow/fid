#!/bin/bash
# tests/cancel/check.sh [FID]: the runtimes' token slot and the waits that
# respect it (complaint-roadmap CA7), and the task chain of a panic on the
# native runtimes (CA9).
#
#   CA7  tests/bootstrap/test_task_wait_cancel.fd prints its expected lines
#        under fid run (interpreter), the C path (ccompile), the Windows
#        native path (emit: pe_ir and pe_runtime), and, where a Linux
#        x86-64 binary can run (Linux, or WSL from Windows), the machine
#        layer (emit --target x86_64-linux: linux_runtime); and on Windows
#        also emit --nat (linux_runtime over kernel32).
#        tests/cancel/send_full.fd (a send on a full channel gives up at the
#        token's deadline) on the compiled paths.
#   CA9  tests/bootstrap/test_task_chain_fail.fd built with emit (pe_ir, and
#        the machine layer where it can run) exits non-zero and prints
#        `task chain: b <- a <- main`.
#
# Run from the project root. Needs a C compiler for ccompile (CC).
#
#   bash tests/cancel/check.sh                      # release/fid.exe
#   bash tests/cancel/check.sh release/fid-dev.exe
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;;
esac
fail=0

ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }

# The "// Expected output:" lines of a test file
expected() {
    sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | sed -n 's|^// \(.*\)$|\1|p' | grep -v '^Expected output:$' | sed 's/^  //'
}

# Run a Linux x86-64 binary: here on Linux, through WSL on Windows
run_linux() {
    if [ "$WIN" = 1 ]; then
        command -v wsl >/dev/null || return 125
        local p
        p=$(cd "$(dirname "$1")" && pwd -W)/$(basename "$1")
        wsl -e bash -c "chmod +x \"\$(wslpath '$p')\" && timeout 60 \"\$(wslpath '$p')\""
    else
        chmod +x "$1" && timeout 60 "$1"
    fi
}

compare() {   # name, test file, output file
    nocr "$3"
    if diff <(expected "$2") "$3" > "$OUT/diff" 2>&1; then
        ok "$1"
    else
        bad "$1" "$(head -c 400 "$OUT/diff")"
    fi
}

T=tests/bootstrap/test_task_wait_cancel.fd
S=tests/cancel/send_full.fd

# --- CA7: interpreter ------------------------------------------------------
timeout 120 "$FID" run "$T" > "$OUT/run.out" 2> "$OUT/run.err"
compare "CA7 run (interpreter)" "$T" "$OUT/run.out"

# --- CA7: C path -----------------------------------------------------------
for f in "$T" "$S"; do
    b=$(basename "$f" .fd)
    if timeout 600 "$FID" ccompile "$f" -o "$OUT/c_$b$EXE" > "$OUT/c_$b.build" 2>&1; then
        timeout 60 "$OUT/c_$b$EXE" > "$OUT/c_$b.out" 2> "$OUT/c_$b.err"
        compare "CA7 ccompile $b" "$f" "$OUT/c_$b.out"
    else
        bad "CA7 ccompile $b: build" "$(tail -c 300 "$OUT/c_$b.build")"
    fi
done

# --- CA7: native Windows (pe_ir + pe_runtime; --nat: linux_runtime) -------
if [ "$WIN" = 1 ]; then
    for mode in pe nat; do
        flag=""
        [ "$mode" = nat ] && flag="--nat"
        for f in "$T" "$S"; do
            b=$(basename "$f" .fd)
            if timeout 600 "$FID" emit "$f" $flag -o "$OUT/${mode}_$b.exe" > "$OUT/${mode}_$b.build" 2>&1; then
                timeout 60 "$OUT/${mode}_$b.exe" > "$OUT/${mode}_$b.out" 2> "$OUT/${mode}_$b.err"
                compare "CA7 emit $flag $b" "$f" "$OUT/${mode}_$b.out"
            else
                bad "CA7 emit $flag $b: build" "$(tail -c 300 "$OUT/${mode}_$b.build")"
            fi
        done
    done
fi

# --- CA7: machine layer, Linux x86-64 --------------------------------------
for f in "$T" "$S"; do
    b=$(basename "$f" .fd)
    if timeout 600 "$FID" emit "$f" --target x86_64-linux -o "$OUT/lx_$b" > "$OUT/lx_$b.build" 2>&1; then
        run_linux "$OUT/lx_$b" > "$OUT/lx_$b.out" 2> "$OUT/lx_$b.err"
        if [ $? = 125 ]; then
            echo "  skip  CA7 emit --target x86_64-linux $b (no way to run it here)"
        else
            compare "CA7 emit --target x86_64-linux $b" "$f" "$OUT/lx_$b.out"
        fi
    else
        bad "CA7 emit --target x86_64-linux $b: build" "$(tail -c 300 "$OUT/lx_$b.build")"
    fi
done

# --- CA9: the task chain on the native runtimes ----------------------------
C=tests/bootstrap/test_task_chain_fail.fd
chain_ok() {   # name, out, err, code
    nocr "$2" "$3"
    if [ "$4" -ne 0 ] && grep -q "^task chain: b <- a <- main$" "$3" && ! grep -q "after" "$2"; then
        ok "$1"
    else
        bad "$1" "exit $4; $(head -c 400 "$3")"
    fi
}
if [ "$WIN" = 1 ]; then
    if timeout 600 "$FID" emit "$C" -o "$OUT/chain_pe.exe" > "$OUT/chain_pe.build" 2>&1; then
        timeout 60 "$OUT/chain_pe.exe" > "$OUT/chain_pe.out" 2> "$OUT/chain_pe.err"
        chain_ok "CA9 emit (pe_ir)" "$OUT/chain_pe.out" "$OUT/chain_pe.err" $?
    else
        bad "CA9 emit (pe_ir): build" "$(tail -c 300 "$OUT/chain_pe.build")"
    fi
    if timeout 600 "$FID" emit "$C" --nat -o "$OUT/chain_nat.exe" > "$OUT/chain_nat.build" 2>&1; then
        timeout 60 "$OUT/chain_nat.exe" > "$OUT/chain_nat.out" 2> "$OUT/chain_nat.err"
        chain_ok "CA9 emit --nat" "$OUT/chain_nat.out" "$OUT/chain_nat.err" $?
    else
        bad "CA9 emit --nat: build" "$(tail -c 300 "$OUT/chain_nat.build")"
    fi
fi
if timeout 600 "$FID" emit "$C" --target x86_64-linux -o "$OUT/chain_lx" > "$OUT/chain_lx.build" 2>&1; then
    run_linux "$OUT/chain_lx" > "$OUT/chain_lx.out" 2> "$OUT/chain_lx.err"
    code=$?
    if [ "$code" = 125 ]; then
        echo "  skip  CA9 emit --target x86_64-linux (no way to run it here)"
    else
        chain_ok "CA9 emit --target x86_64-linux" "$OUT/chain_lx.out" "$OUT/chain_lx.err" "$code"
    fi
else
    bad "CA9 emit --target x86_64-linux: build" "$(tail -c 300 "$OUT/chain_lx.build")"
fi

rm -rf "$OUT"
if [ "$fail" -ne 0 ]; then
    echo "tests/cancel/check.sh: $fail check(s) failed"
    exit 1
fi
echo "tests/cancel/check.sh: all checks passed"
