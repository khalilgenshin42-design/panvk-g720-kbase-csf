# Experimental PanVK ADPKG package (Winlator / Adrenotools)

This package is built from this repository's Android ARM64 PanVK/Kbase-CSF build.
It is an experimental custom Vulkan driver package; it does not replace Android's
system/vendor Vulkan driver.

## Package contract

The ZIP root contains:
- `meta.json` (ADPKG schema version 1)
- `libvulkan_panfrost.so` (main Vulkan ICD)
- `libpanvk_drm.so` (private Android ARM64 libdrm dependency with a private SONAME)
- `SHA256SUMS`

The driver is patched in the package copy only:
- `DT_NEEDED libdrm.so` becomes `libpanvk_drm.so`
- `DT_RUNPATH` becomes `$ORIGIN`

The staged build artifact remains unchanged. This prevents accidental dependency
collision with any platform-provided library named `libdrm.so` and makes the
private dependency resolvable from the package directory when loaded by a
compatible Adrenotools frontend.

## Important limitations

A successful package audit proves archive structure, ELF architecture and the
declared private dependency path. It does **not** prove that the target frontend
accepts the package, that its linker namespace resolves the dependency, that
Kbase 49.x ABI calls work, or that Vulkan device creation, WSI, DXVK or games work.

The driver has not been certified for the Dimensity 8400 Ultra / Mali-G720 MC7.
Do not treat it as a production driver. Keep the stock System driver available
as the fallback.

## Build

CI invokes `.github/scripts/package-panvk-adpkg.sh` after compiling PanVK and
Android ARM64 libdrm. The resulting `PanVK-G720-experimental.zip` is uploaded
as a separate artifact alongside the untouched staged build and audit.
