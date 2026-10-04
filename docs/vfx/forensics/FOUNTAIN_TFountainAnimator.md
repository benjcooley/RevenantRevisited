# FOUNTAIN TFountainAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | X04 (base) — covers the 4 color variants (INVENTORY X04 base + X05/06/07/08 leaves) |
| **Class(es)** | `TFountainAnimator` (abstract base; holds Initialize/Animate/Render/RefreshZBuffer) + 4 trivial color-leaf subclasses `TCyanFountainAnimator`, `TRedFountainAnimator`, `TGreenFountainAnimator`, `TBlueFountainAnimator` (each only overrides `SetColorObject()` to set `colorobj` to one of `0,1,2,3`). Registered animator-name strings: `"CYANFONT"`, `"REDFONT"`, `"GREENFONT"`, `"BLUEFONT"` (`src/effect_old.cpp:3909,3915,3921,3927`). No paired `TEffect` subclass — the spawned object is a generic `TEffect` (the leaf is just an animator over the `"CyanFont"` / `"RedFont"` / `"GreenFont"` / `"BlueFont"` EFFECT type from `Class.Def`). |
| **Status** | forensics-complete |
| **Retail fidelity** | **retail-partial.** Asset is byte-identical (md5 `8211199ce94bb64f4f0b52b6ed94c4d8` snapshot vs shipped `data/imagery.rvi:Imagery/Misc/sparkle.i3d`); registration name strings (`CYANFONT`/`REDFONT`/`GREENFONT`/`BLUEFONT`) and per-variant builder thunks (LAB_004e4360/4380/43a0/43c0) are present and contiguous in the retail registration table — strong identity corroboration. Retail did NOT extract decompiled Init/Animate/Render bodies for these animators (the four addresses are tiny builder-registration thunks, not the class bodies). So **per-particle constants, render state, and the Render-blend choice are SNAPSHOT-ONLY**: high-confidence the effect is the snapshot's, lower confidence the per-tick numbers were untouched at ship. The blend (`SetBlendState` = Alpha on a self-lit "sparkle" sprite) is the textbook **SUSPECT** case (§7 BLEND SANITY-CHECK) — flag and visually vet. |
| **Author / Date** | vfx-forensics-agent (redo) / 2026-05-29 |
| **Family** | water/environment & magic-marker (scripted appear/disappear marker — used as Sabu's magical spawn/despawn puff and the lock-opening sparkle, **not** an actual fountain-of-water). |
| **Draws** | particle emitter — 10 ScreenAligned sparkle billboards (single sub-object per variant) rising from a small ground disc, each shrinking over its life. |
| **Archetype(s)** | (E) simple emitter — **continuous looping** rising-sparkle particles, no gravity, linear shrink-to-death, immediate respawn at floor on death. No texture animation, no associated light, no audio, no state machine. |

---

## 1. Summary

`TFountainAnimator` (registered as `"CYANFONT"`/`"REDFONT"`/`"GREENFONT"`/
`"BLUEFONT"`) is the **magical "sparkle column"** effect — a small, vertically-
rising column of 10 colored sparkle billboards that spawn on a 40×40-wu floor
disc, drift straight up at one of three speeds, shrink linearly to nothing, then
respawn on the disc with a fresh random delay. The 4 subclasses are the
**identical** animator in 4 colors; the only per-variant code is
`SetColorObject() { colorobj = 0|1|2|3; }` (`src/effect.h:805,820,835,850`),
which selects one of the 4 pre-tinted `photon` sub-objects in the shared asset
`Misc\Sparkle.I3D` (§4). Despite the class name "Fountain", the live
in-game usage is **not water decoration** — it is the scripted magic-marker
puff for Sabu the witch's vanish/appear scenes (`REDFONT`) and the chest-unlock
sparkle in the forest tomb (`CYANFONT`). The reconstruction should read as a
small, soft, colored sparkle pillar (cyan / red / green / blue) about 40 wu wide
and ≲ 40-ish wu tall, looping forever until the script `.delete`'s it. Blend in
the snapshot is straight Alpha (§7) — the sprite is a bright self-lit photon,
so this is the textbook "code says Alpha but looks like a glow" case;
recommend visually vetting against a retail capture of the dungeon Sabu scene
before committing.

---

## 2. Sources & evidence

- **Retail decomp:** **SPARSE.** No decompiled `Initialize` / `Animate` /
  `Render` body exists for `TFountainAnimator` in any of `recon/classes/`,
  `recon/classes_original/`, `recon/discovered/`, or `recon/mappings/` (verified
  by grep for `Fountain` / `TFount` / `NUM_FOUNTAIN_BUBBLES` / `FOUNTAIN_RADIUS`
  / `FOUNTAIN_SCALE_STEP` — only matches are snapshot headers + the recon's own
  `all_source_classes.txt` index). What IS present:
  - The 4 animator-name strings as data labels: `s_CYANFONT_005e10b0`,
    `s_REDFONT_005e10bc`, `s_GREENFONT_005e10c4`, `s_BLUEFONT_005e10d0`
    (`recon/classes/_data.txt:107397-107412`), each XREF'd from exactly one
    address in the registration thunk table:

    ```
    s_CYANFONT_005e10b0  XREF[1]:  004e4360 (*)
    s_REDFONT_005e10bc   XREF[1]:  004e4380 (*)
    s_GREENFONT_005e10c4 XREF[1]:  004e43a0 (*)
    s_BLUEFONT_005e10d0  XREF[1]:  004e43c0 (*)
    ```

  - And those four addresses appear contiguously in the global animator-builder
    function-pointer table at `005c532c..005c5338`
    (`recon/classes/_data.txt:55940-55946`), immediately before
    `Ribbon`/`RIBBON` at `005c533c/40`. The 0x20-byte spacing between thunks
    confirms each is a tiny `REGISTER_3DANIMATOR(...)` factory stub, NOT a full
    class body. (Compare `005c5350 → 004e53a0` = the `"sparks"` builder thunk,
    same shape — `SPARKS_TSparkAnimator.md:158-162`.)
  - The nearest decompiled class to the FONT-thunk addresses is
    `cls_0x4e4400` (404-byte struct, minimal `meth_0x4e4400` body at
    `recon/classes/cls_0x4e4400.cpp`) which sits 0x40 bytes after the BLUEFONT
    thunk — likely the next class in the file, not `TFountainAnimator` itself.
  - The shipped binary's `TEffect` base = `cls_0x5a47f0` (60-byte slim base,
    `recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml`) is what each Fountain
    leaf inherits-from at the object side.
- **Pre-release (snapshot — authoritative for behavior):**
  `src/effect_old.cpp:3786-3927` (the entire `TFountainAnimator` + 4 leaves):
  - `Initialize`: `:3797-3813`
  - `Animate(bool draw)`: `:3822-3852`
  - `Render()`: `:3861-3887`
  - `RefreshZBuffer()`: `:3890-3903`
  - 4 leaf registrations: `:3909` `CYANFONT/TCyanFountainAnimator`, `:3915`
    `REDFONT/TRedFountainAnimator`, `:3921` `GREENFONT/TGreenFountainAnimator`,
    `:3927` `BLUEFONT/TBlueFountainAnimator`.
- **Class declarations:** `src/effect.h:754-852` —
  - Constants `NUM_FOUNTAIN_BUBBLES=10`, `FOUNTAIN_SCALE_STEP=.15`,
    `FOUNTAIN_RADIUS=20` at `:758-760`.
  - Base class fields `p[10]`, `scale[10]`, `rise[10]`, `framenum[10]`,
    `colorobj` at `:767-773`.
  - The 4 trivial leaves at `:799-852` (each is just
    `SetColorObject() { colorobj = N; }`).
- **Asset:** `Misc\Sparkle.I3D` registered 4 times in `legacy/Class.Def:2023-2026`
  (under EFFECT TYPES "CyanFont"/"RedFont"/"GreenFont"/"BlueFont"); file at
  `legacy/Imagery/Misc/Sparkle.I3D` (15,992 B). **Byte-identical** to the
  shipped `Imagery/Misc/sparkle.i3d` extracted from `data/imagery.rvi` (both md5
  `8211199ce94bb64f4f0b52b6ed94c4d8`, both 15,992 B — verified with
  `unzip -o data/imagery.rvi Imagery/Misc/sparkle.i3d && md5 …`).
- **Render-state helpers (authoritative):**
  - `SaveBlendState` / `RestoreBlendState` `src/effect_old.cpp:181-210`
  - `SetBlendState` `src/effect_old.cpp:221-233` — `D3DTBLEND_MODULATE` +
    `D3DBLEND_SRCALPHA / D3DBLEND_INVSRCALPHA`, `ZWRITEENABLE=false`,
    `ZENABLE=true` ⇒ NOMENCLATURE **Alpha**.
  - `SetAddBlendState` `src/effect_old.cpp:235-244` — `D3DTBLEND_DECALALPHA` +
    `D3DBLEND_ONE / D3DBLEND_ONE` ⇒ NOMENCLATURE **AdditiveStraight**.
  - (Fountain calls `SetBlendState`, NOT `SetAddBlendState`, `:3864`.)
- **`T3DAnimator` base — used for the per-frame world-transform matrix:**
  - `T3DAnimator::Animate(draw)` `src/3dimage.cpp:2573-2605` — copies
    `inst->GetPos()` to `pos`, `inst->GetFace()/256 * 2π` to `rot.Z`, sets
    `changed = true` when the instance moved.
  - `T3DAnimator::PreRender` `:2627-2640` — rebuilds the per-frame `matrix`
    from `pos/rot` (via `MakeMatrix`, `:2617-2625` — `RotZ→RotX→RotY→Trans`).
  - `T3DAnimator::RenderObject(obj)` (overload) `src/3dimage.h:467-468` —
    forwards to `T3DImagery::RenderObject(obj, state, frame, &matrix, tex,
    uselastmatrix)`; the animator's `matrix` is the **world transform** the
    per-object local transform composes with.
  - `T3DAnimator::GetObject(int)` `src/3dimage.cpp:2864-2869` — bounds-checked
    fetch from `animobjs[]` (`MAX3DANIMOBJECTS=256`, `src/3dimage.h:97`).
    Returns `nullptr` if out of range — `colorobj=0..3` is safely in range for
    Sparkle.I3D's 4 sub-objects (§4).
- **`T3DImagery::RenderObject`** `src/3dimage.cpp:1364-1500ish` — composes the
  per-object animobj `matrix` (computed by `CalcObjectMatrix` from
  pos/rot/scl + flag-encoded order, `:1271-1342`) with the animator's `pos`
  matrix to get world. Per `OBJ3D_POS1/2/3` and `OBJ3D_SCL1/2/3` ordering
  (`src/3dimage.h:101-111` + `:1308-1332`): `OBJ3D_SCL1 | OBJ3D_POS2` means
  **`Scale(scl)` is applied first, then `Translate(pos)`** (the bubble local
  transform), the result multiplied into the animator's world `pos` matrix.
- **`random(min,max)`** `src/revutils.cpp:1597-1612` — `rand() % (max-min+1) +
  min`; **inclusive** on both ends (same as the SPARKS doc records).
- **In-game callers (scripted, live in shipped Ahkuilon module):**
  - `CYANFONT`: `data/Modules/Ahkuilon_unzipped/forest.s:246` — chest-unlock
    sparkle ("FORSTYXXTOMB" object, `ADDAT 4268 23080 105 CYANFONT`).
  - `REDFONT`: `data/Modules/Ahkuilon_unzipped/dungeon.s:782-785` and
    `:903-906` — Sabu witch vanish/appear puff (NOWAIT add + .MOVE 0 0 100 +
    .DELETE, twice).
  - `GREENFONT` and `BLUEFONT`: **no live caller** in shipped modules (see
    §12 — they exist as registered animators but the green/blue variants are
    not triggered by any shipped script).
- **Sister effects consulted:**
  - `THealAnimator` `src/effect_old.cpp:730-833` — the closest analogue (also
    uses `colorobj`-style `GetObject(idx)` to pick a tinted sub-object of a
    shared `.I3D`; also `SetBlendState`/Alpha; also custom per-particle Render
    + UpdateExtents; also bubble z-rise). Differences vs Fountain: Heal uses a
    matrix-mode (`OBJ3D_MATRIX`) per-bubble with x-flat (`rot.x = -π/2`) and a
    z-rotation tied to `inst->GetFace`; Heal has a 2nd glow sub-object;
    Heal has a `HEAL_DURATION` to stop respawn. Fountain is the **simpler**
    loop variant of Heal's bubble machinery.
  - `TParticle3DAnimator` (`SPARKS_TSparkAnimator.md`) — same `Sparkle.I3D`-
    family ancestry (Sparks.i3d also has `photon`/`photon01/02/03`) and the
    same `SetBlendState`-on-bright-sprite Alpha-vs-Additive question; the
    SPARKS forensics carefully notes the "GREEN Fountain/Sparkle" effect
    as a separate effect from combat sparks — that's THIS effect.
- **Source-of-truth ranking:** pre-release `effect_old.cpp` is **authoritative**
  for all behavior. Retail corroborates *existence* + *registration name* +
  *asset binding* but not the per-tick math (see §2.1). The constants live
  entirely inside the snapshot's `#define`s (`src/effect.h:758-760`) — they
  are not in any caller, so retail-caller-grep (the technique SPARKS used)
  doesn't apply here.

### 2.1 Retail-vs-snapshot reconciliation

`src/effect_old.cpp` is the pre-release dev snapshot, not shipped. Here's the
4-sub-check verdict.

**(1) Constant grep — INCONCLUSIVE.** The 4 magic constants in `src/effect.h:758-760`
are:
- `NUM_FOUNTAIN_BUBBLES = 10` → would appear as immediate `0xa` in the retail
  init loop bound (and as `0xfffffffb`/`-5` from `NUM_FOUNTAIN_BUBBLES/2 = 5`
  in `random(-5,0)`).
- `FOUNTAIN_RADIUS = 20` → immediate `0x14` in `random(-20,20)` bounds.
- `FOUNTAIN_SCALE_STEP = .15` (double literal in the header, cast to float in
  the body, `:3838`) → float `0x3e19999a` or double `0x3fc3333333333333`.
- `rise = random(1,3)/2.0` → immediates `1`, `3`, float `0x40000000` (2.0).

Because **no decompiled `Initialize`/`Animate` body exists for
`TFountainAnimator` in the recon dump**, there is no function whose immediates
can be compared against these values. The retail's `_data.txt` is the *.data*
section (string + jump tables); the *.text* disassembly for the FONT thunks
isn't in the extracted slice. So this sub-check is **inconclusive** — we know
the FONT classes exist and register, but cannot confirm or refute the per-tick
numbers without re-decompiling `004e4360 / 4380 / 43a0 / 43c0` + their callees.

**(2) Asset identity — IDENTICAL.** Snapshot `legacy/Imagery/Misc/Sparkle.I3D`
and shipped `data/imagery.rvi:Imagery/Misc/sparkle.i3d` are **byte-for-byte
identical**: both 15,992 B, both md5 `8211199ce94bb64f4f0b52b6ed94c4d8`
(verified by `unzip -o data/imagery.rvi Imagery/Misc/sparkle.i3d && md5 …`).
The 4 photon sub-objects (§4) and the 64×64 texture did not change between
snapshot and ship. Strong corroboration that the visual identity is unchanged.

**(3) Structure / layout — partial.** No `TFountainAnimator`/`TEffect`-leaf
class entry in `recon/classes/` so a member-by-member comparison isn't
possible. The base `TEffect` candidate (`cls_0x5a47f0`, 60 B slim layout,
`recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml`) is consistent with the
snapshot's `class TEffect : TObjectInstance` ancestry, but that doesn't tell
us about `TFountainAnimator` specifically.

**(4) Registration + naming — CONFIRMED.** All 4 animator names
(`CYANFONT`/`REDFONT`/`GREENFONT`/`BLUEFONT`) live in retail `.rdata` at
`005e10b0..005e10d0` with single-XREF builder thunks at `004e4360..004e43c0`,
landing contiguously in the global animator-builder pointer table at
`005c532c..005c5338` (`recon/classes/_data.txt:55940-55946, 107397-107412`).
The order, count, and naming match the snapshot's
`REGISTER_3DANIMATOR("CYANFONT"/"REDFONT"/"GREENFONT"/"BLUEFONT", …)`
declarations at `src/effect_old.cpp:3909/3915/3921/3927` exactly.

**Verdict: retail-partial.** Effect existence + asset + the 4-variant naming
are confirmed; per-tick numbers + render-state (blend) are unverified against
retail decomp because the relevant `.text` slice isn't decompiled. **Use the
snapshot values, but treat blend + per-tick speed as the high-risk dimensions
when visually matching against a retail in-game capture (§12).** §3 marks each
constant accordingly.

---

## 3. Constants

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `NUM_FOUNTAIN_BUBBLES` | 10 | count | `src/effect.h:758` | snapshot-only (no retail body; existence corroborated by 4 contiguous registration thunks) |
| `FOUNTAIN_RADIUS` | 20 | wu (half-extent of the X/Y spawn disc — full disc is 40×40 wu) | `src/effect.h:760` | snapshot-only |
| `FOUNTAIN_SCALE_STEP` | 0.15 | wu/tick (scale decrement per Animate; from `2.0` to 0 ≈ 13–14 ticks) | `src/effect.h:759` (declared as `.15` → double literal, cast to `(float)` at `:3838`) | snapshot-only |
| initial bubble scale | 2.0 | wu (uniform xyz scale at spawn) | `src/effect_old.cpp:3808, 3847` | snapshot-only |
| initial bubble pos.x / pos.y | `random(-20, 20)` (inclusive) | wu (object-local) | `src/effect_old.cpp:3804-3805, 3843-3844` | snapshot-only |
| initial bubble pos.z | 0.0 | wu (floor of the disc) | `src/effect_old.cpp:3806, 3845` | snapshot-only |
| per-bubble rise rate `rise` | `random(1, 3) / 2.0` ⇒ ∈ {0.5, 1.0, 1.5} | wu / tick (only positive) | `src/effect_old.cpp:3807, 3846` | snapshot-only |
| initial `framenum` (start-delay) | `random(-NUM_FOUNTAIN_BUBBLES/2, 0)` = `random(-5, 0)` (inclusive) | ticks (negative = "wait", incremented per tick to 0 then >0 = active) | `src/effect_old.cpp:3809, 3848` | snapshot-only |
| `colorobj` (per-variant) | 0 (Cyan) / 1 (Red) / 2 (Green) / 3 (Blue) | sub-object index into `Sparkle.I3D` (`photon`/`01`/`02`/`03`) | `src/effect.h:805, 820, 835, 850` (leaf `SetColorObject()`); used at `effect_old.cpp:3866` | **yes (retail)** — 4 variant names exist as `s_CYANFONT/REDFONT/GREENFONT/BLUEFONT` with builder thunks `004e4360/4380/43a0/43c0`, asset has matching 4 sub-objects `photon`/`photon01`/`photon02`/`photon03` (§4) |
| `RefreshZBuffer` size_x | 100 | screen px | `src/effect_old.cpp:3897` | snapshot-only |
| `RefreshZBuffer` size_y | 75 | screen px | `src/effect_old.cpp:3898` | snapshot-only |
| `RefreshZBuffer` y-offset | `+30 − size_y` = `−45` (i.e. patch top is 45 px above projected effect Y) | screen px | `src/effect_old.cpp:3901` | snapshot-only |
| sim-tick base (engine-wide) | 24 Hz (original engine integer-frame stepping) | Hz | engine convention (NOMENCLATURE §6) — Fountain inherits it (`Animate(bool draw)` called once per tick) | snapshot |

**Notes on the constants:**
- Each bubble's *visible lifetime* = scale 2.0 → 0 at `0.15/tick` = **~13.33
  ticks** of visibility (after its negative `framenum` has counted up to 0).
- Each bubble's *rise distance over its visible life* = `rise * 13.33` ∈
  {6.67, 13.33, 20} wu. So a single sparkle rises ~7–20 wu before vanishing —
  the column is **short**, not tall. (Combined with the 0..5-tick start delay,
  the *average* sparkle is visible for ~13 ticks ≈ 0.55 s at 24 Hz.)
- The effect **loops forever**: there is no kill-on-done. The dying scene
  scripts (`forest.s` `TOGGLE PAUSE = 1`, `dungeon.s` `redfont.delete`) are
  responsible for terminating the effect. (Compare H02 Heal which gates respawn
  on `HEAL_DURATION`, `effect_old.cpp:748`.)
- `framenum` semantics: starts at `random(-5, 0)` and **always** increments by
  1 per tick (`:3830`). The body that does the rising + shrinking + respawn
  only runs when `framenum > 0` (`:3832`). So `framenum` ≤ 0 = "wait", `> 0` =
  "alive, rising/shrinking". On respawn the new `framenum` is again
  `random(-5, 0)` — a fresh stagger.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Sparkle | `legacy/Imagery/Misc/Sparkle.I3D` (shipped: `data/imagery.rvi:Imagery/Misc/sparkle.i3d`, byte-identical) | 15,992 B (md5 `8211199ce94bb64f4f0b52b6ed94c4d8`) | the sparkle/photon imagery — STILL (2D-billboard) I3D containing **4 sub-objects** (one per color variant) sharing **one 64×64 texture**. ALSO carries 3 named sounds (`missilelaunch`, `missilefly`, `missileexplode`) at the file head — leftover from the asset's missile-system origin (the asset is reused by other effects too). | EFFECT type registered 4 times in `legacy/Class.Def:2023-2026` under `"CyanFont"` / `"RedFont"` / `"GreenFont"` / `"BlueFont"` (each pointing to `Misc\Sparkle.I3D`); loaded by the OBJCLASS_EFFECT registry when the scripted `add <name>` runs (`Class.Def` line is the source of truth for the imagery binding); bound by `T3DAnimator` |

**Sub-objects (4)** — read directly from the I3D object table at file offset
`0x85c` (the count word `04 00 00 00` at `0x85c` followed by four 32-byte name
records, `xxd` of `0x854..0x8d0`):

| # | name (offset) | role | used by variant |
|---|---------------|------|-----------------|
| 0 | `photon` (`0x85c`) | sparkle billboard quad (color 0 — cyan-tint) | `TCyanFountainAnimator` (`colorobj = 0`) |
| 1 | `photon01` (`0x87c`) | sparkle billboard quad (color 1 — red-tint) | `TRedFountainAnimator` (`colorobj = 1`) |
| 2 | `photon02` (`0x89c`) | sparkle billboard quad (color 2 — green-tint) | `TGreenFountainAnimator` (`colorobj = 2`) |
| 3 | `photon03` (`0x8bc`) | sparkle billboard quad (color 3 — blue-tint) | `TBlueFountainAnimator` (`colorobj = 3`) |

(Mapping #↔colorobj↔variant inferred by *order of registration* in
`effect_old.cpp:3909-3927` matching the leaf `SetColorObject()` indices 0..3 in
`effect.h:805..850`. The sub-object **names** are `photon`/01/02/03 — the
color "cyan/red/green/blue" labelling is the snapshot's assignment, not in the
asset itself. The actual cyan-vs-red-vs-green-vs-blue read is set by each
sub-object's authored vertex DIFFUSE color and/or per-sub-object texture
sub-rect, **not** by any per-frame tint from the animator** — Fountain writes
**no** per-vertex color in its Render path (§6, §7).)

**Texture (1, shared across all 4 sub-objects):**

- **Dimensions:** 64 × 64 (header at `0x49c`: `40 00 00 00 40 00 00 00`).
- **Format:** 16-bit (RGB565 or 1555), single image — body type tag `0x10`
  ("STILL") at `0x108`.
- **Animation:** **none** (no `framehtexs` array, no UV-scroll). Static.
- **Atlas layout:** the single texture is *not* a 2×2 atlas of 4 cells in
  the way Sparks.i3d is — `Sparkle.I3D` has 4 named sub-objects but only 1
  texture. The 4 color variants are realized by **per-sub-object authored
  vertex DIFFUSE color** (and/or per-sub-object UV sub-rects into a single
  source bitmap; the snapshot Animate/Render never writes a color/tint and
  uses MODULATE blending, so any color difference between variants MUST be
  baked into the sub-objects' verts). The reconstruction agent should:
  1. Decode the I3D's per-sub-object UV rects via `GetObjVerts` (the SPARKS
     forensics's technique, `SPARKS_TSparkAnimator.md:255-261`) to see if
     each photon0..3 maps to a *different sub-rect* of the 64×64 texture (a
     baked color atlas), OR if all 4 share `0..1` UV but the verts have
     different baked diffuse RGB.
  2. Either way, **render the chosen sub-object as authored** (its own UVs +
     its own vertex colors). Do NOT substitute a procedural sparkle, and do
     NOT tint per-particle from spell/character data — the color identity
     comes from the asset.

The 64×64 texture itself contains the sparkle sprite (a small bright photon-
family blob, same lineage as `Misc\Sparks.i3d`). Body size in the file: 6,868
B (`d4 1a 00 00` at `0x10c`), of which the texture takes 64×64×2 = 8,192 B
worth of decoded pixels — actually the body holds the texture data + 4
sub-object headers/vert/face/UV blocks within those 6,868 B (compressed/
quantized; precise per-block parse not needed for forensics, but the *total*
file size + sub-object count + texture WH are the load-bearing facts).

**Side-channel sounds in the asset:** the I3D file head (offsets `0x18..0xff`)
declares **3 named sounds** carried with the imagery —
`missilelaunch`/`missilefly`/`missileexplode` (visible as null-terminated
ASCII at file `0x20`, `0x60`, `0xb0`). These are leftovers from the asset's
missile-system reuse and **are not played by Fountain** (the
`TFountainAnimator::Animate`/`Render` paths make no audio call — §11). Note
their presence so a reconstruction's loader doesn't mistakenly fire them.

**The effect loads a real asset — do NOT substitute a procedural sparkle
billboard.** The 4 per-variant tinted photon sub-objects ARE the visual
identity. (See [[feedback-no-standins]].)

---

## 5. Spawn & emit

- **Trigger semantics:** **continuous looping.** All 10 bubbles are seeded at
  once in `Initialize` (`:3802-3810`); each, on dying (scale ≤ 0), respawns
  immediately at floor with a fresh `random(-5,0)` start delay (`:3841-3849`).
  The effect runs forever until the script `.delete`'s it (§12).
- **Count per trigger:** **10** concurrent bubbles at all times
  (`NUM_FOUNTAIN_BUBBLES = 10`).
- **Initial direction / distribution:** **uniform random on a 40×40-wu floor
  disc** (actually a *square*, not a disc — `pos.x` and `pos.y` are each
  `random(-20, 20)` independently). All `rise` motion is straight `+z` —
  there is no horizontal velocity, no jitter, no swirl.
- **Emit anchor convention:** the *object* is placed at the scripted
  `ADDAT x y z <name>` world coordinates (forest.s `4268 23080 105`, dungeon.s
  `10001 9458 144` / unspecified-current for the second). Particles are
  seeded **relative to the object origin** (`p[n] = random-square + z=0`),
  so the column rises from a 40×40-wu patch centered on the scripted
  `(x,y,z)` point. (Z=0 in the particle local frame means *at the object's
  base*, not absolute floor; the object's own z is what the script supplies.)
- **Coordinate space:** particle `p[n]` is **object-local** (offset from the
  effect's world position). Render composes `(scl=scale[n], pos=p[n])` then
  multiplies with the animator's world `matrix` (built from `inst->GetPos`,
  see §7) — so each bubble draws at world-`(inst.pos + p[n])` scaled by
  `scale[n]`. (The animator's `rot.Z` from `inst->GetFace` rotates the bubble
  layout if the effect's invoker has a face, but for these scripted spawns
  the invoker is just a positioned TEffect with face=0 by default.)
- **Spread / jitter:** position ±20 wu on x and y (the 40×40 square), no z
  spread (always 0). `rise` ∈ {0.5, 1.0, 1.5} wu/tick (3 quantized speeds).
  Start delay ∈ {0,1,2,3,4,5} ticks. No directional spread (always straight
  up).

### Spatial diagram

```
   wz (up)
   │            ScreenAligned sparkle billboards (face camera),
   │            colored per variant (one of 4 photon sub-objects)
   │       *
   │      *  *    ← 10 sparkles, each pos = random in 40x40 square,
   │     * *  *      rise straight +z at random {0.5, 1.0, 1.5} wu/tick,
   │    *  *  *      shrink scale 2.0 → 0 at 0.15/tick (~13.3 ticks),
   │   *  *  * *     RESPAWN at floor on death (continuous loop)
   │  ·  · · ·  ·
   └──────────────── wx
  ╱   object origin = ADDAT (x,y,z)   (script-supplied)
 ╱    base = 40x40 wu square in local x,y (random(-20,20) each axis)
wy

per-bubble rise range over its ~13.3-tick life: 6.7..20 wu (a SHORT column)
no horizontal velocity, no jitter — purely vertical motion
respawn point is random again each cycle (so the column "twinkles", doesn't track)
```

---

## 6. Behavior & per-frame logic

The full per-particle machinery lives entirely in
`TFountainAnimator::{Initialize, Animate, Render}` plus the 4 leaves'
trivial `SetColorObject`. No state machine, no phases, no sub-emitters, no
audio, no light.

### 6.1 Seeding (`Initialize`, `effect_old.cpp:3797-3813`)

```
Initialize():                              // effect_old.cpp:3797
    T3DAnimator::Initialize()              //   base setup: animobjs[], matrix, etc.
    for n in 0..NUM_FOUNTAIN_BUBBLES-1:    //   n=0..9 (NUM=10)         :3802
        p[n].x = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)   // ±20 wu  :3804
        p[n].y = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)   // ±20 wu  :3805
        p[n].z = 0.0                                                   //  :3806
        rise[n] = random(1, 3) / 2.0       //   ∈ {0.5, 1.0, 1.5}      :3807
        scale[n] = 2.0                     //   uniform xyz             :3808
        framenum[n] = random(-NUM_FOUNTAIN_BUBBLES/2, 0)  // random(-5, 0)  :3809
    SetColorObject()                       //   leaf: colorobj = 0|1|2|3   :3812
```

All 10 bubbles are seeded simultaneously, each with an independent rise speed
(3 discrete values) and an independent negative start delay (0..5 ticks). The
leaf's `colorobj` is set ONCE at init — it never changes during the effect's
life.

### 6.2 Per-tick update (`Animate(bool draw)`, `effect_old.cpp:3822-3852`)

```
Animate(draw):                             // effect_old.cpp:3822
    T3DAnimator::Animate(draw)             //   base: pos/rot mirror inst, matrix flag  :3824

    for n in 0..NUM_FOUNTAIN_BUBBLES-1:    //                                 :3827
        framenum[n]++                      //   ALWAYS advance (incl. waiting bubbles)  :3830

        if framenum[n] > 0:                //   bubble is alive               :3832
            p[n].z += rise[n]              //   rise straight up              :3835
            scale[n] -= FOUNTAIN_SCALE_STEP   //   shrink (0.15 per tick)     :3838

            if scale[n] <= 0:              //   died — respawn on the disc   :3841
                p[n].x = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)   //       :3843
                p[n].y = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)   //       :3844
                p[n].z = 0.0                                          //      :3845
                rise[n] = random(1, 3) / 2.0    //  fresh rise speed          :3846
                scale[n] = 2.0                  //  back to full              :3847
                framenum[n] = random(-NUM_FOUNTAIN_BUBBLES/2, 0)  // fresh delay  :3848
```

- **Velocity integration:** purely additive on z; no euler/gravity/drag.
- **Scale curve:** linear ramp `2.0 → 0` at `0.15/tick` (~13.33 ticks).
  **NOT a fade-in/out:** the bubble is fully scaled at spawn, then shrinks
  monotonically. No alpha curve — alpha is whatever the texture/diffuse hold.
- **No rotation animation.** `rise[n]` is the only animated parameter (sort of
  — `scale` is too, but it's monotonically decaying).
- **No kill-on-done:** when a bubble dies it *respawns*. The effect itself
  never sets `OF_KILL` — termination is the script's responsibility (§12).
- **No `Animate(draw)` gating:** the body runs every tick regardless of the
  `draw` flag (`draw` is forwarded to the base but never consulted by Fountain
  for early-out). So bubbles continue to rise/shrink even when off-camera
  (consistent with a looping ambient effect).

### Temporal diagram (one bubble's life-cycle)

```
state ┤                                    
alive │              ╱──╲                   
      │             ╱    ╲                  
wait  │  ──────────╱      ╲___ respawn ── ╱── … (loops forever)
      │   ↑↑↑ framenum<0 ↑           ↑
      │   incrementing   scale 2.0   scale 0
      └──────────────────────────────────── ticks
      0..5 ticks   |    ~13.33 ticks
       start delay |     visible life
                   ▲
                   framenum first goes > 0  (drawn from this tick on)

z position
20 ┤                  *
   │                * 
   │              *      ← rise ∈ {0.5, 1.0, 1.5} wu/tick (3 speeds)
   │           *
 0 ┤ ────────*────────────  spawn here, respawn here
   └──────────────────── ticks
                ▲
                drawn while framenum>0 AND scale>0

scale
2.0┤ ────●
   │      ╲
   │       ╲___          linear shrink 0.15/tick
   │           ╲___
 0 ┤              ●──── pop off + respawn
   └──────────────────── ticks
   start    ~13.3 ticks
```

The whole column "twinkles": each of the 10 sparkles is on its own loop, with
3 quantized rise speeds + a 0..5-tick start-stagger reseeded on each respawn,
so the visual is a constantly-renewing fountain of small rising/shrinking
sparkles — never empty, never bursty.

---

## 7. Rendering (original render state + geometry)

- **What it draws:** for each `framenum > 0` bubble, **one billboard quad =
  the chosen `photon[colorobj]` sub-object of `Sparkle.I3D`**, placed at the
  bubble's local position and uniformly scaled. The other 3 sub-objects are
  NOT drawn (Fountain overrides the base `T3DAnimator::Render()` walk-all-
  objects loop with a custom Render that calls `GetObject(colorobj)` once and
  renders only that). The body (`effect_old.cpp:3861-3887`):

```
Render():                                   // effect_old.cpp:3861
    SaveBlendState()                        //                             :3863
    SetBlendState()                         //   Alpha (MODULATE + SRC_ALPHA / INV_SRC_ALPHA)  :3864
    obj = GetObject(colorobj)               //   pick the one tinted sub-object  :3866

    for n in 0..NUM_FOUNTAIN_BUBBLES-1:     //                              :3868
        if framenum[n] > 0:                 //   skip waiting bubbles       :3870
            Get3DImagery()->ResetExtents()  //                              :3872

            obj->flags = OBJ3D_SCL1 | OBJ3D_POS2   // scale-first, translate-second  :3874
            obj->scl.x = obj->scl.y = obj->scl.z = scale[n]   //            :3875
            obj->pos = p[n]                                   //            :3876

            RenderObject(obj)               //   == T3DImagery::RenderObject(obj, state, frame, &matrix, -1, false)  :3878

            UpdateExtents()                 //                              :3880

    RestoreBlendState()                     //                              :3884
    return true
```

- **Blend mode:** **Alpha (per code).** `Render` calls `SetBlendState()`
  `effect_old.cpp:3864` → `D3DTBLEND_MODULATE` + `D3DBLEND_SRCALPHA` /
  `D3DBLEND_INVSRCALPHA` + `ZWRITEENABLE=false` + `ZENABLE=true`
  (`:221-233`). In NOMENCLATURE this is **Alpha**.

  **BLEND SANITY-CHECK — SUSPECT.** This is the textbook "code says Alpha but
  the sprite is a bright self-lit sparkle on dark background" case the
  protocol flags (§7 bullet). The `photon` sub-objects in `Sparkle.I3D` are
  the same family as `Sparks.I3D`'s photons — bright glints on (presumably)
  chroma-key-black or low-alpha-coverage backgrounds. A bright-on-black
  sparkle under MODULATE Alpha (which respects the texture's alpha channel)
  reads as a **soft translucent dot**, whereas if the texture is
  bright-on-near-black with weak alpha (the cheap "bake the glow into RGB,
  rely on additive to mask the black" approach common to the late-90s D3D
  engine), Alpha will read as **dim, with visible dark fringes**, and the
  effect's intended **glow** read would require **AdditiveStraight** (`ONE/
  ONE`) — the same suspicion SPARKS forensics records about its own combat
  burst (`SPARKS_TSparkAnimator.md:418-437`). Specific risk factors here:

  1. The retail `TFountainAnimator::Render` body is NOT decompiled (§2.1) —
     the blend choice is **snapshot-only / unverified-against-shipped**.
  2. Sister effects with similar bright-on-dark glints flip to additive:
     `TPhotonAnimator`, the symbol-glow / pillar-of-fire family
     (`SetAddBlendState` callers grep `effect_old.cpp:1273, 1915, 2398,
     3097, …`).
  3. The developer feedback in
     `[feedback-vfx-one-effect-onscreen]` + the project memory
     `feedback-vfx-color-health-signal` both flag that a "pale/missing
     color" read on a port = wrong blend.

  **Reconstruct snapshot-faithful Alpha first, but visually vet against the
  retail dungeon Sabu scene (§12) — if the column reads dim/washed/with
  dark fringes, flip to AdditiveStraight (`SetAddBlendState`) and the
  effect is the recommended fix.** Record both readings.

  (Developer-history note: SPARKS forensics specifically calls out the green
  Fountain/Sparkle as an effect that probably IS additive
  (`SPARKS_TSparkAnimator.md:434-437` — *"the additive read applies to that
  effect (if/when reconstructed), not here"*) — that observation is exactly
  this effect; treat it as a strong prior for AdditiveStraight after a
  visual vet.)
- **Lit vs self-lit:** **Unlit / self-lit.** Fountain writes no per-vertex
  color, never zeroes the material, and never calls a scene-lighting helper.
  It draws the imagery's authored verts/texture as-is. The 4 sub-objects'
  baked diffuse colors are what give Cyan/Red/Green/Blue their hue under
  MODULATE. Classify **Unlit** (NOMENCLATURE §4).
- **Depth / Z:** **TestNoWrite.** `SetBlendState` sets `ZENABLE=true,
  ZWRITEENABLE=false` (`effect_old.cpp:224-225`). `RefreshZBuffer`
  (`:3890-3903`) restores scene Z over a fixed **100×75 px** patch centered
  on the projected effect origin, shifted up by 45 px so the patch sits
  *above* the floor anchor (since the bubbles rise above it):
  ```
  size_x = 100; size_y = 75                                  // :3897-3898
  WorldToScreen(effect_pos, screen)                          // :3900
  RestoreZ(screen.x - 50, screen.y - 45, 100, 75)           // :3901
                       └─ size_x/2 ─┘  └─ -size_y+30 ─┘
  ```
  (The `+0` in the source `(screen.x - (size_x/2))+0` is dead. The y term
  `screen.y - size_y + 30 = screen.y - 45` shifts the patch top 45 px above
  the projected anchor, leaving 75 px of Z-restore vertically — enough to
  cover ~13 ticks of rising sparkles at ~1.5 wu/tick projected to screen.)
  This is the same pattern as Heal, Createfood, Cure, etc.
- **Orientation:** **ScreenAligned** (camera-facing billboards). Render sets
  *only* `obj->scl` and `obj->pos` (`OBJ3D_SCL1 | OBJ3D_POS2`); it never
  sets `obj->rot` and never touches `rot.x` (no `-π/2` ground-tip). The
  `photon` sub-objects keep their authored STILL-billboard facing. Default
  particle/sparkle orientation (NOMENCLATURE §2; AGENT_GUIDE §4.2.1.6 —
  *"fire sparks … should always face the camera"*).
- **Per-quad transform:** scale + translate only.
  `OBJ3D_SCL1 | OBJ3D_POS2` ⇒ `M_local = Scale(scale[n]) ·
  Translate(p[n])` (in the order encoded by `CalcObjectMatrix`,
  `src/3dimage.cpp:1308, 1314, 1317` — POS-1 / SCL-1 are applied first,
  POS-2 / SCL-2 second). Then `M_world = M_local · animator.matrix` (the
  animator's `pos` matrix from `T3DAnimator::Animate`, mirroring
  `inst->GetPos()` + `inst->GetFace()`'s z-rotation). For these scripted
  spawns the invoker face is 0, so `animator.matrix` is purely a translate
  to the object's world `(x,y,z)`.

  No per-quad rotation. No per-quad spin. No per-quad random orientation
  jitter. The 10 bubbles all face the camera identically and uniformly
  scale from `2.0` down to `0`.
- **Per-vertex color packing:** **NONE written by the effect.** Verts come
  from `Sparkle.I3D` as authored, with each sub-object's baked vertex
  DIFFUSE color (the cyan/red/green/blue tint that distinguishes the 4
  variants). Under MODULATE the sampled texture pixel is multiplied by the
  authored diffuse — so picking `photon02` (colorobj=2) draws the green-tinted
  variant. The animator adds nothing.
- **Cull / wireframe / fill / texture wrap:** untouched — inherits the
  `SaveBlendState`-restored scene values for `D3DRENDERSTATE_CULLMODE`
  (which `SaveBlendState` saves at `:186`; `SetBlendState` doesn't change
  it). Fountain calls neither `SetTextureStageState` nor a custom
  cull/clamp. So whatever Scene3D's last cull/wrap was, that's what bubbles
  use.
- **Per-bubble draw loop:** runs `Get3DImagery()->ResetExtents()` /
  `UpdateExtents()` **per-bubble** (`:3872, :3880`) — the bound rect is
  recomputed each draw, presumably so the sub-frame Z-restore patch and the
  dirty-rect tracking land on the union of all 10 bubble rects.

---

## 8. Texture animation

**N/A — none.** The 4 photon sub-objects share a single static 64×64 STILL
texture (§4). No `framehtexs` array, no `SetTextureFrame` call, no UV
scroll. `Render` never mutates `tu/tv` and only writes `scl` + `pos`. All
motion is positional (rise) + scale (shrink), not UV/flipbook.

---

## 9. Associated light

**N/A — none.** `TFountainAnimator::Animate`/`Render`/`Initialize`
(`effect_old.cpp:3786-3903`) make **no** `AddPointLight` / `Scene3D.AddLight*`
/ `Scene3D.LightAffectObject` call beyond what `T3DAnimator::PreRender` does
generically (`:2631`, which only *samples* the existing scene lighting at the
effect's pos to fold into vertex lit verts — but the Fountain Render path
overrides via `SetBlendState`+self-lit verts, so even that is effectively
inert here).

The scripts that fire this effect (`forest.s:246`, `dungeon.s:782,903`)
do **not** wrap it in a `.LIGHT` or `AddLight` script verb either. **The
sparkle column does not light the scene in the original.**

---

## 10. Color

- **Source:** **the authored `Misc\Sparkle.I3D` photon sub-object** (§4) —
  each of the 4 sub-objects has a different baked-in vertex DIFFUSE tint
  (and/or different UV sub-rect) that produces the cyan / red / green /
  blue identity. The leaf class's `colorobj = 0|1|2|3` picks which
  sub-object renders.
- **Exact values:** **READ THE ASSET.** The tints are not in code — they're
  in `Sparkle.I3D`'s sub-object vertex blocks. Per the asset header layout
  (object name table at file `0x854..0x8c0`, body data following), the
  reconstruction agent must decode each photon's vertex DIFFUSE (via
  `GetObjVerts`) or per-sub-object UV sub-rect to get the precise RGB. The
  **expected visual** (from class names) is:
  - `photon` / colorobj=0 = **CYAN** (light blue-green)
  - `photon01` / colorobj=1 = **RED** (warm red, presumably saturated)
  - `photon02` / colorobj=2 = **GREEN** (saturated green — this is the one
    SPARKS forensics flagged as the "green sparkle" reference that's NOT
    combat sparks)
  - `photon03` / colorobj=3 = **BLUE** (saturated blue)
- **Expected visual:** **richly saturated, bright per-variant color** — this
  is exactly the situation the project memory `feedback-vfx-color-health-
  signal` describes: the effect is designed around having a specific saturated
  color identity per variant, so a pale/washed reconstruction = broken port
  (stand-in, default fallback, wrong blend, wrong lit-mode, chroma-key
  miss). The CYAN sparkle on a chest unlock should read as a visible cyan
  glow; Sabu's RED puff should read as a clearly red magical signature.
- **Normalization / boosts:** none. No `NormalizeColors` call, no hilt
  brightness math, no spell-color injection.

---

## 11. Audio coupling

**No audio coupling.** `TFountainAnimator::{Initialize, Animate, Render}`
make **no** `PlayWave` / `PLAY()` / `PlaySound` call. (Confirmed by reading
the full bodies `:3786-3903` — no sound-related symbol appears.)

Note: the asset `Sparkle.I3D` *carries* 3 named sounds in its header
(`missilelaunch`, `missilefly`, `missileexplode`) — these are leftover from
when this asset was used by the missile effect family (also confirmed by the
scripts NOT triggering them with Fountain). **They are not part of this
effect** and should not fire when Fountain spawns. A reconstruction loader
that auto-plays imagery-side sounds on object-creation would incorrectly
play `missilelaunch` here — don't do that.

The scripts that fire Fountain (`forest.s`, `dungeon.s`) also don't play a
companion sound at the spawn site (no `PLAYWAVE`/`PLAYSAMPLE`/`PLAY` verb
adjacent to the `ADD redfont` / `ADDAT CYANFONT` lines in the read region).

---

## 12. Triggers & in-game appearance

- **Registration:** 4 builders in `src/effect_old.cpp` —
  - `:3909  REGISTER_3DANIMATOR("CYANFONT", TCyanFountainAnimator)`
  - `:3915  REGISTER_3DANIMATOR("REDFONT",  TRedFountainAnimator)`
  - `:3921  REGISTER_3DANIMATOR("GREENFONT", TGreenFountainAnimator)`
  - `:3927  REGISTER_3DANIMATOR("BLUEFONT", TBlueFountainAnimator)`
  Each maps a registered animator-name string to its leaf class; the
  TEffect side comes from `legacy/Class.Def:2023-2026` under EFFECT TYPES
  (`"CyanFont"`/`"RedFont"`/`"GreenFont"`/`"BlueFont"`, each pointing at
  `Misc\Sparkle.I3D`). Spawn from scripts is `add <name>` (which builds a
  generic `TEffect` of that TYPE and resolves the animator via
  `CreateAnimator`).
- **Spawned by:** scripted scenes in the shipped Ahkuilon module —
  - **CYANFONT — chest-unlock sparkle.**
    `data/Modules/Ahkuilon_unzipped/forest.s:237-256` (object
    `"FORSTYXXTOMB"`):
    ```
    OBJECT "FORSTYXXTOMB"
    BEGIN
        ALWAYS
        BEGIN
            IF THIS.STAT LOCKED = 0
            BEGIN
                STYXXCHEST.TOGGLE NOWALK = 0
                STYXXCHEST.TOGGLE INVISIBLE = 0
                PLAYER.DELINV ROSE
                ADDAT 4268 23080 105 CYANFONT          // ← here
                TOGGLE PAUSE = 1
            END
            …
        END
    END
    ```
    A one-time cyan sparkle column at world `(4268, 23080, 105)` when the
    Styxx tomb chest unlocks; the `TOGGLE PAUSE = 1` immediately exits the
    handler, so the effect is **never explicitly deleted** by the script —
    it stays running on the chest until the player leaves the area / the
    sector unloads.
  - **REDFONT — Sabu vanish/appear puff (× 2).**
    `data/Modules/Ahkuilon_unzipped/dungeon.s:782-786` (first scene,
    Sabu5 dialog beat):
    ```
    state 14
    nowait add redfont          // spawn on current object's pos
    redfont.move 0 0 100        // raise +100 wu in local z (z = "up")
    wait 24                     // 24 ticks (1 second @24Hz) of column
    redfont.delete              // remove
    ```
    `data/Modules/Ahkuilon_unzipped/dungeon.s:903-907` (second scene,
    Sabu9 — chosen-by-player dialog):
    ```
    state 11
    NOWAIT ADDAT 10001 9458 144 redfont    // spawn at world (10001,9458,144)
    REDFONT.MOVE 0 0 100                   // raise +100 in local z
    WAIT 48                                // 48 ticks (2 seconds @24Hz)
    REDFONT.DELETE
    ```
    In both Sabu scenes the script pattern is identical: spawn → vertically
    `.MOVE 0 0 100` (which the engine animates over `wait` ticks) → wait →
    delete. The effect is the magical signature of Sabu the witch's
    teleport — a red sparkle column that visibly **rises 100 wu** while the
    player watches, then disappears.

    > **Behavior note** on `.MOVE 0 0 100`: this moves the *object* (the
    > TEffect instance) +100 wu in z over the wait window — Fountain's
    > particles then rise above their object origin, which is itself moving
    > up. So the visible column gets *tall* (effect-origin-z + bubble-z, up
    > to 100 wu of object travel + bubble-life travel) — this is how the
    > short ~20-wu sparkle column visually becomes a tall rising column for
    > the witch's teleport. The reconstruction must accept the script
    > `.MOVE` driving `inst->Pos()` and have the animator continuously
    > read it (which it does via `T3DAnimator::Animate`'s
    > `inst->GetPos()` mirror, `src/3dimage.cpp:2585-2594`).
- **GREENFONT — no live caller in shipped modules.** `grep -irn
  "greenfont" data/Modules legacy/jared.def legacy/spell.def legacy/area.def`
  finds no script trigger. The animator + class registration exist
  ("vestigial" in INVENTORY terms) but the green variant is not invoked
  by any shipped script. SPARKS forensics
  (`SPARKS_TSparkAnimator.md:434-437`) discusses a "green-sparkle clip"
  observed historically — that may have been a debug spawn or an
  unshipped area; either way, treat green as a registered-but-not-fired
  variant. The asset visual (the photon02 sub-object's green tint) is
  still authoritative if reconstruction reaches for it.
- **BLUEFONT — no live caller in shipped modules.** Same situation as
  GREENFONT — registered, asset-bound, never spawned by a shipped script.
- **Vestigial?** GREENFONT and BLUEFONT are effectively vestigial. CYANFONT
  and REDFONT are LIVE (verified callers above).
- **Where to see it in the original game:**
  - **CYANFONT:** unlock the Styxx tomb chest in the Ahkuilon forest area
    (the tomb labeled FORSTYXXTOMB; `forest.s:237`). Specific world coord
    `(4268, 23080, 105)`.
  - **REDFONT:** trigger one of the Sabu witch dialog scenes in the
    Ahkuilon dungeon (the dungeon.s handler at `:782` is the Sabu5
    beat → `state 14` say-sequence; the `:903` handler is the Sabu9
    chosen-dialog branch — pick any of the three player-choice options
    BLOOD / CAUSE / HEAD and the column appears after state 11).
  - **GREENFONT / BLUEFONT:** not triggered in shipped content. The asset
    sub-objects (photon02 green, photon03 blue) are still authored and
    usable.

---

## 13. Gaps & uncertainties

- **13.1 No retail body for the FONT animators (§2.1).** The 4 builder
  thunks at `004e4360 / 4380 / 43a0 / 43c0` are the only retail evidence
  for the FOUNTAIN family. The actual `Initialize`/`Animate`/`Render`
  bodies were not in the recon's decompile slice. Consequence: every
  per-tick number in §3 (counts, radii, scale step, rise speeds, start
  delay) is **snapshot-only**, unverified against ship. **Risk dimensions
  for visual vetting:** rise speed (could be re-tuned), scale step (could
  be re-tuned), bubble count (could differ), and especially the **blend
  state** (§7 SUSPECT — top snapshot-drift risk per protocol).
- **13.2 BLEND IS THE TOP UNKNOWN (§7).** The snapshot calls
  `SetBlendState()` = Alpha on a bright-on-dark photon sprite — classic
  SUSPECT case. The retail `TFountainAnimator::Render` body wasn't
  decompiled. Strong prior (from SPARKS forensics + sister-effect blend
  patterns + developer comment) is that the GREEN variant in particular
  reads additive in retail footage. Recommend the reconstruction land
  Alpha first (snapshot-faithful), then VISUALLY VET against a retail
  Sabu-dungeon capture; if dim/washed/dark-fringed, flip to
  AdditiveStraight (`SetAddBlendState` semantics). Either outcome is
  *consistent* with this forensics doc.
- **13.3 Exact per-variant color.** The 4 photon sub-objects' baked vertex
  DIFFUSE colors (or per-sub-object UV sub-rects into a single colored
  source) are inside `Sparkle.I3D`'s sub-object data blocks. Forensics
  reads the names (`photon`/`01`/`02`/`03`) and confirms 4 distinct
  sub-objects share 1 64×64 texture; the precise RGB per sub-object
  should be decoded during reconstruction (via `GetObjVerts` or by
  parsing the I3D sub-object vert blocks). Not a behavioral unknown —
  just "render the real sub-object as authored and verify the tint reads
  cyan / red / green / blue."
- **13.4 Texture layout — single-tex-shared vs 4-cell atlas.** §4 records
  that all 4 sub-objects share *one* 64×64 texture. Whether the per-variant
  color comes from
  (a) **baked vertex DIFFUSE only** (all 4 sub-objects sample the same UV
      sub-rect, just with different multiplied-in vertex color), or
  (b) **4 different UV sub-rects** into the same texture (a baked color
      atlas — different sub-rects hold different-colored sparkles), or
  (c) some hybrid,
  was not determined from the texture-region byte pattern alone. The
  reconstruction should READ the per-sub-object UVs off the I3D geometry
  to settle which (SPARKS forensics's lesson from
  `SPARKS_TSparkAnimator.md:240-262`). Either way, MODULATE Alpha + correct
  sub-object selection produces the right color.
- **13.5 Asset-side leftover sounds.** `Sparkle.I3D` carries 3 sound names
  (`missilelaunch`/`missilefly`/`missileexplode`) in its head. These are
  vestigial from the asset's missile-system origin and **must not fire** on
  Fountain spawn — confirmed by reading the Fountain bodies. A naive
  loader that auto-plays imagery-side sounds would misfire here.
- **13.6 `inst->GetFace()` rotation.** `T3DAnimator::Animate` rotates the
  whole animator matrix by `inst->GetFace() / 256 * 2π` around Z
  (`src/3dimage.cpp:2596-2601`). For scripted FONT spawns the invoker has
  no character — `face` defaults to 0 — so this z-rotation is effectively
  null, and the 40×40 spawn square is axis-aligned to world. If a future
  caller spawns a Fountain on a character (with non-zero face), the
  whole column would rotate around z, which would visibly change the
  spawn-square's orientation relative to world. Not a snapshot issue
  for shipped content (no character-attached FONT call exists), but
  worth noting for the reconstruction's invariants.
- **13.7 GREEN / BLUE are dormant.** No shipped script spawns them. The
  reconstruction must still build them (registration + animator) for
  completeness — they could be invoked by mod content or by future patches
  — but visual ground-truth for green/blue can only come from the asset
  sub-objects, not from in-game footage. (`grep` of all module scripts +
  the def files in `data/Modules/Ahkuilon_unzipped/` and `legacy/` found
  zero call sites.)
- **13.8 No state-machine / no termination.** Fountain has no internal
  termination — it loops forever until the script `.delete`'s the object
  or the sector unloads. The CYANFONT chest-unlock case has no explicit
  `.delete` either (the script just `TOGGLE PAUSE = 1`s the handler and
  exits) — the cyan sparkle column on the unlocked chest is *persistent*
  for the remainder of the player's visit to that area. The reconstruction
  must honor this (no "auto-die after N seconds" guard).
- **13.9 Frame-rate independence (project policy).** Per
  `feedback-framerate-independent-anim`, the reconstruction must convert
  the per-tick values to per-second and integrate against real delta-time
  rather than gating to 24 Hz. Specifically:
  - `rise[n]` (wu/tick @ 24Hz) → wu/sec by `× 24`: spawned ∈ {12, 24, 36}
    wu/sec.
  - `FOUNTAIN_SCALE_STEP` (per tick @ 24Hz) → per-sec by `× 24 = 3.6 /
    sec` (scale 2.0 → 0 takes 2.0/3.6 ≈ 0.556 s).
  - `framenum`-as-start-delay (ticks) → seconds (`/ 24`): ∈ {0, 0.042,
    0.083, 0.125, 0.167, 0.208} s.
  - The `framenum++` rate is the sim-tick rate (24 Hz). Reconstruction
    should accumulate delta-time and step the start-delay/scale/rise math
    in continuous units.

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Sparkle.I3D (15,992 B, md5 8211199ce94bb64f4f0b52b6ed94c4d8;
      retail-identical to data/imagery.rvi:Imagery/Misc/sparkle.i3d). The asset
      has 4 STILL billboard sub-objects: 0=photon, 1=photon01, 2=photon02,
      3=photon03, sharing one 64x64 RGB565 texture. NO procedural sparkle
      stand-in. (§4)
- [ ] Implement the SHARED animator (TFountainAnimator base) once, with 4 trivial
      leaf classes (TCyanFountainAnimator/Red/Green/Blue) whose only override is
      SetColorObject() returning 0/1/2/3. Register the 4 animator names CYANFONT
      / REDFONT / GREENFONT / BLUEFONT against their leaf classes. (§2, §6.1)
- [ ] Wire the 4 EFFECT TYPEs CyanFont/RedFont/GreenFont/BlueFont in the
      reconstruction's Class.Def-equivalent registry, all pointing at
      Misc\Sparkle.I3D (mirror legacy/Class.Def:2023-2026). (§4, §12)
- [ ] Seed 10 bubbles at Initialize, each with:
        pos.x ∈ random(-20,20), pos.y ∈ random(-20,20), pos.z = 0 (object-local)
        rise ∈ random(1,3)/2 = {0.5, 1.0, 1.5} wu/tick (3 quantized speeds)
        scale = 2.0 (uniform xyz)
        framenum ∈ random(-5,0) (negative start delay, ticks)
      Inclusive random ranges. Use the leaf's colorobj per variant. (§6.1)
- [ ] Per-tick update (FRAMERATE-INDEPENDENT — convert per-tick rates to
      per-sec, integrate by real dt; do NOT step at 24Hz, see §13.9):
        framenum advances at 24 ticks/sec (continuous "elapsed-since-stagger" sec)
        if framenum > 0:
            p.z += rise * dt (wu/sec scale: {12, 24, 36})
            scale -= 0.15 * 24 * dt = 3.6 * dt   (scale 2.0 → 0 in ~0.556 s)
            if scale <= 0: respawn with fresh random pos/rise/scale/framenum (§6.2)
- [ ] Loop forever; effect has NO internal kill. Script .delete (or sector
      unload) is the only termination. The CYANFONT chest-unlock case is
      PERSISTENT — no explicit delete, the column stays until area unload. (§6.2, §12)
- [ ] Core geometry: ScreenAligned (camera-facing) billboard per live bubble,
      placed at p[n], uniformly scaled by scale[n]. Compose local transform
      Scale(scale[n]) · Translate(p[n]), then multiply with animator.matrix
      from T3DAnimator::Animate (inst->GetPos() + inst->GetFace() z-rotation).
      No per-bubble rotation, no per-bubble spin. (§5, §7)
- [ ] Blend = Alpha (SetBlendState → MODULATE + SRC_ALPHA/INV_SRC_ALPHA).
      *** SUSPECT — visually vet against retail (§7, §13.2). *** If the column
      reads dim/washed/dark-fringed in real footage, flip to AdditiveStraight
      (SetAddBlendState → DECALALPHA + ONE/ONE) — the GREEN variant is
      especially likely to need this. Record both readings and pick the one
      matching the retail Sabu-dungeon capture. (§7)
- [ ] lit-mode = Unlit (no scene lighting folded in; verts authored colors
      pass through MODULATE unchanged). (§7)
- [ ] depth = TestNoWrite (ZENABLE=true, ZWRITEENABLE=false), per SetBlendState. (§7)
- [ ] orientation = ScreenAligned — the photon sub-objects' authored STILL
      billboard facing, no per-bubble rotation override. (§7)
- [ ] Variant pick = ONE photon sub-object per FONT variant, via colorobj
      (Cyan=0, Red=1, Green=2, Blue=3). NEVER tint per-frame — the color is
      baked into each sub-object's authored vertex DIFFUSE (and/or its UV
      sub-rect). Decode per-sub-object UVs/diffuse via GetObjVerts to render
      the correct cell/color; do NOT map 0..1 UV onto each quad. (§4, §6.1, §10)
- [ ] Texture animation = NONE (single STILL 64x64, no framehtexs, no UV
      scroll). All motion is positional + scale. (§8)
- [ ] RefreshZBuffer: restore scene Z over a 100x75 px patch centered on
      WorldToScreen(effect_pos), top 45 px above the projected anchor
      (size_x/2 left of projected x, size_y-30 above projected y). Per-bubble
      ResetExtents/UpdateExtents inside the draw loop tracks the dirty rect. (§7)
- [ ] Color from Sparkle.I3D per-sub-object diffuse/UV = saturated CYAN /
      RED / GREEN / BLUE. Verify the real sub-object loads with its baked
      tint (pale/washed = wrong blend / wrong sub-object / stand-in /
      chroma-key miss). (§10)
- [ ] Associated dynamic light: NONE. (§9)
- [ ] Audio coupling: NONE. The asset carries 3 leftover missile-system
      sounds (missilelaunch/missilefly/missileexplode) — these MUST NOT
      fire when Fountain spawns. (§11, §13.5)
- [ ] Sub-effects spawned: NONE. (§6)
- [ ] Trigger wiring (live):
        CYANFONT: ADDAT (4268, 23080, 105) — Ahkuilon forest tomb
                  (forest.s:246, FORSTYXXTOMB chest-unlock; PERSISTENT, no .delete).
        REDFONT:  ADDAT current-obj +0,0,100 over 24 ticks then .delete
                  (dungeon.s:782-785, Sabu5 vanish) AND
                  ADDAT (10001, 9458, 144) +0,0,100 over 48 ticks then .delete
                  (dungeon.s:903-906, Sabu9 vanish). The script .MOVE 0 0 100
                  raises the OBJECT 100wu in z while the bubble loop continues —
                  the visible column appears TALL because object pos + bubble z
                  both grow. Reconstruction must honor inst->Pos()'s scripted
                  motion. (§12, §13.6)
        GREENFONT, BLUEFONT: registered but no shipped caller (vestigial). (§12)
- [ ] Do NOT reconstruct any of: a kill-on-done timer, a fade-in/fade-out
      alpha curve, a per-bubble rotation, a gravity term, an associated point
      light, a spawn-burst (this is continuous, not one-shot). All absent in
      the original. (§6, §9)
```

**Definition of done:** spawning `CYANFONT` at a fixed world point produces
a small (40×40-wu wide), short-and-twinkling cyan sparkle column (10
concurrent sparkles, each rising ~7-20 wu over ~0.55 s, respawning at the
floor disc on death, forever-looping). Spawning `REDFONT` in the Sabu
scene and triggering the script's `.MOVE 0 0 100` raises the column over
1-2 seconds to a tall red magical-teleport signature, then `.DELETE`
removes it cleanly. The 4 variants read as visibly CYAN / RED / GREEN /
BLUE (rich saturation, not pale). The blend matches retail footage —
Alpha if footage confirms snapshot-faithful, AdditiveStraight if the
retail glow reads brighter/cleaner (likely the right answer for at least
the GREEN variant; vet per-variant). Smooth/time-based motion (no 24Hz
stepping). Draw ONE FONT effect on screen at a time when verifying
(per the project's one-effect-on-screen rule).
