# Mali-G720 v12 PanVK capability matrix — 2026-11-05

Target: Mali-G720 MC7 (PAN_ARCH=12), Android Kbase CSF backend.
Source under audit: `experiment/g720-performance-audit`, physical-device
source blob `6f8062ef02dd8b7707b2a16d4868aa7d84f4a4fb`,
shader source blob `4a163dcbf74a10c48caea79801318da48f4affcc`,
query-pool source blob `65db1d4ef1a111bd731b4b57180116b7beebbc31`.

This is an implementation audit, not a claim of Vulkan conformance. A
hardware capability, architecture gate, extension bit, feature bit, property
limit, and working end-to-end Vulkan implementation are separate things.
Do not enable a feature by changing only its advertised boolean.

## 1. Already exposed on v12

The current physical-device extension table uses `PAN_ARCH >= 10` for
several v12 capabilities, including:

- `VK_KHR_draw_indirect_count` / `VK_KHR_copy_memory_indirect`
- `VK_KHR_robustness2` and `VK_EXT_robustness2`
- `VK_EXT_conditional_rendering`
- `VK_EXT_transform_feedback`
- `VK_EXT_nested_command_buffer`
- `VK_EXT_rasterization_order_attachment_access`
- `VK_EXT_sampler_filter_minmax`
- `VK_EXT_ycbcr_2plane_444_formats` and `VK_EXT_ycbcr_image_arrays`
- `VK_KHR_dynamic_rendering`, synchronization2, timeline semaphores,
  buffer device address, descriptor indexing, mutable descriptors, and
  Vulkan memory model.
- ARM shader-core builtins/properties and scheduling controls are advertised.

The v12 core/extension feature structure already sets true for descriptor
indexing and its update-after-bind variants, runtime descriptor arrays,
non-uniform descriptor indexing, 8/16-bit storage, shader int8/int16/int64,
float16, buffer device address, Vulkan memory model, dynamic rendering,
subgroup operations, conditional rendering, transform feedback, and many
modern dynamic-state features. These are not missing toggles to flip again.
Their exact limits and Kbase runtime behavior still need validation.

## 2. Deliberately unavailable or only partially implemented

| Capability | Current source evidence | Classification / action |
|---|---|---|
| `multiViewport` | Feature false; `maxViewports=1`; draw state uploads viewport 0. | Not native in current path. Software emulation exists in JimVulkan's later PanVK fork via GS viewport-index transform + FS scissor discard. Port is a multi-file feature, not a boolean flip. |
| `shaderClipDistance` / `shaderCullDistance` | Feature bits false; max clip/cull distances are all 0. Existing NIR lowering converts direct VS->FS distance values to generic varyings and FS discard, but explicitly excludes FS reads of the arrays and geometry/tessellation propagation. | Partial software implementation. Candidate to complete and test; do not expose until all legal stage/link paths and properties are implemented. |
| `pipelineStatisticsQuery` | Feature false; query pool supports occlusion/timestamp/primitives-generated, not pipeline-statistics. Shader sysval `load_stat_query_address_poly` returns the safe OOB sink. | Not implemented in this branch. JimVulkan counts selected pipeline stats from draw/dispatch parameters on CPU, with documented approximation for geometry/tessellation. This is compatibility/diagnostic functionality, not a GPU speed feature. |
| `vertexPipelineStoresAndAtomics` | v12 false unless explicit force-atomics debug option. Source documents speculative references to invalid indices on pre-v13. | Keep disabled on v12 pending proof of safe semantics. JimVulkan also keeps this false for v10-v12 and enables natively only on v13+. |
| `geometryStreams` | Feature false; NIR lowering maps rasterization stream to constant stream 0. | Multi-stream transform feedback is not implemented. |
| Sparse binding/residency | Kbase path forces `has_sparse=false`; sparse bind queue is absent. | Backend-limited. Keep disabled; Kbase cannot provide the required GPU virtual-page remapping path in this integration. |
| Float atomic add | Float32 atomics are exposed, but buffer/shared/image float32 atomic-add bits are false. | Distinct atomic-add operation support must be audited in compiler/ISA before exposure. |
| `VK_KHR_ray_query` | No ray-query extension or acceleration-structure implementation in this source branch's device extension table. | Not implemented here. Requires acceleration-structure objects/build/update, traversal lowering, SPIR-V/NIR support, memory/sync semantics, and feature/property reporting. |

### Why clip/cull is not a one-line enable

In `panvk_vX_shader.c`, the current implementation:
- only captures clip/cull counts for a direct VS -> FS link;
- limits the combined count to eight;
- converts cull-distance values to discard indicators in the vertex shader;
- adds fragment-shader discard based on generic varying values;
- refuses the fragment lowering when the application fragment shader declares
  its own clip/cull inputs;
- does not propagate distances through geometry/tessellation stages.

The feature bits remain false in the same source. The properties report zero
maximum clip/cull distances. Enabling either feature now would let applications
submit legal shaders for which the current path does not preserve semantics.

### Why pipeline statistics are not a one-line enable

The query-pool implementation has no `VK_QUERY_TYPE_PIPELINE_STATISTICS`
case, and the compiler lowering redirects its internal statistics address to
`PAN_SHADER_OOB_ADDRESS`. Setting `pipelineStatisticsQuery=true` would expose
a path with no real result writer. A CPU-counted implementation is possible,
but must define and test each statistic across direct/indirect draws,
dispatches, software geometry/tessellation, and conditional rendering.

## 3. High-value comparison with later PanVK work

The later `JimVulkan/mali-panvk` tree (Mesa 27.0.0-devel; its README reports
G720 testing) is a useful implementation reference for:
- multiple viewports: geometry-shader viewport-index output, vertex position
  remapping, and fragment discard against the selected viewport rectangle;
- clip/cull distances: generic varying transport and fragment discard on the
  direct pre-raster-to-fragment path (its linker still passes zero clip/cull
  counts when GS/TES precedes FS);
- pipeline statistics: CPU accounting from command parameters, with documented
  omissions for indirect work and fragment invocations and approximations for
  geometry/tessellation;
- BCn texture emulation for formats not natively decoded by a Mali GPU;
- `VK_EXT_descriptor_heap`, with descriptor heap tables, push-data handling,
  shader lowering and device feature/property support.

This is a valuable implementation reference, not proof that its changes can
be copied directly into this Mesa 26.3-derived branch. The later tree has
additional files, generated Vulkan registry support, and API changes. Each
feature needs a source-level port, build, CTS/focused tests, and runtime tests.

Useful references:
- https://github.com/JimVulkan/mali-panvk
- https://github.com/JimVulkan/mali-panvk/blob/main/src/panfrost/vulkan/panvk_vX_physical_device.c
- https://github.com/JimVulkan/mali-panvk/blob/main/src/panfrost/vulkan/panvk_vX_shader.c
- https://github.com/JimVulkan/mali-panvk/blob/main/src/panfrost/vulkan/panvk_vX_query_pool.c
- https://github.com/JimVulkan/mali-panvk/blob/main/README.md

## 4. Priority order for this project

1. **Capability inventory at runtime:** collect complete `vkEnumerateDeviceExtensionProperties`,
   `vkGetPhysicalDeviceFeatures2`, and `vkGetPhysicalDeviceProperties2`
   output from the imported driver, not just DXVK's enabled-extension log.
2. **DX11 compatibility:** audit and port the completed clip/cull and
   multi-viewport software paths if the source and tests show value; complete
   geometry-stream support only if a real workload requires it.
3. **D3D12 compatibility:** evaluate `VK_EXT_descriptor_heap` porting as a
   substantial Mesa-registry + driver + compiler change. Do not confuse it
   with core descriptor indexing, which is already exposed.
4. **BCn texture path:** query the actual G720 format bits first. If BCn is
   unavailable, compare the JimVulkan GPU decode/emulation path and its
   memory/performance cost against current DXVK fallback behavior.
5. **Pipeline statistics:** useful for validation and diagnosis, but lower
   priority than rendering and descriptor capabilities; CPU estimates must
   be labelled and validated.
6. **Ray query:** long-term research only. The G925 experimental path is a
   different architecture and does not establish a safe G720 v12 port.
7. **Keep unsafe paths disabled:** sparse Kbase binding, v12 vertex-pipeline
   stores/atomics, and unsupported geometry streams stay off until complete
   implementations and evidence exist.

## 5. Immediate next source task

Do not edit the feature table yet. First compare the current branch against
JimVulkan's clip/cull and multi-viewport implementations and map every
required helper, state field, shader variant, and test. Start with clip/cull
because the current branch already has a partial implementation; it is the
smallest coherent feature-completion candidate. Keep this feature on a
dedicated branch until build and focused runtime correctness tests pass.

## 6. Benchmark correction

The 65,536-threshold run was min 9.0 / max 17.6 / average 13.5 FPS.
The 262,144-threshold run was min 7.8 / max 18.9 / average **13.9 FPS**
(corrected by the user; earlier 3.9 was a transcription omission). This is a
+0.4 FPS average change with lower minimum FPS. It is not enough evidence to
promote the candidate or continue threshold tuning as the main project.


### Clip/cull follow-up source inspection on dedicated branch

Created dedicated branch `experiment/g720-clip-cull-capabilities` from the
performance-audit checkpoint. No runtime source files have been changed yet.

Further inspection confirms the generic Mesa NIR helpers already exist in
this tree:
- `nir_merge_clip_cull_distance_vars()` and
  `nir_gather_clip_cull_distance_sizes_from_vars()` in
  `src/compiler/nir/nir_clip_cull_distance_io_utils.c` (blob
  `79555c202ba93bb43d1ff5edc7905a2a3051166e`).
- `nir_separate_merged_clip_cull_io()` in
  `src/compiler/nir/nir_separate_merged_clip_cull_io.c` (blob
  `b47d2aeb353780c3b7bd0508c8e24766a948b5db`).
- `nir_opt_clip_cull_const()` in
  `src/compiler/nir/nir_opt_clip_cull_const.c` (blob
  `4f937bda39fa4d893f80a9eb2b92472047834a41`).

The current PanVK `panvk_vX_shader.c` contains no calls to these helpers;
it only invokes its own direct VS->FS lowering. This suggests the next step
should be to wire and validate the standard NIR representation/array
normalization through PanVK's VS, TES, GS-emulation and FS pipeline before
changing feature bits.

Important correction to the comparison reference: JimVulkan's later PanVK
sets clip/cull feature bits true and handles the direct pre-raster-to-FS path,
but its shader-linking code explicitly sets clip/cull counts to zero when a
geometry or tessellation stage precedes the FS, with a comment that geometry
emulation does not yet carry those values. Treat it as a reference for the
software lowering, **not proof of complete all-stage Vulkan semantics**.
Likewise its pipeline-statistics implementation explicitly omits indirect
work and fragment invocations and approximates geometry/tessellation counts.
These caveats are now part of the port plan.


## 5. Follow-up source audit: stage transport (2026-11-05)

The shared compiler tree contains useful building blocks that were not wired
into PanVK's current clip/cull path:

- NIR has generic gather, merge, and float-array-to-vec4 lowering helpers,
  including arrayed interfaces for geometry and tessellation stages.
- libpoly's software VS/TES output-buffer ABI uses the complete varying-slot
  output mask; TCS per-vertex output masks likewise include ordinary slots.
  This is a plausible transport path for clip/cull values, subject to NIR
  interface and layout tests.
- The passthrough geometry helper has explicit clip/cull sizes and compact
  packing logic. PanVK's active custom software-GS path instead calls
  `poly_nir_lower_gs()`; its raster-shader output collector is per-slot and
  does not visibly consume clip/cull size metadata. These are distinct paths.

Therefore, the next implementation must explicitly test and, if needed, fix
custom GS transport rather than assuming passthrough-GS support covers it.
TCS/TES-alone and TES->GS paths also require independent verification. The
current direct VS->FS lowering still excludes explicit FS reads and non-direct
stage links. No feature bits or property limits have been changed.


### Correctness gates before implementation

- Existing direct cull emulation uses an interpolated 0/1 marker and an
  FS threshold of `0.9999`. That is an approximation of primitive-level
  culling and can produce a narrow false-discard region for mixed-sign
  vertex values. A reference test must define the accepted behavior.
- NIR clip/cull helper ordering is not yet established for this tree. The
  merge pass rewrites cull locations and the vec4 pass gathers sizes by
  location; add NIR tests for clip-only, cull-only, mixed/non-vec4-aligned
  arrays, and arrayed GS/TCS/TES IO before wiring either pass into PanVK.


RADV's pipeline source confirms the interface-linking sequence
`nir_gather_clip_cull_distance_sizes_from_vars()` then
`nir_merge_clip_cull_distance_vars()`. The separate
`nir_lower_clip_cull_distance_to_vec4s()` helper must not be assumed to follow
that sequence; it changes the representation and needs its own test.


## 6. Initial automated NIR coverage

The dedicated clip/cull branch now includes `src/compiler/nir/tests/clip_cull_distance_tests.cpp`, covering clip-only, cull-only, mixed VS arrays, a mixed array crossing a vec4 slot boundary, and mixed FS inputs for the generic gather/merge helpers. A host CI workflow runs the NIR suite; Android build workflows are also enabled. Results are pending. This does not yet validate vec4 lowering or PanVK stage transport and discard behavior. Feature exposure remains disabled.


CI note: the first NIR workflow reached Meson configuration successfully but failed at the build step because the requested Ninja target path was invalid (`src/compiler/nir/nir_tests`). The workflow was corrected to target `nir_tests`; a rerun is queued. This is not a compiler/test failure, and no successful test run is claimed yet.
