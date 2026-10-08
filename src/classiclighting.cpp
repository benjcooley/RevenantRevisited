// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     classiclighting.cpp - Retail lighting model inputs (Classic)      *
// *************************************************************************
//
// See classiclighting.h and docs/LIGHTING_FIDELITY.md §2-§3.

#include "classiclighting.h"

#include "graphics.h"   // SColor
#include "renderer.h"   // SClassicLightModel

#include <algorithm>
#include <cmath>

namespace {

// ---- DLS light table, MMX form (colortable.cpp SetLightColor; retail
// FUN_0041da10 fills MMXLightTable @0x00635144). Every CPU with MMX (all
// from 1997 on) transfers through it (FUN_0043d1f0 -> FUN_0043c860): a
// multiplier byte M per channel, and a 5-bit colour channel c5 becomes
// min(255, c5 * M) >> 3, so a byte of 8 is identity and the gain is M / 8.
constexpr double  kMmxTableScale     = 80.0;  // mmxclr @0x5a3a28
constexpr double  kMultiplierScale   = 20.0;  // multiplierscale  @0x5c6e68
constexpr double  kAmbientMultiplier = 1.0;   // AmbientMultiplier @0x5c6e70
constexpr double  kMultiplierByteMax = 255.0; // @0x5a3a20
constexpr double  kIdentityByte      = 8.0;   // >> 3 in the transfer

// ---- T3DScene (3dscene.cpp; retail FUN_00414310 / FUN_004143d0 /
//      FUN_00412db0 / FUN_00411b70 / FUN_00411eb0)
constexpr int32_t kAmbientLevelScale       = 8;      // ambient * 8 * Ambient3D / 100
constexpr double  kLightBrightnessAtSource = 0.2;    // GetBrightness @0x5a37e8
constexpr double  kDirFactorRealTimeLight  = 1.5;    // @0x5a3810
constexpr double  kDirFactor               = 3.0;    // @0x5a36dc
// EnhancedLighting picks MODULATE4X on a device that reports it (any
// hardware since the late 1990s, and dgVoodoo), else MODULATE2X.
constexpr int32_t kEnhancedOverbright      = 4;
// Key light, world space, direction of travel (scene init FUN_00411eb0).
constexpr double  kDirLightTravel[3]       = { 0.0, -0.78125, -0.625 };

// Classic renders retail's RealTimeLight=No tile path (the DLS light
// tables), so the 3D scene uses that configuration's key-light factor.
constexpr bool kClassicRealTimeLight = false;

} // namespace

SClassicLightModel ComputeClassicLightModel(int32_t amblight, const SColor &ambcolor,
                                            const SClassicLightSettings &settings)
{
    SClassicLightModel m;
    const int32_t ambient = (std::clamp)(amblight, 0, 255);
    const int32_t chan_in[3] = { ambcolor.red, ambcolor.green, ambcolor.blue };
    const int32_t chan_max = (std::max)({ chan_in[0], chan_in[1], chan_in[2] });

    // ---- Static tiles: SetLightColor builds each MMX multiplier byte as
    //   M = int(min(255, 80*l*(1 - A/255)*(I/63)*(mult/20) + int(min(255, 80*a*(A/255)*AmbientMultiplier))))
    // with a and l scaled so their largest channel is 1. The ambient byte is
    // truncated on its own, before the light is added; the light pass adds
    // the light term and truncates again.
    const double ambient_fraction = double(ambient) / 255.0;
    for (int32_t i = 0; i < 3; ++i)
    {
        const double a = chan_max > 0 ? double(chan_in[i]) / double(chan_max) : 0.0;
        const double ambient_byte =
            std::floor((std::min)(kMmxTableScale * a * ambient_fraction * kAmbientMultiplier, kMultiplierByteMax));
        m.tile_ambient[i] = float(ambient_byte / kIdentityByte);
    }
    m.tile_gain_per_mult = float(kMmxTableScale * (1.0 - ambient_fraction) / kMultiplierScale / kIdentityByte);

    // ---- 3D meshes. Every light input is divided by the overbright scale s
    // and the texture stage multiplies it back; integer steps as retail.
    const int32_t s = settings.enhanced_lighting ? kEnhancedOverbright : 1;
    int32_t color256[3] = {};                    // SetAmbientColor: (c << 8) / max, cap 255
    for (int32_t i = 0; i < 3; ++i)
        color256[i] = chan_max > 0 ? (std::min)(255, (chan_in[i] << 8) / chan_max) : 0;

    const int32_t level  = settings.ambient3d * ambient * (kAmbientLevelScale / s) / 100;
    const int32_t level8 = settings.ambient3d * ambient * kAmbientLevelScale / 100;
    int32_t chan[3] = {};
    int32_t sum8 = 0;
    for (int32_t i = 0; i < 3; ++i)
    {
        chan[i] = (std::min)(255, (level * color256[i]) >> 8);
        sum8   += (level8 * color256[i]) >> 8;
    }
    // SetAmbientLight's ambient intensity, subtracted from each light's
    // brightness before the [0,1] clamp (vertex-colour units, before s).
    m.mesh_ambient_intensity = float((std::min)(255, sum8 / 3)) / 255.0f;

    const double dir_factor = (kClassicRealTimeLight && settings.max_lights != 0)
                                  ? kDirFactorRealTimeLight : kDirFactor;
    for (int32_t i = 0; i < 3; ++i)
    {
        if (settings.use_dir_light)
        {
            const int32_t amb = (std::min)(255, (100 - settings.dir_light_percent) * chan[i] / 100);
            const double  dir = (std::min)(1.0, double(chan[i]) * settings.dir_light_percent *
                                                    dir_factor * 0.01 / 256.0);
            m.mesh_ambient[i]   = float(s * amb) / 255.0f;
            m.mesh_dir_color[i] = float(s * dir);
        }
        else
        {
            m.mesh_ambient[i]   = float(s * chan[i]) / 255.0f;
            m.mesh_dir_color[i] = 0.0f;
        }
    }
    m.mesh_gain_per_mult = float(kLightBrightnessAtSource * settings.light_mult3d * 0.01);

    const double len = std::sqrt(kDirLightTravel[0] * kDirLightTravel[0] +
                                 kDirLightTravel[1] * kDirLightTravel[1] +
                                 kDirLightTravel[2] * kDirLightTravel[2]);
    for (int32_t i = 0; i < 3; ++i)
        m.mesh_dir_to_light[i] = float(-kDirLightTravel[i] / len);
    m.mesh_overbright = float(s);
    // LightAffectObject (0x00415c70) adds an object's nearest map lights only
    // when RealTimeLight is on; under RealTimeLight=No a 3D object gets the
    // ambient and the key light alone.
    m.mesh_map_lights = kClassicRealTimeLight;
    return m;
}
