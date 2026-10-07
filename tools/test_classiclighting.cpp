// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *       test_classiclighting.cpp - Classic (retail) light model         *
// *************************************************************************
//
// Pins ComputeClassicLightModel to the retail MMX light-table and T3DScene
// arithmetic derived in docs/LIGHTING_FIDELITY.md. Expected values are
// worked by hand from the retail formulas, not read back from the code.
// Run via build/test_classiclighting after build.
// *************************************************************************

#include "../src/classiclighting.h"
#include "../src/gameoptions.h"
#include "../src/graphics.h"
#include "../src/renderer.h"

#include <gtest/gtest.h>

namespace {

constexpr float kEps = 1e-4f;

SColor Color(uint8_t r, uint8_t g, uint8_t b) { return SColor{ r, g, b }; }

// Shipped Revenant.ini: Ambient3D=100, EnhancedLighting off.
SClassicLightSettings ShippedIni()
{
    SClassicLightSettings s;
    s.ambient3d = 100;
    return s;
}

} // namespace

TEST(ClassicTile, AmbientIsTheTruncatedMmxByte)
{
    // MMX table: the ambient byte is int(80 * A / 255) for a white ambient,
    // and the transfer's >> 3 makes a byte of 8 identity, so the gain is
    // byte / 8. AMBLIGHT 26 gives int(8.16) = 8, identity; 32 gives
    // int(10.04) = 10, a gain of 1.25.
    const SClassicLightModel m26 = ComputeClassicLightModel(26, Color(255, 255, 255), ShippedIni());
    for (float a : m26.tile_ambient)
        EXPECT_EQ(a, 1.0f);
    const SClassicLightModel m32 = ComputeClassicLightModel(32, Color(255, 255, 255), ShippedIni());
    for (float a : m32.tile_ambient)
        EXPECT_EQ(a, 10.0f / 8.0f);
}

TEST(ClassicTile, KeepAmbientAtGammaThree)
{
    // The Keep: AMBLIGHT 4 plus GammaLevel 3's offset of 10 = 14, AMBCOLOR
    // 155,155,210 (colour scaled to max 1). Bytes: int(80 * 155/210 * 14/255)
    // = int(3.24) = 3, and int(80 * 14/255) = int(4.39) = 4. This is the
    // multiplier the retail S1 capture shows on unlit floor and wall
    // (docs/LIGHTING_FIDELITY.md section 8).
    const SClassicLightModel m = ComputeClassicLightModel(14, Color(155, 155, 210), ShippedIni());
    EXPECT_EQ(m.tile_ambient[0], 3.0f / 8.0f);
    EXPECT_EQ(m.tile_ambient[1], 3.0f / 8.0f);
    EXPECT_EQ(m.tile_ambient[2], 4.0f / 8.0f);
    // Light byte per unit intensity and multiplier: 80 * (1 - 14/255) / 20,
    // as a gain (/ 8). Multiplier 12 at full intensity: 5.67.
    EXPECT_NEAR(m.tile_gain_per_mult, 80.0f * (1.0f - 14.0f / 255.0f) / 20.0f / 8.0f, kEps);
    EXPECT_NEAR(m.tile_gain_per_mult * 12.0f, 5.6696f, 1e-3f);
}

TEST(ClassicTile, KeepAmbientWithoutGammaOffsetIsNearBlack)
{
    // AMBLIGHT 4 alone (GammaLevel 2): the red and green bytes truncate to
    // 0 and blue to 1, so unlit stone is near black with a blue cast.
    const SClassicLightModel m = ComputeClassicLightModel(4, Color(155, 155, 210), ShippedIni());
    EXPECT_EQ(m.tile_ambient[0], 0.0f);
    EXPECT_EQ(m.tile_ambient[1], 0.0f);
    EXPECT_EQ(m.tile_ambient[2], 1.0f / 8.0f);
}

TEST(ClassicTile, BlackAmbientColourIsSafe)
{
    const SClassicLightModel m = ComputeClassicLightModel(30, Color(0, 0, 0), ShippedIni());
    for (float a : m.tile_ambient)
        EXPECT_EQ(a, 0.0f);
    for (float a : m.mesh_ambient)
        EXPECT_EQ(a, 0.0f);
}

TEST(ClassicMesh, KeepAmbientSplitsIntoKeyLight)
{
    // level = 100 * 4 * 8 / 100 = 32; colour256 = (188, 188, 255);
    // chan = (32*c) >> 8 = (23, 23, 31). UseDirLight: D3D ambient is 15% of
    // chan, the key light 85% * 3.0 / 256 (RealTimeLight=No).
    const SClassicLightModel m = ComputeClassicLightModel(4, Color(155, 155, 210), ShippedIni());
    EXPECT_NEAR(m.mesh_ambient[0], 3.0f / 255.0f, kEps);
    EXPECT_NEAR(m.mesh_ambient[2], 4.0f / 255.0f, kEps);
    EXPECT_NEAR(m.mesh_dir_color[0], 23.0f * 0.85f * 3.0f / 256.0f, kEps);
    EXPECT_NEAR(m.mesh_dir_color[2], 31.0f * 0.85f * 3.0f / 256.0f, kEps);
    // ambint = ((23 + 23 + 31) / 3) / 255.
    EXPECT_NEAR(m.mesh_ambient_intensity, 25.0f / 255.0f, kEps);
    EXPECT_NEAR(m.mesh_gain_per_mult, 0.2f * 2.5f, kEps);
    EXPECT_EQ(m.mesh_overbright, 1.0f);
}

TEST(ClassicMesh, EnhancedLightingQuartersInputsAndAllowsOverbright)
{
    SClassicLightSettings s = ShippedIni();
    s.enhanced_lighting = true;
    // level = 100 * 4 * 2 / 100 = 8; chan = (5, 5, 7); the D3D ambient's
    // 15% truncates to 0; outputs are scaled back up by 4.
    const SClassicLightModel m = ComputeClassicLightModel(4, Color(155, 155, 210), s);
    EXPECT_EQ(m.mesh_ambient[0], 0.0f);
    EXPECT_NEAR(m.mesh_dir_color[2], 4.0f * 7.0f * 0.85f * 3.0f / 256.0f, kEps);
    // ambint is computed at the full x8 level regardless of the mode.
    EXPECT_NEAR(m.mesh_ambient_intensity, 25.0f / 255.0f, kEps);
    EXPECT_EQ(m.mesh_overbright, 4.0f);
}

TEST(ClassicMesh, KeyLightComesFromAboveAndPlusY)
{
    const SClassicLightModel m = ComputeClassicLightModel(30, Color(255, 255, 255), ShippedIni());
    EXPECT_NEAR(m.mesh_dir_to_light[0], 0.0f, kEps);
    EXPECT_GT(m.mesh_dir_to_light[1], 0.0f);
    EXPECT_GT(m.mesh_dir_to_light[2], 0.0f);
    const float len2 = m.mesh_dir_to_light[0] * m.mesh_dir_to_light[0] +
                       m.mesh_dir_to_light[1] * m.mesh_dir_to_light[1] +
                       m.mesh_dir_to_light[2] * m.mesh_dir_to_light[2];
    EXPECT_NEAR(len2, 1.0f, kEps);
}

TEST(ClassicMesh, MapLightsDoNotReachMeshesWithoutRealTimeLight)
{
    // LightAffectObject (0x00415c70) adds map lights only with
    // RealTimeLight on; Classic renders the RealTimeLight=No image.
    const SClassicLightModel m = ComputeClassicLightModel(14, Color(155, 155, 210), ShippedIni());
    EXPECT_FALSE(m.mesh_map_lights);
}

TEST(GammaAmbient, OffsetPerLevel)
{
    // TMapPane::SetAmbientLight (0x00453640): (GammaLevel * 5 - 10) * 2.
    EXPECT_EQ(GammaAmbientOffset(0), -20);
    EXPECT_EQ(GammaAmbientOffset(2), 0);
    EXPECT_EQ(GammaAmbientOffset(3), 10);
    EXPECT_EQ(GammaAmbientOffset(4), 20);
}

TEST(ClassicLights, NonPositiveMultiplierUsesDefaultTable)
{
    EXPECT_EQ(RetailLightMultiplier(0), 28);
    EXPECT_EQ(RetailLightMultiplier(-1), 28);
    EXPECT_EQ(RetailLightMultiplier(12), 12);
}
