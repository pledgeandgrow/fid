#!/bin/bash
# tests/uefi/boot.sh — boot one UEFI application under QEMU with OVMF and
# keep what it printed on the console (used by tests/uefi/run.sh, which
# runs it directly or, from Git Bash on Windows, inside WSL).
#
#   boot.sh <esp folder> <console file> <timeout s> <last expected line>
#
# The folder is the EFI system partition: the image is EFI/BOOT/BOOTX64.EFI
# there (the firmware boots removable media without a shell; OVMF builds
# without the UEFI shell, such as Ubuntu's, need this), and startup.nsh runs
# it under a shell when the firmware starts one instead. QEMU stops when the
# last expected line is on the console, or after the timeout.
# Environment: QEMU (default qemu-system-x86_64), OVMF (default: the first
# firmware image found in the usual places).
set -u
esp="$1"; con="$2"; t="$3"; last="$4"
QEMU="${QEMU:-qemu-system-x86_64}"
OVMF="${OVMF:-}"
if [ -z "$OVMF" ]; then
  for f in /usr/share/ovmf/OVMF.fd /usr/share/qemu/OVMF.fd /usr/share/OVMF/OVMF.fd \
           /usr/share/edk2/ovmf/OVMF.fd /usr/share/OVMF/OVMF_CODE_4M.fd /usr/share/OVMF/OVMF_CODE.fd \
           /usr/share/edk2/ovmf/OVMF_CODE.fd /usr/share/edk2-ovmf/x64/OVMF_CODE.fd \
           /usr/share/qemu/edk2-x86_64-code.fd "/c/Program Files/qemu/share/edk2-x86_64-code.fd"; do
    if [ -f "$f" ]; then OVMF="$f"; break; fi
  done
fi
if ! command -v "$QEMU" >/dev/null 2>&1 || [ -z "$OVMF" ]; then
  echo "SKIP"
  exit 3
fi
# a firmware-only image (…_CODE…) goes in flash; a whole one is the BIOS
case "$(basename "$OVMF")" in
  *CODE*|*code*) fw=(-drive "if=pflash,format=raw,readonly=on,file=$OVMF") ;;
  *) fw=(-bios "$OVMF") ;;
esac
"$QEMU" -machine q35 -m 256 -nographic -no-reboot -net none "${fw[@]}" \
  -drive "format=raw,file=fat:rw:$esp" > "$con" 2>&1 < /dev/null &
pid=$!
n=0
while [ "$n" -lt "$t" ] && kill -0 "$pid" 2>/dev/null; do
  if sed -e 's/\x1b\[[0-9;?]*[A-Za-z]//g' -e 's/\r//g' "$con" 2>/dev/null | grep -qxF -- "$last"; then
    sleep 1
    break
  fi
  sleep 1
  n=$((n + 1))
done
kill "$pid" 2>/dev/null
wait "$pid" 2>/dev/null
exit 0
