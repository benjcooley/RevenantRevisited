// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   imageryatlas.h - the game's imagery fed into the engine's bitmap    *
// *                    atlases (inventory icons, 2D imagery previews)     *
// *************************************************************************

#pragma once

#include "engine/render/bitmapatlas.h"

class TAnimation;

// Registers every frame of an animation with the UI atlas build in progress.
void RegisterUIBitmapAtlasAnimation(TAnimation* anim);
// Registers every imagery state's inventory icon (and icon animation).
// Returns the number of bitmaps registered.
int32_t RegisterUIBitmapAtlasInventoryImagery();
// Packs the registry's 2D imagery into one atlas, for the imagery browser.
bool BuildImageryBitmapAtlas(SBitmapAtlas* atlas);
