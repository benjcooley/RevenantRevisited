// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionspane.h - TOptionsPane (DEF options screen + keymapper)     *
// *************************************************************************
//
// The game Options screen: a DEF-driven panel (options.def, via TDefPane)
// plus what the generic widget engine doesn't know about — the settings its
// toggles and sliders edit, and the controller list bound to the key-binding
// table (TControlMap ControlMap) with key rebinding.
//
// Retail TOptionsPane (recon cls_0x5b9744, instance 0x0066fcc0): opening
// copies the setting globals and the key bindings into the pane; the
// controls edit those copies; OK writes them back, saves [Options] and
// [Controls]; Cancel drops them. The music slider applies while it moves.
// docs/gameflow/forensics/OPTIONS.md. Hosted modally by the in-game menu and
// as an ordinary pane by the title's Options screen; after the pane's own
// work, "ok" / "cancel" go to the host's activation handler
// (INGAME_MENU.md §7).

#pragma once

#include "defpane.h"

#include <cstdint>
#include <vector>

// The settings as the pane edits them (retail members +0x180..+0x1a8).
struct SOptionsPaneValues
{
    bool    realTimeLight    = false;   // "Realtime"
    bool    autoCombat       = false;   // "Auto"
    bool    playSpeech       = false;   // "Dialog"
    bool    combatFace       = false;   // "Face"
    bool    enhancedLighting = false;   // "Enhanced"
    bool    limitSpeed       = false;   // "Limit"
    bool    noCombatResults  = false;   // "NoCombatRes"
    int32_t violence         = 0;       // "Violence"
    int32_t music            = 0;       // "Music"
    int32_t effects          = 0;       // "Sound"
    int32_t gamma            = 0;       // "Gamma"
};

class TOptionsPane : public TDefPane
{
  public:
    TOptionsPane() = default;

    // REVSYNC: Open @ 0x0053a8b0 -- copies the settings and the key bindings
    // into the pane, then opens options.def panel "default", 640x480 at
    // display (x,y), with the in-game chrome and fade when `fromGame`
    // (DEF flags 0x11, else 0).
    bool OpenOptions(bool fromGame, int32_t x, int32_t y);

  protected:
    // Event 1: the toggles and sliders from the copies, the controller list.
    void OnOpened() override;
    // A toggle sets its copy; "ok" applies and saves; then the host's handler.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    // Event 4000: the slider's copy; the music level applies at once.
    void OnSliderChanged(const SDefWidget& slider) override;
    // With a controller row selected, the next key becomes its Key 1 binding.
    void OnKey(int32_t vk, bool down) override;

  private:
    void Apply();                   // "ok": the copies into the settings, saved
    void RefreshControllerList();   // rebuild the 3-column rows from the bindings
    [[nodiscard]] int32_t* BindingKeys(int32_t control, int32_t code);

    SOptionsPaneValues values;
    // Retail rebind buffer +0x1ac: each control's CODESPERCOMMAND codes of
    // KEYSPERCODE keys, edited here and committed to ControlMap by "ok".
    std::vector<int32_t> bindings;
};
