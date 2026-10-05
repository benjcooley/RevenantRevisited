// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionspane.h - TOptionsPane (DEF options screen + keymapper)     *
// *************************************************************************
//
// The game Options screen: a DEF-driven panel (options.def, via TDefPane)
// plus the game-specific behaviour the generic widget engine doesn't know
// about — binding the controller LISTBOX to the live key-binding table
// (TControlMap ControlMap), key rebinding, and apply/cancel.
//
// Retail TOptionsPane (recon cls_0x5b9744, instance 0x0066fcc0): a DEF pane
// (TDefPane) plus the ControlMap wiring. Toggle/slider binding to the setting
// globals is roughed in (see OpenOptions()); the controller list + rebind +
// persist are the real path. Hosted modally by the in-game menu and as an
// ordinary pane by the title's Options screen; its "ok" / "cancel" go to the
// host's activation handler (docs/gameflow/forensics/INGAME_MENU.md §7).

#pragma once

#include "defpane.h"

#include <cstdint>

class TOptionsPane : public TDefPane
{
  public:
    TOptionsPane() = default;

    // REVSYNC: Open @ 0x0053a8b0 -- options.def panel "default", 640x480 at
    // display (x,y), with the in-game chrome and fade when `fromGame`
    // (DEF flags 0x11, else 0). Ensures ControlMap is populated and binds the
    // controller list to it.
    bool OpenOptions(bool fromGame, int32_t x, int32_t y);

  protected:
    // "ok" persists the key bindings, then the activation handler runs.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    // With a controller row selected, the next key becomes its Key 1 binding.
    void OnKey(int32_t vk, bool down) override;

  private:
    void RefreshControllerList();   // rebuild the 3-column rows from ControlMap
};
