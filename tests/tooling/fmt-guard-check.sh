#!/usr/bin/env bash
# fmt-guard-check.sh — `fid fmt` refuses what it must not format and
# reports what it could not do (docs/roadmap/bugs-roadmap.md):
#   BG364  a file that does not lex or parse: an error, exit 1, the file
#          unchanged (`-w` reindented it and said "formatted"; `--check`
#          said "would reformat")
#   BG358  `-w` on a file that cannot be written: an error, exit 1 (it said
#          "formatted", exit 0)
#   BG487  a CRLF file with a string literal spanning a line break: `-w`
#          then `--check` passes ("already formatted"), `-w` again says
#          "already formatted" (it was "would reformat" for ever)
#   BG484  `opt ?? 0` and `val (a, b) = ...` keep their spaces (fmt wrote
#          `opt ??0` and `val(a, b)`)
#
#   FID=release/fid.exe bash tests/tooling/fmt-guard-check.sh
#   OUT=<dir>   default build/fmt-guard-check
# Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/fmt-guard-check}"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
run() {
    "$@" > "$OUT/.so" 2> "$OUT/.se"; rc=$?
    so=$(tr -d '\r' < "$OUT/.so"); se=$(tr -d '\r' < "$OUT/.se")
}

# --- BG364 -------------------------------------------------------------------
printf 'fn main() {\n  val s = "unterminated;\n  if x { foo(\n}\nfn other() { return 1; }\n' > "$OUT/bad.fd"
cp "$OUT/bad.fd" "$OUT/bad_w.fd"
run "$FID" fmt "$OUT/bad_w.fd" -w
if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "does not parse" && cmp -s "$OUT/bad.fd" "$OUT/bad_w.fd"; then
    ok "BG364 -w on a file that does not parse: refused, unchanged"
else
    bad "BG364 -w: exit $rc, stderr [$se], file changed: $(cmp -s "$OUT/bad.fd" "$OUT/bad_w.fd" && echo no || echo yes)"
fi
run "$FID" fmt "$OUT/bad.fd" --check
if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "does not parse" && ! printf '%s' "$se" | grep -q "would reformat"; then
    ok "BG364 --check: a parse error, not \"would reformat\""
else
    bad "BG364 --check: exit $rc, stderr [$se]"
fi
printf 'fn main() {\n    println(1 +);\n}\n' > "$OUT/bad2.fd"
run "$FID" fmt "$OUT/bad2.fd" --check
if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "does not parse"; then
    ok "BG364 --check: a parser (not lexer) error is refused too"
else
    bad "BG364 parser error: exit $rc, stderr [$se]"
fi

# --- BG358 -------------------------------------------------------------------
printf 'fn main() {\n    val  x=1;\n    println(x);\n}\n' > "$OUT/rough.fd"
cp "$OUT/rough.fd" "$OUT/ro.fd"
chmod a-w "$OUT/ro.fd" 2>/dev/null
command -v attrib > /dev/null 2>&1 && attrib +R "$(cygpath -w "$OUT/ro.fd" 2>/dev/null || echo "$OUT/ro.fd")" > /dev/null 2>&1
if [ -w "$OUT/ro.fd" ] && [ "$(id -u 2>/dev/null)" = 0 ]; then
    echo "skip BG358: running as root, a read-only file is writable"
else
    run "$FID" fmt "$OUT/ro.fd" -w
    if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "could not write" && cmp -s "$OUT/rough.fd" "$OUT/ro.fd"; then
        ok "BG358 -w on a read-only file: an error, exit 1"
    else
        bad "BG358 -w on a read-only file: exit $rc, stderr [$se]"
    fi
fi
command -v attrib > /dev/null 2>&1 && attrib -R "$(cygpath -w "$OUT/ro.fd" 2>/dev/null || echo "$OUT/ro.fd")" > /dev/null 2>&1
chmod u+w "$OUT/ro.fd" 2>/dev/null

# --- BG487 -------------------------------------------------------------------
printf 'fn main() {\r\n    val s = "a\r\nb";\r\n    println(s);\r\n}\r\n' > "$OUT/crlf.fd"
run "$FID" fmt "$OUT/crlf.fd" -w
run "$FID" fmt "$OUT/crlf.fd" --check
if [ "$rc" = 0 ] && printf '%s' "$se" | grep -q "already formatted"; then
    ok "BG487 CRLF with a multi-line string: --check passes after -w"
else
    bad "BG487 --check after -w: exit $rc, stderr [$se]"
fi
printf 'fn main() {\r\n    val s = "a\r\nb";\r\n    println(s);\r\n}\r\n' > "$OUT/crlf2.fd"
run "$FID" fmt "$OUT/crlf2.fd" --check
if [ "$rc" = 0 ]; then
    ok "BG487 CRLF with a multi-line string, already formatted: --check passes"
else
    bad "BG487 --check on the CRLF file: exit $rc, stderr [$se]"
fi

# --- BG484 -------------------------------------------------------------------
cat > "$OUT/sp.fd" <<'EOF'
fn main() {
    val opt: Option<i64> = Option::Some(4);
    println(opt ?? 0);
    val (t0, t1) = (1, "x");
    println(t0);
    println(t1);
}
EOF
run "$FID" fmt "$OUT/sp.fd"
if printf '%s' "$so" | grep -qF 'opt ?? 0' && printf '%s' "$so" | grep -qF 'val (t0, t1)'; then
    ok "BG484 \`?? 0\` and \`val (a, b)\` keep their spaces"
else
    bad "BG484 formatted: [$(printf '%s' "$so" | grep -E '\?\?|t0, t1' | tr '\n' '|')]"
fi
run "$FID" fmt "$OUT/sp.fd" --check
if [ "$rc" = 0 ]; then
    ok "BG484 the spaced form passes --check"
else
    bad "BG484 --check of the spaced form: exit $rc, stderr [$se]"
fi

if [ "$fails" -gt 0 ]; then
    echo "fmt-guard-check: $fails failed"
    exit 1
fi
echo "fmt-guard-check: all passed"
