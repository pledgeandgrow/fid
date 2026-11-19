#!/usr/bin/env bash
# Checks of the machine-layer goals MI8, MI22, MI19 and MI20
# (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/machine_frame_check.sh [fid]   (default release/fid.exe)
# - MI8: tests/bootstrap/test_machine_frame.fd built with --stats for each
#   Linux target: `big`'s frame (the "[stats] big: ..., frame N" line) is
#   below 512 bytes and smaller than with FID_NAT_OFF=frameslots (the old
#   slot per IR value).
# - MI22: tests/bootstrap/test_machine_addrmode.fd's `sum_vec` has an
#   `ldx` in its MIR (FID_NAT_DUMP) on x86-64 and AArch64, none with
#   FID_NAT_OFF=ldx and none on RISC-V; `bump_all` an `stx` on x86-64.
# - every build with FID_VERIFY=1 (the MIR verifier, the allocation checker
#   and MI20's primitive-interface check) and, where the image runs here
#   (x86-64 Linux through WSL on Windows, natively on Linux), the output
#   equals the test's expected lines, with each switch on and off. MI19's
#   shared nat_ctx_swap / trampoline are exercised by
#   tests/bootstrap/test_machine_tasks_once.fd.
# - tests/nattools/enc-oracle-mi22.py when zig is on PATH (the SIB and
#   register-offset encodings against clang).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-frame"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# frame <stats file> <function>: the frame of that function's stats line
frame() { sed -n "s/^\[stats\] $2: .*, frame \([0-9]*\)\$/\1/p" "$1" | head -1; }

runs_here() {
    [ "$1" = x86_64-linux ] || return 1
    if [ -n "$RUN" ]; then return 0; fi
    [ "$(uname -m)" = x86_64 ] && [ "$(uname -s)" = Linux ]
}

check_run() {   # <name> <image> <test file>
    if runs_here "$4"; then
        local got
        got=$($RUN "$2" 2>&1)
        if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
    fi
}

T=tests/bootstrap/test_machine_frame.fd
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    img="$OUT/frame-$tgt"
    if FID_VERIFY=1 "$FID" emit "$T" --target "$tgt" --stats -o "$img" 2>"$img.stats" >/dev/null; then
        new=$(frame "$img.stats" big)
        FID_VERIFY=1 FID_NAT_OFF=frameslots "$FID" emit "$T" --target "$tgt" --stats -o "$img.old" 2>"$img.old.stats" >/dev/null
        old=$(frame "$img.old.stats" big)
        if [ -n "$new" ] && [ -n "$old" ] && [ "$new" -lt 512 ] && [ "$new" -lt "$old" ]; then
            ok "MI8 $tgt: big's frame $new bytes (a slot per value: $old)"
        else
            bad "MI8 $tgt: big's frame '${new}' (a slot per value: '${old}')"
        fi
        check_run "MI8 $tgt" "$img" "$T" "$tgt"
        check_run "MI8 $tgt frameslots off" "$img.old" "$T" "$tgt"
    else
        bad "MI8 $tgt: emit failed: $(tail -3 "$img.stats")"
    fi
done

# MI8: a spilled rematerialized vreg (RA8) has no slot. `keys` spills
# large constants across the call in its loop: its frame must be smaller
# than with FID_NAT_OFF=remat (each constant stored in a slot of its own);
# FID_VERIFY=1 also makes ng_mir_alloc report any rematerialized vreg
# given a slot ("MI8: rematerialized v.. has a spill slot").
T=tests/bootstrap/test_machine_frame_remat.fd
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    img="$OUT/remat-$tgt"
    if FID_VERIFY=1 "$FID" emit "$T" --target "$tgt" --stats -o "$img" 2>"$img.stats" >/dev/null; then
        new=$(frame "$img.stats" keys)
        FID_VERIFY=1 FID_NAT_OFF=remat "$FID" emit "$T" --target "$tgt" --stats -o "$img.old" 2>"$img.old.stats" >/dev/null
        old=$(frame "$img.old.stats" keys)
        if [ -n "$new" ] && [ -n "$old" ] && [ "$new" -lt "$old" ]; then
            ok "MI8 remat $tgt: keys's frame $new bytes (constants stored: $old)"
        else
            bad "MI8 remat $tgt: keys's frame '${new}' (constants stored: '${old}')"
        fi
        check_run "MI8 remat $tgt" "$img" "$T" "$tgt"
        check_run "MI8 remat $tgt remat off" "$img.old" "$T" "$tgt"
    else
        bad "MI8 remat $tgt: emit failed: $(grep -v '^\[' "$img.stats" | tail -3)"
    fi
done

T=tests/bootstrap/test_machine_addrmode.fd
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    img="$OUT/addr-$tgt"
    if FID_VERIFY=1 FID_NAT_DUMP='*' "$FID" emit "$T" --target "$tgt" -o "$img" 2>"$img.mir" >/dev/null; then
        n=$(grep -c '^\[mir\] [0-9]*: ldx' "$img.mir")
        ns=$(grep -c '^\[mir\] [0-9]*: stx' "$img.mir")
        if [ "$tgt" = riscv64-linux ]; then
            if [ "$n" -eq 0 ] && [ "$ns" -eq 0 ]; then ok "MI22 $tgt: no indexed access selected"; else bad "MI22 $tgt: $n ldx / $ns stx on RISC-V"; fi
        else
            if [ "$n" -gt 0 ] && [ "$ns" -gt 0 ]; then ok "MI22 $tgt: $n ldx, $ns stx"; else bad "MI22 $tgt: $n ldx, $ns stx (want both > 0)"; fi
        fi
        check_run "MI22 $tgt" "$img" "$T" "$tgt"
        FID_VERIFY=1 FID_NAT_OFF=ldx FID_NAT_DUMP='*' "$FID" emit "$T" --target "$tgt" -o "$img.off" 2>"$img.off.mir" >/dev/null
        n2=$(grep -c '^\[mir\] [0-9]*: \(ldx\|stx\)' "$img.off.mir")
        if [ "$n2" -eq 0 ]; then ok "MI22 $tgt: FID_NAT_OFF=ldx selects none"; else bad "MI22 $tgt: $n2 indexed accesses with ldx off"; fi
        check_run "MI22 $tgt ldx off" "$img.off" "$T" "$tgt"
    else
        bad "MI22 $tgt: emit failed: $(grep -v '^\[mir\]' "$img.mir" | tail -3)"
    fi
done

T=tests/bootstrap/test_machine_tasks_once.fd
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    img="$OUT/tasks-$tgt"
    if FID_VERIFY=1 "$FID" emit "$T" --target "$tgt" -o "$img" >/dev/null 2>"$img.err"; then
        ok "MI19/MI20 $tgt: built with FID_VERIFY=1"
        check_run "MI19 $tgt" "$img" "$T" "$tgt"
    else
        bad "MI19/MI20 $tgt: emit failed: $(tail -3 "$img.err")"
    fi
done

if command -v zig >/dev/null 2>&1; then
    if python3 tests/nattools/enc-oracle-mi22.py --fid "$FID" > "$OUT/oracle.txt" 2>&1; then
        ok "MI22 encodings agree with clang ($(tail -1 "$OUT/oracle.txt"))"
    else
        bad "MI22 encoder oracle: $(grep MISMATCH "$OUT/oracle.txt" | head -5)"
    fi
fi

if [ "$fails" -eq 0 ]; then echo "all machine frame / address-mode checks passed"; exit 0; fi
echo "$fails check(s) failed"
exit 1
