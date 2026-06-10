// *************************************************************************
// *                         Cinematix Revenant                            *
// *          i3dgltf.cpp - I3D to glTF 2.0 (GLB) asset exporter           *
// *************************************************************************
//
// See i3dgltf.h for the export model. Coordinate conversion: I3D model
// space is 3DS-Max-style right-handed Z-up (the engine's vertical display
// stretch WORLD3D_Z_SCALE applies to mesh-local Z), while glTF is
// right-handed Y-up. Positions, normals and transforms are rotated -90
// degrees about X — (x, y, z) -> (x, z, -y) — a pure rotation, so no
// mirroring and no winding flip. Materials are exported double-sided,
// matching the engine's no-cull rendering.

#include "i3dgltf.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <stb_image_write.h>   // impl lives in framesnap.cpp

#include "3dimage.h"
#include "imagery.h"
#include "revdefs.h"
#include "imageres.h"
#include "logging.h"
#include "meshextract.h"
#include "renderer.h"

namespace {

constexpr float kLegacyAnimFps = 24.0f;             // legacy integer-frame rate
constexpr float kRootYScale    = WORLD3D_Z_SCALE;   // engine vertical display
    // stretch (model-local Z = glTF Y after conversion), on the root node only

// ---------------------------------------------------------------------------
// Small JSON / binary helpers
// ---------------------------------------------------------------------------

std::string JsonEscape(const char* s)
{
    std::string out;
    for (; s && *s; ++s)
    {
        const unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') { out.push_back('\\'); out.push_back(char(c)); }
        else if (c < 0x20) { char b[8]; std::snprintf(b, sizeof(b), "\\u%04x", c); out += b; }
        else out.push_back(char(c));
    }
    return out;
}

std::string Num(double v)
{
    if (!std::isfinite(v)) v = 0.0;
    char b[32];
    std::snprintf(b, sizeof(b), "%.9g", v);
    return b;
}

// Joins JSON fragments: "a","b","c" -> "a,b,c"
std::string Join(const std::vector<std::string>& parts)
{
    std::string out;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (i) out.push_back(',');
        out += parts[i];
    }
    return out;
}

struct SGlbBuilder
{
    std::vector<uint8_t>     bin;
    std::vector<std::string> buffer_views;
    std::vector<std::string> accessors;

    // Appends raw bytes 4-aligned and records a bufferView. Returns its index.
    int32_t AddView(const void* data, size_t size, const char* target_json)
    {
        while (bin.size() % 4) bin.push_back(0);
        const size_t offset = bin.size();
        bin.insert(bin.end(), (const uint8_t*)data, (const uint8_t*)data + size);
        std::string v = "{\"buffer\":0,\"byteOffset\":" + std::to_string(offset)
                      + ",\"byteLength\":" + std::to_string(size);
        if (target_json && target_json[0]) v += std::string(",\"target\":") + target_json;
        v += "}";
        buffer_views.push_back(std::move(v));
        return int32_t(buffer_views.size()) - 1;
    }

    int32_t AddAccessor(int32_t view, int32_t component_type, const char* type,
                        size_t count, const std::string& extra = std::string())
    {
        std::string a = "{\"bufferView\":" + std::to_string(view)
                      + ",\"componentType\":" + std::to_string(component_type)
                      + ",\"count\":" + std::to_string(count)
                      + ",\"type\":\"" + type + "\"";
        if (!extra.empty()) a += "," + extra;
        a += "}";
        accessors.push_back(std::move(a));
        return int32_t(accessors.size()) - 1;
    }

    int32_t AddFloatAccessor(const std::vector<float>& data, const char* type,
                             int32_t components, bool with_min_max)
    {
        const size_t count = data.size() / size_t(components);
        const int32_t view = AddView(data.data(), data.size() * sizeof(float), nullptr);
        std::string extra;
        if (with_min_max && count > 0)
        {
            std::vector<float> mn(components, FLT_MAX), mx(components, -FLT_MAX);
            for (size_t i = 0; i < count; ++i)
            {
                for (int32_t k = 0; k < components; ++k)
                {
                    mn[k] = (std::min)(mn[k], data[i * components + k]);
                    mx[k] = (std::max)(mx[k], data[i * components + k]);
                }
            }
            std::string mns, mxs;
            for (int32_t k = 0; k < components; ++k)
            {
                if (k) { mns.push_back(','); mxs.push_back(','); }
                mns += Num(mn[k]);
                mxs += Num(mx[k]);
            }
            extra = "\"min\":[" + mns + "],\"max\":[" + mxs + "]";
        }
        return AddAccessor(view, 5126 /*FLOAT*/, type, count, extra);
    }
};

// ---------------------------------------------------------------------------
// Transform decomposition (with the LH->RH Z-flip baked in)
// ---------------------------------------------------------------------------

struct STrs
{
    float t[3] = { 0, 0, 0 };
    float r[4] = { 0, 0, 0, 1 };   // glTF order: x, y, z, w
    float s[3] = { 1, 1, 1 };
};

// `m16` is the row-major p' = M*p matrix from BuildStaticObjectMatrix.
// Conjugates by the Z-up -> Y-up basis rotation C: (x,y,z) -> (x,z,-y),
// i.e. M' = C * M * C^T, then decomposes into TRS.
STrs DecomposeYUp(const float m16[16])
{
    static constexpr int32_t kAxis[3] = { 0, 2, 1 };
    static constexpr float   kSign[3] = { 1.0f, 1.0f, -1.0f };
    float m[3][4];
    for (int32_t r = 0; r < 3; ++r)
    {
        for (int32_t c = 0; c < 3; ++c)
            m[r][c] = kSign[r] * kSign[c] * m16[kAxis[r] * 4 + kAxis[c]];
        m[r][3] = kSign[r] * m16[kAxis[r] * 4 + 3];
    }

    STrs out;
    out.t[0] = m[0][3]; out.t[1] = m[1][3]; out.t[2] = m[2][3];

    float col[3][3];
    for (int32_t j = 0; j < 3; ++j)
    {
        col[j][0] = m[0][j]; col[j][1] = m[1][j]; col[j][2] = m[2][j];
        out.s[j] = std::sqrt(col[j][0] * col[j][0] + col[j][1] * col[j][1] +
                             col[j][2] * col[j][2]);
    }
    const float det =
          m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
        - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
        + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    if (det < 0.0f)
    {
        out.s[0] = -out.s[0];
        for (int32_t k = 0; k < 3; ++k) col[0][k] = -col[0][k];
    }
    float R[3][3];
    for (int32_t j = 0; j < 3; ++j)
    {
        const float inv = (std::fabs(out.s[j]) > 1e-12f) ? 1.0f / out.s[j] : 0.0f;
        R[0][j] = col[j][0] * inv;
        R[1][j] = col[j][1] * inv;
        R[2][j] = col[j][2] * inv;
    }

    // Shepperd's method, column-vector convention.
    const float trace = R[0][0] + R[1][1] + R[2][2];
    float x, y, z, w;
    if (trace > 0.0f)
    {
        const float r4 = std::sqrt(1.0f + trace) * 2.0f;
        w = 0.25f * r4;
        x = (R[2][1] - R[1][2]) / r4;
        y = (R[0][2] - R[2][0]) / r4;
        z = (R[1][0] - R[0][1]) / r4;
    }
    else if (R[0][0] > R[1][1] && R[0][0] > R[2][2])
    {
        const float r4 = std::sqrt(1.0f + R[0][0] - R[1][1] - R[2][2]) * 2.0f;
        x = 0.25f * r4;
        w = (R[2][1] - R[1][2]) / r4;
        y = (R[0][1] + R[1][0]) / r4;
        z = (R[0][2] + R[2][0]) / r4;
    }
    else if (R[1][1] > R[2][2])
    {
        const float r4 = std::sqrt(1.0f + R[1][1] - R[0][0] - R[2][2]) * 2.0f;
        y = 0.25f * r4;
        w = (R[0][2] - R[2][0]) / r4;
        x = (R[0][1] + R[1][0]) / r4;
        z = (R[1][2] + R[2][1]) / r4;
    }
    else
    {
        const float r4 = std::sqrt(1.0f + R[2][2] - R[0][0] - R[1][1]) * 2.0f;
        z = 0.25f * r4;
        w = (R[1][0] - R[0][1]) / r4;
        x = (R[0][2] + R[2][0]) / r4;
        y = (R[1][2] + R[2][1]) / r4;
    }
    const float len = std::sqrt(x * x + y * y + z * z + w * w);
    const float inv = (len > 1e-12f) ? 1.0f / len : 1.0f;
    out.r[0] = x * inv; out.r[1] = y * inv; out.r[2] = z * inv; out.r[3] = w * inv;
    return out;
}

std::string AssetStem(const char* path)
{
    std::string base = path ? path : "";
    const size_t slash = base.find_last_of("/\\");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    const size_t dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);
    if (base.empty()) base = "asset";
    return base;
}

void PngToMemory(void* context, void* data, int size)
{
    auto* out = (std::vector<uint8_t>*)context;
    out->insert(out->end(), (uint8_t*)data, (uint8_t*)data + size);
}

}  // namespace

bool DumpI3DToGltfPath(const char* asset_path, const char* out_path)
{
    namespace fs = std::filesystem;
    if (!asset_path || !asset_path[0])
    {
        log_error("[gltf] asset path is empty");
        return false;
    }
    const std::string stem = AssetStem(asset_path);
    const fs::path out_file = (out_path && out_path[0])
        ? fs::path(out_path)
        : (fs::current_path() / (stem + ".glb"));

    // Retain decoded texture RGBA CPU-side; must be set before LoadImagery.
    T3DImagery::g_retain_decoded_rgba = true;

    int32_t img_id = TObjectImagery::FindImagery(asset_path);
    if (img_id < 0)
    {
        std::string path_copy = asset_path;
        img_id = TObjectImagery::RegisterImagery(path_copy.data());
        if (img_id < 0)
        {
            log_error("[gltf] FindImagery + RegisterImagery both failed for '%s'", asset_path);
            return false;
        }
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[gltf] LoadImagery(id=%d '%s') failed", img_id, asset_path);
        return false;
    }
    T3DImagery* img = dynamic_cast<T3DImagery*>(base);
    if (!img)
    {
        log_error("[gltf] '%s' is not a T3DImagery", asset_path);
        return false;
    }

    const int32_t num_objects  = img->NumObjects();
    const int32_t num_textures = img->NumTextures();
    const int32_t num_states   = img->NumStates();
    if (num_objects <= 0)
    {
        log_error("[gltf] '%s' has no sub-objects", asset_path);
        return false;
    }

    SGlbBuilder glb;
    std::vector<std::string> meshes, nodes, materials, textures, images, animations;

    // ---- materials / textures / images (texture frame 0) ----
    // glTF UV origin matches D3D (top-left, v down), so rows go out as-is.
    stbi_flip_vertically_on_write(0);
    // material index per texture slot; slot 0 = untextured run.
    std::vector<int32_t> slot_material(size_t(num_textures) + 1, -1);
    for (int32_t t = 0; t < num_textures; ++t)
    {
        S3DTex tex = {};
        img->GetTexture(t, &tex);
        const int32_t w = int32_t(tex.desc.width);
        const int32_t h = int32_t(tex.desc.height);
        if (t >= int32_t(img->dump_textures.size())) continue;
        const auto& frames = img->dump_textures[size_t(t)];
        if (frames.empty() || frames[0].empty() ||
            int32_t(frames[0].size()) != w * h * 4)
            continue;

        std::vector<uint8_t> png;
        if (stbi_write_png_to_func(PngToMemory, &png, w, h, 4,
                                   frames[0].data(), w * 4) == 0 || png.empty())
        {
            log_warn("[gltf] png encode failed for texture[%d] of '%s'", t, asset_path);
            continue;
        }
        const int32_t view = glb.AddView(png.data(), png.size(), nullptr);
        images.push_back("{\"bufferView\":" + std::to_string(view)
                         + ",\"mimeType\":\"image/png\",\"name\":\"texture_"
                         + std::to_string(t) + "\"}");
        textures.push_back("{\"sampler\":0,\"source\":"
                           + std::to_string(int32_t(images.size()) - 1) + "}");
        materials.push_back(
            "{\"name\":\"tex_" + std::to_string(t) + "\""
            ",\"pbrMetallicRoughness\":{\"baseColorTexture\":{\"index\":"
            + std::to_string(int32_t(textures.size()) - 1)
            + "},\"metallicFactor\":0,\"roughnessFactor\":1}"
            ",\"alphaMode\":\"MASK\",\"alphaCutoff\":0.5,\"doubleSided\":true}");
        slot_material[size_t(t) + 1] = int32_t(materials.size()) - 1;
    }
    int32_t untextured_material = -1;

    // ---- meshes + nodes (flat list under one root) ----
    // node_of_object[o] = glTF node index, or -1 when the sub-object never
    // produced geometry (pure helpers are skipped; animation is baked in
    // world space so nothing depends on them structurally).
    std::vector<int32_t> node_of_object(size_t(num_objects), -1);
    std::vector<int32_t> object_of_node;   // reverse map for animation baking
    const SAnimPose rest_pose = SampleI3DAnimPose(img, 0, 0);
    for (int32_t o = 0; o < num_objects; ++o)
    {
        std::vector<std::string> prims;
        for (int32_t slot = 0; slot < num_textures + 1; ++slot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(img, o, slot, verts, indices))
                continue;
            if (verts.empty() || indices.empty())
                continue;

            std::vector<float> pos, nrm, uv;
            pos.reserve(verts.size() * 3);
            nrm.reserve(verts.size() * 3);
            uv.reserve(verts.size() * 2);
            for (const SMeshVertex& v : verts)
            {
                pos.push_back(v.pos[0]);
                pos.push_back(v.pos[2]);
                pos.push_back(-v.pos[1]);
                float nx = v.normal[0], ny = v.normal[2], nz = -v.normal[1];
                const float nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
                if (nlen > 1e-6f) { nx /= nlen; ny /= nlen; nz /= nlen; }
                else              { nx = 0; ny = 0; nz = 1; }
                nrm.push_back(nx); nrm.push_back(ny); nrm.push_back(nz);
                uv.push_back(v.uv[0]);
                uv.push_back(v.uv[1]);
            }
            const int32_t pos_acc = glb.AddFloatAccessor(pos, "VEC3", 3, true);
            const int32_t nrm_acc = glb.AddFloatAccessor(nrm, "VEC3", 3, false);
            const int32_t uv_acc  = glb.AddFloatAccessor(uv,  "VEC2", 2, false);
            const int32_t idx_view = glb.AddView(indices.data(),
                                                 indices.size() * sizeof(uint16_t),
                                                 "34963");
            const int32_t idx_acc = glb.AddAccessor(idx_view, 5123 /*USHORT*/,
                                                    "SCALAR", indices.size());

            int32_t mat = slot_material[size_t(slot)];
            if (mat < 0)
            {
                if (untextured_material < 0)
                {
                    materials.push_back(
                        "{\"name\":\"untextured\",\"pbrMetallicRoughness\":"
                        "{\"baseColorFactor\":[1,1,1,1],\"metallicFactor\":0},"
                        "\"doubleSided\":true}");
                    untextured_material = int32_t(materials.size()) - 1;
                }
                mat = untextured_material;
            }
            prims.push_back(
                "{\"attributes\":{\"POSITION\":" + std::to_string(pos_acc)
                + ",\"NORMAL\":" + std::to_string(nrm_acc)
                + ",\"TEXCOORD_0\":" + std::to_string(uv_acc)
                + "},\"indices\":" + std::to_string(idx_acc)
                + ",\"material\":" + std::to_string(mat) + "}");
        }
        if (prims.empty())
            continue;

        const char* name = img->GetObjectName(o);
        const std::string node_name =
            (name && name[0]) ? JsonEscape(name) : ("obj_" + std::to_string(o));
        meshes.push_back("{\"name\":\"" + node_name + "\",\"primitives\":["
                         + Join(prims) + "]}");

        // Rest pose: state 0, frame 0 world transform.
        float m16[16];
        BuildAnimPoseObjectMatrix(img, rest_pose, 0, o, m16);
        const STrs trs = DecomposeYUp(m16);
        nodes.push_back(
            "{\"name\":\"" + node_name + "\""
            ",\"mesh\":" + std::to_string(int32_t(meshes.size()) - 1)
            + ",\"translation\":[" + Num(trs.t[0]) + "," + Num(trs.t[1]) + "," + Num(trs.t[2])
            + "],\"rotation\":[" + Num(trs.r[0]) + "," + Num(trs.r[1]) + ","
                                 + Num(trs.r[2]) + "," + Num(trs.r[3])
            + "],\"scale\":[" + Num(trs.s[0]) + "," + Num(trs.s[1]) + "," + Num(trs.s[2])
            + "]}");
        node_of_object[size_t(o)] = int32_t(nodes.size()) - 1;   // root prepended later
        object_of_node.push_back(o);
    }
    if (meshes.empty())
    {
        log_error("[gltf] '%s' produced no mesh data", asset_path);
        return false;
    }

    // ---- animations: one per state, world TRS keys at 24 Hz ----
    for (int32_t s = 0; s < num_states; ++s)
    {
        const SImageryStateHeader* sh = img->GetState(s);
        const int32_t frames = sh ? (std::max)(int32_t(sh->frames), 1) : 1;

        std::vector<float> times(size_t(frames), 0.0f);
        for (int32_t f = 0; f < frames; ++f)
            times[size_t(f)] = float(f) / kLegacyAnimFps;
        const int32_t time_acc = glb.AddFloatAccessor(times, "SCALAR", 1, true);

        // Sample the legacy keys once per frame, then resolve every node's
        // world matrix from that pose.
        struct STracks { std::vector<float> t, r, s; };
        std::vector<STracks> tracks_of_node(object_of_node.size());
        for (int32_t f = 0; f < frames; ++f)
        {
            const SAnimPose pose = SampleI3DAnimPose(img, s, f);
            for (size_t n = 0; n < object_of_node.size(); ++n)
            {
                const int32_t o = object_of_node[n];
                float m16[16];
                BuildAnimPoseObjectMatrix(img, pose, s, o, m16);
                STrs trs = DecomposeYUp(m16);
                STracks& tk = tracks_of_node[n];
                // Keep successive quaternions on the same hemisphere so the
                // importer's linear interpolation takes the short arc.
                if (!tk.r.empty())
                {
                    const float* prev = &tk.r[tk.r.size() - 4];
                    const float dot = prev[0] * trs.r[0] + prev[1] * trs.r[1] +
                                      prev[2] * trs.r[2] + prev[3] * trs.r[3];
                    if (dot < 0.0f)
                        for (float& q : trs.r) q = -q;
                }
                tk.t.push_back(trs.t[0]); tk.t.push_back(trs.t[1]); tk.t.push_back(trs.t[2]);
                tk.r.push_back(trs.r[0]); tk.r.push_back(trs.r[1]);
                tk.r.push_back(trs.r[2]); tk.r.push_back(trs.r[3]);
                // A state can hide a sub-object; glTF has no visibility
                // animation, so collapse hidden parts to zero scale.
                const bool hidden = img->IsHidden(o, s);
                for (int32_t k = 0; k < 3; ++k)
                    tk.s.push_back(hidden ? 0.0f : trs.s[k]);
            }
        }

        std::vector<std::string> samplers, channels;
        for (size_t n = 0; n < object_of_node.size(); ++n)
        {
            std::vector<float>& tdata = tracks_of_node[n].t;
            std::vector<float>& rdata = tracks_of_node[n].r;
            std::vector<float>& sdata = tracks_of_node[n].s;
            const int32_t node_idx = 1 + int32_t(n);   // +1: root is node 0
            const struct { const char* path; std::vector<float>* data; const char* type; } tracks[] = {
                { "translation", &tdata, "VEC3" },
                { "rotation",    &rdata, "VEC4" },
                { "scale",       &sdata, "VEC3" },
            };
            for (const auto& tr : tracks)
            {
                const int32_t out_acc = glb.AddFloatAccessor(*tr.data, tr.type,
                    int32_t(tr.data->size() / size_t(frames)), false);
                samplers.push_back("{\"input\":" + std::to_string(time_acc)
                                   + ",\"interpolation\":\"LINEAR\",\"output\":"
                                   + std::to_string(out_acc) + "}");
                channels.push_back("{\"sampler\":"
                                   + std::to_string(int32_t(samplers.size()) - 1)
                                   + ",\"target\":{\"node\":" + std::to_string(node_idx)
                                   + ",\"path\":\"" + tr.path + "\"}}");
            }
        }
        const std::string anim_name = (sh && sh->animname[0])
            ? JsonEscape(sh->animname)
            : ("state_" + std::to_string(s));
        animations.push_back("{\"name\":\"" + anim_name
                             + "\",\"samplers\":[" + Join(samplers)
                             + "],\"channels\":[" + Join(channels) + "]}");
    }

    // ---- root node + JSON assembly ----
    std::string children;
    for (size_t n = 0; n < object_of_node.size(); ++n)
    {
        if (n) children.push_back(',');
        children += std::to_string(1 + n);
    }
    const std::string root_node =
        "{\"name\":\"" + JsonEscape(stem.c_str()) + "\""
        ",\"scale\":[1," + Num(kRootYScale) + ",1]"
        ",\"children\":[" + children + "]}";

    std::string json;
    json += "{\"asset\":{\"version\":\"2.0\",\"generator\":\"RevenantRevisited i3dgltf\"}";
    json += ",\"scene\":0,\"scenes\":[{\"nodes\":[0]}]";
    json += ",\"nodes\":[" + root_node + "," + Join(nodes) + "]";
    json += ",\"meshes\":[" + Join(meshes) + "]";
    json += ",\"materials\":[" + Join(materials) + "]";
    if (!textures.empty())
    {
        json += ",\"samplers\":[{\"magFilter\":9729,\"minFilter\":9987,"
                "\"wrapS\":10497,\"wrapT\":10497}]";
        json += ",\"textures\":[" + Join(textures) + "]";
        json += ",\"images\":[" + Join(images) + "]";
    }
    if (!animations.empty())
        json += ",\"animations\":[" + Join(animations) + "]";
    json += ",\"bufferViews\":[" + Join(glb.buffer_views) + "]";
    json += ",\"accessors\":[" + Join(glb.accessors) + "]";
    json += ",\"buffers\":[{\"byteLength\":" + std::to_string(glb.bin.size()) + "}]";
    json += "}";

    // ---- GLB container ----
    while (json.size() % 4) json.push_back(' ');
    while (glb.bin.size() % 4) glb.bin.push_back(0);
    const uint32_t total = 12 + 8 + uint32_t(json.size()) + 8 + uint32_t(glb.bin.size());

    std::error_code ec;
    if (out_file.has_parent_path())
        fs::create_directories(out_file.parent_path(), ec);
    std::ofstream f(out_file, std::ios::binary);
    if (!f)
    {
        log_error("[gltf] cannot open '%s' for write", out_file.string().c_str());
        return false;
    }
    auto write_u32 = [&f](uint32_t v) { f.write((const char*)&v, 4); };
    write_u32(0x46546C67);            // 'glTF'
    write_u32(2);
    write_u32(total);
    write_u32(uint32_t(json.size()));
    write_u32(0x4E4F534A);            // 'JSON'
    f.write(json.data(), std::streamsize(json.size()));
    write_u32(uint32_t(glb.bin.size()));
    write_u32(0x004E4942);            // 'BIN'
    f.write((const char*)glb.bin.data(), std::streamsize(glb.bin.size()));
    if (!f.good())
    {
        log_error("[gltf] write failed for '%s'", out_file.string().c_str());
        return false;
    }

    log_info("[gltf] '%s' -> %s (%d nodes, %d meshes, %d animations, %zu KB)",
             asset_path, out_file.string().c_str(),
             int32_t(object_of_node.size()), int32_t(meshes.size()),
             int32_t(animations.size()), (12 + json.size() + glb.bin.size()) / 1024);
    return true;
}

bool DumpGltfFromStartupArgs(const char* path_arg, const char* out_arg)
{
    namespace fs = std::filesystem;
    if (!path_arg || !path_arg[0])
        return false;

    if (path_arg[0] != '@')
    {
        std::string out;
        if (out_arg && out_arg[0])
        {
            std::string o = out_arg;
            const bool is_glb = o.size() > 4 &&
                strcasecmp(o.c_str() + o.size() - 4, ".glb") == 0;
            out = is_glb ? o
                         : (fs::path(o) / (AssetStem(path_arg) + ".glb")).string();
        }
        return DumpI3DToGltfPath(path_arg, out.c_str());
    }

    // Batch: @LIST_FILE, one asset path per non-empty/non-comment line.
    const char* list_path = path_arg + 1;
    std::ifstream list(list_path);
    if (!list)
    {
        log_error("[gltf] cannot open list file '%s'", list_path);
        return false;
    }
    const fs::path base_out = (out_arg && out_arg[0])
        ? fs::path(out_arg)
        : (fs::current_path() / "gltf_dump");
    std::error_code ec;
    fs::create_directories(base_out, ec);

    int32_t ok = 0, fail = 0;
    std::string line;
    while (std::getline(list, line))
    {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' ||
                                 line.back() == ' '  || line.back() == '\t'))
            line.pop_back();
        size_t start = 0;
        while (start < line.size() && (line[start] == ' ' || line[start] == '\t'))
            ++start;
        if (start) line = line.substr(start);
        if (line.empty() || line[0] == '#')
            continue;

        const fs::path out = base_out / (AssetStem(line.c_str()) + ".glb");
        if (DumpI3DToGltfPath(line.c_str(), out.string().c_str()))
            ++ok;
        else
            ++fail;
    }
    log_info("[gltf] batch done: ok=%d fail=%d base='%s'",
             ok, fail, base_out.string().c_str());
    return ok > 0;
}
