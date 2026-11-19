#!/usr/bin/env bash
# `fid emit --checked-alloc --target x86_64-linux` (release M7, complaint
# CM9, RL16): the programs of this folder through the machine layer's Linux
# runtime (stdlib/linux_runtime/base.fd pe_ck_fault), checked against the
# same `// expect-checked:` / `// expect-output:` lines as run.sh (native
# Windows, pe_ir). The report's last part differs: pe_ir names the image
# offset of the allocating call, the Linux runtime its return address and
# function ("allocated by the call returning to 0x... in main"), then the
# first two program functions on the stack ("; allocated in f <- main"),
# which `// expect-linux: <text>` lines check (run.sh ignores them).
#
#   bash tests/checked_alloc/run-linux.sh
#
# Runs the executables natively on Linux x86-64, else through WSL
# (WSL_DISTRO=<name> to pick one). FID: the compiler (default
# release/fid.exe). Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-checked-alloc-linux}"
mkdir -p "$OUT"
pass=0
fail=0

run_linux() {
    if [ "$(uname -s)" = "Linux" ] && [ "$(uname -m)" = "x86_64" ]; then
        chmod +x "$1"; "$1"
    else
        local w
        w=$(wslpath -a "$1" 2>/dev/null || echo "$1" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#; s#\\#/#g; s#^/([a-z])/#/mnt/\1/#')
        if [ -n "${WSL_DISTRO:-}" ]; then
            MSYS_NO_PATHCONV=1 wsl.exe -d "$WSL_DISTRO" -- sh -c "chmod +x '$w' && '$w'"
        else
            MSYS_NO_PATHCONV=1 wsl.exe -- sh -c "chmod +x '$w' && '$w'"
        fi
    fi
}

for src in tests/checked_alloc/*.fd; do
    name=$(basename "$src" .fd)
    exe="$OUT/$name.out"
    if ! "$FID" emit "$src" --target x86_64-linux --checked-alloc -o "$exe" > "$OUT/$name.build" 2>&1; then
        echo "FAIL $name: does not build"; tail -5 "$OUT/$name.build"; fail=$((fail + 1)); continue
    fi
    run_linux "$exe" > "$OUT/$name.txt" 2>&1
    ok=1
    want_fault=0
    while IFS= read -r line; do
        case "$line" in
            "// expect-checked: "*)
                want_fault=1
                text="${line#// expect-checked: }"
                grep -qF -- "$text" "$OUT/$name.txt" || { echo "FAIL $name: missing: $text"; ok=0; } ;;
            "// expect-linux: "*)
                text="${line#// expect-linux: }"
                grep -qF -- "$text" "$OUT/$name.txt" || { echo "FAIL $name: missing: $text"; ok=0; } ;;
            "// expect-output: "*)
                text="${line#// expect-output: }"
                grep -qxF -- "$text" "$OUT/$name.txt" || { echo "FAIL $name: missing line: $text"; ok=0; } ;;
        esac
    done < "$src"
    # (the exit status is not compared: under WSL 1 a program that panics
    # from its signal handler can still exit 0)
    if [ $want_fault = 0 ] && grep -q "^Panic:" "$OUT/$name.txt"; then echo "FAIL $name: panicked"; ok=0; fi
    if [ $ok = 1 ]; then pass=$((pass + 1)); echo "ok   $name"; else fail=$((fail + 1)); sed 's/^/     | /' "$OUT/$name.txt"; fi
done

echo "checked-alloc (x86_64-linux): $pass passed, $fail failed"
[ $fail = 0 ]
