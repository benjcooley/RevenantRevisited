// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *             render_metadata.cpp - Render Policy Metadata               *
// *************************************************************************

#include "render_metadata.h"

#include <exception>

namespace {

std::string Lower(std::string s)
{
    for (char& c : s)
        c = char(std::tolower((unsigned char)c));
    return s;
}

template <class T>
bool ParseEnum(const std::string& text, const std::map<std::string, T>& table,
               T& out, const char* field, const std::string& name, std::string* error)
{
    auto it = table.find(Lower(text));
    if (it != table.end())
    {
        out = it->second;
        return true;
    }
    if (error)
        *error = "unknown " + std::string(field) + " '" + text + "' in policy '" + name + "'";
    return false;
}

const std::map<std::string, ERenderMetaPass>& PassTable()
{
    static const std::map<std::string, ERenderMetaPass> k = {
        {"opaque_world", ERenderMetaPass::OpaqueWorld},
        {"transparent_world", ERenderMetaPass::TransparentWorld},
        {"overlay", ERenderMetaPass::Overlay},
    };
    return k;
}

const std::map<std::string, ERenderMetaBlend>& BlendTable()
{
    static const std::map<std::string, ERenderMetaBlend> k = {
        {"opaque", ERenderMetaBlend::Opaque},
        {"alpha", ERenderMetaBlend::Alpha},
        {"additive", ERenderMetaBlend::Additive},
    };
    return k;
}

const std::map<std::string, ERenderMetaLighting>& LightingTable()
{
    static const std::map<std::string, ERenderMetaLighting> k = {
        {"lit", ERenderMetaLighting::Lit},
        {"fullbright", ERenderMetaLighting::Fullbright},
        {"vertex_lit", ERenderMetaLighting::VertexLit},
        {"unlit_shadow", ERenderMetaLighting::UnlitShadow},
    };
    return k;
}

const std::map<std::string, ERenderMetaDrawable>& DrawableTable()
{
    static const std::map<std::string, ERenderMetaDrawable> k = {
        {"mesh", ERenderMetaDrawable::Mesh},
        {"water_particles", ERenderMetaDrawable::WaterParticles},
        {"waterfall_particles", ERenderMetaDrawable::WaterfallParticles},
    };
    return k;
}

const std::map<std::string, ERenderMetaTextureAnim>& TextureAnimTable()
{
    static const std::map<std::string, ERenderMetaTextureAnim> k = {
        {"none", ERenderMetaTextureAnim::None},
        {"frame_by_instance", ERenderMetaTextureAnim::FrameByInstance},
    };
    return k;
}

bool ReadCommonPolicy(const defdoc::Node& n, SRenderPolicy& out, std::string* error)
{
    out.imagery = n.get_string("IMAGERY", out.imagery);

    if (n.contains("DRAWABLE") &&
        !ParseEnum(n.get_string("DRAWABLE"), DrawableTable(), out.drawable, "DRAWABLE", out.name, error))
        return false;
    if (n.contains("PASS") &&
        !ParseEnum(n.get_string("PASS"), PassTable(), out.pass, "PASS", out.name, error))
        return false;
    if (n.contains("BLEND") &&
        !ParseEnum(n.get_string("BLEND"), BlendTable(), out.blend, "BLEND", out.name, error))
        return false;
    if (n.contains("LIGHTING") &&
        !ParseEnum(n.get_string("LIGHTING"), LightingTable(), out.lighting, "LIGHTING", out.name, error))
        return false;
    if (n.contains("TEXTURE_ANIM") &&
        !ParseEnum(n.get_string("TEXTURE_ANIM"), TextureAnimTable(), out.texture_anim, "TEXTURE_ANIM", out.name, error))
        return false;

    out.ztest = n.get_bool("ZTEST", out.ztest);
    out.zwrite = n.get_bool("ZWRITE", out.zwrite);
    return true;
}

bool ReadSubmeshPolicy(const defdoc::Node& n, SRenderSubmeshPolicy& out,
                       const SRenderPolicy& parent, std::string* error)
{
    out.pass = parent.pass;
    out.blend = parent.blend;
    out.lighting = parent.lighting;
    out.ztest = parent.ztest;
    out.zwrite = parent.zwrite;
    out.name = n.get_string("$name");

    if (out.name.empty())
    {
        if (error) *error = "SUBMESH policy missing name in helper '" + parent.name + "'";
        return false;
    }
    if (n.contains("PASS") &&
        !ParseEnum(n.get_string("PASS"), PassTable(), out.pass, "SUBMESH PASS", out.name, error))
        return false;
    if (n.contains("BLEND") &&
        !ParseEnum(n.get_string("BLEND"), BlendTable(), out.blend, "SUBMESH BLEND", out.name, error))
        return false;
    if (n.contains("LIGHTING") &&
        !ParseEnum(n.get_string("LIGHTING"), LightingTable(), out.lighting, "SUBMESH LIGHTING", out.name, error))
        return false;
    out.ztest = n.get_bool("ZTEST", out.ztest);
    out.zwrite = n.get_bool("ZWRITE", out.zwrite);
    return true;
}

} // namespace

const SRenderPolicy* SRenderMetadata::FindEffect(const std::string& name) const
{
    auto it = effects.find(Lower(name));
    return it == effects.end() ? nullptr : &it->second;
}

const SRenderObjectPolicy* SRenderMetadata::FindHelper(const std::string& name) const
{
    auto it = helpers.find(Lower(name));
    return it == helpers.end() ? nullptr : &it->second;
}

bool LoadRenderMetadata(const defdoc::Document& doc, SRenderMetadata& out,
                        std::string* error)
{
    out = {};

    for (const defdoc::Node* n : doc.root().blocks("EFFECT"))
    {
        SRenderPolicy p;
        p.name = n->get_string("$name");
        if (p.name.empty())
        {
            if (error) *error = "EFFECT policy missing name";
            return false;
        }
        if (!ReadCommonPolicy(*n, p, error))
            return false;
        out.effects[Lower(p.name)] = std::move(p);
    }

    for (const defdoc::Node* n : doc.root().blocks("HELPER"))
    {
        SRenderObjectPolicy p;
        p.name = n->get_string("$name");
        if (p.name.empty())
        {
            if (error) *error = "HELPER policy missing name";
            return false;
        }
        if (!ReadCommonPolicy(*n, p, error))
            return false;

        for (const defdoc::Node* sub : n->blocks("SUBMESH"))
        {
            SRenderSubmeshPolicy sp;
            if (!ReadSubmeshPolicy(*sub, sp, p, error))
                return false;
            p.submeshes.push_back(std::move(sp));
        }
        out.helpers[Lower(p.name)] = std::move(p);
    }

    return true;
}

bool LoadRenderMetadataFile(const char* path, SRenderMetadata& out,
                            std::string* error)
{
    try
    {
        defdoc::ParseOptions opts;
        opts.preserve_comments = false;
        return LoadRenderMetadata(defdoc::Document::parse_file(path, opts), out, error);
    }
    catch (const std::exception& e)
    {
        if (error) *error = e.what();
        return false;
    }
}
