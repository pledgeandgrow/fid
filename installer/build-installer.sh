#!/bin/bash
# installer/build-installer.sh — Build the Fides Windows installer
#
# Requirements:
#   - NSIS 3.x installed (sudo apt install nsis on Ubuntu/Debian)
#   - The self-hosted compiler at release/fid.exe (built here when
#     missing: RELEASE=1 ./scripts/bootstrap.sh, which needs
#     target/release/fidesc and a C compiler)
#   - stdlib/ and runtime/ directories present
#
# Usage:
#   ./installer/build-installer.sh
#
# Output:
#   installer/Fides-Setup-<version>.exe

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
# FIDES_VERSION overrides the default version written in installer/fides.nsi
VERSION="${FIDES_VERSION:-$(sed -n 's/^ *!define VERSION "\(.*\)"/\1/p' "$SCRIPT_DIR/fides.nsi" | head -1)}"

cd "$ROOT_DIR"

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
if [ ! -f "release/fid.exe" ]; then
    # The self-hosted compiler: 3-stage bootstrap with a fixpoint check
    echo "  release/fid.exe not found: building it (scripts/bootstrap.sh)..."
    if ! RELEASE=1 ./scripts/bootstrap.sh build/bootstrap 3; then
        echo "ERROR: the self-hosted build failed (see above)."
        exit 1
    fi
fi
echo "  OK: $(ls -lh release/fid.exe | awk '{print $5}')"

echo "[check] Companion tools (fidesc, ark, scribe, veil)..."
# The installer ships the canonical fidesc.exe and the companion tools next
# to fid.exe. Stage them from the Cargo build if they are not there yet.
for tool in fidesc ark scribe veil; do
    if [ ! -f "release/$tool.exe" ]; then
        if [ -f "target/release/$tool.exe" ]; then
            cp "target/release/$tool.exe" "release/$tool.exe"
        else
            echo "ERROR: release/$tool.exe not found (run: cargo build --release)."
            exit 1
        fi
    fi
done
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
INSTALLER="Fides-Setup-${VERSION}.exe"
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
    echo "    - Install to C:\\Program Files\\Fides\\"
    echo "    - Add fid.exe to PATH"
    echo "    - Associate .fd files"
    echo "    - Create Start Menu shortcuts"
    echo "    - Include uninstaller"
else
    echo "=== FAILED ==="
    echo "  Installer was not created. Check NSIS output above."
    exit 1
fi
