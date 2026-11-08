# PanVK G720 project continuity handoff
Last updated: 2026-11-08 19:05 UTC

## Authoritative current state — 2026-11-08

This section supersedes older "next steps", queued-CI, and "no code changed" statements in the historical addenda below. Keep the historical record, but use the branch-specific state here when deciding what is current.

- Canonical continuity branch: `docs/project-continuity-handoff-2026-11-03`. It remains the only branch containing the root handoff file. Its pre-this-pass HEAD was `2ead3df163443f8234ea0fead043de2f8c21fddb`; that documentation-only commit triggered two wrapper-only CI runs, recorded in the 2026-11-08 12:42 UTC addendum. This update uses `[skip ci]`.
- Current task branch: `experiment/g720-performance-audit`. Its pre-this-pass HEAD was `6d1ed6db4c0ef050a516ce47de2c1991a30b2e90`; it is documentation-only beyond driver-source commit `a2e5ac142b4f0df71cb28204ece4337bace046de`. The latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`; Android-only run `42215758218` and wrapper run `42215758167` both succeeded. The audit refresh for this pass is commit `de47624438c6951437855dfd66c65b526fdf0b51`.
- Separate 4x heap-threshold candidate: `experiment/g720-heap-renew-262k`, HEAD `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`. It changes the compiled default from 65,536 to 262,144; it is not an environment-variable propagation test.
- Separate descriptor-heap implementation: `experiment/g720-descriptor-heaps`, HEAD `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`. Its exact-head Android and wrapper builds now pass; the package is independently audited below. No device runtime test has been performed.
- Separate clip/cull work: `experiment/g720-clip-cull-capabilities`, HEAD `9188e7bf92a032431ca16ac02d54d81be5a5e12e`. Android, wrapper, and NIR compiler-test workflows pass; feature exposure remains disabled pending full stage-path validation.
- Separate fragment-shading-rate work: `experiment/g720-fragment-shading-rate`, HEAD `157418c71475b9d96170a43f24157a80673d4160`; driver-source commit `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`. Android package, Android wrapper variants, and the Windows VRS capability probe all pass CI; package and probe artifacts were independently inspected. No device runtime test has been performed.

- Current VRS continuation: `experiment/g720-dynamic-fsr-sysval` is at `c027d4d8c747787c85256cfc0e9768e991262c16`. The v12+ DCD `Disable VRS Clamp 2x2` bit (bit 12) was enabled in the regular graphics DCD and internal fullscreen/frame-shader DCD paths. This fix is now **runtime validated on the target Mali-G720 MC7** by the complete user-supplied `panvk_vrs_render_probe.log` (SHA-256 `846cdb67b7dcae4b6f70f597bc8b02e877433198842d29464970642aca869b75`). Static KEEP rates all pass with exact expected SSBO invocation counts and directional pixel signatures: 1x1=4096; 2x1=2048 (H=2048,V=0); 1x2=2048 (H=0,V=2048); 2x2=1024 (H=2048,V=2048); 4x2=512 (H=3072,V=2048); 2x4=512 (H=2048,V=3072); 4x4=256 (H=3072,V=3072). Dynamic command-buffer cases shown in the log for 2x1, 1x2, 4x2, 2x4, and 4x4 match their static invocation counts and checksums. Pipeline creation, submit, fence wait, and readback pass. The log ends with `advertised rate matrix: PASS` and `VRS GPU RENDER PASS`. This establishes pipeline VRS rasterization/invocation behavior for the tested simple shader and target; it does not yet validate derivative/implicit-LOD behavior, interpolation, helper invocations, discard/depth/sample interactions, attachment/primitive VRS, or game performance. Candidate Android package was run `42509835857`, artifact `13628779694`, inner package SHA-256 `5ea2bb16e45cf4591a56786cdd531c41f454c6429103b3b48352aa0d8982c161`; matching Win64 probe artifact `13628887002` from run `42509380933`. Keep this VRS work separate from the heap-threshold performance-audit experiment. Next engineering step: audit the exact VRS feature/property advertisement and shader lowering paths, then extend the probe with shader-semantic tests before any production-safety or performance claim.
- These branches are parallel experiments. Do not merge or substitute their packages for one another without an explicit compatibility review.

### Current implementation state

The performance-audit branch already contains opt-in Kbase, descriptor-pool, and transient-BO telemetry; descriptor-set allocation failures are split into overflow, capacity, and VMA-allocation causes; and the Kbase tiler-heap helper logs its effective renewal-work threshold once. These diagnostics remain disabled by default. No allocator policy, heap topology, queue-ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver change was made in this update.

The threshold helper uses `debug_get_num_option("PANVK_KBASE_HEAP_RENEW_WORK", 65536)`. A positive environment value overrides the 65,536 compiled default; non-positive values disable work-triggered renewal. The log reports the effective value, not its origin. Therefore a 262,144 log from the separate 262K-default build does **not** prove that Winlator passed an environment variable. Use the verified 65K-default package for that proof: with the variable absent it must log 65,536; with `PANVK_KBASE_HEAP_RENEW_WORK=262144` it must log 262,144.

The VRS branch currently exposes pipeline fragment shading rate on v12, while primitive- and attachment-based rate features remain false. The Tomb Raider/DXVK 1.7.2 capture did not show the application enabling `VK_KHR_fragment_shading_rate`; do not treat Tomb Raider as a VRS performance benchmark. The descriptor-heap, clip/cull, and VRS packages are separate from the current heap-threshold test.

### Build and package status

- Performance-audit exact branch HEAD `fb70e9b5cf8b41806da4bc363c4f29d78caeaac8`: Android-only run `42082282882` SUCCESS; wrapper run `42082283389` SUCCESS in Release and Debugoptimized. Artifact `13442371778`, outer SHA-256 `3fc3eef74d546c11a1f1e799b38e4135fc935d25286c8b013cdfde5a897def6c`. Independently verified inner flat package SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`; four members, all member checksums valid, ELF64 AArch64/Android API 30, driver RUNPATH `$ORIGIN`, private `libpanvk_drm.so` dependency, no missing DRM symbols. This package has the 65,536 compiled default and is the current environment-override test package.
- Descriptor-heaps exact source `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`: Android-only run `42082198442` SUCCESS and wrapper run `42082198906` SUCCESS (Release and Debugoptimized). Artifact `13441682356`, outer SHA-256 `0a4f6b7899f3df42a89133f071b2c441e62739e8000ad55bdf14af7329fccf71`; inner package SHA-256 `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6`. Four-member flat package, all hashes valid, AArch64/API 30, RUNPATH and private-libdrm checks pass, missing-DRM-symbol report empty. Build/package success is not runtime validation.
- Clip/cull branch `9188e7bf92a032431ca16ac02d54d81be5a5e12e`: NIR tests run `42033527839` SUCCESS, Android-only run `42033527656` SUCCESS, wrapper run `42033527658` SUCCESS. These tests do not prove all GS/TCS/TES transport or runtime discard semantics.
- VRS branch `157418c71475b9d96170a43f24157a80673d4160`: Android-only run `42140194412` SUCCESS; wrapper/probe run `42140194467` SUCCESS for Release, Debugoptimized, and Windows x64 probe. Android artifact `13468166958`, outer SHA-256 `a7077179b08ae616abed1334ccb580583b89e0b79993206108df50ec793e60ca`; inner package SHA-256 `fa83005808ca2f7207008a9b2a5139f82f36de269c95c9da97784fc9d1350cbe`. Probe artifact `13467578263`, SHA-256 `618ca16d798d97ecc0ba3c316f1fbbca831578ac271adbc41790bd0bd761306f`. Both ZIPs and inner package hashes were verified; probe is PE32+ x86-64. It records capabilities and command-buffer recording but does not submit a draw.

### Benchmark and threshold status

The older `7.7 / 19.7 / 13.6 FPS` capture had profiling enabled and is diagnostic, not a clean performance baseline. The later documented clean 65,536-threshold result is `9.0 / 17.6 / 13.5 FPS`. The 262,144 candidate result was corrected by the user to `7.8 / 18.9 / 13.9 FPS` (the initially supplied 3.9 average was mathematically inconsistent). That candidate's startup log confirms effective threshold 262,144, but its branch compiled 262,144 as the default. The +0.4 average delta with a 1.2 FPS lower minimum is one-run, inconclusive evidence; do not promote it or claim an improvement.

No new Tomb Raider log archive was attached in this run. The current conversation file list contains the earlier `TombRaider_logs_2026-11-05_02-03-50.zip`; the later 02:33 and 02:42 results are recorded in the audit history but their archives were not newly supplied here.

### User action gate and exact next test

Use only the verified 65K-default package for the next controlled environment-variable test. Keep it imported as a separate Winlator Mali custom driver; never replace the Android system/vendor driver.

1. Baseline process: leave `PANVK_KBASE_HEAP_RENEW_WORK` absent. Restart the container/game process and verify the startup log says `tiler heap renewal work threshold=65536`.
2. Override process: set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144`, restart the container/game process, and verify the startup log says `tiler heap renewal work threshold=262144`. Because this binary's compiled default is 65,536, that log is direct evidence that Winlator passed the override.
3. If the override run logs 65,536 or the threshold log is missing, stop: the override test is invalid and no FPS comparison should be reported.
4. Keep `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` absent for both performance runs. Do not set them to `0`; leave them unset.
5. Use the same Tomb Raider benchmark scene, resolution, quality, DXVK 1.7.2, wrapper, and comparable thermal starting state. Record min/max/average, completion, visual correctness, stutter, OOM/device loss, and teardown separately. Prefer repeated A/B samples; retain the 65K package as rollback.

Do not change source or allocator policy until this test is complete. Never modify the phone's system/vendor Vulkan driver or kernel.

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


## 2026-11-06 continuation — VRS probe CI command fix

- Initial VRS probe workflow run `42138721988` failed in the shell/link stage before meaningful source compilation. The workflow's multi-line command encoded literal backslash arguments; MinGW linker reported `cannot find \\\\: No such file or directory`.
- This is a CI command formatting defect, not a driver build or Vulkan implementation failure.
- Fixed `.github/workflows/build.yml` to invoke the compiler as one explicit command line.
- Fix commit on `experiment/g720-fragment-shading-rate`: `157418c71475b9d96170a43f24157a80673d4160`.
- A fresh probe build result has not yet been observed. Do not distribute or test a probe executable until its artifact exists and is verified.
- Exact-head Android-only workflow `42138343360` remained in dependency installation at the last poll; host tools, Android ARM64 driver compile, dependency audit and package stages were still pending. No current importable package is available.
- The exact source driver commit remains `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`; the workflow-only fix does not change driver source.


### 2026-11-06 continuation — CI re-poll after probe command correction

A fresh GitHub Actions poll confirms the following state (this supersedes the older dependency-install status above):

- Exact-source wrapper run `42138343235` is now confirmed completed successfully in both Android ARM64 configurations: release and debugoptimized. The previously recorded artifact IDs and digests remain the successful wrapper artifacts for driver source SHA `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`. These are not the flat Winlator import package.
- Exact-source Android-only package run `42138343360` has progressed beyond dependency installation: its active step is now **Generate host tools**. No artifact exists yet; Android ARM64 libdrm build, PanVK Android-only compile/stage, ELF/dependency audits, flat package, and upload remain unconfirmed.
- The workflow-only probe fix commit `157418c71475b9d96170a43f24157a80673d4160` triggered fresh workflow runs: Android-only run `42140194412`, wrapper/probe run `42140194467`. Both are queued at the latest poll; no fresh probe artifact is available.
- A documentation-branch push also queued wrapper run `42140230712`; it is redundant for driver-source validation because it does not change the feature branch source. Prefer the feature-branch runs for the probe fix and exact-source package.
- Earlier run `42138669765` confirms the corrected driver source had a successful wrapper release job, while debugoptimized was still compiling at that earlier poll and the old probe compile job failed. Exact-source wrapper run `42138343235` is the authoritative successful pair.

**Current gate:** do not ask for device testing yet. Wait for run `42138343360` to complete all Android-only package audits and upload, and for run `42140194467` to produce a verified Windows probe artifact. Keep the phone's vendor/system Vulkan driver unchanged; any eventual test is through Winlator Mali 1.2's custom importer. No current VRS package is approved for installation.


### 2026-11-06 continuation — latest poll (00:57 UTC)

- Re-polled exact-head Android-only job `127632082794` in run `42138343360`: still active in **Generate host tools**; dependency installation is confirmed complete, no failed steps are reported, and all Android libdrm/driver/package steps remain pending.
- The GitHub job-log download endpoint returned `BlobNotFound` while the job is still running. This is an unavailable live-log archive, not a reported build failure. Continue using job-step status until the run completes and logs become downloadable.
- Fresh feature-branch runs remain queued: Android-only `42140194412` and wrapper/probe `42140194467`. No probe artifact has been uploaded.
- No code or driver-source change was made during this poll. Handoff updated to preserve the exact job IDs, states, and log limitation.


## 2026-11-06 — stock Vulkan extension inventory audited against PanVK

### User-provided stock-driver inventory

The user supplied the complete device-extension and instance-extension lists reported by the Xiaomi 15T stock MediaTek Vulkan driver. Treat these as the stock driver/firmware capability baseline, not as proof that each extension's optional feature bits are enabled and not as capabilities automatically implemented by PanVK.

Instance/device distinction recorded:
- Instance extensions concern instance creation, physical-device discovery, platform surfaces, and capability queries. They are not direct rendering performance switches.
- Device extensions add GPU/API functionality and must be checked against PanVK's extension table, feature query, property query, entrypoints, and actual backend/compiler implementation.

### PanVK source audit

Audited `src/panfrost/vulkan/panvk_vX_physical_device.c` on `experiment/g720-fragment-shading-rate` (driver source remains `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`; later `157418c...` only fixes the workflow command). Current v12 extension initializer contains 187 extension-table entries.

Already exposed by PanVK, including important gaming/translation features:
- `VK_EXT_descriptor_indexing` and project-specific `VK_EXT_descriptor_heap` (the latter is not in the user's stock list).
- `VK_KHR_buffer_device_address`, `VK_KHR_timeline_semaphore`, `VK_KHR_synchronization2`.
- `VK_KHR_dynamic_rendering`, `VK_KHR_dynamic_rendering_local_read`.
- `VK_KHR_fragment_shading_rate` on v12, with pipeline feature enabled; primitive and attachment features remain false in the current experiment.
- `VK_EXT_subgroup_size_control`, `VK_EXT_shader_subgroup_ballot`, `VK_EXT_shader_subgroup_vote`, `VK_EXT_shader_tile_image`, `VK_KHR_shader_integer_dot_product`, `VK_KHR_shader_float16_int8` and other shader features.
- `VK_EXT_pipeline_creation_cache_control`, `VK_EXT_pipeline_creation_feedback`, `VK_EXT_multisampled_render_to_single_sampled`, `VK_EXT_rasterization_order_attachment_access`.
- `VK_ARM_shader_core_builtins`, `VK_ARM_shader_core_properties`, and `VK_ARM_scheduling_controls`.
- Current PanVK source also exposes newer maintenance and other extensions not in the user's stock list; do not interpret the stock extension count as a target to match.

Relevant stock-listed device extensions missing from the current PanVK v12 extension table include:
- `VK_EXT_fragment_density_map` and `VK_EXT_fragment_density_map2`.
- `VK_EXT_image_compression_control` and `VK_EXT_image_compression_control_swapchain`.
- `VK_ARM_render_pass_striped`.
- `VK_KHR_cooperative_matrix`.
- `VK_KHR_acceleration_structure`, `VK_KHR_deferred_host_operations`, `VK_KHR_ray_query`, and `VK_KHR_ray_tracing_position_fetch`.
- Also absent are some diagnostics/feedback extensions such as `VK_EXT_frame_boundary` and `VK_EXT_subpass_merge_feedback`; these are not first-line FPS targets.

### Performance relevance / implementation triage

1. **Do not add extension-name flags alone.** Each extension needs the required Vulkan feature/property structs, entrypoints, format/image usage/layout behavior, synchronization and backend/compiler semantics. False advertising is likely to cause device-creation failures, CTS failures, or game crashes.
2. **Fragment density map** can reduce fragment invocations in selected regions (commonly foveated rendering/VR). It is not an automatic full-screen FPS boost. Khronos' sample describes it as a region-of-framebuffer density attachment; it also recommends considering KHR fragment shading rate. Our current pipeline-only FSR does not yet implement attachment-based rate control, so FDM is a separate substantial follow-on rather than a one-line exposure.
3. **VK_ARM_render_pass_striped is not a generic seven-core load-balancing switch.** The Khronos proposal's primary purpose is to split a render-pass instance into stripes and signal completion per stripe so an external/non-Vulkan consumer can begin post-processing before the entire image is complete. It should not be pursued as an assumed multi-core FPS optimization.
4. **VK_KHR_cooperative_matrix** exposes cooperative matrix operations, primarily useful for compute/matrix-multiply workloads. It is not expected to accelerate ordinary DXVK raster games unless a game/translation layer uses those operations and the Mali compiler has a native or efficient lowering path.
5. **Ray-query/acceleration-structure group** is for ray tracing. It is not a general performance extension and requires a full acceleration-structure build/update/traversal implementation, feature/property reporting, shader/SPIR-V/NIR support, memory/addressing, and synchronization. Do not expose it until that stack is implemented. The stock list alone does not prove a dedicated ray-tracing unit or the exact ray-query feature bits.
6. **Image compression control** may affect memory/bandwidth behavior only when the implementation supports the relevant compression modes and image formats. PanVK's existing AFBC path is not equivalent to this Vulkan extension; no safe exposure shortcut is justified.
7. **Highest-value current work remains FSR completion/validation and descriptor-heap correctness**, not indiscriminately enabling every stock extension. For the user's DX11 Tomb Raider/DXVK 1.7.2 benchmark, the log did not show FSR being enabled by the application, so pipeline FSR cannot be expected to raise that game's FPS. Keep it as a regression test.

Reference material reviewed:
- Khronos Fragment Density Map sample: https://docs.vulkan.org/samples/latest/samples/extensions/fragment_density_map/README.html
- Khronos VK_ARM_render_pass_striped proposal: https://docs.vulkan.org/features/latest/features/proposals/VK_ARM_render_pass_striped.html
- Khronos VK_KHR_cooperative_matrix proposal: https://docs.vulkan.org/features/latest/features/proposals/VK_KHR_cooperative_matrix.html
- Khronos VK_KHR_fragment_shading_rate reference: https://docs.vulkan.org/refpages/latest/refpages/source/VK_KHR_fragment_shading_rate.html

No extension flags or driver source were changed during this audit. Next implementation candidate should be selected based on real game/API use and the stock driver's feature/property bits, not merely the presence of an extension name.

### VRS package and probe CI completion

- Android-only package run `42138343360`, exact driver source SHA `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`: SUCCESS through host tools, Android ARM64 libdrm, Android-only PanVK compile/stage, ELF/dependency audit, flat Winlator/Adrenotools package and upload. Artifact ID `13467518179`, SHA-256 `ba19fb0927a8a9721d232aea0cd4e2dc1b2583a6059c61a7c6bad28fd485a19d`.
- Newer workflow-fix-commit package run `42140194412`, branch `experiment/g720-fragment-shading-rate`, workflow commit `157418c71475b9d96170a43f24157a80673d4160` (driver source unchanged): SUCCESS through all Android-only package stages. Artifact ID `13468166958`, SHA-256 `a7077179b08ae616abed1334ccb580583b89e0b79993206108df50ec793e60ca`. This is the preferred current Android-only package candidate, subject to final artifact inspection before user handoff.
- Corrected Windows x64 VRS probe job in run `42140194467`: SUCCESS; artifact ID `13467578263`, SHA-256 `618ca16d798d97ecc0ba3c316f1fbbca831578ac271adbc41790bd0bd761306f`. Probe performs extension/feature/property enumeration, device creation with pipeline FSR enabled, and records a 2x2 KEEP/KEEP command buffer; it does not submit a draw and is not a rendering/performance test.
- In the same run, Android wrapper Release job is SUCCESS; Debugoptimized job was still in dependency installation at the latest poll. Do not report the latter as passed until complete.
- No device testing has been requested yet in this update. Package and probe artifacts are available but should be checked together before asking the user to test.

## VRS device-probe result — 2026-11-06

User ran the Windows x64 VRS probe in Winlator Mali 1.2 and supplied `panvk_vrs_probe.log`. Exact output:

```
PanVK VRS probe: VK_KHR_fragment_shading_rate

Device 0: Wrapper(Mali-G720 MC7)
  API version: 1.3.354
  Driver: panvk (Mesa 26.3.0-devel (git-157418c714))
  VK_KHR_fragment_shading_rate: missing

VRS PROBE FAIL
```

Interpretation:
- The probe enumerated a PanVK device identified by the wrapper as Mali-G720 MC7 and driver build string `git-157418c714`, matching the VRS workflow snapshot.
- The loaded runtime does **not** enumerate `VK_KHR_fragment_shading_rate`. Thus the earlier CI success proved the probe executable built, not that the runtime driver exposed VRS. Pipeline VRS is not device-validated and no VRS rendering/performance test should be attempted yet.
- The source branch snapshot inspected after the result has `.KHR_fragment_shading_rate = PAN_ARCH >= 12` in `panvk_vX_physical_device.c`, and `.pipelineFragmentShadingRate = PAN_ARCH >= 12` while primitive/attachment features are false. The missing extension therefore needs root-cause investigation rather than assuming the feature bits or command recording are correct.
- Possible causes to discriminate: wrong/old Android custom-driver package actually imported; runtime PanVK architecture macro/compiled vX path not >=12; extension table registration/dispatch generation or build configuration; probe loading a different ICD/library than expected. The driver string alone does not prove the exact Android package artifact was imported.
- Next audit: verify which Android package is installed in Winlator and its hash/build identity; inspect the exact VRS build's generated extension/entrypoint registration and per-arch compilation; add probe diagnostics for physical-device properties (vendorID, deviceID, driverID, driverVersion, deviceName, API version) and enumerate the full extension list. Do not change extension exposure to unconditional true as a workaround.
- User is not yet asked for another test. First resolve the source/build/runtime discrepancy and produce a corrected Android package plus probe.

## VRS missing-extension follow-up — 2026-11-06

Further source/build audit after the first failed Winlator probe:

- The VRS source branch's `src/panfrost/vulkan/meson.build` explicitly generates device entrypoints with `--device-prefix panvk_v12` and builds the `panvk_v12` static library with `-DPAN_ARCH=12`. The Android-only CI log for run `42140194412` confirms compilation of `src/panfrost/vulkan/libpanvk_v12.a` objects. Therefore the v12 backend is present in the package build; this rules out the simple "v12 backend omitted from Meson" explanation.
- `panvk_macros.h` dispatches arch 12 to `panvk_v12_*`; arch 11 dispatches to `panvk_v10_*`. The physical-device kbase init accepts arch 11 and 12. If the kernel-reported GPU ID unexpectedly decodes as arch 11, the FSR gate will be false even if the marketing/device label says G720. This is a hypothesis only; Mali-G720 is documented as v12, and the device's actual `gpu_id` has not yet been logged.
- The initial Windows probe was too sparse: it logged only deviceName, API version, driverName/driverInfo, and whether the one extension was found. It did not log vendorID/deviceID, driverVersion, extension count, full extension list, or raw kernel GPU ID/decoded PanVK arch.
- Diagnostic commits on `experiment/g720-fragment-shading-rate`:
  - `9aa4bdcc1201d5cd2de08d68df104652bb6ca346`: kbase initialization logs raw `gpu_id`, decoded `arch`, and the supported-extension-table `KHR_fragment_shading_rate` bit immediately after architecture dispatch.
  - `5df8d3fb0243e1e3691b5b10ccaa1e73c180ab80`: expands `tools/panvk_vrs_probe_win.c` to log Vulkan vendorID/deviceID, driverVersion, extension count, and every enumerated device extension with specVersion.
  - The second commit is branch HEAD and has the first diagnostic commit as its parent (both changes are present).
- CI push run `42217026859` for diagnostic HEAD `5df8d3fb0243e1e3691b5b10ccaa1e73c180ab80` was observed in progress. Do not call it successful or ask for testing until the Android wrapper and relevant Android-only package jobs finish and artifacts are inspected.
- Next: inspect run `42217026859` and any paired Android-only workflow run. If only wrapper/probe artifacts are built, trigger the Android-only package workflow for the same exact HEAD. Once a new Android package is successfully built, user must import that exact package (not the previous artifact) and rerun the expanded probe. Correlate its full Vulkan extension list with the startup log's `gpu_id`, `arch`, and extension-table bit.
- Do not force the extension on, and do not change FSR feature semantics until this correlation identifies the fault.



## 2026-11-06 continuation — full handoff re-read and VRS enumeration-path audit

At the user's request, re-read the canonical `PROJECT_CONTINUITY_HANDOFF.md` from the first section through the latest VRS diagnostic entries, including the descriptor-heap, capability-census, clip/cull, WSI, and VRS history. The canonical file remains on `docs/project-continuity-handoff-2026-11-03`; no duplicate handoff was created on experiment branches.

### Current VRS source facts re-verified

Working source branch: `experiment/g720-fragment-shading-rate`.
Current branch HEAD inspected: `5df8d3fb0243e1e3691b5b10ccaa1e73c180ab80`.

- `panvk_vX_physical_device.c::get_physical_device_extensions()` explicitly sets `.KHR_fragment_shading_rate = PAN_ARCH >= 12`.
- The matching feature initializer explicitly sets `.pipelineFragmentShadingRate = PAN_ARCH >= 12`, with primitive and attachment FSR false.
- The v12 physical-device property initializer reports the conservative pipeline-FSR limits/rates recorded above.
- Meson generates a `panvk_v12` entrypoint prefix and builds the `panvk_v12` static library with `-DPAN_ARCH=12`; the Android-only CI log from run `42140194412` confirmed that v12 objects were compiled. The v12 backend is not simply omitted from the build.
- `panvk_physical_device_init_kbase()` computes `arch = pan_arch(device->kmod.dev->props.gpu_id)`, dispatches the architecture-specific extension table, and now logs raw `gpu_id`, decoded `arch`, and the resulting `KHR_fragment_shading_rate` bit immediately before `vk_physical_device_init()`. This is the decisive next diagnostic.
- Kbase initialization accepts architecture 11 as well as 12/13, while the FSR gate is specifically >=12. Therefore the raw GPU ID and decoded architecture must be observed; the Winlator marketing label alone cannot settle this.

### Wrapper pass-through audit

Inspected the public Winlator/GameNative wrapper source in `FristOneRR-Admin/FristOneRR-Wrapperv1`, branch `fristonerr-wrapper-v1`:
- `include/vulkan/vulkan_core.h` contains `VK_KHR_fragment_shading_rate`.
- `wrapper_setup_device_extensions()` queries the underlying ICD's device-extension list and copies each known extension into the wrapper's supported-extension table unless that extension is explicitly in `wrapper_filter_extensions`.
- The checked default filter table contains HDR metadata, Google display timing, shared-presentable-image, and image-compression-control-swapchain; it does **not** filter `VK_KHR_fragment_shading_rate`.
- `wrapper_EnumerateDeviceExtensionProperties` returns the wrapper's common supported-extension table. Thus the inspected wrapper source has no intentional VRS blacklist. If PanVK's new startup diagnostic reports the extension bit as true but the probe's wrapper-visible list omits it, the next step is to verify the exact wrapper build/source and inspect its underlying extension enumeration/blacklist environment rather than changing PanVK's feature gate blindly.
- The wrapper's `WRAPPER_EXTENSION_BLACKLIST` environment option can remove extensions dynamically; the user's previous captured environment did not list it, but confirm only if the PanVK startup bit is true and wrapper enumeration still omits VRS.
- The wrapper registry header contains the extension macro, so a missing macro in that checked source revision is not the explanation.

### Latest diagnostic CI state (polled 2026-11-06 13:24 CET)

- Android-only PanVK diagnostic package run `42217026789`: host tools, Android ARM64 libdrm, configure all succeeded; Android compile/stage was still in progress. ELF/dependency/package audits and artifact upload had not started.
- Wrapper/probe run `42217026859`:
  - Windows probe job: MinGW installation still in progress; compile/upload pending.
  - Android wrapper Release: SUCCESS, including compile and artifact upload.
  - Android wrapper Debugoptimized: compile in progress.
- The Android-only diagnostic source contains the Kbase GPU-ID/architecture/extension-bit log. The Windows probe source at branch HEAD `5df8d3f...` prints vendorID/deviceID, driverVersion, extension enumeration result/count, and the complete wrapper-visible device extension list.
- No new artifact is available yet. Do not ask the user to run another probe until the Android package containing the diagnostic log and the expanded Windows probe artifact are both complete and independently inspected.

### Decision

No runtime source or feature-gate change was made during this audit. Do not force-enable VRS. The next evidence correlation is:
1. PanVK startup: raw GPU ID, decoded architecture, and supported-extension-table bit.
2. Probe: vendor/device IDs, driver identity, extension count, and full extension list.
3. If raw architecture is 11, investigate GPU-ID decoding/device identification and whether the kernel reports the expected product ID before changing any gate.
4. If architecture is 12 and PanVK's table bit is 1 but the wrapper list omits the extension, investigate wrapper enumeration, extension blacklist, and the actual wrapper build.
5. If architecture is 12 but the table bit is 0, inspect dispatch/compiled per-architecture object selection and the exact loaded package.

Keep the custom PanVK ICD isolated in Winlator Mali 1.2. Never replace the Android vendor/system Vulkan driver.

## 2026-11-06 12:25 UTC — scheduled GitHub build and artifact monitoring

### Exact branch and workflow snapshot

- Active source branch: `experiment/g720-performance-audit`, HEAD `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`.
- The HEAD commit changes only `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`; it is not a driver-source change. Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- The latest matching Android-only and Android-wrapper runs both built the exact `b17b98...` HEAD. No workflow was manually dispatched and no source code was modified during this monitoring pass.

### Exact-head workflow results

- Android-only run [`42215758218`](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218): **completed / success**. Job `127885224447` (`build-android-only`) completed successfully. Every substantive step passed: checkout, checkout verification, Python setup, dependency installation, host-tool generation, Android ARM64 libdrm build, Android-only Meson configure, compile/stage, driver/manifest/dependency/staged-file audit, flat Winlator/Adrenotools package creation, and artifact upload. Setup and post-job cleanup steps also completed successfully. No pending or failed steps.
- Android-wrapper run [`42215758167`](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167): **completed / success**.
  - Release job `127885224388` (`build-android-wrapper (release)`): success. Checkout, memfd_create compatibility patch, Python setup, build-dependency installation, host-tool generation, Meson configure, compile, and artifact upload all passed; setup/cleanup also completed.
  - Debugoptimized job `127885224836` (`build-android-wrapper (debugoptimized)`): success. The same substantive steps all passed; setup/cleanup also completed.
- No first actionable CI failure exists for this exact HEAD. Logs were not needed to diagnose a failure because all three jobs and their steps completed successfully.

### Android-only package validation

- Workflow artifact `13501237590`, `panvk-g720-android-only-experimental-package`, downloaded and checked. Outer ZIP size: 16,753,940 bytes. Outer SHA-256: `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`. ZIP integrity test passed.
- The outer artifact contains the staged install, audit reports, and inner `PanVK-G720-experimental.zip`. Inner package SHA-256: `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; ZIP integrity test passed.
- Inner package has exactly four root members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three member hashes verified:
  - driver: `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`
  - private libdrm: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
  - metadata: `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`
- Metadata identifies `PanVK Mali G720 (experimental)`, package version `26.3.0-devel-android-kbase-b17b98cef92f`, driver version `26.3.0-devel`, and minimum Android API 30. It explicitly states the driver is not Vulkan conformant and device/game compatibility is unverified.
- Both package libraries are ELF64 little-endian AArch64 Android API 30 binaries built with NDK r30. Driver SONAME is `libvulkan_panfrost.so`; RUNPATH is `$ORIGIN`; its private dependency is `libpanvk_drm.so`. The private libdrm SONAME is `libpanvk_drm.so` and its only NEEDED dependency is Android `libc.so`. Driver Android dependencies are `liblog.so`, `libsync.so`, `libhardware.so`, `libnativewindow.so`, the bundled private libdrm, `libm.so`, `libz.so`, `libdl.so`, and `libc.so`.
- The workflow audit's `missing-drm-symbols.txt` is empty, confirming every DRM symbol imported by the driver is defined by the bundled libdrm. The staged ICD manifest reports Vulkan API `1.4.354`; it is included in the outer audit/stage artifact, while the flat inner importer package carries the Adrenotools `meta.json`.
- **State: artifact-validated at static/package level.** The inner ZIP structure, metadata, ELF architecture/API, RUNPATH, private dependency, and member hashes all pass. This is suitable to treat as a structurally validated experimental Winlator custom-driver package, not as a runtime-validated or conformant release.

### Android-wrapper artifact validation

- Release artifact `13502135385`, `libvulkan_wrapper-aarch64-release`: outer ZIP size 4,288,169 bytes; outer SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; one-member ZIP integrity passed. Extracted `libvulkan_panfrost.so` SHA-256: `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Debugoptimized artifact `13502061037`, `libvulkan_wrapper-aarch64-debugoptimized`: outer ZIP size 50,663,624 bytes; outer SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; one-member ZIP integrity passed. Extracted `libvulkan_panfrost.so` SHA-256: `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both wrapper libraries are ELF64 AArch64 Android API 30 binaries. Debugoptimized includes debug information. Both have RUNPATH `$ORIGIN/../../android_stub` and a `libdrm.so` dependency; the X11-enabled wrapper build also imports `libxcb`, `libX11-xcb`, `libxcb-dri3`, `libxcb-present`, `libxcb-xfixes`, `libxcb-sync`, `libxcb-randr`, `libxcb-shm`, and `libxshmfence` libraries.
- Each wrapper artifact contains only `libvulkan_panfrost.so`; neither contains a manifest, checksum file, companion `android_stub/libdrm.so`, or the X11 dependency set. Therefore these are **build-passed and ELF-checked raw wrapper libraries**, not independently package-validated/self-contained Winlator import packages. Runtime resolution of the stub and X11 dependencies remains dependent on the intended wrapper environment.

### Monitoring conclusion and next gate

- Current exact-head state: Android-only build **PASSED**; Android-wrapper Release **PASSED**; Android-wrapper Debugoptimized **PASSED**.
- Android-only ADPKG: **STATIC/PACKAGE VALIDATED**. Wrapper outputs: **RAW ELF VALIDATED ONLY**. No device runtime test was performed by CI or in this monitoring pass.
- No source code, build configuration, or workflow was changed. Both documentation updates use `[skip ci]` so the required audit trail does not trigger another build.
- Next runtime gate remains a user-controlled test of the Android-only package imported separately through Winlator Mali's custom-driver importer. Do not replace the phone's system/vendor Vulkan driver or modify the live kernel.



## 2026-11-06 — root cause found for missing VRS extension enumeration

A source-level root cause has been identified for the first Winlator VRS probe reporting `VK_KHR_fragment_shading_rate: missing`.

### Confirmed code path

1. The Android-only package workflow configures Mesa with `-Dplatform-sdk-version=30`.
2. Mesa's `meson.options` sets `android-strict=true` by default.
3. In `meson.build`, that option defines `ANDROID_STRICT`.
4. `src/vulkan/runtime/vk_physical_device.c::vk_common_EnumerateDeviceExtensionProperties()` filters every advertised extension through `vk_android_allowed_device_extensions` when `ANDROID_STRICT` is defined.
5. `src/vulkan/util/vk_extensions.py` explicitly whitelists `VK_KHR_fragment_shading_rate` only from Android API 31 (`ALLOWED_ANDROID_VERSION["VK_KHR_fragment_shading_rate"] = 31`).
6. Since this package was compiled with `ANDROID_API_LEVEL=30`, the generated allowlist bit is false and the common enumeration function hides the extension even when PanVK's architecture-specific extension table reports it as supported.

This exactly matches the symptom: source says `.KHR_fragment_shading_rate = PAN_ARCH >= 12`, but runtime extension enumeration says missing. It is a compile-time Android CTS allowlist filter, not evidence of unsupported G720 hardware or a broken VRS feature table.

### Winlator-only build correction

The Android-only workflow on `experiment/g720-fragment-shading-rate` was corrected:
- Keep `-Dplatform-sdk-version=30` and the existing Android API 30 minimum.
- Add `-Dandroid-strict=false` for this separate Winlator/Adrenotools custom ICD package.
- Rationale: this package is not installed as Android's system Vulkan HAL; it is loaded by Winlator's custom-driver wrapper. The Android CTS extension allowlist must not hide otherwise implemented Vulkan device extensions from the Winlator process.
- This disables the generated Android instance/device extension allowlist checks; it does not turn on any PanVK extension or feature bit. The PanVK extension table and feature/property implementation remain the source of advertised capabilities.

The workflow edit was verified in the committed file: `-Dplatform-sdk-version=30` remains and `-Dandroid-strict=false` is passed directly as a Meson option (comments are outside the shell command continuation).
- Workflow correction commit: `cf078c491c86b01c452542cde014439410277182`.
- The immediately preceding workflow-only edit `4a93b748...` was corrected before being treated as a valid build configuration; use `cf078c4...` as the authoritative workflow head.
- Driver implementation source still includes the VRS extension/feature gates and the Kbase startup diagnostic from commits `9aa4bdcc1201d5cd2de08d68df104652bb6ca346` and `5df8d3fb0243e1e3691b5b10ccaa1e73c180ab80`.

### Validation still required

- Build a fresh Android-only package from workflow head `cf078c4...`; do not use the previous strict/API-30 package.
- Retain the Windows probe artifact from the expanded-probe source commit `5df8d3f...` once its job completes.
- Import only the nested `PanVK-G720-experimental.zip` from the new artifact.
- In the new PanVK log, verify the raw Kbase `gpu_id`, decoded architecture, and supported-extension-table bit. Expected for a correctly identified G720 is v12 and bit 1.
- Run the expanded probe and confirm `VK_KHR_fragment_shading_rate` appears in the complete extension list, `pipelineFragmentShadingRate=true`, device creation succeeds, and dynamic command recording succeeds.

Do not yet claim VRS is device-validated. No FSR rendering draw has been submitted by the current probe. Keep the custom ICD isolated from the Android system/vendor Vulkan driver.



### Follow-up architecture cross-check

- `src/panfrost/model/pan_model.c` maps the G720 product family through `FIFTHGEN_MODEL(PAN_PROD_ID(12, 8, 0), ... "G720" ...)`, confirming the model-table architecture expected for a real G720 is v12.
- `src/panfrost/vulkan/panvk_macros.h::panvk_arch_dispatch` routes architecture 11 to the v10 implementation and architecture 12 to the v12 implementation. Therefore, if the Kbase diagnostic reports arch 11, the driver is not dispatching the G720 v12 implementation; investigate the raw GPU ID/model decoding rather than weakening the VRS gate.
- Mesa's public Panfrost driver documentation independently classifies G720 as 5th Gen (v12). This supports the expected classification but does not replace the device's raw Kbase diagnostic.
- The Android strict-filter diagnosis remains the primary explanation for the extension's absence because the build's API-30 allowlist condition is independently proven. The next build must disable strict filtering and still log/verify the actual Kbase architecture.



### CI poll after allowlist diagnosis (2026-11-06 13:30 CET)

- Earlier Android-only run `42217026789` completed successfully and uploaded artifact `13501453528` (`panvk-g720-android-only-experimental-package`, SHA-256 `15274e447777e4c3a35a2751ca28e4f87408c785b65951098f239008e107c4e8`). Its source SHA is `5df8d3fb0243e1e3691b5b10ccaa1e73c180ab80`; it was built before the `-Dandroid-strict=false` workflow correction and is **not** the package to test for the VRS enumeration fix.
- Wrapper/probe run `42217026859`: Android wrapper Release and Debugoptimized jobs both completed successfully. Artifacts: Release `13502460833` (SHA-256 `5b3863a7889f1908165ee913f0d266eed19199994b30c19db49c649aadf232fb`), Debugoptimized `13502630575` (SHA-256 `fe077345b532ece9a6fcae5c6c22ee97f46219a01fb17f29be203c049bd01a69`). The Windows probe job remained in progress at MinGW installation; no probe artifact yet.
- The authoritative corrected workflow file is on source branch head `cf078c491c86b01c452542cde014439410277182`. Its Android-only job must be allowed to complete before choosing a package. The earlier strict/API-30 package is explicitly superseded for this diagnostic.



### Wrapper-side Android strict-filter risk (must not overlook)

A second possible filtering layer was identified while checking the exact GameNative/FristOneRR wrapper source:
- Its `wrapper_setup_device_extensions()` copies underlying ICD extension names into the wrapper's own supported table and does not explicitly blacklist `VK_KHR_fragment_shading_rate`.
- Its `WRAPPER_EnumerateDeviceExtensionProperties` calls `vk_common_EnumerateDeviceExtensionProperties()`.
- That common runtime enumerator has the same `#ifdef ANDROID_STRICT` allowlist filter. Therefore, if the *installed Winlator wrapper binary* was built with `android-strict=true` and `ANDROID_API_LEVEL=30`, it could hide FSR even after the PanVK ICD itself is rebuilt with strict filtering disabled.
- The external wrapper's `android/mesa3d_cross.mk` passes `-Dplatform-sdk-version=$(PLATFORM_SDK_VERSION)` and allows `BOARD_MESA3D_MESON_ARGS`; it does not itself show an explicit `-Dandroid-strict=false`. The exact installed wrapper build's Meson args/API level have not been verified, so do not assert that the user's wrapper definitely filters FSR.
- The `libvulkan_wrapper-aarch64-release/debugoptimized` artifacts produced by this project's `.github/workflows/build.yml` are misleadingly named: that workflow actually uploads `build/src/panfrost/vulkan/libvulkan_panfrost.so`, i.e. alternate PanVK ICD builds, not the GameNative wrapper library. Do not tell the user these are wrapper binaries.
- After the corrected PanVK package is installed, compare PanVK's startup capability-audit line with the probe's extension list. If PanVK logs `KHR_fragment_shading_rate=1` but the wrapper-visible probe still omits FSR, then the installed wrapper is the remaining suspect. At that point, obtain/verify the exact wrapper build or produce a separate wrapper build with `-Dandroid-strict=false`; do not modify PanVK's hardware gate.

This distinction is now part of the diagnostic decision tree. The driver-side allowlist fix is necessary, but wrapper-side filtering remains an unverified possible second gate.



### Corrected-build CI run IDs (2026-11-06)

- Corrected Android-only PanVK package run: `42218076513`, run #189, source/workflow SHA `cf078c491c86b01c452542cde014439410277182`. This is the authoritative package build with `-Dandroid-strict=false`.
- Expanded Windows probe + alternate Android PanVK build run: `42218076595`, run #449, same source/workflow SHA. The Windows probe is the useful artifact from this run. The `libvulkan_wrapper-aarch64-*` artifacts from this job are actually `libvulkan_panfrost.so` alternate driver builds (as proven by `.github/workflows/build.yml` upload path), not loader/wrapper libraries.
- At the last poll both runs were still in progress. Android-only job was compiling/staging; the Windows probe was installing MinGW-w64; both alternate PanVK variants were compiling. Do not use artifacts from the pre-fix run `42217026789`.
- User testing remains blocked until run `42218076513` uploads the corrected PanVK package and run `42218076595` uploads `panvk-vrs-probe-win64`.



### Pipeline-FSR static/dynamic state source audit

While the corrected package builds, audited the pipeline-state plumbing to ensure the extension is not merely an enumeration stub:
- `src/vulkan/runtime/vk_graphics_state.h` contains `vk_fragment_shading_rate_state`, the FSR dynamic-state enum, and the `fsr` member in both dynamic and pipeline graphics state.
- `src/vulkan/runtime/vk_graphics_state.c` parses `VkPipelineFragmentShadingRateStateCreateInfoKHR` from pipeline `pNext`, initializes the fragment extent and both combiner operations, and defaults to 1x1 with KEEP/KEEP when the structure is absent. The generic `FOREACH_STATE_GROUP`/state-init machinery routes the FSR state through the same pipeline-state initialization/copy path.
- `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c` consumes `dyns->fsr` on v12+, converts the width/height to the hardware log2 VRS draw-rate encoding, writes both Mali VRS combiner fields into `PRIMITIVE_FLAGS_2`, and forces 1x1 KEEP/KEEP for sample-shaded fragment shaders.
- Therefore, the source path has both generic static pipeline FSR state and PanVK CSF draw-register programming. This is still source inspection only; device execution and output correctness remain unvalidated.



### Corrected PanVK package built and independently inspected (2026-11-06 13:35 CET)

- Android-only run `42218076513` completed successfully from source/workflow SHA `cf078c491c86b01c452542cde014439410277182`.
- Meson summary in the actual build log explicitly confirms `android-strict: false` and `platform-sdk-version: 30`. The build therefore retains Android API 30 minimum compatibility while disabling the CTS extension allowlist that hid FSR.
- Artifact ID: `13502283504`; artifact name `panvk-g720-android-only-experimental-package`; outer artifact SHA-256 `cf81c2c7697a273050df6b3fc9ff9ac7e778b95c75cf1e326685e3660d5aaaa0`.
- Nested Winlator package SHA-256: `ba54e040ed1dcd2c9f8148c74a2618f395085f477fc44d7806e67e01b67482b6`.
- Nested package was downloaded and inspected locally. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Metadata reports `packageVersion=26.3.0-devel-android-kbase-cf078c491c86`, `minApi=30`, and `libraryName=libvulkan_panfrost.so`.
- Embedded checksums verified against the extracted library files. The CI ELF/dependency audit passed, `audit/missing-drm-symbols.txt` is empty, and the staged ICD manifest resolves the expected library. The package is structurally valid and ready for Winlator import.
- Do not confuse this corrected package with artifact `13501453528` from pre-fix run `42217026789`; that earlier package had strict filtering enabled and is superseded.
- Expanded Windows probe job in run `42218076595` was still in progress at this poll; wait for its artifact before asking for device testing so the test uses the full extension-list/device-ID diagnostics.



### Diagnostic payload verification

Extracted the corrected nested package and ran `strings` on its `libvulkan_panfrost.so`. The binary contains the exact startup diagnostic format string:
`PanVK capability audit: gpu_id=0x%llx arch=v%u KHR_fragment_shading_rate=%u kbase=1`.
This confirms the GPU-ID/architecture/extension-bit diagnostic was linked into the artifact, not merely present in source.



### Nested package dynamic-link audit

Inspected the extracted nested ADPKG itself (not just the pre-patched staging ELF):
- `libvulkan_panfrost.so` has `DT_NEEDED: libpanvk_drm.so` and `RUNPATH: $ORIGIN`.
- `libpanvk_drm.so` has SONAME `libpanvk_drm.so` and depends only on `libc.so`.
- No `libdrm.so` dependency remains in the nested driver. The private libdrm ABI is packaged under a private SONAME to avoid interposing on the Android platform's libdrm.
- This confirms the flat ADPKG contains the complete private dependency pair and is relocatable as expected by the Winlator Mali 1.2 importer.



### Device-test handoff — corrected package ready (2026-11-06)

The first user test is now unblocked; the expanded probe job is optional for this first pass.
- Use Android-only artifact `13502283504` from run `42218076513` (corrected strict=false build), and import the nested `PanVK-G720-experimental.zip` only.
- The same Windows probe executable from run `42140194467` / artifact `13467578263` that generated the previous `panvk_vrs_probe.log` is sufficient for a first rerun: it checks extension presence, FSR feature/property enumeration, device creation with pipeline FSR enabled, and 2x2 KEEP/KEEP command-buffer recording.
- The expanded probe job in run `42218076595` remains stuck/in progress at the MinGW-w64 install step; it adds vendor/device IDs and full extension-list output. Keep monitoring, but do not block the first test on it.
- Ask the user to import the corrected custom driver into Winlator Mali 1.2, run the existing probe again, and send the new log. Do not replace the system/vendor driver.
- Do not start a game benchmark or claim FSR performance benefit until the probe passes. If the extension still appears missing, compare the PanVK startup capability-audit line (raw GPU ID, decoded arch, extension-table bit) with the wrapper-visible probe result; the installed wrapper's Android strict setting is still an open possibility.


---

## 2026-11-06 continuation — canonical handoff re-read and wrapper-side VRS filter identified

### Canonical continuity source verified

- Re-read the entire canonical `PROJECT_CONTINUITY_HANDOFF.md` on `docs/project-continuity-handoff-2026-11-03` from line 1 through the then-current final line 4842. The file remains the only canonical handoff; no experiment-branch copies were created.
- Canonical docs branch HEAD before this update: `b0ab753f7ae0da68245bbd110c3acc0effae2f46`.
- Re-read the related performance audit (`docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`, 1,022 lines) and capability matrix (`docs/G720_CAPABILITY_MATRIX_2026-11-05.md`, 687 lines) on `experiment/g720-performance-audit`.
- Performance-audit branch HEAD at this check: `d9394de49dc560fe2212a92b71280c372e66b0e6`; this is documentation-only, with driver-source commit still `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- The VRS work remains isolated on `experiment/g720-fragment-shading-rate`. Do not conflate its PanVK package with the performance-audit branch's 65K heap-threshold package or with the descriptor-heap branch.

### New source-confirmed second VRS enumeration filter

The corrected PanVK package has `-Dandroid-strict=false`, but the selected GameNative wrapper can independently filter the extension after querying the ICD. The public GameNative Mesa `wrapper-25` build path was audited at pinned branch commit `6bcfbaa701a8d1efa7f6c54145495eda5329f767`:

- `meson_options.txt` blob `5a452dad3c02b72b11ef89ac3a736cb3ef8de903` defaults `android-strict=true` and `platform-sdk-version=25`.
- `.github/scripts/build-wrapper-gamenative.sh` blob `1531d51b223ae276e1d58d6d832d41b4a84a5b52` sets `ANDROID_API=24` for NDK compilation and invokes Meson with `-Dvulkan-drivers=wrapper`, but does not override `android-strict` or `platform-sdk-version`. Thus its default Meson values apply.
- `meson.build` in that source sets `ANDROID_API_LEVEL` from `platform-sdk-version` and defines `ANDROID_STRICT` when `android-strict` is true.
- `src/vulkan/util/vk_extensions.py` blob on this pinned source sets `VK_KHR_fragment_shading_rate` minimum allowed Android API to 31.
- `src/vulkan/wrapper/wrapper_physical_device.c::wrapper_setup_device_extensions()` copies extensions reported by the underlying ICD into the wrapper's supported-extension table. Its `wrapper_EnumerateDeviceExtensionProperties()` then calls `vk_common_EnumerateDeviceExtensionProperties()`.
- `src/vulkan/runtime/vk_physical_device.c::vk_common_EnumerateDeviceExtensionProperties()` filters supported extensions through `vk_android_allowed_device_extensions` under `#ifdef ANDROID_STRICT`. Therefore a wrapper built with the public defaults (strict=true, SDK 25) will hide FSR even if PanVK advertises it and is itself built strict=false.
- The Winlator Mali 1.2 source branch `bionic-mali-1.2` confirms `ManageGraphicsDriversFragment.java` lists `wrapper-gamenative.tzst` as a separately updateable wrapper package. Its update flow requires the selected file's basename to match exactly and stores it in the app's private `graphics_driver` directory; the Remove/Reset action restores the bundled wrapper. This is separate from the Adrenotools PanVK ICD import and does not replace Android's system/vendor Vulkan driver.
- Qualification: this proves the behavior of the public GameNative `wrapper-25` build configuration and common runtime code. The exact provenance/build options of the binary currently bundled in the user's installed app have not been independently extracted. Treat this as the likely second gate, to be resolved by a controlled wrapper package build and runtime probe rather than claiming the installed binary's exact build flags are known.

### Dedicated custom GameNative wrapper build

Added a separate wrapper-only workflow to the VRS branch; no PanVK runtime source or feature bit changed:
- Workflow file: `.github/workflows/build-wrapper-gamenative-vrs.yml`.
- Commit: `0b3ac80e49fb6e594d91de9f72a8fa48407fc961`.
- It checks out GameNative Mesa at pinned commit `6bcfbaa701a8d1efa7f6c54145495eda5329f767`, patches only the wrapper build's Meson invocation with `-Dandroid-strict=false`, preserves the existing Android SDK target (Meson SDK 25; NDK API 24), and packages the result under the exact Winlator Mali update filename `wrapper-gamenative.tzst`.
- The package adds a `version.txt` label identifying the strict-filter-disabled custom build. The workflow checks Meson reports `android-strict=false`, `platform-sdk-version=25`, verifies `-DANDROID_STRICT` is absent from the wrapper compile commands, checks the archive, and uploads the package plus SHA-256 and Meson configuration report.
- New wrapper build run: `42220780814`, job `127901980165`. At the latest poll it was still checking out the pinned GameNative source; no wrapper artifact or build success is claimed.
- The workflow-only commit also triggered unrelated PanVK Android-only and wrapper CI runs `42220781009` and `42220780797`; these do not validate the GameNative custom wrapper.

### VRS probe CI install correction

The expanded Windows probe workflow in run `42218076595` remains stuck at the broad `mingw-w64` meta-package install. To reduce the install set, `.github/workflows/build.yml` now installs only `gcc-mingw-w64-x86-64-posix`, which supplies the x86-64 Windows compiler needed by this probe:
- Workflow-only commit: `bfd2e522f52986024c7a8fc5eef2ec6ee89da12d`.
- New probe run: `42220312994`, job `127900417338`; at the latest poll it had not yet reached the MinGW install step.
- New Android-only run `42220313021` was also triggered by the workflow change. No new probe artifact is available yet.
- The earlier probe job's MinGW install is still running; do not count it as a completed probe build.

### Corrected device-test gate

The earlier instruction to test the corrected PanVK package immediately is superseded by the wrapper-side finding. Do not ask the user to rerun the probe against the unchanged GameNative wrapper yet.

Next sequence:
1. Finish and inspect the custom `wrapper-gamenative.tzst` build; require Meson strict=false proof, absence of `ANDROID_STRICT`, archive integrity, AArch64 ELF validation, and SHA-256.
2. Finish and inspect the expanded Windows probe artifact (full extension list, vendor/device/driver identity).
3. Only then ask the user to update the wrapper through Winlator Mali's Manage Graphics Drivers screen using the exact file `wrapper-gamenative.tzst`; explain that the app's Remove/Reset action restores its bundled wrapper.
4. Import the corrected PanVK nested `PanVK-G720-experimental.zip` from Android-only artifact `13502283504` if it is not already installed. Never replace the Android system/vendor Vulkan driver.
5. Run the probe and correlate its full extension list with the PanVK startup diagnostic `gpu_id`, decoded `arch`, and `KHR_fragment_shading_rate` extension-table bit. Expected G720 result is v12 and bit 1. Require extension presence, `pipelineFragmentShadingRate=true`, successful device creation, and successful dynamic command recording before attempting a rendering test.
6. Do not claim VRS rendering correctness or performance until a draw is submitted and validated. Tomb Raider/DXVK 1.7.2 remains a regression test only.

No new PanVK runtime source changes, no system-driver replacement, and no VRS performance claims were made in this continuation.


### Expanded Winlator probe built and independently inspected

The minimal MinGW package correction worked:
- Source/workflow commit: `bfd2e522f52986024c7a8fc5eef2ec6ee89da12d`.
- Wrapper/probe workflow run: `42220312994`; Windows probe job `127900417338`: **SUCCESS**.
- Artifact: `13503216545`, `panvk-vrs-probe-win64`, GitHub SHA-256 `4bb3b7a95dcead9ead19061e04cc1de596e06c09a55ba2ebfb1e49a96efabceb`.
- Downloaded and inspected the artifact ZIP: it contains one file, `panvk-vrs-probe.exe`, 274,278 bytes. The executable is PE32+ x86-64; extracted EXE SHA-256 is `4f8462e3d70b960b4a36cc16717a16ba57d7994088b09e712e359b29ec4fb1ca`.
- Binary strings confirm the expanded diagnostics are present: Vulkan vendorID/deviceID, driverVersion, device extension enumeration result/count, and the complete extension-list output. This is a diagnostic probe, not a rendering test.
- Android wrapper Release and Debugoptimized jobs in the same run were still compiling at the last poll; they are not required for the Windows probe result.
- The older run `42218076595` remains stalled at the broad `mingw-w64` package install; use the new probe artifact, not a probe from that old job.

### Latest custom-wrapper build status

- The dedicated GameNative strict-filter-disabled wrapper workflow is run `42220780814`, job `127901980165`, on source/workflow commit `0b3ac80e49fb6e594d91de9f72a8fa48407fc961`.
- Pinned GameNative source checkout and dependency installation completed. At the latest poll, the job had advanced to **Build and package GameNative wrapper**; strict=false Meson verification and artifact upload were still pending.
- No custom `wrapper-gamenative.tzst` artifact exists yet. Do not ask the user to replace the wrapper until this job passes and the package is downloaded/verified.


### Correction: GameNative wrapper does not apply Android strict extension filtering

A deeper check of the actual Meson platform-selection logic invalidates the preceding wrapper-side allowlist hypothesis. This correction supersedes the section above that described the GameNative wrapper as a likely second Android-strict filter.

- GameNative `meson.build` on pinned commit `6bcfbaa701a8d1efa7f6c54145495eda5329f767` defines `with_platform_android = _platforms.contains('android')` (line 411).
- The actual `build-wrapper-gamenative.sh` Meson invocation explicitly uses `-Dplatforms=x11` (line 93), not `-Dplatforms=android`.
- The `ANDROID_API_LEVEL` and `ANDROID_STRICT` preprocessor definitions are emitted only inside `if with_platform_android` (Meson lines 870-896). Since this wrapper build has only X11 platform support, `ANDROID_STRICT` is **not defined** in its compilation, regardless of the option default `android-strict=true` or default `platform-sdk-version=25`.
- The wrapper's `vk_common_EnumerateDeviceExtensionProperties()` contains the Android allowlist filter only under `#ifdef ANDROID_STRICT`; the GameNative wrapper build described above therefore does not execute that filter. `wrapper_setup_device_extensions()` copies the underlying ICD extension table, then the common enumerator returns it without Android strict filtering.
- Thus the public GameNative `wrapper-25` build configuration does not explain a missing FSR extension. The earlier concern was based on the option defaults without following the `with_platform_android` gate. Do not rebuild or replace the wrapper for this reason.
- The temporary wrapper-only workflow `.github/workflows/build-wrapper-gamenative-vrs.yml` was removed from the VRS branch in commit `aedf0cabc17f4a9ed1febd8d93300e1f17751282`. Its already-started run `42220780814` may continue in GitHub Actions, but any resulting artifact is irrelevant and must **not** be imported. No wrapper replacement is part of the test plan.
- The prior handoff statement that the wrapper-side allowlist was an unverified possible second gate is now narrowed: it is not present in the audited public GameNative wrapper build path. The exact bundled binary's provenance remains unknown, but the source configuration does not support the proposed filter hypothesis.

### Updated immediate VRS validation plan

The corrected PanVK package and expanded probe are now the only artifacts needed:
- PanVK package: Android-only artifact `13502283504` from run `42218076513`; nested `PanVK-G720-experimental.zip`, SHA-256 `ba54e040ed1dcd2c9f8148c74a2618f395085f477fc44d7806e67e01b67482b6`. Meson confirmed `android-strict=false`, API 30 retained.
- Expanded Windows probe: artifact `13503216545` from run `42220312994`; PE32+ x86-64 executable, extracted SHA-256 `4f8462e3d70b960b4a36cc16717a16ba57d7994088b09e712e359b29ec4fb1ca`. It logs vendor/device IDs, driver version, complete device extension list, FSR feature/property/rate data, device creation and dynamic command recording.
- Keep the user's existing Winlator Mali 1.2 wrapper selection unchanged. Update only the custom PanVK driver through the app's custom-driver importer; do not replace the system/vendor Vulkan driver or the wrapper.
- Re-run the expanded probe and provide its `panvk_vrs_probe.log`. Correlate the full extension list with PanVK's startup `gpu_id`, decoded architecture, and `KHR_fragment_shading_rate` extension-table bit.
- If FSR remains missing, the probe's device/vendor/driver identity and PanVK startup diagnostic now let us isolate driver architecture detection, extension table state, loader/ICD selection, or a different wrapper binary without changing more components speculatively.



### Follow-up validation of the discarded wrapper workflow

- The temporary wrapper workflow run `42220780814` did complete the GameNative wrapper build and package step, including archive creation and AArch64 ELF verification. It failed only in the custom Meson-option parser: the parser selected the trailing words of the option description (`CTS` / `version.`) instead of the option values. No artifact was uploaded.
- This verifier failure does not indicate a wrapper compile failure. The workflow has already been deleted from the VRS branch; do not repair or rerun it because the corrected `with_platform_android` source audit proves that this X11 wrapper build does not define `ANDROID_STRICT` and does not apply the Android extension allowlist.
- The VRS branch now has no wrapper-replacement workflow. Keep the installed Winlator wrapper unchanged.

### Latest branch and artifact state after correction

- VRS branch latest workflow/source commit after removing the unnecessary wrapper workflow: `aedf0cabc17f4a9ed1febd8d93300e1f17751282`. No PanVK runtime source changed in this cleanup.
- Canonical handoff branch latest commit before this addendum: `a82c6aff08d432ee4160e14c2de2cb909d338e4b`.
- Corrected PanVK artifact `13502283504` (run `42218076513`, source/workflow SHA `cf078c491c86b01c452542cde014439410277182`) remains the valid API-30 / `android-strict=false` custom ICD package. Its nested ZIP checksum is `ba54e040ed1dcd2c9f8148c74a2618f395085f477fc44d7806e67e01b67482b6`.
- Expanded probe artifact `13503216545` (run `42220312994`, probe job `127900417338`) is complete and independently inspected. ZIP contains the PE32+ x86-64 `panvk-vrs-probe.exe`; executable SHA-256 `4f8462e3d70b960b4a36cc16717a16ba57d7994088b09e712e359b29ec4fb1ca`. It includes the full extension list and vendor/device/driver identity diagnostics.
- The Android-only rebuild run `42220313021` and the Release/Debugoptimized jobs in wrapper run `42220312994` were still in progress at the last poll. They are not required to validate the already-built corrected PanVK artifact `13502283504` or the completed Windows probe artifact `13503216545`.
- User device validation is now the only missing input. Ask for one probe run with the corrected PanVK package and the existing wrapper unchanged. Do not request a wrapper import, game benchmark, or VRS rendering benchmark yet.



### Final wrapper-side blacklist check

Inspected `src/vulkan/wrapper/wrapper_device.c` on the same pinned GameNative commit. Its `wrapper_filter_extensions` table contains only:
- `VK_EXT_hdr_metadata`
- `VK_GOOGLE_display_timing`
- `VK_KHR_shared_presentable_image`
- `VK_EXT_image_compression_control_swapchain`

It does not blacklist `VK_KHR_fragment_shading_rate`. Together with the `-Dplatforms=x11` / no-`ANDROID_STRICT` build path, there is no source-supported GameNative wrapper filter for FSR in this version. The wrapper rebuild was unnecessary and remains deleted.


### User-device VRS probe result: PASS (2026-11-06)

The user supplied `panvk_vrs_probe.log` from the corrected PanVK package and existing Winlator wrapper. The full log was read from the uploaded file and confirms:

- Physical device: `Wrapper(Mali-G720 MC7)`.
- Vulkan API: `1.3.354`.
- Vendor/device IDs: `0x13b5 / 0xc8700010`.
- Driver: `panvk (Mesa 26.3.0-devel (git-cf078c491c))`.
- Device extension enumeration succeeded, count 163.
- `VK_KHR_fragment_shading_rate` is present (spec version 2).
- `pipelineFragmentShadingRate=true`.
- `primitiveFragmentShadingRate=false`; `attachmentFragmentShadingRate=false`, as expected for this pipeline-only implementation.
- Reported properties: max fragment size 4x4, max aspect ratio 2, max coverage samples 16, max rasterization samples 4x.
- Enumerated rates: 4x4@1x; 4x2@1x; 2x4@1x/2x; 2x2, 2x1, 1x2@1x/2x/4x; 1x1 at all sample counts.
- `vkCreateDevice` with `pipelineFragmentShadingRate` enabled returned `VK_SUCCESS (0)`.
- `vkCmdSetFragmentShadingRateKHR` command recording and command-buffer end returned `VK_SUCCESS (0)`.
- Probe concludes `PASS: extension query, device creation, and dynamic FSR command recording` and `VRS PROBE PASS`.

Interpretation and limits:
- This proves that the tested custom PanVK ICD is active in the Winlator container, that FSR pipeline mode is visible to the application, that the feature can be enabled, and that the dynamic command path can be recorded without validation/API failure in this probe.
- The reported device name and device ID are consistent with the expected Mali-G720 MC7 target. The log itself does not include the PanVK startup diagnostic's raw kernel `gpu_id`, decoded `pan_arch`, or extension-table bit; therefore those exact internal values remain uncorrelated. The user-visible Vulkan identity and successful v12-gated FSR path are strong evidence but are not a substitute for that startup line.
- This is not yet proof of actual VRS rasterization behavior, visual correctness, shader-rate lowering correctness, or performance improvement. No game benchmark should be interpreted as a VRS result until an actual draw has been submitted with a non-1x1 rate and its output/performance compared.

Next engineering gate:
1. Build a minimal controlled render probe (or extend the existing probe) that creates a small offscreen color target and pipeline, submits real geometry at a deliberately coarse rate (e.g. 2x2), then reads back or otherwise verifies output. Include a 1x1 control path and ensure identical geometry/shaders/state except for the shading-rate state.
2. Verify the command is actually executed on the GPU, not merely recorded; capture submission/fence completion and output checksum/readback if supported by the current probe environment.
3. Only after the controlled rendering path passes, ask the user to run a short controlled test in Winlator. Then evaluate whether a DXVK title can use the feature and whether there is a measurable benefit. Do not claim that DXVK automatically uses this extension; the application/translation layer must opt into the relevant Vulkan functionality.
4. Keep primitive and attachment FSR disabled until independently implemented and validated. Do not expose additional unimplemented extensions just to match the stock MediaTek extension list.

No system/vendor driver replacement and no wrapper replacement were performed. The tested driver remains the separately imported custom PanVK package.


### VRS source audit and real-render probe implementation started (2026-11-06)

After the user's successful Winlator extension/device/command-recording probe, the VRS branch source was audited directly:

- `src/panfrost/genxml/v12.xml` defines `VRS Draw Rate` as `(log2(width) << 2) | log2(height)`, with 1x1=0, 1x2=1, 2x1=4, 2x2=5, 4x4=10. Its `VRS Combiner` values are Keep=0, Replace=1, Min=2, Max=3, Mul=4.
- Vulkan's `VkFragmentShadingRateCombinerOpKHR` values are exactly 0..4 in the same order. The current direct enum cast is therefore consistent with the v12 GenXML enum.
- `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c` writes the encoded draw rate and both combiner values to `PRIMITIVE_FLAGS_2` through the CSF IDVS `TILER_FLAGS2` register when FSR state is dirty. It forces 1x1/KEEP/KEEP when sample shading is active.
- `src/panfrost/vulkan/panvk_vX_shader.c` decodes the fragment shading-rate system value using width bits [3:2] and height bits [1:0], consistent with the Vulkan SPIR-V fragment-rate encoding; it lowers the fragment-center/sample-position-or-center intrinsic when a pipeline may use VRS.
- `src/panfrost/vulkan/panvk_vX_physical_device.c` gates `VK_KHR_fragment_shading_rate` and `pipelineFragmentShadingRate` at `PAN_ARCH >= 12`, with primitive and attachment features disabled. Its advertised rates fit the declared 4x4 maximum, 16 coverage-sample limit and 4x rasterization-sample limit.
- No VRS register-encoding mismatch was found in this pass. The earlier idea of reversing width/height decode is explicitly not supported by the Vulkan SPIR-V encoding; do not change those bit positions.

A new Windows/Winlator offscreen render probe is being developed on `experiment/g720-fragment-shading-rate`:
- Added `tools/vrs_probe.vert` and `tools/vrs_probe.frag`. The fragment shader emits a coordinate-dependent color pattern based on `gl_FragCoord`, designed to distinguish per-pixel shading from 2x2 shared fragment shading.
- Added `tools/panvk_vrs_render_probe_win.c`. It dynamically loads the Winlator Vulkan loader, enables pipeline FSR, creates an offscreen RGBA8 target, builds two static graphics pipelines (1x1 control and 2x2 test), submits each draw, waits on a fence, copies the output to coherent host-visible memory, and compares checksums / equal-neighbor counts for the expected 2x2 block signature.
- Added `.github/workflows/build-vrs-render-probe.yml` to compile GLSL to SPIR-V, embed the SPIR-V arrays, cross-compile the Windows executable with MinGW, and upload a standalone diagnostic artifact.
- The source was refined to use a per-render framebuffer/target, require coherent host readback memory, explicitly set `basePipelineIndex=-1`, and wait for device idle before cleaning up after a failed post-submit fence wait.
- Latest VRS experiment branch commit at this point: `d7ff112be106941a7bb10d45b513b0f011d8cc2a`.
- **Build status is not yet verified.** Do not ask the user to run or import this probe until CI succeeds, the executable is inspected, and the artifact is downloaded and hashed.
- The probe's pass criterion is intentionally stronger than command recording: both 1x1 and 2x2 draws must submit, complete, and read back; the 2x2 output must differ from 1x1 and show a substantial increase in equal adjacent pixel pairs. A failure is diagnostic, not automatically proof that the GPU lacks VRS; first inspect the exact Vulkan result and readback signatures.

Next work: finish static review and compile the new probe in CI. Then inspect the artifact and only request one controlled Winlator run if it is valid. No PanVK runtime code was changed in this probe-harness step, and no driver or wrapper package needs replacing.


### VRS combiner capability correction and extended GPU test (2026-11-06)

A second pass over the Vulkan properties exposed one conservative setting that did not match the v12 hardware register definitions:

- The v12 GenXML `VRS Combiner` enum explicitly defines `Keep=0`, `Replace=1`, `Min=2`, `Max=3`, and `Mul=4`.
- PanVK's draw path writes the Vulkan combiner enum values directly into the two hardware combiner fields; the enum values match exactly.
- The physical-device properties had nevertheless reported `fragmentShadingRateNonTrivialCombinerOps=false`, which tells Vulkan applications that MIN/MAX/MUL are unavailable. This is inconsistent with the v12 register interface and unnecessarily constrains translation layers that need nontrivial D3D12 VRS combiners.
- Updated `src/panfrost/vulkan/panvk_vX_physical_device.c` to report `fragmentShadingRateNonTrivialCombinerOps = PAN_ARCH >= 12`. `fragmentShadingRateStrictMultiplyCombiner` remains false for now; the extended render probe will compare actual MUL output with the reported property before this is treated as hardware-verified. No primitive or attachment FSR feature was enabled.
- Source change commit: `1991b323ddb3ccc3ac8f71d4c6b65bf395ce8d35`.

The render probe was extended to query and log `fragmentShadingRateNonTrivialCombinerOps` and `fragmentShadingRateStrictMultiplyCombiner`, then render/read back five cases:
1. 1x1 with KEEP/KEEP (fine-rate control).
2. 2x2 with KEEP/KEEP.
3. 2x2 with MAX/KEEP (should preserve 2x2 against the default 1x1 primitive rate).
4. 2x2 with MIN/KEEP (should reduce to 1x1).
5. 1x1 with MUL/KEEP (distinguishes strict vs non-strict multiply behavior according to the queried property).

The probe checks GPU submission/fence/readback, the expected 2x2 repeated-pixel signature, and output equivalence for MIN/MAX/MUL against the property value. It now queries FSR properties only after confirming the extension is present.

Latest probe source commit: `12379968b4eca4ed074c2c72b9d8d764851cd5e4` on `experiment/g720-fragment-shading-rate`. The CI build and artifact inspection are still pending. Do not give the user a download or request a device run until the executable has compiled, been inspected, and the matching Android ICD build containing the combiner-property correction has succeeded.


### Current VRS probe build queue and latest source checkpoint (2026-11-06)

The expanded render probe has been statically reviewed and corrected through several source passes:
- Framebuffer/image ownership is now per-render invocation, so each 1x1/2x2/combiner case reads back the exact image attached to its framebuffer.
- Host readback requires HOST_VISIBLE + HOST_COHERENT memory; no non-coherent read is accepted without an invalidate path.
- A failed fence wait after a successful submit attempts `vkDeviceWaitIdle` before releasing resources.
- The main path checks extension support before chaining/querying `VkPhysicalDeviceFragmentShadingRatePropertiesKHR`.
- The render probe exercises KEEP, MAX, MIN, and MUL combiners with static pipeline FSR state. The 1x1 MUL case is compared against `fragmentShadingRateStrictMultiplyCombiner`.
- The Windows source includes a coordinate-dependent fragment shader and embedded-SPIR-V build step. No driver code beyond the nontrivial-combiner property correction was changed.

Latest source commit: `10472e2c0c22a72dd4acd32d67ee145953b3d555` on `experiment/g720-fragment-shading-rate`.

GitHub Actions runs observed for this source checkpoint:
- Android NDK build (wrapper variant): run `42224083920`.
- Android-only PanVK experiment: run `42224083693`.
- Winlator VRS render probe: run `42224083881`.
- All three were **queued** at the last poll. Earlier runs from intermediate commits were also queued; only the latest matching-source artifacts should be considered if they complete. The final source checkpoint is not yet compile-verified and no render-probe artifact is ready to distribute.

Next: poll these exact runs, inspect the Windows build logs and Android build artifacts, and correct any compile/runtime issues before asking for a device test. The previously tested package `cf078c491c86b01c452542cde014439410277182` predates the nontrivial-combiner property correction and must not be reused to validate the updated combiner behavior.


### VRS render-probe static audit while CI is queued (2026-11-06)

A further static review was completed against the exact VRS source checkpoint `10472e2c0c22a72dd4acd32d67ee145953b3d555`:

- Reviewed the complete `tools/panvk_vrs_render_probe_win.c` render path and both GLSL sources. The fragment shader writes a coordinate-dependent 8x8 color pattern, so 2x2 fragment shading should create repeated adjacent-pixel pairs in the offscreen RGBA8 readback.
- Confirmed the render pass transitions the color target to `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`, then copies to a host-visible, host-coherent staging buffer. A transfer-write-to-host-read buffer barrier and fence wait precede CPU readback.
- Confirmed the probe's 1x1 MUL/KEEP case is intentionally useful for distinguishing the strict-multiply property: both pipeline and default primitive rates are 1x1; strict multiplication leaves 1x1, while the legacy non-strict behavior combines 1 and 1 to produce 2 in that dimension. The probe compares its checksum against the 1x1 control when strict=true and against the 2x2 control when strict=false. This matches the Khronos Vulkan combiner/property definitions.
- The 2x2 MAX/KEEP case should preserve the pipeline rate against the default 1x1 primitive rate; 2x2 MIN/KEEP should reduce to 1x1. These output comparisons are appropriate for the pipeline-only path under test.
- No new source defect requiring an immediate edit was found in this pass. There is one cosmetic logging-format issue in the per-render "creating target" message (it may print a literal `\\n` rather than a line break); it does not affect Vulkan execution or the pass criteria and is deferred until after the current build checkpoint to avoid creating another redundant queued build.
- Latest exact-source CI runs remain queued: Windows render probe `42224083881`, Android NDK wrapper build `42224083920`, and Android-only PanVK build `42224083693`. An earlier Windows probe build for source `12379968b4eca4ed074c2c72b9d8d764851cd5e4` (run `42223802868`) is still in progress at the shader-tool installation step; it is not a substitute for the exact latest-source build.
- Repository-wide Actions snapshot showed 22 queued and 16 in-progress runs, consistent with runner contention. Do not push another source commit just to refresh the queue. Keep waiting for the three exact-source runs and only accept artifacts whose source SHA is `10472e2c0c22a72dd4acd32d67ee145953b3d555`.

No user-device action is required yet. The Android package and Windows render probe must both be built from the exact source checkpoint, inspected, and hashed before requesting the next Winlator test.


### Windows VRS render-probe CI success and artifact inspection (2026-11-06)

The exact latest source checkpoint `10472e2c0c22a72dd4acd32d67ee145953b3d555` has now produced a successful Windows render-probe build:

- Workflow run: `42224083881` — completed, conclusion `success`.
- All steps succeeded: dependency installation, GLSL-to-SPIR-V compilation, Windows probe compilation, executable verification, and artifact upload.
- Artifact: `panvk-vrs-render-probe-win64`, artifact ID `13505267008`, ZIP size 109,092 bytes.
- GitHub artifact SHA-256: `38ea69c729c3d4580d7c0e10e3e05a785157a5626c643574f04e48da8fef2ece`; independently downloaded ZIP hash matches exactly.
- Extracted executable: `panvk-vrs-render-probe.exe`, 267,872 bytes, PE32+ x86-64.
- Executable SHA-256: `7364b581bf9bf5b4ac810a0b88ed4d62a0bbce413f2034cb08e0c715c1038506`.
- Static binary inspection confirms only standard `KERNEL32.dll` and `msvcrt.dll` imports; Vulkan is dynamically loaded through `vulkan-1.dll` at runtime. Expected render-pass, combiner comparison and final PASS/FAIL diagnostic strings are present.
- This confirms the standalone probe builds and is packaged correctly. It does not yet establish GPU runtime success.

Matching Android PanVK build gate remains open:
- Android-only run `42224083693` is in progress at Configure Android-only build; no artifact yet.
- Android NDK wrapper run `42224083920` remains queued; no artifact yet.
- An older Windows build from source `12379968b4eca4ed074c2c72b9d8d764851cd5e4` is still in progress and is not needed because the exact latest-source Windows artifact succeeded.

Do not ask the user to test yet. Wait for the Android-only PanVK package from the exact same source SHA, inspect the package contents and runtime library dependencies, and hash it. Then provide both the Windows probe and matching custom PanVK package for one controlled Winlator run. Keep the existing Winlator wrapper unchanged and do not replace the Android system/vendor driver.


### Exact-source Android PanVK package and VRS render-probe artifacts verified (2026-11-06)

The matching Android-only PanVK build and both Windows probe workflows have completed successfully for source commit `10472e2c0c22a72dd4acd32d67ee145953b3d555` on `experiment/g720-fragment-shading-rate`.

**Android-only PanVK build**
- Workflow run `42224083693`: completed, conclusion `success`; all 12 build/audit/package/upload steps succeeded.
- Artifact `panvk-g720-android-only-experimental-package`, ID `13506052377`, size 16,816,894 bytes.
- GitHub artifact ZIP SHA-256: `d53cab115d734b27bf7aa4f63d2fd307ef8b5d92f52a4a6d0e28a87694772adf`; independently downloaded archive hash matched.
- Inner Winlator/Adrenotools package: `PanVK-G720-experimental.zip`, 4,148,529 bytes; SHA-256 `bdf62c9dfdec8103119ada67b4d16188b5d03d11706ba8bdbaf006f273122880`.
- Flat package entries: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. Package-internal SHA256SUMS verification passed for all three payload files.
- Driver is ELF64 AArch64 Android (NDK r30, API 30); package metadata identifies version `26.3.0-devel-android-kbase-10472e2c0c22`, min API 30, and library `libvulkan_panfrost.so`.
- Packaging script rewrites the driver dependency from generic `libdrm.so` to private `libpanvk_drm.so`, sets SONAME accordingly, and sets RUNPATH to `$ORIGIN`. Independently inspected final inner package: driver NEEDED entry is `libpanvk_drm.so`, RUNPATH is `$ORIGIN`; private libdrm is AArch64, SONAME `libpanvk_drm.so`, and its only NEEDED library is Android `libc.so`. Thus the final flat package does not depend on the earlier staging-only `android_stub/libdrm.so` path and avoids collision with a platform libdrm.
- The staging audit's missing-DRM-symbol report is empty: all driver DRM undefined symbols are provided by the bundled Android-target libdrm. The package's own final linker layout was checked separately after packaging.
- Final driver binary SHA-256 from the flat package: `8aeb33f4dbc001733522e118d020a31d4d36e842a62d8ca31589507a4841007b` (the staged pre-package driver has a different hash because the packaging script applies SONAME/NEEDED/RUNPATH edits).
- No device runtime proof yet; static packaging and ELF dependency checks are clean.

**Windows VRS real-render probe**
- Workflow run `42224083881`: completed, conclusion `success`, exact same source SHA.
- Artifact `panvk-vrs-render-probe-win64`, ID `13505267008`, ZIP size 109,092 bytes.
- GitHub artifact ZIP SHA-256 `38ea69c729c3d4580d7c0e10e3e05a785157a5626c643574f04e48da8fef2ece`; independently downloaded hash matches.
- Executable `panvk-vrs-render-probe.exe`, 267,872 bytes, PE32+ x86-64; SHA-256 `7364b581bf9bf5b4ac810a0b88ed4d62a0bbce413f2034cb08e0c715c1038506`.
- Static import table contains only `KERNEL32.dll` and `msvcrt.dll`; Vulkan is dynamically loaded. The executable includes the full five-case offscreen draw/submit/fence/staging-readback probe, not merely the older extension-enumeration test.
- This confirms build and packaging, not actual GPU rendering. Runtime pass still requires execution in the same Winlator container after selecting the custom PanVK ICD.

**Additional wrapper workflow**
- Workflow run `42224083920`: completed, conclusion `success`; Winlator VRS probe plus Android NDK wrapper debugoptimized and release jobs all succeeded.
- Artifacts: `panvk-vrs-probe-win64` ID `13506636095`; `libvulkan_wrapper-aarch64-release` ID `13506587387`; `libvulkan_wrapper-aarch64-debugoptimized` ID `13505734013`.
- These wrapper outputs are not required for the controlled test; keep the user's existing Winlator wrapper unchanged to isolate PanVK behavior.

**User test gate is now open**
1. Import `PanVK-G720-experimental.zip` (the inner flat package) through Winlator Mali 1.2's custom driver importer. Do not install or replace the Android system/vendor Vulkan driver.
2. Keep the currently working Winlator wrapper, container, game settings, and other variables unchanged.
3. In that same container, run `panvk-vrs-render-probe.exe` from the matching Windows artifact and capture its complete console output. Ensure the custom PanVK driver is the selected ICD for the container before running it.
4. Send the full log, including the device/driver header and all five render-case checks. If the executable cannot start, send the exact error and any loader log instead.
5. Do not run a game benchmark yet. First determine whether real GPU rendering, fence completion, readback, 2x2 shading signature, and MIN/MAX/MUL comparisons pass. Only after this validation should VRS be considered hardware-confirmed or used for DXVK/Winlator performance experiments.

Canonical handoff updated before requesting device work. Preserve this branch and file as the single continuity source; do not create a duplicate handoff.


### First real VRS render log analysis and stage-diagnostic probe (2026-11-06)

User ran the exact-source VRS probe and uploaded `panvk_vrs_render_probe.log` (450 bytes). Findings:
- Device enumeration selected `Wrapper(Mali-G720 MC7)`; driver is PanVK Mesa 26.3.0-devel git `10472e2c0c`; Vulkan API 1.3.354.
- `pipelineFragmentShadingRate=1`, `nonTrivialCombinerOps=1`, `strictMultiplyCombiner=0`.
- `vkCreateDevice=0`, `vkCreateRenderPass=0`, shader modules and pipeline layout result `0`.
- Crucially, the 1x1 KEEP/KEEP control completed real submission, fence wait, and staging readback: `PASS`.
- The log ends after entering the 2x2 render setup, immediately after the generic `creating 2x2 pipeline and offscreen target` message. The prior probe emitted no intermediate stage markers and therefore cannot identify which subsequent Vulkan operation stopped. It is not valid to conclude from this truncated log alone that the 2x2 GPU draw or VRS itself failed.
- No 2x2 submit/readback or combiner output was recorded. VRS remains unverified; the 1x1 control is confirmed.

To isolate the exact call, instrumented `tools/panvk_vrs_render_probe_win.c` with flushed begin/result checkpoints for image creation, memory requirements/allocation/binding, image view/framebuffer, staging buffer, graphics pipeline creation, command pool/buffer, command recording, queue submit, fence wait, map/readback, and cleanup. No PanVK driver source or VRS register programming was changed in this diagnostic commit.
- Diagnostic probe source commit: `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`.
- Exact-source Windows probe workflow run: `42260680210`, observed in progress.
- Exact-source Android-only package workflow run: `42260680099`, observed in progress.
- Android wrapper variant run: `42260680099`, in progress. Wrapper rebuild is not required for this test; keep the existing Winlator wrapper unchanged.
- New package/probe must be built and inspected from source SHA `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d` before next user test. Keep wrapper unchanged and do not benchmark a game yet.
- Ask user whether the prior probe visibly exited/crashed or remained stuck only if the new diagnostic run cannot be performed; the new stage logs should identify the last successful call even after an application crash.

Next: poll exact-source Android-only and Windows probe runs, inspect artifacts/hashes/dependencies, then ask user to rerun with the instrumented probe and matching ICD package. Continue to avoid declaring VRS support until the 2x2 output signature and combiner comparisons pass.


### Stage-diagnostic Windows probe built and inspected (2026-11-06)

- Diagnostic source commit: `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`.
- Windows probe workflow `42260680210`: compile and executable verification succeeded; artifact upload completed. Exact-source artifact ID `13525121377`, archive digest `a27bf55d180bcbba0af25e9f8b454148236597d47b5fdd371bb34a0ff2ee66a7`; independently downloaded archive hash matches.
- Executable `panvk-vrs-render-probe.exe`, 271,968 bytes, PE32+ x86-64; SHA-256 `4e12b8e68baa0ee82afb092cc66c271737498b7bd91f291a85df307c7ab2e1e0`.
- Binary string audit confirms all new per-stage diagnostics are present, including vkCreateImage, memory allocation/binding, image view/framebuffer, staging buffer, graphics pipeline, command pool/buffer, and fence wait.
- Android-only package build `42260680071` for the exact same SHA is still in progress; latest state was Generate host tools, no package artifact yet.
- Android wrapper variant run `42260680099` is also in progress but not required for this diagnostic test.
- Do not ask user to rerun until the matching Android package for this diagnostic source is built and its flat package/ELF dependencies are independently inspected.


### Stage-diagnostic VRS package build and independent audit completed (2026-11-06)

The exact-source Android PanVK package and Windows stage-diagnostic render probe are now both built and independently inspected.

- Feature branch: `experiment/g720-fragment-shading-rate`
- Exact source/workflow commit: `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`
- Commit changes only `tools/panvk_vrs_render_probe_win.c`: it adds flushed per-operation checkpoints and VkResult logging around image creation/allocation/binding, image view/framebuffer, staging buffer, pipeline creation, command pool/buffer allocation and recording, queue submission, fence wait, map/readback, and cleanup. No PanVK VRS register, compiler, or runtime driver behavior was changed by this diagnostic commit.

#### Android-only CI and package

- Workflow run `42260680071`, job `128039482139`: SUCCESS. All 12 substantive steps passed, including Android ARM64 libdrm, Android-only configure, PanVK compile/stage, driver/dependency audit, flat ADPKG packaging, and artifact upload.
- Artifact ID: `13524573718`; outer artifact SHA-256: `38554c72e94dbe78e597bc6ec7c1822640f496c6bd9febc665e5d98db55a9933`.
- Extracted inner `PanVK-G720-experimental.zip`: 4,148,410 bytes; SHA-256 `9b0b27cd512f82d81a4259bb5c35c84c8cab111afe6b72cc4bb7589722b13afc`.
- Inner ZIP integrity passed. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three payload hashes verify.
- Metadata: `26.3.0-devel-android-kbase-665d86a75d94`, min API 30, library `libvulkan_panfrost.so`.
- Both libraries are ELF64 AArch64 Android API 30. Driver SONAME is `libvulkan_panfrost.so`, RUNPATH is `$ORIGIN`, and DT_NEEDED resolves to private `libpanvk_drm.so`; that helper depends only on libc. CI's missing DRM symbol audit is empty.
- The final driver binary contains the PanVK capability-audit startup diagnostic string for raw GPU ID, decoded architecture, and the VRS extension-table bit.

#### Windows stage-diagnostic probe

- Workflow run `42260680210`, job `128039486326`: SUCCESS. GLSL/SPIR-V compilation, MinGW executable build, executable verification, and upload all passed.
- Artifact ID: `13525121377`; outer ZIP SHA-256 `a27bf55d180bcbba0af25e9f8b454148236597d47b5fdd371bb34a0ff2ee66a7`.
- Executable `panvk-vrs-render-probe.exe`: 271,968 bytes, PE32+ x86-64; SHA-256 `4e12b8e68baa0ee82afb092cc66c271737498b7bd91f291a85df307c7ab2e1e0`.
- Static import table contains only `KERNEL32.dll` and `msvcrt.dll`; Vulkan is loaded dynamically through the Winlator loader.

#### Current interpretation and next device gate

This is build/package validation only. It does not establish 2x2 GPU execution, shading-rate output, combiner semantics, or VRS performance.

The prior probe established that the 1x1 KEEP/KEEP control completed submit, fence wait, and readback, then stopped during 2x2 setup. The new probe should identify the exact operation/result where progress stops.

Next user action:
1. Import only the inner `PanVK-G720-experimental.zip` from Android artifact `13524573718` through Winlator Mali 1.2's custom-driver importer. Do not import the outer Actions ZIP, and do not replace the Android system/vendor Vulkan driver.
2. Keep the existing Winlator wrapper and container unchanged.
3. Run the matching `panvk-vrs-render-probe.exe` from artifact `13525121377` in that same container.
4. Send the complete `panvk_vrs_render_probe.log` (or full console output) and state whether the process exits, crashes, or remains stuck.

Do not run a game benchmark yet. First identify the last successful 2x2 Vulkan stage and its exact VkResult. Preserve the 1x1 PASS as a confirmed control; VRS remains unverified until the 2x2 readback and combiner comparisons pass.



### Repeated stage-diagnostic VRS run confirms reproducible 2x2 pipeline-creation stall (2026-11-06)

The user reran the same Windows probe from the same experimental PanVK package/container and uploaded a fresh complete log at `/mnt/data/panvk_vrs_render_probe.log`. The log identifies device `Wrapper(Mali-G720 MC7)`, PanVK Mesa `26.3.0-devel`, git `665d86a75d94`, Vulkan API `1.3.354`.

Observed results from this second run:
- Device creation, render-pass creation, shader modules, and pipeline layout all return success.
- Reported `pipelineFragmentShadingRate=1`, `nonTrivialCombinerOps=1`, `strictMultiplyCombiner=0`.
- The 1x1 KEEP/KEEP control passes completely: image/buffer allocation and binding, image view/framebuffer, graphics pipeline creation, command pool/buffer creation and recording, queue submit, 30-second fence wait, memory map/readback, and cleanup all return success. Log reports `1x1 KEEP control submit/fence/readback=PASS`.
- The 2x2 run also successfully creates the offscreen image, allocates and binds image memory, creates image view/framebuffer, creates and allocates the staging buffer, and binds it.
- The final log line is `stage: vkCreateGraphicsPipelines rate=2x2 begin`. No result is logged. The process was observed by the user still running after approximately 2m41s with 0 FPS, then terminated for log recovery. This points to a reproducible stall or crash inside/under 2x2 graphics pipeline creation, not a normal returned VkResult.
- The log contains literal escaped newline sequences in the stage labels, but the flushed checkpoints and final operation boundary remain unambiguous.

Interpretation / investigation priorities:
1. Treat this as a confirmed reproducible 2x2 pipeline-creation failure boundary, not a Kbase queue-submit/fence/readback failure. The 1x1 path exercises and passes those downstream operations.
2. Inspect the architecture-12 PanVK graphics pipeline path and compiler lowering for `VkPipelineFragmentShadingRateStateCreateInfoKHR`, including pNext parsing, rate validation/normalization, pipeline key/cache serialization, shader compilation inputs, and Mali v12 fragment state generation.
3. Determine whether the probe's exact 2x2 pipeline state is valid for the exposed properties/features. Do not assume the advertised extension table bit or `pipelineFragmentShadingRate=1` proves that 2x2 is implemented correctly.
4. Do not change Kbase memory mapping, queue synchronization, or CSF scheduling based on this log; the stall occurs before command submission.
5. Do not expose broader VRS combinations or mark VRS supported until pipeline creation returns, submission/fence/readback pass, and the pixel signature verifies the requested rate.
6. No game benchmark is authorized yet; preserve the 1x1 PASS as the control.

No source or driver binary was changed by this rerun. The canonical feature branch remains `experiment/g720-fragment-shading-rate` at `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`. The handoff branch is updated by this entry only.


### Source audit after reproducible VRS stall: advertised pipeline FSR has incomplete visible PanVK lowering (2026-11-06)

Inspected the exact feature branch at source commit `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`, focusing on the architecture-12 shader/compiler path and generated Mali v12 register definitions.

Findings:
- `src/panfrost/vulkan/panvk_vX_physical_device.c` advertises `.KHR_fragment_shading_rate = PAN_ARCH >= 12` and reports `.pipelineFragmentShadingRate = PAN_ARCH >= 12`, while primitive and attachment FSR are false. The properties report `.fragmentShadingRateNonTrivialCombinerOps = PAN_ARCH >= 12`, strict multiply false, and multiple additional restrictions (depth/stencil writes, sample mask, conservative rasterization, interlock, custom sample locations false).
- In `src/panfrost/vulkan/panvk_vX_shader.c`, the only explicit PanVK FSR-specific shader handling found is:
  - `fs_may_use_vrs()` tests the pipeline/feature state and returns a boolean.
  - `panvk_hash_state()` hashes that boolean into the shader cache key.
  - `panvk_compile_nir()` optionally rewrites `nir_intrinsic_load_sample_pos_or_center` into a calculation based on `nir_load_frag_shading_rate()`.
- The above is a shader semantic adjustment for fragment sample-position/center behavior. It is not itself implementation of the requested pipeline fragment rate, and it does not visibly encode `VkPipelineFragmentShadingRateStateCreateInfoKHR` rate dimensions or combiner operations into Mali hardware state.
- `src/panfrost/vulkan/panvk_vX_cmd_draw.c` has no FSR-specific references.
- `src/panfrost/genxml/v12.xml` contains no fragment-shading-rate / shading-rate fields or named register definitions (searched case-insensitively for shading-rate and fragment-rate terms).
- The PanVK Meson source list uses common `panvk_vX_*.c` sources for arch 12; the `fifthgen` directory is empty except `.gitkeep`. This reinforces that there is no separate fifth-generation FSR implementation file in this branch.

Assessment (provisional, not a confirmed root cause):
- The branch currently appears to advertise pipeline fragment shading rate on v12 without a corresponding visible hardware pipeline-state programming path in PanVK/genxml. The 2x2 pipeline-create stall may be in shader compilation/cache handling triggered by the VRS-enabled state, but the log alone cannot prove which internal function is hanging.
- It is not safe to keep advertising this extension merely because the feature bit is true. Before further device tests, trace the Vulkan runtime pipeline callback path into PanVK's `panvk_compile_shaders` and then the Mali compiler, and verify how `state->fsr` is consumed outside the shader source. Also inspect Mali G720 hardware documentation/firmware interface for any actual fragment-rate control not represented in the current genxml.
- Do not patch in guessed register fields or expose a speculative 2x2 implementation. The safe experimental fallback, if this audit confirms there is no supported hardware path, is to disable `VK_KHR_fragment_shading_rate` and its feature/property advertisement for this branch, then build a non-VRS control driver. Keep that separate from the existing diagnostic package until the code path is proven.
- No driver source or package was modified during this source audit. Next work remains source tracing and hardware capability verification; no user test is requested yet.


### Correction to VRS implementation audit: CSF draw-state programming is present (2026-11-06)

A deeper search of the architecture-specific CSF draw backend found a VRS implementation that the prior audit missed. The earlier statement that there was no visible Mali v12 hardware-state programming path was incorrect and is superseded by this correction.

Verified implementation:
- `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c:set_tiler_idvs_flags()` checks `dyn_gfx_state_dirty(cmdbuf, FSR)` and packs `MALI_PRIMITIVE_FLAGS_2` on `PAN_ARCH >= 12`.
- It reads `dyns->fsr.fragment_size.width/height` and both combiner ops.
- It forces 1x1 and KEEP/KEEP when the fragment shader uses sample shading.
- It encodes the rate as `(log2(max(width,1)) << 2) | log2(max(height,1))`, then packs both combiner enums into `vrs_combiner_0/1`.
- `src/panfrost/genxml/v12.xml` defines `Primitive Flags 2`: VRS Draw Rate (4 bits), Combiner 0 (3 bits), Combiner 1 (3 bits). The enum explicitly maps 1x1=0, 1x2=1, 1x4=2, 2x1=4, 2x2=5, 2x4=6, 4x1=8, 4x2=9, 4x4=10; combiner Keep=0, Replace=1, Min=2, Max=3, Mul=4. Thus the 2x2 formula yields 5, matching the v12 GenXML enum.
- The physical-device property comment and reported property values correspond to pipeline-only VRS. The GenXML also contains VRS image-rate definitions, but this driver does not advertise attachment VRS.
- The probe fragment shader uses `gl_FragCoord` to emit an 8x8 coordinate pattern; it does not use `gl_SamplePosition` / sample-position intrinsics. Therefore the optional NIR lowering in `panvk_vX_shader.c` that injects `nir_load_frag_shading_rate()` is not expected to be exercised by this particular probe shader.

Revised investigation:
- VRS hardware draw-state programming exists in the CSF path; do not disable the extension solely on the earlier missing-path claim.
- The observed hang remains during `vkCreateGraphicsPipelines`, before command recording or the CSF draw-state packer is called. The 2x2 rate is present in the graphics pipeline pNext state, and Mesa's Vulkan runtime parses it into `vk_graphics_pipeline_state.fsr`. The PanVK shader compiler callback receives this state.
- The investigation should now focus on pipeline-state parsing/validation/compilation/cache interactions when static FSR is non-default, and on whether the compiler's `fs_may_use_vrs` / `panvk_hash_state` handling or an unrelated pipeline-state path loops or corrupts state. Since the probe shader only reads `gl_FragCoord`, isolate whether the hang occurs before compiler entry, during VS compile, during FS compile, or after shader compile.
- Next diagnostic improvement: instrument the PanVK `panvk_compile_shaders` callback and `compile_shaders` loop with flushed checkpoints per stage, and instrument entry/exit around `panvk_compile_nir` / `pan_shader_compile`. Also log the static FSR state (width, height, combiner ops, dynamic bit) as read by PanVK. This can identify the precise compiler boundary without changing driver behavior.
- No driver code or package was changed during this correction. The prior "disable extension because no state programming exists" proposal is withdrawn pending these more precise diagnostics.


### Added PanVK VRS pipeline/compiler checkpoints for the next device log (2026-11-06)

After correcting the prior source-audit miss, a diagnostic-only source change was committed to the feature branch:
- Commit: `4b9556729f7807689f49a8efa9673f7b4a10847b`
- File changed: `src/panfrost/vulkan/panvk_vX_shader.c`
- No hardware behavior, feature advertisement, compiler lowering, memory allocation, or CSF draw programming was changed.
- Added `mesa_logi` checkpoints prefixed `PANVK_VRS_DIAG`:
  - `panvk_compile_shaders` callback entry/exit, including static FSR dimensions, combiner ops, and dynamic-FSR bit.
  - Per-stage compile entry/exit with stage index, stage enum, and VkResult.
  - `panvk_compile_nir` entry with FSR state.
  - `pan_shader_compile` entry/exit with generated binary byte count.
- This instrumentation will distinguish a hang before PanVK's shader callback, during per-stage NIR preparation, inside the Mali compiler, or after shader compilation.
- CI triggered by feature branch update: Android NDK Wrapper Variant run `42269851892` and Android-only PanVK run `42269851869`. At handoff update time both were in progress; inspect their results/artifacts before asking the user to install anything.
- Once the diagnostic Android package is built, the next test should run the same Winlator probe unchanged. Collect both the normal probe log and Android/Mesa driver logs containing `PANVK_VRS_DIAG`. The critical outcome is the last checkpoint printed before the 2x2 pipeline create stall.
- Do not modify the extension advertisement or disable VRS yet: the CSF v12 draw-state implementation exists and the 2x2 hardware enum mapping is consistent with the GenXML. The pipeline-create hang remains localized to an earlier stage and needs instrumentation evidence.


### Independent Arm confirmation of Mali-G720 VRS capability (2026-11-06)

Checked Arm's official Mali-G720 product support page. Arm lists Variable Rate Shading support on Mali-G720 for pipeline, primitive, and attachment shading rates, up to 4x4:
- https://support.arm.com/compute-ip/mali-g720

This confirms the GPU family has VRS capability in hardware; it does not by itself prove every PanVK path or every Vulkan feature/property is correctly implemented. The current PanVK branch intentionally exposes only pipeline FSR, and the CSF v12 draw backend packs the pipeline rate and combiner state. Keep primitive and attachment FSR disabled until those separate paths are implemented and validated.


### VRS diagnostic instrumentation format fix (2026-11-06)

A compile-warning prevention fix was committed after reviewing Mesa's `util_dynarray_num_elements()` macro return type (`size_t`):
- Feature branch commit: `dcd607bb809b271ac53e1138c071e5faeac5ef53`
- Changed the diagnostic byte-count format from `%u` to `%zu`.
- No functional driver behavior changed.
- New CI runs triggered for this corrected source commit; use only artifacts from the corrected head `dcd607bb809b271ac53e1138c071e5faeac5ef53`, not the preceding diagnostic commit's build.


### Added Vulkan runtime checkpoints before PanVK compiler callback (2026-11-06)

The PanVK-only compiler instrumentation could not distinguish a stall during Mesa's generic pipeline precompile from a stall before the driver callback. Added runtime checkpoints to `src/vulkan/runtime/vk_pipeline.c` on the feature branch:
- Feature branch commit: `3b2cb51178ff6f22d0160375b11cd681e324790d`
- Logs runtime graphics pipeline entry and the parsed FSR dimensions/combiner state.
- Logs entry/exit of each `vk_pipeline_precompile_shader()` call with stage index and VkResult.
- Logs entry/exit of `vk_graphics_pipeline_compile_shaders()`.
- Together with the PanVK shader logs, this identifies whether the stall occurs in generic state parsing, SPIR-V-to-NIR precompile, PanVK per-stage compilation, or `pan_shader_compile()`.
- This is diagnostic-only and does not alter pipeline state or GPU behavior.
- Use build artifacts only from feature branch head `3b2cb51178ff6f22d0160375b11cd681e324790d`; earlier CI runs were superseded by the added runtime checkpoints.


### Confirmed PanVK rate enumeration includes the probe's 2x2 configuration (2026-11-06)

Inspected `src/panfrost/vulkan/panvk_physical_device.c:panvk_GetPhysicalDeviceFragmentShadingRatesKHR()`:
- The enumerated rate list includes 2x2 with sample counts 1x, 2x, and 4x.
- It also lists 2x1 and 1x2 at 1x/2x/4x; 2x4 at 1x/2x; 4x2 and 4x4 at 1x; and 1x1 at all sample counts.
- The probe uses a 1x-sampled color target and requests 2x2, so the configuration is explicitly in the driver's own enumerated supported-rate list.
- This further weakens the possibility that the test is simply requesting an unadvertised rate. The failure remains a pipeline-create stall despite the rate being enumerated and its GenXML hardware encoding being defined.


### Diagnostic log format hardening (2026-11-06)

The VRS diagnostic log arguments were reviewed for C printf type correctness. Explicit casts were added for enum, boolean, and VkResult arguments in both PanVK and Vulkan runtime checkpoints. This avoids format warnings under the Android compiler's strict warning settings.
- Feature branch head: `f29602475f75b53665d30041c53a00c81b4f0933`
- No functional behavior changed.
- The build containing the final instrumented source must use this exact feature-branch head.


### Final format-argument cleanup and authoritative build head (2026-11-06)

Completed the remaining explicit enum/bool/VkResult casts in the PanVK diagnostic calls after reviewing the exact source lines.
- Authoritative feature branch head: `fa4ae716d1e643f680a4109db81470aba23492ea`.
- This commit changes diagnostic formatting only; no runtime driver behavior.
- All previous builds are superseded. Use only the Android build artifacts generated from this exact head.


### Runtime VRS diagnostic package built and audited (2026-11-07)

Both CI workflows for authoritative feature commit `fa4ae716d1e643f680a4109db81470aba23492ea` completed successfully:
- Android-only workflow `42271462292`, job `128076798673`.
- Android wrapper + Windows probe workflow `42271462129`; Android release job `128076798947`, debugoptimized job `128076798592`, Windows probe job `128076799058`.
- Android-only artifact ID `13529083912`; Android wrapper artifacts `13529094029` (release) and `13528994353` (debugoptimized); Windows probe artifact ID `13528824074`.
- Independently downloaded inner package `PanVK-G720-experimental.zip`: 4 files (`meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`); ZIP integrity passed; SHA256 `229f1df7a86b7c06e8658450b4e153db3c8372dedc8248b34ca31e6d99216ac9`.
- The outer Actions artifact contains staging and audit files. Import only the inner package, not the outer ZIP.
- Windows probe executable: PE32+ x86-64, 274,278 bytes, SHA256 `5e3418aa6c00ee874259e4bd5194ce179255255e1dd145d9e1559a1b980742fa`.
- Android build audit passed; missing-DRM-symbol report is empty.

These are diagnostic builds from corrected feature head `fa4ae716d1e643f680a4109db81470aba23492ea`, not a performance release. Next: import the inner package into the same Winlator test container, replace/run the Windows probe there, save the complete output as `panvk_vrs_render_probe.log`, and send it. Keep wrapper/container settings unchanged. Do not run Borderlands until the 2x2 pipeline-create checkpoint is understood.


### User-uploaded VRS capability probe result (2026-11-07)

The user uploaded `panvk_vrs_probe.log` from the PanVK package built at feature head `fa4ae716d1e643f680a4109db81470aba23492ea`. The complete log was read.

Observed:
- Device: `Wrapper(Mali-G720 MC7)`; PanVK Mesa `26.3.0-devel (git-fa4ae716d1)`; Vulkan API `1.3.354`.
- Device extension enumeration succeeds and includes `VK_KHR_fragment_shading_rate` (spec version 2).
- Reported feature bits: `pipelineFragmentShadingRate=true`; primitive and attachment FSR are false.
- Reported maximum fragment size is 4x4; supported-rate enumeration includes 2x2 with sampleCounts `0x7`, which includes the probe's 1x sample count.
- `vkCreateDevice` with pipeline FSR returns `VK_SUCCESS`.
- `vkCmdSetFragmentShadingRateKHR` command-buffer recording/end returns `VK_SUCCESS`.
- Probe ends `VRS PROBE PASS`.

Interpretation:
- This validates extension enumeration, feature enablement, and dynamic FSR command recording on the user's actual Winlator/Mali-G720 setup.
- This is the capability/command-recording probe, not the newer render probe. It does not create a graphics pipeline with static 2x2 `VkPipelineFragmentShadingRateStateCreateInfoKHR`, submit a draw, wait on a fence, or read back pixels.
- Therefore it does not resolve the separately reproduced stall at `vkCreateGraphicsPipelines rate=2x2 begin`, and it does not prove VRS rendering correctness or performance.
- Keep the prior 1x1 render control PASS and the 2x2 static pipeline-creation stall as separate evidence. Do not disable VRS based on this result; do not claim 2x2 rendering works; do not run a game benchmark yet.
- No driver source or package was changed in response to this log. Next investigation remains the exact static-FSR pipeline compilation boundary using the instrumented render-probe build. Confirm the user has run the matching `panvk-vrs-render-probe.exe` before requesting another test; if the uploaded file is the only available output, explain the distinction and provide the correct matching probe artifact.


### 2026-11-07 continuation — canonical handoff re-read and pipeline compiler-boundary audit

At the user's request, the complete canonical `PROJECT_CONTINUITY_HANDOFF.md` was fetched and read from its first byte through EOF before continuing. The fetched document was 533,817 characters; current blob SHA before this addendum: `c676b0726f08d543e0caa2470d3932dab1660972`. The canonical file remains on `docs/project-continuity-handoff-2026-11-03`. No divergent handoff copy was created.

#### Live branch/source verification

- Repository: `khalilgenshin42-design/panvk-g720-kbase-csf`.
- Live feature branch `experiment/g720-fragment-shading-rate` still points to `fa4ae716d1e643f680a4109db81470aba23492ea`.
- The matching Android-only workflow `42271462292` is completed successfully; matching Android wrapper + Windows probe workflow `42271462129` is also completed successfully.
- The current package/probe are diagnostic builds, not a VRS performance release. Do not substitute the older capability-only `panvk_vrs_probe.exe` for the render probe.

#### Fresh source-path audit

Re-fetched the current feature-branch source at the live HEAD:

1. `src/vulkan/runtime/vk_pipeline.c::vk_create_graphics_pipeline()` logs:
   - graphics-pipeline entry;
   - parsed static FSR dimensions and combiner operations;
   - each generic runtime shader-precompile begin/end and VkResult;
   - generic shader-compile callback begin/end and VkResult.

2. `vk_pipeline_precompile_shader()` performs SPIR-V-to-NIR conversion, optional driver NIR preprocessing, generic descriptor-heap lowering, precompile-object creation/cache insertion, and cleanup. It does not receive the static graphics FSR state as an argument to the driver's `preprocess_nir` callback.

3. `vk_graphics_pipeline_compile_shaders()` eventually calls `vk_compile_shaders()` with `compile_info->state`, so the static FSR state is passed to the PanVK shader callback after generic precompilation.

4. `src/panfrost/vulkan/panvk_vX_shader.c::panvk_compile_shaders()` logs callback entry with rate/combiner/dynamic-state values, then invokes `compile_shaders()` under PanVK's compiler lock. The per-stage loop logs stage begin/end and VkResult.

5. `panvk_compile_nir()` logs entry with the stage and FSR state, performs PanVK shader/NIR preparation (including `panvk_lower_nir()`, IO lowering, post-processing, sysval and push-constant lowering), then logs immediately before and after `pan_shader_compile()`. This provides a clear boundary around the Mali compiler itself.

#### Diagnostic interpretation for the next render-probe log

Use the last `PANVK_VRS_DIAG` marker to localize the 2x2 pipeline stall:

- No runtime precompile end: inspect generic SPIR-V/NIR precompile, driver preprocess, descriptor-heap lowering, or precompile cache.
- Runtime precompile completes but shader-compile begin has no callback begin: inspect generic runtime transition/dispatch.
- Callback begins but no stage-compile begin: inspect PanVK compiler-lock/setup and stage-link preparation.
- Stage compile begins but no `compile_nir begin`: inspect per-stage PanVK shader setup.
- `compile_nir begin` appears but `pan_shader_compile begin` does not: stall is in PanVK NIR lowering/preparation, not the Mali compiler.
- `pan_shader_compile begin` appears without its matching end: investigate the Mali compiler path with the exact stage/NIR dump.
- `pan_shader_compile end` appears but stage compile does not end: inspect binary allocation/copy and shader upload.
- Stage/callback complete but generic runtime shader-compile end is absent: inspect generic shader cache/link/post-callback handling.

The 1x1 render remains the passing control. The reproducible 2x2 stall remains bounded to `vkCreateGraphicsPipelines`; there is still no evidence of a queue submit, fence, Kbase mapping, or GPU execution failure in the failing 2x2 path. Do not change Kbase mappings, queue synchronization, shader-core dispatch, descriptor heap topology, or VRS feature exposure based on the current evidence.

#### Current next gate

No additional source change is justified before seeing the instrumented run's actual markers. The next user-dependent evidence is the matching `panvk-vrs-render-probe.exe` run against the matching diagnostic PanVK package, with:
- complete `panvk_vrs_render_probe.log` output; and
- the Winlator/Wine or Android driver log containing `PANVK_VRS_DIAG` messages from the same process.

The current diagnostic build identifiers are Android artifact `13529083912` and Windows probe artifact `13528824074`, both from source `fa4ae716d1e643f680a4109db81470aba23492ea`. Import only the nested `PanVK-G720-experimental.zip` from the Android Actions artifact. Keep the existing wrapper/container unchanged and never replace the Android system/vendor Vulkan driver.

No runtime source change, new package, benchmark, or VRS rendering pass is claimed in this continuation.


### 2026-11-07 correction — distinguish capability probe from render probe artifact

A final workflow/source cross-check found that the Windows artifact ID `13528824074` from run `42271462129` is the **capability/command-recording probe**, built from `tools/panvk_vrs_probe_win.c`. It is the executable that produces `panvk_vrs_probe.log` and reports `VRS PROBE PASS`. It does not create a graphics pipeline or submit a draw. The user's latest `panvk_vrs_probe.log` is therefore the expected capability-probe result and does not contain the pipeline compiler checkpoints.

The required next test is the separate **render probe**:
- Windows render-probe artifact ID: `13525121377`, from workflow run `42260680210`.
- Executable: `panvk-vrs-render-probe.exe`, PE32+ x86-64, SHA-256 `4e12b8e68baa0ee82afb092cc66c271737498b7bd91f291a85df307c7ab2e1e0`.
- Matching diagnostic PanVK Android package: artifact ID `13529083912`, from run `42271462292`, inner package SHA-256 `229f1df7a86b7c06e8658450b4e153db3c8372dedc8248b34ca31e6d99216ac9`.

The render-probe executable was built from source commit `665d86a75d94b9dc015b3de7b1959d6ef3fc9d6d`. A GitHub compare of `665d86a...` against the diagnostic driver commit `fa4ae716d1e643f680a4109db81470aba23492ea` confirms that the only five intervening commits modify `src/panfrost/vulkan/panvk_vX_shader.c` and `src/vulkan/runtime/vk_pipeline.c`; **no probe source or shader file changed**. Thus the existing render-probe executable is source-compatible with the latest diagnostic driver and does not need rebuilding.

Use the latest diagnostic Android package together with the existing render-probe executable. Save its output as `panvk_vrs_render_probe.log` and also capture the same process's Winlator/Wine or Android driver log containing `PANVK_VRS_DIAG`. Keep the capability-probe log as a separate successful API/device-create result; do not confuse it with the render-probe result.

This correction supersedes the previous sentence in the 2026-11-07 continuation that called artifact `13528824074` the matching render probe. No source or binary was changed by this correction.


### 2026-11-07 VRS render-probe log correlation — root cause isolated to unsupported NIR intrinsic

The user uploaded the Winlator log archive `[Community] Tomb Raider (1) (copy)_logs_2026-11-07_00-28-42.zip`. The archive contains `wine_debug.log` (18,130 bytes), which was read in full and correlated with the previously uploaded `panvk_vrs_render_probe.log`.

#### Runtime/device identity

- Winlator Mali 1.2.1 Bionic (23), Wine `10-arm64ec-0`, Android 16, device Xiaomi 25069PTEBG / MT6899.
- Graphics driver is the imported `wrapper-gamenative`; DX wrapper metadata reports DXVK+VKD3D but the executable launched in this run is the standalone render probe, not Tomb Raider.
- PanVK reports GPU ID `0xc8700010`, architecture v12, Kbase backend enabled, and `VK_KHR_fragment_shading_rate=1`.
- The log contains two render-probe launches. Both reproduce the same 1x1-success / 2x2-failure sequence.

#### Exact failure sequence

For each launch:
1. The 1x1 graphics pipeline's vertex and fragment stages both pass through `pan_shader_compile`, return success, and the callback completes.
2. The 2x2 pipeline's generic runtime precompilation completes successfully for vertex and fragment stages.
3. The PanVK callback starts with `rate=2x2`, `ops=0,0`, `dynamic=0`.
4. The vertex stage compiles successfully.
5. The fragment stage reaches `pan_shader_compile begin stage=4`.
6. The compiler prints `Unhandled intrinsic load_frag_shading_rate`.
7. Wine reports `_wassert ... "!status && \"vkCreateGraphicsPipelines\"`.
8. No matching fragment `pan_shader_compile end`, stage completion, or successful 2x2 pipeline creation follows.

This is not a stall in generic runtime precompilation, the PanVK callback setup, the vertex compiler, queue submission, fence wait, Kbase mapping, or GPU execution. The 2x2 fragment shader enters the Mali compiler and fails on an unsupported NIR intrinsic. The probe's own 2x2 pipeline creation assertion is downstream of this compiler failure.

#### Source correlation at feature branch HEAD

Feature branch `experiment/g720-fragment-shading-rate` remains at `fa4ae716d1e643f680a4109db81470aba23492ea`.

In `src/panfrost/vulkan/panvk_vX_shader.c`:
- `lower_vrs_frag_center()` (around lines 719–739) replaces `nir_intrinsic_load_sample_pos_or_center` with a calculation that calls `nir_load_frag_shading_rate()`, marks `SYSTEM_VALUE_FRAG_SHADING_RATE` as read, extracts X/Y log2 rate components, and scales the 0.5 center.
- In `panvk_compile_nir()` (around lines 1290–1300), that pass is enabled for fragment shaders on PAN_ARCH >= 12 when sample shading is not enabled and `fs_may_use_vrs()` returns true.
- The resulting `load_frag_shading_rate` is still present when `pan_shader_compile()` is called. The current Panfrost/Mali compiler path has no working lowering for this intrinsic, which is exactly what the runtime diagnostic reports.

Important nuance: the existing helper is trying to preserve sample-position/fragment-center semantics under VRS. Simply deleting the intrinsic or replacing it unconditionally with a constant could silently break shaders using sample position/center, and dynamic FSR cannot generally be represented by one compile-time constant. Do not apply a broad constant replacement or disable the extension as a speculative fix.

#### Next source investigation / safe implementation direction

Before changing code:
1. Inspect how `nir_intrinsic_load_frag_shading_rate` is represented in this Mesa revision, and whether Panfrost's compiler already has a supported hardware system-value route for fragment shading rate or whether this path needs an explicit PanVK lowering.
2. Inspect the render-probe fragment shader/SPIR-V to determine why it contains `load_sample_pos_or_center` and whether the intrinsic is required for this minimal shader.
3. Determine the exact encoding of the static `VkPipelineFragmentShadingRateStateCreateInfoKHR` rate and whether the current compiler path can safely specialize a **static** FSR rate at pipeline compile time while retaining a correct path (or rejecting unsupported usage) for dynamic FSR.
4. Add a focused NIR validation/assertion or diagnostic immediately before `pan_shader_compile()` so unsupported `load_frag_shading_rate` cannot silently reach the compiler.
5. Only after the above is proven, implement the narrowest valid lowering and run compiler/NIR tests plus the existing 1x1/2x2 render probe. Do not modify Kbase memory mapping, queue synchronization, shader-core dispatch, descriptor heaps, or general extension exposure for this failure.

No source code, build artifact, package, or device configuration was modified by this diagnosis. The user's wrapper and container settings must remain unchanged, and the Android system/vendor Vulkan driver must not be replaced.


#### Follow-up probe-shader audit

The exact render-probe shaders were fetched from the same feature-branch HEAD:
- `tools/vrs_probe.frag` computes `ivec2(floor(gl_FragCoord.xy))` and writes an 8-pixel coordinate pattern.
- `tools/vrs_probe.vert` emits a full-screen triangle using `gl_VertexIndex`.

This confirms the fragment shader deliberately reads `gl_FragCoord`. PanVK's VRS helper replaces `load_sample_pos_or_center` for the fragment-center calculation with `load_frag_shading_rate`; therefore the probe itself exercises the VRS-adjusted fragment-center system value. The log's `Unhandled intrinsic load_frag_shading_rate` is directly connected to this shader behavior, not an unrelated shader feature.

A source search of the current branch's `src/panfrost/compiler/bifrost/bifrost_nir.c` and `bifrost_compile.c` found no explicit `frag_shading_rate` or `SYSTEM_VALUE_FRAG_SHADING_RATE` handling in those files. This is consistent with the compiler rejection, but does not by itself establish the exact hardware register/instruction needed for a general dynamic-FSR implementation.

A narrow static-rate specialization remains a candidate for the current render probe because its `VkPipelineFragmentShadingRateStateCreateInfoKHR` is static (`dynamic=0`) and the rate is fixed per pipeline. It must only be implemented after verifying the Vulkan/Mesa rate encoding and the expected `load_sample_pos_or_center` semantics. A constant substitution is not valid for dynamic FSR and must not be used as a general lowering. The probe's use of `gl_FragCoord` is important: the center calculation affects observable pixel coordinates, so a passing pipeline-creation test alone is insufficient; the final pixel readback comparison must also pass.


### 2026-11-07 implementation attempt — static VRS fragment-center lowering

After correlating the runtime failure with the probe shader, a narrowly scoped source change was committed to the feature branch:

- Feature branch: `experiment/g720-fragment-shading-rate`
- Source commit: `00ed7243e5dea048361a1682753b7e4630450bb8`
- Changed file: `src/panfrost/vulkan/panvk_vX_shader.c`
- Commit message: `panvk: lower static VRS fragment center to NIR constant`

#### Change

The previous `lower_vrs_frag_center()` emitted `nir_load_frag_shading_rate()`, which the Bifrost/Valhall compiler currently rejects. It now receives a precomputed NIR bitfield and uses `nir_imm_int()` instead. The static rate is encoded according to the NIR intrinsic definition:
- bits 0..1: log2(fragment height), representing Vertical2/Vertical4;
- bits 2..3: log2(fragment width), representing Horizontal2/Horizontal4.

For 2x2, the encoded value is `0x5` (bit 0 + bit 2). The existing NIR calculation then produces fragment center (1.0, 1.0), preserving the original helper's intended center calculation for a 2x2 invocation. The rate calculation is limited by assertions to supported dimensions 1, 2, and 4.

The pass is now applied only when all these conditions hold:
- fragment shader is not using sample shading;
- FSR is enabled for this pipeline;
- pipeline has a concrete static `state->fsr`;
- `MESA_VK_DYNAMIC_FSR` is not set;
- both pipeline/primitive and attachment combiner operations are KEEP.

This covers the failing render probe's static 2x2 configuration (`dynamic=0`, combiner ops `0,0`) without emitting the unsupported intrinsic.

For dynamic FSR, missing static state, or non-KEEP combiners, the pass is deliberately skipped. The original `load_sample_pos_or_center` path is retained so the compiler is not handed `load_frag_shading_rate`. This is a conservative compiler-crash avoidance fallback, **not proof of correct `gl_FragCoord` semantics under those dynamic/combined-rate configurations**. Those cases remain explicitly unresolved and must not be described as fully supported.

#### Build status and next validation

The Android-only build workflow was automatically triggered by the feature-branch push:
- Workflow run: `42313022389`
- Run URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42313022389
- Source SHA: `00ed7243e5dea048361a1682753b7e4630450bb8`
- At the time of this handoff update, GitHub reported the run as `in_progress`, with dependency installation underway. No successful compile or package artifact is claimed yet.

If the build succeeds, import only the inner `PanVK-G720-experimental.zip` produced by this run, alongside the already-built `panvk-vrs-render-probe.exe`. Keep the existing Winlator wrapper/container unchanged and never replace the Android system/vendor Vulkan driver. Re-run the same render probe and collect both:
1. `panvk_vrs_render_probe.log`, checking for a successful 2x2 pipeline creation, queue submit, fence wait, readback, and expected 2x2 pixel repetition;
2. the same process's Winlator/Wine or Android driver log, checking for `PANVK_VRS_DIAG static frag-center lowering rate=2x2 encoded=0x5` and absence of `Unhandled intrinsic load_frag_shading_rate`.

A successful compile is only a build gate. A successful 2x2 pipeline creation is only an intermediate runtime gate. The final gate is correct readback/pixel-pattern verification. No performance gain is claimed. Dynamic FSR and non-KEEP combiner semantics remain open work.


### 2026-11-07 CI recheck — host-tool generation underway, package not ready

A fresh check of workflow run `42313022389` (source SHA `00ed7243e5dea048361a1682753b7e4630450bb8`) confirms that dependency installation has completed successfully and the job has advanced to **Generate host tools**. The Android ARM64 libdrm build, Android-only Meson configure, PanVK compile/staging, dependency audit, flat Winlator package, and artifact upload are still pending. The run remains `in_progress`; GitHub's run `updated_at` timestamp remained `2026-11-06T23:36:23Z` across repeated reads, so this backend status may be stale or stalled. The job-log download endpoint currently returns 404 while the job is active, so there is no further diagnostic output to inspect.

No artifact ID, ZIP, driver SHA256, successful build, or device validation exists for this commit yet. **Do not ask the user to download or test anything at this stage.** Continue polling the same run; if it completes, inspect the conclusion, job steps/logs and uploaded artifact before issuing any test request. If it remains stuck, report the CI blockage rather than claiming a build or inventing a package. The previous device-test acceptance criteria remain unchanged.


CI progression follow-up (same 2026-11-07 recheck): a later jobs poll shows **Generate host tools completed successfully** and **Build real Android ARM64 libdrm** is now `in_progress`. The remaining Android configure/compile/audit/package/upload steps are pending, and the run-level timestamp is still stale. Artifact listing is currently empty. This is genuine step progression, but still no build/package success and no device test request is appropriate yet.


### 2026-11-07 CI progression — Android compile/staging underway

A subsequent poll of workflow run `42313022389` (source `00ed7243e5dea048361a1682753b7e4630450bb8`) confirms two more successful steps: **Build real Android ARM64 libdrm** and **Configure Android-only build**. The job has advanced to **Compile and stage Android install**, currently `in_progress`. The dependency setup, host-tool generation, libdrm build, and Android-only configuration are all reported successful. Audit, flat Winlator package creation, and artifact upload remain pending; artifact listing remains empty. The run-level updated timestamp remains stale, and job-log download currently returns 404 while active.

This is meaningful build progress, but not yet proof that PanVK compiled or that a package exists. No phone action is required. Continue checking the compile step, then require the symbol/dependency audit and package artifact before giving the user a download or asking for the VRS render-probe run.



### 2026-11-07 VRS static-rate patch — Android package built and independently inspected

The Android-only workflow for the static VRS fragment-center lowering patch has now completed successfully.

#### Exact source/build provenance

- Feature branch: `experiment/g720-fragment-shading-rate`
- Source commit: `00ed7243e5dea048361a1682753b7e4630450bb8`
- Workflow: Android-only PanVK G720 Experiment
- Run ID: `42313022389`
- Run URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42313022389
- Job ID: `128218802704`, `build-android-only`
- Run conclusion: SUCCESS.
- All 12 substantive steps passed, including real Android ARM64 libdrm build, Android-only configure, PanVK compile/stage, driver/manifest/dependency audit, flat Winlator/Adrenotools packaging, and artifact upload.
- Workflow artifact ID: `13549348126`
- Artifact name: `panvk-g720-android-only-experimental-package`
- Outer artifact size: 16,815,604 bytes.
- Outer artifact SHA-256: `982932ef2c0a1c19f98ceab8a81f00e6328e1a908a26550c04268cc96648d0ac`
- Artifact expires 2027-02-04.

#### Independently inspected importable package

The outer Actions artifact was downloaded and inspected. Its importable nested file is `PanVK-G720-experimental.zip` (do not import the outer audit artifact ZIP).

- Inner flat package size: 4,151,682 bytes.
- Inner package SHA-256: `63e4de771c1ad91cca6a89eef067d695c4d3f88dd858cc0999e255142f813e99`
- Flat root contains exactly: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- `meta.json` package version: `26.3.0-devel-android-kbase-00ed7243e5de`; min Android API 30; explicitly labels the driver experimental, non-conformant, and runtime compatibility unverified.
- Driver SHA-256: `6229f669a75e38c37abd52c082cc8ecfd64fc1685d880a52ae5a07620748d6e5`
- Private libdrm SHA-256: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
- Metadata SHA-256: `6ebf440528854a687fcb6301c5dee866976906d7a45c9abbb90c19db3fc5e419`
- All three member hashes were recomputed and match `SHA256SUMS`.
- Driver is ELF64 AArch64 Android API 30. Its package-patched NEEDED entry is `libpanvk_drm.so`, SONAME is `libvulkan_panfrost.so`, and RUNPATH is `$ORIGIN`.
- Private `libpanvk_drm.so` is ELF64 AArch64, SONAME `libpanvk_drm.so`, and depends only on Android `libc.so`.
- CI's DRM ABI audit produced an empty `missing-drm-symbols.txt`; the build's DRM undefined-symbol set is covered by the bundled real Android ARM64 libdrm before package-private SONAME rewriting.
- The staged intermediate ICD's `$ORIGIN/../../android_stub` path and libdrm stub are build-stage details; the importable flat package is separately patched and uses the private `libpanvk_drm.so` dependency as intended.

#### Interpretation and device-test gate

This closes the compile, link, package, checksum, architecture, and dependency-audit gates for the static-rate patch. It does **not** prove Vulkan conformance, Kbase ABI/runtime compatibility, general/dynamic FSR correctness, or performance.

The next required test is now ready:
1. Import only the inner `PanVK-G720-experimental.zip` into Winlator Mali's custom driver slot. Do not install it over Android's system/vendor driver.
2. Keep the existing Winlator wrapper, container, and game settings unchanged.
3. Run the already-built `panvk-vrs-render-probe.exe` (the render probe, not the capability-only `panvk-vrs-probe.exe`).
4. Save the complete output as `panvk_vrs_render_probe.log`, and capture the same run's Winlator/Wine or Android driver log containing `PANVK_VRS_DIAG`.

Acceptance requires all of the following:
- The static 2x2 graphics pipeline creation returns successfully.
- Queue submission, fence wait, and readback all complete.
- The 2x2 pixel pattern matches the expected repeated-block/fragment-center behavior.
- Driver log contains `PANVK_VRS_DIAG static frag-center lowering rate=2x2 encoded=0x5`.
- No `Unhandled intrinsic load_frag_shading_rate` appears.

Pipeline creation alone is an intermediate pass; pixel readback is the final correctness gate. If the probe hangs, fails, or pixels mismatch, provide both logs and stop; do not resume Borderlands benchmarking or alter Kbase mappings, synchronization, descriptor heaps, queue settings, or wrapper configuration. Dynamic FSR and non-KEEP combiner cases remain explicitly unresolved.

No change was made to the phone's system/vendor Vulkan driver or kernel.


### 2026-11-07 VRS render-probe follow-up — static 2x2 KEEP reaches readback; MAX triggers CS error

The user supplied a fresh render-probe output and the matching Winlator log archive:
- `panvk_vrs_render_probe.log`: 4,924 bytes, 124 lines.
- `[Community] Tomb Raider (1) (copy)_logs_2026-11-07_00-57-07.zip`: contains `logcat.log` (589,332 bytes, 4,149 lines) and `wine_debug.log` (13,603 bytes, 199 lines). Both members were read and correlated with the probe output.
- The launched executable is `D:\\panvk-vrs-render-probe-win64\\panvk-vrs-render-probe.exe`, not Tomb Raider. The archive filename is the Winlator log bundle name; this run is the standalone VRS render probe.

#### Device and package identity confirmed again

The logcat startup diagnostic appears for the same process:
`PanVK capability audit: gpu_id=0xc8700010 arch=v12 KHR_fragment_shading_rate=1 kbase=1`.
The probe reports `Wrapper(Mali-G720 MC7)`, PanVK Mesa `26.3.0-devel (git-00ed7243e5)`, Vulkan API 1.3.354. This confirms the intended Mali-G720 v12 custom PanVK ICD and Kbase backend are active. The same process prints the static lowering marker:
`PANVK_VRS_DIAG static frag-center lowering rate=2x2 encoded=0x5`.

#### Exact test results

1. **1x1 KEEP/KEEP control: completed successfully.** Image allocation/binding, pipeline creation, command recording, queue submission, 30-second fence wait, host mapping/readback and cleanup all succeeded.
2. **2x2 KEEP/KEEP: reached successful GPU completion and host readback.** The 2x2 image, memory, view, framebuffer and staging buffer all create/bind successfully. The static 2x2 graphics pipeline now compiles: vertex and fragment stages complete, with the fragment stage taking the constant-lowering path and reporting encoded rate 0x5. Queue submit returns VK_SUCCESS, fence wait returns VK_SUCCESS, and host mapping/readback returns VK_SUCCESS. Probe prints `2x2 KEEP submit/fence/readback=PASS`.
3. **Important probe-criteria qualification:** this build of `tools/panvk_vrs_render_probe_win.c` only calculates/prints checksums and equal-neighbour counts after all five render cases succeed. The 2x2 KEEP case's PASS therefore proves submit/fence/readback, but the uploaded log does **not** contain its pixel checksum/equal-neighbour signature. Do not yet call the 2x2 pixel pattern validated.
4. **2x2 MAX/KEEP: pipeline compilation succeeds, GPU execution fails.** Runtime logs show parsed state `rate=2x2 ops=3,0 dynamic=0`; both generic precompile stages complete; the PanVK vertex stage compiles; the fragment stage deliberately remains on the native sample-center path (the static KEEP-only lowering is not applied), and Mali shader compilation returns successfully. The callback and generic pipeline compile both return success. Queue submission returns VK_SUCCESS, but fence wait returns `-4` (VK_ERROR_DEVICE_LOST). Logcat reports `kbase: CS error 0x1 on subqueue 0` twice and `kbase_queue_wait_current: failed to wait for subqueue 0 seqno 5`.
5. **MIN and MUL were not independently run.** The probe source gates them behind the previous result (`ok[3] = ok[2] && render_rate(...)`, etc.). Once MAX fails, MIN and MUL are skipped; their printed FAIL labels are consequences of short-circuiting, not separate hardware test results.

#### Source correlation and current interpretation

- The static fragment-center compiler failure is fixed for the tested static 2x2 KEEP/KEEP case: the unsupported `nir_load_frag_shading_rate()` is replaced by a constant NIR rate bitfield; the exact marker and successful fragment compiler exit are present in the new log.
- The architecture-12 CSF draw path in `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c::set_tiler_idvs_flags()` writes `PRIMITIVE_FLAGS_2` fields `vrs_draw_rate`, `vrs_combiner_0`, and `vrs_combiner_1` from `dyns->fsr`. For this MAX case the logged Vulkan combiner value 3 is written to hardware combiner 0.
- `src/panfrost/genxml/v12.xml` declares VRS combiner values Keep=0, Replace=1, Min=2, Max=3, Mul=4 and places the combiner fields in `Primitive Flags 2`. The C enum cast and XML values match numerically. This source-level enum match does not prove the non-KEEP operation is accepted or behaves correctly on the device.
- The observed `cell->error` path in `kbase_subqueue_wait_seqno()` turns the queue into VK_ERROR_DEVICE_LOST. The log currently exposes only error value 0x1 and subqueue 0; it does not include a decoded CS exception/status payload or the failing command-stream ring contents. The log is therefore insufficient to attribute the fault definitively to the MAX encoding, the native sample-center shader path, or another command-stream state interaction.
- The failure occurs after pipeline compilation and submit, during GPU/CSF execution. Do not return to the earlier unsupported-NIR-intrinsic diagnosis for this new failure. Do not modify Kbase virtual-memory mappings, descriptor heaps, generic queue synchronization, or shader-core dispatch as a speculative response.

#### Next engineering work — no user retest requested yet

1. Improve the PanVK Kbase diagnostic path so that when `cell->error` is observed, it also emits `kbase_log_subqueue_state(queue, subqueue, "CS error")` (or equivalent guarded diagnostics): completion/progress markers, insert/extract/active state, error value, last-job metadata, and ring-buffer qwords. Where safely available, drain/log the corresponding Kbase CSF notification so the real exception type/status/sideband are not lost behind the local error latch.
2. Improve the Windows render probe: emit checksum and equal-neighbour results immediately after each successful readback, even if a later case fails. Make each non-KEEP combiner case independently selectable/run in a fresh process/device so a device-lost MAX case cannot short-circuit MIN and MUL. Include an explicit 2x2 KEEP pixel-signature result as its own acceptance gate.
3. Inspect the VT/tiler subqueue command stream and register programming around `PRIMITIVE_FLAGS_2` for the MAX case. Preserve the passing 1x1 and 2x2 KEEP controls. Compare KEEP versus MAX with the smallest possible one-draw workload before changing hardware-state programming.
4. Build and statically audit the diagnostic Android package and independent Windows probe from the same exact source checkpoint. Only then ask the user for another controlled Winlator test. Keep the existing wrapper and container unchanged.

Current feature branch source for this test is `experiment/g720-fragment-shading-rate` at commit `00ed7243e5dea048361a1682753b7e4630450bb8`. This user run validates the build's static 2x2 KEEP pipeline compilation, submit, fence, and readback, but does not yet validate its pixel signature or any non-KEEP combiner. No performance result is claimed. The Android system/vendor driver and kernel remain untouched.


### 2026-11-07 Kbase CS-fault diagnostics added after the MAX/KEEP device-loss result

A diagnostic-only change was committed on `experiment/g720-fragment-shading-rate`:
- Commit: `e4473dce4547eca9f5159ff238f3ae17e6a7db46`.
- File: `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`.
- In the cold `cell->error` failure path, PanVK now calls the existing `kbase_log_subqueue_state(queue, subqueue, "CS error")` helper before marking the Vulkan queue lost. This records the seqno/progress markers, error value, CS ring insert/extract/active state, last-job metadata, and relevant ring-buffer qwords.
- It then drains already queued Kbase CSF notifications through the existing nonblocking `kbase_kmod_csf_process_events()` path and logs the result. If the kernel has a corresponding fatal/recoverable notification, the existing notification logger should now expose its exception status and sideband instead of the local completion-cell error hiding it.
- This runs only after a CS error and does not alter the normal submission, completion, memory mapping, queue scheduling, or VRS hardware state.

Android-only build run:
- Workflow run `42315762033`, job `128227831317`, exact source SHA `e4473dce4547eca9f5159ff238f3ae17e6a7db46`.
- At the latest poll the job was in progress at dependency installation; all earlier setup/checkout/verification steps passed. No package artifact or build success is claimed yet.
- Next gate: wait for Android compile, ABI/dependency audit, flat package creation and artifact upload. Reuse the existing Windows render probe for this driver-diagnostic run; no probe source change is needed to collect the new Kbase logs.

Do not ask the user to install or test until this exact-source package completes and is independently inspected. On the next run, keep the wrapper/container unchanged and send both the probe output and matching logcat/Wine log. The purpose is to obtain the VT/tiler CS ring and any Kbase CSF exception details for the MAX/KEEP failure. Preserve the 1x1 and 2x2 KEEP readback results; do not treat the pixel signature as validated until a probe explicitly reports the checksum/equal-neighbour comparison.


### 2026-11-07 follow-up — isolated combiner probe and Kbase error-state capture queued for CI

After reviewing the first render-probe source, two test-quality gaps were corrected before requesting another device run.

#### Windows render-probe improvements

- Commit `ff80089f8ad8c6556faeafd2b065111947ec8ef3` changes `tools/panvk_vrs_render_probe_win.c` to accept one explicit case per process: `--case=keep|max|min|mul|replace`.
- Commit `5641713220782142eeaa79908041bce86b72a88b` fixes diagnostic line endings in the generated log strings.
- Every case first renders a 1x1 KEEP control and a 2x2 KEEP control. It immediately prints each image checksum and equal-neighbour count and checks the 2x2 repeated-block pixel signature before attempting the selected combiner.
- Non-KEEP cases then run independently in a fresh process/device, so a MAX device loss cannot short-circuit MIN, MUL or REPLACE. Expected comparisons are MAX -> 2x2 KEEP, MIN -> 1x1 KEEP, MUL -> 2x2 KEEP, REPLACE -> 1x1 KEEP. The MUL case checks 2x2 execution with the default 1x1 primitive rate; it is not a strict-multiply property discriminator.
- Commit `4c9d20d6c1ac7deef0ef46204a7a3f0bf4c44277` adds `tools/run-vrs-cases.cmd`, which launches keep/max/min/mul/replace in separate processes and preserves one log per case.
- Commit `bb9af784a16167e6dd963a59854f7deb46f99960` updates `.github/workflows/build-vrs-render-probe.yml` to include the runner script in the artifact.
- Commit `c0e807c899b93ae76093f56ccc59a1395a237202` fixes the runner's executable path for the Actions artifact layout. The script resides under `tools/` and launches the executable one directory above it.

#### Current branch and build gates

Feature branch HEAD: `c0e807c899b93ae76093f56ccc59a1395a237202`. The PanVK driver source change remains the Kbase CS-fault diagnostic commit `e4473dce4547eca9f5159ff238f3ae17e6a7db46`; later commits in this sequence change only probe tooling/packaging. The Android package at HEAD therefore contains the same diagnostic driver implementation as that source commit.

Latest exact-HEAD CI runs:
- Windows VRS probe: run `42315974283`, workflow `Build Winlator VRS render probe`.
- Android-only PanVK package: run `42315974348`, workflow `Android-only PanVK G720 Experiment`.
- Android wrapper variant: run `42315974403`.
At the latest poll these runs were queued; earlier runs for intermediate commits were still in progress. No new executable or Android package artifact is claimed ready until the exact-HEAD jobs finish and the uploaded files are inspected.

#### Next user gate

Do not ask for another device run until both exact-HEAD artifacts are available and independently checked. Then provide the GitHub Actions artifact links (not a guessed local file path). User should extract the Windows probe artifact, run `tools\\run-vrs-cases.cmd` inside the existing Winlator container with the diagnostic PanVK package installed, and send all five per-case logs plus the matching Winlator/logcat archive. Keep wrapper and container settings unchanged. This is a correctness/diagnostics run only; do not restart Tomb Raider/Borderlands performance benchmarking yet.


### 2026-11-07 CI update — refreshed Windows VRS probe artifact available

The exact-HEAD Windows render-probe workflow completed successfully:
- Run: `42315974283`
- Job: `128228539539`, `build-vrs-render-probe`
- Source: feature branch `experiment/g720-fragment-shading-rate`, commit `c0e807c899b93ae76093f56ccc59a1395a237202`
- Artifact: `panvk-vrs-render-probe-win64`, ID `13551136207`
- Artifact SHA-256: `e15e23538a3d8b7839d18392681f8aba1ce2731205b17d78bc8f100fddac1046`
- Direct GitHub artifact URL: `https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42315974283/artifacts/13551136207`
- Artifact expires 2027-02-05.

This is the rebuilt probe with `--case=keep|max|min|mul|replace`, per-readback checksum/equal-neighbour output, and the `tools/run-vrs-cases.cmd` runner. The workflow completed compilation, executable verification and artifact upload successfully.

The matching Android-only PanVK package workflow `42315974348` is still in progress. At the latest poll, Android configuration and dependency preparation had passed and the ARM64 driver compile/stage step was running. No updated Android package is available or approved for device testing yet. Continue to wait for compile, ABI/dependency audit, flat package creation, upload, and independent artifact inspection before requesting another Winlator run.

The separate wrapper workflow `42315974403` has its VRS capability probe completed, but its release/debugoptimized Android wrapper builds remain in progress. These wrapper outputs are not the PanVK driver package and are not required for the current VRS diagnosis; do not substitute them for the custom PanVK ICD.


### 2026-11-07 VRS diagnostic Android package — exact-HEAD build and independent package audit passed

The exact-HEAD Android-only PanVK workflow completed successfully and its artifact was downloaded and independently inspected.

#### Build provenance

- Feature branch: `experiment/g720-fragment-shading-rate`
- Exact source HEAD: `c0e807c899b93ae76093f56ccc59a1395a237202`
- Android-only workflow run: `42315974348`
- Job: `128228540390`, `build-android-only`, conclusion SUCCESS.
- All substantive steps passed: checkout/source verification, Python/dependencies, host tools, real Android ARM64 libdrm build, Android-only Meson configuration, driver compile/stage, driver/manifest/dependency audit, flat Winlator/Adrenotools package creation, and artifact upload.
- Artifact: `panvk-g720-android-only-experimental-package`, ID `13551810052`.
- Direct GitHub artifact URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42315974348/artifacts/13551810052
- Outer artifact size: 16,817,414 bytes.
- Outer artifact SHA-256, independently recomputed: `cb5601a76d94f54c7401f0e656a265ea89a5be5a5d7f14c212a84310934c169e`.
- Artifact expiry: 2027-02-05.

#### Inner importable package audit

The outer artifact was downloaded and inspected. Import only the nested `PanVK-G720-experimental.zip`, not the outer audit ZIP.

- Inner flat package size: 4,151,341 bytes.
- Inner package SHA-256, independently recomputed: `669afc554c6a1955090277a80909173b68df1f9f720280c9d92e0ef3b4d0f885`.
- Package root contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- Package version: `26.3.0-devel-android-kbase-c0e807c899b9`; minimum Android API 30; metadata explicitly says non-conformant and device/game compatibility unverified.
- Driver: ELF64 AArch64 Android API 30, SONAME `libvulkan_panfrost.so`, NEEDED `libpanvk_drm.so`, RUNPATH `$ORIGIN`.
- Private libdrm: ELF64 AArch64 Android API 30, SONAME `libpanvk_drm.so`, depends only on Android `libc.so`.
- All three inner `SHA256SUMS` entries were recomputed and passed.
- CI audit's `missing-drm-symbols.txt` is empty. The staged driver's undefined DRM symbols are covered by the bundled real Android ARM64 libdrm; the final package uses the private SONAME and relative RUNPATH.
- The outer archive also contains detailed ELF, dynamic-section, dependency, manifest, symbol, and staged-file audit reports. The staged `$ORIGIN/../../android_stub` dependency is not the importable package's runtime arrangement; the inner flat package was independently checked and correctly resolves the private `libpanvk_drm.so` beside the driver.

#### What is and is not proven

This closes exact-source compilation, linking, Android architecture, flat-package structure, checksum, SONAME/RUNPATH, and DRM symbol/dependency audit gates for the diagnostic driver. It does not prove device runtime compatibility, Vulkan conformance, VRS pixel correctness, or game performance.

The matching Windows render-probe artifact is also ready:
- Run `42315974283`, artifact ID `13551136207`.
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42315974283/artifacts/13551136207
- Artifact SHA-256: `e15e23538a3d8b7839d18392681f8aba1ce2731205b17d78bc8f100fddac1046`.
- It contains the per-case runner `tools/run-vrs-cases.cmd` and executable `panvk-vrs-render-probe.exe`; cases are isolated as keep/max/min/mul/replace.

#### Next device-test gate

The diagnostic package and probe are now built from the same exact feature-branch HEAD. The user can now perform the controlled diagnostic run:

1. Download the Android Actions artifact and extract the **inner** `PanVK-G720-experimental.zip`.
2. Import that inner ZIP only through Winlator Mali's custom driver importer. Do not replace or modify Android's system/vendor Vulkan driver or kernel.
3. Keep the existing Winlator wrapper, container, and game configuration unchanged.
4. Extract the Windows probe artifact and run `tools\\run-vrs-cases.cmd` in that same container.
5. Send all five logs (`panvk_vrs_keep.log`, `panvk_vrs_max.log`, `panvk_vrs_min.log`, `panvk_vrs_mul.log`, `panvk_vrs_replace.log`) and the matching Winlator/logcat archive.

The run is for VRS pixel-signature and CS-fault diagnosis, not performance benchmarking. Preserve the existing 1x1 and 2x2 KEEP controls. MAX previously caused VK_ERROR_DEVICE_LOST; the new driver adds cold-error CS ring/subqueue diagnostics and queued Kbase CSF notification logging. MIN/MUL/REPLACE were not previously independently tested. Do not resume Tomb Raider or Borderlands FPS comparisons until the VRS correctness/fault investigation is complete.

No system/vendor driver or kernel modification was made.


### 2026-11-07 VRS render-probe results — KEEP pixel signature passes; all non-KEEP static combiners fault

The user supplied:
- `panvk_vrs_render_probe.log`
- `panvk_vrs_keep.log`
- `panvk_vrs_max.log`
- `Container (copy)_logs_2026-11-07_01-22-19.zip`, containing `logcat.log` and `wine_debug.log`.

The two KEEP logs are byte-identical. The combined logcat contains the process-by-process traces for KEEP, MAX, MIN, MUL and REPLACE, with two rounds of the non-KEEP cases. The archive therefore contains enough information to analyze all four non-KEEP operations even though the separate MIN/MUL/REPLACE output files were not attached.

#### Verified pixel signature

- 1x1 KEEP completed submit, fence wait and host readback. Checksum: `0x06d78d985ad50383`; equal-neighbour count: 0.
- Static 2x2 KEEP completed submit, fence wait and host readback. Checksum: `0xcad734dde7920383`; equal-neighbour count: 4096.
- Probe explicitly reports `2x2 KEEP pixel-signature=PASS (control=0 vrs=4096)` and `VRS GPU RENDER PASS`.
- This is the first actual pixel-pattern validation, not merely pipeline/submit/readback success.

#### All non-KEEP combiners independently reproduced the same primary fault

The logcat's PanVK pipeline-state traces record the actual Vulkan combiner enum values and their corresponding queue fault:
- MAX: `ops=3,0`, then CSI 0 recoverable fault `status 0x00011058`.
- MIN: `ops=2,0`, same CSI 0 fault.
- MUL: `ops=4,0`, same CSI 0 fault.
- REPLACE: `ops=1,0`, same CSI 0 fault.
- The sequence repeats in a second round. Every case compiles its pipeline and fragment shader, submits, then fails during GPU execution before successful target readback.
- Fault sideband is consistently `0x0000005fff39c1a8`.
- Kbase's later CSI 1 notification is `0x0000000f`, CS_RESOURCE_TERMINATED, a secondary termination notification after the primary fault; do not treat it as the root exception.
- `0x58` is the CSF `DATA_INVALID_FAULT` exception, documented by Arm Kbase CSF fault-name handling as an invalid descriptor fault. This is not the previously discussed GPU shareability fault `0x88`.
- The non-KEEP shader logs consistently state `frag-center dynamic/combined rate left on native sample-center path`; KEEP instead uses the working constant NIR lowering. The same exception across all four combiners, combined with this compiler-path difference, makes the native fragment-center/system-value path the leading hypothesis. It is not yet proven that this is the only root cause.

The standalone uploaded `panvk_vrs_max.log` is inconsistent with the matching archive: it labels the selected case MAX but its pipeline trace shows `ops=4` (MUL). For operation identity, use the combined archive's process traces, which show MAX=3 and MUL=4 in separate runs. A new probe now asserts the Vulkan enum ABI at compile time and logs numeric enum values before submitting each selected case.

#### Enum-audited Windows probe

- Source commit: `a2ca300629f0f38ce5aa048c92546aed06abea2f`.
- Windows probe workflow run: `42318169016`, job `128235882397`, SUCCESS.
- Artifact ID: `13551667465`, name `panvk-vrs-render-probe-win64`.
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42318169016/artifacts/13551667465
- Outer artifact SHA-256 independently recomputed: `4788e5111be1a13b01f96b61831bda12069bd7700b34453b0a3e0fa0aeb30ddc`.
- Contains `panvk-vrs-render-probe.exe` and `tools/run-vrs-cases.cmd`; executable PE signature verified and embedded strings confirm the enum-audit diagnostics are present.
- The source has compile-time assertions for KEEP=0, REPLACE=1, MIN=2, MAX=3, MUL=4. Successful build proves the probe's header enum ABI matches Vulkan specification values. The executable logs all five values and the requested op0/op1 numeric values for each target case.

#### Driver-side static pipeline-only lowering fix committed

Commit `4fdb9968d407618cfe3f8101d409fd803574bba3` on `experiment/g720-fragment-shading-rate` changes `src/panfrost/vulkan/panvk_vX_shader.c`:
- Adds a log2-domain combiner helper for KEEP, REPLACE, MIN, MAX and MUL.
- Resolves both static pipeline combiners against the implicit 1x1 primitive rate and 1x1 attachment rate. This is valid for the current advertised device feature configuration: pipeline FSR enabled, primitive and attachment FSR disabled.
- Honors the advertised `fragmentShadingRateStrictMultiplyCombiner=false` special case: multiplying 1x1 by 1x1 produces 2x2.
- Lowers the resolved effective rate into the fragment-center NIR calculation, rather than leaving non-KEEP static pipeline rates on the native sample-center path.
- Dynamic FSR and unsupported feature combinations are intentionally not changed and remain unresolved.

This is a focused hypothesis-driven compiler fix; no Kbase memory mapping, descriptor heap, queue synchronization, or hardware register encoding was changed.

#### Build gate and next action

- Exact-source Android-only build run: `42318330286`, source HEAD `4fdb9968d407618cfe3f8101d409fd803574bba3`.
- At this handoff update it is still in progress at host-tool generation. No package artifact or compile success is claimed yet.
- Do not ask for a device retest until this exact source builds, passes the Android ELF/DRM dependency audit, and its inner flat package is independently inspected.
- When ready, use the enum-audited Windows probe artifact above with the newly built driver. Preserve the existing wrapper/container and import only the inner `PanVK-G720-experimental.zip` through Winlator's custom driver slot.
- The next test should capture all five individual logs and matching logcat. Expected effective rates for the probe's 2x2 pipeline rate and implicit 1x1 primitive/attachment rates: MAX=2x2, MIN=1x1, MUL=2x2, REPLACE=1x1. Success requires target readback and checksum matching the corresponding KEEP control, not merely a completed fence.
- Do not resume game FPS benchmarking until the non-KEEP fault is resolved and pixel signatures are correct.

No Android system/vendor driver or kernel was modified.


#### Diagnostic precision correction — distinguish inherited CSI faults from the primary exception

A closer timestamp-by-timestamp correlation of the first test batch refines the summary above:
- First batch MAX (ops=3) and MIN (ops=2) produce the primary CSI 0 `DATA_INVALID_FAULT` (`0x58`, sideband `0x5fff39c1a8`).
- The subsequent first-batch MUL/REPLACE process failures include `0x4b` `CS_INHERIT_FAULT` notifications after the subqueue/CSG has already faulted. These are inherited/secondary and must not be counted as independent primary `0x58` events.
- The second complete batch independently records `0x58` on CSI 0 for MAX (3), MIN (2), MUL (4), and REPLACE (1), each with the same sideband `0x5fff39c1a8`. This second batch is the evidence that every non-KEEP static combiner reproduces the primary data-invalid fault.
- CSI 1 `0x0f` is `CS_RESOURCE_TERMINATED`; `0x4b` is `CS_INHERIT_FAULT`. Neither should be conflated with the primary `0x58` event.

The targeted static pipeline-only NIR lowering fix remains justified by the second batch and by the repeated difference between the successful KEEP compiler path and the failing native sample-center path.


## 2026-11-07 12:15 UTC — scheduled CI and artifact revalidation

### Branch and run discovery

- Re-read this handoff from `docs/project-continuity-handoff-2026-11-03` and the performance audit from `experiment/g720-performance-audit` before checking CI.
- At the start of this monitoring pass, active source branch HEAD was `d9394de49dc560fe2212a92b71280c372e66b0e6`. Its commit message is `docs: record 2026-11-06 PanVK artifact monitoring [skip ci]`; the inspected diff changes only `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`. The latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- GitHub's branch-filtered Actions listing returned 125 runs. The newest relevant Android-only and wrapper runs are still the 2026-11-06 pair below; no newer run exists on this branch at this monitoring time.
- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), head `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`, job `127885224447`: `completed / success`. All steps completed successfully: job setup, selected-branch checkout, checkout verification, Python setup, dependency installation, host-tool generation, Android ARM64 libdrm build, Android-only Meson configure, compile/stage, driver/manifest/dependency/staged-file audit, flat package creation, artifact upload, post-steps, and job completion. Pending: 0; failed: 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same head:
  - Release job `127885224388`: `completed / success`. Setup, checkout, memfd_create compatibility patch, Python/dependencies, host-tool generation, Meson configure, compile, artifact upload, post-steps, and completion all passed. Pending: 0; failed: 0.
  - Debugoptimized job `127885224836`: `completed / success`; the same steps all passed. Pending: 0; failed: 0.
- No failed job exists in the latest pair, so no failure log diagnosis was needed. The older known runs `41947103034` and `41947103085` are superseded by these newer successful runs.

### Artifact revalidation performed in this monitoring pass

**Android-only artifact — static/package validated**

- Workflow artifact `13501237590`, name `panvk-g720-android-only-experimental-package`, 16,753,940 bytes. GitHub's SHA-256 digest and the independently recomputed downloaded ZIP hash both equal `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`. Outer ZIP integrity test passed.
- The embedded import package `PanVK-G720-experimental.zip` is 4,133,089 bytes, SHA-256 `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; its ZIP integrity test passed. It contains exactly four flat-root files: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- All embedded SHA256SUMS entries were recomputed and matched: driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`; private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`; metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`.
- `meta.json` identifies `26.3.0-devel-android-kbase-b17b98cef92f`, minimum Android API 30, library `libvulkan_panfrost.so`, and explicitly labels the driver experimental/non-conformant with device/game compatibility unverified.
- Both packaged ELF files are ELF64 AArch64 with Android API 30 notes. Driver RUNPATH is `$ORIGIN`; its NEEDED private dependency is `libpanvk_drm.so`. The private libdrm has SONAME `libpanvk_drm.so` and depends only on `libc.so`. The outer artifact's `audit/missing-drm-symbols.txt` is empty. The staged ICD manifest is present in the outer artifact at `stage/usr/local/share/vulkan/icd.d/panfrost_icd.aarch64.json`, reports Vulkan API `1.4.354`, and points to the staged PanVK library.
- Classification: **static/package validated**. This is not device-runtime, Vulkan-conformance, WSI, or game validation.

**Android wrapper artifacts — raw ELF checked, not standalone package validated**

- Release artifact `13502135385`: 4,288,169-byte ZIP; API digest and recomputed SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; contains only `libvulkan_panfrost.so`. Extracted ELF SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Debugoptimized artifact `13502061037`: 50,663,624-byte ZIP; API digest and recomputed SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; contains only `libvulkan_panfrost.so`. Extracted ELF SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both are ELF64 AArch64/Android API 30. Both RUNPATHs are `$ORIGIN/../../android_stub`. Their NEEDED set includes Android system libraries plus `libdrm.so`, `libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`, and `libxshmfence.so.1`.
- Neither wrapper ZIP contains a manifest, checksum file, `android_stub/libdrm.so`, or the X11/XCB dependency closure. Their ELF format and hashes are verified, but the artifacts are not proven self-contained or directly importable as complete driver packages. Runtime dependency resolution remains unverified.

### Current blockers and safety

- The Android-only inner ZIP is the only artifact in this run classified as static/package validated. A controlled Winlator Mali custom-driver runtime test is still required before claiming it loads or works on the Xiaomi 15T.
- The wrapper Release/Debugoptimized outputs need their intended runtime dependency layout verified before they can be described as complete packages.
- No source code, build workflow, system/vendor Vulkan driver, or live kernel was changed. No build was triggered. Keep the experimental Android-only package separate in Winlator; retain the stock System Driver as fallback.


## 2026-11-07 13:33 UTC — performance-audit revalidation

The performance-audit branch and package were rechecked after the 12:20 UTC
monitoring corrections.

- At the start of this pass, `experiment/g720-performance-audit` HEAD was
  `5ebf10a4792126a0cb85ec78e380e50dcc063794`. Its latest driver-source
  commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.
  Comparing those commits shows only documentation paths changed; there is
  no newer source or workflow change.
- The latest exact-source CI pair remains Android-only run
  `42215758218` (job `127885224447`) and wrapper run `42215758167`
  (Release `127885224388`, Debugoptimized `127885224836`). Every job and
  step was rechecked and completed successfully. No newer relevant run was
  found; no build was triggered.
- Descriptor failure classification was re-read: overflow, descriptor-set
  slot/heap capacity, and VMA allocation failure are counted separately;
  aggregate count and Vulkan return behavior are preserved.
- The heap-threshold log reports only the effective threshold. A 262,144 log
  from a binary compiled with that default does not establish environment
  propagation. The 65,536-default package remains the only valid test binary
  for proving Winlator passes the override.

### Package file revalidation

The local `PanVK-G720-65k-env-override-test.zip` was checked again:
SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`.
ZIP integrity and all embedded member hashes pass. It contains exactly
`meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and
`SHA256SUMS`; both libraries are ELF64 AArch64, the driver uses RUNPATH
`$ORIGIN` and private `libpanvk_drm.so`, and the private libdrm depends
only on `libc.so`. This is static/package validation only, not device
runtime validation.

### Device evidence and next action

No new Tomb Raider log archive, benchmark result, or runtime observation was
supplied in this pass. The latest clean documented comparison remains
9.0 / 17.6 / 13.5 FPS at 65,536 versus the single 7.8 / 18.9 / 13.9 FPS
262,144-compiled-default candidate; it is inconclusive and not an environment
override test. The earlier 7.7 / 19.7 / 13.6 capture was instrumented.

Next: import only the verified 65K-default ZIP as the separate Winlator custom
driver. Run the matched Tomb Raider scene once with
`PANVK_KBASE_HEAP_RENEW_WORK` absent and require the startup log to report
65,536. Restart the process, set exactly
`PANVK_KBASE_HEAP_RENEW_WORK=262144`, and require the log to report 262,144.
Keep all profiling variables unset for both FPS runs. If the second log
reports 65,536 or is missing, the override was not demonstrated and the
comparison is invalid. Preserve the stock System Driver as fallback.

Audit record: `experiment/g720-performance-audit` documentation update
`a8a6e5b904de570dc9984d45b5b0890c7ad98a0d`. No driver source, Vulkan
semantics, allocator policy, queue sizing, shader scheduling, Kbase ABI,
kernel, or Android system/vendor driver was changed. No FPS gain is claimed.


## 2026-11-07 14:55 UTC — static FSR lowering build and package audit completed

The previously pending Android-only build for the targeted static fragment-shading-rate lowering fix has completed successfully.

### Exact source and CI

- Source branch: `experiment/g720-fragment-shading-rate`.
- Exact source HEAD: `4fdb9968d407618cfe3f8101d409fd803574bba3`.
- Android-only workflow run: `42318330286`; job `128236433525`; conclusion SUCCESS.
- All 12 substantive workflow steps passed: checkout and source verification, Python/dependency setup, host tools, real Android ARM64 libdrm build, Android-only Meson configure, compile/stage, driver/manifest/dependency audit, flat Winlator/Adrenotools package build, artifact upload.
- The exact source compiled; this is not a build of the earlier diagnostic-only commit.

### Artifact and independent package inspection

- Outer Actions artifact: `panvk-g720-android-only-experimental-package`, ID `13552276626`.
- Download URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42318330286/artifacts/13552276626
- Outer artifact SHA-256: `077b3ee72f907fac75c6a3ef3b4ba13370fc23fbd7c4673aae696785644a9cf1`.
- Outer artifact is an audit bundle. The importable file is the nested `PanVK-G720-experimental.zip`; do not import the outer archive or the `stage/` copy.
- Inner package SHA-256, independently recomputed: `0d4f2de29220a291d582109cd872d8267e5a5a883fd9d82e577a0fb91bfc47f2`.
- Inner ZIP contains exactly four flat-root members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- Embedded SHA-256 values independently match:
  - `libvulkan_panfrost.so`: `957339ac7e762db884507b81fdb69dbae93c2c1aa79e1f40988f16dc5d89899c`.
  - `libpanvk_drm.so`: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`.
  - `meta.json`: `c84f842179793c2260980b38d9ec46faede0b39508d47bcbbba41d472b574003`.
- Both packaged libraries are ELF64 little-endian AArch64. The packaged Vulkan ICD's dynamic section has RUNPATH `$ORIGIN`, NEEDED `libpanvk_drm.so`, SONAME `libvulkan_panfrost.so`. The private DRM library has SONAME `libpanvk_drm.so` and depends only on Android `libc.so`.
- This is important: the workflow's outer `stage/` diagnostic copy retains the pre-package `libdrm.so` dependency and its staged RUNPATH. The package script patches separate copies for the inner ADPKG. The inner package was independently extracted and inspected and has the correct private dependency and flat layout. Import only that inner ZIP.
- The packaging script is `.github/scripts/package-panvk-adpkg.sh`; it checks AArch64, SONAME, NEEDED, RUNPATH, metadata, and flat archive members.
- Metadata identifies package version `26.3.0-devel-android-kbase-4fdb9968d407`, min Android API 30, and explicitly labels the driver experimental/non-conformant with device/game compatibility unverified.
- The workflow's DRM symbol audit found no missing symbols. This is static linker/package validation, not device runtime proof.

### Matching Windows probe

- Enum-audited Windows probe artifact from run `42318169016`, artifact ID `13551667465`.
- URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42318169016/artifacts/13551667465
- Artifact SHA-256: `4788e5111be1a13b01f96b61831bda12069bd7700b34453b0a3e0fa0aeb30ddc`.
- Contains `panvk-vrs-render-probe.exe` and `tools/run-vrs-cases.cmd`; compile-time assertions cover KEEP=0, REPLACE=1, MIN=2, MAX=3, MUL=4.
- This probe is intended to exercise the five static combiner cases and capture per-case logs. Its source commit is `a2ca300629f0f38ce5aa048c92546aed06abea2f`; the Android driver is the later static-lowering fix commit above.

### Device test gate

The package is ready for a controlled Winlator runtime test. Import only the inner `PanVK-G720-experimental.zip` using Winlator Mali's custom driver importer. Keep the current wrapper and container unchanged; retain System Driver as rollback. Do not replace or patch the phone's Android system/vendor Vulkan driver or kernel.

Run `tools\\run-vrs-cases.cmd` from the Windows probe artifact in the same container and return all five logs plus the matching logcat/Wine archive. Expected effective rates for the probe's 2x2 pipeline rate with implicit 1x1 primitive and attachment rates:
- MAX (3,0): 2x2; expected pixel signature matches 2x2 KEEP.
- MIN (2,0): 1x1; expected signature matches 1x1 KEEP.
- MUL (4,0): 2x2 under the currently advertised non-strict multiply behavior; expected signature matches 2x2 KEEP.
- REPLACE (1,0): 1x1; expected signature matches 1x1 KEEP.

A valid pass requires successful target readback and matching checksums, not just submit/fence completion. Capture whether the driver logs the resolved effective rate and numeric combiner operations. If any case faults, retain its full logcat timestamps and CSF exception/sideband. Do not resume game FPS benchmarking until all static cases produce correct pixel signatures.

No device runtime test has yet been performed for this package. No system/vendor driver, kernel, or phone firmware was modified.


## 2026-11-07 18:20 UTC — VRS source audit: hardware path confirmed; probe coverage gap identified

A follow-up source audit was performed after the Android package was built.

### Hardware VRS programming is present

The v12 CSF draw implementation contains actual hardware programming in
`src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`, function
`set_tiler_idvs_flags()`:
- It marks `FSR` as a dynamic graphics state dirty bit.
- For PAN_ARCH >= 12 it reads `dyns->fsr.fragment_size` and both combiner ops.
- It packs `cfg.vrs_draw_rate`, `cfg.vrs_combiner_0`, and
  `cfg.vrs_combiner_1` into `PRIMITIVE_FLAGS_2`, then writes
  `TILER_FLAGS2`.
- If the fragment shader uses sample shading, it forces the hardware rate to
  1x1 and both combiners to KEEP.

The generated v12 XML confirms the hardware descriptor fields:
`VRS Draw Rate`, `VRS Combiner 0`, `VRS Combiner 1` in
`PRIMITIVE_FLAGS_2`. This confirms the driver has a real hardware VRS
programming path; it is not solely a fragment-coordinate software emulation.

### Compiler behavior and feature scope

- `panvk_vX_shader.c` resolves static pipeline FSR rates against implicit
  1x1 primitive and attachment rates for fragment-centre NIR lowering.
- Dynamic FSR state is explicitly excluded from that static lowering and
  currently remains on the native sample-centre path. The compiler patch
  therefore does not establish correct dynamic-FSR shader system-value
  behavior.
- The extension and `pipelineFragmentShadingRate` feature are exposed on
  PAN_ARCH >= 12; primitive and attachment features remain false.
- Do not claim full extension conformance or enable dynamic FSR in application
  workloads until the dynamic shader-centre path is separately validated.
- The static command path does program the hardware VRS rate and combiners.
  The compiler's static effective-rate calculation is intended to keep
  fragment-centre values consistent with the hardware-combined rate.

### Probe coverage gap

The current Windows render probe checks pixel-pattern signatures and successful
GPU submit/fence/readback. Its fragment shader only writes a colour derived
from `gl_FragCoord`; it does not count fragment shader invocations.
Consequently, a matching 2x2 repeated-pixel signature proves fragment-centre
behavior but does not, by itself, prove the hardware reduced invocation count
from 1x1 to 2x2.

Next probe revision should add a fragment-shader storage-buffer atomic counter
(or another independently validated invocation counter), capture counts for
1x1 and 2x2 KEEP, and require a substantial invocation reduction for 2x2.
Combiner cases should match both the expected pixel checksum and the expected
invocation count. If storage atomics alter or disable VRS behavior on this
implementation, that limitation must be recorded and a different reliable
counter path found; do not infer invocation reduction from colour repetition.

### Combiner semantics correction

A preliminary concern that the non-strict MUL exception should be applied to
the complete 2D pair was incorrect. Khronos specifies the exception per
component: when both input values are 1 in a given dimension, that dimension's
result is 2. The existing component-wise test
`a == 0 && b == 0` in log2 space is consistent with that rule. No source
change was made for this concern.

### Current test status

The Android package from source HEAD
`4fdb9968d407618cfe3f8101d409fd803574bba3` remains statically audited and
ready for a controlled static-FSR test, but it has not yet had device runtime
validation. The prior enum-audited probe remains valid for testing static
combiner/pixel signatures, but invocation reduction remains unproven. A new
invocation-counting probe should be prepared before interpreting a pixel-only
PASS as proof of true coarse fragment execution.

No system/vendor driver, kernel, or firmware was modified.


## 2026-11-07 17:35 UTC — fragment invocation counter added to VRS render probe

### Context preserved

The last device archive established:
- 1x1 KEEP pixel checksum `0x06d78d985ad50383`, equal-neighbour count 0.
- 2x2 KEEP pixel checksum `0xcad734dde7920383`, equal-neighbour count 4096; pixel-signature test passed.
- All four non-KEEP static combiners faulted with CSF `DATA_INVALID_FAULT` (`0x58`) on the earlier `c0e807c...` diagnostic driver package.
- Driver commit `4fdb9968d407618cfe3f8101d409fd803574bba3` then added compile-time resolution of static pipeline/primitive/attachment combiner rates. Its Android-only package passed exact-source build and package audits (artifact `13552276626`, inner package SHA-256 `63e4de771c1ad91cca6a89eef067d695c4d3f88dd858cc0999e255142f813e99`). That newer package has not yet had a device runtime test.
- The 2026-11-07 18:20 UTC source audit confirmed that v12 CSF writes the VRS draw-rate and combiner fields to `PRIMITIVE_FLAGS_2`, but the pixel-only probe did not measure fragment invocation reduction.

### Probe-only implementation

On `experiment/g720-fragment-shading-rate`, a diagnostic-only probe revision was pushed. It does not modify PanVK driver source, Kbase memory mapping, CSF scheduling, queue synchronization, Android kernel, or the phone's system/vendor driver.

Commits:
- `13609a23ea4bd47a9a8e4d698d729dcb88d56b39`: add an SSBO atomic counter to `tools/vrs_probe.frag`.
- `9d0aba455c38ee09968cd4b464fc7ca39f37abba`: bind the counter storage buffer in `tools/panvk_vrs_render_probe_win.c`, reset it before each draw, add a fragment-shader-write to host-read buffer barrier, and report the readback count for each render.
- `7061318e2a431e1a0a05dc2b55c49f4610929636`: clean up descriptor/buffer/memory resources on early setup failures.

The probe checks `fragmentStoresAndAtomics` before device creation and enables it for the fragment-shader SSBO atomic. It uses host-visible, host-coherent counter memory. The shader increments the counter once per fragment invocation while retaining the existing `gl_FragCoord` pixel pattern.

New acceptance conditions:
1. 1x1 KEEP and 2x2 KEEP must still pass their pixel-signature checks.
2. Both invocation counts must be nonzero.
3. 2x2 KEEP must report more than a 2x reduction in fragment invocations relative to 1x1 KEEP.
4. Each non-KEEP case must match both the expected pixel checksum and the corresponding 1x1/2x2 reference invocation count.
5. The probe logs the feature gate and every invocation count. If `fragmentStoresAndAtomics` is unavailable, the probe reports SKIP rather than claiming a VRS invocation result.

This instrumentation is diagnostic and may itself affect shader scheduling/coarse-shading eligibility. A missing reduction is therefore not by itself proof that hardware VRS is absent; correlate the reported feature, shader compilation, pixel signature, and invocation counts. If the atomic side effect prevents coarse execution, consider a supported pipeline-statistics query or another independently validated counter.

### Build state and next gate

The probe workflow `.github/workflows/build-vrs-render-probe.yml` is configured to run on changes to both modified files. The final source checkpoint is `7061318e2a431e1a0a05dc2b55c49f4610929636`. At the time of this update, no exact-head Windows probe artifact has been independently inspected, so do not ask the user to download or run this probe yet.

The Android driver package remains the already-audited static-combiner package from source `4fdb9968d407618cfe3f8101d409fd803574bba3`; no driver rebuild is required for this probe-only change. Once the exact-head Windows probe build succeeds, pair that executable with that exact inner Android package, preserving the existing Winlator wrapper/container. Run each case independently via `tools\\run-vrs-cases.cmd` and collect all five per-case logs plus the matching logcat/Wine archive.

No performance gain is claimed. Do not resume game FPS benchmarking until the static combiner cases pass both pixel and invocation comparisons. Never replace or modify the Android system/vendor Vulkan driver or kernel.


## 2026-11-07 — Exact-head VRS artifacts built and statically audited

### Exact source checkpoint and CI

The final VRS diagnostic source checkpoint is
`7061318e2a431e1a0a05dc2b55c49f4610929636` on
`experiment/g720-fragment-shading-rate`.

Both exact-head workflows completed successfully:
- Windows render probe run: `42396877986`, success.
- Android-only PanVK package run: `42396877989`, success.

Both runs report the same source SHA above. This resolves the earlier uncertainty about whether the Windows probe was built from the final invocation-counter source.

### Windows probe artifact

- Artifact: `panvk-vrs-render-probe-win64`, ID `13584215653`.
- GitHub Actions run: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42396877986
- Artifact archive SHA-256: `c289881ee62b27d3f41199f9bc12c5d41974c132970157637dabf4221123a5f2`.
- Executable: `panvk-vrs-render-probe.exe`, SHA-256
  `fa60815f71d90b78b9b588a209bd90545936cbeb26d53a868e6321c551bdaff4`.
- Verified as PE32+ Windows x86-64. Imports are limited to Windows system DLLs `KERNEL32.dll` and `msvcrt.dll`; no Vulkan loader DLL is statically imported because the probe resolves Vulkan entry points at runtime.
- Includes `tools/run-vrs-cases.cmd`, which runs KEEP, MAX, MIN, MUL, and REPLACE in separate processes and copies each case log.

### Android driver package artifact

- Artifact: `panvk-g720-android-only-experimental-package`, ID `13584176404`.
- GitHub Actions run: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42396877989
- Artifact archive SHA-256: `78d4a1835d9f23ceb06272548f9d67a28edc887d09c5c02b17559f10e8ad6105`.
- Inner Winlator/Adrenotools package: `PanVK-G720-experimental.zip`, SHA-256
  `5e1f8080ec6d32bb57a743224d5ef31973debb9a1e086904d8bf6780968b204d`.
- Packaged ICD SHA-256:
  `6b03eac29efcea0aae28df228c98e7e9da6d58da6a170803b9fc2928ce282b7c`.
- Private DRM dependency SHA-256:
  `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`.
- Both shared objects are ELF64 AArch64 Android API 30 / NDK r30.
- The final ADPKG is flat and contains `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- The packaged ICD has `RUNPATH=$ORIGIN` and depends on the private `libpanvk_drm.so`, whose SONAME is also `libpanvk_drm.so`. This avoids accidentally resolving a platform-provided `libdrm.so` with a mismatched ABI. The outer CI artifact also contains the staged/audit tree; the user should import only the inner `PanVK-G720-experimental.zip`.
- The build's DRM symbol audit reports no missing symbols for the bundled Android libdrm before packaging. The package script then renames the dependency and patches the ICD NEEDED entry/RUNPATH; the packaged ELF dynamic sections were independently inspected after extraction and match that intended layout.
- This is an experimental custom Winlator ICD only. It is not Vulkan conformant, and runtime/device/game compatibility remains unverified. No system/vendor driver, kernel, or firmware was modified.

### User test is now required

The exact-head Windows probe and matching Android package are now available from the two GitHub Actions run pages above. The next step cannot be completed without device-side testing.

Import the inner `PanVK-G720-experimental.zip` in Winlator Mali 1.2's custom driver importer. Do not replace the phone's system/vendor Vulkan driver. Keep the existing wrapper and container unchanged, and retain System Driver as rollback. Then run `tools\\run-vrs-cases.cmd` from the Windows probe artifact in the same container.

Return all five generated logs (`panvk_vrs_keep.log`, `panvk_vrs_max.log`, `panvk_vrs_min.log`, `panvk_vrs_mul.log`, `panvk_vrs_replace.log`) and the matching Winlator/logcat archive. Do not run Borderlands FPS benchmarks yet: first establish whether all static cases render correctly and whether the SSBO invocation counter detects a greater-than-2x reduction for 2x2 KEEP. A counter result without a reduction may be affected by the atomic instrumentation itself and must not be treated as conclusive evidence that VRS hardware is absent.



## 2026-11-07 — Device VRS results: KEEP proven; non-KEEP combiner path faults

### Uploaded device test results

The user ran the exact-head probe and Android package from source
`7061318e2a431e1a0a05dc2b55c49f4610929636` on Xiaomi 15T / Mali-G720 MC7,
Android 16, Winlator Mali 1.2.1.

The probe identified:
- Device: `Wrapper(Mali-G720 MC7)`
- Driver: `panvk (Mesa 26.3.0-devel (git-7061318e2a))`
- Vulkan API: 1.3.354
- `pipelineFragmentShadingRate=1`
- `fragmentStoresAndAtomics=1`

**KEEP is a real hardware VRS pass, including measured fragment invocation reduction:**
- 1x1 KEEP: successful submit/fence/readback; checksum
  `0x06d78d985ad50383`; equal-neighbour count 0; fragment invocations 4096.
- 2x2 KEEP: successful submit/fence/readback; checksum
  `0xcad734dde7920383`; equal-neighbour count 4096; fragment invocations 1024.
- Pixel signature passed.
- Invocation reduction passed: 4096 -> 1024 = 4x fewer invocations (75% reduction).
- `panvk_vrs_render_probe.log` ended with `VRS GPU RENDER PASS`.

This is the first direct runtime proof in this project that the G720 v12 VRS draw-rate path actually reduces fragment shader invocation count. The SSBO atomic counter did not prevent coarse execution for KEEP.

### Non-KEEP combiner failures

The isolated MAX, MIN, MUL, and REPLACE cases all failed at
`vkWaitForFences` with `VkResult=-4` (device lost). They passed image,
pipeline, command-buffer, and queue-submit creation; failure occurred during
GPU execution.

The matching Winlator logcat records CSF recoverable faults during the
non-KEEP tests:
- Three occurrences of exception `0x58` (`DATA_INVALID_FAULT`), followed by
  CSI 1 `0x0f` resource-terminated notifications.
- One occurrence of exception `0x4b` (`CS_INHERIT_FAULT`).
- The CS error reports identify subqueue 0 and show the sideband address
  associated with the CS stream for the `0x58` failures.

Do not attribute these faults to the fragment SSBO atomic alone: the earlier
pixel-only probe also faulted on non-KEEP operations, and the current trace
reports the first `0x58` fault on CSI 0 before the secondary CSI 1
resource-termination notification. The precise offending CS state/register
still needs isolation.

The checked-in v12 GenXML defines VRS combiner values
KEEP=0, REPLACE=1, MIN=2, MAX=3, MUL=4 and places them in
`PRIMITIVE_FLAGS_2`; the v12 register map includes `TILER_FLAGS2`.
Thus the Vulkan enum numeric values match the declared hardware enum values.
See the v12 definitions in `src/panfrost/genxml/v12.xml`; do not “fix” this
by changing enum numbers without new evidence.

### Conservative capability correction in progress

Because all four non-KEEP modes fault on the actual G720 device, the current
source no longer advertises `fragmentShadingRateNonTrivialCombinerOps`.
Pipeline VRS with KEEP remains enabled. This prevents compliant applications
from assuming non-KEEP combiners work while preserving the proven 2x2 KEEP
path.

Source commits on `experiment/g720-fragment-shading-rate`:
- `a1ffee93892b25a05faa0eb558dc46331093c3b8`: report
  `fragmentShadingRateNonTrivialCombinerOps=false` on this experimental
  PanVK build, with a comment documenting the observed device fault.
- `098d62ef7814852dd2096721d38bbb033abd16fc`: log the actual packed
  `TILER_FLAGS2` word and the requested rate/combiner values whenever a
  non-KEEP state reaches draw emission.
- `955fcede05c0381e73fc79f78e15f5ea65a9fdd4`: make the Windows probe skip
  non-KEEP tests when the property is not advertised, rather than knowingly
  issuing unsupported operations.
- `9088e0326a9e398209b2c5500655f293e6f8adf0`: add
  `tools/run-vrs-keep-check.cmd`.
- `af52d3d5e9a3eeadfa836da95ec6771ac11bcbf8`: update the Windows workflow to
  package the KEEP-only regression script.
- `deeb2ab9ff7b6b26893d4c9679f85e556dfc67a3`: fix the probe's SKIP log
  newline. This is the exact current source checkpoint for the next package.

### Build state and next gate

The updated Windows probe workflow completed its compile and upload steps for
exact source `deeb2ab9ff7b6b26893d4c9679f85e556dfc67a3`:
- Run: `42398543208`
- Artifact: `panvk-vrs-render-probe-win64`, ID `13584586137`
- Artifact SHA-256:
  `2b08c90efc061cee7d0e0e65dde68cccfe5389df7a666b3ce8824621b33b411c`
- It includes both `tools/run-vrs-cases.cmd` and
  `tools/run-vrs-keep-check.cmd`.

The matching Android-only package build for the same exact source checkpoint
is run `42398543226`; at the time of this handoff update it is still running
and has not uploaded an artifact. Do not ask the user to test the new package
until that exact-head Android build and package audit complete successfully.

Once available, import only the inner `PanVK-G720-experimental.zip` from the
outer CI artifact, keep the Winlator container and wrapper unchanged, and run
`tools\\run-vrs-keep-check.cmd`. Expected: the device still reports
`pipelineFragmentShadingRate=1`, now reports
`nonTrivialCombinerOps=0`, and repeats the KEEP pixel signature and
4096-to-1024 invocation reduction without submitting any non-KEEP combiner.
Return `panvk_vrs_keep.log` and the matching logcat archive.

No Borderlands FPS benchmark should be run until the new package passes this
KEEP regression check. No Android system/vendor driver, kernel, or firmware
was modified.


## 2026-11-07 18:06 UTC — Conservative VRS capability build completed

### Compile failure and correction

The first Android build attempt after adding the packed-register diagnostic failed during
`csf/panvk_vX_cmd_draw.c` compilation because the diagnostic referred to
`frag_w`, `frag_h`, `op0`, and `op1` outside the preprocessor/C scope
where they were declared. This was caught by CI before any artifact was published.

- Failed build run: `42398543226` (source `deeb2ab9ff7b6b26893d4c9679f85e556dfc67a3`).
- Corrective commit: `c9d61e10a9c3489d8193a8fb0767f4b5187d79f3`.
- The correction moves the VRS state declarations into the `PAN_ARCH >= 12`
  scope surrounding the packing operation, and emits the diagnostic after
  `pan_pack` has populated `tiler_flags_2`.
- The corrected build compiled all architecture variants and passed the full
  Android-only build, staged-file/dependency audit, and ADPKG packaging steps.

### Current Android package

Exact source: `c9d61e10a9c3489d8193a8fb0767f4b5187d79f3`.

- Successful Android-only workflow run: `42398941271`.
- GitHub Actions page:
  https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42398941271
- Outer CI artifact: `panvk-g720-android-only-experimental-package`,
  artifact ID `13584986539`.
- Outer artifact SHA-256:
  `0cb6fdc5da9af10c06d8fe7efed511511686f62c34834976e0c78a26862e88e5`.
- Inner Winlator/Adrenotools package: `PanVK-G720-experimental.zip`.
- Inner package SHA-256:
  `12f1ffe5f38c442b80c22cc56b93cff6fe5fead87231cf7068ceed4165a4aace`.
- ICD SHA-256:
  `fb7cc3b4697f82b3abc6c535cf5901380d18ea16ed593bd19e98d7dad936444e`.
- Private DRM dependency SHA-256:
  `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`.
- Independently extracted and verified: valid flat ZIP; `meta.json` parses;
  ICD is ELF64 AArch64; ICD RUNPATH is `$ORIGIN`; ICD NEEDED entry is
  `libpanvk_drm.so`; private dependency SONAME is `libpanvk_drm.so`.
  The packaged ICD does not depend on `libdrm.so`. All inner ZIP entries
  passed CRC checks and match `SHA256SUMS`.
- Import only this inner ZIP from the outer CI artifact. Do not import the
  outer archive or the staged `stage/` tree.

### Matching Windows probe

The Windows probe build from source `deeb2ab9ff7b6b26893d4c9679f85e556dfc67a3`
is compatible with the Android package above: the subsequent driver-only
commit `c9d61e1...` did not modify the probe, shader, or Windows workflow.
Its artifact is `panvk-vrs-render-probe-win64`, ID `13584586137`, from run
`42398543208`:
https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42398543208

The archive SHA-256 is
`2b08c90efc061cee7d0e0e65dde68cccfe5389df7a666b3ce8824621b33b411c`.
It contains `panvk-vrs-render-probe.exe`, the five-case script
`tools/run-vrs-cases.cmd`, and the new single-case script
`tools/run-vrs-keep-check.cmd`. The executable SHA-256 is
`e50619979a46d949212e159e959d7868123e4cb02a3f2bb4f543eba013b81d2b`
and its PE machine field is AMD64 (`0x8664`).

### Next device gate

The user is needed for one final KEEP-only regression check against the new
capability-conservative driver:

1. Download both artifacts from the GitHub Actions run pages above.
2. Import only the inner `PanVK-G720-experimental.zip` from the Android
   artifact into Winlator Mali 1.2.
3. Keep the existing container and wrapper unchanged; retain System Driver as
   rollback. Do not replace or modify Android's system/vendor Vulkan driver
   or kernel.
4. Run `tools\\run-vrs-keep-check.cmd` from the Windows probe artifact in
   the same container.
5. Return `tools\\panvk_vrs_keep.log` (or the copied
   `panvk_vrs_keep.log`) and the matching Winlator/logcat archive.

Expected result: `pipelineFragmentShadingRate=1`,
`nonTrivialCombinerOps=0`, 1x1 KEEP invocation count 4096, 2x2 KEEP
invocation count 1024, pixel-signature PASS, invocation-reduction PASS, and
`VRS GPU RENDER PASS`. This confirms that the safe property report did not
regress the already-proven KEEP path. Non-KEEP operations should be skipped
by the updated probe because they are no longer advertised.

Do not resume game FPS benchmarking until this regression passes. The
non-KEEP combiner hardware/CSF fault remains an open investigation; the
packed `TILER_FLAGS2` diagnostic is now available if a later controlled
test is needed. No performance gain beyond the proven KEEP invocation
reduction is claimed.


## 2026-11-08 00:17 UTC — New-build VRS KEEP regression passed

### Device result

The user ran the KEEP-only render probe against the corrected Android package built
from PanVK source commit
`c9d61e10a9c3489d8193a8fb0767f4b5187d79f3` on the Mali-G720 MC7.

The probe reports:
- Device: `Wrapper(Mali-G720 MC7)`
- Driver: `panvk (Mesa 26.3.0-devel (git-c9d61e10a9))`
- Vulkan API: 1.3.354
- `pipelineFragmentShadingRate=1`
- `nonTrivialCombinerOps=0`
- `strictMultiplyCombiner=0`
- `fragmentStoresAndAtomics=1`

Both render cases completed successfully:
- 1x1 KEEP: queue submit, fence wait, and readback PASS; checksum
  `0x06d78d985ad50383`; equal-neighbour count 0; fragment invocations 4096.
- 2x2 KEEP: queue submit, fence wait, and readback PASS; checksum
  `0xcad734dde7920383`; equal-neighbour count 4096; pixel signature PASS;
  fragment invocations 1024.
- Invocation reduction PASS: 4096 -> 1024, exactly 4x fewer fragment
  invocations (75% reduction).
- Final probe status: `VRS GPU RENDER PASS`.

This verifies that the capability-conservative property report
(`fragmentShadingRateNonTrivialCombinerOps=false`) did not regress the
working pipeline KEEP path. The tested package is the exact corrected driver
commit above, not the older `git-7061318e2a` package.

### Log availability and interpretation

The user could not attach the Winlator/logcat archive because of attachment
limits. The standalone KEEP-only probe output was pasted directly and contains
all required acceptance data for this regression test. No logcat-based
cross-check of kernel/CSF warnings is available for this run; this does not
invalidate the successful Vulkan submit/fence/readback and invocation-count
results. Do not request another archive solely to close this KEEP regression.

### Current VRS status and next work

- Pipeline fragment shading rate with KEEP is runtime-proven on this G720:
  2x2 rate produces the expected coarse pixel signature and a measured 75%
  reduction in fragment shader invocations versus 1x1.
- Non-trivial combiners remain conservatively unadvertised. Their earlier
  MAX/MIN/MUL/REPLACE tests caused device loss; the root cause is still open.
- Do not re-enable `fragmentShadingRateNonTrivialCombinerOps` without a
  separate controlled hardware test and successful correctness validation.
- The next investigation can proceed in source/CI without another user action:
  isolate the non-KEEP combiner state programming and review the packed
  `TILER_FLAGS2` diagnostic path. Keep KEEP as the known-good baseline.
- Do not claim a game-performance gain from the 75% invocation reduction
  alone. First validate application-level use, visual quality, and frame-time
  impact before making any FPS claim.
- No Android system/vendor driver, kernel, or firmware was modified.


## 2026-11-08 — Vulkan FSR combiner capability correction and REPLACE probe

### Specification correction

A review of the current Vulkan specification found that
`fragmentShadingRateNonTrivialCombinerOps=false` does **not** prohibit
`VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR`. The property only
controls whether operations other than KEEP or REPLACE (MIN, MAX, MUL) may
be used. Therefore the previous probe logic that skipped REPLACE whenever
the property was false was over-restrictive, and the earlier description of
REPLACE as an unsupported non-trivial operation was incorrect.

The current Vulkan specification states that:
- KEEP and REPLACE are permitted when this property is false.
- MIN, MAX, and MUL require the property to be true.
- `fragmentShadingRateStrictMultiplyCombiner` must be false when
  `fragmentShadingRateNonTrivialCombinerOps` is false.

Reference: Vulkan specification, fragment shading rate properties and
`vkCmdSetFragmentShadingRateKHR` valid usage:
https://docs.vulkan.org/spec/latest/chapters/limits.html
https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdSetFragmentShadingRateKHR.html

### Probe correction

On `experiment/g720-fragment-shading-rate`:
- Commit `ec70a4a0aad4621527b66bca71dd3c206721c9dc` changes the probe gate so
  REPLACE is tested even when `fragmentShadingRateNonTrivialCombinerOps=0`;
  MIN/MAX/MUL remain skipped.
- Commit `d00064758eb0e1697593672fdbaa3a0cdde58052` adds
  `tools/run-vrs-replace-check.cmd`.
- Commit `39093235c5a39f2bce0aa7af8fa45911968f821b` includes the new script
  in the Windows probe workflow and artifact.
- Commit `94bcead1ab18d3aae0b7db827f4edff61b4c11e6` tidies the diagnostic.
- Driver source and Android package are unchanged by this probe-only work.

### Next device test

After the updated Windows probe artifact finishes building, test
`tools/run-vrs-replace-check.cmd` with the known-good Android package
(`git-c9d61e10a9`, property `nonTrivialCombinerOps=0)). The script produces
`panvk_vrs_replace.log`; only that small text log is required, not a Winlator
logcat archive.

The earlier REPLACE/KEEP expected-result paragraph was superseded by the
valid-usage correction below: REPLACE cannot be tested unless primitive or
attachment FSR is enabled for the selected combiner slot.

Keep MIN/MAX/MUL disabled pending separate investigation. The KEEP 2x2 path
remains the verified baseline (4096 -> 1024 fragment invocations).


## 2026-11-08 — Corrected REPLACE test validity gate

A second Vulkan valid-usage review refined the previous addendum: although
REPLACE is permitted when `fragmentShadingRateNonTrivialCombinerOps=false`,
it is only legal in a combiner slot whose source feature is enabled:
- combinerOps[0] requires `primitiveFragmentShadingRate`;
- combinerOps[1] requires `attachmentFragmentShadingRate`.
Pipeline FSR alone does not make either non-KEEP slot legal. The current
probe enabled only `pipelineFragmentShadingRate`, so its historical
REPLACE test was invalid when primitive/attachment FSR were disabled. This
may explain why that case faulted; it must not be treated as proof that the
hardware cannot implement REPLACE.

Probe changes on `experiment/g720-fragment-shading-rate`:
- `3e8381454050a6dc8f6b28f114aad5c70737503a`: report all three feature
  booleans; enable primitive/attachment features only when reported; gate
  combiner tests on the corresponding input feature; select op0 when
  primitive FSR is supported, otherwise op1 when attachment FSR is supported.
- `b05d2f8fe58bc7b6185580403a688890e7f9c976`: diagnostic formatting only.
- Existing `tools/run-vrs-replace-check.cmd` invokes the isolated REPLACE
  test and writes `panvk_vrs_replace.log`; workflow includes it in the
  Windows artifact.
- No Android driver source or package changed.

The new probe must first report
`pipelineFragmentShadingRate`,
`primitiveFragmentShadingRate`, and
`attachmentFragmentShadingRate`.
If both primitive and attachment features are false, it will correctly
SKIP REPLACE rather than issuing invalid Vulkan state. If either is true,
it enables that feature and places REPLACE in the corresponding legal
combiner slot. MIN/MAX/MUL remain gated by
`fragmentShadingRateNonTrivialCombinerOps`.

Next device action, once the updated Windows probe artifact is available:
run `tools/run-vrs-replace-check.cmd` with the known-good Android driver
package and paste only `panvk_vrs_replace.log`. No Winlator logcat archive
is required. This is a probe-only rebuild; the Android package remains
`git-c9d61e10a9`. Do not infer hardware REPLACE support until a legal
test actually runs and passes.


## 2026-11-08 — REPLACE probe artifact built and ready

The corrected Windows-only VRS probe has now completed successfully.

- Probe source commit: `b05d2f8fe58bc7b6185580403a688890e7f9c976`.
- Workflow run: `42429775774` (run #28), conclusion: **success**.
- Workflow page: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42429775774
- Artifact: `panvk-vrs-render-probe-win64`, artifact ID `13596813051`.
- Artifact SHA-256: `c4b80fd9643935f6df39e77b1f114ea4ed32f5506d337338f6d0a94338c9f097`.
- Artifact expires 2027-02-06.

The artifact includes the isolated `tools/run-vrs-replace-check.cmd` script.
Use this probe with the already validated Android package from source
`c9d61e10a9c3489d8193a8fb0767f4b5187d79f3`; no Android driver reinstall or
new package is needed for this feature-query test.

### Required device check

1. Download `panvk-vrs-render-probe-win64` from the successful workflow run above.
2. Extract it inside the existing Winlator container.
3. Run `tools\\run-vrs-replace-check.cmd`.
4. Paste only `panvk_vrs_replace.log` here. Do not send a logcat archive.

The probe prints the three FSR feature booleans. If both
`primitiveFragmentShadingRate` and `attachmentFragmentShadingRate` are
false, REPLACE is correctly skipped and the result is still useful: it means
the current PanVK device feature exposure does not provide a legal combiner
input path. If either input feature is true, the probe enables that feature
and tests REPLACE only in its corresponding legal combiner slot.

This is diagnostic work only. Do not import any new Android driver package,
change the container, or alter the system/vendor driver or kernel. Keep the
previously validated 2x2 KEEP result as the baseline (4096 to 1024 fragment
invocations; 75% reduction). No game FPS improvement is claimed.

## 2026-11-08 — Device result: no legal non-KEEP combiner input

User ran the corrected artifact on the target Mali-G720 MC7 using the known
working PanVK build (git-c9d61e10a9). Results:

- Driver: PanVK Mesa 26.3.0-devel, API 1.3.354.
- pipelineFragmentShadingRate=1.
- primitiveFragmentShadingRate=0.
- attachmentFragmentShadingRate=0.
- fragmentShadingRateNonTrivialCombinerOps=0.
- strictMultiplyCombiner=0.
- Device creation and SSBO invocation-counter setup succeeded.
- 1x1 KEEP render/readback passed; checksum 0x06d78d985ad50383;
  fragment invocations: 4096.
- 2x2 KEEP render/readback passed; checksum 0xcad734dde7920383;
  pixel signature passed; fragment invocations: 1024.
- Invocation reduction: 4x (75% fewer fragment invocations).
- REPLACE was correctly skipped because neither primitive nor attachment
  FSR is supported by the currently exposed feature set.
- Overall result: VRS GPU RENDER PASS.

### Interpretation

This is a clean positive result for pipeline-rate FSR, and a definitive
feature-query result for this PanVK build: only the pipeline rate input is
exposed. It is not evidence that Mali-G720 hardware lacks primitive or
attachment rate capabilities; PanVK may simply not implement or expose
those paths. Do not force either feature bit to true. Vulkan feature bits
promise complete valid behavior; exposing them without matching command,
state, and attachment implementation would be incorrect and risks device
loss.

No Android driver change was made. The verified pipeline 2x2 KEEP path is
preserved as a baseline. This result does not yet demonstrate game FPS gains.

### Engineering decision / next source investigation

Do not spend another device run on REPLACE with the current binary: it will
continue to skip. The next source-level task is to audit the Mali-G720
architecture-12 implementation for actual primitive and attachment FSR paths:

1. Architecture-12 feature reporting in the per-architecture physical-device
   feature implementation.
2. vkCmdSetFragmentShadingRateKHR primitive-rate state and primitive-rate
   output from the primitive/tiler path.
3. Attachment-rate image format, usage, and layout restrictions, plus
   fragment descriptor and state programming.
4. Hardware-generation-specific register/descriptor support and Vulkan limits.

Compare against upstream Mesa PanVK v12 and the relevant GenXML definitions;
do not infer capability solely from the G720 product specification. Expose
the feature only after a complete implementation path is verified.

Device retest required: none at this stage. Continue source audit first.


## 2026-11-08 — VRS source audit after the KEEP device pass

Audited the device-tested VRS driver source
`c9d61e10a9c3489d8193a8fb0767f4b5187d79f3`, the v12 GenXML, and the
current VRS probe sources. This supersedes the earlier plan to immediately
investigate non-KEEP hardware combiners: the corrected Vulkan feature gate
shows that no non-KEEP combiner input is legally enabled in the current build.

### Source findings

- PanVK advertises `VK_KHR_fragment_shading_rate` on v12+ and reports
  `pipelineFragmentShadingRate=true`, but
  `primitiveFragmentShadingRate=false` and
  `attachmentFragmentShadingRate=false`.
- `fragmentShadingRateNonTrivialCombinerOps=false` remains conservative.
- The pipeline path packs the draw rate and combiner fields into v12
  `PRIMITIVE_FLAGS_2`, then writes `IDVS.TILER_FLAGS2`.
- The v12 GenXML contains `Framebuffer Parameters.VRS Image Rate` and
  `VRS Image Plane` fields, but the inspected PanVK CSF framebuffer
  construction and `pan_fb.c` paths do not program a VRS image attachment.
- No PanVK v12 shader-output path was found that consumes
  `PrimitiveShadingRateKHR` and emits a per-primitive rate. Keep primitive
  and attachment feature bits false.

### Important dynamic-state gap

The static pipeline path is the only VRS render path currently proven.
`panvk_get_static_pipeline_vrs_rate()` rejects dynamic FSR state;
`panvk_compile_nir()` leaves dynamic state on the native sample-center path,
while its adjacent comment says the Bifrost/Valhall compiler does not
implement `nir_load_frag_shading_rate()`. The existing capability probe
records `vkCmdSetFragmentShadingRateKHR` but does not submit a draw. The
render probe tests static pipeline FSR and `gl_FragCoord`, not dynamic FSR.

Therefore the 1x1/2x2 static KEEP result remains valid, but dynamic FSR draw
execution and fragment-center correctness are not yet established. Do not
claim full extension validation until this is tested and, if necessary,
implemented.

### Combiner validity correction

Because both primitive and attachment FSR features are false, Vulkan valid
usage requires combiner slot 0 and slot 1 to be KEEP, respectively. The
non-trivial-combiner property is also false. The earlier non-KEEP device-loss
cases were not legal tests under the current feature set and do not prove the
G720 hardware lacks those operations. The corrected probe's SKIP is expected.
Do not force-enable the feature bits.

### Next work

1. Extend the Windows render probe with a dynamic-state 2x2 KEEP draw using
   `VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR` and
   `vkCmdSetFragmentShadingRateKHR`; use the existing static 1x1/2x2
   renders as controls. The dynamic case must submit, wait, read back,
   compare the `gl_FragCoord` pixel signature, and count fragment
   invocations.
2. Build and independently inspect the exact-head Windows probe artifact
   before asking the user to run it. No Android driver reinstall is needed
   for this probe-only test; retain the known-good `git-c9d61e10a9` package.
3. Test all advertised pipeline rates at their advertised sample counts
   before considering the rate list fully runtime-qualified.
4. Keep primitive and attachment features false until complete shader-output
   and framebuffer/image paths, format/usage/layout/synchronization behavior,
   and CTS coverage exist.

No Android driver source, kernel, firmware, or system/vendor driver changed
in this audit. No new device test is requested until the dynamic render probe
is built and independently checked.

Detailed VRS source audit:
[docs/G720-VRS-IMPLEMENTATION-AUDIT.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-fragment-shading-rate/docs/G720-VRS-IMPLEMENTATION-AUDIT.md).


## 2026-11-08 — dynamic FSR probe built and artifact verified

The source audit identified dynamic pipeline FSR as the next unverified path.
Added `--case=dynamic-keep` to the Windows render probe. It creates a
pipeline with `VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR`, records
`vkCmdSetFragmentShadingRateKHR(2x2, KEEP/KEEP)`, submits a draw using the
existing `gl_FragCoord` pattern, waits, reads back, and compares pixel
signature and invocation count against static 2x2 KEEP. Static 1x1/2x2
controls remain in the test.

Build and artifact verification:
- Source commit: `d793346dc75ab8e58676b39d41afbee75aa77c83`
- Workflow run #32: success
  https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42431201125
- Artifact: `panvk-vrs-render-probe-win64`, ID `13597576380`
- ZIP SHA-256:
  `9b54bf53664d2ff16923eb621a4c9ac93e0bcce543b1ac60d44f4c648b67696b`
- Executable SHA-256:
  `f0e7fc5d84645bf6f4927b4adf5ec1ee5b447d2187eca088b44d64a9d1d083a8`
- Downloaded the artifact and independently checked the ZIP contents, PE32+
  x86-64 executable type, dynamic-case strings, and included script.

### User test required next

1. Download the artifact from the Actions run above.
2. Extract it and run `tools/run-vrs-dynamic-check.cmd`.
3. Return only `panvk_vrs_dynamic.log`.

This is a Windows probe-only artifact; it does not contain the Android
driver. Do not install or change the working Android package. Keep
`git-c9d61e10a9` selected. The dynamic test uses legal 2x2 KEEP/KEEP state;
do not run the older REPLACE/MIN/MAX/MUL cases.

Detailed audit:
[docs/G720-VRS-IMPLEMENTATION-AUDIT.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-fragment-shading-rate/docs/G720-VRS-IMPLEMENTATION-AUDIT.md).


## 2026-11-08 — dynamic pipeline FSR device test result

The user ran `tools/run-vrs-dynamic-check.cmd` using the existing Android
PanVK package `git-c9d61e10a9`. The dynamic probe completed all Vulkan
operations without device loss, but the final render validation failed.

### Exact device measurements

| Case | Submit/fence/readback | Checksum | Equal neighbours | Fragment invocations |
|---|---|---|---:|---:|
| Static 1x1 KEEP | PASS | `0x06d78d985ad50383` | 0 | 4096 |
| Static 2x2 KEEP | PASS | `0xcad734dde7920383` | 4096 | 1024 |
| Dynamic 2x2 KEEP | PASS | `0x0e61401d4af57b83` | 4096 | 1024 |

Static 2x2 pixel signature PASS. Dynamic 2x2 pixel signature FAIL against
static 2x2. Dynamic invocation count equals static 2x2 (1024), proving that
the dynamic command produces coarse 2x2 fragment execution, but not that
fragment-coordinate/sample-centre semantics are correct. Overall probe
result: `VRS GPU RENDER FAIL` because of the dynamic pixel-signature
mismatch, not because of submit, fence, or readback failure.

### Source-level likely cause

Rechecked `panvk_vX_shader.c` and the CSF draw path at the device-tested
source checkpoint:

- `panvk_get_static_pipeline_vrs_rate()` rejects
  `MESA_VK_DYNAMIC_FSR`.
- Static FSR lowering replaces
  `nir_intrinsic_load_sample_pos_or_center` using the statically resolved
  fragment size.
- `panvk_compile_nir()` explicitly leaves dynamic FSR on the native
  sample-centre path; the source comment says Bifrost/Valhall does not
  currently implement `nir_load_frag_shading_rate()`.
- The CSF draw path does write dynamic fragment size/combiner state into
  `IDVS.TILER_FLAGS2`, consistent with the measured 1024 invocations.

The leading hypothesis is a missing dynamic fragment-centre/sysval lowering:
hardware coarse shading works, while observable fragment coordinates differ
from static 2x2. This is not yet proven as the root cause; do not patch
compiler behavior until a focused probe isolates it.

### Next engineering work

1. Audit NIR fragment-rate/sample-position intrinsics and Bifrost/Valhall
   sysval lowering to establish whether dynamic rate can be read in shader
   code or reconstructed from command state.
2. Extend the probe to isolate `gl_FragCoord.xy` and invocation identity,
   and add a dynamic-vs-static shader that does not read `gl_FragCoord`.
   This will separate coarse shading behavior from fragment-coordinate
   lowering.
3. Only then implement a correct dynamic sample-centre path if the backend
   can support it. Rerun static 1x1/2x2 controls and dynamic 2x2.
4. Keep primitive and attachment FSR features false. Do not claim complete
   dynamic FSR support until pixel semantics and relevant CTS coverage pass.

Detailed source audit and full measurements:
[docs/G720-VRS-IMPLEMENTATION-AUDIT.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-fragment-shading-rate/docs/G720-VRS-IMPLEMENTATION-AUDIT.md)
(updated in commit `40a93afad85ba0cf5d710a490ef94882baf2e8a4`).

No Android driver source was changed, and no new device test is requested
until the next probe isolates the fragment-coordinate discrepancy.


### 2026-11-08 follow-up: compiler path narrowed

A further source trace found a specific likely cause for the dynamic pixel
signature mismatch:

- `src/panfrost/compiler/pan_nir_lower_sample_position.c` lowers
  `nir_intrinsic_load_sample_pos_or_center` to constant `(0.5, 0.5)`
  whenever sample shading is disabled.
- The PanVK static FSR lowering runs before this pass and substitutes a
  rate-derived fragment center (for static 2x2, `(1.0, 1.0)`).
- Dynamic FSR is intentionally not handled by that static pass, so the
  backend's normal `(0.5, 0.5)` lowering remains, while the CSF draw path
  still programs the dynamic 2x2 rate.

This explains the measured 1024 invocation count matching static 2x2 while
the coordinate-derived output checksum differs. It is strongly supported
by source, but still a leading diagnosis because the probe's pixel signature
depends on `gl_FragCoord`.

Next probe improvement: add a second fragment shader that increments the
same SSBO counter but writes constant colour without reading
`gl_FragCoord`. Compare static and dynamic 2x2 with that shader, then
compare the existing coordinate-reading shader. If constant-colour output
matches and coordinate output does not, the issue is isolated to
sample-centre/fragment-coordinate lowering. Do not hardcode a dynamic 2x2
center: a correct implementation must track the effective dynamic rate,
support other advertised rates, and preserve multisample/sample-shading
semantics.

Detailed audit updated:
[docs/G720-VRS-IMPLEMENTATION-AUDIT.md](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-fragment-shading-rate/docs/G720-VRS-IMPLEMENTATION-AUDIT.md)
(commit `047f8244c39169af40f48001c3d1b1cf7ddc3fb6`).


## 2026-11-08 — dynamic FSR isolation probe engineering

The device's dynamic 2x2 KEEP run completed submit/fence/readback and matched
the static fragment invocation count (1024), but the `gl_FragCoord`-derived
pixel checksum differed from static 2x2. The leading source-level hypothesis
is a fragment-centre lowering mismatch: static FSR resolves the 2x2 centre,
while dynamic FSR bypasses the static NIR rewrite. This is not yet proven.

A constant-colour fragment shader (same SSBO atomic counter, no
`gl_FragCoord`) and a new `--case=dynamic-isolation` runner now compare static
versus dynamic 2x2 hashes and invocation counts. A pass for constant-colour
output/counts alongside the existing coordinate mismatch would isolate the
problem to sample-centre/coordinate semantics. A failure in the constant
colour case would instead point to a broader dynamic state issue. Do not patch
the compiler or change the Android driver until this discriminating result is
available. User testing waits until the Windows probe artifact builds and is
verified.


### Probe build correction / current CI state (2026-11-08)

The first compile attempt exposed two harness errors: the constant shader's
SPIR-V array was not yet embedded by the workflow revision used for that run,
and the coordinate-render result was referenced outside its local scope.
The workflow now compiles and embeds `vrs_probe_constant.frag`, and the
isolation case explicitly re-renders the coordinate shader dynamically in
its own scope. Log newline formatting was also corrected.

Fix commits on `experiment/g720-fragment-shading-rate`:
- `d32ac72d15a659c80d40a72865696b64be4ed9de` — scope dynamic coordinate comparison.
- `68e1817825671ded7cf4b4d79cc8f9004ad59207` — clean isolation log formatting.

The first failed Actions run was `42478566443` (compile failure, not device
failure). A new build for the corrected source is queued as run
`42478657493`; another earlier queued run is `42478639094`. Do not distribute
or run an artifact until a corrected build succeeds and its ZIP contents and
hashes have been checked. No Android driver update or device-side action is
needed yet.


Additional isolation-probe guard: the coordinate-reading dynamic rerender now
logs and asserts its fragment invocation count against static 2x2. The test
only reports the intended sample-centre diagnosis if both shader variants
execute at the same coarse invocation count and only the coordinate-derived
pixels differ. Latest source commit: `c3ca013ac4568624a40aa97f0cb43f04ea52a3e7`.


Final harness QA: two diagnostic log format strings were corrected from
literal backslash-n output to actual line breaks. Latest source commit:
`09d9684e45201933f0301175dc912a4a39606067`. An earlier successful Windows
probe build is available, but it predates the coordinate invocation-count
assertion and final log formatting fix; do not send that artifact for device
testing. The latest build is queued. Keep current Android driver installed;
no user-side action until the latest artifact is built and verified.


## 2026-11-08 12:08 UTC — scheduled build monitor refresh

Read the canonical handoff and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` before checking the active performance-audit branch and GitHub Actions.

- Active performance-audit branch at the start of this pass: `a8a6e5b904de570dc9984d45b5b0890c7ad98a0d`; latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`. Latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. The intervening branch changes are documentation-only; no driver or workflow changes were found.
- Latest Android-only run [42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`: SUCCESS; all returned steps completed, pending 0, failed 0.
- Latest wrapper run [42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167): Release job `127885224388` and Debugoptimized job `127885224836` both SUCCESS; all returned steps completed, pending 0, failed 0. The original monitored runs `41947103034` and `41947103085` are superseded.
- No failure logs were needed; no builds were triggered.
- Android-only artifact `13501237590` was downloaded again. Outer SHA-256 `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a` and nested package SHA-256 `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86` match the GitHub artifact digest and prior record. ZIP integrity, all three embedded member hashes, metadata JSON, staged ICD JSON, AArch64/API 30 ELF headers, private libdrm dependency, and empty missing-DRM-symbol report were rechecked. Classification remains **static/package validated, not runtime validated**.
- Wrapper Release artifact `13502135385`: outer SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; extracted ELF SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Wrapper Debugoptimized artifact `13502061037`: outer SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; extracted ELF SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`. Both are AArch64 Android API 30 ELF files, but each ZIP contains only the raw library; RUNPATH expects `$ORIGIN/../../android_stub`, and libdrm plus X11/XCB dependencies are not bundled. They remain **not standalone-package validated**.
- Next gate: test only the flat Android-only package through Winlator Mali's separate custom-driver importer; gather runtime logs before claiming device success. Wrapper dependency closure remains unresolved.
- No source, stock/vendor driver, or live kernel was modified.

The detailed step list, manifest fields, dependency analysis, and artifact validation are recorded in the performance audit. Its monitoring update commit is `24c2e1957d9b4d0a11c07dda2e1d81ded14bcf1e`.


### Final dynamic FSR isolation probe ready for device test (2026-11-08)

GitHub Actions run #38 succeeded for source
`09d9684e45201933f0301175dc912a4a39606067`:
https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42479065982
Artifact ID: `13616202931`, name `panvk-vrs-render-probe-win64`.
ZIP SHA-256: `69d232372041beec38e9f7751e2cee53e23bc488cdfadedaf542bb77596911a3`.
Executable SHA-256: `06b777aaf67f2bee2af879e1e973c448b71ea88bcb2dc054f4c51d567540dc92`.
The archive was downloaded and independently inspected: valid x86-64 Windows
PE executable, expected dynamic isolation runner present, and both shader
variant invocation-comparison diagnostics embedded.

**User action now required:** download the artifact from the Actions run above,
extract it, open the extracted `tools` directory, and run
`run-vrs-dynamic-isolation.cmd`. Keep existing Android driver
`git-c9d61e10a9` installed; this is a Windows probe only and does not update
the Android driver. Send only `tools/panvk_vrs_dynamic_isolation.log`. If the
named log is absent but `panvk_vrs_render_probe.log` exists, send that instead.
Wait for the command window to finish before closing it. No reinstall, logcat,
or full archive is needed.


### Monitoring correction — documentation push queued wrapper CI

The handoff refresh commit `cf8a7dd9d2b58aaece1554872bbe06f6ee07717c`
was pushed without a `[skip ci]` suffix. The wrapper workflow is configured
with an unfiltered `on: [push]`, so this documentation-only push unintentionally
queued wrapper CI:

- [Run 42479110079](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42479110079), branch
  `docs/project-continuity-handoff-2026-11-03`, head
  `cf8a7dd9d2b58aaece1554872bbe06f6ee07717c`: status QUEUED at
  2026-11-08 12:12 UTC.
- Release job `128726936872`: QUEUED, no steps started/returned.
- Debugoptimized job `128726937136`: QUEUED, no steps started/returned.

This is not a build of the active performance-audit source branch and is not
part of its latest-run status. The current GitHub connector exposes no workflow
cancellation action, so this run could not be cancelled from this session.
No source or driver file was changed. **Future commits to the handoff branch
must include `[skip ci]` in the commit message** because of the wrapper
workflow's unfiltered push trigger. This correction itself is committed with
`[skip ci]`.


### Related handoff-branch wrapper runs observed during monitoring

The wrapper workflow's unfiltered `on: [push]` also means documentation-only
commits on this handoff branch can queue wrapper jobs. In addition to the
monitoring commit run `42479110079` recorded above, two separate concurrent
handoff documentation commits were observed:

- [Run 42479092549](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42479092549), commit
  `36074e16917ae87980735f4a4369cbf77e670c53`
  (`docs: record final dynamic FSR probe QA`): Release job
  `128726885198` and Debugoptimized job `128726885072`, both QUEUED.
- [Run 42479223286](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42479223286), commit
  `b165cd29e275855c3d9cdec0054ab6fcc8751b17`
  (`docs: mark dynamic FSR isolation probe ready for device test`): Release
  job `128727287782` and Debugoptimized job `128727288151`, both QUEUED.

These two commits were separate from this monitor's commit. All three runs
target the documentation branch, not `experiment/g720-performance-audit`.
No steps had started when checked. The connected GitHub toolset exposes no
workflow-cancel operation. Future handoff-branch documentation commits should
use `[skip ci]`; the monitoring correction commit does so and did not queue
a further wrapper run.


## 2026-11-08 — dynamic FSR isolation result received

User supplied the complete `--case=dynamic-isolation` log from the verified
Windows probe, using the already-installed Android PanVK package
`git-c9d61e10a9` on the Mali-G720 MC7.

### Device result

- Static 1x1 KEEP: submit/fence/readback PASS; checksum
  `0x06d78d985ad50383`; 4096 fragment invocations.
- Static 2x2 KEEP: submit/fence/readback PASS; checksum
  `0xcad734dde7920383`; 1024 fragment invocations; pixel signature PASS.
- Constant-colour static 2x2: submit/fence/readback PASS.
- Constant-colour dynamic 2x2: submit/fence/readback PASS.
- Constant-colour checksums are identical:
  `0x57ef86c490d1a383`; pixel comparison PASS.
- Constant-colour fragment invocations: static 1024, dynamic 1024;
  comparison PASS.
- Coordinate shader dynamic render: submit/fence/readback PASS.
- Coordinate shader fragment invocations: dynamic 1024, static 1024;
  comparison PASS.
- Coordinate-derived pixels still differ between static and dynamic 2x2.
- Final probe status: `VRS GPU RENDER PASS` (the isolation assertions pass).

### Interpretation

This proves dynamic 2x2 VRS reaches the GPU and executes at the expected
coarse rate. The constant-colour image is bit-identical to static 2x2, so
there is no general output or render-target mismatch. The defect is exposed
by `gl_FragCoord.xy`-dependent shader output. This strongly isolates the
remaining issue to fragment-coordinate/sample-centre semantics, while not
yet proving that sample-centre is the only possible coordinate-related
cause.

Source trace:
- Static FSR shader lowering replaces
  `nir_intrinsic_load_sample_pos_or_center` using the effective static rate.
- Dynamic FSR bypasses that compile-time lowering.
- Generic `pan_nir_lower_sample_pos_impl()` supplies (0.5, 0.5) when sample
  shading is disabled.
- CSF draw emission programs dynamic FSR from `dyns->fsr` into
  `IDVS.TILER_FLAGS2`; measured invocation counts confirm hardware execution.

Do not hardcode a 2x2 centre. Dynamic FSR is draw state and may change while a
pipeline remains bound. A real implementation must deliver the effective
rate/centre to the fragment shader at draw time or use a correct hardware
sysval, and preserve sample-shading/multisample semantics.

### Next engineering work

1. Add spatial pixel-difference diagnostics to the probe so the exact
   coordinate delta is observable.
2. Audit hardware sysval support and PanVK FAU/sysval plumbing for a
   per-draw dynamic rate value. Check all draw paths and state dirtiness
   before changing compiler/driver code.
3. Validate all advertised pipeline rates and sample-count interactions.
4. Keep primitive/attachment FSR false and do not claim full dynamic FSR
   correctness until coordinate semantics and relevant CTS tests pass.

No Android driver source, system/vendor driver, kernel, or firmware was
changed. No new device test is requested until a further diagnostic artifact
is built and verified.

Detailed audit:
https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/blob/experiment/g720-fragment-shading-rate/docs/G720-VRS-IMPLEMENTATION-AUDIT.md


### Follow-up source audit — dynamic FSR sysval design path

A source trace identified a plausible per-draw fragment FAU/sysval route:
`panvk_graphics_sysvals` has an FS sysval block; the architecture-specific
`cmd_prepare_draw_sysvals` function populates per-draw values and marks
`FS_PUSH_UNIFORMS` dirty when the shader uses changed sysvals; CSF
`prepare_push_uniforms` uploads the FS FAU block and updates
`IDVS.FRAGMENT_FAU`. The draw emitter already reads dynamic FSR state from
`dyns->fsr` and programs `IDVS.TILER_FLAGS2`.

Candidate implementation is to carry the effective dynamic rate in a
fragment sysval and lower the dynamic sample-centre input to that sysval.
This is not implemented yet. Remaining audit requirements include sysval
usage/remapping, exact dynamic-state dirty propagation, all draw variants,
and consistency with the CSF rule that sample shading forces effective VRS
to 1x1. Do not just copy the requested rate if sample shading or combiner
semantics alter the effective rate.


## 2026-11-08 13:25 UTC — dynamic FSR sysval implementation started on isolated branch

The dynamic-isolation device result established that dynamic 2x2 KEEP reaches the GPU and executes 1024 fragment invocations, matching static 2x2, and that constant-colour output is bit-identical. The remaining observed mismatch is limited to coordinate-dependent output. This strongly points to fragment-centre/sample-position semantics; it does not prove that all other coordinate-related semantics are correct.

A separate source branch, experiment/g720-dynamic-fsr-sysval, was created from experiment/g720-fragment-shading-rate. The known-good/tested branch and the installed Android package remain unchanged.

### Source changes committed (not yet compiled)

- a289ec077bedcaee186449cc6914ae01edf3ccd5 — add aligned graphics fragment sysval fs.frag_rate.
- 4561b7f73a64d7f5d5eb669beb0d49609db36ff2 — populate the effective dynamic rate in cmd_prepare_draw_sysvals() at the shared per-draw update point. The value uses the same log2(width)/log2(height) bit encoding as the static centre lowering and CSF TILER_FLAGS2. Sample-shading pipelines publish 1x1. Only KEEP/KEEP is lowered, matching the currently exposed pipeline-only feature scope.
- 938ef7b8c057b418ebac9797232b264f3792aa5b — replace dynamic load_sample_pos_or_center with a fragment FAU/sysval load, and separate static/dynamic FSR shader cache keys. Static effective rates are now included in the cache key because they alter compile-time NIR constants.
- e4a28fb14673843e81f648ca4f8bfecf160bac35 — record the implementation and validation plan in docs/G720-VRS-IMPLEMENTATION-AUDIT.md.

The shared cmd_prepare_draw_sysvals() function is called by ordinary draws, tessellation, GS, and meta draw paths in the CSF implementation. The sysval is therefore updated at the common preparation point, before fragment FAU upload. This source-level call-site audit is not a substitute for build or runtime validation.

### Current status and next gates

Status: experimental source patch; build pending; device validation pending.

The source branch has not yet passed compilation. The audit-document push should queue the repository's push-triggered wrapper CI, but that workflow is not the Android-only PanVK build. The Android-only workflow is manual-dispatch in the repository; no dispatch action is exposed in the connected GitHub tools in this session. Do not claim an Android build or package exists until a successful Android-only run is independently inspected.

Next steps:
1. Review the complete source diff for ABI, FAU packing, dynamic dirty-state, and shader cache correctness.
2. Obtain a successful Android/AArch64 build on this exact branch; verify compiler logs and artifact provenance.
3. Audit the resulting package (ELF class/ABI/API level, dependencies, symbols, ZIP integrity, and hashes) before any device import.
4. Re-run the dynamic-isolation coordinate test, then add a same-command-buffer 1x1-to-2x2 rate transition and sample-shading 1x1 test.
5. Only after the above passes, compare performance and stability in Winlator. Keep the existing known-good PanVK package available for rollback.

No system/vendor driver, kernel, firmware, or installed Android package was modified. Primitive and attachment FSR remain disabled/unexposed. No new device-side action is requested yet.


## 2026-11-08 — Android-only build allowlist corrected for dynamic FSR branch

Correction to the preceding status note: .github/workflows/android-only-experiment.yml is not workflow_dispatch-only. It supports workflow_dispatch and push, but push is restricted to an explicit branch allowlist. The newly created experiment/g720-dynamic-fsr-sysval branch was initially absent from that allowlist, so its earlier source and audit-document commits did not trigger the Android-only build.

The allowlist now includes experiment/g720-dynamic-fsr-sysval. Workflow change commit: 927a24b620a496a61c2aab510995aae995dcbdc9. This push is intended to launch the Android-only PanVK build against the experimental branch head, including the sysval changes. The connected GitHub status endpoint currently returns no check records for that commit, so the run has not yet been independently confirmed as queued or completed. Do not claim build success until the Actions run and artifact are inspected.

The source implementation remains experimental and unvalidated. No user device action is requested while the Android build is pending. The known-good experiment/g720-fragment-shading-rate branch and installed Android package remain unchanged.


## 2026-11-08 — second-pass dynamic FSR source/FAU audit and CI guardrails

Continued work on experiment/g720-dynamic-fsr-sysval. The NIR sysval path was traced through the actual PanVK push-constant lowering implementation:

- load_sysval(b, graphics, 32, fs.frag_rate) creates a load_push_constant with base SYSVALS_PUSH_CONST_BASE and a constant byte offset from sysval_offset(graphics, fs.frag_rate).
- collect_push_constant() recognizes that base as a driver sysval and marks the corresponding byte range with shader_use_sysval_range().
- move_push_constant() remaps the sysval byte offset through shader_remapped_sysval_offset() before backend compilation.
- The per-draw set_gfx_sysval() writes the same struct field into graphics sysvals, and the existing upload/packing machinery handles the field like other graphics sysvals.

This confirms that the new field uses the established FAU/sysval mechanism rather than an untracked raw push-constant load. The field is placed after the existing blend descriptor array, preserving the required blend-constant prefix and 8-byte struct alignment (the new uint32_t is paired with a uint32_t pad).

The Android-only workflow was strengthened in commit 0804f245e8bf0e4fc7e6c70e6867a375a7bd73aa. Its pre-build source verification now explicitly checks for:
- the fs.frag_rate declaration;
- per-draw set_gfx_sysval() population;
- lower_dynamic_vrs_frag_center();
- the load_sysval() read of fs.frag_rate;
- static/dynamic shader-cache key handling.

The workflow allowlist already includes experiment/g720-dynamic-fsr-sysval (commit 927a24b620a496a61c2aab510995aae995dcbdc9). GitHub combined-status queries for both workflow commits still return no check records, and the connected GitHub Actions tools exposed in this session do not include a list/dispatch workflow-run action. Therefore no Android build, artifact, or device test is claimed. The branch must still pass compiler/build validation.

Current branch head after the CI guardrail change: 0804f245e8bf0e4fc7e6c70e6867a375a7bd73aa. The experimental source changes remain uncompiled. No device package or vendor/system driver was changed.

Next: obtain and inspect the Android-only workflow run for this exact branch head; if it fails, fix the first actual compiler/build error rather than making speculative source changes. If it succeeds, inspect its artifact and package before asking for device-side testing. Device tests remain static 2x2 vs dynamic 2x2 coordinate output, same-command-buffer 1x1-to-2x2 transition, and sample-shading 1x1 effective-rate behavior.


## 2026-11-08 — Dynamic FSR Android build and package validation

### Source revision
- Experimental source branch: `experiment/g720-dynamic-fsr-sysval`
- Source commit: `0804f245e8bf0e4fc7e6c70e6867a375a7bd73aa`
- This branch contains the dynamic fragment-rate graphics sysval, dynamic fragment-centre lowering, cache-key separation, source guards, and Android-only workflow allowlist update.

### CI results
- Android NDK wrapper-variant workflow run `42480864476`: completed successfully.
  - Debugoptimized and release Android AArch64 Panfrost/PanVK shared-library jobs succeeded.
  - Artifacts: `libvulkan_wrapper-aarch64-debugoptimized` and `libvulkan_wrapper-aarch64-release`.
- Android-only PanVK G720 workflow run `42480864484`: completed successfully.
  - Built real Android ARM64 libdrm.
  - Configured and compiled the Android-only Kbase build.
  - Staged install and driver/manifest/dependency audit succeeded.
  - Flat Winlator/Adrenotools package build and artifact upload succeeded.
- Workflow run URL: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42480864484

### Package artifact
- Artifact: `panvk-g720-android-only-experimental-package`
- GitHub artifact ID: `13617542479`
- Artifact SHA-256: `422db8213301238ead0db0ea4a4102331b1da1880c2cac9ba5a10c644d0d5787`
- Inner installable package: `PanVK-G720-experimental.zip`
- Inner package SHA-256: `91c4de31a7c740749c016cfb897337d325c08e1c442a5a17ad63eb387c3177bc`
- Driver ELF SHA-256 from build audit: `3aeab38c79eb1756f7c7853ffeb0e24044a0b76aebcd04bb32a0af90e51499c6`
- Package contents: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`.
- Metadata identifies Mesa `26.3.0-devel`, package version `26.3.0-devel-android-kbase-0804f245e8bf`, min API 30, and explicitly marks device/game compatibility unverified and non-conformant.
- Verified locally in the audit step: package ZIP integrity passes; both driver libraries are ELF64 little-endian AArch64 Android shared objects; driver RUNPATH is `$ORIGIN/../../android_stub`; required bundled `libdrm.so` is staged; audit reports zero missing DRM symbols.

### Important qualification
- CI build, ELF/dependency audit, and ZIP integrity are confirmed. Runtime loading and Vulkan execution on the user's Dimensity 8400 Ultra / Mali-G720 MC7 are NOT yet verified.
- This is an experimental custom Winlator/Adrenotools import package. Do not replace or modify the Android system/vendor Vulkan driver.
- The wrapper-variant build artifact is a separate CI output; the Android-only package above is the intended flat custom-driver package for device testing.

### Next device validation
1. Download and import the inner `PanVK-G720-experimental.zip` into Winlator Mali 1.2's custom driver path only.
2. First run a Vulkan capability/device probe and the existing VRS render probe; capture full logs.
3. Verify that the reported driver revision matches `0804f245` and that the ICD loads without missing-library or symbol errors.
4. Run static 2x2 KEEP coordinate test, dynamic 2x2 KEEP coordinate test, then dynamic 1x1-to-2x2 transitions in one command buffer/pipeline.
5. Confirm sample shading forces effective 1x1.
6. Only after correctness and stability pass, benchmark Borderlands GOTY at the established 1280x720 settings against the known-good stock-driver baseline. Keep the known-good VRS branch and stock driver untouched.


### Post-package ELF verification
- Inspected the inner ZIP independently after extraction.
- Packaged driver now has RUNPATH `$ORIGIN` and a `DT_NEEDED` entry for `libpanvk_drm.so`; companion library SONAME is `libpanvk_drm.so`.
- Confirmed the packaged driver's 24 undefined `drm*` dynamic references are all provided by the packaged companion library (212 exported dynamic symbols); no unresolved DRM names were found.
- The package-stage driver hash differs from the pre-package staged-driver hash because the packaging step adjusts the runtime dependency name and RUNPATH. The authoritative install-package hashes are in the inner ZIP's `SHA256SUMS`:
  - `libvulkan_panfrost.so`: `01588024ab8549c92f3b5981bdd2239af0b9ec9796c50f91f13c012bca14453b`
  - `libpanvk_drm.so`: `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
- Local ZIP integrity check: all four package entries passed `unzip -t`.


## 2026-11-08 — First device-side render probe on dynamic-FSR build

### User-provided device log
- Driver/device: Wrapper (Mali-G720 MC7), PanVK Mesa 26.3.0-devel, `git-0804f245e8`, Vulkan API 1.3.354.
- FSR feature report: pipeline=1, primitive=0, attachment=0; non-trivial combiner ops=0; strict multiply=0.
- Device creation with `VK_KHR_fragment_shading_rate` and pipeline FSR succeeded.
- Invocation-counter descriptor set, SSBO, image, staging buffer, render pass, shader modules, pipeline, command buffer, queue submit, fence wait and readbacks all succeeded.
- Static 1x1 KEEP: 4096 fragment invocations; checksum `0x06d78d985ad50383`; equal-neighbors=0.
- Static 2x2 KEEP: 1024 fragment invocations; checksum `0xcad734dde7920383`; equal-neighbors=4096.
- Pixel signature passed. Invocation reduction passed (4:1, 75% fewer fragment invocations). Final status: `VRS GPU RENDER PASS`.

### Correct interpretation / probe CLI correction
- The log says `selected render case: --case=keep`. Source inspection of `tools/panvk_vrs_render_probe_win.c` confirms this case runs the 1x1 and static 2x2 pipeline FSR baselines only; it does NOT enter the dynamic FSR comparison branch.
- The probe supports explicit `--case=dynamic-keep`, which runs the same 1x1 and static 2x2 baselines, then renders dynamic 2x2 KEEP and requires its image checksum and fragment invocation count to match static 2x2.
- It also supports `--case=dynamic-isolation`, which compares static and dynamic 2x2 constant-colour output/invocations, then verifies dynamic coordinate-dependent rendering reaches the expected invocation count while remaining distinct from the static coordinate output. This is the stronger isolation test for the sysval/fragment-centre change.
- Therefore this device run proves static pipeline FSR is genuinely executing on the G720 and the package loads. It does NOT yet validate the new dynamic sysval lowering.
- Next exact test: run the same executable with argument `--case=dynamic-keep`; after that run `--case=dynamic-isolation`. Preserve both complete logs. Do not benchmark games until dynamic correctness is established.


## 2026-11-08 12:42 UTC — performance-audit follow-up and attachment verification

### Current branch and source state

- Re-read the complete canonical handoff and the performance audit before inspecting the repository, active branch, source files, CI, and current conversation attachments.
- Performance-audit branch pre-update HEAD: `6d1ed6db4c0ef050a516ce47de2c1991a30b2e90`. Latest driver-source commit: `a2e5ac142b4f0df71cb28204ece4337bace046de`. Compare reports 52 commits ahead / 0 behind from source to branch HEAD, with only documentation paths changed. No driver source or workflow file changed.
- Descriptor failure classification remains in source: `788d34b622ce6cf51aa410c6366a06b86a6a4988` adds overflow/capacity/VMA counters; `751fddd64efc201c103ffa496b29608d315a55e6` corrects formatting; `a2e5ac142b4f0df71cb28204ece4337bace046de` fixes the profile log call. Original allocator behavior and Vulkan results are preserved.
- Effective heap threshold logging remains from `52afaaf880fdf3a8cb4e6e0c5cd6598ec482d3ee`. Default is 65,536. The log only reports the effective value, so environment propagation must be demonstrated with the 65K-default package.
- Performance audit documentation was updated at commit `de47624438c6951437855dfd66c65b526fdf0b51`.

### CI state and branch separation

- Performance-audit Android-only run [42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218) and wrapper run [42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167) both remain SUCCESS for source snapshot `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. The branch's later commits are documentation-only.
- The handoff branch's prior commits without `[skip ci]` triggered wrapper-only runs [42481490094](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42481490094) and [42481461421](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42481461421). At the time of inspection, both Release and Debugoptimized compile steps were still in progress. These are not performance-audit or Android-only PanVK builds. The earlier wrapper run `42479110079` completed SUCCESS. Use `[skip ci]` on all future handoff documentation commits.
- Parallel VRS work remains separate: `experiment/g720-dynamic-fsr-sysval` at `0804f245e8bf0e4fc7e6c70e6867a375a7bd73aa` passed Android-only run `42480864484` and wrapper run `42480864476`; Android artifact `13617542479` was recorded with outer digest `422db8213301238ead0db0ea4a4102331b1da1880c2cac9ba5a10c644d0d5787` and inner package hash `91c4de31a7c740749c016cfb897337d325c08e1c442a5a17ad63eb387c3177bc`. This is a separate dynamic-FSR experiment and must not be substituted for the heap-threshold package.

### Current attachment verification and benchmark evidence

- The newly surfaced `PanVK-G720-65k-env-override-test.zip` is the same package already recorded: SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`. ZIP integrity and all embedded checksums pass. It contains the four expected flat files. The driver is ELF64 AArch64/API 30 with RUNPATH `$ORIGIN` and private NEEDED `libpanvk_drm.so`; the companion library is ELF64 AArch64 with SONAME `libpanvk_drm.so` and only `libc.so` as a dependency. This confirms static package integrity, not runtime loading or game compatibility.
- No new Tomb Raider benchmark log was attached. The available archive is still `TombRaider_logs_2026-11-05_02-03-50.zip`, SHA-256 `6f54b17e422795673411dfc3106a184922ea283024801f1e257ed5dbd9f68a56`, timestamped 2026-11-05 02:03.
- That instrumented archive records 4,068 heap renewals, 52,804.121 ms aggregate renewal wait, 0 ring wraps, 1.8 KiB sampled occupancy high-water, 0 transient BO allocation failures, 0 backing BO creation failures, and 299 aggregate descriptor-set allocation failures. It has neither the new descriptor failure-category output nor the effective-threshold log, so it cannot classify those 299 failures or validate the environment override.
- The `7.7 / 19.7 / 13.6 FPS` result remains an instrumented diagnostic capture. Clean 65K baseline remains `9.0 / 17.6 / 13.5 FPS`; the 262K compiled-default candidate remains `7.8 / 18.9 / 13.9 FPS`. The result is inconclusive; no FPS improvement is claimed.

### Next device test

Use the verified 65K-default package as a separate Winlator Mali custom driver:

1. Run the same Tomb Raider scene/settings with `PANVK_KBASE_HEAP_RENEW_WORK` absent; restart the game/driver process and require `tiler heap renewal work threshold=65536`.
2. Repeat with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144`; restart and require `tiler heap renewal work threshold=262144`. The package's compiled default is 65,536, so this demonstrates environment propagation.
3. If the second run reports 65,536 or has no threshold log, stop and classify the override test as invalid.
4. Leave all profiling variables unset in both performance runs: `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE`.
5. Keep scene, resolution, quality, DXVK 1.7.2, wrapper, and thermal starting state matched; capture min/max/average FPS, frame pacing, visual correctness, stability, and teardown. Prefer repeated A/B samples. Run descriptor telemetry separately if failure categories are still needed.

No source, allocator policy, queue sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed. Do not make further heap-policy changes until this controlled device test is complete.


## 2026-11-08 — parallel VRS device result captured during performance-audit refresh

The separate dynamic-FSR sysval branch has one new device result recorded in the preceding handoff history:

- Device/package: Wrapper (Mali-G720 MC7), PanVK Mesa 26.3.0-devel, revision `0804f245e8`, Vulkan API 1.3.354.
- Pipeline fragment shading rate is reported supported; primitive and attachment FSR remain disabled.
- Static 1x1 KEEP rendered 4,096 fragment invocations; static 2x2 KEEP rendered 1,024. Both image checksums/signatures passed, and the probe reported `VRS GPU RENDER PASS`.
- This is a genuine static pipeline-FSR render result, not just an extension/feature query. It does not validate dynamic fragment-rate sysval lowering: the supplied run used `--case=keep`, not a dynamic case.
- Next VRS-only device tests are `--case=dynamic-keep`, then `--case=dynamic-isolation`. Preserve the full logs. Do not treat these results as Tomb Raider or heap-renewal performance measurements.
- This VRS work remains on its isolated branch and is not merged into `experiment/g720-performance-audit`.

The performance-audit branch remains unchanged at driver-source commit `a2e5ac142b4f0df71cb28204ece4337bace046de`; the audit document refresh is `de47624438c6951437855dfd66c65b526fdf0b51`. The heap-threshold A/B device test remains the next action for that branch.


## 2026-11-08 — handoff-branch wrapper CI status recheck

- Wrapper run `42481490094` for handoff-branch commit `2ead3df163443f8234ea0fead043de2f8c21fddb` has now completed SUCCESS.
- Wrapper run `42481461421` for handoff-branch commit `b4d4963f182ab198d10ad7184f8460e16da59855` has now completed SUCCESS.
- A later documentation-only commit `cbf03e1225419db38f9d781c2a8c31b410bfa8f5` records the static VRS render result and triggered wrapper run `42481867670`. At the latest status recheck, that run was still in progress in the Release and Debugoptimized compile steps. It is a wrapper-only build on the handoff branch; it is not an Android-only PanVK build and does not change the performance-audit source status.
- The current handoff branch head before this status note was `d5fa3bb1aa2f2b0388f4980069ec8a4e3207344b`. This documentation update uses `[skip ci]` to avoid another push-triggered wrapper run.


## 2026-11-08 14:10 CET — VRS render-probe output clarification

The Windows GPU render-probe source on `experiment/g720-dynamic-fsr-sysval` (HEAD `0804f245e8bf0e4fc7e6c70e6867a375a7bd73aa`) intentionally does not print its diagnostic stream to stdout. Its `LOG` macro writes only to `panvk_vrs_render_probe.log`, opened in the process working directory by `main()`. Therefore an immediate return to the CMD prompt with no console text is expected and is not evidence that the test did nothing or failed.

The exact Windows probe artifact is `panvk-vrs-probe-win64`, artifact ID `13617397046`, from wrapper workflow run `42480864476` (artifact SHA-256 `7379f254796c736d4cf0a1336b0cf2a9037494162e56065b126b48018e7e39ce`). This is the render-probe build from the dynamic-FSR source snapshot, not merely the earlier capability-only probe.

Device test procedure: run one case from `D:\\panvk-vrs-render-probe-win64>`, then inspect `type panvk_vrs_render_probe.log` in that same directory. The exact cases remain `--case=dynamic-keep` and `--case=dynamic-isolation`. Run each case separately because each invocation overwrites the same log file; copy/rename the first log before running the second if both are to be retained. Do not infer a PASS from a silent CMD prompt; only the log's final result and detailed comparisons count.

No driver source or package changed in this clarification.


## 2026-11-08 — dynamic VRS probe argument-independent rebuild

The user's repeated attempts to run `--case=dynamic-keep` continued to produce a log identifying `selected render case: --case=keep`. This is a static KEEP result only; dynamic FSR remains untested on device. To remove dependence on Winlator command-line argument forwarding, the dedicated Windows probe's no-argument default has been changed from `--case=keep` to `--case=dynamic-keep`.

Repository changes on `experiment/g720-dynamic-fsr-sysval`:
- Source change commit: `8e48bb6e0d6147b726ae8022f83dd41492fe7afb` — `tools/panvk_vrs_render_probe_win.c` now defaults to dynamic KEEP when no argument is supplied. Explicit supported case arguments remain available.
- CI workflow change commit: `314daa08871f44029d8759402964fdc2fdd2753a` — include the dynamic-FSR branch in the Windows probe workflow push trigger and include the root runner script in the uploaded artifact.
- One-click runner commit: `1cddda82b7c58549055a822561bd8dbe1b811a2b` — adds root-level `Run-Dynamic-VRS.cmd`; it deletes any old log, runs the executable with no arguments, prints the new log, and pauses.
- Windows probe workflow run `42485499280` was triggered from workflow commit `314daa08871f44029d8759402964fdc2fdd2753a`; at handoff update time it was still in progress. Do not distribute the new executable until this run succeeds and its artifact is available.

Next step after successful CI: user downloads the newly built `panvk-vrs-render-probe-win64` artifact from run `42485499280`, extracts it, and double-clicks `Run-Dynamic-VRS.cmd`. The expected first log marker is `selected render case: --case=dynamic-keep`; dynamic success additionally requires static-vs-dynamic checksum and invocation comparisons to PASS and final `VRS GPU RENDER PASS`. A static-only log remains insufficient evidence. The known-good static 1x1/2x2 result (4096 vs 1024 fragment invocations, 75% reduction) remains valid and unchanged.

No PanVK driver implementation or Android package was modified by this probe harness fix. The VRS experiment remains isolated from the performance-audit branch; no system/vendor driver replacement is involved.


### Dynamic VRS probe rebuild CI result

- Windows probe workflow run `42485499280` completed SUCCESS; all steps passed, including shader compilation, MinGW executable build, executable verification, and artifact upload.
- New artifact: `panvk-vrs-render-probe-win64`, artifact ID `13619392010`, size 117,149 bytes, created 2026-11-08 13:33:43Z; not expired at verification time.
- Artifact download page: `https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42485499280`.
- Artifact includes the rebuilt `panvk-vrs-render-probe.exe` and root `Run-Dynamic-VRS.cmd` plus existing case scripts, as declared in the successful workflow upload step. New EXE defaults to `--case=dynamic-keep` when launched with no arguments.
- CI success proves the Windows probe compiled and packaged; it does not prove dynamic FSR runtime correctness on the Mali-G720. Device run/log still required.



## 2026-11-08 — Mali-G720 dynamic pipeline FSR runtime PASS

The user ran the rebuilt argument-independent Windows render probe on the actual Wrapper(Mali-G720 MC7) device. The supplied log selects `--case=dynamic-keep` without arguments and completes both static controls and the dynamic-state test successfully.

Device/runtime identity:
- Wrapper(Mali-G720 MC7), PanVK Mesa 26.3.0-devel, driver revision `0804f245e8`, Vulkan API 1.3.354.
- `pipelineFragmentShadingRate=1`; primitive and attachment FSR remain unsupported/disabled (`0`). `nonTrivialCombinerOps=0`, `strictMultiplyCombiner=0`.
- Probe device creation, descriptor setup, render resources, graphics pipelines, queue submissions, fences, memory maps, and readbacks all returned success.

Measured and verified results:
- Static 1x1 KEEP: checksum `0x06d78d985ad50383`, equal-neighbors 0, fragment invocations 4096; submit/fence/readback PASS.
- Static 2x2 KEEP: checksum `0xcad734dde7920383`, equal-neighbors 4096, pixel signature PASS; fragment invocations 1024; invocation reduction PASS (4x fewer invocations / 75% reduction).
- Dynamic-state 2x2 KEEP: `vkCmdSetFragmentShadingRateKHR` recorded with rate 2x2 and combiner ops 0,0; submit/fence/readback PASS.
- Dynamic 2x2 checksum exactly matches static 2x2 (`0xcad734dde7920383`), equal-neighbors 4096, pixel signature PASS.
- Dynamic fragment invocations 1024, matching static 2x2 count 1024; comparison PASS.
- Final result: `VRS GPU RENDER PASS`.

This is the first actual-device runtime validation of the dynamic pipeline-rate command plus PanVK dynamic fragment-rate sysval path for this test. It validates the tested 2x2 KEEP scenario only. It does not validate other combiner modes, primitive/attachment shading rates, all render-pass configurations, application workloads, or performance gains in DXVK games. The hardware/driver-reported capability limits above remain unchanged; do not expose unsupported FSR modes.

Next engineering action: move from isolated probe correctness to integration/performance validation. Add a controlled application-level A/B test that compares baseline 1x1 against dynamic 2x2 KEEP, first with a simple deterministic full-screen workload and GPU timestamps, then a real DXVK workload. Check image correctness and frame time, thermal/frequency stability, and CPU submission overhead. Keep the feature gated/opt-in until end-to-end results demonstrate a benefit. Do not merge into the performance-audit branch or alter the Android package/system driver without a separate review and explicit test evidence.


## 2026-11-08 15:16 CET — GPU timestamp capability probe added

The next VRS engineering step is to measure whether the Mali-G720 PanVK device supports GPU timestamp queries before adding timed render cases. Source audit found that PanVK's `timestampComputeAndGraphics` property is gated by `device->kmod.dev->props.gpu_can_query_timestamp`, and `timestampPeriod` is populated from the GPU system timestamp period on v10+. The Windows VRS render probe previously did not report these properties or the selected graphics queue family's timestamp-valid-bit count.

- On `experiment/g720-dynamic-fsr-sysval`, commit `2c8f5d2bfefa90cb5942f87b27903bd70900cb7a` updates `tools/panvk_vrs_render_probe_win.c` to print `timestampComputeAndGraphics`, `timestampPeriod` in nanoseconds, and the selected graphics queue family's `timestampValidBits`.
- The probe explicitly reports whether GPU timestamp queries are unavailable; if so, any fallback fence timing must be labeled CPU+GPU elapsed time, not GPU-only execution time.
- Build workflow `Build Winlator VRS render probe` run `42489106136` is in progress for this source commit. Android-only run `42489106018` and wrapper run `42489106032` are also in progress. No device artifact is ready yet.
- This is probe-only instrumentation. It does not change PanVK runtime behavior, advertised Vulkan features, shader lowering, the Android package, or any system/vendor driver.

After CI succeeds, run the rebuilt probe on the actual G720 and capture the full log. If `timestampComputeAndGraphics=1`, `timestampPeriod>0`, and the graphics queue's `timestampValidBits>0`, implement a GPU timestamp A/B workload. If any condition fails, do not claim GPU-only timings; use clearly labeled fence-based elapsed-time measurements or investigate the Kbase timestamp capability path separately.


### Timestamp-capability probe build result

- Windows VRS probe workflow run `42489106136` completed SUCCESS for source commit `2c8f5d2bfefa90cb5942f87b27903bd70900cb7a`; shader compilation, MinGW build, executable verification, and artifact upload all passed.
- Artifact `panvk-vrs-render-probe-win64`: ID `13621270555`, 118,053 bytes, SHA-256 `1d0920d661bf3481946563889b8d06941c8f49e6cf55f7a666d406fb345c2333`, created 2026-11-08 14:18:17 UTC, expires 2027-02-06.
- Android-only PanVK run `42489106018` and Android wrapper run `42489106032` were still in progress at this update. The Windows probe artifact can be tested independently; this change is only probe source and does not require installing a new driver package.
- Device-side timestamp availability remains unknown until the user runs the new artifact and shares `panvk_vrs_render_probe.log`.

## 2026-11-08 — Mali-G720 timestamp capability inconsistency identified

The user ran the rebuilt timestamp-capability probe on Wrapper(Mali-G720 MC7), PanVK Mesa 26.3.0-devel (0804f245e8). The dynamic VRS render result remains a full PASS (same static/dynamic 2x2 checksum 0xcad734dde7920383, 1,024 fragment invocations versus 4,096 at 1x1, and successful submit/fence/readback).

Timestamp fields reported:
- timestampComputeAndGraphics=1
- timestampPeriod=0 ns
- graphics queue timestampValidBits=64
- Probe correctly reports GPU timestamp queries unavailable because the period is zero.

Source audit on experiment/g720-dynamic-fsr-sysval found the inconsistency:
- src/panfrost/lib/kmod/kbase_kmod.c sets props->gpu_can_query_timestamp = (props->gpu_id != 0).
- The same initialization explicitly sets props->timestamp_frequency = 0, with a comment explaining the GPU/system timestamp frequency is not populated from the GPU property blob.
- src/panfrost/vulkan/panvk_physical_device.c implements panvk_get_gpu_system_timestamp_period(): it returns zero when gpu_can_query_timestamp is false OR timestamp_frequency is zero; otherwise it returns timestamp_cycles_to_ns_factor.
- Queue-family timestamp valid bits are set to 64 on arch >= 10 whenever gpu_can_query_timestamp is true. Thus the current Kbase property combination produces the observed mismatch: timestamp feature flag and valid bits are advertised while timestampPeriod is zero.

The Kbase UAPI header src/panfrost/lib/kmod/kbase_uapi.h defines KBASE_IOCTL_GET_CPU_GPU_TIMEINFO output fields including monotonic seconds/nanoseconds, system-wide timestamp, and GPU cycle counter. mali_base_kernel.h defines request flags for monotonic time, system-wide timestamp, and GPU cycle counter. The backend's existing kbase_kmod_query_timestamp() requests only BASE_TIMEINFO_TIMESTAMP_FLAG and returns out.timestamp.

No runtime driver change has been made yet. Do NOT simply force a guessed timestamp frequency or claim that the GPU timestamp counter is broken. Next engineering task is to determine precisely which counter Vulkan timestamp writes use on this kbase CSF path, and whether a safe frequency can be calibrated from paired monotonic + system timestamp samples via KBASE_IOCTL_GET_CPU_GPU_TIMEINFO. Check units, source flags, wraparound, sampling stability, and whether timestamp_cycles_to_ns_factor is expected to be reciprocal frequency before implementing. If calibration cannot be validated, correct the capability reporting so gpu_can_query_timestamp, queue timestampValidBits, and timestampPeriod are mutually consistent (disable Vulkan GPU timestamp support rather than advertise zero period). Keep this isolated to the experimental branch; no system/vendor driver replacement.

Probe source and CI artifact are unchanged by this finding. The user does not need to change Winlator settings or rerun the same rendering test until a new probe is built.

Additional source comparison after the initial finding:
- The Panfrost DRM backend obtains DRM_PANFROST_PARAM_SYSTEM_TIMESTAMP_FREQUENCY and computes timestamp_cycles_to_ns_factor = 1,000,000,000 / timestamp_frequency.
- The Panthor backend imports its kernel-reported timestamp_frequency and computes the same reciprocal-Hz conversion.
- Therefore timestampPeriod is in nanoseconds per timestamp tick, and the zero value is specifically caused by the Kbase backend leaving timestamp_frequency at zero. Do not substitute the GPU core frequency unless the kernel timestamp counter is proven to be that same counter.
- The Kbase timeinfo UAPI distinguishes system-wide timestamp and GPU cycle_counter fields; this distinction must be respected during any calibration experiment.

## 2026-11-08 — Conservative Kbase timestamp capability fix committed (CI pending)

Further source audit established that PanVK CSF Vulkan timestamp queries on arch 10+ are not populated by kbase_kmod_query_timestamp(). The Vulkan query path in src/panfrost/vulkan/csf/panvk_vX_cmd_query.c emits MALI_CS_STATE_TIMESTAMP through cs_store_state(); the Kbase ioctl query_timestamp operation separately returns req.out.timestamp from BASE_TIMEINFO_TIMESTAMP_FLAG. These are distinct paths/counters and must not be assumed to share a frequency.

PanVK currently derives timestampComputeAndGraphics from gpu_can_query_timestamp, derives graphics queue timestampValidBits from the same boolean, and derives timestampPeriod from timestamp_frequency. Since Kbase reports gpu_can_query_timestamp=true solely from nonzero gpu_id but leaves timestamp_frequency=0, the device advertised an unusable combination (feature=true, validBits=64, period=0). KHR_calibrated_timestamps was also exposed from that same boolean.

Experimental source fix committed on experiment/g720-dynamic-fsr-sysval:
- Commit aa22fd664c203a961e35fdff88b788f19b2a39e0 updates src/panfrost/lib/kmod/kbase_kmod.c to set props->gpu_can_query_timestamp=false until the hardware timestamp counter frequency and its relationship to Kbase timeinfo counters are validated. timestamp_frequency remains zero. The comment explicitly distinguishes MALI_CS_STATE_TIMESTAMP from the Kbase ioctl system timestamp path.
- This makes PanVK conservatively report timestampComputeAndGraphics=false, graphics timestampValidBits=0, timestampPeriod=0, and disables KHR_calibrated_timestamps on this backend. It does not remove the internal kbase_kmod_query_timestamp() ioctl path.
- CI workflow commit 78456d1ce6553826a2edc7c1e158b73ea5a4f837 adds a source assertion for the conservative capability setting to android-only-experiment.yml.
- This is a correctness/advertisement fix only; no claim of GPU timestamp hardware failure. No timestamp frequency is guessed, and no changes to Android system/vendor drivers are made.

Next: verify Android-only CI completes successfully. Then produce a small Vulkan properties probe on the phone to confirm timestampComputeAndGraphics=0, timestampPeriod=0, graphics timestampValidBits=0, and KHR_calibrated_timestamps is not exposed. Only after this is validated should work resume on a separate calibration experiment; use paired Kbase timeinfo monotonic/timestamp/cycle_counter requests and prove which counter corresponds to MALI_CS_STATE_TIMESTAMP before computing a frequency. The existing dynamic FSR GPU render PASS remains valid and unaffected.

## 2026-11-08 — Hard gate Kbase Vulkan timestamp exposure at the PanVK API boundary

The user reran the VRS render probe and confirmed the same mismatch after the conservative Kbase backend change: `timestampComputeAndGraphics=1`, `timestampPeriod=0 ns`, and graphics `timestampValidBits=64`. The VRS static/dynamic render tests still pass. The user explicitly asked to stop repeating package-verification steps and fix the source directly.

The prior Kbase initialization fix (`props->gpu_can_query_timestamp = false`) is necessary but did not change the runtime-reported Vulkan properties in the user's tested build. The Vulkan-facing capability logic was therefore hardened independently of that backend flag:

- Commit `bb69bf30025796f29c03db4a6db1a1f37993e16d` on `experiment/g720-dynamic-fsr-sysval`: in `src/panfrost/vulkan/panvk_vX_physical_device.c`, all timestamp-dependent extension/feature exposure now additionally requires a nonzero `timestamp_frequency` and that the device is not using the Kbase backend (`!device->kbase_node_path[0]`). This covers KHR/EXT calibrated timestamps, shader clock extension/feature, optional present timing, and `timestampComputeAndGraphics`.
- Commit `50cc2102a9ff737d775b47bfc57d5051aeaab965`: in `src/panfrost/vulkan/panvk_physical_device.c`, queue-family `timestampValidBits` now also requires a nonzero frequency and a non-Kbase backend. The timestamp period function already returns zero when frequency is zero.
- Commit `cabe200fd0c72e1267269b308171d0747a43f24a`: Android-only CI workflow now asserts these independent API-boundary gates are present, in addition to the Kbase backend's false capability flag.

Expected Vulkan result for the Kbase G720 build after this source revision: `timestampComputeAndGraphics=0`, `timestampPeriod=0 ns`, graphics `timestampValidBits=0`, and calibrated-timestamp/shader-clock extensions not exposed. This is an API capability consistency fix, not a claim that hardware timestamps are unusable in all possible Kbase implementations. No timestamp frequency is guessed. VRS code and its verified static/dynamic KEEP behavior are unchanged.

Next action: wait for the Android-only workflow on commit `cabe200fd0c72e1267269b308171d0747a43f24a` to compile. Then install that experimental PanVK package in Winlator (never replace the Android system/vendor Vulkan driver) and rerun the existing properties/VRS probe. If the values remain true after this API-boundary hard gate is compiled and loaded, instrument the Vulkan property population path at runtime; do not ask the user to repeat generic package checks. Preserve the current VRS PASS and continue separate VRS combiner/workload tests after timestamp consistency is fixed.


## 2026-11-08 — Expand real-GPU VRS coverage beyond the verified 2x2 case

The next VRS step is now implemented in the Winlator probe source on `experiment/g720-dynamic-fsr-sysval`:

- Commit `c07f68f8e85ffbcfa5b648d832c2fe5cdfb9bf44`: add `--case=rate-matrix` to `tools/panvk_vrs_render_probe_win.c`. It exercises the advertised pipeline-only rates `2x1`, `1x2`, and `4x4`, in both static pipeline state and dynamic `vkCmdSetFragmentShadingRateKHR` state.
- For each rate, the probe checks a changed pixel signature against the 1x1 control, repeated-neighbor structure, expected fragment invocation count (area-scaled), and exact static-vs-dynamic pixel/invocation equivalence. The existing 2x2 control and dynamic KEEP path remain prerequisites.
- Commit `ebe68c68a96f92ac3229c9535bf24cb97651c6a6`: add `tools/run-vrs-rate-matrix.cmd` for a one-command Winlator run.
- Commit `45a8b2b57457a7eb77c2cda95ad736e7bdabcbdf`: include the new runner in the VRS probe build workflow trigger paths and uploaded artifact.

The rate set matches the current PanVK KHR rate enumeration: 2x1, 1x2 and 4x4 are advertised at 1x MSAA; the probe uses 1x MSAA. This is a test expansion, not a claim those three rates have passed. Existing verified status remains static 2x2 KEEP and dynamic 2x2 KEEP only. No combiner other than KEEP/KEEP is enabled or claimed. The next step is to obtain the rebuilt Win64 probe artifact, run `tools/run-vrs-rate-matrix.cmd` in Winlator with the experimental PanVK driver selected, and review the resulting log before changing advertised rate properties or recommending VRS for DXVK games.

Source review also confirmed that the current dynamic fragment-rate sysval is populated once per draw and decoded in the fragment shader; the new matrix specifically checks that each dynamic rate produces the same output and invocation count as its static equivalent. This remains a targeted correctness probe, not a derivative/implicit-LOD compatibility proof for arbitrary game shaders.


The matrix was then widened to cover the complete set of currently enumerated non-2x2 pipeline rates, not only the initial three:
- Commit `f68ae93e32ec591dae26338260e085fd8143807b`: include `4x2` and `2x4` alongside `2x1`, `1x2`, and `4x4`. The existing baseline and dedicated 2x2 checks cover the remaining advertised sizes.
- Commit `9c7e4688a37cc6b709b320d49a50f8f99a9bd15d`: clarify the runner description to state that it exercises all advertised non-2x2 rates.

Thus the next probe run checks all seven sizes returned by PanVK's current KHR rate enumeration (1x1 and 2x2 are the baseline/control paths; the new matrix covers 2x1, 1x2, 4x2, 2x4, 4x4), static and dynamic KEEP. A rate is not considered validated until both modes pass on the G720.


## 2026-11-08 — VRS matrix probe build validation and combiner-spec correction

The first Win64 probe CI attempts failed at link time because the standalone probe used Mesa's `ARRAY_SIZE` macro without including Mesa utility headers. This was a probe-only build issue, not a PanVK driver failure.

- Commit `8eb434070300a2b641f7585b6a8109067acc693c` replaces that dependency with portable `sizeof(array) / sizeof(array[0])`.
- GitHub Actions run `42507915708` completed successfully and produced artifact `13627523816` (`panvk-vrs-render-probe-win64`). The subsequent usage-string-only correction is commit `8d32632b50644bccd54392106b00fd00d424fbcb`; it does not change probe behavior.
- The Android-only PanVK build at the timestamp API hard-gate source revision `cabe200fd0c72e1267269b308171d0747a43f24a` also completed successfully in run `42507009169`, producing artifact `13627504804` (`panvk-g720-android-only-experimental-package`). That build contains the timestamp API-boundary guards and the existing VRS implementation. The subsequent tool-only commits do not alter the driver binary.

A Vulkan-spec audit corrected the earlier interpretation of the combiner tests: with both `primitiveFragmentShadingRate=false` and `attachmentFragmentShadingRate=false`, valid API use requires `combinerOps[0]` and `combinerOps[1]` to be KEEP. REPLACE is only valid for the corresponding enabled primitive/attachment input; MIN/MAX/MUL additionally require `fragmentShadingRateNonTrivialCombinerOps=true`. Therefore the probe's non-KEEP paths are correctly skipped for this device feature combination. Do not treat an invalid non-KEEP call/device-lost result as evidence of a Mali hardware defect. The current truthful capability report remains pipeline FSR only, KEEP/KEEP only.

Next user-side validation is now justified: install the experimental PanVK package from run `42507009169` through Winlator's custom driver import (do not replace the Android system driver), then run the new `tools/run-vrs-rate-matrix.cmd` from the Win64 probe artifact. Send the complete `panvk_vrs_render_probe.log`; this is the only required action before deciding whether all advertised rates are genuinely supported.


## 2026-11-08 — VRS rate-matrix runtime review and CSF state trace

The user supplied the rate-matrix probe output from the actual Wrapper(Mali-G720 MC7) device, running PanVK Mesa 26.3.0-devel at revision `git-cabe200fd0`, Vulkan API 1.3.354.

### Confirmed runtime results in the supplied log

- Kbase timestamp reporting is now internally consistent: `timestampComputeAndGraphics=0`, `timestampPeriod=0 ns`, and graphics queue `timestampValidBits=0`. The probe correctly labels fence timing as CPU+GPU elapsed time, not GPU-only timing.
- Device creation, FSR feature enablement, invocation-counter descriptor/buffer allocation, image and staging allocation, pipeline creation, queue submission, fence waits, and readbacks all succeed.
- 1x1 KEEP: 4,096 fragment invocations.
- 2x2 KEEP: 1,024 invocations; the 4:1 reduction and pixel signature pass.
- 2x1 static and dynamic KEEP: both 2,048 invocations; pixels and static/dynamic equivalence pass.
- 1x2 static and dynamic KEEP: both 2,048 invocations; pixels and static/dynamic equivalence pass.
- 4x2 static and dynamic KEEP: both 1,024 invocations, while the probe expects 512. Pixel checks pass, but the invocation-count check fails.
- 2x4 static and dynamic KEEP: both 1,024 invocations, while the probe expects 512. Pixel checks pass, but the invocation-count check fails.
- The user-supplied message ends during the 4x4 setup, before its result. Do not record a 4x4 runtime result from this excerpt. If a later complete log provides it, append the result separately.

### Correction to the preceding assistant diagnosis

A source trace disproves the suggestion that PanVK only updates the shader-side `fs.frag_rate` and fails to program the rasterizer.

On `experiment/g720-dynamic-fsr-sysval` at HEAD `8d32632b50644bccd54392106b00fd00d424fbcb`:

- `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`, function `set_tiler_idvs_flags()`, reads the effective `dyns->fsr.fragment_size` and combiner operations.
- It packs `PRIMITIVE_FLAGS_2` and writes the packed value directly to `IDVS.TILER_FLAGS2` with `cs_move32_to()`.
- `src/panfrost/genxml/v12.xml` defines the hardware VRS draw-rate encodings: 1x1=0, 1x2=1, 1x4=2, 2x1=4, 2x2=5, 2x4=6, 4x1=8, 4x2=9, 4x4=10. The driver's expression `(log2(width) << 2) | log2(height)` maps the tested sizes to those exact values.
- `src/panfrost/vulkan/panvk_vX_cmd_draw.c` separately populates `fs.frag_rate` for the shader-side sample-centre lowering. That is not the hardware rasterizer programming path.

Thus the next investigation must not start by adding a missing TILER_FLAGS2 write. The register write already exists. The observed plateau at 1,024 invocations for 4x2 and 2x4 needs a discriminating test to determine whether the GPU is clamping the requested state, the probe's atomic-based invocation counter is not measuring the effective coarse rate as assumed, or another pipeline condition is forcing a smaller effective rate.

### Hardware capability cross-check

Arm's official Mali-G720 product page states that the GPU supports pipeline, primitive, and attachment VRS up to 4x4: https://support.arm.com/compute-ip/mali-g720 . The G720 performance-counter reference guide describes the fragment shading-rate counter as coarse quads generated relative to fine quads: https://documentation-service.arm.com/static/67487f4427eda361ad4dc5f6?token= . This confirms that 4x4 is a hardware capability of the G720 family; it does not prove that this specific PanVK implementation is programming it correctly.

### Next engineering actions

1. Inspect the complete 4x4 result if the user supplies the remaining lines; do not infer it from the truncated excerpt.
2. Audit the test's invocation-count measurement independently from its pixel signature. The current fragment shader performs an SSBO `atomicAdd` once per invocation and also writes a `gl_FragCoord` pattern. Keep the counter and pixel-pattern results separately classified.
3. Add a targeted, low-noise diagnostic of the effective Vulkan FSR state and the packed `TILER_FLAGS2` value for the tested large rates. Avoid unconditional per-draw logging in ordinary game workloads.
4. Compare the actual command-state values for static and dynamic 4x2, 2x4, and 4x4. Confirm the effective rate is not changed by sample shading, rasterization sample count, or another pipeline state before draw submission.
5. Only after this separation should the driver rate enumeration or hardware programming be changed. Do not reduce the advertised limits or claim larger rates work until a corrected probe demonstrates the expected behavior.

No PanVK source, feature advertisement, Kbase behavior, package, kernel, or Android system/vendor driver was changed in this review. Keep the VRS work isolated from the performance-audit and descriptor-heap branches.


### Follow-up source diagnostic committed

After tracing the actual CSF register path, a narrowly scoped diagnostic was added on `experiment/g720-dynamic-fsr-sysval`:

- Commit `6ff24e63fefb7eee875d4a99353a4f45224c4845`.
- File: `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`, in `set_tiler_idvs_flags()`.
- For requested rates with width or height greater than 2, PanVK now logs requested size, effective size after sample-shading rules, sample-shading enable, packed VRS rate code, decoded combiner fields, and the full `PRIMITIVE_FLAGS_2` packed word immediately before writing `IDVS.TILER_FLAGS2`.
- The diagnostic is limited to larger rates; it does not log every ordinary 1x1/2x2 draw. It does not alter the encoded state or feature advertisement.

CI triggered automatically:
- Android-only package build run `42509014534`.
- Android wrapper build run `42509014577`.
- Both were in progress when this note was written. Their results and artifact IDs must be added after completion.

This is diagnostic instrumentation only. It is not a claimed VRS fix. Do not ask for a new device run until the Android package build succeeds and its artifact is available. Continue to keep the source branch and package separate from the performance-audit branch and never replace the Android system/vendor driver.


### 2026-11-08 — VRS pixel-signature diagnostic strengthened

A second probe-only change was committed on `experiment/g720-dynamic-fsr-sysval`:

- Commit `13824a90757752f4796a67f73b15ddd142699ec9`.
- `tools/panvk_vrs_render_probe_win.c` now reports equal horizontal and vertical neighboring-pixel counts separately for each static rate-matrix case, in addition to the combined count and SSBO atomic invocation count.
- This makes the output pattern directional: an actual 4x2 coarse region should produce a different horizontal/vertical repetition signature from 2x2; 2x4 should produce the inverse signature. This is an independent check against relying on a single aggregate neighbor count.
- The diagnostic does not change the shader, driver, or advertised features. Existing aggregate pixel checks and invocation-count checks remain in place.
- Winlator probe CI run `42509283686` was triggered for this commit. Android-only and wrapper CI also triggered (`42509283794`, `42509283912`) because the workflow watches the branch; these builds are not evidence of a driver change since only the probe changed in this commit.

Source review also confirmed the Mali v11/v12 GenXML VRS field layout: the 4-bit draw-rate enum is at bits 0..3; combiner 0 at bits 4..6; combiner 1 at bits 7..9. The enum values include 2x4=6, 4x2=9, and 4x4=10. PanVK's formula `(log2(width) << 2) | log2(height)` therefore matches the documented field enum and the packed diagnostic's decode. Do not change axis packing without new runtime evidence.

Arm's G720 product specification lists pipeline, primitive, and attachment VRS up to 4x4. The G720 performance-counter guide's coarse-quad metric is useful for hardware profiling, but it is not the same measurement as this shader SSBO atomic invocation count. Keep those measurement concepts distinct.

Next steps:
1. Inspect CI run `42509283686`; only offer the new probe to the user after it succeeds.
2. Inspect Android package run `42509283794`; the diagnostic package from `6ff24e6` is sufficient for CSF register logs if it succeeds. Since `13824a9` only changed the probe, the driver binary need not be rebuilt again for the axis-signature probe, but a matching artifact is preferable if it completes.
3. In the next device log, compare requested/effective/packed rate code with horizontal and vertical pixel repetition counts and SSBO invocation counts for each rate. A 2x2-like axis signature at larger requested rates indicates clamping or a probe/state interaction; a correct directional signature with flat SSBO count indicates the counter path is misleading.
4. Keep VRS validation separate from the broader performance work; no game-performance gain is established by this synthetic probe.


### Directional probe build result

- Winlator probe workflow run `42509283686` completed successfully for commit `13824a90757752f4796a67f73b15ddd142699ec9`.
- Artifact: `panvk-vrs-render-probe-win64`, artifact ID `13629050724`, size 119,691 bytes, not expired at time of check.
- The probe now emits separate horizontal and vertical equal-neighbor counts for each static matrix rate.
- PanVK reports `pipelineStatisticsQuery=false`, so the probe cannot use `VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT` as a second hardware-side invocation measurement. Retain the SSBO atomic counter and directional pixel signature as two distinct observations.
- Android-only diagnostic package run `42509014534` was still building at the time of this update. It is the preferred package for the next test because it includes the targeted CSF packed-state log from commit `6ff24e6`. The Winlator probe artifact above is already ready, but wait for the Android package before asking the user to test.


### Directional pixel footprint is now checked against exact expected geometry

Probe commit `84b6c3d745a4865595893da312f4ca20f627ee14` extends the previous axis logging:

- The probe computes expected horizontal and vertical equal-neighbor counts from the requested fragment dimensions and 64x64 render target.
- It reports actual/expected horizontal and vertical counts plus a `shape=PASS/FAIL` result.
- The rate-matrix pass condition now requires the exact directional pixel footprint in addition to the SSBO invocation count and static/dynamic equivalence.
- Expected signatures for the current 64x64 target are:
  - 2x1: H=2048, V=0
  - 1x2: H=0, V=2048
  - 4x2: H=3072, V=2048
  - 2x4: H=2048, V=3072
  - 4x4: H=3072, V=3072
  - 2x2 control: H=2048, V=2048
- This provides a more discriminating readout than the previous aggregate equal-neighbor total. If larger requested rates still produce H=2048/V=2048, their rendered footprint is 2x2-like even if the CSF register encoding is correct.

The previous probe build `42509283686` / artifact `13629050724` corresponds to commit `13824a9` and does not include the exact shape assertion. The new commit `84b6c3d` triggers a fresh probe build; do not ask the user to run the older artifact for the final directional test.


## 2026-11-08 — Candidate root cause found: v12 DCD VRS 2x2 clamp

A source/GenXML audit found a likely explanation for the exact 2x2 invocation plateau:

- In `src/panfrost/genxml/v12.xml`, `DCD_FLAGS_0` contains `Disable VRS Clamp 2x2` at bit 12. The same field is present in v13 and v14 GenXML.
- In the existing `build_dcd_flags()` implementation in `src/panfrost/vulkan/csf/panvk_vX_cmd_draw.c`, PanVK packed `DCD_FLAGS_0` without setting this field. Since `out` is zeroed before packing, the disable bit remained 0.
- PanVK already packed the larger requested rates correctly into `IDVS.TILER_FLAGS2`. A separate DCD-level 2x2 clamp would explain why 4x2, 2x4, and 4x4 requests all rendered with a 2x2-like signature.
- Arm's official G715 developer overview describes the Mali VRS rate set as 1x1, 1x2, 2x1, 2x2, 2x4, 4x2, and 4x4, confirming that larger rates are intended hardware functionality on the family. The G720 product page separately lists pipeline VRS up to 4x4.

### Candidate fix committed

Commit `3b9e6a7d0f83abf929162ebef62cc96b3dc656f0` on `experiment/g720-dynamic-fsr-sysval`:

- Sets `cfg.disable_vrs_clamp_2x2 = true` in `build_dcd_flags()` for `PAN_ARCH >= 12`.
- Leaves sample-shading behavior unchanged; `set_tiler_idvs_flags()` still forces the effective draw rate to 1x1 when sample shading is enabled.
- Adds a targeted `PANVK_VRS_DIAG DCD_FLAGS_0` line for requested rates larger than 2 in either dimension, reporting bit 12, the packed DCD word, and requested dimensions.
- Retains the separate `TILER_FLAGS2` diagnostic from commit `6ff24e6`.
- This is a candidate fix, not yet runtime-validated. The bit's intended polarity is directly indicated by the GenXML field name; the device test must demonstrate that larger rates now reach the expected pixel footprint and invocation count.

Builds triggered for candidate fix commit `3b9e6a7`:
- Android-only package build and wrapper build; record run IDs/artifact IDs once complete.
- The directional Winlator probe is independently built from probe commit `84b6c3d` as artifact `13628887002` (run `42509380933`), but the final device test should use it with the candidate-fix Android package.

### Next validation

1. Confirm Android-only candidate package build succeeds and inspect its package audit before handing it to the user.
2. Use the Winlator probe artifact `13628887002` with the candidate PanVK package. Run `tools/run-vrs-rate-matrix.cmd`.
3. Confirm each larger rate emits DCD bit 12 = 1 and the correct TILER_FLAGS2 enum (4x2=9, 2x4=6, 4x4=10).
4. Confirm directional signatures and SSBO invocation counts match the expected rate:
   - 4x2: H=3072, V=2048, invocations=512
   - 2x4: H=2048, V=3072, invocations=512
   - 4x4: H=3072, V=3072, invocations=256
5. If the DCD bit is set and larger rates still clamp, investigate DCD packing/serialization and the hardware clamp semantics before touching the TILER_FLAGS2 encoding.


The candidate clamp change received a whitespace-only follow-up commit `3cfbf43c3db1d666392d76b494443eddd288175c` to align the inserted block with surrounding code. No behavior changed. The Android-only build for `3b9e6a7` is still useful for compile/runtime validation; a newer build may also be triggered for the branch tip.


### Diagnostic-only Android package build completed

- Android-only run `42509014534` completed SUCCESS for source commit `6ff24e63fefb7eee875d4a99353a4f45224c4845`.
- Artifact `13627869712`, `panvk-g720-android-only-experimental-package`, 16,821,512 bytes, not expired.
- The full compile, staged install, driver/manifest/dependency audit, flat Winlator package build, and artifact upload all passed.
- This package includes the large-rate `TILER_FLAGS2` diagnostic but predates the DCD clamp fix. It is not the package to use for the next rate-matrix run; wait for the candidate-fix package from run `42509483552` or newer.
- The exact-shape Winlator probe for commit `84b6c3d` is artifact `13628887002` from successful run `42509380933`.


### Cross-check against independent PanVK v12 implementation and internal DCD paths

A GitHub code search found an independent Mesa-derived PanVK implementation in `DsoTsin/mesa-manet` (commit `c6ffbdc5ea4685203e89d648e71cc6c7ecfc44ca`). Its v12+ source sets `disable_vrs_clamp_2x2 = true` in the regular DCD flags path and also sets `flags_0.disable_vrs_clamp_2x2 = true` for internal fullscreen/frame-shader DCDs. This independently confirms that the field is intended to be set for v12+ PanVK draws, rather than only for one special large-rate case.

Our branch was extended to mirror those internal paths:

- Commit `3ffa1bf0d30a7830eac22d41f3993ee4844c7212`: set the bit in the CSF `cmd_fb_barrier` fullscreen DCD.
- Commit `9131a736ea1323174069fec9c6617c69bc6538d7`: set the bit in the frame-shader fallback DCD path.
- Commit `c027d4d8c747787c85256cfc0e9768e991262c16`: set the bit in the PAN_ARCH >= 12 frame-shader DCD base descriptor. This is the current branch HEAD.
- The ordinary CSF graphics DCD path remains covered by candidate commit `3b9e6a7`; the later changes complete the internal paths.
- The draw-rect meta path obtains `DCD_FLAGS_0` from `build_dcd_flags()`, so it inherits the main fix.

All changes are compile/runtime candidates only. They do not expose primitive/attachment FSR features or change the advertised rate list. The next Android package must be built from `c027d4d8` or later, not from the earlier `3b9e6a7` artifact, so all ordinary and internal DCD paths are represented in the test binary.


### Candidate core-fix package compiled and passed package audit

- Android-only run `42509483552` completed SUCCESS for commit `3b9e6a7d0f83abf929162ebef62cc96b3dc656f0`.
- Artifact ID `13628843631`, name `panvk-g720-android-only-experimental-package`, size 16,823,004 bytes.
- The log confirms the AArch64 Android API 30 Vulkan driver was built, staged, audited, packaged, and uploaded successfully. The flat ADPKG ZIP SHA-256 is `6c5b01fb623ff24fc77152db696f043219cbc3698526895a2f09184e72bae666`.
- This package contains the ordinary-draw DCD clamp fix, but not the later internal fullscreen/frame-shader path completion. It is sufficient to test the main pipeline-rate clamp hypothesis; the preferred test package remains the current branch-tip build `42509835857` for commit `c027d4d`.


### Full candidate package for the next device test is ready

- Android-only PanVK build run `42509835857` completed SUCCESS for exact source commit `c027d4d8c747787c85256cfc0e9768e991262c16`.
- Artifact ID: `13628779694`, name `panvk-g720-android-only-experimental-package`, size 16,825,016 bytes.
- Artifact page: https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42509835857/artifacts/13628779694
- The workflow completed compile/stage, Android ARM64 ELF audit, ICD manifest/dependency audit, flat ADPKG packaging, and upload.
- Inner importable `PanVK-G720-experimental.zip` SHA-256: `5ea2bb16e45cf4591a56786cdd531c41f454c6429103b3b48352aa0d8982c161`.
- Driver SHA-256 before packaging: `3d105175d0b95ab9d93d087ab6f15c9e7e72584c6b8a259f4891f0b9f11b6ff0`.
- Driver is ELF64 AArch64, Android API 30; the package audit and upload succeeded.
- This is the first package containing the complete ordinary-draw, fullscreen barrier, and frame-shader DCD clamp changes.

The matching exact-shape Winlator render probe is artifact `13628887002` from run `42509380933`, source commit `84b6c3d745a4865595893da312f4ca20f627ee14`. It includes `tools/run-vrs-rate-matrix.cmd` and reports horizontal/vertical pixel-footprint checks as well as SSBO invocation counts.

### User test gate — ready now

Use only the candidate PanVK package from run `42509835857` and the render probe artifact from run `42509380933`:

1. Download the Android-only artifact and extract the nested `PanVK-G720-experimental.zip`.
2. Import it in Winlator Mali 1.2 as a separate custom driver. Do not replace the Android system/vendor driver.
3. Download/extract the Win64 probe artifact and run `tools/run-vrs-rate-matrix.cmd`.
4. Send the complete `panvk_vrs_render_probe.log` and any PanVK diagnostic output containing `PANVK_VRS_DIAG`.

Expected large-rate values if the DCD clamp fix works:
- 4x2: DCD disable-clamp bit 12 = 1, TILER rate code 9, H=3072/V=2048, SSBO invocations=512.
- 2x4: DCD disable-clamp bit 12 = 1, TILER rate code 6, H=2048/V=3072, SSBO invocations=512.
- 4x4: DCD disable-clamp bit 12 = 1, TILER rate code 10, H=3072/V=3072, SSBO invocations=256.

Keep the 1x1, 2x1, 1x2, and 2x2 controls passing. If the large rates pass, next validate the shader semantics that matter to real workloads (derivatives/implicit LOD, interpolation, discard/depth/sample behavior) before considering the feature production-safe. No game-FPS claim follows from this synthetic test.

## 2026-11-08 — VRS full rate-matrix device validation passed

The user supplied the complete panvk_vrs_render_probe.log after importing the candidate PanVK package into Winlator Mali 1.2 on the target Wrapper(Mali-G720 MC7) device. File SHA-256: 846cdb67b7dcae4b6f70f597bc8b02e877433198842d29464970642aca869b75.

### Confirmed static pipeline-rate results

| Rate | Expected invocations | Measured | Horizontal equal neighbors | Vertical equal neighbors | Result |
|---|---:|---:|---:|---:|---|
| 1x1 | 4096 | 4096 | 0 | 0 | PASS |
| 2x1 | 2048 | 2048 | 2048 | 0 | PASS |
| 1x2 | 2048 | 2048 | 0 | 2048 | PASS |
| 2x2 | 1024 | 1024 | 2048 | 2048 | PASS |
| 4x2 | 512 | 512 | 3072 | 2048 | PASS |
| 2x4 | 512 | 512 | 2048 | 3072 | PASS |
| 4x4 | 256 | 256 | 3072 | 3072 | PASS |

The 4x2, 2x4, and 4x4 rates now produce the expected 4-wide/4-high pixel footprints and 8x/8x/16x invocation reductions relative to 1x1. This resolves the earlier 2x2 plateau. All static rate cases report render/pixel/count/shape success.

### Dynamic command-buffer cases

The complete log includes dynamic vkCmdSetFragmentShadingRateKHR cases for 2x1, 1x2, 4x2, 2x4, and 4x4. Each matches its corresponding static checksum and invocation count. The dynamic test path successfully records the command, submits, waits for a fence, and reads back the result. The log ends with `advertised rate matrix: PASS` and `VRS GPU RENDER PASS`.

### What this proves — and what it does not

- Proven on this target/package: pipeline VRS rate programming and the tested dynamic rate command produce the correct simple-shader pixel footprint and SSBO atomic invocation count for the seven listed rates.
- Not yet proven: derivative/implicit texture LOD semantics, interpolation and centroid behavior, helper invocation semantics, shader discard/depth/stencil/coverage interactions, multisample/sample-shading interactions, render-pass attachment/primitive VRS, broader Vulkan conformance, stability in games, or measurable game-FPS gains.
- Do not expose primitive- or attachment-based VRS features based on this result. Keep their feature bits disabled until their independent implementation and validation are complete.
- Do not claim that 4x4 VRS makes games 16x faster; this is a shader invocation reduction in a controlled synthetic workload, not a frame-time measurement.

### Next autonomous engineering work

1. Audit vkGetPhysicalDeviceFragmentShadingRatesKHR, VkPhysicalDeviceFragmentShadingRatePropertiesKHR, pipeline FSR feature gating, dynamic FSR state storage/dirty tracking, and the NIR/FSR shader-lowering path on experiment/g720-dynamic-fsr-sysval.
2. Audit how fs.frag_rate / sample-centre lowering interacts with the hardware rate, especially derivatives and implicit texture LOD. Do not change working hardware rate encoding or DCD clamp without new evidence.
3. Extend the Windows render probe with targeted semantic tests: a varying texture-coordinate/implicit-LOD test with a mipmapped texture; interpolation gradients across coarse regions; discard/depth side effects; and sample-shading fallback. Keep each test isolated and report per-test pass/fail.
4. Keep all new tests and source changes on the VRS experiment branch; do not merge into the performance-audit, descriptor-heap, clip/cull, or system-driver branches.
5. Build the probe in CI and use a device run only after the probe is ready. No further user action is needed until a device runtime result is genuinely required.

## 2026-11-08 — Post-pass VRS capability and shader-lowering audit

After the successful device rate matrix, the current VRS implementation was re-audited on experiment/g720-dynamic-fsr-sysval. No driver source change was made in this audit.

### Feature and rate advertisement

- src/panfrost/vulkan/panvk_vX_physical_device.c exposes VK_KHR_fragment_shading_rate only for PAN_ARCH >= 12 and enables pipelineFragmentShadingRate only. primitiveFragmentShadingRate and attachmentFragmentShadingRate remain false.
- The queried rate list in src/panfrost/vulkan/panvk_physical_device.c matches the rates validated by the render probe: 4x4, 4x2, 2x4, 2x2, 2x1, 1x2, and 1x1. The 4x4 and 4x2 modes are advertised for 1x MSAA; 2x4 is advertised for 1x/2x; 2x2, 2x1, and 1x2 for 1x/2x/4x; 1x1 for all sample counts.
- VkPhysicalDeviceFragmentShadingRatePropertiesKHR reports maxFragmentSize={4,4}, maxFragmentSizeAspectRatio=2, maxFragmentShadingRateCoverageSamples=16, and maxFragmentShadingRateRasterizationSamples=4x. Restrictions involving shader depth/stencil writes, sample mask, conservative rasterization, interlock, and custom sample locations remain false.
- fragmentShadingRateNonTrivialCombinerOps remains false; primitive and attachment FSR remain disabled. The successful KEEP matrix does not justify changing these restrictions.

### Static/dynamic shader-centre lowering

- panvk_vX_shader.c lowers NIR load_sample_pos_or_center for static pipeline FSR to a compile-time center offset derived from the effective rate. The rate is included in the shader cache key, and the pipeline rate is combined against implicit 1x1 primitive/attachment rates for the supported pipeline-only configuration.
- Dynamic pipeline FSR uses the 32-bit fs.frag_rate member in panvk_shader.h, loaded by lower_dynamic_vrs_frag_center() and populated per draw by cmd_prepare_draw_sysvals() in panvk_vX_cmd_draw.c. The rate encoding matches the hardware field: log2(width) in bits 2..3 and log2(height) in bits 0..1.
- set_tiler_idvs_flags() marks FSR state dirty and programs PRIMITIVE_FLAGS_2 / IDVS.TILER_FLAGS2. The render matrix proves the tested static/dynamic KEEP path updates this state correctly.
- The probe's current shader uses floor(gl_FragCoord.xy) to form its output pattern. Its directional pixel-footprint checks therefore also exercise the coarse fragment-coordinate centre path; this is stronger than invocation counts alone, but it does not test interpolated user varyings or texture derivatives/implicit LOD.

### Remaining semantic test gaps

1. Add a mipmapped texture sampled with implicit derivatives while a smooth UV gradient varies across the screen. Compare 1x1 and coarse rates using stable image-region/LOD signatures; avoid treating exact implementation-specific LOD as a fixed universal value.
2. Add a varying-interpolation test that writes interpolated UV/color values to the target and checks that values are evaluated at the correct coarse-fragment location.
3. Add isolated discard and depth/stencil side-effect cases, respecting the reported false interaction properties; unsupported combinations should be rejected or skipped rather than silently claimed supported.
4. Add a sample-shading pipeline control to verify the existing driver fallback forces effective hardware rate 1x1 and matching shader sample centre.
5. Keep all semantic tests at 1x MSAA initially. Only add sample-count variants that are present in the returned rate list and supported interaction properties.

No additional user action is required yet. Continue with the semantic-probe design and CI build; request a device run only after a complete, deterministic probe artifact is ready.


## 2026-11-08 — VRS derivative-semantics probe implementation

A follow-on semantic probe has been implemented on `experiment/g720-dynamic-fsr-sysval`, after the earlier hardware matrix passed.

- Source commits:
  - `d71c22f8b0b83a8400ac406ca536a2aa7faf1906` — extend `tools/panvk_vrs_render_probe_win.c` with derivative-signature assertions.
  - `48e96f996aad2f6ab50155fd64bad9d9f7dac519` — emit a smooth affine UV varying from `tools/vrs_probe.vert`.
  - `e7dbced9a7799c0eec5c00a3aef1b3af9fb8ccee` — encode `dFdx(test_uv.x)` and `dFdy(test_uv.y)` in the output B/A channels from `tools/vrs_probe.frag`.
- The probe expects derivatives to scale with the fragment footprint: for a 64x64 target and WxH fragment rate, the affine UV derivative spans W/64 and H/64. The shader encodes these as normalized 8-bit signatures; host validation checks the centre pixel with a +/-2 UNORM tolerance.
- Static and dynamic rates in the existing advertised rate matrix must each report the expected derivative signature, in addition to matching invocation counts and output checksums. The test also checks the 1x1 and 2x2 baseline signatures.
- This is relevant to DXVK because Vulkan's derivative operations are measured between fragment invocations; coarse multi-pixel fragments can produce larger derivatives and therefore select coarser implicit texture LOD. This test checks derivative scaling without introducing texture descriptor/resource setup yet.
- CI workflow `build-vrs-render-probe.yml` was triggered for source commit `d71c22f8b0b83a8400ac406ca536a2aa7faf1906`; run ID `42515697158` was queued at handoff update time. Build status and artifact are pending.
- No PanVK driver source changes were made in this step. This is a diagnostic probe change only. Do not import or benchmark a new Android driver package based on these commits; wait for the probe build and then the device-side semantic result.


### Derivative probe CI result and device handoff

- Final shader commit `e7dbced9a7799c0eec5c00a3aef1b3af9fb8ccee` triggered Windows probe workflow run `42515701317`.
- CI completed successfully: SPIR-V generation, MinGW C compilation, PE verification, and artifact upload all passed. The compiler emitted only existing unused-function-pointer and GetProcAddress cast warnings; there were no compile errors.
- Artifact: `panvk-vrs-render-probe-win64`, artifact ID `13630399137`, 120,440 bytes.
- Executable SHA-256: `84cd818fe269f0239e2723c26891ca96fcf839e7e0a1aabd2e5630730d953b77`.
- Device run command after extracting the artifact while preserving its directory layout: `tools\\run-vrs-rate-matrix.cmd`. It runs `panvk-vrs-render-probe.exe --case=rate-matrix` and prints `panvk_vrs_render_probe.log`.
- The test now validates derivative signatures for 1x1, 2x2, 2x1, 1x2, 4x2, 2x4, and 4x4; each matrix rate is tested in both static pipeline state and dynamic command-buffer state. The expected signatures scale with the actual WxH fragment footprint.
- This remains a Windows diagnostic executable only. No new Android PanVK package was built or changed. Do not treat this as a driver update.
- User action is now required for the device-side run. Capture the complete `panvk_vrs_render_probe.log` and return it unchanged; especially retain derivative-signature lines and the final PASS/FAIL line.


## 2026-11-08 — G720 VRS derivative-signature device result

The user ran the derivative-extended Windows probe on the target Winlator Mali-G720 MC7 device and supplied the complete log.

- Log SHA-256: `c8356152429b8e523df8017096cb97cea04229ca816aec1c6f715584acde88a3`.
- Runtime device: `Wrapper(Mali-G720 MC7)`, PanVK Mesa `26.3.0-devel (git-c027d4d8c7)`, Vulkan API `1.3.354`.
- Final status: `advertised rate matrix: PASS` and `VRS GPU RENDER PASS`.
- Derivative signatures passed for 1x1 control and 2x2 baseline, and for every static and dynamic matrix rate:
  - 2x1: encoded dFdx=128, dFdy=64.
  - 1x2: encoded dFdx=64, dFdy=128.
  - 4x2: encoded dFdx=255, dFdy=128.
  - 2x4: encoded dFdx=128, dFdy=255.
  - 4x4: encoded dFdx=255, dFdy=255.
  - Each measured channel exactly matched the expected encoded signature (within the probe's +/-2 tolerance).
- Invocation counts remained exact: 1x1=4096; 2x1=2048; 1x2=2048; 2x2=1024; 4x2=512; 2x4=512; 4x4=256. Static and dynamic render checksums matched at each tested matrix rate.
- This is consistent with Vulkan's fragment shading rate semantics: derivative results may be coarse or fine but implementations must choose consistently between x/y; pixel derivatives are recovered by dividing dFdx by fragment width and dFdy by fragment height. Implicit-LOD sampling uses those derivatives as its Grad operands. See the Vulkan specification fragment-shading-rate derivative discussion and implicit-LOD rule.
- No evidence of a derivative scaling defect was found in the tested pipeline-only KEEP path. This is a targeted affine-varying derivative test, not yet a mipmapped texture sampling/implicit-LOD test, and does not validate all interpolant/helper-invocation edge cases.
- No PanVK driver source change is indicated by this result. Next engineering step: add an actual mipmapped sampled-image test with implicit LOD (plus a stable reference signature), then test perspective-correct varying interpolation. Keep sample-shading and unsupported depth/stencil/sample-mask interactions separate and respect the advertised false interaction properties.
- The uploaded log contains literal backslash-n sequences in some diagnostic messages due to escaped newline formatting in the probe's LOG strings; the actual test result and all derivative numeric values remain readable and unambiguous.


### Follow-up precision note and next test design

- The uploaded derivative log includes a 1x1 control and a 2x2 baseline derivative signature (static baseline). The additional static+dynamic derivative matrix explicitly iterates 2x1, 1x2, 4x2, 2x4, and 4x4. Do not state that a separate dynamic 2x2 derivative signature was captured in this run; earlier 2x2 dynamic render/invocation validation is distinct.
- The Vulkan specification explicitly notes that derivatives are calculated between fragments rather than pixels; when fragment invocations cover multiple pixels, derivatives cover the wider area and implicit-LOD image sampling uses those derivatives as Grad operands. Pixel derivatives can be obtained by dividing dFdx by fragment width and dFdy by fragment height. This validates the rationale behind the new affine-varying derivative signature test.
- Proposed next test: create a 256x256 sampled image with five mip levels, each mip level filled with a distinct constant RGBA signature; use a linear affine UV gradient spanning [0,1] across the 64x64 render target and a nearest-mipmap sampler. At 1x1, the texture-space derivative magnitude is approximately 4 texels/invocation (LOD 2); 2x rates increase the footprint to approximately 8 texels (LOD 3); 4x in either dimension reaches approximately 16 texels along the major axis (LOD 4). Validate the returned per-level color signatures for static and dynamic FSR. Use margins / robust LOD boundaries and check the device's sampled-image/format support before treating the expected mip as deterministic.
- Keep this as a separate semantic probe mode to avoid destabilizing the already-passing invocation/derivative matrix. Required API plumbing includes image allocation with TRANSFER_DST|SAMPLED usage, staging upload for all mip levels, transfer-to-shader-read layout transitions, sampler and combined-image-sampler descriptor binding, and readback of the color signature. No driver changes are justified until this test identifies a real defect.
