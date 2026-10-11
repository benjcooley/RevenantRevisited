# Forensics: the effects a blow triggers

**Topic:** what the shipped game draws when combat lands, misses or kills:
the blood (EffectBurst "blood" and the death-animation squirts), the combat
flash (EffectCombatFlash, the `combatflash` effect on `misc\Impact.i3d`),
the struck body's white-hot flash (TCharacter `+0x224`), the sparks of a
blocked blow, and the weapon swipe. Who bleeds, and in what colour.
**Status:** 2026-10-11. Blood, combat flash, struck-body flash and the
weapon swipe are ported (§2–§4, §6); sparks were already ported and are
unchanged (§5).
**Evidence:** the unchanged retail image through `rdis.py`/`fn.py`
(capstone over `recon/retail_asm/baseline/Revenant.rebuilt.exe`); the
retail TBloodAnimator droplet code and the retail TWeaponSwipe run in the
in-process emulator against the port (§2.6, §6.8); the DOSBox lab's recording of real retail combat,
`~/RevenantRetailLab/captures/gameflow/opening-20261006-run2.avi`
(the Rahul fight, 370–466 s: 30-odd blows with blood, nine-colour flashes
and struck bodies); the port in the combat arena
(`tools/combatarena`, `locke_vs_araknid`, one tick per frame) and in the
VFX gallery.

Callers are in [COMBAT_HIT.md](COMBAT_HIT.md) §3.7, §3.8, §3.12 and §4
(RNG). The VFX ledger rows are `Blood:0xddc4042e` and
`combatflash:0xad92bd29` in [../../vfx/EFFECT_BURNDOWN.md](../../vfx/EFFECT_BURNDOWN.md).

## 1. What was wrong

The user saw a spray of opaque black squares beside a struck Araknid that
lingered, and no colour. Two separate faults:

1. **Blood was a generic effect.** The port had no builder for the "Blood"
   type, so EffectBurst created a plain TEffect whose default animator drew
   the eight `Misc\Blood.I3D` boxes as static meshes: four black ARGB4444
   opacity masks and four red RGB565 boxes, opaque. EffectBurst never
   passed the spray parameters (the call was commented out: a generic
   TEffect has no SetParams). Taking blood out of the run (a temporary
   `return` in EffectBurst) removed the squares.
2. **Eight of nine combat flashes were unadmitted.** EffectCombatFlash
   picks one of nine Impact start states; the port admitted only state 0.
   For the other eight the partsys tags failed to parse (`numtrails=2:3`, a
   trailing `,`) or were refused ("start1 profile supports state0 only"), so
   the effect's meshes fell to the opaque mesh path: black-keyed RGB565 flare
   art drawn as small dark quads, and no particles at all.

Missing besides: retail draws the struck character a second time,
additively, for five ticks (§4); the port never did.

## 2. Blood

### 2.1 Spawning (TCharacter, `0x004c85d0` and Pulse `0x004c1bb0`)

EffectBurst `0x004c85d0`, the "blood" branch (`stricmp(name, "Blood")`):

```
if (burning(+0x1b8) && stricmp(name, "blood") == 0) return
def = {objclass 0x19 EFFECT, objtype FindObjType(name), uid DAT_00666970,
       pos = pos + (0, 0, height), facing byte 0}
inst = GetInstance(NewObject(&def)); if (!inst) return
count = random(1, 5)                                   // 0x004c866b, drawn after the object exists
inst->SetParams(height, facing + 0x80, 3, 5, 5, count) // vtable +0x200 = 0x004f9490
```

`facing + 0x80` is a byte promoted to int (128..383), not masked; the
animator wraps it. Height is 40 for an "impale" impact, else 50
(ResolveImpact / ResolveDead, COMBAT_HIT §3.7–3.8).

TBloodEffect (`0x19c` bytes, vtable `0x005acb0c`, builder `0x004f1870`)
stores the six parameters at `+0x184..+0x198` (SetParams `0x004f9490`,
GetParams `0x004f94d0`, same order). Its Initialize (`0x004f1890`) is empty
and Pulse jumps to TEffect::Pulse; OffScreen (slot `0x128`, `0x004f9400`)
kills it.

TCharacter::Pulse squirts blood from four death animations
(`0x004c1cc3..0x004c20d0`), each through NewObject + SetParams with the
facing read as a **signed** byte:

| Action (doing->Is) | Gate | Frames | Height | SetParams |
|---|---|---|---|---|
| `decapdeath` | bleeder, not CF_BADBLEEDER | 5 < f < 65, f % 5 == 0 | 75 | (75, face−91, 64, 91, 5, 1) |
| `combat to ripinhalfed` | same | 65 < f < 73, even f | 105 | (105, face−91, 64, 91, 5, 1) |
| `ripkilldeath` | same | 115 < f < 126 | 130, twice a tick | (130, face−91, 64, 91, 5, 1) |
| `walk to dead`, character named "Rand" | none | f % 5 == 0 | 40 | (40, face−128, 32, 32, 5, 1) |

The port had the 1998 snapshot's single "impdecap" squirt instead.
A spell also bleeds (`0x00524bfb`, the "Blood Drain" family; not combat).

### 2.2 Who bleeds, and in what colour

- `chardata+0x164` is BLEEDER (`rules.h` `bleeder`, default 1; char.def
  `BLEEDER`/`BLEADER`), used only as a yes/no. `chardata+0xc8 & 0x10` is
  CF_BADBLEEDER, set when the BLEEDER value fails to parse.
- ResolveImpact bleeds only **without** CF_BADBLEEDER; ResolveDead only
  **with** it (COMBAT_HIT §3.8, §9.3 (5)), so in the shipped data a death
  never bleeds through ResolveDead. The death squirts above use the
  ResolveImpact sense. A burning character never bleeds.
- **Colour:** there is one blood, red: `Misc\Blood.I3D` texture 0, RGB565
  (148,0,0)-class red over black, and texture 1, an ARGB4444 black opacity
  mask. Nothing per character selects another. (`Trollblood` is a separate
  spell effect.) The violence level (INI `Violence`, retail
  `0x005d79e8`, default 5) sizes the spray (§2.3) and decides whether
  droplets stay on the ground (§2.4).

### 2.3 TBloodAnimator::Initialize `0x004f18d0`

TBloodAnimator: `0x558` bytes, vtable `0x005acd18`, builder `0x004f18b0`
(vtable `0x005acd14`, New `0x004f9550`). Overrides: Initialize (slot
`0x18`), Pulse (`0x2c`, `0x004f1c60`), Render (`0x34`, `0x004f1de0`),
RefreshZBuffer (`0x3c`, `0x004f2130`). The droplets are 30 records of
`0x24` bytes at `+0xfc`: vel xyz, pos xyz, scale, used (dword), then the
bytes size, stage, count, delay.

```
T3DAnimator::Initialize()                         // 0x0040dd60
inst->GetParams(&height, &ha, &va, &hs, &vs, &unused)
mask[0..3]  = GetObject(4..7); colour[0..3] = GetObject(0..3)   // +0x534..+0x550
GetVerts(each, D3DFVF_LVERTEX 0x1e2)
n = min(violence * 3, 14)
count = (n - random(0, n) > 0) ? n - random(0, n) : 0   // a max() macro: two draws
clear all 30
for i in 0 .. count-1:            // droplet i and its trailing partner i + count
    a = byte((random(-hs, hs) + ha) & 0xff) * 360 * (1/256) * deg   // deg = 0.017453292522222223
    e = byte((random(-vs, vs) + va) & 0xff) * 360 * (1/256) * deg
    d.used = 1; d.size = random(0, 2); d.pos = (0, 0, height)
    d.vel.x = 2*random(100,300) * cos(a) * 0.01
    d.vel.y = 2*random(100,300) * sin(a) * 0.01
    d.vel.z = 2*random(100,300) * sin(e) * 0.005     // half the snapshot's
    d.scale = 0.5; d.stage = fly; d.delay = 0
    t.used = 1; t.size = random(0, t.size) = 0        // equal endpoints: no draw
    t.pos = (0, 0, height)
    t.vel.x = random(60,80) * d.vel.x * cos(a) / 70   // the cosine applied twice
    t.vel.y = random(60,80) * d.vel.y * sin(a) / 70
    t.vel.z = random(60,80) * d.vel.z * sin(e) / 80
    t.delay = byte(random(-1, 1) + d.delay + 3); t.stage = fly; t.scale = 0.5
```

The requested count (`+0x198`) is never read. With the default violence 5 a
blow throws up to 14 droplets plus 14 trailing ones.

### 2.4 TBloodAnimator::Pulse `0x004f1c60` (once a tick)

```
done = 1; T3DAnimator::Pulse()                     // 0x0040e2e0
for each used droplet:
    if delay: delay--, continue
    done = 0
    fly:    pos += vel; vel.xy *= 0.95; vel.z -= 0.37; scale += 0.01
            if pos.z <= 20: pos.z = 20, stage = splat, count = 0,
                            unused if violence < 3 or size == 0
    splat:  if count == 0: scale = 1.8 - random(0, 7) * 0.1
            if ++count > 30: stage = shrink, count = 0
            if size == 2: size = 3                  // a big droplet lies as the splat sprite
    shrink: scale -= 0.1; if scale <= 0: scale = 0, unused
if done: inst->SetFlags(flags | OF_KILL)
```

Ground is the spawn's own frame: `pos.z` starts at the spray height and
lands at 20 (render z is `pos.z − height`, §2.5).

### 2.5 TBloodAnimator::Render `0x004f1de0`

Per used droplet, in slot order, two draws:

1. blend mode 2 (alpha; no z-write, z-test): the mask box by size,
   objects 4/5/6/7 (small, medium, big, splat; texture 1, black);
2. blend mode 8 (add, ONE/ONE): the colour box by size, objects 0/1/2/3
   (texture 0, red).

Matrix: `Scale(scale)`, then for a flying droplet `RotateX(−π/2)`,
`RotateZ(−π/4)` (`0xbfc90fdb`, `0xbf490fdb`: the flat ground box stands up
to face the camera), then `Translate(pos.x, pos.y, pos.z − height)`;
RenderObject composes it with the animator's world matrix. Splat and
shrink keep the box flat on the ground.

Prelit vertex colour (both draws, computed once): start from twice the map
ambient (`MapPane +0x8cc`, `0x006671a4`), add the brightness of each of up
to `MaxLights` (≤ 3) closest lights at the droplet (`0x00415b40`,
`0x00415f40`), divide by the number found (at least 1), `e = ftol(x·255)`,
then an ill-formed byte merge (`0x004f1fb6..0x004f1fda`):
`ARGB = (((e | 0xffffff00) << 8 | e) << 8) | e`, i.e. A=FF,
R = e0|e1|e2, G = e0|e1, B = e0 for the bytes of `e`. Twice the ambient
already exceeds 1, so in practice the red pass is near full intensity:
blood is effectively unlit.

RefreshZBuffer (`0x004f2130`) restores the 2D Z buffer behind each droplet
for the tile renderer; the port's depth buffer needs no equivalent.

### 2.6 Port

- `src/effects/bloodspray.{h,cpp}`: the droplet simulation (Initialize and
  Pulse bodies), engine-free. `src/effects/retailblood.{h,cpp}`:
  TBloodAnimator (Initialize, Pulse, Submit), the "Blood" effect builder
  (TBloodEffect) and the animator builder, which attaches a visual
  component so the boxes never draw as static meshes. The 1998
  TBloodAnimator declaration in `effect.h` is kept under `#ifdef UNUSED`.
- `src/character.cpp`: EffectBurst passes retail's parameters;
  `SquirtBlood` and the four Pulse squirts replace the snapshot's.
- Rendering: two FX quads per droplet in submission order (Alpha, then
  AdditiveStraight ONE/ONE), depth-tested without writes, prelit colour,
  corners from the authored box through the owner's transform.

**Verification (emulator).** `tools/retail_runtime/blood_spray_probe.py`
runs the original Initialize and Pulse on a fixture animator (T3DAnimator
calls, GetObject, GetVerts and SetFlags answered and recorded; GetParams
original) and compiles `bloodspray.cpp` unchanged with the same MSVC rand
stream: 33 cases (combat and squirt parameters, violence 0–5, 24 seeded
headings), 1769 ticks, every droplet field bit-identical, the same rand()
count and kill tick; no float differed even in the sin/cos products.
`test_blood_spray.py` adds two negative controls (the snapshot's vertical
speed; a single count draw), both caught.

**Verification (images).** [blood_retail_vs_port.png](combat_effects/blood_retail_vs_port.png):
retail splats on the Keep floor (red, 10–20 px, gone within about a
second) against the port's Araknid hit in the arena (spray, splat, hold,
shrink). [blood_isolated_gallery.png](combat_effects/blood_isolated_gallery.png):
one owner alone (`--test=vfx --vfx=TBloodEffect_RETAIL`). The 6–8 fps
DOSBox capture barely resolves droplets in flight; the splats' form,
colour, size and lifetime match.

**Divergences (none noticeable).** The port eases each droplet between
its last two ticks within a stage (retail drew the tick state; the port
lags it by at most one tick). The animator initializes when the port
creates it (the first frame after the spawning tick, like retail's
on-screen creation), so its draws come after that tick's other pulses.

## 3. Combat flash

### 3.1 EffectCombatFlash `0x004c8500`

```
def = {EFFECT, FindObjType("combatflash"), pos = pos + (30, 30, 80)}
inst = GetInstance(NewObject(&def)); if (!inst) return
n = imagery ? imagery->NumStates() : 1                // 18
if n < 1: inst->SetState(0); return                   // vtable +0x18
s = 2 * random(0, n / 2); if (s != n - 2 && s >= n - 2) s = n - 2
inst->SetState(s)
```

`random(0, 9)` doubled and capped at 16: start1..start8 one chance in ten
each, start9 two. Callers: ResolveImpact and ResolveDead on the first tick,
each also setting `+0x224 = 5` (§4), and a monster AI script step
(`0x004cb4ea`, outside melee).

### 3.2 The asset and its nine colours

`combatflash` (0xad92bd29) is a generic EFFECT with the default animator
on `misc\Impact.i3d`: 18 states (nine 19-frame starts, each followed by a
one-frame end), 21 objects, 10 RGB565 64×64 textures (a star and a glow
dot over a coloured lens flare), 18 tags. Each start state has
`blendcont litadd` on its first frame (every object additive, `#` objects
camera-facing) and one `partsys`. The generic TEffect::Pulse
(`0x004de800`) removes the effect when the 19-frame state ends, taking
its particles with it (about 0.8 s).

| Start | State | Particle (texture) | Flare | Particles: colour over life | Notes |
|---|---|---|---|---|---|
| 1 | 0 | #particle01 (0) | #impact01, orange | (255,225,100)→(255,175,50)→black | 250 pps, 30–45 ticks |
| 2 | 2 | #particle02 (1) | #impact02, cyan | (100,200,255)→(50,125,255)→black | **numtrails 2:3**, glow dots |
| 3 | 4 | #particle03 (3) | #impact03, violet | white→(120,1,255)→black | trailing `,` |
| 4 | 6 | #particle01 (0) | #impact04, blue | white→(1,1,255)→black | velocity −25..−40: implodes |
| 5 | 8 | #particle02 (1) | #impact05, violet | white→(100,1,255)→black | numtrails 0:0 |
| 6 | 10 | #particle03 (3) | #impact06, orange-red | white→(255,50,1)→black | trailing `,` |
| 7 | 12 | #particle01 (0) | (#impact07: no faces) | white→(115,240,255)→black | implodes |
| 8 | 14 | #particle02 (1) | #impact08, red | white→(255,1,1)→black | numtrails 0:0 |
| 9 | 16 | #particle03 (3) | (#impact09: no faces) | white→(100,225,75)→black | trailing `,` |

The other states' flares stay parked at tiny scales (0.02–0.15) and draw
as specks, as in retail. The exact tag strings and fingerprints are in
`src/combatflashprofile.h`.

### 3.3 Partsys semantics the other states need (all from retail)

- **Trailing comma.** ParseParams `0x0040d3e0`: after an item, a `,` is
  consumed (`0x0040d461..0x0040d475`) and the loop ends cleanly on end of
  text (`0x0040d47a`), so `...,azimuth=360,` parses.
- **numtrails low:high.** Particle Init `0x004031c0`, last in the spawn
  (`0x00403265..0x004032a4`): if the template's low endpoint (`+0x6c`) is
  nonzero, `count = random(ftol(low), ftol(high))` (a draw unless equal),
  `count` history entries initialized to the corrected spawn position;
  low 0 means none whatever the high.
- **Trail draws.** Particle Render `0x004026d0`: the main draw, then
  history shifted down one and each entry drawn with the same rotation,
  scale and colour, the last (`0x00402990..0x00402a0b`) being the current
  position: `n + 1` draws for `n` trails.

### 3.4 Port

- `src/combatflashprofile.h` fingerprints the whole asset;
  `ValidateCombatFlashPartSysProfile` admits it, and every state runs its
  blendcont and partsys (`3dimage.cpp`). `CombatFlashBaseMeshBlend` /
  `CombatFlashBaseMeshWorldMatrix(object)` draw any Impact flare additive,
  depth-tested, `#` camera matrices, at its authored key for the current
  state, with CalcObjectMatrix's scale gate (`≤ 1e-5` rejects). An Impact
  object off that route draws nothing (`maprenderer.cpp`): no opaque
  substitute.
- `partsysdefinition.cpp` accepts the trailing comma and integer
  `numtrails` ranges 0..64; `authoredpartsys.cpp` draws the trail count
  last in the spawn; `SubmitPartSys` draws combat-flash trails after the
  main draw. Retail rendered once a tick, shifting history per render; the
  port shifts on the first render after a tick so a trail spans ticks at
  any frame rate.
- Gallery: `TCombatFlashEffect_AUTHORED_START1..9`
  (`src/effects/combatflashpreview.cpp`), one owner in one state.

**Verification.** [combatflash_retail_vs_port.png](combat_effects/combatflash_retail_vs_port.png):
retail hits from the recording (green start9, cyan start2 with its glow
dots and streaks, orange-red start6, the bright onset of a white start)
against the port's arena hits (start4 blue flare and implosion, start9)
and gallery states. [combatflash_states.png](combat_effects/combatflash_states.png):
all nine states, one owner each, on black. The gallery's filmstrip begins
after the first tick, so its first column is already past the white onset
the arena shows. The partsys changes leave the VFX track's compiled
controls green (`test_skin_trails.py`, now with range/zero-low/trailing-
comma controls, and the byte-identical default-zero traces;
`test_speed_packets.py`, `test_quicksilver.py`, `test_static_particles.py`,
`test_vfxreviewspawn.py`).

**Open.** No emulator run of an Impact state other than 0 yet (the trail
draw order and count are read from the asm, not executed); render-time
rotation sampling still draws on the game's random stream once per drawn
frame, as for every partsys effect in the port.

## 4. The struck body (`+0x224`)

ResolveImpact (`0x004c75ab`), ResolveDead (`0x004c788a`), ResolveHit's
lethal hit (`0x004c680a`, `0x004c6a67`) and KnockBack (`0x004d3885`) set
TCharacter `+0x224 = 5`; Pulse decrements it early on
(`0x004c1c21..0x004c1c2c`). TCharAnimator::Render (`0x004d7a50`), after the
character's normal draw:

```
if (!preview(+0x1a4) && inst+0x224 >= 1):        // 0x004d7bd8
    Scene.SetBlendMode(0x10, 1)                   // litadd: ONE/ONE, lit, depth-tested, no writes
    T3DAnimator::Render()                         // the whole character again
    Scene.SetBlendMode(4, 1)
```

and the player's equipment pass draws worn items again in mode 8 (add,
`0x004d8148`). The lit colour adds onto itself: for five ticks the struck
body shines white-hot (retail recording: Locke and Rahul turn pale yellow
on every blow). The snapshot's `RenderCombatFlashes` (a utility quad) is
not what shipped.

**Port.** `TCharAnimator::DrawsCombatFlash()`; `SMeshSubmit::additive`
draws a mesh's lit colour again with ONE/ONE against its own depth
(LESS_EQUAL, no writes; `renderer.cpp`); the map renderer submits that
second pass while the ticks run. Worn equipment is not drawn on characters
in the port yet, so its mode 8 pass waits for it.
[struck_body_retail_vs_port.png](combat_effects/struck_body_retail_vs_port.png).

## 5. Sparks (unchanged)

A miss against a blocker with both armed: EffectBurst "sparks"
(the sparks branch of `0x004c85d0`): object at the attacker, `random(−80, 80)` on
the facing for the spray, the point halfway to the target 45 up,
`random(15, 25)` particles, spread 3/0.5, gravity 0.25, trails 2, start
0–8, life 20–40, bounce, kill with the object. The port's branch matches
(`character.cpp`), and its TSparkEffect has the VFX track's editor-default
visual pass. Not exercised here: the arena's fights produced no block.

## 6. Weapon swipe

The ribbon a weapon leaves through an attack. Retail draws it for every
character with a SWIPECOLOR; the 1998 snapshot's version (`weapontrail.cpp`)
is not what shipped.

### 6.1 Where it lives

TCharAnimator embeds a TWeaponSwipe at `+0x1a8` (vtable `0x005a7e98`, whose
only slot is the deleting destructor; the constructor inline at
`0x004d770c` clears its running flag `+0x528`).

- **Animate** `0x004d7a20` (vtable slot 12): SetupWeaponSwipe `0x004d8f10`
  while the swipe isn't running, then T3DAnimator::Animate.
- **Render** `0x004d7a50`: nothing while OF_INVISIBLE or below the drawn
  alpha floor (0.01). At `0x004d7c23`, unless the animator is drawing the
  inventory portrait (`+0x1a4`, set only around `0x00536fda`) and while
  running: Update `0x004d95b0`; then, still running, blend mode 2
  (`0x00417d60`), BuildStrip `0x004d9380` and RenderObject `0x0040a8f0`
  under the animator's matrix (`+0xac`).
- The destructor closes it (`0x004d7902`).

### 6.2 SetupWeaponSwipe `0x004d8f10`

1. The weapon's bounds, GetWeaponExtents `0x004d8e60`: a player's weapon
   in hand (`+0x2b0`) on its own imagery, the object GetWeaponNum
   `0x004d8bd0` finds for this body (the weapon's "still"/"all" state, or
   the body type's, a name prefix; then its first shown object the body
   has a place for, "weapon" falling back to "sword"); otherwise this
   character's own `weapon`, `sword` or `ogrokaxe` object. T3DImagery
   `0x00408c20` at state 0, frame 0: the vertex bounds, from ±1e6 with
   strict compares. Zero with no object.
2. A player's weapon in hand: its name (`+0x38`) is copied, and SwipeFull
   is the weapon's `SwipeFull` stat (vtable `+0xd4`; class.def's weapon
   STATS `SWFL 0 0 1`).
3. No SWIPECOLOR (`+0x168..+0x16a` all zero): nothing. (A check of the
   bytes against −1 after zero-extension never fires.)
4. The colour: each byte × `0.00392156886f` (`0x005a3830`). The parser
   (`0x0048a4a3`, `"%b, %b, %b"`) stores red at `+0x16a`, blue at `+0x168`.
   **Not normalised:** NormalizeColors `0x004d9980` is reachable only from
   ChangeColor `0x004d9ac0`, which nothing calls. Locke's 200,0,230 draws
   0.78,0,0.90; the 5,5,5 of Jhaga, the Shadow Minion and the Dark
   Revenant draws an almost black, smoky trail (normalised, as the
   snapshot did, it would be white).
5. A character's SwipeFull is SWIPEFULL (`+0x16c`, a byte).
6. Init `0x004d90b0` with maxsegs 40 and smooth 8.

### 6.3 Init `0x004d90b0`, ChooseAxis `0x004d9820`

40/8 + 3 = **8 samples** a side, hilt (`+0x35c`) and tip (`+0x360`), in
zero-filled arrays (`0x00482fb0`); a 112-vertex D3DLVERTEX buffer (FVF
`0x1e2`) and 80 faces (i, i+1, i+2). Then 41 AddPoints of the weapon as it
is (with the previous axis: zero the first time) and ChooseAxis: the
longest axis of the bounds, ties to the later axis. The hilt point is the
weapon's pivot (or, with SwipeFull, the low end of that axis); the tip is
the high end, **except on y**: `0x004d9909` loads `+0x388` (min.y) again,
so a weapon modelled along y sweeps its short end. The port keeps that.

### 6.4 Update `0x004d95b0`, once a drawn frame

- Not doing ACTION_ATTACK (doing `+0xd8`, action 7), or a player with no
  weapon in hand: **ten Shrinks** (maxsegs/4). Each pair of samples closes
  on its midpoint by 1 − 1/8 (`0x004d9b60`), so an idle swipe vanishes in
  a few frames.
- Attacking, with attack data (GetDoing, vtable `+0x1fc`, `+0x48`) and a
  window (swipeframeon `+0xec` / swipeframeoff `+0xf0`, char.def ATTACK
  fields 23/24; both 0 is none): at frame == on, **Reset** `0x004d9c60`
  sets every sample to the weapon now; while on ≤ frame ≤ off,
  **AddPoint** `0x004d96c0`: Shift (`0x004d9ae0`), Shrink, and sample 0 =
  the weapon object's matrix (GetObject `0x0040eef0` `+0x58`, model space)
  × the two axis points (`0x0043ad80`).
- Otherwise (outside the window, no data): Shift and Shrink: the history
  runs on into the stationary newest sample.

AddPoint closes the swipe when a player's weapon changes (the copied name
against the one in hand, `_stricmp` `0x0059a530`); the next Animate sets
it up again. With no weapon object AddPoint would transform through a null
matrix; every character with a SWIPECOLOR has one (`Skeleton.I3D` calls its
sword `ogrokaxe`).

### 6.5 BuildStrip `0x004d9380` and the draw

Five segments of eight steps: Catmull-Rom (`0x0043b280`) through samples
p..p+3 for p = 0..4, so the strip runs **from sample 1**, a frame behind
the blade, to sample 6; sample 0 only bends the first segment. Each step
is a vertex pair (hilt, tip) of colour `alpha<<24 | R<<16 | G<<8 | B`
(`ftol(x × 255)`); alpha starts at 0.7 and loses 0.7/48 a step (0.13 at
the tail). Blend mode 2 (`0x00417f46`): SRCALPHA/INVSRCALPHA, depth
tested, no depth writes, no culling; prelit, untextured. The vertices are
in the character's model space and drawn under the animator matrix
(object flags `0x5a110` with an identity matrix), so the trail moves with
the character.

**Retail bug.** The 80 faces run two past the 80 vertices BuildStrip
writes: faces 78 and 79 index the zero-filled vertices 80 and 81 (the
emulator confirms they stay zero), a faint wedge, alpha 0.13 → 0 toward
black, from the strip's tail to the character's origin at its feet. The
port draws the 78 strip triangles (REVSYNC-DIVERGENCE in
`charanimator.cpp`).

SWIPECOLOR in char.def: Locke 200,0,230; Bayne, Morganna, Navarro 50,0,75;
Jong 155,0,155; the seven Ogroks 255,0,0; Dorogar 0,25,0; Acolyte and
Arcanist 0,0,150; Cinderate and Burning Praetor 155,0,0; Druid and Invoker
0,150,0; Jhaga, Shadow Minion, Dark Revenant 5,5,5; Skeleton 255,0,0;
Vashar 0,80,40; Ramage 0,50,0; Sirocco 50,0,0. Locke's swords swipe in
swing (frames 2–6), chop (3–8), upslash, spinswing and the chains; his
hand-to-hand attacks have no window.

### 6.6 Cadence

Retail updates once a drawn frame, and drew at most once a 24 Hz tick. The
port updates on the first drawn frame after each tick, at that tick's key
(`T3DAnimator::ObjectModelMatrix` at fraction 0, the pose the renderer
draws there), and keeps the history the next key's update would make
(fraction 1). Frames between draw the history eased toward it: at each
tick the strip is retail's, and between ticks it follows the interpolated
blade at any frame rate. No random numbers are drawn.

### 6.7 Port

- `src/weaponswipe.{h,cpp}`: the retail class, free of engine types. It
  replaces the VFX track's S09 Phase B class (snapshot constants, unused
  in play) under the same name.
- `TCharAnimator` (`charanimator.cpp`): SetupWeaponSwipe, WeaponExtents,
  WeaponObjectNum, WeaponSwipeFrame, UpdateWeaponSwipe (from
  UpdateDrawState) and SubmitFx: 39 FX quads a strip, Alpha,
  TestNoWrite, unlit, per-corner colour.
- `T3DImagery::GetObjectBounds` (`0x00408c20`);
  `T3DAnimator::ObjectModelMatrix` and `NextKey`; `TTransform::ComposeLocal`.
- The map renderer calls `T3DAnimator::SubmitFx` once an object a gather.
- The VFX gallery's `TWeaponSwipe` entry drives the retail class on the
  rig's character; `--vfx-rig=<type>` picks it (e.g. `Skeleton`).

### 6.8 Evidence

- **Emulator A/B** (`tools/retail_runtime/weapon_swipe_probe.py`): the
  original SetupWeaponSwipe, Update and BuildStrip a drawn frame at a time
  against `src/weaponswipe.cpp` compiled unchanged: 45 cases, 2138 frames
  (2125 running). The running flag, the colour, both eight-sample
  histories and the 80 strip vertices (position bits and packed colour)
  match bit for bit in every frame. Cases cover char.def's swings, all
  bounds shapes (each axis, both tie kinds, zero, no vertices),
  SwipeFull, no window, missing attack data, a skipped opening frame,
  off before on, other actions, an armed and an unarmed player, weapon
  changes (case-insensitive) and no SWIPECOLOR, plus fuzzed sequences.
  `test_weapon_swipe.py` adds three negative controls that must fail and
  do: the snapshot's strip from sample 0, the tip at max.y, and no reset
  when the window opens.
- **Combat arena**, `locke_vs_araknid`, one tick a frame: Locke's sword
  swing and chop,
  [swipe_arena_locke.png](combat_effects/swipe_arena_locke.png).
- **Gallery**, one Skeleton swinging,
  [swipe_gallery_skeleton.png](combat_effects/swipe_gallery_skeleton.png).
- **No retail footage yet.** The recorded Rahul fight has none to show:
  Locke fights unarmed (hand attacks have no window) and Rahul has no
  SWIPECOLOR. The lab capture to make (load the Port Resave slot in
  retail, fight the forest's Druhgs with Locke's sword) could not run: the
  DOSBox lab was held by another dojo run.

## 7. Bloody chunks

`TCharAnimator::RenderBloodyChunks` is an empty snapshot stub, and retail's
TCharAnimator::Render draws no such thing (shadow, character, struck-body
pass, swipe, torch). The gore of a pulped body is TPulpAnimator (vtable
`0x005aa820`, Animate `0x004e8d10`, which drops "Blood%d"/"BloodS%d" floor
decals), spawned by spells, not by melee.

## 8. RNG

In retail order within a blow: EffectBurst's `random(1,5)` (object
created), EffectCombatFlash's `random(0, n/2)`; at the next draw the blood
animator's Initialize (per pair: two spreads, size, three speeds, three
trailing factors, a delay), then each tick's splat sizes; the combat
flash's partsys draws in its Pulse (spawn) and Render (rotation samples,
numtrails). The weapon swipe draws none. The katas seam EffectBurst and EffectCombatFlash, so they are
unaffected: melee-resolvers 2100/2100, melee-damage 1137/1137, melee-hit
7855/7855, combat-update 259/259 match; the arena stays deterministic
(`--repeat 2`, identical traces).
