#!/bin/bash
# tests/uefi/run.sh — UEFI applications from `fid emit --uefi` (roadmap
# Phase 16, compiler/pe_ir.fd pe_ir_generate_uefi).
#
#   1. the emitter's own unit test, compiled through C and run:
#        fid ccompile tests/uefi/uefi_image_unit.fd -o unit.exe
#        unit.exe <out>/unit.efi
#      (its output is compared with the `// Expected output:` block)
#   2. `fid emit tests/bootstrap/test_uefi_hello.fd --uefi -o hello.efi`
#   3. tests/uefi/check_efi.py on both images (PE32+ structure, subsystem
#      EFI_APPLICATION, relocations, entry)
#   4. when QEMU and OVMF are there (here, or in WSL from Git Bash on
#      Windows): boot each image as the removable-media boot file
#      EFI/BOOT/BOOTX64.EFI of a FAT folder (tests/uefi/boot.sh) and compare
#      what it printed on the console with the expected output
#
# Usage:  FID=release/fid.exe bash tests/uefi/run.sh
# Environment:
#   FID      the compiler (default: release/fid, else release/fid-dev)
#   FID_UEFI_DRIVER  a built tests/uefi/uefi_emit_driver.fd to make
#            hello.efi with instead of `$FID emit --uefi`
#   QEMU     default qemu-system-x86_64
#   OVMF     OVMF firmware image (default: the first found by
#            tests/uefi/boot.sh; none: step 4 is skipped)
#   OUT      where the images go (default: a new temporary folder)
#   TIMEOUT  seconds per QEMU boot at most (default 90)
#   UNIT=0   skip step 1; UNIT=run runs it through the interpreter (slow)
#   CC       the C compiler of step 1 (default "zig cc")
# Exit status: 0 when every step that ran passed.
set -u
cd "$(dirname "$0")/../.." || exit 2

FID="${FID:-}"
if [ -z "$FID" ]; then
  for f in release/fid release/fid.exe release/fid-dev release/fid-dev.exe; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
[ -n "$FID" ] || { echo "error: no fid (set FID)"; exit 2; }
QEMU="${QEMU:-qemu-system-x86_64}"
TIMEOUT="${TIMEOUT:-90}"
UNIT="${UNIT:-1}"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-/tmp}/uefi-test.XXXXXX")}"
mkdir -p "$OUT"
PY="$(command -v python3 || command -v python)"
fail=0

# The lines after `// Expected output:` up to a line that is not a comment
expected() {
  awk '/^\/\/ Expected output:/ {on=1; next}
       on && /^\/\/ / {sub(/^\/\/ /, ""); print; next}
       on {exit}' "$1"
}

if [ "$UNIT" != 0 ]; then
  if [ "$UNIT" = run ]; then
    how="through the interpreter"
    "$FID" run tests/uefi/uefi_image_unit.fd "$OUT/unit.efi" 2>/dev/null | tr -d '\r' > "$OUT/unit.out"
  else
    # compiled through C (a few minutes; the interpreter takes much longer)
    how="compiled through C"
    if CC="${CC:-zig cc}" "$FID" ccompile tests/uefi/uefi_image_unit.fd -o "$OUT/unit.exe" > "$OUT/unit-cc.log" 2>&1; then
      "$OUT/unit.exe" "$OUT/unit.efi" 2>/dev/null | tr -d '\r' > "$OUT/unit.out"
    else
      echo "FAIL unit: ccompile (log: $OUT/unit-cc.log)"; : > "$OUT/unit.out"
    fi
  fi
  if diff <(expected tests/uefi/uefi_image_unit.fd) "$OUT/unit.out" >/dev/null; then
    echo "PASS unit (pe_ir_generate_uefi $how)"
  else
    echo "FAIL unit"; diff <(expected tests/uefi/uefi_image_unit.fd) "$OUT/unit.out"; fail=1
  fi
  if [ -f "$OUT/unit.efi" ]; then
    "$PY" tests/uefi/check_efi.py "$OUT/unit.efi" Fides "hello from UEFI
" || fail=1
  fi
fi

# FID_UEFI_DRIVER: a built tests/uefi/uefi_emit_driver.fd instead of
# `$FID emit --uefi` (the same steps; for a fid without the --uefi flag)
if [ -n "${FID_UEFI_DRIVER:-}" ]; then
  emit_cmd=("$FID_UEFI_DRIVER" tests/bootstrap/test_uefi_hello.fd "$OUT/hello.efi")
else
  emit_cmd=("$FID" emit tests/bootstrap/test_uefi_hello.fd --uefi -o "$OUT/hello.efi")
fi
# (the 'Emitted UEFI application' line is required: a fid without the flag
# builds an ordinary hello.efi.exe, and Git Bash's `[ -f hello.efi ]` is
# true for hello.efi.exe)
rm -f "$OUT/hello.efi" "$OUT/hello.efi.exe"
emit_ok=0
if "${emit_cmd[@]}" >"$OUT/emit.log" 2>&1 && grep -q 'Emitted UEFI application' "$OUT/emit.log" \
   && [ -f "$OUT/hello.efi" ] && [ ! -f "$OUT/hello.efi.exe" ]; then
  echo "PASS emit --uefi ($(grep -m1 'Emitted UEFI' "$OUT/emit.log"))"
  emit_ok=1
  "$PY" tests/uefi/check_efi.py "$OUT/hello.efi" Fides "greeting: " || fail=1
else
  echo "FAIL emit --uefi:"; cat "$OUT/emit.log"; fail=1
fi

# Where QEMU runs: here, or (Git Bash on Windows) inside WSL
RUNNER=local
if ! command -v "$QEMU" >/dev/null 2>&1 && command -v wsl >/dev/null 2>&1 && command -v cygpath >/dev/null 2>&1; then
  if wsl -- bash -c "command -v $QEMU" >/dev/null 2>&1; then RUNNER=wsl; fi
fi
# a path as the runner sees it
rpath() {
  if [ "$RUNNER" = wsl ]; then wsl wslpath -a "$(cygpath -m "$1")" | tr -d '\r'; else echo "$1"; fi
}

# boot <image> <expected-output file>
boot() {
  local img="$1" exp="$2" dir="$OUT/esp-$(basename "$1" .efi)"
  mkdir -p "$dir/EFI/BOOT"
  # (an earlier run's files out first: in Git Bash `cp x BOOTX64.EFI` writes
  # into an existing BOOTX64.EFI.exe, a name the firmware does not boot)
  rm -f "$dir/EFI/BOOT/BOOTX64.EFI" "$dir/EFI/BOOT/BOOTX64.EFI.exe" "$dir/app.efi" "$dir/app.efi.exe" \
    "$dir/console.raw" "$dir/console.txt"
  cp "$img" "$dir/EFI/BOOT/BOOTX64.EFI"
  cp "$img" "$dir/app.efi"
  printf 'fs0:\r\napp.efi\r\nreset -s\r\n' > "$dir/startup.nsh"
  local last rc=0
  last="$(tail -n 1 "$exp")"
  if [ "$RUNNER" = wsl ]; then
    # (MSYS_NO_PATHCONV: Git Bash would turn /mnt/... into Windows paths)
    MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' \
    wsl -- env QEMU="$QEMU" OVMF="${OVMF:-}" bash "$(rpath tests/uefi/boot.sh)" "$(rpath "$dir")" \
      "$(rpath "$dir")/console.raw" "$TIMEOUT" "$last" > "$dir/boot.log" 2>&1 || rc=$?
  else
    QEMU="$QEMU" OVMF="${OVMF:-}" bash tests/uefi/boot.sh "$dir" "$dir/console.raw" "$TIMEOUT" "$last" \
      > "$dir/boot.log" 2>&1 || rc=$?
  fi
  if [ "$rc" = 3 ]; then
    echo "SKIP boot $(basename "$img"): no $QEMU or OVMF (apt install qemu-system-x86 ovmf)"
    return
  fi
  # the console: ANSI sequences and CRs out
  sed -e 's/\x1b\[[0-9;?]*[A-Za-z]//g' -e 's/\r//g' "$dir/console.raw" > "$dir/console.txt"
  local missing=0
  while IFS= read -r line; do
    grep -qxF -- "$line" "$dir/console.txt" || { echo "  missing: $line"; missing=1; }
  done < "$exp"
  if [ $missing = 0 ]; then echo "PASS boot $(basename "$img")"; else
    echo "FAIL boot $(basename "$img") (console: $dir/console.txt)"; fail=1; fi
}

if [ "$emit_ok" = 1 ]; then
  expected tests/bootstrap/test_uefi_hello.fd > "$OUT/hello.exp"
  boot "$OUT/hello.efi" "$OUT/hello.exp"
fi
if [ -f "$OUT/unit.efi" ]; then
  printf 'hello from UEFI\nFides\n6765\n' > "$OUT/unit.exp"
  boot "$OUT/unit.efi" "$OUT/unit.exp"
fi
exit $fail
