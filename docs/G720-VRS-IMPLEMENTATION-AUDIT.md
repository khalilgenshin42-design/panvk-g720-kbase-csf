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


### Follow-up compiler-path inspection

A closer trace narrowed the likely coordinate discrepancy to the existing
Bifrost/Valhall sample-position lowering:

- `src/panfrost/compiler/pan_nir_lower_sample_position.c` handles
  `nir_intrinsic_load_sample_pos_or_center`. When sample shading is
  disabled, it unconditionally replaces the value with `(0.5, 0.5)`.
- PanVK's static FSR pass runs earlier in `panvk_compile_nir()` and replaces
  that intrinsic with a rate-derived center: for 2x2, `(1.0, 1.0)`.
- For dynamic FSR, the static pass deliberately declines the shader. The
  Bifrost lowering then supplies `(0.5, 0.5)`, even though the CSF draw path
  programs a dynamic 2x2 tiler rate.

This is a concrete, source-supported mechanism consistent with the observed
result: static and dynamic 2x2 both execute 1024 fragment invocations and
produce 4096 equal-neighbour pairs, but their coordinate-derived output
checksums differ. It is still a leading diagnosis, not a proven complete
root cause, because the current shader's only pixel signature is derived
from `gl_FragCoord`.

The next probe should use a second fragment shader that performs the same
SSBO atomic invocation count but writes a constant colour and never reads
`gl_FragCoord`. Run static 2x2 and dynamic 2x2 with that shader, then retain
the current coordinate-reading shader as a separate test. If constant-colour
outputs match while coordinate outputs differ, the problem is isolated to
fragment-coordinate/sample-centre semantics. If they still differ, inspect
other dynamic state / shader differences before changing sample-position
lowering.

Do not simply change the dynamic centre to `(1.0, 1.0)` globally: dynamic
FSR can select multiple fragment sizes, and Vulkan's required center must
follow the actual effective rate. Any fix must handle all supported sizes
and interactions with multisampling/sample shading, not just this 2x2 case.


## 2026-11-08 — dynamic FSR sample-centre isolation probe

The device run of `--case=dynamic-keep` passed command recording, queue
submission, fence wait, readback, and fragment-invocation comparison (1024
static versus 1024 dynamic), but its coordinate-derived pixel checksum did
not match static 2x2. Source inspection identifies a plausible compiler
split: static FSR folds the fragment centre into NIR, whereas dynamic FSR
bypasses that static lowering; the generic sample-position lowering may then
supply the 1x1 centre. This is a hypothesis, not yet a confirmed root cause.

A discriminating probe adds a second fragment shader that increments the same
SSBO invocation counter but writes a constant colour and does not read
`gl_FragCoord`. It compares static 2x2 and dynamic 2x2 output hashes and
invocation counts, while retaining the coordinate shader's static/dynamic
mismatch as the contrasting result. If constant-colour pixels and invocation
counts match while coordinate pixels differ, the evidence isolates the failure
to coordinate/sample-centre semantics rather than the rate state itself. If
either constant-colour comparison fails, investigate the broader dynamic
pipeline state path before touching compiler lowering.

No driver feature bits or Android driver source are changed by this probe.


The isolation harness was tightened before distribution: it now also records
and asserts that the coordinate-reading dynamic shader's fragment invocation
count equals the static 2x2 count. This avoids interpreting a pixel mismatch
without first proving the dynamic coordinate shader ran at the expected coarse
rate. The constant-colour path independently checks its own static/dynamic
invocation equality and equality to that same 2x2 baseline.


Final harness review found that two newly added diagnostic format strings
contained escaped backslash-n text rather than C newline escapes. They are now
corrected so each result is written as a separate readable log line. Latest
source commit for this correction: `09d9684e45201933f0301175dc912a4a39606067`.
A successful earlier compile exists, but its artifact predates the final
invocation-count assertion and newline correction and must not be used for the
next device run. Await the build for the latest commit before distributing.


## 2026-11-08 — final isolation probe artifact verified

The corrected Windows probe build completed successfully in GitHub Actions
run #38:
https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42479065982
Source: `09d9684e45201933f0301175dc912a4a39606067`.

Artifact `panvk-vrs-render-probe-win64` (ID `13616202931`) is available from
the run's Artifacts section. ZIP SHA-256:
`69d232372041beec38e9f7751e2cee53e23bc488cdfadedaf542bb77596911a3`.
Executable SHA-256:
`06b777aaf67f2bee2af879e1e973c448b71ea88bcb2dc054f4c51d567540dc92`.
Independent inspection confirmed the ZIP contains the x86-64 Windows PE
executable and `tools/run-vrs-dynamic-isolation.cmd`; executable strings
include both constant-colour and coordinate-shader invocation comparisons.

Device test instructions: extract the ZIP, open the extracted `tools` folder,
and run `run-vrs-dynamic-isolation.cmd`. This uses the currently installed
PanVK driver; do not install or replace the driver. Send only
`tools/panvk_vrs_dynamic_isolation.log` (or the generated
`tools/panvk_vrs_render_probe.log` if the copy step does not run). The run
should take only as long as the render/submit/readback sequence; wait for the
console to finish and the log file to appear. Do not run the older dynamic
check script for this test.


## 2026-11-08 — dynamic FSR isolation result: coordinate-dependent mismatch

The user ran the verified `--case=dynamic-isolation` probe on the existing
Android package `git-c9d61e10a9` (Mesa 26.3.0-devel, Mali-G720 MC7). The
result is decisive for separating hardware rate execution from shader
coordinate behavior:

| Test | Static 2x2 KEEP | Dynamic 2x2 KEEP | Result |
|---|---:|---:|---|
| Constant-colour checksum | `0x57ef86c490d1a383` | `0x57ef86c490d1a383` | Exact pixel match |
| Constant-colour fragment invocations | 1024 | 1024 | Match |
| Coordinate shader fragment invocations | 1024 | 1024 | Match |
| Coordinate shader pixels | — | checksum differs | Mismatch |

The earlier static controls also remain green: 1x1 KEEP executes 4096
fragment invocations; static 2x2 KEEP executes 1024 and passes its expected
pixel signature. Dynamic 2x2 command recording, queue submission, fence wait,
and readback all pass. The probe ends with `VRS GPU RENDER PASS` because its
isolation acceptance criteria pass; this does **not** mean coordinate
semantics are equivalent.

### What this proves

- Dynamic 2x2 KEEP is not merely accepted by the command recorder: it reaches
  the GPU and produces the same 4x fragment-invocation reduction as static
  2x2 KEEP.
- The dynamic path is not causing a general render-target corruption or
  constant-output mismatch: the constant-colour static/dynamic images are
  bit-identical.
- The remaining discrepancy is specifically exposed by the shader that reads
  `gl_FragCoord.xy`. This strongly supports a fragment-coordinate /
  sample-centre lowering defect, rather than a failure to program dynamic
  VRS state.

This is a narrower diagnosis than the previous test, but not yet proof that
the only affected observable is sample centre. The coordinate shader uses
`floor(gl_FragCoord.xy)` and a modulo-8 colour pattern; the next probe should
log concrete coordinate-derived pixel differences (including their spatial
distribution), rather than rely only on an aggregate checksum.

### Source trace and implementation constraint

At the tested source checkpoint:

- `panvk_get_static_pipeline_vrs_rate()` intentionally rejects
  `MESA_VK_DYNAMIC_FSR`.
- Static FSR lowering replaces
  `nir_intrinsic_load_sample_pos_or_center` with a rate-derived center
  before `pan_postprocess_nir()`. A 2x2 rate resolves to (1.0, 1.0).
- `pan_nir_lower_sample_pos_impl()` later lowers
  `load_sample_pos_or_center` to (0.5, 0.5) when sample shading is disabled.
  Dynamic FSR currently bypasses the static replacement, so it follows this
  generic path.
- CSF draw emission reads the current dynamic FSR state from
  `dyns->fsr` and writes the encoded rate and combiner operations into
  `IDVS.TILER_FLAGS2`. The device counter result independently confirms
  that this state reaches the hardware.

A correct fix cannot hardcode (1.0, 1.0) for dynamic draws. Dynamic FSR is
command-buffer state and can change between draws while the fragment shader
pipeline remains bound. The implementation must make the effective
fragment-centre information available at draw execution time, or use a
correct hardware-provided fragment-coordinate/sysval path. It must preserve
sample-shading and multisample behavior. A candidate per-draw FAU/sysval
plumbing change requires auditing the shader FAU ABI, command-buffer draw
state updates, pipeline hashing/cache behavior, and all draw paths before
implementation.

### Next engineering gate

1. Extend the diagnostic output to identify the actual spatial/coordinate
   delta between static and dynamic images (not just a checksum).
2. Audit whether Panfrost's Bifrost/Valhall backend can consume a hardware
   fragment-rate/centre sysval. If not, design a per-draw rate sysval that is
   updated whenever dynamic FSR state changes and is available to fragment
   shaders without recompilation.
3. Test all advertised pipeline fragment sizes and sample-count interactions
   before exposing dynamic FSR as fully correct.
4. Keep primitive and attachment FSR features false; do not change advertised
   feature bits based on this result.

No Android driver source or package was changed by this device test. No new
Android driver install is requested yet.


### Follow-up source audit — per-draw FAU route identified (design only)

The next source pass found a plausible implementation route without changing
driver code:

- `struct panvk_graphics_sysvals` in `panvk_shader.h` already carries
  fragment-stage driver values in its `fs` block.
- `panvk_per_arch(cmd_prepare_draw_sysvals)` in
  `panvk_vX_cmd_draw.c` builds the per-draw graphics sysval update set and
  intersects dirty sysvals with `fs->fau.used_sysvals` to set
  `FS_PUSH_UNIFORMS`.
- CSF `prepare_push_uniforms()` uploads the fragment FAU block when
  `FS_PUSH_UNIFORMS` is dirty, then updates `IDVS.FRAGMENT_FAU`.
- The current CSF draw path reads `dyns->fsr` at draw emission and programs
  `IDVS.TILER_FLAGS2`. The same effective dynamic rate can therefore be
  encoded into a driver sysval at draw preparation time, provided the shader
  compiler marks that sysval as used and the dirty-state propagation is
  correct.

Candidate design: add a compact per-draw effective fragment-rate field to the
fragment sysval ABI; lower the dynamic fragment-centre input to a read of that
field; populate it from the effective Vulkan dynamic FSR state in
`cmd_prepare_draw_sysvals`; and ensure it is refreshed on every relevant
dynamic-state change before fragment FAU upload. The encoding must match the
rate bitfield consumed by the static lowering and be validated against the
combiner semantics. This is a design direction, not a patch: the helper
macros, sysval remapping/usage accounting, dynamic-state dirty enum, indirect
draw behavior, and all CSF draw paths still need a complete audit.

A particularly important correctness constraint is that sample shading forces
hardware VRS back to 1x1 in the CSF draw emitter. The shader-side effective
rate must use the same rule, not merely copy `dyns->fsr.fragment_size`.
Likewise, unsupported or non-pipeline combiner sources must remain fail-closed.


## 2026-11-08 — dynamic FSR sample-centre sysval implementation (experimental)

The coordinate-only mismatch from the dynamic isolation probe has now been addressed in a separate experimental branch, without changing the known-good experiment/g720-fragment-shading-rate branch or the installed Android driver.

Source changes on experiment/g720-dynamic-fsr-sysval:

- panvk_graphics_sysvals.fs now has an aligned 32-bit frag_rate field. Its encoding matches the existing PanVK VRS encoding: log2(width) in bits 2..3 and log2(height) in bits 0..1.
- cmd_prepare_draw_sysvals() refreshes this value from the current dynamic graphics FSR state for every graphics draw preparation. It writes 1x1 when sample shading is enabled, matching the existing CSF TILER_FLAGS2 path. It currently only derives the sysval for KEEP/KEEP combiner state, which is the valid pipeline-only configuration with primitive and attachment FSR features disabled.
- Dynamic fragment shaders replace nir_intrinsic_load_sample_pos_or_center with a load of the per-draw FAU sysval, then derive the fragment centre from the effective rate. This keeps the rate dynamic without recompiling the shader.
- Shader cache hashing now distinguishes static and dynamic FSR variants and includes the effective static rate when that rate is folded into NIR. This prevents static pipelines with different compile-time centres from aliasing the same shader cache key.

The existing draw preparation function is called from ordinary draws, tessellation paths, GS paths, and the meta draw path; the sysval update is therefore placed at the shared per-draw update point rather than in only the ordinary draw entry point.

### Validation still required

This is a source patch, not a validated driver build. The next gate is a successful Android/AArch64 PanVK build, followed by package audit. Then run the existing dynamic-isolation render probe with:
1. static 2x2 KEEP as the reference;
2. dynamic 2x2 KEEP with the coordinate-reading fragment shader;
3. dynamic 1x1 then dynamic 2x2 in the same command stream/pipeline to prove the FAU value refreshes between draws;
4. sample-shading enabled to prove the effective sysval remains 1x1.

Do not replace the installed driver until the new package passes the build, ELF/dependency audit, import test, and device render checks. Primitive and attachment FSR remain unexposed. The source branch is experimental and has not yet passed compilation or device validation.


## 2026-11-16 — dynamic sysval build/package audit and VRS capability correction

This entry supersedes the preceding statement that the per-draw sysval patch had not yet compiled. It does **not** claim target-device runtime validation.

### Current branch and source review

- Current branch HEAD: `experiment/g720-dynamic-fsr-sysval` at `c4f091e86c29eae88d2a659813d80b416f95fbf0`. The VRS branch `experiment/g720-fragment-shading-rate` at `95f6ea30a26790d329ce66f149f15d31665ca49e` is an ancestor; the dynamic-FSR branch contains the per-draw sysval implementation and subsequent probe work.
- `panvk_graphics_sysvals.fs.frag_rate` stores log2(width) in bits 2..3 and log2(height) in bits 0..1. `lower_dynamic_vrs_frag_center()` loads this FAU sysval and derives the dynamic fragment centre without recompiling for each rate.
- `cmd_prepare_draw_sysvals()` updates it from current dynamic FSR state at draw preparation, only for KEEP/KEEP (the valid pipeline-only configuration while primitive and attachment FSR remain disabled). It selects 1x1 when sample shading is enabled, matching CSF draw emission.
- The shader cache key distinguishes dynamic FSR from static FSR and includes the effective static rate for static lowering.
- The helper is called from the inspected CSF ordinary, indirect, tessellation, geometry-shader and software-stage draw paths. Source inspection supports coverage of the identified draw paths; it does not prove correct GPU execution or every possible command-buffer edge case.
- Primitive FSR and attachment FSR remain disabled; non-trivial combiner operations remain unadvertised. The extension is still experimental.

### CI and static package evidence

- Exact-head Android-only run [42645674486](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674486): SUCCESS at `c4f091e`.
- Exact-head wrapper run [42645674848](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674848): Release and Debugoptimized SUCCESS at `c4f091e`.
- Exact-head Windows probe build [42645674514](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674514): SUCCESS.
- Android package artifact [13688885153](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674486/artifacts/13688885153): outer ZIP SHA-256 `fbfa977275e8d54ffe176c8ff9999ff706166ae7b55b41a6a553975e8aca4fc2`; nested flat package SHA-256 `02691a2f06125d4ffd7d0725fb31c99a5d54bc1f4b32b819cfb57fc0d800b7ff`. ZIP integrity and all embedded member hashes pass. Metadata identifies `26.3.0-devel-android-kbase-c4f091e86c29`, min API 30. Driver and private DRM helper are ELF64 AArch64 Android API 30; driver RUNPATH is `$ORIGIN`; private `libpanvk_drm.so` has only `libc.so` as NEEDED. The package audit reports 24 undefined DRM imports, 212 private definitions and zero missing DRM symbols.
- Windows probe artifact [13685924631](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674514/artifacts/13685924631): outer ZIP SHA-256 `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`; PE32+ x86-64 probe SHA-256 `5e0526c4735fed05a83647825a210653bf83740dc403a94128a8141d6e9a639b`. It contains `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd`.

Classification: exact-head CI and static package validation passed. No new Android/Winlator runtime test, Vulkan initialization, render probe log, stability result or FPS result was available for the per-draw sysval implementation.

### Additional source-audit finding: advertised VRS sample-count mask

The current `panvk_GetPhysicalDeviceFragmentShadingRatesKHR()` table has two inconsistencies:

- The comment says the larger `4x4/4x2/2x4/1x2` rates are exposed at 1x MSAA only, but the `2x4` and `1x2` entries also include `VK_SAMPLE_COUNT_2_BIT`.
- The `1x1` entry uses `~0u`, which includes undefined/reserved sample-count flag bits and is broader than the actual device framebuffer sample-count mask returned by `panvk_get_sample_counts()`.

A conservative correction was started on isolated branch `work/vrs-rate-sample-count-fix`, based on `c4f091e`:

- Derive the 1x1 rate's `sampleCounts` from the same `panvk_get_sample_counts(arch, max_tib_size, max_cbuf_atts, 16)` calculation used for physical-device framebuffer limits.
- Restrict `2x4` and `1x2` to 1x MSAA, matching the existing source comment until target-device MSAA evidence justifies more.
- Add Android-only workflow assertions against reintroducing `~0u` or the unverified 2x MSAA entries.

The correction commit is `385e51bb951829e95c17d55410855f9cb982ca80`. Its Android-only and wrapper CI runs are pending at the time of this entry; the probe-build job has succeeded. Do not use its package until both exact-head build workflows finish and its artifact is audited.

### Required device validation

After the sample-count correction's exact-head CI and package audit complete, import that experimental package only through Winlator Mali's custom-driver importer (keep System Driver as fallback; do not touch Android system/vendor Vulkan files or the kernel). Use the probe from artifact `13685924631`:

1. Run `Run-Dynamic-VRS.cmd` to exercise the coordinate-reading dynamic-KEEP case.
2. Run `tools/run-vrs-mip-pattern.cmd` to compare static and dynamic `1x1, 2x1, 1x2, 2x2, 4x2, 2x4, 4x4` spatial mip patterns, pixels and invocation counts.
3. Send the complete `panvk_vrs_render_probe.log`.

These probes do not yet cover changing 1x1 then 2x2 between draws in one command buffer with the same pipeline, nor the sample-shading-enabled fallback. Those remain explicit follow-up cases. No FPS gain or full VRS correctness is claimed.


## 2026-11-16 — sample-mask correction build failure and retry

The first sample-count correction commit `385e51bb951829e95c17d55410855f9cb982ca80` failed both Android and wrapper compilation. The compiler diagnostic was specific: `initializer element is not a compile-time constant` for `{{1, 1}, sample_counts}`, because the rate array was declared `static const` while its sample mask is computed at runtime.

The source was corrected on `work/vrs-rate-sample-count-fix` by removing `static` from the local rate-table declaration, retaining `const` automatic storage. Current retry HEAD: `c22dc2f595046ddf640ce8c148453f230cde7cfc`.

- Android-only retry [43902597004](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902597004): in progress at this note.
- Wrapper/probe retry [43902595797](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902595797): in progress at this note; the Windows probe-build job completed successfully, while Release and Debugoptimized wrapper builds were still compiling.

The failed first attempt is not a driver-runtime failure; it is a C storage-duration/initializer issue. No package was produced from the failed commit. Do not claim the correction builds until the retry's exact-head Android and both wrapper jobs complete.


## 2026-11-16 — VRS sample-count mask must also respect the 4x rasterization limit

A second audit of the sample-count correction found one remaining Vulkan property inconsistency in the `work/vrs-rate-sample-count-fix` candidate at `c22dc2f595046ddf640ce8c148453f230cde7cfc`.

The candidate correctly replaced the invalid `~0u` 1x1 mask with `panvk_get_sample_counts(...)`, and conservatively restricted the larger 4x4/4x2/2x4/1x2 rates to 1x MSAA. However, on the G720 model:

- `pan_query_tib_size()` returns the G720 TIB size of 32 KiB or 64 KiB.
- `pan_get_max_cbufs()` caps the color attachment count at 8.
- With `format_size = 16`, `pan_get_max_msaa()` computes at least 16x for either G720 TIB size, capped at 16x.
- Consequently `panvk_get_sample_counts(..., 16)` returns 1x, 2x, 4x, 8x and 16x bits on architecture v12.

The VRS physical-device properties still advertise `maxFragmentShadingRateRasterizationSamples = VK_SAMPLE_COUNT_4_BIT`. Vulkan's current valid-usage rule requires the highest bit in every returned rate's `sampleCounts` to be no greater than that property. See the [Vulkan fragment-shading-rate properties](https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceFragmentShadingRatePropertiesKHR.html) and [fragment-shading-rate query rules](https://docs.vulkan.org/spec/latest/chapters/primsrast.html).

Therefore the c22 1x1 entry still over-advertises 8x/16x for VRS. This is a source/property-query correctness issue, not a runtime failure. **Do not import or device-test the c22 package.**

A conservative follow-up was created on isolated branch `work/vrs-sample-count-rasterization-limit`, based on c22:

- Mask the 1x1 sampleCounts to 1x/2x/4x, matching the currently advertised 4x VRS rasterization limit.
- Clarify the source comment.
- Add Android workflow guards for the mask and the 4x property.
- Add the new branch to the Android-only workflow filter.

Current follow-up HEAD: `b1525b9a33923876f724a742aaebc7a7325a2a58`.

- Android-only exact-head run [43906343930](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343930): in progress at the time of this note.
- Wrapper/probe exact-head run [43906343923](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343923): in progress at the time of this note.

No package has been audited from this follow-up yet, and no new target-device VRS/MSAA test is available. Keep pipeline-only VRS and all other branches separate. The next device candidate must come from this follow-up only after both exact-head build workflows finish and the Android package passes independent hash/ELF/dependency checks.


### Follow-up — 2026-11-16: corrected sample-mask package built and statically audited

The isolated follow-up `work/vrs-sample-count-rasterization-limit` at exact HEAD `b1525b9a33923876f724a742aaebc7a7325a2a58` has now completed its Android-only package workflow successfully.

- Android run [43906343930](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343930): success; all 15 steps passed, including Android ARM64 compilation, staged install audit, flat package generation, and artifact upload.
- Artifact 14245318290 outer SHA-256: `ab4055b70929a2a64dbdec88ce6106ddf8e8709de61b5bb2837e4824255ecc1a` (16,822,686 bytes); ZIP integrity passes.
- Flat package SHA-256: `03d35c615bcd605a87cddfcaf3b9de9d90980f2f0ca6469fb5d05266176a5013`. Four members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. All three embedded hashes pass.
- Metadata: `26.3.0-devel-android-kbase-b1525b9a3392`, min API 30, explicitly non-conformant and runtime-unverified.
- Driver and private helper are ELF64 AArch64, Android API 30. Driver RUNPATH is `$ORIGIN`; driver NEEDED includes `libpanvk_drm.so`; helper NEEDED is only `libc.so`. The 24 DRM imports are all provided by 212 helper definitions; the audit missing-symbol list is empty. Staged ICD manifest reports Vulkan API 1.4.354 and the expected AArch64 library path.

The exact-head wrapper/probe run [43906343923](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343923) is still in progress: the capability-probe job passed, but Release and Debugoptimized wrapper compilation have not completed and no wrapper artifacts are available yet. Artifact 14244179654 is only the small VRS capability probe, not the render probe.

The independent render probe remains artifact 13685924631 from source c4f (ZIP SHA-256 `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`), containing the render executable and runner scripts. No device import or runtime test has occurred. The new Android package is a valid static candidate, but wait for exact-head wrapper success before asking for the Winlator test.
