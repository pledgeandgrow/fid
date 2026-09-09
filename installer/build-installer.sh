#!/bin/bash
# installer/build-installer.sh — Build the Fides Windows installer
#
# Requirements:
#   - NSIS 3.x installed (sudo apt install nsis on Ubuntu/Debian)
#   - The Fides compiler built at release/fides-c3.exe
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
VERSION="1.0.0"

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
if [ ! -f "release/fides-c3.exe" ]; then
    echo "ERROR: release/fides-c3.exe not found. Build the compiler first."
    exit 1
fi
echo "  OK: $(ls -lh release/fides-c3.exe | awk '{print $5}')"

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
makensis -V2 "$BUILD_SCRIPT"

# Clean up temp script
if [ -f "fides-build.nsi" ]; then
    rm "fides-build.nsi"
fi

echo ""
INSTALLER="Fides-Setup-${VERSION}.exe"
if [ -f "$INSTALLER" ]; then
    echo "=== SUCCESS ==="
    echo "  Installer:  installer/$INSTALLER"
    echo "  Size:       $(ls -lh $INSTALLER | awk '{print $5}')"
    echo ""
    echo "  Users can download and run this .exe to install Fides."
    echo "  It will:"
    echo "    - Install to C:\\Program Files\\Fides\\"
    echo "    - Add fides.exe to PATH"
    echo "    - Associate .fd files"
    echo "    - Create Start Menu shortcuts"
    echo "    - Include uninstaller"
else
    echo "=== FAILED ==="
    echo "  Installer was not created. Check NSIS output above."
    exit 1
fi
