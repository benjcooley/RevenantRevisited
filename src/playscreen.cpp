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
#include "logging.h"
#include "maprenderer.h"
#include "savegame.h"
#include "time.h"

// Cursor bitmaps loaded from gamedata at boot. Other modules
// (automap, editor console, mappane) reference these by symbol; keep
// them as globals so the existing call sites compile unchanged.
PTBitmap PointerCursor = nullptr;
PTBitmap HandCursor    = nullptr;

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
    // back to its defaults (Misthaven-area).
    mapRenderer = std::make_unique<TMapRenderer>();
    if (!mapRenderer->InitializeFromStartupArgs())
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

    EditorLoadState();

    log_info("[playscreen] initialize done");
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

    // Drive player movement from the latest command-flag state. The
    // actual flag bits are toggled in KeyPress / Joystick / Command via
    // TPlayer; UpdateMove translates them into a movement step.
    if (controlon && !demomode)
        UpdateMove();

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
    if (mapRenderer)
        mapRenderer->RenderFrame();
}

// Legacy entry points still referenced by drivers / pane code. Pulse
// pumps the per-frame state update; Animate fires the world render
// (matching what TTestScreen does for TestModes::Render). DrawBackground
// is dead -- no BITMAP.100 backdrop on the new path.
void TPlayScreen::Pulse()                  { Update(); }
void TPlayScreen::Animate(bool /*draw*/)   { RenderFrame(); EditorDrawChrome(); }
void TPlayScreen::DrawBackground()         { /* no backdrop blit on the new path */ }

// *************************************************************************
// * Input                                                                 *
// *************************************************************************

void TPlayScreen::KeyPress(int32_t key, bool down)
{
    // Editor toggle. F12 is the retail-era hotkey.
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

    TScreen::KeyPress(key, down);
}

void TPlayScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TScreen::MouseClick(button, x, y);
    if (mapRenderer)
        mapRenderer->HandleMouseClick(button, x, y);
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
    // TODO(port): apply CMDFLAG_DIRFLAGS direction state to TPlayer's
    // movement. Wired up once TPlayer's movement path is live on the new
    // engine.
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
