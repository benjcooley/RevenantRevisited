// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *       shaders.h  - Backend shader umbrella (pick MSL/GLSL/HLSL)       *
// *************************************************************************
//
// TRenderer sources its shader strings through this header. CMake sets
// one of SOKOL_METAL / SOKOL_GLCORE33 / SOKOL_GLES3 / SOKOL_D3D11 on the
// Revenant target; this umbrella translates that into a concrete bundle
// of backend-agnostic constants (kCompositeVs / kCompositeFs / kTileVs
// / kTileFs / kAOVs / kAOFs / kShadowVs / kShadowFs / kLightVs / kLightFs)
// that renderer.cpp
// consumes.
//
// Per-pass shader files live alongside this header -- one per backend:
//
//      composite.metal.h   composite.glsl.h   composite.hlsl.h
//      tile.metal.h        tile.glsl.h        tile.hlsl.h
//      ao.metal.h          ao.glsl.h          ao.hlsl.h
//      shadow.metal.h      shadow.glsl.h      shadow.hlsl.h
//      light.metal.h       light.glsl.h       light.hlsl.h
//
// Note: VS entry points differ across backends (_main on MSL, main on
// GLSL, main_vs on HLSL). Pipeline setup in renderer.cpp picks the
// right entry via SOKOL_<BACKEND> guards alongside the texture / uniform
// bindings that also differ per backend.
//
// *************************************************************************

#pragma once

#if defined(SOKOL_METAL)
    #include "composite.metal.h"
    #include "tile.metal.h"
    #include "mesh.metal.h"
    #include "ao.metal.h"
    #include "shadow.metal.h"
    #include "light.metal.h"

    inline constexpr const char* kCompositeVs = kCompositeVsMetal;
    inline constexpr const char* kCompositeFs = kCompositeFsMetal;
    inline constexpr const char* kTileVs      = kTileVsMetal;
    inline constexpr const char* kTileFs      = kTileFsMetal;
    inline constexpr const char* kMeshVs      = kMeshVsMetal;
    inline constexpr const char* kMeshFs      = kMeshFsMetal;
    inline constexpr const char* kAOVs        = kAOVsMetal;
    inline constexpr const char* kAOFs        = kAOFsMetal;
    inline constexpr const char* kShadowVs    = kShadowVsMetal;
    inline constexpr const char* kShadowFs    = kShadowFsMetal;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsMetal;
    inline constexpr const char* kLightVs     = kLightVsMetal;
    inline constexpr const char* kLightFs     = kLightFsMetal;
    inline constexpr const char* kShaderVsEntry = "_main";
    inline constexpr const char* kShaderFsEntry = "_main";
#elif defined(SOKOL_GLCORE33) || defined(SOKOL_GLES3)
    #include "composite.glsl.h"
    #include "tile.glsl.h"
    #include "mesh.glsl.h"
    #include "ao.glsl.h"
    #include "shadow.glsl.h"
    #include "light.glsl.h"

    inline constexpr const char* kCompositeVs = kCompositeVsGlsl;
    inline constexpr const char* kCompositeFs = kCompositeFsGlsl;
    inline constexpr const char* kTileVs      = kTileVsGlsl;
    inline constexpr const char* kTileFs      = kTileFsGlsl;
    inline constexpr const char* kMeshVs      = kMeshVsGlsl;
    inline constexpr const char* kMeshFs      = kMeshFsGlsl;
    inline constexpr const char* kAOVs        = kAOVsGlsl;
    inline constexpr const char* kAOFs        = kAOFsGlsl;
    inline constexpr const char* kShadowVs    = kShadowVsGlsl;
    inline constexpr const char* kShadowFs    = kShadowFsGlsl;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsGlsl;
    inline constexpr const char* kLightVs     = kLightVsGlsl;
    inline constexpr const char* kLightFs     = kLightFsGlsl;
    inline constexpr const char* kShaderVsEntry = "main";
    inline constexpr const char* kShaderFsEntry = "main";
#elif defined(SOKOL_D3D11)
    #include "composite.hlsl.h"
    #include "tile.hlsl.h"
    #include "mesh.hlsl.h"
    #include "ao.hlsl.h"
    #include "shadow.hlsl.h"
    #include "light.hlsl.h"

    inline constexpr const char* kCompositeVs = kCompositeVsHlsl;
    inline constexpr const char* kCompositeFs = kCompositeFsHlsl;
    inline constexpr const char* kTileVs      = kTileVsHlsl;
    inline constexpr const char* kTileFs      = kTileFsHlsl;
    inline constexpr const char* kMeshVs      = kMeshVsHlsl;
    inline constexpr const char* kMeshFs      = kMeshFsHlsl;
    inline constexpr const char* kAOVs        = kAOVsHlsl;
    inline constexpr const char* kAOFs        = kAOFsHlsl;
    inline constexpr const char* kShadowVs    = kShadowVsHlsl;
    inline constexpr const char* kShadowFs    = kShadowFsHlsl;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsHlsl;
    inline constexpr const char* kLightVs     = kLightVsHlsl;
    inline constexpr const char* kLightFs     = kLightFsHlsl;
    inline constexpr const char* kShaderVsEntry = "main_vs";
    inline constexpr const char* kShaderFsEntry = "main_ps";
#else
    #error "shaders.h: no SOKOL_<BACKEND> macro defined -- check CMake"
#endif
