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
