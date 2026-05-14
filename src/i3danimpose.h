// *************************************************************************
// *                         Cinematix Revenant                            *
// *              i3danimpose.h - I3D-to-pose animation bridge              *
// *************************************************************************

#pragma once

#include "animsystem.h"

class T3DImagery;

// ---------------------------------------------------------------------------
// Legacy I3D adapter boundary
// ---------------------------------------------------------------------------
//
// Runtime clips should be track data keyed in seconds; sampling tracks produces
// a pose buffer, and blending only blends pose buffers. I3D files are legacy
// state/frame data, so everything below this line is adapter code whose job is
// translating old integer-frame keys into caller-owned pose buffers. The
// no-allocation hot path is the fixed pose-buffer API; SAnimPose return values
// remain for tools/tests and migration-only callers.

// Configure the standard per-bone local TRS layout used by I3D character data.
// This may allocate layout metadata and is intended for load/attach time.
void ConfigureI3DLegacyPoseLayout(T3DImagery* img, SAnimPoseLayout& layout);

// Sample one legacy state/frame into `out`. The caller owns and reuses `out`;
// this function performs no allocations in normal operation. `prevstate` and
// `prevframe` are passed through to the legacy key decoder only to preserve
// old cross-state interpolation behavior while the game still drives integer
// frame state.
bool SampleI3DLegacyFrameToPose(T3DImagery* img,
                                const SAnimPoseLayout& layout,
                                int32_t state,
                                int32_t frame,
                                int32_t prevstate,
                                int32_t prevframe,
                                float* out,
                                int32_t out_count);

// Sample the current legacy frame, optionally sample the next frame, and blend
// the two sampled pose buffers by framefrac. If prevstate/prevframe describe a
// real state-entry transition, the previous pose is blended into the current
// pose with the same core pose-buffer blender. Loop wraps are valid key
// interpolation: the key after the last frame is the first frame, never a
// transition. The three buffers are caller-owned fixed storage; no normal
// per-frame allocation is allowed here. `out_pose` points at one of those
// buffers on success.
bool SampleI3DLegacyFrameBlendToPose(T3DImagery* img,
                                     const SAnimPoseLayout& layout,
                                     int32_t state,
                                     int32_t frame,
                                     int32_t nextstate,
                                     int32_t nextframe,
                                     float framefrac,
                                     int32_t prevstate,
                                     int32_t prevframe,
                                     float* current_out,
                                     float* next_out,
                                     float* blended_out,
                                     int32_t out_count,
                                     const float** out_pose);

// Sample legacy I3D object animation keys into the modern channel-pose model.
// Rotation keys are converted from legacy Euler XYZ to quaternion channels.
// Compatibility/tooling API only: this map-backed pose allocates and must not
// be used by the live per-frame animator.
SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame);
SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame,
                            int32_t prevstate, int32_t prevframe);

// Facade for the old integer-frame system: game logic owns state/frame, while
// render code can ask for a fractional pose between the current and next frame.
// Compatibility/tooling API only; live code should blend fixed buffers.
SAnimPose SampleI3DAnimPoseInterpolated(T3DImagery* img, int32_t state,
                                        int32_t frame, int32_t nextframe,
                                        float framefrac,
                                        int32_t prevstate = -1,
                                        int32_t prevframe = 0);

SAnimPose BlendI3DAnimPoses(const SAnimPose& a, const SAnimPose& b, float t);
