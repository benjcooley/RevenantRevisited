// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *      meshextract.h - Convert T3DImagery sub-objects to mesh data      *
// *************************************************************************
//
// Pulls the per-sub-object geometry out of a T3DImagery and adapts it to
// the TRenderer mesh pipeline. This mirrors how T3DImagery::RenderObject()
// consumes the decoded data:
//
//   - GetObjVerts(..., Vertex) returns the stored retail-style S3DVertex
//     records; we forward pos/normal/uv into SMeshVertex.
//   - GetObjFaces repacks the object's per-texture face runs into one dense
//     object-local face buffer; the S3DFace indices remain local to the
//     sub-object's own vertex array.
//
// Also provides a static pose helper that walks the T3DImagery hierarchy
// at a given state/frame and returns each sub-object's world matrix in
// row-major layout (ready for SMeshSubmit::world).
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <vector>

#include "animsystem.h"
#include "renderer.h"

class T3DImagery;

// Extract one sub-object as mesh data. Returns false if the sub-object is
// empty or the imagery has no mesh data.
bool ExtractSubMesh(T3DImagery* img, int32_t objnum,
                    std::vector<SMeshVertex>& verts,
                    std::vector<uint16_t>&    indices);

// Extract one object's faces for a specific texture slot, matching the
// T3DImagery::RenderObject per-texture dispatch. Slot 0 is the untextured run.
bool ExtractSubMeshTextureSlot(T3DImagery* img, int32_t objnum, int32_t texslot,
                               std::vector<SMeshVertex>& verts,
                               std::vector<uint16_t>&    indices);

// Walk the state-0 (or given state) parent hierarchy and write the
// row-major 4x4 world matrix for `objnum` into `out16`. Safe to call on
// any valid sub-object index.
void BuildStaticObjectMatrix(T3DImagery* img, int32_t objnum,
                             int32_t state, int32_t frame,
                             float out16[16]);

// Same as BuildStaticObjectMatrix, but uses a live instance-style
// prevstate/prevframe pair so T3DImagery::GetAniKey can perform the
// normal state-transition interpolation path.
void BuildAnimatedObjectMatrix(T3DImagery* img, int32_t objnum,
                               int32_t state, int32_t frame,
                               int32_t prevstate, int32_t prevframe,
                               float out16[16]);

// Sample I3D object animation keys into the modern channel-pose model.
// Rotation keys are converted from legacy Euler XYZ to quaternion channels.
SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame);
SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame,
                            int32_t prevstate, int32_t prevframe);
SAnimPose BlendI3DAnimPoses(const SAnimPose& a, const SAnimPose& b, float t);

// Build a renderer row-major object matrix from a sampled pose. The hierarchy
// variant walks parent objects from the imagery before returning `objnum`.
void MatrixFromAnimPoseObject(const SAnimPose& pose, uint16_t target, float out16[16]);
void BuildAnimPoseObjectMatrix(T3DImagery* img, const SAnimPose& pose,
                               int32_t state, int32_t objnum, float out16[16]);

// Extract the imagery's ENTIRE global vert pool and face list as a single
// mesh, ignoring sub-object decomposition. Useful as a sanity check
// against per-sub-object extraction.
bool ExtractWholeMesh(T3DImagery* img,
                      std::vector<SMeshVertex>& verts,
                      std::vector<uint16_t>&    indices);
