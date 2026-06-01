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
// No hand-rolled chroma-key passes, no procedural silhouette stand-ins.
//
// *************************************************************************

#include "uiequiptest.h"

#include "3dimage.h"
#include "animation.h"
#include "bitmap.h"
#include "character.h"
#include "display.h"
#include "imagery.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "player.h"  // EQ_HEAD / EQ_NECK / ... NUM_EQ_SLOTS
#include "renderer.h"
#include "surface.h"
#include "time.h"
#include "uidragstate.h"   // harness_equip[] for cross-pane drag-drop (#7d / #14)

#include <cstdint>

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
// Character body (paperdoll subject) — Locke, rendered as a baked .i3d
// `invitem` icon. The live 3D animated paperdoll is OUT OF SCOPE — see
// ENGINE/ASSET WALL note in RenderUIEquipMode(); three independent
// blockers (char3d regression, missing SubmitMesh*ToTarget helper,
// no `invanim` body cycle in Locke's .i3d) need to be cleared upstream
// before the body can render live. The instance is spawned so the
// inventory image streamer can hand us the baked head icon.
// =====================================================================
TObjectInstance* g_bodyInst   = nullptr;  // spawned Locke (or first player type)
PTBitmap         g_bodyIcon   = nullptr;  // baked .i3d head icon (invitem)
TAnimation*      g_bodyAnim   = nullptr;  // baked .i3d animated icon (invanim — none for Locke)
int32_t          g_bodyFrames = 0;

// Wall-clock rate for the body's invitem/invanim FALLBACK.
constexpr double kBodyAnimMs = 100.0;

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
// SpawnBodyInstance — spawn Locke (or first available player/character)
// so the paperdoll has a body to draw. Same shape as
// uiplyrstatusbartest::SpawnPortraitInstance (which spawns the same
// Locke instance to source the head icon). The body's imagery loads
// asynchronously after OnScreen; per-frame TryExtractBody pulls the
// bitmap/animation once it lands.
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
    def.rotatez  = 32;  // matches plyrstatusbar facing
    def.group    = 0;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
    {
        log_warn("[ui-equip] NewObject failed for paperdoll body (class=%d type=%d)",
                 objclass, objtype);
        return;
    }
    // Set a default root state — matches plyrstatusbar's portrait spawn.
    // The character's idle pose is the state we want frame-walked for the
    // paperdoll animation.
    if (auto* chr = dynamic_cast<TCharacter*>(inst))
    {
        const char* root = chr->DefaultRootState();
        if (root && *root) chr->SetState((char*)root);
    }
    inst->OnScreen();
    g_bodyInst = inst;
    log_info("[ui-equip] spawned paperdoll body: class=%d type=%d", objclass, objtype);
}

// Per-frame: pull the spawned body's baked icon/animation once its
// imagery has streamed in. Prefers invanim (animated body cycle); falls
// back to invitem (static body bitmap). Retried each frame until it
// succeeds (the imagery body streams in asynchronously after spawn —
// mirrors uiplyrstatusbartest::TryExtractPortrait).
//
// Scans ALL states — the body bitmap/animation might live on a non-zero
// state (idle, walk, attack, ...). The portrait scan in plyrstatusbar
// uses the same defensive sweep.
void TryExtractBody()
{
    if (g_bodyIcon || g_bodyAnim) return;  // already bound
    if (!g_bodyInst) return;
    TObjectImagery* img = g_bodyInst->GetImagery();
    if (!img || img->NumStates() <= 0) return;

    for (int32_t s = 0; s < img->NumStates(); ++s)
    {
        // Prefer invanim (animated body) per user's "Locke ... doing his
        // animation patterns" — that's a per-frame invanim playback.
        if (TAnimation* a = img->GetInvAnimation(s))
        {
            g_bodyAnim   = a;
            g_bodyFrames = a->NumFrames();
            log_info("[ui-equip] paperdoll body: invanim state %d, %d frames",
                     s, g_bodyFrames);
            return;
        }
    }
    // No invanim found anywhere — fall back to static invitem. That's
    // still correct (just non-animated), matching the user's "fall back
    // to GetInvImage if no anim" directive. Log a WARNING so it's clear
    // the body is non-animated AND likely the .i3d only carries a head
    // portrait icon (in which case the body viewport draws it at native
    // size in the upper-torso area, not stretched — see Refresh).
    for (int32_t s = 0; s < img->NumStates(); ++s)
    {
        if (PTBitmap bm = img->GetInvImage(s))
        {
            g_bodyIcon = bm;
            log_warn("[ui-equip] paperdoll body: NO invanim — falling back to "
                     "invitem state %d %dx%d (the character .i3d doesn't carry "
                     "a per-frame body cycle; a real animated paperdoll requires "
                     "an invanim baked into the character's .i3d, which Locke "
                     "doesn't have)",
                     s, bm->width, bm->height);
            return;
        }
    }
    log_warn("[ui-equip] paperdoll body: NEITHER invanim NOR invitem available "
             "on the spawned character — the chrome's painted silhouette will "
             "show through unmodified");
}

// Resolve the current body bitmap (FALLBACK ONLY — used in case the 3D
// mesh render path doesn't engage). invanim → time-stepped frame;
// invitem → flat bitmap; otherwise null (placeholder period before async
// load completes).
PTBitmap CurrentBodyBitmap()
{
    if (g_bodyAnim && g_bodyFrames > 0)
    {
        const double nowMs = TTime::Time() * 1000.0;
        const int32_t f    = (int32_t)(nowMs / kBodyAnimMs) % g_bodyFrames;
        return g_bodyAnim->GetFrame(f);
    }
    return g_bodyIcon;
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
        // Same retry for the paperdoll body (Locke). Probes invanim first
        // (animated body), falls back to invitem (static body).
        TryExtractBody();

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
        // Body / paperdoll: the live 3D render is OUT OF SCOPE (see
        // RenderUIEquipMode for the three blockers). The chrome itself
        // bakes a painted doll silhouette into the central column, so
        // the visual reads as a body even without a live render. No 2D
        // body blit here — stretching a 30-px invitem head to fill a
        // body-shaped viewport would be visually wrong.
        // -----------------------------------------------------------------

        // -----------------------------------------------------------------
        // Spec §5 step 3 — Per-slot loop, i = 0..10.
        //
        // #7d / #14: Cross-pane drag-drop priority.
        // harness_equip[i] is written by UIDragState::CompleteDrag when a
        // drag from any pane (Inventory, BarInv, or another Equip slot) is
        // dropped onto EQ slot i.  Check harness_equip[] FIRST; if it has
        // an item, that takes precedence over the local g_demo[] spawn state
        // (which seeds the initial content at Initialize).  This means any
        // committed Inventory→Equip drop immediately shows in the pane on
        // the next Refresh() without waiting for a re-spawn.
        //
        // Active drag visual (#7b — drag FROM equip source):
        // If a drag is currently in flight from this equip slot (source ==
        // Equip, source_idx == i), skip drawing that slot — it shows empty
        // while the item follows the cursor.
        //
        // Draw order per spec §5:
        //   1. Empty slot → placeholder pictogram (DM_TRANSPARENT)
        //   2. Occupied slot → item icon (harness_equip first, then g_demo)
        // No drop shadows (UNCONFIRMED-F).
        // -----------------------------------------------------------------
        {
            const SUIDragState& drag = UIDragState::Get();
            const bool dragFromEquip = (drag.source == EDragSource::Equip);

            for (int32_t i = 0; i < 11; ++i)
            {
                const SSlotAnchor a = kSlotAnchor[i];

                // #7b: skip this slot if it's the active drag source.
                if (dragFromEquip && drag.source_idx == i) continue;

                // Priority 1: harness_equip[i] (set by CompleteDrag on drop).
                PTBitmap itemIcon = nullptr;
                if (i < kHarnessEquipSlots)
                {
                    const SHarnessSlot& hs = UIDragState::harness_equip[i];
                    if (hs.inst && hs.icon)
                        itemIcon = hs.icon;
                }

                // Priority 2: g_demo[] initial spawn state (seeded at Initialize).
                if (!itemIcon)
                {
                    for (int32_t j = 0; j < kDemoCount; ++j)
                        if (g_demo[j].eqslot == i)
                        {
                            itemIcon = CurrentIconBitmap(g_demo[j]);
                            if (itemIcon) break;
                        }
                }

                // Decide fill: occupied icon or placeholder pictogram.
                PTBitmap fill = itemIcon ? itemIcon : g_placeholders[i];
                if (!fill) continue;

                // Stretch into the 40×40 slot well (tolerates non-square sources).
                Renderer->DrawBitmapSubrectStretchedToTarget(
                    fill,
                    /*dst*/ a.x, a.y, kSlotW, kSlotH,
                    /*src*/ 0, 0, fill->width, fill->height,
                    tw, th);
            }
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

    delete g_pane;
    g_pane = nullptr;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIEquipMode()
{
    // Slate backdrop so the right-side equip pane reads in isolation.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();

    // Live 3D Locke paperdoll is OUT OF SCOPE for this reconstruction.
    // Three blockers, all owned outside this file:
    //   1) `--test=char3d` is regressed (renders blank) — engine fix.
    //   2) No `Renderer->SubmitMesh*ToTarget` helper exists to render a
    //      mesh into a TSurface RT, which is required to compose the
    //      body UNDER the (opaque-center) chrome.
    //   3) Locke's .i3d carries only `invitem` (head, used as inventory
    //      icon), NO `invanim` body cycle — animating the body needs an
    //      asset/authoring task (bake an invanim into the .i3d).
    // HUD compose only — chrome + per-slot icons (static AND animated).
    g_hud.Refresh();
}

void CloseUIEquipMode()
{
    Renderer->RemoveHud(&g_hud);

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
    g_bodyIcon   = nullptr;
    g_bodyAnim   = nullptr;
    g_bodyFrames = 0;
}
