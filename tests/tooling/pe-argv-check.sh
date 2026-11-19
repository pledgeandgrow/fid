#!/bin/bash
# pe_ir's argv (stdlib/pe_runtime/base.fd pert_argc / pert_argv,
# pe_argv_scan): a native Windows executable parses its command line by
# the C runtime's rules, as a program built through C does. `w.exe "/a b"`
# gave argv[1] = '"/a' (the machine stub split at every space and kept the
# quote).
# Windows only (pe_ir executables); elsewhere it prints SKIP.
#   FID=release/fid.exe bash tests/tooling/pe-argv-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) ;;
  *) echo "pe-argv-check: SKIP (not Windows)"; exit 0 ;;
esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/av.fd" <<'EOF'
fn main() -> i64 {
    val n: i64 = p_argc();
    println("argc " + int_to_str(n));
    var i: i64 = 1;
    while i < n {
        match arg_at(i) {
            Option::Some(a) => {
                println("[" + a + "]");
            }
            Option::None => {
                println("none");
            }
        }
        i = i + 1;
    }
    match arg_at(n) {
        Option::Some(a) => {
            println("past the end: " + a);
        }
        Option::None => {
            println("past the end: none");
        }
    }
    return 0;
}
EOF
if ! "$FID" emit "$TMP/av.fd" -o "$TMP/av.exe" > "$TMP/emit.log" 2>&1; then
  cat "$TMP/emit.log"
  echo "pe-argv-check: FAIL (emit)"
  exit 1
fi
# MSYS quotes each argument for CreateProcess (spaces, embedded quotes,
# trailing backslashes); MSYS_NO_PATHCONV keeps "/a b" as it is
MSYS_NO_PATHCONV=1 "$TMP/av.exe" "/a b" plain 'q"x' 'tr\' 'a\\"b' "" end > "$TMP/out.txt" 2>&1
cat > "$TMP/want.txt" <<'EOF'
argc 8
[/a b]
[plain]
[q"x]
[tr\]
[a\\"b]
[]
[end]
past the end: none
EOF
fail=0
if ! diff "$TMP/want.txt" "$TMP/out.txt"; then
  echo "FAIL arguments quoted by MSYS"
  fail=1
fi
# A command line written by hand: "" inside quotes, 2n+1 backslashes then a
# quote, a tab as a separator
if command -v cmd.exe > /dev/null 2>&1; then
  w=$(cygpath -w "$TMP/av.exe" 2>/dev/null || echo "$TMP/av.exe")
  printf '@"%s" "a""b c" x\\\\\\"y\ttab\r\n' "$w" > "$TMP/run.cmd"
  cmd.exe //c "$(cygpath -w "$TMP/run.cmd")" > "$TMP/out2.txt" 2>&1
  printf 'argc 4\n[a"b c]\n[x\\"y]\n[tab]\npast the end: none\n' > "$TMP/want2.txt"
  if ! diff "$TMP/want2.txt" <(tr -d '\r' < "$TMP/out2.txt"); then
    echo "FAIL a command line written by hand"
    fail=1
  fi
fi
if [ $fail = 0 ]; then echo "pe-argv-check: PASS"; else echo "pe-argv-check: FAIL"; exit 1; fi
