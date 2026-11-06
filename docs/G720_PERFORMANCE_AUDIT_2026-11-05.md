# Mali-G720 PanVK performance audit — 2026-11-05

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