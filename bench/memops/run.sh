#!/usr/bin/env bash
# The runtime's memory routines against byte loops (competitive roadmap
# OP23), on every backend this machine can build and run: bench/memops/
# memops.fd built through C, the default native backend, `--nat` on Windows
# and the machine layer's Linux targets (in WSL / under qemu-user).
#
#   bash bench/memops/run.sh                 # FID=release/fid.exe
#   FID=path/to/fid bash bench/memops/run.sh
#   TARGETS="c native" bash bench/memops/run.sh
#
# Each backend prints `<case> <runtime us> <byte loop us> <x faster>` per
# case and `check ok` last; a backend whose last line is not `check ok`
# computed a wrong result. Times are wall clock on a busy machine: compare
# the ratio column, best of a few runs.
set -u
cd "$(dirname "$0")/../.."
FID="${FID:-release/fid.exe}"
OUT="${OUT:-bench/out/memops}"
mkdir -p "$OUT"
SRC=bench/memops/memops.fd
X=""
WIN=0
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe"; WIN=1 ;;
esac
DEFAULT="c native"
[ "$WIN" = 1 ] && DEFAULT="c native nat"
DEFAULT="$DEFAULT x86_64-linux aarch64-linux riscv64-linux"
TARGETS="${TARGETS:-$DEFAULT}"
export FID_HOME="${FID_HOME:-$(pwd)}"

# run a Linux binary: natively on Linux x86-64, else in WSL; qemu-user for
# the other architectures
run_linux() {
  local exe="$1" qemu="$2"
  if [ "$WIN" = 1 ]; then
    local w
    w="$(cygpath -w "$exe" 2>/dev/null || echo "$exe")"
    MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} sh -c "p=\$(wslpath -a '$w'); chmod +x \"\$p\"; $qemu \"\$p\""
  else
    chmod +x "$exe"
    $qemu "$exe"
  fi
}

status=0
for t in $TARGETS; do
  exe="$OUT/memops-$t"
  echo "== $t"
  case "$t" in
    c)      "$FID" ccompile "$SRC" -o "$exe$X" >"$exe.log" 2>&1 && "./$exe$X" ;;
    native) "$FID" emit "$SRC" -o "$exe$X" >"$exe.log" 2>&1 && "./$exe$X" ;;
    nat)    "$FID" emit "$SRC" --nat -o "$exe$X" >"$exe.log" 2>&1 && "./$exe$X" ;;
    x86_64-linux)  "$FID" emit "$SRC" --target x86_64-linux -o "$exe" >"$exe.log" 2>&1 && run_linux "$exe" "" ;;
    aarch64-linux) "$FID" emit "$SRC" --target aarch64-linux -o "$exe" >"$exe.log" 2>&1 && run_linux "$exe" qemu-aarch64 ;;
    riscv64-linux) "$FID" emit "$SRC" --target riscv64-linux -o "$exe" >"$exe.log" 2>&1 && run_linux "$exe" qemu-riscv64 ;;
    *) echo "unknown target $t"; false ;;
  esac
  rc=$?
  if [ $rc -ne 0 ]; then
    echo "($t failed with $rc; build log: $exe.log)"
    status=1
  fi
done
exit $status
