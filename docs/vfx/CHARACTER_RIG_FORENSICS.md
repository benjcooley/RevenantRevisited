# VFX `CharacterRig` — forensics + design

Pre-implementation forensic notes for the `--test=vfx` rig that previews
effects attached to a real character playing real animations. Pulled
together so the rig matches existing engine conventions instead of
inventing parallel infrastructure.

Status: forensics complete, design locked, implementation in progress on
this branch.

Companion reading:

- [AGENT_GUIDE.md §3.0](AGENT_GUIDE.md) — forensics-first rule.
- [AGENT_GUIDE.md §3.1](AGENT_GUIDE.md) — rig taxonomy (this doc adds the
  character/character-attack categories).
- [INVENTORY.md S09](INVENTORY.md) — `TWeaponSwipe` row whose §6 already
  documented the bulk of the `CharacterWithWeaponRig` requirements; this
  doc folds those findings into a reusable primitive.
- [INVENTORY.md M09](INVENTORY.md) — `TTeleporterAnimator` (Misthaven
  recall), the user-flagged downstream consumer this rig has to unblock.

## 1. How `--test=char3d` previews characters today

`--test=char3d` is the existing character previewer. It lives entirely
in `src/testmodes.cpp` as a single-file module, not a `charpreview.{h,cpp}`
split. The vfx rig reuses the same boot path verbatim — same imagery
load, same animator drive — and adds an effect-anchor query on top.

Key references (line numbers in `src/testmodes.cpp`):

- `SCharPreviewState` — :460-480. Holds the loaded `TObjectInstance*`,
  its `T3DImagery*`, a flat `subs[]` list of `(MeshHandle, objnum,
  texslot)` triples, a roster of `(objclass, objtype)` pairs, the
  auto-fit scale, and the `paused` / `last_legacy_tick` controls.
- `SampleCharPreviewPose()` — :482. Calls `SampleI3DAnimPose(img,
  state, frame, prevstate, prevframe)` to get a blended pose for the
  current sim tick.
- `RebuildCharPreviewForRosterIndex(idx)` — :508. The full character-
  load pipeline:
  1. `TObjectClass::GetClass(objclass)` — :526.
  2. Build an `SObjectDef` with `pos = (0,0,0)`, `rotatez = 32` — :529.
  3. `cl->NewObject(&def)` — :542. This is the canonical path; it
     allocates the right TCharacter / TPlayer subclass, attaches
     imagery, and runs the ctor RULES.DEF parse.
  4. If `IsCharacter()`, call `DefaultRootState()` and `SetState(root)`
     — :545-553.
  5. `inst->OnScreen()` — :554. Triggers animator creation.
  6. Cache `T3DImagery*` from `inst->GetImagery()` — :557.
  7. Walk every visible (objnum, texslot) and register a sokol mesh
     via `Renderer->RegisterMesh()` — :571-665. Populates `subs[]` +
     bounding-box.
  8. Auto-fit scale = `280.0f / max(bbox extent)` — :670-674.
- `InitializeCharPreviewMode()` — :682. Builds the roster from
  `CharacterClass` + `PlayerClass` types; honours `StartupAssetPath`
  to pick a specific character at boot.
- `RenderCharPreviewMode()` — :712. Per-frame:
  - **Tick gate at 24 Hz** via `TTime::LegacyFrameCount()` — :717. On
    a new legacy tick advance the instance with
    `NextFrame()` + `OnScreen()` (lazy animator) + `Animate(false)`.
  - Apply directional + ambient lighting, fixed `kCam=2750.0` orthoish
    camera, gray clear — :734-753.
  - For each `sub` build the world matrix from the current
    `SAnimPose` via `BuildAnimPoseObjectMatrix(img, pose, state,
    sub.objnum, w)`, scale, recenter on bbox, apply spin rotation,
    `Renderer->SubmitMesh(m)` — :766-784.
- Input — `HandleMouseClick("char3d", ...)` at :2506-2537:
  - Left mouse — cycle anim state (`SetState(state+1 % NumStates())`).
  - Right mouse — cycle roster character.
  - Middle mouse — toggle pause.

Takeaways for the rig:

- The character-load pipeline (`cl->NewObject(def)` → `SetState(root)` →
  `OnScreen()`) is **already factored to a few lines**. The rig will
  inline the same calls; refactoring char3d to share is more friction
  than copy-paste at this volume of code.
- Sokol mesh extraction (`ExtractSubMeshTextureSlot` + bbox walk +
  `Renderer->RegisterMesh()` per visible objnum/texslot) is the
  reusable bit. The rig calls the same helpers.
- The 24 Hz tick advance pattern (`legacy_tick != last` →
  `NextFrame() + Animate(false)`) is straight-copy; same gate the rest
  of the engine uses for animation playback.
- char3d uses `inst->SetState(int)` to cycle through every animation
  state in the imagery; the rig adds animation-state cycling via the
  same call so user can pick "attack1" / "combat" / "walk" etc.

## 2. Character attachment / marker points

Revenant **doesn't** use Win32-skeletal anim — it uses an `I3D` mesh
with named sub-objects, where each sub-object has its own bone-style
matrix that gets composed up through a parent chain every frame. The
"marker points" effects attach to are these named sub-objects.

The API surface that exposes them lives on `T3DAnimator` (the per-
instance animator class held as a component on a TObjectInstance):

- `T3DImagery::GetObjectNum(name)` — `src/3dimage.cpp:945`. Returns
  the integer sub-object index, or -1 if no sub-object has that name.
- `T3DAnimator::GetObjectNum(name)` — `src/3dimage.cpp:2819`. Same
  lookup, ranging over the live animobj array.
- `T3DAnimator::GetObjectMatrix(objnum, hmm_mat4* out)` —
  `src/3dimage.cpp:2853`. Per-frame: triggers `UpdateBoneTransforms()`
  then returns the cached `animobj->matrix`.
- `T3DAnimator::GetObjectPos(objnum, hmm_vec3& out, hmm_vec3* in=null)`
  — `src/3dimage.cpp:2871`. Wrap the matrix to transform a local-space
  point into the animator's frame (default: the origin = the bone
  pivot).
- `T3DAnimator::GetObjectMapPos(name, S3DPoint& out)` —
  `src/3dimage.cpp:2905`. Convenience: returns the bone pivot as an
  integer `S3DPoint` with `REV_FIX_Z_VALUE` applied (the live world Z
  the engine uses everywhere else).

`animobj->matrix` semantics: the matrix is **already chained up through
the parent hierarchy** by `T3DAnimator::Render()`
(`MtxMultiply(&m, &m, &animobj->parent->matrix)`) every frame, and the
character's instance-level `TTransform` is folded in via
`inst->Transform().RefreshHierarchy()` inside `UpdateBoneTransforms()`
— see `src/3dimage.cpp:2520-2528`. So a single `GetObjectMatrix` call
yields the **world-space** transform of the sub-object for the current
animation frame. Live consumers `src/spell.cpp:399` (cast-from-rhand)
and `src/effect2.cpp:1457` (effect-from-`warrior_he`) confirm this
contract.

The conventional sub-object names baked into character meshes (grepped
across live code):

| Name           | Use                                                 | Citation                     |
|----------------|-----------------------------------------------------|------------------------------|
| `"rhand"`      | Right hand — spell cast origin                      | spell.cpp:399                |
| `"lhand"`      | Left hand — TStripEffect anchor (see S01)           | stripeffect.cpp:486          |
| `"head"`       | Head — used by `RenderCombatFlashes` (off currently) | charanimator.cpp:570         |
| `"weapon"`     | Weapon-in-hand sub-object (primary lookup)          | charanimator.cpp:264         |
| `"sword"`      | Weapon-in-hand sub-object (fallback)                | charanimator.cpp:266         |
| `"ogrokaxe"`   | Weapon-in-hand sub-object (Ogrok variant fallback)  | weapontrail.cpp:615          |
| `"warrior_he"` | Head, warrior variant                               | effect2.cpp:1457             |

There are likely more (per-monster torso / feet / wings / tail bones);
the rig probe enumerates all named sub-objects on the loaded character
mesh and lets the user pick from the live list rather than hardcoding.

**Weapon tip vs weapon hilt:** the matrix returned for the `"weapon"`
sub-object is the hilt pivot. The tip is the extreme +Z vertex of the
weapon-object's mesh — see `TWeaponSwipe::GetWeaponExtents()` in the
`#if 0` body at `src/weapontrail.cpp:557` for the canonical pattern
(walk the weapon-object's verts, take min/max Z, those are hilt/tip
local-space points; transform by `weaponobj->matrix`). For the
preview rig, "weapon tip" = hilt-matrix * (0, 0, +tipZ_local). The rig
caches the local-tip-Z once at imagery-load time.

## 3. Weapon equipping in-engine

Two paths exist:

### 3a. Weapon-as-sub-object (the common path)

Most character meshes include their weapon as a **named sub-object of
the character mesh itself** — the "weapon" or "sword" entry in the
table above. The animation moves the weapon as a bone-attached child
of the hand bone; no separate instance, no equip step.

This is what every retail combat-effect lookup uses
(`charanimator.cpp:264`, `weapontrail.cpp:609`). For the preview rig
the implication is **you don't need to equip anything** — load a
character whose mesh ships with a "weapon" sub-object and it draws
its weapon out of the box.

### 3b. Equip-a-TObjectInstance (TPlayer-only path)

`TPlayer` supports real equip slots (EQ_PRIMEHAND, EQ_BODY, …) and
`Equip(inst, slot)` mounts an external imagery into the slot — the
swap re-resolves the combat root animation set so swinging a "Short
Sword" plays sword-anim states. See `src/playscreen.cpp:283-339` for
the live starter-loadout call sequence:

1. `cl->NewObject(&def)` for the weapon (where `cl` is found via
   `TObjectClass::FindObjType("Short Sword")`).
2. `p->AddToInventory(inst)`.
3. `p->Equip(inst, EQ_PRIMEHAND)`.

The downside: this path drags in `PlayerManager`, inventory persistence,
and the full RULES.DEF combat-root reshuffle. For a read-only preview
rig that's a lot of dependency surface.

**Decision for the rig:** default to path 3a. The rig loads a character
mesh by name; if the mesh has a `weapon`/`sword`/`ogrokaxe` sub-object
that exposes a weapon attachment, the rig presents it as a valid anchor.
The user toggles between characters via the existing roster cycle; if
the user wants to validate an effect against an equipped-weapon TPlayer
that's a future extension (carve-out, not v1).

## 4. 24 Hz sim tick driving animation

The engine ticks animation on a fixed 24 Hz sim clock, exposed via
`TTime::LegacyFrameCount()` (`src/time.h:41`). Every consumer that
needs to gate work on a sim-tick boundary follows the same pattern:

```cpp
const int64_t legacy_tick = TTime::LegacyFrameCount();
if (legacy_tick != last_tick) {
    last_tick = legacy_tick;
    // ... sim-rate work ...
}
```

This is exactly what `--test=char3d` does at `testmodes.cpp:717-726`
and what existing effects do for animation cadence (e.g.
`TFireEffect::TickAndSubmitForTest`). The rig inherits the same gate;
animation state advance via `inst->NextFrame() + inst->Animate(false)`
is sim-rate, but the per-frame attachment-matrix sample is render-rate
(matches retail spell.cpp / effect2.cpp callers, which run inside the
per-instance render loop).

## 5. Rig features the deferred effects need

Walked `INVENTORY.md` for rows whose status reads as rig-dependent
(explicit "Rig category" / "needs CharacterRig" / "character-attached"
mentions, plus the rows whose effect type implies an anchor on a
character — auras, burn overlays, healing visuals, projectiles whose
launch point is a hand, etc.).

| ID  | Effect                  | Anchor needed                    | Rig style                  |
|-----|-------------------------|----------------------------------|----------------------------|
| M01 | `TAuraEffect`           | Character body (all-over glow)   | char-attached, idle anim   |
| M02 | `TAuraAnimator`         | (bridge to M01)                  | char-attached              |
| M03 | `THealEffect`           | Character body + cast-hand spawn | char-attached, cast anim   |
| M04 | `TBurnEffect`           | Character body (status overlay)  | char-attached, hurt anim   |
| M06 | `TSpellBlock`           | Character body (shield bubble)   | char-attached, idle anim   |
| M07 | `TPhotonEffect`         | Cast hand → ground impact        | spell-cast (caster→ground) |
| M09 | `TTeleporterAnimator`   | Caster feet / center (Misthaven) | spell-cast (caster→ground) |
| S09 | `TWeaponSwipe`          | Weapon hilt + weapon tip (arc)   | char+weapon+attack-anim    |
| X09 | `TShieldAnimator`       | Character body (bubble)          | char-attached, idle anim   |
| X10 | `TSymGlowAnimator`      | Character body / symbol prop     | char-attached or static    |
| X11 | `TRibbonAnimator`       | Weapon-tip-style trailing ribbon | weapon-attached            |
| X18 | `TArrowEffect`          | Bow hand (launch), target (hit)  | projectile + char hand     |
| L03 | `TBallAnimator`         | Cast hand → flight path          | spell-cast (caster→target) |

(Plus M02 / M05 / B04 status overlays per their `VO`/`PE+IM` pipeline
flags, when those land — `VO` is deferred so they aren't on the
critical path yet.)

Distilling the anchor demands: **head**, **chest** (whole-body center),
**right hand** (cast-from-hand spell origin), **left hand** (S01 strip
anchor reference), **weapon hilt** (sword-slash hilt anchor), **weapon
tip** (sword-slash tip anchor, ribbon anchor), **feet** (teleport
ground circle, status floor halos), **whole-character position +
facing** (auras, burns, shield bubble).

That collapses to two anchor primitives in the rig API:

1. `Bone(name)` — read `T3DAnimator::GetObjectMatrix(GetObjectNum(name))`.
   Covers head / rhand / lhand / weapon-hilt / chest-bone-if-named.
2. `BoneLocalPoint(name, local_offset)` — same but transform a fixed
   local-space offset through the matrix. Covers weapon-tip (offset =
   (0, 0, +tipZ_local) cached from weapon mesh extents) and any
   "n cm above head" floating overlays.

Plus two whole-character anchors that don't need a named bone:

3. `CharacterRoot()` — `inst->GetPos()` + facing from
   `inst->GetRotate()`. Covers feet halos, ground circles, the
   teleport pulse.
4. `CharacterBoundsCenter()` — root + (0, 0, half_bbox_height).
   Covers aura center, shield bubble, burn overlay.

## 6. Design constraints for M09 (Misthaven teleport)

M09 `TTeleporterAnimator` is the user-flagged downstream consumer.
INVENTORY tags its rig category as **Spell-cast (caster→ground)** —
caster character plays a cast animation; ground impact point
(usually under the caster's feet for self-teleport recall) emits a
visual that plays out to completion.

No retail recon body exists yet (INVENTORY: `recon file: not-extracted`)
so the rig requirements are derived from the registry name and
pre-release intent rather than disassembly. What we know:

- Triggered via `SPELL "Teleport"` in `data/Resources/spell.def`.
- Registered through `REGISTER_3DANIMATOR("Teleporter", ...)` —
  follows the standard animator-builder pattern (see F02 / F08 entries
  in `effect.cpp` for the registry mechanism).
- Misthaven recall variant is self-cast, so caster + ground point
  collapse to the same XY; Z is feet-of-caster.

**Required rig features to support M09 once it's ported:**

1. **Character with cast animation playing.** Use the rig's anim-
   state cycle to pick a state matching `cast*` / `magic*` / `invoke*`
   on the character imagery (varies by character — Locke has spell
   anims; not every NPC does).
2. **Caster-feet anchor.** `CharacterRoot()` gives the world XY; Z is
   `inst->GetPos().z` (engine already snaps characters to walkmap).
3. **Cast-hand anchor for the spawn flourish.** `Bone("rhand")` per
   the engine-wide spell-cast convention (spell.cpp:399). M09's
   ground burst typically grows out of a brief hand flourish.
4. **No mid-effect teardown.** Teleport plays through its full
   animation; the rig's per-effect retrigger timer needs a mode that
   says "let this finish, then re-fire" rather than the existing
   Combat / SpellGround styles that re-trigger on a fixed cadence.
   Add `CharacterAnim` preview style with an animation-driven
   re-trigger.
5. **Camera that frames feet → head.** Existing harness camera (kCam
   = 2750, character ~150 wu tall) frames the body already; the
   teleport ring is feet-anchored ≤ 200 wu radius so it fits without
   adjustment.

These all fall out of the design in §7 below — M09's port author will
register an effect with `preview_style = CharacterRig` (or new
`CharacterSpellCast`) and the rig handles the rest.

---

# Design

The rig is one logical primitive — `VfxTest::CharacterRig` — with four
preview-style modes that select which interactions are exposed:

| `EVfxPreviewStyle` value | What it does                                                      |
|--------------------------|-------------------------------------------------------------------|
| `CharacterIdle`          | Load character + play default idle state on loop. Anchors: any bone, root, bounds-center. Status overlays (M04 burn / M01 aura) attach here. |
| `CharacterAttack`        | As above, but state defaults to `attack1` / first state matching `attack*` (cycled with key). Used by S09 swipe. |
| `CharacterCast`          | As above, but state defaults to first `cast*` / `magic*` / `invoke*` match. Used by M07 photon / M09 teleport. |

(The non-character styles — Static, Combat, SpellGround, Projectile —
stay as they are; they're not character-attached and don't need the
rig.)

Combining `CharacterAttack` + `CharacterCast` into a single
`CharacterAnim` style with a per-effect anim-name hint is the more
compact alternative; rejected because it forces every effect to spell
the anim name which couples the effect to a specific character's
RULES.DEF naming. Keeping the three explicit modes lets the rig pick
sensible defaults per character (first match for the role) and the user
still cycles freely between every available state via the existing key.

## API surface in `vfxtest.{h,cpp}`

Two new types — both header-only, declared in `vfxtest.h`:

```cpp
// What world-space point + orientation does an effect attach to.
// Computed every frame from the live character animator. Returned by
// CharacterRig::ResolveAnchor() into the effect's per-frame submit
// callback.
struct SVfxAttachment
{
    float world_pos[3]    = {0, 0, 0};
    float world_basis[9]  = {1,0,0, 0,1,0, 0,0,1};  // 3x3 row-major (right, up, fwd)
    bool  resolved        = false;   // false = bone missing / character not loaded
};

// What an effect declares it needs from the rig. Effects register one
// of these alongside their SEffect entry; the rig resolves it to an
// SVfxAttachment each frame before calling the effect's submit hook.
struct SVfxAnchor
{
    enum class EKind : uint8_t {
        CharacterRoot,        // inst->GetPos() + facing from inst rotate
        CharacterBoundsCenter,// root + (0, 0, bbox_height_half)
        Bone,                 // T3DAnimator::GetObjectMatrix(bone_name)
        BoneLocalPoint,       // Bone() then transform local_offset
    };
    EKind   kind   = EKind::CharacterRoot;
    char    bone_name[24] = "";       // for Bone / BoneLocalPoint
    float   local_offset[3] = {0,0,0};// for BoneLocalPoint
};
```

`SEffect` gains an optional `anchor` field; the rig fills out
`SVfxAttachment` and passes it to a new submit signature:

```cpp
struct SEffect {
    // ... existing fields ...

    // Character-attached effects set this; the rig resolves it to an
    // SVfxAttachment and calls submit_attached() instead of submit().
    // Only one of the two callbacks should be set per effect.
    SVfxAnchor                                       anchor;
    std::function<void(void* ctx, EFxDebugMode dbg,
                       const SVfxAttachment&)>       submit_attached;
};
```

The rig:

1. When the active effect's preview-style is in {`CharacterIdle`,
   `CharacterAttack`, `CharacterCast`} the rig owns a long-lived
   `TObjectInstance` (the rig character) and ticks it every sim-tick
   exactly like char3d.
2. Each render frame the rig resolves `effect.anchor` to an
   `SVfxAttachment` (live bone matrix or character root) and passes
   it to `submit_attached`.
3. The rig's spawn callback for an attached effect passes the **anchor
   point** as the origin (so the effect's factory still receives a
   world-space spawn position).

The rig owns one character at a time. Cycling characters tears down
the current one and spawns a new one — same pattern as the F01/B01
effect-respawn flow. Anchor resolution gracefully fails (returns
`resolved=false`) when the requested bone doesn't exist on the loaded
character; the rig logs once and the effect is responsible for
deciding whether to draw nothing or fall back to root.

## UI / interaction

ImGui browser panel gets a "Character" sub-section visible when the
selected effect's preview-style is character-attached:

```
[ Character: Locke ▼ ]   < prev   next >
[ Anim state:   attack1 (3/14) ▼ ]  < prev   next >
Anchor: rhand (resolved at world (...))
[ ✔ Pause anim ]   [ Step one tick ]
```

Reused from char3d:

- The character roster + cycle logic (`PlayerClass.NumTypes()` +
  `CharacterClass.NumTypes()` walk). The rig copies that bootstrap.
- The 24 Hz tick gate.
- The per-state cycle (Left/Right on the anim-state combo).
- Mesh extraction (`ExtractSubMesh*`).

Reimplemented:

- The lifecycle. char3d's `g_charPreview` is module-global and
  presumes the test-mode owns the entire screen; the rig is a
  sub-component of vfxtest, swapped in/out as effects are cycled.
- The submit path. char3d submits the character mesh as part of its
  scene; the rig submits it as a background pass so the effect
  always reads against a live character.

Not reused / left alone:

- char3d's ImGui (none currently — char3d uses mouse + keyboard only).
  The rig's controls live in the existing VFX browser ImGui panel.
- char3d's roster bootstrap (`InitializeCharPreviewMode`). The rig
  has its own initializer because it needs a fixed "default rig
  character" choice, not a roster walk.

## Anchor frame source

`SVfxAttachment::world_basis` is a 3×3 row-major basis where row 0
is "right", row 1 is "up", row 2 is "forward" — same convention the
renderer uses for billboard orientation. For bone-attached anchors
the basis is the rotation block of the bone matrix (which already
includes the character's instance-level rotation per §2). For
`CharacterRoot` it's the identity rotated by `inst->GetRotate().z` —
read from `TObjectInstance::GetFace()`.

The basis is what an effect like a sword-trail needs to know which
way the weapon is currently pointing; bare-position effects like a
ground halo can ignore it.

---

# Implementation notes

Build target: `src/vfxtest.{h,cpp}` only. No new files needed — the
rig is small enough to live alongside the existing browser code.
The character-mesh background-render path can be a static helper
inside `vfxtest.cpp`.

Field initializers, `[[nodiscard]]` on accessors, const-correct
member functions; no raw `fprintf`, use `log_info/warn/error`.

The proof attachment (Phase 4 of the task brief) will be **H04
TDripEffect attached to a character's right hand** — the drip is
already in the registry as a Static-preview entry, and re-binding
it as a `CharacterIdle` rhand anchor exercises the new code path
without porting anything new. If the H03 ripple chain still works
after the rebind, that confirms the rig delivers anchors usable by
real effects.

Alternative considered + rejected for proof attachment: F01 TFlame
on rhand. Rejected because the flame's flipbook math currently bakes
the world position into its component on spawn; re-anchoring it
each frame needs a small change to TFlipbookBillboardComponent's
Submit signature that's out of scope for this task ("don't touch
effects beyond the 1 proof attachment"). H04's TickAndSubmitForTest
already takes an effect-driven origin every tick, so re-anchoring is
a one-call change.
