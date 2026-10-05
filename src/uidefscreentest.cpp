// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidefscreentest.cpp - DEF widget engine test harness                  *
// *************************************************************************
//
// Drives the in-scope DEF screens in isolation:
//   --test=ui-ingamemenu   ingamemenu.def  "ingamemenu"  (ESC pause menu)
//   --test=ui-options      options.def     "default"     (Settings)  -> the real
//                          TOptionsPane (controller list bound to ControlMap +
//                          key rebinding)
//   --test=ui-savegame     savegame.def    "default"     (Save)
//   --test=ui-loadgame     loadgame.def    "default"     (Load)
//
// Each screen's pane rect + chrome dat are the host-side DefScreen_Open
// constants from the respective *_SPEC.md (the .def carries pane-LOCAL coords).
// Simple screens run on a raw TDefPane with synthetic FIELD data; Options runs
// on TOptionsPane which sources its controller list from the live keymapper.

#include "uidefscreentest.h"

#include "defpane.h"
#include "display.h"
#include "logging.h"
#include "optionspane.h"
#include "revenant.h"   // CurrentScreen
#include "screen.h"
#include "surface.h"

#include <cstring>

namespace {

struct SScreenSpec
{
    const char* mode;
    const char* def;        // <def>.def
    const char* panel;      // PANEL name
    int32_t     x, y, w, h; // pane rect on the 640x480 design canvas
    const char* bgdat;      // chrome archive
    bool        modal;      // pushed as a modal (retail RunModal) vs added as a pane
};

constexpr SScreenSpec kScreens[] = {
    // InGameMenuDef_SPEC §3: pane TL (126,65), 394x316. Retail runs it modally
    // (TPlayScreen in-game menu 0x0047e500 -> RunModal).
    {"ui-ingamemenu", "ingamemenu", "ingamemenu", 126, 65, 394, 316, "ingamemenunotex.dat", true},
    // OptionsDef_SPEC §3: full-display pane (0,0) 640x480. The "alpha" variant is
    // the full pre-composited Ahkuilon backdrop + frame + OPTIONS plate (retail);
    // "notex" is frame-only (software fallback), "tex" is the ARGB4444 variant.
    {"ui-options",    "options",    "default",    0,   0,  640, 480, "optionsalpha.dat",    false},
    // Save/Load full-display chrome. Load is reached from the main menu and ships
    // an "alpha" scene backdrop (battle vista); Save is in-game only and has a
    // transparent interior meant to overlay the frozen game frame.
    {"ui-savegame",   "savegame",   "default",    0,   0,  640, 480, "savegamenotex.dat",   false},
    {"ui-loadgame",   "loadgame",   "default",    0,   0,  640, 480, "loadgamealpha.dat",   false},
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

TDefPane*          g_pane = nullptr;
const SScreenSpec* g_spec = nullptr;

void PopulateSynthetic(const SScreenSpec& spec, TDefPane& pane)
{
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
        pane.SetListRows("gamelist", std::move(rows));
        if (SDefWidget* gl = pane.Find("gamelist")) gl->selrow = 1;  // "Misty Caves"

        if (SDefWidget* w = pane.Find("gamename")) w->text = "Misty Caves";
        if (SDefWidget* w = pane.Find("modname"))  w->text = "Ahkuilon";
        if (SDefWidget* w = pane.Find("charname")) w->text = "Locke";
    }
}

// Pushes the modal pane and re-pushes it when it ends, so the test screen keeps
// showing it after each choice (the result is logged).
void PushModalPane()
{
    if (!g_pane || !CurrentScreen) return;
    CurrentScreen->PushModal(g_pane, 0, [](int32_t result) {
        log_info("[ui-defscreen] modal result %d", result);
        PushModalPane();
    });
}

}  // namespace

bool IsUIDefScreenMode(const char* mode)
{
    return FindSpec(mode) != nullptr;
}

bool InitializeUIDefScreenMode(const char* mode)
{
    g_spec = FindSpec(mode);
    if (!g_spec || !CurrentScreen)
        return false;

    log_info("[ui-defscreen] === DEF pane '%s' (%s.def / panel '%s') ===",
             g_spec->mode, g_spec->def, g_spec->panel);

    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    const int32_t x = ox + g_spec->x, y = oy + g_spec->y;

    bool ok = false;
    if (IsOptions(g_spec))
    {
        auto* options = new TOptionsPane();
        ok = options->OpenOptions(x, y, g_spec->w, g_spec->h, g_spec->bgdat);
        g_pane = options;
    }
    else
    {
        g_pane = new TDefPane();
        ok = g_pane->Open(g_spec->def, g_spec->panel, x, y, g_spec->w, g_spec->h, g_spec->bgdat);
        if (ok) PopulateSynthetic(*g_spec, *g_pane);
    }
    if (!ok)
    {
        log_error("[ui-defscreen] Open failed for %s", g_spec->mode);
        delete g_pane;
        g_pane = nullptr;
        g_spec = nullptr;
        return false;
    }

    g_pane->SetOnActivate([](TDefPane& pane, const SDefWidget& w, int32_t index) {
        log_info("[ui-defscreen] clicked '%s'", w.name.c_str());
        if (g_spec && g_spec->modal)
            pane.EndModal(index);
    });

    if (g_spec->modal)
        PushModalPane();
    else
        CurrentScreen->AddPane(g_pane);
    return true;
}

void RenderUIDefScreenMode()
{
    // The pane composes and draws itself through the screen's pane layer; the
    // test only clears to a dark backdrop so transparent chrome reads.
    Display.BackBuffer()->StartPass(0.10f, 0.10f, 0.12f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIDefScreenMode()
{
    if (g_pane)
    {
        if (CurrentScreen)
            CurrentScreen->RemovePane(g_pane);
        g_pane->Close();
        delete g_pane;
        g_pane = nullptr;
    }
    g_spec = nullptr;
}
