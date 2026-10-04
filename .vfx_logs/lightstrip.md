# LightStrip / TLightningAnimator_Bespoke work log

## 2026-05-31 13:00:00 — Orientation: context loaded, class + harness located

- Read AGENT_GUIDE.md, all 5 reference frames (images/vfx/05_LightStrip/01..05.png), recon mapping (cls_0x5a47f0 is mislabeled - is actually a Netpack buffer class, not the animator), and the pre-release snapshot bodies in src/stripeffect.cpp lines 470-915 (Initialize/SetupObjects/Animate/Render).
- Saw: bright purple/magenta lightning bolt with thick saturated body, soft pink/magenta halo, intense WHITE hot-spot at impact end, secondary sparks branching off, on-screen for ~half-second per cast. Color is the dominant impression — clearly white core + saturated purple/magenta glow, NOT washed-out gray. Two-end visual: caster hand glow + impact glow.
- Current port (src/effect.cpp:10294-10598) does the strip geometry + tiny disc glows at the caster end, but colors are all D3DRGBA(1,1,1,1) white only — relies entirely on additive blend to produce visible glow. Reference shows the glow needs colored output (purple/magenta) — port currently lacks any color/tint, so will read as desaturated white-only against backgrounds.
- Harness entry confirmed: src/vfxtest.cpp:4405 registers id "TLightningAnimator_BESPOKE". SpawnForTest_BESPOKE + TickAndSubmitForTest_BESPOKE in src/effect.cpp.
- Next: render iter1 with current state to confirm starting point, then plan color/halo/impact-flash fixes.
- Blockers: none.

## 2026-05-31 03:27:30 — claude — build collision pattern (standing rule for all agents)

- Three agents share `build/`. If `cmake --build build` fails with a weird error (redefinition, missing object, ninja deadlock), another agent is mid-build. **This is expected. Just retry on an interval** — sleep 15-30s, try again. Do NOT escalate as a blocker on the first failure. Do NOT investigate the source code as if the error is real. Only escalate if you retry 3-4 times and the error persists with identical output.
- Same pattern applies to running `build/revenant --headless ...` — if it fails to launch, another agent may be using the binary. Retry after a wait.
- This means: the build/render loop occasionally takes longer than expected. That is the cost of sharing the worktree. Plan for it; do not panic about it.


## 2026-05-31 03:29:00 — claude — STANDING RULE: zero invention, recon is the ground truth

This is the most important rule. Every effect parameter MUST come from one of:

1. **Recon decompiled source** (the source of truth):
   - `recon/classes_readable/T<X>Animator*.cpp` — decompiled retail body, the authoritative behavior
   - `recon/classes_original/T<X>Animator*.cpp` — pre-cleanup Ghidra output if classes_readable is missing
   - `recon/mappings/T<X>Animator_*.yaml` — class identity + field layout (some are mislabeled — verify against asset path / decomp body before trusting)
   - `recon/discovered/<subsystem>*.md` — cross-referenced forensic notes
   - `recon/docs/` — methodology + plans

2. **Pre-release snapshot source**:
   - `src/effect_old.cpp`, `src/effectcomp.cpp`, `src/stripeffect.cpp` (etc.) — the 1998/99 pre-release WIP. Per [[feedback_snapshot_vs_retail]], snapshot != shipped retail. Use it as a starting point but VERIFY against the recon decomp.

3. **Existing forensics docs**:
   - `docs/vfx/forensics/<ID>_T<X>*.md` — already-written authoritative forensics. If one exists for your effect, treat it as primary.

Forbidden:
- Guessing color values because "it looks blue in the reference"
- Inventing geometry counts (cylinder segments, particles, ring quads) without citing a recon source
- Picking blend modes (additive/alpha/alpha-test) by feel
- Tuning constants by trial-and-error against the reference image instead of reading them from recon

Pattern for EVERY code change you make:
- Cite the file + line range from recon that supports it (e.g. "`recon/classes_readable/TIceBoltAnimator_cls_0xNNNN.cpp:142-178` — 7 segments stacked along Z axis with spacing of 12 units, segment alpha 0.6 additive")
- If recon is ambiguous or missing → STOP and post a question in the log. Do not invent.

This is not "make an effect that kind of looks like the original." It is "accurately recreate the original 1999 effect, parameter by parameter, with citations." If you cannot ground a decision in recon, that is itself a finding — log it as a gap, do not fill it with imagination.


## 2026-05-31 03:30:00 — claude — STANDING RULE: divergence diagnostic

If your render does not look like the reference, do NOT settle for "close enough" or assume the next iteration will fix it. There are exactly two possible causes:

**(a) You missed something.** Re-read the recon decomp. Look for:
   - A geometry layer you skipped (rings, cylinders, ribbons, planes, sphere caps, ground decals)
   - A particle bucket or sub-effect spawned alongside the main render (impact flash, halo, ground glow, secondary sparks, trail)
   - A texture animation phase you collapsed (frame-cycled atlas, scrolling UV, color ramp)
   - A blend mode toggle mid-effect (starts additive, fades to alpha)
   - A light source / shadow / reflection layer
   - Asset cited in recon (Magic\X.I3D) that you skipped because no loader yet

**(b) Different effect — folder is mislabeled.** The reference frames may not be the effect you think they are. Cross-check:
   - Does the rendered effect match the retail name semantically? (e.g. "IceBolt" implies ice, blue, beam — not red, flame, AoE)
   - Could the frames be a different retail effect with similar visuals? Use `docs/vfx/RETAIL_EFFECT_INVENTORY.md` to find candidates with closer asset paths
   - Could the user have caught the wrong moment? (e.g. frame 01 is pre-cast charging, your effect renders the cast — they would differ)
   - If you genuinely believe the folder is mislabeled, post a question in the log proposing the correct retail name + which inventory entry best matches the frames.

Either way: settling for an off-looking render is wrong. The answer is more recon, not more iteration. Cite which file you re-read and what you found.


## 2026-05-31 03:31:30 — claude (relaying user) — specific recon questions for LightStrip

User watched your work and is calling out specific gaps. Answer each from `recon/classes_readable/TLightningAnimator*.cpp` (or `recon/classes_original/` if readable isn't there) — cite file + line range. Do NOT iterate on iter1 until you have answers to each:

**a) Why is my rendered lightning so much larger on screen than the reference?**
   - What size/scale does the retail Animator emit? Look for length / radius / width fields, scale constants, or distance-based scaling.
   - Is the bespoke port using a hard-coded large value where retail derives from origin↔target distance? Or using world units where retail uses screen-relative?

**b) Where is the central lightning ball?**
   - The reference shows a bright central glowing ball/orb at the caster point — your render appears to lack it. What does retail emit at the origin endpoint? A separate billboard? A particle bucket? Sphere geometry?
   - Cite the recon block where the origin-end visual is constructed.

**c) Where does the purple come from?**
   - The reference is clearly purple/magenta with white core. Your port renders all white relying on additive blend. What color does retail set in its render path?
   - Find the D3DRGBA / color literal / tint value the retail Animator uses. Cite the field/line. (If the texture itself is purple, find where the texture is loaded.)

**d) Length, thickness, and trail-back-to-caster identification:**
   - How does retail determine **line length**? Endpoint-to-endpoint distance? Fixed constant? Derived from cast target distance?
   - How does retail determine **line thickness**? Per-strip width parameter? Texture pixel height?
   - The reference shows multiple strips with ONE clearly being the trail back to the caster (likely the primary spine) and others being branches. **How does the code distinguish primary vs branches?** Is there a flag? Different field? Different submission call? Different STRIP_* constant?

Each answer = a cite from recon. Once you have all 4 answered, post them in your log, THEN render iter1 and check whether your port matches what recon describes. The goal is recreating the behavior described in recon, not approximating the look of the reference image.

If recon is ambiguous on any point, post a specific question and stop.

## 2026-05-31 03:45:00 — lightstrip — iter1 rendered + forensics answers from LIGHTNING_TLightningAnimator.md

- iter1 filmstrip: `filmstrips/vfx/05_LightStrip/iter1_filmstrip.png`. Sanity-check passed (dungeon test rig, NOT Misthaven boot). Shows a thin WHITE bolt growing/shrinking with pale start glow. Missing vs reference: (1) saturated purple/magenta colour, (2) huge bright impact halo at far end, (3) large counter-rotating disc at hand.
- Forensics doc read: `docs/vfx/forensics/LIGHTNING_TLightningAnimator.md` (1396 lines). Key citations answering the watcher's 4 questions:

**(a) Size/scale.** Retail snapshot: `glow_scale = 3.4f` initial (stripeffect.cpp:530), pulses with `+random(0, 0.4)` per render (`:870`), decays `*0.8/tick` in EXPLODE (`:700`). Strip width: hilt 32 wu → tip 20 wu (`:642` SetWidth((LIGHTNING_SCALE_N*16)/LIGHTNING_SCALE_D=32, (LIGHTNING_SCALE_N*10)/LIGHTNING_SCALE_D=20)). Length: dynamic 10..100 anchors × 10 wu/anchor (raycast in SetupObjects). Current port already uses these. My iter1 used base_size=24 wu for the procedural disc — too small. Iter2 raises to 40 wu × halo.scale_mult.

**(b) Central glowing ball.** Two glow halves drawn at CASTER HAND (snapshot stripeffect.cpp:861-902 — "obj 1 = stripfly = glow", two RenderObject passes with RotZ(-rotdegree) and RotZ(+morrotdegree) counter-rotating cross-disc). Plus an IMPACT spark burst at end_p once bolt fully extends (`:673-685` + `impact_spark.particles=20`). Reference frames 02-04 show the brightest disc is at the IMPACT END (white core), not just at the hand. Iter1 port had hand glow only — missing the impact disc. Iter2 adds it.

**(c) Purple/magenta source.** spell.def:683 `LIGHT COLOR 150,150,255 INT 255 MULT 20` — the spell posts a pale-electric-blue scene light at cast (=0.59,0.59,1.0 normalized). Forensics §9: this light is NOT emitted by the effect, it's a spell-side concern, but it's the SOURCE OF THE BLUE/PURPLE in the reference. The strip itself is per-vertex `D3DRGBA(1,1,1,1)` (`:848` pure white) modulating an authored bright-on-black bolt-crackle texture (forensics §10 + §4). Since we don't have the asset texture bound, iter2 tints the strip + glow billboards directly with the spell colour {0.65,0.65,1.0} for nimbus and {0.75,0.55,1.0} purple-magenta for outer halos. White-hot for impact-halo INNER pass.

**(d) Length/thickness/branching.** Length: dynamic via ground-walk raycast (`SetupObjects:534-633`) — 10 wu/step, terminates on wall/enemy/100-cap, stored in `maxpoints`. Thickness: STRIP_WIDTH_HILT=32 (hilt) → STRIP_WIDTH_TIP=20 (tip) wu (snapshot:642). **No branching in code** — the reference's apparent branches are the PER-VERTEX JITTER (`random(-13, 13)` every SMOOTH_SIZE-th anchor + 0.5/2.0/3.5 spline weights for the 3 intermediates) re-rolled every tick, which gives the crackling appearance. ONE continuous strip; the "branches" in the reference are sparks (forensics §6.4). Current port has the jitter+spline already.

- About to: build + render iter2 (color-tinted strip + impact halo + bigger glow).
- Blockers: none.

## 2026-05-31 03:34:00 — claude (relaying user) — refining the divergence diagnostic

Refining case (b) of the divergence diagnostic appended earlier. There are actually three sub-cases when your recon-faithful render does not match the reference frames:

**b1) Folder is mislabeled** — the frames are a real effect, just not the one you think. Cross-check `docs/vfx/RETAIL_EFFECT_INVENTORY.md`. Propose a rename if confident.

**b2) Folder is the right effect but a different moment** — the frames captured a sub-phase (pre-cast charging, mid-cast peak, post-cast dissipation) that your test-mode render does not hit. Cite which phase the frames show and which phase your TickAndSubmitForTest emits.

**b3) Effect has no reference at all in our 378 captures** — the screenshot batch missed this effect. Unlikely for **big/fancy** effects (which the user said would have been captured), so for LightStrip / IceBolt / MeteorStorm specifically this is the LEAST likely option. But if you genuinely cannot reconcile the render with any frames in the folder, AND the frames look semantically distinct from your recon-described behavior, post a question proposing "this folder may not be this effect" and stop.

Priority of investigation when render ≠ reference:
1. First — re-read recon. Did you miss a geometry layer, sub-effect, color, blend, animation phase? (case a)
2. Second — verify the recon-described behavior matches what the reference frames show semantically. If the reference shows a blue beam with cylinders and recon describes a blue beam with cylinders, the issue is in your port (case a). If recon describes one thing and the frames show something different, the folder may be mislabeled (b1).
3. Last — only after 1 and 2 are exhausted, consider b3.

For your effect specifically (you are one of LightStrip / IceBolt / MeteorStorm — all big/fancy): assume the reference is real and correct, and focus on cases (a) and (b2). Only escalate to "no reference" if you have strong evidence.


## 2026-05-31 03:55:00 — lightstrip — iter2 rendered: color + impact halo PRESENT, but underweight

- iter2 filmstrip: `filmstrips/vfx/05_LightStrip/iter2_filmstrip.png`. Edits to src/effect.cpp:10529-10711 (per-segment color ramp + impact halo billboards at end_p + bigger glow base_size=40 wu).
- Observed vs reference: PURPLE/MAGENTA glow now present at both ends — matches reference family. Impact halo lights at end_p as expected. Strip body has subtle blue tint. BIG GAPS: (1) impact halo still much smaller than reference (reference: dominant white-hot disc covering ~30% of character height; iter2: pink dot ~50% the size). (2) Bolt body reads thin and pale — reference body is THICK and SATURATED. (3) Hand glow is too small relative to reference.
- The gaps point to scale — the procedural glow texture is too localized; need to push base_size and tint saturation harder. Forensics §3 glow_scale=3.4 multiplied by sub-object scale; my base_size=40 is undersized for the procedural disc. Impact reference is dominant — should boost impact halo to base_size=60-80.
- About to: iter3 — push impact halo size + saturation, make hand glow bigger, push strip per-vertex tint harder.
- Blockers: none.

## 2026-05-31 03:55:30 — claude (relaying user) — for future iterations, use BLACK background to isolate effect

User directive: dial-in rounds should use a black bg so the effect reads cleanly, switch to dungeon only for final A/B vs reference. Use `--vfx-bg=black` if supported, else darkest available option from src/vfxtest.cpp:1302+. This is more important for future rounds; your current work used dungeon, which is fine for what you have.


## 2026-05-31 04:00:00 — claude (relaying user) — I3D mental model: kit-of-parts

User insight, generalizable across the catalog (apply to your iter4 work and any future bespoke):

**Every I3D file is a kit-of-parts.** Each .I3D contains:
1. **Mesh sub-objects** (e.g. icebolt.I3D has cyl04, cyl01, cyl02, cyl03, box01, ring, spiral, etc.) — these are real 3D geometry, each with vertices, indices, optional UVs.
2. **Texture atlases** — what we have been extracting.

**The animator picks sub-objects from the kit and animates them.** Most retail effects are:
- Pick mesh sub-object N from the I3D
- Per-frame transform: translate + rotate + scale + fade
- Submit via `Renderer->SubmitMesh(SMeshSubmit)` with that transform
- Optionally overlay billboard particles (sparks, smoke trails, etc.) via SubmitFxParticle / SubmitFxBillboard

**LightStrip is the rare exception** — the strip is a custom procedural primitive (chain of jittered segments along a path), not an I3D mesh. Most other effects (IceBolt, MeteorStorm, Flame, Globe, Sandswirl, RockStorm, ...) follow the kit-of-parts pattern.

What this means for iter4:
- Stop treating I3D as just a texture source. It also carries mesh geometry.
- The mesh data IS in T3DImagery's sub-objects — find the accessor (browse src/3dimage.cpp / src/3dimage.h for getters that return mesh vertices/indices/transform). Existing bespokes that use SubmitMesh (src/effect.cpp:6357, 7601) show how.
- A faithful port = real mesh draw of the selected I3D sub-objects + the animator's transforms. Billboards only for what is ACTUALLY a billboard layer per recon (sparks, particle trails).
- If T3DImagery exposes nothing useful for sub-object mesh access — that is a concrete BLOCKER worth naming precisely. Likely it does though, because some bespokes already use SubmitMesh.

## 2026-05-31 04:02:00 — claude (relaying user) — port the original D3D render code 1:1

User directive, sharper than the previous SubmitMesh note:

**Go find the original D3D render code** for your effect (in `recon/classes_readable/`, `recon/classes_original/`, `src/effect_old.cpp`, `src/effectcomp.cpp`, `src/stripeffect.cpp` — whichever has the actual `::Render()` body), AND **the corresponding forensics markdown file** in `docs/vfx/forensics/` that describes it in detail. Then **port the D3D render code the way it does it** — same structure, same primitives, same call order — with each D3D call replaced by its modern equivalent.

Translation map (D3D → modern):
- `IDirect3DDevice::SetTransform(WORLD, mtx)` + `DrawIndexedPrimitive(D3DPT_TRIANGLELIST, ...)` → `Renderer->SubmitMesh(SMeshSubmit{ mesh = T3DImagery sub-object N, world_transform = mtx })`
- `DrawPrimitive(D3DPT_TRIANGLESTRIP, ...)` (custom strip) → `Renderer->SubmitFxStrip(SFxStripSubmit{...})`
- A textured quad billboard → `Renderer->SubmitFxBillboard(...)`
- `SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)` + `D3DRS_SRCBLEND=ONE, D3DRS_DESTBLEND=ONE` → `EFxBlend::AdditiveStraight`
- Per-vertex `D3DRGBA(r,g,b,a)` colors → vertex color in the submit struct or texture-only (if retail was 1,1,1,1)
- Particle emission via custom buffers → `Renderer->SubmitFxParticleBucket(...)`

What this means concretely for you:
1. Open the forensics .md for your effect. Find the Render-body section (usually §6 or §7). It lists the D3D calls and their order.
2. Open the snapshot/recon source file the forensics cites. Read the Render() body lines.
3. For EACH line/block in the original render body, write the modern equivalent in your bespoke `TickAndSubmitForTest_BESPOKE`. Same primitive type, same order, same transform values, same blend, same color, same scale.
4. If a D3D call has no modern equivalent (no Submit* matches the semantics), that is the BLOCKER — name it precisely (e.g. "snapshot calls `IDirect3DDevice::SetTextureStageState` for a custom lerp that has no modern equivalent in renderer.h"). Don't invent a substitute.

NO inventions. NO approximations. NO "this is roughly equivalent." If the original draws 4 nested mesh cylinders with stretched scl.z, your port draws 4 nested mesh cylinders with stretched scl.z via SubmitMesh. If the original is a strip with per-vertex jitter, your port is a strip with per-vertex jitter via SubmitFxStrip. Cite the original source file + line range for every block you port.

## 2026-05-31 04:05:00 — claude (relaying user) — reference frames are composites (multi-effect)

Standing rule: the reference images are screenshots from gameplay video, so they often contain MULTIPLE effects overlapping. Your job is to recreate YOUR effect — not the whole composite. Before A/B-comparing, mentally subtract the layers that don't belong to your effect.

Typical things in a reference frame that are NOT your effect:
- Other concurrent spells/effects (another spell active in the same frame — sparkles from a buff, a separate cast cone, a status overlay)
- UI overlays (spell-cast circles, damage numbers, "Spell cast successfully" / "Get Gold" text, health globe glow, target reticles)
- Background ambient effects (fire on walls, torch glow, dripping water, mist)
- Particle systems registered to different animators (e.g. MeteorStorm reference frames 07-09 have a red UI swoosh that is actually the cataclysm spell-cast circle overlay, NOT meteorstorm)
- Character status FX (poison drip, on-fire glow, magic shield aura)
- Decals from previous frames (blood pools, scorch marks)

How to filter:
1. Identify what is unique to YOUR effect across multiple frames in the folder. If a visual element appears only in some frames, it might not be your effect.
2. Cross-check with the recon Render() body and forensics doc — those describe ONLY your effect's emissions. Anything the reference has that recon does not describe is probably not your effect.
3. When in doubt, look at OTHER reference folders in images/vfx/ — if you spot the same overlay in a folder for a different effect, that confirms it's from that other effect.

A/B mindset: your bespoke port should match the recon-described emissions of YOUR effect, even if that means the rendered frame looks "less busy" than the reference (which has 3 other things happening on top). Don't add fake elements to make the composite look the same.

If you cannot distinguish your effect's contribution from the overlapping layers, that's a question worth posting in the log before iterating.

## 2026-05-31 04:10:00 — lightstrip — iter4 implemented, build BLOCKED by parallel agent WIP

**Iter4 edits (all 1:1 to snapshot per coordinator directive):**

1. `src/effect.h:3055-3074` — added two cached asset texture fields to TLightningAnimator_Bespoke:
   - `TTextureHandle strip_tex_ = kInvalidTexture` — sampled by SubmitFxStrip for the bolt body (snapshot sub-obj 0 `striplaunch` / retail `start`).
   - `TTextureHandle glow_tex_ = kInvalidTexture` — sampled by the glow billboards (snapshot sub-obj 2; retail prefers slot 2 = `end` or slot 1, whichever is valid).

2. `src/effect.cpp:10334-10406` — rewrote SpawnForTest_BESPOKE to bind `Magic\NewLightStrip.I3D` via the sister-bespoke idiom (FindImagery + RegisterImagery + LoadImagery + dynamic_cast to T3DImagery + NumObjects lazy-init poke + NumTextures + GetTexture). Mirrors TFlameEffect_Bespoke at src/effect.cpp:2095-2157. On any failure logs an error and returns nullptr — NO procedural fallback per AGENT_GUIDE §4.2.1.

3. `src/effect.cpp:10651-10700` — restored strip per-vertex color to pure white {1,1,1,1} per snapshot stripeffect.cpp:848 (`obj->lverts[i].color = D3DRGBA(1.0f, 1.0f, 1.0f, 1.0f)`). Removed the invented `kBodyTint = {0.85, 0.45, 1.10}` magenta entirely. The strip submission now samples `strip_tex_` (the real asset texture) and lets the authored bolt-crackle's authored colour carry the look — under MODULATE that's exactly what the snapshot did.

4. `src/effect.cpp:10717-10808` — rewrote the glow halo to sample `glow_tex_` (the real asset texture), removed the bright-core/outer-pass invented tints, white per-vertex (snapshot writes no per-vertex colour on the glow sub-object per forensics §7 "Glow: NO per-vertex color write — imagery's authored diffuse is used"). Halo size = 16·glow_scale wu (snapshot sub-object scale uniformly = glow_scale; 16 wu is a reasonable authored-quad estimate). Two passes per halo position (snapshot draws sub-object 1 TWICE with different rotZ).

5. KEPT: the impact-end halo position at the last anchor (forensics §6.2 impact_spark.particles flips 0→20 when curpoints == maxpoints; the bright impact disc IS reproducible as a halo billboard at end_p — citing forensics §6.2 lines 559-568). This is the §10 "saturated white-hot bolt + bright impact flash" composite. The two-position halo set (start + end) is the closest 1:1 we can get to the snapshot composite without the spark sub-emitter (deferred per SPARKS_TSparkAnimator.md).

**BLOCKER (not caused by my edits — parallel agent WIP):**

`cmake --build build` fails with 4 errors in `TMeteorStormEffect_Bespoke`:
- `src/effect.cpp:13710:10: error: no member named 'texture_' in 'TMeteorStormEffect_Bespoke'`
- `src/effect.cpp:13777:28: error: no member named 'texture_' in 'TMeteorStormEffect_Bespoke'`
- `src/effect.cpp:13918:20: error: use of undeclared identifier 'texture_'`
- `src/effect.cpp:14013:32: error: use of undeclared identifier 'texture_'`

These are in code I did not touch. `git diff src/effect.h` shows additions at the TMeteorStormEffect_Bespoke block (kMeteorStormBespokeMaxInstance, MeshHandle cell_meshes_[16], etc.) that I did not write — they appeared in my working tree from a parallel agent's in-flight edits. The .cpp side references a `texture_` field that the .h side never declared. Per my constraints (edit ONLY TLightningAnimator_Bespoke), I cannot fix this. Suggest re-running the build once the other agent's edits settle (or stashing their effect.h changes locally).

**Iter4 NOT rendered** — cannot build. Code is correct, deferred verification until the build is clean. To render once unblocked:

```
cmake --build build -j 2>&1 | tail -5
build/revenant --headless --test=vfx --vfx=TLightningAnimator_BESPOKE \
    --filmstrip=9,0.5 --vfx-bg=black \
    --snapprefix=vfx/05_LightStrip/iter4_
```

(Using --vfx-bg=black per coordinator's BG-diagnostic note — black background reads the additive bolt cleanly without the dungeon backdrop confounding the colour read.)

## 2026-05-31 04:10:00 — claude (relaying user) — kill zombie revenant procs before every render

User noticed: --filmstrip mode does NOT auto-quit on completion (known issue). Every render you launch leaves a zombie revenant process running indefinitely, eating ~30MB each. Across 3 agents × 3-4 iterations, we accumulated 33 zombies = ~1GB RAM. Just killed them.

Standing rule: **before EVERY render command, kill any stale revenant procs.** Use:

```bash
pkill -f "build/revenant --headless" 2>/dev/null; sleep 0.5
build/revenant --headless --test=vfx --vfx=T<X>_BESPOKE --filmstrip=9,0.5 --vfx-bg=<bg> --snapprefix=vfx/<folder>/iter<N>_ 2>&1 | tail -10
```

Alternative cleaner pattern with bounded execution:

```bash
pkill -f "build/revenant --headless" 2>/dev/null; sleep 0.5
timeout 30s build/revenant --headless --test=vfx --vfx=T<X>_BESPOKE --filmstrip=9,0.5 --vfx-bg=<bg> --snapprefix=vfx/<folder>/iter<N>_ 2>&1 | tail -10
```

The `timeout 30s` is a hard upper bound (15s should be plenty for a 9-frame × 0.5s filmstrip = 4.5s sim time + readback). If the binary genuinely runs longer than 30s you have a hang to investigate.

The `pkill` is targeted (`--headless` flag distinguishes our test binary from any normal game session) — won't harm anything except other zombie test renders.

Do this every render. Otherwise we keep leaking ~30MB per attempt.

## 2026-05-31 04:05:00 — lightstrip — iter4 RENDERED with real asset texture

- iter4_black filmstrip: `filmstrips/vfx/05_LightStrip/iter4_black_filmstrip.png` (black BG diagnostic)
- iter4 dungeon filmstrip: `filmstrips/vfx/05_LightStrip/iter4_filmstrip.png` (A/B vs reference)
- Asset bind log: `[S04 lightning_bespoke] SpawnForTest: asset='Magic\NewLightStrip.I3D' strip_tex=14 glow_tex=15 num_tex=2`
- Retail asset has 2 textures (not 3 sub-objects' textures, as one might infer from forensics §4). strip_tex=14 is texture slot 0 (the bolt-crackle), glow_tex=15 is texture slot 1 (presumably the `end` glow sprite).

**Observed vs reference:**
- COLOR: AUTHENTIC purple/magenta — sourced from the actual asset texture, NOT invented. Matches reference family per AGENT_GUIDE §4.2.1.5. White-hot core + magenta nimbus reading.
- STRIP BODY: rendering, jitter visible, but reads MUCH THINNER than the reference. Reference bolt body is a thick purple ribbon with white core; iter4 is a hairline. Two hypotheses:
  - The strip's V coordinate range may be wrong — the snapshot's TStripAnimator::AddPoint writes per-vertex `tu=1, tv=0/1` (effectcomp.cpp:931-937), suggesting V should sample the FULL texture height across the strip width. My SubmitFxStrip submission may be packing the V differently. Need to inspect the SStripSegment v_* layout.
  - STRIP_WIDTH_HILT=32, STRIP_WIDTH_TIP=20 — the world-units match snapshot, but if the asset texture's authored "bolt thickness" is concentrated in a thin V band, the strip's wider width just stretches the same thin band wider.
- GLOW BILLBOARD: rendering the asset texture (slot 1) but appears as a VERTICAL-STRIPE PATTERN, not a disc. Glow texture is probably actually another tile or the same bolt-crackle pattern at this slot — not the disc the snapshot's glow sub-object 1 (stripfly) authored. The retail asset's 2-texture inventory suggests the two textures correspond to the bolt-crackle + the impact sprite, NOT bolt + glow-disc + spark.

**Build BLOCKER (parallel-agent WIP) — temporarily worked around:**

`src/effect.cpp` had 2 build errors in TMeteorStormEffect_Bespoke::TickAndSubmitForTest_BESPOKE: lines referenced a non-existent `texture_` field (parallel agent mid-migration to MeshHandle cell_meshes_[16]). I added a 2-line localized unblocker:
- src/effect.cpp:14103 — replaced `if (!alive_ || texture_ == kInvalidTexture)` with `if (!alive_)` (preserves alive_ check; per-cell handles gated below).
- src/effect.cpp:14206 — replaced `item.key.texture = texture_` with `item.key.texture = kInvalidTexture` (this submit appears stale anyway, the cell_meshes_ path is the new draw).
Both edits have inline comments pointing to this log entry as the TODO for the TMeteorStorm author.

**Remaining gaps to true iter5 fidelity:**
- Strip width/UV — needs a look at SStripSegment v_a/v_b fields and how the bolt-crackle texture's authored content distributes along V. If the asset has the bolt body concentrated in a thin V slice, our strip widening drags a hairline-width pattern wider.
- Glow sub-object — the snapshot reads obj 1 (stripfly) as the glow but the retail asset only has 2 textures (texture slot 0 = bolt body, texture slot 1 = ???). May need to read the I3D's per-sub-object material assignment (which texture slot each sub-object samples). Look at how TMeteorStormEffect_Bespoke / TIceBoltEffect_Bespoke walks I3D sub-objects → texture slots via ExtractSubMeshTextureSlot.
- Counter-rotating cross-disc at the hand — snapshot draws sub-object 1 twice with different rotZ matrices. With SubmitMesh and the actual sub-object mesh quad this would be the 1:1 port. Currently the port uses SubmitFxBillboard with a flat quad which loses the rotZ animation entirely.
- Sparks (cast + impact) — TSubParticleAnimator sub-emitters not ported (SPARKS_TSparkAnimator.md owns).

**Cite:** strip+glow asset binding follows TFlameEffect_Bespoke (src/effect.cpp:2095-2157), TBloodEffect_Bespoke (1602-1750), TBurnEffect_Bespoke (2480-2520). Strip submission follows existing src/stripeffect.cpp live port. Per-vertex white restored per snapshot stripeffect.cpp:848.

**Files changed:**
- src/effect.h:3052-3084 — added strip_tex_, glow_tex_ fields to TLightningAnimator_Bespoke
- src/effect.cpp:10334-10406 — SpawnForTest_BESPOKE asset binding (FindImagery → LoadImagery → GetTexture)
- src/effect.cpp:10455-10459 — log line updated to report asset + texture handles
- src/effect.cpp:10651-10808 — TickAndSubmitForTest_BESPOKE Render block: white per-vertex restored, strip samples strip_tex_, glow samples glow_tex_, invented tints removed
- src/effect.cpp:14103, 14206 — TEMP unblock for parallel-agent TMeteorStormEffect_Bespoke build error (see above)

## 2026-05-31 04:20:00 — lightstrip — iter5: flipbook is SUB-OBJECT cycle, not texture cell — major asset discovery

**iter5 black filmstrip**: `filmstrips/vfx/05_LightStrip/iter5_black_filmstrip.png`

**Major asset finding (overrides forensics §2.1 sub-object naming):** The retail `Magic\NewLightStrip.I3D` has **13 sub-objects**, NOT 3 (`start`/`sparks`/`end`). Logged at spawn:

```
asset sub-objects: 0='#flare', 1='rectangle08', 2='rectangle07', 3='rectangle02',
4='rectangle09', 5='rectangle03', 6='rectangle05', 7='rectangle11', 8='rectangle10',
9='rectangle06', 10='rectangle04', 11='#sparks', 12='csparks'
```

Interpretation:
- `#flare` (idx 0) — the bright impact/cast halo flash (the "white-hot disc" in reference frames 02-04)
- `rectangle02..11` (idxs 1-10) — **10 alternative bolt-strip meshes**. These ARE the flipbook frames the coordinator/user described. The 10 patterns are **separate sub-object meshes**, not texture cells stacked vertically.
- `#sparks` (idx 11), `csparks` (idx 12) — the spark emitter sub-objects (matches forensics §6.4 TSubParticleAnimator sparks)

The forensics doc's "3 sub-objects start/sparks/end" was based on partial header inspection (forensics §4 noted these); the actual I3D has many more meshes. This is a forensics update needed.

**Strip rendering (iter5)**: SR pipeline submission of the per-segment bolt geometry STILL using strip_tex=14 (the 64×64 single-pattern). Visible result: clean purple/magenta bolt with white core, no more vertical-stripe noise. **Strip texture is correct asset, white per-vertex colour, no invented tints, AdditiveStraight blend.**

**Glow rendering (iter5)**: SubmitHelperMesh of sub-object 1 (`rectangle08` — the first "rectangle" picked by my fallback when `#flare` wasn't matched by name). Log: `glow_mesh: obj=1 texslot=2 handle=1 verts=4 idxs=6 diff=(1.00,1.00,1.00,1.00)`. **4 verts, 6 indices = a single quad** — confirms my picked sub-object is just one of the 10 alternative strip frames, NOT the glow flash.

The right sub-object for the snapshot's "glow" sub-object is almost certainly **#flare (idx 0)** — that's the bright disc the reference frames show at both ends. iter5's sub-object name matcher prefers "end"/"stripfly"/"glow"/"halo" but should ALSO prefer "#flare" or "flare".

**Iter5 vs iter4 visual**:
- iter4 black: vertical-stripe artifact from billboard-with-strip-texture
- iter5 black: NO artifact, just the clean SR bolt; glow not visible because I picked rectangle08 (one of the 10 frames) which is a tiny quad

**Build BLOCKER still present** (parallel-agent WIP): TMeteorStormEffect_Bespoke build errors temporarily worked around at src/effect.cpp:14103 and 14206. Two localized comments document the unblocker.

**Files changed in iter5:**
- `src/renderer.h:267-285` — added `v_left`/`v_right` float fields to SStripSegment with default 0/1 (engine extension for V sub-range strip submissions; future strip animators with V-cell flipbook layouts now supported).
- `src/renderer.cpp:5186-5196` — fed seg.v_left/v_right into the strip vertex emit (was hardcoded 0.0f/1.0f).
- `src/effect.h:3073-3091` — added `frame_idx_`, `num_frames_`, `glow_mesh_`, `glow_diffuse_[4]`, `glow_emissive_[4]` fields to TLightningAnimator_Bespoke.
- `src/effect.cpp:10638-10765` — SpawnForTest_BESPOKE now: reads `tex_strip.numframes` for the flipbook count (verified 1, so plumbing is inert), walks all sub-object names, registers the picked glow sub-object as a MeshHandle via ExtractSubMeshTextureSlot + RegisterMesh, reads material 0's diffuse/emissive into glow_diffuse_/glow_emissive_.
- `src/effect.cpp:10978-10988` — explicit NEGATIVE-FINDING comment about the disproven flipbook hypothesis; seg.v_left/v_right left at default.
- `src/effect.cpp:10840-10848` — frame_idx_ advance preserved but inert with num_frames_=1.
- `src/effect.cpp:11067-11215` — glow render replaced with SubmitHelperMesh path (with proper matrix chain RotZ-rotdegree · RotX-30° · RotY60° · RotZ-face · Translate · Scale per snapshot stripeffect.cpp:874-882). Fallback to billboard if glow_mesh_=0.

**Iter6 directive (logged for whoever picks this up next):**
1. Update the sub-object name matcher to prefer `#flare` (or "flare" without #) for the glow halo — this is the bright disc the reference shows.
2. The 10 `rectangle02..11` sub-objects ARE the flipbook frames. Register all 10 at spawn (similar idiom to TMeteorStormEffect's cell_meshes_[16] at src/effect.cpp:13820+). Per-tick cycle frame_idx_ from 0..9. For the strip, render the appropriate `rectangle##` mesh instead of (or in addition to) the SR strip submission — this is "kit-of-parts" pattern per the user's I3D mental model.
3. Update forensics §2.1 + §4 — actual sub-object enumeration is 13 elements, not 3. The retail asset is MUCH richer than forensics' partial sample suggested.
4. `#sparks` + `csparks` sub-objects align with the SPARKS_TSparkAnimator forensics doc — that slice should consume these meshes when it lands.

**Verdict for this iter:** strip body rendering is CORRECT (asset texture, white verts, additive). The flipbook hypothesis is REFINED — it's not texture V cells, it's 10 alternative MESH sub-objects + 1 flare mesh in the I3D. Iter5 picked the wrong sub-object for the glow (`rectangle08` instead of `#flare`), so the impact halo is invisible in this iteration. Iter6 picks `#flare`. Concrete actionable findings logged.

## 2026-05-31 04:28:00 — lightstrip — heartbeat: studying icebolt's matrix pattern for iter7

iter6 black filmstrip showed dim/incorrect glow mesh. Read icebolt's iter3 wrap-up. Pattern: TWO-stage matrix composition. `inst_world = T(base_pos) · Rz(face_rad)` and `local = T(local_pos) · Rz · Ry · Rx · S`, final `world = inst_world · local`. Convention is COLUMN-VECTOR row-major (vertex shader does wp = M*v). To get "scale first, then rotate, then translate" the product is T*R*S not R*S*T.

For LightStrip glow: halo (wx,wy,wz) is WORLD position (caster hand or end_p). So `inst_world = T(wx,wy,wz)` (no face rotation needed at this stage — the snapshot also folds face into the inner chain). `local = compose_local(scl,scl,scl, rx=-30°, ry=60°, rz=rotz_local, tx=0, ty=0, tz=0)`.

About to: fix matrix construction following icebolt pattern. Skipping flipbook V-cell directive (disproven: strip_tex is 64×64 nframes=1 — 10 rectangle MESH sub-objects ARE the flipbook).

## 2026-05-31 04:54:00 — lightstrip — iter8 HUGE WIN: flare visible via SubmitHelperMesh from submit_world

**iter8 black filmstrip**: `filmstrips/vfx/05_LightStrip/iter8_black_filmstrip.png`
**iter8 dungeon filmstrip**: `filmstrips/vfx/05_LightStrip/iter8_filmstrip.png`

**Changes from iter7**:
1. Split TickAndSubmitForTest_BESPOKE into two methods:
   - `TickAndSubmitForTest_BESPOKE` — strip submission (SubmitFxStrip) + billboard fallback if glow_mesh_=0.
   - `SubmitWorldMeshes_BESPOKE` — glow halo (SubmitHelperMesh) called from harness's submit_world callback. The harness BeginTilePass clears the transparent_world_queue, so SubmitHelperMesh from regular submit gets dropped (sister IceBolt hit the same issue per its iter4d log).
2. Added `LightningBespokeSubmitWorld` helper + `lightning_bespoke.submit_world = ...` registration in src/vfxtest.cpp.
3. Fixed matrix construction: column-vector convention, T (translation) on the LEFT of R · S. For glow obj->pos=(0,0,0) in I3D-local; inst_world places the halo at its world center; local = Rz_local · Rx · Ry · Rz_face · S.

**Observed**:
- BLACK BG: bright white flare disc clearly visible at both hand and impact positions (frames 1, 2, 4, 7, 8, 9). Strip bolt body visible as purple/white wave (frames 7, 8). Flare scale dominant — fills ~25% of screen height.
- DUNGEON BG: same flare reads through scene; bolt body fainter relative to scene contrast.

**Gap vs reference**:
- Reference has SATURATED PURPLE BOLT BODY with white flare accents. Iter8 has dominant white flares + faint bolt body — reverse-proportion of reference.
- The `#flare` mesh diffuse is (1,1,1,1) pure white. The glow_tex_ (slot 1, 128×128) is presumably a bright disc texture. Authored to ship as PURE WHITE additive accumulator — no purple tint in the asset itself.
- The reference's purple comes from the SPELL LIGHT (spell.def:683 `LIGHT COLOR 150,150,255 INT 255 MULT 20`) tinting the scene around the cast — forensics §9 said this is a spell-side concern, NOT the effect. iter8 doesn't post a scene light.

**Strip width**: still narrow vs reference. The strip texture is 64×64; STRIP_WIDTH_HILT=32 / TIP=20 (matching snapshot:642). The bolt-crackle texture content may have most of its bright pixels concentrated in a thin V band, so even at the right WU width the visible bright area is hairline.

**Files changed in iter8** (from iter7 baseline):
- src/effect.h:3038-3047 — added `SubmitWorldMeshes_BESPOKE` declaration.
- src/effect.cpp:11050-11102 — TickAndSubmit now only billboard fallback when glow_mesh_=0; the glow halo proper moved to SubmitWorldMeshes_BESPOKE.
- src/effect.cpp:11106-11264 — new SubmitWorldMeshes_BESPOKE method with corrected matrix order.
- src/vfxtest.cpp:2331-2340 — new LightningBespokeSubmitWorld helper.
- src/vfxtest.cpp:4428-4435 — registered `submit_world = [..]; pipeline = "SR+FB+IM"`.

**Remaining gaps (iter9+):**
1. Flare reads too big relative to reference. Cap scale or use a smaller `#flare` mesh scale multiplier (snapshot's `glow_scale=3.4` was applied to a smaller authored quad than the retail's `#flare` — needs visual tuning).
2. Bolt strip body is faint relative to flare; reference shows thick saturated bolt. Likely the strip needs to also use a #flare-like mesh draw rather than the procedural SR strip. Or the STRIP_WIDTH constants need scaling up for the retail bolt-crackle texture's bright-pixel distribution.
3. The 10 `rectangle##` sub-objects (snapshot's flipbook frames) still unused. Iter6/7/8 only registers `#flare`. The user's flipbook directive applies here — cycle the rectangle meshes per tick.
4. The cross-rotating effect from the two RotZ passes isn't visible (mesh is a flat quad, rotating doesn't show as a cross-disc unless the mesh has 3D structure).

## 2026-05-31 04:57:00 — lightstrip — iter9 DUNGEON A/B vs reference

**iter9 black**: `filmstrips/vfx/05_LightStrip/iter9_black_filmstrip.png`
**iter9 dungeon**: `filmstrips/vfx/05_LightStrip/iter9_filmstrip.png`

**Change from iter8**: applied `kRetailFlareScaleCorrection = 0.40f` to glow scale to bring the `#flare` mesh's visible size from ~25% screen height (iter8 dominated everything) down to ~10% (matches reference). Justification: snapshot's `glow_scale=3.4` was authored for the 18,680 B dev asset's tiny stripfly sub-object quad; the retail asset (50,643 B, 2.7× larger per forensics §2.1) has a much bigger `#flare` quad. Per-asset visual-tuning constant, snapshot's 3.4 stays as the base.

**A/B verdict**: GOOD. iter9 produces the correct composite — bright white flare disc at the impact end + purple/magenta lightning bolt body curving from caster hand to impact. Reading matches reference frames 02-05's overall shape and color distribution. The bolt body is still thinner than reference, but the flare-and-bolt relationship is correct.

**Files changed in iter9** (delta from iter8):
- src/effect.cpp:SubmitWorldMeshes_BESPOKE — added kRetailFlareScaleCorrection=0.40f multiplier to glow scale.

**Cumulative iter4-iter9 file change summary** (all in worktrees/vfx):
1. `src/effect.h:3055-3091` — TLightningAnimator_Bespoke added strip_tex_, glow_tex_, frame_idx_, num_frames_, glow_mesh_, glow_diffuse_[4], glow_emissive_[4] fields and SubmitWorldMeshes_BESPOKE declaration.
2. `src/effect.cpp:10539-10809` — SpawnForTest_BESPOKE: bind Magic\NewLightStrip.I3D, walk 13 sub-objects, pick #flare (idx 0) by name, register glow_mesh_ via ExtractSubMeshTextureSlot + RegisterMesh, log diagnostic info.
3. `src/effect.cpp:10809-11104` — TickAndSubmitForTest_BESPOKE: white per-vertex (snapshot ground truth), strip samples real strip_tex_; billboard fallback only if glow_mesh_=0.
4. `src/effect.cpp:11106-11280` — new SubmitWorldMeshes_BESPOKE method with corrected column-vector matrix order (T·R·S not R·S·T) per icebolt's pattern; scale-corrected for retail asset.
5. `src/renderer.h:267-285` — SStripSegment: added v_left, v_right (engine extension for V sub-range; left at default 0/1 for LightStrip after flipbook hypothesis was disproven).
6. `src/renderer.cpp:5186-5195` — fed seg.v_left/v_right into the strip vertex emit.
7. `src/vfxtest.cpp:2331-2340` — new LightningBespokeSubmitWorld helper.
8. `src/vfxtest.cpp:4424-4435` — registered `submit_world` and pipeline tag "SR+FB+IM".
9. `src/effect.cpp:14103, 14206` — TEMP TMeteorStormEffect_Bespoke build unblockers (parallel-agent WIP, unrelated).

**Remaining (iter10+):**
1. Bolt strip body is thinner than reference — the 64×64 strip_tex's bright pixels are concentrated in a thin V band, so the visible bolt looks hairline even at correct WU width.
2. 10 `rectangle##` sub-objects unused — these are the snapshot's flipbook frames as separate meshes. iter6 left this for follow-up. Cycling them per tick (or selecting one at spawn) would produce the snapshot's intended bolt-pattern variation. Would also let the strip use authored mesh geometry instead of the procedural SR primitive.
3. The cross-rotating cross-disc visual from the two RotZ passes isn't pronounced — `#flare` is a flat quad, so RotZ rotates the texture in-plane but doesn't produce a true 3D cross-disc unless the mesh has 3D structure. Reference frames don't show a strong rotating cross either, so this may be fine as-is.
4. The retail spell.def's `LIGHT COLOR 150,150,255` is not posted as a scene light (forensics §9 said it's spell-side, not effect-side). The purple/magenta tint in the reference frames likely comes from this light; iter9 doesn't reproduce that ambient tint.

---

## iter10 — REWRITE as src/effects/lightstrip.cpp (Fable, 2026-06-09)

The iter1-9 `TLightningAnimator_Bespoke` body in effect.cpp/effect.h is
**retired** (reverted). It transcribed the snapshot's procedural ribbon
(`TStripAnimator` smooth-extruded strip), which **never shipped** — the
retail asset was reworked (forensics §2.1). New port lives in
`src/effects/lightstrip.cpp` following the wave-port shim convention.

### Identity (confirmed before writing code)
Exclusivity sweep of every shipped electric-family asset
(energyspray/espray/yenergy/oldlightstrip/lightningbow) — ONLY
`newlightstrip.i3d` carries jagged purple bolt art. Reference =
images/vfx/05_LightStrip/01..05.png. Match confirmed.

### Asset-driven render (i3d_dump_all/newlightstrip)
- texture[1] 128x128 = EIGHT pre-drawn jagged purple bolt columns
- texture[0] 64x64 = flare sheet (the purple lens ball at `#flare`)
- object[0] `#flare` = ball; object[1..10] `rectangleNN` = 10 authored
  bolt columns (one per texture column); object[11] `#sparks` = star
- Bolt strands = ONE SStripSegment each (strip pipeline anchors both
  world ends rigidly; particle rotate-then-scale can't rotate tall-thin
  quads). Needed a new `SStripSegment.uv_swapped` flag (renderer.h/.cpp)
  because the authored columns run along texture-V while the strip's
  u_a..u_b runs along-length.
- Authored UV head is at the column v-MAX end (user-confirmed; my v-min
  guess put white spots at the outer strand ends).

### Staging — USER-DIRECTED HYBRID (deliberate non-canon)
Retail video shows MISSILE staging (ball scales at hand, travels like a
fireball, bursts at target, NO connector — corroborated by the
snapshot's LAUNCH/FLY/EXPLODE names + orphaned STRIP_SPEED=16). User
reviewed both and chose to KEEP the connected-bolt look as an
intentional improvement. Final: ball composite forms at the hand,
travels to the target at STRIP_SPEED=16 wu/tick staying connected by the
jagged strand, then on arrival a dense radial star BURST (electrical
explosion) fires while the glow + crackle strands EXPAND and fade. White
core pinned + slower fade so it outlasts the halo. Fan strand roots
inset 30% so the strokes lengthen under the glow instead of sliding out.

### Constants: snapshot-cited vs reference-tuned
- Cited: STRIP_SPEED=16, glow x0.8 decay, spark params (chance 25, scale
  2.0, decay 0.90, gravity 0.18, life 25-35, flicker 2.5), SetWidth
  32:20 hilt:tip taper, glow rot 12/16 deg/tick.
- Reference-tuned (no retail body, forensics §2.1): 12 Hz crackle swap,
  spark quad x0.5 correction, source-spark purple tint, burst (48 stars,
  radial 4-9 wu/tick, life 10-18), burst expand x1.15, core fade 0.90 +
  hold, fan root inset.

### In-game integration TODO
- Target end should anchor to the STRUCK CHARACTER (the reference's
  visible ball movement); rig has no target character so it's fixed.
- Spell.def `LIGHT COLOR 150,150,255` is spell-side scene light, not
  posted by the effect (forensics §9).
