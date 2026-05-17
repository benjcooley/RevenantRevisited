// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           vfxtest.h - --test=vfx browser harness                      *
// *************************************************************************
//
// The --test=vfx mode boots a single-effect preview that exercises the
// Phase 1 FX submission API (see docs/vfx/PHASE1_SPINE.md and
// docs/vfx/TEST_MODE.md).
//
// Effects register themselves with this module at startup using
// VfxTest::Register; the browser builds a sorted catalogue and lets the
// user step through them with Left/Right arrows. Each entry is a tiny
// data record + factory + per-frame submit hook. The harness owns the
// active effect's lifecycle (spawn / destroy / restart).
//
// Phase 2 (2026-05-17): added the CharacterRig primitive — a long-lived
// TObjectInstance + animator driving an attachment point each frame.
// Effects can declare an SVfxAnchor (bone name / character-root / bounds-
// center) and receive a resolved SVfxAttachment in their submit hook.
// See docs/vfx/CHARACTER_RIG_FORENSICS.md for the design rationale.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "renderer.h"
#include "revtypes.h"     // S3DPoint

class TObjectInstance;

namespace VfxTest {

// Preview style controls how the harness drives the effect over time --
// real game spawn patterns aren't a single continuous emitter. Combat
// effects accent strikes at varied screen positions; spell effects play
// once at the ground plane; ambient effects (torch) persist at a fixed
// spot. The factory is called each time the harness wants to (re)spawn
// the effect; the harness passes the world position to use.
//
// CharacterIdle / CharacterAttack / CharacterCast (added 2026-05-17)
// spawn a long-lived character mesh + animator and route the effect's
// spawn origin through an SVfxAnchor lookup against the live animator.
// The effect receives the anchor world position each frame via the
// submit_attached callback (instead of submit), and its factory is
// called once at boot with the initial anchor — the moving anchor is
// re-applied on the owning instance every frame, so any effect whose
// emit logic reads inst->Pos() tracks the bone automatically.
enum class EVfxPreviewStyle : uint8_t
{
    Static,            // spawn once at origin, never re-trigger (torch, ambient)
    Combat,            // re-spawn every ~0.6s at a random spot in the visible area
                       // (hit flash, blood splat, swing arc) -- mimics combat cadence
    SpellGround,       // re-spawn every ~3s at the ground plane (cast/teleport burst)
    Projectile,        // re-spawn every ~1.2s travelling from a source point along a vector
    CharacterIdle,     // attached to a character playing its default idle / root state
    CharacterAttack,   // attached to a character playing an attack* state on loop
    CharacterCast,     // attached to a character playing a cast* / magic* / invoke* state on loop
};

// Resolved world-space frame for a character attachment. Filled in by
// the rig each frame before calling the effect's submit_attached
// callback.
//
//   world_pos    — world-space point (e.g. live position of the rhand
//                  bone for an Anchor::Kind::Bone("rhand")).
//   world_basis  — 3x3 row-major rotation; row 0 = "right",
//                  row 1 = "up", row 2 = "forward". For bones this is
//                  the rotation block of the bone matrix; for root /
//                  bounds-center anchors it's the character's facing.
//   resolved     — false if the bone wasn't found on the loaded
//                  character (effect can fall back to its baked-in
//                  spawn position or skip its submit).
struct SVfxAttachment
{
    float world_pos[3]   = {0, 0, 0};
    float world_basis[9] = {1,0,0, 0,1,0, 0,0,1};
    bool  resolved       = false;
};

// Declarative description of what part of the rig character this
// effect anchors to. Effects fill this in alongside their SEffect row.
// The rig resolves it to an SVfxAttachment every render frame.
struct SVfxAnchor
{
    enum class EKind : uint8_t
    {
        CharacterRoot,         // inst->GetPos() + facing from rotatez
        CharacterBoundsCenter, // root + (0, 0, half_bbox_height) — body-center auras / shield bubble
        Bone,                  // T3DAnimator::GetObjectMatrix(GetObjectNum(bone_name))
        BoneLocalPoint,        // Bone() then transform local_offset through the bone matrix
    };

    EKind   kind          = EKind::CharacterRoot;
    char    bone_name[24] = "";              // for Bone / BoneLocalPoint
    float   local_offset[3] = {0, 0, 0};     // for BoneLocalPoint
};

// Per-effect registration record. `factory` is called to (re)spawn the
// effect at a harness-provided origin; it returns a context pointer the
// harness passes back into `submit` and `destroy`. `submit` is called
// once per frame while the effect is selected and the harness is
// unpaused.
//
// Character-attached effects (preview_style in {CharacterIdle,
// CharacterAttack, CharacterCast}) set `anchor` and `submit_attached`
// instead of `submit`. The rig resolves the anchor each frame and
// passes the world position into the submit hook; the rig also keeps
// the effect's owning instance position synced via ForcePos() so
// in-game-style emit-from-Pos() effects track the bone naturally.
struct SEffect
{
    std::string id;
    std::string family;
    std::string pipeline;   // "FB" / "PE" / "SR" / "LS" / "MP" / "VO" / "CX"
    EVfxPreviewStyle preview_style = EVfxPreviewStyle::Static;
    SVfxAnchor       anchor        = {};
    std::function<void*(const S3DPoint&)>                  factory;
    std::function<void(void* ctx, EFxDebugMode dbg)>       submit;
    std::function<void(void* ctx, EFxDebugMode dbg,
                       const SVfxAttachment& attach)>      submit_attached;
    // Called between BeginTilePass + EndTilePass so the effect can
    // submit mesh draws (SubmitMesh / SubmitHelperMesh) that need to
    // ride the tile/lighting/transparent-world pipeline. The harness
    // calls submit/submit_attached FIRST (for fx-queue billboards +
    // ticking), then BeginTilePass, then submit_world, then EndTilePass.
    // Effects that don't draw meshes leave this null. Per
    // M09_FORENSICS.md §M09b: required for I3D-mesh effects, since
    // BeginTilePass clears the transparent_world_queue which holds
    // pending SubmitHelperMesh entries.
    std::function<void(void* ctx, EFxDebugMode dbg)>       submit_world;
    std::function<void(void* ctx)>                         destroy;
};

void Register(const SEffect& effect);

bool Initialize();
void Close();
void Render();
void HandleKeyPress(int32_t key, bool down);

// The map renderer's flame builder spawns a TFlameEffect when the
// owning T3DImagery initializes. The harness needs to defer registration
// until after the renderer is alive (TFlameEffect uses real GPU
// textures). Callers register here at process startup; we replay the
// registrations into VfxTest::Register the first time the browser
// initializes.
void DeferredRegister(const SEffect& effect);

}  // namespace VfxTest
