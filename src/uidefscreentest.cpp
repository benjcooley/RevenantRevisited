// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidefscreentest.cpp - DEF widget engine test harness                  *
// *************************************************************************
//
// Drives the in-scope DEF screens in isolation:
//   --test=ui-ingamemenu   ingamemenu.def  "ingamemenu"  (ESC pause menu)
//   --test=ui-options      options.def     "default"     (Settings)  -> the real
//                          TOptionsScreen (controller list bound to ControlMap +
//                          key rebinding)
//   --test=ui-savegame     savegame.def    "default"     (Save)
//   --test=ui-loadgame     loadgame.def    "default"     (Load)
//
// Each screen's pane rect + chrome dat are the host-side DefScreen_Open
// constants from the respective *_SPEC.md (the .def carries pane-LOCAL coords).
// Simple screens run on a raw TDefScreen with synthetic FIELD data; Options runs
// on TOptionsScreen which sources its controller list from the live keymapper.

#include "uidefscreentest.h"

#include "defscreen.h"
#include "display.h"
#include "logging.h"
#include "optionsscreen.h"
#include "renderer.h"
#include "revdefs.h"   // MB_LEFTDOWN / MB_LEFTUP
#include "surface.h"

#include <cstring>

namespace {

// Per-screen host open parameters (DefScreen_Open args from each *_SPEC.md).
struct SScreenSpec
{
    const char* mode;
    const char* def;        // <def>.def
    const char* panel;      // PANEL name
    int32_t     x, y, w, h; // pane rect on the 640x480 stage
    const char* bgdat;      // chrome archive
};

constexpr SScreenSpec kScreens[] = {
    // InGameMenuDef_SPEC §3: pane TL (126,65), 394x316.
    {"ui-ingamemenu", "ingamemenu", "ingamemenu", 126, 65, 394, 316, "ingamemenunotex.dat"},
    // OptionsDef_SPEC §3: full-display pane (0,0) 640x480. The "alpha" variant is
    // the full pre-composited Ahkuilon backdrop + frame + OPTIONS plate (retail);
    // "notex" is frame-only (software fallback), "tex" is the ARGB4444 variant.
    {"ui-options",    "options",    "default",    0,   0,  640, 480, "optionsalpha.dat"},
    // Save/Load full-display chrome. Load is reached from the main menu and ships
    // an "alpha" scene backdrop (battle vista); Save is in-game only and has a
    // transparent interior meant to overlay the frozen game frame.
    {"ui-savegame",   "savegame",   "default",    0,   0,  640, 480, "savegamenotex.dat"},
    {"ui-loadgame",   "loadgame",   "default",    0,   0,  640, 480, "loadgamealpha.dat"},
};

const SScreenSpec* FindSpec(const char* mode)
{
    if (!mode) return nullptr;
    for (const SScreenSpec& s : kScreens)
        if (std::strcmp(mode, s.mode) == 0)
            return &s;
    return nullptr;
}

bool IsOptions(const SScreenSpec* s) { return s && std::strcmp(s->mode, "ui-options") == 0; }

TDefScreen*        g_screen  = nullptr;   // simple screens
TOptionsScreen*    g_options = nullptr;   // ui-options
const SScreenSpec* g_spec    = nullptr;

// Feed synthetic data into the FIELD-bound widgets of the simple screens so they
// read in isolation (no live game state behind the test). Options sources its
// real data from ControlMap inside TOptionsScreen, so it is not handled here.
void PopulateSynthetic(const SScreenSpec& spec)
{
    if (!g_screen) return;
    if (std::strcmp(spec.mode, "ui-savegame") == 0 ||
        std::strcmp(spec.mode, "ui-loadgame") == 0)
    {
        std::vector<std::vector<std::string>> rows;
        const char* names[] = {
            "Ahkuilon Entrance", "Misty Caves", "The Lost Vault",
            "Tomb of Set-amun", "Sea of Serpents", "Final Descent",
        };
        for (const char* n : names)
            rows.push_back({n});
        g_screen->SetListRows("gamelist", std::move(rows));
        if (SDefWidget* gl = g_screen->Find("gamelist")) gl->selrow = 1;  // "Misty Caves"

        if (SDefWidget* w = g_screen->Find("gamename")) w->text = "Misty Caves";
        if (SDefWidget* w = g_screen->Find("modname"))  w->text = "Ahkuilon";
        if (SDefWidget* w = g_screen->Find("charname")) w->text = "Locke";
    }
}

// HUD drawable: blit the already-composed RT at the pane origin. The RT is
// composed in RenderUIDefScreenMode (outside the swapchain pass) — composing
// here would open a nested render pass (sokol asserts).
class TDefScreenHud : public THudDrawable
{
  public:
    void Draw() override
    {
        if (!g_spec) return;
        TSurface* s = g_options ? g_options->Surface()
                    : g_screen  ? g_screen->Surface() : nullptr;
        if (s) Renderer->DrawSurface(s, g_spec->x, g_spec->y);
    }
};

TDefScreenHud g_hud;

}  // namespace

bool IsUIDefScreenMode(const char* mode)
{
    return FindSpec(mode) != nullptr;
}

bool InitializeUIDefScreenMode(const char* mode)
{
    g_spec = FindSpec(mode);
    if (!g_spec)
        return false;

    log_info("[ui-defscreen] === DEF screen '%s' (%s.def / panel '%s') ===",
             g_spec->mode, g_spec->def, g_spec->panel);

    bool ok = false;
    if (IsOptions(g_spec))
    {
        delete g_options;
        g_options = new TOptionsScreen();
        ok = g_options->Open(g_spec->x, g_spec->y, g_spec->w, g_spec->h, g_spec->bgdat);
        if (!ok) { delete g_options; g_options = nullptr; }
    }
    else
    {
        delete g_screen;
        g_screen = new TDefScreen();
        ok = g_screen->Open(g_spec->def, g_spec->panel,
                            g_spec->x, g_spec->y, g_spec->w, g_spec->h, g_spec->bgdat);
        if (!ok) { delete g_screen; g_screen = nullptr; }
        else PopulateSynthetic(*g_spec);
    }

    if (!ok)
    {
        log_error("[ui-defscreen] Open failed for %s", g_spec->mode);
        g_spec = nullptr;
        return false;
    }

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIDefScreenMode()
{
    // Compose the screen into its RT here — outside the swapchain pass — so the
    // HUD Draw() only blits the finished surface (no nested pass).
    if (g_options) g_options->Render();
    else if (g_screen) g_screen->Render();

    // Dim backdrop so the modal chrome's transparent edges read in isolation.
    Display.BackBuffer()->StartPass(0.10f, 0.10f, 0.12f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIDefScreenMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_screen;  g_screen  = nullptr;
    delete g_options; g_options = nullptr;
    g_spec = nullptr;
}

void HandleMouseClickUIDefScreenMode(int32_t button, int32_t x, int32_t y)
{
    if (!g_spec) return;
    const int32_t lx = x - g_spec->x;
    const int32_t ly = y - g_spec->y;
    if (button == MB_LEFTDOWN)
    {
        if (g_options) g_options->OnMouseDown(lx, ly);
        else if (g_screen) g_screen->OnMouseDown(lx, ly);
    }
    else if (button == MB_LEFTUP)
    {
        const char* hit = g_options ? g_options->OnMouseUp(lx, ly)
                        : g_screen  ? g_screen->OnMouseUp(lx, ly) : nullptr;
        if (hit) log_info("[ui-defscreen] clicked '%s'", hit);
    }
}

void HandleMouseMoveUIDefScreenMode(int32_t button, int32_t x, int32_t y)
{
    (void)button;
    if (!g_spec) return;
    const int32_t lx = x - g_spec->x, ly = y - g_spec->y;
    if (g_options) g_options->OnMouseMove(lx, ly);
    else if (g_screen) g_screen->OnMouseMove(lx, ly);
}

void HandleKeyPressUIDefScreenMode(int32_t key, bool down)
{
    if (g_options) g_options->OnKey(key, down);
    else if (g_screen) g_screen->OnKey(key, down);
}
