#!/usr/bin/env bash
# Per-module backend matrix for the standard library (complaint CD2:
# "each module tested on every backend").
#
# For every test program given (default: tests/bootstrap/test_lib_*.fd and
# the stdlib tests named below), runs it three ways and compares standard
# output (CR stripped) with the interpreter's:
#   run    fid run <file>
#   c      fid ccompile <file> (CC, default "zig cc"), then the executable
#   native fid emit <file> (pe_ir on Windows, nat_ir with TARGET=<triple>)
# A program whose header lists "// Expected output:" lines is also checked
# against them. Prints one line per program and a summary; exits 1 when any
# backend differs or fails to build.
#
#   FID=release/fid.exe bash tests/stdlib_backends.sh
#   FID=release/fid.exe bash tests/stdlib_backends.sh tests/bootstrap/test_lib_banking_batch.fd
#   SKIP=native bash tests/stdlib_backends.sh      # leave a backend out (run, c, native)
#   TARGET=aarch64-linux SKIP=c bash tests/stdlib_backends.sh
#       native through nat_ir for a Linux target; the executables run under
#       qemu-user (qemu-aarch64, qemu-riscv64; x86_64 directly), in WSL on
#       a Windows host (WSL_DISTRO picks the distribution)
#   LIST=sl bash tests/stdlib_backends.sh          # only the SL-track depth tests
#
# One fid process at a time (several at once slow every one of them down).
# A build stops after BUILD_TIMEOUT seconds (600), a program after
# RUN_TIMEOUT (120): a hang is reported as "exit"/"DIFF", not waited on.
set -u
FID="${FID:-release/fid.exe}"
CC="${CC:-zig cc}"
SKIP="${SKIP:-}"
TARGET="${TARGET:-}"
LIST="${LIST:-all}"
BUILD_TIMEOUT="${BUILD_TIMEOUT:-600}"
RUN_TIMEOUT="${RUN_TIMEOUT:-120}"
OUT="${OUT:-${TMPDIR:-${TEMP:-/tmp}}/fides_stdlib_backends${TARGET:+-$TARGET}}"
mkdir -p "$OUT"

# The competitive SL track's areas (HTTP, JSON, TLS, regex, time, CSV,
# compression, encodings): one program per feature, each on every backend.
SL_TESTS="test_http test_http_health test_http2_frames test_http2_limits test_stdlib_http_binary \
 test_stdlib_http2_length test_json test_json2 test_json_swar test_stdlib_jsondoc \
 test_stdlib_jsondoc_loaders test_crypto_tls_vectors test_runtime_tls_record \
 test_stdlib_tls13_schedule test_tls_alpn test_regex test_regex_linear test_regex_captures \
 test_regex_flags test_datetime test_iso_duration test_tz test_tz_julian test_tzif test_stdlib_calendar \
 test_stdlib_calendar_lunar test_lib_time test_csv test_csv_dialects test_compress \
 test_deflate_huffman test_zstd test_brotli test_zstd_compress test_brotli_compress \
 test_encoding test_protobuf test_toml_spec \
 test_yaml_spec test_xml_spec test_xml_dtd_ns test_bigint test_decimal \
 test_http2_upgrade test_http2_tasks test_stdlib_http3 test_tls13_local"

if [ $# -gt 0 ]; then
    files=("$@")
else
    files=()
    if [ "$LIST" != sl ]; then
        files=(tests/bootstrap/test_lib_*.fd)
        for extra in test_lib_banking_batch test_process_command test_world_fakes \
                     test_blake3_md5 test_hmac_sha512 test_io_streams \
                     test_fs_handles test_trace_spans test_stdlib_table test_stdlib_test_clock; do
            f="tests/bootstrap/$extra.fd"
            [ -f "$f" ] && files+=("$f")
        done
    fi
    for extra in $SL_TESTS; do
        f="tests/bootstrap/$extra.fd"
        [ -f "$f" ] && files+=("$f")
    done
fi

# How the native executables run: here, or for a Linux target in WSL (on
# Windows) or under qemu-user.
RUNNER=""
TOS=""
case "$TARGET" in
    aarch64-linux*|arm64-linux*) RUNNER=qemu-aarch64; TOS=linux ;;
    riscv64-linux*)              RUNNER=qemu-riscv64; TOS=linux ;;
    x86_64-linux*)               RUNNER="";           TOS=linux ;;
esac
case "$(uname -s)" in
    Linux)
        case "$(uname -m)-$RUNNER" in
            aarch64-qemu-aarch64|riscv64-qemu-riscv64) RUNNER="" ;;
        esac
        IN_WSL=0 ;;
    *) IN_WSL=1 ;;
esac
[ "$TOS" = linux ] || IN_WSL=0
wsl_path() {  # C:/x/y or /c/x/y -> /mnt/c/x/y
    cygpath -m "$1" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#'
}
run_native() {  # run_native <exe> <stdout file>
    if [ "$IN_WSL" = 1 ]; then
        MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} \
            timeout "$RUN_TIMEOUT" $RUNNER "$(wsl_path "$1")" < /dev/null 2>/dev/null | tr -d '\r' > "$2"
    else
        timeout "$RUN_TIMEOUT" $RUNNER "$1" < /dev/null 2>/dev/null | tr -d '\r' > "$2"
    fi
}

expected_of() {
    # the "// Expected output:" block of a test header, comment marks removed
    # (only a line that is exactly "// Expected output:"; prose that starts
    # with the words, as "// Expected output: every line ...", is not one)
    sed -n '/^\/\/ Expected output:[[:space:]]*$/,/^[^\/]/p' "$1" | sed '1d' | grep '^//' | sed 's|^// \{0,1\}||'
}

skip() { case ",$SKIP," in *",$1,"*) return 0;; esac; return 1; }

pass=0; fail=0
printf '%-40s %-8s %-8s %-8s %s\n' "program" "run" "c" "native" "expected"
for f in "${files[@]}"; do
    case "$f" in *_fail.fd) continue;; esac
    name=$(basename "$f" .fd)
    base="$OUT/$name"
    r_run="-"; r_c="-"; r_nat="-"; r_exp="-"; bad=0

    if ! skip run; then
        if timeout "$RUN_TIMEOUT" "$FID" run "$f" < /dev/null 2>"$base.run.err" | tr -d '\r' > "$base.run.out" \
           && [ "${PIPESTATUS[0]}" = 0 ]; then
            r_run="ok"
        else
            r_run="exit"; bad=1
        fi
    fi
    # the reference: the interpreter's output, or with run skipped the
    # first backend's (printed "ref")
    ref=""
    [ "$r_run" != "-" ] && ref="$base.run.out"

    if ! skip c; then
        rm -f "$base.c.exe"
        if CC="$CC" timeout "$BUILD_TIMEOUT" "$FID" ccompile "$f" -o "$base.c.exe" >"$base.c.log" 2>&1 && [ -f "$base.c.exe" ]; then
            timeout "$RUN_TIMEOUT" "$base.c.exe" < /dev/null 2>/dev/null | tr -d '\r' > "$base.c.out"
            if [ -z "$ref" ]; then ref="$base.c.out"; r_c="ref"
            elif cmp -s "$base.c.out" "$ref"; then r_c="same"; else r_c="DIFF"; bad=1; fi
        elif grep -q "unable to find dynamic system library\|cannot find -l\|LNK1181" "$base.c.log"; then
            # a C library this machine does not have (SDL2, sqlite3): not
            # the module's fault, not counted
            r_c="NOLIB"
        else
            r_c="BUILD"; bad=1
        fi
    fi

    if ! skip native; then
        rm -f "$base.n.exe"
        tflag=()
        [ -n "$TARGET" ] && tflag=(--target "$TARGET")
        if timeout "$BUILD_TIMEOUT" "$FID" emit "$f" "${tflag[@]+"${tflag[@]}"}" -o "$base.n.exe" >"$base.n.log" 2>&1 && [ -f "$base.n.exe" ]; then
            run_native "$base.n.exe" "$base.n.out"
            if [ -z "$ref" ]; then ref="$base.n.out"; r_nat="ref"
            elif cmp -s "$base.n.out" "$ref"; then r_nat="same"; else r_nat="DIFF"; bad=1; fi
        else
            r_nat="BUILD"; bad=1
        fi
    fi

    if grep -q '^// Expected output:[[:space:]]*$' "$f"; then
        expected_of "$f" > "$base.expected"
        if [ -n "$ref" ] && cmp -s "$base.expected" "$ref"; then r_exp="same"; else r_exp="DIFF"; bad=1; fi
    fi

    printf '%-40s %-8s %-8s %-8s %s\n' "$name" "$r_run" "$r_c" "$r_nat" "$r_exp"
    if [ $bad -eq 0 ]; then pass=$((pass + 1)); else fail=$((fail + 1)); fi
done
echo "programs: $pass agree on every backend, $fail differ or fail (outputs in $OUT)"
[ $fail -eq 0 ]
