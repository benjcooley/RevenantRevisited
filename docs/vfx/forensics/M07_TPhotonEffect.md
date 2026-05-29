# M07 TPhotonEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | M07 (covers `TPhotonEffect` + `TPhotonAnimator`) |
| **Class(es)** | `TPhotonEffect : TMissileEffect : TEffect` + `TPhotonAnimator : T3DAnimator`. The base **`TMissileEffect`** machinery (travel + impact state machine) is documented as **[BASE]** in [F07 §5–§6](F07_TFireBallEffect.md); this doc cites it without redocumenting and captures only photon-specific deltas. |
| **Status** | forensics-complete (see §13) |
| **Retail fidelity** | **retail-partial** — `Magic\Photon.I3D` is **byte-identical** to the shipped `data/imagery.rvi:Imagery/Magic/photon.i3d`; the `TPhotonEffect` subclass `cls_0x5b3c18` is structurally a perfect match (Initialize sets `range=0x8000`, `speed=0x100000`, `firsttime=1`; Pulse on FLY-entry plays a sound — exactly the snapshot pattern); the `TMissileEffect` base Pulse logic at `cls_0x5b401c::virt_meth_0x510220` carries the snapshot's literal immediates (`0x1e0/(speed>>16)` flight range, `0x21=33 wu` char-hit gate, `vel.z = -(speed>>4)`); the `"Photon"` registry-string XREF is confirmed at `005e1b20`. The `TPhotonAnimator`'s **per-spark kinematics** (the 32-spark choreography: 16 launch-flourish sparks + 16 trailing photons, `PHOTON_LAUNCH_RADIUS=20`, `PHOTON_LAUNCH_DURATION=25`, `PHOTON_LAUNCH_SCALE_STEP=0.25`, the EXPLODE burst `±3 * 3/2` velocity spread) are **snapshot-only** — the animator class wasn't isolated by Ghidra. See §2.1. |
| **Author / Date** | vfx-forensics-agent (worktree wf_b5d7064f-682-3) / 2026-05-29 |
| **Family** | magic (projectile / missile sub-family) — base shared with Fire Ball / FireColumn / FlameDisc / IceStorm / YFireBall |
| **Draws** | composite — a single I3D billboard quad (`photon` sub-object) drawn **up to 32 times** at varying positions/scales: 1 central flare + 15 satellite **launch sparks** (LAUNCH state), then 1 lead photon + 15 **trailing photons** (FLY state), then 16 **explosion sparks** (EXPLODE state, with 16 lingering trailers) + scene point light (carried via the `"lightning"` sound mount — actually a **sound coupling**, not light, see §9) |
| **Archetype(s)** | **A** UV-anim (per-spark `frame = framenum % numtexframes` cycles the bound texture), **B** I3D-geo (`photon` quad sub-object), **D** trail (16-slot delayed trailing-photon ring spawned at FLY entry, positions back-projected along the velocity vector), **E** simple particle emitter (LAUNCH flourish + EXPLODE burst — straight-line velocity, scale envelope, no gravity), **F** custom 3-state choreography (LAUNCH "sparks converge into the flare → photon launches" handshake + EXPLODE "sparks fly out" burst), **H** audio (LIGHTNING_SOUND mount/play on FLY entry). NO associated light (§9). NO gravity (§3). |

---

## 1. Summary

The Photon is a **magic energy bolt** missile: a small bright sparkle texture used 32× simultaneously to draw a multi-stage energy-bolt visual. While **launching** (LAUNCH) it plays a contracting-sparkle "spell-charge" flourish — a 1.0-scale central flare with 15 sub-sparks spawned in a ±20-wu cube around the caster that move toward the center, scaling up then down; when 2/3 of the sparks have died the base missile is told to launch. While **flying** (FLY) it draws one large leading flare at the effect's world position with **16 delayed trailing photons** spaced back along the velocity vector, each `1 wu/ROLLOVER` further behind the previous — a flickering energy trail. On impact (EXPLODE) all 32 slots fire **outward sparks** with random ±3·(3/2)=±4.5 wu/tick isotropic velocity and a 3.0→0 shrink, plus the trailing photons continue to drain. The bolt mounts the `"lightning"` sound on `Initialize` and triggers `PLAY("lightning")` on the LAUNCH→FLY edge (the bolt-launch zap). It carries no dynamic scene light. The whole effect is built on the shared `TMissileEffect : TEffect` travel base ([F07 §5–§6 [BASE]](F07_TFireBallEffect.md)) — the same 3-state LAUNCH/FLY/EXPLODE machine that the Fire Ball uses, with `PHOTON_SPEED=16` (vs. fireball's 8 → photon is twice as fast and travels half the time per range unit).

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/classes/cls_0x5b3c18.cpp` — TPhotonEffect (404 bytes, vftable 128 slots; structure confirmed by `Initialize` writing `range=0x8000` & `speed=0x100000` & `firsttime=1`, and `Pulse` checking `state==1` + `firsttime` then calling sound `Play`). **STRUCTURE+SOUND-DISPATCH CONFIRMED**, no method bodies for the animator.
  - `recon/classes/cls_0x5b401c.cpp` — TMissileEffect base (1288 bytes, vftable 24 slots). Pulse at `virt_meth_0x510220` has the literal immediates that match snapshot `TMissileEffect::Pulse`: `0x1e0 = 240 = 240*MISSILE_RANGE`, char-hit gate `0x21 = 33` (≈ the `>32` Distance check), `-((speed+(speed>>0x1f & 0xf))>>4)` is `vel.z = -speed/16`, OF_MOVING/OF_WEIGHTLESS flag set as `0x10008 | ~1`. **MISSILE-BASE PULSE CONFIRMED.**
  - `recon/mappings/TPhotonEffect_cls_0x5b401c_candidate.yaml` — superseded by reading: cls_0x5b401c is actually the **TMissileEffect** base (size 1288 fits the TEffect-inherited footprint + missile's `range`/`speed`/`status`); cls_0x5b3c18 (404 bytes) is **TPhotonEffect** itself (carries `firsttime` flag mbr_0x190 and the LIGHTNING_SOUND mount/play). See §13.1.
  - `recon/classes/_data.txt:108674-108716` — multiple "lightning" string slots at `005e1a60` / `005e1a6c` / `005e1aec` / `005e1af8` (XREFs to `virt_meth_0x510400` = Photon Initialize and `virt_meth_0x510450` = Photon Pulse, plus more — see §11).
  - `recon/classes/_data.txt:108733-108734` — `"Photon"` string at `005e1b20` (the registry name, XREF'd into the Photon class's builder/dispatch path).
- **Pre-release (snapshot — authoritative for the animator):**
  - `src/missileeffect.h:18-115` — `TMissileEffect` base + `TPhotonEffect` subclass + `TPhotonAnimator` class declarations (with all the `PHOTON_*` defines).
  - `src/missileeffect.cpp:31-141` — `TMissileEffect::Pulse` shared body ([BASE], see F07 §6.1).
  - `src/missileeffect.cpp:147-170` — `TPhotonEffect` builder + `Initialize` + `Pulse` (sound mount + FLY-entry play).
  - `src/missileeffect.cpp:176-471` — `TPhotonAnimator` animator (`Initialize`, `Animate`, `Render`, `RefreshZBuffer`).
  - Whole file is gated `#if 0` (`:15`/`:1702`) — pre-port snapshot preserved verbatim.
- **Sister effects consulted:**
  - `TFireBallEffect` / `TFireBallAnimator` (`missileeffect.cpp:474-1156`) — same `TMissileEffect` base; the comprehensive **[BASE]** reference for travel + impact is in [F07_TFireBallEffect.md §5.1, §6.1](F07_TFireBallEffect.md). Confirms `SetBlendState` (Alpha) is the sister convention.
  - `TFlameDiscEffect` (`missileeffect.cpp:1380-1700`) — also mounts `LIGHTNING_SOUND` (`:1393`) and plays on LAUNCH→FLY edge (`:1421`); identical audio handshake.
  - `TFireColumnEffect` (`missileeffect.cpp:1158-1377`) — same `TMissileEffect` base, sound commented out.
- **Asset:** `legacy/Imagery/Magic/Photon.I3D` (48,340 B) registered `"Photon"` (`Class.Def:2018`).
- **Source-of-truth ranking:** snapshot `missileeffect.cpp` is authoritative for *the animator* (the only readable body). Retail decomp corroborates *structure + base-Pulse immediates + asset + registration + the LIGHTNING_SOUND handshake on FLY*. Where they speak: retail wins; recorded as divergence in §2.1 / §13.

### 2.1 Retail-vs-snapshot reconciliation (verdict: **retail-partial**)

**1. Asset identity — CONFIRMED IDENTICAL.** The shipped `data/imagery.rvi`
(a stored ZIP) member `Imagery/Magic/photon.i3d` is **byte-for-byte identical**
to the legacy disk copy:

```
md5  a9e3e83ae8d1caae171da2823ef9a86f   data/imagery.rvi:Imagery/Magic/photon.i3d   (48340 B, dated 1998-05-05)
md5  a9e3e83ae8d1caae171da2823ef9a86f   legacy/Imagery/Magic/Photon.I3D            (48340 B)
cmp → IDENTICAL
```

The mesh + texture the shipped photon consumes are exactly what the snapshot
animator addresses (the registry points at `Magic\Photon.I3D`,
`Class.Def:2018`). Strong corroboration that the *visual asset* did not change.
(`data/imagery.rvi` also carries `Imagery/Magic/redphoton.i3d` — a different
asset, 48284 B, 1998-01-29 — apparently a dev variant; the live registry uses
`Photon.I3D`, not `redphoton.i3d`.)

**2. TPhotonEffect (subclass) structure — CONFIRMED.** `cls_0x5b3c18` (404 bytes,
128-slot vftable):
- `virt_meth_0x510400` (Initialize, vtable slot 508 = +0x1FC): writes `mbr_0xe0=0`,
  `mbr_0x184=0x8000`, `mbr_0x188=0x100000`, then calls `cls_0x41c7d0::meth_0x49b880`
  (Miles `AIL_*` audio mount → `SoundPlayer.Mount`) and sets `mbr_0x190=1`. This is
  the exact body of `TPhotonEffect::Initialize` (`missileeffect.cpp:150-159`):
  `TMissileEffect::Initialize()` sets `range=32768=0x8000` and `SetSpeed(16) → speed=16*ROLLOVER=0x100000`,
  then `SoundPlayer.Mount(LIGHTNING_SOUND); firsttime=true`.
- `virt_meth_0x510450` (Pulse, vtable slot 272 = +0x110): calls into
  `cls_0x5b401c::virt_meth_0x510220` (TMissileEffect base Pulse), then `if (mbr_0xc==1 && mbr_0x190 != 0)` → set `mbr_0x190=0`, then `meth_0x49c430` (read mount handle) + `meth_0x49b990` (`AIL_start_sample` → `SoundPlayer.Play`). That is **exactly** the snapshot's `TPhotonEffect::Pulse` (`missileeffect.cpp:161-170`): "if `state==MISSILE_FLY (1)` and `firsttime`, clear firsttime, `SoundPlayer.Play(LIGHTNING_SOUND)`."
- Field map: `mbr_0xc = state` (short at +12), `mbr_0x184 = range` (+388 dword),
  `mbr_0x188 = speed` (+392 dword), `mbr_0x18c = (status?)` (+396, set to 1 at Init —
  the snapshot only sets `status=false` and that's at offset depending on layout; the
  retail layout has 1 here at Init, then the base Pulse clears it — see point 3), `mbr_0x190 = firsttime` (+400 byte/dword).
- The snapshot's `TPhotonEffect::Initialize` (`missileeffect.cpp:150-159`) calls
  `TMissileEffect::Initialize() → SetSpeed(PHOTON_SPEED) → SoundPlayer.Mount(LIGHTNING_SOUND) → firsttime = true`. The retail body matches **operation-for-operation**.

**3. TMissileEffect (base) Pulse immediates — CONFIRMED.** `cls_0x5b401c::virt_meth_0x510220` (the base Pulse) carries:
- `sVar1 = *(short*)&this->mbr_0xc` reads the state. The 3 cases are 0 (LAUNCH), 1 (FLY), 2 (EXPLODE) — matching `MISSILE_LAUNCH/FLY/EXPLODE = 0/1/2` (`missileeffect.h:20-24`).
- LAUNCH branch: if `mbr_0x18c != 0` (= `status`) and `mbr_0x188 != 0` (= `speed`), calls
  `cls_0x5b8e94_TObjectInstance::meth_0x4df070` (GetAngle) then `FUN_0046db20_ConvertToVector(angle, speed, &mbr_0x1c, 0)` — the snapshot's `ConvertToVector(GetAngle(), speed, vel)` (`missileeffect.cpp:64`); sets `mbr_0x8 = mbr_0x8 & ~1 | 0x10008` (OF_MOVING|OF_WEIGHTLESS, clearing OF_IMMOBILE — matches `:65`); writes
  `mbr_0x24 = -((speed + (speed>>0x1f & 0xf)) >> 4)` (= `vel.z = -speed/16` with proper signed division rounding — matches `:66`); writes `mbr_0x184 = 0x1e0 / (speed>>16)` (= `range = (240*MISSILE_RANGE=2) / (speed/ROLLOVER) = 480 / (speed/65536)`, which for the literal `0x1e0=480` matches `240 * MISSILE_RANGE=2` — `:68`).
- FLY branch: decrement `mbr_0x184` (range--, `:79`); if `(bVar2 & 2) == 0` (no MOVE_BLOCKED, `:84`), iterate characters with `cls_0x44ceb0` (TMapIterator), skip the invoker (`this_00 == iter`), check the iter is alive (`virt_meth_0x477e50_448` = IsDead negated), then check `Distance < 0x21` (= `<= 32` — matches the snapshot's `Distance(chr) > 32 → continue`, i.e. ≤32 hits — `:105`), then IsEnemy gate (`meth_0x4c89c0_IsEnemy`, matches `:108`); on any hit, jump to `LAB_00510310` → set flags `& ~0x1008 | 1` (clear OF_MOVING|OF_WEIGHTLESS, set OF_IMMOBILE — matches `:120`), then `virt_meth_0x5104c0_24` (= SetState — `:121`).
- EXPLODE branch: if `mbr_0x58 == 0` (= `!HasAnimator()`), call `meth_0x4defe0` (= `KillThisEffect`-equivalent, matches `:128-129`).

**Every immediate in the snapshot `TMissileEffect::Pulse` body has a matching immediate in retail.** This is the strongest possible structural confirmation short of a full body decomp.

**4. Registration + naming — CONFIRMED.** The retail binary registers `"Photon"`
at the rdata string `s_Photon_005e1b20` with 2 XREFs (`recon/classes/_data.txt:108733`):
- `005103e0` (within `virt_meth_0x510220` in cls_0x5b401c) — likely a registry XREF (`DEFINE_BUILDER("Photon", …)` matches `missileeffect.cpp:147`).
- `005104a0` (within `virt_meth_0x510450` in cls_0x5b3c18) — likely a registry XREF (`REGISTER_3DANIMATOR("Photon", …)` matches `missileeffect.cpp:176`).
The animator class for `"Photon"` isn't isolated in the decomp, but the registration string itself + the matching subclass dispatch are confirmed.

**5. Audio handshake — CONFIRMED.** The shipped binary mounts a sound on the
TPhotonEffect Init path (`meth_0x49b880` inside `virt_meth_0x510400`) and plays it
on the FLY-entry edge (`meth_0x49b990` inside `virt_meth_0x510450`). The `"lightning"`
string at `005e1a60` is XREF'd from `virt_meth_0x510400` (`_data.txt:108674`) and at
`005e1a6c` is XREF'd from `virt_meth_0x510450` (`_data.txt:108680`) — i.e. the
Initialize **mounts** "lightning" and the Pulse **plays** "lightning" on FLY entry.
This matches `TPhotonEffect::Initialize` calling `SoundPlayer.Mount(LIGHTNING_SOUND)`
(`missileeffect.cpp:156`, `LIGHTNING_SOUND = "lightning"`, `effect.h:473`) and
`TPhotonEffect::Pulse` calling `SoundPlayer.Play(LIGHTNING_SOUND)` on the FLY-entry
edge (`missileeffect.cpp:168`).

**6. `TPhotonAnimator` constants — NOT corroborated (snapshot-only).** The animator
class (which holds the 32-spark choreography: `PHOTON_LAUNCH_RADIUS=20`, `PHOTON_LAUNCH_DURATION=25`,
`PHOTON_LAUNCH_SCALE_STEP=0.25`, the FLY-entry lead-photon `scale=3.5`, the EXPLODE
spark `scale=3.0` + `velocity = ±3 * 3/2` isotropic = ±4.5 wu/tick, etc.) is **not
isolated** in the recon. The animator-specific tuning constants in §3 are best-evidence
from the snapshot but **unverified against shipped retail**.

**7. Spell-side / caller wiring — NO LIVE CALLER CONFIRMED (vestigial-or-NPC).**
`grep -i photon spell.def rules.def *.def` finds **no** `EFFECT "Photon"` /
`SPELL Photon` / `MAGICATTACK *photon*` / `INVOKE *photon*` reference anywhere in
the shipped data (verified across `spell.def`, `cbspell.def`, `rules.def`, etc.).
The Class.Def imagery registration exists (`Class.Def:2018`) and the builder is
registered (`missileeffect.cpp:147`), but the snapshot has **no spell or attack
caller**. The retail binary has the registration but no surviving caller XREF was
located; the effect is best regarded as **vestigial in the shipped data we have**
or, alternatively, was driven by a now-dropped dev/test path. (Compare F07 fireball,
which has 2 live `spell.def` variants.) See §12.

**Verdict — retail-partial.** *Asset, the TPhotonEffect subclass structure, the TMissileEffect base Pulse immediates, the registration string, and the LIGHTNING_SOUND mount/play handshake* are retail-confirmed. The *32-spark animator choreography numbers* (scales, radii, durations, EXPLODE velocities) and the *caller wiring* are snapshot-only / open. The reconstruction's fidelity risk is concentrated in the §3 animator constants and the §12 trigger question.

---

## 3. Constants

`[BASE]` = lives in `TMissileEffect` (shared by FireBall/FireColumn/FlameDisc/IceStorm —
fully described in [F07 §3 [BASE]](F07_TFireBallEffect.md)).  
`[PH]` = photon-specific.

| name | value | units | scope | source | confirmed? |
|------|-------|-------|-------|--------|------------|
| MISSILE_SPEED (default) | 16 | pos-units/tick (pre-ROLLOVER) | [BASE] | missileeffect.h:26 | yes (retail: `mbr_0x188 = 0x100000 = 16*ROLLOVER` at base Init, cls_0x5b401c::virt_meth_0x5101b0:55-57) |
| MISSILE_RANGE | 2 | range multiplier | [BASE] | missileeffect.h:27 | yes (retail: `0x1e0 = 480 = 240*2` immediate in base Pulse, cls_0x5b401c::virt_meth_0x510220:95) |
| range (initial) | 32768 (`0x8000`) | ticks (huge until launched) | [BASE] | missileeffect.cpp:35 | yes (retail: `mbr_0x184 = 0x8000` at Init, cls_0x5b3c18::virt_meth_0x510400:42 and cls_0x5b401c::virt_meth_0x5101b0:55) |
| range (flying) | `(240*MISSILE_RANGE)/(speed/ROLLOVER) = 480/(speed/65536)` | ticks of flight | [BASE] | missileeffect.cpp:68 | yes (retail: same expression at cls_0x5b401c::virt_meth_0x510220:95) |
| ROLLOVER | 65536 (`1<<16`) | sub-unit precision | [BASE] | revdefs.h:447 | yes (retail: shifts by 0x10 throughout base Pulse) |
| char-hit gate | 32 | wu (Distance check during FLY) | [BASE] | missileeffect.cpp:105 | yes (retail: `< 0x21 = ≤32` immediate, cls_0x5b401c::virt_meth_0x510220:118) |
| vel.z on launch | `speed / -16` | pos-units/tick (slight downward arc) | [BASE] | missileeffect.cpp:66 | yes (retail: `-(speed+(speed>>31&0xf))>>4` = `vel.z = -speed/16` rounded, cls_0x5b401c::virt_meth_0x510220:93) |
| states | LAUNCH=0, FLY=1, EXPLODE=2 | enum | [BASE] | missileeffect.h:20-24 | yes (retail: switch on `mbr_0xc` for 0/1/2 in cls_0x5b401c::virt_meth_0x510220:82,101,132; I3D state names `missilelaunch`/`missilefly`/`missileexplode`) |
| **PHOTON_SPEED** | 16 | pos-units/tick (pre-ROLLOVER) | [PH] | missileeffect.h:61 | yes (retail: `mbr_0x188 = 0x100000 = 16*ROLLOVER` at TPhotonEffect Init, cls_0x5b3c18::virt_meth_0x510400:44) |
| LIGHTNING_SOUND | `"lightning"` | sound-mount key | [PH] | effect.h:473; missileeffect.cpp:156 | yes (retail: `"lightning"` string at 005e1a60 / 005e1a6c, XREF'd from Photon Init + Pulse, _data.txt:108674,108680) |
| firsttime (init) | true | bool | [PH] | missileeffect.cpp:158 | yes (retail: `mbr_0x190 = 1` at Init, cls_0x5b3c18::virt_meth_0x510400:46) |
| NUM_PHOTON_SPARKS | 16 | count (per side — total slots = ×2) | [PH] | missileeffect.h:84 | snapshot-only |
| total spark slots | 32 (`NUM_PHOTON_SPARKS * 2`) | per-slot arrays `p[32]`, `v[32]`, `scale[32]`, `framenum[32]` | [PH] | missileeffect.h:93-96 | snapshot-only |
| PHOTON_LAUNCH_RADIUS | 20 | wu — LAUNCH sparks born in ±20 cube around effect pos (local) | [PH] | missileeffect.h:85 | snapshot-only |
| PHOTON_LAUNCH_DURATION | 25 | ticks — half-mark and convergence timing | [PH] | missileeffect.h:86 | snapshot-only |
| PHOTON_LAUNCH_SCALE_STEP | 0.25 | scale/tick (LAUNCH ramp; EXPLODE uses 0.125 = STEP/2) | [PH] | missileeffect.h:87 | snapshot-only |
| central flare scale (LAUNCH init) | 1.0 | scale | [PH] | missileeffect.cpp:202 | snapshot-only |
| central flare max scale | 5.5 | scale cap (after `framenum[0] > DURATION/2 = 12`) | [PH] | missileeffect.cpp:258 | snapshot-only |
| satellite spark p[n] init | `random(-20, +20)` per axis | wu (per-axis cube around effect pos) | [PH] | missileeffect.cpp:209-211 | snapshot-only |
| satellite spark v[n] (n > 16/3 ≈ 5) | `-p[n] / (DURATION*3/4) = -p[n]/18` per axis | wu/tick (converge to origin) | [PH] | missileeffect.cpp:214-219 | snapshot-only |
| stationary sparks (n ≤ 5) | v = 0 (just pulse in place) | wu/tick | [PH] | missileeffect.cpp:220-221 | snapshot-only |
| satellite spark scale init | 0.0 | scale | [PH] | missileeffect.cpp:223 | snapshot-only |
| satellite spark framenum init | `random(-DURATION/2, 0) = random(-12, 0)` | ticks (negative = start-delay) | [PH] | missileeffect.cpp:225 | snapshot-only |
| trailing photon framenum init | `-1` (slots 16..31) | ticks | [PH] | missileeffect.cpp:228-230 | snapshot-only |
| activesparks (LAUNCH init) | `NUM_PHOTON_SPARKS - 1 = 15` | count | [PH] | missileeffect.cpp:233 | snapshot-only |
| LAUNCH ramp threshold | `framenum[n] > DURATION/2 = 12` | ticks (half-life → scale-down) | [PH] | missileeffect.cpp:258, 274 | snapshot-only |
| activesparks → status edge | `activesparks < NUM_PHOTON_SPARKS/3 ≈ 5` → `SetStatus(true)` | count | [PH] | missileeffect.cpp:285-286 | snapshot-only |
| FLY-entry lead-photon scale | 3.5 | scale | [PH] | missileeffect.cpp:309 | snapshot-only |
| FLY-entry trailing-photon framenum | `framenum[16+t] = -t` (t=1..16) | ticks (staggered start-delay 1..16) | [PH] | missileeffect.cpp:315 | snapshot-only |
| FLY-entry leading-spark framenum clear | `framenum[t] = -1` (t=1..16) | ticks (deactivate launch-flourish slot t) | [PH] | missileeffect.cpp:316 | snapshot-only |
| FLY-entry trailing-photon pos | `p[16+t] = (-dir.xyz * t / ROLLOVER)` for t=1..16 | wu (back along velocity, in local effect space) | [PH] | missileeffect.cpp:317-319 | snapshot-only |
| FLY-entry trailing-photon scale | `3.5 - t * PHOTON_LAUNCH_SCALE_STEP = 3.5 - t*0.25` (t=1..16) | scale (1.75..3.25 ramp, tail = smallest) | [PH] | missileeffect.cpp:320 | snapshot-only |
| FLY trailing framenum advance | `framenum[16+n]++` per tick | ticks | [PH] | missileeffect.cpp:326-327 | snapshot-only |
| EXPLODE-entry lead/sub-spark scale | 3.0 | scale | [PH] | missileeffect.cpp:344 | snapshot-only |
| EXPLODE-entry spark velocity | `random(-3, +3) * 3 / 2 = ±4.5` per axis (isotropic) | wu/tick | [PH] | missileeffect.cpp:346-348 | snapshot-only |
| EXPLODE-entry framenum reset | 0 (each of 16 lead slots) | ticks | [PH] | missileeffect.cpp:350 | snapshot-only |
| EXPLODE-entry activesparks | `NUM_PHOTON_SPARKS = 16` | count | [PH] | missileeffect.cpp:352 | snapshot-only |
| EXPLODE per-tick scale step | `-PHOTON_LAUNCH_SCALE_STEP/2 = -0.125` per slot | scale/tick | [PH] | missileeffect.cpp:369, 386 | snapshot-only |
| EXPLODE death gate | `activesparks == 0` → `SetCommandDone(true)` | count | [PH] | missileeffect.cpp:377-378 | snapshot-only |
| RefreshZBuffer texel size | `tex_u = 16`, `tex_v = 16` | px (base Z-restore patch; multiplied by spark scale) | [PH] | missileeffect.cpp:447-448 | snapshot-only |
| Render obj flags | `OBJ3D_SCL1 \| OBJ3D_POS2` (uniform scale + local pos) | flags | [PH] | missileeffect.cpp:424 | snapshot-only |
| numtexframes | `Get3DImagery()->GetTexture(0).numframes = 1` (LAUNCH state) | per-tick frame count | [PH] | missileeffect.cpp:194-195; photon.i3d state-1 cell shorts at 0x44 | yes (asset: I3D LAUNCH-state texture record numframes=1) |
| EXPLODE texture frame count | 8 | frames in I3D EXPLODE-state texture record | [PH] | photon.i3d state-3 cell shorts at 0xd4 (`0x0008`) | yes (asset) |
| frame cycling | `frame = framenum[n] % numtexframes` | per-spark texture frame | [PH] | missileeffect.cpp:429 | snapshot-only (mechanism); but with `numtexframes=1`, frame is always 0 — see §8 |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Photon mesh | `legacy/Imagery/Magic/Photon.I3D` (= retail `data/imagery.rvi:Imagery/Magic/photon.i3d`, **identical**) | 48,340 B (MD5 `a9e3e83ae8d1caae171da2823ef9a86f`) | the photon sparkle billboard quad + its texture(s) | registered `Class.Def:2018` under name `"Photon"` (ID `0x113803f4`); loaded by the OBJCLASS_EFFECT registry on spawn; sub-object addressed by `T3DAnimator::GetObject(0)` (`missileeffect.cpp:413`) |

**Asset identity:** byte-identical to shipped `data/imagery.rvi` (§2.1.1).

**I3D header (from the binary):** magic `CGSR`; **3 animation states**
`missilelaunch`, `missilefly`, `missileexplode` (matching `MISSILE_LAUNCH/FLY/EXPLODE = 0/1/2`,
`missileeffect.h:20-24`); **1 sub-object** named `"photon"` (string at file offset
`0x85c`; "objects = 1" field at header offset `0x14`). The animator addresses this
single sub-object by index:

| sub-object index | I3D name | role in animator | cite |
|------------------|----------|------------------|------|
| `GetObject(0)` | `photon` | the **photon sparkle quad** — drawn 32× per frame at the 32 per-slot scales/positions (LAUNCH sparks, FLY lead+trail, EXPLODE outburst). One quad, many transforms. | missileeffect.cpp:413, 431 |

### 4.1 Texture atlas / UV decode

**This is a single-cell sprite, NOT a multi-cell atlas.** Per-state texture cell
records in the I3D (each state has its own 6-short record at offsets 0x44, 0x88,
0xd4 — the per-state texture-region descriptor):

| state | I3D record offset | flags | num_frames | src_w × src_h | dst_w × dst_h | meaning |
|-------|-------------------|-------|------------|---------------|---------------|---------|
| `missilelaunch` (0) | 0x44 | 0x2000 | **1** | 64 × 80 | 32 × 80 | one 32×80 sub-rect within a 64×80 page |
| `missilefly` (1) | 0x88 | 0x2000 | **1** | 64 × 80 | 32 × 80 | identical to LAUNCH |
| `missileexplode` (2) | 0xd4 | 0x2000 | **8** | 64 × 80 | 32 × 80 | 8-frame flipbook of the same cell size |

Reading the shorts at each per-state texture cell (little-endian):
- state 1 (`0x44`): `00 20 01 00 40 00 50 00 20 00 50 00` → flags=0x2000, **numframes=1**, srcW=64, srcH=80, dstW=32, dstH=80.
- state 2 (`0x88`): `00 20 01 00 40 00 50 00 20 00 50 00` → **numframes=1** (identical to LAUNCH).
- state 3 (`0xd4`): `00 20 08 00 40 00 50 00 20 00 50 00` → **numframes=8**, otherwise identical layout.

**UV mapping:** the `photon` sub-object's quad vertices (parsed at file offset
`0x1be4` onward — 4 corner vertices) use the default 0..1 UV mapping over each
selected cell (the I3D engine binds the active state's texture cell as the source
rect; the quad UVs are 0..1 across that cell). There is no per-corner UV override
in the snapshot — the animator only sets `OBJ3D_SCL1 | OBJ3D_POS2` (`missileeffect.cpp:424`),
NOT `OBJ3D_VERTS` (the sister fireball animator uses `OBJ3D_VERTS` for its 4×4 atlas,
`missileeffect.cpp:766`, but the photon does NOT). So **the photon draw uses the
authored full-cell UVs (0..1 over the active-state's texture cell)**, and the
animator's `frame = framenum % numtexframes` (`:429`) advances the **engine's
SetAnimFrame index** within the active state's flipbook.

**Effective flipbook by state:**
- LAUNCH state texture: 1 frame → frame=0 always → static single-cell sprite.
- FLY state texture: 1 frame → frame=0 always → static single-cell sprite.
- EXPLODE state texture: 8 frames → frame cycles 0..7 → 8-cell flipbook.

**HOWEVER**, the animator caches `numtexframes` once at `Initialize` (state=LAUNCH at that point) (`missileeffect.cpp:194-195`):
```
Get3DImagery()->GetTexture(0, &tex);   // queries state-0 texture
numtexframes = tex.numframes;          // = 1
```
…and `frame = framenum[n] % numtexframes` (`:429`) uses this **cached value of 1**.
So **the EXPLODE state's 8 texture frames are never actually animated by the spark
loop** — `frame = framenum % 1 = 0` for every slot for every state. The 8-frame
EXPLODE texture record in the I3D is authored but the snapshot animator never
cycles it (`frame` is stuck at 0 because `numtexframes` was cached at LAUNCH).
**Snapshot-quirk / dead-feature** — possibly the developer intended to re-query
`numtexframes` per state and never did. See §13.3.

**Texture pixel data (RGB565):** the file ends in long runs of `0x017d` words —
RGB565 `0x017d` = R:0, G:0x0b, B:0x1d = a very dim pale-blue/lavender background
with bright pixels (e.g. `0x117d` words = brighter) clustered to form the spark
silhouette. This is the **bright-on-near-black** photon sparkle texture (the
chroma-key/black background is the alpha-transparent area, the bright pixels are
the sparkle). See §7 BLEND SANITY-CHECK and §10.

**No external texture files** — pixels are embedded in the `.I3D`.

The effect loads a real asset — do **NOT** substitute a procedural spark sprite.
The photon sparkle texture is the visual identity ([feedback-no-standins]).

---

## 5. Spawn & emit

### 5.1 [BASE] How a missile enters the world & launches

See [F07 §5.1 [BASE]](F07_TFireBallEffect.md) for the full description. In brief:
- One-shot spawn → 3-state machine (LAUNCH → FLY → EXPLODE → die).
- Caller `TSpell::Timer` (or equivalent) builds the effect at the caster's hand/source pos with the variant `HEIGHT`, facing the caster's aim ([F07 §5.1]).
- LAUNCH→FLY transition is the **animator-driven handshake**: the animator's launch-flourish finishes (activesparks < NUM_PHOTON_SPARKS/3, `missileeffect.cpp:285`), calls `((PTMissileEffect)inst)->SetStatus(true)` (`:286`); the base `Pulse` sees `status`, converts `GetAngle()` → `vel`, sets `OF_MOVING|OF_WEIGHTLESS`, applies `vel.z = speed/-16`, computes flight `range = (240*MISSILE_RANGE) / (speed/ROLLOVER) = 480/16 = 30 ticks` for the photon at `PHOTON_SPEED=16`, switches to FLY.
- Photon overrides `Pulse` to chain base + play `LIGHTNING_SOUND` on FLY-entry edge (the `firsttime` flag, `:165-169`).

### 5.2 [PH] Coordinate space & origin

- The effect *object* moves in **world space** (the engine mover integrates `vel`).
- The 32 spark `p[n]` positions are in **local space relative to the effect's world pos**: at Render the animator writes `obj->pos = p[n]` (`missileeffect.cpp:426`) — the I3D pipeline adds the effect's world transform on top. At `RefreshZBuffer` (`:461-468`) the animator manually adds `effect.x/y/z` to `p[n]` for the world-projection step, confirming the per-spark local convention.
- The flare/lead photon `p[0] = (0,0,0)` is the effect's origin point itself (`missileeffect.cpp:201, 308, 343`).

### 5.3 [PH] LAUNCH-state spark emit (the spell-charge flourish)

At `Initialize` the animator seeds 16 slots (slots 0..15) in **local space around the effect origin**:

```
p[0] = (0,0,0); scale[0] = 1.0; framenum[0] = 0      // central flare

for n in 1..15:
    p[n] = ( random(-20, +20), random(-20, +20), random(-20, +20) )    // ±20 wu cube, 3-axis isotropic
    if n > NUM_PHOTON_SPARKS/3 (= 5):
        v[n] = -p[n] / 18   // 3*DURATION/4 = 3*25/4 = 18.75 ≈ 18, per-axis convergence
    else:
        v[n] = (0,0,0)      // stationary sparks (just pulse in place)
    scale[n] = 0.0
    framenum[n] = random(-12, 0)    // -DURATION/2..0 = staggered start delays

for n in 16..31:                    // trailing photons inactive during LAUNCH
    framenum[n] = -1

activesparks = NUM_PHOTON_SPARKS - 1 = 15
```

So during LAUNCH the visual is a **±20-wu cube of 16 sparks around the caster**,
each starting after a random 0..12-tick delay, with 10 of them (n=6..15) flying
toward the central flare and 5 (n=1..5) pulsing in place. The central flare is
the bright always-active anchor.

- **Trigger semantics:** one-shot (the caster spawns the effect once); LAUNCH-state continuous spark animation; bounded by `activesparks < 5` → handshake to FLY.
- **Count per trigger:** 16 LAUNCH sparks + (later) 16 trailing photons + (later) 16 EXPLODE sparks = up to 32 slots active.
- **Initial direction / distribution:** LAUNCH sparks ±20 cube + per-axis convergence; FLY trailing photons along `-velocity * t/ROLLOVER`; EXPLODE sparks isotropic ±4.5 wu/tick.
- **Emit anchor convention:** the effect's world pos (caster→aim source) for the central flare; all 32 slots in local space relative to it (§5.2).

### 5.4 Spatial diagram

```
              ┌────────────────────  LAUNCH (state 0)  ───────────────────┐
              │                                                            │
   wz (up)    │     · 4    · 5     · 11             ScreenAligned-ish      │
   │          │  ·          .       ·  ←  sparks    sparkle quad (the      │
   │          │  10  · 2  ✦ flare ·       (random   "photon" sub-object,   │
   │          │       ·  (p[0])  · 14    cube ±20)  drawn once per slot)   │
   │          │  · 6  · 3   ·  · 13                                        │
   │          │     ·     · 8                                              │
   ├─────────►│  per-spark convergence v[n] = -p[n]/18 (n>5);  v[n]=0 (n≤5)│
   wx         │  framenum[n] = random(-12,0)..+...  →  scale 0→0.5→1.0 etc │
              │  central flare scale 1.0 → +0.25/tick after framenum>12,   │
              │  capped at 5.5                                              │
              │  HANDSHAKE: activesparks<5 → SetStatus(true) → base launches│
              └────────────────────────────────────────────────────────────┘

              ┌────────────────────────  FLY (state 1)  ─────────────────────────┐
              │   wz                                                              │
              │   │     leading photon p[0]=(0,0,0)  scale 3.5                    │
              │   │                ✦                                              │
              │   │     trailing: p[16+t] = -dir * t/ROLLOVER (local, -velocity)  │
              │   │            ·                                                  │
              │   │       ·  scale[16+t] = 3.5 - t*0.25  (1.75..3.25; tail = smallest)
              │   │  ·  framenum[16+t]=-t (staggered start delays 1..16 ticks)    │
              │   └───────────────────────────────► wx   effect travels along aim │
              │  ╱   PLAY("lightning") on FLY-entry edge (firsttime once)          │
              │ wy                                                                │
              └───────────────────────────────────────────────────────────────────┘

              ┌──────────────────────  EXPLODE (state 2)  ──────────────────────┐
              │   wz                                                             │
              │   │     · · ←  16 outburst sparks (slots 0..15)                  │
              │   │  ·       · scale 3.0, vel = ±4.5 wu/tick isotropic           │
              │   │     ·  ·   (random(-3,+3)*3/2 per axis)                      │
              │   │  · ✦ ·     -scale/0.125 per tick → cull at 0                 │
              │   │     ·  ·   activesparks-- ; ==0 → SetCommandDone             │
              │   │  ·       · trailing photons continue draining                │
              │   └───────────────────────────────────────► wx                   │
              │  ╱  emit anchor = effect impact pos (where missile stopped)      │
              │ wy                                                               │
              └─────────────────────────────────────────────────────────────────┘
```

---

## 6. Behavior & per-frame logic

### 6.1 [BASE] `TMissileEffect::Pulse` — the shared travel/impact machine

See [F07 §6.1 [BASE]](F07_TFireBallEffect.md) for the full annotated body
(`missileeffect.cpp:52-135`). For the photon:
- `PHOTON_SPEED=16` → `speed = 16*ROLLOVER = 0x100000` after `SetSpeed(16)`.
- range-at-FLY = `(240*MISSILE_RANGE=2) / (PHOTON_SPEED=16) = 480/16 = 30 ticks` of flight before auto-detonate.
- char-hit gate at 32 wu (any non-invoker, non-dead, enemy character within 32).
- `MOVE_BLOCKED` → EXPLODE.
- EXPLODE → `KillThisEffect` when `!HasAnimator()`.

### 6.2 [PH] `TPhotonEffect::Initialize` + `Pulse`
(`missileeffect.cpp:150-170`)

```
Initialize:
    TMissileEffect::Initialize()        // state=LAUNCH, range=32768, speed=16*ROLLOVER, status=false
    SetSpeed(PHOTON_SPEED=16)            // redundant: re-asserts speed=16*ROLLOVER (no-op vs base default)
    SoundPlayer.Mount(LIGHTNING_SOUND)   // pre-load the "lightning" sample
    firsttime = true                     // play it once on FLY entry

Pulse:                                   // each sim tick
    TMissileEffect::Pulse()              // [BASE] travel + impact handling
    if state == MISSILE_FLY and firsttime:
        firsttime = false
        SoundPlayer.Play(LIGHTNING_SOUND)    // the bolt-launch zap

destructor:
    SoundPlayer.Unmount(LIGHTNING_SOUND)     // (missileeffect.h:73)
```

### 6.3 [PH] `TPhotonAnimator::Animate` — per-frame 32-slot choreography
(`missileeffect.cpp:243-399`)

```
on each render frame (Animate(draw)):
    T3DAnimator::Animate(draw)
    inst->SetStatus(false)             // clear launch handshake each tick
    inst->SetCommandDone(true)

    switch state:

    case MISSILE_LAUNCH (0):           // §5.3 spell-charge flourish
        framenum[0]++                  // central flare frame counter
        if framenum[0] > DURATION/2 (=12) and scale[0] < 5.5:
            scale[0] += PHOTON_LAUNCH_SCALE_STEP (=0.25)    // flare grows after half-mark
        for n in 1..15:
            framenum[n]++              // satellite spark frame counter
            if framenum[n] >= 0:        // start-delay expired
                p[n] += v[n]            // move toward origin (or stand still)
                if framenum[n] > 12:    // past half-mark → SHRINK
                    if scale[n] != 0:
                        scale[n] -= 0.25
                        if scale[n] <= 0:           // spark died
                            activesparks--
                            if activesparks < NUM_PHOTON_SPARKS/3 (=5):
                                inst->SetStatus(true)   // HANDSHAKE → base launches
                            scale[n] = 0
                else:                  // first half → GROW
                    scale[n] += 0.25

    case MISSILE_FLY (1):              // §5.4 flying bolt + trail
        if state != oldstate:           // FLY-ENTRY edge — fires once
            S3DPoint dir; inst->GetVel(dir)   // sub-unit velocity (×ROLLOVER)
            // lead photon
            p[0] = (0,0,0); scale[0] = 3.5; framenum[0] = 0
            // trailing photons: back along velocity, staggered start
            for n=16..31, t=1..16:
                framenum[n] = -t            // delay 1..16 ticks
                framenum[t] = -1            // kill the LAUNCH-side leading-spark slot t
                p[n].xyz = (-dir.xyz * t) / ROLLOVER    // 1 wu back per trail-step
                scale[n] = 3.5 - t*0.25     // 3.25..1.75 (tail smallest)
        else:                           // continuing FLY ticks
            for n=16..31: framenum[n]++ // trailing photons march forward in time

    case MISSILE_EXPLODE (2):           // §5.5 impact outburst
        if state != oldstate:           // EXPLODE-ENTRY edge — fires once
            S3DPoint dir; inst->GetVel(dir)   // (unused — sparks isotropic)
            for n in 0..15:
                p[n] = (0,0,0); scale[n] = 3.0
                v[n].xyz = random(-3,+3) * 3.0 / 2.0    // ±4.5 wu/tick isotropic
                framenum[n] = 0
            activesparks = NUM_PHOTON_SPARKS = 16
        else:                           // continuing EXPLODE ticks
            for n in 0..15, t=16..31:    // lead + corresponding trailer
                framenum[n]++
                p[n] += v[n]            // straight-line spark
                if scale[n] > 0:
                    scale[n] -= 0.125    // STEP/2 (slower fade than LAUNCH)
                    if scale[n] <= 0:
                        scale[n] = 0
                        activesparks--
                        if activesparks == 0:
                            inst->SetCommandDone(true)  // ALL done → reap
                if scale[t]:           // trailing photons keep draining
                    framenum[t]++
                    scale[t] -= 0.125
                    if scale[t] <= 0: scale[t] = 0

    oldstate = state                    // edge-tracking for next tick
```

**Death gate.** EXPLODE drains: `activesparks` decrements each time a spark's
scale falls to 0; when it hits 0, `SetCommandDone(true)` (`:378`). The base
`TMissileEffect::Pulse`'s EXPLODE case (`missileeffect.cpp:128`) calls
`KillThisEffect()` when `!HasAnimator()` — but `HasAnimator()` checks `commanddone`,
so the chain is: animator drains → `SetCommandDone(true)` → base sees done → kill.

**Note — `inst->SetCommandDone(true)` on EVERY tick.** Line `:250` sets command-done
true at the top of Animate every tick (looks like a leftover/bug); the EXPLODE-end
gate (`:378`) does it again only after activesparks==0. The unconditional set every
tick is suspicious; it may be why the base's `!HasAnimator()` death check needs the
animator to also clear it via the activesparks gate — i.e. the death is gated by
activesparks==0, with the unconditional set being a no-op in practice. See §13.4.

### 6.4 Helper sub-systems

**None.** Unlike the fireball (which uses `TSubParticleAnimator` for sparks and
`TShockAnimator` for the impact ring), the photon's 32 slots are managed
**directly** inside `TPhotonAnimator` — no embedded helper systems, no spawned
child effects.

### 6.5 Temporal diagram

```
state:   LAUNCH (variable)           │ FLY (≤30 ticks)              │ EXPLODE (until activesparks=0) │ dead
         16 sparks: 5 stationary,    │ leading photon scale 3.5     │ 16 sparks ±4.5 wu/tick         │
         10 converging to origin;    │ 16 trailing photons back     │ shrink 3.0→0 @ -0.125/tick     │
         central flare grows 1→5.5;  │ along -velocity, scale 3.25→ │ trailing photons drain in     │
         ends when activesparks<5    │ 1.75 (tail smallest); PLAY   │ parallel; activesparks==0 →    │
         → SetStatus → base launches │ ("lightning") on entry-edge  │ SetCommandDone → reap          │

central flare:
5.5 ┤         ┌──────────►              (3.5)         (3.0 → 0)
    │        ╱
    │       ╱        ───────────────►●  ─────●─────────●
1.0 ┤●─────╯                                  │        │
    │  (LAUNCH grow                            ╲        ╲ (EXPLODE shrink)
0   ┤             after framenum>12)            ╲        ╲___
    └────────────────────────────────────────────────────────► ticks

per-satellite-spark scale envelope (slots 1..15) during LAUNCH:
0.25*step:    0 → scale ramp up → peak (DURATION/2=12) → scale ramp down → 0 (dead, activesparks--)
   shape:   ●─────●        ●
            (start delay)  (peak)         ●─── (dead)
                         framenum[n]: -12..0 ............ 12 .............. dies
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** the **single `photon` sub-object quad** (`Misc/Magic\Photon.I3D:GetObject(0)`), drawn **up to 32 times per frame** at the per-slot scale and position (`missileeffect.cpp:415-436`). Each call is `RenderObject(obj)` after writing `obj->pos = p[n]`, `obj->scl = (scale[n], scale[n], scale[n])`, `obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2`.
- **Blend mode (original — TRACED to the helper):** `SaveBlendState()` + **`SetBlendState()`** at `missileeffect.cpp:410-411`, then `RestoreBlendState()` at `:438`. `SetBlendState()` (`effect_old.cpp:221-233`) sets:
  - `TEXTUREMAPBLEND = D3DTBLEND_MODULATE`
  - `SRCBLEND = D3DBLEND_SRCALPHA`
  - `DESTBLEND = D3DBLEND_INVSRCALPHA`
  - `ZWRITEENABLE = false`
  - `ZENABLE = true`
  → **Alpha (modulated)**, NOMENCLATURE §3 / [knowledge/03 §1.1](knowledge/03_RENDERING_CONVENTIONS.md). Not the additive helper.
- **BLEND SANITY-CHECK (MANDATORY).** This is a **classic bright-on-near-black sparkle texture** (§4.1: RGB565 background `0x017d` — very dim blue/black, sparkle pixels brighter). Bright-on-black sprite + Alpha-blend with chroma-key-converted alpha = **SUSPECT additive**. Cross-checks:
  - **Sister-family blend:** the sibling `TFireBallAnimator` (F07) also calls `SetBlendState()` (Alpha) and the F07 forensics records it as Alpha — confirmed by tracing the helper. So the missile-family snapshot convention is consistently `SetBlendState` (Alpha).
  - **Sister fire-family spells** (FireFlash, FireWind, FireCone, DragonFire) use `SetAddBlendState` for their glows — a different family convention.
  - **The retail `*::Render` bodies are NOT decompiled** for this effect — the blend choice is **snapshot-only**, unverified against the shipped binary.
  - **Confirmed-case cross-reference:** the sparks animator (X22) is snapshot-`SetBlendState` (Alpha) but shipped Additive (video-confirmed). Combat sparks (CA_SPARKS) are also confirmed Alpha. So neither precedent fully wins for the photon.
  - **VERDICT:** **code = Alpha, but a bright energy-bolt sparkle on a black background is the textbook ADDITIVE sprite.** Flag as **snapshot-only Alpha; visually vet against in-game retail footage** — likely shipped Additive (electric/energy glows almost always do). See §13.2.
- **Lit vs self-lit:** **Unlit / self-lit.** No `DoLighting` call in `TPhotonAnimator::Render`; no per-vertex color overwrite. The sparkle texture supplies its own brightness (pale lavender/blue against the chroma-key background). Pure texture-driven color, no scene light sampling. (`missileeffect.cpp:408-441` — no `D3DRGB` / `GetClosestLights` / `GetAmbientLight` call.)
- **Depth / Z:** **TestNoWrite** — `ZWRITEENABLE=false`, `ZENABLE=true` set by `SetBlendState` (`effect_old.cpp:224-225`). `RefreshZBuffer` (`missileeffect.cpp:443-471`) repairs scene Z under each active spark by projecting `p[n] + effect_pos` to screen and calling `RestoreZ(screen.x - x_size/2, screen.y - y_size/2, x_size, y_size)` where `x_size = scale[n] * 16`, `y_size = scale[n] * 16`. So Z-restore footprint scales with per-spark scale.
- **Orientation / per-quad transform:** `obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2` (`missileeffect.cpp:424`). **No rotation flag set, no matrix flag** — the quad is drawn at its **authored I3D orientation with uniform scale + local position offset**. The `photon` sub-object's vertex layout (parsed at I3D file offset 0x1be4: 4 corner vertices each with embedded UVs) is a standard billboard quad, so this renders as a **ScreenAligned** sparkle in practice (the I3D pipeline's default for a quad with no rotation transform applied is camera-facing).
- **Per-vertex color packing:** **none** — the animator does NOT write `lverts[*].color` or call `DoLighting`. Color comes 100% from the texture × MODULATE-stage default white vertex color. (`OBJ3D_VERTS` flag is NOT set, only `OBJ3D_SCL1 | OBJ3D_POS2`.)
- **Per-quad UV (atlas):** the active-state's authored 0..1 UVs over its texture-region (§4.1). No `SetAnimFrame` / `tu/tv` rewrite in the animator. UV-cell selection is by **engine state** (the I3D's per-state texture record), driven by `inst->state` (LAUNCH/FLY/EXPLODE).
- **Draw order in `Render`:** one bracket `SaveBlendState/SetBlendState/.../RestoreBlendState`, then a single linear scan of slots 0..31. Slots with `framenum[n] < 0` are SKIPPED (start-delay not expired yet, `:418`). Per-slot it: resets extents → sets `obj->flags`, `obj->scl`, `obj->pos` → sets `frame = framenum[n] % numtexframes` → `RenderObject(obj)` → `UpdateExtents()`. Active slots vary by state (LAUNCH: 0+1..15; FLY: 0+16..31; EXPLODE: 0..15 + 16..31 trailing draining).

---

## 8. Texture animation

**Mechanism: engine-state-driven texture-region selection + cached `numtexframes=1` flipbook index.** Per §4.1 the photon I3D has 3 per-state texture records (LAUNCH/FLY have numframes=1, EXPLODE has numframes=8). The animator's `Initialize` queries `numtexframes` once at state=LAUNCH (`missileeffect.cpp:194-195`), caching **`numtexframes=1`**. The per-spark loop computes `frame = framenum[n] % numtexframes` (`:429`), which with `numtexframes=1` is **always 0** — i.e. `SetAnimFrame(0, …)` for every spark every tick.

Net effect:
- **LAUNCH / FLY** — single-frame static sprite (numtexframes matches the authored 1-frame I3D record); the texture is the same throughout the state.
- **EXPLODE** — even though the I3D's EXPLODE-state record has 8 frames authored, the cached `numtexframes=1` means **frame=0 always** → only the first of the 8 EXPLODE frames is used. The other 7 frames in the I3D are authored but never displayed.

No UV-scroll, no `SetTextureFrame` handle-swap (`framehtexs[]`) — UVs are static at the per-state texture cell. Per-spark per-instance phase = none beyond the latent `framenum[n]` differences (which `% 1 = 0` collapses).

Distinguish from the sister `TFireBallAnimator` (F07) which uses a real 4×4 UV-atlas flipbook via `SetAnimFrame` writing 4 corner UVs explicitly — the photon does **not** do this.

See §13.3 — the cached-`numtexframes=1` is plausibly a snapshot bug (intended to re-query per state).

---

## 9. Associated light

**N/A — none emitted.** `TPhotonAnimator` makes no `AddPointLight`/dynamic-light call;
the photon does not light the scene. (Contrast the fireball which carries a
`spell.def`-data-driven `LIGHT COLOR 255,130,0 INT 255 MULT 20` — the photon has
no such spell-data entry because it has no `spell.def` entry at all, §12.)

The `SoundPlayer.Mount(LIGHTNING_SOUND)` at Initialize is **audio**, not light —
the name `"lightning"` is the AIL sound sample key, not a light color (§11).

If the reconstruction wants the photon to feel right against the scene, this is a
**deliberate omission** in the original (probably because it was a fast missile
and didn't get a light pass); record as "none" not as a missing/forgotten light.

---

## 10. Color

- **Source:** the **authored `photon` sub-object's texture** within the I3D, in its
  per-state region (§4.1). The pixel data is RGB565 (16-bit) with the chroma-key
  background `0x017d` (R≈0, G≈11/63, B≈29/31, i.e. a very dim deep-blue/lavender)
  and bright sparkle pixels (e.g. `0x117d` = R≈2/31, G≈11/63, B≈29/31; brighter
  values toward white-blue). The black-keyed background becomes transparent under
  the Alpha/MODULATE blend (§7).
- **Per-state palette:** all three states share the same `photon` quad UV mapping
  (0..1 over each state's 32×80 cell) and use the same kind of pale-blue/lavender
  sparkle hue (LAUNCH/FLY/EXPLODE textures are all variants of "energy sparkle").
- **Exact values:** the dominant pixel value `0x017d` decodes to RGB = (0×255/31,
  11×255/63, 29×255/31) ≈ **(0, 44, 239)** — bright pure-blue. Brighter pixels
  `0x117d` ≈ **(16, 44, 239)**; the texture skews **electric blue / pale lavender**
  overall — the canonical "magic energy" hue.
- **Per-vertex color:** the animator does NOT repaint vertex color (`OBJ3D_VERTS`
  flag is not set). Color is texture × MODULATE × default white vertex color.
- **Expected visual:** **bright electric-blue / pale-lavender sparkle bolt** — a
  shimmering blue-white photon. Pale/grey/red at reconstruction = port broken
  (stand-in texture, wrong blend, missed chroma-key, or wrong I3D loaded — there's
  a `redphoton.i3d` in the archive that is NOT the live registration). See
  [feedback-vfx-color-health-signal].
- **Normalization:** **none** — the texture directly carries the hue; no
  `NormalizeColors`, no hilt-brightness boost, no tint math.

---

## 11. Audio coupling

- **[PH] Mount:** `SoundPlayer.Mount(LIGHTNING_SOUND)` in `TPhotonEffect::Initialize` (`missileeffect.cpp:156`). `LIGHTNING_SOUND = "lightning"` (`effect.h:473`).
- **[PH] Play:** `SoundPlayer.Play(LIGHTNING_SOUND)` in `TPhotonEffect::Pulse` on the LAUNCH→FLY edge (`firsttime` flag) (`missileeffect.cpp:165-169`). Fires **once per missile**, at the moment the bolt launches.
- **[PH] Unmount:** `SoundPlayer.Unmount(LIGHTNING_SOUND)` in the `~TPhotonEffect` destructor (`missileeffect.h:73`).
- **Retail corroboration:** `"lightning"` string at `005e1a60` (XREF'd from `virt_meth_0x510400` = Photon Initialize, `_data.txt:108674`) and `005e1a6c` (XREF'd from `virt_meth_0x510450` = Photon Pulse, `_data.txt:108680`). The retail binary mounts on Init and plays on Pulse — **mechanism CONFIRMED**.
- **Family share:** the `"lightning"` sound key is shared by `TPhotonEffect`, `TFlameDiscEffect` (`missileeffect.cpp:1393, 1421`), and a couple of strip/lightning effects (`_data.txt:108709, 108716` show 4 distinct copies of the string at .rdata, suggesting multiple-class mount calls). All three families use the same sample on launch.
- **The I3D may carry per-(state,frame) tag-driven sounds** ([knowledge/02 §8](knowledge/02_ASSETS_IMAGERY.md)) — not enumerated here.

---

## 12. Triggers & in-game appearance

- **Spawned by:** `DEFINE_BUILDER("Photon", TPhotonEffect)` (`missileeffect.cpp:147`) + `REGISTER_3DANIMATOR("Photon", TPhotonAnimator)` (`:176`). The effect is **registered and reachable through the OBJCLASS_EFFECT/FindObjType path**, but…
- **NO LIVE CALLER FOUND in shipped data.** Exhaustive `grep -i "photon"` across `legacy/spell.def`, `legacy/rules.def`, `legacy/cbspell.def`, `legacy/cbrules.def`, every `*.def` and every `data/Resources/*.def` finds **no** `EFFECT "Photon"` / `SPELL ... "photon"` / `MAGICATTACK ... "photon"` / `INVOKE ... "photon"` reference. The retail binary's only XREFs of the `"Photon"` string at `005e1b20` are the **registry slots themselves** (the DEFINE_BUILDER / REGISTER_3DANIMATOR ones at `005103e0` / `005104a0`), not a caller.
- **Compare:** F07 fireball has `VARIANT "Fireball", ... "fireball"` in `spell.def:848,850` and is mapped to the priest/talisman/monk `invoke3` MAGICATTACK (`rules.def:1548`). The photon has **no equivalent** spell.def variant or rules.def MAGICATTACK entry.
- **Best interpretation: vestigial in shipped data.** The class is wired, the asset is shipped (byte-identical, §2.1.1), and the audio handshake is in retail — but no caller was preserved in the data files we have. Possibilities:
  - The Photon may have been an **early/abandoned prototype** energy-bolt spell that was kept compiling but removed from the spell list before ship.
  - It may be **driven by a now-stripped scripted scene** (intro/cutscene/test level) that doesn't survive in the current Modules directory.
  - It may be **callable from a dev/cheat path** that isn't in the shipped def files.
- **Where to see it in the original game:** **unknown from the data alone.** If reconstruction wants to test it, the harness approach (`--test=vfx --vfx=TPhotonEffect` mirror of how X21/X22 test) is the only available path — there's no in-game scene that fires it. Verify against in-game retail footage only if such footage exists (it likely doesn't in normal play); otherwise the harness is the **only** visual ground-truth source.
- **Vestigial?** **Yes, by the strict definition** (registered, asset shipped, no live caller in data). The reconstruction agent should treat the asset + animator as the visual identity and not block reconstruction on finding a caller. Ground truth then comes from: asset inspection + sister-family blend convention + the visible texture color.

---

## 13. Gaps & uncertainties

1. **Recon class labels: the yaml has them swapped vs. the structural evidence.** The candidate yaml labels `cls_0x5b401c` as TPhotonEffect (size 1288), but reading the bodies: `cls_0x5b401c`'s methods are the **TMissileEffect base** (the `virt_meth_0x510220` Pulse has the exact base immediates — range countdown, char-hit gate at 32, `vel.z=-speed/16`, OF_MOVING/OF_WEIGHTLESS flags) and `cls_0x5b3c18` (404 bytes, vftable 128 slots) is the **TPhotonEffect subclass** (Initialize mounts a sound, Pulse plays on FLY-entry). The yaml's "MEDIUM" rating with the open question reads as exactly this kind of preliminary attribution — recon needs a rerun to assign `cls_0x5b401c → TMissileEffect` and `cls_0x5b3c18 → TPhotonEffect`. The forensics here records the **functional** mapping from the body bodies regardless of the yaml label. (No effect on reconstruction; just a recon-mapping carve-out.)

2. **Snapshot-Alpha blend on a bright-on-black energy-bolt sprite — flagged SUSPECT (likely shipped Additive).** §7 traces `SetBlendState` (Alpha) in the snapshot, but the sprite is a textbook additive-glow asset (bright sparkle on chroma-key/black background) and the retail `Render` body is **not decompiled**. The sister X22 sparks case is precedent: snapshot-Alpha but shipped-Additive (video-confirmed). The reconstruction MUST visually vet the photon's blend against retail footage if any exists; default to Alpha per the snapshot but be prepared to switch to AdditiveStraight if the shipped render is brighter/glowier than the alpha-pass produces.

3. **EXPLODE-state 8-frame flipbook is authored but never displayed (`numtexframes` cached at LAUNCH=1).** The I3D has 8 frames in the EXPLODE texture record (§4.1), but `TPhotonAnimator::Initialize` queries `GetTexture(0)` at construction time (state=LAUNCH, numframes=1) and caches `numtexframes=1` (`missileeffect.cpp:194-195`). The per-spark `frame = framenum % 1 = 0` (`:429`) collapses to "frame 0 always". So the EXPLODE-state animation in the I3D is **dead data in the snapshot**. Possibilities: (a) a snapshot bug — the developer intended to re-query per state, never did; (b) deliberate — the EXPLODE 8-frame sequence is an authored alternative the dev didn't wire. Reconstruction should either re-query per state (faithful to the asset intent) OR follow the snapshot literally (cache LAUNCH-state numtexframes once). **Decision is a reconstruction call; flag for visual vet.** If retail visually animates an 8-frame EXPLODE sequence, the snapshot is buggy; if it doesn't, the snapshot is faithful.

4. **`inst->SetCommandDone(true)` is called unconditionally each tick at `Animate` entry (`:250`).** This is suspicious — the EXPLODE death gate only fires `SetCommandDone(true)` when `activesparks==0` (`:378`), but the early-tick set already did it. With the base `TMissileEffect::Pulse` EXPLODE case (`missileeffect.cpp:128`) calling `KillThisEffect()` when `!HasAnimator()` — and `HasAnimator()` typically checks the animator pointer, not commanddone — the death gate works via `activesparks==0` regardless of the unconditional set. The unconditional set is likely a leftover/no-op in practice. Reconstruction should follow the activesparks==0 gate for death; the unconditional set is functionally inert and can be preserved or dropped without visual impact.

5. **The animator-side §3 constants are all snapshot-only.** The 32-slot choreography (`PHOTON_LAUNCH_RADIUS=20`, `PHOTON_LAUNCH_DURATION=25`, `PHOTON_LAUNCH_SCALE_STEP=0.25`, central-flare max 5.5, FLY lead scale 3.5, FLY trail scale ramp 3.25→1.75, EXPLODE scale 3.0, EXPLODE velocity ±4.5, EXPLODE scale step 0.125, RefreshZBuffer texel 16×16) are not corroborated in the retail decomp — the animator class wasn't isolated. The structure (32-slot pattern, 16+16 split, the three states) is implied by the registration but the numbers must be visually vetted against ground truth (if any exists, §12).

6. **No live caller in shipped data — the effect may be vestigial.** §12 — `grep -i photon` finds no `EFFECT/SPELL/MAGICATTACK/INVOKE` reference anywhere in the shipped def files. The class is registered and the asset is shipped (byte-identical) and the LIGHTNING_SOUND handshake is in retail, but there's no surviving caller. Treat as vestigial-in-shipped-data; reconstruction targets the harness (`--test=vfx` style) as the only ground-truth surface, not an in-game scene.

7. **Coordinate convention for `S3DPoint dir` / `ROLLOVER` scaling at FLY entry.** `inst->GetVel(dir)` returns the integer-scaled velocity (×ROLLOVER, since `vel` is stored as fixed-point in the engine, `revdefs.h:447`). The trailing-photon back-positioning `p[16+t].xyz = -dir.xyz * t / ROLLOVER` (`:317-319`) converts back to float wu-per-tick × t-ticks-back-along-velocity. With `PHOTON_SPEED=16` and the launch `vel.z=-speed/16=-1` adjustment, the trailing-photon spacing is roughly `t * (sqrt(8²+8²+1²) ≈ 11) wu` per slot — i.e. trail spans up to **16*11 ≈ 175 wu**. Verify the spacing reads as a coherent bolt vs. an over-long streak when visualized.

8. **Effect 'photon' I3D sub-object has 4 raw vertices but the animator uses `OBJ3D_SCL1 | OBJ3D_POS2` (NOT `OBJ3D_MATRIX`/`OBJ3D_ROT*`)** — so the quad renders at its authored orientation with uniform scale + position only. Sister fireball uses `OBJ3D_MATRIX` for fixed -30°/+60° tilt; photon does NOT — it defaults to whatever the I3D pipeline does for an axis-aligned quad with no rotation transform applied (effectively ScreenAligned in practice). Verify the quad reads as camera-facing in render, not edge-on or tipped.

9. **Light wiring — none in spell.def either.** Even if a live caller is found, the photon has no `LIGHT COLOR` directive in any `*.def` that mentions it (since no def mentions it). The fireball's warm-orange spell light is data-driven; the photon has no equivalent. Documented as "none" not as "unknown."

---

## 14. Reconstruction burndown

```
[BASE — shared by FireBall / FireColumn / FlameDisc / IceStorm / YFireBall]
- [ ] TMissileEffect 3-state machine: LAUNCH→FLY→EXPLODE→die (§6.1, F07 §6.1)
- [ ] launch handshake: animator SetStatus(true) → base converts aim→vel,
      OF_MOVING|OF_WEIGHTLESS, vel.z = speed/-16, range=(240*2)/(speed/ROLLOVER) (§5.1, F07 §5.1)
- [ ] FLY impact detection: range countdown, MOVE_BLOCKED, char-within-32wu enemy check (§6.1)
- [ ] OffScreen override: keep simulating until exploding-and-done (§6.1, F07 §6.1)

[PHOTON]
- [ ] load Magic\Photon.I3D (byte-identical to retail); address GetObject(0) = "photon" quad (§4)
- [ ] set PHOTON_SPEED=16 (overrides base default 16 — same value, just re-asserted) (§3, §6.2)
- [ ] mount LIGHTNING_SOUND="lightning" at Initialize; play on LAUNCH→FLY edge (firsttime flag once);
      unmount at destruction (§6.2, §11)
- [ ] LAUNCH state spawn (16 slots, §5.3): p[0]=(0,0,0) scale 1.0 framenum 0; p[1..15] in random ±20-wu
      cube around origin; p[n>5] gets v[n] = -p[n]/18 (converges to origin in DURATION*3/4 ticks);
      p[n≤5] stationary v=0; scale[n]=0; framenum[n]=random(-12,0) start delay (§5.3)
- [ ] LAUNCH per-tick: framenum++ each slot; if framenum>=0 then p[n]+=v[n]; scale ramps
      +0.25/tick until framenum>12 then -0.25/tick to 0; central flare framenum[0]++ + scale grows
      +0.25/tick after framenum>12, capped at 5.5; on scale→0 activesparks--; when
      activesparks<5 → SetStatus(true) → handshake to base (§6.3)
- [ ] FLY state ENTRY (firsttime edge): set p[0]=(0,0,0) scale 3.5 framenum 0; for t=1..16:
      framenum[16+t]=-t (staggered delay), framenum[t]=-1 (kill LAUNCH-side slot t),
      p[16+t]=-dir.xyz*t/ROLLOVER (t-wu back along velocity vector, local space),
      scale[16+t] = 3.5 - 0.25*t (trail ramp 3.25..1.75; tail smallest) (§5.4, §6.3)
- [ ] FLY state continuing: trailing photon framenum[16+n]++ each tick (no position update —
      trail stays static in effect-local space, so as the effect moves, the trail trails) (§6.3)
- [ ] FLY entry plays LIGHTNING_SOUND once (the bolt-launch zap) (§6.2, §11)
- [ ] EXPLODE state ENTRY: 16 slots (0..15): pos=(0,0,0), scale=3.0,
      vel = (random(-3,+3)*1.5, random(-3,+3)*1.5, random(-3,+3)*1.5) (±4.5 isotropic), framenum=0;
      activesparks=16 (§5.5, §6.3)
- [ ] EXPLODE continuing: for n=0..15: framenum++, pos+=vel, scale-=0.125; on scale→0
      activesparks--, if ==0 → SetCommandDone(true) → effect reaped via base !HasAnimator path;
      trailing photons continue to scale-=0.125 in parallel (§6.3)
- [ ] core geometry: single I3D "photon" quad reused 32× per frame with per-slot scale + position
      (OBJ3D_SCL1 | OBJ3D_POS2 — uniform scale + local pos; NO matrix transform, NO rotation,
      NO vertex color repaint). Effectively ScreenAligned billboard in practice (§7)
- [ ] blend: SetBlendState = Alpha (MODULATE + SRCALPHA/INVSRCALPHA, ZWrite off, ZEnable on)
      ← SNAPSHOT-ONLY: this is a bright-on-black sparkle texture so AdditiveStraight is the
      probable shipped blend; visually vet against retail footage (§7, §13.2)
- [ ] lit-mode: Unlit/self-lit (no DoLighting, no per-vertex color overwrite) (§7, §10)
- [ ] depth: TestNoWrite; RefreshZBuffer restores scene Z per active spark over a
      (scale[n]*16) × (scale[n]*16) px patch around the projected world pos (§7)
- [ ] texture: static single-cell sprite per state (LAUNCH=1 frame, FLY=1 frame,
      EXPLODE=1 frame effectively because numtexframes is cached at LAUNCH=1 — the
      I3D's 8-frame EXPLODE record is authored but never animated by the snapshot animator;
      see §13.3 if visual matching shows it SHOULD animate) (§4.1, §8)
- [ ] color: pale electric-blue/lavender sparkle from the .I3D texture, modulated by default
      white vertex color; verify reads as bright magic-energy blue, NOT pale grey or red
      (don't load the redphoton.i3d variant by mistake) (§10)
- [ ] associated dynamic light EMITTED: NONE (the photon does not light the scene — explicitly
      no AddPointLight call, no spell.def LIGHT directive) (§9)
- [ ] death gate: EXPLODE activesparks==0 → SetCommandDone(true), base TMissileEffect::Pulse's
      EXPLODE case sees !HasAnimator → KillThisEffect (§6.3, §6.1)
- [ ] sub-effects spawned: NONE (no helper TSubParticleAnimator/TShockAnimator; the 32 slots are
      managed directly inside the animator) (§6.4)
- [ ] audio (audio phase): LIGHTNING_SOUND="lightning" — mount on Initialize, play once on
      LAUNCH→FLY edge, unmount on destructor (§11, retail-confirmed)
- [ ] OPEN: no live caller in shipped data — effect is vestigial; reconstruction's ground truth
      is the harness + the asset's intrinsic color/shape, NOT an in-game scene (§12)
- [ ] OPEN: visually vet snapshot-Alpha vs likely-shipped-Additive blend against any retail
      footage (§13.2) — if shipped Additive, swap SetBlendState → SetAddBlendState
- [ ] OPEN: visually vet whether EXPLODE's 8-frame I3D record should animate or be static
      (§13.3) — snapshot caches numtexframes=1 (static), but the asset has 8 frames authored
```

**Definition of done:** spawning the Photon at a caster's hand reproduces a
multi-phase electric-blue energy-bolt visual — a 1-second-ish "spell-charge"
shimmer of 16 sparks converging onto a bright central flare, followed by a
LIGHTNING_SOUND zap and a flying bolt with a 16-step trailing photon train back
along its velocity (3.25→1.75 scale ramp, tail smallest), ending on impact (wall,
enemy within 32 wu, or 30-tick range expiry) with a 16-spark outburst at ±4.5
wu/tick isotropic spread shrinking 3.0→0 at 0.125/tick — drawn as up to 32
instances per frame of the single `photon` I3D quad (no rotation transform, no
vertex color repaint), under `SetBlendState`-Alpha (or AdditiveStraight if visual
vet says so, §13.2), Unlit, TestNoWrite, with the per-active-spark Z-restore
footprint. The effect emits no scene light, spawns no sub-effects, and self-reaps
when the EXPLODE outburst's activesparks counter hits 0. Color reads as bright
pale-lavender/electric-blue, never red (avoid the dev `redphoton.i3d` variant).
