// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *        spritetexoverride.h - modern sprite art from a local tree     *
// *************************************************************************
//
// Retail ships its characters, monsters, props and effect billboards as 3D
// models whose textures live inside the .I3D files as palettised bitmaps.
// This module lets a local tree of PNGs replace those pixels, so modern
// sprite art can be edited and seen in game without touching the retail
// assets. It is the sprite-side twin of tiletexoverride.h.
//
// The tree is laid out the way --dumpi3d writes it:
//
//   <root>/<subdir>/<name>/texture_<NN>_frame_<NN>.png   one frame
//   <root>/<subdir>/<name>/texture_<NN>.png               every frame
//
// <subdir>/<name> is the imagery file's path without its extension, lower
// case: "Chars\locke.i3d" -> "chars/locke". A folder named after the bare
// file name ("locke") is accepted too, as is one that keeps the extension.
//
// The art has to have the retail texture's size: the model's own dimensions
// come from it (flipbook cells, face aspect, the effect billboards' atlas
// grid), so a differently sized replacement is skipped with a warning.
//
// Settings (revisited/resources/Revenant.ini, [Revisited]):
//   SpriteOverride = 1        index the tree
//   SpriteRoot     = <path>   tree root; empty = <overlay>/Sprites
//   SpriteMode     = 0|1      0 = retail art, 1 = modern (override) art

#pragma once

#include <cstdint>
#include <vector>

namespace SpriteTexOverride {

struct SImage {
    int32_t              width  = 0;
    int32_t              height = 0;
    std::vector<uint8_t> rgba;   // width * height * 4
    // The PNG brought its own alpha (a colour type with an alpha channel).
    // Then that alpha is the art's own shape; without one the retail
    // black-key inference decides the coverage, like it does for the .I3D.
    bool                 has_alpha = false;
};

// Index the configured tree. Call at boot and again whenever the tree may
// have grown (the debug panel's reindex button). Returns the number of
// indexed texture files.
int32_t Init();

bool Enabled();

// True when the modern art replaces the retail pixels.
bool Active();

const char* Root();
int32_t     Count();

// True when the tree holds art for this texture frame of this imagery file.
bool Has(const char* imagery_filename, int32_t texture_index, int32_t frame_index);

// The PNG that would be used for this texture frame ("" when there is none).
// For the log: which file the art came from.
const char* ArtPath(const char* imagery_filename, int32_t texture_index, int32_t frame_index);

// Decoded RGBA of the override for one texture frame of one imagery file.
// `imagery_filename` is the retail file name as the registry keeps it
// ("Chars\locke.i3d").
bool Load(const char* imagery_filename, int32_t texture_index, int32_t frame_index, SImage& out);

} // namespace SpriteTexOverride

// Reloads the sprite textures of the game. Every loaded 3D imagery is
// visited; the frames the sprite tree has art for are the ones that can
// differ from what the GPU holds, so the rest are left alone.
namespace SpriteReload {

// Replaces the pixels of the sprite textures. `out_images` receives the
// number of imagery that changed. Returns the number of texture frames
// updated.
int32_t ReloadAll(int32_t* out_images = nullptr);

// Switches the art mode (0 = retail, 1 = modern), re-indexes the tree and
// reloads. The debug panel's radio buttons and the console command both go
// through here, so the two can not drift apart.
int32_t ApplyMode(int32_t mode, int32_t* out_images = nullptr);

} // namespace SpriteReload
