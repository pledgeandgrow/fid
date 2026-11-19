#!/usr/bin/env bash
# stage0-names-check.sh — the compiler's sources still parse with the stage-0
# compiler (the frozen Rust `fidesc`, which builds stage 1 in every CI job
# and in fides-os's CI: release-roadmap A4, F5, G4).
#
# `fid` accepts some words as names that `fidesc` reserves (`guard`,
# `region`, `handle`, `query`, ...). fidesc takes such a word as a
# parameter, a `var` name or inside an expression, but a statement that
# starts with it (`guard = guard + 1;`, `region.x = 1;`) is read as that
# keyword's statement and fails; await/comptime/spawn/try also fail as an
# operand, so those may not name a variable at all. On Nov 15 every CI build
# (ci.yml, macos.yml, bench.yml, bare-metal.yml) stopped at stage 1 with
# "Parse error at 2183:31: unexpected token 'Eq'": a `guard = guard + 1;` in
# compiler/comptime_eval.fd. This check finds those statements without
# building anything, so it can run first in CI.
#
# Usage: scripts/stage0-names-check.sh [file.fd ...]
#   default files: compiler/*.fd, stdlib/*.fd, stdlib/*/*.fd, tools/*.fd
#   FROM=<root.fd>  only the files the stage-0 build of <root.fd> reads:
#               stdlib/core.fd and the `use` closure of <root.fd>
#               (`use a::b` -> a/b.fd, `use x` -> stdlib/x.fd). A job that
#               only builds fid (FROM=compiler/main.fd) is then not stopped
#               by a stdlib module the build never parses.
#   PROBE=1     derive both word lists from ./target/release/fidesc (FIDESC=
#               to name another) and print them, instead of the built-in ones
#   SELFTEST=1  check the scanner on built-in samples
# Exit 1 when a file has such a statement or name.
set -u
cd "$(dirname "$0")/.." || exit 1

# Keywords of fidesc's lexer (src/lexer.rs) that fid takes as names. Words
# that both compilers reserve (`if`, `while`, ...) are left out: fid refuses
# those itself.
# `val` is fidesc's declaration keyword (`val x = 1`): fid takes it as a name,
# and fidesc parses `var val: i64` and `val` in an expression, but a
# statement `val = ...` fails ("expected identifier but got 'Eq'", Nov 17).
RESERVED="select defer region sandbox unsafe txn persist handle do resume guard grad backward query use val await comptime spawn try"
RESERVED_ANY="await comptime spawn try"

probe() {
  local fidesc="${FIDESC:-./target/release/fidesc}" t k st="" any=""
  [ -x "$fidesc" ] || [ -x "$fidesc.exe" ] || { echo "PROBE=1: no $fidesc" >&2; exit 2; }
  t=$(mktemp -d)
  for k in $(sed -nE 's/^[[:space:]]*"([a-z_]+)" => TokenKind::[A-Za-z]+,.*/\1/p' src/lexer.rs); do
    # keywords of fid's own (fidesc's `var fn: i64` parses, fid's does not)
    case " fn var mut const return if else match for while loop break continue true false struct enum impl pub " in
      *" $k "*) continue ;;
    esac
    # a word fidesc refuses even as a `var` name: fid refuses it too
    printf 'fn main() -> i64 {\n    var %s: i64 = 0;\n    return 0;\n}\n' "$k" > "$t/v.fd"
    "$fidesc" dump-ast "$t/v.fd" 2>&1 | grep -q 'Parse error' && continue
    printf 'fn main() -> i64 {\n    var x: i64 = 0;\n    var %s: i64 = 0;\n    %s = 1;\n    return 0;\n}\n' "$k" "$k" > "$t/a.fd"
    "$fidesc" dump-ast "$t/a.fd" 2>&1 | grep -q 'Parse error' && st="$st $k"
    printf 'fn main() -> i64 {\n    var %s: i64 = 0;\n    var x: i64 = %s + 1;\n    return x;\n}\n' "$k" "$k" > "$t/r.fd"
    "$fidesc" dump-ast "$t/r.fd" 2>&1 | grep -q 'Parse error' && any="$any $k"
  done
  rm -rf "$t"
  RESERVED="${st# }"; RESERVED_ANY="${any# }"
}

# scan <files...>: "file:line: word" for each statement that starts with a
# word of $RESERVED (an assignment, a field or element store, a method
# call) and each variable named by a word of $RESERVED_ANY
scan() {
  local alt any
  alt=$(echo "$RESERVED" | tr ' ' '|'); any=$(echo "$RESERVED_ANY" | tr ' ' '|')
  {
    grep -nHE "^[[:space:]]*($alt)[[:space:]]*(=[^=>]|[-+*/%|&^]=|[.]|\[)" "$@" 2>/dev/null \
      | sed -E "s/^([^:]+:[0-9]+):[[:space:]]*($alt).*/\1: \2/"
    if [ -n "$any" ]; then
      grep -nHE "(^|[^A-Za-z0-9_])(var|let|val)[[:space:]]+(mut[[:space:]]+)?($any)[[:space:]]*[:=]" "$@" 2>/dev/null \
        | grep -vE '^[^:]+:[0-9]+:[[:space:]]*//' \
        | sed -E "s/^([^:]+:[0-9]+):.*(var|let|val)[[:space:]]+(mut[[:space:]]+)?($any)[[:space:]]*[:=].*/\1: \4/"
    fi
  } | sort -t: -k1,1 -k2,2n
}

# reached <root.fd>: stdlib/core.fd (built into fidesc) and every file the
# `use` lines of <root.fd> reach, one per line, relative to the current
# directory. `use a::b;` is a/b.fd (or a/b/mod.fd), looked for next to the
# importing file, then at the top, then under stdlib/; `use x;` the same
# with x. Comments are dropped first; a name with no file is skipped.
reached() {
  local seen=" " queue=("$1" stdlib/core.fd) f d m p c hit
  while [ ${#queue[@]} -gt 0 ]; do
    f=${queue[0]}; queue=("${queue[@]:1}")
    case "$seen" in *" $f "*) continue ;; esac
    [ -f "$f" ] || continue
    seen="$seen$f "; echo "$f"
    d=$(dirname "$f")
    for m in $(sed -E 's#//.*##' "$f" \
        | grep -oE '(^|[^A-Za-z0-9_])use[[:space:]]+[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*[[:space:]]*;' \
        | sed -E 's/.*use[[:space:]]+//; s/[[:space:]]*;$//'); do
      p=${m//:://}; hit=""
      for c in "$d/$p.fd" "$p.fd" "$p/mod.fd" "stdlib/$p.fd" "stdlib/$p/mod.fd"; do
        [ -f "$c" ] && { hit=${c#./}; break; }
      done
      [ -n "$hit" ] && queue+=("$hit")
    done
  done
}

if [ "${SELFTEST:-0}" = 1 ]; then
  t=$(mktemp -d); trap 'rm -rf "$t"' EXIT
  cat > "$t/bad.fd" <<'EOI'
fn f(region: i64, x: i64) -> i64 {
    var guard: i64 = 0;
    guard = guard + 1;
    guard += 1;
    val = guard;
    region.count = 2;
    handle[0] = 1;
    var spawn: i64 = 1;
    return region + guard;
}
EOI
  cat > "$t/good.fd" <<'EOI'
// guard = 1;   (a comment)
fn g(guarded: i64, handler: i64, guard: i64) -> i64 {
    var region: str = "x";
    var p2: Pair = Pair { region: Region::Stack, b: 2 };
    if guard == 1 { return handler; }
    var selection: i64 = Kind::Guard as i64;
    guarded = guarded + 1;
    match handler { _ => { return Opt::select(); } }
    return selection;
}
EOI
  ok=1
  n=$(scan "$t/bad.fd" | wc -l)
  [ "$n" = 6 ] || { echo "selftest: expected 6 hits in bad.fd, got $n"; scan "$t/bad.fd"; ok=0; }
  n=$(scan "$t/good.fd" | wc -l)
  [ "$n" = 0 ] || { echo "selftest: false hits in good.fd:"; scan "$t/good.fd"; ok=0; }
  # FROM: the use closure (a commented-out use and an unused module are out)
  mkdir -p "$t/r/compiler" "$t/r/stdlib"
  printf 'use core; use vec; use compiler::a;\nfn main() -> i64 { return 0; }\n' > "$t/r/compiler/main.fd"
  printf '// use compiler::c;\nuse compiler::b;\nfn a() -> i64 { return 1; }\n' > "$t/r/compiler/a.fd"
  printf 'use compiler::a;\nfn b() -> i64 { var guard: i64 = 0; return guard; }\n' > "$t/r/compiler/b.fd"
  printf 'fn c() -> i64 {\n    var guard: i64 = 0;\n    guard = guard + 1;\n    return guard;\n}\n' > "$t/r/compiler/c.fd"
  printf 'fn x() -> i64 { return 0; }\n' > "$t/r/stdlib/core.fd"
  printf 'use core;\n' > "$t/r/stdlib/vec.fd"
  cp "$t/bad.fd" "$t/r/stdlib/yaml.fd"
  got=$(cd "$t/r" && reached compiler/main.fd | sort | tr '\n' ' ')
  want="compiler/a.fd compiler/b.fd compiler/main.fd stdlib/core.fd stdlib/vec.fd "
  [ "$got" = "$want" ] || { echo "selftest: FROM reached '$got', expected '$want'"; ok=0; }
  n=$(cd "$t/r" && scan $(reached compiler/main.fd) | wc -l)
  [ "$n" = 0 ] || { echo "selftest: FROM scanned a file the build does not read"; ok=0; }
  if [ "$ok" = 1 ]; then echo "stage0-names-check selftest: PASS"; exit 0; fi
  echo "stage0-names-check selftest: FAIL"; exit 1
fi

if [ "${PROBE:-0}" = 1 ]; then
  probe
  echo "fidesc refuses at a statement's start: $RESERVED"
  echo "fidesc refuses as any name: $RESERVED_ANY"
fi

if [ $# -gt 0 ]; then files=("$@")
elif [ -n "${FROM:-}" ]; then
  [ -f "$FROM" ] || { echo "stage0-names-check: no $FROM"; exit 1; }
  mapfile -t files < <(reached "$FROM")
  echo "stage0-names-check: $FROM reads ${#files[@]} files"
else
  shopt -s nullglob
  files=(compiler/*.fd stdlib/*.fd stdlib/*/*.fd tools/*.fd)
fi
hits=$(scan "${files[@]}")
if [ -n "$hits" ]; then
  echo "$hits"
  echo "stage0-names-check: FAIL ($(echo "$hits" | wc -l) statements the stage-0 compiler (fidesc) cannot parse; rename the variable)"
  exit 1
fi
echo "stage0-names-check: PASS (${#files[@]} files)"
