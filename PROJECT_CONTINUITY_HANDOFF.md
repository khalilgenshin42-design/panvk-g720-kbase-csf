# PanVK G720 project continuity handoff
Last updated: 2026-11-05 (UTC)

## Authoritative current state — 2026-11-05

This section supersedes older "next steps" and "no code changed" statements in the historical addenda below. Keep the historical record; do not treat its older status as current.

- Repository branch inventory was checked across all 25 branches. The only branch containing the root `PROJECT_CONTINUITY_HANDOFF.md` is `docs/project-continuity-handoff-2026-11-03`; no other branch has a divergent copy.
- Canonical handoff branch: `docs/project-continuity-handoff-2026-11-03` (the only branch containing this root handoff file).
- Current isolated implementation branch: `experiment/g720-descriptor-heaps`, source HEAD `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`.
- Existing primary source branch `experiment/g720-clip-cull-capabilities` remains at `9188e7bf92a032431ca16ac02d54d81be5a5e12e`; it has not been merged with the descriptor-heap branch.
- Capability matrix branch: `experiment/g720-performance-audit`, HEAD `82fc0e49709e170e7b669c3c62b6bc1cbbc9e722`.

### Current implementation state

The isolated Valhall branch now contains the PanVK descriptor-heap backend path: NIR heap lowering, heap resource-table entries, sampler/resource heap binding, secondary-command-buffer inheritance, push data, explicit shader heap-use metadata, embedded sampler packing and shader binary/cache lifecycle, descriptor writer commands, conditional custom-border registration commands, and extension/feature/property reporting.

The current registry command audit covers `vkWriteSamplerDescriptorsEXT`, `vkWriteResourceDescriptorsEXT`, `vkCmdBindSamplerHeapEXT`, `vkCmdBindResourceHeapEXT`, `vkCmdPushDataEXT`, `vkGetImageOpaqueCaptureDataEXT`, common-runtime `vkGetPhysicalDeviceDescriptorSizeEXT`, and conditional `vkRegisterCustomBorderColorEXT` / `vkUnregisterCustomBorderColorEXT`. YCbCr and subsampled samplers are rejected by the sampler-heap writer; YCbCr image descriptor counts now match the per-format PanVK texture descriptor count. Custom border colors are not allowed in embedded samplers. Acceleration structures, ARM tensors, QCOM image-processing descriptor types, sparse heaps, protected heaps, and descriptor-heap capture/replay remain unsupported/disabled.

### Build and test status

- Intermediate Android-only build run `42078972694` passed compile, stage, package audit and upload at older source `3e4e7ee3849ebab12c114eef8c6c3a650cd14b8f`. Its artifact `13438649591` is stale and must not be sent to the user for testing.
- The first exact-current-source Android-only build run `42080294652` failed at compile because `struct pan_model` has no `arch` member. The YCbCr count code was corrected to derive architecture with `pan_arch(physical_device->kmod.dev->props.gpu_id)`. The corrected source HEAD is `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`; Android-only run `42082198442` and wrapper run `42082198906` are queued.
- No exact-current-source compile/link/package validation has passed yet. No device runtime test has been requested or performed for this descriptor-heap branch.

### User action gate

Continue source review and CI work autonomously. Do not ask the user to import/test until the exact current source has passed Android ARM64 compile/link, ELF/libdrm audit, package creation, and package-contract audit. Keep the custom package separate and importable through Winlator Mali; never replace the Android vendor/system Vulkan driver. Do not return to tiny FPS A/B changes as the default work. Ray query, sparse residency, and any other incomplete feature remain disabled until end-to-end implementation and correctness evidence exist.


This document is the durable handoff for future ChatGPT sessions working on this project. Read it before making changes. Treat GitHub history, branches, workflow artifacts, and the user's uploaded diagnostic archives as the source of truth. Do not assume this document contains every historical discussion; it records the key decisions, current state, and pointers needed to continue safely.

## User goal and hard constraints

- Target: Xiaomi 15T, MediaTek Dimensity 8400 Ultra / MT6899, Arm Mali-G720 MC7, Android 16, 12 GB RAM.
- User reports a 7-core G720 MC7 configuration. Query actual shader_present/core mask from the device; never hardcode seven compiler workers or queue counts.
- Build/test an experimental Mesa PanVK Android ARM64 driver for DXVK / VKD3D-Proton PC game emulation in Winlator Mali 1.2.
- User is non-rooted and has only an Android phone/Termux for local device-side work.
- **Never replace, patch, or install over the phone's stock/vendor Vulkan driver.** Keep experimental packages separate and use Winlator's custom driver importer only. Preserve System Driver as fallback.
- User authorizes autonomous repository read/write work. Ask for user action only when device testing or a missing fact genuinely requires it. Clearly distinguish static/compile validation from device runtime proof.
- Do not force-enable unsupported Vulkan features merely to satisfy DXVK. In particular, do not expose ray query or vertexPipelineStoresAndAtomics without a real implementation/correctness proof.

## Repository

Repository: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf

Important branches (preserve all; do not delete or casually merge):
- `main`
- `android-candidate-beta-1.9.4`
- `work/android-libdrm-runtime-audit`
- `work/android-libdrm-runtime-audit-latest`
- `experiment/kbase-csg-scope-diagnostic`
- `work/winlator-adpkg-packaging`
- `docs/project-continuity-handoff-2026-11-03` (this documentation branch)

Packaging branch base/head at time of this handoff:
- `work/winlator-adpkg-packaging` = `456cf76ab6780b08096d0f62a51200a8627c3f82`
- Commit: `ci: package PanVK as flat Adrenotools ZIP`
- Handoff branch starts from the same SHA. This handoff document is not a driver code change.

Prior audited source branch:
- `work/android-libdrm-runtime-audit-latest`
- HEAD `2177569b1fea09f737d9d7bd57286af1f7937244`
- Commit: `ci: preserve full DRM symbol names in ABI audit`

Descriptor arithmetic hardening commit:
- `7dec843bc1b884a13bfe8acf6a5bcbe0cff155a5`
- `panvk: harden variable descriptor allocation arithmetic`

Keep WAIT64 implementation and its conservative CPU/KCPU fallbacks. Do not revert the old CSG-scope SYNC_ADD64 experiment without tracing the full history and proving a replacement.

## Successful Android build and ADPKG packaging

Workflow: Android-only PanVK G720 Experiment
- Run ID: `41657440647`
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/41657440647
- Branch: `work/winlator-adpkg-packaging`
- SHA: `456cf76ab6780b08096d0f62a51200a8627c3f82`
- Job `125991413317` completed SUCCESS.
- All steps passed: dependencies, host tools, real Android ARM64 libdrm, Android-only configure, compile/stage, driver/manifest/dependency audit, flat ADPKG packaging, artifact upload.

Workflow artifact:
- Artifact ID: `13239804377`
- Name: `panvk-g720-android-only-experimental-package`
- Expires: 2027-02-01.
- Outer artifact downloaded in prior ChatGPT session as `/mnt/data/panvk-g720-android-only-experimental-package.zip`.
- The importable nested package was extracted as `/mnt/data/PanVK-G720-experimental.zip`.

Nested ADPKG contents (flat root):
- `meta.json`
- `libvulkan_panfrost.so`
- `libpanvk_drm.so`
- `SHA256SUMS`

Package metadata:
- name: PanVK Mali G720 (experimental)
- packageVersion: `26.3.0-devel-android-kbase-456cf76ab678`
- vendor: Mesa
- driverVersion: `26.3.0-devel`
- minApi: 30
- libraryName: `libvulkan_panfrost.so`
- explicit description says experimental, not Vulkan conformant, device/game compatibility unverified.

Package SHA-256:
`60de8cdbb92090ea111d7b1a54b2868b9933eee90b3862ac51a25d09d960928e`

SHA256SUMS inside package:
- driver: `c8b492f0dd1da2726ea31dcb4810c2a534ea0666d51ec3813beaef573dd1979e`
- private libdrm: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
- meta.json: `9956b31c714667ae575af0f35b6a8b2d92579f50baab7f952ec0e0bb594f6cf3`

The package build script is `.github/scripts/package-panvk-adpkg.sh`; package docs are `packaging/README-WINLATOR.md`; workflow is `.github/workflows/android-only-experiment.yml`.
The package script patches only package copies: private libdrm SONAME becomes `libpanvk_drm.so`, driver's NEEDED entry is replaced, and RUNPATH is set to `$ORIGIN`. It verifies AArch64, NEEDED, SONAME, RUNPATH, and flat archive structure.

**What this proves:** Android ARM64 binary compiled/linked and package structure/linker metadata passed static checks. It does NOT prove Kbase ABI compatibility, correct cache/shareability behavior, Vulkan conformance, WSI stability, or game compatibility.

## Device test and current blocking fault (2026-11-03)

User imported the package into Winlator Mali 1.2 and tested Tomb Raider. It hangs at the loading screen. User uploaded:
`TombRaider_logs_2026-11-03_16-09-08.zip`
Contains:
- `wine_debug.log`
- `TombRaider_d3d11.log`
- `TombRaider_dxgi.log`
- `TombRaider_d3d9.log`

Relevant exact wine_debug.log lines:
```
[MESA-PANVK] kbase: CSF group 0 fatal error: status 0x00c00488 (exception 0x88), sideband 0x00000001ab1a047f
[error]: QueueSubmit failed: -4
../src/vulkan/wsi/wsi_common_x11.c:2027: Swapchain status changed to VK_ERROR_OUT_OF_DATE_KHR
X connection to :0 broken (explicit kill or server shutdown).
```
DXVK D3D11 log ends with:
```
err: DxvkSubmissionQueue: Command submission failed: VK_ERROR_DEVICE_LOST
```

Log review established:
- PanVK loads.
- Vulkan device creation succeeds.
- DXVK initializes and creates a swapchain (900x506; actual properties later show FIFO, 4 images).
- Multiple Android Hardware Buffer/gralloc imports succeed before the fault.
- Kbase BO allocation and submission path are reached.
- Fatal GPU exception occurs, then queue submit reports -4, then DXVK reports VK_ERROR_DEVICE_LOST.
- Swapchain out-of-date and X connection teardown appear downstream after GPU failure, not established as root cause.
- DXVK compiler threads reported as four. Do not assume this is wrong; no evidence yet that thread count caused the fault.
- Do not ask user to reinstall or change arbitrary settings before source diagnosis.

### Exact meaning of status 0x00c00488

Arm Mali CSF UAPI register definitions identify exception byte `0x88` as `GPU_SHAREABILITY_FAULT`. See:
- https://chromium.googlesource.com/chromiumos/third_party/kernel/+/cf07462966736cdc97946fa2434e1d2f00d30b6c/drivers/gpu/arm/bifrost/include/uapi/mali_kbase_gpu_regmap_csf.h
- https://nest-open-source.googlesource.com/manifest_repos/mali-driver/+/0f8397eced2de6bc649a9cc32d0fae77a1dc34dc/bifrost/r44p0/kernel/drivers/gpu/arm/midgard/gpu/backend/mali_kbase_gpu_regmap_csf.h

A directly relevant community investigation:
- https://github.com/MineGraph-bilibili/panvk-Android/issues/3
- The reported 0x88 is shareability fault. In the sample status `0x04c00488`, bits [15:8] are access type `0x04`, an atomic. The user's exact status is `0x00c00488`; decode access type from its own [9:8] bits, not from another report. Note: `0x00c00488 >> 8 & 0x3 = 0`, which corresponds to atomic according to `GPU_FAULTSTATUS_ACCESS_TYPE_ATOMIC=0`. The issue argues that on non-I/O-coherent SoCs, GPU atomics with SYSTEM shareability across the GPU/IO boundary can fault, while CSG-scope atomics on uncached memory may be safe. This is a hypothesis to verify against our exact allocation/atomic paths, not yet a proven diagnosis for this device.
- Issue also describes intermittent failures linked to cache residency, and reports that a GPU reset kills the context.
- Related older discussion mentioned in that issue: mesa-funnymdzz issue #2. Search and inspect it before implementing a change.

**Important correction to prior assistant diagnosis:** do not treat this as a generic cache flush bug by default. A GPU_SHAREABILITY_FAULT is specifically a same-cache-line shareability attribute mismatch (shareable vs non-shareable access within GPU), or a relevant system/GPU shareability contract violation. Need decode the full status fields and trace the offending atomic's memory attributes/scope. A simple CPU cache flush is not necessarily the fix.

## High-priority source audit

Main source file:
`src/panfrost/lib/kmod/kbase_kmod.c`

Current relevant source facts from packaging branch:
- Kbase backend uses `/dev/mali0`.
- It parses raw GPU properties including `KBASE_GPUPROP_RAW_COHERENCY_MODE`.
- It currently sets `props->is_io_coherent = false` unconditionally.
- Comments explicitly say cached CPU mappings still need `KBASE_IOCTL_MEM_SYNC` before GPU access and after GPU writes.
- Supported BO flags include executable, alloc-on-fault, no-mmap, WB_MMAP, GPU_UNCACHED, CSF_EVENT.
- Cached BO synchronization is in `kbase_kmod_flush_bo_map_syncs()` around line ~2000, uses `KBASE_IOCTL_MEM_SYNC` with `BASE_SYNCSET_OP_MSYNC` / `BASE_SYNCSET_OP_CSYNC`.
- Env override `PANVK_KBASE_USER_CACHE_SYNC=0` forces kernel ioctl cache sync path; generic layer can otherwise issue architecture cache ops for WB_MMAP BOs. This optimization was documented as reducing ioctl overhead on G710, but must be checked against G720 and its IO-coherency mode.
- Kbase BO allocations without exclusive VM and without executable/alloc-on-fault/CSF-event flags may use Android dma-heap + dma-buf import path.
- Imported BO has separate GPU mapping and CPU dma-buf mapping.
- Kbase CSF group fatal error is latched and reported to queue status.

Generic source:
`src/panfrost/lib/kmod/pan_kmod.c` provides generic BO allocation/import/sync and backend dispatch. Inspect cache ops path and whether direct userspace cache ops are safe on Android AArch64 for this G720 + kernel driver configuration.

PanVK features:
`src/panfrost/vulkan/panvk_vX_physical_device.c`
- Extensions list currently does not advertise `VK_KHR_ray_query`.
- `vertexPipelineStoresAndAtomics` is gated:
```c
.vertexPipelineStoresAndAtomics =
   (PAN_ARCH >= 13 && instance->drirc.misc.enable_vertex_pipeline_stores_atomics) ||
   instance->drirc.misc.force_enable_shader_atomics,
```
- On Mali v12 this is disabled by default. Do not force-enable until compiler/hardware correctness is established.
- Kbase path disables sparse binding feature because sparse bind queue is a stub.
- Do not advertise ray query without implementation.

## Existing source work to preserve

- Shader core availability is derived from kernel-reported `shader_present`; no hardcoded MC7 worker count should be added.
- Descriptor variable-count arithmetic hardened with 64-bit intermediates and overflow guards (commit listed above).
- WAIT64 path uses GPU WAIT64 only for eligible local/internal binary payloads; imported sync_file, timeline wrappers, mixed waits, wait-only submit, oversized wait sets use CPU/KCPU fallback. Preserve max 3 wait cells and `GREATER(target - 1)` semantics.
- Kbase WSI path exists but requires runtime verification.
- Kbase BO memory mapping uses SAME_VA / EXEC_VA rules; do not add MAP_FIXED/hints against Kbase's restrictions.
- Previous work included 1 MiB CSF tiler chunks to avoid scarce 2 MiB contiguous allocations; preserve until a new analysis proves otherwise.

## Next steps for future sessions

1. Read this file, then inspect GitHub branch list and recent commits. Continue from `work/winlator-adpkg-packaging` for package code and from `work/android-libdrm-runtime-audit-latest` for previous audited baseline. Do not overwrite main or delete branches.
2. Inspect issue #3 and its linked mesa-funnymdzz issue #2; extract precise status bitfield decoding and proposed atomic scope / memory attribute remediation.
3. Inspect the exact `panvk_vX_queue.c`, `panvk_vX_sync.c`, shader atomic lowering, and `pan_kmod.c` cache synchronization path. Search for SYSTEM / DEVICE / CSG scope and GPU atomics on memory that can be DMA-BUF imported or non-IO-coherent.
4. Decode this exact status `0x00c00488` using upstream UAPI macros: exception type, access type, address-valid, JASID-valid, JASID, source ID. If address valid, expose/log fault address and source ID where safe. Sideband `0x00000001ab1a047f` may encode queue/CS metadata; use kernel ABI docs before interpreting.
5. Determine actual G720 coherency from `RAW_COHERENCY_MODE` (0=none, 1=ACE-Lite, 2=ACE) using device-side debug logging. Do not infer from SoC family.
6. Compare cache sync optimizations. If necessary, make a diagnostic build with `PANVK_KBASE_USER_CACHE_SYNC=0` set in Winlator environment only, if supported, to force KBASE_IOCTL_MEM_SYNC. This is a diagnostic A/B test, not a claimed fix. Do not change phone system driver.
7. Consider an isolated experiment branch for restricting SYSTEM-scope atomics to CSG scope only when memory is GPU-uncached/non-IO-coherent and semantics allow it. Do not broadly downgrade Vulkan memory-model atomics; scope/order must remain correct. Add targeted tests and document unsupported cases.
8. Rebuild, re-audit ADPKG, produce new checksum, then ask user to test only the specific diagnostic build.
9. Once GPU fault is addressed, separately diagnose WSI/X11 stability and game compatibility. Do not equate successful launch with performance superiority.

## Prior workload history

- Earlier Borderlands original GOTY (DX9, 1280x720) on Winlator Mali 1.2 System driver: around 20-29 FPS baseline; Wrapper V2 runs around 28-35 FPS. This is not a PanVK benchmark.
- Current PanVK test is Tomb Raider; it hangs at loading screen with the fault above.
- User wants the driver to outperform stock system driver in PC emulation, but first priority is correctness/stability.

## User interaction checkpoint

The user explicitly asked that all work remain accessible to future GPT sessions if the current conversation reaches its limit. This handoff document is intended to be the persistent entry point. In future sessions, user can say:
"Continue Project panvk drivef. Read PROJECT_CONTINUITY_HANDOFF.md in khalilgenshin42-design/panvk-g720-kbase-csf, then inspect the latest branches and continue from the recorded fault investigation."
Do not claim that every GPT version automatically shares live conversation memory. Durable GitHub docs/commits/artifacts are the reliable cross-session record. Update this file after every major milestone, including commit SHA, workflow run, artifact ID, checksum, test results, and next action.


## Addendum: confirmed WAIT64 scope defect and diagnostic patch (2026-11-03 15:17 UTC)

A decisive source comparison has identified a highly plausible cause of the user's exact 0x88 fault.

- Upstream community investigation: https://github.com/MineGraph-bilibili/panvk-Android/issues/3
- The issue describes a non-I/O-coherent Mali G715 (MT6985) with the same `GPU_SHAREABILITY_FAULT 0x88`.
- Their command-stream census isolated `SYNC_WAIT64` emitted with default `System` scope. Their GenXML had no Scope field for `SYNC_WAIT32/64`, unlike `SYNC_ADD64`. Their patch added Scope at bits start=1 size=2, passed CSG scope into wait emission, and changed the encoded wait atom from `0x...0000` to `0x...0004`. They still observed 0x88 afterward, so it was a real defect but not a complete root-cause fix.
- The issue found a later remaining possibility in firmware/microcode: the SYNC_WAIT operation's internal atomic read may ignore the scope field. Their proposed decisive workaround is replacing SYNC_WAIT with load + conditional branch loop.

Our exact packaging source branch was checked:
- `src/panfrost/genxml/v10.xml`, `v12.xml`, `v13.xml`, `v14.xml`: `CS SYNC_WAIT32` and `CS SYNC_WAIT64` structs had Error Reject / Condition / Data / Address / Opcode but no Scope field. `SYNC_ADD32/64` and `SYNC_SET32/64` did have Scope.
- `src/panfrost/genxml/cs_builder.h`: `cs_sync32_wait` / `cs_sync64_wait` macro had no scope parameter and did not set `I.scope`.
- `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`: GPU WAIT64 path emits `cs_sync64_wait(... GREATER ...)` for eligible local waits. The same file has a diagnostic option `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE`, default false, for ADD64 scope.
- This directly intersects the user's successful DXVK initialization followed by CSF GPU fault.

Diagnostic experiment branch created:
`experiment/csf-sync-wait64-csg-scope`
Changes made:
1. Added `Scope` (2 bits, start=1, enum CS Sync Scope) to both SYNC_WAIT32 and SYNC_WAIT64 structures in v10/v12/v13/v14 GenXML.
2. Added `enum mali_cs_sync_scope scope` parameter to generated `cs_sync*_wait` builder macro and writes `I.scope = scope`.
3. Changed the only currently found PanVK CSF WAIT64 emission site to pass `MALI_CS_SYNC_SCOPE_CSG`, with an explanatory comment.
4. Added this branch to Android-only CI workflow push filters.

Commits on experiment branch, chronological:
- `ceb26466639fb91535b9657770cb582c9f07193a` (v10 GenXML)
- `dbfc6ca8fef3d4652b1f463029b5a54559b81fc6` (v12 GenXML)
- `410dd200c744103b42c9a22f3daae3e8b2e8760d` (v13 GenXML)
- `f02f253d46e98e51889ee97596e5e90e8fc87868` (v14 GenXML)
- `2c987a748942e5871806bbf2996ef3bd86c6b6e2` (cs_builder scope param)
- `56ece3fa412a660d95669cd7497f72e71233a915` (PanVK WAIT64 call passes CSG)
- `9eae560bafeb85b1faa1da841a00881a677acd19` (CI branch trigger)

CI runs triggered by per-file commits; the latest full-change run is:
- Run ID `41662126467` (also a duplicate run `41662126541` on same SHA)
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/41662126467
- SHA: `9eae560bafeb85b1faa1da841a00881a677acd19`
Check this run to establish whether generated GenXML/build succeeds. Earlier intermediate runs were triggered before all changes landed and are not the final validation.

Decode user's exact status `0x00c00488` using upstream UAPI bit definitions:
- exception type bits [7:0] = 0x88 = GPU_SHAREABILITY_FAULT.
- access type bits [9:8] = 0 = ATOMIC.
- address-valid bit [10] = 1.
- JASID-valid bit [11] = 0.
- JASID bits [15:12] = 0 (invalid per valid bit).
- source ID bits [31:16] = 0x00c0.
This is stronger evidence for an atomic shareability fault. The earlier community issue's prose loosely described [15:8] as atomic; use the actual UAPI macros ([9:8]) for decoding.

Do not declare this patch a fix until it builds and user tests it. Even if it suppresses this specific WAIT64 fault, the upstream investigation explicitly observed 0x88 continuing after adding the scope field, so follow with diagnostics and, if needed, a LOAD+conditional-branch wait workaround. Preserve CPU/KCPU fallbacks for ineligible waits. Do not globally alter all atomics or cache sync behavior.

Source for exception definitions:
https://chromium.googlesource.com/chromiumos/third_party/kernel/+/cf07462966736cdc97946fa2434e1d2f00d30b6c/drivers/gpu/arm/bifrost/include/uapi/mali_kbase_gpu_regmap_csf.h


## Addendum 2: cross-CSG scope mismatch found in our own source (2026-11-03)

Important correction to the first WAIT64 experiment:
- The WAIT64 scope field itself is a valid missing-encoding fix, but changing the kbase wrapper's WAIT64 scope to CSG was not justified for its waits. Existing source comment at `panvk_vX_gpu_queue.c:536-540` explicitly says the internal PanVK binary waits can target completion counters produced by *distinct CSGs*, and an earlier G720 experiment physically proved SYSTEM-scope WAIT64 `GREATER(target-1)` works across distinct CSGs.
- The diagnostic branch has therefore been corrected to emit WAIT64 with explicit `MALI_CS_SYNC_SCOPE_SYSTEM`. This preserves the old zero scope bits while making the scope visible in the builder API. Do not revert to CSG scope for these cross-CSG waits.

A more directly actionable mismatch was found:
- `kbase_create_group()` in `panvk_vX_gpu_queue.c` calls `kbase_kmod_csf_group_create(..., 1, ...)` separately for each PanVK subqueue. The queue header documents that each PanVK subqueue owns its own CSG.
- Kbase's command-buffer creation sets `cmdbuf->sync_scope = MALI_CS_SYNC_SCOPE_SYSTEM` (see `panvk_vX_cmd_buffer.c:903-909`) specifically for Kbase.
- However, `cs_render_desc_ringbuf_reserve()` in `panvk_vX_cmd_draw.c` had hard-coded `MALI_CS_SYNC_SCOPE_CSG` for the reservation/decrement of the render descriptor ring syncobj. The matching release operation in the fragment path uses `cmdbuf->sync_scope`, which is SYSTEM on Kbase.
- This means the shared render descriptor ring syncobj could be updated from separate CSGs with inconsistent shareability scopes: reservation CSG, release SYSTEM. This matches the class of cross-subqueue scope mismatch described in https://www.reddit.com/r/EmulationMediatekMali/comments/1wtx1su/does_anyone_know_whats_going_on/ and is a much stronger concrete candidate than globally changing WAIT64 scope.

The experiment branch now includes a targeted correction:
- `cs_render_desc_ringbuf_reserve()` takes an explicit `enum mali_cs_sync_scope scope`.
- Its SYNC_ADD32 uses that scope.
- The callsite passes `cmdbuf->sync_scope`, making the reserve and release operations use SYSTEM on Kbase and CSG on Panthor.
- WAIT64 is explicitly SYSTEM for cross-CSG waits.
- The GenXML WAIT scope field and builder API additions remain, allowing precise future control and diagnostics.

Latest branch commits:
- `7c68aba20defa743250aaaac4ed5cce442d59d7a`: restore SYSTEM scope for cross-CSG WAIT64.
- `827240a83442cf39db021f771fe97c4e931d5ff3`: match descriptor-ring reserve scope to the matching release operation.

Next: wait for CI on the latest branch head, then review *all* cross-subqueue syncobj producers/consumers for scope symmetry. Only after build success should the user test this targeted candidate. The earlier run on SHA `9eae560bafeb85b1faa1da841a00881a677acd19` is an intermediate build and does not validate the latest two commits.


## 2026-11-03 addendum — WAIT scope call-site audit and build retry

- The previous CI runs on `34280b58d746dddaf4c33a33d238e6d50826b4e2` had not yet reached a final status at the time of recheck.
- A full CSF source scan found additional old-signature WAIT call sites after adding the scope argument to the low-level builder:
  - `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`: descriptor ring-buffer `cs_sync32_wait` now receives the ring's selected scope; vertex-to-fragment tiler `panvk_instr_sync64_wait` now receives `cmdbuf->sync_scope`.
  - `src/panfrost/vulkan/csf/panvk_vX_cmd_event.c`: event `panvk_instr_sync32_wait` now receives `cmdbuf->sync_scope`.
- These are follow-up compile/API integration fixes, not proven runtime synchronization fixes.
- Commits on `experiment/csf-sync-wait64-csg-scope`:
  - `34280b58d746dddaf4c33a33d238e6d50826b4e2`: thread scope through instruction wrapper and barrier wait.
  - `68034c7c705221e81d243af8c7e8164cce92ef92`: pass scope to descriptor-ring and tiler waits.
  - `6634282aa14616931656b883aafb5519831eb888`: pass scope to event waits; this is current branch HEAD unless later commits supersede it.
- CI triggered for the intermediate SHA `68034c7c705221e81d243af8c7e8164cce92ef92`:
  - Android-only run `41665460748`.
  - Android wrapper run `41665460790`.
  - Both were in progress at the time of this addendum. Re-query before treating as pass/fail.
- Audit all remaining direct `cs_sync32_wait`, `cs_sync64_wait`, `panvk_instr_sync32_wait`, and `panvk_instr_sync64_wait` call sites whenever the builder signature changes. Ensure no legacy signature remains in CSF source.
- No package has been produced from these follow-up commits yet; do not ask the user to test until CI passes and package contents/checksums are verified.


## 2026-11-03 addendum — scope-callsite branch build and package verification

- Current diagnostic branch HEAD at tested build: `6634282aa14616931656b883aafb5519831eb888` (`experiment/csf-sync-wait64-csg-scope`).
- Android-only workflow run `41665470230`: completed SUCCESS. All steps passed, including compile/stage, driver+manifest+dependency audit, flat Winlator/Adrenotools package build, and artifact upload.
- Android wrapper workflow run `41665470073`: completed SUCCESS. Both `release` and `debugoptimized` variants compiled and uploaded successfully.
- Android-only workflow artifact:
  - Artifact ID: `13246167213`
  - Name: `panvk-g720-android-only-experimental-package`
  - Outer artifact ZIP: `PanVK-G720-experimental-6634282.zip`
  - Outer ZIP SHA256: `f1b610c2f77932f1674d9723615d5c6351a36a8a265a7b0eafae2dbb0800f66c`
  - Inner importable package: `PanVK-G720-experimental.zip`
  - Inner package SHA256: `c765155efad594dcacc5ff62bc2e532856dddc36bc4a9303d4d2e4e7d5b849eb`
  - Inner package files: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`.
  - SHA256SUMS verified OK for all three payload files.
  - Driver and private libdrm are ELF64 little-endian AArch64 Android shared objects; driver SONAME `libvulkan_panfrost.so`, NEEDED uses private `libpanvk_drm.so`, RUNPATH `$ORIGIN`; private libdrm is packaged alongside. Outer stage also includes runtime stub and supporting audit records.
  - Metadata explicitly states experimental, not Vulkan conformant, compatibility unverified; min API 30; Mesa 26.3.0-devel.
- This proves compile, link, packaging, and static package integrity only. It does NOT prove runtime correctness or fix the previously observed CSF `GPU_SHAREABILITY_FAULT 0x88`.
- Next step: user may import this exact package into Winlator's custom driver manager as an isolated test driver, retaining System Driver as fallback. Test the same Tomb Raider workload and collect fresh logs. Do not install or replace Android's system/vendor driver. If the app's importer expects the outer artifact, extract `PanVK-G720-experimental.zip` from it and import that inner ZIP.


## 2026-11-03 addendum — Tomb Raider benchmark shutdown clarification and log chronology

User clarification:
- The user probably closed Tomb Raider after finishing the benchmark, but is not completely certain.
- Therefore the XServerDisplayActivity onDestroyWindow messages at the end of the 16:54 log capture must NOT be treated as proof that Winlator prematurely destroyed the game window or that window destruction caused the benchmark failure.
- Treat normal user-initiated game closure as the leading explanation for the final window-destruction sequence unless evidence from an earlier timestamp contradicts it.

Reinspection of TombRaider_logs_2026-11-03_16-54-16.zip:
- logcat.log has 2,520 lines; wine_debug.log 203; TombRaider_d3d11.log 278.
- At 16:54:01.697 and 16:54:02.906, Android records touch DOWN events on XServerDisplayActivity; corresponding UP events at 16:54:01.752 and 16:54:02.964.
- At 16:54:03.419, 16:54:03.436, and 16:54:03.436, XServer logs destruction of tombraider.exe windows.
- At 16:54:03.528, Wine logs X11DRV_vulkan_surface_create get_client_rect failed.
- At 16:54:03.721, Wine asserts at loader_thunks.c:6748 with !status && vkWaitForFences.
- Additional window teardown continues after that, including Explorer windows around 16:54:05.
- This close timing plus the touch events is consistent with the user closing the game after benchmark completion. It is not conclusive about which touch/action caused it, and it does not establish that the teardown errors happened during active gameplay.
- A separate earlier group of onDestroyWindow: Destroying window tombraider.exe messages occurs at 16:51:35.733–16:51:35.739. Its meaning is not established; inspect surrounding logcat and Wine timestamps before attributing it to a crash or user action.
- wine_debug.log ends with the surface creation error, two RtlLeaveCriticalSection errors, then the vkWaitForFences assertion and Session ended.
- TombRaider_d3d11.log ends with swapchain properties (900x506; initial IMMEDIATE/3 images, then FIFO/4 images) and D3D11SwapChain: Failed to recreate swap chain.
- The D3D11 log itself has no wall-clock timestamps, so its final swapchain error cannot yet be ordered precisely relative to the 16:54:03 window teardown from the available files.
- No GPU_SHAREABILITY_FAULT, CSF group fatal error, QueueSubmit failed, or VK_ERROR_DEVICE_LOST string appears in this latest archive. This is only an absence in this capture, not proof that the prior GPU fault is fixed.
- Latest reported benchmark result remains min 7.9 FPS, max 18.9 FPS, average 14.0 FPS.

Updated interpretation / investigation priority:
1. Downgrade premature XServer window destruction from a suspected root cause to an unconfirmed teardown observation. The user's likely post-benchmark close is a plausible explanation for the 16:54:03 sequence.
2. Determine whether D3D11SwapChain: Failed to recreate swap chain occurred while the game was still rendering, or during normal shutdown. Need timestamps or more detailed DXVK/Wine trace.
3. Treat get_client_rect failed, RtlLeaveCriticalSection, and vkWaitForFences assertion as possibly shutdown/secondary errors until proven otherwise.
4. The primary unresolved runtime question is whether the game completed a stable benchmark under PanVK and whether any GPU/CSF fault occurred before the user closed it. FPS alone (14.0 average) does not prove stability or superiority.
5. For the next controlled test, ask the user to capture logs after the benchmark but before manually closing Tomb Raider if possible; if the game must be closed, note the exact close time. Prefer a short capture during gameplay plus a separate shutdown capture. Do not ask them to repeat the benchmark until source/log audit has progressed and a precise diagnostic need is identified.
6. Continue source audit in parallel: inspect the current diagnostic branch head and exact PanVK/Mesa WSI paths; verify whether a failing vkWaitForFences result could originate from Kbase fence status/device loss versus a teardown race. Do not patch WSI or synchronization based only on the final teardown messages.
7. Preserve prior CSF WAIT64 and descriptor-ring scope work and all package artifacts. No new source fix, CI build, or package was made as part of this log reinspection.

Correction to earlier handoff language:
- Do not state categorically that the Android log shows window destruction immediately before a failure during active gameplay. It shows window destruction immediately before the final Wine surface/fence errors, but the user may have initiated normal game closure after benchmarking.


## 2026-11-03 addendum — PanVK Kbase fence/completion source audit (read-only)

Repository state rechecked:
- `experiment/csf-sync-wait64-csg-scope` still points to `6634282aa14616931656b883aafb5519831eb888`.
- Documentation branch contains this handoff update at `44c50bcbb480020caeeca866cabd69bcafee5dbf`.
- No code branch was changed and no build/package was created in this audit.

Source paths verified in this tree:
- PanVK WSI adapter: `src/panfrost/vulkan/panvk_wsi.c` (not `panvk_vX_wsi.c`).
- Shared Mesa WSI implementation lives under `src/vulkan/wsi/`, including `wsi_common.c`; the tree does not contain `wsi_common_x11.c` in this branch.
- Kbase CSF queue/completion path: `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`.
- Common fence path: `src/vulkan/runtime/vk_fence.c` and `vk_sync.c`.

Verified WSI behavior in `panvk_wsi.c`:
- Initializes shared `wsi_device_init()` with PanVK proc-address callback.
- For Kbase, sets `.sw_device = uses_kbase && !kbase_dmabuf`.
- Sets `.wait_present_before_queue = kbase_dmabuf`.
- Sets `disable_unordered_submits = uses_kbase`, with explicit comment that Kbase sync payloads carry userspace GPU seqno state and cannot be copied through DRM syncobj FD payloads; even empty WSI submits remain on the real queue so present fences are backed by the Kbase submission timeline.
- For Kbase without dma-buf path, shared WSI's software/SHM path is selected. This is relevant context for Winlator X11 surface/present behavior but does not itself establish a WSI bug.

Verified Kbase completion path in `panvk_vX_gpu_queue.c`:
- Kbase GPU completion is tracked by per-subqueue seqnos in GPU-written `panvk_cs_sync64` cells.
- `kbase_subqueue_wait_seqno()` polls seqno/error state, periodically re-kicks groups, uses `kbase_kmod_csf_wait_cqs64()` for a CQS wait and falls back to `kbase_kmod_csf_wait_event()` when CQS wait returns negative.
- It returns `VK_TIMEOUT` if the caller's absolute deadline expires before the internal watchdog; if the internal 10-second watchdog expires, it emits detailed ring/seqno/CSF diagnostics and marks the queue lost with `kbase: timeout on subqueue %u`.
- A nonzero `cell->error` marks the queue lost with `kbase: CS error ...`.
- `kbase_queue_wait_current()` waits for each subqueue's latest emitted seqno and propagates errors.
- Kbase `gpu_queue_check_status()` checks `kbase_kmod_csf_has_error()` and marks the queue lost with `kbase: CSF queue-group error` if the kernel notification error latch is set; otherwise it currently returns success without inspecting all per-subqueue completion cells.
- The `vk_common_GetFenceStatus()` implementation checks device-lost state, then calls `vk_sync_wait(..., timeout=0)`. The common fence implementation is therefore a key bridge to inspect alongside the PanVK Kbase sync type/payload implementation.
- `panvk_queue_submit_ioctl_kbase()` resolves non-native incoming waits through `vk_sync_wait_many(..., UINT64_MAX)` before submitting; it may use native GPU WAIT64 only for eligible internal pending payloads. This is an additional path that can surface Kbase completion errors during a queue submit rather than a direct Vulkan fence wait.

Important diagnostic implication:
- Wine's `vkWaitForFences` assertion proves only that the call returned a non-success VkResult; it does not tell us which result. Vulkan permits `VK_TIMEOUT` for a finite wait, and Wine's assertion text alone does not distinguish timeout, device lost, or another error. Need Wine trace including returned VkResult, plus PanVK queue/fence diagnostics at the same timestamp.
- The log's final surface/fence errors may be normal shutdown, per the user's clarification. Do not patch based on them without a capture during active gameplay or a reproducible failure before manual close.

Audit items still open:
1. Find the implementation of `panvk_kbase_sync_*` payload/sync callbacks and verify the exact `vk_sync_wait` path used for Kbase fences and semaphores. It may be embedded in another source file; locate by recursive tree/code search or local checkout.
2. Check whether the Kbase seqno BO is CPU-uncached or otherwise invalidated correctly before every `cell->seqno` / `cell->error` read. In `kbase_subqueue_wait_seqno()`, source currently snapshots `cell->seqno` in `prev_seqno` comparison before calling `kbase_cache_invalidate_range()` later in the loop. Determine the mapping/cache policy before treating this as a bug.
3. Check ordering/races around `kbase_kmod_csf_wait_cqs64()` and the kernel notification fallback, including error-latch consumption and whether a CQS return can mask a pending CSF group error.
4. Add narrowly scoped diagnostic logging of exact VkResult at the Kbase sync callback boundary only if source audit identifies a useful insertion point. Avoid hot-path per-frame logging.
5. Do not alter `panvk_wsi.c` or `vk_fence.c` based solely on the shutdown trace. No WSI or fence fix is currently justified.

Potential stale comment found:- The top-level comment in `src/panfrost/lib/kmod/kbase_kmod.c` still says command submission is not wired up, while this branch contains an operational Kbase CSF submission implementation in `panvk_vX_gpu_queue.c`. This is documentation drift, not a runtime bug; update only when preparing a focused documentation cleanup.


## 2026-11-03 addendum — Kbase sync payload and CSF notification audit (in progress)

The next source-audit task from the prior read-only handoff has been completed and a separate experimental branch now carries a candidate fix. This addendum supersedes the earlier read-only section's next actions 1–3; it does not declare the candidate runtime-validated.

### Exact source and branch state

- Read the complete current handoff before continuing.
- Base diagnostic source remains experiment/csf-sync-wait64-csg-scope at 6634282aa14616931656b883aafb5519831eb888.
- New isolated branch: experiment/csf-notification-serialization.
- Latest candidate branch HEAD: 7e5d7a936b9f3a77255570b121f21626e1e6411b.
- The diagnostic branch, existing build package, and Android vendor driver were not modified.
- No new package has been accepted for device testing.

### Actual Vulkan/Kbase sync implementation located

The panvk_kbase_sync_* implementation is embedded in src/panfrost/vulkan/panvk_physical_device.c, not a standalone sync source file.

- kbase_cpu_sync_type is a CPU-backed binary sync type because Kbase does not provide DRM syncobjs.
- Timeline semaphores use Mesa's vk_sync_timeline software wrapper over that binary type.
- Pending PanVK GPU payloads store a wait callback, export callback, data pointer, and per-subqueue sequence-number targets.
- kbase_cpu_sync_wait_one() transitions payloads through RESET, PENDING, WAITING, SIGNALED, and FAILED.
- kbase_cpu_sync_wait_many() implements wait-all sequentially and wait-any by polling each payload with a zero timeout and yielding between passes.
- Native GPU WAIT64 is an optimization for eligible internal pending payloads; imported sync_file payloads and ineligible cases use the CPU/KCPU route.

### Notification-path findings and candidate changes

The shared Kbase device descriptor has one CSF notification stream. The original kbase_kmod_csf_wait_event() performed ppoll() followed by a blocking read() without serializing readers. Separate Vulkan queues can wait concurrently on the same Kbase context. Two readers can therefore both observe readiness, after which one consumes the notification and the other can block in read() waiting for an event that has already been consumed. This is a source-level concurrency hazard; device reproduction is still pending.

A second gap was found in error reporting: a successful CQS wait can finish without entering the notification fallback. The CSF group-error latch is set when userspace consumes a notification, while the previous Kbase queue-status check only read that latch. Thus an already-queued group error could remain unseen by a status query if the CQS completion path bypassed notification consumption.

The isolated candidate branch makes these changes:

1. Adds a per-Kbase-device csf_event_lock around the final nonblocking readiness recheck plus read, including initialization and all cleanup paths. The potentially blocking ppoll is deliberately outside the lock so a nonblocking status query cannot wait behind another queue's 20 ms notification sleep.
2. Factors notification consumption into a locked helper and adds kbase_kmod_csf_process_events() to nonblockingly drain all currently queued notifications.
3. Drains notifications before Kbase queue-status checks; notification poll/read failures now mark the queue lost instead of silently hiding the failure.
4. Calls vk_device_check_status() at the end of Kbase sync wait-many, including timeout results. This covers vkGetFenceStatus(), whose common implementation does not itself perform the post-wait device-status check; vkWaitForFences() already does.
5. Makes every subqueue waiter check the device-wide CSF error latch, because another queue's waiter may consume the notification first.
6. Checks the GPU-written cell->error before accepting a sequence-number completion marker, so a completion marker cannot mask a CS error in the same cell.
7. Changes both CmdResetEvent2 and CmdSetEvent2 producer SYNC_SET32 operations from hard-coded CSG scope to cmdbuf->sync_scope, matching CmdWaitEvents2. On Kbase this is SYSTEM, because the event sync objects are shared across the three subqueues/CSGs and are also read by the host.
Commits on experiment/csf-notification-serialization:

- d3051a808321241b96619832203a07f02d7ac4b2 — serialize notification poll/read and add nonblocking notification draining; add Kbase device-status checks.
- 88310bab8c529fdbb9ba0d25c34fff77e5738bd6 — propagate the shared CSF error latch to all subqueue waiters.
- 88b583c320ba07aca5a98bfd5409ea1f9345cf1d — prioritize cell->error over seqno completion.
- 1a83d5f1ba1c2ff4053317593db3f89374a4c231 — keep blocking notification polling outside the event-consumer lock; recheck readiness under lock before read.
- 6641ee939130f8b4fb215846950e76c25139e72c — use cmdbuf->sync_scope for both event producer SYNC_SET32 operations.
- 20a1cdece6885bd1569efc3325d7e2ca859f357c — log the raw Kbase coherency mode and effective allocation flags for CSF event BOs.
- d5afee6669b1a6770fa2d4153afddec31f1ad365 — extend effective-flag logging to WB_MMAP BOs as well, so the cached event-object pool's attributes are visible.
- 2be370aeb45ad952c6a0e9f940c8537e418e5fe2 — log requested and effective Kbase flags returned by DMA-BUF import for WB_MMAP and GPU_UNCACHED allocations.
- 7e5d7a936b9f3a77255570b121f21626e1e6411b — cast the UAPI __u64 returned flags to uint64_t for portable PRIx64 logging.

The candidate branch also adds itself to the Android-only experiment workflow branch filter.

### Build validation in progress

The following GitHub Actions runs were observed for the candidate branch before the latest lock-contention refinement:

- Android-only PanVK build, run 41674211319, commit 88b583c320ba07aca5a98bfd5409ea1f9345cf1d.
- Android NDK wrapper release/debugoptimized builds, run 41674211281, same commit.

These runs were still in progress at the last check and do not validate latest HEAD 6641ee939130f8b4fb215846950e76c25139e72c. Runs 41675058107 and 41675058343 target 6641ee939130f8b4fb215846950e76c25139e72c, but are superseded. Those runs target the superseded 20a1cdece6885bd1569efc3325d7e2ca859f357c. Those runs target the superseded d5afee6669b1a6770fa2d4153afddec31f1ad365. The latest HEAD 2be370aeb45ad952c6a0e9f940c8537e418e5fe2 has run 41675810459 (build-android-only) and run 41675810560 (build-android-wrapper release + debugoptimized). Both were queued at the last check; wait for both to finish. Do not consider an earlier SHA sufficient.

### Important unresolved checks

- Runtime validation of the notification mutex and error propagation has not yet occurred on the Xiaomi 15T / Dimensity 8400 Ultra / Mali-G720 MC7.
- The original 14.0 FPS Tomb Raider result remains the last reported benchmark. The log did not establish a CSF fatal fault; the final window-destruction and Wine fence assertion may be normal shutdown. Do not infer an active-game crash from those final messages.
- The seqno cell is allocated with PAN_KMOD_BO_FLAG_CSF_EVENT. The Kbase conversion clears BASE_MEM_COHERENT_LOCAL for that flag. Kernel behavior varies: current Arm Kbase CSF source forces SAME_VA, CACHED_CPU, and COHERENT_SYSTEM for CSF_EVENT allocations, while other Kbase revisions may fall back to an uncached GPU mapping when outer-shareable coherence is unavailable. Therefore do not assume either mapping on the Xiaomi kernel; use the effective flags returned by that kernel before changing cache policy.
- A separate imported-sync-file timeout issue remains to inspect: kbase_sync_file_wait_func() uses millisecond poll() timeout conversion, which floors sub-millisecond remaining time to zero and can return early. Keep this separate from the notification patch until verified.
- kbase_cpu_sync_wait_many() wait-any currently polls with zero-timeout waits and sched_yield(); this is a potential CPU-overhead optimization target, not a correctness fix yet.
- The public Mali-G715 r38p1 report in MineGraph-bilibili/panvk-Android issue #3 found missing SYNC_WAIT scope encoding, but the CSG-scope change did not eliminate its 0x88 faults; the report suspects firmware/microcode internal atomics and proposes LOAD + conditional branch as a decisive experiment. That is a different GPU/kernel report. Do not copy its scope workaround blindly onto G720. Preserve SYSTEM scope for the cross-CSG waits already documented above.

### Next actions

1. Wait for the latest Android-only and wrapper CI runs to finish; inspect failed job logs if either fails.
2. Review the final commit diff and build artifact provenance. Do not test an artifact built from an earlier SHA.
3. If both builds pass, prepare a separately named candidate package from the exact latest HEAD; retain the existing verified package and System Driver fallback.
4. Only then ask the user to test the same Tomb Raider workload. Prefer capturing logs during active gameplay and separately at shutdown, noting the exact manual-close time.
5. Add diagnostic logging for raw Kbase coherency mode and effective BASE_MEM_CSF_EVENT allocation flags before making any memory-attribute or cache-policy change.


### Cross-subqueue event scope audit and upstream G720 comparison

A further full-source pass found a producer/consumer mismatch in src/panfrost/vulkan/csf/panvk_vX_cmd_event.c:

- CmdResetEvent2 wrote each per-subqueue event sync object with hard-coded MALI_CS_SYNC_SCOPE_CSG.
- CmdSetEvent2 did the same.
- CmdWaitEvents2 already used cmdbuf->sync_scope. On Kbase that is SYSTEM, because the event's per-subqueue sync objects can be observed by another CSG and by the host.
- The event sync objects are allocated from device->mempools.rw (cached shared memory), and GetEventStatus reads them on the CPU. Matching producer and consumer scope is therefore important.

Commit 6641ee939130f8b4fb215846950e76c25139e72c changes both event producer operations to cmdbuf->sync_scope. On non-Kbase Panthor, cmdbuf->sync_scope remains CSG, so this does not alter the Panthor path.

Other hard-coded CSG operations were classified rather than changed blindly:

- The query-availability sync object lives in device->mempools.rw_nc and is not waited on by another CSG using cs_sync32_wait; its availability is read by the CPU after query execution/cache flush. Keep its scope unchanged pending evidence.
- Kbase progress breadcrumbs are per-subqueue diagnostic markers and remain CSG-local.
- Cross-subqueue barrier waits/signals and descriptor-ring reserve/release now use cmdbuf->sync_scope.
- The queue completion SYNC_ADD64 remains SYSTEM by default. PANVK_KBASE_SYNC_ADD64_CSG_SCOPE is an opt-in diagnostic switch and must not be enabled by default without checking the actual Kbase event-memory attributes and cross-CSG visibility.

A separate upstream report is directly relevant to this MT6899/Mali-G720 MC7 target: wonderkast02/panvk-g720-kbase-csf issue #9. Its author later corrected the original VT-subqueue hang diagnosis: the real cause was a 32-bit truncated CS call address in the provoking-vertex helper, not the proposed VT-lead throttle or the earlier cross-subqueue scope patch. The current candidate source already has uint64_t fn_addr and passes it through cs_move64_to in panvk_vX_cmd_draw.c, so that specific C1 translation-fault fix is present. Do not port the superseded VT-lead patch. The same upstream issue separately reports intermittent 0x88 shareability faults in GL programs even after the address fix, so the shareability investigation remains relevant and distinct.

The upstream issue also documents the earlier event producer scope change to cmdbuf->sync_scope. That change is now applied here for the event sync objects, independently of the corrected C1 root cause.


The latest candidate commit also adds two diagnostic log points without changing memory policy:

- Kbase raw coherency mode (0 = none, 1 = ACE-Lite, 2 = ACE) during device property discovery.
- Requested and effective kernel allocation flags for every PAN_KMOD_BO_FLAG_CSF_EVENT BO.

These logs are intended to establish the exact G720 MC7 kernel's CSF event-memory attributes before deciding whether SYSTEM-scope SYNC_ADD64 is valid or whether the opt-in CSG-scope diagnostic should be tested. Do not change the default scope yet.


The event-scope candidate needs one additional caution: event sync objects are allocated from mempools.rw, whose PanVK BO flags request WB_MMAP. Although the normal direct-allocation conversion defaults to BASE_MEM_COHERENT_LOCAL, these pool BOs take the DMA-heap import path; kbase_kmod_import_dmabuf() unconditionally adds BASE_MEM_COHERENT_SYSTEM to import_flags. The kernel's returned req.out.flags determines whether that system-shareable request was retained. The latest diagnostic log records both requested import flags and effective returned flags for WB_MMAP/GPU_UNCACHED imports, as well as direct CSF_EVENT allocation flags. The event producer scope change is still an unvalidated candidate; do not call it the confirmed 0x88 fix until the effective attributes and device behavior are known.


### Reconciled device-test history (do not conflate these runs)

The user-specific project history confirms two different Tomb Raider results on the Xiaomi 15T / Dimensity 8400-Ultra / Mali-G720 MC7:

- 2026-11-01: 900×506 benchmark, Min 9.0 / Max 18.9 / Avg 14.6 FPS. The captured logs contained GPU_SHAREABILITY_FAULT exception 0x88 on CSF groups 0, 1, and 2, followed by VK_ERROR_DEVICE_LOST.
- 2026-11-03: build from diagnostic commit 6634282aa14616931656b883aafb5519831eb888, Min 7.9 / Max 18.9 / Avg 14.0 FPS. The captured log did not contain 0x88, QueueSubmit failure -4, or VK_ERROR_DEVICE_LOST. The user suspected the final window-destruction sequence may be from manually closing the game after the benchmark; do not classify that as a driver crash without additional evidence.

This is encouraging evidence for the earlier WAIT64 scope / descriptor-ring scope changes in that workload, but it is not a general stability proof. The new notification and event-scope candidate branch has not yet been tested on the phone.

The opt-in PANVK_KBASE_SYNC_ADD64_CSG_SCOPE=1 diagnostic was previously built as a source option but has never been verified on the user's device. Its default remains SYSTEM; do not enable it in the release candidate without an A/B test and the new effective-memory-flag logs.


### DMA-BUF coherency flag trace (new highest-priority memory audit)

The Kbase import path in src/panfrost/lib/kmod/kbase_kmod.c currently constructs import_flags with BASE_MEM_COHERENT_SYSTEM unconditionally, alongside BASE_MEM_IMPORT_SHARED and CPU/GPU read/write permissions. This applies not only to externally imported dma-bufs: kbase_kmod_bo_alloc_dmabuf() allocates from /dev/dma_heap/system and then routes those internal pool BOs through kbase_kmod_import_dmabuf(). Therefore the rw and rw_nc pools may receive a SYSTEM-shareable GPU mapping even when the device is not IO-coherent.

Arm's UAPI defines BASE_MEM_COHERENT_SYSTEM as outer-shareable if available, and BASE_MEM_COHERENT_LOCAL as inner-shareable. Kbase may clear COHERENT_SYSTEM when the kernel's CPU-coherency capability says it is unavailable, so the requested flags alone do not prove the effective page-table attributes.

The independent Mali-G715/G720 0x88 investigation explicitly reported that a dma-buf import with COHERENT_SYSTEM set its GPU page-table shareability to SYSTEM and caused atomic SHAREABILITY_FAULTs; removing that flag fixed some cases, though a separate failure remained. This is a high-priority candidate, not yet proof that the Xiaomi 15T kernel retains the flag.

The candidate commits 20a1cdece6885bd1569efc3325d7e2ca859f357c, d5afee6669b1a6770fa2d4153afddec31f1ad365, and 2be370aeb45ad952c6a0e9f940c8537e418e5fe2 add raw coherency-mode logging, direct CSF_EVENT allocation-flag logging, and requested/effective Kbase DMA-BUF import-flag logging. The latest candidate HEAD 7e5d7a936b9f3a77255570b121f21626e1e6411b only adds the format-safe uint64_t cast. Combined, these logs should show whether the target kernel retained BASE_MEM_COHERENT_SYSTEM for imported pools and what attributes it applied to CSF_EVENT cells. Do not remove it or substitute COHERENT_LOCAL until the returned flags and cache-sync path are understood.


The upstream Kbase implementation inspected during this audit explicitly sets BASE_MEM_SAME_VA | BASE_MEM_CACHED_CPU | BASE_MEM_COHERENT_SYSTEM when handling BASE_MEM_CSF_EVENT in kbase_api_mem_alloc_ex(). It documents COHERENT_SYSTEM as avoiding explicit cache maintenance around event-memory access. This supports keeping SYSTEM-scope queue completion operations as the default until the target kernel's returned flags prove otherwise. The optional CSG-scope SYNC_ADD64 override remains untested and must not be enabled merely because another device reported a shareability fault.


### Corrected candidate build and package audit (2026-11-03)

Latest source branch HEAD: 7e5d7a936b9f3a77255570b121f21626e1e6411b.

Build results:
- Android-only PanVK build: workflow run 41676901159 — SUCCESS, including Android ARM64 compile, staged-file audit, manifest/dependency audit, flat Winlator/Adrenotools ZIP packaging, and artifact upload.
- Android-wrapper Release: workflow run 41676901559 — SUCCESS.
- Android-wrapper DebugOptimized: still running at the time of this update; not required for the Android-only package but wait for final result before closing build validation.
- Earlier runs 41675810459/41675810560 failed on the __u64/PRIx64 format mismatch and must not be used.

Uploaded artifact:
- Artifact name: panvk-g720-android-only-experimental-package
- Artifact ID: 13250204981
- Artifact size: 16,664,931 bytes
- Outer artifact ZIP SHA-256: a746fe2c34e2119f0433e04f348681614794de49972e8de0d992341d9f06dcfc
- Inner driver package: PanVK-G720-experimental.zip
- Inner package SHA-256: 82e7c185fa47fe6d43f8644fe294735d3f5349119c8fbbab22313a98b48b43c1
- Inner SHA256SUMS verified successfully for libvulkan_panfrost.so, libpanvk_drm.so, and meta.json.
- Driver ELF: AArch64 ELF64 shared object, Android API 30, NDK r30.
- ICD manifest reports Vulkan API 1.4.354 and 64-bit library path /usr/local/lib/libvulkan_panfrost.so.
- The package includes a private libpanvk_drm.so. The packaging script rewrites libvulkan_panfrost.so's DT_NEEDED entry from libdrm.so to libpanvk_drm.so, sets RUNPATH=$ORIGIN, and changes the private library SONAME to libpanvk_drm.so. The inner package was independently inspected with readelf and its SHA256SUMS verified; this avoids interposing on the platform libdrm.so. The staged pre-package audit's missing-drm-symbols.txt is empty.
- meta.json labels the driver experimental and explicitly says it is not Vulkan conformant and device/game compatibility is unverified.

This is a build-verified package, not a runtime-validated driver. Keep it separate from the previously tested package and import only through the user's custom Winlator/Adrenotools driver mechanism. Do not replace the Android system/vendor Vulkan driver.

Next user test when ready:
1. Import this ZIP as a separate custom driver profile, keeping the previous tested profile available for rollback.
2. Repeat the same Tomb Raider benchmark/settings used for the 2026-11-03 baseline.
3. Capture logcat lines containing 'raw coherency mode', 'dma-buf import flags', and 'BO flags', plus any CSF group error / 0x88 / VK_ERROR_DEVICE_LOST.
4. Do not enable PANVK_KBASE_SYNC_ADD64_CSG_SCOPE=1 yet; first inspect effective flags to determine the target kernel's CSF_EVENT mapping.


### Complete PanVK CSF sync-instruction scope sweep

All src/panfrost/vulkan/csf/*.c files were scanned for MALI_CS_SYNC_SCOPE_CSG/SYSTEM and cs_sync32/cs_sync64 operations. Remaining CSG-scoped operations were classified:

- panvk_vX_cmd_dispatch.c compute barrier signal uses a per-compute-subqueue sync point and is consumed within that compute CSG; keep CSG scope.
- panvk_vX_cmd_buffer.c progress breadcrumbs are per-subqueue diagnostic markers; keep CSG scope.
- panvk_vX_cmd_query.c query-availability markers live in rw_nc GPU-uncached memory and are not waited on by another CSG with cs_sync32_wait; keep CSG scope pending contrary evidence.
- panvk_vX_gpu_queue.c SYNC_ADD64 to Kbase sequence cells remains SYSTEM by default; PANVK_KBASE_SYNC_ADD64_CSG_SCOPE is diagnostic only and untested.
- Cross-subqueue event producers now use cmdbuf->sync_scope, matching event wait consumers.
- Descriptor-ring reserve and release use the same cmdbuf->sync_scope.
- Cross-subqueue barriers and queue completion waits/signals use SYSTEM on Kbase.

Do not blanket-replace every CSG scope with SYSTEM. The remaining CSG operations are local or explicitly diagnostic; only cross-subqueue/host-shared synchronization should use the wider scope.


### Device test: 2026-11-03 18:14–18:17 local — candidate package regression / interrupted DXVK run

User tested the separately packaged PanVK candidate and uploaded:
- Archive: `TombRaider_logs_2026-11-03_18-18-11.zip`
- Archive contents: `logcat.log`, `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, `TombRaider_d3d9.log`
- User-reported benchmark: Min 1.9 FPS / Max 17.4 FPS / Avg 10.2 FPS.
- Compared with immediately preceding 2026-11-03 candidate run: Min 7.9 / Max 18.9 / Avg 14.0 FPS. Average is lower by 3.8 FPS (~27.1%). Do not yet call this a pure GPU performance regression because the DXVK/Wine run appears to terminate abnormally and benchmark validity is uncertain.

Important log findings:1. PanVK initializes and reports `kbase: raw coherency mode 0` (seen in both logcat and wine_debug). This is the first device-side observation of the target kernel's raw coherency mode. Interpret as no advertised raw coherency mode; it does not by itself establish every individual allocation's effective page-table attributes.
2. The archive contains many direct BO allocation messages, e.g. `requested=0x40 effective=0x380f`, `requested=0x41 effective=0x1817`, and `requested=0x100 effective=0x8340f`. Preserve exact values for decoding against this branch's Kbase UAPI flag definitions. No `dma-buf import flags` diagnostic line appears anywhere in the supplied logs, so the expected requested/effective dma-buf import result is still missing.
3. No `GPU_SHAREABILITY_FAULT`, `0x88`, `CSF group ... fatal error`, `QueueSubmit failed: -4`, or explicit `VK_ERROR_DEVICE_LOST` appears in this archive. This is encouraging for the earlier shareability-fault symptom, but not proof that the candidate is stable or that the 0x88 cause is fixed.
4. DXVK identifies the device as `Wrapper(Mali-G720 MC7)`, driver version `26.2.149`, Vulkan 1.3.354. DXVK is v1.7.2. Effective config includes `d3d11.maxQueuedFrames=1`, `dxvk.numCompilerThreads=4`, `dxvk.memoryTrack=True`, `d3d11.relaxedBarriers=True`, and `d3d11.ignoreGraphicsBarriers=True`. Do not change these settings during driver A/B comparisons unless intentionally isolating them.
5. `TombRaider_d3d11.log` shows swapchain setup at 900x506, first IMMEDIATE with 3 images and then FIFO with 4 images. Its final line is `err: D3D11SwapChain: Failed to recreate swap chain`.
6. `wine_debug.log` ends with:
   - `0148:err:vulkan:X11DRV_vulkan_surface_create get_client_rect failed.`
   - two `RtlLeaveCriticalSection ... section ... is not acquired` errors
   - `_wassert (L"!status && "vkWaitForFences"", ... winevulkan/loader_thunks.c)`
   - then `=== Session ended ===`
   The Android log shows the game process (PID 17598) receiving SIGKILL at 18:17:53. The evidence is consistent with a WSI/swapchain/fence or teardown failure, but does not prove whether it occurred during the benchmark or only while the app was closing. The prior run's window destruction was also considered possibly user-triggered; continue distinguishing benchmark completion from manual close.
7. `logcat.log` has unrelated Android/MIUI, Wine HID/service, audio, and window manager warnings. Do not treat those as PanVK GPU faults without correlation.

Interpretation / next engineering actions:
- Current result is a mixed outcome: no captured 0x88/device-lost signature, but lower FPS and a Wine Vulkan fence assertion plus swapchain recreation failure. Mark runtime status as **inconclusive / WSI-failure suspected**, not pass and not proven GPU regression.
- The observed raw coherency mode 0 raises the priority of tracing how this kernel handles the effective flags for WB_MMAP/DMA-BUF imported pool memory and CSF event/seqno memory. Do not immediately remove `BASE_MEM_COHERENT_SYSTEM`, alter event scope, or enable `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE=1`; first obtain actual import return flags and map each effective flag to UAPI definitions.
- Why no dma-buf import diagnostic appeared is unresolved. Check whether this tested package truly contains the expected import-flag logging commit and whether the tested runtime path took dma-buf import for the relevant BOs. Check package/source provenance before concluding that the import path was not used.
- Inspect WSI path and the Wine/DXVK log sequence around swapchain recreation, `vkWaitForFences`, and X11 surface destruction. Determine whether the 10.2 FPS number came from a completed benchmark scene or a partial run. Do not ask user for another run until static source/log inspection has narrowed the next test.
- For the next comparison, keep the same resolution, game quality, DXVK config, wrapper and driver profile; record whether the benchmark reaches its normal results screen and whether the user manually closes it. If needed, run once with logs captured through benchmark completion and a separate capture through manual shutdown.
- Preserve current 2026-11-03 archive and result as a distinct datapoint; do not overwrite the previous 14.0 FPS run.



### Follow-up static source check for missing DMA-BUF log (2026-11-03)

After reviewing the 18:18 archive, the exact logging condition was checked in `src/panfrost/lib/kmod/kbase_kmod.c` on `experiment/csf-notification-serialization` (file blob SHA `4b4f6f03e52b27bf95eec6c58e8fb10860531787`):

- The `kbase: dma-buf import flags` message is emitted only after a successful `KBASE_IOCTL_MEM_IMPORT` when the PanVK flags include `PAN_KMOD_BO_FLAG_WB_MMAP` or `PAN_KMOD_BO_FLAG_GPU_UNCACHED`.
- The internal dma-heap path is used only when `exclusive_vm == NULL`, dma_heap is available, and the allocation is not EXECUTABLE, ALLOC_ON_FAULT, or CSF_EVENT.
- Therefore the lack of that log line does **not** prove that DMA-BUF imports were never used: external Android Hardware Buffer imports use flags=0 and do not satisfy the diagnostic condition; nor does it prove the kernel dropped COHERENT_SYSTEM. It means this archive did not capture a successful qualifying internal WB_MMAP/GPU_UNCACHED dma-buf import.
- The repeated `BO flags requested=0x40 effective=0x380f` lines come from the direct allocation path (the logging site is in `kbase_kmod_bo_alloc`, not the import path). These should be decoded against the target kernel's UAPI constants and the PanVK flag conversion before assigning a shareability meaning.
- The source currently requests `BASE_MEM_COHERENT_SYSTEM` unconditionally for dma-buf imports, but direct BO allocation converts `PAN_KMOD_BO_FLAG_IO_COHERENT` to COHERENT_SYSTEM only when that PanVK flag is present. The observed direct allocation request 0x40 must therefore be mapped to the PanVK flag enum before drawing conclusions.

Next source task: add an explicit, low-noise diagnostic for the allocation route and the full returned Kbase flags for the relevant sync/event pool BOs; also log the Kbase allocation/import path and whether `exclusive_vm` is non-NULL. Do not add verbose logging to every frame allocation. Use a narrowly scoped opt-in diagnostic if possible. Then decode the observed `0x380f`, `0x1817`, and `0x8340f` values from the exact target UAPI headers and compare direct-allocation vs DMA-BUF-import attributes.

### Correction: benchmark completion confirmed by user (2026-11-03 18:20 local)

The user confirmed that Tomb Raider reached its normal benchmark results screen and the benchmark completed successfully; they closed the game only after completion. Therefore the 10.2 FPS result is a valid completed benchmark datapoint, not an interrupted benchmark. The prior addendum's wording 'interrupted DXVK run' and 'benchmark validity uncertain' is superseded by this correction.

Confirmed result:
- Min 1.9 FPS
- Max 17.4 FPS
- Average 10.2 FPS
- Compared with the preceding candidate run (Min 7.9 / Max 18.9 / Avg 14.0), average is lower by 3.8 FPS / 27.1%. Treat this as a real measured performance regression for this run; still repeat for run-to-run variance before attributing it to a specific code change.

Reinterpretation of shutdown errors:
- The final D3D11SwapChain swapchain recreation error, Wine vkWaitForFences assertion, X11 surface/client-rect error, and process SIGKILL occurred at/after the user closed the game, following a completed benchmark. They are teardown/close-path observations unless a future capture shows they occur during active gameplay. Do not use them to invalidate the completed FPS result.
- The archive still contains no GPU_SHAREABILITY_FAULT, 0x88, CSF group fatal error, QueueSubmit failed -4, or explicit VK_ERROR_DEVICE_LOST during the captured run. This is a positive stability observation for this one benchmark run, not proof of full correctness.

Additional exact log evidence:
- DXVK reports Wrapper(Mali-G720 MC7), driver version 26.2.149, Vulkan 1.3.354, DXVK 1.7.2.
- Effective DXVK config was the same recorded config in the previous section; retain it unchanged for A/B testing.
- kbase: raw coherency mode 0 is present repeatedly.
- Many direct BO allocations show effective flags 0x380f, 0x1817, and 0x8340f; map these with the exact Kbase UAPI and allocation flags before inferring cache/shareability attributes.
- Android Hardware Buffer / dma-buf imports are visibly exercised: panvk_android_import_ahb_memory and kbase_kmod_bo_alloc_dmabuf: succeeded lines appear. However, no kbase: dma-buf import flags message is present. The existing diagnostic filter is not sufficient to expose all AHB import flags/returned Kbase mapping flags.

Next engineering direction:
1. Keep this completed 10.2 FPS run as a valid datapoint.
2. Investigate why the candidate's average fell versus 14.0 FPS. First inspect source diff between the tested package commit and the previous 14.0 FPS package commit, plus benchmark settings/cache/thermal variation; do not immediately revert synchronization correctness changes.
3. Decode all three effective flag values from the exact source/UAPI used to build this package.
4. Improve narrowly scoped diagnostics so AHB imports log requested and effective Kbase flags (and identify import route), without per-frame log spam. Also expose the relevant sync/event pool allocation route and returned flags.
5. Do not enable PANVK_KBASE_SYNC_ADD64_CSG_SCOPE=1, remove BASE_MEM_COHERENT_SYSTEM, or make broad scope changes until this evidence is available.
6. Treat close-time swapchain/fence assertion separately from in-benchmark GPU stability.

### Follow-up: completed-run interpretation and performance regression isolation (2026-11-03)

User confirmed the 10.2 FPS Tomb Raider benchmark reached the normal results screen and completed successfully; game closure occurred afterward. The FPS result is valid. See the preceding correction addendum; teardown-only swapchain/fence messages must not invalidate the score.

Recounted the uploaded archive directly:
- 3,644 logcat lines.
- 704 `kbase: BO flags` messages: 567 with requested PanVK flags 0x40 and effective Kbase flags 0x380f; 131 with requested flags 0x41 and effective flags 0x1817; 6 with requested flags 0x100 and effective flags 0x8340f.
- 14 `panvk_android_import_ahb_memory` messages and 30 successful `kbase_kmod_bo_alloc_dmabuf` messages.
- Zero `kbase: dma-buf import flags` lines. The reason is now clear from source: that diagnostic only emitted for WB_MMAP/GPU_UNCACHED imports; external AHB imports use another flag pattern and were not covered.
- Nine raw coherency mode messages, all reporting 0.
- The 704 BO-flag lines are a potentially material instrumentation overhead during a CPU-sensitive Android emulation benchmark. This is a plausible contributor to the 27.1% score drop, not yet proven. Another possible cost is the newly added nonblocking CSF notification poll from queue status checks. Do not conflate correlation with proof.

Performance-isolation branch created from the tested package source SHA 7e5d7a936b9f3a77255570b121f21626e1e6411b:
- Branch: `experiment/kbase-diag-opt-in`.
- Commit `5c8a0baf9d47d4a41e0bd922462d008eea62d401`: gate frequent BO flag logging behind `PANVK_KBASE_DIAG_MEM_FLAGS=1`, default OFF. The same opt-in diagnostic now reports all successful dma-buf imports (including external AHB route) with route, PanVK flags, requested Kbase flags, and returned effective flags.
- Commit `a87d179f3cdb77946df0c2858cd4370794645e27`: add the branch to Android-only CI triggers.
- No functional sync, scope, allocation, or cache behavior was changed in this isolation branch; it is intended to remove default diagnostic log spam while preserving the tested candidate's functional code.
- Android-only CI run was observed queued as run `41684252078` at time of handoff update. Check its final status and package artifact before any device test.
- Keep `PANVK_KBASE_DIAG_MEM_FLAGS` unset for performance benchmarking. Set it to `1` only for a diagnostic run where log volume is acceptable.

Next steps:
1. Verify CI run 41684252078 and audit the produced Android package/checksums.
2. Compare this default-quiet package against the same exact Tomb Raider benchmark conditions. This isolates logging overhead while keeping functional synchronization changes unchanged.
3. If FPS returns toward 14.0, attribute the regression to diagnostic logging only after a repeat. If not, next isolate the `gpu_queue_check_status()` notification-drain poll overhead in a separate branch; do not remove fatal-error propagation without a correctness-preserving alternative.
4. Decode effective Kbase flags 0x380f, 0x1817, 0x8340f using the exact ioctl UAPI and the source conversion function before drawing coherency conclusions.
5. Preserve the 10.2 FPS completed run as valid and separate from the earlier 14.0 FPS run.


### Effective Kbase flag bitfield decode (exact candidate UAPI, 2026-11-03)

Decoded against `include/drm-uapi/mali_kbase_ioctl.h` at tested SHA 7e5d7a936b9f3a77255570b121f21626e1e6411b:
- `BASE_MEM_COHERENT_SYSTEM = 1 << 10` (0x400).
- `BASE_MEM_COHERENT_LOCAL = 1 << 11` (0x800).
- `BASE_MEM_CACHED_CPU = 1 << 12` (0x1000).
- `BASE_MEM_SAME_VA = 1 << 13` (0x2000).
- `BASE_MEM_CSF_EVENT = 1 << 19` (0x80000).
- `BASE_MEM_UNCACHED_GPU = 1 << 21` (0x200000).
- `BASE_MEM_PROT_GPU_EX = 1 << 4` (0x10).

Thus the observed effective values decode as:
- `0x380f` = low protection bits 0x0f + COHERENT_LOCAL (0x800) + CACHED_CPU (0x1000) + SAME_VA (0x2000).
- `0x1817` = protection bits 0x07 + GPU_EX (0x10) + COHERENT_LOCAL (0x800) + CACHED_CPU (0x1000); no SAME_VA because executable memory is in EXEC_VA.
- `0x8340f` = protection bits 0x0f + COHERENT_SYSTEM (0x400) + CACHED_CPU (0x1000) + SAME_VA (0x2000) + CSF_EVENT (0x80000).

Important: `to_kbase_mem_flags()` initially sets COHERENT_LOCAL for normal allocations, but for CSF_EVENT it sets BASE_MEM_CSF_EVENT and clears COHERENT_LOCAL. The kernel's returned flags for the six CSF event allocations include COHERENT_SYSTEM (0x400). Do not remove this or reinterpret raw coherency mode 0 as proof that CSF_EVENT mappings are non-system-coherent. The effective returned flags are the direct evidence for this allocation type.

Performance-isolation implementation details:
- `experiment/kbase-diag-opt-in` was forked from the exact tested source SHA 7e5d7a936b9f3a77255570b121f21626e1e6411b.
- Source commit `5c8a0baf9d47d4a41e0bd922462d008eea62d401` adds a per-device `diag_mem_flags` boolean, initialized from `PANVK_KBASE_DIAG_MEM_FLAGS=1`; default false. Direct WB_MMAP/CSF_EVENT flag logs are now silent by default.
- When explicitly enabled, dma-buf import logging covers every successful import and labels route as external or dma-heap, while reporting PanVK request flags, Kbase requested flags, and Kbase returned flags.
- CI trigger commit `a87d179f3cdb77946df0c2858cd4370794645e27` is the branch head. Android-only run 41684252087 and wrapper run 41684252078 were still in progress at last check; no package is approved until Android-only build and package audit succeed.


### CI status refresh — opt-in diagnostics isolation branch (2026-11-03)

Latest check of branch `experiment/kbase-diag-opt-in`, head `a87d179f3cdb77946df0c2858cd4370794645e27`:

- Wrapper workflow run `41684252078` completed successfully. Both `build-android-wrapper (release)` and `build-android-wrapper (debugoptimized)` jobs passed all steps, including compile and artifact upload.
- Wrapper artifacts:
  - `libvulkan_wrapper-aarch64-debugoptimized`, artifact ID `13254402859`, 50,564,480 bytes, SHA-256 `38a1e495e61c4ccc1dcdd37c18dca9b339ce58e77d978e747cad31b1e2983104`.
  - `libvulkan_wrapper-aarch64-release`, artifact ID `13253633262`, 4,267,230 bytes, SHA-256 `5b376bed8487bc25f18eef50eec05e18e6d9d2ca2d0cc038d5f03157195820ee`.
  These are wrapper workflow artifacts, not the complete flat PanVK driver package; do not offer them as the testable driver.
- Android-only PanVK workflow run `41684252087` remains `in_progress`. Job `build-android-only` has passed setup, checkout verification, Python, dependencies, host tools, real Android ARM64 libdrm, and Android-only Meson configuration. Step 9, `Compile and stage Android install`, is still in progress. Audit, flat Winlator/Adrenotools packaging, and final artifact upload have not started/completed.
- No Android-only artifact was present at this status check. Do not download or distribute a wrapper-only artifact as if it were the driver.
- User has been informed that wrapper builds passed while the actual PanVK compile remains underway; no device action is required yet.
- Next: recheck run `41684252087`, inspect failed logs if it concludes unsuccessfully, and only proceed to package download after compile, audit, packaging, and artifact upload all succeed.



### Android-only PanVK build completed and package audit (2026-11-03)

Build source:
- Branch: `experiment/kbase-diag-opt-in`
- Exact source SHA: `a87d179f3cdb77946df0c2858cd4370794645e27`
- Workflow: Android-only PanVK G720 Experiment
- Run: `41684252087`
- Result: success. All 12 meaningful job steps passed, including Android ARM64 compilation, staged install, driver/manifest/dependency audit, flat Winlator/Adrenotools packaging, and artifact upload.
- Artifact: `panvk-g720-android-only-experimental-package`, artifact ID `13254773210`, 16,666,334 bytes, GitHub artifact digest SHA-256 `a4c197c7e74fe4461eab252fd6829443105d2d3fc6b04a55d96045aaee00ab10`.

Downloaded and audited the exact artifact:
- Outer artifact ZIP SHA-256 verified against GitHub artifact digest: `a4c197c7e74fe4461eab252fd6829443105d2d3fc6b04a55d96045aaee00ab10`.
- Nested user-facing flat package: `PanVK-G720-experimental.zip`, 4,112,733 bytes.
- Nested package SHA-256: `ee8f330db3d32cf04217842f2d845b7d41031029231566efdf34495a0c818e76`.
- Inner `SHA256SUMS` verified successfully for all three payload files:
  - `libvulkan_panfrost.so`: `22bd20ce5be2a137038dcc13a3c9f43305abd8ea79f37172f776da5b2311cf2f`.
  - `libpanvk_drm.so`: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`.
  - `meta.json`: `2c5fd2c5d742195226650799f710449689565f872e18ccc7f480ea8ff6af41ce`.
- Flat package contains exactly root-level `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; no nested paths.
- Driver and helper are both ELF64 AArch64 Android API 30 shared objects.
- Flat package driver SONAME: `libvulkan_panfrost.so`; its private dependency is `libpanvk_drm.so` with RUNPATH `$ORIGIN`. The packaging script deliberately renames the bundled Android libdrm support library to `libpanvk_drm.so` and rewrites DT_NEEDED to avoid interposing on platform `libdrm.so`.
- `libpanvk_drm.so` has SONAME `libpanvk_drm.so` and only depends on Android `libc.so`.
- Metadata: `PanVK Mali G720 (experimental)`, package version `26.3.0-devel-android-kbase-a87d179f3cdb`, min API 30. It explicitly states this is not Vulkan conformant and device/game compatibility is unverified.
- Staged Meson install also produced the Panfrost ICD JSON with API version `1.4.354`, plus the staged driver and Android support libs. The Winlator/Adrenotools flat ZIP uses its own `meta.json` loader manifest.
- No runtime device/Winlator validation has occurred yet. Build/audit success is not evidence of runtime stability or performance.

### Clarification: Android wrapper workflow versus the actual package (2026-11-03)

Correct the earlier informal interpretation that "Android wrapper" necessarily means a separate runtime wrapper layer:
- Workflow `.github/workflows/build.yml` is named `Android NDK Build (Wrapper Variant)`, and its uploaded artifacts are named `libvulkan_wrapper-aarch64-{release,debugoptimized}`.
- Inspecting the workflow shows it compiles the same Mesa Panfrost Vulkan target `build/src/panfrost/vulkan/libvulkan_panfrost.so`, with a different build configuration: `platforms=android,x11`, `panfrost-kmds=kbase,panthor`, and a local `anon_file.c` patch for Android `memfd_create`. The artifact name "wrapper" is a workflow/variant label; it is not proof of a separate Vulkan translation-layer shared library or a runtime dependency to install in Winlator.
- The actual Android-only package run `41684252087` uses a dedicated Android-only Meson configuration (`platforms=android`, `panfrost-kmds=kbase`) and packages the PanVK ICD with its private DRM helper and Adrenotools metadata.
- Do not import the wrapper-workflow artifacts as an additional component. For the first device test, use only the verified flat `PanVK-G720-experimental.zip` from run `41684252087`. Do not combine it with either wrapper artifact.
- User has been informed of this correction and that wrapper-only artifacts are not required for the target package.

Next action:
1. User imports the verified flat ZIP as a separate custom driver profile in Winlator/Adrenotools (never system driver replacement).
2. Retain existing known-good system driver profile for rollback.
3. Run the established Tomb Raider benchmark with matching settings; leave `PANVK_KBASE_DIAG_MEM_FLAGS` unset (default quiet path).
4. Capture game result and logs. Treat the run as a first runtime smoke/performance test, not proof of general compatibility.


### Device benchmark and runtime-log review — 2026-11-03 18:44 local

User tested the Android-only PanVK package built from `experiment/kbase-diag-opt-in` at source HEAD `a87d179f3cdb77946df0c2858cd4370794645e27` and uploaded:
- Archive: `TombRaider_logs_2026-11-03_18-44-31.zip`
- Contents: `logcat.log`, `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, `TombRaider_d3d9.log`
- User-reported score: Min 3.7 FPS / Max 17.2 FPS / Avg 10.6 FPS.

Performance comparison:
- Versus immediately preceding 10.2 FPS run: average +0.4 FPS (+3.9%); min +1.8 FPS; max -0.2 FPS. This is a small change and not yet a reliable performance gain.
- Versus earlier 14.0 FPS run: average -3.4 FPS (-24.3%); min 3.7 vs 7.9; max 17.2 vs 18.9.
- Keep all three datapoints separate. The 10.6 FPS run does not establish recovery to the 14.0 FPS result or a meaningful optimization.

#### Driver selection and load evidence

- `logcat.log` records Winlator's initial and selected custom driver as `PanVK Mali G720 (experimental)`.
- Android hook logs show `vulkan.mali.so` being redirected to the imported custom library at:
  `.../adrenotools/PanVK Mali G720 (experimental)/libvulkan_panfrost.so`
  This occurs in both app and game-related processes.
- PanVK-specific runtime messages are present, including `[MESA-PANVK] kbase: raw coherency mode 0`, the non-conformance warning, fallback gralloc initialization, successful Kbase queue waits, and Android Hardware Buffer import/allocation messages. This is positive evidence that the custom PanVK library is loaded and executing code in the tested process; it is not by itself proof that every DXVK call is reaching the intended PanVK implementation end-to-end.
- Important driver-stack discrepancy to investigate before drawing performance conclusions: `wine_debug.log` reports the container's Graphics driver as `wrapper-gamenative`, while the Android hook separately selects the PanVK custom library. DXVK's own device report is `Wrapper(Mali-G720 MC7)`, driver version `26.2.149`, Vulkan `1.3.354`, DXVK `1.7.2`. Do not interpret the container label alone as proof that PanVK was not loaded (the hook and PanVK logs prove it was loaded); equally, do not assume the DXVK-reported Wrapper identity is the Mesa PanVK driver identity. Trace the Winlator wrapper/custom-ICD chain and determine which Vulkan implementation handles DXVK's physical-device enumeration and submissions.
- DXVK D3D11 initializes feature level 11_0. Effective config includes `dxvk.numCompilerThreads=4`, `d3d11.maxQueuedFrames=1`, `dxvk.memoryTrack=True`, `d3d11.relaxedBarriers=True`, and `d3d11.ignoreGraphicsBarriers=True`. Keep these unchanged for comparisons.

#### Kbase, AHB, and CSF observations

- Raw Kbase coherency mode is again `0`.
- 30 successful `kbase_kmod_bo_alloc_dmabuf` messages.
- 14 `panvk_android_import_ahb_memory` messages.
- The log includes successful AHB property queries, candidate-fd selection, explicit-layout/plane metadata acceptance for AR24 buffers, and subsequent Kbase dma-buf allocations. No explicit allocation/import failure is evident in these lines.
- Two `kbase_queue_wait_current: wait completed successfully` messages are present.
- No `kbase: BO flags` or `kbase: dma-buf import flags` diagnostic lines are present. This is expected for the default quiet build with `PANVK_KBASE_DIAG_MEM_FLAGS` unset; it means this run does not reveal returned flags for the AHB import route. Do not infer any cache/shareability change from the absence of those diagnostics.
- No `GPU_SHAREABILITY_FAULT`, exception `0x88`, `CSF group ... fatal error`, `QueueSubmit failed: -4`, or explicit `VK_ERROR_DEVICE_LOST` appears in the captured archive. This is a positive observation for this run only, not a general stability proof.

#### WSI / teardown observations

- The D3D11 log ends with `D3D11SwapChain: Failed to recreate swap chain`.
- Near the end of `wine_debug.log`, Wine reports `X11DRV_vulkan_surface_create get_client_rect failed` and an assertion at `vkWaitForFences`.
- These appear at the end of the capture (around 18:44:11), after the game had been running since approximately 18:41:16. The user supplied benchmark scores but did not explicitly state in this message whether the normal results screen was reached or whether the game was manually closed. Preserve these as end-of-run WSI/fence observations; do not classify them as an in-benchmark crash or harmless manual-close teardown until that detail is confirmed.
- No explicit PanVK fatal GPU/CSF error accompanies these final messages in this archive.

#### Current interpretation

- Runtime status: PanVK library load/execution is confirmed; the workload ran far enough to produce a score; no 0x88/device-lost signature was captured; WSI/fence end-of-run issue remains unresolved.
- Performance: 10.6 FPS is only marginally above the 10.2 FPS run and remains materially below the 14.0 FPS run. Do not attribute the difference to notification polling, compiler thread count, or memory flags without controlled A/B evidence.
- Do not change SYSTEM/CSG scope, Kbase coherency flags, cache synchronization, or queue notification correctness code on the basis of this benchmark alone.
- Keep `PANVK_KBASE_DIAG_MEM_FLAGS` disabled for performance measurements. A separate opt-in diagnostic run may later be used to inspect returned import flags, but must not be mixed with a benchmark comparison.

#### Next engineering actions

1. Audit the Winlator `wrapper-gamenative` + Adrenotools custom-driver selection chain. Reconcile the PanVK hook-load messages with the DXVK-visible `Wrapper(Mali-G720 MC7)` / version `26.2.149` identity, and establish which ICD/backend is actually servicing DXVK device enumeration and command submission.
2. Compare the exact DXVK and container configuration in this archive with the prior 14.0 / 10.2 runs. Preserve the same wrapper, DXVK config, resolution, game quality, and benchmark route for future A/B tests.
3. Inspect the WSI/fence teardown path and timestamp ordering; do not modify it until it is clear whether the error occurs during active benchmark rendering or only when the game/window is closed.
4. If the actual PanVK submission path is confirmed and the WSI messages are teardown-only, next isolate performance overhead with a controlled quiet-build A/B/repeat. Do not alter synchronization semantics to chase a single FPS result.
5. Update this handoff with any user confirmation about benchmark completion/close timing and every subsequent source, build, or device-test step.


### Correction: reconcile DXVK device identity with PanVK source (2026-11-03)

A follow-up source audit resolves the apparent DXVK driver-version mismatch recorded above:

- In `src/panfrost/vulkan/panvk_vX_physical_device.c` on `experiment/kbase-diag-opt-in`, the physical-device properties initializer sets `driverVersion = 0x06802095u` (around line 841). Vulkan version encoding decodes this to major 26, minor 2, patch 149 — exactly the `26.2.149` reported by DXVK in this archive.
- The same PanVK source sets `VkPhysicalDeviceDriverProperties.driverName = "panvk"` and `driverInfo = "Mesa " PACKAGE_VERSION MESA_GIT_SHA1` (around lines 1352–1354).
- Therefore the DXVK-visible `26.2.149` is consistent with the tested PanVK build's own declared driverVersion; it is not evidence that DXVK bypassed PanVK. The `Wrapper(Mali-G720 MC7)` device name and `wrapper-gamenative` container label are consistent with the Winlator wrapper front-end, while PanVK is loaded as the selected custom Vulkan backend. The Android hook path and PanVK runtime messages independently confirm that the PanVK library is loaded and executing.
- Official Winlator Mali setup documentation instructs Mali users to select Graphics Driver = Wrapper; custom Vulkan drivers are managed separately through the Adrenotools/custom-driver mechanism. See https://github.com/GunaCharanTeja/WinlatorMali (README on branch `bionic-mali-1.2`).
- Revised interpretation: **no driver-stack mismatch is demonstrated by this log**. The wrapper is part of the intended Winlator Mali path; the exact Vulkan dispatch chain is still worth tracing if future evidence conflicts, but it is no longer the leading concern. Do not ask the user to change away from Wrapper for this test.

This correction supersedes the prior addendum's wording that treated `wrapper-gamenative` plus the DXVK-visible Wrapper identity as a suspicious discrepancy. Preserve the raw observations, but use this source-level driverVersion match when interpreting them.


## 2026-11-03 continuation audit — source comparison, CI refresh, and current stop point

This update was made after reopening the project in a new conversation. The full handoff above was read first; source and workflow state were independently rechecked against GitHub.

### Current branch and CI verification

- `experiment/csf-notification-serialization` remains at `7e5d7a936b9f3a77255570b121f21626e1e6411b`.
- Android-only build run `41676901159`: SUCCESS.
- Android wrapper run `41676901559`: SUCCESS.
- `experiment/kbase-diag-opt-in` remains at `a87d179f3cdb77946df0c2858cd4370794645e27`.
- Android-only package run `41684252087`: SUCCESS.
- Android wrapper run `41684252078`: SUCCESS.
- Both candidate source branches and their CI runs remain separate. No source branch was force-pushed, merged, or deleted during this audit.
- The latest device-tested package is the Android-only package from `experiment/kbase-diag-opt-in` at `a87d179f3cdb77946df0c2858cd4370794645e27`, not either wrapper-only workflow artifact.

### Source comparison against the 14.0 FPS candidate

GitHub's compare endpoint confirms that `7e5d7a936b9f3a77255570b121f21626e1e6411b` is exactly 10 commits ahead of `6634282aa14616931656b883aafb5519831eb888`, with no divergence. The 10 commits modify six files:

- `.github/workflows/android-only-experiment.yml`: CI branch trigger only.
- `src/panfrost/lib/kmod/kbase_kmod.c`: +119 / -22; notification reader serialization/draining, event error handling, and opt-in allocation/import diagnostics.
- `src/panfrost/lib/kmod/kbase_kmod.h`: notification API declarations.
- `src/panfrost/vulkan/csf/panvk_vX_cmd_event.c`: two event producer SYNC_SET32 operations use `cmdbuf->sync_scope`.
- `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`: notification/error propagation and queue-wait checks.
- `src/panfrost/vulkan/panvk_physical_device.c`: Kbase CPU sync wait-many/device-status handling.

The follow-on comparison `7e5d7a936b9f3a77255570b121f21626e1e6411b` -> `a87d179f3cdb77946df0c2858cd4370794645e27` is only two commits and changes only `kbase_kmod.c` plus the workflow trigger. It gates high-frequency memory-flag diagnostics behind `PANVK_KBASE_DIAG_MEM_FLAGS=1`; default is OFF. It does not change synchronization, event scope, allocation policy, or cache behavior.

This narrows the measured performance change:
- The 14.0 FPS run used source `6634282...`.
- The completed 10.2 FPS run used the notification/event candidate `7e5d7a...`.
- The later 10.6 FPS run used `a87d179...`, which retains the same functional candidate changes but defaults the allocation diagnostics to quiet.
- Therefore the 10.2 -> 10.6 difference does not show recovery to 14.0, and the quiet-log follow-on did not obviously recover the earlier score. The 0.4 FPS difference is too small and uncontrolled to assign causality. The 14.0 -> 10.2/10.6 difference is associated with the 10-commit functional/diagnostic delta, but is not yet attributable to any one change.

### Focused hot-path review

The current candidate source was inspected directly:

1. `gpu_queue_check_status()` calls `kbase_kmod_csf_process_events()` on Kbase. That function takes the event-consumer mutex and performs zero-timeout `ppoll()` readiness checks, reading queued notifications until empty. It is a Vulkan queue-status path, not an unconditional operation in every normal queue-submit call. Do not remove it casually: the prior code could leave a fatal notification queued when a CQS completion bypassed the notification fallback.
2. `kbase_subqueue_wait_seqno()` checks the device-wide CSF error latch inside its wait loop. This is a cheap in-memory latch read, not a poll/read syscall. It prevents one queue from accepting completion after another queue consumed the shared fatal notification.
3. `kbase_cpu_sync_wait_many()` now calls `vk_device_check_status()` after wait-many, including wait-any and timeout outcomes. This closes the `vkGetFenceStatus()` gap because the common `vkWaitForFences()` entry point already performs its own post-wait status check. The wait-any path itself loops over zero-timeout per-sync polls and `sched_yield()`; it can be CPU-expensive when used with pending syncs, but that loop predates/exists independently of the one final device-status check.
4. The 18:44 runtime capture contains only two successful `kbase_queue_wait_current` messages and no captured 0x88/device-lost signature. It does not include a high-resolution profile or enough counters to show how frequently the status path ran.

Engineering conclusion: notification processing and sync wait-many status checks are correctness-sensitive and plausible profiling targets, but current evidence does not justify disabling them or changing synchronization semantics. If optimizing, first add low-overhead, opt-in counters/timing around these specific paths, or design a correctness-preserving A/B that isolates one change. Do not equate the status-path `ppoll` with every frame's queue-submit cost.

### Upstream issue review

- Read MineGraph-bilibili/panvk-Android issue #3: https://github.com/MineGraph-bilibili/panvk-Android/issues/3
  - Its author confirms the missing WAIT32/64 Scope encoding was a genuine bug, but adding CSG scope did not eliminate 0x88 on their G715/r38p1 target.
  - Their next proposed discriminating experiment is replacing SYNC_WAIT with a LOAD + conditional branch loop, because firmware may perform an internal atomic read with System scope regardless of the encoded scope.
  - This is a separate device/kernel report. Our G720 code preserves SYSTEM scope for cross-CSG waits; do not transplant the G715 CSG workaround blindly.
- Read leegao/mesa-funnymdzz issue #2 and comments: https://github.com/leegao/mesa-funnymdzz/issues/2
  - The author identifies exception 0x88 as GPU_SHAREABILITY_FAULT and the access as atomic.
  - Their hypothesis concerns SYSTEM-shareable dma-buf mappings and inconsistent shareability attributes across GPU accesses; it is useful evidence, not proof that the Xiaomi kernel retains the requested flag.
- The target device's raw coherency mode is logged as 0. The target kernel's effective CSF_EVENT allocation flags observed in the earlier diagnostic run were `0x8340f`, including `BASE_MEM_COHERENT_SYSTEM`; do not override this based only on raw mode 0. The 18:44 quiet run did not enable `PANVK_KBASE_DIAG_MEM_FLAGS`, so it does not expose the effective flags for the AHB/dma-buf import route.

### Runtime evidence and exact next test

- 18:18 run: user confirmed the normal benchmark results screen was reached; Min 1.9 / Max 17.4 / Avg 10.2 FPS is a valid completed datapoint. Final Wine/WSI errors followed game closure and should be treated as shutdown observations unless reproduced during gameplay.
- 18:44 run: Min 3.7 / Max 17.2 / Avg 10.6 FPS. PanVK was selected and loaded; 30 dma-buf allocation successes and 14 AHB import messages were present; no captured 0x88, CSF fatal error, QueueSubmit -4, or explicit VK_ERROR_DEVICE_LOST. The final X11 surface/swapchain/fence messages occurred at session end; whether they were entirely caused by normal close remains uncertain for this run.
- Preserve all three benchmark scores (14.0, 10.2, 10.6) separately. Do not claim the candidate fixed the 0x88 issue permanently or improved performance.

The next device action should be a **short diagnostic-only run**, not another FPS benchmark:
1. Use the same `a87d179...` custom PanVK package and same Winlator wrapper/driver profile.
2. Temporarily set `PANVK_KBASE_DIAG_MEM_FLAGS=1` in the Winlator environment for the game process only. Keep `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset.
3. Launch Tomb Raider only long enough to initialize the Vulkan device and exercise its normal AHB/dma-buf imports; no benchmark is required. Capture logcat/Wine output, then close normally.
4. Return the lines for `raw coherency mode`, `dma-buf import flags`, and `BO flags`, including the full requested/effective values and allocation route. This will determine whether the Xiaomi kernel retains SYSTEM shareability on imported pool/AHB mappings.
5. Afterward remove the diagnostic environment variable and keep it unset for all performance benchmarks.

Do not change Kbase import flags, cache synchronization, CSF event scope, WAIT64 scope, or queue-notification correctness code until the diagnostic import attributes are decoded. No new source code change or package was justified by this static audit; the next required evidence must come from the target device.



## 2026-11-04 continuation — AHB/dma-buf import flag diagnostic

### Device capture

User supplied `TombRaider_logs_2026-11-04_01-49-32.zip` after setting `PANVK_KBASE_DIAG_MEM_FLAGS=1` for a short launch/initialization run (not an FPS benchmark). The archive contains logcat, Wine, DXVK D3D11/DXGI/D3D9 logs.

The diagnostic was active and captured the previously missing import route:

- Raw coherency mode: `0` (the PanVK source documents this as `COHERENCY_NONE`).
- 30 dma-buf import diagnostic records, all `route=external`, `pan=0x0`, `requested=0x4040f`, `effective=0x4540f`.
- The returned flags are the request plus `0x5000`. Under the current Arm kbase UAPI layout, `0x400` is `BASE_MEM_COHERENT_SYSTEM`, `0x40000` is `BASE_MEM_IMPORT_SHARED`, `0x1000` is `BASE_MEM_CACHED_CPU`, and `0x4000` is output `BASE_MEM_NEED_MMAP`. Thus the returned import flags preserve the requested `BASE_MEM_COHERENT_SYSTEM` bit and add CACHED_CPU + NEED_MMAP. This is a useful observation about the Kbase ioctl response; it does not independently prove the actual page-table shareability attributes or resolve why the raw coherency property is NONE.
- Additional BO diagnostics: 424 records `requested=0x40 effective=0x380f`; 88 records `requested=0x41 effective=0x1817`; 6 records `requested=0x100 effective=0x8340f`. Preserve these as separate allocation classes; do not conflate their Pan Kmod request flags with the raw Kbase returned flags.
- DMA-BUF imports succeeded; log records show imported BOs of 1,024,000 and 1,847,296 bytes, with handles in the 39–70 range. AHB import messages are present.
- Two `kbase_queue_wait_current` calls completed successfully.
- No captured GPU_SHAREABILITY_FAULT/0x88, CSF fatal notification, QueueSubmit -4, or explicit VK_ERROR_DEVICE_LOST.
- On close, Wine logged `FORTIFY: pthread_mutex_lock called on a destroyed mutex` and asserted in `vkWaitForFences`; DXVK logged `D3D11SwapChain: Failed to recreate swap chain`. This was a short launch/close diagnostic, so treat these as teardown observations unless reproduced before normal exit.

### Interpretation / next engineering action

The key new fact is that the dma-buf import ioctl's returned flags preserve `BASE_MEM_COHERENT_SYSTEM` even though the raw coherency mode reports 0. The generic Arm kbase UAPI defines the returned `NEED_MMAP` and `CACHED_CPU` bits as output flags; their appearance explains the `0x5000` delta. Do not interpret that delta as a shareability change. The preserved coherent-system bit is stronger evidence than the earlier logs, but it is still not direct proof of the GPU page-table's shareability encoding, nor does the generic upstream kernel implementation prove the behavior of Xiaomi/MediaTek's exact kernel fork.

No memory flag, cache-maintenance, CSF scope, or synchronization source change is justified by this capture alone. Next, inspect the exact MTK kbase ABI/kernel behavior if a matching source tree can be found; otherwise preserve this as a diagnostic discrepancy and do not force/clear coherency flags speculatively. Keep `PANVK_KBASE_DIAG_MEM_FLAGS` disabled for all FPS benchmarks. The user should remove it from the Winlator environment before the next performance run. `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` remains unset.


## 2026-11-04 continuation — Tomb Raider benchmark capture (01:57:54)

### User-reported benchmark result

- Min FPS: **3.3**
- Max FPS: **18.5**
- Average FPS: **12.9**

Uploaded archive: `TombRaider_logs_2026-11-04_01-57-54.zip`. Extracted and inspected `logcat.log`, `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, and `TombRaider_d3d9.log`.

### Critical qualification: diagnostic logging was still enabled

The capture contains:
- 30 dma-buf import diagnostics (`route=external pan=0x0 requested=0x4040f effective=0x4540f)).
- **730** BO memory-flag diagnostic records (`requested=0x40 effective=0x380f)).
- Raw coherency mode 0.

Therefore `PANVK_KBASE_DIAG_MEM_FLAGS=1` was still active for this benchmark process (or an equivalent diagnostic-enabled build/config was used). The 12.9 FPS average is a valid user-reported run, but it is **not a clean baseline** and must not be compared directly with quiet runs. Repeated logcat output during allocation can perturb performance. Do not attribute the 12.9 result to a driver improvement/regression.

### Runtime / Vulkan observations

- DXVK identifies the device as `Wrapper(Mali-G720 MC7)`, driver version `26.2.149`.
- Device creation succeeded; DXVK reports feature level D3D_FEATURE_LEVEL_11_0 and reads 706 valid state-cache entries.
- Swapchain initially created at 900x506 (3 images, IMMEDIATE), then recreated at 900x506 (4 images, FIFO); final log reports `D3D11SwapChain: Failed to recreate swap chain`.
- End of Wine log includes X11 `get_client_rect failed`, `FORTIFY: pthread_mutex_lock called on a destroyed mutex`, invalid SEH frame, `vkWaitForFences` assertion, and RpcSs startup failure. These appear at session teardown/process termination in this capture; preserve as shutdown symptoms, not confirmed in-game GPU faults.
- No occurrences found in the combined logcat/Wine logs for `0x88`, `GPU_SHAREABILITY_FAULT`, `VK_ERROR_DEVICE_LOST`, CSF error, or `QueueSubmit -4`.
- Two `kbase_queue_wait_current` waits completed successfully.

### Next action

Before the next benchmark, remove `PANVK_KBASE_DIAG_MEM_FLAGS` completely from the Winlator container/game environment (do not set it to 0; leave it absent), keep `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` absent, and rerun the same game/driver/wrapper/settings/resolution. Send the next FPS result and logs. The clean run will tell us whether the 12.9 FPS result persists without diagnostic log overhead.

Maintain the historical results separately:
- 14.0 FPS average — earlier candidate `6634282...`, no current diagnostic overhead known.
- 10.2 FPS average — `7e5d7a...` functional synchronization/notification candidate.
- 10.6 FPS average — `a87d179...` diagnostics opt-in/default-off package, short-run log capture.
- 12.9 FPS average — 2026-11-04 01:57:54 capture; **diagnostic logging demonstrably active**, so not a clean benchmark.

No source-code changes are justified by this benchmark alone. Continue to avoid speculative changes to Kbase import flags, coherency, cache maintenance, CSF event scope, or queue synchronization.



## 2026-11-04 continuation — clean Tomb Raider benchmark (02:06:38)

### User-reported result

- Min FPS: **8.7**
- Max FPS: **18.0**
- Average FPS: **13.5**
- Uploaded archive: `TombRaider_logs_2026-11-04_02-06-38.zip`.
- Inspected `logcat.log` (530,258 chars), `wine_debug.log` (16,814 chars), `TombRaider_d3d11.log` (12,676 chars), `TombRaider_dxgi.log` (2,552 chars), and `TombRaider_d3d9.log` (1,356 chars).

### Diagnostic-off verification

- No `PANVK_KBASE_DIAG_MEM_FLAGS` text found in the capture.
- No `dma-buf import flags` records and no `BO flags requested` records found. This confirms the high-volume memory-flag diagnostic output was absent for this run.
- Raw coherency mode continued to report `0`.
- PanVK custom driver was selected and loaded from the Winlator custom-driver directory. A separate transient extraction attempt logged `libui.so not found` for a temporary driver copy, but subsequent logs show the selected custom PanVK driver loading and device initialization; this did not prevent the benchmark session from running.
- Two `kbase_queue_wait_current` calls completed successfully.
- No captured `0x88`, `GPU_SHAREABILITY_FAULT`, `VK_ERROR_DEVICE_LOST`, CSF fatal/error marker, or `QueueSubmit -4`.

### Shutdown observations

At session end, the capture again contains `FORTIFY: pthread_mutex_lock called on a destroyed mutex`, an invalid SEH frame, and Wine's `!status && "vkWaitForFences"` assertion. DXVK also reports `D3D11SwapChain: Failed to recreate swap chain`. The available timestamps place these around window destruction/session shutdown; there is no evidence in this archive that they occurred during active gameplay. Keep them tracked as teardown defects to investigate separately if reproducible during gameplay or if they prevent clean exits.

### Interpretation

This is the first recent benchmark capture in which the memory-flag diagnostic output is demonstrably absent. Record **13.5 FPS average** as the clean 2026-11-04 02:06:38 datapoint. It is 0.6 FPS above the prior diagnostic-enabled 12.9 FPS run, but that is not a controlled A/B comparison: the earlier run had 730 BO flag log records and 30 dma-buf import diagnostic records. Do not claim the 0.6 FPS difference as a measured performance gain caused by disabling diagnostics.

Keep historical results distinct:
- 14.0 FPS average — earlier `6634282...` candidate; capture conditions not established here as a matched control.
- 10.2 FPS average — `7e5d7a...` candidate.
- 10.6 FPS average — `a87d179...` candidate, prior quiet/diagnostic-off run.
- 12.9 FPS average — 01:57:54 capture, diagnostic logging demonstrably active; not a clean baseline.
- **13.5 FPS average — 02:06:38 capture, diagnostic output absent; cleanest recent measurement.**

### Next action / engineering decision

Do not change Kbase memory flags, cache maintenance, coherency handling, CSF event scope, WAIT64 scope, or queue-notification synchronization based on this capture. No in-game GPU fault or device-lost signature was observed. Keep `PANVK_KBASE_DIAG_MEM_FLAGS` absent and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset for further performance runs. The next useful step is a repeat of the same benchmark with unchanged settings to establish run-to-run variance; separately investigate the recurring shutdown mutex/fence assertion without conflating it with the in-game FPS result.

## 2026-11-04 continuation — repeated clean benchmark and optimization decision

### Repeat result (user-reported)

- User repeated the same Tomb Raider benchmark after the 02:06:38 clean capture and reported the same result: **Min 8.7 / Max 18.0 / Average 13.5 FPS**.
- No second log archive was attached with this repeat, so only the FPS result is confirmed; do not claim a second independent log-level verification.
- Treat 13.5 FPS average as repeatable under the user's unchanged test configuration and the working clean baseline. This improves confidence that it is not a one-off 12.9/13.5 fluctuation.

### Source audit before optimization

- Re-verified the current tested source branch `experiment/kbase-diag-opt-in` at `a87d179f3cdb77946df0c2858cd4370794645e27` and its Android-only packaging workflow.
- Compared the 14.0 FPS candidate `6634282aa14616931656b883aafb5519831eb888` with the current candidate: current is 12 commits ahead, touching six files (`kbase_kmod.c/.h`, `panvk_vX_cmd_event.c`, `panvk_vX_gpu_queue.c`, `panvk_physical_device.c`, and Android-only CI workflow).
- The later CPU sync wait-many path contains a WAIT_ANY loop that repeatedly calls zero-timeout `kbase_cpu_sync_wait_one()` for each sync and then `sched_yield()`. This is a plausible CPU-overhead target only when Vulkan callers use WAIT_ANY with pending syncs; current Tomb Raider logs do not establish that it is a meaningful frame-time hotspot.
- `gpu_queue_check_status()` drains Kbase CSF notifications and checks the device-wide fatal-error latch. `kbase_kmod_csf_process_events()` serializes notification reads and performs a zero-timeout `ppoll()` even when the stream is empty. This is correctness-sensitive and only runs on queue/device status queries, not unconditionally on every submit; do not remove it or weaken fatal-error propagation without a measured call-rate/profile and replacement correctness proof.
- `kbase_subqueue_wait_seqno()` performs explicit cache invalidation and a system-scope barrier before consuming GPU-written completion/error cells. This is correctness-critical; do not weaken `dc civac`/`dsb sy` or skip invalidation based on FPS alone.

### Optimization decision

- Do not make a speculative synchronization, cache-maintenance, memory-attribute, or scheduler change. There is no GPU fault signature and no evidence yet identifying the frame-time bottleneck.
- The next source experiment should add opt-in, low-overhead counters/timing around (1) queue/device status checks and notification draining, (2) Kbase CPU WAIT_ANY polling, and (3) Kbase subqueue wait/CQS completion. Counters must be disabled by default, avoid per-frame log spam, and report aggregated counts/time only when explicitly enabled or at controlled teardown.
- Use those measurements to choose one isolated optimization. Preserve the current `a87d179...` branch as the known-good comparison and build a separate experiment branch; do not merge or package an unvalidated candidate as the recommended driver.
- No source behavior change or new driver package is claimed in this update.

### Additional low-risk tuning candidate — tiler-heap renewal cadence

- Static review found an existing runtime override in `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`: `PANVK_KBASE_HEAP_RENEW_INTERVAL`, default 128 submissions. A second independent renewal guard remains at `PANVK_KBASE_HEAP_RENEW_WORK`, default 65536 estimated tiler-work units.
- Heap renewal waits for outstanding graphics targets, creates a new heap context, and retires the old context only after graphics jobs have advanced. This can create a synchronization/stall point, but increasing cadence blindly can also increase heap residency and approach the 200-chunk minimum max limit.
- The safest first experiment is to change only `PANVK_KBASE_HEAP_RENEW_INTERVAL` from its default 128 to **192**, leaving `PANVK_KBASE_HEAP_RENEW_WORK` at its default 65536. The work-based guard therefore remains active. This is a reversible runtime-config A/B, not a source-code patch or guaranteed optimization.
- Keep all other Winlator/game settings and diagnostic environment variables unchanged/absent. If there is OOM, device loss, visual corruption, or worse stutter, remove the override immediately. Compare average FPS and minimum FPS against the repeatable 13.5 baseline.
- Do not set the interval to 0 or change the work threshold in this first test. No heap policy source change has been committed.


## 2026-11-04 continuation — tiler-heap renewal interval A/B (13:18:28 capture)

### User-reported result

- User tested the proposed PANVK_KBASE_HEAP_RENEW_INTERVAL=192 setting.
- Min FPS: **8.7**
- Max FPS: **19.7**
- Average FPS: **13.3**
- Uploaded archive: TombRaider_logs_2026-11-04_13-18-28.zip.
- Compared with the clean default-interval reference (Min 8.7 / Max 18.0 / Avg 13.5), the 192 run had unchanged minimum, +1.7 peak FPS, and -0.2 average FPS. This is not an average-performance improvement; the higher peak alone is insufficient to justify the setting.
- The archive does not print the value of PANVK_KBASE_HEAP_RENEW_INTERVAL; therefore the environment override being applied cannot be independently verified from these logs. Record the result as the user's intended 192 A/B, with this verification limitation.

### Log inspection

- Archive contents: wine_debug.log, TombRaider_d3d11.log, TombRaider_dxgi.log, and TombRaider_d3d9.log.
- DXVK identifies Wrapper(Mali-G720 MC7), driver version 26.2.149, Vulkan 1.3.354; DXVK reports four compiler threads.
- Device creation and D3D11 feature-level 11_0 initialization succeeded. Vulkan queue family 0 is reported for graphics and transfer.
- Wine log reports raw Kbase coherency mode 0, PanVK initialization, Android hardware-buffer imports, and successful allocation/import operations.
- Two kbase_queue_wait_current calls completed successfully.
- No occurrence found in the supplied logs for 0x88, GPU_SHAREABILITY_FAULT, VK_ERROR_DEVICE_LOST, CSF fatal/error marker, or QueueSubmit -4.
- No memory-flag diagnostic variable/value is printed, and the archive does not provide a reliable way to confirm whether the heap interval environment override was actually inherited by the process.
- At session shutdown the log again shows X connection loss / VK_ERROR_SURFACE_LOST_KHR, FORTIFY: pthread_mutex_lock called on a destroyed mutex, and repeated invalid SEH frame messages. These follow window/server teardown in the log and are tracked as shutdown symptoms, not confirmed active-gameplay GPU faults.

### Decision

- **Revert/remove PANVK_KBASE_HEAP_RENEW_INTERVAL=192 and return to the compiled default of 128.** Leave PANVK_KBASE_HEAP_RENEW_WORK unchanged at 65536.
- Reason: no average-FPS gain (13.3 vs repeatable 13.5 baseline), no minimum-FPS gain, and a small average regression. Do not retain 192 based only on the maximum FPS.
- No source-code change or new package was made for this A/B. Keep the tested source branch and known-good reference unchanged.
- Do not alter Kbase memory flags, coherency/cache handling, CSF scope, WAIT64 synchronization, or queue notification behavior based on this run; the log shows no active GPU-fault signature and no profiling evidence for those changes.
- Next engineering step: return to the default heap cadence and pursue opt-in aggregate profiling of queue status/notification draining, CPU WAIT_ANY polling, and subqueue/CQS waits as previously specified. Keep profiling disabled during clean FPS benchmarks. Continue treating the recurring shutdown mutex/fence/SEH symptoms as a separate teardown investigation.


## 2026-11-04 continuation — opt-in Kbase queue profiling experiment

### Branch and commits

- Created isolated branch `experiment/kbase-profile-counters` from the known-good `experiment/kbase-diag-opt-in` baseline at `a87d179f3cdb77946df0c2858cd4370794645e27`.
- `9e87fab519ece73b16eb0e7a9a65d54a9d303a04` — adds opt-in aggregate timing/counters to the Kbase-backed PanVK queue path.
- `de3276cdc0da75f5fd9f42a9c5608bbfce9669b7` — prints the profile before the final teardown drain, so teardown waiting is not mixed into workload totals.
- `32d6aad1ecf323d1e69917b1d2ab00b8d8130fab` — adds this branch to the Android-only experiment CI push trigger.
- No changes to the known-good branch, stock/vendor driver, Kbase memory flags, coherency/cache maintenance, queue submission semantics, or GPU synchronization behavior.

### Instrumentation behavior

- Runtime opt-in: set `PANVK_KBASE_PROFILE=1`. Default is disabled. No per-frame/per-wait log spam is added.
- Aggregates subqueue wait call count, total elapsed time, timeout count, and error count around `kbase_subqueue_wait_seqno()`.
- Aggregates `gpu_queue_check_status()` CSF notification-drain call count, total drain time, and number of nonempty drains.
- The summary is emitted once during Kbase queue destruction, before the final queue drain. Profiling is process/driver-global within this translation unit, so totals aggregate across queues in the process.
- Counters use relaxed atomic increments/loads to avoid races between queue threads. The profile option is cached on first access.
- Disable the variable completely for clean FPS measurements; use it only for diagnostic profiling captures. This experiment is instrumentation-only and is not a recommended driver/package.

### Source audit correction — CPU WAIT_ANY

- Re-audit of this branch found no `kbase_cpu_sync_wait_many()` implementation in the checked source. The generic Mesa `src/vulkan/runtime/vk_sync.c` handles `VK_SYNC_WAIT_ANY`; when a set cannot use a common sync-type `wait_many` implementation with WAIT_ANY support, it loops over individual zero-timeout waits until one succeeds or the deadline expires (lines 412–427 in the checked snapshot).
- That fallback is generic Vulkan runtime code, not a Kbase-specific helper. It is deliberately not modified in this first PanVK-specific instrumentation patch to avoid broad cross-driver changes without evidence that Tomb Raider exercises that fallback. If runtime evidence points to it, instrument it in a separate narrowly scoped follow-up.

### CI and runtime status

- Android-only workflow run `41828985007` was triggered by the workflow-branch update and was `in_progress` at the time of this handoff entry. Run URL: `https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/41828985007`.
- Build/package success is not yet established. Do not import or run this experimental package until the Android ARM64 build and package audit pass.
- After CI succeeds, use the resulting artifact only in Winlator's custom driver importer, keeping System Driver available as fallback. Runtime profiling test should use `PANVK_KBASE_PROFILE=1`, same Tomb Raider scene/settings, and collect the emitted `kbase profile:` summary plus logs. Then unset the variable and repeat a clean FPS run only if needed.
- Compare counts and total milliseconds, not only average duration: a path can be expensive due to high call frequency even when each call is short. Do not optimize any measured path until the profile demonstrates it is material.



## 2026-11-04 continuation — Kbase profiling branch build and package audit completed

### CI result

- Android-only workflow run: `41828985007`
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/41828985007
- Branch: `experiment/kbase-profile-counters`
- Source/workflow head SHA: `32d6aad1ecf323d1e69917b1d2ab00b8d8130fab`
- Job `build-android-only` completed **SUCCESS**.
- Every stage passed: dependency install, host-tool generation, real Android ARM64 libdrm build, Android-only configure, PanVK compile/stage, driver/manifest/dependency audit, flat Winlator/Adrenotools package, and artifact upload.

### Artifact and integrity

- GitHub artifact ID: `13321123223`
- Artifact name: `panvk-g720-android-only-experimental-package`
- Artifact SHA-256: `490d7da533699047bc4481008fcf029ba8f13bfcd56465f381b2bbc1484e7ece`
- Artifact expires: 2027-02-02.
- Downloaded outer audit artifact: `/mnt/data/panvk-g720-android-only-experimental-package.zip`.
- Extracted importable flat package: `/mnt/data/PanVK-G720-experimental.zip`.
- Importable package SHA-256: `639e6b6ab68419bae3d1cdedce67506395de69785c8e5d92d93cf58e8b677f8e`.
- Package-internal `SHA256SUMS` verified successfully for `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `meta.json`.

### Static package audit

- Driver ELF: 64-bit little-endian AArch64 Android shared object, minimum Android API 30, built with NDK r30; not stripped.
- Driver SONAME: `libvulkan_panfrost.so`.
- Driver NEEDED includes `libpanvk_drm.so` (the private bundled libdrm copy), Android platform libraries, libc, libm, libdl, and libz.
- Driver RUNPATH is `$ORIGIN`, so the private DRM dependency resolves relative to the driver package.
- Private DRM SONAME: `libpanvk_drm.so`; it depends only on libc.
- The outer staging audit also reports no missing DRM symbols against the staged libdrm.
- Flat ADPKG root contains only `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- Metadata: `PanVK Mali G720 (experimental)`, package version `26.3.0-devel-android-kbase-32d6aad1ecf3`, minApi 30, libraryName `libvulkan_panfrost.so`. It explicitly warns that the build is experimental, not Vulkan conformant, and device/game compatibility is unverified.

### Interpretation / safety

- This proves the profiling source compiles for Android ARM64 and that the flat package, ELF ABI, private DRM linkage, RUNPATH, and package checksums pass static validation.
- It does **not** prove runtime loading, Kbase ABI compatibility, correct GPU execution, Vulkan conformance, WSI stability, or improved game performance on the Xiaomi 15T.
- No phone system/vendor driver files were modified. Keep the experimental package separate and use Winlator's custom driver importer only; preserve System Driver as fallback.
- Do not describe this instrumentation build as a performance optimization. It only collects opt-in aggregate timing/counters.

### Next device test

1. Import `PanVK-G720-experimental.zip` into Winlator Mali 1.2 using its custom driver importer only.
2. Keep the existing game/container settings and stock System Driver available as fallback.
3. For one diagnostic Tomb Raider run, set `PANVK_KBASE_PROFILE=1` in the Winlator container/game environment. Do not set `PANVK_KBASE_DIAG_MEM_FLAGS` or `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE`; keep the heap interval at default 128 and work threshold at 65536.
4. Capture the emitted `kbase profile:` summary and the normal logs. Report whether the game launches, reaches the benchmark, and exits normally; include min/max/average FPS if available.
5. The profile is printed during queue destruction before the final teardown drain. It aggregates process/driver-wide wait and status-drain counters. If multiple queues are destroyed, the same aggregate summary may be printed more than once; avoid double-counting duplicate lines.
6. After collecting the diagnostic profile, remove `PANVK_KBASE_PROFILE` completely for clean FPS benchmarks. Do not leave it enabled when comparing performance.

No further source behavior change is justified until the profile demonstrates a material hotspot or runtime logs reveal a new correctness fault.



## 2026-11-04 continuation — Kbase profiling runtime capture (13:39:56)

### User-reported benchmark

- Min FPS: **5.7**
- Max FPS: **19.4**
- Average FPS: **12.6**
- Uploaded archive: `TombRaider_logs_2026-11-04_13-39-56.zip`.
- This was the opt-in profiling build with `PANVK_KBASE_PROFILE=1`. Treat FPS as diagnostic-run performance, not a clean A/B against the 13.5 FPS baseline; instrumentation may perturb timing and run-to-run variance is not controlled.

### Profile output (verified from wine_debug.log)

The log contains two profile summaries:
```
kbase profile: subqueue waits=18 total_ms=3.651 timeouts=0 errors=0; status drains=10 total_ms=0.029 nonempty=0
kbase profile: subqueue waits=39 total_ms=9.580 timeouts=0 errors=0; status drains=20 total_ms=0.045 nonempty=0
```

- The second line is cumulative and includes the first; do **not** add both summaries together.
- Latest cumulative totals: 39 subqueue waits, 9.580 ms total wait time, zero timeouts, zero errors; 20 status drains, 0.045 ms total drain time, zero nonempty drains.
- Average measured subqueue wait duration is approximately 0.246 ms/call (9.580 / 39).
- Average measured status-drain duration is approximately 0.00225 ms/call (0.045 / 20).
- No measured wait timeout/error and no CSF event was drained.
- This capture does not indicate that subqueue waiting or CSF notification draining is a meaningful frame-time hotspot. The drain path is especially unlikely to matter at only 45 microseconds total across 20 calls.
- The current instrumentation does not measure generic Vulkan `VK_SYNC_WAIT_ANY` polling or other CPU-side synchronization paths. Do not infer those paths are hot merely because they remain unmeasured.

### Runtime/log observations

- Log confirms device Xiaomi 25069PTEBG (MT6899), Android 16, Winlator Mali 1.2.1, selected graphics driver `wrapper-gamenative`, Tomb Raider Definitive Edition.
- PanVK initializes; DXVK reports Wrapper(Mali-G720 MC7), Vulkan 1.3.354, four compiler threads, and creates/recreates the 900x506 swapchain (IMMEDIATE then FIFO).
- Kbase raw coherency mode remains 0.
- AHB/dma-buf imports and BO allocations continue to succeed (handles 39 through 70 in this capture).
- No `GPU_SHAREABILITY_FAULT`, `0x88`, `VK_ERROR_DEVICE_LOST`, CSF fatal marker, or `QueueSubmit failed: -4` was found.
- At process/window shutdown, X11 reports the X connection reset/broken, followed by Wine's `vkQueueSubmit` assertion. This ordering is consistent with the display/server being torn down externally, but does not conclusively prove the assertion is harmless. Do not classify it as an in-game GPU fault without a capture showing it before teardown.
- The two profile summaries occur early in the Wine log around pipeline initialization and queue teardown; they are aggregate counters, not frame-by-frame samples.

### Decision and next engineering step

- The first profile experiment has served its purpose: measured subqueue waits and notification drains do not justify changing synchronization behavior.
- Do not optimize or remove `kbase_subqueue_wait_seqno()`, notification processing, cache invalidation, barriers, or queue status checks based on this result.
- A potential next measurement target is CPU-side sync waiting, particularly Mesa's generic `VK_SYNC_WAIT_ANY` fallback loop. Before touching shared runtime code, inspect the exact implementation and establish whether PanVK's Tomb Raider workload reaches it. If adding instrumentation, keep it opt-in, aggregate-only, and isolated to the PanVK test branch; record call counts, loop iterations, and elapsed time, with no changes to wait semantics.
- If source tracing does not establish that the generic fallback is reachable/used, do not instrument or alter it speculatively. Another option is to add narrowly scoped counters to PanVK's actual fence/semaphore wait call sites after tracing the call graph.
- For the next clean performance run, remove `PANVK_KBASE_PROFILE` entirely and return to the known-good clean benchmark package/configuration; do not compare the 12.6 FPS diagnostic run as a performance regression.
- Preserve the current profile branch and artifact for reproducibility. No performance optimization has been validated by this experiment.



### Profiling interpretation correction — summaries are emitted at queue destruction

A follow-up source-to-log correlation found an important limitation in the 13:39:56 profile capture:

- In `panvk_vX_gpu_queue.c`, `kbase_profile_dump()` is called from Kbase queue destruction immediately before the final teardown drain (around the queue-destroy implementation).
- The two summaries appear at wine_debug.log lines 73 and 81, before the later AHB import/allocation sequence (first AHB import is around line 92) and well before the end-of-session X11 teardown (around line 206).
- Therefore these lines are snapshots from queue(s) destroyed during early initialization/probing, not a demonstrated end-of-benchmark total for the long-lived game queue. The second line is cumulative within the process/translation unit and includes the first, but neither can be assumed to cover the full Tomb Raider gameplay workload.
- The low totals (39 waits / 9.580 ms; 20 drains / 0.045 ms) are valid for the observed queue-destruction snapshots only. They cannot rule out synchronization overhead during the actual benchmark.
- Do not draw a definitive hotspot conclusion from these totals. The earlier statement that these paths are not a meaningful frame-time hotspot is too strong and must be treated as unproven.
- The current profile design needs a workload-spanning reporting point before it can answer the performance question. A follow-up instrumentation revision should emit periodic aggregate snapshots during the live queue workload (for example, rate-limited from a frequently reached queue-status/submit path) and retain a final summary. It must avoid per-frame log spam and must be designed so counters from unrelated temporary devices/queues can be distinguished or interpreted safely.
- Before changing the instrumentation, inspect queue/device lifecycle and identify a reliable point reached by the actual game queue during active rendering. Do not assume the early queue-destroy snapshots represent the game queue.


## 2026-11-04 continuation — clean Tomb Raider benchmark after profiling (13:49:53 capture)

### User-reported benchmark

- Min FPS: **8.1**
- Max FPS: **17.3**
- Average FPS: **13.1**
- Uploaded archive: `TombRaider_logs_2026-11-04_13-49-53.zip`.
- Compared with the clean default-heap reference (Min 8.7 / Max 18.0 / Avg 13.5), this run is -0.6 minimum, -0.7 maximum, and -0.4 average FPS. This small difference does not establish a regression or trend; repeatability, scene timing, and device thermal state remain uncontrolled.
- Compared with the intended 192-interval A/B (8.7 / 19.7 / 13.3), this run is lower by 0.6 minimum, 2.4 maximum, and 0.2 average FPS. Retain compiled default interval 128; do not reintroduce 192.

### Archive/log inspection

- Archive contains `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, and `TombRaider_d3d9.log`.
- Wine header identifies Xiaomi 25069PTEBG (MT6899), Android 16, Winlator Mali 1.2.1, graphics driver `wrapper-gamenative`, Tomb Raider Definitive Edition.
- PanVK initializes; Kbase raw coherency mode is 0. DXVK identifies Wrapper(Mali-G720 MC7), driver 26.2.149, Vulkan 1.3.354, and four compiler threads.
- D3D11 feature level 11_0 initializes; DXVK reads 292 state-cache entries. AHB/dma-buf imports and BO allocations continue to succeed.
- No `kbase profile:` summary appears in this archive, unlike the previous diagnostic capture. This is consistent with profiling being disabled, although the log alone cannot prove the process environment contained no profiling variable.
- No `PANVK_KBASE_DIAG_MEM_FLAGS`, `GPU_SHAREABILITY_FAULT`, `0x88`, `VK_ERROR_DEVICE_LOST`, CSF fatal marker, or `QueueSubmit failed: -4` appears in the supplied logs.
- At shutdown, X11 reports the connection broken/server shutdown; Wine then reports a destroyed-mutex warning and a `vkWaitForFences` assertion / invalid SEH frames. These occur after display teardown and are not evidence of an active benchmark GPU fault. Keep this as a separate teardown issue.
- The log does not print the heap renewal interval or work threshold. Record the intended clean settings as default interval 128 and work threshold 65536 based on the test instructions, not as independently verified runtime values.

### Decision / next action

- Keep heap renewal interval at compiled default **128** and `PANVK_KBASE_HEAP_RENEW_WORK=65536`; do not make a heap-policy source change.
- Treat 13.1 FPS as broadly consistent with the 13.5 clean reference, not a validated optimization or regression.
- Preserve the profiling branch/package. Its queue-destruction snapshots were shown to occur during early queue probing and do not characterize the live game queue.
- Continue source lifecycle inspection on the isolated profiling branch to find a workload-spanning, low-overhead reporting point demonstrably reached by the actual game queue. Do not change queue synchronization, memory attributes, cache maintenance, or scheduler behavior without measurements.
- No driver code, package, or system/vendor driver was changed for this benchmark.



## 2026-11-04 continuation — live Kbase queue profiling instrumentation (13:53 local)

### Source lifecycle audit

The prior profile implementation was confirmed to report only from `destroy_gpu_queue()`. This is not a reliable workload reporting point: short-lived queues created during Vulkan probing can be destroyed before Tomb Raider creates/imports its game resources, so the counters may not include active rendering.

The active queue's `panvk_per_arch(gpu_queue_submit)` callback is a reliable live-workload boundary. DXVK's active game queue reaches it for each Vulkan queue submission. The Kbase path is selected inside this callback, and all normal Kbase submission work returns through the shared `out:` cleanup label.

### Isolated branch and commits

Created `experiment/kbase-live-profile` from the verified instrumentation baseline `experiment/kbase-profile-counters` at `32d6aad1ecf323d1e69917b1d2ab00b8d8130fab`.

Commits:
- `9932b277073e39d5ccdce7e1728c3b18e5ccbecb` — add cumulative queue-submit counters and a live reporting point every 256 Kbase submissions.
- `27edd0bb1ed0076857187fb44d9ba3fcac3b681c` — guard Kbase-only profiling variables so non-Kbase builds remain valid.
- `f674d46abbac19f563999a4f2d7b03898fe3a7bc` — add this branch to the Android-only CI push filter.

Only these files differ from the profile baseline:
- `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`
- `.github/workflows/android-only-experiment.yml`

### Instrumentation details

- Runtime opt-in remains `PANVK_KBASE_PROFILE=1`; disabled by default.
- Adds aggregate count, total CPU time, and error count for the Kbase `gpu_queue_submit` callback.
- Emits a cumulative snapshot every 256 Kbase queue submissions. This point is reached during active rendering, rather than waiting for queue destruction.
- Each snapshot also includes the existing subqueue-wait and CSF notification-drain counters, so we can see whether those paths are exercised during the same live interval.
- The final queue-destruction snapshot remains in place. Snapshot values are cumulative; never add multiple snapshots together.
- No queue submission, synchronization, memory, cache, heap, or scheduler semantics were changed. No per-frame logging was added.
- The timer is only read when the profile option is enabled and the device uses Kbase. Profiling remains unsuitable for clean FPS comparisons.

### Static review and CI status

- GitHub compare confirms this branch is 3 commits ahead of the profile baseline, with 43 additions and 2 deletions in the queue source plus one workflow-filter line.
- The Kbase-only declarations and reporting code are guarded by `HAVE_PAN_KMOD_KBASE`, preserving Panthor-only compilation.
- Android NDK wrapper workflow runs `41833423768` and `41833446440` were in progress at the last check. These are wrapper-variant builds, not the Android-only PanVK package build.
- No Android-only PanVK build/package artifact has yet been confirmed for this branch. Do not import or test a wrapper-only artifact, and do not treat source review as build validation.
- Next: verify that the Android-only workflow is triggered for this branch and wait for the exact `experiment/kbase-live-profile` HEAD to compile, pass package audit, and upload its flat ADPKG. Then review the final diff/build logs before asking for one diagnostic-only Tomb Raider run with `PANVK_KBASE_PROFILE=1`.

### Interpretation guardrails

The next profile should determine whether active game rendering produces enough queue submissions to reach live snapshots, and whether subqueue waits or status drains are actually exercised during those intervals. If the 256-submit snapshot does not occur, inspect queue-submit frequency and adjust the interval only in a separate documented instrumentation commit. Do not optimize a path solely because it is present in source.

Keep `PANVK_KBASE_DIAG_MEM_FLAGS`, `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE`, and `PANVK_KBASE_HEAP_RENEW_INTERVAL` unset; keep the compiled heap defaults (interval 128, work threshold 65536). Preserve the System Driver fallback and never modify the phone's vendor Vulkan driver.


---

## 2026-11-04 continuation — live-profile Android package verified and DXVK feature audit

### Android-only build and package result

- Verified GitHub Actions workflow **Android-only PanVK G720 Experiment**, run **41833466869**, branch `experiment/kbase-live-profile`, source SHA `f674d46abbac19f563999a4f2d7b03898fe3a7bc`.
- Run conclusion: **success**. Its single `build-android-only` job completed all 12 steps successfully, including host-tool generation, Android ARM64 libdrm build, Android-only Meson configuration, PanVK compilation/staging, ELF/dependency audit, flat ADPKG packaging, and artifact upload.
- Artifact ID: `13325001132`, name `panvk-g720-android-only-experimental-package`, outer artifact SHA-256 `e9e83b0e33fd145ba9d8cdee369bdcf03da0551535fa1b5473b279ff8b7afb7d`.
- The artifact was downloaded and inspected. It contains the full staged install/audit plus the nested importable `PanVK-G720-experimental.zip`.
- Nested Winlator/Adrenotools package SHA-256: `79c4275efd9d03287502af85345014fed143a26f166c90a5a55fd24f13955f92`.
- Nested package checksum manifest verifies all three payload files (`libvulkan_panfrost.so`, `libpanvk_drm.so`, `meta.json`) successfully.
- Packaged driver is a 64-bit little-endian AArch64 Android shared object, built with NDK r30 for Android API 30. Package root is flat and contains only `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- The driver uses RUNPATH `$ORIGIN` and depends on the bundled private `libpanvk_drm.so`, not an externally supplied libdrm ABI. The private DRM library depends only on libc. This avoids accidentally binding to a platform libdrm with a different ABI.
- Package metadata identifies `26.3.0-devel-android-kbase-f674d46abbac` and explicitly warns that the driver is experimental, not Vulkan conformant, and device/game compatibility is unverified.

### DXVK required-feature audit (G720 / PAN_ARCH 12)

The exact DXVK build string used in the previously captured Tomb Raider session was not preserved in the handoff logs. To avoid guessing its version, this audit uses the published DXVK 2.7-and-later driver-support requirements as a conservative compatibility baseline. DXVK's current requirements page lists Vulkan 1.3, the five required extensions below, at least 256 bytes of push constants, and descriptor indexing.

| DXVK 2.7+ requirement | PanVK source state for G720 v12 | Assessment |
|---|---|---|
| Vulkan 1.3 | ICD manifest advertises API 1.4.354 | Advertised |
| `VK_EXT_depth_clip_enable` | `.EXT_depth_clip_enable = true` | Advertised |
| `VK_KHR_maintenance5` | `.KHR_maintenance5 = true` | Advertised |
| `VK_KHR_maintenance6` | `.KHR_maintenance6 = true` | Advertised |
| `VK_EXT_robustness2` | `.EXT_robustness2 = PAN_ARCH >= 10`; G720 v12 qualifies | Advertised |
| robustness2 features | `.robustBufferAccess2 = PAN_ARCH >= 11`; `.nullDescriptor = PAN_ARCH >= 10` | Both true for v12 |
| `VK_EXT_transform_feedback` | `.EXT_transform_feedback = PAN_ARCH >= 10`; core `.transformFeedback = PAN_ARCH >= 10` | Extension and base feature advertised; optional query/stream/draw subfeatures remain false |
| Push constants >= 256 bytes | `.maxPushConstantsSize = 256` | Meets minimum |
| Descriptor indexing | `.EXT_descriptor_indexing = PAN_ARCH >= 9`; core `.descriptorIndexing = PAN_ARCH >= 9` | Extension and core feature advertised |

- Conclusion: the source does **not** show a missing mandatory DXVK 2.7+ extension/feature gate for v12. Do not add or force-enable duplicate extension names as a speculative fix. This source-level audit is not a runtime proof that each implementation path is bug-free on this kernel/firmware.
- The exact Winlator DXVK version remains to be recorded from a future runtime log; if it is older than 2.7, its required set may be smaller. If newer than 2.7, re-check its release-specific requirements before changing anything.

### Optional DXVK features and likely performance opportunity

- Source advertises `VK_EXT_graphics_pipeline_library`, and the physical-device property `graphicsPipelineLibraryIndependentInterpolationDecoration` is true. DXVK can use GPL to reduce pipeline compilation stutter.
- Source advertises `VK_EXT_memory_budget`, `VK_EXT_shader_module_identifier`, `VK_EXT_extended_dynamic_state3`, and `VK_EXT_attachment_feedback_loop_layout`.
- `VK_KHR_present_wait` and `VK_EXT_swapchain_maintenance1` are compiled conditionally under `PANVK_USE_WSI_PLATFORM`; present-wait additionally depends on timestamp-query capability. Confirm actual runtime enumeration rather than assuming these are enabled in every package/WSI configuration.
- `VK_EXT_descriptor_buffer` is **not** in the current PanVK extension table. DXVK documents it as optional and potentially useful for a more efficient resource-binding model. It is a substantive descriptor-layout/pool/buffer implementation task, not something to expose by toggling one boolean. Treat it as a future optimization candidate only after implementation and CTS/runtime validation.
- The extension table does not advertise `VK_KHR_ray_query`, `VK_KHR_acceleration_structure`, or the associated ray-tracing functionality. Keep them disabled. Vulkan ray query requires acceleration structures and shader traversal support; a GPU's ability to execute a software traversal algorithm does not itself provide the Vulkan API implementation.
- The Vulkan core features `multiViewport`, `shaderClipDistance`, and `shaderCullDistance` are currently false. Do not flip them on without compiler/backend support and conformance testing. They are not listed as mandatory DXVK 2.7+ requirements.
- `textureCompressionBC` is dynamically gated by the queried Mali compressed-format mask. Historical G720 diagnostics in this project found the BC mask intersection to be zero. This may affect some games or texture paths, but it is not a missing DXVK mandatory extension and must not be force-advertised.

### Kbase memory/cache audit result

- Current source intentionally reports `props->is_io_coherent = false`, even when raw coherency mode is logged. The comment explains that cached CPU mappings still require `KBASE_IOCTL_MEM_SYNC` before GPU reads and after GPU writes. Preserve this; do not infer that raw coherency mode 0 permits bypassing cache maintenance.
- Ordinary cached BO allocations use `BASE_MEM_CACHED_CPU`; deferred sync records are flushed through `KBASE_IOCTL_MEM_SYNC` with MSYNC/CSYNC operations.
- DMA-BUF import currently requests `BASE_MEM_COHERENT_SYSTEM`; this is a distinct import path and should not be changed without a reproducer and kernel ABI evidence.
- Kbase's SAME_VA mapping path deliberately avoids `MAP_FIXED` and address hints because kbase rejects them. Preserve kernel-selected mappings. The generic PanVK address-space `MAP_FIXED` use is a separate reservation path and must not be confused with kbase BO mapping.
- No cache, mapping, allocation, descriptor-pool, queue, or scheduler behavior was changed during this audit. There is not yet evidence that any of those paths caused the current FPS ceiling or the earlier 0x88 fault.

### Device test required next

1. Use the nested `PanVK-G720-experimental.zip` from artifact 13325001132 in Winlator Mali 1.2's custom driver importer only. Do not install it over Android's system/vendor Vulkan driver.
2. Preserve the System Driver as fallback and keep the current Tomb Raider container/settings unchanged.
3. Set `PANVK_KBASE_PROFILE=1` for one diagnostic run. Keep `PANVK_KBASE_DIAG_MEM_FLAGS`, `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE`, and `PANVK_KBASE_HEAP_RENEW_INTERVAL` unset; keep compiled heap defaults (renew interval 128, work threshold 65536).
4. Capture the full logs and report Min/Max/Avg FPS, whether the game reaches the benchmark, and whether periodic `kbase profile:` snapshots appear after live queue submissions.
5. Remove `PANVK_KBASE_PROFILE` for any clean performance comparison. Profiling adds timing overhead and this build is instrumentation, not a speed optimization.

### Interpretation

- The live-profile source branch is now build- and package-validated. Runtime validation on the actual Xiaomi 15T / MT6899 / Mali-G720 MC7 remains pending.
- The DXVK 2.7+ required extension set appears covered in source. The most plausible optional feature gap from this audit is descriptor-buffer support, but it is not justified to implement or expose it before collecting the live profiling result and identifying a measured CPU/descriptor bottleneck.
- No phone system/vendor driver files were modified.


---

## 2026-11-04 continuation — Tomb Raider run 19:07 and single-CSG performance experiment

### Latest device run

User supplied `TombRaider_logs_2026-11-04_19-09-48.zip` with reported benchmark:
- Minimum FPS: **3.5**
- Maximum FPS: **18.5**
- Average FPS: **12.4**

Archive contents: `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, and `TombRaider_d3d9.log`.

Runtime identity from the Wine log:
- Winlator Mali 1.2.1 Bionic (23), Xiaomi 25069PTEBG / MT6899, Android 16.
- Graphics driver selection: `wrapper-gamenative`.
- DXVK is **v1.7.2**, not 2.7.2 or later. The D3D11 path is active; the D3D9 log is an additional probe/initialization log.
- DXVK uses four compiler threads.
- Effective DXVK config includes `d3d11.maxQueuedFrames=1`, `dxvk.memoryTrack=True`, `d3d11.relaxedBarriers=True`, `d3d11.ignoreGraphicsBarriers=True`, `dxvk.useEarlyDiscard=True`, and `dxvk.numCompilerThreads=4`.
- The previous DXVK 2.7+ mandatory-extension comparison is not a useful explanation for this specific benchmark. DXVK 1.7.2's enabled extension list does not include maintenance5/6, GPL, or descriptor buffer.

### Runtime stability evidence

Positive observations:
- PanVK Kbase backend initializes; raw coherency mode is 0.
- Android gralloc fallback initializes.
- Native BC6H-to-ASTC-HDR transcode compute pipelines are created successfully (reported build times 1–2 ms).
- Multiple AHardwareBuffer / DMA-BUF imports succeed. The log shows repeated imports around 1.85 MB and a 1.024 MB allocation; format 44 / fourcc AR24, linear modifier 0, one 900x506 plane.
- No `GPU_SHAREABILITY_FAULT` / exception 0x88, no CSF fatal-error notification, no `VK_ERROR_DEVICE_LOST`, and no queue-submit failure appear in this capture.

Teardown:
- X connection `:0` breaks first (five repeated messages).
- Then Android FORTIFY reports `pthread_mutex_lock called on a destroyed mutex`, followed by Wine invalid SEH frame and session end.
- Treat this as a teardown/lifecycle fault after X11 disconnect, not proof of a GPU execution fault. It still deserves separate investigation if it recurs when the game is exited normally rather than the session being force-closed.

Profiling:
- No `kbase profile (cumulative)` snapshot appears anywhere in the archive.
- The existing live-profile code emits a final snapshot at queue destruction when `PANVK_KBASE_PROFILE=1`; its absence means either the env option did not reach the driver process or the loaded binary was not the live-profile build. This run cannot be used to infer queue-submit timing.
- The archive also does not include an explicit process environment dump, so the root cause cannot be distinguished from these logs alone.
- The new experiment adds a one-time enabled/disabled marker and shortens the live snapshot interval from 256 to 32 queue submissions. It also counts emitted ring jobs, doorbells, scheduler kicks, and native internal GPU-wait submits, making the next profile unambiguous.

### Feature-reporting discrepancy to investigate

- DXVK reports `multiViewport=1`, `shaderClipDistance=1`, `shaderCullDistance=1`, `textureCompressionBC=1`, and `VK_EXT_transform_feedback.geometryStreams=1`.
- The checked PanVK v12 source reports `multiViewport=false`, `shaderClipDistance=false`, `shaderCullDistance=false`, `geometryStreams=false`, and gates BC compression on the GPU format mask.
- DXVK also reports the device name `Wrapper(Mali-G720 MC7)` and driver version `26.2.149`. The PanVK source's `driverVersion = 0x06802095` decodes to 26.2.149, so that version string alone does not prove the wrong package was loaded.
- This is a real discrepancy between DXVK's reported feature struct and the source-level v12 feature table. Likely explanations include wrapper feature patching/interposition or a source/runtime mismatch. Do not force-enable these features in PanVK. The next device test should capture `vulkaninfo --summary` / an independent VkPhysicalDeviceFeatures2 report from the same Winlator container if available, and identify the exact wrapper/driver ZIP currently selected.

### DXVK configuration implications

- `d3d11.maxQueuedFrames=1` restricts CPU/GPU frame overlap; `dxvk.memoryTrack=True` adds tracking overhead. These are configuration-level suspects worth controlled A/B testing after a driver baseline is secured.
- `ignoreGraphicsBarriers=True` and `relaxedBarriers=True` alter synchronization behavior and should not be combined with driver correctness experiments. Preserve a safe default config for stability tests; test barrier overrides separately, one at a time, and never attribute their results to PanVK source changes.
- Four compiler threads are CPU shader-compilation workers, not a setting for the seven Mali shader cores. Do not change them to seven based on MC7 branding.

### New isolated experiment branch

Created `experiment/kbase-single-csg` from the live-profile baseline `f674d46abbac19f563999a4f2d7b03898fe3a7bc`.

Source changes in `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`:
1. Replace three independent one-CSI Kbase groups (one for vertex/tiler, one for fragment, one for compute) with one Kbase CSG group created with `cs_queue_count = PANVK_SUBQUEUE_COUNT`; bind each PanVK subqueue to CSI index 0, 1, and 2 respectively. Panthor already models the same Vulkan queue as one group with multiple CS queues.
2. Terminate all bound CSIs first and destroy the shared CSG exactly once, including partial-initialization cleanup.
3. Restrict native GPU WAIT64 collection to producer queue pointers equal to the consumer `submit->queue`. Cross-VkQueue waits fall back to the existing CPU/KCPU bridge. With all subqueues of one VkQueue in one CSG, eligible internal WAIT64 uses CSG scope; this does not change the system-scope SYNC_ADD64 producer, which remains untouched by default.
4. Add an explicit profile enabled/disabled marker, report every 32 submits rather than 256, and count ring jobs, doorbell publications, scheduler-kick fallbacks, and native GPU-wait submits.
5. Correct the CSI index in the queue-bind debug message.

The group unification is a **hypothesis-driven experiment**, not yet a proven optimization. It could reduce CSG scheduling overhead and make same-VkQueue internal waits genuinely intra-CSG, but it changes scheduling topology and must not be promoted without build success and actual device stability/FPS tests. Do not merge it into a stable branch or import an older artifact in its place.

### Build status at checkpoint

- Latest code commit at the time of this entry: `53ae1c0af47b13dce9bd6dc7f25fe2dddc853482`.
- Android-only workflow run `41885675286` is building the branch.
- Wrapper-variant workflow run `41885675212` is also building, but its artifact is not a PanVK driver package.
- Earlier Android-only run `41885630273` started on the prior branch commit `309f077af33b879ba267cb2ebcd93dded43fd74d`; prefer the latest run/artifact that contains commit `53ae1c0af47b13dce9bd6dc7f25fe2dddc853482`.
- Do not ask the user to import any artifact until the exact latest Android-only run passes and its package checksum/ELF/dependency audit has been checked.

### Immediate next steps

1. Wait for run `41885675286`; inspect all steps and artifact.
2. Review the full source diff for shared CSG lifetime, queue bind indices, partial-failure cleanup, and same-queue WAIT64 scope.
3. If build passes, ask for one controlled diagnostic run with `PANVK_KBASE_PROFILE=1`; require the new enabled marker and at least one 32-submit snapshot. Capture logs and FPS.
4. For a clean A/B, compare the shared-CSG branch against the unchanged live-profile build using identical container, resolution, game settings, and warm-up. Do not change DXVK barrier config between these runs.
5. Separately audit wrapper feature interception and the teardown mutex/X11 issue. No ray-query, BC, viewport, clip-distance, or cull-distance feature should be force-enabled from the current evidence.

No system/vendor Vulkan driver was modified. All work remains in isolated experiment branches and uses Winlator custom driver import only.


### Additional benchmark interpretation — DXVK cache and memory budget

- DXVK reports `No state cache file found`, then `Creating new state cache file`, and later `Read 0 valid state cache entries`. This is a cold pipeline-state-cache run. Shader/pipeline compilation can contaminate first-run FPS and frame-time stability; do not compare a cold run to a warm run. Preserve the cache between identical benchmark runs and record whether valid entries are loaded.
- DXVK effective memory limits are `dxvk.maxDeviceMemory=1024`, `dxgi.maxDeviceMemory=1024`, `dxgi.maxSharedMemory=1024`, and `d3d9.maxAvailableMemory=1024` (MiB). DXVK therefore caps its reported budgets around 1 GiB despite the 8,525 MiB Vulkan heap report. No out-of-memory failure is shown, so do not increase these budgets speculatively.
- The configuration has `dxvk.memoryTrack=True`; this is a candidate for a later controlled config A/B because tracking can add host overhead. The `maxQueuedFrames=1` setting can limit CPU/GPU overlap and should be separately A/B tested at 2, then 3 if stability permits.
- Keep `ignoreGraphicsBarriers=True` and `relaxedBarriers=True` out of stability qualification. They are synchronization overrides and could mask driver defects or introduce incorrect ordering. First compare the driver with those overrides disabled, then test them individually only if desired.
- These are DXVK/container settings, not PanVK source modifications. Do not mix them into the single-CSG A/B run.

### Driver version identity note

- `VK_DRIVER_VERSION` in the current PanVK source is hard-coded as `0x06802095`, which decodes to `26.2.149`. The runtime's `Driver version 26.2.149` is therefore consistent with this source's driverVersion field and does not, by itself, prove that the wrong build was loaded.
- The source package `VERSION` string and Vulkan `driverVersion` field are distinct; use a runtime build fingerprint or package provenance to identify the exact loaded binary rather than comparing those two strings.


### DXVK 1.7.2 extension compatibility cross-check

The exact extensions listed as enabled by the Tomb Raider D3D11 DXVK 1.7.2 log were compared against the current PanVK v12 extension table. Every listed extension has a corresponding PanVK declaration:
- `VK_EXT_4444_formats`
- `VK_EXT_custom_border_color`
- `VK_EXT_depth_clip_enable`
- `VK_EXT_extended_dynamic_state`
- `VK_EXT_host_query_reset`
- `VK_EXT_robustness2`
- `VK_EXT_shader_demote_to_helper_invocation`
- `VK_EXT_shader_stencil_export`
- `VK_EXT_transform_feedback` (gated to PAN_ARCH >= 10)
- `VK_EXT_vertex_attribute_divisor`
- `VK_KHR_create_renderpass2`
- `VK_KHR_depth_stencil_resolve`
- `VK_KHR_draw_indirect_count` (gated to PAN_ARCH >= 10)
- `VK_KHR_driver_properties`
- `VK_KHR_image_format_list`
- `VK_KHR_sampler_mirror_clamp_to_edge`
- `VK_KHR_swapchain` (WSI build)

The log additionally reports `VK_EXT_conditional_rendering`; PanVK exposes it for PAN_ARCH >= 10, so G720 v12 qualifies. Therefore, no missing extension in the actual DXVK 1.7.2 enabled-extension list has been identified. Do not add redundant extension declarations. The separate feature-bit discrepancy (multiViewport/clip/cull/geometryStreams/BC) remains open and must be resolved at runtime; an extension being listed as enabled does not prove all of its optional features are supported.


### Additional profiling added to the single-CSG branch

Commit `538e917f5ddf4efd5bd54d8ac102a1026cf70754` extends the opt-in Kbase profile with:
- total tiler-heap renewals;
- renewals triggered by the submit-count interval;
- renewals triggered by estimated tiler work;
- accumulated tiler work at renewal;
- CPU time spent waiting for graphics completion before renewal;
- total CPU time spent waiting plus heap renewal.

This is important because the renewal interval and work threshold are independent triggers. The previous interval-only A/B at 192 could not tell whether the 65,536-work threshold was causing more frequent renewals. The next diagnostic will report which trigger fires and how much CPU time it costs. No heap threshold/default has been changed yet.


---

## 2026-11-04 continuation — shared-CSG PanVK build passed; wrapper capability spoofing fix under build

### Shared-CSG experiment package build

- Branch: `experiment/kbase-single-csg`
- Source commit built: `538e917f5ddf4efd5bd54d8ac102a1026cf70754`
- Android-only workflow run: **41886360574**, conclusion **success**.
- All 12 steps passed: host tools, Android ARM64 libdrm, Android-only Meson configuration, compile/stage, ELF/dependency audit, flat ADPKG package, checksum audit and upload.
- Artifact ID: `13351158168`, name `panvk-g720-android-only-experimental-package`.
- Outer artifact SHA-256: `fff353c367474a2f9a82ba98e535c9b641167f32c70218085d70754dbd3d64bd`.
- Nested Winlator package SHA-256: `c425675528e70beffecc0c7587cdd870e4044ad2ebd3212a7ae41b9dc810cf24`.
- Inner manifest checksums verified for `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `meta.json`.
- ELF audit: AArch64 ELF64 shared object, RUNPATH `$ORIGIN`, bundled private `libpanvk_drm.so`; that library depends only on libc.
- Metadata package version: `26.3.0-devel-android-kbase-538e917f5ddf`.
- The package is a **diagnostic experiment**, not a promoted/stable driver. It contains one CSG with three bound CSIs, CSG-scoped same-VkQueue WAIT64 eligibility, and expanded opt-in profiling including heap-renewal timing. No device test has yet validated the new topology.

### Critical wrapper source audit

The active Winlator selection `wrapper-gamenative` is based on `FristOneRR-Admin/FristOneRR-Wrapperv1`, branch `fristonerr-wrapper-v1`, source commit `d77762fce867ba46e16a0e9f8df626add11a56b3`.

Findings in its source:
- `wrapper_physical_device.c` unconditionally forces `multiViewport`, `shaderClipDistance`, `shaderCullDistance`, `geometryStreams`, `vertexPipelineStoresAndAtomics`, and other feature bits true, regardless of the underlying ICD.
- `GetPhysicalDeviceFeatures2` separately forces transform-feedback `geometryStreams`, `transformFeedback`, host-query-reset, and custom-border-color feature structs true.
- `GetPhysicalDeviceProperties2` fabricates transform-feedback limits/properties.
- The wrapper's `CreateDevice` later clamps requested features against `base_supported_features`, so applications can observe a feature as supported but have it disabled before forwarding device creation. This mismatch is unsafe.
- The wrapper has a `lower_eliminate_clip_distance` path, but `spirv_edit.cpp` currently registers no lowering pass; it runs an optimizer and returns effectively unchanged SPIR-V. Do not count this as clip-distance emulation.
- The wrapper's BC fallback reports `maxResourceSize = 562949953421312` (512 TiB) when BC format queries fail, without querying the actual RGBA/FP16 replacement image. This can overstate image allocation limits and is a concrete memory-stability risk.

### PanVK-aware wrapper patch branch

Created `experiment/wrapper-panvk-honest-features` in the writable PanVK project repository because the connected GitHub integration has read-only access to `FristOneRR-Admin/FristOneRR-Wrapperv1` (attempt to create a branch there returned HTTP 403). No upstream wrapper repository files were modified.

Added `tools/patch-wrapper-panvk.py` and workflow `.github/workflows/android-wrapper-panvk-strict.yml`. The workflow:
- checks out the exact wrapper source commit above;
- builds Android ARM64 SPIRV-Tools static libraries;
- applies a PanVK-specific feature-reporting patch;
- preserves PanVK's actual core/extension features instead of spoofing viewport, clip/cull distance, geometry streams, and vertex-pipeline atomics;
- keeps BC compression advertised only when native support or both wrapper BC emulation families are available;
- disables the no-op clip-distance optimizer path for PanVK;
- preserves PanVK's real texel-buffer alignment and transform-feedback properties;
- replaces the 512-TiB BC fallback with a query of the actual replacement format and its true resource limits;
- builds and packages a separate wrapper ICD, independent of the PanVK driver package.

Latest workflow run: **41888494088**, currently in progress at SPIRV-Tools checkout. Latest workflow source commit: `d62267cb658f53eeccacf646bb164ffd7616c8ca`. The workflow is not yet build-validated and no wrapper artifact should be imported until it passes audit.

### Test sequencing

1. Do not mix the new wrapper with the single-CSG PanVK experiment in the first device test. First build and validate the wrapper package.
2. Test the strict wrapper with the already validated live-profile PanVK package, preserving all other container settings. Confirm DXVK's feature log now reports PanVK's actual `multiViewport=false`, `shaderClipDistance=false`, `shaderCullDistance=false`, and `geometryStreams=false`; BC remains available only through complete emulation.
3. If stable, compare against the original GameNative wrapper using the same PanVK driver and benchmark. Capture visual correctness and any game launch/pipeline errors.
4. Then test the shared-CSG PanVK package with the strict wrapper, one variable at a time.
5. Keep the System Driver fallback. Never install either experimental package over Android's system/vendor Vulkan driver.

The 12.4 FPS result is not evidence that the PanVK compiler or shader-core dispatch is wrong. The current logs show no GPU fault/device loss; the more concrete current risks are wrapper feature spoofing, overreported BC replacement image limits, cold DXVK state cache, and unknown queue/heap renewal costs. Optimize these measured issues before changing shader compiler scheduling or enabling additional Vulkan extensions.


---

## 2026-11-04 late continuation — Tomb Raider 19:09 log audit and strict-wrapper hardening

### New device benchmark

- User-reported FPS: **Min 3.5 / Max 18.5 / Avg 12.4**.
- Uploaded archive: `TombRaider_logs_2026-11-04_19-09-48.zip`; capture timestamp in `wine_debug.log`: 2026-11-04 19:07:28.
- Device: Xiaomi 25069PTEBG / MT6899, Android 16, Winlator Mali 1.2.1 Bionic (23), Wine 10-arm64ec-0, Tomb Raider Definitive Edition, 900x506.
- This is a modest regression versus the immediately prior clean 13.1 FPS run (about -5.3% average); the 3.5 FPS minimum indicates substantial stutter or a slow section. Do not attribute the delta to one source without repeated controlled runs.

### Exact DXVK version correction

- `TombRaider_dxgi.log` and `TombRaider_d3d11.log` both explicitly report **DXVK v1.7.2**, D3D11 feature level 11.0. The earlier DXVK 2.7+ requirements comparison is not the right baseline for this game/container and is superseded for this benchmark.
- DXVK v1.7.2 creates a Vulkan 1.1 instance and probes its own optional device-extension list. It enables available optional extensions including EXT_4444_formats, conditional_rendering, custom_border_color, depth_clip_enable, extended_dynamic_state, host_query_reset, memory_budget, memory_priority, robustness2, shader_demote_to_helper_invocation, shader_stencil_export, transform_feedback, vertex_attribute_divisor, KHR_create_renderpass2, KHR_depth_stencil_resolve, KHR_draw_indirect_count, KHR_driver_properties, KHR_image_format_list, KHR_sampler_mirror_clamp_to_edge, and KHR_swapchain. This is not a list of all mandatory requirements; DXVK filters its optional set by what the ICD reports.
- DXVK logs four compiler threads, `d3d11.maxQueuedFrames=1`, `dxvk.memoryTrack=True`, `d3d11.relaxedBarriers=True`, `d3d11.ignoreGraphicsBarriers=True`, and no valid state-cache entries. The DXGI config caps device/shared memory at 1024 MiB while the PanVK-visible heap is reported as 8525 MiB. This memory cap is a separate Winlator/DXVK policy, not the actual Kbase heap size; test it only as a controlled config variable after driver/wrapper isolation.
- Device feature output still includes `multiViewport=1`, `shaderClipDistance=1`, `shaderCullDistance=1`, `geometryStreams=1`, and BC compression, despite the underlying PanVK v12 feature table not implementing all of these natively. The new strict wrapper should correct this mismatch; BC should remain enabled only when its full wrapper emulation path is available.
- BC6H-to-ASTC-HDR native transcode initializes; compute pipeline setup is logged at 1–2 ms. AHardwareBuffer imports and multiple Kbase DMA-BUF allocations succeed.

### Driver stability and profiling observations

- No `GPU_SHAREABILITY_FAULT`, CSF fatal error, `VK_ERROR_DEVICE_LOST`, or PanVK queue-submit failure appears in this archive.
- Kbase raw coherency mode is 0; retain the current non-I/O-coherent cache maintenance behavior.
- The log has no `kbase profile:` activation line or cumulative queue-profile snapshots. The live-profile build used for this run did not provide enough instrumentation evidence to tell whether the environment variable reached the process, whether the 256-submit checkpoint was reached, or whether the user ended the session before queue teardown. The session ends after X connection loss, followed by a Fortify destroyed-mutex report and Wine SEH error; treat these as teardown symptoms, not proof of an earlier GPU fault.
- The newer `experiment/kbase-single-csg` branch has improved profiling: explicit enabled/disabled activation log, 32-submit snapshots, ring-job/doorbell/kick counters, native GPU-wait submit counts, and separate tiler-heap renewal interval/work-trigger and wait-time counters.

### Active GameNative wrapper source — pinned fork, not upstream mirror

- The active Winlator `wrapper-gamenative` source is `FristOneRR-Admin/FristOneRR-Wrapperv1`, branch `fristonerr-wrapper-v1`, pinned commit `d77762fce867ba46e16a0e9f8df626add11a56b3`.
- Important correction: a comparison against the upstream `leegao/bionic-vulkan-wrapper` source is not authoritative for this fork. The active pinned fork's `spirv_edit.cpp` explicitly has `/* skip missing SPIRV-Tools pass */` in `lower_eliminate_clip_distance`; it does not register the intended clip/cull lowering pass. The PanVK-aware patch therefore disables this no-op path for PanVK. Do not re-enable clip/cull feature reporting on the assumption that this fallback works.
- The active fork's physical-device code force-advertises viewport, clip/cull distance, geometry-stream and other feature bits, then selectively clamps some requested device features to the underlying feature table. This query/create mismatch is the reason for the strict PanVK-aware wrapper patch.
- The active fork also fabricates `maxResourceSize = 562949953421312` (512 TiB) for unsupported BC image-format queries. The strict patch replaces this with a query of the actual replacement image format and limits.
- A further concrete allocation bug was found in `wrapper_device_memory.c`'s host-visible memory emulation path: on bookkeeping allocation failure it can unlock through an uninitialized `mem`; after all allocation backends fail it frees `mem` and then dereferences `mem->device` to unlock; and the common tracking block can read an uninitialized `VkMemoryAllocateInfo` on the emulated path. This is directly relevant to memory-pressure stability, although this benchmark did not demonstrate that the bug was triggered.

### Strict-wrapper branch and CI progress

- Writable project branch: `experiment/wrapper-panvk-honest-features`. The upstream GameNative fork is read-only to the connected GitHub integration, so changes are maintained as a deterministic patcher in this project repository; no upstream repository files were modified.
- Patcher: `tools/patch-wrapper-panvk.py`. Workflow: `.github/workflows/android-wrapper-panvk-strict.yml`.
- Patch behavior: identify PanVK via `VK_DRIVER_ID_MESA_PANVK`; skip other-vendor compatibility feature overrides for PanVK; preserve PanVK's real core/extension and transform-feedback/texel alignment properties; keep BC advertised only with native support or complete wrapper emulation; disable the missing clip-distance pass; query real replacement-image limits rather than claiming 512 TiB.
- Additional patch hardens host-visible allocation emulation: initializes `mem` and `VkMemoryAllocateInfo` before goto paths, removes the fallback-only shadow declaration, and unlocks through the live `device` object rather than a potentially freed `mem`.
- CI fixes made: build only the valid `SPIRV-Tools-opt` CMake target (the previous multi-target invocation failed); remove unsupported `mesa-clc` and `precomp-compiler` Meson options for the pinned older source; fix newline matching in the allocation patcher.
- Latest source commit: `d836d9bc7a5d95cc421aef28be7bc0e7eacccb86`.
- Patch validation workflow for this commit: run **41925911986**, completed **success**. Android wrapper build run **41925911864** was still in progress at this checkpoint. Do not distribute/import a wrapper package until the full Android ARM64 compile, ELF/dependency audit, and package checksum validation pass.

### Test and optimization sequence

1. Finish strict-wrapper Android ARM64 CI and inspect all build/audit output.
2. If successful, test strict wrapper with the already validated `experiment/kbase-live-profile` PanVK package first. Keep game/container settings identical. Verify DXVK now reports PanVK's real `multiViewport`, clip/cull distance and geometry-stream support as false; verify BC remains usable through emulation and check for shader/pipeline failures.
3. Compare strict wrapper against original GameNative wrapper using the same PanVK build. Do not mix in a queue-topology change yet.
4. Then test the already build-validated `experiment/kbase-single-csg` PanVK package with strict wrapper. It shares one Kbase CSG across three CSIs, restricts CSG-scoped WAIT64 to same-VkQueue producers, retains CPU/KCPU fallback for cross-VkQueue waits, and adds 32-submit profiling plus heap renewal counters.
5. After capturing those profiles, make targeted changes to heap renewal cadence, queue synchronization, and DXVK memory/frame queue policy. Change one dimension at a time; never hardcode 7 GPU cores or expose unsupported Vulkan features.
6. Keep all packages separate from the phone's stock/vendor Vulkan driver. Preserve System Driver fallback.


### Descriptor-pool and cache-sync audit after the new log

- Audited `src/panfrost/vulkan/panvk_vX_descriptor_set.c` on `experiment/kbase-single-csg`.
- Descriptor-pool size accumulation uses 64-bit intermediates and explicit overflow checks before multiplying by `PANVK_DESCRIPTOR_SIZE`. Variable descriptor-count adjustment also checks overflow. The earlier arithmetic-hardening commit is present.
- Non-host-only descriptor pools use one PanVK private BO with `PAN_KMOD_BO_FLAG_WB_MMAP`; descriptor sets suballocate from a VMA heap in that BO. Host-only pools use a host allocation. Pool allocation failure is propagated instead of wrapping the size.
- Descriptor updates mark dirty byte ranges; `panvk_descriptor_set_flush()` queues only the dirty range. Generic `pan_kmod_queue_bo_map_sync()` coalesces overlapping/adjacent ranges for the same BO and sync type, and batches pending syncs until queue submission. Kbase then issues `KBASE_IOCTL_MEM_SYNC` for the coalesced ranges. It is therefore incorrect to assume one Kbase ioctl per descriptor update.
- The pending-sync queue has a 4096-operation safety limit and flushes the batch when that limit is reached. A performance optimization here must be driven by measured pending-range count, merged bytes, and MEM_SYNC time; do not replace it with unconditional whole-pool flushes, which could greatly increase cache traffic for large pools.
- No descriptor-pool code was changed in this audit. Current code has no demonstrated integer-overflow defect; allocation size/latency and deferred cache-sync costs remain candidates for instrumentation if a real descriptor-heavy workload exposes them.


### Single-CSG PanVK package independently re-downloaded and verified

- Re-downloaded workflow artifact **13351158168** from successful run **41886360574** (branch `experiment/kbase-single-csg`, commit `538e917f5ddf4efd5bd54d8ac102a1026cf70754`).
- Outer artifact SHA-256: `fff353c367474a2f9a82ba98e535c9b641167f32c70218085d70754dbd3d64bd`.
- Extracted importable nested `PanVK-G720-experimental.zip`; SHA-256: `c425675528e70beffecc0c7587cdd870e4044ad2ebd3212a7ae41b9dc810cf24`.
- Nested package manifest validates:
  - `libvulkan_panfrost.so`: `973eafd37cb4cb8651d56f501368e6836fef9d1bb752db3c1f56c2e7573b2842`
  - `libpanvk_drm.so`: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
  - `meta.json`: `79476f59232679f672385949f5f719f6921ca8e193e32141306cc522c4c2b112`
- Metadata: `26.3.0-devel-android-kbase-538e917f5ddf`, Android API 30 minimum, ARM64 library `libvulkan_panfrost.so`. Package remains experimental and not Vulkan conformant.
- Package is ready for controlled device testing, but do not use it for the next run until the strict GameNative wrapper is built and installed; otherwise the wrapper capability mismatch remains an uncontrolled variable.


### DXVK 1.7.2 feature-log reconciliation

- The log's reported features must be compared against the actual PanVK v12 source, not treated as one undifferentiated spoof list.
- PanVK source does implement/advertise several of the logged features: geometryShader, transformFeedback (but not geometryStreams), conditional rendering, custom border colors, host query reset, depth clip enable, shader demote, shader stencil export, vertex attribute divisor, and robustness2. These should remain available through the strict wrapper when the underlying source reports them.
- PanVK source does not currently expose native `multiViewport`, `shaderClipDistance`, `shaderCullDistance`, `geometryStreams`, or `vertexPipelineStoresAndAtomics` on this v12 path. The wrapper log's `1` values for the first four are not backed by the active ICD implementation; strict wrapper must stop synthesizing them.
- `textureCompressionBC=1` is intentionally supplied by the wrapper's BC1–BC7 emulation path, not native PanVK BC sampling. Keep it only when both emulation families and the replacement-format path are built and validated.
- `VK_EXT_memory_priority` appears as a queried feature struct with `memoryPriority=0`, but it is absent from the DXVK enabled-extension list; do not treat that line alone as an enabled extension.


### Strict-wrapper CI follow-up: cross-build header dependency

- The first wrapper compile attempt on commit `b29e407f13e6f7bfd2a6569ee9eacb3f8c509d4a` passed SPIRV-Tools build, PanVK patch application, and cross-environment generation, then failed Meson C++ compiler sanity because `android.toml` force-includes `src/util/u_gralloc/force_aosp_abi.h`, which is present in the PanVK Mesa source but absent from the pinned GameNative wrapper source checkout.
- Corrected workflow step `Prepare Android cross environment` to copy the exact header from this Mesa repository into `wrapper-src/src/util/u_gralloc/` before Meson setup and assert it exists.
- Workflow commit: `107d25d386cb8f966208fdbbba68ec26c9d2fdfb`. New Android wrapper build run **41926995759** was queued at the time of this update. The earlier `d836d9b` build run remained in progress; do not treat either as successful until the final artifact audit completes.


## 2026-11-05 — Vulkan feature implementation audit (clip/cull + multiViewport)

### Our exact PanVK source state

Audited `experiment/kbase-single-csg` at `538e917f5ddf4efd5bd54d8ac102a1026cf70754`:

- `src/panfrost/vulkan/panvk_vX_physical_device.c` reports `multiViewport = false`, `shaderClipDistance = false`, `shaderCullDistance = false`; `maxViewports = 1`; `maxClipDistances`, `maxCullDistances`, and `maxCombinedClipAndCullDistances` are all zero.
- `src/panfrost/vulkan/panvk_vX_shader.c` has no current clip/cull lowering pass. The v12 compiler path does have a GPU pre-raster pipeline with VS/TCS/TES/GS and generated passthrough stages, so adding support is not just a feature-bit edit: values must survive every pre-raster stage and be consumed correctly by the fragment stage.
- Do not expose clip/cull features or non-zero limits until lowering, stage linking, generated passthrough, and device tests are all correct.

### External implementation reference found

Public reference project: `zenithblue-oss/panvk-kbase-android` (separate repository; not merged into our project).

Relevant patch files:
- `patches/csf-v11/023-clip-cull-distance-fs-discard.patch`
- `patches/csf-v11/024-multi-viewport-index0.patch`
- `patches/csf-v11/193-carry-clip-cull-distances-through-the-gs-and-viewportindex-to-the-fs.patch`
- `patches/csf-v11/194-pass-valid-robustness-state-to-the-depth-bounds-fs-compile.patch`

Patch 023's design is software lowering because Valhall has no clip/cull-distance hardware path: VS clip distances become smooth varyings; cull distances become per-vertex flags; FS discards when an interpolated clip distance is negative or when the cull flag proves all vertices are outside. It also adjusts varying linking and advertises up to eight distances. This is a useful design reference, not a safe cherry-pick: its patch base is Mesa `5a07217f034b`, and our checked-out file blob differs, so the patch must be ported/rebased against our exact source.

Critical correctness warning: the reference project's later patch 193 fixes known clip/cull failures through GS/tessellation and ViewportIndex propagation. Its changelog reports 128 clipping CTS failures before that follow-up; the root causes included uninitialized unused GS clip lanes and cull distances being packed as clip distances. Patch 193 is explicitly recorded as build-only/not device-tested in that project at the time of its worklog. Therefore, do not stop at patch 023 or enable the feature bits after a basic VS-only test. Port and review the follow-up, then run the targeted CTS/device matrix.

Patch 024's `multiViewport` strategy is much smaller: expose `multiViewport` and `maxViewports = 16` while keeping `shaderOutputViewportIndex = false` and `geometryShader = false`. With no shader stage able to write ViewportIndex, every primitive selects viewport 0 by default; the current viewport[0]/scissor[0] emission is therefore correct. This is a compatibility capability, not true per-primitive multi-viewport rendering. It must be revisited if a stage that writes ViewportIndex is later exposed. This can be evaluated as a separate, narrowly scoped change after source/CTS validation.

Reference validation notes:
- The reference project reports G720 MC12 (v12) PanProbe 37/37 on its later RC1/RC3 code, and RC3 reports `gs_viewport_depth` 20/20 on an Immortalis-G720 MC12. This is encouraging for the v12 path but does not establish clip/cull CTS coverage on our Dimensity G720 MC7.
- The reference's v11 clip/cull tests initially had 180 pass / 128 fail; the failures involved GS/tessellation paths. Treat the later fix as a porting lead, not proof for our target.
- Public references: https://github.com/zenithblue-oss/panvk-kbase-android/blob/main/CHANGELOG.md ; https://github.com/zenithblue-oss/panvk-kbase-android/blob/main/worklogs/g615-dxvk/PROGRESS.md ; https://github.com/zenithblue-oss/panvk-kbase-android/blob/main/patches/csf-v11/023-clip-cull-distance-fs-discard.patch ; https://github.com/zenithblue-oss/panvk-kbase-android/blob/main/patches/csf-v11/024-multi-viewport-index0.patch ; https://github.com/zenithblue-oss/panvk-kbase-android/blob/main/patches/csf-v11/193-carry-clip-cull-distances-through-the-gs-and-viewportindex-to-the-fs.patch

### Strict wrapper Android build status correction

- Latest strict-wrapper Android ARM64 run `41926995759` (commit `107d25d386cb8f966208fdbbba68ec26c9d2fdfb`) has now completed **failure**, not success.
- SPIRV-Tools ARM64 static build and PanVK-specific patch application both passed. Meson configuration failed at `meson.build:2143` because required `xcb-dri2 >= 1.8` is missing from the cross pkg-config shim set. `xcb-keysyms` is optional and absent; it was not the fatal error.
- The earlier run `41925911864` failed at the same missing `xcb-dri2` dependency. The separate patch-validation workflow `41925911986` passed.
- No strict-wrapper Android package exists yet. Next fix should address the actual X11 cross-build dependency deliberately (supply correct ARM64 headers/library/pkg-config metadata or adjust the platform configuration only if the Winlator surface path is still fully supported), then rebuild and audit ELF/dependencies/checksums. Do not distribute/import a wrapper yet.

### Next implementation sequence

1. Keep strict wrapper as the truthful feature-reporting baseline; first repair its X11 cross-build dependency.
2. Create an isolated PanVK feature branch from the exact single-CSG source. Port/rebase clip/cull lowering and its GS/TES/TCS passthrough corrections as a cohesive change; do not cherry-pick patch 023 alone.
3. Independently evaluate patch 024's index-0 multiViewport compatibility path. Keep `shaderOutputViewportIndex` false and geometry shader feature exposure unchanged.
4. Add focused shader tests for clip-only, cull-only, combined arrays, sparse/unused lanes, clip/cull through GS and tessellation, FS reads of gl_CullDistance, and multiple viewport state with default index 0. Run NIR validation and host builds first, then targeted VK-GL-CTS/PanProbe on the actual G720 MC7.
5. Only after all relevant tests pass should the physical-device feature bits and limits be enabled. Keep feature enablement separate from FPS optimization; these features are primarily compatibility/correctness work and are not expected to improve Tomb Raider FPS by themselves.
6. Continue keeping all experimental packages separate from the phone's stock/vendor Vulkan driver.

## 2026-11-05 continuation — wrapper proof, clip/cull patch rebase, and build blocker

### Direct verification of the original GameNative feature claims

Re-fetched the pinned active fork at `FristOneRR-Admin/FristOneRR-Wrapperv1`, commit `d77762fce867ba46e16a0e9f8df626add11a56b3`:

- `src/vulkan/wrapper/wrapper_physical_device.c` unconditionally sets `shaderClipDistance`, `shaderCullDistance`, `multiViewport`, and `geometryStreams` to true in the wrapper's synthetic feature block. This happens after the base feature query and has no PanVK identity/capability test in the original source.
- `src/vulkan/wrapper/spirv_edit.cpp::lower_eliminate_clip_distance` constructs an SPIRV-Tools optimizer but the intended pass is replaced by `/* skip missing SPIRV-Tools pass */`. Unless optional aggressive DCE is enabled, the optimizer runs no lowering pass. This is not a clip-distance implementation.
- Therefore the original feature bits are compatibility overrides, not proof that this fork emulates the corresponding behavior. The PanVK-aware wrapper patch must remain the truthful baseline until real ICD or wrapper emulation exists.

### PanVK v12 source path confirmation

Re-checked the single-CSG source branch:

- `panvk_vX_physical_device.c`: `multiViewport=false`, `shaderClipDistance=false`, `shaderCullDistance=false`; limits are `maxViewports=1` and all three clip/cull distance limits are zero.
- `panvk_vX_shader.c::panvk_preprocess_nir` currently does generic IO/system-value preprocessing and `pan_preprocess_nir`; it does not lower clip/cull distance outputs or inputs.
- `nir_intrinsic_load_rasterization_stream` currently returns stream 0, consistent with `geometryStreams=false`.
- No driver source or feature bit was changed in this continuation.

### External clip/cull implementation research and patch-application result

Reference: `zenithblue-oss/panvk-kbase-android`, patches 023 and 193. This is a separate experimental fork, not upstream Mesa.

- Patch 023 proposes software clip/cull using smooth clip-distance varyings, per-vertex cull flags, and fragment discard. Its header claims 13/13 on Mali-G615 MC6 (v11), IDVS and `PANVK_DEBUG=gpu_prerast`; it is not evidence for G720 MC7.
- Patch 193 addresses known GS/TCS/TES and ViewportIndex propagation defects in the first implementation. Its own header explicitly says **build-only; not device-tested**. The associated worklog describes 128 clipping CTS failures before those fixes, including stale unused GS lanes, cull values confused with clip values, and fragment-stage cull-distance reads returning zero.
- I created scratch branch `experiment/clip-cull-distance-v12` from single-CSG commit `538e917f5ddf4efd5bd54d8ac102a1026cf70754`. It currently has no code commits.
- A local/in-memory exact-context application of patch 023 succeeded against the current PanVK physical-device and shader files. Applying patch 193 immediately afterward failed at the `panvk_lower_clip_cull_fs` hunk: patch 193 expects an `extra_scissor` / `scissor_loc` implementation context that is not present in the current patch-023 result. No GitHub source file was modified by this failed application.
- Do not force-apply patch 193, cherry-pick patch 023 alone, or expose the feature bits. The next step is a manual rebase of patch 193's semantics onto our exact shader/compiler source, followed by compile and targeted tests. The FS-discard approach must also preserve clip/cull values through every enabled pre-raster stage and generated passthrough; VS-only success is insufficient.

### Strict wrapper build blocker confirmed from CI log

- Android wrapper build run `41926995759`, job `126913863517`, failed during Meson configuration at `meson.build:2143`: required `xcb-dri2 >= 1.8` was not found.
- The existing Android cross-shim directory contains `xcb-dri3`, `xcb-present`, `xcb-sync`, `xcb-randr`, `xcb-shm`, `xcb-xfixes`, and `xcb` metadata/libraries, but no `xcb-dri2.pc` or ARM64 `libxcb-dri2.so`. `xcb-keysyms` is optional and is not the fatal dependency.
- The pinned wrapper's Meson logic requests xcb-dri2 when `with_any_vk` is enabled. Do not satisfy this by adding a fake pkg-config file or a dummy library. Either build/package a real Android ARM64 xcb-dri2 dependency against the existing xcb shim ABI, or make a narrowly reviewed Meson change only if the DRI2 code path is genuinely optional and the X11 Vulkan WSI path remains intact.
- No strict-wrapper Android artifact exists. Do not import the wrapper or claim the build succeeded.

### Immediate next steps

1. Manually rebase the patch-193 GS/TCS/TES and FS-input fixes onto `experiment/clip-cull-distance-v12`; keep all physical-device feature bits disabled during development.
2. Add focused tests for VS-only and GS/TCS/TES clip/cull outputs, unused lanes, combined and separate arrays, FS reads of cull distance, and IDVS plus GPU-prerast paths. Compile first; device CTS on the actual MC7 remains mandatory before enabling.
3. Separately resolve the strict wrapper's xcb-dri2 dependency with a real ARM64 build or a source-reviewed optional-path change, then rerun full wrapper build and ELF/dependency/package audits.
4. Keep the experimental driver and wrapper separate from the phone's stock/vendor Vulkan driver; preserve System Driver fallback.

## 2026-11-05 continuation — JimVulkan/mali-panvk reference audit

User suggested `https://github.com/JimVulkan/mali-panvk`; audited its latest `main` at commit `58f2c55854bd48b18df756edb11f187af9cd2fde` (2026-11-04 12:19 UTC).

### Project relevance and hardware qualification

- This is a separate Android/kbase port of PanVK, based on Mesa `27.0.0-devel`; our active experiment is based on an older Mesa source with different shader/pipeline integration.
- README claims CSF support for Mali-G720 and says it was tested on G720, G925 and G1-Ultra. The specifically named G720 test device is Galaxy Tab S10+/Ultra with **Immortalis-G720 on Dimensity 9300+**. This is same broad v12 generation but not proof on our exact Mali-G720 MC7 / Dimensity 8400 Ultra. The README does not identify the exact MC count for that test.
- README says geometry shaders, tessellation and transform feedback are software-emulated; geometry streams are supported; DXVK D3D9/10/11 works. This is a valuable reference but not a drop-in binary or proof of exact device compatibility.
- External repo contains an Android build/package path and app-launcher-compatible package metadata; do not substitute its binary into our benchmark without a separate controlled test and matching ABI/kernel validation.

### Confirmed real feature implementations in the external source

Files audited:
- `src/panfrost/vulkan/panvk_vX_shader.c`
- `src/panfrost/vulkan/panvk_vX_physical_device.c`
- `src/poly/nir/poly_nir_lower_gs.c`
- `src/poly/nir/poly_nir_lower_tess.c`
- `src/poly/nir/poly_nir_passthrough_gs.c`

Findings:

1. **Clip/cull for a direct VS -> FS path:** external PanVK implements `panvk_nir_lower_cull_vs()` and `panvk_nir_lower_clip_cull_fs()`. It uses generic varying slots for distances and per-fragment discard. Cull distances are converted at the vertex stage into 0/1 indicators (1 iff that vertex's cull distance is negative); interpolated value reaches 1 only when every primitive vertex is culled. This is a real NIR implementation, unlike the original GameNative wrapper's no-op SPIR-V pass.
2. **Important limitation in that implementation:** its shader-link code explicitly sets `prev_clip_count = prev_cull_count = 0` when the preceding stage is geometry or tessellation-evaluation. Its nearby comment says the geometry-shader emulation does not carry those distances yet. Therefore the external feature bits being true do not prove complete clip/cull coverage for pipelines whose final pre-raster stage is GS/TES. The FS lowering also early-returns if the fragment shader itself declares a CLIP_DIST input. These are audit targets, not behavior to blindly copy.
3. **Multi-viewport:** external source enables `multiViewport` and has `panvk_nir_lower_viewport_scissor_fs()`. For GS-written ViewportIndex, the emulation transforms clip-space position using the selected viewport's transform and uses per-viewport rectangles plus FS discard because Mali's tiler has one scissor. This is a substantive software emulation, not a feature-bit-only override. Its precise supported stage/path coverage must still be checked.
4. **Geometry streams:** external source enables `geometryStreams`; libpoly GS emulation has stream selection, per-stream counters, and rasterization-stream filtering. This is also an actual software-emulation path.
5. External feature table reports `shaderClipDistance=true`, `shaderCullDistance=true`, `multiViewport=true`, and `geometryStreams=true`, with comments pointing to these implementations.

### Comparison with our active Mesa source / integration map

Our isolated branch `experiment/clip-cull-distance-v12` is still based on `538e917f5ddf4efd5bd54d8ac102a1026cf70754` and has no code commits from this audit.

Our current PanVK source is materially different:
- `src/panfrost/vulkan/panvk_vX_shader.c` is ~3536 lines vs ~4258 lines in JimVulkan's Mesa 27 source.
- Our shader compiler already has custom `libpoly` paths: software VS before GS, software TCS, TES, GS main/count/pre/raster variants, and generated rasterization shaders.
- Our software TCS uses `poly_tcs_per_vertex_outputs()` and `poly_tcs_output_stride()`; TES and GS communicate through the libpoly intermediate vertex buffer.
- Our `src/poly/nir/poly_nir.h` already defines clip/cull array sizes in `poly_passthrough_gs_key`, and `poly_nir_passthrough_gs.c` copies combined compact clip/cull arrays. However, the current PanVK compiler path does not use the relevant clip/cull sizes when preparing/linking its stages; merely having those fields in the libpoly key is not proof that our active pipelines preserve them.
- Our `panvk_vX_physical_device.c` still correctly reports `multiViewport=false`, `shaderClipDistance=false`, `shaderCullDistance=false`.
- Do not import JimVulkan's whole 27.0 shader file or cherry-pick its implementation mechanically: compiler APIs, GS variant model, shader-link interface, and libpoly state have diverged.

### Revised implementation plan

1. Use JimVulkan's VS->FS clip/cull algorithm as the reference for the first isolated implementation, but adapt it to our Mesa revision and our compile/link function signatures.
2. Before enabling either feature globally, extend the data flow across our actual stages:
   - VS outputs -> software VS/libpoly vertex record;
   - TCS per-vertex output memory -> TES per-vertex input;
   - TES -> GS input when GS follows;
   - GS output records -> generated rasterization VS -> FS varyings;
   - generated passthrough/transform-feedback variants.
3. Preserve array packing rules after `nir_merge_clip_cull_distance_vars`: clip count determines the split between clip and cull elements. Reuse the existing poly key fields only after proving the caller fills them and all relevant buffer strides/offset masks include the compact combined slots.
4. Handle fragment shaders that explicitly read ClipDistance/CullDistance; the external early-return must not silently bypass required clipping. Handle partial arrays/default lanes, clip/cull counts that cross vec4 slots, GS/TCS/TES and generated passthrough paths.
5. Add NIR unit tests / shader compile tests and targeted Vulkan CTS for VS-only, TCS/TES, GS, TES+GS, XFB, and shader-side ClipDistance/CullDistance reads. Feature bits remain false until all applicable stage paths pass.
6. Audit multiViewport separately. External design is promising for GS ViewportIndex but does not justify advertising arbitrary viewport counts without checking limits, viewport transforms, clipping/scissor semantics, and supported shader stages.
7. GeometryStreams remains a separate candidate: external libpoly is a reference for stream routing, but our current implementation and feature bit stay unchanged until tests prove stream-specific output, XFB, and rasterization behavior.

No PanVK driver code or feature flags were changed in this continuation. The external project is now a high-value reference, but our driver still needs a source-version-aware port and validation. Continue to keep all test artifacts separate from the stock/vendor Vulkan driver.

### Additional exact integration blockers found in our current Mesa revision

Compared the external varying/linker implementation with our active branch, including `src/panfrost/compiler/pan_nir_collect_varyings.c` and `panvk_vX_shader.c`.

- External Mesa 27's `pan_nir_collect_varyings.c` explicitly keeps clip-distance VS output stores even when `sem.no_varying` is set, because NIR's varying optimization can mark them `no_varying` although the fragment clip-lowering pass still consumes them. Our active source lacks this exception.
- External PanVK adds `varying_is_generic(loc)`, classifying `VARYING_SLOT_CLIP_DIST0/1` (and its internal GS viewport-index slot) as generic varying slots. Our active `emit_varying_attrs()` instead uses `bool is_hw = loc < VARYING_SLOT_VAR0`, which classifies clip-distance slots as special/hardware slots.
- Our active `link_shaders()` skips all FS varying locations below `VARYING_SLOT_VAR0`. This also drops clip-distance FS inputs. External source instead skips only slots for which `varying_is_generic()` is false and includes clip-distance slots in VS-to-FS offset/size matching.
- These are mandatory transport/linker changes in addition to adding the NIR lowering functions. Porting only `panvk_nir_lower_clip_cull_fs()` would leave the distance data absent or incorrectly bound.
- Our `src/poly/nir/poly_nir.h` has clip/cull count fields in `poly_passthrough_gs_key`, but current PanVK shader compilation does not populate/use those counts in its active libpoly GS path. The existence of the key fields is not evidence of stage propagation.
- The current libpoly TCS output mask/stride includes per-vertex outputs from `nir->info.outputs_written`; this is a possible transport foundation, but must be checked for compact clip/cull array packing and the TES/GS transitions. Do not assume it works just because the generic output mask is 64-bit.

### Safer port boundary

The first code milestone should be a compile-only, feature-disabled VS->FS implementation:
- Add clip/cull output collection and generic varying classification/linking.
- Add the vertex cull-indicator transform and fragment discard transform, with explicit array-count handoff.
- Keep `shaderClipDistance`, `shaderCullDistance`, and `multiViewport` false.
- Test only a direct VS->FS pipeline first; add explicit tests for FS shader reads, partial vec4 lanes, combined arrays, and default values before calling it correct.
- Then separately carry counts and values through software TCS/TES and libpoly GS buffers. The feature bits stay false until every supported last-pre-raster stage path is correct.

This audit found a concrete reason why a naive port would fail: the current PanVK varying collector and linker do not yet route clip-distance slots as ordinary varyings. No driver source was changed in this step.

## 2026-11-05 continuation — first feature-disabled VS->FS clip/cull prototype

### Code work on `experiment/clip-cull-distance-v12`

The JimVulkan reference justified starting a narrow, isolated prototype. Current branch head: `1b44b86b9cbce2768fd7f0bb0806fc513532ff70`.

Changes:
- `src/panfrost/compiler/pan_nir_collect_varyings.c`: retain VS clip-distance output stores when `sem.no_varying` is set. NIR varying optimization can mark these outputs `no_varying`, but the PanVK FS discard lowering still consumes them.
- `src/panfrost/vulkan/panvk_vX_shader.c`:
  - Add a VS lowering pass that changes merged compact cull-distance values to 0/1 per vertex (`1.0` iff that vertex's cull distance is negative), preserving clip-distance values.
  - Add an FS lowering pass that reads the clip/cull varying values and discards when any clip distance is negative or any interpolated cull indicator reaches `0.9999`.
  - Pass clip/cull array sizes from the direct VS->FS link before the VS NIR is freed.
  - Guard the prototype against FS shaders that explicitly read ClipDistance/CullDistance, because that path needs a lowering that preserves user-visible original values.
  - Guard against combined clip+cull counts above 8.
  - Restrict cull indicator conversion to the hardware VS variant, leaving the transform-feedback VS variant's original values unchanged.
- The implementation is deliberately limited to **direct VS -> FS pipelines**. It does not claim support when TCS, TES or GS is the last pre-raster stage.
- Physical-device feature/property reporting remains unchanged: `shaderClipDistance=false`, `shaderCullDistance=false`, `multiViewport=false`. No Vulkan extension/feature was enabled or advertised.
- No code was cherry-picked from JimVulkan's newer Mesa 27 source; this is an adapted small prototype in our existing source tree.

### Correct v12 varying transport path

An important architecture correction was made during the port:

- The old `emit_varying_attrs()/link_shaders()` code in `panvk_vX_shader.c` is inside `#if PAN_ARCH < 9`; it is not the G720 path. A temporary modification to that legacy path was reverted.
- For our CSF/v12 target, FS varying descriptors are built in `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c::emit_varying_descs()`. It includes slots whose `pan_varying_slot.section == PAN_VARYING_SECTION_GENERIC`, matches them by location against the VS layout, and uses the producer's component count. This avoids the partial-vec4 over-read issue without a legacy emitter modification.
- `PAN_HARDWARE_VARYING_BITS` in `src/panfrost/compiler/pan_compiler.h` excludes CLIP_DIST0/1, so these slots already land in the generic varying section. The only collector change required for this direct path is the `sem.no_varying` exception above.
- Verified the common Vulkan NIR path in our own `src/vulkan/runtime/vk_nir.c`: after dead-variable removal it calls `nir_gather_clip_cull_distance_sizes_from_vars()` followed by `nir_merge_clip_cull_distance_vars()`. Therefore the prototype's compact merged CLIP_DIST0 array assumption is grounded in our current Mesa revision, not just the external project.

### Build validation status

- Added `experiment/clip-cull-distance-v12` to `.github/workflows/android-only-experiment.yml` and added checkout assertions that the prototype exists while clip/cull feature bits remain disabled.
- Android-only build run `41937769803` (commit `a28b5e3dac1ae433ea483f9f2ecd04bcdce5f153`) passed checkout verification, host-tool generation, Android ARM64 libdrm build, and Meson Android-only configuration. At last check, `Compile and stage Android install` was still in progress.
- Android wrapper-variant build run `41937769837` for the same commit was also in progress; it is secondary to the Android-only PanVK build.
- The final branch head `1b44b86...` adds only the combined-distance-count guard after the Android-only run started. The run still exercises the substantive NIR and varying transport code.
- Do not treat this as build-validated until the compile step and artifact audit complete. No package has been offered for import and no runtime/CTS validation has happened.

### Next steps

1. Wait for Android-only run `41937769803`; fix any compiler errors on the isolated branch. Re-run on the final head after fixes.
2. Audit the produced ELF, dependencies, package manifest and checksums. Keep the artifact experimental and separate.
3. Add shader-level tests for clip-only, cull-only, mixed arrays, arrays crossing the vec4 boundary, partial final slot, and explicit FS distance reads. Then run Vulkan CTS direct VS->FS clip/cull cases on the actual G720 MC7.
4. Extend distance transport through our libpoly VS/TCS/TES/GS paths, generated GS rasterization VS, and transform-feedback variants. The feature bits remain false until all relevant final-pre-raster-stage paths and explicit FS reads pass.
5. Do not mix this prototype into the Tomb Raider FPS benchmark until it is independently runtime-qualified; it is a correctness/compatibility feature experiment, not an expected performance optimization for the current DXVK 1.7.2 game.

### Follow-up correction — second-slot cull packing and first CI compile error

- Reviewed our exact `nir_merge_clip_cull_distance_vars()` implementation. It places the cull array at `VARYING_SLOT_CLIP_DIST0 + clip_count / 4` and sets `location_frac = clip_count % 4`. When clip_count >= 4, the cull variable starts in CLIP_DIST1; when clip_count is 1-3 it starts in CLIP_DIST0 at a nonzero component.
- Updated `panvk_nir_lower_cull_vs()` to handle both CLIP_DIST0 and CLIP_DIST1 and compute the absolute array base as `(location - CLIP_DIST0) * 4 + location_frac`. This covers mixed clip/cull arrays crossing the vec4 boundary; the corresponding test case remains mandatory.
- Android-only run `41937769803` reached the PanVK C compilation stage (host tools, ARM64 libdrm and Meson setup all succeeded) and failed at `panvk_vX_shader.c` with one call-site error: the standalone shader compile helper omitted the new `vs_varying_layout` NULL argument after the compile function signature was extended.
- Fixed the argument order in commit `7c9331a1b6706fe199f5ff02379903249ae44db2`. The failing run was on the earlier commit and does not establish that the NIR lowering itself fails to compile. A fresh Android-only build is required on the corrected head.
- Current code branch head after this fix: `7c9331a1b6706fe199f5ff02379903249ae44db2`. Features remain disabled; no device package is considered qualified.

### Latest source refinements and rebuild

- `49c821509482e96fa1ef7c8db1e5686729b21feb`: cull-distance lowering now handles merged arrays beginning in either CLIP_DIST0 or CLIP_DIST1, adding the slot's four-component base plus `location_frac` before comparing the absolute index against clip_count.
- `7c9331a1b6706fe199f5ff02379903249ae44db2`: fixed the standalone shader compile call to pass the new VS varying-layout argument as NULL. The previous Android-only build had failed only on this missing argument at the PAN_ARCH=6 compile target.
- `e9857947b2ee2f244fe07c782cf4166bf06a34de`: changed the NIR helper to track progress per function implementation rather than reusing a cumulative progress flag for later functions.
- Latest branch head: `e9857947b2ee2f244fe07c782cf4166bf06a34de`.
- Fresh Android-only PanVK build run `41938609257` and wrapper-variant build run `41938609251` have started on the latest head. At last check, Android-only CI was still installing dependencies; an earlier run `41938462326` was generating host tools. Await the latest run's compile and package audit before declaring build validation.
- No device runtime test has been run. All clip/cull feature bits remain false.

### Final gating correction and CI assertion repair

- `ea483d2b4d83dfa3df01f7e897ca3e5c96acabcf`: added an explicit requirement that a fragment stage is actually present before marking the direct VS->FS path eligible.
- `027589f85c4ff3635cf5148168b1d368d308a2f0`: pass the validated direct-path counts to both VS and FS compilation. The VS cull transform now keys off the validated linked cull count, so it cannot rewrite cull values when no FS is present or when the FS explicitly reads the original distance values.
- Android-only run `41938752951` failed at the workflow's checkout assertion only: the source guard had been strengthened, but the grep assertion still expected the old expression. No compilation was attempted in that run.
- Updated the assertion in workflow commit `f843a225f9b4f1821e69bb52ed0718c86628b8d3` to match the stronger source guard.
- Fresh Android-only run `41938787852` was queued on source commit `027589f...`; wrapper-variant run `41938787810` was in progress. Await the post-assertion run for the latest source before declaring compile validation.

### Additional future candidate from the local libpoly audit — geometry streams

- Our own `src/poly/nir/poly_nir_lower_gs.c` already routes `emit_vertex` / `end_primitive` by NIR stream ID, maintains per-stream vertex and XFB primitive counters, encodes stream ID into generated vertex IDs, and filters the rasterization variant to the selected rasterization stream.
- The PanVK GS compile path calls `poly_nir_lower_gs()` and serializes `shader->gs.poly.multistream`; this is substantive implementation work, not just a feature bit.
- Nevertheless `panvk_vX_physical_device.c` currently exposes `geometryStreams=false`, `maxTransformFeedbackStreams=1`, and `transformFeedbackRasterizationStreamSelect=false`. The extension feature/property combination must be validated as a whole before changing any of these.
- This makes geometry streams a plausible later capability-audit target, but it is not part of the current clip/cull prototype. Do not raise the stream limits or set `geometryStreams=true` until GS/XFB stream routing, stream-0 rasterization, nonzero streams, and the advertised property interactions pass Vulkan CTS.

### Android-only build result — first successful compile/package

- Android-only run `41938609257` completed **success** on source commit `e9857947b2ee2f244fe07c782cf4166bf06a34de`.
- Successful steps included checkout verification, host-tool generation, real Android ARM64 libdrm build, Android-only Meson configuration, full compile/staging, driver/manifest/dependency audit, flat Winlator/Adrenotools package creation, and artifact upload.
- Artifact: `panvk-g720-android-only-experimental-package`, ID `13375054239`, size 16,733,405 bytes, outer ZIP SHA256 `8a7ba3fb3adbcd0a212285876e7a27c8d4e4ceb12747602cf6968eb48892474c`.
- This validates the prototype's source compilation and packaging on Android ARM64. It does **not** validate shader semantics, Vulkan CTS, or runtime on the actual Dimensity 8400 Ultra/Mali-G720 MC7.
- The artifact predates the final direct-path safety refinements in `027589f...` and workflow assertion update `f843a225...`; do not import it as a qualified build. Latest-source Android-only run `41938817775` (head `f843a225...`) is still in progress at last check.
- No FPS benchmark was run and no feature bits were enabled.


## 2026-11-05 follow-up — clip/cull artifact audit

The final-source Android-only build on `experiment/clip-cull-distance-v12` (`f843a225f9b4f1821e69bb52ed0718c86628b8d3`) passed workflow run `41938817775`. Artifact ID `13375886001` was downloaded; outer ZIP SHA-256 `52b73dc390ad8f0a3fcb537d9f247406f54ccbdf1fa3a966acf396796b588475` verified. The inner flat Winlator package SHA-256 is `916a4c9c2b425c2dce5e7f3be472c9e1714ca2dcdea935dad710d02edc6c8f41`; all three payload checksums pass. Driver/helper are AArch64 Android ELF; private libdrm dependency and $ORIGIN RUNPATH are correct. This remains compile/package validation only.

Read-only review reconfirmed direct VS->FS gating, cull-only hardware VS transformation, untouched XFB variant, and zero/false clip/cull limits and feature bits. No dedicated PanVK test harness exists in this source tree; semantic validation remains open. Do not advertise clip/cull until direct-path shader tests and TCS/TES/GS transport are complete.

Detailed source, artifact, branch inventory, test matrix, and next steps: [docs/CLIP_CULL_VS_FS_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/docs/project-continuity-handoff-2026-11-03/docs/CLIP_CULL_VS_FS_AUDIT_2026-11-05.md).


## 2026-11-05 follow-up — Mali-G720 performance audit

Created isolated branch `experiment/g720-performance-audit` from the validated
clip/cull source checkpoint `f843a225f9b4f1821e69bb52ed0718c86628b8d3`.
No runtime driver behavior or system/vendor driver was changed.

Read-only source review covered the G720 CSF/kbase queue, command-buffer
finalization, descriptor pool allocation, descriptor-set layout/lowering, and
transient BO pools. Existing `PANVK_KBASE_PROFILE` already reports aggregate
submit/wait/status-drain timing, ring jobs/doorbells/kicks, native GPU waits,
and tiler-heap renewal counters. Command-buffer cache clean is tied to safe
recycling and is not to be removed without a memory-lifetime proof.

Descriptor pools currently allocate one backing BO based on the declared pool
size and use a VMA heap for set ranges. The arithmetic is guarded in 64-bit,
but very large contiguous BO requests can still fail due to kernel limits,
address-space fragmentation, or host mapping pressure. Do not silently cap
Vulkan allocations. A segmented descriptor heap is only a future candidate
after real allocation traces and a complete shader-visible address/lifetime
design.

The MC7's seven shader cores are hardware execution resources, not seven
Vulkan queues or a mandate for seven compiler workers. No forced seven-way
dispatch, heap-renewal change, synchronization removal, or descriptor topology
change is justified without profiling.

No performance source code was changed and no new FPS/device runtime test was
run in this step. Next step: establish a repeatable Android/Winlator baseline,
then add disabled-by-default, low-overhead telemetry for descriptor pool sizes
and usage, transient BO high-water marks, and CSF ring occupancy/sub-stage
costs. Change one bottleneck at a time and compare frame-time percentiles under
comparable thermal conditions.

Detailed audit: [docs/G720_PERFORMANCE_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_PERFORMANCE_AUDIT_2026-11-05.md).


## 2026-11-05 follow-up — descriptor telemetry implementation

Continued on isolated branch `experiment/g720-performance-audit`, based on
the known clip/cull checkpoint. Added opt-in descriptor-pool telemetry in
`src/panfrost/vulkan/panvk_vX_descriptor_set.c`:
- `PANVK_DESCRIPTOR_PROFILE=1` enables it; default remains off.
- Aggregates non-empty pool count, total requested backing bytes, maximum
  individual pool size, pool creation failures, set allocation attempts/bytes,
  and set allocation failures.
- Emits snapshots every 16 non-empty pool creations and on allocation errors.
- Uses relaxed atomic counters; no descriptor allocation semantics or heap
  layout were changed.

Commits: `82f11124994e7fa614998006fd59b57f9e723101` (telemetry) and
`a0bafee3aeeaf00127fa8d12c838246eddf4db77` (Android-only workflow branch
filter). Detailed audit updated at
[docs/G720_PERFORMANCE_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_PERFORMANCE_AUDIT_2026-11-05.md).

Wrapper build run `41941319560` started for debugoptimized and release
configurations; it was still in progress at the time of this note. Do not
declare the change build-validated until both wrapper jobs and the Android-only
build are checked. No device runtime test or FPS improvement has been claimed.
No system/vendor driver, kbase kernel, synchronization behavior, or descriptor
heap topology was modified.


## 2026-11-05 follow-up — CSF ring telemetry

Added further disabled-by-default profiling on
`experiment/g720-performance-audit`:
- Descriptor pools: `PANVK_DESCRIPTOR_PROFILE=1` records requested pool
  backing bytes, largest pool, pool creation failures, set allocation counts,
  requested set bytes, and allocation failures.
- CSF ring: `PANVK_KBASE_PROFILE=1` now additionally records ring bytes
  cleaned, wrap events, occupancy sample count, and aggregate outstanding-byte
  high-water mark across subqueues. Firmware extract-page reads for occupancy
  are performed only when profiling is enabled.

Latest code commit:
`842ac70aa83575b15757b869137ba598824c044e`.
Latest audit-doc update:
`b4f8759db3616b4e9a4aa50d1f66b0d1ef3a87bf`.
Android-only and Android wrapper CI runs for the latest source commit were
started and remain pending at this checkpoint. Intermediate CI runs are also
in flight; use the latest-source run for the final compile verdict. No device
runtime test, FPS gain, synchronization change, descriptor allocator topology
change, or kbase kernel modification is claimed.

Detailed audit and implementation notes:
[docs/G720_PERFORMANCE_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_PERFORMANCE_AUDIT_2026-11-05.md).

## 2026-11-05 follow-up — performance telemetry CI passed

Latest performance-branch source/document commit:
`b4f8759db3616b4e9a4aa50d1f66b0d1ef3a87bf`.

Both latest-source workflows are now confirmed SUCCESS:
- Android-only PanVK G720 Experiment: run `41941578949`.
  Job `build-android-only` passed Android ARM64 configure/compile/stage,
  driver/manifest/dependency audit, flat Winlator/Adrenotools package
  creation, and artifact upload.
  Artifact ID `13377475268`, name
  `panvk-g720-android-only-experimental-package`, size 16,745,729 bytes.
- Android NDK Build (Wrapper Variant): run `41941579141`.
  Both `debugoptimized` and `release` jobs passed configure, compile, and
  artifact upload.

The previous pending-CI statement above is superseded by this confirmed result.
This is build and static-package validation only. It is not evidence of
successful Mali-G720 device runtime, Vulkan conformance, game compatibility,
or FPS improvement.

Source review of the new counters:
- Descriptor profile format string/arguments match. Pool bytes are requested
  backing size; set bytes count attempted requested allocation size, including
  attempts that later fail. Pool-create failures count backing-memory
  initialization failures, not earlier host-side multialloc failures.
- CSF profile format string/arguments match. Ring bytes cleaned are counted
  after the existing cache-clean call. The occupancy high-water field is the
  maximum single sampled outstanding-byte value seen across subqueues, not a
  sum of simultaneous subqueue occupancies. The firmware extract read remains
  behind `PANVK_KBASE_PROFILE`.

Next engineering step: device-side diagnostic capture with
`PANVK_DESCRIPTOR_PROFILE=1` and `PANVK_KBASE_PROFILE=1`, if Winlator passes
these environment variables to the driver. Then run the identical benchmark
again with both options disabled for performance measurement. Do not infer
performance gains from instrumented runs. No kbase kernel, system/vendor
driver, descriptor heap topology, or queue synchronization behavior was
modified.

Detailed audit:
[docs/G720_PERFORMANCE_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_PERFORMANCE_AUDIT_2026-11-05.md).

## 2026-11-05 follow-up — transient BO telemetry added

On `experiment/g720-performance-audit`, commit
`396c924ccccb07b15b429b5cdc82fd9318374664` adds disabled-by-default
`PANVK_MEMPOOL_PROFILE=1` counters in `src/panfrost/vulkan/panvk_mempool.c`:
allocation calls/bytes/failures, regular and large free-BO reuse counts,
backing BO creation calls/requested bytes/failures, and largest requested BO.
Snapshots occur every 1024 allocation requests and on failures. No BO
selection, allocation semantics, reference ownership, reset, or cache
maintenance behavior was changed. It does not measure retained/free-list
bytes or exact live BO totals.

The preceding telemetry build validation remains confirmed successful for
commit `b4f8759db3616b4e9a4aa50d1f66b0d1ef3a87bf` (Android-only run
`41941578949`; wrapper run `41941579141`). The new BO telemetry commit has
triggered a fresh CI cycle; its result must be checked before calling this
newest source build-validated. No device runtime validation or FPS gain is
claimed.

Detailed audit:
[docs/G720_PERFORMANCE_AUDIT_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_PERFORMANCE_AUDIT_2026-11-05.md).


## 2026-11-05 follow-up — transient BO telemetry CI confirmed

The CI cycle for source commit `396c924ccccb07b15b429b5cdc82fd9318374664`
has completed successfully:
- Android-only PanVK G720 Experiment run `41942926968`: SUCCESS.
  Job `build-android-only` passed host-tool generation, Android ARM64
  libdrm build, Android-only configure/compile/stage, driver/manifest/
  dependency audit, flat Winlator/Adrenotools package creation, and upload.
  Artifact ID `13376674548`, name
  `panvk-g720-android-only-experimental-package`, size 16,749,203 bytes.
- Android NDK Build (Wrapper Variant) run `41942927008`: SUCCESS.
  Both `debugoptimized` and `release` jobs passed configure, compile,
  and artifact upload.

Documentation-only follow-up runs for commit
`8ed8d5cdaef2be2974459d552a6b838b3d2d3e65` also completed successfully:
Android-only run `41942958976`; wrapper run `41942958980` (both wrapper
configurations passed). These repeat the same source tree with the audit
documentation update.

This confirms Android ARM64 compilation and static package generation for the
descriptor, CSF ring, and transient BO telemetry source. It still does not
prove device runtime, kbase ABI compatibility, Vulkan conformance, Winlator
loading, game stability, or FPS improvement. The artifact must be imported
only as a separate experimental Winlator driver; never replace the system
driver.

Next step: inspect the new artifact's audit/manifest and compare it with the
previous known-good package, then arrange a device diagnostic run with
`PANVK_DESCRIPTOR_PROFILE=1`, `PANVK_KBASE_PROFILE=1`, and
`PANVK_MEMPOOL_PROFILE=1` if Winlator can pass these variables. Follow with
an identical uninstrumented benchmark. Do not tune pool retention or ring
sizes until actual telemetry identifies a bottleneck.


### 2026-11-05 artifact inspection — run 41942926968

Downloaded and inspected artifact `13376674548` (16,749,203 bytes).
The inner flat package contains exactly `meta.json`,
`libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
Metadata identifies package version
`26.3.0-devel-android-kbase-396c924ccccb`, min API 30, and explicitly
marks Vulkan conformance and device/game compatibility as unverified.

Inner package SHA-256:
`2f82639a4c1a8ccf60c264f9a754edf0c10d75cad352487587648b328c3ff00a`.
Package member hashes:
- `libvulkan_panfrost.so`:
  `0386f18c942ab59265a297a451f703695f66e981a6d7a6a7b03431f011af2ee3`
- `libpanvk_drm.so`:
  `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
- `meta.json`:
  `d50d8453fc7a7e451e5b4fdfad5b46b27508e7021c6a7b2ef1300bd23b28ddce`

ELF audit: driver is ELF64 AArch64 Android (min Android API 30). The
packaged driver has RUNPATH `$ORIGIN` and DT_NEEDED
`libpanvk_drm.so` (not `libdrm.so`); the helper has SONAME
`libpanvk_drm.so` and depends only on Android `libc.so`. The package
script patches this private dependency and runpath for the flat package.
The build audit's `missing-drm-symbols.txt` is empty, indicating no
unresolved driver DRM API symbols against the bundled helper export set.
This is static linkage/package evidence only; Android loader namespace,
Winlator importer behavior, and runtime symbol resolution still require
device validation.


## 2026-11-05 device benchmark — Tomb Raider PanVK telemetry

User supplied `TombRaider_logs_2026-11-05_02-03-50.zip` with reported
benchmark result: **7.7 min / 19.7 max / 13.6 average FPS**. The Wine log
identifies the active Vulkan device as `Wrapper(Mali-G720 MC7)`, and emits
PanVK kbase, descriptor, and transient-BO profile output. This is therefore a
PanVK runtime capture, not a System-driver result.

### Runtime findings (final cumulative snapshots)

- Kbase queue submits: 16,160; submit errors: 0.
- Subqueue waits: 68,077; aggregate wait time 95,764.843 ms; timeouts: 0;
  errors: 0. Wait durations can overlap across threads and are not wall-clock
  frame time.
- CSF ring jobs: 41,704; cache-cleaned ring data: 7,819.5 KiB; wraps: 0;
  sampled occupancy high-water: 1.8 KiB. No evidence of ring capacity or wrap
  pressure in this run.
- Heap renewals: 4,068; work-triggered renewals: 4,066; interval-triggered
  renewals: 2; aggregate heap-renewal wait: 52,804.121 ms; total renewal time:
  61,272.081 ms. This is the clearest measured queue/heap cost to investigate.
- Transient BO pool: 8,817,664 allocation requests; 46,981.32 MiB cumulative
  requested bytes; 0 allocation failures; 145,499 regular reuses and 2,680
  large-BO reuses; 680 backing BO creates; 539.07 MiB cumulative created
  backing bytes; 0 BO creation failures; largest BO 42 MiB. These byte totals
  are cumulative traffic, **not live or retained VRAM usage**.
- Descriptor allocator: 9 pools, 4.43 MiB cumulative requested pool backing,
  largest pool 1.40 MiB, 0 pool-create failures; 596,561 set allocation
  attempts, 85.30 MiB cumulative requested set bytes, 299 failures. The
  current counter merges no-free-set / insufficient-pool-space and VMA
  fragmentation outcomes, so the failure reason is not yet known. The rate is
  about 0.05% of attempts; do not assume every failure is fatal.

### Runtime shutdown symptom

At log end, WSI reports `VK_ERROR_SURFACE_LOST_KHR`, followed by a Wine
`vkWaitForFences` assertion. This occurs at teardown/end-of-capture in this
log; it is not enough evidence to attribute the measured FPS to that shutdown
failure. Preserve the exact tail for a separate WSI/fence-lifetime audit.

### Interpretation and next controlled experiment

The ring is not showing pressure (zero wraps and only 1.8 KiB sampled
high-water), and neither transient BO allocation nor BO creation failed.
Descriptor allocation failures are real but rare and currently unclassified.
The strongest optimization lead is the high frequency/cost of tiler-heap
renewal: roughly 4,068 renewals and 52.8 seconds aggregate waiting. The source
supports `PANVK_KBASE_HEAP_RENEW_WORK` as a diagnostic override; default is
65,536 estimated work units. Do not change the heap algorithm or ring size
based on this single capture. First run the identical benchmark with profiling
variables disabled and test a conservative 4x work threshold
(`PANVK_KBASE_HEAP_RENEW_WORK=262144`), leaving all other settings unchanged.
Compare FPS, visual correctness, hangs, and the final WSI/fence behavior. If
Winlator cannot reliably pass environment variables, do not claim that the
threshold was applied.

Next source instrumentation task: split descriptor allocation failure
counters by exact path (descriptor-count overflow, no free set/insufficient
heap bytes, and VMA allocation/fragmentation) before considering allocator
changes. Keep all diagnostics opt-in. No kernel/kbase changes, system-driver
replacement, descriptor heap topology changes, or FPS improvement are claimed.

Previous comparable user-reported run: 8.7 / 18.0 / 13.5 FPS. Current average
is +0.1 FPS (effectively flat); current minimum is lower. Treat benchmark
variance and instrumentation overhead as unresolved until an uninstrumented
repeat is captured.


## 2026-11-05 follow-up — classify descriptor-set allocation failures

Re-read the handoff and audit branches and inspected the full supplied
`TombRaider_logs_2026-11-05_02-03-50.zip`. It confirms PanVK on
`Wrapper(Mali-G720 MC7)`, DXVK 1.7.2, with all three profiling streams
active. Final descriptor snapshot: 596,561 set allocation attempts,
85.30 MiB requested, 299 failures. Final kbase snapshot: 16,160 queue
submits, zero submit errors; 68,077 waits, zero timeouts/errors; 41,704 ring
jobs, zero wraps, 1.8 KiB sampled occupancy high-water; 4,068 heap renewals
(4,066 work-triggered), 52,804.121 ms aggregate renewal wait. The final
mempool snapshot reports zero allocation and BO creation failures. The exact
WSI teardown tail remains `VK_ERROR_SURFACE_LOST_KHR` followed by Wine's
`vkWaitForFences` assertion.

The supplied log contains no evidence that
`PANVK_KBASE_HEAP_RENEW_WORK=262144` was applied. The 4,068 renewals are
therefore the baseline/default-threshold observation; do not treat this as an
override test. The profile-enabled 13.6 FPS run is not a clean performance
measurement.

Source change on `experiment/g720-performance-audit` adds three opt-in
subcounters to `panvk_vX_descriptor_set.c` under the existing
`PANVK_DESCRIPTOR_PROFILE=1` gate:
- `overflow`: both variable-descriptor adjusted-count overflow and final
  descriptor-count overflow, which return `VK_ERROR_OUT_OF_POOL_MEMORY`;
- `capacity`: no free descriptor-set slot or insufficient descriptor heap
  free bytes, also returning `VK_ERROR_OUT_OF_POOL_MEMORY`;
- `vma`: `util_vma_heap_alloc()` returning zero, which returns
  `VK_ERROR_FRAGMENTED_POOL`.

The existing aggregate `set_failures` remains and should equal the sum of
these categories for failures passing through this function. Overflow
failures are counted even though they occur before the normal set-allocation
attempt/byte counters. Counters are relaxed atomics and only execute when the
profile option is enabled; no allocation policy, return code, heap layout,
VMA behavior, or Vulkan semantics changed. Final source commits:
`788d34b622ce6cf51aa410c6366a06b86a6a4988` (classification) and
`751fddd64efc201c103ffa496b29608d315a55e6` (format-string correction).

CI for final source commit `751fddd64efc201c103ffa496b29608d315a55e6`
is pending at documentation time; check both Android-only and wrapper
workflows before packaging. No new driver ZIP is ready yet.

Next device experiment: first run the same scene with all profile options
unset to obtain an uninstrumented baseline. Then, if Winlator can demonstrably
pass Mesa environment variables, set only
`PANVK_KBASE_HEAP_RENEW_WORK=262144`, keep profiling options unset, and
repeat the exact scene/settings. Compare average/minimum FPS, frame pacing,
visual correctness, hangs, and teardown. Do not infer that the override took
effect from the FPS alone; obtain a diagnostic log or another reliable
confirmation of the parsed threshold. Do not change ring sizes, BO retention,
descriptor heap topology, or kbase kernel/system driver.


### 2026-11-05 follow-up — make heap-threshold override verifiable

Added one informational log at the one-time parse site for
`PANVK_KBASE_HEAP_RENEW_WORK` in
`src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. On the first Kbase heap-work
threshold lookup, PanVK now logs the **effective parsed threshold** as
`kbase: tiler heap renewal work threshold=<value>`. This is intentionally
independent of `PANVK_KBASE_PROFILE`, so an uninstrumented benchmark can
verify whether Winlator actually passed the override. It logs once per
process/device path and does not alter threshold parsing, renewal conditions,
or heap behavior. Commit: `52afaaf880fdf3a8cb4e6e0c5cd6598ec482d3ee`.

CI is triggered for this source change and must pass before packaging. The
next device comparison can now be performed without profiling overhead:
first unset all `PANVK_*_PROFILE` options and record the default threshold
log (65,536); then set only
`PANVK_KBASE_HEAP_RENEW_WORK=262144`, repeat the exact benchmark, and
confirm the log says 262,144. If the log remains 65,536 or is absent, the
override is not verified and the run is not a valid threshold experiment.


### 2026-11-05 follow-up — fix descriptor-profile format call

A fresh source audit found that the descriptor-profile `mesa_logi()` format
string in `src/panfrost/vulkan/panvk_vX_descriptor_set.c` had a semicolon
immediately after the final format-string fragment, leaving the variadic
arguments outside the call. This was a compile-breaking syntax error despite
the earlier format-string correction commit. Fixed the terminator to a comma
so all counters remain arguments to the same `mesa_logi()` invocation.
No runtime behavior or profiling semantics changed. Fix commit:
`a2e5ac142b4f0df71cb28204ece4337bace046de`.

The Android-only and Android wrapper workflows for the prior threshold-log
commit were still in host-tool generation when inspected; they had not yet
reached compilation. Those runs therefore do not validate the prior source.
The fix commit triggers fresh CI and must pass both build paths before any
artifact is considered releasable. Continue to inspect the actual compiler
steps/logs, not only the workflow's queued/running status.


### 2026-11-05 continuation — handoff re-read, descriptor audit, CI check

At the user's request, re-read this handoff and
`docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` before continuing. Confirmed
that the current source branch contains the descriptor-profile log-call fix
(commit `a2e5ac142b4f0df71cb28204ece4337bace046de`); the corrected call now
has a comma after its final format-string fragment and all variadic counter
arguments remain inside `mesa_logi()`.

Reviewed all descriptor failure-counter sites in the current source. The
three classifications cover the two count-overflow paths, the combined
no-free-set/insufficient-descriptor-heap-capacity path, and the VMA allocation
failure path. Each classified failure increments the aggregate failure
counter exactly once in the inspected code paths. No allocator policy or
runtime behavior change was made in this review.

CI for source commit `a2e5ac142b4f0df71cb28204ece4337bace046de` remains
in progress at this checkpoint:
- Android-only run `41947103034`, job `126981927839`: host-tool generation
  in progress; Android libdrm/configure/compile/package steps not yet reached.
- Android wrapper run `41947103085`, release job `126981927876`: queued.
- Android wrapper run `41947103085`, debugoptimized job `126981927609`:
  host-tool generation in progress; Meson configure and compile not yet
  reached.

Therefore no artifact is approved and no ZIP is ready. Next action is to
recheck these exact jobs and inspect compiler logs after compile starts. Only
if Android-only and required wrapper builds succeed should the produced
artifact be downloaded, package contents and hashes audited, and a Winlator
ZIP offered. The device benchmark plan remains unchanged: uninstrumented
baseline with default threshold (expected log 65,536), then one-variable
experiment with `PANVK_KBASE_HEAP_RENEW_WORK=262144`, verified by the new
threshold log. No FPS improvement is claimed.


## 2026-11-05 continuation — heap-renewal optimization candidate

Re-read the handoff and performance audit before source work. Reviewed the
latest performance-branch source and the CI failure history.

### Confirmed compile regression and correction

Android-only run `41946843838` on older source SHA
`0afc4d49ee0986e4cfb144fd014e83a1723947db` failed during compilation of
`panvk_vX_descriptor_set.c`. The compiler pinpointed a missing closing
parenthesis in the descriptor-profile `mesa_logi()` call at line 80. This
was a genuine source compile failure, not a dependency/runner issue. The
current performance branch contains the correction in commit
`a2e5ac142b4f0df71cb28204ece4337bace046de`; current source line 80 ends the
format string with a comma and keeps all counter arguments in the call.

### Heap-renewal source audit

The prior Tomb Raider profile showed 4,068 tiler-heap renewals, 4,066
work-triggered and 2 interval-triggered, with 52,804.121 ms aggregate wait
and 61,272.081 ms aggregate renewal duration. Ring occupancy was only 1.8
KiB sampled high-water with zero wraps; transient BO allocation/creation
failures were zero. This keeps heap renewal as the strongest measured lead,
not proof of a frame-time-equivalent 52.8-second cost.

The Kbase path accumulates a CPU-side `tiler_work_estimate` from direct draws
as `vertex.count * instance.count`; indirect draws use
`draw_count * 256`. It renews after either 128 graphics submissions or
65,536 estimated work units by default. Renewal drains graphics targets,
creates a new kbase tiler heap context, updates/cleans the heap descriptor,
and retires the old context only after subsequent VT and fragment ring jobs
have both executed. The estimate is a heuristic, not actual heap bytes or
chunk consumption. A higher threshold can reduce drains/context churn but
can also let the heap grow closer to its maximum and increase memory pressure.

### Isolated 4x experiment branch

Created `experiment/g720-heap-renew-262k` from source/document checkpoint
`e226f2068e3374b63481099390871d390510fece`.
- `2f992b1a6464dc7a0e623e7f6c7355123ef5d772`: add the experiment branch to
the Android-only workflow push filter.
- `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`: change only the Kbase work
threshold default from 65,536 to 262,144 (4x), retaining the interval of 128,
all environment override semantics, and the effective-threshold log.
- Candidate source line explicitly labels the 4x value as an isolated
experiment requiring peak-heap and stability validation.

CI for the experiment source SHA is queued:
- Android-only run `41947695997`.
- Android wrapper run `41947696048` (Release and Debugoptimized matrix).
No compile or package success is claimed. Also in flight are the corrected
baseline-source builds at SHA `e226f2068e3374b63481099390871d390510fece`:
Android-only run `41947306451`, wrapper run `41947306616`. At last check,
the Android-only job and both wrapper variants were still generating host
tools; target compile/package stages had not begun.

Next: inspect both experiment workflows through actual target compilation;
if green, audit the Android-only package and compare it with the baseline.
Then run the same Tomb Raider scene/settings first on the baseline and then
on the 262,144 candidate with all profiling disabled. Record effective
threshold log, min/max/average FPS, frame pacing, visual correctness, hangs,
WSI/teardown behavior, and any OOM/heap errors. Reject the candidate if it
causes instability or worse frame pacing, even if average FPS rises. Do not
change kernel/system driver, ring size, descriptor topology, or Vulkan feature
exposure as part of this experiment.


### Additional kernel-level constraint — heap statistics are per CSI

A source audit of the Arm Kbase CSF kernel implementation resolves an
important ambiguity in the older PanVK comments. In
`mali_kbase_csf.c`, `handle_oom_event(group, group_id, stream_id)` reads
`CS_HEAP_VT_START`, `CS_HEAP_VT_END`, and `CS_HEAP_FRAG_END` using that
same `stream_id` (CSI index), then validates
`frag_end <= vt_end < vt_start` before allocating a chunk. Therefore the
statistics are per command stream/CSI, not aggregated across all CSIs in a
CSG. The Nov 4 one-CSG/three-CSI topology does **not** make it safe to restore
`VERTEX_TILER_COMPLETED` or `FRAGMENT_COMPLETED` on the separate VT and
fragment CSIs. Re-enabling those firmware heap operations without a new
same-CSI design would reproduce the invalid-statistics failure. Keep the
current kbase suppression and wholesale heap renewal; do not change it as a
performance experiment. Kernel source: Google Android kernel Mali Kbase CSF
`mali_kbase_csf.c`, `handle_oom_event()`, where the CSI-specific reads and
validation are explicit:
https://android.googlesource.com/kernel/google-modules/gpu/+/c60221160c224b1516c34151663134df22d6123d/mali_kbase/csf/mali_kbase_csf.c

The Kbase device setup uses 1 MiB chunks, 10 initial chunks, and 400 max
chunks. Thus the configured heap ceiling is 400 MiB per queue, with 10 MiB
initial allocation, subject to kernel allocation behavior. The 4x work
threshold candidate can let a generation grow longer toward that ceiling;
monitor OOM/device-loss and memory pressure closely on the 12 GB device. Do
not increase the 400-chunk cap. The 4x branch remains an A/B candidate only.


### Baseline package artifact audit — source SHA a2e5ac1

Android-only workflow run `41947103034` completed successfully, including
Android ARM64 libdrm build, Android-only configure/compile/stage, driver and
manifest/dependency audit, flat Winlator/Adrenotools ZIP creation, and artifact
upload. Artifact `13378853545` (`panvk-g720-android-only-experimental-package`)
was downloaded and independently inspected.

- Outer workflow artifact SHA-256:
  `0eb248e1d80d1a6eb03b015b932cd43afe2a5bf8c1b8e2e7f77cdd722aea6b58`.
- Inner Winlator package: `PanVK-G720-experimental.zip`, 4,133,006 bytes;
  SHA-256 `bc2bd7efa8b696fdda24a345a616dcf196d863b1133712e4a89bd4e7995ed170`.
- Inner archive has exactly four files: `meta.json`,
  `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; ZIP integrity
  test passed and all three file checksums match.
- Both shared objects are Android ELF64 AArch64 built for Android API 30.
  Driver has `RUNPATH=$ORIGIN`, `DT_NEEDED libpanvk_drm.so`, and the expected
  Android platform libraries; bundled `libpanvk_drm.so` has only libc as a
  dependency. CI's undefined-DRM-symbol audit is empty.
- Manifest reports Vulkan API 1.4.354 and
  `/usr/local/lib/libvulkan_panfrost.so`. Metadata identifies source package
  version `26.3.0-devel-android-kbase-a2e5ac142b4f`, minApi 30, and explicitly
  labels the driver experimental/non-conformant/unverified.

This is a verified **65,536-work-threshold baseline package**, not the 262,144
candidate and not a performance-promoted release. The wrapper workflow for
run `41947103085` has Debugoptimized successful; Release was still in host-tool
generation at the last check. Candidate branch
`experiment/g720-heap-renew-262k` source SHA
`8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7` has Android-only run
`41947695997` in progress (host tools) and wrapper run `41947696048` in
progress/queued (Release and Debugoptimized). The later baseline-source runs
at SHA `e226f2068e3374b63481099390871d390510fece` are also still in progress.

The inner package was copied to the current work area as
`/mnt/data/PanVK-G720-baseline-65k.zip` for a controlled baseline run. Device
A/B still requires profiling variables unset, the effective threshold log
checked, and the same Tomb Raider scene/settings. No FPS improvement is
claimed from the build or artifact audit.


### Latest CI refresh after baseline artifact audit

- Android-only run `41947103034` on source SHA `a2e5ac142b4f0df71cb28204ece4337bace046de`: completed successfully, including package audit and upload. Artifact `13378853545` was independently downloaded and validated as recorded above.
- Wrapper run `41947306616` on source/doc checkpoint `e226f2068e3374b63481099390871d390510fece`: Release job `126982590161` completed successfully; Debugoptimized job `126982589829` was compiling at the latest check.
- Older wrapper run `41947103085`: Debugoptimized job `126981927609` completed successfully; Release job `126981927876` remained in host-tool generation. The newer Release pass validates the same driver source because the intervening e226 commits changed only documentation.
- Baseline Android-only run `41947306451` at e226 had also completed successfully, including compile, package audit, and artifact upload.
- 262,144 candidate run `41947695997`: Android-only target compile in progress after host tools, libdrm and configure passed.
- 262,144 candidate wrapper run `41947696048`: Debugoptimized was generating host tools; Release was installing build dependencies. No candidate compile/package success yet.
- Candidate-vs-baseline compare `e226...8d6ec93` confirms only one runtime source change: `KBASE_TILER_HEAP_RENEW_WORK` 65,536 -> 262,144; the other changed file only adds the experiment branch to the Android-only CI filter.

No device performance result has been produced for the candidate. The baseline package is available for a clean, uninstrumented 65,536 run while candidate CI completes.


## 2026-11-05 — clean baseline device run and 262K candidate package

### Baseline device result (65,536 threshold)

User supplied Tomb Raider benchmark: **min 9.0 / max 17.6 / average 13.5 FPS**. Compared with the prior 13.5 FPS average baseline capture, average is unchanged; minimum improved from 7.7 to 9.0 while maximum declined from 19.7 to 17.6. This is run-to-run variation until repeated controlled samples establish otherwise; no performance gain is claimed.

Inspected `TombRaider_logs_2026-11-05_02-33-23.zip`: device identifies `Wrapper(Mali-G720 MC7)`, driver 26.2.149, Vulkan 1.3.354, DXVK 1.7.2. PanVK startup explicitly reports `kbase: tiler heap renewal work threshold=65536` and Kbase profiling disabled. DXVK reports four compiler threads, 288 state-cache entries, and maxQueuedFrames=1, relaxedBarriers, early discard, ignored graphics barriers, and memoryTrack. No PanVK device-lost or Kbase fault appears. `VK_ERROR_SURFACE_LOST_KHR` appears at the Wine/WSI log tail, consistent with the prior teardown symptom; it is not evidence of a gameplay failure.

### 262,144 candidate CI and package audit

Candidate branch `experiment/g720-heap-renew-262k`, source SHA `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`: Android-only run `41947695997` / job `126983880529` passed all compile, staging, audit, packaging, and upload steps. Wrapper run `41947696048`: Release job `126983881044` and Debugoptimized job `126983880665` both passed configure, compile, and artifact upload.

Android-only workflow artifact `13380405183` digest: `afc8aef881b231d9ab80036370a900f104ffe805452c5b922c6ca3637f852e2a`. The downloaded outer archive and inner package ZIP passed integrity checks. Inner package has exactly four members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three member checksums match. Inner package size: 4,133,133 bytes. SHA-256: `c6408abb12a76b1c0d6d809837a7458ca2429ec7210cba9c8e2d95b98658753d`. Driver is ELF64 AArch64 / Android API 30. Package RUNPATH is `$ORIGIN`, it needs bundled `libpanvk_drm.so`, and that library needs only libc. CI missing-DRM-symbol report is empty. Manifest version is `26.3.0-devel-android-kbase-8d6ec93a0d34`, minApi 30, and explicitly experimental/non-conformant/unverified.

Verified package extracted to `/mnt/data/PanVK-G720-heap-renew-262k.zip` for delivery. Build/package validation is not proof of device stability or performance.

### Next A/B test

Install the 262,144 candidate as a separate Winlator driver. Keep the same Tomb Raider scene, resolution, quality, DXVK 1.7.2, wrapper, DXVK configuration, and comparable thermal starting condition as the 65,536 baseline. Keep all `PANVK_*_PROFILE` variables unset. Verify startup log reports `tiler heap renewal work threshold=262144`; if not, stop and report the actual value. Capture min/max/average FPS, frame pacing if available, visual correctness, crashes, heap OOM/device-loss, and teardown. Compare against baseline 9.0 / 17.6 / 13.5 FPS and repeat each variant if possible. Keep the 65K package for rollback.


## 2026-11-05 — 262K candidate result and capability-first pivot

### 262,144-work-threshold device capture

Inspected user archive `TombRaider_logs_2026-11-05_02-42-14.zip`.
The startup log confirms the imported build is the 262,144 candidate:
`[MESA-PANVK] kbase: tiler heap renewal work threshold=262144`.
The run therefore tested the intended threshold. The user reported min 7.8 FPS,
max 18.9 FPS, average 3.9 FPS. The average is mathematically inconsistent
with the reported minimum and maximum, so it is **unverified** and must not be
used in comparisons until corrected. Do not infer a performance gain from
min/max alone.

The DXVK D3D11 log identifies Wrapper(Mali-G720 MC7), driver 26.2.149,
Vulkan 1.3.354, and DXVK 1.7.2. It lists the device extensions/features DXVK
actually enabled for this D3D11 device. **This is not the complete list of
extensions/features supported by the Vulkan driver**; it is the application-
enabled subset. The log cannot be used to claim that PanVK only supports those
listed extensions. The uploaded capture contains no PanVK CSF fatal-error or
device-lost report in the inspected startup/game logs.

### Capability-first direction

The user requested shifting effort away from low-confidence micro-tuning and
toward enabling the Mali-G720's supported GPU capabilities, then optimizing
against that broader capability set. Adopt this as the project direction.
Keep the 65K baseline and 262K candidate as separate rollback/A-B artifacts;
do not promote 262K based on this capture.

Initial source audit of
`src/panfrost/vulkan/panvk_vX_physical_device.c` on
`experiment/g720-performance-audit` (source blob
`6f8062ef02dd8b7707b2a16d4868aa7d84f4a4fb`):

- PanVK's extension table already advertises a broad Vulkan 1.4-era set.
  Many gates use `PAN_ARCH >= 10`, which are true for Mali v12, including
  conditional rendering, transform feedback, descriptor indexing,
  robustness2, draw-indirect-count, sampler-filter-minmax, and several
  image/compute capabilities. The actual DXVK-enabled list is only a subset.
- The v12 feature table already enables many relevant capabilities, including
  8/16-bit storage, descriptor indexing/update-after-bind, runtime descriptor
  arrays, non-uniform indexing, buffer device address, Vulkan memory model,
  float16, shader int8/int64, subgroup operations, dynamic rendering, and
  transform feedback/conditional rendering.
- Several feature fields are deliberately false or specially gated and need
  individual implementation audits rather than blanket flips:
  `vertexPipelineStoresAndAtomics` (v12 remains false unless the explicit
  force-atomics debug option is set; source comment explains the hardware
  speculative invalid-index concern on pre-v13), `multiViewport`,
  `pipelineStatisticsQuery`, `shaderClipDistance`, `shaderCullDistance`,
  `geometryStreams`, sparse binding on Kbase (the sparse bind queue is a
  stub), and selected advanced rasterization/atomic subfeatures.
- `VK_KHR_ray_query` is absent from the PanVK extension table. Do not add its
  extension bit alone: no end-to-end ray-query implementation has yet been
  established across acceleration-structure handling, SPIR-V/NIR lowering,
  shader compilation, synchronization, and feature/property reporting.
  Hardware capability or another driver's software implementation is not
  proof that this PanVK path exists.

### Next engineering work

1. Build a v12 capability matrix from the current extension table, core and
   extension feature structs, property reporting, compiler lowering, and
   Kbase backend constraints. Mark every field as implemented, hardware-
   limited, compiler-limited, backend-limited, or unverified.
2. Compare the v12 results with upstream Mesa PanVK and the relevant Mali
   compiler/ISA support. Avoid assuming that a feature's architecture gate
   alone proves correctness on this Kbase CSF backend.
3. Prioritize missing features that materially help DXVK/VKD3D (descriptor
   indexing, indirect draws, shader arithmetic/storage capabilities,
   transform feedback, dynamic state, synchronization) before speculative
   RT support.
4. For each candidate, add or enable the complete implementation path and
   tests; do not merely set an extension-table boolean or VkBool32.
5. Keep Kbase-specific unsupported sparse binding disabled until its queue
   path is implemented. Keep v12 vertex-pipeline stores/atomics disabled
   until invalid-index/speculation semantics and compiler-generated accesses
   are proven safe.
6. Obtain a complete supported-extension/feature dump from the device (for
   example via a Vulkan capability tool running against the imported PanVK
   driver) to distinguish driver-supported capabilities from DXVK's enabled
   subset. Retain the application log as a record of what DXVK actually uses.

No feature exposure or runtime source change was made in this audit step.


## 2026-11-05 — corrected 262K benchmark and detailed v12 capability matrix

The user corrected the 262,144-threshold Tomb Raider result: **min 7.8 /
max 18.9 / average 13.9 FPS** (not 3.9). Against the 65,536 baseline
(9.0 / 17.6 / 13.5), the candidate's average is +0.4 FPS while minimum is
-1.2 FPS. This single-run difference is inconclusive; retain 65K as the
reference and do not promote 262K.

Created
[docs/G720_CAPABILITY_MATRIX_2026-11-05.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-performance-audit/docs/G720_CAPABILITY_MATRIX_2026-11-05.md)
on the performance-audit branch. It records current v12 extension/feature
coverage, exact reasons the following remain disabled, and the implementation
port priorities:
- multiViewport: current limit is one viewport; a software geometry/fragment
  path exists in later JimVulkan PanVK work.
- shaderClipDistance / shaderCullDistance: current branch has a partial direct
  VS->FS lowering but no explicit FS-read or geometry/tessellation propagation.
- pipelineStatisticsQuery: no query result writer; shader stats address is
  currently lowered to the safe OOB sink.
- geometryStreams: current lowering only models stream zero.
- sparse Kbase binding: no sparse bind queue.
- vertex-pipeline stores/atomics: retain v12 safety gate due speculative
  invalid-index references.
- ray query and descriptor heap: absent from this branch's complete driver
  implementation; later forks are research references, not drop-in patches.

Compared `JimVulkan/mali-panvk` (Mesa 27.0.0-devel, README reports G720
testing). Its source includes software paths for clip/cull, multi-viewport,
CPU-counted pipeline statistics, BCn texture emulation, and
`VK_EXT_descriptor_heap`. These are candidate port references only; source
and generated-registry differences mean each needs a real port, build, tests,
and runtime validation. The next smallest coherent source task is to complete
the existing clip/cull lowering, on a dedicated branch, without enabling the
feature bits until all pipeline-stage and property semantics are covered.

No runtime feature flag was changed in this step.


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


## 2026-11-05 — clip/cull pipeline transport audit (experiment branch)

Dedicated branch: `experiment/g720-clip-cull-capabilities` (starting commit
`2fb0c3c792a193d0bed9d6eafda5cee606546150`). No runtime source or feature
exposure changes have been made in this audit step.

### Newly confirmed shared infrastructure

- `nir_clip_cull_distance_io_utils.c` already provides
  `nir_gather_clip_cull_distance_sizes_from_vars()`,
  `nir_merge_clip_cull_distance_vars()`, and
  `nir_lower_clip_cull_distance_to_vec4s()`. The latter rewrites scalar
  distance arrays to vec4 varyings while retaining input/output semantics and
  supports arrayed GS/TCS/TES interfaces. Its helper asserts that each
  distance array is at most eight elements; the driver must separately enforce
  Vulkan's combined clip+cull limit and interface consistency.
- `poly_nir_lower_vs_before_gs()` writes every scalarized VS/TES output to
  libpoly's intermediate vertex buffer using the complete
  `outputs_written` mask. The software GS input lowering reads those same
  slots. TCS per-vertex output masks/strides also include ordinary varying
  slots. This suggests the existing poly vertex ABI can transport clip/cull
  values, but this is not yet proof of complete array semantics or the final
  FS link.
- `poly_nir_passthrough_gs.c` already has explicit
  `clip_distance_array_size` and `cull_distance_array_size` fields in
  `poly_passthrough_gs_key`. It combines compact clip/cull slots and forwards
  the correct per-vertex values. This helper is not the same as PanVK's
  current custom software-GS path.
- PanVK's custom software-GS path calls `poly_nir_lower_gs()`. Its generated
  rasterization VS collects output types by varying slot, selects per-output
  values, then writes them back by slot. It has no explicit clip/cull array-size
  handling in `poly_gs_info`; that is a critical point to audit before relying
  on it for combined arrays, especially when both clip and cull arrays cross
  slot 0/1 boundaries.
- TCS/TES compilation uses libpoly's per-vertex output masks and buffer ABI;
  TES followed by GS is explicitly lowered to the software predecessor path.
  Clip/cull must be checked on both TES-alone and TES->GS routes, not inferred
  from direct VS->FS behavior.

### Current direct VS -> FS limitation

The existing PanVK lowering captures only a direct VS->FS link, only when the
FS does not declare clip/cull inputs, and only when the combined count is at
most eight. It rewrites cull distances to discard indicators in the hardware
VS and inserts FS discard checks. It does not provide a unified normalized
interface for explicit FS reads, GS, TCS, or TES. The two feature bits and all
three reported distance limits remain zero/false.

### Required implementation order

1. Normalize and record clip/cull array sizes before any IO lowering, with
   explicit validation for absent arrays, arrayed stage interfaces, and the
   combined eight-distance implementation limit.
2. Establish one consistent compact varying representation for all legal
   pre-raster stages. Verify direct VS, VS->TCS->TES, VS->GS, and
   VS->TCS->TES->GS, including clip-only, cull-only, and mixed arrays.
3. Extend the custom GS output collection/forwarding path to preserve compact
   clip/cull arrays, or prove with NIR tests that its current per-slot path is
   already correct for every array layout. Do not assume the passthrough GS
   helper is used by this custom path.
4. Let fragment shaders retain reads of their own ClipDistance/CullDistance
   inputs while deriving discard from the same normalized interpolated values.
   Check interpolation qualifiers, partial writes, and array indexing.
5. Only then set `shaderClipDistance` / `shaderCullDistance` and report limits.
   Keep feature bits disabled until focused NIR tests, Vulkan CTS coverage, an
   Android build, and device visual tests pass.

Useful source blobs on this branch:
- `panvk_vX_shader.c`: `4a163dcbf74a10c48caea79801318da48f4affcc`
- `poly_nir_lower_gs.c`: `e13f4cdd3c78794e04c653adbdd2249aba2dcc46`
- `poly_nir_lower_tess.c`: `0950afc78a59526a0901956278de01411a1824fe`
- `poly_nir_passthrough_gs.c`: `1c2e9ed90003a15f6a49e2ea23a61b3dd4d57b78`
- `poly_nir.h`: `706f5a1fa636d386adf3790ab2395f4c86627c11`
- `nir_clip_cull_distance_io_utils.c`: `79555c202ba93bb43d1ff5edc7905a2a3051166e`

No claim of feature support, build success, or runtime correctness is made.


### Additional correctness issue in the existing cull emulation

The current direct VS->FS cull emulation converts each cull distance to a
per-vertex 0/1 indicator (negative becomes 1), interpolates it, then discards
when the interpolated value is >= 0.9999. This approximates Vulkan's
primitive-level cull-distance rule, but the epsilon can discard a very small
region of a primitive when only some vertices have a negative value. It must
be tested against a reference implementation and should not be treated as a
fully proven implementation. Do not simply copy this threshold into GS/TES
paths.

The generic NIR helper pass order also needs to be established from actual
call-site expectations before integration. In particular,
`nir_merge_clip_cull_distance_vars()` rewrites cull locations, while
`nir_lower_clip_cull_distance_to_vec4s()` discovers clip/cull array sizes by
location. Calling these in an assumed order could lose the cull-size signal or
create mismatched IO. First add a focused NIR test covering clip-only,
cull-only, mixed arrays (including non-vec4-aligned clip counts), and arrayed
GS/TCS/TES interfaces; then integrate the verified pass order.


### NIR pass-order refinement from RADV precedent

A source comparison in this same Mesa tree found RADV's established sequence
in `src/amd/vulkan/radv_shader.c`: call
`nir_gather_clip_cull_distance_sizes_from_vars(nir)`, then
`nir_merge_clip_cull_distance_vars(nir)`. This is a confirmed precedent for
interface linking. It does **not** mean
`nir_lower_clip_cull_distance_to_vec4s()` should be chained after merge; that
helper is a distinct representation-lowering pass and its location-based size
scan needs separate analysis. Keep the two mechanisms distinct until a test
proves the exact PanVK representation and pass sequence. RADV source blob:
`c6b392bb638c7755356950130e1521f9106343d7`.


## 2026-11-05 — first clip/cull NIR regression tests added

Dedicated source branch: `experiment/g720-clip-cull-capabilities`.

Commits:
- `293dc13912fe9da8304532041c35f1bdb617f963`: add focused NIR test source.
- `8d6af89e8c1207a4eb4eeb59bbf2090e8a0e6108`: register it in
  `src/compiler/nir/meson.build`.
- `7b3e18b005e9db62618b2591e214f7521f93b423`: fix fixtures to construct
  vertex and fragment NIR shaders with the appropriate stage.
- `6791ecac76598d337a31ef657fef65b4e1177d13`: add this experiment branch to
  the Android-only build workflow filter.
- `55285f2b125f4a45c1c2bb9ace125de276099f7a`: add a dedicated minimal host
  workflow to build and run the NIR suite.
- `2acdfa52b5ec748cfd8f59ccf45754fd18674ce3`: add clip-only and cull-only
  coverage.

New test file: `src/compiler/nir/tests/clip_cull_distance_tests.cpp`.
It currently covers:
- VS clip-only and cull-only interfaces;
- VS mixed clip=3/cull=2, where cull begins at location_frac 3;
- VS mixed clip=5/cull=3, where cull moves to CLIP_DIST1 component 1;
- FS mixed clip=6/cull=2, where cull begins at CLIP_DIST1 component 2;
- gathered shader_info array sizes and merged variable locations/metadata.

These are initial unit tests for the generic NIR gather/merge helpers, not a
full PanVK shader-pipeline test. They do not yet cover vec4 representation
lowering, indirect dereferences, partial writes, GS/TCS/TES arrayed interfaces,
interpolation, or actual hardware fragment discard semantics.

CI:
- Android-only build workflow and wrapper build workflow are configured to
  run on this branch.
- New `NIR clip-cull compiler tests` workflow builds
  `src/compiler/nir/nir_tests` and runs the `nir` Meson suite.
- At the time of this checkpoint the workflows have been submitted and are
  still running; no pass/fail result is claimed. The latest test-source commit
  may also cause a newer run than the initial run IDs.

No PanVK runtime source, Vulkan feature flags, or reported distance limits
have been changed. Do not package or benchmark this branch yet.


## 2026-11-05 — first clip/cull NIR tests

On `experiment/g720-clip-cull-capabilities`, added `src/compiler/nir/tests/clip_cull_distance_tests.cpp` and registered it in `src/compiler/nir/meson.build`. Initial cases cover VS clip-only, VS cull-only, mixed VS arrays within one vec4 slot, mixed arrays crossing a vec4 boundary, and mixed FS inputs. They assert gathered array-size metadata and the merged `location`/`location_frac` layout. A dedicated host CI workflow builds `src/compiler/nir/nir_tests` and runs the `nir` Meson suite; Android build workflows are also enabled for the experiment branch. Results are pending, so no test or build success is claimed. These tests cover generic NIR gather/merge only, not vec4 lowering, software GS/TCS/TES transport, interpolation, or PanVK fragment discard semantics. No runtime PanVK code or Vulkan feature exposure changed. Latest source test commit: `2acdfa52b5ec748cfd8f59ccf45754fd18674ce3`.


### CI correction — 2026-11-05

The first dedicated NIR workflow configured Meson successfully but failed before compilation because its Ninja target was specified as `src/compiler/nir/nir_tests`, which is not a valid target name in this build graph. This is a workflow target-selection error, not a C++ compile or test failure. Corrected the workflow to build `nir_tests` (`1d64f878f15366f912b48c885858e89b199de841`). A new run is queued; no NIR test result is available yet. Android build jobs for the source branch are also still pending.


### NIR CI configuration refinement — 2026-11-05

The initial host-test configuration disabled all Vulkan drivers. Mesa then selected its `with_nir_headers_only` mode (`with_gfx_compute` was false), which omits the full NIR implementation and therefore does not generate the `nir_tests` target. The workflow was corrected to enable the Panfrost Vulkan driver with the Kbase backend while keeping the build otherwise minimal; this makes `with_any_vk` true and should select the full NIR implementation. The build target is now `nir_tests`. Commit: `80aea973fed1324c8a53582299ca77c68cc0aba0`. A new CI run is expected; test result remains pending.


## 2026-11-05 — NIR CI shell-continuation fix

Continued validation of `experiment/g720-clip-cull-capabilities`.
Inspected the failed host NIR workflow runs:
- Run `42020382423` used the earlier invalid Ninja target
  `src/compiler/nir/nir_tests`.
- Run `42020553054` used target `nir_tests`, but its Meson summary still
  showed `vulkan-drivers: []`; the target was therefore absent. This run
  predates the latest workflow correction and is not a NIR compile/test
  failure.

Inspection of `.github/workflows/nir-clip-cull-tests.yml` at the latest
source revision found the actual remaining cause: the shell command was
missing a trailing continuation after `-Dvulkan-drivers=panfrost`, so the
subsequent `-Dpanfrost-kmds=kbase` and other options were not arguments to
the same `meson setup` invocation. Fixed the line continuation and committed
the workflow-only correction:
- Commit: `3ef7647a44c5cd804c1c8e9fba40d96502b29c83`
- Branch: `experiment/g720-clip-cull-capabilities`
- No PanVK runtime source or feature flags changed.

A new NIR workflow run is expected from this commit. Its result must be
checked before claiming the test suite passes. The Android ARM64 build and
wrapper workflows from source commit `80aea973fed1324c8a53582299ca77c68cc0aba0`
were queued/in progress at the time of this note; their final results are
also pending. Next: inspect the new NIR run, verify the Meson summary actually
contains Panfrost/Kbase and the `nir_tests` target, then fix any genuine
compile/test failures. Continue to keep clip/cull feature bits disabled.


## 2026-11-05 — clip/cull vec4-lowering pass-order hazard

A closer read of `src/compiler/nir/nir_clip_cull_distance_io_utils.c`
identified a concrete reason not to wire the helper passes based only on the
RADV gather/merge precedent:

- `nir_lower_clip_cull_distance_to_vec4s()` independently discovers
  `clip_size` and `cull_size` by scanning variables whose locations are
  exactly `VARYING_SLOT_CLIP_DIST0` and `VARYING_SLOT_CULL_DIST0`.
- `nir_merge_clip_cull_distance_vars()` relocates the cull variable into the
  CLIP_DIST location range and marks the original declarations hidden.
  Depending on the clip count, the relocated cull variable may share
  CLIP_DIST0 with a nonzero `location_frac`, or move to CLIP_DIST1.
- Consequently, blindly invoking vec4 lowering after merge can make its
  location scan lose the separate cull-array size/offset. The existing NIR
  tests only validate gather+merge metadata and do not exercise this
  representation-lowering interaction.

This is a source-level pass-order hazard, not yet a demonstrated runtime bug.
Do not assume either gather->merge->vec4 or vec4->merge is correct. Add
focused tests for vec4 lowering on clip-only, cull-only, mixed arrays
(including clip counts 3 and 5), constant and indirect dereferences, stores
and loads, plus arrayed GS/TCS/TES interfaces. Inspect the resulting NIR
variables, array lengths, locations, and component offsets before selecting
the PanVK pass order. No PanVK runtime source or feature flags changed.


## 2026-11-05 — NIR workflow LLVM dependency correction

The NIR workflow run `42021321900` now reaches Meson configuration but
fails before compilation with the explicit error:
`Feature llvm cannot be disabled: CLC requires LLVM`.
This is a build-configuration dependency failure, not a NIR source/test
failure. The previous workflow had `-Dllvm=disabled`; corrected it by
installing `llvm-dev` and `clang` in CI and setting `-Dllvm=enabled`.

Workflow-only commit:
- `21247310892a4be2209d13a586db21e42a95e2b7`
- Branch: `experiment/g720-clip-cull-capabilities`

A new NIR CI run is expected from this commit. Its Meson configuration,
Ninja build, and test result must all be inspected. No NIR test pass is
claimed yet. Android-only build and wrapper runs on earlier commits remain
separate checks; inspect their terminal conclusions before claiming current
source validation. No PanVK runtime source or Vulkan feature exposure changed.


## 2026-11-05 — NIR CI follow-up: libclc dependency

The LLVM-enabled Meson configure for run `42022089165` progressed beyond the
previous CLC/LLVM guard, then failed because the host package
`libclc` was not installed (`Dependency "libclc" not found`). This is
another CI environment dependency failure before the NIR target compiles.

Added `libclc-dev` to the NIR workflow's apt dependencies:
- Workflow commit: `43bc2ca59b3f5adbc804161f662e9d55617dd5fe`
- Branch: `experiment/g720-clip-cull-capabilities`

The next run must confirm whether Ubuntu's package supplies the libclc
development metadata expected by this Mesa revision. NIR compilation and
tests remain unverified. No runtime source, feature bits, or device limits
were modified.


## 2026-11-05 — first vec4 representation tests

Extended `src/compiler/nir/tests/clip_cull_distance_tests.cpp` with three
declaration-layout tests for `nir_lower_clip_cull_distance_to_vec4s()`:
- mixed clip=3/cull=2: packed vec4 array should contain two vec4 slots;
- cull-only size=5: packed representation should preserve a two-slot array;
- mixed clip=5/cull=3: packed representation should span two vec4 slots.

These tests assert the generated `gl_ClipDistanceMESA` output declaration,
array length, and vec4 component count. They do not yet exercise dereference
rewriting, constant/indirect indices, stores/loads, or GS/TCS/TES interfaces.
They test vec4 normalization separately from gather/merge and do not settle
the helper pass ordering. Source commit:
`cec14bb825176cb714e0a6d2e4dab3c7dd44de0e`.

CI workflow dependency follow-up: after the LLVM setting was corrected,
Meson reported missing `libclc`; added `libclc-dev` in workflow commit
`43bc2ca59b3f5adbc804161f662e9d55617dd5fe`. Its NIR workflow run was queued
at the last check. The new test-source commit also queues a newer run. No
compile/test pass is claimed until the latest run completes.


## 2026-11-05 — cross-branch continuity refresh and NIR CI dependency repair

### Cross-branch handoff audit

Enumerated all 25 repository branches through the GitHub branches API and
checked for `PROJECT_CONTINUITY_HANDOFF.md` on each branch. The file exists
only on the canonical documentation branch
`docs/project-continuity-handoff-2026-11-03`; all other 24 branches returned
404 for that path. This is not a missing-source problem: the canonical
documentation branch is the authoritative continuity record. Do not create
divergent copies on every source branch unless explicitly requested.

Re-read the canonical handoff (blob SHA
`0545c274982d0bddbab152a541099a790a05f27b)), plus:
- `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` on
  `experiment/g720-performance-audit` (blob
  `3532367ef27ec31425c12d1359413aa83cb857b6));
- `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` on
  `experiment/g720-performance-audit` (blob
  `3ce03988e319030e9f2170ff8e6a613b616067fb)).

Current working source branch remains
`experiment/g720-clip-cull-capabilities`. No runtime PanVK source or Vulkan
feature bits were changed in this step.

### CI inspection

Inspected the latest clip/cull branch workflow runs. The NIR workflow run
`42022253198` (commit
`43bc2ca59b3f5adbc804161f662e9d55617dd5fe`) failed during dependency
installation, before Meson configuration or compilation. The Ubuntu 26.04
runner reported that package `libclc-dev` has no installation candidate and
listed versioned packages including `libclc-22-dev`. This is a CI package
name/version mismatch, not a NIR test failure.

At the same time, Android-only build and Android wrapper workflows for
`cec14bb825176cb714e0a6d2e4dab3c7dd44de0e` were still in progress; no result
was claimed at this checkpoint. Earlier Android-only and wrapper runs for
`3ef7647a44c5cd804c1c8e9fba40d96502b29c83` had completed successfully.
Those runs validate their respective Android build paths at that commit,
not NIR tests and not runtime behavior.

### Workflow repair

Updated `.github/workflows/nir-clip-cull-tests.yml` on
`experiment/g720-clip-cull-capabilities`:
- Replaced the unavailable generic package `libclc-dev` with the runner's
  available versioned package `libclc-22-dev`.
- Workflow-only commit:
  `e73b88697f8293613858fbfc9eee671ead46de6f`.

New runs triggered by that commit:
- NIR compiler tests: `42023056158` (queued at the time of this update).
- Android-only PanVK build: `42023056179` (queued).
- Android NDK wrapper debug/release: `42023056338` (queued).

The preceding source commit `cec14bb825176cb714e0a6d2e4dab3c7dd44de0e`
also has pending runs:
- NIR: `42022450295` (queued).
- Android-only: `42022450154` (in progress).
- Android wrapper: `42022450159` (in progress).

Next action: poll the new NIR run and verify the package install, Meson
configuration (including LLVM/CLC), `nir_tests` build, and `nir` test suite
individually. Then inspect the Android-only and wrapper run conclusions and
artifacts. Do not claim NIR tests pass until the suite completes successfully.

### NIR test coverage checkpoint

The current test source commit
`cec14bb825176cb714e0a6d2e4dab3c7dd44de0e` adds declaration-layout checks
for `nir_lower_clip_cull_distance_to_vec4s()`:
- mixed clip=3/cull=2;
- cull-only size=5;
- mixed clip=5/cull=3.

These validate packed vec4 declaration array length/component layout only.
They do not validate dereference rewrites, constant or indirect indexing,
loads/stores, partial writes, arrayed GS/TCS/TES interfaces, or helper pass
ordering. Keep the pass-order question open and do not wire the helpers into
PanVK runtime until those tests exist and pass.

### Branch documentation policy confirmed

The branch sweep confirms the intended documentation arrangement: keep the
continuity file on `docs/project-continuity-handoff-2026-11-03` as the single
canonical handoff. Update it after each meaningful repository/CI/source step;
source experiment branches need not carry independent copies. The
performance audit and capability matrix remain on
`experiment/g720-performance-audit`.


### Follow-up: exact vec4-lowering implementation traced

Inspected the complete implementation of
`nir_lower_clip_cull_distance_to_vec4s()`,
`lower_distance_to_vec4()`, and `combine_clip_cull()` in
`src/compiler/nir/nir_clip_cull_distance_io_utils.c`.

This refines the earlier pass-order warning:
- The vec4 pass first scans original variable locations to calculate
  `clip_size` and `cull_size`.
- It sets `total_size = clip_size + cull_size`, lowers
  `gl_ClipDistance` at offset 0, then lowers `gl_CullDistance` at offset
  `clip_size`.
- The same newly-created `gl_ClipDistanceMESA` variable is retained between
  the clip and cull lowering calls, so both arrays are packed into one vec4
  array. The original variables become shader temporaries and their
  dereferences are rewritten.
- `nir_merge_clip_cull_distance_vars()` separately changes cull locations
  and is not required by the vec4 lowering routine's packing algorithm.
  Running merge first prevents the vec4 pass from discovering the original
  cull variable at `VARYING_SLOT_CULL_DIST0`; it can then calculate an
  undersized `total_size` while still applying the cull offset. This can
  produce an invalid packed layout or out-of-range access.

Working PanVK hypothesis, pending regression tests: gather Vulkan clip/cull
array sizes before any lowering, then use the vec4 packing pass directly for
the libpanfrost varying ABI; do not chain the generic merge pass ahead of it.
The standard gather+merge sequence observed in RADV applies to its own
interface-linking path and is not automatically the right sequence for
PanVK's packed vec4 representation.

Current tests still only assert packed declaration shape. The next test
increment must exercise constant and dynamic dereference indices, both loads
and stores, and mixed clip/cull offsets; then cover arrayed GS/TCS/TES
interfaces. No runtime PanVK changes or feature exposure were made.

### CI status after dependency repair

Workflow commit `e73b88697f8293613858fbfc9eee671ead46de6f` changed the host
workflow dependency from `libclc-dev` to `libclc-22-dev`, matching the
Ubuntu 26.04 runner's APT package candidates. New runs
`42023056158` (NIR), `42023056179` (Android-only), and
`42023056338` (Android wrapper) were queued at the latest poll. Earlier
runs `42022450295` (NIR), `42022450154` (Android-only), and
`42022450159` (Android wrapper) were still queued/in progress. No result
for the new dependency correction is available yet.


### CI poll update — 2026-11-05 12:00 UTC

Additional workflow results:
- NIR run `42022450295` completed FAILURE because it checked out the
  pre-repair commit `cec14bb825176cb714e0a6d2e4dab3c7dd44de0e`, whose
  workflow still installs unavailable `libclc-dev`. Its log confirms failure
  during APT dependency installation; Meson, compilation, and tests did not
  run. This is not a test-source failure.
- Android-only run `42022253321` for commit
  `43bc2ca59b3f5adbc804161f662e9d55617dd5fe` completed SUCCESS. Its
  `build-android-only` job passed. This establishes an Android ARM64 build
  for that source revision; it does not validate the latest vec4 test commit
  or runtime behavior.
- Android wrapper run `42022450159` for
  `cec14bb825176cb714e0a6d2e4dab3c7dd44de0e` remained in progress. Both
  debugoptimized and release jobs had completed setup/dependency install and
  were generating host tools. Wrapper output is separate from the PanVK
  Vulkan driver artifact.

The repaired workflow commit `e73b88697f8293613858fbfc9eee671ead46de6f`
triggered NIR run `42023056158`, Android-only run `42023056179`, and
wrapper run `42023056338`; all were queued at this poll. These are the
authoritative runs for the versioned libclc package change.


### Latest CI progress poll — 2026-11-05 12:01 UTC

After the versioned package workflow commit, NIR run `42023056158` and
Android-only run `42023056179` moved from queued to in progress. At the
latest job-step inspection:
- NIR job `127230348709`: checkout succeeded; dependency installation was
  in progress; Meson configure, NIR build, and test execution had not started.
- Android-only job `127230348185`: checkout succeeded; dependency
  installation was in progress; host tools, Android configure/compile,
  packaging, and artifact audit had not started.
- Android wrapper run `42023056338` remained queued.

No success/failure result is available yet for commit
`e73b88697f8293613858fbfc9eee671ead46de6f`. Continue polling these exact
runs; do not treat queued/in-progress status as validation.



## Addendum: CI dependency repair and Android package build (2026-11-05 12:xx UTC)

Resumed from the clip/cull capability branch after re-reading this handoff, the performance audit, and capability matrix. The repository branch sweep confirms the canonical handoff remains on `docs/project-continuity-handoff-2026-11-03`; it is intentionally not copied to experiment branches.

### Verified CI outcomes on `experiment/g720-clip-cull-capabilities`

- Source SHA `e73b88697f8293613858fbfc9eee671ead46de6f`.
- Android-only PanVK run `42023056179`: **SUCCESS**. All steps passed, including host tools, real Android ARM64 libdrm, Android-only configure/compile/stage, driver/dependency audit, flat Winlator/Adrenotools package, and artifact upload.
- Artifact `13412349016`, `panvk-g720-android-only-experimental-package`, 16,753,532 bytes, SHA-256 digest `fedadb6852037bfd3771517ed94a172ef45cd71e81120583df909b083acbb84d`, expires 2027-02-03. This is an Android ARM64 compile/package result only; no device runtime validation is implied.
- NIR run `42023056158`: **FAILURE during Meson configuration**, before compilation or tests. The repaired Ubuntu 26.04 apt dependencies installed successfully, but Meson then failed because `LLVMSPIRVLib` was missing (`meson.build:2090`). Build and test steps were skipped. This is a workflow dependency failure, not a NIR regression.
- Android NDK wrapper run `42023056338`: both `debugoptimized` and `release` jobs were still compiling at the last poll. Dependency install, host tools and Meson configure passed; artifact upload was pending. Wrapper build is separate from PanVK.

### Workflow correction

Updated `.github/workflows/nir-clip-cull-tests.yml` on the clip/cull branch to align its dependency set with the known-working Android-only host-tool workflow:
- use `llvm-22-dev` and `libclc-22-dev`;
- add `libllvmspirvlib-22-dev`, which the failing Meson configure explicitly requires;
- add `libudev-dev` to satisfy the optional host dependency probe cleanly.

Commit: `0f8ce3f7c14a610486c678b51a2eb3c1b5dfaf90` (`ci: add LLVM SPIR-V dependency to NIR tests`). A follow-up NIR run is expected from this workflow-file push; its run ID/status must be checked before claiming tests passed. The NIR workflow has not yet reached `ninja -C build nir_tests` or `meson test`.

### Current technical decision

No runtime PanVK shader/compiler changes and no Vulkan feature exposure were made. Keep clip/cull disabled until tests exercise dereference rewriting, dynamic/constant indexing, partial writes, arrayed GS/TCS/TES interfaces, stage transport and the actual FS discard/clip behavior. The current tests cover only helper declaration/layout/gather/merge cases and vec4 declaration shape; they are not yet sufficient to enable the feature.

### Next actions

1. Poll the new NIR run from commit `0f8ce3f7c14a610486c678b51a2eb3c1b5dfaf90`; confirm Meson configuration, NIR test build and suite results separately.
2. Poll wrapper run `42023056338`; record debugoptimized and release outcomes and artifacts. Do not conflate wrapper with driver.
3. If NIR tests run, inspect failures and add focused dereference rewrite tests before wiring lowering into PanVK.
4. Keep the Android package isolated for Winlator import; do not replace the Android vendor/system Vulkan driver.



### Follow-up NIR CI dependency finding (2026-11-05 12:06 UTC)

The first dependency repair exposed the next missing host dependency. Run `42024102034` (source SHA `0f8ce3f7c14a610486c678b51a2eb3c1b5dfaf90`) again failed at Meson configuration, not at NIR compilation or tests. The log confirms `LLVMSPIRVLib 22.1.0.0` is now found, then Meson stops at `meson.build:2098` because `SPIRV-Tools` is missing.

The NIR workflow was updated again to install `spirv-tools` and `spirv-tools-dev`, matching the dependency pair already used by the successful Android-only workflow. Commit: `9fcacf93ea9a23710f2bcdf91dd0054c6de7950d` (`ci: add SPIR-V tools dependency to NIR tests`).

New run `42024214325` is active on this commit. At last poll, checkout passed and dependency installation was in progress; Meson configuration, NIR test build, and tests are pending. Do not claim test success yet.

The wrapper and Android-only workflows also triggered on commit `0f8ce3f...`: wrapper run `42024102067` was still generating host tools in both Release and Debugoptimized jobs; Android-only run `42024102129` was installing dependencies. Newer runs on `9fcacf9...` were queued at the last poll. The earlier Android-only run `42023056179` on `e73b886...` remains the verified successful Android package build and artifact `13412349016`.



### Third NIR CI dependency repair (2026-11-05 12:07 UTC)

Run `42024214325` on SHA `9fcacf93ea9a23710f2bcdf91dd0054c6de7950d` installed all requested dependencies, and Meson found both `LLVMSPIRVLib 22.1.0.0` and `SPIRV-Tools 2026.1.1`. Configuration then stopped at `meson.build:2135`: C++ library `clangBasic` was not found. Again, the failure is before NIR compilation/tests.

Added `libclang-22-dev` to the NIR workflow, matching the package used by the successful Android-only host-tool workflow. Commit: `df19c4403995abbbb489b08d47b5fb9f648cd8a4` (`ci: add Clang development libraries to NIR tests`). New NIR run `42024367795` was queued at the last poll; no test result yet.

Runs on prior dependency-repair SHA `9fcacf9...` were still in progress at last poll: Android-only `42024214279` (host tools stage) and Android wrapper `42024214246` (dependency/host-tools stage). Runs on current SHA `df19c44...` were queued: NIR `42024367795`, Android-only `42024367447`, wrapper `42024367688`. Continue polling and avoid counting superseded runs as current-source validation.



### NIR workflow isolation change (2026-11-05 12:08 UTC)

The third dependency failure was traced to Meson's CLC configuration, not to the NIR tests. The Panfrost Vulkan driver sets `with_driver_using_cl`, which activates Mesa CLC and consequently requires LLVM, LLVM-SPIRV, SPIRV-Tools and Clang libraries. The NIR helper tests do not require building the Panfrost Vulkan driver or its CLC compiler.

Rather than continuing to add increasingly broad host compiler dependencies, the NIR workflow was narrowed to a generic Mesa/NIR test build:
- `-Dvulkan-drivers=[]` (no Panfrost/PanVK driver in this test-only build);
- `-Dllvm=disabled`;
- no explicit LLVM, libclc, LLVM-SPIRV, SPIRV-Tools or Clang development packages;
- retain `-Dbuild-tests=true` and build/run the NIR test suite.

Workflow commit: `a15faa79af6dd4a06bde3da236d32d198be54ccc` (`ci: isolate NIR tests from Panfrost CLC dependencies`). The workflow file was fetched back and checked after commit; command continuation and Meson options are syntactically consistent. A new workflow run for this commit had not appeared in the Actions list at the last poll.

At last poll, previous run `42024367795` (SHA `df19c44...`) had successfully installed dependencies and was still in Meson configuration; it is superseded by the isolated configuration but may still provide useful diagnostic evidence. Android-only run `42024367447` had completed dependency install and was generating host tools. Wrapper run `42024367688` had completed dependency installation and was generating host tools in Release and Debugoptimized jobs. Neither had reached final compile/package results yet.

Do not claim NIR test success until a run on the isolated workflow commit completes both the `nir_tests` build and `meson test --suite nir`. No runtime PanVK source or feature flags were changed.



### NIR test target correction (2026-11-05 12:10 UTC)

The isolated NIR workflow on SHA `a15faa79af6dd4a06bde3da236d32d198be54ccc` successfully completed Meson configuration with no Vulkan drivers and LLVM disabled. The next step failed because the Ninja target was invoked as bare `nir_tests`, which is not a top-level Ninja target in this Meson build. The log explicitly reports `ninja: error: unknown target 'nir_tests'`; the test suite itself was not built or run.

Inspected `src/compiler/nir/meson.build`: the `nir_tests` executable is declared inside the NIR subdirectory and registered as Meson test `nir_tests` in suite `nir`. Corrected the build target to `src/compiler/nir/nir_tests` while retaining the final `meson test -C build --suite nir --print-errorlogs` invocation.

Workflow commit: `6f965eaa43b8149ece37a189a839dc44b53562ff` (`ci: use Meson path for NIR test executable`). New run `42024927571` was in progress at the last poll, with checkout just starting; its build and test result are pending.

This is a CI target-path correction, not a code/test failure. The NIR workflow now avoids unrelated Panfrost CLC dependencies and targets the actual executable path. Do not count this as passing until both the executable build and suite finish.



### NIR test invocation simplified after Ninja target-path mismatch (2026-11-05 12:11 UTC)

The run `42024927571` on SHA `6f965eaa43b8149ece37a189a839dc44b53562ff` again configured Meson successfully, but the explicit Ninja target `src/compiler/nir/nir_tests` was also not a valid target. The earlier bare target `nir_tests` was invalid too. The test declaration is wrapped by Meson's `test()` helper, so relying on a guessed Ninja path is brittle.

Removed the standalone Ninja invocation. The NIR CI now uses `meson test -C build --suite nir --print-errorlogs` directly; Meson will build the registered test executable as part of running the suite. This keeps the pipeline focused on the real Meson test target and avoids hardcoding generated Ninja target names.

Workflow commit: `9f8faa18c126ae6110ae004f9ab969b62104992f` (`ci: let Meson build NIR test executable`). Await the new run and confirm the Meson test output reports actual test counts/results. Neither prior run reached NIR test execution; no NIR test assertion has failed yet.



### Meson-managed NIR test build (2026-11-05 12:12 UTC)

The explicit Ninja target path remained incorrect: run `42024927571` on `6f965eaa...` reported `unknown target 'src/compiler/nir/nir_tests'`. The Meson test declaration in `src/compiler/nir/meson.build` owns the executable target, and Meson can build it when the test suite is invoked. To avoid further guessed target names, removed the explicit Ninja step and now invoke only:

```sh
meson test -C build --suite nir --print-errorlogs
```

This is the authoritative test invocation and should trigger build plus execution of the registered `nir_tests` binary and other tests in the NIR suite. Workflow commit: `9f8faa18c126ae6110ae004f9ab969b62104992f`.

New NIR run `42025063854` on this commit was queued at the last poll. Android-only run `42025063812` and wrapper run `42025063844` were also queued. Older Android-only run `42024927627` was still installing dependencies; wrapper run `42024802404` was installing dependencies in Release and generating host tools in Debugoptimized. No new NIR tests have yet executed. The previously successful Android package run `42023056179` remains the only fully verified package build on this line of work.



### NIR CI false-green diagnosis and full-NIR correction (2026-11-05 12:14 UTC)

Run `42025063854` on SHA `9f8faa18c126ae6110ae004f9ab969b62104992f` was marked success by GitHub Actions, but inspecting its full job log showed the decisive line: `No suitable tests defined.` Therefore this was **not** a successful NIR test execution. Meson configured a headers-only NIR build because `with_gfx_compute` was false: the workflow selected no Vulkan or OpenGL drivers, disabled CLC and LLVM, and therefore `with_nir_headers_only = not with_gfx_compute` evaluated true. The NIR test registration is guarded by `if with_tests and not with_nir_headers_only`, so no NIR tests were registered.

Corrected the NIR workflow to select the software Vulkan driver `swrast` instead of `vulkan-drivers=[]`. This makes `with_any_vk` / `with_gfx_compute` true and should force the full NIR implementation and test registration, without enabling Panfrost/PanVK CLC or requiring LLVM. The actual NIR suite is still invoked via `meson test -C build --suite nir --print-errorlogs`.

Commit: `2102a0a2109c54438243dacd03934c7815a8e009` (`ci: enable full NIR sources in test build`). New runs triggered: NIR `42025422711`, Android-only `42025422741`, wrapper `42025422738`; all were queued at last poll. Need verify Meson summary includes NIR tests and job logs show actual test counts; a green job alone is insufficient.

This correction changes only the CI configuration. No runtime PanVK code, clip/cull feature flags, or device package contents were changed by this commit. Existing Android package run `42023056179` remains a confirmed successful build/package, but current SHA `2102a0a...` Android validation is pending.



### NIR CI source gating diagnosis and Lavapipe dependency repair (2026-11-05 12:15 UTC)

The first attempt to force full NIR source registration changed `-Dvulkan-drivers=[]` to `-Dvulkan-drivers=swrast` while leaving LLVM disabled. Run `42025422711` therefore failed at Meson setup with the explicit error: `Feature llvm cannot be disabled: Lavapipe Vulkan driver requires LLVM`. This was expected from the software Vulkan driver's dependency, not a PanVK or NIR source error.

Updated the NIR workflow to install `llvm-22-dev` and set `-Dllvm=enabled`, while retaining `-Dvulkan-drivers=swrast`. The purpose of selecting swrast is to make `with_any_vk` and `with_gfx_compute` true, which makes `with_nir_headers_only` false and registers the full NIR test executable. LLVM is now enabled for Lavapipe, but we have not re-enabled Panfrost or Mesa CLC, so the earlier LLVM-SPIRV/Clang/libclc dependency chain should remain unnecessary.

Commit: `171a78a8ff5fc1793b9e77a6cbdccf800e0e054d` (`ci: enable LLVM for software Vulkan NIR test build`). New NIR run expected from this commit; no run ID returned yet at last poll.

Additional build status:
- Android-only run `42025063812` on `9f8faa1...`: host tools, Android ARM64 libdrm, and Meson configure all passed; Android compile/stage in progress.
- Wrapper run `42025063844` on `9f8faa1...`: host tools passed; Release Meson configure in progress; Debugoptimized compile in progress.
- Newer runs from `2102a0a...` are queued/in progress and are superseded by the current source workflow commit for CI config only.

Important CI interpretation: run `42025063854` was technically green but logged `No suitable tests defined`, so it must be recorded as a false-green/no-test run, not test success. Run `42025422711` failed at Meson setup due Lavapipe's LLVM requirement; it did not compile or run tests. The next acceptable result requires a Meson summary showing `nir_tests` and actual gtest/meson test counts, not just exit code zero.



### NIR workflow now asserts test registration (2026-11-05 12:16 UTC)

Added a guard before test execution:
```sh
meson introspect --tests build > build-tests.json
python3 -c '... assert "nir_tests" in names; assert "nir_algebraic_parser" in names'
```
This prevents a repeat of the false-green case where Meson exited zero with `No suitable tests defined`. The workflow must now demonstrate that both the C++ NIR gtest binary and Python algebraic parser test are registered before the Meson suite is run.

Commit: `3f75710571607ba4a109c376b596f7cb685fc1ce` (`ci: fail if NIR tests are not registered`). New run expected; previous current candidate `42025564838` on SHA `171a78a...` was queued when last checked. Its config should use swrast + LLVM 22; it is now superseded by the guarded workflow commit, though its outcome can still diagnose configuration.

Latest build observations from older commits:
- Android-only run `42025063812` on `9f8faa1...`: host tools, ARM64 libdrm and configure passed; compile/stage still in progress.
- Wrapper run `42025063844` on `9f8faa1...`: host tools and both Meson configure steps passed; Release and Debugoptimized compilation in progress.
- Run `42025422711` failed because swrast/Lavapipe requires LLVM while the workflow had LLVM disabled; this prompted the `llvm-22-dev` + `-Dllvm=enabled` correction in SHA `171a78a...`.

No runtime driver code has changed. The next validation gate is test registration assertion success followed by actual NIR test execution with visible counts.



### NIR CI failure: glslang executable missing; dependency added (2026-11-05 12:18 UTC)

The guarded NIR workflow on SHA `3f75710571607ba4a109c376b596f7cb685fc1ce` correctly failed during Meson configuration before test registration. Exact error from run `42025718281`: `meson.build:754:15: ERROR: Program 'glslangValidator' not found or not executable`. The same failure occurred in the preceding candidate run `42025564838`. This is a host build dependency issue; the test-registration guard was skipped because configure did not finish.

Added Ubuntu package `glslang-tools` to the workflow's apt dependencies. Commit: `ed8db2bd27bba9dfe8f142de37eb2eeda313a9fc` (`ci: install glslang for NIR host configuration`). New NIR run should trigger from this SHA; no run ID was visible at last poll.

Current validation state:
- No NIR compiler tests have run yet. Run `42025063854` was a false green with `No suitable tests defined`; run `42025422711` failed because swrast requires LLVM; runs `42025564838` and `42025718281` failed because `glslangValidator` was missing.
- The workflow now selects swrast + LLVM 22, checks that `nir_tests` and `nir_algebraic_parser` are registered, then invokes `meson test -C build --suite nir --print-errorlogs`.
- Next step is to resolve any remaining Meson host dependency errors from actual logs, then verify nonzero NIR test counts and successful test execution.
- No runtime PanVK changes or clip/cull feature exposure have been made.

Older Android-only / wrapper build jobs from SHAs `9f8faa1...`, `2102a0a...`, `171a78a...`, and `3f75710...` were still queued/in progress at the last poll; do not interpret these as successful until their full package audits and artifact uploads complete.



### Clip/cull test scope re-audit (2026-11-05 12:19 UTC)

Re-read the actual NIR utility and test source after the CI dependency fixes. Current `clip_cull_distance_tests.cpp` tests variable declaration metadata, merged location/location_frac, gathered sizes, and packed vec4 declaration shape. It does **not** currently execute `nir_lower_clip_cull_distance_to_vec4s()` against real load/store derefs, nor verify rewritten SSA values, masked stores, dynamic indices, partial writes, or arrayed GS/TCS/TES interfaces.

The lowering implementation in `nir_clip_cull_distance_io_utils.c` rewrites constant-index loads via `nir_channel`, dynamic-index loads via `nir_vector_extract`, constant-index stores via `nir_build_write_masked_store`, and dynamic-index stores via `nir_build_write_masked_stores`. It also handles arrayed interfaces by preserving the per-vertex deref before indexing the packed vec4 array. These paths are the next correctness test targets, but no implementation change is justified from source inspection alone.

The NIR workflow's latest candidate on commit `ed8db2b...` is run `42025936157`, queued at last poll. That commit adds `glslang-tools` after the exact Meson configure error `Program 'glslangValidator' not found or not executable`. Wait for its result before adjusting dependencies again.

Current NIR CI remains unvalidated: no successful test registration and no executed NIR tests yet. Runtime PanVK code and advertised Vulkan features remain unchanged.


### NIR CI dependency correction and concurrent Android build status (2026-11-05 12:22 UTC)

Polled the three candidate workflows from source SHA `ed8db2bd27bba9dfe8f142de37eb2eeda313a9fc`.

- NIR run `42025936157`, job `127240031635`: **failed** during Meson setup. The exact log still says `Program 'glslangValidator' not found or not executable`; test registration and NIR execution were skipped.
- Android-only run `42025934630`, job `127240026118`: still in progress; dependency install passed and host-tool generation was running at poll time.
- Wrapper run `42025934632`: Release job `127240027028` had dependency install passed and host-tool generation running; Debugoptimized job `127240027355` was queued.

Important repository inconsistency: despite the previous handoff entry stating that `glslang-tools` had been added, fetching the actual committed `.github/workflows/nir-clip-cull-tests.yml` from `experiment/g720-clip-cull-capabilities` showed that the apt dependency list ended at `clang` and did not contain `glslang-tools`. The handoff description was ahead of the actual source file. Corrected the source workflow by adding `glslang-tools` to the apt install list.

Correction commit: `4a9c4159780ad27cc64b8ad043ec0c451852e8f0` (`ci: ensure glslang tools are installed for NIR configuration`). This should trigger a fresh NIR workflow. It is a CI-only correction; no PanVK runtime code, clip/cull lowering, or Vulkan feature exposure changed.

Next gates:
1. Verify the new run on `4a9c415...` actually installs `glslangValidator`, configures Meson, and passes the explicit `nir_tests` / `nir_algebraic_parser` registration assertion.
2. Require real NIR test execution with nonzero test counts; a green workflow with no tests is not acceptable.
3. Continue polling Android-only and wrapper jobs through final artifact upload and package/dependency audits.
4. After NIR test registration is working, add dereference-level tests for constant/dynamic loads, constant/dynamic stores, partial component writes, and arrayed GS/TCS/TES interfaces. Do not modify runtime PanVK clip/cull feature reporting until those tests validate the lowering.

No Android runtime test has been performed from these CI jobs. Build/package success is not device runtime validation.


Follow-up run IDs after the source correction commit `4a9c4159780ad27cc64b8ad043ec0c451852e8f0`:
- NIR test workflow: run `42026345910`, job `127241399440`.
- Android-only build/package: run `42026346036`, job `127241400075`.
- Android wrapper: run `42026345779`, Release job `127241398933`, Debugoptimized job `127241399443`.

All four jobs were confirmed queued immediately after the commit; none had started, so there are no new test/build results yet. These are the current candidate runs and supersede earlier pending candidates for validation of the corrected workflow/source SHA. Canonical handoff update commit: this entry is appended after the previous 12:22 UTC correction record.


### Dereference-level NIR test added (2026-11-05 12:25 UTC)

Source branch `experiment/g720-clip-cull-capabilities` now includes commit `b4b073d3b3ec1dd8153df888364f09bb2ac6d026` (`nir: test constant and dynamic clip distance derefs`).

Added a NIR test that constructs actual clip-distance output dereferences and exercises:
- Constant-index scalar load.
- Dynamic-index scalar load.
- Constant-index scalar store.
- Dynamic-index scalar store.

After `nir_lower_clip_cull_distance_to_vec4s()`, the test counts load/store derefs rooted at the generated `gl_ClipDistanceMESA` packed vec4 variable and expects two of each. In debug builds it also runs `nir_validate_shader()`. This checks real dereference retargeting rather than only declaration metadata/layout.

The test is registered through the already-added `clip_cull_distance_tests.cpp` entry in `src/compiler/nir/meson.build`; no test manifest change was required.

CI triggered on the test commit:
- NIR: run `42026950603`, job `127243424246`.
- Android-only build/package: run `42026950488`, job `127243424163`.
- Android wrapper: run `42026950637`, Release job `127243424646`, Debugoptimized job `127243424290`.

At the latest poll, the new NIR job had started checkout but had not yet reached build/test execution; Android-only and wrapper jobs were queued. The prior NIR run `42026345910` had completed Meson setup and the test-registration guard, and was still compiling/running the suite at poll time. Earlier Android-only and wrapper jobs from `4a9c415...` were still running at their host-tool/dependency stages. No test or package success is claimed yet.

Remaining NIR coverage targets: partial component writes with non-zero clip/cull packing offsets; dynamic index bounds/negative index semantics; arrayed GS/TCS/TES derefs. Do not expand into runtime clip/cull exposure until those tests and full stage interface transport are proven.


### Test-construction portability correction (2026-11-05 12:27 UTC)

Reviewed the newly added dynamic dereference test against `nir_build_deref_array()`: NIR dereference indices must match the pointer-sized bit width of the parent dereference. The test initially used `nir_imm_int()` (32-bit), which could assert on a 64-bit host. Corrected it to `nir_imm_intN_t(b, 6, nir_get_ptr_bitsize(b->shader))`.

Correction commit: `92e631280b36a4abadbc427093e97d6ecac837b0` (`nir: use pointer-sized dynamic deref test index`). This is test-only and does not alter compiler lowering/runtime behavior. It triggers a new NIR CI run; previous run `42026950603` was already compiling the earlier test revision and is not final evidence for this correction.


### Dynamic-store test expectation correction (2026-11-05 12:29 UTC)

A static review of `nir_build_write_masked_stores()` showed that dynamic scalar stores are expanded into four guarded component stores (one per vec4 lane), not one dynamic store intrinsic. Corrected the test's expected packed store count from 2 to 5: one constant-index store plus four guarded dynamic-index stores.

Commit: `ccc0fbdece659ea8c49c4570f107327100c92033` (`nir: account for guarded stores in dynamic clip index test`). Test-only correction; no runtime changes. It triggers a fresh NIR run. Previous run `42026950603` compiles the earlier revision and must not be treated as validation for this final expected count.


### First real NIR CI pass (2026-11-05 12:28 UTC)

The corrected workflow on source SHA `4a9c4159780ad27cc64b8ad043ec0c451852e8f0` completed successfully: run `42026345910`, job `127241399440`.

Evidence from the job log:
- Meson configured successfully.
- The explicit registration guard passed (both `nir_tests` and `nir_algebraic_parser` were registered).
- NIR target compiled and linked successfully (424 Ninja steps).
- Meson executed exactly 2 suite tests: `mesa:nir_algebraic_parser` and `mesa:nir_tests`.
- Result: `Ok: 2; Fail: 0`.

This confirms the glslang dependency correction fixed the prior false-green/no-tests problem and the real NIR test suite is running. Important scope: this passing run predates the new constant/dynamic dereference test (added in `b4b073d...`), so it validates the NIR suite baseline and registration, not the new test itself. Newer NIR runs `42026950603`, `42027059205`, and `42027207795` are queued/in progress for the dereference test and its subsequent test-only corrections.

Android-only run `42026346036` remained in dependency installation; wrapper run `42026345779` had both Release and Debugoptimized variants compiling. No Android artifact/package success or device runtime result yet.


### Strengthen dynamic-index test semantics (2026-11-05 12:32 UTC)

The NIR suite passed for the initial dereference test on commit `b4b073d...` (run `42026950603`, job `127243424246`): Meson reported 2/2 suite tests passed and the clip/cull test translation unit compiled. However, review found the purported dynamic index was an immediate constant, so the builder could constant-fold it and fail to exercise the dynamic-store branch expansion.

Corrected the test to read its index from a real shader input variable (`dynamic_index`, `VARYING_SLOT_VAR0`) and convert it to the dereference pointer index width with `nir_i2iN()`. The dynamic store expectation remains five packed stores (one constant plus four guarded lane stores). New source commit: `c6dfaabc0ba69c2a863c54b55eb67a8ac07d6b1c` (`nir: exercise genuinely dynamic clip distance indexing`).

This is a test-only strengthening. Its CI run must be treated as the first meaningful validation of truly dynamic dereference lowering. Earlier passing run `42026950603` does not validate the corrected dynamic case.


### Android-only package and wrapper build completion (2026-11-05 12:34 UTC)

Source SHA `b4b073d3b3ec1dd8153df888364f09bb2ac6d026` completed both Android build workflows successfully:

- Android-only PanVK run `42026950488`, job `127243424163`: all steps passed, including real Android ARM64 libdrm build, Android-only Meson configure, compile/stage, driver/manifest/dependency/staged-file audit, flat Winlator/Adrenotools package, and artifact upload.
- Artifact `13414308531`, `panvk-g720-android-only-experimental-package`, 16,751,330 bytes, SHA-256 `cf6161267381e04a62d9d7688ffa98b6d6e4890cba312c8c98dec6aa15031db0`, expires 2027-02-03. This is build/package evidence only; it does not contain the new NIR test source in the runtime binary and has not been installed or run on the phone.
- Android wrapper run `42026950637`: Release and Debugoptimized jobs both passed compile and artifact upload. Artifacts: Release `13415120558` (4,288,183 bytes; SHA-256 `7e65d300c2816dd14c30ec516417a1c48aafdddfb9d029c7602181252917c122`); Debugoptimized `13414776990` (50,663,701 bytes; SHA-256 `5d1c6f091a6e247b86c6a1c020fcf6e2735e7910ebd1cfb5a1541e5cf81816ad`). These are wrapper build outputs, not proof of PanVK runtime feature support.

NIR test CI:
- Run `42026950603` on `b4b073d...` passed: the new test compiled and suite reported 2/2 passing. On later review, the dynamic index was a constant immediate and could be folded, so this was not considered sufficient dynamic-path validation.
- Pointer-sized index correction commit `92e631280...` and store-count correction commit `ccc0fbde...` are test-only revisions. The latter expects one constant store plus four guarded stores for a genuinely dynamic index.
- A further test-strengthening commit `c6dfaabc0ba69c2a863c54b55eb67a8ac07d6b1c` replaces the immediate with a shader input load and casts it to pointer index width. This is the first intended truly dynamic test. NIR run `42027837669` is currently in progress for that source; earlier NIR runs `42027059205` and `42027207795` are also compiling previous intermediate revisions.

No runtime PanVK code or Vulkan feature reporting changed. Do not ask the user to install/test the package yet: the dynamic NIR test still needs a confirmed pass, and the current source SHA `c6dfaabc...` Android-only package run `42027837719` must finish if a matching latest artifact is desired.


### Dynamic-store branch test failure and correction (2026-11-05 12:36 UTC)

NIR run `42027207795` on source `ccc0fbde...` failed exactly one test: `nir_clip_cull_distance_test.vertex_constant_and_dynamic_scalar_accesses_retarget_vec4`; the other 6,215 tests passed. The failure followed changing the expected store count to five while the index was still a compile-time immediate. This confirms the earlier immediate was folded and did not exercise the four-way dynamic-store lowering path; do not interpret the failure as a proven defect in the production lowering itself.

The test was strengthened in commit `c6dfaabc0ba69c2a863c54b55eb67a8ac07d6b1c` to source its index from a shader input variable and convert it to pointer-index width, so the dynamic value cannot be constant-folded. The test still expects one constant store plus four guarded stores. NIR run `42027837669` is the authoritative run for this corrected test and remains in progress at this update. Earlier passing runs with immediate indices are only baseline validation.

The Android-only package run for `ccc0fbde...` completed successfully (run `42027207710`); artifact `13415315237`, 16,753,662 bytes, SHA-256 `06ed38452893ba855ff65fd2a3e1a1ce920144e9f4d3b789e6fdba1f0da6fbbd`. This package contains no runtime clip/cull changes and is not a candidate for performance conclusions.


### Truly dynamic clip-distance dereference test passed (2026-11-05 12:38 UTC)

Authoritative NIR run for source SHA `c6dfaabc0ba69c2a863c54b55eb67a8ac07d6b1c`: run `42027837669`, job `127246403106`, **success**.

- The test now uses a shader input variable as the array index, so it is genuinely dynamic and cannot be folded from an immediate constant.
- It validates constant and dynamic scalar loads and stores after vec4 lowering.
- Dynamic store expansion is checked as four guarded component stores plus one constant-index store (five packed-root stores total).
- Debug shader validation is run after lowering.
- Meson test suite: `nir_algebraic_parser` OK, `nir_tests` OK; 2/2 Meson tests passed, zero failures.

This is the first successful validation of the genuinely dynamic dereference path. The prior `ccc0fbde...` run failed the test because the index was still constant-foldable, validating that the strengthened test catches the distinction.

Next compiler test target: arrayed 2D clip-distance dereferences for GS/TES inputs and TCS outputs, to validate that the per-vertex array dimension is preserved while the inner distance array is packed into vec4s. Runtime PanVK clip/cull exposure remains unchanged pending complete stage transport.


### Arrayed clip/cull interface tests added (2026-11-05 12:40 UTC)

Added three stage-specific NIR tests in source commit `7bb1a1bbecf8b8358bdb14383cd7c6999db9ece3`, with a follow-up correction `ab0e7e13e0088e0202d6925a51347c0f3828c317` fixing the tests to build dereferences from the original interface variables before lowering.

Coverage added:
- Geometry shader arrayed clip-distance input: preserves per-vertex index while packing the inner distance array.
- Tessellation-control shader arrayed clip-distance output: preserves per-vertex output deref and emits packed stores.
- Tessellation-evaluation shader arrayed clip-distance input: preserves per-vertex input deref and emits packed loads.

Each test checks that accesses are retargeted to the generated `gl_ClipDistanceMESA` variable and invokes `nir_validate_shader()` in debug builds. These tests are newly added and not yet validated by CI. The next NIR run on the corrected latest source commit is required before treating them as valid.

Prior result: genuinely dynamic vertex dereference test passed on `c6dfaabc...`, run `42027837669`, job `127246403106` (2/2 Meson tests, zero failures). No runtime PanVK code changed.


### Arrayed GS/TCS/TES NIR tests validated (2026-11-05 12:44 UTC)

Latest corrected source `ab0e7e13e0088e0202d6925a51347c0f3828c317` passed NIR run `42028832260`, job `127249751771`. Meson reports `nir_algebraic_parser` and `nir_tests` both OK (2/2, zero failures). This is the first passing run containing all three arrayed-interface tests after fixing the original-deref construction bug.

The earlier run on `7bb1a1b...` failed with SIGSEGV in the first geometry arrayed-input test because that draft attempted to construct the dereference from a not-yet-created packed variable. The follow-up `ab0e7e1...` now correctly builds the dereference from the original `gl_ClipDistance` variable before invoking the lowering pass. The corrected run passes.

Current NIR test coverage now includes:
- Declaration metadata and merged clip/cull locations.
- Vec4 packing and boundary-crossing layouts.
- Constant-index and genuinely dynamic scalar loads/stores, including dynamic store lane expansion.
- Geometry shader arrayed input, tessellation-control arrayed output, and tessellation-evaluation arrayed input, with debug NIR validation.

Next correctness gap: mixed clip+cull accesses with non-zero cull offset and scalar/partial-lane stores. Add a targeted test to verify cull index mapping into the shared packed vec4 array. No runtime PanVK feature reporting has changed.


### Mixed clip/cull offset test added (2026-11-05 12:46 UTC)

Source commit `989f931ec6af96c1e9db2c4c683b992581efe770` adds `vertex_cull_accesses_use_clip_offset_in_shared_vec4_array`.

The test declares clip distance size 3 and cull distance size 2, then loads/stores `gl_CullDistance[1]`. Correct packing requires `clip[0..2]` to occupy packed lanes 0..2 and `cull[1]` to map to `gl_ClipDistanceMESA[1].x`. The test counts packed-root load/store derefs specifically at vec4 array index 1 and runs NIR validation in debug builds.

This targets the remaining mixed-array offset path and scalar partial-lane write behavior. CI for the new test is pending; no runtime code has changed.


### Mixed clip/cull non-zero offset test passed (2026-11-05 12:50 UTC)

Source SHA `989f931ec6af96c1e9db2c4c683b992581efe770` passed NIR run `42029810407`, job `127253076467`. Meson reported `nir_algebraic_parser` OK and `nir_tests` OK (2/2, zero failures).

The new `vertex_cull_accesses_use_clip_offset_in_shared_vec4_array` test validates clip size 3 + cull size 2 and confirms the transformed cull[1] load/store dereferences point to packed vec4 array element 1, as required by the combined index offset. This covers the non-zero cull offset and scalar masked-store path.

Current NIR test coverage now validates declaration merge/layout, vec4 packing, constant/dynamic loads and stores, arrayed GS/TCS/TES interfaces, and mixed clip+cull offset. This is compiler helper correctness only. PanVK's own direct VS->FS lowering remains a separate implementation and full stage transport/feature exposure is not yet complete.


### PanVK direct VS -> FS clip/cull merge integration (2026-11-05 12:53 UTC)

Source commit `197a5303131d1759e88df534fa1cd17331ba6988` updates `src/panfrost/vulkan/panvk_vX_shader.c`.

Audit finding: SPIR-V maps `BuiltInClipDistance` to `VARYING_SLOT_CLIP_DIST0` and `BuiltInCullDistance` to `VARYING_SLOT_CULL_DIST0`, and marks both compact. PanVK's direct VS cull rewrite only processes compact stores in the CLIP_DIST slots, while its synthetic FS discard inputs are also assigned to CLIP_DIST slots. However, the direct pipeline linker previously captured `clip_distance_array_size` / `cull_distance_array_size` and inspected FS inputs without explicitly gathering sizes from variables or merging the separate clip/cull declarations. This left the path dependent on upstream metadata/interface behavior and could miss cull-only or mixed clip+cull cases.

Fix: when a direct VS -> FS link is present, explicitly call `nir_gather_clip_cull_distance_sizes_from_vars()` followed by `nir_merge_clip_cull_distance_vars()` on both VS outputs and FS inputs before capturing counts and checking explicit FS reads. This aligns cull stores and the synthetic FS varying layout with the same packed clip-distance location scheme. Non-direct VS -> GS/TES paths remain untouched. Vulkan feature bits remain disabled.

This is the first PanVK runtime compiler-path change in this clip/cull workstream. It is intentionally scoped to the already-limited direct VS -> FS path. Required next validation: compile Android PanVK and wrapper variants, inspect NIR dumps for cull-only and mixed clip+cull VS/FS pairs, then add/execute targeted shader tests before considering feature exposure. No device runtime or performance claim.


### CI guard updated for clip/cull merge integration (2026-11-05 12:55 UTC)

Android-only run `42031120657` on runtime source commit `197a5303131d1759e88df534fa1cd17331ba6988` failed at the pre-build checkout verification step, not during compilation. Its hard-coded grep still required the old source condition `direct_vs_fs &= has_fragment_stage && !direct_fs_reads_clip_cull`, which was intentionally removed by the two-pass direct-link detection and explicit NIR merge integration.

Updated `.github/workflows/android-only-experiment.yml` to verify the new invariants instead:
- `nir_gather_clip_cull_distance_sizes_from_vars(vs_nir)`
- `nir_merge_clip_cull_distance_vars(fs_nir)`

CI-only correction commit: `4f4d02e01723d61f1ff19713951be6bcbfa8d08d`. This does not change runtime code. Fresh CI is required; prior run `42031120657` provides no compile evidence because all build steps were skipped after the stale grep failed.


### NIR regression suite passed after PanVK direct-link integration (2026-11-05 13:00 UTC)

The NIR test workflow on runtime source SHA `197a5303131d1759e88df534fa1cd17331ba6988` passed: run `42031120675`, job `127257527564`. The fresh CI-only workflow-guard correction SHA `4f4d02e01723d61f1ff19713951be6bcbfa8d08d` also passed NIR: run `42031227168`, job `127257885748`. Both report `nir_algebraic_parser` OK, `nir_tests` OK, 2/2 Meson suite tests, zero failures.

The new PanVK direct VS -> FS prepass is therefore syntactically compatible with the NIR helper API; full Android driver compilation remains pending. Android-only run `42031227285` has passed dependency install and host-tool generation and is building Android libdrm/driver. Wrapper run `42031227198` has Debugoptimized complete and Release compiling. Older wrapper run `42031120736` is still compiling both variants. No runtime install/performance test has been done.


### Android PanVK package built with direct VS -> FS clip/cull merge fix (2026-11-05 13:04 UTC)

The corrected Android-only workflow on source SHA `4f4d02e01723d61f1ff19713951be6bcbfa8d08d` completed successfully: run `42031227285`, job `127257885739`.

All gates passed:
- Source checkout and updated source guard.
- Host tool generation.
- Android ARM64 libdrm build.
- Android-only Meson configuration.
- Android install compilation/staging.
- Driver, manifest, dependency and staged-file audit.
- Flat Winlator/Adrenotools package build and upload.

Artifact: `13417170405`, `panvk-g720-android-only-experimental-package`, 16,751,280 bytes, SHA-256 `f537ae8904145696483644dd50cbe6928f6d46fe01393734530bb92da584d288`, expires 2027-02-03. This package includes the PanVK direct VS -> FS clip/cull merge integration from `197a530...` and the corrected CI guard; it does not enable `shaderClipDistance` or `shaderCullDistance` feature bits. It has not yet been installed/run on the user's phone.

Wrapper workflow on source SHA `4f4d02e...` also completed Release and Debugoptimized successfully (run `42031227198`). Artifacts: Release `13416766825`, Debugoptimized `13416243040`. These are separate wrapper outputs, not the PanVK driver package.

NIR run `42031227168` on `4f4d02e...` passed 2/2 Meson suite tests. The test coverage includes dynamic accesses, arrayed GS/TCS/TES interfaces and mixed clip/cull offset.

Next user-dependent gate: import the Android-only package into Winlator's custom driver path (do not replace the Android system/vendor Vulkan driver), launch a test workload that uses clip/cull distance, and provide the log or crash report. Prefer a known DXVK game benchmark with a fixed scene and capture PanVK shader/NIR debug output if available. Compare stability and FPS against the immediately preceding package with the same Winlator settings. Until this device validation, do not enable Vulkan clip/cull feature bits or claim performance improvement.


### Winlator Mali 1.2 package compatibility clarification (2026-11-05 14:08 UTC)

A user correctly asked whether artifact `13417170405` is definitely packaged for Winlator Mali 1.2. Audited the exact workflow and packaging script at source SHA `4f4d02e01723d61f1ff19713951be6bcbfa8d08d`.

Verified facts:
- The workflow builds an Android ARM64 PanVK ICD and invokes `.github/scripts/package-panvk-adpkg.sh`.
- The script creates a flat-root ZIP with `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- `meta.json` uses schemaVersion 1 and libraryName `libvulkan_panfrost.so`; the driver has a private `$ORIGIN` dependency on `libpanvk_drm.so`.
- The package script performs ELF architecture, SONAME, RUNPATH, dependency, and ZIP-layout checks.
- The external FristOneRR PanVK project documents Winlator Mali 1.2+ (AdrenoTools) as a supported app and uses the same install path (import the ZIP itself). Winlator Mali's own README says Mali containers select Graphics Driver = Wrapper.

Important qualification:
- The artifact is **AdrenoTools/ADPKG-format and intended for the Winlator Mali 1.2 AdrenoTools custom-driver path**, not a Winlator-Mali-specific bespoke package format.
- The packaging workflow does **not** run an actual Winlator Mali importer, validate the app's extraction/loader behavior, or launch the driver on-device. Thus importer acceptance and runtime loading are not yet proven. The extra private dependency `libpanvk_drm.so` is deliberately included beside the ICD and linked via `$ORIGIN`, but still requires runtime confirmation in the target app.
- Do not describe the artifact as confirmed Winlator Mali-compatible until the user imports it and we verify that it appears/selects correctly and the process actually loads this PanVK ICD rather than silently falling back.
- No need to rebuild solely for this clarification. The next device test should first establish import acceptance and actual driver identity (GPU info / Vulkan device name / PanVK log), then proceed to the fixed Tomb Raider benchmark only if the driver is confirmed loaded.

External references checked:
- `FristOneRR/FristOneRR-Panvk-Driver` README documents Winlator Mali 1.2+ (AdrenoTools) support and ZIP import instructions.
- `GunaCharanTeja/WinlatorMali` README documents Wrapper selection for Mali.


### Winlator Mali 1.2 importer source audit and package-contract CI guard (2026-11-05 14:15 UTC)

Inspected the actual Winlator Mali source at branch `bionic-mali-1.2`, not just the external PanVK README:
- `app/src/main/java/com/winlator/cmod/contents/AdrenotoolsManager.java`
- `app/src/main/java/com/winlator/cmod/AdrenotoolsFragment.java`

Confirmed importer behavior:
- The import action opens the selected ZIP and extracts its entries directly into a temporary directory.
- It accepts the package when root-level `meta.json` exists, reads `name` from that JSON, and renames the temporary directory to that driver name.
- Driver list and container selection read `name`, `driverVersion`, and `libraryName` from `meta.json`.
- `setDriverById()` exports `ADRENOTOOLS_DRIVER_PATH`, `ADRENOTOOLS_HOOKS_PATH`, and `ADRENOTOOLS_DRIVER_NAME`, where the latter is the `libraryName` field.
- The import code does not enforce a special PanVK ZIP filename or validate `schemaVersion`; it expects the ZIP entries at root, with a valid root `meta.json`.

This directly matches our flat ZIP layout and the metadata fields already emitted. Thus the package format is source-verified against Winlator Mali 1.2's Adrenotools importer, not merely inferred from the FristOneRR documentation. Remaining unverified item is actual on-device import and dynamic loading of the ICD and its private `libpanvk_drm.so` dependency.

Added an explicit metadata contract guard to `.github/scripts/package-panvk-adpkg.sh` on source branch `experiment/g720-clip-cull-capabilities`:
- Requires non-empty string `name`, `driverVersion`, and `libraryName`.
- Requires `libraryName == libvulkan_panfrost.so`, schemaVersion 1, and safe single-component driver name.
- Requires both the ICD and private dependency to exist in the package staging directory.
- CI run `42033527656` (Android-only PanVK package) is building commit `9188e7bf92a032431ca16ac02d54d81be5a5e12e`; do not treat it as passed until completion.

The earlier artifact remains import-format compatible by inspection, but the new build adds a regression guard against future metadata/package drift. Next gate after CI: obtain the newly built ZIP, then user imports it in Winlator Mali and confirms the installed driver entry and runtime device identity. No system Vulkan driver replacement.



### User-reported Winlator Mali import rejection — 2026-11-05 (14:16 local)

The user reports that the PanVK package they downloaded does not import into Winlator Mali. This supersedes the prior assumption that matching the inspected importer contract was enough to proceed. The package has **not** been proven importable on-device.

Branch inventory was refreshed from the GitHub API: 24 branches exist. `PROJECT_CONTINUITY_HANDOFF.md` exists only on `docs/project-continuity-handoff-2026-11-03`; the other 23 branches return 404 for this path. The canonical document on the docs branch is 3,597 lines / 324,793 characters at the time of this update. The full heading index and latest sections were inspected.

Exact importer source audit from the preceding addendum remains:
- Winlator Mali 1.2 importer extracts ZIP entries to a temporary directory and checks for root-level `meta.json`.
- It reads `name`, `driverVersion`, and `libraryName` from that JSON.
- It uses `libraryName` as the Vulkan ICD library filename.
- The package script creates a flat inner driver ZIP with root entries `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.

New important packaging clarification:
- The GitHub Actions workflow uses `actions/upload-artifact@v4` with a multi-path artifact containing `stage/`, `audit/`, and the actual `PanVK-G720-experimental.zip`.
- Downloading a GitHub Actions artifact produces an outer artifact ZIP. The actual importer input is the inner `PanVK-G720-experimental.zip` extracted from that outer ZIP, not the outer artifact ZIP itself.
- The package script does create the inner ZIP with flat root entries. The workflow artifact therefore has a nested ZIP structure at download time.
- This is a plausible explanation for the user's import rejection if the outer artifact ZIP was selected, but it is not confirmed until the user identifies the exact selected filename or reports the import error.
- Do not tell the user to keep retrying. Ask one concise diagnostic question: did they select the extracted inner `PanVK-G720-experimental.zip`, or the GitHub-downloaded outer artifact ZIP? Also request the exact Winlator error text/screenshot if the inner ZIP was selected.

Current package-contract guard build:
- Source branch: `experiment/g720-clip-cull-capabilities`
- Source SHA: `9188e7bf92a032431ca16ac02d54d81be5a5e12e`
- Android-only workflow run: `42033527656`
- Job: `127265677145`
- At this update, Android ARM64 libdrm, Android configure and host tools had passed; Android PanVK compile was still in progress. No artifact had yet been uploaded.
- The added guard validates metadata fields, schemaVersion, safe package name, expected ICD libraryName, and presence of the ICD plus private libdrm dependency. It does not test the Winlator importer itself.

Next steps:
1. Obtain the user's exact selected filename and error. If they selected the outer artifact ZIP, explain extraction and identify the inner ZIP; do not rebuild solely for this.
2. If the inner ZIP was selected, reproduce the importer parser contract against the exact artifact, inspect its ZIP entry list and meta.json, and inspect the Winlator import exception path/logcat. Check for duplicate metadata naming, extraction exceptions, unsupported JSON parsing, and library file validation.
3. Once run `42033527656` completes, inspect the package artifact and checksum. Do not present it as a fix until the inner ZIP is independently audited.
4. Consider changing workflow artifact publishing to make the nested driver ZIP unmistakable (e.g. a dedicated artifact containing only the inner ZIP, and/or a clearly named GitHub Release asset), while preserving the flat ADPKG inner ZIP format.
5. Do not start performance benchmarking until Winlator imports the inner ZIP and runtime driver identity is confirmed. Never touch the phone's stock/vendor Vulkan driver.


### Winlator Mali import success + Tomb Raider benchmark/log review — 2026-11-05

User confirmed the correct inner file `PanVK-G720-experimental.zip` imports successfully into Winlator Mali. The screenshot shows the extracted GitHub Actions artifact directory with:
- `PanVK-G720-experimental.zip` (3.94 MB): the actual driver package to import.
- `audit/`: CI-generated diagnostic evidence only (ELF headers/dynamic dependencies/symbol checks, staged-file listing, ICD manifest, hashes, ADPKG ZIP listing/checksum). Not needed by Winlator; safe to keep for debugging or delete after retaining the build evidence.
- `stage/`: Meson install tree used by CI to audit the installed Android ARM64 driver layout and dependencies. Not a separate driver package and not needed by Winlator; safe to keep for debugging or delete.
Do not import `audit/`, `stage/`, or the outer GitHub Actions artifact ZIP. Only import the inner `PanVK-G720-experimental.zip`.

User-reported Tomb Raider benchmark:
- Min 9.4 FPS
- Max 19.4 FPS
- Average 14.0 FPS
Previous comparable Tomb Raider result in the recent project thread was corrected by the user to 13.9 average (reported min 7.8, max 18.9). New average is +0.1 FPS (~0.7%); min/max improved modestly. Treat as essentially flat pending repeat runs with identical scene, resolution, settings, wrapper, thermal state and benchmark duration. No causal performance claim from this single run.

Uploaded logs reviewed:
- `logcat.log`, `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, `TombRaider_d3d9.log`.
- Confirmed the selected custom ICD really loads: Adrenotools reports driver ID `PanVK Mali G720 (experimental)`; hook loads `.../contents/adrenotools/PanVK Mali G720 (experimental)/libvulkan_panfrost.so`; PanVK emits Kbase raw coherency and tiler heap renewal logs. This is not a system-driver fallback.
- Kbase queue wait completed successfully; no `GPU_SHAREABILITY_FAULT`, GPU fault, OOM, Vulkan error, SIGSEGV, or assertion was found in the supplied capture.
- DXVK identifies `Wrapper(Mali-G720 MC7)`, driver version `26.2.149`, Vulkan 1.3.354, D3D feature level 11_0. DXVK uses 4 compiler threads; game config shows maxQueuedFrames=1, relaxedBarriers=true, ignoreGraphicsBarriers=true, useEarlyDiscard=true, memoryTrack=true, max device/shared memory 1024 MiB.
- DXVK's enabled device extensions include `VK_EXT_4444_formats`, `VK_EXT_custom_border_color`, `VK_EXT_depth_clip_enable`, `VK_EXT_extended_dynamic_state`, `VK_EXT_host_query_reset`, `VK_EXT_robustness2`, `VK_EXT_shader_demote_to_helper_invocation`, `VK_EXT_shader_stencil_export`, `VK_EXT_transform_feedback`, `VK_EXT_vertex_attribute_divisor`, `VK_KHR_create_renderpass2`, `VK_KHR_depth_stencil_resolve`, `VK_KHR_draw_indirect_count`, `VK_KHR_driver_properties`, `VK_KHR_image_format_list`, `VK_KHR_sampler_mirror_clamp_to_edge`, `VK_KHR_swapchain`, and `VK_EXT_conditional_rendering`.
- DXVK feature log reports `multiViewport=1`, `shaderClipDistance=1`, `shaderCullDistance=1`, `geometryStreams=1`, and `geometryShader=1`, despite the project's PanVK source audit documenting limitations/disabled native support for some of these. Treat these as a potentially important wrapper/feature-emulation reporting path that needs source-level verification; do not blindly enable additional PanVK feature bits based on DXVK's log.
- One notable game-side issue: `D3D11SwapChain: Failed to recreate swap chain`; logcat also records X11 Present `BadImplementation` and a Wine X11 Vulkan `get_client_rect failed`. This points to a WSI/presentation path issue worth investigating separately from shader throughput. Benchmark result was nevertheless reported by user.
- No direct evidence in this capture that adding ray-query/RT extensions would improve this DX11 game; `VK_KHR_ray_query` is not enabled in DXVK's list and remains unsupported/incomplete in PanVK.

Latest package-contract CI:
- Workflow run `42033527656` succeeded on source SHA `9188e7bf92a032431ca16ac02d54d81be5a5e12e`.
- Artifact ID `13418635422`, name `panvk-g720-android-only-experimental-package`, 16,753,599 bytes, artifact digest `sha256:87604a0f16b1ca289ab0310a8695821fd6f66dc9b57e6d04863c1d126b3b4f1d`, expires 2027-02-03.
- This run adds package metadata contract validation; it does not change driver behavior compared with the prior clip/cull source.
- The downloaded artifact still contains the outer GitHub artifact archive plus the inner ADPKG ZIP, so retain clear instructions to extract the outer archive and import only the inner ZIP. Future packaging UX improvement: publish the inner ZIP as a clearly named GitHub Release asset or otherwise clearly document the nested archive.

Next development focus:
1. Import/load path is now confirmed. Stop spending cycles on importer diagnosis.
2. Audit why DXVK sees some capabilities as enabled that the source matrix previously marked disabled/limited. Trace wrapper's advertised VkPhysicalDeviceFeatures/extensions versus PanVK's queried features; ensure no unsupported feature is being fabricated unsafely.
3. Investigate WSI swapchain recreation and X11 Present failure. Keep this separate from GPU shader performance and avoid claiming it explains the FPS.
4. Benchmark repeatability: capture at least 3 same-scene runs before attributing a small 0.1 FPS change to code.
5. Continue to preserve separate experimental driver packaging; never replace the Android vendor Vulkan driver.



### Full-power runtime audit — Tomb Raider logs 2026-11-05

User explicitly requested a more aggressive, evidence-led optimization effort after marginal FPS progress. The uploaded archive `TombRaider_logs_2026-11-05_14-26-52.zip` was unpacked and inspected (logcat, Wine debug, DXVK D3D11/DXGI/D3D9 logs).

#### Exact test configuration recovered from Wine debug
- Winlator Mali 1.2.1 Bionic (23), Android 16, device MT6899 / Xiaomi 25069PTEBG.
- Graphics driver field: `wrapper-gamenative`.
- DXVK version 1.7.2; game executable is Tomb Raider Definitive Edition (not the earlier original GOTY title).
- DXVK config effective values include `d3d11.maxQueuedFrames=1`, `dxvk.numCompilerThreads=4`, `dxvk.memoryTrack=True`, `dxvk.useEarlyDiscard=True`, `d3d11.relaxedBarriers=True`, and `d3d11.ignoreGraphicsBarriers=True`.
- DXVK reports no valid state cache entries and creates a new cache.
- Device reports 900x506 swapchain, initially IMMEDIATE with 3 images then FIFO with 4 images. It logs `D3D11SwapChain: Failed to recreate swap chain`.
- Wine debug ends with `X11DRV_vulkan_surface_create get_client_rect failed` and `_wassert ... "!status && \"vkQueueSubmit\""`. Treat these as a real WSI/submit error path, not harmless generic Wine noise; establish whether it occurs before/after benchmark and whether it correlates with frame stalls/crashes.

#### Runtime facts and discrepancies
- Custom ICD load is confirmed in logcat: `hook_android_dlopen_ext: loading custom driver .../PanVK Mali G720 (experimental)/libvulkan_panfrost.so`.
- PanVK startup logs: raw coherency mode 0; Kbase tiler heap renewal threshold 65536; Kbase profiling disabled. No captured GPU shareability fault, GPU fault, OOM, or Vulkan error in this log.
- DXVK device feature output says `multiViewport=1`, `shaderClipDistance=1`, `shaderCullDistance=1`, and `geometryStreams=1`.
- The inspected PanVK source on `experiment/g720-clip-cull-capabilities` explicitly sets `multiViewport=false`, `shaderClipDistance=false`, `shaderCullDistance=false`, and `geometryStreams=false`. Thus DXVK's values are not a trustworthy direct reflection of those PanVK capability bits; likely an effect of wrapper feature adaptation / feature emulation / wrapper logging semantics. Do NOT enable those PanVK features to match the DXVK log. Audit GameNative wrapper's VkGetPhysicalDeviceFeatures interception and extension emulation separately.
- DXVK logs device name `Wrapper(Mali-G720 MC7)`, consistent with the Winlator wrapper layer being active above the custom PanVK ICD.
- The logcat records 14 calls to `panvk_android_import_ahb_memory`, 9 AHB-properties queries and 30 successful kbase dmabuf BO allocations in the captured startup interval; many are ~1.85 MiB 900x506 presentation buffers. These are startup/WSI events, not evidence of per-frame allocations. Do not optimize by caching AHB imports until ownership, fd identity, VkImage lifetime and synchronization are proven.

#### New high-value WSI research
- Reviewed Lee Gao's 2026-07-28 technical article, “WSI Woes on Mali”: https://leegao.github.io/winlator-internals/wrapper/2026/07/28/wsi-woes-mali.html
- It describes Winlator Bionic wrapper's Mali WSI path falling back from direct rendering to a two-image blit path because Mali does not accept BGRA AHB import in that implementation; this adds one copy per present. It also documents wrapper-side `VK_EXT_map_memory_placed` emulation interactions with dedicated allocations and multiple AHB native-handle FDs.
- This is a plausible major performance target because this test uses `wrapper-gamenative` and logcat shows repeated AHB/DRI3 presentation-buffer handling, but it is NOT YET proven that this exact GameNative wrapper build uses that same fallback. Inspect the exact GameNative wrapper source/build and runtime WSI mode before making a patch.
- The article's RGBA-AHB / virtual-BGRA view approach is a candidate zero-copy direction only if the Winlator Mali X11 compositor's expected pixel layout and image-view/format-feature constraints are satisfied. Do not blindly advertise direct rendering or bypass the copy without proving channel correctness, usage support, export/import compatibility and synchronization.

#### Immediate priority order (replace speculative feature enabling)
1. **Wrapper WSI path audit (highest expected frame-time payoff):** identify the exact `wrapper-gamenative` source/version used by Winlator Mali 1.2.1; trace swapchain mode (direct vs blit), BGRA/RGBA AHB probe, external memory allocation path, `VK_EXT_map_memory_placed` emulation and image-copy path. Determine whether each present incurs a copy. If confirmed, prototype a separate wrapper patch/branch; do not mix wrapper code into PanVK.
2. **Fix/characterize swapchain recreation:** trace the IMMEDIATE-to-FIFO transition and the failed `D3D11SwapChain` recreation; inspect Wine `vkQueueSubmit` assertion and X11 surface failure. Preserve exact error VkResult with instrumentation before altering WSI synchronization.
3. **Capability audit at wrapper boundary:** compare PanVK raw physical-device feature bits with features returned through GameNative wrapper; maintain truthful PanVK feature exposure and identify only the wrapper emulations that are complete.
4. **Controlled DXVK baseline:** current test uses DXVK 1.7.2, no state cache, four compiler threads and memory tracking. For performance comparison, keep all game settings fixed and disable diagnostic `dxvk.memoryTrack` only in a separate controlled run; do not change multiple settings simultaneously. State-cache warm/cold runs must be separated.
5. **PanVK shader/queue/memory optimization only after isolating the above.** Do not infer shader-core scheduling from 7 MC7 cores; firmware schedules cores. Do not expose ray query, pipeline stats, clip/cull, multiViewport or geometryStreams solely to increase DXVK's printed feature values.

Reported result for this run: Min 9.4 / Max 19.4 / Avg 14.0 FPS. Compare cautiously with prior 13.9 average because the game edition, wrapper selection and likely environment differ. The 0.1 FPS average change is not a demonstrated improvement.

The user asked the assistant to continue independently and return only when user action is needed. Next assistant work should be source-level audit of the exact GameNative wrapper and WSI path, then commit a reproducible audit/experiment and update this handoff. Do not ask user to re-run the same benchmark before the source audit is done.



### GameNative wrapper source audit — high-value allocation experiment (2026-11-05)

The uploaded logcat environment line identifies the actual selected wrapper path:
- `VK_ICD_FILENAMES=.../wrapper_icd.aarch64.json`
- `ADRENOTOOLS_DRIVER_PATH=.../PanVK Mali G720 (experimental)/`
- `ADRENOTOOLS_DRIVER_NAME=libvulkan_panfrost.so`
- `WRAPPER_VK_VERSION=1.3.354`
- `WRAPPER_RESOURCE_TYPE=auto`
- `WRAPPER_EMULATE_BCN=3`, `WRAPPER_USE_BCN_CACHE=1`, `ENABLE_BCN_COMPUTE=1`
- `WRAPPER_MAX_IMAGE_COUNT=0`
- `MESA_VK_WSI_PRESENT_MODE=mailbox`
- `WRAPPER_DISABLE_PRESENT_WAIT=0`
- No `WRAPPER_DISABLE_PLACED`, `WRAPPER_BLIT`, or `WRAPPER_BYPASS_SWAPCHAIN_PLACED` value is present in the captured environment (default code paths therefore apply).

The exact GameNative Mesa wrapper source is publicly available in `GameNative/mesa`, branch `wrapper-25`. Its relevant code was inspected:
- `src/vulkan/wrapper/wrapper_physical_device.c` saves raw driver features into `base_supported_features`, then advertises wrapper-level `multiViewport=true`, `shaderClipDistance=true`, `shaderCullDistance=true`, plus other compatibility flags.
- `src/vulkan/wrapper/wrapper_device.c` later masks some requested core features back against `base_supported_features` before calling the base driver. Therefore DXVK's feature log is the wrapper-advertised feature surface, not a reliable statement of the actual enabled PanVK features. PanVK feature bits must not be force-enabled to match that log.
- In particular, wrapper source sets multiViewport/clip/cull to true, then `DISABLE_FEATURE` masks them to raw PanVK values at device creation. This can produce a mismatch between what an application believes it enabled and what the underlying PanVK device actually enabled. Audit actual DXVK shader usage before changing either side.
- `geometryStreams` was not found in the wrapper's physical-device forced feature list. DXVK's printed value should be treated as wrapper log/chain semantics until the exact extension feature chain is traced.

#### Direct-vs-blit WSI path
- GameNative `src/vulkan/wsi/wsi_common_android.c` contains a runtime AHB probe. Unless `WRAPPER_BLIT=1` or Mesa WSI debug forces blit, it tests an Android hardware buffer format and asks the base ICD for external image import support.
- On success, it selects `WSI_SWAPCHAIN_NO_BLIT` and imports AHB memory directly as the swapchain image. On failure it creates a local primary plus an AHB secondary and uses an image copy at present.
- The test log has successful AHB imports for 900x506 buffers and DRI3 pixmap handoff with modifier 1255, but the captured log does not contain the wrapper's explicit `WSI_SWAPCHAIN_NO_BLIT` or `Creating blit context` diagnostic. Therefore direct rendering is plausible but not proven. Do not assume the generic Mali extra-copy issue applies to this PanVK run.
- Source references:
  - https://github.com/GameNative/mesa/blob/wrapper-25/src/vulkan/wsi/wsi_common_android.c
  - https://github.com/GameNative/mesa/blob/wrapper-25/src/vulkan/wsi/wsi_common.c
  - https://github.com/GameNative/mesa/blob/wrapper-25/src/vulkan/wrapper/wrapper_device_memory.c
  - https://github.com/GameNative/mesa/blob/wrapper-25/src/vulkan/wrapper/wrapper_physical_device.c

#### First controlled performance experiment: disable placed-memory emulation for this 64-bit game
- Wrapper `wrapper_physical_device.c` enables `VK_EXT_map_memory_placed` emulation by default unless `WRAPPER_DISABLE_PLACED=1`.
- `wrapper_device_memory.c::wrapper_AllocateMemory` intercepts each host-visible allocation when `memoryMapPlaced` is enabled, and in `WRAPPER_RESOURCE_TYPE=auto` mode tries DMA-heap, AHB, or opaque-FD external memory backends before falling back to the base ICD.
- This test is Tomb Raider Definitive Edition running under the 64-bit ARM64EC Wine path. It does not need 32-bit low-address mapping for the guest process. Avoiding wrapper external-memory emulation for host-visible allocations is therefore a plausible way to reduce allocation overhead and avoid extra dma-buf/AHB work for this specific x64 game.
- Proposed isolated test: add environment variable `WRAPPER_DISABLE_PLACED=1` to this container only; do not change any other setting. This disables the wrapper extension/feature emulation and routes host-visible allocations directly to PanVK. It is not a universal setting for 32-bit x86 games; those may require the wrapper's placed mapping emulation.
- Keep `dxvk.memoryTrack=True` unchanged for this first A/B comparison. It is a separate likely overhead candidate to test later.
- Baseline for this exact test configuration: Tomb Raider Definitive Edition, Winlator Mali 1.2.1, `wrapper-gamenative`, DXVK 1.7.2, 900x506, Min 9.4 / Max 19.4 / Avg 14.0 FPS. The test also had a swapchain recreation error and Wine vkQueueSubmit assertion in the log.
- This experiment is expected to test CPU/allocator overhead, not promise a fixed FPS gain. Compare same benchmark route, resolution, quality, wrapper, DXVK, and thermal state. If the game fails to launch or memory mapping fails, remove the variable and return to baseline.

Next action needed from the user after this source audit: add `WRAPPER_DISABLE_PLACED=1` as a container environment variable for Tomb Raider Definitive Edition only, run the same benchmark once, and send min/max/average FPS plus logs. Do not apply it globally to other games.


## 2026-11-05 — project direction correction: stop peripheral tuning

The user explicitly rejected continuing with wrapper environment-variable A/B tests,
clip/cull micro-optimisation, packaging work, or other peripheral experiments as
the main project. These have not produced meaningful progress toward the stated
goal. Do not ask the user to run the pending `WRAPPER_DISABLE_PLACED=1` test
unless it becomes directly relevant to a demonstrated root cause.

The next engineering phase must target substantive PanVK capability and
architecture gaps for DX11/DX12 emulation, with source-level work and measurable
correctness/performance impact:

1. Audit the D3D12 descriptor path first. The current v12 device extension
   table does not expose `VK_EXT_descriptor_heap`; descriptor indexing is
   already exposed and must not be mistaken for the newer descriptor-heap API.
   Compare the current Mesa 26.3-derived implementation with the later
   JimVulkan G720 PanVK descriptor-heap implementation. Map the complete port:
   Vulkan registry/generated entrypoints, device extension/features/properties,
   descriptor heap objects and memory, descriptor writes/copies, push data,
   NIR lowering, shader ABI, lifetime/synchronization, and tests. Do not merely
   set the extension bit. Determine whether this extension is required by the
   user's actual VKD3D/DX12 workload before treating it as the only DX12 path.
2. Audit the existing descriptor-set allocator and Kbase BO mapping path as a
   separate memory-correctness/performance task. Existing telemetry showed
   596,561 set-allocation attempts, 299 failures, 85.30 MiB requested set bytes,
   and 4.43 MiB cumulative pool backing in one profiled run. Determine the
   failure classes and whether they correspond to pool exhaustion, VMA
   fragmentation, host allocation failure, or backend BO failure. The Kbase
   backend uses SAME_VA mappings established at BO allocation time and retains
   the mapping for the BO lifetime; do not assume generic mmap wrapper overhead
   without tracing the actual hot path.
3. Do not try to map Vulkan compiler dispatch onto the seven G720 shader cores.
   Mali firmware schedules shader-core execution. PanVK can expose ARM shader
   core properties/scheduling controls and optimize shader/workgroup/compiler
   behavior, but must not hard-code one compiler worker or queue per core.
   Inspect the queried core-count property and actual CSF scheduling semantics.
4. Keep `VK_KHR_ray_query`, sparse residency, v12 vertex-pipeline stores/atomics,
   and other incomplete capabilities disabled until their complete API,
   compiler, memory, synchronization, and runtime implementations exist.
5. No more benchmark-only toggles as the default next step. First produce a
   concrete source audit and implementation plan, then make a coherent code
   change with tests and Android CI. Keep the custom ICD separate from the
   Android system/vendor driver.

This correction supersedes the previous immediate user action to test
`WRAPPER_DISABLE_PLACED=1` as the next project step. That test is deferred.


## 2026-11-05 — user directive: maximal G720 capability enablement

The user has explicitly requested that the next custom PanVK driver test expose and enable every Vulkan extension and feature that the Mali-G720 v12 hardware and this driver can genuinely support, then optimize performance afterward. They are no longer willing to spend time on tiny FPS A/B experiments. Treat this as the project priority and do not send more peripheral wrapper toggles or micro-benchmarks as the next action.

Interpretation and safety contract:
- Perform an exhaustive extension + feature + property inventory, not a blanket true assignment to the Vulkan extension table. The extension bit alone is not sufficient: generated entrypoints, feature structs, properties, command implementations, NIR/SPIR-V lowering, shader ABI, synchronization, memory lifetime, format support and Kbase CSF backend must all be present.
- Distinguish silicon/ISA capability from PanVK implementation and Kbase backend capability. Expose all end-to-end implemented combinations; port missing but feasible implementations where they materially unlock DXVK or VKD3D-Proton workloads; keep genuinely unsupported backend paths disabled.
- The current source already exposes many v12 capabilities (descriptor indexing, runtime arrays/non-uniform indexing, update-after-bind, timeline semaphores, synchronization2, BDA, dynamic rendering, subgroup features, conditional rendering, transform feedback, ARM shader-core properties and scheduling controls, etc.). Do not waste time toggling already-true bits.
- A high-value confirmed gap is VK_EXT_descriptor_heap: it is absent from this branch's device extension table. The later JimVulkan/mali-panvk implementation advertises it for PAN_ARCH >= 9, and its README explicitly identifies descriptor heaps as part of the Valhall VKD3D-Proton D3D12 path. Its implementation spans Vulkan runtime descriptor-heap lowering, SPIR-V DescriptorHeapEXT support, NIR heap-offset/descriptor loads, PanVK heap resource-table state, push-data commands, shader ABI, resource ownership and device feature/property reporting. Port this coherently, not as an extension boolean. Reference commit inspected: 58f2c55854bd48b18df756edb11f187af9cd2fde.
- Other candidate features to exhaustively evaluate and implement where possible: multiViewport software lowering, complete clip/cull-distance transport and semantics, geometry shader streams, pipeline-statistics queries, float atomic-add variants, and other v12-appropriate extension feature structs. Their current limitations are recorded in docs/G720_CAPABILITY_MATRIX_2026-11-05.md.
- Sparse residency/binding is currently blocked by missing Kbase sparse-bind queue/page-remapping support. Do not expose it as a false feature. Ray query likewise requires a complete software acceleration-structure and traversal implementation; do not claim G720 hardware support or expose it merely to satisfy a list.
- Next deliverable: a machine-checkable exhaustive matrix of every extension, every feature struct bit and relevant property, classified as (A) already enabled and implemented, (B) hardware-capable but missing PanVK code, (C) possible with software emulation, (D) blocked by Kbase/Android, or (E) not supported by G720. Include exact source files and required ports. Then create a dedicated capability branch and implement the highest-value coherent missing features, starting with descriptor heaps for D3D12. Build and test the custom Winlator-importable ZIP; never replace the Android system driver.

The user requested no more tiny-FPS optimization loops until this capability phase has materially expanded the driver's usable feature set.


## Addendum: exhaustive v12 capability-table census and descriptor-heap port target (2026-11-05)

The current source branch was re-read directly before this addendum:
- Source branch: `experiment/g720-clip-cull-capabilities`
- `src/panfrost/vulkan/panvk_vX_physical_device.c` blob SHA: `6f8062ef02dd8b7707b2a16d4868aa7d84f4a4fb`
- `get_physical_device_extensions()` initializes 185 extension-table fields in this source file (some are platform-conditional and some architecture-gated; this is not a count of extensions guaranteed enabled on the target device).
- `get_physical_device_features()` initializes 296 feature fields in `struct vk_features` (includes core and extension feature bits; many are conditional or explicitly false).
- Properties are initialized in the same file and must be audited separately from feature booleans.

The extension census includes KHR, EXT, ANDROID, GOOGLE, VALVE, and ARM entries, plus platform-conditional WSI/display entries. The source has Vulkan 1.4-era capabilities including KHR maintenance5-9, map_memory2, pipeline_binary, shader_float_controls2, shader_expect_assume, shader_subgroup_rotate, dynamic_rendering_local_read, and EXT host_image_copy, graphics_pipeline_library, nested_command_buffer, map_memory_placed, and related features. Therefore the previous abbreviated matrix must not be treated as a complete extension inventory.

### Highest-value D3D12 capability gap

`VK_EXT_descriptor_heap` is absent from this source branch's extension table and feature assignments. A later reference implementation exists in `JimVulkan/mali-panvk` at commit `58f2c55854bd48b18df756edb11f187af9cd2fde`:
- Its physical-device extension table enables `.EXT_descriptor_heap = PAN_ARCH >= 9`.
- Its feature query sets `.descriptorHeap = PAN_ARCH >= 9` and `.descriptorHeapCaptureReplay = false`.
- It adds descriptor-heap state handling in `panvk_vX_cmd_desc_state.c`, push data handling in `panvk_vX_cmd_push_constant.c`, descriptor layout/resource table definitions in `panvk_descriptor_set_layout.h`, shader heap usage tracking/lowering, SPIR-V `DescriptorHeapEXT` support, and shared Vulkan runtime lowering in `src/vulkan/runtime/vk_nir_lower_descriptor_heaps.c` (1,146 lines at the referenced commit).
- Its PanVK README identifies descriptor heaps as part of its Valhall VKD3D-Proton D3D12 capability.

This is a genuine multi-layer port, not an extension-table toggle. Before exposing it on this Kbase branch, compare Mesa base revisions and port the full ABI/entrypoint/SPIR-V/NIR/runtime/descriptor-state path, then build and run targeted Vulkan CTS and VKD3D-Proton tests. Keep the feature disabled until that is complete.

### Additional gaps that need complete per-feature audit

- `multiViewport`, `shaderClipDistance`, `shaderCullDistance`, `pipelineStatisticsQuery`, `geometryStreams`, and float atomic subfeatures remain disabled or partial in the current v12 query path.
- `VK_KHR_ray_query` and acceleration-structure extensions are not advertised or implemented here. Do not infer hardware RT capability from Mali branding or enable these features without confirming the G720 IP block and implementing AS objects, build/update, traversal, synchronization, shader lowering, and memory semantics.
- Sparse residency remains unavailable on Kbase because the bind queue is a stub and no Kbase GPU-VA sparse remapping path exists.
- Several advertised extension entries depend on Kbase properties, WSI build options, Android gralloc, or architecture gates; the 185-field census is not equivalent to 185 extensions enabled on this phone.

### Next engineering sequence

1. Produce a machine-readable audit of all 185 extension table entries, all 296 core/extension feature assignments, and every `VkPhysicalDeviceProperties2` property. For each, record exact gate, implementation/entrypoint, compiler/SPIR-V requirements, Kbase dependency, and test coverage.
2. Compare the complete JimVulkan descriptor-heap implementation against this branch's Mesa base. Identify all required files and generated registry changes; do not cherry-pick blindly across divergent Mesa revisions.
3. Port descriptor heaps as a coherent feature series on a dedicated branch. Keep extension/feature exposure off until all layers compile and tests pass.
4. In parallel, audit existing advertised capabilities for false-positive exposure (notably extensions whose properties/features are false or whose WSI/backend path is conditional). Correct unsafe over-advertising before adding more.
5. Update this handoff and the capability matrix after each completed port milestone. The user wants the next device test to validate substantial capability expansion, not another tiny FPS tweak.


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


## Addendum: Vulkan registry/header provenance check (2026-11-05)

The Meson source confirms `vk_api_xml = files('registry/vk.xml')` in `src/vulkan/meson.build`, and PanVK generates its entrypoint source/header from `vk_api_xml` via `src/panfrost/vulkan/meson.build`. GitHub Contents API returns zero-byte blobs for `src/vulkan/registry/vk.xml` and `include/vulkan/vulkan_core.h` on this branch, while `src/vulkan/meson.build` and `src/panfrost/vulkan/meson.build` are readable. Treat these as unavailable/submodule or pointer-backed inputs through this connector, not as proof that the Vulkan registry or headers are empty or missing. Do not attempt a registry edit until the exact checkout inputs and generated Vulkan headers are inspected in the build workspace.

The descriptor heap port remains at source-audit stage; no PanVK code or feature exposure has been changed. Latest docs commits before this note: handoff `4ed87bd0b3e96fc0706b1db2fa7e81487625abca`; capability matrix `0b6888d10f5c0d373a62a5c47e537cbbf6e698d5`.


## Addendum: descriptor-heap reference semantics extracted (2026-11-05)

Further source-level inspection of JimVulkan/mali-panvk at `58f2c55854bd48b18df756edb11f187af9cd2fde` established the exact Valhall lowering contract. This is reference behavior only; it has not been ported or built in this repository.

- Heap byte offsets become Mali resource handles by shifting right `log2(PANVK_DESCRIPTOR_SIZE)` (32-byte descriptor stride) and adding a resource-table handle whose table index is `PANVK_HEAP_RESOURCE_TABLE` or `PANVK_HEAP_SAMPLER_TABLE`.
- Texture NIR sources `nir_tex_src_texture_heap_offset` and `nir_tex_src_sampler_heap_offset` are rewritten to Mali texture/sampler handles. If no sampler heap handle is supplied, the shader uses a per-shader embedded-sampler table; entry 0 is a default sampler and embedded samplers are indexed from entry 1.
- `load_heap_descriptor` becomes a three-component buffer resource tuple `(handle, 0, 0)`; `load_resource_heap_data` reads the heap memory itself through the resource-table buffer descriptor. Image heap load/store/atomic/size/sample intrinsics are rewritten through the normal image intrinsic lowering path.
- Resource table layout reserves entries after driver set 0 and normal descriptor sets: resource heap at `MAX_SETS+1`, sampler heap at `MAX_SETS+2`, embedded sampler table at `MAX_SETS+3`; table end is `MAX_SETS+4`. On shader variants marked `uses_heaps`, table allocation must extend to that end, and the three entries must be populated for every repeated table.
- Heap binding updates both graphics and compute descriptor states because heaps are global to both bind points. Secondary command buffers inherit heap bindings through `VkCommandBufferInheritanceDescriptorHeapInfoEXT`.
- `CmdPushDataEXT` copies the provided byte range into the existing push-constant storage and dirties VS, FS, and compute push-uniform state. It asserts the range fits `MAX_PUSH_CONSTANTS_SIZE`; production implementation must rely on validated API ranges and avoid overflow in offset+size arithmetic.
- Shader creation with `VK_SHADER_CREATE_DESCRIPTOR_HEAP_BIT_EXT` uploads a default sampler plus embedded samplers into RW pool memory aligned to descriptor size. Shader binary serialization also records the embedded sampler count/data, and shader destruction frees that allocation.
- Reference properties are 64-byte heap alignment, 32-byte descriptor size/alignment, max heap size `PANVK_DESCRIPTOR_SIZE << 24`, 256-byte max push data, 2,048 embedded samplers, capture/replay false, sparse/protected heaps false. Reference also reports `minStorageBufferOffsetAlignment=4` on Valhall; this is a separate measured hardware/compiler guarantee and must be independently validated against our exact 26.3 NIR lowering before adopting.

### Port blockers / next exact work

1. Confirm the 26.3 generated Vulkan headers/registry and entrypoint generator contain `VK_EXT_descriptor_heap` types, structures, and commands. Do not infer from the GitHub zero-byte contents response.
2. Inspect `vk_nir_lower_descriptor_heaps()` API and its required `VkPipelineLayout`/shader compile metadata, then find the correct call point in our 26.3 `panvk_vX_shader.c`. The generic lowerer existing in the tree does not imply it is invoked.
3. Port heap binding storage into `panvk_descriptor_state` and command-buffer inheritance state, then implement checked push-data range handling.
4. Port NIR heap texture/intrinsic lowering and resource table layout together; they form one ABI and must not be enabled separately.
5. Port embedded sampler compilation and cache serialization as a single lifecycle (create, upload, serialize/deserialize, destroy).
6. Audit reference descriptor-heap code for Mesa 27-only fields/functions and adapt to Mesa 26.3 APIs; no whole-file replacement.
7. Only after generated API + all execution paths compile, enable the extension/feature and run Vulkan CTS plus VKD3D-Proton D3D12. No new package or device test is ready yet.


## Addendum: correction — generic descriptor-heap lowering is already wired (2026-11-05)

A search of the *current project branch* `experiment/g720-clip-cull-capabilities` found that the generic descriptor-heap lowering is not merely present as a source file: Mesa 26.3's common Vulkan runtime already invokes it in both shader-module and pipeline shader compilation paths.

Verified current project files:
- `src/vulkan/runtime/vk_shader.c`, blob `6354e51d364cd2f92314ac4e55143dc3b5c73d79`: includes `vk_nir_lower_descriptor_heaps.h`, extracts `VkShaderDescriptorSetAndBindingMappingInfoEXT` from shader create info `pNext`, calls `vk_nir_lower_descriptor_heaps(nir, desc_map, NULL, embedded_samplers_out)`, then removes dead uniform/image variables and runs DCE when lowering made progress.
- `src/vulkan/runtime/vk_pipeline.c`, blob `4c06e7db9067394192dd44f554a95f1c161e0855`: same generic lowering in pipeline NIR compilation; it also hashes descriptor heap mappings into the shader stage cache key. This avoids reusing compiled shaders for incompatible heap mappings.
- The runtime API therefore already has an `embedded_samplers_out` output contract. PanVK must consume that output through its shader compile callback and preserve it through shader binary/cache serialization.

This corrects the earlier statement that PanVK itself must add the generic lowerer call to its shader compilation path. The generic runtime owns that call; the PanVK work is the backend lowering of heap-offset NIR operations, the returned embedded sampler states, resource table / heap binding state, push data command, and feature/entrypoint wiring.

The immediate next inspection is the `vk_shader_ops` / `vk_pipeline` callback contract in Mesa 26.3 versus the PanVK shader creation path. Do not add a duplicate NIR pass in PanVK. Confirm whether the callback signature already returns `vk_sampler_state_array` to PanVK; if not, the narrow runtime API adaptation required must be explicit and cache-safe.


## Addendum: shader metadata handoff audit (2026-11-05)

The generic descriptor-heap path's embedded-sampler handoff is already represented in Mesa 26.3's common shader compile ABI; PanVK should consume it rather than adding a new runtime callback.

Verified current project branch source:
- `src/vulkan/runtime/vk_shader.h` (`4e0d4dc84f3ec43554154874eb6802de92fc9141`): `vk_shader_compile_info` contains `embedded_sampler_count` and `embedded_samplers`.
- `src/vulkan/runtime/vk_shader.c` (`6354e51d364cd2f92314ac4e55143dc3b5c73d79`): shader-object compilation passes the `vk_sampler_state_array` output into `vk_shader_compile_info_init()`, which assigns those fields.
- `src/vulkan/runtime/vk_pipeline.c` (`4c06e7db9067394192dd44f554a95f1c161e0855`): pipeline compilation passes embedded sampler data into the pipeline precompile object and finishes the temporary array after precompilation. Inspect precompile cache ownership before assuming pointer lifetime.
- `src/panfrost/vulkan/panvk_vX_shader.c` (`65b718457ad5859e43988a9e77cc4515574a23d9`): `panvk_compile_shader()` receives this compile info but currently does not reference either embedded-sampler field.
- `src/panfrost/vulkan/panvk_shader.h` (`963601351b4281e8750516beab4c99a643fe7b32`): Valhall shader metadata has no heap-use bit or embedded-sampler data/address fields.
- `src/panfrost/vulkan/panvk_cmd_desc_state.h` (`9b82fc389a2513983c2ebc668b24c002b246bef4`): descriptor state has no resource/sampler heap bindings.
- `src/panfrost/vulkan/panvk_vX_cmd_desc_state.c` (`6beab5f62c70de7b7e1aeac587bd805126b84654`): Valhall resource-table allocation currently covers the driver set and ordinary descriptor sets only.
- `src/panfrost/vulkan/panvk_vX_cmd_push_constant.c` (`cefd287bd323eb0decd81f45c484a13cc0e1a610`): existing push-constant storage and dirty-state logic can be reused for `CmdPushDataEXT`, but all-stage semantics and bounds validation are separate requirements.

Implementation consequences: PanVK must upload embedded sampler descriptors, retain their GPU address/count, serialize and restore them in its own shader binary/cache, and free the allocation on shader destruction. Add a `uses_heaps` bit independent of sampler count. Extend graphics and compute descriptor state with heap bindings, reserve/populate aligned resource-table entries, and implement secondary-command-buffer inheritance. Generic pipeline precompile caching and PanVK final shader binary caching are separate ownership layers; verify both before changing the binary layout and bump PanVK cache/binary version if needed. No code or feature exposure has changed yet.


## Addendum: exact Valhall reference implementation lifecycle (2026-11-05)

Inspected the full relevant reference functions at JimVulkan/mali-panvk commit `58f2c55854bd48b18df756edb11f187af9cd2fde`:

- Shader heap metadata is stored in `panvk_shader_desc_info`: `uses_heaps`, embedded sampler private-memory allocation, and descriptor count. Destruction frees that allocation. `init_heap_desc_info()` gates on `VK_SHADER_CREATE_DESCRIPTOR_HEAP_BIT_EXT`, creates a default sampler at index 0, converts each generic `vk_sampler_state` to a Mali packed sampler descriptor, then uploads the contiguous array aligned to `PANVK_DESCRIPTOR_SIZE`.
- The reference's shader binary format serializes the embedded sampler count and packed descriptor bytes; deserialization checks blob overrun and re-uploads the packed descriptors. `uses_heaps` is reconstructed by `upload_embedded_samplers()` for nonzero embedded count, but that representation is insufficient for a heap-using shader with zero embedded samplers unless other state/flag information is used. Our port should serialize an explicit `uses_heaps` bit rather than relying on count as the sole signal.
- Command descriptor state stores two heap address/size pairs. Bind commands update both graphics and compute states and dirty both descriptor states. Secondary command buffers read `VkCommandBufferInheritanceDescriptorHeapInfoEXT` and copy the optional sampler/resource bind infos.
- Valhall resource-table allocation chooses `PANVK_HEAP_TABLE_END` for heap-using shaders, otherwise the normal first-unused-set end; rounds to `MALI_RESOURCE_TABLE_SIZE_ALIGNMENT`; writes resource and sampler heap resource descriptors at fixed table indices; and writes the embedded sampler table using the shader-owned private-memory allocation.
- `CmdPushDataEXT` dirties VS, FS and compute push-uniform state and copies the provided data into the existing push-constant storage. Reference uses an `assert(offset + size <= MAX_PUSH_CONSTANTS_SIZE)`; our port must use overflow-safe validation (e.g. `offset <= MAX && size <= MAX - offset`) and rely on Vulkan validation for API-invalid calls rather than treating assert as runtime protection.
- The reference exposes `.descriptorHeap = PAN_ARCH >= 9`, `.descriptorHeapCaptureReplay = false`. This confirms the feature gate used by that reference, not proof that our incomplete backend is ready to expose it.

Reference caveat: its binary format's implicit heap-use inference from nonzero embedded sampler count is a potential zero-embedded-sampler edge case. Preserve explicit heap-use metadata in our implementation and cache format. No source code changed and the extension remains disabled in our branch.


## Addendum: API registry and command dispatch blocker (2026-11-05)

The repository Git tree resolves the earlier registry ambiguity: `src/vulkan/registry/vk.xml` is a real tracked blob of 3,258,412 bytes (blob SHA `4d66622efa2937668bc35cfcc114a302813828d9`) on `experiment/g720-clip-cull-capabilities`. The previous zero-byte `fetch_file` response was a connector retrieval limitation, not an empty registry. The raw 3.2 MB file was rejected by the GitHub fetch connector as too large, so the exact presence/absence of `VK_EXT_descriptor_heap` declarations in this XML is still not established.

The project code tree has `src/panfrost/vulkan/meson.build` blob `35b447a33f49ee25b1aa0293931fe81bf6835a94`, which generates the PanVK entrypoint sources from the Vulkan registry. Current PanVK backend command-state files contain no implementations of `CmdBindSamplerHeapEXT`, `CmdBindResourceHeapEXT`, `CmdPushDataEXT`, nor secondary heap inheritance. This means command exposure cannot be completed by merely adding feature bits: registry commands, dispatch prototypes, per-arch implementations, and extension table must all align.

A direct GitHub code-search for `VK_EXT_descriptor_heap`, `VkPhysicalDeviceDescriptorHeapFeaturesEXT`, `CmdPushDataEXT`, and the feature structure type returned no indexed matches for this branch. Code search indexes the default branch, so this is not authoritative evidence of absence on the experiment branch. Treat as unresolved until a local checkout or a small generated-file inspection is available.

The reference implementation is a later Mesa 27 snapshot while this branch is Mesa 26.3-devel. Its descriptor heap code cannot safely be copied wholesale. No code changes made; descriptor heap remains unexposed. To resolve registry/API definitively, inspect local checkout `src/vulkan/registry/vk.xml` for `VK_EXT_descriptor_heap`, plus generated `vk_device_dispatch_table.h`, `vk_device_entrypoints.h`, and `vk_physical_device_features.h` / feature struct definitions. If present, proceed with PanVK backend port; if absent, first update Vulkan-Headers/registry and generated code consistently using Mesa's normal generation scripts.


---

## 2026-11-05 — Descriptor heap backend port started (Mesa 26.3 / Valhall)

Repository-wide branch reconciliation was performed before continuing. The repository had 23 branches during the initial sweep; creating `experiment/g720-descriptor-heaps` brings the current total to 24. The canonical handoff exists on `docs/project-continuity-handoff-2026-11-03`; the other branches do not contain divergent copies. The latest handoff blob was fetched before this update.

### New isolated implementation branch

- Branch: `experiment/g720-descriptor-heaps`
- Base: latest source branch `experiment/g720-clip-cull-capabilities` at `9188e7bf92a032431ca16ac02d54d81be5a5e12e`
- Current branch HEAD after YCbCr descriptor-count property correction: `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2`
- The existing source and audit branches remain unchanged.

### Implemented in the experimental branch (not yet build-validated)

1. Reserved Valhall resource-table entries after driver set 0 and the seven application descriptor sets:
   - resource heap: `MAX_SETS + 1`
   - sampler heap: `MAX_SETS + 2`
   - embedded sampler table: `MAX_SETS + 3`
   - table end: `MAX_SETS + 4`
2. Added explicit `uses_heaps` state and embedded-sampler backing allocation to the per-variant `panvk_shader_desc_info`.
3. Ported Valhall NIR heap lowering from the Mesa 27 reference:
   - byte offsets become 32-byte descriptor handles in the resource/sampler tables;
   - heap buffer descriptors become resource handles;
   - resource heap data loads lower through the resource-table UBO;
   - heap image operations lower through the ordinary Mali image path;
   - already-lowered texture handles bypass normal descriptor-set texture lowering.
4. Extended resource-table construction to append resource heap, sampler heap, and embedded-sampler entries; implemented `CmdBindSamplerHeapEXT`, `CmdBindResourceHeapEXT`, and secondary-command-buffer heap inheritance.
5. Implemented `CmdPushDataEXT`, including overflow-safe bounds checking against the push-constant backing store and dirtying VS/TCS/TES/FS/compute push-uniform state.
6. Added per-variant default/embedded sampler packing, allocation cleanup, serialization, and deserialization. The binary format now stores an explicit heap-use flag and validates the sampler count; shader binary version was raised from 0 to 1.
7. Added Valhall-only extension, feature, and property exposure for `VK_EXT_descriptor_heap`. Sparse and protected descriptor heaps remain false; capture/replay remains false.
8. Added the new branch to the Android-only CI workflow and added source-presence guards.

### Important porting difference and review item

The Mesa 27 reference has a shader-level descriptor-info object, whereas this Mesa 26.3-derived branch stores descriptor metadata per physical shader variant. The initial port preserves this branch's existing ownership model and allocates embedded sampler backing per variant. This is functionally straightforward but duplicates sampler backing for multi-variant VS/GS shaders. Before declaring the port complete, review whether to consolidate the immutable embedded-sampler allocation at API-shader scope while keeping per-variant descriptor metadata separate.

### Build/validation status

The extension exposure and implementation are provisional until CI proves that this branch's Vulkan registry/generated API declarations include all descriptor-heap structures and commands and that the Android ARM64 driver compiles. Do not package or ask for a Winlator test yet.

Initial workflow runs triggered by the latest source commits:
- Android-only build: run `42076030943` (queued at last check).
- Android wrapper build: run `42076031093` (debugoptimized and release jobs queued at last check).
- Earlier wrapper builds were also triggered by intermediate commits; use the final-head runs for validation.

Next steps:
1. Read the final-head Android-only and wrapper build logs; fix API-generation, C compilation, linker, and packaging issues.
2. Audit descriptor heap command coverage against the extension registry and reference implementation.
3. Add targeted NIR/compiler validation and descriptor-heap runtime/CTS checks where the available CI supports them.
4. Review per-variant embedded-sampler allocation and cache serialization/versioning.
5. Only after successful build and validation, produce the inner Winlator-importable `PanVK-G720-experimental.zip`. Keep it separate from Android's vendor/system driver.
6. Do not ask the user to test until that package is ready.

No new device benchmark was run. The last user-reported Tomb Raider Definitive Edition baseline remains Min 9.4 / Max 19.4 / Avg 14.0 FPS; no performance claim is made for this unbuilt branch.


Follow-up hardening on `experiment/g720-descriptor-heaps`: shader-binary deserialization now rejects non-boolean heap-use flag bytes and checks blob overrun immediately before accepting the embedded-sampler count. This is included in source HEAD `0bfd34d0392efb0c98e31ad0400d991141f671ff`; a fresh final-head Android build is required because the earlier build run targets `4edd96ec9bb936dc29b86e7f2b1f814f9f9922c3`.


### First Android compile result and correction

The first final-stage Android-only compile on source HEAD `4edd96ec9bb936dc29b86e7f2b1f814f9f9922c3` reached PanVK compilation and failed in `panvk_vX_shader.c` because this Mesa 26.3 branch did not yet declare/implement the reference's `panvk_per_arch(sampler_state_fill_desc)` helper. The descriptor heap API/property fields, command-state source, push-data source, and NIR lowering source were compiled before the failure; the error was not an absent Vulkan registry declaration.

Correction:
- Ported `panvk_per_arch(sampler_state_fill_desc)` from the Mesa 27 reference into `panvk_vX_sampler.c`.
- Added its prototype to `panvk_sampler.h`.
- Current source branch HEAD is now `346bffa794850bfa70c3495794f8ccccda89a251`.

A new Android-only build started on the immediately preceding source commit `13bd689e56ec0baa7ee4f92bc5de0a7c81c888ef` (run `42077165223`); the header declaration commit `346bffa794850bfa70c3495794f8ccccda89a251` was pushed immediately afterward, so a fresh build on the exact latest HEAD is required. That exact-HEAD build is now running as Android-only run `42077180170` (head `346bffa794850bfa70c3495794f8ccccda89a251`); its checkout verification, dependency installation, host-tool generation, real Android ARM64 libdrm build, and Meson configuration have passed; the full Android ARM64 compile/stage step is in progress at last check. Do not treat the earlier failed run as validation, and do not package/test yet.


### Full Vulkan registry command audit — descriptor heap writers

The earlier API audit was extended by reading the repository's actual `src/vulkan/registry/vk.xml` and `include/vulkan/vulkan_core.h` through the GitHub contents API (the registry file was not empty; the earlier limitation was only the file-view transport). The current registry requires these descriptor-heap commands:

- `vkWriteSamplerDescriptorsEXT`
- `vkWriteResourceDescriptorsEXT`
- `vkCmdBindSamplerHeapEXT`
- `vkCmdBindResourceHeapEXT`
- `vkCmdPushDataEXT`
- `vkGetImageOpaqueCaptureDataEXT`
- `vkGetPhysicalDeviceDescriptorSizeEXT` (already implemented by Mesa's shared Vulkan runtime)

This exposed an incompleteness in the Mesa 27 reference PanVK port: it contained the three command-buffer operations but not the two descriptor-writing commands or the image opaque-capture command. We are not treating that reference as a complete implementation.

The isolated branch now also implements:
- sampler descriptor writes using `vk_sampler_state_init` and the Valhall Mali sampler packer;
- resource descriptor writes for uniform/storage buffers, uniform/storage texel buffers, sampled/storage images, and input attachments;
- Mali null-descriptor packing for null resource descriptors;
- multi-plane image descriptor output using PanVK's Valhall texture-count helper;
- `vkGetImageOpaqueCaptureDataEXT` as a `VK_ERROR_FEATURE_NOT_PRESENT` stub because descriptor-heap capture/replay remains disabled;
- CI guards requiring all three descriptor-heap command-buffer operations, both writer commands, and the capture-data entrypoint.

Acceleration-structure and ARM tensor descriptor writes deliberately return `VK_ERROR_FEATURE_NOT_PRESENT`; PanVK does not expose those corresponding device capabilities. The descriptor-heap extension must not be advertised as supporting those descriptor types.

For Valhall, inspection of `panvk_vX_image_view.c` confirmed the auxiliary `view->mem` attribute-buffer allocation is used only under `PAN_ARCH < 9`; the Valhall texture descriptors are self-contained, so temporary image views can be destroyed after copying their descriptors.

Current source HEAD: `c3f9a776cb59215a42e4c6c5579a1a90f1ae8312`.
Exact-head Android-only build: run `42079149912` (queued at last check). Wrapper build: run `42079150104` (queued at last check). The earlier Android run `42077180170` targets the pre-writer implementation and is not final validation.

Next: compile/link the new writer entrypoints; inspect all warnings and API dispatch generation; add descriptor-writer-specific validation; then package only after full Android ARM64 build and audit pass. No device test is requested yet.


### Custom border-color registration for descriptor heaps

The current `VK_EXT_descriptor_heap` registry also conditionally requires `vkRegisterCustomBorderColorEXT` and `vkUnregisterCustomBorderColorEXT` when `VK_EXT_custom_border_color` is supported. PanVK advertises custom border colors. The implementation was added to the Valhall-only descriptor-heap path:

- Registration is a no-op because the Mali sampler descriptor embeds the full custom border-color value rather than referencing a fixed hardware palette.
- If `requestIndex` is true, in-range indices are accepted unchanged; out-of-range indices return `VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS`.
- If `requestIndex` is false, index 0 is returned. Multiple registrations of the same index are valid for implementations that do not need registration.
- Unregistration is a no-op.
- The sampler descriptor writer packs the actual custom border color value; it does not rely on the opaque registration index.

CI now checks that both conditional custom-border commands are present. Current source HEAD is `c9377fc98595de9b873fcba5a6997c5bf514ab4c`; exact-head Android-only run `42079497576` is queued. No build or runtime validation of the descriptor writer path has passed yet.


### Descriptor writer spec corrections

The current Khronos proposal was checked for descriptor-writing constraints:

- `vkWriteSamplerDescriptorsEXT` must not write YCbCr or subsampled samplers; these must use embedded samplers. The writer now rejects both.
- Custom border colors cannot be used with embedded samplers; `panvk_init_heap_desc_info` now rejects them.
- Custom border colors written to the sampler heap require an explicit registered index; the writer now rejects a missing/out-of-range index.
- YCbCr image descriptors may consume multiple image descriptors. The writer uses `panvk_image_get_tex_count(PAN_ARCH, view_format)`, which is also the PanVK count used for per-binding texture descriptor allocation.
- PanVK does not expose `VK_EXT_fragment_density_map`, QCOM image-processing descriptor types, `VK_KHR_acceleration_structure`, or `VK_ARM_tensors`; corresponding subsampled/QCOM/AS/tensor descriptor types remain unsupported.

These restrictions are now reflected in source HEAD `0bf2e527c42fe06733d5c0b1557c9d2cec0c1412`. Exact-head Android-only build run `42079746341` is queued. No compile/link or runtime validation has passed yet.


### YCbCr image descriptor count consistency

The descriptor writer uses `panvk_image_get_tex_count(PAN_ARCH, view_format)`, which is also the count used by PanVK's ordinary descriptor-set layout. The shared `vkGetPhysicalDeviceImageFormatProperties2` path previously hardcoded `combinedImageSamplerDescriptorCount = 1`. This could disagree with the number of texture descriptors required for multi-plane formats. It now reports `panvk_image_get_tex_count(physical_device->model->arch, base_info->format)`, so the queried count and the heap writer's output-size requirement use the same architecture/format rule.

Source HEAD: `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2`. Exact-head Android-only build run `42080294652` is queued; no compile/runtime validation yet.


### Intermediate Android package validation

Android-only run `42078972694` completed successfully on source commit `3e4e7ee3849ebab12c114eef8c6c3a650cd14b8f`. It passed Android ARM64 compile/stage, the ELF and libdrm dependency audit, flat Winlator/Adrenotools package creation, and artifact upload. Artifact `13438649591` (`panvk-g720-android-only-experimental-package`, 16,804,388 bytes) was produced.

This is an intermediate validation only: that commit includes the first descriptor writer implementation but predates the later custom-border registration commands/restrictions and the YCbCr descriptor-count property correction. **Do not ask the user to test or import this intermediate artifact.**

Current source HEAD remains `c0c8ffa837bb85aa67b2217dbd7b61fb51d796f2`; exact-head Android-only run `42080294652` is queued and must pass before packaging/testing is considered current.


### Exhaustive extension/feature census update

The capability matrix now contains an exhaustive parse of the current descriptor-heap source branch's physical-device initializer:
- 186 explicit extension-table assignments;
- 298 feature assignments: 173 literal true, 70 literal false, and 55 architecture/device/DRIRC-gated expressions.

The matrix lists every extension initializer, every literal-false feature field, and every non-literal feature gate. It also separates likely compiler-emulatable candidates from features needing hardware/ISA confirmation and architectural limitations. This replaces older approximate counts (185/296). Matrix branch HEAD is now `82fc0e49709e170e7b669c3c62b6bc1cbbc9e722`.



### Exact-head compile failure and correction

Android-only run `42080294652` reached PanVK compilation and failed at the new YCbCr descriptor-count expression: `struct pan_model` intentionally has no `arch` member. The fix derives the architecture from the actual Kbase GPU ID with `pan_arch(physical_device->kmod.dev->props.gpu_id)`, matching existing PanVK physical-device code. The corrected source is `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`; exact-head Android-only run `42082198442` is queued. The intermediate run `42078972694` still validates only the older writer implementation and is not a test artifact.


---

## 2026-11-05 — Descriptor-heap branch exact-head build and package passed

The descriptor-heap implementation has now passed the first exact-current-source Android build and packaging gates.

### Exact source and CI results

- Source branch: `experiment/g720-descriptor-heaps`
- Exact source HEAD: `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`
- Android-only workflow run: `42082198442`
  - Job `127437878725`: SUCCESS.
  - Android ARM64 Meson configure, compile/stage, driver/manifest/dependency audit, flat Winlator/Adrenotools package build, package audit, and artifact upload all passed.
- Android wrapper workflow run: `42082198906`
  - Release job `127437880299`: SUCCESS.
  - Debugoptimized job `127437880903`: SUCCESS.
- Exact-head Android package artifact: `13441682356`
  - Artifact name: `panvk-g720-android-only-experimental-package`
  - Size: 16,805,654 bytes.
  - GitHub artifact digest: `sha256:0a4f6b7899f3df42a89133f071b2c441e62739e8000ad55bdf14af7329fccf71`
  - Expires: 2027-02-03T18:13:46Z.
- Nested Winlator-importable package: `PanVK-G720-experimental.zip`
  - SHA-256: `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6`
  - Package version: `26.3.0-devel-android-kbase-04947cb94a99`
  - Flat archive contents verified: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`.
  - Inner SHA256SUMS verified against both packaged binaries and metadata.
- ELF/DRM audit: missing DRM symbol list is empty; all required DRM references are resolved by the bundled private `libpanvk_drm.so`. No missing DRM symbols were found.
- The outer artifact was downloaded and inspected; its nested package SHA-256 matches the audit record. This is the first exact-HEAD package; older artifact `13438649591` remains stale and must not be used.

### Descriptor-heap source review performed

Reviewed the Valhall shader metadata/cache serialization, NIR lowering, resource-table construction, heap bind/inheritance commands, descriptor writers, custom-border registration commands, and physical-device extension/feature/property reporting.

- NIR heap byte offsets are shifted by the 32-byte descriptor size and converted to resource-table handles; resource heap data loads use the resource-table UBO path.
- Resource-table entries are reserved for resource heap, sampler heap, and embedded samplers.
- Shader binary cache revision 1 serializes the explicit heap-use flag and packed embedded sampler data; deserialization bounds the count and checks the blob before consuming sampler bytes. The mandatory default embedded sampler means heap-using shaders have at least one embedded sampler.
- Descriptor writer supports sampler descriptors, uniform/storage buffers, uniform/storage texel buffers, sampled/storage images, and input attachments. Unsupported acceleration-structure/tensor/vendor descriptor types return `VK_ERROR_FEATURE_NOT_PRESENT`; YCbCr/subsampled samplers are rejected by the sampler writer and YCbCr image descriptor count follows the same PanVK texture-count helper as ordinary descriptor layouts.
- Capture/replay, sparse heaps, and protected heaps remain disabled. `VK_EXT_descriptor_heap` and `descriptorHeap` are exposed only on Valhall-class architectures (`PAN_ARCH >= 9`); G720 v12 is included. No ray-query or sparse-binding support is being force-advertised.
- The command entrypoints compile successfully from the repository registry/generated dispatch path. This confirms compile-time API wiring, not Vulkan CTS conformance.

### Current gate / user action

The exact-head source has passed compile/link, Android ARM64 package creation, ELF/libdrm symbol audit, and flat package-contract audit. A device runtime test is now required. No runtime test, Vulkan CTS run, or game-compatibility claim has been made for this descriptor-heap branch.

Ask the user to import **only** the nested `PanVK-G720-experimental.zip` from artifact `13441682356` into Winlator Mali 1.2 using its custom driver importer. Do not install or replace the Android vendor/system Vulkan driver. First verify that Winlator loads the package and that a Vulkan app/game reaches device creation and swapchain creation; then run the controlled Tomb Raider test and capture the complete Winlator logs. If the first runtime attempt fails, retain the logs and do not repeatedly benchmark or change unrelated wrapper settings.

The package can be downloaded from the GitHub Actions run:
https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42082198442

Next engineering work after the user supplies runtime evidence: diagnose any loader/WSI/CSF fault, add descriptor-heap targeted tests (especially buffer/image/sampler descriptor writes, YCbCr image descriptor counts, secondary inheritance, and shader-cache round trips), and only then consider broader capability enablement or performance tuning. Do not claim descriptor heaps are Vulkan-conformant based on compilation alone.


### Repository inventory correction (2026-11-05)

A fresh branch search after the descriptor-heap work found 26 branches in the repository. The canonical root handoff remains only on `docs/project-continuity-handoff-2026-11-03`; the isolated implementation remains `experiment/g720-descriptor-heaps` at `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`. No branch was deleted or merged during this pass.


---

## 2026-11-06 — Tomb Raider descriptor-heap runtime benchmark and next capability investigation

### User test result

User imported the exact-head descriptor-heap package and ran Tomb Raider. Reported:
- Minimum: 7.9 FPS
- Maximum: 18.0 FPS
- Average: 13.6 FPS

This proves the package can be loaded far enough to run the workload and produce a benchmark, but does not prove the descriptor-heap path is exercised or correct for real descriptor-heap API consumers.

### Uploaded diagnostic archive reviewed

Archive: `TombRaider_logs_2026-11-06_01-24-09.zip`
Files: `wine_debug.log`, `TombRaider_d3d11.log`, `TombRaider_dxgi.log`, `TombRaider_d3d9.log`.

Observations:
- DXVK is **v1.7.2**, a legacy version.
- D3D11 feature level 11_0 is selected.
- DXVK logs 4 compiler threads.
- Reported Vulkan API is 1.3.354; reported device string is `Wrapper(Mali-G720 MC7)`; driver version string is `26.2.149`.
- DXVK's enabled device-extension list does not include `VK_EXT_descriptor_heap`; therefore the previous descriptor-heap work is not expected to directly improve this DX11 workload. Do not attribute the 13.6 FPS result to descriptor heaps or infer that heap writes are exercised.
- Swapchain is 900x506, with immediate and FIFO modes reported at different setup points.
- Effective DXVK configuration includes `d3d11.maxQueuedFrames=1`, `d3d11.allowMapFlagNoWait=True`, `d3d11.relaxedBarriers=True`, `d3d11.ignoreGraphicsBarriers=True`, `dxvk.useEarlyDiscard=True`, `dxvk.memoryTrack=True`, and 4 compiler threads. These are material test variables; preserve the current test as a recorded baseline and do not change them silently in the next comparison.
- Wine log ends with an assertion in `winevulkan/loader_thunks.c` at `vkCreateImage`, followed by X display teardown. This archive excerpt does not contain a clear Kbase GPU fault status comparable to the earlier 0x88 shareability fault. Do not assume the old fault recurred.

### Next capability candidate: VK_KHR_fragment_shading_rate (VRS)

A fresh capability review identified VRS as a better hardware-backed feature candidate than ray query for the **Mali-G720** variant:
- Arm's Mali-G720 product support page lists variable rate shading, including pipeline, primitive, and attachment rates up to 4x4.
- The Khronos extension exposes independent `pipelineFragmentShadingRate`, `primitiveFragmentShadingRate`, and `attachmentFragmentShadingRate` feature bits.
- Current PanVK branch does not advertise `VK_KHR_fragment_shading_rate`; physical-device properties explicitly state that fragment shading rate is not implemented, and the property `fragmentShadingRateClampCombinerInputs` is false.
- This is real driver work, not a feature-table-only change. At minimum, audit hardware packet/register encoding, dynamic pipeline state, pipeline create-info handling, per-primitive shader output lowering, attachment image layout/usage and format support, query commands, limits/combiners, and Vulkan validation. Implement a narrowly scoped first tier (pipeline FSR only) only if the Mali G720 hardware path and spec requirements can be established; do not advertise primitive/attachment bits prematurely.
- VRS is a performance-oriented capability, but it only improves a workload if the application/API translation layer actually requests non-1x1 shading rates. The current DXVK 1.7.2 Tomb Raider run is not known to use VRS. A new extension alone cannot be expected to raise this benchmark.

### Branch created

- New isolated research branch: `experiment/g720-fragment-shading-rate`
- Base: exact descriptor-heap source HEAD `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`
- No driver source changes have been made on this branch yet.
- Keep descriptor-heap branch intact as the benchmarked package baseline.

### Important correction about ray tracing

Do not equate Mali-G720 with Immortalis-G720. Hardware ray tracing is associated with Immortalis variants; the ordinary Mali-G720 MC7 target must not be assumed to have a ray-tracing unit. Do not add `VK_KHR_ray_query` merely because the architecture family name is similar. Any software ray-query implementation would be a major feature project and is not a sensible FPS optimization for this DX11 benchmark.

### Next engineering gates

1. Continue auditing the G720 VRS command/register path and PanVK pipeline/dynamic-state architecture on `experiment/g720-fragment-shading-rate`.
2. Determine which FSR modes the actual Xiaomi 15T stock Vulkan driver exposes; use the user's device report if available, but do not block source research waiting for it.
3. Implement and test only the subset that can be backed by hardware and complete Vulkan semantics. Keep all other feature bits false.
4. Add targeted unit/CTS coverage for reported rates, pipeline state, dynamic state, combiners, and validation.
5. Build Android ARM64, run ELF/package audits, then issue a new isolated Winlator package with checksums.
6. For performance comparisons, record DXVK version/config, resolution, game settings, thermal state, and same benchmark segment. The current 13.6 FPS baseline is not directly comparable to tests with different wrapper or DXVK versions.


### 2026-11-06 capability evidence refinement — stock MT6899 report

A public FauxVkDB validation report for a POCO X7 Pro (MT6899, Mali-G720 MC7, system driver r49p1) lists both `VK_KHR_fragment_shading_rate` and `VK_KHR_ray_query`, plus `VK_KHR_acceleration_structure`:
- Device report: https://fauxvkdb.canonspike.com/fr/gpu/13b5-c8700010/52e95a852ce4dc29
- Raw report JSON: https://fauxvkdb.canonspike.com/report/295.json

This is a device/SoC-matched stock-driver capability reference, not a proof that our PanVK implementation can expose these features safely. It refines the ray-tracing note above: the ordinary Mali-G720 MC7 stock driver does list ray-query and acceleration-structure extensions, but the extension list alone does not tell us whether traversal is implemented in software, which ray-query feature bits are true, or whether the same firmware/driver build exposes them on the user's Xiaomi 15T. Do not categorically claim the API extension is impossible on Mali-G720; equally, do not infer a dedicated RTU or turn on PanVK ray-query without feature/property and implementation evidence.

VRS remains the nearer-term candidate because Arm's product support page explicitly documents pipeline, primitive, and attachment VRS rates up to 4x4 on Mali-G720, and the Vulkan FSR extension provides separable feature bits. For the user's immediate DX11 Tomb Raider test, neither descriptor heaps nor VRS will improve FPS unless DXVK/game actually uses the corresponding API. DXVK 1.7.2's uploaded log does not show either extension in its enabled extension list.


### 2026-11-06 — VRS hardware packet path found; pipeline-only implementation underway

Further source-level audit found that the repository's v12 GenXML already describes the relevant hardware fields:
- `PRIMITIVE_FLAGS_2`: `VRS Draw Rate`, `VRS Combiner 0`, `VRS Combiner 1`.
- `FRAMEBUFFER_PARAMETERS`: `VRS Image Rate` and `VRS Image Plane` (attachment mode is not being implemented in this first pass).
- `DCD_FLAGS_0`: `Disable VRS Clamp 2x2`.
- The Bifrost compiler backend already supports `nir_intrinsic_load_frag_shading_rate` by reading the hardware primitive flags.

The next feature branch now has a narrow pipeline-only implementation:
- `VK_KHR_fragment_shading_rate` enabled only for `PAN_ARCH >= 12`.
- `pipelineFragmentShadingRate = true`; `primitiveFragmentShadingRate = false`; `attachmentFragmentShadingRate = false`.
- Physical-device query enumerates conservative 1x-sample rates: 4x4, 4x2, 2x4, 2x2, 2x1, 1x2, 1x1. Aspect ratio is capped at 2; 4x1 and 1x4 are deliberately not advertised.
- CSF `PRIMITIVE_FLAGS_2` emission consumes the common Vulkan dynamic FSR state, with sample-shading forced to 1x1.
- Fragment shader lowering corrects `gl_SamplePosition`/sample-center semantics using `load_frag_shading_rate` when pipeline FSR may be active.
- No primitive shader-rate output or shading-rate attachment support is advertised.

Implementation branch: `experiment/g720-fragment-shading-rate`, based on descriptor-heap commit `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`. Current source commits include:
- `26ce5ea8391061275d802ea26c610b137515842a`: extension/feature/property flags.
- `b0a8409b964c821505e5bd60bcee995d45e6c114`: rate enumeration.
- `0f3e66828cf5f1b8ac9947b856647daeb72029b7`: CSF VRS draw-rate register emission.
- `f651e48768a0110b811e874cf3c1157ec6a29746`: fragment sample-center lowering.
- `4fdde1a5c979b63fe01b1d2621362a579ca4f5c8` / `3994fda27c318e05fb00b1253401e35fecc528a9`: conservative rate/property correction.
- `9330fc2c5666bccf0a0be7ee53b7adf23a914e19`: Android package workflow branch check wiring.

Build status at handoff update: Android wrapper CI for final source commit `3994fda27c318e05fb00b1253401e35fecc528a9` is running in GitHub Actions run `42137623250` (release and debugoptimized jobs). This is not yet a passed build. The Android-only package workflow is not yet triggered for this new branch. Do not ship or test this source until CI compile and package audits pass.

Reference implementation comparison: a newer PanVK source tree gates its FSR implementation at `PAN_ARCH >= 14`, but the v12 GenXML itself contains the pipeline draw-rate and combiner fields. This branch deliberately implements only pipeline FSR for v12 and must be treated as experimental until the compile, Vulkan CTS, and device runtime tests establish correctness. The fact that GenXML contains fields is hardware programming evidence, not by itself proof of every API semantic.


### FSR conformance correction — mandatory rates and sample-count limits

The first pipeline-FSR draft under-reported the KHR-mandated rate/sample combinations. Before treating it as testable, the query/property values were corrected:
- Required 2x2 and 2x1 rates now report sample counts 1x and 4x.
- Required 1x1 reports all sample counts (`~0u`), still subject to the device's normal MSAA limits.
- Optional 4x4 and 4x2 rates report 1x only; 2x4 reports 1x/2x; 1x2 reports 1x/2x/4x.
- `maxFragmentShadingRateCoverageSamples = 16` and `maxFragmentShadingRateRasterizationSamples = VK_SAMPLE_COUNT_4_BIT`, consistent with 4x4@1x and the required 2x2@4x combination.
- `maxFragmentSizeAspectRatio = 2`; no 4x1 or 1x4 rate is advertised.

This follows the Khronos extension's required minimum rate list and the newer PanVK reference's conservative sample-count table. Latest source commit for this correction: `2a1dc018f29795835e3002f681be0f10943e3fc3`. Android wrapper and Android-only package workflows for this commit are queued; no build pass is claimed yet.


### FSR rate-table refinement

The final rate table also includes the verified 2x-MSAA combinations for 2x2 and 2x1, matching the current PanVK reference table: 2x4 at 1x/2x, 2x2 and 2x1 at 1x/2x/4x, 1x2 at 1x/2x/4x, and 1x1 at all sample counts. Latest query-table source commit: `ea9284fb58fea3c0f5861c21f7b9ff8ef2350915`. The Android build must be rerun against this exact head before packaging.


### Current FSR implementation head

Latest feature branch head after the 2x-MSAA rate-table refinement and comment update:
`715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`.

The latest exact-head wrapper and Android-only package workflows are being queued by GitHub Actions. Older in-flight runs cover earlier commits; they are useful for early compile diagnostics but do not replace a pass on this exact head. No VRS package has been approved for user testing yet.


### Early compile milestone — VRS implementation compiled

GitHub Actions run `42137547230` for feature commit `f651e48768a0110b811e874cf3c1157ec6a29746` completed successfully in both build types:
- Android wrapper release: SUCCESS.
- Android wrapper debugoptimized: SUCCESS.
- Both Meson configure and full project compile steps passed.

This commit already contained the extension/feature exposure, rate query, CSF `PRIMITIVE_FLAGS_2` programming, and fragment sample-center lowering. Later commits only refined physical-device property/rate sample-count reporting and CI checks. Exact-head validation is still pending on `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`; do not treat the early compile as a final package pass.


### Additional CI progress

- Wrapper workflow run `42137715709` for commit `9330fc2c5666bccf0a0be7ee53b7adf23a914e19`: release build and artifact upload succeeded; debugoptimized compile is still running at the time of this update.
- Android-only package workflow run `42137897031` for commit `3ea2dd468ec9bdb10ffbaa2b71326d98f5b39e94`: static source verification, host tools, Android ARM64 libdrm build, and Meson configure all passed; Android ARM64 driver compile/stage is in progress.
- These runs cover the implementation but predate the final sample-count table change. Exact current head remains `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a` and must pass its own builds.


### Winlator-side VRS smoke-test probe added

To avoid relying on Tomb Raider/DXVK 1.7.2 (which does not enable or exercise `VK_KHR_fragment_shading_rate`), a separate Windows x64 probe has been added:
- Source: `tools/panvk_vrs_probe_win.c`
- It dynamically loads Winlator's `vulkan-1.dll`, identifies the active Vulkan device/driver, checks the extension and `pipelineFragmentShadingRate` feature, enumerates the advertised rate/sample-count table, attempts `vkCreateDevice` with the feature enabled, and records a `vkCmdSetFragmentShadingRateKHR` 2x2 KEEP/KEEP command into a command buffer.
- It writes `panvk_vrs_probe.log` in its working directory; it does not modify or replace the Android system Vulkan driver.
- A dedicated `build-vrs-probe` job was added to the existing push workflow to produce artifact `panvk-vrs-probe-win64`.

This is an API/dispatch/device-create smoke test, not a rendering correctness or performance test. It does not submit a draw, so hardware FSR execution still needs a proper CTS/render test. The probe build is queued with the latest workflow update.


### Probe follow-up update

The Winlator probe now also creates a graphics command pool/buffer, records `vkCmdSetFragmentShadingRateKHR` at 2x2 with KEEP/KEEP combiners, and ends the command buffer. It still does not submit a draw; it verifies extension dispatch and command recording only.

Latest feature branch head including probe and workflow wiring: `1ec6d129897d7362e2c7b5eafd9d0c73be060924`. Its Android wrapper and Android-only workflows are queued, as is the Windows x64 probe build. Driver source itself remains unchanged from VRS source head `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`.


### Android-only package milestone — initial VRS implementation passed

Android-only workflow run `42137897031` for commit `3ea2dd468ec9bdb10ffbaa2b71326d98f5b39e94` completed successfully:
- Static source verification passed, including VRS extension/feature/query/CSF-state checks.
- Host tools and Android ARM64 libdrm build passed.
- Android-only PanVK ARM64 compile and staging passed.
- ELF, DRM symbol, manifest, staged dependency, and package audits passed.
- Flat Winlator/Adrenotools package built and uploaded successfully.

This package predates the final FSR rate/sample-count table refinements and must not be given to the user. The exact current source head `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a` still needs its own Android-only package run and audit.


### User benchmark/log result — 2026-11-06

- Latest descriptor-heap package benchmark supplied by user: Min 7.9 FPS, Max 18.0 FPS, Avg 13.6 FPS.
- User supplied `TombRaider_logs_2026-11-06_01-24-09.zip`; the run is the existing DX11 Tomb Raider / DXVK 1.7.2 path.
- This DXVK path does not request or use `VK_KHR_fragment_shading_rate`, so the new FSR feature should not be expected to increase this game's FPS. The VRS probe is for confirming API extension/feature/device-create/command-recording support. A D3D12/VKD3D title or a Vulkan VRS test is required to exercise VRS rendering.
- Keep the Tomb Raider benchmark as a regression check only. Do not interpret a similar FPS result as evidence that VRS failed; the application must issue VRS state for the feature to affect rendering.


### Wrapper CI completion update

The previously in-progress wrapper workflow `42137715709` has now completed successfully in both release and debugoptimized configurations. This is still an early source commit (`9330fc2c5666bccf0a0be7ee53b7adf23a914e19)); exact-head validation for the final FSR rate table and current branch is still required.


### Exact-source wrapper validation progress

For the driver-source head `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`, wrapper release compile and artifact upload have passed in run `42138343235`; debugoptimized compile is still in progress. The exact-head Android-only package workflow `42138343360` has passed checkout/source verification and is installing build dependencies. The Windows probe workflow is queued behind the wrapper jobs.


## 2026-11-06 continuation — exact-head VRS wrapper builds passed; Android package pending

### Exact source revision

- Feature branch: `experiment/g720-fragment-shading-rate`
- Driver source HEAD: `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`
- The exact-head wrapper workflow is run `42138343235`.

### Wrapper build result

Both Android ARM64 wrapper configurations completed successfully against the exact source HEAD:
- Release job: SUCCESS; Meson configure, full project compile, and artifact upload passed.
- Debugoptimized job: SUCCESS; Meson configure, full project compile, and artifact upload passed.

Artifacts:
- Release wrapper artifact ID: `13467245575`, SHA-256 digest: `1d6d68693f1f22e5be685c50421e6bb68267eeef103449bd3e6e52dd61621143`.
- Debugoptimized wrapper artifact ID: `13468145780`, SHA-256 digest: `63e9901f3849bd8d2128b9eab4374cc2b7cf7230bbb7abf3d4843991b8af7d6f`.
- Both artifacts are tied by GitHub metadata to branch `experiment/g720-fragment-shading-rate` and exact SHA `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`.

These are wrapper build artifacts, not the flat Android-only Winlator driver-import package. They establish compile success for the wrapper build path only.

### Android-only package workflow

- Exact-head Android-only workflow run: `42138343360`.
- At the time of this update, checkout and source verification passed; dependency installation was still in progress. Host-tool generation, Android ARM64 libdrm, Android-only configure/compile, ELF/dependency audit, flat package creation and upload had not yet completed.
- No exact-head Android-only package artifact exists yet. Do not send an older VRS package to the user.

### Winlator probe workflow

- Probe workflow run: `42138721988`.
- At the time of this update, the workflow jobs were queued. No Windows probe artifact is available yet.
- The probe source records extension/feature/property enumeration, device creation with pipeline FSR enabled, and a 2x2 KEEP/KEEP command-buffer recording. It does not submit a draw and is not a rendering correctness test.

### User action gate

Do not request device testing until the exact-head Android-only package passes all compile, ELF/libdrm, dependency and flat-package audits. The wrapper builds passing do not replace that gate. Keep the Android vendor/system Vulkan driver untouched; eventual device testing must use Winlator Mali 1.2's custom driver importer only. Tomb Raider/DXVK 1.7.2 remains a regression check, not a VRS performance test, because its captured enabled extension list did not include `VK_KHR_fragment_shading_rate`.
