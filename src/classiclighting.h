// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *      classiclighting.h - Retail lighting model inputs (Classic)       *
// *************************************************************************
//
// The light pass's Classic model (TRenderer lighting mode 0) reproduces
// retail's lighting: static tiles through the DLS light tables
// (colortable.cpp / dls.cpp) and 3D meshes through T3DScene's vertex
// lighting (3dscene.cpp). This module turns the area ambient and the
// lighting settings into the model's per-frame constants and resolves
// authored light multipliers the way retail did.
//
// Every constant and formula is cited against the retail binary in
// docs/LIGHTING_FIDELITY.md.
//
// *************************************************************************

#pragma once

#include <cstdint>

struct SColor;
struct SClassicLightModel;

// Retail NewLightIndex (colortable.cpp; retail FUN_0041d890): a light whose
// multiplier is 0 or less uses the default light table, multiplier 28.
inline constexpr int32_t kRetailDefaultLightMultiplier = 28;

[[nodiscard]] constexpr int32_t RetailLightMultiplier(int32_t authored)
{
    return authored > 0 ? authored : kRetailDefaultLightMultiplier;
}

// The settings the retail lighting reads. Defaults are retail's own
// (GetINISettings / the Options reader / 3dscene.cpp statics).
struct SClassicLightSettings {
    int32_t ambient3d         = 130;    // [Lighting]Ambient3D
    int32_t light_mult3d      = 250;    // [Lighting]LightMult3D
    int32_t max_lights        = 3;      // [Lighting]MaxLights
    bool    enhanced_lighting = false;  // [Options]EnhancedLighting
    bool    use_dir_light     = true;   // UseDirLight
    int32_t dir_light_percent = 85;     // DirLightPercent
};

// Classic light-model constants for an ambient level and colour (the
// TMapPane ambient, i.e. AREA.DEF AMBLIGHT / AMBCOLOR after day/night
// blending).
[[nodiscard]] SClassicLightModel ComputeClassicLightModel(int32_t amblight, const SColor &ambcolor,
                                                          const SClassicLightSettings &settings);
