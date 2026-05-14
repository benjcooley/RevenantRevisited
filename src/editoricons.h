// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editoricons.h - Object-category icons for the editor        *
// *************************************************************************
//
// Loads PNG icons from REV_EDITOR_DATA_DIR/icons/<category>.png as renderer
// textures, keyed by OBJCLASS_*. Inventory item classes (WEAPON, ARMOR ...
// MAPSCROLL) all collapse onto the single "ITEM" icon. Categories with
// no icon return kInvalidTexture.
//
// Lifecycle:
//   EditorIcons::Build();    // call once after the renderer is up
//   EditorIcons::Shutdown(); // releases the renderer refs on app exit
//
// *************************************************************************

#pragma once

#include "render3d_types.h"

#include <cstdint>

namespace EditorIcons {

void     Build();
void     Shutdown();

// kInvalidTexture when no icon exists for the class.
TTextureHandle ForObjClass(int32_t obj_class);

// Generic / sector folder icon.
TTextureHandle Sector();

} // namespace EditorIcons
