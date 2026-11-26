#!/usr/bin/env bash
# tools/soak.fd (roadmap AD4) prints and writes what scripts/verify/soak.py
# does: both run with the same arguments, each with its own --out, and
# their stdout, stderr, exit status and <out>/soak-<backend>.csv are
# compared. A soak's numbers are its own (heartbeats counted, memory read
# at a given moment, seconds), so every number is masked as N before the
# comparison, and the CSV is compared by its header and its number of
# rows (a flat run's slope is +0.00 or -0.00 by the noise of the samples:
# its sign is masked too); the --out directories' names are masked too ("\r\n" from Python
# on Windows counts as "\n").
#
#   bash tests/tools/soak/check.sh            # about two minutes
#   SLOW=1 bash tests/tools/soak/check.sh     # + the stalled service (70 s each)
#   ONLY=grow bash tests/tools/soak/check.sh  # one case
#
# Cases: tests/verify/soak_service.fd for 6 s, a sample a second, on
# ccompile and on emit (PASS, flat memory); early.fd, which stops after
# three heartbeats (exit 3 and its stderr); grow.fd, whose memory grows
# about 2 MB a second (FAIL: memory grows, limit 0.5); broken.fd, which
# does not compile (build failed, the compiler's message); -h and two
# argparse errors; with SLOW=1 stall.fd, one heartbeat and then nothing
# for 70 s (FAIL: no heartbeat).
#
# FID: the compiler both build the programs with and that builds the tool
# (default release/fid.exe; a path Python can start too, so on Windows not
# a /c/... one). Run from the repository root; needs python3 (or python)
# with psutil.
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-soak-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import psutil" > /dev/null 2>&1 || PY=python
exe="$OUT/soak"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/soak.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/soak.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
T=tests/tools/soak
pass=0
fail=0
# compare <name> <args...>: both get the args and --out <OUT>/<name>.py.d
# or <name>.fd.d
compare() {
    [ -n "${ONLY:-}" ] && [ "$ONLY" != "$1" ] && return 0
    local name="$1"; shift
    local d pc fc bad=0
    for d in py fd; do rm -rf "$OUT/$name.$d.d"; done
    "$PY" scripts/verify/soak.py "$@" --out "$OUT/$name.py.d" > "$OUT/$name.py.out" 2> "$OUT/$name.py.err"; pc=$?
    "$exe" "$@" --out "$OUT/$name.fd.d" > "$OUT/$name.fd.out" 2> "$OUT/$name.fd.err"; fc=$?
    norm() { sed -E -e 's/\r$//' -e "s#$name\.(py|fd)\.d#OUTDIR#g" -e 's#\\#/#g' -e 's/[0-9]+(\.[0-9]+)?/N/g' -e 's/slope [-+]N/slope ~N/' "$1"; }
    for s in out err; do
        diff <(norm "$OUT/$name.py.$s") <(norm "$OUT/$name.fd.$s") >> "$OUT/$name.diff" || bad=1
    done
    [ $pc = $fc ] || bad=1
    local c
    for c in "$OUT/$name.py.d"/soak-*.csv; do
        [ -f "$c" ] || continue
        local f="$OUT/$name.fd.d/$(basename "$c")"
        if [ ! -f "$f" ]; then bad=1; echo "  ($(basename "$c") missing)" >> "$OUT/$name.diff"; continue; fi
        [ "$(head -1 "$c" | tr -d '\r')" = "$(head -1 "$f")" ] || { bad=1; echo "  (csv header differs)" >> "$OUT/$name.diff"; }
        local rp rf
        rp=$(wc -l < "$c"); rf=$(wc -l < "$f")
        # the sampling loop's last sleep can end just past the duration:
        # one row more or less is the same run
        [ $((rp - rf)) -le 1 ] && [ $((rf - rp)) -le 1 ] || { bad=1; echo "  (csv rows $rp vs $rf)" >> "$OUT/$name.diff"; }
        # every row: seconds,rss_mb,heartbeats,last
        grep -vqE '^[0-9]+\.[0-9],[0-9]+\.[0-9]{2},[0-9]+,' <(tail -n +2 "$f") && { bad=1; echo "  (a csv row of the port is malformed)" >> "$OUT/$name.diff"; }
    done
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc: $(head -1 "$OUT/$name.fd.out" "$OUT/$name.fd.err" 2>/dev/null | grep -v '^==>' | grep . | head -1 | cut -c1-100))"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
rm -f "$OUT"/*.diff
compare service --fid "$FID" --duration 6 --sample 1
compare service-emit --fid "$FID" --backend emit --duration 6 --sample 1
compare early --fid "$FID" --program "$T/early.fd" --duration 6 --sample 1
compare grow --fid "$FID" --program "$T/grow.fd" --duration 6 --sample 0.5 --max-growth-mb-per-hour 0.5
compare broken --fid "$FID" --program "$T/broken.fd" --duration 2 --sample 1
compare help -h
compare bad-int --duration 1.5
compare bad-float --sample x
if [ "${SLOW:-0}" = 1 ]; then
    compare stall --fid "$FID" --program "$T/stall.fd" --duration 70 --sample 5
fi
echo "soak port: $pass same, $fail differ"
[ $fail = 0 ]
