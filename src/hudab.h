// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      hudab.h - the port's side of the HUD's retail A/B dumps          *
// *************************************************************************
//
// Targets for `Revenant --retail-ab=<target>` (retailab.h) that check the
// HUD's building blocks against the shipped game's own code in the
// emulator's HUD slot (tools/retail_runtime/slots/hud/, docs/ui/
// HUD_REBUILD.md). Each takes a case's tab-separated fields and returns the
// result object as JSON, or sets `error`.
//
// *************************************************************************

#pragma once

#include <string>
#include <vector>

namespace HudAB
{

// draw-put: fields = dest format (565 | 555 | 4444 | 1555), width, height,
// dest pixels (hex), source bitmap (hex: a TBitmap's bytes, header and
// blocks), x, y, drawmode. Puts the bitmap into the destination with
// TBitmap::Put and returns {"pixels": hex}. Retail side: draw_ab.py.
std::string DrawPut(const std::vector<std::string>& fields, std::string& error);

} // namespace HudAB
