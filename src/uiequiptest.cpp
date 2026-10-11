// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiequiptest.cpp - --test=ui-equip                                    *
// *************************************************************************
//
// Clean-room reconstruction of TEquipPane (= retail cls_0x5a55dc, the
// EquipSidebarPane — NOT cls_0x54dd40, which the yaml false-positively
// matched; see EquipPane_SPEC.md §0 mis-target note). Built ONLY from
// docs/ui/forensics/EquipPane_SPEC.md with FORENSICS_PROTOCOL,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL.
//
// What this pane draws (spec §1 / §2 / §4 / §5):
//   - Chrome `Equip` 188x306 (equippane.dat[0], BM_15BIT magenta-key opaque)
//     stamped at pane-local (0,0). The chrome bakes 11 slot wells, a doll
//     silhouette, and a gold compass-rose base.
//   - For each of 11 EQ_* slots: if equipped, blit the item's
//     oi->InventoryImage() at slot[i].(x,y), DM_TRANSPARENT|DM_BACKGROUND;
//     if empty, blit the slot's named placeholder pictogram
//     (Head/Neck/Chest/Hand2/Hand/Ring/Ring2/Range/Ammo/Legs/Feet) at the
//     same anchor, DM_TRANSPARENT.
//   - Per-slot (x,y) anchors come from the Wave-6 decoded table
//     DAT_005e3f60 = 11 × {int x, int y} in EQ_* enum order (spec §4 table).
//
// Architecture (spec §3 direct-renderer contract): compose the WHOLE pane
// (chrome + 11 slot placeholders/icons) into ONE offscreen TSurface RT
// 188×306 via *ToTarget primitives, then DrawSurface it once in the HUD
// pass at screen (452, 0). Mirrors uisidetabstest — the canonical
// compose-to-target contract.
//
// Contents (spec §5): the main player's equipment. Each well shows
// Player->GetEquip(i) through the shared TInvSlot (static `invitem` or
// animated `invanim` icons, src/3dimage.cpp:362-371), or the slot's
// placeholder pictogram when empty (spec §6.3, the retail-specific
// addition). The paperdoll is the same player wearing the same items, so
// the wells and the body always agree. The --test=ui-equip host supplies a
// demo player. Body-drag rotation is wired through the shared testmode
// input path so scripted captures can exercise the paperdoll interaction.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — opaque chrome stamp
//   TInvSlot::Draw                        — icon/placeholder into the
//                                          40×40 slot wells
//   Renderer->CompositeLitTargetSubrectToTarget — paperdoll body sub-rect
//                                          (added for the 3D Locke path, #11)
// No hand-rolled chroma-key passes, no procedural silhouette stand-ins.
//
// Live 3D paperdoll (#11, #13 — landed):
//   The pane draws the main player's model wearing its equipment, and:
//   1. Poses it from the pane's own state (retail EquipPane +0xbc, slot
//      19), never the player's: the body in the world keeps its state.
//   2. Uses the recovered slot-19 animation string (DAT_005e4060 = "walk")
//      instead of cycling winv/binv gesture exports, which produce the
//      raised/crossed-arm poses seen in the broken paperdoll.
//   3. Renders the 3D body through a retail D3D-viewport mesh projection
//      into a full 188x306 pane-sized sub-rect of lit_target.
//   4. Composites that lit_target sub-rect 1:1 into the equip pane RT on
//      top of the chrome stone backdrop.
//   Present suppression is scoped to the paperdoll render call so the
//   same pane can be embedded in the full HUD / live game safely.
//
// Equipment-on-skeleton (#12 — landed for this sokol test path):
//   The retail equipment-replacement system lives in
//   src/charanimator.cpp::HideCharParts + RenderEquipment. It walks
//   Player->GetEquip(i), for each occupied slot finds the BodyType-
//   matched state in the equipment's .i3d, marks the player's matching
//   sub-objects OBJ3D_HIDE, then renders the equipment's sub-objects
//   in their place via Scene3D.RenderObject. The sokol port mirrors that
//   by extracting BodyType-matched equipment meshes, skipping replaced
//   player sub-objects, and submitting the equipment with the replaced
//   player sub-object's animated matrix.
//
// *************************************************************************

#include "uiequiptest.h"

#include "3dimage.h"
#include "animation.h"
#include "bitmap.h"
#include "bitmapatlas.h"
#include "character.h"
#include "display.h"
#include "i3danimpose.h"
#include "imagery.h"
#include "invslot.h"
#include "logging.h"
#include "math3d.h"
#include "meshextract.h"
#include "multi.h"
#include "object.h"
#include "player.h"  // EQ_HEAD / EQ_NECK / ... NUM_EQ_SLOTS
#include "renderer.h"
#include "revdefs.h"
#include "surface.h"
#include "time.h"
#include "uidragstate.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

// =====================================================================
// Geometry constants — all cited to EquipPane_SPEC.md.
// =====================================================================

// --- pane rect (spec §3 "Pane on screen") ----------------------------
// Derived from the sibling Stats ctor (same upper-region slot) at
// cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp:6-21 AND cross-checked
// against the measured chrome bitmap dimensions in equippane.dat[0] —
// both equal 188×306. The EquipPane inlined ctor at 0x4807a0 writes the
// same constants (EquipSidebarPane.md:28). The pane's right edge is
// 452+188 = 640 (the screen right edge); top edge y=0; bottom y=306.
constexpr int32_t kPaneX  = 0x1c4;   // 452 (§3) — pane TL x in screen
constexpr int32_t kPaneY  = 0x000;   //   0 (§3) — pane TL y in screen
constexpr int32_t kPaneW  = 0x0bc;   // 188 (§3) — chrome width
constexpr int32_t kPaneH  = 0x132;   // 306 (§3) — chrome height

// --- slot well size (spec §4 "per-slot") -----------------------------
// All 11 wells are 40×40 — matches every empty-slot bitmap WxH measured
// in equippane.dat AND the INVITEMREALWIDTH/HEIGHT=40 constant (snapshot
// src/revdefs.h:131-132). Hit-rects in spec §10 use the same 40×40.
constexpr int32_t kSlotW = 40;       // §4 / §10
constexpr int32_t kSlotH = 40;       // §4 / §10

// --- paperdoll body viewport (pane-local) — UNUSED at present -------
// Reserved for the live 3D body render. Not consumed today; see the
// ENGINE/ASSET WALL note in RenderUIEquipMode() for why the 3D body
// path is out of scope for this reconstruction. The chrome's painted-
// doll silhouette in the center column (x≈50..140, y≈30..250) is what
// reads as "the body" in the current visual.

// --- per-slot anchor table (spec §4 Wave-6) --------------------------
// 11 × (int x, int y) in EQ_* enum order, decoded from DAT_005e3f60.
// HIGH-confidence on (x, y) values AND on EQ slot assignment — every
// entry matches a unique visual chrome well (frame-inset Δx∈[3,9],
// Δy∈[0,10] in §4 Wave-6 table). The L/R chirality follows the
// paperdoll-viewed-face-on mirror convention: PRIMEHAND (char's right
// hand) sits at viewer's LEFT.
//
// EQ_* enum values are from src/player.h:21-31 (NUM_EQ_SLOTS=11):
//   0=EQ_HEAD, 1=EQ_NECK, 2=EQ_BODY, 3=EQ_OFFHAND, 4=EQ_PRIMEHAND,
//   5=EQ_R_ACCESSORY, 6=EQ_L_ACCESSORY, 7=EQ_RANGEDWEAPON, 8=EQ_AMMO,
//   9=EQ_LEGS, 10=EQ_FEET.
struct SSlotAnchor { int32_t x; int32_t y; };
constexpr SSlotAnchor kSlotAnchor[11] = {
    { 0x4a, 0x25 },  // 0 EQ_HEAD         (74,  37) — top-center
    { 0x07, 0x3a },  // 1 EQ_NECK         (7,   58) — row 2 LEFT
    { 0x8d, 0x3a },  // 2 EQ_BODY/Chest   (141, 58) — row 2 RIGHT
    { 0x8d, 0x6a },  // 3 EQ_OFFHAND      (141, 106) — row 3 RIGHT
    { 0x07, 0x6a },  // 4 EQ_PRIMEHAND    (7,   106) — row 3 LEFT (mirror)
    { 0x07, 0x9a },  // 5 EQ_R_ACCESSORY  (7,   154) — row 4 LEFT (mirror)
    { 0x8d, 0x9a },  // 6 EQ_L_ACCESSORY  (141, 154) — row 4 RIGHT
    { 0x18, 0x0b },  // 7 EQ_RANGEDWEAPON (24,  11) — shoulder-top LEFT
    { 0x7c, 0x0b },  // 8 EQ_AMMO         (124, 11) — shoulder-top RIGHT
    { 0x07, 0xca },  // 9 EQ_LEGS         (7,   202) — row 5 LEFT
    { 0x8d, 0xca },  // 10 EQ_FEET        (141, 202) — row 5 RIGHT
};

// Slot-name table (spec §2 / §6.3): name in equippane.dat for each EQ_*
// enum value's placeholder pictogram. Lookup order matches Initialize's
// name-keyed walk of PTR_DAT_005e3fb8 (cls_0x46d6b0.cpp:50-90) — so the
// per-slot mbr offset is mbr_0x68 + slot * 4 (§4 chain HIGH-confidence).
constexpr const char* kPlaceholderName[11] = {
    "Head",   // 0 EQ_HEAD         §2
    "Neck",   // 1 EQ_NECK         §2
    "Chest",  // 2 EQ_BODY         §2
    "Hand2",  // 3 EQ_OFFHAND      §2 (off-hand)
    "Hand",   // 4 EQ_PRIMEHAND    §2 (primary hand)
    "Ring",   // 5 EQ_R_ACCESSORY  §2
    "Ring2",  // 6 EQ_L_ACCESSORY  §2
    "Range",  // 7 EQ_RANGEDWEAPON §2
    "Ammo",   // 8 EQ_AMMO         §2
    "Legs",   // 9 EQ_LEGS         §2
    "Feet",   // 10 EQ_FEET        §2
};

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kArchive    = "equippane.dat";
constexpr const char* kChromeName = "Equip";   // §2 idx 0 — 188×306 BM_15BIT

TMulti*  g_equipDat   = nullptr;
PTBitmap g_chrome     = nullptr;
PTBitmap g_placeholders[11] = { nullptr };

TSurface* g_pane = nullptr;

// =====================================================================
// Character body (paperdoll subject) — Locke, rendered LIVE as the 3D
// character model. Mirrors the --test=char3d render path (testmodes.cpp
// :862-1141): spawn TObjectInstance, walk T3DImagery sub-objects through
// ExtractSubMeshTextureSlot, RegisterMesh each, then per-frame submit a
// SMeshSubmit per mesh handle with the BuildAnimPoseObjectMatrix world
// transform. Per-frame tile-pass + lighting-pass run with a small camera
// origin (kBody3DSrc{X,Y,W,H} in screen space) so Locke projects into a
// fixed sub-rect of lit_target; the equip pane RT then samples that
// sub-rect via Renderer->CompositeLitTargetSubrectToTarget so the live
// Locke draws on top of the chrome stone backdrop.
//
// Pose: the pane's own state, never the player's. Retail slot 19 copies
// DAT_005e4060 into the pane's animation buffer; the recovered data
// resolves that string as "walk".
//
// Empty placeholder until the imagery streamer has loaded the .i3d body
// (async, mirrors uiplyrstatusbartest::TryExtractPortrait): per-frame
// TryBuildBodyMeshes retries until the sub-objects are decodable.
// =====================================================================
// The meshes below are built for one player, its equipment and its bow
// mode; BindBody rebuilds them when any of those changes.
TSafeRef<TPlayer>         g_bodyPlayer;
TSafeRef<TObjectInstance> g_bodyEquip[NUM_EQ_SLOTS];
bool                      g_bodyBowMode = false;
// The paperdoll's pose: the first frame of this state, independent of the
// player's state in the world. Retail slot 19 (EquipPane_SPEC §9) steps an
// idle animation chosen through the character's vtable +0x131, which isn't
// identified yet, so the pane holds the recovered state's first frame.
int32_t                   g_bodyState = -1;
T3DImagery*      g_body3DImg   = nullptr;  // dynamic_cast<T3DImagery*> of imagery
TTextureHandle   g_bodyFallbackAlbedo = kInvalidTexture;

struct SBodyMesh
{
    MeshHandle handle  = 0;
    int32_t    objnum  = -1;
    int32_t    texslot = -1;
};
std::vector<SBodyMesh> g_bodyMeshes;
float          g_bodyBBoxMin[3] = { 0, 0, 0 };
float          g_bodyBBoxMax[3] = { 0, 0, 0 };
float          g_bodyScale      = 1.0f;
bool           g_bodyMeshesBuilt = false;

// #12 — Per-equipment sub-mesh registry. One entry per equipment sub-
// object that should attach to the player's skeleton (mirrors retail
// charanimator.cpp::ProcessEquipment / RENDEREQUIPPARTS branch).
//
// Fields:
//   handle       = registered mesh extracted from the EQUIPMENT'S
//                  T3DImagery (ExtractSubMeshTextureSlot of equipObjnum
//                  in equipState).
//   equipImg     = the equipment's T3DImagery (kept so the texture
//                  resolution can re-run in case the equipped item's
//                  imagery is hot-swappable).
//   equipObjnum  = which sub-object of the equipment we extracted from.
//   equipState   = the BodyType-matched state (or "still"/"all" group
//                  state) we extracted in.
//   playerObjnum = the player sub-object whose animated world matrix
//                  this equipment mesh rides — i.e. the bone matrix
//                  (the weapon bone for a sword, the head bone for a
//                  helmet, …). retail charanimator.cpp:303 memcpy's
//                  this matrix into the equipobj before RenderObject.
//                  The sokol port computes the matrix from the player's
//                  pose via BuildAnimPoseObjectMatrix(playerObjnum).
struct SEquipMesh
{
    MeshHandle  handle       = 0;
    T3DImagery* equipImg     = nullptr;
    int32_t     equipObjnum  = -1;
    int32_t     equipState   = -1;
    int32_t     playerObjnum = -1;
};
std::vector<SEquipMesh> g_equipMeshes;

// Which player sub-object indices to SKIP when extracting Locke's
// meshes (they're replaced by equipment sub-objects). Populated
// alongside g_equipMeshes when equipment is processed. Mirrors the
// HIDECHARPARTS branch in retail ProcessEquipment.
std::vector<int32_t> g_hiddenPlayerObjnums;

// Idle animation state cycle (#13). Locke's .i3d carries 5 dedicated
// "walk inventory" idle poses (`winv1..winv5`) — verified by dumping all
// 352 state names from the loaded imagery. These are the named retail
// equip-pane idle behaviours (weight shift, stretch, look around,
// etc.); each one is a short looped animation. We probe FindState for
// each name and cycle on a wall-clock timer.
//
// `binv1..binv5` are the bow-equipped variants (when Locke is in bow
// mode); we include them as fallback in case the spawn lands in bow
// mode. `walk` is the universal root fallback if no inv-pose exists.
constexpr const char* kIdleStateCandidates[] = {
    // Static / neutral stand candidates probed FIRST -- any of these
    // would be the canonical "paperdoll resting pose" if Locke's .i3d
    // has them. Most won't exist; the loop drops the misses.
    "still", "stand", "wstand", "bstand", "ready", "wready", "main",
    "all", "none", "default", "neutral", "rest", "wrest", "brest",
    "pose", "wpose", "bpose", "stand1", "stand2", "winv0", "binv0",
    // winv*/binv* (the SPEC §9 named retail equip idle cycle) come next.
    "winv1", "winv2", "winv3", "winv4", "winv5",
    "binv1", "binv2", "binv3", "binv4", "binv5",
    "walk",  // last-ditch fallback — root locomotion cycle
};
constexpr int32_t kIdleStateCandidateCount =
    int32_t(sizeof(kIdleStateCandidates) / sizeof(kIdleStateCandidates[0]));

bool g_hudVisible = true;

void ReleaseBodyMeshRefs()
{
    if (Renderer)
    {
        for (const auto& sub : g_bodyMeshes)
            if (sub.handle) Renderer->ReleaseMeshAssetRef(sub.handle, 1);
        for (const auto& eq : g_equipMeshes)
            if (eq.handle) Renderer->ReleaseMeshAssetRef(eq.handle, 1);
    }
    g_bodyMeshes.clear();
    g_equipMeshes.clear();
    g_hiddenPlayerObjnums.clear();
    g_bodyMeshesBuilt = false;
}

void AddHiddenPlayerObjnum(int32_t objnum)
{
    if (objnum < 0) return;
    for (int32_t hidden : g_hiddenPlayerObjnums)
        if (hidden == objnum) return;
    g_hiddenPlayerObjnums.push_back(objnum);
}

const char* StripI3DVisibilityMarker(const char* name)
{
    if (!name) return "";
    while (*name == '*') ++name;
    return name;
}

const char* EquipmentMatchName(const char* equipName,
                               const char* bodyType,
                               int32_t bodyTypeLen)
{
    const char* matchName = StripI3DVisibilityMarker(equipName);
    if (bodyTypeLen > 0 && bodyType &&
        !strncasecmp(matchName, bodyType, bodyTypeLen))
        matchName += bodyTypeLen;
    return matchName;
}

// Retail slot-19 copies the fixed string at DAT_005e4060 into the pane's
// animation-name buffer; the data dump resolves that string as "walk".
// Keep the paperdoll on that recovered base state instead of cycling the
// exported winv/binv inventory gestures, which includes the distracting
// arms-raised / arms-crossed poses reported in the current pane.
constexpr const char* kRecoveredPaperdollState = "walk";

// Retail FUN_00536bc0 builds the paperdoll root matrix directly:
// Scale(2,2,2), RotateZ(pi), RotateY(0), RotateX(-1.8707963228),
// Translate(0,-75,200), with identity view and the fixed T3DScene
// projection. Mouse drag adds only an extra yaw offset on top of that base.
constexpr float kDefaultFacingUnits = 0.0f;
constexpr float kRotateUnitsPerPixel = 0.35f;
float   g_bodyFacingUnits = kDefaultFacingUnits;
bool    g_bodyDragRotating = false;
int32_t g_bodyDragStartX = 0;
float   g_bodyDragStartFacingUnits = kDefaultFacingUnits;

// The retail live paperdoll uses the full EquipmentPane rect as its D3D
// viewport: pane-local (0,0,188,306). We render that same full viewport into
// a transparent lit-target sub-rect, then composite it 1:1 over the pane RT.
constexpr int32_t kBody3DSrcX = 16;   // off-screen-edge buffer in lit_target
constexpr int32_t kBody3DSrcY = 16;
constexpr int32_t kBody3DSrcW = kPaneW;
constexpr int32_t kBody3DSrcH = kPaneH;

constexpr int32_t kBodyDstX = 0;
constexpr int32_t kBodyDstY = 0;
constexpr int32_t kBodyDstW = kPaneW;
constexpr int32_t kBodyDstH = kPaneH;

// Mouse rotation begins from the central figure area, not from the equipment
// wells around the chrome.
constexpr int32_t kBodyHitX = 48;
constexpr int32_t kBodyHitY = 78;
constexpr int32_t kBodyHitW = 92;
constexpr int32_t kBodyHitH = 166;

SInvSlotStyle g_equipSlotStyle;    // no amounts or pouches on the paperdoll
TInvSlot* g_equipSlots[NUM_EQ_SLOTS] = {};

void DestroyEquipSlots()
{
    for (int32_t i = 0; i < NUM_EQ_SLOTS; ++i)
    {
        delete g_equipSlots[i];
        g_equipSlots[i] = nullptr;
    }
}

void BuildEquipSlots()
{
    DestroyEquipSlots();
    for (int32_t i = 0; i < NUM_EQ_SLOTS; ++i)
    {
        g_equipSlots[i] = new TInvSlot(kSlotAnchor[i].x,
                                       kSlotAnchor[i].y,
                                       kSlotW,
                                       kSlotH,
                                       /*allowed_type*/ i,
                                       /*placeholder*/  g_placeholders[i],
                                       /*style*/        &g_equipSlotStyle);
    }
}

// =====================================================================
// Asset lookup helper — same shape as the other ui*test panes.
// =====================================================================
PTBitmap LookupByName(TMulti* m, const char* name)
{
    if (!m || !name) return nullptr;
    for (int32_t i = 0; i < m->numoffsets; ++i)
    {
        const char* nm = (const char*)m->names[i].ptr();
        if (nm && !std::strcmp(nm, name))
            return m->Bitmap(i);
    }
    return nullptr;
}

// The body's player, or null when it has none to draw.
TPlayer* Body()
{
    return g_bodyPlayer.Get();
}

// Follow the main player: when the player, its equipment or its bow mode
// changed since the meshes were built, drop them so TryBuildBodyMeshes
// rebuilds them (and re-picks the pose state for a new player).
void BindBody()
{
    TPlayer* player = Player;
    bool changed = (g_bodyPlayer.Get() != player);
    if (changed)
    {
        g_bodyPlayer = player;
        g_body3DImg  = nullptr;
        g_bodyState  = -1;
    }
    for (int32_t i = 0; player && i < NUM_EQ_SLOTS; ++i)
        if (g_bodyEquip[i].Get() != player->GetEquip(i))
        {
            g_bodyEquip[i] = player->GetEquip(i);
            changed = true;
        }
    const bool bowMode = player && player->IsBowMode();
    if (bowMode != g_bodyBowMode)
    {
        g_bodyBowMode = bowMode;
        changed = true;
    }
    if (changed)
        ReleaseBodyMeshRefs();
}

float NormalizeFacingUnits(float units)
{
    units = std::fmod(units, 256.0f);
    if (units < 0.0f) units += 256.0f;
    return units;
}

void SetBodyFacingUnits(float units)
{
    g_bodyFacingUnits = NormalizeFacingUnits(units);
}

bool ScreenToPaneLocal(int32_t sx, int32_t sy, int32_t& lx, int32_t& ly)
{
    const int32_t dw = Display.Width();
    const int32_t pane_x = (dw > 0 ? dw : (kPaneX + kPaneW)) - kPaneW;
    lx = sx - pane_x;
    ly = sy - kPaneY;
    return lx >= 0 && lx < kPaneW && ly >= 0 && ly < kPaneH;
}

bool IsPaperdollBodyPoint(int32_t sx, int32_t sy)
{
    int32_t lx = 0, ly = 0;
    if (!ScreenToPaneLocal(sx, sy, lx, ly))
        return false;
    return lx >= kBodyHitX && lx < kBodyHitX + kBodyHitW &&
           ly >= kBodyHitY && ly < kBodyHitY + kBodyHitH;
}

// Build mesh handles for every visible sub-object of the body's .i3d
// imagery, one RegisterMesh call per (objnum, texslot) pair. Mirrors the
// char3d testmode pattern (testmodes.cpp:923-1019). Imagery is async-
// loaded after OnScreen, so this retries each frame until the imagery
// is ready (NumStates > 0, NumObjects > 0) — same shape as
// uiplyrstatusbartest::TryExtractPortrait.
//
// Also computes the world-space bounding box of the body so the camera
// reconstruction params can frame Locke automatically inside the body
// rect (kBody3DSrc{W,H}).
// #12 — Port of charanimator.cpp::ProcessEquipment to the sokol mesh-
// submit path. Walks Player->GetEquip(i), and for each equipped item
// whose imagery is OBJIMAGE_MESH3D:
//   1. Find the equipment state matching the player's BodyType (or the
//      "still"/"all" universal state if the equipment has no BodyType
//      variant).
//   2. For each non-hidden sub-object in that state, look up the
//      corresponding player sub-object — name-matched, with BodyType
//      prefix stripped where applicable. For weapons (PRIMEHAND): the
//      player sub-object is "weapon" or "sword" depending on which
//      exists.
//   3. Register the equipment's sub-mesh via ExtractSubMeshTextureSlot
//      and remember which PLAYER sub-object's animated matrix it should
//      ride.
//   4. Mark the player sub-object as hidden so TryBuildBodyMeshes will
//      skip it (the equipment is replacing it).
//
// This must run AFTER the body imagery is loaded (so g_body3DImg is
// valid) and BEFORE TryBuildBodyMeshes (so the hidden list is in place
// before the body extract).
//
// Returns true if processing actually happened (imagery + player ready),
// false if it should be retried next frame (e.g. equipment imagery still
// streaming in). Idempotent on partial success — entries are only added
// for items whose imagery has loaded; remaining items are picked up on
// subsequent calls.
bool ProcessEquipmentForSkeleton()
{
    TPlayer* player = Body();
    if (!g_body3DImg || !player) return false;

    // Player must have a BodyType for the equipment to match.
    const char* bodyType = player->BodyType();
    if (!bodyType || !*bodyType) return false;
    const int32_t bodyTypeLen = int32_t(std::strlen(bodyType));
    TCharacter* character = player;

    // Retail TCharAnimator::Render starts by hiding the placeholder
    // "sword"/"weapon" player objects; equipment can still render on the
    // same matrix later, but the baked-in placeholder is not drawn.
    AddHiddenPlayerObjnum(g_body3DImg->GetObjectNum((char*)"sword"));
    AddHiddenPlayerObjnum(g_body3DImg->GetObjectNum((char*)"weapon"));

    for (int32_t eq = 0; eq < NUM_EQ_SLOTS; ++eq)
    {
        TObjectInstance* oi = player->GetEquip(eq);
        if (!oi) continue;

        TObjectImagery* equipImagery = oi->GetImagery();
        if (!equipImagery) continue;
        // header->imageryid is the OBJIMAGE_* type (1 = MESH3D); not to
        // be confused with the imagery's entry-array index returned by
        // ImageryId() (which is a unique handle, not a type tag).
        // Mirrors retail charanimator.cpp:214.
        SImageryHeader* hdr = equipImagery->GetHeader();
        if (!hdr) continue;
        if (hdr->imageryid != OBJIMAGE_MESH3D)
        {
            log_info("[ui-equip] equip slot %d: imagery is not MESH3D (header "
                     "type=%d) — no skeleton render for this item",
                     eq, hdr->imageryid);
            continue;
        }

        T3DImagery* equip3D = (T3DImagery*)equipImagery;
        if (equip3D->NumStates() <= 0 || equip3D->NumObjects() <= 0)
            continue;  // Imagery still streaming.

        // Find the equipment state matching the player's BodyType
        // (mirrors charanimator.cpp:222-238).
        int32_t equipState = -1;
        int32_t nlen = 0;  // BodyType prefix length to strip from object names
        for (int32_t st = 0; st < equip3D->NumStates(); ++st)
        {
            const char* sn = equipImagery->GetAniName(st);
            if (!sn) continue;
            if (!strcasecmp(sn, "still") || !strcasecmp(sn, "all"))
            {
                equipState = st;
                nlen = 0;
                break;
            }
            if (!strcasecmp(sn, bodyType))
            {
                equipState = st;
                nlen = bodyTypeLen;
                break;
            }
        }
        if (equipState < 0)
        {
            // Some retail equipment packs the body type only in object
            // names (`*normalhead`, `*normalchest`, ...), with generic
            // state names. The retail replacement logic is still
            // name-driven, so fall back to the first state that exposes a
            // visible object for this BodyType.
            for (int32_t st = 0; st < equip3D->NumStates() && equipState < 0; ++st)
            {
                for (int32_t o = 0; o < equip3D->NumObjects(); ++o)
                {
                    if (equip3D->IsHidden(o, st)) continue;
                    const char* rawName = equip3D->GetObjectName(o);
                    const char* stripped = StripI3DVisibilityMarker(rawName);
                    if (!strncasecmp(stripped, bodyType, bodyTypeLen))
                    {
                        equipState = st;
                        nlen = bodyTypeLen;
                        log_info("[ui-equip] equip slot %d: using state %d "
                                 "via object bodytype prefix '%s'",
                                 eq, st, rawName ? rawName : "?");
                        break;
                    }
                }
            }
            if (equipState < 0)
            {
                log_info("[ui-equip] equip slot %d: no body-type-matched state "
                         "or visible object prefix in imagery (bodytype='%s')",
                         eq, bodyType);
                continue;
            }
        }

        const int32_t equipTexslots = equip3D->NumTextures() + 1;

        for (int32_t o = 0; o < equip3D->NumObjects(); ++o)
        {
            if (equip3D->IsHidden(o, equipState)) continue;

            char* equipName = equip3D->GetObjectName(o);
            if (!equipName) continue;

            // Strip the I3D editor visibility marker first, then strip
            // BodyType prefix if present (charanimator.cpp:253-254).
            const char* matchName = EquipmentMatchName(equipName, bodyType, bodyTypeLen);

            // Find player sub-object to replace.
            int32_t playerObjnum = -1;
            if (eq == EQ_PRIMEHAND)
            {
                // Retail gate: prime-hand geometry renders only outside
                // bow mode, then rides "weapon"/"sword" if present.
                if (character && !character->IsBowMode())
                {
                    playerObjnum = g_body3DImg->GetObjectNum((char*)"weapon");
                    if (playerObjnum < 0)
                        playerObjnum = g_body3DImg->GetObjectNum((char*)"sword");
                    if (playerObjnum < 0)
                        playerObjnum = g_body3DImg->GetObjectNum((char*)matchName);
                }
            }
            else if (eq == EQ_RANGEDWEAPON)
            {
                // Retail gate: ranged geometry renders only in bow mode.
                if (character && character->IsBowMode())
                {
                    playerObjnum = g_body3DImg->GetObjectNum((char*)"weapon");
                    if (playerObjnum < 0)
                        playerObjnum = g_body3DImg->GetObjectNum((char*)"sword");
                    if (playerObjnum < 0)
                        playerObjnum = g_body3DImg->GetObjectNum((char*)matchName);
                }
            }
            else
            {
                playerObjnum = g_body3DImg->GetObjectNum((char*)matchName);
            }

            if (playerObjnum < 0)
                continue;  // No bone for this part — skip silently.

            // Extract this equipment sub-object's geometry. Iterate
            // texture slots like the body extract does.
            bool extracted_any = false;
            for (int32_t texslot = 0; texslot < equipTexslots; ++texslot)
            {
                std::vector<SMeshVertex> verts;
                std::vector<uint16_t> indices;
                if (!ExtractSubMeshTextureSlot(equip3D, o, texslot, verts, indices))
                    continue;

                TTextureHandle albedo = g_bodyFallbackAlbedo;
                if (texslot > 0)
                {
                    S3DTex tex = {};
                    equip3D->GetTexture(texslot - 1, &tex);
                    if (tex.htexture != kInvalidTexture)
                        albedo = tex.htexture;
                }
                MeshHandle h = Renderer->RegisterMesh(
                    verts.data(), int32_t(verts.size()),
                    indices.data(), int32_t(indices.size()),
                    albedo);
                if (!h) continue;

                g_equipMeshes.push_back({h, equip3D, o, equipState, playerObjnum});
                extracted_any = true;
            }
            if (!extracted_any)
            {
                // Single-texture object fallback (mirrors body extract).
                std::vector<SMeshVertex> verts;
                std::vector<uint16_t> indices;
                if (ExtractSubMesh(equip3D, o, verts, indices))
                {
                    TTextureHandle albedo = g_bodyFallbackAlbedo;
                    if (equip3D->NumTextures() > 0)
                    {
                        S3DTex tex = {};
                        equip3D->GetTexture(0, &tex);
                        if (tex.htexture != kInvalidTexture)
                            albedo = tex.htexture;
                    }
                    MeshHandle h = Renderer->RegisterMesh(
                        verts.data(), int32_t(verts.size()),
                        indices.data(), int32_t(indices.size()),
                        albedo);
                    if (h)
                    {
                        g_equipMeshes.push_back({h, equip3D, o, equipState, playerObjnum});
                        extracted_any = true;
                    }
                }
            }

            if (extracted_any)
            {
                // Mark the player sub-object as hidden — the equipment
                // is taking its place.
                AddHiddenPlayerObjnum(playerObjnum);

                log_info("[ui-equip] equip slot %d sub-obj %d ('%s') → "
                         "player bone %d ('%s'); player obj hidden",
                         eq, o, equipName, playerObjnum,
                         g_body3DImg->GetObjectName(playerObjnum));
            }
        }
    }

    return true;
}

// Check whether ALL Player-equipped items have their imagery loaded.
// Equipment imagery streams in async (separately from the body); the
// build-body-meshes step depends on the equipment being ready so the
// hide+attach logic can run in one pass.
bool AreEquipmentImageriesReady()
{
    TPlayer* player = Body();
    if (!player) return true;  // No player → nothing to wait on.
    for (int32_t eq = 0; eq < NUM_EQ_SLOTS; ++eq)
    {
        TObjectInstance* oi = player->GetEquip(eq);
        if (!oi) continue;
        TObjectImagery* im = oi->GetImagery();
        if (!im) return false;
        SImageryHeader* hdr = im->GetHeader();
        if (!hdr) return false;
        // Some items use non-MESH3D imagery (2D-only icons); those don't
        // contribute to the skeleton — they just need to exist.
        if (hdr->imageryid != OBJIMAGE_MESH3D) continue;
        T3DImagery* t3d = (T3DImagery*)im;
        if (t3d->NumStates() <= 0 || t3d->NumObjects() <= 0)
            return false;
    }
    return true;
}

void TryBuildBodyMeshes()
{
    if (g_bodyMeshesBuilt) return;
    TPlayer* body = Body();
    if (!body || !Renderer) return;
    if (g_bodyFallbackAlbedo == kInvalidTexture)
        g_bodyFallbackAlbedo = Renderer->WhiteTextureHandle();

    if (!g_body3DImg)
        g_body3DImg = dynamic_cast<T3DImagery*>(body->GetImagery());
    if (!g_body3DImg) return;
    if (g_body3DImg->NumStates() <= 0 || g_body3DImg->NumObjects() <= 0) return;

    // Wait for ALL equipped items' imagery to load before extracting
    // (#12) — the equipment processing depends on the items' imagery
    // being ready to extract sub-meshes from.
    if (!AreEquipmentImageriesReady()) return;

    // #12: process equipped items FIRST so the hidden-player-objnum list
    // is populated before we extract Locke's submeshes. Equipment meshes
    // are accumulated in g_equipMeshes; player sub-objects that are being
    // replaced are added to g_hiddenPlayerObjnums.
    ProcessEquipmentForSkeleton();

    // The paperdoll's own state: the recovered "walk" (retail slot 19,
    // DAT_005e4060), never the state the player is in; a stripped-down
    // character without one stands in its default root.
    if (g_bodyState < 0)
    {
        g_bodyState = body->FindState((char*)kRecoveredPaperdollState);
        if (g_bodyState < 0)
            g_bodyState = body->FindState((char*)body->DefaultRootState());
        if (g_bodyState < 0)
            g_bodyState = 0;
    }
    const int32_t state = g_bodyState;
    const SAnimPose pose = SampleI3DAnimPose(g_body3DImg, state, 0, state, 0);
    bool bbox_init = false;
    const int32_t texslots = g_body3DImg->NumTextures() + 1;

    for (int32_t objnum = 0; objnum < g_body3DImg->NumObjects(); ++objnum)
    {
        if (g_body3DImg->IsHidden(objnum, state))
            continue;

        bool obj_kept = false;
        for (int32_t texslot = 0; texslot < texslots; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t> indices;
            if (!ExtractSubMeshTextureSlot(g_body3DImg, objnum, texslot, verts, indices))
                continue;

            TTextureHandle albedo = g_bodyFallbackAlbedo;
            if (texslot > 0)
            {
                S3DTex tex = {};
                g_body3DImg->GetTexture(texslot - 1, &tex);
                if (tex.htexture != kInvalidTexture)
                    albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) return;
            g_bodyMeshes.push_back({h, objnum, texslot});
            obj_kept = true;

            float world[16];
            BuildAnimPoseObjectMatrix(g_body3DImg, pose, state, objnum, world);
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = world[0]*x + world[1]*y + world[2]*z + world[3];
                const float wy = world[4]*x + world[5]*y + world[6]*z + world[7];
                const float wz = world[8]*x + world[9]*y + world[10]*z + world[11];
                if (!bbox_init) {
                    g_bodyBBoxMin[0] = g_bodyBBoxMax[0] = wx;
                    g_bodyBBoxMin[1] = g_bodyBBoxMax[1] = wy;
                    g_bodyBBoxMin[2] = g_bodyBBoxMax[2] = wz;
                    bbox_init = true;
                } else {
                    g_bodyBBoxMin[0] = std::fmin(g_bodyBBoxMin[0], wx);
                    g_bodyBBoxMin[1] = std::fmin(g_bodyBBoxMin[1], wy);
                    g_bodyBBoxMin[2] = std::fmin(g_bodyBBoxMin[2], wz);
                    g_bodyBBoxMax[0] = std::fmax(g_bodyBBoxMax[0], wx);
                    g_bodyBBoxMax[1] = std::fmax(g_bodyBBoxMax[1], wy);
                    g_bodyBBoxMax[2] = std::fmax(g_bodyBBoxMax[2], wz);
                }
            }
        }
        if (!obj_kept)
        {
            // Fallback: whole-mesh extract for objects with no
            // per-texture-slot decomposition (single-texture objects).
            // Same fallback char3d uses (testmodes.cpp:976-1018).
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t> indices;
            if (!ExtractSubMesh(g_body3DImg, objnum, verts, indices))
                continue;
            TTextureHandle albedo = g_bodyFallbackAlbedo;
            if (g_body3DImg->NumTextures() > 0)
            {
                S3DTex tex = {};
                g_body3DImg->GetTexture(0, &tex);
                if (tex.htexture != kInvalidTexture)
                    albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) return;
            g_bodyMeshes.push_back({h, objnum, -1});

            float world[16];
            BuildAnimPoseObjectMatrix(g_body3DImg, pose, state, objnum, world);
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = world[0]*x + world[1]*y + world[2]*z + world[3];
                const float wy = world[4]*x + world[5]*y + world[6]*z + world[7];
                const float wz = world[8]*x + world[9]*y + world[10]*z + world[11];
                if (!bbox_init) {
                    g_bodyBBoxMin[0] = g_bodyBBoxMax[0] = wx;
                    g_bodyBBoxMin[1] = g_bodyBBoxMax[1] = wy;
                    g_bodyBBoxMin[2] = g_bodyBBoxMax[2] = wz;
                    bbox_init = true;
                } else {
                    g_bodyBBoxMin[0] = std::fmin(g_bodyBBoxMin[0], wx);
                    g_bodyBBoxMin[1] = std::fmin(g_bodyBBoxMin[1], wy);
                    g_bodyBBoxMin[2] = std::fmin(g_bodyBBoxMin[2], wz);
                    g_bodyBBoxMax[0] = std::fmax(g_bodyBBoxMax[0], wx);
                    g_bodyBBoxMax[1] = std::fmax(g_bodyBBoxMax[1], wy);
                    g_bodyBBoxMax[2] = std::fmax(g_bodyBBoxMax[2], wz);
                }
            }
        }
    }

    if (g_bodyMeshes.empty() || !bbox_init)
        return;

    // Keep the bbox around for diagnostics. The actual paperdoll scale now
    // comes from the recovered retail root matrix in FUN_00536bc0.
    const float bbox_z = g_bodyBBoxMax[2] - g_bodyBBoxMin[2];
    g_bodyScale = (bbox_z > 1e-3f) ? (float(kBody3DSrcH) * 0.85f / bbox_z) : 1.0f;
    g_bodyMeshesBuilt = true;
    log_info("[ui-equip] body meshes built: %zu submeshes bbox=(%.0f..%.0f, %.0f..%.0f, %.0f..%.0f)",
             g_bodyMeshes.size(),
             g_bodyBBoxMin[0], g_bodyBBoxMax[0],
             g_bodyBBoxMin[1], g_bodyBBoxMax[1],
             g_bodyBBoxMin[2], g_bodyBBoxMax[2]);
}

// Multiply two row-major 4x4 matrices. Local helper (testmodes.cpp has
// the same fn as a file-local; not pulling that in via include).
void MatMul16(const float a[16], const float b[16], float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t i = 0; i < 4; ++i)
                s += a[r * 4 + i] * b[i * 4 + c];
            out[r * 4 + c] = s;
        }
}

void TransposeSourceToRenderer16(const hmm_mat4& src, float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
            out[r * 4 + c] = src.Elements[c][r];
}

void BuildRetailPaperdollRoot(float out[16])
{
    constexpr float kTurn = float(M_PI * 2.0 / 256.0);
    constexpr float kRetailYawRad = float(M_PI);
    constexpr float kRetailTiltXRad = -1.8707963228225708f; // 0xbfef7641

    hmm_mat4 src = {};
    MtxClear(&src);
    hmm_vec3 scale = { 2.0f, 2.0f, 2.0f };
    MtxScale(&src, &scale);
    MtxRotateZ(&src, kRetailYawRad + g_bodyFacingUnits * kTurn);
    MtxRotateY(&src, 0.0f);
    MtxRotateX(&src, kRetailTiltXRad);
    hmm_vec3 pos = { 0.0f, -75.0f, 200.0f };
    MtxTranslate(&src, &pos);

    TransposeSourceToRenderer16(src, out);
}

// Per-frame: render the 3D body via the renderer's tile-pass + lighting-
// pass machinery. Mirrors char3d::RenderCharPreviewMode but with a small
// camera origin so Locke projects into a known sub-rect of lit_target
// (kBody3DSrc{X,Y,W,H}); the equip pane RT then samples that sub-rect
// in Refresh() via Renderer->CompositeLitTargetSubrectToTarget.
//
// The pose is the first frame of the pane's own state (g_bodyState).
void RenderBody3D()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer()) return;
    if (!Body() || g_bodyMeshes.empty() || !g_body3DImg || g_bodyState < 0) return;

    const int32_t state = g_bodyState;

    // Retail FUN_00536bc0 sets the D3D viewport to the full pane rect, uses
    // identity view (FUN_00412c50), then restores the fixed T3DScene
    // projection (FUN_00412b90). In the offscreen sokol path, vp is the
    // full pane sub-rect center in lit_target coordinates.
    const int32_t cam_ox = kBody3DSrcX + kBody3DSrcW / 2;
    const int32_t cam_oy = kBody3DSrcY + kBody3DSrcH / 2;

    // Retail light: AddLight at (-90,70,0), white diffuse, ambient
    // 0xff5f5a55, intensity 0xff, scalar 0x26. The D3D light's ambient
    // color is already the fill term; do not multiply it by 0x26/255.
    // The directional component remains an approximation until the recovered
    // point-light path is forwarded through the paperdoll projection.
    Renderer->SetLight(-0.789352f, 0.613941f, 0.0f,
                       1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    Renderer->SetAmbientColor(95.0f / 255.0f, 90.0f / 255.0f, 85.0f / 255.0f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetTileViewMode(0);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);

    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Renderer->SetMeshProjectionMode(1);
    Renderer->ClearPointLights();
    // Transparent backdrop — we only want Locke's lit pixels in the body
    // sub-rect; the rest of lit_target stays clear so the equip pane
    // chrome reads cleanly over it on swapchain present.
    Renderer->BeginTilePass(0.0f, 0.0f, 0.0f, 0.0f);

    float root[16];
    BuildRetailPaperdollRoot(root);

    const SAnimPose pose = SampleI3DAnimPose(g_body3DImg, state, 0, state, 0);

    // Submit the body meshes — but skip any player sub-objects that are
    // being replaced by equipment (#12). The hidden list is populated by
    // ProcessEquipmentForSkeleton.
    auto isHidden = [](int32_t objnum) {
        for (int32_t h : g_hiddenPlayerObjnums) if (h == objnum) return true;
        return false;
    };

    auto submitWithRoot = [&](MeshHandle mesh, int32_t objnum) {
        SMeshSubmit m = {};
        m.mesh = mesh;
        float local[16];
        BuildAnimPoseObjectMatrix(g_body3DImg, pose, state, objnum, local);
        float world[16];
        MatMul16(root, local, world);
        std::memcpy(m.world, world, sizeof(world));
        m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
        Renderer->SubmitMesh(m);
    };

    for (const auto& sub : g_bodyMeshes)
    {
        if (isHidden(sub.objnum)) continue;
        submitWithRoot(sub.handle, sub.objnum);
    }

    // #12 — Equipment meshes ride the corresponding PLAYER sub-object's
    // bone matrix, NOT the equipment's own pose. Mirrors retail
    // charanimator.cpp:303 (memcpy player_obj.matrix → equip_obj.matrix
    // before RenderObject).
    for (const auto& eq : g_equipMeshes)
        submitWithRoot(eq.handle, eq.playerObjnum);

    Renderer->EndTilePass();
    Renderer->SetMeshProjectionMode(0);
    Renderer->RunLightingPass();
}


// =====================================================================
// HUD drawable — composes the pane RT once per frame and DrawSurface's
// it at the spec §3 screen anchor (452, 0). The screen TR-anchored
// formula (screen_w - 188, 0) is identity-equivalent at Classic 640
// width; at hi-res the right-aligned anchor keeps the pane flush against
// the right screen edge.
// =====================================================================
class TEquipHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_hudVisible) return;
        if (!g_pane) return;
        // spec §3 anchor detection: TR-anchored, offset 188 from right.
        // At Classic 640 this collapses to the literal kPaneX = 452.
        const int32_t dw = Display.Width();
        const int32_t x  = (dw > 0 ? dw : (kPaneX + kPaneW)) - kPaneW;
        Renderer->DrawSurface(g_pane, x, kPaneY);
    }

    void Refresh()
    {
        if (!g_chrome) return;
        EnsurePane();
        if (!g_pane) return;

        // Per-frame mesh build for the paperdoll body. Imagery loads
        // async; retries each frame until the .i3d submeshes are
        // available and registerable.
        TryBuildBodyMeshes();

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear — the chrome itself fills the pane opaque
        // (spec §7: chrome blitted DM_USEDEFAULT, no drop shadow). Any
        // pixels not touched by the chrome would be transparent, but
        // since the chrome is 188×306 (= pane rect), every pixel is
        // covered.
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // -----------------------------------------------------------------
        // Spec §5 step 2 — Chrome blit at pane-local (0,0), opaque.
        // Init :8961 fetches asset `Equip` into mbr_0x64; paint draws it
        // first as the backdrop with all 11 slot wells baked in.
        // -----------------------------------------------------------------
        Renderer->DrawBitmapToTarget(g_chrome, 0, 0, tw, th);

        // -----------------------------------------------------------------
        // Live 3D body (#11). RenderBody3D (called from
        // RenderUIEquipMode BEFORE this pane's Refresh) has just
        // rendered Locke into lit_target at kBody3DSrc{X,Y,W,H}.
        // Sample that sub-rect into the pane RT at the body slot, on
        // top of the chrome — Locke draws over the chrome stone
        // backdrop matching paperdoll-empty-locke_nude.png.
        //
        // The composite is no-op until any_target_ever_written is true
        // (i.e. the tile-pass has run at least once). On the first
        // frame the chrome shows un-overlaid, which is fine —
        // subsequent frames show Locke.
        // -----------------------------------------------------------------
        if (g_bodyMeshesBuilt)
        {
            Renderer->CompositeLitTargetSubrectToTarget(
                /*dst*/ kBodyDstX, kBodyDstY, kBodyDstW, kBodyDstH,
                /*src screen*/ kBody3DSrcX, kBody3DSrcY,
                /*src size*/ kBody3DSrcW, kBody3DSrcH,
                tw, th);
        }

        // -----------------------------------------------------------------
        // Spec §5 step 3 — Per-slot loop, i = 0..10:
        //   - If Player->GetEquip(i) has an item AND its icon has loaded,
        //     blit that equipped icon (DM_TRANSPARENT|DM_BACKGROUND
        //     analog — alpha from the source). This keeps the slot wells
        //     in sync with the same equipment array driving the live
        //     paperdoll mesh replacement.
        //   - Else blit the named placeholder pictogram
        //     (DM_TRANSPARENT — magenta-keyed; spec §7).
        // No drop shadows (spec §7: FUN_00438d80 NOT XREF'd from any
        // of the four leaf methods — UNCONFIRMED-F).
        // -----------------------------------------------------------------
        TPlayer* player = Body();
        const SUIDragState& drag = UIDragState::Get();
        const bool draggingEquip = UIDragState::IsDragging()
                                && drag.source == EDragSource::Equip;
        for (int32_t i = 0; i < NUM_EQ_SLOTS; ++i)
        {
            TInvSlot* slot = g_equipSlots[i];
            if (!slot) continue;

            if (draggingEquip && drag.source_idx == i)
                continue;

            slot->BindItem(player ? player->GetEquip(i) : nullptr, int32_t(TTime::LegacyFrameCount()));
            slot->Draw(tw, th);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        // Spec §3: pane is a fixed 188×306 region (chrome dim == pane).
        // The to-target RT therefore has the same fixed size; no
        // display-width tracking needed (this is a sidebar pane, not a
        // bottom-bar pane that stretches to width).
        if (g_pane) return;
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
    }
};

TEquipHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIEquipMode()
{
    log_info("[ui-equip] === TEquipPane (clean-room from spec) ===");

    // Spec §2: real retail equippane.dat — chrome `Equip` at archive
    // idx 0, plus the 11 named placeholder pictograms.
    g_equipDat = TMulti::LoadMulti((char*)kArchive);
    RegisterUIBitmapAtlasArchive(g_equipDat);
    if (g_equipDat)
    {
        g_chrome = LookupByName(g_equipDat, kChromeName);
        for (int32_t i = 0; i < 11; ++i)
            g_placeholders[i] = LookupByName(g_equipDat, kPlaceholderName[i]);
    }

    log_info("[ui-equip] assets: chrome %s (expect 188x306)",
             g_chrome ? "OK" : "MISS");
    if (g_chrome)
        log_info("[ui-equip] chrome %dx%d", g_chrome->width, g_chrome->height);
    for (int32_t i = 0; i < 11; ++i)
        log_info("[ui-equip] placeholder[%d]=%s : %s%s",
                 i, kPlaceholderName[i],
                 g_placeholders[i] ? "OK" : "MISS",
                 g_placeholders[i]
                   ? "" : " (expect 40x40)");
    BuildEquipSlots();

    // The retail paperdoll matrix supplies the base orientation itself;
    // this env override is only an extra yaw offset for visual sweeps.
    if (const char* override_face = std::getenv("REVENANT_EQUIP_FACING"))
        g_bodyFacingUnits = NormalizeFacingUnits(float(std::atoi(override_face)));
    else
        g_bodyFacingUnits = kDefaultFacingUnits;

    delete g_pane;
    g_pane = nullptr;
    g_hudVisible = true;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIEquipMode()
{
    // Slate backdrop so the right-side equip pane reads in isolation.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();

    RenderUIEquipModeEmbedded();
}

void RenderUIEquipModeEmbedded()
{
    if (!g_hudVisible)
        return;

    // 0. Follow the main player and its equipment.
    BindBody();

    // 1. Run the 3D body render so lit_target holds Locke at the
    //    kBody3DSrc{...} rect. SuppressPresent is scoped to this render so
    //    the fullscreen lit_target composite does NOT paint behind the UI.
    //    Only the equip pane RT samples the relevant sub-rect.
    Renderer->SuppressPresent(true);
    RenderBody3D();
    Renderer->SuppressPresent(false);

    // 2. Compose the equip pane RT: chrome + lit_target sub-rect
    //    (body) + per-slot icons. RenderBody3D must run FIRST so the
    //    Refresh's CompositeLitTargetSubrectToTarget sees a freshly
    //    rendered Locke (any_target_ever_written + valid lit_target
    //    pixels).
    g_hud.Refresh();
    Renderer->SuppressSceneCompositeThisFrame();
}

void SetUIEquipModeVisible(bool visible)
{
    g_hudVisible = visible;
    if (!visible)
        g_bodyDragRotating = false;
}

void CloseUIEquipMode()
{
    Renderer->RemoveHud(&g_hud);

    Renderer->SuppressPresent(false);
    g_hudVisible = true;

    delete g_pane;
    g_pane = nullptr;
    DestroyEquipSlots();

    g_chrome   = nullptr;
    for (int32_t i = 0; i < 11; ++i) g_placeholders[i] = nullptr;
    g_equipDat = nullptr;

    g_bodyPlayer.Clear();
    for (TSafeRef<TObjectInstance>& item : g_bodyEquip)
        item.Clear();
    g_bodyBowMode = false;
    g_bodyState   = -1;
    // Mesh handles registered via RegisterMesh are owned by the renderer;
    // the renderer's asset cache will GC them on shutdown. We just drop
    // our refcount.
    ReleaseBodyMeshRefs();
    g_body3DImg          = nullptr;
    g_bodyFallbackAlbedo = kInvalidTexture;
    g_bodyFacingUnits    = kDefaultFacingUnits;
    g_bodyDragRotating   = false;
    g_bodyDragStartX     = 0;
    g_bodyDragStartFacingUnits = kDefaultFacingUnits;
}

void HandleMouseClickUIEquipMode(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN && IsPaperdollBodyPoint(x, y))
    {
        g_bodyDragRotating = true;
        g_bodyDragStartX = x;
        g_bodyDragStartFacingUnits = g_bodyFacingUnits;
        return;
    }

    if (button == MB_LEFTUP)
        g_bodyDragRotating = false;
}

void HandleMouseMoveUIEquipMode(int32_t button, int32_t x, int32_t y)
{
    (void)y;
    if (!g_bodyDragRotating || !(button & MB_LEFTDOWN))
        return;

    const float delta = float(x - g_bodyDragStartX) * kRotateUnitsPerPixel;
    SetBodyFacingUnits(g_bodyDragStartFacingUnits + delta);
}
