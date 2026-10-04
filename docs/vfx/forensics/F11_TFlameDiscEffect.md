# F11 TFlameDiscEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F11 (covers `TFlameDiscEffect` + its paired `TFlameDiscAnimator`; reuses **TMissileEffect [BASE]**, fully documented in F07) |
| **Class(es)** | `TFlameDiscEffect : TMissileEffect : TEffect` + `TFlameDiscAnimator : T3DAnimator` |
| **Status** | forensics-complete (see §13 — pre-release vestigial; key visual unknowns called out for ground-truth match) |
| **Retail fidelity** | **retail-partial.** Retail binary **does** register the class (cls_0x5b45ac, 401 B; vftable 129 entries; Pulse override @0x512bf0 lines up byte-for-byte with the snapshot's "play LIGHTNING sound on FLY-enter once" body — see §2.1). The `"FLAMEDISC"` registry-name XREF and `"lightning"` audio-key XREF both land in this class. **HOWEVER:** (a) the asset `Magic\FlameDisc.I3D` is **NOT in shipped `data/imagery.rvi`** (`Magic/flamedisc.i3d` absent — `FireColumn.I3D` is also missing; cf. `Photon.I3D` + `NewFireBall.I3D` which DID ship), and (b) `"FlameDisc"` is **NOT registered in the shipped `class.def`** (only legacy `Class.Def:2033` has it). No spell.def variant in shipped or legacy invokes `"FLAMEDISC"`. So the class code shipped but **the effect was never instantiable in the retail game** — a **vestigial** effect frozen at pre-release. The snapshot animator body is the only source for the visual. See §2.1 + §12. |
| **Author / Date** | vfx-forensics-agent (worktree-wf_b5d7064f-682-8) / 2026-05-29 |
| **Family** | fire (missile sub-family — shares the LAUNCH/FLY/EXPLODE state machine with Photon/FireBall/FireColumn) |
| **Draws** | composite — 1 world-oriented (ground-tipped) `flamedisc` quad spinning/scaling at the missile origin + 5 `spark` quads radiating outward on the ground plane (all from one `Magic\FlameDisc.I3D` mesh) |
| **Archetype(s)** | **(E) particle emitter** — 5 simple gravityless ground-plane sparks with per-particle lifetime, velocity, rebirth (see §6); **(F) composite + custom procedural logic** — central spinning disc + 5 radiating sparks orchestrated by the missile state machine, the disc growing then spawning sparks at FLY entry (per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md)). Also (C) world-oriented (WorldXY ground plane via `rot.x = -π/2`) and (B) I3D-geo (the `spark`/`flamedisc` sub-objects, transform-animated). |

---

## 1. Summary

A ground-pinned spinning **flame disc** projectile: at the caster's position the missile spawns at a random ±20-wu cubic jitter, then a flat textured disc (sub-object `flamedisc`) tipped onto the world ground plane (`rot.x = -π/2`) ramps up its scale across the LAUNCH phase. Once full size it flips to FLY (the shared `TMissileEffect` travel machine kicks in — the disc moves along the cast aim vector at speed 16 with the standard −1/16 downward arc), and during FLY + EXPLODE the animator continuously rebirths **5 small `spark` quads** that radiate **backward** from the disc (angle = caster_angle + 128 ± random(−32,32), i.e. the rear hemisphere ±~45°), each spark living until its scale decays to zero, then re-seeding from origin with a new random ±~45° back-vector. On impact (range-out / `MOVE_BLOCKED` / character hit within 32 wu) the base swaps to EXPLODE; the disc stops spawning fresh sparks but lets the in-flight ones decay; once `activesparks == 0` the animator signals done and the missile dies. The whole thing reads as a flat fire-disc projectile sliding along the floor leaving a trail of sparks behind it.

**Vestigial in shipped — see §12.** The asset wasn't included in `data/imagery.rvi` and no spell.def variant invokes `"FLAMEDISC"`; the retail binary still wires the class. Reconstruction can build it from the snapshot but cannot ground-truth-match against in-game footage — only against the asset itself.

The `TMissileEffect [BASE]` state-machine + launch/fly/impact logic is documented in **F07 §5.1 + §6.1** and is not re-described here; this doc focuses on **FlameDisc-specific** scale/spark/orientation behavior.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/classes_original/cls_0x5b45ac.cpp` — sparse: Pulse override (`virt_meth_0x512bf0`), factory (`virt_meth_0x514fb0`), dtor — bodies extracted, animator bodies not isolated.
  - `recon/classes_original/cls_0x5b45ac__vftable_5b45ac.cpp` — 129-entry vftable (516 B); Pulse override at vtable slot 68 (`virt_meth_0x512bf0` = +0x110 in vftable).
  - `recon/mappings/TFlameDiscEffect_cls_0x5b45ac_candidate.yaml` — MEDIUM confidence. `"FLAMEDISC"` registry-string XREF + `"lightning"` sound-key XREF both land in this class.
  - `recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml` — adjacent sibling class also touches `"FLAMEDISC"` at +0x710 (likely the registrar that loads the FlameDisc imagery on behalf of FireColumn / a sibling); MissileEffect.cpp assert string XREFs land in cls_0x5b4534 confirming the source file.
  - No `cls_0x_*Animator` mapping for TFlameDiscAnimator — bodies not isolated.
- **Pre-release (snapshot):** `src/missileeffect.cpp` (whole file under `#if 0`, gate `:15`/`:1702`) + `src/missileeffect.h`. `TMissileEffect` `:31-141`; `TFlameDiscEffect` `:1450-1482`; `TFlameDiscAnimator` `:1484-1701`. Class declarations `missileeffect.h:296-347`.
- **Sister effects consulted:**
  - `TPhotonEffect`/`TPhotonAnimator` (`missileeffect.cpp:147-471`) — same base; corroborates the LAUNCH-flourish → SetStatus → FLY hand-off pattern and the `Get3DImagery()->GetTexture(0,&tex)` / `numtexframes` flipbook-frame mechanism (`:194-195`). Photon does **not** tip to ground (no `rot.x = -π/2`).
  - `TFireBallEffect`/`TFireBallAnimator` (`missileeffect.cpp:478-1150`) — same base; per-frame UV atlas + trail + ring choreography (documented in F07). FlameDisc shares NONE of that — different geometry, different orientation.
  - `TFireColumnAnimator::Render` (`missileeffect.cpp:1343-1442`) — same `SetBlendState` + `CULLMODE=NONE` family pattern; uses `OBJ3D_MATRIX` builder instead of `OBJ3D_ROT1|SCL2|POS3`.
  - Shared blend helpers `SetBlendState` / `SetAddBlendState` `effect_old.cpp:221-244`.
- **Asset:** `legacy/Imagery/Magic/FlameDisc.I3D` (89,428 B, MD5 `a0d521e1bb3d4364352ba702d304e91c`). Registered as `"FlameDisc"` (`legacy/Class.Def:2033`) → `Magic\FlameDisc.I3D` `0x12daffce`.
- **Source-of-truth ranking:** snapshot `missileeffect.cpp` is authoritative for behavior + constants (the only readable body for the animator). Retail decomp corroborates *only* the class existence + Pulse handshake + vftable size + audio key — the animator/render bodies and all tuning constants are snapshot-only. The shipped game has no caller, so the animator was never re-tuned for retail — the snapshot is the closest thing to "ground truth" the effect ever had.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial; effect is vestigial in shipped)

**1. Constant grep — partial.** No tuning constants found in the readable retail body. The Pulse override (`cls_0x5b45ac::virt_meth_0x512bf0`, `recon/classes_original/cls_0x5b45ac.cpp:24-40`) decompiles cleanly and **matches the snapshot's `TFlameDiscEffect::Pulse` byte-for-byte in shape**:
```
// retail (decomp paraphrase)               // snapshot (missileeffect.cpp:1473-1482)
cls_0x5b401c::virt_meth_0x510220(this);     TMissileEffect::Pulse();
if (this->mbr_0xc == 1 &&                   if ((state == MISSILE_FLY) &&
    this->mbr_0x190 != 0) {                     (firsttime))
  this->mbr_0x190 = 0;                          firsttime = false;
  cls_0x41c7d0::meth_0x49b990(audio, …,1);      SoundPlayer.Play(LIGHTNING_SOUND); }
}
```
- `mbr_0xc == 1` ↔ `state == MISSILE_FLY` (FLY = 1, `missileeffect.h:22`).
- `mbr_0x190` (the 1-byte member at offset 400) ↔ `firsttime`.
- `cls_0x41c7d0::meth_0x49b990(…, 0x7f, 1)` plays a `0x7f` (127) volume / pan-center sound — matches `SoundPlayer.Play(LIGHTNING_SOUND)`.
This **confirms the FLY-entry sound trigger ships exactly as in the snapshot.** No other tuning constants (FLAMEDISC_SPEED, FLAMEDISC_LAUNCH_RADIUS, NUM_FLAME_SPARKS, FLAMEDISC_MAX_SCALE_STEP, FLAMEDISC_SCALE_STEP) appear in any readable retail body — animator code is not isolated.

**2. Asset identity — DIVERGENT.** `legacy/Imagery/Magic/FlameDisc.I3D` (89,428 B, MD5 `a0d521e1bb3d4364352ba702d304e91c`) **is NOT in the shipped `data/imagery.rvi`** (verified by `unzip -l` of the archive — `flamedisc.i3d` does not appear; cf. `Magic/photon.i3d` 48,340 B and `Magic/newfireball.i3d` 144,664 B which DID ship). Sister `FireColumn.I3D` is also absent. So the asset shipped to dev/QA but was dropped from the final data pack. The snapshot copy is the only extant version.

**3. Structure/layout — CONFIRMED EXISTS.** cls_0x5b45ac is 401 B, vftable 129 entries (516 B). The snapshot's `TFlameDiscEffect` adds only 1 byte (`firsttime` bool) over `TMissileEffect` — the base must therefore be ~400 B. Member offsets in the retail struct (`mbr_0xc`, `mbr_0xe0`, `mbr_0x184`–`mbr_0x18c`, `mbr_0x190`) are consistent with `TEffect`/`TMissileEffect` field layout (state, sub-object/pos/spell pointers near 0xe0–0x18c, the 1-byte `firsttime` at 0x190). The class exists in retail with the same general layout.

**4. Registration + naming — CONFIRMED but ORPHANED.**
- `"FLAMEDISC"` string at `.rdata 005e1b4c`; **two** XREFs (`recon/classes_original/_data.txt:108753-108754`):
  - 0x00512b40 lands at +0x710 in cls_0x5b4534 (the FireColumn class) — likely the *imagery loader* (the class registers all its sub-imagery names, FlameDisc included).
  - 0x00512c40 lands at +0x50 in cls_0x5b45ac (this class's `virt_meth_0x512bf0` body) — likely the builder/registrar tail.
- `"lightning"` (`LIGHTNING_SOUND`) XREF lands at +0x26 in this class — consistent with the snapshot's `SoundPlayer.Mount/Play(LIGHTNING_SOUND)` calls in `TFlameDiscEffect::Initialize`/`Pulse` (`missileeffect.cpp:1468,1480`).
- The shipped `class.def` (extracted from `data/imagery.rvi`'s `class.def` member) **does NOT contain `"FlameDisc"`** — only legacy `Class.Def:2033` has it. **No spell.def variant in legacy or shipped invokes `"FLAMEDISC"`** (grep -i confirmed).
- So: the retail binary contains the class + the builder registration + the audio code, but the class is **unreachable at runtime** because (a) the asset name isn't in shipped `class.def`, and (b) nothing calls `NewObject("FlameDisc"…)` or `DEFINE_BUILDER("FLAMEDISC"…)` produces a builder that no spell selects.

**5. Audio coupling — CONFIRMED.** `LIGHTNING_SOUND = "lightning"` (`src/effect.h:473`). The retail Pulse plays it via `cls_0x41c7d0::meth_0x49b990(…, 0x7f, 1)`; the snapshot calls `SoundPlayer.Play(LIGHTNING_SOUND)` on FLY-entry. Volume `0x7f` (127, "mid") matches the `SoundPlayer.Play` default. Shared with `TPhotonEffect` / `TFireColumnEffect` which mount the same sound key.

**Verdict — retail-partial / vestigial.**
*What it is, that it exists, its Pulse handshake, and its audio* are **retail-confirmed**. *Asset content, animator constants (5 sparks, scale step .2, max scale 3.0, launch radius 20, speed 16, angle offset 128 ± 32), per-frame logic, and rendering* are **snapshot-only** — the animator wasn't decompiled and the effect is vestigial in shipped, so there is **no in-game ground truth** to vet against. The reconstruction must work from the snapshot + the legacy I3D as-is and visually self-test (no retail capture is possible — see §12).

---

## 3. Constants

`[BASE]` = lives in `TMissileEffect` (shared by Photon/FireBall/FireColumn/FlameDisc — see F07 §3 for the full BASE table).
`[FD]` = FlameDisc-specific.

| name | value | units | scope | source | confirmed? |
|------|-------|-------|-------|--------|------------|
| MISSILE_SPEED (default) | 16 | pos-units/tick (pre-ROLLOVER) | [BASE] | missileeffect.h:26; set `Initialize` :37 | snapshot-only |
| MISSILE_RANGE | 2 | range multiplier | [BASE] | missileeffect.h:27 | snapshot-only |
| range (init) | 32768 | ticks (huge until launched) | [BASE] | missileeffect.cpp:35 | snapshot-only |
| range (flying) | `(240*MISSILE_RANGE)/(speed/ROLLOVER)` = 30 ticks @ speed 16 | ticks of flight | [BASE] | missileeffect.cpp:68 | snapshot-only |
| ROLLOVER | 65536 (`1<<16`) | sub-unit precision | [BASE] | revdefs.h | snapshot-only |
| vel.z on launch | `speed / -16` | pos-units/tick (slight downward arc) | [BASE] | missileeffect.cpp:66 | snapshot-only |
| character-hit radius | 32 | wu (Distance check during FLY) | [BASE] | missileeffect.cpp:105 | snapshot-only |
| states | LAUNCH=0, FLY=1, EXPLODE=2 | enum | [BASE] | missileeffect.h:20-24 | yes (retail: mbr_0xc compared `== 1` at FLY) |
| FLAMEDISC_SPEED | 16 (comment "// 2", overridden to 16) | pos-units/tick (pre-ROLLOVER) | [FD] | missileeffect.h:296; set :1466 | snapshot-only |
| FLAMEDISC_LAUNCH_RADIUS | 20 | wu (spawn-position cubic jitter) | [FD] | missileeffect.h:297; used :1460-1462 | snapshot-only |
| NUM_FLAME_SPARKS | 5 | count (indices 1..5 are sparks; index 0 is the central disc, so arrays are size 6) | [FD] | missileeffect.h:320; loops :1515,1568,1612,1663 | snapshot-only |
| FLAMEDISC_MAX_SCALE_STEP | 3.0 (comment "// 5.0") | scale cap | [FD] | missileeffect.cpp:1538 | snapshot-only |
| FLAMEDISC_SCALE_STEP | 0.2 | scale/tick (grow during LAUNCH, decay/grow for sparks) | [FD] | missileeffect.cpp:1539 | snapshot-only |
| spark init `framenum` | `-random(2,8)` | ticks of delay before spark is born | [FD] | missileeffect.cpp:1518,1584,1628 | snapshot-only |
| spark angle (back-cone) | `(caster_angle + 128 + random(-32,32)) & 255` | byte-angle (0..255 = 0..360°), so the rear hemisphere ±~45° | [FD] | missileeffect.cpp:1524,1589 | snapshot-only |
| spark velocity magnitude | `MissileEffect::GetSpeed()` = 16 (post-ROLLOVER divide) | pos-units/tick | [FD] | missileeffect.cpp:1524,1589 | snapshot-only |
| spark `v.z` | 0 (sparks stay on the ground plane) | wu/tick | [FD] | missileeffect.cpp:1527,1592 | snapshot-only |
| spark birth scale | 3.0 | scale | [FD] | missileeffect.cpp:1575,1620 | snapshot-only |
| spark decay rate | −0.2/tick (= −FLAMEDISC_SCALE_STEP) | scale/tick (after birth, every tick) | [FD] | missileeffect.cpp:1580,1624 | snapshot-only |
| disc grow rate (LAUNCH) | +0.2/tick once `framenum[0] >= 0` | scale/tick | [FD] | missileeffect.cpp:1558 | snapshot-only |
| disc max scale | 3.0 (FLAMEDISC_MAX_SCALE_STEP) | scale cap (then LAUNCH→FLY via inst->SetCommandDone) | [FD] | missileeffect.cpp:1554-1561 | snapshot-only |
| disc init `framenum[0]` | `-random(2,8)` | ticks of delay before disc starts growing | [FD] | missileeffect.cpp:1518 | snapshot-only |
| disc init `scale[0]` | 0.0 | scale | [FD] | missileeffect.cpp:1517 | snapshot-only |
| obj `rot.x` (both objs) | `-π/2` | rad (tips the I3D's authored-XY quad onto the world ground plane → WorldXY) | [FD] | missileeffect.cpp:1677 | snapshot-only |
| obj `rot.y` / `rot.z` | 0 / 0 | rad (no extra spin authored — see §13 re: "spinning disc" comment) | [FD] | missileeffect.cpp:1678-1679 | snapshot-only |
| numtexframes (per I3D) | read at runtime from `Get3DImagery()->GetTexture(0,&tex).numframes` | frames | [FD] | missileeffect.cpp:1507-1508 | snapshot-only (asset-dependent) |
| EXPLODE termination | `activesparks == 0` → `inst->SetCommandDone(true)` (animator done) → `TMissileEffect::Pulse` sees `!HasAnimator()` → `KillThisEffect` | gate | [FD] | missileeffect.cpp:1608-1609 | snapshot-only |
| audio sound key | `"lightning"` (`LIGHTNING_SOUND`) | string | [FD] | effect.h:473; missileeffect.cpp:1468,1480 | yes (retail: `lightning` XREF at +0x26 in cls_0x5b45ac; Pulse decomp plays it on FLY-entry) |

**Note on the disc orientation.** `rot.z = 0` is set explicitly (`missileeffect.cpp:1679`). The class header/asset names ("FlameDisc", "Spinning Flame Disc Animator") suggest a spin, but the snapshot's Render body sets `rot.z = 0` every tick. Either (a) the disc was intended to be visibly spinning via per-tick rotation that the snapshot hadn't wired (a TODO/WIP), or (b) the "spin" is implied by the **texture itself** animating (per-frame flipbook via `framehtexs[]` advancing each tick: `frame = framenum[n] % numtexframes`, `missileeffect.cpp:1689`). The asset has `numframes > 0` (the only texture index queried), so the texture-frame-animation hypothesis is plausible — visual rotation comes from the cycling I3D texture handles, not from a `rot.z` per-frame advance. See §13 gap re: this spin question.

---

## 4. Assets

| asset | path | size (B) | MD5 | role | how loaded (original) |
|-------|------|----------|-----|------|-----------------------|
| FlameDisc mesh | `legacy/Imagery/Magic/FlameDisc.I3D` | 89,428 | `a0d521e1bb3d4364352ba702d304e91c` | the spinning ground disc + the radiating sparks (one mesh, two sub-objects) | registry name `"FlameDisc"` (`legacy/Class.Def:2033`, hash `0x12daffce`), `RegisterImagery`/`LoadImagery` at class load |

**Asset identity:** the legacy file is the ONLY copy — **NOT shipped in `data/imagery.rvi`** (§2.1.2). The reconstruction must use the legacy disk copy directly; there is no retail variant to byte-compare against. Treat the legacy I3D as the source of truth.

**I3D structure (from the binary, decoded from raw hex):**
- Magic `CGSR`, version `1`.
- **3 animation states** (state name records at file offsets `0x1c`, `0x70`, `0xc4`):
  - state 0 = `"missilelaunch"` (MISSILE_LAUNCH)
  - state 1 = `"missilefly"` (MISSILE_FLY)
  - state 2 = `"missileexplode"` (MISSILE_EXPLODE)
  Note: these are full names with the `missile` prefix (vs. `NewFireball.I3D`'s short `launch`/`fly`/`explode`) — the framework selects state by `state` value, so the actual string is decorative.
- **2 sub-objects**, named at file offsets `0x858` and `0x87c`:
  - `GetObject(0)` = `"spark"` — the radiating ground-spark quad (used for sparks at array indices 1..5)
  - `GetObject(1)` = `"flamedisc"` — the central ground-disc quad (used for the disc at array index 0)
- **One texture slot** (`numtex = 1`, hex `01 00 00 00` at file offset `0x14`) shared by both sub-objects via the engine's standard material/texture binding. The animator reads `numtexframes = tex.numframes` from this slot at Initialize (`missileeffect.cpp:1507-1508`) and advances per-tick via `frame = framenum[n] % numtexframes`. Mechanism is **flipbook** (`framehtexs[]` per-frame engine handles), not UV sub-rect atlas — confirmed by the snapshot's `frame = … % numtexframes` write (engine-side flipbook), the **absence** of any per-vertex `tu/tv` rewrite, and the I3D having only one texture slot (no 4×4-style cell grid).

**Texture atlas / UV decode.**
- **The asset is a flipbook, NOT a UV atlas grid.** The single texture slot is animated by handle-swap (`tex.framehtexs[frame]` per draw) — the *whole* texture is replaced each frame. There is **no** 2×2 or 4×4 atlas grid in this effect; **no** per-frame UV sub-rect; both sub-objects share the same flipbook texture slot and each draw picks its own current frame via `frame = framenum[n] % numtexframes`.
- **Per-sub-object UVs (read from the I3D geometry, file offsets `0x1bd0`+):** the `spark` and `flamedisc` quads both authored as flat 4-vertex planes with UVs at the cell corners `(0,0)`, `(1,1)`, `(0,1)`, `(1,0)` (the canonical full-texture quad). Verified by raw float decode at offsets 0x1bd0–0x1c30 (smaller quad, `±0.881..±2.643` extent at `z=±2.158`) and 0x1c50–0x1cb0 (larger quad, `±1.753..±5.259` extent at `z=±4.294`). Each quad uses the **entire** texture face — no sub-rect. The per-frame flipbook step animates the whole quad.
- **Per-instance selection rule.** Disc uses `framenum[0]`; each of the 5 sparks uses its own `framenum[n]` (n=1..5). Sparks are seeded with random `−random(2,8)` start values, so they start cycling at different texture frames — gives a desynchronized flicker / motion across the 5 sparks. The wrap is at `numtexframes`.

**No external textures** — the texture surface is embedded in the I3D. Black-key (chroma-key) convention: standard for missile-family effect textures (`knowledge/02_ASSETS_IMAGERY.md §4`) — black pixels are transparent under the Alpha blend.

No procedural-only path: the visual identity (disc + sparks + flame texture) is the loaded I3D mesh. Do NOT substitute a procedural disc/sparks (feedback-no-standins). If the legacy I3D cannot be loaded, escalate as a blocker.

---

## 5. Spawn & emit

### 5.1 [BASE] How a missile enters the world & launches

(Same as F07 §5.1 — abbreviated here; full pseudocode in F07.) The `TFlameDiscEffect` is constructed by a `NewObject("FLAMEDISC", …)` style call from a (theoretical, never-wired) spell or script. `TFlameDiscEffect::Initialize` (`missileeffect.cpp:1457-1471`) runs **before** `TMissileEffect::Initialize` finishes (because the child calls base's `Initialize()` mid-body):

```
TFlameDiscEffect::Initialize():
    // pre-jitter the spawn position (cubic ±20 wu around the constructor's pos)
    pos.x += random(-20, 20)
    pos.y += random(-20, 20)
    pos.z += random(-20, 20)
    TMissileEffect::Initialize()        // sets state=LAUNCH, angle=0, range=32768, speed=16*ROLLOVER, status=false
    SetSpeed(FLAMEDISC_SPEED=16)        // re-set (idempotent — TMissileEffect::Initialize already set 16)
    SoundPlayer.Mount("lightning")      // load the sound; doesn't play yet
    firsttime = true                    // FLY-entry edge tracker
```

Launch hand-off is the standard family pattern: the animator runs the LAUNCH sequence (grow the disc) and calls `inst->SetCommandDone(true)` when the disc reaches max scale (`missileeffect.cpp:1561`). The base `TMissileEffect::Pulse` reads `status` (set elsewhere via SetStatus — but **note** the FlameDisc animator does NOT call `((PTMissileEffect)inst)->SetStatus(true)`, so the snapshot LAUNCH→FLY transition appears to be gated by `SetCommandDone` only — see §13 gap). On FLY-entry the next Pulse fires the `lightning` sound once and sets `firsttime=false`.

### 5.2 [FD] LAUNCH spawn — the disc

- **Trigger semantics:** one-shot spell cast → 3-state machine (LAUNCH → FLY → EXPLODE → die). Sparks continuously rebirth during FLY + EXPLODE.
- **Count per trigger:** 1 central disc + 5 sparks = **6 quads on screen at peak** (during FLY; during LAUNCH only the disc is active; during EXPLODE only in-flight sparks remain).
- **Initial position (disc, index 0):** `p[0] = (0,0,0)` (local to the effect's world pos). Drawn at `effect_pos` (no per-frame `p[0]` update — disc stays at the missile center).
- **Emit anchor convention:** the missile's own world position (which moves under `TMissileEffect::Pulse`'s velocity integration once in FLY).
- **Coordinate space:** sparks' `p[n]` is **local** to the missile origin — accumulated `p[n] += v[n]` per tick (the disc has no `p[0]` update). The missile itself moves in world space via the engine integrator.

### 5.3 [FD] Spark emit — 5 ground-radiating sparks

Each spark slot (`n` = 1..5) holds (`p[n]`, `v[n]`, `scale[n]`, `framenum[n]`). At Initialize:
- `scale[n] = 0`, `framenum[n] = -random(2,8)` (so each spark waits 2..7 ticks before being born; the negative `framenum` also gates render in `Render()`, `missileeffect.cpp:1665` "Only render if framenum >= 0").
- `p[n] = (0,0,0)` (local origin).
- `v[n] = ConvertToVector(angle + 128 + random(-32,32), speed=16)` → `v.x = pnt.xf`, `v.y = pnt.yf`, **`v.z = 0`**. With `angle` = `((TMissileEffect*)inst)->GetAngle()` (the caster's aim byte-angle 0..255), `angle+128` is **180° rotated** (i.e. directly behind the missile), and the `±random(-32,32)` adds **±45°** (32/256·360° ≈ 45°) jitter — so sparks emit into the **rear ±45° hemisphere of the disc's travel direction**, **flat on the ground** (z=0 in velocity).

The `ConvertToVector` lookup (`object.cpp:168-188`) is the engine's 8-bit-angle → unit-XY-direction × speed table: `vect.x = DistX[angle] * speed / 256`, `vect.y = DistY[…] * speed / 256`, `vect.z = 0` (since `zangle = 0` default). So spark `v.x`/`v.y` magnitudes are bounded by `speed = 16` (after the `GetSpeed()`'s ROLLOVER divide — `MissileEffect::GetSpeed() = speed / ROLLOVER = 16`).

During Animate (FLY and EXPLODE), once a spark's `framenum >= 0` and a tick after birth, it accumulates `p[n] += v[n]` per tick → the spark slides outward on the ground plane along its `v[n]` vector at ~16 wu/tick.

**Rebirth (FLY only — EXPLODE never rebirths):** when a spark's `scale[n] <= 0`, the FLY branch (`missileeffect.cpp:1582-1593`) re-seeds it: new `framenum = -random(2,8)`, `p[n] = (0,0,0)`, new `v[n]` with a fresh `angle+128±random(-32,32)` direction. So during FLY the disc continuously emits a fresh ground-radiating spark each time an old one dies → **continuous trail-like spray** of 5 sparks behind the disc.

### 5.4 Spatial diagram

```
                 wz (up)
                 │     [LAUNCH]                              [FLY]
                 │     ─────────                             ────
                 │      ↑ disc growing 0→3.0 over            disc full-size, ground-tipped,
                 │        ~15 ticks (FLAMEDISC_SCALE_STEP    sliding along aim (speed 16, vel.z=−1)
                 │        per tick after random(2,8)-tick                                  ╲
                 │        delay)                                                            ●─→ aim vector
                 │                                            ←─ ·                          ╱
                 │                                          ←──   ·  sparks radiating
                 │     ●  ← flamedisc quad lies flat       ← ──     ·   into the REAR ±45°
                 │   ─────  on world XY plane (rot.x=−π/2)    spark ground hemisphere
                 │   spawn @ caster pos + ±20 wu                  ·  (angle+128±32)
                 │   cubic jitter                              ·   each spark ~16 wu/tick, v.z=0,
                 └───────────────────────────────── wx        rebirth on scale<=0 → fresh angle.
                ╱
              wy
```

ASCII top-down view (showing the spark cone behind the moving disc):

```
                   spark
                  ╲  ●                      missile aim direction →
   spark           ╲  ·                  ╲
       ●····________●─── flamedisc ──── ─ ─ ─ ─ →
   spark           ╱  ·                  ╱
                  ╱  ●
                   spark              (±45° rear cone, 5 sparks total)
```

---

## 6. Behavior & per-frame logic

### 6.1 [BASE] `TMissileEffect::Pulse` — the shared travel/impact machine

See **F07 §6.1** for the full pseudocode (`missileeffect.cpp:52-135`). Recap: LAUNCH waits for `status=true` then converts aim→velocity / sets OF_MOVING|OF_WEIGHTLESS / `vel.z = speed / −16` / `range = (240*2)/(speed/ROLLOVER)`; FLY counts down range, scans for `MOVE_BLOCKED` and character-within-32-wu hits, switches to EXPLODE on any of those; EXPLODE waits for `!HasAnimator()` → `KillThisEffect`.

**FlameDisc-specific override:**

```
TFlameDiscEffect::Pulse():                          // missileeffect.cpp:1473-1482
    TMissileEffect::Pulse()                         // base travel + impact
    if (state == MISSILE_FLY && firsttime):         // FLY-entry edge
        firsttime = false
        SoundPlayer.Play("lightning")               // play once on FLY-entry
```

This is the entire effect-side Pulse override — exactly what the retail decomp at `cls_0x5b45ac.cpp:24-40` corroborates (§2.1.1).

### 6.2 [FD] `TFlameDiscAnimator::Initialize` (`missileeffect.cpp:1498-1529`)

```
TFlameDiscAnimator::Initialize():
    T3DAnimator::Initialize()
    Get3DImagery()->GetTexture(0, &tex)
    numtexframes = tex.numframes                    // how many frames the flipbook has
    activesparks = 0
    state = oldstate = MISSILE_LAUNCH               // start in LAUNCH
    angle = ((TMissileEffect*)inst)->GetAngle()     // cache caster aim (0..255 byte-angle)
    for n in 0..NUM_FLAME_SPARKS:                   // n = 0,1,2,3,4,5 (so 6 slots, index 0 = disc, 1..5 = sparks)
        scale[n] = 0.0
        framenum[n] = -random(2, 8)                 // -2..-8 tick birth delay
        p[n] = (0, 0, 0)
        ConvertToVector((angle + 128 + random(-32,32)) & 255, GetSpeed()=16, pnt)
        v[n] = (pnt.xf, pnt.yf, 0)                  // rear hemisphere ±~45°, on ground plane
```

Note: **index 0 (the disc) ALSO gets a v[0] assigned** (`missileeffect.cpp:1524-1527`), but the disc's `p[0]` is never updated in Animate (LAUNCH doesn't move it; FLY/EXPLODE update only sparks at n=1..5). So `v[0]` is **dead data** — never integrated. (Snapshot WIP artifact.)

### 6.3 [FD] `TFlameDiscAnimator::Animate` (`missileeffect.cpp:1541-1645`)

```
TFlameDiscAnimator::Animate(draw):
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)                     // reset the LAUNCH→FLY readiness flag

    switch state:
        case MISSILE_LAUNCH:
            // grow the central disc; ignore sparks during LAUNCH
            if (scale[0] < FLAMEDISC_MAX_SCALE_STEP=3.0):
                framenum[0]++                       // tick the delay; reach 0 then advances texture frame
                if (framenum[0] >= 0):
                    scale[0] += FLAMEDISC_SCALE_STEP=0.2
            else:
                inst->SetCommandDone(true)          // disc reached max → LAUNCH complete
            break

        case MISSILE_FLY:
            // disc just sits at full scale; sparks rebirth & integrate
            for n in 1..NUM_FLAME_SPARKS:
                framenum[n]++
                if (framenum[n] >= 0):
                    if (framenum[n] == 0):          // moment of birth (the tick framenum becomes 0)
                        scale[n] = 3.0
                        activesparks++
                    else:
                        scale[n] -= FLAMEDISC_SCALE_STEP=0.2    // -0.2/tick decay
                        if (scale[n] <= 0):                       // spark dies
                            activesparks--
                            framenum[n] = -random(2, 8)           // re-seed delay
                            p[n] = (0, 0, 0)
                            ConvertToVector((angle + 128 + random(-32,32)) & 255, GetSpeed()=16, pnt)
                            v[n] = (pnt.xf, pnt.yf, 0)             // fresh rear ±45° direction
                        else:
                            p[n] += v[n]                           // integrate position
            break

        case MISSILE_EXPLODE:
            if (activesparks == 0):
                inst->SetCommandDone(true)          // last spark gone → animator done
            for n in 1..NUM_FLAME_SPARKS:
                framenum[n]++
                if (framenum[n] >= 0):
                    if (framenum[n] == 0):          // any spark that was queued mid-delay still births once
                        activesparks++
                        scale[n] = 3.0
                    else:
                        scale[n] -= FLAMEDISC_SCALE_STEP=0.2
                        if (scale[n] <= 0):
                            activesparks--
                            framenum[n] = -32767     // PERMANENT mute (NOT re-seeded — different from FLY!)
                        else:
                            p[n] += v[n]
            break

    oldstate = state
```

**Key behavioral differences between FLY and EXPLODE:**
| | FLY | EXPLODE |
|---|---|---|
| spark dies (scale ≤ 0) | re-seed (delay + new direction) → continuous emission | `framenum = -32767` → silent forever |
| termination | never (loops as long as missile flies) | `activesparks == 0` → `SetCommandDone(true)` → missile killed |

So EXPLODE is a "drain mode": stops emission, lets the in-flight sparks finish their last decay, then dies. There's **no impact burst, no shockwave ring, no blast damage, no associated visual on impact** — just stop spawning and wait for the trail to fade. This is consistent with the snapshot's `TFlameDiscEffect::Pulse` doing nothing special on EXPLODE entry beyond the base's `KillThisEffect when !HasAnimator()`.

**The "disc was already at max" oddity in Animate FLY:** the FLY branch only loops `n in 1..NUM_FLAME_SPARKS` — there is no update for the disc at index 0 in FLY. The disc just stays at scale 3.0, sitting at the missile origin while the missile moves through world space (per-draw `obj->pos = effect_pos` makes the disc track the missile center). The "spin" is purely the per-tick texture-flipbook (see §8). This may be a snapshot WIP — see §13.

### 6.4 Helper sub-systems

**None.** Unlike F07 (which composes `TSubParticleAnimator` + `TShockAnimator`), F11 has **no sub-animators** — all 6 quads are drawn directly by `TFlameDiscAnimator::Render` via the I3D's two sub-objects, repurposed per slot. The class declaration confirms: only `p[]`, `v[]`, `scale[]`, `framenum[]`, `numtexframes`, `activesparks`, `oldstate`, `angle` as fields (`missileeffect.h:326-334`) — no `TSubParticleAnimator spark`, no `TShockAnimator ring`.

### 6.5 Temporal diagram

```
state:    LAUNCH                       │ FLY                                      │ EXPLODE              │ dead
          (disc grows, no sparks)      │ (sparks rebirth continuously)            │ (drain)              │
delay:    rand(2..8) ticks per slot    │
disc:     scale 0 ──+0.2/tick──► 3.0   │ scale = 3.0 (frozen at max)              │ (still drawn ─ until │
          (~15 ticks total inc. delay) │                                          │  animator dies, then │
                                       │                                          │  missile killed)     │
spark[n]: silent (framenum<0)          │ ↓ scale=3.0 at birth, then -0.2/tick:    │ same decay, but on   │
                                       │   3.0 → 0 over 15 ticks, then re-seed    │   death framenum=-32767 │
                                       │   with new ±45° rear direction;          │   (mute forever, no  │
                                       │   p[n] += v[n] each tick                 │   rebirth)           │
activesparks: 0 →                      │ ~5 (steady state, occasional 4/6 as      │ 5 → 4 → 3 → ... → 0  │
                                       │  ones rebirth)                            │ → SetCommandDone     │

trigger:  inst->SetCommandDone(true)   │ TMissileEffect::Pulse handles range/     │ activesparks==0 →    │ TMissileEffect::Pulse:
          when scale[0] >= 3.0         │ hit/MOVE_BLOCKED → state=EXPLODE         │ SetCommandDone(true) │ !HasAnimator →
          → LAUNCH done                │                                          │                      │ KillThisEffect
```

Per-spark envelope (scale vs ticks after birth, base 24Hz):

```
scale
3.0 ┤●                                   ← spark birth (framenum hits 0)
    │ ╲
    │  ╲
    │   ╲ −0.2/tick
    │    ╲
0.0 ┤     ●─→  age in ticks
    0     15
```

---

## 7. Rendering (original render state + geometry)

`TFlameDiscAnimator::Render` body: `missileeffect.cpp:1654-1701`.

- **What it draws:** up to 6 quads per frame — 1 `flamedisc` quad (from `GetObject(1)`) at the missile origin + 0..5 `spark` quads (from `GetObject(0)`) at `p[n]` offsets, **only drawn when `framenum[n] >= 0`**. Both quads come from one I3D mesh.
- **Sub-object index inversion (CONFIRMED in the snapshot code, even though the comment is wrong).** The Render body says:
  ```
  // missileeffect.cpp:1667-1670
  if (n)
      obj = GetObject(0);     // 0 is Spark, 1 is FlameDisc   ← code is right, COMMENT MATCHES
  else
      obj = GetObject(1);     // 0 is Spark, 1 is FlameDisc   ← code is right, COMMENT MATCHES
  ```
  i.e. when n=0 (the disc slot) use sub-object 1 (`flamedisc`); when n=1..5 (the spark slots) use sub-object 0 (`spark`). This matches the I3D sub-object naming (index 0 = `spark`, index 1 = `flamedisc`, verified from the binary at offsets `0x858`/`0x87c`).
- **Blend mode (original):** the animator calls `SaveBlendState() / SetBlendState() / RestoreBlendState()` (`missileeffect.cpp:1659-1660,1698`). **`SetBlendState()` = Alpha** (texture-stage `D3DTBLEND_MODULATE`, `SRCBLEND = SRCALPHA`, `DESTBLEND = INVSRCALPHA`, ZWRITE off, ZTEST on) (`effect_old.cpp:221-233`). Same as F07 (FireBall) — modulated alpha with black-keyed texture.
  - **BLEND SANITY-CHECK.** The flame texture is the textbook "bright-fire on black background" sprite — exactly the family that *usually* ships **AdditiveStraight** for the warm glow read. Sister-family check: Photon, FireBall, FireColumn all use **`SetBlendState` (Alpha)** in the snapshot too (`missileeffect.cpp:410, 1062-1082, 1348-1349`). The whole missile family traces straight Alpha in the snapshot, not Additive. The closest *family* additive precedent is the cast spells (`FireFlash`, `FireWind`, `FireCone`, `DragonFire` — all `SetAddBlendState`). FlameDisc is a projectile, not a cast-glow, so the Alpha trace is *consistent with the missile family pattern*, but **the render blend is a top snapshot-drift risk** (retail Render bodies are NOT decompiled here, exactly per the SPARKS lesson: snapshot-Alpha glow may have shipped Additive). **Flag for the reconstruction:** the snapshot says Alpha, the bright-on-black sprite design hints Additive, no retail Render to confirm, **and the effect is vestigial in shipped so no in-game footage exists to vet.** Use snapshot-Alpha as the build target; if visual self-test (asset preview) reads weak/transparent, AdditiveStraight is the next-best hypothesis. See §13.
- **Lit vs self-lit:** **self-lit / Unlit** — Render body never reads/writes ambient/diffuse lights and does not zero material slots. Color is texture × per-vertex diffuse (which `SetBlendState`'s MODULATE-mode applies). Standard for missile-family glows.
- **Depth / Z:** TestNoWrite — `SetBlendState` sets ZWRITE off, ZTEST on. The Render body does NOT call `RefreshZBuffer` (no override is declared in the class; `missileeffect.h:324-347` lists no `RefreshZBuffer`, unlike `TFireBallAnimator` which has one at `missileeffect.h:231`). So nothing repairs scene Z under the disc/sparks — they read against the existing scene depth and don't write their own.
- **Orientation:** **WorldXY (ground-tipped)** — `obj->rot.x = -π/2` on **every quad** (`missileeffect.cpp:1677`), no per-quad facing rotation. The I3D's authored-XY-plane quad is tipped onto the world Z=0 plane, so the disc and sparks lay flat on the ground (foreshortening under the iso/perspective camera). `rot.y = 0`, `rot.z = 0` per `missileeffect.cpp:1678-1679`.
- **Per-quad / per-object transform flags (`missileeffect.cpp:1682-1686`):**
  - Sparks (n != 0): `flags |= OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3` — apply rotation + scale + offset position. `obj->pos = p[n]` (the spark's local-space drifted position).
  - Disc (n == 0): `flags |= OBJ3D_ROT1 | OBJ3D_SCL2` (no POS3) — apply rotation + scale only; the disc draws at the I3D's authored origin = the missile's world position (engine handles the missile-relative placement via the animator's default origin). The `;;` double-semicolon at `:1686` is a snapshot typo / harmless.
- **Scale (CONFIRMED BUG in the snapshot code).** `missileeffect.cpp:1676` sets `obj->scl.x = obj->scl.y = obj->scl.z = (float)scale[0]` — **all** quads (disc AND sparks) are scaled by `scale[0]` (the disc's scale), regardless of the spark's own `scale[n]`. The `scale[n]` per-spark decay computed in Animate (3.0 → 0 over 15 ticks) is therefore **never used for rendering** — it's only used to gate spark life/rebirth (the `if scale[n] <= 0` check). So at render time, every visible quad uses `scale[0] = 3.0` once the disc has grown to max. This is almost certainly a snapshot bug (should be `scale[n]` not `scale[0]`). Two ways to read this for reconstruction:
  - **(a) Port it faithfully** (sparks pop in/out at fixed scale 3.0 — `scale[n]` only gates visibility) — matches the snapshot exactly but is visually flat.
  - **(b) Fix it to `scale[n]`** (sparks fade out smoothly as they decay) — this is *intent* per the Animate body's careful `scale[n] -= 0.2/tick` math, and matches sister Photon's `obj->scl = scale[n]` pattern (`missileeffect.cpp:425`).
  **Recommendation:** (b) is the intent; the `scale[0]` write is a typo. The matching FireColumn / Photon animators all use `scale[n]` per-slot. Treat as a §13 gap with a strong "fix to `scale[n]`" guidance. (Captured because reconstruction MUST decide explicitly — silently picking (a) would ship a clearly-broken effect, silently picking (b) deviates from snapshot.)
- **Frame selection per quad (`missileeffect.cpp:1689`):** `frame = framenum[n] % numtexframes` — the engine's flipbook handle picker (sets `obj->textureframe[0..1]` indirectly via the `frame` member). Each quad picks its own texture-flipbook frame based on its `framenum[n]` modulo the texture's frame count. This is what gives the visual sense of "spinning" / animated fire — the texture cycles per tick. See §8.
- **Per-vertex color packing:** none — the Render body does NOT write per-vertex diffuse. Color comes from the texture × the MODULATE blend stage's default vertex color (whatever the I3D's authored `lverts[k].color` is).
- **Draw order in `Render` (`missileeffect.cpp:1663-1696`):** the loop is `for n = 0..NUM_FLAME_SPARKS`, so **draw order is: disc first (n=0), then sparks 1, 2, 3, 4, 5 in that order**. Within the missile family, disc drawn before sparks = sparks overdraw the disc edges when their `p[n]` lands on top of it — typically harmless because sparks emit *behind* the disc (rear ±45° cone). No CULLMODE override (unlike FireColumn which sets `CULLMODE = NONE` at `missileeffect.cpp:1352-1353`); the default cull may clip the back face of the ground-tipped quad — visual unclear, see §13.

---

## 8. Texture animation

**Mechanism: `framehtexs[]` flipbook** — per-tick engine texture-handle swap, NOT UV scroll, NOT UV sub-rect atlas.

- **How it works.** The I3D's single texture slot carries `numframes` engine texture handles in `framehtexs[]` (see `src/3dimage.h:43-50` for `S3DTex` layout). The animator reads `numtexframes = tex.numframes` once at Initialize (`missileeffect.cpp:1507-1508`). At Render, `frame = framenum[n] % numtexframes` (`missileeffect.cpp:1689`) selects which framehtexs index to bind for that draw. Same mechanism as `TFireAnimator` (F03b) and Photon's central flare.
- **Rate.** `framenum[n]` increments **+1/tick** in Animate (LAUNCH: only `framenum[0]` for the disc; FLY/EXPLODE: `framenum[1..5]` for the sparks; sparks while framenum<0 also tick toward 0 during the birth delay).
- **Wrap.** Modulo `numtexframes`. (No `% (numframes-1)` or skip-a-frame logic like F07 fireball's `skip glow_frame`.)
- **Per-instance phase offset.** Each slot is born with `framenum = -random(2,8)`, so after birth (the moment `framenum` crosses 0) the slots are out of phase — the disc and the 5 sparks all start the flipbook at different frames, desynchronized. Together with the per-spark rebirth (FLY), every spark you see is at a random texture phase. This is what conveys the "fire shimmer" impression and (probably) the "spinning" of the disc — the flipbook frames likely include rotation built into the texture (e.g. an authored 8-frame spin cycle of the disc texture).
- **Sub-object sharing.** Both sub-objects (`spark`, `flamedisc`) share the same I3D texture slot — same flipbook. The "two different visuals" come from the I3D's authored *geometry* (size + the embedded UVs picking different parts of the same texture would only be true if the UVs differed, but per §4 both quads use (0,0)/(1,1)/(0,1)/(1,0) full-texture UVs — so both quads render the **identical texture** content at the current frame, just at different positions/scales).

If `numtexframes == 1` the texture is static (one frame); the effect's "spin" then collapses to "flat disc + flat sparks." Since the snapshot reads `numtexframes` at runtime, the actual frame count is asset-dependent — read it from the legacy I3D at reconstruction (the asset has multiple frames per the `framehtexs[]` table; a deeper I3D-decode would enumerate them but isn't required here since the engine reads it dynamically).

---

## 9. Associated light

**No associated dynamic light** — the snapshot has no `LIGHT` directive in the (non-existent) spell.def variant that would invoke FlameDisc, and the animator body does NOT touch the scene-light system. Unlike `TFireBallEffect` which gets its warm point light from the `spell.def:844` `LIGHT COLOR 255,130,0 INT 255 MULT 20` directive, there is **no spell.def block for FlameDisc** (confirmed by grep across both legacy and shipped spell.def files — §2.1.4) and the animator does not create a light directly.

Since the effect is vestigial in shipped (§12), no spell-time light wiring was ever finalized. The reconstruction should ship **no light** unless visual self-test against the asset demands one — in which case use the missile-family default (a warm orange point light, same MULT 20 as the other fire spells, e.g. `(255,130,0)` INT 255 MULT 20 to match FireBall). **Default: no light, matching the snapshot.**

---

## 10. Color

- **Source:** the `Magic\FlameDisc.I3D` mesh's embedded texture (read via `Get3DImagery()->GetTexture(0,&tex)` then bound per-frame via `framehtexs[frame]`). No per-vertex color repaint in the Render body. No spell-color tint (no spell driver). No hardcoded `D3DRGBA` in the animator.
- **Exact values:** none in the source — color is entirely **texture-driven**. The asset is on disk at `legacy/Imagery/Magic/FlameDisc.I3D` (89 KB). The embedded texture's color content is in the binary (not decoded byte-by-byte in this forensics pass — visual self-test against the asset is the right verification step).
- **Expected visual:** warm orange / yellow fire matching the missile family aesthetic. The asset name (`FlameDisc`) and the LIGHTNING sound key (which is used family-wide for fire/projectile effects, not literal lightning) imply a warm fire palette. If the reconstruction reads pale / grey / off-color, the port is broken (stand-in texture, wrong blend, missing chroma-key, or premultiply-alpha conversion miss) — see [feedback-vfx-color-health-signal].
- **Normalization / boosts:** none — Render body has no `NormalizeColors`, no hilt-brightness math, no per-vertex tint. The texture provides the hue directly.

---

## 11. Audio coupling

- **One sound — `LIGHTNING_SOUND = "lightning"`** (`src/effect.h:473`).
  - **Mount on Initialize:** `SoundPlayer.Mount(LIGHTNING_SOUND)` (`missileeffect.cpp:1468`) — loads the sample.
  - **Play on FLY-entry (one-shot):** `SoundPlayer.Play(LIGHTNING_SOUND)` (`missileeffect.cpp:1480`), gated by `firsttime` so it fires exactly once at the LAUNCH→FLY transition.
  - **Unmount on destruction:** `~TFlameDiscEffect: SoundPlayer.Unmount(LIGHTNING_SOUND)` (`missileeffect.h:309`).
- **Retail confirmation.** The Pulse decomp at `cls_0x5b45ac.cpp:24-40` plays the sound exactly once on FLY-entry via `cls_0x41c7d0::meth_0x49b990` (the engine audio play call) with arg `0x7f` (mid volume) and arg `1` (some mode flag). The XREF of the `"lightning"` string lands +0x26 in this class. **§2.1.5 corroborates the snapshot's Mount/Play exactly.**
- **No other audio** — no looping ambience, no impact sound, no per-spark birth/death sound.
- The legacy `.I3D` may carry tag-driven sounds per (state, frame) per knowledge-base `02 §8`, but none are documented for FlameDisc in the snapshot.

The "lightning" sound key is the shared family audio handle — also used by `TPhotonEffect` (`missileeffect.cpp:156, 168`) and *commented-out* in `TFireColumnEffect` (`missileeffect.cpp:1167, 1183`). The sound asset itself is whatever the engine resolves `"lightning"` to in the shipped audio bank — not a literal lightning crackle but a generic spell-projectile "whoosh."

---

## 12. Triggers & in-game appearance

- **Spawned by:** **NOBODY in the shipped game.**
  - Registration: `DEFINE_BUILDER("FLAMEDISC", TFlameDiscEffect)` (`missileeffect.cpp:1454`), `REGISTER_BUILDER(TFlameDiscEffect)` (`:1455`), `REGISTER_3DANIMATOR("FLAMEDISC", TFlameDiscAnimator)` (`:1488`) — present in source.
  - The retail binary contains the class (cls_0x5b45ac, §2.1) and the `"FLAMEDISC"` registry string with XREFs from a builder/registrar site.
  - **BUT:** no `"FlameDisc"` row in the shipped `class.def` (`data/imagery.rvi:class.def`) — only legacy `Class.Def:2033` has it.
  - **AND:** no spell.def variant invokes `"FLAMEDISC"` (grep -i across `legacy/spell.def`, `data/Resources/spell.def`, `data/resources_unzipped/spell.def` — zero matches).
  - **AND:** the asset `Magic\FlameDisc.I3D` is missing from shipped `data/imagery.rvi`.
- **Where to see it in the original game:** **nowhere.** The effect is **vestigial / pre-release WIP** — the class exists in the binary but cannot be instantiated by any shipped script, spell, or registry path. (Compare: `TFireColumnEffect` is similarly orphaned in shipped, missing both asset and spell variant. The pre-release plan likely had a "Flame Disc" spell that was cut or rolled into another effect — perhaps `TFireColumnEffect`'s impact, given the FLAMEDISC ↔ FireColumn class adjacency at cls_0x5b45ac / cls_0x5b4534 per §2.1.4.)
- **Ground truth for the reconstruction:** **the legacy asset + this forensics doc + the snapshot animator code.** There is no in-game footage to capture against. Visual self-test must compare against:
  1. The legacy `Magic\FlameDisc.I3D` rendered with the right blend + WorldXY orientation (does the disc look like the authored asset?).
  2. The 5-spark rear-cone behavior on a moving missile (does the spray look right behind the disc?).
  3. The flipbook texture animation (does the disc texture cycle visibly?).
- **Reconstruction harness note (for the reconstruction agent, not this doc's scope):** since there's no live spell, the effect must be exercised via a `--test=vfx` test-mode harness that spawns it standalone at a fixed origin with a chosen aim direction. Document everything else from the snapshot.

---

## 13. Gaps & uncertainties

1. **All §3 tuning constants are snapshot-only.** Spark count (5), scale step (0.2), max scale (3.0), launch radius (20), birth delay (random 2..8), rear-cone half-width (±32 byte-angle = ±~45°) — none readable in the Ghidra decomp. The retail Pulse confirms only the FLY-entry sound trigger. **The effect is vestigial in shipped — no in-game ground truth exists.** Reconstruction must work from the snapshot exactly and visually self-test against the asset.
2. **Effect is vestigial in shipped (§12).** No spell, no class.def entry, no asset shipped. The retail binary contains the class shell but the effect cannot be instantiated. Reconstruction can build it but cannot validate against retail footage.
3. **The `obj->scl = scale[0]` render-time scale (§7).** All quads (disc AND sparks) are scaled by `scale[0]` instead of per-slot `scale[n]`. This appears to be a snapshot bug (the per-spark `scale[n]` decay computation in Animate is otherwise carefully written and matches sister Photon's per-slot pattern). **Strong recommendation:** the reconstruction uses `obj->scl = scale[n]` per quad (matches intent + sister effects). Document the deviation explicitly in the reconstruction notes.
4. **The "spinning disc" — `rot.z = 0` per tick (§3, §6.3).** The snapshot's Render body explicitly sets `obj->rot.z = 0` every tick, so there's no per-tick z-rotation on the disc geometry. The disc's *spin* impression must come from the **texture flipbook** cycling (per-tick `frame` advance, §8). If the asset's texture frames don't include rotation, the disc visually does NOT spin — just sits and flickers. Two reasonable readings:
   - **(a) Snapshot intent matches code:** no geometric spin, the flipbook texture supplies any rotation animation. (Most likely — matches Photon's static-geometry pattern.)
   - **(b) WIP intent was geometric spin:** the developer left `rot.z = 0` as a TODO and meant `rot.z = some_rate * t`. (Possible — the asset name "Spinning Flame Disc" in the source comment `missileeffect.cpp:1494` "scaling, rotating disc" hints at rotation.)
   **Resolution:** decode the legacy I3D's texture frames to see if rotation is baked in. If yes → (a). If frames are static → reconstruction may add a per-tick `rot.z` ramp matching the spell-family vibe. Not blocking for the initial port.
5. **LAUNCH→FLY hand-off without `SetStatus(true)` (§5.1).** The snapshot's `TFlameDiscAnimator::Animate` calls `inst->SetCommandDone(true)` when the disc reaches max scale (`missileeffect.cpp:1561`), but DOES NOT call `((PTMissileEffect)inst)->SetStatus(true)`. The base `TMissileEffect::Pulse` LAUNCH branch keys off `if (status)` to do the aim→velocity conversion (`missileeffect.cpp:60-71`). So **how does FlameDisc actually transition to FLY?** Either (a) `SetCommandDone` triggers the transition through some pathway not in the snapshot (engine-side), or (b) the snapshot is incomplete and a `SetStatus(true)` call is missing — compare to `TPhotonAnimator::Animate` (`missileeffect.cpp:285-286`) which DOES call `SetStatus(true)` when its launch flourish completes. **Most likely:** the snapshot is WIP; reconstruction should add `SetStatus(true)` next to `SetCommandDone(true)` to match the family pattern. Note for the reconstruction agent.
6. **`v[0]` is dead data (§6.2).** The animator's Initialize loop assigns `v[0]` (the disc's velocity) but Animate never integrates `p[0] += v[0]` — only sparks `n=1..5` move locally. The disc tracks the missile's world position via `obj->pos`-fallback. Pre-release artifact; reconstruction can ignore `v[0]`.
7. **Texture frame count is asset-dependent.** `numtexframes` is read at runtime (`missileeffect.cpp:1508`) from the legacy I3D. A deeper I3D decode of `Magic\FlameDisc.I3D` to enumerate `numframes` and dump the per-frame texture content would resolve "spin via texture vs static texture" (§13.4). Out of scope for this pass; the engine will read it on load.
8. **No `RefreshZBuffer` override (§7).** Unlike `TFireBallEffect` which carefully repairs scene Z under each sub-object, `TFlameDiscAnimator` declares no `RefreshZBuffer`. If the deferred renderer needs Z-repair for the ground-tipped quads (e.g. to read correctly against scene depth at the disc/spark footprints), the reconstruction must add one. Not in the snapshot — pre-release WIP, or simply not needed because the disc/sparks read transparently under TestNoWrite.
9. **CULLMODE not overridden (§7).** Unlike FireColumn (`CULLMODE = NONE`), FlameDisc keeps the default CCW cull. The ground-tipped quad's authored normal points +Z (per the `nz = +1` in the verts at offset 0x1c10+, §4 decode); under WorldXY tipping the normal points up — should render visible from above. From below, the back face is culled (invisible). Probably fine for a ground effect, but if the camera ever dips below ground plane, the disc would disappear. Note for reconstruction.
10. **Snapshot blend = Alpha; render-bodies-not-decompiled risk (§7).** The render blend may have shipped Additive for the warm glow read (sister sparks lesson). No retail capture exists to vet against (vestigial effect). **Build to snapshot-Alpha first; if visual self-test reads weak, AdditiveStraight is the next-best hypothesis.** Flag this explicitly in the reconstruction's visual review.

---

## 14. Reconstruction burndown

```
[BASE — shared with Photon / FireBall / FireColumn — see F07 §14 for full BASE list]
- [ ] TMissileEffect 3-state machine: LAUNCH→FLY→EXPLODE→die (§6.1, F07 §6.1)
- [ ] launch handshake: animator SetStatus(true) → base converts aim→vel,
      OF_MOVING|OF_WEIGHTLESS, vel.z = speed/-16, range=(240*2)/(speed/ROLLOVER)=30 ticks @ speed 16 (§5.1)
- [ ] FLY impact detection: range countdown, MOVE_BLOCKED, char-within-32wu enemy check (§6.1)
- [ ] OffScreen override: keep simulating until exploding-and-done (§6.1)

[FLAMEDISC]
- [ ] load Magic\FlameDisc.I3D from the LEGACY copy (NOT shipped); address GetObject(0)=spark, GetObject(1)=flamedisc (§4)
- [ ] spawn-position cubic jitter: pos.x/y/z += random(±20) BEFORE TMissileEffect::Initialize runs (§5.1)
- [ ] Mount LIGHTNING_SOUND ("lightning") at Initialize; Unmount in dtor (§11)
- [ ] play LIGHTNING_SOUND once on FLY-entry (firsttime edge in Pulse override) (§6.1, §11)
- [ ] FLAMEDISC_SPEED = 16 (overrides base; even though base default is also 16 — keep the explicit SetSpeed call) (§3)
- [ ] disc orientation: rot.x = -π/2 (WorldXY ground-tipped), rot.y = 0, rot.z = 0 — applies to ALL 6 quads (§7)
- [ ] disc grow: scale[0] starts 0.0, framenum[0] = -random(2..8) delay, then +0.2/tick until scale[0] = 3.0
      (FLAMEDISC_MAX_SCALE_STEP), then SetCommandDone(true) to end LAUNCH (§6.3)
- [ ] [GAP §13.5] also call ((PTMissileEffect)inst)->SetStatus(true) at LAUNCH-done to actually trigger the base's
      aim→velocity conversion (snapshot likely missing this — sister Photon does it; per family pattern)
- [ ] FLY: disc just sits at scale 3.0 at the missile center; per-tick 5 sparks rebirth & integrate (§6.3)
- [ ] spark birth: scale[n] = 3.0, activesparks++ on the tick framenum[n] hits 0 (§6.3)
- [ ] spark decay: scale[n] -= 0.2/tick; p[n] += v[n]/tick (v.z = 0, ground plane) (§6.3)
- [ ] spark direction: ConvertToVector((angle + 128 + random(-32,32)) & 255, GetSpeed()=16, pnt) → v = (pnt.xf, pnt.yf, 0)
      — rear ±~45° hemisphere of the disc's aim direction (§5.3)
- [ ] FLY spark rebirth: on scale[n] <= 0, re-seed framenum = -random(2..8), p = 0, fresh random v (§6.3)
- [ ] EXPLODE drain: NO rebirth — on scale[n] <= 0 set framenum = -32767 (permanent mute) (§6.3)
- [ ] EXPLODE termination: when activesparks == 0 → SetCommandDone(true) → animator done → base sees !HasAnimator → KillThisEffect (§6.3, §6.1)
- [ ] render blend: SetBlendState (Alpha — MODULATE + SRCALPHA/INVSRCALPHA), self-lit / Unlit, ZTest no-write (§7);
      flagged as snapshot-only risk per §13.10 — visual self-test; AdditiveStraight is the fallback hypothesis
- [ ] per-quad transform flags: OBJ3D_ROT1 | OBJ3D_SCL2 (disc) + |OBJ3D_POS3 (sparks, with obj->pos = p[n]) (§7)
- [ ] [GAP §13.3] use obj->scl = scale[n] per-quad (NOT scale[0] as written in the snapshot — that's a typo);
      fixes the smooth spark decay; matches sister Photon
- [ ] frame = framenum[n] % numtexframes per quad — flipbook texture cycling via framehtexs[] (§8)
- [ ] only draw when framenum[n] >= 0 (silent during birth-delay window) (§7)
- [ ] sub-object mapping: n == 0 → GetObject(1) (flamedisc); n != 0 → GetObject(0) (spark) (§7)
- [ ] no associated light (snapshot has none; no spell-driven LIGHT directive) (§9)
- [ ] color: warm orange/yellow fire from the legacy I3D's embedded texture (texture-driven, no vertex tint) (§10)
- [ ] audio: LIGHTNING_SOUND played once on FLY-entry (already covered above) (§11)
- [ ] [VESTIGIAL §12] effect cannot be invoked in shipped — wire as standalone test-mode (--test=vfx) only;
      visual ground-truth = asset render + this doc, NOT in-game footage
- [ ] [OPEN §13.4] decide whether to add per-tick rot.z spin to the disc — depends on whether the asset's flipbook
      frames already include rotation; if no rotation in the texture frames, port may add a rot.z ramp;
      if yes, leave rot.z = 0 per the snapshot
```
