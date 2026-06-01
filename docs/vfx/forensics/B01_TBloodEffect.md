# B01 TBloodEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | B01 (covers B01 `TBloodEffect`, B02 `TBloodAnimator`, B03 `TBloodSystem`) |
| **Class(es)** | `TBloodEffect` (object, near-empty shell) + `TBloodAnimator` (visual bridge) + **`TBloodSystem`** (the real particle system, embedded in the animator). The mechanism lives entirely in `TBloodSystem`. |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Retail fidelity** | **retail-partial** — asset byte-identical to shipped + registration name + caller wiring + the `SetParams` directional-spray handshake (incl. the 128 angle offset, vangle 64, height 40/75) are **retail-confirmed**; the `TBloodSystem` kinematic constants (gravity 0.37, drag 0.95, splat 1.8, z-offset 20, vel 2.0, durations) are **snapshot-only** (the system code is embedded in the animator class, which the Ghidra decomp did not isolate). See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | blood/combat |
| **Draws** | particle emitter — up to `MAX_BLOODS=30` blood droplets, each one `box01..box08` sub-object of `Misc\Blood.I3D` placed at a particle position; each droplet drawn **twice** (Alpha base pass + ONE/ONE additive overlay pass). |
| **Archetype(s)** | (E) particle emitter — **directional spray** with gravity + horizontal air-drag, NOT a radial cone, NOT rising. (F2) custom 3-stage state machine (FLY → SPLAT → SHRINK). (G-as-input) scene-**lit** (samples nearby world lights, LitFlat). NO associated dynamic light *emitted* (§9). NO texture animation (§8). NO audio (§11). |

---

## 1. Summary

Blood is the combat-feedback splatter that flies off a character when it takes
a damaging hit (and in heavier bursts on impale/decapitation/death). On a hit,
`TCharacter` spawns a `"Blood"` effect at the victim and hands it a **directional
spray** descriptor (a mean horizontal angle = the attack direction + 180°, a
vertical angle, angular spread, and a droplet count). The `TBloodSystem` seeds
that many droplets at the hit height, each launched outward at `BLOOD_VEL`×(1..3)
along the spray angle; the droplets **fly** under gravity with horizontal
air-drag (growing slightly as they go), **splat** flat on reaching ground height
(`z=BLOOD_ZOFFSET=20`) where they snap to a fixed splat size and hold ~25 ticks,
then **shrink** away. The droplet sprites are the authored `Misc\Blood.I3D` box
quads (dark red), drawn **scene-lit** (a grayscale brightness sampled from the
nearby world lights + ambient multiplies the texture — blood reads darker in
shadow), and composited as an **Alpha base pass + an additive ONE/ONE overlay
pass** per droplet (the additive pass gives the wet, slightly self-luminous
sheen). There is **no per-droplet alpha fade**; the visual end-of-life is the
SHRINK scale ramp. Droplet count, max droplet size, and the "trail-fill" of
extra small droplets are all scaled by the global `ViolenceLevel`.

> **Authoritative mechanism = `TBloodSystem`** (`src/effectcomp.cpp:1182-1554`).
> `TBloodEffect` is an empty shell (just stores 6 spray params via `SetParams`);
> `TBloodAnimator` is a thin bridge that fills an `SBloodSystemParams` from those
> params + 8 mesh sub-objects and forwards `Init`/`Animate`/`Render`/`RefreshZBuffer`
> to its embedded `TBloodSystem bloods`. Two **dev-disabled alternates** exist and
> must NOT be reconstructed: the commented `REGISTER_MULTI_3DANIMATOR_("blood",
> TBloodAnimator, TParticle3DAnimator)` (`src/effect_old.cpp:4691`) and the
> commented `TBloodParticle3DAnimator` stub bodies (`src/effect_old.cpp:5008-5034`).

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/cls_0x5acaa8.cpp` — **SPARSE** for blood
  (ctors/dtors/allocators only; a Ghidra-merged cluster of TBloodEffect +
  TMistEffect + TMistFogEffect + TWaterFallEffect — see
  `recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml`). The `TBloodSystem`
  particle code (the kinematics) is **not** in this file — it lives in the blood
  *animator* class, which the decomp did not isolate. Useful retail evidence DOES
  exist for the caller side: `recon/discovered/cls_TCharacter_EffectBurst_4c85d0.cpp`
  and `recon/discovered/cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp` (the live
  blood spawn + `SetParams` immediates).
- **Pre-release (snapshot — authoritative for the mechanism):**
  - `TBloodEffect`: `DEFINE_BUILDER("Blood", TBloodEffect)` `src/effect_old.cpp:11243`;
    bodies `:11246-11253` (empty `Initialize`, base `Pulse`); class +
    `SetParams`/`GetParams` `src/effect.h:2405-2458`.
  - `TBloodAnimator`: `REGISTER_3DANIMATOR("Blood", TBloodAnimator)`
    `src/effect_old.cpp:11261`; bodies `Initialize` `:11269-11297`, `Animate`
    `:11306-11317`, `Render` `:11326-11335`, `RefreshZBuffer` `:11337-11340`; class
    `src/effect.h:2464-2486`.
  - **`TBloodSystem`** (B03 — the real system): class + structs
    `src/effectcomp.h:356-402`; constants `src/effectcomp.cpp:1164-1180`; `Init`
    `:1182-1307`, `Animate` `:1309-1370`, `DoLighting` `:1372-1413`, `Render`
    `:1415-1530`, `RefreshZBuffer` `:1532-1554`.
  - **Caller (combat on-hit):** `TCharacter::EffectBurst` `src/character.cpp:2225-2318`
    (blood branch `:2242-2261`); triggers `src/character.cpp:1842-1848`, `:1924-1925`;
    death/decap path `src/character.cpp:231-256`.
- **Asset:** `Misc\Blood.I3D` registered `legacy/Class.Def:2014`
  (`"Blood" "Misc\Blood.I3D" 0xddc4042e`); file
  `legacy/Imagery/Misc/blood.i3d` (68,820 B). Preloaded at `src/playscreen.cpp:241`.
- **Sister effects consulted:**
  - `TPulpEffect` (B04, `src/effect_old.cpp:6759-7170`) — separate gore system with
    its own `SBloodDrop`/`SBloodSplat` (`PULP_GRAVITY`); confirms blood is a
    distinct subsystem, not shared. Do not conflate `SBloodDrop` (B04) with
    `SBloodParticle` (B03).
  - Blend helpers `SetBlendState`/`SetAddBlendState`/`Save`/`Restore`
    (`src/effect_old.cpp:181-244`) — to classify the two render passes.
  - `TWaterFallAnimator` (`src/effect_old.cpp:11801-11810`) — same `GetAmbientLight`
    scene-lit pattern, corroborates the LitFlat reading.
- **Source-of-truth ranking:** the **snapshot is authoritative for the mechanism**
  (full `TBloodSystem` bodies present; retail equivalent not isolated). The
  **retail decomp is authoritative for the caller wiring + spray params** (the
  `SetParams` immediates appear directly in the retail `ResolveAttack`). Where the
  snapshot's spawn `SetParams` call is **commented out** (a port workaround,
  `character.cpp:2260`), the retail `ResolveAttack` proves the un-commented call is
  the real shipped behavior — retail wins, the live call is restored in §5/§12.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp`/`effectcomp.cpp` is a pre-release snapshot. Cross-checks:

**1. Asset identity — IDENTICAL (strong corroboration).** The snapshot asset
`legacy/Imagery/Misc/blood.i3d` (68,820 B) and the shipped asset packed in the
retail archive `data/imagery.rvi` member `Imagery/Misc/blood.i3d` (68,820 B) are
**byte-for-byte identical**: MD5 `9c0596112cd46d6e3cb91df9f97770df` for both
(`cmp` reports identical). The effect's visual asset did not change between the
snapshot and ship — the geometry, the 8 box sub-objects, and the droplet/splat
textures the effect consumes are unchanged. (`data/imagery.rvi` is a stored ZIP;
member listed via `unzip -l`.)

**2. Registration + naming — CONFIRMED.** The retail binary registers the same
`"Blood"` name and the same `Misc\Blood.I3D` asset:
- `"Blood"` builder/dispatch string at retail data `005df9c0` and `005e11e8`
  (`recon/classes/_data.txt:104235,107565`); the `005e11e8` copy XREFs into
  `cls_0x5acaa8` (`...candidate.yaml:9`).
- `"Misc\Blood.I3D"` string at `005d7470`, XREF'd from the imagery registration
  `virt_meth_0x47a660` (`recon/classes/_data.txt:86491-86492`).
- The blood leaf class allocates `0x110` = **272 bytes** with `vftable_5acaa8`
  (`cls_0x5acaa8.cpp:124,132`), consistent with a small `TEffect` shell (matches
  the snapshot's near-empty `TBloodEffect`).

**3. Caller wiring + the `SetParams` handshake — CONFIRMED (incl. the 128 offset).**
The retail combat code spawns the blood effect and calls the spray-param virtual
exactly as the snapshot intends. The `SetParams` slot is **vtable +0x200**, and
the retail `TCharacter::ResolveAttack` (`cls_0x5a7b98_..._4c1bb0.cpp`) calls it
with concrete immediates:
- Decap/impdecap squirts (`:93,125,154`): `(+0x200)(0x4b, face-0x5b, 0x40, 0x5b, 5, 1)`
  → height **75**, hangle = face−91, vangle **64**, hspread **91**, vspread **5**,
  num **1**. (Matches the snapshot's impdecap `SetParams(95, 0, 64, 255, 5, 1)`
  in family/shape: same vangle 64, vspread 5, num 1; `character.cpp:254`.)
- Impale/attack squirt (`:190`): `(+0x200)(0x28, face-0x80, 0x20, 0x20, 5, 1)`
  → **height 0x28 = 40**, **hangle = face − 0x80 = face − 128 (the 180° offset)**,
  vangle 32, hspread 32, vspread 5, num 1. The **height 40** matches the snapshot's
  `EffectBurst("blood", 40)` (`character.cpp:1843`), and **face − 128** matches the
  snapshot's commented `(GetFace() + 128) & 255` (`character.cpp:2260`) — equal mod
  256. The spawn builds `OBJCLASS_EFFECT` (0x19), `FindObjType("blood")`, pos.z +
  0x28 (`:181-190`).
- The retail `EffectBurst` (`cls_TCharacter_EffectBurst_4c85d0.cpp:13-17`) has the
  same `burning && stricmp(type,"blood")` early-out and the same `"Blood"` branch
  calling `(+0x200)(... face−0x80 ..., rand(1,5))` — the **`random(1,5)` count**
  in the snapshot's commented line is retail.

  ⇒ The snapshot's commented `SetParams(...)` at `character.cpp:2260` is a **port
  workaround** (the comment cites a vtable/typeinfo crash, not a design change).
  The retail call is **live**. Reconstruct the un-commented call.

**4. Kinematic constants — NOT corroborated (snapshot-only).** The
`TBloodSystem::Init/Animate/Render` immediates (gravity `0.37`, drag `0.95`, vel
`2.0`, splat `1.8`, z-offset `20`, durations, the `random(100,300)/100` velocity
scale, the 128 angle conversion math) do **not** appear in the recon decomp. The
float bit-patterns (`0.37f`=0x3EBD70A4, `0.95f`=0x3F733333, `1.8f`=0x3FE66666)
return **zero hits** across `recon/` (both byte orders); decimal `0.37`/`0.95`
hits land in unrelated classes (TPlayer/multiplayer), never co-occurring with the
blood render fingerprint (two-pass + `GetClosestLights`). This is because the
`TBloodSystem` code is embedded in the blood *animator* class, which Ghidra merged
into / split off from `cls_0x5acaa8` and did not produce readable bodies for
(`...candidate.yaml:26` calls for a Ghidra rerun). So the kinematics are
**best-evidence-from-snapshot, unverified against shipped**.

**Verdict — retail-partial.** The *what/where/how-triggered* is retail-confirmed
(identical asset, same `"Blood"` registration, live `SetParams` handshake with the
128 offset / vangle 64 / height 40-75 / num / random(1,5)). The *exact kinematics
inside `TBloodSystem`* are snapshot-only. Risk to reconstruction is the numeric
tuning (gravity/drag/splat-size/durations), which must be verified by visually
matching against in-game ground-truth (§12, §13). Given the byte-identical asset
and the matching param structure, the kinematics are *likely* unchanged, but that
is an inference, not a confirmation.

---

## 3. Constants

`me` below = the `SBloodSystemParams` filled by `TBloodAnimator::Initialize`
(`src/effect_old.cpp:11269-11297`) from the effect's `SetParams` values (§5). The
`BLOOD_*` defines are `src/effectcomp.cpp:1164-1180`.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| MAX_BLOODS | 30 | count (array cap) | effectcomp.h:374 | snapshot-only |
| BLOOD_VEL | 2.0 | wu/tick (base launch speed) | effectcomp.cpp:1173 | snapshot-only |
| velocity scale | `random(100,300)/100` ⇒ ×1.0..3.0 per droplet per axis | — | effectcomp.cpp:1229-1231 | snapshot-only |
| BLOOD_GRAVITY | 0.37 | wu/tick² (subtracted from vel.z each tick, FLY) | effectcomp.cpp:1165 | snapshot-only |
| BLOOD_AIRRESIST | 0.95 | ×/tick on vel.x & vel.y (FLY, horizontal drag) | effectcomp.cpp:1164 | snapshot-only |
| BLOOD_FLYSCALE | 1.0 | initial droplet scale | effectcomp.cpp:1178 | snapshot-only |
| BLOOD_FLYSCALESTEP | 0.01 | scale added per tick during FLY (slight growth) | effectcomp.cpp:1177 | snapshot-only |
| BLOOD_ZOFFSET | 20 | wu — ground/floor height; FLY ends, droplet snaps here & SPLATs | effectcomp.cpp:1172 | snapshot-only |
| BLOOD_SPLATSIZE | 1.8 | scale held during SPLAT | effectcomp.cpp:1169 | snapshot-only |
| BLOOD_SPLATDURATION | 25 | ticks held in SPLAT before SHRINK | effectcomp.cpp:1170 | snapshot-only |
| BLOOD_SPLATSCALE | 0.1 | scale removed per tick during SHRINK | effectcomp.cpp:1171 | snapshot-only |
| BLOOD_SQUIRTDELAY | 3 | ticks of start-delay added to trail-fill droplets | effectcomp.cpp:1174 | snapshot-only |
| stage codes | FLY=0, SPLAT=1, SHRINK=2 | enum | effectcomp.cpp:1166-1168 | snapshot-only |
| **SetParams height** | 40 (attack/impale), 50 (default arg), 75/95 (decap) | wu above char pos = spawn z | character.cpp:1843,2225; retail 0x28/0x4b | yes (retail: 0x28, 0x4b at ResolveAttack:190,93) |
| **SetParams hangle** | `(GetFace() + 128) & 255` (attack); 0 (decap) | byte-angle (0-255) — mean horizontal spray dir | character.cpp:2260; retail face−0x80/face−0x5b | yes (retail: face−0x80 = 128 offset, ResolveAttack:190) |
| **SetParams vangle** | 0 (attack); 64 (decap) | byte-angle — mean vertical (elevation) | character.cpp:2260,254; retail 0x40 | yes (retail: 0x40=64, ResolveAttack:93) |
| **SetParams hspread** | 80 (attack); 255 (decap) | byte-angle ± jitter on hangle | character.cpp:2260,254; retail 0x20/0x5b | retail-partial (retail uses 0x20=32 / 0x5b=91; snapshot 80/255 differ) |
| **SetParams vspread** | 20 (attack); 5 (decap) | byte-angle ± jitter on vangle | character.cpp:2260,254; retail 5 | yes (retail: 5 at ResolveAttack:93,190) |
| **SetParams num** | `random(1,5)` (attack); 1 (decap) | droplet count requested | character.cpp:2260,254; retail rand(1,5)/1 | yes (retail: rand(1,5) EffectBurst; 1 ResolveAttack) |
| me.maxsize | 2 | max droplet size index (0=sml,1=med,2=big) | effect_old.cpp:11292 | snapshot-only |
| me.zbuf | (10, 10, 0) | px — base Z-restore patch size (×scl in RefreshZBuffer) | effect_old.cpp:11279-11281 | snapshot-only |
| ViolenceLevel | 0..5 (default 5) | global — clamps count/maxsize, gates trail-fill & splat-stick | revmain.cpp:165; revenant.h:54 | yes (retail: blood/gore level is a shipped setting) |
| num clamp | `num = min(num, ViolenceLevel*2)` | count | effectcomp.cpp:1214-1215 | snapshot-only |
| maxsize clamp | `maxsize = min(maxsize, ViolenceLevel)` | size index | effectcomp.cpp:1216-1217 | snapshot-only |
| BLOOD_MOSTBLOODS | 10 | trail-fill budget tuning constant | effectcomp.cpp:1180 | snapshot-only |
| BLOOD_BLOODSPERSQUIRT | 4 | trail-fill budget tuning constant | effectcomp.cpp:1179 | snapshot-only |
| trail-fill budget | `left = max(MAX_BLOODS − (MOSTBLOODS − num)*4, 4)`; `left=0 if ViolenceLevel<4` | count | effectcomp.cpp:1211,1238-1239 | snapshot-only |
| trail-fill vel scale | `vel × random(60,75)/80` ⇒ ×0.75..0.9375 | — | effectcomp.cpp:1297-1299 | snapshot-only |
| trail-fill extra delay | `parent.delay + BLOOD_SQUIRTDELAY + random(-1,1)` | ticks | effectcomp.cpp:1300 | snapshot-only |
| splat-stick gate | FLY→SPLAT droplet dropped (`used=false`) if `ViolenceLevel<3` OR `size==0` | — | effectcomp.cpp:1341-1344 | snapshot-only |
| DoLighting ambient gain | ambient/255 × 4.0 | — | effectcomp.cpp:1400-1402 | snapshot-only |
| DoLighting DIVISOR | 1.5 (clamp + normalize of summed brightness) | — | effectcomp.cpp:1404-1407 | snapshot-only |
| FLY rotation | rot.x = −90°, rot.z = −45° | degrees | effectcomp.cpp:1469-1470 | snapshot-only |
| BLOOD_BIFDELAY/BIFSCALESTEP | 8 / 0.5 | (impact-splash "bif" — DEAD, see §6/§13) | effectcomp.cpp:1175-1176 | snapshot-only (dead code) |
| TORADIAN | π/180 | rad/deg | revdefs.h:25 | snapshot-only |

`random(min,max)` is **inclusive** on both ends (`rand()%(max−min+1)+min`,
`src/revutils.cpp:1597-1612`). All speeds/durations are authored at the intended
**24 Hz sim tick** (NOMENCLATURE §6); `Animate` runs once per tick.

> **Spread-value caveat (the only param disagreement).** The snapshot's attack
> `hspread=80, vspread=20` differ from the retail `ResolveAttack` impale immediates
> `hspread=0x20=32, vspread=5`. Both are byte-angle spreads; the difference is
> tuning, not structure. Treat the **retail** values as ship-truth for the impale
> path and the snapshot value as a development figure (flagged in §13). The decap
> path's `vspread=5` matches retail exactly.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Blood | `legacy/Imagery/Misc/blood.i3d` (= retail `data/imagery.rvi:Imagery/Misc/blood.i3d`, **identical**) | 68,820 B (MD5 `9c0596112cd46d6e3cb91df9f97770df`) | the blood droplet/splat sprite imagery — a **STILL** (2D billboard) I3D with **8 sub-objects** | registered `Class.Def:2014` under name `"Blood"` (ID `0xddc4042e`); loaded by the OBJCLASS_EFFECT registry on spawn (also preloaded `playscreen.cpp:241`); sub-objects bound by `T3DAnimator::GetObject(0..7)` |

**Sub-objects (8)** — names read directly from the I3D object table (file is
`CGSR`-magic, type tag `STILL` at offset `0x1C`): `box01, box02, box03, box04,
box05, box06, box07, box08`. They are billboard quads sharing the authored blood
sprite textures (dark-red droplets + splats, black-keyed background). The
`TBloodAnimator` maps them into **two parallel render-pass sets**
(`src/effect_old.cpp:11284-11291`):

| pass set | role | size 0 (small) | size 1 (med) | size 2 (big) | splat |
|----------|------|----------------|--------------|--------------|-------|
| **pass k=0** (Alpha base) | `s,m,b,sp` | `GetObject(4)`=box05 | `GetObject(5)`=box06 | `GetObject(6)`=box07 | `GetObject(7)`=box08 |
| **pass k=1** (additive overlay) | `s2,m2,b2,sp2` | `GetObject(0)`=box01 | `GetObject(1)`=box02 | `GetObject(2)`=box03 | `GetObject(3)`=box04 |

So the *same* droplet is drawn from box(4+size) in the Alpha pass and box(0+size)
in the additive pass — the two box sets are the two layers of one droplet's look
(a solid red core + an additive wet sheen). **Size 2 (big) and the splat (`sp`)
sub-objects are loaded but their draws are commented out** — only small/med
render (§6, §7). The splat `sp/sp2` (box08/box04) are referenced only by the dead
"bif" impact-splash block (§13).

**The effect loads a real asset — do NOT substitute a procedural blood sprite.**
The dark-red box quads + their black-keyed textures are the visual identity (§10).
The header records a texture cell of 64×120 (`0x44`: `40 00 78 00`) within a
128×160 sheet, 16-bit (RGB565).

---

## 5. Spawn & emit

- **Trigger semantics:** **one-shot burst.** All droplets (plus their trail-fill)
  are seeded at once in `TBloodSystem::Init` (`effectcomp.cpp:1182-1307`); no
  continuous emission. The effect object self-kills when every droplet finishes
  (`done` → `OF_KILL`, §6).
- **Count per trigger:** the requested `num` (attack: `random(1,5)`; decap: 1),
  then **clamped** to `min(num, ViolenceLevel*2)` and capped at `MAX_BLOODS=30`
  (`effectcomp.cpp:1214-1215,1220`). After the main droplets, a **trail-fill** loop
  adds extra *small* droplets up to a `left` budget (`:1211,1287-1306`), so the
  on-screen droplet count is `num + trail-fill` (0 if `ViolenceLevel<4`).
- **Initial direction / distribution:** a **directional spray**, NOT a radial cone
  and NOT rising. For each droplet (`effectcomp.cpp:1222-1231`):
  ```
  ha = ((hangle + random(-hspread, hspread)) & 255) * 360/256 * TORADIAN   // horizontal angle (rad)
  va = ((vangle + random(-vspread, vspread)) & 255) * 360/256 * TORADIAN   // vertical angle (rad)
  vel.x = BLOOD_VEL * cos(ha) * random(100,300)/100      // 2.0 * cos(ha) * (1.0..3.0)
  vel.y = BLOOD_VEL * sin(ha) * random(100,300)/100      // 2.0 * sin(ha) * (1.0..3.0)
  vel.z = BLOOD_VEL * sin(va) * random(100,300)/100      // 2.0 * sin(va) * (1.0..3.0)
  ```
  `hangle` is the mean spray direction in the world XY plane (byte-angle); for an
  attack it is `(victim_face + 128) & 255` — i.e. **180° from the victim's facing**,
  so blood sprays *back toward the attacker / away from the victim's front**. The
  byte-angle ± `hspread`/`vspread` jitter fans the droplets around that mean. The
  per-droplet `random(100,300)/100` multiplier gives a 1×..3× speed spread, so some
  droplets fly far and some barely leave.
- **Emit anchor convention:** the effect *object* is placed at the **victim's world
  position with z + height** (`def.pos = pos; def.pos.z += height`,
  `character.cpp:2244-2245` / `:246-247`), height = 40/50 (attack) or 75/95 (decap)
  — i.e. roughly chest/wound height. Droplets are seeded **at object-local origin
  (0,0,height)**: `pos.x=pos.y=0`, `pos.z = me.height` (`effectcomp.cpp:1226-1228`).
  Note `me.height` here is the *same* spawn-height number passed as the first
  `SetParams` arg, used both to set the object's world z (caller) and the droplet's
  local start z (system); the render later subtracts `height` back off (§7).
- **Coordinate space:** droplet `pos`/`vel` are integrated in the effect's
  **local space** (object origin = victim world pos+height); render places each box
  via `OBJ3D_MATRIX` at the local pos minus `height` (§7); `RefreshZBuffer` adds the
  effect world pos back to project (§7).
- **Spread / jitter:** horizontal ± `hspread` byte-angle, vertical ± `vspread`
  byte-angle, speed ×1..3, count `random(1,5)` then ViolenceLevel-clamped, droplet
  size `random(0, min(maxsize,2))` per droplet (`effectcomp.cpp:1225`).

### Spatial diagram

```
   wz (up)
   │              · ·          directional spray (NOT radial):
   │           ·    ·  ·       droplets fan around mean angle (ha, va),
   │         ·   · ·    ·      speed BLOOD_VEL*(1..3) — some far, some near
   │        ·  · ·  ·          gravity 0.37 ↓, horizontal drag *0.95/tick
   ▲ emit ··○·· ·               emit origin = victim_pos, z = +height(40/75)
   │ origin   ╲                 droplet local start = (0,0,height)
   │           ╲___ splat at z=BLOOD_ZOFFSET=20 (droplet snaps flat, WorldXY)
   └──────────────────── wx
  ╱   mean hangle = (victim_face + 128) → 180° from victim front
 ╱    (blood sprays back toward the attacker)
wy

top-down:   victim_front →        spray points the OTHER way  ← ← ←
```

---

## 6. Behavior & per-frame logic

The whole behavior is `TBloodSystem`, driven each tick by `TBloodAnimator::Animate`
(`effect_old.cpp:11306-11317`): it calls the base, marks `SetCommandDone(false)`,
runs `bloods.Animate()`, and when `bloods.GetDone()` it sets `OF_KILL` on the
effect object (self-destruct when all droplets are done).

### 6.1 Seeding (`TBloodSystem::Init`, `effectcomp.cpp:1182-1307`)

```
Init(me):                                       // me = SBloodSystemParams (§5)
    store animator + 8 box sub-objects (s,m,b,sp + s2,m2,b2,sp2)   // :1185-1202
    height = me.height;  eff = me.effectpos                         // :1203-1204
    bifon = (ViolenceLevel ? BLOOD_BIFDELAY : 0)                    // :1206-1209  (bif = dead, §13)
    left = max(MAX_BLOODS - (BLOOD_MOSTBLOODS - num)*BLOOD_BLOODSPERSQUIRT, BLOOD_BLOODSPERSQUIRT) // :1211
    num    = min(num,  ViolenceLevel*2)          // count clamp     // :1214-1215
    maxsize= min(maxsize, ViolenceLevel)         // size clamp      // :1216-1217

    for i in 0 .. min(num, MAX_BLOODS)-1:        // main droplets   // :1220
        ha, va = spray angles (see §5)                              // :1222-1223
        used=true; size=random(0, min(maxsize,2))                   // :1224-1225
        pos = (0, 0, height)                                        // :1226-1228
        vel = BLOOD_VEL * (cos ha, sin ha, sin va) * random(100,300)/100  // :1229-1231
        scl = BLOOD_FLYSCALE(1.0); stage=FLY(0); delay=0           // :1232-1234
        left--

    if ViolenceLevel < 4: left = 0               // no trail-fill below VL4   // :1238-1239

    // (DEAD, commented) trail bigs→med/small, meds→med/small      // :1242-1283
    // trail-fill: while (left): for each used droplet i, add a small // :1287-1306
    //   blood[num]: size=random(0,1), pos=(0,0,height),
    //               vel = blood[i].vel * random(60,75)/80,  (0.75..0.9375x)
    //               delay = blood[i].delay + BLOOD_SQUIRTDELAY + random(-1,1)
    //               stage=FLY, scl=1.0;  num++; left--
```

The trail-fill makes the spray read denser by spawning slower, slightly-delayed
small droplets riding behind the main ones. The two larger trail loops (bigs→…,
meds→…) are **commented out** (`:1242-1283`) — do not reconstruct them.

### 6.2 Per-tick update (`TBloodSystem::Animate`, `effectcomp.cpp:1309-1370`)

```
Animate():                                       // once per 24Hz sim tick
    done = true
    for i in 0 .. MAX_BLOODS-1:
        if !used[i]: continue
        if delay[i] > 0: delay[i]--; continue    // start-delay countdown
        done = false

        if stage[i] == FLY:                                         // :1324
            pos[i]   += vel[i]                                      //   integrate
            vel[i].x *= BLOOD_AIRRESIST(0.95)                       //   horizontal drag
            vel[i].y *= BLOOD_AIRRESIST(0.95)
            vel[i].z -= BLOOD_GRAVITY(0.37)                         //   gravity
            scl[i]   += BLOOD_FLYSCALESTEP(0.01)                    //   slight growth in flight
            if pos[i].z <= BLOOD_ZOFFSET(20):                       //   hit "ground"
                pos[i].z = 20; stage[i] = SPLAT; count[i] = 0
                if ViolenceLevel < 3:  used[i] = false              //   low-violence: no splat-stick
                if size[i] == 0:       used[i] = false              //   smalls never splat

        else if stage[i] == SPLAT:                                  // :1347
            scl[i] = BLOOD_SPLATSIZE(1.8)                           //   snap to splat size
            count[i]++
            if count[i] > BLOOD_SPLATDURATION(25): stage = SHRINK; count = 0

        else if stage[i] == SHRINK:                                 // :1357
            scl[i] -= BLOOD_SPLATSCALE(0.1)                         //   ramp down
            if scl[i] <= 0: scl=0; used[i] = false                  //   droplet gone

    bifon--; bifscale += BLOOD_BIFSCALESTEP   // (bif = dead, §13)
```

- **Velocity field:** ballistic with *horizontal* air-drag. Each tick the XY
  velocity is multiplied by 0.95 (so horizontal travel decays), while Z gets
  gravity. Net path: a forward arc that flattens out as it falls.
- **No alpha curve.** Nothing modifies a per-droplet alpha. The only "fade" is the
  SHRINK scale ramp (0.1/tick from 1.8 → 0). Brightness varies only via DoLighting
  (scene light), not via age.
- **The 3-stage state machine** is the effect's identity — see the temporal
  diagram. Note the **two early-kills at FLY→SPLAT**: at `ViolenceLevel<3` *no*
  droplet sticks (they vanish on landing), and **size-0 (small) droplets never
  splat** regardless of violence (they always vanish on landing). So the surviving
  ground splats are only the med/big droplets at `ViolenceLevel>=3`.
- **Death:** `done` stays true only when every used droplet has finished;
  `TBloodAnimator::Animate` then kills the object (`effect_old.cpp:1313-1316`).

### Temporal diagram

```
scale (scl) over a droplet's life:
1.8 ┤            ●─────────●            ← SPLAT: snap to 1.8, hold 25 ticks
    │           ╱           ╲
1.0 ┤●────────╯              ╲          ← FLY: 1.0 + 0.01/tick (slight grow)
    │ (flying, arc)           ╲___
0.0 ┤                             ●──   ← SHRINK: −0.1/tick to 0, then killed
    └──────────────────────────────── ticks
     stage:  FLY (until z<=20) │ SPLAT(25) │ SHRINK(~18) │ dead

low-violence (VL<3) or size==0:  FLY ──(z<=20)──► killed immediately (no SPLAT/SHRINK)
```

---

## 7. Rendering (original render state + geometry)

`TBloodAnimator::Render` wraps `bloods.Render()` in `SaveBlendState()` /
`RestoreBlendState()` (`effect_old.cpp:11326-11335`). `TBloodSystem::Render`
(`effectcomp.cpp:1415-1483`) draws **each used droplet twice** (loop `k=0,1`):

```
for each used droplet i:
    for k in 0,1:
        if k==0:  RestoreBlendState(); SetBlendState()            // Alpha base
        else:     RestoreBlendState();
                  TEXTUREMAPBLEND = D3DTBLEND_MODULATE            // additive overlay
                  SRCBLEND = D3DBLEND_ONE;  DESTBLEND = D3DBLEND_ONE
                  (ZWRITE/ZENABLE NOT touched — inherit k=0's: ZWRITE off, ZENABLE on)
        object = (k==0 ? {sml,med} : {sml2,med2})[ size==1 ? med : sml ]   // big/splat draws commented
        object->flags = OBJ3D_MATRIX | OBJ3D_VERTS
        matrix = Scale(scl) * (FLY ? RotateX(-90°)*RotateZ(-45°) : identity)
                            * Translate(pos.x, pos.y, pos.z - height)
        DoLighting(pos, object)             // writes per-vertex grayscale (§10)
        RenderObject(object); UpdateExtents()
```

- **What it draws:** for each live droplet, one `Blood.I3D` box sub-object (a
  billboard quad), positioned/scaled by an explicit object matrix — drawn once per
  pass (two passes), so **2 quads per droplet**.
- **Blend mode (original) — two passes, traced via the helper + the inline state:**
  - **k=0 (base):** `SetBlendState()` (`effect_old.cpp:221-233`) = texture stage
    `D3DTBLEND_MODULATE`, `SRCBLEND=SRC_ALPHA`, `DESTBLEND=INV_SRC_ALPHA`,
    `ZWRITEENABLE=false`, `ZENABLE=true` → **Alpha (modulated)** (NOMENCLATURE §3).
    This is the solid red droplet body.
  - **k=1 (overlay):** the system does **NOT** call `SetAddBlendState()`; it sets
    the states inline (`effectcomp.cpp:1435-1438`): `TEXTUREMAPBLEND=MODULATE`
    (note: MODULATE, *not* the DECALALPHA that `SetAddBlendState` uses),
    `SRCBLEND=ONE`, `DESTBLEND=ONE` → **AdditiveStraight (ONE/ONE) with a MODULATE
    texture stage**. The `ZWRITEENABLE`/`ZENABLE` lines are commented out, so the
    additive pass inherits k=0's depth states (ZWRITE off, ZENABLE on). This adds a
    self-luminous wet sheen on top of the alpha body. **Trace note:** classify k=1
    as AdditiveStraight, but record that the texture stage is MODULATE (so the
    grayscale DoLighting vertex color modulates the additive contribution too) and
    that it is hand-rolled, not `SetAddBlendState`.
  - The outer `SaveBlendState`/`RestoreBlendState` (animator) and the per-pass
    `RestoreBlendState()` (system) bracket/reset the states so each pass starts
    clean.
- **Lit vs self-lit:** **scene-lit (LitFlat).** `DoLighting`
  (`effectcomp.cpp:1372-1413`) samples up to `MaxLights` (default 1) closest world
  lights at the droplet's world pos + ambient×4, clamps/normalizes to a single
  grayscale value `r=g=b`, and writes it to all 4 quad verts via `D3DRGB`. Under
  the MODULATE texture stage this multiplies the blood texture — so blood is
  **darker in shadow, brighter in lit areas** (it does NOT glow on its own; the
  additive pass adds sheen, but the magnitude is still gated by the scene-lit
  vertex color). This is the same `GetAmbientLight()` scene-lit family as the
  water/waterfall effects (NOMENCLATURE §4, knowledge 03 §2).
- **Depth / Z:** **TestNoWrite** — `ZENABLE=true`, `ZWRITEENABLE=false` (set by k=0
  `SetBlendState`, inherited by k=1). `RefreshZBuffer` repairs scene Z under each
  droplet (below).
- **Orientation:** **conditional.** During **FLY**, the droplet quad is tipped to
  the ground (`RotateX(-90°)`) and spun in-plane (`RotateZ(-45°)`) — i.e. **WorldXY**
  ground-flat with a 45° in-plane rotation (`effectcomp.cpp:1467-1471`). During
  **SPLAT/SHRINK** there is NO rotation applied — the matrix is just Scale ×
  Translate, leaving the box at its authored facing (the splat lies as authored).
  (The `-90°` is the canonical WorldXY tip; NOMENCLATURE §2.)
- **Per-quad transform:** explicit `OBJ3D_MATRIX` = `Scale(scl) · [FLY rot] ·
  Translate(pos.x, pos.y, pos.z − height)`. The `− height` cancels the local start
  height so the droplet's *world* z is the object origin + the integrated delta.
- **Per-vertex color packing:** `DoLighting` writes `D3DRGB(r,g,b)` (grayscale)
  into `object->lverts[0..3].color` (`effectcomp.cpp:1409-1412`). `OBJ3D_VERTS` is
  set so the effect's own lvert buffer is used.
- **Big & splat draws are commented out:** the `else if (size==2) object=big`
  lines (`effectcomp.cpp:1447-1448,1455-1456`) and the entire "bif" impact-splash
  block (`:1485-1529`) are commented. Only **small (box0/box4) and med (box1/box5)**
  droplets render. So even though Init can assign `size==2`, no big sprite is drawn
  — a size-2 droplet falls through to the `sml`/`sml2` default object. Reconstruct
  small + med only (§13).

---

## 8. Texture animation

**N/A — none.** The 8 box sub-objects are STILL sprites (no `framehtexs` flipbook,
no `SetTextureFrame`/`textureframe[]` call anywhere in the blood path) and the
render never mutates `tu/tv` (no UV scroll). All motion is positional (the droplet
flying) plus the matrix scale/rotation. Texture is static. (Knowledge 03 §8.6: this
is neither flipbook nor UV-scroll.)

---

## 9. Associated light

**N/A — none emitted.** `TBloodSystem` makes no `AddPointLight`/dynamic-light call;
blood does not light the scene. It is a *consumer* of scene light (DoLighting,
§7/§10), not a *source*. (Contrast fire/explosions which emit light.)

---

## 10. Color

- **Source:** the **authored `Misc\Blood.I3D` box textures** (dark red), modulated
  by a **scene-lit grayscale** per-vertex color from `DoLighting`. The effect
  supplies no hue of its own — the red is the texture; the vertex color only sets
  brightness.
- **DoLighting math** (`effectcomp.cpp:1372-1413`):
  ```
  GetClosestLights(worldpos) -> l1,l2,l3
  r = brightness(l1) + (MaxLights>1 ? brightness(l2) : 0) + (MaxLights>2 ? brightness(l3) : 0)
  r += (GetAmbientLight()/255) * 4.0
  r = min(r, 1.5) / 1.5            // clamp + normalize to 0..1
  g = b = r
  lverts[0..3].color = D3DRGB(r, g, r)   // grayscale
  ```
  With default `MaxLights=1`, brightness comes from the single closest light +
  ambient×4, clamped to [0,1] after the /1.5 normalize.
- **Exact values:** texture = dark/blood red (RGB565, black-keyed bg); vertex tint
  = grayscale 0..1 (scene brightness). Final pixel ≈ blood-red × scene-brightness,
  with the k=1 additive pass adding the same texture×brightness again (sheen).
- **Expected visual:** rich **dark red** droplets/splats that read brighter in lit
  rooms and darker in shadow — never pink, never gray, never additive-bright-red
  on their own. A washed/pale/orange result at reconstruction = broken port
  (likely a procedural stand-in instead of the real Blood.I3D sprite #1, wrong
  blend on the base pass, missed black-key, or DoLighting not wired so the texture
  shows un-modulated). The additive overlay is *subtle sheen*, not a glow — if
  blood looks like glowing red embers, the k=1 pass is being treated as the only
  pass or with the wrong factors (see AGENT_GUIDE §4.2.1.5).
- **Normalization / boosts:** the DIVISOR=1.5 clamp+normalize is the only color
  math; ambient is boosted ×4 before clamp so even unlit-by-point-light blood is
  visible at typical ambient.

---

## 11. Audio coupling

**No audio coupling found** in the blood effect itself. `TBloodSystem` and
`TBloodAnimator` make no sound calls, and the spawn sites (`character.cpp:1842-1848`,
`:2242-2261`, `:231-256`) play no blood-specific wave. (The combat *hit* sound is
played elsewhere in attack resolution, not paired to the blood spawn.) Record:
blood is silent; the impact sound is a combat-code concern, not this effect.

---

## 12. Triggers & in-game appearance

- **Spawned by:** combat damage resolution in `TCharacter`. Builder
  `DEFINE_BUILDER("Blood", TBloodEffect)` (`effect_old.cpp:11243`); animator
  `REGISTER_3DANIMATOR("Blood", TBloodAnimator)` (`:11261`). Live triggers:
  - **On a damaging attack hit** (the main case): `EffectBurst("blood", 40|50)` when
    the attack's first frame deals damage and the attack carries the `CA_BLOOD` flag
    (`0x0200`, "Show blood on impact", `rules.h:41`):
    `if (ab->firsttime && ab->damage>0 && (!ab->attack || (ab->attack->flags &
    CA_BLOOD))) EffectBurst("blood", ...)` (`character.cpp:1845-1848`, also
    `:1924-1925`). `EffectBurst` (`:2225-2261`) spawns the effect at the victim
    (pos.z+height) and calls `SetParams(height, (GetFace()+128)&255, 0, 80, 20,
    random(1,5))` — **the SetParams call is commented out in the snapshot
    (`:2260`) as a port workaround, but the retail `ResolveAttack` proves it is
    live (§2.1).** Reconstruct the un-commented call.
  - **On impale:** `random(0,5)==1` chance → `EffectBurst("blood", 40)`
    (`character.cpp:1842-1843`).
  - **On decapitation/death (`impdecap` action):** every 5th frame in [6,65) a
    `"blood"` effect is spawned directly with `SetParams(95, 0, 64, 255, 5, 1)`
    (`character.cpp:232-256`) — many small upward squirts over the decap animation.
    Retail equivalent: `ResolveAttack` (`cls_0x5a7b98_..._4c1bb0.cpp:93,125,154`)
    calls `SetParams(75, face−91, 64, 91, 5, 1)` on the decap frames.
  - **Suppressed when burning:** `if (burning && !stricmp(name,"blood")) return;`
    (`character.cpp:2229`) — burning characters don't bleed (also retail,
    `EffectBurst_4c85d0.cpp:7`).
- **Where to see it in the original game:** hit any fleshly enemy with a damaging
  melee/weapon attack that has `CA_BLOOD` — blood sprays off the wound back toward
  you. Easiest repro: attack a town guard or a basic enemy and watch the splatter
  on a normal hit; for the heavy burst, perform/observe a decapitation finisher
  (`impdecap`) for the repeating upward squirts. Set `ViolenceLevel` to 5 (default)
  to see splat-sticking + trail-fill; below 3 droplets vanish on landing.
- **Vestigial?** No — live combat caller (retail-confirmed). The only "vestige" is
  the snapshot's *commented* `SetParams` line (a port crash workaround, not a design
  removal) and the dev-disabled alternates (§13).

---

## 13. Gaps & uncertainties

- **13.1 Kinematic constants are snapshot-only (the headline risk).** Per §2.1, the
  retail decomp does not contain readable `TBloodSystem` bodies, so gravity 0.37,
  drag 0.95, vel 2.0, splat-size 1.8, z-offset 20, durations 25/0.1, the
  `random(100,300)/100` speed scale, and the trail-fill budget math are
  **unverified against shipped retail**. The byte-identical asset + matching param
  structure make them *likely* unchanged, but reconstruction must visually match
  the spray distance/arc, splat size, and how long splats persist against in-game
  ground-truth (§12) and be ready to retune these specific numbers.
- **13.2 The attack `hspread`/`vspread` disagree with retail.** Snapshot attack
  path = `hspread 80, vspread 20`; retail `ResolveAttack` impale = `hspread 32,
  vspread 5`. Both are byte-angle spreads; the difference is tuning. Prefer the
  **retail** values for the impale/attack path (tighter spray); the decap path's
  spreads match. Verify the on-screen fan width against ground-truth.
- **13.3 Big (size-2) droplets are loaded but never drawn.** Init assigns
  `size=random(0, min(maxsize,2))` (so size 2 is possible), but the `object=big`
  draws are commented out (§7) — a size-2 droplet renders with the *small* sprite.
  Likely a late dev decision to drop the big sprite. Reconstruct **small + med
  only**; do not invent a big sprite draw. (If retuning, a size-2 droplet behaves
  kinematically like the others but visually uses the small box.)
- **13.4 The "bif" impact-splash is dead code.** `bifon`/`bifscale`/`spl`/`spl2`
  (box08/box04) and the `BLOOD_BIFDELAY/BIFSCALESTEP` constants drive a
  one-shot ground-splash decal at the impact point, but the entire render block is
  commented out (`effectcomp.cpp:1485-1529`) and the trailing `bifon--`/`bifscale+=`
  in Animate update unused state. **Do NOT reconstruct the bif splash.** (If a
  ground-impact decal is wanted later, it would be a separate, deliberate addition,
  not part of faithful B01.)
- **13.5 The big/med trail loops are dead.** The "trail bigs with meds and smalls"
  and "trail meds…" loops (`effectcomp.cpp:1242-1283`) are commented; only the
  **smalls-trail-smalls** loop (`:1287-1306`) is live. Reconstruct only the live
  trail-fill.
- **13.6 Dev-disabled alternates — do-not-reconstruct.** The commented
  `REGISTER_MULTI_3DANIMATOR_("blood", TBloodAnimator, TParticle3DAnimator)`
  (`effect_old.cpp:4691`, alongside the live `"sparks"` registration) and the
  commented `TBloodParticle3DAnimator` stub bodies (`:5008-5034`, which only call
  the `TParticle3DAnimator` base + a `// tenshu` placeholder) are an abandoned
  attempt to drive blood through the generic particle animator. The shipped path is
  `TBloodAnimator` + `TBloodSystem`. Ignore the alternates entirely.
- **13.7 `TBloodEffect::SetParams` body lives in the header.** The `SetParams`/
  `GetParams` accessors are inline in `src/effect.h:2438-2439` (store/read 6 ints).
  They are simple accessors; the param *meaning* is defined by how `Init` consumes
  them (§5). No behavioral ambiguity, just noting the body location.
- **13.8 `MaxLights` default = 1.** DoLighting reads up to 3 closest lights but only
  if `MaxLights>1`/`>2` (`effectcomp.cpp:1388,1394`); the global defaults to 1
  (`3dscene.cpp:31`). So in practice blood is lit by the single closest light +
  ambient×4 unless the engine raised `MaxLights`. Reconstruction's LitFlat
  approximation (hemisphere/world-up) is a reasonable stand-in for this
  closest-light sampling; verify brightness reads right in lit vs shadowed areas.

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Blood.I3D (68,820 B, byte-identical to retail). Address its 8 STILL
      box billboard sub-objects: box01..box08 (GetObject 0..7). Build the two
      pass-sets: base = {box05,box06,box07,box08} (obj 4-7), overlay = {box01..box04}
      (obj 0-3); within a set [0]=small,[1]=med,[2]=big,[3]=splat. NO procedural
      blood sprite. (§4)
- [ ] Drive it as TBloodSystem (the real system), bridged by TBloodAnimator from a
      TBloodEffect that stores 6 spray params via SetParams. Do NOT use the generic
      TParticle3DAnimator path (dev-disabled). (§2, §6)
- [ ] Spawn = one-shot BURST: num droplets (attack random(1,5); decap 1), clamped to
      min(num, ViolenceLevel*2), cap 30; object self-kills when all droplets done. (§5, §6.1)
- [ ] Emit origin = victim world pos, z + height (40/50 attack, 75/95 decap); droplet
      local start = (0,0,height); integrate in object-local space. (§5)
- [ ] Initial velocity = DIRECTIONAL SPRAY (not radial, not rising): per droplet
      ha=(hangle±hspread byte-angle)→rad, va=(vangle±vspread)→rad;
      vel=(BLOOD_VEL=2.0)*(cos ha, sin ha, sin va)*random(100,300)/100. hangle for an
      attack = (victim_face+128)&255 = 180° from victim front. (§5)
- [ ] Per-droplet size = random(0, min(maxsize,2)); maxsize clamped to ViolenceLevel. (§3,§6.1)
- [ ] Trail-fill: while budget `left`, add small droplets riding existing ones at
      vel*random(60,75)/80 (0.75..0.94x) and delay = parent.delay + 3 + random(-1,1);
      left=0 (no trail-fill) if ViolenceLevel<4. ONLY the smalls-trail loop. (§6.1, §13.5)
- [ ] Per-tick (24Hz) FLY: pos+=vel; vel.xy*=0.95 (drag); vel.z-=0.37 (gravity);
      scl+=0.01; on pos.z<=20 → snap z=20, stage=SPLAT (but drop droplet if
      ViolenceLevel<3 OR size==0). (§6.2)
- [ ] State machine SPLAT: scl=1.8, hold count>25 → SHRINK. SHRINK: scl-=0.1 to 0 →
      kill droplet. NO per-droplet alpha fade — the only fade is the SHRINK ramp. (§6.2)
- [ ] Core geometry: per used droplet, 2 quads (one per pass), placed by matrix
      Scale(scl)·[FLY: RotateX(-90°)·RotateZ(-45°)]·Translate(pos - (0,0,height)).
      SPLAT/SHRINK have NO rotation. (§7)
- [ ] Blend = TWO passes per droplet: pass0 = Alpha (SetBlendState: MODULATE,
      SRC_ALPHA/INV_SRC_ALPHA); pass1 = AdditiveStraight ONE/ONE but with MODULATE
      texture stage, depth states inherited (ZWRITE off, ZENABLE on). Both passes
      modulate by the DoLighting vertex color. (§7)
- [ ] Lit-mode = scene-LIT (LitFlat): per-droplet grayscale = clamp(sum(closest
      lights) + ambient/255*4, 1.5)/1.5, written to all 4 verts (D3DRGB), modulating
      the dark-red texture. NOT self-lit/unlit. (§7, §10)
- [ ] Depth = TestNoWrite; RefreshZBuffer restores scene Z per droplet over a
      (zbuf.xy=10×10)*scl patch around the projected droplet world pos. (§7)
- [ ] Texture animation = NONE (static STILL sprites, no UV scroll, no flipbook). (§8)
- [ ] Color = Blood.I3D dark-red texture × scene-brightness; verify it reads rich red
      (darker in shadow), not pink/gray/glowing. (§10)
- [ ] Orientation note: FLY droplets lie WorldXY (ground-flat) + 45° in-plane spin;
      splats keep authored facing. (§7)
- [ ] Render small + med ONLY — big (size-2) sprite draw is commented out (size-2
      droplets fall back to the small sprite). Do NOT draw big or the bif splash. (§7, §13.3, §13.4)
- [ ] Associated dynamic light EMITTED: NONE (blood is a light consumer, not source). (§9)
- [ ] Sub-effects spawned: NONE (the trail-fill is internal droplets, not child
      effects). (§6)
- [ ] Audio: NONE in the effect. (§11)
- [ ] Trigger wiring: spawned on a damaging hit with CA_BLOOD (rules.h:41) via
      EffectBurst("blood", 40/50) (character.cpp:1845-1848); impale random chance
      (:1842); decap repeating squirts (:232-256); suppressed when burning (:2229).
      RESTORE the commented SetParams call at character.cpp:2260 — retail proves it
      live (vtable +0x200, face-128, random(1,5)). (§12, §2.1)
- [ ] Do NOT reconstruct: the bif impact-splash block, the big/med trail loops, the
      TBloodParticle3DAnimator alternate, or the commented multi-animator
      registration — all dead/dev-disabled. (§13.4, §13.5, §13.6)
```

**Definition of done:** a damaging hit on a fleshly enemy produces a one-shot
directional spray of dark-red blood droplets that fly out (180° from the victim's
front, at 1×–3× base speed) along a ballistic arc with horizontal air-drag, splat
flat on reaching ground height (z=20) at scale 1.8, hold ~25 ticks, then shrink
away — each droplet drawn as two passes (alpha body + additive sheen) of the real
Blood.I3D box sprites, scene-lit so blood darkens in shadow, with count/size/splat
persistence scaled by ViolenceLevel, the effect self-killing when the last droplet
is gone — and NO big sprite, NO bif splash, NO emitted light, NO sound, NO alpha
fade.
