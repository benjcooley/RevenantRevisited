// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *              render_metadata.h - Render Policy Metadata                *
// *************************************************************************

#pragma once

#include "defdoc.h"

#include <map>
#include <string>
#include <vector>

enum class ERenderMetaPass
{
    OpaqueWorld,
    TransparentWorld,
    Overlay,
};

enum class ERenderMetaBlend
{
    Opaque,
    Alpha,
    Additive,
};

enum class ERenderMetaLighting
{
    Lit,
    Fullbright,
    VertexLit,
    UnlitShadow,
};

enum class ERenderMetaDrawable
{
    Mesh,
    WaterParticles,
    WaterfallParticles,
};

enum class ERenderMetaTextureAnim
{
    None,
    FrameByInstance,
};

struct SRenderPolicy
{
    std::string name;
    std::string imagery;
    ERenderMetaDrawable drawable = ERenderMetaDrawable::Mesh;
    ERenderMetaPass pass = ERenderMetaPass::OpaqueWorld;
    ERenderMetaBlend blend = ERenderMetaBlend::Opaque;
    ERenderMetaLighting lighting = ERenderMetaLighting::Lit;
    bool ztest = true;
    bool zwrite = true;
    ERenderMetaTextureAnim texture_anim = ERenderMetaTextureAnim::None;
};

struct SRenderSubmeshPolicy
{
    std::string name;
    ERenderMetaPass pass = ERenderMetaPass::OpaqueWorld;
    ERenderMetaBlend blend = ERenderMetaBlend::Opaque;
    ERenderMetaLighting lighting = ERenderMetaLighting::Lit;
    bool ztest = true;
    bool zwrite = true;
};

struct SRenderObjectPolicy : SRenderPolicy
{
    std::vector<SRenderSubmeshPolicy> submeshes;
};

struct SRenderMetadata
{
    std::map<std::string, SRenderPolicy> effects;
    std::map<std::string, SRenderObjectPolicy> helpers;

    const SRenderPolicy* FindEffect(const std::string& name) const;
    const SRenderObjectPolicy* FindHelper(const std::string& name) const;
};

bool LoadRenderMetadata(const defdoc::Document& doc, SRenderMetadata& out,
                        std::string* error = nullptr);
bool LoadRenderMetadataFile(const char* path, SRenderMetadata& out,
                            std::string* error = nullptr);
