#!/bin/bash
# install-layout-check.sh — release-roadmap A10 and N12: the compiler works
# from an installed layout, the way a user has it, not from the repository.
#
#   scripts/install-layout-check.sh <fid>
#       stage the layout the installer and the archives ship (fid(.exe) with
#       stdlib/ and runtime/ beside it) in a fresh directory, then check it
#   scripts/install-layout-check.sh --installed <dir>
#       check a layout an installer or an archive already wrote:
#       <dir>/fid(.exe), its stdlib/ beside it or in <dir>/../stdlib
#
# From a work directory that has no stdlib/, with FID_HOME unset (the
# compiler finds its library next to itself) and HOME fresh, the compiler
# started by name from PATH must
#   fid version       print a line starting "fid "
#   fid run           run a program that imports a stdlib module
#   fid ccompile      build it through C, and the binary print the same
#                     (when a C compiler is there: CC, or zig/gcc/clang/cl)
#   fid emit          build it natively, and the binary print the same
#                     (Windows: the default PE; x86-64 Linux: --target
#                     x86_64-linux; other hosts: skipped)
# Exit status 0 when every step that ran passed.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
INSTALLED=""
FID=""
if [ "${1:-}" = "--installed" ]; then
  INSTALLED="${2:-}"
  if [ -z "$INSTALLED" ]; then echo "usage: $0 --installed <dir>"; exit 2; fi
  command -v cygpath > /dev/null 2>&1 && INSTALLED="$(cygpath -u "$INSTALLED")"
else
  FID="${1:-}"
  if [ -z "$FID" ]; then
    for f in release/fid-dev release/fid-dev.exe release/fid.exe release/fid; do
      if [ -e "$ROOT/$f" ]; then FID="$ROOT/$f"; break; fi
    done
  fi
  if [ -z "$FID" ]; then echo "install-layout-check: no compiler (give its path)"; exit 2; fi
  [ -e "$FID" ] || [ ! -e "$FID.exe" ] || FID="$FID.exe"
  if [ ! -e "$FID" ]; then echo "install-layout-check: $FID not found"; exit 2; fi
fi
EXE=""
WINDOWS=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WINDOWS=1 ;; esac

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/install-layout.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
if [ -z "$INSTALLED" ]; then
  # the layout of installer/fides.nsi and of the release archives
  INSTALLED="$WORK/install"
  mkdir -p "$INSTALLED"
  cp "$FID" "$INSTALLED/fid$EXE"
  cp -R "$ROOT/stdlib" "$INSTALLED/stdlib"
  [ -d "$ROOT/runtime" ] && cp -R "$ROOT/runtime" "$INSTALLED/runtime"
fi
if [ ! -e "$INSTALLED/fid$EXE" ]; then
  echo "install-layout-check: no fid$EXE in $INSTALLED"; exit 1
fi
if [ ! -f "$INSTALLED/stdlib/core.fd" ] && [ ! -f "$INSTALLED/../stdlib/core.fd" ]; then
  echo "install-layout-check: no stdlib/core.fd beside $INSTALLED/fid$EXE"; exit 1
fi
mkdir -p "$WORK/work" "$WORK/home"
cat > "$WORK/work/layout.fd" << 'EOF'
use string;

fn main() -> i64 {
    var parts: Vec<str> = Vec::new();
    parts.push("installed");
    parts.push("layout");
    parts.push("ok");
    println(str_join(parts, " "));
    var total: i64 = 0;
    var i: i64 = 1;
    while i <= 10 {
        total = total + i;
        i = i + 1;
    }
    println(int_to_str(total));
    return 0;
}
EOF
printf 'installed layout ok\n55\n' > "$WORK/expected.txt"

fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
# in_layout <command...>: in the work directory, fid found by name on PATH
in_layout() {
  (cd "$WORK/work" && PATH="$INSTALLED:$PATH" HOME="$WORK/home" USERPROFILE="$WORK/home" FID_HOME= "$@")
}
same() { # same <label> <output file>
  if [ "$(tr -d '\r' < "$2")" = "$(cat "$WORK/expected.txt")" ]; then
    ok "$1"
  else
    bad "$1 printed:"
    tr -d '\r' < "$2" | head -10 | sed 's/^/        | /'
  fi
}

echo "== install-layout-check: $INSTALLED"
if [ "$(ls "$WORK/work" | grep -c stdlib)" != 0 ]; then bad "the work directory has a stdlib/"; fi

in_layout fid version > "$WORK/version.txt" 2>&1
if [ $? -eq 0 ] && head -1 "$WORK/version.txt" | grep -q "^fid "; then
  ok "fid version: $(head -1 "$WORK/version.txt" | tr -d '\r')"
else
  bad "fid version:"; head -5 "$WORK/version.txt" | sed 's/^/        | /'
fi

in_layout fid run layout.fd > "$WORK/run.txt" 2> "$WORK/run.err"
if [ $? -eq 0 ]; then same "fid run (stdlib from the layout)" "$WORK/run.txt"; else bad "fid run failed:"; head -10 "$WORK/run.err" | sed 's/^/        | /'; fi

has_cc=0
if [ -n "${CC:-}" ]; then has_cc=1; fi
for c in zig gcc clang cc cl tcc; do command -v "$c" > /dev/null 2>&1 && has_cc=1; done
if [ "$has_cc" = 1 ]; then
  rm -f "$WORK/work/layout_c$EXE"
  in_layout fid ccompile layout.fd -o "layout_c$EXE" > "$WORK/cc.log" 2>&1
  if [ -f "$WORK/work/layout_c$EXE" ]; then
    (cd "$WORK/work" && "./layout_c$EXE") > "$WORK/cc.txt" 2>&1
    same "fid ccompile, then the program" "$WORK/cc.txt"
  else
    bad "fid ccompile wrote nothing:"; head -10 "$WORK/cc.log" | sed 's/^/        | /'
  fi
else
  echo "  skip  fid ccompile (no C compiler)"
fi

emit_args=""
if [ "$WINDOWS" = 1 ]; then
  emit_args="-o layout_n.exe"
elif [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then
  emit_args="--target x86_64-linux -o layout_n"
fi
if [ -n "$emit_args" ]; then
  # shellcheck disable=SC2086
  in_layout fid emit layout.fd $emit_args > "$WORK/emit.log" 2>&1
  if [ -f "$WORK/work/layout_n$EXE" ]; then
    chmod +x "$WORK/work/layout_n$EXE" 2> /dev/null
    (cd "$WORK/work" && "./layout_n$EXE") > "$WORK/emit.txt" 2>&1
    same "fid emit, then the program" "$WORK/emit.txt"
  else
    bad "fid emit wrote nothing:"; head -10 "$WORK/emit.log" | sed 's/^/        | /'
  fi
else
  echo "  skip  fid emit (no native target for $(uname -s) $(uname -m) here)"
fi

if [ "$fail" = 0 ]; then echo "install-layout-check: PASS"; else echo "install-layout-check: FAIL"; fi
exit "$fail"
