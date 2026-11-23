#!/bin/bash
# BG106: `fid veil` reports the methods of a generic struct. An instance
# lowers as `Cell__get__i64` (mono.fd mangle_method) while the source names
# `Cell_get`, so veil.fd vl_is_user dropped it with core and the stdlib: the
# report had `main` alone.
#   FID=release/fid.exe bash tests/tooling/veil-generic-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
cat > "$TMP/cell.fd" <<'EOF'
struct Cell<T> {
    v: T,
}

impl<T> Cell<T> {
    pub fn get(self: Cell<T>) -> T {
        return self.v;
    }
}

fn main() -> i64 {
    val c: Cell<i64> = Cell { v: 7 };
    println(c.get());
    return 0;
}
EOF
"$FID" veil "$TMP/cell.fd" --output "$TMP/cov" > "$TMP/out" 2>&1
code=$?
if [ $code -eq 0 ]; then echo "ok   fid veil ran"; else echo "FAIL fid veil exit $code"; fail=1; fi
if grep -qE "^main +[1-9]" "$TMP/out"; then echo "ok   main in the report"; else echo "FAIL main not in the report"; fail=1; fi
if grep -qE "^Cell__get[_a-z0-9]* +[1-9][0-9]* +[1-9][0-9]* +100\.0% OK" "$TMP/out"; then
    echo "ok   the generic method is in the report, covered"
else
    echo "FAIL the generic method Cell::get is not in the report"; fail=1
fi
if grep -qE "^Functions: +2/2 " "$TMP/out"; then echo "ok   2/2 functions"; else echo "FAIL want Functions: 2/2"; fail=1; fi
if [ $fail -ne 0 ]; then tr -d '\r' < "$TMP/out" | head -30; fi
exit $fail
