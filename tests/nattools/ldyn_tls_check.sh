#!/usr/bin/env bash
# 2g.99 (docs/reference/known-bugs.md; RL25, CI2 in
# docs/roadmap/llvm-roadmap.md): a dynamically linked x86_64-linux image
# of the machine layer must keep the C library's thread pointer. Before,
# the runtime moved fs to its own thread block (arch_prctl ARCH_SET_FS) and
# libc's thread-local locale was read from the block's neighbours: strtod
# faulted at 0x8 in test_verify_abi_c_mixed. Now the block is the word at
# fs:[-8] (compiler/nat_ir.fd nat_tls_get / nat_tls_set): the executable's
# 8-byte PT_TLS block in a dynamically linked image (nat_enc.fd
# nat_elf_img), fs = block + 72 in a static one and on the runtime's own
# threads (nat_clone_thread). Run after the compiler is built:
#   bash tests/nattools/ldyn_tls_check.sh [fid]   (default release/fid.exe)
# 1. test_verify_abi_c_mixed.fd and test_verify_abi_c_tls.fd (C functions
#    that read libc's thread-local state), `emit --target x86_64-linux`:
#    dynamically linked (PT_INTERP), with a PT_TLS segment, and printing
#    the file's expected output.
# 2. Static images (no C function): no PT_TLS; programs with threads and
#    tasks print what `fid run` prints.
# Linux images run directly on Linux, through WSL on Windows (`wsl.exe -e`
# from the image's folder); readelf runs where the images run. Without a
# way to run them, the images are built only. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-ldyn-tls"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
RUN=""
CAN=1
if [ "$HOST" = windows ]; then
    if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; else CAN=0; fi
fi

# segs <exe>: the program header types readelf lists (empty without readelf)
segs() { (cd "$OUT" && $RUN sh -c "readelf -lW ./$1 2>/dev/null" | tr -d '\r' | awk '/^ *[A-Z_]+ +0x/ {print $1}' | tr '\n' ' '); }

for src in tests/bootstrap/test_verify_abi_c_mixed.fd tests/bootstrap/test_verify_abi_c_tls.fd; do
    base=$(basename "$src" .fd)
    if ! "$FID" emit "$src" --target x86_64-linux -o "$OUT/$base" >"$OUT/$base.log" 2>&1; then
        bad "$base: build failed: $(grep -v '^\[' "$OUT/$base.log" | tail -2 | tr '\n' ' ')"
        continue
    fi
    [ $CAN = 1 ] || { ok "$base: built (not run on this host)"; continue; }
    s=$(segs "$base")
    if [ -n "$s" ]; then
        case " $s " in *" INTERP "*" TLS "*|*" TLS "*" INTERP "*) ok "$base: dynamically linked with PT_TLS ($s)" ;;
            *) bad "$base: want PT_INTERP and PT_TLS, have: $s" ;; esac
    fi
    got=$(cd "$OUT" && $RUN "./$base" 2>&1 | tr -d '\r')
    if [ "$got" = "$(expected "$src")" ]; then
        ok "$base: output as expected"
    else
        bad "$base: output differs"
        diff <(expected "$src") <(echo "$got") | head -8
    fi
done

for src in tests/bootstrap/hello.fd tests/bootstrap/test_async_threads.fd tests/bootstrap/test_alloc_threads_stress.fd tests/bootstrap/test_machine_tasks_once.fd tests/bootstrap/test_spawn_scope.fd; do
    [ -f "$src" ] || continue
    base=$(basename "$src" .fd)
    if ! "$FID" emit "$src" --target x86_64-linux -o "$OUT/$base" >"$OUT/$base.log" 2>&1; then
        bad "$base: build failed: $(grep -v '^\[' "$OUT/$base.log" | tail -2 | tr '\n' ' ')"
        continue
    fi
    [ $CAN = 1 ] || { ok "$base: built (not run on this host)"; continue; }
    s=$(segs "$base")
    case " $s " in *" TLS "*) bad "$base: a static image with PT_TLS ($s)" ;; *) ok "$base: static, no PT_TLS" ;; esac
    want=$("$FID" run "$src" 2>&1)
    wrc=$?
    want=$(echo "$want" | tr -d '\r')
    got=$(cd "$OUT" && $RUN "./$base" 2>&1)
    rc=$?
    got=$(echo "$got" | tr -d '\r')
    if [ "$got" = "$want" ] && [ $rc -eq $wrc ]; then
        ok "$base: runs as fid run"
    else
        bad "$base: output or status differs from fid run ($rc vs $wrc)"
        diff <(echo "$want") <(echo "$got") | head -8
    fi
done

echo "ldyn_tls_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
