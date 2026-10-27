// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionspane.cpp - TOptionsPane (DEF options pane + keymapper)        *
// *************************************************************************

#include "optionspane.h"

#include "ctrlmap.h"      // TControlMap / SControlEntry / KEYSPERCODE
#include "dialog.h"       // DialogList: the keys' display names
#include "gameoptions.h"  // the settings the pane edits, SaveOptions
#include "logging.h"
#include "mappane.h"      // MapPane: OK re-sets the ambient with the gamma offset
#include "playscreen.h"   // InitDefaultControlMap()
#include "revenant.h"     // ControlMap, AutoBeginCombat, PlaySpeech, ViolenceLevel, EnhancedLighting
#include "sound.h"        // ApplyMusicVolume / ApplyEffectsVolume, level ranges

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <string>

namespace {

constexpr int32_t kKeysPerControl = CODESPERCOMMAND * KEYSPERCODE;

// The controller list shows two cells a control, its first two codes (Key 1,
// Key 2); its eight rows are labelled by the TEXT widgets name0..name7.
constexpr int32_t kCellsPerControl = 2;
constexpr int32_t kNameLabels      = 8;

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

// A key slot holding nothing: 0 in the bindings, -1 in the chord being
// captured.
bool NoKey(int32_t key) { return key == 0 || key == -1; }

// REVSYNC: the options pane's field getter @ 0x0053b5d0, "key"
// (0x0053b660-0x0053b79e) -- a code's keys joined with "-" in the order they
// were pressed: each by its display name, the dialog line tagged "KEY" + its
// key name (english.def KEYCTRL = "Cont"), or the key name itself when
// there's no such line; a key without a name shows as "-". No keys: "-".
std::string KeyCellText(const int32_t* keys)
{
    if (NoKey(keys[0]))
        return "-";
    std::string text;
    for (int32_t k = 0; k < KEYSPERCODE; ++k)
    {
        if (NoKey(keys[k]))
            continue;
        std::string key = "-";
        if (const char* name = ControlMap.GetKeyName(keys[k]))
        {
            key = DialogList.DisplayName((std::string("KEY") + name).c_str());
            if (key.rfind("KEY", 0) == 0)
                key = name;
        }
        text += text.empty() ? key : "-" + key;
    }
    return text;
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
    capturing  = false;
    rebindItem = -1;
    chord      = {-1, -1, -1};
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

    // The controller list: two cells a control (0x00430c50(NumControls * 2)),
    // and the names of the controls in its window.
    SetListCount("controller", Controls() * kCellsPerControl);
    RefreshNames();
}

void TOptionsPane::OnSliderChanged(const SDefWidget& slider)
{
    for (const SSliderBinding& s : kSliders)
    {
        if (slider.name != s.name)
            continue;
        values.*s.field = slider.bar.value;
        // The music changes while the slider moves (0x0049a5c0). Retail's
        // gamma slider set the display ramp live too (0x004a98f0); the port
        // has no gamma ramp (OPTIONS.md §9).
        if (s.field == &SOptionsPaneValues::music)
            ApplyMusicVolume(slider.bar.value);
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
// It then re-sets the map ambient through SetAmbientLight (0x00453640).
// REVSYNC-DIVERGENCE: it handed the value it had just read back in, and that
// value already carries the gamma offset, so each OK pressed the offset on
// again and the map crept brighter until the next area ambient (question 93,
// reproduced in the shipped game). ReapplyAmbientGamma takes the old offset
// off first: OK is idempotent and a GammaLevel change still reaches the map,
// which is what retail's re-set was for.
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
    MapPane.ReapplyAmbientGamma();
    ApplyEffectsVolume(EffectsVolume);
    CombatFace       = values.combatFace;
    SaveOptions();

    // The rebind buffer back into the control map, keys only (0x00439060 /
    // 0x00439110), and [Controls] saved (0x00439dc0).
    for (int32_t i = 0; i < Controls(); ++i)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(i, &ce);
        for (int32_t c = 0; c < CODESPERCOMMAND; ++c)
            std::memcpy(ce.codes[c].keys, BindingKeys(i, c), sizeof(ce.codes[c].keys));
        ControlMap.SetControlEntry(i, &ce);
    }
    ControlMap.Save(const_cast<char*>("Controls"));

    log_info("[options] applied: realtime=%d auto=%d speech=%d face=%d enhanced=%d limit=%d "
             "nocombatres=%d violence=%d music=%d effects=%d gamma=%d ambient=%d",
             RealTimeLight, AutoBeginCombat, PlaySpeech, CombatFace, EnhancedLighting,
             !NoGameSpeedLimit, NoCombatResults, ViolenceLevel, MusicVolume, EffectsVolume,
             GammaLevel, MapPane.GetAmbientLight());
}

int32_t TOptionsPane::Controls() const
{
    return (std::min)(ControlMap.NumControls(), static_cast<int32_t>(bindings.size() / kKeysPerControl));
}

// The cell being rebound shows the chord so far while it's captured.
std::string TOptionsPane::FieldText(const std::string& field, int32_t index, SDefColor& colour)
{
    const int32_t control = index / kCellsPerControl;
    if (field != "key" || index < 0 || control >= Controls())
        return TDefPane::FieldText(field, index, colour);
    if (capturing && index == rebindItem)
        return KeyCellText(chord.data());
    return KeyCellText(BindingKeys(control, index % kCellsPerControl));
}

void TOptionsPane::OnListScrolled(const SDefWidget& list)
{
    if (list.name == "controller")
        RefreshNames();
}

// name0..name7 show the names of the controls in the list's rows, from its
// first.
void TOptionsPane::RefreshNames()
{
    const SDefWidget* list = Find("controller");
    const int32_t first = list ? list->top / kCellsPerControl : 0;
    for (int32_t k = 0; k < kNameLabels; ++k)
    {
        char label[8];
        std::snprintf(label, sizeof(label), "name%d", k);
        SControlEntry ce{};
        const int32_t control = first + k;
        if (control < Controls())
            ControlMap.GetControlEntry(control, &ce);
        // Localized through the "Control Names" lines ("Invoke 1" ->
        // INVOKE1), the table's name without one.
        SetText(label, ce.name ? DialogList.DisplayName(ce.name) : std::string());
    }
}

// REVSYNC: MouseClick @ 0x0053ba50 -- every click ends the capture's
// display; the target and the chord so far stay.
void TOptionsPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    capturing = false;
    TDefPane::MouseClick(button, x, y);
}

// REVSYNC: event 5002 @ 0x0053aa90 -- the mouse released on a key cell arms
// the capture for it.
void TOptionsPane::OnListClick(const SDefWidget& list, int32_t item)
{
    if (list.name != "controller")
        return;
    capturing = true;
    rebindItem = list.selitem;
    (void)item;
}

// REVSYNC: KeyPress @ 0x0053b9e0 -- while the controller list has a
// selection every key goes to the rebind (no button keys); otherwise the
// pane's own handling.
void TOptionsPane::OnKey(int32_t vk, bool down)
{
    const SDefWidget* list = Find("controller");
    if (list && list->selitem >= 0)
        Rebind(vk, down);
    else
        TDefPane::OnKey(vk, down);
}

// REVSYNC: Rebind @ 0x0053ba80 -- a chord is up to KEYSPERCODE keys held
// together, in the order pressed. Its first key's release commits it and
// ends the selection; a key past the last replaces it and commits at once,
// the selection staying. A key down again while held changes nothing.
// Modifiers are keys like any other, and every key can be bound.
void TOptionsPane::Rebind(int32_t vk, bool down)
{
    bool commit = false;
    if (!down)
    {
        if (chord[0] == vk)
        {
            capturing = false;
            SelectListItem("controller", -1);
            commit = true;
        }
    }
    else if (rebindItem >= 0)
    {
        if (!capturing)
        {
            // A target without a capture (a click elsewhere ended it): a
            // fresh chord.
            chord = {vk, -1, -1};
            capturing = true;
        }
        else
        {
            int32_t n = 0;
            while (n < KEYSPERCODE && chord[n] != -1)
            {
                if (chord[n] == vk)
                    return;
                ++n;
            }
            if (n < KEYSPERCODE)
                chord[n] = vk;
            else
            {
                capturing = false;
                chord[KEYSPERCODE - 1] = vk;
                commit = true;
            }
        }
    }
    if (commit)
        ClearConflicts();

    SetDirty(true);
    if (capturing || rebindItem < 0)
        return;
    // Not capturing with a target: the chord goes into its cell.
    int32_t* keys = BindingKeys(rebindItem / kCellsPerControl, rebindItem % kCellsPerControl);
    for (int32_t k = 0; k < KEYSPERCODE; ++k)
        keys[k] = chord[k] != -1 ? chord[k] : 0;
    SControlEntry ce{};
    ControlMap.GetControlEntry(rebindItem / kCellsPerControl, &ce);
    log_info("[options] '%s' key %d: %s (until ok)", ce.name ? ce.name : "?", rebindItem % kCellsPerControl + 1,
             KeyCellText(keys).c_str());
    chord = {-1, -1, -1};
    rebindItem = -1;
}

// REVSYNC: 0x0053bb22-0x0053bc00 -- before a chord goes in, every code of
// every control sharing a mode with the target's that the chord matches is
// cleared (its other codes and the third, unshown code included). An empty
// chord slot matches any key, so a single key clears every code it starts.
void TOptionsPane::ClearConflicts()
{
    if (rebindItem < 0)
        return;
    SControlEntry target{};
    ControlMap.GetControlEntry(rebindItem / kCellsPerControl, &target);
    for (int32_t control = 0; control < Controls(); ++control)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(control, &ce);
        if (!(ce.mode & target.mode))
            continue;
        for (int32_t code = 0; code < CODESPERCOMMAND; ++code)
        {
            int32_t* keys = BindingKeys(control, code);
            bool match = true;
            for (int32_t k = 0; k < KEYSPERCODE; ++k)
                match = match && (chord[k] == -1 || keys[k] == chord[k]);
            if (match)
                std::fill(keys, keys + KEYSPERCODE, 0);
        }
    }
}
