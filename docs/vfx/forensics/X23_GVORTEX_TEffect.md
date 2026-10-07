# X23 `gvortex` — the resurrection vortex (generic `TEffect` + `T3DAnimator`)

The green vortex that rises over the resurrection circle at the start of a new
game, while Locke is brought back. Retail decomp is the source for every fact
below unless marked; addresses are in `Revenant.exe` (retail, Ghidra project
RevenantDev).

## 1. Summary

`gvortex` is not a bespoke effect class. It is an EFFECT-class object type
whose imagery, `Misc\Gvortex.i3d`, carries everything: a 240-frame keyframed
animation of 12 objects (two cylinders for the column, five swept "blast"
ribbons, four flare cards, a floor quad), a `play` tag for its sound and a
`blendcont` tag that makes every object draw additively. Retail builds it as
a plain `TEffect` driven by the default `T3DAnimator`. The effect plays its
animation once (10 s at 24 ticks/s), plays `gvortex.wav` on its first tick,
and removes itself when the animation has played out.

The opening's script makes it (`keep.s`, `OBJECT "SardokR"`):

```
wait 20
nowait add gvortex            ; at the camera's map position (Locke, 1212,545)
nowait gvortex.move 0 0 20    ; 20 units up
wait 50
player.toggle invisible
fadecharacterin player
player.try resurrect          ; Locke.I3D state 350 'resurrect', 248 frames
```

Nothing deletes it; its own animation ends it.

## 2. Sources & evidence

| What | Where |
|---|---|
| Type entry | `class.def` (in `imagery.rvi`): `"gvortex" "Misc\Gvortex.i3d" 0xad99bd33 {1,0} {}` in `CLASS "EFFECT"`; the braces are the class stats FullZRefresh=1, DetailLevel=0 |
| Object builder | none named `gvortex` (no such string in the exe). `TObjectClass::AddType` falls back to the class name; retail's EFFECT builder is registered as `"effect"` (0x004de750, string 0x005e1054), builder vtable 0x005a85a8, `Build` 0x004f4a30: a 0x184-byte `TEffect`, vtable 0x005a85ac, flags \|= def flags \| 0x48001 (OF_IMMOBILE, OF_PULSE, OF_NOTIFY) |
| Animator | no animator builder named `gvortex` or `EFFECT`; the default `T3DAnimatorBuilder` ("default", 0x0040dbd0, registry 0x005e851c / count 0x005e872c) builds a `T3DAnimator` (0x0040dc00, 0xfc bytes, vtable 0x005a370c). Retail's torches are the same shape: `"FLAME"` (0x004e4ea0) goes to the named animator-builder ctor 0x0040db90, not the object-builder ctor 0x0046df00 |
| `TEffect::Pulse` | 0x004de800 (vtable slot 0x110) |
| `TObjectInstance::Pulse` | 0x004708e0: `animator->Pulse()` (slot 0x2c), then the script |
| `T3DAnimator` | Initialize 0x0040dd60, Close 0x0040de10, Pulse 0x0040e2e0, Animate 0x0040e460, Render 0x0040e8d0, RefreshControllers 0x0040df90 (slot 0x50) |
| `TObjectAnimator::Animate` | 0x00445a50: copies the instance's state, prev state, frame, prev frame, frame rate into the animator |
| Tag sounds | `T3DImagery::PlaySound` 0x0040b2e0; name resolution 0x0040af50; terrain lookup 0x00452ea0; `TObjectInstance::PlayWave` 0x00473990; FindSound 0x0049c430, load 0x0049b650, play 0x0049b990 |
| Tag controllers | registry 0x005e8304 (count 0x005e8728, 128 slots), builder ctor 0x0040d320; `blendcont` registration 0x00405750 (builder vtable 0x005a3578, Build 0x00405f60, controller vtable 0x005a357c); its ParseItem 0x00405770, Initialize 0x00405940; base Initialize 0x0040d750, base ParseItem 0x0040d4a0 ("obj") |
| Draw | `RenderObject` 0x0040a8f0 (object flag 0x1000000 + mode at S3DAnimObj +0x348 → 0x00417d60); blend states 0x00417d60; render-state wrapper 0x00417060 (software rasterizer 0x0056d400 when 0x005d7a28 is set) |
| Asset facts | dumped with the port's I3D loader and `--dumpi3d` |
| Retail footage | dosbox-x lab (Rev98.exe, **Software3D**, 640×480, RETAIL_CAPTURE.md): stills `docs/gameflow/reference/opening/02_vortex_swirl.png`, `03_vortex_glow.png`, `04_vortex_column.png`; the recording with audio `~/RevenantRetailLab/captures/gameflow/opening-20261006-run1.avi` (the vortex starts about 17 s in) |

The 1998 snapshot has `TEffect::Pulse` as `TObjectInstance::Pulse(); SetFrame(0); SetCommandDone(false);` and a controller system with only `scrolltex` / `animtex`; retail differs in both (§6, §7).

## 3. Constants

| Constant | Value | Source |
|---|---|---|
| Animation | 1 state `start`, 240 frames, aniflags 0x2000 (AF_NOMOTION; **not** AF_LOOPING) | Gvortex.i3d |
| Frame rate | one frame per game tick (`NextFrame`), 24 ticks/s: 10.0 s | engine |
| Spawn position | `add`: the camera's map position; `move 0 0 20`: +20 z | keep.s |
| Sound | tag `play` state 0 frame 1 = `gvortex` → `Sound\effects\gvortex.wav`, 22050 Hz mono 16-bit, 229293 frames = 10.398 s | Gvortex.i3d, resources.rvr |
| Blend | tag `blendcont` state 0 frame 2 = `litadd` → mode 0x10 on every object | Gvortex.i3d |
| Materials | 3, each diffuse = ambient = emissive = (1,1,1), alpha 1; textures 0-2 | Gvortex.i3d |
| Light | none (no OF_LIGHT, no light definition) | class.def, 0x004de800 |

## 4. Assets

`Imagery\Misc\Gvortex.i3d` (110394 B; `Magic\gvortex.I3D` is a byte-identical
copy the Misthaven recall uses). Version 3, flags 0xdc.

| # | Object | Verts / faces | Material → texture | Role (from the animation) |
|---|---|---|---|---|
| 0 | `box01` | 4 / 2 | 0 → tex 0 | floor quad, 13 × 13 at z 0 |
| 1-3, 6-7 | `blast 01`-`05` | 20 / 18 | 1 → tex 1 | the swept ribbons, 47.7 tall |
| 4, 5 | `cylinder07`, `cylinder06` | 22 / 20 | 2 → tex 2 | the column (scaled up by the animation) |
| 8-11 | `#$flare01`-`04` | 48 / 24 | 0 → tex 0 | flare cards (star, glow) |

Textures: tex 0 64×64 (an atlas: glow disc, star, column gradient, arc), tex 1
128×128 (green swirl streaks), tex 2 128×128 (green column streaks). All green
on black; the black is keyed out (alpha 0 on 40 / 68 / 29 % of texels).

## 5. Spawn & emit

One object, placed by the script (§1); no particles, no sub-effects. The
animation's keys move, rotate and scale the 12 objects about the object's
position.

## 6. Behavior & per-frame logic

```
TEffect::Pulse (0x004de800), the part a script-added effect runs:
    TObjectInstance::Pulse()            // animator->Pulse(): controllers, tag sounds; script
    if animator && !lightdef(+0xd8)
       && !(imagery->GetAniFlags(state) & AF_LOOPING)
       && CommandDone():                // NextFrame passed the last frame
        SetFlags(flags | OF_KILL)       // removed next PulseObjects
    SetCommandDone(false)
```

The rest of 0x004de800 (not used by gvortex): a start delay at +0x138 (holds
OF_ANIMATE off until it counts down), a spell effect following its invoker or
target (+0xe4), the light fade of effects with a light definition (+0xd8), a
timed life (+0x130 / +0x12c) that blinks the effect out over its last +0x134
ticks. The snapshot's `SetFrame(0)` is gone.

```
T3DAnimator::Pulse (0x0040e2e0):
    RefreshControllers(inst->state)     // 0x0040df90, slot 0x50
    for each controller: Pulse()        // slot 0x14
    imagery->PlaySound(inst, state, frame)   // 0x0040b2e0
    tags "effect" / "deleteeffect" for (state, frame)
    tag "knockbackplayer" (0x0040b5c0)
```

Tick order (TMapPane::Pulse): `NextFrameObjects` (frame += 1) then
`PulseObjects`; the animator copies the instance's frame when it draws
(0x00445a50). So at pulse time the animator holds the frame drawn last and
the instance is one ahead.

**Tag sounds (0x0040b2e0).** Every `play` tag of the state whose frame equals
the animator's frame + 1 (i.e. the frame the instance has just reached) plays
one name picked at random from its comma list. All matching tags play, not
just the first. With no instance the sample plays flat at volume 0x7f;
otherwise the name goes through 0x0040af50 and plays at the object
(0x00473990):

| Name | Retail resolution (0x0040af50) |
|---|---|
| contains `STEP`, on a character or the player | + terrain under it: walkmap bits 10-15 (0x00452ea0) & 0xf: 5 `wood`, 2 `stone`, 8 `carpet`, 4 `grass`, else `dirt`; volume 0x5c (0x40 sneaking) |
| starts `IMP`, no trailing digit, a struck character with a weapon | + `sword` / `bigsword` (two-handers, 1 in 5) / `staff` / `bow` / `hand` |
| starts `BLOCK` | nothing |
| anything else | as is |

gvortex's tag `gvortex` (frame 1) therefore plays on the effect's first tick.

**Tag controllers (0x0040df90).** On the state the animator enters (and, on
its first refresh, every state -1 tag) each tag whose name — compared without
case — is a registered controller builds one; `play`, `beg`, `end` never do.
The controller parses its string as `item[=value],...`; an unknown item
fails it (deleted). `blendcont`:

| Item | Mode (S3DAnimObj +0x348) |
|---|---|
| `none` | 0 |
| `normal` | 0x01 |
| `alpha` | 0x02 |
| `litalpha` | 0x04 |
| `add` | 0x08 |
| `litadd` | 0x10 |
| `nocheckz` | 0x41 |
| `litalphaz` | 0x44 |
| `litaddz` | 0x50 |
| `alphaadd` | 0x20 |
| `obj=(a,b)` | names objects (base parser) |

Initialize (0x00405940): when the tag named **no** objects, every object of
the animator gets flag 0x1000000 and `mode | (old & 0x40)`; a tag with an
object list sets nothing (retail never applies it to the listed objects). The
controller's Pulse and Render are empty. The mode stays on the objects after
the state changes. Shipped data: 56 `blendcont` tags in 42 I3D files — litadd
36, litaddz 12, litalpha 8 — none with an object list or `filename`.

## 7. Rendering

`RenderObject` (0x0040a8f0) sets the object's mode through 0x00417d60 before
its DrawIndexedPrimitive (FVF 0x112: position, normal, one UV — D3D lights
it with the object's material). Lowest set mode bit wins:

| Mode | ZWRITE | ZENABLE | CULL | SRC / DEST | Color stage | Alpha stage |
|---|---|---|---|---|---|---|
| normal | z-buffer on | on unless 0x40 | CCW | ONE / ZERO | tex × diffuse (MODULATE, 2X/4X with overbright) | tex × diffuse |
| alpha | off | ″ | none | SRCALPHA / INVSRCALPHA | tex | tex |
| litalpha | off | ″ | none | SRCALPHA / INVSRCALPHA | tex × diffuse | tex × diffuse |
| add | off | ″ | none | ONE / ONE | tex | tex |
| **litadd** | off | ″ | none | **ONE / ONE** | **tex × diffuse** | tex |
| alphaadd | off | ″ | none | SRCALPHA / ONE | tex | tex |

(D3D6 render states 0xe ZWRITEENABLE, 7 ZENABLE, 0x16 CULLMODE, 0x13/0x14
SRC/DESTBLEND; texture stage ops via 0x00417390. ZWRITE is on for the
blended modes only with bit 0x80, which no tag sets. On a card flagged
0x0066818c, litalpha draws as alpha and litadd / alphaadd as add.)

"Lit" means the D3D vertex lighting of the object's material: `diffuse =
saturate(emissive + ambient·A + Σ diffuse·light)`. RenderObject turns the 2X
overbright stage off while it sets the mode of an object whose material has
a non-zero +0x34..+0x3c (its emissive color; 0x0040ae1f). gvortex's
materials have emissive (1,1,1), so on the hardware path its color is the
texture unchanged, added to the screen, both faces drawn, depth-tested, not
depth-written.

**The lab footage is the Software3D path** (0x005d7a28 set: render states
go to the software rasterizer's own state, 0x0056d400, and draws to its
DrawIndexedPrimitive, 0x0056eb30). For FVF 0x112 that function transforms
and lights each vertex itself (0x0056edc3-0x0056f0f6): color = the scene
ambient (0x00676058..60) + each directional light's N·L × color + each point
light in range, N·L / distance × color, clamped to 1 and stored as 5 bits.
**The material isn't read**: no emission, no material diffuse. So in
software the vortex adds the texture scaled by the dark Keep's light. The
lab footage agrees: the column adds about a fifth of the texture (measured
+18 green at its top, +36 mid, against +95 / +187 on the port's
hardware-path draw; no channel drops, so it is additive), and Locke reads
clearly inside it. On a Direct3D device the material's emission saturates
the color and the texture adds at full strength. Which look the author
intends is question 140; the port draws the Direct3D path, as it does for
every other 3D object (LIGHTING_FIDELITY.md §3).

## 8. Texture animation

None: no `scrolltex` / `animtex` / `fadeobj` tags. All motion is object
keys.

## 9. Associated light

None.

## 10. Color

Texture-driven green; additive stacking of both cylinder walls, the ribbons
and the flares saturates the column's base toward yellow-white on the
hardware path.

## 11. Audio coupling

Evidence: the lab audio (run1, a 50 s span from just before the vortex,
48 kHz DOSBox mix) cross-correlated with each sample; times are into that
span, the figure in brackets the normalized correlation.

| Sound | When | Evidence |
|---|---|---|
| `gvortex.wav` (10.4 s) | effect's first tick | at 3.27 s (0.98), as the vortex appears (first green at ≈3.7 s) |
| Locke `resurrect` (Locke.I3D state 350, 248 frames) frame 33 `loc1resscream` | — | no such sample in resources.rvr, imagery.rvi or Ahkuilon.rvm: silent in retail too (none found in the clip) |
| frames 163 `step1r,step2r`, 167 `step1l,step2l`, 199 `step1r,step2r` | as the frames come up | `step1ldirt` at 27.82 s (0.84), `step1rdirt` at 29.09 s (0.97); no stone/wood/carpet/grass variant matches: the circle is terrain 0 (dirt) |
| frame 169 `loc1resbreath` | — | no such sample: silent |

`Sound\effects\blank.wav` (196 B): a valid 22050 Hz mono 16-bit WAV whose
data chunk is empty, followed by a LIST chunk. Locke's grunt lists (states
72, 75, 85, 101, 102, 103, 105, 107, 110, 111) include `blank` entries so a
pick can be silence. Retail loads it (0x0049b650 reads the file) and Miles
plays it as nothing.

## 12. Triggers & in-game appearance

Only the opening (`keep.s` SardokR). The Misthaven recall uses the same
imagery through the `Teleporter` animator (M09), a different path.

Lab footage (Software3D), vortex lifetime ≈ 25 s of wall clock because the
emulated game slows under the effect: a small green flash at the circle; a
tall translucent green column; green swirl ribbons sweeping around the
circle; the circle's floor glowing yellow-green; Locke floating up inside the
column (his resurrect animation); the column narrowing and vanishing,
leaving Locke crouched, then standing.

## 13. Gaps & uncertainties

- **Hardware vs software look** (§7): the only footage is Software3D.
- The persistence of a blendcont mode across state changes has no effect for
  single-state effects; untested for multi-state ones.
- The `effect` / `deleteeffect` / `knockbackplayer` tags (0x0040e2e0,
  0x0040b5c0) aren't used by gvortex and aren't documented here.
- `STEP` terrain: the walkmap terrain bits (10-15) aren't documented here
  beyond the lookup.

## 14. Reconstruction (done 2026-10-06, gameflow track)

| Piece | Port |
|---|---|
| Builder / animator | already generic: `TGenericEffectBuilder("EFFECT")`, default `T3DAnimator` (src/effect.cpp, src/3dimage.cpp) |
| `TEffect::Pulse` | retail's generic part: no `SetFrame(0)`; non-looping state played out → OF_KILL (src/effect.cpp) |
| Tag sounds | `T3DImagery::PlaySound`: every `play` tag at frame + 1, BLOCK rule; STEP terrain and IMP weapon suffixes not ported (walkmap has no terrain bits; combat's weapon kinds) — the plain step samples are the dirt recordings, which is what retail plays here |
| blendcont | data, not a controller object: `T3DImagery::ResolveStateBlends` reads the tags at load into a per-state mode (`StateBlend`), BLEND3D_* in src/3dimage.h (per the 2026-04-30 decision not to bring back the T3DController hierarchy, attic/src/3dcontroller.cpp) |
| Draw | the map renderer's mesh submit sends a blend-mode object to the transparent pass (`SubmitHelperMesh`) with the mode's blend and shading: `EHelperMeshShade::TextureLit` = tex × saturate(emissive + ambient·A + diffuse·L·N.L), `Texture` = tex alone, `premultiply_alpha` for alphaadd (src/maprenderer.cpp, src/renderer.{h,cpp}). `...z` (no depth test) draws depth-tested, warned once |
| blank.wav | decodes to an empty, silent sound instead of failing (src/audio_backend.cpp, src/sound.cpp) |
| Trace | `[sound] <obj> plays <file> (<ms> ms)` / `no sound '<name>'` at TRACE from `TObjectInstance::PlayWave`; `[effect] <name>: state N played out, removed` at DEBUG |

Captures (port top, lab Software3D bottom, matched by the fraction of the
vortex's life: the lab's runs ≈25 s, frames a_037-a_290 at 10 fps, because
the emulated game slows under it; the port's 10 s):

- [captures/X23_gvortex_beats.png](../captures/X23_gvortex_beats.png): the
  three reference shots `docs/gameflow/reference/opening/02-04` (lab frames
  a_129, a_168, a_280: 36 %, 52 %, 96 %) against the port at 3.6 s, 5.2 s,
  9.6 s.
- [captures/X23_gvortex_port_vs_lab.png](../captures/X23_gvortex_port_vs_lab.png):
  seven beats from before the vortex to after it.

Differences that remain: the brightness (§7, question 140); the camera
(the circle sits about 50 px higher on the port's screen than on retail's
at the same moment); the pit inside the circle is black in the port and
blue-violet in retail (lighting track, RETAIL_CAPTURE.md §3); the floor
disc's glow reads smaller in the port because the circle's floor around it
is darker.
