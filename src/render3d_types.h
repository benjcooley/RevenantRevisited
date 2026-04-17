// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *  render3d_types.h - Modern replacements for the D3D3 type vocabulary  *
// *                                                                       *
// *  These types replace the Direct3D Immediate-Mode 3.0 surface that the *
// *  1998 renderer leaned on (D3DVERTEX / D3DPRIMITIVETYPE / execute      *
// *  buffers / material + texture handles). They are plain data so mesh   *
// *  loaders and cached animation data keep working; the rendering path   *
// *  translates them to sokol_gfx pipelines and bindings.                 *
// *************************************************************************

#pragma once

#include <cstdint>

#include <HandmadeMath.h>
#include <sokol_gfx.h>

// Primitive topology. Mirrors the subset of D3DPRIMITIVETYPE that Revenant
// actually uses — the mesh format only ever records triangle lists; line and
// point primitives appear in effect/debug draws.
enum class ERender3DPrim : uint8_t
{
    PointList      = 1,
    LineList       = 2,
    LineStrip      = 3,
    TriangleList   = 4,
    TriangleStrip  = 5,
    TriangleFan    = 6,
};

// Vertex layout selector. Revenant's D3D3 code carried three layouts in
// parallel (D3DVERTEX with normals for lit geometry, D3DLVERTEX with per-vert
// colors for pre-lit geometry, D3DTLVERTEX with screen-space positions for
// HUD / effects). Meshes embed which layout they want; the renderer picks
// the matching pipeline.
enum class ERender3DVertex : uint8_t
{
    Vertex      = 1,  // position + normal + uv (lit by dynamic lights)
    LitVertex   = 2,  // position + packed diffuse/specular + uv (pre-lit)
    TLVertex    = 3,  // screen-space position (x,y,z,rhw) + color + uv
};

// World-space lit-by-engine vertex.
struct S3DVertex
{
    hmm_vec3 pos;
    hmm_vec3 normal;
    float    tu, tv;
};

// World-space pre-lit vertex (colors baked in).
struct S3DLVertex
{
    hmm_vec3 pos;
    uint32_t diffuse;   // ARGB8
    uint32_t specular;  // ARGB8
    float    tu, tv;
};

// Screen-space transformed + lit vertex.
struct S3DTLVertex
{
    float    sx, sy, sz;  // screen position
    float    rhw;         // 1 / w (for perspective-correct interp)
    uint32_t diffuse;
    uint32_t specular;
    float    tu, tv;
};

// Render state keys that survived the port. The D3D3 enum had ~80 entries —
// most were driver-specific toggles we don't need under sokol. What's left
// is what the game code actually sets.
enum class ERender3DState : uint8_t
{
    Cull,           // 0 = none, 1 = cw, 2 = ccw
    ZEnable,        // 0 = off, 1 = on
    ZWriteEnable,
    ZFunc,          // LESS/LEQUAL/etc (see ERender3DCmpFunc)
    AlphaBlend,
    SrcBlend,
    DstBlend,
    AlphaTest,
    AlphaRef,
    AlphaFunc,
    Fog,
    FogColor,
    FogStart,
    FogEnd,
    TextureFilter,  // 0 = point, 1 = linear
    Dither,
    Wrap,
    TexAddress,     // 0 = wrap, 1 = clamp
};

enum class ERender3DCmpFunc : uint8_t
{
    Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always,
};

enum class ERender3DBlend : uint8_t
{
    Zero, One, SrcColor, InvSrcColor, SrcAlpha, InvSrcAlpha,
    DstColor, InvDstColor, DstAlpha, InvDstAlpha,
};

// Transform-stack slot (was D3DTRANSFORMSTATETYPE).
enum class ERender3DTransform : uint8_t
{
    World, View, Projection,
};

// Light-state key (was D3DLIGHTSTATETYPE). Only ambient + material survive;
// the rest was legacy fixed-function stuff.
enum class ERender3DLightState : uint8_t
{
    Material, Ambient,
};

// Integer handles issued by the renderer for materials/textures. We keep
// them opaque at the game layer; the renderer maps them to sg_image plus
// uniform slots.
using TMaterialHandle = uint32_t;
using TTextureHandle  = uint32_t;
static constexpr TMaterialHandle kInvalidMaterial = 0;
static constexpr TTextureHandle  kInvalidTexture  = 0;

// 4-int rectangle — replaces D3DRECT. Used by extents/update-rect code.
struct SRenderRect
{
    int32_t x1, y1, x2, y2;
};
