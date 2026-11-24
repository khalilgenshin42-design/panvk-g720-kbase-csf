# Mali-G720 PanVK performance audit — 2026-11-05

## Latest authoritative state — 2026-11-24 13:45 Africa/Tunis

This section supersedes the 13:07 summary below. Preserve prior entries as history.

- Re-read the canonical handoff and this audit before checking GitHub. Pre-update branch heads were audit `f91590d4cc7f146ed8581c8bab015ff5bda6e053` and handoff `a310505aa4b71fe5a594e46ff9968797286f752d`. Executable active source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Compare source-to-active-branch is 34 ahead / 0 behind and lists only this audit Markdown file. No active executable source or workflow change exists.
- Branch-filtered Actions API returned 139 runs, with 100 newest in the first page. Latest active-source Android-only run `43716474968` (job `132819003208`) is completed/success, 15/15 returned steps completed/success. Latest wrapper run `43716474955`: Release `132819003046` and Debugoptimized `132819003226` are completed/success, 12/12 returned steps each. All three target exact source `73873d730110bf3394ceea19b2a4d45ddba73834`, created 2026-11-15 12:57:36Z. Pending 0, failed 0. No newer active-branch Android-only or wrapper run appeared. Initial watch runs `41947103034` and `41947103085` remain historical. No failure logs were required and no build was triggered.
- Re-downloaded Android artifact `14163230171` (16,747,780 bytes). Outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8` matches GitHub digest; outer ZIP passes integrity and contains the staged tree, audit records, and nested flat ZIP. Nested flat ZIP SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318` matches the embedded audit record. It passes ZIP integrity and contains exactly four members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`; all 3 embedded checksums pass. Metadata identifies version `26.3.0-devel-android-kbase-73873d730110`, minApi 30, experimental/non-conformant status, and unverified device/game compatibility. Both ELFs are AArch64 ELF64/Android 30. Driver RUNPATH `$ORIGIN`; private helper NEEDED only `libc.so`; driver NEEDED includes bundled helper and Android platform libraries. Staged ICD JSON is valid, API 1.4.354, architecture 64, library path `/usr/local/lib/libvulkan_panfrost.so`; staged files include Android `libdrm.so` stub and expat files. Recomputed dynamic symbol sets show 24 unique driver DRM imports and 212 unique helper DRM exports, zero unresolved. Static/package validation only; no device runtime claim.
- Re-downloaded wrapper Release artifact `14162259759` (4,288,198 bytes), SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`, and Debugoptimized artifact `14162119843` (50,651,760 bytes), SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`. Both match GitHub digests and pass ZIP integrity. Each archive contains only one AArch64 ELF64/Android 30 `libvulkan_panfrost.so`; RUNPATH `$ORIGIN/../../android_stub`; NEEDED includes external `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1`. They do not include the full dependency closure, ICD manifest, or checksum manifest and are not standalone Winlator packages.
- No new Winlator import, ICD/device-creation log, timestamp-property capture, heap-threshold proof, descriptor profile, stability report, or FPS measurement was available. Historical Tomb Raider results remain inconclusive. Separate timestamp-correction candidate `f5084ae0` remains unmerged and device-unvalidated; do not substitute its artifact or results for active source `73873d73`.
- No source, workflow, allocator policy, Kbase ABI, kernel, or Android system/vendor Vulkan driver was modified. No build was triggered. Preserve the non-rooted Android/Termux constraint and keep the experimental driver separate from the stock system driver.
- Next gate: import the verified source-73873 Android flat ZIP using Winlator Mali's custom-driver importer while retaining System Driver as fallback. Capture ICD load, physical/logical device creation, minimal render/submit, and teardown. Only after runtime correctness, prove the 262,144 override from the 65,536-default package's startup log before first Kbase submission; keep uninstrumented FPS tests separate from descriptor profiling.


## Latest authoritative state — 2026-11-24 13:07 Africa/Tunis

This section supersedes the 2026-11-23 13:08 summary below. Preserve prior entries as history.

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit`. Before this run's documentation updates, performance-audit HEAD was `37f4a623bd54767610d866628919494355d32472` (32 ahead / 0 behind executable source) and handoff HEAD was `2c4ed45aef7db43920acbcfd2a53fe873d58cdb7`. The first audit/handoff updates in this run were commits `c753ab41b4a912b5010f297b68394353c0a3beea` and `e89032e6febe74cd4f4c2a56391e63cf49bb52bc`; this follow-up correction is another docs-only commit on each branch. After these documentation-only updates, the performance branch is 34 ahead / 0 behind source with only this audit Markdown changed; the handoff branch is 3 commits ahead / 0 behind `f424d7c79b491ee59030dd35e1162e35b97efe59`, with only `PROJECT_CONTINUITY_HANDOFF.md` changed. No executable PanVK source or workflow changed on the performance branch.
- Re-read `src/panfrost/vulkan/panvk_vX_descriptor_set.c` at executable source `73873d73`. The five failure categories remain mutually exclusive: `overflow`, `no_free_set_only`, `heap_capacity_only`, `both_capacity`, and `vma`, plus aggregate `set_failures`. Overflow is counted before `set_allocs`/`set_alloc_bytes`; those latter counters count post-overflow allocation attempts and requested bytes, including requests that subsequently fail capacity or VMA allocation, not successful sets only. Overflow/capacity paths retain `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure retains `VK_ERROR_FRAGMENTED_POOL`. The additional heap-capacity read is still only in the profiling-enabled failure path. Counters use relaxed atomics and profile lines are cumulative; compare category sum to aggregate only in a quiescent final snapshot. The historical 299 failures predate the split and remain unclassifiable.
- Re-read `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. Compiled defaults remain `KBASE_TILER_HEAP_RENEW_INTERVAL=128` and `KBASE_TILER_HEAP_RENEW_WORK=65536`. The environment values are read with `debug_get_num_option()`, cached together through C11 `call_once`, and the effective work threshold is logged once independently of profiling. Work accumulation saturates at `UINT64_MAX`; only graphics submissions with nonzero tiler-work estimates contribute; renewal waits for graphics targets before replacing the heap. Interval and work triggers are independent and both counters may increment on one renewal. A 262144 test is valid only when the 65K-default binary logs `kbase: tiler heap renewal work threshold=262144` before its first Kbase submission.
- Rechecked the separate `experiment/g720-heap-renew-262k` branch at `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`. It is diverged from active source (2 ahead / 75 behind; merge base `e226f2068e3374b63481099390871d390510fece`) and changes the compiled default to 262144 plus its Android workflow. It is not an environment-propagation test and must not be merged or used as proof that Winlator passes the variable.
- Branch-filtered Actions inventory returned 139 runs for the performance branch. Newest active-source runs remain Android-only `43716474968` and wrapper `43716474955`, both created 2026-11-15 12:57:36 UTC for exact source `73873d73`. Android job completed successfully with all 15 returned steps successful; wrapper Release and Debugoptimized jobs each completed successfully with all 12 returned steps successful. No newer active-branch run was present in the returned inventory. No build was triggered for this documentation-only audit.
- The separate timestamp-correction candidate remains `work/kbase-timestamp-capability-fix` at `f5084ae0ebc8a4a8e38ac4007a6ca96daffe96b9`, 8 ahead / 0 behind active source. It changes Kbase timestamp capability reporting and relevant Vulkan feature/property/extension gating, not heap renewal or descriptor allocation. Android run `43900657540` passed all 15 steps; wrapper run `43900657634` passed Release and Debugoptimized (12 steps each). It remains isolated and has no Xiaomi 15T runtime validation; do not mix it into heap A/B measurements.
- Rechecked the available conversation attachment `PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. ZIP integrity and all three embedded SHA256SUMS entries pass; it contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Both ELF files are 64-bit AArch64 for Android API 30; the Vulkan driver RUNPATH is `$ORIGIN`; the private DRM helper is bundled and depends on `libc.so`. Metadata explicitly marks the package experimental/non-conformant and device/game compatibility unverified. The prior exact-source package audit reports 24 DRM imports resolved against 212 helper exports, zero missing. This is the same 65K-default source-73873 package uploaded previously, not a new build.
- No new Tomb Raider benchmark, startup-threshold log, descriptor-profile capture, device stability result, or FPS sample was found. Current conversation files contain only the repeated 73873 package; Library search for Tomb Raider/PanVK benchmark logs found no matching new log, and the recent Library listing contains older VRS diagnostics rather than new Tomb Raider data. Historical measurements remain inconclusive: clean 65K 9.0/17.6/13.5 FPS; one compiled-default 262K run 7.8/18.9/13.9 FPS (not override proof); instrumented 7.7/19.7/13.6 FPS. No FPS improvement is established. Historical telemetry remains 4,068 heap renewals, 52,804.121 ms aggregate renewal wait, zero CSF ring wraps, 1.8 KiB sampled occupancy high-water, zero transient BO allocation/creation failures, and 299 pre-split descriptor failures.
- No PanVK source, allocator policy, CI workflow, Kbase ABI, kernel, ring/heap topology, or Android system/vendor driver was modified. No build was triggered. Keep all experimental packages in Winlator's separate custom-driver import path and retain System Driver as fallback.
- Next device gate: use only the source-73873 65K-default package. Run A with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require startup log value `65536`; restart the game process for B, set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission, and require startup log value `262144`. If the log is absent or still says 65536, the override is unproven. Keep profiling variables unset for matched repeated FPS runs (same Tomb Raider scene, resolution, quality, DXVK 1.7.2, wrapper, and thermal start). In separate processes only, collect Kbase renewal telemetry and `PANVK_DESCRIPTOR_PROFILE=1` category counters; validate the five-category sum against aggregate `set_failures` after quiescence. Keep timestamp-candidate correctness testing separate from this performance comparison.

## Latest authoritative state — 2026-11-23 13:08 Africa/Tunis

This section supersedes the 2026-11-22 16:10 audit summary below. Preserve prior entries as history.

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` (pre-update HEAD `cc7f509d422a0350c8299ba6762a50f1f94a29bd`) and this audit on `experiment/g720-performance-audit` (pre-update HEAD `137c679a57176d145980f9b120359435e5e1d810`). Compare from executable source `73873d730110bf3394ceea19b2a4d45ddba73834` to the audit branch is 29 ahead / 0 behind; the only changed path is this audit Markdown file. The handoff branch is 2 ahead / 0 behind its prior recorded head `1c898cd945462cedd6e4c4a48565ddac530b36a3`, with only `PROJECT_CONTINUITY_HANDOFF.md` changed. Executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`; no driver source or workflow changed on the active performance branch.
- Re-read `panvk_vX_descriptor_set.c` at the active source. The five mutually exclusive failure buckets are present: `overflow`, `no_free_set_only`, `heap_capacity_only`, `both_capacity`, and `vma`. The aggregate `set_failures` remains. Overflow is counted before `set_allocs`/`set_alloc_bytes`; those two fields count post-overflow allocation attempts and requested bytes, including later capacity/VMA failures, not successful sets only. Capacity and overflow paths preserve `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA allocation failure preserves `VK_ERROR_FRAGMENTED_POOL`. The profiling-only extra heap-capacity read remains confined to the failure path. Counters are relaxed atomics and log lines are cumulative snapshots; compare the five-category sum to `set_failures` only at a quiescent final snapshot. The historical 299 failures predate this split and cannot be classified retrospectively.
- Re-read `panvk_vX_gpu_queue.c`: compiled defaults remain interval 128 and work 65,536. `PANVK_KBASE_HEAP_RENEW_INTERVAL` and `PANVK_KBASE_HEAP_RENEW_WORK` are parsed with `debug_get_num_option()`, cached through C11 `call_once`, and the effective work threshold is logged once independently of `PANVK_KBASE_PROFILE`. The 262,144 experiment is valid only when the 65K-default package logs `kbase: tiler heap renewal work threshold=262144` before the first Kbase submission. The interval trigger remains independent; interval/work trigger counters may both increment for one renewal.
- Reconfirmed a separate active-source Vulkan timestamp-capability inconsistency. Active Kbase source sets `gpu_can_query_timestamp=(gpu_id != 0)` while `timestamp_frequency=0`; `panvk_get_gpu_system_timestamp_period()` therefore returns zero, but active PanVK still advertises timestamp query bits/properties and calibrated-timestamp/shader-clock features from the boolean. Keep this correctness issue separate from the heap performance experiment. Candidate `work/kbase-timestamp-capability-fix` at `f5084ae0ebc8a4a8e38ac4007a6ca96daffe96b9` is 8 commits ahead / 0 behind active source. It disables Kbase GPU timestamp querying and gates the relevant features/extensions/properties on a valid nonzero frequency and non-Kbase path. Its Android run [43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540) passed 15/15; wrapper run [43900657634](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657634) passed Release and Debugoptimized, 12/12 each. It remains separate and has no target-device runtime validation.
- Re-downloaded timestamp-candidate artifact 14240094671. Outer SHA-256 `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`; nested flat package SHA-256 `28752b6aa0ec594af9f6334fbe61e4d1211b46b68e89d835784c7d02f1152d318`. Outer and inner ZIP integrity pass; inner package has exactly four expected members and all embedded checksums pass. Both ELFs are AArch64/Android API 30; driver RUNPATH is `$ORIGIN`; private DRM helper needs only `libc.so`; 24 undefined DRM imports resolve against 212 helper exports, zero missing; staged ICD manifest is valid JSON and reports API 1.4.354. Metadata marks it experimental/non-conformant and device/game compatibility unverified.
- Rechecked the newly uploaded active `PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; four expected members, ZIP integrity and all embedded checksums pass; both ELFs are AArch64/Android API 30; driver RUNPATH `$ORIGIN`; bundled helper dependency closure is intact. It is the same 65K-default source-73873 package as previously validated. Do not substitute the older 64c43833 package.
- Active-source exact CI remains Android run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), 15/15, and wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), Release and Debugoptimized 12/12 each, all for source `73873d73`. The latest branch-filtered Actions inventory recorded in the 2026-11-22 16:10 handoff found no newer active-source run. This pass re-fetched the known jobs but the available connector does not expose a branch-wide workflow-run listing, so it cannot independently exclude a run created after that inventory.
- No new Tomb Raider benchmark log or device-side diagnostic was found. Current conversation uploads contain only repeated PanVK driver ZIPs; the Library search for Tomb Raider benchmark logs returned no results, and the recent Library listing only showed older November 6/7 Tomb Raider archives. No 65K/262K effective-threshold proof, new descriptor-profile capture, stability report, or new FPS sample exists. Preserve historical measurements as inconclusive: clean 65K 9.0/17.6/13.5 FPS; one compiled-default 262K 7.8/18.9/13.9 FPS (not environment-override proof); instrumented 7.7/19.7/13.6 FPS. No FPS improvement is established. Historical telemetry remains 4,068 renewals, 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO failures, and 299 pre-split descriptor failures.
- No source edit, allocator-policy change, CI run, or driver build was made in this audit. No ring sizing, heap topology, BO retention, shader dispatch, Kbase ABI, kernel, or Android system/vendor driver was changed.
- Device sequence: first smoke-test the separate timestamp-correction candidate package from artifact 14240094671 through Winlator Mali's custom-driver importer, retaining System Driver as fallback; capture ICD load, physical-device enumeration, logical-device creation, one minimal render/submit, timestamp properties/features, and teardown. Do not mix this correctness check into FPS comparisons. Then use only the 73873 65K-default package for matched uninstrumented heap A/B runs: (A) leave `PANVK_KBASE_HEAP_RENEW_WORK` absent and require startup log 65536; (B) restart and set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission and require startup log 262144. If the value is absent or remains 65536, the override is unproven. Keep all profile variables unset for FPS runs and match scene, resolution, quality, DXVK 1.7.2, wrapper, and thermal conditions; repeat runs. Collect Kbase renewal telemetry and the five descriptor failure buckets only in separate instrumented processes. Never replace the system/vendor driver or modify the kernel.


## Scope and safety

This audit starts from `experiment/clip-cull-distance-v12` at
`f843a225f9b4f1821e69bb52ed0718c86628b8d3`. It is an isolated performance
work branch; it does not change the clip/cull branch, the system/vendor Vulkan
driver, or the active Tomb Raider benchmark configuration.

Target: Android 16, Dimensity 8400 Ultra, Mali-G720 MC7 (Valhall v12), using
the Android kbase CSF path. The driver is experimental and must remain a
separately imported userspace package.

## Findings from the current source

### 1. CSF/kbase submission

File: `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`

The kbase path owns userspace ring buffers (64 KiB per ring), emits ring jobs,
publishes the USER_IO insert offset, and kicks the scheduler. The code already
has opt-in aggregate `PANVK_KBASE_PROFILE` counters for:
- queue submit calls/time/errors;
- subqueue wait calls/time/timeouts/errors;
- status-drain calls/time/nonempty;
- ring jobs, doorbells, kicks and native GPU-wait submits;
- tiler heap renewal count, trigger type, accumulated work and wait/total time.

This is valuable instrumentation, but it currently does not expose per-subqueue
submit latency distributions, ring occupancy/high-water marks, CPU time spent
in ring reservation/clean/publish/kick sub-stages, or the ratio of useful GPU
work to host submission. Do not infer GPU saturation from doorbell or kick
counts alone.

### 2. Command-buffer finalization

File: `src/panfrost/vulkan/csf/panvk_vX_cmd_buffer.c`

Kbase progress breadcrumbs are already gated by `PANVK_DEBUG(KBASE_DIAG)`,
so the diagnostic stores and flushes are absent in normal operation. The
command-buffer finalization path waits for CS slots and performs a cache clean
before command-buffer allocations can be recycled. That clean is a lifetime/
coherency requirement, not an obvious removable performance tax. Any attempt
to elide or narrow it must first prove that recycled descriptor/CS cache lines
cannot be written back stale after CPU reuse.

### 3. Descriptor pools and large heaps

Files:
- `src/panfrost/vulkan/panvk_vX_descriptor_set.c`
- `src/panfrost/vulkan/panvk_vX_descriptor_set_layout.c`
- `src/panfrost/vulkan/panvk_vX_nir_lower_descriptors.c`

A non-host-only descriptor pool currently allocates one backing BO sized from
the application's pool declaration and initializes a `util_vma_heap` over
that BO. The sizing path accumulates descriptor counts and multiplies by
descriptor size in 64-bit arithmetic with overflow checks. Set allocation
reserves ranges from this heap; individual descriptors are written into the
mapped descriptor memory, with dirty min/max tracking.

This is simple and avoids per-set BO allocations, but a game that declares a
very large descriptor pool can request one correspondingly large BO up front.
Potential failures include driver/kernel allocation limits, address-space
fragmentation, host mapping pressure, and a mismatch between declared pool
capacity and actual use. The existing 64-bit arithmetic guards arithmetic
overflow; it cannot make a physically impossible contiguous allocation succeed.

Do not silently cap or under-allocate Vulkan descriptor pools: that would
violate application-visible allocation semantics. A future segmented/chunked
descriptor heap design is a possible engineering project, but it must preserve
the descriptor address model (including shader-visible set bases), alignment,
lifetime, reset/free behavior, host-only pools, and address-binding reports.
First measure actual game pool sizes and allocation failures.

### 4. Transient BO pools

File: `src/panfrost/vulkan/panvk_mempool.c`

The allocator reuses regular and large BOs from command-pool-owned free lists.
It selects a fitting large BO, retains the backing with the greatest remaining
capacity, aligns shared write-back-mapped allocations to the non-coherent atom
size, and can flush/invalidate whole owned BO lists. This is a sensible reuse
baseline. Risks to measure are retained large-BO memory, full-BO cache
maintenance cost, and allocation churn when workload sizes vary.

## Optimisation order

### Phase A — baseline and observability (no behavior changes)

1. Build a known-good Android ARM64 package from this branch.
2. Run the exact same Winlator workload and capture frame-time percentiles,
   FPS, device temperature, and throttling state.
3. Enable `PANVK_KBASE_PROFILE=1` for a diagnostic run and retain the complete
   log. Keep the normal performance run without debug instrumentation.
4. Record descriptor pool create sizes, set allocation/free/reset counts,
   allocated descriptor bytes, BO allocation failures, transient pool BO
   counts/bytes, and per-subqueue ring occupancy. Add counters only if their
   overhead is demonstrably negligible and disabled by default.
5. Capture shader compilation/cache hit behavior and GPU/CPU utilization if the
   Android build exposes reliable counters. Do not substitute CPU utilization
   for GPU busy measurements.

### Phase B — isolate one bottleneck at a time

- **Host-bound:** reduce redundant state/descriptor updates, avoid unnecessary
  queue kicks, and batch command submission only where Vulkan ordering and
  application latency permit.
- **GPU-bound:** inspect generated Valhall ISA, register pressure, spills,
  occupancy, texture/cache behavior, and tiler/fragment balance before changing
  compiler heuristics.
- **Memory-bound:** measure BO mapping and cache maintenance costs, descriptor
  pool high-water marks, VA allocation failures, and memory fragmentation.
- **Synchronization-bound:** use the existing wait/status counters and CSF
  traces to identify waits that actually block. Never remove a barrier, wait,
  cache clean, or ring publication fence without a proof of the required
  memory-ordering and lifetime semantics.

### Phase C — candidate experiments

Each candidate gets its own commit and benchmark record:
- tune kbase tiler heap renewal interval/work threshold only after correlating
  renewal wait time and heap exhaustion against frame time;
- optimize descriptor dirty-range flushing only after confirming the actual
  flush implementation and non-coherent atom alignment;
- investigate reusable/chunked descriptor backing only with measured large-pool
  failures and a correctness design;
- inspect Valhall compiler scheduling/register allocation on captured hot
  shaders; do not force seven compiler workers or seven-way dispatch based on
  the MC7 marketing core count;
- compare command submission batching and ring kick frequency with identical
  scene traces and frame-time percentiles.

## MC7 scheduling facts

Seven shader cores are execution resources managed by the GPU/firmware. They are
not seven Vulkan queues and do not imply that the compiler should spawn seven
threads or that each draw should be split into seven jobs. A single draw can
already be distributed by the hardware scheduler. Host-side over-partitioning
can increase synchronization and scheduling overhead. Tune only based on
measured occupancy and per-core/busy evidence.

## Acceptance gates

A candidate is retained only if:
- Android ARM64 build and package audits pass;
- no Vulkan validation/CTS regression is introduced in the touched area;
- the same workload completes without hangs, black screens, or descriptor
  allocation failures;
- repeated A/B runs show a reproducible improvement in median and tail frame
  times, not merely one FPS peak;
- thermals and clocks are comparable;
- the result is documented and the handoff updated.

## Current status

- Read-only source audit completed.
- No performance source changes have been made yet.
- No new runtime benchmark or device profile has been collected in this step.
- The next concrete code task is low-overhead, opt-in allocator/descriptor
  telemetry, followed by a reproducible baseline. Do not change descriptor
  heap topology, heap renewal thresholds, shader scheduling, or synchronization
  until the counters identify the limiting path.


## 2026-11-05 implementation checkpoint — descriptor telemetry

Commit `82f11124994e7fa614998006fd59b57f9e723101` adds opt-in descriptor
pool telemetry to `panvk_vX_descriptor_set.c`, controlled by
`PANVK_DESCRIPTOR_PROFILE=1`. The default is disabled. When enabled it
aggregates:
- number of non-empty descriptor pools created;
- total requested descriptor backing bytes and largest individual pool;
- pool creation failures;
- descriptor set allocation attempts and requested bytes;
- descriptor set allocation failures, including out-of-pool and fragmented
  pool returns.

Snapshots are emitted every 16 non-empty pool creations and on allocation
failures. The counters use relaxed atomic operations and do not add per-event
logging or timing calls. This is diagnostic instrumentation, not a runtime
performance optimisation; it is intended to reveal whether giant pool
declarations or descriptor-set fragmentation correlate with game failures.

Commit `a0bafee3aeeaf00127fa8d12c838246eddf4db77` adds the isolated branch to
the Android-only build workflow's push filter. The existing Android wrapper
workflow also started on the branch. At checkpoint time, wrapper build run
`41941319560` was still in progress (debugoptimized and release jobs); no
successful compile claim is made until its results are inspected. The Android-
only workflow result must also be checked independently.

No allocator semantics, descriptor heap layout, cache maintenance, queue
synchronization, or shader behavior has been changed.


## 2026-11-05 implementation checkpoint — CSF ring telemetry

Commit `842ac70aa83575b15757b869137ba598824c044e` extends the existing
`PANVK_KBASE_PROFILE=1` counters with:
- total ring bytes written and cache-cleaned;
- ring-wrap event count;
- occupancy sample count;
- aggregate high-water mark of outstanding ring bytes across subqueues.

The occupancy sample reads the firmware's CS_EXTRACT value only when profiling
is enabled, after the new insert offset is published in userspace memory. The
high-water mark uses relaxed atomic compare/exchange. Normal operation has no
new extract-page reads or counters beyond the already-existing profile gate.
This instrumentation is intended to determine whether ring pressure or wrap/
reclaim waits are significant; it does not alter ring capacity, wrap handling,
cache cleaning, barriers, doorbells, or scheduler kicks.

Android-only and wrapper CI runs for this latest source commit
`842ac70aa83575b15757b869137ba598824c044e` have started; compile results are
pending. Earlier CI runs from intermediate commits are also in flight. Runtime
testing is still required to ensure the telemetry output is useful on the
actual Mali-G720 MC7.

## 2026-11-05 validation checkpoint — performance telemetry builds

Latest performance-branch source/document commit:
`b4f8759db3616b4e9a4aa50d1f66b0d1ef3a87bf`.

Both latest-source GitHub Actions workflows completed successfully:
- Android-only PanVK G720 Experiment: run `41941578949`, conclusion SUCCESS.
  Job `build-android-only` passed configure, compile/stage, driver/manifest/
  dependency audit, flat Winlator/Adrenotools package creation, and artifact
  upload. Artifact ID `13377475268`, name
  `panvk-g720-android-only-experimental-package`, size 16,745,729 bytes.
- Android NDK Build (Wrapper Variant): run `41941579141`, conclusion SUCCESS.
  Both `debugoptimized` and `release` jobs passed configure, compile, and
  artifact upload.

This validates compilation and static packaging on the latest branch commit.
It does not establish on-device kbase ABI compatibility, runtime stability,
Vulkan conformance, or any FPS improvement. No runtime test has yet been run.

### Telemetry source review notes

The descriptor profile format string and argument ordering match. Pool byte
counters measure requested backing size; set allocation bytes count requested
bytes before heap allocation, so failed attempts are included. Pool-create
failure counts the backing-memory initialization failure path, not every
possible earlier host allocation failure. The option is disabled by default.

The CSF profile format string and argument ordering match. Ring bytes cleaned
are counted after the existing cache-clean call. The occupancy high-water
counter is the **maximum single sampled outstanding-byte value observed across
all subqueues**, not the sum of simultaneous occupancy across subqueues. It is
a diagnostic peak sample, not an exact global ring-memory usage total.
The extract read is gated by `PANVK_KBASE_PROFILE`; no queue/ring behavior is
changed. Monotonic insert/extract sequence semantics are assumed by the
existing ring code and should be confirmed against runtime logs.

Next: obtain a reproducible device-side run with both profile options enabled
for a diagnostic capture, then repeat the performance benchmark with profiling
disabled. Keep instrumented and uninstrumented measurements separate.

## 2026-11-05 implementation checkpoint — transient BO telemetry

Commit `396c924ccccb07b15b429b5cdc82fd9318374664` adds opt-in counters to
`panvk_mempool.c`, enabled only by `PANVK_MEMPOOL_PROFILE=1` (default off):
- transient pool allocation calls and requested bytes;
- allocation failures;
- reuse counts from regular and large free-BO pools;
- BO creation calls and requested backing bytes;
- BO creation failures and largest requested BO size.

The counters use relaxed atomics, with snapshots every 1024 pool allocation
requests and on failures. This does not alter BO selection, allocation size,
reference ownership, pool reset, cache flush/invalidate, or Vulkan allocation
semantics. It is diagnostic telemetry, not a performance optimization. It does
not yet report retained/free-list bytes or current live BO totals; those require
lifetime-aware accounting and are intentionally not inferred from request
counters.

CI for this new source commit is pending. No runtime test has been run.


## 2026-11-05 CI result — transient BO telemetry commit

The latest source commit `396c924ccccb07b15b429b5cdc82fd9318374664`
is now build-validated:
- Android-only workflow run `41942926968`: SUCCESS. All steps passed,
  including Android ARM64 libdrm, configure, compile/stage, dependency and
  manifest audit, flat package creation, and artifact upload.
  Artifact `13376674548` (`panvk-g720-android-only-experimental-package`),
  16,749,203 bytes.
- Android NDK Wrapper workflow run `41942927008`: SUCCESS for both
  `debugoptimized` and `release`; configure, compile, and upload passed.

The documentation commit `8ed8d5cdaef2be2974459d552a6b838b3d2d3e65`
also passed both workflows: Android-only run `41942958976` and wrapper run
`41942958980` (both wrapper variants successful).

**Interpretation:** the source compiles for Android ARM64 and the experimental
package is generated and statically audited. This is not runtime validation:
there is still no evidence of the PanVK driver loading in Winlator on the
Mali-G720, successful kbase operation, Vulkan conformance, game compatibility,
or FPS improvement.

### Next controlled experiment

First inspect the artifact manifest/audit and compare its shared-library
hashes and metadata against the previous package. Then, only if Winlator
supports forwarding driver environment variables, collect one diagnostic
run with all three options enabled:
- `PANVK_DESCRIPTOR_PROFILE=1`
- `PANVK_KBASE_PROFILE=1`
- `PANVK_MEMPOOL_PROFILE=1`

Capture driver logs and the exact game/workload. Repeat the same workload with
all profile variables disabled for performance measurement. Do not compare
instrumented FPS against baseline and do not change ring capacity, BO reuse
policy, descriptor heap topology, or synchronization until the counters
identify a measured bottleneck. The BO counters currently do not measure
retained/free-list bytes or exact live BO totals.


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


## 2026-11-05 — isolated tiler-heap renewal threshold experiment

### Source review

The current Kbase submit path accumulates `tiler_work_estimate` and triggers
heap renewal when either the graphics-submit count reaches 128 or accumulated
work reaches 65,536 by default. Direct draws contribute
`vertex.count * instance.count`; indirect draws contribute
`draw_count * 256`. This is a CPU-side proxy, not an observation of actual
firmware heap occupancy, allocated chunks, or bytes.

On renewal, PanVK waits for graphics subqueue target sequence numbers,
creates a replacement Kbase tiler heap context, updates and cache-cleans the
heap descriptor, and defers destruction of the old context until both VT and
fragment subqueues have emitted and executed later ring entries. The last
profiled Tomb Raider capture recorded 4,068 renewals (4,066 work-triggered,
2 interval-triggered), 52,804.121 ms aggregate wait, and 61,272.081 ms total
renewal time. These are aggregate counters and must not be interpreted as
wall-clock frame time. The CSF ring had zero wraps and only 1.8 KiB sampled
occupancy high-water; transient BO allocation and BO creation failures were
zero. Therefore the threshold is a defensible next A/B variable, but heap
memory growth and correctness are explicit risks.

### Compile failure diagnosis

Android-only run `41946843838` failed at
`src/panfrost/vulkan/panvk_vX_descriptor_set.c:80` while compiling the
profile log call: missing closing parenthesis. This was subsequently fixed
on the baseline branch at commit
`a2e5ac142b4f0df71cb28204ece4337bace046de`. Current source excerpt was
re-fetched and verified to contain the closing parenthesis and all variadic
arguments within `mesa_logi()`.

### 4x threshold candidate

Created branch `experiment/g720-heap-renew-262k` from
`e226f2068e3374b63481099390871d390510fece`. The candidate changes only
`KBASE_TILER_HEAP_RENEW_WORK` from 65,536 to 262,144; interval remains 128,
and the runtime environment override and effective-value log remain intact.
This is a 4x threshold experiment, not a permanent optimization claim.

Commits:
- `2f992b1a6464dc7a0e623e7f6c7355123ef5d772`: CI push-filter inclusion.
- `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`: 262,144 default.

Queued validation for candidate SHA `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7`:
- Android-only run `41947695997`.
- Android wrapper run `41947696048` (Release and Debugoptimized).
Baseline-source CI at SHA `e226f2068e3374b63481099390871d390510fece` also
remains in progress: Android-only `41947306451`, wrapper `41947306616`.
The jobs were still in host-tool generation at the last detailed step check;
no target compile or package success is yet established.

### Acceptance test

Do not distribute the candidate until Android-only and wrapper builds pass
and the generated flat Winlator package passes manifest, ELF, dependency,
member-hash, and archive-layout audits. Device A/B must use the same Tomb
Raider scene, resolution, graphics settings, wrapper, and thermal starting
state. Disable all profile variables in both runs. Verify the effective
threshold log (65,536 baseline vs 262,144 candidate); capture min/max/average
FPS and frame-time distribution if available; inspect visual artifacts,
crashes, device-lost/CSF faults, heap allocation/OOM behavior, and WSI teardown.
A 4x threshold should nominally reduce work-triggered renewal frequency only
if the heuristic tracks the workload linearly and the heap does not reach its
limit; neither is assumed. Keep the baseline branch unchanged for rollback.


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


## 2026-11-05 — detailed G720 capability audit and benchmark correction

The user corrected the 262,144-work-threshold Tomb Raider result to **min
7.8 / max 18.9 / average 13.9 FPS**. The prior 65,536 baseline is 9.0 /
17.6 / 13.5 FPS. Candidate average delta is +0.4 FPS with lower minimum;
single-run evidence is inconclusive and does not justify promotion.

Added
[`docs/G720_CAPABILITY_MATRIX_2026-11-05.md`](G720_CAPABILITY_MATRIX_2026-11-05.md)
to this branch. It inventories current v12 advertised capabilities and
separates implemented features from partially implemented, compiler-limited,
and Kbase-backend-limited functionality.

Key source-level findings:
- `multiViewport` is false and `maxViewports=1`; later JimVulkan PanVK
  implements it in software via geometry shader viewport remapping and
  fragment scissor discard.
- `shaderClipDistance` and `shaderCullDistance` are false with zero reported
  limits. Current PanVK has partial direct VS->FS NIR lowering, but explicitly
  does not cover fragment shaders reading their own distance arrays or
  geometry/tessellation propagation.
- `pipelineStatisticsQuery` is false; query-pool support has no pipeline
  statistics case and compiler stats-address lowering returns the safe OOB
  sink. JimVulkan implements CPU-side parameter accounting with stated
  approximation for geometry/tessellation.
- `geometryStreams` is false; current NIR lowering maps rasterization stream
  to zero.
- Kbase sparse binding remains disabled because its sparse bind queue is absent.
- v12 vertex-pipeline stores/atomics remain disabled because of the documented
  speculative invalid-index issue. Later JimVulkan code preserves this gate
  until v13.
- `VK_EXT_descriptor_heap` is a potentially relevant D3D12 compatibility
  port, distinct from descriptor indexing which is already exposed. It needs
  generated registry/API support plus driver and compiler paths.
- BCn support is queried from hardware format bits; JimVulkan includes BCn
  emulation for Mali formats without native decode. Query the actual G720
  format properties before considering that port.
- Ray query is not implemented in this branch. The experimental G925 v13
  path is not evidence of a safe G720 v12 port.

The next source task is a dedicated clip/cull completion comparison against
JimVulkan's later implementation. Do not flip feature bits before completing
stage propagation, explicit fragment-input semantics, properties, and tests.
No runtime source change was made in this audit step.


## 2026-11-05 — additional clip/cull stage-transport findings

The follow-up audit found shared NIR helpers and a more nuanced libpoly
transport picture. The complete details and source blob identifiers are in
`docs/G720_CAPABILITY_MATRIX_2026-11-05.md` on the performance branch and in
the continuity handoff. In brief: NIR provides gather/merge/vec4-normalization
passes; software VS/TES and TCS paths transport outputs through varying-slot
masks; passthrough GS has explicit compact clip/cull support; PanVK's active
custom GS lowering is a separate path whose per-slot output collection still
needs verification. Explicit FS reads and stage propagation remain incomplete.
No runtime changes or feature exposure were made. The dedicated source branch
is `experiment/g720-clip-cull-capabilities`, starting at
`2fb0c3c792a193d0bed9d6eafda5cee606546150`.


### Cull semantic and NIR pass-order checks added to the gate

The current direct-path cull emulation uses a per-vertex 0/1 indicator and
fragment discard at `>= 0.9999` to approximate primitive-level culling. The
epsilon can create a narrow false-discard region when only a subset of
vertices has a negative cull distance. This requires reference-based tests;
it must not be generalized to other stage chains without proof.

Also, do not assume the order of NIR's clip/cull merge and vec4-normalization
helpers. Merge rewrites cull locations, while vec4 normalization discovers
sizes from variable locations. A dedicated NIR test matrix must establish the
correct ordering and mixed-array behavior before runtime integration.


### NIR pass-order refinement

RADV in this Mesa tree confirms the standard interface-linking sequence:
`nir_gather_clip_cull_distance_sizes_from_vars()` followed by
`nir_merge_clip_cull_distance_vars()` (`src/amd/vulkan/radv_shader.c`, blob
`c6b392bb638c7755356950130e1521f9106343d7`). Treat
`nir_lower_clip_cull_distance_to_vec4s()` as a distinct representation
lowering pass, not an assumed next step after merge. The PanVK test should
validate which representation is required by its varying ABI.


## 2026-11-05 — first clip/cull NIR tests

On `experiment/g720-clip-cull-capabilities`, added and registered
`src/compiler/nir/tests/clip_cull_distance_tests.cpp`. The initial tests cover
vertex clip-only, cull-only, mixed arrays across a vec4 boundary, and fragment
mixed inputs, checking the metadata and location/location_frac result of the
standard gather-then-merge helper sequence. A dedicated host CI workflow builds
and runs the NIR suite; Android PanVK build workflows are also enabled on this
branch. The latest workflow results are pending. These tests validate the
shared NIR merge helper only; they do not prove PanVK GS/TCS/TES transport or
hardware discard semantics. No runtime feature exposure or driver code has
changed.


## 2026-11-05 — initial clip/cull NIR tests

Added `src/compiler/nir/tests/clip_cull_distance_tests.cpp` on `experiment/g720-clip-cull-capabilities`. Cases cover vertex clip-only, cull-only, mixed arrays, a mixed array crossing a vec4 slot boundary, and fragment mixed inputs. They check gather metadata and merged varying locations/components. A dedicated host CI workflow builds and runs the NIR suite; Android build workflows are enabled on this branch. Results are pending. These tests do not yet cover vec4 lowering, GS/TCS/TES transport, interpolation, or PanVK discard semantics. No runtime code or feature bits changed.


CI note: the first NIR workflow reached Meson configuration successfully but failed at the build step because the requested Ninja target path was invalid (`src/compiler/nir/nir_tests`). The workflow was corrected to target `nir_tests`; a rerun is queued. This is not a compiler/test failure, and no successful test run is claimed yet.


Host NIR CI was refined after discovering that disabling all Vulkan drivers caused Meson to select `with_nir_headers_only`, omitting the real NIR implementation and test target. The workflow now enables `vulkan-drivers=panfrost` and `panfrost-kmds=kbase` so the full NIR library/tests are built. Rerun pending; no successful result is claimed.


### Vec4 lowering / merge ordering hazard (source audit)

The generic NIR helper `nir_lower_clip_cull_distance_to_vec4s()` discovers
clip and cull sizes from variables at the original CLIP_DIST0 and CULL_DIST0
locations. The separate merge helper moves cull IO into the CLIP_DIST location
range and marks declarations hidden. Therefore, blindly running vec4
normalization after merge can lose the original cull-array size/offset.
Existing tests currently cover gather+merge only, not this interaction.
Treat helper ordering as unresolved until focused NIR tests cover mixed
arrays, dereference loads/stores, indirect indexing, and arrayed GS/TCS/TES
interfaces. No PanVK feature bits are enabled.


### Host NIR CI — LLVM dependency correction

The corrected workflow shell invocation reached Meson, which reported
`Feature llvm cannot be disabled: CLC requires LLVM`. This is a CI dependency
configuration failure before compilation. The workflow now installs
`llvm-dev` and `clang` and configures `-Dllvm=enabled`
(commit `21247310892a4be2209d13a586db21e42a95e2b7`). Await the next run;
do not classify this as a NIR test failure or claim tests pass.


### NIR CI follow-up: libclc

After enabling LLVM, Meson configuration advanced but stopped because
`libclc` was not installed. Added Ubuntu package `libclc-dev` to the host
workflow (commit `43bc2ca59b3f5adbc804161f662e9d55617dd5fe`). This is still
a host CI dependency issue before compilation; no NIR test result exists yet.


### Initial vec4 representation regression tests

Added three tests to `src/compiler/nir/tests/clip_cull_distance_tests.cpp`
for `nir_lower_clip_cull_distance_to_vec4s()`: mixed clip=3/cull=2,
cull-only size 5, and mixed clip=5/cull=3. They assert the packed
`gl_ClipDistanceMESA` declaration is an array of vec4s with the expected
two-slot length. This is declaration-level coverage only; it does not yet
test dereference rewrites, indirect indexing, or arrayed stage interfaces.
The tests are separate from gather/merge and do not prove pass order.
Commit `cec14bb825176cb714e0a6d2e4dab3c7dd44de0e`.


## 2026-11-06 continuation — exact-head build/package audit and environment-override correction

### Current performance-audit branch source

- CI/package snapshot HEAD: `fb70e9b5cf8b41806da4bc363c4f29d78caeaac8` (documentation/capability-matrix update); this audit refresh was committed as `3aeb3e8a441b346d231f5f25198956f542271c1f`.
- Latest code commit on this branch: `a2e5ac142b4f0df71cb28204ece4337bace046de` (`panvk: fix descriptor profile log call`). The descriptor-failure classification and effective heap-threshold log are present in the current tree.
- Source review of `panvk_vX_descriptor_set.c` confirms the set-allocation failure returns are classified as:
  - `overflow`: variable-count adjustment overflow and final descriptor-count overflow; returns remain `VK_ERROR_OUT_OF_POOL_MEMORY`.
  - `capacity`: no free descriptor-set slot or insufficient descriptor-heap free bytes; returns remain `VK_ERROR_OUT_OF_POOL_MEMORY`.
  - `vma`: `util_vma_heap_alloc()` returns zero; return remains `VK_ERROR_FRAGMENTED_POOL`.
  - The aggregate failure counter is retained and incremented with each category. Profiling is gated by `PANVK_DESCRIPTOR_PROFILE=1`; no allocation semantics or Vulkan return codes changed.
- Source review of `panvk_vX_gpu_queue.c` confirms the threshold helper calls `debug_get_num_option("PANVK_KBASE_HEAP_RENEW_WORK", KBASE_TILER_HEAP_RENEW_WORK)`, accepts positive values, maps non-positive values to zero, and logs the effective value once. On this branch the compiled default is still 65,536.

### Important correction: the earlier 262K run was not an environment-variable test

Commit `8d6ec93a0d3435d2a08c8a62eb478cb9a207d2f7` on `experiment/g720-heap-renew-262k` changes only the compiled default macro from 65,536 to 262,144 (plus a diagnostic comment); the environment-variable parser remains present. Therefore the 02:42 startup log showing 262,144 proves the candidate binary's effective threshold, but **does not prove Winlator passed `PANVK_KBASE_HEAP_RENEW_WORK`**. This distinction supersedes any earlier wording that described that run as an override-propagation test.

Documented controlled datapoints remain:
- Clean 65,536 baseline: **9.0 min / 17.6 max / 13.5 average FPS** (02:33 capture; log recorded effective 65,536 and profiling disabled).
- 262,144 compiled-default candidate: **7.8 min / 18.9 max / 13.9 average FPS** (02:42 result corrected by the user; log recorded effective 262,144).
- Candidate delta: +0.4 average, -1.2 minimum, +1.3 maximum FPS. One run per variant is inconclusive; no gain or promotion is justified.
- The earlier **7.7 / 19.7 / 13.6 FPS** capture had diagnostic profiling enabled and is not a clean comparison.
- No new Tomb Raider archive was supplied in this update; current conversation attachments contain the earlier 02:03:50 archive only.

### Exact-head performance-audit CI and independently verified 65K package

- Android-only run `42082282882`, job `127438170627`: SUCCESS through host-tool generation, Android ARM64 libdrm build, Android-only configure/compile/stage, driver/manifest/dependency audit, flat package creation, and artifact upload.
- Wrapper run `42082283389`: SUCCESS for Release job `127438172351` and Debugoptimized job `127438172901`.
- Workflow artifact `13442371778`, outer SHA-256 `3fc3eef74d546c11a1f1e799b38e4135fc935d25286c8b013cdfde5a897def6c`.
- Independently downloaded inner package `PanVK-G720-experimental.zip`: SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`; ZIP integrity test passed; exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three member hashes pass.
- Metadata version: `26.3.0-devel-android-kbase-fb70e9b5cf8b`, min API 30. Both libraries are ELF64 AArch64/Android API 30. Driver RUNPATH is `$ORIGIN`, its private dependency is `libpanvk_drm.so`, bundled libdrm depends only on libc, and the workflow's missing-DRM-symbol report is empty.
- This is the correct package for an environment-override test because its compiled threshold remains 65,536. It is not a runtime-validated release.

### Parallel branch status (not merged into this branch)

- Descriptor heaps: `experiment/g720-descriptor-heaps` at `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`; Android-only run `42082198442` and wrapper run `42082198906` passed. Artifact `13441682356` outer SHA-256 `0a4f6b7899f3df42a89133f071b2c441e62739e8000ad55bdf14af7329fccf71`; inner package SHA-256 `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6`. Package integrity, member hashes, AArch64/API 30, RUNPATH, private libdrm, and empty missing-symbol report verified. No device test.
- Clip/cull: `experiment/g720-clip-cull-capabilities` at `9188e7bf92a032431ca16ac02d54d81be5a5e12e`; NIR tests run `42033527839`, Android-only run `42033527656`, and wrapper run `42033527658` passed. This does not prove all legal stage chains or runtime semantics; feature bits remain disabled.
- VRS: `experiment/g720-fragment-shading-rate` at workflow-fix HEAD `157418c71475b9d96170a43f24157a80673d4160`, driver source `715cbc888cc0ccf12917b2aa38aa34f35ac39c3a`. Android-only run `42140194412`, wrapper/probe run `42140194467` (Release, Debugoptimized, Windows x64 probe) passed. Android artifact `13468166958` outer SHA-256 `a7077179b08ae616abed1334ccb580583b89e0b79993206108df50ec793e60ca`; inner package SHA-256 `fa83005808ca2f7207008a9b2a5139f82f36de269c95c9da97784fc9d1350cbe`. Probe artifact `13467578263`, SHA-256 `618ca16d798d97ecc0ba3c316f1fbbca831578ac271adbc41790bd0bd761306f`; it is a PE32+ x86-64 capability/command-recording probe, not a rendering benchmark. Both package and probe ZIPs passed integrity checks; inner package hashes passed. No device runtime test.
- Keep these experiments isolated. In particular, do not use the VRS or descriptor-heap package for the heap-threshold A/B test.

### Exact next device test

Use the verified 65K-default package from workflow artifact `13442371778`. It has been copied to the working delivery file `PanVK-G720-65k-env-override-test.zip`, SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`.

1. Run the same Tomb Raider scene/settings with `PANVK_KBASE_HEAP_RENEW_WORK` absent; restart the game/driver process and require the startup log `tiler heap renewal work threshold=65536`.
2. Run again with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144`; restart the process and require `tiler heap renewal work threshold=262144`. On this 65K-default binary, that value proves Winlator passed the override.
3. If the second log says 65,536 or is absent, stop and report that environment propagation was not demonstrated. Do not treat its FPS as the 262K result.
4. Leave `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset in both performance runs. Keep DXVK 1.7.2, wrapper, resolution, quality, scene, and thermal conditions matched. Record min/max/average and visual/stability observations; ideally repeat each value.

No driver source was changed in this continuation. No FPS improvement is claimed.

## 2026-11-06 12:25 UTC — exact-head CI and artifact monitoring

Active branch HEAD: `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. The HEAD commit is documentation-only; latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.

### Exact-head runs

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`: completed SUCCESS. Checkout, verification, Python/dependencies, host tools, real Android ARM64 libdrm, Android-only configure, compile/stage, driver/manifest/dependency audit, flat package, and artifact upload all passed. No pending/failed steps.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167):
  - Release job `127885224388`: completed SUCCESS; checkout, memfd_create patch, Python/dependencies, host tools, Meson configure, compile, and artifact upload passed.
  - Debugoptimized job `127885224836`: completed SUCCESS; the same substantive steps passed.
- No failed job exists on the latest exact HEAD; there is no actionable CI failure to diagnose.

### Artifact verification

Android-only artifact `13501237590`:
- Outer ZIP: 16,753,940 bytes; SHA-256 `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`; ZIP test passed.
- Inner `PanVK-G720-experimental.zip`: SHA-256 `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; ZIP test passed.
- Exactly four flat-root files: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. Member hashes pass: driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`, private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`, metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`.
- Metadata version `26.3.0-devel-android-kbase-b17b98cef92f`, minimum API 30. Both libraries are ELF64 AArch64/Android API 30. Driver RUNPATH `$ORIGIN`, NEEDED private `libpanvk_drm.so`; private libdrm needs only `libc.so`. The workflow's missing-DRM-symbol report is empty. Staged ICD manifest reports API version `1.4.354`.
- Classification: **static/package validated**, not device/runtime validated or Vulkan conformant.

Wrapper artifacts from run `42215758167`:
- Release artifact `13502135385`: outer SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; extracted library SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Debugoptimized artifact `13502061037`: outer SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; extracted library SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both ZIPs contain only `libvulkan_panfrost.so`; both extracted files are ELF64 AArch64/Android API 30. RUNPATH is `$ORIGIN/../../android_stub`; dependencies include `libdrm.so` and X11/XCB libraries. The artifacts do not bundle a manifest, checksum file, `android_stub/libdrm.so`, or X11 dependency closure.
- Classification: **build passed / raw ELF checked**, not independently package-validated or proven self-contained. External dependency resolution must be validated in the intended wrapper runtime.

No source or workflow changes were made. This monitoring addendum is documentation-only; commit message uses `[skip ci]` to avoid triggering another build. Keep the Android-only package separate in Winlator; never replace the phone's system/vendor Vulkan driver or modify the live kernel.


## 2026-11-07 12:15 UTC — scheduled CI and artifact revalidation

At the start of this monitoring pass, branch `experiment/g720-performance-audit`
was at documentation-only HEAD `d9394de49dc560fe2212a92b71280c372e66b0e6`;
its inspected diff only appends the prior monitoring record. The latest
driver-source commit remains
`a2e5ac142b4f0df71cb28204ece4337bace046de`. No build was triggered by this
monitoring pass.

The branch-filtered Actions listing contained 125 runs. The newest relevant
runs remain:

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218),
  job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`:
  completed SUCCESS. Every setup, checkout, dependency, host-tool, Android
  ARM64 libdrm, configure, compile/stage, manifest/dependency audit, package,
  and upload step passed. Pending 0; failed 0.
- Wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167),
  same source:
  - Release job `127885224388`: completed SUCCESS; all steps including
    memfd_create patch, dependency/host-tool setup, Meson configure, compile,
    and artifact upload passed. Pending 0; failed 0.
  - Debugoptimized job `127885224836`: completed SUCCESS; same step set
    passed. Pending 0; failed 0.

No newer run supersedes this pair; no latest-run failure exists to diagnose.

### Revalidated artifacts

- Android-only artifact `13501237590`: outer ZIP SHA-256
  `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`
  (matches GitHub digest; ZIP test passes). Embedded flat package SHA-256
  `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`
  (ZIP test passes). Four members: `meta.json`,
  `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`; every
  member checksum recomputed successfully. Metadata version
  `26.3.0-devel-android-kbase-b17b98cef92f`, min API 30. ELF64 AArch64/API 30;
  driver RUNPATH `$ORIGIN`, private NEEDED `libpanvk_drm.so`; private libdrm
  needs only `libc.so`; missing-DRM-symbol report empty. The outer artifact
  also contains the staged ICD JSON (API 1.4.354). Classification:
  **static/package validated, not runtime validated**.
- Wrapper Release artifact `13502135385`: outer SHA-256
  `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`;
  extracted ELF SHA-256
  `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Wrapper Debugoptimized artifact `13502061037`: outer SHA-256
  `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`;
  extracted ELF SHA-256
  `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both wrapper ZIPs contain only the ELF, which is AArch64 Android API 30.
  RUNPATH is `$ORIGIN/../../android_stub`; NEEDED includes `libdrm.so`
  and X11/XCB libraries. No manifest, checksums, android_stub libdrm, or
  X11/XCB dependency closure is bundled. Classification:
  **build passed / raw ELF hash and ABI checked; complete package/runtime
  dependency closure not validated**.

The next concrete gate remains a separate Winlator Mali import/runtime test of
the Android-only flat package. Do not treat the wrapper ZIPs as complete
packages until their dependency layout is proven. No system/vendor driver or
live kernel modification is permitted.


### Documentation commit produced by this monitoring pass

The initial 2026-11-07 monitoring record was committed as
`960815737d61ad45af5b22fdc4a97b5f16f94ba9`. Subsequent corrections in this
monitoring pass are also documentation-only and use `[skip ci]`. No source
code or workflow file changed and no new build was triggered; the latest
CI-tested code snapshot remains
`b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`.


## 2026-11-07 13:33 UTC — performance-audit revalidation

This pass re-read the canonical handoff and this audit before checking the live
branch, source, Actions jobs, package file, and available Tomb Raider captures.

### Branch and source state

- Performance-audit branch HEAD: `5ebf10a4792126a0cb85ec78e380e50dcc063794`
  (`docs: clarify PanVK monitoring snapshot chronology [skip ci]`).
- Latest driver-source commit remains
  `a2e5ac142b4f0df71cb28204ece4337bace046de`
  (`panvk: fix descriptor profile log call`).
- GitHub compare from the latest source commit to branch HEAD reports 49
  documentation commits ahead and zero behind. Its only changed paths are
  `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this performance audit;
  no driver source or workflow file has changed since the source commit.
- Re-read of `panvk_vX_descriptor_set.c` confirms the opt-in failure split:
  overflow (variable-count arithmetic/final uint32 bound),
  capacity (no free set slot or insufficient descriptor heap free bytes),
  and VMA (descriptor VMA allocation returns zero). The aggregate failure
  counter is retained. The original Vulkan return codes and allocation policy
  are unchanged.
- Re-read of `panvk_vX_gpu_queue.c` confirms the effective threshold is
  obtained through `debug_get_num_option("PANVK_KBASE_HEAP_RENEW_WORK", 65536)`
  and logged once. The log reports the effective value, not whether it came
  from the environment or the compiled default. The branch's 65,536-default
  package is therefore still required to prove environment propagation.

### CI revalidation

No newer relevant workflow run appeared after the exact-head 2026-11-06 pair:

- Android-only run
  [42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218),
  job `127885224447`: completed SUCCESS. Checkout, dependencies, host tools,
  Android ARM64 libdrm, configure, compile/stage, ELF/dependency audit,
  flat package creation, and artifact upload all completed successfully.
- Wrapper run
  [42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167):
  Release job `127885224388` and Debugoptimized job `127885224836` both
  completed SUCCESS; all steps completed successfully.
- These runs test source snapshot
  `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. Later branch commits are
  documentation-only. No build was triggered in this revalidation.

### Local delivery-file revalidation

The current delivery file
`PanVK-G720-65k-env-override-test.zip` was independently checked in the
working container:

- SHA-256:
  `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`.
- ZIP integrity passes; it contains exactly `meta.json`,
  `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- All three embedded SHA256SUMS entries pass.
- Metadata version is
  `26.3.0-devel-android-kbase-fb70e9b5cf8b`, minimum Android API 30.
- Driver and private libdrm are ELF64 AArch64. Driver RUNPATH is
  `$ORIGIN`, NEEDED includes private `libpanvk_drm.so`; private libdrm
  depends only on `libc.so`.

This confirms archive integrity and static packaging only; it does not prove
Winlator import, environment-variable propagation, device runtime stability,
Vulkan conformance, or game performance.

### Benchmark and next gate

No new Tomb Raider benchmark log or FPS result was supplied in this pass.
The only local Tomb Raider archive remains
`TombRaider_logs_2026-11-05_02-03-50.zip`; no new measurement supersedes the
documented clean 65,536 result (9.0 / 17.6 / 13.5 FPS) or the single
262,144-compiled-default candidate (7.8 / 18.9 / 13.9 FPS). The latter is
not an environment-variable propagation test and remains inconclusive.

The next device action is unchanged: use only the verified 65K-default package,
run matched Tomb Raider tests with the variable absent and then set to exactly
`PANVK_KBASE_HEAP_RENEW_WORK=262144`, restarting the process each time.
Require startup logs of 65,536 and 262,144 respectively. Leave all profiling
variables unset in both FPS runs. If the override log remains 65,536 or is
missing, stop and classify the override test as invalid.

No source, allocator behavior, queue sizing, shader scheduling, Kbase ABI,
kernel, or Android system/vendor driver was changed in this pass. No FPS gain
is claimed.


## 2026-11-08 12:08 UTC — scheduled Android build and package revalidation

### Branch/source provenance

- Active branch at start of this pass: `experiment/g720-performance-audit`, HEAD `a8a6e5b904de570dc9984d45b5b0890c7ad98a0d` (`docs: revalidate PanVK performance audit state [skip ci]`).
- Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- Latest CI-tested snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`; the latest branch commits after that snapshot are documentation-only. GitHub compare from the driver-source commit to the branch HEAD reports 50 commits ahead / 0 behind and only documentation paths (`docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this audit) in the aggregate changed-file list. No source or workflow change requiring a rebuild was found.
- The canonical handoff and this audit were read before inspecting Actions.

### Latest relevant Actions runs

The branch-filtered Actions listing contains 125 runs. The latest relevant pair remains the 2026-11-06 runs below; the older initial monitoring IDs `41947103034` and `41947103085` are superseded.

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`, job `127885224447`: completed SUCCESS. All 15 returned steps completed successfully: setup, checkout, checkout verification, Python, dependencies, host tools, Android ARM64 libdrm, Android-only configure, compile/stage, driver/manifest/dependency/staging audit, flat package creation, artifact upload, post-Python, post-checkout, and job completion. Pending 0; failed 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source:
  - Release job `127885224388`: completed SUCCESS; all 12 returned steps passed, including memfd_create patch, dependencies, host tools, Meson configure, compile, artifact upload, cleanup, and completion. Pending 0; failed 0.
  - Debugoptimized job `127885224836`: completed SUCCESS; same 12 returned steps passed. Pending 0; failed 0.
- No latest relevant job failed or remains pending, so no failure log diagnosis was needed. No workflow was rerun and no build was triggered during this monitoring pass.

### Fresh artifact download and validation

Android-only artifact `13501237590` (16,753,940 bytes):
- GitHub artifact digest and independently computed outer ZIP SHA-256 both equal `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`; outer ZIP integrity passes.
- The outer archive contains the staged install, audit records, and nested `PanVK-G720-experimental.zip`.
- Nested flat package SHA-256: `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; nested ZIP integrity passes.
- Exactly four nested members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. `sha256sum -c SHA256SUMS` passes for all members:
  - driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`
  - private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
  - metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`
- Metadata JSON validates: package version `26.3.0-devel-android-kbase-b17b98cef92f`, min API 30, library `libvulkan_panfrost.so`.
- Both libraries are ELF64 AArch64 / Android API 30. Driver RUNPATH is `$ORIGIN`; its private NEEDED entry is `libpanvk_drm.so`. The bundled private libdrm depends only on `libc.so`. The workflow audit's `missing-drm-symbols.txt` is zero bytes.
- The staged ICD manifest parses as valid JSON and reports Vulkan API `1.4.354`, library path `/usr/local/lib/libvulkan_panfrost.so`. The importable nested package itself is the flat four-file package described above; the absolute staged manifest path is not a path to copy into Android's system directories.
- Classification: **passed and statically/package validated**. This is not a device-runtime, Vulkan-conformance, WSI, or game-compatibility result.

Wrapper Release artifact `13502135385`:
- Outer ZIP SHA-256 matches GitHub digest: `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; ZIP integrity passes.
- Contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`; ELF64 AArch64 / Android API 30.
- RUNPATH `$ORIGIN/../../android_stub`; NEEDED includes `libdrm.so` and X11/XCB libraries.

Wrapper Debugoptimized artifact `13502061037`:
- Outer ZIP SHA-256 matches GitHub digest: `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; ZIP integrity passes.
- Contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`; ELF64 AArch64 / Android API 30 with debug information.
- RUNPATH and NEEDED dependency set match Release.

Neither wrapper ZIP contains `android_stub/libdrm.so`, the X11/XCB dependency closure, an ICD manifest, or a checksum manifest. Classification for both: **build passed / raw ELF and ABI checked, but standalone package and runtime dependency closure not validated**. Do not present these raw ZIPs as complete importable packages without proving the intended wrapper runtime supplies those dependencies.

### Blocker and safe next step

- No new device logs or runtime test were available in this monitoring pass. The Android-only package is ready for a controlled separate Winlator Mali custom-driver import test, but runtime loading, Vulkan device creation, stability, and game performance remain unverified.
- Wrapper packaging/dependency closure remains an open packaging issue.
- Preserve the non-rooted Android/Termux workflow. Do not replace the phone's system/vendor Vulkan driver or modify the live kernel.
- This record is documentation-only; no source or workflow files were changed and no build was triggered.


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


## 2026-11-08 12:42 UTC — performance-audit branch revalidation

### Branch and source provenance

- Performance-audit branch HEAD before this documentation update: `6d1ed6db4c0ef050a516ce47de2c1991a30b2e90` (`docs: clarify handoff push CI side effect [skip ci]`).
- Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de` (`panvk: fix descriptor profile log call`).
- GitHub compare from the source commit to the branch HEAD reports 52 commits ahead, 0 behind. The aggregate changed-file list contains only `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this audit; no driver source or workflow file changed after the source snapshot.
- The descriptor source was re-read at this branch head. Failure classification was introduced in `788d34b622ce6cf51aa410c6366a06b86a6a4988`, its format string was corrected in `751fddd64efc201c103ffa496b29608d315a55e6`, and the final profile log-call correction is `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- The three opt-in descriptor-set failure classes are present:
  - overflow: variable descriptor-count arithmetic overflow or final count exceeding the 32-bit bound;
  - capacity: no free descriptor-set slot or insufficient descriptor-heap free bytes;
  - VMA: `util_vma_heap_alloc()` returns zero.
  The aggregate counter is retained. The original Vulkan return values and allocator policy are unchanged. The counters are only updated when `PANVK_DESCRIPTOR_PROFILE=1`; the old benchmark log predates this split and cannot be retrospectively classified.
- The Kbase helper still obtains the threshold through `debug_get_num_option("PANVK_KBASE_HEAP_RENEW_WORK", KBASE_TILER_HEAP_RENEW_WORK)`, with compiled default 65,536, and logs the effective value once. The source change is `52afaaf880fdf3a8cb4e6e0c5cd6598ec482d3ee`. The log does not identify whether the value came from the environment or compiled default.

### CI status

The latest relevant CI runs for the performance-audit source snapshot remain:

- Android-only run [42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: SUCCESS. All 15 steps completed successfully, including Android ARM64 libdrm, configure, compile/stage, dependency audit, package, and artifact upload.
- Wrapper run [42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source: SUCCESS. Release job `127885224388` and Debugoptimized job `127885224836` both completed all 12 steps successfully.
- There is no newer CI run on `experiment/g720-performance-audit`. Its newer commits are documentation-only, so these source builds remain applicable to the unchanged driver code, but are not builds of the documentation HEAD.

The handoff branch's previous non-`[skip ci]` documentation pushes also queued wrapper-only runs [42481490094](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42481490094) and [42481461421](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42481461421). At this check both were still compiling Release and Debugoptimized wrapper variants. They target `docs/project-continuity-handoff-2026-11-03`, not the performance-audit source branch and not the Android-only PanVK workflow. The earlier documentation-triggered wrapper run `42479110079` completed SUCCESS. Future handoff commits must include `[skip ci]`.

### Newly surfaced files and benchmark evidence

- The newly surfaced `PanVK-G720-65k-env-override-test.zip` is byte-for-byte the same verified package already recorded in this audit: SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`. ZIP integrity and all three embedded SHA256SUMS entries pass again. Metadata is `26.3.0-devel-android-kbase-fb70e9b5cf8b`, minimum API 30. The driver is ELF64 AArch64 with RUNPATH `$ORIGIN` and private NEEDED `libpanvk_drm.so`; the companion is ELF64 AArch64 with SONAME `libpanvk_drm.so` and only `libc.so` as a dependency. This is static/package validation only, not Winlator runtime validation.
- No new Tomb Raider benchmark archive was supplied. The available archive remains `TombRaider_logs_2026-11-05_02-03-50.zip`, SHA-256 `6f54b17e422795673411dfc3106a184922ea283024801f1e257ed5dbd9f68a56`, with four logs dated 2026-11-05 02:03.
- Re-reading that archive confirms the diagnostic capture's final Kbase counters: 4,068 heap renewals (4,066 work-triggered), 52,804.121 ms aggregate renewal wait, 61,272.081 ms total renewal time, 0 ring wraps, 1.8 KiB sampled occupancy high-water, 0 transient BO allocation failures, 0 backing BO creation failures, and 299 descriptor-set allocation failures. The descriptor line contains only the aggregate count because this capture predates the failure-category logging. No `tiler heap renewal work threshold=...` line is present, so this archive does not prove environment-variable propagation.
- The `7.7 / 19.7 / 13.6 FPS` result remains an instrumented diagnostic capture, not a clean performance baseline. The clean 65,536 run remains `9.0 / 17.6 / 13.5 FPS`; the separate 262,144-compiled-default candidate remains `7.8 / 18.9 / 13.9 FPS`. The single-run +0.4 average delta and 1.2 FPS lower minimum are inconclusive. No FPS improvement is claimed.

### Next controlled device test

Use the verified 65,536-default package only, imported as a separate Winlator Mali custom driver:

1. Run the same Tomb Raider scene/settings with `PANVK_KBASE_HEAP_RENEW_WORK` absent; restart the game/driver process and require the startup log `tiler heap renewal work threshold=65536`.
2. Repeat with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144`; restart the process and require `tiler heap renewal work threshold=262144`. Because the package's compiled default is 65,536, this second log is the evidence that Winlator passed the override.
3. If the second run logs 65,536 or the log is absent, stop and mark the override test invalid.
4. Leave `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset in both performance runs. Keep DXVK 1.7.2, wrapper, resolution, quality, scene, and thermal starting conditions matched.
5. Record min/max/average, frame pacing, visual correctness, hangs/device loss, and teardown. Prefer repeated A/B samples. If descriptor failure causes are still needed, collect a separate profiling-enabled diagnostic run after the uninstrumented A/B comparison.

No source, allocator policy, heap topology, ring size, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed in this revalidation. Do not make further heap-policy changes until the override test is complete.


## 2026-11-09 12:42 UTC — scheduled Android build and artifact revalidation

### Branch and source provenance

- Active source branch: `experiment/g720-performance-audit`, HEAD `de47624438c6951437855dfd66c65b526fdf0b51` (`docs: revalidate heap-threshold experiment and benchmark evidence [skip ci]`).
- Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`. GitHub compare from that source commit to branch HEAD reports 53 commits ahead / 0 behind; the only changed paths are `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this audit. No driver source or workflow file changed after the latest CI-tested source snapshot.
- Latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. Later branch commits are documentation-only.

### Latest relevant Actions runs

The branch-filtered Actions listing returned 125 runs. No newer Android-only or Android-wrapper workflow run exists on this branch than the 2026-11-06 pair:

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: completed SUCCESS. All 15 returned steps completed successfully: setup, checkout, checkout verification, Python, dependencies, host tools, Android ARM64 libdrm, Android-only configure, compile/stage, driver/manifest/dependency/staging audit, flat package creation, artifact upload, post-Python, post-checkout, and job completion. Pending 0; failed 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source:
  - Release job `127885224388`: completed SUCCESS; all 12 returned steps passed, including checkout, memfd_create compatibility patch, Python/dependencies, host tools, Meson configure, compilation, artifact upload, cleanup, and job completion. Pending 0; failed 0.
  - Debugoptimized job `127885224836`: completed SUCCESS; all 12 returned steps passed. Pending 0; failed 0.
- The immediately preceding duplicate runs for the earlier documentation snapshot `3aeb3e8a441b346d231f5f25198956f542271c1f` also completed successfully: Android-only run `42215702964`, job `127885035658`; wrapper run `42215702908`, Release job `127885036865`, Debugoptimized job `127885037206`. They are superseded by the later `b17b98c` pair.
- No relevant job failed or remains pending, so no failure log diagnosis was required. No workflow was rerun and no build was triggered in this monitoring pass.

### Fresh artifact download and independent validation

Android-only artifact `13501237590` (16,753,940 bytes):
- GitHub artifact digest and independently recomputed outer ZIP SHA-256 both equal `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`; ZIP integrity passes.
- The outer archive contains the staged Android install, ICD manifest, audit records, and nested `PanVK-G720-experimental.zip`. The staged tree includes `stage/usr/android_stub/libdrm.so`, `stage/usr/local/lib/libvulkan_panfrost.so`, and `stage/usr/local/share/vulkan/icd.d/panfrost_icd.aarch64.json`.
- Nested flat package SHA-256: `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; nested ZIP integrity passes.
- Exactly four nested members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three embedded checksums pass:
  - driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`
  - private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`
  - metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`
- Metadata JSON is valid: package version `26.3.0-devel-android-kbase-b17b98cef92f`, minimum API 30, library `libvulkan_panfrost.so`. It explicitly describes the package as experimental and not Vulkan conformant.
- Both nested libraries are ELF64 AArch64 / Android API 30. Driver SONAME is `libvulkan_panfrost.so`, RUNPATH is `$ORIGIN`, and NEEDED includes the bundled private `libpanvk_drm.so`. That private library's only NEEDED entry is Android `libc.so`.
- The staged ICD manifest parses as valid JSON and reports API version `1.4.354`, architecture 64, library path `/usr/local/lib/libvulkan_panfrost.so`. The audit's `missing-drm-symbols.txt` is empty.
- Classification: **build passed / outer artifact and inner flat package statically validated**. This does not establish Winlator import, Kbase runtime compatibility, Vulkan conformance, WSI stability, or game performance.

Wrapper Release artifact `13502135385`:
- Outer ZIP SHA-256 matches GitHub digest: `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; ZIP integrity passes.
- Contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`; ELF64 AArch64 / Android API 30.
- RUNPATH is `$ORIGIN/../../android_stub`. NEEDED includes `libdrm.so`, `libxcb.so.1`, `libX11-xcb.so.1`, other XCB libraries, and `libxshmfence.so.1`.

Wrapper Debugoptimized artifact `13502061037`:
- Outer ZIP SHA-256 matches GitHub digest: `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; ZIP integrity passes.
- Contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`; ELF64 AArch64 / Android API 30 with debug information.
- RUNPATH and NEEDED dependency set match Release.

Neither wrapper ZIP bundles `android_stub/libdrm.so`, the X11/XCB dependency closure, an ICD manifest, or a checksum manifest. Classification for both: **build passed / raw ELF and ABI verified; complete package and runtime dependency closure not validated**. Do not describe these wrapper ZIPs as standalone Winlator import packages unless the intended runtime's companion dependency layout is independently demonstrated.

### Blockers and safety

- No newer active-branch build exists; all latest source build jobs remain passed.
- The Android-only package is structurally validated but still needs a controlled separate Winlator Mali custom-driver import and device-runtime log. Runtime loading, Vulkan device creation, stability, and game performance are not verified by CI.
- Wrapper packaging/dependency closure remains unresolved.
- No source, workflow, allocator policy, Kbase ABI, kernel, or Android system/vendor driver was changed. This record is documentation-only and uses `[skip ci]`.


## 2026-11-09 12:58 UTC — source, CI, and package revalidation

### Branch and source provenance

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit` before checking source and Actions.
- Performance-audit branch HEAD before this update: `c756ad757eedd8499d84a0e098b5ced54ee79a7c`. Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- GitHub compare from the driver-source commit to branch HEAD reports 54 commits ahead / 0 behind. The only changed paths are `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this audit; no driver source or workflow file changed after the source snapshot.
- Latest CI-tested source remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. The later commits are documentation-only.

### Descriptor-set allocation failure classification re-read

The current `panvk_desc_pool_allocate_set()` implementation was inspected at the active branch head. Its failure returns are classified as follows, with the aggregate counter retained:

- **Overflow:** variable descriptor-count adjustment overflow and the final descriptor-count bound check. Both return `VK_ERROR_OUT_OF_POOL_MEMORY`.
- **Capacity:** no free set slot or insufficient descriptor-heap free bytes. Returns `VK_ERROR_OUT_OF_POOL_MEMORY`.
- **VMA:** `util_vma_heap_alloc()` returns zero. Returns `VK_ERROR_FRAGMENTED_POOL`.

The profiling counters are gated by `PANVK_DESCRIPTOR_PROFILE=1`; allocation policy, descriptor layout, Vulkan return values, and allocator behavior are unchanged. The set-allocation-attempt/byte counters are incremented after arithmetic validation, so arithmetic-overflow failures are represented in the aggregate/category failure counters but not in the normal set-attempt count. The historical 299-failure capture predates this split and cannot be classified retroactively. No allocator change is warranted from that old aggregate alone.

### Heap-renewal threshold source review

- `kbase_tiler_heap_renew_work()` reads `PANVK_KBASE_HEAP_RENEW_WORK` using `debug_get_num_option()`, with compiled default `65536`. Positive values replace the work threshold; non-positive values disable work-triggered renewal. The helper logs the effective value once.
- The value is cached in a function-static variable on first use. Set the environment before the Winlator/game process reaches its first relevant Kbase queue submission, and restart that process between the absent-variable and override variants. Changing the environment after the first call cannot update the cached threshold.
- The independent interval trigger remains enabled at its default of 128 tiler submissions. The 262,144 test changes only the work threshold; it does not disable or modify the interval path. Historical telemetry showed 4,066 work-triggered renewals and 2 interval-triggered renewals out of 4,068 total, so the work threshold is the dominant measured trigger in that capture.
- The log reports the effective value, not its origin. The 65,536-default package is required: absent variable must log 65,536; an actually propagated `PANVK_KBASE_HEAP_RENEW_WORK=262144` must log 262,144.

### CI revalidation

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: SUCCESS; all 15 steps completed, including Android ARM64 libdrm, configure, compile/stage, dependency audit, flat package creation, and artifact upload.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source: SUCCESS. Release and Debugoptimized jobs each completed all 12 steps.
- Branch-filtered Actions listing contains no newer Android-only or wrapper run on `experiment/g720-performance-audit`. No workflow was rerun and no build was triggered in this pass.

### Newly attached package and benchmark evidence

- The newly attached `PanVK-G720-65k-env-override-test.zip` was independently checked in the current runtime. SHA-256: `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`. ZIP integrity passes; exactly four members are present: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three embedded member hashes pass. Both libraries are ELF64 AArch64; the driver SONAME is `libvulkan_panfrost.so`, RUNPATH is `$ORIGIN`, and it needs the bundled private `libpanvk_drm.so`; the private library needs only `libc.so`. Metadata identifies package `26.3.0-devel-android-kbase-fb70e9b5cf8b`, min API 30, and explicitly marks it experimental/non-conformant. This confirms package integrity and static linker metadata only, not Winlator runtime compatibility.
- No new Tomb Raider benchmark log/archive or FPS result was attached. The available diagnostic archive and historical telemetry remain unchanged: 4,068 renewals (4,066 work-triggered, 2 interval-triggered), 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO allocation failures, zero backing BO creation failures, and 299 descriptor-set allocation failures before category instrumentation.
- FPS evidence remains unchanged: instrumented diagnostic 7.7 / 19.7 / 13.6; clean 65,536 baseline 9.0 / 17.6 / 13.5; separate 262,144-compiled-default candidate 7.8 / 18.9 / 13.9. These are not matched repeated A/B runs. No performance gain is claimed, and the 262K compiled-default candidate does not prove environment propagation.

### Next device test and safety

Use the verified 65,536-default package only, imported as a separate Winlator Mali custom driver:

1. With `PANVK_KBASE_HEAP_RENEW_WORK` absent before process startup, run the same Tomb Raider scene/settings and capture the first threshold log; require `tiler heap renewal work threshold=65536`.
2. Restart the game/container process with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` already set before first Kbase submission; require `tiler heap renewal work threshold=262144`.
3. If the override run reports 65,536 or no threshold log appears, mark the test invalid and do not compare its FPS.
4. Leave `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset in both uninstrumented performance runs. Keep scene, resolution, quality, DXVK 1.7.2, wrapper, and thermal starting conditions matched; record frame pacing, visual correctness, hangs/device loss, and teardown. Prefer repeated A/B samples.
5. Only after the uninstrumented comparison, run a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic capture if new descriptor failure counts are needed.

No driver source, allocator policy, heap topology, ring size, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed. The experimental package remains a separate Winlator import; keep the stock System Driver available for rollback.


### CI/source ancestry clarification — 2026-11-09 12:59 UTC

A direct GitHub comparison was made to ensure the successful CI run actually contains the last driver-source change:

- Base: latest driver-source commit `a2e5ac142b4f0df71cb28204ece4337bace046de`.
- Head: CI-tested commit `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`.
- Result: `b17b98c` is 45 commits ahead and 0 behind `a2e5ac1`; the only changed paths in the comparison are `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`. No driver source or workflow changed between them.
- Therefore Android-only run `42215758218` and wrapper run `42215758167` do cover the latest driver source at `a2e5ac1`; they are not stale relative to that source. This resolves the apparent source/CI snapshot gap in the preceding addendum.
- No new build was needed or triggered. No source or runtime result changed.


## 2026-11-10 12:25 UTC — scheduled active-branch CI and artifact revalidation

### Branch and source provenance

- Re-read this handoff and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` before inspecting Actions.
- Handoff branch pre-update HEAD: `53829591eafd4c606989bf6fb0c95b9c26636853`. Performance-audit branch pre-update HEAD: `977e22667a38aedc7544e6c332c0363bd733ac24`.
- Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`; latest CI-tested snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`.
- GitHub compare from the driver-source commit to active branch HEAD reports 56 commits ahead / 0 behind. The only changed paths are `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` (added) and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` (modified). No PanVK source or workflow file changed after the CI-tested snapshot.

### Latest relevant GitHub Actions

- Branch-filtered Actions API returned 125 total runs (100 newest in the first page). The newest Android-only and wrapper workflows on `experiment/g720-performance-audit` remain the 2026-11-06 runs below; no newer relevant run was found.
- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: completed SUCCESS. All 15 returned steps completed successfully: setup, selected-branch checkout, checkout verification, Python setup, dependency installation, host tools, Android ARM64 libdrm, Android-only configuration, compile/stage, driver/manifest/dependency/staging audit, flat package creation, artifact upload, post-Python, post-checkout, and job completion. Pending 0; failed 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source:
  - Release job `127885224388`: SUCCESS; all 12 returned steps completed, including checkout, `memfd_create` compatibility patch, Python/dependencies, host tools, Meson configuration, compilation, artifact upload, cleanup, and completion. Pending 0; failed 0.
  - Debugoptimized job `127885224836`: SUCCESS; all 12 returned steps completed. Pending 0; failed 0.
- No job failed or remained pending; no failure-log investigation was necessary. No workflow was rerun and no build was triggered. The originally supplied 41947103034 / 41947103085 IDs remain superseded.

### Artifact download and independent validation

**Android-only artifact 13501237590**
- GitHub artifact size: 16,753,940 bytes; expiry 2027-02-04. GitHub digest and independently computed outer ZIP SHA-256 both equal `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`; ZIP integrity test passed.
- Outer ZIP contains the staged Android install, audit reports, and nested `PanVK-G720-experimental.zip`. Staged files include `stage/usr/android_stub/libdrm.so`, `stage/usr/local/lib/libvulkan_panfrost.so`, and `stage/usr/local/share/vulkan/icd.d/panfrost_icd.aarch64.json`.
- Nested flat package SHA-256: `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; ZIP integrity passed. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- All three embedded member hashes independently match: driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`; private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`; metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`.
- Metadata is valid JSON: package version `26.3.0-devel-android-kbase-b17b98cef92f`, minimum API 30, library `libvulkan_panfrost.so`; it explicitly says experimental/non-conformant and device/game compatibility unverified.
- Both inner libraries are ELF64 AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; NEEDED includes bundled private `libpanvk_drm.so`. The private library's only NEEDED entry is Android `libc.so`.
- Staged ICD manifest is valid JSON and reports Vulkan API `1.4.354`, architecture 64, path `/usr/local/lib/libvulkan_panfrost.so`. Audit lists 24 undefined DRM symbols; all 24 are present in the bundled libdrm defined-symbol set (zero computed missing), and `missing-drm-symbols.txt` is empty.
- Classification: **build passed; outer staged artifact and inner flat package statically/package validated**. This does not prove Winlator import, device runtime, Kbase compatibility, Vulkan conformance, WSI stability, or game performance.

**Android wrapper artifacts**
- Release [artifact 13502135385](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502135385): 4,288,169-byte ZIP; outer SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; one member `libvulkan_panfrost.so`, SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Debugoptimized [artifact 13502061037](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502061037): 50,663,624-byte ZIP; outer SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; one member `libvulkan_panfrost.so`, SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both ZIP integrity tests passed. Both extracted libraries are ELF64 AArch64 Android API 30. Both use RUNPATH `$ORIGIN/../../android_stub` and depend on `libdrm.so`, `libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`, and `libxshmfence.so.1`, in addition to Android platform libraries.
- Neither wrapper ZIP contains the referenced Android stub, X11/XCB dependency closure, an ICD manifest, or a checksum manifest. Classification: **build passed; raw ELF/ABI verified; complete package/dependency closure not validated**. Do not call these standalone importable packages unless the intended runtime supplies and resolves the complete dependency set.

### Blockers and safety

- Android-only package's next gate is a controlled import through Winlator Mali's separate custom-driver importer, followed by device Vulkan initialization/device creation and stability logs. No device-runtime result is implied by CI.
- Wrapper dependency closure remains unverified.
- No source code, workflow, allocator policy, Kbase ABI, kernel, or Android system/vendor driver was changed. This update is documentation-only and uses `[skip ci]`; no new workflow was triggered.


## 2026-11-10 12:55 UTC — scheduled source, CI, and package revalidation

### Branch and source provenance

- Re-read this audit and the canonical `PROJECT_CONTINUITY_HANDOFF.md` before inspecting the repository.
- Performance-audit branch HEAD before this documentation update: `05da2a13d95aa914e2dcd9b51906c0325302fe6a` (commit message: `docs: record 2026-11-10 PanVK build monitoring [skip ci]`). The documentation branch HEAD before its update: `dc7ef23bd6738654ffedc404fe7c78a653e69a54`.
- Latest PanVK driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`. Comparing it with the performance-audit HEAD reports 57 commits ahead / 0 behind; the only changed paths are `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this performance-audit document. No PanVK source or workflow file changed after the tested source snapshot.
- Latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`.

### Source audit: descriptor failure classification

- Re-read `src/panfrost/vulkan/panvk_vX_descriptor_set.c` and `src/panfrost/vulkan/panvk_descriptor_set_layout.h` at the active branch.
- The three failure categories are present and each failed allocation path increments exactly one category plus the aggregate:
  - `overflow`: variable descriptor-count arithmetic overflow or final descriptor count above `UINT32_MAX`; existing result remains `VK_ERROR_OUT_OF_POOL_MEMORY`.
  - `capacity`: no free set slot or insufficient descriptor heap free bytes; existing result remains `VK_ERROR_OUT_OF_POOL_MEMORY`.
  - `vma`: `util_vma_heap_alloc()` returns zero; existing result remains `VK_ERROR_FRAGMENTED_POOL`.
- The layout's `desc_count` and binding counts are `unsigned` (32-bit on the Android target). The variable-count path asserts `base_descs <= num_descs`, so `adjusted_descs` cannot exceed the original 32-bit layout count before the `UINT32_MAX - adjusted_descs` check. Non-inline variable descriptor multiplication is promoted to `uint64_t`. No arithmetic-underflow defect was found in this path under its current invariants.
- Counters use relaxed atomics and are process-global. `panvk_desc_profile_dump()` logs on each failure, so keep `PANVK_DESCRIPTOR_PROFILE=1` strictly to a separate diagnostic run; it is not suitable for FPS measurement. No allocation policy, descriptor layout, or Vulkan return code was changed.

### Source audit: heap renewal threshold semantics

- `kbase_tiler_heap_renew_work()` still reads `PANVK_KBASE_HEAP_RENEW_WORK` once via `debug_get_num_option()`, with compiled default `KBASE_TILER_HEAP_RENEW_WORK=65536`, and logs the effective value.
- The call site guards work-triggered renewal with `renew_work && queue->kbase_tiler_work_count >= renew_work`; an effective value of zero disables the work trigger rather than renewing every submission.
- The separate interval trigger remains enabled by default at 128 tiler submissions. Its independent override is `PANVK_KBASE_HEAP_RENEW_INTERVAL`; leave it unset during the work-threshold A/B test.
- The work threshold is cached on first use. Set the environment variable before the first Kbase queue submission and restart the game/container process between variants.

### CI status

- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: SUCCESS; job `127885224447`, all 15 steps completed successfully.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source: SUCCESS; Release job `127885224388` and Debugoptimized job `127885224836`, all 12 steps in each completed successfully.
- The branch-filtered Actions listing returned 125 runs; the newest Android-only and wrapper runs for this branch remain the two above. No newer performance-audit build was found. No build was triggered by this documentation-only update.

### Newly attached package recheck

- The newly attached `PanVK-G720-65k-env-override-test.zip` is byte-for-byte the already recorded 65K-default package: SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`.
- ZIP integrity passes. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three embedded member checksums pass.
- Metadata identifies `26.3.0-devel-android-kbase-fb70e9b5cf8b`, minimum Android API 30, and explicitly labels the package experimental/non-conformant with device/game compatibility unverified.
- Both libraries are ELF64 AArch64. The driver has RUNPATH `$ORIGIN` and NEEDED `libpanvk_drm.so`; the private libdrm library has SONAME `libpanvk_drm.so` and depends only on Android `libc.so`.
- This is static/package verification only. It does not establish Winlator import, runtime initialization, Kbase ABI compatibility, WSI correctness, or game performance.

### Benchmark and device gate

- No new Tomb Raider benchmark log was attached. The available Tomb Raider archive remains the 2026-11-05 02:03:50 capture; no new FPS or telemetry data can be inferred from the repeated package upload.
- Retain the documented results without reinterpretation: clean 65,536 baseline 9.0 / 17.6 / 13.5 FPS; 262,144 compiled-default candidate 7.8 / 18.9 / 13.9 FPS (one run each, inconclusive); 7.7 / 19.7 / 13.6 FPS was instrumented and is diagnostic only. No FPS improvement is claimed.
- Use this 65K-default package for the environment-propagation test: first run with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require the startup log to show 65536; then restart with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` set before first Kbase submission and require 262144. If the second log is absent or remains 65536, the override was not demonstrated and the run is invalid for comparison.
- Keep all profiling variables unset in both performance runs. Match the Tomb Raider scene/settings, DXVK 1.7.2, wrapper, resolution, and thermal starting conditions; record min/max/average, frame pacing, visual correctness, stability, and teardown. Collect descriptor-category counts only in a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic run.

No driver source, allocator policy, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed. The experimental package remains a separate Winlator import.


## 2026-11-11 12:08 UTC — scheduled active-branch CI and artifact revalidation

### Branch and source provenance
- Re-read this handoff and the performance audit at their requested branches before monitoring.
- Pre-update branch heads: `docs/project-continuity-handoff-2026-11-03` = `15acf3c5b3a9bdfa3827082cc9754b27c8037593`; `experiment/g720-performance-audit` = `218739b263c8e8d17ac0d8dd5018f4d8067e28b4`.
- The latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`; latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. No newer relevant Android-only or Android-wrapper run is present on the active branch. The latest relevant runs remain the 2026-11-06 runs below.
- The active branch HEAD is a documentation-only update after the tested source snapshot; no new driver build was triggered or inferred.

### Latest active-branch Actions
- Branch-filtered Actions listing returned 125 runs; newest relevant Android-only and wrapper entries remain run `42215758218` and `42215758167`. The originally tracked 41947103034 / 41947103085 are superseded.
- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: completed SUCCESS. All 15 returned steps completed: job setup, selected-branch checkout and verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, driver/manifest/dependency audit, flat package creation, artifact upload, post steps, and completion. Pending 0; failed 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source:
  - Release job `127885224388`: SUCCESS; all 12 returned steps completed, including checkout, `memfd_create` compatibility patch, Python/dependencies, host tools, Meson configuration, compilation, upload, cleanup, and completion. Pending 0; failed 0.
  - Debugoptimized job `127885224836`: SUCCESS; all 12 returned steps completed. Pending 0; failed 0.
- No job failed or remained pending. No failure-log investigation was needed. No build was triggered during this monitoring pass.

### Fresh artifact download and validation

**Android-only artifact 13501237590**
- GitHub reports 16,753,940 bytes, not expired, expires 2027-02-04. Freshly downloaded ZIP integrity passes. Its SHA-256 is `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a`, matching GitHub's artifact digest.
- Outer ZIP contains 28 files, including the staged install, audit reports, and nested `PanVK-G720-experimental.zip`. The nested flat package SHA-256 is `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; ZIP integrity passes.
- Inner flat package contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three embedded member hashes independently match: driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`; private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`; metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`.
- Metadata is valid JSON, version `26.3.0-devel-android-kbase-b17b98cef92f`, minimum API 30, and explicitly labels the driver experimental/non-conformant with device/game compatibility unverified.
- Inner driver and private libdrm are ELF64 AArch64 Android API 30. The flat driver RUNPATH is `$ORIGIN` and its private `libpanvk_drm.so` dependency is bundled; that library needs only Android `libc.so`.
- The outer staged install includes `stage/usr/android_stub/libdrm.so`, the staged driver, and a valid ICD manifest at `stage/usr/local/share/vulkan/icd.d/panfrost_icd.aarch64.json`. Manifest API is `1.4.354`, architecture 64, path `/usr/local/lib/libvulkan_panfrost.so`. Staged ELF files are AArch64. The 24 listed undefined DRM symbols resolve against the bundled stub's defined-symbol set; `missing-drm-symbols.txt` is empty.
- Classification: **build passed; outer staged artifact and inner flat package statically/package validated**. No Winlator import, device runtime, Kbase compatibility, Vulkan conformance, WSI, or game-performance claim is made.

**Android-wrapper artifacts**
- Release artifact [13502135385](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502135385): 4,288,169 bytes; fresh ZIP SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`, matching GitHub digest. Its sole member `libvulkan_panfrost.so` SHA-256 is `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Debugoptimized artifact [13502061037](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502061037): 50,663,624 bytes; fresh ZIP SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`, matching GitHub digest. Its sole member `libvulkan_panfrost.so` SHA-256 is `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both ZIPs pass integrity checks. Both libraries are ELF64 AArch64 Android API 30. Both use RUNPATH `$ORIGIN/../../android_stub` and need `libdrm.so`, `libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`, and `libxshmfence.so.1`, in addition to Android platform libraries.
- Each ZIP contains only the ELF. Neither includes the referenced Android stub, X11/XCB dependency closure, ICD manifest, or checksum manifest. Classification: **build passed; raw ELF and ABI verified; complete package/dependency closure not validated**.

### Blockers and safety
- Android-only: next gate is controlled import of the inner flat ZIP through Winlator Mali's separate custom-driver importer, followed by Vulkan initialization, device creation, and runtime stability capture.
- Wrapper: dependency closure and runtime layout remain unresolved/unverified; do not describe either wrapper ZIP as a standalone import package.
- No source code, workflow, allocator policy, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed. This monitoring update is documentation-only and uses `[skip ci]`; no new build was triggered.



## 2026-11-11 13:09 UTC — uploaded package provenance and Tomb Raider archive recheck

### Branch/source provenance
- Pre-update heads: `experiment/g720-performance-audit` = `218739b263c8e8d17ac0d8dd5018f4d8067e28b4`; `docs/project-continuity-handoff-2026-11-03` = `15acf3c5b3a9bdfa3827082cc9754b27c8037593`.
- Current PanVK source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`; active performance-audit HEAD remains documentation-only after it. GitHub compare from `a2e5ac14` to CI snapshot `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24` is 45 commits ahead / 0 behind, with only `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` and this audit document changed.
- The newly uploaded `PanVK-G720-experimental-396c924.zip` identifies package version `26.3.0-devel-android-kbase-396c924ccccb`. Commit `396c924ccccb07b15b429b5cdc82fd9318374664` is an ancestor of the current source commit; compare reports 10 commits ahead / 0 behind to `a2e5ac14`. The only driver source files changed in that interval are `panvk_vX_gpu_queue.c` (+2 lines for the effective threshold log) and `panvk_vX_descriptor_set.c` (+25/-4 for failure classification).
- Consequently, the 396c924 package is stale for the current experiment: it contains the `PANVK_KBASE_HEAP_RENEW_WORK` option string but not the effective-threshold startup log, and its descriptor profile format contains only aggregate `set_failures`, without `overflow/capacity/vma` counters. Do not use it for the controlled threshold A/B or the descriptor-category diagnostic.
- The current 65K package remains the correct candidate. Its version is `26.3.0-devel-android-kbase-fb70e9b5cf8b`; it contains both the threshold log and the expanded descriptor-profile format. The re-upload is byte-identical to the already audited package.

### Uploaded package validation
- `PanVK-G720-experimental-396c924.zip` SHA-256: `2f82639a4c1a8ccf60c264f9a754edf0c10d75cad352487587648b328c3ff00a`. ZIP integrity and all three embedded member checksums pass. Four expected members are present: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`.
- Its driver is ELF64 AArch64 Android API 30, build ID `f82887f99420283de47618241282efb519cc0ed4`, RUNPATH `$ORIGIN`, and depends on the bundled `libpanvk_drm.so`. All 24 undefined DRM symbols resolve against the private library's exports. This is static package validation only; no runtime compatibility is inferred.
- Re-uploaded 65K package SHA-256 remains `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`; driver build ID `fe696905a32984ab33360d6ffdd298efd050d94c`. Its ZIP/member checksums, AArch64/API 30 metadata, `$ORIGIN` RUNPATH, and all 24 DRM-symbol resolutions pass. This is still not a device-runtime result.

### Tomb Raider archive recheck
- The attached `TombRaider_logs_2026-11-05_02-03-50.zip` is the existing 2026-11-05 02:03 capture, SHA-256 `6f54b17e422795673411dfc3106a184922ea283024801f1e257ed5dbd9f68a56`; it contains the same four logs and no new benchmark run.
- DXVK is 1.7.2 and the Vulkan device is `Wrapper(Mali-G720 MC7)`; DXVK reports four compiler threads. The final cumulative Kbase snapshot reports 16,160 submits; 68,077 waits; zero wait timeouts/errors; 41,704 ring jobs; zero ring wraps; 1.8 KiB sampled occupancy high-water; 4,068 heap renewals (4,066 work-triggered, 2 interval-triggered); 52,804.121 ms aggregate renewal wait and 61,272.081 ms total renewal time.
- Final transient-pool snapshot: 8,817,664 allocation calls; 46,981.32 MiB cumulative requested; zero allocation failures; 145,499 small-BO reuses; 2,680 large-BO reuses; 680 BO creates / 539.07 MiB cumulative created; zero BO creation failures; 42 MiB largest BO.
- Final descriptor snapshot remains aggregate-only: 9 pools / 4.43 MiB requested / 1.40 MiB largest; zero pool failures; 596,561 set allocations / 85.30 MiB requested; 299 set allocation failures. The archive has no effective-threshold log and no category breakdown. Its descriptor log format is the pre-split format; it cannot classify those 299 failures.
- The archive ends with `VK_ERROR_SURFACE_LOST_KHR`, a destroyed-mutex FORTIFY report, and Wine's `vkWaitForFences` assertion during teardown. Preserve this as a stability/teardown issue to investigate separately; the capture does not establish that it caused the measured frame rate.
- No new FPS measurement was supplied. Keep 7.7 / 19.7 / 13.6 FPS classified as instrumented diagnostic data; clean 65K 9.0 / 17.6 / 13.5 and the single 262K compiled-default 7.8 / 18.9 / 13.9 remain inconclusive. No FPS gain is claimed.

### CI and next gate
- Latest active-branch CI remains Android-only run `42215758218` (SUCCESS, 15/15 steps) and wrapper run `42215758167` (SUCCESS, Release and Debugoptimized, 12/12 steps each), both on `b17b98c`. No newer performance-audit Android build was found; this update triggers no CI because it is documentation-only.
- Use only the verified 65K-default package for the environment test. With the variable absent, require `tiler heap renewal work threshold=65536`; restart the process with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` set before first Kbase submission and require `262144`. If missing or still 65536, the override is not proven.
- Keep all profiling variables unset for both matched FPS runs. Then run a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic to obtain overflow/capacity/VMA counts. Do not use the 396c924 package for either test.
- No driver source, allocator policy, heap topology, ring size, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed.


## 2026-11-12 12:46 UTC — scheduled active-branch CI and artifact revalidation

### Branch/source provenance
- Re-read the canonical handoff and this audit before monitoring. Active performance-audit branch pre-update HEAD: `4880c79ade8a443119506945864bf85ebb50f795`; canonical handoff branch pre-update HEAD: `f48a1a0f1d2331cfd0b4ea451a6886efd8597318`.
- Latest driver-source commit remains `a2e5ac142b4f0df71cb28204ece4337bace046de`; latest CI-tested source snapshot remains `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`. GitHub compare is 60 ahead / 0 behind and contains only `docs/G720_CAPABILITY_MATRIX_2026-11-05.md` plus this audit document; no driver source or workflow file changed after the tested source snapshot.

### Active-branch GitHub Actions
- Branch-filtered Actions API: 125 total runs, 100 newest returned. No Android-only or Android-wrapper build newer than the 2026-11-06 pair was found. Original run IDs 41947103034 and 41947103085 are superseded.
- Android-only [run 42215758218](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218), job `127885224447`, source `b17b98cef92ff24b0ecb4e72fd1bd0d39b2a4a24`: SUCCESS; 15/15 steps completed (setup, checkout, checkout verification, Python, dependencies, host tools, Android ARM64 libdrm, Meson configure, compile/stage, driver/manifest/dependency audit, flat package, artifact upload, post steps, completion). Pending 0; failed 0.
- Android wrapper [run 42215758167](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167), same source: Release job `127885224388` SUCCESS, 12/12 steps completed; Debugoptimized job `127885224836` SUCCESS, 12/12 steps completed. Both include checkout, `memfd_create` compatibility patch, Python/dependencies, host tools, Meson configure, compile, upload, cleanup, completion. Pending 0; failed 0.
- No failure logs were needed; no builds were triggered during this pass.

### Freshly downloaded artifact validation
- Android-only artifact [13501237590](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758218/artifacts/13501237590), 16,753,940 bytes, not expired: outer ZIP SHA-256 `f5e58163b3caf3f9fa5791dedf56ade82fd87d17dcfdb42722cd846e51ed825a` matches GitHub digest and ZIP integrity passes. Outer archive includes staged driver, `stage/usr/android_stub/libdrm.so`, ICD manifest, audits, and nested flat package. Nested package SHA-256 `14f7b13ab1735daffd31a0723c1f6d949155e5c6570f1980f98291cb54383e86`; nested ZIP integrity passes. Four expected members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. All embedded hashes pass: driver `362ad05b978c673b82bf1e66b4ab553f8ea79154c8c5a0a6731b65648359bfb1`, private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`, metadata `064c52b79a1d0dc14aab89fa98857d4a905c2167c41d7ee28536b81337b82e81`. Metadata valid: `26.3.0-devel-android-kbase-b17b98cef92f`, min API 30, explicitly experimental/non-conformant. Both libraries ELF64 AArch64 Android API 30; driver RUNPATH `$ORIGIN`, private libdrm bundled and depends only on `libc.so`. ICD JSON valid, API `1.4.354`, architecture 64, correct staged library path. All 24 audited undefined DRM symbols resolve; missing report empty. Classification: outer staged package and inner flat package statically/package validated; no device-runtime claim.
- Wrapper Release artifact [13502135385](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502135385): 4,288,169 bytes; ZIP SHA-256 `5647fe999a9225123116118eb26ef91f277eef38d4ea2c2a29e98e965d3b0f04`; sole ELF SHA-256 `edce8b3dd4fea68ffda7e44622d4c778cdaf0a22fc219cc2b4368335060aa80a`.
- Wrapper Debugoptimized artifact [13502061037](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42215758167/artifacts/13502061037): 50,663,624 bytes; ZIP SHA-256 `fcd0b4d2dcba93c67f3c09dedb6cdc4ab843cd7eef45f34813e90916adbd4159`; sole ELF SHA-256 `eebdc6229236e4c91204bd01148409c2b91c71977d1cf71a44a25181b3617477`.
- Both wrapper ZIPs pass integrity checks and contain one ELF64 AArch64 Android API 30 library. Both use RUNPATH `$ORIGIN/../../android_stub` and depend on `libdrm.so`, X11/XCB libraries (`libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`), and `libxshmfence.so.1`, in addition to Android libraries. No stub, X11/XCB closure, ICD manifest, or checksum manifest is included. Classification: raw ELF/ABI verified, standalone package/dependency closure NOT validated.

### Blockers and safety
- Next Android-only gate: controlled Winlator Mali custom-driver import of the inner flat ZIP, then Vulkan initialization, device creation, and stability testing.
- Wrapper dependency closure must be supplied or proven by the target runtime before either ZIP is treated as a complete package.
- No source, workflow, allocator policy, Kbase ABI, kernel, or system/vendor Vulkan driver change. No device runtime result is inferred from CI.


## 2026-11-12 13:02 UTC — descriptor failure buckets made mutually exclusive

### Source changes
- Source commit `b91fa8e0ff114f002b438026671b2bc3595430ca` split the former combined descriptor-capacity failure bucket into no-free-set and descriptor-heap-capacity conditions. Its Android-only run `43221907435` and wrapper run `43221907406` both completed successfully; Release and Debugoptimized wrapper jobs passed.
- Review identified a classification ambiguity: both guards can be true for the same allocation (the set-slot bitmap can be exhausted while the descriptor heap also lacks the requested range). The first split used no-free-set precedence and would hide the simultaneous heap-capacity condition.
- Commit `07725210d9b463496e5974b4a2d4315bfafa6df4` added an explicit combined-capacity bucket. The resulting mutually exclusive diagnostic categories are: overflow, no-free-set only, heap-capacity only, both capacity conditions, and VMA allocation failure. The aggregate `set_alloc_failures` counter remains and, for paths in this function, should equal the sum of those five buckets.
- Commit `64c43833ae5e5f8e7f7a27e872a1ac5449188aee` moved the second heap-capacity read inside the profiling-enabled failure branch. The uninstrumented allocation fast path retains the original short-circuit guard and does not perform an additional `free_size` read. This keeps the diagnostic refinement out of successful allocation hot paths.
- All changes are opt-in telemetry only. Descriptor allocation policy, heap layout, Vulkan result codes, VMA behavior, and object lifetime are unchanged. Profiling still logs on each failure and must be used only in a separate diagnostic run.

### CI status
- The source commit `b91fa8e` passed Android-only run `43221907435` and wrapper run `43221907406` (Release and Debugoptimized).
- Latest source HEAD is `64c43833ae5e5f8e7f7a27e872a1ac5449188aee`. Its Android-only run `43223244479` and wrapper run `43223244570` were still in progress at the 13:02 UTC check. Do not package or recommend the latest counters until these exact-head builds pass and the Android artifact is inspected.
- The previous 65K package hash `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72` was re-uploaded again and is byte-identical to the prior package. It has the effective threshold log but predates the five-bucket descriptor profile; it remains usable for the threshold-only test, not for the new category diagnostic. Prefer the latest exact-head package once CI completes.

### Benchmark and device evidence
- The documentation commits ee61de6157290be062b37ee91dff3154085dc542 (audit branch) and ac5d931b6db685e7aaebe12de6317e48e875ab65 (handoff branch) also triggered CI: audit-branch Android-only run 43428658241, audit-branch wrapper run 43428658245, and handoff-branch wrapper run 43428666520. All three remained in progress at 2026-11-13 12:51 UTC; the source-commit runs 43428494162 and 43428494165 were also still in progress. These documentation-triggered builds contain no additional source changes.
- No new Tomb Raider log or FPS result was supplied. The only mounted Tomb Raider archive remains `TombRaider_logs_2026-11-05_02-03-50.zip`; the newly attached file is the repeated 65K driver package.
- Keep the measured results unchanged: clean 65K baseline 9.0 / 17.6 / 13.5 FPS; one 262K compiled-default run 7.8 / 18.9 / 13.9 FPS; instrumented diagnostic 7.7 / 19.7 / 13.6 FPS. These remain inconclusive and the instrumented result is not a clean comparator. No FPS gain is claimed.
- The old archive's 299 descriptor allocation failures cannot be retrospectively classified. A new run with the five-bucket build and `PANVK_DESCRIPTOR_PROFILE=1` is required. Do not enable descriptor profiling in the threshold A/B performance runs.

### Next step and safety
- Wait for Android-only and both wrapper builds at `64c43833` to pass. Independently verify the exact-head Android package, then use that 65K-default package for the threshold A/B and a separate descriptor diagnostic.
- For the A/B, restart between variants; with `PANVK_KBASE_HEAP_RENEW_WORK` absent require the log value 65536, then set exactly 262144 before the first Kbase submission and require 262144. Keep all profiling variables unset for both FPS runs.
- No allocator policy, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed. Keep the experimental driver as a separate Winlator custom import.


## 2026-11-12 13:08 UTC — exact-head descriptor diagnostics build and package verified

### Source and classification
- Latest driver-source commit: `64c43833ae5e5f8e7f7a27e872a1ac5449188aee`. The descriptor profile has five mutually exclusive failure buckets: overflow; no free descriptor-set slot only; descriptor-heap capacity only; both capacity conditions simultaneously; and VMA allocation failure.
- The aggregate `set_alloc_failures` counter remains. At a quiescent final snapshot, the five category totals should sum to the aggregate for `panvk_desc_pool_allocate_set()` failure paths.
- The final hot-path refinement keeps the original short-circuit capacity guard. The second heap-capacity read needed to distinguish the simultaneous case occurs only after a failure and only when `PANVK_DESCRIPTOR_PROFILE=1` is enabled. No allocator policy, descriptor layout, VMA behavior, Vulkan result code, or lifetime semantics changed.

### Exact-head CI
- Android-only [run 43223244479](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43223244479): SUCCESS, all 15 build/audit/package/upload steps completed.
- Android wrapper [run 43223244570](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43223244570): SUCCESS; Release and Debugoptimized jobs both completed all 12 steps.
- The immediately preceding source commits also passed: `b91fa8e` (Android `43221907435`, wrapper `43221907406`) and `07725210` (Android `43223015215`, wrapper `43223015197`). Latest source HEAD `64c43833` is now exact-head build tested.

### Fresh Android artifact verification
- Artifact [13946901882](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43223244479/artifacts/13946901882), 16,758,519 bytes; outer ZIP SHA-256 `5d5afcafdc3423e7a18b535f941d95a4391093720dd17d174a890ef6e490494e`, matching the GitHub digest. ZIP integrity passes.
- The nested flat Winlator package SHA-256 is `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`; both copies in the outer archive are byte-identical and ZIP integrity passes.
- Package metadata version: `26.3.0-devel-android-kbase-64c43833ae5e`, minimum Android API 30, explicitly experimental/non-conformant. Four flat members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. All embedded hashes pass: driver `ce03eeadea586804a3f9761e739ec6916cea184956eba39ce323c98a76628266`; private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`; metadata `61c4b6b1fa5224f7b96f281068bbfaa23d61697d1b347f9685710bdeb9ab63cb`.
- Both inner libraries are ELF64 AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; its private `libpanvk_drm.so` dependency is bundled. The private library has SONAME `libpanvk_drm.so` and depends only on Android `libc.so`. Independently recomputed: 24 undefined DRM imports, 212 private-library DRM exports, zero unresolved DRM imports. Outer staged ICD JSON is valid (API 1.4.354, architecture 64, staged library path correct); staged libdrm stub is present and `audit/missing-drm-symbols.txt` is empty.
- Binary strings confirm the effective-threshold startup log and all five descriptor profile labels, including `no_free_set_only`, `heap_capacity_only`, and `both_capacity`. Classification: exact-head build and static/package audits pass; Winlator runtime and game compatibility remain unverified.

### Uploaded package and benchmark
- The newly attached `PanVK-G720-65k-env-override-test.zip` is still the old package (SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`, metadata commit `fb70e9b5cf8b`). It contains the threshold log but only the older aggregate capacity label; do not use it for the five-bucket descriptor diagnostic.
- The latest exact-head package is the new artifact above. It retains the compiled 65,536 work threshold and is suitable for both the threshold propagation A/B and the separate five-bucket diagnostic.
- No new Tomb Raider log or FPS measurement was supplied. The only archive remains the 2026-11-05 02:03:50 capture. Keep clean 65K 9.0 / 17.6 / 13.5 FPS, one 262K compiled-default run 7.8 / 18.9 / 13.9 FPS, and instrumented 7.7 / 19.7 / 13.6 FPS unchanged and inconclusive. No FPS gain is claimed.

### Exact next device test and safety
- Import only the exact-head flat package as a separate Winlator Mali custom driver; preserve the stock System Driver as fallback.
- For the uninstrumented A/B, restart between variants. With `PANVK_KBASE_HEAP_RENEW_WORK` absent, require startup log value 65536. Then set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission and require 262144. If the value is missing or remains 65536, the override is not demonstrated.
- Keep `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset for both FPS runs. Match scene, settings, DXVK 1.7.2, wrapper, and thermal starting conditions; repeat A/B runs if possible.
- After performance runs, perform a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic, set before process launch. Capture aggregate failures and all five buckets; verify their sum. Profile output is cumulative and is emitted on pool-creation failure, every 16 successful descriptor-pool creates, and each set-allocation failure. Do not sum repeated log lines; use the final/highest cumulative values. Absence of a profile line alone is not proof of zero failures because there is no unconditional device-destroy dump. Profiling is not suitable for FPS comparisons.
- No heap/allocator policy, topology, ring size, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed.

## 2026-11-13 12:51 UTC — heap-renewal option semantics and package revalidation

- Re-read both canonical documents and revalidated branch refs. Before this entry, experiment/g720-performance-audit was 071ccd5bbd7948ff3c7e19a45eeb47c67ca04350; docs/project-continuity-handoff-2026-11-03 was f4693c2793e574da259e02b5d2bcd59015a92442.
- The latest driver implementation before this pass was 64c43833ae5e5f8e7f7a27e872a1ac5449188aee, with the five mutually exclusive descriptor allocation failure buckets already present. No allocator behavior or Vulkan return code change was needed.
- Source audit found a stale comment in kbase_tiler_heap_renew_interval(): it claimed PANVK_KBASE_HEAP_RENEW_INTERVAL=0 disables renewal entirely. The implementation maps non-positive or out-of-range values to UINT32_MAX; this only moves the interval trigger to that large threshold, while work-triggered renewal remains independently controlled by PANVK_KBASE_HEAP_RENEW_WORK. Updated the comment only in source commit 5ffd9d3353a6922718232a184c35316f498d5b85 (panvk: clarify heap renewal interval override semantics). This is documentation-only source text; no executable behavior, default, ABI, queue state, or heap policy changed.
- Effective defaults remain interval 128 tiler submissions and work threshold 65,536. For the planned work-threshold A/B, leave PANVK_KBASE_HEAP_RENEW_INTERVAL unset; change only PANVK_KBASE_HEAP_RENEW_WORK.
- Exact-source CI for 5ffd9d3 was triggered: Android-only run 43428494162 and wrapper run 43428494165. Both were in progress at 2026-11-13 12:51 UTC (dependency installation stage); do not mark them successful or package the new commit until they complete. Previous exact-head source 64c43833 remains green: Android-only run 43223244479 (15/15 steps) and wrapper run 43223244570 (Release and Debugoptimized, 12/12 steps each).
- Newly attached PanVK-G720-64c43833-flat.zip SHA-256 is 83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433, byte-identical to the already verified exact-head artifact package for 64c43833. ZIP integrity and embedded checksums pass; metadata marks it experimental/non-conformant, min API 30; both libraries are ELF64 AArch64; driver RUNPATH is $ORIGIN; private libpanvk_drm.so is bundled. Binary strings include the effective-threshold log and all five descriptor diagnostic labels. Runtime compatibility is still unverified. Because 5ffd9d3 changes only a comment, this package has identical executable behavior, but its package metadata still identifies 64c43833; wait for the new CI artifact if strict exact-commit provenance is required.
- No new Tomb Raider log or FPS result was supplied. The mounted archive remains TombRaider_logs_2026-11-05_02-03-50.zip, SHA-256 6f54b17e422795673411dfc3106a184922ea283024801f1e257ed5dbd9f68a56, with the same four logs. Current measurements remain clean 65K 9.0 / 17.6 / 13.5 FPS, one 262K compiled-default run 7.8 / 18.9 / 13.9 FPS, and instrumented diagnostic 7.7 / 19.7 / 13.6 FPS. These are inconclusive; no FPS gain is claimed.
- Existing telemetry remains the only baseline: 4,068 heap renewals (4,066 work-triggered, 2 interval-triggered), 52,804.121 ms aggregate renewal wait, zero CSF ring wraps, 1.8 KiB sampled occupancy high-water, zero transient BO allocation/creation failures, and 299 aggregate descriptor-set allocation failures. The old capture predates the five-bucket descriptor instrumentation and cannot be reclassified.
- Next device test remains: use the verified 65K-default package as a separate Winlator custom driver; with PANVK_KBASE_HEAP_RENEW_WORK absent, restart and require threshold log 65536; restart with exactly PANVK_KBASE_HEAP_RENEW_WORK=262144 set before first Kbase submission and require 262144. If the log is absent or still 65536, the override is unproven. Keep all profiling variables unset in both matched FPS runs; then do a separate PANVK_DESCRIPTOR_PROFILE=1 diagnostic and check the five category totals against aggregate set_failures.
- No allocator policy, descriptor heap layout, CSF ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed.


## 2026-11-13 12:52 UTC — active-branch Android CI progress check

- Re-read the canonical handoff and performance audit before checking GitHub Actions. Active source branch HEAD at this check: `ee61de6157290be062b37ee91dff3154085dc542`; the commit message is `docs: record Nov 13 PanVK option audit and package recheck`. Its verified diff only appends monitoring documentation to `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`; no driver source or workflow files changed in this commit.
- The preceding driver-source commit is `5ffd9d3353a6922718232a184c35316f498d5b85`. Its verified diff changes only a comment above `kbase_tiler_heap_renew_interval()`: non-positive/out-of-range interval values saturate the interval trigger to `UINT32_MAX`, while work-triggered renewal remains independently controlled by `PANVK_KBASE_HEAP_RENEW_WORK`. No executable behavior, default, ABI, queue state, or heap policy changed.
- Newest current-HEAD Android-only run [43428658241](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241), job `131896297915`: `in_progress`. Step snapshot: 7 completed successfully, 1 in progress (Configure Android-only build), 6 pending (compile/stage, driver/manifest/dependency audit, flat package, upload, and two post steps); 0 failed.
- Newest current-HEAD Android-wrapper run [43428658245](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245): Release job `131896298179` `in_progress`, 5 completed, 1 in progress (Generate Host Tools), 5 pending; Debugoptimized job `131896298540` `in_progress`, 6 completed, 1 in progress (Configure Meson), 4 pending. No failed steps.
- Exact-source predecessor runs for `5ffd9d3` are also still running: Android-only [43428494162](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494162), job `131895759577`, 6 completed / 1 in progress (Build real Android ARM64 libdrm) / 7 pending; wrapper [43428494165](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494165), Debugoptimized job `131895759417` 7 completed / 1 in progress (Compile Project) / 3 pending, Release job `131895759805` 6 completed / 1 in progress (Configure Meson) / 4 pending. All six jobs remain `in_progress`; no failed steps were reported.
- Artifact listing for the two newest current-HEAD runs is empty at this snapshot. The predecessor runs have not completed/uploaded artifacts either. Therefore there is no new artifact to inspect or describe as usable. Existing exact-head `64c43833` package validation remains the last completed package validation; its outer artifact and inner flat package hashes are recorded above.
- No failure logs were needed because no job or step has failed. No source change, rerun, or build was initiated by this monitoring pass; the four runs above were already triggered by pushes. The documentation-only `ee61de6` push itself also triggered these current-HEAD workflows.
- Next gate: wait for all three jobs on current HEAD `ee61de6` and all three predecessor jobs on `5ffd9d3` to complete. If any fails, inspect its job log and identify the first actionable error. If Android-only succeeds, download and independently verify the exact-head package, ICD manifest, bundled private DRM dependency, all member hashes, and unresolved-symbol audit. Wrapper Release/Debugoptimized artifacts must be checked for dependency closure; a green build alone is not package validation.
- No driver behavior, allocator policy, heap topology, CSF ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed. Keep the experimental package separate for the non-rooted Winlator custom-driver import path.


## 2026-11-13 12:57 UTC — current-HEAD Android and wrapper builds completed; artifact audit

### Source provenance and workflow status
- Current CI-tested branch snapshot: `ee61de6157290be062b37ee91dff3154085dc542` (`docs: record Nov 13 PanVK option audit and package recheck`). Its diff from `5ffd9d3353a6922718232a184c35316f498d5b85` only appends this audit record; no driver-source or workflow changes were present in the tested HEAD commit.
- Preceding source commit `5ffd9d3353a6922718232a184c35316f498d5b85` was independently diff-checked: the sole source change corrects a comment in `kbase_tiler_heap_renew_interval()` to state that non-positive/out-of-range interval values saturate the interval trigger to `UINT32_MAX`, while work-triggered renewal remains independently controlled by `PANVK_KBASE_HEAP_RENEW_WORK`. No executable behavior, default, ABI, queue state, or heap policy changed.
- Current-HEAD Android-only [run 43428658241](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241), job `131896297915`: SUCCESS, 15/15 steps completed. Checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, driver/manifest/dependency audit, flat package creation, upload, and post-job cleanup all completed; pending 0, failed 0.
- Current-HEAD wrapper [run 43428658245](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245): Release job `131896298179` SUCCESS, 12/12 steps; Debugoptimized job `131896298540` SUCCESS, 12/12 steps. Both completed checkout, `memfd_create` compatibility patch, Python/dependency setup, host-tool generation, Meson configuration, compilation, artifact upload, and cleanup; pending 0, failed 0.
- Preceding source-commit runs also completed successfully: Android-only [43428494162](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494162), job `131895759577`, 15/15; wrapper [43428494165](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494165), Release job `131895759805` and Debugoptimized job `131895759417`, 12/12 each. No failed jobs or steps across the six jobs; no failure logs required.
- The current-HEAD builds were triggered by the documentation-only push `ee61de6`; no build was manually dispatched or rerun by this monitoring pass.

### Current-HEAD Android-only artifact verification
- Artifact [14037579498](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241/artifacts/14037579498), 16,754,432 bytes; outer ZIP SHA-256 `e11fc0e1ebf108d1be51d96f51b0a50aa78d2f9c0f297aac8d80331e951e1540`, matching GitHub's artifact digest. ZIP integrity passes.
- Nested flat package SHA-256 `90ee4a2d740ae71c5df174ebc9e93fb1b30f519b2cbf7f8895b3440f2c850c14`; ZIP integrity passes. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- Metadata is valid JSON: package `26.3.0-devel-android-kbase-ee61de615729`, minimum API 30, explicitly experimental/non-conformant with device/game compatibility unverified. All embedded hashes match: driver `e543e8986f5d6d9b90ecd107317ea06d2b192877d22c5667aa2484902017b1bb`; private libdrm `55b2a44016d44f4ef3ce802d62b23b146e98ad915894a38b9fcaf096ee8e2b8c`; metadata `0a5ac66bfcf13639f17bf416cc141d3f8802c24176f0d068885088fa684865f9`.
- Both inner libraries are ELF64 little-endian AArch64. Driver RUNPATH is `$ORIGIN`; NEEDED entries include Android platform libraries plus bundled `libpanvk_drm.so`. Private libdrm has SONAME `libpanvk_drm.so` and depends only on Android `libc.so`.
- Outer staged ICD manifest parses correctly: Vulkan API `1.4.354`, architecture 64, library path `/usr/local/lib/libvulkan_panfrost.so`. Outer staging includes `stage/usr/android_stub/libdrm.so`. The driver audit lists 24 undefined DRM imports and 212 private-library exports; unresolved set is empty and `audit/missing-drm-symbols.txt` is empty.
- Classification: current-HEAD Android package is statically/package validated for separate Winlator custom-driver import. No Winlator runtime, Vulkan device-creation, game, stability, or performance test has been performed.

### Current-HEAD wrapper artifact verification
- Release artifact [14039410095](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245/artifacts/14039410095), 4,289,124 bytes; outer ZIP SHA-256 `ac0c631dbbd8f7f4a78dfad78f96a5ac4e2fda82d258f915f6f006e832bd8d45`, matching GitHub's digest. Sole ELF member SHA-256 `e06188b4dee36957f80f197dc4e7581f649e2db92228c59c6e2cbaf5ef66e385`, size 19,053,392 bytes.
- Debugoptimized artifact [14038895577](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245/artifacts/14038895577), 50,660,929 bytes; outer ZIP SHA-256 `49b33dfc5a315e7b7ce824d17b552cd9f3dca1f8f9d79b964d092036e7ee2ba5`, matching GitHub's digest. Sole ELF member SHA-256 `9e09ca8f1d608555176ee1172a8762a883eb4f039740b11b71276b9dd2323cbb`, size 122,645,224 bytes.
- Both ZIP integrity checks pass; both sole members are ELF64 little-endian AArch64. Both use RUNPATH `$ORIGIN/../../android_stub` and depend on `libdrm.so`, X11/XCB libraries (`libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`), and `libxshmfence.so.1`, plus Android platform libraries.
- Each wrapper ZIP contains only `libvulkan_panfrost.so`; neither includes the referenced Android stub, X11/XCB dependency closure, ICD manifest, or checksum manifest. Classification: raw ELF/ABI and ZIP integrity verified, but neither wrapper artifact is a complete standalone import package.

### Remaining gates and safety
- The latest Android-only flat package is structurally validated; next proof requires user-side controlled import through Winlator Mali's custom-driver importer, Vulkan initialization/device creation, and stability testing. Preserve the stock System Driver as fallback.
- Wrapper dependency closure must be provided or demonstrated by the intended runtime before either wrapper ZIP is treated as a complete package.
- No new Tomb Raider log or FPS measurement was supplied. The previously recorded clean 65K result (9.0/17.6/13.5 FPS), single 262K compiled-default result (7.8/18.9/13.9 FPS), and instrumented diagnostic (7.7/19.7/13.6 FPS) remain inconclusive; no performance gain is claimed.
- No allocator policy, descriptor layout, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed. Keep the experimental package separate for the non-rooted Winlator import path.


## 2026-11-14 12:37 UTC — scheduled active-branch CI and package revalidation

### Branch provenance
- Re-read the canonical handoff and this audit before monitoring. Pre-update active branch HEAD: ddd92b1f08e177eadb82bffe5ef954f75ae22ea4; pre-update handoff branch HEAD: 370636069f650b710b06e938f5e278024396435f.
- The active experiment/g720-performance-audit branch has 135 Actions runs in the branch-filtered listing; the 100 newest were inspected. No newer relevant Android-only or wrapper build exists beyond runs 43428658241 and 43428658245.
- Latest driver-source commit is 5ffd9d3353a6922718232a184c35316f498d5b85. GitHub compare from 5ffd9d3 to current branch HEAD ddd92b1 reports 4 ahead / 0 behind, with only this audit document changed. The source commit's only code-file change is a comment clarification in kbase_tiler_heap_renew_interval(); no executable behavior changed after the source snapshot built below.

### GitHub Actions
- Exact driver-source commit Android-only run 43428494162, job 131895759577: SUCCESS, 15/15 steps (checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, dependency/manifest audit, flat package, upload, cleanup).
- Exact driver-source commit wrapper run 43428494165: Release job 131895759805 and Debugoptimized job 131895759417 both SUCCESS, 12/12 steps each (checkout, memfd_create compatibility patch, dependencies, host tools, Meson, compilation, artifact upload, cleanup).
- Latest active-branch workflow snapshot ee61de6157290be062b37ee91dff3154085dc542: Android-only run 43428658241, job 131896297915, SUCCESS, 15/15; wrapper run 43428658245, Release job 131896298179 and Debugoptimized job 131896298540, SUCCESS, 12/12 each. Pending steps: 0. Failed steps: 0. No job failure logs required.
- Current branch HEAD ddd92b1 is a later audit-document-only [skip ci] commit; no executable source changed since 5ffd9d3. No new workflow was dispatched or rerun during this monitoring pass.

### Android-only artifact verification
- Artifact 14037579498, 16,754,432 bytes. Fresh outer ZIP SHA-256 e11fc0e1ebf108d1be51d96f51b0a50aa78d2f9c0f297aac8d80331e951e1540, matching GitHub's digest. ZIP integrity passes.
- Nested flat package SHA-256 90ee4a2d740ae71c5df174ebc9e93fb1b30f519b2cbf7f8895b3440f2c850c14; nested ZIP integrity passes. It contains exactly meta.json, libvulkan_panfrost.so, libpanvk_drm.so, and SHA256SUMS; all three embedded member hashes pass.
- Metadata is valid JSON: package 26.3.0-devel-android-kbase-ee61de615729, minimum API 30, explicitly experimental/non-conformant with device/game compatibility unverified. Both package libraries are ELF64 little-endian AArch64, Android API 30.
- Driver RUNPATH is $ORIGIN; NEEDED includes bundled libpanvk_drm.so. Private libdrm has SONAME libpanvk_drm.so and depends only on Android libc.so. The outer staged tree contains a valid ICD manifest (API 1.4.354, architecture 64, path /usr/local/lib/libvulkan_panfrost.so) and stage/usr/android_stub/libdrm.so. Clarification: the ICD manifest and staged stub are in the outer staged tree, not members of the nested four-file flat ZIP.
- The audit lists 24 undefined DRM imports and 212 bundled-stub definitions; audit/missing-drm-symbols.txt is empty (0 unresolved symbols). Classification: build passed; Android artifact statically/package validated for separate Winlator import; device runtime not validated.

### Wrapper artifact verification
- Release artifact 14039410095, 4,289,124 bytes; fresh outer ZIP SHA-256 ac0c631dbbd8f7f4a78dfad78f96a5ac4e2fda82d258f915f6f006e832bd8d45, matching GitHub digest.
- Debugoptimized artifact 14038895577, 50,660,929 bytes; fresh outer ZIP SHA-256 49b33dfc5a315e7b7ce824d17b552cd9f3dca1f8f9d79b964d092036e7ee2ba5, matching GitHub digest.
- Both ZIPs pass integrity checks and contain only libvulkan_panfrost.so, an ELF64 little-endian AArch64 Android API 30 library. Both use RUNPATH $ORIGIN/../../android_stub and need libdrm.so, X11/XCB libraries, and libxshmfence.so.1, in addition to Android platform libraries. Neither ZIP includes the referenced stub, X11/XCB dependency closure, ICD manifest, or checksum manifest. Classification: raw ELF/ZIP verified; standalone package and dependency closure not validated.

### Related agent-bootstrap branch and remaining blockers
- The handoff records a new public-repository OpenCode bootstrap branch, agent/free-panvk-agent, commit 49599fe7c85da3a06c557592cdeb55baf31a95e7. Compare from ddd92b1 shows six infrastructure/setup/workflow files changed and no PanVK driver source changes.
- Its Android-only run 43480954800, job 132076967574, passed 15/15; wrapper run 43480954763, Release job 132076968895 and Debugoptimized job 132076968133, passed 12/12 each. Agent Android artifact 14066160614 has outer SHA-256 096d633d7183cea27c9dab44e99ddeedb9d5b05a6a55927c91d4a5e80dfafb07, nested package SHA-256 ffc6e25b168cac9bc5fd352c679ef86d08f4c18554cbc0e2f76b9158792541f8; all four flat-package files, embedded hashes, AArch64/API 30 ELF, staged ICD manifest, bundled private libdrm, and zero-unresolved-symbol audit pass. Its wrapper artifacts remain raw ELF-only with the same external dependency closure and are not standalone-package validated.
- The prior handoff-branch wrapper run 43428666520 has also completed successfully: Release job 131896326353 and Debugoptimized job 131896325976, 12/12 steps each. Its artifacts are outside the active-branch package assessment.
- No new phone-side Tomb Raider log, heap-threshold override log, descriptor-profile capture, Vulkan initialization log, or device-runtime result was supplied. Existing 65K/262K FPS comparisons remain inconclusive; no performance gain is claimed.
- Next gates: controlled import of the Android flat package through Winlator Mali, Vulkan initialization/device creation/stability; matched uninstrumented 65K-default versus 262144 environment-override test with startup-log confirmation; then a separate PANVK_DESCRIPTOR_PROFILE=1 run to collect the five mutually exclusive failure buckets. Keep profiling disabled for FPS runs.
- No driver source, allocator policy, Kbase ABI, kernel, Android system/vendor Vulkan driver, or live device configuration was changed. Documentation update only; commit uses [skip ci].



## 2026-11-14 12:54 UTC — scheduled source, CI, and uploaded-package revalidation

- Re-read this audit and the canonical handoff before checking repository state. Pre-update heads: `experiment/g720-performance-audit` = `cb2455baa3be3f4f0e3c27053960cc2ed8badff9`; `docs/project-continuity-handoff-2026-11-03` = `9d76fc5b79215491ee0866f4dd873e2aae827d1b`.
- Latest executable driver-source commit remains `5ffd9d3353a6922718232a184c35316f498d5b85`. Its only source change is a comment correction documenting that non-positive/out-of-range `PANVK_KBASE_HEAP_RENEW_INTERVAL` saturates the interval trigger to `UINT32_MAX`; work-triggered renewal remains independently controlled. Compare from `5ffd9d3` to pre-update active HEAD `cb2455b` reports 5 ahead / 0 behind and only this audit document changed. No PanVK executable source or workflow changes were found.
- Rechecked the descriptor allocator at `src/panfrost/vulkan/panvk_vX_descriptor_set.c`, lines 614–710. The five opt-in failure buckets remain mutually exclusive: overflow; no-free-set only; heap-capacity only; both capacity conditions; VMA allocation failure. Existing Vulkan returns remain `VK_ERROR_OUT_OF_POOL_MEMORY` for overflow/capacity and `VK_ERROR_FRAGMENTED_POOL` for VMA failure. The extra heap-capacity read used to distinguish simultaneous conditions occurs only inside the failure path when `PANVK_DESCRIPTOR_PROFILE` is enabled. No allocation policy or default fast-path behavior changed. At a quiescent final snapshot, the five categories should sum to aggregate `set_failures`; concurrent snapshots may be transiently inconsistent because counters are relaxed atomics.
- Rechecked Kbase threshold logic: compiled work threshold is 65,536; `PANVK_KBASE_HEAP_RENEW_WORK` is read and cached on first use and its effective value is logged once. The interval default remains 128. Non-positive/out-of-range interval values saturate that trigger to `UINT32_MAX`; they do not disable work-triggered renewal. Set the work override before the first Kbase submission and restart the game process between variants.
- Latest active-branch CI remains green: Android-only run [43428658241](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241), 15/15 steps; wrapper run [43428658245](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245), Release and Debugoptimized jobs 12/12 each. The active branch listing has 135 runs; the newest relevant runs are the 13 November pair above. The separate infrastructure-only `agent/free-panvk-agent` runs 43480954800 and 43480954763 also pass; that branch adds OpenCode setup/workflow files only, not PanVK driver source.
- The newly attached `PanVK-G720-64c43833-flat.zip` was independently checked: SHA-256 `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`; ZIP integrity and all embedded SHA256SUMS pass; metadata identifies `26.3.0-devel-android-kbase-64c43833ae5e`, min API 30; both libraries are ELF64 little-endian AArch64; driver RUNPATH is `$ORIGIN`; `libpanvk_drm.so` is bundled. The binary contains the effective-threshold log string and the five descriptor-profile labels. A dynamic-symbol audit found 24 undefined DRM imports, 212 private-library DRM exports, and zero unresolved DRM symbols. This package is from source commit `64c43833`, before the later comment-only `5ffd9d3` change; executable behavior is unchanged, but it is not exact-HEAD provenance. It is suitable for the threshold and descriptor diagnostic tests. The older `fb70e9b5` package (SHA-256 `42f63022e8b65aca79e47a1cd37808a53bbe793d61b8a2d7b544ec651fd2ac72`) has the threshold log but predates the five-bucket descriptor counters, so do not use it for descriptor classification.
- No new Tomb Raider benchmark archive or device log is attached or found in the searched conversation/library files. Current mounted uploads are driver packages only. Keep the existing clean 65K result (9.0/17.6/13.5 FPS), one 262K compiled-default result (7.8/18.9/13.9 FPS), and instrumented diagnostic result (7.7/19.7/13.6 FPS) inconclusive. The compiled-default 262K run does not prove environment-variable propagation; no FPS gain is claimed.
- Next device gate is unchanged: with the 65K-default binary, run an uninstrumented baseline with the work variable absent and require log value 65,536; restart with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` set before first Kbase submission and require 262,144. If the second log is absent or remains 65,536, the override is unproven and its FPS result is invalid as a 262K comparison. Keep all profiling variables unset for both FPS runs; then run a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic and capture all five categories plus aggregate failures.
- No driver source, allocator policy, descriptor layout, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor Vulkan driver was changed. Device runtime compatibility remains unverified.


## 2026-11-15 12:37 UTC — scheduled active-branch CI and package revalidation

### Branch and source provenance
- Re-read the canonical handoff and this audit before monitoring. Pre-update active branch HEAD is `ded5ae97f99cd698e79d2d5e56d45904b6563178`; pre-update handoff file blob is `4260d08a60c362f732126999635959d445371f18`; audit file blob is `1d5758e6434412dca5dd4e54adbb808317ae9661`.
- GitHub's latest commit query for `experiment/g720-performance-audit` confirms HEAD `ded5ae97f99cd698e79d2d5e56d45904b6563178` (2026-11-14 13:01:11 UTC). Compare from latest driver-source commit `5ffd9d3353a6922718232a184c35316f498d5b85` to HEAD reports 6 ahead / 0 behind, with exactly one changed file: this performance-audit Markdown document (106 additions, 0 deletions). No executable PanVK source or workflow change occurred after the tested driver-source commit.
- The original baseline runs `41947103034` and `41947103085` remain superseded.

### Latest active-branch GitHub Actions
- Branch-filtered Actions query returned the latest 100 runs; newest relevant active-branch Android-only and wrapper runs are still the 13 November pair. No newer relevant run was found.
- Android-only [run 43428658241](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241), head `ee61de6157290be062b37ee91dff3154085dc542`, job `131896297915`: conclusion SUCCESS; all 15 returned steps completed. Steps include checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Android-only Meson configuration, compile/stage, driver/manifest/dependency audit, flat package creation, upload, and post-job cleanup. Pending 0; failed 0.
- Wrapper [run 43428658245](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245), same head:
  - Release job `131896298179`: SUCCESS, all 12 returned steps completed (setup, checkout, `memfd_create` compatibility patch, Python, dependencies, host tools, Meson configure, compile, artifact upload, post-job cleanup).
  - Debugoptimized job `131896298540`: SUCCESS, all 12 returned steps completed.
  - Pending 0; failed 0.
- Exact driver-source commit `5ffd9d3353a6922718232a184c35316f498d5b85` also has successful builds: Android-only [run 43428494162](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494162), job `131895759577`, 15/15; wrapper [run 43428494165](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428494165), Release job `131895759805` and Debugoptimized job `131895759417`, 12/12 each. No failed jobs or steps; failure-log inspection was not applicable. No build or rerun was triggered by this monitoring pass.

### Latest Android-only artifact audit
- Downloaded artifact [14037579498](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658241/artifacts/14037579498), 16,754,432 bytes. Fresh outer ZIP SHA-256 `e11fc0e1ebf108d1be51d96f51b0a50aa78d2f9c0f297aac8d80331e951e1540`, matching GitHub's artifact digest; ZIP integrity passes.
- Nested `PanVK-G720-experimental.zip` SHA-256 `90ee4a2d740ae71c5df174ebc9e93fb1b30f519b2cbf7f8895b3440f2c850c14`; nested ZIP integrity passes. Exactly four members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. All three embedded checksums pass.
- Metadata is valid JSON: package `26.3.0-devel-android-kbase-ee61de615729`, minimum Android API 30, explicitly experimental/non-conformant and device/game compatibility unverified.
- Both flat-package libraries are ELF64 little-endian AArch64. Driver RUNPATH is `$ORIGIN`; its NEEDED private library `libpanvk_drm.so` is bundled. Private library SONAME is `libpanvk_drm.so` and its only NEEDED entry is Android `libc.so`.
- Outer staged tree contains a valid ICD manifest at `stage/usr/local/share/vulkan/icd.d/panfrost_icd.aarch64.json`: file format 1.0.1, Vulkan API 1.4.354, architecture 64, library path `/usr/local/lib/libvulkan_panfrost.so`. Staged tree also contains `stage/usr/android_stub/libdrm.so`; note these outer staged files are not members of the nested four-file flat package.
- The workflow audit lists 24 undefined DRM imports; `audit/missing-drm-symbols.txt` is empty. Classification: Android-only build PASSED and outer/nested package STATICALLY VALIDATED for separate Winlator custom-driver importing. Runtime, Vulkan device creation, game compatibility, stability, and performance remain unverified.

### Wrapper artifact audit
- Release artifact [14039410095](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245/artifacts/14039410095), 4,289,124 bytes; fresh outer ZIP SHA-256 `ac0c631dbbd8f7f4a78dfad78f96a5ac4e2fda82d258f915f6f006e832bd8d45`, matching GitHub digest. Sole ELF member SHA-256 `e06188b4dee36957f80f197dc4e7581f649e2db92228c59c6e2cbaf5ef66e385`.
- Debugoptimized artifact [14038895577](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43428658245/artifacts/14038895577), 50,660,929 bytes; fresh outer ZIP SHA-256 `49b33dfc5a315e7b7ce824d17b552cd9f3dca1f8f9d79b964d092036e7ee2ba5`, matching GitHub digest. Sole ELF member SHA-256 `9e09ca8f1d608555176ee1172a8762a883eb4f039740b11b71276b9dd2323cbb`.
- Both ZIP integrity checks pass; each contains only `libvulkan_panfrost.so`, ELF64 little-endian AArch64 Android API 30. Both use RUNPATH `$ORIGIN/../../android_stub` and need `libdrm.so`, `libxcb.so.1`, `libX11-xcb.so.1`, `libxcb-dri3.so.0`, `libxcb-present.so.0`, `libxcb-xfixes.so.0`, `libxcb-sync.so.1`, `libxcb-randr.so.0`, `libxcb-shm.so.0`, and `libxshmfence.so.1`, plus Android platform libraries. Neither archive bundles the referenced stub, X11/XCB dependency closure, ICD manifest, or checksum manifest. Classification: wrapper builds PASSED; raw ELF/ZIP integrity verified; complete standalone package/dependency closure NOT validated.

### Current blockers and safety
- Android-only next gate: controlled import of the nested flat package through Winlator Mali's custom-driver importer, then capture Vulkan initialization, device creation, and stability. Preserve System Driver as fallback.
- Wrapper next gate: establish/provide the complete dependency closure in the intended runtime before calling either wrapper archive a complete package.
- No new Tomb Raider log, FPS run, heap-override startup log, descriptor-profile capture, or device-runtime result was supplied. Keep existing 65K/262K comparison inconclusive; do not claim a performance improvement.
- No source, allocator policy, descriptor layout, heap topology, queue/ring sizing, shader scheduling, Kbase ABI, kernel, Android system/vendor Vulkan driver, or live device configuration was modified. This documentation-only update uses `[skip ci]`.


## 2026-11-15 — scheduled source-concurrency audit and exact-head CI gate

### Source review and changes
- Re-read this audit and the canonical handoff from their respective branches. Active task branch before this documentation update: `experiment/g720-performance-audit` at `73873d730110bf3394ceea19b2a4d45ddba73834`; canonical handoff branch pre-update HEAD: `b66ed43422e322be684f3102c0b2016034c22a2b`.
- Source audit found unsynchronized C function-local lazy caches for process-wide diagnostic/environment options. These could race on first use when separate Vulkan queues, descriptor pools, or BO-pool users enter from different application threads. The descriptor-pool allocation host synchronization only serializes access to one pool, not distinct pools; queue submission can also be concurrent across queue objects.
- Commit `d6557723d9c53e717caf098e98e7a7263d081fa2` changed descriptor-profile enablement and the pair of heap-renewal options to C11 `call_once` initialization.
- Commit `73873d730110bf3394ceea19b2a4d45ddba73834` applied the same one-time initialization to the Kbase profile flag and transient-BO profile flag. The current source uses `once_flag` / `ONCE_FLAG_INIT` / `call_once` via `util/u_thread.h` for all four caches (descriptor profile, Kbase profile, mempool profile, and heap-renewal options).
- The interval/work options are now parsed together exactly once; the effective work threshold is still logged once. Defaults and policy are unchanged: interval 128 submissions, work threshold 65,536; non-positive work threshold disables only the work trigger, while interval renewal remains independent. Environment values must be present before first Kbase queue submission.
- The five descriptor-set failure buckets and aggregate counter are unchanged: overflow, no-free-set-only, heap-capacity-only, both-capacity, and VMA. Existing Vulkan error returns and allocator behavior are unchanged.
- This is concurrency/correctness hardening of diagnostic/configuration caches, not an FPS optimization. The hot paths now use the C11 once primitive's completed fast path; target-device overhead and runtime behavior have not been measured.

### CI status for current source
- Exact current source HEAD `73873d730110bf3394ceea19b2a4d45ddba73834`: Android-only [run 43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) and wrapper [run 43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) were in progress at the time of this update. No artifact from this source is offered until both workflows pass and the Android package is independently audited.
- Intermediate source commit `d6557723d9c53e717caf098e98e7a7263d081fa2`: Android-only [run 43716418873](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716418873) and wrapper [run 43716418887](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716418887) were also still running; these are superseded by the final 73873 source build.
- Last known successful source baseline remains `5ffd9d3353a6922718232a184c35316f498d5b85`: Android-only run `43428494162` and wrapper run `43428494165` passed; the documentation snapshot `ee61de6157290be062b37ee91dff3154085dc542` also passed Android-only run `43428658241` and wrapper run `43428658245`. Those successes predate the call_once changes.

### Uploaded package and benchmark evidence
- The newly attached `PanVK-G720-64c43833-flat.zip` is a duplicate of the prior package. Its SHA-256 is `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`; ZIP integrity and all three embedded member checksums pass. It contains `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; both libraries are ELF64 little-endian AArch64 Android API 30, driver RUNPATH is `$ORIGIN`, and private `libpanvk_drm.so` is bundled. Metadata identifies source `64c43833`. This package predates the new thread-safe cache commits and is not the current-head candidate; do not substitute it for the pending 73873 artifact.
- Current conversation uploads contain only repeated PanVK package ZIPs; the Library search returned no Tomb Raider log. No new Tomb Raider FPS, threshold startup log, descriptor-profile capture, or device runtime result was supplied.
- Keep the historical measurements unchanged and inconclusive: clean 65K baseline 9.0/17.6/13.5 FPS; one 262K compiled-default run 7.8/18.9/13.9 FPS; instrumented diagnostic capture 7.7/19.7/13.6 FPS. The compiled-default 262K run does not demonstrate environment-variable propagation; no performance gain is claimed.

### Next gates and safety
1. Complete both exact-head workflows for `73873d730110bf3394ceea19b2a4d45ddba73834`. Inspect failures if any; if both pass, download and independently verify the Android artifact and nested flat package before offering it.
2. With the current-head 65K-default package, run an uninstrumented baseline with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require the log value `65536`; restart and set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before first Kbase submission, requiring log value `262144`. If missing or still 65536, the override is unproven and the comparison is invalid.
3. Keep `PANVK_KBASE_PROFILE`, `PANVK_DESCRIPTOR_PROFILE`, `PANVK_MEMPOOL_PROFILE`, `PANVK_KBASE_DIAG_MEM_FLAGS`, and `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` unset for both FPS runs. Run descriptor profiling separately and verify the five category counts sum to aggregate `set_failures` at the final snapshot.
4. Preserve the stock System Driver as fallback. No Android system/vendor Vulkan driver or kernel modification is permitted.
- No heap policy, descriptor allocator policy/layout, ring size, BO retention, shader scheduling, Kbase ABI, kernel, or Vulkan extension exposure was changed.


## 2026-11-15 15:01 UTC — exact-head CI completion and Android package audit

- Rechecked exact driver-source HEAD `73873d730110bf3394ceea19b2a4d45ddba73834`, which contains the C11 `call_once` hardening for descriptor-profile, heap-renewal, Kbase-profile, and transient-BO-profile option caches.
- Exact-head Android-only workflow run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) completed SUCCESS. All 15/15 job steps passed, including Android ARM64 libdrm build, Meson configuration, driver compilation/staging, dependency and manifest audit, flat package creation, and artifact upload.
- Exact-head wrapper workflow run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) completed SUCCESS. Release and Debugoptimized jobs each passed all 12/12 steps. These are wrapper build results, not proof that the wrapper ZIPs are complete standalone runtime packages.
- Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, was downloaded and independently inspected. Outer ZIP integrity passes. The nested flat package SHA-256 is `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; nested ZIP integrity and all three embedded member hashes pass.
- The nested package contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Both shared libraries are ELF64 little-endian AArch64 Android API 30 binaries. The driver RUNPATH is `$ORIGIN`; `libpanvk_drm.so` is bundled and depends only on Android `libc.so`. The audit's `missing-drm-symbols.txt` is empty. The staged ICD manifest is present and declares Vulkan API 1.4.354.
- Package metadata explicitly labels this driver experimental and non-conformant; device and game compatibility remain unverified. Static package validity is not a device runtime test.
- No Android device import, Vulkan initialization, game launch, heap-threshold A/B, or FPS benchmark was performed in this audit. Preserve the 65,536 compiled work-threshold default and all existing profiling-off requirements for clean FPS runs. Next gate is a controlled import through Winlator Mali's custom-driver importer, preserving System Driver as fallback, followed by runtime initialization and the documented 65K/262K environment-override test.
- No driver source, allocator policy, descriptor layout, heap topology, queue sizing, Kbase ABI, kernel, Android system/vendor Vulkan driver, or live device configuration was changed by this audit.


## 2026-11-16 14:05 UTC — scheduled source, CI, artifact, and benchmark revalidation

### Branch and source provenance
- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit` before inspection.
- Pre-update audit branch HEAD: `8a0cd707e9ca74c1702d36cd46b0572d73195d8e`. Current executable PanVK source HEAD remains `73873d730110bf3394ceea19b2a4d45ddba73834`. GitHub compare from source HEAD to the pre-update audit HEAD reports 2 commits ahead / 0 behind; the only changed path is this audit Markdown file. No executable source or workflow change has landed after `73873d73`.
- The C11 `call_once` changes remain the latest source work: descriptor-profile and heap-option initialization in `d6557723d9c53e717caf098e98e7a7263d081fa2`; Kbase-profile and transient-BO-profile initialization in `73873d730110bf3394ceea19b2a4d45ddba73834`. No additional source change was made during this pass.

### Source audit
- Re-read `src/panfrost/vulkan/panvk_vX_descriptor_set.c` around `panvk_desc_pool_allocate_set()`. The five opt-in failure buckets remain overflow, no-free-set-only, heap-capacity-only, both-capacity, and VMA allocation failure. Overflow/capacity continue returning `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure continues returning `VK_ERROR_FRAGMENTED_POOL`. The aggregate counter remains. The extra heap-capacity read is only made after a failure and only when descriptor profiling is enabled; normal allocation behavior is unchanged.
- The layout descriptor count is an unsigned 32-bit field; variable-count arithmetic is widened to 64 bits, and the existing `base_descs <= num_descs` layout invariant bounds the subtraction used by the overflow guard. No new arithmetic or Vulkan-semantics defect was identified in this pass. The five relaxed-atomic counters should be compared with the aggregate at a quiescent final snapshot; concurrent snapshots can be transiently inconsistent.
- Re-read the Kbase option helper. `PANVK_KBASE_HEAP_RENEW_WORK` is parsed once through `call_once`, defaults to 65,536, and is logged as the effective threshold. Interval renewal remains independently defaulted to 128 submissions. Non-positive work values disable only the work trigger. The log does not reveal the value's origin; because it is cached at first use, set the environment before the first Kbase submission and restart between variants.
- All four process-wide option caches (descriptor profile, heap-renewal options, Kbase profile, transient-BO profile) use C11 `once_flag` / `call_once`. This is concurrency hardening, not a measured performance optimization.

### Exact-head CI and Android artifact
- Android-only workflow [run 43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), source `73873d730110bf3394ceea19b2a4d45ddba73834`: SUCCESS, all 15/15 steps passed.
- Wrapper workflow [run 43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), same source: SUCCESS; Release and Debugoptimized each passed all 12/12 steps. These wrapper ZIPs remain raw ELF outputs and are not complete standalone runtime packages.
- Downloaded Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), 16,747,780 bytes. Outer ZIP SHA-256: `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`; ZIP integrity passed.
- Nested flat Winlator package SHA-256: `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Nested ZIP integrity and all three embedded member hashes passed. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`.
- Metadata identifies `26.3.0-devel-android-kbase-73873d730110`, minimum Android API 30, experimental/non-conformant, with runtime compatibility unverified. Both libraries are ELF64 little-endian AArch64/API 30. The driver uses `$ORIGIN` RUNPATH and bundles `libpanvk_drm.so`, whose only NEEDED dependency is Android `libc.so`. The 24 undefined DRM imports resolve against the private library's 212 defined symbols; `audit/missing-drm-symbols.txt` is empty. The outer staged tree also contains a valid ICD manifest (format 1.0.1, Vulkan API 1.4.354). Static package validation passed; no device import or runtime test was performed.
- A verified flat package copy was materialized at `/mnt/data/PanVK-G720-73873-flat.zip` with SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. This package is suitable for the next controlled Winlator test, not a claim of device compatibility.

### Benchmark evidence and next gate
- No new Tomb Raider benchmark archive, threshold startup log, descriptor-profile capture, or FPS result was found in current conversation uploads or the Library search. Recent uploads are repeated older PanVK package ZIPs; the newly downloaded file is the CI package, not a game log.
- Keep the existing results inconclusive: clean 65K baseline 9.0/17.6/13.5 FPS; one 262K compiled-default run 7.8/18.9/13.9 FPS; instrumented diagnostic 7.7/19.7/13.6 FPS. The compiled-default 262K run does not prove Winlator environment propagation; no FPS gain is claimed.
- Next device gate: with this 65K-default package, run uninstrumented baseline with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require log value 65,536; restart and set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before first Kbase submission and require 262,144. If the second log is absent or remains 65,536, the override is unproven and its FPS result is invalid. Keep all profiling variables unset for both FPS runs. Then run a separate `PANVK_DESCRIPTOR_PROFILE=1` diagnostic and capture the five category counters plus aggregate `set_failures`.
- No heap policy, descriptor allocation policy/layout, heap topology, ring sizing, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan feature exposure, or Android system/vendor driver was changed. This documentation-only update uses `[skip ci]`.


## 2026-11-16 13:50 UTC — scheduled Actions and artifact revalidation

- Re-read both required branch-specific documents before this check. The audit branch HEAD before this append was `8a37ae1cb0d103a669ddfc63276d65229128d81a`; the handoff branch HEAD was `75055e53800b540602ba9dfee68413ba132d3f6f`. Executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. GitHub compare reports 3 commits ahead / 0 behind from source to audit HEAD, with only this audit Markdown file changed. No executable driver or workflow change was found.
- The active-branch Actions listing reports 139 runs. No newer Android-only or wrapper run has superseded the exact-source pair:
  - Android-only [run 43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`: SUCCESS. All 15 returned steps completed, including checkout/verification, Python and dependencies, host tools, Android ARM64 libdrm, Meson configure, compile/stage, driver/manifest/dependency audit, flat-package creation, upload, and post-job cleanup. Pending 0; failed 0.
  - Wrapper [run 43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), Release job `132819003046`: SUCCESS, all 12 returned steps completed. Debugoptimized job `132819003226`: SUCCESS, all 12 returned steps completed. Both completed checkout, `memfd_create` compatibility patch, Python/dependencies, host tools, Meson configure, compile, upload, and post-job cleanup. Pending 0; failed 0.
  - No failed job or step exists in the latest pair, so failure-log diagnosis was not applicable. No build or rerun was triggered by monitoring.
- Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), 16,747,780 bytes: downloaded again. Outer ZIP SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8` matches the GitHub artifact digest; ZIP integrity passes. Nested `PanVK-G720-experimental.zip` SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; nested ZIP integrity passes. Exactly four members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three embedded member hashes independently pass. Metadata is valid JSON and identifies `26.3.0-devel-android-kbase-73873d730110`, minimum API 30, experimental/non-conformant. Both libraries are ELF64 little-endian AArch64/API 30. The flat driver has RUNPATH `$ORIGIN`, needs the bundled `libpanvk_drm.so`, and that private library needs only Android `libc.so`. The outer staged tree contains the valid ICD manifest (format 1.0.1, Vulkan API 1.4.354, 64-bit, expected library path) and `stage/usr/android_stub/libdrm.so`. The DRM audit lists 24 undefined driver imports and 212 private-library definitions; `audit/missing-drm-symbols.txt` is empty. Classification: Android package statically/package validated, not device-runtime validated.
- Wrapper Release artifact [14162259759](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162259759), 4,288,198 bytes: outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9` matches GitHub digest; sole ELF member SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`.
- Wrapper Debugoptimized artifact [14162119843](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162119843), 50,651,760 bytes: outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44` matches GitHub digest; sole ELF member SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`.
- Both wrapper ZIPs pass integrity checks and contain only one ELF64 AArch64 Android API 30 `libvulkan_panfrost.so`. Both use RUNPATH `$ORIGIN/../../android_stub` and require `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1` in addition to Android platform libraries. Neither archive includes the referenced stub, the X11/XCB dependency closure, an ICD manifest, or a checksum manifest. Classification: wrapper jobs passed and ELF/ZIP integrity verified; neither wrapper is a complete standalone package.
- No new phone runtime, Vulkan initialization/device-creation, game, heap-override startup log, descriptor-profile capture, or FPS evidence was supplied. Keep the 65K/262K comparison inconclusive; no performance improvement is claimed. Next device gate remains controlled import of the flat Android package through Winlator Mali's custom-driver importer, followed by Vulkan initialization/device creation/stability and the documented 65K-default versus environment-override test. Keep System Driver as fallback; do not modify the Android system/vendor driver or live kernel.
- Timestamp reconciliation: the preceding document heading says 14:05 UTC, while its GitHub commit was authored at 13:10:02Z and the companion handoff commit at 13:10:06Z. The automation runtime reports 14:50 Africa/Tunis (+01:00), i.e. 13:50 UTC. The prior 14:05 UTC label is therefore inconsistent with the commit/runtime timestamps; it is preserved unchanged as historical text. This entry uses the runtime-derived 13:50 UTC timestamp.
- No source, allocator policy, descriptor layout, heap topology, ring sizing, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan feature exposure, Android system/vendor Vulkan driver, or live device configuration was changed. This documentation-only append uses `[skip ci]`.


## 2026-11-16 — scheduled source, feature and artifact audit

### Active source and exact-head CI

- Re-read the canonical handoff and this active audit document before inspection. Current `experiment/g720-performance-audit` HEAD is `1e2eff6e1b9cd4a80e6c3c34b53f79e58509f620`; the latest executable PanVK source commit remains `73873d730110bf3394ceea19b2a4d45ddba73834`. GitHub compare reports 4 commits ahead / 0 behind from source to audit HEAD, with only this audit Markdown file changed.
- Exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968): SUCCESS. Exact-source wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955): Release and Debugoptimized SUCCESS.
- Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171): outer ZIP SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`; nested flat package SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. ZIP integrity and embedded member hashes pass. Metadata is experimental, min API 30. Both libraries are ELF64 AArch64 Android API 30; driver RUNPATH `$ORIGIN`; bundled private `libpanvk_drm.so` has libc-only dependency. All 24 undefined DRM imports resolve against 212 private definitions; missing-symbol report empty. Static/package validated only.
- Wrapper Release and Debugoptimized artifacts are valid ZIPs with one ELF64 AArch64 Android API 30 library each. Their RUNPATH is `$ORIGIN/../../android_stub`; they require external `libdrm.so`, X11/XCB and `libxshmfence.so.1`, none of which is included in the raw ELF-only wrapper ZIP. Wrapper build success is not a complete standalone runtime package.

### New source correctness finding: Kbase timestamp capability advertisement

The active source commit `73873d7` sets `gpu_can_query_timestamp = (gpu_id != 0)` in `kbase_dev_query_props()` while explicitly leaving `timestamp_frequency = 0`. `panvk_get_gpu_system_timestamp_period()` returns 0 when the frequency is absent, but the same source still advertises KHR/EXT calibrated timestamps, KHR shader clock, shader clock features, `timestampComputeAndGraphics`, and queue-family `timestampValidBits = 64` based on the optimistic boolean/coherency flag. That is an internally inconsistent GPU timestamp domain; it risks exposing timestamp features with `timestampPeriod == 0`.

A narrow isolated fix was implemented on `work/kbase-timestamp-capability-fix`, HEAD `f5084ae0ebc8a4a8e38ac4007a6ca96daffe96b9`:

- Kbase now reports GPU timestamp queries unavailable until the hardware counter frequency and relationship to Kbase timeinfo are validated; `timestamp_frequency` remains 0.
- Calibrated-timestamp and shader-clock extensions/features, present timing, `timestampComputeAndGraphics`, and queue timestamp bits are gated on a valid nonzero frequency and non-Kbase node path.
- Android-only checkout guards assert the conservative Kbase capability policy.

Exact-head Android-only run [43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540): SUCCESS. Exact-head wrapper run [43900657634](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657634): Release and Debugoptimized SUCCESS. Android artifact [14240094671](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540/artifacts/14240094671) was downloaded and audited: outer ZIP SHA-256 `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`; nested flat package SHA-256 `28752b6aa0ec594af9f6334fbe61e4d1211b46b68b15d98f1b83c86b249265e9`; metadata identifies `26.3.0-devel-android-kbase-f5084ae0ebc8`, min API 30; both libraries are ELF64 AArch64 Android API 30; driver RUNPATH `$ORIGIN`; private DRM helper bundled; 24 imports/212 definitions/zero missing. This is a static/package-validated candidate, not device-runtime validated. The branch remains isolated; no merge or release.

### VRS dynamic sysval and sample-count audit

- `experiment/g720-dynamic-fsr-sysval` source checkpoint `c4f091e86c29eae88d2a659813d80b416f95fbf0` is a descendant of the earlier VRS implementation. Its current branch HEAD is a documentation-only follow-up `a459d564da4597975645099940d7b3fd554b552d`; executable source remains `c4f091e`.
- The per-draw `fs.frag_rate` FAU sysval is implemented. Dynamic fragment-centre lowering reads it; draw preparation derives it from dynamic FSR state; sample shading forces 1x1; only KEEP/KEEP is supported; shader cache hashing separates dynamic and static variants. Source review found calls from the inspected CSF ordinary, indirect, tessellation, geometry and software-stage draw paths.
- Exact-head Android-only [42645674486](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674486), wrapper [42645674848](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674848), and Windows probe [42645674514](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674514) all succeeded at source `c4f091e`. Android artifact [13688885153](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674486/artifacts/13688885153) outer SHA-256 `fbfa977275e8d54ffe176c8ff9999ff706166ae7b55b41a6a553975e8aca4fc2`; nested package SHA-256 `02691a2f06125d4ffd7d0725fb31c99a5d54bc1f4b32b819cfb57fc0d800b7ff`. Package/ELF/dependency checks pass; no runtime test followed the sysval patch.
- The VRS rate query has a separate static inconsistency: `2x4` and `1x2` include 2x MSAA despite a comment claiming those rates are 1x-only; the 1x1 entry uses `~0u` instead of the device's actual framebuffer sample-count mask.
- Conservative correction is isolated on `work/vrs-rate-sample-count-fix`, HEAD `385e51bb951829e95c17d55410855f9cb982ca80`. It derives 1x1 sample counts through `panvk_get_sample_counts()`, restricts 2x4/1x2 to 1x MSAA pending device evidence, and adds CI guards. Android-only run [43901478216](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43901478216) and wrapper run [43901478331](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43901478331) were still in progress at this audit checkpoint; the Windows probe-build job in the wrapper workflow had succeeded. Do not use its package until exact-head Android and both wrapper jobs finish and the artifact is audited.
- The c4f probe artifact [13685924631](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674514/artifacts/13685924631) outer SHA-256 `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`; PE32+ x86-64 executable SHA-256 `5e0526c4735fed05a83647825a210653bf83740dc403a94128a8141d6e9a639b`. It includes `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd`. Neither has been run on the target device after the sysval change. A same-command-buffer dynamic 1x1-to-2x2 transition and sample-shading fallback still need explicit runtime coverage.

### Other branches and feature exposure

- `experiment/g720-descriptor-heaps` HEAD `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`: exact-head Android and wrapper builds succeeded. Android package artifact [13441682356](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42082198442/artifacts/13441682356) outer SHA-256 `0a4f6b7899f3df42a89133f071b2c441e62739e8000ad55bdf14af7329fccf71`; nested package SHA-256 `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6`; member checks pass, AArch64/API 30, private DRM dependency closure present. This branch is divergent and adds substantial `VK_EXT_descriptor_heap` lowering/advertisement and shader-binary metadata; no device runtime test exists. Keep isolated.
- `experiment/g720-clip-cull-capabilities` HEAD `9188e7bf92a032431ca16ac02d54d81be5a5e12e`: Android, wrapper and NIR clip/cull compiler tests succeeded; clip/cull feature exposure remains disabled pending stage-path/runtime evidence.
- `experiment/csf-notification-serialization` and `experiment/csf-sync-wait64-csg-scope` exact-head Android/wrapper builds succeeded and their source commits are ancestors of current source `73873d7`; do not treat them as unmerged parallel fixes.
- `experiment/g720-heap-renew-262k` exact-head Android/wrapper builds succeeded, but no new device startup log or benchmark exists. Keep the 65K/262K FPS comparison inconclusive.
- `agent/free-panvk-agent` remains infrastructure-only at `49599fe7c85da3a06c557592cdeb55baf31a95e7`; Android and wrapper CI succeeded, but no Termux diagnostic bridge or device runtime integration is established.
- No `VK_KHR_ray_query` or acceleration-structure extension is advertised by the inspected PanVK physical-device extension table. Do not expose ray query based only on the GPU model; the driver implementation path is absent.
- No new branch names existed before this audit. The two new isolated work branches are `work/kbase-timestamp-capability-fix` and `work/vrs-rate-sample-count-fix`.

### Device evidence and next gate

No new phone-side logs, Vulkan initialization/device-creation result, Winlator import, benchmark archive, heap-threshold startup log, descriptor-profile capture, or FPS result was available. No direct Android/Termux/Winlator control bridge exists. Existing 65K/262K FPS observations remain inconclusive; no performance gain is claimed.

Next device action after the VRS sample-count branch's exact-head CI and package audit: import that package only through Winlator Mali's custom-driver importer, run `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd`, and provide the complete `panvk_vrs_render_probe.log`. Keep System Driver as fallback. Never modify the Android system/vendor Vulkan driver or kernel.


### VRS sample-count correction retry addendum (2026-11-16)

The initial `work/vrs-rate-sample-count-fix` commit `385e51b` failed Android and wrapper compilation because `sample_counts` was runtime-derived but the local rate array was `static const`. The array is now automatic `const` storage at branch HEAD `c22dc2f595046ddf640ce8c148453f230cde7cfc`.

Exact-head retry runs [43902597004](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902597004) (Android-only) and [43902595797](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902595797) (wrapper plus probe) are pending at this addendum. The Windows probe-build job has succeeded; Android, Release and Debugoptimized compilation have not yet completed. No package is available from the corrected head at this point.


## 2026-11-16 16:20 UTC — follow-up audit: timestamp gating and VRS sample-count limits

### Active branch and CI

- Active branch pre-update HEAD: `6738002070997bd62c8c732eeb8901641fb87330`; executable source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Compare is 6 ahead / 0 behind, with only this audit document changed.
- Exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) and wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) remain successful. Their artifacts are tied to source SHA 73873; Android artifact digest is `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`.
- No PanVK source or workflow change is present on this active performance branch. It still does not implement `VK_KHR_fragment_shading_rate`, `VK_KHR_ray_query`, or acceleration structures.

### Isolated Kbase timestamp capability correction

Branch `work/kbase-timestamp-capability-fix`, HEAD `f5084ae0ebc8a4a8e38ac4007a6ca96daffe96b9`, contains three conservative source commits plus CI guards:

- Kbase reports `gpu_can_query_timestamp=false` and `timestamp_frequency=0` until the hardware counter frequency/domain is validated.
- Calibrated timestamp, shader clock, present timing, timestamp compute/graphics, and queue timestampValidBits exposure are gated on a valid nonzero frequency and a non-Kbase path.
- Android run [43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540) and wrapper run [43900657634](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657634) succeeded.
- Android artifact 14240094671 outer SHA-256: `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`; nested package SHA-256: `28752b6aa0ec594af9f6334fbe61e4d1211b46b68b15d98f1b83c86b249265e9`. Embedded hashes pass; metadata `26.3.0-devel-android-kbase-f5084ae0ebc8`, min API 30; driver and private DRM helper ELF64 AArch64/API 30; RUNPATH `$ORIGIN`; 24 undefined DRM imports are resolved by 212 helper definitions; no missing symbols. Wrapper artifacts are raw ELF-only with external X11/XCB/xshmfence dependencies, not standalone packages. No target-device timestamp test exists. Keep this branch isolated.

### VRS sample-count correction and second-order property bug

The first correction branch `work/vrs-rate-sample-count-fix`, HEAD `c22dc2f595046ddf640ce8c148453f230cde7cfc`, successfully builds but is **not a device candidate**.

- Its Android run [43902597004](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902597004) and wrapper/probe run [43902595797](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43902595797) succeeded.
- Package artifact 14243545386 outer SHA-256 `dea579bee3791ec8e4043dd79de6741abcfc72d8e165d761e8a6a3ca5152c605`; nested flat package SHA-256 `b607254ecfc0fa95eda013ec8108967c5c8ccdcd23123dd95e0e5b3b001f8de3`. ZIP integrity and all embedded hashes pass; metadata `26.3.0-devel-android-kbase-c22dc2f59504`, min API 30; AArch64/API 30; driver RUNPATH `$ORIGIN`; private DRM closure complete; 24 imports/212 definitions/zero missing.
- The package still advertises 8x/16x sample bits in the 1x1 VRS entry while the VRS properties advertise `maxFragmentShadingRateRasterizationSamples=4x`. The Vulkan VRS query valid-usage rule requires every entry's highest sample count to be no greater than this property. See [VRS properties](https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceFragmentShadingRatePropertiesKHR.html) and [query rules](https://docs.vulkan.org/spec/latest/chapters/primsrast.html).
- This is explained by G720's 32/64 KiB TIB, max eight color attachments, and `pan_get_max_msaa(..., 16)`, which yields 16x (capped at 16) and therefore causes `panvk_get_sample_counts(..., 16)` to return 1x/2x/4x/8x/16x. The prior fix removed `~0u` but did not intersect the result with the advertised 4x VRS limit.

A conservative follow-up branch was created: `work/vrs-sample-count-rasterization-limit`, HEAD `b1525b9a33923876f724a742aaebc7a7325a2a58`, based on c22. It masks the 1x1 VRS sampleCounts to 1x/2x/4x, updates the source comment, and adds Android workflow assertions plus a branch filter.

- Android-only exact-head run [43906343930](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343930) completed successfully; all 15 steps passed, including compile, staged install/dependency audit, flat package generation, and upload. Artifact 14245318290 outer SHA-256: `ab4055b70929a2a64dbdec88ce6106ddf8e8709de61b5bb2837e4824255ecc1a`; flat package SHA-256: `03d35c615bcd605a87cddfcaf3b9de9d90980f2f0ca6469fb5d05266176a5013`. ZIP integrity and all embedded hashes pass; four flat members; metadata `26.3.0-devel-android-kbase-b1525b9a3392`, min API 30; driver/helper ELF64 AArch64/API 30; driver RUNPATH `$ORIGIN`; helper needs only libc; 24 DRM imports are resolved by 212 helper definitions; zero missing symbols; staged ICD reports API 1.4.354.
- Wrapper/probe exact-head run [43906343923](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343923) completed successfully: capability probe plus Release and Debugoptimized wrapper jobs all passed (12 steps per wrapper job). Release artifact 14246900452 outer SHA-256 `b58e93fa48628ce5c888fb335308e3d607d8731bf8f6b6c760e44f09a280779d`; one raw ELF64 AArch64/API 30 `libvulkan_panfrost.so` (SHA-256 `899ba2f9d5772d6053009094ab322c7ca2a671c04f1d4dffbd1d11f0bd9d645c`). Debugoptimized artifact 14245188335 outer SHA-256 `4cc41f39af9e3b60e3804d978fac9fd63bb6b76d713c2de794ff87d772c495bc`; one raw ELF64 AArch64/API 30 `libvulkan_panfrost.so` (SHA-256 `793ce367792802e6da497defd89da13d19fdd0cc13ec408027cb784619602610`). Both use RUNPATH `$ORIGIN/../../android_stub` and require external libdrm, X11/XCB, and xshmfence; these are raw build outputs, not standalone packages. Capability-probe artifact 14244179654 is a single PE32+ x86-64 capability executable, not the render probe.
- The corrected Android package is now both exact-head CI-verified and statically audited, but remains device/runtime-unverified. Do not import c22. A controlled Winlator custom-driver probe is now the next action; keep System Driver as fallback.

The correct Winlator **render** probe is separate from the smaller capability probe: artifact 13685924631 from run 42645674514. Re-downloaded ZIP SHA-256 `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`; it contains the PE32+ x86-64 render executable (SHA-256 `5e0526c4735fed05a83647825a210653bf83740dc403a94128a8141d6e9a639b`) and the dynamic/mip-pattern runner scripts. This probe source is unchanged between c4f and the sample-count follow-up.

### Source review and device evidence

- Descriptor allocation still performs variable-count overflow checks, 64-bit size calculation, free-set and heap-capacity checks, VMA failure handling, and mutually exclusive opt-in failure counters. Existing Vulkan error results remain unchanged.
- Descriptor and Kbase profile/heap option initialization uses C11 `call_once`; heap-renew defaults remain interval 128 and work threshold 65,536. Non-positive work threshold disables only work-triggered renewal; the interval trigger remains independent. Environment values must be set before the first Kbase submission and process restart is required between variants.
- Production SYNC_ADD64 remains SYSTEM-scope. `PANVK_KBASE_SYNC_ADD64_CSG_SCOPE` is an opt-in diagnostic switch and must remain disabled until target hardware validates cross-CSG visibility. CQS completion/error-event handling remains as previously audited; no new queue-source change was found.
- The old `work/android-libdrm-runtime-audit` branch is stale (last commit 2026-09-01), diverges by five unique commits, and includes older MC8/tessellation work with a temporary early-return runtime isolation path. No recent CI exists; do not cherry-pick it.
- No new phone-side logs, Vulkan device-creation result, Winlator import, benchmark archive, heap-threshold startup log, descriptor-profile capture, or FPS measurement was available. No direct Android/Termux/Winlator bridge exists. No system/vendor driver or kernel was modified.

Next device action only after the follow-up branch builds and its package passes static audit: import the `work/vrs-sample-count-rasterization-limit` package through Winlator Mali's custom-driver importer, keep System Driver as fallback, run `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd` from the verified render-probe ZIP, and provide the complete `panvk_vrs_render_probe.log`. Do not import the c22 package.


## 2026-11-17 12:42 UTC — scheduled source, CI, and package revalidation

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit` before inspection.
- Pre-update performance-audit HEAD: `52979bac6c687b1da55a4f2519e65ec9206e90e8`. Compare from executable source `73873d730110bf3394ceea19b2a4d45ddba73834` to the branch reports 9 ahead / 0 behind; the only changed path is this performance-audit Markdown file. No PanVK source or workflow change exists after source commit `73873d7`.
- Re-read the descriptor allocator at `src/panfrost/vulkan/panvk_vX_descriptor_set.c`. The five opt-in, mutually exclusive failure categories remain: overflow; no-free-set only; heap-capacity only; both capacity guards; and VMA allocation failure. The aggregate `set_alloc_failures` counter remains. Overflow/capacity return `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA allocation failure returns `VK_ERROR_FRAGMENTED_POOL`. The extra heap-capacity read for the “both” split remains inside the profiling-enabled failure path, not the allocation fast path. Counters use relaxed atomic operations; a concurrent live log is cumulative but not a globally synchronized snapshot, so compare the category sum to the aggregate only after activity is quiescent. Profiling remains opt-in and diagnostic-only.
- Re-read Kbase renewal code at `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. C11 `call_once` initializes the interval/work pair once. Defaults remain 128 tiler submissions and 65,536 work units. The effective work threshold is logged once; the log does not identify whether the value came from the environment or compiled default. Set the environment before the first Kbase submission and restart the game process between variants. A 262,144 log from the separate compiled-262K branch is not evidence of Winlator environment propagation.
- Exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) remains SUCCESS, 15/15 steps. Exact-source wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) remains SUCCESS: Release and Debugoptimized each 12/12 steps. Android artifact 14163230171 is unexpired; GitHub outer artifact digest is `sha256:ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`. Wrapper artifacts 14162259759 and 14162119843 are raw ELF-only ZIPs with external dependencies, not standalone driver packages.
- The newly attached `PanVK-G720-73873-flat.zip` is byte-identical to the previously audited exact-source flat package: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Independent checks this run: ZIP integrity passes; exactly four flat members; metadata identifies `26.3.0-devel-android-kbase-73873d730110`, experimental/non-conformant, minimum API 30; embedded hashes pass; both libraries are ELF64 little-endian AArch64; driver RUNPATH is `$ORIGIN`; private `libpanvk_drm.so` is bundled; 24 undefined DRM/GBM imports resolve against 212 private dynamic exports (zero unresolved). The CI staged-install audit reports a valid ICD manifest. This remains static/package validation only; no Winlator runtime validation.
- No new Tomb Raider benchmark archive, startup-threshold log, descriptor-profile capture, or FPS measurement was present in the current uploads or Library search. The mounted files were package ZIPs only. Retain the historical clean 65K baseline (9.0 / 17.6 / 13.5 FPS), single compiled-262K candidate (7.8 / 18.9 / 13.9 FPS), and instrumented diagnostic (7.7 / 19.7 / 13.6 FPS) as inconclusive; no performance gain is claimed. Historical telemetry remains 4,068 heap renewals, 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO allocation/creation failures, and 299 aggregate descriptor failures from a pre-split capture.
- Exact next device gate: import only the verified 65K-default `73873d73` package through Winlator's separate custom-driver importer, retaining System Driver as fallback. With `PANVK_KBASE_HEAP_RENEW_WORK` absent before first Kbase submission, restart and require the threshold log to show 65,536. Restart again with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` set before first submission and require 262,144. If absent or 65,536, mark the override unproven. Keep all profiling variables unset for matched/repeated FPS runs; then use a separate `PANVK_DESCRIPTOR_PROFILE=1` run to capture the five failure buckets and aggregate after workload quiescence.
- No source, allocation policy, descriptor layout, heap topology, ring size, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan extension exposure, Android system/vendor driver, or live device configuration was changed. This entry records revalidation only.


## 2026-11-17 12:55 UTC — scheduled exact-source Android/wrapper CI and artifact revalidation

- Re-read the canonical continuity handoff and this audit before inspection. Pre-update active branch HEAD is `27ce8c148a4e64e68e8d600e20d2a885197a1381`; executable driver-source commit remains `73873d730110bf3394ceea19b2a4d45ddba73834`. GitHub compare reports 10 commits ahead / 0 behind and only this audit Markdown file changed. No driver-source or workflow change is present after the tested source commit.
- Branch-filtered Actions listing reports 139 runs (100 newest returned). Latest relevant Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, SUCCESS, all 15 returned steps completed. Main steps include checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, driver/manifest/dependency audit, flat package creation, and upload; post/cleanup steps also completed.
- Latest wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955): Release job `132819003046` and Debugoptimized job `132819003226` both SUCCESS, 12/12 returned steps each. Compatibility patch, dependencies, host tools, Meson configuration, compilation, upload, and cleanup all passed. Pending 0; failed 0; no failure logs were needed. Both latest runs target exact source SHA `73873d730110bf3394ceea19b2a4d45ddba73834`.
- Android artifact 14163230171 is unexpired (16,747,780 bytes). GitHub digest and independently recomputed outer ZIP SHA-256 match: `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`. Nested flat package SHA-256: `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. ZIP integrity passes; the flat package contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; all three embedded hashes pass. Metadata is valid JSON, version `26.3.0-devel-android-kbase-73873d730110`, minimum API 30, explicitly experimental/non-conformant. Both libraries are ELF64 little-endian AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; its private `libpanvk_drm.so` is bundled and depends only on `libc.so`. All 24 audited undefined DRM imports resolve against the private helper's 212 dynamic definitions; `audit/missing-drm-symbols.txt` is empty.
- The outer staged tree separately includes `stage/usr/android_stub/libdrm.so` and a valid ICD manifest (API 1.4.354, 64-bit, staged library path). These staged files are not members of the nested four-file flat package; the flat package uses the private DRM helper.
- Wrapper Release artifact 14162259759 outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`; Debugoptimized artifact 14162119843 outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`. ZIP integrity passes. Each archive contains only `libvulkan_panfrost.so`, ELF64 AArch64 Android API 30. Release ELF SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`; Debugoptimized ELF SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`. Both use RUNPATH `$ORIGIN/../../android_stub` and require external `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1`; neither archive includes that dependency closure, an ICD manifest, or checksum manifest. Treat as raw ELF outputs, not standalone import packages.
- No new Winlator/Termux runtime log, Vulkan initialization/device-creation result, heap-threshold startup log, descriptor-profile capture, Tomb Raider benchmark archive, or FPS measurement was supplied. Preserve existing 65K/262K results as inconclusive; no performance improvement is claimed. The next gate remains controlled import of the verified 65K-default Android flat package through Winlator's custom-driver importer, with System Driver retained as fallback, followed by Vulkan initialization/device creation and stability evidence. Heap override A/B and descriptor profiling remain separate diagnostic runs.
- No PanVK source, workflow, allocator policy, Kbase ABI, kernel, Android system/vendor Vulkan driver, or live device configuration was changed. This entry records monitoring and package revalidation only; the documentation commit uses `[skip ci]`.


## 2026-11-17 15:24 UTC — descriptor-heap branch state invalidation audit

- Re-read the canonical handoff and this active audit before checking the repository. This is a cross-branch correctness review; the active performance-audit driver source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Pre-update audit-doc HEAD was `14e9bb46e5afc38c011c5eb9f020db0c65fdc8a2`; no PanVK source change was made on this branch.
- Reviewed the separate `experiment/g720-descriptor-heaps` branch at `04947cb94a9987bbe8f6b98d63142a5b64b1abf3`. Exact-head Android run [42082198442](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42082198442) and wrapper Release/Debugoptimized run [42082198906](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42082198906) succeeded. Android artifact 13441682356 was downloaded and rechecked: outer SHA-256 `0a4f6b7899f3df42a89133f071b2c441e62739e8000ad55bdf14af7329fccf71`; nested flat package SHA-256 `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6`. Both ZIP integrity checks and all three embedded flat-package hashes pass. Metadata is `26.3.0-devel-android-kbase-04947cb94a99`, minimum Android API 30. Both libraries are ELF64 AArch64/API 30; the flat driver uses RUNPATH `$ORIGIN` and NEEDED `libpanvk_drm.so`; the helper depends only on `libc.so`. The artifact's missing-DRM-symbol report is empty. This is static/package validation only, not target-device validation.
- **Source-level correctness finding; branch must remain isolated:** `src/panfrost/vulkan/panvk_vX_cmd_desc_state.c` stores descriptor heaps in both graphics and compute descriptor states. `cmd_bind_desc_heap()` updates both heap slots but does not invalidate existing bound descriptor-set/push-descriptor state. Conversely, `cmd_desc_state_bind_sets()` and `cmd_get_push_desc_set()` update descriptor-set state without invalidating either bound heap. The `VK_EXT_descriptor_heap` specification states that heap binding invalidates all descriptor-set/descriptor-buffer/offset state, and setting descriptor-set or descriptor-buffer state immediately invalidates all descriptor heaps: [Khronos VK_EXT_descriptor_heap proposal](https://docs.vulkan.org/features/latest/features/proposals/VK_EXT_descriptor_heap.html). The source therefore retains stale state across heap↔descriptor-set mode switches; a later pipeline can consume an old heap or set without the required rebind. This is a source/spec finding; no device execution sequence has yet demonstrated it.
- Required follow-up before merging or importing this branch: implement command-buffer-wide invalidation in both directions, include push-descriptor paths, mark both graphics and compute descriptor state dirty when global heap state changes, and add regression coverage for heap → descriptor sets → heap and descriptor sets → heap → descriptor sets. Re-run exact-head Android and wrapper CI and then validate on target. No fix or merge was made in this audit.
- The descriptor-heap experiment exposes `VK_EXT_descriptor_heap`/feature on `PAN_ARCH >= 9`, with capture/replay, sparse and protected heaps disabled and 32-byte descriptor records. It binds application-provided heap addresses; it does not modify Kbase memory-mapping wrappers or replace the classic descriptor-pool allocator's single backing-BO allocation. It therefore does not by itself prove that oversized classic descriptor pools are safe. Acceleration-structure descriptor writes explicitly return `VK_ERROR_FEATURE_NOT_PRESENT`; ray query/acceleration-structure support remains unimplemented. No device runtime or benchmark evidence exists for this branch.
- No system/vendor Vulkan driver, kernel, or live device configuration was changed. Keep this branch separate from the active performance and VRS experiments.


## 2026-11-18 12:51 UTC — scheduled active-branch CI and artifact revalidation

- Re-read this handoff and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` from their required branches before inspection. Active performance-audit branch HEAD before this append: `d599322e22ffb6498cfa6c7bf28c5f0c2130bfdc`. Comparing executable source `73873d730110bf3394ceea19b2a4d45ddba73834` to the active branch reports 13 ahead / 0 behind; the only changed path is this audit Markdown file. No driver-source or workflow change is present after the tested executable source.
- Queried the first 100 newest repository Actions runs (1,157 total) and filtered by exact head branch `experiment/g720-performance-audit`. No newer Android-only or wrapper build exists after 15 November. Latest active-branch Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`: completed / success; all 15 returned steps completed successfully (checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, driver/manifest/dependency audit, flat-package creation, artifact upload, and post/cleanup steps). Latest wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955): Release job `132819003046` and Debugoptimized job `132819003226` both completed / success, 12/12 returned steps each (checkout, `memfd_create` compatibility patch, Python/dependencies, host tools, Meson configuration, compile, artifact upload, post/cleanup). Pending 0; failed 0; no failure logs required. Both runs target exact source SHA `73873d730110bf3394ceea19b2a4d45ddba73834`. Original IDs 41947103034/41947103085 remain superseded.
- Freshly downloaded Android artifact 14163230171 (16,747,780 bytes). GitHub digest and independently recomputed outer ZIP SHA-256 match: `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`. Outer ZIP integrity passes. Nested flat package SHA-256: `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; nested ZIP integrity and all three embedded SHA256SUMS entries pass. Exactly four flat members: `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`. Metadata identifies `26.3.0-devel-android-kbase-73873d730110`, min API 30, explicitly experimental/non-conformant. Both libraries are ELF64 little-endian AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; its private `libpanvk_drm.so` is bundled and depends only on Android `libc.so`. The outer staged tree separately contains `stage/usr/android_stub/libdrm.so` and a valid ICD manifest (API 1.4.354, 64-bit, staged library path); these are not members of the nested flat ZIP. Audit lists 24 undefined DRM imports; all resolve against the bundled helper and `audit/missing-drm-symbols.txt` is empty. Static/package validated only; no device-runtime validation.
- Freshly downloaded wrapper Release artifact 14162259759: outer ZIP SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`; extracted ELF SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`. Debugoptimized artifact 14162119843: outer ZIP SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`; extracted ELF SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`. Both ZIPs pass integrity; each contains only one ELF64 AArch64 Android API 30 `libvulkan_panfrost.so`. Both use RUNPATH `$ORIGIN/../../android_stub` and require external `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1`; archives omit the stub/dependency closure, ICD manifest, and checksum manifest. They remain raw ELF outputs, not standalone packages.
- No new Winlator/Termux runtime log, successful custom-driver import, Vulkan initialization/device-creation result, heap-threshold override proof, descriptor-profile capture, or Tomb Raider benchmark/FPS data was available. Next active-source device gate remains importing only the verified Android flat package through Winlator Mali's custom-driver importer with System Driver retained as fallback, then recording Vulkan initialization, physical-device enumeration, logical-device creation, and stability. Keep heap A/B, descriptor profiling, and uninstrumented FPS runs separate.
- Separate-branch caveat remains: the descriptor-heap branch has a source/spec state-invalidation gap across heap↔descriptor-set/push-descriptor transitions; it must remain isolated until bidirectional command-buffer invalidation and regression coverage are implemented and exact-head CI passes. The active source branch does not thereby gain descriptor-heap or ray-query support. No system/vendor Vulkan driver, kernel, or live device configuration was changed. Monitoring/documentation only; this update uses `[skip ci]` and does not trigger builds.


## 2026-11-18 12:56 UTC — scheduled source, CI, package, and benchmark revalidation

- Re-read the canonical continuity handoff from `docs/project-continuity-handoff-2026-11-03` and this audit from `experiment/g720-performance-audit` before inspection. Pre-update branch heads: audit `3729590e313f08a74f2828ff9d28c19f92f42a70`; handoff `481f8d454c4b41dbe37b9d9e2fd5850e35f89916`.
- Compared executable driver source `73873d730110bf3394ceea19b2a4d45ddba73834` with audit HEAD: 14 commits ahead / 0 behind; the only changed path is `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`. The latest 12 active-branch commits are documentation-only; no PanVK source or workflow change has landed after `73873d7`.
- Re-read `src/panfrost/vulkan/panvk_vX_descriptor_set.c` at the exact source commit. The five opt-in, mutually exclusive set-allocation failure buckets remain: overflow/final-count bound; no-free-set only; heap-capacity only; both capacity guards; and VMA allocation failure. Capacity/overflow still return `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure still returns `VK_ERROR_FRAGMENTED_POOL`. The five relaxed-atomic category counters and aggregate `set_alloc_failures` are unchanged. The additional heap-capacity read for distinguishing the “both” case is still confined to the profiling-enabled failure branch. `set_allocs` counts attempts that pass the count-bound checks, including later capacity/VMA failures. Per-failure profile dumps make this diagnostic-only; compare category sum to aggregate only after workload quiescence because relaxed atomic snapshots are not globally synchronized. No allocator policy, descriptor layout, or Vulkan result changed.
- Re-read `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. The compiled defaults remain `KBASE_TILER_HEAP_RENEW_INTERVAL=128` submissions and `KBASE_TILER_HEAP_RENEW_WORK=65536` work units. C11 `call_once` initializes both options together. Non-positive work values become zero (disabling only the work trigger); interval renewal remains independent. The effective threshold is logged once and cached at first use. The renewal decision requires nonzero estimated tiler work and either interval or work threshold reached. Set `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission and restart the process between variants; a 262144 startup value is required before treating the override as applied.
- Latest exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, remains SUCCESS with all 15 returned steps complete. Latest wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) remains SUCCESS: Release job `132819003046` and Debugoptimized job `132819003226`, 12/12 steps each. Both target exact source `73873d730110bf3394ceea19b2a4d45ddba73834`. Branch-filtered Actions results show no newer Android-only or wrapper run after 15 November; no build was triggered for this documentation-only revalidation.
- Rechecked the newly attached `PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; ZIP integrity and all three embedded checksums pass. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Metadata identifies `26.3.0-devel-android-kbase-73873d730110`, experimental/non-conformant, min API 30. Both libraries are ELF64 little-endian AArch64; driver RUNPATH is `$ORIGIN`; private `libpanvk_drm.so` is bundled and depends only on `libc.so`. The exact-source CI artifact audit reports all 24 undefined DRM imports resolved and a valid staged ICD manifest. The older re-uploaded `PanVK-G720-64c43833-flat.zip` is a separate, stale source package (SHA-256 `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`); do not substitute it for the 73873 package. Static/package validation only; no Winlator runtime validation.
- Repeated the current-conversation and Library search for Tomb Raider benchmark logs; no result was found. The only mounted files are the two PanVK package ZIPs. No new Tomb Raider FPS, heap-threshold startup log, or descriptor-profile capture exists. Keep historical clean 65K (9.0/17.6/13.5 FPS), one compiled-262K candidate (7.8/18.9/13.9 FPS), and instrumented diagnostic (7.7/19.7/13.6 FPS) inconclusive; do not claim a performance gain. Historical telemetry remains 4,068 heap renewals, 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO allocation/creation failures, and 299 aggregate descriptor failures from a pre-split capture.
- Next device gate remains: import only the verified 65K-default 73873 package through Winlator's separate custom-driver importer, retaining System Driver as fallback. With the work variable absent before first Kbase submission, restart and require the threshold log to show 65,536. Restart with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` and require 262,144; otherwise the override is unproven. Keep all profiling variables unset for matched/repeated FPS runs. In a separate process, enable `PANVK_DESCRIPTOR_PROFILE=1`, collect the five category counters and aggregate after the workload becomes quiescent, and verify the sum. First record successful import, Vulkan physical-device enumeration, logical-device creation, and stability before interpreting game FPS.
- No driver source, workflow, allocator policy, descriptor layout, heap topology, ring size, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan extension exposure, Android system/vendor driver, or live device configuration was changed. This is monitoring and source audit only; use `[skip ci]`.


## 2026-11-18 15:39 UTC (16:39 Africa/Tunis) — scheduled full branch, source, CI, and artifact revalidation

- Re-read the canonical handoff and this active audit before inspection. Live GitHub refs: canonical docs branch `docs/project-continuity-handoff-2026-11-03` = `a4d0d08b3208e3c122f754fce7205db1aaa63615`; active audit branch `experiment/g720-performance-audit` = `3e956d36317caf33970080c40112a2b88dbea771`; executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Source-to-active compare is 15 ahead / 0 behind and changes only this audit Markdown file. The docs branch is one documentation-only commit ahead of `481f8d4`, changing only the root handoff. GitHub returned 31 branches and no further page; no additional unrecorded branch was found.
- The branch-filtered Actions query reports 139 runs for the active performance branch. Latest exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) and wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) remain completed / SUCCESS at source SHA `73873d7`; no newer active-branch Android or wrapper run exists. Android job completed all 15 returned steps. Wrapper Release and Debugoptimized jobs each completed all 12 returned steps. These builds validate the executable source commit, not the documentation-only branch tip.
- Re-downloaded Android artifact 14163230171. GitHub digest and independently recomputed outer ZIP SHA-256 both equal `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`; nested flat package SHA-256 is `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Both ZIP integrity checks and all three embedded checksums pass. The flat package contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Metadata marks it experimental/non-conformant, min API 30. Both ELF files are 64-bit little-endian AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; private helper is bundled and depends only on `libc.so`. The staged ICD manifest reports API 1.4.354. The audit lists 24 undefined DRM imports and an empty `missing-drm-symbols.txt`. This remains static/package validation only.
- Re-downloaded wrapper artifacts 14162259759 and 14162119843. Outer ZIP SHA-256 values remain `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9` and `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`. Both contain only one AArch64 Android API 30 `libvulkan_panfrost.so`; RUNPATH is `$ORIGIN/../../android_stub`, with external `libdrm.so`, X11/XCB, and `libxshmfence.so.1` dependencies. They do not include that dependency closure or an ICD/package manifest and remain raw ELF build outputs, not standalone import packages.
- Re-downloaded the corrected VRS sample-limit candidate artifact 14245318290 from exact-head Android run [43906343930](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343930). Outer SHA-256 `ab4055b70929a2a64dbdec88ce6106ddf8e8709de61b5bb2837e4824255ecc1a`; nested flat package SHA-256 `03d35c615bcd605a87cddfcaf3b9de9d90980f2f0ca6469fb5d05266176a5013`. ZIP integrity and embedded hashes pass; AArch64/API 30, `$ORIGIN`, bundled private DRM helper, empty missing-symbol report. Exact-head wrapper/capability-probe run [43906343923](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43906343923) succeeded. Source masks the 1x1 VRS rate sample-count mask to 1x/2x/4x, consistent with `maxFragmentShadingRateRasterizationSamples=4x`; the preceding c22 package still contains the 8x/16x inconsistency and must not be imported.
- Re-downloaded the separate VRS render-probe artifact 13685924631 from run [42645674514](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/42645674514). ZIP SHA-256 `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`; PE32+ x86-64 executable SHA-256 `5e0526c4735fed05a83647825a210653bf83740dc403a94128a8141d6e9a639b`. The bundle includes `Run-Dynamic-VRS.cmd`, static/dynamic runners, and `tools/run-vrs-mip-pattern.cmd`. It is a probe, not a PanVK driver package.
- Re-downloaded the Kbase timestamp-capability candidate artifact 14240094671 from exact-head Android run [43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540). Outer SHA-256 `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`; nested package SHA-256 `28752b6aa0ec594af9f6334fbe61e4d1211b46b68b15d98f1b83c86b249265e9`; ZIP and embedded hashes pass, AArch64/API 30, `$ORIGIN`, private DRM helper, zero missing DRM symbols. Exact-head wrapper run [43900657634](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657634) succeeded. The candidate conservatively disables Kbase GPU timestamps and gates calibrated timestamps/shader-clock advertisement on valid nonzero frequency and a non-Kbase path; it remains isolated and has no device test.
- Re-downloaded descriptor-heap artifact 13441682356 and agent-bootstrap artifact 14066160614; their outer digests match GitHub, their nested flat-package digests remain `2142ecffb957580f317071b9f1c0370029650cade149d10aa62f472fc5de82d6` and `ffc6e25b168cac9bc5fd352c679ef86d08f4c18554cbc0e2f76b9158792541f8`, and embedded hashes pass. Both are AArch64/API 30 flat packages with `$ORIGIN`, bundled private DRM helper, and empty missing-DRM-symbol reports. The descriptor-heap branch still has the documented heap↔descriptor-set/push-descriptor invalidation defect; do not import or merge it. The agent branch remains configuration/bootstrap only; no bidirectional GPT/OpenCode orchestrator or Termux diagnostic bridge exists.
- Source recheck: active Kbase source still sets `gpu_can_query_timestamp=(gpu_id != 0)` while leaving `timestamp_frequency=0`; active extension/feature advertisement still keys calibrated timestamps and shader clock on that boolean. This is a capability inconsistency on the active source; the separate timestamp candidate contains the conservative correction. Kbase completion remains SYSTEM-scope SYNC_ADD64 by default; CSG scope is opt-in only. CQS waits retain the bounded notification fallback and queued-error drain. C11 `call_once` protects process-wide option caches; heap-renewal defaults remain interval 128 and work 65,536. Descriptor-set allocation retains five opt-in failure buckets and existing Vulkan error results; classic non-host-only descriptor pools still allocate one backing BO sized from the pool declaration, so do not silently cap large pools. Kbase reports the raw `shader_present` property; do not hard-code a seven-core compiler dispatch count. The active source does not implement pipeline VRS, `VK_KHR_ray_query`, or acceleration structures. The separate VRS branch exposes pipeline FSR only; primitive/attachment FSR remain disabled.
- Branch separation remains essential: descriptor heaps, dynamic VRS, clip/cull, timestamp correction, heap-renew 262K, and stale libdrm/MC8 experiments have independent bases and must not be merged wholesale. The dynamic-VRS branch is five documentation commits beyond source checkpoint `c4f091e`; the older fragment-shading-rate branch is one documentation commit beyond `c2eb0b6`. The stale `work/android-libdrm-runtime-audit` branch remains 5 ahead / 180 behind the active source and contains unrelated MC8/tessellation changes.
- No new target-device logs, Vulkan initialization/device-creation results, Winlator import result, heap-threshold startup log, descriptor-profile capture, or FPS benchmark was available in this run; Files search found no VRS render log or new Tomb Raider log. No phone/Termux/Winlator control bridge is established. No runtime or performance claim is made. No PanVK source, workflow, kernel, or Android system/vendor driver was changed; this entry is documentation-only and uses `[skip ci]`.
- Next device action: import only the corrected b1525 flat package through Winlator Mali's custom-driver importer while retaining System Driver as fallback; run `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd` from the verified render-probe bundle; return the complete `panvk_vrs_render_probe.log`. This is required to validate the newer dynamic per-draw fragment-center sysval path, which the older successful VRS log predates.


## 2026-11-19 13:11 Africa/Tunis — scheduled CI and package revalidation

- Re-read the continuity handoff and this audit before querying GitHub.
- Active branch pre-update HEAD `582d0799c4b24a1c791263018c2fe6e23c7301ae`; executable source `73873d730110bf3394ceea19b2a4d45ddba73834`. Compare: 16 ahead / 0 behind; only this audit document differs from the tested source.
- Latest relevant Actions remain 15 Nov: Android-only [run 43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, SUCCESS 15/15 steps; wrapper [run 43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), Release `132819003046` and Debugoptimized `132819003226`, both SUCCESS 12/12. No newer active-branch build found in the 100 newest branch-filtered runs (139 total). Pending 0; failed 0.
- Android artifact 14163230171 re-downloaded: outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8` (GitHub digest match), nested flat ZIP `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Four expected members; all embedded hashes and ZIP integrity pass. Metadata version `26.3.0-devel-android-kbase-73873d730110`, min API 30. AArch64 ELF64, flat driver RUNPATH `$ORIGIN`, bundled private DRM helper with only libc dependency; 24 audited DRM imports, no unresolved symbols. Outer stage contains valid ICD manifest (API 1.4.354) and Android libdrm stub. Structurally/package validated, not device-runtime validated.
- Wrapper artifacts Release 14162259759 (outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`) and Debugoptimized 14162119843 (outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`) re-downloaded and ZIP-tested. Each is ELF-only; RUNPATH `$ORIGIN/../../android_stub`; external libdrm/X11/XCB/xshmfence closure absent. Not complete import packages.
- No new device logs or FPS data. Next blocker is Winlator Mali runtime import/initialization/device creation for the Android flat package; wrapper closure remains unresolved. No source, workflow, kernel, or system/vendor driver changes; this entry is documentation-only and uses `[skip ci]`.

## 2026-11-19 13:47 Africa/Tunis — active performance branch, source, CI, and uploaded-package revalidation

- Re-read the canonical handoff from `docs/project-continuity-handoff-2026-11-03` and this audit from `experiment/g720-performance-audit` before inspection. Pre-update branch heads: audit `b64179f52005d218534deac001979b8e11a8ebaa`; handoff `defcb046fe79995d49b6f75e55e662888c5968b7`. The current executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`.
- GitHub compare from executable source to the pre-update audit head reports 17 ahead / 0 behind; the only changed path is `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md`. The newest audit-branch commit `b64179f52005d218534deac001979b8e11a8ebaa` is documentation-only (`docs: record Nov 19 active-branch CI and package audit [skip ci]`). No PanVK source or workflow change has landed after `73873d7`.
- Re-read the descriptor-set allocation path in `src/panfrost/vulkan/panvk_vX_descriptor_set.c` at the exact source SHA. The five mutually exclusive diagnostic buckets remain overflow, no-free-set only, heap-capacity only, both capacity guards, and VMA allocation failure. Overflow/capacity continue returning `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure returns `VK_ERROR_FRAGMENTED_POOL`. Each failure increments the aggregate and exactly one category only when `PANVK_DESCRIPTOR_PROFILE` is enabled. The extra heap-capacity read used to distinguish the both bucket is confined to the profiling-enabled failure path. The counters use relaxed atomics; validate category sum against `set_failures` only after workload quiescence. `set_allocs` is an attempt counter after count validation, not a failure category. Per-failure log dumps make this diagnostic-only. No allocation policy, descriptor layout, or Vulkan result changed.
- Re-read `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. Work threshold remains `PANVK_KBASE_HEAP_RENEW_WORK` with compiled default 65,536; interval threshold remains 128 submissions. C11 `call_once` initializes and caches both options. Non-positive work values disable only the work trigger; interval renewal remains independent. The effective work threshold is logged once. The submit-side work estimate saturates at `UINT64_MAX`, and renewal waits the graphics targets before replacing the heap generation, then resets the counters. Set the environment override before the first Kbase submission and restart the process between variants. A 262,144 effective-value log from the 65K-default package is required to prove Winlator propagated the override.
- Active-branch Actions API returned 139 total runs (100 newest queried). Latest exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), source SHA `73873d730110bf3394ceea19b2a4d45ddba73834`, completed SUCCESS: job `132819003208`, all 15/15 steps succeeded, including Android ARM64 libdrm, configure/compile/stage, driver/manifest/dependency audit, flat package creation and upload. Latest wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) completed SUCCESS: Release job `132819003046` and Debugoptimized job `132819003226`, 12/12 steps each. No newer active-branch Android or wrapper run exists after 15 Nov; pending 0, failed 0. The 17 commits after source are documentation-only and do not require a rebuild.
- Revalidated the newly uploaded `PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; ZIP integrity passes and all three embedded `SHA256SUMS` entries pass. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Metadata identifies `26.3.0-devel-android-kbase-73873d730110`, min API 30, and explicitly labels the driver experimental/non-conformant. Both shared objects are ELF64 little-endian AArch64; the driver RUNPATH is `$ORIGIN`; the private DRM helper is bundled and depends only on `libc.so`. The exact-source CI audit previously recorded 24/24 driver DRM imports resolved and a valid staged ICD manifest. This is static/package verification only; no Winlator runtime result exists. The other uploaded `PanVK-G720-64c43833-flat.zip` is stale source `64c43833`, SHA-256 `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`; do not substitute it for the 73873 package.
- Repeated current-conversation and Library searches for Tomb Raider benchmark logs, threshold startup logs, and descriptor-profile captures; no results were found. The only mounted uploads are the two PanVK package ZIPs. No new device measurement exists. Preserve the historical clean 65K result (9.0 / 17.6 / 13.5 FPS), the single 262K compiled-default candidate (7.8 / 18.9 / 13.9 FPS), and the instrumented diagnostic (7.7 / 19.7 / 13.6 FPS) as inconclusive; the compiled-default run does not prove environment propagation, and no FPS gain is claimed. The historical capture still reports 4,068 renewals, 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO allocation/creation failures, and 299 descriptor failures from a pre-split capture. Those 299 cannot be retroactively categorized.
- Next device gate: import only the verified 73873 65K-default package through Winlator Mali's separate custom-driver importer, retaining System Driver as fallback. First verify driver load, physical-device enumeration, logical-device creation and stability. Run baseline with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require the effective-threshold log to show 65,536. Restart, set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission, and require the log to show 262,144; otherwise the override is unproven. Keep all profiling variables unset for matched/repeated FPS runs. In a separate process set `PANVK_DESCRIPTOR_PROFILE=1`, capture the five categories and aggregate after quiescence, and verify their sum.
- The active source's timestamp-capability advertisement concern remains a separate correctness workstream; the conservative timestamp candidate is on another branch and is not merged or device-validated. No timestamp/extension change was made here. No driver source, workflow, allocator policy, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, Android system/vendor driver, or live device configuration was changed. This is documentation-only and uses `[skip ci]`.

## 2026-11-19 — VRS candidate descriptor-heap exposure audit

The previously recommended b1525 VRS package is **not an import candidate** until its descriptor-heap extension is disabled or its state transitions are fixed. The b1525 source advertises `VK_EXT_descriptor_heap` and `descriptorHeap` on PAN_ARCH >= 9, but `panvk_vX_cmd_desc_state.c` does not invalidate descriptor-set/push-descriptor state when heaps are bound, does not invalidate heap state when descriptor sets/push descriptors are bound, and marks graphics descriptor state dirty without marking compute state dirty after heap binding. `CmdPushDataEXT` also leaves push-descriptor state intact. This conflicts with the current Vulkan descriptor-heaps state invalidation rules (https://docs.vulkan.org/spec/latest/chapters/descriptorheaps.html).

A safer isolated candidate was created from b1525:

- Branch: `work/vrs-sample-count-no-descriptor-heap`
- Source commit: `f3fda14616f1fa62dabf5f82b6db37cf24b91ba2`
- Current branch head: `b5e30b3775aaf12825f7d624b77939cfb7b98414`
- `.EXT_descriptor_heap = false` and `.descriptorHeap = false`; the extension's command implementation remains compiled but is not exposed.
- Pipeline-only VRS and the 1x/2x/4x sample-count cap are retained.
- Android-only run [44553408179](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408179) and wrapper/probe run [44553408360](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408360) were pending at this audit update. Static checkout checks passed; VRS probe subjob passed; compile jobs had not completed. No package is approved or device-validated yet.

Do not import artifact 14245318290 from b1525. The descriptor-heap experiment remains separate and blocked. No device test, benchmark, merge, system-driver change, or kernel change occurred.


## 2026-11-19 — descriptor-heap safety correction, exact-head CI, and package audit

The b1525 VRS package was found to advertise `VK_EXT_descriptor_heap` and `descriptorHeap=true` despite incomplete command-buffer state invalidation. Heap binding does not clear descriptor-set/push-descriptor state, descriptor-set/push-descriptor commands do not clear heap state, heap binding updates both graphics/compute records but only marks graphics descriptor state dirty, and `CmdPushDataEXT` leaves push-descriptor state intact. This conflicts with the current Vulkan descriptor-heaps state rules (https://docs.vulkan.org/spec/latest/chapters/descriptorheaps.html). Artifact 14245318290 from b1525 is superseded; do not import it.

A safer isolated candidate was created: `work/vrs-sample-count-no-descriptor-heap`, executable source `b5e30b3775aaf12825f7d624b77939cfb7b98414` (current branch tip `321bf9638390c067068fd13814af82d56d6a9fdb` is documentation-only). It disables both `.EXT_descriptor_heap` and `.descriptorHeap`, retains pipeline-only VRS on v12+, and caps 1x1 VRS sampleCounts to 1x/2x/4x.

Exact-head Android run [44553408179](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408179) passed 15/15 steps. Exact-head wrapper run [44553408360](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408360) passed Release 12/12, Debugoptimized 12/12, and capability-probe build 7/7. Android artifact 14542499046 outer SHA-256 `231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309`; inner flat package SHA-256 `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. Embedded hashes pass; AArch64/API 30, `$ORIGIN`, private DRM helper; 24/24 DRM imports resolved, zero missing. ICD API 1.4.354. Wrapper outputs are raw ELF-only with external X11/XCB/libdrm/xshmfence dependencies, not standalone packages.

The capability probe (artifact 14544685368) is a PE32+ x86-64 executable. The separate render/mip bundle (artifact 13685924631) includes the dynamic VRS and mip-pattern runners. No target-device result or benchmark exists yet. Next device gate is Winlator custom-driver import of only the b5e30 flat package, followed by dynamic VRS and mip-pattern logs. Keep System Driver as fallback; do not touch the system/vendor driver or kernel.


## 2026-11-20 13:01 Africa/Tunis — active performance branch revalidation

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit` before inspection. Pre-update audit branch HEAD: `f3c2f509bc6406f0ac8eb6c3bfe89457f7c98c57`; pre-update handoff branch HEAD: `15f54d35ea2f9ff5d81adde5eaa14136084df603`. Executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`.
- GitHub compare from executable source `73873d73` to the pre-update performance-audit branch reports 20 ahead / 0 behind; the only changed path is this audit Markdown file. The handoff branch is 3 commits ahead of its prior recorded base `defcb046fe79995d49b6f75e55e662888c5968b7`, with only `PROJECT_CONTINUITY_HANDOFF.md` changed. No driver-source or workflow change was found on the performance branch.
- Re-read `src/panfrost/vulkan/panvk_vX_descriptor_set.c` at the executable source SHA. The five opt-in descriptor-set failure buckets remain overflow, no-free-set only, heap-capacity only, both capacity guards, and VMA allocation failure. Overflow/capacity retain `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure retains `VK_ERROR_FRAGMENTED_POOL`. The aggregate `set_alloc_failures` is incremented with exactly one category. `set_allocs` is an attempt counter after count validation, not a failure bucket. The additional heap-capacity read is confined to the profiling-enabled failure path. Counters are relaxed atomics; compare category totals with the aggregate only after workload quiescence, and do not sum repeated cumulative log snapshots. No allocator policy, descriptor layout, Vulkan result, or object lifetime was changed.
- Re-read `src/panfrost/vulkan/csf/panvk_vX_gpu_queue.c`. `PANVK_KBASE_HEAP_RENEW_WORK` still defaults to 65,536 and `PANVK_KBASE_HEAP_RENEW_INTERVAL` to 128 submissions. C11 `call_once` initializes/caches both values; the effective work threshold is logged once. Non-positive work values disable only the work trigger; the interval trigger remains independent. The setting must be present before the first Kbase submission, and the process must be restarted between variants. A 262,144 log from a binary compiled with a 262,144 default is not proof of environment propagation; use the 65,536-default package for that check.
- Latest exact-source CI evidence remains Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), SUCCESS with 15/15 steps, and wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), SUCCESS with Release and Debugoptimized jobs at 12/12 steps each. The Android artifact is `14163230171`, created 2026-11-15, with outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`. These runs test source `73873d73`; later commits on this branch are documentation-only. No newer exact-source PanVK build evidence was found in the current audit.
- Rechecked the newly uploaded `PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`, byte-identical to the previously verified package. ZIP integrity and all three embedded SHA256SUMS entries pass. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`; metadata identifies source `73873d730110`, min API 30, experimental/non-conformant status. Both libraries are ELF64 little-endian AArch64; the driver RUNPATH is `$ORIGIN`, and the private DRM helper is bundled and depends only on `libc.so`. The exact-source CI audit previously recorded 24/24 DRM imports resolved and a valid staged ICD manifest. The uploaded `PanVK-G720-64c43833-flat.zip` is an older package (SHA-256 `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`) and must not replace the 73873 package. Package checks do not establish Winlator runtime compatibility.
- Searched current-conversation and Library files for Tomb Raider benchmark logs, threshold startup logs, and descriptor-profile captures; no matching logs were found. The only mounted uploads are the two PanVK package ZIPs. No new device measurements exist. Preserve the historical clean 65K result (9.0 / 17.6 / 13.5 FPS), the single compiled-default 262K candidate (7.8 / 18.9 / 13.9 FPS), and the instrumented diagnostic (7.7 / 19.7 / 13.6 FPS) as inconclusive. No FPS gain is claimed. The historical 4,068 renewals, 52,804.121 ms aggregate renewal wait, zero ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO failures, and 299 pre-split descriptor failures remain unchanged; those 299 cannot be categorized retrospectively.
- Exact next device gate: import only the verified 73873 65K-default package through Winlator Mali's separate custom-driver importer, retaining System Driver as fallback. First verify driver load, physical-device enumeration, logical-device creation, basic rendering, and teardown. Run with `PANVK_KBASE_HEAP_RENEW_WORK` absent and require the effective-threshold log to show 65,536. Restart the game process, set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before the first Kbase submission, and require the log to show 262,144; otherwise the override is unproven. Keep all profiling variables unset for matched/repeated FPS runs. In a separate process set `PANVK_DESCRIPTOR_PROFILE=1`, capture the five categories and aggregate after quiescence, and verify their sum.
- No PanVK source, workflow, allocator policy, heap topology, ring sizing, shader scheduling, Kbase ABI, kernel, or Android system/vendor driver was changed. No direct Winlator/Termux device-control bridge is established. This entry is documentation-only and does not require a driver rebuild.

## 2026-11-20 13:04 Africa/Tunis — scheduled CI and package revalidation

- Re-read the canonical handoff from `docs/project-continuity-handoff-2026-11-03` and this audit from `experiment/g720-performance-audit` before querying Actions. Pre-update handoff HEAD was `c1d75b2dcef2d9c464a1cd55c09a05919a2d1953`; pre-update audit HEAD was `8edbbb34ab988ab90c1f0f5cf753dc4b406ca51a`. The executable source on the active performance branch remains `73873d730110bf3394ceea19b2a4d45ddba73834`.
- GitHub Actions branch-filtered query reports 139 runs for `experiment/g720-performance-audit`. Latest exact-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) completed SUCCESS: job `132819003208`, 15/15 steps (setup, checkout/verification, Python/dependencies, host tools, Android ARM64 libdrm, Meson/configure, compile/stage, driver/manifest/dependency audit, flat package, upload, cleanup). Latest exact-source wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) completed SUCCESS: Release job `132819003046` and Debugoptimized job `132819003226`, 12/12 steps each (setup, checkout, memfd_create compatibility patch, Python/dependencies, host tools, configure, compile, upload, cleanup). No newer Android-only or wrapper run exists on the active branch after 15 Nov. Pending 0; failed 0; no failure-log inspection required.
- Re-downloaded active-branch Android artifact `14163230171` (16,747,780 bytes). Outer ZIP SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, exact GitHub digest match. Nested flat ZIP SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; ZIP integrity and all three embedded SHA256SUMS entries pass. Nested package has exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Metadata identifies `26.3.0-devel-android-kbase-73873d730110`, min API 30, and experimental/non-conformant status. Both libraries are ELF64 little-endian AArch64 Android API 30. Driver RUNPATH is `$ORIGIN`; it depends on Android system libraries plus bundled `libpanvk_drm.so`; helper depends only on `libc.so`. The outer stage separately contains the valid ICD manifest (API 1.4.354, architecture 64, path `/usr/local/lib/libvulkan_panfrost.so`) and `stage/usr/android_stub/libdrm.so`. Audit lists 24 undefined DRM imports, 212 defined helper symbols, and an empty `missing-drm-symbols.txt`. Static/package validated only; no device runtime claim.
- Re-downloaded active-branch wrapper Release artifact `14162259759`: outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`; extracted ELF SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`. Debugoptimized artifact `14162119843`: outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`; ELF SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`. Both ZIPs pass integrity checks and each contains only one ELF64 AArch64 Android API 30 library. Both use RUNPATH `$ORIGIN/../../android_stub` and need `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1`; neither archive supplies the complete dependency closure, ICD manifest, or checksum manifest. These remain raw wrapper outputs, not standalone import packages.
- Separately rechecked the newer isolated candidate branch `work/vrs-sample-count-no-descriptor-heap`; it is not the active performance-audit source branch. Exact executable checkpoint is `b5e30b3775aaf12825f7d624b77939cfb7b98414`; current branch tip is two documentation-only commits ahead (only `docs/G720-VRS-IMPLEMENTATION-AUDIT.md` changed). Exact-head Android run [44553408179](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408179), job `135662889996`, SUCCESS 15/15. Exact-head wrapper/probe run [44553408360](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408360): Release `135662892381` SUCCESS 12/12, Debugoptimized `135662892165` SUCCESS 12/12, VRS capability-probe `135662891649` SUCCESS 7/7. Pending 0; failed 0.
- Re-downloaded candidate Android artifact `14542499046` (16,823,157 bytes): outer SHA-256 `231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309`, GitHub digest match; nested flat ZIP SHA-256 `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. It contains the same four expected flat members, with all embedded hashes passing. Metadata identifies `26.3.0-devel-android-kbase-b5e30b3775aa`, min API 30, experimental/non-conformant. Both ELF files are AArch64/API 30; driver RUNPATH `$ORIGIN`, bundled private DRM helper depends only on libc. The outer staged tree contains ICD API 1.4.354 and Android libdrm stub; 24 undefined DRM imports, 212 helper definitions, zero missing imports. Static/package validated only.
- Candidate wrapper Release artifact `14543539493`: outer SHA-256 `4b9f292bd130cb3eb659c4466e39c1a93e3556afd49506c389461ffc9724ebd5`, ELF SHA-256 `fc48f8e1f616ad8a0703b302c6def356653e71c745f6e013f4db7da3ba1ce901`. Debugoptimized artifact `14544716174`: outer SHA-256 `f0b1e39ee20aa7acde149ae84f3f2b2befd3730a5dca67a882bc2ce6018b2578`, ELF SHA-256 `3bf2cc789c107b79470ff0e03f936aba5afcc273b6879702ae1345e5f7e249c8`. Both are ELF-only archives with the same external libdrm/X11/XCB/xshmfence dependency gap; not standalone packages. Do not import the superseded b1525 artifact `14245318290`, which advertises the incomplete descriptor-heap implementation. The b5e30 candidate disables both `.EXT_descriptor_heap` and `.descriptorHeap` while retaining the isolated pipeline-VRS sample-count correction.
- No new Xiaomi 15T/Winlator import result, Vulkan physical/logical device creation log, VRS render/mip-pattern log, heap-threshold startup log, descriptor-profile capture, stability report, or Tomb Raider FPS measurement was available. No direct phone/Termux/Winlator bridge exists. Do not infer runtime compatibility or performance improvement from CI/package validation.
- No PanVK source, workflow, allocator policy, Kbase ABI, kernel, Android system/vendor driver, or live device configuration was modified; no workflow was triggered. Next device gate remains import of the verified b5e30 flat package through Winlator Mali's separate custom-driver importer, with System Driver retained as fallback, then run the verified dynamic-VRS and mip-pattern probes and return complete logs. The active performance branch's 73873 package remains the separate 65K-default baseline for the heap-threshold A/B.


## 2026-11-20 16:22 Africa/Tunis — scheduled audit and independent candidate revalidation

- Re-read the canonical handoff and this active audit before querying GitHub. Repository branch inventory: 32 refs. The active performance-audit branch is documentation-only relative to executable source `73873d730110bf3394ceea19b2a4d45ddba73834`; current audit head before this entry was `3a964451ccbacc9b68ca44af07066f70eb41318c`, compare 22 ahead / 0 behind, with only this Markdown path changed.
- Exact active-source Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) remains SUCCESS (15/15 steps); wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) remains SUCCESS (Release and Debugoptimized 12/12 each). No newer active-branch build exists. Android artifact 14163230171 was re-downloaded; GitHub outer digest and nested package SHA remain the handoff values. No device runtime evidence.
- Revalidated the separate `work/vrs-sample-count-no-descriptor-heap` candidate. Its current branch tip before this entry was `230c25f9c7538d0de1a9dc49b60c9d9ab894d228`; executable source remains `b5e30b3775aaf12825f7d624b77939cfb7b98414`, with only documentation commits after the tested source. Exact-head Android run [44553408179](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408179) remains SUCCESS (15/15); wrapper/probe run [44553408360](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408360) remains SUCCESS (Release 12/12, Debugoptimized 12/12, capability probe 7/7).
- Re-downloaded Android artifact 14542499046. Outer ZIP SHA-256 `231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309`; nested flat package SHA-256 `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. ZIP integrity and all embedded hashes pass. The flat package has exactly four members; driver and private DRM helper are ELF64 little-endian AArch64 with Android API 30 notes. Driver RUNPATH is `$ORIGIN`, NEEDED includes the bundled `libpanvk_drm.so`; helper NEEDED is `libc.so`. The 24 DRM/GBM imports are all defined by the helper (212 dynamic definitions); missing-DRM report is empty. The outer staged ICD manifest reports API 1.4.354. This is static/package validation only, not Winlator runtime validation.
- Re-downloaded wrapper Release artifact 14543539493 and Debugoptimized artifact 14544716174. Both are ELF64 AArch64/API 30 raw ELF-only archives; RUNPATH is `$ORIGIN/../../android_stub`; each needs Android libraries plus external `libdrm.so`, X11/XCB and `libxshmfence.so.1`. They are not standalone import packages. Capability probe artifact 14544685368 is a separate PE32+ x86-64 executable, not the render probe.
- Re-downloaded render-probe artifact 13685924631 from source `c4f091e86c29eae88d2a659813d80b416f95fbf0`; GitHub digest and local ZIP SHA-256 both equal `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`. It is unexpired, contains the expected dynamic/mip runners and a PE32+ x86-64 executable. The probe C source blob is identical at c4f and b5e30, so the probe matches the candidate's source checkpoint.
- Descriptor-heap audit correction: earlier notes incorrectly said `cmd_bind_desc_heap` marks only graphics descriptor state dirty. The inspected `04947cb` source actually marks both graphics and compute `DESC_STATE` dirty. The actual spec blocker remains: heap bind does not invalidate previously set non-heap descriptor/push-descriptor state; descriptor-set and push-descriptor commands do not invalidate heap state; and `CmdPushDataEXT` does not invalidate non-heap descriptor state. The Vulkan descriptor-heaps specification requires those invalidations (https://docs.vulkan.org/spec/latest/chapters/descriptorheaps.html). The b5e30 candidate keeps both `.EXT_descriptor_heap` and `.descriptorHeap` false. The descriptor-heap implementation branch remains blocked and must not be imported or merged.
- Source review: Kbase remains one CSG with three CSI subqueues, per-subqueue sequence cells, SYSTEM-scope SYNC_ADD64 by default, CQS64 waits in bounded 20 ms slices with notification fallback, periodic re-kick and a 10 s watchdog. The device-wide fatal-error latch is checked before accepting a seqno. CSG-scope SYNC_ADD64 remains diagnostic-only; internal GPU WAIT64 is restricted to subqueues of the same VkQueue. Heap-renewal options are initialized once with `call_once`; defaults remain interval 128 and work 65,536. Classic descriptor pools still reserve one backing BO; 64-bit size/overflow checks and rollback on partial set allocation are present, but no segmented allocator is implemented.
- Active-source timestamp advertisement remains a correctness concern: Kbase reports `gpu_can_query_timestamp = (gpu_id != 0)` while `timestamp_frequency = 0`; active Vulkan code reports `timestampValidBits = 64`, `timestampComputeAndGraphics = true`, and calibrated timestamps from that boolean while `timestampPeriod = 0`. The isolated `work/kbase-timestamp-capability-fix` candidate disables Kbase GPU timestamp support and gates the Vulkan properties/extensions; its prior exact-head CI/package checks remain separate and no device test exists.
- No new Xiaomi 15T/Winlator logs, Vulkan device-creation result, dynamic VRS render result, benchmark, or FPS measurement was available. No phone access/diagnostic bridge is established. No source, kernel, system/vendor driver, or device state was changed. Next device gate remains importing only the b5e30 flat package through Winlator's custom-driver importer with System Driver retained as fallback, then running `Run-Dynamic-VRS.cmd` and `tools/run-vrs-mip-pattern.cmd` and returning the complete logs.


## 2026-11-21 13:25 Africa/Tunis — active-source, CI, package, and benchmark-log revalidation

- Re-read the canonical handoff on `docs/project-continuity-handoff-2026-11-03` and this audit on `experiment/g720-performance-audit` before querying source, Actions, uploads, and package contents. Pre-update branch heads: audit `c28af79cf6eadbc5765cb21c5216e09267e295ce`; handoff `a3a2ab06e7e20b678cb4ac5548aca15e28ac54fb`.
- The executable PanVK source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. GitHub compare reports 23 commits ahead / 0 behind from that source to the audit head, with exactly one changed path: this Markdown file. No driver source, workflow, or build configuration changed. The handoff branch is 2 commits ahead of its 2026-11-20 pre-update head, with only `PROJECT_CONTINUITY_HANDOFF.md` changed.
- Re-read the live source: `panvk_vX_gpu_queue.c` defines interval default 128 and work default 65,536 at lines 82–83. The process-wide options are read via `debug_get_num_option()`, cached with C11 `call_once`, and the effective work threshold is logged once. Per queue, tiler-work estimates saturate at `UINT64_MAX`; renewal occurs when either the interval count or positive work threshold is reached, waits for graphics targets, renews the heap, then resets both per-queue accumulators. The profile trigger counters can both increment for one renewal if both predicates are true; do not assume the interval/work trigger counters form mutually exclusive buckets. Environment override must be set before the first Kbase submission and must be proven from the 65K-default binary's 262,144 startup log.
- Re-read `panvk_vX_descriptor_set.c`: overflow, no-free-set-only, heap-capacity-only, both-capacity, and VMA failure counters remain mutually exclusive on the set-allocation failure paths. Existing Vulkan results remain unchanged: overflow/capacity return `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure returns `VK_ERROR_FRAGMENTED_POOL`. The additional heap-capacity read is still confined to the profiling-enabled failure path. Important telemetry interpretation: `set_allocs` and `set_alloc_bytes` are incremented before free-slot/capacity checks, so they measure allocation attempts/requested bytes, not only successful descriptor sets. Counters are relaxed atomics; use a quiescent final snapshot for category-sum checks. Repeated profile lines are cumulative snapshots.
- Exact-source GitHub Actions recheck: Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968) completed SUCCESS with all 15 steps; wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955) completed SUCCESS for Release and Debugoptimized, 12 steps each. Both runs were created 2026-11-15 against source `73873d73`; the active-branch Actions query shows no newer build through 2026-11-21. The Android artifact is 14163230171 (outer GitHub digest `sha256:ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`).
- Newly uploaded `PanVK-G720-73873-flat.zip` was checked directly at `/mnt/data/PanVK-G720-73873-flat.zip`: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; ZIP integrity and all three embedded SHA256SUMS entries pass; exactly four members (`meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, `SHA256SUMS`). Metadata identifies source `73873d730110`, minimum API 30, and explicitly says experimental/non-conformant and runtime unverified. Both libraries are ELF64 little-endian AArch64 with Android API 30 notes; driver RUNPATH is `$ORIGIN`; driver depends on bundled `libpanvk_drm.so`; helper depends only on `libc.so`. The driver has 24 undefined DRM imports and the helper exports 212 dynamic functions; all 24 resolve, zero missing. This is static/package validation only, not Winlator load or game-runtime validation. The separately re-uploaded `64c43833` package is older (SHA-256 `83ebb994328d6ba2e52fbd63f16c25b567f4fbe3763b853a1e17584df23f8433`) and must not replace the 73873 package.
- No new Tomb Raider archive or device log was present in the mounted uploads; `/mnt/data` contains only the two PanVK ZIP packages, and the conversation/Library file search returned no benchmark-log match. No new threshold-startup proof or descriptor-profile capture exists. Preserve the historical results as inconclusive: clean 65K baseline 9.0/17.6/13.5 FPS; one compiled-default 262K run 7.8/18.9/13.9 FPS; instrumented diagnostic 7.7/19.7/13.6 FPS. The 262K compiled-default run does not prove environment propagation; no FPS gain is claimed. Historical telemetry remains 4,068 renewals, 52,804.121 ms aggregate renewal wait, zero CSF ring wraps, 1.8 KiB sampled occupancy high-water, zero transient BO allocation/creation failures, and 299 descriptor failures from the pre-split capture.
- No source edit, allocator-policy change, CI run, or new package build was made. No heap topology, ring size, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan extension exposure, or Android system/vendor driver was changed.
- Next device gate: import only the verified 73873 flat package through Winlator Mali's separate custom-driver importer, retaining System Driver as fallback. Run matched uninstrumented A/B processes: (A) leave `PANVK_KBASE_HEAP_RENEW_WORK` absent and require the startup log `tiler heap renewal work threshold=65536`; (B) restart and set exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` before first Kbase submission and require `tiler heap renewal work threshold=262144`. If the second log is absent or remains 65,536, the override is unproven and its FPS result is invalid as a 262K test. Leave all profiling variables unset for FPS runs; match scene, resolution, quality, DXVK 1.7.2, wrapper, and thermal starting conditions, preferably with repeated A/B samples. In a separate process, enable `PANVK_DESCRIPTOR_PROFILE=1`, collect all five failure buckets and aggregate `set_failures`, and compare only a quiescent final snapshot.


## 2026-11-21 13:33 Africa/Tunis — scheduled active-branch Actions and artifact revalidation

- Re-read the canonical handoff from `docs/project-continuity-handoff-2026-11-03` and the performance audit from `experiment/g720-performance-audit` before querying Actions and artifacts. Pre-update branch heads: handoff `44e8603695c713d4e4c51dc70949e82e2ec93cb2`; performance-audit `a91a99861d00944825a1e37e39c0eb63496531f4`.
- Active performance-audit branch has 139 workflow runs. The latest relevant Android-only run is [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), run #320, created 2026-11-15 12:57:36Z, source `73873d730110bf3394ceea19b2a4d45ddba73834`; job `132819003208` (`build-android-only`) completed SUCCESS, 15/15 steps. Exact completed steps: Set up job; Check out selected source branch; Verify checkout; Set up Python; Install build dependencies; Generate host tools; Build real Android ARM64 libdrm; Configure Android-only build; Compile and stage Android install; Audit driver, manifest, dependencies and staged files; Build flat Winlator/Adrenotools package; Upload complete experimental package and audit; Post Set up Python; Post Check out selected source branch; Complete job. Pending 0; failed 0.
- Latest relevant wrapper run is [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), run #694, same source commit and creation time. Release job `132819003046` (`build-android-wrapper (release)`) completed SUCCESS, 12/12 steps; Debugoptimized job `132819003226` (`build-android-wrapper (debugoptimized)`) completed SUCCESS, 12/12 steps. Both completed: Set up job; Checkout repository; Patch anon_file.c for memfd_create; Set up Python; Install Build Dependencies; Generate Host Tools; Configure Meson; Compile Project; Upload Vulkan Wrapper Artifact; Post Set up Python; Post Checkout repository; Complete job. Pending 0; failed 0. No failure logs were required.
- Branch-filtered Actions listing shows no newer Android-only or wrapper run on the active branch after 2026-11-15. The next prior pair is run 43716418873 (Android-only) and 43716418887 (wrapper) against source `d6557723d9c53e717caf098e98e7a7263d081fa2`; those are superseded by the 73873 pair. Initial watch IDs 41947103034 and 41947103085 remain historical only.
- Re-downloaded all three artifacts and independently checked local ZIP integrity, SHA-256, package members, ELF headers, Android notes, dynamic dependencies, RUNPATHs, and manifest:
  - Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), 16,747,780 bytes. Outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, exact match to GitHub artifact digest. Outer ZIP passes `unzip -t`; it contains 28 entries including staged driver, staged Android libdrm stub, expat files, ICD manifest, audit outputs, and nested `PanVK-G720-experimental.zip`. Nested flat ZIP SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; nested ZIP passes integrity and contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All three embedded SHA256SUMS entries pass. Metadata: `26.3.0-devel-android-kbase-73873d730110`, min API 30, explicitly experimental/non-conformant and device/game compatibility unverified. Both shared objects are ELF64 little-endian AArch64 with Android API 30 notes. Driver RUNPATH `$ORIGIN`; its NEEDED list uses bundled `libpanvk_drm.so` plus Android platform libraries. The private helper needs only `libc.so`. Outer staged ICD manifest is valid JSON, API `1.4.354`, architecture 64, path `/usr/local/lib/libvulkan_panfrost.so`; outer stage includes `stage/usr/android_stub/libdrm.so`. Audit lists 24 undefined DRM imports, helper has 212 dynamic definitions, and `missing-drm-symbols.txt` is empty (0 missing). Static/package validation only; no device runtime claim.
  - Wrapper Release artifact [14162259759](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162259759): 4,288,198 bytes, outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`, exact GitHub digest match; ZIP passes integrity; contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`.
  - Wrapper Debugoptimized artifact [14162119843](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162119843): 50,651,760 bytes, outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`, exact GitHub digest match; ZIP passes integrity; contains only `libvulkan_panfrost.so`; extracted ELF SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`.
- Both wrapper ELFs are ELF64 little-endian AArch64 with Android API 30 notes and RUNPATH `$ORIGIN/../../android_stub`. Their NEEDED lists include `libdrm.so`, X11/XCB libraries (`libxcb.so.1`, `libX11-xcb.so.1`, DRI3/present/xfixes/sync/randr/shm variants), and `libxshmfence.so.1`, in addition to Android libraries. Those dependencies, a complete ICD manifest, and a checksum manifest are not supplied by either one-file ZIP. Wrapper jobs passed, but their artifacts remain raw ELF outputs, not standalone Winlator packages.
- Active branch head is `a91a99861d00944825a1e37e39c0eb63496531f4`. Compare from executable source `73873d730110bf3394ceea19b2a4d45ddba73834` is 24 ahead / 0 behind and reports exactly one changed path, this performance-audit Markdown. No PanVK source, workflow, build configuration, allocator policy, Kbase ABI, kernel, or Android system/vendor driver changed.
- No new phone-side import, Vulkan physical/logical device creation, Winlator log, dynamic VRS render/mip-pattern result, heap-threshold startup proof, descriptor-profile capture, stability report, or Tomb Raider FPS archive was available. No device-control bridge is available in this monitoring environment. Runtime compatibility and performance remain unverified; do not infer either from CI.
- Next gate: on the non-rooted Xiaomi 15T, import only the verified 73873 flat package through Winlator Mali's separate custom-driver importer and retain System Driver as fallback. Verify ICD load, physical-device enumeration, logical-device creation, teardown, and logs. Then run matched uninstrumented 65,536-default versus 262,144 override processes, requiring the effective-threshold startup log after a full restart and before first Kbase submission. Collect descriptor profiling separately with `PANVK_DESCRIPTOR_PROFILE=1`; use a quiescent final snapshot. No system/vendor driver replacement or live-kernel modification.


## 2026-11-21 16:46 Africa/Tunis — exact-head CI, package, and source revalidation

- Re-read the canonical handoff and this audit before querying GitHub. Live branch inventory is 32 refs. Current performance-audit head is c35cad54816c51a70304c6a002de10fbd7daa8a7; parent is a91a99861d00944825a1e37e39c0eb63496531f4. Compare from executable source 73873d730110bf3394ceea19b2a4d45ddba73834 is 25 ahead / 0 behind with only this audit Markdown changed. No driver source or workflow change exists on the active branch.
- Exact-source Android run 43716474968 is SUCCESS (15/15); wrapper run 43716474955 is SUCCESS (Release and Debugoptimized 12/12 each). GitHub head-SHA query returns only these two runs for 73873. Android artifact 14163230171 was re-downloaded: outer digest ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8; inner flat-package SHA-256 824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318. ZIP integrity and embedded checksums pass; four flat members; ELF64 AArch64/API 30; $ORIGIN; private DRM helper; 24 DRM imports, 212 helper dynamic definitions, empty missing-symbol report. Staged ICD manifest reports API 1.4.354. Runtime remains unverified.
- Re-downloaded both active wrapper artifacts. Release outer SHA-256 438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9; Debugoptimized outer SHA-256 ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44. Both are ELF-only AArch64/API 30 archives with RUNPATH $ORIGIN/../../android_stub and external libdrm, X11/XCB, and xshmfence dependencies. Neither is a complete standalone package.
- Revalidated safe VRS source b5e30: Android 44553408179 SUCCESS 15/15; wrapper/probe 44553408360 SUCCESS (12/12 per wrapper variant; capability probe 7/7). Artifact 14542499046 outer digest 231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309; nested package digest a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018. The flat package's four members and checksums pass; AArch64/API 30, $ORIGIN, bundled private DRM helper, zero missing DRM imports. This branch is 222 ahead / 68 behind active source 73873; its 4-commit documentation tail is separate. Do not use it as an active-source performance baseline.
- Timestamp candidate f5084ae is 8 commits ahead / 0 behind 73873. Android 43900657540 and wrapper 43900657634 are successful (15/15 and 12/12 per variant). Artifact 14240094671 was re-downloaded; outer SHA-256 1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74, inner flat SHA-256 28752b6aa0ec594af9f6334fbe61e4d1211b46b68e89d835784c7d02f1152d318; four members and hashes pass; AArch64/API 30, $ORIGIN, private DRM helper, empty missing-symbol report. Candidate remains isolated and device-unvalidated.
- Source review: active 73873 still sets gpu_can_query_timestamp from nonzero GPU ID while Kbase timestamp_frequency is zero. Its queue-family timestampValidBits and timestampComputeAndGraphics, calibrated-timestamp extensions, and shader-clock extension/feature are enabled from that boolean; timestampPeriod remains zero. This is an active-source capability inconsistency. The f508 candidate disables Kbase GPU timestamps, while the b5e30 VRS source also disables them and gates extension/property advertisement on valid frequency and non-Kbase path.
- VRS candidate b5e30 exposes pipeline-only VK_KHR_fragment_shading_rate on v12+, with primitive/attachment FSR disabled and nontrivial combiners disabled. It also disables VK_EXT_descriptor_heap and descriptorHeap. The dynamic-FSR source checkpoint c4f091e has exact Android run 42645674486, wrapper run 43901384646, and render-probe artifact 13685924631. The historical user log validates basic pipeline-rate rasterization but predates the dynamic per-draw fragment-center sysval change; no new device result exists.
- Descriptor-heap source review correction: cmd_bind_desc_heap marks both graphics and compute descriptor states dirty. The real blocker is missing bidirectional invalidation of heap and non-heap descriptor/push-descriptor state, including CmdPushDataEXT. Keep branch 04947 blocked. Classic descriptor pools remain single-backing-BO allocations.
- Kbase CSF defaults and concurrency safeguards remain unchanged: SYSTEM-scope SYNC_ADD64 default; CSG scope diagnostic-only; CQS64 wait slices plus event fallback/re-kick/watchdog; call_once for process-wide heap-renewal options; interval 128/work 65,536. Renewal trigger counters are not mutually exclusive. Descriptor profile counters are opt-in relaxed atomics; allocation attempt counters are incremented before capacity checks. Shader-core masks use the Kbase shader_present mask, not a hard-coded seven-way compiler dispatch. No ray-query or acceleration-structure extension is advertised in the active table.
- No new device log, Winlator import, benchmark, threshold proof, or descriptor-profile capture exists. No direct device bridge exists. No source, kernel, or Android system/vendor driver was changed.
- Next gate is the isolated b5e30 dynamic-VRS/mip-pattern device probe. Because this branch is 68 commits behind active source, use its results only to validate that branch's VRS semantics; do not compare its FPS against 73873 or treat it as the current baseline.


## 2026-11-22 13:16 Africa/Tunis — scheduled active-branch source, CI, package, and benchmark audit

- Re-read the canonical handoff from `docs/project-continuity-handoff-2026-11-03` and this audit from `experiment/g720-performance-audit` before inspecting the live repository. Pre-update branch heads: performance audit `5c8a0b1385b830d092888ed2d3fe5eba7786846f`; handoff `02c2a831f0cd047369f2872766e5b6b308cceb34`. The active executable source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Compare from that source to the performance branch is 26 ahead / 0 behind, with exactly one changed path: this audit Markdown. The latest audit-branch commit `5c8a0b1` is documentation-only; no driver source, workflow, or build configuration changed. The handoff branch remains intentionally separate and was not merged.
- Live branch inventory returned 31 refs. The related `experiment/g720-heap-renew-262k` branch is not a current-source continuation: compare to `73873d73` is diverged (2 ahead / 75 behind; merge base `e226f206`), changing the Android workflow filter and the compiled default constant from 65,536 to 262,144. Its own Android/wrapper CI runs `41947695997` and `41947696048` succeeded, but the binary's compiled default is not proof that Winlator forwarded an environment override. Do not use this old branch/package as the 65K-default override-propagation test.
- Exact-source active-branch CI remains successful: Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, 15/15 steps; wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), Release job `132819003046` and Debugoptimized job `132819003226`, 12/12 steps each. Both target exact source `73873d73`, created 2026-11-15 12:57:36Z. Branch-filtered Actions shows no newer Android-only or wrapper run on the performance branch. The commit combined-status endpoint returns no separate status records; the completed Actions jobs are the build evidence.
- The newly uploaded `PanVK-G720-73873-flat.zip` is byte-identical to the previously audited package: SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Local `unzip -t` and all three embedded `SHA256SUMS` entries pass. It contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. Both libraries are ELF64 little-endian AArch64, Android API 30; driver RUNPATH is `$ORIGIN`, driver NEEDED includes the bundled `libpanvk_drm.so`, and the helper depends only on `libc.so`. Metadata identifies `26.3.0-devel-android-kbase-73873d730110` and explicitly says experimental/non-conformant with device/game compatibility unverified. This is static package validation, not Winlator runtime validation.
- No new Tomb Raider benchmark archive or log was supplied. Current mounted conversation files are repeated 73873/64c43833 packages only; Library metadata has older VRS probe logs and Nov. 7 Tomb Raider-related archives, not a newer performance capture. Semantic search returned no matching benchmark. No new 65K/262K startup-threshold proof, descriptor-profile capture, stability report, or FPS measurement exists.
- Source recheck at `73873d73`: `PANVK_KBASE_HEAP_RENEW_INTERVAL` defaults to 128 and `PANVK_KBASE_HEAP_RENEW_WORK` to 65,536. The options are read as `int64_t`, cached together through C11 `call_once`, and the effective work threshold is logged once. A value of 262,144 is within range and is stored as `uint64_t`; only the startup log `tiler heap renewal work threshold=262144` from this 65K-default binary proves that the override reached PanVK before the first Kbase submission. Work accumulation is saturating and counts only nonzero tiler estimates on graphics-touched submissions; renewal waits for outstanding graphics targets before replacing the heap and resets the per-queue accumulators. Interval and work trigger counters can both increment for one renewal; they are not mutually exclusive. No renewal policy was changed.
- Descriptor source recheck confirms five mutually exclusive failure buckets: overflow, no-free-set-only, heap-capacity-only, both-capacity, and VMA allocation failure. Overflow/capacity returns remain `VK_ERROR_OUT_OF_POOL_MEMORY`; VMA failure remains `VK_ERROR_FRAGMENTED_POOL`. The additional heap-capacity read is inside the opt-in failure path. `set_allocs`/`set_alloc_bytes` count allocation attempts/requested bytes after overflow validation but before capacity/VMA checks. Counters are relaxed atomics; compare the five category totals with aggregate `set_failures` only after workload quiescence, and do not sum repeated cumulative snapshots.
- Separate correctness follow-up: active source `73873d73` still sets `gpu_can_query_timestamp` from nonzero GPU ID while Kbase `timestamp_frequency` is zero. The isolated branch `work/kbase-timestamp-capability-fix` at `f5084ae0` is 8 commits ahead / 0 behind `73873d73`, changes Kbase timestamp capability and Vulkan physical-device advertisement, and has successful Android run `43900657540` plus wrapper run `43900657634`. It disables Kbase GPU timestamp queries and gates timestamp extension/property exposure on a valid nonzero frequency and non-Kbase path. This is a separate correctness candidate, not merged or device-validated in the performance branch; do not mix it into the heap-renewal A/B without a separately reviewed build/test.
- Historical performance remains inconclusive: clean 65K 9.0/17.6/13.5 FPS; one compiled-default 262K run 7.8/18.9/13.9 FPS; instrumented diagnostic 7.7/19.7/13.6 FPS. The 2026-11-05 telemetry remains 4,068 heap renewals, 52,804.121 ms aggregate renewal wait, zero CSF ring wraps, 1.8 KiB sampled ring occupancy high-water, zero transient BO allocation/creation failures, and 299 descriptor failures from the pre-split capture. Do not claim a performance gain.
- No PanVK source edit, allocator-policy change, package rebuild, or device action occurred. No heap topology, ring size, BO retention, shader scheduling, Kbase ABI, kernel, Vulkan extension exposure, or Android system/vendor driver changed.
- Next device gate: import only the verified 73873 65K-default flat package through Winlator Mali's separate custom-driver importer, retaining System Driver as fallback. First verify ICD load, physical-device enumeration, logical-device creation, rendering, and teardown. Then run matched uninstrumented processes: (A) work variable absent, restart, require startup threshold 65,536; (B) restart with exactly `PANVK_KBASE_HEAP_RENEW_WORK=262144` set before first Kbase submission, require startup threshold 262,144. If absent or still 65,536, override propagation is unproven and that FPS sample is invalid as a 262K test. Keep all profiling variables unset for repeated matched FPS runs. In a separate process, enable only `PANVK_DESCRIPTOR_PROFILE=1`, capture all five buckets plus aggregate `set_failures`, and compare a quiescent final snapshot.


## 2026-11-22 13:22 Africa/Tunis — scheduled active-branch Actions and artifact revalidation

- Re-read the canonical handoff and this audit before querying GitHub. Pre-update branch heads: handoff `1c898cd945462cedd6e4c4a48565ddac530b36a3`; audit `9d748aac11b5bee9eb85ad16976049cdb91057dd`. Executable source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Source-to-audit comparison: 27 ahead / 0 behind, only `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` changed.
- Active-branch Actions API returned 139 runs (100 newest inspected). No Android-only or wrapper run newer than 2026-11-15. Android-only [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, completed SUCCESS (15/15 steps); wrapper [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955), Release job `132819003046` and Debugoptimized job `132819003226`, each completed SUCCESS (12/12 steps). All returned steps completed successfully; pending 0, failed 0. No failure logs were required.
- Re-downloaded all three artifacts. Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), 16,747,780 bytes: outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8` matches GitHub digest; nested flat ZIP SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. Inner ZIP passes integrity, contains exactly four expected members, and all three embedded checksums pass. Both ELF files are AArch64/Android API 30; driver RUNPATH `$ORIGIN`; private DRM helper bundled and depends only on `libc.so`. Outer stage contains a valid 64-bit ICD manifest with API 1.4.354 and Android ARM64 `stage/usr/android_stub/libdrm.so`. Audit import/export sets were compared: 24 undefined DRM imports, 212 definitions, zero unresolved, empty missing-symbol report.
- Wrapper Release artifact [14162259759](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162259759): outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`; extracted ELF SHA-256 `d2e956c6f7952bdeed75be3f99b7dbb5233e142f9a2488771b4375ee296d4bf9`. Debugoptimized artifact [14162119843](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162119843): outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`; extracted ELF SHA-256 `afbec512138f061797dd796c60ded36e630e31efce8fbf63fc49af3d55225bf4`. Both pass ZIP integrity, contain one ELF64 AArch64/API 30 library, use RUNPATH `$ORIGIN/../../android_stub`, and require external libdrm, X11/XCB and xshmfence libraries. They lack complete dependency closure, ICD and checksum manifests; not standalone packages.
- No new phone-side import, Vulkan initialization/device creation, heap threshold log, descriptor profile, stability result, or Tomb Raider FPS capture was available. No source/build/kernel/system-driver change or build trigger occurred. Next gate remains separate Winlator custom-driver runtime validation of the verified Android flat package, with stock System Driver retained as fallback, then controlled heap-threshold A/B and descriptor diagnostics.


## 2026-11-22 16:10 Africa/Tunis — independent source, CI, and artifact revalidation

- Re-read the canonical handoff (latest authoritative entry 2026-11-22 13:22) and this active audit before checking GitHub. Branch search returned 32 refs and the continuation cursor was exhausted; no new branch names appeared. Compare from executable source `73873d730110bf3394ceea19b2a4d45ddba73834` to `experiment/g720-performance-audit` is now 28 ahead / 0 behind and lists only this audit Markdown path. The 13:22 entry recorded 27 ahead; the additional commit is documentation-only. No active PanVK source or workflow change was found.
- Independently re-fetched exact-source Actions jobs. Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, is completed SUCCESS with all 15 steps successful. Wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955): Release job `132819003046` and Debugoptimized job `132819003226` are completed SUCCESS with all 12 steps successful each. Both exact tested head SHAs are `73873d730110bf3394ceea19b2a4d45ddba73834`. The 13:22 branch-filtered Actions inventory reported no newer exact-source build; the available connector in this pass exposes exact-run job/artifact queries but not a fresh branch-wide run listing, so a run created after that inventory cannot be excluded.
- Re-downloaded active Android artifact `14163230171` (16,747,780 bytes): outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, matching GitHub artifact metadata; nested flat ZIP SHA-256 `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`. ZIP integrity and all three embedded `SHA256SUMS` entries pass; exactly four flat members. Driver/helper are ELF64 little-endian AArch64 with Android API 30 notes. Driver RUNPATH is `$ORIGIN`; NEEDED includes the bundled `libpanvk_drm.so`; helper NEEDED is only `libc.so`. The 24 audited DRM imports all resolve against 212 helper exports (zero missing). The outer stage contains a valid 64-bit ICD manifest, API 1.4.354, plus staged Android libdrm/expat files. Package metadata explicitly says experimental/non-conformant and device/game compatibility unverified.
- Re-downloaded safe-VRS Android artifact `14542499046`: outer SHA-256 `231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309`; nested flat ZIP SHA-256 `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. Its four members, embedded checksums, AArch64/API 30 ELFs, `$ORIGIN` RUNPATH, private helper dependency closure, 24/24 DRM imports, zero missing symbols, and staged ICD API 1.4.354 all revalidate.
- Re-downloaded timestamp-candidate Android artifact `14240094671`: outer SHA-256 `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`; nested flat ZIP SHA-256 `28752b6aa0ec594af9f6334fbe61e4d1211b46b68e89d835784c7d02f1152d318`. Four members and embedded hashes pass; ELF64 AArch64/API 30, `$ORIGIN`, private helper closure, 24/24 DRM imports, zero missing symbols, ICD API 1.4.354. Its metadata identifies source `f5084ae0ebc8`.
- Re-downloaded wrapper Release artifacts for active source `14162259759` (outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`), timestamp candidate `14242286679` (`a8827d0b3c63a2feaf9b29637849846d405b54f3f85acdb0b564f6455879543d`), and safe VRS `14543539493` (`4b9f292bd130cb3eb659c4466e39c1a93e3556afd49506c389461ffc9724ebd5`). All pass ZIP integrity and contain only one ELF64 AArch64/API 30 `libvulkan_panfrost.so`; RUNPATH is `$ORIGIN/../../android_stub`. NEEDED includes external `libdrm.so`, X11/XCB libraries, and `libxshmfence.so.1`. These are raw wrapper outputs, not standalone Winlator packages. Debugoptimized jobs passed, but no claim is made that their external dependencies are bundled.
- Re-fetched timestamp candidate exact-head jobs: Android run `43900657540` passed 15/15; wrapper run `43900657634` passed Release and Debugoptimized 12/12 each, with artifact head SHA `f5084ae0ebc8a4a8e38ac4007a6ca96daffe96b9`. Compare from active source `73873d73` to this branch is 8 ahead / 0 behind, so it is a fast-forward correctness candidate rather than a diverged VRS lineage. Source inspection confirms Kbase now sets `gpu_can_query_timestamp=false` while frequency remains zero; calibrated-timestamp and shader-clock feature/extension exposure is gated on nonzero frequency and non-Kbase path.
- Active-source timestamp defect independently confirmed: `kbase_kmod.c` sets `gpu_can_query_timestamp=(gpu_id != 0)` while `timestamp_frequency=0`; `panvk_get_gpu_system_timestamp_period()` returns 0 when frequency is zero. Yet active physical-device advertisement uses the boolean for `timestampComputeAndGraphics`, `VK_KHR/EXT_calibrated_timestamps`, and shader-clock features. Do not claim usable Kbase GPU timestamps on source `73873d73`; keep the isolated `f5084ae0` correction unmerged pending target-device validation.
- Descriptor-heap branch `04947cb9` remains diverged from active source (62 ahead / 68 behind) and blocked. Source inspection confirms `cmd_bind_desc_heap()` updates both graphics and compute heap records and marks both descriptor states dirty; the actual defect is missing bidirectional invalidation between heap and non-heap descriptor/push-descriptor state. `CmdPushDataEXT` dirties push uniforms but does not invalidate non-heap descriptor state. Do not import or merge this implementation.
- Safe VRS `b5e30b37` is a separate diverged candidate (223 ahead / 68 behind active source); its exact Android run `44553408179` passed 15/15 and wrapper/probe run `44553408360` passed Release 12/12, Debugoptimized 12/12, and capability probe 7/7. Keep it for VRS functionality testing only; do not compare its FPS with active source. Dynamic-FSR and older fragment-shading-rate branches are also diverged from the active source. Agent branch changes remain OpenCode/configuration/workflow infrastructure, not a PanVK driver implementation.
- Active Kbase queue source retains interval 128 and work threshold 65,536, process-wide option initialization via C11 `call_once`, and opt-in profiling. Descriptor telemetry counts allocation attempts/requested bytes before capacity/VMA checks; five failure buckets remain separate. No allocator semantics, ring sizing, synchronization policy, shader scheduling, Kbase ABI, kernel, or system/vendor driver was changed.
- No new Xiaomi 15T/Winlator import, Vulkan device-creation log, dynamic-VRS render log, heap-threshold startup proof, descriptor-profile capture, stability result, or FPS benchmark was available. No direct Android/Termux/Winlator control bridge is established.
- Next device gate: validate the active-lineage timestamp correction first using the separate f5084 Android flat package from [run 43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540), with System Driver retained as fallback. Capture loader/ICD load, physical-device enumeration, logical-device creation, one minimal render/submit, and teardown logs. Do not replace the Android system/vendor driver. Keep the 73873 65K-default package as the separate performance baseline; do not mix this correctness test into FPS A/B measurements.


## 2026-11-23 13:16 Africa/Tunis — scheduled active-branch CI and artifact verification

- Re-read both canonical branch documents before monitoring. The branch-filtered Actions API returned 139 runs (100 newest returned); the newest relevant active-branch runs remain Android-only `43716474968` and wrapper `43716474955`, both created 2026-11-15 12:57:36Z at exact source `73873d730110bf3394ceea19b2a4d45ddba73834`. No newer Android-only or Android-wrapper run exists on `experiment/g720-performance-audit` in the returned inventory.
- Android-only run [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), job `132819003208`, completed SUCCESS; all 15 summarized steps are completed/successful, including source verification, dependencies, host tools, Android ARM64 libdrm, Meson configuration, compile/stage, manifest/dependency audit, flat-package creation, upload, and post-job cleanup. Wrapper run [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955): Release job `132819003046` and Debugoptimized job `132819003226` both completed SUCCESS with all 12 summarized steps successful per job. Pending 0, failed 0; no failure logs required.
- Freshly downloaded Android artifact [14163230171](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968/artifacts/14163230171), 16,747,780 bytes, outer SHA-256 `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, exactly matching GitHub artifact digest. Outer ZIP integrity passes. Nested `PanVK-G720-experimental.zip` SHA-256 is `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; nested ZIP integrity passes and it contains exactly `meta.json`, `libvulkan_panfrost.so`, `libpanvk_drm.so`, and `SHA256SUMS`. All 3 embedded file checksums pass. Both ELF files are little-endian ELF64 AArch64 with Android API 30 ident notes. Driver RUNPATH is `$ORIGIN`; driver NEEDED includes bundled `libpanvk_drm.so` plus Android platform libraries; helper NEEDED is only `libc.so`. Metadata version is `26.3.0-devel-android-kbase-73873d730110` and explicitly says experimental/non-conformant with device/game compatibility unverified. Outer stage contains ICD manifest `panfrost_icd.aarch64.json` with API 1.4.354, staged driver, Android `libdrm.so` stub, and expat files. Audit reports 24 undefined DRM imports, 212 helper definitions, and an empty missing-symbol report. This is static/package validation only, not Winlator runtime validation.
- Freshly downloaded wrapper Release artifact [14162259759](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162259759), 4,288,198 bytes, outer SHA-256 `438e864926675ef1deda056d09576e8d2acf22fe214b632f5c974ec1166e3bc9`; Debugoptimized artifact [14162119843](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955/artifacts/14162119843), 50,651,760 bytes, outer SHA-256 `ee8ebb9c15fd47a10ee224537d1908b0e3f4e943cd61fbac554473c050896f44`. Both match GitHub digests, pass ZIP integrity, and contain only `libvulkan_panfrost.so` (ELF64 AArch64, Android API 30). Both use RUNPATH `$ORIGIN/../../android_stub`; NEEDED includes Android platform libraries plus external `libdrm.so`, X11/XCB variants, and `libxshmfence.so.1`. Neither wrapper archive contains the dependency closure, ICD manifest, or checksum manifest; they remain raw wrapper build outputs, not standalone Winlator packages.
- Compare from executable source `73873d730110bf3394ceea19b2a4d45ddba73834` to `experiment/g720-performance-audit` reports 30 commits ahead / 0 behind and only `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` modified. No executable driver or workflow change is present on the active branch beyond the tested source. The separate timestamp-correction candidate `f5084ae0` and safe-VRS candidate `b5e30b37` remain isolated as documented; do not conflate their artifacts or results with active source.
- No new Xiaomi 15T/Winlator import, Vulkan loader/device-creation, dynamic-VRS, heap-threshold startup, descriptor-profile, stability, or FPS evidence was available. No source, build workflow, kernel, Kbase ABI, Android system/vendor driver, or device configuration was changed; no build was triggered. Next gate remains testing the separately packaged Android flat driver through Winlator's custom-driver importer with System Driver retained as fallback, capturing ICD load, physical/logical device creation, rendering, and teardown. Keep the timestamp-correction validation separate from the 73873 heap/performance baseline.

## 2026-11-23 17:03 Africa/Tunis — independent branch, Actions, package, and source re-audit

- Re-read the canonical handoff and this audit before checking GitHub. Branch inventory returned 32 refs with no new branch names. Executable source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. Current audit HEAD `ac95172862cf187fad99f9a57b1f4cc8478c9c99` is 31 ahead / 0 behind and differs from executable source only in this audit Markdown. Canonical handoff HEAD is `f424d7c79b491ee59030dd35e1162e35b97efe59`; its three commits since `cc7f509d` modify only the handoff file.
- Fresh branch-filtered Actions query returned 139 runs for this audit branch. Latest exact-source Android run `43716474968` is SUCCESS (15/15); wrapper `43716474955` is SUCCESS (Release and Debugoptimized 12/12 each), all at exact source `73873d73`. The repository-wide first page of 100 newest runs is topped by Nov 19 run IDs `44553408360`/`44553408179`; no newer run appeared. This is a current source/CI inventory, not a new build.
- Re-fetched jobs for timestamp `f5084ae0`, descriptor heap `04947cb9`, and safe-VRS source `b5e30b37`. Android jobs are 15/15 successful; wrapper variants are 12/12 each; safe-VRS capability probe is 7/7. Current branch tips after those builds are documentation-only: dynamic FSR `7504f4d7` vs tested `c4f091e8` (+5 docs), fragment shading rate `95f6ea30` vs tested `c2eb0b61` (+1 doc), and safe VRS `bdfbbfb3` vs tested `b5e30b37` (+5 docs).
- Re-downloaded and independently inspected active Android artifact `14163230171`, timestamp artifact `14240094671`, and safe-VRS artifact `14542499046`. Outer hashes match GitHub artifact digests. Nested flat-package hashes: active `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`; timestamp `28752b6aa0ec594af9f6334fbe61e4d1211b46b68b15d98f1b83c86b249265e9`; VRS `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. Correction: the timestamp nested hash transcribed in earlier audit entries was wrong; the value above is recomputed from the downloaded ZIP and matches its embedded audit hash file. All three inner ZIPs pass integrity and embedded checksums; four expected members; ELF64 AArch64/Android API 30; driver RUNPATH `$ORIGIN`; private helper NEEDED only `libc.so`; 24 DRM imports / 212 helper exports / zero missing; valid staged ICD API 1.4.354.
- Re-downloaded Release and Debugoptimized wrapper artifacts for all three candidate families. ZIP digests match GitHub metadata and integrity passes. Each is a single AArch64/API 30 ELF with RUNPATH `$ORIGIN/../../android_stub`; NEEDED includes external libdrm, X11/XCB, and xshmfence. These are not standalone Winlator packages. Artifact `14544685368` is only the x86-64 Windows VRS capability probe.
- The actual render probe is artifact `13685924631`, run `42645674514`, built at source `c4f091e8`. It is unexpired and its ZIP digest is `8c70c4cae93e1f12bd8d7b4d284f63e1ca1686fa3a38ce8ce233b309c8812af4`. The safe-VRS source `b5e30b37` is 7 commits after c4f; its diff changes the Android workflow and two physical-device files only, not the Windows render-probe source. The capability-probe run `44553408360` did not build a new render-probe executable.
- Active timestamp defect remains confirmed in source: Kbase reports GPU timestamp-query support from nonzero GPU ID while frequency is zero; active Vulkan timestampValidBits, timestampComputeAndGraphics, calibrated timestamp/shader-clock exposure can therefore disagree with a zero timestampPeriod. Isolated candidate `f5084ae0` disables Kbase GPU timestamp exposure and gates the related properties/features/extensions on valid frequency and non-Kbase path. It is CI/package validated only; no target-device result.
- Descriptor-heap branch `04947cb9` remains diverged (62 ahead / 68 behind) and blocked. `cmd_bind_desc_heap()` marks both graphics and compute descriptor state dirty; the actual defect is missing bidirectional invalidation between heap and non-heap descriptor/push-descriptor state, including CmdPushDataEXT. Do not import or merge it.
- Active Kbase source retains interval/work defaults 128/65,536 and C11 call_once initialization. SYSTEM-scope SYNC_ADD64 remains default; CSG scope is diagnostic-only. CQS64 wait slices are capped at 20 ms, periodic re-kick is 500 ms, watchdog 10 s. Kbase SAME_VA BO mappings are established once at allocation and retained for BO lifetime. Userspace cache operations are used where supported unless `PANVK_KBASE_USER_CACHE_SYNC=0` selects the kernel sync path. Classic descriptor pools still allocate one contiguous BO with 64-bit sizing/overflow checks; no segmented allocator.
- Core-mask source intersects requested masks with Kbase `shader_present`; do not hard-code seven compiler workers or dispatch lanes. The active extension table does not advertise `VK_KHR_ray_query` or acceleration structures. Keep those features disabled until complete driver implementations exist.
- VRS audit comment “G720 runtime probes fault on every non-KEEP combiner” is not a justified hardware conclusion: the earlier non-KEEP probes were invalid for the advertised pipeline-only feature subset because primitive/attachment rates and nontrivial combiners are disabled. Keep the conservative feature advertisement; clarify the source comment in a future isolated change.
- No new device logs, Winlator import, runtime validation, stability result, or FPS sample. No phone bridge, kernel, Kbase ABI, system/vendor driver, or source code was changed. Next gate: smoke-test timestamp artifact `14240094671` through Winlator's separate custom-driver importer with System Driver retained as fallback; capture ICD/device creation, timestamp properties, minimal render/submit, and teardown.



## 2026-11-24 16:48 Africa/Tunis — exact-head job, branch, and Android package revalidation

- Re-read the canonical handoff before this audit. Branch search returned 32 names with no new branch names. Active executable source remains `73873d730110bf3394ceea19b2a4d45ddba73834`. At audit start, the performance-audit branch was 35 ahead / 0 behind active source and changed only this audit document. The handoff branch was one documentation-only commit beyond its 13:45 pre-update head. The two documentation commits recorded in this entry add no driver source or workflow changes. Agent, descriptor-heap, dynamic-FSR, fragment-shading-rate, timestamp-correction, and compiled-default heap-renewal branch tips match their previously recorded SHAs. Safe-VRS branch is six docs-only commits beyond tested source b5e30. The 32-branch comparison found no new experiment/work branch; older CSF/profile and packaging branches are behind or diverged and must not be merged into the active source. `main` has no common ancestor with the active experimental source.
- Re-fetched exact-head Android and wrapper job states and full logs for active source 73873, timestamp candidate f5084, and safe-VRS source b5e30. All jobs are completed/success: active Android 15/15; active wrapper Release/Debugoptimized 12/12 each; timestamp Android 15/15 and wrapper 12/12 each; VRS Android 15/15 and wrapper Release/Debugoptimized 12/12 each plus capability probe 7/7. Logs confirm Android `SOURCE_SHA` and wrapper checkout SHA exactly match each candidate. Active runs: [43716474968](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474968), [43716474955](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43716474955); timestamp: [43900657540](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657540), [43900657634](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/43900657634); VRS: [44553408179](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408179), [44553408360](https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf/actions/runs/44553408360). The available GitHub connector has no repository-wide workflow-run listing action in this pass; the 13:45 branch-filtered inventory remains the latest inventory evidence (139 runs, no newer active-source Android-only/wrapper run). No build was triggered.
- Downloaded and re-audited Android artifacts 14163230171, 14240094671, and 14542499046. Outer SHA-256 values respectively: `ed4f080a12c02587a0948d0089ba716482522cffae1e70ca973f604a64d9c1d8`, `1bbd821ecef8c541267671bea86d4a2a6852ed1a8033366072f2e990e67d3e74`, and `231b4118b24c252b04a6c686becfacad5127441e255075c223ab9bf68bbca309`. Nested flat-package SHA-256 values respectively: `824b8af7149f764697a33ac2b39e7badaad66d19e89d835784c7d02f1152d318`, `28752b6aa0ec594af9f6334fbe61e4d1211b46b68b15d98f1b83c86b249265e9`, and `a3a985334089846d0132222769d48d43298054056a6c5123c9a84cb7a66b5018`. All outer/inner ZIPs pass integrity; four flat-package members and embedded SHA256SUMS pass. Both ELF files in each package are ELF64 AArch64, Android API 30; driver RUNPATH `$ORIGIN`; helper NEEDED only `libc.so`; driver has bundled private helper plus Android platform NEEDED libraries. Each has 24 unique DRM imports, 212 helper exports, zero missing symbols, and staged ICD API 1.4.354. Metadata continues to say experimental/non-conformant and device/game compatibility unverified. This is static package validation, not runtime validation.
- Rechecked wrapper artifact metadata: active Release 14162259759 / Debugoptimized 14162119843; timestamp Release 14242286679 / Debugoptimized 14241846836; VRS Release 14543539493 / Debugoptimized 14544716174. These are raw AArch64 wrapper ELF outputs with external libdrm/X11/XCB/xshmfence dependencies, not standalone Winlator packages. The VRS capability probe 14544685368 is not the render probe 13685924631.
- Active timestamp inconsistency and isolated f5084 correction remain as previously documented. Descriptor heaps remain blocked by missing bidirectional invalidation; keep the feature disabled. Pipeline-only VRS remains the only exposed FSR subset in the candidate. The historical dynamic-2x2 device probe from 2026-11-08 completed submit/fence/readback with 1024 invocations but failed its pixel-signature comparison against static 2x2; no dynamic-FSR runtime pass is established. The VRS source comment claiming all non-KEEP combiners fault on G720 still overstates the valid-usage evidence.
- No new device-side Winlator import/log, timestamp smoke test, dynamic-VRS result, descriptor profile, heap-threshold proof, stability result, or FPS sample was available. No device bridge, kernel/system-driver change, or PanVK source change was made.
- Next gate: test the separate f5084 timestamp-correction Android flat package through Winlator's custom-driver importer with System Driver retained as fallback; capture loader/ICD, physical/logical device creation, timestamp properties/extensions, one minimal render/submit, and teardown. Keep this separate from performance A/B.

