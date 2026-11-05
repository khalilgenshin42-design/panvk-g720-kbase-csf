#!/usr/bin/env bash
# Build an AdrenoTools-compatible package for frontends that support Mesa ADPKG.
# This does not install or replace the Android system Vulkan driver.
set -euo pipefail

: "${DRIVER:?set DRIVER to the Android ARM64 libvulkan_panfrost.so}"
: "${LIBDRM:?set LIBDRM to the Android ARM64 libdrm.so}"
: "${OUT_DIR:?set OUT_DIR to the package output directory}"
: "${SOURCE_SHA:?set SOURCE_SHA to the source commit SHA}"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="$(tr -d '[:space:]' < "$ROOT/VERSION")"
PACKAGE_VERSION="${VERSION}-android-kbase-${SOURCE_SHA:0:12}"
PACKAGE_NAME="PanVK Mali G720 (experimental)"
LIBRARY_NAME="libvulkan_panfrost.so"

command -v patchelf >/dev/null
command -v readelf >/dev/null
command -v zip >/dev/null
test -s "$DRIVER"
test -s "$LIBDRM"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
cp "$DRIVER" "$OUT_DIR/$LIBRARY_NAME"
cp "$LIBDRM" "$OUT_DIR/libpanvk_drm.so"

# Keep the private dependency self-contained and avoid interposing on a
# platform-provided libdrm.so with a different ABI.
patchelf --set-soname libpanvk_drm.so "$OUT_DIR/libpanvk_drm.so"
patchelf --replace-needed libdrm.so libpanvk_drm.so "$OUT_DIR/$LIBRARY_NAME"
patchelf --set-rpath '$ORIGIN' "$OUT_DIR/$LIBRARY_NAME"

cat > "$OUT_DIR/meta.json" <<EOF
{
  "schemaVersion": 1,
  "name": "$PACKAGE_NAME",
  "description": "Experimental Mesa PanVK for Arm Mali v12/Kbase CSF. Not Vulkan conformant; device and game compatibility are unverified.",
  "author": "khalilgenshin42-design",
  "packageVersion": "$PACKAGE_VERSION",
  "vendor": "Mesa",
  "driverVersion": "$VERSION",
  "minApi": 30,
  "libraryName": "$LIBRARY_NAME"
}
EOF

# Match Winlator Mali 1.2's AdrenotoolsManager parser contract:
# it reads name, driverVersion and libraryName from root-level meta.json,
# then loads libraryName from the extracted driver directory.
python3 - "$OUT_DIR" <<'PY'
import json
import pathlib
import sys

root = pathlib.Path(sys.argv[1])
meta = json.loads((root / "meta.json").read_text())
required = ("name", "driverVersion", "libraryName")
for key in required:
    value = meta.get(key)
    if not isinstance(value, str) or not value.strip():
        raise SystemExit(f"meta.json: missing/non-string required field: {key}")
if meta["libraryName"] != "libvulkan_panfrost.so":
    raise SystemExit("meta.json: unexpected Vulkan libraryName")
if "/" in meta["name"] or "\\" in meta["name"]:
    raise SystemExit("meta.json: driver name must be a single safe directory name")
if meta.get("schemaVersion") != 1:
    raise SystemExit("meta.json: unsupported ADPKG schemaVersion")
for filename in (meta["libraryName"], "libpanvk_drm.so"):
    if not (root / filename).is_file():
        raise SystemExit(f"package is missing required library: {filename}")
PY

# Structural and linker checks against the exact flat package layout.
readelf -h "$OUT_DIR/$LIBRARY_NAME" | grep -Fq 'AArch64'
readelf -d "$OUT_DIR/$LIBRARY_NAME" > "$OUT_DIR/driver-dynamic-section.txt"
readelf -d "$OUT_DIR/libpanvk_drm.so" > "$OUT_DIR/libpanvk-drm-dynamic-section.txt"
grep -Fq 'Shared library: [libpanvk_drm.so]' "$OUT_DIR/driver-dynamic-section.txt"
grep -Fq 'Library runpath: [$ORIGIN]' "$OUT_DIR/driver-dynamic-section.txt"
grep -Fq 'Library soname: [libpanvk_drm.so]' "$OUT_DIR/libpanvk-drm-dynamic-section.txt"
! grep -Fq 'Shared library: [libdrm.so]' "$OUT_DIR/driver-dynamic-section.txt"

(
  cd "$OUT_DIR"
  sha256sum "$LIBRARY_NAME" libpanvk_drm.so meta.json > SHA256SUMS
  zip -9 -q -r "../PanVK-G720-experimental.zip" \
    meta.json "$LIBRARY_NAME" libpanvk_drm.so SHA256SUMS
)
unzip -Z1 "$OUT_DIR/../PanVK-G720-experimental.zip" | sort > "$OUT_DIR/zip-contents.txt"
grep -Fxq 'meta.json' "$OUT_DIR/zip-contents.txt"
grep -Fxq "$LIBRARY_NAME" "$OUT_DIR/zip-contents.txt"
grep -Fxq 'libpanvk_drm.so' "$OUT_DIR/zip-contents.txt"
if grep -q '/' "$OUT_DIR/zip-contents.txt"; then
  echo "Package must contain flat root entries only" >&2
  exit 1
fi
sha256sum "$OUT_DIR/../PanVK-G720-experimental.zip"
