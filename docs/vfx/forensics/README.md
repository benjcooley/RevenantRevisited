# VFX Forensics & Reconstruction

Two-phase workflow for restoring Revenant visual effects, separating
**forensics** (documenting how an effect worked) from **reconstruction**
(rebuilding it in the engine). Mirrors the UI track's panel-forensics approach.

## Why split it

Single-agent ports repeatedly shipped procedural stand-ins and missed visual
constants (M09 teleporter glow instead of the real `gvortex.I3D` cylinders; F03
fire procedural gradient instead of `Fire.I3D`; S09 swipe default-gold instead
of Locke's `chardata->swipecolor` magenta). The common cause: one agent
"understood it well enough" and rushed into code, where forensics gaps got
buried. Separating the phases removes the coding escape hatch — an incomplete
spec is a visible forensics failure.

## The two roles

1. **Forensics agent** — [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Reads
   retail decomp + pre-release source, traces every helper, and produces a
   complete reconstruction document: constants table, asset list, spawn shape,
   per-frame pseudocode, render spec, color source, callers, test rig + BG, and
   an ordered rebuild roadmap. Writes NO engine code.

2. **Reconstruction agent** — [RECONSTRUCTION_PROTOCOL.md](RECONSTRUCTION_PROTOCOL.md).
   Takes a forensics doc and rebuilds the effect from it, verifies visually on
   the recommended backgrounds, and reports any doc gaps so the document (and
   the protocol) improve over time.

The test of the handoff: **a competent engine programmer who never read the
retail source could rebuild the effect from the forensics doc and get a visual
match.** If they'd have to go read `effect_old.cpp`, the doc is incomplete.

## Invoking as skills

Local `/`-invocable wrappers live in `.claude/skills/vfx-forensics/` and
`.claude/skills/vfx-reconstruct/` (gitignored — these protocol docs in
`docs/vfx/forensics/` are the version-controlled source of truth). When
dispatching agents, point them at the protocol doc directly.

## Output docs

Per-effect forensics docs live here as `<EFFECT_ID>_<ClassName>.md`. The
[INVENTORY](../INVENTORY.md) row for each effect links to its doc and tracks
status (forensics-complete → reconstruction → normal).

Gold-standard examples of the depth expected:
- `../M09_FORENSICS.md` — the teleporter (constants, helper-traced blend, asset
  enumeration, pseudocode, gaps).
- The H04 TDripEffect INVENTORY entry — full 7-point template filled inline.

## Related

- [../AGENT_GUIDE.md](../AGENT_GUIDE.md) — overall VFX agent protocol; §3.0
  forensics rules, §4.2.* verification rules (no stand-ins, color health
  signal, orientation, capture framing, BG selection) that both phases follow.
- [../EFFECT_USAGE_MAP.md](../EFFECT_USAGE_MAP.md) — which effects to prioritize
  (asset-driven = High), where to capture in-game reference.
