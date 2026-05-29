# Effect Archetypes

The author's-eye-view of how Revenant's original effects behaved. Where the
[catalog](04_EFFECT_CATALOG.md) is the per-class index and the
[rendering conventions](03_RENDERING_CONVENTIONS.md) are the per-mechanism
detail, this doc is the **conceptual taxonomy**: the handful of recurring
patterns nearly every effect is built from. A forensics agent should read this
first to recognize, at a glance, "which archetype am I looking at?" — then go to
the catalog + conventions for the specifics.

These archetypes are not mutually exclusive. Real effects combine them (a
fireball is a moving I3D-ish billboard + a trail + a particle spray + an impact
burst). The complex ones (fire ring, meteor storm) are compositions of several.

---

## The building blocks

### A. Texture / UV animation (used extensively)

The single most pervasive technique. Effects animate their texture over time,
two distinct ways — **distinguish them in forensics**:

- **UV animation** — the texture coordinates scroll/animate over the quad while
  the same texture stays bound (flowing fire, streaming energy, scrolling beam).
  Look for per-frame UV offset / scroll-rate math in the animate body.
- **Frame flipbook (`framehtexs`)** — the bound texture handle is swapped each
  frame from a per-frame array (`framehtexs[]`), advancing a frame index. NOT a
  UV sub-rect atlas — each frame is its own engine texture. (F03 fire uses this.)

Many effects use one or both. Record which, the rate, wrap behavior, and any
per-instance phase offset. See [02_ASSETS_IMAGERY.md](02_ASSETS_IMAGERY.md) for
the mechanism detail.

### B. I3D geometry with transforms

Effects that draw a real `.I3D` mesh (or its sub-objects) and animate it with
**rotation, scale, and translation** over time — the teleporter's rotating
`gvortex.I3D` cylinders (M09b) are the canonical example. The visual identity is
in the mesh + its authored textures, NOT in procedural geometry. Look for
`rot.x/y/z`, scale, and per-tick rotation-rate math applied to a loaded imagery.
**Never substitute procedural geometry for an I3D draw** — see
[[feedback-no-standins]].

### C. World-oriented shapes, centered on the effect origin

A large fraction of effects render quads **oriented to the world** (lying on the
ground / WorldXY plane), **centered on the effect's origin point**, rather than
camera-facing billboards. Fire rings, ground halos, ripples, AoE markers,
ground-scatter fire. The pre-release tell is `rot.x = -π/2` tipping the
authored-XY quad onto the floor. The shape radiates from / is centered on the
spawn origin (often the ground-projected target point). See NOMENCLATURE
"WorldXY" and the emit-anchor convention.

### D. Trails and beams

- **Trail** — a fading streak that follows a moving emitter (fireball trail).
  Built from a ring buffer of recent emitter positions, splined into a strip,
  with color/alpha fading from head (bright, at the emitter) to tail (gone).
  Same SR-strip machinery as the sword swipe (S09).
- **Beam** — a stretched quad/strip spanning two endpoints (ice-freeze beam from
  caster to target). Two anchor points; the geometry stretches between them; the
  texture often scrolls along the beam (archetype A).

### E. Particle emitters

The workhorse. Two sub-flavors by motion:

- **Simple emitter** — particles spawn, drift, and **fade out (color/alpha) over
  their lifetime**. Color is rich and intentional (warm for fire, cool for ice,
  per [[feedback-vfx-color-health-signal]]). The minimum viable effect.
- **Rising particles** — upward velocity (smoke, mist wisps, heat shimmer,
  flame licks). Often with slight horizontal drift/jitter.
- **Swirling particles** — angular/orbital velocity around the origin axis
  (vortex, magic gather, summon swirl). Look for polar/rotational velocity math.

All three share the lifetime/age01 + color-curve + alpha-curve envelope. The
difference is purely the velocity field.

### F2. Custom procedural logic (capture it, don't summarize it)

Effects are not just particle systems — many run **bespoke per-frame code** that
defines their identity. Forensics must capture this logic in full (pseudocode),
not gloss it:

- **Transform animation** — the whole effect or its sub-objects scaling,
  rotating, translating over time (teleporter cylinder spin, rising fire column,
  pulsing ring). Capture the axis, rate, and curve.
- **Procedural placement / target selection** — e.g. meteor storm picking random
  scatter positions over time. Capture the RNG range, the count, the placement
  rule (radius around caster? screen-area scatter?).
- **Custom vertex / geometry animation** — e.g. ice-freeze deforming a mesh
  per-vertex. Transcribe the per-vertex math; do NOT summarize as "animates the
  geometry." This is exactly the kind of detail a reconstruction agent cannot
  re-derive.
- **Internal triggers / conditionals** — range checks, collision, timers,
  damage-in-radius hooks that gate the visual.

If you find yourself writing "and then it does some custom animation," stop and
transcribe the actual code. That sentence is a forensics failure.

### G. Associated dynamic light

MANY effects emit a **dynamic light** onto the scene (fire flickers warm light on
nearby walls, explosions flash, magic glows cast colored light). This is the LS
pipeline and is a first-class component of the effect, not an afterthought.
Capture the light's color, radius/range, intensity, how it animates (flicker /
pulse / fade), and whether it's pinned to the origin or follows a moving part.
An effect ported without its light reads flat and wrong even if the billboards
are perfect.

### H. Audio coupling (be on the lookout)

Effects may trigger sounds (`PLAY("drip")` on impact, looping ambience while
alive, a cast whoosh on spawn). Audio port is out of VFX scope, but record any
audio you find — the sound name + when it fires — so it isn't lost when the
audio phase arrives. If none, note "no audio coupling found."

### F. Complex composites

Effects assembled from several of the above, often as a small state machine or a
multi-emitter coordinator:

- **Fire ring** — a WorldXY ground ring (C) + rising flame particles (E) +
  animated fire texture (A), expanding/sustaining/collapsing over phases.
- **Meteor storm** — multiple projectile meteors (B/D, each a mesh + trail)
  spawned over time at scattered positions, each with an impact burst (E) on
  landing — a spawner coordinating many sub-effects (sub-emit pattern).

These need **careful decomposition** in forensics: identify each component, its
archetype, and how the parent coordinates timing/placement. Don't try to capture
a composite as one monolithic description — break it into its parts and document
the orchestration separately.

---

## Forensics checklist by archetype

When you identify the archetype(s), make sure the forensics doc captures the
archetype-specific facts:

| archetype | must-capture facts |
|-----------|--------------------|
| A. UV/texture anim | which mechanism (UV scroll vs framehtexs), rate, wrap, per-instance phase |
| B. I3D geometry | exact asset path + sub-objects, rotation axis + rate, scale, the load API (no stand-ins) |
| C. world-oriented shape | WorldXY orientation, center/origin convention, radius/extent, ground-projection |
| D. trail/beam | ring-buffer length (trail) or two endpoints (beam), spline params, head→tail color/alpha fade, UV scroll |
| E. particle emitter | spawn rate/burst, lifetime, velocity field (drift/rise/swirl), color curve, alpha curve, count |
| F. composite | each component + its archetype, the orchestration (phases/timing/placement), sub-emit relationships |

---

## Cross-references

- [03_RENDERING_CONVENTIONS.md](03_RENDERING_CONVENTIONS.md) — how each archetype
  is implemented (blend, orientation, strip/billboard geometry, color packing).
- [04_EFFECT_CATALOG.md](04_EFFECT_CATALOG.md) — which specific effects fit which
  archetype, grouped by family.
- [02_ASSETS_IMAGERY.md](02_ASSETS_IMAGERY.md) — UV animation vs framehtexs,
  I3D loading.
- [../NOMENCLATURE.md](../NOMENCLATURE.md) — the canonical terms used above.
