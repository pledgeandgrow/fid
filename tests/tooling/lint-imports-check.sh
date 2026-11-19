#!/bin/bash
# N5 (docs/roadmap/release-roadmap.md): `fid lint` resolves `use` imports
# (compiler/compiler.fd lint_with_imports), so a module that needs another
# is linted instead of failing on the other module's names ("unchecked" in
# scripts/lint-check.sh). Fixture: tests/projects/n5_lint_import.
#   FID=release/fid.exe bash tests/tooling/lint-imports-check.sh
# Run from the repository root. Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
D=tests/projects/n5_lint_import
fail=0
chk() {
    if [ "$2" = 1 ]; then echo "ok   $1"; else echo "FAIL $1"; fail=1; fi
}
OUT=$("$FID" lint "$D/main.fd" 2>&1)
code=$?
OUT=$(echo "$OUT" | tr -d '\r')
chk "main.fd: lint succeeds" "$([ $code = 0 ] && echo 1)"
chk "main.fd: its unused function is reported" "$(echo "$OUT" | grep -q "unused function 'never_called'" && echo 1)"
chk "main.fd: no error about the imported function" "$(echo "$OUT" | grep -q "undefined function" || echo 1)"
chk "main.fd: the helper module's own warning is not the importer's" "$(echo "$OUT" | grep -q "n5helper_unused_private" || echo 1)"
chk "main.fd: exactly one warning" "$(echo "$OUT" | grep -q "^1 lint warning(s)" && echo 1)"
OUT2=$("$FID" lint "$D/clean.fd" 2>&1)
code2=$?
OUT2=$(echo "$OUT2" | tr -d '\r')
chk "clean.fd: lint succeeds" "$([ $code2 = 0 ] && echo 1)"
chk "clean.fd: no lint warnings" "$(echo "$OUT2" | grep -q "^No lint warnings" && echo 1)"
OUT3=$("$FID" lint "$D/n5helper.fd" 2>&1 | tr -d '\r')
chk "n5helper.fd: its own unused function is reported" "$(echo "$OUT3" | grep -q "unused function 'n5helper_unused_private'" && echo 1)"
if [ $fail = 0 ]; then echo "lint-imports-check: PASS"; else echo "lint-imports-check: FAIL"; fi
exit $fail
