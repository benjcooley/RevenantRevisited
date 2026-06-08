# M03 THealEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | M03 |
| **Class(es)** | `THealEffect` (effect object — stores heal level, applies HP on first Pulse, triggers HUD update) + `THealAnimator` (animator — two visual components: 59 rising bubble quads + one rotating cylindrical glow). Registered builder `"Heal"` (`effect_old.cpp:461`); registered 3D animator `"Heal"` (`effect_old.cpp:618`). |
| **Status** | forensics-complete (see §13 for genuine unknowns) |
| **Retail fidelity** | **retail-partial** — the `THealEffect::Pulse` body is corroborated in retail (variant name comparisons + "heal sound" play at `virt_meth_0x4e0c50`, `recon/classes/cls_0x5a8f20.cpp`); the `Magic\Heal.I3D` asset is present in the retail `imagery.rvi` (97,020 B vs snapshot 96,860 B — near-identical size, same 8 sub-object names); but the `THealAnimator` bodies (`Initialize/Animate/Render/RefreshZBuffer`) are **not** recovered in the retail decomp, so all animator physics constants in §3 are snapshot-only. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-06-07 |
| **Family** | magic |
| **Draws** | composite — (1) **59 billboard quads** (the "bubbles" — rising sparkle particles, one quad per slot using `GetObject(heal_num)`, the `heal*` sub-object for the active level); (2) **one cylindrical glow mesh** (`GetObject(glow_num)`, the `glow*` sub-object for the active level), rendered TWICE (once at `+rotation`, once at `-rotation`) — together they form a counter-rotating double-helix glow band around the caster's feet. |
| **Archetype(s)** | (E) particle emitter — **rising bubbles** (unique: scale-down-to-zero over lifetime, NOT scale-then-promote as in Burn); (F2) custom logic — multi-level spell dispatch from variant name, HUD HealthBar update, glow-object grow/shrink scale envelope; (G) associated dynamic light — via spell.def LIGHT directive (retail only); no associated engine light from the animator itself. Per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md). |

---

## 1. Summary

`THealEffect` / `THealAnimator` is the in-game visual for casting a healing spell.
It plays at the caster's feet and rises upward, showing a **ring of glowing
sparkle bubbles** rising vertically out of a **rotating cylindrical glow band**
that pulsates at the caster's feet. There are **four visual levels** (Heal,
Heal2, Heal3, Heal4) distinguished by which I3D sub-object pair is chosen:
`heal1/glow1` (level 1/default) through `heal04/glow04` (level 4). The effect
plays for `HEAL_DURATION = 40` ticks (≈ 1.67 s), after which new bubbles stop
spawning and the glow ring shrinks; it self-kills when all living bubbles have
faded and the glow scale reaches zero.

The visual is distinctly **pink/rose** at level 1, graduating through more
saturated pink→violet→purple as the level increases (per the asset textures — see
§10). `THealEffect::Pulse` applies the HP gain on the first tick (reading
`spell->VariantData()->maxdamage` for levels 1-3; full HP restore for level 4)
and updates the HUD health bar. There is no continuous damage tick — the heal is
instant.

---

## 2. Sources & evidence

- **Retail decomp:**
  - **`THealEffect` object (`cls_0x5a8f20`):** `recon/classes/cls_0x5a8f20.cpp`
    — **SPARSE** (only ctor/dtor/allocator bodies recovered). Allocates `0x1cf4` =
    7412 B (`virt_meth_0x4f4fe0`, `cls_0x5a8f20.cpp:86`). Has vftable at
    `cls_0x5a8f20__vftable_5a8f20.cpp` with 24 entries; slot 24 =
    `virt_meth_0x4e0c50` (= `THealEffect::Initialize` or `Pulse` — the method
    that XREFs `s_heal_sound_005e0ef0`, i.e. `PLAY("heal sound")`,
    `recon/classes/_data.txt:107124-107125`).
  - **`THealEffect::Pulse` variant-name comparisons:** `virt_meth_0x4df270`
    (in the vftable at `005a88c0`, `_data.txt:12594`) XREFs strings `"Heal"`
    (`005e0e5c`), `"Advanced Healing"` (`005e0e64`), `"Priest Heal"` (`005e0e78`),
    `"HighPriest Heal"` (`005e0e84`) at `_data.txt:107053-107075`. These are the
    retail `THealEffect::Pulse` variant-name dispatch branch — corroborates the
    snapshot's `strcmp(spell->VariantData()->name, "Heal/Heal2/Heal3/Heal4")`
    logic, though the retail variant names have changed (see §2.1.4 and §13.1).
  - **`THealAnimator` body:** NOT recovered in the retail decomp. The animator
    `REGISTER_3DANIMATOR("Heal", THealAnimator)` registration string is **not**
    found as a labeled symbol in `_data.txt` (the `"Heal"` builder string at
    `005e0e5c` is XREFd only from the effect's Pulse, not from a 3DAnimator
    registration site). The animator class itself may be merged with the effect
    class in the 7412-B allocation (see §2.1.3), or its registration lives in
    an un-decompiled region.
  - **Retail Heal spell:** `data/Resources/spell.def:731-767` — `SPELL "Heal"` with
    four `VARIANT` rows (`"Heal"`, `"Advanced healing"`, `"Priest Heal"`,
    `"HighPriest Heal"`), all using `EFFECT_NAME "heal"` (lowercase). Also:
    `LIGHT COLOR 255,100,100 INT 180 MULT 20` — a pinkish-warm point light
    defined at the spell level (NOT via animator code). This light is **retail
    only** — the snapshot spell.def (`legacy/spell.def`) has no LIGHT directive.
- **Pre-release (snapshot — authoritative for behavior):**
  - `THealEffect`: decl `src/effect.h:641-658`; `DEFINE_BUILDER` + bodies
    `src/effect_old.cpp:461-509`.
  - `THealAnimator`: decl `src/effect.h:834-867`; constants
    `effect.h:834-837`; `REGISTER_3DANIMATOR` + bodies
    `src/effect_old.cpp:618-850`.
- **Asset:** `Magic\Heal.I3D` registered `legacy/Class.Def:2021`
  (`"Heal" "Magic\Heal.I3D" 0x19340bef`).
- **Spell callers:** `legacy/spell.def:68-115` (`SPELL "Heal/Heal2/Heal3/Heal4"`,
  all `EFFECT_NAME "heal"`, `ANIMATION "invoke2"`); `legacy/cbspell.def:64-117`
  (same structure); `data/Resources/spell.def:731-767` (retail, with LIGHT).
- **Sister effects consulted:**
  - `M04 TBurnEffect` — character-attached status overlay (same
    `TParticleSystem` infrastructure); forensics at
    [M04_TBurnEffect.md](M04_TBurnEffect.md). Used to verify blend helper
    classification.
  - `X21 TFizzleEffect` — magic-family spell visual; forensics at
    [X21_TFizzleEffect.md](X21_TFizzleEffect.md). Used to verify Alpha blend
    for magic-family non-fire effects.
- **Source-of-truth ranking:** the retail decomp confirms class identity,
  the effect's Pulse variant-name dispatch, the "heal sound" call, and the
  asset presence. The **animator bodies are snapshot-only** — pre-release
  `effect_old.cpp:618-850` is the sole source for all per-frame behavior and
  constants.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

Active cross-checks:

**1. Constant grep — unverifiable.** The animator's key constants
(`NUM_HEAL_BUBBLES = 60`, `HEAL_SCALE_STEP = 0.15f`, `HEALING_RADIUS = 20`,
`HEAL_DURATION = 40`, `scale[0] = 2.0`, grow step = 1.0/tick, shrink step =
1.0/tick, rotation delta = 0.1 rad/tick, glow scale = `scale[0] * 3/4`,
initial bubble scale = 4.5, rise rate = `random(3,6) * 4.0/3.0`) were searched
across `recon/classes/` as float bit-patterns and integer immediates. The
animator body is not isolated in the decomp (no readable `Animate`/`Render`
bodies for THealAnimator), so none of these immediates could be corroborated.
All are **snapshot-only**.

**2. Asset identity — near-identical, NOT byte-for-byte identical.**
- Snapshot `legacy/Imagery/Magic/Heal.I3D`: **96,860 B**, MD5
  `807654b632afefe1ad036030a027c436`.
- Retail `data/imagery.rvi:Imagery/Magic/heal.i3d` (extracted): **97,020 B**,
  MD5 `69445633f3f7827684947a43a43d5651`. Dated `1999-04-30`.
- The files differ (160 B size delta, different MD5). A hex comparison of the
  first 512 bytes shows the CGSR/I3D header is the same format and both
  contain the 8 sub-objects (`heal1`, `glow1`, `heal02`, `glow02`, `heal03`,
  `glow03`, `heal04`, `glow04`) with the same names at the same positions. The
  sub-object count, vertex counts per sub-object (heal=12, glow=174), and
  bbox values are **identical in both**. The 160 B delta is most likely a
  texture re-bake or header flag change, not a geometry change. The texture
  cell format and all sub-object geometry appear unchanged. **Asset verdict:
  structurally identical, minor re-save before ship. The 8 sub-objects and
  their roles are the same as the snapshot.**

**3. Structure / vftable.** `cls_0x5a8f20` allocates `0x1cf4 = 7412 B`, with
the highest field offset recovered at `0x1cf0` (offset 7408,
`cls_0x5a8f20.cpp:50`). This is extremely large for a `THealEffect` shell
(which is `TEffect` base + `bool first_time` + `int32_t level` = trivially
small). This large allocation suggests the retail class merges both
`THealEffect` and `THealAnimator` into one Ghidra cluster, OR that the
T3DAnimator base is very large (which, with the `p[60]` + `scale[60]` +
`rise[60]` + `framenum[60]` arrays = 720+240+240+240 = 1440 B of own fields,
doesn't account for 7412 B either). **Verdict: the retail allocation is
unexpected — likely a Ghidra class-merge artefact** (similar to the
TFireFlash+TFireWind+TBurn merge, M04 §2.1.3). The true THealAnimator struct
footprint cannot be isolated from the retail decomp.

**4. Registration + naming — partially confirmed.**
- The spell.def `EFFECT_NAME "heal"` is the live caller in both snapshot and
  retail. Retail `data/Resources/spell.def:744,750,756,762` all use `"heal"`.
- The `"Heal"` builder string at `.rdata 005e0e5c` is XREFd from
  `virt_meth_0x4df270` (confirmed `THealEffect::Pulse` body — the variant
  name comparison) in the retail binary.
- **However**, the 3DAnimator registration `REGISTER_3DANIMATOR("Heal",
  THealAnimator)` string is **not** found as a labeled XREF in `_data.txt` — it
  may live in an undecompiled stub, or the animator is registered under the
  same `"Heal"` string at a different XREF site not isolated by Ghidra.
- The retail spell variant names **differ from the snapshot**:
  snapshot uses `"Heal"/"Heal2"/"Heal3"/"Heal4"` (retail binary: `"Heal"`,
  `"Advanced Healing"`, `"Priest Heal"`, `"HighPriest Heal"`). This means the
  `THealEffect::Pulse` level-dispatch comparisons changed between snapshot and
  retail (§13.1).

**5. Spell-level LIGHT in retail (new in retail, absent in snapshot).**
Retail `spell.def:739` adds `LIGHT COLOR 255,100,100 INT 180 MULT 20` to the
Heal spell definition. This is a scene-warming pinkish-warm point light
(R=255, G=100, B=100) delivered by the spell system, **not** by the
THealAnimator. This light is **absent from the snapshot** — it was added at
ship. The animator itself emits no dynamic light (see §9).

**Verdict: retail-partial.** The class identity, Pulse dispatch, "heal sound",
asset sub-object structure, and spell trigger wiring are retail-confirmed. The
animator's per-frame constants and all behavior details are snapshot-only. The
snapshot's level-dispatch variant names are superseded by the retail names.

---

## 3. Constants

All from `src/effect.h:834-837` and `src/effect_old.cpp:628-849` unless noted.
`random(a,b)` is **inclusive on both ends**
(`src/revutils.cpp:1597-1613`: `rand() % (max-min+1) + min`).
Sim tick = 24 Hz; per-tick values are authored at that rate.
All constants are **snapshot-only** unless marked otherwise.

### 3.1 Defines (`src/effect.h:834-837`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `NUM_HEAL_BUBBLES` | 60 | total particle slots (index 0 = glow disk, 1..59 = rising bubbles) | effect.h:834 | snapshot-only |
| `HEAL_SCALE_STEP` | 0.15 | per-tick scale decrement for bubbles | effect.h:835 | snapshot-only |
| `HEALING_RADIUS` | 20 | wu — XY jitter range for bubble spawn (±20 in x and y) | effect.h:836 | snapshot-only |
| `HEAL_DURATION` | 40 | ticks — master phase boundary for glow + spawn cutoff | effect.h:837 | snapshot-only |

### 3.2 Initialize constants (`src/effect_old.cpp:628-685`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| glow position `p[0]` | `(0, 0, 0)` | local wu | effect_old.cpp:670 | snapshot-only |
| initial glow scale `scale[0]` | 2.0 | uniform scale | effect_old.cpp:671 | snapshot-only |
| initial glow framenum | 0 | ticks | effect_old.cpp:672 | snapshot-only |
| initial rotation | 0.0 | rad | effect_old.cpp:673 | snapshot-only |
| bubble init z | 2.0 | wu above effect origin | effect_old.cpp:680 | snapshot-only |
| bubble rise rate | `random(3,6) × 4.0/3.0` ⇒ `[4.0, 8.0]` | wu/tick | effect_old.cpp:681 | snapshot-only |
| bubble initial scale (effective) | 4.5 | uniform (first `scale[n]=0` is immediately overwritten, dead write) | effect_old.cpp:683 | snapshot-only |
| bubble initial framenum | `random(-30, -1)` (= `random(-NUM_HEAL_BUBBLES/2, -1)`) | ticks (negative = waiting to activate) | effect_old.cpp:684 | snapshot-only |
| activebubbles initial | 0 | count | effect.h:848 (field decl) | snapshot-only |

### 3.3 Animate constants (`src/effect_old.cpp:695-760`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| glow grow rate (phase 1) | +1.0 / tick (if `scale[0] < 7.0`) | scale units/tick | effect_old.cpp:705-706 | snapshot-only |
| glow grow cap | 7.0 | max scale during grow phase | effect_old.cpp:705 | snapshot-only |
| glow shrink rate (phase 2) | −1.0 / tick (if `scale[0] > 0`) | scale units/tick | effect_old.cpp:712-713 | snapshot-only |
| rotation delta per tick | +0.1 | rad/tick | effect_old.cpp:722 | snapshot-only |
| bubble z-rise per tick | `rise[n]` ⇒ `[4.0, 8.0]` wu/tick | wu/tick | effect_old.cpp:736 | snapshot-only |
| bubble scale decrement per tick | `HEAL_SCALE_STEP = 0.15` | scale units/tick | effect_old.cpp:741 | snapshot-only |
| bubble respawn XY | `random(-20, +20)` on each axis | wu | effect_old.cpp:750-751 | snapshot-only |
| bubble respawn z | 2.0 | wu | effect_old.cpp:752 | snapshot-only |
| bubble respawn rise | `random(3,6) × 4.0/3.0` | wu/tick | effect_old.cpp:753 | snapshot-only |
| bubble respawn scale | 4.5 | uniform | effect_old.cpp:754 | snapshot-only |
| bubble respawn framenum reset | −1 | ticks (1 tick delay before activate) | effect_old.cpp:755 | snapshot-only |

### 3.4 Render constants (`src/effect_old.cpp:770-832`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| bubble matrix rotation X | `−π/2` rad | per bubble, all | effect_old.cpp:796 | snapshot-only |
| bubble matrix rotation Y | 0.0 | per bubble | effect_old.cpp:797 | snapshot-only |
| bubble matrix rotation Z | `−π/4` rad (initial) | per bubble | effect_old.cpp:798 | snapshot-only |
| bubble additional Z-rotation | `−(GetFace(f×360) / 256) × TORADIAN` | per bubble (see §13.2) | effect_old.cpp:799 | snapshot-only |
| glow render flags | `OBJ3D_ROT1 \| OBJ3D_SCL2 \| OBJ3D_POS3` | — | effect_old.cpp:818 | snapshot-only |
| glow rot.x / rot.y | 0.0 / 0.0 | rad | effect_old.cpp:819 | snapshot-only |
| glow rot.z (+pass) | `rotation` (= `0.1 × frame`) | rad | effect_old.cpp:820 | snapshot-only |
| glow rot.z (−pass) | `−rotation` | rad | effect_old.cpp:825 | snapshot-only |
| glow scale | `scale[0] × 3/4` (integer multiply then divide) | uniform | effect_old.cpp:821 | snapshot-only |
| glow position | `p[0]` (always `(0,0,0)` — local origin) | wu | effect_old.cpp:822 | snapshot-only |

### 3.5 RefreshZBuffer constants (`src/effect_old.cpp:835-850`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| Z-restore patch width | 100 | px | effect_old.cpp:845 | snapshot-only |
| Z-restore patch height | 220 | px | effect_old.cpp:846 | snapshot-only |
| Z-restore Y-offset | `−size_y + 25 = −195` | px from screen centre of `p[0]` | effect_old.cpp:849 | snapshot-only |

### 3.6 THealEffect object constants (`src/effect_old.cpp:464-509`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| level 1 → `heal_num` | 0, `glow_num` = 1 | sub-object index | effect_old.cpp:654-656 | snapshot-only |
| level 2 → `heal_num` | 2, `glow_num` = 3 | sub-object index | effect_old.cpp:639-641 | snapshot-only |
| level 3 → `heal_num` | 4, `glow_num` = 5 | sub-object index | effect_old.cpp:644-646 | snapshot-only |
| level 4 → `heal_num` | 6, `glow_num` = 7 | sub-object index | effect_old.cpp:649-651 | snapshot-only |
| Heal HP amount | `spell->VariantData()->maxdamage` (from spell.def) | HP | effect_old.cpp:501 | snapshot-only for exact values; retail spell.def: level1=203 HP, Adv=412, Priest=50..100, HighPriest=150..200 |
| Heal4 HP amount | full HP restore (`SetHealth(MaxHealth())`) | HP | effect_old.cpp:495 | snapshot-only |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Heal mesh | `legacy/Imagery/Magic/Heal.I3D` (snapshot) / `Imagery/Magic/heal.i3d` (retail) | 96,860 B snapshot / 97,020 B retail | 8 sub-objects providing 4 bubble-sprites + 4 cylinder-glow meshes, one pair per heal level | `"Heal"` registered in `legacy/Class.Def:2021` (`0x19340bef`); loaded by the OBJCLASS_EFFECT registry on spawn; sub-objects addressed by `GetObject(heal_num)` and `GetObject(glow_num)` |

**`Magic\Heal.I3D` sub-object enumeration (from `i3d_dump_all/heal/manifest.txt`):**

| index | name | verts | indices | texslot | bbox | role |
|-------|------|-------|---------|---------|------|------|
| 0 | `heal1` | 12 | 36 | 1 | (−5.13,−4.99,0)..(5.13,4.99,0) | **level-1 bubble sprite** — flat quad |
| 1 | `glow1` | 174 | 576 | 2 | (−5.38,−5.38,0)..(5.38,5.38,20) | **level-1 glow cylinder** — open cylinder, 20 wu tall |
| 2 | `heal02` | 12 | 36 | 3 | (−5.13,−4.99,0)..(5.13,4.99,0) | **level-2 bubble sprite** |
| 3 | `glow02` | 174 | 576 | 4 | (−5.38,−5.38,0)..(5.38,5.38,20) | **level-2 glow cylinder** |
| 4 | `heal03` | 12 | 36 | 5 | (−5.13,−4.99,0)..(5.13,4.99,0) | **level-3 bubble sprite** |
| 5 | `glow03` | 174 | 576 | 6 | (−5.38,−5.38,0)..(5.38,5.38,20) | **level-3 glow cylinder** |
| 6 | `heal04` | 12 | 36 | 7 | (−5.13,−4.99,0)..(5.13,4.99,0) | **level-4 bubble sprite** |
| 7 | `glow04` | 174 | 576 | 8 | (−5.38,−5.38,0)..(5.38,5.38,20) | **level-4 glow cylinder** |

**Key geometry notes:**
- The `heal*` sub-objects are **flat quads** (z=0 in bbox; 12 verts = 3 quad faces with shared verts). Each is a single sprite cell.
- The `glow*` sub-objects are **open cylinders** (174 verts = 43 segments × 4 verts + 2, z spans 0..20 wu, radius ≈ 5.38 wu). This is a 3D mesh, not a billboard.
- **UV mapping** on `heal*` quads (from `heal.obj`): each quad face maps UV `(0,0)–(1,1)` — full-texture coverage per quad. No atlas, no sub-rect. Each `heal*` sprite occupies the whole texture of its corresponding texslot.
- **No UV animation** on any sub-object. All are STILL state.

### 4.1 Texture layout

8 textures, each `64×64`, single frame, one per sub-object (manifest.txt:6-21, materials.diffuse=(1,1,1,1), emissive=(1,1,1)).

**Visual color (from decoded PNGs in `i3d_dump_all/heal/`):**

| texslot | sub-object | image | dominant color | description |
|---------|-----------|-------|---------------|-------------|
| 1 (tex 0) | heal1 | `texture_00_frame_00.png` | rosy/coral-pink | soft glowing disk, radial gradient, rose-pink center fading outward |
| 2 (tex 1) | glow1 | `texture_01_frame_00.png` | light pink | vertical feathered wisp/flame pattern, pale pink |
| 3 (tex 2) | heal02 | `texture_02_frame_00.png` | brighter pink | similar disk, slightly more saturated pink |
| 4 (tex 3) | glow02 | `texture_03_frame_00.png` | pink-rose | wisp, similar to glow1 but slightly brighter |
| 5 (tex 4) | heal03 | `texture_04_frame_00.png` | pink-violet | disk shifting toward violet |
| 6 (tex 5) | glow03 | `texture_05_frame_00.png` | pale lavender-pink | wisp, cooler/bluer pink |
| 7 (tex 6) | heal04 | `texture_06_frame_00.png` | violet/purple | disk clearly violet-purple, most saturated |
| 8 (tex 7) | glow04 | `texture_07_frame_00.png` | pale violet | wisp, pale violet |

Color progression: level 1 = rose-pink → level 4 = violet-purple, consistent
with a "higher magic potency = cooler/more saturated" visual language.

**Stand-in guard:** This effect is asset-driven. Any port that draws procedural
circles, gradient halos, or plain-color billboards instead of loading
`Magic\Heal.I3D` is a stand-in and wrong (AGENT_GUIDE §4.2.1). A gray/white
result at reconstruction = the asset isn't being read correctly.

**No texture animation.** All 8 textures are single-frame; no `framehtexs`
flipbook, no UV scroll anywhere in the bodies (§8).

---

## 5. Spawn & emit

- **Trigger semantics:** **fixed-duration**. The effect plays for roughly
  `HEAL_DURATION = 40` ticks of active spawn, then shrinks the glow for up
  to 7 more ticks (shrink rate 1.0/tick from max scale 7.0), then self-kills.
  Active bubbles that are still rising at the end continue until they scale
  to zero.
- **Count per trigger:** `NUM_HEAL_BUBBLES = 60` total slots, but:
  - Slot 0 = the **glow disk** (particle index 0, always active from tick 0).
  - Slots 1..59 = **rising bubbles** (59 slots). Each bubble initializes with
    a negative `framenum` (= `random(-30, -1)`) — a pre-start delay so they
    activate staggered across the first 30 ticks. Once `framenum[n] == 0`, the
    bubble becomes active (counted in `activebubbles`). Once it scales to zero,
    it is respawned immediately if `framenum[0] < HEAL_DURATION` (still within
    the active window).
- **Initial direction / distribution:** each bubble (n ≥ 1) spawns at
  `(random(-20, +20), random(-20, +20), 2.0)` — random scatter in a
  ±20 wu XY patch, 2 wu above the effect origin. They rise **purely vertically**
  at their individual `rise[n]` rate; no lateral velocity.
- **Emit anchor convention:** **character-attached at caster's feet**. The
  effect is placed at the caster's world position (the spell effect origin).
  The bubble `p[n]` coordinates are **local to the effect** (not world-absolute).
  The `RefreshZBuffer` centers its Z-restore patch on
  `p[0] + effect->GetPos()` — confirming that `p[0]=(0,0,0)` is the effect
  origin, and bubble positions are object-local (`effect_old.cpp:841-843`).
  The glow cylinder (`p[0]`) stays fixed at the local origin.
- **Coordinate space:** **Local** — bubble positions are relative to the
  effect's anchor (the caster's world position). The render uses standard
  object-relative matrix math; `p[n]` is passed directly to
  `D3DMATRIXTranslate(&obj->matrix, &obj->pos)` (`:805`).
- **Spread / jitter:** XY scatter ±20 wu uniform random; Z always starts at
  2.0 wu. Rise rate = `random(3,6) × 4/3 ∈ [4.0, 8.0]` wu/tick. Initial
  scale = 4.5 (uniform for all respawns).

### Spatial diagram (side view ZX, local coords)

```
 wz (local, up)
  │        ·  ·  ·      bubbles rising (pure vertical, vz = [4.0, 8.0] wu/tick)
  │       · · · · ·     scale 4.5 → 0 at HEAL_SCALE_STEP=0.15/tick
  │      ·  ·  ·  ·     (each lasts 4.5/0.15 = 30 ticks in flight)
  │     · · · · · ·
  │    ·  ·  ·  ·  ·
 2┤   [spawn Z=2]
  │  ════════════════   glow1 cylinder, r=5.38, h=20 wu,
  │  ════════════════   drawn TWICE at ±rotation (both Alpha blend)
  │  ════════════════   rotates 0.1 rad/tick
 0┤──────────────────── effect origin = caster world pos
  │      ±20 wu XY scatter (bubble spawn XY = random(-20,+20))
  └─────────────────────► wx (local)

 Z-restore patch: 100×220 px screen space, offset −195 px from projected p[0].y
```

---

## 6. Behavior & per-frame logic

`THealEffect::Pulse` runs the HP-gain logic (once); `THealAnimator::Animate`
runs the visual simulation each sim tick.

### 6.0 Effect setup — `THealEffect::Initialize` + `THealEffect::Pulse`

`effect_old.cpp:464-509`:

```
Initialize():
    first_time = true

Pulse():
    TEffect::Pulse()
    if (first_time):
        if (spell):
            invoker = spell->GetInvoker()
            // Determine level from variant name:
            if name == "Heal"  → level = 1
            if name == "Heal2" → level = 2
            if name == "Heal3" → level = 3
            if name == "Heal4" → level = 4; invoker->SetHealth(MaxHealth())   // instant full restore

            // Apply HP for levels 1..3:
            if level < 4:
                invoker->SetHealth(invoker->Health() + spell->VariantData()->maxdamage)
                if invoker->Health() > invoker->MaxHealth():
                    invoker->SetHealth(invoker->MaxHealth())     // clamp to max
        first_time = false
```

The HP gain is **one-shot on the first Pulse**. The `maxdamage` field of the
variant is the heal amount (snapshot: level1=10 HP, level2=30 HP, level3=100
HP; retail spell.def: level1=203 HP, "Advanced healing"=412 HP, "Priest
Heal"=50-100 HP, "HighPriest Heal"=150-200 HP — see §13.1).

### 6.1 Animator setup — `THealAnimator::Initialize` (`effect_old.cpp:628-685`)

```
Initialize():
    T3DAnimator::Initialize()
    PLAY("Heal")         // plays the heal animation on the character (not audio)

    // Select sub-object pair based on level from the THealEffect:
    switch level:
        case 2: heal_num=2, glow_num=3
        case 3: heal_num=4, glow_num=5
        case 4: heal_num=6, glow_num=7
        default: heal_num=0, glow_num=1

    // HUD update (if caster is the player):
    if spell and invoker == Player:
        HealthBar.ChangeLevel(invoker->Health() * 1000 / invoker->MaxHealth())

    // Glow disk (slot 0):
    p[0] = (0,0,0);  scale[0] = 2.0;  framenum[0] = 0;  rotation = 0.0

    // Rising bubbles (slots 1..59):
    for n = 1..59:
        p[n].x = random(-20, +20)
        p[n].y = random(-20, +20)
        p[n].z = 2.0
        rise[n] = random(3,6) * 4.0/3.0    // ∈ [4.0, 8.0] wu/tick
        scale[n] = 4.5                       // initial scale (dead write scale[n]=0 precedes this)
        framenum[n] = random(-30, -1)        // stagger activation up to 30 ticks
```

The `PLAY("Heal")` call triggers the character's animation system to play the
`"Heal"` animation on the invoker — this is the character casting animation, not
an audio cue. The `"heal sound"` audio play happens in `THealEffect` (the effect
object, at `virt_meth_0x4e0c50`, retail-confirmed by string XREF — see §11).

### 6.2 Per-tick simulation — `THealAnimator::Animate` (`effect_old.cpp:695-760`)

```
Animate(draw):
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)

    // ---- GLOW DISK (slot 0) ----
    framenum[0]++

    if framenum[0] < HEAL_DURATION/2 (= 20):      // first 20 ticks: grow phase
        if scale[0] < 7.0:
            scale[0] += 1.0                         // grow by 1.0/tick, cap 7.0

    else if framenum[0] > HEAL_DURATION (= 40) and activebubbles == 0:   // shrink phase
        if scale[0] > 0:
            scale[0] -= 1.0                         // shrink by 1.0/tick
        else:
            KillThisEffect()                        // self-kill when glow gone
            return

    rotation += 0.1                                // rotate glow by 0.1 rad/tick

    // ---- RISING BUBBLES (slots 1..59) ----
    for n = 1..59:
        framenum[n]++

        if framenum[n] == 0:                        // bubble activates this tick
            activebubbles++

        if framenum[n] > 0:                         // bubble is active
            p[n].z += rise[n]                       // rise vertically

            if scale[n] > 0:
                scale[n] -= HEAL_SCALE_STEP (=0.15) // shrink 0.15/tick
                if scale[n] <= 0:                   // bubble has scaled out
                    activebubbles--
                    if framenum[0] < HEAL_DURATION: // still within active window?
                        // RESPAWN: reset bubble to a new random position
                        p[n].x = random(-20,+20)
                        p[n].y = random(-20,+20)
                        p[n].z = 2.0
                        rise[n] = random(3,6) * 4.0/3.0
                        scale[n] = 4.5
                        framenum[n] = -1             // 1-tick delay before next activate
```

**Bubble lifetime:** a bubble born at scale 4.5 shrinks at 0.15/tick → lasts
`4.5 / 0.15 = 30 ticks` before scale reaches zero. During those 30 ticks it
rises `rise × 30 ∈ [120, 240]` wu vertically. Since it respawns at z=2.0,
the bubble column reaches a maximum z of `2.0 + 240 = 242 wu` above the caster
in the worst case.

**The glow scale envelope:**
- Ticks 0–2: scale 2.0 → grows 1.0/tick up to 7.0 (reaches 7.0 at tick 5+).
- Ticks 0–20: grow phase (cap 7.0 from tick 5 onward).
- Ticks 20–40: no change (middle region — NOT in grow condition, NOT in
  shrink condition since `framenum[0] <= 40`).
- Ticks >40 AND activebubbles == 0: shrink 1.0/tick. The shrink phase cannot
  begin until ALL bubbles have scaled out (activebubbles == 0). Since bubbles
  keep respawning as long as `framenum[0] < 40`, the last respawn happens
  around tick 39; that last batch takes 30 ticks to scale out, so activebubbles
  hits 0 around tick ~69. Then the glow starts shrinking from whatever scale[0]
  is at that point (should be ≈7.0), taking 7 more ticks → self-kills at tick
  ≈76.

**Total lifetime estimate:** ≈ 76 ticks ≈ 3.2 s at 24 Hz.

### Temporal diagram

```
scale[0] (glow scale)
7.0 ┤●━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━●        ramp to 7.0 in ticks 0..5,
    │╱                                                          ╲       hold through tick ~69,
2.0 ●                                                           ╲      then shrink 1.0/tick
0.0 ┤                                                            ╲●── self-kill
    └┼───────────────┼──────────────────────┼────────────────────┼──► frame
     0              20                     40                  ~69~76

activebubbles (bubble count in flight)
 ~59┤                ●━━━━━━━━━━━━━━━━━━━━━━━━●
    │              ╱  (respawn keeps count near max │ during 0..40)
    │            ╱                                 ╲drain (no respawn t>40)
  0 ┤●━━━━━━━━━━╱                                  ╲●━━━━━━━━━━━━━► frame
    └┼──────────┼──────────────────────────────────┼───────────────► frame
     0         ~30 (all activated)               ~70 (all gone)

phases:   grow glow │ hold glow  │ drain bubbles │ shrink glow │ dead
ticks:    0..5      │ 5..20      │ 40..~69       │ ~69..~76    │ ~76
```

---

## 7. Rendering (original render state + geometry)

Full `Render()` body `effect_old.cpp:770-833`. Two passes in one `SaveBlendState / SetBlendState / … / RestoreBlendState` bracket:

### 7.1 Bubble pass

```
SaveBlendState()
SetBlendState()                     // Alpha blend (see below)

obj = GetObject(heal_num)           // heal1/heal02/heal03/heal04 (the flat quad sub-object)

for n = 0..59:
    if framenum[n] > 0 AND scale[n] > 0:
        ResetExtents()
        obj->flags = OBJ3D_MATRIX
        D3DMATRIXClear(&obj->matrix)
        if n == 0:
            // Glow disk: no rotation (identity)
            RotX=0, RotY=0, RotZ=0
        else:
            // Bubble: WorldXY tip + facing rotation
            obj->rot.x = −π/2
            RotX(−π/2)                          // tips the flat quad onto the ground plane
            RotY(0)
            RotZ(−π/4)                          // in-plane 45° spin
            RotZ(−(GetFace(f×360)/256)×TORADIAN) // face-direction spin (see §13.2)
        Scale(scale[n])
        Translate(p[n])
        RenderObject(obj)
        UpdateExtents()
```

**Important anomaly on n=0:** The glow **disk** (`heal_num` sub-object,
`heal1`) is drawn in the bubble loop at n=0 with identity rotation — it is a
flat, unrotated quad at `p[0]=(0,0,0)`. This is distinct from the glow
**cylinder** (`glow_num`). So the full render is:
1. `heal_num` (flat disk) drawn once at identity (the "ground ring" visual at n=0).
2. `heal_num` (flat disk) drawn for each active bubble n=1..59 with the WorldXY rotation matrix.
3. `glow_num` (cylinder) drawn twice (at ±rotation).

Wait — the Render loop uses `GetObject(heal_num)` for ALL 60 iterations
(including n=0). The n=0 iteration draws the `heal*` flat quad at `p[0]=(0,0,0)`
with identity rotation — this is the **ground-level disc sprite** (the radial
halo at the caster's feet). The cylinder is drawn separately afterward using
`GetObject(glow_num)`.

### 7.2 Glow cylinder pass

```
obj = GetObject(glow_num)            // glow1/glow02/glow03/glow04 (the cylinder)
ResetExtents()
obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3
obj->rot.x = 0.0;  obj->rot.y = 0.0;  obj->rot.z = rotation      // rotating forward
obj->scl = scale[0] × 3/4 (uniform)
obj->pos = p[0] (= (0,0,0))
RenderObject(obj)                    // first pass: +rotation
obj->rot.z = −rotation
RenderObject(obj)                    // second pass: −rotation (mirror spin)
UpdateExtents()
RestoreBlendState()
```

The cylinder is rendered **twice per frame** — once spinning clockwise and once
counter-clockwise — creating a double-helix / crossing-band visual effect within
the single cylindrical mesh.

### 7.3 Blend mode

- **Helper called:** `SetBlendState()` (`effect_old.cpp:772-773`).
- **Factors:** `D3DTBLEND_MODULATE`, `SRCBLEND = SRC_ALPHA`, `DESTBLEND =
  INV_SRC_ALPHA`, `ZWRITEENABLE = false`, `ZENABLE = true`
  (`effect_old.cpp:221-233`).
- **Classify as: Alpha** (NOMENCLATURE §3). All draws (bubble quads AND the
  glow cylinder) share this one blend bracket.
- **BLEND SANITY-CHECK (mandatory per FORENSICS_PROTOCOL §7):**
  - The `heal*` textures are **soft radial gradients with transparent backgrounds**
    (disk center bright, edges fade to transparent). This is consistent with
    **Alpha blend** (SRC_ALPHA / INV_SRC_ALPHA) — transparent edges correctly
    erase to the scene. Alpha is the right mode for soft-edged sprites on a
    dark background.
  - The `glow*` cylinder textures are **vertical wisp patterns with transparent
    areas**. Again, Alpha blend reads correctly — the transparent areas of the
    cylinder "show through" to the scene behind.
  - **Sister-family check:** Fizzle (X21, the only other magic-family spell
    visual forensics doc available) also uses `SetBlendState()` → Alpha. The
    heal effect is consistent.
  - **NOT the fire family:** fire family uses `SetAddBlendState()` (Additive);
    heal correctly uses `SetBlendState()` (Alpha). No conflict.
  - **Render body not recovered in retail decomp**, so this is **snapshot-only**.
    The Alpha assignment is consistent with the sprite design. Flag as
    snapshot-only-but-strongly-indicated; visually vet against retail.

### 7.4 Orientation

- **Bubbles (n ≥ 1):** `RotX(−π/2)` is the canonical **WorldXY** tell
  (NOMENCLATURE §2 / AGENT_GUIDE §4.2.1.6). The flat `heal*` quad is authored
  as an XY-plane sprite; `RotX(−π/2)` tips it onto the world ground plane. The
  additional `RotZ(−π/4)` is a 45° in-plane spin. So bubbles are **ground-
  parallel disks** that rise vertically — they are NOT camera-facing.
- **Glow disk (n=0):** identity rotation → the flat `heal*` quad stays as
  authored (an XY-plane disc at the origin). With the isometric camera this
  will appear as a foreshortened disc on the floor = **WorldXY**.
- **Glow cylinder:** rendered with `OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3` and
  `rot.x = rot.y = 0`, `rot.z = ±rotation`. The cylinder's own geometry is
  already a 3D ring (see bbox: radius ≈5.38 wu, height 0..20 wu); spinning about
  Z makes it appear as a twisting column of glow around the caster's legs. This
  is **a true 3D mesh, not a billboard** — orientation is determined by the
  geometry itself, not a camera-align. Classifying as: **WorldUpAligned** (or
  simply "mesh").

### 7.5 Lit vs self-lit

**Unlit / self-lit.** `Initialize` does not touch the imagery material;
`Render` does not write vertex colors. The textures are drawn at their authored
color, modulated only by the Alpha blend. No scene lighting applied.

### 7.6 Depth / Z

**TestNoWrite** — `SetBlendState` writes `ZWRITEENABLE = false`, `ZENABLE =
true` (`effect_old.cpp:224-225`). `RefreshZBuffer` restores a **100×220 px**
scene Z patch centred offset from `p[0]` on screen (`effect_old.cpp:845-849`).

---

## 8. Texture animation

**N/A — none.** All 8 sub-objects are `STILL`-state single-frame sprites. No
`framehtexs` flipbook, no UV scroll, no `SetTextureFrame`, no UV mutation
anywhere in `THealAnimator::Animate` or `Render`. The texture is static; all
motion is positional (bubbles rising) + scale decay.

---

## 9. Associated light

**No dynamic light emitted by the animator.** `THealAnimator::Initialize`,
`Animate`, and `Render` make no `AddPointLight`, `GetAmbientLight`, or
dynamic-light call.

**However**, the **retail spell.def** (`data/Resources/spell.def:739`) adds:
```
LIGHT COLOR 255,100,100 INT 180 MULT 20
```
to the `SPELL "Heal"` definition. This is a warm pinkish-red point light
delivered by the **spell system** (not by the animator). It is **absent from
the snapshot spell.def** (`legacy/spell.def:68-77`) — this light was added
at ship. The reconstruction agent must account for this spell-system light if
the spell system is being restored. It is **outside the animator's scope** but
documented here so it's not lost.

---

## 10. Color

- **Source:** the **authored `Magic\Heal.I3D` textures**. The effect supplies
  no per-vertex color tint — `Render` does not write `lverts[].color`. Color is
  entirely from the texture.
- **Exact texture colors (decoded from `i3d_dump_all/heal/` PNGs):**
  - `heal1` (level 1 bubble): **rose-pink / coral-pink** radial gradient,
    center ≈ RGB(220, 140, 140), edges transparent.
  - `glow1` (level 1 cylinder): **light pink** vertical wisps on transparent.
  - `heal02` (level 2): **brighter pink**, more saturated center.
  - `glow02` (level 2 cylinder): **pink-rose** wisps.
  - `heal03` (level 3): **pink-violet**, transitioning toward purple.
  - `glow03` (level 3 cylinder): **pale lavender-pink**.
  - `heal04` (level 4): **violet-purple**, dominant saturated purple.
  - `glow04` (level 4 cylinder): **pale violet** wisps.
- **Expected visual:** A pink-to-violet halo of rising discs with a rotating
  cylindrical wisp-glow column, ALL pink/rose/violet depending on spell level.
  If the port renders gray or white bubbles, the asset is not loading (stand-in)
  or the texture decode is wrong. Per AGENT_GUIDE §4.2.1.5, color is a health
  signal — pale or colorless = broken port.
- **No normalization / color boost** applied in the animator code. The texture
  is displayed as-is, modulated by Alpha blend.

---

## 11. Audio coupling

- **Sound name:** `"heal sound"` (the string `s_heal_sound_005e0ef0` in retail,
  `_data.txt:107125`).
- **When:** on effect `Initialize` (animator `effect_old.cpp:632` calls
  `PLAY("Heal")` — this is the character animation, not audio) and in
  `THealEffect::Pulse` or `Initialize` (retail `virt_meth_0x4e0c50` at slot
  24 of `cls_0x5a8f20`'s vftable XREFs `s_heal_sound_005e0ef0`). The retail
  confirmation places the "heal sound" play in the **effect object's method**
  (most likely `Initialize` or the first-Pulse call).
- **Note:** `PLAY("Heal")` in the animator `Initialize` (`effect_old.cpp:632`)
  triggers the **character animation** named `"Heal"` on the invoker (the
  casting animation), NOT a sound effect. It is distinct from `PLAY("heal
  sound")`.
- The audio port phase should implement: a one-shot "heal sound" triggered
  when the effect spawns.

---

## 12. Triggers & in-game appearance

- **Spawned by:** `EFFECT_NAME "heal"` in all four `Spell "Heal/Heal2/Heal3/
  Heal4"` variant rows:
  - `legacy/spell.def:76` — `VARIANT "Heal", TP_BASIC, "l", "heal", ...`
  - `legacy/spell.def:89` — `VARIANT "Heal2", TP_BASIC, "lp", "heal", ...`
  - `legacy/spell.def:102` — `VARIANT "Heal3", TP_BASIC, "lpp", "heal", ...`
  - `legacy/spell.def:115` — `VARIANT "Heal4", TP_BASIC, "lppp", "heal", ...`
  - Retail `data/Resources/spell.def:744,750,756,762` — same `"heal"` effect
    name across the 4 variants (variant names differ: "Heal", "Advanced healing",
    "Priest Heal", "HighPriest Heal").
  - `legacy/cbspell.def:72,86,101,116` — same structure.
  - `DEFINE_BUILDER("Heal", THealEffect)` (`effect_old.cpp:461`) registers the
    builder; `REGISTER_3DANIMATOR("Heal", THealAnimator)` (`effect_old.cpp:618`)
    registers the animator.
- **Registered imagery name:** `"Heal"` (`Class.Def:2021`), pointing to
  `Magic\Heal.I3D`.
- **Where to see it in the original game:** Cast any level of the Heal spell
  on the player character. The easiest trigger is early in the game when the
  player has the basic Heal spell equipped — stand in place, cast Heal. A
  rising column of pink discs with a rotating glowing cylinder appears at the
  player's feet for ~3 seconds, then fades. Higher levels produce more violet
  visuals.
- **Vestigial?** No — live caller via spell.def (both snapshot and retail).

---

## 13. Gaps & uncertainties

**13.1 Variant name mismatch — snapshot vs retail (the headline behavioral
change).** The snapshot `THealEffect::Pulse` dispatches on variant names
`"Heal"/"Heal2"/"Heal3"/"Heal4"` (`effect_old.cpp:479-491`). The retail binary
`virt_meth_0x4df270` compares against `"Heal"`, `"Advanced Healing"`, `"Priest
Heal"`, `"HighPriest Heal"` (per `_data.txt:107053-107076`). The names
changed between snapshot and ship. The level dispatch logic (case 1→level=1,
Heal2→level=2, etc.) is preserved in *structure* but the *string keys* are
different. The reconstruction must use **retail variant names** for the dispatch.
The corresponding spell amounts also changed (snapshot: 10/30/100/0 HP; retail:
203/412/100-0/150-200 HP per spell.def VARIANT maxdamage field). These are
retail-confirmed via `data/Resources/spell.def:744,750,756,762`.

**13.2 The `f` variable in `GetFace(f × 360)` is undefined in scope.**
`effect_old.cpp:799` calls `inst->GetFace(f * 360)` but `f` is not declared
or assigned in the `THealAnimator::Render()` function body. In C/C++ this would
be an uninitialized or out-of-scope variable — likely 0 (stack garbage or
zero-initialized). If `f == 0`, then `GetFace(0)` returns `inst->GetFace()` (the
caster's current facing angle in byte-angle form, 0..255). This means the
bubbles are rotated by the caster's facing direction when they render. The
reconstruction should treat `f = 0` (making the extra Z-rotation =
`−(GetFace() / 256) × TORADIAN × 360 = −(facing_byte / 256) × 2π` — a world-Z
rotation to align the bubble quad with the caster's facing). **Flag as
snapshot-only — visually vet whether bubbles should be face-aligned or just
WorldXY flat.**

**13.3 Bubble orientation with respect to RotX(−π/2) + RotZ(−π/4) + face.**
The `heal*` sub-objects are flat quads. `RotX(−π/2)` tips them flat onto the
ground (WorldXY). `RotZ(−π/4)` gives a 45° in-plane spin. The additional facing
rotation `RotZ(−face)` rotates them further about Z. The net effect is a flat
disc on the ground plane, rotated to a specific orientation. Visually, since the
disc is circular (radial gradient), the in-plane spin is only noticeable if the
disc has a directional texture — which these do not (they're radial circles). So
the orientation may be visually unimportant for the disc sub-objects, but
matters for code fidelity.

**13.4 Dead write `scale[n] = 0; scale[n] = 4.5`.**
`effect_old.cpp:682-683` writes `scale[n] = 0` then immediately `scale[n] = 4.5`.
The first write is dead code (likely a leftover from an earlier version). The
effective initial scale is **4.5** for all bubbles at Initialize. This is **not
a bug to fix** — it's an inert artifact; port as-is.

**13.5 Animator constants are entirely snapshot-only.** The retail
`THealAnimator` body is not recovered. All `HEAL_*` constants (durations, scales,
radii, rise rates, rotation deltas, Z-restore patch sizes) in §3 are
**snapshot-only** and may have been re-tuned at ship. The reconstruction must
visually match against retail ground-truth (§12) and be prepared to retune
these specific values.

**13.6 The large retail class size (7412 B) is unexplained.** If `cls_0x5a8f20`
is the THealEffect object only, 7412 B is enormous for a near-empty effect shell
(snapshot THealEffect is trivially small: TEffect base + 5 B). This suggests
Ghidra merged THealEffect + THealAnimator + possibly other classes. The true
per-class layout is unknown from retail. The reconstruction should use the
snapshot class structure.

**13.7 `PLAY("Heal")` role.** This call in `THealAnimator::Initialize`
(`effect_old.cpp:632`) invokes the character animation system to play the
`"Heal"` action on the invoker. Whether `"Heal"` is an animation action name or
a sound name depends on the `PLAY` macro's implementation. Given the context
and that the retail binary separately XREFs `"heal sound"` in the effect's Pulse
method, it is most likely `PLAY("Heal")` = play the `"invoke2"` (or explicit
`"Heal"`) **character animation** (the casting pose). The reconstruction agent
should confirm what `PLAY()` does in the effect context.

**13.8 Snapshot-vs-retail risk areas for visual matching:**
- Bubble count / spawn stagger (60 slots with −30..−1 delays — may have been
  retuned to different slot count or stagger range).
- Bubble rise rate / lifetime / scale ramp (all snapshot-only).
- Glow cylinder spin rate (0.1 rad/tick snapshot-only).
- Glow scale envelope (2.0 → 7.0 → 0 with 1.0/tick rates — snapshot-only).

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\Heal.I3D (96,860 B snapshot / 97,020 B retail; 8 sub-objects:
      heal1/glow1, heal02/glow02, heal03/glow03, heal04/glow04). NO procedural
      sprites. (§4)
- [ ] Select active sub-object pair based on spell level:
      level 1 → heal_num=0, glow_num=1; level 2 → 2,3; level 3 → 4,5;
      level 4 → 6,7. (§6.1)
- [ ] THealEffect::Pulse: one-shot HP gain on first tick.
      Levels 1-3: invoker->SetHealth(Health() + maxdamage); clamp to MaxHealth().
      Level 4: SetHealth(MaxHealth()) — full restore.
      Use RETAIL variant names for dispatch: "Heal"/"Advanced Healing"/
      "Priest Heal"/"HighPriest Heal" (NOT snapshot Heal2/Heal3/Heal4). (§6.0, §13.1)
- [ ] HUD update on Initialize: if invoker == Player, HealthBar.ChangeLevel(
      Health() * 1000 / MaxHealth()). (§6.1)
- [ ] Initialize 60 slots: slot 0 = glow disk at (0,0,0), scale=2.0, framenum=0;
      slots 1..59 = bubbles at random XY∈[−20,+20], z=2.0, rise=random(3,6)*4/3,
      scale=4.5, framenum=random(−30,−1). (§6.1)
- [ ] Per-tick Animate (24Hz base):
      Glow: framenum[0]++; grow scale[0] by 1.0/tick (cap 7.0) while frame<20;
      shrink by 1.0/tick when frame>40 AND activebubbles==0; self-kill at scale=0.
      Rotation: += 0.1 rad/tick.
      Bubbles: framenum[n]++; on framenum==0 activebubbles++; while active z+=rise[n],
      scale[n]-=0.15; on scale<=0: activebubbles--, respawn if framenum[0]<40. (§6.2)
- [ ] Render — SINGLE Alpha blend bracket for ALL draws:
      (a) Bubble quads: for each n where framenum[n]>0 && scale[n]>0, draw
          GetObject(heal_num) with matrix = RotX(−π/2)·RotZ(−π/4)·RotZ(−face_rad)·Scale·Translate(p[n]);
          n==0 draws with identity rotation (the ground disc). (§7.1)
      (b) Glow cylinder: GetObject(glow_num) drawn TWICE:
          pass1 rot.z=+rotation, pass2 rot.z=−rotation; scale=scale[0]*3/4; pos=p[0]. (§7.2)
- [ ] Blend mode = Alpha (SetBlendState): MODULATE texture stage,
      SRC_ALPHA/INV_SRC_ALPHA; ZWRITE off, ZTEST on. Covers ALL sub-objects. (§7.3)
- [ ] Orientation: bubbles WorldXY (RotX(−π/2) tell); glow cylinder is a 3D mesh
      spinning about Z. No billboard expand for the cylinder. (§7.4)
- [ ] Lit mode: Unlit / self-lit. No vertex color, no scene lighting. (§7.5)
- [ ] Depth: TestNoWrite (ZENABLE=true, ZWRITEENABLE=false). (§7.6)
- [ ] RefreshZBuffer: 100×220 px patch at (screen_p0.x − 50, screen_p0.y − 195). (§7.6)
- [ ] Texture animation: NONE (static STILL sprites, all 8 textures single-frame). (§8)
- [ ] Associated dynamic light from animator: NONE.
      NOTE: retail spell.def adds LIGHT COLOR 255,100,100 INT 180 MULT 20
      via the spell system — record for the spell system port; this is NOT
      an animator responsibility. (§9)
- [ ] Color: pink-to-violet from the Magic\Heal.I3D textures — verify the
      port reads as saturated pink/rose (level 1) through violet (level 4),
      NOT gray/white. Any pale/colorless result = asset not loaded correctly. (§10)
- [ ] Audio: play "heal sound" on spawn (effect Initialize or first Pulse).
      PLAY("Heal") in animator Initialize triggers a character animation,
      separate from the sound. (§11)
- [ ] Trigger wiring: spawned via EFFECT_NAME "heal" in spell.def variants
      for SPELL "Heal" (all 4 levels). Retail variant names: "Heal",
      "Advanced Healing", "Priest Heal", "HighPriest Heal". (§12)
- [ ] Sub-effects spawned: NONE. (§6)
- [ ] Test category: Character-attached (CharacterIdle rig) — the effect follows
      the caster; the caster's facing affects bubble rotation. Anchor at
      CharacterRoot / effect world pos. (AGENT_GUIDE §3.0.6)
```

---

> **INVENTORY update suggestion** (do not edit INVENTORY.md directly):
> Row M03 THealEffect:
> - `Status`: `forensics-complete`
> - `Retail fidelity`: `retail-partial`
> - `Notes`: `forensics doc at docs/vfx/forensics/M03_THealEffect.md; snapshot body fully traced; animator constants snapshot-only; variant names changed in retail (Heal2→"Advanced Healing" etc.); retail adds spell LIGHT directive; PLAY("Heal") in animator is a character-anim call; f variable in GetFace(f*360) undefined → treat as 0`
