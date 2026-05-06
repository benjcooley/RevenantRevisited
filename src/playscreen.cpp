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
// the postanim/posttext arrays, and the Pulse/Animate -> Display->Put/ZPut/
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

#include "display.h"
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
// attic/src/playscreen.cpp; the trimmed first cut keeps the directional
// + run / sneak entries that drive Locke's movement. Combat / bow /
// inventory / spell-slot bindings come back as the receiving systems
// get wired. Single-key chords for now -- no Ctrl/Shift combos.
//
// `mode` is currently set to ALLMODES (0xFFFFFFFF) since combat/bow/
// sneak modes aren't gating anything yet; refine when those gameplay
// modes come back online.
static SControlEntry g_defaultGameControls[] =
{
    {"Run",         "Run",       ALLMODES, {{'R'}},                        GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_RUN,        false},
    {"Sneak",       "Sneak",     ALLMODES, {{'S'}},                        GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_SNEAK,      false},
    {"Left",        "Left",      ALLMODES, {{VK_LEFT},  {VK_JOYLEFT}},     GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_LEFT,       false},
    {"Right",       "Right",     ALLMODES, {{VK_RIGHT}, {VK_JOYRIGHT}},    GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_RIGHT,      false},
    {"Up",          "Up",        ALLMODES, {{VK_UP},    {VK_JOYUP}},       GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_UP,         false},
    {"Down",        "Down",      ALLMODES, {{VK_DOWN},  {VK_JOYDOWN}},     GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_DOWN,       false},
    {"Up Left",     "UpLeft",    ALLMODES, {{VK_HOME},  {VK_JOYUPLEFT}},   GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_UPLEFT,     false},
    {"Up Right",    "UpRight",   ALLMODES, {{VK_PRIOR}, {VK_JOYUPRIGHT}},  GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_UPRIGHT,    false},
    {"Down Left",   "DownLeft",  ALLMODES, {{VK_END},   {VK_JOYDOWNLEFT}}, GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_DOWNLEFT,   false},
    {"Down Right",  "DownRight", ALLMODES, {{VK_NEXT},  {VK_JOYDOWNRIGHT}},GAMECMD_DIRDOWN,  GAMECMD_DIRUP,  CMDFLAG_DOWNRIGHT,  false},
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

    log_info("[playscreen] initialize done");
    return true;
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
    CurrentScreen = saved_screen;

    log_info("[player] spawned Locke in sector %d_%d_%d at world (%d,%d,%d)",
             level, sx, sy, def.pos.x, def.pos.y, def.pos.z);
    return true;
}

void TPlayScreen::Close()
{
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

    // Advance per-frame counters. The map renderer ticks its own animator
    // off LegacyFrameCount(), so we just track our own session bookkeeping
    // here.
    ++gameframes;
    gametime = lastsessionframes
             + (gameframes - sessionstart) * 100 / kGameFrameRate;
    timeofday = TimeOfDayMinutes(gametime);
}

void TPlayScreen::RenderFrame()
{
    if (!mapRenderer) return;
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

    ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.55f);
    if (ImGui::Begin("Player Debug", nullptr,
                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("pos     : (%d, %d, %d)  level=%d", p.x, p.y, p.z, lvl);
        ImGui::Text("sector  : %d_%d", sx_world, sy_world);
        ImGui::Text("walkmap : %d  (z-walk = %d)", walk, z_delta);
        ImGui::Text("radius  : %d   r_h=%d  min=%d  max=%d",
                    radius, r_h, r_min, r_max);
        const bool z_blocks    = std::abs(z_delta) > 32;
        const bool no_floor    = (walk == 0);
        const bool slope_min   = std::abs(r_min) > 32;
        const bool slope_max   = std::abs(r_max) > 32;
        const bool char_blocks = (blocker != nullptr);
        ImGui::Text("would block: %s%s%s%s%s%s",
                    z_blocks    ? "Z>32 "       : "",
                    no_floor    ? "no-floor "   : "",
                    slope_min   ? "min>32 "     : "",
                    slope_max   ? "max>32 "     : "",
                    char_blocks ? "char-blk "   : "",
                    (!z_blocks && !no_floor && !slope_min && !slope_max && !char_blocks)
                                ? "no" : "");
        ImGui::Text("moving  : %s   moveangle=%d   anim=%s",
                    Player->IsMoving() ? "yes" : "no",
                    Player->GetMoveAngle(),
                    state_name ? state_name : "?");
        // Animation state breakout. state index by itself is opaque;
        // pair it with frame/total + decoded ani-flags so the cycle bug
        // ("walkf done but never advances") is readable at a glance.
        // TComplexObject overrides GetState() to return const char*
        // (the action-block name). Reach the integer state index via
        // the TObjectInstance base.
        const int32_t st_idx   = static_cast<TObjectInstance*>(Player)->GetState();
        const int32_t st_total = Player->NumStates();
        TObjectImagery* img    = Player->GetImagery();
        const int32_t fr_total = img ? img->GetAniLength(st_idx) : 0;
        const uint32_t ani_fl  = Player->GetAniFlags();
        char flagstr[64] = {};
        int  fpos = 0;
        auto add = [&](const char* tag, uint32_t mask) {
            if ((ani_fl & mask) && fpos < int(sizeof(flagstr)) - 12) {
                fpos += std::snprintf(flagstr + fpos, sizeof(flagstr) - fpos,
                                      fpos ? "|%s" : "%s", tag);
            }
        };
        add("LOOP", AF_LOOPING);
        add("PP",   AF_PINGPONG);
        add("ROOT", AF_ROOT);
        add("R2R",  AF_ROOT2ROOT);
        add("FLY",  AF_FLY);
        add("MOVE", AF_MOVE);
        ImGui::Text("anim    : %s  state=%d/%d  frame=%d/%d  flags=%s  done=%s",
                    state_name ? state_name : "?",
                    st_idx, st_total, Player->GetFrame(), fr_total,
                    fpos ? flagstr : "-",
                    Player->CommandDone() ? "yes" : "no");
        ImGui::Text("framerate=%d  HasAnimator=%s",
                    Player->GetFrameRate(),
                    Player->HasAnimator() ? "yes" : "NO");
    }
    ImGui::End();
}

// Legacy entry points still referenced by drivers / pane code. Pulse
// pumps the per-frame state update; Animate fires the world render
// (matching what TTestScreen does for TestModes::Render). DrawBackground
// is dead -- no BITMAP.100 backdrop on the new path.
void TPlayScreen::Pulse()                  { Update(); }
void TPlayScreen::Animate(bool /*draw*/)   { RenderFrame(); EditorDrawChrome(); DrawPlayerStatusOverlay(); }
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
    // renderer's gizmo / drag picker, game mode keeps the click for
    // future walk-to / interact wiring.
    CurrentMode()->HandleMouseClick(button, x, y);
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
