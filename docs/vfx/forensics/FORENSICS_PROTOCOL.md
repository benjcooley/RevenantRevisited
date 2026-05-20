# VFX Forensics Protocol

You are a **forensics/reconstruction agent**. Your sole job is to produce a
complete, self-contained reconstruction document for one Revenant visual
effect. You do **NOT** write engine code. The document you produce is the
deliverable — a different agent (the reconstruction agent, see
[RECONSTRUCTION_PROTOCOL.md](RECONSTRUCTION_PROTOCOL.md)) rebuilds the effect
from your document alone.

## Why this exists

Single-agent ports kept shipping stand-ins and missing constants because the
same agent who "understood it well enough" rushed into code. Separating
forensics from reconstruction removes the coding escape hatch: if your document
is incomplete, that is a visible forensics failure, not something buried in a
port. The reconstruction agent should be able to rebuild the effect **without
re-deriving anything** — every constant, asset, blend mode, color source,
orientation, and behavior must be in your document with a source citation.

The test of a good forensics doc: **a competent engine programmer who has
never seen the retail source could rebuild the effect from your doc and get a
visual match.** If they would have to go read `effect_old.cpp` themselves, you
left something out.

## Hard rules

1. **No code.** You produce a document. You may read all source, run greps, and
   inspect assets, but you do not edit `src/`. (You DO write your output doc and
   may update the INVENTORY row.)
2. **Cite every fact.** Every constant, every asset path, every behavior gets a
   `file:line` citation. "Roughly 15 particles" is useless; "`NUMFIRES = 15`
   (effect_old.cpp:889)" is the standard.
3. **Trace every helper.** When a render/tick/pulse body calls a helper
   (texture load, blend-state set, color packer, spawn dispatch, envelope
   function), follow it. Visual constants live one or two levels below the
   lifecycle code. If you find yourself unable to cite where a number comes
   from, you have not finished tracing. (This is the §3.0(8) rule from
   [../AGENT_GUIDE.md](../AGENT_GUIDE.md).)
4. **Name every asset.** If the effect loads a `.I3D` / `.RVI` / texture /
   sound, record the exact path, file size, and role. A reconstruction agent
   that doesn't know an asset exists will write a procedural stand-in — the
   #1 failure mode. See [[feedback-no-standins]].
5. **Flag gaps honestly.** Anything you cannot determine goes in the Gaps
   section with what you tried and what a reasonable guess would be (with
   rationale). Never silently fabricate a value to fill a hole.
6. **Use shared nomenclature.** All orientation / space / blend / lighting /
   depth / animation / pipeline / rig terms come from
   [NOMENCLATURE.md](NOMENCLATURE.md). Don't invent synonyms (say "WorldXY",
   not "floor-flat" or "ground billboard"). If you need a term that isn't
   defined there, add it to NOMENCLATURE.md with a definition.
7. **Draw diagrams.** Wherever spatial (emit shape, orientation) or temporal
   (envelope, phase timeline) behavior is clearer drawn than described, include
   an ASCII diagram per the conventions in [NOMENCLATURE.md](NOMENCLATURE.md)
   §9. A diagram orients the reconstruction agent fast; it supplements (never
   replaces) the constants table + pseudocode.

## Source-of-truth order

For any given effect (from [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.1):

1. **Retail decomp** under `recon/classes/` or `recon/discovered/` (Ghidra) —
   ground truth for *behavior*. Check the recon mapping. If sparse (ctor/dtor
   only), note it and fall back to pre-release.
2. **Pre-release source** under `src/effect_old.cpp` / `src/effectcomp.cpp` —
   authoritative for *intent*. Most effects' full bodies live here.
3. **Existing port shell** in `src/effect.{h,cpp}` — note what's already there.
4. **Sister-family effects** — when an effect's own source is sparse, a sibling
   in the same family (fire-family, magic-family, combat-family) usually shares
   the render/blend/color pattern. Cite the sibling.

If retail and pre-release disagree, **retail wins for behavior**, but record the
divergence — it is information.

## The document template

Write your output to `docs/vfx/forensics/<EFFECT_ID>_<ClassName>.md` (e.g.
`docs/vfx/forensics/M09_TTeleporterEffect.md`). Fill out **every** section. If a
section genuinely doesn't apply, write "N/A — <why>", don't delete it.

````markdown
# <EFFECT_ID> <ClassName> — Forensics & Reconstruction Spec

**Status:** forensics-complete | forensics-partial (see Gaps)
**Author:** <agent-id>  **Date:** <YYYY-MM-DD>
**Pipeline:** FB | PE | SR | LS | IM | VO | CX (composite)
**One-line:** <what this effect is, in one sentence>

## 0. Sources
- Retail decomp: `recon/classes/<file>` — <COMPLETE | SPARSE | NONE>
- Pre-release: `src/effect_old.cpp:<start>-<end>` (`<ClassName>` + `<AnimatorName>`)
- Existing port shell: `src/effect.{h,cpp}:<lines>` or "none"
- Sister effects consulted: `<ClassName>` (`effect_old.cpp:<lines>`) — <what for>
- Source-of-truth ranking for this effect: <which source is authoritative + why>

## 1. Constants (EVERY numeric, with citation)
| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| <NUMFIRES> | 15 | count | effect_old.cpp:889 | yes |
| <gravity> | 0.37 | wu/tick² | effect_old.cpp:10947 | yes |
| ... | ... | ... | ... | guessed: <rationale> |

Include: counts, lifetimes/durations, speeds, gravities, drag, sizes, scales,
rotation rates, spawn rates/cadence, burst counts, blend factors, atlas dims,
sim-tick gating period, and any magic numbers in the transform/color math.

## 2. Assets (THE STAND-IN GUARD)
| asset | path | size | role | load API |
|-------|------|------|------|----------|
| <gvortex> | legacy/Imagery/Magic/gvortex.I3D | 277KB | rotating cylinder mesh | TObjectImagery::RegisterImagery → ExtractSubMeshTextureSlot |
| ... | ... | ... | ... | ... |

For mesh assets, enumerate the **sub-objects** and what each renders (pivot,
cylinder, flare, blast, etc.). For atlas textures, record frame count + cell
dims + how frames are indexed. If the effect loads NO asset (pure procedural),
say so explicitly with the citation that proves it (e.g. the pre-release
"no texture" comment).

## 3. Spawn / emit shape
- **Trigger semantics:** one-shot | fixed-duration | continuous | looping
- **Count per trigger:** <N particles/quads/segments>
- **Initial direction / distribution:** <radial cone? caller vector? char-attached? polar? cite the math>
- **Emit anchor convention:** where does the visual ORIGINATE — at the spawn
  call's exact (x,y,z), or ground-projected, or character-relative? (cite)
- **Coordinate space:** World | Local (relative to anchor) — cite
- **Spread / jitter:** <distribution + range, cite>
- **Spatial diagram:** ASCII sketch of the emit shape — world axes, emit origin,
  spread, and quad facing (per [NOMENCLATURE.md](NOMENCLATURE.md) §9).

## 4. Per-frame integration (PSEUDOCODE)
Transcribe the Animate/Tick/Pulse loop as readable pseudocode. Capture:
- velocity integration + gravity/drag
- color curve over lifetime (start/end, easing, per-channel)
- alpha curve over lifetime (start/end, easing)
- scale curve over lifetime
- atlas frame advance (rate, wrap, per-instance offset)
- sub-emit (does it spawn child effects? when? what?)
- sim-tick gating (24Hz? frame-rate independent? ungated in pre-release →
  note the port must gate)
- state machine if any (phases, transitions, durations)

```
// Pseudocode — cite effect_old.cpp:<lines> for each block
on each sim tick (24Hz):
    age += 1
    vel.z -= gravity
    pos += vel
    alpha = lerp(0.3, 0.0, age01)
    if pos.z <= 0: spawn_ripple(pos); kill()
```

- **Temporal diagram:** ASCII envelope (value vs age01) and/or phase timeline
  for multi-stage effects, per [NOMENCLATURE.md](NOMENCLATURE.md) §9.

## 5. Render
- **Pipeline:** FB | PE | SR | LS | IM | VO
- **Blend mode:** <Alpha | Additive | AdditiveStraight | PremulAlpha | Decal>
  — TRACE the actual SetBlendState/SaveBlendState helper to get the real
  src/dst factors; cite. (Don't guess "additive" — F03 was wrongly guessed
  additive when it was Decal/Alpha.)
- **Light mode:** Unlit | LitFlat — cite
- **Depth mode:** TestNoWrite | TestWrite | None — cite
- **Orientation:** ScreenAligned | WorldXY (ground) — the `rot.x = -π/2`
  pre-release tell means WorldXY. Cite the transform.
- **Per-quad transform:** rotation (x/y/z), scale, offset — cite
- **Texture / atlas:** which texture slot, frame layout, UV mapping
- **Per-vertex color packing:** how diffuse/alpha get written (cite the PackARGB
  or equivalent)

## 6. Color spec (HEALTH SIGNAL)
- **Where color comes from:** chardata field | spell color | texture palette |
  hardcoded constant — cite the exact source
- **Exact values:** <RGB(A), 0-255 or 0-1, cite>
- **Expected visual:** what color SHOULD this read as in-game (warm orange fire?
  ice-blue? Locke's magenta swipe?). A reconstruction that renders pale/gray is
  broken — see [[feedback-vfx-color-health-signal]].
- **Normalization / boosts:** any NormalizeColors / hilt-brightness / tint math (cite)

## 7. Callers / triggers (for reference capture)
- **Spawned by:** spell.def variant(s) | monster attack | scripted scene |
  environmental/ambient | vestigial (no live caller) — cite the registration
  (DEFINE_BUILDER / REGISTER_3DANIMATOR / spell.def line / ATTACHEFFECT)
- **In-game locations to capture reference:** <specific: "cast Teleport in any
  safe spot", "BLOODMIRE NW ambient mist", "every melee hit on Locke">
- **Vestigial?** If no live retail caller, say so — reference must come from
  asset inspection + sister effects, not in-game capture.

## 8. Test rig + background
- **Rig category:** Standalone | Character-attached | Character+weapon+attack |
  Projectile | Spell-cast | Environment
- **Recommended BG for verification:** Dungeon (fire/floor-parallel) | Forest
  (bright outdoor) | Black (additive check) | LtGray (alpha check) — and which
  diagnostic each confirms. Many effects warrant BOTH a diagnostic BG and a
  game-view BG (see [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §4.2.1.7).

## 9. Reconstruction roadmap (ordered steps for the rebuild agent)
1. <Load asset X via API Y — see <existing ported effect> as template>
2. <Allocate bucket / billboard with blend=Z, light=W, depth=V, orientation=U>
3. <Implement spawn: N particles, distribution D>
4. <Implement tick: integration + curves per §4>
5. <Wire harness entry: SpawnForTest + TickAndSubmitForTest, preview_style=P, rig=R>
6. <Verify on BG per §8; expected color per §6>

Reference the closest already-ported effect for each step (e.g. "asset load:
mirror H04 TDripEffect's `LoadImagery` + `GetTexture(0)`; mesh draw: mirror M09b
TTeleporterEffect's `SubmitHelperMesh`").

## 10. Gaps / unknowns
- <what you couldn't determine, what you tried, reasonable guess + rationale>
- <anything that needs visual iteration at reconstruction time>
- <carve-outs: follow-up work beyond the core port>
````

## Process

1. Read [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.0, §3.1, §3.2.1, §4.2.1.5,
   §4.2.1.6, §4.2.1.7.
2. Read the INVENTORY row for your effect — it may already have partial
   forensics. Build on it, don't redo.
3. Locate all sources (§0). Rank them.
4. Fill the template top to bottom, tracing every helper, citing every fact.
5. Self-test: "Could a stranger rebuild this from my doc and get a visual
   match, without reading the retail source?" If not, keep going.
6. Update the INVENTORY row: `Status` and a one-line "forensics doc at
   docs/vfx/forensics/<file>".
7. Commit the doc (+ INVENTORY edit) on `feature/vfx`. Don't write engine code.

## What good looks like

`docs/vfx/M09_FORENSICS.md` (the teleporter) and the H04 TDripEffect INVENTORY
entry are the gold-standard examples of the depth expected — constants tables,
helper-traced blend modes, asset enumeration, pseudocode, gaps flagged.
