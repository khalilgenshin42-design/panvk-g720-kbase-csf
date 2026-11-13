# Free PanVK engineering agent setup

This repository is public. GitHub Actions standard hosted runners are free for public repositories, so the existing Android-only and wrapper build workflows can remain the build farm. See GitHub's current billing documentation: https://docs.github.com/en/actions/concepts/billing-and-usage.

OpenCode is the open-source coding-agent runtime. It is not itself a model and does not make every model/API free. You must select a model/provider that currently offers a free quota, or run a local model. Free-provider availability and rate limits can change. Never paste an API key into Git, a prompt, an issue, or a log.

## What is already wired in this branch

- `AGENTS.md`: project-specific engineering rules and hard safety limits.
- `opencode.json`: selects the project PanVK primary agent.
- `.opencode/agents/panvk-engineer.md`: implementation agent; source edits are enabled, shell commands require approval by default.
- `.opencode/agents/panvk-reviewer.md`: read-only review subagent.
- Android-only CI push filter includes `agent/free-panvk-agent`.
- Existing wrapper workflow already runs on pushes. Existing workflows still perform the actual builds; this setup does not add an untrusted PR auto-write bot or auto-merge action.

## Run OpenCode on the Android phone

OpenCode's supported Linux ARM64 binary expects a normal Linux userspace. On Android, use a Debian userspace inside Termux's proot-distro rather than trying to execute a glibc Linux binary directly in Android's Bionic environment. This does not require root.

In Termux:

```sh
pkg update
pkg install -y proot-distro git
proot-distro install debian
proot-distro login debian
```

Inside the Debian shell:

```sh
apt update
apt install -y ca-certificates curl git nodejs npm python3 python3-venv build-essential
npm install -g @opencode/cli
opencode --version
```

Official install and platform notes: https://opencode.ai/docs/ and https://opencode.ai/product.

Clone the agent branch inside Debian (or use a verified bind-mounted existing checkout if you already know how to expose it safely):

```sh
mkdir -p ~/src
cd ~/src
git clone --filter=blob:none --branch agent/free-panvk-agent https://github.com/khalilgenshin42-design/panvk-g720-kbase-csf.git
cd panvk-g720-kbase-csf
git fetch origin docs/project-continuity-handoff-2026-11-03
opencode
```

OpenCode reads the repository's `AGENTS.md` and project agent configuration. In OpenCode, use its provider connection flow (for example, `/connect`) and select a model/provider that explicitly shows a free tier. Then select `panvk-engineer`. Use `@panvk-reviewer` for a read-only second-pass review.

The agent is configured to ask before shell commands by default. This is intentional: approve the individual Git/build/test commands you understand. It may edit source in the topic branch, but it must not push or merge without explicit approval.

## Git write access

Public clone/fetch is anonymous. To push branches, configure GitHub SSH authentication inside the Debian environment:

```sh
ssh-keygen -t ed25519 -C "panvk-termux-agent"
cat ~/.ssh/id_ed25519.pub
```

Add the displayed **public** key to GitHub Settings → SSH and GPG keys. Keep `~/.ssh/id_ed25519` private; never upload or paste it. Test with `ssh -T git@github.com`, then change the repository remote to the SSH URL if desired. The agent should push only topic branches and open PRs; never enable auto-merge.

## Build/test division of labour

- Use GitHub Actions for Android ARM64 and wrapper compilation. Do not attempt repeated full Mesa builds on the phone unless there is a specific reason.
- The Android-only package workflow performs the existing ELF/ICD/dependency/package checks. Inspect artifact hashes and contents after each build.
- The phone is the hardware validation target. CI cannot prove Vulkan device creation, Winlator compatibility, stability, or FPS.
- Keep device tests manual until a safe, verified Android control/telemetry bridge exists. Do not grant root or install a system driver. A future bridge can fetch an artifact, stage it for Winlator's custom-driver importer, and collect logs; it must not silently import/activate a driver or launch a game.
- The existing daily project monitor automations are separate from OpenCode. This branch does not create duplicate schedules.

## First run task

After connecting a free model, give the agent this task:

> Read AGENTS.md, fetch and read the canonical PROJECT_CONTINUITY_HANDOFF.md from docs/project-continuity-handoff-2026-11-03, then read docs/G720_PERFORMANCE_AUDIT_2026-11-05.md. Do not edit code yet. Report the exact current source commit, pending CI/artifact state, unresolved device tests, and the single highest-value next engineering task. Cite paths and SHAs.

## Important cost/availability caveat

OpenCode is free/open-source software; model inference is a separate service. GitHub Actions is free for standard hosted runners on public repositories, but artifact storage, abuse limits, and fair-use rules still apply. There is no honest promise of unlimited free AI inference or 120 FPS.
