// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           camerascroll.h - one camera for the whole picture           *
// *************************************************************************
//
// The world is drawn against the camera's position at the tick's start, and
// the part of the camera's step that has passed scrolls the picture by a
// fraction of a pixel, so the view moves smoothly instead of jumping once
// a tick (see TMapPane::CameraScrollStep).
//
// The ground takes that scroll in its screen position; the meshes have to
// be drawn from the same camera, or every character drifts off the ground
// by up to a whole camera step and snaps back, 24 times a second.
//
// Header only and free of engine types: the source-slice tests compile a
// piece of the renderer against this file.
// *************************************************************************

#pragma once

#include <cmath>

// The ground's scroll, in screen pixels.
inline void CameraScrollPixels(float scroll_x, float scroll_y, float zoom,
                               float& screen_x, float& screen_y)
{
    screen_x = -(scroll_x - scroll_y) * zoom;
    screen_y = -0.5f * (scroll_x + scroll_y) * zoom;
}

// The camera's step snapped to whole screen pixels, carried back in world
// units so every drawing path (the ground, the meshes, the overlays and the
// effects) sees the same quantized camera and they all move together. The
// fractional scroll drew everything between two pixels, and the resampled
// pixels crawled while the camera moved.
inline void SnapCameraScroll(float scroll_x, float scroll_y, float zoom,
                             float& out_wx, float& out_wy)
{
    out_wx = scroll_x;
    out_wy = scroll_y;
    if (!(zoom > 0.0001f))
        return;
    float psx = 0.0f, psy = 0.0f;
    CameraScrollPixels(scroll_x, scroll_y, zoom, psx, psy);
    const float rsx = std::floor(psx + 0.5f);
    const float rsy = std::floor(psy + 0.5f);
    // The iso projection inverted: a world step (wx, wy) moves the picture
    // by (-(wx-wy), -0.5*(wx+wy)) * zoom pixels.
    out_wx = -(rsx + 2.0f * rsy) / (2.0f * zoom);
    out_wy = ( rsx - 2.0f * rsy) / (2.0f * zoom);
}

// The world shift that lands an object drawn between pixels onto the pixel
// grid. The ortho projection is affine, so shifting the object's world
// position by the result moves every vertex by the same pixel amount: the
// mesh keeps its shape, does not blur, and moves in whole-pixel steps.
// rel_* is the object's anchor relative to its camera; 0.867 is the
// shaders' ISO_COS30 (mesh.glsl.h / mesh.hlsl.h / mesh.metal.h).
inline void MeshPixelSnapDelta(float rel_x, float rel_y, float rel_z, float zoom,
                               float& out_wx, float& out_wy)
{
    out_wx = 0.0f;
    out_wy = 0.0f;
    if (!(zoom > 0.0001f))
        return;
    const float S = rel_x - rel_y;
    const float T = 0.5f * (rel_x + rel_y) - rel_z * 0.867f;
    const float dx = std::floor(S * zoom + 0.5f) - S * zoom;
    const float dy = std::floor(T * zoom + 0.5f) - T * zoom;
    // A world step (wx, wy) moves the image by ((wx-wy), 0.5*(wx+wy)) * zoom
    // pixels; solve for the step that lands the anchor on the grid.
    out_wx =  (dx + 2.0f * dy) / (2.0f * zoom);
    out_wy = (-dx + 2.0f * dy) / (2.0f * zoom);
}

// The camera the meshes are drawn from: the tick's camera plus that same
// scroll. The depth base is compensated by the same amount, so the scene
// depth (which the ground's depth is measured against) does not move.
// 0.867 is the shaders' ISO_COS30 (mesh.glsl.h / mesh.hlsl.h / mesh.metal.h).
inline void MeshCameraUniforms(float center_wx, float center_wy, float kcam_forward,
                                float scroll_x, float scroll_y,
                                float& camw_x, float& camw_y, float& camz_z)
{
    camw_x = center_wx + scroll_x;
    camw_y = center_wy + scroll_y;
    camz_z = kcam_forward - 0.867f * (scroll_x + scroll_y);
}
