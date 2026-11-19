#!/usr/bin/env bash
# RA7 / RA3 (docs/roadmap/llvm-roadmap.md; docs/reference/argument-registers.md
# "Windows: code that Windows calls"): on --nat Windows the machine layer
# keeps values in rdi and rsi (argument-register hints, RA7; free argument
# registers, RA3), which Windows' x64 convention makes callee-saved. The
# only code Windows calls and then gets control back from is the vectored
# exception handler (compiler/nat_ir.fd ng_gen_primitives_win, `veh`):
# Windows' dispatcher goes on with rdi/rsi/rbx/rbp/r12-r15/xmm6-15 after it
# returns. The entry point and the thread start (`nat_win_tramp`) never
# return (ExitProcess, ExitThread). This check finds the handler's bytes
# in a --nat image and requires:
#   start: push rbp; push rdi; push rsi; mov rsi, rcx; sub rsp, 160
#          (55 57 56 48 89 CE 48 81 EC A0 00 00 00)
#   then the ten xmm6..xmm15 saves (movdqu [rsp + 16 i], xmmN), and
#   end:   add rsp, 160; pop rsi; pop rdi; pop rbp; ret
#          (48 81 C4 A0 00 00 00 5E 5F 5D C3)
# The Fides function it calls (pe_win_on_exception) keeps rbx and r12-r15
# itself (the layer's callee-saved registers), so with these the handler
# returns every register Windows' convention keeps. A change that drops
# the rdi/rsi saves (or the xmm ones) fails here, before a crash in
# ntdll after a first-chance exception that a later handler takes.
#   bash tests/nattools/win_callback_regs_check.sh [fid]   (Windows only)
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-win-callback"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "skip: --nat images run on Windows only (the bytes are checked anyway)" ;; esac
PY=python3
command -v "$PY" >/dev/null 2>&1 || PY=python
SRC="$OUT/veh_probe.fd"
cat >"$SRC" <<'EOF'
fn work(n: i64) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < n {
        s = s + i * 3 / 2;
        i = i + 1;
    }
    return s;
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(work(100));
    println(v.get(0));
    return 0;
}
EOF
fails=0
for mode in default noargs; do
    img="$OUT/veh_$mode.exe"
    if [ "$mode" = noargs ]; then
        FID_NAT_OFF=arghint,argfree "$FID" emit "$SRC" --nat -o "$img" >"$img.log" 2>&1
    else
        "$FID" emit "$SRC" --nat -o "$img" >"$img.log" 2>&1
    fi
    if [ $? -ne 0 ]; then echo "FAIL $mode: build"; tail -5 "$img.log"; fails=$((fails + 1)); continue; fi
    res=$("$PY" - "$img" <<'EOF'
import sys
b = open(sys.argv[1], "rb").read()
# (first the general registers: push rbp, rdi, rsi; mov rsi, rcx ...
# pop rsi, rdi, rbp; ret; then the xmm frame of MI14 inside)
start = bytes.fromhex("55 57 56 48 89 CE")
end = bytes.fromhex("5E 5F 5D C3")
i = b.find(start)
if i < 0:
    print("no handler prologue (push rbp, rdi, rsi; mov rsi, rcx): rdi/rsi not saved")
    sys.exit(0)
if b.find(start, i + 1) >= 0:
    print("two handler prologues")
    sys.exit(0)
j = b.find(end, i)
if j < 0 or j - i > 600:
    print("no matching epilogue (pop rsi, rdi, rbp; ret) after the prologue: rdi/rsi not restored")
    sys.exit(0)
body = b[i:j]
if body.find(bytes.fromhex("48 81 EC A0 00 00 00")) != 6 or not body.endswith(bytes.fromhex("48 81 C4 A0 00 00 00")):
    print("rdi, rsi and rbp kept, but no 160-byte xmm frame (sub rsp, 160 ... add rsp, 160)")
    sys.exit(0)
# movdqu [rsp + d], xmmN: F3 [REX 44 for xmm8..15] 0F 7F, ModRM with rm=100 (SIB 24)
saves = 0
k = 0
while k < len(body) - 3:
    if body[k] == 0xF3:
        q = k + 1
        if body[q] in (0x44,):
            q += 1
        if body[q] == 0x0F and body[q + 1] == 0x7F:
            saves += 1
    k += 1
if saves < 10:
    print("rdi, rsi and rbp kept, but only %d xmm saves in the handler (xmm6..xmm15 need 10)" % saves)
    sys.exit(0)
print("ok")
EOF
)
    if [ "$res" = ok ]; then echo "ok   $mode: the exception handler keeps rdi, rsi, rbp and xmm6..xmm15"; else echo "FAIL $mode: $res"; fails=$((fails + 1)); fi
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*)
            got=$("$img" 2>&1 | tr -d '\r')
            if [ "$got" = "7400" ]; then echo "ok   $mode: runs"; else echo "FAIL $mode: printed '$got', want 7400"; fails=$((fails + 1)); fi ;;
    esac
done
if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "all passed"
