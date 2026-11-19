#!/bin/bash
# tier2-targets-check.sh — PL3: the Tier 2 targets of `fid ccompile
# --target` (compiler/main.fd normalize_target, cross_target_os,
# cross_zig_triple) and the C runtime's platform branches
# (compiler/codegen_c.fd: FreeBSD's headers, Windows ARM64's unwinder).
#
# For each Tier 2 target, in both spellings (`aarch64-windows` and the
# OS-first `windows-arm64`, ...), builds a few programs through C with zig
# cc and checks each binary is an executable of that target
# (scripts/bin-target-check.py: PE machine ARM64, ELF tagged FreeBSD,
# Mach-O x86-64, ELF RISC-V). The riscv64-linux binaries also run, under
# qemu-riscv64 (in WSL on Windows), and must print what `fid run` prints.
# Without the targets, `--target x86_64-freebsd` is "Unknown --target";
# without the FreeBSD branches the C does not compile (<alloca.h>,
# malloc_usable_size); without the ARM64 unwinder a Windows ARM64 build
# fails on CONTEXT.Rip.
#
# Usage: FID=<fid> bash tests/tooling/tier2-targets-check.sh
#   (needs zig on PATH and python3; RUN=0 skips the riscv64 runs;
#   PROGS="a.fd ..." other programs; SPELLINGS="x86_64-freebsd ..." only
#   these spellings)
# Exit 0 = every target built every program and every check passed.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
[ -x "$FID" ] || { echo "tier2-targets-check: no compiler at $FID (set FID)"; exit 1; }
command -v zig > /dev/null 2>&1 || { echo "tier2-targets-check: zig is not on PATH"; exit 1; }
export CC="zig cc"
PY=""
for p in python3 python; do command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null && { PY=$p; break; }; done
[ -n "$PY" ] || { echo "tier2-targets-check: no python3"; exit 1; }
W="build/tier2-targets-check"
rm -rf "$W"   # this script's own output of an earlier run
mkdir -p "$W"
bad=0
# hello: the runtime's basics; test_async: tasks (the context switch, the
# kqueue/epoll wait, fibers on Windows ARM64); test_int_width: arithmetic
PROGS="${PROGS:-tests/bootstrap/hello.fd tests/bootstrap/test_async.fd tests/bootstrap/test_int_width.fd}"
# PL3: the platform-dependent tests, built for each target's canonical
# spelling only: one @cfg variant per target (compiler.fd check_cfg knows
# freebsd, unix, wasi), qsort_s/qsort_r by system. Without check_cfg's
# freebsd, the FreeBSD variants are second definitions on every target.
CFG_PROGS="${CFG_PROGS-tests/bootstrap/test_cfg.fd tests/bootstrap/test_cfg_unix.fd tests/bootstrap/test_cfg_platforms.fd tests/bootstrap/test_c_callback_user_data.fd}"
for p in $PROGS $CFG_PROGS; do [ -f "$p" ] || { echo "missing $p"; exit 1; }; done

# spelling -> the canonical target bin-target-check.py reads
for pair in aarch64-windows:aarch64-windows windows-arm64:aarch64-windows \
            x86_64-macos:x86_64-macos macos-x64:x86_64-macos \
            x86_64-freebsd:x86_64-freebsd freebsd-x64:x86_64-freebsd \
            riscv64-linux:riscv64-linux linux-riscv64:riscv64-linux; do
  spell="${pair%%:*}"; canon="${pair#*:}"
  if [ -n "${SPELLINGS:-}" ]; then
    case " $SPELLINGS " in *" $spell "*) ;; *) continue ;; esac
  fi
  progs="$PROGS"
  if [ "$spell" = "$canon" ]; then progs="$PROGS $CFG_PROGS"; fi
  for p in $progs; do
    b=$(basename "$p" .fd)
    ext=""
    case "$canon" in *-windows) ext=".exe" ;; esac
    out="$W/$b-$spell$ext"
    rm -f "$out"
    if ! timeout 900 "$FID" ccompile "$p" --target "$spell" --cc-opt 0 --no-cache -o "$W/$b-$spell" > "$W/$b-$spell.log" 2>&1 || [ ! -f "$out" ]; then
      echo "FAIL: $p --target $spell did not build ($W/$b-$spell.log):"
      grep -a -m5 -i 'error\|unknown' "$W/$b-$spell.log" | sed 's/^/    /'
      bad=$((bad + 1)); continue
    fi
    if ! "$PY" scripts/bin-target-check.py "$canon" "$out" --quiet > "$W/$b-$spell.check" 2>&1; then
      echo "FAIL: $out is not a $canon executable:"; sed 's/^/    /' "$W/$b-$spell.check"
      bad=$((bad + 1)); continue
    fi
    echo "ok: $p --target $spell"
  done
done

# The riscv64 binaries run: same output and exit code as `fid run`
if [ "${RUN:-1}" != 0 ]; then
  run_rv() {  # binary -> runs it under qemu-riscv64, stdout to $2
    if [ "$HOST" = windows ]; then
      local wp
      wp=$(cygpath -m "$(pwd)/$1" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
      MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e timeout 60 qemu-riscv64 "$wp" < /dev/null > "$2" 2> /dev/null
    elif [ "$(uname -m)" = riscv64 ]; then
      timeout 60 "./$1" < /dev/null > "$2" 2> /dev/null
    else
      timeout 60 qemu-riscv64 "./$1" < /dev/null > "$2" 2> /dev/null
    fi
  }
  for p in $PROGS $CFG_PROGS; do
    b=$(basename "$p" .fd)
    bin="$W/$b-riscv64-linux"
    [ -f "$bin" ] || bin="$W/$b-linux-riscv64"
    [ -f "$bin" ] || continue
    case " $CFG_PROGS " in
      *" $p "*)
        # the test's own "// Expected output:" block (the interpreter has
        # no qsort_s/qsort_r for test_c_callback_user_data)
        awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$p" > "$W/$b.expected"
        erc=0 ;;
      *)
        timeout 120 "$FID" run "$p" < /dev/null > "$W/$b.expected" 2> /dev/null
        erc=$? ;;
    esac
    run_rv "$bin" "$W/$b.rv.actual"
    arc=$?
    if [ "$arc" = 127 ]; then
      echo "FAIL: qemu-riscv64 is not installed$([ "$HOST" = windows ] && echo ' in WSL') (apt install qemu-user; RUN=0 skips the runs)"
      bad=$((bad + 1)); break
    fi
    if [ "$arc" = "$erc" ] && cmp -s <(tr -d '\r' < "$W/$b.expected") <(tr -d '\r' < "$W/$b.rv.actual"); then
      echo "ok: $b runs on riscv64-linux as under fid run"
    else
      echo "FAIL: $b on riscv64-linux: rc $arc (fid run $erc), output differs:"
      diff <(tr -d '\r' < "$W/$b.expected") <(tr -d '\r' < "$W/$b.rv.actual") | head -10 | sed 's/^/    /'
      bad=$((bad + 1))
    fi
  done
fi

if [ "$bad" = 0 ]; then echo "tier2-targets-check: ok"; else echo "tier2-targets-check: $bad failed"; fi
exit "$bad"
