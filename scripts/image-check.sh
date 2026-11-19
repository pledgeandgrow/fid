#!/bin/bash
# image-check.sh — every executable image our own writers produce, read by
# the system's tools (own-backend roadmap RL9): an image must pass every
# reader that is installed, with no error and no warning.
#
# Usage: scripts/image-check.sh [test.fd ...]
#   default programs: a few tests of tests/bootstrap/ (hello, closures,
#   maps, floats, tasks)
#   FID=<fid>      the compiler (release/fid.exe, else release/fid)
#   FORMATS="elf pe macho obj efi coff"   which writers to check (default
#                  all of them)
#   OUT=<dir>      build/image-check
#
# Writers and the images they make:
#   elf    `fid emit --target x86_64-linux|aarch64-linux|riscv64-linux`
#          (compiler/nat_ir.fd + nat_enc.fd: static ELF with debug sections)
#   pe     `fid emit` on Windows (pe_ir, the default) and
#          `fid emit --target x86_64-windows --nat` (the machine layer)
#   macho  `fid emit --target aarch64-macos|x86_64-macos`
#   obj    `fid emit --obj --target x86_64-linux|aarch64-linux` (TG12: ELF
#          relocatable objects, nat_elf_object)
#   efi    `fid emit tests/bootstrap/test_uefi_hello.fd --uefi` (a UEFI
#          application, pe_build_efi; once, not per test program)
#   coff   `fid codegen <f> --coff` (the machine layer's COFF object,
#          nat_ir.fd nat_emit_object)
# Our own readers, run on every image whatever is installed (so no format
# is left unchecked): scripts/image-lint.py (ELF, PE, COFF: headers,
# segments, sections, symbols, relocations, imports, base relocations,
# .pdata, checksum), scripts/macho-check.py (Mach-O), tests/uefi/
# check_efi.py (what a UEFI loader needs).
# Readers, each used when it is installed:
#   ELF    readelf -W -a --debug-dump=info,line; llvm-readobj --all;
#          llvm-objdump -d; llvm-dwarfdump --verify
#   PE     llvm-readobj --all; llvm-objdump -d; dumpbin /headers /imports
#          (on Windows, from a Visual Studio prompt); objdump -x (binutils
#          with PE support)
#   Mach-O llvm-readobj --all; llvm-objdump -d; otool -hlv (macOS);
#          scripts/macho-check.py (what the kernel and dyld check, with the
#          code signature)
#   ELF and COFF objects: readelf -W -a / llvm-readobj --all /
#          llvm-objdump -dr; dumpbin and objdump -x for COFF
#   On Windows, dumpbin is also found in Visual Studio's MSVC tools without
#   a prompt (DUMPBIN=<path>), and readelf/objdump/llvm-* in WSL
#   (WSL_DISTRO=<name>, none: off) when they are not on PATH.
#   READERS_ONLY=1: build nothing, list which readers would run.
# A reader that exits non-zero, or reports a warning or an error, fails the
# image; its output is kept in $OUT/<image>.<tool>.log (stderr: .err).
#
# Exit status: the number of failed checks. Readers that are missing are
# listed; a format with no reader at all is reported as unchecked.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/image-check}"
FORMATS="${FORMATS:-elf pe macho obj efi coff}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=posix ;; esac
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
if [ $# -gt 0 ]; then
  TESTS=("$@")
else
  TESTS=()
  for t in hello test_closure test_hashmap test_float test_tasks; do
    [ -f "tests/bootstrap/$t.fd" ] && TESTS+=("tests/bootstrap/$t.fd")
  done
fi

have() { command -v "$1" > /dev/null 2>&1; }
# On Windows a reader missing from PATH is looked for in two more places:
#   dumpbin  the newest Visual Studio's MSVC tools (no prompt needed);
#            DUMPBIN=<path> picks one
#   readelf, objdump, llvm-*  a WSL distribution that has them
#            (WSL_DISTRO=<name>, default the first one; WSL_DISTRO=none: off);
#            the image path is passed as /mnt/<drive>/...
DUMPBIN="${DUMPBIN:-}"
WSL_D=""
if [ "$HOST" = windows ]; then
  if [ -z "$DUMPBIN" ] && ! have dumpbin; then
    # 64-bit Visual Studio (2022 and later) first; in each, the glob sorts,
    # so the last match is the newest
    for root in "/c/Program Files/Microsoft Visual Studio" "/c/Program Files (x86)/Microsoft Visual Studio"; do
      for d in "$root"/*/*/VC/Tools/MSVC/*/bin/Hostx64/x64/dumpbin.exe; do
        [ -x "$d" ] && DUMPBIN="$d"
      done
      [ -n "$DUMPBIN" ] && break
    done
  fi
  WSL_D="${WSL_DISTRO:-}"
  if [ -z "$WSL_D" ] && have wsl.exe; then
    WSL_D=$(wsl.exe -l -q 2>/dev/null | tr -d '\0\r' | grep -v '^docker-desktop' | head -n 1)
  fi
  [ "$WSL_D" = none ] && WSL_D=""
fi
[ -z "$DUMPBIN" ] && have dumpbin && DUMPBIN=dumpbin
WSL_HAS=" "
if [ -n "$WSL_D" ]; then
  WSL_HAS=" $(wsl.exe -d "$WSL_D" -e sh -c 'for t in readelf objdump llvm-readobj llvm-objdump llvm-dwarfdump; do command -v $t > /dev/null 2>&1 && printf "%s " $t; done' 2>/dev/null | tr -d '\0\r') "
fi
# rd <tool>: true when the tool runs here or in WSL
rd() { have "$1" || case "$WSL_HAS" in *" $1 "*) return 0 ;; *) return 1 ;; esac; }
# run_rd <tool> <args...> <image>: run it (the last argument is the image)
run_rd() {
  local tool="$1"; shift
  if have "$tool"; then "$tool" "$@"; return; fi
  local args=("$@") n=$# img
  if [ "$n" -gt 0 ] && [ -f "${args[$((n-1))]}" ]; then
    img=$(cd "$(dirname "${args[$((n-1))]}")" && pwd)/$(basename "${args[$((n-1))]}")
    img="/mnt/$(echo "$img" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
    args[$((n-1))]="$img"
  fi
  MSYS_NO_PATHCONV=1 wsl.exe -d "$WSL_D" -e "$tool" "${args[@]}"   # keep /mnt/... as is
}
bad=0; ok=0; missing=""
note_missing() { case " $missing " in *" $1 "*) ;; *) missing="$missing $1" ;; esac; }

# check <image> <tool label> <command...>: run a reader over the image. A
# finding is a non-zero exit or a warning/error line on stderr (where
# readelf, llvm-readobj and objdump report them; dumpbin, which reports on
# stdout, is checked there too). stdout is not searched otherwise: a
# disassembly or a symbol table may hold a function named `error`.
check() {
  local img="$1" label="$2"; shift 2
  local log="$img.$label.log" err="$img.$label.err"
  "$@" > "$log" 2> "$err"
  local rc=$? hit=""
  hit=$(grep -aiE 'warning|error' "$err" | head -n 2 | tr '\n' ' ')
  if [ "$label" = dumpbin ] && [ -z "$hit" ]; then
    hit=$(grep -aiE ' warning [A-Z]+[0-9]+| fatal error|: error ' "$log" | head -n 2 | tr '\n' ' ')
  fi
  if [ "$label" = otool ] && [ -z "$hit" ]; then
    # otool reports a broken file on stdout
    hit=$(grep -aiE 'truncated or malformed|bad (load command|section)|past end of file' "$log" | head -n 2 | tr '\n' ' ')
  fi
  if [ $rc != 0 ] && [ -z "$hit" ]; then
    # a checker that reports on stdout (tests/uefi/check_efi.py)
    hit=$(grep -aiE 'fail|error' "$log" | head -n 2 | tr '\n' ' ')
  fi
  if [ $rc != 0 ] || [ -n "$hit" ]; then
    echo "  FAIL $label $(basename "$img") (rc $rc): $hit"
    bad=$((bad + 1))
  else
    ok=$((ok + 1))
  fi
}

readers_elf() {
  local img="$1" n=0
  if rd readelf; then check "$img" readelf run_rd readelf -W -a --debug-dump=info,line "$img"; n=$((n+1)); else note_missing readelf; fi
  if rd llvm-readobj; then check "$img" llvm-readobj run_rd llvm-readobj --all "$img"; n=$((n+1)); else note_missing llvm-readobj; fi
  if rd llvm-objdump; then check "$img" llvm-objdump run_rd llvm-objdump -d "$img"; n=$((n+1)); else note_missing llvm-objdump; fi
  if rd llvm-dwarfdump; then check "$img" llvm-dwarfdump run_rd llvm-dwarfdump --verify "$img"; n=$((n+1)); else note_missing llvm-dwarfdump; fi
  check "$img" image-lint "$PY" scripts/image-lint.py "$img" --quiet; n=$((n+1))
  return $n
}
# ELF relocatable objects (TG12): no segments, so no DWARF line check of
# addresses; the relocations are disassembled with their targets
readers_obj() {
  local img="$1" n=0
  if rd readelf; then check "$img" readelf run_rd readelf -W -a "$img"; n=$((n+1)); else note_missing readelf; fi
  if rd llvm-readobj; then check "$img" llvm-readobj run_rd llvm-readobj --all "$img"; n=$((n+1)); else note_missing llvm-readobj; fi
  if rd llvm-objdump; then check "$img" llvm-objdump run_rd llvm-objdump -dr "$img"; n=$((n+1)); else note_missing llvm-objdump; fi
  check "$img" image-lint "$PY" scripts/image-lint.py "$img" --quiet; n=$((n+1))
  return $n
}
# COFF objects: what reads PE reads them too
readers_coff() {
  local img="$1" n=0
  if rd llvm-readobj; then check "$img" llvm-readobj run_rd llvm-readobj --all "$img"; n=$((n+1)); else note_missing llvm-readobj; fi
  if rd llvm-objdump; then check "$img" llvm-objdump run_rd llvm-objdump -dr "$img"; n=$((n+1)); else note_missing llvm-objdump; fi
  if [ -n "$DUMPBIN" ]; then
    check "$img" dumpbin "$DUMPBIN" -nologo -headers -symbols -relocations "$(cygpath -w "$img" 2>/dev/null || echo "$img")"; n=$((n+1))
  else note_missing dumpbin; fi
  if [ -n "$OBJDUMP_PE" ]; then
    check "$img" objdump run_rd objdump -x "$img"; n=$((n+1))
  else note_missing "objdump(pei-x86-64)"; fi
  check "$img" image-lint "$PY" scripts/image-lint.py "$img" --quiet; n=$((n+1))
  return $n
}
# UEFI applications: the PE readers, and what a UEFI loader checks
readers_efi() {
  local img="$1" n=0
  readers_pe "$img"
  n=$?
  check "$img" check-efi "$PY" tests/uefi/check_efi.py "$img"; n=$((n+1))
  return $n
}
OBJDUMP_PE=""   # objdump knows PE (pei-x86-64): asked once
if rd objdump && run_rd objdump -i 2>/dev/null | tr -d '\r' | grep -q 'pei-x86-64'; then OBJDUMP_PE=1; fi
readers_pe() {
  local img="$1" n=0
  if rd llvm-readobj; then check "$img" llvm-readobj run_rd llvm-readobj --all "$img"; n=$((n+1)); else note_missing llvm-readobj; fi
  if rd llvm-objdump; then check "$img" llvm-objdump run_rd llvm-objdump -d "$img"; n=$((n+1)); else note_missing llvm-objdump; fi
  if [ -n "$DUMPBIN" ]; then
    # dash options: Git Bash rewrites a /word argument into a path
    check "$img" dumpbin "$DUMPBIN" -nologo -headers -imports "$(cygpath -w "$img" 2>/dev/null || echo "$img")"; n=$((n+1))
  else note_missing dumpbin; fi
  if [ -n "$OBJDUMP_PE" ]; then
    check "$img" objdump run_rd objdump -x "$img"; n=$((n+1))
  else note_missing "objdump(pei-x86-64)"; fi
  check "$img" image-lint "$PY" scripts/image-lint.py "$img" --quiet; n=$((n+1))
  return $n
}
readers_macho() {
  local img="$1" n=0
  if rd llvm-readobj; then check "$img" llvm-readobj run_rd llvm-readobj --all "$img"; n=$((n+1)); else note_missing llvm-readobj; fi
  if rd llvm-objdump; then check "$img" llvm-objdump run_rd llvm-objdump -d "$img"; n=$((n+1)); else note_missing llvm-objdump; fi
  if have otool; then check "$img" otool otool -hlv "$img"; n=$((n+1)); else note_missing otool; fi
  check "$img" macho-check "$PY" scripts/macho-check.py "$img" --quiet; n=$((n+1))
  return $n
}

if [ "${READERS_ONLY:-}" = 1 ]; then
  for t in readelf llvm-readobj llvm-objdump llvm-dwarfdump; do
    if have "$t"; then echo "$t: PATH"; elif rd "$t"; then echo "$t: WSL $WSL_D"; else echo "$t: missing"; fi
  done
  echo "dumpbin: ${DUMPBIN:-missing}"
  echo "objdump (pei-x86-64): $([ -n "$OBJDUMP_PE" ] && echo yes || echo no)"
  echo "otool: $(have otool && echo PATH || echo missing)"
  echo "own: scripts/image-lint.py, scripts/macho-check.py, tests/uefi/check_efi.py ($PY)"
  exit 0
fi

# build <format> <name> <emit args...>: one image; then its readers
# (coff: `fid codegen <f> --coff <out>`, the machine layer's object writer
# nat_emit_object; the others `fid emit ... -o <out>`)
build() {
  local fmt="$1" name="$2"; shift 2
  local img="$OUT/$name"
  rm -f "$img"
  local cmd=("$FID" emit "$@" -o "$img")
  [ "$fmt" = coff ] && cmd=("$FID" codegen "$@" --coff "$img")
  if ! timeout 600 "${cmd[@]}" > "$img.build.log" 2>&1 || [ ! -s "$img" ]; then
    echo "  BUILD $name (see $img.build.log)"
    bad=$((bad + 1))
    return
  fi
  "readers_$fmt" "$img"
  local n=$?
  if [ $n = 0 ]; then echo "  unchecked $name: no $fmt reader installed"; fi
}

for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  echo "$b"
  for fmt in $FORMATS; do
    case "$fmt" in
      elf)
        for t in x86_64-linux aarch64-linux riscv64-linux; do build elf "$b.$t" "$f" --target "$t"; done ;;
      pe)
        if [ "$HOST" = windows ]; then build pe "$b.peir.exe" "$f"; fi
        build pe "$b.nat.exe" "$f" --target x86_64-windows --nat ;;
      macho)
        for t in aarch64-macos x86_64-macos; do build macho "$b.$t" "$f" --target "$t"; done ;;
      obj)
        for t in x86_64-linux aarch64-linux; do build obj "$b.$t.o" "$f" --target "$t" --obj; done ;;
      coff)
        build coff "$b.coff.obj" "$f" ;;
    esac
  done
done
# The UEFI application: a bare program of its own (no stdlib), once
case " $FORMATS " in
  *" efi "*)
    if [ -f tests/bootstrap/test_uefi_hello.fd ]; then
      echo "test_uefi_hello"
      # `emit` names a PE output .exe on Windows: ask for that name
      build efi "test_uefi_hello.efi.exe" tests/bootstrap/test_uefi_hello.fd --uefi
    fi ;;
esac
[ -n "$missing" ] && echo "readers not installed:$missing"
echo "image-check: $ok checks passed, $bad failed"
exit $bad
