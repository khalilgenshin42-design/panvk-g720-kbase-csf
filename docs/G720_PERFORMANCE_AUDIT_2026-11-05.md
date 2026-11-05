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
