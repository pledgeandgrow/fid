#!/usr/bin/env bash
# examples-check.sh — N11: five example projects build + run + lint clean.
set -e
FID="${1:-release/fid-dev}"
for f in lexer parser json_parser codegen mini_compiler; do
    echo "[examples] lint $f"
    out="$("$FID" lint "examples/$f.fd" 2>&1)" || true
    echo "$out" | grep -q "No lint warnings" || { echo "FAIL: lint $f: $out"; exit 1; }
    echo "[examples] run $f"
    "$FID" run "examples/$f.fd" > /dev/null || { echo "FAIL: run $f"; exit 1; }
done
echo "[examples] OK"
