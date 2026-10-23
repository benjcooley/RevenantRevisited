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

// The pixel-snap: a fractional scroll draws everything between two pixels
// and the resampled pixels crawl while the camera moves, so the scroll is
// snapped to whole screen pixels (the ground, the meshes and the effects
// then step together), and every object is shifted onto the grid as a
// whole (a world translation -- the ortho projection is affine, so the
// mesh keeps its shape and does not blur).
TEST(CameraScroll, SnappedScrollProjectsToWholePixels)
{
    for (float zoom : {1.0f, 1.5f, 2.0f, 2.6666f, 4.0f})
    {
        for (float wx = -6.0f; wx <= 6.0f; wx += 0.37f)
        {
            for (float wy = -5.0f; wy <= 5.0f; wy += 0.43f)
            {
                float psx = 0.0f, psy = 0.0f;
                CameraScrollPixels(wx, wy, zoom, psx, psy);
                const float want_x = std::floor(psx + 0.5f);
                const float want_y = std::floor(psy + 0.5f);

                float snap_wx = 0.0f, snap_wy = 0.0f;
                SnapCameraScroll(wx, wy, zoom, snap_wx, snap_wy);
                float rsx = 0.0f, rsy = 0.0f;
                CameraScrollPixels(snap_wx, snap_wy, zoom, rsx, rsy);

                EXPECT_NEAR(rsx, want_x, kEps);
                EXPECT_NEAR(rsy, want_y, kEps);

                // Snapping again must not move it: it is already whole.
                float snap2_wx = 0.0f, snap2_wy = 0.0f;
                SnapCameraScroll(snap_wx, snap_wy, zoom, snap2_wx, snap2_wy);
                EXPECT_NEAR(snap2_wx, snap_wx, kEps);
                EXPECT_NEAR(snap2_wy, snap_wy, kEps);
            }
        }
    }
}

TEST(CameraScroll, MeshSnapDeltaLandsTheAnchorOnTheGrid)
{
    for (float zoom : {1.0f, 1.5f, 2.0f, 2.6666f, 4.0f})
    {
        for (float wx = -30.0f; wx <= 30.0f; wx += 1.31f)
        {
            for (float wy = -30.0f; wy <= 30.0f; wy += 0.77f)
            {
                for (float wz = -4.0f; wz <= 4.0f; wz += 1.7f)
                {
                    float dx = 0.0f, dy = 0.0f;
                    MeshPixelSnapDelta(wx, wy, wz, zoom, dx, dy);

                    // The camera is not moved by the shift: the anchor's
                    // projected pixels land on the grid.
                    const float sx = (wx - wy) * zoom;
                    const float sy = (0.5f * (wx + wy) - wz * kIsoCos30) * zoom;
                    const float snapped_sx = ((wx + dx) - (wy + dy)) * zoom;
                    const float snapped_sy = (0.5f * ((wx + dx) + (wy + dy)) - wz * kIsoCos30) * zoom;
                    EXPECT_NEAR(snapped_sx, std::floor(sx + 0.5f), kEps);
                    EXPECT_NEAR(snapped_sy, std::floor(sy + 0.5f), kEps);
                }
            }
        }
    }
}
