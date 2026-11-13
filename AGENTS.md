# PanVK G720 engineering agent rules

## Mandatory context and target

Before any engineering task, read the canonical handoff from its documentation branch and the current performance audit:

```sh
git fetch origin docs/project-continuity-handoff-2026-11-03
git show origin/docs/project-continuity-handoff-2026-11-03:PROJECT_CONTINUITY_HANDOFF.md
sed -n '1,260p' docs/G720_PERFORMANCE_AUDIT_2026-11-05.md
```

Repository: `khalilgenshin42-design/panvk-g720-kbase-csf`.
Target: Xiaomi 15T / MT6899 / Dimensity 8400 Ultra / Arm Mali-G720 MC7, Android 16, Winlator Mali 1.2. User is non-rooted; device-side build/test access is through Termux and Winlator.

## Non-negotiable safety constraints

- Never replace, patch, sideload over, or instruct the user to modify the Android system/vendor Vulkan driver, boot image, kernel, or Mali kernel module.
- Keep experimental packages separate and import only through Winlator's custom-driver mechanism. Preserve System Driver as rollback.
- Do not use root-only or kernel-write techniques. Do not assume ADB, Shizuku, wireless debugging, or Winlator UI automation is available until explicitly verified.
- Never expose an extension/feature merely because the GPU architecture might support it. Require a complete implementation, spec-correct feature/property reporting, positive/negative tests, and runtime validation. In particular, do not advertise ray query or hardware ray tracing without a real, correct implementation.
- Never hardcode seven compiler workers, queues, or execution contexts from the MC7 marketing/core count. Inspect the kernel-reported GPU topology and the actual Mesa scheduler/compiler model first.
- Do not tune or alter Kbase ABI, CSF queue/ring sizes, heap policy, descriptor layout, memory flags, or synchronization semantics without tracing the complete producer/consumer and lifetime rules and adding focused tests.
- Never treat a successful compile/package as proof of Vulkan device creation, game compatibility, stability, or performance.
- Never claim a frame-rate improvement from one run. Use matched, repeated A/B samples and retain minimum/average/maximum, thermal state, settings, driver commit, package hash, and logs.

## Repository workflow

- Treat `PROJECT_CONTINUITY_HANDOFF.md` and `docs/G720_PERFORMANCE_AUDIT_2026-11-05.md` as durable project state. Git history, exact workflow runs, artifacts, and user-provided logs are the evidence.
- Preserve existing experimental branches. Do not reset, force-push, delete, or casually merge them.
- Start each independent experiment from the correct current source branch and create a new descriptive branch. Keep unrelated VRS, descriptor-heap, heap-renewal, clip/cull, and sync experiments isolated until compatibility is audited.
- Inspect `git status`, branch, base SHA, and diff before editing. Never overwrite user changes.
- Prefer small, reviewable commits. Keep build outputs and generated packages out of Git unless the repository already explicitly versions them.
- Update the canonical handoff on its documentation branch after meaningful source changes, CI results, package audits, and user device benchmarks. Include exact SHAs, workflow IDs, artifact IDs/hashes, test scope, and remaining gates. Do not fabricate a test result.
- Push work to a topic branch and open a PR for review; do not merge automatically.

## Engineering method

1. Establish the exact source commit, current branch, and clean/dirty worktree.
2. Reproduce the issue or establish a measurable baseline before changing code.
3. Trace the complete relevant path across Vulkan entrypoints, PanVK state, compiler/NIR lowering, CSF submission/synchronization, Kbase ioctl/mmap, and teardown as applicable.
4. Check upstream Mesa history and Arm/Mali/Kbase ABI/spec documentation before introducing vendor-specific assumptions.
5. State a falsifiable hypothesis and the smallest experiment that can test it.
6. Add a focused test or diagnostic before changing behavior where practical.
7. Make one logically isolated change at a time.
8. Run the narrowest relevant tests, then Android-only and wrapper CI. Inspect the actual logs and artifacts, not only green badges.
9. Audit package hashes, ELF class/ABI, RUNPATH/NEEDED closure, ICD manifest, and unresolved symbols before recommending device import.
10. Request user action only for physical-device steps or facts the cloud/local agent cannot observe. Give exact commands and explain rollback.
11. Record results in the handoff and identify the next experiment.

## Evidence labels

Every report must distinguish:
- source-reviewed
- compile-validated
- unit/test validated
- package validated
- Vulkan device-creation validated
- Winlator/game runtime validated
- performance validated

Never promote a result to a stronger evidence level without the corresponding test.

## Primary optimization objective

Improve real DXVK/VKD3D-Proton workloads in Winlator on the target Mali-G720 while preserving correctness and stability. The aspirational 120 FPS target is not a promise or a reason to trade away correctness. Prioritize profiling evidence, bottleneck isolation, and reproducible wins over speculative flags or broad "optimization" patches.
