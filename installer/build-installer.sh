#!/bin/bash
# installer/build-installer.sh — Build the Fides Windows installer
#
# Requirements:
#   - NSIS 3.x installed (sudo apt install nsis on Ubuntu/Debian)
#   - The self-hosted compiler at release/fid.exe, the release build of
#     this source (scripts/release-fresh.sh; rebuilt here when missing or
#     stale: RELEASE=1 ./scripts/bootstrap.sh, which needs
#     target/release/fidesc and a C compiler; FIDES_NO_REBUILD=1 refuses)
#   - A C compiler for ark, scribe and veil, built from tools/*.fd with
#     release/fid.exe ccompile (CC, default zig cc; FIDES_RUST_TOOLS=1 ships
#     the Rust builds from target/release instead)
#   - stdlib/ and runtime/ directories present
#
# Usage:
#   ./installer/build-installer.sh
#
# Output:
#   installer/Fid-Setup-<version>.exe

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
# FIDES_VERSION overrides the default version written in installer/fides.nsi
VERSION="${FIDES_VERSION:-$(sed -n 's/^ *!define VERSION "\(.*\)"/\1/p' "$SCRIPT_DIR/fides.nsi" | head -1)}"

cd "$ROOT_DIR"
TOOLS_DIR="build/installer/tools"
mkdir -p "$TOOLS_DIR"
clean_tool_sidecars() {
    local tool="$1"
    rm -f "$TOOLS_DIR/$tool.c" "$TOOLS_DIR/$tool.exe.fides-stamp" "$TOOLS_DIR/$tool.pdb"
}

echo "=== Fides Installer Build ==="
echo ""

# Check requirements
echo "[check] NSIS..."
if ! command -v makensis &> /dev/null; then
    echo "ERROR: NSIS not found. Install with:"
    echo "  Ubuntu/Debian: sudo apt install nsis"
    echo "  macOS:         brew install nsis"
    echo "  Windows:       Download from https://nsis.sourceforge.io/"
    exit 1
fi
echo "  OK: $(makensis /VERSION)"

echo "[check] Compiler..."
# release-roadmap A10: ship only the release bootstrap's build of this
# source. scripts/release-fresh.sh checks release/fid.exe against the stamp
# RELEASE=1 scripts/bootstrap.sh writes (binary and source hashes): a
# missing, unstamped or stale binary is rebuilt (3 stages, fixpoint
# checked), and the installer is refused when the rebuild fails or the
# result is still not fresh. FIDES_NO_REBUILD=1 refuses instead of
# rebuilding (a release job that has just built it).
if [ ! -f "release/fid.exe" ] || ! bash scripts/release-fresh.sh release/fid.exe; then
    if [ "${FIDES_NO_REBUILD:-}" = 1 ]; then
        echo "ERROR: release/fid.exe is missing or not the release build of this source"
        echo "  (FIDES_NO_REBUILD=1: not rebuilding). Run: RELEASE=1 ./scripts/bootstrap.sh"
        exit 1
    fi
    echo "  release/fid.exe missing or stale: rebuilding it (RELEASE=1 scripts/bootstrap.sh)..."
    if ! RELEASE=1 ./scripts/bootstrap.sh build/bootstrap 3; then
        echo "ERROR: the self-hosted build failed (see above)."
        exit 1
    fi
    if ! bash scripts/release-fresh.sh release/fid.exe; then
        echo "ERROR: release/fid.exe is still not the stamped release build of this source."
        exit 1
    fi
fi
echo "  OK: $(ls -lh release/fid.exe | awk '{print $5}')"

echo "[check] Companion tools (ark, scribe, veil, fidesc)..."
# release-roadmap N8: ark, scribe and veil are the Fides ports
# (compiler/{ark,scribe,veil}.fd through their entries tools/<tool>.fd),
# built here by the fid.exe being shipped, through C. They are always
# rebuilt so they match that fid.exe. CC picks the C compiler (default
# `zig cc` when zig is on PATH; fid finds one itself otherwise).
# FIDES_RUST_TOOLS=1 ships the Rust builds from target/release instead
# (the frozen bootstrap's, as before Nov 16). fidesc.exe is the Rust
# compiler and still comes from the Cargo build. All four executables are
# staged under build/installer/tools/, not release/.
if [ -z "${CC:-}" ] && command -v zig &> /dev/null; then
    export CC="zig cc"
fi
for tool in ark scribe veil; do
    if [ "${FIDES_RUST_TOOLS:-}" = 1 ]; then
        if [ ! -f "target/release/$tool.exe" ]; then
            echo "ERROR: target/release/$tool.exe not found (run: cargo build --release)."
            exit 1
        fi
        cp "target/release/$tool.exe" "$TOOLS_DIR/$tool.exe"
        clean_tool_sidecars "$tool"
        echo "  $tool.exe: the Rust build (FIDES_RUST_TOOLS=1)"
        continue
    fi
    if [ ! -f "tools/$tool.fd" ]; then
        echo "ERROR: tools/$tool.fd not found."
        exit 1
    fi
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) ;;
        *)
            # fid.exe does not run on this host: take tools built by it on
            # Windows (newer than fid.exe), or refuse
            if [ -f "$TOOLS_DIR/$tool.exe" ] && [ "$TOOLS_DIR/$tool.exe" -nt "release/fid.exe" ]; then
                echo "  $tool.exe: the existing $TOOLS_DIR/$tool.exe (newer than fid.exe; built on Windows)"
                continue
            fi
            echo "ERROR: $TOOLS_DIR/$tool.exe must be built by release/fid.exe, which does not run on"
            echo "  this host: build the installer on Windows, or set FIDES_RUST_TOOLS=1."
            exit 1
            ;;
    esac
    rm -f "$TOOLS_DIR/$tool.exe"
    mkdir -p build/installer
    if ! ./release/fid.exe ccompile "tools/$tool.fd" -o "$TOOLS_DIR/$tool.exe" > "build/installer/$tool.log" 2>&1 \
        || [ ! -f "$TOOLS_DIR/$tool.exe" ]; then
        echo "ERROR: release/fid.exe ccompile tools/$tool.fd failed (build/installer/$tool.log):"
        tail -n 20 "build/installer/$tool.log"
        echo "  (FIDES_RUST_TOOLS=1 ships the Rust $tool.exe instead)"
        exit 1
    fi
    echo "  $tool.exe: built from tools/$tool.fd ($(ls -lh "$TOOLS_DIR/$tool.exe" | awk '{print $5}'))"
    clean_tool_sidecars "$tool"
done
if [ ! -f "$TOOLS_DIR/fidesc.exe" ]; then
    if [ -f "target/release/fidesc.exe" ]; then
        cp "target/release/fidesc.exe" "$TOOLS_DIR/fidesc.exe"
    else
        echo "ERROR: $TOOLS_DIR/fidesc.exe not found (run: cargo build --release)."
        exit 1
    fi
fi
echo "  OK"

echo "[check] Standard library..."
if [ ! -d "stdlib" ] || [ -z "$(ls stdlib/*.fd 2>/dev/null)" ]; then
    echo "ERROR: stdlib/ directory is empty or missing."
    exit 1
fi
echo "  OK: $(ls stdlib/*.fd | wc -l) modules"

echo "[check] Runtime..."
if [ ! -d "runtime" ]; then
    echo "ERROR: runtime/ directory missing."
    exit 1
fi
echo "  OK: $(ls runtime/*.c runtime/*.h 2>/dev/null | wc -l) files"

echo "[check] Documentation..."
if [ ! -d "docs" ]; then
    echo "WARNING: docs/ directory missing. Installer will skip docs."
fi
echo "  OK"

echo "[check] Examples..."
if [ ! -f "tests/bootstrap/hello.fd" ]; then
    echo "WARNING: tests/bootstrap/hello.fd not found. No examples will be included."
fi
echo "  OK"

# The seeded store of unit objects (inc-roadmap B6, docs/reference/incremental.md):
# release/fid.exe builds hello and the examples per module with
# `--seed-store`, which copies every unit's object but main's into
# build/installer/cache; the installer puts it in $INSTDIR\cache, where an
# installed fid looks before compiling a unit (compiler.fd
# fid_seed_store_dir). An object's name holds the C compiler and its flags
# (CC is not in the unit key), so the loop runs once with the CC this build
# uses (fid's default when unset) and once per other C compiler on PATH.
echo "[seed] Seeded store of unit objects (inc-roadmap B6)..."
rm -rf build/installer/cache build/installer/seed
mkdir -p build/installer/cache build/installer/seed
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
        seed_ccs=("${CC:-}")
        for cc in gcc clang "zig cc"; do
            if [ "$cc" != "${CC:-}" ] && command -v "${cc%% *}" > /dev/null 2>&1; then
                seed_ccs+=("$cc")
            fi
        done
        for cc in "${seed_ccs[@]}"; do
            tag="${cc:-default}"
            tag="${tag// /-}"
            for f in tests/bootstrap/hello.fd examples/*.fd; do
                b=$(basename "$f" .fd)
                log="build/installer/seed/$b.$tag.log"
                if [ -n "$cc" ]; then
                    CC="$cc" ./release/fid.exe ccompile "$f" --inc -o "build/installer/seed/$b.exe" --seed-store build/installer/cache > "$log" 2>&1 || echo "  (skipped $f with CC=$cc: $log)"
                else
                    ./release/fid.exe ccompile "$f" --inc -o "build/installer/seed/$b.exe" --seed-store build/installer/cache > "$log" 2>&1 || echo "  (skipped $f: $log)"
                fi
            done
        done
        ;;
    *)
        echo "  (release/fid.exe does not run on this host: no seeded store)"
        ;;
esac
echo "  OK: $(ls build/installer/cache | wc -l) objects"

echo ""
echo "[build] Creating installer..."

# Check for icon
if [ ! -f "installer/fides.ico" ]; then
    echo "  WARNING: installer/fides.ico not found. Using default NSIS icon."
    # Remove icon references from the script for this build
    sed 's/!define MUI_ICON "fides.ico"/; !define MUI_ICON "fides.ico"/' \
        "installer/fides.nsi" > "installer/fides-build.nsi"
    sed -i 's/!define MUI_UNICON "fides.ico"/; !define MUI_UNICON "fides.ico"/' \
        "installer/fides-build.nsi"
    BUILD_SCRIPT="installer/fides-build.nsi"
else
    BUILD_SCRIPT="installer/fides.nsi"
fi

# Build the installer
cd installer
makensis -V2 "-DVERSION=${VERSION}" "$(basename "$BUILD_SCRIPT")"

# Clean up temp script
if [ -f "fides-build.nsi" ]; then
    rm "fides-build.nsi"
fi

echo ""
INSTALLER="Fid-Setup-${VERSION}.exe"
if [ -f "$INSTALLER" ]; then
    # Optional Authenticode signature (unsigned installers make Windows
    # SmartScreen warn). Set FIDES_SIGN_PFX to a .pfx code-signing
    # certificate and FIDES_SIGN_PASS to its password; signtool (Windows SDK)
    # or osslsigncode (Linux/macOS) is used, whichever is on PATH.
    if [ -n "${FIDES_SIGN_PFX:-}" ]; then
        TS_URL="${FIDES_SIGN_TS:-http://timestamp.digicert.com}"
        echo "[sign] $INSTALLER"
        if command -v signtool &> /dev/null; then
            signtool sign /fd sha256 /tr "$TS_URL" /td sha256                 /f "$FIDES_SIGN_PFX" /p "${FIDES_SIGN_PASS:-}" "$INSTALLER"
        elif command -v osslsigncode &> /dev/null; then
            osslsigncode sign -pkcs12 "$FIDES_SIGN_PFX" -pass "${FIDES_SIGN_PASS:-}"                 -h sha256 -ts "$TS_URL" -in "$INSTALLER" -out "$INSTALLER.signed"
            mv "$INSTALLER.signed" "$INSTALLER"
        else
            echo "ERROR: FIDES_SIGN_PFX is set but neither signtool nor osslsigncode is on PATH."
            exit 1
        fi
    else
        echo "  NOTE: not signed (set FIDES_SIGN_PFX to sign)."
    fi
    # SHA-256 checksums to publish beside the installer
    (cd "$ROOT_DIR" && bash scripts/release-checksums.sh "installer/$INSTALLER")

    echo "=== SUCCESS ==="
    echo "  Installer:  installer/$INSTALLER"
    echo "  Size:       $(ls -lh $INSTALLER | awk '{print $5}')"
    echo ""
    echo "  Users can download and run this .exe to install Fides."
    echo "  It will:"
    echo "    - Install to C:\\Program Files\\Fid\\"
    echo "    - Add fid.exe to PATH"
    echo "    - Associate .fd files"
    echo "    - Create Start Menu shortcuts"
    echo "    - Include uninstaller"
else
    echo "=== FAILED ==="
    echo "  Installer was not created. Check NSIS output above."
    exit 1
fi
