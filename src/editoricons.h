// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editoricons.h - Object-category icons for the editor        *
// *************************************************************************
//
// Loads PNG icons from REV_EDITOR_DATA_DIR/icons/<category>.png as sokol
// images, keyed by OBJCLASS_*. Inventory item classes (WEAPON, ARMOR ...
// MAPSCROLL) all collapse onto the single "ITEM" icon. Categories with
// no icon return a zeroed sg_image so callers can branch on `id == 0`.
//
// Lifecycle:
//   EditorIcons::Build();    // call once after sokol_gfx is up
//   ImGui::Image((ImTextureID)(uintptr_t) EditorIcons::ForObjClass(c).id, sz);
//   EditorIcons::Shutdown(); // free the textures on app exit
//
// *************************************************************************

#pragma once

#include <sokol_gfx.h>

#include <cstdint>

namespace EditorIcons {

void     Build();
void     Shutdown();

// sg_image{0} when no icon exists for the class.
sg_image ForObjClass(int32_t obj_class);

// Generic / sector folder icon.
sg_image Sector();

} // namespace EditorIcons
