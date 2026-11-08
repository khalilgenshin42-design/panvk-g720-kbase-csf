# Mali-G720 VRS / VK_KHR_fragment_shading_rate implementation audit

Date: 2026-11-06  
Research branch: `experiment/g720-fragment-shading-rate`  
Base: descriptor-heap source HEAD `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`

## Decision

Investigate `VK_KHR_fragment_shading_rate` as the next hardware-backed capability for Mali-G720 v12. Do not advertise the extension or any FSR feature bit until the hardware programming path and all advertised API semantics are implemented.

The target is ordinary **Mali-G720 MC7**, not Immortalis-G720. Do not conflate its VRS capability with Immortalis-only ray-tracing hardware.

## Evidence

- Arm Mali-G720 Product Support lists Variable Rate Shading and specifies pipeline, primitive, and attachment shading rates up to 4x4:
  https://support.arm.com/compute-ip/mali-g720
- Arm's Mali-G720 performance-counter reference describes coarse fragment quads and the impact of variable-rate shading:
  https://documentation-service.arm.com/static/6627bb0b2f18166b6a308b3a?token=
- Khronos describes three independent FSR modes and their independent feature bits:
  https://docs.vulkan.org/features/latest/features/proposals/VK_KHR_fragment_shading_rate.html
- Khronos extension requirements include `vkCmdSetFragmentShadingRateKHR`, `vkGetPhysicalDeviceFragmentShadingRatesKHR`, pipeline state, optional primitive rate shader output, and attachment handling. The feature structure has `pipelineFragmentShadingRate`, `primitiveFragmentShadingRate`, and `attachmentFragmentShadingRate`.
- Current PanVK source explicitly says it does not implement `VK_KHR_fragment_shading_rate`; the extension is absent from `get_physical_device_extensions`, and the physical-device properties comment says the feature is not implemented.
- A public Mali-G720 MC7 Vulkan capability report also lists `VK_KHR_fragment_shading_rate` on the stock Mali driver. This is corroborating device-specific evidence, not a substitute for PanVK implementation:
  https://fauxvkdb.canonspike.com/fr/gpu/13b5-c8700010/52e95a852ce4dc29

## Scope recommendation

Start with a **pipeline-only FSR implementation** if the v12 hardware command path can be established:
- `pipelineFragmentShadingRate = true`
- `primitiveFragmentShadingRate = false`
- `attachmentFragmentShadingRate = false`

This is a deliberately limited first step. It must still implement both pipeline-static FSR and `VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR` / `vkCmdSetFragmentShadingRateKHR` semantics where required by the extension, the physical-device query command, rate enumeration, combiners, limits, and all feature/property constraints. If the extension specification or common runtime requires more than this narrow feature subset, keep the extension disabled until those requirements are met.

Do not claim an FPS uplift merely from enabling the extension. A game or translation layer must actually request a rate coarser than 1x1. The current Tomb Raider run uses DXVK 1.7.2 and its log does not show `VK_KHR_fragment_shading_rate` in the enabled device-extension list, so that test is not expected to exercise VRS without a DXVK update or explicit application-side use.

## PanVK source audit items

1. Find the exact v12 fragment/rasterization command descriptor fields and packer support for pipeline fragment size and combiner operations. Do not invent bit positions or infer a packet layout from product-level VRS marketing.
2. Trace graphics pipeline create-info pNext parsing and static/dynamic state tracking in PanVK; identify the existing common Vulkan runtime state representation.
3. Determine how the Mali command stream expresses the effective rate and whether pipeline/primitive/attachment combiners map to hardware directly.
4. Confirm v12 support for all candidate rates, sample-count restrictions, combiner operations, and required rate enumeration. Derive values from hardware documentation or verified stock-driver properties; do not guess.
5. Only implement primitive FSR if SPIR-V `PrimitiveShadingRateKHR` output can be lowered to the correct Mali per-primitive state.
6. Only implement attachment FSR if the Kbase BO/image allocation, image format features, tiling, render-pass/dynamic-rendering attachment, layout transitions, synchronization, and attachment descriptor all support the shading-rate image contract.
7. Add feature/property queries, command dispatch, pipeline/dynamic-state handling, format queries, and validation tests together. Extension table alone is not implementation.
8. Add targeted Vulkan CTS coverage and a minimal FSR sample that verifies 1x1 and coarser rates before exposing it to games.

## Performance relevance

VRS can reduce fragment-shader invocation and bandwidth pressure when the workload is fragment-bound and accepts reduced shading quality in selected regions. It will not directly reduce DXVK CPU translation overhead, shader compilation overhead, draw-call submission overhead, or a vertex/tiler bottleneck. Measure fragment counters if available before attributing a performance gain.

For the user's Tomb Raider baseline, preserve:
- Min 7.9 FPS
- Max 18.0 FPS
- Avg 13.6 FPS
- DXVK 1.7.2, D3D11 feature level 11_0, 4 compiler threads
- swapchain 900x506
- config includes relaxed barriers, ignored graphics barriers, early discard, memory tracking, max queued frames 1, and MapNoWait.

The current uploaded log does not prove that descriptor heaps are exercised. Its enabled extension list does not include `VK_EXT_descriptor_heap`.

## Explicit non-goals

- Do not expose `VK_KHR_ray_query` based only on the Mali-G720 product name. Mali-G720 and Immortalis-G720 are distinct SKUs; ray tracing is not established for the user's ordinary Mali-G720 MC7.
- Do not force-enable `vertexPipelineStoresAndAtomics` on v12. The existing PanVK gate intentionally treats it differently on pre-v13 architectures.
- Do not advertise VRS by changing extension/feature booleans alone.


## 2026-11-08 source audit — pipeline, primitive, attachment, and dynamic paths

Audited the device-tested driver source checkpoint
`c9d61e10a9c3489d8193a8fb0767f4b5187d79f3` and its v12 GenXML.

### Confirmed pipeline path

- `get_physical_device_extensions()` advertises
  `VK_KHR_fragment_shading_rate` for `PAN_ARCH >= 12`.
- The reported feature set is deliberately pipeline-only:
  `pipelineFragmentShadingRate=true`,
  `primitiveFragmentShadingRate=false`, and
  `attachmentFragmentShadingRate=false`.
- `fragmentShadingRateNonTrivialCombinerOps=false` is retained.
- `panvk_vX_cmd_draw.c` packs pipeline draw rate and the two combiner fields
  into `PRIMITIVE_FLAGS_2`, then writes `IDVS.TILER_FLAGS2`.
- The v12 GenXML confirms the register and field definitions. The tested
  1x1 and 2x2 KEEP paths both pass GPU submit, fence, readback, and pixel
  signature; the SSBO counter measured 4096 versus 1024 invocations.

### Primitive and attachment paths are not implemented/exposed

- No PanVK v12 path was found that consumes the SPIR-V
  `PrimitiveShadingRateKHR` output and transfers it into per-primitive
  tiler state. The feature is explicitly false.
- v12 GenXML does contain `Framebuffer Parameters.VRS Image Rate` and
  `VRS Image Plane` fields, but the inspected PanVK CSF framebuffer
  construction path and `pan_fb.c` do not populate or manage a VRS image
  attachment. The feature is explicitly false.
- These GenXML fields prove that the register/descriptor format has fields;
  they do not prove the full Vulkan primitive or attachment implementation.
  Keep both feature bits false.

### Dynamic pipeline FSR remains an unverified gap

- `panvk_get_static_pipeline_vrs_rate()` explicitly rejects
  `MESA_VK_DYNAMIC_FSR`; only static pipeline state is folded into the
  fragment-center NIR lowering.
- `panvk_compile_nir()` leaves dynamic FSR on the native sample-center path.
  The adjacent compiler comment says the Bifrost/Valhall compiler does not
  currently implement `nir_load_frag_shading_rate()`.
- The existing capability probe only records
  `vkCmdSetFragmentShadingRateKHR` and ends the command buffer; it does not
  submit a draw. The render probe exercises static pipeline FSR and
  `gl_FragCoord`, not dynamic FSR.
- Therefore dynamic FSR command recording is demonstrated, but dynamic FSR
  draw execution and correct fragment-center semantics are not yet proven.
  Do not describe the extension as fully validated based on the static KEEP
  test alone.

### Combiner validity correction

With primitive and attachment features both false, Vulkan valid usage
requires combiner slot 0 and slot 1 to remain KEEP, respectively. The
non-trivial-combiner property is also false. Consequently the earlier
REPLACE/MIN/MAX/MUL device-loss cases did not constitute legal tests of
those operations on this feature set. They must not be used to conclude that
G720 hardware lacks those combiners. The corrected probe's SKIP is the right
result; no feature bits should be forced on.

### Next engineering work

1. Add a separate dynamic-FSR render case: create a pipeline with
   `VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR`, record
   `vkCmdSetFragmentShadingRateKHR(2x2, KEEP/KEEP)`, draw the same
   `gl_FragCoord` pattern, submit, wait, read back, and verify both the
   coarse pixel signature and invocation count. Keep the static KEEP cases
   as controls.
2. If dynamic FSR fails or shader compilation leaves an unsupported
   intrinsic, implement the required dynamic fragment-rate/sample-center
   sysval path before claiming dynamic support.
3. Test every rate returned by `vkGetPhysicalDeviceFragmentShadingRatesKHR`
   at its advertised sample counts before treating the full rate list as
   runtime-qualified.
4. Keep primitive and attachment features disabled until their shader-output
   and framebuffer/image paths, format/usage restrictions, synchronization,
   and CTS coverage are complete.

No Android driver source was changed in this audit. No new device test is
requested until the dynamic render probe is built and its artifact is
independently checked.


## 2026-11-08 — dynamic FSR render probe prepared

Added a separate `--case=dynamic-keep` path to the Windows render probe.
It creates a graphics pipeline with
`VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR`, records
`vkCmdSetFragmentShadingRateKHR(2x2, KEEP/KEEP)`, submits an actual draw
using the existing `gl_FragCoord` shader, waits for completion, reads back
the image, and compares both the pixel checksum and fragment invocation
count against the static 2x2 KEEP result. The existing static 1x1 and 2x2
controls run first in the same process.

Build verification:
- Source commit: `d793346dc75ab8e58676b39d41afbee75aa77c83`
- GitHub Actions run #32: success
  https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42431201125
- Artifact: `panvk-vrs-render-probe-win64`, ID `13597576380`
- Artifact ZIP SHA-256:
  `9b54bf53664d2ff16923eb621a4c9ac93e0bcce543b1ac60d44f4c648b67696b`
- Executable SHA-256:
  `f0e7fc5d84645bf6f4927b4adf5ec1ee5b447d2187eca088b44d64a9d1d083a8`
- ZIP contents were downloaded and independently inspected: valid x86-64
  Windows PE executable plus the KEEP, REPLACE, all-cases, and dynamic-check
  scripts. The dynamic command and expected log strings are present in the
  executable.

This is a probe-only artifact. It does not contain or replace the Android
PanVK driver. Do not install a new driver for this test; use the already
working `git-c9d61e10a9` package. The next user action is to run
`tools/run-vrs-dynamic-check.cmd` from the extracted artifact and return
only `panvk_vrs_dynamic.log`. The test is deliberately isolated from the
invalid non-KEEP combiner cases.


## 2026-11-08 device result — dynamic FSR render fails pixel signature

The new `--case=dynamic-keep` probe was run on the already-installed
`git-c9d61e10a9` PanVK package on the Mali-G720 MC7. Static controls remain
green, and the dynamic draw submits and completes without device loss.

### Exact result

- Static 1x1 KEEP: submit/fence/readback PASS; checksum
  `0x06d78d985ad50383`; equal-neighbours 0; fragment invocations 4096.
- Static 2x2 KEEP: submit/fence/readback PASS; checksum
  `0xcad734dde7920383`; equal-neighbours 4096; pixel signature PASS;
  fragment invocations 1024.
- Dynamic 2x2 KEEP: submit/fence/readback PASS; checksum
  `0x0e61401d4af57b83`; equal-neighbours 4096; pixel signature FAIL
  against static 2x2; fragment invocations 1024, matching static 2x2.
- Overall probe: FAIL, solely because the dynamic pixel signature differs.

### Interpretation

The dynamic command is accepted, the draw executes, the GPU fence completes,
the readback succeeds, and the SSBO counter confirms the same 4x reduction in
fragment invocations as static 2x2. This demonstrates that the dynamic state
is affecting the hardware fragment execution rate.

It does **not** demonstrate correct dynamic fragment-centre/interpolation
semantics. The driver source explains the discrepancy:

- `panvk_get_static_pipeline_vrs_rate()` rejects
  `MESA_VK_DYNAMIC_FSR`.
- The static compiler lowering replaces
  `nir_intrinsic_load_sample_pos_or_center` with the center derived from
  the statically resolved rate.
- For dynamic FSR, `panvk_compile_nir()` explicitly leaves the shader on
  the native sample-centre path, with a source comment that Bifrost/Valhall
  does not currently implement `nir_load_frag_shading_rate()`.

The most likely explanation is therefore a missing dynamic fragment-centre
lowering/sysval path: the hardware is executing 2x2 coarse shading, but the
shader's observable fragment coordinate differs from the static path. This
is a source-supported diagnosis, but it is not yet a proven root cause; a
minimal shader isolating `gl_FragCoord`/sample-centre behavior is required
before changing compiler code.

### Next engineering steps — no new user test yet

1. Inspect NIR's fragment shading-rate and sample-position intrinsics and
   the Bifrost/Valhall backend's supported sysval lowering. Determine whether
   the dynamic rate is exposed to the shader as a hardware sysval or must be
   reconstructed from the dynamic command state.
2. Add a focused probe with separate output channels for pixel coordinate,
   `gl_FragCoord.xy`, and invocation ID/counter so the source of the checksum
   difference is visible rather than inferred from one aggregate checksum.
3. Compare dynamic and static 2x2 with a shader that does not read
   `gl_FragCoord`. This separates hardware rate execution from fragment
   coordinate lowering.
4. Only then implement dynamic sample-centre handling (if the backend can
   represent it safely) and rerun static controls plus dynamic KEEP.
5. Keep primitive/attachment feature bits false and do not advertise full
   dynamic FSR correctness until both output semantics and relevant CTS
   coverage pass.

No Android driver source was modified by this test. No package reinstall or
additional user-side test is requested at this point.
