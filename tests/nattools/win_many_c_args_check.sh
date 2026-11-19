#!/usr/bin/env bash
# --nat Windows: C functions with more than six arguments are imported and
# called (compiler/nat_ir.fd ng_ldyn_sig_lim: no System V register limit on
# Windows, whose stub, compiler/nat_enc.fd nat_pe_win64_stub, passes
# arguments 4 and up on the stack). Before, such a call was left
# unresolved and the build failed with
#   error: link: the C function 'BCryptEncrypt' is not in any library the program links
# (test_crypto_bind_rsa, test_stdlib_postgres_tls, test_tls13_local:
# bcrypt's BCryptEncrypt/Decrypt/..., secur32's AcquireCredentialsHandleA,
# InitializeSecurityContextA, AcceptSecurityContext).
# Builds tests/nattools/win_many_c_args.fd (kernel32's eight-argument
# WideCharToMultiByte) with `fid emit --nat`, runs it and compares its
# output with the file's "Expected output" block; then the same through
# `fid ccompile` when a C compiler is there (CC, else zig cc).
#   bash tests/nattools/win_many_c_args_check.sh [fid]   (Windows only)
set -u
FID="${1:-release/fid.exe}"
SRC=tests/nattools/win_many_c_args.fd
OUT=$(mktemp -d)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "skip: --nat images run on Windows only"; exit 0 ;; esac
# The expected lines: the comment block after "// Expected output:"
sed -n '/^\/\/ Expected output:/,/^$/p' "$SRC" | sed '1d;/^$/d;s/^\/\/ \{0,1\}//' > "$OUT/expected.txt"
fails=0
"$FID" emit "$SRC" --nat -o "$OUT/nat.exe" > "$OUT/nat.log" 2>&1
if [ $? -ne 0 ]; then
    echo "FAIL --nat: build"; grep -v '^\[' "$OUT/nat.log" | tail -8; fails=$((fails + 1))
else
    "$OUT/nat.exe" | tr -d '\r' > "$OUT/nat.txt"; rc=${PIPESTATUS[0]}
    if [ "$rc" -ne 0 ] || ! cmp -s "$OUT/nat.txt" "$OUT/expected.txt"; then
        echo "FAIL --nat: exit $rc, output differs:"; diff "$OUT/expected.txt" "$OUT/nat.txt" | head -20; fails=$((fails + 1))
    else
        echo "ok   --nat"
    fi
fi
CCV="${CC:-}"
if [ -z "$CCV" ] && command -v zig > /dev/null 2>&1; then CCV="zig cc"; fi
if [ -n "$CCV" ]; then
    CC="$CCV" "$FID" ccompile "$SRC" -o "$OUT/c.exe" > "$OUT/c.log" 2>&1
    if [ $? -ne 0 ]; then
        echo "FAIL ccompile: build"; grep -v '^\[' "$OUT/c.log" | tail -8; fails=$((fails + 1))
    else
        "$OUT/c.exe" | tr -d '\r' > "$OUT/c.txt"
        if cmp -s "$OUT/c.txt" "$OUT/expected.txt"; then echo "ok   ccompile"; else echo "FAIL ccompile: output differs"; diff "$OUT/expected.txt" "$OUT/c.txt" | head -20; fails=$((fails + 1)); fi
    fi
else
    echo "skip ccompile: no C compiler (CC or zig)"
fi
rm -rf "$OUT"
if [ "$fails" -ne 0 ]; then echo "win_many_c_args: $fails failed"; exit 1; fi
echo "win_many_c_args: all ok"
