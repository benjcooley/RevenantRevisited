# 04 — Effect Class Catalog (original 1998/99 VFX)

> **Scope.** A taxonomy of the *original* Revenant effect classes, grouped by
> family. For each effect: class name, INVENTORY id (if mapped), what the original
> **draws** (primitive class + pipeline shorthand), primary asset, trigger, a
> one-line behavior summary, complexity archetype, and a `file:line` cite.
> This is reference for forensics agents — there are **no** "how to rebuild in our
> engine" notes here; engine mapping is reconstruction-side
> (see `docs/vfx/INVENTORY.md` for port status).

---

## 0. How to read this catalog

### 0.1 Pipeline / primitive shorthand

The "Draws" column says what the *original* renders. Shorthand (from
`docs/vfx/INVENTORY.md:30-48`; **GAP:** `NOMENCLATURE.md` does not exist yet):

| Code | Original draws |
|------|----------------|
| `FB` | flipbook billboard (camera-facing textured quad, frame-cycled) |
| `PE` | particle emitter (many small sprites with color+fade-over-life) |
| `SR` | strip / ribbon (textured generated geometry along a path; UV-scrolled) |
| `IM` | I3D mesh (a real `.I3D` sub-mesh, animated/rot/scaled) |
| `LS` | dynamic light source (no sprite; lights the scene) |
| `VO` | volumetric / screen overlay (fog, freeze tint, aura glow) |
| `CX` | composite — multiple of the above driven together |
| `-`  | no draw output (audio-only / data-only / base class) |

### 0.2 Trigger taxonomy

`spell-cast` (a `spell.def` VARIANT names the effect class) · `monster-attack`
(NPC/boss spell or melee) · `scripted` (placed by map script / `ATTACHEFFECT`) ·
`ambient` (world placement, e.g. torch/candle/waterfall) · `sub-effect` (spawned
by another effect) · `vestigial` (declared/registered but no live caller found).

### 0.3 Complexity archetype gradient (author's framing)

Each effect is tagged with the archetype(s) it fits, on a complexity gradient:

| Tag | Archetype |
|-----|-----------|
| **(a)** | simple particle emitter — color + fadeout over life |
| **(b)** | rising particles |
| **(c)** | swirling particles |
| **(d)** | trails (fireball) + beams (ice freeze / lightning) |
| **(e)** | world-oriented shape centered on the effect origin (disc/ring/dome) |
| **(f)** | I3D-geometry effect with rotation / scale / UV-scroll |
| **(g)** | complex composite (fire ring, meteor storm) — **multi-component, needs careful decomposition** |

### 0.4 Name → class binding

Effect classes self-register a *name* via `DEFINE_BUILDER("Name", TFooEffect)` and
a paired *animator* via `REGISTER_3DANIMATOR("Name", TFooAnimator)`. The name is
the same string used in `Class.Def`'s `CLASS "EFFECT"` block (asset binding) and
in `spell.def` VARIANT lines (the 4th field). Source files:
`src/effect_old.cpp` (most), `src/effect2.cpp` (Para/ManaDrain/Shriek/Puke/
Funnel/Invisible/Rest/Cataclysm/IceStorm), `src/missileeffect.cpp` (missile
family), `src/stripeffect.cpp` (strips), `src/effect.cpp` (TFlame/TEffect base).

### 0.5 Effect ⇄ Animator split (universal pattern)

Almost every effect is a **pair**: a `TFooEffect : TEffect` (the simulation /
lifetime / spell-coupling object) and a `TFooAnimator : T3DAnimator` (the
per-frame draw). The Effect runs `Pulse()` (sim tick); the Animator runs
`Animate()` (advance) + `Render()` (draw). When this catalog lists one row per
effect, the matching animator carries the visual — both must be ported together.

---

## 1. Fire family

**Shared signature:** additive blend (`SetAddBlendState`, ONE/ONE,
`src/effect_old.cpp:235-244`), hot orange/yellow saturated color, black-keyed
textures (premultiplied). Fire effects split into FB scatter-cards
(flame/fire/swarm), CX composites (meteor/column/cone via storm/particle systems),
and IM missile cores (fireball). Pale/grey fire = port broken.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| F01 | `TFlameEffect` | FB | `Magic\Flame.I3D` (`Class.Def:2030`) | ambient (torch/candle), scripted | (a)(f) | Single flame card, 18-frame flipbook + spark particles; the canonical torch flame. | `DEFINE_BUILDER("FLAME", …)` `src/effect_old.cpp:4398`; anim `:4486-4540` (frame wrap at 18, `:4486-4519`) |
| F02 | `TFlameAnimator` | FB | (drives F01/Flame) | — | (a) | Flame-card animator: scale-down ×0.5, cycle 18 frames. | `REGISTER_3DANIMATOR` n/a (built via FLAME); body `src/effect_old.cpp:4486-4540` |
| F03 | `TFireEffect` | FB | `Misc\Fire.I3D` (`Class.Def:2015`) | **vestigial** | (a)(e) | Multi-quad scatter fire patch. **No `DEFINE_BUILDER("Fire", …)` exists** — declared only. | decl `src/effect.h:380`; vestigial note `src/effect.cpp:3056` |
| F04 | `TFireAnimator` | FB | (would drive F03) | **vestigial** | (a) | Declared, never registered. | decl `src/effect.h:634` |
| F05 | `TFireSwarmEffect` | PE | `Magic\FireSwarm.i3d` (`Class.Def:2062`) | spell-cast | (a)(c) | Swarm of small fire particles. | `DEFINE_BUILDER("FireSwarm", …)` `src/effect_old.cpp:10453`; anim `:10483-10524` |
| F06 | `TFaultFireEffect` | IM+SR | `Magic\faultfire.i3d` (`Class.Def:2071`) | spell-cast | (d)(f) | Rising fire along a fault line; **UV-scrolls `tu` by `random(2,8)/100`** per frame (continuous flame flow). | `DEFINE_BUILDER("FaultFire", …)` `src/effect_old.cpp:11127`; UV scroll `:11174-11179` |
| F07 | `TFireBallEffect` | IM+PE+LS | `Magic\NewFireBall.I3D` (`Class.Def:2031`) | spell-cast (`"fireball"`, `spell.def:848,850`) | (d)(f) | Flying fireball mesh + **trailing photon-spark particles** + light. Base = `TMissileEffect`, not `TEffect`. | `DEFINE_BUILDER("FireBall", …)` `src/missileeffect.cpp:479`; trail `:573-580`; class `src/missileeffect.h:125` |
| F08 | `TFireBallAnimator` | IM+PE | (drives F07) | — | (d) | Fireball mesh anim + spark trail emit (FIREBALL_MAX/NORM_SPARK). | `REGISTER_3DANIMATOR("FireBall", …)` `src/missileeffect.cpp:498`; `:549-580` |
| F09 | `TFireColumnEffect` | IM+PE+LS | `Magic\FireColumn.I3D` (`Class.Def:2040`) | spell-cast (impact of fireball) | (e)(g) | Vertical fire column at impact. Base `TMissileEffect`. | class `src/missileeffect.h:242`; recon `recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml` |
| F10 | `TFireColumnAnimator` | IM+PE | (drives F09) | — | (e) | Column mesh + particles. | class `src/missileeffect.h:264` |
| F11 | `TFlameDiscEffect` | IM+PE | `Magic\FlameDisc.I3D` (`Class.Def:2033`) | spell-cast | (e)(f) | World-oriented spinning flame disc on the ground. Base `TMissileEffect`. | class `src/missileeffect.h:301`; recon `recon/mappings/TFlameDiscEffect_cls_0x5b45ac_candidate.yaml` |
| —   | `TFireFlashEffect` | IM+VO | `Magic\fireflash.I3D` (`Class.Def:2048`) | spell-cast (`"fireflash"`, `spell.def:79,89`) | (e)(f) | Expanding fire flash burst. Ghidra-merged with FireWind+Burn (`cls_0x5a9194`). | `DEFINE_BUILDER("FireFlash", …)` `src/effect_old.cpp:1988`; anim `:2113-2392` |
| —   | `TFireWindEffect` | PE+IM | `Magic\firewind.I3D` (`Class.Def:2049,2054`) | spell-cast (`"firewind"`, `spell.def:723,725`) | (c)(d) | Swirling fire-wind cone. | `DEFINE_BUILDER("FireWind", …)` `src/effect_old.cpp:2516`; anim `:2686-3091` |
| —   | `TFireConeEffect` | PE+IM | `Magic\FireCone.I3D` (`Class.Def:2076`) | spell-cast (`"firecone"`/Napalm, `spell.def:1097,1099`) | (d)(g) | Forward fire-cone spray (Napalm). | `DEFINE_BUILDER("FIRECONE", …)` `src/effect_old.cpp:7255`; anim `:7319-7707` |
| X20 | `TDragonFireEffect` | FB+PE | `Misc\DragonFire.I3D` | monster-attack (Dragon NPC) | (d) | Dragon breath fire stream. Name conjectural (recon `cls_0x5aacac`). | `DEFINE_BUILDER("DragonFire", …)` `src/effect_old.cpp:7782`; anim `:7803-7939` |
| X19 | `TYFireBallEffect` | IM+PE+LS | (Yhagoro variant) | monster-attack (Yhagoro NPC) | (d) | NPC fireball variant (`spell.def "YFireBall"`). | recon `recon/mappings/TYFireBallEffect_cls_0x5b4814_candidate.yaml`; spell `data/Resources/spell.def` |

---

## 2. Ice / cold family

**Shared signature:** cool blue/cyan/white saturated color, additive or alpha
blend; bolt = IM mesh + beam, "Iced" = a status overlay (no own mesh). Pale/no
blue = port broken (`vfx_color_health_signal`).

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| I20 | `TIceEffect` | PE | `Misc\ice.I2D` (`Class.Def:2017`) | **vestigial** | (a) | Ice shard burst — declared, **no `DEFINE_BUILDER("Ice", …)`**. | decl `src/effect.h:463`; no registration in `src/` |
| I21 | `TIceBoltEffect` | IM+SR(d) | `Magic\icebolt.I3D` (`Class.Def:2055`) | spell-cast (`"icebolt"`, `spell.def:1228`) | (d)(f) | Flying ice-bolt mesh + freeze beam/trail; posts "Iced" status on hit. | `DEFINE_BUILDER("IceBolt", …)` `src/effect_old.cpp:7984`; anim `:8015-8464` |
| I22 | `TIcedEffect` | VO | `Magic\iced.i3d` (`Class.Def:2056`) | sub-effect (status tag) | (e) | Frozen-character overlay. **Not a standalone TEffect class** — "Iced" is a status posted by IceBolt/Storm. | `DEFINE_BUILDER("Iced", …)` `src/effect_old.cpp:8723` (note INVENTORY I22 says "no class"; **see §11 discrepancy**); anim `:8749-8900` |
| —   | `TIceStormEffect` | CX | (uses storm system) | spell-cast (`"tornado"`/Ice Storm, `spell.def:1118,1120`) | (c)(g) | Ice-storm area effect; base `TMissileEffect`. | `DEFINE_BUILDER("IceStorm", …)` `src/effect2.cpp:3500`; anim `:3504-3521` |

> Cold sparkle assets: `"Icedsparks"`/`"Snow"` reuse `Magic\iced.i3d`
> (`Class.Def:2057,2061`); the `TSparkAnimator` (`REGISTER_MULTI_3DANIMATOR_("sparks", …)`,
> `src/effect_old.cpp:4688`) is the shared sparkle particle animator.

---

## 3. Magic / arcane family

**Shared signature:** cool saturated palette (blue/cyan/purple/gold), additive
glow; many are VO overlays (aura/invisible/shield) or status spells with small or
no sprite. Photon = a missile (energy bolt).

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| M01 | `TAuraEffect` | VO+LS | `Magic\Aura.I3D` (`Class.Def:2078`) | spell-cast (`"aura"`, `spell.def`) | (e)(f) | Character aura glow dome. | `DEFINE_BUILDER("Aura", …)` `src/effect_old.cpp:3479`; anim `:3535-3657` |
| M02 | `TAuraAnimator` | VO | (drives M01) | — | (e) | Aura dome anim. | `REGISTER_3DANIMATOR("Aura", …)` `src/effect_old.cpp:3533` |
| M03 | `THealEffect` | VO+PE | `Magic\Heal.I3D` (`Class.Def:2021`) | spell-cast (`"heal"`, `spell.def:744,750,756,762`) | (b)(e) | Rising heal sparkles + glow. | `DEFINE_BUILDER("Heal", …)` `src/effect_old.cpp:461`; anim `:628-770` |
| M04 | `TBurnEffect` | VO | `Magic\burnbabyburn.I3D` (`Class.Def:2050`) | sub-effect (on-fire status) | (e) | Burning-character overlay. | `DEFINE_BUILDER("BURN", …)` `src/effect_old.cpp:3214`; anim `:3292-3449` |
| M07 | `TPhotonEffect` | IM+LS | `Magic\Photon.I3D` (`Class.Def:2018`) | spell-cast | (d)(f) | Energy photon bolt (missile). Base `TMissileEffect`. | class `src/missileeffect.h:65`; recon `recon/mappings/TPhotonEffect_cls_0x5b401c_candidate.yaml` |
| M08 | `TPixieEffect` | PE+LS | `misc\pixies.i3d` (`Class.Def:2075`) | spell-cast / ambient | (c) | Swirling pixie light-particles. | `DEFINE_BUILDER("Pixie", …)` `src/effect_old.cpp:12082`; anim `:12109-12229` |
| M09 | `TTeleporterEffect` | IM+LS+PE | `Misc\IrisFlare.I3D` (`Class.Def:2039`) | spell-cast (`"Teleporter"`, 16× in `spell.def`) | (e)(f) | Iris-flare teleport portal. Shares asset with IrisFlare. | `DEFINE_BUILDER("Teleporter", …)` `src/effect_old.cpp:5471`; anim `:5762-5942` |
| X21 | `TFizzleEffect` | PE | `Magic\Fizzle.I3D` (`Class.Def:2077`) | spell-cast (spell-fail) | (a) | Spell-fizzle puff. | `DEFINE_BUILDER("Fizzle", …)` `src/effect_old.cpp:12329`; anim `:12347-12489` |
| —   | `TManaDrainEffect` | SR/VO | `Magic\ManaDrain.I3D` (`Class.Def`) | spell/monster-attack (`"ManaDrain"`, 5× `spell.def`) | (d) | Mana/health drain beam between caster+victim. | `DEFINE_BUILDER("ManaDrain", …)` `src/effect2.cpp:738`; anim `:906-931` |
| —   | `TParaEffect` | IM+SR | `Magic\Paralize.I3D` (`Class.Def:2080`) | spell-cast (`"Paralize1"`, `spell.def`) | (c)(f) | Paralyze swirl; **UV-scrolls `tu`/`tv` by `val*80`**. | `DEFINE_BUILDER("Paralize1", …)` `src/effect2.cpp:86`; UV scroll `:649-650` |
| —   | `TInvisibleEffect` | VO | `Magic\Invisible.I3D` (`Class.Def`) | spell-cast (`"Invisible"`, `spell.def`) | (e) | Invisibility shimmer overlay. | `DEFINE_BUILDER("Invisible", …)` `src/effect2.cpp:2346`; anim `:2395-2415` |
| —   | `TShriekEffect` | VO/PE | `Magic\Shriek.I3D` (`Class.Def`) | monster/spell (`"Shriek"`, `spell.def`) | (a) | Sonic shriek burst. | `DEFINE_BUILDER("Shriek", …)` `src/effect2.cpp:1184`; anim `:1204-1222` |
| —   | `TPukeEffect` | PE | `Magic\Puke.I3D` (`Class.Def`) | monster/spell (`"Puke"`, `spell.def`) | (a)(b) | Vomit projectile/spray. | `DEFINE_BUILDER("Puke", …)` `src/effect2.cpp:1383`; anim `:1403-1420` |
| —   | `TRestEffect` | VO | (rest/regen) | spell-cast (`"Rest"`/Regeneration) | (e) | Rest/regen overlay. | `DEFINE_BUILDER("Rest", …)` `src/effect2.cpp:2735`; anim `:2739-2756` |
| —   | `TCreateFoodEffect` | IM | `Magic\createfood.I3D` (`Class.Def:2047`) | spell-cast (`"createfood"`) | (f) | Conjure-food materialization. | `DEFINE_BUILDER("CreateFood", …)` `src/effect_old.cpp:990`; anim `:1034-1267` |
| —   | `TCureEffect` | VO+PE | `Magic\cure.I3D` (`Class.Def:2046`) | spell-cast (Cure) | (b)(e) | Cure/cleanse glow. | `DEFINE_BUILDER("Cure", …)` `src/effect_old.cpp:1434`; anim `:1587-1910` |
| —   | `TReviveEffect` | IM | `Magic\Ribbon.I3D` (`Class.Def:2027`) | spell-cast | (f) | Revive (registered under name `"Ribbon"` — see §11). | `DEFINE_BUILDER("Ribbon", TReviveEffect)` `src/effect_old.cpp:3967` |

---

## 4. Lightning / energy / strip family (`SR`)

**Shared signature:** generated strip/ribbon geometry along a path, **UV-scrolled
along U** to make it flow (`ScrollTexture`, §02 doc §5.3), additive blend, bright
electric color. `TLightningAnimator` is the shared strip animator.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| S01 | `TStripEffect` | SR | `Magic\NewLightStrip.I3D` (`Class.Def:2034`) | spell-cast (`"lightstrip"`, `spell.def`) | (d) | Generic light-strip / lightning ribbon; the SR base. | `DEFINE_BUILDER("LightStrip", …)` `src/stripeffect.cpp:392`; class `src/stripeffect.h:57` |
| S04 | `TLightningAnimator` | SR | (drives S01) | — | (d) | Strip animator; **UV-scroll `-0.1`/frame** for flow; Effect3.cpp class. | `REGISTER_3DANIMATOR("LightStrip", TLightningAnimator)` `src/stripeffect.cpp:470`; scroll `:817`; recon `recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml` |
| S02 | `TWindStripEffect` | SR | `Magic\wind.I3D` (`Class.Def:2065`) | spell-cast | (d) | Wind streak ribbon. | `DEFINE_BUILDER("WindStrip", …)` `src/stripeffect.cpp:979`; class `src/stripeffect.h:153` |
| S03 | `TWindStripAnimator` | SR | (drives S02) | — | (d) | Wind-strip animator. | `REGISTER_3DANIMATOR("WindStrip", …)` `src/stripeffect.cpp:1046` |
| S06 | `TStripAnimator` | SR | (strip infra) | — | (d) | Strip geometry generator + `ScrollTexture()`. | class `src/effectcomp.h:249`; `ScrollTexture` `src/effectcomp.cpp:863-871` |
| S05 | `TShockAnimator` | SR | (shock arc) | sub-effect | (d) | Shock/arc strip variant. | class `src/effectcomp.h:220` |
| S07 | `TStreamerEffect` | SR | `Magic\streamer.i3d` (`Class.Def:2059`) | spell-cast (`"Streamer"`, `spell.def:176`) | (d) | Streaming energy ribbon (Cure Poison). | `DEFINE_BUILDER("Streamer", …)` `src/effect_old.cpp:10224`; anim `:10297-10390` |
| X11 | `TRibbonAnimator` | SR | `Magic\Ribbon.I3D` (`Class.Def:2027`) | spell-cast | (d)(f) | Ribbon-mesh strip animator. | `REGISTER_3DANIMATOR("RIBBON", …)` `src/effect_old.cpp:4069`; anim `:4084-4244`; recon `recon/mappings/TRibbonAnimator_cls_0x5b9fac_candidate.yaml` |
| —   | `TLightning` (`"LightningStorm"`) | CX | `Magic\Lightning2.I3D` (`Class.Def:2091`) | spell-cast (EnergySpray/LightningStorm) | (c)(g) | Area lightning storm. | `Class.Def:2091`; energy strings in `cls_0x5b0a28` |

---

## 5. Projectile / missile family (`TMissileEffect` base — NOT `TEffect`)

**Shared signature:** a flying `.I3D` mesh that travels origin→target, usually
with a particle trail (d) and often a point light; on impact spawns a sub-effect
(column/flash). Base hierarchy is `TMissileEffect` (`src/missileeffect.h:31`),
distinct from the `TEffect`-derived spell/area effects.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| S08 | `TMissileEffect` | IM | (base) | base class | (d)(f) | Missile base: travel + trail + impact dispatch. | class `src/missileeffect.h:31`; recon `recon/mappings/TMissileEffect_cls_0x5b3c18_candidate.yaml` |
| F07 | `TFireBallEffect` | IM+PE+LS | `Magic\NewFireBall.I3D` | spell-cast | (d) | (see Fire §1) | `src/missileeffect.h:125` |
| M07 | `TPhotonEffect` | IM+LS | `Magic\Photon.I3D` | spell-cast | (d) | (see Magic §3) | `src/missileeffect.h:65` |
| F09 | `TFireColumnEffect` | IM+PE+LS | `Magic\FireColumn.I3D` | impact sub-effect | (e)(g) | (see Fire §1) | `src/missileeffect.h:242` |
| F11 | `TFlameDiscEffect` | IM+PE | `Magic\FlameDisc.I3D` | impact sub-effect | (e) | (see Fire §1) | `src/missileeffect.h:301` |
| X18 | `TArrowEffect` | PE | `Equip\Arrow.I3D` (`Class.Def:1954`) | combat (arrow impact) | (a) | Arrow-impact particle puff. | recon `recon/mappings/TArrowEffect_cls_0x5b0cfc_candidate.yaml` |
| S09 | `TWeaponSwipe` | SR | (weapon mesh extents) | combat (melee swing) | (d) | Sword/weapon swing trail ribbon, built from weapon vertex extents. **Legacy class, currently attic** in port. | `src/weapontrail.cpp:45-59` (legacy class outline); `src/charanimator.h:13` |

---

## 6. Blood / gore / combat family

**Shared signature:** dark red, alpha blend (NOT additive — blood is opaque-ish),
gravity-driven downward particles (b inverted), splat decals on surfaces.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| B01 | `TBloodEffect` | PE | `Misc\Blood.I3D` (`Class.Def:2014`) | combat (on-hit) | (a) | Directional blood spray; gravity Euler + alpha fade. | `DEFINE_BUILDER("Blood", …)` `src/effect_old.cpp:11243`; anim `:11269-11326` |
| B02 | `TBloodAnimator` | PE | (drives B01) | — | (a) | Blood droplet animator. | `REGISTER_3DANIMATOR("Blood", …)` `src/effect_old.cpp:11261` |
| B03 | `TBloodSystem` | PE | (blood infra) | — | (a) | Particle-system specialization: droplets/splats, size variants, trails. | class `src/effectcomp.h:377` |
| B04 | `TPulpEffect` | PE+IM | `Misc\Pulp.I3D` (`Class.Def:2051`) | combat (gib/death) | (a)(f) | Gore burst: body-part meshes + blood splash. | `DEFINE_BUILDER("PULP", …)` `src/effect_old.cpp:7046`; anim `:6759-6972` |

---

## 7. Water / environment / ambient family

**Shared signature:** mixed — water/drip/ripple are alpha-blended subtle effects;
mist/fog are VO volumetrics; many are ambient (placed in the world, not cast).
Several share `misc\water.i3d` / `Magic\mist*.i3d`.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| H01 | `TWaterEffect` | PE | `misc\water.i3d` (`Class.Def:2072`) | ambient | (a) | Water surface particles. | `DEFINE_BUILDER("Water", …)` `src/effect_old.cpp:11879`; anim `:11950-12016` |
| H02 | `TWaterFallEffect` | CX | `misc\water.i3d` (`Class.Def:2073`) | ambient | (b)(g) | Waterfall: falling water particles + spray + sound. Ghidra-merged w/ Blood+Mist. | `DEFINE_BUILDER("WaterFall", …)` `src/effect_old.cpp:11669`; anim `:11747-11813` |
| H03 | `TRippleEffect` | FB | `Magic\ripples.I3D` (`Class.Def:2067`) | sub-effect (drip landing), ambient | (e) | Expanding ground-ring ripple (world-XY billboard). | `DEFINE_BUILDER("Ripple", …)` `src/effect_old.cpp:10686`; anim `:10713-10866` |
| H04 | `TDripEffect` | PE | `Magic\drip.i3d` (`Class.Def:2068`) | ambient | (b) | Cave/ceiling drip; dead→wait→fall, spawns `TRippleEffect` on landing. Couples to a landing **sound**. | `DEFINE_BUILDER("Drip", …)` `src/effect_old.cpp:10951`; anim `:10987-11074` |
| M05 | `TMistEffect` | PE/VO | `Magic\mist.i3d` (`Class.Def:2069`) | ambient, scripted (teleport) | (b) | Drifting mist wisps; gravity, respawn on landing. | `DEFINE_BUILDER("Mist", …)` `src/effect_old.cpp:11346`; anim `:11376-11463` |
| —   | `TMistFogAnimator` | VO | `Magic\mistfog.i3d` (`Class.Def:2070,2074`) | ambient | (e) | Volumetric fog plane. | `REGISTER_3DANIMATOR("MistFog", …)` `src/effect_old.cpp:11558`; anim `:11560-11614` |
| W06 | `TFogAnimator` | VO | `Misc\Fog.I3D` (`Class.Def:2045`) | ambient | (e) | Fog overlay. | `REGISTER_3DANIMATOR("Fog", …)` `src/effect_old.cpp:6619`; anim `:6621-6701` |
| L02 | `THaloEffect` | FB+LS | `Magic\Halo.I3D` (`Class.Def:2066`) | ambient (light glow halo) | (e) | Soft glow halo / corona around light sources. | `DEFINE_BUILDER("Halo", …)` `src/effect_old.cpp:10568`; anim `:10592-10632` |
| X01 | `TFlyEffect` | PE | `Misc\Flies.I3D` (`Class.Def:2044`) | ambient (corpses) | (c) | Swarming flies particles. | `DEFINE_BUILDER("Flies", …)` `src/effect_old.cpp:6511`; anim `:6525-6587` |
| X02 | `TAmbSoundEffect` | - | `Misc\Speaker.I3D` (`Class.Def:2092`) | ambient | — | **No draw** — positional ambient-sound emitter. | `DEFINE_BUILDER("Speaker", …)` `src/effect_old.cpp:13629`; anim `:13746-13757` |

---

## 8. Weather / large-area composites (`CX` — needs careful decomposition)

> **These are the (g) complex composites.** Each drives an embedded
> `TStormAnimator`/particle-system + one or more `.I3D` meshes + lights together.
> Forensics agents: do **not** treat these as a single sprite — decompose into
> their sub-systems (mesh core, particle layer, impact layer, light). Several are
> Ghidra-merged into shared `cls_*` files and need a clean split.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| W01 | `TMeteorStormEffect` | IM+PE+LS+SR | `Magic\Meteor.I3D` (`Class.Def:2042`) | spell-cast (`spell.def:264,277`) | (g) | Falling meteors (8×2 frame grid) + 4×4 impact grid, gravity 0.37, via `TStormAnimator`. | `DEFINE_BUILDER("MeteorStorm", …)` `src/effect_old.cpp:6353`; storm params `:6371-6447` |
| W02 | `TTornadoEffect` | IM+PE+SR | `Magic\Tornado.i3d` (`Class.Def:2058`) | spell-cast (`"Funnel"`, `spell.def:477`) | (c)(g) | Tornado funnel mesh + swirling particles. Merged w/ Sandswirl+Quicksand (`cls_0x5ab460`). | `DEFINE_BUILDER("Tornado", …)` `src/effect_old.cpp:9710`; anim `:9818-10092` |
| W03 | `TVortexEffect` | PE+SR | `Magic\Vortex.I3D` (`Class.Def:2043`) | scripted / sub-effect (no direct `"Vortex"` spell VARIANT found; `VortexM` casts `"Teleporter"`, `spell.def:1471`) | (c)(g) | Swirling vortex of particles + strips (large `Animate` body). | `DEFINE_BUILDER("Vortex", …)` `src/effect_old.cpp:12845`; anim `:12860-13483` |
| W04 | `TSandswirlEffect` | PE | `Magic\Sandswirl.i3d` (`Class.Def:2064`) | spell-cast | (c) | Swirling sand particles. | `DEFINE_BUILDER("Sandswirl", …)` `src/effect_old.cpp:9448`; anim `:9476-9644` |
| W05 | `TQuicksandEffect` | PE | `Magic\Quicksand.i3d` (`Class.Def:2063`) | spell-cast (`"Quicksand"`/Swamp, `spell.def:306,326`) | (c) | Quicksand sink swirl. | `DEFINE_BUILDER("Quicksand", …)` `src/effect_old.cpp:9001`; anim `:9137-9265` (also `:12542-12737`) |
| W07 | `TStormAnimator` | CX | (storm infra) | — | (c)(g) | Shared storm/particle driver used by meteor/tornado/sand/quicksand. | class `src/effectcomp.h:73`; recon `recon/mappings/TStormAnimator_cls_0x5ab460_candidate.yaml` |
| —   | `TCataclysmEffect` | CX | `Magic\Cataclysm.I3D` (`Class.Def`) | spell-cast (`"Cataclysm"`, `spell.def`) | (g) | Cataclysm area devastation. | `DEFINE_BUILDER("Cataclysm", …)` `src/effect2.cpp:2970`; anim `:2974-3002` |
| —   | `TFunnelEffect` | CX | `Magic\Funnel.I3D` (`Class.Def`) | spell-cast (Tornado, `spell.def:477`) | (c)(g) | Tornado-funnel driver (note: Tornado spell casts `"Funnel"`). Holds sin/cos angle state. | `DEFINE_BUILDER("Funnel", …)` `src/effect2.cpp:2106`; anim `:2110-2122` |
| —   | `TSetVortexEffect` | CX | `Magic\SetVortex.I3D` (`Class.Def:2084`) | scripted | (c)(g) | Placed/static vortex variant. | `DEFINE_BUILDER("SetVortex", …)` `src/effect_old.cpp:5302`; anim `:5366-5404` |
| —   | `TSmokeEffectAnimator` | PE | `Misc\smoke.I3D` (`Class.Def:2041`) | ambient / sub-effect | (b) | Rising smoke particles. | `REGISTER_3DANIMATOR("Smoke", …)` `src/effect_old.cpp:6243`; anim `:6245-6299` |

---

## 9. Glow / light / overlay family

**Shared signature:** additive glow, often a flat world-oriented disc/quad (e),
sometimes UV-scrolled (f); pure visual, no damage.

| INV | Class | Draws | Asset (`Class.Def`) | Trigger | Archetype | Behavior | Cite |
|-----|-------|-------|---------------------|---------|-----------|----------|------|
| L01 | `TLightSource` | LS | (light data) | ambient | — | Dynamic point-light coupling (no sprite). | recon `cls_0x5427f0` (lightsource.cpp); INVENTORY L01 |
| X03 | `TFlareAnimator` | FB+LS | `Misc\Flare.I3D` (`Class.Def:2020`) | ambient (lens flare) | (e) | Iris/lens flare glow. | `REGISTER_3DANIMATOR("Flare", …)` `src/effect_old.cpp:515`; anim `:517-628`; recon `recon/mappings/TFlareAnimator_cls_0x5a9f2c_candidate.yaml` |
| —   | `TIrisFlareAnimator` | FB+LS | `Misc\IrisFlare.I3D` (`Class.Def:2036`) | ambient / teleport | (e) | Iris-flare (teleport portal glow). | `REGISTER_3DANIMATOR("IrisFlare", …)` `src/effect_old.cpp:5060`; anim `:5062-5132` |
| X10 | `TSymGlowAnimator` | FB+LS | `Misc\SymGlow.I3D` (`Class.Def:2032`) | scripted (rune sigil) | (e)(f) | Glowing sigil; **UV-scrolls `tu` by `u`/frame** + scale pulse ×1.4. | `REGISTER_3DANIMATOR("SymGlow", …)` `src/effect_old.cpp:4584`; UV scroll `:4651-4656`; recon `recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml` |
| X09 | `TShieldAnimator` | VO+IM | `Magic\Shield.I3D` / `Shield2.I3D` (`Class.Def:2028-2029`) | spell-cast | (e)(f) | Shield dome around character. | `REGISTER_3DANIMATOR("SHIELD", …)` `src/effect_old.cpp:4327`; anim `:4336-4364` |
| L03 | `TBallAnimator` | IM+LS | (energy ball) | **vestigial** | (f) | Energy-ball mesh — declared, not registered. | decl `src/effect.h:603` |
| X04 | `TFountainAnimator` | PE | (font sparkle base) | sub-effect | (b) | Particle-fountain base (rising sparkles). | `src/effect_old.cpp:3797`; anim `:3797-3861` |
| X05–X08 | `TCyan/Red/Green/BlueFountainAnimator` | PE | `Misc\Sparkle.I3D` (`Class.Def:2023-2026`) | sub-effect | (b) | Color-tinted fountain variants (CYANFONT/REDFONT/GREENFONT/BLUEFONT). | `REGISTER_3DANIMATOR("CYANFONT"…)` `src/effect_old.cpp:3909-3927`; classes `src/effect.h:799-844` |
| —   | `TSparkAnimator` | PE | `Misc\Sparks.I3D` (`Class.Def:2035`) | sub-effect | (a) | Generic spark burst (multi-anim via `TParticle3DAnimator`). | `REGISTER_MULTI_3DANIMATOR_("sparks", …)` `src/effect_old.cpp:4688` |
| —   | `TBarrierAnimator` (EW/NS) | IM+VO | `Misc\BarrierEW.I3D` / `BarrierNS.I3D` (`Class.Def:2037-2038`) | scripted (wall barriers) | (e)(f) | Energy barrier wall; **UV-scrolls `tu`+`tv` by 0.01/frame**. | `REGISTER_MULTI_3DANIMATOR_("BarrierEW/NS", …)` `src/effect_old.cpp:5939-5940`; UV scroll `:6177-6178` |

---

## 10. Infrastructure / base / particle systems (`-` or infra)

| INV | Class | Role | Cite |
|-----|-------|------|------|
| I01 | `TEffect` (base) | Base for all `TEffect`-derived effects; ctor takes `TCharacter*`; `Pulse()` lifetime. | `DEFINE_BUILDER("effect", TEffect)` `src/effect_old.cpp:415`; recon `recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml` |
| I02 | `TParticleSystem` | PE backbone (emit/integrate/reap). | class `src/effectcomp.h:329` |
| I03 | `TParticle3DAnimator` | PE animator base (used by sparks). | INVENTORY I03 |
| I04 | `TSubParticleAnimator` | Sub-particle spawning. | class `src/effectcomp.h:155` |
| B03 | `TBloodSystem` | Blood-specialized particle system. | class `src/effectcomp.h:377` |
| W07 | `TStormAnimator` | Storm/particle driver (composites). | class `src/effectcomp.h:73` |
| —   | `TSubParticleAnimator`/`SSubParticleParams` | sub-emitter params (fireball trail uses it). | `src/missileeffect.cpp:573` |

---

## 11. Discrepancies vs `docs/vfx/INVENTORY.md`

Verified each INVENTORY row against the actual builder/animator registrations and
class declarations in source. Findings:

### 11.1 In source but MISSING / under-rowed in INVENTORY
These effect classes self-register a builder but have **no dedicated INVENTORY
row** (they live entirely in `src/effect2.cpp` or were folded into a sibling row):

| Class | Name | Source |
|-------|------|--------|
| `TParaEffect` | "Paralize1" | `src/effect2.cpp:86` |
| `TManaDrainEffect` | "ManaDrain" | `src/effect2.cpp:738` |
| `TShriekEffect` | "Shriek" | `src/effect2.cpp:1184` |
| `TPukeEffect` | "Puke" | `src/effect2.cpp:1383` |
| `TFunnelEffect` | "Funnel" (cast by Tornado spell) | `src/effect2.cpp:2106` |
| `TInvisibleEffect` | "Invisible" | `src/effect2.cpp:2346` |
| `TRestEffect` | "Rest" | `src/effect2.cpp:2735` |
| `TCataclysmEffect` | "Cataclysm" | `src/effect2.cpp:2970` |
| `TIceStormEffect` | "IceStorm" | `src/effect2.cpp:3500` |
| `TCreateFoodEffect` | "CreateFood" | `src/effect_old.cpp:990` |
| `TCureEffect` | "Cure" | `src/effect_old.cpp:1434` |
| `TFireFlashEffect` | "FireFlash" | `src/effect_old.cpp:1988` |
| `TFireWindEffect` | "FireWind" | `src/effect_old.cpp:2516` |
| `TFireConeEffect` | "FIRECONE" | `src/effect_old.cpp:7255` |
| `TBurnEffect` | "BURN" | `src/effect_old.cpp:3214` |
| `TSetVortexEffect` | "SetVortex" | `src/effect_old.cpp:5302` |
| `TBarrierAnimator` (EW/NS) | "BarrierEW"/"BarrierNS" | `src/effect_old.cpp:5939-5940` |
| `TIrisFlareAnimator` | "IrisFlare" | `src/effect_old.cpp:5060` |
| `TMistFogAnimator` | "MistFog" | `src/effect_old.cpp:11558` |
| `TFountainAnimator` | (base) | `src/effect_old.cpp:3797` |
| `TSparkAnimator` | "sparks" | `src/effect_old.cpp:4688` |
| `TSmokeEffectAnimator` | "Smoke" | `src/effect_old.cpp:6243` |

> INVENTORY's M04 (TBurnEffect), M03 (THealEffect), M01 (TAuraEffect) etc. *are*
> rowed; the table above is only the classes with **no** INVENTORY id at all. The
> `effect2.cpp` cluster is the biggest gap — INVENTORY's Magic-family section
> stops at M09 and never enumerates Para/ManaDrain/Shriek/Puke/Funnel/Invisible/
> Rest/Cataclysm/IceStorm.

### 11.2 Naming surprises (registry name ≠ class name)
- **`TReviveEffect` is registered as `"Ribbon"`** (`DEFINE_BUILDER("Ribbon",
  TReviveEffect)`, `src/effect_old.cpp:3967`) — not a "ribbon" effect. The visual
  ribbon strip is `TRibbonAnimator` (`REGISTER_3DANIMATOR("RIBBON", …)`,
  `:4069`). INVENTORY M-rows don't list TReviveEffect.
- **`TLightningAnimator` is registered as `"LightStrip"`**, paired with
  `TStripEffect` (not a class named "Lightning"); `src/stripeffect.cpp:470`.
- **Tornado spell casts `"Funnel"`**, not `"Tornado"` (`spell.def:477` → effect
  name `Funnel`). INVENTORY W02 (`TTornadoEffect`) and the `TFunnelEffect`
  in effect2.cpp are *both* live and distinct.

### 11.3 INVENTORY rows that are VESTIGIAL in source (declared, never registered)
Confirmed: declared in `effect.h` with **no** `DEFINE_BUILDER`/`REGISTER_3DANIMATOR`:

| INV | Class | Status | Cite |
|-----|-------|--------|------|
| F03 | `TFireEffect` | declared only (note `src/effect.cpp:3056`) | `src/effect.h:380` |
| F04 | `TFireAnimator` | declared only | `src/effect.h:634` |
| I20 | `TIceEffect` | declared only (no `"Ice"` builder) | `src/effect.h:463` |
| —   | `TIceAnimator` | declared only | `src/effect.h:665` |
| L03 | `TBallAnimator` | declared only | `src/effect.h:603` |

### 11.4 INVENTORY claim that source contradicts
- **I22 `TIcedEffect`**: INVENTORY (pass-2) sets recon to "(no class)" and calls
  "Iced" a status tag with no standalone class. **Source disagrees:** there *is*
  a `TIcedEffect`/`TIcedAnimator` pair registered as `"Iced"`
  (`DEFINE_BUILDER("Iced", TIcedEffect)`, `src/effect_old.cpp:8723`; anim
  `:8749-8900`; class `src/effect.h:1549`). The status-tag observation may be true
  at the retail-decomp level, but the pre-release source has a real class. Flag
  for reconciliation.
- **X13 `TTest3DAnimator`** (INVENTORY "not-in-1999"): no registration found in
  the searched source files — consistent with "test/unused". Not cataloged above.

### 11.5 Not separately cataloged (covered by their `*Effect` row)
Per the universal Effect⇄Animator pairing (§0.5), the matching animators
(`TFlameAnimator`, `THaloAnimator`, `TDripAnimator`, etc.) are listed where
load-bearing but not given their own family rows — they always travel with their
Effect. INVENTORY makes the same simplification (`INVENTORY.md:254`).

---

## 12. Family sister-checks (quick sanity reference)

When verifying a single effect's port, compare against its siblings:

| Family | All members share | Red flag (port broken) |
|--------|-------------------|------------------------|
| Fire | additive ONE/ONE, hot saturated orange/yellow, black-keyed tex | grey/pale flame, dark fringe (chroma miss) |
| Ice/cold | cool blue/cyan/white, "Iced" posts a status | warm tint, missing freeze beam |
| Magic | cool palette (blue/cyan/gold), additive glow, many VO | washed color, opaque where it should glow |
| Lightning/strip (SR) | generated ribbon geometry, **U-axis UV scroll** | static (non-flowing) strip = scroll not wired |
| Missile | flying `.I3D` + trail + impact sub-effect, `TMissileEffect` base | no trail, no impact spawn |
| Blood | dark red, **alpha** (not additive), gravity-down | additive (too bright), no fade |
| Water/env | subtle alpha or VO, mostly **ambient** placement | over-bright, treated as cast effect |
| Composites (CX) | embedded `TStormAnimator`/particle-sys + mesh + light | rendered as one sprite (decomposition missing) |

---

## 13. Counts

- Effect/animator classes cataloged with source cites: **~70 distinct classes**
  across 12 families (106 total `DEFINE_BUILDER`/`REGISTER_3DANIMATOR`/`REGISTER_MULTI`
  registration sites counted in `src/effect_old.cpp`, `src/effect2.cpp`,
  `src/stripeffect.cpp`, `src/missileeffect.cpp`).
- `CLASS "EFFECT"` registry entries (asset bindings): **~80 names**,
  `legacy/Class.Def:2014-2092` (some names alias the same file).
- INVENTORY discrepancies surfaced: **22 unrowed `effect2`/`effect_old` classes**
  (§11.1), **3 naming surprises** (§11.2), **5 confirmed-vestigial** (§11.3),
  **1 source-vs-INVENTORY conflict** (I22, §11.4).
