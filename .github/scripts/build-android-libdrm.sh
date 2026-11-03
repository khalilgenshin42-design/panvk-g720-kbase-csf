#!/usr/bin/env bash
# Build the pinned upstream libdrm as an Android ARM64 shared library.
# This is a build-time dependency for PanVK's generic Panfrost helpers;
# it is not a replacement for the device's vendor Mali/Kbase driver.
set -euo pipefail

: "${ANDROID_NDK_HOME:?ANDROID_NDK_HOME must point to the Android NDK}"
: "${MESON_WORKING_DIR:?MESON_WORKING_DIR must point to the Mesa checkout}"

readonly VERSION="2.4.133"
readonly ARCHIVE="libdrm-${VERSION}.tar.xz"
readonly URL="https://dri.freedesktop.org/libdrm/${ARCHIVE}"
readonly SHA256="fc68f9d0ba2ea63c9432a299e14fea09fad7a8a66e8039fcd7802ca59f77b4f5"

curl --fail --location --retry 4 --retry-all-errors --retry-delay 3 "${URL}" -o "${ARCHIVE}"
echo "${SHA256}  ${ARCHIVE}" | sha256sum --check -
tar -xf "${ARCHIVE}"
rm "${ARCHIVE}"

meson setup build-libdrm "libdrm-${VERSION}" \
  --cross-file android-cross.txt \
  --prefix=/usr \
  --libdir=lib \
  --buildtype=release \
  -Ddefault_library=shared \
  -Dauto_features=disabled \
  -Dtests=false \
  -Dudev=false \
  -Dvalgrind=disabled \
  -Dintel=disabled \
  -Dradeon=disabled \
  -Damdgpu=disabled \
  -Dnouveau=disabled \
  -Dvmwgfx=disabled \
  -Domap=disabled \
  -Dexynos=disabled \
  -Dfreedreno=disabled \
  -Dtegra=disabled \
  -Dvc4=disabled \
  -Detnaviv=disabled

meson compile -C build-libdrm
DESTDIR="${MESON_WORKING_DIR}/libdrm-stage" meson install -C build-libdrm

LIBDIR="${MESON_WORKING_DIR}/libdrm-stage/usr/lib"
# Upstream libdrm deliberately builds an unversioned DSO on Android.
test -f "${LIBDIR}/libdrm.so"
cp "${LIBDIR}/libdrm.so" "${MESON_WORKING_DIR}/shims/libdrm.so"
readelf -d "${MESON_WORKING_DIR}/shims/libdrm.so" | grep -Fq 'Library soname: [libdrm.so]'

nm -D --defined-only "${MESON_WORKING_DIR}/shims/libdrm.so" \
  | awk '{print $3}' | sed 's/@.*//' | sort -u > "${MESON_WORKING_DIR}/libdrm-defined-symbols.txt"

for symbol in drmSyncobjWait drmPrimeHandleToFD drmGetDevices2; do
  grep -Fxq "${symbol}" "${MESON_WORKING_DIR}/libdrm-defined-symbols.txt" || {
    echo "libdrm ${VERSION} is missing required export: ${symbol}" >&2
    exit 1
  }
done

echo "Built Android ARM64 libdrm ${VERSION}; required DRM exports verified."
