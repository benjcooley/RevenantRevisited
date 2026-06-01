// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionsscreen.cpp - TOptionsScreen (DEF options screen + keymapper)   *
// *************************************************************************

#include "optionsscreen.h"

#include "ctrlmap.h"      // TControlMap / SControlEntry / KEYSPERCODE
#include "logging.h"
#include "playscreen.h"   // InitDefaultControlMap()
#include "revenant.h"     // extern TControlMap ControlMap

#include <cstdio>
#include <cstring>

namespace {

// TEXT_LEFT | TEXT_SHADOW for the controller-row columns.
constexpr uint32_t kRowFlags = 0x0001 | 0x0400;

// Render one SControlKey ("CTRL-A", "F1", or "-" when unbound) into buf.
const char* KeyToString(const SControlKey& k, char* buf, int32_t buflen)
{
    int32_t n = 0;
    while (n < KEYSPERCODE && k.keys[n] != 0) ++n;
    if (n == 0) { std::snprintf(buf, buflen, "-"); return buf; }
    return ControlMap.MakeKeyString(n, const_cast<int32_t*>(k.keys), buf, buflen);
}

}  // namespace

bool TOptionsScreen::Open(int32_t x, int32_t y, int32_t w, int32_t h,
                          const char* bgDat)
{
    if (!def.Open("options", "default", x, y, w, h, bgDat))
        return false;

    // The controller list binds to the global key-binding table. Make sure it
    // is populated even when no game session exists (main menu / standalone
    // test) — TPlayScreen would otherwise be the only initializer.
    InitDefaultControlMap();

    // The name0..name7 "Up/Down/..." TEXT widgets are a dead earlier-design
    // overlay; the live controller list replaces them (retail does not draw
    // them — see the reference screenshot / OptionsDef_SPEC §4.6).
    for (int32_t i = 0; i < 8; ++i)
    {
        char n[8];
        std::snprintf(n, sizeof(n), "name%d", i);
        if (SDefWidget* lbl = def.Find(n))
            lbl->text.clear();
    }

    RefreshControllerList();

    // TODO(options): bind the toggles (Realtime/Auto/Dialog/Face/Enhanced/Limit)
    // and sliders (Violence/Music/Sound/Gamma) to the live setting globals and
    // apply them on OK (OptionsDef_SPEC §6.1/§6.3). Roughed in with sensible
    // initial state for now so the panel reads; the keymapper path below is the
    // real one.
    auto check  = [&](const char* nm, bool on) { if (SDefWidget* g = def.Find(nm)) g->selected = on; };
    auto slider = [&](const char* nm, int32_t v, int32_t mx) {
        if (SDefWidget* g = def.Find(nm)) { g->value = v; g->maxval = mx; }
    };
    check("Realtime", true);  check("Enhanced", true);
    slider("Violence", 4, 5); slider("Music", 96, 127);
    slider("Sound", 110, 127); slider("Gamma", 3, 8);

    return true;
}

void TOptionsScreen::Close() { def.Close(); }
void TOptionsScreen::Render() { def.Render(); }

void TOptionsScreen::RefreshControllerList()
{
    SDefWidget* ctrl = def.Find("controller");
    if (!ctrl) return;

    // Three columns aligned to the headers (Control Name @70, Key 1 @300,
    // Key 2 @455); Key 2 in the field-yellow. x is relative to the row content
    // origin (listbox left + RECT inset).
    const int32_t cx = ctrl->x + ctrl->style.rect.l;
    auto col = [&](int32_t paneX, int32_t cw, bool yellow) {
        SDefListField f;
        f.x = paneX - cx; f.y = 0; f.w = cw; f.h = ctrl->style.itemh;
        f.flags = kRowFlags;
        f.color = yellow ? SDefColor{255, 255, 100, true} : SDefColor{255, 255, 255, true};
        return f;
    };
    ctrl->rowfields = { col(70, 220, false), col(300, 140, false), col(455, 120, true) };

    std::vector<std::vector<std::string>> rows;
    rows.reserve(static_cast<size_t>(ControlMap.NumControls()));
    char k1[64], k2[64];
    for (int32_t i = 0; i < ControlMap.NumControls(); ++i)
    {
        SControlEntry ce;
        ControlMap.GetControlEntry(i, &ce);
        rows.push_back({ ce.name ? ce.name : "",
                         KeyToString(ce.codes[0], k1, sizeof(k1)),
                         KeyToString(ce.codes[1], k2, sizeof(k2)) });
    }
    ctrl->rows = std::move(rows);
}

void TOptionsScreen::OnMouseDown(int32_t lx, int32_t ly) { def.OnMouseDown(lx, ly); }
void TOptionsScreen::OnMouseMove(int32_t lx, int32_t ly) { def.OnMouseMove(lx, ly); }

const char* TOptionsScreen::OnMouseUp(int32_t lx, int32_t ly)
{
    const char* hit = def.OnMouseUp(lx, ly);
    if (hit && std::strcmp(hit, "ok") == 0)
        ControlMap.Save(const_cast<char*>("Controls"));   // persist rebinds to INI
    return hit;
}

void TOptionsScreen::OnKey(int32_t vk, bool down)
{
    if (!down || vk <= 0) return;

    // Rebind: when a controller row is selected, the next keypress becomes its
    // primary binding (Key 1). Single-key for now; chord capture (CTRL-/SHIFT-)
    // is a refinement. Writes through the real ControlMap + redraws the row.
    SDefWidget* ctrl = def.Find("controller");
    if (ctrl && ctrl->selrow >= 0 && ctrl->selrow < ControlMap.NumControls())
    {
        SControlEntry ce;
        ControlMap.GetControlEntry(ctrl->selrow, &ce);
        ce.codes[0].keys[0] = vk;
        ce.codes[0].keys[1] = 0;
        ce.codes[0].keys[2] = 0;
        ControlMap.SetControlEntry(ctrl->selrow, &ce);
        RefreshControllerList();
        log_info("[options] rebound '%s' -> vk 0x%x", ce.name ? ce.name : "?", vk);
    }
}
