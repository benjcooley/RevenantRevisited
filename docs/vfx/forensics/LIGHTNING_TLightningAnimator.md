# LIGHTNING TLightningAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | LIGHTNING (covers S01 `TStripEffect` shell + S04 `TLightningAnimator` mechanism + S06 `TStripAnimator` strip-geometry helper). The composite is **registered as `"LightStrip"`** — *not* `"Lightning"`. The `"Lightning"` spell (`spell.def:675`) names its EFFECT_NAME `"lightstrip"` (`spell.def:688,694,700`). |
| **Class(es)** | `TStripEffect : TEffect` (the spawnable object, a thin state-machine shell) + `TLightningAnimator : T3DAnimator` (the visual mechanism — the composite that owns the strip, glow, sparks). Drives a single embedded `TStripAnimator` (strip-geometry helper) + two embedded `TSubParticleAnimator` instances (`spark`, `impact_spark`). |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Retail fidelity** | **snapshot-only (unverified).** The retail Ghidra extraction has NO decompiled `TLightningAnimator` body — only the `"LightStrip"` string XREFs from two builder/registrar functions at `0x5222f0` / `0x522720` (`recon/classes/_data.txt:111993-111994,56478-56480`) and no readable Initialize/SetupObjects/Animate/Render. The `cls_0x5b0a28` candidate per `recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml` is actually the **`"LightningStorm"`** registrar (separate effect, `Class.Def:2091`), not LightStrip. The shipped `Magic\NewLightStrip.I3D` asset is **NOT identical** to the snapshot — retail 50,643 B vs snapshot 18,680 B; retail has 3 named sub-objects (`start`, `sparks`, `end`) vs the snapshot's 4 nameless development meshes (`striplaunch`, `stripfly`, `stripexplode`, empty 4th). The companion `Magic\Lightning Spark.I3D` asset IS byte-identical. The spell.def trigger wiring + the `"LightStrip"` builder name are confirmed; the constants and per-sub-object geometry must be visually matched against in-game footage. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-29 |
| **Family** | lightning (electric/projectile spell) |
| **Draws** | **composite** — one connected quad strip (the bolt) + two billboard glow halves (the rotating glow at the caster hand) + two TSubParticleAnimator sub-emitter sprays (cast-hand sparks + impact sparks). |
| **Archetype(s)** | (d) trail/beam (the strip) + (E) particle emitter (two TSubParticleAnimator sprays) + (f) I3D-mesh + UV-scroll (the rotating glow object + the `ScrollTexture(-0.1)` on the strip) + (g) custom logic (multi-stage state machine LAUNCH→FLY→EXPLODE; per-vertex jitter + cubic spline smoothing; ground-walk raycast to find the bolt end). NO associated dynamic light **emitted by this effect** (the spell.def `LIGHT` block, `spell.def:683`, may post a separate scene light — see §9). |

---

## 1. Summary

`"LightStrip"` is the **Lightning spell bolt** (Locke's "Electric Bolt" / "Priest
Bolt" / "HighPriest Bolt", `data/Resources/spell.def:675-705`). On cast, an
animated lightning ribbon shoots out of the caster's left hand
(`"lhand"`, `src/stripeffect.cpp:486`), forward along the caster's facing,
forking and crackling with per-vertex random jitter that re-rolls every sim
tick, while a two-half rotating "glow disc" pulses at the hand-point and two
spark sub-emitters spray (`spark` = the cast-hand fountain, `impact_spark` = the
fixed-end impact spray, fired only after the bolt has fully extended). The
strip texture continuously U-scrolls at `-0.1`/tick to give a flowing-energy
read. The bolt's length is **set by a ground-walking raycast** in
`SetupObjects` — it advances a probe in 10-wu steps along the caster's facing
direction until it either hits a target character (within 16 wu) or the
floor-walk-height check fails, then locks the endpoint in `end_p`
(`stripeffect.cpp:534-633`). The effect lives ~`STRIP_FLY_DURATION=20`
sim-ticks at full visibility, then enters `STRIP_EXPLODE`, deletes points off
the tail at `ADD_FACTOR=2`/tick and pinches the glow scale ×0.8/tick until
both the strip and the spark systems empty, at which point the effect
self-kills via `KillThisEffect()` (`stripeffect.cpp:818-819`). Damage is
applied **once**, on the first `STRIP_EXPLODE` tick, to the target character
the raycast found (`stripeffect.cpp:706-716`).

> **Naming surprise — registry name is `LightStrip`, NOT `Lightning`.** The
> spell is called `"Lightning"` in `spell.def`, but its EFFECT_NAME is the
> lower-case string `"lightstrip"` (`spell.def:688`), which `Class.Def:2034`
> registers as `"LightStrip" "Magic\NewLightStrip.I3D" 0x10da54d0`, and which
> both `DEFINE_BUILDER("LightStrip", TStripEffect)` (`stripeffect.cpp:392`)
> and `REGISTER_3DANIMATOR("LightStrip", TLightningAnimator)`
> (`stripeffect.cpp:470`) bind in code. The animator class **name** is
> `TLightningAnimator`; the dispatched **string** is `"LightStrip"`.

> **Do not confuse with sister effects.** There are FIVE lightning-flavoured
> registrations in `Class.Def`, of which this doc covers exactly one:
> - **`"LightStrip"` `Magic\NewLightStrip.I3D` (Class.Def:2034) — this doc.**
> - `"Lightning"` `Misc\lightning.I2D` (`:2016`) — a 2D bolt sprite (Locke's
>   lightning-bow equip), separate from the spell.
> - `"Lightning2"` `Magic\Lightning.I3D` (`:2022`) — the abandoned
>   `LIGHTNING2` animator in `effect_old.cpp:3690-3784` (commented out — DEAD).
> - `"LightningStorm"` `Magic\Lightning2.I3D` (`:2091`) — separate area-storm
>   effect (`TStormAnimator`, see knowledge `04_EFFECT_CATALOG.md:164`).
> - `"WindStrip"` `Magic\wind.I3D` (`:2065`) — sister of LightStrip; same
>   `TStripAnimator` strip geometry, different visual identity (the wind
>   tornado, S02/S03). Documented separately.

---

## 2. Sources & evidence

- **Retail decomp:** **NONE for `TLightningAnimator`.** No `recon/classes/`
  body exists for the LightStrip builder (`0x5222f0`) or animator (`0x522720`);
  Ghidra produced only the function-pointer table entries
  (`recon/classes/_data.txt:56478-56480 → LAB_005222f0, LAB_00522720`) and
  the string XREF (`:111993-111994`). The two readable mapping artifacts that
  carry the `TLightningAnimator` name are **mis-targeted**:
  - `recon/mappings/TLightningAnimator_cls_0x5a47f0_map.yaml` HIGH-confidence
    score 93/100 — this mapping was **demoted** in pass-2 because `cls_0x5a47f0`
    is now the `TEffect` base itself
    (`recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml` per the INVENTORY
    `I01` row).
  - `recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml` MEDIUM-HIGH
    — this class actually hosts the **`"LightningStorm"`** registry XREF
    (`recon/classes/_data.txt:107956-107957` at `0x503520`), the `ENERGYSPRAY`
    string, and the `d:\revenant\Effect3.cpp` assert — i.e. it is the
    **storm/composite** animator, NOT `TLightningAnimator`. The candidate
    yaml's own "Open question" acknowledges this (`...candidate.yaml:33-42`).
  - `recon/classes_readable/TLightningAnimator.{h,cpp}` is a **byproduct of
    the demoted `cls_0x5a47f0` mapping** and decompiles a generic
    netbuffer/serializer (the function bodies reference
    `s_Netpack__Largest_Size__005e65d4`, `recon/classes_readable/TLightningAnimator.cpp:781`)
    — this is NOT `TLightningAnimator`; ignore it entirely for this effect.
- **Pre-release (snapshot — the only authoritative source for the mechanism):**
  - `TStripEffect`: declarations + class `legacy/snapshot src/stripeffect.h:42-66`
    (preserved via initial-commit `git show 4c34258:src/stripeffect.h`);
    builder `DEFINE_BUILDER("LightStrip", TStripEffect)` `src/stripeffect.cpp:392`
    (the `#if 0` block at `:381-1420` preserves the verbatim pre-release D3D
    body); bodies `Initialize` `:395-404`, `Pulse` `:406-457`, `OffScreen`
    `:459-463`.
  - `TLightningAnimator`: class `legacy/snapshot src/stripeffect.h:79-120`
    (`git show 4c34258:src/stripeffect.h`); registrar `REGISTER_3DANIMATOR(
    "LightStrip", TLightningAnimator)` `src/stripeffect.cpp:470`; bodies
    `Initialize` `:472-532`, `SetupObjects` `:534-662`, `Animate` `:664-820`,
    `Render` `:822-915`, `RefreshZBuffer` `:917-973`.
  - **`TStripAnimator`** (S06 — the strip geometry helper): class
    `src/effectcomp.h:248-300`; constructor `src/effectcomp.cpp:776-850`;
    `SetTextureRange` `:852-861`; `ScrollTexture` `:863-871`; `GenerateStrip`
    `:873-920`; `AddPoint` `:922-947`; `DelEndPoint` `:950-965`; `SetWidth`
    `:1009-1012`.
  - **`TSubParticleAnimator`** (the spark sub-emitter — used twice here):
    struct `SSubParticleParams` `src/effectcomp.h:109-136`; struct `SParticle`
    `:138-151`; class `:153-191`; constants `SUBPARTICLE_MAX_PARTICLE = 20`
    `:107`; bodies `Set/Get` `src/effectcomp.cpp:335-345`, `GetCount` `:347-359`,
    `Animate` `:361-403`, `Init` `:405-453`, `Create` `:455-524`, `Render`
    `:526-570`, `RefreshZBuffer` `:572-606`.
  - **Blend helpers:** `SaveBlendState`/`RestoreBlendState`/`SetBlendState`/
    `SetAddBlendState` `src/effect_old.cpp:181-244`.
- **Trigger (spell.def):** `SPELL "Lightning"` `data/Resources/spell.def:675-705`,
  three variants all EFFECT_NAME `"lightstrip"` (`:688,694,700`), all `ATTACH
  "lhand"` (`:691,697,703`); `DAMAGETYPE DT_BURN` (`:680`); `LIGHT COLOR
  150,150,255 INT 255 MULT 20` (`:683`); `ANIMATION "invoke3"` (`:681`).
- **Asset (Class.Def):** `"LightStrip" "Magic\NewLightStrip.I3D" 0x10da54d0`
  `legacy/Class.Def:2034`. Companion asset (not explicitly registered for the
  bolt but **the same `Magic\Lightning Spark.I3D` may be loaded by the spark
  sub-objects**; see §4): `legacy/Imagery/Magic/Lightning Spark.I3D`
  (9,092 B, snapshot ≡ retail).
- **Sister effects consulted:**
  - `TWindStripEffect` / `TWindStripAnimator` (S02/S03,
    `src/stripeffect.cpp:979-1418`) — same `TStripAnimator` strip helper +
    same `SetBlendState()` Alpha pattern in its base body, but switches to
    `SetAddBlendState()` for the halo overlay (`:1333`). Corroborates the
    Alpha base / Additive-overlay split as a strip-family rendering pattern.
  - `TFireBallEffect` (F07,
    `src/missileeffect.cpp:573-580` — trail) — also drives a `TSubParticleAnimator`
    for trailing photon sparks; corroborates the SUBPARTICLE_MAX_PARTICLE=20
    cap and the `Animate()`/`Render()` cadence.
- **Source-of-truth ranking:** the **snapshot is authoritative for the
  mechanism** (the only readable source). The retail decomp contributes ONLY
  the registration-name confirmation + the strict negative that the shipped
  asset bytes differ from snapshot (§2.1). For everything else — constants,
  state machine, blend, render geometry — the snapshot is the best available
  evidence, **flagged as snapshot-only** per the protocol.

### 2.1 Retail-vs-snapshot reconciliation (verdict: snapshot-only)

The four cross-checks called for by the protocol §2.1:

**1. Constant grep — NOT corroborated.** The snapshot's lightning-specific
immediates (`STRIP_SPEED=16`, `STRIP_RANGE=2`, `MAX_MAXPOINTS=100`,
`SMOOTH_SIZE=4`, `ADD_FACTOR=2`, `LIGHTNING_SCALE_N=2`, `LIGHTNING_SCALE_D=1`,
`glow_scale=3.4f` (=`0x4059999A`), the `glow_scale *= 0.8f` decay rate
(=`0x3F4CCCCD`), the strip texture U-scroll rate `-0.1f` (=`0xBDCCCCCD`), the
`rotdegree += 12 / morrotdegree += 16` per-tick advances, the `random(-13,
13)` jitter range, the spark params `chance=25`, `gravity=0.18f`,
`flicker_size=2.5f`, `min_life=25`/`max_life=35`, `scale=2.0f`,
`scale_dec=0.9f`, `velocity_spread.x/y=2.0f`, `velocity_spread.z=1.0f`) do
**not** appear in any decompiled `recon/classes/` body. Because Ghidra
produced no readable LightStrip animator at `0x5222f0`/`0x522720` (§2), there
is no decomp to grep against — only the empty XREF-only entries. **None of
the constants in §3 are retail-corroborated.**

**2. Asset identity — MIXED (one identical, one DIFFERENT).**
- `Magic\NewLightStrip.I3D` (the main bolt asset, `Class.Def:2034`):
  - Snapshot: `legacy/Imagery/Magic/NewLightStrip.I3D` — **18,680 B**, MD5
    `545cba18b9cab01355af3cf15a20b1b6`, **4 sub-objects** with development
    names `striplaunch` / `stripfly` / `stripexplode` / *(empty 4th slot)*
    (header `nobjects=0x04` at offset `0x18`, names scanned at offsets
    `0x60,0xB0,0x100,0x130`).
  - Retail (`data/imagery.rvi` member `Imagery/Magic/newlightstrip.i3d`):
    **50,643 B**, MD5 `53edb2c688636f5e5a5a66b01528376c`, **3 sub-objects**
    with shipped names `start` / `sparks` / `end` (and internal `flare` +
    rectangle## mesh nodes), header `nobjects=0x03` at offset `0x18`. The
    bytes are **NOT identical** (`cmp` reports "differ: char 9"). The retail
    asset is **2.7× larger** with completely different sub-object naming/count.
  - **Conclusion: the LightStrip visual asset was reworked before ship.**
    The snapshot's `striplaunch/stripfly/stripexplode` naming maps to the
    state-machine phases the code's `GetObject(0/1/2)` accesses
    (`stripeffect.cpp:540-661`); the retail's `start/sparks/end` naming maps
    differently (likely 0=glow-start, 1=spark-sprite, 2=glow-end). The
    `SetupObjects` body still does `NewObject(0)` for the strip carrier,
    `NewObject(1)` for the glow, `NewObject(2)` for the spark
    (`stripeffect.cpp:540,655,660`) — so the **slot meaning shifted** between
    snapshot and ship. Reconstruction must work from the retail asset's
    sub-object names + content; the snapshot's `striplaunch/stripfly/
    stripexplode` are dev placeholders that did not ship.
- `Magic\Lightning Spark.I3D` (the spark sprite — sub-objects 2's photon
  imagery, candidate for `sparks` carrier in the retail asset):
  - Snapshot: 9,092 B, MD5 `71831c1115b2e2568d7b2f23f3560893`.
  - Retail (`data/imagery.rvi` member `Imagery/Magic/lightning spark.i3d`):
    9,092 B, MD5 `71831c1115b2e2568d7b2f23f3560893` — **byte-identical.**
  - **Conclusion: the spark sprite did not change.** The
    `TSubParticleAnimator`'s photon sprite is intact between snapshot and ship.
    (But the spark animator reads its sub-object as `GetObject(2)` of the
    strip's *own* imagery, NOT this `Lightning Spark.I3D` file
    (`stripeffect.cpp:496-498`) — so this asset may be a separate sister
    sprite used elsewhere, or the retail rework relocated this content into
    the `sparks` sub-object of `newlightstrip.i3d`. Reconstruction must
    confirm by inspecting which sub-object the retail `Render` reads at slot 2.)

**3. Structure / vftable — NOT corroborated.** No `cls_*` mapping for
`TLightningAnimator` survives the pass-2 cleanup (the two existing yaml files
are mis-targeted, §2). Without a confirmed `cls_*` for the animator, vftable
method count and field offsets cannot be checked against the snapshot's class
declaration (`stripeffect.h:79-120`, 14 fields: `spark`, `impact_spark`,
`glow_scale`, `total_offset`, `end_p`, `damage_flag`, `firsttime`, `mystrip`,
`numpoints`, `maxpoints`, `showangle`, `rotdegree`, `morrotdegree`,
`maxsize`, `down`). Note `recon/mappings/TLightningAnimator_cls_0x5a47f0_map.yaml`
auto-mapped a 60-byte class with 13 fields to this name — a near-fit on
size/count — but that class is **`TEffect`** (the demoted mapping). So even
the apparent size/count agreement is coincidence.

**4. Registration + naming — CONFIRMED (but only the name).** The retail
binary holds two contiguous function-table entries at `005c5760`/`005c5764`
that point at `0x5222f0` and `0x522720` (`recon/classes/_data.txt:56478-56480`),
both of which reference the literal `"LightStrip"` string at `0x5e2d84`
(`:111993-111994`). This is the expected `DEFINE_BUILDER` (`stripeffect.cpp:392`)
+ `REGISTER_3DANIMATOR` (`:470`) pair. The snapshot's spell.def trigger
(`"lightstrip"` EFFECT_NAME for all three Lightning variants,
`spell.def:688,694,700`) survives unchanged in shipped `data/Resources/spell.def`
(the same file is shipped). So the **name-binding** between spell and effect
is retail-confirmed; the function bodies on the receiving end are not.

**Verdict — snapshot-only (unverified).** The registration name + spell
trigger are confirmed; the visual asset is **demonstrably different from
snapshot** (50,643 B vs 18,680 B, different sub-object names/count); the
mechanism (constants, state machine, render, blend) is **only** in the
snapshot, with NO retail body to verify against. The fidelity risk to
reconstruction is therefore concentrated in two places:

  1. **Per-sub-object geometry.** The retail asset's `start`/`sparks`/`end`
     naming implies a different sub-object layout than the snapshot's
     `striplaunch`/`stripfly`/`stripexplode`. Reconstruction must read the
     **retail asset** to identify which sub-object is the strip carrier
     (probably index 0 = `start` still, paired with the new `end`?), which
     is the glow (probably 0/2 = `start`/`end` together?), and which is the
     spark sprite (index 1 = `sparks`). The snapshot's "obj 0 = strip,
     obj 1 = glow, obj 2 = spark" slot meaning shifted.
  2. **All numeric constants.** Gravity 0.18, scale 2.0, lifetimes 25/35,
     glow_scale 3.4 + 0.8/tick decay, the `-0.1` U-scroll rate, the
     ±13 wu jitter range, the `STRIP_SPEED=16` raycast step, the
     `MAX_MAXPOINTS=100` cap. All snapshot-only. Reconstruction should be
     prepared to visually retune against in-game ground-truth (§12).

Per-constant, every value in §3 is marked **snapshot-only** in the
"confirmed?" column. None are retail-confirmed.

---

## 3. Constants

`STRIP_*` defines: `legacy/snapshot src/stripeffect.h:42-50` and `:79-86`.
`me`-fields below refer to `SSubParticleParams` (`effectcomp.h:109-136`)
filled in `TLightningAnimator::Initialize`. All values are pre-release
snapshot.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| STRIP_LAUNCH | 0 | enum | stripeffect.h:39-42 | snapshot-only |
| STRIP_FLY | 1 | enum | stripeffect.h:39-42 | snapshot-only |
| STRIP_EXPLODE | 2 | enum | stripeffect.h:39-42 | snapshot-only |
| **STRIP_SPEED** | **16** | wu/raycast-step (length of `vel` from `ConvertToVector(facing, 10, ...)`; see §13 for the 10-vs-16 caveat) | stripeffect.h:47 | snapshot-only |
| **STRIP_RANGE** | **2** | wu — overshoot bias added to `targetrange` on raycast hit (`pnt + 2`) | stripeffect.h:47, stripeffect.cpp:433 (in `#if 0`) / 4c34258:src/stripeffect.cpp:62 | snapshot-only |
| **STRIP_FLY duration init** | **20** | sim ticks of full visibility | stripeffect.cpp:398 (`#if 0`) / 4c34258:27 | snapshot-only |
| **STRIP_EXPLODE duration init** | **5** | sim ticks for tail shrink-out | stripeffect.cpp:447 (`#if 0`) / 4c34258:76 | snapshot-only |
| TStripEffect z-offset | **+50** | wu — spawn z bump (rises bolt to hand height) | stripeffect.cpp:402 (`#if 0`) | snapshot-only |
| Pulse-raycast step count | **16** iterations | count | stripeffect.cpp:425 (`#if 0`) | snapshot-only |
| Pulse-raycast vel length arg | **10** | wu/step (`ConvertToVector(facing, 10, vel)`) | stripeffect.cpp:423 (`#if 0`) | snapshot-only |
| **SMOOTH_SIZE** | **4** | count — anchor stride between major (jittered) points; intermediate 3 are spline-interpolated | 4c34258:src/stripeffect.h:73 | snapshot-only |
| **ADD_FACTOR** | **2** | count — anchors added per tick during FLY; anchors removed per tick during EXPLODE | 4c34258:src/stripeffect.h:74 | snapshot-only |
| **TARGET_POS** | **1** | flag — when 0 a dev path multiplies `maxpoints` by `SMOOTH_SIZE` (=4) for full-bolt demos. Live setting = 1 (no multiply) | 4c34258:src/stripeffect.h:75 | snapshot-only (dev flag) |
| **LIGHTNING_SCALE_N** | **2** | strip width numerator (×16/×10 below) | 4c34258:src/stripeffect.h:76 | snapshot-only |
| **LIGHTNING_SCALE_D** | **1** | strip width denominator | 4c34258:src/stripeffect.h:77 | snapshot-only |
| strip start (hilt) width | `(LIGHTNING_SCALE_N*16)/LIGHTNING_SCALE_D` = **32** | wu | stripeffect.cpp:642 (`#if 0`) | snapshot-only |
| strip end (tip) width | `(LIGHTNING_SCALE_N*10)/LIGHTNING_SCALE_D` = **20** | wu | stripeffect.cpp:642 (`#if 0`) | snapshot-only |
| **MAX_MAXPOINTS** | **100** | hard cap on raycast step count + final `min(100, maxpoints)` cap | 4c34258:src/stripeffect.h:78; stripeffect.cpp:628,634 (`#if 0`) | snapshot-only |
| SetupObjects raycast step | **10** | wu/step (`r = 10.0f` in the dx/dy build) | stripeffect.cpp:553 (`#if 0`) | snapshot-only |
| SetupObjects char-hit range | **16** | wu — target proximity threshold for `Distance(point, char_pos) > 16` skip | stripeffect.cpp:607 (`#if 0`) | snapshot-only |
| SetupObjects probe z-drop | **-100** | wu — `new_pos.z = (int)point.z - 100` for the target-search reference (`new_pos`) | stripeffect.cpp:582 (`#if 0`) | snapshot-only |
| SetupObjects end_p z-offset | **-20.0** | wu — final `end_p.z = point.z - 20.0` | stripeffect.cpp:632 (`#if 0`) | snapshot-only |
| initial numpoints | **2** | count — strip seeded with 2 anchors | stripeffect.cpp:549 (`#if 0`) | snapshot-only |
| strip anchor stride (y) | **-10** | wu/anchor along local Y (`p.y = i * -10`) | stripeffect.cpp:648,728 (`#if 0`) | snapshot-only |
| **strip U-scroll rate** | **-0.1** per tick | unit (texture U) | stripeffect.cpp:817 (`#if 0`) | snapshot-only |
| jitter px range | **random(-13, 13)** | wu (per axis x and z) on every SMOOTH_SIZE-th anchor | stripeffect.cpp:753-754 (`#if 0`) | snapshot-only |
| spline interp t=1 weight | **0.5** | fraction (`(next−last)*0.5/SMOOTH_SIZE`) | stripeffect.cpp:775-776 (`#if 0`) | snapshot-only |
| spline interp t=2 weight | **2.0** | fraction | stripeffect.cpp:780-781 (`#if 0`) | snapshot-only |
| spline interp t=3 weight | **3.5** | fraction | stripeffect.cpp:785-786 (`#if 0`) | snapshot-only |
| down-droop z gain | **-(i-1)*20/(maxpoints-1)** | wu — Z drop per anchor when `down=true` (linear droop toward tip) | stripeffect.cpp:803 (`#if 0`) | snapshot-only |
| rotdegree advance / tick | **+12 (% 360)** | deg | stripeffect.cpp:814 (`#if 0`) | snapshot-only |
| morrotdegree advance / tick | **+16 (% 360)** | deg | stripeffect.cpp:815 (`#if 0`) | snapshot-only |
| **glow_scale initial** | **3.4** | unit-scale | stripeffect.cpp:530 (`#if 0`) | snapshot-only |
| **glow_scale EXPLODE decay** | **×0.8 per tick** | fraction | stripeffect.cpp:700 (`#if 0`) | snapshot-only |
| glow_scale draw threshold | **0.005** | unit-scale (below = skip glow draw) | stripeffect.cpp:861 (`#if 0`) | snapshot-only |
| glow per-frame scale jitter | **+ random(0, 4/10)** ⇒ +(0..0.4) | wu | stripeffect.cpp:870 (`#if 0`) | snapshot-only |
| glow base orient X | **-30°** | deg (rotateX, both glow halves) | stripeffect.cpp:875,889 (`#if 0`) | snapshot-only |
| glow base orient Y | **+60°** | deg (rotateY, both glow halves) | stripeffect.cpp:876,890 (`#if 0`) | snapshot-only |
| glow rotZ half-A | `-rotdegree·TORADIAN` | rad | stripeffect.cpp:874 (`#if 0`) | snapshot-only |
| glow rotZ half-B | `+morrotdegree·TORADIAN` | rad | stripeffect.cpp:888 (`#if 0`) | snapshot-only |
| glow facing offset | `-(face·360/256)·TORADIAN` | rad — final rotZ that points the glow toward camera-relative caster facing | stripeffect.cpp:879,893 (`#if 0`) | snapshot-only |
| strip per-vert lit color | `D3DRGBA(1,1,1,1)` ⇒ **pure white, full alpha** | float 0..1 each | stripeffect.cpp:848 (`#if 0`) | snapshot-only |
| strip face-angle offset (showangle) | **0** | byte-angle (snapshot init, never modified) | stripeffect.cpp:542 (`#if 0`) | snapshot-only |
| GenerateStrip angle bias | **+224 (% 256)** | byte-angle — internal offset applied by `TStripAnimator::GenerateStrip` so the world-face maps into the 8-direction lookup | effectcomp.cpp:878 | snapshot-only |
| strip face dir lookup count | **8** | count — the `myx[]`/`myz[]` table is 8 entries at compile-time z-rotation `{-60,-60,0,60,60,60,0,-60}` deg | effectcomp.cpp:783 | snapshot-only |
| RefreshZBuffer effect patch | **80×40 px** | screen-px — primary clear around projected effect origin | stripeffect.cpp:929-932 (`#if 0`) | snapshot-only |
| RefreshZBuffer per-point patch | **20×20 px** | screen-px — repeat clear per anchor along the strip | stripeffect.cpp:942-958 (`#if 0`) | snapshot-only |
| SUBPARTICLE_MAX_PARTICLE | **20** | count — hard cap on a `TSubParticleAnimator`'s active particle count | effectcomp.h:107 | snapshot-only |
| spark.particles (initial) | **20** | count — `spark` active particle target | stripeffect.cpp:503 (`#if 0`) | snapshot-only |
| spark.chance | **25** | percent — `random(1,100) > chance ⇒ skip create` per tick (so on average 1 in 4 ticks adds a new particle when there's room) | stripeffect.cpp:504, effectcomp.cpp:458-459 | snapshot-only |
| spark.velocity_dir | **(0, 0, 0)** | sign-mask — all 3 axes mixed-sign velocity (see §6.1 Create rule) | stripeffect.cpp:505-507 (`#if 0`) | snapshot-only |
| spark.pos | **effect.pos + lhand.translation** | wu (world) — caster's left-hand world position | stripeffect.cpp:508-510 (`#if 0`) | snapshot-only |
| spark.pos_spread | **(0, 0, 0)** | wu — no position jitter | stripeffect.cpp:511 (`#if 0`) | snapshot-only |
| spark.scale | **(2.0, 2.0, 2.0)** | unit-scale | stripeffect.cpp:512 (`#if 0`) | snapshot-only |
| spark.scale_dec | **(0.90, 0.90, 0.90)** | per-tick multiplier on per-particle scale (geometric decay) | stripeffect.cpp:513 (`#if 0`) | snapshot-only |
| spark.scale_spread | **(0, 0, 0)** | unit-scale | stripeffect.cpp:514 (`#if 0`) | snapshot-only |
| spark.velocity | **(0, 0, 0)** | wu/tick — no bias | stripeffect.cpp:515 (`#if 0`) | snapshot-only |
| spark.velocity_spread | **(2.0, 2.0, 1.0)** | wu/tick — per-axis jitter ±2 xy, ±1 z | stripeffect.cpp:516-517 (`#if 0`) | snapshot-only |
| spark.gravity | **0.18** | wu/tick² (subtracted from velocity.z each tick) | stripeffect.cpp:518 (`#if 0`) | snapshot-only |
| spark.min_life | **25** | ticks | stripeffect.cpp:519 (`#if 0`) | snapshot-only |
| spark.max_life | **35** | ticks | stripeffect.cpp:520 (`#if 0`) | snapshot-only |
| spark.flicker | **true** | bool | stripeffect.cpp:521 (`#if 0`) | snapshot-only |
| spark.flicker_size | **2.5** | unit-scale (multiplies `scale` on the half of ticks flicker is on) | stripeffect.cpp:522 (`#if 0`) | snapshot-only |
| impact_spark.particles (initial) | **0** | count — disabled until strip fully extends | stripeffect.cpp:526 (`#if 0`) | snapshot-only |
| impact_spark.particles (when bolt landed) | **20** | count — switched on by `Animate` once `mystrip->curpoints == maxpoints` | stripeffect.cpp:676-677 (`#if 0`) | snapshot-only |
| impact_spark.pos | **end_p** (world) | wu — fixed at the bolt's far end | stripeffect.cpp:527 (`#if 0`) | snapshot-only |
| TSubParticleAnimator sub-object | **GetObject(2)** — the spark sub-object on the LightStrip imagery | — | stripeffect.cpp:496-498 (`#if 0`) | snapshot-only |
| TSubParticle render rot.Z | **-π/3** | rad | effectcomp.cpp:541 | snapshot-only |
| TSubParticle render rot.X | **-π/4** | rad | effectcomp.cpp:542 | snapshot-only |
| FIX_Z_VALUE | passes z through (`#define FIX_Z_VALUE(x) (x)`) | — | revdefs.h | snapshot-only |
| TORADIAN | π/180 | rad/deg | revdefs.h | snapshot-only |

> **`random(min,max)`** is inclusive on both ends (`rand()%(max-min+1)+min`,
> `src/revutils.cpp:1597-1612`).
> **Sim-tick cadence:** the original ran `Pulse()` + `Animate()` together at
> 24 Hz (NOMENCLATURE §6, knowledge `01_ARCHITECTURE.md:143-144`); all
> per-tick values above are per-24Hz-tick.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| LightStrip (snapshot) | `legacy/Imagery/Magic/NewLightStrip.I3D` | **18,680 B**, MD5 `545cba18b9cab01355af3cf15a20b1b6` | bolt + glow + spark **carrier** (the imagery the animator's `GetObject(0/1/2)` walks) — **STILL** (2D billboard-style) I3D with 4 sub-objects | registered `Class.Def:2034` under name `"LightStrip"` (hash `0x10da54d0`); bound via the `OBJCLASS_EFFECT` factory when the spell's `"lightstrip"` EFFECT_NAME spawns; sub-objects addressed by `T3DAnimator::GetObject(0..2)` |
| LightStrip (retail) | `data/imagery.rvi:Imagery/Magic/newlightstrip.i3d` | **50,643 B**, MD5 `53edb2c688636f5e5a5a66b01528376c` | **same role, different asset** — see §2.1 | same loader path |
| Lightning Spark | `legacy/Imagery/Magic/Lightning Spark.I3D` (= retail `data/imagery.rvi:Imagery/Magic/lightning spark.i3d`, **identical**) | 9,092 B, MD5 `71831c1115b2e2568d7b2f23f3560893` | spark sprite imagery — a companion still asset; NOT registered in `Class.Def` (no `"LightningSpark"` row) and **not directly loaded by `TLightningAnimator`** in the snapshot (snapshot reads spark sprite from `GetObject(2)` of the *strip's* imagery, `stripeffect.cpp:496-498`). Likely either (a) part of an unwired path or (b) injected via the retail asset's `sparks` sub-object (§13). | not loaded by this effect in the snapshot |

**Sub-objects — SNAPSHOT (4)**, read directly from the I3D object table by
header offset `0x18`=`0x04` and string scan:

| # | name | role (per the code that addresses it) | accessed by |
|---|------|---------------------------------------|-------------|
| 0 | `striplaunch` | **strip carrier** — `NewObject(0)` passes this to `new TStripAnimator(o, maxpoints)`, which then takes ownership of its verts/faces array and rebuilds them every tick as the quad strip (`stripeffect.cpp:540`, `effectcomp.cpp:776-849`). The sub-object's **authored texture** is the U-scrolled strip diffuse — a horizontal "lightning crackle" pattern texture-mapped along the bolt's length. | strip pass |
| 1 | `stripfly` | **glow** — `NewObject(1)`, drawn as the rotating two-half disc at the caster hand (`stripeffect.cpp:655-657`, `:862-902`). | glow pass (twice — see §7) |
| 2 | `stripexplode` | **spark sub-object** — `NewObject(2)`, passed to BOTH `spark.Init(this, o)` and `impact_spark.Init(this, o)` as the photon sprite that every spark particle draws (`stripeffect.cpp:496-498,660`). | both spark passes |
| 3 | *(empty / unused 4th slot)* | inert; not addressed by any `GetObject(n)` call in `TLightningAnimator` | none |

**Sub-objects — RETAIL (3),** read directly:

| # | name | role (inferred) |
|---|------|-----------------|
| 0 | `start` | likely the strip carrier (matches the snapshot's `NewObject(0)` strip slot), or alternatively half of a glow pair |
| 1 | `sparks` | likely the spark sub-object (the snapshot's `GetObject(2)` slot), now relocated to index 1. **Reconstruction must verify which index the retail render reads as the spark.** |
| 2 | `end` | the other end of the bolt — either the impact glow or the impact-spark anchor sub-object. (Retail also embeds internal mesh node names `flare`, `rectangle02..rectangle11` under these objects — these are the mesh-piece geometry, not separate animator sub-objects.) |

### Texture atlas decode

The snapshot's authored texture **on sub-object 0 (`striplaunch`)** is the
quad strip's **single-cell texture** (NOT a 2×N flipbook atlas, NOT a 2×2
sprite atlas). The `TStripAnimator::AddPoint` body writes per-vertex UVs as
`tu = 1`, `tv = 0` / `tv = 1` for each newly added strip rib
(`effectcomp.cpp:931-937`); `GenerateStrip` then sweeps `tu` across the
length of the strip per anchor via `tfreq = (pnt * ufrequency) + uoff` with
`ufrequency = (uend - ubeg) / maxpoints` and `uoff` advanced by
`ScrollTexture(-0.1f)` each tick (`effectcomp.cpp:856-872,914-915`). With
`SetTextureRange(0, 1)` (default; `effectcomp.cpp:797`), the **entire texture
U range is one full sweep along the bolt** — so the strip texture is read as
**one image stretched and scrolled along U over the full strip length**, with
**V locked to the full sheet (0..1)** across the strip's width. There is NO
atlas grid, NO per-cell selection — just one image sampled with U scrolling.

The texture cell decoded from the snapshot header bytes at offset `0x44` is
`64 wide × 80 high` (RGB565, 16-bit) — a single bright bolt-crackle sprite.
(`0x44: 40 00 50 00` = 64w×80h.)

The **glow** sub-object 1 (`stripfly`) similarly carries a single still
sprite; its UVs are never mutated by the animator (`Render` only writes
`matrix`/`scl`/`pos`, `stripeffect.cpp:869-898` — no `tu/tv` touch). So the
glow is a **single image quad** drawn twice with two rotated matrices for
the cross-rotating halves (§7).

The **spark** sub-object 2 (`stripexplode`) is the photon sprite for each
emitted spark particle; `TSubParticleAnimator::Render` likewise never mutates
UVs (`effectcomp.cpp:526-570`) — single image per spark.

**No `framehtexs[]` flipbook** is set up by any of the lightning code path;
no `OBJ3D_TEXFRAME` flag is written. All three sub-objects use **static
single-image textures** (NOMENCLATURE §6 / knowledge `02_ASSETS_IMAGERY.md`
§5.4 mechanism = UV-scroll on the strip only).

> **Retail note (§2.1):** the retail asset's `start/sparks/end` sub-objects
> almost certainly carry different texture content/dimensions. Decoding the
> retail texture for each sub-object (and its UV span) is a reconstruction
> task — the snapshot decode above describes the **dev** asset, not the
> shipped one.

**The effect loads real assets — do NOT substitute procedural strip / glow /
spark sprites.** The authored bolt-crackle on sub-object 0 is the visual
identity; the glow is an authored bright disc; the spark is the photon
sprite. (Knowledge `02_ASSETS_IMAGERY.md` §4 — most `.I3D` effect textures
are black-keyed; the loader's >20%-black auto-detect converts them to
premultiplied alpha for clean edge blending.)

---

## 5. Spawn & emit

- **Trigger semantics:** **fixed-duration** composite (~20-tick FLY +
  5-tick EXPLODE = ~25 ticks @24Hz ≈ ~1.04 s nominally), then self-kill.
  Internally **continuous** for the cast-side `spark` sub-emitter while
  bolt is alive, **one-shot-deferred** for the `impact_spark` sub-emitter
  (count switches from 0 to 20 the tick the bolt fully extends, then back
  to 0 when EXPLODE starts).
- **Count per trigger:** ONE bolt + ONE rotating glow + UP TO 20+20 = 40
  spark particles (cap `SUBPARTICLE_MAX_PARTICLE = 20` per sub-emitter,
  `effectcomp.h:107`).
- **Initial direction / distribution:** **directional, along the caster's
  facing in world XY.** The bolt's endpoint is found by a ground-walking
  raycast in `SetupObjects`:
  ```
  r = 10.0
  dx = r * sin(face/256 * 360 * TORADIAN)
  dy = -r * cos(face/256 * 360 * TORADIAN)
  point = effect.pos
  do:
      point.x += dx; point.y += dy
      zheight = MapPane.GetWalkHeight(point)
      // check each character within 16 wu of (point with z -100): if enemy hit, flag=true
      maxpoints++
  while (point.z > zheight AND not character-hit AND maxpoints < MAX_MAXPOINTS=100)
  end_p = (point.x, point.y, point.z - 20)
  maxpoints = min(100, maxpoints)
  ```
  So the bolt extends in a **straight horizontal line in caster.facing
  direction**, dropping 10 wu/step until either (a) the floor walk-height
  rises above the current point.z (hit a wall / obstacle), or (b) an enemy
  character is within 16 wu (hit a target), or (c) 100 step cap is reached.
- **Emit anchor convention:** the effect object's **world position** is the
  caster's pos at cast (via the spell's `ATTACH "lhand"` semantics, but the
  literal anchor used by `SetupObjects` is `((PTEffect)inst)->GetPos(effect)`
  — i.e. the spawned effect's position). At spawn, `TStripEffect::Initialize`
  **bumps z by +50** (`stripeffect.cpp:402`) so the bolt's first anchor is at
  hand/eye height. The **strip anchors live in object-local space**: anchor
  `[i]` = `(0, i * -10, 0)` in local (`stripeffect.cpp:646-650, 723-728`),
  so the strip extends along **local -Y** (which is the strip-helper's
  authored "down-the-bolt" axis); `TStripAnimator::GenerateStrip(angle)`
  later rotates the whole strip about Z by `(face + showangle + 224) % 256`
  byte-angle so it points in caster.facing direction (`effectcomp.cpp:873-919`).
  The **glow** is drawn at the (lhand-translation-relative) caster origin via
  `obj->pos` (`stripeffect.cpp:881-882, 895-896`). The **cast-hand sparks**
  spawn at `effect.x + hand_pos.x`, `effect.y + hand_pos.y`, `effect.z +
  hand_pos.z` (`stripeffect.cpp:508-510`) — i.e. the caster's left-hand
  world-space matrix translation (read from `GetObjectMatrix(GetObjectNum("lhand"))`,
  `:486`). The **impact sparks** spawn at `end_p` (`:527`) — the bolt's
  computed far-end world position.
- **Coordinate space:** strip anchors are local to the effect object
  (object origin = caster hand+50z) and rotated to world via the strip's own
  matrix in `GenerateStrip`. Spark particle positions are stored as **world**
  (the `params.pos = effect.pos + hand_pos.xyz` write is world; `pos_spread =
  (0,0,0)` so all particles spawn at exactly that world point; per-particle
  velocity is then integrated in world). Glow position is the sub-object's
  own `obj->pos` (object-local; the OBJ3D_MATRIX form applies the
  scale/rot/translate built up in the render body).
- **Spread / jitter:**
  - Strip: per-vertex anchor jitter `random(-13, 13)` wu on local-x and
    local-z at every SMOOTH_SIZE=4-th anchor; intermediate 3 anchors are
    spline-weighted from the two surrounding major-anchor jitter values
    (§6.3 weights 0.5 / 2.0 / 3.5 ÷ 4).
  - Cast sparks: per-particle velocity ±(2, 2, 1) wu/tick around (0,0,0)
    (mixed-sign since `velocity_dir.x/y/z = 0`); per-particle scale starts
    at 2.0 with no spread; lifetime 25..35 ticks; flicker doubles size to
    5.0 on the random ~half of ticks (`flicker_status = random(0,1)`).
  - Impact sparks: same params as cast sparks except `particles=0` until
    the bolt fully extends.
- **Spatial diagram (required):**

```
   wz (up)
   │                                ___
   │   GLOW (rotating)             /   \                  cast-end caster
   │   2 quads, X=-30,Y=+60       │ * * │ * SPARK ARC
   │   Z spins +12 / +16 /tick    │/   \│  fall under gravity 0.18
   │                              ▲      \              /
   │     ┌─────────────────────── strip ───────────────│
   │     │   bolt = TStripAnimator: numpoints anchors  │
   │     │   width hilt 32 → tip 20 wu, U-scrolls −0.1│
   │     │   per-vertex jitter ±13 every SMOOTH_SIZE-th│
   │     │   anchor, spline 0.5/2.0/3.5 between        │
   │     ▼  down-droop z=-(i-1)*20/(maxpoints-1)      ▼
   │  HAND                                          end_p
   │  origin = effect_pos.xyz + lhand.translation  (raycast endpoint, z-20)
   │  z = effect_pos.z + 50                                ▲
   │                                                       │ impact_spark
   └──────────────────────────────────── wx               cone (lit after
  ╱   bolt direction = ConvertToVector(face, 10) per step   bolt fully grown)
 ╱
wy

top-down: caster_face → ───────────── bolt direction (straight) ──────────→ end_p
```

---

## 6. Behavior & per-frame logic

The composite has three coupled state-machines (one in `TStripEffect::Pulse`,
one in `TLightningAnimator::Animate`, two in the embedded
`TSubParticleAnimator`s). All run at 24 Hz sim-tick.

### 6.1 Object-side state machine (`TStripEffect::Pulse`, `stripeffect.cpp:406-457`)

```
Pulse():                                       // 24Hz tick
    if !spell: KillThisEffect(); return        // :408-412

    switch my_state:
      STRIP_LAUNCH:                            // :416-440
          invoker.GetPos(newpos)
          ConvertToVector(facing, 10, vel)
          for pnt in 0..15:
              newpos.x += vel.x; newpos.y += vel.y
              block = MapPane.GetWalkHeight(newpos)
              if block == 0 or Player.CharBlocking(newpos):
                  targetrange = pnt + 2; break
          my_state = STRIP_FLY                 // unconditionally next tick

      STRIP_FLY:                               // :441-449
          duration--
          if duration == 0:
              my_state = STRIP_EXPLODE
              duration = 5

      STRIP_EXPLODE:                           // :451-453
          if duration: duration--

    TEffect::Pulse()
```

Notes:
- LAUNCH is a **1-tick** state: it does a 16-step short-range raycast for the
  spell's `targetrange` field (independent of `SetupObjects`'s longer
  raycast — they look like two parallel implementations of "find where the
  bolt ends"), then unconditionally advances to FLY.
- FLY counts down 20 ticks of `duration`, then transitions to EXPLODE with a
  fresh `duration=5`. EXPLODE counts down toward 0 but is **not** what kills
  the effect — the kill happens in `Animate` (§6.2) when both spark systems
  are empty.

### 6.2 Animator state machine (`TLightningAnimator::Animate`, `stripeffect.cpp:664-820`)

```
Animate(draw):                                 // 24Hz tick
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)                // :670

    // 1. Impact-spark trigger (only fires when strip is fully grown)
    if mystrip.curpoints == mystrip.maxpoints:                       // :673
        impact_spark.Get(params); params.particles = 20
        impact_spark.Set(params)
        if !damage_flag:                                              // :678
            damage_flag = true
            (compute end_p − 80 reference point; UNUSED — not stored,
             just shadows a local `new_pos`)                          // :681-685
    else:
        impact_spark.Get(params); params.particles = 0
        impact_spark.Set(params)                                      // :689-692

    // 2. EXPLODE side-effects (drain sparks, shrink glow, delete strip tail, deal damage)
    if state == STRIP_EXPLODE:                                        // :694
        params.particles = 0; spark.Set(params); impact_spark.Set(params)
        glow_scale *= 0.8                                             // :700
        for i in 0..ADD_FACTOR-1:  mystrip.DelEndPoint()              // :702-703
        total_offset += ADD_FACTOR * -10                              // :704

        if firsttime:                                                 // :706
            if (PTStripEffect)inst.chr:                               // :708
                if (PTStripEffect)inst.GetSpell():
                    spell.Damage(chr); firsttime = false              // :712-713

    // 3. GROWTH (FLY): add ADD_FACTOR=2 anchors per tick until numpoints == maxpoints
    if numpoints < maxpoints:                                         // :719
        for i in 0..ADD_FACTOR-1, while numpoints < maxpoints:
            p = (0, numpoints * -10, 0)                               // :725-727
            numpoints++
            if numpoints == maxpoints: maxsize = true                 // :730-731
            mystrip.AddPoint(&p)

    // 4. Tick the sub-emitters
    spark.Animate(); impact_spark.Animate()                           // :737-738

    // 5. Per-vertex strip jitter + cubic-spline smoothing
    //    (see §6.3 below)

    // 6. Rotating glow advances
    rotdegree = (rotdegree + 12) % 360                                // :814
    morrotdegree = (morrotdegree + 16) % 360                          // :815

    // 7. Strip texture U-scroll
    mystrip.ScrollTexture(-0.1)                                       // :817

    // 8. Self-destruct gate
    if state == STRIP_EXPLODE and !spark.GetCount() and !impact_spark.GetCount():
        ((PTEffect)inst).KillThisEffect()                             // :818-819
```

**Damage payload:** runs exactly **once**, on the first EXPLODE tick when
`damage_flag` has flipped true and the bolt found a `chr`. (Note the
commented-out per-anchor damage check inside `SetupObjects:613-616` —
`spell->Damage(chr)` was originally intended to be called there; live damage
is the single EXPLODE-time call.)

### 6.3 Strip jitter + smoothing (`stripeffect.cpp:740-810`)

```
px[i], pz[i] for i in 0..numpoints+SMOOTH_SIZE-1 := 0    // :744-746

// Major-anchor random offsets every SMOOTH_SIZE=4 anchors
for i in 1..numpoints-1:                                  // :749
    if i % SMOOTH_SIZE == 0:
        px[i] = random(-13, 13)
        pz[i] = random(-13, 13)

// Spline-interpolated offsets for the 3 intermediate anchors per group of 4
last_point = 0; next_point = SMOOTH_SIZE
for i in 1..numpoints-1:                                  // :762
    if i == next_point: last_point = next_point; next_point += SMOOTH_SIZE
    mod = i % SMOOTH_SIZE
    if mod == 1:   px[i] = (px[next]-px[last]) * 0.5 / SMOOTH_SIZE; pz similar
    elif mod == 2: px[i] = (px[next]-px[last]) * 2.0 / SMOOTH_SIZE; pz similar
    elif mod == 3: px[i] = (px[next]-px[last]) * 3.5 / SMOOTH_SIZE; pz similar
    px[i] += px[last]; pz[i] += pz[last]

// Apply jitter + down-droop to live strip points
for i in 1..numpoints-1:                                  // :797
    p = &mystrip[i]
    p.x = 0; p.z = 0
    if down: p.z = -((i-1) * 20) / (maxpoints-1)          // :802-803
    if i < maxpoints-1:                                   // :804
        p.x = px[i]                                        // :806
        p.z += pz[i]                                       // :807
```

Notes:
- The jitter is applied to **strip-local x and z** (the y axis is the
  strip's length axis, already set to `i * -10` in `AddPoint`).
- `down = true` (set in `SetupObjects:546`) introduces a **linear z-droop**
  from anchor 1 toward the tip: each anchor drops by `-(i-1)*20/(maxpoints-1)`
  in z, so the bolt sags slightly downward from hilt to tip across a 20 wu
  drop at the far end.
- The **last anchor (i == maxpoints-1)** intentionally keeps `p.x = 0,
  p.z = 0` (plus the down-droop) — it stays on the target. The jitter only
  fans the bolt's body, not its terminus (`if i < maxpoints-1` guard).
- The jitter is **re-rolled every tick** for the major anchors (`random(-13,
  13)` is called fresh each `Animate`), so the bolt visibly crackles/dances
  along its length even when its global path is fixed. The intermediate
  anchors derive from the major ones, so the spline curve is continuous each
  frame but renews each tick.

The 0.5/2.0/3.5 weights in the spline ramp are an asymmetric 4-knot cubic
fit (not a uniform Catmull-Rom): the tangent is heavier near the next major
anchor (weight 3.5 at mod=3) than near the previous (weight 0.5 at mod=1),
which biases the curve toward the next major control point.

### 6.4 Sub-emitter behavior (`TSubParticleAnimator::Animate`/`Create`, `effectcomp.cpp:361-524`)

Each tick, for each `TSubParticleAnimator`:

```
Animate():
    desired = min(params.particles, max_particles=20)
    while live_count < desired: Create()       // :370-372

    for each live particle:
        flicker_status = random(0, 1)          // :383 — re-rolled every tick
        life--; if life < 0: used = false      // :386-388
        scale.{x,y,z} *= scale_dec.{x,y,z}     // :391-393 — 0.90/tick geometric decay
        pos.{x,y,z} += velocity.{x,y,z}        // :396-398
        velocity.z -= gravity                  // :401 — 0.18/tick
```

```
Create():
    if random(1,100) > chance: return          // :458-459 — only ~25% chance each call
    find unused slot i; mark used = true       // :462-469
    pos = params.pos + random_per_axis(-pos_spread, +pos_spread)   // :472-474 (all 0 here)
    scale = params.scale + random_per_axis(-scale_spread, +scale_spread)  // (no spread)
    velocity per axis:
        if velocity_dir < 0: v = random(-velocity_spread, 0)        // (mixed-sign here)
        elif velocity_dir == 0: v = random(-velocity_spread, velocity_spread)
        else: v = random(0, velocity_spread)
        // For sparks: velocity_dir = (0,0,0) → all mixed-sign
        velocity = params.velocity (=0) + v
    gravity = params.gravity (= 0.18)
    life = random(min_life, max_life) (= 25..35)
    flicker = true; flicker_status = random(0,1); flicker_size = 2.5
```

Key behaviors of the spark systems for this effect:
- **Active particles cap** at the smaller of `params.particles` (20) and
  `max_particles` (20) → so up to **20 live sparks per emitter**, 40 total
  across both. Each tick `Animate` *tries* to top off to that number by
  calling `Create()` once per missing slot, BUT each `Create()` only succeeds
  ~25% of the time (`chance = 25`) — so the buildup is **probabilistic**:
  expected ~5 new particles per tick when starting from empty, ramping
  asymptotically to the cap. (The `TSubParticleAnimator::Animate` calls
  `Create()` exactly `desired - live_count` times per tick, NOT in a loop
  until success; so a high-chance value would fill fast and low values
  trickle. `chance=25` is a slow trickle.)
- **Velocity field:** all-axis mixed-sign jitter ±(2,2,1) wu/tick around 0,
  then gravity 0.18 on z. Net: sparks spew **radially-isotropic** in XY
  (with slightly tighter Z spread), arcing back down. NO directional bias
  toward the bolt's tip — the cast sparks "fountain" at the caster hand
  (omnidirectional), and the impact sparks "fountain" at the bolt-end
  (also omnidirectional, but triggered only when the bolt is fully grown).
- **Scale decay:** 0.90 per tick — particles shrink to ~0.81 after 2 ticks,
  ~0.59 after 5, ~0.10 after 22, effectively invisible by the upper end of
  the 25..35 lifetime. So the sparks fade by **scale**, not by alpha.
- **Flicker:** every tick, `flicker_status` is re-rolled `random(0, 1)` →
  ~50% of frames the particle's scale is multiplied by `flicker_size = 2.5`
  on draw. This produces the "twinkling" read on sparks.
- **No homing, no bounce, no sub-emit.**

### 6.5 Temporal diagram (required for multi-stage)

```
TStripEffect::Pulse state machine:
   LAUNCH(1)  │ FLY(20 ticks)                          │ EXPLODE(5..∞ until sparks empty) │ KILL
   raycast    │ duration 20→1                          │ duration 5→0, glow_scale *=0.8/t │
              │ ADD_FACTOR=2 anchors/tick added        │ ADD_FACTOR=2 anchors/tick popped │
              │ jitter re-rolls every tick             │ jitter still re-rolls            │
              │ U-scroll −0.1/tick                     │ U-scroll continues               │
   bolt grows from 2 anchors → maxpoints, then holds.  bolt shrinks from tail.
   
   numpoints (anchor count) vs tick:
     maxpoints┤                ●●●●●●●●●●●●●●●●●●
              │              ╱                  ╲
            2 ┤●──── (LAUNCH)                     ╲___
              │                                       ●── (EXPLODE)
            0 ┤                                            ●── (dead)
              0    1                 ~10                  21    ~30+
              tick

   glow_scale vs tick:
     3.4 ┤●─────────────────────────────●
         │                                ╲
         │ (constant during FLY)            ╲ ×0.8/tick (EXPLODE)
     0.0 ┤                                    ●─── (glow stops drawing at <0.005)
         FLY                              EXPLODE                                  

   spark (cast-hand) particle count:
     20  ┤        ●───────────────────●
         │       ╱ trickles up via      ╲ (params.particles = 0 in EXPLODE; lifetime
         │      ╱  chance=25 every tick  ╲  drains existing particles in 25..35 ticks)
      0  ┤●────                            ●───── 
         0       ~10                  20     ~50

   impact_spark particle count:
     20  ┤                  ●─────●
         │                  ↑      ╲
         │                  (turned ╲ (params.particles=0; drain)
         │                  on when  ╲
      0  ┤●──────────────────         ●─────
         │                  curpoints
         │                  == maxpoints
         0          ~10                   ~50
```

**Notes on the kill gate:** the effect doesn't die at `EXPLODE` end-of-
duration — it dies when **all sparks have aged out**. With max_life=35 and
spark trigger turning off at EXPLODE start, the sparks can live up to ~35
ticks past EXPLODE start before the kill fires. So the visible "tail" of
fading sparks at the impact site lasts ~1–1.5 s after the bolt visually
collapses.

For composites, the **decomposition** by component:

| component | archetype | lifecycle | renderer | blend | sub-object |
|-----------|-----------|-----------|----------|-------|------------|
| **strip** | trail/beam (SR) | grows over FLY, holds at maxpoints, shrinks tail in EXPLODE | TStripAnimator → `RenderObject` of obj 0 | Alpha (§7) | 0 (`striplaunch`) |
| **glow halves** | I3D-mesh (IM) | always drawn while `glow_scale > 0.005`; scale jitter per tick + cross-rotation +12°/+16° per tick | `RenderObject` of obj 1 twice | Alpha (§7) | 1 (`stripfly`) |
| **spark (cast)** | particle emitter (PE) | active during FLY only (params turned off at EXPLODE start) | `TSubParticleAnimator::Render` | Alpha (§7) | 2 (`stripexplode`) |
| **impact_spark** | particle emitter (PE) | turns on when bolt fully grown, off at EXPLODE start | `TSubParticleAnimator::Render` | Alpha (§7) | 2 (`stripexplode`) |

---

## 7. Rendering (original render state + geometry)

`TLightningAnimator::Render` (`stripeffect.cpp:822-915`) brackets the entire
composite in **Alpha** blend + cull-NONE:

```
Render():
    SaveBlendState()                                                  // :826
    SetBlendState()                                                   // :827 — Alpha
    savedcull = D3DRENDERSTATE_CULLMODE
    Device2.SetRenderState(CULLMODE, D3DCULL_NONE)                    // :830-831

    // ===== 1. STRIP =====
    if state != STRIP_EXPLODE OR duration > 0:                        // :836
        ResetExtents()
        mystrip.GenerateStrip(inst.GetFace() + showangle)             // :842
        obj = mystrip.GetStripObj()
        for v in 0..obj.numverts-1:
            obj.lverts[v].color = D3DRGBA(1, 1, 1, 1)                 // :848 — pure white
        if showangle != 0:                                            // :851
            D3DMATRIXRotateZ(&obj.matrix, (showangle*360/256)*TORADIAN)
        RenderObject(obj)                                             // :855
        UpdateExtents()

    // ===== 2. GLOW =====
    if glow_scale > 0.005:                                            // :861
        obj = GetObject(2)                                            // :863 (NOTE: obj 2, not obj 1 — likely a snapshot bug, see §13)
        ResetExtents()
        obj.flags = OBJ3D_MATRIX
        obj.scl = (glow_scale + random(0, 0.4)) * (1,1,1)             // :870

        // First half: rotate -rotdegree about Z
        D3DMATRIXClear(&obj.matrix)
        D3DMATRIXRotateZ(&obj.matrix, -rotdegree * TORADIAN)          // :874
        D3DMATRIXRotateX(&obj.matrix, -30 * TORADIAN)                 // :875
        D3DMATRIXRotateY(&obj.matrix,  60 * TORADIAN)                 // :876
        D3DMATRIXRotateZ(&obj.matrix, -((inst.GetFace*360)/256)*TORADIAN)  // :879 (offset to face)
        D3DMATRIXTranslate(&obj.matrix, &obj.pos)
        D3DMATRIXScale(&obj.matrix, &obj.scl)
        RenderObject(obj)

        // Second half: rotate +morrotdegree about Z (opposite direction)
        D3DMATRIXClear(&obj.matrix)
        D3DMATRIXRotateZ(&obj.matrix, morrotdegree * TORADIAN)        // :888
        D3DMATRIXRotateX(&obj.matrix, -30 * TORADIAN)
        D3DMATRIXRotateY(&obj.matrix,  60 * TORADIAN)
        D3DMATRIXRotateZ(&obj.matrix, -((inst.GetFace*360)/256)*TORADIAN)
        D3DMATRIXTranslate(&obj.matrix, &obj.pos)
        D3DMATRIXScale(&obj.matrix, &obj.scl)
        RenderObject(obj)
        UpdateExtents()

    // ===== 3. SPARKS =====
    spark.Render()                                                    // :906
    impact_spark.Render()                                             // :907

    Device2.SetRenderState(CULLMODE, savedcull)
    RestoreBlendState()                                               // :912
    return true
```

- **What it draws:**
  - **Strip:** one connected quad ribbon of `(numpoints-1) * 2` triangles
    (`effectcomp.cpp:807-848`); per-tick `GenerateStrip` rotates the
    "thickness" axis (`myx[ang]`, `myz[ang]`) about local-Z based on world
    face-angle so the ribbon faces the camera in an iso view
    (`effectcomp.cpp:873-919`). Vertex color is **pure white, full alpha**
    explicitly written into all `lverts[i].color` (`stripeffect.cpp:848`).
  - **Glow:** the same sub-object drawn **twice**, with two independent
    Z-rotations that **rotate in opposite directions** (`-rotdegree`,
    `+morrotdegree`) at slightly different rates (12 vs 16 °/tick), creating
    a continuously-recombining cross/star figure at the caster hand. Both
    are rotated -30° about X and +60° about Y as a base orientation, then
    a final `-((face/256)*360)*TORADIAN` Z-rotate aligns the cross with the
    caster's facing. Scale per-tick jitters `+random(0, 0.4)` on top of
    `glow_scale` so the glow visibly pulses/sizzles.
  - **Sparks:** one billboard per particle (per `spark` + per `impact_spark`);
    `TSubParticleAnimator::Render` (`effectcomp.cpp:526-570`) sets
    `OBJ3D_MATRIX | OBJ3D_ABSPOS`, clears the matrix, then rotates
    `(rotZ -π/3, rotX -π/4, rotY 0)` (so each spark quad tips toward
    the camera ≈ a screen-aligned billboard up to a fixed bias), scales by
    `particle.scale * (flicker ? flicker_size : 1)`, translates to
    `particle.pos` (world). Note: not a free billboard — a fixed pre-rotated
    quad ABS-positioned (this is the sub-emitter's stock orientation). UVs
    are not modified.
- **Blend mode (original) — Alpha, traced via the helper:**
  - `Render` calls `SetBlendState()` (`stripeffect.cpp:827`), which sets
    `D3DTBLEND_MODULATE` + `SRC_ALPHA / INV_SRC_ALPHA` + `ZWRITE off` +
    `ZTEST on` (`effect_old.cpp:221-233`) ⇒ **`Alpha` (modulated)**
    (NOMENCLATURE §3).
  - The blend state is **NEVER switched to `SetAddBlendState()`** anywhere
    in `TLightningAnimator::Render` (`stripeffect.cpp:822-915`). All three
    components — strip, glow, sparks — render through the same Alpha state.

  **BLEND SANITY-CHECK (mandatory):**
  - The strip texture is a **bright bolt-crackle on a black-keyed background**
    (knowledge `02_ASSETS_IMAGERY.md` §4 — the >20%-black auto-detect fires
    for these effect textures). The glow is also bright-on-black. Both are
    **textbook additive** sprites by design: under additive blending the
    black contributes nothing and only the bright bolt/glow energy
    accumulates — the canonical "self-lit overlay" look (NOMENCLATURE §3
    `AdditiveStraight` row).
  - **The snapshot draws all three at `SetBlendState` (Alpha). This is
    SUSPECT.** Per the protocol § 7 BLEND SANITY-CHECK rule: a bright-on-
    black sprite drawn with Alpha is a snapshot-Alpha-on-an-obvious-glow
    pattern — **likely shipped additive**, and the retail render body is NOT
    decompiled (§2.1 cross-check 3), so the blend is unverified-vs-shipped
    and at high snapshot-drift risk.
  - **Sister-family cross-check (corroborating the flag):**
    - `TWindStripAnimator::Render` (`stripeffect.cpp:1244-1369`) is the
      DIRECT sister effect (same `TStripAnimator` machinery, also a glow
      object). It opens with `SetBlendState()` for the wind ribbons + the
      spark drops (`:1254`), THEN **switches to `SetAddBlendState()` for the
      halo overlay** (`:1333`). So the sister's halo IS additive even though
      its ribbon body is alpha. This is the **two-pass blend pattern** we'd
      expect to see here (strip Alpha base for crispness + glow Additive for
      energy bloom + sparks Additive for self-lit twinkle). The snapshot
      `TLightningAnimator` does **not** do that — it stays Alpha
      throughout.
    - Fire-family glows (`TFireFlashEffect`, `TFireWindEffect`,
      `TFireConeEffect`, `TDragonFireEffect`) all use `SetAddBlendState`
      per the catalog notes (knowledge `03_RENDERING_CONVENTIONS.md`); ice
      and magic glows similarly. Lightning being pure-Alpha is an outlier.
  - **AGENT_GUIDE §4.2.1.5 (color = health signal):** if the reconstruction
    draws the bolt as a desaturated translucent ribbon rather than a
    saturated white-hot bolt that BURNS through anything behind it, the
    Alpha-vs-Additive call is the first place to debug.
  - **FLAG:** `code = Alpha for ALL passes, but every sprite is bright-on-
    black; the sister TWindStripAnimator already does Additive for its
    halo; the retail TLightningAnimator render body is NOT in the recon.`
    Treat the snapshot blend as a **hypothesis to visually vet** against
    in-game footage (§12). Most likely retail render: **strip = Alpha
    (matches snapshot)** for a crisp body; **glow = AdditiveStraight (ONE/ONE)**
    for energy bloom; **sparks = AdditiveStraight** for twinkle.
    Reconstruction should A/B both options against the in-game capture and
    pick whichever matches the bright/burning-out reading.
- **Lit vs self-lit:** **Unlit / self-lit.** Strip writes
  `D3DRGBA(1,1,1,1)` per-vert (no scene light contribution), glow writes no
  per-vertex color (so the imagery's authored diffuse passes through
  unmodified under MODULATE), sparks write no per-vertex color. The bolt
  glows on its own; no `DoLighting` or `GetClosestLights` call anywhere in
  the lightning code path. NOMENCLATURE §4 `Unlit`.
- **Depth / Z:** **TestNoWrite** — `SetBlendState` sets `ZENABLE=true`,
  `ZWRITEENABLE=false` (`effect_old.cpp:224-225`). All three passes inherit.
  `RefreshZBuffer` (`stripeffect.cpp:917-973`) restores scene Z over the
  composite afterward (see below).
- **Orientation:**
  - **Strip:** the ribbon is **WorldFaced** — `TStripAnimator::GenerateStrip`
    rotates the thickness axis about local-Z based on the world face-angle
    so the strip stays edge-on-to-camera in iso view. Not exactly
    `ScreenAligned` (no camera-facing computation), not `WorldXY` (not
    floor-flat) — it's a **horizontal ribbon** whose width plane is set by
    a precomputed 8-bin angle lookup (`myx[8]/myz[8]` table in the
    `TStripAnimator` ctor, `effectcomp.cpp:783-789`). The ribbon then takes
    a Z-rotation from `showangle` (=0 in this effect, so identity) per
    `stripeffect.cpp:851-852`. NOMENCLATURE wise this is **closest to
    `StretchedAlongVelocity`** (future enum, NOMENCLATURE §2) but stretched
    along the bolt's local-Y axis rotated to caster.facing, not a true
    velocity vector.
  - **Glow:** **WorldFaced** with explicit XYZ rotation cocktail (`X=-30°,
    Y=+60°, Z=-rot/+morrot deg, Z=-face deg`). Not screen-aligned. Pre-
    canned matrix transform creates a 3D star/cross figure that orbits the
    hand. NOMENCLATURE — needs a new term for "pre-canned 3-axis tumbling
    glow" — for now describe explicitly.
  - **Sparks:** **fixed pre-rotated quad, world-positioned.** Always
    rotated `rotZ -π/3, rotX -π/4, rotY 0` regardless of camera; positioned
    in absolute world via `OBJ3D_ABSPOS`. Approximates a screen-aligned
    billboard for the canonical iso view but won't rotate with the camera.
- **Per-quad / per-object transform:**
  - Strip: `OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES |
    OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES` (set by
    `TStripAnimator::GenerateStrip`, `effectcomp.cpp:918`); matrix is
    cleared then optionally rotated by `showangle`.
  - Glow: `OBJ3D_MATRIX`; matrix = `RotZ * RotX(-30°) * RotY(60°) * RotZ(-face) *
    Translate(obj.pos) * Scale(obj.scl)`. Note: this matrix-order convention
    means the **scale is applied LAST** in the matrix chain (it's a column-
    vector D3D matrix, so the right-most multiplied is applied first to the
    vertex). So `(scale → translate → faceRotZ → Y60 → X-30 → rotZ)` is the
    effective vertex order.
  - Sparks: `OBJ3D_MATRIX | OBJ3D_ABSPOS`; matrix = `Scale * RotZ(-π/3) *
    RotX(-π/4) * RotY(0) * Translate(particle.pos)`.
- **Per-vertex color packing:**
  - Strip: `obj->lverts[i].color = D3DRGBA(1.0f, 1.0f, 1.0f, 1.0f)` for ALL
    verts (`stripeffect.cpp:848`) — pure white, full alpha. Under MODULATE
    the strip texture passes through at full intensity.
  - Glow: NO per-vertex color write — imagery's authored diffuse is used.
  - Sparks: NO per-vertex color write — imagery's authored diffuse on the
    spark sub-object is used.
- **RefreshZBuffer** (`stripeffect.cpp:917-973`):
  - One 80×40 px patch centered on the projected effect origin.
  - One 20×20 px patch per anchor along the strip (looped through every
    `mystrip[i]` for `i in 0..numpoints`, projecting each anchor to screen
    and clearing).
  - The SPARKS `RefreshZBuffer` IS COMMENTED OUT (`:938` `//spark.RefreshZBuffer(4, 4)`).
  - The GLOW `RefreshZBuffer` IS COMMENTED OUT (`:966-972`).
  - Net: the bolt strip clears Z generously; the glow and sparks rely on
    the strip's clears + the engine background's prior Z.

---

## 8. Texture animation

- **Strip:** **UV-scroll**, ON. `TLightningAnimator::Animate` calls
  `mystrip->ScrollTexture(-0.1f)` every tick (`stripeffect.cpp:817`).
  `TStripAnimator::ScrollTexture` advances `uoff` by `deltau` (=-0.1) and
  wraps within `[ubeg, uend]` (`effectcomp.cpp:863-871`). `SetTextureRange(0,1)`
  is the default (`effectcomp.cpp:797`), so the U range is `[0, 1]`; the
  texture scrolls **backward along U** at 0.1/tick = ~2.4 wraps/sec @ 24Hz.
  Direction `-0.1` causes the texture to **flow toward the caster** along
  the bolt (since the strip's U axis runs from caster to tip per
  `tfreq = pnt * ufrequency + uoff` in `GenerateStrip`, `effectcomp.cpp:908-915`).
- **Glow:** **NONE.** `Render` never mutates `tu/tv` and never calls
  `SetTextureFrame`.
- **Sparks:** **NONE.** `TSubParticleAnimator::Render` never mutates
  `tu/tv` (`effectcomp.cpp:526-570`).
- **No `framehtexs[]` flipbook** is set up anywhere in the lightning code
  path. (No `SetTextureFrame`, no `textureframe[]`, no `OBJ3D_TEXFRAME` flag
  written.)

Mechanism per `02_ASSETS_IMAGERY.md` §5: this is **UV-scroll on the strip
only**.

---

## 9. Associated light

- **Effect-emitted light:** **NONE.** `TLightningAnimator::Initialize` and
  `Animate` make no `AddPointLight` / `AddLight` / dynamic-light call
  (`stripeffect.cpp:472-820`); `TStripEffect` makes none either. The bolt
  doesn't add a runtime scene-light contribution from inside the effect.
- **Spell-emitted light:** the **spell** (`spell.def:683`) does carry a
  `LIGHT COLOR 150,150,255 INT 255 MULT 20` directive — this is a
  **150/150/255 = pale electric blue** light at intensity 255 with a
  multiplier of 20, posted by the spell-cast machinery (not by this effect).
  Whether that light follows the bolt or just flashes the room briefly is
  a spell-side concern outside `TLightningAnimator`. **Record for the
  reconstruction:** the cast event has an associated 150/150/255 light;
  whether it's a one-shot flash or a lifetime-following light depends on
  the spell loader's interpretation of `LIGHT`, not on this effect's body.
  (The reconstruction agent may need to surface this from the spell-loader
  forensics.)

---

## 10. Color

- **Source:**
  - **Strip:** the authored bolt-crackle texture on sub-object 0 (`striplaunch`
    in snapshot, likely `start` in retail), **modulated by pure-white
    full-alpha per-vertex color** explicitly written by Render
    (`stripeffect.cpp:848`). The effect supplies no hue of its own — the
    color is whatever the sprite is, passed through at full intensity.
  - **Glow:** the authored glow sprite on sub-object 1 (`stripfly` /
    likely a `start`/`end` half in retail), un-tinted (no per-vertex color
    write).
  - **Sparks:** the authored photon sprite on sub-object 2 (`stripexplode` /
    likely retail `sparks`), un-tinted.
- **Exact values:** sprites are **bright white/blue electric** by inspection
  of the registered asset family + the spell.def `LIGHT COLOR 150,150,255`
  hint + the surrounding lightning naming. Specifically:
  - Strip texture: bright white/blue crackle on black-keyed background (a
    horizontal line-art pattern that reads as a lightning bolt).
  - Glow texture: bright white-blue disc, bright center fading to black.
  - Spark sprite: bright white/yellow-white photon (the same photon family
    as F07 fireball trails, X22 sparks — see SPARKS_TSparkAnimator.md §10).
- **Expected visual:** a **pure-white-hot zigzag bolt** with a pale-blue
  electric glow nimbus at the caster's hand and twinkling white sparks
  spraying from both the hand (during FLY) and the impact point (at the
  moment the bolt fully extends). The whole composite should read as a
  **saturated, self-luminous, near-blinding** electric strike — **NOT** a
  translucent grey-blue ribbon. (AGENT_GUIDE §4.2.1.5 — pale/desaturated =
  reconstruction broken; the likely culprits are (a) wrong blend mode per
  §7's SANITY-CHECK flag, (b) per-vertex color override not honored, or
  (c) procedural strip texture stand-in instead of the real `NewLightStrip.I3D`
  imagery.)
- **Normalization / boosts:** none in the effect code; all brightness comes
  from the texture × white per-vertex modulate + blend accumulation.

---

## 11. Audio coupling

- **Spawn sound:** `PLAY("lightning sound")` in `TLightningAnimator::Initialize`
  (`stripeffect.cpp:479`). One shot, played once when the animator constructs
  (i.e. at spell-cast time). The literal wave name string is
  `"lightning sound"` (note the spaces — looks up that exact wave name).
- **Impact / damage sound:** **none in the effect.** No `PLAY()` call in
  `Animate`, `Render`, or the EXPLODE-side damage block (`stripeffect.cpp:706-716`).
  If the spell's `Damage()` plays a hit sound on the target, that is in the
  combat/spell code, not here.
- **Spell.def `DELAY 5`** (`spell.def:682`) is a spellcast cadence param,
  not an audio cue.

Record for the audio phase: **the bolt is audio-paired with `"lightning sound"`
at cast spawn (`stripeffect.cpp:479`); no additional sound on impact or
EXPLODE.**

---

## 12. Triggers & in-game appearance

- **Spawned by:** the **Lightning spell** (`spell.def:675-705`), via three
  variants:
  - `"Electric Bolt"` (Locke "AI" talisman, mana 105, dmg 155-187,
    `spell.def:688`)
  - `"Priest Bolt"` (Mahatma "MJ" talisman, mana 48, dmg 58-68,
    `spell.def:694`)
  - `"HighPriest Bolt"` (Malachi "MAL" talisman, mana 82, dmg 82-112,
    `spell.def:700`)

  All three set EFFECT_NAME `"lightstrip"` and `ATTACH "lhand"`. The
  spell system spawns a `TStripEffect` (built via
  `DEFINE_BUILDER("LightStrip", TStripEffect)` `stripeffect.cpp:392`, hashed
  via `REGISTER_BUILDER(TStripEffect)` `:393`) and pairs it with the
  `TLightningAnimator` (`REGISTER_3DANIMATOR("LightStrip", TLightningAnimator)`
  `:470`). The animator runs the visual; the effect runs the state machine
  + raycast + damage.
- **DAMAGETYPE:** `DT_BURN` (`spell.def:680`). Note: lightning deals **burn
  damage type**, NOT a separate "electric" type — combat resistances will
  treat it like fire.
- **Where to see it in the original game:** cast any Lightning variant in
  combat. Easiest repro: Locke at the start of the game does NOT have
  Lightning; the `MJ` (Priest Bolt) variant unlocks for low-level Priest
  characters early-game, the `AI` (Locke's Electric Bolt) becomes available
  mid-Locke-progression. For the cleanest standalone capture: enter any
  outdoor area with a clear line-of-sight to an enemy, cast Lightning on
  the target, observe the bolt-fly-out + impact-spark burst. Look for:
  - rapid bolt extension from hand toward target (~0.5 s)
  - per-tick crackling jitter along the bolt length (visibly redraws each
    sim tick)
  - the U-scrolled "flow" along the bolt's length (texture animation)
  - cross-rotating glow at the hand (orbits in opposite directions at 12/16
    deg/tick)
  - twinkling sparks at hand throughout FLY
  - secondary spark burst at impact point once bolt fully extended
  - tail-shrink fade-out (~0.2 s) after FLY completes
  - residual sparks fading at impact site for ~1 s after the bolt vanishes
- **Vestigial?** No — live spell caller; the `"lightstrip"` EFFECT_NAME
  appears 3 times in shipped `spell.def`.

---

## 13. Gaps & uncertainties

- **13.1 RETAIL ASSET DIVERGENCE — `NewLightStrip.I3D` is NOT identical
  (the headline risk).** Per §2.1, the shipped asset is 50,643 B with 3 sub-
  objects (`start`/`sparks`/`end`) vs the snapshot's 18,680 B with 4 dev-
  named sub-objects (`striplaunch`/`stripfly`/`stripexplode` + empty 4th).
  The animator hard-codes `GetObject(0)` = strip carrier, `GetObject(1)` =
  glow, `GetObject(2)` = spark sub-object — these indices in the **retail**
  asset map to `start`, `sparks`, `end` respectively, which is a different
  set of authored content. **Reconstruction MUST use the retail asset's
  per-sub-object content** (decode each retail sub-object's textures +
  geometry) and verify which slot is the strip-carrier vs glow vs spark; do
  NOT assume the snapshot's dev meshes are shipped.
- **13.2 RENDER BLEND is snapshot-only and SUSPECT (per §7 SANITY-CHECK).**
  `SetBlendState()` (Alpha) for all three components on a bright-on-black
  sprite is the textbook Alpha-on-glow drift case, and the retail render
  body is NOT decompiled. Sister `TWindStripAnimator` switches to additive
  for its halo. **Most likely retail:** strip = Alpha (crisp ribbon); glow +
  sparks = AdditiveStraight (energy bloom). Reconstruction should visually
  vet both blends against in-game footage and prefer the additive halo +
  sparks if the snapshot reading reads dull.
- **13.3 All numeric constants are snapshot-only (§2.1, §3).** Nothing in
  §3 is retail-corroborated. The reconstruction agent should consider all
  immediate values (gravity 0.18, scale 2.0, glow_scale 3.4, decay 0.8,
  durations 20/5, jitter ±13, U-scroll -0.1, rotdegree 12/16, raycast
  step 10 wu) as **best-evidence-from-snapshot** and be prepared to retune
  to match in-game visuals.
- **13.4 Snapshot bug: glow loads sub-object 2, not 1.** `Render` reads the
  glow as `GetObject(2)` (`stripeffect.cpp:863`), but `SetupObjects` did
  `NewObject(1)` for the glow (`:656`) and `NewObject(2)` for the spark
  (`:660`), and the spark sub-object is also addressed as `GetObject(2)` in
  `Initialize` (`:496`). So **the glow is rendering the SAME sub-object as
  the sparks** in the snapshot — almost certainly a snapshot dev bug
  (off-by-one in the glow index). Reconstruction should very likely
  **render glow from sub-object 1**, not 2. Verify against retail: if
  retail's sub-object 1 (`sparks`) is the spark sprite, then the glow must
  come from elsewhere — possibly the retail asset combines `start` + `end`
  as the two glow halves (one per Z-rotation direction), or the retail
  asset just doesn't have a glow at all and the visual is sparks-only.
  This is a fidelity question that ONLY the retail asset can answer.
- **13.5 Two parallel raycasts for "bolt length" (`Pulse` vs `SetupObjects`).**
  `TStripEffect::Pulse` does a 16-step short raycast and stores `targetrange`
  (`stripeffect.cpp:425-436`); `TLightningAnimator::SetupObjects` does an
  open-ended ground-walk raycast up to MAX_MAXPOINTS=100 steps and stores
  `end_p` + `maxpoints` (`:566-633`). They look like two implementations of
  the same idea; `targetrange` is **never read** in the snapshot's animator
  code, only `maxpoints`/`end_p` matter. The `Pulse` raycast appears
  vestigial in the snapshot; reconstruction can omit it. (The commented-out
  `maxpoints = targetrange == -1 ? 40 : targetrange` in `SetupObjects:550`
  suggests an abandoned attempt to wire the two together.)
- **13.6 `damage_flag` block in Animate is partially dead code.** When the
  bolt fully extends, the animator computes `new_pos = (end_p.x, end_p.y,
  end_p.z - 80)` (`stripeffect.cpp:681-685`), but **never stores it** —
  `new_pos` is a stack local that goes out of scope. This was probably
  intended to be used for an extra impact-damage range check that didn't
  ship. The actual damage call lives in the EXPLODE branch
  (`spell.Damage(chr)`, `:712`) using the `chr` found during `SetupObjects`.
  Reconstruction can omit the unused `new_pos` block.
- **13.7 SetupObjects target-search loop has a subtle bug.** The inner loop
  iterates `for (TMapIterator i...) { ((PTStripEffect)inst)->chr = (PTCharacter)i.Item();
  ... break; }` (`stripeffect.cpp:596-619`). When `break` fires, `chr` is
  set to the matched enemy. When the loop runs to completion without break,
  `chr` holds the LAST iterated character — then the post-loop check
  `if (chr == invoker) chr = nullptr` (`:620-621`) clears it only if the
  last iterated character was the caster, NOT if it was a non-targeted ally
  or an unreachable enemy. So `chr` may be left holding a "wrong" character
  reference whose distance check failed. The downstream `spell.Damage(chr)`
  in `Animate` (`:712`) would then damage the wrong target. This is a
  latent snapshot bug; the actual `damage_flag` gate is also only true when
  `flag == true` upstream, so `chr` should always be the intended hit, but
  the code is fragile. Reconstruction should clean this up — set `chr =
  nullptr` *before* the iterator loop and assign only on the `flag = true`
  branch.
- **13.8 LAUNCH state's raycast doesn't update `end_p` or `maxpoints`.**
  `Pulse`'s LAUNCH branch stores `targetrange` only; `SetupObjects` (which
  runs as part of `T3DAnimator::Initialize`, called from animator
  construction earlier) has already computed `end_p` + `maxpoints` from its
  own raycast. So the LAUNCH state effectively does nothing visible to the
  visual mechanism. Treat it as 1-tick state-machine glue.
- **13.9 SetupObjects's `if (!TARGET_POS) maxpoints *= SMOOTH_SIZE` block
  is DEAD in the shipped path.** `TARGET_POS` is `#define`d to `1`
  (`stripeffect.h:75`), so `!TARGET_POS` is `0` and the multiply never
  fires. This is a developer demo path; do not reconstruct.
- **13.10 The Class.Def `"Lightning Spark"` asset is not referenced by this
  effect's code.** `Magic\Lightning Spark.I3D` (snapshot ≡ retail byte-
  identical, §2.1) exists but `TLightningAnimator` reads its spark sprite
  from `GetObject(2)` of the *strip's* own imagery, not from this separate
  asset. Possible explanations:
  - It's a sibling sprite used by `TLightningStorm` (cls_0x5b0a28) or
    another lightning effect.
  - The retail rework embedded this sprite's content into the retail
    `newlightstrip.i3d`'s `sparks` sub-object (so the byte-identical 9092 B
    asset and the new 50643 B asset share the spark content).
  - It's a stale orphan asset.

  Reconstruction should decode the retail `newlightstrip.i3d`'s `sparks`
  sub-object first; if its spark imagery matches `Lightning Spark.I3D`, then
  hypothesis (b) is confirmed.
- **13.11 Glow draw uses MATRIX scale even though scale also pre-multiplies
  obj.scl.** Render writes `obj->scl.x = obj->scl.y = obj->scl.z = glow_scale
  + random(0, 0.4)` (`:870`) AND then `D3DMATRIXScale(&obj->matrix, &obj->scl)`
  (`:882, 896`). Since `obj->flags = OBJ3D_MATRIX`, the matrix-built scale
  is what's used; the `obj->scl` field is set both for the matrix build and
  (redundantly) for the OBJ3D_SCL flag path which is NOT set here. So
  `obj->scl` is just a vec3 holder for the matrix build. No bug, just
  redundant authorship.
- **13.12 The strip clears its own per-vertex color to white every frame,
  but the strip's authored `lverts` are also pure-white-alpha by default
  (`AddPoint` writes only z/tu/tv, not color). So the explicit `D3DRGBA(1,1,1,1)`
  fill at `:848` is either defensive or for a planned future per-vertex
  alpha curve (e.g. fading at the tip). Currently it does nothing observable.
- **13.13 Effect3.cpp source filename.** The `recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml:9-31`
  notes that `cls_0x5b0a28` carries a `d:\revenant\Effect3.cpp` assert
  string. This is the **`TLightningStorm`** class (not `TLightningAnimator`),
  but it suggests there was a separate `Effect3.cpp` translation unit in the
  retail source containing the lightning-storm code. The snapshot's
  `stripeffect.cpp` predates this split. Useful as context for the recon
  pass; not a behavioral fact about this effect.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\NewLightStrip.I3D — USE THE RETAIL ASSET (50,643 B, 3 sub-objects
      start/sparks/end), NOT the snapshot's 18,680 B 4-object dev version. Decode
      each retail sub-object's textures + geometry and identify which slot is the
      strip carrier vs the glow vs the spark sprite — the snapshot's "obj0=strip,
      obj1=glow, obj2=spark" indices may have shifted. (§2.1, §4, §13.1)
- [ ] Drive it as TStripEffect (state-machine shell) + TLightningAnimator (visual
      composite), with an embedded TStripAnimator (strip helper) + two
      TSubParticleAnimator instances (spark, impact_spark). Registration name =
      "LightStrip" (NOT "Lightning"). spell.def EFFECT_NAME = "lightstrip" lower-
      case for all three Lightning variants. (§2, §12)
- [ ] Initialize:
      - Spawn z bump: effect.z += 50 (hand height). (§3, §5)
      - Read caster's left-hand world position from the "lhand" sub-object's
        animation matrix; this is the cast-spark spawn origin. (§5, §6.2)
      - Play "lightning sound" (single shot). (§11)
      - Initialize the cast `spark` sub-emitter with: particles=20, chance=25,
        pos=hand_world, pos_spread=0, scale=2.0, scale_dec=0.90,
        velocity_spread=(2,2,1), velocity_dir=0 (mixed-sign), gravity=0.18,
        min_life=25, max_life=35, flicker=true, flicker_size=2.5, all sub-
        object = sub-object index that holds the spark sprite. (§3, §6.4)
      - Initialize the `impact_spark` sub-emitter with the SAME params except
        particles=0 (off until bolt fully extends) and pos=end_p (the bolt's far
        end, computed by SetupObjects). (§3, §6.4)
      - glow_scale = 3.4, total_offset = 0. (§3)
- [ ] SetupObjects (compute end_p + maxpoints via ground-walking raycast):
      - r=10; step direction = ConvertToVector(caster.face). (§5)
      - Walk forward in 10-wu steps from effect.pos; at each step check
        MapPane.GetWalkHeight + search for any enemy character within 16 wu of
        (point.x, point.y, point.z-100); break on either (wall or enemy hit).
      - Cap maxpoints at min(100, raycast_steps). (§3 MAX_MAXPOINTS=100)
      - end_p = (final_point.x, final_point.y, final_point.z - 20). (§3, §6.2)
      - Seed strip with 2 anchors at (0, 0, 0) and (0, -10, 0) in object-local
        space. Strip width = hilt 32 wu → tip 20 wu via SetWidth(32, 20). (§3, §5)
- [ ] Pulse state machine (24Hz):
      - LAUNCH (1 tick): unconditionally → FLY. (Don't bother reconstructing the
        16-step Pulse raycast — it's vestigial in the snapshot, §13.5.)
      - FLY: duration starts 20, --duration per tick; on 0 → EXPLODE, duration=5.
      - EXPLODE: duration--. (Self-kill is NOT here — it's in Animate, gated on
        sparks empty.) (§6.1)
- [ ] Animate per tick (24Hz):
      1. If mystrip.curpoints == mystrip.maxpoints: impact_spark.particles = 20
         (else 0); damage_flag latches true on first time. (§6.2)
      2. If EXPLODE: spark.particles = impact_spark.particles = 0; glow_scale *=
         0.8; pop 2 anchors from strip tail (DelEndPoint × ADD_FACTOR=2);
         total_offset += -20; on `firsttime`, spell.Damage(chr) ONCE. (§6.2)
      3. If numpoints < maxpoints (FLY growth): add 2 anchors per tick at
         (0, numpoints*-10, 0); set maxsize=true when numpoints hits maxpoints.
         (§3, §6.2)
      4. spark.Animate() + impact_spark.Animate() (per §6.4 — each tries to top
         up to .particles via Create() with chance=25 gate, ages particles by
         decrementing life, decays scale ×0.90, integrates p+=v, v.z-=0.18).
      5. Strip jitter + spline smoothing (§6.3):
         - Allocate px[numpoints+4], pz[numpoints+4] zeroed.
         - Major anchors (i % 4 == 0): px=pz=random(-13, 13).
         - Intermediate anchors: linear-fraction interpolation with weights
           0.5/2.0/3.5 (NOT uniform Catmull-Rom).
         - Apply: strip[i].x = (i < maxpoints-1) ? px[i] : 0; strip[i].z =
           (-((i-1)*20)/(maxpoints-1)) + ((i < maxpoints-1) ? pz[i] : 0).
         - (Last anchor stays on target — no jitter on the terminus.)
      6. rotdegree = (rotdegree + 12) % 360; morrotdegree = (morrotdegree + 16) %
         360. (§3, §6.2)
      7. mystrip.ScrollTexture(-0.1). (§3, §6.2, §8)
      8. If EXPLODE && spark.empty && impact_spark.empty: KillThisEffect(). (§6.2)
- [ ] Strip render geometry: TStripAnimator builds a quad ribbon of
      (curpoints-1)*2 triangles via 8-bin angle-lookup-rotated thickness;
      GenerateStrip(face + showangle=0) per render frame. Texture mapped along U
      with ufrequency=1/maxpoints; uoff advances per ScrollTexture call. (§7)
- [ ] Strip rendering:
      - SaveBlendState + SetBlendState → Alpha (MODULATE, SRCALPHA/INVSRCALPHA,
        ZWRITE off, ZTEST on). NOTE: snapshot-only — see §7 SANITY-CHECK and
        §13.2; visually vet vs in-game capture. Most likely actual retail
        blend = Alpha (matches snapshot) for strip body but Additive for
        glow + sparks. (§7)
      - CULLMODE = D3DCULL_NONE (both faces). (§7)
      - lit-mode = Unlit (per-vertex white, no DoLighting). (§7, §10)
      - depth = TestNoWrite (ZTEST on, ZWRITE off). (§7)
      - Per-vert color = D3DRGBA(1, 1, 1, 1) on ALL strip lverts. (§7)
- [ ] Glow rendering: draw sub-object TWICE if glow_scale > 0.005, with
      scale = glow_scale + random(0, 0.4) and:
      - Half-A: matrix = RotZ(-rotdegree) · RotX(-30°) · RotY(60°) · RotZ(-face·360/256)
        · Translate(obj.pos) · Scale(obj.scl). (§7)
      - Half-B: matrix = RotZ(+morrotdegree) · same chain. (§7)
      - Same blend/depth as strip (or vet additive per §13.2). (§7)
      - NOTE snapshot reads glow from GetObject(2) which is also the spark sub-
        object — likely a snapshot bug, §13.4. Use the retail asset's actual
        glow sub-object (probably index 0=start and/or 2=end). (§13.4)
- [ ] Spark rendering: for each TSubParticleAnimator (spark + impact_spark), for
      each live particle, draw the spark sub-object via:
      - OBJ3D_MATRIX | OBJ3D_ABSPOS
      - matrix = Scale(particle.scale * (flicker_status ? 2.5 : 1)) · RotZ(-π/3)
        · RotX(-π/4) · RotY(0) · Translate(particle.pos.xyz with FIX_Z_VALUE on z)
      - same blend/depth as the rest (or vet additive). (§7)
- [ ] Texture animation = strip U-scroll -0.1/tick, default range [0, 1] with
      wrap. Glow + sparks have NO texture animation (no UV scroll, no flipbook).
      (§8)
- [ ] Color = authored bright white/blue from the asset textures × white per-
      vertex modulate on the strip; un-tinted glow + sparks. Verify reads as
      saturated white-hot bolt — pale/grey = port broken. (§10, §13.2)
- [ ] Associated dynamic light EMITTED by the effect: NONE. The spell.def
      `LIGHT COLOR 150,150,255 INT 255 MULT 20` is a spell-side concern, not
      this effect's body. (§9)
- [ ] RefreshZBuffer: 80×40 px patch around projected effect origin + 20×20 px
      patch per anchor along the strip. Sparks + glow Z-restores are commented
      out — relying on the strip's per-anchor clears + the background's prior Z.
      (§7)
- [ ] Audio: PLAY("lightning sound") ONCE on animator Initialize; NO additional
      sound from this effect (impact sound is the spell's `Damage()` concern).
      (§11)
- [ ] Trigger wiring: spawned by Lightning spell, EFFECT_NAME "lightstrip" with
      ATTACH "lhand"; three variants (Electric Bolt / Priest Bolt / HighPriest
      Bolt). DAMAGETYPE = DT_BURN. (§12)
- [ ] Self-kill: NOT at EXPLODE end-of-duration — at first tick after EXPLODE
      where both spark sub-emitters' GetCount() == 0 (so the tail of fading
      sparks lasts ~25-35 ticks past the bolt's visual end). (§6.2)
- [ ] Do NOT reconstruct:
      - The 16-step Pulse raycast → targetrange (§13.5 — vestigial).
      - The `new_pos = end_p - 80` block in Animate's damage_flag branch
        (§13.6 — stack-local that goes out of scope, dead).
      - The `if (!TARGET_POS) maxpoints *= SMOOTH_SIZE` demo path (§13.9 —
        TARGET_POS is hard-1).
      - The snapshot's likely glow-loads-spark-sub-object bug (§13.4 — pick
        the right retail sub-object).
      - The LIGHTNING2 commented animator (`effect_old.cpp:3690-3784`) —
        separate, dead, different asset (`Magic\Lightning.I3D`).
```

**Definition of done:** casting the Lightning spell on an enemy spawns a
bolt that grows from the caster's left hand toward the enemy along caster
facing (~10 ticks growth at 2 anchors/tick to a raycast-determined endpoint
≤100 anchors away), holds at full length for ~20 ticks with continuous
per-vertex crackle jitter (±13 wu at every 4th anchor, splined intermediates)
and a backward-flowing texture U-scroll, while a cross-rotating two-half
glow disc pulses at the cast hand and a fountain of ~20 twinkling sparks
flicker at the hand throughout, an impact-spark burst lighting at the bolt's
endpoint the moment the bolt fully extends — then over ~5 ticks the bolt
shrinks from the tail at 2 anchors/tick while the glow scales ×0.8/tick and
the strip texture continues scrolling, damage is applied ONCE to the target
character on the first EXPLODE tick (DT_BURN), and the effect self-kills
when the last spark particle has aged out (~1–1.5 s after the bolt
disappears) — reading as a saturated white-hot zigzag electric strike (NOT
a translucent grey ribbon — vet blend per §7 sanity-check + §13.2), played
with `PLAY("lightning sound")` at cast spawn.
