# PanVK G720 project continuity handoff
Last updated: 2026-11-03 (UTC)

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

Potential stale comment found:
- The top-level comment in `src/panfrost/lib/kmod/kbase_kmod.c` still says command submission is not wired up, while this branch contains an operational Kbase CSF submission implementation in `panvk_vX_gpu_queue.c`. This is documentation drift, not a runtime bug; update only when preparing a focused documentation cleanup.


## 2026-11-03 addendum — Kbase sync payload and CSF notification audit (in progress)

The next source-audit task from the prior read-only handoff has been completed and a separate experimental branch now carries a candidate fix. This addendum supersedes the earlier read-only section's next actions 1–3; it does not declare the candidate runtime-validated.

### Exact source and branch state

- Read the complete current handoff before continuing.
- Base diagnostic source remains experiment/csf-sync-wait64-csg-scope at 6634282aa14616931656b883aafb5519831eb888.
- New isolated branch: experiment/csf-notification-serialization.
- Latest candidate branch HEAD: 1a83d5f1ba1c2ff4053317593db3f89374a4c231.
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

Commits on experiment/csf-notification-serialization:

- d3051a808321241b96619832203a07f02d7ac4b2 — serialize notification poll/read and add nonblocking notification draining; add Kbase device-status checks.
- 88310bab8c529fdbb9ba0d25c34fff77e5738bd6 — propagate the shared CSF error latch to all subqueue waiters.
- 88b583c320ba07aca5a98bfd5409ea1f9345cf1d — prioritize cell->error over seqno completion.
- 1a83d5f1ba1c2ff4053317593db3f89374a4c231 — keep blocking notification polling outside the event-consumer lock; recheck readiness under lock before read.

The candidate branch also adds itself to the Android-only experiment workflow branch filter.

### Build validation in progress

The following GitHub Actions runs were observed for the candidate branch before the latest lock-contention refinement:

- Android-only PanVK build, run 41674211319, commit 88b583c320ba07aca5a98bfd5409ea1f9345cf1d.
- Android NDK wrapper release/debugoptimized builds, run 41674211281, same commit.

These runs were still in progress at the last check and do not validate latest HEAD 1a83d5f1ba1c2ff4053317593db3f89374a4c231. Confirm that a new run is created for the latest HEAD; if not, trigger an explicit workflow dispatch or a normal Git push/contents commit on the candidate branch. Do not consider an earlier SHA sufficient.

### Important unresolved checks

- Runtime validation of the notification mutex and error propagation has not yet occurred on the Xiaomi 15T / Dimensity 8400 Ultra / Mali-G720 MC7.
- The original 14.0 FPS Tomb Raider result remains the last reported benchmark. The log did not establish a CSF fatal fault; the final window-destruction and Wine fence assertion may be normal shutdown. Do not infer an active-game crash from those final messages.
- The seqno cell is allocated with PAN_KMOD_BO_FLAG_CSF_EVENT. The Kbase conversion clears BASE_MEM_COHERENT_LOCAL for that flag. Arm's CSF UAPI says CSF event memory uses an uncached GPU mapping when outer-shareable coherence is unavailable, so do not blindly add GPU_UNCACHED or change cache policy. The exact device's raw coherency mode and the effective allocation flags returned by its kernel still need diagnostic logging or device evidence.
- A separate imported-sync-file timeout issue remains to inspect: kbase_sync_file_wait_func() uses millisecond poll() timeout conversion, which floors sub-millisecond remaining time to zero and can return early. Keep this separate from the notification patch until verified.
- kbase_cpu_sync_wait_many() wait-any currently polls with zero-timeout waits and sched_yield(); this is a potential CPU-overhead optimization target, not a correctness fix yet.
- The public Mali-G715 r38p1 report in MineGraph-bilibili/panvk-Android issue #3 found missing SYNC_WAIT scope encoding, but the CSG-scope change did not eliminate its 0x88 faults; the report suspects firmware/microcode internal atomics and proposes LOAD + conditional branch as a decisive experiment. That is a different GPU/kernel report. Do not copy its scope workaround blindly onto G720. Preserve SYSTEM scope for the cross-CSG waits already documented above.

### Next actions

1. Wait for the latest Android-only and wrapper CI runs to finish; inspect failed job logs if either fails.
2. Review the final commit diff and build artifact provenance. Do not test an artifact built from an earlier SHA.
3. If both builds pass, prepare a separately named candidate package from the exact latest HEAD; retain the existing verified package and System Driver fallback.
4. Only then ask the user to test the same Tomb Raider workload. Prefer capturing logs during active gameplay and separately at shutdown, noting the exact manual-close time.
5. Add diagnostic logging for raw Kbase coherency mode and effective BASE_MEM_CSF_EVENT allocation flags before making any memory-attribute or cache-policy change.
