#!/bin/bash
# tests/tp/check.sh [FID]: roadmap P6, tensor parallelism over several ranks
# on the C path (fid ccompile). One process per rank: p_tp_init(n) starts
# ranks 1..n-1 (FID_TP_WORLD=n in the environment overrides n); p_all_reduce,
# p_broadcast, p_all_gather, p_scatter and p_send_tensor/p_recv_tensor move
# tensors between them over pipes (docs/reference/tensor-parallel.md).
#
#   multirank.fd  A x B with A's columns and B's rows split over the ranks;
#                 the all-reduce must equal the product computed on one rank,
#                 and the same on 2 and on 3 ranks
#   rank_fail.fd  rank 1 panics: rank 0 must stop with an error, not hang
#
# Needs a C compiler (CC, as for ccompile). Run from the project root.
#
#   bash tests/tp/check.sh                      # release/fid.exe
#   bash tests/tp/check.sh release/fid-dev.exe
#
# The native backends (stdlib/pe_runtime/tensor.fd, merged into pe_ir and
# nat_ir builds) with TP_TARGET:
#   TP_TARGET=native        fid emit (the host: pe_ir on Windows)
#   TP_TARGET=x86_64-linux  fid emit --target x86_64-linux (nat_ir); from
#                           Windows the programs run in WSL
#   TP_TARGET=aarch64-linux / riscv64-linux: the same under qemu-user
#                           (qemu-aarch64 / qemu-riscv64 in WSL or Linux)
set -u
FID="${1:-release/fid.exe}"
TP_TARGET="${TP_TARGET:-}"
OUT=$(mktemp -d)
EXE=""
ON_WIN=0
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*) EXE=".exe"; ON_WIN=1 ;;
esac
QEMU=""
case "$TP_TARGET" in
    aarch64-linux) QEMU="qemu-aarch64" ;;
    riscv64-linux) QEMU="qemu-riscv64" ;;
esac
WSLRUN=0
case "$TP_TARGET" in
    *-linux) EXE=""; [ $ON_WIN -eq 1 ] && WSLRUN=1 ;;
esac
fail=0
ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }

# runp WORLD EXE: run a built program (FID_TP_WORLD=WORLD when not empty),
# at most 60 s, in WSL for a Linux target built on Windows
runp() {
    local world="$1" exe="$2"
    if [ $WSLRUN -eq 1 ]; then
        local lp
        lp=$(MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} wslpath -a "$(cygpath -m "$exe")" | tr -d '\r')
        if [ -n "$world" ]; then
            MSYS_NO_PATHCONV=1 timeout 60 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} env FID_TP_WORLD="$world" $QEMU "$lp"
        else
            MSYS_NO_PATHCONV=1 timeout 60 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} $QEMU "$lp"
        fi
    elif [ -n "$world" ]; then
        FID_TP_WORLD="$world" timeout 60 $QEMU "$exe"
    else
        timeout 60 $QEMU "$exe"
    fi
}

for p in multirank rank_fail rank_exit; do
    case "$TP_TARGET" in
        "") "$FID" ccompile "tests/tp/$p.fd" -o "$OUT/$p$EXE" > "$OUT/$p.build" 2>&1 ;;
        native) "$FID" emit "tests/tp/$p.fd" -o "$OUT/$p$EXE" > "$OUT/$p.build" 2>&1 ;;
        *) "$FID" emit "tests/tp/$p.fd" --target "$TP_TARGET" -o "$OUT/$p$EXE" > "$OUT/$p.build" 2>&1 ;;
    esac
    if [ $? -ne 0 ]; then
        bad "build $p" "$(tail -3 "$OUT/$p.build")"
    fi
done
echo "tp: ${TP_TARGET:-C (ccompile)}"

cat > "$OUT/two.expected" <<'X'
world 2
ranks [0, 1]
all_reduce matches one rank: yes
c[0] 0 -2.5 -18.5
broadcast sum 84
scatter/gather [[0], [10], [20], [30], [40], [50]]
ring [100, 0]
X
cat > "$OUT/three.expected" <<'X'
world 3
ranks [0, 1, 2]
all_reduce matches one rank: yes
c[0] 0 -2.5 -18.5
broadcast sum 126
scatter/gather [[0], [10], [20], [30], [40], [50]]
ring [200, 0, 100]
X

# two ranks, started by the program (p_tp_init(2))
runp "" "$OUT/multirank$EXE" > "$OUT/two.out" 2> "$OUT/two.err"
code=$?
nocr "$OUT/two.out"
if [ $code -eq 0 ] && diff -q "$OUT/two.expected" "$OUT/two.out" > /dev/null; then
    ok "2 ranks: all_reduce/all_gather/broadcast/scatter/ring"
else
    bad "2 ranks" "exit $code: $(diff "$OUT/two.expected" "$OUT/two.out" | head -10) $(head -c 300 "$OUT/two.err")"
fi

# three ranks from the environment
runp 3 "$OUT/multirank$EXE" > "$OUT/three.out" 2> "$OUT/three.err"
code=$?
nocr "$OUT/three.out"
if [ $code -eq 0 ] && diff -q "$OUT/three.expected" "$OUT/three.out" > /dev/null; then
    ok "3 ranks (FID_TP_WORLD=3)"
else
    bad "3 ranks" "exit $code: $(diff "$OUT/three.expected" "$OUT/three.out" | head -10) $(head -c 300 "$OUT/three.err")"
fi

# the sharded product is the same on 2 and 3 ranks
if [ "$(grep '^c\[0\]' "$OUT/two.out")" = "$(grep '^c\[0\]' "$OUT/three.out")" ] && [ -n "$(grep '^c\[0\]' "$OUT/two.out")" ]; then
    ok "same product on 2 and 3 ranks"
else
    bad "same product" "$(grep '^c\[0\]' "$OUT/two.out") / $(grep '^c\[0\]' "$OUT/three.out")"
fi

# a rank that fails: rank 0 stops with an error naming it (no hang)
runp "" "$OUT/rank_fail$EXE" > "$OUT/fail.out" 2> "$OUT/fail.err"
code=$?
nocr "$OUT/fail.err"
if [ $code -ne 0 ] && [ $code -ne 124 ] && grep -q "all_reduce on rank 0: rank 1 is gone" "$OUT/fail.err" && grep -q "rank 1 gives up" "$OUT/fail.err" && ! grep -q "not reached" "$OUT/fail.out"; then
    ok "a failed rank stops the run (exit $code)"
else
    bad "failed rank" "exit $code: $(head -c 400 "$OUT/fail.err")"
fi

# rank_exit.fd: rank 1 ends with status 3 after the collective; rank 0
# prints, waits for it at the end of main, and fails the run
runp "" "$OUT/rank_exit$EXE" > "$OUT/exit.out" 2> "$OUT/exit.err"
code=$?
nocr "$OUT/exit.out" "$OUT/exit.err"
if [ $code -eq 1 ] && [ "$(cat "$OUT/exit.out")" = "world 2 sum 6" ] && grep -q "tp: rank 1 exited with status 3" "$OUT/exit.err"; then
    ok "rank 0 waits for the others and fails with them (exit $code)"
else
    bad "rank 0 waits" "exit $code: $(head -c 200 "$OUT/exit.out") / $(head -c 300 "$OUT/exit.err")"
fi

rm -rf "$OUT"
if [ $fail -ne 0 ]; then
    echo "tp: $fail check(s) failed"
    exit 1
fi
echo "tp: all checks passed"
