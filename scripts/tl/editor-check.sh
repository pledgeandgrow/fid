#!/usr/bin/env bash
# editor-check.sh — the editor configs in editors/ exercised (competitive-
# roadmap TL10). Every editor talks to `fid lsp`; scripts/tl/lsp_probe.py
# checks the server's answers, this checks each editor's side.
#
# Without the editor installed (always run, reads files only):
#   helix    languages.toml parses; a language "fides" for *.fd with the
#            server fid = `fid lsp`, and `//` comments
#   sublime  the LSP client settings parse (JSON with // comments); the
#            client runs `fid lsp`; its "selector" is a scope that a
#            .sublime-syntax in editors/sublime/ gives *.fd files (without
#            one, .fd opens as Plain Text and the client never starts)
#   emacs    the auto-mode-alist pattern of fides.el, read as Emacs reads a
#            string and matched as an Emacs regexp: x.fd, not x.fdx
#   neovim   fides.lua maps the extension fd to the filetype fides and
#            starts { fid, "lsp" }
# With the editor on PATH (REQUIRE=1: a missing editor is a FAIL, for CI):
#   nvim     headless (scripts/tl/nvim_check.lua): filetype, the client
#            attaches, diagnostics arrive, inlay hints on
#   emacs    batch (scripts/tl/emacs_check.el): fides-mode, eglot connects,
#            diagnostics arrive through flymake
#   hx       `hx --health fides` with the config: the server is found
#   and lsp_probe.py against the same fid.
# JetBrains (LSP4IJ, configured by hand in its settings) is reported, not
# checked. Zed (editors/zed, editors/tree-sitter-fides) is checked by
# tests/tooling/zed-check.sh.
#
# Usage: scripts/tl/editor-check.sh
#   FID=<fid>     the server (default release/fid(.exe), then release/fid-dev);
#                 a `fid` that runs it is put first on PATH for the editors
#   OUT=<dir>     scratch files (default build/editor-check)
#   REQUIRE=1     fail when nvim, emacs or hx is missing; STATIC=1 static only
# Exit status: 1 on any FAIL.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$PWD"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=.exe ;; *) X= ;; esac
PY=python3; "$PY" -c 'import sys' > /dev/null 2>&1 || PY=python
OUT="${OUT:-build/editor-check}"
case "$OUT" in /*|[A-Za-z]:*) ;; *) OUT="$ROOT/$OUT" ;; esac
mkdir -p "$OUT"
fails=0

# ---- static checks -------------------------------------------------------
"$PY" - <<'EOF' || fails=$((fails + 1))
import json, re, sys
bad = 0
def ok(c, what, detail=""):
    global bad
    print(("ok   " if c else "FAIL ") + what + ("" if c or not detail else f" ({detail})"))
    bad += 0 if c else 1

# helix
try:
    import tomllib
except ImportError:
    tomllib = None
if tomllib is None:
    print("skip helix: languages.toml (no tomllib: Python 3.11+ needed)")
else:
    h = tomllib.load(open("editors/helix/languages.toml", "rb"))
    srv = h.get("language-server", {}).get("fid", {})
    ok(srv.get("command") == "fid" and srv.get("args") == ["lsp"], "helix: language-server fid is `fid lsp`", repr(srv))
    langs = [l for l in h.get("language", []) if l.get("name") == "fides"]
    ok(len(langs) == 1, "helix: one [[language]] fides")
    if langs:
        l = langs[0]
        ok("fd" in l.get("file-types", []), "helix: file-types has fd", repr(l.get("file-types")))
        ok("fid" in l.get("language-servers", []), "helix: fides uses the server fid", repr(l.get("language-servers")))
        ok(l.get("comment-token") == "//", "helix: comment-token //")
        ok(bool(l.get("scope")), "helix: scope set (Helix requires one)")

# sublime
src = open("editors/sublime/LSP-fides.sublime-settings", encoding="utf-8").read()
nocom = "\n".join(re.sub(r'^\s*//.*$', "", ln) for ln in src.splitlines())
try:
    s = json.loads(nocom)
    c = s.get("clients", {}).get("fid", {})
    ok(c.get("command") == ["fid", "lsp"], "sublime: client fid runs `fid lsp`", repr(c.get("command")))
    sel = c.get("selector", "")
    ok(bool(sel), "sublime: client has a selector")
    import glob
    scopes = {}
    for f in glob.glob("editors/sublime/*.sublime-syntax"):
        t = open(f, encoding="utf-8").read()
        m = re.search(r'^scope:\s*(\S+)', t, re.M)
        e = re.search(r'^file_extensions:\s*\n((?:\s+-\s*\S+\n?)+)', t, re.M)
        exts = re.findall(r'-\s*(\S+)', e.group(1)) if e else []
        if m:
            scopes[m.group(1)] = (f, exts)
    have = scopes.get(sel)
    ok(have is not None and "fd" in have[1],
       f"sublime: a .sublime-syntax in editors/sublime/ gives *.fd the scope {sel}",
       "none: .fd files open as Plain Text, so the selector never matches and the server never starts"
       if have is None else f"{have[0]} has file_extensions {have[1]}")
except json.JSONDecodeError as e:
    ok(False, "sublime: settings parse as JSON (after // comments)", str(e))

# emacs: read the string literal as the Emacs reader does, then match it
# as an Emacs regexp (\\' is end of string, \\. a literal dot)
el = open("editors/emacs/fides.el", encoding="utf-8").read()
m = re.search(r"auto-mode-alist\s+'\(\"((?:[^\"\\]|\\.)*)\"\s*\.\s*fides-mode\)", el)
ok(m is not None, "emacs: fides.el adds an auto-mode-alist entry for fides-mode")
if m:
    lit, s, i = m.group(1), "", 0
    while i < len(lit):
        ch = lit[i]
        if ch == "\\" and i + 1 < len(lit):
            n = lit[i + 1]
            s += {"n": "\n", "t": "\t", "\\": "\\", '"': '"'}.get(n, n)
            i += 2
        else:
            s += ch
            i += 1
    py, i = "", 0
    while i < len(s):
        if s[i] == "\\" and i + 1 < len(s):
            n = s[i + 1]
            py += {"'": r"\Z", "`": r"\A", ".": r"\.", "\\": r"\\"}.get(n, re.escape(n))
            i += 2
        else:
            py += {".": ".", "*": "*", "+": "+", "?": "?", "^": "^", "$": "$", "[": "[", "]": "]"}.get(s[i], re.escape(s[i]))
            i += 1
    good = re.search(py, "dir/x.fd") is not None
    wrong = re.search(py, "dir/x.fdx") is not None or re.search(py, "dir/xfd") is not None
    ok(good and not wrong, "emacs: auto-mode-alist pattern matches x.fd only",
       f"the string reads as the regexp {s!r}: x.fd {'matches' if good else 'does not match'}"
       + (", and x.fdx or xfd matches" if wrong else "")
       + ("; write \"\\\\.fd\\\\'\"" if not good or wrong else ""))
ok('("fid" "lsp")' in el.replace("'", ""), "emacs: eglot-server-programs entry runs `fid lsp`")

# neovim
lua = open("editors/neovim/fides.lua", encoding="utf-8").read()
ok(re.search(r'extension\s*=\s*\{\s*fd\s*=\s*"fides"', lua) is not None, "neovim: extension fd -> filetype fides")
ok(re.search(r'cmd\s*=\s*\{[^}]*"lsp"\s*\}', lua) is not None, "neovim: client cmd ends with \"lsp\"")
sys.exit(1 if bad else 0)
EOF
echo "skip jetbrains: LSP4IJ is set up by hand in the IDE (editors/README.md)"
echo "--- zed: tests/tooling/zed-check.sh"; bash tests/tooling/zed-check.sh || fails=$((fails + 1))

if [ "${STATIC:-0}" = 1 ]; then
  [ $fails = 0 ] && echo "editor-check: ok (static)" || echo "editor-check: FAIL"
  exit $([ $fails = 0 ] && echo 0 || echo 1)
fi

# ---- the editors themselves ---------------------------------------------
FID="${FID:-}"
if [ -z "$FID" ]; then for f in release/fid$X release/fid-dev$X; do [ -x "$f" ] && { FID="$f"; break; }; done; fi
if [ -z "$FID" ]; then
  echo "FAIL no fid (set FID): the editors cannot be exercised"; exit 1
fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
mkdir -p "$OUT/bin"
# the configs run plain `fid`: put one that runs $FID first on PATH
if [ -n "$X" ]; then cp -f "$FID" "$OUT/bin/fid.exe"; else ln -sf "$FID" "$OUT/bin/fid"; fi
export PATH="$OUT/bin:$PATH"
# a file with an error (assigning a `val`, line 7) inside the repository, so
# every editor finds a project root (.git)
cat > "$OUT/probe.fd" <<'EOF'
fn add(a: i64, b: i64) -> i64 {
    return a + b;
}

fn main() -> i64 {
    val x: i64 = add(1, 2);
    x = add(x, 3);
    print(x);
    return 0;
}
EOF

missing() {
  if [ "${REQUIRE:-0}" = 1 ]; then echo "FAIL $1: not installed"; fails=$((fails + 1)); else echo "skip $1: not installed"; fi
}

if command -v nvim > /dev/null 2>&1; then
  echo "--- $(nvim --version | head -1)"
  timeout 120 nvim --headless -u NONE -i NONE -l scripts/tl/nvim_check.lua \
    "$ROOT/editors/neovim/fides.lua" "$OUT/probe.fd" < /dev/null || fails=$((fails + 1))
else missing nvim; fi

if command -v emacs > /dev/null 2>&1; then
  echo "--- $(emacs --version | head -1)"
  timeout 120 emacs --batch -Q -l scripts/tl/emacs_check.el \
    "$ROOT/editors/emacs/fides.el" "$OUT/probe.fd" < /dev/null 2> "$OUT/emacs.stderr" || {
      fails=$((fails + 1)); tail -5 "$OUT/emacs.stderr" | sed 's/^/    /'; }
else missing emacs; fi

HX=$(command -v hx || command -v helix || true)
if [ -n "$HX" ]; then
  echo "--- $("$HX" --version | head -1)"
  mkdir -p "$OUT/xdg/helix"
  cp editors/helix/languages.toml "$OUT/xdg/helix/languages.toml"
  XDG_CONFIG_HOME="$OUT/xdg" timeout 60 "$HX" --health fides > "$OUT/hx-health.txt" 2>&1
  tr -d '\r' < "$OUT/hx-health.txt" | sed 's/^/    /' | head -12
  if grep -q "fid" "$OUT/hx-health.txt" && ! grep -qE "fid.*(not found|✘)|✘ fid" "$OUT/hx-health.txt"; then
    echo "ok   helix: hx --health fides finds the server fid"
  else
    echo "FAIL helix: hx --health fides does not find the server fid"; fails=$((fails + 1))
  fi
else missing hx; fi

echo "--- server side: scripts/tl/lsp_probe.py"
"$PY" scripts/tl/lsp_probe.py "$FID" || fails=$((fails + 1))

[ $fails = 0 ] && echo "editor-check: ok" || echo "editor-check: FAIL ($fails parts)"
[ $fails = 0 ]
