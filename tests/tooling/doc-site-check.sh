#!/bin/bash
# TL11 (docs/roadmap/competitive-roadmap.md): what `fid doc <dir>` puts on a
# site besides links (doc-links-check.sh checks those):
#   1. index.html carries a version label (FID_DOC_VERSION when set, else the
#      package's ark.toml, else the fid version);
#   2. a doc comment's `/// Example:` block is on the page as code, with the
#      note that scripts/doctest.py runs it, and its code keeps its
#      indentation; text around it stays text;
#   3. each item links to its line on the module's source page, and that
#      line of the source page holds the item;
#   4. a signature's type links to the section of a type of another module;
#   5. two files with the same module name get two pages (none overwrites
#      the other): as many module pages as modules;
#   6. the same holds on `fid doc stdlib` (version label, examples, as many
#      pages as modules, a source page per module).
#   FID=release/fid.exe bash tests/tooling/doc-site-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
PY=python3; "$PY" -c 'import sys' > /dev/null 2>&1 || PY=python

# a small package: two modules, one type used across them, an example, and
# a second file whose module name is the same as the first's
mkdir -p "$TMP/pkg/src/more"
cat > "$TMP/pkg/ark.toml" <<'EOF'
[package]
name = "geo"
version = "2.3.1"
EOF
cat > "$TMP/pkg/src/shapes.fd" <<'EOF'
// shapes -- points and boxes
module shapes;

/// A point.
pub struct Point {
    pub x: i64,
    pub y: i64,
}

/// The origin.
/// Example:
///   val p: Point = origin();
///   if p.x == 0 {
///       print("zero");
///   }
/// Text after the example.
pub fn origin() -> Point {
    return Point { x: 0, y: 0 };
}
EOF
cat > "$TMP/pkg/src/area.fd" <<'EOF'
// area -- sizes of shapes
module area;
use shapes;

/// The distance between two points along the axes.
pub fn manhattan(a: Point, b: Point) -> i64 {
    return 0;
}
EOF
cat > "$TMP/pkg/src/more/shapes.fd" <<'EOF'
// a second module named shapes
module shapes;

/// Another function.
pub fn other() -> i64 {
    return 1;
}
EOF

FID_DOC_VERSION= "$FID" doc "$TMP/pkg/src" --output "$TMP/site" > "$TMP/doc.out" 2>&1
if [ ! -s "$TMP/site/index.html" ]; then
    echo "FAIL fid doc wrote no index.html"; cat "$TMP/doc.out"; exit 1
fi
FID_DOC_VERSION=v9-test "$FID" doc "$TMP/pkg/src" --output "$TMP/site_env" > /dev/null 2>&1
FID_DOC_VERSION= "$FID" doc stdlib --output "$TMP/std" > "$TMP/std.out" 2>&1

"$PY" - "$TMP" <<'EOF2'
import os, re, sys, html
tmp = sys.argv[1]
site = os.path.join(tmp, "site")
fails = 0
def check(label, cond, extra=""):
    global fails
    print(("ok   " if cond else "FAIL ") + label + ("" if cond else " " + extra))
    if not cond:
        fails += 1
def read(p):
    try:
        return open(p, encoding="utf-8", errors="replace").read()
    except OSError:
        return ""

index = read(os.path.join(site, "index.html"))
check("1. version label from ark.toml", 'class="version">Version geo 2.3.1<' in index,
      re.findall(r'class="version">[^<]*', index))
envidx = read(os.path.join(tmp, "site_env", "index.html"))
check("1. version label from FID_DOC_VERSION", 'class="version">Version v9-test<' in envidx,
      re.findall(r'class="version">[^<]*', envidx))

shapes = read(os.path.join(site, "shapes.html"))
m = re.search(r'<p class="example-note">Example[^<]*<code>scripts/doctest.py</code></p>\n<pre class="example"><code>(.*?)</code></pre>', shapes, re.S)
check("2. the example is a code block with the doctest.py note", m is not None)
if m:
    code = html.unescape(m.group(1))
    check("2. the example's code keeps its indentation",
          code == 'val p: Point = origin();\nif p.x == 0 {\n    print("zero");\n}', repr(code))
check("2. text after the example stays text", "Text after the example." in shapes and "The origin." in shapes)

loc = re.search(r'<section id="origin">.*?<p class="location">Defined at <a class="src" href="(src/[^"#]+)#L(\d+)"', shapes, re.S)
check("3. origin links to its source line", loc is not None)
if loc:
    src = read(os.path.join(site, loc.group(1)))
    line = re.search(r'<span id="L%s">(.*?)</span>' % loc.group(2), src)
    check("3. that line of the source page holds `pub fn origin`",
          line is not None and "pub fn origin()" in html.unescape(re.sub(r"<[^>]+>", "", line.group(1))),
          line.group(1) if line else "no such line")

area = read(os.path.join(site, "area.html"))
check("4. area's signature links Point to shapes.html#Point",
      re.search(r'<pre class="signature">[^\n]*<a class="ty" href="shapes\.html#Point">Point</a>', area) is not None)
check("4. the parameter table links Point too",
      '<td class="type"><a class="ty" href="shapes.html#Point">Point</a></td>' in area)
check("4. shapes' own Point links within the page",
      '<a class="ty" href="#Point">Point</a>' in shapes)

pages = [f for f in os.listdir(site) if f.endswith(".html") and f != "index.html"]
links = re.findall(r'<tr><td><a href="([^"]+)\.html">', index)
check("5. a page per module (3 modules, distinct pages)", len(pages) == 3 and len(set(links)) == 3, repr(sorted(pages)) + " " + repr(links))
other = [p for p in pages if p not in ("shapes.html", "area.html")]
check("5. the second module shapes has its own page with `other`",
      len(other) == 1 and 'id="other"' in read(os.path.join(site, other[0])), repr(other))

std = os.path.join(tmp, "std")
out = read(os.path.join(tmp, "std.out"))
nm = re.search(r"(\d+) module\(s\)", out)
spages = [f for f in os.listdir(std) if f.endswith(".html") and f != "index.html"] if os.path.isdir(std) else []
check("6. fid doc stdlib: as many pages as modules", nm is not None and int(nm.group(1)) == len(spages),
      out.strip()[-200:] + " pages=%d" % len(spages))
srcs = os.listdir(os.path.join(std, "src")) if os.path.isdir(os.path.join(std, "src")) else []
check("6. fid doc stdlib: a source page per module", len(srcs) == len(spages), "src=%d pages=%d" % (len(srcs), len(spages)))
sidx = read(os.path.join(std, "index.html"))
check("6. fid doc stdlib: version label on index.html", re.search(r'class="version">Version fid [^<]+<', sidx) is not None)
nex = sum(read(os.path.join(std, p)).count('class="example-note"') for p in spages)
check("6. fid doc stdlib: examples on the pages (%d)" % nex, nex > 100)
sys.exit(1 if fails else 0)
EOF2
st=$?
if [ $st = 0 ]; then echo "doc-site-check: PASS"; else echo "doc-site-check: FAIL"; fi
exit $st
