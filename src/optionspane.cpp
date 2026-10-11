// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionspane.cpp - TOptionsPane (DEF options pane + keymapper)        *
// *************************************************************************

#include "optionspane.h"

#include "ctrlmap.h"      // TControlMap / SControlEntry / KEYSPERCODE
#include "gameoptions.h"  // the settings the pane edits, SaveOptions
#include "logging.h"
#include "mappane.h"      // MapPane: OK re-sets the ambient with the gamma offset
#include "playscreen.h"   // InitDefaultControlMap()
#include "revenant.h"     // ControlMap, AutoBeginCombat, PlaySpeech, ViolenceLevel, EnhancedLighting
#include "sound.h"        // ApplyMusicVolume / ApplyEffectsVolume, level ranges

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace {

// TEXT_LEFT | TEXT_SHADOW for the controller-row columns.
constexpr uint32_t kRowFlags = 0x0001 | 0x0400;

constexpr int32_t kKeysPerControl = CODESPERCOMMAND * KEYSPERCODE;

// The toggles and the copies they edit (retail events 0xbb9 / 0xbba, by name).
struct SToggleBinding
{
    const char* name;
    bool SOptionsPaneValues::* field;
};
constexpr SToggleBinding kToggles[] = {
    { "Realtime",    &SOptionsPaneValues::realTimeLight },
    { "Auto",        &SOptionsPaneValues::autoCombat },
    { "Dialog",      &SOptionsPaneValues::playSpeech },
    { "Face",        &SOptionsPaneValues::combatFace },
    { "Enhanced",    &SOptionsPaneValues::enhancedLighting },
    { "Limit",       &SOptionsPaneValues::limitSpeed },
    { "NoCombatRes", &SOptionsPaneValues::noCombatResults },
};

// The sliders, their copies, ranges and pages (0x0053aa90 event 1,
// 0x0053ad4d-0x0053ae00: a track click moves Music by 12, Sound by 15).
struct SSliderBinding
{
    const char* name;
    int32_t SOptionsPaneValues::* field;
    int32_t maxval;
    int32_t page;
};
constexpr SSliderBinding kSliders[] = {
    { "Violence", &SOptionsPaneValues::violence, 4,                1 },
    { "Music",    &SOptionsPaneValues::music,    kMusicLevelMax,   0xc },
    { "Sound",    &SOptionsPaneValues::effects,  kEffectsLevelMax, 0xf },
    { "Gamma",    &SOptionsPaneValues::gamma,    4,                1 },
};

// Render one code's keys ("CTRL-A", "F1", or "-" when unbound) into buf.
const char* KeyToString(int32_t* keys, char* buf, int32_t buflen)
{
    int32_t n = 0;
    while (n < KEYSPERCODE && keys[n] != 0) ++n;
    if (n == 0) { std::snprintf(buf, buflen, "-"); return buf; }
    return ControlMap.MakeKeyString(n, keys, buf, buflen);
}

}  // namespace

bool TOptionsPane::OpenOptions(bool fromGame, int32_t x, int32_t y)
{
    // REVSYNC: 0x0053a8b0 -- the copies the controls edit ("Limit" is the
    // inverse of the no-limit flag).
    values.realTimeLight    = RealTimeLight;
    values.autoCombat       = AutoBeginCombat;
    values.playSpeech       = PlaySpeech;
    values.combatFace       = CombatFace;
    values.enhancedLighting = EnhancedLighting;
    values.limitSpeed       = !NoGameSpeedLimit;
    values.noCombatResults  = NoCombatResults;
    values.violence         = ViolenceLevel;
    values.music            = MusicVolume;
    values.effects          = EffectsVolume;
    values.gamma            = GammaLevel;

    // The controller list binds to the global key-binding table. Make sure it
    // is populated even when no game session exists (main menu / standalone
    // test) — TPlayScreen would otherwise be the only initializer. Then copy
    // every control's keys into the rebind buffer (0x00439060; the code
    // flags are not copied).
    InitDefaultControlMap();
    const int32_t controls = ControlMap.NumControls();
    bindings.assign(static_cast<size_t>(controls) * kKeysPerControl, 0);
    for (int32_t i = 0; i < controls; ++i)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(i, &ce);
        for (int32_t c = 0; c < CODESPERCOMMAND; ++c)
            std::memcpy(BindingKeys(i, c), ce.codes[c].keys, sizeof(ce.codes[c].keys));
    }

    return Open("options", "default", fromGame ? DEF_INGAME : 0, x, y, WIDTH, HEIGHT, "options");
}

int32_t* TOptionsPane::BindingKeys(int32_t control, int32_t code)
{
    return &bindings[static_cast<size_t>((control * CODESPERCOMMAND + code) * KEYSPERCODE)];
}

// REVSYNC: 0x0053aa90 event 1. Setting a slider's value raises its change
// event when it differs from the fresh slider's 0, as retail's SetValue
// does: a Violence of 5 clamps to the slider's 4 and the copy follows, and
// the music level applies.
void TOptionsPane::OnOpened()
{
    // The name0..name7 "Up/Down/..." TEXT widgets are a dead earlier-design
    // overlay; the live controller list replaces them (retail does not draw
    // them — see the reference screenshot / OptionsDef_SPEC §4.6).
    for (int32_t i = 0; i < 8; ++i)
    {
        char n[8];
        std::snprintf(n, sizeof(n), "name%d", i);
        if (SDefWidget* lbl = Find(n))
            lbl->text.clear();
    }

    // Retail disables "Enhanced" on a device without MODULATE2X/4X; every
    // device the port runs on has them.
    for (const SToggleBinding& t : kToggles)
        if (SDefWidget* w = Find(t.name))
            w->selected = values.*t.field;
    for (const SSliderBinding& s : kSliders)
    {
        SetSliderRange(s.name, 0, s.maxval);
        SetSliderPage(s.name, s.page);
        SetSliderValue(s.name, values.*s.field);
    }

    RefreshControllerList();
}

void TOptionsPane::OnSliderChanged(const SDefWidget& slider)
{
    for (const SSliderBinding& s : kSliders)
    {
        if (slider.name != s.name)
            continue;
        values.*s.field = slider.value;
        // The music changes while the slider moves (0x0049a5c0). Retail's
        // gamma slider set the display ramp live too (0x004a98f0); the port
        // has no gamma ramp (OPTIONS.md §9).
        if (s.field == &SOptionsPaneValues::music)
            ApplyMusicVolume(slider.value);
        return;
    }
}

void TOptionsPane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    for (const SToggleBinding& t : kToggles)
    {
        if (widget.name == t.name)
        {
            values.*t.field = widget.selected;
            return;
        }
    }

    // "cancel" restores nothing in the port: retail re-applied the gamma
    // ramp, which the port doesn't have; the copies and the rebind buffer
    // are dropped. A music level dragged to stays until the next open, OK
    // or restart, as in retail (OPTIONS.md §6.1).
    if (widget.name == "ok")
        Apply();
    TDefPane::OnActivate(widget, buttonIndex);
}

// REVSYNC: 0x0053aa90 event 3000 "ok" -- retail's order. Retail sets the
// display gamma ramp (0x004a98f0) here; the port has none (OPTIONS.md §9).
// It then re-sets the map ambient through SetAmbientLight (0x00453640),
// which adds the gamma offset to an ambient that already carries it, so
// each OK brightens the map until the area ambient is next set (question 93,
// kept as retail).
void TOptionsPane::Apply()
{
    RealTimeLight    = values.realTimeLight;
    PlaySpeech       = values.playSpeech;
    AutoBeginCombat  = values.autoCombat;
    EnhancedLighting = values.enhancedLighting;
    NoCombatResults  = values.noCombatResults;
    ViolenceLevel    = values.violence;
    MusicVolume      = values.music;
    NoGameSpeedLimit = !values.limitSpeed;
    ApplyMusicVolume(MusicVolume);
    EffectsVolume    = values.effects;
    GammaLevel       = values.gamma;
    MapPane.SetAmbientLight(MapPane.GetAmbientLight());
    ApplyEffectsVolume(EffectsVolume);
    CombatFace       = values.combatFace;
    SaveOptions();

    // The rebind buffer back into the control map, keys only (0x00439060 /
    // 0x00439110), and [Controls] saved (0x00439dc0).
    const int32_t controls = (std::min)(ControlMap.NumControls(),
                                        static_cast<int32_t>(bindings.size() / kKeysPerControl));
    for (int32_t i = 0; i < controls; ++i)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(i, &ce);
        for (int32_t c = 0; c < CODESPERCOMMAND; ++c)
            std::memcpy(ce.codes[c].keys, BindingKeys(i, c), sizeof(ce.codes[c].keys));
        ControlMap.SetControlEntry(i, &ce);
    }
    ControlMap.Save(const_cast<char*>("Controls"));

    log_info("[options] applied: realtime=%d auto=%d speech=%d face=%d enhanced=%d limit=%d "
             "nocombatres=%d violence=%d music=%d effects=%d gamma=%d",
             RealTimeLight, AutoBeginCombat, PlaySpeech, CombatFace, EnhancedLighting,
             !NoGameSpeedLimit, NoCombatResults, ViolenceLevel, MusicVolume, EffectsVolume,
             GammaLevel);
}

void TOptionsPane::RefreshControllerList()
{
    SDefWidget* ctrl = Find("controller");
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

    const int32_t controls = (std::min)(ControlMap.NumControls(),
                                        static_cast<int32_t>(bindings.size() / kKeysPerControl));
    std::vector<std::vector<std::string>> rows;
    rows.reserve(static_cast<size_t>(controls));
    char k1[64], k2[64];
    for (int32_t i = 0; i < controls; ++i)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(i, &ce);
        rows.push_back({ ce.name ? ce.name : "",
                         KeyToString(BindingKeys(i, 0), k1, sizeof(k1)),
                         KeyToString(BindingKeys(i, 1), k2, sizeof(k2)) });
    }
    ctrl->rows = std::move(rows);
}

void TOptionsPane::OnKey(int32_t vk, bool down)
{
    if (!down || vk <= 0) return;

    // Rebind: when a controller row is selected, the next keypress becomes its
    // primary binding (Key 1) in the rebind buffer; "ok" commits it. Single-key
    // for now; chord capture (CTRL-/SHIFT-) is a refinement.
    SDefWidget* ctrl = Find("controller");
    const int32_t controls = static_cast<int32_t>(bindings.size() / kKeysPerControl);
    if (!(ctrl && ctrl->selrow >= 0 && ctrl->selrow < controls))
    {
        TDefPane::OnKey(vk, down);
        return;
    }

    int32_t* keys = BindingKeys(ctrl->selrow, 0);
    keys[0] = vk;
    keys[1] = 0;
    keys[2] = 0;
    RefreshControllerList();
    SetDirty(true);

    SControlEntry ce{};
    ControlMap.GetControlEntry(ctrl->selrow, &ce);
    log_info("[options] rebound '%s' -> vk 0x%x (until ok)", ce.name ? ce.name : "?", vk);
}
