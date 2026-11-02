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
// / kTileFs / kAOVs / kAOFs / kShadowVs / kShadowFs / kLightVs / kLightFs
// / ...) that renderer.cpp consumes.
//
// kLightModel is a fragment-shader prefix, not a shader: the lighting
// model both lit passes share. TRenderer prepends it to kLightFs and
// kMeshTranslucentFs when it builds those shaders.
//
// Per-pass shader files live alongside this header -- one per backend:
//
//      composite.metal.h   composite.glsl.h   composite.hlsl.h
//      tile.metal.h        tile.glsl.h        tile.hlsl.h
//      ao.metal.h          ao.glsl.h          ao.hlsl.h
//      shadow.metal.h      shadow.glsl.h      shadow.hlsl.h
//      lightmodel.metal.h  lightmodel.glsl.h  lightmodel.hlsl.h
//      light.metal.h       light.glsl.h       light.hlsl.h
//      mesh.metal.h        mesh.glsl.h        mesh.hlsl.h
//
// Note: VS entry points differ across backends (_main on MSL, main on
// GLSL, main_vs on HLSL). Pipeline setup in renderer.cpp picks the
// right entry via SOKOL_<BACKEND> guards alongside the texture / uniform
// bindings that also differ per backend.
//
// *************************************************************************

#pragma once

#if defined(SOKOL_METAL)
    #include "engine/render/shaders/composite.metal.h"
    #include "engine/render/shaders/tile.metal.h"
    #include "engine/render/shaders/mesh.metal.h"
    #include "engine/render/shaders/ao.metal.h"
    #include "engine/render/shaders/shadow.metal.h"
    #include "engine/render/shaders/lightmodel.metal.h"
    #include "engine/render/shaders/light.metal.h"
    #include "engine/render/shaders/fx.metal.h"

    inline constexpr const char* kCompositeVs = kCompositeVsMetal;
    inline constexpr const char* kCompositeFs = kCompositeFsMetal;
    inline constexpr const char* kCompositeReduceFs = kCompositeReduceFsMetal;
    inline constexpr const char* kColorMeshVs = kColorMeshVsMetal;
    inline constexpr const char* kColorMeshFs = kColorMeshFsMetal;
    inline constexpr const char* kTileVs      = kTileVsMetal;
    inline constexpr const char* kTileFs      = kTileFsMetal;
    inline constexpr const char* kMeshVs      = kMeshVsMetal;
    inline constexpr const char* kMeshFs      = kMeshFsMetal;
    inline constexpr const char* kMeshDepthFs = kMeshDepthFsMetal;
    inline constexpr const char* kMeshTranslucentFs = kMeshTranslucentFsMetal;
    inline constexpr const char* kAOVs        = kAOVsMetal;
    inline constexpr const char* kAOFs        = kAOFsMetal;
    inline constexpr const char* kShadowVs    = kShadowVsMetal;
    inline constexpr const char* kShadowFs    = kShadowFsMetal;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsMetal;
    inline constexpr const char* kLightVs     = kLightVsMetal;
    inline constexpr const char* kLightModel  = kLightModelMetal;
    inline constexpr const char* kLightFs     = kLightFsMetal;
    inline constexpr const char* kFxBillboardVs = kFxBillboardVsMetal;
    inline constexpr const char* kFxBillboardFs = kFxBillboardFsMetal;
    inline constexpr const char* kFxParticleVs  = kFxParticleVsMetal;
    inline constexpr const char* kFxParticleFs  = kFxParticleFsMetal;
    inline constexpr const char* kFxStripVs     = kFxStripVsMetal;
    inline constexpr const char* kFxStripFs     = kFxStripFsMetal;
    inline constexpr const char* kShaderVsEntry = "_main";
    inline constexpr const char* kShaderFsEntry = "_main";
#elif defined(SOKOL_GLCORE33) || defined(SOKOL_GLES3)
    #include "engine/render/shaders/composite.glsl.h"
    #include "engine/render/shaders/tile.glsl.h"
    #include "engine/render/shaders/mesh.glsl.h"
    #include "engine/render/shaders/ao.glsl.h"
    #include "engine/render/shaders/shadow.glsl.h"
    #include "engine/render/shaders/lightmodel.glsl.h"
    #include "engine/render/shaders/light.glsl.h"
    #include "engine/render/shaders/fx.glsl.h"

    inline constexpr const char* kCompositeVs = kCompositeVsGlsl;
    inline constexpr const char* kCompositeFs = kCompositeFsGlsl;
    inline constexpr const char* kCompositeReduceFs = kCompositeReduceFsGlsl;
    inline constexpr const char* kColorMeshVs = kColorMeshVsGlsl;
    inline constexpr const char* kColorMeshFs = kColorMeshFsGlsl;
    inline constexpr const char* kTileVs      = kTileVsGlsl;
    inline constexpr const char* kTileFs      = kTileFsGlsl;
    inline constexpr const char* kMeshVs      = kMeshVsGlsl;
    inline constexpr const char* kMeshFs      = kMeshFsGlsl;
    inline constexpr const char* kMeshDepthFs = kMeshDepthFsGlsl;
    inline constexpr const char* kMeshTranslucentFs = kMeshTranslucentFsGlsl;
    inline constexpr const char* kAOVs        = kAOVsGlsl;
    inline constexpr const char* kAOFs        = kAOFsGlsl;
    inline constexpr const char* kShadowVs    = kShadowVsGlsl;
    inline constexpr const char* kShadowFs    = kShadowFsGlsl;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsGlsl;
    inline constexpr const char* kLightVs     = kLightVsGlsl;
    inline constexpr const char* kLightModel  = kLightModelGlsl;
    inline constexpr const char* kLightFs     = kLightFsGlsl;
    inline constexpr const char* kFxBillboardVs = kFxBillboardVsGlsl;
    inline constexpr const char* kFxBillboardFs = kFxBillboardFsGlsl;
    inline constexpr const char* kFxParticleVs  = kFxParticleVsGlsl;
    inline constexpr const char* kFxParticleFs  = kFxParticleFsGlsl;
    inline constexpr const char* kFxStripVs     = kFxStripVsGlsl;
    inline constexpr const char* kFxStripFs     = kFxStripFsGlsl;
    inline constexpr const char* kShaderVsEntry = "main";
    inline constexpr const char* kShaderFsEntry = "main";
#elif defined(SOKOL_D3D11)
    #include "engine/render/shaders/composite.hlsl.h"
    #include "engine/render/shaders/tile.hlsl.h"
    #include "engine/render/shaders/mesh.hlsl.h"
    #include "engine/render/shaders/ao.hlsl.h"
    #include "engine/render/shaders/shadow.hlsl.h"
    #include "engine/render/shaders/lightmodel.hlsl.h"
    #include "engine/render/shaders/light.hlsl.h"
    #include "engine/render/shaders/fx.hlsl.h"

    inline constexpr const char* kCompositeVs = kCompositeVsHlsl;
    inline constexpr const char* kCompositeFs = kCompositeFsHlsl;
    inline constexpr const char* kCompositeReduceFs = kCompositeReduceFsHlsl;
    inline constexpr const char* kColorMeshVs = kColorMeshVsHlsl;
    inline constexpr const char* kColorMeshFs = kColorMeshFsHlsl;
    inline constexpr const char* kTileVs      = kTileVsHlsl;
    inline constexpr const char* kTileFs      = kTileFsHlsl;
    inline constexpr const char* kMeshVs      = kMeshVsHlsl;
    inline constexpr const char* kMeshFs      = kMeshFsHlsl;
    inline constexpr const char* kMeshDepthFs = kMeshDepthFsHlsl;
    inline constexpr const char* kMeshTranslucentFs = kMeshTranslucentFsHlsl;
    inline constexpr const char* kAOVs        = kAOVsHlsl;
    inline constexpr const char* kAOFs        = kAOFsHlsl;
    inline constexpr const char* kShadowVs    = kShadowVsHlsl;
    inline constexpr const char* kShadowFs    = kShadowFsHlsl;
    inline constexpr const char* kShadowBlurFs = kShadowBlurFsHlsl;
    inline constexpr const char* kLightVs     = kLightVsHlsl;
    inline constexpr const char* kLightModel  = kLightModelHlsl;
    inline constexpr const char* kLightFs     = kLightFsHlsl;
    inline constexpr const char* kFxBillboardVs = kFxBillboardVsHlsl;
    inline constexpr const char* kFxBillboardFs = kFxBillboardFsHlsl;
    inline constexpr const char* kFxParticleVs  = kFxParticleVsHlsl;
    inline constexpr const char* kFxParticleFs  = kFxParticleFsHlsl;
    inline constexpr const char* kFxStripVs     = kFxStripVsHlsl;
    inline constexpr const char* kFxStripFs     = kFxStripFsHlsl;
    inline constexpr const char* kShaderVsEntry = "main_vs";
    inline constexpr const char* kShaderFsEntry = "main_ps";
#else
    #error "shaders.h: no SOKOL_<BACKEND> macro defined -- check CMake"
#endif
