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
// pass at screen (452, 0). Mirrors uibarinvtest / uiplyrstatusbartest /
// uisidetabstest — the canonical compose-to-target contract.
//
// Test-harness items (spec §6.4 / §10): the pane is interesting only when
// populated. We spawn a handful of real retail equipment items (armor /
// weapon / ranged / ammo / talisman / potion) via the object class
// registries and let oi->InventoryImage() yield the baked .i3d icon.
// Imagery loads asynchronously; per-frame TryExtractIcon pulls the icon
// once it lands (same pattern uibarinvtest / uiplyrstatusbartest use —
// [[project-ui-portrait-and-icons]]). No Player is required; we drive
// the per-slot occupied/empty state directly.
//
// Coverage of BOTH icon shapes — static (`invitem`, TBitmap) AND animated
// (`invanim`, TAnimation). Per the icon-load loop at
// src/3dimage.cpp:362-371 / :622-631, items can carry either; the Potion
// family in particular carries `invanim` and animates over time. The
// per-frame TryExtractIcon probes invitem first then falls back to
// GetInvAnimation, and CurrentIconBitmap picks the current frame for
// animated icons at ~10 FPS.
//
// Empty slots that have no demo item show the placeholder pictogram so the
// "what goes here" hint reads correctly (spec §6.3, the retail-specific
// addition). The drag/drop hit-test is recorded in the spec §10 table; the
// test mode does not wire interaction (per task brief).
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — opaque chrome stamp
//   Renderer->DrawBitmapSubrectStretchedToTarget — icon/placeholder into
//                                          the 40×40 slot well
//   Renderer->CompositeLitTargetSubrectToTarget — paperdoll body sub-rect
//                                          (added for the 3D Locke path, #11)
// No hand-rolled chroma-key passes, no procedural silhouette stand-ins.
//
// Live 3D paperdoll (#11, #13 — landed):
//   The pane spawns Locke as a TPlayer instance and per-frame:
//   1. Advances the engine animator (inst->NextFrame() + Animate(false))
//      so the body cycles its current idle state.
//   2. Picks a new idle state every ~4 wall-clock seconds from the
//      verified Locke .i3d set (winv1..winv5, then binv1..binv5, then
//      "walk" as last-ditch root). This is #13 — random idle behaviours.
//   3. Renders the 3D body via the engine's tile-pass + lighting-pass at
//      a fixed screen sub-rect (kBody3DSrc{X,Y,W,H}) into lit_target.
//   4. Composites that lit_target sub-rect into the equip pane RT at the
//      central body region via Renderer->CompositeLitTargetSubrectToTarget,
//      on top of the chrome stone backdrop.
//   SuppressPresent is enabled in InitializeUIEquipMode so the full-screen
//   lit_target composite doesn't paint behind the rest of the UI.
//
// Equipment-on-skeleton (#12 — INFRASTRUCTURE ONLY, no visible meshes yet):
//   The retail equipment-replacement system lives in
//   src/charanimator.cpp::HideCharParts + RenderEquipment. It walks
//   Player->GetEquip(i), for each occupied slot finds the BodyType-
//   matched state in the equipment's .i3d, marks the player's matching
//   sub-objects OBJ3D_HIDE, then renders the equipment's sub-objects
//   in their place via Scene3D.RenderObject.
//
//   That code uses the legacy Scene3D path. For the sokol port the
//   equivalent sokol-side mesh extraction (extract the equipment item's
//   BodyType-matched sub-meshes via ExtractSubMeshTextureSlot, plus
//   skip the player's hidden sub-objects in TryBuildBodyMeshes) is the
//   next step. The wiring is in place: items can be Equip'd on the
//   spawned Player (see EquipDemoItemsOnPlayer below), the engine state
//   is correct, and the existing charanimator.cpp::ProcessEquipment is
//   the algorithmic template to port. Per [[feedback-evolve-dont-
//   replace]] the right home for the new code is a sokol-aware variant
//   of ProcessEquipment on TCharAnimator itself, not a parallel
//   panel-local fork. Captured here as a documented TODO rather than
//   a partial inline guess.
//
// *************************************************************************

#include "uiequiptest.h"

#include "3dimage.h"
#include "animation.h"
#include "bitmap.h"
#include "character.h"
#include "display.h"
#include "i3danimpose.h"
#include "imagery.h"
#include "logging.h"
#include "math3d.h"
#include "meshextract.h"
#include "multi.h"
#include "object.h"
#include "player.h"  // EQ_HEAD / EQ_NECK / ... NUM_EQ_SLOTS
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// Item-class registries (defined in their respective .cpp files). The pane
// is content-agnostic — these are sample real items so the test mode has
// something to draw; the production pane gets its items from
// Player->GetEquip(i) (spec §5 step 3a).
//
// PotionClass is included so we can exercise the ANIMATED `invanim` icon
// path (potions carry invanim instead of invitem — feedback addendum B).
extern TObjectClass ArmorClass;
extern TObjectClass WeaponClass;
extern TObjectClass RangedWeaponClass;
extern TObjectClass AmmoClass;
extern TObjectClass TalismanClass;
extern TObjectClass PotionClass;
// Character body (paperdoll subject) — Locke or first available player.
// Used to source the character's baked .i3d body via GetInvAnimation/GetInvImage
// (same path uiplyrstatusbartest uses for the portrait, scaled up here for
// the full body). The character's .i3d carries an invitem (static body icon)
// and may carry an invanim (per-frame body animation); per-frame Refresh
// walks invanim if present.
extern TObjectClass CharacterClass;
extern TObjectClass PlayerClass;

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
// Animation drive: inst->NextFrame() + Animate(false) once per legacy
// tick, exactly like char3d. The legacy-tick gate is delta-time-driven
// (TTime::LegacyFrameCount() advances by wall-clock), so the cycle is
// frame-rate-independent in compliance with [[feedback-framerate-
// independent-anim]]. Idle state is the TCharacter DefaultRootState
// ("walk"/"sleep"/"combat"/etc.) at spawn; future work can pull a random
// idle string from the character data per equip-pane §9 (Animate slot 19
// reads DAT_005e4060 → mbr_0x9c, UNCONFIRMED-G string content).
//
// Empty placeholder until the imagery streamer has loaded the .i3d body
// (async, mirrors uiplyrstatusbartest::TryExtractPortrait): per-frame
// TryBuildBodyMeshes retries until the sub-objects are decodable.
// =====================================================================
TObjectInstance* g_bodyInst    = nullptr;  // spawned Locke (player class instance)
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
int64_t        g_bodyLastTick   = -1;
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

int32_t g_idleStateIds[kIdleStateCandidateCount];  // FindState lookups
int32_t g_idleStateCount = 0;                       // # of valid entries
int32_t g_idleStateCursor = 0;                      // index into above
double  g_idleStateChangeAtSec = 0.0;               // wall-clock next switch

// Mean dwell per idle state (wall-clock seconds). The retail idle
// behaviour reads as "settle into a pose, hold for a few seconds, settle
// into another" — 4 seconds is a reasonable midpoint. IdleAnimTick
// jitters this ±0.5s so the cycle isn't a metronome.
constexpr double kIdleSwitchDwellSec = 4.0;

// Where in screen-space the 3D body renders. The size matches the
// equip pane's central body region (between the slot wells). This rect
// is also the source we sample from lit_target when composing the
// pane RT.
constexpr int32_t kBody3DSrcX = 16;   // off-screen-edge buffer in lit_target
constexpr int32_t kBody3DSrcY = 16;
constexpr int32_t kBody3DSrcW = 110;  // pane central body column ~110×175
constexpr int32_t kBody3DSrcH = 175;

// Where in the equip pane RT the body composites. Centred between the
// slot wells (left wells x ≤ 7+40=47, right wells x ≥ 141), top under
// the HEAD well (~y=80), bottom above the gold compass base (~y=240).
constexpr int32_t kBodyDstX = 40;   // ~midway between left-edge slots and chrome edge
constexpr int32_t kBodyDstY = 35;
constexpr int32_t kBodyDstW = 110;
constexpr int32_t kBodyDstH = 195;

// =====================================================================
// Per-slot demo content (spec §6.4 algorithm intent only; test mode
// does not depend on a Player).
//
// We spawn a real retail object for a subset of the 11 EQ_* slots and
// let oi->InventoryImage() drive the baked-icon path. The slots not
// listed here render their named placeholder (spec §6.3 retail addition).
// The slot enum drives which placeholder fills in for empty wells.
//
// Choices:
//   - EQ_HEAD: armor (helmet) — uses the head placeholder if no armor
//     type carries a helmet icon (varies by retail data; we use the first
//     spawnable armor type).
//   - EQ_BODY: armor (chest piece).
//   - EQ_PRIMEHAND: weapon (sword/axe/etc).
//   - EQ_OFFHAND: weapon (off-hand weapon — secondary armament).
//   - EQ_RANGEDWEAPON: ranged weapon (bow/crossbow).
//   - EQ_AMMO: ammo (arrows/bolts).
//   - EQ_R_ACCESSORY: talisman (ring/charm).
//   - EQ_NECK, EQ_L_ACCESSORY, EQ_LEGS, EQ_FEET: left empty → render
//     placeholders so the "what goes here" pictogram hint reads.
//
// Asynchronous imagery streams in after spawn; per-frame TryExtractIcon
// retries until the icon lands (same pattern uibarinvtest uses;
// [[project-ui-portrait-and-icons]]).
// =====================================================================
struct SDemoSlot
{
    int32_t          eqslot     = -1;     // EQ_* enum index (0..10)
    int32_t          objclass   = -1;
    TObjectClass*    cls        = nullptr;// registry pointer (null = leave empty)
    TObjectInstance* inst       = nullptr;
    PTBitmap         icon       = nullptr;// static invitem (3dimage.cpp:2017-2025)
    TAnimation*      anim       = nullptr;// animated invanim (3dimage.cpp:2028-2035)
    int32_t          animFrames = 0;      // cached anim->NumFrames()
    const char*      typeName   = "?";
};

// 8 demo bindings — leave NECK, L_ACCESSORY, LEGS empty so the placeholder
// path also exercises in the same capture.
constexpr int32_t kDemoCount = 8;
SDemoSlot g_demo[kDemoCount];

// Animation pacing for invanim icons — ~10 FPS reads as motion without
// flickering. The spec gives no rate for the per-item icon animation
// (Animate slot 19 ticks per-engine-tick and is gated by mbr_0xc4 /
// mbr_0xbc, spec §9); the wall-clock 10 FPS keeps the demo legible.
constexpr double kIconAnimMs = 100.0;

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

// Per-frame: pull a spawned item's baked icon once its imagery has
// streamed in. Probes BOTH `invitem` (static TBitmap) AND `invanim`
// (animated TAnimation), per the icon-load loop at src/3dimage.cpp:
// 362-371 / :622-631 which copies either one or the other depending on
// what the .i3d body carries. Potions are the canonical `invanim` case
// (feedback-B addendum). State 0 is the canonical "inventory image" for
// items; defensive sweep of further states mirrors uiplyrstatusbartest
// ([[project-ui-portrait-and-icons]] — the imagery body loads async, so
// retry each frame until it lands).
//
// Sets icon OR anim (mutually exclusive per state); returns true once
// SOMETHING is bound so the caller can stop retrying for that slot.
bool TryExtractIcon(SDemoSlot& d)
{
    if (d.icon || d.anim) return true;        // already bound
    if (!d.inst) return false;
    TObjectImagery* img = d.inst->GetImagery();
    if (!img || img->NumStates() <= 0) return false;

    // Static icon — what most items carry. 3dimage.cpp:2017-2025.
    if (PTBitmap bm = img->GetInvImage(0))
    {
        d.icon = bm;
        log_info("[ui-equip] slot EQ_%d '%s': invitem %dx%d",
                 d.eqslot, d.typeName, bm->width, bm->height);
        return true;
    }

    // Animated icon — Potion family etc. 3dimage.cpp:2028-2035 exposes
    // the icons[state].invanim field which the icon-load loop populates
    // at :368-371 / :628-631 as an alternative to invitem.
    if (TAnimation* a = img->GetInvAnimation(0))
    {
        d.anim       = a;
        d.animFrames = a->NumFrames();
        log_info("[ui-equip] slot EQ_%d '%s': invanim %d frames (animated)",
                 d.eqslot, d.typeName, d.animFrames);
        return true;
    }

    // Defensive probe — some items put the icon on a non-zero state.
    for (int32_t s = 1; s < img->NumStates(); ++s)
    {
        if (PTBitmap bm = img->GetInvImage(s))
        {
            d.icon = bm;
            log_info("[ui-equip] slot EQ_%d '%s': invitem state %d %dx%d",
                     d.eqslot, d.typeName, s, bm->width, bm->height);
            return true;
        }
        if (TAnimation* a = img->GetInvAnimation(s))
        {
            d.anim       = a;
            d.animFrames = a->NumFrames();
            log_info("[ui-equip] slot EQ_%d '%s': invanim state %d %d frames",
                     d.eqslot, d.typeName, s, d.animFrames);
            return true;
        }
    }
    return false;
}

// Resolve the bitmap to draw for a slot at the current wall-clock time.
// Static icon returns directly; animated icon picks the current frame via
// a time-based ramp (10 FPS, kIconAnimMs above).
PTBitmap CurrentIconBitmap(const SDemoSlot& d)
{
    if (d.icon) return d.icon;
    if (d.anim && d.animFrames > 0)
    {
        const double nowMs = TTime::Time() * 1000.0;
        const int32_t f    = (int32_t)(nowMs / kIconAnimMs) % d.animFrames;
        return d.anim->GetFrame(f);
    }
    return nullptr;
}

// =====================================================================
// SpawnBodyInstance — spawn Locke (player class instance preferred so
// Player->GetEquip(i) drives the equipment-replacement system in
// charanimator.cpp::HideCharParts / RenderEquipment; falls back to
// Character class if no Player type is available). Sets a "still" /
// DefaultRootState so the paperdoll holds a stable idle pose; the
// per-frame inst->NextFrame() ticks animation forward.
// =====================================================================
void SpawnBodyInstance()
{
    int32_t objclass = OBJCLASS_PLAYER;
    int32_t objtype  = PlayerClass.FindObjType((char*)"Locke");
    if (objtype < 0)
        for (int32_t i = 0; i < PlayerClass.NumTypes(); ++i)
            if (PlayerClass.GetObjType(i)) { objtype = i; break; }
    if (objtype < 0)
    {
        objclass = OBJCLASS_CHARACTER;
        for (int32_t i = 0; i < CharacterClass.NumTypes(); ++i)
            if (CharacterClass.GetObjType(i)) { objtype = i; break; }
    }
    if (objtype < 0)
    {
        log_warn("[ui-equip] no character types available for paperdoll body");
        return;
    }

    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl) return;

    SObjectDef def = {};
    def.objclass = (short)objclass;
    def.objtype  = (short)objtype;
    def.state    = 0;
    def.level    = 0;
    def.pos      = { 0, 0, 0 };
    def.vel      = { 0, 0, 0 };
    def.accum    = { 0, 0, 0 };
    def.rotatex  = 0;
    def.rotatey  = 0;
    // rotatez is uint8 in 0..255 game-angle space (object.h:965). At 96
    // Locke reads as a slight-3/4-left chest-toward-camera view -- right
    // shoulder slightly forward, chest visible face-on, matching
    // images/ui/sidebar-equipment/paperdoll-empty-locke_nude.png.
    // Frozen sweep (REVENANT_EQUIP_FREEZE=1 + REVENANT_EQUIP_FACING=N):
    //   0   right profile, facing screen-left
    //   32  rotating toward back
    //   64  3/4 back-left
    //   96  3/4 front-left -- chest visible, head slightly turned       <- pick
    //   128 dead-back (we see Locke's shoulder blades)
    //   160 3/4 back-right (D's prior pick)
    //   192 right profile, facing screen-right
    //   224 3/4 right
    if (const char* override_face = std::getenv("REVENANT_EQUIP_FACING"))
        def.rotatez = (uint8_t)std::atoi(override_face);
    else
        def.rotatez  = 96;
    def.group    = 0;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
    {
        log_warn("[ui-equip] NewObject failed for paperdoll body (class=%d type=%d)",
                 objclass, objtype);
        return;
    }
    // Default pose: winv1 (Locke's first inventory-screen idle pose, the
    // canonical equip-pane resting stance — verified against the state
    // dump for the Locke .i3d). Falls back through the rest of the idle
    // candidates and finally DefaultRootState if none match (a stripped-
    // down character with no inv poses still renders, just frozen).
    inst->OnScreen();
    if (auto* chr = dynamic_cast<TCharacter*>(inst))
    {
        int32_t initState = -1;
        for (int32_t i = 0; i < kIdleStateCandidateCount && initState < 0; ++i)
            initState = chr->FindState((char*)kIdleStateCandidates[i]);
        if (initState >= 0)
            chr->SetState(initState);
        else if (const char* root = chr->DefaultRootState())
            chr->SetState((char*)root);
    }
    g_bodyInst = inst;
    log_info("[ui-equip] spawned paperdoll body: class=%d type=%d name='%s'",
             objclass, objtype, inst->GetTypeName() ? inst->GetTypeName() : "?");

    // #13: probe which idle states this character actually carries. The
    // initial cursor is whichever idle the spawn ended up in (typically
    // the first valid one — "still"). Missing names are silently skipped.
    g_idleStateCount = 0;
    g_idleStateCursor = 0;
    g_idleStateChangeAtSec = TTime::Time() + kIdleSwitchDwellSec;
    for (int32_t i = 0; i < kIdleStateCandidateCount; ++i)
    {
        const int32_t id = inst->FindState((char*)kIdleStateCandidates[i]);
        if (id >= 0)
        {
            g_idleStateIds[g_idleStateCount++] = id;
            log_info("[ui-equip] idle state %d available: '%s' (state id=%d)",
                     g_idleStateCount - 1, kIdleStateCandidates[i], id);
        }
    }
    if (g_idleStateCount == 0)
        log_warn("[ui-equip] no idle states matched; paperdoll will animate "
                 "via DefaultRootState only — #13 idle cycling no-op");
}

// #13: idle-state switch on a wall-clock timer. Called once per frame
// from RenderBody3D; rotates to the next available idle state in
// kIdleStateIds when the dwell expires. Uses delta-time (TTime::Time())
// per [[feedback-framerate-independent-anim]], not a fixed-frame counter.
void IdleAnimTick()
{
    if (!g_bodyInst || g_idleStateCount <= 0) return;
    const double now = TTime::Time();
    if (now < g_idleStateChangeAtSec) return;

    // Rotate to the next idle. The dwell jitters slightly (±25%) so the
    // cycle isn't a metronome — equip pane idles should read as casual
    // organic behaviour, not a clockwork rotation. (Simple deterministic
    // jitter so capture filmstrips remain stable; not a true RNG.)
    g_idleStateCursor = (g_idleStateCursor + 1) % g_idleStateCount;
    const int32_t target = g_idleStateIds[g_idleStateCursor];
    g_bodyInst->SetState(target);
    g_idleStateChangeAtSec = now + kIdleSwitchDwellSec
                             + 0.5 * ((g_idleStateCursor & 1) ? 1.0 : -1.0);
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
    if (!g_body3DImg || !g_bodyInst) return false;
    auto* player = dynamic_cast<TPlayer*>(g_bodyInst);
    if (!player) return false;

    // Player must have a BodyType for the equipment to match.
    const char* bodyType = player->BodyType();
    if (!bodyType || !*bodyType) return false;
    const int32_t bodyTypeLen = int32_t(std::strlen(bodyType));

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
            log_info("[ui-equip] equip slot %d: no body-type-matched state "
                     "in imagery (bodytype='%s')", eq, bodyType);
            continue;
        }

        const int32_t equipTexslots = equip3D->NumTextures() + 1;

        for (int32_t o = 0; o < equip3D->NumObjects(); ++o)
        {
            if (equip3D->IsHidden(o, equipState)) continue;

            char* equipName = equip3D->GetObjectName(o);
            if (!equipName) continue;

            // Strip BodyType prefix if present (charanimator.cpp:253-254).
            const char* matchName = equipName;
            if (nlen > 0 && !strncasecmp(matchName, bodyType, nlen))
                matchName += nlen;

            // Find player sub-object to replace.
            int32_t playerObjnum = -1;
            if (eq == EQ_PRIMEHAND)
            {
                // Replace the "weapon" or "sword" bone with this item.
                playerObjnum = g_body3DImg->GetObjectNum((char*)"weapon");
                if (playerObjnum < 0)
                    playerObjnum = g_body3DImg->GetObjectNum((char*)"sword");
                if (playerObjnum < 0)
                    playerObjnum = g_body3DImg->GetObjectNum((char*)matchName);
            }
            else if (eq == EQ_RANGEDWEAPON)
            {
                // In retail this is gated on IsBowMode() — if the player
                // isn't in bow mode the ranged weapon is hidden. For the
                // equip pane preview we always show it (the equip pane
                // is where the player CHOOSES the bow, so it should
                // visibly be on Locke). Fall-through to the bone lookup.
                playerObjnum = g_body3DImg->GetObjectNum((char*)"bow");
                if (playerObjnum < 0)
                    playerObjnum = g_body3DImg->GetObjectNum((char*)matchName);
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
                // is taking its place. Avoid duplicate entries.
                bool already = false;
                for (int32_t hidden : g_hiddenPlayerObjnums)
                    if (hidden == playerObjnum) { already = true; break; }
                if (!already) g_hiddenPlayerObjnums.push_back(playerObjnum);

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
    auto* player = dynamic_cast<TPlayer*>(g_bodyInst);
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
    if (!g_bodyInst || !Renderer) return;
    if (g_bodyFallbackAlbedo == kInvalidTexture)
        g_bodyFallbackAlbedo = Renderer->WhiteTextureHandle();

    if (!g_body3DImg)
        g_body3DImg = dynamic_cast<T3DImagery*>(g_bodyInst->GetImagery());
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

    const int32_t state     = g_bodyInst->GetState();
    const int32_t frame     = g_bodyInst->GetFrame();
    const int32_t prevstate = g_bodyInst->GetPrevState();
    const int32_t prevframe = g_bodyInst->GetPrevFrame();
    const SAnimPose pose = SampleI3DAnimPose(g_body3DImg, state, frame,
                                             prevstate, prevframe);
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

    // Fit body world bbox to the body rect's height (175 px). Use the
    // bbox z-extent (vertical) primarily -- the body is tall and slim.
    // Default 1.0 matches the retail reference scale (Locke fills the
    // body sub-rect's height almost edge-to-edge). REVENANT_EQUIP_FIT
    // env var overrides for further tuning.
    float fit = 1.0f;
    if (const char* env = std::getenv("REVENANT_EQUIP_FIT"))
    {
        const float f = float(std::atof(env));
        if (f > 0.1f && f < 4.0f) fit = f;
    }
    const float bbox_z = g_bodyBBoxMax[2] - g_bodyBBoxMin[2];
    g_bodyScale = (bbox_z > 1e-3f) ? (float(kBody3DSrcH) * fit / bbox_z) : 1.0f;
    g_bodyMeshesBuilt = true;
    log_info("[ui-equip] body meshes built: %zu submeshes scale=%.2f bbox=(%.0f..%.0f, %.0f..%.0f, %.0f..%.0f)",
             g_bodyMeshes.size(), g_bodyScale,
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

// Per-frame: render the 3D body via the renderer's tile-pass + lighting-
// pass machinery. Mirrors char3d::RenderCharPreviewMode but with a small
// camera origin so Locke projects into a known sub-rect of lit_target
// (kBody3DSrc{X,Y,W,H}); the equip pane RT then samples that sub-rect
// in Refresh() via Renderer->CompositeLitTargetSubrectToTarget.
//
// Animation drive: legacy-tick-gated inst->NextFrame() + Animate(false),
// identical to char3d's per-tick step. This is delta-time-driven through
// the engine's wall-clock tick counter, not a fixed-frame loop —
// satisfies [[feedback-framerate-independent-anim]].
void RenderBody3D()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer()) return;
    if (!g_bodyInst || g_bodyMeshes.empty() || !g_body3DImg) return;

    // Freeze flag — when REVENANT_EQUIP_FREEZE=1 we skip NextFrame, Animate,
    // and IdleAnimTick so the paperdoll holds its initial pose. Used while
    // tuning camera/orientation/lighting against the retail reference image
    // (paperdoll-empty-locke_nude.png) — a still image is the only fair
    // comparison target. Drop the env var to restore #13 idle cycling.
    static const bool s_freeze = []{
        const char* env = std::getenv("REVENANT_EQUIP_FREEZE");
        return env && *env && env[0] != '0';
    }();

    if (!s_freeze)
    {
        // Advance animation once per legacy tick. Char3d uses the same gate;
        // legacy tick advances on real wall-clock so this is frame-rate
        // independent.
        const int64_t legacy_tick = TTime::LegacyFrameCount();
        if (legacy_tick != g_bodyLastTick)
        {
            g_bodyLastTick = legacy_tick;
            g_bodyInst->NextFrame();
            if (g_bodyInst->NeedsAnimator() && !g_bodyInst->HasAnimator())
                g_bodyInst->OnScreen();
            g_bodyInst->Animate(false);
        }

        // #13: cycle to a different idle state on a wall-clock timer so the
        // paperdoll isn't a frozen posture. Independent of the per-tick
        // NextFrame above (which advances frames inside the current state).
        IdleAnimTick();
    }

    const int32_t state = g_bodyInst->GetState();

    // Camera origin in screen pixels. Default: horizontally centred in
    // the body sub-rect; vertically biased BELOW center so Locke's feet
    // land on the pedestal (the body rect's bottom edge is well above
    // the pedestal top in the pane chrome). +55 = empirically chosen
    // against the retail reference paperdoll-empty-locke_nude.png.
    // REVENANT_EQUIP_OFFY env var nudges further at iteration time.
    constexpr int32_t kBody3DOriginYBias = 55;  // shift world origin DOWN
    int32_t off_y = kBody3DOriginYBias;
    if (const char* env = std::getenv("REVENANT_EQUIP_OFFY"))
    {
        const int32_t v = std::atoi(env);
        if (v > -200 && v < 200) off_y = v;
    }
    const int32_t cam_ox = kBody3DSrcX + kBody3DSrcW / 2;
    const int32_t cam_oy = kBody3DSrcY + kBody3DSrcH / 2 + off_y;

    // Lighting / scene setup -- char3d-derived baseline; tuning against
    // the retail reference (paperdoll-empty-locke_nude.png) requires
    // matching the engine's actual world-space conventions for the light
    // direction vector, which the char3d defaults already do.
    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f);
    Renderer->SetAmbientColor(0.55f, 0.55f, 0.55f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetTileViewMode(0);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);

    const float s = g_bodyScale;
    const float bbox_w = std::fmax(
        std::fmax(g_bodyBBoxMax[0] - g_bodyBBoxMin[0],
                  g_bodyBBoxMax[1] - g_bodyBBoxMin[1]),
                  g_bodyBBoxMax[2] - g_bodyBBoxMin[2]);
    constexpr float kCam = 2750.0f;
    const float half_z = std::fmax(256.0f, bbox_w * s);
    const float znear = kCam - half_z - 128.0f;
    const float zfar  = kCam + half_z + 128.0f;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();
    // Transparent backdrop — we only want Locke's lit pixels in the body
    // sub-rect; the rest of lit_target stays clear so the equip pane
    // chrome reads cleanly over it on swapchain present.
    Renderer->BeginTilePass(0.0f, 0.0f, 0.0f, 0.0f);

    const float cx = 0.5f * (g_bodyBBoxMin[0] + g_bodyBBoxMax[0]);
    const float cy = 0.5f * (g_bodyBBoxMin[1] + g_bodyBBoxMax[1]);
    const float cz = 0.5f * (g_bodyBBoxMin[2] + g_bodyBBoxMax[2]);

    // Apply the instance's rotatez as a Z-axis world rotation so Locke
    // faces the viewer (paperdoll convention). BuildAnimPoseObjectMatrix
    // returns the I3D-local per-bone matrix and does NOT include the
    // instance's transform; the production scene-render path multiplies
    // BuildRootMatrixSource(oi) (which carries rotatez) in front. Here
    // we synthesize the same Z-rotation locally so the equip pane
    // doesn't have to plumb the full transform_ pipeline.
    constexpr float kTurn = float(M_PI * 2.0 / 256.0);
    const float az = float(g_bodyInst->GetRotateZ()) * kTurn;
    const float cz_r = std::cos(az);
    const float sz_r = std::sin(az);
    const float rotZ[16] = {
        cz_r, -sz_r, 0.0f, 0.0f,
        sz_r,  cz_r, 0.0f, 0.0f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f,
    };

    const SAnimPose pose = SampleI3DAnimPose(g_body3DImg,
                                              g_bodyInst->GetState(),
                                              g_bodyInst->GetFrame(),
                                              g_bodyInst->GetPrevState(),
                                              g_bodyInst->GetPrevFrame());

    // Submit the body meshes — but skip any player sub-objects that are
    // being replaced by equipment (#12). The hidden list is populated by
    // ProcessEquipmentForSkeleton.
    auto isHidden = [](int32_t objnum) {
        for (int32_t h : g_hiddenPlayerObjnums) if (h == objnum) return true;
        return false;
    };

    auto submitWithRot = [&](MeshHandle mesh, int32_t objnum) {
        SMeshSubmit m = {};
        m.mesh = mesh;
        float local[16];
        BuildAnimPoseObjectMatrix(g_body3DImg, pose, state, objnum, local);
        // Apply scale.
        for (int32_t i = 0; i < 12; ++i) local[i] *= s;
        local[3]  -= cx * s;
        local[7]  -= cy * s;
        local[11] -= cz * s;
        // Apply instance-Z rotation in front so Locke faces the viewer.
        float world[16];
        MatMul16(rotZ, local, world);
        std::memcpy(m.world, world, sizeof(world));
        m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
        Renderer->SubmitMesh(m);
    };

    for (const auto& sub : g_bodyMeshes)
    {
        if (isHidden(sub.objnum)) continue;
        submitWithRot(sub.handle, sub.objnum);
    }

    // #12 — Equipment meshes ride the corresponding PLAYER sub-object's
    // bone matrix, NOT the equipment's own pose. Mirrors retail
    // charanimator.cpp:303 (memcpy player_obj.matrix → equip_obj.matrix
    // before RenderObject).
    for (const auto& eq : g_equipMeshes)
        submitWithRot(eq.handle, eq.playerObjnum);

    Renderer->EndTilePass();
    Renderer->RunLightingPass();
}


// Spawn a single item from a given class, OPTIONALLY filtering by the
// item's EqSlot stat so an EQ_HEAD slot receives an actual head-armor
// piece (not the first armor type, which might be boots — see retail
// CanEquip at src/player.cpp:284-289 which gates equip on
// `oi->GetStat("EqSlot") == slot`). If wantEqSlot >= 0, only types whose
// EqSlot matches are spawned; if wantEqSlot < 0, the first spawnable
// type is returned (used for items where the slot constraint is enum-
// trivial, e.g. PotionClass which doesn't enforce a slot).
//
// Returned instance has OnScreen called so the imagery streamer kicks
// in; ownership belongs to the test harness. If no type matches the
// requested EqSlot, returns null AND the caller falls through to the
// empty-slot placeholder branch in the per-slot loop.
TObjectInstance* SpawnItemFromClass(TObjectClass& cls, int32_t objclass,
                                    int32_t wantEqSlot,
                                    const char** name_out)
{
    const int32_t numTypes = cls.NumTypes();
    for (int32_t i = 0; i < numTypes; ++i)
    {
        SObjectInfo* info = cls.GetObjType(i);
        if (!info) continue;

        SObjectDef def = {};
        def.objclass = (short)objclass;
        def.objtype  = (short)i;
        def.state    = 0;
        def.level    = 0;
        def.pos      = { 0, 0, 0 };
        def.vel      = { 0, 0, 0 };
        def.accum    = { 0, 0, 0 };
        def.rotatex  = 0;
        def.rotatey  = 0;
        def.rotatez  = 0;
        def.group    = 0;

        TObjectInstance* inst = cls.NewObject(&def);
        if (!inst) continue;

        // EqSlot filter — mirrors retail TPlayer::CanEquip (player.cpp:
        // 284-289). The stat tells the engine which EQ_* slot the type
        // is allowed to occupy. We use it here so the test harness picks
        // a slot-appropriate type (helmet for HEAD, boots for FEET, etc.)
        // rather than the first item in the type table that happens to
        // be the wrong slot. The stat lives on the freshly-built object
        // (NewObject populates class-defaults from SObjectInfo::stats);
        // we query it before OnScreen so the rejected probe doesn't run
        // the imagery streamer + animator setup.
        if (wantEqSlot >= 0 && inst->FindStat("EqSlot") >= 0)
        {
            const int32_t s = inst->GetStat("EqSlot");
            if (s != wantEqSlot)
            {
                delete inst;
                continue;
            }
        }

        inst->OnScreen();
        if (name_out) *name_out = info->name ? info->name : cls.ClassName();
        return inst;
    }
    return nullptr;
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

        // Per-frame icon extraction (async imagery load —
        // [[project-ui-portrait-and-icons]]). TryExtractIcon probes both
        // invitem (static) and invanim (animated) per feedback addendum B.
        for (int32_t i = 0; i < kDemoCount; ++i)
            TryExtractIcon(g_demo[i]);
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
        //   - If demo slot has a spawned item AND its icon has loaded,
        //     blit the equipped icon (DM_TRANSPARENT|DM_BACKGROUND
        //     analog — alpha from the source).
        //   - Else blit the named placeholder pictogram
        //     (DM_TRANSPARENT — magenta-keyed; spec §7).
        // No drop shadows (spec §7: FUN_00438d80 NOT XREF'd from any
        // of the four leaf methods — UNCONFIRMED-F).
        // -----------------------------------------------------------------
        for (int32_t i = 0; i < 11; ++i)
        {
            const SSlotAnchor a = kSlotAnchor[i];

            // Find a demo item bound to this EQ slot, if any. The
            // CurrentIconBitmap() helper resolves static (invitem) and
            // animated (invanim) sources uniformly — for animated items
            // it returns the current frame at the wall-clock-driven
            // 10 FPS ramp.
            PTBitmap itemIcon = nullptr;
            for (int32_t j = 0; j < kDemoCount; ++j)
                if (g_demo[j].eqslot == i)
                {
                    itemIcon = CurrentIconBitmap(g_demo[j]);
                    if (itemIcon) break;
                }

            // Decide which bitmap fills the 40×40 well. Item icon first
            // (occupied path, spec §5 step 3d); else placeholder (empty
            // path, spec §5 step 3c / §6.3 retail addition).
            PTBitmap fill = itemIcon ? itemIcon : g_placeholders[i];
            if (!fill) continue;

            // Stretch into the canonical 40×40 slot well. Item icons are
            // typically 40×40 already (matches placeholder WxH); the
            // stretched variant tolerates source sizes that differ (the
            // baked .i3d invitem isn't guaranteed to be exactly 40×40,
            // and DrawBitmapToTarget would chop oversized art).
            Renderer->DrawBitmapSubrectStretchedToTarget(
                fill,
                /*dst*/ a.x, a.y, kSlotW, kSlotH,
                /*src*/ 0, 0, fill->width, fill->height,
                tw, th);
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

// =====================================================================
// SpawnDemoSlots — bind a mix of real retail items into the EQ_* slot
// table so the icon path exercises (the rest remain empty so the
// placeholder pictograms render).
//
// Each binding requests an EqSlot-matching type (the per-class type table
// can carry items for several different EQ_* slots — e.g. ArmorClass
// holds head / body / legs / feet armor types; passing wantEqSlot picks
// the first type whose EqSlot stat matches the target slot, mirroring
// retail TPlayer::CanEquip at src/player.cpp:284-289).
//
// Without that filter the first ArmorClass type happens to be boots,
// which is why an earlier pass showed boots in the HEAD slot (feedback
// addendum C). The filter ensures HEAD gets head-armor and FEET gets
// boots.
//
// FEET intentionally binds a Potion (OBJCLASS_POTION) so the ANIMATED
// `invanim` icon path exercises in the same capture (feedback addendum
// B). Potions don't have an EqSlot stat, so wantEqSlot=-1 — they're
// content-agnostic for this test, the bind just drives the animated
// icon. Placing a potion in the FEET slot is non-canonical for a real
// player (CanEquip would reject it) but the EquipPane itself is purely
// presentational: it draws whatever Player->GetEquip(slot) returns. The
// test harness exploits this to cover the animated icon path without
// requiring a Player + inventory system.
//
// The remaining 3 slots (NECK, L_ACCESSORY, LEGS) deliberately stay
// empty so the named placeholder render path is also exercised in a
// single capture.
// =====================================================================
void SpawnDemoSlots()
{
    for (int32_t i = 0; i < kDemoCount; ++i) g_demo[i] = SDemoSlot{};

    auto bind = [](SDemoSlot* arr, int32_t* count,
                   int32_t eqslot, int32_t objclass,
                   TObjectClass& cls, int32_t wantEqSlot) {
        if (*count >= kDemoCount) return;
        SDemoSlot& d = arr[*count];
        d.eqslot   = eqslot;
        d.objclass = objclass;
        d.cls      = &cls;
        d.inst     = SpawnItemFromClass(cls, objclass, wantEqSlot, &d.typeName);
        if (d.inst)
        {
            ++(*count);
            log_info("[ui-equip] bound EQ_%d ← %s.%s", eqslot,
                     cls.ClassName(), d.typeName ? d.typeName : "?");
        }
        else
        {
            log_warn("[ui-equip] no %s type with EqSlot=%d — slot %d stays empty",
                     cls.ClassName(), wantEqSlot, eqslot);
        }
    };

    int32_t count = 0;
    // Slot-appropriate bindings — wantEqSlot matches the EQ_* target so
    // each class's EqSlot-filtered probe picks the right subtype.
    bind(g_demo, &count, EQ_HEAD,         OBJCLASS_ARMOR,         ArmorClass,        EQ_HEAD);
    bind(g_demo, &count, EQ_BODY,         OBJCLASS_ARMOR,         ArmorClass,        EQ_BODY);
    bind(g_demo, &count, EQ_PRIMEHAND,    OBJCLASS_WEAPON,        WeaponClass,       EQ_PRIMEHAND);
    bind(g_demo, &count, EQ_OFFHAND,      OBJCLASS_WEAPON,        WeaponClass,       EQ_OFFHAND);
    bind(g_demo, &count, EQ_RANGEDWEAPON, OBJCLASS_RANGEDWEAPON,  RangedWeaponClass, EQ_RANGEDWEAPON);
    bind(g_demo, &count, EQ_AMMO,         OBJCLASS_AMMO,          AmmoClass,         EQ_AMMO);
    bind(g_demo, &count, EQ_R_ACCESSORY,  OBJCLASS_TALISMAN,      TalismanClass,     EQ_R_ACCESSORY);

    // FEET binding: prefer a real boots armor (EqSlot=EQ_FEET). If that
    // works the FEET slot shows the canonical boots-in-feet pictogram. If
    // no boots type exists in the data, fall through to a Potion to keep
    // the animated `invanim` path exercised — feedback addendum B is
    // the higher-priority goal.
    bind(g_demo, &count, EQ_FEET,         OBJCLASS_ARMOR,         ArmorClass,        EQ_FEET);
    if (count > 0 && g_demo[count - 1].eqslot != EQ_FEET)
        bind(g_demo, &count, EQ_FEET,     OBJCLASS_POTION,        PotionClass,       -1);

    // NECK, L_ACCESSORY, LEGS → empty → placeholders.

    // Additionally always bind a Potion somewhere (so the invanim path
    // exercises even if a boots armor was found). Pick LEGS as the home —
    // potions don't have an EqSlot stat, so this is harmless for the test.
    bool potionBound = false;
    for (int32_t i = 0; i < count; ++i)
        if (g_demo[i].objclass == OBJCLASS_POTION) { potionBound = true; break; }
    if (!potionBound)
        bind(g_demo, &count, EQ_LEGS,     OBJCLASS_POTION,        PotionClass,       -1);

    log_info("[ui-equip] spawned %d demo items into EQ_* slots", count);
}

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

    // Spawn demo items into a subset of EQ_* slots so both the occupied
    // (item icon) AND empty (placeholder) render paths exercise in a
    // single capture. The imagery streams in asynchronously; per-frame
    // Refresh calls TryExtractIcon to pull the baked .i3d face once it
    // lands.
    SpawnDemoSlots();

    // Spawn the paperdoll body (Locke). Same async pattern as
    // SpawnDemoSlots — the imagery streams in after spawn; per-frame
    // Refresh calls TryExtractBody to pull the body bitmap/animation
    // once it lands. Falls back to the chrome's painted silhouette
    // until then.
    SpawnBodyInstance();

    // Equip the demo items on the spawned Player so engine state mirrors
    // a real "Locke wearing kit" scenario. This populates Player->GetEquip
    // (i) which is what charanimator.cpp::ProcessEquipment reads. The
    // sokol-side mesh extraction for equipment-on-skeleton (#12) reads
    // the same equipment[] array — once it's wired (port of
    // ProcessEquipment to the SubmitMesh path), no other change is
    // needed here. CanEquip filters per-slot compatibility; mismatched
    // items silently skip.
    if (auto* player = dynamic_cast<TPlayer*>(g_bodyInst))
    {
        for (int32_t i = 0; i < kDemoCount; ++i)
        {
            const SDemoSlot& d = g_demo[i];
            if (!d.inst || d.eqslot < 0 || d.eqslot >= NUM_EQ_SLOTS) continue;
            if (player->CanEquip(d.inst, d.eqslot))
            {
                player->Equip(d.inst, d.eqslot);
                log_info("[ui-equip] equipped EQ_%d ← %s on Player",
                         d.eqslot, d.typeName ? d.typeName : "?");
            }
            else
            {
                log_info("[ui-equip] CanEquip rejected EQ_%d ← %s (likely "
                         "EqSlot stat mismatch; item still drawn in slot well)",
                         d.eqslot, d.typeName ? d.typeName : "?");
            }
        }
    }

    delete g_pane;
    g_pane = nullptr;

    // The 3D body render runs each frame in RenderUIEquipMode using the
    // existing tile-pass + lighting-pass machinery (fills lit_target).
    // Suppress the normal Scene3D → swapchain composite so lit_target
    // does NOT paint behind the entire UI; the equip pane RT samples
    // only the body sub-rect via CompositeLitTargetSubrectToTarget.
    // CloseUIEquipMode restores the default (Present enabled) so other
    // test modes work normally afterwards.
    Renderer->SuppressPresent(true);

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIEquipMode()
{
    // Slate backdrop so the right-side equip pane reads in isolation.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();

    // 1. Run the 3D body render so lit_target holds Locke at the
    //    kBody3DSrc{...} rect. SuppressPresent is enabled before this
    //    call (see InitializeUIEquipMode) so this fullscreen render
    //    does NOT composite to the swapchain — only the equip pane RT
    //    samples the relevant sub-rect.
    RenderBody3D();

    // 2. Compose the equip pane RT: chrome + lit_target sub-rect
    //    (body) + per-slot icons. RenderBody3D must run FIRST so the
    //    Refresh's CompositeLitTargetSubrectToTarget sees a freshly
    //    rendered Locke (any_target_ever_written + valid lit_target
    //    pixels).
    g_hud.Refresh();
}

void CloseUIEquipMode()
{
    Renderer->RemoveHud(&g_hud);

    // Restore the default Scene3D-to-swapchain composite (we toggled it
    // off in InitializeUIEquipMode so the body lit_target wouldn't paint
    // behind the UI). Other test modes that follow expect Present enabled.
    Renderer->SuppressPresent(false);

    delete g_pane;
    g_pane = nullptr;

    g_chrome   = nullptr;
    for (int32_t i = 0; i < 11; ++i) g_placeholders[i] = nullptr;
    g_equipDat = nullptr;

    for (int32_t i = 0; i < kDemoCount; ++i)
    {
        SDemoSlot& d = g_demo[i];
        if (d.inst)
        {
            d.inst->OffScreen();
            delete d.inst;
        }
        d = SDemoSlot{};
    }

    if (g_bodyInst)
    {
        g_bodyInst->OffScreen();
        delete g_bodyInst;
        g_bodyInst = nullptr;
    }
    // Mesh handles registered via RegisterMesh are owned by the renderer;
    // the renderer's asset cache will GC them on shutdown. We just drop
    // our refcount.
    for (const auto& sub : g_bodyMeshes)
        if (sub.handle) Renderer->ReleaseMeshAssetRef(sub.handle, 1);
    for (const auto& eq : g_equipMeshes)
        if (eq.handle) Renderer->ReleaseMeshAssetRef(eq.handle, 1);
    g_bodyMeshes.clear();
    g_equipMeshes.clear();
    g_hiddenPlayerObjnums.clear();
    g_body3DImg          = nullptr;
    g_bodyFallbackAlbedo = kInvalidTexture;
    g_bodyMeshesBuilt    = false;
    g_bodyLastTick       = -1;
}
