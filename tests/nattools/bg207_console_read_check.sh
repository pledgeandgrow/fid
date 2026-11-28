#!/usr/bin/env bash
# BG207 (docs/roadmap/bugs-roadmap.md): on --nat Windows, console input. The
# runtime's read on a console handle (stdlib/linux_runtime/win.fd
# pe_win_read) drops the CR of each CR LF, and read_line reads one byte at a
# time, so the CR of "abc\r\n" came back as a read of 0 bytes: the end of
# the input for read_line, which then gave an empty line and stopped
# reading (lines typed after it were lost). A read left with only CRs now
# reads on.
#
# The program reads lines until the end of the input and shows each in
# brackets with its length. tests/nattools/console_feed.py runs it in a
# console of its own and types: abc, de, an empty line, fgh, then Ctrl+Z.
# Expected: [abc] 3, [de] 2, [] 0, [fgh] 3, then "end", exit 0. Before the
# fix the console showed "[abc] 3", "[] 0", ... and "end" before fgh.
#   bash tests/nattools/bg207_console_read_check.sh [fid]   (Windows only)
set -u
FID="${1:-release/fid.exe}"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "skip: needs a Windows console"; exit 0 ;; esac
OUT="${TMPDIR:-/tmp}/fid-bg207"
mkdir -p "$OUT"
PY=python3
command -v "$PY" >/dev/null 2>&1 || PY=python
SRC="$OUT/console_lines.fd"
cat >"$SRC" <<'EOF'
fn main() -> i64 {
    var going: bool = true;
    var n: i64 = 0;
    while going {
        match read_line() {
            Option::Some(l) => {
                println("[" + l + "] " + int_to_str(str_len(l)));
                n = n + 1;
            }
            Option::None => {
                going = false;
            }
        }
        if n > 20 {
            going = false;
        }
    }
    println("end");
    return 0;
}
EOF
EXE="$OUT/console_lines.exe"
if ! "$FID" emit "$SRC" --nat -o "$EXE" >"$OUT/build.log" 2>&1; then
    echo "FAIL: build"; tail -5 "$OUT/build.log"; exit 1
fi
got=$("$PY" tests/nattools/console_feed.py "$EXE" abc de "" fgh "^Z" | tr -d '\r')
echo "$got"
# the program's own lines, in order (the typed echo is on lines of its own)
mine=$(printf '%s\n' "$got" | sed 's/^ *//' | grep -E '^\[|^end$|^rc |^TIMEOUT' | tr '\n' '|')
want='[abc] 3|[de] 2|[] 0|[fgh] 3|end|rc 0|'
if [ "$mine" = "$want" ]; then
    echo "PASS: bg207 console read_line"
    exit 0
fi
echo "FAIL: bg207 console read_line: got '$mine', want '$want'"
exit 1
