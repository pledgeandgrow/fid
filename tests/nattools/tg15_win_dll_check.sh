#!/usr/bin/env bash
# TG15 (docs/roadmap/llvm-roadmap.md): C functions of DLLs called from
# `--nat` Windows executables through the import table the machine layer
# writes (compiler/nat_enc.fd nat_pe_win64_stub, nat_pe_dll_for; the
# nat_ir.fd wiring: ng_ldyn_ok, ng_gen_ldyn_stubs, ng_gen_win_imports).
# Run after the compiler is built, on Windows:
#   bash tests/nattools/tg15_win_dll_check.sh [fid]   (default release/fid.exe)
#
# 1. tests/nattools/tg15_win64_stub_check.py: every stub's bytes against
#    clang's assembler (any host with zig).
# 2. tests/nattools/tg15_win_dll_prog.fd built with `fid emit --nat`
#    (FID_VERIFY=1): its import table (tests/nattools/pe_imports_check.py)
#    has kernel32.dll!MulDiv, lstrlenA, MultiByteToWideChar,
#    ucrtbase.dll!hypot, ldexp, fmaxf and msvcrt.dll!atoi, atof, toupper,
#    and the runtime's own imports (kernel32.dll!ExitProcess); run, it
#    prints the file's expected output.
# 3. tests/bootstrap/test_machine_ldyn_targets.fd (the Linux TG15 test:
#    hypot and ldexp through `link "m"`, atoi from the C library) built the
#    same way prints its expected output on Windows too.
# Exit 1 on a failure.
set -u
FID="${1:-${FID:-release/fid.exe}}"
OUT="${TMPDIR:-/tmp}/fid-tg15-win"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

if python3 tests/nattools/tg15_win64_stub_check.py --fid "$FID"; then
    ok "win64 stubs agree with clang"
else
    bad "win64 stubs"
fi

case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) ;;
    *) echo "not Windows: steps 2 and 3 skipped"; [ "$fails" -eq 0 ]; exit $? ;;
esac

build_run() {
    local src="$1" name="$2"
    shift 2
    local exe="$OUT/$name.exe"
    rm -f "$exe"
    if ! FID_VERIFY=1 "$FID" emit "$src" --nat -o "$exe" > "$OUT/$name.build" 2>&1 || [ ! -f "$exe" ]; then
        bad "$name: build (see $OUT/$name.build)"
        tail -5 "$OUT/$name.build"
        return
    fi
    if python3 tests/nattools/pe_imports_check.py "$exe" "$@" > "$OUT/$name.imports" 2>&1; then
        ok "$name: imports $(tail -1 "$OUT/$name.imports")"
    else
        bad "$name: imports"
        cat "$OUT/$name.imports"
    fi
    local got want
    got="$("$exe" 2>&1 | tr -d '\r')"
    want="$(expected "$src")"
    if [ "$got" = "$want" ]; then
        ok "$name: output"
    else
        bad "$name: output"
        diff <(echo "$want") <(echo "$got") | head -20
    fi
}

build_run tests/nattools/tg15_win_dll_prog.fd win_dll \
    kernel32.dll!MulDiv kernel32.dll!lstrlenA kernel32.dll!MultiByteToWideChar \
    ucrtbase.dll!hypot ucrtbase.dll!ldexp ucrtbase.dll!fmaxf \
    msvcrt.dll!atoi msvcrt.dll!atof msvcrt.dll!toupper kernel32.dll!ExitProcess
build_run tests/bootstrap/test_machine_ldyn_targets.fd ldyn_targets \
    ucrtbase.dll!hypot ucrtbase.dll!ldexp msvcrt.dll!atoi

if [ "$fails" -ne 0 ]; then
    echo "tg15_win_dll_check: $fails failures"
    exit 1
fi
echo "tg15_win_dll_check: ok"
