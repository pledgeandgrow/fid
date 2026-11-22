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
# keep_both: `var b: Vec<i64> = a;` is line 31
has "Vec share in keep_both" "line 31: clone required here because \`a\` still owns the value (reading a variable borrows it), and the copy is kept in \`b\`"
has "share cost" "a Vec share: the buffer is copied only when one of the two changes it"
# into_field: the parameter kept in a field (line 40); this also exercises a
# string owner increment without copying its bytes.
has "parameter borrowed" "line 40: clone required here because it is a parameter, and parameters are borrowed (the caller keeps its owner)"
has "copy kept in a field" "the copy is kept in a struct field"
has "string cost" "the string's owner count goes up: no bytes are copied"
rm -f "$OUT"
exit $fail
