// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *         test_camerascroll.cpp - ground and mesh camera               *
// *************************************************************************
//
// The world is drawn against the camera's position at the tick's start, and
// the part of the camera's step that has passed scrolls the picture by a
// fraction of a pixel. The ground (the tiles) takes that scroll in its
// screen position; the meshes have to be drawn from the same camera, or
// every character drifts off the ground by up to a whole camera step and
// snaps back, 24 times a second.
//
// Expected values are worked by hand from the two projections -- the
// ground's iso projection plus the scroll, and the mesh vertex shader's
// projection (mesh.glsl.h / mesh.hlsl.h / mesh.metal.h) -- and not read
// back from the code under test.
// Run via build/test_camerascroll after build.
// *************************************************************************

#include "../src/camerascroll.h"

#include <gtest/gtest.h>

namespace {

constexpr float kIsoCos30 = 0.867f;   // the shaders' ISO_COS30
constexpr float kEps = 1e-3f;

// The mesh vertex shader's projection: camera-relative world coordinates
// (z stays absolute) turned into iso screen pixels.
void MeshProject(float wx, float wy, float wz,
                 float camw_x, float camw_y, float zoom,
                 float& screen_x, float& screen_y)
{
    const float rx = wx - camw_x;
    const float ry = wy - camw_y;
    screen_x = (rx - ry) * zoom;
    screen_y = (0.5f * (rx + ry) - wz * kIsoCos30) * zoom;
}

// The mesh vertex shader's scene depth. The ground's depth is measured
// against the same value, so it must not move when the camera scrolls.
float MeshSceneZ(float wx, float wy, float wz,
                 float camw_x, float camw_y, float camz_z)
{
    const float rx = wx - camw_x;
    const float ry = wy - camw_y;
    return camz_z - kIsoCos30 * (rx + ry) - 0.5f * wz;
}

} // namespace

TEST(CameraScroll, MeshesShareTheGroundCamera)
{
    const float zoom = 1.5f;
    const float center_wx = 100.0f, center_wy = 250.0f;
    const float kcam_forward = 2750.0f;
    const float scroll_x = 0.75f, scroll_y = -0.25f;    // world units
    const float wx = 140.0f, wy = 300.0f, wz = 12.0f;

    // the ground: the tick's camera, then the scroll in screen pixels
    float scroll_px = 0.0f, scroll_py = 0.0f;
    CameraScrollPixels(scroll_x, scroll_y, zoom, scroll_px, scroll_py);
    float ground_x = 0.0f, ground_y = 0.0f;
    MeshProject(wx, wy, wz, center_wx, center_wy, zoom, ground_x, ground_y);
    ground_x += scroll_px;
    ground_y += scroll_py;

    // the meshes: the tick's camera plus the same scroll
    float camw_x = 0.0f, camw_y = 0.0f, camz_z = 0.0f;
    MeshCameraUniforms(center_wx, center_wy, kcam_forward,
                       scroll_x, scroll_y, camw_x, camw_y, camz_z);
    float mesh_x = 0.0f, mesh_y = 0.0f;
    MeshProject(wx, wy, wz, camw_x, camw_y, zoom, mesh_x, mesh_y);

    EXPECT_NEAR(mesh_x, ground_x, kEps);
    EXPECT_NEAR(mesh_y, ground_y, kEps);
}

TEST(CameraScroll, MeshDepthDoesNotMove)
{
    const float center_wx = 100.0f, center_wy = 250.0f;
    const float kcam_forward = 2750.0f;
    const float scroll_x = 0.75f, scroll_y = -0.25f;
    const float wx = 140.0f, wy = 300.0f, wz = 12.0f;

    const float depth_before = MeshSceneZ(wx, wy, wz,
                                          center_wx, center_wy, kcam_forward);

    float camw_x = 0.0f, camw_y = 0.0f, camz_z = 0.0f;
    MeshCameraUniforms(center_wx, center_wy, kcam_forward,
                       scroll_x, scroll_y, camw_x, camw_y, camz_z);
    const float depth_after = MeshSceneZ(wx, wy, wz,
                                         camw_x, camw_y, camz_z);

    EXPECT_NEAR(depth_after, depth_before, kEps);
}

TEST(CameraScroll, NoScrollKeepsTheWholeCamera)
{
    const float center_wx = 100.0f, center_wy = 250.0f;
    const float kcam_forward = 2750.0f;

    float camw_x = 0.0f, camw_y = 0.0f, camz_z = 0.0f;
    MeshCameraUniforms(center_wx, center_wy, kcam_forward,
                       0.0f, 0.0f, camw_x, camw_y, camz_z);

    EXPECT_FLOAT_EQ(camw_x, center_wx);
    EXPECT_FLOAT_EQ(camw_y, center_wy);
    EXPECT_FLOAT_EQ(camz_z, kcam_forward);
}
