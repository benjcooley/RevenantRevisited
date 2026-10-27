// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *         tiletreload.h - live reload of the tile art (I2D)            *
// *************************************************************************
//
// The tile art (tiletexoverride.h) was applied once, when the tile's
// texture was uploaded: an edited PNG showed up only after a restart,
// because the colour image the tile pass binds was made once and never
// touched again.
//
// This module puts the wanted pixels into the images the game ALREADY
// draws. The map renderer keeps a registry of the loaded tiles (its
// implementation, maprenderer_internal.h): the image pair's handle, the
// retail bitmap, the tile's type. For every entry the colour image is
// replaced -- the depth map, the height chain and the tight proxy hull stay
// retail's, so only the tile's albedo changes.
//
// Two modes:
//   0 = retail  -- the tile shows the retail pixels again
//   1 = modern  -- the tile shows the tree's art (the PNG)
//
// The tree is re-indexed first (files may have been added, renamed or
// edited since the last index), then the loaded tiles are re-coloured.
//
// The debug panel and the "reloadtiles" console command both go through
// here, so the two can not drift apart.

#pragma once

#include <cstdint>

namespace TileArtReload {

// Reloads the tile art (the tiles' colour images).
//   mode 0 = the retail pixels, 1 = the art from the tree
// The tree is re-indexed first, then the already loaded tiles are
// re-coloured. Returns the number of tiles that were updated.
// `out_tiles` receives the number of tiles the renderer knows (the ones the
// reload walked).
int32_t ReloadAll(int32_t mode, int32_t* out_tiles = nullptr);

// True when the tile art tree is indexed and the feature is on.
bool Enabled();

// How many tiles the renderer knows (the ones the reload can re-colour).
int32_t TileCount();

} // namespace TileArtReload
