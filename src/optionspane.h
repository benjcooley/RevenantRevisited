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
// Retail TOptionsPane (recon cls_0x5b9744): a DEF pane (TDefPane) plus the
// ControlMap wiring. Toggle/slider binding to the setting globals is roughed
// in (see OpenOptions()); the controller list + rebind + persist are the real
// path.

#pragma once

#include "defpane.h"

#include <cstdint>

class TOptionsPane : public TDefPane
{
  public:
    TOptionsPane() = default;

    // Open the "options" panel (options.def, panel "default") at display (x,y)
    // size (w,h) with chrome `bgDat` (e.g. "optionsalpha.dat"). Ensures
    // ControlMap is populated and binds the controller list to it.
    bool OpenOptions(int32_t x, int32_t y, int32_t w, int32_t h, const char* bgDat);

  protected:
    // "ok" persists the key bindings, then the activation handler runs.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    // With a controller row selected, the next key becomes its Key 1 binding.
    void OnKey(int32_t vk, bool down) override;

  private:
    void RefreshControllerList();   // rebuild the 3-column rows from ControlMap
};
