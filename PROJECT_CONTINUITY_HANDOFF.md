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
