#!/usr/bin/env bash
# zed-check.sh -- the Tree-sitter grammar of Fides (editors/tree-sitter-fides)
# and the Zed extension (editors/zed) (competitive-roadmap TL10).
#
# Always (node and python3; no tree-sitter CLI, no Zed):
#   - tests/tooling/ts_grammar_check.js: grammar.js loads, its rules are all
#     defined, and every node, field and token the query files and the test
#     corpus name exists in the grammar (an unknown name makes Zed refuse
#     the query), the Zed highlights are the grammar's, every lexer keyword
#     the grammar uses is highlighted;
#   - extension.toml: id, schema_version, a grammar `fides` at
#     editors/tree-sitter-fides of this repository, a language server `fid`
#     for the language "Fides"; languages/fides/config.toml: name "Fides",
#     grammar "fides", path suffix fd, `//` comments; src/lib.rs starts
#     `fid lsp`.
# With the tree-sitter CLI on PATH (REQUIRE=1: its absence is a FAIL):
#   - `tree-sitter generate` succeeds (no unresolved conflict) and
#     `tree-sitter test` passes the corpus (test/corpus/*.txt);
#   - every stdlib/**/*.fd and tests/bootstrap/*.fd file is parsed; the share
#     parsed without an ERROR node must be at least MIN_CLEAN percent
#     (default 90) and is printed;
#   - `tree-sitter query` runs each query file on a stdlib file (the query
#     compiles against the generated parser).
# Usage: bash tests/tooling/zed-check.sh   (from the repository root)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$PWD"
PY=python3; "$PY" -c 'import sys' > /dev/null 2>&1 || PY=python
fails=0

echo "--- grammar and queries (no CLI)"
node tests/tooling/ts_grammar_check.js "$ROOT" || fails=$((fails + 1))

echo "--- Zed extension files"
"$PY" - <<'EOF' || fails=$((fails + 1))
import sys
try:
    import tomllib
except ImportError:
    print("skip zed toml: Python 3.11+ (tomllib) needed")
    sys.exit(0)
bad = 0
def ok(c, what, detail=""):
    global bad
    print(("ok   " if c else "FAIL ") + what + ("" if c or not detail else f" ({detail})"))
    bad += 0 if c else 1
e = tomllib.load(open("editors/zed/extension.toml", "rb"))
ok(e.get("id") == "fides" and e.get("schema_version") == 1 and bool(e.get("version")), "extension.toml: id fides, schema_version 1, a version")
g = e.get("grammars", {}).get("fides", {})
ok(g.get("path") == "editors/tree-sitter-fides" and "github.com" in g.get("repository", "") and bool(g.get("rev")),
   "extension.toml: grammar fides at editors/tree-sitter-fides", repr(g))
ls = e.get("language_servers", {}).get("fid", {})
ok("Fides" in ls.get("languages", []), "extension.toml: language server fid for Fides", repr(ls))
c = tomllib.load(open("editors/zed/languages/fides/config.toml", "rb"))
ok(c.get("name") == "Fides" and c.get("grammar") == "fides", "config.toml: name Fides, grammar fides")
ok(c.get("path_suffixes") == ["fd"], "config.toml: path suffix fd", repr(c.get("path_suffixes")))
ok("// " in c.get("line_comments", []), "config.toml: // comments")
lib = open("editors/zed/src/lib.rs", encoding="utf-8").read()
ok('which("fid")' in lib and '"lsp"' in lib and "register_extension!" in lib, "src/lib.rs: runs `fid lsp` from the PATH")
cargo = tomllib.load(open("editors/zed/Cargo.toml", "rb"))
ok("zed_extension_api" in cargo.get("dependencies", {}) and cargo.get("lib", {}).get("crate-type") == ["cdylib"],
   "Cargo.toml: a cdylib on zed_extension_api")
sys.exit(1 if bad else 0)
EOF

TS=$(command -v tree-sitter || true)
if [ -z "$TS" ]; then
  if [ "${REQUIRE:-0}" = 1 ]; then echo "FAIL tree-sitter CLI not installed"; fails=$((fails + 1))
  else echo "skip generate/test/parse: no tree-sitter CLI (npm install -g tree-sitter-cli)"; fi
else
  echo "--- $("$TS" --version)"
  OUT="${OUT:-$ROOT/build/zed-check}"
  mkdir -p "$OUT"
  ( cd editors/tree-sitter-fides && "$TS" generate ) > "$OUT/generate.txt" 2>&1
  if [ $? = 0 ]; then
    echo "ok   tree-sitter generate"
    grep -i "warning" "$OUT/generate.txt" | sed 's/^/    /' | head -5
  else
    echo "FAIL tree-sitter generate"; sed 's/^/    /' "$OUT/generate.txt" | head -40; fails=$((fails + 1))
  fi
  if [ -s editors/tree-sitter-fides/src/parser.c ]; then
    if ( cd editors/tree-sitter-fides && "$TS" test ) > "$OUT/test.txt" 2>&1; then
      echo "ok   tree-sitter test (corpus)"
    else
      echo "FAIL tree-sitter test"; sed 's/^/    /' "$OUT/test.txt" | tail -40; fails=$((fails + 1))
    fi
    find stdlib tests/bootstrap -maxdepth 2 -name '*.fd' | sort > "$OUT/files.txt"
    total=0; clean=0; : > "$OUT/errors.txt"
    while read -r f; do
      total=$((total + 1))
      if ( cd editors/tree-sitter-fides && "$TS" parse --quiet "$ROOT/$f" ) > /dev/null 2>&1; then
        clean=$((clean + 1))
      else
        echo "$f" >> "$OUT/errors.txt"
      fi
    done < "$OUT/files.txt"
    pct=$(( total > 0 ? clean * 100 / total : 0 ))
    if [ "$pct" -ge "${MIN_CLEAN:-90}" ]; then
      echo "ok   $clean of $total .fd files parse without an error ($pct%)"
    else
      echo "FAIL only $clean of $total .fd files parse without an error ($pct%, need ${MIN_CLEAN:-90}); first ones:"
      head -10 "$OUT/errors.txt" | sed 's/^/    /'; fails=$((fails + 1))
    fi
    for q in editors/tree-sitter-fides/queries/highlights.scm editors/zed/languages/fides/*.scm; do
      if ( cd editors/tree-sitter-fides && "$TS" query "$ROOT/$q" "$ROOT/stdlib/json.fd" ) > /dev/null 2> "$OUT/query.txt"; then
        echo "ok   query $q compiles"
      else
        echo "FAIL query $q"; head -5 "$OUT/query.txt" | sed 's/^/    /'; fails=$((fails + 1))
      fi
    done
  fi
fi

[ $fails = 0 ] && echo "zed-check: ok" || echo "zed-check: FAIL ($fails parts)"
[ $fails = 0 ]
