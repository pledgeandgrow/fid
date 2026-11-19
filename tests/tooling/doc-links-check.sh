#!/bin/bash
# TL11 (docs/roadmap/competitive-roadmap.md): `fid doc stdlib` writes a site
# whose links all resolve, with links between types and to the source.
# Checks over the generated site (python3):
#   1. every relative href names a file of the site, and its #fragment an
#      id on that page (a link checker);
#   2. type cross-links: a signature or a parameter/field type of one page
#      links to the section of a type defined on another page;
#   3. source links: an item links to its source line (a page of the site
#      with an id per line, or a file beside the site);
# 2 and 3 hold since Nov 17 (scribe.fd scribe_link_types, scribe_source_page);
# tests/tooling/doc-site-check.sh checks the rest of the site (TL11).
#   FID=release/fid.exe bash tests/tooling/doc-links-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
"$FID" doc stdlib --output "$TMP/site" > "$TMP/doc.out" 2>&1
if [ ! -s "$TMP/site/index.html" ]; then
    echo "FAIL fid doc stdlib wrote no index.html"; cat "$TMP/doc.out"; exit 1
fi
python3 - "$TMP/site" <<'EOF2'
import html.parser, os, sys, urllib.parse

site = sys.argv[1]

class Page(html.parser.HTMLParser):
    def __init__(self):
        super().__init__()
        self.ids, self.links, self.stack = set(), [], []
    def handle_starttag(self, tag, attrs):
        a = dict(attrs)
        if "id" in a:
            self.ids.add(a["id"])
        if tag == "a" and "name" in a:
            self.ids.add(a["name"])
        if tag == "a" and a.get("href"):
            ctx = next((c for c in reversed(self.stack) if c), "")
            self.links.append((a["href"], ctx))
        cls = a.get("class", "")
        self.stack.append(cls if tag in ("pre", "td", "p") else "")
    def handle_endtag(self, tag):
        if self.stack:
            self.stack.pop()

pages = {}
for root, _, files in os.walk(site):
    for f in files:
        if f.endswith(".html"):
            p = Page()
            full = os.path.join(root, f)
            p.feed(open(full, encoding="utf-8", errors="replace").read())
            pages[os.path.relpath(full, site).replace("\\", "/")] = p

fails = 0
def check(label, cond, extra=""):
    global fails
    print(("ok   " if cond else "FAIL ") + label + ("" if cond else extra))
    if not cond:
        fails += 1

broken, total, type_links, source_links = [], 0, 0, 0
for name, p in pages.items():
    for href, ctx in p.links:
        u = urllib.parse.urlparse(href)
        if u.scheme or href.startswith("//"):
            continue
        total += 1
        target = name if not u.path else os.path.normpath(os.path.join(os.path.dirname(name), u.path)).replace("\\", "/")
        if not os.path.exists(os.path.join(site, target)):
            broken.append(name + " -> " + href)
            continue
        if u.fragment and target in pages and u.fragment not in pages[target].ids:
            broken.append(name + " -> " + href + " (no id)")
            continue
        if u.path and target != name and target in pages and ctx in ("signature", "type"):
            type_links += 1
        if "src" in target.split("/")[:-1] or target.endswith(".fd") or (ctx == "location" and u.path):
            source_links += 1
check("1. every relative link resolves (%d checked)" % total, not broken, ": " + "; ".join(broken[:10]))
check("2. type cross-links between pages", type_links > 0, ": none found")
check("3. links from items to their source", source_links > 0, ": none found")
sys.exit(1 if fails else 0)
EOF2
st=$?
if [ $st = 0 ]; then echo "doc-links-check: PASS"; else echo "doc-links-check: FAIL"; fi
exit $st
