#!/usr/bin/env bash
set -e

# Usage: ./package.sh RelWithDebInfo

if [ -z "$1" ]; then
    echo "❌ No build type provided!"
    echo "Usage: $0 <Debug|Release|RelWithDebInfo>"
    exit 22
fi

BUILD_TYPE=$1

# Base directory where your compiled binaries live
BASE_DIR="../MSVC2022_64/$BUILD_TYPE"

# Output directories (relative to script)
PACKAGE_DIR="package"
DATE_STR=$(date +%Y-%m-%d)
ZIP_NAME="tes3mp-${BUILD_TYPE}-${DATE_STR}.zip"

# Required executables and data
PACKAGE_REQS=(
    "tes3mp.exe"
    "tes3mp-browser.exe"
    "tes3mp-server.exe"
    "openmw-launcher.exe"
    "openmw-wizard.exe"
    "openmw-iniimporter.exe"
    "defaults.bin"
    "defaults-cs.bin"
    "tes3mp-client-default.cfg"
    "tes3mp-server-default.cfg"
    "gamecontrollerdb.txt"
    "openmw.cfg"
)

# Docs to include
DOCS=(
    "tes3mp-credits.md"
    "tes3mp-changelog.md"
    "AUTHORS.md"
    "LICENSE"
)

# Clean and create fresh package folder
echo "🧹 Cleaning old package folder..."
rm -rf "$PACKAGE_DIR" "$ZIP_NAME"
mkdir -p "$PACKAGE_DIR"

# Copy resources and plugins
echo "📦 Copying resources and plugin folders..."
cp -r "$BASE_DIR/resources" "$PACKAGE_DIR/" 2>/dev/null || echo "(no resources folder found)"
cp -r "$BASE_DIR/osgPlugins-3.6.5" "$PACKAGE_DIR/" 2>/dev/null || echo "(no osgPlugins folder found)"
cp -r "$BASE_DIR/platforms" "$PACKAGE_DIR/" 2>/dev/null || echo "(no platforms folder found)"

# Copy executables and config files
echo "📦 Copying executables and configs..."
for REQ in "${PACKAGE_REQS[@]}"; do
    find "$BASE_DIR" -name "$REQ" -exec cp "{}" "$PACKAGE_DIR/" \; 2>/dev/null || true
done

# Copy documentation (search in repo root or nearby)
echo "📄 Copying documentation..."
for DOC in "${DOCS[@]}"; do
    find .. -maxdepth 2 -name "$DOC" -exec cp "{}" "$PACKAGE_DIR/" \; 2>/dev/null || true
done

# Copy all DLLs
echo "🧩 Copying DLLs..."
find "$BASE_DIR" -name "*.dll" -exec cp "{}" "$PACKAGE_DIR/" \;

# Clone CoreScripts directly into /server/
echo "🌐 Cloning CoreScripts..."
mkdir -p "$PACKAGE_DIR/server"
git clone --depth 1 https://github.com/TES3MP/CoreScripts.git "$PACKAGE_DIR/server"

# Remove git metadata
echo "🧼 Removing .git from CoreScripts..."
rm -rf "$PACKAGE_DIR/server/.git"

# Ensure server/lib exists
mkdir -p "$PACKAGE_DIR/server/lib"

# Download Lua dependencies into /server/lib/
echo "🔽 Acquiring IO2..."
curl -L -o "$PACKAGE_DIR/server/lib/io2.dll" https://github.com/DreamWeave-MP/Lua-io2/releases/download/Stable-CI/io2-MinSizeRel.dll

echo "🔽 Acquiring CJson..."
curl -L -o "$PACKAGE_DIR/server/lib/cjson.dll" https://github.com/DreamWeave-MP/lua-cjson/releases/download/Stable-CI/cjson-MinSizeRel-Windows.dll

# Check for zip tool
if ! command -v zip >/dev/null 2>&1; then
    echo "❌ 'zip' command not found!"
    echo "👉 To install it in Git Bash (MSYS2), run:  pacman -S zip"
    echo "Or install it via Chocolatey:  choco install zip"
    exit 1
fi

# Zip the result
echo "🗜️ Creating ZIP archive..."
(cd "$PACKAGE_DIR" && zip -r "../$ZIP_NAME" . > /dev/null)

echo "✅ Packaging complete!"
echo "Final directory: $PACKAGE_DIR/"
echo "ZIP archive: $ZIP_NAME"
