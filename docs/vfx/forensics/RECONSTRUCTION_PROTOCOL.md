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

The forensics doc is your spec. It describes the ORIGINAL effect; **you** make
all the engine decisions (which pipeline, the test rig + background, the build
order). Build from the doc's sections:
- §3 Constants → your literals (exact values + units).
- §4 Assets → load the named assets via the original-load API the doc cites,
  mapped to our engine's loader. **NEVER substitute a procedural stand-in for a
  named asset.** If §4 lists an `.I3D`, you load that `.I3D`. See [[feedback-no-standins]].
- §5 Spawn & emit → your spawn shape, count, distribution, anchor, space.
- §6 Behavior & per-frame logic → your tick loop, transform animation, and any
  custom/procedural algorithms (transcribe the doc's pseudocode faithfully).
- §7 Rendering → your blend/lit/depth/orientation/transform (map the original
  D3D state the doc names to our `EFxBlend`/light/depth/orientation enums).
- §8 Texture animation → UV-scroll vs framehtexs flipbook, per the doc.
- §9 Associated light → wire the dynamic light (color/range/intensity/animation).
- §10 Color → wire color from the cited source (chardata / spell / palette), not
  a default. A pale render is a bug, not a starting point. See [[feedback-vfx-color-health-signal]].
- §11 Audio → record for the audio phase (out of your scope unless told otherwise).
- §12 Triggers → tells you where to capture ground-truth reference in the
  original game.

**Read the cited retail code freely** — the doc's citations point you straight at
it, and confirming details against source is expected, not a failure. A doc gap
is when the doc *omitted* something (a constant, an asset, a behavior) or pointed
at the wrong place — not merely that you opened `effect_old.cpp`. Report genuine
gaps (see below) so the doc improves.

**You own the engine decisions** the forensics doc deliberately does NOT make:
the pipeline (FB/PE/SR/LS/IM/VO/CX), the test-rig category + which background(s)
to verify against (Dungeon/Forest/Black/LtGray per
[../AGENT_GUIDE.md](../AGENT_GUIDE.md) §4.2.1.7), and the build order. Use the
closest already-ported effect as a template (e.g. asset load: H04 TDripEffect's
`LoadImagery` + `GetTexture(0)`; I3D mesh draw: M09b's `SubmitHelperMesh`).

## Hard rules (project-wide + VFX-specific)

- **No stand-ins.** The doc's §4 assets are mandatory. If the engine lacks an API
  to load an asset the doc requires, STOP and escalate as a blocker — do not fall
  back to procedural. ([../AGENT_GUIDE.md](../AGENT_GUIDE.md) §4.2.1)
- **Color is a health signal.** If your render comes out pale/gray/muted, you
  have a bug (stand-in, default-fallback, wrong blend, wrong lit-mode, chroma
  miss). Re-check before declaring done. (§4.2.1.5)
- **Quad orientation** is almost always ScreenAligned or WorldXY ground-
  oriented. (§4.2.1.6)
- **Capture practice**: frame to peak bbox, hit diagnostic moments, sequences
  for motion-essential effects, annotate, include scene context, self-test
  "could a stranger verify match?". (§4.2.2)
- **Background selection** (your call): Dungeon for fire/floor-parallel, Forest
  for bright outdoor, Black for additive check, LtGray for alpha check. Capture
  on both a diagnostic BG and a game-view BG when correctness depends on either.
  (§4.2.1.7)
- Modern C++: no PT*/PS* typedefs (raw `T*`/`S*`), no raw threads, no raw
  `fprintf` (use `log_*`), default member initializers, const-correctness,
  `[[nodiscard]]`, `override`/`=default`/`constexpr`/`nullptr`.
- Preserve old code under `#if 0` with rationale, or rely on `effect_old.cpp`
  holding the pre-release reference verbatim.
- Minimal includes.

## Process

1. Read the forensics doc end to end. If its Status is `forensics-partial` or the
   §13 Gaps section is large, flag to the orchestrator that the doc may not be
   reconstruction-ready before sinking time into it.
2. Read [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.2.1 (engine-vs-bespoke),
   §4.2.* (verification rules), and the cited retail code for any detail you want
   to confirm.
3. Work the doc's §14 reconstruction burndown as your task list — it's the flat
   checklist of every discrete element to recreate, each back-referencing the
   detail section. Decide the engine approach (pipeline, bespoke-vs-engine) and
   build, pulling constants/assets/behavior/render/color from §3–§10 as each
   burndown item directs. Check items off as you go; the burndown is your
   definition of done.
4. Wire the `--test=vfx` harness entry (SpawnForTest + TickAndSubmitForTest);
   choose the preview_style + rig category yourself from the effect's nature.
5. Build clean (`cmake --build build`, no new warnings).
6. Verify visually on the background(s) you judge most diagnostic (Dungeon /
   Forest / Black / LtGray). Compare to ground-truth reference at the in-game
   location the doc's §12 names (or sister effects for vestigial). Confirm color
   matches the doc's §10 expectation.
7. Update the INVENTORY row to `normal` with what landed.
8. Commit on `feature/vfx`. Report.

## Reporting doc gaps (closes the loop)

The forensics→reconstruction split only improves if doc gaps feed back. Reading
the cited retail code is normal and expected — that is NOT a gap. A genuine doc
gap is where the doc **omitted or misstated** something. In your report, include
a **"Doc gaps"** section listing anything where:
- A constant was missing or wrong (the doc didn't have it / had it wrong).
- An asset wasn't named (you found a load the doc missed entirely).
- A behavior was underspecified or the pseudocode didn't match reality.
- The color/blend/orientation/light stated in the doc produced the wrong visual.

These get folded back into the forensics doc (so it's correct for the next
person) and, if systemic, into [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md)'s
template. Report how complete the doc was — did it point you at everything, or
did you have to discover things it should have named?

## Report back

(1) Files changed. (2) How complete the forensics doc was (the Doc gaps section).
(3) Engine-vs-bespoke decision + rationale. (4) Screenshot path(s) — diagnostic
BG + game-view BG. (5) Color match vs the doc's §10 expectation. (6) Branch +
commit SHAs. (7) Carve-outs.
