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

// The ground's scroll, in screen pixels.
inline void CameraScrollPixels(float scroll_x, float scroll_y, float zoom,
                               float& screen_x, float& screen_y)
{
    screen_x = -(scroll_x - scroll_y) * zoom;
    screen_y = -0.5f * (scroll_x + scroll_y) * zoom;
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
