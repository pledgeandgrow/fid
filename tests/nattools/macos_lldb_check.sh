#!/usr/bin/env bash
# TL5 / DB2 / DB4 on a real Mac: lldb (Xcode's) on a native macOS debug
# build. Nothing in the Windows or Linux lanes can run this (lldb needs the
# Mach-O to execute); scripts there only read the __DWARF segment
# (tests/nattools/macho_dwarf_check.py, dbg_scopes_check.py).
#   bash tests/nattools/macos_lldb_check.sh [fid]     (default release/fid)
# Builds tests/bootstrap/test_pe_dbg_vars.fd with `fid emit --debug` for the
# host (aarch64-macos or x86_64-macos), then in one lldb batch session:
#   - a breakpoint on test_pe_dbg_vars.fd:29 (@scale_add) is hit and
#     `frame variable` shows base = 6, factor = 7, acc = 42, step = 3;
#   - `bt` names scale and main with their source lines;
#   - `next` stops on line 30 (@scale_ret) with acc = 45;
#   - a breakpoint on the function count_bytes stops with s = "fides";
#   - `dwarfdump --verify` accepts the image's DWARF (when on PATH).
# Exit 0 when every check holds; 2 when not on macOS or without lldb.
set -u
FID="${1:-release/fid}"
T=tests/bootstrap/test_pe_dbg_vars.fd
if [ "$(uname -s)" != Darwin ]; then echo "not on macOS: nothing checked"; exit 2; fi
if ! command -v lldb > /dev/null 2>&1; then echo "SKIP: no lldb"; exit 2; fi
case "$(uname -m)" in arm64) TGT=aarch64-macos ;; *) TGT=x86_64-macos ;; esac
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
fail=0
ok() { echo "ok   $1"; }
no() { echo "FAIL $1"; fail=1; }
if ! "$FID" emit "$T" --target "$TGT" --debug -o "$OUT/t" > "$OUT/emit.log" 2>&1; then
    no "emit --debug --target $TGT"; tail -5 "$OUT/emit.log"; exit 1
fi
ok "emit --debug --target $TGT"
lldb --batch \
    -o "breakpoint set --file test_pe_dbg_vars.fd --line 29" \
    -o "breakpoint set --name count_bytes" \
    -o "run" \
    -o "frame variable" \
    -o "bt" \
    -o "next" \
    -o "frame variable acc" \
    -o "continue" \
    -o "frame variable s" \
    -o "kill" \
    -- "$OUT/t" > "$OUT/lldb.txt" 2>&1
cat "$OUT/lldb.txt"
has() {
    if grep -qE -- "$2" "$OUT/lldb.txt"; then ok "$1"; else no "$1 (no line matching: $2)"; fi
}
has "breakpoint on test_pe_dbg_vars.fd:29 resolved" "Breakpoint 1: .*test_pe_dbg_vars.fd:29"
has "stopped at line 29 in scale" "stop reason = breakpoint 1"
has "base = 6" "base = 6"
has "factor = 7" "factor = 7"
has "acc = 42" "acc = 42"
has "step = 3" "step = 3"
has "backtrace names scale" "frame #0: .*scale.*test_pe_dbg_vars.fd:29"
has "backtrace names main" "frame #1: .*main.*test_pe_dbg_vars.fd"
has "next stops on line 30" "test_pe_dbg_vars.fd:30"
has "acc = 45 after the step" "acc = 45"
has "breakpoint on count_bytes" "stop reason = breakpoint 2"
has "s = \"fides\"" 's = .*"fides"'
if command -v dwarfdump > /dev/null 2>&1; then
    if dwarfdump --verify "$OUT/t" > "$OUT/verify.txt" 2>&1; then ok "dwarfdump --verify"; else no "dwarfdump --verify"; tail -20 "$OUT/verify.txt"; fi
fi
[ $fail = 0 ] && echo "macos lldb check: ok" || echo "macos lldb check: FAILED"
exit $fail
