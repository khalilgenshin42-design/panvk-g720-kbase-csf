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
