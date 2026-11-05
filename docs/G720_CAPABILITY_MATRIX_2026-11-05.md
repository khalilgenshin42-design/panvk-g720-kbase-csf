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


Host NIR CI was refined after discovering that disabling all Vulkan drivers caused Meson to select `with_nir_headers_only`, omitting the real NIR implementation and test target. The workflow now enables `vulkan-drivers=panfrost` and `panfrost-kmds=kbase` so the full NIR library/tests are built. Rerun pending; no successful result is claimed.


## 5. Exhaustive source census (2026-11-05)

Directly parsed from `src/panfrost/vulkan/panvk_vX_physical_device.c` on `experiment/g720-clip-cull-capabilities` (blob `6f8062ef02dd8b7707b2a16d4868aa7d84f4a4fb`):
- 185 extension-table field initializers in `get_physical_device_extensions()`.
- 296 feature-field initializers in `get_physical_device_features()`.
- These are source initializer counts, **not** counts of extensions/features enabled at runtime. `#ifdef` platform gates, architecture conditions, Kbase property checks, and explicit false feature bits must be evaluated individually.
- The table includes Vulkan 1.4-era KHR maintenance5-9, KHR map_memory2, KHR pipeline_binary, KHR dynamic_rendering_local_read, and EXT host_image_copy, graphics_pipeline_library, nested_command_buffer, map_memory_placed, plus ARM core builtins/properties/scheduling controls. The older abbreviated list above is incomplete.

### Descriptor heap: priority implementation target

Current branch does not initialize `.EXT_descriptor_heap` and does not set `descriptorHeap`. Reference `JimVulkan/mali-panvk` commit `58f2c55854bd48b18df756edb11f187af9cd2fde` has a Valhall implementation (`PAN_ARCH >= 9`) with feature `descriptorHeap=true`, capture/replay false. It is not a one-line port: relevant implementation includes descriptor state, push data, descriptor resource table/layout, shader `uses_heaps`, SPIR-V `DescriptorHeapEXT`, NIR lowering, and Vulkan runtime code (`vk_nir_lower_descriptor_heaps.c`, 1,146 lines). Treat this as a full multi-file port and keep it unadvertised until end-to-end complete.

### Capability safety flags

- No `VK_KHR_ray_query` or acceleration-structure implementation exists in this source path. Do not expose ray query without verifying G720 RT IP support and implementing AS build/update/traversal, shader translation, memory and synchronization semantics.
- Sparse binding/residency is unavailable on Kbase: bind queue is a stub; do not enable sparse feature bits.
- Existing disabled/partial v12 areas include multiViewport, clip/cull distance, pipeline statistics queries, geometry streams, and selected float atomic feature bits. Audit each full path rather than flipping booleans.
- Validate that every advertised extension's required entrypoints and feature/property structures are actually wired, especially for platform-conditional WSI and Android gralloc extensions.

### Next actions

1. Expand this matrix into a per-extension/per-feature/per-property ledger with exact source references, gates, entrypoints, compiler paths, Kbase dependencies, and CTS coverage.
2. Compare and port the JimVulkan descriptor-heap implementation against this branch's exact Mesa base, including generated Vulkan registry/entrypoint/SPIR-V/NIR/runtime pieces.
3. Audit false-positive exposure in currently advertised extensions before broadening the advertised set.
4. Build on Android ARM64 and run descriptor indexing/heap CTS plus VKD3D-Proton D3D12 workloads only after the implementation is complete.


## Addendum: descriptor-heap infrastructure re-check (2026-11-05, follow-up)

A deeper file-level check corrected an earlier assumption: this Mesa 26.3.0-devel source tree already contains much of the *generic Vulkan runtime and SPIR-V infrastructure* for descriptor heaps, even though PanVK does not advertise or consume the feature.

Verified on `experiment/g720-clip-cull-capabilities`:
- `src/vulkan/runtime/vk_nir_lower_descriptor_heaps.c` exists (1,107 lines; blob `b832cda57612600a3ce201a0d69cbb31743fb2b2`) and is already included by `src/vulkan/runtime/meson.build` (blob `7da99c886ea4dce01ff5c74cb820101009d031f3`, source list around line 265).
- `src/vulkan/runtime/vk_nir_lower_descriptor_heaps.h` is byte-identical to the reference JimVulkan file at commit `58f2c55854bd48b18df756edb11f187af9cd2fde` (blob `6b344dc2d653f31517e1007ade3922cc44fbb70c`).
- `src/compiler/spirv/spirv_to_nir.c` already recognizes `DescriptorHeapEXT` as a supported SPIR-V capability (blob `ed441438c167b7ff582795146c614f40a0f1b49d`). This is compiler capability parsing, not proof PanVK has shader lowering or execution support.
- `src/panfrost/vulkan/panvk_vX_nir_lower_descriptors.c` (1,432 lines), `panvk_shader.h` (723 lines), `panvk_vX_shader.c` (3,716 lines), `panvk_vX_cmd_desc_state.c` (450 lines), `panvk_vX_cmd_push_constant.c` (124 lines), `panvk_descriptor_set_layout.h` (212 lines), and `panvk_vX_physical_device.c` (1,433 lines) currently have no direct `descriptor_heap`, `DescriptorHeapEXT`, `uses_heaps`, or `CmdPushDataEXT` matches. The generic runtime and SPIR-V parser are therefore present but PanVK integration is missing.
- Current tree reports `VERSION` = `26.3.0-devel`; JimVulkan reference is a later Mesa 27.0.0-devel line. Port must account for API/ABI drift rather than copy whole files.

### Revised descriptor heap port plan

1. Confirm `include/vulkan/vulkan_core.h` and `src/vulkan/registry` are generated/submodule inputs in this checkout; the attempted header path fetch returned an empty GitHub blob, so use local build generation or the correct vendored registry path before modifying registry data.
2. Inspect the generic runtime source delta (current 1,107 lines vs reference 1,146) and identify whether missing 39 lines are upstream evolution or relevant fixes. Keep current runtime implementation unless a specific required helper/API is absent.
3. Port PanVK-specific descriptor heap state, heap/resource table layout, push data command, shader `uses_heaps` metadata, descriptor NIR lowering, and shader ABI from JimVulkan, adapting to Mesa 26.3 signatures.
4. Check `vk_device_extension_table` generated availability and extension XML/headers before adding extension table field; do not edit generated outputs manually if they are regenerated by Meson.
5. Add extension/feature exposure only after all call paths are implemented and compile; test shader module creation, descriptor heap creation/update/bind, push data, descriptor indexing, and VKD3D-Proton D3D12.

This is a meaningful narrowing of the port: generic runtime + SPIR-V capability parsing are already present, so the primary missing work is PanVK-specific integration and version adaptation, not reimplementing the 1,100-line runtime lowerer from scratch.


## Addendum: PanVK-side descriptor heap integration map (2026-11-05)

Reference implementation inspected at `JimVulkan/mali-panvk` commit `58f2c55854bd48b18df756edb11f187af9cd2fde` (Mesa 27.0.0-devel). The current project is Mesa `26.3.0-devel`; do not assume source/API compatibility.

### Exact PanVK integration points confirmed in reference

- `panvk_descriptor_set_layout.h`: reserves resource-table slots after driver set and normal descriptor sets: `PANVK_HEAP_RESOURCE_TABLE`, `PANVK_HEAP_SAMPLER_TABLE`, `PANVK_HEAP_EMBEDDED_TABLE`, `PANVK_HEAP_TABLE_END`; architecture-gated for PAN_ARCH >= 9.
- `panvk_shader.h`: descriptor metadata adds `uses_heaps` and embedded sampler private-memory/count state.
- `panvk_vX_cmd_desc_state.c`: resource-table allocation grows to `PANVK_HEAP_TABLE_END` when shader uses heaps; fills resource/sampler heap table entries; implements `CmdBindSamplerHeapEXT`, `CmdBindResourceHeapEXT`, and secondary-command-buffer heap inheritance.
- `panvk_vX_cmd_push_constant.c`: implements `CmdPushDataEXT` on PAN_ARCH >= 9; push data occupies push-constant space and is visible to all stages.
- `panvk_vX_nir_lower_descriptors.c`: lowers texture/image/buffer heap offsets to Mali resource handles and resource-table UBO loads. This file is 1,630 lines in the reference versus 1,432 lines in the project branch, so it contains other Mesa 27 changes too; isolate descriptor-heap hunks rather than replacing wholesale.
- `panvk_vX_shader.c`: detects `VK_SHADER_CREATE_DESCRIPTOR_HEAP_BIT_EXT`, packs embedded sampler descriptors, sets `desc_info.uses_heaps`, and uploads embedded sampler data.
- `panvk_vX_physical_device.c`: extension `.EXT_descriptor_heap = PAN_ARCH >= 9`; feature `.descriptorHeap = PAN_ARCH >= 9`, capture/replay false; properties include 64-byte heap alignment, heap size derived from 32-byte descriptor stride and 24-bit handles, 256-byte push data, 2,048 embedded samplers, and sparse/protected heaps false. It also sets `minStorageBufferOffsetAlignment = 4` on PAN_ARCH >= 9 with a comment explaining the measured vkd3d-proton offset requirement (G68, G720, G925, G1 Ultra).

### Current branch missing-path verification

The same named project files were fetched on `experiment/g720-clip-cull-capabilities`; none contains `descriptor_heap`, `DescriptorHeapEXT`, `uses_heaps`, or `CmdPushDataEXT` markers. The project branch's descriptor-state file is 450 lines vs 542 in reference; push-constant file 124 vs 151; descriptor layout 212 vs 220; shader compiler 3,716 vs 4,258; descriptor NIR lowering 1,432 vs 1,630; physical device 1,433 vs 1,535. These size differences are not all attributable to descriptor heaps; use targeted hunks and API adaptation.

### Confirmed generic support already in project tree

- Runtime `vk_nir_lower_descriptor_heaps.c` exists and is built (1,107 lines in project vs 1,146 in reference).
- Its header is byte-identical to the reference header.
- SPIR-V parser recognizes `DescriptorHeapEXT`.

### Next implementation order

1. Verify registry/header generation source and `vk_device_extension_table` member availability in the 26.3 tree. Current `include/vulkan/vulkan_core.h` GitHub contents fetch returned a zero-byte blob, so don't infer absence/presence from that fetch; inspect the actual local generated header or generator input.
2. Fetch both versions of each PanVK file and isolate reference additions by semantic marker, then adapt the descriptor-state ABI and resource table indices.
3. Add shader heap offset lowering and resource-handle conversion in the existing PanVK NIR pass.
4. Add heap binding/inheritance and push-data entrypoints, then shader metadata and embedded samplers.
5. Add feature/property query only after implementation and generated extension/entrypoint wiring are confirmed.
6. Build all PAN_ARCH variants used by the build and run Vulkan CTS descriptor-heap tests plus VKD3D-Proton D3D12.

No descriptor-heap feature has been exposed or code changed yet. The extension remains deliberately disabled until this sequence is completed.


## Descriptor heap lowering contract (source-level reference audit)

The JimVulkan Valhall reference implementation was inspected at `58f2c55854bd48b18df756edb11f187af9cd2fde`. This pins down the port contract beyond the previous high-level file map:

| Layer | Required behavior | Port status |
|---|---|---|
| API generation | `VK_EXT_descriptor_heap` registry types, feature/property structs, command prototypes and generated entrypoints | Not yet verified in the local 26.3 generated headers/registry |
| Shader compilation | invoke generic `vk_nir_lower_descriptor_heaps()` and preserve shader create flags / embedded sampler metadata | Generic runtime + SPIR-V parser exist; PanVK call site not present |
| Texture NIR | heap byte offset / 32 becomes Mali resource-table handle; texture and sampler heap sources become native Mali handles | Missing in project branch |
| Buffer NIR | `load_heap_descriptor` becomes `(handle,0,0)`; heap data reads use resource-table UBO access | Missing in project branch |
| Image NIR | image heap load/store/atomic/size/sample intrinsics rewrite through normal image lowering | Missing in project branch |
| Resource table | reserve heap resource, heap sampler and embedded sampler entries after ordinary sets; extend allocation for `uses_heaps` shaders | Missing in project branch |
| Command state | bind resource/sampler heap into graphics + compute descriptor state; inherit into secondary command buffers | Missing in project branch |
| Push data | `CmdPushDataEXT` copies into push-constant backing and dirties all stage push-uniform state; validate overflow-safe range | Missing in project branch |
| Embedded samplers | default sampler + embedded sampler array uploaded with correct alignment; lifecycle and binary cache serialization/deserialization | Missing in project branch |
| Feature/property | advertise only after all above paths work; 64-byte heap alignment, 32-byte descriptors, 24-bit handle range, 256-byte push data, 2048 embedded samplers in reference | Disabled in project branch |

Important implementation invariant: the 32-byte descriptor stride, resource-table index allocation, NIR handle arithmetic, and max-heap-size property must agree exactly. A mismatch can silently address the wrong descriptor. The reference's `minStorageBufferOffsetAlignment=4` is supported by its measured Valhall compiler/hardware behavior and VKD3D-Proton requirements, but must be separately validated in this Mesa 26.3 source and on G720 before adopting.

Next: verify generated Vulkan API inputs and inspect the generic lowerer's call contract; then port as a coherent feature series, not independent feature-bit flips.


## Correction: generic heap NIR lowering is called by the shared runtime

The current 26.3 project branch already calls `vk_nir_lower_descriptor_heaps()` in both `src/vulkan/runtime/vk_shader.c` and `src/vulkan/runtime/vk_pipeline.c`. The runtime extracts `VkShaderDescriptorSetAndBindingMappingInfoEXT` from shader create-info `pNext`, lowers NIR, removes dead uniform/image variables and runs DCE. Pipeline compilation additionally hashes descriptor heap mappings into the shader stage cache key. Therefore do **not** add a duplicate generic lowering call in PanVK. PanVK must instead implement backend heap-offset/handle lowering and consume the generic lowerer's `embedded_samplers_out` data through the shader compilation callback; verify callback compatibility before porting.


## Registry / dispatch integration remains a hard blocker

The experiment branch tracks `src/vulkan/registry/vk.xml` as a 3,258,412-byte blob (`4d66622efa2937668bc35cfcc114a302813828d9`). The GitHub connector cannot return its contents because it is oversized; previous empty file responses were transport limitations, not proof of an empty registry. The PanVK Meson file generates entrypoints from this registry. Exact descriptor-heap declaration presence therefore remains unverified. Current PanVK command state has no `CmdBindSamplerHeapEXT`, `CmdBindResourceHeapEXT`, `CmdPushDataEXT`, or heap inheritance implementation. A code-search no-match is non-authoritative because search indexes the default branch. Before any feature exposure, verify the local registry and generated dispatch/header files; if declarations are absent, update Vulkan-Headers/registry and generated code as a coherent Mesa change. No code change or package is ready.


## Descriptor heap status — 2026-11-05 follow-up

A separate implementation branch, `experiment/g720-descriptor-heaps`, now contains an initial Valhall-only `VK_EXT_descriptor_heap` backend port. This is a development status update, not yet a validated capability.

| Area | Initial implementation | Validation state |
|---|---|---|
| API exposure | `VK_EXT_descriptor_heap`, `descriptorHeap=true` on PAN_ARCH >= 9 | Pending Android compile |
| NIR lowering | Heap byte offsets to 32-byte Mali descriptor handles; buffer/image heap operations | Pending compiler validation |
| Resource table | Resource, sampler, embedded-sampler table entries after MAX_SETS | Pending compile/runtime tests |
| Commands | Bind sampler heap, bind resource heap, push data | Pending generated-entrypoint/link validation |
| Secondary buffers | Heap binding inheritance | Pending CTS/runtime validation |
| Embedded samplers | Default sampler + shader embedded samplers, per-variant storage | Pending cache and lifetime validation |
| Shader binary | Explicit uses-heaps bit + packed sampler data; binary version 1 | Pending serialization round-trip tests |
| Properties | 64-byte heap alignment, 32-byte descriptor size/alignment, 24-bit handle space, 256-byte push data; sparse/protected/capture-replay false | Pending compile and spec/CTS validation |

The port intentionally does not enable descriptor-heap sparse residency, protected heaps, or capture/replay. The source branch has a per-variant descriptor-info layout, unlike the Mesa 27 reference's per-shader layout; the first implementation duplicates embedded-sampler backing for variants. Consolidating immutable sampler backing is a review item before finalizing.

**Do not mark this extension production-ready or package it for Winlator until the Android ARM64 build, generated Vulkan API/entrypoint checks, NIR validation, and descriptor-heap runtime tests pass.** The stock Android driver remains untouched.


Compile follow-up: the first Android compile reached the PanVK sources and found one missing Mesa-26.3 sampler-state-to-Mali-descriptor helper required for embedded samplers. That helper and its declaration have now been ported; exact latest-HEAD build is pending. The earlier failed run does not count as descriptor-heap validation.


### Descriptor heap command completeness follow-up

The exact repository Vulkan registry was inspected. In addition to heap binding and push data, the current `VK_EXT_descriptor_heap` command set requires `vkWriteSamplerDescriptorsEXT`, `vkWriteResourceDescriptorsEXT`, and `vkGetImageOpaqueCaptureDataEXT`; `vkGetPhysicalDeviceDescriptorSizeEXT` is already handled by the shared runtime. The initial Mesa 27 PanVK reference omitted the two writer commands and capture-data command, so it was not a complete port.

The experimental G720 branch now has writer implementations for:
- samplers (Mali sampler descriptor packing);
- uniform/storage buffers;
- uniform/storage texel buffers;
- sampled/storage images and input attachments (including Valhall multi-plane descriptor output);
- null resource descriptors.

Image opaque capture returns `VK_ERROR_FEATURE_NOT_PRESENT` because capture/replay is not exposed. Acceleration-structure and ARM tensor descriptor writes are not supported and return `VK_ERROR_FEATURE_NOT_PRESENT`, consistent with those capabilities remaining disabled.

Source HEAD `c3f9a776cb59215a42e4c6c5579a1a90f1ae8312`; exact-head Android-only build run `42079149912` queued. This remains provisional pending compile/link and runtime tests.


Conditional custom-border-color commands required by the registry have also been added on Valhall. Since Mali sampler descriptors embed the full border-color value, registration/unregistration are valid no-op operations; requested in-range indices are preserved, and non-requested registration returns index 0. This is provisional until the latest Android build and runtime tests pass. Current source HEAD `c9377fc98595de9b873fcba5a6997c5bf514ab4c`.


Descriptor writer constraints checked against the current Khronos proposal: YCbCr and subsampled samplers are rejected by `vkWriteSamplerDescriptorsEXT` (they must be embedded); custom border colors are rejected for embedded samplers and require an explicit registered index for sampler-heap writes; YCbCr image descriptors use PanVK's per-format texture descriptor count. Fragment-density-map, QCOM image processing, acceleration structures, and ARM tensors are not exposed and their descriptor types remain unsupported. Source HEAD `0bf2e527c42fe06733d5c0b1557c9d2cec0c1412`; build pending.


YCbCr count consistency fix: `VkSamplerYcbcrConversionImageFormatProperties::combinedImageSamplerDescriptorCount` is now derived from `panvk_image_get_tex_count(model->arch, format)`, matching the descriptor writer and PanVK descriptor-set layout. Source HEAD `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2`; Android build pending.


Intermediate build evidence: Android-only run `42078972694` at source `3e4e7ee3849ebab12c114eef8c6c3a650cd14b8f` passed compile, staging, ELF/libdrm audit, and package upload. It validates the initial writer-command implementation but predates custom-border command/restriction and YCbCr count fixes. Artifact `13438649591` is not the current test package. Exact latest source `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2` still awaits run `42080294652`.


## Exhaustive Vulkan initializer census — 2026-11-05

Source of truth: `src/panfrost/vulkan/panvk_vX_physical_device.c` on `experiment/g720-descriptor-heaps` at `65ea07c485e2a9a1d936ff4fc17c9f6d6e07f249` (source branch commit `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2`). These counts describe explicit C initializer assignments, not the number of extensions/features enabled at runtime on every architecture.

- Extension-table assignments: **186**
- Feature-structure assignments: **298** — 173 literal `true`, 70 literal `false`, 55 architecture/device/DRIRC-gated expressions.
- For the Dimensity 8400 Ultra's Mali-G720 (PanVK `PAN_ARCH=12`), gates such as `PAN_ARCH >= 9/10/11` evaluate true; `PAN_ARCH >= 13` does not. Device-property and DRIRC gates still require runtime values.

### All explicit extension-table assignments

| Extension field (without `VK_` prefix) | Source expression |
|---|---|
| `KHR_8bit_storage` | `true` |
| `KHR_16bit_storage` | `true` |
| `KHR_shader_atomic_int64` | `PAN_ARCH >= 9` |
| `KHR_bind_memory2` | `true` |
| `KHR_buffer_device_address` | `true` |
| `KHR_calibrated_timestamps` | `device->kmod.dev->props.gpu_can_query_timestamp` |
| `KHR_compute_shader_derivatives` | `PAN_ARCH >= 9` |
| `KHR_copy_commands2` | `true` |
| `KHR_copy_memory_indirect` | `PAN_ARCH >= 10` |
| `KHR_create_renderpass2` | `true` |
| `KHR_dedicated_allocation` | `true` |
| `KHR_descriptor_update_template` | `true` |
| `KHR_depth_clamp_zero_one` | `true` |
| `KHR_depth_stencil_resolve` | `true` |
| `KHR_device_group` | `true` |
| `KHR_draw_indirect_count` | `PAN_ARCH >= 10` |
| `KHR_driver_properties` | `true` |
| `KHR_dynamic_rendering` | `true` |
| `KHR_dynamic_rendering_local_read` | `true` |
| `KHR_external_fence` | `true` |
| `KHR_external_fence_fd` | `true` |
| `KHR_external_memory` | `true` |
| `KHR_external_memory_fd` | `true` |
| `KHR_external_semaphore` | `true` |
| `KHR_external_semaphore_fd` | `true` |
| `KHR_format_feature_flags2` | `true` |
| `KHR_get_memory_requirements2` | `true` |
| `KHR_global_priority` | `true` |
| `KHR_image_format_list` | `true` |
| `KHR_imageless_framebuffer` | `true` |
| `KHR_index_type_uint8` | `true` |
| `KHR_internally_synchronized_queues` | `true` |
| `KHR_line_rasterization` | `true` |
| `KHR_load_store_op_none` | `true` |
| `KHR_maintenance1` | `true` |
| `KHR_maintenance2` | `true` |
| `KHR_maintenance3` | `true` |
| `KHR_maintenance4` | `true` |
| `KHR_maintenance5` | `true` |
| `KHR_maintenance6` | `true` |
| `KHR_maintenance7` | `true` |
| `KHR_maintenance8` | `true` |
| `KHR_maintenance9` | `true` |
| `KHR_map_memory2` | `true` |
| `KHR_multiview` | `true` |
| `KHR_pipeline_binary` | `true` |
| `KHR_pipeline_executable_properties` | `true` |
| `KHR_pipeline_library` | `true` |
| `KHR_push_descriptor` | `true` |
| `KHR_relaxed_block_layout` | `true` |
| `KHR_robustness2` | `PAN_ARCH >= 10` |
| `KHR_sampler_mirror_clamp_to_edge` | `true` |
| `KHR_sampler_ycbcr_conversion` | `true` |
| `KHR_separate_depth_stencil_layouts` | `true` |
| `KHR_shader_clock` | `device->kmod.dev->props.gpu_can_query_timestamp` |
| `KHR_shader_draw_parameters` | `true` |
| `KHR_shader_expect_assume` | `true` |
| `KHR_shader_float_controls` | `true` |
| `KHR_shader_float_controls2` | `true` |
| `KHR_shader_float16_int8` | `true` |
| `KHR_shader_fma` | `true` |
| `KHR_shader_integer_dot_product` | `true` |
| `KHR_shader_maximal_reconvergence` | `PAN_ARCH >= 9` |
| `KHR_shader_non_semantic_info` | `true` |
| `KHR_shader_quad_control` | `PAN_ARCH >= 9` |
| `KHR_shader_relaxed_extended_instruction` | `true` |
| `KHR_shader_subgroup_extended_types` | `true` |
| `KHR_shader_subgroup_rotate` | `true` |
| `KHR_shader_subgroup_uniform_control_flow` | `PAN_ARCH >= 9` |
| `KHR_shader_terminate_invocation` | `true` |
| `KHR_shader_untyped_pointers` | `PAN_ARCH >= 9` |
| `KHR_spirv_1_4` | `true` |
| `KHR_storage_buffer_storage_class` | `true` |
| `KHR_present_id` | `true` |
| `KHR_present_id2` | `true` |
| `KHR_present_wait` | `true` |
| `KHR_present_wait2` | `true` |
| `KHR_swapchain` | `true` |
| `KHR_swapchain_mutable_format` | `true` |
| `KHR_swapchain_maintenance1` | `true` |
| `KHR_synchronization2` | `true` |
| `KHR_timeline_semaphore` | `true` |
| `KHR_unified_image_layouts` | `true` |
| `KHR_uniform_buffer_standard_layout` | `true` |
| `KHR_variable_pointers` | `true` |
| `KHR_vertex_attribute_divisor` | `true` |
| `KHR_vulkan_memory_model` | `true` |
| `KHR_workgroup_memory_explicit_layout` | `true` |
| `KHR_zero_initialize_workgroup_memory` | `true` |
| `EXT_4444_formats` | `true` |
| `EXT_attachment_feedback_loop_dynamic_state` | `true` |
| `EXT_attachment_feedback_loop_layout` | `true` |
| `EXT_border_color_swizzle` | `true` |
| `EXT_buffer_device_address` | `true` |
| `EXT_calibrated_timestamps` | `device->kmod.dev->props.gpu_can_query_timestamp` |
| `EXT_conditional_rendering` | `PAN_ARCH >= 10` |
| `EXT_color_write_enable` | `true` |
| `EXT_conservative_rasterization` | `PAN_ARCH >= 11` |
| `EXT_custom_border_color` | `true` |
| `EXT_debug_marker` | `true` |
| `EXT_depth_bias_control` | `true` |
| `EXT_depth_clamp_control` | `true` |
| `EXT_depth_clamp_zero_one` | `true` |
| `EXT_depth_clip_enable` | `true` |
| `EXT_depth_clip_control` | `true` |
| `EXT_device_address_binding_report` | `true` |
| `EXT_device_memory_report` | `true` |
| `EXT_descriptor_heap` | `PAN_ARCH >= 9` |
| `EXT_display_control` | `true` |
| `EXT_descriptor_indexing` | `PAN_ARCH >= 9` |
| `EXT_dynamic_rendering_unused_attachments` | `true` |
| `EXT_extended_dynamic_state` | `true` |
| `EXT_extended_dynamic_state2` | `true` |
| `EXT_extended_dynamic_state3` | `true` |
| `EXT_external_memory_acquire_unmodified` | `true` |
| `EXT_external_memory_dma_buf` | `true` |
| `EXT_global_priority` | `true` |
| `EXT_global_priority_query` | `true` |
| `EXT_graphics_pipeline_library` | `true` |
| `EXT_hdr_metadata` | `true` |
| `EXT_host_image_copy` | `true` |
| `EXT_host_query_reset` | `true` |
| `EXT_image_2d_view_of_3d` | `true` |
| `EXT_image_drm_format_modifier` | `true` |
| `EXT_image_robustness` | `true` |
| `EXT_image_sliced_view_of_3d` | `true` |
| `EXT_image_view_min_lod` | `true` |
| `EXT_index_type_uint8` | `true` |
| `EXT_legacy_dithering` | `true` |
| `EXT_line_rasterization` | `true` |
| `EXT_load_store_op_none` | `true` |
| `EXT_map_memory_placed` | `true` |
| `EXT_nested_command_buffer` | `PAN_ARCH >= 10` |
| `EXT_memory_budget` | `true` |
| `EXT_non_seamless_cube_map` | `true` |
| `EXT_mutable_descriptor_type` | `PAN_ARCH >= 9` |
| `EXT_multisampled_render_to_single_sampled` | `true` |
| `EXT_physical_device_drm` | `true` |
| `EXT_pipeline_creation_cache_control` | `true` |
| `EXT_pipeline_creation_feedback` | `true` |
| `EXT_pipeline_robustness` | `true` |
| `EXT_present_timing` | `device->kmod.dev->props.gpu_can_query_timestamp` |
| `EXT_private_data` | `true` |
| `EXT_primitive_topology_list_restart` | `true` |
| `EXT_provoking_vertex` | `true` |
| `EXT_queue_family_foreign` | `true` |
| `EXT_rasterization_order_attachment_access` | `PAN_ARCH >= 10` |
| `EXT_rgba10x6_formats` | `PAN_ARCH >= 11` |
| `EXT_robustness2` | `PAN_ARCH >= 10` |
| `EXT_sampler_filter_minmax` | `PAN_ARCH >= 10` |
| `EXT_scalar_block_layout` | `true` |
| `EXT_separate_stencil_usage` | `true` |
| `EXT_shader_atomic_float` | `true` |
| `EXT_shader_demote_to_helper_invocation` | `true` |
| `EXT_shader_image_atomic_int64` | `PAN_ARCH >= 9` |
| `EXT_shader_module_identifier` | `true` |
| `EXT_shader_replicated_composites` | `true` |
| `EXT_shader_stencil_export` | `true` |
| `EXT_shader_subgroup_ballot` | `true` |
| `EXT_shader_subgroup_vote` | `true` |
| `EXT_shader_tile_image` | `PAN_ARCH >= 9` |
| `EXT_shader_uniform_buffer_unsized_array` | `true` |
| `EXT_subgroup_size_control` | `true` |
| `EXT_swapchain_maintenance1` | `true` |
| `EXT_texel_buffer_alignment` | `true` |
| `EXT_transform_feedback` | `PAN_ARCH >= 10` |
| `EXT_astc_decode_mode` | `PAN_ARCH >= 9` |
| `EXT_texture_compression_astc_hdr` | `true` |
| `EXT_tooling_info` | `true` |
| `EXT_vertex_attribute_divisor` | `true` |
| `EXT_vertex_input_dynamic_state` | `true` |
| `EXT_ycbcr_2plane_444_formats` | `PAN_ARCH >= 10` |
| `EXT_ycbcr_image_arrays` | `PAN_ARCH >= 10` |
| `EXT_zero_initialize_device_memory` | `true` |
| `EXT_inline_uniform_block` | `true` |
| `ANDROID_external_memory_android_hardware_buffer` | `has_gralloc` |
| `ANDROID_native_buffer` | `has_gralloc` |
| `GOOGLE_decorate_string` | `true` |
| `GOOGLE_display_timing` | `wsi_instance_supports_google_display_timing(&instance->vk, &instance->drirc.options)` |
| `GOOGLE_hlsl_functionality1` | `true` |
| `GOOGLE_user_type` | `true` |
| `VALVE_mutable_descriptor_type` | `PAN_ARCH >= 9` |
| `ARM_shader_core_builtins` | `true` |
| `ARM_shader_core_properties` | `true` |
| `ARM_scheduling_controls` | `true` |
| `ARM_rasterization_order_attachment_access` | `PAN_ARCH >= 10` |

### All literal-false feature fields

These are disabled in the initializer. A false bit is not by itself proof of a hardware limitation; each candidate must be traced through hardware, compiler lowering, descriptor/command semantics, and CTS before enabling.

`depthBounds`, `alphaToOne`, `multiViewport`, `pipelineStatisticsQuery`, `shaderTessellationAndGeometryPointSize`, `shaderStorageImageMultisample`, `shaderClipDistance`, `shaderCullDistance`, `shaderFloat64`, `shaderResourceResidency`, `shaderResourceMinLod`, `sparseResidency2Samples`, `sparseResidency4Samples`, `sparseResidency8Samples`, `sparseResidency16Samples`, `sparseResidencyAliased`, `variableMultisampleRate`, `inheritedQueries`, `multiviewGeometryShader`, `multiviewTessellationShader`, `protectedMemory`, `descriptorHeapCaptureReplay`, `bufferDeviceAddressMultiDevice`, `shaderOutputViewportIndex`, `shaderOutputLayer`, `decodeModeSharedExponent`, `smoothLines`, `stippledRectangularLines`, `stippledBresenhamLines`, `stippledSmoothLines`, `pipelineProtectedAccess`, `leastRepresentableValueForceUnormRepresentation`, `floatRepresentation`, `extendedDynamicState2PatchControlPoints`, `extendedDynamicState3TessellationDomainOrigin`, `extendedDynamicState3PolygonMode`, `extendedDynamicState3AlphaToOneEnable`, `extendedDynamicState3RasterizationStream`, `extendedDynamicState3ExtraPrimitiveOverestimationSize`, `extendedDynamicState3SampleLocationsEnable`, `extendedDynamicState3ColorBlendAdvanced`, `extendedDynamicState3ProvokingVertexMode`, `extendedDynamicState3LineStippleEnable`, `extendedDynamicState3DepthClipNegativeOneToOne`, `extendedDynamicState3ViewportWScalingEnable`, `extendedDynamicState3ViewportSwizzle`, `extendedDynamicState3CoverageToColorEnable`, `extendedDynamicState3CoverageToColorLocation`, `extendedDynamicState3CoverageModulationMode`, `extendedDynamicState3CoverageModulationTableEnable`, `extendedDynamicState3CoverageModulationTable`, `extendedDynamicState3CoverageReductionMode`, `extendedDynamicState3RepresentativeFragmentTestEnable`, `extendedDynamicState3ShadingRateImageEnable`, `geometryStreams`, `primitiveTopologyPatchListRestart`, `transformFeedbackPreservesProvokingVertex`, `robustImageAccess2`, `shaderBufferFloat32AtomicAdd`, `shaderBufferFloat64Atomics`, `shaderBufferFloat64AtomicAdd`, `shaderSharedFloat32AtomicAdd`, `shaderSharedFloat64Atomics`, `shaderSharedFloat64AtomicAdd`, `shaderImageFloat32AtomicAdd`, `sparseImageFloat32AtomicAdd`, `sparseImageInt64Atomics`, `memoryMapRangePlaced`, `unifiedImageLayoutsVideo`, `shaderFmaFloat64`

### All non-literal feature gates

| Feature field | Source expression |
|---|---|
| `multiDrawIndirect` | `PAN_ARCH >= 10` |
| `textureCompressionETC2` | `has_texture_compression_etc2(device)` |
| `textureCompressionASTC_LDR` | `has_texture_compression_astc_ldr(device)` |
| `textureCompressionBC` | `has_texture_compression_bc(device)` |
| `vertexPipelineStoresAndAtomics` | `(PAN_ARCH >= 13 && instance->drirc.misc.enable_vertex_pipeline_stores_atomics) || instance->drirc.misc.force_enable_shader_atomics` |
| `sparseBinding` | `has_sparse` |
| `sparseResidencyBuffer` | `has_sparse` |
| `sparseResidencyImage2D` | `has_sparse` |
| `sparseResidencyImage3D` | `has_sparse` |
| `drawIndirectCount` | `PAN_ARCH >= 10` |
| `shaderBufferInt64Atomics` | `PAN_ARCH >= 9` |
| `shaderSharedInt64Atomics` | `PAN_ARCH >= 9` |
| `shaderFloat16` | `PAN_ARCH >= 10` |
| `descriptorIndexing` | `PAN_ARCH >= 9` |
| `descriptorHeap` | `PAN_ARCH >= 9` |
| `descriptorBindingUniformBufferUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingSampledImageUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingStorageImageUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingStorageBufferUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingUniformTexelBufferUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingStorageTexelBufferUpdateAfterBind` | `PAN_ARCH >= 9` |
| `descriptorBindingUpdateUnusedWhilePending` | `PAN_ARCH >= 9` |
| `descriptorBindingPartiallyBound` | `PAN_ARCH >= 9` |
| `samplerFilterMinmax` | `PAN_ARCH >= 10` |
| `bufferDeviceAddressCaptureReplay` | `(PAN_ARCH >= 10) && (device->memory.max_supported_va > (1ull << 32))` |
| `textureCompressionASTC_HDR` | `has_texture_compression_astc_hdr(device)` |
| `computeDerivativeGroupQuads` | `PAN_ARCH >= 9` |
| `computeDerivativeGroupLinear` | `PAN_ARCH >= 9` |
| `extendedDynamicState3ConservativeRasterizationMode` | `PAN_ARCH >= 11` |
| `indirectMemoryCopy` | `PAN_ARCH >= 10` |
| `conditionalRendering` | `PAN_ARCH >= 10` |
| `inheritedConditionalRendering` | `PAN_ARCH >= 10` |
| `transformFeedback` | `PAN_ARCH >= 10` |
| `customBorderColorWithoutFormat` | `PAN_ARCH != 7` |
| `robustBufferAccess2` | `PAN_ARCH >= 11` |
| `nullDescriptor` | `PAN_ARCH >= 10` |
| `shaderTileImageColorReadAccess` | `PAN_ARCH >= 9` |
| `shaderTileImageDepthReadAccess` | `PAN_ARCH >= 9` |
| `shaderTileImageStencilReadAccess` | `PAN_ARCH >= 9` |
| `shaderSubgroupClock` | `device->kmod.dev->props.gpu_can_query_timestamp` |
| `shaderDeviceClock` | `device->kmod.dev->props.timestamp_device_coherent` |
| `shaderUntypedPointers` | `PAN_ARCH >= 9` |
| `sparseImageFloat32Atomics` | `has_sparse` |
| `shaderImageInt64Atomics` | `PAN_ARCH >= 9` |
| `formatRgba10x6WithoutYCbCrSampler` | `PAN_ARCH >= 11` |
| `ycbcr2plane444Formats` | `PAN_ARCH >= 10` |
| `ycbcrImageArrays` | `PAN_ARCH >= 10` |
| `nestedCommandBuffer` | `PAN_ARCH >= 10` |
| `nestedCommandBufferRendering` | `PAN_ARCH >= 10` |
| `nestedCommandBufferSimultaneousUse` | `PAN_ARCH >= 10` |
| `mutableDescriptorType` | `PAN_ARCH >= 9` |
| `shaderFmaFloat16` | `PAN_ARCH >= 10` |
| `rasterizationOrderColorAttachmentAccess` | `PAN_ARCH >= 10` |
| `rasterizationOrderDepthAttachmentAccess` | `PAN_ARCH >= 10` |
| `rasterizationOrderStencilAttachmentAccess` | `PAN_ARCH >= 10` |

### Prioritized false-feature audit queue

The following are candidates for a deeper source/hardware audit, not instructions to force-enable them:

1. **Software/compiler-emulatable candidates:** `multiViewport`, `shaderClipDistance`, `shaderCullDistance`, `pipelineStatisticsQuery`, `shaderResourceMinLod`, `shaderOutputViewportIndex`, `shaderOutputLayer`, `inheritedQueries`.
2. **Hardware/ISA confirmation required:** `depthBounds`, `alphaToOne`, `geometryStreams`, `shaderStorageImageMultisample`, `transformFeedbackPreservesProvokingVertex`, float32 atomic-add feature bits.
3. **Likely architectural/stack limitations unless new evidence appears:** sparse residency/binding (Kbase reports `has_sparse=false`), protected memory, float64 shader support, multisample variable-rate behavior, and unsupported line-stipple modes.

Specific safety gates already established:
- `vertexPipelineStoresAndAtomics` is only enabled on `PAN_ARCH >= 13` with the DRIRC opt-in, or with the explicit force option. G720 is v12; leave disabled because invalid-index speculative accesses remain unsafe.
- `sparseBinding` and sparse residency feature bits depend on `has_sparse`; current Kbase path reports false, so do not expose sparse features.
- `descriptorHeapCaptureReplay` remains false; image opaque-capture command returns `VK_ERROR_FEATURE_NOT_PRESENT`.
- `shaderFloat64` and related float64 atomic/FMA fields remain false; do not infer support from unrelated FP16/FP32 features.

This census is the baseline for the remaining "enable every genuinely supported feature" audit. It supersedes earlier approximate counts of 185 extension and 296 feature initializers.
