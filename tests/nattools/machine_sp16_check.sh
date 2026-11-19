#!/usr/bin/env bash
# Checks of the machine-layer goal SP16 (docs/roadmap/llvm-roadmap.md: the
# compiler itself built by our own backend), the parts that do not need
# the whole compiler built natively (scripts/native-fixpoint.sh does that):
#   bash tests/nattools/machine_sp16_check.sh [fid]   (default release/fid.exe)
# - selfhost_builtins_check.py: every runtime builtin the compiler and the
#   stdlib modules it reaches call resolves on the machine layer (Linux,
#   --nat Windows and macOS runtimes), statically, and every extern "C"
#   function reachable from main is a pert_c_/nat_c_ or a primitive of
#   nat_ir (tests/nattools/selfhost_extern_probe.fd must be rejected).
# - tests/bootstrap/test_machine_selfhost_rt.fd (p_string_from,
#   p_string_substring, p_read_line, p_read_stdin, p_stdin_eof) built
#   natively prints its expected lines with empty stdin, and reads a line
#   and four bytes of piped input.
# - tests/nattools/main_stack_unit.fd: a recursion 400000 calls deep (well
#   past 8 MB of stack) finishes under the default `ulimit -s` (the runtime
#   raises RLIMIT_STACK; --nat reserves 256 MB).
# - tests/nattools/selfhost_front_unit.fd: the compiler's own lexer and
#   parser, built natively, give `fid run`'s token counts, checksums and
#   declaration counts on hello.fd and compiler/token.fd (BIG=1: also
#   compiler/lexer.fd and compiler/parser.fd, and a timing of each).
# Native images run here: --nat on Windows, x86_64-linux through WSL on
# Windows or natively on an x86-64 Linux. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-sp16"
mkdir -p "$OUT"
cd "$(dirname "$0")/../.." || exit 1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
RUN=""
if [ "$WIN" = 1 ]; then
    EMIT=(emit --nat); EXT=.exe
else
    EMIT=(emit --target x86_64-linux); EXT=""
    [ "$(uname -m)" = x86_64 ] || { echo "skip: native images need x86-64 here"; EMIT=(); }
fi
# On Windows the Linux image runs too, through WSL (the RLIMIT_STACK path)
LINUX_TOO=0
if [ "$WIN" = 1 ] && command -v wsl.exe > /dev/null 2>&1; then LINUX_TOO=1; fi

# 1. static: the builtins
for layer in "" win mac; do
    if python3 tests/nattools/selfhost_builtins_check.py ${layer:+--layer $layer} > "$OUT/builtins$layer.txt" 2>&1; then
        ok "builtins resolve (${layer:-linux})"
    else
        bad "builtins (${layer:-linux}): $(grep -c '^  ' "$OUT/builtins$layer.txt") missing"
        sed -n '/^MISSING/,$p' "$OUT/builtins$layer.txt" | head -20
    fi
done
# the extern "C" half must see an unresolvable extern (pledge_net_socket)
if python3 tests/nattools/selfhost_builtins_check.py --main tests/nattools/selfhost_extern_probe.fd > "$OUT/probe.txt" 2>&1; then
    bad "builtins check passed the probe that reaches pledge_net_socket"
elif grep -q "pledge_net_socket" "$OUT/probe.txt"; then
    ok "builtins check rejects an extern \"C\" the machine layer cannot run"
else
    bad "builtins check probe: $(tail -2 "$OUT/probe.txt")"
fi
[ ${#EMIT[@]} -gt 0 ] || { [ $fails = 0 ]; exit $?; }

build() {  # file out command [target args...] (fid takes the file first)
    local f="$1" o="$2" cmd="$3"; shift 3
    rm -f "$o"
    "$FID" "$cmd" "$f" "$@" -o "$o" > "$o.log" 2>&1 && [ -s "$o" ]
}
wslpath_of() { wsl.exe -e wslpath -a "$(cygpath -w "$1")" 2> /dev/null | tr -d '\r'; }

# 2. the runtime builtins
T=tests/bootstrap/test_machine_selfhost_rt.fd
if build "$T" "$OUT/rt$EXT" "${EMIT[@]}"; then
    if "$OUT/rt$EXT" < /dev/null > "$OUT/rt.txt" 2>&1 && diff <(expected "$T") <(tr -d '\r' < "$OUT/rt.txt") > /dev/null; then
        ok "selfhost_rt: expected lines (empty stdin)"
    else
        bad "selfhost_rt with empty stdin: $(head -c 300 "$OUT/rt.txt")"
    fi
    got=$(printf 'first line\r\nabcdefg' | "$OUT/rt$EXT" 2>&1 | tr -d '\r' | tail -3 | tr '\n' '|')
    if [ "$got" = "line: first line|bytes: abcd|eof: 0|" ]; then ok "selfhost_rt: piped input"; else bad "selfhost_rt piped: $got"; fi
else
    bad "selfhost_rt does not build: $(tail -3 "$OUT/rt$EXT.log")"
fi

# 3. the main stack
U=tests/nattools/main_stack_unit.fd
if build "$U" "$OUT/stack$EXT" "${EMIT[@]}"; then
    if ( ulimit -s 8192 2> /dev/null; "$OUT/stack$EXT" ) > "$OUT/stack.txt" 2>&1 && diff <(expected "$U") <(tr -d '\r' < "$OUT/stack.txt") > /dev/null; then
        ok "main stack: 400000 calls deep"
    else
        bad "main stack: $(head -c 200 "$OUT/stack.txt")"
    fi
else
    bad "main_stack_unit does not build: $(tail -3 "$OUT/stack$EXT.log")"
fi
if [ "$LINUX_TOO" = 1 ]; then
    if build "$U" "$OUT/stack_linux" emit --target x86_64-linux; then
        p=$(wslpath_of "$OUT/stack_linux")
        if wsl.exe -e bash -c "ulimit -s 8192; '$p'" > "$OUT/stack_linux.txt" 2>&1 && diff <(expected "$U") <(tr -d '\r' < "$OUT/stack_linux.txt") > /dev/null; then
            ok "main stack (x86_64-linux, WSL, ulimit -s 8192)"
        else
            bad "main stack (x86_64-linux): $(head -c 200 "$OUT/stack_linux.txt")"
        fi
    else
        bad "main_stack_unit does not build for x86_64-linux"
    fi
fi

# 4. the compiler's front end. The reference is another backend's image of
# the same program: pe_ir (`fid emit`) on Windows, the C path (`fid
# ccompile`) elsewhere; `fid run` interprets the parser too slowly for
# more than a tiny file, so it only checks hello.fd.
F=tests/nattools/selfhost_front_unit.fd
INPUTS=(tests/bootstrap/hello.fd compiler/token.fd)
[ "${BIG:-0}" = 1 ] && INPUTS+=(compiler/lexer.fd compiler/parser.fd)
if [ "$WIN" = 1 ]; then REFB=(emit); else REFB=(ccompile); fi
if build "$F" "$OUT/front$EXT" "${EMIT[@]}"; then
    t0=$(date +%s%N)
    "$OUT/front$EXT" "${INPUTS[@]}" 2>&1 | tr -d '\r' > "$OUT/front_nat.txt"
    t1=$(date +%s%N)
    if build "$F" "$OUT/front_ref$EXT" "${REFB[@]}"; then
        "$OUT/front_ref$EXT" "${INPUTS[@]}" 2>&1 | tr -d '\r' > "$OUT/front_ref.txt"
        t2=$(date +%s%N)
        if [ -s "$OUT/front_ref.txt" ] && diff "$OUT/front_ref.txt" "$OUT/front_nat.txt" > /dev/null; then
            ok "front end built natively = ${REFB[0]} image (${#INPUTS[@]} files; $(( (t1 - t0) / 1000000 )) ms native, $(( (t2 - t1) / 1000000 )) ms ${REFB[0]})"
        else
            bad "front end: native output differs from the ${REFB[0]} image"
            diff "$OUT/front_ref.txt" "$OUT/front_nat.txt" | head -6
        fi
    else
        echo "skip: the ${REFB[0]} reference image does not build"
    fi
    "$FID" run "$F" tests/bootstrap/hello.fd > "$OUT/front_run.raw" 2> "$OUT/front_run.err"
    tr -d '\r' < "$OUT/front_run.raw" | grep '^tests/bootstrap/hello.fd: ' > "$OUT/front_run.txt"
    # (under `fid run` the program's own file is an argument too)
    if [ -s "$OUT/front_run.txt" ] && diff "$OUT/front_run.txt" <(grep '^tests/bootstrap/hello.fd: ' "$OUT/front_nat.txt") > /dev/null; then
        ok "front end on hello.fd = fid run"
    else
        bad "front end on hello.fd: native '$(grep hello.fd "$OUT/front_nat.txt")', fid run '$(cat "$OUT/front_run.txt")' ($(tail -c 200 "$OUT/front_run.err"))"
    fi
else
    bad "selfhost_front_unit does not build: $(tail -3 "$OUT/front$EXT.log")"
fi

echo "machine_sp16_check: $fails failure(s)"
[ $fails = 0 ]
