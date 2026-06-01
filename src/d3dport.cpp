// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  d3dport.cpp - implementation of the D3D-to-sokol shim                *
// *************************************************************************
//
// See d3dport.h for design rationale + convention notes.
// Reference impls extracted from:
//   - src/effect.cpp:7800-8480 (TIceBoltEffect_Bespoke iter4d)
//   - src/effect.cpp:13965-14690 (TMeteorStormEffect_Bespoke iter5)
//
// *************************************************************************

#include "d3dport.h"

#include <cmath>
#include <cstring>
#include <vector>

#include "logging.h"
#include "math3d.h"      // S3DPoint (shared with effect.cpp)

// ExtractSubMeshTextureSlot lives in 3dimage.cpp's translation unit but
// is declared globally; mirror the declaration here without dragging in
// a heavy header.
struct SMeshVertex;
extern bool ExtractSubMeshTextureSlot(T3DImagery* img3d,
                                      int32_t obj_num,
                                      int32_t tex_slot,
                                      std::vector<SMeshVertex>& verts_out,
                                      std::vector<uint16_t>& indices_out);

namespace d3d {

// --------------------------------------------------------------------------
// Matrix primitives. Storage is row-major 4×4 with translation in
// m[3], m[7], m[11]. Column-vector convention: vertex transformed as
// wp = M * vp.
// --------------------------------------------------------------------------

static void mat_identity_impl(float m[16])
{
    for (int32_t i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
}

static void mat_mul_impl(const float a[16], const float b[16], float out[16])
{
    float tmp[16];
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t k = 0; k < 4; ++k)
                s += a[r * 4 + k] * b[k * 4 + c];
            tmp[r * 4 + c] = s;
        }
    std::memcpy(out, tmp, sizeof(tmp));
}

void MatrixIdentity(Matrix& out)
{
    mat_identity_impl(out.m);
}

// D3D-flavored accumulators. Each Op is "applied AFTER existing M" in
// vertex-transform order. Under column-vector convention that means:
//   M_new = Op * M_old
void MatrixScale(Matrix& m, float sx, float sy, float sz)
{
    float S[16]; mat_identity_impl(S);
    S[0]  = sx;
    S[5]  = sy;
    S[10] = sz;
    float out[16];
    mat_mul_impl(S, m.m, out);
    std::memcpy(m.m, out, sizeof(out));
}

void MatrixRotateX(Matrix& m, float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    float R[16]; mat_identity_impl(R);
    R[5]  =  c; R[6]  = -s;
    R[9]  =  s; R[10] =  c;
    float out[16];
    mat_mul_impl(R, m.m, out);
    std::memcpy(m.m, out, sizeof(out));
}

void MatrixRotateY(Matrix& m, float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    float R[16]; mat_identity_impl(R);
    R[0]  =  c; R[2]  =  s;
    R[8]  = -s; R[10] =  c;
    float out[16];
    mat_mul_impl(R, m.m, out);
    std::memcpy(m.m, out, sizeof(out));
}

void MatrixRotateZ(Matrix& m, float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    float R[16]; mat_identity_impl(R);
    R[0]  =  c; R[1]  = -s;
    R[4]  =  s; R[5]  =  c;
    float out[16];
    mat_mul_impl(R, m.m, out);
    std::memcpy(m.m, out, sizeof(out));
}

void MatrixTranslate(Matrix& m, float tx, float ty, float tz)
{
    float T[16]; mat_identity_impl(T);
    T[3]  = tx;
    T[7]  = ty;
    T[11] = tz;
    float out[16];
    mat_mul_impl(T, m.m, out);
    std::memcpy(m.m, out, sizeof(out));
}

void MatrixTranslate(Matrix& m, const S3DPoint& p)
{
    MatrixTranslate(m, float(p.x), float(p.y), float(p.z));
}

void MatrixMultiply(Matrix& out, const Matrix& a, const Matrix& b)
{
    mat_mul_impl(a.m, b.m, out.m);
}

void MatrixWorldFromBase(Matrix& out, const S3DPoint& base_pos, float face_rad)
{
    const float c = std::cos(face_rad);
    const float s = std::sin(face_rad);
    out.m[0]  = c;    out.m[1]  = -s;   out.m[2]  = 0.0f; out.m[3]  = float(base_pos.x);
    out.m[4]  = s;    out.m[5]  =  c;   out.m[6]  = 0.0f; out.m[7]  = float(base_pos.y);
    out.m[8]  = 0.0f; out.m[9]  = 0.0f; out.m[10] = 1.0f; out.m[11] = float(base_pos.z);
    out.m[12] = 0.0f; out.m[13] = 0.0f; out.m[14] = 0.0f; out.m[15] = 1.0f;
}

// --------------------------------------------------------------------------
// Mesh registration. Walks tex slots looking for the one with non-empty
// vert/index data; for degenerate-UV sub-objects (cylinder bodies,
// glow blobs), texslot 0 (untextured) wins and we bind WhiteTexture.
// --------------------------------------------------------------------------
MeshHandle RegisterSubMesh(TRenderer& renderer,
                           T3DImagery* img3d,
                           int32_t sub_obj_num,
                           int32_t num_textures_in_imagery)
{
    if (!img3d || sub_obj_num < 0) return 0;
    const int32_t texslots = num_textures_in_imagery + 1;  // +1 for slot 0 (untextured)
    for (int32_t texslot = 0; texslot < texslots; ++texslot)
    {
        std::vector<SMeshVertex> verts;
        std::vector<uint16_t>    indices;
        if (!ExtractSubMeshTextureSlot(img3d, sub_obj_num, texslot, verts, indices))
            continue;
        if (verts.empty() || indices.empty()) continue;
        // Pick texture: slot 0 = untextured / solid-fill (degenerate UVs
        // expected → bind white texture). slot N>0 = imagery texture N-1.
        TTextureHandle albedo = renderer.WhiteTextureHandle();
        if (texslot > 0 && texslot - 1 < num_textures_in_imagery)
        {
            S3DTex t = {};
            img3d->GetTexture(texslot - 1, &t);
            if (t.htexture != kInvalidTexture) albedo = t.htexture;
        }
        return renderer.RegisterMesh(verts.data(), int32_t(verts.size()),
                                     indices.data(), int32_t(indices.size()),
                                     albedo);
    }
    return 0;
}

void LoadMaterial(T3DImagery* img3d,
                  int32_t sub_obj_num,
                  float diffuse_out[4],
                  float emissive_out[4])
{
    diffuse_out[0]  = diffuse_out[1]  = diffuse_out[2]  = 1.0f;
    diffuse_out[3]  = 1.0f;
    emissive_out[0] = emissive_out[1] = emissive_out[2] = 0.0f;
    emissive_out[3] = 1.0f;
    if (!img3d || sub_obj_num < 0) return;
    S3DObj o = {};
    img3d->GetObject(sub_obj_num, &o);
    if (o.material < 0 || o.material >= img3d->NumMaterials()) return;
    S3DMat m = {};
    img3d->GetMaterial(o.material, &m);
    diffuse_out[0]  = m.matdesc.diffuse.r;
    diffuse_out[1]  = m.matdesc.diffuse.g;
    diffuse_out[2]  = m.matdesc.diffuse.b;
    diffuse_out[3]  = m.matdesc.diffuse.a > 0.001f ? m.matdesc.diffuse.a : 1.0f;
    emissive_out[0] = m.matdesc.emissive.r;
    emissive_out[1] = m.matdesc.emissive.g;
    emissive_out[2] = m.matdesc.emissive.b;
}

// --------------------------------------------------------------------------
// RenderObject — single mesh draw via SubmitHelperMesh. Must be called
// from the harness submit_world callback (inside BeginTilePass scope).
// --------------------------------------------------------------------------
void RenderObject(TRenderer& renderer,
                  MeshHandle mesh,
                  const Matrix& local,
                  const float diffuse[4],
                  const float emissive[4],
                  BlendMode blend,
                  const Matrix* inst_world,
                  float sort_z)
{
    if (mesh == 0) return;
    float world[16];
    if (inst_world)
        mat_mul_impl(inst_world->m, local.m, world);
    else
        std::memcpy(world, local.m, sizeof(world));

    SHelperMeshSubmit m = {};
    m.mesh           = mesh;
    m.additive_blend = (blend == BlendMode::Additive);
    m.shadow_plane   = false;
    std::memcpy(m.world, world, sizeof(world));
    // Route diffuse → both diffuse and ambient so the mesh reads in
    // unlit ambient too (icebolt iter4d pattern).
    std::memcpy(m.diffuse, diffuse, 4 * sizeof(float));
    std::memcpy(m.ambient, diffuse, 4 * sizeof(float));
    m.specular[0] = m.specular[1] = m.specular[2] = m.specular[3] = 0.0f;
    std::memcpy(m.emissive, emissive, 4 * sizeof(float));
    m.power      = 1.0f;
    m.sort_depth = sort_z;
    renderer.SubmitHelperMesh(m);
}

// --------------------------------------------------------------------------
// Stateful blend mode — mirrors snapshot SetBlendState/SaveBlendState.
// Thread-local so concurrent test agents on the same binary don't race.
// --------------------------------------------------------------------------
static thread_local BlendMode g_current_blend = BlendMode::Alpha;

void SetBlendState()       { g_current_blend = BlendMode::Alpha;    }
void SetAddBlendState()    { g_current_blend = BlendMode::Additive; }
BlendMode CurrentBlend()   { return g_current_blend; }

BlendStateGuard::BlendStateGuard()  : prev(g_current_blend) {}
BlendStateGuard::~BlendStateGuard() { g_current_blend = prev; }

// --------------------------------------------------------------------------
// RenderObject — the snapshot's `animator->RenderObject(obj)` analogue.
// Reads obj.flags to compose the per-draw matrix; dispatches by
// orientation.
// --------------------------------------------------------------------------
void RenderObject(TRenderer&  renderer,
                  const Obj&  obj,
                  Orientation orientation,
                  TTextureHandle texture,
                  const Matrix*  inst_world)
{
    if (orientation == Orientation::ScreenAligned)
    {
        // Snapshot OBJ3D_SCL1 | OBJ3D_POS2 path: scale applied first,
        // then translate. For a screen-aligned billboard this collapses
        // to: size = scl, position = inst_world * pos.
        float wp[3];
        if (inst_world)
        {
            // inst_world * pos (column-vector multiply, taking pos as
            // a translation point with w=1).
            for (int32_t r = 0; r < 3; ++r)
            {
                wp[r] = inst_world->m[r * 4 + 0] * obj.pos[0]
                      + inst_world->m[r * 4 + 1] * obj.pos[1]
                      + inst_world->m[r * 4 + 2] * obj.pos[2]
                      + inst_world->m[r * 4 + 3];
            }
        }
        else
        {
            wp[0] = obj.pos[0];
            wp[1] = obj.pos[1];
            wp[2] = obj.pos[2];
        }

        // Size: average of scl.x/y for a billboard (the snapshot writes
        // uniform scl on all three, so this is equivalent to scl.x).
        // size_wu is half-extent; multiply by 2 to match the snapshot's
        // "obj->scl is the half-extent in mesh-local units" convention.
        // We treat obj.scl as a uniform multiplier on the mesh's authored
        // half-extent — which for billboards in this codebase is usually
        // ~1 wu. So size_wu = scl directly.
        SBillboardDrawItem item = {};
        item.world_pos[0] = wp[0];
        item.world_pos[1] = wp[1];
        item.world_pos[2] = wp[2];
        item.size_wu[0]   = obj.scl[0];
        item.size_wu[1]   = obj.scl[1];

        // Per-vertex UV — if OBJ3D_VERTS is set, the lverts[] specify
        // the atlas-cell sub-rect (D3D's per-vertex `tu/tv` writes).
        // Snapshot order is lverts[0..3] = TL/BL/TR/BR (per the
        // TStormAnimator example, effectcomp.cpp:265-275).
        if (obj.flags & Flag_Verts)
        {
            const float u_lo = obj.lverts[0].tu;
            const float v_lo = obj.lverts[0].tv;
            const float u_hi = obj.lverts[3].tu;
            const float v_hi = obj.lverts[3].tv;
            item.uv_rect[0] = u_lo;
            item.uv_rect[1] = v_lo;
            item.uv_rect[2] = u_hi - u_lo;
            item.uv_rect[3] = v_hi - v_lo;
        }
        // else item.uv_rect stays at the default (0,0,1,1) full quad.

        item.color_rgba[0] = obj.diffuse[0];
        item.color_rgba[1] = obj.diffuse[1];
        item.color_rgba[2] = obj.diffuse[2];
        item.color_rgba[3] = obj.diffuse[3];

        item.key.texture     = texture;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(
            g_current_blend == BlendMode::Additive
                ? EFxBlend::AdditiveStraight
                : EFxBlend::Alpha);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = EFxDebugMode::Normal;
        renderer.SubmitFxBillboard(item);
        return;
    }

    // WorldMesh path: full mesh-submit via SubmitHelperMesh. Compose the
    // matrix from obj.flags + obj.scl + obj.pos + obj.matrix.
    Matrix local;
    if (obj.flags & Flag_Matrix)
    {
        local = obj.matrix;
    }
    else
    {
        MatrixIdentity(local);
        // POS1 → translate first
        if (obj.flags & Flag_Pos1)
            MatrixTranslate(local, obj.pos[0], obj.pos[1], obj.pos[2]);
        // SCL1 → scale first (or after POS1)
        if (obj.flags & Flag_Scl1)
            MatrixScale(local, obj.scl[0], obj.scl[1], obj.scl[2]);
        // POS2 → translate second
        if (obj.flags & Flag_Pos2)
            MatrixTranslate(local, obj.pos[0], obj.pos[1], obj.pos[2]);
        // SCL2 → scale second
        if (obj.flags & Flag_Scl2)
            MatrixScale(local, obj.scl[0], obj.scl[1], obj.scl[2]);
    }

    // Delegate to the existing mesh-submit overload.
    RenderObject(renderer, obj.mesh, local, obj.diffuse, obj.emissive,
                 g_current_blend, inst_world, 0.0f);
}

} // namespace d3d
