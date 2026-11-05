# Clip/cull VS→FS prototype audit — 2026-11-05

## Source and build

- Code branch: `experiment/clip-cull-distance-v12`
- Source commit: `f843a225f9b4f1821e69bb52ed0718c86628b8d3`
- Android-only CI run: [41938817775](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/41938817775), job 126954039130 — SUCCESS; all 12 meaningful steps passed.
- Artifact ID: 13375886001; outer ZIP SHA-256: `52b73dc390ad8f0a3fcb537d9f247406f54ccbdf1fa3a966acf396796b588475`.
- The artifact was downloaded and inspected. Inner flat package SHA-256: `916a4c9c2b425c2dce5e7f3be472c9e1714ca2dcdea935dad710d02edc6c8f41`.
- Inner package has exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three checksums pass.
- Driver and helper are ELF64 AArch64 Android shared objects. Driver RUNPATH is `$ORIGIN`, NEEDED private helper is `libpanvk_drm.so`; helper NEEDED is only `libc.so`. Missing imported DRM symbols report is empty.
- Metadata version is `26.3.0-devel-android-kbase-f843a225f9b4`, min API 30, and explicitly marks compatibility unverified / non-conformant.

This proves compile, link, packaging and static integrity only. It does not prove runtime correctness, Vulkan CTS conformance, or a fix for the prior CSF shareability fault.

## Source audit

Reviewed:
- `src/panfrost/vulkan/panvk_vX_shader.c`
- `src/panfrost/compiler/pan_nir_collect_varyings.c`
- `src/panfrost/vulkan/panvk_vX_physical_device.c`
- `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`

Confirmed:
1. The VS transform rewrites compact cull-distance array stores only in the hardware VS variant; the XFB variant preserves original values.
2. The merged-array index is based on `(location - VARYING_SLOT_CLIP_DIST0) * 4 + location_frac`, so a cull array can begin in CLIP_DIST0 at a nonzero component or in CLIP_DIST1.
3. The internal direct VS→FS path requires an FS, an exact VS next-stage mask of FS, no explicit clip/cull input declaration in FS, and combined count ≤ 8.
4. The FS transform creates one or two vec4 generic inputs and inserts a discard based on clip/cull lanes.
5. The v12 descriptor path is `emit_varying_descs()`; the old `emit_varying_attrs()/link_shaders()` code is pre-v9 and irrelevant to G720.
6. Feature bits and limits remain zero/false: shaderClipDistance, shaderCullDistance, maxClipDistances, maxCullDistances, maxCombinedClipAndCullDistances, multiViewport and geometryStreams.

No driver code was changed in this audit.

## Validation gap / next work

The source tree has generic NIR unit tests and Panfrost compiler tests, but no dedicated PanVK test harness for these private lowering helpers. CI does not validate shader semantics.

Before enabling feature bits:
- Create a direct VS→FS NIR harness or disposable diagnostic build for a tightly filtered Vulkan CTS run. Keep any temporary feature exposure off production branches/packages.
- Test clip-only, cull-only, mixed arrays with clip counts 1/3/4/7, second-slot packing, partial final vec4, dynamic array index, explicit FS reads, no-FS/XFB, and triangles proving the negative-distance sign convention.
- Verify interpolation semantics against Vulkan primitive clip/cull rules.
- Extend data flow through software TCS/TES/GS and generated GS rasterization VS before advertising features globally.
- Keep feature bits/limits false and do not mix this feature experiment into Tomb Raider FPS testing.

## Repository inventory

A full branch API listing found 19 preserved branches. No branches were deleted, merged, or rewritten. Current important refs include:
- `experiment/clip-cull-distance-v12`: `f843a225f9b4f1821e69bb52ed0718c86628b8d3`
- `experiment/csf-sync-wait64-csg-scope`: `6634282aa14616931656b883aafb5519831eb888`
- `experiment/csf-notification-serialization`: `7e5d7a936b9f3a77255570b121f21626e1e6411b`
- `experiment/kbase-diag-opt-in`: `a87d179f3cdb77946df0c2858cd4370794645e27`
- `experiment/kbase-live-profile`: `f674d46abbac19f563999a4f2d7b03898fe3a7bc`
- `work/android-libdrm-runtime-audit-latest`: `2177569b1fea09f737d9d7bd57286af1f7937244`
- `work/winlator-adpkg-packaging`: `456cf76ab6780b08096d0f62a51200a8627c3f82`
- `main`: `a2a58d76a8413c50a4a17be8fbf14bd5393846c3`

The canonical continuity file remains on `docs/project-continuity-handoff-2026-11-03`; this audit is a supporting detailed record.
