#!/usr/bin/env bash
# Checks of the machine-layer goals of batch 8, run after the compiler is
# built:
#   bash tests/nattools/machine_batch8_check.sh [fid]   (default release/fid.exe)
# - DB6 (llvm-roadmap): tests/bootstrap/test_machine_cfi_saves.fd
#   * on x86-64, AArch64 and RISC-V Linux: .debug_frame gives the
#     callee-saved registers the prologue stores a rule, and on x86-64 the
#     store is really at CFA - 8n (tests/nattools/cfi_saves_check.py);
#     scripts/dbg-check.py still passes (the CFA at entry, after the
#     prologue, at each ret);
#   * `fid emit --nat`: an exception directory whose RUNTIME_FUNCTIONs each
#     cover a framed function with the shared UNWIND_INFO
#     (tests/nattools/pe_unwind_check.py); FID_NAT_OFF=unwind: none.
# - DB9: the sampling profiler on Windows `--nat` (win.fd's sampler
#   thread) and macOS (setitimer, the Darwin ucontext): scripts/
#   prof-check.sh's program with FID_PROF=1 prints the report, fib first
#   by self time, main on the stack; run here on a Windows host (--nat) and
#   on a Mac (its own target).
# - F7: tests/bootstrap/test_machine_os_services.fd (errno, directory
#   listing, spawn) for aarch64-macos and x86_64-macos: scripts/
#   macho-check.py passes, the binds name the new libSystem imports
#   (_socket, _poll, _fork, _execve, _wait4, _setitimer, _sigaltstack);
#   on a Mac the program prints the test's expected lines. --nat and
#   x86-64 Linux (WSL) run it too.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch8"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
RUN=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

wslpath_of() {
    local p
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
}

# --- DB6: .debug_frame on Linux ---------------------------------------------
T=tests/bootstrap/test_machine_cfi_saves.fd
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    img="$OUT/cfi-$tgt"
    build "$img" "$T" "$tgt" || { bad "DB6 $tgt builds"; continue; }
    if r=$($PY tests/nattools/cfi_saves_check.py "$img"); then ok "DB6 $tgt: $r"; else bad "DB6 $tgt: $(echo "$r" | head -3)"; fi
done
if [ -f scripts/dbg-check.py ]; then
    if r=$($PY scripts/dbg-check.py --fid "$FID" 2>&1); then ok "DB6 dbg-check.py"; else bad "DB6 dbg-check.py: $(echo "$r" | tail -3)"; fi
fi
if [ -n "$RUN" ] || [ "$HOST" = linux ]; then
    img="$OUT/cfi-x86_64-linux"
    if [ -f "$img" ]; then
        if [ -n "$RUN" ]; then got=$(MSYS_NO_PATHCONV=1 $RUN "$(wslpath_of "$img")" 2>&1); else got=$("$img" 2>&1); fi
        if [ "$got" = "$(expected "$T")" ]; then ok "DB6 x86_64-linux runs"; else bad "DB6 x86_64-linux runs (output differs)"; fi
    fi
fi

# --- DB6: .pdata / .xdata for --nat -----------------------------------------
img="$OUT/cfi-nat.exe"
if FID_VERIFY=1 "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1; then
    if r=$($PY tests/nattools/pe_unwind_check.py "$img" --min 3); then ok "DB6 --nat: $r"; else bad "DB6 --nat: $(echo "$r" | head -3)"; fi
    if FID_NAT_OFF=unwind "$FID" emit "$T" --nat -o "$OUT/cfi-nat-off.exe" >/dev/null 2>&1; then
        if $PY tests/nattools/pe_unwind_check.py "$OUT/cfi-nat-off.exe" >/dev/null; then bad "DB6 --nat FID_NAT_OFF=unwind still has unwind data"
        else ok "DB6 --nat FID_NAT_OFF=unwind: no exception directory"; fi
    fi
    if [ "$HOST" = windows ]; then
        got=$("$img" 2>&1 | tr -d '\r')
        if [ "$got" = "$(expected "$T")" ]; then ok "DB6 --nat runs"; else bad "DB6 --nat runs (output differs)"; fi
    fi
else
    bad "DB6 --nat builds"
fi

# --- DB9: the profiler on Windows --nat and macOS ---------------------------
PROF="$OUT/prof_hot.fd"
cat > "$PROF" <<'FD'
fn fib(n: i64) -> i64 {
    if n < 2 {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

fn work(k: i64) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < k {
        s = s + fib(24 + i % 4);
        i = i + 1;
    }
    return s;
}

fn main() -> i64 {
    println(work(16));
    return 3;
}
FD
prof_ok() {
    # $1 label, $2 stdout, $3 stderr file, $4 exit code
    if [ "$2" != "1756816" ] || [ "$4" != 3 ]; then bad "$1: output '$2', exit $4 (want 1756816, 3)"; return; fi
    local n first
    n=$(sed -n 's/^profile (FID_PROF): \([0-9]*\) samples.*/\1/p' "$3" | tail -1)
    first=$(sed -n '/self%/{n;p;q}' "$3" | awk '{print $NF}')
    if [ -z "$n" ] || [ "$n" -le 0 ]; then bad "$1: no samples ($(head -2 "$3"))"; return; fi
    if [ "$first" != fib ]; then bad "$1: first by self time is '$first', want fib"; return; fi
    if ! grep -q " main$" "$3"; then bad "$1: main is not on the stack"; return; fi
    ok "$1: $n samples, fib first, main on the stack"
}
if [ "$HOST" = windows ]; then
    img="$OUT/prof-nat.exe"
    if "$FID" emit "$PROF" --nat -o "$img" >"$img.log" 2>&1; then
        "$img" >/dev/null 2>"$OUT/prof-nat.err"
        if [ -s "$OUT/prof-nat.err" ]; then bad "DB9 --nat without FID_PROF: something on stderr"; else ok "DB9 --nat without FID_PROF: quiet"; fi
        out=$(FID_PROF=1 "$img" 2>"$OUT/prof-nat.err" | tr -d '\r'); rc=${PIPESTATUS[0]}
        tr -d '\r' < "$OUT/prof-nat.err" > "$OUT/prof-nat.err2"
        prof_ok "DB9 --nat FID_PROF=1" "$out" "$OUT/prof-nat.err2" "$rc"
    else
        bad "DB9 --nat builds"
    fi
fi
MAC_HOST_TGT=""
if [ "$HOST" = macos ]; then
    if [ "$(uname -m)" = arm64 ]; then MAC_HOST_TGT=aarch64-macos; else MAC_HOST_TGT=x86_64-macos; fi
    img="$OUT/prof-mac"
    if "$FID" emit "$PROF" --target "$MAC_HOST_TGT" -o "$img" >"$img.log" 2>&1; then
        chmod +x "$img"
        out=$(FID_PROF=1 "$img" 2>"$OUT/prof-mac.err"); rc=$?
        prof_ok "DB9 $MAC_HOST_TGT FID_PROF=1" "$out" "$OUT/prof-mac.err" "$rc"
    else
        bad "DB9 $MAC_HOST_TGT builds"
    fi
fi

# --- F7: the macOS runtime services -----------------------------------------
T=tests/bootstrap/test_machine_os_services.fd
for tgt in aarch64-macos x86_64-macos; do
    img="$OUT/os-$tgt"
    if ! "$FID" emit "$T" --target "$tgt" -o "$img" >"$img.log" 2>&1; then bad "F7 $tgt builds"; continue; fi
    if [ -f scripts/macho-check.py ]; then
        if $PY scripts/macho-check.py "$img" --quiet >/dev/null 2>&1; then ok "F7 $tgt: macho-check.py"; else bad "F7 $tgt: macho-check.py"; fi
    fi
    # each import as a bind opcode: SET_SYMBOL_TRAILING_FLAGS_IMM 0, name, NUL
    missing=$($PY -c 'import sys
d = open(sys.argv[1], "rb").read()
print(" ".join(s for s in sys.argv[2:] if (b"\x40" + s.encode() + b"\0") not in d))' "$img" \
        _socket _bind _connect _accept _poll _fcntl _fork _execve _wait4 _setitimer _sigaltstack __exit _getppid)
    if [ -z "$missing" ]; then ok "F7 $tgt: binds the socket, poll, process and timer imports"; else bad "F7 $tgt: no bind for $missing"; fi
    if [ "$tgt" = "$MAC_HOST_TGT" ]; then
        chmod +x "$img"
        got=$(cd "$OUT" && "./os-$tgt" 2>&1)
        if [ "$got" = "$(expected "$T")" ]; then ok "F7 $tgt runs"; else bad "F7 $tgt runs (output differs)"; fi
    fi
done
img="$OUT/os-nat.exe"
if "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1; then
    if [ "$HOST" = windows ]; then
        got=$(cd "$OUT" && ./os-nat.exe 2>&1 | tr -d '\r')
        if [ "$got" = "$(expected "$T")" ]; then ok "F7 test on --nat runs"; else bad "F7 test on --nat runs (output differs)"; fi
    fi
else
    bad "F7 test builds for --nat"
fi
img="$OUT/os-x86_64-linux"
if "$FID" emit "$T" --target x86_64-linux -o "$img" >"$img.log" 2>&1; then
    if [ -n "$RUN" ]; then
        got=$(cd "$OUT" && MSYS_NO_PATHCONV=1 $RUN "$(wslpath_of "$img")" 2>&1)
        if [ "$got" = "$(expected "$T")" ]; then ok "F7 test on x86_64-linux runs"; else bad "F7 test on x86_64-linux runs (output differs)"; fi
    elif [ "$HOST" = linux ] && [ "$(uname -m)" = x86_64 ]; then
        got=$(cd "$OUT" && "$img" 2>&1)
        if [ "$got" = "$(expected "$T")" ]; then ok "F7 test on x86_64-linux runs"; else bad "F7 test on x86_64-linux runs (output differs)"; fi
    fi
else
    bad "F7 test builds for x86_64-linux"
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
