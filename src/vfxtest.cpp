// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           vfxtest.cpp - --test=vfx browser harness                    *
// *************************************************************************
//
// Single-effect preview harness for the Phase 1 VFX restoration. See
// docs/vfx/TEST_MODE.md for the user-facing spec and
// docs/vfx/PHASE1_SPINE.md for the renderer API the registered effects
// submit into.
//
// Lifecycle:
//   --test=vfx -> TestModes::Initialize("vfx") -> VfxTest::Initialize
//   VfxTest::Initialize replays deferred registrations and spawns the
//   first effect's context. Render() runs the per-frame submit hook of
//   the active effect inside a minimal lit scene (ground tile + sun).
//   Key handling cycles effects, restarts, pauses, and steps debug mode.
//
// *************************************************************************

#include "vfxtest.h"

#include "3dimage.h"     // T3DImagery / T3DAnimator (rig attachment lookup)
#include "character.h"   // TCharacter::DefaultRootState (rig boot anim pick)
#include "display.h"
#include "effect.h"
#include "i3danimpose.h" // SampleI3DAnimPose (rig render)
#include "imagery.h"     // TObjectClass / TObjectImagery
#include "imgui.h"
#include "logging.h"
#include "mappane.h"     // MapPane.MakeIndex (rig sector-less spawn)
#include "meshextract.h" // BuildAnimPoseObjectMatrix + Extract* (rig render)
#include "object.h"
#include "particlefx.h"
#include "renderer.h"
#include "revenant.h"     // VK_LEFT, VK_RIGHT, VK_SPACE
#include "stripeffect.h"  // TStripEffect (S01 SR-pipeline port)
#include "weaponswipe.h"  // TWeaponSwipe (S09 SR-pipeline port)
#include "surface.h"
#include "testconfig.h"  // StartupVfxId, StartupVfxHideUi
#include "time.h"

#include "stb_image.h"   // diagnostic-backdrop PNG load (forest/dungeon)

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <strings.h>   // strcasecmp
#include <vector>

#ifndef REV_VFX_TEST_BG_DIR
#  define REV_VFX_TEST_BG_DIR "tools/vfx/test_backgrounds"
#endif

extern TObjectClass CharacterClass;
// PlayerClass already extern'd in revenant.h.

namespace {

// =========================================================================
//   CharacterRig — long-lived character + animator for attached effects
// =========================================================================
//
// Lives inside vfxtest's SState. Spawned the first time the user selects
// an effect whose preview_style is in
// {CharacterIdle, CharacterAttack, CharacterCast}; recycled across
// character-attached effects (no rebuild per effect), torn down when the
// user cycles to a non-character effect.
//
// Storage model mirrors char3d's SCharPreviewState (testmodes.cpp:460):
// the instance + sub-mesh list + bbox + auto-fit scale. Differences:
//   * char3d is the whole test mode; the rig is a sub-component swapped
//     in/out as the active effect changes.
//   * char3d submits character meshes as the only scene content; the
//     rig submits them as a background pass and lets the effect draw
//     in the same lighting frame.
//   * char3d cycles characters from the user roster; the rig defaults
//     to "Locke" (PlayerClass, picked first; falls back to CharacterClass
//     first valid type) and exposes UI cycling for both roster + state.
//
// See docs/vfx/CHARACTER_RIG_FORENSICS.md for the design rationale.
struct SRigSubMesh
{
    MeshHandle handle  = 0;
    int32_t    objnum  = -1;
    int32_t    texslot = -1;
};

struct SCharacterRig
{
    // Roster of (objclass, objtype). Boot-built from CharacterClass +
    // PlayerClass; same scan as InitializeCharPreviewMode.
    std::vector<std::pair<int32_t, int32_t>> roster;
    int32_t roster_idx = 0;

    // Currently-loaded character.
    TObjectInstance* inst   = nullptr;
    T3DImagery*      img    = nullptr;
    std::vector<SRigSubMesh> subs;

    // For the background mesh render — auto-fit to ~280 wu tall (same
    // value char3d uses; gives consistent visual size for any character).
    TTextureHandle fallback_albedo = kInvalidTexture;
    float bbox_min[3] = {0, 0, 0};
    float bbox_max[3] = {0, 0, 0};
    float scale       = 1.0f;

    // Sim-tick gate, mirrored from char3d (24 Hz, gated on
    // TTime::LegacyFrameCount()).
    int64_t last_legacy_tick = -1;
    bool    paused_anim      = false;

    // Per-character "what bones does this mesh expose" cache. Built at
    // load-time so the UI can list real options instead of guessing
    // from the conventional name table. Each entry is the name string
    // (from T3DImagery::GetObjectName), index, and a render-frame
    // sample of "is this bone non-degenerate" so the UI can grey out
    // sub-objects that resolve to the origin (typical when the bone
    // isn't authored on this character).
    struct SBoneEntry { std::string name; int32_t objnum; bool valid; };
    std::vector<SBoneEntry> bones;
};

struct SState
{
    std::vector<VfxTest::SEffect> deferred;
    std::vector<VfxTest::SEffect> catalogue;
    int32_t      active_idx       = -1;
    void*        active_ctx       = nullptr;
    EFxDebugMode debug_mode       = EFxDebugMode::Normal;
    bool         paused           = false;
    bool         step_once        = false;
    int64_t      frames_rendered  = 0;
    bool         initialized      = false;
    bool         first_submit_logged = false;
    // Retrigger pattern state -- driven by the active effect's
    // EVfxPreviewStyle. Resets on SelectIndex / Restart.
    float        retrigger_timer  = 0.0f;
    // Character rig (only valid when the active effect's preview style
    // is character-attached; otherwise inst==nullptr and the rig is
    // dormant).
    SCharacterRig rig;
    // Cached attachment for the current frame, computed by the rig
    // before calling the active effect's submit_attached hook. Effects
    // that read inst->Pos() get the moving anchor "for free" because
    // the rig also ForcePos()es the effect's owner instance to the same
    // value (see TickCharacterRig).
    VfxTest::SVfxAttachment current_attach;

    // Diagnostic backdrop the user cycles via `B` / the rig sub-panel.
    // Forest / Dungeon textures are loaded once at Initialize from
    // tools/vfx/test_backgrounds/ and released in Close. kInvalidTexture
    // simply means "the PNG didn't load" -- the harness falls back to a
    // solid backdrop in that case.
    VfxTest::EVfxBackground background       = VfxTest::EVfxBackground::LtGray;
    TTextureHandle          forest_texture   = kInvalidTexture;
    TTextureHandle          dungeon_texture  = kInvalidTexture;
};

SState g_state;

// Was the active effect's preview style character-attached? Helps the
// dispatch glue swap between submit / submit_attached and decide
// whether to render the rig background pass.
constexpr bool IsCharacterAttached(VfxTest::EVfxPreviewStyle style)
{
    return style == VfxTest::EVfxPreviewStyle::CharacterIdle    ||
           style == VfxTest::EVfxPreviewStyle::CharacterAttack  ||
           style == VfxTest::EVfxPreviewStyle::CharacterCast;
}

// Per-style retrigger cadence + position policy. Static = never re-fires.
// =========================================================================
//   CharacterRig helpers
// =========================================================================

// Pick the default anim state to play for a given style. CharacterIdle
// uses DefaultRootState (TCharacter's "walk"/"combat"/"sleep"/"dead"
// per-character default); CharacterAttack/CharacterCast scan the
// imagery's state names for the first attack* / cast*|magic*|invoke*
// match. Returns -1 if no match — caller leaves the instance on
// whatever state SetState(DefaultRootState) put it in.
int32_t PickInitialState(TObjectInstance* inst, VfxTest::EVfxPreviewStyle style)
{
    if (!inst || !inst->GetImagery()) return -1;
    TObjectImagery* img = inst->GetImagery();
    auto starts_with_ci = [](const char* s, const char* prefix) {
        if (!s) return false;
        for (size_t i = 0;; ++i) {
            const char p = prefix[i];
            if (!p) return true;
            const char c = s[i];
            const char pl = (p >= 'A' && p <= 'Z') ? char(p - 'A' + 'a') : p;
            const char cl = (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c;
            if (cl != pl) return false;
        }
    };
    const int32_t n = inst->NumStates();

    // Two-pass scan for CharacterAttack so windup-only poses
    // (`backswing`) don't out-prioritize the actual in-motion swing
    // states that come after them in the state table. Pass 1 = names
    // that exhibit live weapon arc motion; pass 2 = wider net
    // including windup / pose-only `attack*` shorthands.
    if (style == VfxTest::EVfxPreviewStyle::CharacterAttack)
    {
        // Pass 1 — in-motion swings.
        for (int32_t i = 0; i < n; ++i)
        {
            auto* s = img->GetState(i);
            if (!s || !s->animname[0]) continue;
            const char* nm = s->animname;
            // Exclude `backswing` explicitly from this pass — it
            // starts with "swing"-adjacent chars but lacks blade
            // motion.
            if (starts_with_ci(nm, "backswing")) continue;
            if (starts_with_ci(nm, "swing")     ||
                starts_with_ci(nm, "upslash")   ||
                starts_with_ci(nm, "spinswing") ||
                starts_with_ci(nm, "turnswing"))
                return i;
        }
        // Pass 2 — any attack-flavoured name. Catches both `attack*`
        // (monster naming) and `backswing` (Locke fallback) when no
        // in-motion swing exists on the character.
        for (int32_t i = 0; i < n; ++i)
        {
            auto* s = img->GetState(i);
            if (!s || !s->animname[0]) continue;
            const char* nm = s->animname;
            if (starts_with_ci(nm, "attack")    ||
                starts_with_ci(nm, "backswing"))
                return i;
        }
        return -1;
    }

    for (int32_t i = 0; i < n; ++i)
    {
        auto* s = img->GetState(i);
        if (!s || !s->animname[0]) continue;
        const char* name = s->animname;
        if (style == VfxTest::EVfxPreviewStyle::CharacterCast)
        {
            if (starts_with_ci(name, "cast")   ||
                starts_with_ci(name, "magic")  ||
                starts_with_ci(name, "invoke") ||
                starts_with_ci(name, "spell"))
                return i;
        }
    }
    return -1;
}

void TeardownRig()
{
    SCharacterRig& rig = g_state.rig;
    if (rig.inst)
    {
        rig.inst->OffScreen();
        delete rig.inst;
        rig.inst = nullptr;
    }
    // Sokol meshes registered for the rig leak past TeardownRig; the
    // renderer's RegisterMesh API doesn't expose Unregister at present.
    // The handle vector is dropped here; a follow-up pass should add an
    // explicit free path (carry-out, not in scope for the rig itself).
    rig.subs.clear();
    rig.bones.clear();
    rig.img = nullptr;
    rig.last_legacy_tick = -1;
}

bool BuildRigCharacter(int32_t roster_idx,
                       VfxTest::EVfxPreviewStyle initial_style)
{
    SCharacterRig& rig = g_state.rig;
    if (!Renderer || roster_idx < 0 ||
        roster_idx >= int32_t(rig.roster.size()))
        return false;

    TeardownRig();
    rig.roster_idx = roster_idx;

    const auto [objclass, objtype] = rig.roster[roster_idx];
    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl) return false;

    SObjectDef def = {};
    def.objclass = short(objclass);
    def.objtype  = short(objtype);
    def.state    = 0;
    def.level    = 0;
    def.pos      = {0, 0, 0};
    def.vel      = {0, 0, 0};
    def.accum    = {0, 0, 0};
    def.rotatex  = 0;
    def.rotatey  = 0;
    def.rotatez  = 32;     // matches char3d; faces +X-ish in iso view
    def.group    = 0;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
    {
        log_warn("[vfx.rig] NewObject failed for class=%d type=%d", objclass, objtype);
        return false;
    }
    if (inst->IsCharacter())
    {
        if (auto* chr = dynamic_cast<TCharacter*>(inst))
        {
            const char* root = chr->DefaultRootState();
            if (root && *root)
                chr->SetState(root);
        }
    }
    inst->OnScreen();
    rig.inst = inst;

    rig.img = dynamic_cast<T3DImagery*>(inst->GetImagery());
    if (!rig.img)
    {
        log_warn("[vfx.rig] character '%s' has no T3DImagery; rig disabled",
                 inst->GetTypeName());
        TeardownRig();
        return false;
    }

    if (rig.fallback_albedo == kInvalidTexture)
        rig.fallback_albedo = Renderer->WhiteTextureHandle();

    // Pick initial state if the style requests an attack / cast preset.
    if (const int32_t s = PickInitialState(inst, initial_style); s >= 0)
        inst->SetState(s);

    const int32_t state = inst->GetState();
    const SAnimPose pose = SampleI3DAnimPose(rig.img, state, inst->GetFrame(),
                                             inst->GetPrevState(),
                                             inst->GetPrevFrame());

    bool bbox_init = false;
    const int32_t texslots = rig.img->NumTextures() + 1;
    for (int32_t objnum = 0; objnum < rig.img->NumObjects(); ++objnum)
    {
        if (rig.img->IsHidden(objnum, state))
            continue;
        bool obj_kept = false;
        for (int32_t texslot = 0; texslot < texslots; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(rig.img, objnum, texslot, verts, indices))
                continue;
            TTextureHandle albedo = rig.fallback_albedo;
            if (texslot > 0)
            {
                S3DTex tex = {};
                rig.img->GetTexture(texslot - 1, &tex);
                if (tex.htexture != kInvalidTexture) albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) { TeardownRig(); return false; }
            rig.subs.push_back({h, objnum, texslot});
            obj_kept = true;

            float world[16];
            BuildAnimPoseObjectMatrix(rig.img, pose, state, objnum, world);
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = world[0]*x + world[1]*y + world[2] *z + world[3];
                const float wy = world[4]*x + world[5]*y + world[6] *z + world[7];
                const float wz = world[8]*x + world[9]*y + world[10]*z + world[11];
                if (!bbox_init) {
                    rig.bbox_min[0] = rig.bbox_max[0] = wx;
                    rig.bbox_min[1] = rig.bbox_max[1] = wy;
                    rig.bbox_min[2] = rig.bbox_max[2] = wz;
                    bbox_init = true;
                } else {
                    rig.bbox_min[0] = std::fmin(rig.bbox_min[0], wx);
                    rig.bbox_min[1] = std::fmin(rig.bbox_min[1], wy);
                    rig.bbox_min[2] = std::fmin(rig.bbox_min[2], wz);
                    rig.bbox_max[0] = std::fmax(rig.bbox_max[0], wx);
                    rig.bbox_max[1] = std::fmax(rig.bbox_max[1], wy);
                    rig.bbox_max[2] = std::fmax(rig.bbox_max[2], wz);
                }
            }
        }
        if (!obj_kept)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMesh(rig.img, objnum, verts, indices))
                continue;
            TTextureHandle albedo = rig.fallback_albedo;
            if (rig.img->NumTextures() > 0)
            {
                S3DTex tex = {};
                rig.img->GetTexture(0, &tex);
                if (tex.htexture != kInvalidTexture) albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) { TeardownRig(); return false; }
            rig.subs.push_back({h, objnum, -1});
        }
    }
    if (rig.subs.empty() || !bbox_init)
    {
        TeardownRig();
        return false;
    }

    // Auto-fit so the character is about 280 world units tall on screen
    // (matches char3d's value; the harness camera frames cleanly).
    const float bbox_w = std::fmax(
        std::fmax(rig.bbox_max[0] - rig.bbox_min[0],
                  rig.bbox_max[1] - rig.bbox_min[1]),
                  rig.bbox_max[2] - rig.bbox_min[2]);
    rig.scale = (bbox_w > 1e-3f) ? (280.0f / bbox_w) : 1.0f;

    // Probe bone list. Walk all sub-object names so the UI can show
    // them; the per-bone "valid" flag is set on first render.
    for (int32_t i = 0; i < rig.img->NumObjects(); ++i)
    {
        const char* nm = rig.img->GetObjectName(i);
        if (nm && nm[0])
            rig.bones.push_back({std::string(nm), i, true});
    }

    log_info("[vfx.rig] loaded class=%s type=%s state=%d numStates=%d bones=%zu subs=%zu scale=%.2f",
             cl->ClassName(), inst->GetTypeName(),
             state, inst->NumStates(),
             rig.bones.size(), rig.subs.size(), rig.scale);
    return true;
}

bool EnsureRigForStyle(VfxTest::EVfxPreviewStyle style)
{
    if (!IsCharacterAttached(style))
        return false;
    SCharacterRig& rig = g_state.rig;
    if (rig.inst)
        return true;

    // First-time roster bootstrap. PlayerClass first (Locke is the
    // canonical rig character per playscreen.cpp:361); then
    // CharacterClass for NPC/monster previews. Cycle order: players,
    // then characters, alphabetical-ish within class by objtype index.
    rig.roster.clear();
    for (int32_t i = 0; i < PlayerClass.NumTypes(); ++i)
        if (PlayerClass.GetObjType(i))
            rig.roster.push_back({OBJCLASS_PLAYER, i});
    for (int32_t i = 0; i < CharacterClass.NumTypes(); ++i)
        if (CharacterClass.GetObjType(i))
            rig.roster.push_back({OBJCLASS_CHARACTER, i});
    if (rig.roster.empty())
    {
        log_warn("[vfx.rig] no characters in CharacterClass/PlayerClass; rig unavailable");
        return false;
    }

    // Try each roster entry in order until one builds successfully.
    // Some imageries (esp. early-load monsters) won't have a valid
    // T3DImagery; skip them rather than failing outright.
    for (int32_t i = 0; i < int32_t(rig.roster.size()); ++i)
    {
        if (BuildRigCharacter(i, style))
            return true;
    }
    log_warn("[vfx.rig] no roster entry produced a usable T3DImagery character");
    return false;
}

// Resolve an SVfxAnchor against the current rig state. Returns false
// + sets attach.resolved=false when the anchor can't be honoured
// (rig not loaded, named bone not present on character).
bool ResolveAnchor(const VfxTest::SVfxAnchor& a, VfxTest::SVfxAttachment& out)
{
    out = {};
    SCharacterRig& rig = g_state.rig;
    if (!rig.inst) return false;

    switch (a.kind)
    {
        case VfxTest::SVfxAnchor::EKind::CharacterRoot:
        {
            const S3DPoint p = rig.inst->Pos();
            out.world_pos[0] = float(p.x);
            out.world_pos[1] = float(p.y);
            out.world_pos[2] = float(p.z);
            // Identity basis for now; future: read inst->GetRotate().z
            // and rotate world_basis accordingly so spell-cast-ground
            // halos can orient with the caster facing.
            out.resolved = true;
            return true;
        }
        case VfxTest::SVfxAnchor::EKind::CharacterBoundsCenter:
        {
            const S3DPoint p = rig.inst->Pos();
            const float half_h = 0.5f *
                (rig.bbox_max[2] - rig.bbox_min[2]) * rig.scale;
            out.world_pos[0] = float(p.x);
            out.world_pos[1] = float(p.y);
            out.world_pos[2] = float(p.z) + half_h;
            out.resolved = true;
            return true;
        }
        case VfxTest::SVfxAnchor::EKind::Bone:
        case VfxTest::SVfxAnchor::EKind::BoneLocalPoint:
        {
            // Compute the bone matrix via BuildAnimPoseObjectMatrix
            // (matches char3d's render-side matrix construction). This
            // bypasses T3DAnimator::GetObjectMatrix to avoid touching
            // animator state mid-render, and gives us a matrix that
            // already chained through the parent hierarchy. Result is
            // in character-local space (instance pos = (0,0,0) for the
            // rig), so we add inst->Pos() to land in world space.
            const int32_t objnum = rig.img ? rig.img->GetObjectNum(
                const_cast<char*>(a.bone_name)) : -1;
            if (objnum < 0)
            {
                out.resolved = false;
                return false;
            }
            const int32_t state = rig.inst->GetState();
            const SAnimPose pose = SampleI3DAnimPose(
                rig.img, state, rig.inst->GetFrame(),
                rig.inst->GetPrevState(), rig.inst->GetPrevFrame());
            float m[16];
            BuildAnimPoseObjectMatrix(rig.img, pose, state, objnum, m);
            // Apply the rig's display scale + bbox recentre so anchors
            // land where the visible character is, not where the
            // unscaled mesh would be.
            const float s = rig.scale;
            const float cx = 0.5f * (rig.bbox_min[0] + rig.bbox_max[0]);
            const float cy = 0.5f * (rig.bbox_min[1] + rig.bbox_max[1]);
            const float cz = 0.5f * (rig.bbox_min[2] + rig.bbox_max[2]);
            const float lx = (a.kind == VfxTest::SVfxAnchor::EKind::BoneLocalPoint)
                             ? a.local_offset[0] : 0.0f;
            const float ly = (a.kind == VfxTest::SVfxAnchor::EKind::BoneLocalPoint)
                             ? a.local_offset[1] : 0.0f;
            const float lz = (a.kind == VfxTest::SVfxAnchor::EKind::BoneLocalPoint)
                             ? a.local_offset[2] : 0.0f;
            const float bx = m[0]*lx + m[1]*ly + m[2] *lz + m[3];
            const float by = m[4]*lx + m[5]*ly + m[6] *lz + m[7];
            const float bz = m[8]*lx + m[9]*ly + m[10]*lz + m[11];
            const S3DPoint p = rig.inst->Pos();
            out.world_pos[0] = float(p.x) + (bx - cx) * s;
            out.world_pos[1] = float(p.y) + (by - cy) * s;
            out.world_pos[2] = float(p.z) + (bz - cz) * s;
            // Extract bone basis rows from the upper-3 of the matrix.
            // The matrix is row-major; the basis vectors are the
            // columns of the rotation block. (Sufficient for spell-cast
            // facing — sword-trail will need a tangent derivation
            // when S09 Phase B comes online.)
            out.world_basis[0] = m[0]; out.world_basis[1] = m[4]; out.world_basis[2] = m[8];
            out.world_basis[3] = m[1]; out.world_basis[4] = m[5]; out.world_basis[5] = m[9];
            out.world_basis[6] = m[2]; out.world_basis[7] = m[6]; out.world_basis[8] = m[10];
            out.resolved = true;
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------------------------
// Weapon-sub-object helpers (S09 TWeaponSwipe Phase B)
//
// The retail TWeaponSwipe pulled the weapon hilt + tip from the
// character's `weapon` / `sword` / `ogrokaxe` named sub-object
// (priority order matches src/weapontrail.cpp:609-615 fallback chain).
// For the harness we need the same lookup plus the cached local-space
// max-Z extent of the weapon mesh (so a long sword draws a wide arc
// and a dagger draws a narrow one — same `GetWeaponExtents()` shape
// per S09 forensics §1).
// -------------------------------------------------------------------------

// Find the weapon sub-object's bone index on the currently loaded rig
// character, trying the three retail-canonical names in order.
// Returns -1 if none are present.
int32_t FindWeaponObjNum()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.img) return -1;
    static constexpr const char* kNames[] = { "weapon", "sword", "ogrokaxe" };
    for (const char* nm : kNames)
    {
        const int32_t n = rig.img->GetObjectNum(const_cast<char*>(nm));
        if (n >= 0) return n;
    }
    return -1;
}

const char* FindWeaponBoneName()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.img) return nullptr;
    static constexpr const char* kNames[] = { "weapon", "sword", "ogrokaxe" };
    for (const char* nm : kNames)
    {
        if (rig.img->GetObjectNum(const_cast<char*>(nm)) >= 0) return nm;
    }
    return nullptr;
}

// Scan the weapon sub-object's vertices for the maximum local-Z value;
// that's the weapon-tip local coordinate that retail's
// `GetWeaponExtents()` cached. Result is cached for the current rig
// character on the rig itself so we only pay the extract cost once.
struct SWeaponCache
{
    const T3DImagery* for_img    = nullptr;  // invalidated on character swap
    int32_t  objnum              = -1;
    float    tip_local_z         = 0.0f;
    bool     valid               = false;
};
SWeaponCache g_weapon_cache;

const SWeaponCache* GetWeaponCache()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.img)
    {
        g_weapon_cache = {};
        return nullptr;
    }
    if (g_weapon_cache.valid && g_weapon_cache.for_img == rig.img)
        return &g_weapon_cache;

    g_weapon_cache = {};
    g_weapon_cache.for_img = rig.img;
    g_weapon_cache.objnum  = FindWeaponObjNum();
    if (g_weapon_cache.objnum < 0)
        return &g_weapon_cache;

    // Pull the weapon mesh vertices and walk Z for the max extent.
    // Same shape as retail GetWeaponExtents in src/weapontrail.cpp:504:
    // start the hilt at local (0,0,0) (retail does ivweapbeg but then
    // clears it to zero; the rotation hub is the bone pivot) and let
    // the tip's Z drive the strip width.
    std::vector<SMeshVertex> verts;
    std::vector<uint16_t>    indices;
    if (!ExtractSubMesh(rig.img, g_weapon_cache.objnum, verts, indices))
    {
        log_warn("[vfx.swipe] weapon sub-object %d extract failed; "
                 "tipZ unavailable",
                 g_weapon_cache.objnum);
        return &g_weapon_cache;
    }
    float max_z = -1e9f;
    for (const auto& v : verts)
        if (v.pos[2] > max_z) max_z = v.pos[2];
    if (max_z <= 0.0f)
    {
        // Pre-release `GetWeaponExtents` starts `end = -100000` and
        // would carry a degenerate end if all verts had Z<=0. Skip.
        log_warn("[vfx.swipe] weapon mesh has no positive-Z extent; "
                 "tip degenerate, swipe disabled");
        return &g_weapon_cache;
    }
    g_weapon_cache.tip_local_z = max_z;
    g_weapon_cache.valid       = true;
    {
        const int32_t st = rig.inst ? rig.inst->GetState() : -1;
        auto* sd = (st >= 0 && rig.img) ? rig.img->GetState(st) : nullptr;
        log_info("[vfx.swipe] weapon='%s' objnum=%d tipZ=%.1f verts=%zu "
                 "rig_state=%d ('%s')",
                 FindWeaponBoneName(), g_weapon_cache.objnum,
                 double(max_z), verts.size(),
                 st, (sd && sd->animname[0]) ? sd->animname : "?");
    }
    return &g_weapon_cache;
}

// Is the rig character currently playing an "attack*" state? Used by
// the swipe to gate visible Submit. Mirrors retail's IsAttack()
// (charanimator.cpp:55 Render check) — for the harness we look at the
// state name rather than the IsDoing(ACTION_*) action mask, since the
// rig drives state changes directly via SetState rather than through
// the action system.
bool RigIsInAttackState()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.inst || !rig.img) return false;
    const int32_t st = rig.inst->GetState();
    auto* s = rig.img->GetState(st);
    if (!s) return false;
    const char* name = s->animname;
    if (!name || !name[0]) return false;
    // Case-insensitive "attack" prefix match.
    auto starts_with_ci = [](const char* str, const char* prefix) {
        for (size_t i = 0;; ++i) {
            const char p = prefix[i];
            if (!p) return true;
            const char c = str[i];
            const char pl = (p >= 'A' && p <= 'Z') ? char(p - 'A' + 'a') : p;
            const char cl = (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c;
            if (cl != pl) return false;
        }
    };
    // Match the same keyword set PickInitialState uses for
    // CharacterAttack — see note there about per-character naming
    // (Locke uses `swing` family; some monsters use `attack*`).
    return starts_with_ci(name, "attack")     ||
           starts_with_ci(name, "swing")      ||
           starts_with_ci(name, "backswing")  ||
           starts_with_ci(name, "upslash")    ||
           starts_with_ci(name, "spinswing")  ||
           starts_with_ci(name, "turnswing");
}

// Resolve the (hilt, tip) world-space anchor pair for the current
// rig's weapon. Returns false if the weapon sub-object isn't present
// or the rig isn't loaded; the swipe should skip Submit in that case.
bool ResolveWeaponAnchors(VfxTest::SVfxAttachment& out_hilt,
                          VfxTest::SVfxAttachment& out_tip)
{
    out_hilt = {};
    out_tip  = {};
    SCharacterRig& rig = g_state.rig;
    if (!rig.inst || !rig.img) return false;

    const SWeaponCache* wc = GetWeaponCache();
    if (!wc || !wc->valid) return false;

    const char* bone = FindWeaponBoneName();
    if (!bone) return false;

    // Hilt = weapon bone pivot (local origin transformed to world).
    VfxTest::SVfxAnchor hilt_anchor = {};
    hilt_anchor.kind = VfxTest::SVfxAnchor::EKind::Bone;
    std::strncpy(hilt_anchor.bone_name, bone,
                 sizeof(hilt_anchor.bone_name) - 1);
    if (!ResolveAnchor(hilt_anchor, out_hilt) || !out_hilt.resolved)
        return false;

    // Tip = local (0, 0, tipZ_local) through the weapon bone matrix.
    // The rig's BoneLocalPoint resolver already handles the scale +
    // bbox recentre composition.
    VfxTest::SVfxAnchor tip_anchor = {};
    tip_anchor.kind = VfxTest::SVfxAnchor::EKind::BoneLocalPoint;
    std::strncpy(tip_anchor.bone_name, bone,
                 sizeof(tip_anchor.bone_name) - 1);
    tip_anchor.local_offset[0] = 0.0f;
    tip_anchor.local_offset[1] = 0.0f;
    tip_anchor.local_offset[2] = wc->tip_local_z;
    if (!ResolveAnchor(tip_anchor, out_tip) || !out_tip.resolved)
        return false;
    return true;
}

void TickCharacterRig()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.inst) return;
    if (rig.paused_anim) return;
    const int64_t legacy_tick = TTime::LegacyFrameCount();
    if (legacy_tick == rig.last_legacy_tick) return;
    rig.last_legacy_tick = legacy_tick;
    rig.inst->NextFrame();
    if (rig.inst->NeedsAnimator() && !rig.inst->HasAnimator())
        rig.inst->OnScreen();
    rig.inst->Animate(false);

    // Auto-loop the active state. Retail anim cycles for `swing` /
    // `cast` are non-AF_LOOPING (per imagery.h:427 + object.cpp:1633):
    // when the frame reaches statesize-1, TObjectInstance::NextFrame
    // PINS frame at the end pose and sets the animator's `complete`
    // flag. For the harness we want the animation to keep cycling so
    // the swipe trail has live blade motion to trace — re-trigger via
    // ResetState() once we've held the end pose for a tick.
    //
    // Only kicks in for character-attached preview styles where the
    // user picked a specific animation (attack / cast). Idle states
    // are typically already AF_LOOPING (`combat` / `walk`) so this
    // path is a no-op for them.
    if (rig.img)
    {
        const int32_t st = rig.inst->GetState();
        const int32_t len = rig.img->GetAniLength(st);
        const int32_t flg = rig.img->GetAniFlags(st);
        const int32_t fr  = rig.inst->GetFrame();
        if (len > 1 && fr >= len - 1 && (flg & AF_LOOPING) == 0)
        {
            rig.inst->ResetState();
        }
    }
}

void RenderCharacterRigBackground()
{
    SCharacterRig& rig = g_state.rig;
    if (!rig.inst || rig.subs.empty() || !rig.img) return;

    const int32_t state = rig.inst->GetState();
    const SAnimPose pose = SampleI3DAnimPose(
        rig.img, state, rig.inst->GetFrame(),
        rig.inst->GetPrevState(), rig.inst->GetPrevFrame());

    const float s = rig.scale;
    const float cx = 0.5f * (rig.bbox_min[0] + rig.bbox_max[0]);
    const float cy = 0.5f * (rig.bbox_min[1] + rig.bbox_max[1]);
    const float cz = 0.5f * (rig.bbox_min[2] + rig.bbox_max[2]);
    const S3DPoint root = rig.inst->Pos();

    for (const auto& sub : rig.subs)
    {
        SMeshSubmit m = {};
        m.mesh = sub.handle;
        float w[16];
        BuildAnimPoseObjectMatrix(rig.img, pose, state, sub.objnum, w);
        // Scale uniformly then translate so bbox center lands at the
        // instance world position. This matches char3d's recenter
        // pattern but uses the instance pos as the anchor rather than
        // pinning to origin — so when (future) the rig allows moving
        // the character, the meshes follow.
        for (int32_t i = 0; i < 12; ++i) w[i] *= s;
        w[3]  += float(root.x) - cx * s;
        w[7]  += float(root.y) - cy * s;
        w[11] += float(root.z) - cz * s;
        std::memcpy(m.world, w, sizeof(w));
        m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
        Renderer->SubmitMesh(m);
    }
}

S3DPoint PickPreviewOrigin(VfxTest::EVfxPreviewStyle style)
{
    auto frand = []() { return float(std::rand()) / float(RAND_MAX); };
    switch (style)
    {
        case VfxTest::EVfxPreviewStyle::Static:
            return S3DPoint{0, 0, 0};
        case VfxTest::EVfxPreviewStyle::Combat:
        {
            // Random spot inside a ~240wu radius square at z=0 (the
            // visible scene area at the default iso camera). Combat
            // accents land all over the screen.
            constexpr float kHalf = 240.0f;
            return S3DPoint{ int32_t((frand() - 0.5f) * 2.0f * kHalf),
                             int32_t((frand() - 0.5f) * 2.0f * kHalf),
                             0 };
        }
        case VfxTest::EVfxPreviewStyle::SpellGround:
        {
            // Centred at origin, small jitter so successive casts
            // don't perfectly overlap. Spell casts read as ground-
            // level events: low Z, narrow XY range.
            constexpr float kJitter = 40.0f;
            return S3DPoint{ int32_t((frand() - 0.5f) * 2.0f * kJitter),
                             int32_t((frand() - 0.5f) * 2.0f * kJitter),
                             0 };
        }
        case VfxTest::EVfxPreviewStyle::Projectile:
            // TODO Phase 2.x: launch from one side toward the other
            // along a vector. Placeholder = static for now.
            return S3DPoint{-200, 0, 80};
        case VfxTest::EVfxPreviewStyle::CharacterIdle:
        case VfxTest::EVfxPreviewStyle::CharacterAttack:
        case VfxTest::EVfxPreviewStyle::CharacterCast:
            // The factory is given the *current* anchor world point;
            // the rig updates it every frame via ForcePos (effects that
            // read inst->Pos() track the bone automatically). If the
            // rig isn't loaded yet (first selection of this effect)
            // fall back to origin -- EnsureRigForStyle runs in
            // SpawnActive before the factory is called.
            if (g_state.current_attach.resolved)
                return S3DPoint{ int32_t(g_state.current_attach.world_pos[0]),
                                 int32_t(g_state.current_attach.world_pos[1]),
                                 int32_t(g_state.current_attach.world_pos[2]) };
            return S3DPoint{0, 0, 0};
    }
    return S3DPoint{0, 0, 0};
}

float RetriggerInterval(VfxTest::EVfxPreviewStyle style)
{
    switch (style)
    {
        case VfxTest::EVfxPreviewStyle::Static:           return 0.0f; // never
        case VfxTest::EVfxPreviewStyle::Combat:           return 0.6f;
        case VfxTest::EVfxPreviewStyle::SpellGround:      return 3.0f;
        case VfxTest::EVfxPreviewStyle::Projectile:       return 1.2f;
        // Character-attached effects retrigger less aggressively: the
        // character anim loop is the natural cadence, and any per-frame
        // attachment-tracking already keeps the effect "live". Idle =
        // long-lived ambient (status overlays / auras); attack/cast =
        // re-fire on the swing/cast cycle period (loose 1.5 s default).
        case VfxTest::EVfxPreviewStyle::CharacterIdle:    return 0.0f;
        case VfxTest::EVfxPreviewStyle::CharacterAttack:  return 1.5f;
        case VfxTest::EVfxPreviewStyle::CharacterCast:    return 1.5f;
    }
    return 0.0f;
}

void SpawnActive()
{
    if (g_state.active_idx < 0 || g_state.active_idx >= int32_t(g_state.catalogue.size()))
        return;
    const auto& e = g_state.catalogue[g_state.active_idx];

    // Character-attached: bring the rig online (no-op if already built),
    // then pre-resolve the anchor so the factory's spawn origin lands
    // on the bone instead of (0,0,0). Falls through with current_attach.resolved=false
    // if the rig can't be built — effects can still run, just at origin.
    if (IsCharacterAttached(e.preview_style))
    {
        if (EnsureRigForStyle(e.preview_style))
            ResolveAnchor(e.anchor, g_state.current_attach);
        else
            g_state.current_attach = {};
    }

    if (e.factory)
        g_state.active_ctx = e.factory(PickPreviewOrigin(e.preview_style));
    g_state.first_submit_logged = false;
    g_state.retrigger_timer = RetriggerInterval(e.preview_style);
    log_info("[vfx] active='%s' family='%s' pipeline='%s' style=%d debug=%d",
             e.id.c_str(), e.family.c_str(), e.pipeline.c_str(),
             int(e.preview_style), int(g_state.debug_mode));
}

void DestroyActive()
{
    if (g_state.active_idx < 0) return;
    const auto& e = g_state.catalogue[g_state.active_idx];
    if (e.destroy && g_state.active_ctx)
        e.destroy(g_state.active_ctx);
    g_state.active_ctx = nullptr;
}

void SelectIndex(int32_t idx)
{
    if (g_state.catalogue.empty()) return;
    const int32_t n = int32_t(g_state.catalogue.size());
    idx = ((idx % n) + n) % n;
    if (idx == g_state.active_idx) return;
    // Decide before destroying whether to tear the rig down: keep it
    // alive across character-attached → character-attached transitions
    // (no need to reload the mesh between, say, an aura and a halo on
    // the same Locke); free it on any transition out of the rig family.
    const bool prev_attached = IsCharacterAttached(
        g_state.catalogue[g_state.active_idx].preview_style);
    const bool next_attached = IsCharacterAttached(
        g_state.catalogue[idx].preview_style);
    DestroyActive();
    if (prev_attached && !next_attached)
        TeardownRig();
    g_state.active_idx = idx;
    SpawnActive();
}

void Restart()
{
    if (g_state.active_idx < 0) return;
    DestroyActive();
    SpawnActive();
    log_info("[vfx] restart idx=%d id='%s'",
             g_state.active_idx,
             g_state.catalogue[g_state.active_idx].id.c_str());
}

void CycleDebug()
{
    int v = int(g_state.debug_mode) + 1;
    if (v > int(EFxDebugMode::CurrentFrame)) v = 0;
    g_state.debug_mode = EFxDebugMode(v);
    log_info("[vfx] debug_mode -> %d", v);
}

const char* DebugModeName(EFxDebugMode m)
{
    switch (m)
    {
        case EFxDebugMode::Normal:       return "Normal";
        case EFxDebugMode::SolidColor:   return "Solid";
        case EFxDebugMode::FullTexture:  return "FullTex";
        case EFxDebugMode::CurrentFrame: return "CurrFrame";
    }
    return "?";
}

// =========================================================================
//   Diagnostic backdrops (Black / LtGray / Forest / Dungeon)
// =========================================================================

constexpr float kBackdropLtGrayRgba[4] = { 0.45f, 0.46f, 0.50f, 1.0f };
constexpr float kBackdropBlackRgba [4] = { 0.0f,  0.0f,  0.0f,  1.0f };

const char* BackgroundName(VfxTest::EVfxBackground b)
{
    switch (b)
    {
        case VfxTest::EVfxBackground::Black:   return "Black";
        case VfxTest::EVfxBackground::LtGray:  return "LtGray";
        case VfxTest::EVfxBackground::Forest:  return "Forest";
        case VfxTest::EVfxBackground::Dungeon: return "Dungeon";
    }
    return "?";
}

[[nodiscard]] TTextureHandle LoadBackgroundPNG(const char* relpath)
{
    if (!Renderer) return kInvalidTexture;
    char path[1024];
    std::snprintf(path, sizeof(path), "%s/%s", REV_VFX_TEST_BG_DIR, relpath);

    int w = 0, h = 0, n = 0;
    stbi_uc* px = stbi_load(path, &w, &h, &n, 4);
    if (!px) {
        log_warn("[vfx] failed to load backdrop '%s' (%s); BG entry disabled",
                 path, stbi_failure_reason());
        return kInvalidTexture;
    }

    const size_t bytes = size_t(w) * size_t(h) * 4u;
    const TTextureHandle texture = Renderer->RegisterTextureAsset(
        0,
        px,
        bytes,
        w,
        h,
        ERendererTextureFormat::RGBA8,
        bytes,
        ERendererTextureFilter::Linear);
    if (texture != kInvalidTexture)
        Renderer->AddTextureAssetRef(texture);
    stbi_image_free(px);

    if (texture == kInvalidTexture)
        log_warn("[vfx] backdrop texture upload failed for '%s'", path);
    else
        log_info("[vfx] loaded backdrop '%s' (%dx%d)", path, w, h);
    return texture;
}

void LoadBackgroundTextures()
{
    g_state.forest_texture  = LoadBackgroundPNG("forest.png");
    g_state.dungeon_texture = LoadBackgroundPNG("dungeon.png");
}

void ReleaseBackgroundTextures()
{
    if (Renderer) {
        if (g_state.forest_texture != kInvalidTexture)
            Renderer->ReleaseTextureAssetRef(g_state.forest_texture);
        if (g_state.dungeon_texture != kInvalidTexture)
            Renderer->ReleaseTextureAssetRef(g_state.dungeon_texture);
    }
    g_state.forest_texture  = kInvalidTexture;
    g_state.dungeon_texture = kInvalidTexture;
}

// Dispatch g_state.background to a renderer call. Black / LtGray pass a
// solid clear; Forest / Dungeon composite their image into the visible
// rect with LtGray letterbox bars (so partial-aspect coverage doesn't
// read as a jarring black band against the rig's mid-tone scene).
void ApplyBackdrop()
{
    if (!Renderer) return;
    switch (g_state.background)
    {
        case VfxTest::EVfxBackground::Black:
            Renderer->DrawBackdrop(kInvalidTexture,
                                   kBackdropBlackRgba[0], kBackdropBlackRgba[1],
                                   kBackdropBlackRgba[2], kBackdropBlackRgba[3]);
            break;
        case VfxTest::EVfxBackground::LtGray:
            Renderer->DrawBackdrop(kInvalidTexture,
                                   kBackdropLtGrayRgba[0], kBackdropLtGrayRgba[1],
                                   kBackdropLtGrayRgba[2], kBackdropLtGrayRgba[3]);
            break;
        case VfxTest::EVfxBackground::Forest:
            Renderer->DrawBackdrop(g_state.forest_texture,
                                   kBackdropLtGrayRgba[0], kBackdropLtGrayRgba[1],
                                   kBackdropLtGrayRgba[2], kBackdropLtGrayRgba[3]);
            break;
        case VfxTest::EVfxBackground::Dungeon:
            Renderer->DrawBackdrop(g_state.dungeon_texture,
                                   kBackdropLtGrayRgba[0], kBackdropLtGrayRgba[1],
                                   kBackdropLtGrayRgba[2], kBackdropLtGrayRgba[3]);
            break;
    }
}

void CycleBackground()
{
    int v = int(g_state.background) + 1;
    if (v > int(VfxTest::EVfxBackground::Dungeon)) v = 0;
    g_state.background = VfxTest::EVfxBackground(v);
    log_info("[vfx] background -> %s", BackgroundName(g_state.background));
}

void DrawBrowserPanel()
{
    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.86f);
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoFocusOnAppearing;
    if (!ImGui::Begin("VFX Browser", nullptr, kFlags))
    {
        ImGui::End();
        return;
    }

    const int32_t n = int32_t(g_state.catalogue.size());

    if (n == 0)
    {
        ImGui::TextDisabled("(no effects registered)");
    }
    else
    {
        const int32_t cur = (g_state.active_idx < 0 || g_state.active_idx >= n)
            ? 0 : g_state.active_idx;
        const VfxTest::SEffect& sel = g_state.catalogue[cur];

        ImGui::Text("Effect %d / %d", cur + 1, n);
        ImGui::SetNextItemWidth(360.0f);
        if (ImGui::BeginCombo("##effect", sel.id.c_str()))
        {
            for (int32_t i = 0; i < n; ++i)
            {
                const bool selected = (i == cur);
                if (ImGui::Selectable(g_state.catalogue[i].id.c_str(), selected))
                    SelectIndex(i);
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("##prev", ImGuiDir_Left))  SelectIndex(cur - 1);
        ImGui::SameLine();
        if (ImGui::ArrowButton("##next", ImGuiDir_Right)) SelectIndex(cur + 1);

        ImGui::Separator();
        ImGui::Text("Family:   %s", sel.family.empty()   ? "-" : sel.family.c_str());
        ImGui::Text("Pipeline: %s", sel.pipeline.empty() ? "-" : sel.pipeline.c_str());

        // Character-rig controls (visible when an attached effect is
        // active and the rig has actually loaded a character).
        if (IsCharacterAttached(sel.preview_style) && g_state.rig.inst)
        {
            SCharacterRig& rig = g_state.rig;
            ImGui::Separator();
            ImGui::TextUnformatted("Character rig:");
            const auto [oc, ot] = rig.roster[rig.roster_idx];
            TObjectClass* cl = TObjectClass::GetClass(oc);
            ImGui::Text("  Character: %s / %s",
                        cl ? cl->ClassName() : "?",
                        rig.inst->GetTypeName());
            ImGui::SameLine();
            if (ImGui::SmallButton("Next char##rigchar"))
            {
                const int32_t next = (rig.roster_idx + 1) %
                                     int32_t(rig.roster.size());
                BuildRigCharacter(next, sel.preview_style);
                ResolveAnchor(sel.anchor, g_state.current_attach);
            }

            const int32_t state = rig.inst->GetState();
            const char* state_name =
                (rig.img && rig.img->GetState(state))
                    ? rig.img->GetState(state)->animname : "?";
            ImGui::Text("  Anim state: %d / %d  '%s'",
                        state + 1, rig.inst->NumStates(),
                        state_name ? state_name : "?");
            ImGui::SameLine();
            if (ImGui::SmallButton("Next state##riganim"))
            {
                int32_t st = (state + 1) % rig.inst->NumStates();
                rig.inst->SetState(st);
                rig.last_legacy_tick = -1;
            }

            // Anchor readback (live attachment world point).
            if (g_state.current_attach.resolved)
                ImGui::Text("  Anchor: %s @ (%.0f, %.0f, %.0f)",
                            (sel.anchor.kind == VfxTest::SVfxAnchor::EKind::Bone ||
                             sel.anchor.kind == VfxTest::SVfxAnchor::EKind::BoneLocalPoint)
                                ? sel.anchor.bone_name : "[root]",
                            g_state.current_attach.world_pos[0],
                            g_state.current_attach.world_pos[1],
                            g_state.current_attach.world_pos[2]);
            else
                ImGui::TextDisabled("  Anchor: '%s' not present on character",
                                    sel.anchor.bone_name[0] ? sel.anchor.bone_name : "[root]");

            ImGui::Checkbox("Pause anim##riganim", &rig.paused_anim);
        }
    }

    // Diagnostic backdrop -- dev cycles Black/LtGray/Forest/Dungeon to vet
    // effects against different scene contexts (additive bloom, alpha
    // see-through, real outdoor + interior screenshots). Unconditionally
    // visible so it works for both rig-attached and free-effect previews.
    ImGui::Separator();
    {
        int32_t bg = int32_t(g_state.background);
        ImGui::TextUnformatted("Backdrop:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(160.0f);
        const char* items[] = { "Black", "LtGray", "Forest", "Dungeon" };
        if (ImGui::Combo("##backdrop", &bg, items, IM_ARRAYSIZE(items)))
        {
            g_state.background = VfxTest::EVfxBackground(bg);
            log_info("[vfx] background -> %s (UI)",
                     BackgroundName(g_state.background));
        }
        if (g_state.background == VfxTest::EVfxBackground::Forest &&
            g_state.forest_texture == kInvalidTexture)
        {
            ImGui::SameLine();
            ImGui::TextDisabled("(missing)");
        }
        if (g_state.background == VfxTest::EVfxBackground::Dungeon &&
            g_state.dungeon_texture == kInvalidTexture)
        {
            ImGui::SameLine();
            ImGui::TextDisabled("(missing)");
        }
    }

    ImGui::Separator();
    int32_t mode = int32_t(g_state.debug_mode);
    ImGui::TextUnformatted("Debug:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Normal",    &mode, 0)) g_state.debug_mode = EFxDebugMode::Normal;
    ImGui::SameLine();
    if (ImGui::RadioButton("Solid",     &mode, 1)) g_state.debug_mode = EFxDebugMode::SolidColor;
    ImGui::SameLine();
    if (ImGui::RadioButton("FullTex",   &mode, 2)) g_state.debug_mode = EFxDebugMode::FullTexture;
    ImGui::SameLine();
    if (ImGui::RadioButton("CurrFrame", &mode, 3)) g_state.debug_mode = EFxDebugMode::CurrentFrame;

    ImGui::Checkbox("Pause", &g_state.paused);
    ImGui::SameLine();
    if (ImGui::Button("Step"))    { if (g_state.paused) g_state.step_once = true; }
    ImGui::SameLine();
    if (ImGui::Button("Restart")) Restart();

    ImGui::Separator();
    ImGui::TextDisabled("frame=%lld  debug=%s%s",
                        static_cast<long long>(g_state.frames_rendered),
                        DebugModeName(g_state.debug_mode),
                        g_state.paused ? "  [paused]" : "");
    ImGui::TextDisabled("Keys: \xe2\x86\x90/\xe2\x86\x92  cycle   D  debug   "
                        "B  backdrop   Space  pause   .  step   R  restart   Esc  quit");

    ImGui::End();
}

}  // namespace

namespace VfxTest {

void DeferredRegister(const SEffect& effect)
{
    g_state.deferred.push_back(effect);
}

void Register(const SEffect& effect)
{
    g_state.catalogue.push_back(effect);
    std::sort(g_state.catalogue.begin(), g_state.catalogue.end(),
              [](const SEffect& a, const SEffect& b) {
                  if (a.family != b.family) return a.family < b.family;
                  return a.id < b.id;
              });
    // Selection by stored id (sort may have shuffled active index).
    if (g_state.active_idx >= 0 && !g_state.catalogue.empty())
    {
        const std::string active_id =
            g_state.catalogue[g_state.active_idx].id;
        for (int32_t i = 0; i < int32_t(g_state.catalogue.size()); ++i)
            if (g_state.catalogue[i].id == active_id) { g_state.active_idx = i; break; }
    }
}

bool Initialize()
{
    g_state.catalogue.clear();
    g_state.active_idx = -1;
    g_state.active_ctx = nullptr;
    g_state.frames_rendered = 0;
    g_state.paused = false;
    g_state.step_once = false;
    g_state.debug_mode = EFxDebugMode::Normal;
    g_state.first_submit_logged = false;
    g_state.background = EVfxBackground::LtGray;

    LoadBackgroundTextures();

    // --vfx-bg=<...> startup override (lower-case, leading hyphens
    // already stripped by argh). Useful for scripted multi-BG captures.
    if (StartupVfxBackground[0])
    {
        auto eq_ci = [](const char* a, const char* b) {
            while (*a && *b) {
                const char ca = (*a >= 'A' && *a <= 'Z') ? char(*a - 'A' + 'a') : *a;
                const char cb = (*b >= 'A' && *b <= 'Z') ? char(*b - 'A' + 'a') : *b;
                if (ca != cb) return false;
                ++a; ++b;
            }
            return *a == 0 && *b == 0;
        };
        if      (eq_ci(StartupVfxBackground, "black"))   g_state.background = EVfxBackground::Black;
        else if (eq_ci(StartupVfxBackground, "ltgray"))  g_state.background = EVfxBackground::LtGray;
        else if (eq_ci(StartupVfxBackground, "forest"))  g_state.background = EVfxBackground::Forest;
        else if (eq_ci(StartupVfxBackground, "dungeon")) g_state.background = EVfxBackground::Dungeon;
        else log_warn("[vfx] --vfx-bg='%s' unknown; staying on default LtGray",
                      StartupVfxBackground);
        log_info("[vfx] startup background = %s",
                 BackgroundName(g_state.background));
    }

    for (const auto& e : g_state.deferred)
        Register(e);

    g_state.initialized = true;

    log_info("[vfx] --test=vfx initialize (%d effects registered)",
             int(g_state.catalogue.size()));
    for (size_t i = 0; i < g_state.catalogue.size(); ++i)
        log_info("[vfx]   [%zu] id='%s' family='%s' pipeline='%s'",
                 i,
                 g_state.catalogue[i].id.c_str(),
                 g_state.catalogue[i].family.c_str(),
                 g_state.catalogue[i].pipeline.c_str());

    if (!g_state.catalogue.empty())
    {
        // --vfx=<id> CLI flag: pre-select an effect by id (case-
        // sensitive match against catalogue entry's id). Falls back
        // to first alphabetically-sorted entry if no match.
        g_state.active_idx = 0;
        if (StartupVfxId[0])
        {
            bool matched = false;
            for (int32_t i = 0; i < int32_t(g_state.catalogue.size()); ++i)
            {
                if (g_state.catalogue[i].id == StartupVfxId)
                {
                    g_state.active_idx = i;
                    matched = true;
                    log_info("[vfx] --vfx='%s' matched entry %d",
                             StartupVfxId, i);
                    break;
                }
            }
            if (!matched)
                log_warn("[vfx] --vfx='%s' did not match any entry; "
                         "using default index 0", StartupVfxId);
        }
        SpawnActive();
    }
    else
    {
        log_warn("[vfx] no effects registered -- empty browser");
    }
    return true;
}

void Close()
{
    DestroyActive();
    TeardownRig();
    ReleaseBackgroundTextures();
    g_state.catalogue.clear();
    g_state.active_idx = -1;
    g_state.initialized = false;
    log_info("[vfx] close (frames=%lld)", static_cast<long long>(g_state.frames_rendered));
}

void Render()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer())
        return;

    ++g_state.frames_rendered;

    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const int32_t cam_ox = vw / 2;
    const int32_t cam_oy = vh / 2;

    // Neutral 3D scene: directional sun, modern lighting, no shadows/AO.
    // Camera centred at world origin, kCam world-units back. Scene is
    // deliberately bright + neutral so effects read clearly against the
    // background regardless of their own color; effects render in the
    // post-lighting fx_pass so the sun/ambient don't tint them anyway.
    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.85f);
    Renderer->SetAmbientColor(0.80f, 0.80f, 0.80f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);
    Renderer->SetShadowWorldDir(0.6f, -0.6f, 0.4f);
    Renderer->SetShadowVariance(0.0f, 0.0f, 1.0f);

    constexpr float kCam  = 2750.0f;
    constexpr float zHalf =  512.0f;
    const float znear = kCam - zHalf;
    const float zfar  = kCam + zHalf;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();

    // Step 1: tick the character rig (24 Hz sim-tick gated animation)
    // BEFORE resolving anchors. This way the same render frame sees a
    // consistent character pose used both for the background-mesh
    // submit and the effect attachment.
    const bool should_tick = !g_state.paused || g_state.step_once;
    g_state.step_once = false;
    if (should_tick)
        TickCharacterRig();

    // Step 2: active-effect submit hook. LS-pipeline effects call
    // AddPointLight here so RunLightingPass picks them up; FB / PE / SR
    // effects fill the fx queue, drained by DrainFxQueue inside
    // RunLightingPass.
    if (should_tick && g_state.active_idx >= 0)
    {
        const auto& e = g_state.catalogue[g_state.active_idx];

        // Resolve attachment for character-attached effects before any
        // retrigger or submit work. The resolved point is also used to
        // re-position the effect's owning instance via ForcePos (effects
        // that read inst->Pos() in their tick — Halo, Drip, Mist —
        // track the bone automatically without per-effect code).
        if (IsCharacterAttached(e.preview_style))
        {
            ResolveAnchor(e.anchor, g_state.current_attach);
            if (g_state.current_attach.resolved && g_state.active_ctx)
            {
                // Effects expose their TObjectInstance* differently —
                // some bury it inside a void* context wrapper struct
                // (FlameCtx etc), some are TObjectInstance themselves.
                // Rather than re-plumb every effect, the rig publishes
                // the attachment via PickPreviewOrigin (used at spawn)
                // and exposes ForcePos through a separate hook below.
                // See ApplyAttachmentTo helpers near each effect's
                // registration block.
            }
        }

        // Re-trigger pattern per preview style: non-Static styles
        // tick down a timer, destroy the previous instance and spawn
        // a fresh one at a new origin when the timer expires. This
        // is what makes a combat splat play repeatedly across the
        // visible area instead of firing once and never again.
        const float interval = RetriggerInterval(e.preview_style);
        if (interval > 0.0f)
        {
            g_state.retrigger_timer -= float(TTime::DeltaTime());
            if (g_state.retrigger_timer <= 0.0f)
            {
                if (e.destroy && g_state.active_ctx)
                    e.destroy(g_state.active_ctx);
                g_state.active_ctx = nullptr;
                if (e.factory)
                    g_state.active_ctx = e.factory(PickPreviewOrigin(e.preview_style));
                g_state.retrigger_timer = interval;
            }
        }

        // Dispatch submit_attached for character-attached effects,
        // submit for everyone else. Effects choose which they set in
        // their SEffect record (both populated is treated as
        // submit_attached wins for character-attached styles).
        if (IsCharacterAttached(e.preview_style) && e.submit_attached &&
            g_state.active_ctx)
        {
            e.submit_attached(g_state.active_ctx, g_state.debug_mode,
                              g_state.current_attach);
            if (!g_state.first_submit_logged)
            {
                g_state.first_submit_logged = true;
                log_info("[vfx] first submit_attached: id='%s' style=%d "
                         "anchor_resolved=%d at (%.0f,%.0f,%.0f)",
                         e.id.c_str(), int(e.preview_style),
                         int(g_state.current_attach.resolved),
                         g_state.current_attach.world_pos[0],
                         g_state.current_attach.world_pos[1],
                         g_state.current_attach.world_pos[2]);
            }
        }
        else if (e.submit && g_state.active_ctx)
        {
            e.submit(g_state.active_ctx, g_state.debug_mode);
            if (!g_state.first_submit_logged)
            {
                g_state.first_submit_logged = true;
                log_info("[vfx] first submit: id='%s' pipeline='%s' debug=%d",
                         e.id.c_str(), e.pipeline.c_str(), int(g_state.debug_mode));
            }
        }
    }

    // Step 3: tile pass. Empty unless the rig is active, in which case
    // we submit the character meshes here so they get lit alongside the
    // normal scene path. The G-buffer is always cleared transparent
    // (alpha=0) so the light shader's `alb.a < 0.01` discard reveals
    // whatever the backdrop step (below) wrote into lit_target -- this
    // way Black / LtGray / Forest / Dungeon all read identically for the
    // scene-empty pixels.
    Renderer->BeginTilePass(kBackdropLtGrayRgba[0], kBackdropLtGrayRgba[1],
                            kBackdropLtGrayRgba[2], kBackdropLtGrayRgba[3]);
    RenderCharacterRigBackground();
    // Per M09_FORENSICS.md §M09b: effects that submit through the
    // transparent_world_queue (SubmitHelperMesh) MUST do so after
    // BeginTilePass — that call clears the queue. So we give effects
    // a submit_world hook here.
    if (g_state.active_idx >= 0 &&
        g_state.active_idx < int32_t(g_state.catalogue.size()))
    {
        const auto& e = g_state.catalogue[g_state.active_idx];
        if (e.submit_world && g_state.active_ctx)
            e.submit_world(g_state.active_ctx, g_state.debug_mode);
    }
    Renderer->EndTilePass();
    // Diagnostic backdrop: write lit_target with the selected BG before
    // the deferred light shader runs. Light shader keeps these pixels
    // wherever the scene has no contribution (discard on alb.a<0.01);
    // post-light fx_pass then blends against the backdrop just like it
    // would against any lit scene content. See vfxtest.h EVfxBackground.
    ApplyBackdrop();
    Renderer->RunLightingPass();

    if (!StartupVfxHideUi)
        DrawBrowserPanel();
}

void HandleKeyPress(int32_t key, bool down)
{
    if (!down) return;
    switch (key)
    {
        case VK_LEFT:    SelectIndex(g_state.active_idx - 1); break;
        case VK_RIGHT:   SelectIndex(g_state.active_idx + 1); break;
        case 'R':        Restart(); break;
        case VK_SPACE:   g_state.paused = !g_state.paused;
                         log_info("[vfx] paused=%d", g_state.paused ? 1 : 0); break;
        case '.':        if (g_state.paused) g_state.step_once = true; break;
        case 'D':        CycleDebug(); break;
        case 'B':        CycleBackground(); break;
        case '1':        g_state.debug_mode = EFxDebugMode::Normal;       log_info("[vfx] debug_mode -> 0"); break;
        case '2':        g_state.debug_mode = EFxDebugMode::SolidColor;   log_info("[vfx] debug_mode -> 1"); break;
        case '3':        g_state.debug_mode = EFxDebugMode::FullTexture;  log_info("[vfx] debug_mode -> 2"); break;
        case '4':        g_state.debug_mode = EFxDebugMode::CurrentFrame; log_info("[vfx] debug_mode -> 3"); break;
        default: break;
    }
}

}  // namespace VfxTest

// *************************************************************************
// * Built-in test effects                                                 *
// *************************************************************************
//
// These four effects validate the Phase 1 FX submission pipelines
// end-to-end. The FB (F01), PE (B01), and SR (S01) slots are real
// per-effect Phase 2 ports (`TFlameEffect`, `TBloodEffect`,
// `TStripEffect`) — the lambdas here are thin shims that defer all
// spawn / kinematic / draw work to the effect class. The LS (X17) slot
// is still a placeholder pending its own Phase 2 port (`TFlareAnimator`).
//
// The flare placeholder keeps a static solid-color texture handle created
// lazily on first spawn; the real effects bring their own textures via
// the imagery cache (see TFlameEffect::SpawnForTest / TBloodEffect::SpawnForTest)
// or fall back to the renderer's white texture handle (TStripEffect — the
// Magic\lightning.* asset hasn't been identified yet; tracked as a S01
// forensics gap).
//
// *************************************************************************

namespace {

TTextureHandle WhiteTexture()
{
    return Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;
}

TTextureHandle GoldFlareTexture()
{
    if (!Renderer) return kInvalidTexture;
    return Renderer->SolidColorTexture(0x46584C4641524full,  // "FXLFLARO"
                                       0xFF40D8FFu,           // warm yellow
                                       "vfx.flare.solid");
}

// --- FB: real TFlameEffect (TorchFlame, Magic/flame.i3d) -----------------
// Spawns a sector-less TFlameEffect at world origin via SpawnForTest and
// drives its flipbook component each frame through the same Submit path
// the in-game per-instance walk uses (maprenderer.cpp Billboard branch).
// SpawnForTest is allowed to fail if the imagery archive isn't mounted /
// the asset is missing; in that case we register a no-op submit so the
// harness still navigates past F01 instead of crashing.
struct SFlameCtx {
    TFlameEffect* flame = nullptr;
};

void* FlameSpawn(const S3DPoint& origin)
{
    auto* c = new SFlameCtx();
    c->flame = TFlameEffect::SpawnForTest(origin);
    if (!c->flame)
        log_warn("[vfx] TFlameEffect::SpawnForTest returned null; F01 entry will draw nothing");
    return c;
}

void FlameDestroy(void* cp)
{
    auto* c = static_cast<SFlameCtx*>(cp);
    delete c->flame;
    delete c;
}

void FlameSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlameCtx*>(cp);
    if (!c->flame || !Renderer)
        return;
    auto* flipbook = c->flame->GetComponent<TFlipbookBillboardComponent>();
    if (!flipbook || flipbook->Texture() == kInvalidTexture)
        return;
    flipbook->SetDebugMode(dbg);
    flipbook->Submit(*Renderer, *c->flame);
}

// --- FB: real TBloodEffect (B01, Misc/Blood.I3D) ------------------------
// Spawns a sector-less single-burst TBloodEffect at the harness origin via
// SpawnForTest and drives the ported TBloodSystem loop through
// TickAndSubmitForTest each frame. Faithful direct port (not a
// TParticleBucket / effects.def re-derivation) — see src/effect.cpp B01
// block.
//
// Single non-overlapping burst (per RECONSTRUCTION_PROTOCOL + the §4.1
// "one effect on screen at a time" rule): the harness lets a burst fully
// play out (IsAlive() goes false when the last droplet expires), waits a
// clear gap, then re-fires ONE fresh burst. Blood bounces and splats so
// the gap is slightly longer than for sparks (~1.0s) — enough to clearly
// separate one burst from the next so the directional spray + splat-stick
// + shrink can be verified per-burst. Static preview style (internal
// cadence is owned here, not by the harness destroy/respawn timer).
struct SBloodCtx {
    TBloodEffect* blood   = nullptr;
    S3DPoint      origin  = {0, 0, 0};
    float         gap     = 0.0f;     // post-death cooldown before re-fire
};

constexpr float kBloodRetriggerGap = 1.0f;   // clear gap between bursts (s)

void* BloodSpawn(const S3DPoint& origin)
{
    auto* c = new SBloodCtx();
    c->origin = origin;
    c->blood = TBloodEffect::SpawnForTest(origin);
    if (!c->blood)
        log_warn("[vfx] TBloodEffect::SpawnForTest returned null; B01 entry will draw nothing");
    return c;
}

void BloodDestroy(void* cp)
{
    auto* c = static_cast<SBloodCtx*>(cp);
    delete c->blood;
    delete c;
}

void BloodSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SBloodCtx*>(cp);
    if (!c)
        return;

    // Burst has fully played out -> wait a clear gap, then re-fire ONE
    // fresh burst. Keeps exactly one clean, non-overlapping instance
    // visible for verification (no overlapping/grouped bursts).
    if (!c->blood || !c->blood->IsAlive())
    {
        c->gap -= float(TTime::DeltaTime());
        if (c->gap <= 0.0f)
        {
            delete c->blood;
            c->blood = TBloodEffect::SpawnForTest(c->origin);
            c->gap   = kBloodRetriggerGap;
        }
    }

    if (c->blood)
        c->blood->TickAndSubmitForTest(dbg);
}

// --- SR: real TStripEffect (S01 lightning bolt) --------------------------
// Spawns a sector-less TStripEffect at the harness-provided origin via
// SpawnForTest and drives its segment ring through TickAndSubmitForTest
// each frame. Mirrors the F01 / B01 pattern: the harness lambda is a thin
// shim; all spawn / kinematic / draw logic lives on the real effect class.
// See src/stripeffect.cpp for the SR-pipeline scope boundary (Phase 2.3 =
// strip-only validator; glow + sparks composite lands in S01a).
struct SStripCtx {
    TStripEffect* strip = nullptr;
};

void* StripSpawn(const S3DPoint& origin)
{
    auto* c = new SStripCtx();
    c->strip = TStripEffect::SpawnForTest(origin);
    if (!c->strip)
        log_warn("[vfx] TStripEffect::SpawnForTest returned null; S01 entry will draw nothing");
    return c;
}

void StripDestroy(void* cp)
{
    auto* c = static_cast<SStripCtx*>(cp);
    delete c->strip;
    delete c;
}

void StripSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SStripCtx*>(cp);
    if (!c->strip)
        return;
    c->strip->TickAndSubmitForTest(dbg);
}

// --- FB: real TRippleEffect (procedural ring atlas, H03) ----------------
// Spawns a sector-less TRippleEffect at the harness-provided origin via
// SpawnForTest and drives its ring expansion through TickAndSubmitForTest
// each frame. The ripple uses no I3D asset (no `Magic\Ripple.I3D` or
// equivalent exists in the data tree — see INVENTORY H03 gap 7.2); the
// 4x4 atlas is built procedurally in src/effect.cpp::RippleAtlasTexture.
// The harness lambda is a thin shim — all spawn / animator / submit
// logic lives on the real TRippleEffect class. Mirrors F01 / B01 / S01.
//
// SpellGround preview style — water ripple is a ground-anchored event
// (drip from above lands; the ring expands at the impact point), so
// the cadence reads as "drip every few seconds, small XY jitter".
struct SRippleCtx {
    TRippleEffect* ripple = nullptr;
};

void* RippleSpawn(const S3DPoint& origin)
{
    auto* c = new SRippleCtx();
    c->ripple = TRippleEffect::SpawnForTest(origin);
    if (!c->ripple)
        log_warn("[vfx] TRippleEffect::SpawnForTest returned null; H03 entry will draw nothing");
    return c;
}

void RippleDestroy(void* cp)
{
    auto* c = static_cast<SRippleCtx*>(cp);
    delete c->ripple;
    delete c;
}

void RippleSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SRippleCtx*>(cp);
    if (!c->ripple)
        return;
    c->ripple->TickAndSubmitForTest(dbg);
}

// --- PE: real TMistEffect (Magic/mist.i3d, M05) --------------------------
// Spawns a sector-less TMistEffect at the harness-provided origin via
// SpawnForTest and drives its 50 long-lived ascending wisps through
// TickAndSubmitForTest each frame. Unlike B01 (one-shot 10-droplet
// burst), this is a *continuous emitter*: drops self-recycle in place
// on landing, so the bucket count stays steady-state at 50 from spawn
// to destroy. See INVENTORY M05 forensics §3 for the cadence and §6
// for the rig category.
//
// Static preview style — mist is ambient environmental, never re-fires;
// the harness destroying-and-respawning would defeat the test value
// (the per-drop steady-state needs a few seconds to develop). Cycle
// out and back in with Left/Right to restart.
struct SMistCtx {
    TMistEffect* mist = nullptr;
};

void* MistSpawn(const S3DPoint& origin)
{
    auto* c = new SMistCtx();
    c->mist = TMistEffect::SpawnForTest(origin);
    if (!c->mist)
        log_warn("[vfx] TMistEffect::SpawnForTest returned null; M05 entry will draw nothing");
    return c;
}

void MistDestroy(void* cp)
{
    auto* c = static_cast<SMistCtx*>(cp);
    delete c->mist;
    delete c;
}

void MistSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SMistCtx*>(cp);
    if (!c->mist)
        return;
    c->mist->TickAndSubmitForTest(dbg);
}

// --- PE: real TDripEffect (Magic/drip.i3d, H04) + drip→ripple chain -----
// Spawns a sector-less TDripEffect at the harness-provided origin via
// SpawnForTest and drives its single-drop ceiling-emitter through
// TickAndSubmitForTest each frame. H04 is the natural retail caller of
// H03 TRippleEffect — on each landing the drip spawns a real
// TRippleEffect owned via std::unique_ptr inside the drip. This makes
// H04 the harness's **first "effect that spawns another effect"**
// demonstration (see INVENTORY H04 §6).
//
// Static preview style — drip is ambient environmental: the cyclic
// respawn happens internally within TickAndSubmitForTest, the harness
// never re-fires the drip itself. Cycle out and back in with Left/Right
// to reset the emitter state.
struct SDripCtx {
    TDripEffect* drip = nullptr;
};

void* DripSpawn(const S3DPoint& origin)
{
    auto* c = new SDripCtx();
    c->drip = TDripEffect::SpawnForTest(origin);
    if (!c->drip)
        log_warn("[vfx] TDripEffect::SpawnForTest returned null; H04 entry will draw nothing");
    return c;
}

void DripDestroy(void* cp)
{
    auto* c = static_cast<SDripCtx*>(cp);
    delete c->drip;
    delete c;
}

void DripSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SDripCtx*>(cp);
    if (!c->drip)
        return;
    c->drip->TickAndSubmitForTest(dbg);
}

// --- FB+LS: real THaloEffect (procedural radial-gradient texture, L02) --
// Spawns a sector-less THaloEffect at the harness-provided origin via
// SpawnForTest and drives its triangle-wave scale envelope through
// TickAndSubmitForTest each frame. Halo submits one additive billboard
// (FB pipeline) AND re-adds one dynamic point light (LS pipeline) each
// frame — the row's "FB+LS" pipeline-tag in INVENTORY. Mirrors the
// F01/H03/M05 lambda-shim pattern; all spawn/animator/submit logic
// lives on THaloEffect.
//
// SpellGround preview style — the halo's triangle-wave + brief lifetime
// reads as a spell-cast ground halo (per INVENTORY L02 §5 inference):
// ground-anchored event, occasional re-fire at small XY jitter, 3.0 sec
// retrigger cadence (same as H03 ripple).
struct SHaloCtx {
    THaloEffect* halo = nullptr;
};

void* HaloSpawn(const S3DPoint& origin)
{
    auto* c = new SHaloCtx();
    c->halo = THaloEffect::SpawnForTest(origin);
    if (!c->halo)
        log_warn("[vfx] THaloEffect::SpawnForTest returned null; L02 entry will draw nothing");
    return c;
}

void HaloDestroy(void* cp)
{
    auto* c = static_cast<SHaloCtx*>(cp);
    delete c->halo;
    delete c;
}

void HaloSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SHaloCtx*>(cp);
    if (!c->halo)
        return;
    c->halo->TickAndSubmitForTest(dbg);
}

// --- FB: real TFireEffect (procedural orange/yellow scatter, F03) --------
// Spawns a sector-less TFireEffect at the harness-provided origin via
// SpawnForTest and ticks its 15-quad scatter scatter through
// TickAndSubmitForTest each frame. FB pipeline only (no particle bucket,
// no point light); per INVENTORY F03 forensics §6 the pre-release effect
// renders additive textured quads directly.
//
// SpellGround preview style — F03 has no live retail caller (forensics
// §4) so the cadence choice is a presentational decision; SpellGround
// 3 sec re-fire reads as "another patch of ambient fire pops up nearby",
// which matches the scatter-patch ambient-fire semantic and the
// spell-killed lifetime in pre-release Pulse.
struct SFireCtx {
    TFireEffect* fire = nullptr;
};

void* FireSpawn(const S3DPoint& origin)
{
    auto* c = new SFireCtx();
    c->fire = TFireEffect::SpawnForTest(origin);
    if (!c->fire)
        log_warn("[vfx] TFireEffect::SpawnForTest returned null; F03 entry will draw nothing");
    return c;
}

void FireDestroy(void* cp)
{
    auto* c = static_cast<SFireCtx*>(cp);
    delete c->fire;
    delete c;
}

void FireSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFireCtx*>(cp);
    if (!c->fire)
        return;
    c->fire->TickAndSubmitForTest(dbg);
}

// --- FB: real TTeleporterEffect (procedural cool-blue glow, M09) --------
// Spawns a sector-less TTeleporterEffect at the harness-provided origin
// (which the CharacterRig resolves to the caster's CharacterRoot anchor
// each frame — see drip_hand for the canonical pattern). Drives the
// pre-release 4-phase state machine (Init→Out→Move→In) + 5-flare
// triangle-wave envelope via TickAndSubmitForTest each frame.
//
// CharacterCast preview style — the Teleporter is the Misthaven recall
// spell (spell.def line 1471 `VortexM`) plus 15 other spell variants
// (Teleport / Priest Teleport / 12 monster summons; M09_FORENSICS.md §5).
// The rig spawns Locke with a cast* / invoke* anim looping; the effect
// engulfs his feet in a rotating glow column (~4.17 s lifetime).
//
// Anchor: SVfxAnchor::CharacterRoot — caster's feet. NO hand bone (M09
// forensics §6, §7.8). The visual is the entire vertical column
// centred on the caster's body, not a hand flourish.
//
// Per M09_FORENSICS.md §6 the natural lifetime (~4.17 s) is longer than
// the default SpellGround retrigger (3.0 s); a mid-flight restart reads
// as "another teleport fires" rather than a glitch and is acceptable
// for the visual primitive. Gameflow's in-game wiring will fire the
// effect at single-cast intervals (no auto-retrigger).
struct STeleCtx {
    TTeleporterEffect* tele = nullptr;
};

void* TeleSpawn(const S3DPoint& origin)
{
    auto* c = new STeleCtx();
    c->tele = TTeleporterEffect::SpawnForTest(origin);
    if (!c->tele)
        log_warn("[vfx] TTeleporterEffect::SpawnForTest returned null; "
                 "M09 entry will draw nothing");
    return c;
}

void TeleDestroy(void* cp)
{
    auto* c = static_cast<STeleCtx*>(cp);
    delete c->tele;
    delete c;
}

void TeleTick(void* cp, EFxDebugMode /*dbg*/)
{
    auto* c = static_cast<STeleCtx*>(cp);
    if (!c->tele) return;
    c->tele->TickForTest();
}

void TeleSubmitWorld(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<STeleCtx*>(cp);
    if (!c->tele) return;
    c->tele->SubmitWorldForTest(dbg);
}

// --- FB+LS: real TFireBallEffect (F07, Magic/NewFireBall.I3D) -----------
// Spawns a sector-less single-cast TFireBallEffect at the harness origin
// via SpawnForTest and drives the ported TMissileEffect Pulse +
// TFireBallAnimator Animate/Render bodies through TickAndSubmit each
// frame. Faithful direct port — see src/effect.cpp F07 block.
//
// Two-hook wiring (mirrors TTeleporterEffect M09b): `submit` drives the
// FB-pipeline billboards (ball/glow/trail/burst/sparks) + tick + spell
// point-light; `submit_world` drives the cylinder01 shockwave ring via
// SubmitHelperMesh AFTER BeginTilePass opens. Required because helper-
// meshes land in the transparent_world_queue which BeginTilePass clears.
//
// Single non-overlapping cast (per RECONSTRUCTION_PROTOCOL + the §4.1
// "one effect on screen at a time" rule): the harness lets a full cycle
// play out (IsAlive() goes false when LAUNCH→FLY→EXPLODE finishes and
// all sub-systems are quiet), waits a clear ~1.5 s gap (a fireball's
// total visible lifetime), then re-fires ONE fresh cast. Combat preview
// style — the cadence reads as repeated casts at the same spot.
struct SFireBallCtx {
    TFireBallEffect* fireball = nullptr;
    S3DPoint         origin   = {0, 0, 0};
    float            gap      = 0.0f;
};

constexpr float kFireBallRetriggerGap = 1.5f;  // clear gap between casts (s)

void* FireBallSpawn(const S3DPoint& origin)
{
    auto* c = new SFireBallCtx();
    c->origin = origin;
    c->fireball = TFireBallEffect::SpawnForTest(origin);
    if (!c->fireball)
        log_warn("[vfx] TFireBallEffect::SpawnForTest returned null; F07 entry will draw nothing");
    return c;
}

void FireBallDestroy(void* cp)
{
    auto* c = static_cast<SFireBallCtx*>(cp);
    delete c->fireball;
    delete c;
}

void FireBallSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFireBallCtx*>(cp);
    if (!c)
        return;

    // Single non-overlapping cast — let the LAUNCH→FLY→EXPLODE cycle
    // fully play out, wait a clear gap, then re-fire ONE fresh cast.
    if (!c->fireball || !c->fireball->IsAlive())
    {
        c->gap -= float(TTime::DeltaTime());
        if (c->gap <= 0.0f)
        {
            delete c->fireball;
            c->fireball = TFireBallEffect::SpawnForTest(c->origin);
            c->gap = kFireBallRetriggerGap;
        }
    }

    if (c->fireball)
        c->fireball->TickAndSubmit(dbg);
}

void FireBallSubmitWorld(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFireBallCtx*>(cp);
    if (!c || !c->fireball)
        return;
    c->fireball->SubmitWorldRing(dbg);
}

// --- FB: real TSparkEffect (X22, Misc/Sparks.I3D) -----------------------
// Spawns a sector-less single-burst TSparkEffect at the harness origin via
// SpawnForTest and drives the ported TParticle3DAnimator loop through
// TickAndSubmitForTest each frame. Faithful direct port (not a
// TParticleBucket re-derivation) — see src/effect.cpp X22 block.
//
// Single non-overlapping burst (per the verification requirement): the
// harness lets a burst fully play out (IsAlive() goes false when the last
// spark expires), waits a clear gap, then re-fires ONE fresh burst. No
// overlapping/grouped bursts — exactly one clean instance is visible at a
// time so a stranger can verify the single-color additive glint look.
// Static preview style (the internal cadence is owned here, not by the
// harness's destroy/respawn timer).
struct SSparkCtx {
    TSparkEffect* spark   = nullptr;
    S3DPoint      origin  = {0, 0, 0};
    float         gap     = 0.0f;     // post-death cooldown before re-fire
};

constexpr float kSparkRetriggerGap = 0.8f;   // clear gap between bursts (s)

void* SparkSpawn(const S3DPoint& origin)
{
    auto* c = new SSparkCtx();
    c->origin = origin;
    c->spark = TSparkEffect::SpawnForTest(origin);
    if (!c->spark)
        log_warn("[vfx] TSparkEffect::SpawnForTest returned null; X22 entry will draw nothing");
    return c;
}

void SparkDestroy(void* cp)
{
    auto* c = static_cast<SSparkCtx*>(cp);
    delete c->spark;
    delete c;
}

void SparkSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SSparkCtx*>(cp);
    if (!c)
        return;

    // Burst has fully played out -> wait a clear gap, then re-fire ONE
    // fresh burst. This keeps exactly one clean, non-overlapping instance
    // visible for verification.
    if (!c->spark || !c->spark->IsAlive())
    {
        c->gap -= float(TTime::DeltaTime());
        if (c->gap <= 0.0f)
        {
            delete c->spark;
            c->spark = TSparkEffect::SpawnForTest(c->origin);
            c->gap   = kSparkRetriggerGap;
        }
    }

    if (c->spark)
        c->spark->TickAndSubmitForTest(dbg);
}

// --- LS: flare + dynamic point light placeholder -------------------------
struct SFlareCtx {
    float age = 0.0f;
    S3DPoint origin = {0, 0, 0};
};

void* FlareSpawn(const S3DPoint& origin)
{
    auto* c = new SFlareCtx();
    c->origin = origin;
    return c;
}
void  FlareDestroy(void* cp) { delete static_cast<SFlareCtx*>(cp); }

void FlareSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlareCtx*>(cp);
    c->age += float(TTime::DeltaTime());
    const float pulse = 0.7f + 0.3f * std::sin(c->age * 2.0f);

    // Submit a small additive billboard for the visible glow.
    const float wx = float(c->origin.x);
    const float wy = float(c->origin.y);
    const float wz = float(c->origin.z) + 80.0f;
    SBillboardDrawItem it = {};
    it.world_pos[0] = wx;
    it.world_pos[1] = wy;
    it.world_pos[2] = wz;
    it.size_wu[0]   = 50.0f * pulse;
    it.size_wu[1]   = 50.0f * pulse;
    it.color_rgba[0] = 1.0f;
    it.color_rgba[1] = 0.85f;
    it.color_rgba[2] = 0.25f;
    it.color_rgba[3] = pulse;
    it.uv_rect[0] = 0.0f; it.uv_rect[1] = 0.0f; it.uv_rect[2] = 1.0f; it.uv_rect[3] = 1.0f;
    it.key.texture     = GoldFlareTexture();
    it.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    it.key.blend       = uint8_t(EFxBlend::Additive);
    it.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    it.debug_mode      = dbg;
    Renderer->SubmitFxBillboard(it);

    // And re-add a dynamic point light at the same world position. The
    // LS pipeline coupling is "effects re-add point lights each frame";
    // ClearPointLights ran at the top of VfxTest::Render.
    Renderer->AddPointLight(wx, wy, wz, 320.0f,
                            1.0f, 0.85f, 0.25f,
                            1.2f * pulse);
}

// --- SR: TWeaponSwipe (S09 sword slash trail) ---------------------------
// Character-attached strip effect. Owned per-character in retail; here
// we instantiate one TWeaponSwipe inside the harness ctx and feed it
// the live (hilt, tip) anchor pair every frame from the rig. The
// strip ring + Catmull-Rom spline + per-vertex colour curve all live
// inside TWeaponSwipe (src/weaponswipe.cpp); this glue just resolves
// the two anchors and drives the visibility gate.
//
// **CharacterAttack** preview style — `PickInitialState` cycles to the
// first `attack*` animation on the loaded character (any character
// with one will work; Locke has multiple). The retrigger cadence
// (1.5 s) re-seeds the ring buffer between swings so the trail starts
// clean on each new attack.
//
// SCharData::swipecolor wiring — retail pulls the colour from
// chardata->swipecolor (RGB 0..255), set per-character in RULES.DEF
// (e.g. Locke = `SWIPECOLOR 0, 0, 10` → normalizes to vivid blue with
// the +0.4 hilt boost reading purplish-pink over warm backdrops).
// We read it from the rig's loaded character every submit so cycling
// characters in the harness picks up the right tint automatically.
struct SSwipeCtx {
    TWeaponSwipe* swipe = nullptr;
    int32_t       last_roster_idx = -1;  // re-read chardata only on character change
};

void* SwipeSpawn(const S3DPoint& /*origin*/)
{
    auto* c = new SSwipeCtx();
    c->swipe = new TWeaponSwipe();
    c->swipe->ResetForNewWeapon();
    // Colour deferred to first submit — chardata is only available once
    // the rig has loaded its character.
    return c;
}

void SwipeDestroy(void* cp)
{
    auto* c = static_cast<SSwipeCtx*>(cp);
    delete c->swipe;
    delete c;
}

void SwipeSubmitAttached(void* cp, EFxDebugMode dbg,
                         const VfxTest::SVfxAttachment& /*hilt_attach*/)
{
    auto* c = static_cast<SSwipeCtx*>(cp);
    if (!c->swipe) return;

    // Pull the per-character swipecolor from chardata when the rig's
    // loaded character changes. Retail's TCharAnimator::SetupWeaponSwipe
    // does this on attach; we mirror it on roster-cycle. Zero colour
    // short-circuits the trail entirely (matches retail behaviour for
    // characters without a SWIPECOLOR entry in RULES.DEF).
    SCharacterRig& rig = g_state.rig;
    if (c->last_roster_idx != rig.roster_idx)
    {
        c->last_roster_idx = rig.roster_idx;
        c->swipe->ResetForNewWeapon();
        if (auto* chr = dynamic_cast<TCharacter*>(rig.inst))
        {
            if (SCharData* cd = chr->GetCharData())
            {
                c->swipe->SetColour(float(cd->swipecolor.red)   / 255.0f,
                                    float(cd->swipecolor.green) / 255.0f,
                                    float(cd->swipecolor.blue)  / 255.0f);
            }
        }
    }

    // Resolve the (hilt, tip) anchor pair against the live weapon
    // matrix. The `hilt_attach` passed in by the dispatcher is the
    // hilt; we discard it and re-resolve both at once so a single
    // call site reads the rig state consistently (and so the tip
    // local-Z cache lives next to the lookup it gates).
    VfxTest::SVfxAttachment hilt, tip;
    const bool ok = ResolveWeaponAnchors(hilt, tip);
    if (!ok)
    {
        // Rig isn't loaded, or character has no recognised weapon
        // sub-object. Keep ring at zero — the strip never draws and
        // the next valid character/frame picks up cleanly.
        return;
    }

    // Visibility gate matches retail TCharAnimator::Render:
    //   IsAttack() && initialized && (player ? primehand != null : true)
    // The harness has no equip slots so the primehand check is a
    // no-op (the bone presence check above already covers "weapon
    // exists"). IsAttack() maps to RigIsInAttackState() above.
    const bool attack_visible = RigIsInAttackState();

    c->swipe->TickAndSubmit(hilt.world_pos, tip.world_pos,
                            attack_visible, dbg);
}

}  // namespace

// Defer registration until VfxTest::Initialize runs (renderer must
// exist for SolidColorTexture). The deferred queue is replayed at the
// top of Initialize.
namespace {
struct SVfxTestBootstrap {
    SVfxTestBootstrap() {
        VfxTest::SEffect flame = {};
        flame.id            = "TFlameEffect";
        flame.family        = "fire";
        flame.pipeline      = "FB";
        flame.preview_style = VfxTest::EVfxPreviewStyle::Static;
        flame.factory       = [](const S3DPoint& o) -> void* { return FlameSpawn(o); };
        flame.submit        = [](void* c, EFxDebugMode d) { FlameSubmit(c, d); };
        flame.destroy       = [](void* c) { FlameDestroy(c); };
        VfxTest::DeferredRegister(flame);

        VfxTest::SEffect blood = {};
        blood.id            = "TBloodEffect";
        blood.family        = "blood";
        blood.pipeline      = "FB";
        // B01 = one-shot combat directional spray. Static cadence: the
        // single-burst non-overlapping re-fire is owned inside BloodSubmit
        // (let one burst die, wait a clear gap, fire one fresh burst) so a
        // clean single instance is always visible for verification.
        blood.preview_style = VfxTest::EVfxPreviewStyle::Static;
        blood.factory       = [](const S3DPoint& o) -> void* { return BloodSpawn(o); };
        blood.submit        = [](void* c, EFxDebugMode d) { BloodSubmit(c, d); };
        blood.destroy       = [](void* c) { BloodDestroy(c); };
        VfxTest::DeferredRegister(blood);

        VfxTest::SEffect strip = {};
        strip.id            = "TStripEffect";
        strip.family        = "strip";
        strip.pipeline      = "SR";
        // Lightning bolt is combat-cadence: re-fires across the screen
        // like successive casts (the in-game cadence is roughly one
        // bolt per cast-cycle; the harness re-fire every ~0.6s reads
        // as a flurry of casts, which is fine for the SR-pipeline
        // validator visual.)
        strip.preview_style = VfxTest::EVfxPreviewStyle::Combat;
        strip.factory       = [](const S3DPoint& o) -> void* { return StripSpawn(o); };
        strip.submit        = [](void* c, EFxDebugMode d) { StripSubmit(c, d); };
        strip.destroy       = [](void* c) { StripDestroy(c); };
        VfxTest::DeferredRegister(strip);

        VfxTest::SEffect ripple = {};
        ripple.id            = "TRippleEffect";
        ripple.family        = "water";
        ripple.pipeline      = "FB";
        // Ripple = drip-into-water event: ground-level, occasional
        // re-fire at small XY jitter. Matches SpellGround cadence.
        ripple.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        ripple.factory       = [](const S3DPoint& o) -> void* { return RippleSpawn(o); };
        ripple.submit        = [](void* c, EFxDebugMode d) { RippleSubmit(c, d); };
        ripple.destroy       = [](void* c) { RippleDestroy(c); };
        VfxTest::DeferredRegister(ripple);

        VfxTest::SEffect mist = {};
        mist.id            = "TMistEffect";
        mist.family        = "ambient";
        mist.pipeline      = "PE";
        // Mist is a continuous ambient emitter — Static cadence (never
        // re-fires; cycle out/in to restart). See INVENTORY M05 §6.
        mist.preview_style = VfxTest::EVfxPreviewStyle::Static;
        mist.factory       = [](const S3DPoint& o) -> void* { return MistSpawn(o); };
        mist.submit        = [](void* c, EFxDebugMode d) { MistSubmit(c, d); };
        mist.destroy       = [](void* c) { MistDestroy(c); };
        VfxTest::DeferredRegister(mist);

        VfxTest::SEffect drip = {};
        drip.id            = "TDripEffect";
        drip.family        = "water";
        drip.pipeline      = "PE";
        // Drip is ambient environmental — the cyclic respawn happens
        // inside TickAndSubmitForTest (one drop per ~10 sec at the
        // harness's reduced period). Static cadence; cycle in/out to
        // reset. See INVENTORY H04 §6.
        drip.preview_style = VfxTest::EVfxPreviewStyle::Static;
        drip.factory       = [](const S3DPoint& o) -> void* { return DripSpawn(o); };
        drip.submit        = [](void* c, EFxDebugMode d) { DripSubmit(c, d); };
        drip.destroy       = [](void* c) { DripDestroy(c); };
        VfxTest::DeferredRegister(drip);

        VfxTest::SEffect halo = {};
        halo.id            = "THaloEffect";
        halo.family        = "light";
        halo.pipeline      = "FB+LS";
        // Halo = spell-cast ground halo (per INVENTORY L02 §5): brief
        // triangle-wave pulse, ground-anchored, ~1 sec lifetime.
        // SpellGround cadence (3 sec retrigger, small XY jitter) reads
        // as repeated spell casts at the player's feet.
        halo.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        halo.factory       = [](const S3DPoint& o) -> void* { return HaloSpawn(o); };
        halo.submit        = [](void* c, EFxDebugMode d) { HaloSubmit(c, d); };
        halo.destroy       = [](void* c) { HaloDestroy(c); };
        VfxTest::DeferredRegister(halo);

        VfxTest::SEffect fire = {};
        fire.id            = "TFireEffect";
        fire.family        = "fire";
        fire.pipeline      = "FB";
        // F03 = ambient scatter-patch fire (per INVENTORY F03 §6).
        // SpellGround re-fire cadence reads as "occasional patch flares
        // up nearby" — matches the spell-killed pre-release lifetime
        // (forensics §4) and the scatter-patch ambient semantic.
        fire.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        fire.factory       = [](const S3DPoint& o) -> void* { return FireSpawn(o); };
        fire.submit        = [](void* c, EFxDebugMode d) { FireSubmit(c, d); };
        fire.destroy       = [](void* c) { FireDestroy(c); };
        VfxTest::DeferredRegister(fire);

        // --- Character-attached proof: TDripEffect tracked to "rhand" ---
        // Validates the CharacterRig primitive end-to-end. The drip
        // already reads inst->Pos() inside its tick path each emit, so
        // the per-frame ForcePos call from submit_attached drags the
        // emit origin to wherever the character's right-hand bone is —
        // every fresh drop launches from the live hand position, then
        // falls under gravity to spawn a ground ripple at landing
        // (the H03 chain that H04 already wires).
        //
        // Reuses the existing TDripEffect::SpawnForTest entry point —
        // no effect-side changes required. Idle preview style + Locke
        // character: stand a peaceful Locke and watch water drip from
        // his palm. If the rig fails to load any character, the
        // attachment is unresolved and the effect renders at (0,0,0)
        // like the existing TDripEffect entry.
        VfxTest::SEffect drip_hand = {};
        drip_hand.id            = "TDripEffect.OnHand";
        drip_hand.family        = "water";
        drip_hand.pipeline      = "PE";
        drip_hand.preview_style = VfxTest::EVfxPreviewStyle::CharacterIdle;
        drip_hand.anchor.kind   = VfxTest::SVfxAnchor::EKind::Bone;
        std::strncpy(drip_hand.anchor.bone_name, "rhand",
                     sizeof(drip_hand.anchor.bone_name) - 1);
        drip_hand.factory = [](const S3DPoint& o) -> void* {
            return DripSpawn(o);
        };
        drip_hand.submit_attached = [](void* c, EFxDebugMode d,
                                       const VfxTest::SVfxAttachment& at) {
            auto* ctx = static_cast<SDripCtx*>(c);
            if (!ctx->drip) return;
            // Re-anchor before tick: the drip's tick path reads Pos()
            // when respawning a fresh drop, so any new drop launches
            // from the moving rhand. Existing in-flight drops continue
            // under physics (a falling drop doesn't track the hand,
            // which is the correct physical behaviour).
            if (at.resolved)
                ctx->drip->ForcePos(S3DPoint{ int32_t(at.world_pos[0]),
                                              int32_t(at.world_pos[1]),
                                              int32_t(at.world_pos[2]) });
            ctx->drip->TickAndSubmitForTest(d);
        };
        drip_hand.destroy = [](void* c) { DripDestroy(c); };
        VfxTest::DeferredRegister(drip_hand);

        // --- M09 TTeleporterEffect — Misthaven recall on a CharacterCast rig
        // The user-flagged headline VFX. Anchored on the caster's
        // CharacterRoot (feet); the effect's pre-release state machine
        // grows a rotating glow column over ~2 s, "teleports" (single
        // payload tick that gameflow hooks), shrinks back over ~2 s.
        // Total visible lifetime ~4.17 s. The harness's CharacterCast
        // rig picks a `cast*` / `invoke*` / `magic*` animation
        // automatically — for Locke this matches `invoke4` (the
        // animation spell.def Teleport line 807 specifies). See
        // docs/vfx/M09_FORENSICS.md.
        VfxTest::SEffect tele = {};
        tele.id            = "TTeleporterEffect";
        tele.family        = "spell";
        tele.pipeline      = "FB";
        tele.preview_style = VfxTest::EVfxPreviewStyle::CharacterCast;
        tele.anchor.kind   = VfxTest::SVfxAnchor::EKind::CharacterRoot;
        tele.factory       = [](const S3DPoint& o) -> void* { return TeleSpawn(o); };
        tele.submit_attached = [](void* c, EFxDebugMode d,
                                  const VfxTest::SVfxAttachment& at) {
            auto* ctx = static_cast<STeleCtx*>(c);
            if (!ctx->tele) return;
            // Re-anchor each frame: the teleport's per-frame submit
            // reads Pos() to position each of its 5 glow flares, so
            // pinning Pos() to the live CharacterRoot makes the column
            // track the caster as the rig animator moves him (cast
            // anim has minor root motion). The actual mesh draw happens
            // in submit_world below (must run after BeginTilePass clears
            // the transparent_world_queue; see M09_FORENSICS.md §M09b).
            if (at.resolved)
                ctx->tele->ForcePos(S3DPoint{ int32_t(at.world_pos[0]),
                                              int32_t(at.world_pos[1]),
                                              int32_t(at.world_pos[2]) });
            TeleTick(c, d);
        };
        tele.submit_world = [](void* c, EFxDebugMode d) {
            TeleSubmitWorld(c, d);
        };
        tele.destroy = [](void* c) { TeleDestroy(c); };
        VfxTest::DeferredRegister(tele);

        // --- SR: TWeaponSwipe — S09 sword slash trail -----------------
        // Strip ribbon traced from the live weapon hilt + tip on a
        // character playing an attack* animation. Bespoke per
        // AGENT_GUIDE §3.2.1 (per-vertex custom geometry,
        // weapon-extents-driven width, character-attached behaviour).
        // The hilt anchor is registered explicitly so the dispatcher
        // resolves SOMETHING (gives the UI panel a readable
        // "Anchor: weapon @ (...)" line); the submit lambda re-
        // resolves both hilt + tip together via ResolveWeaponAnchors.
        VfxTest::SEffect swipe = {};
        swipe.id            = "TWeaponSwipe";
        swipe.family        = "strip";
        swipe.pipeline      = "SR";
        swipe.preview_style = VfxTest::EVfxPreviewStyle::CharacterAttack;
        swipe.anchor.kind   = VfxTest::SVfxAnchor::EKind::Bone;
        // "weapon" is the primary retail name (charanimator.cpp:264);
        // FindWeaponBoneName() inside the submit lambda walks the full
        // fallback chain (weapon -> sword -> ogrokaxe). The anchor name
        // here is purely for the UI readout.
        std::strncpy(swipe.anchor.bone_name, "weapon",
                     sizeof(swipe.anchor.bone_name) - 1);
        swipe.factory         = [](const S3DPoint& o) -> void* { return SwipeSpawn(o); };
        swipe.submit_attached = [](void* c, EFxDebugMode d,
                                   const VfxTest::SVfxAttachment& at) {
            SwipeSubmitAttached(c, d, at);
        };
        swipe.destroy         = [](void* c) { SwipeDestroy(c); };
        VfxTest::DeferredRegister(swipe);

        // --- F07 TFireBallEffect — flagship offensive spell (Magic\NewFireBall.I3D)
        // Composite effect: 3-state LAUNCH→FLY→EXPLODE machine, 10-slot
        // mesh trail + 30-40 photon-spark particle trail, impact burst
        // (10 quads) + shockwave ring (cylinder01) + warm point light.
        // The ring uses SubmitHelperMesh (same as M09b teleporter) and
        // therefore MUST be submitted from submit_world (after the
        // tile pass clears transparent_world_queue). See
        // docs/vfx/forensics/F07_TFireBallEffect.md.
        VfxTest::SEffect fireball = {};
        fireball.id            = "TFireBallEffect";
        fireball.family        = "fire";
        fireball.pipeline      = "FB+LS";   // billboards + carried point light + helper-mesh ring
        // Static cadence: the single-cast non-overlapping re-fire is
        // owned inside FireBallSubmit so a clean single instance is
        // always visible for the LAUNCH→FLY→EXPLODE verification.
        fireball.preview_style = VfxTest::EVfxPreviewStyle::Static;
        fireball.factory       = [](const S3DPoint& o) -> void* { return FireBallSpawn(o); };
        fireball.submit        = [](void* c, EFxDebugMode d) { FireBallSubmit(c, d); };
        fireball.submit_world  = [](void* c, EFxDebugMode d) { FireBallSubmitWorld(c, d); };
        fireball.destroy       = [](void* c) { FireBallDestroy(c); };
        VfxTest::DeferredRegister(fireball);

        VfxTest::SEffect spark = {};
        spark.id            = "TSparkEffect";
        spark.family        = "blood";   // combat-feedback family (forensics §0)
        spark.pipeline      = "FB";
        // X22 = one-shot combat block-spark burst. Static cadence: the
        // single-burst non-overlapping re-fire is owned inside SparkSubmit
        // (let one burst die, wait a clear gap, fire one fresh burst) so a
        // clean single instance is always visible for verification.
        spark.preview_style = VfxTest::EVfxPreviewStyle::Static;
        spark.factory       = [](const S3DPoint& o) -> void* { return SparkSpawn(o); };
        spark.submit        = [](void* c, EFxDebugMode d) { SparkSubmit(c, d); };
        spark.destroy       = [](void* c) { SparkDestroy(c); };
        VfxTest::DeferredRegister(spark);

        VfxTest::SEffect flare = {};
        flare.id            = "TFlareAnimator.placeholder";
        flare.family        = "light";
        flare.pipeline      = "LS";
        // Flare = spell-cast burst: ground-level, occasional re-fire.
        flare.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        flare.factory       = [](const S3DPoint& o) -> void* { return FlareSpawn(o); };
        flare.submit        = [](void* c, EFxDebugMode d) { FlareSubmit(c, d); };
        flare.destroy       = [](void* c) { FlareDestroy(c); };
        VfxTest::DeferredRegister(flare);
    }
};
SVfxTestBootstrap g_vfx_test_bootstrap;
}  // namespace
