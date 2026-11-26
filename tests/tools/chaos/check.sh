#!/usr/bin/env bash
# tools/chaos.fd (roadmap AD4) prints and writes what
# scripts/verify/chaos.py does: both run with the same arguments, each
# with its own --out, and their stdout, stderr, exit status and
# <out>/report.txt are compared, with each scenario's seconds masked (the
# column after the name) and the --out directories' names too ("\r\n" from
# Python on Windows counts as "\n"; a NUL, which wsl's UTF-16 messages are
# full of and a Fides str cannot hold, is dropped from both).
#
#   bash tests/tools/chaos/check.sh        # about five minutes
#   ONLY=host bash tests/tools/chaos/check.sh   # one case
#
# Cases: the host scenarios on ccompile, and on emit with --strict (each
# run takes a minute: the silent server is open finding C1, killed at 60
# s; on emit "argument past the last" is C8; --strict makes them fail the
# run); --linux with no host backend (the x86_64-linux build, the full
# disk and out of memory scenarios and chaos_linux.py's clock and
# partition parts, all through wsl: where WSL has no distribution, as on
# the machine this was written on, each fails at once with wsl's message,
# which both print the same); a driver that does not compile (--src
# broken.fd); a --linux-out that is not a Linux path; -h; a bad option.
#
# FID: the compiler that builds the driver and the tool (default
# release/fid.exe; a path Python can start too, so on Windows not a
# /c/... one). Run from the repository root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-chaos-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import sys" > /dev/null 2>&1 || PY=python
exe="$OUT/chaos"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/chaos.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/chaos.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
# compare <name> <args...>: both get the args and --out <OUT>/<name>.py.d
# or <name>.fd.d
compare() {
    [ -n "${ONLY:-}" ] && [ "$ONLY" != "$1" ] && return 0
    local name="$1"; shift
    local d pc fc bad=0
    rm -f "$OUT/$name.diff"
    for d in py fd; do rm -rf "$OUT/$name.$d.d"; done
    "$PY" scripts/verify/chaos.py "$@" --out "$OUT/$name.py.d" > "$OUT/$name.py.out" 2> "$OUT/$name.py.err"; pc=$?
    "$exe" "$@" --out "$OUT/$name.fd.d" > "$OUT/$name.fd.out" 2> "$OUT/$name.fd.err"; fc=$?
    norm() {
        tr -d '\r\0' < "$1" | sed -E -e "s#$name\.(py|fd)\.d#OUTDIR#g" -e 's/ +[0-9]+\.[0-9] s  / N s  /' -e 's/: [0-9]+ms$/: Nms/'
    }
    local f
    for f in out err; do
        diff <(norm "$OUT/$name.py.$f") <(norm "$OUT/$name.fd.$f") >> "$OUT/$name.diff" || bad=1
    done
    [ $pc = $fc ] || bad=1
    if [ -f "$OUT/$name.py.d/report.txt" ] || [ -f "$OUT/$name.fd.d/report.txt" ]; then
        diff <(norm "$OUT/$name.py.d/report.txt") <(norm "$OUT/$name.fd.d/report.txt") >> "$OUT/$name.diff" 2>&1 \
            || { bad=1; echo "  (report.txt differs)" >> "$OUT/$name.diff"; }
    fi
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(cat "$OUT/$name.fd.out" "$OUT/$name.fd.err" | wc -l) lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare host --fid "$FID" --backends ccompile
compare host-emit-strict --fid "$FID" --backends emit --strict
compare linux --fid "$FID" --backends "" --linux
compare broken --fid "$FID" --backends ccompile --src tests/tools/chaos/broken.fd
compare linux-out --fid "$FID" --linux-out relative/dir
compare help -h
compare bad-option --clock=1
echo "chaos port: $pass same, $fail differ"
[ $fail = 0 ]
