#!/usr/bin/env bash
# Nightly fuzzing (release M9): random programs must agree on the
# interpreter, the C path, the native path (Windows) and the C path under the
# checked allocator; on Linux the same programs also run under ASan/UBSan.
#
#   FID=release/fid.exe ./scripts/fuzz/nightly.sh            # 300 programs
#   N=1000 SEED=42 ./scripts/fuzz/nightly.sh
#   N=3000 BATCH=25 OUT=build/fuzz/long ./scripts/fuzz/nightly.sh   # long run
#
# The seed defaults to the date, so each night tries new programs and a
# failure is reproduced with `--seed <s> -n 1` (add `--rich` for one kept
# under rich/). Failures are kept in build/fuzz/<date>/ (OUT=...): the
# program and each backend's output, summarised by scripts/fuzz/triage.py.
# The runs of record are in docs/verification/fuzz-runs.md.
set -u
cd "$(dirname "$0")/../.."

FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
N="${N:-300}"
SEED="${SEED:-$(date +%Y%m%d)}"
OUT="${OUT:-build/fuzz/$(date +%Y-%m-%d)}"
# BATCH=k compiles k programs as one file (gen_programs.py --batch): the
# same programs at a fraction of the compiler start-ups, for long runs
BATCH="${BATCH:-1}"
# RETRIES=k: a disagreeing program runs up to k more times before it is
# kept (a busy machine's out-of-memory C compiler is not a miscompile)
RETRIES="${RETRIES:-1}"
# RUN_TIMEOUT=s: seconds a program may run; the interpreter is slow on
# programs that make many Vecs (docs/verification/fuzz-runs.md, I1)
RUN_TIMEOUT="${RUN_TIMEOUT:-60}"
export CC="${CC:-zig cc}"

case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) BACKENDS="run,ccompile,emit,checked" ;;
    Linux) BACKENDS="run,ccompile,checked"
        # the native path on Linux: the machine layer (nat_ir) for this host
        case "$(uname -m)" in
            x86_64) BACKENDS="$BACKENDS,emit:x86_64-linux" ;;
            aarch64) BACKENDS="$BACKENDS,emit:aarch64-linux" ;;
        esac
        [ -n "${NO_NATIVE:-}" ] && BACKENDS="run,ccompile,checked" ;;
    *) BACKENDS="run,ccompile,checked" ;;
esac
BACKENDS="${FUZZ_BACKENDS:-$BACKENDS}"

python3 scripts/fuzz/gen_programs.py -n "$N" --seed "$SEED" --fid "$FID" \
    --backends "$BACKENDS" --keep "$OUT" --batch "$BATCH" --retries "$RETRIES" \
    --run-timeout "$RUN_TIMEOUT"
status=$?
# the same number of programs again with structs, enums, strings and floats
# (gen_programs.py --rich; VF1), kept apart so a seed names one program
python3 scripts/fuzz/gen_programs.py -n "$N" --seed "$SEED" --fid "$FID" \
    --backends "$BACKENDS" --keep "$OUT/rich" --rich --batch "$BATCH" --retries "$RETRIES" \
    --run-timeout "$RUN_TIMEOUT" || status=1
# what the kept failures have in common (one line per signature)
for d in "$OUT" "$OUT/rich"; do
    ls "$d"/*.out > /dev/null 2>&1 && python3 scripts/fuzz/triage.py "$d"
done

# Linux and macOS: the failing and a sample of passing programs under the
# sanitizers (scripts/sanitize.sh needs clang with ASan)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) SAN_HOST="" ;; *) SAN_HOST=1 ;; esac
if [ -n "$SAN_HOST" ] && command -v clang >/dev/null 2>&1; then
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
