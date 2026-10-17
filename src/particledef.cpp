// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        particledef.cpp - derived fields and validation               *
// *************************************************************************

#include "particledef.h"

#include <cctype>

namespace {

bool IEquals(std::string_view a, std::string_view b)
{
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    return true;
}

} // namespace

void SParticleBucketEffectDef::PostLoad()
{
    additive = IEquals(blend, "additive");
    chroma_key_enabled = chroma_key.X >= 0.0f && chroma_key.Y >= 0.0f && chroma_key.Z >= 0.0f;
    if (chroma_key_enabled)
    {
        chroma_key_rgb[0] = chroma_key.X / 255.0f;
        chroma_key_rgb[1] = chroma_key.Y / 255.0f;
        chroma_key_rgb[2] = chroma_key.Z / 255.0f;
    }
}

void SParticleEffectDef::PostLoad()
{
    debug_solid = IEquals(debug, "solid_green");
}

const SParticleBucketEffectDef* SParticleEffectDef::FindBucket(std::string_view bucket_name) const
{
    for (const SParticleBucketEffectDef& b : bucket)
        if (b.name == bucket_name)
            return &b;
    return nullptr;
}

bool SParticleEffectDef::Validate(std::string* error) const
{
    auto fail = [&](const std::string& msg) {
        if (error) *error = "effect " + name + ": " + msg;
        return false;
    };
    if (bucket.empty())
        return fail("has no buckets");
    bool any_self_spawn = false;
    for (const SParticleBucketEffectDef& b : bucket)
        if (b.spawn_burst > 0 || b.spawn_count > 0)
            any_self_spawn = true;
    if (emitter.empty() && !any_self_spawn)
        return fail("has no emitters and no self-spawning buckets");
    for (const SParticleBucketEffectDef& b : bucket)
    {
        if (b.name.empty())
            return fail("has an unnamed bucket");
        if (b.texture_slot < 0)
            return fail("bucket " + b.name + " has invalid texture_slot");
        if (b.atlas_cols <= 0 || b.atlas_rows <= 0 || b.atlas_frames <= 0)
            return fail("bucket " + b.name + " has invalid atlas settings");
        if (b.width <= 0.0f || b.height <= 0.0f)
            return fail("bucket " + b.name + " has invalid size");
        if (b.scale <= 0.0f)
            return fail("bucket " + b.name + " has invalid scale");
        if (b.spawn_burst > 0 && b.spawn_count > 0)
            return fail("bucket " + b.name + " has both spawn_burst and spawn_count set");
        if (b.tick_hz < 0)
            return fail("bucket " + b.name + " has negative tick_hz");
        if (!b.chain_bucket.empty() && !FindBucket(b.chain_bucket))
            return fail("bucket " + b.name + " chain_bucket='" + b.chain_bucket + "' targets unknown bucket");
    }
    for (const SParticleEmitterEffectDef& e : emitter)
    {
        if (e.name.empty())
            return fail("has an unnamed emitter");
        if (e.output.empty())
            return fail("emitter " + e.name + " has no outputs");
        for (const SParticleEmitterOutputDef& o : e.output)
            if (!FindBucket(o.name))
                return fail("emitter " + e.name + " targets unknown bucket " + o.name);
    }
    return true;
}

const SParticleEffectDef* SParticleEffectsFile::Find(std::string_view effect_name) const
{
    for (const SParticleEffectDef& e : effect)
        if (IEquals(e.name, effect_name))
            return &e;
    return nullptr;
}
