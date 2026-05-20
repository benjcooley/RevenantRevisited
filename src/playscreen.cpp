// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                  playscreen.cpp - main play-mode screen               *
// *************************************************************************
//
// Rewrite of TPlayScreen on top of the new render path. World rendering
// flows through the owned TMapRenderer (same instance type proven in
// --test=sector); panes composite 2D on top.
//
// Behaviors that lived in the retail TPlayScreen and were dropped because
// they don't fit the new architecture: BITMAP.100 backdrop, DrawOverhangs,
// the postanim/posttext arrays, and the Pulse/Animate -> Display.Put/ZPut/
// PutDim/WriteText draw chain. The legacy code is preserved at
// attic/src/playscreen.cpp for reference.
//
// What survived (often as stubs or shims):
//  - Game-time tracking (gameframes, gametime, time-of-day curve).
//  - Mode flags (fullscreen / demo / control).
//  - Save-load entry points (deferred to next Update so we don't tear
//    down state mid-frame).
//  - GAMECOMMAND dispatch + UpdateMove plumbing for player input.
//  - Pane plumbing (AddPane / SetExclusivePane / etc. live on TScreen).
//
// *************************************************************************

#include "playscreen.h"

#include <cstring>

#include "3dimage.h"
#include "area.h"
#include "cursor.h"
#include "display.h"
#include "multi.h"
#include "editor.h"
#include "editorstub.h"
#include "imagery.h"
#include "imgui.h"
#include "logging.h"
#include "ctrlmap.h"
#include "gamemap.h"
#include "mapmanager.h"
#include "mappane.h"
#include "maprenderer.h"
#include "player.h"
#include "revisited_settings.h"
#include "runtimemode.h"
#include "savegame.h"
#include "sector.h"
#include "time.h"

#include <vector>

// Cursor bitmaps loaded from gamedata at boot. Other modules
// (automap, editor console, mappane) reference these by symbol; keep
// them as globals so the existing call sites compile unchanged.
PTBitmap PointerCursor = nullptr;
PTBitmap HandCursor    = nullptr;

// Default game controls. Mirrors the retail table from
// attic/src/playscreen.cpp. Mode flags differ per binding: directional
// keys are ALLMODES (work in walk / combat / bow / sneak), combat
// strikes only fire in CTRL_COMBATMODE, etc. Mode bits live in the
// ControlMap and gate which command a key generates -- the retail
// values matter once the mode-switching is wired through ControlMap.
static constexpr uint32_t CTRL_NORMALMODE    = 1;
static constexpr uint32_t CTRL_COMBATMODE    = 2;
static constexpr uint32_t CTRL_BOWMODE       = 4;
static constexpr uint32_t CTRL_SNEAKMODE     = 8;
static constexpr uint32_t CTRL_INVENTORYMODE = 16;

static SControlEntry g_defaultGameControls[] =
{
    // -- spell invocation slots --
    {"Invoke 1", "Invoke1", CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{VK_F1}}, GAMECMD_INVOKE1, 0, 0, false},
    {"Invoke 2", "Invoke2", CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{VK_F2}}, GAMECMD_INVOKE2, 0, 0, false},
    {"Invoke 3", "Invoke3", CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{VK_F3}}, GAMECMD_INVOKE3, 0, 0, false},
    {"Invoke 4", "Invoke4", CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{VK_F4}}, GAMECMD_INVOKE4, 0, 0, false},

    // -- mode switches --
    {"Combat Mode", "CombatMode", CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{VK_RETURN}}, GAMECMD_COMBAT,     0, 0, false},
    {"Bow Mode",    "BowMode",    CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{'M'}},       GAMECMD_BOW,        0, 0, false},
    {"Full Screen", "FullScreen", ALLMODES,                                          {{VK_SPACE}},  GAMECMD_FULLSCREEN, 0, 0, false},

    // -- movement modifiers (state flags, repeat-down/up) --
    {"Sneak", "Sneak", CTRL_NORMALMODE | CTRL_SNEAKMODE, {{'S'}}, GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_SNEAK, false},
    {"Run",   "Run",   CTRL_NORMALMODE | CTRL_COMBATMODE | CTRL_BOWMODE, {{'R'}}, GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_RUN, false},

    // -- world interaction --
    {"Use",  "Use",  CTRL_NORMALMODE, {{'U'}, {'T'}}, GAMECMD_USE,  0, 0, false},
    {"Get",  "Get",  CTRL_NORMALMODE, {{'G'}},         GAMECMD_GET,  0, 0, false},
    {"Jump", "Jump", CTRL_NORMALMODE, {{'J'}},         GAMECMD_JUMP, 0, 0, false},

    // -- walk-mode action slots (unmapped by default; scripted hooks bind these) --
    {"Walk Action 1", "WalkAction1", CTRL_NORMALMODE, {{0}}, GAMECMD_WALKACTION1, 0, 0, false},
    {"Walk Action 2", "WalkAction2", CTRL_NORMALMODE, {{0}}, GAMECMD_WALKACTION2, 0, 0, false},
    {"Walk Action 3", "WalkAction3", CTRL_NORMALMODE, {{0}}, GAMECMD_WALKACTION3, 0, 0, false},

    // -- inventory --
    {"Inventory",       "Inventory", CTRL_NORMALMODE,    {{'I'}},       GAMECMD_INVENTORY, 0, 0, false},
    {"Inventory Use",   "InvUse",    CTRL_INVENTORYMODE, {{'U'}},       GAMECMD_INVUSE,    0, 0, false},
    {"Inventory Move",  "InvMove",   CTRL_INVENTORYMODE, {{'M'}},       GAMECMD_INVMOVE,   0, 0, false},
    {"Inventory Drop",  "InvDrop",   CTRL_INVENTORYMODE, {{'D'}},       GAMECMD_INVDROP,   0, 0, false},
    {"Inventory Exit",  "InvExit",   CTRL_INVENTORYMODE, {{VK_ESCAPE}}, GAMECMD_INVEXIT,   0, 0, false},

    // -- combat combos (Ctrl + key, Shift + key) --
    {"Combat Combo 1",  "CombatCombo1",  CTRL_COMBATMODE, {{VK_CONTROL, 'A'}}, GAMECMD_COMBO1,  0, 0, false},
    {"Combat Combo 2",  "CombatCombo2",  CTRL_COMBATMODE, {{VK_CONTROL, 'S'}}, GAMECMD_COMBO2,  0, 0, false},
    {"Combat Combo 3",  "CombatCombo3",  CTRL_COMBATMODE, {{VK_CONTROL, 'D'}}, GAMECMD_COMBO3,  0, 0, false},
    {"Combat Combo 4",  "CombatCombo4",  CTRL_COMBATMODE, {{VK_CONTROL, 'F'}}, GAMECMD_COMBO4,  0, 0, false},
    {"Combat Combo 5",  "CombatCombo5",  CTRL_COMBATMODE, {{VK_CONTROL, 'G'}}, GAMECMD_COMBO5,  0, 0, false},
    {"Combat Combo 6",  "CombatCombo6",  CTRL_COMBATMODE, {{VK_CONTROL, 'H'}}, GAMECMD_COMBO6,  0, 0, false},
    {"Combat Combo 7",  "CombatCombo7",  CTRL_COMBATMODE, {{VK_SHIFT,   'A'}}, GAMECMD_COMBO7,  0, 0, false},
    {"Combat Combo 8",  "CombatCombo8",  CTRL_COMBATMODE, {{VK_SHIFT,   'S'}}, GAMECMD_COMBO8,  0, 0, false},
    {"Combat Combo 9",  "CombatCombo9",  CTRL_COMBATMODE, {{VK_SHIFT,   'D'}}, GAMECMD_COMBO9,  0, 0, false},
    {"Combat Combo 10", "CombatCombo10", CTRL_COMBATMODE, {{VK_SHIFT,   'F'}}, GAMECMD_COMBO10, 0, 0, false},
    {"Combat Combo 11", "CombatCombo11", CTRL_COMBATMODE, {{VK_SHIFT,   'G'}}, GAMECMD_COMBO11, 0, 0, false},
    {"Combat Combo 12", "CombatCombo12", CTRL_COMBATMODE, {{VK_SHIFT,   'H'}}, GAMECMD_COMBO12, 0, 0, false},

    // -- combat strikes / blocks / dodge / leap --
    {"Combat Block",  "CombatBlock",  CTRL_COMBATMODE, {{'Q'}}, GAMECMD_BLOCKDOWN, GAMECMD_BLOCKUP, CMDFLAG_BLOCK, false},
    {"Combat Dodge",  "CombatDodge",  CTRL_COMBATMODE, {{'W'}}, GAMECMD_DODGE,     0,                0,            false},
    {"Combat Leap",   "CombatLeap",   CTRL_COMBATMODE, {{'F'}}, GAMECMD_LEAPDOWN,  GAMECMD_LEAPUP,   CMDFLAG_LEAP,  false},
    {"Combat Swing",  "CombatSwing",  CTRL_COMBATMODE, {{'A'}}, GAMECMD_SWING,     0,                0,            false},
    {"Combat Thrust", "CombatThrust", CTRL_COMBATMODE, {{'S'}}, GAMECMD_THRUST,    0,                0,            false},
    {"Combat Chop",   "CombatChop",   CTRL_COMBATMODE, {{'D'}}, GAMECMD_CHOP,      0,                0,            false},

    // -- combat action slots (scripted) --
    {"Combat Action 1", "CombatAction1", CTRL_COMBATMODE, {{0}}, GAMECMD_COMBATACTION1, 0, 0, false},
    {"Combat Action 2", "CombatAction2", CTRL_COMBATMODE, {{0}}, GAMECMD_COMBATACTION2, 0, 0, false},
    {"Combat Action 3", "CombatAction3", CTRL_COMBATMODE, {{0}}, GAMECMD_COMBATACTION3, 0, 0, false},

    // -- bow mode --
    {"Bow Shoot",    "BowShoot",    CTRL_BOWMODE, {{'Z'}}, GAMECMD_BOWAIM,    GAMECMD_BOWSHOOT, 0, false},
    {"Bow Action 1", "BowAction1",  CTRL_BOWMODE, {{0}},   GAMECMD_BOWACTION1, 0, 0, false},
    {"Bow Action 2", "BowAction2",  CTRL_BOWMODE, {{0}},   GAMECMD_BOWACTION2, 0, 0, false},
    {"Bow Action 3", "BowAction3",  CTRL_BOWMODE, {{0}},   GAMECMD_BOWACTION3, 0, 0, false},

    // -- sneak attacks / actions (scripted) --
    {"Sneak Attack 1", "SneakAttack1", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKATTACK1, 0, 0, false},
    {"Sneak Attack 2", "SneakAttack2", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKATTACK2, 0, 0, false},
    {"Sneak Attack 3", "SneakAttack3", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKATTACK3, 0, 0, false},
    {"Sneak Action 1", "SneakAction1", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKACTION1, 0, 0, false},
    {"Sneak Action 2", "SneakAction2", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKACTION2, 0, 0, false},
    {"Sneak Action 3", "SneakAction3", CTRL_SNEAKMODE, {{0}}, GAMECMD_SNEAKACTION3, 0, 0, false},

    // -- 8-way movement (cardinal + diagonals) --
    {"Left",       "Left",      ALLMODES, {{VK_LEFT},  {VK_JOYLEFT}},      GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_LEFT,      false},
    {"Right",      "Right",     ALLMODES, {{VK_RIGHT}, {VK_JOYRIGHT}},     GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_RIGHT,     false},
    {"Up",         "Up",        ALLMODES, {{VK_UP},    {VK_JOYUP}},        GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UP,        false},
    {"Down",       "Down",      ALLMODES, {{VK_DOWN},  {VK_JOYDOWN}},      GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWN,      false},
    {"Up Left",    "UpLeft",    ALLMODES, {{VK_HOME},  {VK_JOYUPLEFT}},    GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UPLEFT,    false},
    {"Up Right",   "UpRight",   ALLMODES, {{VK_PRIOR}, {VK_JOYUPRIGHT}},   GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UPRIGHT,   false},
    {"Down Left",  "DownLeft",  ALLMODES, {{VK_END},   {VK_JOYDOWNLEFT}},  GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWNLEFT,  false},
    {"Down Right", "DownRight", ALLMODES, {{VK_NEXT},  {VK_JOYDOWNRIGHT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWNRIGHT, false},
};

// Game runs at this many internal ticks per real-time second. Used by
// time-of-day + the frame-to-minutes helpers.
static constexpr int32_t kGameFrameRate = 30;

// One in-game day = 24 game-minutes of game time. The legacy clamp says
// "morning starts at 6", "noon at 12", etc.; reuse the same buckets so
// scripted lighting checks keep working.
static int32_t TimeOfDayMinutes(int32_t gametime_centi_secs)
{
    // gametime is in 1/100s of a second; convert to in-game minutes.
    // Retail used a 60x scale (1 real second = 60 in-game seconds = 1
    // in-game minute). Match that so saved games stay aligned.
    const int32_t in_game_seconds = gametime_centi_secs / 100;
    return in_game_seconds / 60;
}

int32_t ConvertFramesToMinutes(int32_t frames)
{
    if (frames <= 0) return 0;
    return frames / kGameFrameRate / 60;
}

int32_t ConvertMinutesToFrames(int32_t minutes)
{
    if (minutes <= 0) return 0;
    return minutes * 60 * kGameFrameRate;
}

// *************************************************************************
// * Construction / lifetime                                               *
// *************************************************************************

TPlayScreen::TPlayScreen() = default;
TPlayScreen::~TPlayScreen() = default;

bool TPlayScreen::Initialize()
{
    log_info("[playscreen] initialize start");
    // TScreen's base Initialize() returns false (it's a "must override"
    // hook); skip it and do our own setup.

    // Load the play-screen multi-resource (cursors, gameplay bitmaps,
    // fonts referenced by HUD widgets). Retail did this at the top of
    // TPlayScreen::Initialize (legacy/playscreen.cpp:179). Anything
    // touching GameData->Bitmap("cursor") / Font(...) / Animation(...)
    // before this load would see a null pointer.
    if (!GameData)
    {
        GameData = TMulti::LoadMulti((char*)"playscrn.dat");
        if (!GameData)
            log_warn("[playscreen] failed to load playscrn.dat -- GameData stays null");
        else
            log_info("[playscreen] playscrn.dat loaded");
    }

    log_info("[playscreen] booting map renderer");
    // Spin up the map renderer. InitializeFromStartupArgs reads --level
    // and --sector to pick the starting world; with neither set, we fall
    // back to its defaults (Misthaven-area). The post-load hook injects
    // Locke into the anchor sector before the renderer scans for
    // drawables, so the player ends up in the initial draw list.
    mapRenderer = std::make_unique<TMapRenderer>();
    if (!mapRenderer->InitializeFromStartupArgs(
            [this](int32_t lvl, int32_t sx, int32_t sy) { SpawnDefaultPlayer(lvl, sx, sy); }))
    {
        log_warn("[playscreen] map renderer failed to initialize from startup args");
        // Keep going -- we still want to land on PlayScreen with empty
        // world rather than abort the whole boot.
    }
    // Push global Revisited point-light multipliers now that MapRenderer
    // is live. Per-area POINTLIGHTINT / POINTLIGHTRANGE will compose on
    // top of these each time TArea::Enter fires.
    ApplyRevisitedSettingsToMapRenderer(mapRenderer.get());
    log_info("[playscreen] map renderer ready");

    // Cache common effect imagery for damage/spell overlays. Optional --
    // missing imagery is logged but doesn't gate boot.
    auto load = [](const char* path) -> TObjectImagery* {
        const int32_t id = TObjectImagery::FindImagery(path);
        if (id < 0) return nullptr;
        return TObjectImagery::LoadImagery(id);
    };
    bloodimagery  = load("Misc\\Blood.I3D");
    sparksimagery = load("Misc\\Sparks.I3D");

    // Default keyboard bindings. Direction keys + Run / Sneak; classic
    // arrow + numpad-diagonal layout. UpdateMove synthesizes diagonals
    // from adjacent cardinals so laptop users without a Home/PgUp/End/
    // PgDn cluster still get full 8-way movement.
    ControlMap.Initialize(int32_t(sizearray(g_defaultGameControls)),
                          g_defaultGameControls);

    EditorLoadState();

    // Load the active module's area.def (forest / Misthaven / House
    // Interior / etc.). Until this runs, area->Enter() never fires and
    // MapPane stays at its default ambient. The Ahkuilon module ZIP is
    // already mounted by InitGlobals so AreaManager.Initialize finds
    // area.def via rev_fopen + VFS.
    if (!AreaManager.Initialize())
        log_warn("[playscreen] AreaManager.Initialize failed; ambient will fall back to MapPane defaults");

    // Runtime mode owns mode-specific UI state (cursor, overlay
    // visibility, etc.). At static init g_currentMode defaults to game
    // mode, but OnEnter() is only invoked by SetCurrentMode for
    // *transitions* -- so the initial mode never gets its setup hook
    // fired. Invoke it explicitly here so the game cursor (and any
    // future game-mode init) is in place from the first frame.
    if (CurrentMode())
        CurrentMode()->OnEnter();

    log_info("[playscreen] initialize done");
    return true;
}

// Hand-rolled starter loadout for Demo 1. Stand-in until newgame.sav
// loading (see [docs/gameplay/BURNDOWN.md](../docs/gameplay/BURNDOWN.md)
// phase E) supersedes this. Each entry: (item name as known to
// class.def, target equipment slot). Name lookup scans every
// TObjectClass so we don't have to hand-pick OBJCLASS_WEAPON vs
// OBJCLASS_ARMOR per row. Order matters: PRIMEHAND first so the
// combat-root re-resolution in TPlayer::Equip has its target before
// the body armor decoration lands on top.
// Item names per the runtime class.def packed in imagery.rvi (not the
// older legacy/Class.Def at repo root, which uses different identifiers).
// Brown Cloth set is the unarmored peasant look Locke wakes up in.
struct SStarterItem { const char *name; int32_t slot; };
static const SStarterItem kStarterLoadout[] = {
    { "Short Sword",         EQ_PRIMEHAND },
    { "Brown Cloth Shirt",   EQ_BODY      },
    { "Brown Cloth Pants",   EQ_LEGS      },
    { "Brown Cloth Boots",   EQ_FEET      },
};

static void EquipStarterLoadout(TPlayer *p)
{
    if (!p) return;
    for (const SStarterItem &row : kStarterLoadout)
    {
        TObjectClass *cl = nullptr;
        int32_t ot = -1;
        for (int32_t i = 0; i < MAXOBJECTCLASSES && !cl; ++i)
        {
            TObjectClass *c = TObjectClass::GetClass(i);
            if (!c) continue;
            const int32_t t = c->FindObjType(row.name);
            if (t >= 0) { cl = c; ot = t; }
        }
        if (!cl)
        {
            log_warn("[player] starter: no class.def type named '%s'", row.name);
            continue;
        }

        SObjectDef def;
        memset(&def, 0, sizeof(def));
        def.objclass = cl->ClassId();
        def.objtype  = ot;
        p->GetPos(def.pos);    // placeholder; AddToInventory reparents

        TObjectInstance *inst = cl->NewObject(&def);
        if (!inst)
        {
            log_warn("[player] starter: NewObject('%s') failed", row.name);
            continue;
        }
        if (!p->AddToInventory(inst))
        {
            log_warn("[player] starter: AddToInventory('%s') failed", row.name);
            continue;
        }
        if (!p->Equip(inst, row.slot))
        {
            log_warn("[player] starter: Equip('%s', slot=%d) refused "
                     "(EqSlot mismatch or missing combat ani)",
                     row.name, row.slot);
            continue;
        }
        log_info("[player] starter: equipped '%s' in slot %d", row.name, row.slot);
    }
}

// Drop a "Locke" TPlayer into the loaded sector at (level, sx, sy) and
// register with PlayerManager. Called from inside the TMapRenderer
// post-load hook so the player exists in the sector before the
// renderer scans sector contents into its drawable list. Skipped if a
// Player already exists (loaded from a save).
bool TPlayScreen::SpawnDefaultPlayer(int32_t level, int32_t sx, int32_t sy)
{
    if (Player)
        return true;
    if (!mapRenderer)
        return false;

    TSector* sec = mapRenderer->FindLoadedSector(level, sx, sy);
    if (!sec)
    {
        log_warn("[player] anchor sector %d_%d_%d not loaded; cannot spawn Locke",
                 level, sx, sy);
        return false;
    }

    const int32_t locke_type = PlayerClass.FindObjType("Locke");
    if (locke_type < 0)
    {
        log_error("[player] PlayerClass has no 'Locke' objtype");
        return false;
    }

    // Sector-center is the Misthaven fountain at the default anchor.
    // Offset northwest of center onto open plaza paving so Locke isn't
    // stuck on / against the fountain geometry. Picked empirically --
    // TODO: replace with a real start-of-game position once save-load
    // / a NewGame data path is wired up.
    SObjectDef def;
    memset(&def, 0, sizeof(def));
    def.objclass = OBJCLASS_PLAYER;
    def.objtype  = locke_type;
    def.level    = level;
    def.pos.x    = (sx << SECTORWSHIFT) + (SECTORWIDTH  / 2) - 128;
    def.pos.y    = (sy << SECTORHSHIFT) + (SECTORHEIGHT / 2) - 128;
    // Read the floor height from the sector walkmap so Locke spawns
    // on top of the terrain instead of beneath it. MapPane's level
    // window doesn't follow Locke yet (paging not wired); read off
    // the renderer-owned sector directly.
    {
        const int32_t local_x = (def.pos.x & (SECTORWIDTH  - 1)) >> WALKMAPSHIFT;
        const int32_t local_y = (def.pos.y & (SECTORHEIGHT - 1)) >> WALKMAPSHIFT;
        def.pos.z = sec->ReturnWalkmap(local_x, local_y);
        log_info("[player] spawn: world=(%d,%d,%d) sector=(%d_%d_%d) "
                 "walkmap[%d,%d]=%d",
                 def.pos.x, def.pos.y, def.pos.z, level, sx, sy,
                 local_x, local_y, def.pos.z);

        // One-shot Z reference probes: walk every loaded sector of the current
        // map and log up to two example pos.z values -- one from any
        // OBJCLASS_CHARACTER (NPC / monster, since monsters are CHARACTER too),
        // one from the first non-tile static prop. Both render at correct Z
        // visually, so comparing their stored pos.z + walkmap-under-feet to
        // ours tells us whether Locke's spawn uses the wrong reference value.
        if (TGameMap* gm = MapManager.CurrentMap())
        {
            int32_t char_logged = 0, prop_logged = 0;
            for (TSector* probe_sec : gm->Sectors())
            {
                if (!probe_sec) continue;
                if (char_logged && prop_logged) break;
                for (int32_t i = 0; i < probe_sec->NumItems(); ++i)
                {
                    TObjectInstance* probe = probe_sec->GetInstance(i);
                    if (!probe) continue;
                    const int32_t cls = probe->ObjClass();
                    const bool is_char = (cls == OBJCLASS_CHARACTER);
                    const bool is_prop = (cls != OBJCLASS_TILE &&
                                          cls != OBJCLASS_EXIT &&
                                          cls != OBJCLASS_EFFECT &&
                                          cls != OBJCLASS_LIGHTSOURCE &&
                                          cls != OBJCLASS_HELPER &&
                                          cls != OBJCLASS_PLAYER &&
                                          cls != OBJCLASS_CHARACTER);
                    if (!is_char && !is_prop) continue;
                    if (is_char && char_logged) continue;
                    if (is_prop && prop_logged) continue;
                    S3DPoint pp; probe->GetPos(pp);
                    const int32_t plx = (pp.x & (SECTORWIDTH  - 1)) >> WALKMAPSHIFT;
                    const int32_t ply = (pp.y & (SECTORHEIGHT - 1)) >> WALKMAPSHIFT;
                    const int32_t pwalk = probe_sec->ReturnWalkmap(plx, ply);
                    log_info("[player] z-probe %s '%s' cls=%d pos=(%d,%d,%d) "
                             "sector=(%d_%d) walkmap[%d,%d]=%d delta(z-walk)=%d",
                             is_char ? "CHAR" : "PROP",
                             probe->GetTypeName() ? probe->GetTypeName() : "?",
                             cls, pp.x, pp.y, pp.z,
                             probe_sec->SectorX(), probe_sec->SectorY(),
                             plx, ply, pwalk, pp.z - pwalk);
                    if (is_char) ++char_logged;
                    else         ++prop_logged;
                    if (char_logged && prop_logged) break;
                }
            }
            if (!char_logged) log_info("[player] z-probe: no OBJCLASS_CHARACTER found in current map");
            if (!prop_logged) log_info("[player] z-probe: no static prop found in current map");
        }
    }

    TObjectInstance* oi = PlayerClass.NewObject(&def);
    if (!oi)
    {
        log_error("[player] PlayerClass.NewObject failed for Locke");
        return false;
    }

    // The Aggressive stat defaults to 1 in DEFOBJSTAT(Character, Aggressive,
    // ..., 1, 0, 1). The retail rules.def CHAR entry for the player class
    // overrode that to 0; with the fallback SCharData (no rules.def CHAR
    // entries) Locke inherits the default and spawns in "combat" root.
    // That gates all movement on combat-walk animations and effectively
    // freezes him. Force walk-mode by clearing Aggressive and re-running
    // the character init so DefaultRootState resolves to "walk".
    TCharacter* c = (TCharacter*)oi;
    c->SetAggressive(0);
    c->ClearChar();


    sec->AddObject(oi);

    // SetMainPlayer fires UI side effects (CenterOnObj / RefreshEquip /
    // Inventory / HealthBar / StaminaBar) when CurrentScreen == &PlayScreen.
    // We are inside TPlayScreen::Initialize -- CurrentScreen is already
    // set, but the UI panes those calls touch are not yet initialized,
    // and one of them hangs. Clear CurrentScreen for the duration of
    // the spawn so the UI hookups are skipped; the player still becomes
    // the main player, just without the per-screen redraws.
    TScreen* saved_screen = CurrentScreen;
    CurrentScreen = nullptr;
    PlayerManager.AddPlayer((TPlayer*)oi);
    PlayerManager.SetMainPlayer((TPlayer*)oi);
    EquipStarterLoadout((TPlayer*)oi);
    CurrentScreen = saved_screen;

    log_info("[player] spawned Locke in sector %d_%d_%d at world (%d,%d,%d)",
             level, sx, sy, def.pos.x, def.pos.y, def.pos.z);

    // One-shot stat dump so we can see what ClearPlayer produced and
    // confirm the rules.def/char.def + parser fix actually populated
    // Locke's stats correctly.
    {
        TPlayer *p = (TPlayer*)oi;
        const SCharData *cd  = p->GetCharData();
        const SClassData *kd = (cd && cd->classdata) ? cd->classdata : nullptr;
        if (cd)
        {
            for (int32_t i = 0; i < cd->attacks.NumItems() && i < 6; i++)
            {
                const SCharAttackData &a = cd->attacks[i];
                log_info("[player-attacks] [%d] name='%s' btn=%d flags=0x%x "
                         "mindist=%d maxdist=%d fatigue=%d weaponmask=0x%x",
                         i, a.attackname, (int)a.button, (unsigned)a.flags,
                         (int)a.mindist, (int)a.maxdist,
                         (int)a.fatigue, (unsigned)a.weaponmask);
            }
        }
        log_info("[player-stats] Lv=%d STR=%d CON=%d AGI=%d RFL=%d MND=%d LCK=%d "
                 "H=%d/%d F=%d/%d M=%d/%d "
                 "chardata=%s classdata=%s "
                 "fatiguemod=%d healthmod=%d manamod=%d "
                 "statreqs=[%d,%d,%d,%d,%d,%d]",
                 (int)p->Level(),
                 (int)p->Strn(), (int)p->Cons(), (int)p->Agil(),
                 (int)p->Rflx(), (int)p->Mind(), (int)p->Luck(),
                 (int)p->Health(), (int)p->MaxHealth(),
                 (int)p->Fatigue(), (int)p->MaxFatigue(),
                 (int)p->Mana(), (int)p->MaxMana(),
                 cd ? cd->name : "(null)",
                 kd ? "yes" : "(null)",
                 kd ? kd->fatiguemod : -1,
                 kd ? kd->healthmod  : -1,
                 kd ? kd->manamod    : -1,
                 kd ? kd->statreqs[0] : -1, kd ? kd->statreqs[1] : -1,
                 kd ? kd->statreqs[2] : -1, kd ? kd->statreqs[3] : -1,
                 kd ? kd->statreqs[4] : -1, kd ? kd->statreqs[5] : -1);

        // One-shot dump of Locke's animation state names + their play-tag
        // counts. Answers two open questions: does a "run" state exist
        // (run-mode toggle), and which states carry "play" sound tags
        // (footsteps). Remove once both are wired.
        if (TObjectImagery *img = p->GetImagery())
        {
            const int32_t n = img->NumStates();
            log_info("[anim-dump] Locke has %d animation states:", n);
            for (int32_t i = 0; i < n; i++)
            {
                SImageryHeader *hdr = img->GetHeader();
                const char *nm = (hdr && i < hdr->numstates) ? hdr->states[i].animname : "?";
                log_info("[anim-dump]   [%d] '%s'", i, nm ? nm : "?");
            }
        }
    }

    return true;
}

void TPlayScreen::Close()
{
    AreaManager.Close();
    if (mapRenderer)
    {
        mapRenderer->Shutdown();
        mapRenderer.reset();
    }
    // TScreen::Close is a no-op base hook; skip the call.
}

// *************************************************************************
// * Per-frame                                                             *
// *************************************************************************

void TPlayScreen::Update()
{
    // Apply any deferred pane add scheduled by SetNextPane().
    if (nextpane)
    {
        AddPane(nextpane);
        nextpane = nullptr;
    }

    // Honor save / load requests staged from the input layer or scripts.
    if (loadgame)
    {
        loadgame = false;
        if (loadgamepath[0])
        {
            log_info("[playscreen] loading save '%s'", loadgamepath);
            ::SaveGame.ReadGame(loadgamepath);
            loadgamepath[0] = '\0';
        }
        else
        {
            ::SaveGame.ReadGame(gamenum);
        }
    }
    if (savegame)
    {
        savegame = false;
        ::SaveGame.WriteGame(gamenum);
    }

    // Per-frame work that differs between game and editor: input ->
    // movement, pulse / move over the active sector window, camera
    // follow. Editor mode's Tick is a no-op (the editor drives camera
    // + pulse itself).
    CurrentMode()->Tick();

    // Tick the area system: detects player Enter/Exit of each TArea's
    // RECTs, runs day/night ambient interpolation, fires CDPLAYLIST /
    // AUDIOENV transitions. Must run after CurrentMode()->Tick() so
    // MapPane.GetMapPos reflects this frame's player position.
    AreaManager.Pulse();

    // Advance fixed-tick counters. CurrentMode()->Tick() owns gameplay frame
    // advancement; the renderer only samples/interpolates the current pose.
    ++gameframes;
    gametime = lastsessionframes
             + (gameframes - sessionstart) * 100 / kGameFrameRate;
    timeofday = TimeOfDayMinutes(gametime);
}

void TPlayScreen::RenderFrame()
{
    if (!mapRenderer) return;
    // Late camera update: gameplay has already moved the player for this
    // fixed tick, so follow from the final transform immediately before
    // rendering. The renderer compensates the camera origin by the followed
    // height so Locke stays centered while walking up/down terrain.
    if (CurrentMode() == GameMode() && Player)
    {
        S3DPoint p;
        Player->GetPos(p);
        mapRenderer->SetCameraWorld(Player->GetLevel(), p.x, p.y, p.z);
    }

    // While the editor is paused and nothing is dirty, skip the world
    // render entirely. The renderer's lit_target persists, so the Game
    // View panel keeps displaying the previously rendered frame.
    if (!EditorShouldRenderWorld()) return;
    mapRenderer->RenderFrame();
}

// Diagnostic overlay: prints Locke's world position, walkmap height
// under his feet, the delta that drives TCharacter::Blocked, sector
// id, and current action. Game-mode only -- editor has its own
// inspectors. Cheap; lifetime is the duration of the player-input
// bring-up and can be removed once movement reliably works.
static void DrawPlayerStatusOverlay()
{
    if (!Player) return;
    if (CurrentMode() != GameMode()) return;

    S3DPoint p; Player->GetPos(p);
    const int32_t lvl = Player->GetLevel();
    const int32_t walk = MapPane.GetWalkHeight(p);
    const int32_t z_delta = p.z - walk;
    const char* state_name = Player->GetStateName();
    const int32_t sx_world = p.x >> SECTORWSHIFT;
    const int32_t sy_world = p.y >> SECTORHSHIFT;
    const int32_t radius = ((TCharacter*)Player)->Radius();
    int32_t r_min = 0, r_max = 0, r_h = 0;
    MapPane.GetWalkHeightRadius(p, radius, r_min, r_max, r_h);
    const TCharacter* blocker = ((TCharacter*)Player)->CharBlocking();
    const bool z_blocks    = std::abs(z_delta) > 32;
    const bool no_floor    = (walk == 0);
    const bool slope_min   = std::abs(r_min) > 32;
    const bool slope_max   = std::abs(r_max) > 32;
    const bool char_blocks = (blocker != nullptr);

    TObjectInstance* pinst = static_cast<TObjectInstance*>(Player);
    const int32_t st_idx   = pinst->GetState();
    const int32_t st_total = Player->NumStates();
    TObjectImagery* img    = Player->GetImagery();
    const int32_t fr_total = img ? img->GetAniLength(st_idx) : 0;
    const uint32_t ani_fl  = Player->GetAniFlags();
    char flagstr[64] = {};
    int  fpos = 0;
    auto add_flag = [&](const char* tag, uint32_t mask) {
        if ((ani_fl & mask) && fpos < int(sizeof(flagstr)) - 12) {
            fpos += std::snprintf(flagstr + fpos, sizeof(flagstr) - fpos,
                                  fpos ? "|%s" : "%s", tag);
        }
    };
    add_flag("LOOP", AF_LOOPING);
    add_flag("PP",   AF_PINGPONG);
    add_flag("ROOT", AF_ROOT);
    add_flag("R2R",  AF_ROOT2ROOT);
    add_flag("FLY",  AF_FLY);
    add_flag("MOVE", AF_MOVE);
    const int32_t prev_idx = pinst->GetPrevState();
    const char* prev_name = (img && prev_idx >= 0 && prev_idx < img->NumStates())
        ? img->GetAniName(prev_idx)
        : "?";

    TCharacter* pc = static_cast<TCharacter*>(Player);
    const SCharData* pcd = pc->GetCharData();

    ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(460, 340), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.55f);
    if (ImGui::Begin("Player Debug", nullptr, ImGuiWindowFlags_NoNav))
    {
        if (ImGui::BeginTabBar("PlayerDebugTabs"))
        {
            if (ImGui::BeginTabItem("Move"))
            {
                ImGui::Text("pos     : (%d, %d, %d)  level=%d", p.x, p.y, p.z, lvl);
                ImGui::Text("sector  : %d_%d", sx_world, sy_world);
                ImGui::Text("walkmap : %d  (z-walk = %d)", walk, z_delta);
                ImGui::Text("radius  : %d   r_h=%d  min=%d  max=%d",
                            radius, r_h, r_min, r_max);
                ImGui::Text("would block: %s%s%s%s%s%s",
                            z_blocks    ? "Z>32 "       : "",
                            no_floor    ? "no-floor "   : "",
                            slope_min   ? "min>32 "     : "",
                            slope_max   ? "max>32 "     : "",
                            char_blocks ? "char-blk "   : "",
                            (!z_blocks && !no_floor && !slope_min &&
                             !slope_max && !char_blocks) ? "no" : "");
                ImGui::Text("moving  : %s   moveangle=%d   anim=%s",
                            Player->IsMoving() ? "yes" : "no",
                            Player->GetMoveAngle(),
                            state_name ? state_name : "?");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Anim"))
            {
                ImGui::Text("anim    : %s", state_name ? state_name : "?");
                ImGui::Text("state   : %d/%d  frame=%d  last=%d  count=%d  rate=%d",
                            st_idx, st_total, pinst->GetFrame(),
                            fr_total > 0 ? fr_total - 1 : -1,
                            fr_total, pinst->GetFrameRate());
                ImGui::Text("flags   : %s  done=%s  animator=%s",
                            fpos ? flagstr : "-",
                            Player->CommandDone() ? "yes" : "no",
                            Player->HasAnimator() ? "yes" : "NO");
                ImGui::Text("prev    : state=%d \"%s\"  frame=%d",
                            prev_idx, prev_name, pinst->GetPrevFrame());
                if (T3DAnimator* d3 = dynamic_cast<T3DAnimator*>(pinst->GetAnimator()))
                {
                    ImGui::Text("bridge  : transition=%d state=%d prev=%d/%d",
                                d3->DebugTransitionActive() ? 1 : 0,
                                d3->DebugTransitionState(),
                                d3->DebugTransitionPrevState(),
                                d3->DebugTransitionPrevFrame());
                    ImGui::Text("bridge  : last=%d high=%d",
                                d3->DebugTransitionLastFrame(),
                                d3->DebugTransitionHighestFrame());
                    ImGui::Text("render  : frame=%d next=%d/%d frac=%.3f",
                                d3->DebugPoseUpdateFrame(),
                                d3->DebugPoseUpdateNextState(),
                                d3->DebugPoseUpdateNextFrame(),
                                d3->DebugPoseUpdateFrameFrac());
                }
                ImGui::Separator();
                ImGui::Text("sets    : total=%u same=%u",
                            pinst->DebugStateSetCount(),
                            pinst->DebugSameStateSetCount());
                ImGui::Text("last set: %d->%d at frame=%d gf=%d",
                            pinst->DebugLastStateSetFrom(),
                            pinst->DebugLastStateSetTo(),
                            pinst->DebugLastStateSetFrame(),
                            pinst->DebugLastStateSetGameFrame());
                ImGui::Text("wraps   : total=%u",
                            pinst->DebugLoopWrapCount());
                ImGui::Text("last wrap: state=%d frame=%d gf=%d",
                            pinst->DebugLastLoopWrapState(),
                            pinst->DebugLastLoopWrapFrame(),
                            pinst->DebugLastLoopWrapGameFrame());
                ImGui::Separator();
                ImGui::Text("doing   : action=%d name=\"%s\"",
                            pc->DoingAction(), pc->DoingName());
                ImGui::Text("desired : action=%d name=\"%s\"",
                            pc->DesiredAction(), pc->DesiredName());
                ImGui::Text("root    : action=%d name=\"%s\"",
                            pc->RootAction(), pc->RootName());
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Combat"))
            {
                ImGui::Text("mode    : combat=%d bow=%d walk=%d sneak=%d run=%d",
                            pc->IsCombat() ? 1 : 0,
                            pc->IsBowMode() ? 1 : 0,
                            pc->IsWalkMode() ? 1 : 0,
                            pc->IsSneakMode() ? 1 : 0,
                            pc->IsRunMode() ? 1 : 0);
                if (pcd && pcd->classdata)
                {
                    const SClassData* cd = pcd->classdata;
                    ImGui::Text("classmod: H=%d F=%d M=%d  statreqs=[%d,%d,%d,%d,%d,%d]",
                                cd->healthmod, cd->fatiguemod, cd->manamod,
                                cd->statreqs[0], cd->statreqs[1], cd->statreqs[2],
                                cd->statreqs[3], cd->statreqs[4], cd->statreqs[5]);
                }
                if (pcd)
                    ImGui::Text("chardata: name=\"%s\" numattacks=%d combatrng=%d/%d attkrng=%d",
                                pcd->name, pcd->attacks.NumItems(),
                                pcd->combatrangemin, pcd->combatrangemax,
                                pcd->maxattackrange);
                else
                    ImGui::Text("chardata: NULL");
                ImGui::Text("hasAni  : combat=%d swing=%d thrust=%d chop=%d",
                            Player->HasActionAni("combat") ? 1 : 0,
                            Player->HasActionAni("swing")  ? 1 : 0,
                            Player->HasActionAni("thrust") ? 1 : 0,
                            Player->HasActionAni("chop")   ? 1 : 0);
                ImGui::EndTabItem();
            }

            if (pcd && ImGui::BeginTabItem("Attacks"))
            {
                const int32_t wt   = pc->WeaponType();
                const int32_t wbit = 1 << wt;
                TPlayer* tpl = (Player->ObjClass() == OBJCLASS_PLAYER) ? (TPlayer*)Player : nullptr;
                const int32_t skAtk = tpl ? tpl->Skill(SK_ATTACK) : 999;
                ImGui::Text("player  : weapontype=%d mask=0x%x attackskill=%d",
                            wt, wbit, skAtk);
                ImGui::Text("fatigue : %d/%d", pc->Fatigue(), pc->MaxFatigue());
                if (tpl)
                {
                    ImGui::Text("stats   : Lv=%d Exp=%d STR=%d CON=%d AGI=%d RFL=%d MND=%d LCK=%d",
                                tpl->Level(), tpl->Exp(),
                                tpl->Strn(), tpl->Cons(), tpl->Agil(),
                                tpl->Rflx(), tpl->Mind(), tpl->Luck());
                    ImGui::Text("max     : H=%d/%d F=%d/%d M=%d/%d classdata=%s",
                                tpl->Health(), tpl->MaxHealth(),
                                tpl->Fatigue(), tpl->MaxFatigue(),
                                tpl->Mana(), tpl->MaxMana(),
                                (pcd->classdata ? pcd->classdata->name : "(NULL)"));
                }
                ImGui::Separator();
                for (int btn = 1; btn <= 3; ++btn)
                {
                    int with_button = 0, after_anim = 0, after_mode = 0,
                        after_weapon = 0, after_skill = 0, after_wskill = 0,
                        after_fatigue = 0;
                    const char* first_pass = nullptr;
                    for (int32_t i = 0; i < pcd->attacks.NumItems(); ++i)
                    {
                        const SCharAttackData& ad = pcd->attacks[i];
                        if (ad.button != btn) continue;
                        ++with_button;
                        if (!Player->HasActionAni(ad.attackname)) continue;
                        ++after_anim;
                        bool mode_ok = true;
                        if ((ad.flags & CA_SNEAKMODE) && !pc->IsSneakMode()) mode_ok = false;
                        else if ((ad.flags & CA_WALKMODE) && !pc->IsWalkMode()) mode_ok = false;
                        else if ((ad.flags & CA_BOWMODE) && !pc->IsBowMode()) mode_ok = false;
                        else if (!pc->IsCombat()) mode_ok = false;
                        if (!mode_ok) continue;
                        ++after_mode;
                        if (Player->ObjClass() == OBJCLASS_PLAYER)
                        {
                            TPlayer* pl = (TPlayer*)Player;
                            if (!(ad.weaponmask & wbit)) continue;
                            ++after_weapon;
                            if (pl->Skill(SK_ATTACK) < ad.attackskill) continue;
                            ++after_skill;
                            if (pl->WeaponSkill(wt) < ad.weaponskill) continue;
                            ++after_wskill;
                        }
                        else
                        {
                            after_weapon = after_skill = after_wskill = after_mode;
                        }
                        if (pc->Fatigue() < ad.fatigue) continue;
                        ++after_fatigue;
                        if (!first_pass) first_pass = ad.attackname;
                    }
                    ImGui::Text("btn%d: btn=%d ani=%d mode=%d wpn=%d skl=%d wsk=%d fat=%d [%s]",
                                btn, with_button, after_anim, after_mode,
                                after_weapon, after_skill, after_wskill,
                                after_fatigue,
                                first_pass ? first_pass : "(rejected)");
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

// Closest-monster diagnostic overlay. Walks the active sector window
// and finds the OBJCLASS_CHARACTER instance closest to the player
// (excluding dead, hidden, etc.). Surfaces the AI-relevant fields so
// "why is this monster not engaging" is answerable without instrumented
// logging on every Pulse. Game-mode only.
static void DrawClosestMonsterOverlay()
{
    if (!Player) return;
    if (CurrentMode() != GameMode()) return;

    const S3DPoint pp = Player->Pos();
    TCharacter*    closest      = nullptr;
    int32_t        closest_dist = INT32_MAX;

    if (TGameMap* gm = MapManager.CurrentMap())
    {
        for (TSector* sec : gm->Sectors())
        {
            if (!sec) continue;
            for (int32_t i = 0; i < sec->NumItems(); ++i)
            {
                TObjectInstance* oi = sec->GetInstance(i);
                if (!oi) continue;
                if (oi->ObjClass() != OBJCLASS_CHARACTER) continue;
                if (oi == (TObjectInstance*)Player) continue;
                S3DPoint mp = oi->Pos();
                const int32_t dx = mp.x - pp.x;
                const int32_t dy = mp.y - pp.y;
                const int32_t d2 = dx * dx + dy * dy;
                if (d2 < closest_dist)
                {
                    closest_dist = d2;
                    closest = (TCharacter*)oi;
                }
            }
        }
    }

    ImGui::SetNextWindowPos(ImVec2(8, 220), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.55f);
    if (!ImGui::Begin("Closest Monster", nullptr,
                      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    if (!closest)
    {
        ImGui::Text("(no character in current map)");
        ImGui::End();
        return;
    }

    const S3DPoint mp = closest->Pos();
    const int32_t  dx = mp.x - pp.x;
    const int32_t  dy = mp.y - pp.y;
    const int32_t  flat_dist = int32_t(std::sqrt(double(dx) * dx + double(dy) * dy));
    const SCharData* cd = closest->GetCharData();

    ImGui::Text("name    : %s  type=%s  class=%d",
                closest->GetName() ? closest->GetName() : "?",
                closest->GetTypeName() ? closest->GetTypeName() : "?",
                closest->ObjClass());
    ImGui::Text("pos     : (%d, %d, %d)  dist=%d", mp.x, mp.y, mp.z, flat_dist);
    ImGui::Text("flags   : aggressive=%d dead=%d disabled=%d moving=%d fighting=%d",
                closest->Aggressive() ? 1 : 0,
                closest->IsDead() ? 1 : 0,
                (closest->Flags() & OF_DISABLED) ? 1 : 0,
                closest->IsMoving() ? 1 : 0,
                closest->IsFighting() ? 1 : 0);
    ImGui::Text("global  : NoAI=%d Editor=%d", NoAI ? 1 : 0, Editor ? 1 : 0);
    ImGui::Text("ticks   : pulse=%u ai=%u", closest->ai_pulse_count, closest->ai_ai_count);
    if (TCharacter* tgt = closest->Fighting())
        ImGui::Text("target  : %s  dist=%d",
                    tgt->GetName() ? tgt->GetName() : "?",
                    closest->Distance(tgt));
    else
        ImGui::Text("target  : (none)");
    if (cd)
        ImGui::Text("chardata: walkspd=%d combatrng=%d/%d attkrng=%d freq=%d-%d",
                    cd->walkspeed, cd->combatrangemin, cd->combatrangemax,
                    cd->maxattackrange, cd->minattackfreq, cd->maxattackfreq);
    else
        ImGui::Text("chardata: NULL");
    const char* anim = closest->GetStateName();
    TObjectInstance* cinst = static_cast<TObjectInstance*>(closest);
    TObjectImagery* cimg = closest->GetImagery();
    const int32_t cstate = cinst->GetState();
    const int32_t clen = cimg ? cimg->GetAniLength(cstate) : 0;
    ImGui::Text("anim    : %s  state=%d frame=%d/%d prev=(%d,%d) done=%d",
                anim ? anim : "?",
                cstate, cinst->GetFrame(), clen,
                cinst->GetPrevState(), cinst->GetPrevFrame(),
                closest->CommandDone() ? 1 : 0);
    ImGui::Text("sets    : total=%u same=%u last %d->%d frame=%d gf=%d",
                cinst->DebugStateSetCount(),
                cinst->DebugSameStateSetCount(),
                cinst->DebugLastStateSetFrom(),
                cinst->DebugLastStateSetTo(),
                cinst->DebugLastStateSetFrame(),
                cinst->DebugLastStateSetGameFrame());
    ImGui::Text("wraps   : total=%u last state=%d frame=%d gf=%d",
                cinst->DebugLoopWrapCount(),
                cinst->DebugLastLoopWrapState(),
                cinst->DebugLastLoopWrapFrame(),
                cinst->DebugLastLoopWrapGameFrame());

    ImGui::Text("doing   : action=%d name=\"%s\"  target=(%d,%d)",
                closest->DoingAction(), closest->DoingName(),
                closest->DoingTargetX(), closest->DoingTargetY());
    ImGui::Text("desired : action=%d name=\"%s\"",
                closest->DesiredAction(), closest->DesiredName());
    ImGui::Text("root    : action=%d name=\"%s\"",
                closest->RootAction(), closest->RootName());
    ImGui::Text("attack  : nextattack=%d radius=%d numattacks=%d",
                closest->NextAttack(), closest->Radius(),
                cd ? cd->attacks.NumItems() : -1);

    ImGui::End();
}

// Legacy entry points still referenced by drivers / pane code. Pulse
// pumps the per-frame state update; Animate fires the world render
// (matching what TTestScreen does for TestModes::Render). DrawBackground
// is dead -- no BITMAP.100 backdrop on the new path.
void TPlayScreen::Pulse()                  { Update(); }
void TPlayScreen::Animate(bool /*draw*/)   { RenderFrame(); EditorDrawChrome(); DrawPlayerStatusOverlay(); DrawClosestMonsterOverlay(); }
void TPlayScreen::DrawBackground()         { /* no backdrop blit on the new path */ }

// *************************************************************************
// * Input                                                                 *
// *************************************************************************

void TPlayScreen::KeyPress(int32_t key, bool down)
{
    // Editor toggle. F12 is the retail-era hotkey -- always handled at
    // the screen level so the user can flip modes regardless of who
    // currently owns input.
    if (down && key == VK_F12)
    {
        if (Editor)
        {
            ShutDownEditor();
            Redraw();
        }
        else
        {
            StartEditor();
        }
        return;
    }

    // Give the active mode first crack at the key (game-mode movement
    // bindings, etc.). If it consumes the event we stop here.
    if (CurrentMode()->HandleKey(key, down)) return;

    TScreen::KeyPress(key, down);
}

void TPlayScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TScreen::MouseClick(button, x, y);
    // Route to the active runtime mode; editor mode forwards to the
    // renderer's gizmo / drag picker, game mode forwards to MapPane's
    // gameplay click handler (movement / interact / combat).
    CurrentMode()->HandleMouseClick(button, x, y);
}

void TPlayScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TScreen::MouseMove(button, x, y);
    // Game mode -> MapPane.MouseMove (drives wedge cursor while walking,
    // bow aim, etc.). Editor mode default no-op until we wire its
    // hover/drag overlays.
    CurrentMode()->HandleMouseMove(button, x, y);
}

void TPlayScreen::Joystick(int32_t /*key*/, bool /*down*/)
{
    // TODO(port): wire sokol_app gamepad events. Stubbed for now.
}

void TPlayScreen::Command(GAMECOMMAND /*command*/)
{
    // TODO(port): full GAMECOMMAND dispatch (combat / inventory / spells
    // / dodge / leap / etc.). Tracked alongside the player-input revival.
}

void TPlayScreen::UpdateMove()
{
    if (!Player) return;

    uint32_t state, changed;
    ControlMap.GetCommandFlags(state, changed);

    // Run / sneak mode toggles. The 'R' and 'S' (in sneak-mode binding)
    // keys carry a CMDFLAG_RUN / CMDFLAG_SNEAK bit alongside their
    // GAMECMD_MOVEDOWN dispatch -- ControlMap maintains the bit while
    // held, and `changed` flags the bits that flipped this poll. Press
    // edge -> swap the player's root animation to run/sneak; release
    // edge -> swap back to walk. Both keyboard direction keys AND the
    // mouse walk-to path then naturally pick up the new root, so
    // hold-R + right-click runs toward the cursor, etc.
    if (changed & CMDFLAG_RUN)
    {
        if (state & CMDFLAG_RUN) Player->SetRunMode();
        else                     Player->SetWalkMode();
    }
    if (changed & CMDFLAG_SNEAK)
    {
        if (state & CMDFLAG_SNEAK) Player->SetSneakMode();
        else                       Player->SetWalkMode();
    }

    // Synthesize diagonal flags from adjacent cardinals so keyboards
    // without a Home / PgUp / End / PgDn cluster can still walk
    // diagonally with two arrow keys. The retail bit-scan picks the
    // lowest set CMDFLAG bit, so without this UP+RIGHT would yield
    // pure RIGHT instead of UPRIGHT. We OR the diagonal in and clear
    // the cardinals so the scan resolves cleanly.
    auto synthesize_diagonal = [](uint32_t s, uint32_t a, uint32_t b, uint32_t diag) -> uint32_t {
        if ((s & a) && (s & b)) s = (s & ~(a | b)) | diag;
        return s;
    };
    state = synthesize_diagonal(state, CMDFLAG_UP,   CMDFLAG_RIGHT, CMDFLAG_UPRIGHT);
    state = synthesize_diagonal(state, CMDFLAG_UP,   CMDFLAG_LEFT,  CMDFLAG_UPLEFT);
    state = synthesize_diagonal(state, CMDFLAG_DOWN, CMDFLAG_RIGHT, CMDFLAG_DOWNRIGHT);
    state = synthesize_diagonal(state, CMDFLAG_DOWN, CMDFLAG_LEFT,  CMDFLAG_DOWNLEFT);

    // Bow-aim mode: left/right adjust aim instead of moving.
    if (Player->IsBowMode() && Player->IsBowDrawn())
    {
        if      (state & CMDFLAG_LEFT)  Player->AimBowLeft();
        else if (state & CMDFLAG_RIGHT) Player->AimBowRight();
        else                            return;
    }

    // Direction flags occupy bits 0..7 in clockwise order starting
    // from UPRIGHT; the lowest set bit picks the angle (steps of 32).
    int32_t angle = 0;
    int32_t c;
    for (c = 1; c < (1 << 8) && !(c & state); c <<= 1, angle += 32);
    if (angle > 255) angle = -1;  // no direction held

    if (angle >= 0)
    {
        if (Player->IsFighting() && (state & CMDFLAG_LEAP))
            Player->Leap(angle);
        else if (Player->GetMoveAngle() != angle ||
                 !(Player->IsDoing(ACTION_MOVE) || Player->IsDoing(ACTION_COMBATMOVE)))
            Player->Go(angle);
    }
    else if (changed && Player->IsMoving() && !Player->IsGoto())
    {
        Player->Stop();
    }

    // Block / unblock toggle (combat mode).
    if ((state & CMDFLAG_BLOCK) &&
        Player->IsFighting() && !Player->IsDoing(ACTION_BLOCK))
        Player->Block(10000);
    if (!(state & CMDFLAG_BLOCK) && Player->IsDoing(ACTION_BLOCK))
        Player->StopBlock();
}

// *************************************************************************
// * Mode flags                                                            *
// *************************************************************************

void TPlayScreen::SetFullScreen(bool on)
{
    fullscreen = on;
    interfacedirty = true;
}

void TPlayScreen::SetDemoMode(bool on)
{
    demomode = on;
}

void TPlayScreen::SetControlOn(bool on)
{
    controlon = on;
}

void TPlayScreen::HideLowerPanes()
{
    // TODO(port): when inventory / health / stamina / textbar panes are
    // re-attached, drive their .Hide() here. Stubbed during scaffold.
}

void TPlayScreen::ShowLowerPanes()
{
    // TODO(port): symmetric reveal.
}

// *************************************************************************
// * Save / load                                                           *
// *************************************************************************

void TPlayScreen::LoadGame(int32_t game)
{
    loadgame = true;
    gamenum  = game;
    loadgamepath[0] = '\0';
}

void TPlayScreen::LoadGameFile(const char* path)
{
    if (!path) return;
    loadgame = true;
    strncpyz(loadgamepath, path, MAXPATHLEN);
}

void TPlayScreen::SaveGame(int32_t game)
{
    savegame = true;
    gamenum  = game;
}

void TPlayScreen::SaveMap()
{
    savemap = true;
}

// *************************************************************************
// * Game time                                                             *
// *************************************************************************

int32_t TPlayScreen::GameFrame() const
{
    return gameframes;
}

void TPlayScreen::SetGameTime(int32_t t)
{
    gametime = t;
    sessionstart = gameframes;
    lastsessionframes = t;
}

int32_t TPlayScreen::TimeOfDay() const
{
    return timeofday;
}

int32_t TPlayScreen::Daylight() const
{
    // Smooth 0..255 ambient curve as a function of in-game minutes.
    // Mirrors the retail dawn/dusk fade without bothering with the
    // legacy table-of-deltas. Tweakable in lighting/script systems.
    const int32_t day_minutes = 24 * 60;
    int32_t t = timeofday % day_minutes;
    if (t < 0) t += day_minutes;
    // Centered cosine on noon: 0 at midnight, 255 at noon.
    const float frac  = float(t) / float(day_minutes);
    const float angle = (frac - 0.5f) * 2.0f * 3.14159265f;
    const float cosv  = std::cos(angle);
    const float v     = (cosv * 0.5f + 0.5f) * 255.0f;
    if (v < 0.0f)   return 0;
    if (v > 255.0f) return 255;
    return int32_t(v);
}

int32_t TPlayScreen::DayTimeFlag() const
{
    const int32_t mins = timeofday % (24 * 60);
    const int32_t h = (mins < 0 ? mins + 24 * 60 : mins) / 60;
    if (h < 5)  return DAY_MIDNIGHT;
    if (h < 8)  return DAY_MORNING;
    if (h < 12) return DAY_DAYTIME;
    if (h < 14) return DAY_NOON;
    if (h < 19) return DAY_DAYTIME;
    if (h < 21) return DAY_EVENING;
    return DAY_NIGHT;
}

// *************************************************************************
// * Post-character overlays (stub)                                        *
// *************************************************************************

void TPlayScreen::AddPostCharAnim(int32_t /*x*/, int32_t /*y*/, int32_t /*z*/,
                                  PTBitmap /*bm*/, uint32_t /*drawmode*/, int32_t /*dim*/)
{
    // TODO(port): forward to TMapRenderer overlay queue once the spell-
    // / damage-overlay path is added there. Currently dropped on the
    // floor.
}

void TPlayScreen::AddPostCharText(char* /*text*/, int32_t /*x*/, int32_t /*y*/,
                                   PSColor /*color*/, int32_t /*wrapwidth*/)
{
    // TODO(port): same as AddPostCharAnim, for floating dialog text.
}

// *************************************************************************
// * Legacy hooks kept as no-ops                                            *
// *************************************************************************

void TPlayScreen::CreateBackgroundAreas() { /* legacy backdrop dead */ }
void TPlayScreen::DrawOverhangs(bool /*temporary*/) { /* ditto */ }

// *************************************************************************
// * TScreen pulse-driver hook                                              *
// *************************************************************************

TScreen* TPlayScreen::ShowScreen(TScreen* screen, int32_t /*ticks*/)
{
    // The base TScreen's ShowScreen handled fade transitions and timed
    // hand-offs to LogoScreen. We don't have those wired yet -- just
    // return the same screen so the main loop keeps driving it.
    return screen;
}
