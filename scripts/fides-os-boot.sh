#!/usr/bin/env bash
# fides-os-boot.sh — fides-os as a standing test of bare-metal work
# (competitive-roadmap AD3, release G4).
#
# Builds the fides-os kernels (github.com/pledgeandgrow/fides-os) with the
# fid under test — `fid emit kernel/<k>.fd --boot --load 0x1000000`, a
# Multiboot kernel with the compiler's long-mode boot code — boots each with
# `qemu-system-x86_64 -kernel` and checks what it prints on COM1:
#
#   minimal      exactly "Hello from Fides OS!" (one line)
#   shell        the shell's banner "Fides OS v0.1", the end of its command
#                list and the prompt it reaches after running every init
#                function and its start-up tests
#   shell-input  the same kernel, typed at through the PS/2 keyboard (QEMU
#                monitor `sendkey`): `echo hi` must print " hi" and `mem`
#                the memory map and a timer tick count above 0, so the
#                keyboard and timer interrupts and the command loop run
#
# This is fides-os's own scripts/qemu-boot-test.sh run from the compiler's
# side, so a compiler change that breaks the kernel shows up here (CI job
# `fides-os` in .github/workflows/bare-metal.yml) and not later in the OS's
# repository. It writes nothing into the fides-os tree.
#
# Usage: scripts/fides-os-boot.sh [minimal|shell|shell-input]...
#   (default: all three; shell and shell-input share one build of shell.fd)
#   FID=<fid>        compiler (default release/fid(.exe), then release/fid-dev)
#   FIDES_OS=<dir>   fides-os checkout (default ../fides-os)
#   OUT=<dir>        kernels and logs (default build/fides-os)
#   QEMU=<exe>       default qemu-system-x86_64; on Windows it runs in WSL
#                    (WSL_DISTRO picks the distribution) when not on PATH
#   TIMEOUT=<s>      seconds a kernel gets to print its line (default 60)
#   BUILD=0          boot the kernels already in OUT; RUN=0: build only
#   SELFTEST=1       check the serial-output matcher on built-in samples
# Exit status: the number of kernels that failed (2: setup error).
set -u
cd "$(dirname "$0")/.." || exit 2
ROOT="$PWD"

abs() { case "$1" in /*|[A-Za-z]:*) echo "$1" ;; *) echo "$ROOT/$1" ;; esac; }

# expected COM1 output, one check a line, every one must hold (CR dropped):
#   exact:<text>  the whole log (one trailing newline allowed)
#   has:<text>    text somewhere in the log
#   line:<text>   a line of the log is exactly <text>
#   re:<regex>    a line matches the extended regular expression
#   ends:<text>   the log ends with <text> (trailing newlines dropped)
expect_of() {
  case "$1" in
    minimal) printf '%s\n' "exact:Hello from Fides OS!" ;;
    shell)   printf '%s\n' "has:Fides OS v0.1" "line:  cat     - read file" "ends:> " ;;
    shell-input)
             printf '%s\n' "has:Fides OS v0.1" "line:> echo hi" "line: hi" \
               "line:Memory: kernel at 0x100000" "re:^Timer ticks: [1-9][0-9]*$" "ends:> " ;;
    *)       echo "" ;;
  esac
}

# the kernel source a case builds, and the keys it types (QEMU sendkey names)
src_of() { case "$1" in shell-input) echo shell ;; *) echo "$1" ;; esac; }
keys_of() {
  case "$1" in
    shell-input) echo "e c h o spc h i ret m e m ret" ;;
    *)           echo "" ;;
  esac
}

# serial_ok <expect> <log file> -> 0 if the log matches; the first check
# that fails is in $SERIAL_MISS
SERIAL_MISS=""
serial_ok() {
  local e="$1" f="$2" got one
  SERIAL_MISS="no log"
  [ -f "$f" ] || return 1
  got=$(tr -d '\r' < "$f")
  while IFS= read -r one; do
    [ -n "$one" ] || continue
    SERIAL_MISS="$one"
    case "$one" in
      exact:*) [ "$got" = "${one#exact:}" ] || return 1 ;;
      has:*)   case "$got" in *"${one#has:}"*) ;; *) return 1 ;; esac ;;
      line:*)  printf '%s\n' "$got" | grep -qaxF -- "${one#line:}" || return 1 ;;
      re:*)    printf '%s\n' "$got" | grep -qaE -- "${one#re:}" || return 1 ;;
      ends:*)  case "$got" in *"${one#ends:}") ;; *) return 1 ;; esac ;;
      *)       return 1 ;;
    esac
  done <<< "$e"
  SERIAL_MISS=""
  return 0
}

if [ "${SELFTEST:-0}" = 1 ]; then
  t=$(mktemp -d); bad=0
  printf 'Hello from Fides OS!\r\n' > "$t/a"; serial_ok "exact:Hello from Fides OS!" "$t/a" || { echo "selftest: exact with CRLF"; bad=1; }
  printf 'Hello from Fides OS!\nmore\n' > "$t/b"; serial_ok "exact:Hello from Fides OS!" "$t/b" && { echo "selftest: exact accepted extra output"; bad=1; }
  printf 'boot\nFides OS v0.1 (x86-64)\n> ' > "$t/c"; serial_ok "has:Fides OS v0.1" "$t/c" || { echo "selftest: has"; bad=1; }
  printf 'Fides OS v0.0\n' > "$t/d"; serial_ok "has:Fides OS v0.1" "$t/d" && { echo "selftest: has accepted a wrong banner"; bad=1; }
  serial_ok "has:x" "$t/none" && { echo "selftest: missing log accepted"; bad=1; }
  sh_ok=$(expect_of shell); in_ok=$(expect_of shell-input)
  printf 'Fides OS v0.1\r\n\r\nCommands:\r\n  cat     - read file\r\n\r\nFailed to load user program\r\n> ' > "$t/e"
  serial_ok "$sh_ok" "$t/e" || { echo "selftest: shell rejected a good boot ($SERIAL_MISS)"; bad=1; }
  printf 'Fides OS v0.1\n\nCommands:\n  cat     - read file\n\nLoading user program from sector 1...\n' > "$t/f"
  serial_ok "$sh_ok" "$t/f" && { echo "selftest: shell accepted a boot that never reached the prompt"; bad=1; }
  [ "$SERIAL_MISS" = "ends:> " ] || { echo "selftest: wrong miss reported: $SERIAL_MISS"; bad=1; }
  printf 'Fides OS v0.1\n> echo hi\n hi\n> mem\n\nMemory: kernel at 0x100000\nTimer ticks: 161\n> ' > "$t/g"
  serial_ok "$in_ok" "$t/g" || { echo "selftest: shell-input rejected a good session ($SERIAL_MISS)"; bad=1; }
  printf 'Fides OS v0.1\n> echo hi\n hi\n> mem\n\nMemory: kernel at 0x100000\nTimer ticks: 0\n> ' > "$t/h"
  serial_ok "$in_ok" "$t/h" && { echo "selftest: shell-input accepted a timer that never ticked"; bad=1; }
  printf 'Fides OS v0.1\n> echo hi\n> mem\n\nMemory: kernel at 0x100000\nTimer ticks: 7\n> ' > "$t/i"
  serial_ok "$in_ok" "$t/i" && { echo "selftest: shell-input accepted an echo that printed nothing"; bad=1; }
  [ "$(src_of shell-input)" = shell ] && [ "$(src_of minimal)" = minimal ] && [ -n "$(keys_of shell-input)" ] && [ -z "$(keys_of shell)" ] \
    || { echo "selftest: src_of/keys_of"; bad=1; }
  rm -rf "$t"
  [ $bad = 0 ] && echo "fides-os-boot selftest: ok"
  exit $bad
fi

case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows; X=.exe ;; *) HOST=other; X= ;; esac
FID="${FID:-}"
if [ -z "$FID" ]; then
  for f in release/fid$X release/fid-dev$X; do [ -x "$f" ] && { FID="$f"; break; }; done
fi
[ -n "$FID" ] || { echo "error: no fid (set FID)"; exit 2; }
FID=$(abs "$FID")
FIDES_OS=$(abs "${FIDES_OS:-../fides-os}")
OUT=$(abs "${OUT:-build/fides-os}")
TIMEOUT="${TIMEOUT:-60}"
BUILD="${BUILD:-1}"
RUN="${RUN:-1}"
LOAD="${LOAD:-0x1000000}"
QEMU="${QEMU:-qemu-system-x86_64}"
[ -f "$FIDES_OS/kernel/minimal.fd" ] || { echo "error: no fides-os checkout at $FIDES_OS (set FIDES_OS)"; exit 2; }
mkdir -p "$OUT"

# The boot runs as one sh script, natively or inside WSL, so the poll and
# the kill of QEMU happen on the side where QEMU runs. With keys ($5), QEMU's
# monitor reads from a FIFO: once the shell shows its prompt the keys go in
# as `sendkey`s, then `quit` once the output has settled.
BOOT_SH='bin=$1; log=$2; t=$3; q=$4; keys=$5; rm -f "$log"; p=
# settled: up to t s, until there is output and it has not grown for 2 s
settled() {
  i=0; last=-1; still=0
  while [ $i -lt $((t * 2)) ]; do
    kill -0 $p 2>/dev/null || return 1
    n=0; [ -f "$log" ] && n=$(wc -c < "$log")
    if [ "$n" -gt 0 ] && [ "$n" = "$last" ]; then still=$((still + 1)); [ $still -ge 4 ] && return 0; else still=0; fi
    last=$n; sleep 0.5; i=$((i + 1))
  done
  return 1
}
# prompt: up to t s, until the log ends with the shell prompt "> "
prompt() {
  i=0
  while [ $i -lt $((t * 2)) ]; do
    kill -0 $p 2>/dev/null || return 1
    [ -f "$log" ] && [ "$(tail -c 2 "$log")" = "> " ] && return 0
    sleep 0.5; i=$((i + 1))
  done
  return 1
}
set -- -kernel "$bin" -m 256 -display none -no-reboot -serial "file:$log" \
  -device isa-debug-exit,iobase=0xf4,iosize=0x01
if [ -z "$keys" ]; then
  "$q" "$@" -monitor none < /dev/null > "$log.qemu" 2>&1 &
  p=$!
  settled
else
  fifo=$(mktemp -u); mkfifo "$fifo" || exit 1
  "$q" "$@" -monitor stdio < "$fifo" > "$log.qemu" 2>&1 &
  p=$!
  {
    if prompt; then
      for k in $keys; do echo "sendkey $k"; sleep 0.2; done
      settled
    fi
    echo quit
  } > "$fifo"
  j=0; while kill -0 $p 2>/dev/null && [ $j -lt 20 ]; do sleep 0.5; j=$((j + 1)); done
  rm -f "$fifo"
fi
kill $p 2>/dev/null; wait $p 2>/dev/null; exit 0'

QPRE=()
wslp() { local p; p=$(cygpath -m "$1"); local d; d=$(printf '%s' "${p:0:1}" | tr 'A-Z' 'a-z'); echo "/mnt/$d${p:2}"; }
if [ "$HOST" = windows ] && ! command -v "$QEMU" > /dev/null 2>&1; then
  command -v wsl > /dev/null 2>&1 || { echo "error: no $QEMU and no WSL"; exit 2; }
  QPRE=(wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e)
fi
hostp() { if [ ${#QPRE[@]} -gt 0 ]; then wslp "$1"; else echo "$1"; fi; }

KERNELS=("$@")
[ ${#KERNELS[@]} = 0 ] && KERNELS=(minimal shell shell-input)
echo "fid:      $FID"
echo "fides-os: $FIDES_OS ($(git -C "$FIDES_OS" log -1 --format='%h %cs' 2>/dev/null || echo 'not a git checkout'))"
fail=0
built=" "; badsrc=" "
for k in "${KERNELS[@]}"; do
  want=$(expect_of "$k")
  [ -n "$want" ] || { echo "FAIL $k: no expected output known for it"; fail=$((fail + 1)); continue; }
  src=$(src_of "$k")
  bin="$OUT/$src.bin"
  case "$badsrc" in *" $src "*) echo "FAIL $k: build of kernel/$src.fd failed"; fail=$((fail + 1)); continue ;; esac
  if [ "$BUILD" = 1 ] && case "$built" in *" $src "*) false ;; *) true ;; esac; then
    rm -f "$bin"
    t0=$(date +%s)
    if ! (cd "$FIDES_OS" && "$FID" emit "kernel/$src.fd" --boot --load "$LOAD" -o "$bin") > "$OUT/$src.build.log" 2>&1 || [ ! -s "$bin" ]; then
      echo "FAIL $k: build"; grep -v '^\[' "$OUT/$src.build.log" | head -20 | sed 's/^/    /'
      badsrc="$badsrc$src "; fail=$((fail + 1)); continue
    fi
    built="$built$src "
    echo "built    $src: $(wc -c < "$bin") bytes in $(( $(date +%s) - t0 )) s"
  fi
  [ "$RUN" = 1 ] || continue
  log="$OUT/$k.serial.log"
  rm -f "$log"
  MSYS_NO_PATHCONV=1 "${QPRE[@]}" sh -c "$BOOT_SH" sh "$(hostp "$bin")" "$(hostp "$log")" "$TIMEOUT" "$QEMU" "$(keys_of "$k")"
  if serial_ok "$want" "$log"; then
    echo "ok       $k: COM1 has every expected line ($(printf '%s\n' "$want" | grep -c .) checks)"
  else
    echo "FAIL $k: COM1 does not match \"$SERIAL_MISS\" within ${TIMEOUT}s"
    echo "    --- COM1 ---"; head -c 2000 "$log" 2>/dev/null | tr -d '\r' | sed 's/^/    /'
    sed 's/^/    qemu: /' "$log.qemu" 2>/dev/null | head -5
    fail=$((fail + 1))
  fi
done
echo "fides-os-boot: $(( ${#KERNELS[@]} - fail ))/${#KERNELS[@]} kernels ok"
exit $fail
