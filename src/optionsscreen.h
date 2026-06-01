// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  optionsscreen.h - TOptionsScreen (DEF options screen + keymapper)     *
// *************************************************************************
//
// The game Options screen: a DEF-driven panel (options.def, via TDefScreen)
// plus the game-specific behaviour the generic widget engine doesn't know
// about — binding the controller LISTBOX to the live key-binding table
// (TControlMap ControlMap), key rebinding, and apply/cancel.
//
// This is the port's equivalent of retail TOptionsScreen (recon cls_0x5b9744):
// it owns a TDefScreen and layers the ControlMap wiring on top. Toggle/slider
// binding to the setting globals is roughed in (see Open()); the controller
// list + rebind + persist are the real path.

#pragma once

#include "defscreen.h"

#include <cstdint>

class TSurface;

class TOptionsScreen
{
  public:
    TOptionsScreen() = default;

    // Open the "options" panel at the given pane rect with the given chrome dat
    // (e.g. "optionsalpha.dat"). Ensures ControlMap is populated and binds the
    // controller list to it.
    bool Open(int32_t x, int32_t y, int32_t w, int32_t h, const char* bgDat);
    void Close();
    void Render();

    [[nodiscard]] TSurface* Surface() const { return def.Surface(); }
    [[nodiscard]] int32_t   PaneX() const { return def.PaneX(); }
    [[nodiscard]] int32_t   PaneY() const { return def.PaneY(); }

    // Input (pane-local coords). Mouse delegates to the DEF screen (button /
    // slider / row select); keys drive rebinding when a controller row is the
    // current selection. OnMouseUp returns the activated button name (and
    // persists bindings on "ok").
    void        OnMouseDown(int32_t lx, int32_t ly);
    const char* OnMouseUp(int32_t lx, int32_t ly);
    void        OnMouseMove(int32_t lx, int32_t ly);
    void        OnKey(int32_t vk, bool down);

  private:
    void RefreshControllerList();   // rebuild the 3-column rows from ControlMap

    TDefScreen def;
};
