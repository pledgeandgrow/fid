#!/usr/bin/env bash
# doc-cli-check.sh — the documentation and evidence commands report
# failures and write what they say (docs/roadmap/bugs-roadmap.md):
#   BG321  `fid doc` / `fid scribe` of an input that cannot be read: the
#          message on stderr, exit 1 (it was stdout, exit 0)
#   BG361  `fid doc m.fd -o <dir> -f markdown`: scribe's short options are
#          taken (they were ignored and HTML went to ./docs/)
#   BG360  a typed `const NAME: T = v;` is documented as NAME, not NAME:
#   BG322  `fid doctest` of a missing file: "cannot read", exit 1 (it was
#          "0 examples", exit 0)
#   BG327  `fid qualify -o q.json` writes JSON a parser reads (every string
#          was quoted twice: "source": ""q.fd"")
#   BG462  `fid notebook` escapes the source (a "<script>" in a string was
#          live markup) and keeps a code block's lines in one cell
#   BG483  `fid --help`, `fid help`, `fid -h`: the usage, exit 0 (it was
#          "Unknown command: ", exit 1); the usage lists lsp, repl, ark,
#          notebook and test-rules
#
#   FID=release/fid.exe bash tests/tooling/doc-cli-check.sh
#   OUT=<dir>   default build/doc-cli-check
# Needs python3 for the JSON check. Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/doc-cli-check}"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

# run <command...>: sets rc, so (stdout), se (stderr), CRs removed
run() {
    "$@" > "$OUT/.so" 2> "$OUT/.se"; rc=$?
    so=$(tr -d '\r' < "$OUT/.so"); se=$(tr -d '\r' < "$OUT/.se")
}

# --- BG321: an unreadable input is an error ---------------------------------
for cmd in doc scribe; do
    run "$FID" $cmd "$OUT/no_such_file.fd" --output "$OUT/site_$cmd"
    if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "Cannot read" && ! printf '%s' "$so" | grep -q "Cannot read"; then
        ok "BG321 fid $cmd of a missing file: stderr, exit 1"
    else
        bad "BG321 fid $cmd of a missing file: exit $rc, stdout [$so], stderr [$se]"
    fi
done

# --- BG361 / BG360: scribe's options, the constant's name -------------------
cat > "$OUT/m.fd" <<'EOF'
/// The limit.
pub const LIMIT: i64 = 10;

/// Twice `x`.
pub fn twice(x: i64) -> i64 {
    return x * 2;
}
EOF
run "$FID" doc "$OUT/m.fd" -o "$OUT/outx" -f markdown
if [ "$rc" = 0 ] && [ -f "$OUT/outx/m.md" ]; then
    ok "BG361 fid doc -o <dir> -f markdown writes <dir>/m.md"
else
    bad "BG361 fid doc -o/-f: exit $rc, no $OUT/outx/m.md (stdout [$so])"
fi
run "$FID" doc "$OUT/m.fd" "$OUT/m.fd" -o "$OUT/outy"
if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "unknown argument"; then
    ok "BG361 a second input is an error"
else
    bad "BG361 a second input: exit $rc, stderr [$se]"
fi
run "$FID" scribe "$OUT/m.fd" --output "$OUT/outh"
html="$OUT/outh/m.html"
if [ -f "$html" ] && grep -q 'id="LIMIT"' "$html" && ! grep -q 'LIMIT:<' "$html" && ! grep -q 'id="LIMIT:"' "$html"; then
    ok "BG360 the constant is documented as LIMIT"
else
    bad "BG360 the constant's name: $(grep -o 'id="LIMIT[^"]*"' "$html" 2>/dev/null | head -1)"
fi
if [ -f "$OUT/outx/m.md" ] && grep -qE '^#+ LIMIT$' "$OUT/outx/m.md"; then
    ok "BG360 markdown heading LIMIT (no colon)"
else
    bad "BG360 markdown heading: $(grep -E '^#+ LIMIT' "$OUT/outx/m.md" 2>/dev/null | head -1)"
fi

# --- BG322: doctest of a missing file ----------------------------------------
run "$FID" doctest "$OUT/no_such_file.fd"
if [ "$rc" = 1 ] && printf '%s' "$se" | grep -q "cannot read"; then
    ok "BG322 fid doctest of a missing file: cannot read, exit 1"
else
    bad "BG322 fid doctest of a missing file: exit $rc, stdout [$so], stderr [$se]"
fi

# --- BG327: qualify's JSON --------------------------------------------------
cat > "$OUT/q.fd" <<'EOF'
@crashfree
fn safe_add(a: i64, b: i64) -> i64 {
    return a + b;
}

fn main() {
    println(safe_add(1, 2));
}
EOF
run "$FID" qualify "$OUT/q.fd" -o "$OUT/q.json"
if command -v python3 > /dev/null 2>&1; then
    if [ -f "$OUT/q.json" ] && python3 -c "import json,sys; d=json.load(open(sys.argv[1])); assert d['source'].endswith('q.fd'), d['source']" "$OUT/q.json" 2> "$OUT/.json_err"; then
        ok "BG327 qualify -o: valid JSON, \"source\" is the file name"
    else
        bad "BG327 qualify -o: $(tail -1 "$OUT/.json_err" 2>/dev/null) ($(grep '"source"' "$OUT/q.json" 2>/dev/null))"
    fi
else
    if grep -q '"source": ""' "$OUT/q.json" 2>/dev/null; then
        bad "BG327 qualify -o: \"source\" quoted twice"
    else
        ok "BG327 qualify -o: \"source\" quoted once (no python3: not parsed)"
    fi
fi

# --- BG462: notebook escaping -----------------------------------------------
cat > "$OUT/n.fd" <<'EOF'
/// Compares <b>two</b> numbers
fn main() {
    val a = 1;
    if a < 2 {
        println("<script>alert(1)</script>");
    }
}
EOF
run "$FID" notebook "$OUT/n.fd" -o "$OUT/n.html"
nb="$OUT/n.html"
if [ -f "$nb" ] && ! grep -q '<script>' "$nb" && grep -q '&lt;script&gt;' "$nb" && grep -q 'a &lt; 2' "$nb" && ! grep -q '<b>two</b>' "$nb"; then
    ok "BG462 notebook: the source is escaped"
else
    bad "BG462 notebook: unescaped markup in $nb"
fi
if [ -f "$nb" ] && [ "$(grep -c 'class="cell code"' "$nb")" = 1 ] && grep -q '        println' "$nb"; then
    ok "BG462 notebook: one code cell, indentation kept"
else
    bad "BG462 notebook: $(grep -c 'class="cell code"' "$nb" 2>/dev/null) code cells (want 1, indented)"
fi

# --- BG483: help -------------------------------------------------------------
for h in --help help -h; do
    run "$FID" $h
    all="$so$se"
    if [ "$rc" = 0 ] && ! printf '%s' "$all" | grep -q "Unknown command" && printf '%s' "$all" | grep -q "Usage:"; then
        ok "BG483 fid $h: the usage, exit 0"
    else
        bad "BG483 fid $h: exit $rc, [$(printf '%s' "$all" | head -1)]"
    fi
done
run "$FID" --help
for c in "fid lsp" "fid repl" "fid ark" "fid notebook" "fid test-rules"; do
    if printf '%s' "$so$se" | grep -q -- "$c "; then
        ok "BG483 the usage lists $c"
    else
        bad "BG483 the usage does not list $c"
    fi
done
run "$FID" --borrowck=off
if [ "$rc" = 1 ] && ! printf '%s' "$so$se" | grep -q "Unknown command"; then
    ok "BG483 a global flag alone: the usage, no empty \"Unknown command\""
else
    bad "BG483 fid --borrowck=off: exit $rc [$(printf '%s' "$so" | head -1)]"
fi

if [ "$fails" -gt 0 ]; then
    echo "doc-cli-check: $fails failed"
    exit 1
fi
echo "doc-cli-check: all passed"
