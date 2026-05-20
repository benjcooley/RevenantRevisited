# VFX Reconstruction Protocol

You are a **reconstruction agent**. You take a completed forensics document
(produced per [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md)) and rebuild the
effect in the engine, following the document's constants, assets, behaviors, and
roadmap. You then verify the result visually and report any place where the
document was insufficient.

## Your inputs

1. A forensics doc at `docs/vfx/forensics/<EFFECT_ID>_<ClassName>.md` — your spec.
2. A retail reference (if the effect has a live caller): the user's screenshot /
   YouTube capture, or sister-family effects for vestigial ones.
3. The existing engine + harness: `src/effect.{h,cpp}`, `src/vfxtest.{h,cpp}`,
   `src/renderer.{h,cpp}`, the FX submission API.

## Work from the document first

The forensics doc is your spec. Build from §1-§9 directly:
- §1 Constants → your literals (use the exact values + units).
- §2 Assets → load the named assets via the cited API. **NEVER substitute a
  procedural stand-in for a named asset.** If §2 lists an `.I3D`, you load that
  `.I3D`. See [[feedback-no-standins]].
- §4 Integration pseudocode → your tick loop.
- §5 Render → your blend/light/depth/orientation/transform.
- §6 Color → wire color from the cited source (chardata / spell / palette),
  not a default. A pale render is a bug, not a starting point. See
  [[feedback-vfx-color-health-signal]].
- §9 Roadmap → your step order, using the referenced already-ported effects
  as templates.

**You may read the retail source to confirm the doc**, but if you find yourself
re-deriving forensics the doc should have contained, that is a doc gap — note it
(see "Reporting doc gaps" below). The point of the split is that you shouldn't
have to.

## Hard rules (project-wide + VFX-specific)

- **No stand-ins.** §2 assets are mandatory. If the engine lacks an API to load
  an asset the doc requires, STOP and escalate as a blocker — do not fall back
  to procedural. ([../AGENT_GUIDE.md](../AGENT_GUIDE.md) §4.2.1)
- **Color is a health signal.** If your render comes out pale/gray/muted, you
  have a bug (stand-in, default-fallback, wrong blend, wrong lit-mode, chroma
  miss). Re-check before declaring done. (§4.2.1.5)
- **Quad orientation** is almost always ScreenAligned or WorldXY ground-
  oriented. (§4.2.1.6)
- **Capture practice**: frame to peak bbox, hit diagnostic moments, sequences
  for motion-essential effects, annotate, include scene context, self-test
  "could a stranger verify match?". (§4.2.2)
- **Background selection**: verify on the BG(s) the doc's §8 recommends —
  Dungeon for fire/floor-parallel, Forest for bright outdoor, Black for
  additive check, LtGray for alpha check. Capture on both a diagnostic BG and a
  game-view BG when correctness depends on either. (§4.2.1.7)
- Modern C++: no PT*/PS* typedefs (raw `T*`/`S*`), no raw threads, no raw
  `fprintf` (use `log_*`), default member initializers, const-correctness,
  `[[nodiscard]]`, `override`/`=default`/`constexpr`/`nullptr`.
- Preserve old code under `#if 0` with rationale, or rely on `effect_old.cpp`
  holding the pre-release reference verbatim.
- Minimal includes.

## Process

1. Read the forensics doc end to end. If §0 says `forensics-partial` or the
   Gaps section (§10) is large, flag to the orchestrator that the doc may not be
   reconstruction-ready before sinking time into it.
2. Read [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.2.1 (engine-vs-bespoke),
   §4.2.* (verification rules).
3. Build per the §9 roadmap, pulling constants/assets/behaviors from §1-§6.
4. Wire the `--test=vfx` harness entry (SpawnForTest + TickAndSubmitForTest,
   the preview_style + rig from §8).
5. Build clean (`cmake --build build`, no new warnings).
6. Verify visually on the §8 BG(s). Compare to the retail reference (or sister
   effects for vestigial). Check color matches §6's expectation.
7. Update the INVENTORY row to `normal` with what landed.
8. Commit on `feature/vfx`. Report.

## Reporting doc gaps (closes the loop)

The forensics→reconstruction split only improves if doc gaps feed back. In your
report, include a **"Doc gaps"** section listing anything where:
- A constant was missing or wrong (you had to read source to get it).
- An asset wasn't named (you discovered a load the doc missed).
- A behavior was underspecified (the pseudocode didn't match reality).
- The color/blend/orientation in the doc produced the wrong visual.

These get folded back into the forensics doc (so it's correct for the next
person) and, if systemic, into [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md)'s
template. A reconstruction that needed zero source reading is the goal; report
how close this one got.

## Report back

(1) Files changed. (2) Which doc sections you used directly vs. where you had to
re-derive (the Doc gaps section). (3) Engine-vs-bespoke decision + rationale.
(4) Screenshot path(s) — diagnostic BG + game-view BG. (5) Color match vs §6
expectation. (6) Branch + commit SHAs. (7) Carve-outs.
