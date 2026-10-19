// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *         tiletexoverride.h - high-res tile art from a local tree       *
// *************************************************************************
//
// Revenant Revisited ships no artwork. A player (or us, while working on the
// upscale pipeline) can keep a tree of upscaled tile albedos next to the
// game; when --revisited is on and the tree exists, the tile upload path
// swaps the retail bitmap's colour for the matching PNG and leaves
// everything else (depth map, height min/max chain, tight proxy hull,
// coverage, registrations) exactly as retail has it.
//
// Naming follows what --dumptiles writes: "<NNNN>_<Name>.png", NNNN = the
// tile objtype index and Name = its object type name (e.g. 0763_Kep.png).
// The tree is scanned once at boot; a file under a "Final" folder wins over
// the plain set (hand-finished art over the bulk upscale). See
// revisited/builders/import_tile_textures.py for a tree we can copy from.
//
// Only the colour changes. The tile shader keys coverage off the colour
// alpha (tile.hlsl.h:124) and reconstructs normals + heights from the retail
// depth texture through UVs, so a higher-resolution colour is safe by
// construction; UploadTileBitmap still clips the override's alpha to the
// retail silhouette so art outside retail coverage cannot leak in.
//
// Settings (revisited/resources/Revenant.ini, [Revisited]):
//   HighResTiles     = 1        use the tree; 0 disables the whole feature
//   HighResTileRoot  = <path>   tree root; empty = <overlay>/Imagery

#pragma once

#include <cstdint>
#include <vector>

namespace TileTexOverride {

struct SImage {
    int32_t              width  = 0;
    int32_t              height = 0;
    std::vector<uint8_t> rgba;   // width * height * 4
};

// Index the configured tree. Call once at boot, after LoadRevisitedSettings.
// Returns the number of indexed tiles (0 when --revisited is off, the
// feature is disabled, or the root holds nothing usable).
int32_t Init();

bool Enabled();

// Decoded RGBA of the override for a tile, or false when the tile has none.
// The tiles this covers are stills only (state 0), like the dumper's.
bool Load(int32_t objtype, const char* type_name, SImage& out);

// Diagnostics for logs / the debug panel.
const char* Root();
int32_t     Count();

} // namespace TileTexOverride
