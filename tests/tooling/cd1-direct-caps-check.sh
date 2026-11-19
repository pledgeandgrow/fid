#!/usr/bin/env bash
# cd1-direct-caps-check.sh -- ark.toml's [capabilities] hold for `fid run`,
# `fid check` and `fid build` run directly in a project, not only for `ark`
# (docs/roadmap/complaint-roadmap.md CD1; compiler/compiler.fd section
# "CD1: per-dependency capabilities in a direct build").
#
# A project with two pacts in ark_modules/:
#   tmplpact  calls the stdlib's template::render_file, which reads a file
#             (p_file_read): it uses fs. Its text has no rule of ark's scan
#             (no `use fs`, no `read_file(`), and it names p_socket_connect
#             in a comment only: what it uses comes from the checked
#             program, so fs counts and net does not.
#   netpact   calls p_socket_connect: net (the call and the type checker's
#             Net effect).
# Checks:
#   1. without [capabilities] the program runs ("ok")
#   2. with both pacts granted nothing, `fid run`, `fid check` and
#      `fid build` fail, naming each pact, the capability, the file and the
#      call path, and the grant line that would allow it; tmplpact is not
#      blamed for net
#   3. granted (fs; net), "*", or a grant written over several lines: runs
#   4. a pact not listed is granted nothing
#   5. a pact in vendor/ is checked too
#   6. FID_ARK_CAPS=off skips the check
# Before CD1 every one of the failing cases ran: only `ark` read
# [capabilities].
#
# Usage: tests/tooling/cd1-direct-caps-check.sh   (FID=<fid> to choose)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export FID_HOME="${FID_HOME:-$ROOT}"
unset FID_ARK_CAPS
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
P="$TMP/capp"
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }

mkdir -p "$P/ark_modules/tmplpact/src" "$P/ark_modules/netpact/src"
cat > "$P/ark_modules/tmplpact/src/lib.fd" <<'EOF'
use template;

// p_socket_connect is only named in this comment: no net here
pub fn page(path: str) -> str {
    return render_file(path, "{}");
}
EOF
cat > "$P/ark_modules/netpact/src/lib.fd" <<'EOF'
pub fn ping(host: str) -> i64 {
    return p_socket_connect(host, 80);
}
EOF
cat > "$P/main.fd" <<'EOF'
use tmplpact;
use netpact;

fn main() {
    var never: i64 = 0;
    if never > 0 {
        print(page("x.txt"));
        print(int_to_str(ping("example.invalid")));
    }
    print("ok\n");
}
EOF
manifest() {
  printf '[package]\nname = "capp"\nversion = "0.1.0"\n%s' "$1" > "$P/ark.toml"
}
# run_in <log> <fid args...>: in the project directory
run_in() {
  local log="$1"; shift
  (cd "$P" && "$FID" "$@") > "$TMP/$log" 2>&1
}
expect_runs() {
  local label="$1" log="$2"
  if run_in "$log" run main.fd && grep -q '^ok$' "$TMP/$log"; then ok "$label"
  else bad "$label"; grep -v '^\[' "$TMP/$log" | head -15; fi
}
has() {
  if grep -qF -- "$3" "$TMP/$2"; then ok "$1"
  else bad "$1: no '$3' in $2"; grep -v '^\[' "$TMP/$2" | head -20; fi
}
hasnt_re() {
  if grep -qE -- "$3" "$TMP/$2"; then bad "$1: '$3' in $2"; grep -E -- "$3" "$TMP/$2" | head -5
  else ok "$1"; fi
}

# 1. no [capabilities]: nothing is checked
manifest ''
expect_runs "1. no [capabilities]: fid run" r1.log

# 2. both granted nothing
manifest '
[capabilities]
tmplpact = []
netpact = []
'
if run_in r2.log run main.fd; then bad "2. fid run with ungranted capabilities succeeded"; else ok "2. fid run fails"; fi
has "2. tmplpact named" r2.log "pact 'tmplpact' uses capabilities ark.toml does not grant it"
has "2. fs, through the stdlib, down to the builtin" r2.log "page -> render_file -> p_file_read"
has "2. the grant line for tmplpact" r2.log 'tmplpact = ["fs"]'
has "2. netpact named" r2.log "pact 'netpact' uses capabilities ark.toml does not grant it"
has "2. net, with the call" r2.log "ping -> p_socket_connect"
has "2. the grant line for netpact" r2.log 'netpact = ["net"]'
hasnt_re "2. a name in a comment is no use (tmplpact: no net)" r2.log "^  net  .*tmplpact"
has "2. the count" r2.log "2 pact(s) use capabilities they are not granted"
if run_in r2c.log check main.fd; then bad "2. fid check succeeded"; else ok "2. fid check fails"; fi
has "2. check: the same error" r2c.log "pact 'netpact' uses capabilities"
if run_in r2b.log build main.fd -o "$TMP/capp_out"; then bad "2. fid build succeeded"; else ok "2. fid build fails"; fi
has "2. build: the same error" r2b.log "pact 'tmplpact' uses capabilities"

# 3. granted
manifest '
[capabilities]
tmplpact = ["fs"]   # reads its templates
netpact = ["net"]
'
expect_runs "3. granted: fid run" r3.log
if run_in r3c.log check main.fd; then ok "3. granted: fid check"; else bad "3. granted: fid check"; grep -v '^\[' "$TMP/r3c.log" | head -10; fi
manifest '
[capabilities]
tmplpact = [
    "fs",
]
"netpact" = ["*"]
'
expect_runs "3. a grant over several lines, and \"*\"" r3b.log

# 4. a pact not listed is granted nothing
manifest '
[capabilities]
tmplpact = ["fs"]
'
if run_in r4.log run main.fd; then bad "4. an unlisted pact ran"; else ok "4. an unlisted pact is granted nothing"; fi
has "4. netpact named" r4.log "pact 'netpact' uses capabilities"
hasnt_re "4. tmplpact is granted" r4.log "pact 'tmplpact'"

# 5. vendor/ (ark vendor's copies) is checked too
mkdir -p "$P/vendor"
mv "$P/ark_modules/tmplpact" "$P/vendor/tmplpact"
manifest '
[capabilities]
tmplpact = []
netpact = ["net"]
'
if run_in r5.log run main.fd; then bad "5. a vendored pact was not checked"; else ok "5. a vendored pact is checked"; fi
has "5. tmplpact from vendor/" r5.log "vendor/tmplpact/src/lib.fd"

# 6. FID_ARK_CAPS=off
if (cd "$P" && FID_ARK_CAPS=off "$FID" run main.fd) > "$TMP/r6.log" 2>&1 && grep -q '^ok$' "$TMP/r6.log"; then ok "6. FID_ARK_CAPS=off skips the check"
else bad "6. FID_ARK_CAPS=off"; grep -v '^\[' "$TMP/r6.log" | head -10; fi

if [ "$fail" = 0 ]; then echo "cd1-direct-caps-check: PASS"; else echo "cd1-direct-caps-check: FAIL"; fi
exit "$fail"
