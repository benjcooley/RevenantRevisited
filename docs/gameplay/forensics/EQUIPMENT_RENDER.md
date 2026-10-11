# Forensics: how a character holds and wears its equipment

**Topic:** where a character's weapon, shield and clothes are drawn: the
character's own "sword"/"weapon" objects, a player's equipped items riding
the character's objects, and the per-state object hierarchy those objects
hang from. The draw order of the rest of TCharAnimator::Render (shadow,
transparency, vision indicator, blood) is in
[../../RENDERER_ARCHITECTURE.md](../../RENDERER_ARCHITECTURE.md) and is not
repeated.
**Status:** 2026-10-11. Read from the retail disassembly (`rdis.py` over
`recon/retail_asm/baseline`); Ghidra's decompile of `0x4d7f20`
(`recon/ghidra/cls_0x5a7e38.cpp`) was used as a map only (it drops the stack
argument). Asset facts are from the shipped `imagery.rvi` and the port's
loader. The result was checked against the shipped game in the DOSBox lab
(§6).

## 1. What was wrong

The report (the user, watching the arena): "the attachment points of
weapons are not correct; Locke does not hold his sword". The blade showed
upright behind him, flat beside his head, down his hip, in a different
place nearly every frame, and he wore no shirt or trousers.

Two independent faults:

1. **The object hierarchy was always state 0's.** An I3D object has a parent
   *per state*. The port wired each object's parent once, for state 0
   (`T3DAnimator::SetupObjects`), and the live pose path
   (`UpdateBoneTransforms` → bone `TTransform`s → the map renderer) never
   rewired it. Locke's "sword" hangs off his chest in `walk` (state 0) but
   off his right hand in `combat` and every swing, his left hand in bow
   mode, his right forearm in the thrust chains, and nothing in four
   transitions; its keys in those states are relative to that parent. Under
   the chest the hand-relative keys put the blade anywhere. The same bug
   moves Ogrok's club, a Skeleton's whole body in its death states, Earth
   Golem's head and Locke's thighs in four states (§4).
2. **Nothing drew the equipment.** Retail hides the character's own
   "sword"/"weapon" and draws the equipped items' meshes instead (§3). The
   map renderer submitted only the character's own objects, so Locke showed
   the built-in placeholder blade and no clothes at all.

## 2. Function table

| Retail | Identity | Evidence |
|---|---|---|
| `0x004d7a50` | TCharAnimator::Render (vtables `0x5a7dd8`, `0x5a7e38`, slot `+0x34`) | asm |
| `0x004d7f20` | TCharAnimator::ProcessEquipment(task) — `thiscall(task) ret 4`; task 1 hides, task 2 draws | asm; Render calls it at `0x4d7b82` (1) and `0x4d7c1e` (2) |
| `0x004d82a0` | TCharAnimator::SetMaterialTransparency(imagery) | asm |
| `0x0040e8d0` | T3DAnimator::Render | asm |
| `0x0040ebd0` | T3DAnimator::SetupObjects | asm |
| `0x0040edc0` | T3DAnimator::GetObjectNum(name): `_strlwr(name)` (`0x59bd3e`), then an exact compare with each object's name | asm |
| `0x0040eef0` | T3DAnimator::GetObject(i) (`i < 0x100`) | asm |
| `0x0040a420` | T3DImagery::CalcObjectMatrix(obj, state, frame, pos, calcparents) | asm |
| `0x0040a8f0` | T3DImagery::RenderObject(obj, state, frame, pos, tex, uselastmatrix) | asm |
| `0x00409950` | T3DImagery::GetAniKey (the transition blend) | asm |
| `0x0059a530` / `0x0059a600` | `_stricmp` / `_strnicmp` | asm (ASCII case fold) |
| `0x004dab80` | wildcard name match (`?`, `*`, `[..]`) | asm |

Object layouts used below. S3DAnimObj (`0x34c` bytes): flags `+0`, objnum
`+4`, parent `+8`, animtrack `+0xc`, pos `+0x10`, rot `+0x28`, scl `+0x40`,
matrix `+0x58` (16 floats). T3DAnimator: imagery `+8`, state `+0xc`, frame
`+0x14`, object count `+0x44`, objects `+0x54`, root matrix `+0xac`.
TCharAnimator adds: paperdoll flag `+0x1a4`, drawn alpha `+0x588`, its
equipment draw record (an S3DAnimObj) `+0x594`. Imagery object: parent
table (one `int` per state) `+0x20`. TPlayer: equipment `+0x2a0` (11
slots), root action block `+0xe0` (its action at `+0`), combat flash ticks
`+0x224`.

## 3. Retail

### 3.1 The per-state hierarchy (T3DAnimator)

SetupObjects `0x40ebd0` makes one object per imagery object and, at
`0x40eccc..0x40ed28`, parents each to `objects[objnum]->parent[0]` when
that index is an object of the animator whose objnum matches, else none.

Render `0x40e8d0` does the same at the top of every draw for the
animator's **current** state (`0x40e8f8..0x40e95b`): for each object
without `OBJ3D_PARENT` (`0x800000`), parent = `objects[objnum]->parent[+0xc]`
under the same test. Then, per texture pass, it walks each unrendered
object's parent chain (at most `0x40` deep, `0x40e9c5..0x40e9e1`) and draws
root first; an object with no keys in the state (`+0x44[state] == 0`) gets
`OBJ3D_HIDE` (`0x40ea80`).

CalcObjectMatrix `0x40a420` builds the object's matrix from its keys and,
when it has a parent, recurses (with `calcparents`) and multiplies by the
parent's matrix (`0x40a6d9..0x40a717`). The object matrix at `+0x58` is
therefore character-local and composed through **this state's** parents.

RenderObject `0x40a8f0`: without `OBJ3D_MATRIX` (`0x100`) it calculates the
matrix (`0x40a9b8..0x40a9ec`); an `OBJ3D_HIDE` object returns unrendered
(`0x40aa4b`); the world matrix is the object matrix times the root matrix
argument (`0x43aa90` at `0x40aadf`).

### 3.2 TCharAnimator::Render, the equipment part

After the alpha head (RENDERER_ARCHITECTURE.md):

```
0x4d7b0b  for every object: name == "sword" or "weapon" (_stricmp)
              ? flags |= OBJ3D_HIDE : flags &= ~OBJ3D_HIDE
0x4d7b70  if (inst is a player, class 0xb) ProcessEquipment(1)   // hide what equipment replaces
          shadow; SetMaterialTransparency(body); T3DAnimator::Render
0x4d7bd8  if (!paperdoll && combatflash(+0x224) >= 1)
              blend 0x10 (litadd), T3DAnimator::Render again, blend 4   // the hit flash
0x4d7c11  if (player) ProcessEquipment(2)                         // draw the equipment
0x4d7ce6  root->Is("torch") (0x4dab80), result unused
```

So **every** character hides objects named "sword" or "weapon"; only a
player draws anything in their place. A monster's weapon that is part of
its mesh under another name (Ogrok's "ogrokaxe") draws as an ordinary
object. Unarmed Locke shows no blade, as the opening's reference shot
shows ([../../gameflow/reference/opening/10_rahul_fight.png](../../gameflow/reference/opening/10_rahul_fight.png):
the short sword sits in his inventory, and neither the map nor the
paperdoll shows one).

### 3.3 ProcessEquipment `0x4d7f20`

```
if (inst class != 0xb) return                              // 0x4d7f29
zero the draw record +0x594 (0xd3 dwords)                 // 0x4d7f37
for slot 0..10:                                            // 0x4d7f53, 0x4d819d
    item = player->equip[slot]; if (!item) continue
    if (item->GetImagery() header type != 1 (mesh)) continue   // 0x4d7f68..0x4d7f76
    len = strlen(player->BodyType())                       // vtable +0x300
    for o in 0 .. imagery->NumObjects():                   // 0x4d7fab
        name = imagery->GetObjectName(o)
        hides = true
        if (name[0] == '*' or '@') { hides = name[0] != '@'; name++ }   // 0x4d7fca..0x4d7fe0
        if (!_strnicmp(name, BodyType(), len)) name += len           // 0x4d7ff4..0x4d8000
        if (slot == 4):                                    // prime hand, 0x4d8008
            if (root && root->action == 0x19) continue     // ACTION_BOW: not in bow mode
            bone = GetObjectNum("weapon"), else GetObjectNum("sword"), else by name
        elif (slot == 7):                                  // ranged weapon, 0x4d8050
            if (!root || root->action != 0x19) continue    // only in bow mode
            if (paperdoll +0x1a4) continue
            bone = "weapon", else "sword", else by name
        else bone = GetObjectNum(name)                     // 0x4d80a8
        if (bone < 0) continue
        if (task == 1) { if (hides) GetObject(bone)->flags |= OBJ3D_HIDE }   // 0x4d80c4
        elif (task == 2):                                  // 0x4d80dd
            record.objnum = o; record.matrix = GetObject(bone)->matrix (+0x58, 16 dwords)
            record.flags = OBJ3D_MATRIX
            SetMaterialTransparency(item imagery)          // the character's alpha
            item imagery->RenderObject(&record, 0, 0, &root matrix +0xac, -1, 0)
            if (item class == 6) copy the record to +0x8e0 // a held light source; no reader found
            if (combatflash >= 1) blend 8 (add), draw it again   // 0x4d8145
```

Points worth stating plainly:

- The item's object draws with the character object's matrix verbatim
  (`OBJ3D_MATRIX`): the item's own keys, parents and states play no part,
  and RenderObject is given state 0, frame 0. Every object of the item
  imagery is considered, whatever its state data.
- The body type prefix is dropped whenever the name starts with it. A
  shirt ships one set of objects per body type (`*normalchest`,
  `*largechest`, `*lithechest`, `*dwarfchest`, ...); for a "normal" player
  only the `normal` ones name one of his objects, the rest match nothing.
  An item object whose name names no object of the character (a shirt's
  `ground`) isn't drawn.
- `'*'` and a bare name both hide the character's object; `'@'` draws over
  it. An item object with no faces still hides (Locke's red shirt has
  empty `*normalrarmpad` and `*normallarmpad`: wearing it removes his
  shoulder pads).
- The 1998 snapshot (`charanimator.cpp` before this change) looked for a
  `still`/`all`/body-type *state* in the item, skipped objects without keys
  there and stripped the prefix only when a body-type state existed; it had
  no `'*'`/`'@'` rule, no paperdoll gate on the ranged weapon, no flash pass
  and no light-source copy. Retail replaced all of that with the name rules
  above.

### 3.4 The transition blend, GetAniKey `0x409950`

A state's first five frames blend from the previous state's key
(`(frame + 1) * 0.2`, `0x409b06`), in key space, under the **current**
state's parents — unless any of these holds (in order): interpolation off
(`0x5d7a14`); the state or the previous state has `AF_NOINTERPOLATION`
(`0x40`, state header `+0x30`; `0x4099c7`, `0x4099e8`); the previous state
ended on its last frame on an `end` tag equal to this state's `beg` tag
(`0x409a7c..0x409ad1`); this state is shorter than five frames
(`GetAniLength`, vtable `+0x90`; `0x409afd`).

Locke's attacks, impacts and his draw/sheathe transitions are all
`AF_NOINTERPOLATION` (`chop`, `swing`, `backswing`, `thrust`, the swing and
chop chains, `cimph`..`cimplh`, `walk to combat`, `combat to walk`,
`walk to bow`, `bow to walk`): they snap in.

## 4. The per-state parents in the shipped characters

From each I3D's parent table (state count in brackets):

| Character | Object | Parents over its states |
|---|---|---|
| Locke [352] | `sword` | chest 151, right hand 129, left hand 63, right forearm 3, none 6 |
| Locke | `lthigh`, `rthigh`, `waist` | hips 348, waist / none 4 |
| Ogrok, Pale / Dark Ogrok [73] | `ogrokaxe` | right hand 33, chest 33, none 7 |
| Jong [62] | `ogrokaxe` | right hand 45, chest 13, right forearm 3, left hand 1 |
| Skeleton [36] | every limb, `torso` | its usual parent 30, none 6 |
| Earth Golem [30], Ogrok | `head` | chest 28 / 71, none 2 |
| Ramage [31] | `ogrokaxe` | left hand 28, hips 3 |
| Acolyte [41] | `ogrokaxe` | left hand 38, none 3 |
| Vashar [35] | `ogrokaxe` | right hand 33, none 2 |
| Gatekeeper [13] | `ogrokaxe` | chest 10, none 3 |

Locke's sword by state: `walk` and the walk family, `walk to combat`,
`combat to walk`: chest (sheathed on his back; the draw animation moves it
from there over his head). `combat`, every swing, chop, block and impact:
right hand. Bow states: left hand (the bow rides it, §3.3).
`thrustchain1..3`: right forearm. `walk to bow`, `bow to walk` and four
scripted sequences (`armpinner`, the `ripkilldeath` pair, `atkimpale to
walk`): none.

## 5. The port

**Per-state hierarchy** (`src/3dimage.cpp`):

- `T3DAnimator::ResolveObjectParents(state)` is retail's Render head: each
  object without `OBJ3D_PARENT` takes the imagery's parent for the state,
  and its `TTransform` hangs off that parent's (a root off the instance's
  transform). A parent loop in the data makes the object a root (retail
  bounds its chain walk at 64; a transform loop would recurse forever). It
  also orders the objects parent-first for the matrix pass.
- `SetupObjects` wires state 0, as retail; `UpdateBoneTransforms` rewires
  when the drawn state changes, before it writes the pose, and then
  composes the character-local `S3DAnimObj::matrix` (retail `+0x58`, what
  `GetObjectMatrix` hands attachment and effect callers) in that order, so
  a parent with a higher index than its child is no longer a frame stale.
  An animator whose own SetupObjects never wired from the imagery
  (`parents_state < 0`: helpers, strip effects) keeps its wiring.
- The port interpolates between 24 Hz keys for smooth motion, retail
  doesn't. Across a state boundary (the last frame of `walk to combat`
  toward frame 0 of `combat`) an object whose parent changes, or which has
  keys in only one of the two states, holds its current key
  (`HoldBonesAcrossStates`, `src/i3danimpose.cpp`): keys under different
  parents don't interpolate.
- The transition blend now has retail's conditions
  (`BlendsFromPreviousState`, same file; `T3DImagery::GetAniKey` gains the
  length test): attacks, impacts and the draw/sheathe transitions snap in.
  Before, the port blended five frames into every swing.

**Equipment** (`src/charanimator.*`, `src/maprenderer.cpp`):

- `TCharAnimator::UpdateEquipment` is the hide loop and ProcessEquipment's
  choice, run once per drawn frame from `UpdateDrawState`: it sets the
  hide flags on the character's objects and lists `SEquipAttachment`s
  (item, item object, character object). Data only.
- The map renderer submits. A character's object hidden by its animator
  is not drawn (`SSectorDrawableInst::Submit`, characters only — VFX
  animators keep their own audited hide rules). For a player, after its
  body, `TMapRenderer::Impl::submitEquipment` draws each attachment's
  meshes with the character object's bone transform (world =
  `bone.transform.Matrix()`, the same matrix its own object would draw
  with), the character's alpha, the player's pick id and surface. Worn
  items are in no sector, so their meshes are registered on first draw
  (`equipObjectMeshes`, shared extraction `RegisterI3DObjectMesh`) rather
  than kept in the sector draw list.
- The legacy `TCharAnimator::Render` (24 Hz path) uses the same list.

## 6. Verification

Images in [equipment/](equipment/).

- **Retail against the port, same save, same spot.** The DOSBox lab loaded
  the arena save (`Arena` in its Load Game list: the `Port Resave` slot,
  Locke in the forest with the short sword, the red cloth shirt and the
  brown cloth trousers equipped;
  [retail_arena_walk.png](equipment/retail_arena_walk.png), paperdoll
  included). C put him in combat, S and D swung (`tools/retaillab/retail.py
  key`, `capture`). [retail_vs_port.png](equipment/retail_vs_port.png):
  retail above, the port's arena below (`locke_vs_araknid` and a stance
  run, `--fixedstep`): walking with the sword sheathed, drawing it over the
  shoulder, the combat stance with the blade straight up, a low swing and a
  back swing. The blade is in his hand at the same angles in each, shirt
  and trousers drawn. (The port's forest is lit bluer; lighting is
  LIGHTING_FIDELITY.md's.)
- **Before and after.** [locke_before_after.png](equipment/locke_before_after.png)
  (one tick per frame): before, a bare Locke with the built-in blade
  floating upright behind him, beside his head, off to his left; after, the
  sheathed hilt, the draw, the sword in hand through the swings.
  [ogrok_before_after.png](equipment/ogrok_before_after.png): Ogrok's club
  under the state-0 parent and under each state's.
- **Bow mode** (Light Bow equipped, bow mode entered through a temporary
  console change, see §7): the bow in his left hand, the sword not drawn
  ([port_bow_mode.png](equipment/port_bow_mode.png)).
- **Unarmed:** the opening's reference shot (§3.2).
- The simulation is untouched: the 1440-tick arena trace of
  `locke_vs_araknid` is identical before and after (`7e12165b…`), and
  deterministic (`--repeat 2`). Every combat dojo kata still matches
  (`retail_ab.py`, 27 targets, 25,905 cases, no difference).

## 7. Gaps

- **The hit flash.** Retail draws a struck character again additively for
  `+0x224` ticks (body `litadd`, equipment `add`). The port draws neither
  pass; equipment needs it when the body gets it.
- **Paperdoll.** `src/uiequiptest.cpp` (the equip pane's live body) has its
  own copy of the 1998 matching (state lookup, no `'@'`, prefix only with a
  body-type state). Retail's paperdoll is the same TCharAnimator::Render with
  `+0x1a4` set; the pane should take `TCharAnimator::EquipAttachments` (the
  ranged weapon then needs the paperdoll gate).
- **The light-source copy** at `+0x8e0` has no reader in the retail image;
  not ported.
- **Bow mode by key.** `GAMECMD_BOW` (M) has no handler in the port, and no
  console command enters bow mode; the bow check above made `beginfighting`
  use `ACTION_BOW` for the run. Retail's bow mode wasn't captured (the save
  has no bow).
- **A numeric A/B.** The emulator can run Render's head and
  CalcObjectMatrix (`0x40a420`) on a character I3D for any state and frame;
  a `char-pose` kata comparing those object matrices with the port's would
  pin §5 down to the float. Not built.
