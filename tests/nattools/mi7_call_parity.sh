#!/usr/bin/env bash
# MI7 / MI17 (docs/roadmap/llvm-roadmap.md): Windows has two call lowerings
# today, pe_ir's own (compiler/pe_ir.fd Inst::Call / Inst::CallPtr, the
# default `fid emit`) and the machine layer's (compiler/nat_ir.fd
# ng_gen_call_std / ng_load_args_i, `fid emit --nat`). pe_ir's is deleted
# when Windows moves to the machine layer; until then, and as the gate for
# that move, both must print what `fid run` prints on the programs that
# exercise call lowering.
#
#   FID=release/fid.exe bash tests/nattools/mi7_call_parity.sh
#   FILES="tests/bootstrap/x.fd ..."  to choose the programs
#   LINUX=1  also build --target x86_64-linux and run it in WSL (or natively
#            on a Linux host)
#
# For the first file (tests/bootstrap/test_call_lowering_parity.fd) the
# interpreter's output is also checked against the "Expected output" block
# of the file. Passing: the last line is "mi7_call_parity: N same, 0 differ".
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/mi7_call_parity}"
FILES="${FILES:-tests/bootstrap/test_call_lowering_parity.fd tests/bootstrap/test_machine_tailcall_stack.fd tests/bootstrap/test_natabi_retregs.fd tests/bootstrap/test_variadic_args.fd tests/bootstrap/test_closure_capture.fd tests/bootstrap/test_args.fd}"
mkdir -p "$OUT"
same=0
differ=0

# program output only: the driver's [build]/[prof] progress lines go
clean() {
    grep -v '^\[build\]\|^\[prof\]\|^Built native executable' | tr -d '\r'
}

expected_of() {
    sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | grep '^// ' | grep -v 'Expected output:' | sed 's#^// ##' | tr -d '\r'
}

compare() {
    local label="$1" want="$2" got="$3"
    if cmp -s "$want" "$got"; then
        same=$((same + 1))
    else
        differ=$((differ + 1))
        echo "DIFFER $label"
        diff "$want" "$got" | head -10
    fi
}

first=1
for f in $FILES; do
    b=$(basename "$f" .fd)
    "$FID" run "$f" 2>/dev/null | clean >"$OUT/$b.run"
    if [ "$first" = 1 ]; then
        expected_of "$f" >"$OUT/$b.want"
        compare "$b: run vs Expected output" "$OUT/$b.want" "$OUT/$b.run"
        first=0
    fi
    # pe_ir (the default Windows emit)
    rm -f "$OUT/$b.pe.exe"
    if "$FID" emit "$f" -o "$OUT/$b.pe.exe" >"$OUT/$b.pe.log" 2>&1 && [ -s "$OUT/$b.pe.exe" ]; then
        "$OUT/$b.pe.exe" 2>/dev/null | clean >"$OUT/$b.pe.out"
        compare "$b: pe_ir vs run" "$OUT/$b.run" "$OUT/$b.pe.out"
    else
        differ=$((differ + 1))
        echo "DIFFER $b: pe_ir emit failed (see $OUT/$b.pe.log)"
    fi
    # the machine layer on Windows
    rm -f "$OUT/$b.nat.exe"
    if "$FID" emit "$f" --nat -o "$OUT/$b.nat.exe" >"$OUT/$b.nat.log" 2>&1 && [ -s "$OUT/$b.nat.exe" ]; then
        "$OUT/$b.nat.exe" 2>/dev/null | clean >"$OUT/$b.nat.out"
        compare "$b: --nat vs run" "$OUT/$b.run" "$OUT/$b.nat.out"
    else
        differ=$((differ + 1))
        echo "DIFFER $b: --nat emit failed (see $OUT/$b.nat.log)"
    fi
    if [ "${LINUX:-0}" = 1 ]; then
        rm -f "$OUT/$b.x64"
        if "$FID" emit "$f" --target x86_64-linux -o "$OUT/$b.x64" >"$OUT/$b.x64.log" 2>&1 && [ -s "$OUT/$b.x64" ]; then
            if [ "$(uname -s)" = Linux ]; then
                chmod +x "$OUT/$b.x64"
                "$OUT/$b.x64" 2>/dev/null | clean >"$OUT/$b.x64.out"
            else
                wsl -- "$(wsl wslpath -a "$OUT/$b.x64" | tr -d '\r')" 2>/dev/null | clean >"$OUT/$b.x64.out"
            fi
            compare "$b: x86_64-linux vs run" "$OUT/$b.run" "$OUT/$b.x64.out"
        else
            differ=$((differ + 1))
            echo "DIFFER $b: x86_64-linux emit failed (see $OUT/$b.x64.log)"
        fi
    fi
done
echo "mi7_call_parity: $same same, $differ differ"
[ "$differ" = 0 ]
