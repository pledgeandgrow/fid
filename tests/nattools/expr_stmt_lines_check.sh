#!/usr/bin/env bash
# DB1: a top-level expression statement of a function body (`println(x);`)
# gets its own line, not the function's first one (compiler/lower.fd
# Lowerer_lower_func_body sets the statement's line as Lowerer_lower_stmt
# does for nested statements).
#
#   FID=release/fid.exe bash tests/nattools/expr_stmt_lines_check.sh
#
# Run from the repository root. tests/nattools/expr_stmt_lines.fd has
# println calls on lines 7 and 9 at the top level of main and one on line
# 11 inside an `if`. Checks:
#   - `fid ccompile -g`: the C file (kept beside the output, also when no C
#     compiler is found) has a `#line 7`, `#line 9` and `#line 11`
#     directive naming the .fd file;
#   - `fid emit --target x86_64-linux`: with readelf on PATH, the
#     .debug_line rows of expr_stmt_lines.fd include lines 7, 9 and 11;
#   - `fid run` prints 3, 6, 1.
# Exit 0 when every check holds.
set -u
FID="${FID:-release/fid.exe}"
OUT="$(mktemp -d)"
SRC=tests/nattools/expr_stmt_lines.fd
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

got="$("$FID" run "$SRC" 2>/dev/null | tr -d '\r' | tr '\n' ' ')"
if [ "$got" = "3 6 1 " ]; then ok "run prints 3 6 1"; else bad "run printed '$got'"; fi

"$FID" ccompile "$SRC" -g -o "$OUT/lines.exe" > "$OUT/cc.log" 2>&1
cfile="$OUT/lines.c"
if [ -f "$cfile" ]; then
  for l in 7 9 11; do
    if grep -q "^#line $l \".*expr_stmt_lines.fd\"" "$cfile"; then
      ok "C: #line $l"
    else
      bad "C: no #line $l for expr_stmt_lines.fd (the statement took another line)"
    fi
  done
else
  bad "ccompile -g left no C file: $(tail -3 "$OUT/cc.log")"
fi

if "$FID" emit "$SRC" --target x86_64-linux -o "$OUT/lines-x64" > "$OUT/emit.log" 2>&1; then
  if command -v readelf > /dev/null 2>&1; then
    readelf --debug-dump=decodedline "$OUT/lines-x64" > "$OUT/lines.dwarf" 2>&1
    for l in 7 9 11; do
      if awk -v l="$l" '$1 ~ /expr_stmt_lines\.fd/ && $2 == l { found = 1 } END { exit !found }' "$OUT/lines.dwarf"; then
        ok "x86_64-linux: a line row for line $l"
      else
        bad "x86_64-linux: no line row for line $l"
      fi
    done
  else
    echo "SKIP readelf not found"
  fi
else
  bad "emit --target x86_64-linux: $(tail -3 "$OUT/emit.log")"
fi

if [ "$fail" = 0 ]; then echo "expr_stmt_lines_check: ok"; else echo "expr_stmt_lines_check: failures"; fi
exit $fail
