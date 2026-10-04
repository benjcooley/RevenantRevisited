---
name: vfx-forensics
description: This skill should be used when the user asks to "do forensics on" a Revenant visual effect, "document" an effect for reconstruction, produce a "forensics doc" for a VFX effect, or dispatch a forensics-phase agent. It produces a complete, self-contained document describing how the ORIGINAL (1998/99) effect worked — constants, assets, spawn shape, full custom-logic pseudocode, render spec, texture animation, associated light, color, audio, triggers, gaps, and a reconstruction burndown — and writes NO engine code.
version: 0.2.0
---

# VFX Forensics

Produce a complete document describing how one Revenant visual effect worked **in
the original (1998/99) engine**. You do NOT write engine code, and you do NOT
decide how it'll be rebuilt in our engine — the document is the deliverable; a
separate reconstruction agent rebuilds the effect from it.

The full protocol, standard document template, and source-of-truth order are the
version-controlled source of truth at:

**`docs/vfx/forensics/FORENSICS_PROTOCOL.md`**

Read that file in the VFX worktree
(`/Users/benjamincooley/projects/RevenantRevisited/worktrees/vfx`) and follow it
exactly. Read first: `docs/vfx/forensics/knowledge/` (how the original VFX
systems worked), `docs/vfx/forensics/NOMENCLATURE.md` (shared vocabulary), and
the rules in `docs/vfx/AGENT_GUIDE.md` (§3.0–§3.1 forensics + source order,
§4.2.1.5 color = health signal, §4.2.1.6 orientation tells).

## Mission

Clearly and concisely communicate the FULL implementation of the original effect
so the reconstruction agent can reproduce it 100% — exactly as it appeared and
functioned. Cite the retail code liberally (the reconstruction agent follows your
citations). Document the ORIGINAL only — no engine/pipeline mapping, no rig/BG
recommendation, no rebuild roadmap; those are reconstruction decisions.

**Clean slate.** Work purely from the original's own source. Do not study or
reference any prior/in-progress port, and do not frame findings against a
previous implementation — forensics is objective archaeology of the original
(protocol hard-rule 8).

**Retail-vs-snapshot reconciliation (§2.1).** `effect_old.cpp` is a pre-release
SNAPSHOT, not the shipped game — it may differ from retail. You MUST actively
determine whether the snapshot matches what shipped (cross-check the retail
Ghidra decomp: constant grep, asset identity, struct/vftable, registration) and
set the doc's **Retail fidelity** rating (retail-confirmed / retail-partial /
snapshot-only) with evidence. Mark each constant retail-confirmed vs snapshot-
only. Don't launder snapshot-only values as confirmed.

## Quick reference (the protocol has the full 14-section template)

1. Locate + rank sources: retail decomp (`recon/classes/`), pre-release
   (`src/effect_old.cpp` / `effectcomp.cpp`), sister-family effects. (NOT the
   in-progress port — clean slate.)
2. Fill the standard template into `docs/vfx/forensics/<EFFECT_ID>_<ClassName>.md`:
   identity table + §1 summary, §2 sources, §3 constants, §4 assets (stand-in
   guard), §5 spawn/emit + spatial diagram, §6 behavior & per-frame logic
   (ALL custom logic — transforms, procedural placement, custom vertex animation,
   not just particles) + temporal diagram, §7 rendering, §8 texture animation
   (UV-scroll vs framehtexs), §9 associated light, §10 color, §11 audio,
   §12 triggers + where-to-see-in-game, §13 gaps, §14 reconstruction burndown.
3. **Trace every helper** — visual constants live below the lifecycle code.
4. **Cite every fact** with `file:line`. **Name every asset** with path/size/role.
5. **Flag gaps honestly** — never fabricate to fill a hole.
6. **Finish with the §14 burndown** — a flat checklist of every discrete thing to
   recreate. If you can't write a clean burndown item, you haven't documented it.
7. Update the INVENTORY row, commit on `feature/vfx`. No engine code.

The test of success: working from your doc plus the retail code it cites, the
reconstruction agent reproduces the effect 100% without discovering anything you
didn't point them at.
