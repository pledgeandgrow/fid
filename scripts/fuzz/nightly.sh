#!/usr/bin/env bash
# Nightly fuzzing (release M9): random programs must agree on the
# interpreter, the C path, the native path (Windows) and the C path under the
# checked allocator; on Linux the same programs also run under ASan/UBSan.
#
#   FID=release/fid.exe ./scripts/fuzz/nightly.sh            # 300 programs
#   N=1000 SEED=42 ./scripts/fuzz/nightly.sh
#
# The seed defaults to the date, so each night tries new programs and a
# failure is reproduced with `--seed <s> -n 1`. Failures are kept in
# build/fuzz/<date>/ (the program and each backend's output).
set -u
cd "$(dirname "$0")/../.."

FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
N="${N:-300}"
SEED="${SEED:-$(date +%Y%m%d)}"
OUT="build/fuzz/$(date +%Y-%m-%d)"
export CC="${CC:-zig cc}"

case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) BACKENDS="run,ccompile,emit,checked" ;;
    *) BACKENDS="run,ccompile,checked" ;;
esac

python3 scripts/fuzz/gen_programs.py -n "$N" --seed "$SEED" --fid "$FID" \
    --backends "$BACKENDS" --keep "$OUT"
status=$?

# Linux and macOS: the failing and a sample of passing programs under the
# sanitizers (scripts/sanitize.sh needs clang with ASan)
if [ "$BACKENDS" = "run,ccompile,checked" ] && command -v clang >/dev/null 2>&1; then
    SAN_DIR="$OUT/sanitize"
    mkdir -p "$SAN_DIR"
    python3 - "$SEED" "$SAN_DIR" <<'PY'
import random, sys
sys.path.insert(0, "scripts/fuzz")
from gen_programs import Gen
seed, out = int(sys.argv[1]), sys.argv[2]
for s in range(seed, seed + 20):
    with open(f"{out}/fuzz_{s}.fd", "w", newline="\n") as f:
        f.write(Gen(random.Random(s)).program())
PY
    for f in "$SAN_DIR"/fuzz_*.fd; do
        name=$(basename "$f" .fd)
        if CC=clang FIDES_NO_CACHE=1 FIDES_CFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
            "$FID" ccompile "$f" -o "$SAN_DIR/$name" > /dev/null 2>&1; then
            if ! "$SAN_DIR/$name" > /dev/null 2> "$SAN_DIR/$name.san" \
                || grep -q 'ERROR: AddressSanitizer\|runtime error:' "$SAN_DIR/$name.san"; then
                echo "sanitizer finding: $f ($SAN_DIR/$name.san)"
                status=1
            fi
        fi
    done
fi
exit $status
