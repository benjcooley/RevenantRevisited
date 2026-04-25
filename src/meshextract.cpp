// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *      meshextract.cpp - Convert T3DImagery sub-objects to mesh data    *
// *************************************************************************

#include "meshextract.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include <HandmadeMath.h>

#include "3dimage.h"
#include "math3d.h"
#include "render3d_types.h"

static_assert(sizeof(hmm_vec3)   == 12, "hmm_vec3 must be tight 3-float");
static_assert(sizeof(S3DVertex)  == 32, "S3DVertex must match retail vertex size");
static_assert(sizeof(SMeshVertex) == 32, "SMeshVertex must be 32 bytes");
static_assert(sizeof(S3DFace)    == 6,  "S3DFace must be 6 bytes (3x uint16)");

namespace {

SAnimQuat QuatFromEulerXYZ(float x, float y, float z)
{
    const float cx = std::cos(x * 0.5f), sx = std::sin(x * 0.5f);
    const float cy = std::cos(y * 0.5f), sy = std::sin(y * 0.5f);
    const float cz = std::cos(z * 0.5f), sz = std::sin(z * 0.5f);

    SAnimQuat qx = { sx, 0.0f, 0.0f, cx };
    SAnimQuat qy = { 0.0f, sy, 0.0f, cy };
    SAnimQuat qz = { 0.0f, 0.0f, sz, cz };
    auto mul = [](const SAnimQuat& a, const SAnimQuat& b) -> SAnimQuat {
        return {
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        };
    };
    return NormalizeQuat(mul(mul(qz, qy), qx));
}

void MatrixMulLocal(const float a[16], const float b[16], float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
    {
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t i = 0; i < 4; ++i)
                s += a[r * 4 + i] * b[i * 4 + c];
            out[r * 4 + c] = s;
        }
    }
}

} // namespace

bool ExtractSubMesh(T3DImagery* img, int32_t objnum,
                    std::vector<SMeshVertex>& verts,
                    std::vector<uint16_t>&    indices)
{
    if (!img) return false;

    const int32_t num_obj_verts = img->NumObjVerts(objnum);
    const int32_t num_obj_faces = img->NumObjFaces(objnum);
    if (num_obj_verts <= 0 || num_obj_faces <= 0) return false;

    std::vector<S3DVertex> src(num_obj_verts);
    img->GetObjVerts(objnum, src.data(), 0, 0, ERender3DVertex::Vertex);

    // Faces are repacked by GetObjFaces() into one dense object-local buffer.
    std::vector<S3DFace> faces(num_obj_faces);
    img->GetObjFaces(objnum, faces.data(), nullptr, nullptr);

    verts.assign(num_obj_verts, SMeshVertex{});
    for (int32_t i = 0; i < num_obj_verts; ++i) {
        verts[i].pos[0]    = src[i].pos.X;
        verts[i].pos[1]    = src[i].pos.Y;
        verts[i].pos[2]    = src[i].pos.Z;
        verts[i].normal[0] = src[i].normal.X;
        verts[i].normal[1] = src[i].normal.Y;
        verts[i].normal[2] = src[i].normal.Z;
        verts[i].uv[0]     = src[i].tu;
        verts[i].uv[1]     = src[i].tv;
    }

    indices.clear();
    indices.reserve(size_t(num_obj_faces) * 3);

    for (const auto& f : faces) {
        int32_t i0 = int32_t(f.v1);
        int32_t i1 = int32_t(f.v2);
        int32_t i2 = int32_t(f.v3);
        if (i0 < 0 || i0 >= num_obj_verts) continue;
        if (i1 < 0 || i1 >= num_obj_verts) continue;
        if (i2 < 0 || i2 >= num_obj_verts) continue;

        indices.push_back(uint16_t(i0));
        indices.push_back(uint16_t(i1));
        indices.push_back(uint16_t(i2));
    }

    for (auto& v : verts) {
        float nx = v.normal[0], ny = v.normal[1], nz = v.normal[2];
        float len2 = nx * nx + ny * ny + nz * nz;
        if (len2 > 1e-12f) {
            float inv = 1.0f / std::sqrt(len2);
            v.normal[0] = nx * inv; v.normal[1] = ny * inv; v.normal[2] = nz * inv;
        } else {
            v.normal[0] = 0.0f; v.normal[1] = 0.0f; v.normal[2] = 1.0f;
        }
    }

    return !indices.empty();
}

bool ExtractSubMeshTextureSlot(T3DImagery* img, int32_t objnum, int32_t texslot,
                               std::vector<SMeshVertex>& verts,
                               std::vector<uint16_t>&    indices)
{
    if (!img) return false;
    if (img->NumTextures() == 0 && texslot == 0)
        return ExtractSubMesh(img, objnum, verts, indices);

    const int32_t num_obj_verts = img->NumObjVerts(objnum);
    const int32_t num_obj_faces = img->NumObjFaces(objnum);
    if (num_obj_verts <= 0 || num_obj_faces <= 0) return false;

    std::vector<S3DVertex> src(num_obj_verts);
    img->GetObjVerts(objnum, src.data(), 0, 0, ERender3DVertex::Vertex);

    std::vector<S3DFace> faces(num_obj_faces);
    int32_t texfaces[MAXTEXTURES + 1] = {};
    int32_t numtexfaces[MAXTEXTURES + 1] = {};
    img->GetObjFaces(objnum, faces.data(), texfaces, numtexfaces);

    if ((uint32_t)texslot > (uint32_t)MAXTEXTURES) return false;
    const int32_t start = texfaces[texslot];
    const int32_t count = numtexfaces[texslot];
    if (count <= 0 || start < 0 || start + count > num_obj_faces) return false;

    verts.assign(num_obj_verts, SMeshVertex{});
    for (int32_t i = 0; i < num_obj_verts; ++i) {
        verts[i].pos[0]    = src[i].pos.X;
        verts[i].pos[1]    = src[i].pos.Y;
        verts[i].pos[2]    = src[i].pos.Z;
        verts[i].normal[0] = src[i].normal.X;
        verts[i].normal[1] = src[i].normal.Y;
        verts[i].normal[2] = src[i].normal.Z;
        verts[i].uv[0]     = src[i].tu;
        verts[i].uv[1]     = src[i].tv;
    }

    indices.clear();
    indices.reserve(size_t(count) * 3);
    for (int32_t fidx = 0; fidx < count; ++fidx) {
        const S3DFace& f = faces[start + fidx];
        int32_t i0 = int32_t(f.v1);
        int32_t i1 = int32_t(f.v2);
        int32_t i2 = int32_t(f.v3);
        if (i0 < 0 || i0 >= num_obj_verts) continue;
        if (i1 < 0 || i1 >= num_obj_verts) continue;
        if (i2 < 0 || i2 >= num_obj_verts) continue;
        indices.push_back(uint16_t(i0));
        indices.push_back(uint16_t(i1));
        indices.push_back(uint16_t(i2));
    }

    return !indices.empty();
}

// Mirror of the static MakeMatrix() in 3dimage.cpp. Mtx* stack into `m`
// which is initialized to identity before this call.
static void BuildLocalMatrix(hmm_mat4& m, hmm_vec3& pos, hmm_vec3& rot, hmm_vec3& scl)
{
    if (rot.X != 0.0f) MtxRotateX(&m, rot.X);
    if (rot.Y != 0.0f) MtxRotateY(&m, rot.Y);
    if (rot.Z != 0.0f) MtxRotateZ(&m, rot.Z);
    if (pos.X != 0.0f || pos.Y != 0.0f || pos.Z != 0.0f)
        MtxTranslate(&m, &pos);
    if (scl.X != 1.0f || scl.Y != 1.0f || scl.Z != 1.0f)
        MtxScale(&m, &scl);
}

bool ExtractWholeMesh(T3DImagery* img,
                      std::vector<SMeshVertex>& verts,
                      std::vector<uint16_t>&    indices)
{
    if (!img) return false;

    const int32_t nv = img->NumVerts();
    const int32_t nf = img->NumFaces();
    if (nv <= 0 || nf <= 0) return false;

    std::vector<S3DVertex> src(nv);
    img->GetVerts(src.data(), 0, 0, ERender3DVertex::Vertex, 0, nv);

    std::vector<S3DFace> faces(nf);
    img->GetFaces(faces.data());

    verts.assign(nv, SMeshVertex{});
    for (int32_t i = 0; i < nv; ++i) {
        verts[i].pos[0] = src[i].pos.X;
        verts[i].pos[1] = src[i].pos.Y;
        verts[i].pos[2] = src[i].pos.Z;
        verts[i].normal[0] = src[i].normal.X;
        verts[i].normal[1] = src[i].normal.Y;
        verts[i].normal[2] = src[i].normal.Z;
        verts[i].uv[0] = src[i].tu;
        verts[i].uv[1] = src[i].tv;
    }

    indices.clear();
    indices.reserve(size_t(nf) * 3);

    // The stored face indices are local to each sub-object. T3DImagery renders
    // them against a vertex pointer already advanced by obj.startvert, so when
    // flattening the whole imagery into one global mesh we must add that base
    // back onto every face index.
    const int32_t num_objects = img->NumObjects();
    for (int32_t objnum = 0; objnum < num_objects; ++objnum) {
        S3DObj obj = {};
        img->GetObject(objnum, &obj);
        if (obj.numfaces <= 0 || obj.numverts <= 0) continue;

        std::vector<S3DFace> obj_faces(obj.numfaces);
        img->GetObjFaces(objnum, obj_faces.data(), nullptr, nullptr);
        const int32_t base = obj.startvert;

        for (const auto& f : obj_faces) {
            int32_t i0 = base + int32_t(f.v1);
            int32_t i1 = base + int32_t(f.v2);
            int32_t i2 = base + int32_t(f.v3);
            if (i0 < 0 || i0 >= nv) continue;
            if (i1 < 0 || i1 >= nv) continue;
            if (i2 < 0 || i2 >= nv) continue;
            indices.push_back(uint16_t(i0));
            indices.push_back(uint16_t(i1));
            indices.push_back(uint16_t(i2));
        }
    }
    for (auto& v : verts) {
        float nx = v.normal[0], ny = v.normal[1], nz = v.normal[2];
        float len2 = nx * nx + ny * ny + nz * nz;
        if (len2 > 1e-12f) {
            float inv = 1.0f / std::sqrt(len2);
            v.normal[0] = nx * inv; v.normal[1] = ny * inv; v.normal[2] = nz * inv;
        } else {
            v.normal[2] = 1.0f;
        }
    }
    return !indices.empty();
}

void BuildStaticObjectMatrix(T3DImagery* img, int32_t objnum,
                             int32_t state, int32_t frame,
                             float out16[16])
{
    if (!img || objnum < 0 || objnum >= img->NumObjects()) {
        for (int32_t i = 0; i < 16; ++i)
            out16[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }

    const int32_t num_objects = img->NumObjects();
    std::vector<S3DAnimObj> animobjs(num_objects);
    for (int32_t i = 0; i < num_objects; ++i)
    {
        S3DAnimObj& obj = animobjs[i];
        std::memset(&obj, 0, sizeof(obj));
        obj.objnum = i;
        obj.animtrack = i;
        obj.scl.X = obj.scl.Y = obj.scl.Z = 1.0f;
        obj.parent = nullptr;
    }

    for (int32_t i = 0; i < num_objects; ++i)
    {
        const int32_t parent = img->GetObjectParent(i, state);
        if ((uint32_t)parent < (uint32_t)num_objects)
            animobjs[i].parent = &animobjs[parent];
    }

    // Match the source/runtime path: disable cross-state interpolation and let
    // T3DImagery build the object matrix via CalcObjectMatrix.
    img->SetPrevState(-1, 0);
    hmm_mat4 identity = {};
    MtxClear(&identity);
    img->CalcObjectMatrix(&animobjs[objnum], state, frame, &identity, true);
    hmm_mat4 m = animobjs[objnum].matrix;

    // CalcObjectMatrix / math3d use row-vector convention with translation in
    // Elements[3][0..2]. The mesh shader consumes rows as a column-vector
    // affine matrix with translation in w0.w / w1.w / w2.w, so transpose here.
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
            out16[r * 4 + c] = m.Elements[c][r];
}

void BuildAnimatedObjectMatrix(T3DImagery* img, int32_t objnum,
                               int32_t state, int32_t frame,
                               int32_t prevstate, int32_t prevframe,
                               float out16[16])
{
    if (!img || objnum < 0 || objnum >= img->NumObjects()) {
        for (int32_t i = 0; i < 16; ++i)
            out16[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }

    const int32_t num_objects = img->NumObjects();
    std::vector<S3DAnimObj> animobjs(num_objects);
    for (int32_t i = 0; i < num_objects; ++i)
    {
        S3DAnimObj& obj = animobjs[i];
        std::memset(&obj, 0, sizeof(obj));
        obj.objnum = i;
        obj.animtrack = i;
        obj.scl.X = obj.scl.Y = obj.scl.Z = 1.0f;
        obj.parent = nullptr;
    }

    for (int32_t i = 0; i < num_objects; ++i)
    {
        const int32_t parent = img->GetObjectParent(i, state);
        if ((uint32_t)parent < (uint32_t)num_objects)
            animobjs[i].parent = &animobjs[parent];
    }

    img->SetPrevState(prevstate, prevframe);
    hmm_mat4 identity = {};
    MtxClear(&identity);
    img->CalcObjectMatrix(&animobjs[objnum], state, frame, &identity, true);
    hmm_mat4 m = animobjs[objnum].matrix;

    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
            out16[r * 4 + c] = m.Elements[c][r];
}

SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame)
{
    return SampleI3DAnimPose(img, state, frame, -1, 0);
}

SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame,
                            int32_t prevstate, int32_t prevframe)
{
    SAnimPose pose;
    if (!img || state < 0 || state >= img->NumStates())
        return pose;

    img->SetPrevState(prevstate, prevframe);
    for (int32_t objnum = 0; objnum < img->NumObjects(); ++objnum)
    {
        if (img->IsHidden(objnum, state))
            continue;

        hmm_vec3 pos = {};
        hmm_vec3 rot = {};
        hmm_vec3 scl = {1.0f, 1.0f, 1.0f};
        if (!img->GetAniKey(objnum, state, frame, pos, rot, scl))
            continue;

        const uint16_t target = uint16_t(objnum);
        pose.Set(AnimTrack(target, EAnimChannel::PosX), pos.X);
        pose.Set(AnimTrack(target, EAnimChannel::PosY), pos.Y);
        pose.Set(AnimTrack(target, EAnimChannel::PosZ), pos.Z);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleX), scl.X);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleY), scl.Y);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleZ), scl.Z);
        pose.SetQuat(target, QuatFromEulerXYZ(rot.X, rot.Y, rot.Z));
    }
    return pose;
}

SAnimPose BlendI3DAnimPoses(const SAnimPose& a, const SAnimPose& b, float t)
{
    SAnimPose out = a;
    std::vector<uint16_t> quat_targets;
    for (const auto& kv : b.values)
    {
        const AnimTrackId id = kv.first;
        const uint16_t channel = AnimTrackChannel(id);
        const uint16_t target = AnimTrackTarget(id);
        if (channel == uint16_t(EAnimChannel::RotX) ||
            channel == uint16_t(EAnimChannel::RotY) ||
            channel == uint16_t(EAnimChannel::RotZ) ||
            channel == uint16_t(EAnimChannel::RotW))
        {
            quat_targets.push_back(target);
            continue;
        }
        const float av = a.Get(id, kv.second);
        out.Set(id, av + (kv.second - av) * t);
    }
    std::sort(quat_targets.begin(), quat_targets.end());
    quat_targets.erase(std::unique(quat_targets.begin(), quat_targets.end()), quat_targets.end());
    for (uint16_t target : quat_targets)
        out.SetQuat(target, NlerpQuat(a.GetQuat(target), b.GetQuat(target), t));
    return out;
}

void MatrixFromAnimPoseObject(const SAnimPose& pose, uint16_t target, float out16[16])
{
    const float px = pose.Get(AnimTrack(target, EAnimChannel::PosX), 0.0f);
    const float py = pose.Get(AnimTrack(target, EAnimChannel::PosY), 0.0f);
    const float pz = pose.Get(AnimTrack(target, EAnimChannel::PosZ), 0.0f);
    const float sx = pose.Get(AnimTrack(target, EAnimChannel::ScaleX), 1.0f);
    const float sy = pose.Get(AnimTrack(target, EAnimChannel::ScaleY), 1.0f);
    const float sz = pose.Get(AnimTrack(target, EAnimChannel::ScaleZ), 1.0f);
    const SAnimQuat q = pose.GetQuat(target);

    const float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
    const float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
    const float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

    // The retail matrix helpers use row vectors and append T then S. The
    // renderer consumes the transposed form, so emit (R * T * S)^T here.
    const float r00 = 1.0f - 2.0f * (yy + zz);
    const float r01 = 2.0f * (xy + wz);
    const float r02 = 2.0f * (xz - wy);
    const float r10 = 2.0f * (xy - wz);
    const float r11 = 1.0f - 2.0f * (xx + zz);
    const float r12 = 2.0f * (yz + wx);
    const float r20 = 2.0f * (xz + wy);
    const float r21 = 2.0f * (yz - wx);
    const float r22 = 1.0f - 2.0f * (xx + yy);

    out16[0] = r00 * sx;
    out16[1] = r10 * sx;
    out16[2] = r20 * sx;
    out16[3] = px * sx;

    out16[4] = r01 * sy;
    out16[5] = r11 * sy;
    out16[6] = r21 * sy;
    out16[7] = py * sy;

    out16[8] = r02 * sz;
    out16[9] = r12 * sz;
    out16[10] = r22 * sz;
    out16[11] = pz * sz;

    out16[12] = 0.0f; out16[13] = 0.0f; out16[14] = 0.0f; out16[15] = 1.0f;
}

void BuildAnimPoseObjectMatrix(T3DImagery* img, const SAnimPose& pose,
                               int32_t state, int32_t objnum, float out16[16])
{
    if (!img || objnum < 0 || objnum >= img->NumObjects())
    {
        for (int32_t i = 0; i < 16; ++i)
            out16[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }

    MatrixFromAnimPoseObject(pose, uint16_t(objnum), out16);

    const int32_t parent = img->GetObjectParent(objnum, state);
    if ((uint32_t)parent < (uint32_t)img->NumObjects())
    {
        float parent_mtx[16];
        float combined[16];
        BuildAnimPoseObjectMatrix(img, pose, state, parent, parent_mtx);
        MatrixMulLocal(parent_mtx, out16, combined);
        std::memcpy(out16, combined, sizeof(combined));
    }
}
