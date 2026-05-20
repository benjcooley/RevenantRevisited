# VFX Knowledge Base

Common reference for how Revenant's **original (1998/99)** visual-effects system
worked. This is the shared background a forensics agent reads before documenting
any single effect, and that a reconstruction agent leans on to interpret a
forensics doc. Everything here describes the original engine, grounded in source
with `file:line` citations.

Read in this order:

1. **[05_EFFECT_ARCHETYPES.md](05_EFFECT_ARCHETYPES.md)** — the author's-eye-view
   taxonomy. The handful of recurring patterns nearly every effect is built from
   (UV/texture animation, I3D geometry, world-oriented shapes, trails/beams,
   rising/swirling/simple particle emitters, custom procedural logic, associated
   lights, audio, complex composites). **Start here** — it tells you what kind of
   thing you're looking at before you dive into mechanism.

2. **[01_ARCHITECTURE.md](01_ARCHITECTURE.md)** — effect architecture & lifecycle:
   the `TEffect` base, the Effect+Animator split, the lifecycle methods
   (Initialize/Pulse/Animate/Render/RefreshZBuffer) and their cadences, the
   `DEFINE_BUILDER`/`REGISTER_3DANIMATOR` registry, the `MapPane.NewObject` spawn
   path, `spell.def` VARIANT→effect cast wiring, and sub-effects.

3. **[02_ASSETS_IMAGERY.md](02_ASSETS_IMAGERY.md)** — the asset system: `.I3D`
   format + sub-objects, the imagery registry (`Class.Def` / `RegisterImagery` /
   `FindImagery` / `LoadImagery`), the `framehtexs[]` per-frame flipbook vs.
   **UV-coordinate animation** (two distinct texture-animation mechanisms),
   chroma-key/premultiplied-alpha, asset locations + the Magic-vs-Misc duplicate
   pattern, and sound coupling.

4. **[03_RENDERING_CONVENTIONS.md](03_RENDERING_CONVENTIONS.md)** — the original
   D3D render states (the two blend helpers `SetBlendState`/`SetAddBlendState`,
   lit vs self-lit, depth/Z + `RefreshZBuffer`), the iso projection + coordinate
   spaces, rotation/byte-angle conventions, per-vertex color packing + curves,
   the `math3d` helpers, and strip/billboard/trail/beam geometry construction.

5. **[04_EFFECT_CATALOG.md](04_EFFECT_CATALOG.md)** — the catalogue of ~70 effect
   classes across 12 families, each with its asset, trigger, archetype, behavior,
   and source citation, plus the sister-family rendering signatures (fire = warm,
   magic = cool, combat = per-character) used to sanity-check a single effect
   against its siblings.

## How this fits the workflow

- A **forensics agent** ([../FORENSICS_PROTOCOL.md](../FORENSICS_PROTOCOL.md))
  reads this knowledge base, then documents one effect into a standard
  per-effect doc, citing the original source.
- A **reconstruction agent**
  ([../RECONSTRUCTION_PROTOCOL.md](../RECONSTRUCTION_PROTOCOL.md)) uses the
  per-effect doc + this knowledge base to rebuild the effect in our engine.
- Shared vocabulary lives in [../NOMENCLATURE.md](../NOMENCLATURE.md).

## Known reconciliation items (flagged by the authoring agents)

These surfaced while writing the knowledge base; track + resolve as encountered:

- **~22 effect classes have no INVENTORY row** — the `effect2.cpp` cluster
  (Para / ManaDrain / Shriek / Puke / Funnel / Invisible / Rest / Cataclysm /
  IceStorm) plus CreateFood / Cure / FireFlash / FireWind / FireCone / Burn /
  SetVortex / Barrier / IrisFlare / MistFog / Fountain / Spark / Smoke. See
  [04_EFFECT_CATALOG.md](04_EFFECT_CATALOG.md) discrepancies section.
- **Naming surprises:** `TReviveEffect` registers as `"Ribbon"`;
  `TLightningAnimator` registers as `"LightStrip"`; the Tornado spell casts
  `"Funnel"`, not `"Tornado"`.
- **Vestigial (declared, never registered):** `TFireEffect`, `TFireAnimator`,
  `TIceEffect`, `TIceAnimator`, `TBallAnimator`.
- **INVENTORY I22 conflict:** says `TIcedEffect` has "no class," but pre-release
  has `DEFINE_BUILDER("Iced", TIcedEffect)` (`effect_old.cpp:8723`).
- **`legacy/` is gitignored** and absent from VFX worktrees — read `Class.Def`
  and the `Imagery/` asset tree from the main checkout.
