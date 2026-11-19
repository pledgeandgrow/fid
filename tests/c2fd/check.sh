#!/usr/bin/env bash
# tests/c2fd/check.sh -- check the C-to-Fid translator (tools/c2fd.fd).
#
# For every tests/c2fd/<name>.c:
#   1. compile it with a C compiler and run it: the expected output;
#   2. translate it with c2fd and compare with the golden <name>.fd;
#   3. run the golden translation with `fid run` (the interpreter) and with
#      `fid ccompile` (the C path), and compare both outputs (stdout and the
#      exit status) with the C program's.
# Programs listed in MANUAL are expected to need hand edits after the
# translation: only their golden file is checked (gaps.fd loops forever
# until its pointer walk is rewritten with an index).
#
#   bash tests/c2fd/check.sh                 # from anywhere in the repository
#   FID=release/fid.exe CC="zig cc" bash tests/c2fd/check.sh
#   UPDATE=1 bash tests/c2fd/check.sh        # rewrite the golden .fd files
#   C2FD=path/to/c2fd.exe bash tests/c2fd/check.sh   # skip building c2fd
#
# fid needs the repository root as its current directory (it finds
# stdlib/ there), so the script works from there.

set -u
cd "$(dirname "$0")/../.." || exit 2

FID=${FID:-}
if [ -z "$FID" ]; then
    if [ -x release/fid.exe ]; then FID=release/fid.exe
    elif [ -x release/fid ]; then FID=release/fid
    else FID=fid
    fi
fi
CCOMP=${CC:-zig cc}
MANUAL=" gaps "

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# arguments a test program is run with
args_of() {
    case "$1" in
        wordcount) echo "tests/c2fd/words.txt" ;;
        *) echo "" ;;
    esac
}

# run a program, keep stdout (CR removed) and its exit status
run_to() {
    local out=$1
    shift
    "$@" > "$out.raw" 2> /dev/null
    local status=$?
    tr -d '\r' < "$out.raw" > "$out"
    echo "exit=$status" >> "$out"
}

if [ -z "${C2FD:-}" ]; then
    echo "building c2fd with $FID ccompile ..."
    if ! CC="$CCOMP" "$FID" ccompile tools/c2fd.fd -o "$TMP/c2fd.exe" > "$TMP/build.log" 2>&1; then
        echo "FAIL: could not build tools/c2fd.fd"
        tail -20 "$TMP/build.log"
        exit 1
    fi
    C2FD=$TMP/c2fd.exe
fi

fails=0
printf '%-14s %-8s %-8s %-8s %s\n' program golden "fid run" ccompile note
for c in tests/c2fd/*.c; do
    name=$(basename "$c" .c)
    fd=tests/c2fd/$name.fd
    args=$(args_of "$name")
    note=""

    if ! $CCOMP -std=c99 -w -o "$TMP/$name.c.exe" "$c" > "$TMP/$name.cc.log" 2>&1; then
        printf '%-14s C compile failed\n' "$name"
        fails=$((fails + 1))
        continue
    fi
    # shellcheck disable=SC2086
    run_to "$TMP/$name.expected" "$TMP/$name.c.exe" $args

    "$C2FD" "$c" > "$TMP/$name.fd" 2> "$TMP/$name.c2fd.log"
    todos=$(grep -c 'TODO(c2fd)' "$TMP/$name.fd")
    if [ "${UPDATE:-0}" = "1" ]; then
        cp "$TMP/$name.fd" "$fd"
    fi
    golden=same
    if [ ! -f "$fd" ]; then
        golden=missing
    elif ! cmp -s "$TMP/$name.fd" "$fd"; then
        golden=DIFF
    fi

    [ "$todos" -gt 0 ] && note="$todos TODO(c2fd)"
    if [ "${MANUAL#* $name }" != "$MANUAL" ]; then
        # its translation needs hand edits first (it may not even stop)
        [ "$golden" = same ] || fails=$((fails + 1))
        printf '%-14s %-8s %-8s %-8s %s\n' "$name" "$golden" - - "$note; needs manual edits (expected)"
        continue
    fi

    # shellcheck disable=SC2086
    run_to "$TMP/$name.interp" "$FID" run "$fd" $args
    interp=MATCH
    cmp -s "$TMP/$name.expected" "$TMP/$name.interp" || interp=diff

    cpath=MATCH
    if CC="$CCOMP" "$FID" ccompile "$fd" -o "$TMP/$name.fd.exe" > "$TMP/$name.fdcc.log" 2>&1; then
        # shellcheck disable=SC2086
        run_to "$TMP/$name.cpath" "$TMP/$name.fd.exe" $args
        cmp -s "$TMP/$name.expected" "$TMP/$name.cpath" || cpath=diff
    else
        cpath=nobuild
    fi

    if [ "$golden" != same ] || [ "$interp" != MATCH ] || [ "$cpath" != MATCH ]; then
        fails=$((fails + 1))
        if [ "$interp" != MATCH ]; then
            diff "$TMP/$name.expected" "$TMP/$name.interp" | head -6 | sed 's/^/    /'
        elif [ "$cpath" != MATCH ]; then
            diff "$TMP/$name.expected" "$TMP/$name.cpath" 2>/dev/null | head -6 | sed 's/^/    /'
        fi
    fi
    printf '%-14s %-8s %-8s %-8s %s\n' "$name" "$golden" "$interp" "$cpath" "$note"
done

if [ "$fails" -gt 0 ]; then
    echo "c2fd check: $fails failure(s)"
    exit 1
fi
echo "c2fd check: all programs as expected"
