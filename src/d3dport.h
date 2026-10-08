// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  d3dport.h - D3D-to-sokol shim for snapshot-style render code         *
// *************************************************************************
//
// PURPOSE
//   Lets bespoke effect ports transliterate the original 1998/99 D3D
//   render bodies near-verbatim. Each D3D-flavored call has a 1:1
//   `d3d::*` equivalent that hides the convention conversions our sokol
//   pipeline requires (column-vector matrices, helper-mesh queue scope,
//   degenerate-UV → WhiteTexture+material-color routing).
//
//   Extracted from TIceBoltEffect_Bespoke's proven-correct iter4d
//   (src/effect.cpp:7800-8480) and TMeteorStormEffect_Bespoke iter5
//   (src/effect.cpp:13965-14690). See docs/vfx/D3D_PORT_SHIM.md for
//   usage walkthrough.
//
// CONVENTIONS HIDDEN BY THE SHIM
//   1. Matrix accumulation. D3D row-vector `M *= S; M *= Rx; ... M *= T`
//      means vertex transformed by S first, then Rx, ..., then T. Our
//      shader (src/shaders/mesh.glsl.h:36-41) is column-vector
//      (`wp = M * ph`). The shim's MatrixScale/RotateX/RotateY/RotateZ/
//      Translate accumulate as if D3D — internally they pre-multiply
//      so the column-vector M still applies S first.
//
//   2. Rotation sign. RotateX(+π/2) in D3D right-hand math sends mesh
//      +Z → world -Y (matches snapshot effect_old.cpp:8595 and similar
//      `pos.y = -length*i` conventions). The shim preserves D3D signs.
//
//   3. Render queue scope. `d3d::RenderObject` routes Alpha and
//      Additive blends to SubmitHelperMesh — which the caller MUST
//      invoke from the harness `submit_world` callback (inside
//      BeginTilePass scope, see vfxtest.cpp:1515-1529). Calling from
//      the regular `submit` hook means BeginTilePass clears the
//      transparent_world_queue before your draws land → silently
//      invisible.
//
//   4. Degenerate UVs. Many I3D solid-fill sub-objects (cylinder
//      bodies, glow blobs) have all vertex UVs at (0,0). The shim's
//      RegisterSubMesh auto-binds the renderer's white texture for
//      those, so the fragment shader's `base = tex * diffuse` reduces
//      to `base = diffuse` = the I3D material color.
//
// WHAT THE SHIM DOES NOT HIDE
//   - WHICH sub-objects to draw (read recon).
//   - WHAT each sub-object does (read forensics + snapshot).
//   - Particle-bucket emission (use SubmitFxParticle/SubmitFxBillboard
//     directly).
//   - SubmitFxStrip for custom procedural strips (LightStrip family).
//
// *************************************************************************

#pragma once

#include <cstdint>

#include "renderer.h"  // TRenderer, SHelperMeshSubmit, MeshHandle, TTextureHandle
#include "3dimage.h"   // T3DImagery, S3DObj, S3DMat, S3DTex

struct S3DPoint;       // forward decl from gameplay headers

namespace d3d {

// --------------------------------------------------------------------------
// Matrix — row-major 4×4 storage. Translation in [3], [7], [11].
// Column-vector convention internally (wp = M * ph).
// All Matrix* mutators accumulate in D3D row-vector "M *= Op" semantics:
// after the call, the vertex transform sequence is "old ops, then Op",
// which on a column-vector multiplication means PRE-multiplying by Op.
// --------------------------------------------------------------------------
struct Matrix
{
    float m[16];
};

void MatrixIdentity(Matrix& out);

// D3D analogues: each accumulates in M *= Op fashion.
// After call: vertex sequence is "previous_ops, then Op".
void MatrixScale(Matrix& m, float sx, float sy, float sz);
void MatrixRotateX(Matrix& m, float radians);
void MatrixRotateY(Matrix& m, float radians);
void MatrixRotateZ(Matrix& m, float radians);
void MatrixTranslate(Matrix& m, float tx, float ty, float tz);
void MatrixTranslate(Matrix& m, const S3DPoint& p);

// out = a * b (column-vector form: vertex applied by b first, then a).
// Use to compose an instance-world matrix with a local-object matrix
// (`world = inst_world * local`).
void MatrixMultiply(Matrix& out, const Matrix& a, const Matrix& b);

// Build an instance-world matrix from a base position + facing rotation.
// Equivalent to: identity; RotateZ(face_rad); Translate(base_pos).
void MatrixWorldFromBase(Matrix& out, const S3DPoint& base_pos, float face_rad);

// --------------------------------------------------------------------------
// Blend modes — analogues of the snapshot's SetBlendState() /
// SetAddBlendState() calls.
// --------------------------------------------------------------------------
enum class BlendMode : uint8_t
{
    Alpha,      // SRC_ALPHA / INV_SRC_ALPHA — snapshot SetBlendState()
    Additive,   // ONE / ONE — snapshot SetAddBlendState()
};

// Stateful "current blend state" used by RenderObject calls. Mirrors the
// snapshot's render-state-stack pattern (SaveBlendState / SetBlendState /
// SetAddBlendState / RestoreBlendState). Default is Alpha (matches D3D's
// SRCALPHA/INVSRCALPHA default).
void SetBlendState();      // == snapshot SetBlendState()    → Alpha
void SetAddBlendState();   // == snapshot SetAddBlendState() → Additive
BlendMode CurrentBlend();

// RAII guard mirroring SaveBlendState() ... RestoreBlendState() — apply at
// the top of an effect's Render() body to scope mode changes.
struct BlendStateGuard
{
    BlendMode prev;
    BlendStateGuard();
    ~BlendStateGuard();
};

// --------------------------------------------------------------------------
// Obj — working state for one sub-object draw, mirroring retail's
// `obj` structure that the animator populates per draw.
//
// The `flags` field selects which transform pieces apply (matching
// snapshot OBJ3D_* bits). Renderer code reads these flags to know
// which fields are live.
// --------------------------------------------------------------------------
enum ObjFlags : uint32_t
{
    Flag_Matrix    = 0x01,   // OBJ3D_MATRIX  — obj.matrix is full transform
    Flag_AbsPos    = 0x02,   // OBJ3D_ABSPOS  — obj.pos is absolute world
    Flag_Verts     = 0x04,   // OBJ3D_VERTS   — use lverts[] for per-vert UVs
    Flag_Scl1      = 0x08,   // OBJ3D_SCL1    — apply scale FIRST
    Flag_Pos2      = 0x10,   // OBJ3D_POS2    — apply translate SECOND
    Flag_Pos1      = 0x20,   // OBJ3D_POS1    — apply translate FIRST
    Flag_Scl2      = 0x40,   // OBJ3D_SCL2    — apply scale SECOND
};

struct LightingVertex
{
    float tu = 0.0f;   // texture U coord (atlas cell selection)
    float tv = 0.0f;   // texture V coord
};

struct Obj
{
    uint32_t       flags = 0;
    Matrix         matrix;              // OBJ3D_MATRIX path
    float          pos[3] = {0,0,0};    // OBJ3D_POS*
    float          scl[3] = {1,1,1};    // OBJ3D_SCL*
    LightingVertex lverts[4];           // OBJ3D_VERTS: per-corner UVs

    // Sub-object identity (for diagnostics / texture binding).
    int32_t        objnum = -1;
    MeshHandle     mesh   = 0;          // pre-registered via RegisterSubMesh
    float          diffuse[4]  = {1,1,1,1};
    float          emissive[4] = {0,0,0,1};
};

// Build an identity Matrix in-place. Alias for MatrixIdentity that matches
// the snapshot's D3DMATRIXClear naming.
inline void MatrixClear(Matrix& m) { MatrixIdentity(m); }

// --------------------------------------------------------------------------
// Orientation — how the renderer interprets the obj's transform.
//   ScreenAligned: render as a camera-facing billboard (for sparkle/photon
//                  sub-objects authored as STILL flat quads).
//   WorldMesh:     render as a 3D mesh in world space (cylinders, rings,
//                  meteor cards on the ground, etc.).
// Set on the animator-equivalent state per effect; passed to RenderObject.
// --------------------------------------------------------------------------
enum class Orientation : uint8_t
{
    ScreenAligned,
    WorldMesh,
};

// --------------------------------------------------------------------------
// RenderObject — single-line analogue of D3D `animator->RenderObject(obj)`.
//
// Reads `obj.flags` to decide which transform pieces apply, composes the
// world matrix accordingly, and routes the submission:
//   - Orientation::WorldMesh → SubmitHelperMesh (current blend state)
//   - Orientation::ScreenAligned → SubmitFxBillboard
//                                  (uses obj.scl for size, obj.pos for
//                                   placement; lverts[] for atlas cell)
//
// The caller's blend mode comes from CurrentBlend(). For WorldMesh the
// inst_world is optional; for ScreenAligned the inst_world translation
// pre-shifts the billboard origin.
//
// MUST be called from the harness `submit_world` callback when the route
// is WorldMesh; ScreenAligned billboards work from either submit hook but
// submit_world is consistent.
// --------------------------------------------------------------------------
void RenderObject(TRenderer&  renderer,
                  const Obj&  obj,
                  Orientation orientation,
                  TTextureHandle texture,
                  const Matrix*  inst_world = nullptr);

// --------------------------------------------------------------------------
// RenderObjectSpinning — WorldXY-oriented textured billboard with per-
// instance in-plane rotation. Required for effects whose snapshot Render
// body builds OBJ3D_MATRIX with RotateX(-π/2) tip (WorldXY) plus a
// per-particle RotateZ (spin) that changes every tick.
//
// Routes to SubmitFxParticle with WorldXY orientation + rotation_rad.
// The position is inst_world * obj.pos (same as the ScreenAligned path).
// The size is obj.scl[0]/[1] (× base_size_wu is the caller's job — same
// gotcha #3 rule as ScreenAligned).
//
// Shim-extension added for X21 TFizzleEffect (forensics §7: OBJ3D_MATRIX
// path, per-particle rot.z spin, RotateX(-π/2) ground tip, RotateZ(-π/4)
// static spin).
// --------------------------------------------------------------------------
void RenderObjectSpinning(TRenderer&   renderer,
                          const Obj&   obj,
                          float        rotation_rad,
                          TTextureHandle texture,
                          const Matrix*  inst_world = nullptr);

// --------------------------------------------------------------------------
// Mesh registration helpers.
//
// RegisterSubMesh extracts an I3D sub-object as an SMeshVertex buffer +
// indices, registers it with the renderer, and handles the
// degenerate-UV solid-fill case by binding the renderer's white
// texture. Returns 0 (invalid handle) on failure.
//
// LoadMaterial extracts the diffuse + emissive color from the
// sub-object's S3DMat. Defaults to white diffuse / black emissive when
// material is absent.
// --------------------------------------------------------------------------
MeshHandle RegisterSubMesh(TRenderer& renderer,
                           T3DImagery* img3d,
                           int32_t sub_obj_num,
                           int32_t num_textures_in_imagery);

void LoadMaterial(T3DImagery* img3d,
                  int32_t sub_obj_num,
                  float diffuse_out[4],
                  float emissive_out[4]);

// --------------------------------------------------------------------------
// RenderObject — the 1:1 analogue of D3D `animator->RenderObject(obj)`.
//
// Submits one mesh draw via SubmitHelperMesh (alpha or additive
// transparent pipeline). MUST be called from the harness `submit_world`
// callback — calling from regular `submit` makes the draw invisible.
//
// `local` is the D3D-equivalent local matrix built via MatrixScale +
// MatrixRotate* + MatrixTranslate (accumulated in snapshot order).
// `inst_world` is the per-instance world transform (typically built
// once per tick via MatrixWorldFromBase). Pass nullptr if the local
// matrix is already in world space.
//
// `diffuse`/`emissive` come from LoadMaterial. RenderObject routes
// diffuse → SHelperMeshSubmit.diffuse and .ambient (so the mesh reads
// in unlit ambient too) and emissive → .emissive.
// --------------------------------------------------------------------------
void RenderObject(TRenderer& renderer,
                  MeshHandle mesh,
                  const Matrix& local,
                  const float diffuse[4],
                  const float emissive[4],
                  BlendMode blend,
                  const Matrix* inst_world = nullptr,
                  float sort_z = 0.0f);

} // namespace d3d
