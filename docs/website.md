# The website

The project website is a Fid program: [`tools/site.fd`](../tools/site.fd)
(roadmap AD2). It renders the repository's Markdown to HTML when a page is
asked for, with one layout (header, navigation, a sidebar of the section's
pages) and one stylesheet, both served by the program. The site loads nothing
from another host.

| Pages | From |
|---|---|
| Home | `README.md` |
| The book | `docs/book/*.md` (its `README.md` is `/docs/book/`) |
| Cookbook, benchmarks, internal tools, this page | `docs/guides/cookbook.md`, `docs/benchmarks.md`, `docs/reference/internal-tools.md`, `docs/website.md` |
| Guides | `docs/guides/*.md` (`/docs/guides/` lists them) |
| Reference | `docs/reference/*.md` (`/docs/reference/` lists them) |
| All pages | `/pages.html`, generated |

## Running it

From the repository root (`fid` finds `stdlib/` in the directory it runs in):

```bash
CC="zig cc" fid ccompile tools/site.fd -o site.exe
./site.exe                      # http://localhost:8080/
./site.exe --port 9000 --root /path/to/repo
```

`fid run tools/site.fd -- --port 8080` works too, but renders a page in
seconds instead of milliseconds: build it through C. `GET /health` answers
`{"status":"ok"}` (from `stdlib/http.fd`). Ctrl-C (SIGINT) or SIGTERM
stops it: `http.fd` stops accepting and lets open connections finish.

One response without a network, for checking a page:

```bash
./site.exe --get /docs/cookbook.html
MSYS_NO_PATHCONV=1 ./site.exe --get /docs/cookbook.html   # in Git Bash, which
                                                          # rewrites /paths
```

## Static files (GitHub Pages)

```bash
./site.exe --build _site --base /fid/
```

writes every page, `style.css`, the logo, `404.html` and `.nojekyll` to
`_site/` (50 files, 2.2 MB, in 1.5 s on Nov 15). Every link between pages is
relative, so the directory works at any URL path; `--base` is only for
`404.html`, which GitHub Pages shows for a path of any depth and so links
from the site's root (`/fid/` for `pledgeandgrow.github.io/fid/`, `/` by
default). Publish the directory as it is (a `gh-pages` branch, or the
`actions/upload-pages-artifact` step). `.github/workflows/pages.yml` does
this on a push to `main` that changes a page, after merging the weekly
benchmark runs into `docs/benchmarks.md` (`bench/page-merge.sh`). A section's `README.md` is also
written as its `index.html`, so `/docs/book/` works without a server.

## URLs

- `/a/b.html` is the page of `a/b.md`; `/a/b.md` shows the same page.
- `/` and `/index.html` are `README.md`; `/docs/book` redirects to
  `/docs/book/` (301).
- A link to another page becomes that page's `.html`, relative to the page
  it is on; a link to anything else in the repository (source files,
  `docs/roadmap/`, `LICENSE`) goes to the file on GitHub; `#anchors` and
  other sites are kept. Headings get GitHub's anchors (`#crypto--hashing`),
  so the links the Markdown already has work.
- Anything else is a 404 page; a method other than GET or HEAD is 405.

Every response carries `X-Content-Type-Options: nosniff` and a
`Content-Security-Policy` that allows only the site's own stylesheet and
images.

## What a request can reach

Only the Markdown files listed above, the logo (`docs/fid-logo.svg`) and the
generated pages. Before any file is named, a request path is refused (403)
when it does not start with `/`, is over 1024 bytes, contains `%00`, or, after
percent-decoding, contains a control byte, a backslash, `:`, `%` (double
encoding), `//`, or a segment that is `.`, `..` or starts with `.` (`.git`).
The decoded path is then matched against the pages: there is no general
"serve this file" route. `tests/bootstrap/test_site_render.fd` checks these
rules and the routing without a network.

## The Markdown it understands

`stdlib/markdown.fd` renders headings, emphasis, code spans and fences,
links, images, lists, block quotes and rules; `tools/site.fd` adds GitHub
tables (with alignment and `\|`) and passes raw HTML blocks through (the
repository's own Markdown is trusted), except an `<img>` from another site
(the README's badges), which becomes its alt text. Not rendered: nested lists
(items are flattened), footnotes, reference-style links, setext headings,
indented code blocks.

## Limits

- Memory: the server uses `http_serve_scoped` (known-bugs 2g.35 is fixed):
  each request's strings and its response are freed after the response,
  so memory stays flat (tests/tooling/site-memory-check.sh; `http_serve`
  kept about 130 KB per request of the largest page).
- A page's sidebar reads the titles of its section's files on each request
  (about 30 ms for a reference page, through C); nothing is cached, so an
  edited page shows on the next request.
- No TLS: put it behind a proxy, or use the static build.
