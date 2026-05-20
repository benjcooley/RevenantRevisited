# VFX Forensics Protocol

You are a **forensics agent**. Your sole job is to produce a complete,
self-contained document describing how one Revenant visual effect worked **in
the original (1998/99) engine**. You do **NOT** write engine code. The document
you produce is the deliverable — a different agent (the reconstruction agent,
see [RECONSTRUCTION_PROTOCOL.md](RECONSTRUCTION_PROTOCOL.md)) rebuilds the effect
from your document alone.

## Scope: document the ORIGINAL, not the rebuild

You are an archaeologist of the original code, not an engineer of the new one.
**Do not concern yourself with how the effect will be reimplemented in our
engine.** No pipeline mapping, no `EFxBlend`/`EParticleBlendMode` enum
translation, no test-rig or background recommendations, no reconstruction
roadmap, no "how this maps to our port." Those are the reconstruction agent's
decisions, made later from your doc. Your job is to capture — completely and
accurately — what the original effect was: its constants, assets, behavior,
custom logic, rendering, color, lights, audio, and triggers, in the original's
own terms.

Describe the original's rendering in the original's terms (D3D render states,
the actual blend it set, the transforms it applied). You may use the shared
descriptive vocabulary in [NOMENCLATURE.md](NOMENCLATURE.md) for orientation /
space / blend / animation concepts (these describe the original accurately) —
but do not classify by our engine's pipeline enum or recommend our engine's
machinery.

## The mission

**Clearly and concisely communicate the full implementation of the effect, so
the reconstruction agent can reproduce it precisely as it appeared and functioned
in the original game — 100% right.**

Three words carry the weight:
- **Full** — every constant, asset, transform, line of custom logic, the color,
  the light, the audio, the timing. Nothing the reconstruction needs is missing.
- **Clear** — unambiguous. Cited facts, pseudocode, diagrams. The reader never
  has to guess what you meant.
- **Concise** — complete in coverage, economical in expression. No padding, no
  restating the source narratively where a constants table or pseudocode block
  says it precisely. Every line earns its place.

If the reconstruction comes out wrong, the first question is "was it in the
forensics doc, stated clearly?" Your doc is the contract for fidelity.

## Why this exists

Single-agent ports kept shipping stand-ins and missing constants because the
same agent who "understood it well enough" rushed into code. Separating
forensics from reconstruction removes the coding escape hatch: if your document
is incomplete, that is a visible forensics failure, not something buried in a
port.

**Cite the retail code liberally — and expect the reconstruction agent to follow
your citations.** The document does not have to replace the source; it has to
distill it: say what matters, why it matters, and exactly where it lives
(`file:line`), so the reconstruction agent reads your doc first and drills into
the cited code for any detail. Citations are part of the spec, not just
provenance — a precise pointer to the right 8 lines of `effect_old.cpp` is often
clearer than paraphrasing them.

The test of a good forensics doc: **working from your doc plus the retail code it
cites, the reconstruction agent can rebuild the effect 100% — without having to
go discover anything you didn't point them at.** A doc gap is not "they opened
the source"; it's "the doc failed to mention a constant/asset/behavior, or
pointed at the wrong place."

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
8. **Clean slate — no prior-port knowledge.** Document the original purely from
   its own source. You are NOT told what any previous porting attempt did, you do
   not go looking for the current `src/effect.{h,cpp}` port to compare against,
   and you do not frame any finding against a prior implementation ("the old port
   got this wrong", "unlike the current version"). Forensics is objective
   archaeology of the original — describe what IS, with citations. State
   non-obvious behaviors plainly as facts (e.g. "spray is biased along the hit
   normal, not radial"); their value stands on its own without reference to any
   mistake. (Prior-port knowledge belongs to the reconstruction phase, where the
   reconstruction agent may be told what to fix.)

## Source-of-truth order

For any given effect (from [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.1):

1. **Retail decomp** under `recon/classes/` or `recon/discovered/` (Ghidra) —
   ground truth for *behavior*. Check the recon mapping. If sparse (ctor/dtor
   only), note it and fall back to pre-release.
2. **Pre-release source** under `src/effect_old.cpp` / `src/effectcomp.cpp` —
   authoritative for *intent*. Most effects' full bodies live here.
3. **Sister-family effects** — when an effect's own source is sparse, a sibling
   in the same family (fire-family, magic-family, combat-family) usually shares
   the render/blend/color pattern. Cite the sibling.

Document what the **code** says. The original developer is on this project and
their observations are valuable signals — but they can be wrong, and their memory
does NOT override the source. When the developer flags a reconstruction as wrong,
the default assumption is a **reconstruction bug** (the port didn't faithfully
translate the code), not a forensics misinterpretation — debug the port against
the code first. A developer hypothesis about a snapshot-only value is something to
**visually confirm against retail**, recorded as a hypothesis, not enshrined as
ground truth.

(Note: the current in-progress port under `src/effect.{h,cpp}` is **not** a
source — per hard-rule 8 you work clean-slate from the original, not against the
port. The class *declarations* there are fine to confirm a name/signature, but do
not study or describe what the port's bodies do.)

If retail and pre-release disagree, **retail wins for behavior**, but record the
divergence — it is information.

## The document template

Write your output to `docs/vfx/forensics/<EFFECT_ID>_<ClassName>.md` (e.g.
`docs/vfx/forensics/M09_TTeleporterEffect.md`). Fill out **every** section. If a
section genuinely doesn't apply, write "N/A — <why>", don't delete it.

The document has a fixed section order. Fill out **every** section, in order.
If a section genuinely doesn't apply, write `N/A — <why>` rather than deleting it
(a reconstruction agent must be able to trust that an empty section means "the
forensics agent checked and there's nothing," not "the forensics agent forgot").

````markdown
# <EFFECT_ID> <ClassName> — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | <e.g. M09> |
| **Class(es)** | <TXxxEffect + TXxxAnimator> |
| **Status** | forensics-complete \| forensics-partial (see §13) |
| **Retail fidelity** | retail-confirmed \| retail-partial \| snapshot-only (unverified) — see §2.1 |
| **Author / Date** | <agent-id> / <YYYY-MM-DD> |
| **Family** | fire \| ice \| magic \| lightning \| projectile \| blood/combat \| water/environment \| other |
| **Draws** | billboard(s) \| I3D mesh \| strip/ribbon \| particle emitter \| dynamic light \| volumetric \| composite |
| **Archetype(s)** | per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md) (UV-anim, I3D-geo, world-oriented, trail, beam, rising/swirling particles, simple emitter, composite, custom-logic, associated-light, audio) |

## 1. Summary
2–4 sentence prose overview: what the effect is, when/where it appears in the
game, and what it looks like + does. A reader should grasp the gestalt here
before diving into the detail sections.

## 2. Sources & evidence
- **Retail decomp:** `recon/classes/<file>` — COMPLETE \| SPARSE \| NONE
- **Pre-release (snapshot):** `src/effect_old.cpp:<start>-<end>` (`<ClassName>` + `<AnimatorName>`)
- **Sister effects consulted:** `<ClassName>` (`effect_old.cpp:<lines>`) — <what for>
- **Source-of-truth ranking:** which source is authoritative for this effect + why.

### 2.1 Retail-vs-snapshot reconciliation (REQUIRED — answer it, don't assume)
`src/effect_old.cpp` is a **pre-release development snapshot**, not the shipped
game. The retail build came later; effects are exactly the kind of thing that got
re-tuned or rewritten late. **You must actively determine whether the snapshot
matches what shipped** and rate the **Retail fidelity** field accordingly. Do not
default to "the snapshot is the truth."

Cross-check the snapshot against the retail Ghidra decomp — and don't stop at
"recon has no clean bodies." Actively try:

1. **Constant grep.** Take the snapshot's magic numbers (gravity, counts,
   durations, scales, angle offsets, RNG bounds) and search the retail decomp
   (`recon/classes/`, `recon/discovered/`, the wider Ghidra output) for those
   immediates in the effect's functions. Matching immediates = strong corroboration
   even without readable bodies; missing/different ones = a divergence signal.
2. **Asset identity.** Compare the snapshot asset (`legacy/Imagery/...`) to the
   shipped asset in the retail data (main checkout `data/...` / the `.rvi`). Same
   file (size/bytes) ⇒ the effect that consumes it is very likely unchanged;
   a different/renamed/missing asset ⇒ investigate.
3. **Structure/layout.** Compare class size, field count/offsets, and vftable
   method count between the snapshot class and the retail `cls_*` mapping. A
   matching layout corroborates; a different one means the effect was reworked.
4. **Registration + naming.** Confirm the retail binary registers the same
   builder/animator name (string XREF) and that the spell/caller wiring matches.

State the verdict explicitly with the evidence:
- **retail-confirmed** — retail decomp corroborates the snapshot's key constants +
  structure (cite the corroborating immediates/offsets/strings).
- **retail-partial** — some corroboration (e.g. asset identical + name matches) but
  key constants unverifiable; list what's confirmed vs. assumed-from-snapshot.
- **snapshot-only (unverified)** — retail decomp too sparse to corroborate anything
  beyond existence. The snapshot is the best available evidence but **may differ
  from shipped**; this is a fidelity risk the reconstruction must resolve by
  visually matching against retail ground-truth (the in-game capture in §12).

Per-constant, the §3 "confirmed?" column should reflect this: a value the retail
decomp corroborates is "yes (retail)"; a value seen only in the snapshot is
"snapshot-only" — not "yes." Don't launder snapshot-only values as confirmed.

## 3. Constants
Every numeric the effect uses, with citation. Mark confirmed vs guessed.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| NUMFIRES | 15 | count | effect_old.cpp:889 | yes (retail: immediate at cls_0x…) |
| gravity | 0.37 | wu/tick² | effect_old.cpp:10947 | snapshot-only (no retail corroboration) |
| … | … | … | … | guessed: <rationale> |

`confirmed?` values: **yes (retail)** = corroborated in the retail decomp (cite how);
**snapshot-only** = present only in the pre-release source, unverified against
retail; **guessed** = inferred, with rationale. Don't mark snapshot-only as "yes."

Include: counts, lifetimes/durations, speeds, gravities, drag, sizes, scales,
rotation rates, spawn rates/cadence, burst counts, blend factors, atlas dims,
sim-tick period, RNG ranges, and any magic numbers in transform/color math.

## 4. Assets
Every named asset (mesh, texture, sound). The stand-in guard — a missing asset
here becomes a procedural stand-in at reconstruction.

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| gvortex | legacy/Imagery/Magic/gvortex.I3D | 277KB | rotating cylinder mesh | RegisterImagery + GetTexture |
| … | … | … | … | … |

For mesh assets, enumerate the **sub-objects** and what each renders (pivot,
cylinder, flare, blast…). For textures, record frame count + cell dims + whether
animation is **UV-scroll** or **framehtexs flipbook** (see §8). If the effect
loads NO asset (pure procedural in the original), say so explicitly with the
citation proving it.

## 5. Spawn & emit
- **Trigger semantics:** one-shot \| fixed-duration \| continuous \| looping
- **Count per trigger:** <N particles/quads/segments>
- **Initial direction / distribution:** radial cone? caller vector? polar/swirl?
  char-attached? — cite the math
- **Emit anchor convention:** where the visual ORIGINATES — spawn call's exact
  (x,y,z), ground-projected, or character/owner-relative — cite
- **Coordinate space:** World \| Local (relative to anchor) — cite
- **Spread / jitter:** distribution + range — cite
- **Spatial diagram (required):** ASCII sketch — world axes, emit origin, spread,
  quad facing (conventions in [NOMENCLATURE.md](NOMENCLATURE.md) §9).

## 6. Behavior & per-frame logic
Transcribe the FULL Animate/Tick/Pulse logic as readable pseudocode — **every
piece of code the effect runs, not just its particles.** An effect's identity is
often in its custom logic. Cite `effect_old.cpp:<lines>` per block. Cover all
that apply:

- **Particle/emitter motion:** velocity integration, gravity/drag, velocity
  field (drift/rising/swirling — cite the math), color/alpha/scale curves over
  lifetime, spawn rate / burst, sub-emit (child effects — when + what).
- **Transform animation:** scale over time (pulse/grow/triangle-wave — cite
  peak), rotation over time (axis + rate), translation/path over time.
- **Procedural / bespoke algorithms — capture, don't summarize:** random
  target/position selection (RNG range, count, placement rule — e.g. meteor
  scatter), custom per-vertex / geometry animation (transcribe the math — e.g.
  ice-freeze deformation), internal conditionals/triggers (range checks,
  collision, timers, damage hooks).
- **Timing & structure:** sim-tick gating (24Hz? ungated in pre-release → note),
  state machine (phases, transitions, durations).

```
// Pseudocode — cite effect_old.cpp:<lines> per block
on each sim tick (24Hz):
    age += 1
    vel.z -= gravity
    pos += vel
    alpha = lerp(0.3, 0.0, age01)
    if pos.z <= 0: spawn_ripple(pos); kill()
```

For composites (fire ring, meteor storm), **decompose**: document each component
+ its archetype + how the parent orchestrates timing/placement. Don't flatten a
multi-component effect into one description.

- **Temporal diagram (required for multi-stage):** ASCII envelope (value vs
  age01) and/or phase timeline (per [NOMENCLATURE.md](NOMENCLATURE.md) §9).

## 7. Rendering (original render state + geometry)
- **What it draws:** billboard(s) / I3D mesh / strip / particles / light /
  volumetric / composite
- **Blend mode (original):** identify which helper the effect calls —
  `SetBlendState()` (texture MODULATE + SRC_ALPHA/INV_SRC_ALPHA = Alpha) vs
  `SetAddBlendState()` (ONE/ONE = AdditiveStraight); see
  [NOMENCLATURE.md](NOMENCLATURE.md) §3. Cite the call site + the real factors.
  (F03 was wrongly guessed additive — it was straight Alpha. Trace the helper,
  don't guess from how it looks or from a D3D-constant name in a comment.)
- **Lit vs self-lit:** scene-lit or self-lit/glowing? — cite
- **Depth / Z:** ZWRITEENABLE / ZENABLE in the Render body; what RefreshZBuffer
  does — cite
- **Orientation:** ScreenAligned \| WorldXY (`rot.x = -π/2` tell) — cite the transform
- **Per-quad / per-object transform:** rotation (x/y/z), scale, offset — cite
- **Per-vertex color packing:** how diffuse/alpha get written (PackARGB etc.) — cite

## 8. Texture animation
- **Mechanism:** UV animation (scrolling/animated texture coords over time) vs
  frame flipbook (`framehtexs[]` per-frame handle swap) — record which (or both)
- **Rate / wrap / per-instance phase offset** — cite
- If none (static texture), say so.

## 9. Associated light
MANY effects emit dynamic light (fire, explosions, magic glows light the scene).
- **Color / radius (range) / intensity** — cite values
- **Animation:** flicker / pulse / fade with the effect — cite the math
- **Attachment:** at the origin, or following a moving part (projectile, flame)?
- If the effect emits NO light, say so explicitly.

## 10. Color
- **Source:** chardata field \| spell color \| texture palette \| hardcoded — cite
- **Exact values:** RGB(A), note 0-255 or 0-1 — cite
- **Expected visual:** what color it SHOULD read as (warm fire? ice-blue? Locke's
  magenta?). Pale/gray at reconstruction = bug (see [[feedback-vfx-color-health-signal]]).
- **Normalization / boosts:** NormalizeColors / hilt-brightness / tint math — cite

## 11. Audio coupling
Effects may trigger sounds (`PLAY("drip")` on impact). Audio port is out of VFX
scope, but record it so nothing's lost.
- **Sound name(s) + when** (spawn / impact / looping-while-alive) — cite call site
- If none, say "no audio coupling found".

## 12. Triggers & in-game appearance
- **Spawned by:** spell.def variant(s) \| monster attack \| scripted scene \|
  environmental/ambient \| vestigial (no live caller) — cite the registration
  (DEFINE_BUILDER / REGISTER_3DANIMATOR / spell.def line / ATTACHEFFECT)
- **Where to see it in the original game:** specific location/trigger so the
  reconstruction agent can capture ground-truth reference (e.g. "cast Teleport in
  any safe spot", "BLOODMIRE NW ambient mist", "every melee hit on Locke")
- **Vestigial?** If no live caller, say so — ground truth then comes from asset
  inspection + sister-family effects.

## 13. Gaps & uncertainties
- What you couldn't determine from source, what you tried, the most reasonable
  guess + rationale — never a silent fabrication.
- **Snapshot-vs-retail risk:** if Retail fidelity (§2.1) is snapshot-only or
  retail-partial, call out the specific values/behaviors that are unverified
  against shipped retail, so the reconstruction agent knows exactly what to
  scrutinize when visually matching against ground-truth reference.
- Anywhere the original source is ambiguous and the reconstruction agent will
  need to visually match against ground-truth reference to resolve.

## 14. Reconstruction burndown
A flat checklist of every discrete element the reconstruction agent must recreate
to reproduce this effect faithfully. This is the distillation of §1–§13 into
actionable items + the definition of done. List **what** must be recreated (the
original's components/behaviors), not **how** to build it in our engine — the
reconstruction agent owns the how. Each item back-references the section that
specs it, so the builder can jump to the detail. Order roughly by build
dependency (assets first, then geometry, then behavior, then polish).

```
- [ ] <load asset(s) — name them> (§4)
- [ ] <emit/spawn: count, shape, anchor, space> (§5)
- [ ] <core geometry: billboard / mesh / strip / particles> (§7)
- [ ] <blend + lit + depth + orientation> (§7)
- [ ] <texture animation: UV-scroll or framehtexs flipbook, rate> (§8)
- [ ] <transform animation: rotation/scale/translation curves> (§6)
- [ ] <particle motion: velocity field + gravity + curves> (§6)
- [ ] <custom/procedural logic: e.g. random placement, vertex animation> (§6)
- [ ] <state machine / phases, if any> (§6)
- [ ] <associated dynamic light: color/range/intensity/animation, or "none"> (§9)
- [ ] <color wired from the right source> (§10)
- [ ] <sub-effects spawned, if any> (§6)
- [ ] <audio hooks recorded for the audio phase, or "none"> (§11)
```

Keep it concrete and complete: a reconstruction agent should be able to work
top-to-bottom, check each box, and end with a faithful recreation. Items the
effect doesn't have (no light, no audio, no sub-effects) are stated as "none" so
the builder knows they were considered, not forgotten.
````

## Process

1. Read [NOMENCLATURE.md](NOMENCLATURE.md) (shared vocabulary), the knowledge
   base under [knowledge/](knowledge/) (how the original VFX systems worked), and
   [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §3.0–§3.1 (forensics + source order),
   §4.2.1.5 (color = health signal), §4.2.1.6 (orientation tells).
2. Read the INVENTORY row for your effect — it may already have partial
   forensics. Build on it, don't redo.
3. Locate all sources (§2). Rank them. **Do the §2.1 retail-vs-snapshot
   reconciliation** — actively cross-check the snapshot against the retail decomp
   (constant grep, asset identity, structure/vftable, registration) and set the
   Retail fidelity rating with evidence. Don't assume the snapshot is retail.
4. Fill the template top to bottom, tracing every helper, citing every fact,
   capturing ALL custom logic (not just particles), the associated light, and
   any audio. Mark each §3 constant as retail-confirmed vs snapshot-only. Finish
   with the §14 burndown — distill everything above into a flat checklist of
   discrete things to recreate; if you can't write a clean burndown item for some
   behavior, you haven't documented it clearly enough above.
5. Self-test: "Working from my doc plus the retail code it cites, could the
   reconstruction agent reproduce this 100% — without having to discover anything
   I didn't point them at?" If not, keep going.
6. Update the INVENTORY row: `Status` and a one-line "forensics doc at
   docs/vfx/forensics/<file>".
7. Commit the doc (+ INVENTORY edit) on `feature/vfx`. Don't write engine code,
   don't make reconstruction/engine decisions.

## What good looks like

`docs/vfx/M09_FORENSICS.md` (the teleporter) and the H04 TDripEffect INVENTORY
entry are the gold-standard examples of the depth expected — constants tables,
helper-traced blend modes, asset enumeration, pseudocode, gaps flagged.
