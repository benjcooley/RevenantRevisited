// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidefscreentest.cpp - DEF widget engine test harness                  *
// *************************************************************************
//
// Exercises TDefScreen (src/defscreen.{h,cpp}) for the in-scope DEF screens:
//   --test=ui-ingamemenu   ingamemenu.def  "ingamemenu"  (ESC pause menu)
//   --test=ui-options      options.def     "default"     (Settings)
//   --test=ui-savegame     savegame.def    "default"     (Save)
//   --test=ui-loadgame     loadgame.def    "default"     (Load)
//
// Each screen's pane rect + chrome dat are the host-side DefScreen_Open
// constants from the respective *_SPEC.md (the .def carries pane-LOCAL coords;
// the open position/size come from the calling code). Synthetic data feeds the
// FIELD-bound widgets (picture, save list) so the layout reads in isolation.

#include "uidefscreentest.h"

#include "defscreen.h"
#include "display.h"
#include "logging.h"
#include "renderer.h"
#include "revdefs.h"   // MB_LEFTDOWN / MB_LEFTUP
#include "surface.h"

#include <cstdio>
#include <cstring>

namespace {

// Per-screen host open parameters (DefScreen_Open args from each *_SPEC.md).
struct SScreenSpec
{
    const char* mode;
    const char* def;        // <def>.def
    const char* panel;      // PANEL name
    int32_t     x, y, w, h; // pane rect on the 640x480 stage
    const char* bgdat;      // <screen>notex.dat chrome archive
};

constexpr SScreenSpec kScreens[] = {
    // InGameMenuDef_SPEC §3: pane TL (126,65), 394x316.
    {"ui-ingamemenu", "ingamemenu", "ingamemenu", 126, 65, 394, 316, "ingamemenunotex.dat"},
    // OptionsDef_SPEC §3: full-display pane at (0,0), 640x480. The "alpha"
    // variant is the full pre-composited backdrop (Ahkuilon scene + engraved
    // frame + OPTIONS plate) matching retail; "notex" is frame-only (software
    // fallback) and "tex" is the ARGB4444 texture variant.
    {"ui-options",    "options",    "default",    0,   0,  640, 480, "optionsalpha.dat"},
    // Save/Load: full-display 640x480 chrome. Load is reached from the main
    // menu and ships an "alpha" scene backdrop (battle vista) like Options;
    // Save is only reached in-game, so its chrome has a transparent interior
    // (no scene variant) meant to overlay the frozen game frame.
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

TDefScreen*        g_screen = nullptr;
const SScreenSpec* g_spec   = nullptr;

// Feed synthetic data into the FIELD-bound widgets so the screen reads in
// isolation (no live game state behind the test).
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

    if (std::strcmp(spec.mode, "ui-options") == 0)
    {
        // Toggle states (checkbox on/off).
        auto check = [&](const char* n, bool on) { if (SDefWidget* w = g_screen->Find(n)) w->selected = on; };
        check("Realtime", true);
        check("Auto",     false);
        check("Dialog",   true);
        check("Face",     false);
        check("Enhanced", true);
        check("Limit",    false);

        // Slider positions (value/range per OptionsDef_SPEC §4.3 inferred ranges).
        auto slider = [&](const char* n, int32_t v, int32_t mx) {
            if (SDefWidget* w = g_screen->Find(n)) { w->value = v; w->maxval = mx; }
        };
        slider("Violence", 4,   5);
        slider("Music",    96,  127);
        slider("Sound",    110, 127);
        slider("Gamma",    3,   8);

        // The retail controller list (reference image) shows the global
        // key-binding table in THREE columns aligned to the headers — Control
        // Name @x70 (white), Key 1 @x300 (white), Key 2 @x455 (yellow) — NOT the
        // name0..name7 "Up/Down/..." labels (a dead earlier-design overlay). So:
        // suppress those labels and drive the list with 3 column fields.
        for (int i = 0; i < 8; ++i)
        {
            char n[8]; std::snprintf(n, sizeof(n), "name%d", i);
            if (SDefWidget* lbl = g_screen->Find(n)) lbl->text.clear();
        }
        if (SDefWidget* ctrl = g_screen->Find("controller"))
        {
            const int32_t cx = ctrl->x + ctrl->style.rect.l;   // row content origin
            auto col = [&](int32_t paneX, int32_t w, bool yellow) {
                SDefListField f;
                f.x = paneX - cx; f.y = 0; f.w = w; f.h = ctrl->style.itemh;
                f.flags = 0x0001 | 0x0400;                     // TEXT_LEFT | TEXT_SHADOW
                f.color = yellow ? SDefColor{255,255,100,true} : SDefColor{255,255,255,true};
                return f;
            };
            ctrl->rowfields = { col(70,220,false), col(300,140,false), col(455,120,true) };
            ctrl->rows = {
                {"Invoke 1",       "F1",     "JOY7-JOY4"},
                {"Invoke 2",       "F2",     "JOY7-JOY5"},
                {"Invoke 3",       "F3",     "JOY7-JOY6"},
                {"Invoke 4",       "F4",     "-"},
                {"Combat Combo 1", "Cont-A", "JOY8-JOY1"},
                {"Combat Combo 2", "Cont-S", "JOY8-JOY2"},
                {"Combat Combo 3", "Cont-D", "JOY8-JOY3"},
                {"Combat Combo 4", "-",      "-"},
            };
        }
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
        if (!g_screen || !g_spec) return;
        if (TSurface* s = g_screen->Surface())
            Renderer->DrawSurface(s, g_spec->x, g_spec->y);
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

    delete g_screen;
    g_screen = new TDefScreen();
    if (!g_screen->Open(g_spec->def, g_spec->panel,
                        g_spec->x, g_spec->y, g_spec->w, g_spec->h,
                        g_spec->bgdat))
    {
        log_error("[ui-defscreen] Open failed for %s", g_spec->mode);
        delete g_screen;
        g_screen = nullptr;
        return false;
    }

    PopulateSynthetic(*g_spec);

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIDefScreenMode()
{
    // Compose the screen into its RT here — outside the swapchain pass — so the
    // HUD Draw() only blits the finished surface (no nested pass).
    if (g_screen)
        g_screen->Render();

    // Dim backdrop so the modal chrome's transparent edges read in isolation.
    Display.BackBuffer()->StartPass(0.10f, 0.10f, 0.12f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIDefScreenMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_screen;
    g_screen = nullptr;
    g_spec   = nullptr;
}

void HandleMouseClickUIDefScreenMode(int32_t button, int32_t x, int32_t y)
{
    if (!g_screen || !g_spec) return;
    const int32_t lx = x - g_spec->x;
    const int32_t ly = y - g_spec->y;
    if (button == MB_LEFTDOWN)
        g_screen->OnMouseDown(lx, ly);
    else if (button == MB_LEFTUP)
        if (const char* hit = g_screen->OnMouseUp(lx, ly))
            log_info("[ui-defscreen] clicked '%s'", hit);
}

void HandleMouseMoveUIDefScreenMode(int32_t button, int32_t x, int32_t y)
{
    (void)button;
    if (!g_screen || !g_spec) return;
    g_screen->OnMouseMove(x - g_spec->x, y - g_spec->y);
}
