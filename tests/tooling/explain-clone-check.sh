#!/bin/bash
# CP4 (docs/roadmap/complaint-roadmap.md): `fid explain --alloc` says why
# each clone the compiler inserted was required, with its source line.
#   FID=release/fid.exe bash tests/tooling/explain-clone-check.sh
# Exit 0 when every expected line is there, 1 otherwise.
FID=${FID:-release/fid.exe}
SRC=tests/bootstrap/test_explain_clone.fd
OUT=$(mktemp)
fail=0
"$FID" explain "$SRC" --alloc > "$OUT" 2>&1 || { echo "FAIL: explain exited $?"; cat "$OUT"; exit 1; }
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
# keep_both: `var b: Vec<i64> = a;` is line 30
has "Vec share in keep_both" "line 30: clone required here because \`a\` still owns the value"
has "copy kept in b" "the copy is kept in \`b\`"
has "share cost" "a Vec share: the buffer is copied only when one of the two changes it"
# into_field: the parameter kept in a field (line 39)
has "parameter borrowed" "line 39: clone required here because it is a parameter, and parameters are borrowed"
# copy_str: `var t: str = s;` (line 45)
has "string copy" "line 45: clone required here because \`s\` still owns the value"
has "string kept in t" "the copy is kept in \`t\`"
has "string cost" "the string's owner count goes up: no bytes are copied"
rm -f "$OUT"
exit $fail
