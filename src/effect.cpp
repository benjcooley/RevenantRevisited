// *************************************************************************
// *                         Cinematix Revenant                            *
// *                     effect.cpp - TEffect module                       *
// *************************************************************************
//
// Active effect code only. The D3D-era implementations are preserved in
// effect_old.cpp, which is entirely remarked out for reference.
//
// Legacy animator migration map (old registrations, intentionally commented):
//   REGISTER_3DANIMATOR("Flare", TFlareAnimator)
//   REGISTER_3DANIMATOR("Heal", THealAnimator)
//   REGISTER_3DANIMATOR("CreateFood", TCreateFoodAnimator)
//   REGISTER_3DANIMATOR("Cure", TCureAnimator)
//   REGISTER_3DANIMATOR("FireFlash", TFireFlashAnimator)
//   REGISTER_3DANIMATOR("FireWind", TFireWindAnimator)
//   REGISTER_3DANIMATOR("Burn", TBurnAnimator)
//   REGISTER_3DANIMATOR("Aura", TAuraAnimator)
//   REGISTER_3DANIMATOR("CYANFONT", TCyanFountainAnimator)
//   REGISTER_3DANIMATOR("REDFONT", TRedFountainAnimator)
//   REGISTER_3DANIMATOR("GREENFONT", TGreenFountainAnimator)
//   REGISTER_3DANIMATOR("BLUEFONT", TBlueFountainAnimator)
//   REGISTER_3DANIMATOR("RIBBON", TRibbonAnimator)
//   REGISTER_3DANIMATOR("SHIELD", TShieldAnimator)
//   REGISTER_3DANIMATOR("FLAME", TFlameAnimator) -> TFlipbookBillboardComponent
//   REGISTER_3DANIMATOR("SymGlow", TSymGlowAnimator)
//   REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator, TParticle3DAnimator)
//   REGISTER_3DANIMATOR("IrisFlare", TIrisFlareAnimator)
//   REGISTER_3DANIMATOR("SetVortex", TSetVortexAnimator)
//   REGISTER_3DANIMATOR("Teleporter", TTeleporterAnimator)
//   REGISTER_3DANIMATOR("Smoke", TSmokeEffectAnimator)
//   REGISTER_3DANIMATOR("MeteorStorm", TMeteorStormAnimator)
//   REGISTER_3DANIMATOR("Flies", TFlyAnimator)
//   REGISTER_3DANIMATOR("Fog", TFogAnimator)
//   REGISTER_3DANIMATOR("PULP", TPulpAnimator)
//   REGISTER_3DANIMATOR("FIRECONE", TFireConeAnimator)
//   REGISTER_3DANIMATOR("DragonFire", TDragonFireAnimator)
//   REGISTER_3DANIMATOR("IceBolt", TIceBoltAnimator)
//   REGISTER_3DANIMATOR("Iced", TIcedAnimator)
//   REGISTER_3DANIMATOR("Quicksand", TQuicksandAnimator)
//   REGISTER_3DANIMATOR("Sandswirl", TSandswirlAnimator)
//   REGISTER_3DANIMATOR("Tornado", TTornadoAnimator)
//   REGISTER_3DANIMATOR("Streamer", TStreamerAnimator)
//   REGISTER_3DANIMATOR("FireSwarm", TFireSwarmAnimator)
//   REGISTER_3DANIMATOR("Halo", THaloAnimator)
//   REGISTER_3DANIMATOR("Ripple", TRippleAnimator)
//   REGISTER_3DANIMATOR("Drip", TDripAnimator)
//   REGISTER_3DANIMATOR("FaultFire", TFaultFireAnimator)
//   REGISTER_3DANIMATOR("Blood", TBloodAnimator)
//   REGISTER_3DANIMATOR("Mist", TMistAnimator)
//   REGISTER_3DANIMATOR("MistFog", TMistFogAnimator)
//   REGISTER_3DANIMATOR("WaterFall", TWaterFallAnimator)
//   REGISTER_3DANIMATOR("Water", TWaterAnimator)
//   REGISTER_3DANIMATOR("Pixie", TPixieAnimator)
//   REGISTER_3DANIMATOR("Fizzle", TFizzleAnimator)
//   REGISTER_3DANIMATOR("Vortex", TVortexAnimator)
//   REGISTER_3DANIMATOR("Speaker", TAmbSoundAnimator)

#include "revenant.h"
#include "effect.h"

#include "3dimage.h"
#include "character.h"
#include "defdoc.h"
#include "mappane.h"
#include "meshextract.h"   // M09b: ExtractSubMesh for I3D cylinder mesh
#include "revutils.h"
#include "logging.h"
#include "stripeffect.h"   // S04: STRIP_* constants for TLightningAnimator_Bespoke
#include "time.h"

#include <algorithm>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

void ParticleFatal(const std::string& msg)
{
    FatalError(msg.c_str());
}

const defdoc::Node* FindNamedBlock(const defdoc::Node& parent, const char* block_type,
                                   const std::string& name)
{
    for (const defdoc::Node* block : parent.blocks(block_type))
        if ((*block)["$name"].as_string() == name)
            return block;
    return nullptr;
}

void ParseVec3(const defdoc::Node& node, const char* key, float out[3])
{
    const defdoc::Node& value = node[key];
    const defdoc::Node::Array& arr = value.as_array();
    if (arr.size() != 3)
        ParticleFatal(std::string("[particle] expected vec3 array for ") + key);
    out[0] = float(arr[0].as_double());
    out[1] = float(arr[1].as_double());
    out[2] = float(arr[2].as_double());
}

void ParseOptionalColorRgb01(const defdoc::Node& node, const char* key, bool& enabled, float out[3])
{
    if (!node.contains(key))
        return;

    const defdoc::Node::Array& arr = node[key].as_array();
    if (arr.size() != 3)
        ParticleFatal(std::string("[particle] expected RGB array for ") + key);
    out[0] = float(arr[0].as_double() / 255.0);
    out[1] = float(arr[1].as_double() / 255.0);
    out[2] = float(arr[2].as_double() / 255.0);
    enabled = true;
}

std::string ReadParticleEffectsFile()
{
    const char* candidates[] = {
        "data/Resources/effects.def",
        "../data/Resources/effects.def",
        "effects.def",
    };

    char fname[MAXPATHLEN];
    std::snprintf(fname, sizeof(fname), "%seffects.def", ClassDefPath);
    FILE* fp = rev_fopen(fname, "rb");
    if (!fp)
    {
        for (const char* candidate : candidates)
        {
            fp = rev_fopen(candidate, "rb");
            if (fp)
            {
                std::snprintf(fname, sizeof(fname), "%s", candidate);
                break;
            }
        }
    }
    if (!fp)
        ParticleFatal(std::string("[particle] unable to open effects.def; tried ") + fname +
                      ", data/Resources/effects.def, ../data/Resources/effects.def, effects.def");

    std::string text;
    char buf[4096];
    for (;;)
    {
        const size_t n = std::fread(buf, 1, sizeof(buf), fp);
        if (n > 0)
            text.append(buf, n);
        if (n < sizeof(buf))
            break;
    }
    std::fclose(fp);
    return text;
}

SParticleEffectDef ParseParticleEffectDef(const char* wanted)
{
    const std::string text = ReadParticleEffectsFile();
    SParticleEffectDef def = {};

    defdoc::Document doc;
    try
    {
        doc = defdoc::Document::parse(text);
    }
    catch (const std::exception& e)
    {
        ParticleFatal(std::string("[particle] effects.def parse failed: ") + e.what());
    }

    const defdoc::Node* effect = FindNamedBlock(doc.root(), "effect", wanted);
    if (!effect)
        ParticleFatal(std::string("[particle] missing effect definition ") + wanted);

    def.name = (*effect)["$name"].as_string();
    def.debug_solid = effect->get_string("debug") == "solid_green";
    def.inactive_ttl_pulses = uint32_t(effect->get_int("inactive_ttl_pulses", 1));

    for (const defdoc::Node* bucket_node : effect->blocks("bucket"))
    {
        SParticleBucketEffectDef bucket = {};
        bucket.name = (*bucket_node)["$name"].as_string();
        bucket.texture_slot = int32_t(bucket_node->get_int("texture_slot"));
        bucket.atlas_cols = int32_t(bucket_node->get_int("atlas_cols", 1));
        bucket.atlas_rows = int32_t(bucket_node->get_int("atlas_rows", 1));
        bucket.atlas_frames = int32_t(bucket_node->get_int("atlas_frames", 1));
        bucket.random_start_frame = bucket_node->get_bool("random_start_frame", false);
        bucket.width = float(bucket_node->get_double("width", 1.0));
        bucket.height = float(bucket_node->get_double("height", 1.0));
        bucket.scale = float(bucket_node->get_double("scale", 1.0));
        // Legacy `blend = "additive"|"alpha"` toggle preserved for the
        // existing TorchFlame consumer. The richer `blend_mode` slot
        // (additive_straight, premul_alpha) is read in BuildRuntimeBucketDesc
        // if present and overrides the legacy bool.
        bucket.additive = bucket_node->get_string("blend", "additive") == "additive";
        bucket.flip_v = bucket_node->get_bool("flip_v", false);
        ParseOptionalColorRgb01(*bucket_node, "chroma_key", bucket.chroma_key, bucket.chroma_key_rgb);
        bucket.frame_expr = bucket_node->get_string("frame_expr");
        bucket.uv_rect_expr = bucket_node->get_string("uv_rect_expr");

        // VM-extension fields (all optional; absence preserves legacy behaviour).
        bucket.imagery_path  = bucket_node->get_string("imagery_path");
        bucket.blend_mode    = bucket_node->get_string("blend_mode");
        bucket.light_mode    = bucket_node->get_string("light_mode");
        bucket.depth_mode    = bucket_node->get_string("depth_mode");
        bucket.orientation   = bucket_node->get_string("orientation");
        bucket.spawn_expr    = bucket_node->get_string("spawn_expr");
        bucket.tick_expr     = bucket_node->get_string("tick_expr");
        bucket.kill_expr     = bucket_node->get_string("kill_expr");
        bucket.spawn_burst   = int32_t(bucket_node->get_int("spawn_burst", 0));
        bucket.spawn_count   = int32_t(bucket_node->get_int("spawn_count", 0));
        bucket.tick_hz       = int32_t(bucket_node->get_int("tick_hz", 0));
        bucket.default_life  = float(bucket_node->get_double("default_life", -1.0));
        bucket.chain_bucket  = bucket_node->get_string("chain_bucket");
        def.buckets.push_back(std::move(bucket));
    }

    for (const defdoc::Node* emitter_node : effect->blocks("emitter"))
    {
        SParticleEmitterEffectDef emitter = {};
        emitter.name = (*emitter_node)["$name"].as_string();
        ParseVec3(*emitter_node, "local_pos", emitter.local_pos);
        for (const defdoc::Node* output_node : emitter_node->blocks("output"))
        {
            SParticleEmitterOutputDef output = {};
            output.bucket = (*output_node)["$name"].as_string();
            output.life = float(output_node->get_double("life", -1.0));
            emitter.outputs.push_back(std::move(output));
        }
        def.emitters.push_back(std::move(emitter));
    }

    if (def.buckets.empty())
        ParticleFatal("[particle] effect " + def.name + " has no buckets");
    // Emitters are now optional -- buckets with spawn_burst/spawn_count
    // self-emit and don't need an emitter/output block.
    bool any_self_spawn = false;
    for (const SParticleBucketEffectDef& bucket : def.buckets)
        if (bucket.spawn_burst > 0 || bucket.spawn_count > 0)
            any_self_spawn = true;
    if (def.emitters.empty() && !any_self_spawn)
        ParticleFatal("[particle] effect " + def.name + " has no emitters and no self-spawning buckets");
    for (const SParticleBucketEffectDef& bucket : def.buckets)
    {
        if (bucket.name.empty())
            ParticleFatal("[particle] effect " + def.name + " has unnamed bucket");
        if (bucket.texture_slot < 0)
            ParticleFatal("[particle] bucket " + bucket.name + " has invalid texture_slot");
        if (bucket.atlas_cols <= 0 || bucket.atlas_rows <= 0 || bucket.atlas_frames <= 0)
            ParticleFatal("[particle] bucket " + bucket.name + " has invalid atlas settings");
        if (bucket.width <= 0.0f || bucket.height <= 0.0f)
            ParticleFatal("[particle] bucket " + bucket.name + " has invalid size");
        if (bucket.scale <= 0.0f)
            ParticleFatal("[particle] bucket " + bucket.name + " has invalid scale");
        // frame_expr / uv_rect_expr are now optional. A single-frame sprite
        // (B01 droplets, M05 wisp) leaves them empty -- the runtime stamps
        // a default DrawUvRect = full quad at spawn.
        if (bucket.spawn_burst > 0 && bucket.spawn_count > 0)
            ParticleFatal("[particle] bucket " + bucket.name + " has both spawn_burst and spawn_count set");
        if (bucket.tick_hz < 0)
            ParticleFatal("[particle] bucket " + bucket.name + " has negative tick_hz");
        if (!bucket.chain_bucket.empty()
            && !FindNamedBlock(*effect, "bucket", bucket.chain_bucket))
            ParticleFatal("[particle] bucket " + bucket.name +
                          " chain_bucket='" + bucket.chain_bucket +
                          "' targets unknown bucket");
    }
    for (const SParticleEmitterEffectDef& emitter : def.emitters)
    {
        if (emitter.name.empty())
            ParticleFatal("[particle] effect " + def.name + " has unnamed emitter");
        if (emitter.outputs.empty())
            ParticleFatal("[particle] emitter " + emitter.name + " has no outputs");
        for (const SParticleEmitterOutputDef& output : emitter.outputs)
            if (!FindNamedBlock(*effect, "bucket", output.bucket))
                ParticleFatal("[particle] emitter " + emitter.name + " targets unknown bucket " + output.bucket);
    }
    return def;
}

const SParticleEffectDef& TorchFlameDef()
{
    static const SParticleEffectDef def = ParseParticleEffectDef("TorchFlame");
    return def;
}

} // namespace

static TParticleEffectManager g_particle_effect_manager;
TParticleEffectManager* ParticleEffectManager = &g_particle_effect_manager;

namespace {

SParticleBufferLayout ParticleEffectBucketLayout()
{
    SParticleBufferLayout layout = {};
    ParticleLayoutAddVar(layout, EParticleVar::OwnerId);
    ParticleLayoutAddVar(layout, EParticleVar::Life);
    ParticleLayoutAddVar(layout, EParticleVar::Age);
    ParticleLayoutAddVar(layout, EParticleVar::Age01);
    ParticleLayoutAddVar(layout, EParticleVar::Seed);
    ParticleLayoutAddVar(layout, EParticleVar::EmitPos);
    ParticleLayoutAddVar(layout, EParticleVar::EmitVel);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
    ParticleLayoutAddVar(layout, EParticleVar::DrawRot);
    ParticleLayoutAddVar(layout, EParticleVar::DrawColor);
    ParticleLayoutAddVar(layout, EParticleVar::DrawFrame);
    ParticleLayoutAddVar(layout, EParticleVar::DrawUvRect);
    return layout;
}

const SParticleBucketEffectDef* FindEffectBucketDef(const SParticleEffectDef& effect_def,
                                                    const std::string& name)
{
    for (const SParticleBucketEffectDef& bucket : effect_def.buckets)
        if (bucket.name == name)
            return &bucket;
    return nullptr;
}

EParticleBlendMode ResolveBlendMode(const std::string& s, bool legacy_additive)
{
    if (s == "alpha")              return EParticleBlendMode::Alpha;
    if (s == "additive")           return EParticleBlendMode::Additive;
    if (s == "additive_straight")  return EParticleBlendMode::AdditiveStraight;
    if (s == "premul_alpha")       return EParticleBlendMode::PremulAlpha;
    // Empty => fall back to legacy `additive` bool.
    return legacy_additive ? EParticleBlendMode::Additive : EParticleBlendMode::Alpha;
}

EParticleLightMode ResolveLightMode(const std::string& s)
{
    if (s == "lit_flat") return EParticleLightMode::LitFlat;
    return EParticleLightMode::Unlit;   // default for back-compat
}

EParticleDepthMode ResolveDepthMode(const std::string& s)
{
    if (s == "test_write") return EParticleDepthMode::TestWrite;
    if (s == "none")       return EParticleDepthMode::None;
    return EParticleDepthMode::TestNoWrite;
}

EParticleOrientation ResolveOrientation(const std::string& s)
{
    if (s == "world_xy") return EParticleOrientation::WorldXY;
    return EParticleOrientation::ScreenAligned;
}

// Resolve the bucket's texture handle. Honours bucket_def.imagery_path if
// set (B01/M05 patterns that load a non-owner imagery via TObjectImagery::
// LoadImagery), otherwise falls back to the owner imagery's texture_slot.
struct SBucketTextureBinding
{
    TTextureHandle handle = kInvalidTexture;
    int32_t width = 0;
    int32_t height = 0;
};

SBucketTextureBinding ResolveBucketTexture(const SParticleBucketEffectDef& bucket_def,
                                           TObjectInstance* owner)
{
    SBucketTextureBinding binding = {};
    if (!bucket_def.imagery_path.empty())
    {
        const int32_t img_id = TObjectImagery::FindImagery(bucket_def.imagery_path.c_str());
        if (img_id < 0)
        {
            char err[256];
            snprintf(err, sizeof(err),
                     "[particle] bucket %s: imagery_path='%s' FindImagery failed",
                     bucket_def.name.c_str(), bucket_def.imagery_path.c_str());
            FatalError(err);
        }
        TObjectImagery* imagery = TObjectImagery::LoadImagery(img_id);
        T3DImagery* img3d = dynamic_cast<T3DImagery*>(imagery);
        if (!img3d)
        {
            char err[256];
            snprintf(err, sizeof(err),
                     "[particle] bucket %s: imagery_path='%s' is not a T3DImagery",
                     bucket_def.name.c_str(), bucket_def.imagery_path.c_str());
            FatalError(err);
        }
        (void)img3d->NumObjects();  // force lazy mesh init so texture slots populate
        S3DTex tex = {};
        img3d->GetTexture(bucket_def.texture_slot, &tex);
        binding.handle = tex.htexture;
        binding.width  = int32_t(tex.desc.width);
        binding.height = int32_t(tex.desc.height);
        return binding;
    }

    // Owner-imagery path (TorchFlame): use the owner's imagery, demand it
    // exists and has the requested slot populated.
    T3DImagery* img3d = owner ? dynamic_cast<T3DImagery*>(owner->GetImagery()) : nullptr;
    if (!img3d || img3d->NumTextures() <= 0)
        FatalError("[particle] effect requires 3D imagery with at least one texture (set imagery_path= for owner-less buckets)");
    S3DTex tex = {};
    img3d->GetTexture(bucket_def.texture_slot, &tex);
    binding.handle = tex.htexture;
    binding.width  = int32_t(tex.desc.width);
    binding.height = int32_t(tex.desc.height);
    return binding;
}

SParticleBucketDesc BuildRuntimeBucketDesc(const SParticleEffectDef& effect_def,
                                           const SParticleBucketEffectDef& bucket_def,
                                           TObjectInstance* owner)
{
    const SBucketTextureBinding tex = ResolveBucketTexture(bucket_def, owner);
    if (tex.handle == kInvalidTexture)
        FatalError("[particle] effect bucket texture handle is invalid");
    if (tex.width <= 0 || tex.height <= 0)
    {
        char err[256];
        snprintf(err, sizeof(err),
                 "[particle] effect bucket texture dimensions invalid (%dx%d)",
                 tex.width, tex.height);
        FatalError(err);
    }

    SParticleBucketDesc desc = {};
    desc.name = bucket_def.name;
    desc.blend = ResolveBlendMode(bucket_def.blend_mode, bucket_def.additive);
    desc.light_mode = ResolveLightMode(bucket_def.light_mode);
    desc.depth_mode = ResolveDepthMode(bucket_def.depth_mode);
    desc.orientation = ResolveOrientation(bucket_def.orientation);
    desc.sort = EParticleSortMode::None;
    desc.texture = tex.handle;
    desc.texture_width = tex.width;
    desc.texture_height = tex.height;
    desc.frame_cols = bucket_def.atlas_cols;
    desc.frame_rows = bucket_def.atlas_rows;
    desc.default_width = bucket_def.width;
    desc.default_height = bucket_def.height;
    desc.debug_solid = effect_def.debug_solid;
    desc.flip_v = bucket_def.flip_v;
    desc.chroma_key = bucket_def.chroma_key;
    desc.chroma_key_rgb[0] = bucket_def.chroma_key_rgb[0];
    desc.chroma_key_rgb[1] = bucket_def.chroma_key_rgb[1];
    desc.chroma_key_rgb[2] = bucket_def.chroma_key_rgb[2];
    return desc;
}

void InitializeRuntimeParticle(TParticleBucket& bucket, int32_t particle_index,
                               const SParticleBucketEffectDef& bucket_def,
                               const SParticleEmitterEffectDef& emitter,
                               TObjectInstance* owner)
{
    if (float* draw_pos = bucket.VarPtr(particle_index, EParticleVar::DrawPos))
    {
        const S3DPoint& p = owner->Pos();
        draw_pos[0] = float(p.x) + emitter.local_pos[0];
        draw_pos[1] = float(p.y) + emitter.local_pos[1];
        draw_pos[2] = float(p.z) + emitter.local_pos[2];
    }
    if (float* emit_pos = bucket.VarPtr(particle_index, EParticleVar::EmitPos))
    {
        emit_pos[0] = emitter.local_pos[0];
        emit_pos[1] = emitter.local_pos[1];
        emit_pos[2] = emitter.local_pos[2];
    }
    if (float* draw_scl = bucket.VarPtr(particle_index, EParticleVar::DrawScl))
    {
        draw_scl[0] = bucket_def.width * bucket_def.scale;
        draw_scl[1] = bucket_def.height * bucket_def.scale;
        draw_scl[2] = 1.0f;
    }
    if (float* uv_rect = bucket.VarPtr(particle_index, EParticleVar::DrawUvRect))
    {
        if (!bucket_def.uv_rect_expr.empty())
        {
            TParticleExpression expr;
            std::string error;
            if (!expr.Compile(bucket_def.uv_rect_expr.c_str(), &error))
                ParticleFatal("[particle] bucket " + bucket_def.name + " uv_rect expression compile failed: " + error);
            SParticleEvalContext ctx = {};
            ctx.time_frame = float(TTime::Time());
            expr.Eval(ctx, uv_rect, 4);
        }
        else
        {
            // Default: full quad (single-frame sprite, no atlas).
            uv_rect[0] = 0.0f;
            uv_rect[1] = 0.0f;
            uv_rect[2] = 1.0f;
            uv_rect[3] = 1.0f;
        }
    }
    // Default DrawColor = opaque white (lets spawn_expr override per particle).
    if (float* color = bucket.VarPtr(particle_index, EParticleVar::DrawColor))
    {
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
    }
}

// Default-init the per-particle slots a self-spawning bucket needs, then
// run the bucket's compiled spawn_expr to override. Used by EmitSpawnTopup.
void InitializeSelfSpawnParticle(TParticleBucket& bucket, int32_t particle_index,
                                 const SParticleBucketEffectDef& bucket_def,
                                 const TParticleExpression& spawn_expr,
                                 bool has_spawn_expr,
                                 TObjectInstance* owner)
{
    const S3DPoint& p = owner ? owner->Pos() : S3DPoint{0, 0, 0};
    const float emit_pos[3] = { float(p.x), float(p.y), float(p.z) };
    const float emit_vel[3] = { 0.0f, 0.0f, 0.0f };

    // Pre-spawn defaults: pos at owner, white opaque, unit scale, frame 0.
    if (float* dp = bucket.VarPtr(particle_index, EParticleVar::DrawPos))
    { dp[0] = emit_pos[0]; dp[1] = emit_pos[1]; dp[2] = emit_pos[2]; }
    if (float* ep = bucket.VarPtr(particle_index, EParticleVar::EmitPos))
    { ep[0] = emit_pos[0]; ep[1] = emit_pos[1]; ep[2] = emit_pos[2]; }
    if (float* ev = bucket.VarPtr(particle_index, EParticleVar::EmitVel))
    { ev[0] = 0.0f; ev[1] = 0.0f; ev[2] = 0.0f; }
    if (float* ds = bucket.VarPtr(particle_index, EParticleVar::DrawScl))
    { ds[0] = bucket_def.width * bucket_def.scale;
      ds[1] = bucket_def.height * bucket_def.scale;
      ds[2] = 1.0f; }
    if (float* dr = bucket.VarPtr(particle_index, EParticleVar::DrawRot))
        *dr = 0.0f;
    if (float* dc = bucket.VarPtr(particle_index, EParticleVar::DrawColor))
    { dc[0] = 1.0f; dc[1] = 1.0f; dc[2] = 1.0f; dc[3] = 1.0f; }
    if (float* age = bucket.VarPtr(particle_index, EParticleVar::Age))
        *age = 0.0f;
    if (float* age01 = bucket.VarPtr(particle_index, EParticleVar::Age01))
        *age01 = 0.0f;
    // Seed: deterministic-per-slot mix of particle_index + bucket_def address.
    // Computed BEFORE DrawFrame so random_start_frame can use it.
    float seed_val = 0.0f;
    if (float* seed = bucket.VarPtr(particle_index, EParticleVar::Seed))
    {
        uint32_t s = uint32_t(particle_index) * 2654435761u
                   + uint32_t(uintptr_t(&bucket_def) & 0xffffffffu) * 16807u;
        if (s == 0) s = 1;
        *seed = float(s & 0x7fffffu) / float(0x7fffffu);
        seed_val = *seed;
    }
    // DrawFrame + DrawUvRect: pick atlas cell now and write the explicit
    // UV rect (the renderer's UV priority is DrawUvRect > atlas-from-frame,
    // so we must materialize the cell UV here — leaving DrawUvRect at the
    // default (0,0,1,1) would override the atlas pick and render the whole
    // sheet). When the bucket has no atlas grid (cells == 1), DrawUvRect
    // stays at full-quad.
    const int32_t atlas_cells = bucket_def.atlas_cols * bucket_def.atlas_rows;
    int32_t spawn_frame = 0;
    if (bucket_def.random_start_frame && bucket_def.atlas_frames > 1)
        spawn_frame = int32_t(std::floor(seed_val * float(bucket_def.atlas_frames)));
    if (float* df = bucket.VarPtr(particle_index, EParticleVar::DrawFrame))
        *df = float(spawn_frame);
    if (float* uv = bucket.VarPtr(particle_index, EParticleVar::DrawUvRect))
    {
        if (atlas_cells > 1)
        {
            const int32_t cell  = ((spawn_frame % atlas_cells) + atlas_cells) % atlas_cells;
            const int32_t fx    = cell % bucket_def.atlas_cols;
            const int32_t fy    = cell / bucket_def.atlas_cols;
            const float cw      = 1.0f / float(bucket_def.atlas_cols);
            const float ch      = 1.0f / float(bucket_def.atlas_rows);
            uv[0] = float(fx) * cw;
            uv[1] = float(fy) * ch;
            uv[2] = cw;
            uv[3] = ch;
        }
        else
        {
            uv[0] = 0.0f; uv[1] = 0.0f; uv[2] = 1.0f; uv[3] = 1.0f;
        }
    }

    // Now run the user spawn expression (statement-form). Identifier
    // aliases pos/vel/color/scale etc. resolve against the bucket slots
    // we just pre-initialised — spawn_expr can override random_start_frame
    // by writing to `frame` explicitly.
    if (has_spawn_expr)
    {
        SParticleEvalContext ctx = {};
        ctx.time_frame = float(TTime::Time());
        spawn_expr.EvalParticle(ctx, bucket, particle_index, emit_pos, emit_vel);
    }
}

void UpdateRuntimeParticles(const SParticleEffectDef& effect_def, float owner_particle_id,
                            TObjectInstance* owner)
{
    if (!owner)
        return;

    for (const SParticleEmitterEffectDef& emitter : effect_def.emitters)
    {
        for (const SParticleEmitterOutputDef& output : emitter.outputs)
        {
            const SParticleBucketEffectDef* bucket_def = FindEffectBucketDef(effect_def, output.bucket);
            TParticleBucket* bucket = ParticleManager().FindGlobalBucket(output.bucket.c_str());
            if (!bucket_def || !bucket)
                continue;

            for (int32_t particle_index = 0; particle_index < bucket->Count(); ++particle_index)
            {
                const float* owner_id = bucket->VarPtr(particle_index, EParticleVar::OwnerId);
                if (!owner_id || *owner_id != owner_particle_id)
                    continue;
                InitializeRuntimeParticle(*bucket, particle_index, *bucket_def, emitter, owner);
            }
        }
    }
}

} // namespace

TParticleEffectManager::SRuntime* TParticleEffectManager::FindRuntime(
    const SParticleEffectDef* effect_def, int32_t owner_map_index)
{
    if (!effect_def || owner_map_index < 0)
        return nullptr;
    for (SRuntime& runtime : runtimes)
        if (runtime.effect_def == effect_def && runtime.owner.MapIndex() == owner_map_index)
            return &runtime;
    return nullptr;
}

void TParticleEffectManager::PulseEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner)
{
    if (!effect_def || !owner)
        return;

    const int32_t owner_map_index = owner->GetMapIndex();
    SRuntime* runtime = FindRuntime(effect_def, owner_map_index);
    if (!runtime)
    {
        SRuntime new_runtime = {};
        new_runtime.effect_def = effect_def;
        new_runtime.owner = owner;
        new_runtime.owner_particle_id = float(owner_map_index);
        runtimes.push_back(std::move(new_runtime));
        runtime = &runtimes.back();
        StartRuntime(*runtime, owner);
    }
    runtime->last_draw_pulse_pass = ParticleManager().DrawPulsePass();
    UpdateRuntimeParticles(*effect_def, runtime->owner_particle_id, owner);
}

void TParticleEffectManager::StopEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner)
{
    const int32_t owner_map_index = owner ? owner->GetMapIndex() : -1;
    for (size_t i = 0; i < runtimes.size(); ++i)
    {
        if (runtimes[i].effect_def == effect_def && runtimes[i].owner.MapIndex() == owner_map_index)
        {
            StopRuntime(i);
            return;
        }
    }
}

void TParticleEffectManager::ExpireInactiveEffects()
{
    const uint32_t current_pass = ParticleManager().DrawPulsePass();
    if (last_expire_pass == current_pass)
        return;
    last_expire_pass = current_pass;

    for (size_t i = 0; i < runtimes.size(); )
    {
        SRuntime& runtime = runtimes[i];
        const bool owner_dead = !runtime.owner.Get();
        const uint32_t ttl = runtime.effect_def ? runtime.effect_def->inactive_ttl_pulses : 1;
        const bool expired = current_pass != runtime.last_draw_pulse_pass &&
            uint32_t(current_pass - runtime.last_draw_pulse_pass) > ttl;
        if (owner_dead || expired)
            StopRuntime(i);
        else
            ++i;
    }
}

void TParticleEffectManager::Clear()
{
    runtimes.clear();
    last_expire_pass = 0;
}

// Top up a bucket's particle population. For spawn_burst, called once at
// StartRuntime; for spawn_count, called each frame from IntegrateEffect
// to restock after kill_expr fires. The number to emit is the difference
// between the target count and how many live particles this runtime owns.
void TParticleEffectManager::EmitSpawnTopup(SBucketRuntime& brt, TObjectInstance* owner, float owner_particle_id)
{
    if (!brt.bucket || !brt.bucket_def) return;

    int32_t target = 0;
    if (brt.bucket_def->spawn_burst > 0 && brt.spawned_burst == 0)
        target = brt.bucket_def->spawn_burst;
    else if (brt.bucket_def->spawn_count > 0)
    {
        // Count live particles owned by this runtime.
        int32_t live = 0;
        for (int32_t i = 0; i < brt.bucket->Count(); ++i)
        {
            const float* o = brt.bucket->VarPtr(i, EParticleVar::OwnerId);
            if (o && *o == owner_particle_id) ++live;
        }
        target = brt.bucket_def->spawn_count - live;
    }

    for (int32_t k = 0; k < target; ++k)
    {
        const float life = brt.bucket_def->default_life > 0.0f ? brt.bucket_def->default_life : -1.0f;
        const int32_t pi = brt.bucket->AddParticle(owner_particle_id, life);
        if (pi < 0) break;
        InitializeSelfSpawnParticle(*brt.bucket, pi, *brt.bucket_def,
                                    brt.spawn_expr, brt.spawn_compiled, owner);
    }
}

TParticleEffectManager::SBucketRuntime* TParticleEffectManager::FindBucketRuntime(
    SRuntime& runtime, const std::string& bucket_name)
{
    for (SBucketRuntime& brt : runtime.bucket_runtimes)
        if (brt.bucket_def && brt.bucket_def->name == bucket_name)
            return &brt;
    return nullptr;
}

// Spawn one particle in the chain bucket using the dying particle's pos /
// vel as the chain spawn's emit_pos / emit_vel. The chain bucket's
// spawn_expr runs against the chain particle's own slots, with the
// supplied dying-pos/vel exposed as the read-only emit_pos / emit_vel
// identifiers (this is exactly how the legacy emitter/output path
// supplies emit_pos / emit_vel to spawn_expr).
//
// Reflection-plane / chain-bucket mechanism: blood's FLY droplet hits
// ground (kill_expr fires on pos.z <= emit_pos.z), the engine reaps the
// flier AND spawns one SPLAT into the chain bucket at the impact site.
// Generic — any "die into X" effect uses the same wire.
int32_t TParticleEffectManager::EmitChainParticle(SBucketRuntime& chain_brt,
                                                  TObjectInstance* owner,
                                                  float owner_particle_id,
                                                  const float dying_pos[3],
                                                  const float dying_vel[3])
{
    if (!chain_brt.bucket || !chain_brt.bucket_def)
        return -1;

    const float life = chain_brt.bucket_def->default_life > 0.0f
                           ? chain_brt.bucket_def->default_life : -1.0f;
    const int32_t pi = chain_brt.bucket->AddParticle(owner_particle_id, life);
    if (pi < 0)
        return -1;

    // Pre-init the chain particle's slots to sensible defaults (matches
    // InitializeSelfSpawnParticle's defaults). Then run the chain
    // bucket's spawn_expr — but with the dying particle's pos / vel
    // shadowed into emit_pos / emit_vel so the chain spawn can place
    // itself at the impact site naturally.
    TParticleBucket& bucket = *chain_brt.bucket;
    const SParticleBucketEffectDef& bd = *chain_brt.bucket_def;

    if (float* dp = bucket.VarPtr(pi, EParticleVar::DrawPos))
    { dp[0] = dying_pos[0]; dp[1] = dying_pos[1]; dp[2] = dying_pos[2]; }
    if (float* ep = bucket.VarPtr(pi, EParticleVar::EmitPos))
    { ep[0] = dying_pos[0]; ep[1] = dying_pos[1]; ep[2] = dying_pos[2]; }
    if (float* ev = bucket.VarPtr(pi, EParticleVar::EmitVel))
    { ev[0] = dying_vel[0]; ev[1] = dying_vel[1]; ev[2] = dying_vel[2]; }
    if (float* ds = bucket.VarPtr(pi, EParticleVar::DrawScl))
    { ds[0] = bd.width * bd.scale; ds[1] = bd.height * bd.scale; ds[2] = 1.0f; }
    if (float* dr = bucket.VarPtr(pi, EParticleVar::DrawRot))
        *dr = 0.0f;
    if (float* dc = bucket.VarPtr(pi, EParticleVar::DrawColor))
    { dc[0] = 1.0f; dc[1] = 1.0f; dc[2] = 1.0f; dc[3] = 1.0f; }
    if (float* age = bucket.VarPtr(pi, EParticleVar::Age))
        *age = 0.0f;
    if (float* age01 = bucket.VarPtr(pi, EParticleVar::Age01))
        *age01 = 0.0f;
    float chain_seed_val = 0.0f;
    if (float* seed = bucket.VarPtr(pi, EParticleVar::Seed))
    {
        // Seed off the chain particle's index + the dying particle's
        // pos.x so successive chain spawns don't collide.
        uint32_t s = uint32_t(pi) * 2654435761u
                   + uint32_t(int32_t(dying_pos[0])) * 16807u;
        if (s == 0) s = 1;
        *seed = float(s & 0x7fffffu) / float(0x7fffffu);
        chain_seed_val = *seed;
    }
    // DrawFrame + DrawUvRect: same atlas-cell materialization as self-spawn
    // (the renderer's UV priority is DrawUvRect > atlas-from-frame, so we
    // must write the cell UV explicitly here).
    const int32_t chain_atlas_cells = bd.atlas_cols * bd.atlas_rows;
    int32_t chain_spawn_frame = 0;
    if (bd.random_start_frame && bd.atlas_frames > 1)
        chain_spawn_frame = int32_t(std::floor(chain_seed_val * float(bd.atlas_frames)));
    if (float* df = bucket.VarPtr(pi, EParticleVar::DrawFrame))
        *df = float(chain_spawn_frame);
    if (float* uv = bucket.VarPtr(pi, EParticleVar::DrawUvRect))
    {
        if (chain_atlas_cells > 1)
        {
            const int32_t cell  = ((chain_spawn_frame % chain_atlas_cells) + chain_atlas_cells) % chain_atlas_cells;
            const int32_t fx    = cell % bd.atlas_cols;
            const int32_t fy    = cell / bd.atlas_cols;
            const float cw      = 1.0f / float(bd.atlas_cols);
            const float ch      = 1.0f / float(bd.atlas_rows);
            uv[0] = float(fx) * cw;
            uv[1] = float(fy) * ch;
            uv[2] = cw;
            uv[3] = ch;
        }
        else
        {
            uv[0] = 0.0f; uv[1] = 0.0f; uv[2] = 1.0f; uv[3] = 1.0f;
        }
    }

    if (chain_brt.spawn_compiled)
    {
        SParticleEvalContext ctx = {};
        ctx.time_frame = float(TTime::Time());
        // dying_pos / dying_vel become the emit_pos / emit_vel identifiers
        // for this chain spawn — the chain bucket's spawn_expr can read
        // emit_pos / emit_vel to inherit the impact site.
        chain_brt.spawn_expr.EvalParticle(ctx, bucket, pi, dying_pos, dying_vel);
    }

    (void)owner;
    return pi;
}

// Per-bucket integration. dt_seconds is wall-clock for this render frame.
//
// UNIT MODEL: explicit-tick-multiplier.
//
//   The engine's contract with bucket authors is:
//
//   * tick_hz > 0 (the normal case): tick_expr is invoked exactly tick_hz
//     times per simulated second, regardless of render FPS. Leftover ms
//     accumulate in sim_accum_ms across render frames and produce additional
//     integer ticks on subsequent frames. tick_expr is NOT given dt and is
//     NOT scaled per call -- authors write velocity / acceleration literals
//     in PER-TICK units (wu/tick, wu/tick^2). One tick_expr call == one
//     tick's worth of motion.
//
//   * tick_hz == 0: tick_expr runs once per render frame. Motion authored
//     under tick_hz==0 is frame-rate-dependent; prefer tick_hz>0 for any
//     bucket whose dynamics need to be wall-clock-consistent.
//
//   * Age / Life / Age01 are always in SECONDS. Age is advanced by
//     per_tick_seconds (= 1/tick_hz when tick_hz>0) per tick, and Life is
//     stored as the bucket's default_life value in seconds. Age01 = Age/Life.
//     The auto-kill at the bottom of this loop fires when Age >= Life in
//     seconds.
//
//   In short: tick_expr literals are PER-TICK; default_life / Age / Age01
//   are SECONDS. Authors converting between the two convert by hand using
//   tick_hz (e.g. blood_splat's SHRINK comment shows 0.4*1.6s*24Hz = 15.4
//   ticks).
void TParticleEffectManager::IntegrateBucket(SRuntime& runtime, SBucketRuntime& brt,
                                             TObjectInstance* owner, float dt_seconds,
                                             float owner_particle_id)
{
    if (!brt.bucket || !brt.bucket_def) return;
    if (!brt.tick_compiled && !brt.kill_compiled && brt.bucket_def->default_life <= 0.0f
        && brt.bucket_def->spawn_count == 0
        && brt.bucket_def->chain_bucket.empty())
    {
        // Nothing to do -- no per-tick dynamics, no life-based kill,
        // no spawn-topup restock target, no chain emission. The legacy
        // emitter/output path (TorchFlame) hits this branch.
        return;
    }

    // Compute number of integration ticks this frame.
    int32_t ticks = 1;
    float per_tick_seconds = dt_seconds;
    if (brt.bucket_def->tick_hz > 0)
    {
        const double tick_ms = 1000.0 / double(brt.bucket_def->tick_hz);
        brt.sim_accum_ms += double(dt_seconds) * 1000.0;
        ticks = 0;
        while (brt.sim_accum_ms >= tick_ms)
        {
            brt.sim_accum_ms -= tick_ms;
            ++ticks;
        }
        per_tick_seconds = float(tick_ms / 1000.0);
    }

    if (ticks == 0)
        return;

    const S3DPoint& p = owner ? owner->Pos() : S3DPoint{0, 0, 0};
    const float emit_pos[3] = { float(p.x), float(p.y), float(p.z) };
    const float emit_vel[3] = { 0.0f, 0.0f, 0.0f };

    SParticleEvalContext base_ctx = {};
    base_ctx.time_frame = float(TTime::Time());

    // Per-tick: walk the bucket, integrate live particles owned by this
    // runtime, run kill_expr, respawn or reap as appropriate.
    for (int32_t t = 0; t < ticks; ++t)
    {
        for (int32_t i = brt.bucket->Count() - 1; i >= 0; --i)
        {
            const float* owner_slot = brt.bucket->VarPtr(i, EParticleVar::OwnerId);
            if (!owner_slot || *owner_slot != owner_particle_id)
                continue;

            // Advance age.
            float* age = brt.bucket->VarPtr(i, EParticleVar::Age);
            float* age01 = brt.bucket->VarPtr(i, EParticleVar::Age01);
            float* life = brt.bucket->VarPtr(i, EParticleVar::Life);
            if (age) *age += per_tick_seconds;
            if (age && life && *life > 0.0f && age01)
            {
                const float t = *age / *life;
                *age01 = t < 1.0f ? t : 1.0f;
            }
            else if (age01) *age01 = 0.0f;

            SParticleEvalContext ctx = base_ctx;
            if (age)   ctx.age   = *age;
            if (age01) ctx.age01 = *age01;
            // Pull the current seed into ctx so EvalParticle's rng_state
            // initialisation reads it.
            if (const float* seed = brt.bucket->VarPtr(i, EParticleVar::Seed))
                ctx.seed = *seed;

            // Run the tick body.
            if (brt.tick_compiled)
                brt.tick_expr.EvalParticle(ctx, *brt.bucket, i, emit_pos, emit_vel);

            // kill_expr -> respawn (re-run spawn_expr, zero age).
            bool kill = false;
            if (brt.kill_compiled)
                kill = brt.kill_expr.EvalParticlePredicate(ctx, *brt.bucket, i, emit_pos, emit_vel);
            if (!kill && life && *life > 0.0f && age && *age >= *life)
                kill = true;

            if (kill)
            {
                // Chain emission: if the bucket defines a chain_bucket,
                // spawn one particle in the target bucket using this
                // particle's pos / vel as the chain spawn's emit_pos /
                // emit_vel. This is the reflection-plane / state-
                // transition mechanism — blood uses it for FLY droplet
                // -> SPLAT decal. Generic.
                if (!brt.bucket_def->chain_bucket.empty())
                {
                    SBucketRuntime* chain_brt =
                        FindBucketRuntime(runtime, brt.bucket_def->chain_bucket);
                    if (chain_brt)
                    {
                        float dying_pos[3] = {0, 0, 0};
                        float dying_vel[3] = {0, 0, 0};
                        if (const float* dp = brt.bucket->VarPtr(i, EParticleVar::DrawPos))
                        { dying_pos[0] = dp[0]; dying_pos[1] = dp[1]; dying_pos[2] = dp[2]; }
                        if (const float* dv = brt.bucket->VarPtr(i, EParticleVar::EmitVel))
                        { dying_vel[0] = dv[0]; dying_vel[1] = dv[1]; dying_vel[2] = dv[2]; }
                        EmitChainParticle(*chain_brt, owner, owner_particle_id,
                                          dying_pos, dying_vel);
                    }
                }

                if (brt.bucket_def->spawn_count > 0)
                {
                    // Steady-state: respawn in place by re-running spawn_expr
                    // and resetting age. The particle stays in the bucket.
                    if (age)   *age = 0.0f;
                    if (age01) *age01 = 0.0f;
                    InitializeSelfSpawnParticle(*brt.bucket, i, *brt.bucket_def,
                                                brt.spawn_expr, brt.spawn_compiled, owner);
                }
                else
                {
                    // One-shot burst (or chain-source bucket): tag the
                    // particle dead via owner_id mutation, then reap below.
                    if (float* mut_o = brt.bucket->VarPtr(i, EParticleVar::OwnerId))
                        *mut_o = -7777.0f;
                }
            }
        }
        brt.bucket->KillParticlesByOwner(-7777.0f);
    }

    // Steady-state restock (covers the case where a particle was reaped
    // mid-tick by something other than the respawn path, or the bucket
    // started under-populated).
    if (brt.bucket_def->spawn_count > 0)
        EmitSpawnTopup(brt, owner, owner_particle_id);
}

void TParticleEffectManager::IntegrateEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner, float dt_seconds)
{
    if (!effect_def || !owner) return;
    SRuntime* runtime = FindRuntime(effect_def, owner->GetMapIndex());
    if (!runtime) return;
    for (SBucketRuntime& brt : runtime->bucket_runtimes)
        IntegrateBucket(*runtime, brt, owner, dt_seconds, runtime->owner_particle_id);
}

void TParticleEffectManager::StartRuntime(SRuntime& runtime, TObjectInstance* owner)
{
    if (!runtime.effect_def || !owner)
        return;

    runtime.buckets.clear();
    runtime.bucket_runtimes.clear();
    const SParticleBufferLayout layout = ParticleEffectBucketLayout();
    for (const SParticleBucketEffectDef& bucket_def : runtime.effect_def->buckets)
    {
        SParticleBucketDesc desc = BuildRuntimeBucketDesc(*runtime.effect_def, bucket_def, owner);
        TParticleBucket* bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
        runtime.buckets.push_back(bucket);

        // Build per-bucket compiled-expression cache. Buckets without
        // spawn/tick/kill expressions stay all-default (only owns the
        // bucket ptr for the integration loop's spawn-topup path).
        SBucketRuntime brt = {};
        brt.bucket = bucket;
        brt.bucket_def = &bucket_def;
        std::string err;
        if (!bucket_def.spawn_expr.empty())
        {
            brt.spawn_compiled = brt.spawn_expr.Compile(
                bucket_def.spawn_expr.c_str(), &err, EParticleExprMode::Statements);
            if (!brt.spawn_compiled)
                ParticleFatal("[particle] bucket " + bucket_def.name + " spawn_expr compile failed: " + err);
        }
        if (!bucket_def.tick_expr.empty())
        {
            brt.tick_compiled = brt.tick_expr.Compile(
                bucket_def.tick_expr.c_str(), &err, EParticleExprMode::Statements);
            if (!brt.tick_compiled)
                ParticleFatal("[particle] bucket " + bucket_def.name + " tick_expr compile failed: " + err);
        }
        if (!bucket_def.kill_expr.empty())
        {
            brt.kill_compiled = brt.kill_expr.Compile(
                bucket_def.kill_expr.c_str(), &err, EParticleExprMode::Expression);
            if (!brt.kill_compiled)
                ParticleFatal("[particle] bucket " + bucket_def.name + " kill_expr compile failed: " + err);
        }
        runtime.bucket_runtimes.push_back(std::move(brt));
    }

    // Legacy emitter/output path -- still drives TorchFlame. New
    // self-spawning buckets (spawn_burst / spawn_count) are handled by
    // IntegrateEffect's spawn-topup.
    for (const SParticleEmitterEffectDef& emitter : runtime.effect_def->emitters)
    {
        for (const SParticleEmitterOutputDef& output : emitter.outputs)
        {
            const SParticleBucketEffectDef* bucket_def = FindEffectBucketDef(*runtime.effect_def, output.bucket);
            if (!bucket_def)
                continue;

            TParticleBucket* bucket = ParticleManager().FindGlobalBucket(output.bucket.c_str());
            if (!bucket)
                continue;

            const int32_t particle_index = bucket->AddParticle(runtime.owner_particle_id, output.life);
            if (particle_index >= 0)
                InitializeRuntimeParticle(*bucket, particle_index, *bucket_def, emitter, owner);
        }
    }

    // One-shot burst AND steady-state population: emit at attach time so
    // the bucket is visible immediately (the integration loop's per-frame
    // top-up handles restocks after kill_expr fires).
    for (SBucketRuntime& brt : runtime.bucket_runtimes)
    {
        if (brt.bucket_def && brt.bucket_def->spawn_burst > 0 && brt.spawned_burst == 0)
        {
            EmitSpawnTopup(brt, owner, runtime.owner_particle_id);
            brt.spawned_burst = brt.bucket_def->spawn_burst;
        }
        else if (brt.bucket_def && brt.bucket_def->spawn_count > 0)
        {
            EmitSpawnTopup(brt, owner, runtime.owner_particle_id);
        }
    }
}

void TParticleEffectManager::StopRuntime(size_t runtime_index)
{
    if (runtime_index >= runtimes.size())
        return;

    SRuntime& runtime = runtimes[runtime_index];
    // Runtime removal stops the effect system. Buckets themselves are retained;
    // they are storage/render batches and may still contain particles from
    // other systems, or future timed particles that outlive their emitter.
    for (TParticleBucket* bucket : runtime.buckets)
        if (bucket)
            bucket->KillParticlesByOwner(runtime.owner_particle_id);

    runtimes[runtime_index] = std::move(runtimes.back());
    runtimes.pop_back();
}

void TParticleEffectComponent::DrawPulse()
{
    if (!ParticleEffectManager || !effect_def || !Owner())
        return;
    ParticleEffectManager->PulseEffect(effect_def, Owner());
    // Per-frame integration: tick the runtime's bucket expressions
    // (spawn_expr / tick_expr / kill_expr) against the live owner. dt is
    // the shared render-frame delta -- buckets with tick_hz > 0 accumulate
    // and integrate at their fixed cadence; others integrate once per
    // render frame.
    const float dt = float(TTime::DeltaTime());
    ParticleEffectManager->IntegrateEffect(effect_def, Owner(), dt);
}

TObjectClass EffectClass("EFFECT", OBJCLASS_EFFECT, 0);

class TGenericEffectBuilder : public TObjectBuilder
{
  public:
    TGenericEffectBuilder(const char* name) : TObjectBuilder(name) {}
    TObjectInstance* Build(TObjectImagery* img) override { return new TEffect(img); }
    TObjectInstance* Build(SObjectDef* def, TObjectImagery* img) override { return new TEffect(def, img); }
};

static TGenericEffectBuilder g_effect_builder("EFFECT");
static TGenericEffectBuilder g_effect_builder_lower("effect");
static TGenericEffectBuilder g_effect_stillwater_builder("StillWater");
static TGenericEffectBuilder g_effect_flowwater_builder("FlowWater");
static TGenericEffectBuilder g_effect_bendwater1_builder("BendWater1");
static TGenericEffectBuilder g_effect_bendwater2_builder("BendWater2");
static TGenericEffectBuilder g_effect_sewerwater_builder("SewerWater");
static TGenericEffectBuilder g_effect_wave_builder("Wave");
static TGenericEffectBuilder g_effect_waves_builder("WaveS");
static TGenericEffectBuilder g_effect_wavem_builder("WaveM");
static TGenericEffectBuilder g_effect_mistfog_builder("MistFog");
static TGenericEffectBuilder g_effect_pixie_builder("Pixie");
static TGenericEffectBuilder g_effect_speaker_builder("speaker");
static TGenericEffectBuilder g_effect_speaker_caps_builder("Speaker");

void TEffect::Pulse()
{
    TObjectInstance::Pulse();
    SetFrame(0);
    SetCommandDone(false);
}

void TEffect::KillThisEffect()
{
    flags |= OF_KILL | OF_PULSE;
}

int32_t TEffect::GetAngle()
{
    return angle;
}

void TBurnEffect::Set(PTCharacter ch)
{
    character = ch;
    if (character)
    {
        S3DPoint pos;
        character->GetPos(pos);
        SetPos(pos);
    }
}

void TBurnEffect::ResetFrameCount()
{
    // Phase 3: delegate to TBurnAnimator once the burn animator is ported.
}

void TPulpEffect::Set(S3DPoint /*vel*/, PTCharacter ch,
                      int32_t /*num_body_part*/, int32_t /*num_blood*/,
                      int32_t /*num_splat*/)
{
    character = ch;
}

bool SaveBlendState()    { return true; }
bool SetBlendState()     { return true; }
bool RestoreBlendState() { return true; }

void TFlipbookBillboardComponent::Submit(TRenderer& renderer, const TObjectInstance& inst) const
{
    if (texture_handle == kInvalidTexture)
        return;

    SBillboardDrawItem item = {};
    const S3DPoint& p = inst.Pos();
    item.world_pos[0] = float(p.x);
    item.world_pos[1] = float(p.y);
    item.world_pos[2] = float(p.z);
    item.size_wu[0]   = size_w;
    item.size_wu[1]   = size_h;
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    UvRect(item.uv_rect);
    item.key.texture     = texture_handle;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(additive_blend ? EFxBlend::Additive : EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    renderer.SubmitFxBillboard(item);
}

DEFINE_BUILDER("FLAME", TFlameEffect)
REGISTER_BUILDER(TFlameEffect)

void TFlameEffect::InitializeVisualComponent(TObjectImagery* imagery)
{
    AttachVisualComponent(this, imagery);
}

void TFlameEffect::AttachVisualComponent(TObjectInstance* inst, TObjectImagery* imagery)
{
    if (!inst)
        return;

    const SParticleEffectDef& def = TorchFlameDef();
    const SParticleBucketEffectDef& primary_bucket = def.buckets.front();
    const bool needs_flipbook = !inst->GetComponent<TFlipbookBillboardComponent>();
    const bool needs_particle_effect = !inst->GetComponent<TParticleEffectComponent>();
    if (!needs_flipbook && !needs_particle_effect)
        return;

    T3DImagery* img3d = dynamic_cast<T3DImagery*>(imagery);
    if (!img3d)
        return;
    // Force lazy mesh init. NumTextures() itself is not init-triggering
    // (texture slots are only populated by InitializeMesh / OldInitializeMesh),
    // but NumObjects() is — so a quick poke through that path materializes
    // the texture slots before we read them. Otherwise spawning a flame
    // via SpawnForTest immediately after LoadImagery sees NumTextures==0
    // and silently bails.
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        // Constructors can run before T3DImagery has initialized texture
        // slots. The animator-builder hook calls us again after mesh init.
        return;
    }

    S3DTex primary_tex = {};
    img3d->GetTexture(primary_bucket.texture_slot, &primary_tex);
    if (primary_tex.htexture == kInvalidTexture)
        FatalError("[particle] FLAME effect texture handle is invalid");

    const int32_t primary_texture_width = int32_t(primary_tex.desc.width);
    const int32_t primary_texture_height = int32_t(primary_tex.desc.height);
    if (primary_texture_width <= 0 || primary_texture_height <= 0)
    {
        char err[256];
        snprintf(err, sizeof(err),
                 "[particle] FLAME effect texture dimensions invalid desc=%ux%u",
                 primary_tex.desc.width, primary_tex.desc.height);
        FatalError(err);
    }

    if (needs_flipbook)
    {
        auto flipbook = std::make_unique<TFlipbookBillboardComponent>();
        flipbook->Configure(primary_tex.htexture,
                            primary_texture_width, primary_texture_height,
                            primary_bucket.atlas_cols,
                            primary_bucket.atlas_rows,
                            primary_bucket.atlas_frames,
                            primary_bucket.width,
                            primary_bucket.height,
                            primary_bucket.additive, true);
        std::string expr_error;
        if (!flipbook->SetFrameExpression(primary_bucket.frame_expr.c_str(), &expr_error))
            ParticleFatal("[particle] FLAME frame expression compile failed: " + expr_error);
        if (!flipbook->SetUvRectExpression(primary_bucket.uv_rect_expr.c_str(), &expr_error))
            ParticleFatal("[particle] FLAME uv rect expression compile failed: " + expr_error);
        flipbook->SetDebugMode(def.debug_solid ? EFxDebugMode::SolidColor
                                               : EFxDebugMode::Normal);
        inst->AddComponent(std::move(flipbook));
        log_info("[flame-component] attached flipbook component tex=%dx%d handle=%u",
                 primary_texture_width, primary_texture_height, primary_tex.htexture);
    }

    if (needs_particle_effect)
    {
        auto particle_effect = std::make_unique<TParticleEffectComponent>();
        particle_effect->Configure(&def);
        inst->AddComponent(std::move(particle_effect));
        log_info("[flame-component] attached particle effect buckets=%d emitters=%d",
                 int(def.buckets.size()), int(def.emitters.size()));
    }
}

class TFlameAnimatorComponentBuilder : public T3DAnimatorBuilder
{
  public:
    TFlameAnimatorComponentBuilder() : T3DAnimatorBuilder("FLAME") {}
    T3DAnimator* Build(TObjectInstance* oi) override { return new T3DAnimator(oi); }
    void AttachComponents(TObjectInstance* oi) override
    {
        TFlameEffect::AttachVisualComponent(oi, oi ? oi->GetImagery() : nullptr);
    }
};

static TFlameAnimatorComponentBuilder g_flame_animator_component_builder;

// Standalone spawn path used by the --test=vfx harness. The in-game spawn
// route is `MapPane::NewObject(SObjectDef*)`, which threads the instance
// into a loaded sector via the object-class registry; that's overkill (and
// requires a live area / sector) for previewing a single effect. Here we
// take the imagery-only ctor, attach the visual components by hand, and
// give the instance a fresh map index so its components Activate (their
// RegisterUpdate hooks need to be in the global update list for the
// flipbook frame counter to tick). The caller `delete`s the returned
// pointer when done; the destructor releases the imagery refcount and
// detaches the components.
TFlameEffect* TFlameEffect::SpawnForTest(const S3DPoint& origin)
{
    constexpr const char* kImageryPath = "Magic\\flame.i3d";

    const int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        log_error("[flame] SpawnForTest: FindImagery('%s') failed", kImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[flame] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[flame] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Use the imagery-only ctor; we don't have (or want) an SObjectDef /
    // OBJCLASS_EFFECT registry entry for a preview-only instance. Note
    // that this leaves `objclass == -1`; the destructor's component +
    // imagery cleanup runs regardless (see TObjectInstance::~TObjectInstance).
    auto* flame = new TFlameEffect(base);
    flame->ForcePos(origin);

    // OnAttach (which RegisterUpdates the flipbook ticker) only fires once
    // the instance has a map index, since AddComponent gates Activate() on
    // GetMapIndex() >= 0. Stamp one in now so subsequent AddComponent calls
    // (and any future ones from the harness) activate immediately. The
    // visual component on the existing instance was added inside the ctor
    // before the map index existed — re-Activate it explicitly so its
    // OnAttach RegisterUpdate fires too.
    flame->SetMapIndex(MapPane.MakeIndex());

    // Activate every owned component now that the instance has a map
    // index. The ctor's AddComponent calls ran *before* SetMapIndex, so
    // the auto-Activate path in AddComponent (gated on GetMapIndex() >= 0)
    // was skipped — re-fire OnAttach (and any RegisterUpdate inside) here.
    flame->ActivateComponents();

    if (!flame->GetComponent<TFlipbookBillboardComponent>())
    {
        log_error("[flame] SpawnForTest: flipbook component missing after ctor "
                  "(T3DImagery has %d textures, %d objects) — asset may be "
                  "missing texture slot 0",
                  img3d->NumTextures(), img3d->NumObjects());
        delete flame;
        return nullptr;
    }

    log_info("[flame] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "textures=%d",
             kImageryPath, flame->GetMapIndex(),
             origin.x, origin.y, origin.z,
             img3d->NumTextures());
    return flame;
}

// *************************************************************************
// * (B01) TBloodEffect — engine-particle rework                            *
// *************************************************************************
//
// See docs/vfx/forensics/B01_TBloodEffect_RENDER_RESETTLED.md for the
// authoritative spec, docs/vfx/PLAN_B01_engine_rework.md for the rework
// plan. Blood is rebuilt as two engine particle buckets declared in
// data/Resources/effects.def:
//
//   blood_fly    — one-shot 10-droplet burst (spawn_burst=10), 24 Hz
//                  integration, gravity+drag tick_expr, reflection-plane
//                  kill_expr on pos.z <= emit_pos.z. chain_bucket=
//                  "blood_splat" spawns one SPLAT decal at the impact
//                  site when a droplet kills.
//   blood_splat  — chain-driven, ground-flat decal (orientation=world_xy),
//                  holds peak scale for ~60% of life then shrinks to zero.
//
// Render-state per the resettled doc §6:
//   - blend = Alpha (with the texture loader's 1-bit chroma-key alpha
//     baked in by 3dimage.cpp ~line 1690, the result is chroma-keyed
//     opaque -- not semi-transparent, not additive).
//   - light_mode = LitFlat (scene-lit grayscale modulation, retail
//     DoLighting analog).
//   - depth_mode = TestNoWrite.
//   - orientation = ScreenAligned (FLY) / WorldXY (SPLAT).
//
// The bespoke C++ port (1998 TBloodSystem-style per-particle state
// machine on the effect class, two-pass alpha+additive billboard draws)
// is preserved under `#if 0` below for reference, per project
// preserve-old-code rule.
namespace {

constexpr const char* kBloodImageryPath = "Misc\\Blood.I3D";

// Cached parsed effects.def "Blood" block. Lazy on first use; same
// pattern as TorchFlameDef / MistEffectDef above.
const SParticleEffectDef& BloodEffectDef()
{
    static const SParticleEffectDef def = ParseParticleEffectDef("Blood");
    return def;
}

// 24 Hz cadence gate (family-consistent with F03/H03/M05/X22). The original
// per-tick rates (gravity 0.37 wu/tick^2, drag 0.95/tick, scale step 0.01/tick,
// SPLAT duration 25 ticks, SHRINK step 0.1/tick) are integrated once per
// sim-tick of accumulated wall-clock time. See forensics §3 + §6.2.
constexpr int32_t kBloodSimTickMs = 1000 / 24;

// Constants — straight from src/effectcomp.cpp:1164-1180 (forensics §3).
// Names mirror the BLOOD_* defines so the port reads side-by-side with
// the original (the doc cites these by their BLOOD_* spelling).
constexpr float   kBloodAirresist     = 0.95f;
constexpr float   kBloodGravity       = 0.37f;
constexpr int32_t kBloodStageFly      = 0;
constexpr int32_t kBloodStageSplat    = 1;
constexpr int32_t kBloodStageShrink   = 2;
constexpr float   kBloodSplatSize     = 1.8f;
constexpr int32_t kBloodSplatDuration = 25;
constexpr float   kBloodSplatScale    = 0.1f;
constexpr int32_t kBloodZOffset       = 20;
constexpr float   kBloodVel           = 2.0f;
constexpr int32_t kBloodSquirtDelay   = 3;
constexpr float   kBloodFlyScaleStep  = 0.01f;
constexpr float   kBloodFlyScale      = 1.0f;
constexpr int32_t kBloodBloodsPerSquirt = 4;
constexpr int32_t kBloodMostBloods    = 10;

// Retail spray params (TCharacter::ResolveAttack impale immediates per
// recon/discovered/cls_0x5a7b98_..._4c1bb0.cpp:190 — forensics §2.1 / §3).
// These are the live-ship values for the attack-hit path. The snapshot's
// commented `SetParams(40, (face+128)&255, 0, 80, 20, random(1,5))` at
// character.cpp:2260 differs only in spread tuning (snapshot 80/20 vs
// retail 32/5; flagged §13.2) — retail wins for the impale path.
constexpr int32_t kBloodHeightAttack = 40;
constexpr int32_t kBloodVangleAttack = 32;     // retail 0x20 (vangle, not vspread)
constexpr int32_t kBloodHspreadAttack = 32;    // RETAIL 0x20 (snapshot 80)
constexpr int32_t kBloodVspreadAttack = 5;     // RETAIL 5     (snapshot 20)
constexpr int32_t kBloodNumMin = 1;            // random(1,5) count
constexpr int32_t kBloodNumMax = 5;
constexpr int32_t kBloodSizeIdxMax = 2;        // size = random(0, min(maxsize, 2))

// Quad world-unit size when the I3D sub-object has no resolvable size hint.
// The Blood.I3D box billboards are authored at sprite-cell pixel sizes; the
// FB pipeline draws size_wu world-units, so we pick a small base that
// reads as a droplet at typical camera distances. The per-droplet scl
// curve multiplies this (FLY ~1.0 → SPLAT 1.8 → SHRINK → 0).
constexpr float kBloodBaseSizeWu = 16.0f;

// Resolve which texture slot a sub-object draws from by walking its
// texfaces[] table. Each S3DObj records one nonzero entry in
// numtexfaces[1..MAXTEXTURES] for the single texture that actually
// renders its quad (index 0 is no-texture). Returns -1 if none found.
int32_t SubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0)
        return -1;
    // GetObjFaces fills numtexfaces[0..MAXTEXTURES]. Index 0 is the
    // "no texture" bin; the active texture slot is the first nonzero
    // index in [1..MAXTEXTURES]. Return slot-1 (zero-based texture index
    // for GetTexture).
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

// Read a sub-object's authored UV sub-rect from its vertex list. Matches
// the spark UV-cell extraction (see TSparkEffect::SpawnForTest).
void ResolveSubObjUv(T3DImagery* img3d, int32_t objnum, float out[4])
{
    out[0] = 0.0f; out[1] = 0.0f; out[2] = 1.0f; out[3] = 1.0f;
    if (!img3d) return;
    const int32_t nverts = img3d->NumObjVerts(objnum);
    if (nverts <= 0) return;
    std::vector<S3DVertex> vbuf(static_cast<size_t>(nverts), S3DVertex{});
    img3d->GetObjVerts(objnum, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
    float minu = vbuf[0].tu, maxu = vbuf[0].tu;
    float minv = vbuf[0].tv, maxv = vbuf[0].tv;
    for (int32_t i = 1; i < nverts; ++i)
    {
        if (vbuf[i].tu < minu) minu = vbuf[i].tu;
        if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
        if (vbuf[i].tv < minv) minv = vbuf[i].tv;
        if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
    }
    out[0] = minu;
    out[1] = minv;
    out[2] = maxu - minu;
    out[3] = maxv - minv;
}

// Byte-angle → world XY components (matches the original
// `((angle & 255) * 360 / 256) * TORADIAN` math at effectcomp.cpp:1222-1223).
float ByteAngleToRad(int32_t byteangle)
{
    return float((byteangle & 255)) * (float(M_PI) * 2.0f / 256.0f);
}

}   // namespace

// --- Engine-driven implementation (resettled-forensics rework) -----------
//
// Retail TBloodEffect::Initialize / Pulse are the in-game spawn-path hooks
// (decompiled body lives in recon/classes/cls_0x5acaa8.cpp, merged with
// Mist + WaterFall — see forensics §2). The `--test=vfx` path drives
// spawn + tick via SpawnForTest + TickAndSubmitForTest, which wire a
// TParticleEffectComponent configured with the "Blood" effects.def block
// (FLY + SPLAT buckets + chain). The in-game path will be wired in B01a.

TBloodEffect::~TBloodEffect()
{
    // Drop any live particles owned by this effect. Both buckets are
    // engine-owned globals; reaping by owner_id only kills this effect's
    // contribution, leaving other in-flight bursts (if any) intact.
    if (owner_particle_id_ >= 0.0f)
    {
        if (fly_bucket_)   fly_bucket_->KillParticlesByOwner(owner_particle_id_);
        if (splat_bucket_) splat_bucket_->KillParticlesByOwner(owner_particle_id_);
    }
}

void TBloodEffect::Initialize()
{
    // Empty (intentional). The pre-release TBloodEffect::Initialize body
    // is empty (effect_old.cpp:11246-11253); per-burst state now lives
    // in the engine particle buckets keyed by owner_particle_id_.
}

void TBloodEffect::Pulse()
{
    TEffect::Pulse();
    // No per-pulse work — the engine ticks particles via the attached
    // TParticleEffectComponent's DrawPulse path each render frame.
}

bool TBloodEffect::IsAlive() const
{
    // Alive while either bucket still holds particles owned by us. The
    // harness uses this to gate the re-trigger gap (see vfxtest.cpp
    // BloodSubmit). Walking the bucket per-frame is fine — both buckets
    // hold at most a few dozen particles total at the SPLAT/FLY peaks.
    auto any_owned = [this](const TParticleBucket* b) {
        if (!b) return false;
        for (int32_t i = 0; i < b->Count(); ++i)
        {
            const float* o = b->VarPtr(i, EParticleVar::OwnerId);
            if (o && *o == owner_particle_id_)
                return true;
        }
        return false;
    };
    return any_owned(fly_bucket_) || any_owned(splat_bucket_);
}

TBloodEffect* TBloodEffect::SpawnForTest(const S3DPoint& origin)
{
    // Load Misc\Blood.I3D so its textures get registered with the renderer
    // (the bucket descs reference these via `imagery_path` and the engine
    // resolves the htexture handle at StartRuntime; the texture loader's
    // 1-bit chroma-key alpha conversion happens during this load, per
    // 3dimage.cpp ~line 1690). No procedural stand-in -- the dark-red box
    // sprites ARE the visual identity per forensics §10.
    const int32_t img_id = TObjectImagery::FindImagery(kBloodImageryPath);
    if (img_id < 0)
    {
        log_error("[blood] SpawnForTest: FindImagery('%s') failed", kBloodImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[blood] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kBloodImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[blood] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kBloodImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    // Force lazy mesh init so the bucket-desc resolver finds populated
    // texture slots (NumTextures alone doesn't trigger init).
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[blood] SpawnForTest: imagery has no textures after init");
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* blood = new TBloodEffect(base);
    blood->ForcePos(origin);
    blood->SetMapIndex(MapPane.MakeIndex());
    blood->ActivateComponents();

    // Attach the engine-driven particle component. The "Blood" effects.def
    // entry declares two buckets (blood_fly + blood_splat) wired by
    // chain_bucket="blood_splat" on FLY's kill_expr.
    auto particle_effect = std::make_unique<TParticleEffectComponent>();
    particle_effect->Configure(&BloodEffectDef());
    blood->AddComponent(std::move(particle_effect));

    // Drive one DrawPulse so StartRuntime fires NOW and the FLY bucket's
    // spawn_burst=10 emits immediately (visible from frame 1, matching
    // the harness Static preview style).
    if (auto* pe = blood->GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    blood->fly_bucket_       = ParticleManager().FindGlobalBucket("blood_fly");
    blood->splat_bucket_     = ParticleManager().FindGlobalBucket("blood_splat");
    blood->owner_particle_id_ = float(blood->GetMapIndex());

    log_info("[blood] SpawnForTest (engine-driven): '%s' map_index=%d "
             "origin=(%d,%d,%d) fly_bucket=%p splat_bucket=%p def=Blood",
             kBloodImageryPath, blood->GetMapIndex(),
             origin.x, origin.y, origin.z,
             (void*)blood->fly_bucket_, (void*)blood->splat_bucket_);
    return blood;
}

void TBloodEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer) return;

    // Drive the engine's per-frame integration. Pulse first so the
    // FLY tick_expr advances + kill_expr/chain fire before submit.
    if (auto* pe = GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    if (fly_bucket_)
        Renderer->SubmitFxParticleBucket(*fly_bucket_, debug_mode);
    if (splat_bucket_)
        Renderer->SubmitFxParticleBucket(*splat_bucket_, debug_mode);
}

// --- Bespoke faithful port (A/B reference, lives alongside engine port).
//
// The 1998 TBloodSystem-style implementation: 30-particle array on the
// effect class, 3-stage FLY → SPLAT → SHRINK state machine integrated at
// 24 Hz via a sim-tick accumulator, two-pass billboard draw per droplet
// (Alpha base + AdditiveStraight overlay) using 8 box sub-objects of
// Misc\Blood.I3D. Lives alongside the engine-particle rework above as
// the A/B baseline for snap_ab.py side-by-side filmstrip verification
// (see TBloodEffect_Bespoke in effect.h + tools/vfx/snap_ab.py).
//
// The math/constants/sub-object resolution are the snapshot port,
// preserved per project preserve-old-code rule.

TBloodEffect_Bespoke* TBloodEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // --- Load the REAL Misc\Blood.I3D imagery (no procedural stand-in).
    // The dark-red box sprites ARE the visual identity (forensics §10).
    const int32_t img_id = TObjectImagery::FindImagery(kBloodImageryPath);
    if (img_id < 0)
    {
        log_error("[blood] SpawnForTest: FindImagery('%s') failed", kBloodImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[blood] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kBloodImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[blood] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kBloodImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* blood = new TBloodEffect_Bespoke(base);
    blood->ForcePos(origin);
    blood->SetMapIndex(MapPane.MakeIndex());
    blood->ActivateComponents();

    // Lazy-mesh-init poke (mirror F01 / H04 / X22). NumObjects triggers
    // the actual mesh load; NumTextures alone doesn't.
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < 8 || num_tex <= 0)
    {
        log_error("[blood] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d) — expected 8 box sub-objects",
                  num_obj, num_tex);
        delete blood;
        return nullptr;
    }

    // Resolve each of the 8 box sub-objects to (texture, UV sub-rect).
    // Forensics §4 maps:
    //   index 0..3 = box01..box04 = pass 1 (AdditiveStraight overlay)
    //   index 4..7 = box05..box08 = pass 0 (Alpha base)
    // Within each set: [0]=small, [1]=med, [2]=big, [3]=splat.
    // GetObject(0..7) maps directly to T3DImagery sub-object indices 0..7;
    // box01 is sub-object 0, box02 is 1, ..., box08 is 7.
    int32_t resolved_textures[8] = {0,0,0,0,0,0,0,0};
    for (int32_t i = 0; i < 8; ++i)
    {
        if (i >= num_obj)
            break;
        const int32_t tex_slot = SubObjTextureSlot(img3d, i);
        const int32_t slot = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
        resolved_textures[i] = slot;
        S3DTex tex = {};
        img3d->GetTexture(slot, &tex);
        blood->subobjs_[i].texture = tex.htexture;
        ResolveSubObjUv(img3d, i, blood->subobjs_[i].uv_rect);
        blood->subobjs_[i].size_wu = kBloodBaseSizeWu;
    }
    if (blood->subobjs_[0].texture == kInvalidTexture
        || blood->subobjs_[4].texture == kInvalidTexture)
    {
        log_error("[blood] SpawnForTest: sub-object textures unresolved "
                  "(pass1[0]=%u, pass0[4]=%u)",
                  blood->subobjs_[0].texture, blood->subobjs_[4].texture);
        delete blood;
        return nullptr;
    }

    // --- Port of TBloodSystem::Init (effectcomp.cpp:1182-1307).
    // Retail spray params (forensics §2.1 / §3): impale-path immediates.
    // hangle randomized for the harness (no attacker→victim vector); in-game
    // it's `(victim_face + 128) & 255` (180° from victim front).
    int32_t height_param  = kBloodHeightAttack;            // 40 wu above object pos
    int32_t hangle_param  = random(0, 255);                // harness: random horizontal mean
    int32_t vangle_param  = kBloodVangleAttack;            // retail 32
    int32_t hspread_param = kBloodHspreadAttack;           // RETAIL 32 (not snapshot 80)
    int32_t vspread_param = kBloodVspreadAttack;           // RETAIL 5  (not snapshot 20)
    int32_t num_param     = random(kBloodNumMin, kBloodNumMax);  // random(1,5)
    // maxsize from TBloodAnimator::Initialize (effect_old.cpp:11292).
    int32_t maxsize_param = 2;

    // ViolenceLevel clamps (effectcomp.cpp:1214-1217).
    int32_t num     = num_param;
    int32_t maxsize = maxsize_param;
    if (num > ViolenceLevel * 2)
        num = ViolenceLevel * 2;
    if (maxsize > ViolenceLevel)
        maxsize = ViolenceLevel;

    // Trail-fill budget (effectcomp.cpp:1211).
    const int32_t init_left_base =
        kBloodMaxParticles - ((kBloodMostBloods - num) * kBloodBloodsPerSquirt);
    int32_t left = init_left_base > kBloodBloodsPerSquirt
                       ? init_left_base
                       : kBloodBloodsPerSquirt;

    blood->height_local_ = float(height_param);

    // Main droplets (effectcomp.cpp:1220-1236).
    int32_t cur_num = 0;
    const int32_t main_count = num < kBloodMaxParticles ? num : kBloodMaxParticles;
    for (int32_t i = 0; i < main_count; ++i)
    {
        // Byte-angle ± spread → radians, faithful to original
        // `(((mean + random(-spread, spread)) & 255) * 360 / 256) * TORADIAN`.
        const float ha = ByteAngleToRad(
            hangle_param + random(-hspread_param, hspread_param));
        const float va = ByteAngleToRad(
            vangle_param + random(-vspread_param, vspread_param));
        SBloodParticleEx& p = blood->particles_[i];
        p.used = true;
        const int32_t sz_cap = maxsize < kBloodSizeIdxMax ? maxsize : kBloodSizeIdxMax;
        p.size = random(0, sz_cap);
        p.pos.X = 0.0f;
        p.pos.Y = 0.0f;
        p.pos.Z = float(height_param);
        const float vs_x = float(random(100, 300)) / 100.0f;   // 1.0 .. 3.0
        const float vs_y = float(random(100, 300)) / 100.0f;
        const float vs_z = float(random(100, 300)) / 100.0f;
        p.vel.X = kBloodVel * std::cos(ha) * vs_x;
        p.vel.Y = kBloodVel * std::sin(ha) * vs_y;
        p.vel.Z = kBloodVel * std::sin(va) * vs_z;
        p.scl   = kBloodFlyScale;
        p.stage = kBloodStageFly;
        p.delay = 0;
        p.count = 0;
        --left;
        ++cur_num;
    }

    // Trail-fill gate (effectcomp.cpp:1238-1239).
    if (ViolenceLevel < 4)
        left = 0;

    // Smalls-trail-smalls loop (effectcomp.cpp:1285-1306).
    // Only the live loop — the big/med trail loops above (:1242-1283) are
    // dead per forensics §13.5. Walk i forward through the source droplets,
    // adding small droplets riding each one at vel * random(60,75)/80.
    for (int32_t i = 0; left > 0 && cur_num < kBloodMaxParticles; ++i)
    {
        // Original loop has no upper bound on i — it walks until either
        // `left` runs out or `me.num >= MAX_BLOODS` (cur_num == cap), with
        // the source droplet index wrapping by always pulling from the
        // most recent slot (i increments while cur_num grows). Cap i to
        // the live count to avoid an unbounded walk if the seeded droplets
        // ran out (an edge case the original tolerated implicitly via the
        // MAX_BLOODS ceiling).
        if (i >= cur_num)
            break;
        if (!blood->particles_[i].used)
            continue;
        SBloodParticleEx& parent = blood->particles_[i];
        SBloodParticleEx& q = blood->particles_[cur_num];
        q.used = true;
        q.size = random(0, 1);
        q.pos.X = 0.0f;
        q.pos.Y = 0.0f;
        q.pos.Z = float(height_param);
        const float scale_x = float(random(60, 75)) / 80.0f;   // 0.75..0.9375
        const float scale_y = float(random(60, 75)) / 80.0f;
        const float scale_z = float(random(60, 75)) / 80.0f;
        q.vel.X = parent.vel.X * scale_x;
        q.vel.Y = parent.vel.Y * scale_y;
        q.vel.Z = parent.vel.Z * scale_z;
        q.delay = parent.delay + kBloodSquirtDelay + random(-1, 1);
        if (q.delay < 0) q.delay = 0;
        q.stage = kBloodStageFly;
        q.scl   = kBloodFlyScale;
        q.count = 0;
        ++cur_num;
        --left;
    }

    blood->alive_ = (cur_num > 0);
    log_info("[blood] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "particles=%d (main=%d trail=%d) height=%d hangle=%d "
             "hspread=%d vspread=%d ViolenceLevel=%d "
             "subobj_tex={pass1=%u/%u/%u/%u, pass0=%u/%u/%u/%u} "
             "tex_slots={%d,%d,%d,%d,%d,%d,%d,%d}",
             kBloodImageryPath, blood->GetMapIndex(),
             origin.x, origin.y, origin.z,
             cur_num, main_count, cur_num - main_count,
             height_param, hangle_param, hspread_param, vspread_param,
             ViolenceLevel,
             blood->subobjs_[0].texture, blood->subobjs_[1].texture,
             blood->subobjs_[2].texture, blood->subobjs_[3].texture,
             blood->subobjs_[4].texture, blood->subobjs_[5].texture,
             blood->subobjs_[6].texture, blood->subobjs_[7].texture,
             resolved_textures[0], resolved_textures[1],
             resolved_textures[2], resolved_textures[3],
             resolved_textures[4], resolved_textures[5],
             resolved_textures[6], resolved_textures[7]);
    return blood;
}

void TBloodEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TBloodSystem::Animate (effectcomp.cpp:1309-1370),
    // framerate-independent via the 24 Hz sim-tick accumulator. Each tick
    // runs the original per-tick integration exactly once.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kBloodSimTickMs))
        {
            sim_accum_ms_ -= double(kBloodSimTickMs);

            bool done = true;
            for (int32_t i = 0; i < kBloodMaxParticles; ++i)
            {
                SBloodParticleEx& p = particles_[i];
                if (!p.used)
                    continue;
                if (p.delay > 0)
                {
                    p.delay--;
                    continue;
                }
                done = false;

                if (p.stage == kBloodStageFly)
                {
                    // effectcomp.cpp:1326-1334: pos += vel; vel.xy *= 0.95;
                    // vel.z -= 0.37; scl += 0.01.
                    p.pos.X += p.vel.X;
                    p.pos.Y += p.vel.Y;
                    p.pos.Z += p.vel.Z;
                    p.vel.X *= kBloodAirresist;
                    p.vel.Y *= kBloodAirresist;
                    p.vel.Z -= kBloodGravity;
                    p.scl   += kBloodFlyScaleStep;

                    // effectcomp.cpp:1336-1345: ground hit → SPLAT, with
                    // early-kills at ViolenceLevel<3 OR size==0.
                    if (p.pos.Z <= float(kBloodZOffset))
                    {
                        p.pos.Z = float(kBloodZOffset);
                        p.stage = kBloodStageSplat;
                        p.count = 0;
                        if (ViolenceLevel < 3)
                            p.used = false;
                        if (p.size == 0)
                            p.used = false;
                    }
                }
                else if (p.stage == kBloodStageSplat)
                {
                    // effectcomp.cpp:1347-1355.
                    p.scl = kBloodSplatSize;
                    p.count++;
                    if (p.count > kBloodSplatDuration)
                    {
                        p.stage = kBloodStageShrink;
                        p.count = 0;
                    }
                }
                else if (p.stage == kBloodStageShrink)
                {
                    // effectcomp.cpp:1357-1365.
                    p.scl -= kBloodSplatScale;
                    if (p.scl <= 0.0f)
                    {
                        p.scl = 0.0f;
                        p.used = false;
                    }
                }
            }
            // TBloodAnimator self-destruct: OF_KILL when GetDone() goes
            // true (effect_old.cpp:11313-11316). Harness reads alive_ to
            // know when to re-trigger.
            if (done)
            {
                alive_ = false;
                break;
            }
        }
    }

    // --- Render: port of TBloodSystem::Render (effectcomp.cpp:1415-1483).
    // Resettled forensics (B01_TBloodEffect_RENDER_RESETTLED.md §6): the
    // snapshot SetBlendState writes SRC_ALPHA/INV_SRC_ALPHA factors but
    // D3DRENDERSTATE_ALPHABLENDENABLE is NEVER turned on along the blood
    // path, so on shipped DX6/7 the blend is DORMANT. The texture loader
    // (legacy/3dimage.cpp:2079) bakes the keycolor as 1-bit alpha and the
    // shipped read is CHROMA-KEYED OPAQUE, single-pass. The old k=1
    // AdditiveStraight overlay pass has been DROPPED per §6.4 — it was
    // a re-derivation of the wrong (dormant-blend) snapshot read and
    // produced washed-out pink droplets in the harness. Single Alpha
    // pass below uses the box05..box08 cells (indices 4..7) and lets the
    // FB shader's chroma-key discard cut out the black background.
    // Size 2 (big) and the splat sub-objects are commented out in the
    // original — a size-2 droplet renders with the small sprite
    // (forensics §7 / §13.3).
    const S3DPoint& base = Pos();
    for (int32_t i = 0; i < kBloodMaxParticles; ++i)
    {
        const SBloodParticleEx& p = particles_[i];
        if (!p.used || p.delay > 0)
            continue;
        // Per-droplet world position. The render places each box at
        // (pos.x, pos.y, pos.z - height) so the world z is
        // (object_origin.z + pos.z - height) — forensics §7 "the −height
        // cancels the local start height". For the harness, base is the
        // object origin in world coords.
        const float wx = float(base.x) + p.pos.X;
        const float wy = float(base.y) + p.pos.Y;
        const float wz = float(base.z) + (p.pos.Z - height_local_);

        // Size index → sub-object within each pass-set: [0]=small, [1]=med,
        // [2]=big (DEAD, falls through to small per §13.3), [3]=splat
        // (DEAD, only used by the bif splash per §13.4). Render small + med
        // only. Big size collapses to small.
        const int32_t cell = p.size == 1 ? 1 : 0;

        // Common item template (per-pass blend + sub-object override below).
        SBillboardDrawItem item = {};
        item.size_wu[0] = item.size_wu[1] =
            subobjs_[4 + cell].size_wu * p.scl;
        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 1.0f;
        item.color_rgba[3] = 1.0f;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        // Light mode: matches the engine-driven blood_fly bucket
        // (data/Resources/effects.def `light_mode = "unlit"`) which is
        // currently set Unlit as a harness diagnostic until the test scene
        // lights TBloodEffect properly (resettled doc §6.6). Keeping the
        // bespoke aligned with the engine path means the A/B capture
        // isolates BLEND differences only, not lighting.
        item.light_mode = EFxLightMode::Unlit;
        // FLY droplets are tipped ground-flat (RotateX(-90°)+RotateZ(-45°),
        // i.e. WorldXY ground-oriented with a 45° in-plane spin per §7);
        // SPLAT/SHRINK keep authored facing. WorldXY is the engine analog
        // for the canonical ground-flat tip.
        item.orientation = p.stage == kBloodStageFly
                               ? EFxBillboardOrientation::WorldXY
                               : EFxBillboardOrientation::ScreenAligned;
        item.debug_mode  = debug_mode;
        item.world_pos[0] = wx;
        item.world_pos[1] = wy;
        item.world_pos[2] = wz;

        // Single Alpha pass — sub-objects box05..box08 (indices 4..7). The
        // texture's keycolor was baked to 1-bit alpha by the .I3D loader
        // (legacy/3dimage.cpp:2079) so the FB shader's chroma-key discard
        // cuts out the black background and the dark-red droplet reads
        // as a chroma-keyed OPAQUE sprite — same as shipped DX6/7.
        // The pre-resettled k=1 AdditiveStraight overlay pass on
        // box01..box04 was DROPPED per resettled doc §6.4: it was a
        // re-derivation of the dormant-blend snapshot read and produced
        // light-pink washed-out droplets via additive accumulation on top
        // of the dark-red base.
        const SBloodSubObject& so = subobjs_[4 + cell];
        if (so.texture != kInvalidTexture)
        {
            item.key.texture = so.texture;
            item.key.blend   = uint8_t(EFxBlend::Alpha);
            item.uv_rect[0] = so.uv_rect[0];
            item.uv_rect[1] = so.uv_rect[1];
            item.uv_rect[2] = so.uv_rect[2];
            item.uv_rect[3] = so.uv_rect[3];
            Renderer->SubmitFxBillboard(item);
        }
    }
}
// --- end bespoke faithful port (A/B reference)

// *************************************************************************
// * TFlameEffect_Bespoke — F01 A/B reference (faithful direct C++ port)    *
// *************************************************************************
//
// Line-by-line port of TFlameAnimator::Initialize / Animate / Render
// (src/effect_old.cpp:4486-4565). Single ScreenAligned billboard quad of
// Magic\flame.i3d's sole `box01` sub-object; per-frame UV cell pick out
// of a 4-col x 2-row atlas using `n = frame*11/24; col = n%4; row = n/4`.
// `frame` cycles 0..17 (snapshot frame_wrap=18). Blend = Alpha (DECAL) per
// snapshot SetBlendState — F01 forensics §7 BLEND SANITY-CHECK flags this
// as snapshot-only-but-suspect; preserved literally per translation rule 3.
// The snapshot's matrix (RotX(45°)·RotY(30°)·RotZ(160°)·Scale(0.5)) was
// the D3D6/7 way to orient a non-billboarding mesh quad to roughly face
// the iso camera; in the modern FB pipeline ScreenAligned is the
// equivalent (the quad auto-faces the camera, see renderer.h:193) so the
// explicit Euler rotation collapses to "ScreenAligned" — drift-adaptation
// noted in the return.

namespace {

constexpr const char* kFlameBespokeImageryPath = "Magic\\flame.i3d";
constexpr int32_t     kFlameBespokeFrameWrap   = 18;          // snapshot effect_old.cpp:4508
constexpr int32_t     kFlameBespokeSimTickMs   = 1000 / 24;   // 24 Hz cadence gate

}  // namespace

TFlameEffect_Bespoke* TFlameEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // --- Load Magic\flame.i3d (asset path from snapshot Class.Def:2030)
    int32_t img_id = TObjectImagery::FindImagery(kFlameBespokeImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kFlameBespokeImageryPath));
    if (img_id < 0)
    {
        log_error("[flame-bespoke] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kFlameBespokeImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[flame-bespoke] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFlameBespokeImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[flame-bespoke] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kFlameBespokeImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    (void)img3d->NumObjects();   // lazy-mesh-init poke (sister idiom F01/B01/M05/H04)
    if (img3d->NumTextures() <= 0)
    {
        log_error("[flame-bespoke] SpawnForTest: imagery '%s' has 0 textures after lazy-init",
                  kFlameBespokeImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Resolve texture slot 0 (single material slot per F01 forensics §4).
    S3DTex tex0 = {};
    img3d->GetTexture(0, &tex0);
    if (tex0.htexture == kInvalidTexture)
    {
        log_error("[flame-bespoke] SpawnForTest: texture slot 0 handle invalid");
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* flame = new TFlameEffect_Bespoke(base);
    flame->ForcePos(origin);
    flame->SetMapIndex(MapPane.MakeIndex());
    flame->ActivateComponents();
    flame->texture_ = tex0.htexture;
    flame->frame_   = 0;                 // snapshot Initialize :4493

    log_info("[flame-bespoke] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "tex0=%u w=%u h=%u",
             kFlameBespokeImageryPath, flame->GetMapIndex(),
             origin.x, origin.y, origin.z,
             tex0.htexture, tex0.desc.width, tex0.desc.height);
    return flame;
}

void TFlameEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer || texture_ == kInvalidTexture)
        return;

    // --- Animate: port of TFlameAnimator::Animate (effect_old.cpp:4503-4510).
    // Snapshot advances `frame` once per render-frame ungated. Drift-
    // adaptation: framerate-independent via 24 Hz sim-tick accumulator (per
    // feedback-framerate-independent-anim memory). One increment per tick.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kFlameBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kFlameBespokeSimTickMs);
        ++frame_;                                  // :4507
        if (frame_ >= kFlameBespokeFrameWrap)      // :4508
            frame_ = 0;                            // :4509
    }

    // --- Render: port of TFlameAnimator::Render (effect_old.cpp:4519-4565).
    // SetBlendState (snapshot :4521-4522) = Alpha (DECAL) — preserved
    // literally per translation rule 3 / forensics §7 BLEND SANITY-CHECK.
    //
    // Per-frame UV cell pick (snapshot :4543-4544):
    //   n    = (int32_t)(frame * 11 / 24)
    //   xpos = (n % 4) * 0.25
    //   ypos = (n / 4) * 0.5
    // The 4 lvert UVs sample the 0.25 x 0.5 sub-rect at (xpos, ypos)
    // (snapshot :4546-4556).
    const int32_t n    = (frame_ * 11) / 24;
    const float   xpos = float(n % 4) * 0.25f;
    const float   ypos = float(n / 4) * 0.5f;

    const S3DPoint& p = Pos();

    SBillboardDrawItem item = {};
    item.world_pos[0] = float(p.x);
    item.world_pos[1] = float(p.y);
    item.world_pos[2] = float(p.z);
    // Snapshot scale = 0.5 uniform (snapshot :4535-4538). Authored quad
    // base size: per F01 forensics §4 the cell is 32x80 px in source, with
    // the snapshot Render path drawing via the imagery's authored verts
    // through scale 0.5. The modern FB pipeline takes a literal world-unit
    // size_wu; pick a size_wu that approximates the on-screen footprint
    // (the F01 RefreshZBuffer patch was 25x62 px, matches a ~32 wu quad
    // tall under the harness camera; same value the engine-port F03 uses).
    item.size_wu[0]   = quad_size_wu_;
    item.size_wu[1]   = quad_size_wu_ * (80.0f / 32.0f);  // preserve cell aspect 32:80
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.uv_rect[0]   = xpos;
    item.uv_rect[1]   = ypos;
    item.uv_rect[2]   = 0.25f;
    item.uv_rect[3]   = 0.5f;
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);          // SetBlendState :4522
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;               // flame is self-lit (§7)
    item.orientation     = EFxBillboardOrientation::ScreenAligned;  // collapses snapshot Euler tilt
    Renderer->SubmitFxBillboard(item);
}
// --- end TFlameEffect_Bespoke faithful port

// (TFireEffect_Bespoke impl deferred to after the F03 engine port; see
//  the matching block after TFireEffect::TickAndSubmitForTest — that's
//  where the `kFirePatchHalfWu` / `RerollFireQuad` anon-namespace helpers
//  live and are reused verbatim.)

// *************************************************************************
// * TFireSwarmEffect_Bespoke — F05 A/B reference (faithful direct port)    *
// *************************************************************************
//
// Line-by-line port of TFireSwarmAnimator::Initialize / Animate / Render
// (src/effect_old.cpp:10483-10549). Forensics doc:
// docs/vfx/forensics/F05_TFireSwarmEffect.md. The snapshot draws the
// `tube01` sub-object (index 1) of `Magic\FireSwarm.i3d` as a
// transform-animated I3D cylinder mesh: per-frame matrix
// Scale(cylhscl, cylhscl, cylvscl) · RotZ(cylth) with no translate, blend
// = Alpha (SetBlendState, snapshot literal preserved per translation
// rule 3). The mesh has 64 vertices wrapped with a 64×128 ARGB4444 flame
// skin (red core, yellow tongues).
//
// First-pass drift: the Sokol FB pipeline doesn't currently expose an
// arbitrary-mesh + per-effect blend submission API for one-off effects,
// so we approximate the cylinder by a single ScreenAligned billboard
// using the tube01 texture; the billboard width tracks `cylhscl` and the
// height tracks `cylvscl` so the visual still "expands outward and
// flattens vertically" over the 75-tick life. Full mesh path is a
// follow-up — Renderer->SubmitMesh requires a MeshHandle authored
// through TMeshExtractor, which is out of scope for this first-pass.

namespace {

constexpr const char* kFireSwarmBespokeImageryPath = "Magic\\FireSwarm.i3d";
constexpr int32_t     kFireSwarmBespokeDuration    = 75;        // FIRESWARM_DURATION
constexpr float       kFireSwarmBespokeHsclStep    = 0.4f;      // FIRESWARM_CYLHSCLSTEP
constexpr float       kFireSwarmBespokeThStep      = 0.5f;      // FIRESWARM_CYLTHSTEP
constexpr float       kFireSwarmBespokeVsclInit    = 30.0f;     // FIRESWARM_CYLVSCLINIT
constexpr float       kFireSwarmBespokeVsclStep    = 0.4f;      // FIRESWARM_CYLVSCLSTEP
constexpr float       kFireSwarmBespoke2Pi         = 6.2831853071795864f;
constexpr int32_t     kFireSwarmBespokeSimTickMs   = 1000 / 24;
// World-unit size multiplier applied to (cylhscl, cylvscl) — the
// snapshot mesh's authored radius is ~1.0 wu, so the billboard `size_wu`
// = scl * 1.0. The original cylinder was drawn through the engine's
// parent-transform concatenation; in the harness we use a literal world
// size that reads at the test camera. Adjust if the visual scale is off.
constexpr float       kFireSwarmBespokeUnitWu      = 1.0f;

}  // namespace

TFireSwarmEffect_Bespoke* TFireSwarmEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    int32_t img_id = TObjectImagery::FindImagery(kFireSwarmBespokeImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kFireSwarmBespokeImageryPath));
    if (img_id < 0)
    {
        log_error("[fireswarm-bespoke] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kFireSwarmBespokeImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[fireswarm-bespoke] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFireSwarmBespokeImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fireswarm-bespoke] SpawnForTest: imagery '%s' is not a T3DImagery",
                  kFireSwarmBespokeImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    // Lazy-mesh-init poke (same idiom as F01/B01/M05/H04) — NumObjects
    // triggers the actual mesh load; NumTextures alone doesn't.
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < 2 || num_tex <= 0)
    {
        log_error("[fireswarm-bespoke] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d) — expected 2 sub-objects",
                  num_obj, num_tex);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Forensics §4 says tube01 (sub-object index 1) is the cylinder
    // drawn by the animator; box01 (index 0) is unused. The single
    // ARGB4444 64×128 texture is at slot 0.
    S3DTex tex0 = {};
    img3d->GetTexture(0, &tex0);

    auto* swarm = new TFireSwarmEffect_Bespoke(base);
    swarm->ForcePos(origin);
    swarm->SetMapIndex(MapPane.MakeIndex());
    swarm->ActivateComponents();
    swarm->texture_ = tex0.htexture;

    // Snapshot Initialize body (effect_old.cpp:10483-10490) — verbatim.
    swarm->frameon_ = 0;
    swarm->cylth_   = 0.0f;
    swarm->cylhscl_ = kFireSwarmBespokeHsclStep;     // 0.4 (matches one tick's increment)
    swarm->cylvscl_ = kFireSwarmBespokeVsclInit;     // 30.0
    swarm->alive_   = true;
    swarm->base_size_wu_ = kFireSwarmBespokeUnitWu;

    log_info("[fireswarm-bespoke] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "tex0=%u w=%u h=%u numobj=%d numtex=%d",
             kFireSwarmBespokeImageryPath, swarm->GetMapIndex(),
             origin.x, origin.y, origin.z,
             tex0.htexture, tex0.desc.width, tex0.desc.height, num_obj, num_tex);
    return swarm;
}

void TFireSwarmEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!alive_ || !Renderer || texture_ == kInvalidTexture)
        return;

    // --- Animate: port of TFireSwarmAnimator::Animate
    // (src/effect_old.cpp:10499-10515). Snapshot runs ungated every
    // Animate; drift to 24 Hz sim-tick gate per
    // feedback-framerate-independent-anim memory.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kFireSwarmBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kFireSwarmBespokeSimTickMs);

        ++frameon_;                                       // :10504

        cylth_   += kFireSwarmBespokeThStep;              // :10506 (0.5 rad/tick)
        if (cylth_ > kFireSwarmBespoke2Pi)                // :10507
            cylth_ -= kFireSwarmBespoke2Pi;               // :10508 (single subtract wrap)
        cylhscl_ += kFireSwarmBespokeHsclStep;            // :10509 (XY radius grows)
        cylvscl_ -= kFireSwarmBespokeVsclStep;            // :10510 (Z height shrinks)

        if (frameon_ > kFireSwarmBespokeDuration)         // :10511
        {
            alive_ = false;                               // KillThisEffect() :10513
            break;
        }
    }
    if (!alive_)
        return;

    // --- Render: port of TFireSwarmAnimator::Render
    // (src/effect_old.cpp:10524-10549). Snapshot:
    //   SaveBlendState() / SetBlendState() (= Alpha)
    //   obj = GetObject(1)          (tube01)
    //   obj->flags = OBJ3D_MATRIX
    //   D3DMATRIXClear / Scale(cylhscl, cylhscl, cylvscl) / RotZ(cylth)
    //   RenderObject(obj)
    //   RestoreBlendState()
    //
    // First-pass: single ScreenAligned billboard at effect.Pos() with
    // width = cylhscl * base_size_wu and height = cylvscl * base_size_wu,
    // Alpha blend per the snapshot SetBlendState. cylth (yaw) is not
    // applied — ScreenAligned billboards have no in-plane rotation lane
    // in SBillboardDrawItem (would need SParticleDrawItem's rotation_rad
    // or a true mesh submission). Drift documented in batch return.
    const S3DPoint& p = Pos();

    SBillboardDrawItem item = {};
    item.world_pos[0]  = float(p.x);
    item.world_pos[1]  = float(p.y);
    item.world_pos[2]  = float(p.z);
    item.size_wu[0]    = cylhscl_ * base_size_wu_;        // XY radius scale
    item.size_wu[1]    = cylvscl_ * base_size_wu_;        // Z height scale
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.uv_rect[0]    = 0.0f;
    item.uv_rect[1]    = 0.0f;
    item.uv_rect[2]    = 1.0f;
    item.uv_rect[3]    = 1.0f;
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);      // SetBlendState (:10527)
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;           // flame self-lit (forensics §7)
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    Renderer->SubmitFxBillboard(item);
}
// --- end TFireSwarmEffect_Bespoke faithful port

// *************************************************************************
// * TBurnEffect_Bespoke — M04 A/B reference (faithful direct port)         *
// *************************************************************************
//
// Line-by-line port of the snapshot TBurnAnimator::Initialize / Animate /
// Render (src/effect_old.cpp:3292-3460). Forensics doc:
// docs/vfx/forensics/M04_TBurnEffect.md. Two TParticleSystem instances
// (fire + smoke) with BURN_COUNT=70 capacity each, fire→smoke promotion
// on life==life_span (×1.25 scl, ×2/3 life_span). Blend =
// AdditiveStraight (snapshot SetAddBlendState — fire-family consistent
// per forensics §7).
//
// First-pass drift: the harness has no live TCharacter rig, so the
// per-particle "spawn at a random bone's world position" path is
// simplified to spawn at the effect's own world position + ±BURN_SPREAD
// jitter on each axis (size_ defaults to 1 — "one virtual bone" = the
// effect origin). Particle physics (velocity, life, decay, fire→smoke
// promotion, ramp envelope, kill condition) are preserved verbatim.
// Full character-rig attachment is a follow-up.

namespace {

constexpr const char* kBurnBespokeImageryPath = "Magic\\burnbabyburn.I3D";
constexpr int32_t     kBurnBespokeSimTickMs   = 1000 / 24;
constexpr int32_t     kBurnBespokePartMin     = 50;   // BURN_PART_MIN (DEAD per forensics §13.1 — preserve)
constexpr int32_t     kBurnBespokePartMax     = 50;   // BURN_PART_MAX (DEAD)
constexpr int32_t     kBurnBespokeAdd         = 8;    // BURN_ADD
constexpr int32_t     kBurnBespokeSpread      = 2;    // BURN_SPREAD (wu, ± per axis)
constexpr int32_t     kBurnBespokeMinScl      = 15;   // BURN_MIN_SCL (×0.01)
constexpr int32_t     kBurnBespokeMaxScl      = 40;   // BURN_MAX_SCL (×0.01)
constexpr int32_t     kBurnBespokeMinZ        = 30;   // BURN_MIN_Z (×0.1)
constexpr int32_t     kBurnBespokeMaxZ        = 75;   // BURN_MAX_Z (×0.1)
constexpr int32_t     kBurnBespokeMinLife     = 5;    // BURN_MIN_LIFE
constexpr int32_t     kBurnBespokeMaxLife     = 15;   // BURN_MAX_LIFE
constexpr float       kBurnBespokeDec         = 0.97f;// BURN_DEC
constexpr int32_t     kBurnBespokeFrame       = 50;   // BURN_FRAME

// Sub-object enumeration (forensics §4):
//   GetObject(0) = "smoke"    -> smoke system  -> tex slot for smoke
//   GetObject(1) = "smoke01"  -> fire  system  -> tex slot for fire
// Following the SubObjTextureSlot helper above (TBloodEffect_Bespoke).
int32_t BurnSubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0)
        return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

}  // namespace

TBurnEffect_Bespoke* TBurnEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    int32_t img_id = TObjectImagery::FindImagery(kBurnBespokeImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kBurnBespokeImageryPath));
    if (img_id < 0)
    {
        log_error("[burn-bespoke] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kBurnBespokeImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[burn-bespoke] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kBurnBespokeImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[burn-bespoke] SpawnForTest: imagery '%s' is not a T3DImagery",
                  kBurnBespokeImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < 2 || num_tex <= 0)
    {
        log_error("[burn-bespoke] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d) — expected 2 sub-objects",
                  num_obj, num_tex);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Resolve fire / smoke textures per forensics §4.
    auto resolve_subobj_tex = [&](int32_t objnum) -> TTextureHandle {
        const int32_t slot = BurnSubObjTextureSlot(img3d, objnum);
        const int32_t safe_slot = (slot >= 0 && slot < num_tex) ? slot : 0;
        S3DTex tex = {};
        img3d->GetTexture(safe_slot, &tex);
        return tex.htexture;
    };
    const TTextureHandle tex_smoke = resolve_subobj_tex(0);  // smoke sub-object
    const TTextureHandle tex_fire  = resolve_subobj_tex(1);  // smoke01 sub-object (fire)
    if (tex_fire == kInvalidTexture && tex_smoke == kInvalidTexture)
    {
        log_error("[burn-bespoke] SpawnForTest: both sub-object textures invalid");
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* burn = new TBurnEffect_Bespoke(base);
    burn->ForcePos(origin);
    burn->SetMapIndex(MapPane.MakeIndex());
    burn->ActivateComponents();
    burn->tex_fire_  = tex_fire;
    burn->tex_smoke_ = tex_smoke;

    // Snapshot TBurnAnimator::Initialize (effect_old.cpp:3292-3313):
    //   T3DAnimator::Initialize()
    //   size = ca->NumObjects()          <-- harness drift: no rig, default 1
    //   fire.Init(...) / smoke.Init(...) <-- our particle arena is pre-zeroed
    //   frame = 0; to_add = 0;
    burn->frame_         = 0;
    burn->to_add_        = 0;
    burn->size_          = 1;     // harness has no CharAnimator
    burn->alive_         = true;
    burn->sim_accum_ms_  = 0.0;
    for (auto& part : burn->particles_)
        part.used = false;

    log_info("[burn-bespoke] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "tex_fire=%u tex_smoke=%u numobj=%d numtex=%d",
             kBurnBespokeImageryPath, burn->GetMapIndex(),
             origin.x, origin.y, origin.z,
             tex_fire, tex_smoke, num_obj, num_tex);
    return burn;
}

void TBurnEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate: port of TBurnAnimator::Animate
    // (src/effect_old.cpp:3315-3447). Snapshot ungated; drift to 24 Hz
    // sim-tick gate per feedback-framerate-independent-anim.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kBurnBespokeSimTickMs))
        {
            sim_accum_ms_ -= double(kBurnBespokeSimTickMs);

            ++frame_;                                                // :3328
            if (frame_ < kBurnBespokeFrame && to_add_ < kBurnBespokeAdd)
                ++to_add_;                                           // :3329-3330 ramp up
            else if (frame_ >= kBurnBespokeFrame && to_add_ > 0)
                --to_add_;                                           // :3331-3332 ramp down

            // 1. fire→smoke promotion (:3339-3357).
            //    Iterate fire half [0..kBurnBespokeCount); on life ==
            //    life_span, copy into smoke half [count..count*2) with
            //    ×1.25 scl and ×2/3 life_span.
            for (int32_t i = 0; i < kBurnBespokeCount; ++i)
            {
                SBurnBespokeParticle& particle = particles_[i];
                if (!particle.used || particle.system != 0)
                    continue;

                if (particle.life == particle.life_span)
                {
                    particle.used = false;                           // :3347 free fire slot

                    // Compute promoted values from the dying particle.
                    SBurnBespokeParticle promoted = particle;
                    promoted.life_span = (particle.life_span * 2) / 3;   // :3349
                    promoted.scl.X *= 1.25f;                          // :3351
                    promoted.scl.Y *= 1.25f;                          // :3352
                    promoted.scl.Z *= 1.25f;                          // :3353
                    promoted.life   = 0;
                    promoted.system = 1;                              // smoke
                    promoted.used   = true;

                    // smoke.Add(particle) — find first unused slot in
                    // the smoke half [kBurnBespokeCount, 2*kBurnBespokeCount).
                    for (int32_t k = kBurnBespokeCount; k < kBurnBespokeCount * 2; ++k)
                    {
                        if (!particles_[k].used)
                        {
                            particles_[k] = promoted;
                            break;
                        }
                    }
                }
            }

            // 2. fire.Animate() + smoke.Animate()  (:3359-3360)
            //    TParticleSystem::Animate (effectcomp.cpp:1041-1068):
            //      if (life >= life_span) used = false; continue;
            //      ++life; pos += vel; vel *= acc;
            for (auto& particle : particles_)
            {
                if (!particle.used)
                    continue;
                if (particle.life >= particle.life_span)
                {
                    particle.used = false;
                    continue;
                }
                ++particle.life;
                particle.pos.X += particle.vel.X;
                particle.pos.Y += particle.vel.Y;
                particle.pos.Z += particle.vel.Z;
                particle.vel.X *= particle.acc.X;
                particle.vel.Y *= particle.acc.Y;
                particle.vel.Z *= particle.acc.Z;
            }

            // 3. Spawn `to_add_` new fire particles at random bones.
            //    Drift: no CharAnimator, so spawn at effect origin (0,0,0
            //    local) with ±BURN_SPREAD jitter per axis (forensics §5
            //    bone-emit path simplified). The MakeMatrix /
            //    GetObjectMatrix / D3DMATRIXTransform chain collapses to
            //    "world origin offset 0 + jitter". Per-particle physics
            //    is preserved verbatim per :3362-3413.
            const S3DPoint& origin = Pos();
            for (int32_t i = 0; i < to_add_; ++i)
            {
                // Find a free fire slot.
                int32_t free_idx = -1;
                for (int32_t k = 0; k < kBurnBespokeCount; ++k)
                {
                    if (!particles_[k].used)
                    {
                        free_idx = k;
                        break;
                    }
                }
                if (free_idx < 0)
                    break;       // fire arena full

                SBurnBespokeParticle& p = particles_[free_idx];

                // Drift: world-space emit position. snapshot picks a
                // random bone j via ca->GetObject + ca->GetObjectMatrix;
                // we have no rig so size_ = 1 and the bone matrix
                // collapses to identity. Random(0, size-1) = 0 always.
                const int32_t j = (size_ > 1) ? random(0, size_ - 1) : 0;
                (void)j;
                p.pos.X = float(origin.x);
                p.pos.Y = float(origin.y);
                p.pos.Z = float(origin.z);

                p.pos.X += float(random(-kBurnBespokeSpread, kBurnBespokeSpread));   // :3387
                p.pos.Y += float(random(-kBurnBespokeSpread, kBurnBespokeSpread));   // :3388
                p.pos.Z += float(random(-kBurnBespokeSpread, kBurnBespokeSpread));   // :3389

                const float scale = float(random(kBurnBespokeMinScl, kBurnBespokeMaxScl)) * 0.01f;
                p.scl.X = scale;                                                     // :3393
                p.scl.Y = scale;                                                     // :3394
                p.scl.Z = scale;                                                     // :3395

                // p.rot.x = -90; p.rot.z = -45; p.rot.y = 0  (:3398-3400)
                // No rotation lane on SBillboardDrawItem — preserved as
                // dead semantic state (the snapshot bakes these into the
                // matrix at Render in TParticleSystem::Render
                // effectcomp.cpp:1086-1090).

                p.acc.X = 1.0f;                                                      // :3403
                p.acc.Y = 1.0f;
                p.acc.Z = 1.0f;

                p.vel.X = 0.0f;                                                      // :3406
                p.vel.Y = 0.0f;                                                      // :3407
                p.vel.Z = float(random(kBurnBespokeMinZ, kBurnBespokeMaxZ)) * 0.1f;  // :3408

                p.life_span = random(kBurnBespokeMinLife, kBurnBespokeMaxLife);      // :3410
                p.life      = 0;
                p.system    = 0;        // fire system
                p.used      = true;                                                  // fire.Add :3412
            }

            // 4. Per-tick scl decay (:3415-3440) and `done` check.
            bool done = true;
            for (auto& particle : particles_)
            {
                if (!particle.used)
                    continue;
                done = false;
                particle.scl.X *= kBurnBespokeDec;
                particle.scl.Y *= kBurnBespokeDec;
                particle.scl.Z *= kBurnBespokeDec;
            }

            if (frame_ >= kBurnBespokeFrame && to_add_ == 0 && done)
            {
                alive_ = false;                                       // KillThisEffect :3444
                break;
            }
        }
    }

    // --- Render: port of TBurnAnimator::Render
    // (src/effect_old.cpp:3449-3460). SetAddBlendState() = AdditiveStraight
    // (DECALALPHA + ONE/ONE). One particle = one billboard with the
    // particle's per-axis scale applied uniformly (snapshot
    // TParticleSystem::Render bakes Scale·Rot·Translate into the I3D
    // sub-object's per-instance matrix; in the FB pipeline we use the
    // billboard's size_wu lane for the scale and let the particle's
    // world pos drive translate. Per-quad rot.x=-π/2 + rot.z=-π/4 from
    // effectcomp.cpp:1086-1090 is preserved as dead semantic state per
    // the drift note above — billboards have no rotation lane in
    // SBillboardDrawItem; if visual reads wrong, switch to
    // SParticleDrawItem and route rotation_rad).
    SBillboardDrawItem item = {};
    item.color_rgba[0]   = 1.0f;
    item.color_rgba[1]   = 1.0f;
    item.color_rgba[2]   = 1.0f;
    item.color_rgba[3]   = 1.0f;
    item.uv_rect[0]      = 0.0f;
    item.uv_rect[1]      = 0.0f;
    item.uv_rect[2]      = 1.0f;
    item.uv_rect[3]      = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);   // SetAddBlendState :3452
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;                   // fire-family unlit (§7)
    item.orientation     = EFxBillboardOrientation::ScreenAligned;

    for (const auto& particle : particles_)
    {
        if (!particle.used)
            continue;

        const TTextureHandle tex = (particle.system == 0) ? tex_fire_ : tex_smoke_;
        if (tex == kInvalidTexture)
            continue;

        item.key.texture  = tex;
        item.world_pos[0] = particle.pos.X;
        item.world_pos[1] = particle.pos.Y;
        item.world_pos[2] = particle.pos.Z;
        // Particle scale is uniform per axis at spawn; pick X as the
        // scalar (Y/Z identical). Multiply by base_size_wu_ to convert
        // the [0.15, 0.40]-ish range into a readable on-screen quad.
        const float quad = particle.scl.X * base_size_wu_;
        item.size_wu[0]   = quad;
        item.size_wu[1]   = quad;
        Renderer->SubmitFxBillboard(item);
    }
}
// --- end TBurnEffect_Bespoke faithful port

// *************************************************************************
// * TMissileEffect_Bespoke — S08 base infrastructure (faithful direct port)
// *************************************************************************
//
// Faithful translation of TMissileEffect::Initialize / SetSpeed / Pulse /
// OffScreen from src/missileeffect.cpp:31-141 (whole file under #if 0).
// Line-by-line port — same variable names, same constants, same per-tick
// math. The only adaptations are:
//   1. `Move()` collapses to an inline `pos += vel` per tick (the engine's
//      collision-integrating mover is not wired for the --test=vfx path).
//   2. The MOVE_BLOCKED / TMapIterator(character)-hit branches in FLY are
//      no-ops here (no map collision, no live characters in the harness);
//      the range countdown still drives the EXPLODE transition.
//   3. The harness draws a single state-colored marker billboard at the
//      missile world-pos using the renderer's white texture — TMissileEffect
//      has no visual of its own in retail (each leaf supplies its own
//      animator), so the marker exists only to prove state transitions.

void TMissileEffect_Bespoke::Initialize()
{
    // Port of TMissileEffect::Initialize (missileeffect.cpp:31-40).
    state_ = kMissileLaunch;
    angle  = 0;            // TEffect::angle (snapshot `angle = 0;`)
    range_ = 32768;

    SetSpeed(kMissileSpeedDefault);   // SetSpeed(16) -> speed = 16 * ROLLOVER

    status_ = false;

    // Harness-only members (not in snapshot Initialize).
    aim_angle_ = 0;
    vel_ = {0.0f, 0.0f, 0.0f};
    launch_hold_ticks_remaining_ = kMissileHarnessLaunchHoldTicks;
    explode_ticks_remaining_ = 24;
    alive_ = true;
    sim_accum_ms_ = 0.0;
}

void TMissileEffect_Bespoke::Pulse()
{
    // Port of TMissileEffect::Pulse (missileeffect.cpp:52-135), translated
    // line-by-line. The snapshot's `Move()` becomes an inline `pos += vel`
    // here so the marker moves without engine collision involvement.

    // Snapshot: `uint32_t bits = Move();` — Move() integrates vel and
    // returns flags. We integrate inline in FLY state.
    uint32_t bits = 0;   // MOVE_BLOCKED never set in harness (no collision)
    if (state_ == kMissileFly)
    {
        S3DPoint p = Pos();
        p.x += int32_t(vel_.X);
        p.y += int32_t(vel_.Y);
        p.z += int32_t(vel_.Z);
        ForcePos(p);
    }

    switch (state_)
    {
        case kMissileLaunch:
        {
            // missileeffect.cpp:58-72.
            if (status_)
            {
                // Harness-only: hold LAUNCH for ~12 ticks so the
                // LAUNCH -> FLY edge is observable in --test=vfx. The
                // in-game caller leaves launch_hold_=0 so this collapses
                // to the original 1-tick transition.
                if (launch_hold_ticks_remaining_ > 0)
                {
                    --launch_hold_ticks_remaining_;
                    break;
                }

                if (speed_)
                {
                    // ConvertToVector(GetAngle(), speed, vel) — emits an
                    // integer S3DPoint scaled by speed. Use aim_angle_
                    // (the byte-angle stored at spawn) since the harness
                    // overrides TEffect::angle.
                    const int32_t harness_speed =
                        int32_t(float(speed_) * kMissileHarnessSpeedScale);
                    S3DPoint v_int = {0, 0, 0};
                    ConvertToVector(aim_angle_, harness_speed, v_int);
                    vel_.X = float(v_int.x) / float(ROLLOVER);
                    vel_.Y = float(v_int.y) / float(ROLLOVER);

                    // snapshot sets flags = (flags & ~OF_IMMOBILE)
                    //                       | OF_MOVING | OF_WEIGHTLESS
                    // — replicated for any engine-side code path that
                    // reads them. In-harness collision is bypassed.
                    flags = (flags & ~OF_IMMOBILE) | OF_MOVING | OF_WEIGHTLESS;

                    // missileeffect.cpp:66 — vel.z = (speed / -16)
                    // (slight downward arc). `speed / -16` here uses the
                    // SCALED harness speed so the arc matches the
                    // harness flight envelope.
                    vel_.Z = float(harness_speed) / -16.0f / float(ROLLOVER);

                    // missileeffect.cpp:68 — range = (240*MISSILE_RANGE) /
                    //                                (speed/ROLLOVER)
                    // = (240*2)/(16) = 30 ticks of flight at default speed.
                    const int32_t speed_per_tick = speed_ / ROLLOVER;
                    if (speed_per_tick > 0)
                        range_ = (240 * kMissileRangeDefault) / speed_per_tick;
                }
                state_ = kMissileFly;
            }
            break;
        }

        case kMissileFly:
        {
            // missileeffect.cpp:75-124.
            bool explode = false;

            range_--;
            if (range_ <= 0)
                explode = true;

            // missileeffect.cpp:83-84 — MOVE_BLOCKED branch. bits is 0 in
            // the harness so this never fires; preserved for the eventual
            // in-game wiring.
            if (bits & MOVE_BLOCKED)
                explode = true;
            else
            {
                // missileeffect.cpp:87-113 — character-hit branch
                // (TMapIterator over OBJSET_CHARACTER). Skipped in the
                // harness (no live characters); preserved verbatim under
                // `#if 0` below in this same file as the in-game port
                // reference. For the bespoke base path we trust the
                // range countdown to drive EXPLODE.
            }

            if (explode)
            {
                // missileeffect.cpp:119-120 — stop the missile dead.
                flags = (flags & ~OF_MOVING & ~OF_WEIGHTLESS) | OF_IMMOBILE;
                state_ = kMissileExplode;
            }
            break;
        }

        case kMissileExplode:
        {
            // missileeffect.cpp:127-130 — `if (!HasAnimator()) KillThisEffect();`
            // The bespoke base has no animator, so we use an explicit
            // post-explode tick counter to keep the marker visible long
            // enough that the EXPLODE state is observable in --test=vfx.
            if (explode_ticks_remaining_ > 0)
            {
                --explode_ticks_remaining_;
            }
            else
            {
                alive_ = false;
            }
            break;
        }
    }

    // missileeffect.cpp:134 — TEffect::Pulse() chain.
    TEffect::Pulse();
}

TMissileEffect_Bespoke* TMissileEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[missile] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    // Pure infra base — no I3D imagery required. Construct with null
    // imagery (same pattern as TStripEffect::SpawnForTest at
    // src/stripeffect.cpp:150). The marker billboard uses the renderer's
    // white texture.
    auto* m = new TMissileEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    m->ForcePos(origin);
    m->SetMapIndex(MapPane.MakeIndex());
    m->ActivateComponents();

    // Harness aim: random horizontal byte-angle (in-game caller would
    // supply this via SetAngle()).
    m->aim_angle_ = random(0, 255);
    // missileeffect.cpp:39 — status starts false; the animator drives it
    // true on its first tick. In the harness we set it true immediately
    // so the state machine proceeds (no animator to wait on).
    m->status_ = true;

    log_info("[missile] SpawnForTest: map_index=%d origin=(%d,%d,%d) aim=%d "
             "speed=%d (per-tick=%d) range=%d",
             m->GetMapIndex(), origin.x, origin.y, origin.z,
             m->aim_angle_, m->speed_, m->GetSpeed(), m->range_);
    return m;
}

void TMissileEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: drive Pulse() via the 24 Hz sim-tick accumulator (same
    // gate B01/F07/B-family use) so motion is framerate-independent.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kMissileSimTickMs))
        {
            sim_accum_ms_ -= double(kMissileSimTickMs);
            Pulse();
            if (!alive_)
                break;
        }
    }

    // --- Render: ONE marker billboard at the missile world-pos, colored
    // by state. TMissileEffect has no visual in retail (each leaf supplies
    // its own animator); the marker exists only to prove the state-machine
    // ticks LAUNCH -> FLY -> EXPLODE in --test=vfx.
    const S3DPoint& p = Pos();

    float r = 1.0f, g = 1.0f, b = 1.0f;
    switch (state_)
    {
        case kMissileLaunch:  r = 1.0f; g = 1.0f; b = 0.2f; break;   // yellow
        case kMissileFly:     r = 1.0f; g = 0.55f; b = 0.0f; break;  // orange
        case kMissileExplode: r = 1.0f; g = 0.15f; b = 0.10f; break; // red
        default: break;
    }

    SBillboardDrawItem item = {};
    item.world_pos[0]  = float(p.x);
    item.world_pos[1]  = float(p.y);
    item.world_pos[2]  = float(p.z);
    item.size_wu[0]    = kMissileMarkerSizeWu;
    item.size_wu[1]    = kMissileMarkerSizeWu;
    item.color_rgba[0] = r;
    item.color_rgba[1] = g;
    item.color_rgba[2] = b;
    item.color_rgba[3] = 1.0f;
    item.key.texture     = Renderer->WhiteTextureHandle();
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    Renderer->SubmitFxBillboard(item);
}
// --- end S08 TMissileEffect_Bespoke port

#if 0
// REVISITED (earlier WIP): pre-faithful-port body, re-derived blood
// through a TParticleBucket directly (no effects.def, no chain). Kept
// here as historical context only; the resettled engine-driven rework
// at the top of this section is the live path. Renamed to
// SpawnForTest_BESPOKE_WIP to disambiguate from the live bespoke
// faithful-port class above.
TBloodEffect* TBloodEffect::SpawnForTest_BESPOKE_WIP(const S3DPoint& origin)
{
    const int32_t img_id = TObjectImagery::FindImagery(kBloodImageryPath);
    if (img_id < 0)
    {
        log_error("[blood] SpawnForTest: FindImagery('%s') failed", kBloodImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[blood] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kBloodImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[blood] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kBloodImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* blood = new TBloodEffect(base);
    blood->ForcePos(origin);
    blood->SetMapIndex(MapPane.MakeIndex());
    blood->ActivateComponents();

    blood->bucket_ = AcquireBloodBucket(img3d);
    if (!blood->bucket_)
    {
        // Imagery loaded but bucket couldn't be built (texture not ready
        // / asset structurally unexpected). The TBloodEffect itself is
        // still valid — TickAndSubmitForTest is a no-op without a bucket
        // so the harness still navigates past B01 cleanly.
        log_warn("[blood] SpawnForTest: bucket unavailable — B01 will draw nothing");
        return blood;
    }
    blood->owner_particle_id_ = NextBloodOwnerId();

    // Spawn the entire burst right at construction. Blood is a discrete
    // impact: a single splat at one wound point. The harness drives
    // re-trigger by destroying and respawning this effect at varying
    // positions per its EVfxPreviewStyle::Combat setting.
    constexpr int32_t kBurstCount = 10;
    constexpr float   kLife       = 1.4f;
    TParticleBucket& bucket = *blood->bucket_;
    const S3DPoint& p = blood->Pos();
    for (int32_t k = 0; k < kBurstCount; ++k)
    {
        const int32_t pi = bucket.AddParticle(blood->owner_particle_id_, kLife);
        if (pi < 0) break;
        const float u1 = float(std::rand()) / float(RAND_MAX);
        const float u2 = float(std::rand()) / float(RAND_MAX);
        const float angle  = u1 * 6.28318530718f;
        const float radial = 60.0f + 80.0f * u2;     // wu/s
        const float upward = 140.0f + 80.0f * u2;    // wu/s (initial upward burst)
        if (float* dp = bucket.VarPtr(pi, EParticleVar::DrawPos))
        {
            dp[0] = float(p.x);
            dp[1] = float(p.y);
            dp[2] = float(p.z) + 30.0f;
        }
        if (float* vel = bucket.VarPtr(pi, EParticleVar::EmitVel))
        {
            vel[0] = std::cos(angle) * radial;
            vel[1] = std::sin(angle) * radial;
            vel[2] = upward;
        }
        if (float* ds = bucket.VarPtr(pi, EParticleVar::DrawScl))
        {
            const float s = 24.0f + 16.0f * u1;
            ds[0] = s; ds[1] = s; ds[2] = 1.0f;
        }
        if (float* df = bucket.VarPtr(pi, EParticleVar::DrawFrame))
            *df = float(std::rand() % 4);                 // pick 1 of 4 splat variants
        if (float* dr = bucket.VarPtr(pi, EParticleVar::DrawRot))
            *dr = float(std::rand()) / float(RAND_MAX) * 6.28318530718f;
        if (float* col = bucket.VarPtr(pi, EParticleVar::DrawColor))
        {
            col[0] = 1.00f - 0.20f * u2;
            col[1] = 0.10f + 0.10f * u1;
            col[2] = 0.10f + 0.05f * u2;
            col[3] = 1.0f;
        }
    }

    log_info("[blood] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "owner_id=%.0f textures=%d burst=%d",
             kBloodImageryPath, blood->GetMapIndex(),
             origin.x, origin.y, origin.z,
             blood->owner_particle_id_, img3d->NumTextures(), kBurstCount);
    return blood;
}

void TBloodEffect::TickAndSubmitForTest_BESPOKE_WIP(EFxDebugMode debug_mode)
{
    if (!bucket_ || !Renderer)
        return;

    const float dt = float(TTime::DeltaTime());
    age_ += dt;

    // 1. Integrate existing particles owned by this instance.
    //    Simple Euler with constant downward acceleration ("gravity").
    //    Faithful retail kinematics + splat-stick deferred to 2.2.1.
    //    All burst particles were spawned in SpawnForTest -- we never
    //    spawn more here. The harness re-triggers by destroying this
    //    effect and spawning a fresh one (EVfxPreviewStyle::Combat).
    constexpr float kGravity = -480.0f;   // wu / s^2 (rough; looks right at default scale)
    for (int32_t i = 0; i < bucket_->Count(); ++i)
    {
        const float* owner = bucket_->VarPtr(i, EParticleVar::OwnerId);
        if (!owner || *owner != owner_particle_id_)
            continue;
        float* vel = bucket_->VarPtr(i, EParticleVar::EmitVel);
        float* pos = bucket_->VarPtr(i, EParticleVar::DrawPos);
        float* age = bucket_->VarPtr(i, EParticleVar::Age);
        float* life_p = bucket_->VarPtr(i, EParticleVar::Life);
        float* col = bucket_->VarPtr(i, EParticleVar::DrawColor);
        if (vel)
            vel[2] += kGravity * dt;
        if (pos && vel)
        {
            pos[0] += vel[0] * dt;
            pos[1] += vel[1] * dt;
            pos[2] += vel[2] * dt;
        }
        if (age)
            *age += dt;
        // Alpha curve: hold at full opacity for the first half of
        // life, then linearly fade to 0 over the second half. The
        // shader pairs (sampled premul) * (tint.a) so the fade is
        // proportional in both rgb and alpha -- no pink shift.
        if (col && age && life_p && *life_p > 0.0f)
        {
            const float t = *age / *life_p;
            const float fade = (t < 0.5f) ? 1.0f
                             : (t >= 1.0f) ? 0.0f
                             : (1.0f - 2.0f * (t - 0.5f));
            col[3] = fade;
        }
    }

    // 2. Reap expired particles (age > life) for this owner. The bucket's
    //    AddParticle / KillParticlesByOwner are the only mutators; we
    //    swap-and-pop by owner-stamping dead particles back to a sentinel
    //    then killing that sentinel in one pass.
    constexpr float kDeadSentinel = -7777.0f;
    bool any_dead = false;
    for (int32_t i = 0; i < bucket_->Count(); ++i)
    {
        const float* o = bucket_->VarPtr(i, EParticleVar::OwnerId);
        const float* a = bucket_->VarPtr(i, EParticleVar::Age);
        const float* l = bucket_->VarPtr(i, EParticleVar::Life);
        if (!o || *o != owner_particle_id_) continue;
        if (a && l && *l > 0.0f && *a >= *l)
        {
            if (float* mut_o = bucket_->VarPtr(i, EParticleVar::OwnerId))
            {
                *mut_o = kDeadSentinel;
                any_dead = true;
            }
        }
    }
    if (any_dead)
        bucket_->KillParticlesByOwner(kDeadSentinel);

    // 4. Hand the bucket to the FX queue. The renderer drains it inside
    //    RunLightingPass.
    Renderer->SubmitFxParticleBucket(*bucket_, debug_mode);
}
#endif // bespoke TBloodEffect spawn/tick preserved-old-code

// *************************************************************************
// * TFizzleEffect - FAITHFUL DIRECT PORT for --test=vfx (X21)             *
// *************************************************************************
//
// Reconstruction of TFizzleAnimator (effect_old.cpp:12347-12516) +
// TParticleSystem::Animate/Render (effectcomp.cpp:1041-1109). Same model
// as TSparkEffect / TBloodEffect: collapse the per-system particle arrays
// onto the effect class, integrate at the original 24 Hz cadence via a
// real-time accumulator (RECONSTRUCTION_PROTOCOL framerate-independence
// rule), drive everything off the snapshot constants in forensics §3.
//
// The original organizes particles into 3 TParticleSystem instances
// (blue/red/purple), each bound to one sub-object of Magic\Fizzle.I3D.
// Render walks each system in turn; the matrix it builds per particle is:
//
//    M = RotateX(rot.x) * RotateY(rot.y) * RotateZ(rot.z)        // (rot.x/y=0)
//      * RotateX(-pi/2)                                          // ground tip
//      * RotateZ(-pi/4)                                          // static spin
//      * RotateZ(facing)                                         // facing=0
//      * Scale(scl * [1.5 if flicker])
//      * Translate(pos)
//
// Mapped to the FB-particle pipeline:
//   - WorldXY orientation handles the RotateX(-pi/2) ground tip — the
//     quad's 4 corners are placed on the world XY plane at the particle
//     anchor before iso projection.
//   - rotation_rad (per-instance) handles the in-plane spin: rot.z (live
//     animator value) + (-pi/4) (static) folded into one angle. Facing is
//     zero for Fizzle (forensics §13.5), so the third RotateZ is a no-op.
//   - The flicker ×1.5 scale multiplier (forensics §6.2 / §7) is applied
//     by widening size_wu in the render submit.
//
// We use SubmitFxParticle rather than SubmitFxBillboard because billboards
// don't carry a per-instance rotation_rad — and the per-particle spin IS
// the visible identity of the puff.

namespace {

constexpr const char* kFizzleImageryPath = "Magic\\Fizzle.I3D";

// 24Hz cadence gate (family-consistent with F03/H03/M05/X22/B01). The
// original per-tick rates (DUST_SCL_INC=0.05/tick grow, DUST_SCL_DEC=0.02/
// tick shrink, DUST_ADD=1.5/tick emission, DUST_ROT=15deg/tick max spin,
// DUST_FRAME=15 tick emission window) integrate once per sim-tick of
// accumulated wall-clock time. See forensics §3 + §6.2.
constexpr int32_t kFizzleSimTickMs = 1000 / 24;

// Constants — straight from effect_old.cpp:12293-12303 (forensics §3).
// Snapshot-only (the retail animator body is not extracted); rate of
// these is "best-evidence, visually match in-game" per §2.1(1) / §13.0.
constexpr int32_t kFizzleDustCount     = kFizzleParticlesPerSystem; // 30
constexpr int32_t kFizzleDustFrame     = 15;       // emission window (ticks)
constexpr int32_t kFizzleDustSpread    = 15;       // ± xy jitter at spawn (wu)
constexpr int32_t kFizzleDustMinZ      = 5;        // ×0.1 -> 0.5 wu/tick min fall
constexpr int32_t kFizzleDustMaxZ      = 45;       // ×0.1 -> 4.5 wu/tick max fall
constexpr int32_t kFizzleDustRot       = 15;       // ± deg/tick spin rate
constexpr int32_t kFizzleDustMinScl    = 5;        // ×0.01 -> 0.05 min max-scale
constexpr int32_t kFizzleDustMaxScl    = 25;       // ×0.01 -> 0.25 max max-scale
constexpr float   kFizzleDustSclInc    = 0.05f;    // scale/tick GROW
constexpr float   kFizzleDustSclDec    = 0.02f;    // scale/tick SHRINK
constexpr float   kFizzleDustAdd       = 1.5f;     // emission acc /tick
constexpr int32_t kFizzleSpawnZMin     = 70;       // wu above origin
constexpr int32_t kFizzleSpawnZMax     = 130;      // wu above origin

// Phase tags (forensics §6.2 / §13.3 — life_span is OVERLOADED as a phase
// tag, not a tick countdown).
constexpr int32_t kFizzlePhaseGrow     = 100;
constexpr int32_t kFizzlePhaseShrink   = 200;
constexpr int32_t kFizzlePhaseDead     = 0;

// Static in-plane Z-axis rotation from TParticleSystem::Render
// (effectcomp.cpp:1089) — folded into rotation_rad alongside the live
// per-particle rot.z so we don't have to chain transforms per draw.
constexpr float   kFizzleStaticRotRad  = -float(M_PI) / 4.0f;

// Flicker scale boost (effectcomp.cpp:1095-1097).
constexpr float   kFizzleFlickerScale  = 1.5f;

// Base quad world-unit size. The original drew the I3D's authored quad
// (each box01/02/03 is a textured 2-triangle quad sized by its verts);
// the FB-particle pipeline submits a wu-sized billboard, so we pick a
// base size and the per-particle `scl` curve (0..max_scl in [0.05..0.25])
// multiplies it. The 32×32 sprite at peak scale 0.25 reads as a small
// ~8 wu puff at typical camera distances; pick a base that gives the
// peak a visible-but-modest footprint without dominating the burst.
constexpr float   kFizzleBaseSizeWu    = 96.0f;

// Resolve which texture slot a sub-object draws from by walking its
// texfaces[] table. Each S3DObj records one nonzero entry in
// numtexfaces[1..MAXTEXTURES] for the single texture that actually
// renders its quad (index 0 is no-texture). Returns -1 if none found.
// Duplicated locally rather than #include'd because the helper in the
// blood section above is `static` to that anonymous namespace; keep this
// file's structure preserve-and-add (no cross-section coupling).
int32_t FizzleSubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0)
        return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

// Read a sub-object's authored UV sub-rect from its vertex list (same
// shape as ResolveSubObjUv above for blood / the spark UV-cell extraction).
void FizzleResolveSubObjUv(T3DImagery* img3d, int32_t objnum, float out[4])
{
    out[0] = 0.0f; out[1] = 0.0f; out[2] = 1.0f; out[3] = 1.0f;
    if (!img3d) return;
    const int32_t nverts = img3d->NumObjVerts(objnum);
    if (nverts <= 0) return;
    std::vector<S3DVertex> vbuf(static_cast<size_t>(nverts), S3DVertex{});
    img3d->GetObjVerts(objnum, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
    float minu = vbuf[0].tu, maxu = vbuf[0].tu;
    float minv = vbuf[0].tv, maxv = vbuf[0].tv;
    for (int32_t i = 1; i < nverts; ++i)
    {
        if (vbuf[i].tu < minu) minu = vbuf[i].tu;
        if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
        if (vbuf[i].tv < minv) minv = vbuf[i].tv;
        if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
    }
    out[0] = minu;
    out[1] = minv;
    out[2] = maxu - minu;
    out[3] = maxv - minv;
}

// Find an unused slot for spawn. Mirrors TParticleSystem::Add's free-slot
// search (effectcomp.cpp:1133-1158). Returns -1 if the cap is hit.
// Collapsed-array variant: only consider slots tagged with the requested
// system (the original kept 3 separate arrays, so a slot was implicitly
// system-typed — here we search by system field).
int32_t FizzleFindFreeSlot(SFizzleParticle particles[], int32_t system_idx)
{
    // Each system gets its own contiguous range of slots so we preserve
    // the original's per-system DUST_COUNT cap (effect_old.cpp:12316,
    // ::Add at effectcomp.cpp:1133-1158 walks the system's own array
    // until it finds `!used`). System 0 owns slots [0..30), system 1
    // owns [30..60), system 2 owns [60..90).
    const int32_t base = system_idx * kFizzleParticlesPerSystem;
    const int32_t end  = base + kFizzleParticlesPerSystem;
    for (int32_t i = base; i < end; ++i)
        if (!particles[i].used)
            return i;
    return -1;
}

}   // namespace

TFizzleEffect* TFizzleEffect::SpawnForTest(const S3DPoint& origin)
{
    // --- Load the REAL Magic\Fizzle.I3D imagery (no procedural stand-in).
    // The 3 dust-puff sprites (blue/purple/magenta) ARE the visual
    // identity per forensics §10 — substituting procedural dust is
    // explicitly called out as a bug in §10 / no-standins feedback.
    const int32_t img_id = TObjectImagery::FindImagery(kFizzleImageryPath);
    if (img_id < 0)
    {
        log_error("[fizzle] SpawnForTest: FindImagery('%s') failed", kFizzleImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[fizzle] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFizzleImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fizzle] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kFizzleImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* fizzle = new TFizzleEffect(base);
    fizzle->ForcePos(origin);
    fizzle->SetMapIndex(MapPane.MakeIndex());
    fizzle->ActivateComponents();

    // Lazy-mesh-init poke (mirror F01 / H04 / X22 / B01). NumObjects
    // triggers the actual mesh load; NumTextures alone doesn't.
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < kFizzleNumSystems || num_tex <= 0)
    {
        log_error("[fizzle] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d) — expected 3 box sub-objects",
                  num_obj, num_tex);
        delete fizzle;
        return nullptr;
    }

    // --- Sub-object resolution. The original (effect_old.cpp:12354-12356)
    // binds:
    //     blue.Init(this, GetObject(0), ...);
    //     red.Init(this, GetObject(2), ...);          // NOTE: index 2!
    //     purple.Init(this, GetObject(1), ...);       // NOTE: index 1!
    // So system 0 (blue)  -> sub-object 0 (box01, blue sprite)
    //    system 1 (red)   -> sub-object 2 (box03, magenta sprite)
    //    system 2 (purple)-> sub-object 1 (box02, violet/purple sprite)
    // This is the documented index↔label crossing from forensics §4. We
    // preserve it so the per-system random(1,3) distribution lines up
    // with what the original drew.
    const int32_t kSubObjForSystem[kFizzleNumSystems] = { 0, 2, 1 };
    const char*   kSystemLabel[kFizzleNumSystems]    = { "blue", "red", "purple" };
    for (int32_t s = 0; s < kFizzleNumSystems; ++s)
    {
        const int32_t obj = kSubObjForSystem[s];
        const int32_t tex_slot = FizzleSubObjTextureSlot(img3d, obj);
        const int32_t slot = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : obj;
        S3DTex tex = {};
        img3d->GetTexture(slot, &tex);
        fizzle->subobjs_[s].texture = tex.htexture;
        FizzleResolveSubObjUv(img3d, obj, fizzle->subobjs_[s].uv_rect);
        fizzle->subobjs_[s].size_wu = kFizzleBaseSizeWu;
        if (fizzle->subobjs_[s].texture == kInvalidTexture)
        {
            log_error("[fizzle] SpawnForTest: '%s' sub-object %d texture unresolved",
                      kSystemLabel[s], obj);
            delete fizzle;
            return nullptr;
        }
    }

    // Particles are seeded by the per-tick emission loop in
    // TickAndSubmitForTest (the original spawns over the first DUST_FRAME=15
    // ticks, NOT all at once at Initialize). Just zero the cadence state.
    fizzle->emit_add_    = 0.0f;
    fizzle->frame_count_ = 0;
    fizzle->sim_accum_ms_ = 0.0;
    fizzle->alive_       = true;

    log_info("[fizzle] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "subobjs={blue=box%d(tex=%u), red=box%d(tex=%u), purple=box%d(tex=%u)} "
             "uvs={blue=[%.3f,%.3f %.3fx%.3f], red=[%.3f,%.3f %.3fx%.3f], "
             "purple=[%.3f,%.3f %.3fx%.3f]}",
             kFizzleImageryPath, fizzle->GetMapIndex(),
             origin.x, origin.y, origin.z,
             kSubObjForSystem[0] + 1, fizzle->subobjs_[0].texture,
             kSubObjForSystem[1] + 1, fizzle->subobjs_[1].texture,
             kSubObjForSystem[2] + 1, fizzle->subobjs_[2].texture,
             fizzle->subobjs_[0].uv_rect[0], fizzle->subobjs_[0].uv_rect[1],
             fizzle->subobjs_[0].uv_rect[2], fizzle->subobjs_[0].uv_rect[3],
             fizzle->subobjs_[1].uv_rect[0], fizzle->subobjs_[1].uv_rect[1],
             fizzle->subobjs_[1].uv_rect[2], fizzle->subobjs_[1].uv_rect[3],
             fizzle->subobjs_[2].uv_rect[0], fizzle->subobjs_[2].uv_rect[1],
             fizzle->subobjs_[2].uv_rect[2], fizzle->subobjs_[2].uv_rect[3]);
    return fizzle;
}

void TFizzleEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TFizzleAnimator::Animate (effect_old.cpp:12361-12487)
    // + TParticleSystem::Animate (effectcomp.cpp:1041-1068). Framerate-
    // independent via the 24Hz sim-tick accumulator (same shape as
    // sparks/blood). Each accumulated tick runs the original per-tick
    // integration exactly once.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kFizzleSimTickMs))
        {
            sim_accum_ms_ -= double(kFizzleSimTickMs);

            // Emission cadence: add += DUST_ADD; ++frame_count
            // (effect_old.cpp:12367, :12371).
            emit_add_ += kFizzleDustAdd;
            ++frame_count_;

            // TParticleSystem::Animate (effectcomp.cpp:1045-1067) — per
            // used particle: integrate pos += vel, multiply vel *= acc
            // (acc=1.0 -> constant velocity, no gravity/drag for Fizzle).
            // The original's life/life_span lifecycle is COMPLETELY
            // bypassed in practice because the animator's scale machine
            // sets life_span=0 (kill) before any time-based death
            // condition triggers (forensics §6.1 note / §13.3 — life_span
            // is overloaded as a phase tag, NOT a tick-countdown). So
            // we only run the position integration here; the kill
            // condition lives in the scale state machine below.
            for (int32_t i = 0; i < kFizzleMaxParticles; ++i)
            {
                SFizzleParticle& p = particles_[i];
                if (!p.used)
                    continue;
                // pos += vel (acc=1.0, so vel never changes)
                p.pos.X += p.vel.X;
                p.pos.Y += p.vel.Y;
                p.pos.Z += p.vel.Z;
            }

            // --- Emission loop (effect_old.cpp:12378-12423).
            // Spawn ~1.5/tick for the first 15 ticks, distributed
            // randomly across the 3 systems. Each new particle has
            // scl=(0,0,0) (will GROW), random max-scale temp.z, random
            // spin rate temp.y, and phase=100 (GROW).
            while (emit_add_ > 1.0f && frame_count_ < kFizzleDustFrame)
            {
                emit_add_ -= 1.0f;

                // Match the original's r in {1,2,3} → system {blue, red,
                // purple}. Mapped to system_idx 0/1/2.
                const int32_t r = random(1, 3);
                const int32_t system_idx = r - 1;
                const int32_t slot = FizzleFindFreeSlot(particles_, system_idx);
                if (slot < 0)
                    continue;   // system full this tick — original behaviour:
                                // TParticleSystem::Add silently drops if no slot

                SFizzleParticle& p = particles_[slot];
                p.used = true;
                p.system = system_idx;
                // pos: (±DUST_SPREAD, ±DUST_SPREAD, random(70,130)) wu
                p.pos.X = float(random(-kFizzleDustSpread, kFizzleDustSpread));
                p.pos.Y = float(random(-kFizzleDustSpread, kFizzleDustSpread));
                p.pos.Z = float(random(kFizzleSpawnZMin, kFizzleSpawnZMax));
                // vel: (0, 0, -random(5,45)*0.1) — straight down only
                p.vel.X = 0.0f;
                p.vel.Y = 0.0f;
                p.vel.Z = -float(random(kFizzleDustMinZ, kFizzleDustMaxZ)) * 0.1f;
                // scl starts at 0 (invisible); will grow via the
                // state machine.
                p.scl.X = p.scl.Y = p.scl.Z = 0.0f;
                // rot.z starts at 0 — the original assigns a random
                // rot.z = random(0,359) then immediately overwrites all
                // rot to 0 on the next line (effect_old.cpp:12401-12402),
                // so the initial random spin NEVER takes effect
                // (forensics §13.2 — "do NOT reconstruct the dead line").
                p.rot_z_deg = 0.0f;
                // phase tag (life_span in the original; 100 = GROW)
                p.phase = kFizzlePhaseGrow;
                // flicker re-rolled each tick during the per-particle
                // pass; seed it now too (mirrors the spawn assignment).
                p.flicker = (random(0, 1) != 0);
                // spin rate temp.y = ±DUST_ROT deg/tick
                p.spin_rate = float(random(-kFizzleDustRot, kFizzleDustRot));
                // max-scale temp.z = random(DUST_MIN_SCL, DUST_MAX_SCL) * 0.01
                p.max_scl = float(random(kFizzleDustMinScl, kFizzleDustMaxScl)) * 0.01f;
            }

            // --- Per-particle scale state machine + spin + flicker
            // (effect_old.cpp:12426-12478). `done` tracks whether any
            // particle is still alive — if not (and emission is over),
            // the effect self-destructs.
            bool done = true;
            for (int32_t i = 0; i < kFizzleMaxParticles; ++i)
            {
                SFizzleParticle& p = particles_[i];
                if (!p.used)
                    continue;

                if (p.phase == kFizzlePhaseGrow)
                {
                    // scl += DUST_SCL_INC (per axis) — uniform grow
                    p.scl.X += kFizzleDustSclInc;
                    p.scl.Y += kFizzleDustSclInc;
                    p.scl.Z += kFizzleDustSclInc;
                    if (p.scl.X > p.max_scl)
                        p.phase = kFizzlePhaseShrink;
                }
                else if (p.phase == kFizzlePhaseShrink)
                {
                    // scl -= DUST_SCL_DEC (per axis) — uniform shrink
                    p.scl.X -= kFizzleDustSclDec;
                    p.scl.Y -= kFizzleDustSclDec;
                    p.scl.Z -= kFizzleDustSclDec;
                    if (p.scl.X <= 0.0f)
                    {
                        p.scl.X = p.scl.Y = p.scl.Z = 0.0f;
                        p.phase = kFizzlePhaseDead;
                        p.used  = false;     // free slot (mirror "life_span=0"
                                             // semantic: next ::Animate would
                                             // free in the original; we do it
                                             // here to keep the array clean)
                        continue;            // don't spin/flicker a dead particle
                    }
                }

                // Spin: rot.z += temp.y (deg), wrap [0,360).
                // (effect_old.cpp:12467-12472).
                p.rot_z_deg += p.spin_rate;
                while (p.rot_z_deg <    0.0f) p.rot_z_deg += 360.0f;
                while (p.rot_z_deg >= 360.0f) p.rot_z_deg -= 360.0f;

                // Re-roll flicker (effect_old.cpp:12475).
                p.flicker = (random(0, 1) != 0);

                done = false;
            }

            // --- Self-destruct (effect_old.cpp:12482-12485).
            if (frame_count_ >= kFizzleDustFrame && done)
            {
                alive_ = false;
                break;
            }
        }
    }

    // --- Render: port of TFizzleAnimator::Render (effect_old.cpp:12489-12502)
    // + TParticleSystem::Render (effectcomp.cpp:1070-1109). For each used
    // particle: build the rotation/scale/translate transform and submit
    // one billboard. Mapped to the FB-particle pipeline:
    //   - WorldXY orientation = the RotateX(-pi/2) ground tip.
    //   - rotation_rad        = rot.z (radians) + the static -pi/4 spin.
    //   - size_wu * scale     = the per-axis Scale call (×1.5 if flicker).
    //   - color_rgba          = (1,1,1,1) — no per-vertex tint in the
    //                           original; color is entirely in the sprite
    //                           texture (forensics §7 "Per-vertex color
    //                           packing: NONE written by the effect").
    //   - blend = Alpha       — TFizzleAnimator::Render calls
    //                           SetBlendState (= SRC_ALPHA / INV_SRC_ALPHA),
    //                           NOT SetAddBlendState (forensics §7).
    //   - light = Unlit       — animator never folds ambient into vertex
    //                           color; materials are neutral white
    //                           (forensics §7).
    //   - depth = TestNoWrite — SetBlendState sets ZWRITE=false.

    const S3DPoint& base = Pos();

    SParticleDrawItem item = {};
    item.color_rgba[0]   = 1.0f;
    item.color_rgba[1]   = 1.0f;
    item.color_rgba[2]   = 1.0f;
    item.color_rgba[3]   = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::WorldXY;
    item.debug_mode      = debug_mode;

    for (int32_t i = 0; i < kFizzleMaxParticles; ++i)
    {
        const SFizzleParticle& p = particles_[i];
        if (!p.used || p.scl.X <= 0.0f)
            continue;

        const SFizzleSubObject& so = subobjs_[p.system];
        if (so.texture == kInvalidTexture)
            continue;

        // World position = effect origin + object-local particle pos.
        // The original calls FIX_Z_VALUE on pos.z, which corrects from
        // local to world depth via the iso-projection conventions; we
        // submit world coordinates directly so the renderer applies its
        // own (sokol) projection and we don't double-apply the fix.
        item.world_pos[0] = float(base.x) + p.pos.X;
        item.world_pos[1] = float(base.y) + p.pos.Y;
        item.world_pos[2] = float(base.z) + p.pos.Z;

        // Scale: size_wu × current scl × (1.5 if flicker). The original
        // applies the boost to all three axes uniformly, so it's a
        // uniform size multiplier on the rendered quad.
        const float flicker_mult = p.flicker ? kFizzleFlickerScale : 1.0f;
        item.size_wu[0] = so.size_wu * p.scl.X * flicker_mult;
        item.size_wu[1] = so.size_wu * p.scl.Y * flicker_mult;

        // Spin: live per-particle rot.z (degrees -> radians) +
        // the static -pi/4 ground-plane spin (effectcomp.cpp:1089).
        // Facing is 0 for Fizzle (forensics §13.5), so no third term.
        const float rot_z_rad = p.rot_z_deg * float(M_PI) / 180.0f;
        item.rotation_rad = rot_z_rad + kFizzleStaticRotRad;

        item.uv_rect[0] = so.uv_rect[0];
        item.uv_rect[1] = so.uv_rect[1];
        item.uv_rect[2] = so.uv_rect[2];
        item.uv_rect[3] = so.uv_rect[3];
        item.key.texture = so.texture;

        Renderer->SubmitFxParticle(item);
    }
}

// *************************************************************************
// * TRippleEffect - FB-pipeline standalone-spawn for --test=vfx (H03)     *
// *************************************************************************
//
// Scope: Phase 2 H03 row. Ports the water-ring visual from
// `TRippleAnimator` (src/effect_old.cpp:10683-10947) to the FB pipeline,
// reusing the F01 standalone-spawn pattern.
//
// What this *does* deliver:
//   1. A real `TRippleEffect` instance that owns its lifecycle.
//   2. A procedural 4x4 atlas of expanding concentric-ring frames built
//      via `Renderer->RegisterTextureAsset` (no I3D asset on disk for
//      "ripple" — see INVENTORY H03 gap 7.2). Atlas frame layout mirrors
//      the pre-release `SetAnimFrame` UV math (`u = (frame%4)*0.25;
//      v = (frame/4)*0.25`) so the same `rippleframeof[]` reorder table
//      drives the right cell.
//   3. Per-frame ring scale growth + atlas frame cycle, sim-tick-gated
//      to retail's 24 Hz cadence (pre-release was ungated which would
//      run 2.5x too fast at 60 fps).
//   4. Submission as a single screen-aligned SBillboardDrawItem each
//      frame (the camera in vfxtest looks straight forward so a ring
//      reads as a ring; a flat-on-ground orientation would require a
//      new ground-quad pipeline which is out of scope here).
//
// What this does *not* deliver:
//   - Splash droplet sub-emit (H03a; needs PE bucket + recursive ripple
//     spawn — folded in when H04 TDripEffect lands).
//   - In-game spawn path through TDripAnimator (H04 dependency).
//   - Real I3D ripple imagery (H03b if/when asset is identified).
//
namespace {

constexpr int32_t kRippleAtlasCols     = 4;
constexpr int32_t kRippleAtlasRows     = 4;
constexpr int32_t kRippleAtlasCellPx   = 32;   // 32x32 per cell -> 128x128 atlas
constexpr int32_t kRippleAtlasSizePx   = kRippleAtlasCols * kRippleAtlasCellPx;
constexpr int32_t kRippleAtlasFrames   = kRippleAtlasCols * kRippleAtlasRows;
constexpr int32_t kRippleDefaultLength = 96;     // ~4 sec at 24 Hz, "big" ripple by retail RIPPLE_SMALLDURATION=24 cutoff
constexpr int32_t kRippleSimTickMs     = 1000 / 24;
constexpr float   kRippleScaleStep     = 1.0f / 16.0f;   // pre-release: scale += 1/16 per Animate (was per render frame; now per sim tick)
constexpr float   kRippleBaseSizeWu    = 80.0f;          // base ring diameter in world units; scale * this = actual diameter

// Pre-release frame-reorder table from effect_old.cpp:10864. Each row of
// 4 atlas cells is mirrored — the animator's logical frame index gets
// remapped before becoming the rendered cell.
constexpr int8_t kRippleFrameOf[16] = {
    3, 2, 1, 0,
    7, 6, 5, 4,
    11, 10, 9, 8,
    15, 14, 13, 12,
};

// Procedural 4x4 atlas of expanding concentric rings. Each cell is a
// kRippleAtlasCellPx square; cell index N represents the ripple at
// progress t = N / 15. Inside each cell we draw a soft ring at radius
// = base_radius + t * grow_radius with alpha falling smoothly off both
// sides of the ring; later cells (t > 0.5) also fade the whole-cell
// alpha to simulate the ripple dissipating into the water surface.
//
// Used over RGBA so we can keep a clean premultiplied alpha + bright
// rim, identical to S01 LightningGlowTexture's blend-friendly pattern.
TTextureHandle RippleAtlasTexture()
{
    if (!Renderer) return kInvalidTexture;
    constexpr uint64_t kKey = 0x4658524950504C45ull;   // "FXRIPPLE"

    static uint8_t pixels[kRippleAtlasSizePx * kRippleAtlasSizePx * 4];

    for (int32_t cell = 0; cell < kRippleAtlasFrames; ++cell)
    {
        const float t          = float(cell) / float(kRippleAtlasFrames - 1);   // 0..1 over the 16 cells
        const float ring_r     = 0.18f + t * 0.30f;   // ring radius in cell-uv space (0..0.5 max)
        const float ring_thick = 0.06f + (1.0f - t) * 0.04f;   // thinner ring as it ages
        // Whole-cell alpha: full for the first half (expansion), then
        // fade to zero across the second half (dissipation). Matches the
        // pre-release atlas's 0..3 expand loop / 4..15 dissipate split.
        const float cell_alpha = (t < 0.4f) ? 1.0f
                               : (t > 0.95f) ? 0.0f
                               : (1.0f - (t - 0.4f) / 0.55f);

        const int32_t cell_col = cell % kRippleAtlasCols;
        const int32_t cell_row = cell / kRippleAtlasCols;
        const int32_t base_x   = cell_col * kRippleAtlasCellPx;
        const int32_t base_y   = cell_row * kRippleAtlasCellPx;

        for (int32_t py = 0; py < kRippleAtlasCellPx; ++py)
        {
            for (int32_t px = 0; px < kRippleAtlasCellPx; ++px)
            {
                const float u  = (float(px) + 0.5f) / float(kRippleAtlasCellPx);
                const float v  = (float(py) + 0.5f) / float(kRippleAtlasCellPx);
                const float dx = u - 0.5f;
                const float dy = v - 0.5f;
                const float r  = std::sqrt(dx * dx + dy * dy);
                // Ring intensity: peaks at r == ring_r, falls off linearly
                // over ring_thick on each side. Cubic shaping makes the
                // rim feel more "watery" than a flat band.
                const float d  = std::fabs(r - ring_r);
                float inten    = (d < ring_thick) ? (1.0f - d / ring_thick) : 0.0f;
                inten          = inten * inten * inten;   // cubic for tighter rim
                // Killing pixels outside the cell's natural disc keeps a
                // future texture-bleed-safe edge if the renderer ever
                // bilinear-samples across cells.
                if (r > 0.48f)
                    inten = 0.0f;

                const float a8 = inten * cell_alpha;
                const float a8_clamped = (a8 < 0.0f) ? 0.0f : ((a8 > 1.0f) ? 1.0f : a8);
                const uint8_t byte = uint8_t(a8_clamped * 255.0f);
                const int32_t out_x = base_x + px;
                const int32_t out_y = base_y + py;
                const int32_t idx   = (out_y * kRippleAtlasSizePx + out_x) * 4;
                pixels[idx + 0] = byte;   // premultiplied alpha: rgb == a (white * a)
                pixels[idx + 1] = byte;
                pixels[idx + 2] = byte;
                pixels[idx + 3] = byte;
            }
        }
    }

    return Renderer->RegisterTextureAsset(kKey, pixels, sizeof(pixels),
                                          kRippleAtlasSizePx, kRippleAtlasSizePx,
                                          ERendererTextureFormat::RGBA8,
                                          uint64_t(sizeof(pixels)));
}

}   // namespace

void TRippleEffect::Initialize()
{
    // Pre-release TRippleEffect::Initialize is empty (effect_old.cpp:10689).
    // All the animator state lives on our collapsed effect-class members.
}

void TRippleEffect::Pulse()
{
    TEffect::Pulse();
    // Pre-release Pulse just chains to base (effect_old.cpp:10693). The
    // visible per-frame work runs through TickAndSubmitForTest from the
    // harness; the in-game caller path is H04-blocked (no live ripple
    // spawn site exists in the port today — see INVENTORY H03 §5).
}

TRippleEffect* TRippleEffect::SpawnForTest(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[ripple] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    // No imagery — the ripple atlas is procedural (see gap 7.2).
    auto* ripple = new TRippleEffect(static_cast<TObjectImagery*>(nullptr));
    ripple->ForcePos(origin);
    ripple->SetMapIndex(MapPane.MakeIndex());
    ripple->SetLength(kRippleDefaultLength);
    ripple->ActivateComponents();

    // Pre-register the atlas so the first frame's Submit doesn't pay
    // the bake cost (the byte arr is ~64KB; cheap, but log it once so
    // we know it landed).
    const TTextureHandle tex = RippleAtlasTexture();
    if (tex == kInvalidTexture)
    {
        log_warn("[ripple] SpawnForTest: atlas texture register failed; H03 will draw nothing");
    }

    log_info("[ripple] SpawnForTest: map_index=%d origin=(%d,%d,%d) length=%d tex=%u",
             ripple->GetMapIndex(),
             origin.x, origin.y, origin.z,
             ripple->GetLength(), tex);
    return ripple;
}

void TRippleEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!alive_ || !Renderer)
        return;

    // Sim-tick gate (24 Hz). Pre-release animator was ungated -- at
    // modern render rates the ring grows + cycles too fast, so we drive
    // animator math off TTime::DeltaTime accumulated to retail's tick.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kRippleSimTickMs))
    {
        sim_accum_ms_ -= double(kRippleSimTickMs);
        ++frameon_;

        // Pre-release: ripframe advances every other animator frame.
        if ((frameon_ % 2) == 0)
        {
            if (frameon_ <= len - 24)
            {
                ripframe_ = (ripframe_ + 1) & 3;       // 0..3 expansion loop
            }
            else
            {
                if (frameon_ > len - 24 && ripframe_ < 4)
                {
                    // Jump into the dissipation sequence. Short ripples
                    // (len <= 24) start partway through the dissipation
                    // to compress the wind-down.
                    ripframe_ = (len > 24) ? 4 : (4 + (24 - len) / 2);
                }
                else
                {
                    ++ripframe_;
                    if (ripframe_ > 15)
                        ripframe_ = 15;
                }
            }
        }

        scale_ += kRippleScaleStep;

        if (frameon_ > len && ripframe_ == 15)
        {
            alive_ = false;
            return;
        }
    }

    // Submit one screen-aligned billboard sized by current scale, UV-
    // rectangled to the current atlas cell. Atlas frame indirection
    // mirrors pre-release `SetAnimFrame(rippleframeof[15 - ripframe], obj)`.
    const int32_t logical_frame = 15 - ripframe_;
    const int32_t safe_logical  = (logical_frame < 0) ? 0
                                : (logical_frame > 15) ? 15
                                : logical_frame;
    const int32_t cell          = kRippleFrameOf[safe_logical];
    const int32_t col           = cell % kRippleAtlasCols;
    const int32_t row           = cell / kRippleAtlasCols;
    const float   uv_w          = 1.0f / float(kRippleAtlasCols);
    const float   uv_h          = 1.0f / float(kRippleAtlasRows);

    const TTextureHandle tex = RippleAtlasTexture();
    if (tex == kInvalidTexture)
        return;

    const S3DPoint& p = Pos();
    SBillboardDrawItem item = {};
    item.world_pos[0] = float(p.x);
    item.world_pos[1] = float(p.y);
    // Lift z slightly so the ring doesn't z-fight the ground in scenes
    // that have one. (vfxtest's empty tile pass has no ground; harmless.)
    // Retail anchor convention (effect_old.cpp:10726-10746 TRipple-
    // Animator::AddNewRipple via TDripAnimator): caller passes the
    // drop's landing xy + the parent inst's pos.z, i.e. the water
    // surface position. In the test harness PickPreviewOrigin already
    // supplies z=0 -- the orientation knob does the rest (the quad
    // lies flat on z=p.z+1 in the WorldXY path). If a future caller
    // ports a real `WaterSurfaceZAt(x,y)` query, snap p.z to that.
    item.world_pos[2] = float(p.z) + 1.0f;

    const float diameter = kRippleBaseSizeWu * scale_;
    item.size_wu[0]   = diameter;
    item.size_wu[1]   = diameter;
    item.color_rgba[0] = 0.85f;   // soft cool-white water tint
    item.color_rgba[1] = 0.92f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;    // per-pixel alpha lives in the atlas

    item.uv_rect[0] = float(col) * uv_w;
    item.uv_rect[1] = float(row) * uv_h;
    item.uv_rect[2] = uv_w;
    item.uv_rect[3] = uv_h;

    item.key.texture     = tex;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    // PremulAlpha pairs with the premultiplied procedural atlas
    // (rgb == a, see RippleAtlasTexture loop). Same blend choice as
    // S01 LightningGlowTexture; reads as a translucent ring with a
    // bright crisp rim and no dark-fringe artifacts on the soft edge.
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    // Ripples are flat on the water surface. WorldXY expands the quad
    // along world +X / +Y so the ring foreshortens correctly under the
    // iso camera (reads as a horizontally-stretched ellipse). Without
    // this it would render as a camera-facing disc, visually wrong
    // for a "ripple on the water" effect.
    item.orientation     = EFxBillboardOrientation::WorldXY;
    Renderer->SubmitFxBillboard(item);
}

// *************************************************************************
// * TMistEffect - PE-pipeline continuous-emitter spawn for --test=vfx (M05) *
// *************************************************************************
//
// Scope: Phase 2 M05 row. Ports the ambient ascending-wisp emitter from
// `TMistAnimator` (src/effect_old.cpp:11342-11515) to the PE pipeline,
// reusing the B01 TBloodEffect global-bucket pattern with a continuous
// (non-burst) seeding model.
//
// What this *does* deliver:
//   1. A real `TMistEffect` instance that owns its lifecycle.
//   2. A `TParticleBucket` allocated from `ParticleManager()` and
//      submitted every frame via `Renderer->SubmitFxParticleBucket()` —
//      exercising the PE pipeline end-to-end through the real effect
//      class.
//   3. Real `Magic\mist.i3d` imagery (the canonical wisp sprite,
//      8916 bytes at legacy/Imagery/Magic/mist.i3d). Texture sourced
//      via the F01/B01 lazy-mesh-init poke pattern.
//   4. 50 long-lived drops with retail-faithful pos/vel envelope
//      (MIST_LENGTH=64, MIST_WIDTH=20, MIST_HEIGHT=4 — see effect_old
//      and INVENTORY M05 forensics §1). Drops integrate Euler with
//      RIPPLE_GRAVITY=0.37 wu/tick² and respawn-in-place on landing
//      (pos.z <= 0). Per-particle integration gated to retail's 24 Hz
//      sim tick (pre-release was ungated; without the gate drops would
//      fly 2.5× too fast at 60 fps).
//
// What this does *not* deliver:
//   - The `AddNewRipple` on landing (was commented-out in pre-release —
//     see forensics §7.6).
//   - The per-drop 45°+facing rotation (camera-aligned billboard
//     supersedes — forensics §7.5).
//   - In-game spawn through area-effect-registry dispatch (no live
//     registry caller in the port today; harness-only).
//
namespace {

constexpr const char* kMistImageryPath = "Magic\\mist.i3d";
constexpr const char* kMistBucketName  = "vfx.mist.drops";

// Pre-release constants (effect_old.cpp:11370-11374), preserved for
// reviewability. All values authored at retail's 24 Hz sim tick.
constexpr int32_t kMistMaxDrops = 50;
constexpr float   kMistLength   = 64.0f;     // X envelope half-extent (×2 for vel range)
constexpr float   kMistWidth    = 20.0f;     // Y envelope half-extent
constexpr float   kMistHeight   = 4.0f;      // Z initial-velocity max (vz = random(H/2..H))
constexpr float   kMistScale    = 0.7f;      // pre-release sprite scale
constexpr float   kMistGravity  = 0.37f;     // RIPPLE_GRAVITY shared constant, wu/tick²
// Pre-release spawned drops at z=5 (close to ground) and they ballisticed
// to a peak around vz²/(2g) ≈ 4²/(2·0.37) ≈ 21 wu before falling back.
// At retail's 24 Hz the visible cluster lived in the ~5..25 wu band. At
// the harness's iso camera (origin-centred, looking down) drops in that
// band cluster near the centre and read as a low ground-wisp; the
// camera doesn't show much below z=0. The +20 wu lift offsets the
// spawn floor to land in the visible window; the arc still respects
// the retail vz / gravity envelope.
constexpr float   kMistSpawnZ        = 20.0f;     // wu above origin (was 5 in pre-release)
constexpr float   kMistLandFloor     = 0.0f;      // drop respawn threshold (relative to origin.z)
constexpr int32_t kMistSimTickMs = 1000 / 24;  // 24 Hz integration gate

// Owner-id allocator mirrors B01's pattern (per-instance unique id so
// concurrent emitters don't share particles in the global bucket; the
// usual GetMapIndex stamp isn't unique across the test harness's
// detached instances).
float NextMistOwnerId()
{
    static float next = 5000.0f;
    const float v = next;
    next += 1.0f;
    return v;
}

// Lazily allocate the shared mist bucket against the mist imagery's
// texture slot 0. Returns nullptr if the texture handle isn't ready.
TParticleBucket* AcquireMistBucket(T3DImagery* img3d)
{
    if (TParticleBucket* existing = ParticleManager().FindGlobalBucket(kMistBucketName))
        return existing;

    // Same lazy-mesh-init poke as F01/B01: NumTextures() is a passive
    // read; NumObjects() triggers slot population.
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[mist] AcquireMistBucket: imagery has 0 textures after "
                  "lazy-init poke (objects=%d)", img3d->NumObjects());
        return nullptr;
    }

    S3DTex tex = {};
    img3d->GetTexture(0, &tex);
    if (tex.htexture == kInvalidTexture)
    {
        log_error("[mist] AcquireMistBucket: texture slot 0 handle invalid");
        return nullptr;
    }

    SParticleBucketDesc desc = {};
    desc.name           = kMistBucketName;
    desc.scope          = EParticleBucketScope::Global;
    // Mist is unlit (additive bypasses LitFlat per pre-release
    // SetAddBlendState — see forensics §4). PremulAlpha pairs with
    // chroma-key-converted texture for clean edges (3dimage.cpp's
    // >20%-black-pixel rule auto-fires on the mist sprite, same as
    // Blood.I3D).
    desc.light_mode     = EParticleLightMode::Unlit;
    desc.depth_mode     = EParticleDepthMode::TestNoWrite;
    // Pre-release uses `SetAddBlendState` = D3DBLEND_ONE/ONE additive.
    // The match in our enum is AdditiveStraight (forensics §4 + the
    // particlefx.h note: "ONE/ONE = retail-style self-lit additive").
    desc.blend          = EParticleBlendMode::AdditiveStraight;
    desc.sort           = EParticleSortMode::None;
    desc.texture        = tex.htexture;
    desc.texture_width  = int32_t(tex.desc.width  > 0 ? tex.desc.width  : 1);
    desc.texture_height = int32_t(tex.desc.height > 0 ? tex.desc.height : 1);
    // mist.i3d is a single-frame sprite (no atlas — pre-release renders
    // GetObject(0) which is a single billboard per drop).
    desc.frame_cols     = 1;
    desc.frame_rows     = 1;
    // Default-size in world units. Pre-release scaled the texture by
    // 0.7 against the I3D's mesh-space quad (the quad itself was sized
    // implicitly by the model). For modern billboards we choose a fixed
    // wu size that reads at the harness camera distance. The 50-drop
    // cluster wants to read as a "ground-hugging wisp" — sized similar
    // to the blood splats (~48 wu) so the cluster has visible mass.
    desc.default_width  = 96.0f;
    desc.default_height = 96.0f;

    SParticleBufferLayout layout = {};
    ParticleLayoutAddVar(layout, EParticleVar::OwnerId);
    ParticleLayoutAddVar(layout, EParticleVar::Life);
    ParticleLayoutAddVar(layout, EParticleVar::Age);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
    ParticleLayoutAddVar(layout, EParticleVar::DrawColor);
    ParticleLayoutAddVar(layout, EParticleVar::DrawFrame);
    ParticleLayoutAddVar(layout, EParticleVar::DrawRot);
    // EmitVel stores per-particle velocity (world-units / sim-tick to
    // keep the math in retail units, then scaled by tick-count per
    // frame in TickAndSubmitForTest).
    ParticleLayoutAddVar(layout, EParticleVar::EmitVel);

    TParticleBucket* bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
    log_info("[mist] AcquireMistBucket: bucket='%s' tex=%dx%d handle=%u",
             kMistBucketName, desc.texture_width, desc.texture_height,
             tex.htexture);
    return bucket;
}

// Re-seed one drop's pos+vel from the retail envelope. Used both at
// initial spawn and on landing-respawn. Origin is the emitter centre
// in world units; offsets are pre-release-local-space and added on.
void SeedMistDrop(TParticleBucket& bucket, int32_t pi, const S3DPoint& origin)
{
    auto frand = []() { return float(std::rand()) / float(RAND_MAX); };

    if (float* pos = bucket.VarPtr(pi, EParticleVar::DrawPos))
    {
        // Pre-release spawn envelope was ±32 wu × ±10 wu at the harness
        // camera distance; that's a tight ground patch under one tile.
        // Spread the spawn area 3× horizontally so the 50 drops read as
        // a wisp-blanket area rather than a single tight blob — keeps
        // retail's per-drop kinematics intact, just samples a wider
        // ground patch.
        pos[0] = float(origin.x) + (frand() - 0.5f) * kMistLength * 3.0f;   // ±96 wu
        pos[1] = float(origin.y) + (frand() - 0.5f) * kMistWidth  * 3.0f;   // ±30 wu
        pos[2] = float(origin.z) + kMistSpawnZ;                              // +20 wu (forensics §7.5)
    }
    if (float* vel = bucket.VarPtr(pi, EParticleVar::EmitVel))
    {
        // Pre-release: vx = random(-L,L)/32, vy = random(-W,W)/32,
        // vz = random(H/2, H). All wu/tick at 24 Hz. Retail-faithful;
        // the visible "rise" comes from gravity bringing them back
        // down after ~5 ticks of upward arc.
        vel[0] = (frand() * 2.0f - 1.0f) * (kMistLength / 32.0f);    // ±2.0 wu/tick
        vel[1] = (frand() * 2.0f - 1.0f) * (kMistWidth  / 32.0f);    // ±0.625 wu/tick
        vel[2] = (kMistHeight * 0.5f) + frand() * (kMistHeight * 0.5f);  // +2..+4 wu/tick
    }
    if (float* age = bucket.VarPtr(pi, EParticleVar::Age))
        *age = 0.0f;
    // Drops live until pos.z <= 0; the `life` field is a soft upper
    // bound just so the bucket's reaper doesn't tag them as zombies if
    // gravity somehow doesn't bring them back (e.g. negative gravity
    // in a future variant). 5 seconds at 24 Hz = 120 ticks, well
    // beyond any realistic ballistic arc with our vz=2..4 + g=0.37.
    if (float* life = bucket.VarPtr(pi, EParticleVar::Life))
        *life = 5.0f;
}

}   // namespace

TMistEffect::~TMistEffect()
{
    if (bucket_ && owner_particle_id_ >= 0.0f)
        bucket_->KillParticlesByOwner(owner_particle_id_);
}

// Retail TMistEffect::Initialize / Pulse are empty stubs in pre-release
// (effect_old.cpp:11349-11356) — all the work lives in the animator.
// Phase 2 collapses both into TMistEffect; in-game spawn path is
// deferred (no live caller — INVENTORY M05 forensics §5).
void TMistEffect::Initialize() {}
void TMistEffect::Pulse()      { TEffect::Pulse(); }

// Cached parsed effect def. Lazy on first use.
const SParticleEffectDef& MistEffectDef()
{
    static const SParticleEffectDef def = ParseParticleEffectDef("Mist");
    return def;
}

// Engine-driven SpawnForTest. Loads Magic\mist.i3d (still needed for the
// instance refcount / component lifecycle), attaches a TParticleEffectComponent
// configured with the Mist def, and pulses it once so the engine fills the
// 50-drop steady-state population via spawn_count.
//
// Per-particle dynamics (spawn envelope, gravity, respawn-on-landing) live in
// the effects.def `Mist` block; this method is now ~25 lines of component
// wiring instead of ~85 lines of bespoke spawn-loop + 24 Hz integration.
TMistEffect* TMistEffect::SpawnForTest(const S3DPoint& origin)
{
    const int32_t img_id = TObjectImagery::FindImagery(kMistImageryPath);
    if (img_id < 0)
    {
        log_error("[mist] SpawnForTest: FindImagery('%s') failed", kMistImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[mist] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kMistImageryPath);
        return nullptr;
    }

    auto* mist = new TMistEffect(base);
    mist->ForcePos(origin);
    mist->SetMapIndex(MapPane.MakeIndex());
    mist->ActivateComponents();

    // Engine-driven particle component. spawn_count = 50 + tick_hz = 24 +
    // kill_expr "pos.z <= emit_pos.z" gives the continuous respawn-in-place
    // wisp pattern from retail TMistAnimator.
    auto particle_effect = std::make_unique<TParticleEffectComponent>();
    particle_effect->Configure(&MistEffectDef());
    mist->AddComponent(std::move(particle_effect));

    // Drive one DrawPulse so StartRuntime fires and EmitSpawnTopup fills
    // the 50-drop population NOW (visible from frame 1, matching Static
    // preview style expectations).
    if (auto* pe = mist->GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    mist->bucket_ = ParticleManager().FindGlobalBucket("mist_drops");
    mist->owner_particle_id_ = float(mist->GetMapIndex());

    log_info("[mist] SpawnForTest (engine-driven): map_index=%d origin=(%d,%d,%d) "
             "bucket=%p def=Mist",
             mist->GetMapIndex(),
             origin.x, origin.y, origin.z,
             (void*)mist->bucket_);
    return mist;
}

void TMistEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer) return;

    // Drive the engine's per-frame integration.
    if (auto* pe = GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    if (bucket_)
        Renderer->SubmitFxParticleBucket(*bucket_, debug_mode);
}

#if 0
// REVISITED: replaced by effects.def Mist declaration, kept for reference
// until migration of all 5 bespoke effects (B01/M05/F03/H03/H04) lands.
// The original bespoke spawn/tick body seeded 50 long-lived drops with
// retail-faithful pos/vel envelope, gated integration to 24 Hz sim ticks,
// applied RIPPLE_GRAVITY=0.37 per-tick, and respawned drops in-place on
// landing. All of that is now expressed in the effects.def Mist block's
// spawn_expr / tick_expr / kill_expr with tick_hz=24.
TMistEffect* TMistEffect::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    const int32_t img_id = TObjectImagery::FindImagery(kMistImageryPath);
    if (img_id < 0)
    {
        log_error("[mist] SpawnForTest: FindImagery('%s') failed", kMistImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[mist] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kMistImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[mist] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kMistImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* mist = new TMistEffect(base);
    mist->ForcePos(origin);
    mist->SetMapIndex(MapPane.MakeIndex());
    mist->ActivateComponents();

    mist->bucket_ = AcquireMistBucket(img3d);
    if (!mist->bucket_)
    {
        log_warn("[mist] SpawnForTest: bucket unavailable — M05 will draw nothing");
        return mist;
    }
    mist->owner_particle_id_ = NextMistOwnerId();

    // Seed all 50 drops up-front. Unlike B01's one-shot burst, these
    // live forever — they recycle in-place on landing (TickAndSubmitForTest).
    TParticleBucket& bucket = *mist->bucket_;
    const S3DPoint& p = mist->Pos();
    for (int32_t k = 0; k < kMistMaxDrops; ++k)
    {
        const int32_t pi = bucket.AddParticle(mist->owner_particle_id_, /*life*/5.0f);
        if (pi < 0) break;
        SeedMistDrop(bucket, pi, p);

        // DrawScl in this engine is the *absolute* per-particle size
        // in world units (not a multiplier on default_width/height —
        // see renderer.cpp::SubmitFxParticleBucket which overwrites
        // size_wu from ds[]). Per-drop random jitter (±25%) breaks
        // up the otherwise too-uniform 50-drop cluster. Smaller than
        // 96 wu so individual wisps read instead of merging into a
        // single bright blob.
        if (float* ds = bucket.VarPtr(pi, EParticleVar::DrawScl))
        {
            const float frand_u = float(std::rand()) / float(RAND_MAX);
            const float sz = 56.0f * (0.75f + 0.5f * frand_u);
            ds[0] = sz;
            ds[1] = sz;
            ds[2] = 1.0f;
        }
        if (float* df = bucket.VarPtr(pi, EParticleVar::DrawFrame))
            *df = 0.0f;
        if (float* dr = bucket.VarPtr(pi, EParticleVar::DrawRot))
            *dr = 0.0f;
        // Soft cool-white wisp tint. With AdditiveStraight blend and 50
        // drops overlapping near the centre, full white (1,1,1,1) is way
        // too bright — sums saturate to solid white blob. Per-particle
        // intensity ~0.18 means the densest centre (a few drops on top
        // of each other) reads as bright but never clips, while the
        // sparser edges fade naturally to translucent. Slight cyan
        // tilt (b > r) reads as "mist" not "smoke".
        if (float* col = bucket.VarPtr(pi, EParticleVar::DrawColor))
        {
            col[0] = 0.16f;
            col[1] = 0.18f;
            col[2] = 0.22f;
            col[3] = 1.0f;
        }
    }

    log_info("[mist] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "owner_id=%.0f textures=%d drops=%d",
             kMistImageryPath, mist->GetMapIndex(),
             origin.x, origin.y, origin.z,
             mist->owner_particle_id_, img3d->NumTextures(), kMistMaxDrops);
    return mist;
}

void TMistEffect::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!bucket_ || !Renderer)
        return;

    // 24 Hz sim-tick gate (forensics §7.4). Without this the upward
    // velocity (max +4 wu/tick) runs at render rate → 240 wu/s at 60 fps
    // vs. retail's intended 96 wu/s.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    int32_t ticks = 0;
    while (sim_accum_ms_ >= double(kMistSimTickMs))
    {
        sim_accum_ms_ -= double(kMistSimTickMs);
        ++ticks;
    }

    if (ticks > 0)
    {
        const S3DPoint& p = Pos();
        for (int32_t i = 0; i < bucket_->Count(); ++i)
        {
            const float* owner = bucket_->VarPtr(i, EParticleVar::OwnerId);
            if (!owner || *owner != owner_particle_id_)
                continue;

            float* pos = bucket_->VarPtr(i, EParticleVar::DrawPos);
            float* vel = bucket_->VarPtr(i, EParticleVar::EmitVel);
            if (!pos || !vel)
                continue;

            for (int32_t t = 0; t < ticks; ++t)
            {
                pos[0] += vel[0];
                pos[1] += vel[1];
                pos[2] += vel[2];
                vel[2] -= kMistGravity;
                // Retail respawn-in-place on landing (effect_old.cpp:11448).
                // Drops never die — they immediately re-seed at random
                // pos/vel and keep the emitter in steady-state flux.
                if (pos[2] <= float(p.z))
                {
                    SeedMistDrop(*bucket_, i, p);
                    // Re-read pos/vel after SeedMistDrop (they pointed
                    // into the same slot; values changed, pointers
                    // unchanged in this bucket impl, but stop the
                    // inner-tick-loop to avoid double-integrating a
                    // fresh drop in this same frame).
                    break;
                }
            }
        }
    }

    Renderer->SubmitFxParticleBucket(*bucket_, debug_mode);
}
#endif // bespoke TMistEffect spawn/tick preserved-old-code

// *************************************************************************
// * TDripEffect - PE-pipeline single-drop ceiling emitter for --test=vfx  *
// * (H04) + drip→ripple chain via TRippleEffect::SpawnForTest             *
// *************************************************************************
//
// Scope: Phase 2 H04 row. Ports the cyclic single-drop ceiling emitter
// from `TDripAnimator` (src/effect_old.cpp:10987-11108) to the PE
// pipeline, and demonstrates the harness's first "effect that spawns
// another effect" pattern — when the drop lands, it spawns a real
// TRippleEffect (H03) at the impact point via the same standalone
// SpawnForTest path the harness uses directly.
//
// What this *does* deliver:
//   1. A real `TDripEffect` instance that owns its lifecycle.
//   2. A `TParticleBucket` allocated from `ParticleManager()` and
//      submitted every frame via `Renderer->SubmitFxParticleBucket()`.
//      Single particle per drip-emitter (pre-release tracked one
//      in-flight drop, not an array — INVENTORY H04 §2).
//   3. Real `Magic\drip.i3d` imagery (8916 bytes at
//      legacy/Imagery/Magic/drip.i3d). Texture sourced via the F01 / B01
//      / M05 lazy-mesh-init poke pattern.
//   4. Retail-faithful pos/vel envelope (height=128, gravity=0.37
//      wu/tick², vz0=-1.85 — see INVENTORY H04 §1). 24 Hz sim-tick gate
//      preserves the retail time-of-flight ≈ 21 ticks ≈ 0.875 sec
//      (pre-release was ungated; without the gate the drop would fall
//      ~2.5× too fast at 60 fps render rate).
//   5. Cyclic respawn matching pre-release `dead → wait period →
//      coin-flip respawn → fall → land → spawn ripple → dead` loop.
//      Harness-path period reduced (48 → 24) for screencap-friendly
//      visible drop rate; in-game spawn path keeps retail default.
//   6. **Drip→ripple chain.** On landing, calls
//      `TRippleEffect::SpawnForTest(landing_pos)` and owns the
//      resulting effect via `std::unique_ptr` in `spawned_ripples_`.
//      The harness ticks each spawned ripple via its own
//      `TickAndSubmitForTest`, prunes dead ones via `IsAlive()`. Bypass-
//      es the retail `MapPane.NewObject(SObjectDef{...,
//      FindObjType("ripple"), ...})` registry path because the .rvm
//      effect-registry isn't loaded in the harness (INVENTORY H04 §7.5).
//
// What this does *not* deliver:
//   - `PLAY("drip")` landing SFX (audio routing is out of VFX Phase 2
//     scope — see INVENTORY H04 §7.6).
//   - In-game spawn through area-effect-registry dispatch (no live
//     registry caller in the port today; harness-only — same status as
//     H03 / M05 / L02 / F03).
//   - Bone-mesh / I3D-anim composition (drip.i3d is a single-frame
//     billboard sprite — `GetObject(0)` in pre-release Render).
//
namespace {

constexpr const char* kDripImageryPath = "Magic\\drip.i3d";
constexpr const char* kDripBucketName  = "vfx.drip.drops";

// Pre-release constants (effect_old.cpp:10987-11065), preserved for
// reviewability. All values authored at retail's 24 Hz sim tick.
constexpr int32_t kDripDefaultRippleSize = 64;     // ripple lifetime on landing (TRippleEffect::SetLength)
constexpr int32_t kDripDefaultHeight     = 128;    // initial pos.z above origin (wu)
constexpr int32_t kDripDefaultPeriod     = 48;     // dead-state wait before respawn coin flip (frames)
constexpr float   kDripGravity           = 0.37f;  // RIPPLE_GRAVITY shared constant, wu/tick²
constexpr float   kDripInitialVz         = -kDripGravity * 5.0f;  // pre-release respawn: -1.85 wu/tick
constexpr int32_t kDripSimTickMs         = 1000 / 24;  // 24 Hz integration gate

// Harness-path period override. Retail period=48 / 24 Hz = ~26 sec mean
// inter-drop interval (script-driven sector ambient — long, sparse).
// For a 4-sec screencap that's invisible — set the harness-side period
// to ~24 / 24 Hz = ~10 sec mean, enough to see one or two drops per
// capture cycle. In-game placement keeps the retail default via Load().
constexpr int32_t kDripHarnessPeriod     = 24;

// Park-position offset for dead drops. Bucket particles stay alive
// even when the drip is in the dead-state; parking them well below the
// camera floor keeps them invisible without paying the per-frame
// allocate/destroy cost. The harness camera frames roughly z ∈ [0..400];
// -1000 is safely out-of-view.
constexpr float   kDripDeadParkZ         = -1000.0f;

// Visible droplet world-unit size. Pre-release scaled the drip.i3d
// sprite by 0.3 against the I3D's mesh-space quad. For modern
// camera-aligned billboards we choose a fixed wu size that reads at
// the harness camera distance. The drop is small — too big and it
// looks like a fireball; too small and it disappears in a screencap.
// 48 wu reads as a small bright droplet at the harness's iso camera.
constexpr float   kDripDropletSizeWu     = 48.0f;

float NextDripOwnerId()
{
    static float next = 6000.0f;
    const float v = next;
    next += 1.0f;
    return v;
}

// Lazily allocate the shared drip bucket against the drip imagery's
// texture slot 0. Returns nullptr if the texture handle isn't ready.
// Mirrors AcquireMistBucket (M05) one-to-one — same blend/depth/light
// choices because the underlying I3D pixel data has the same
// chroma-key-friendly bright-on-near-black profile.
TParticleBucket* AcquireDripBucket(T3DImagery* img3d)
{
    if (TParticleBucket* existing = ParticleManager().FindGlobalBucket(kDripBucketName))
        return existing;

    // Same lazy-mesh-init poke as F01/B01/M05.
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[drip] AcquireDripBucket: imagery has 0 textures after "
                  "lazy-init poke (objects=%d)", img3d->NumObjects());
        return nullptr;
    }

    S3DTex tex = {};
    img3d->GetTexture(0, &tex);
    if (tex.htexture == kInvalidTexture)
    {
        log_error("[drip] AcquireDripBucket: texture slot 0 handle invalid");
        return nullptr;
    }

    SParticleBucketDesc desc = {};
    desc.name           = kDripBucketName;
    desc.scope          = EParticleBucketScope::Global;
    // Drip is unlit additive (forensics §4 — inferred from the
    // SaveBlendState/SetBlendState wrapper around the Render path,
    // matching the same idiom in adjacent M05 mist code at
    // effect_old.cpp:10823). PremulAlpha pairs with chroma-key-
    // converted texture for clean edges.
    desc.light_mode     = EParticleLightMode::Unlit;
    desc.depth_mode     = EParticleDepthMode::TestNoWrite;
    desc.blend          = EParticleBlendMode::AdditiveStraight;
    desc.sort           = EParticleSortMode::None;
    desc.texture        = tex.htexture;
    desc.texture_width  = int32_t(tex.desc.width  > 0 ? tex.desc.width  : 1);
    desc.texture_height = int32_t(tex.desc.height > 0 ? tex.desc.height : 1);
    // drip.i3d is a single-frame sprite (no atlas — pre-release renders
    // GetObject(0) which is a single billboard per drop).
    desc.frame_cols     = 1;
    desc.frame_rows     = 1;
    desc.default_width  = kDripDropletSizeWu;
    desc.default_height = kDripDropletSizeWu;

    SParticleBufferLayout layout = {};
    ParticleLayoutAddVar(layout, EParticleVar::OwnerId);
    ParticleLayoutAddVar(layout, EParticleVar::Life);
    ParticleLayoutAddVar(layout, EParticleVar::Age);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
    ParticleLayoutAddVar(layout, EParticleVar::DrawColor);
    ParticleLayoutAddVar(layout, EParticleVar::DrawFrame);
    ParticleLayoutAddVar(layout, EParticleVar::DrawRot);
    // EmitVel stores per-particle velocity (wu/sim-tick to keep math in
    // retail units; scaled by tick count per frame in
    // TickAndSubmitForTest).
    ParticleLayoutAddVar(layout, EParticleVar::EmitVel);

    TParticleBucket* bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
    log_info("[drip] AcquireDripBucket: bucket='%s' tex=%dx%d handle=%u",
             kDripBucketName, desc.texture_width, desc.texture_height,
             tex.htexture);
    return bucket;
}

}   // namespace

TDripEffect::~TDripEffect()
{
    if (bucket_ && owner_particle_id_ >= 0.0f)
        bucket_->KillParticlesByOwner(owner_particle_id_);
    // spawned_ripples_ unique_ptrs destruct here, releasing chained
    // TRippleEffects (which delete their map indices + procedural-atlas
    // refcounts cleanly via their own ~TRippleEffect).
}

// Pre-release TDripEffect::Initialize / Pulse are empty stubs
// (effect_old.cpp:10954-10961) — all the per-frame work lives in
// TDripAnimator. Phase 2 collapses both into TDripEffect.
void TDripEffect::Initialize() {}
void TDripEffect::Pulse()      { TEffect::Pulse(); }

void TDripEffect::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TObjectInstance::Load(is, version, objversion);
    is >> ripplesize >> height >> period;
}

void TDripEffect::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
    os << ripplesize << height << period;
}

TDripEffect* TDripEffect::SpawnForTest(const S3DPoint& origin)
{
    const int32_t img_id = TObjectImagery::FindImagery(kDripImageryPath);
    if (img_id < 0)
    {
        log_error("[drip] SpawnForTest: FindImagery('%s') failed", kDripImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[drip] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kDripImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[drip] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kDripImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* drip = new TDripEffect(base);
    drip->ForcePos(origin);
    drip->SetMapIndex(MapPane.MakeIndex());
    drip->ActivateComponents();

    // Harness cadence override (forensics §6 — see kDripHarnessPeriod).
    drip->SetParams(kDripDefaultRippleSize, kDripDefaultHeight, kDripHarnessPeriod);

    drip->bucket_ = AcquireDripBucket(img3d);
    if (!drip->bucket_)
    {
        log_warn("[drip] SpawnForTest: bucket unavailable — H04 will draw nothing");
        return drip;
    }
    drip->owner_particle_id_ = NextDripOwnerId();

    // Seed exactly one particle, parked offscreen in the dead state.
    // The Tick path moves it into the visible region on the respawn
    // coin flip + integrates downward until landing.
    TParticleBucket& bucket = *drip->bucket_;
    const int32_t pi = bucket.AddParticle(drip->owner_particle_id_, /*life*/0.0f);
    if (pi < 0)
    {
        log_warn("[drip] SpawnForTest: bucket AddParticle returned -1");
        return drip;
    }

    if (float* pos = bucket.VarPtr(pi, EParticleVar::DrawPos))
    {
        pos[0] = float(origin.x);
        pos[1] = float(origin.y);
        pos[2] = kDripDeadParkZ;
    }
    if (float* vel = bucket.VarPtr(pi, EParticleVar::EmitVel))
    {
        vel[0] = 0.0f;
        vel[1] = 0.0f;
        vel[2] = 0.0f;
    }
    if (float* ds = bucket.VarPtr(pi, EParticleVar::DrawScl))
    {
        ds[0] = kDripDropletSizeWu;
        ds[1] = kDripDropletSizeWu;
        ds[2] = 1.0f;
    }
    if (float* df = bucket.VarPtr(pi, EParticleVar::DrawFrame))
        *df = 0.0f;
    if (float* dr = bucket.VarPtr(pi, EParticleVar::DrawRot))
        *dr = 0.0f;
    // Cool-water-blue tint with additive blend. Brighter than M05 mist's
    // 0.18 per-particle intensity since only ONE drop is ever alive at
    // a time (no overlap saturation concern) and the streak needs to
    // read as bright against the dark harness background. Slight blue
    // tilt distinguishes water-drip from a generic spark.
    if (float* col = bucket.VarPtr(pi, EParticleVar::DrawColor))
    {
        col[0] = 0.55f;
        col[1] = 0.75f;
        col[2] = 1.0f;
        col[3] = 1.0f;
    }
    // Life is unused in the integration loop (drips manage their own
    // lifecycle via the dead_/time_ FSM) but set to a non-zero value so
    // the bucket's reaper doesn't tag the particle as a zombie.
    if (float* life = bucket.VarPtr(pi, EParticleVar::Life))
        *life = 1e9f;

    log_info("[drip] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "owner_id=%.0f height=%d period=%d ripplesize=%d",
             kDripImageryPath, drip->GetMapIndex(),
             origin.x, origin.y, origin.z,
             drip->owner_particle_id_,
             kDripDefaultHeight, kDripHarnessPeriod, kDripDefaultRippleSize);
    return drip;
}

void TDripEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // 24 Hz sim-tick gate (forensics §7.4) — accumulate render-rate
    // deltas, integrate per retail-tick. Without this the drop would
    // fall 2.5× too fast and the respawn coin flip would fire 2.5×
    // more often at 60 fps.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    int32_t ticks = 0;
    while (sim_accum_ms_ >= double(kDripSimTickMs))
    {
        sim_accum_ms_ -= double(kDripSimTickMs);
        ++ticks;
    }

    const S3DPoint& origin = Pos();

    // ----- Drop integration (per-tick FSM) ----------------------------
    if (bucket_ && owner_particle_id_ >= 0.0f && ticks > 0)
    {
        // Find our single particle (could iterate by owner like M05,
        // but we know it's one slot; the linear scan is cheap).
        const int32_t count = bucket_->Count();
        int32_t pi = -1;
        for (int32_t i = 0; i < count; ++i)
        {
            const float* owner = bucket_->VarPtr(i, EParticleVar::OwnerId);
            if (owner && *owner == owner_particle_id_)
            {
                pi = i;
                break;
            }
        }

        if (pi >= 0)
        {
            float* pos = bucket_->VarPtr(pi, EParticleVar::DrawPos);
            float* vel = bucket_->VarPtr(pi, EParticleVar::EmitVel);
            if (pos && vel)
            {
                for (int32_t t = 0; t < ticks; ++t)
                {
                    if (dead_)
                    {
                        ++time_;
                        // Pre-release: respawn gate is
                        //   `time > period && !random(0, period/2)`.
                        // `random(0, N)` returns 0..N inclusive (N+1
                        // outcomes), so `!random(0, N)` is a
                        // 1/(N+1) Bernoulli per frame. We model the
                        // same with std::rand().
                        const int32_t period_half = (period > 1) ? (period / 2) : 1;
                        if (time_ > period && (std::rand() % (period_half + 1)) == 0)
                        {
                            time_  = 0;
                            dead_  = false;
                            pos[0] = float(origin.x);
                            pos[1] = float(origin.y);
                            pos[2] = float(origin.z) + float(height);
                            vel[0] = 0.0f;
                            vel[1] = 0.0f;
                            vel[2] = kDripInitialVz;
                        }
                    }
                    else
                    {
                        // Euler integrate; gravity in wu/tick².
                        pos[0] += vel[0];
                        pos[1] += vel[1];
                        pos[2] += vel[2];
                        vel[2] -= kDripGravity;
                        if (pos[2] <= float(origin.z))
                        {
                            // ----- Landing: chain into H03 ripple --
                            const S3DPoint landing = {
                                int32_t(pos[0]),
                                int32_t(pos[1]),
                                int32_t(origin.z),
                            };
                            TRippleEffect* ripple = TRippleEffect::SpawnForTest(landing);
                            if (ripple)
                            {
                                ripple->SetLength(ripplesize);
                                spawned_ripples_.emplace_back(ripple);
                                log_info("[drip] landing at (%d,%d,%d) → "
                                         "spawned ripple (len=%d, owned_count=%zu)",
                                         landing.x, landing.y, landing.z,
                                         ripplesize, spawned_ripples_.size());
                            }
                            else
                            {
                                log_warn("[drip] landing at (%d,%d,%d) — "
                                         "ripple SpawnForTest returned null",
                                         landing.x, landing.y, landing.z);
                            }
                            // Park the bucket particle offscreen until
                            // the next respawn coin flip.
                            pos[2] = kDripDeadParkZ;
                            vel[0] = 0.0f;
                            vel[1] = 0.0f;
                            vel[2] = 0.0f;
                            dead_  = true;
                            time_  = 0;
                            // PLAY("drip") SFX is muted in the harness
                            // (forensics §7.6).
                            break;   // don't integrate fresh-parked particle this same frame
                        }
                    }
                }
            }
        }
    }

    // ----- Submit drop bucket ----------------------------------------
    if (bucket_)
        Renderer->SubmitFxParticleBucket(*bucket_, debug_mode);

    // ----- Tick + submit + prune chained ripples ---------------------
    // First tick each ripple forward (their own TickAndSubmitForTest
    // submits via SubmitFxBillboard, independent of our bucket).
    for (auto& r : spawned_ripples_)
    {
        if (r)
            r->TickAndSubmitForTest(debug_mode);
    }
    // Then prune dead ones. erase-remove keeps memory contiguous; the
    // landing site typically holds 1-3 ripples concurrently (a fresh
    // ripple takes ~96 ticks ≈ 4 sec at 24 Hz to dissipate fully via
    // H03's frameon > len gate, and our 10-sec mean inter-drop period
    // means landings are sparser than the ripple lifetime).
    spawned_ripples_.erase(
        std::remove_if(spawned_ripples_.begin(), spawned_ripples_.end(),
                       [](const std::unique_ptr<TRippleEffect>& r) {
                           return !r || !r->IsAlive();
                       }),
        spawned_ripples_.end());
}

// *************************************************************************
// * THaloEffect - FB+LS standalone-spawn for --test=vfx (L02)             *
// *************************************************************************
//
// Scope: Phase 2 L02 row. Ports the triangle-wave radial-glow effect from
// `THaloAnimator` (src/effect_old.cpp:10564-10680) to the FB pipeline +
// re-adds a per-frame dynamic point light (LS pipeline coupling — the row
// tag is "FB+LS"; pre-release source never explicitly adds the light, but
// the LS coupling falls out of the "halo emits light" semantic — see
// INVENTORY L02 §4 + gap 7.4). Reuses the H03 / F01 standalone-spawn
// pattern.
//
// What this *does* deliver:
//   1. A real `THaloEffect` instance that owns its lifecycle.
//   2. A procedural radial-gradient texture built via
//      `Renderer->RegisterTextureAsset` (no I3D asset on disk — see
//      INVENTORY L02 gap 7.2). Texture is a single 128x128 RGBA8 with a
//      bright ring at outer radius, transparent inner + outer falloff,
//      matching the H03 ripple atlas pattern but as a single non-atlas
//      frame (the halo's per-frame variation is scale, not flipbook
//      cycling).
//   3. Triangle-wave scale envelope (haloscale +=/-=halostep, peak at
//      totframes/2, zero at totframes) sim-tick-gated to retail's 24 Hz
//      cadence (pre-release was ungated; without the gate the pulse runs
//      ~2.5x too fast at 60 fps).
//   4. One screen-aligned additive SBillboardDrawItem each frame via
//      SubmitFxBillboard (FB pipeline) + one per-frame Renderer->
//      AddPointLight at the halo's world position (LS pipeline).
//   5. Self-killed when `frameon > totframes`; harness retrigger respawns
//      at the SpellGround 3.0 sec cadence with small XY jitter.
//
// What this does *not* deliver:
//   - Real `Magic\halo.i3d` mesh (L02b if/when asset is identified).
//   - Flat-on-ground orientation (pre-release rotates the I3D mesh onto
//     the ground plane; the harness camera looks straight-on so a
//     ground-flat ring would project edge-on. Screen-aligned billboard
//     reads as a glowing disc and works for the harness camera).
//   - In-game spawn through the OBJCLASS_EFFECT registry ("Halo" builder
//     dispatch from .rvm-archived scripts isn't wired in the port yet —
//     same blocker as H03 / M05).
//
namespace {

// Per-INVENTORY L02 §1: pre-release `totframes` and `halostep` are
// controller-supplied (no retail defaults visible). Common-sense range:
// totframes ~24 ticks (1 sec at 24 Hz) gives a ~0.5 sec grow + 0.5 sec
// shrink pulse; halostep 0.08 yields peak haloscale ≈ 0.96 (close to
// 1.0 = mesh-natural-size — see §1's "haloscale=1.0 is mesh's authored
// size" note).
constexpr int32_t kHaloDefaultTotFrames = 36;            // ~1.5 sec at 24 Hz
constexpr float   kHaloDefaultHaloStep  = 0.10f;         // peak haloscale ≈ 1.8
constexpr int32_t kHaloSimTickMs        = 1000 / 24;     // 24 Hz cadence gate

// Visible diameter at haloscale = 1.0, in world units. Pre-release's
// `RefreshZBuffer` rect of `haloscale * 20` pixels implies a small
// accent; that screen-pixel measure doesn't carry over to the harness's
// iso world-space camera. The harness camera frames roughly ±500 wu
// across the screen width, so a 300 wu base × peak haloscale 1.8 =
// 540 wu peak diameter -- about a quarter of the visible scene width,
// reading cleanly as a spell-cast halo without dominating the frame.
constexpr float   kHaloBaseDiameterWu   = 300.0f;

// LS coupling: dynamic point light radius scales with haloscale. Peak
// radius covers ~3-4x the visible glow, so the light spills onto
// surrounding geometry instead of being clipped to the billboard
// silhouette. The harness has no ground geometry to illuminate so the
// light is mostly visible by the scene's ambient-vs-sun differential;
// in the in-game path (post L02b/L02c wiring) the light would light
// nearby walls + sectors.
constexpr float   kHaloLightRadiusWu    = 600.0f;

// Procedural radial-gradient texture. Single 128x128 RGBA8 frame
// (no atlas) — a bright ring at radius ~0.36, soft falloff inside +
// outside, fully transparent at the edges. Premultiplied alpha so the
// AdditiveStraight blend reads cleanly (rgb = alpha = ring intensity).
// Matches H03 RippleAtlasTexture's premultiplied pattern; one cell
// instead of 16.
constexpr int32_t kHaloTexPx = 128;

TTextureHandle HaloRingTexture()
{
    if (!Renderer) return kInvalidTexture;
    constexpr uint64_t kKey = 0x4658484C4F000000ull;   // "FXHLO\0\0\0"

    static uint8_t pixels[kHaloTexPx * kHaloTexPx * 4];

    constexpr float kRingR     = 0.36f;   // ring radius in tex-uv space (0..0.5 max)
    constexpr float kRingThick = 0.10f;   // soft falloff on both sides of the ring
    constexpr float kOuterCut  = 0.48f;   // hard alpha-zero past this radius
    constexpr float kCoreFill  = 0.15f;   // soft alpha inside the ring (faint disc fill)

    for (int32_t py = 0; py < kHaloTexPx; ++py)
    {
        for (int32_t px = 0; px < kHaloTexPx; ++px)
        {
            const float u  = (float(px) + 0.5f) / float(kHaloTexPx);
            const float v  = (float(py) + 0.5f) / float(kHaloTexPx);
            const float dx = u - 0.5f;
            const float dy = v - 0.5f;
            const float r  = std::sqrt(dx * dx + dy * dy);

            // Ring intensity: peaks at r==kRingR, linear falloff over
            // kRingThick on each side. Cubic shape gives a crisper rim.
            const float d  = std::fabs(r - kRingR);
            float ring     = (d < kRingThick) ? (1.0f - d / kRingThick) : 0.0f;
            ring           = ring * ring * ring;

            // Faint core fill inside the ring — keeps the halo from
            // looking like a hollow donut and adds a sense of "this
            // is filled with light", which matches the LS coupling.
            float core     = (r < kRingR)
                             ? kCoreFill * (1.0f - r / kRingR)
                             : 0.0f;

            // Combine + hard-cut outside.
            float inten    = ring + core;
            if (r > kOuterCut) inten = 0.0f;
            if (inten < 0.0f)  inten = 0.0f;
            if (inten > 1.0f)  inten = 1.0f;

            const uint8_t byte = uint8_t(inten * 255.0f);
            const int32_t idx  = (py * kHaloTexPx + px) * 4;
            pixels[idx + 0] = byte;   // premultiplied alpha: rgb == a
            pixels[idx + 1] = byte;
            pixels[idx + 2] = byte;
            pixels[idx + 3] = byte;
        }
    }

    return Renderer->RegisterTextureAsset(kKey, pixels, sizeof(pixels),
                                          kHaloTexPx, kHaloTexPx,
                                          ERendererTextureFormat::RGBA8,
                                          uint64_t(sizeof(pixels)));
}

}   // namespace

void THaloEffect::Initialize()
{
    // Pre-release THaloEffect::Initialize is empty (effect_old.cpp:10571).
    // Per-instance kinematic state (haloscale_, frameon_) lives on this
    // class now (animator state collapsed onto effect — same as H03 / M05).
}

void THaloEffect::Pulse()
{
    TEffect::Pulse();
    // Pre-release Pulse just chains to base (effect_old.cpp:10575). The
    // visible per-frame work runs through TickAndSubmitForTest from the
    // harness; the in-game caller path is data-script-blocked (no live
    // "Halo" spawn site exists in the port today — see INVENTORY L02 §5).
}

THaloEffect* THaloEffect::SpawnForTest(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[halo] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    // No imagery — the halo texture is procedural (see L02 gap 7.2).
    auto* halo = new THaloEffect(static_cast<TObjectImagery*>(nullptr));
    halo->ForcePos(origin);
    halo->SetMapIndex(MapPane.MakeIndex());
    halo->InitParams(kHaloDefaultTotFrames, kHaloDefaultHaloStep);
    halo->ActivateComponents();

    // Pre-register the texture so the first frame's submit doesn't pay
    // the bake cost (~64KB; cheap, but log once so we know it landed).
    const TTextureHandle tex = HaloRingTexture();
    if (tex == kInvalidTexture)
    {
        log_warn("[halo] SpawnForTest: ring texture register failed; L02 will draw nothing");
    }

    log_info("[halo] SpawnForTest: map_index=%d origin=(%d,%d,%d) "
             "totframes=%d halostep=%.3f tex=%u",
             halo->GetMapIndex(),
             origin.x, origin.y, origin.z,
             halo->GetTotalFrames(), double(halo->GetHaloStep()), tex);
    return halo;
}

void THaloEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!alive_ || !Renderer)
        return;

    // 24 Hz sim-tick gate. Pre-release animator was ungated -- at modern
    // render rates the triangle-wave pulse runs ~2.5x too fast at 60 fps,
    // so we accumulate DeltaTime and integrate one logical tick per
    // kHaloSimTickMs (matches H03 / M05 / F01 cadence-gate decision).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kHaloSimTickMs))
    {
        sim_accum_ms_ -= double(kHaloSimTickMs);

        // Pre-release THaloAnimator::Animate (effect_old.cpp:10605-10623).
        // Triangle-wave envelope: grow for totframes/2 ticks then shrink
        // linearly back to (near) zero by totframes.
        if (frameon_ < totframes / 2)
            haloscale_ += halostep;
        else
            haloscale_ -= halostep;
        ++frameon_;

        if (frameon_ > totframes)
        {
            alive_ = false;
            return;
        }
    }

    // Pre-release gates render on `haloscale > 0` (effect_old.cpp:10639).
    // Triangle-wave with the default 24-tick / 0.08-step settings peaks
    // at ~0.96 and dips to ~0 at the lifetime edge; clamp negative to
    // avoid a degenerate one-frame mirror.
    if (haloscale_ <= 0.0f)
        return;

    const TTextureHandle tex = HaloRingTexture();
    if (tex == kInvalidTexture)
        return;

    const S3DPoint& p = Pos();

    // FB pipeline: single additive billboard. Lifted slightly so the
    // halo doesn't z-fight the ground when scenes have one (the harness
    // empty tile pass has none; harmless). Color = warm gold to read as
    // a "magic ring of light" — matches the X17 placeholder flare's
    // gold tint so LS-pipeline effects share a visual family.
    //
    // Retail anchor convention (effect_old.cpp:10632-10666
    // THaloAnimator::Render): the matrix path applies
    // `RotateX(-π/2)` to the originally-vertical sprite plane, tipping
    // it onto z=0 -- confirming the halo lies flat on the GROUND plane
    // at the effect's `inst pos`. Caller (spell / script) is responsible
    // for setting pos to ground center of the caster; if a future
    // caller has the caster's chest position it must snap pos.z to
    // `GroundZAt(x,y)` first. In the harness PickPreviewOrigin already
    // supplies z=0.
    SBillboardDrawItem item = {};
    item.world_pos[0] = float(p.x);
    item.world_pos[1] = float(p.y);
    item.world_pos[2] = float(p.z) + 5.0f;   // pre-release's +5 wu lift (§1)

    const float diameter = kHaloBaseDiameterWu * haloscale_;
    item.size_wu[0]   = diameter;
    item.size_wu[1]   = diameter;
    item.color_rgba[0] = 1.0f;     // warm gold (same family as X17 flare)
    item.color_rgba[1] = 0.85f;
    item.color_rgba[2] = 0.40f;
    item.color_rgba[3] = 1.0f;     // per-pixel alpha lives in the texture

    item.uv_rect[0] = 0.0f;
    item.uv_rect[1] = 0.0f;
    item.uv_rect[2] = 1.0f;
    item.uv_rect[3] = 1.0f;

    item.key.texture     = tex;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    // AdditiveStraight pairs with the premultiplied procedural texture
    // (rgb == a, see HaloRingTexture loop). Pre-release uses
    // `SetAddBlendState` = D3D ONE/ONE additive (forensics §1); the
    // matching enum here is AdditiveStraight (same choice as H03 /
    // S01 / M05).
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    // Halo lies flat on the ground. WorldXY makes the quad foreshorten
    // as a ground-projected ellipse under iso/perspective, matching the
    // retail `RotateX(-π/2)` semantics (§1 forensics). Without this
    // the gold ring would render as a camera-facing disc.
    item.orientation     = EFxBillboardOrientation::WorldXY;
    Renderer->SubmitFxBillboard(item);

    // LS pipeline coupling: re-add a dynamic point light each frame
    // while alive. Pre-release source doesn't do this explicitly; the
    // LS coupling falls out of the "halo emits light" semantic and the
    // FB+LS row tag (INVENTORY L02 §4 + gap 7.4). Intensity tracks the
    // triangle-wave envelope so the light pulses with the visible glow.
    // ClearPointLights() runs at the top of VfxTest::Render so the
    // re-add is a per-frame rebuild, matching the LS pipeline contract.
    const float light_wx  = float(p.x);
    const float light_wy  = float(p.y);
    const float light_wz  = float(p.z) + 5.0f;
    const float radius    = kHaloLightRadiusWu * haloscale_;
    const float intensity = 1.5f * haloscale_;
    Renderer->AddPointLight(light_wx, light_wy, light_wz, radius,
                            1.0f, 0.85f, 0.40f, intensity);
}

// *************************************************************************
// * TFireEffect — FB-pipeline scatter-patch ambient fire (F03)            *
// *************************************************************************
//
// Per-INVENTORY F03 forensics (full body at docs/vfx/INVENTORY.md row F03):
//
//   §1 retail src: method bodies live only in
//      `legacy/walkcode/effect.cpp:201-218` (Pulse + builder) and
//      `legacy/walkcode/effect.cpp:886-991` (animator). Modern src/effect.cpp
//      has no `DEFINE_BUILDER("fire", ...)` and no
//      `REGISTER_3DANIMATOR("Fire", ...)` — vestigial declarations only.
//   §2 distinct from F01 TFlameEffect: YES. F01 = single torch flipbook +
//      sparks (1 mesh + particle emitter). F03 = multi-quad scatter patch
//      (NUMFIRES=15 random XY-offsets in a +/-50 unit patch, each with
//      independent atlas-frame counter, additive textured billboards).
//      Different asset (`Misc\Fire.I3D` vs. `Magic\flame.i3d`), different
//      geometry footprint, different lifetime (spell-killed vs. infinite).
//   §4 caller / trigger: no live retail caller; legacy/Class.Def:2015
//      registers "Fire" "Misc\Fire.I3D" 0x119f01e3 as OBJCLASS_EFFECT
//      but no spell.def variant or ATTACHEFFECT references it.
//   §6 render passes / rig category: **FB only** (additive textured quads
//      via SetBlendState; per-quad cycling is sprite atlas frame scrolling
//      driven by per-quad `f[c]`, NOT a TParticleBucket emitter). The
//      INVENTORY's prior `FB+PE` guess was incorrect.
//   §7 gaps: legacy loop bound is 10 not NUMFIRES=15 (pre-release WIP);
//      this port uses the full 15 as the modern-correct default.
//
// Preserved-old-code: pre-release D3D bodies live verbatim at
// `legacy/walkcode/effect.cpp:204-218,888-991` and are not duplicated
// here under `#if 0` — the entire legacy/walkcode/ tree is the project's
// "preserve old code" attic for the WIP effects (see project memory note
// "preserve old code we'll need").

namespace {

// Pre-release legacy/walkcode/effect.cpp:897-903: each scatter quad
// starts at xy = random(-50..50, -50..50) with z = 0, frame counter
// f = random(0..20) - 22 (i.e. -22..-2 startup delay before the quad
// becomes visible). Quad bounds (+/-50 wu) ~= a small ground patch
// (the legacy mesh scaled the textured quad by 16 wu — see §6 — so the
// 15 quads spread across a ~100 wu patch with ~16 wu billboards
// reads as a "field of small flame puffs"). The harness camera frames
// roughly +/-500 wu so the patch occupies ~1/5 of the visible scene.
constexpr int32_t kFirePatchHalfWu        = 50;       // legacy `random(0,100) - 50` extent
constexpr float   kFireQuadSizeWu         = 32.0f;    // legacy scl=16 (half-size), so full=32
constexpr int32_t kFireFrameStartLow      = -22;      // legacy `random(0,20) - 22`
constexpr int32_t kFireFrameStartHighExcl = -2;       // legacy random max+1 (exclusive)
constexpr int32_t kFireFrameRespawnAt     = 30;       // legacy `f[c] >= 30` respawn gate
constexpr int32_t kFireSimTickMs          = 1000 / 24;// 24 Hz cadence gate (family-consistent)
constexpr float   kFireQuadLiftWu         = 4.0f;     // small z lift so quads don't z-fight
                                                      // the ground plane in harness scene

// Real-asset path: pre-release Class.Def:2015 registers
// "Fire" -> "Misc\Fire.I3D" (forensics §5). The I3D ships a single
// material/texture slot with ~30 animated frames in `framehtexs[]` —
// the per-quad `obj->textureframe[0] = f[c]` write in
// `TFireAnimator::Render` picks one frame per quad each submit
// (legacy/walkcode/effect.cpp:980-981; resolver lives in
// src/3dimage.cpp:1478-1482 / SetTextureFrame in 3dimage.cpp:1814-1836).
constexpr const char* kFireImageryPath = "Misc\\Fire.I3D";

// Re-roll one scatter quad's XY offset + startup frame. Pre-release
// legacy/walkcode/effect.cpp:897-903 (Initialize) and
// legacy/walkcode/effect.cpp:940-943 / 963-966 (respawn-in-place) both
// use the same formula. Factored here so Initialize() + the
// `f[c] >= 30` respawn gate in TickAndSubmitForTest share one source.
void RerollFireQuad(SFireScatterQuad& q)
{
    q.ox    = float(random(0, 2 * kFirePatchHalfWu) - kFirePatchHalfWu);
    q.oy    = float(random(0, 2 * kFirePatchHalfWu) - kFirePatchHalfWu);
    q.frame = random(0, kFireFrameStartHighExcl - kFireFrameStartLow)
              + kFireFrameStartLow;
}

}   // namespace

void TFireEffect::Pulse()
{
    // Pre-release Pulse (legacy/walkcode/effect.cpp:204-218) damages
    // characters in range under `spell->GetFire()` and self-kills. The
    // standalone harness has no spell context — chain to base only.
    // The in-game caller path is data-script-blocked (no live "fire"
    // spawn site exists; see INVENTORY F03 §4).
    TObjectInstance::Pulse();
}

TFireEffect* TFireEffect::SpawnForTest(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[fire] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    // Real-asset load. Misc\Fire.I3D isn't pre-registered by any
    // TObjectClass::AddType in the modern data path (per F03 forensics §4
    // — vestigial pre-release class), so do the standard
    // FindImagery -> RegisterImagery fallback (same as M09b TTeleporterEffect).
    int32_t img_id = TObjectImagery::FindImagery(kFireImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kFireImageryPath));
    if (img_id < 0)
    {
        log_error("[fire] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kFireImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[fire] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFireImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fire] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kFireImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Lazy-mesh-init poke (same idiom as F01/B01/M05/H04): forces the
    // texture-array to populate before we query frame handles.
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[fire] SpawnForTest: imagery '%s' has 0 textures after lazy-init poke",
                  kFireImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Pull the texture-slot-0 record. Fire.I3D has one material slot;
    // the frame array lives in textures[0].framehtexs[0..numframes).
    S3DTex tex0 = {};
    img3d->GetTexture(0, &tex0);
    const int32_t numframes = tex0.numframes > 0 ? tex0.numframes : 1;
    if (tex0.htexture == kInvalidTexture && numframes == 1)
    {
        log_error("[fire] SpawnForTest: texture slot 0 handle invalid and numframes=1");
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Snapshot the per-frame texture handles. SetTextureFrame swaps
    // `textures[t].htexture` to `framehtexs[f]` (3dimage.cpp:1827); for
    // an FB-pipeline submit we want each billboard to reference its own
    // frame texture directly, not mutate shared imagery state per
    // submit. So we cache the full frame array here and index it from
    // the submit hot path.
    auto* fire = new TFireEffect(base);
    fire->imagery_ = base;
    fire->frame_textures_.resize(numframes);
    for (int32_t f = 0; f < numframes; ++f)
    {
        // copyframes==false (Fire.I3D is the standard pre-baked path):
        // each frame has its own pre-uploaded texture handle. With
        // copyframes==true (legacy paged-pixel path) framehtexs is
        // nullptr and only htexture is valid — fall back to that.
        if (tex0.framehtexs && tex0.copyframes == false)
            fire->frame_textures_[f] = tex0.framehtexs[f];
        else
            fire->frame_textures_[f] = tex0.htexture;
    }

    fire->ForcePos(origin);
    fire->SetMapIndex(MapPane.MakeIndex());

    // Seed all scatter quads. Pre-release Initialize special-cased
    // index 0 to (0,0,0) / f=0 (legacy/walkcode/effect.cpp:892-895);
    // we keep that — the center quad is always visible immediately
    // and anchors the patch.
    fire->quads_[0].ox    = 0.0f;
    fire->quads_[0].oy    = 0.0f;
    fire->quads_[0].frame = 0;
    for (int32_t i = 1; i < kFireScatterQuads; ++i)
        RerollFireQuad(fire->quads_[i]);

    fire->ActivateComponents();

    log_info("[fire] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "quads=%d patch=%dx%d wu tex0_handle=%u numframes=%d w=%u h=%u",
             kFireImageryPath, fire->GetMapIndex(),
             origin.x, origin.y, origin.z,
             kFireScatterQuads,
             2 * kFirePatchHalfWu, 2 * kFirePatchHalfWu,
             tex0.htexture, numframes, tex0.desc.width, tex0.desc.height);
    return fire;
}

void TFireEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!alive_ || !Renderer || frame_textures_.empty())
        return;

    // 24 Hz sim-tick gate. Pre-release Animate / Render were ungated;
    // at modern 60 fps the per-quad frame counter would cycle ~2.5x too
    // fast (same fix applied in F01 / H03 / M05 / L02).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kFireSimTickMs))
    {
        sim_accum_ms_ -= double(kFireSimTickMs);

        // Pre-release legacy/walkcode/effect.cpp:937-947 (Animate) and
        // 957-969 (Render) both advance `f[c]` and respawn-in-place
        // when `f[c] >= 30`. The Render-side advance is the visible one
        // (Animate only re-rolls; Render does it too, redundantly). We
        // do it once here, in the sim-tick gate.
        //
        // Legacy quirk preserved-with-note: pre-release looped `c < 10`
        // even though NUMFIRES=15 (WIP; INVENTORY F03 §7(a)). This port
        // ticks the full kFireScatterQuads (=15) — the modern-correct
        // behavior. The legacy quirk is documented in the inventory row,
        // not replicated.
        for (int32_t i = 0; i < kFireScatterQuads; ++i)
        {
            if (quads_[i].frame >= kFireFrameRespawnAt)
                RerollFireQuad(quads_[i]);
            else
                ++quads_[i].frame;
        }
    }

    const S3DPoint& p = Pos();
    const int32_t   numframes = int32_t(frame_textures_.size());

    // FB pipeline: one alpha-keyed billboard per visible quad. Pre-release
    // `if (f[c] < 0) continue` (legacy effect.cpp:971-972) — the
    // startup-delay window keeps a randomly-staggered subset of quads
    // invisible each frame, which produces the "occasional flickering
    // flame puff" cadence of an ambient fire. Preserved here.
    //
    // F03b helper-trace correction (vs. 2026-05-16 procedural port):
    //   - blend = Alpha (pre-release `SetBlendState()` is DECAL, NOT
    //     `SetAddBlendState()` — see F03b inventory §2)
    //   - orientation = WorldXY (pre-release rot.x=-π/2 tips the quad
    //     onto the ground plane — F03b §3)
    //   - texture = per-quad framehtexs[f[c]] (real I3D atlas — F03b §1)
    SBillboardDrawItem item = {};
    item.size_wu[0]   = kFireQuadSizeWu;
    item.size_wu[1]   = kFireQuadSizeWu;
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.uv_rect[0] = 0.0f;
    item.uv_rect[1] = 0.0f;
    item.uv_rect[2] = 1.0f;
    item.uv_rect[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::WorldXY;

    for (int32_t i = 0; i < kFireScatterQuads; ++i)
    {
        if (quads_[i].frame < 0)
            continue;   // startup-delay window — quad not yet visible

        // Pre-release: `obj->textureframe[0] = f[c]` -> 3dimage.cpp
        // resolves via `framehtexs[texframe % numframes]` (SetTextureFrame
        // wraps via `framenum % numframes`, 3dimage.cpp:1820-1821).
        // Mirror that wrap defensively in case the I3D ships fewer than
        // 30 frames (forensics doesn't pin the exact frame count).
        int32_t f = quads_[i].frame;
        if (f >= numframes)
            f = f % numframes;
        const TTextureHandle ftex = frame_textures_[f];
        if (ftex == kInvalidTexture)
            continue;

        item.key.texture  = ftex;
        item.world_pos[0] = float(p.x) + quads_[i].ox;
        item.world_pos[1] = float(p.y) + quads_[i].oy;
        item.world_pos[2] = float(p.z) + kFireQuadLiftWu;
        Renderer->SubmitFxBillboard(item);
    }
}

// *************************************************************************
// * TFireEffect_Bespoke — F03 A/B reference (faithful direct C++ port)     *
// *************************************************************************
//
// Line-by-line port of the snapshot TFireAnimator
// (legacy/walkcode/effect.cpp:886-991): NUMFIRES=15 scatter quads, each
// with random XY offset in +/- 50 wu patch and a per-quad startup-frame
// counter `f[c]` in (-22..-2) (so they fade in staggered), advance each
// tick and respawn-in-place when `f[c] >= 30`. Per quad picks
// `framehtexs[f[c] mod numframes]` from Misc\Fire.I3D's frame array.
// Blend = Alpha (DECAL default; snapshot SetBlendState). Orientation =
// WorldXY (snapshot rot.x=-π/2 tips the quad onto the ground plane).
//
// F03c gap fix: preserve the snapshot's per-quad rot.z=-π/4 (45° in-plane
// spin) — the SBillboardDrawItem WorldXY path has no per-instance
// rotation lane, so submit via SubmitFxParticle (SParticleDrawItem has
// `rotation_rad`) with the same WorldXY orientation so each quad is
// ground-flat AND in-plane spun.
//
// Reuses the F03-engine-port anon-namespace helpers (kFirePatchHalfWu,
// kFireFrameRespawnAt, RerollFireQuad) defined immediately above. This
// file-position is intentional — keeping the bespoke after the engine
// port lets us share the seed/respawn formula verbatim.

namespace {

constexpr float       kFireBespokeQuadSpinRad = -0.7853981633974483f;  // -π/4 (45° CW)

}  // namespace

TFireEffect_Bespoke* TFireEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[fire-bespoke] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    int32_t img_id = TObjectImagery::FindImagery(kFireImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kFireImageryPath));
    if (img_id < 0)
    {
        log_error("[fire-bespoke] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kFireImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[fire-bespoke] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFireImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fire-bespoke] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kFireImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[fire-bespoke] SpawnForTest: imagery '%s' has 0 textures after lazy-init",
                  kFireImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    S3DTex tex0 = {};
    img3d->GetTexture(0, &tex0);
    const int32_t numframes = tex0.numframes > 0 ? tex0.numframes : 1;
    if (tex0.htexture == kInvalidTexture && numframes == 1)
    {
        log_error("[fire-bespoke] SpawnForTest: texture slot 0 handle invalid and numframes=1");
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* fire = new TFireEffect_Bespoke(base);
    fire->imagery_ = base;
    fire->frame_textures_.resize(numframes);
    for (int32_t f = 0; f < numframes; ++f)
    {
        if (tex0.framehtexs && tex0.copyframes == false)
            fire->frame_textures_[f] = tex0.framehtexs[f];
        else
            fire->frame_textures_[f] = tex0.htexture;
    }

    fire->ForcePos(origin);
    fire->SetMapIndex(MapPane.MakeIndex());

    // Seed NUMFIRES=15 scatter quads (snapshot Initialize :892-903).
    // Quad 0 special-cased to (0,0,0) / f=0 — center quad always visible
    // and anchors the patch.
    fire->quads_[0].ox    = 0.0f;
    fire->quads_[0].oy    = 0.0f;
    fire->quads_[0].frame = 0;
    for (int32_t i = 1; i < kFireScatterQuads; ++i)
        RerollFireQuad(fire->quads_[i]);

    fire->ActivateComponents();

    log_info("[fire-bespoke] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "quads=%d patch=%dx%d wu tex0=%u numframes=%d w=%u h=%u",
             kFireImageryPath, fire->GetMapIndex(),
             origin.x, origin.y, origin.z,
             kFireScatterQuads,
             2 * kFirePatchHalfWu, 2 * kFirePatchHalfWu,
             tex0.htexture, numframes, tex0.desc.width, tex0.desc.height);
    return fire;
}

void TFireEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!alive_ || !Renderer || frame_textures_.empty())
        return;

    // --- Animate: port of TFireAnimator::Animate (snapshot
    // legacy/walkcode/effect.cpp:937-947). Per-tick `f[c]++`; on
    // `f[c] >= 30` respawn-in-place (XY + new f start in (-22..-2)).
    // 24 Hz sim-tick gate for framerate-independence (matches F03
    // engine-port cadence — same kFireSimTickMs).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kFireSimTickMs))
    {
        sim_accum_ms_ -= double(kFireSimTickMs);
        for (int32_t i = 0; i < kFireScatterQuads; ++i)
        {
            if (quads_[i].frame >= kFireFrameRespawnAt)
                RerollFireQuad(quads_[i]);
            else
                ++quads_[i].frame;
        }
    }

    // --- Render: port of TFireAnimator::Render (snapshot
    // legacy/walkcode/effect.cpp:950-991). Per quad:
    //   if (f[c] < 0) continue;           // startup-delay window
    //   p[c].z = 0 (ground); rot.x = -π/2 (ground-flat); rot.z = -π/4 (45° spin)
    //   textureframe[0] = f[c]            // pick frame from framehtexs[]
    //   blend = Alpha (DECAL, SetBlendState — preserved per rule 3)
    //
    // F03c gap fix: SBillboardDrawItem WorldXY has no per-instance rotation
    // lane; submit via SubmitFxParticle (SParticleDrawItem has rotation_rad)
    // with the same WorldXY orientation. Each quad is ground-flat AND
    // 45° in-plane spun.
    const S3DPoint& p         = Pos();
    const int32_t   numframes = int32_t(frame_textures_.size());

    SParticleDrawItem item = {};
    item.size_wu[0]    = kFireQuadSizeWu;
    item.size_wu[1]    = kFireQuadSizeWu;
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.uv_rect[0]    = 0.0f;
    item.uv_rect[1]    = 0.0f;
    item.uv_rect[2]    = 1.0f;
    item.uv_rect[3]    = 1.0f;
    item.rotation_rad    = kFireBespokeQuadSpinRad;             // snapshot rot.z = -π/4
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::Alpha);            // SetBlendState (snapshot)
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::WorldXY;    // rot.x = -π/2

    for (int32_t i = 0; i < kFireScatterQuads; ++i)
    {
        if (quads_[i].frame < 0)
            continue;   // startup-delay window — quad not yet visible

        int32_t f = quads_[i].frame;
        if (f >= numframes)
            f = f % numframes;
        const TTextureHandle ftex = frame_textures_[f];
        if (ftex == kInvalidTexture)
            continue;

        item.key.texture  = ftex;
        item.world_pos[0] = float(p.x) + quads_[i].ox;
        item.world_pos[1] = float(p.y) + quads_[i].oy;
        item.world_pos[2] = float(p.z) + kFireQuadLiftWu;
        Renderer->SubmitFxParticle(item);
    }
}
// --- end TFireEffect_Bespoke faithful port

// *************************************************************************
// * TTeleporterEffect — I3D cylinder rotating glow column (M09 Misthaven) *
// *************************************************************************
//
// Per-INVENTORY M09 + docs/vfx/M09_FORENSICS.md (pre-release authoritative
// — recon not yet extracted, flagged as M09a follow-up; the pre-release
// body lives at src/effect_old.cpp:5444-5880 intact).
//
// What this delivers (Phase B + M09b real-mesh upgrade):
//   1. A real `TTeleporterEffect` instance owning its lifecycle.
//   2. Pre-release 4-phase state machine (Init→Out→Move→In) running on the
//      24 Hz sim tick (~4.17 s total).
//   3. Visual: the real I3D mesh from Magic\gvortex.I3D (Misthaven recall)
//      or Magic\teleportation.I3D (Teleport variants), pulled at spawn
//      via meshextract::ExtractSubMesh and stamped 5x per frame via
//      SubmitHelperMesh (additive helper-mesh pass) — one per "flare" in
//      the pre-release `for (z=0..4) RenderObject(obj)` loop, with
//      rot.z = rotation_rad + z * 0.5f (radians) giving each cylinder a
//      distinct phase offset so their walls beat against each other as
//      they spin (the source of the shimmer pattern that reads as
//      "teleport vortex"). Per-flare scale + position morph drives the
//      grow-then-shrink triangle wave (effect_old.cpp:5838-5860).
//   4. Payload-coupling hook: GetPhase() returns the live ETeleporterPhase
//      so gameflow can fire its caster->SetPos(destination) on the
//      Out→Move transition (M09_FORENSICS.md §7.6 option (a)).
//
// What this does NOT deliver (deferred — see forensics):
//   - Per-spell-variant tint multiply (v1 uses the I3D's own texture
//     unchanged; the spell.def LIGHT COLOR multiply lands when the
//     real spell dispatch path drives variant selection).
//   - Caster fade timing (M09c — gameflow concern; the harness rig
//     doesn't route SetFade through its background render path).
//   - In-game spawn through the spell.def `"Teleporter"` registration
//     (gameflow / spell-dispatch wiring; out of scope for VFX track).
namespace {

// --- Pre-release effect-side constants (effect_old.cpp:5448-5451, 5538) -
constexpr int32_t kTeleOutDurationTicks   = 50;   // OUT phase length (effect_old.cpp:5538)
constexpr int32_t kTeleAnimSelfKillLife   = 100;  // animator KillThisEffect gate (effect_old.cpp:5818)

// --- Pre-release animator-side constants (effect_old.cpp:5770-5785) -----
constexpr int32_t kTeleFlares             = 5;
constexpr int32_t kTeleIterationsCap      = 50;   // OUT-phase morph counter (effect_old.cpp:5804)
constexpr int32_t kTeleMidWideningAt      = 25;   // when iter >= 25, p[1]/p[2] expand horizontally
constexpr float   kTeleMidWideningStep    = 0.5f; // p[1].x +=, p[2].x -= per sim tick
constexpr float   kTeleVerticalSqueezeStep = 1.0f;// p[3].z -=, p[4].z += per sim tick
constexpr float   kTeleRotationStep       = 0.1f; // rotation += per sim tick (radians; ~137°/s at 24 Hz)
constexpr float   kTeleFlarePhaseStep     = 0.5f; // per-flare rot.z offset (radians) — effect_old.cpp:5843

// --- Render envelope (effect_old.cpp:5838-5860) -------------------------
// Triangle-wave clock: ticks_in_phase < 50 → growing, else mirror-shrink.
// Per-flare radius = (0.5*z + 2.5 * (effective_t / 30)), per-flare
// height_z = (10 - 5*(effective_t/30) - z), spin = rotation + z*0.5.
// `% 100` defensive wrap from pre-release (M09_FORENSICS.md §7.3).
constexpr int32_t kTelePhaseClockPeriod   = 100;  // ticks_in_phase = ticks % 100
constexpr int32_t kTelePhaseHalf          = 50;   // grow vs shrink split point
constexpr float   kTelePhaseNormalizer    = 30.0f;// (effective_t / 30) in pre-release math

// Pre-release p[i] base positions are in I3D-imagery local coordinates;
// the imagery's intrinsic mesh scale defines the world-wu output. To
// map p[i].x/z (units of ~5..65 in pre-release) into the world space
// around the caster (anchor = bbox center), we apply a single scalar
// converter calibrated visually so the 5-cyl stack engulfs Locke's
// body. The CharacterRig recenters the anchor on bbox center, so our
// local p_z is measured from body center (z=0 at anchor) — not feet.
//
// VFXRIG calibration: the rig draws Locke at 3.5x display scale
// (see src/vfxtest.cpp:408 `rig.scale = 280/bbox_w`), but
// CharacterRoot anchor returns inst->Pos() in true world space
// (un-scaled). For the effect to read at the visual size of the
// drawn character, both p[i] offsets AND mesh extents must be scaled
// up to the rig's display scale.
constexpr float   kTeleRigVisualScale     = 3.5f; // matches src/vfxtest.cpp:408 default rig scale
constexpr float   kTelePLocalToWu         = 1.0f * kTeleRigVisualScale;
// Pre-release scl envelope produces scl.xy in 0..6.6 and scl.z in 0..10.
// These multiply the imagery's intrinsic geometry. The gvortex / teleportation
// I3D assets have small per-sub-object intrinsic extents (the cyl is
// authored in tight local-units; with the rig drawn 3.5x display-scaled
// they need a matching scale-up to read at character size). The pre-
// release multiplier was designed when characters were drawn at intrinsic
// 1:1 scale — for our 3.5x rig we apply that factor here.
constexpr float   kTeleMeshScaleMultiplier = kTeleRigVisualScale;

constexpr int32_t kTeleSimTickMs          = 1000 / 24; // 24 Hz cadence (family-consistent)

// --- I3D mesh load + cache for the cylinder ----------------------------
// One MeshHandle list (sub-objects × texslots) per imagery path. Lazy-
// loaded on first SpawnForTest; reused across all TTeleporterEffect
// instances targeting the same variant. Static lifetime — meshes get
// freed at renderer shutdown when ResetAssetRefCounts runs.
//
// We extract the full imagery (all sub-objects × all texture slots)
// rather than only pre-release's `GetObject(1)`. Rationale per
// M09_FORENSICS.md §M09b: the asset is a composite (gvortex.I3D ships
// 12 sub-objects — 5 `blast`, 2 `cylinder`, 4 `#$flare`, 1 `box01`
// pivot — authored to draw together). Pre-release rendered only
// sub-obj 1 stamped 5×, which produces a tall thin shape — visually
// inconsistent with the "rotating vortex of cylinders + flares + blasts"
// shape the asset's geometry telegraphs. We preserve the 5× stamping
// (each stamp = full asset draw with its own rot.z + scale) since
// that's the pre-release intent for "5 cylinders rotating at phase
// offsets" — but each stamp draws the WHOLE asset composite, not one
// sub-object.
struct STeleSubMesh
{
    MeshHandle  handle  = 0;
    int32_t     objnum  = -1;
    int32_t     texslot = -1;
    float       parent_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
};

struct STeleMeshAsset
{
    std::string             path;
    std::vector<STeleSubMesh> subs;
    int32_t                 num_objects = 0;
    int32_t                 total_verts = 0;
    int32_t                 total_faces = 0;
    float                   intrinsic_radius_wu = 1.0f; // half max XY of bbox at scl=1
    float                   intrinsic_height_wu = 1.0f; // Z extent of bbox at scl=1
};

std::vector<STeleMeshAsset>& TeleMeshCache()
{
    static std::vector<STeleMeshAsset> cache;
    return cache;
}

// Find existing cached mesh for `path`, or build it. Returns nullptr if
// the imagery can't be loaded or has no usable sub-object.
//
// Per pre-release `obj = GetObject(1)` (effect_old.cpp:5830) we pull
// sub-object **1** first; if that's empty (assets vary) we fall back to
// the largest sub-object by face count (M09_FORENSICS.md §M09b/7).
const STeleMeshAsset* GetOrLoadTeleMesh(const char* path)
{
    if (!path || !*path || !Renderer) return nullptr;

    auto& cache = TeleMeshCache();
    for (const auto& a : cache)
        if (a.path == path) return &a;

    // The teleporter imageries (gvortex / teleportation / jtele) aren't
    // referenced by any TObjectClass::AddType, so they're absent from the
    // FindImagery registry until something asks for them. RegisterImagery
    // loads the on-disk header and inserts it idempotently (a second call
    // for the same filename returns the existing id).
    char path_buf[256];
    std::strncpy(path_buf, path, sizeof(path_buf) - 1);
    path_buf[sizeof(path_buf) - 1] = '\0';
    int32_t img_id = TObjectImagery::FindImagery(path_buf);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(path_buf);
    if (img_id < 0)
    {
        log_error("[teleporter] RegisterImagery('%s') failed (asset missing?)", path);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[teleporter] LoadImagery(id=%d '%s') failed", img_id, path);
        return nullptr;
    }
    T3DImagery* img = dynamic_cast<T3DImagery*>(base);
    if (!img)
    {
        log_error("[teleporter] imagery '%s' is not a T3DImagery", path);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    const int32_t num_objects = img->NumObjects();
    if (num_objects <= 0)
    {
        log_error("[teleporter] imagery '%s' has 0 sub-objects", path);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Log every sub-object's structure (auditable choice for which got
    // included).
    for (int32_t i = 0; i < num_objects; ++i)
    {
        const int32_t nf = img->NumObjFaces(i);
        const int32_t nv = img->NumObjVerts(i);
        const char* nm = img->GetObjectName(i);
        log_info("[teleporter]   '%s' sub-obj %d name='%s' verts=%d faces=%d",
                 path, i, nm ? nm : "(null)", nv, nf);
    }

    // Log textures so we can audit alpha/chroma-key convention.
    const int32_t num_textures = img->NumTextures();
    log_info("[teleporter]   imagery '%s' num_textures=%d num_materials=%d",
             path, num_textures, img->NumMaterials());
    for (int32_t t = 0; t < num_textures; ++t)
    {
        S3DTex tex = {};
        img->GetTexture(t, &tex);
        log_info("[teleporter]     tex[%d]: handle=%u frames=%d "
                 "w=%u h=%u alphaBitDepth=%u",
                 t, tex.htexture, tex.numframes,
                 tex.desc.width, tex.desc.height,
                 tex.desc.alphaBitDepth);
    }

    // Pull the asset's parent transform for each sub-object (state 0,
    // frame 0) so the per-sub-object meshes draw with their authored
    // relative positions inside our per-flare transform.
    STeleMeshAsset asset;
    asset.path        = path;
    asset.num_objects = num_objects;

    float minx = 1e30f, maxx = -1e30f;
    float miny = 1e30f, maxy = -1e30f;
    float minz = 1e30f, maxz = -1e30f;
    bool bbox_init = false;

    for (int32_t objnum = 0; objnum < num_objects; ++objnum)
    {
        if (img->IsHidden(objnum, 0)) continue;

        float parent_matrix[16];
        BuildStaticObjectMatrix(img, objnum, 0, 0, parent_matrix);

        const int32_t texslots = num_textures + 1;   // +1 for slot 0 (untextured)
        bool obj_got_any = false;
        for (int32_t texslot = 0; texslot < texslots; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(img, objnum, texslot, verts, indices))
                continue;
            if (verts.empty() || indices.empty()) continue;

            // Pick the texture: texslot==0 means untextured (use
            // material if HELPER-style, else white). texslot>=1 maps to
            // texture index texslot-1.
            TTextureHandle albedo = Renderer->WhiteTextureHandle();
            if (texslot > 0 && texslot - 1 < num_textures)
            {
                S3DTex tex = {};
                img->GetTexture(texslot - 1, &tex);
                if (tex.htexture != kInvalidTexture) albedo = tex.htexture;
            }
            else if (texslot == 0)
            {
                // Untextured slot — try material color via S3DObj.material.
                S3DObj o = {};
                img->GetObject(objnum, &o);
                if (o.material >= 0 && o.material < img->NumMaterials())
                {
                    S3DMat mat = {};
                    img->GetMaterial(o.material, &mat);
                    if (mat.texture >= 0 && mat.texture < num_textures)
                        albedo = img->GetTextureHandle(mat.texture);
                }
            }

            const MeshHandle h = Renderer->RegisterMesh(
                verts.data(), int32_t(verts.size()),
                indices.data(), int32_t(indices.size()),
                albedo);
            if (!h)
            {
                log_warn("[teleporter] RegisterMesh failed for '%s' sub-obj "
                         "%d texslot %d (%zu verts, %zu indices)",
                         path, objnum, texslot,
                         verts.size(), indices.size());
                continue;
            }

            STeleSubMesh sub;
            sub.handle  = h;
            sub.objnum  = objnum;
            sub.texslot = texslot;
            std::memcpy(sub.parent_matrix, parent_matrix,
                        sizeof(sub.parent_matrix));
            asset.subs.push_back(sub);
            asset.total_verts += int32_t(verts.size());
            asset.total_faces += int32_t(indices.size() / 3);
            obj_got_any = true;

            // Update bbox using the parent-transformed verts.
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = parent_matrix[0]*x + parent_matrix[1]*y + parent_matrix[2] *z + parent_matrix[3];
                const float wy = parent_matrix[4]*x + parent_matrix[5]*y + parent_matrix[6] *z + parent_matrix[7];
                const float wz = parent_matrix[8]*x + parent_matrix[9]*y + parent_matrix[10]*z + parent_matrix[11];
                if (!bbox_init) {
                    minx = maxx = wx; miny = maxy = wy; minz = maxz = wz;
                    bbox_init = true;
                } else {
                    minx = std::fmin(minx, wx); maxx = std::fmax(maxx, wx);
                    miny = std::fmin(miny, wy); maxy = std::fmax(maxy, wy);
                    minz = std::fmin(minz, wz); maxz = std::fmax(maxz, wz);
                }
            }
        }
        (void)obj_got_any;
    }

    if (asset.subs.empty())
    {
        log_error("[teleporter] '%s' produced no submittable sub-meshes "
                  "(num_objects=%d num_textures=%d)",
                  path, num_objects, num_textures);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    asset.intrinsic_radius_wu = bbox_init
        ? 0.5f * std::fmax(maxx - minx, maxy - miny)
        : 1.0f;
    asset.intrinsic_height_wu = bbox_init ? (maxz - minz) : 1.0f;
    cache.push_back(std::move(asset));

    // Free the loaded imagery — RegisterMesh has already uploaded the
    // verts to GPU buffers and the texture handles are renderer-owned.
    TObjectImagery::FreeImagery(base);

    const STeleMeshAsset& a = cache.back();
    log_info("[teleporter] loaded '%s' num_objects=%d submeshes=%zu "
             "total_verts=%d total_faces=%d "
             "intrinsic_radius=%.1f intrinsic_height=%.1f",
             path, a.num_objects, a.subs.size(),
             a.total_verts, a.total_faces,
             double(a.intrinsic_radius_wu), double(a.intrinsic_height_wu));
    return &a;
}

// Per-effect mesh binding. Held in a side table keyed by effect ptr
// rather than as a TTeleporterEffect member to keep the header
// imagery-API-free. (Same lookup pattern as ParticleEffectComponent's
// bucket binding.)
struct STeleBinding
{
    const STeleMeshAsset* asset = nullptr;
};
std::unordered_map<const TTeleporterEffect*, STeleBinding>& TeleBindings()
{
    static std::unordered_map<const TTeleporterEffect*, STeleBinding> bindings;
    return bindings;
}

// Row-major 4x4 multiply: out = a * b.
void MatMul16Local(const float a[16], const float b[16], float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t k = 0; k < 4; ++k)
                s += a[r * 4 + k] * b[k * 4 + c];
            out[r * 4 + c] = s;
        }
}

// Compose a row-major 4x4 affine matrix:
//   M = T(tx, ty, tz) * Rz(theta) * S(sx, sy, sz)
// stored in the renderer's row-major convention (translation in
// out16[3]/[7]/[11], affine rotation+scale in upper-3×3 row blocks).
void ComposeFlareMatrix(float tx, float ty, float tz,
                        float theta_rad,
                        float sx, float sy, float sz,
                        float out16[16])
{
    const float c = std::cos(theta_rad);
    const float s = std::sin(theta_rad);

    // Row 0: ( c*sx, -s*sy, 0, tx )
    out16[0]  = c * sx;
    out16[1]  = -s * sy;
    out16[2]  = 0.0f;
    out16[3]  = tx;
    // Row 1: ( s*sx,  c*sy, 0, ty )
    out16[4]  = s * sx;
    out16[5]  = c * sy;
    out16[6]  = 0.0f;
    out16[7]  = ty;
    // Row 2: ( 0, 0, sz, tz )
    out16[8]  = 0.0f;
    out16[9]  = 0.0f;
    out16[10] = sz;
    out16[11] = tz;
    // Row 3: ( 0, 0, 0, 1 )
    out16[12] = 0.0f;
    out16[13] = 0.0f;
    out16[14] = 0.0f;
    out16[15] = 1.0f;
}

}   // namespace

TTeleporterEffect::~TTeleporterEffect()
{
    // Release the per-instance entry in the mesh-binding side table.
    // The cached STeleMeshAsset itself is process-static and shared
    // across instances; we don't free it here.
    TeleBindings().erase(this);
}

void TTeleporterEffect::Initialize()
{
    // Pre-release effect_old.cpp:5503-5506: sets my_state = TELE_STATE_INIT.
    // Our default member initializer already sets phase_=Init, so this
    // is effectively a no-op. Kept as a vtable hook for in-game spawn
    // (TEffect ctor chain calls Initialize via the SObjectDef path).
    phase_ = ETeleporterPhase::Init;
    life_ = 0;
}

void TTeleporterEffect::Pulse()
{
    // Pre-release Pulse body (effect_old.cpp:5508-5754) is the 4-phase
    // state machine. We port the *visual* state transitions and the
    // animator's life counter; the payload work (resolve destination via
    // map walk, fire caster->SetPos, fire SetFade) is the in-game spawn
    // path that gameflow wires when it dispatches the spell. See
    // M09_FORENSICS.md §3, §7.5, §7.6.
    TEffect::Pulse();

    if (!alive_) return;

    ++life_;

    switch (phase_)
    {
        case ETeleporterPhase::Init:
            // Pre-release line 5513: 1 tick to resolve variant.
            // spell_level_ is set by the in-game spawn path (or by
            // SpawnForTest default) — here we just transition.
            phase_ = ETeleporterPhase::Out;
            life_ = 0;
            break;

        case ETeleporterPhase::Out:
            // Pre-release line 5538: transition to MOVE at life == 50.
            if (life_ >= kTeleOutDurationTicks)
                phase_ = ETeleporterPhase::Move;
            break;

        case ETeleporterPhase::Move:
            // Pre-release line 5541-5736: single-tick payload. In-game
            // this is where caster->SetPos(destination) fires and the
            // effect itself re-anchors to the destination. The harness
            // port keeps the effect in place (no payload destination
            // resolved). Gameflow integrators poll GetPhase() and fire
            // their payload here. Always transition straight to IN.
            phase_ = ETeleporterPhase::In;
            break;

        case ETeleporterPhase::In:
            // Pre-release line 5737-5752: shrink phase, runs until the
            // animator's life >= 100 self-kill (effect_old.cpp:5818).
            // The animator-life counter lives on this class (collapsed
            // — M09_FORENSICS.md §1) via life_; the IN-phase tick budget
            // is whatever's left out of the 100-tick total minus the 51
            // ticks already spent (INIT + OUT + MOVE).
            if (life_ >= (kTeleAnimSelfKillLife - kTeleOutDurationTicks - 2))
            {
                alive_ = false;
                KillThisEffect();
            }
            break;
    }
}

TTeleporterEffect* TTeleporterEffect::SpawnForTest(const S3DPoint& origin,
                                                   const char* imagery_path)
{
    if (!Renderer)
    {
        log_error("[teleporter] SpawnForTest: renderer not initialized");
        return nullptr;
    }
    if (!imagery_path || !*imagery_path)
    {
        log_error("[teleporter] SpawnForTest: null/empty imagery_path");
        return nullptr;
    }

    // Load (or fetch cached) the I3D cylinder mesh. This must succeed —
    // M09b explicitly rejects any procedural fallback per
    // M09_FORENSICS.md §M09b. If the asset can't be loaded we surface a
    // clear failure and the harness draws nothing for this slot.
    const STeleMeshAsset* asset = GetOrLoadTeleMesh(imagery_path);
    if (!asset)
    {
        log_error("[teleporter] SpawnForTest: mesh load failed for '%s'; "
                  "no procedural fallback per M09b — effect will not draw",
                  imagery_path);
        return nullptr;
    }

    auto* tele = new TTeleporterEffect(static_cast<TObjectImagery*>(nullptr));
    tele->ForcePos(origin);
    tele->SetMapIndex(MapPane.MakeIndex());
    tele->ActivateComponents();

    // Seed per-flare position table from pre-release Initialize
    // (effect_old.cpp:5770-5779). Y is unused in our XZ envelope (we
    // only consume p_x_ and p_z_); pre-release p[i].y = 20 is just the
    // base height the imagery is positioned at relative to the
    // effect's origin; with our anchor convention this is folded into
    // kTeleHeightLiftWu.
    for (int32_t i = 0; i < kTeleFlares; ++i)
        tele->flare_p_z_[i] = 45.0f;   // base height in pre-release units
    tele->flare_p_x_[0] = 0.0f;
    tele->flare_p_x_[1] = -5.0f;
    tele->flare_p_x_[2] = +5.0f;
    tele->flare_p_x_[3] = 0.0f;
    tele->flare_p_z_[3] = 45.0f + 20.0f;   // top flare
    tele->flare_p_x_[4] = 0.0f;
    tele->flare_p_z_[4] = 45.0f - 20.0f;   // bottom flare

    tele->iterations_ = 0;
    tele->ticks_ = 0;
    tele->rotation_rad_ = 0.0f;

    // Bind the loaded mesh to this effect instance.
    TeleBindings()[tele].asset = asset;

    log_info("[teleporter] SpawnForTest: map_index=%d origin=(%d,%d,%d) "
             "flares=%d imagery='%s' submeshes=%zu "
             "intrinsic_radius=%.1f intrinsic_height=%.1f",
             tele->GetMapIndex(),
             origin.x, origin.y, origin.z,
             kTeleFlares, imagery_path,
             asset->subs.size(),
             double(asset->intrinsic_radius_wu),
             double(asset->intrinsic_height_wu));
    return tele;
}

void TTeleporterEffect::TickForTest()
{
    if (!alive_)
        return;

    // 24 Hz sim-tick gate. Pre-release Animate (effect_old.cpp:5787-5820)
    // was ungated. Per M09_FORENSICS.md §3 we gate to 24 Hz so the
    // envelope runs at retail speed regardless of render rate. Same
    // pattern as F01 / H03 / M05 / L02 / F03.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kTeleSimTickMs))
    {
        sim_accum_ms_ -= double(kTeleSimTickMs);

        // Pre-release Animate (effect_old.cpp:5787-5820) — the per-tick
        // morph of the 5 flare positions + rotation increment + tick
        // counter advance.
        rotation_rad_ += kTeleRotationStep;

        if (iterations_ < kTeleIterationsCap)
        {
            // Pre-release line 5806-5810: after the midpoint, p[1]/p[2]
            // expand horizontally (the column thickens in its middle).
            if (iterations_ >= kTeleMidWideningAt)
            {
                flare_p_x_[1] += kTeleMidWideningStep;
                flare_p_x_[2] -= kTeleMidWideningStep;
            }
            // Pre-release line 5811-5812: vertical squeeze (top/bottom
            // converge toward mid).
            flare_p_z_[3] -= kTeleVerticalSqueezeStep;
            flare_p_z_[4] += kTeleVerticalSqueezeStep;

            ++iterations_;
        }
        ++ticks_;

        // Pre-release effect-side Pulse runs at the engine sim cadence
        // already; for the harness we drive it ourselves on the same
        // tick so the phase state machine advances in lockstep with the
        // animator. (In-game path: TEffect::Pulse is called by the
        // engine's pulse loop; the animator's Animate runs at render
        // rate gated to sim-rate by the engine's own gates.)
        Pulse();
    }
}

void TTeleporterEffect::SubmitWorldForTest(EFxDebugMode /*debug_mode*/)
{
    if (!alive_ || !Renderer)
        return;

    // Resolve the per-effect mesh binding. No mesh → nothing to draw.
    auto it = TeleBindings().find(this);
    if (it == TeleBindings().end() || !it->second.asset)
        return;
    const STeleMeshAsset& asset = *it->second.asset;
    if (asset.subs.empty()) return;

    const S3DPoint& p = Pos();

    // Pre-release render envelope (effect_old.cpp:5838-5860). The
    // triangle-wave clock + per-flare radius/height/spin formulas.
    const int32_t curticks = ticks_ % kTelePhaseClockPeriod;
    const int32_t effective_t = (curticks < kTelePhaseHalf)
                                ? curticks
                                : (kTelePhaseHalf - (curticks - kTelePhaseHalf));
    const float   t_norm = float(effective_t) / kTelePhaseNormalizer;

    // For helper-mesh sort_depth we use the effect's own world Z
    // (smaller = drawn earlier). The 5 stamps share the same anchor;
    // we offset their sort depths by per-flare z_index so the outermost
    // (widest, shortest) sits behind the innermost.
    const float base_sort_depth = float(p.z);

    // Render 5 stacked textured cylinders. The pre-release per-flare
    // rotation around Z gives each cylinder a distinct phase offset
    // (0.5 rad ≈ 29° between adjacent stamps); their textured walls
    // visibly cross each other as they spin, producing the shimmer
    // pattern that defines the teleport vortex visual.
    for (int32_t z = 0; z < kTeleFlares; ++z)
    {
        const float zf = float(z);

        // Pre-release line 5844: per-cyl height (z-axis scale).
        const float scl_z_units = 10.0f - 5.0f * t_norm - zf;
        if (scl_z_units <= 0.01f)
            continue;   // pre-release cull (effect_old.cpp:5846)

        // Pre-release line 5845: per-cyl radius (x/y scale).
        const float scl_xy_units = (0.5f * zf) + 2.5f * t_norm;

        // Pre-release line 5843: per-flare rotation, radians.
        const float theta_rad = rotation_rad_ + zf * kTeleFlarePhaseStep;

        // Per-flare local position offset (pre-release p[i] in
        // imagery-local units). Map to world wu via kTelePLocalToWu.
        // The p[i].z=45 base is the imagery's mounting height in
        // pre-release; we treat that as the anchor itself (subtract 45)
        // so the column sits centred on the anchor. The ±20 z-spread
        // of p[3]/p[4] then maps directly to vertical offset above/
        // below the anchor.
        const float local_x_wu = flare_p_x_[z] * kTelePLocalToWu;
        const float local_z_wu = (flare_p_z_[z] - 45.0f) * kTelePLocalToWu;

        // Scale: pre-release scl values multiply imagery intrinsic
        // geometry. kTeleMeshScaleMultiplier is a global tuning knob
        // (=1 unless asset reads small/large).
        const float sx = scl_xy_units * kTeleMeshScaleMultiplier;
        const float sy = sx;
        const float sz = scl_z_units  * kTeleMeshScaleMultiplier;

        // Per-flare world transform: T(anchor + local) * Rz(theta) * S(sx,sy,sz).
        // We compose this once per flare; each sub-mesh of the asset is
        // then drawn at flare_transform * parent_matrix.
        float flare_w[16];
        ComposeFlareMatrix(
            float(p.x) + local_x_wu,
            float(p.y),
            float(p.z) + local_z_wu,
            theta_rad,
            sx, sy, sz,
            flare_w);

        for (const auto& sub : asset.subs)
        {
            float world[16];
            MatMul16Local(flare_w, sub.parent_matrix, world);

            SHelperMeshSubmit m = {};
            m.mesh           = sub.handle;
            m.additive_blend = true;   // M09_FORENSICS.md §M09b: additive helper-mesh pass
            m.shadow_plane   = false;
            std::memcpy(m.world, world, sizeof(world));

            // Helper-mesh shader: final color = base * (ambient + light * ndl)
            // + specular + emissive. For an additive glow we want pure
            // emissive (no scene-light dependency, no specular).
            // diffuse=1 keeps the texture's RGB; emissive boosts the
            // overall brightness; ambient=0 + specular=0 keeps the
            // glow self-luminant.
            //
            // Tint comes from the imagery's own texture (the cylinder
            // wall texture in teleportation.I3D / gvortex.I3D is
            // already a colored glow); we don't apply a
            // spell-LIGHT-COLOR multiply here — that's the in-game
            // spell-dispatch path's job (out of scope for the VFX
            // primitive).
            // Helper-mesh FS: col = base*(amb*la.w + lcol*li.w*ndl)
            //                       + spec*spec*li.w + emissive,
            // where base = tex.rgb * diffuse.rgb.
            // For unlit additive glow we want col = tex.rgb * tint.
            // Setting diffuse=1 + emissive=tint (with ambient=specular=0)
            // gives: col = 0 + 0 + emissive = emissive (lighting-
            // independent constant color). To preserve some texture
            // RGB variation we ALSO route through ambient: ambient.rgb
            // multiplies base by `light_col_a.w` (scene ambient
            // intensity, ~0.2-0.4 in default rig lighting).
            //
            // Per-flare alpha attenuation: outer flares (higher z_index)
            // dimmer so the additive accumulation doesn't saturate to
            // flat white at peak. Pre-release used D3D alpha blending
            // where this was implicit; our additive needs explicit
            // attenuation.
            const float fade = 0.40f - 0.06f * zf;
            m.diffuse[0]  = 1.0f; m.diffuse[1]  = 1.0f; m.diffuse[2]  = 1.0f; m.diffuse[3]  = 1.0f;
            m.ambient[0]  = fade; m.ambient[1]  = fade; m.ambient[2]  = fade; m.ambient[3]  = 1.0f;
            m.specular[0] = 0.0f; m.specular[1] = 0.0f; m.specular[2] = 0.0f; m.specular[3] = 0.0f;
            // Slight cool-blue emissive baseline so the mesh reads as
            // "magic vortex" tint even where the texture rgb is dark.
            m.emissive[0] = 0.10f; m.emissive[1] = 0.15f; m.emissive[2] = 0.25f; m.emissive[3] = 1.0f;
            m.power       = 1.0f;
            // Sort: stamp ordering by per-flare z_index (back-to-front
            // for clean additive layering); sub-meshes within a flare
            // are insertion-ordered, fine since additive is commutative.
            m.sort_depth  = base_sort_depth + zf * 0.001f;

            Renderer->SubmitHelperMesh(m);
        }
    }
}

// *************************************************************************
// * (X22) TSparkEffect — generic spark burst ("sparks"/TSparkAnimator)    *
// *************************************************************************
//
// FAITHFUL DIRECT PORT of the pre-release particle loop. See forensics
// docs/vfx/forensics/SPARKS_TSparkAnimator.md. The three ported bodies:
//
//   spawn  : TCharacter::EffectBurst "sparks" branch (character.cpp:2262-2317)
//            builds an SParticleParams + calls anim->InitParticles(&pr);
//            InitParticles (effect_old.cpp:4752-4787) seeds the arrays.
//   update : TParticle3DAnimator::Animate (effect_old.cpp:4790-4942) —
//            non-seeking ballistic path only (seektargets=false for sparks).
//   render : TParticle3DAnimator::Render (effect_old.cpp:4944-4986) — one
//            billboard per live particle + (trails-1) ghost copies stepped
//            forward along velocity (the motion streak).
//
// Engine adaptations (only what the new engine genuinely requires):
//   - SubmitFxBillboard FB-pipeline draws instead of D3D RenderObject.
//   - Framerate-independence: the original integrated on a 24 Hz integer
//     game-frame (p += v; v.z -= gravity; l-- per tick). We keep the math
//     in the original per-tick units and drive it from a 24 Hz sim-tick
//     accumulator (same fix as F03/H03/M05), so the burst plays at the
//     original real-world speed regardless of render framerate.
//   - Single photon variant (objflags = 1 << (ObjId() & 3)) resolved at
//     spawn into one UV sub-rect of the shared 2x2 atlas texture (the 4
//     photon sub-objects partition one texture into 4 tinted cells); every
//     particle in the burst draws that one cell = one solid color.
//
// NOT ported (dead code for the spark use): the seek/homing block
// (effect_old.cpp:4859-4936), ResetTargetInfo, and the params==0 dev
// sample-default. The bounce block IS ported (retail bounce=true).

namespace {

constexpr const char* kSparkImageryPath = "Misc\\Sparks.I3D";

// 24 Hz cadence gate (family-consistent with F03/H03/M05). The original
// per-tick rates (gravity 0.25 wu/tick^2, life 20-40 ticks, etc.) are
// integrated once per sim-tick of accumulated wall-clock time.
constexpr int32_t kSparkSimTickMs = 1000 / 24;

// Retail spark params (TCharacter::EffectBurst "sparks" branch +
// retail reconciliation, forensics §2.1/§3). Constant immediates live in
// the CALLER in the original, not in the generic animator, so this is the
// spark "constant table".
constexpr int32_t kSparkMinCount  = 15;     // random(15,25)
constexpr int32_t kSparkMaxCount  = 25;
constexpr float   kSparkPosJitter = 3.0f;   // pspread ±3 wu/axis
constexpr float   kSparkVelJitter = 0.5f;   // spread ±0.5 wu/tick/axis
constexpr float   kSparkGravity   = 0.25f;  // retail (snapshot 0.2)
constexpr int32_t kSparkTrails    = 2;      // retail (snapshot 1)
constexpr bool    kSparkBounce    = true;   // retail (snapshot false)
constexpr int32_t kSparkMinStart  = 0;      // start-delay 0..8 ticks
constexpr int32_t kSparkMaxStart  = 8;
constexpr int32_t kSparkMinLife   = 20;     // lifetime 20..40 ticks
constexpr int32_t kSparkMaxLife   = 40;
constexpr int32_t kSparkPosZBias  = 45;     // emit z +45 wu (~impact height)
constexpr int32_t kSparkFaceJitterLo = -80; // facing ± random(-80,80) byte-angle
constexpr int32_t kSparkFaceJitterHi =  80;
constexpr int32_t kSparkDirSpeed  = 100;    // ConvertToVector mag, then /100 (≈1 wu/tick)

// random(-100,100)/100 -> [-1,1] (the original's per-axis jitter scale).
float SparkUnitJitter()
{
    return float(random(-100, 100)) / 100.0f;
}

}   // namespace

TSparkEffect* TSparkEffect::SpawnForTest(const S3DPoint& origin)
{
    // --- Load the REAL Misc\Sparks.I3D imagery (no procedural stand-in).
    const int32_t img_id = TObjectImagery::FindImagery(kSparkImageryPath);
    if (img_id < 0)
    {
        log_error("[spark] SpawnForTest: FindImagery('%s') failed", kSparkImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[spark] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kSparkImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[spark] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kSparkImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* spark = new TSparkEffect(base);
    spark->ForcePos(origin);
    spark->SetMapIndex(MapPane.MakeIndex());
    spark->ActivateComponents();

    spark->gravity_ = kSparkGravity;
    spark->trails_  = kSparkTrails;
    spark->bounce_  = kSparkBounce;

    // --- Single color per burst (objflags = 1 << (ObjId() & 3)).
    // The original selects exactly one of the 4 photon sub-objects for the
    // WHOLE burst, keyed off the attacker's object id. A sector-less
    // harness effect has no valid SObjectInfo (ObjId() would null-deref),
    // so for the standalone rig we rotate a static counter — each fresh
    // burst picks the NEXT photon variant, giving the same per-burst (not
    // per-particle) single-color behavior with cycling variety. All 4
    // photon variants share the same authored sprite, so they map to the
    // same texture content; we index GetTexture by the chosen variant
    // clamped to the available texture slots.
    (void)img3d->NumObjects();   // lazy-mesh-init poke (same as F01/B01/H04)
    const int32_t num_tex = img3d->NumTextures();
    if (num_tex <= 0)
    {
        log_error("[spark] SpawnForTest: imagery has 0 textures after lazy-init poke");
        delete spark;
        return nullptr;
    }
    S3DTex tex = {};
    img3d->GetTexture(0, &tex);
    spark->texture_ = tex.htexture;
    if (spark->texture_ == kInvalidTexture)
    {
        log_error("[spark] SpawnForTest: texture slot 0 handle invalid");
        delete spark;
        return nullptr;
    }

    // The Sparks.I3D texture is a 2x2 ATLAS — the 4 photon sub-objects
    // (photon / photon01 / photon02 / photon03, object indices 0..3) each
    // select ONE atlas cell via their authored UVs. The 4 cells are distinct
    // SHAPES (4-pointed stars and blobs), not just color tints. Selecting one
    // sub-object = one cell = one shape/color for the whole burst (the correct
    // single-cell look — drawing the full atlas mixes all 4 = the grid bug).
    //
    // In the game, objflags = 1 << (ObjId() & 3) keys the cell off the
    // attacker's object id, so different attackers/hits land on different
    // cells and the combat sparks alternate stars/blobs across hits. A
    // sector-less harness effect has no valid SObjectInfo (ObjId() would
    // null-deref), so we rotate a static counter: each successive burst
    // advances the cell 0->1->2->3->wrap, reproducing that in-game shape
    // variety (one clean single-cell burst at a time, consecutive bursts
    // alternate). Never mix cells within a burst.
    const int32_t num_obj = img3d->NumObjects();
    static int32_t s_variant_rotor = 0;
    const int32_t variant = num_obj > 0 ? ((s_variant_rotor++) & 0x3) % num_obj : 0;

    // Compute the chosen sub-object's UV sub-rect from its authored vertex
    // UVs (the faithful equivalent of RenderObject drawing that sub-object's
    // quad with its baked UVs). Drawing the full [0,0,1,1] rect would show
    // all 4 atlas cells on every particle (the mixed-color grid bug).
    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
    const int32_t nverts = img3d->NumObjVerts(variant);
    if (nverts > 0)
    {
        std::vector<S3DVertex> vbuf(static_cast<size_t>(nverts), S3DVertex{});
        img3d->GetObjVerts(variant, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
        float minu = vbuf[0].tu, maxu = vbuf[0].tu;
        float minv = vbuf[0].tv, maxv = vbuf[0].tv;
        for (int32_t i = 1; i < nverts; ++i)
        {
            if (vbuf[i].tu < minu) minu = vbuf[i].tu;
            if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
            if (vbuf[i].tv < minv) minv = vbuf[i].tv;
            if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
        }
        u0 = minu; v0 = minv; u1 = maxu; v1 = maxv;
    }
    spark->uv_rect_[0] = u0;
    spark->uv_rect_[1] = v0;
    spark->uv_rect_[2] = u1 - u0;     // width
    spark->uv_rect_[3] = v1 - v0;     // height

    // Billboard size = the sprite cell, scaled to world units. The photon
    // cell is small; keep the quad small enough that the per-tick travel
    // (~1-2 wu/tick) visibly separates particles rather than overlapping
    // into a static clump (a big quad masks the motion). (The original drew
    // the I3D's authored quad; we approximate with a fixed wu size since the
    // FB pipeline submits screen-aligned wu-sized billboards.)
    spark->quad_size_wu_ = 10.0f;

    // --- Port of EffectBurst "sparks" param build (character.cpp:2273-2313)
    // + InitParticles seeding (effect_old.cpp:4774-4787).
    //
    // The combat caller computes the emit origin/facing from the
    // attacker->target geometry (GetFace, ConvertToFacing, Distance). The
    // harness has no target, so we synthesize: a random horizontal facing
    // for the cone direction, emit at the harness origin with z += 45.
    // Everything downstream (the cone fan, jitter, lifetimes, bounce) is
    // the exact original math.
    const int32_t ang = (random(0, 255) + random(kSparkFaceJitterLo, kSparkFaceJitterHi)) & 0xff;
    S3DPoint vect;
    ConvertToVector(ang, kSparkDirSpeed, vect);   // dir vector, mag ~kSparkDirSpeed, z=0

    // params.pos = vect0 = caster-local impact offset with z += 45. In the
    // harness the burst is the centerpiece, so we emit at the local origin
    // (0,0,+45); the effect object itself sits at `origin`.
    // hmm_vec3 uses uppercase .X/.Y/.Z; S3DPoint `vect` uses lowercase .x/.y/.z.
    const hmm_vec3 ppos   = { 0.0f, 0.0f, float(kSparkPosZBias) };
    const hmm_vec3 pdir   = { float(vect.x) / float(kSparkDirSpeed),
                              float(vect.y) / float(kSparkDirSpeed),
                              float(vect.z) / float(kSparkDirSpeed) };

    // `min` is a macro (revtypes.h) — std::min would mis-expand; use a ternary.
    const int32_t rcount = random(kSparkMinCount, kSparkMaxCount);
    const int32_t count  = rcount < kSparkMaxParticles ? rcount : kSparkMaxParticles;
    spark->num_particles_ = count;
    for (int32_t c = 0; c < count; ++c)
    {
        SSparkParticle& pt = spark->particles_[c];
        pt.pos.X = ppos.X + kSparkPosJitter * SparkUnitJitter();
        pt.pos.Y = ppos.Y + kSparkPosJitter * SparkUnitJitter();
        pt.pos.Z = ppos.Z + kSparkPosJitter * SparkUnitJitter();
        pt.vel.X = pdir.X + kSparkVelJitter * SparkUnitJitter();
        pt.vel.Y = pdir.Y + kSparkVelJitter * SparkUnitJitter();
        pt.vel.Z = pdir.Z + kSparkVelJitter * SparkUnitJitter();
        pt.life  = float(random(kSparkMinLife, kSparkMaxLife));
        pt.start = float(random(kSparkMinStart, kSparkMaxStart));
    }

    spark->alive_ = (count > 0);
    log_info("[spark] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "particles=%d variant=%d/%d tex=%u uv=[%.3f,%.3f %.3fx%.3f] "
             "face=%d gravity=%.2f trails=%d bounce=%d",
             kSparkImageryPath, spark->GetMapIndex(),
             origin.x, origin.y, origin.z, count, variant, num_obj,
             spark->texture_, spark->uv_rect_[0], spark->uv_rect_[1],
             spark->uv_rect_[2], spark->uv_rect_[3],
             ang, spark->gravity_, spark->trails_, spark->bounce_ ? 1 : 0);
    return spark;
}

void TSparkEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TParticle3DAnimator::Animate (non-seeking path,
    // effect_old.cpp:4826-4941), framerate-independent via the 24 Hz
    // sim-tick accumulator. Each accumulated tick runs the original
    // per-tick integration exactly once.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kSparkSimTickMs))
        {
            sim_accum_ms_ -= double(kSparkSimTickMs);

            bool isdone = true;
            for (int32_t c = 0; c < num_particles_; ++c)
            {
                SSparkParticle& pt = particles_[c];
                if (pt.life <= 0.0f)           // already dead
                    continue;
                isdone = false;

                if (pt.start > 0.0f)           // still in start delay
                {
                    pt.start -= 1.0f;
                    continue;
                }

                // p += v;  l--;  v.z -= gravity  (effect_old.cpp:4842-4851)
                pt.pos.X += pt.vel.X;
                pt.pos.Y += pt.vel.Y;
                pt.pos.Z += pt.vel.Z;
                pt.life  -= 1.0f;
                pt.vel.Z -= gravity_;

                // bounce block (effect_old.cpp:4852-4858) — retail bounce=true
                if (bounce_ && pt.pos.Z < 0.0f)
                {
                    pt.pos.Z = -pt.pos.Z;
                    pt.vel.Z = -pt.vel.Z * 0.5f;
                    if (pt.vel.Z < 2.0f)       // don't bounce too much
                        pt.life = 0.0f;
                }
            }

            // killobj=true: object self-destructs once all particles expire
            // (effect_old.cpp:4939-4941). The harness reads alive_ to know
            // when to allow a re-trigger.
            if (isdone)
            {
                alive_ = false;
                break;
            }
        }
    }

    if (texture_ == kInvalidTexture)
        return;

    // --- Render: port of TParticle3DAnimator::Render (effect_old.cpp:4944-4986).
    // One screen-aligned billboard per live, started particle, plus
    // (trails-1) ghost copies stepped forward along velocity (the 2-step
    // motion streak; retail trails=2).
    const S3DPoint& base = Pos();

    SBillboardDrawItem item = {};
    item.size_wu[0]   = quad_size_wu_;
    item.size_wu[1]   = quad_size_wu_;
    item.color_rgba[0] = 1.0f;     // no per-vertex tint in the original; color
    item.color_rgba[1] = 1.0f;     // lives entirely in the photon sprite texture
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    // One 2x2 atlas cell (the chosen photon variant), NOT the full texture.
    item.uv_rect[0] = uv_rect_[0];
    item.uv_rect[1] = uv_rect_[1];
    item.uv_rect[2] = uv_rect_[2];
    item.uv_rect[3] = uv_rect_[3];
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    // Blend = Alpha (SRC_ALPHA / INV_SRC_ALPHA) — the combat-spark Render body
    // calls SetBlendState (= Alpha), and forensics §7 records Alpha. The
    // earlier "additive" comparison turned out to be the GREEN Fountain /
    // Sparkle effect (GREENFONT family), a DIFFERENT effect — not combat
    // sparks. Combat sparks ship Alpha. (Still snapshot-only: the retail
    // TParticle3DAnimator::Render TU was never decompiled; vet against real
    // combat-spark footage, not the fountain video.)
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;

    for (int32_t c = 0; c < num_particles_; ++c)
    {
        const SSparkParticle& pt = particles_[c];
        if (pt.start > 0.0f || pt.life <= 0.0f)   // skip delayed/dead
            continue;

        // Trails: the render walks a local copy forward along velocity,
        // drawing trails_ ghost copies (the motion streak). Identical to
        // the original inner loop (effect_old.cpp:4961-4978).
        hmm_vec3 pp = pt.pos;
        hmm_vec3 vv = pt.vel;
        for (int32_t d = 0; d < trails_; ++d)
        {
            item.world_pos[0] = float(base.x) + pp.X;
            item.world_pos[1] = float(base.y) + pp.Y;
            item.world_pos[2] = float(base.z) + pp.Z;
            Renderer->SubmitFxBillboard(item);

            pp.X += vv.X;
            pp.Y += vv.Y;
            pp.Z += vv.Z;
            vv.Z -= gravity_;
            if (bounce_ && pp.Z < 0.0f)
            {
                pp.Z = -pp.Z;
                vv.Z = -vv.Z * 0.5f;
                if (vv.Z < 2.0f)
                    break;
            }
        }
    }
}

// *************************************************************************
// * (F07) TFireBallEffect — TMissileEffect : TEffect leaf, faithful port  *
// *************************************************************************
//
// FAITHFUL DIRECT PORT of the pre-release `TMissileEffect::Pulse`
// (src/missileeffect.cpp:52-135) + `TFireBallAnimator::Animate/::Render`
// (src/missileeffect.cpp:549-1085). See forensics
// docs/vfx/forensics/F07_TFireBallEffect.md.
//
// Engine adaptations (only what the new engine genuinely requires):
//   - SubmitFxBillboard for box01 (ball/glow/trail/burst) and box02
//     (spark) with per-instance UV sub-rect for atlas-cell selection.
//     The original used D3D matrix-rotated quads (rotX-30°, rotY+60°,
//     rotZ=spin+facing); ScreenAligned billboards are the FB-pipeline
//     engine-native approximation. Forensics §13.6 flags this as the
//     uncertain orientation choice.
//   - SubmitHelperMesh for cylinder01 (the shockwave ring) — same
//     primitive M09b TTeleporterEffect uses, additive-blend pass.
//   - Framerate-independent 24 Hz sim-tick accumulator (per-tick rates
//     integrated once per accumulated wall-clock tick of 1/24 s),
//     family-consistent with B01/X22/F03/H03/M05.
//   - Per-frame Renderer->AddPointLight at the ball position with the
//     retail spell-LIGHT colour (255,130,0) for the warm scene glow.

namespace {

constexpr const char* kFireBallImageryPath = "Magic\\NewFireBall.I3D";

// Sub-object indices inside NewFireBall.I3D (forensics §4).
constexpr int32_t kFireBallObjBox01      = 0;   // ball / glow / trail / burst
constexpr int32_t kFireBallObjBox02      = 1;   // spark quad
constexpr int32_t kFireBallObjCylinder01 = 2;   // shockwave ring

// Pre-release `ROLLOVER = 1 << 16` (revdefs.h:447) — sub-unit precision
// used by TMissileEffect's `speed = newspeed * ROLLOVER` integer math.
// We carry the same constant so the port's Pulse arithmetic matches the
// snapshot literal-for-literal (forensics §3).
constexpr int32_t kFireBallRollover = 1 << 16;

// Resolve which texture slot a sub-object draws from by walking its
// texfaces[] table. Duplicates the blood-side helper (effect.cpp B01
// block) — left local to keep the file's section boundaries clean
// rather than pulling the blood namespace open here.
int32_t FireBallSubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0)
        return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

}   // namespace

void TFireBallEffect::Initialize()
{
    // Port of TMissileEffect::Initialize (missileeffect.cpp:31-40) +
    // TFireBallEffect::Initialize (:482-487) + TFireBallAnimator::
    // Initialize (:501-546). The animator-side init is folded onto the
    // class since the per-frame state lives here.
    state_      = 0;   // MISSILE_LAUNCH
    aim_angle_  = 0;
    range_      = 32768;
    status_     = false;
    vel_        = {0.0f, 0.0f, 0.0f};
    old_state_  = 0;
    firsttime_  = 0;
    explode_    = 0;

    // FireBallAnimator init (missileeffect.cpp:516-545):
    fireball_.pos    = {0.0f, 0.0f, 0.0f};
    fireball_.scale  = kFireBallMaxSize / 2.0f;   // 0.30
    fireball_.frame  = 0.0f;
    fireball_.glow   = 1.5f;
    fireball_.used   = true;
    frame_count_     = kFireBallMaxFrame;
    glow_frame_      = kFireBallGlowFrame;
    for (auto& t : trail_)   t = SFireBallData{};
    for (auto& b : burst_)   b = SFireBallData{};
    for (auto& s : sparks_)  s = SFireBallSpark{};
    ring_active_    = false;
    ring_done_      = true;
    ring_scale_     = kFireBallRingScale;
    alive_          = true;
    sim_accum_ms_   = 0.0;
}

void TFireBallEffect::Pulse()
{
    // Pre-release TFireBallEffect::Pulse (missileeffect.cpp:489-492) is a
    // pure pass-through to TMissileEffect::Pulse. The base body lives in
    // StepMissilePulse (a private helper so the port can drive sim-tick
    // gated cadence from TickAndSubmit without a virtual-call detour).
    TEffect::Pulse();
}

void TFireBallEffect::FillAtlasUv(int32_t frame_idx, float out_uv[4]) const
{
    // Port of TFireBallAnimator::SetAnimFrame (missileeffect.cpp:761-779):
    // 4×4 grid, cell size 0.25 × 0.25, u=(f%4)*.25, v=(f/4)*.25.
    const int32_t f = ((frame_idx % frame_count_) + frame_count_) % frame_count_;
    out_uv[0] = float(f % 4) * asset_.atlas_cell_w;
    out_uv[1] = float(f / 4) * asset_.atlas_cell_h;
    out_uv[2] = asset_.atlas_cell_w;
    out_uv[3] = asset_.atlas_cell_h;
}

bool TFireBallEffect::IsTrailDraining() const
{
    // Port of TFireBallAnimator::IsTrail (missileeffect.cpp:781-791):
    // true while the trail head ≠ tail position (the ball still moved
    // recently). Used as the "trail still draining" gate for death.
    const SFireBallData& head = trail_[0];
    const SFireBallData& tail = trail_[kFireBallTrailSize - 1];
    return head.pos.X != tail.pos.X
        || head.pos.Y != tail.pos.Y
        || head.pos.Z != tail.pos.Z;
}

bool TFireBallEffect::AnyBurstAlive() const
{
    for (const auto& b : burst_)
        if (b.used) return true;
    return false;
}

int32_t TFireBallEffect::LiveSparkCount() const
{
    int32_t c = 0;
    for (const auto& s : sparks_)
        if (s.used) ++c;
    return c;
}

void TFireBallEffect::StepMissilePulse()
{
    // Port of TMissileEffect::Pulse (missileeffect.cpp:52-135), translated
    // line-by-line. The original's `Move()` integration becomes an
    // explicit pos += vel here (we drive the effect's TObjectInstance::Pos
    // via ForcePos, since the engine's mover isn't wired for the harness
    // path). The MOVE_BLOCKED branch reduces to a range floor in the
    // harness (no character / tile collision in --test=vfx).

    // Integrate velocity into the effect's world position (the
    // pre-release engine's `Move()` does this). Vel is in per-tick units
    // (set in the LAUNCH→FLY transition below).
    if (state_ == 1 /*MISSILE_FLY*/)
    {
        S3DPoint p = Pos();
        p.x += int32_t(vel_.X);
        p.y += int32_t(vel_.Y);
        p.z += int32_t(vel_.Z);
        ForcePos(p);
    }

    switch (state_)
    {
        case 0: /* MISSILE_LAUNCH */
            // missileeffect.cpp:58-72.
            if (status_)
            {
                // Harness-only: hold LAUNCH for ~16 ticks so the
                // "grow above caster" moment is visibly observable
                // before the ball launches. In-game launch_hold_=0.
                if (launch_hold_ticks_remaining_ > 0)
                {
                    --launch_hold_ticks_remaining_;
                    break;
                }
                // Aim → velocity (ConvertToVector emits an integer
                // S3DPoint scaled by `speed`; we divide by ROLLOVER to
                // get the per-tick wu velocity the harness integrates).
                S3DPoint v_int = {0, 0, 0};
                const float harness_speed = kFireBallSpeed * kFireBallHarnessSpeedScale;
                ConvertToVector(aim_angle_, int32_t(harness_speed * float(kFireBallRollover)), v_int);
                vel_.X = float(v_int.x) / float(kFireBallRollover);
                vel_.Y = float(v_int.y) / float(kFireBallRollover);
                // missileeffect.cpp:66 — vel.z = speed / -16 (slight
                // downward arc).
                vel_.Z = harness_speed / -16.0f;
                // missileeffect.cpp:68 — range = (240 * MISSILE_RANGE) /
                // (speed / ROLLOVER) = (240*2)/8 = 60 ticks of flight
                // (scaled to keep the visible flight ≈2.5 s regardless
                // of harness speed scale).
                range_ = kFireBallFlyRangeTicks;
                state_ = 1;   // MISSILE_FLY
            }
            break;

        case 1: /* MISSILE_FLY */
        {
            // missileeffect.cpp:79-114.
            bool explode = false;
            range_--;
            if (range_ <= 0)
                explode = true;
            // MOVE_BLOCKED / character-hit branches: in the harness there
            // is no map collision and no live characters to query
            // (TMapIterator would walk empty sets / fault), so we fall
            // through. In-game the dispatcher fires these checks per
            // forensics §6.1. Translated body is preserved at the
            // bottom of the file under `#if 0` for the in-game port.
            if (explode)
                state_ = 2;   // MISSILE_EXPLODE
            break;
        }

        case 2: /* MISSILE_EXPLODE */
            // missileeffect.cpp:127-130. The animator drives the visual
            // termination; HasAnimator() goes false once all sub-systems
            // are quiet. In this port the death gate lives in
            // StepAnimate (alive_ = false when IsTrailDraining + sparks +
            // burst + ring are all quiet — forensics §6.2).
            break;
    }
}

void TFireBallEffect::StepAnimate()
{
    // Port of TFireBallAnimator::Animate (missileeffect.cpp:549-759),
    // translated line-by-line. SetCommandDone / SetStatus(false) handshakes
    // collapse to direct boolean assignments on the leaf since there is
    // no separate animator object.

    status_ = false;
    firsttime_ = (old_state_ != state_) ? 1 : 0;

    // explode-edge tracker (:564-567).
    if (explode_ == 1)
        explode_ = -1;
    else if (explode_ == 0 && state_ == 2 /*MISSILE_EXPLODE*/)
        explode_ = 1;

    const S3DPoint effect_pos = Pos();

    // --- spark trail params (:573-621) ---
    int32_t spark_target = 0;
    if (explode_ == -1)
        spark_target = 0;
    else if (explode_ == 1)
        spark_target = kFireBallMaxSpark;
    else
        spark_target = kFireBallNormSpark;

    int32_t spark_chance = 0;
    if (state_ == 0 /*LAUNCH*/)
        spark_chance = 25;
    else if (state_ == 1 /*FLY*/)
        spark_chance = 30;
    else
        spark_chance = 100;

    // velocity spread per state (:604-610).
    const float spread_xy = (state_ == 0) ? 3.0f : 1.0f;
    const float spread_z  = 1.0f;

    // Spawn-to-fill sparks (mirrors TSubParticleAnimator::Animate spawn
    // path at effectcomp.cpp:455-524): each empty slot, until we hit the
    // target count, has a `chance%` probability to spawn this tick.
    int32_t live = LiveSparkCount();
    for (int32_t i = 0; i < kFireBallMaxSpark && live < spark_target; ++i)
    {
        SFireBallSpark& sp = sparks_[i];
        if (sp.used) continue;
        if (random(1, 100) > spark_chance) continue;
        sp.used = true;
        sp.pos.X = float(effect_pos.x);
        sp.pos.Y = float(effect_pos.y);
        sp.pos.Z = float(effect_pos.z);
        sp.vel.X = float(random(-100, 100)) / 100.0f * spread_xy;
        sp.vel.Y = float(random(-100, 100)) / 100.0f * spread_xy;
        sp.vel.Z = float(random(-100, 100)) / 100.0f * spread_z;
        sp.scale = kFireBallSparkScale;
        sp.life  = random(kFireBallSparkMinLife, kFireBallSparkMaxLife);
        ++live;
    }

    // Integrate sparks (TSubParticleAnimator update body — effectcomp.cpp:
    // 1372-1413-ish flavor; the doc cites it via §6.3): pos += vel;
    // vel.z -= gravity; scale *= scale_dec; life--; cull if life<0.
    for (auto& sp : sparks_)
    {
        if (!sp.used) continue;
        sp.pos.X += sp.vel.X;
        sp.pos.Y += sp.vel.Y;
        sp.pos.Z += sp.vel.Z;
        sp.vel.Z -= kFireBallSparkGravity;
        sp.scale *= kFireBallSparkScaleDec;
        sp.life  -= 1;
        if (sp.life <= 0 || sp.scale <= 0.01f)
            sp.used = false;
    }

    // --- mesh-trail ring buffer (:627-636) ---
    for (int32_t i = kFireBallTrailSize - 1; i > 0; --i)
    {
        trail_[i] = trail_[i - 1];
        trail_[i].scale *= kFireBallTrailScale;
    }
    trail_[0] = fireball_;
    trail_[0].scale *= kFireBallTrailScale;
    // Pre-release records the trail head in WORLD coords (:634-636 adds
    // effect_pos to the local 0,0,0 head); we follow suit.
    trail_[0].pos.X = fireball_.pos.X + float(effect_pos.x);
    trail_[0].pos.Y = fireball_.pos.Y + float(effect_pos.y);
    trail_[0].pos.Z = fireball_.pos.Z + float(effect_pos.z);

    // --- ball self-animation (:638-655) ---
    fireball_.glow = 1.0f + 0.05f * float(random(0, 15));
    fireball_.frame += 1.0f;
    if (fireball_.frame > float(frame_count_))
        fireball_.frame = 0.0f;
    if (int32_t(fireball_.frame) == glow_frame_)
    {
        fireball_.frame += 1.0f;
        if (fireball_.frame > float(frame_count_))
            fireball_.frame = 0.0f;
    }
    // In-plane spin accumulator (missileeffect.cpp:651-655). The active
    // snapshot branch is `+2/tick` (FIREBALL_WHITE_FADE is commented out
    // at missileeffect.h:163, so the `#ifndef` branch wins). This value
    // propagates into trail copies via `trail[0] = fireball` below, so
    // each trail slot freezes at the rotation the head had when the
    // copy was recorded — the snapshot's stale-spin tumble.
    fireball_.rotation = std::fmod(fireball_.rotation + 2.0f, 360.0f);

    // --- state-specific actions (:657-757) ---
    int32_t live_burst_count = 0;
    switch (state_)
    {
        case 0: /* MISSILE_LAUNCH */
            status_ = true;   // tell base Pulse to launch (handshake)
            break;

        case 1: /* MISSILE_FLY */
            if (fireball_.scale < kFireBallMaxSize)
                fireball_.scale += kFireBallGrowRate;
            break;

        case 2: /* MISSILE_EXPLODE */
            if (fireball_.scale < kFireBallMaxSize)
                fireball_.scale += kFireBallGrowRate;

            if (firsttime_)
            {
                // Impact world position = fireball.pos + effect_pos
                // (:676-679). In our port fireball_.pos.X/Y/Z are 0 (the
                // ball head sits at the effect origin), so the impact
                // simplifies to effect_pos.
                S3DPoint impact_pos = effect_pos;
                impact_pos.x += int32_t(fireball_.pos.X);
                impact_pos.y += int32_t(fireball_.pos.Y);
                impact_pos.z += int32_t(fireball_.pos.Z);

                // Blast damage (:681-684). The retail call:
                //   BlastCharactersInRange(spell->GetInvoker(), impact_pos,
                //                          150, vd->mindamage, vd->maxdamage,
                //                          spell->SpellData()->damagetype);
                // is gated by HasSpell() — and BlastCharactersInRange itself
                // currently lives under `#if 0` in effect_old.cpp:278 (the
                // body comes back online in the combat-port phase). The
                // gameflow caller wires this; harness has no live spell.
                // See §13.8 (damage source = spell variant data, not the
                // dead FIREBALL_DAMAGE_MIN/MAX macro).
                (void)impact_pos;

                // Burst quads (:687-699). Pre-release sets
                // `burst[i].rotation = 0` (missileeffect.cpp:694); the
                // draw at :846 then applies `-burst[i].rotation *
                // TORADIANf` = 0, so all burst cards share the no-spin
                // orientation. Seed the same so the WorldXY matrix tilt
                // + facing-rotation composes consistently across slots.
                for (int32_t i = 0; i < kFireBallMaxBurst; ++i)
                {
                    SFireBallData& bq = burst_[i];
                    bq.used    = true;
                    bq.pos.X   = fireball_.pos.X + float(random(-15, 20));
                    bq.pos.Y   = fireball_.pos.Y + float(random(-15, 20));
                    bq.pos.Z   = fireball_.pos.Z + float(random(-15, 20));
                    bq.scale   = 0.75f + float(random(0, 5)) * 0.15f;
                    bq.glow    = 1.0f + 0.05f * float(random(0, 15));
                    bq.frame   = float(random(0, frame_count_ - 1));
                    bq.rotation = 0.0f;   // snapshot :694
                    if (int32_t(bq.frame) == glow_frame_)
                        bq.frame += 1.0f;
                    ++live_burst_count;
                }

                // Shockwave ring init (:701-731). Color stops are stored
                // implicitly — the ring helper-mesh draws use the
                // texture-modulated cylinder geometry, and the four
                // RGBA color stops fade together via per-frame alpha.
                ring_active_ = true;
                ring_done_   = false;
                ring_scale_  = kFireBallRingScale;
                ring_pos_.X  = fireball_.pos.X + float(effect_pos.x);
                ring_pos_.Y  = fireball_.pos.Y + float(effect_pos.y);
                ring_pos_.Z  = fireball_.pos.Z + float(effect_pos.z);
            }
            else
            {
                // Continuing EXPLODE ticks (:734-749). Ring grows
                // 1.085 / tick until > 120 → done. Burst quads shrink
                // 0.90 / tick, culled at < 0.50.
                if (ring_active_ && !ring_done_)
                {
                    ring_scale_ *= kFireBallRingFactor;
                    if (ring_scale_ > kFireBallRingMaxSize)
                    {
                        ring_scale_ = kFireBallRingMaxSize;
                        ring_done_  = true;
                    }
                }
                for (auto& bq : burst_)
                {
                    if (!bq.used) continue;
                    ++live_burst_count;
                    bq.scale *= 0.90f;
                    if (bq.scale < 0.50f)
                    {
                        bq.used = false;
                        continue;
                    }
                    bq.glow = 1.0f + 0.05f * float(random(0, 15));
                    bq.frame += 1.0f;
                    if (int32_t(bq.frame) == glow_frame_)
                        bq.frame += 1.0f;
                    bq.frame = float(int32_t(bq.frame) % frame_count_);
                }
            }

            // Death gate (:752-755). The original checks IsTrail() &&
            // spark.GetCount() == 0 && no burst alive && ring.IsDone().
            // All four must be true simultaneously.
            if (!IsTrailDraining()
                && LiveSparkCount() == 0
                && live_burst_count == 0
                && (ring_done_ || !ring_active_))
            {
                alive_ = false;
            }
            break;
    }

    old_state_ = state_;
}

TFireBallEffect* TFireBallEffect::SpawnForTest(const S3DPoint& origin)
{
    if (!Renderer)
    {
        log_error("[fireball] SpawnForTest: renderer not initialized");
        return nullptr;
    }

    // FindImagery → RegisterImagery fallback (NewFireBall.I3D is not
    // pre-registered by any AddType in the modern data path; the only
    // registration was the disabled `DEFINE_BUILDER("FireBall", ...)` in
    // missileeffect.cpp).
    int32_t img_id = TObjectImagery::FindImagery(kFireBallImageryPath);
    if (img_id < 0)
        img_id = TObjectImagery::RegisterImagery(const_cast<char*>(kFireBallImageryPath));
    if (img_id < 0)
    {
        log_error("[fireball] SpawnForTest: FindImagery/RegisterImagery('%s') failed",
                  kFireBallImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[fireball] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kFireBallImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fireball] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kFireBallImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    // Lazy-mesh-init poke (same idiom F01/B01/H04/X22 use).
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < 3 || num_tex <= 0)
    {
        log_error("[fireball] SpawnForTest: imagery '%s' underspec'd "
                  "(objects=%d, textures=%d) — expected ≥3 sub-objects "
                  "(box01/box02/cylinder01)",
                  kFireBallImageryPath, num_obj, num_tex);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* fb = new TFireBallEffect(base);
    fb->ForcePos(origin);
    fb->SetMapIndex(MapPane.MakeIndex());
    fb->ActivateComponents();

    // Resolve box01 (idx 0) and box02 (idx 1) textures via their texfaces
    // table. NewFireBall.I3D's texture[0] is the warm-orange 4×4 atlas;
    // texture[1] (if present) is the spark sprite. Pre-release picks
    // these via GetObject(0)/GetObject(1) at register time.
    auto resolve_subobj_tex = [&](int32_t obj_idx, const char* tag) -> TTextureHandle {
        const int32_t slot = FireBallSubObjTextureSlot(img3d, obj_idx);
        const int32_t actual = (slot >= 0 && slot < num_tex) ? slot : 0;
        S3DTex tex = {};
        img3d->GetTexture(actual, &tex);
        log_info("[fireball]   %s sub-obj=%d -> texture slot=%d handle=%u "
                 "w=%u h=%u frames=%d",
                 tag, obj_idx, actual, tex.htexture,
                 tex.desc.width, tex.desc.height, tex.numframes);
        return tex.htexture;
    };
    fb->asset_.box01_tex = resolve_subobj_tex(kFireBallObjBox01, "box01(ball/atlas)");
    fb->asset_.box02_tex = resolve_subobj_tex(kFireBallObjBox02, "box02(spark)");

    if (fb->asset_.box01_tex == kInvalidTexture)
    {
        log_error("[fireball] SpawnForTest: box01 texture unresolved — port broken");
        delete fb;
        return nullptr;
    }
    if (fb->asset_.box02_tex == kInvalidTexture)
        fb->asset_.box02_tex = fb->asset_.box01_tex;   // safe fallback (atlas has warm cells)

    // Register the cylinder01 sub-mesh for additive-blend helper-mesh
    // submission (the shockwave ring). Same path TTeleporterEffect uses
    // for its glow column. We pull texslot 0 if there's no resolvable
    // material binding; the ring's geometry doesn't carry the warm
    // gradient (that comes from our per-frame vertex tint).
    {
        const int32_t cyl = kFireBallObjCylinder01;
        const int32_t texslots = num_tex + 1;
        for (int32_t texslot = 0; texslot < texslots; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(img3d, cyl, texslot, verts, indices))
                continue;
            if (verts.empty() || indices.empty()) continue;
            TTextureHandle albedo = Renderer->WhiteTextureHandle();
            if (texslot > 0 && texslot - 1 < num_tex)
            {
                S3DTex t = {};
                img3d->GetTexture(texslot - 1, &t);
                if (t.htexture != kInvalidTexture) albedo = t.htexture;
            }
            const MeshHandle h = Renderer->RegisterMesh(
                verts.data(), int32_t(verts.size()),
                indices.data(), int32_t(indices.size()),
                albedo);
            if (h)
            {
                fb->asset_.ring_mesh = h;
                BuildStaticObjectMatrix(img3d, cyl, 0, 0, fb->asset_.ring_mat);
                log_info("[fireball]   cylinder01 sub-obj=%d texslot=%d "
                         "verts=%zu indices=%zu mesh_handle=%u",
                         cyl, texslot, verts.size(), indices.size(), h);
                break;
            }
        }
        if (fb->asset_.ring_mesh == 0)
        {
            log_warn("[fireball] SpawnForTest: cylinder01 mesh extract failed — "
                     "ring won't draw (impact will still produce burst+sparks)");
        }
    }

    // Harness aim: random horizontal byte-angle (in-game caller would
    // supply via TSpell::Timer + GetAngle()).
    fb->aim_angle_ = random(0, 255);

    // Mirror TFireBallAnimator::Initialize z-lift (:511-514): nudge the
    // effect +50 wu on z so the ball grows above the caster.
    S3DPoint p = fb->Pos();
    p.z += int32_t(kFireBallSpawnLiftZ);
    fb->ForcePos(p);

    log_info("[fireball] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "aim=%d box01_tex=%u box02_tex=%u ring_mesh=%u "
             "fly_range_ticks=%d",
             kFireBallImageryPath, fb->GetMapIndex(),
             origin.x, origin.y, origin.z,
             fb->aim_angle_,
             fb->asset_.box01_tex, fb->asset_.box02_tex, fb->asset_.ring_mesh,
             kFireBallFlyRangeTicks);
    return fb;
}

void TFireBallEffect::TickAndSubmit(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // 24 Hz sim-tick accumulator (family pattern). Each drained tick
    // runs the full state machine + animator + spark integration once.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kFireBallSimTickMs))
        {
            sim_accum_ms_ -= double(kFireBallSimTickMs);
            StepMissilePulse();
            StepAnimate();
            if (!alive_)
                break;
        }
    }

    // Submit billboards every render frame (the sim cadence is the
    // simulation; rendering reads the latest tick state). The box01
    // ball/glow/trail/burst + box02 spark quads all go through the
    // FB pipeline.
    SubmitBillboards(debug_mode);

    // Spell-carried point light (forensics §9). Re-added each frame
    // while alive (LS-pipeline contract: ClearPointLights() at the top
    // of VfxTest::Render, effects rebuild per frame).
    if (alive_)
    {
        const S3DPoint p = Pos();
        const float wx = float(p.x) + fireball_.pos.X;
        const float wy = float(p.y) + fireball_.pos.Y;
        const float wz = float(p.z) + fireball_.pos.Z;
        // Intensity tracks the ball's glow flicker (×1.0..1.75 jitter
        // re-rolled each tick) so the warm scene light visibly pulses
        // with the ball's flame flicker.
        const float intensity = kFireBallLightInt * fireball_.glow;
        Renderer->AddPointLight(wx, wy, wz,
                                kFireBallLightRadiusWu,
                                kFireBallLightR, kFireBallLightG, kFireBallLightB,
                                intensity);
    }
}

void TFireBallEffect::SubmitBillboards(EFxDebugMode debug_mode) const
{
    if (!Renderer || asset_.box01_tex == kInvalidTexture)
        return;

    const S3DPoint base = Pos();

    // Per-instance facing rotation. Snapshot draw matrices apply
    // `RotateZ(-(GetFace(...)/256) * TORADIAN)` as the last spin
    // (missileeffect.cpp:846/906/953/1002/1041), composed under the
    // fixed -30°/+60° XY tilt + per-slot rotation. We collapse those
    // into one in-plane rotation per WorldXY-tipped quad:
    //   rotation_rad = -slot.rotation * (π/180)  +  facing_rad
    // facing comes from aim_angle_ (the byte-angle 0..255 we stored at
    // launch) — same source the snapshot used via GetFace.
    const float facing_rad = -(float(aim_angle_) / 256.0f) * 2.0f * float(M_PI);

    // Render-order port of TFireBallAnimator::Render (missileeffect.cpp:
    // 1060-1085): spark first; then per state — LAUNCH/FLY: glow, trail,
    // ball; EXPLODE: trail, burst, ring (ring drawn in SubmitWorldRing).
    //
    // Pipeline = FB particle (SubmitFxParticle / SParticleDrawItem) —
    // billboards don't plumb per-instance rotation_rad, but particles do
    // (renderer.h:248-263). Sister effects (Fizzle, B01 FLY droplets)
    // use the particle pipeline for the same reason: keep the snapshot's
    // matrix-tilt + per-instance spin without re-implementing matrices.
    // Orientation = WorldXY so each quad lies in the world plane the
    // snapshot's RotateX(-30°)/RotateY(+60°) defined; the per-instance
    // rotation_rad then spins it in that plane.
    SParticleDrawItem ball_item = {};
    ball_item.key.texture     = asset_.box01_tex;
    ball_item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    // Blend = Alpha — pre-release calls SetBlendState() = MODULATE/
    // SRCALPHA/INVSRCALPHA (missileeffect.cpp:1062-1063 + forensics §7).
    // The black-keyed atlas supplies transparency; the modulate stage
    // lets the warm-orange texel tint dominate.
    ball_item.key.blend       = uint8_t(EFxBlend::Alpha);
    ball_item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    ball_item.light_mode      = EFxLightMode::Unlit;
    ball_item.orientation     = EFxBillboardOrientation::WorldXY;
    ball_item.debug_mode      = debug_mode;
    // White tint — color lives in the warm-orange atlas texels per §10.
    ball_item.color_rgba[0] = 1.0f;
    ball_item.color_rgba[1] = 1.0f;
    ball_item.color_rgba[2] = 1.0f;
    ball_item.color_rgba[3] = 1.0f;

    // --- Spark trail (TSubParticleAnimator::Render — effectcomp.cpp:
    // 526-570 in spirit). Drawn first so the ball + glow layer on top.
    // Sparks have no per-instance rotation in the snapshot (the original
    // `TSubParticleAnimator::Render` uses a fixed rotZ-60°/rotX-45° face
    // rotation, not a per-particle accumulator), so rotation_rad stays
    // 0 — but they still ride the WorldXY tip so they read as embers in
    // the world plane, not camera-pasted dots.
    SParticleDrawItem spark_item = ball_item;
    spark_item.key.texture = asset_.box02_tex;
    spark_item.key.blend   = uint8_t(EFxBlend::Alpha);
    spark_item.uv_rect[0] = 0.0f; spark_item.uv_rect[1] = 0.0f;
    spark_item.uv_rect[2] = 1.0f; spark_item.uv_rect[3] = 1.0f;
    spark_item.rotation_rad = 0.0f;
    for (const auto& sp : sparks_)
    {
        if (!sp.used) continue;
        const float flicker = ((sp.life & 1) == 0) ? kFireBallSparkFlicker : 1.0f;
        spark_item.world_pos[0] = sp.pos.X;
        spark_item.world_pos[1] = sp.pos.Y;
        spark_item.world_pos[2] = sp.pos.Z;
        spark_item.size_wu[0] = kFireBallSparkQuadWu * sp.scale * flicker * 6.0f;
        spark_item.size_wu[1] = kFireBallSparkQuadWu * sp.scale * flicker * 6.0f;
        Renderer->SubmitFxParticle(spark_item);
    }

    constexpr float kDegToRad = float(M_PI) / 180.0f;

    if (state_ == 0 /*LAUNCH*/ || state_ == 1 /*FLY*/)
    {
        // --- Glow (RenderFireBallGlow — missileeffect.cpp:1022-1057).
        // box01 quad with atlas cell = glow_frame, scale = scale * glow.
        // The original glow has NO per-instance spin (only the static
        // -30°/+60° tilt + facing — :1039-1041), so we leave its
        // rotation_rad at facing_rad with no rotation term.
        SParticleDrawItem glow = ball_item;
        FillAtlasUv(glow_frame_, glow.uv_rect);
        const float gsize = kFireBallBaseQuadWu * fireball_.scale * fireball_.glow;
        glow.size_wu[0] = gsize;
        glow.size_wu[1] = gsize;
        glow.world_pos[0] = float(base.x) + fireball_.pos.X;
        glow.world_pos[1] = float(base.y) + fireball_.pos.Y;
        glow.world_pos[2] = float(base.z) + fireball_.pos.Z;
        glow.rotation_rad = facing_rad;
        Renderer->SubmitFxParticle(glow);

        // --- Mesh trail (RenderFireBallTrail — :871-973). The original
        // applies `RotateZ(-trail[i].rotation * TORADIAN)` PER SLOT
        // (:948), where trail[i].rotation is the stale snapshot of the
        // ball's rotation at the moment that slot was recorded (via
        // `trail[0] = fireball` at :632). That stale-spin propagation is
        // what gives the trail its tumbling streak look — each card sits
        // at a frozen angle in the WorldXY plane, NOT camera-facing.
        SParticleDrawItem trail_item = ball_item;
        // Walk back-to-front so older slots draw first.
        for (int32_t i = kFireBallTrailSize - 1; i >= 0; --i)
        {
            const SFireBallData& tr = trail_[i];
            if (tr.scale <= 0.0001f) continue;
            FillAtlasUv(int32_t(tr.frame), trail_item.uv_rect);
            const float tsize = kFireBallBaseQuadWu * tr.scale;
            trail_item.size_wu[0] = tsize;
            trail_item.size_wu[1] = tsize;
            trail_item.world_pos[0] = tr.pos.X;
            trail_item.world_pos[1] = tr.pos.Y;
            trail_item.world_pos[2] = tr.pos.Z;
            trail_item.rotation_rad = -tr.rotation * kDegToRad + facing_rad;
            Renderer->SubmitFxParticle(trail_item);
        }

        // --- Ball (RenderFireBall — :975-1020). Per-instance spin from
        // the ball-head accumulator (`fireball_.rotation`, +2°/tick).
        SParticleDrawItem ball = ball_item;
        FillAtlasUv(int32_t(fireball_.frame), ball.uv_rect);
        const float bsize = kFireBallBaseQuadWu * fireball_.scale;
        ball.size_wu[0] = bsize;
        ball.size_wu[1] = bsize;
        ball.world_pos[0] = float(base.x) + fireball_.pos.X;
        ball.world_pos[1] = float(base.y) + fireball_.pos.Y;
        ball.world_pos[2] = float(base.z) + fireball_.pos.Z;
        ball.rotation_rad = -fireball_.rotation * kDegToRad + facing_rad;
        Renderer->SubmitFxParticle(ball);
    }
    else /* state_ == MISSILE_EXPLODE */
    {
        // EXPLODE branch (:1075-1079): trail, burst, ring. The trail
        // keeps draining at the frozen stale-spin angles; ring is drawn
        // via SubmitHelperMesh.
        SParticleDrawItem trail_item = ball_item;
        for (int32_t i = kFireBallTrailSize - 1; i >= 0; --i)
        {
            const SFireBallData& tr = trail_[i];
            if (tr.scale <= 0.0001f) continue;
            FillAtlasUv(int32_t(tr.frame), trail_item.uv_rect);
            const float tsize = kFireBallBaseQuadWu * tr.scale;
            trail_item.size_wu[0] = tsize;
            trail_item.size_wu[1] = tsize;
            trail_item.world_pos[0] = tr.pos.X;
            trail_item.world_pos[1] = tr.pos.Y;
            trail_item.world_pos[2] = tr.pos.Z;
            trail_item.rotation_rad = -tr.rotation * kDegToRad + facing_rad;
            Renderer->SubmitFxParticle(trail_item);
        }

        // Burst quads (RenderFireBallBurst — :793-869). Snapshot seeds
        // `burst[i].rotation = 0` (:694), so the per-slot rotation_rad
        // collapses to just the facing_rad term — the matrix tilt at
        // :846-851 reduces to the WorldXY plane + facing rotation.
        SParticleDrawItem burst = ball_item;
        for (const auto& bq : burst_)
        {
            if (!bq.used) continue;
            FillAtlasUv(int32_t(bq.frame), burst.uv_rect);
            const float bsize = kFireBallBaseQuadWu * bq.scale * bq.glow;
            burst.size_wu[0] = bsize;
            burst.size_wu[1] = bsize;
            burst.world_pos[0] = float(base.x) + bq.pos.X;
            burst.world_pos[1] = float(base.y) + bq.pos.Y;
            burst.world_pos[2] = float(base.z) + bq.pos.Z;
            burst.rotation_rad = -bq.rotation * kDegToRad + facing_rad;
            Renderer->SubmitFxParticle(burst);
        }
    }
}

void TFireBallEffect::SubmitWorldRing(EFxDebugMode /*debug_mode*/)
{
    // Shockwave ring (cylinder01 via SubmitHelperMesh, additive blend).
    // Same primitive M09b TTeleporterEffect uses (forensics §7).
    if (!Renderer || asset_.ring_mesh == 0 || !ring_active_)
        return;

    // Compose the world matrix: T(ring_pos) * S(ring_scale,*,*) applied
    // to the cylinder's authored parent matrix. The ring grows isotropic
    // (all 3 axes scale together), matching ring.scale_factor = 1.085 on
    // x/y/z (missileeffect.cpp:715-717).
    float t[16] = {
        ring_scale_, 0.0f,        0.0f,        ring_pos_.X,
        0.0f,        ring_scale_, 0.0f,        ring_pos_.Y,
        0.0f,        0.0f,        ring_scale_, ring_pos_.Z,
        0.0f,        0.0f,        0.0f,        1.0f
    };
    float world[16];
    // world = t * parent_matrix (row-major). Reuse the renderer-side
    // helper structure of TTeleporterEffect — but the helper is local
    // there. Compose inline.
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t k = 0; k < 4; ++k)
                s += t[r * 4 + k] * asset_.ring_mat[k * 4 + c];
            world[r * 4 + c] = s;
        }

    // Alpha-fade per SHOCKWAVE_FLAG_FADE (effectcomp.cpp:732-748):
    // alpha = (max_size - scale) / (max_size - init_scale).
    const float fade_num = kFireBallRingMaxSize - ring_scale_;
    const float fade_den = kFireBallRingMaxSize - kFireBallRingScale;
    const float fade     = fade_den > 0.0f
                               ? std::fmax(0.0f, std::fmin(1.0f, fade_num / fade_den))
                               : 1.0f;

    SHelperMeshSubmit m = {};
    m.mesh           = asset_.ring_mesh;
    m.additive_blend = true;
    m.shadow_plane   = false;
    std::memcpy(m.world, world, sizeof(world));

    // Warm-orange ring tint — the 4 ring color stops (forensics §10):
    // mid stop = (0.78,0.24,0.06) red-orange / (0.97,0.61,0.06) orange.
    // Helper-mesh shader emissive carries the color directly (additive
    // pass = self-luminant), modulated by `fade` so the ring fades out
    // as it grows past max_size.
    m.diffuse[0]  = 1.0f; m.diffuse[1]  = 1.0f; m.diffuse[2]  = 1.0f; m.diffuse[3]  = 1.0f;
    m.ambient[0]  = 0.0f; m.ambient[1]  = 0.0f; m.ambient[2]  = 0.0f; m.ambient[3]  = 1.0f;
    m.specular[0] = 0.0f; m.specular[1] = 0.0f; m.specular[2] = 0.0f; m.specular[3] = 0.0f;
    m.emissive[0] = 0.97f * fade;   // bright orange
    m.emissive[1] = 0.45f * fade;
    m.emissive[2] = 0.06f * fade;
    m.emissive[3] = 1.0f;
    m.power       = 1.0f;
    m.sort_depth  = float(Pos().z);

    Renderer->SubmitHelperMesh(m);
}

#if 0
// REFERENCE — in-game `TMissileEffect::Pulse` impact-detection block.
// The harness path in StepMissilePulse omits the MOVE_BLOCKED / character
// hit branches because --test=vfx has no map collision or live character
// iteration. When the in-game caller wires the fireball into TSpell::
// Timer, this is the body that fires. Preserved verbatim from
// missileeffect.cpp:75-114 so the gameflow porter has a one-stop
// reference. Re-enable by reading the engine's Move() flags + TMapIterator
// when those hooks are wired into the modern path.
case 1 /* MISSILE_FLY */:
{
    bool explode = false;
    range_--;
    if (range_ <= 0)
        explode = true;
    if (bits & MOVE_BLOCKED)
        explode = true;
    else
    {
        PTCharacter invoker = spell ? (PTCharacter)spell->GetInvoker() : nullptr;
        for (TMapIterator i(nullptr, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
        {
            PTCharacter chr = (PTCharacter)i.Item();
            if (chr == invoker)            continue;
            if (chr->IsDead())             continue;
            if (this->Distance(chr) > 32)  continue;
            if (invoker && !invoker->IsEnemy(chr)) continue;
            explode = true;
            break;
        }
    }
    if (explode)
    {
        flags = (flags & ~OF_MOVING & ~OF_WEIGHTLESS) | OF_IMMOBILE;
        state_ = 2 /*MISSILE_EXPLODE*/;
    }
    break;
}
#endif

// *************************************************************************
// * Ice family bespoke ports — wave-bespoke-03-ice                         *
// *************************************************************************
//
// Three sister effects: I20 (TIceEffect stub — no body), I21
// (TIceBoltEffect — composite freeze beam), I22 (TIcedEffect — frozen
// overlay + shatter). Faithful direct ports of the snapshot animator
// bodies at src/effect_old.cpp:8015-8688 (I21) and :8749-8953 (I22).
// Only render API calls change (Renderer->SubmitFxBillboard); per-tick
// math, variable names, constants, and pass order preserved.
//
// Forensics: docs/vfx/forensics/I21_TIceBoltEffect.md,
//            docs/vfx/forensics/I22_TIcedEffect.md.

// --- I20 TIceEffect stub ------------------------------------------------
// Brief: "needs forensics doc first — DO NOT port code blind". Snapshot
// has only class declarations (effect.h:607-615), no Animate/Render
// body anywhere in src/effect_old.cpp or src/effectcomp.cpp. SpawnForTest
// returns nullptr + log_warn; harness draws nothing. Re-enable once a
// forensics doc exists. Status: BLOCKED.
TIceEffect_Bespoke* TIceEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& /*origin*/)
{
    log_warn("[ice/I20] SpawnForTest: BLOCKED — no snapshot body and no "
             "forensics doc; harness draws nothing for this id. "
             "See wave-bespoke-03-ice brief: 'needs forensics doc first — "
             "DO NOT port code blind'.");
    return nullptr;
}

void TIceEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode /*debug_mode*/)
{
    // No-op (paired with the nullptr SpawnForTest above).
}

// --- I21 TIceBoltEffect bespoke -----------------------------------------
//
// FAITHFUL DIRECT PORT of TIceBoltAnimator (snapshot effect_old.cpp:
// 8015-8688). Stationary freeze beam from caster outward — NOT a flying
// missile (TIceBoltEffect base is TEffect; the TMissileEffect calls in
// the snapshot Initialize/Pulse are commented out, retail-confirmed via
// cls_0x5aaf28 inheritance, forensics §6.0).
//
// Engine adaptations:
//   - Render path swaps RenderObject(obj) for SubmitFxBillboard. The
//     7 sub-objects of icebolt.I3D collapse to billboard quads in the
//     bespoke first-pass — the cylinder stretch / spiral twist / ring
//     slide become spatial billboard placement, faithful in count and
//     position but not in mesh geometry. This is the "compiles + boots
//     + renders something" first pass per the brief; a future pass can
//     swap the cylinders to SubmitHelperMesh (cf. F07 ring) for proper
//     mesh draws once the cylinder UVs are extracted.
//   - 24 Hz sim-tick gate (family pattern blood/spark/fireball/iced)
//     replaces the snapshot's per-render-frame integration. Per-tick
//     math (FROST_STEP=0.03, SPIRAL_STEP=1.0, etc.) runs once per
//     accumulated 1/24 s of wall-clock time.
//   - Length march (snapshot :8152-8233) is replaced with a fixed
//     harness `length_` since --test=vfx has no caster/targets/MapPane.
//   - Gameplay beats (DamageCharactersInRange @beat10,
//     "Iced"-posting @beat20) are GUARDED OUT for the standalone
//     harness (no spell/invoker/MapPane). They're preserved as comments
//     citing the snapshot lines so the in-game wire-up is one paste away.
//   - SetSubSpell(3) forced (snapshot :8049), all visual tiers active.

namespace {

constexpr const char* kIceBoltImageryPath = "Magic\\icebolt.I3D";

// Snapshot constants (forensics §3). Same names + values as
// effect_old.cpp:7972-7982 — line-for-line preserved.
constexpr int32_t kIceBoltGrowDuration     = 10;        // GROW_DURATION
constexpr float   kIceBoltSphereStep       = 0.5f;      // SPHERE_STEP
constexpr float   kIceBoltRingSpeed        = 20.0f;     // RING_SPEED
constexpr float   kIceBoltSpiralStep       = 1.0f;      // SPIRAL_STEP
constexpr float   kIceBoltSpiralScaleStep  = 0.1f;      // SPIRAL_SCALE_STEP
constexpr float   kIceBoltFrostInitSize    = 0.5f;      // FROST_INIT_SIZE
constexpr float   kIceBoltFrostStep        = 0.03f;     // FROST_STEP

// Harness: --test=vfx has no caster facing / no target march. Beam runs
// along local −y for a fixed length (snapshot length = ticks*10, ≤1000).
// 500 wu sits between snapshot's plausible 300-800 sample range.
constexpr float   kIceBoltHarnessLength    = 500.0f;
constexpr int32_t kIceBoltHarnessAngle     = 0;         // byte facing (0..255); +Y

// 7 sub-objects of icebolt.I3D (forensics §4). Names match the .I3D
// file order; the snapshot animator addresses them via GetObject(idx).
constexpr int32_t kIceBoltSubObjCount      = 7;
constexpr int32_t kIceBoltCylBase          = 0;   // cylinder04 (and 01/02/03 at 1..3)
constexpr int32_t kIceBoltBox01            = 4;   // glow sphere / frost / snow billboard
constexpr int32_t kIceBoltCylRing          = 5;   // cylinder05 — rings
constexpr int32_t kIceBoltCylSpiral        = 6;   // cylinder06 — spirals

// In-plane billboard size (snapshot scales sub-object meshes; here we
// approximate as a billboard quad). Tuned against the asset's bbox.
constexpr float   kIceBoltCylSizeWu        = 24.0f;     // core beam quad
constexpr float   kIceBoltGlowSizeWu       = 48.0f;     // end glow sphere
constexpr float   kIceBoltFrostSizeWu      = 12.0f;     // frost particle
constexpr float   kIceBoltSnowSizeWu       = 12.0f;     // snow particle
constexpr float   kIceBoltRingSizeWu       = 32.0f;     // ring quad
constexpr float   kIceBoltSpiralSizeWu     = 18.0f;     // spiral quad

}   // namespace

TIceBoltEffect_Bespoke* TIceBoltEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // --- Load Magic\icebolt.I3D (forensics §4 — byte-identical between
    // snapshot and shipped retail, MD5 confirmed).
    const int32_t img_id = TObjectImagery::FindImagery(kIceBoltImageryPath);
    if (img_id < 0)
    {
        log_error("[icebolt] SpawnForTest: FindImagery('%s') failed",
                  kIceBoltImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[icebolt] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kIceBoltImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[icebolt] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kIceBoltImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* bolt = new TIceBoltEffect_Bespoke(base);
    // Mirror TIceBoltAnimator::Initialize :8022-8026 — z-lift +50 wu
    // (caster pos → hand height for the beam origin).
    S3DPoint anchored = origin;
    anchored.z += 50;
    bolt->ForcePos(anchored);
    bolt->SetMapIndex(MapPane.MakeIndex());
    bolt->ActivateComponents();

    // Lazy-mesh-init poke (B01 / F07 / X22 pattern). NumObjects triggers
    // the mesh load that fills in textures.
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj < kIceBoltSubObjCount || num_tex <= 0)
    {
        log_error("[icebolt] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d) — expected >=%d sub-objects",
                  num_obj, num_tex, kIceBoltSubObjCount);
        delete bolt;
        return nullptr;
    }

    // Resolve each of the 7 sub-objects to (texture, UV sub-rect).
    // Forensics §4: all 7 share the .I3D's inline texture pool. Index 4
    // (box01) carries the glow sphere / frost / snow billboard; it's the
    // critical one — if it fails to resolve the entire particle cloud
    // draws nothing.
    for (int32_t i = 0; i < kIceBoltSubObjCount; ++i)
    {
        const int32_t tex_slot = SubObjTextureSlot(img3d, i);
        const int32_t slot = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
        S3DTex tex = {};
        img3d->GetTexture(slot, &tex);
        bolt->subobj_tex_[i] = tex.htexture;
        ResolveSubObjUv(img3d, i, bolt->subobj_uv_[i]);
    }
    if (bolt->subobj_tex_[kIceBoltBox01] == kInvalidTexture)
    {
        log_error("[icebolt] SpawnForTest: box01 (sub-obj 4) texture "
                  "unresolved — glow sphere / frost / snow particles "
                  "will draw nothing");
    }

    // Port of TIceBoltAnimator::Initialize (effect_old.cpp:8015-8135).
    // Same variable names, same per-line assignment, same constants.
    bolt->frameon_       = 0;
    bolt->spherescale_[0] = kIceBoltSphereStep;          // :8029
    bolt->spherescale_[1] = 1.0f;                         // :8030
    bolt->cylscale_      = kIceBoltSphereStep;            // :8031
    bolt->ringout_       = 0.0f;                          // :8032
    bolt->spiralang_     = 0.0f;                          // :8033
    bolt->spiralscale_   = kIceBoltSpiralScaleStep;       // :8034
    bolt->cy_            = 0.0f;                          // :8129
    bolt->angle_         = kIceBoltHarnessAngle;          // :8047 GetAngle — harness fixed
    bolt->subspell_      = 3;                              // :8049 SetSubSpell(3) force
    bolt->length_        = kIceBoltHarnessLength;         // length march replaced

    // Frost particle seed (snapshot :8035-8045). Per-particle p[i],v[i]
    // jitter ±10 / ±3 wu, s[i]=FROST_INIT_SIZE, staggered delay
    // t[i] = random(0, FROST_INIT_SIZE*2/FROST_STEP) = random(0,33).
    for (int32_t i = 0; i < kIceBoltMaxFrost; ++i)
    {
        SIceBoltFrostParticle& f = bolt->frost_[i];
        f.pos.X = float(random(-10, 10));    // :8037
        f.pos.Y = float(random(-10, 10));    // :8038
        f.pos.Z = float(random(-10, 10));    // :8039
        f.vel.X = float(random(-3, 3));      // :8040
        f.vel.Y = float(random(-3, 3));      // :8041
        f.vel.Z = float(random(-3, 3));      // :8042
        f.s     = kIceBoltFrostInitSize;     // :8043
        f.t     = float(random(0,
            int32_t(kIceBoltFrostInitSize * 2.0f / kIceBoltFrostStep))); // :8044
    }

    // Snow particle seed (snapshot :8119-8133, subspell>2 branch — always
    // true here since subspell forced to 3 at :8049).
    for (int32_t i = 0; i < kIceBoltMaxSnow; ++i)
    {
        SIceBoltSnowParticle& sn = bolt->snow_[i];
        sn.h  = float(random(-50, 50));                                       // :8123
        sn.th = float(random(0, 359)) * float(TORADIAN);                      // :8124
        sn.rs = float(random(5, 15)) / 100.0f;                                // :8125
        sn.sz = float(random(int32_t(kIceBoltFrostStep * 100.0f),
                            int32_t(kIceBoltFrostStep
                                    * float(kIceBoltGrowDuration) * 100.0f))) / 100.0f; // :8126
        sn.r  = float(random(25, 50));                                        // :8127
    }

    log_info("[icebolt] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "length=%.0f angle=%d subspell=%d subobj_tex={%u,%u,%u,%u,%u,%u,%u}",
             kIceBoltImageryPath, bolt->GetMapIndex(),
             anchored.x, anchored.y, anchored.z,
             bolt->length_, bolt->angle_, bolt->subspell_,
             bolt->subobj_tex_[0], bolt->subobj_tex_[1], bolt->subobj_tex_[2],
             bolt->subobj_tex_[3], bolt->subobj_tex_[4], bolt->subobj_tex_[5],
             bolt->subobj_tex_[6]);
    return bolt;
}

void TIceBoltEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TIceBoltAnimator::Animate (effect_old.cpp:
    // 8144-8455), framerate-independent via the 24 Hz sim-tick
    // accumulator. Each accumulated tick runs the original per-tick
    // integration exactly once.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kIceBoltSimTickMs))
        {
            sim_accum_ms_ -= double(kIceBoltSimTickMs);

            // Snapshot :8152-8232 — length march. Harness skips
            // (no caster/targets/MapPane); length_ pinned at spawn.

            frameon_++;  // :8234

            // Snow tick (snapshot :8236-8273). subspell>2 branch always
            // true (force-3 at :8049).
            if (subspell_ > 2)
            {
                for (int32_t i = 0; i < kIceBoltMaxSnow; ++i)   // :8238
                {
                    SIceBoltSnowParticle& sn = snow_[i];
                    sn.th += sn.rs;                              // :8240
                    if (sn.th > float(M_2PI))
                        sn.th -= float(M_2PI);                   // :8242
                    sn.rs += 0.01f;                              // :8243
                }
                if (frameon_ > kIceBoltGrowDuration * 2)         // :8245
                {
                    cy_ += length_ / float(kIceBoltGrowDuration * 2);  // :8247
                    if (cy_ >= length_)
                        cy_ = length_;                           // :8249
                }
                if (frameon_ > kIceBoltGrowDuration * 5)         // :8251
                {
                    for (int32_t i = 0; i < kIceBoltMaxSnow; ++i)
                    {
                        SIceBoltSnowParticle& sn = snow_[i];
                        sn.h += 5.0f;                            // :8266
                        sn.r -= 1.0f;                            // :8267
                        if (sn.r < 1.0f)
                            sn.r = 1.0f;                         // :8269
                        sn.rs += 0.05f;                          // :8270
                    }
                }
            }

            // Frost tick (snapshot :8331-8355).
            for (int32_t i = 0; i < kIceBoltMaxFrost; ++i)
            {
                SIceBoltFrostParticle& f = frost_[i];
                f.t -= 1.0f;                                     // :8333
                if (f.t > 0)
                    continue;                                    // :8334
                f.t = 0;                                          // :8337
                f.pos.X += f.vel.X;                              // :8338
                f.pos.Y += f.vel.Y;                              // :8339
                f.pos.Z += f.vel.Z;                              // :8340
                f.s -= kIceBoltFrostStep;                        // :8341
                if (f.s < 0)                                      // :8342
                {
                    // Respawn (:8344-8353).
                    f.pos.X = float(random(-10, 10));
                    f.pos.Y = float(random(-10, 10));
                    f.pos.Z = float(random(-10, 10));
                    f.vel.X = float(random(-3, 3));
                    f.vel.Y = float(random(-3, 3));
                    f.vel.Z = float(random(-3, 3));
                    f.s     = kIceBoltFrostInitSize;
                    f.t     = float(random(0,
                        int32_t(kIceBoltFrostInitSize / kIceBoltFrostStep)));
                    if (frameon_ > kIceBoltGrowDuration * 3)
                        f.t = 100.0f;                            // :8353
                }
            }

            // Beam-body transform envelope (snapshot :8356-8392).
            if (frameon_ < kIceBoltGrowDuration)                 // :8356
            {
                spherescale_[0] += kIceBoltSphereStep;
                spherescale_[1] += kIceBoltSphereStep * 2.0f;
                cylscale_       += kIceBoltSphereStep;
                spiralscale_    += kIceBoltSpiralScaleStep;
                spiralang_      += kIceBoltSpiralStep;
                if (spiralang_ > float(M_2PI))
                    spiralang_ -= float(M_2PI);
            }
            if (frameon_ >= kIceBoltGrowDuration
                && frameon_ < kIceBoltGrowDuration * 2)          // :8366
            {
                cylscale_       -= kIceBoltSphereStep * 0.5f;
                spherescale_[0] += kIceBoltSphereStep;
                spherescale_[1] += kIceBoltSphereStep;
                spiralang_      += kIceBoltSpiralStep;
                if (spiralang_ > float(M_2PI))
                    spiralang_ -= float(M_2PI);
            }
            if (frameon_ >= kIceBoltGrowDuration * 2
                && frameon_ < kIceBoltGrowDuration * 3)          // :8375
            {
                cylscale_       -= kIceBoltSphereStep * 0.5f;
                spherescale_[0] -= kIceBoltSphereStep;
                spherescale_[1] -= kIceBoltSphereStep;
                spiralang_      += kIceBoltSpiralStep;
                if (spiralang_ > float(M_2PI))
                    spiralang_ -= float(M_2PI);
            }
            if (frameon_ >= kIceBoltGrowDuration * 3
                && frameon_ < kIceBoltGrowDuration * 4)          // :8384
            {
                spherescale_[0] -= kIceBoltSphereStep;
                spherescale_[1] -= kIceBoltSphereStep * 2.0f;
                spiralscale_    -= kIceBoltSpiralScaleStep;
            }
            ringout_ += kIceBoltRingSpeed;                       // :8390
            if (ringout_ > length_)
                ringout_ -= length_;                             // :8392

            // Gameplay beats — guarded out in harness (no spell/invoker/
            // MapPane). Snapshot :8393-8454 references for the gameflow
            // wire-up:
            //   frameon == GD       → DamageCharactersInRange(... DAMAGE_ICE)
            //   frameon == 2*GD     → spawn "Iced" (I22) on each enemy
            //   frameon == 12*GD    → KillThisEffect()
            if (frameon_ >= kIceBoltGrowDuration * 12)           // :8451
            {
                alive_ = false;
                break;
            }
        }
    }

    if (!alive_)
        return;

    // --- Render: port of TIceBoltAnimator::Render (effect_old.cpp:
    // 8464-8688). The snapshot uses RenderObject(obj) on per-frame
    // matrix-transformed I3D sub-objects; the first-pass bespoke
    // collapses each draw to a SubmitFxBillboard quad in the same world
    // position, scale, and pass order. Cylinder stretch / spiral twist
    // are approximated as billboard placement (a future pass can swap to
    // SubmitHelperMesh for proper mesh draws, cf. F07 ring).
    //
    // Blend = Alpha (snapshot SetBlendState :8467, MODULATE,
    // SRC_ALPHA/INV_SRC_ALPHA — NOT additive — forensics §7).
    // Lit-mode = Unlit (asset color carries blue, no material zeroing).
    // Depth = TestNoWrite. Orientation = ScreenAligned (the snapshot
    // billboards use ScreenAligned-ish rotation, forensics §7).

    const S3DPoint& base_pos = Pos();
    // Beam direction in local frame: −y (forensics §5/§7). For the
    // first-pass bespoke we use byte-angle 0 (+Y) as the harness facing,
    // so the beam stretches along world +Y from origin.
    const float face_rad =
        float(angle_) * (2.0f * float(M_PI)) / 256.0f;
    const float dx = std::sin(face_rad);    // beam +X step direction
    const float dy = -std::cos(face_rad);   // beam −Y step direction (snapshot :8156)

    // Common billboard template.
    SBillboardDrawItem item = {};
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;

    // --- Core beam cylinders (snapshot :8472-8505).
    // 4 nested cylinders (cyl04/01/02/03 at sub-obj 0..3). The snapshot
    // stretches them to scl.z = length/64 along the beam axis; here we
    // stamp 8 billboard quads along the beam axis instead (count tuned
    // to give continuous beam read at length=500). Pass = first in
    // render order (depth-test on, alpha) — snapshot :8475 subspell>1.
    if (subspell_ > 1)
    {
        const int32_t beam_quads = 8;
        for (int32_t k = 0; k < beam_quads; ++k)
        {
            const float t = float(k) / float(beam_quads - 1);   // 0..1
            const float along = -t * length_;                    // 0..−length
            for (int32_t cyl = 0; cyl < 4; ++cyl)
            {
                const int32_t obj_idx = cyl;
                if (subobj_tex_[obj_idx] == kInvalidTexture)
                    continue;
                item.world_pos[0] = float(base_pos.x) + dx * (-along);
                item.world_pos[1] = float(base_pos.y) + dy * (-along);
                item.world_pos[2] = float(base_pos.z);
                item.size_wu[0] = item.size_wu[1] =
                    kIceBoltCylSizeWu * cylscale_;
                item.key.texture = subobj_tex_[obj_idx];
                std::memcpy(item.uv_rect, subobj_uv_[obj_idx],
                            sizeof(item.uv_rect));
                Renderer->SubmitFxBillboard(item);
            }
        }
    }

    // --- Spirals + rings (snapshot :8507-8571). subspell>0 +
    // frameon < GD*3 (snapshot :8507).
    if (frameon_ < kIceBoltGrowDuration * 3)
    {
        // Spirals (cyl06 = sub-obj 6, snapshot :8510-8545). numrevs =
        // length/100 segments, two mirrored helices per segment.
        if (subspell_ > 0
            && subobj_tex_[kIceBoltCylSpiral] != kInvalidTexture)
        {
            // `max` is a macro from revtypes.h:20, so std::max won't parse
            // here; expand inline.
            const int32_t numrevs_raw = int32_t(length_) / 100;
            const int32_t numrevs = numrevs_raw > 1 ? numrevs_raw : 1;
            const float zscale = length_ / float(numrevs);
            for (int32_t j = 0; j < numrevs; ++j)
            {
                const float ypos = -float(j) * zscale;
                for (int32_t i = 0; i < 2; ++i)
                {
                    const float ang = (i ? float(M_PI) - spiralang_
                                          : spiralang_);
                    // Helix offset perpendicular to beam axis.
                    const float ox = std::cos(ang) * kIceBoltSpiralSizeWu;
                    const float oz = std::sin(ang) * kIceBoltSpiralSizeWu;
                    item.world_pos[0] =
                        float(base_pos.x) + dx * (-ypos) + ox;
                    item.world_pos[1] =
                        float(base_pos.y) + dy * (-ypos);
                    item.world_pos[2] = float(base_pos.z) + oz;
                    item.size_wu[0] = item.size_wu[1] =
                        kIceBoltSpiralSizeWu * spiralscale_;
                    item.key.texture = subobj_tex_[kIceBoltCylSpiral];
                    std::memcpy(item.uv_rect, subobj_uv_[kIceBoltCylSpiral],
                                sizeof(item.uv_rect));
                    Renderer->SubmitFxBillboard(item);
                }
            }
        }
        // Rings (cyl05 = sub-obj 5, snapshot :8547-8570). length/100
        // instances, slide along beam at RING_SPEED, wrap by length.
        if (subobj_tex_[kIceBoltCylRing] != kInvalidTexture)
        {
            // `max`/`min` are macros from revtypes.h:20-21, so std::max won't
            // parse here; expand inline.
            const int32_t num_rings_raw = int32_t(length_) / 100;
            const int32_t num_rings = num_rings_raw > 1 ? num_rings_raw : 1;
            for (int32_t i = 0; i < num_rings; ++i)
            {
                // Snapshot :8563: y = -((ringout + 100*i) % length).
                const int32_t length_raw = int32_t(length_);
                const int32_t lengthi = length_raw > 1 ? length_raw : 1;
                const int32_t y_int =
                    -((int32_t(ringout_) + 100 * i) % lengthi);
                const float ypos = float(y_int);
                item.world_pos[0] = float(base_pos.x) + dx * (-ypos);
                item.world_pos[1] = float(base_pos.y) + dy * (-ypos);
                item.world_pos[2] = float(base_pos.z);
                item.size_wu[0] = item.size_wu[1] =
                    kIceBoltRingSizeWu * cylscale_;
                item.key.texture = subobj_tex_[kIceBoltCylRing];
                std::memcpy(item.uv_rect, subobj_uv_[kIceBoltCylRing],
                            sizeof(item.uv_rect));
                Renderer->SubmitFxBillboard(item);
            }
        }
    }

    // --- End glow spheres (snapshot :8573-8600). 2x box01, one at
    // y=0 (caster end), one at y=−length (target end). subspell>0.
    if (subspell_ > 0
        && subobj_tex_[kIceBoltBox01] != kInvalidTexture)
    {
        for (int32_t i = 0; i < 2; ++i)
        {
            const float ypos = -length_ * float(i);   // :8595
            item.world_pos[0] = float(base_pos.x) + dx * (-ypos);
            item.world_pos[1] = float(base_pos.y) + dy * (-ypos);
            item.world_pos[2] = float(base_pos.z);
            item.size_wu[0] = item.size_wu[1] =
                kIceBoltGlowSizeWu * (spherescale_[i] * 0.5f);
            item.key.texture = subobj_tex_[kIceBoltBox01];
            std::memcpy(item.uv_rect, subobj_uv_[kIceBoltBox01],
                        sizeof(item.uv_rect));
            Renderer->SubmitFxBillboard(item);
        }
    }

    // --- Frost particles (snapshot :8602-8628). 50 box01 quads at
    // p[i] offset −length in y (clustered at target end).
    if (subobj_tex_[kIceBoltBox01] != kInvalidTexture)
    {
        for (int32_t i = 0; i < kIceBoltMaxFrost; ++i)
        {
            const SIceBoltFrostParticle& f = frost_[i];
            if (f.t > 0)
                continue;                                        // :8605
            // Snapshot :8622-8624: pos = (p.x, p.y − length, p.z).
            const float lx = f.pos.X;
            const float ly = f.pos.Y - length_;
            const float lz = f.pos.Z;
            // Rotate local (lx,ly) into world by beam facing (dx,dy).
            item.world_pos[0] =
                float(base_pos.x) + dx * (-ly) + lx;
            item.world_pos[1] =
                float(base_pos.y) + dy * (-ly);
            item.world_pos[2] = float(base_pos.z) + lz;
            item.size_wu[0] = item.size_wu[1] =
                kIceBoltFrostSizeWu * f.s;
            item.key.texture = subobj_tex_[kIceBoltBox01];
            std::memcpy(item.uv_rect, subobj_uv_[kIceBoltBox01],
                        sizeof(item.uv_rect));
            Renderer->SubmitFxBillboard(item);
        }
    }

    // --- Snow flurry (snapshot :8630-8657). subspell>2; orbits the
    // beam axis, sweeps along beam after beat 2*GD, blows up after 5*GD.
    if (subspell_ > 2
        && subobj_tex_[kIceBoltBox01] != kInvalidTexture)
    {
        for (int32_t i = 0; i < kIceBoltMaxSnow; ++i)
        {
            const SIceBoltSnowParticle& sn = snow_[i];
            // Snapshot :8649-8653: pos = (r·cos th, r·sin th − cy?, h).
            const float lx = sn.r * std::cos(sn.th);
            float       ly = sn.r * std::sin(sn.th);
            if (frameon_ > kIceBoltGrowDuration * 2)
                ly -= cy_;                                       // :8652
            const float lz = sn.h;
            item.world_pos[0] =
                float(base_pos.x) + dx * (-ly) + lx;
            item.world_pos[1] =
                float(base_pos.y) + dy * (-ly);
            item.world_pos[2] = float(base_pos.z) + lz;
            item.size_wu[0] = item.size_wu[1] =
                kIceBoltSnowSizeWu * sn.sz;
            item.key.texture = subobj_tex_[kIceBoltBox01];
            std::memcpy(item.uv_rect, subobj_uv_[kIceBoltBox01],
                        sizeof(item.uv_rect));
            Renderer->SubmitFxBillboard(item);
        }
    }
}

// --- I22 TIcedEffect bespoke -------------------------------------------
//
// FAITHFUL DIRECT PORT of TIcedAnimator (snapshot effect_old.cpp:
// 8749-8953). 3-phase state machine: HOLD (240 ticks reveal facets) →
// SHATTER (seed 30 chunks) → TUMBLE/BOUNCE (gravity + bounce death).
// Posted by I21 (and retail-only TStormAnimator); harness runs it
// standalone — no victim coupling (icedchar = nullptr; SetParalize,
// IsIced, IsDead, Stop branches are guarded out for sector-less preview;
// lifecycle ticks purely on frameon).
//
// Engine adaptations:
//   - Render path swaps RenderObject(obj) for SubmitFxBillboard. Phase A
//     (HOLD): draws min(frameon*4, 50) face billboards stacked at the
//     origin (snapshot transforms cycle facets via D3DMATRIXRotateZ).
//     Phase B+C (SHATTER+TUMBLE): one billboard per live chunk at p[i]
//     world position.
//   - 24 Hz sim-tick gate (family pattern). Per-tick math (gravity,
//     bounce, rotation) runs once per accumulated 1/24 s.
//   - icedchar lifecycle paths (SetParalize/Stop/IsDead) guarded out;
//     lifecycle is "elapsed frameon" only.

namespace {

constexpr const char* kIcedImageryPath = "Magic\\iced.I3D";

// Snapshot constants. Variable names follow effect_old.cpp:8715-8721.
constexpr float kIcedBouncePlane    = 16.0f;   // p.z <= 16 (snapshot :8876)
constexpr float kIcedBounceLift     = 20.0f;   // p.z = 20 (:8878)
constexpr float kIcedBounceVelMul   = -0.5f;   // v.z *= -0.5 (:8879)
constexpr float kIcedBounceSpinMul  = 2.0f;    // w *= 2 (:8880-8882)
constexpr int32_t kIcedRowsPerSide  = 5;       // chunk z-row spread (:8819: i/6 implies ~5 rows)
constexpr float kIcedRowSpacingZ    = 25.0f;   // :8819 row spacing 25 wu
constexpr float kIcedFacetSizeWu    = 24.0f;   // single-facet billboard size

}   // namespace

TIcedEffect_Bespoke* TIcedEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // --- Load Magic\iced.I3D (forensics §4 — byte-identical asset
    // confirmed MD5 10e7e15b3cd6b491e47ab1743b5cdf8e snapshot vs ship).
    const int32_t img_id = TObjectImagery::FindImagery(kIcedImageryPath);
    if (img_id < 0)
    {
        log_error("[iced] SpawnForTest: FindImagery('%s') failed",
                  kIcedImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[iced] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  img_id, kIcedImageryPath);
        return nullptr;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[iced] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  kIcedImageryPath);
        TObjectImagery::FreeImagery(base);
        return nullptr;
    }

    auto* iced = new TIcedEffect_Bespoke(base);
    iced->ForcePos(origin);
    iced->SetMapIndex(MapPane.MakeIndex());
    iced->ActivateComponents();

    // Lazy-mesh-init poke.
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj <= 0 || num_tex <= 0)
    {
        log_error("[iced] SpawnForTest: imagery underspec'd "
                  "(objects=%d, textures=%d)", num_obj, num_tex);
        delete iced;
        return nullptr;
    }

    // Resolve sub-object 0 (the cube chunk mesh) texture for billboards.
    // Forensics §4: iced.I3D has face01..43 reveal facets + icicle01..06
    // + a cube chunk mesh at sub-obj 0 used for the shatter phase.
    const int32_t tex_slot = SubObjTextureSlot(img3d, 0);
    const int32_t slot = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
    S3DTex tex = {};
    img3d->GetTexture(slot, &tex);
    iced->facet_tex_ = tex.htexture;
    ResolveSubObjUv(img3d, 0, iced->facet_uv_);
    iced->facet_size_wu_ = kIcedFacetSizeWu;

    if (iced->facet_tex_ == kInvalidTexture)
    {
        log_error("[iced] SpawnForTest: sub-obj 0 texture unresolved — "
                  "facet/chunk billboards will draw nothing");
    }

    // Port of TIcedAnimator::Initialize (snapshot :8749-8756).
    iced->frameon_      = 0;                                    // :8752
    iced->angle_        = float(random(0, 359))
                            * float(TORADIAN);                  // :8753
    // icedchar_ stays nullptr (harness has no victim) — :8754
    iced->donebouncing_ = 0;                                    // :8755

    log_info("[iced] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "angle=%.3f facet_tex=%u num_obj=%d",
             kIcedImageryPath, iced->GetMapIndex(),
             origin.x, origin.y, origin.z,
             iced->angle_, iced->facet_tex_, num_obj);
    return iced;
}

void TIcedEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TIcedAnimator::Animate (effect_old.cpp:
    // 8777-8891), framerate-independent via 24 Hz sim-tick gate.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kIcedSimTickMs))
        {
            sim_accum_ms_ -= double(kIcedSimTickMs);

            frameon_++;   // :8783

            // Victim-coupling lifecycle (:8785-8797): all branches are
            // guarded out for the harness — no icedchar, no Stop(), no
            // IsIced/IsDead. Lifecycle is "elapsed frameon" only.

            // Phase B: SHATTER seed (snapshot :8798-8847, frameon==DURATION).
            if (frameon_ == kIcedDuration)
            {
                // icedchar SetParalize(false) (:8800-8804) guarded out.
                // Snapshot :8807-8846 — seed MAX_ICED_CHUNKS in 4
                // quadrants × 5 rows pattern. Same RNG sequence.
                int32_t a = 0, b = 0;
                for (int32_t i = 0; i < MAX_ICED_CHUNKS; ++i)
                {
                    const int32_t c = (a ? 1 : -1);              // :8810
                    const int32_t d = (b ? 1 : -1);              // :8811
                    SIcedChunk& ch = chunks_[i];

                    // velocity (:8813-8815)
                    ch.vel.X = float(c * random(5, 10) / 10.0
                                     + random(-5, 5) / 10.0);
                    ch.vel.Y = float(d * random(5, 10) / 10.0
                                     + random(-5, 5) / 10.0);
                    ch.vel.Z = float(random(0, 10) / 10.0);
                    // position (:8817-8819)
                    ch.pos.X = float(c * random(20, 40) + random(-20, 20));
                    ch.pos.Y = float(d * random(20, 40) + random(-20, 20));
                    ch.pos.Z = float(int32_t(i / 6) * 25);
                    // angular velocity (:8821)
                    ch.ang.X = ch.ang.Y = float(random(0, 25) / 100.0);
                    // angular position (:8826: t.z = atan2(v.y, v.x))
                    ch.rot.Z = std::atan2(ch.vel.Y, ch.vel.X);
                    // scale (:8828-8839)
                    if (random(0, 1))
                    {
                        ch.scl.X = float(random(25, 75) / 100.0);
                        ch.scl.Y = float(random(25, 75) / 100.0);
                        ch.scl.Z = float(random(25, 75) / 100.0);
                    }
                    else
                    {
                        ch.scl.X = float(random(10, 15) / 100.0);
                        ch.scl.Y = float(random(10, 15) / 100.0);
                        ch.scl.Z = float(random(10, 15) / 100.0);
                    }
                    // life (bounces left, :8841)
                    ch.life = random(2, 3);
                    // separator (:8843-8845)
                    a = 1 - a;
                    if (a)
                        b = 1 - b;
                }
            }

            // Phase C: TUMBLE+BOUNCE (snapshot :8848-8886, frameon>DURATION).
            if (frameon_ > kIcedDuration)
            {
                donebouncing_ = 1;
                for (int32_t i = 0; i < MAX_ICED_CHUNKS; ++i)
                {
                    SIcedChunk& ch = chunks_[i];
                    if (ch.life == 0)
                        continue;
                    donebouncing_ = 0;
                    ch.pos.X += ch.vel.X;                        // :8856
                    ch.pos.Y += ch.vel.Y;                        // :8857
                    ch.pos.Z += ch.vel.Z;                        // :8858
                    ch.vel.Z -= kIcedChunkGravity;               // :8859
                    ch.rot.X += ch.ang.X;                        // :8860
                    ch.rot.Y += ch.ang.Y;                        // :8861
                    ch.rot.Z += ch.ang.Z;                        // :8862
                    // wrap angles 0..2π (:8863-8874)
                    auto wrap = [](float& r) {
                        if (r > float(M_2PI)) r -= float(M_2PI);
                        if (r < 0.0f)         r += float(M_2PI);
                    };
                    wrap(ch.rot.X);
                    wrap(ch.rot.Y);
                    wrap(ch.rot.Z);
                    // bounce (:8876-8884)
                    if (ch.pos.Z <= kIcedBouncePlane)
                    {
                        ch.pos.Z = kIcedBounceLift;
                        ch.vel.Z *= kIcedBounceVelMul;
                        ch.ang.X *= kIcedBounceSpinMul;
                        ch.ang.Y *= kIcedBounceSpinMul;
                        ch.ang.Z *= kIcedBounceSpinMul;
                        ch.life--;
                    }
                }
            }

            // Self-destruct (snapshot :8887-8890).
            if (donebouncing_)
            {
                alive_ = false;
                break;
            }
        }
    }

    if (!alive_)
        return;

    // --- Render: port of TIcedAnimator::Render (effect_old.cpp:
    // 8900-8953). Blend = Alpha, Lit-mode = Unlit, Depth = TestNoWrite.
    // Phase A (HOLD): min(frameon*4, 50) facet billboards at origin.
    // Phase B+C: one billboard per live chunk at world pos.
    if (facet_tex_ == kInvalidTexture)
        return;

    const S3DPoint& base_pos = Pos();

    SBillboardDrawItem item = {};
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    item.key.texture     = facet_tex_;
    std::memcpy(item.uv_rect, facet_uv_, sizeof(item.uv_rect));

    if (frameon_ < kIcedDuration)
    {
        // Phase A: HOLD. Snapshot :8907-8920 — for i in 1..min(frameon*4, 50)
        // RenderObject(GetObject(i)) with RotateZ(angle). All facets share
        // origin transform; with billboard quads, they all stack visually.
        // `min`/`max` are macros from revtypes.h:20-21; expand inline so
        // std::min parses.
        const int32_t reveal_raw = frameon_ * 4;
        const int32_t reveal_count =
            reveal_raw < kIcedRevealCap ? reveal_raw : kIcedRevealCap;
        const float ring_scale = 1.0f;
        for (int32_t i = 1; i < reveal_count; ++i)
        {
            item.world_pos[0] = float(base_pos.x);
            item.world_pos[1] = float(base_pos.y);
            item.world_pos[2] = float(base_pos.z);
            item.size_wu[0] = item.size_wu[1] =
                facet_size_wu_ * ring_scale;
            Renderer->SubmitFxBillboard(item);
        }
    }
    else
    {
        // Phase B+C: SHATTER chunks. Snapshot :8923-8948 — one
        // RenderObject(GetObject(0)) per live chunk at p[i] world pos.
        for (int32_t i = 0; i < MAX_ICED_CHUNKS; ++i)
        {
            const SIcedChunk& ch = chunks_[i];
            if (ch.life == 0)
                continue;
            item.world_pos[0] = float(base_pos.x) + ch.pos.X;
            item.world_pos[1] = float(base_pos.y) + ch.pos.Y;
            item.world_pos[2] = float(base_pos.z) + ch.pos.Z;
            item.size_wu[0] = item.size_wu[1] =
                facet_size_wu_ * (ch.scl.X + ch.scl.Y) * 0.5f;
            Renderer->SubmitFxBillboard(item);
        }
    }
}
// --- end ice bespoke first-pass

// *************************************************************************
// * Magic family bespoke first-pass ports (wave-bespoke-04-aura-magic)     *
// *************************************************************************
//
// Faithful direct ports of the snapshot animator bodies for M01/M03/M05/X09.
// Translation rules (per task brief): per-tick math/variable names/constants
// preserved from the snapshot; blend mode preserved AS WRITTEN
// (SetAddBlendState -> AdditiveStraight; SetBlendState -> Alpha); 24Hz
// sim-tick gate for framerate-independent motion (same pattern as B01).
// First-pass renders mesh-based draws as billboards positioned at the
// original sub-object pos/scl — verifies compile + boot + something draws.

namespace {

// ---- M01 / M03 / X09 share the lazy-load helper pattern ---------------------
// Tries a list of candidate imagery paths in order (the snapshot animator
// classes don't specify a canonical I3D — `Magic\\aura.i3d`, `Magic\\heal*.i3d`,
// `Magic\\shield.i3d` are guesses pending forensics). Falls through to a
// kInvalidTexture handle if nothing loads; the harness will still draw
// nothing visibly but the effect compiles + boots.
TTextureHandle TryLoadMagicTexture(const char* const* candidate_paths,
                                   int32_t num_candidates,
                                   float out_uv_rect[4],
                                   const char* tag)
{
    for (int32_t i = 0; i < num_candidates; ++i)
    {
        const int32_t img_id = TObjectImagery::FindImagery(candidate_paths[i]);
        if (img_id < 0)
            continue;
        TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
        if (!base)
            continue;
        T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
        if (!img3d)
        {
            TObjectImagery::FreeImagery(base);
            continue;
        }
        // Lazy-mesh-init poke (mirror F01/B01/H04/X22): NumObjects() triggers
        // the actual texture-slot population; NumTextures() alone doesn't.
        (void)img3d->NumObjects();
        const int32_t num_tex = img3d->NumTextures();
        if (num_tex <= 0)
            continue;
        S3DTex tex = {};
        img3d->GetTexture(0, &tex);
        if (tex.htexture == kInvalidTexture)
            continue;
        out_uv_rect[0] = 0.0f;
        out_uv_rect[1] = 0.0f;
        out_uv_rect[2] = 1.0f;
        out_uv_rect[3] = 1.0f;
        log_info("[%s] resolved imagery='%s' tex=%u", tag,
                 candidate_paths[i], tex.htexture);
        return tex.htexture;
    }
    log_warn("[%s] no candidate imagery resolved (tried %d paths); will draw "
             "untextured/invisible billboards (first-pass blocker note)",
             tag, num_candidates);
    return kInvalidTexture;
}

}   // namespace

// =========================================================================
// M01 — TAuraEffect_Bespoke
// =========================================================================
//
// Faithful direct port of TAuraAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:3535-3667). The snapshot uses a TParticleSystem
// emitting AURA_COUNT=100 particles randomly distributed across the
// character's animator sub-objects. The harness has no live character,
// so we collapse the per-frame emission onto a single emit-radius around
// the effect's pos (preserving the random spread + Z-range + life span).
// Per-tick scale decay (scl *= AURA_DEC=0.97) is preserved verbatim.
// Render: AdditiveStraight (was SetAddBlendState).

TAuraEffect_Bespoke* TAuraEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // Imagery guess list (no forensics doc yet — aura* candidates).
    static const char* kAuraCandidates[] = {
        "Magic\\aura.i3d",
        "Magic\\Aura.I3D",
        "Magic\\flare.i3d",      // TFlareAnimator uses a similar emit pattern
    };

    // Spawn with no imagery (we resolve textures separately via TryLoad).
    auto* aura = new TAuraEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    aura->ForcePos(origin);
    aura->SetMapIndex(MapPane.MakeIndex());
    aura->ActivateComponents();

    aura->texture_ = TryLoadMagicTexture(kAuraCandidates,
                                         int32_t(sizeof(kAuraCandidates) / sizeof(*kAuraCandidates)),
                                         aura->uv_rect_, "aura");

    aura->frame_  = 0;
    aura->to_add_ = 0;
    aura->alive_  = true;

    log_info("[aura] SpawnForTest_BESPOKE: map_index=%d origin=(%d,%d,%d) tex=%u",
             aura->GetMapIndex(), origin.x, origin.y, origin.z, aura->texture_);
    return aura;
}

void TAuraEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // 24Hz sim-tick gate for framerate-independent integration.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kAuraSimTickMs))
        {
            sim_accum_ms_ -= double(kAuraSimTickMs);

            // Snapshot effect_old.cpp:3571-3575 — `to_add` ramps up/down
            // bracketing the AURA_FRAME window.
            ++frame_;
            if (frame_ < kAuraFrameCap && to_add_ < kAuraAdd)
                ++to_add_;
            else if (frame_ >= kAuraFrameCap && to_add_ > 0)
                --to_add_;

            // Snapshot effect_old.cpp:3587-3636 — emit `to_add` new
            // particles per tick. Without a character animator we emit
            // around the effect's local origin (the snapshot picks a
            // random sub-object and offsets by ±AURA_SPREAD).
            for (int32_t i = 0; i < to_add_; ++i)
            {
                // Find a free slot
                int32_t slot = -1;
                for (int32_t k = 0; k < kAuraCount; ++k)
                {
                    if (!particles_[k].used)
                    {
                        slot = k;
                        break;
                    }
                }
                if (slot < 0)
                    break;

                SAuraParticle& p = particles_[slot];
                p.used = true;
                p.pos.X = float(random(-kAuraSpread, kAuraSpread));
                p.pos.Y = float(random(-kAuraSpread, kAuraSpread));
                p.pos.Z = float(random(-kAuraSpread, kAuraSpread));

                const float scale = float(random(kAuraMinScl, kAuraMaxScl)) * 0.01f;
                p.scl.X = p.scl.Y = p.scl.Z = scale;

                // snapshot acc is unused for these single-tick particles;
                // velocity is only the Z component (ascending aura motes).
                p.vel.X = 0.0f;
                p.vel.Y = 0.0f;
                p.vel.Z = float(random(kAuraMinZ, kAuraMaxZ)) * 0.1f;

                p.life_span = random(kAuraMinLife, kAuraMaxLife);
            }

            // Snapshot effect_old.cpp:3641-3651 — per-particle scale decay
            // each tick. Combined with the inline integration TParticleSystem
            // does (pos += vel; life_span--; kill when life_span <= 0).
            bool done = true;
            for (int32_t i = 0; i < kAuraCount; ++i)
            {
                SAuraParticle& p = particles_[i];
                if (!p.used)
                    continue;
                done = false;
                p.pos.X += p.vel.X;
                p.pos.Y += p.vel.Y;
                p.pos.Z += p.vel.Z;
                p.scl.X *= kAuraDec;
                p.scl.Y *= kAuraDec;
                p.scl.Z *= kAuraDec;
                --p.life_span;
                if (p.life_span <= 0)
                    p.used = false;
            }

            // Snapshot effect_old.cpp:3653-3654 — self-destruct when emit
            // window closes and all particles have died.
            if (frame_ >= kAuraFrameCap && to_add_ == 0 && done)
            {
                alive_ = false;
                break;
            }
        }
    }

    // --- Render (port of TAuraAnimator::Render, effect_old.cpp:3657-3667).
    // SetAddBlendState -> AdditiveStraight, preserved AS WRITTEN.
    if (texture_ == kInvalidTexture)
        return;

    const S3DPoint& base = Pos();
    for (int32_t i = 0; i < kAuraCount; ++i)
    {
        const SAuraParticle& p = particles_[i];
        if (!p.used)
            continue;

        SBillboardDrawItem item = {};
        // size_wu scaled by per-particle scl (snapshot ranges 0.25..0.6).
        const float size = 32.0f * p.scl.X;
        item.size_wu[0] = size;
        item.size_wu[1] = size;
        // Aura tint — warm gold (placeholder until forensics confirms).
        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 0.85f;
        item.color_rgba[2] = 0.40f;
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = uv_rect_[0];
        item.uv_rect[1] = uv_rect_[1];
        item.uv_rect[2] = uv_rect_[2];
        item.uv_rect[3] = uv_rect_[3];
        item.key.texture     = texture_;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = debug_mode;
        item.world_pos[0] = float(base.x) + p.pos.X;
        item.world_pos[1] = float(base.y) + p.pos.Y;
        item.world_pos[2] = float(base.z) + p.pos.Z;
        Renderer->SubmitFxBillboard(item);
    }
}

// =========================================================================
// M03 — THealEffect_Bespoke
// =========================================================================
//
// Faithful direct port of THealAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:628-833). NUM_HEAL_BUBBLES=60 bubbles + a
// cylindrical glow at the feet. HEAL_DURATION=40 ticks total: glow
// grows for HEAL_DURATION/2, then shrinks once activebubbles==0.
// Per-bubble: random pos in ±HEALING_RADIUS=20, rises with `rise` at
// random(3,6)*4/3 wu/tick, scale shrinks by HEAL_SCALE_STEP=0.15
// until invisible; respawns until framenum[0] > HEAL_DURATION.
// SetBlendState -> Alpha (preserved AS WRITTEN).

THealEffect_Bespoke* THealEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    static const char* kHealCandidates[] = {
        "Magic\\heal.i3d",
        "Magic\\Heal.I3D",
        "Magic\\heal1.i3d",
    };

    auto* heal = new THealEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    heal->ForcePos(origin);
    heal->SetMapIndex(MapPane.MakeIndex());
    heal->ActivateComponents();

    heal->bubble_tex_ = TryLoadMagicTexture(kHealCandidates,
                                            int32_t(sizeof(kHealCandidates) / sizeof(*kHealCandidates)),
                                            heal->bubble_uv_, "heal-bubble");
    heal->glow_tex_   = heal->bubble_tex_;
    heal->glow_uv_[0] = heal->bubble_uv_[0];
    heal->glow_uv_[1] = heal->bubble_uv_[1];
    heal->glow_uv_[2] = heal->bubble_uv_[2];
    heal->glow_uv_[3] = heal->bubble_uv_[3];

    // Snapshot effect_old.cpp:628-685 — Initialize. heal_num/glow_num
    // depend on spell level; default branch is heal_num=0, glow_num=1.
    heal->heal_num_ = 0;
    heal->glow_num_ = 1;

    // Glow at the player's feet (index 0).
    heal->p_[0]        = {0.0f, 0.0f, 0.0f};
    heal->scale_[0]    = 2.0f;
    heal->framenum_[0] = 0;
    heal->rotation_    = 0.0f;

    for (int32_t n = 1; n < kHealBubbles; ++n)
    {
        heal->p_[n].X = float(random(-kHealingRadius, kHealingRadius));
        heal->p_[n].Y = float(random(-kHealingRadius, kHealingRadius));
        heal->p_[n].Z = 2.0f;
        heal->rise_[n] = float(random(3, 6) * 4.0f / 3.0f);
        // snapshot assigns scale[n] = 0 then immediately scale[n] = 4.5 —
        // preserve the second assignment (the live value); the leading 0
        // assignment is dead per snapshot effect_old.cpp:682-683.
        heal->scale_[n] = 4.5f;
        heal->framenum_[n] = random(-kHealBubbles / 2, -1);
    }
    heal->activebubbles_ = 0;
    heal->alive_ = true;

    log_info("[heal] SpawnForTest_BESPOKE: map_index=%d origin=(%d,%d,%d) "
             "bubble_tex=%u glow_tex=%u",
             heal->GetMapIndex(), origin.x, origin.y, origin.z,
             heal->bubble_tex_, heal->glow_tex_);
    return heal;
}

void THealEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate port (effect_old.cpp:695-760), 24Hz sim-tick gated.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kHealSimTickMs))
        {
            sim_accum_ms_ -= double(kHealSimTickMs);

            // Snapshot :701-719 — glow framenum/scale lifecycle.
            ++framenum_[0];
            if (framenum_[0] < kHealDuration / 2)
            {
                if (scale_[0] < 7.0f)
                    scale_[0] += 1.0f;
            }
            else if (framenum_[0] > kHealDuration && activebubbles_ == 0)
            {
                if (scale_[0] > 0.0f)
                    scale_[0] -= 1.0f;
                else
                {
                    alive_ = false;
                    break;
                }
            }

            // Snapshot :722 — rotation += 0.1f.
            rotation_ += 0.1f;

            // Snapshot :725-759 — per-bubble integration.
            for (int32_t n = 1; n < kHealBubbles; ++n)
            {
                ++framenum_[n];
                if (framenum_[n] == 0)
                    ++activebubbles_;

                if (framenum_[n] > 0)
                {
                    p_[n].Z += rise_[n];

                    if (scale_[n] > 0.0f)
                    {
                        scale_[n] -= kHealScaleStep;
                        if (scale_[n] <= 0.0f)
                        {
                            --activebubbles_;

                            if (framenum_[0] < kHealDuration)
                            {
                                // Snapshot :749-755 — respawn bubble.
                                p_[n].X = float(random(-kHealingRadius, kHealingRadius));
                                p_[n].Y = float(random(-kHealingRadius, kHealingRadius));
                                p_[n].Z = 2.0f;
                                rise_[n] = float(random(3, 6) * 4.0f / 3.0f);
                                scale_[n] = 4.5f;
                                framenum_[n] = -1;
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Render port (effect_old.cpp:770-832). SetBlendState -> Alpha.
    if (bubble_tex_ == kInvalidTexture)
        return;

    const S3DPoint& base = Pos();

    // Snapshot :777-811 — per-visible-bubble billboard draws.
    for (int32_t n = 1; n < kHealBubbles; ++n)
    {
        if (framenum_[n] <= 0 || scale_[n] <= 0.0f)
            continue;

        // Bubble billboard.
        SBillboardDrawItem item = {};
        const float size = 8.0f * scale_[n];
        item.size_wu[0] = size;
        item.size_wu[1] = size;
        // Soft green-blue heal tint (placeholder until forensics).
        item.color_rgba[0] = 0.5f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 0.7f;
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = bubble_uv_[0];
        item.uv_rect[1] = bubble_uv_[1];
        item.uv_rect[2] = bubble_uv_[2];
        item.uv_rect[3] = bubble_uv_[3];
        item.key.texture     = bubble_tex_;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(EFxBlend::Alpha);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = debug_mode;
        item.world_pos[0] = float(base.x) + p_[n].X;
        item.world_pos[1] = float(base.y) + p_[n].Y;
        item.world_pos[2] = float(base.z) + p_[n].Z;
        Renderer->SubmitFxBillboard(item);
    }

    // Snapshot :813-826 — cylindrical glow at the player's feet, drawn
    // TWICE (forward + reverse rotation), preserving the two-pass shape.
    if (glow_tex_ != kInvalidTexture && scale_[0] > 0.0f)
    {
        SBillboardDrawItem glow = {};
        const float size = 24.0f * scale_[0] * 0.75f;   // snapshot scl * 3/4
        glow.size_wu[0] = size;
        glow.size_wu[1] = size;
        glow.color_rgba[0] = 0.6f;
        glow.color_rgba[1] = 1.0f;
        glow.color_rgba[2] = 0.7f;
        glow.color_rgba[3] = 1.0f;
        glow.uv_rect[0] = glow_uv_[0];
        glow.uv_rect[1] = glow_uv_[1];
        glow.uv_rect[2] = glow_uv_[2];
        glow.uv_rect[3] = glow_uv_[3];
        glow.key.texture     = glow_tex_;
        glow.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        glow.key.blend       = uint8_t(EFxBlend::Alpha);
        glow.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        glow.light_mode      = EFxLightMode::Unlit;
        glow.orientation     = EFxBillboardOrientation::WorldXY;   // ground-plane cylinder
        glow.debug_mode      = debug_mode;
        glow.world_pos[0] = float(base.x) + p_[0].X;
        glow.world_pos[1] = float(base.y) + p_[0].Y;
        glow.world_pos[2] = float(base.z) + p_[0].Z;
        // Pass 1: forward rotation (snapshot obj->rot.z = rotation).
        Renderer->SubmitFxBillboard(glow);
        // Pass 2: reverse rotation (snapshot obj->rot.z = -rotation +
        // RenderObject(obj) again). Preserves the two-pass shape; in our
        // billboard path this duplicates the draw so the overlapping
        // additive contributions stack as in the snapshot.
        Renderer->SubmitFxBillboard(glow);
    }
}

// =========================================================================
// M05 — TMistEffect_Bespoke
// =========================================================================
//
// Faithful direct port of TMistAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:11376-11502). The engine TMistEffect path lives at
// src/effect.cpp:3241 (engine-driven via TParticleEffectComponent +
// effects.def); this bespoke class is the BESPOKE canonical per the
// wave-04 pivot. Both coexist; the harness picks one via the test ID.
//
// 50 drops with retail pos/vel envelope (MIST_LENGTH=64 etc.); ascends,
// falls under gravity (RIPPLE_GRAVITY=0.37 wu/tick²), respawns-in-place
// on landing (pos.z <= 0). SetAddBlendState -> AdditiveStraight.
// One sub-object (snapshot GetObject(0)) renders all 50 billboards.

TMistEffect_Bespoke* TMistEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    static const char* kMistCandidates[] = {
        "Magic\\mist.i3d",
        "Misc\\mist.i3d",        // byte-identical duplicate per M05 forensics §4
    };

    auto* mist = new TMistEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    mist->ForcePos(origin);
    mist->SetMapIndex(MapPane.MakeIndex());
    mist->ActivateComponents();

    mist->texture_ = TryLoadMagicTexture(kMistCandidates,
                                         int32_t(sizeof(kMistCandidates) / sizeof(*kMistCandidates)),
                                         mist->uv_rect_, "mist");

    // Snapshot effect_old.cpp:11383-11392 — seed all 50 drops up-front.
    for (int32_t i = 0; i < kMistMaxDrops; ++i)
    {
        SMistDrop& d = mist->drops_[i];
        d.vel.X = float(random(-kMistLength, kMistLength) / 32.0f);
        d.vel.Y = float(random(-kMistWidth,  kMistWidth)  / 32.0f);
        d.vel.Z = float(random(kMistHeight / 2, kMistHeight));
        d.pos.X = float(random(-kMistLength, kMistLength) / 2.0f);
        d.pos.Y = float(random(-kMistWidth,  kMistWidth)  / 2.0f);
        d.pos.Z = kMistSpawnZ;
        d.dead = false;
    }
    mist->alive_ = true;

    log_info("[mist-bespoke] SpawnForTest_BESPOKE: map_index=%d origin=(%d,%d,%d) "
             "tex=%u drops=%d",
             mist->GetMapIndex(), origin.x, origin.y, origin.z,
             mist->texture_, kMistMaxDrops);
    return mist;
}

void TMistEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // 24Hz sim-tick gate (snapshot was ungated; at modern 60fps drops fly
    // 2.5x too fast — same gate fix as B01/H03/M05-engine/H04/L02).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kMistSimTickMs))
    {
        sim_accum_ms_ -= double(kMistSimTickMs);

        // Snapshot effect_old.cpp:11432-11453 — per-drop Animate. dead
        // drops re-roll pos+vel; live drops integrate Euler with gravity;
        // pos.z <= 0 -> dead.
        for (int32_t i = 0; i < kMistMaxDrops; ++i)
        {
            SMistDrop& d = drops_[i];
            if (d.dead)
            {
                d.vel.X = float(random(-kMistLength, kMistLength) / 32.0f);
                d.vel.Y = float(random(-kMistWidth,  kMistWidth)  / 32.0f);
                d.vel.Z = float(random(kMistHeight / 2, kMistHeight));
                d.pos.X = float(random(-kMistLength, kMistLength) / 2.0f);
                d.pos.Y = float(random(-kMistWidth,  kMistWidth)  / 2.0f);
                d.pos.Z = kMistSpawnZ;
                d.dead = false;
            }
            d.pos.X += d.vel.X;
            d.pos.Y += d.vel.Y;
            d.pos.Z += d.vel.Z;
            d.vel.Z -= kMistGravity;
            if (d.pos.Z <= 0.0f)
            {
                d.dead = true;
                // Snapshot AddNewRipple call is commented-out per M05 §7.6.
            }
        }
    }

    // --- Render port (effect_old.cpp:11463-11502). SetAddBlendState ->
    // AdditiveStraight. Single sub-object drives all 50 drops.
    if (texture_ == kInvalidTexture)
        return;

    const S3DPoint& base = Pos();
    SBillboardDrawItem item = {};
    item.size_wu[0] = kMistBaseSizeWu * kMistScale;   // snapshot scl = 0.7
    item.size_wu[1] = kMistBaseSizeWu * kMistScale;
    // Cool-white wisp tint (matches engine TMistEffect's color choice for
    // visual continuity between bespoke + engine paths).
    item.color_rgba[0] = 0.16f;
    item.color_rgba[1] = 0.18f;
    item.color_rgba[2] = 0.22f;
    item.color_rgba[3] = 1.0f;
    item.uv_rect[0] = uv_rect_[0];
    item.uv_rect[1] = uv_rect_[1];
    item.uv_rect[2] = uv_rect_[2];
    item.uv_rect[3] = uv_rect_[3];
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;

    for (int32_t i = 0; i < kMistMaxDrops; ++i)
    {
        const SMistDrop& d = drops_[i];
        if (d.dead)
            continue;
        item.world_pos[0] = float(base.x) + d.pos.X;
        item.world_pos[1] = float(base.y) + d.pos.Y;
        item.world_pos[2] = float(base.z) + d.pos.Z;
        Renderer->SubmitFxBillboard(item);
    }
}

// =========================================================================
// X09 — TShieldEffect_Bespoke
// =========================================================================
//
// Faithful direct port of TShieldAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:4336-4392). Animator-only in pre-release; bridges
// via an existing TEffect on the caster object. Animator is the simplest
// in this batch: Animate is a no-op; Render draws ONE mesh sub-object at
// pos.z=40 with SHIELD_SCALE=2.0 and a fixed -π/3 X-rotation + -π/4
// Z-rotation. SetBlendState -> Alpha (preserved AS WRITTEN).
//
// First-pass: single Alpha billboard at the shield position; real mesh
// (sphere/ellipsoid enveloping the victim) is a follow-up upgrade.

TShieldEffect_Bespoke* TShieldEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    static const char* kShieldCandidates[] = {
        "Magic\\shield.i3d",
        "Magic\\Shield.I3D",
        "Magic\\shield1.i3d",
    };

    auto* shield = new TShieldEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    shield->ForcePos(origin);
    shield->SetMapIndex(MapPane.MakeIndex());
    shield->ActivateComponents();

    shield->texture_ = TryLoadMagicTexture(kShieldCandidates,
                                           int32_t(sizeof(kShieldCandidates) / sizeof(*kShieldCandidates)),
                                           shield->uv_rect_, "shield");

    // Snapshot effect_old.cpp:4336-4342 — Initialize sets pos = (0,0,40).
    shield->pos_.X = 0.0f;
    shield->pos_.Y = 0.0f;
    shield->pos_.Z = kShieldLiftZ;
    shield->framenum_ = 0;
    shield->alive_ = true;

    log_info("[shield] SpawnForTest_BESPOKE: map_index=%d origin=(%d,%d,%d) "
             "tex=%u",
             shield->GetMapIndex(), origin.x, origin.y, origin.z, shield->texture_);
    return shield;
}

void TShieldEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate port (effect_old.cpp:4351-4355): empty (base call only,
    // a no-op). 24Hz sim-tick gate runs an empty body — kept here for
    // symmetry with the other bespoke effects.
    if (alive_)
    {
        sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
        while (sim_accum_ms_ >= double(kShieldSimTickMs))
        {
            sim_accum_ms_ -= double(kShieldSimTickMs);
            ++framenum_;
            // Snapshot Animate is empty — no state change.
        }
    }

    // --- Render port (effect_old.cpp:4364-4391). SetBlendState -> Alpha.
    // Snapshot binds GetObject(0) and applies:
    //   scl = SHIELD_SCALE=2.0
    //   pos = (0,0,40)
    //   RotateX(-π/3), RotateZ(-π/4)
    // Then RenderObject(obj). One mesh draw per frame.
    if (texture_ == kInvalidTexture)
        return;

    SBillboardDrawItem item = {};
    item.size_wu[0] = kShieldSizeWu * kShieldScale;
    item.size_wu[1] = kShieldSizeWu * kShieldScale;
    // Pale blue shield tint.
    item.color_rgba[0] = 0.6f;
    item.color_rgba[1] = 0.85f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 0.8f;
    item.uv_rect[0] = uv_rect_[0];
    item.uv_rect[1] = uv_rect_[1];
    item.uv_rect[2] = uv_rect_[2];
    item.uv_rect[3] = uv_rect_[3];
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    const S3DPoint& base = Pos();
    item.world_pos[0] = float(base.x) + pos_.X;
    item.world_pos[1] = float(base.y) + pos_.Y;
    item.world_pos[2] = float(base.z) + pos_.Z;
    Renderer->SubmitFxBillboard(item);
}

// *************************************************************************
// * wave-bespoke-05: sparkle/glow family bespoke ports                     *
// *************************************************************************
//
// X03 TFlareEffect_Bespoke / X10 TSymGlowEffect_Bespoke / M07
// TPhotonEffect_Bespoke / M08 TPixieEffect_Bespoke — faithful direct
// ports of the snapshot animators (effect_old.cpp + missileeffect.cpp)
// driving the --test=vfx harness for the FB+LS / PE+LS / IM+LS
// pipelines. See class doc-comments in effect.h for per-effect notes.

namespace {

// Asset paths from Class.Def (registered names → I3D paths). The harness
// loads via TObjectImagery::FindImagery + LoadImagery so paths are
// matched against the canonical registry name.
constexpr const char* kFlareBespokeImageryPath   = "Misc\\IrisFlare.I3D";
constexpr const char* kSymGlowBespokeImageryPath = "Misc\\SymGlow.I3D";
constexpr const char* kPhotonBespokeImageryPath  = "Magic\\Photon.I3D";
constexpr const char* kPixieBespokeImageryPath   = "misc\\Pixies.i3d";

// Local helpers — mirrors SubObjTextureSlot / ResolveSubObjUv at
// effect.cpp:1484 / :1507 (anonymous-namespace scoped there so we
// re-declare here for the sparkle/glow batch).
int32_t SparkleSubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0)
        return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

void SparkleResolveSubObjUv(T3DImagery* img3d, int32_t objnum, float out[4])
{
    out[0] = 0.0f; out[1] = 0.0f; out[2] = 1.0f; out[3] = 1.0f;
    if (!img3d) return;
    const int32_t nverts = img3d->NumObjVerts(objnum);
    if (nverts <= 0) return;
    std::vector<S3DVertex> vbuf(static_cast<size_t>(nverts), S3DVertex{});
    img3d->GetObjVerts(objnum, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
    float minu = vbuf[0].tu, maxu = vbuf[0].tu;
    float minv = vbuf[0].tv, maxv = vbuf[0].tv;
    for (int32_t i = 1; i < nverts; ++i)
    {
        if (vbuf[i].tu < minu) minu = vbuf[i].tu;
        if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
        if (vbuf[i].tv < minv) minv = vbuf[i].tv;
        if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
    }
    out[0] = minu;
    out[1] = minv;
    out[2] = maxu - minu;
    out[3] = maxv - minv;
}

// Load + sanity-check an imagery path. Returns a (TObjectImagery*,
// T3DImagery*) pair or {nullptr, nullptr} on failure. Caller owns the
// returned imagery (delete via TObjectImagery::FreeImagery only if not
// passed into a TEffect — once the TEffect takes it the effect owns it).
struct SLoadedImagery
{
    TObjectImagery* base  = nullptr;
    T3DImagery*     img3d = nullptr;
};

SLoadedImagery SparkleLoadImagery(const char* path, const char* tag)
{
    SLoadedImagery out;
    const int32_t img_id = TObjectImagery::FindImagery(path);
    if (img_id < 0)
    {
        log_error("[%s] SpawnForTest: FindImagery('%s') failed", tag, path);
        return out;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[%s] SpawnForTest: LoadImagery(id=%d '%s') failed",
                  tag, img_id, path);
        return out;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[%s] SpawnForTest: imagery for '%s' is not a T3DImagery",
                  tag, path);
        TObjectImagery::FreeImagery(base);
        return out;
    }
    out.base  = base;
    out.img3d = img3d;
    return out;
}

}   // namespace

// ----- X03 TFlareEffect_Bespoke -------------------------------------------

TFlareEffect_Bespoke* TFlareEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    SLoadedImagery loaded = SparkleLoadImagery(kFlareBespokeImageryPath, "flare");
    if (!loaded.img3d)
        return nullptr;

    auto* flare = new TFlareEffect_Bespoke(loaded.base);
    flare->ForcePos(origin);
    flare->SetMapIndex(MapPane.MakeIndex());
    flare->ActivateComponents();

    // Lazy-mesh-init poke (NumObjects triggers actual mesh load).
    const int32_t num_obj = loaded.img3d->NumObjects();
    const int32_t num_tex = loaded.img3d->NumTextures();
    if (num_obj < 1 || num_tex <= 0)
    {
        log_error("[flare] SpawnForTest: imagery underspec'd (objects=%d, textures=%d)",
                  num_obj, num_tex);
        delete flare;
        return nullptr;
    }

    // Resolve sub-object 0's texture + UV. TFlareAnimator only uses
    // GetObject(0) (effect_old.cpp:586).
    const int32_t tex_slot = SparkleSubObjTextureSlot(loaded.img3d, 0);
    const int32_t slot     = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
    S3DTex tex = {};
    loaded.img3d->GetTexture(slot, &tex);
    flare->texture_ = tex.htexture;
    SparkleResolveSubObjUv(loaded.img3d, 0, flare->uv_rect_);
    flare->size_wu_ = kFlareBespokeBaseSizeWu;
    if (flare->texture_ == kInvalidTexture)
    {
        log_error("[flare] SpawnForTest: sub-object 0 texture unresolved");
        delete flare;
        return nullptr;
    }

    // --- Port of TFlareAnimator::Initialize (effect_old.cpp:517-550).
    // Zero out per-spark position/velocity; the material zeroing in the
    // original is a D3D-specific tint setup we don't need (the renderer
    // applies its own per-instance color modulation).
    for (int32_t c = 0; c < kFlareBespokeNumSparks; ++c)
    {
        flare->sparks_[c].p = hmm_vec3{0.0f, 0.0f, 0.0f};
        flare->sparks_[c].v = hmm_vec3{0.0f, 0.0f, 0.0f};
    }

    log_info("[flare] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "texture=%u uv=[%.3f,%.3f %.3fx%.3f]",
             kFlareBespokeImageryPath, flare->GetMapIndex(),
             origin.x, origin.y, origin.z,
             flare->texture_,
             flare->uv_rect_[0], flare->uv_rect_[1],
             flare->uv_rect_[2], flare->uv_rect_[3]);
    return flare;
}

void TFlareEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TFlareAnimator::Animate (effect_old.cpp:557-582).
    // Framerate-independent via the 24Hz sim-tick accumulator. Each tick
    // runs the original per-tick integration exactly once.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kFlareBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kFlareBespokeSimTickMs);

        for (int32_t c = 0; c < kFlareBespokeNumSparks; ++c)
        {
            // effect_old.cpp:564-575: ground-bounce respawn.
            if (sparks_[c].p.Z <= 0.0f)
            {
                sparks_[c].p.Z = 0.0f;
                sparks_[c].v.Z = -sparks_[c].v.Z * 0.5f;
                if (std::fabs(sparks_[c].v.Z) < 0.4f)
                {
                    sparks_[c].p.X = sparks_[c].p.Y = sparks_[c].p.Z = 0.0f;
                    sparks_[c].v.X = float(random(0, 6) - 3) / 2.0f;
                    sparks_[c].v.Y = float(random(0, 6) - 3) / 2.0f;
                    sparks_[c].v.Z = float(random(5, 8));
                }
            }
            // effect_old.cpp:576: gravity.
            sparks_[c].v.Z -= 0.5f;

            // effect_old.cpp:578-580: position integration.
            sparks_[c].p.X += sparks_[c].v.X;
            sparks_[c].p.Y += sparks_[c].v.Y;
            sparks_[c].p.Z += sparks_[c].v.Z;
        }
    }

    // --- Render: port of TFlareAnimator::Render (effect_old.cpp:584-611).
    // SetBlendState (= Alpha — the snapshot writes SRC_ALPHA / INV_SRC_ALPHA
    // factors; we preserve that verbatim per the no-reinterpretation rule).
    // Each spark drawn with obj->scl=4 and obj->rot=(-pi/2, 0, -pi/4) —
    // the ground-flat tip + 45-degree in-plane spin (F03 pattern). The
    // engine analog for the rot.x=-pi/2 tip is WorldXY orientation; the
    // rot.z=-pi/4 in-plane spin folds into the particle's rotation_rad on
    // a WorldXY quad.
    const S3DPoint& base = Pos();
    SParticleDrawItem item = {};
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::WorldXY;
    item.debug_mode      = debug_mode;
    item.key.texture     = texture_;
    item.uv_rect[0] = uv_rect_[0];
    item.uv_rect[1] = uv_rect_[1];
    item.uv_rect[2] = uv_rect_[2];
    item.uv_rect[3] = uv_rect_[3];
    // Static in-plane spin (-pi/4) — the original obj->rot.z.
    item.rotation_rad = -float(M_PI) / 4.0f;
    // obj->scl = 4 — the original scales the unit-size quad by 4.
    item.size_wu[0] = size_wu_ * kFlareBespokeScale;
    item.size_wu[1] = size_wu_ * kFlareBespokeScale;

    for (int32_t c = 0; c < kFlareBespokeNumSparks; ++c)
    {
        // effect_old.cpp:600: obj->pos = p[c].
        item.world_pos[0] = float(base.x) + sparks_[c].p.X;
        item.world_pos[1] = float(base.y) + sparks_[c].p.Y;
        item.world_pos[2] = float(base.z) + sparks_[c].p.Z;
        Renderer->SubmitFxParticle(item);
    }

    // --- LS coupling: re-add one point light at the effect origin per
    // frame. The flare is "sparkle/glow family"; in-game it would be the
    // L01 TLightSource bound to the effect via spell.def. Warm-yellow
    // 240wu radius matches X17 / F03 family conventions.
    Renderer->AddPointLight(float(base.x), float(base.y), float(base.z) + 16.0f,
                            kFlareBespokeLightRadiusWu,
                            1.0f, 0.85f, 0.35f,
                            kFlareBespokeLightIntensity);
}

// ----- X10 TSymGlowEffect_Bespoke -----------------------------------------

TSymGlowEffect_Bespoke* TSymGlowEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    SLoadedImagery loaded = SparkleLoadImagery(kSymGlowBespokeImageryPath, "symglow");
    if (!loaded.img3d)
        return nullptr;

    auto* sym = new TSymGlowEffect_Bespoke(loaded.base);
    sym->ForcePos(origin);
    sym->SetMapIndex(MapPane.MakeIndex());
    sym->ActivateComponents();

    const int32_t num_obj = loaded.img3d->NumObjects();
    const int32_t num_tex = loaded.img3d->NumTextures();
    if (num_obj < 1 || num_tex <= 0)
    {
        log_error("[symglow] SpawnForTest: imagery underspec'd (objects=%d, textures=%d)",
                  num_obj, num_tex);
        delete sym;
        return nullptr;
    }

    // SetupObjects (effect_old.cpp:4588-4606): iterate sub-objects, shift
    // authored uv.tv by -0.01 per vert. The snapshot mutates the imagery
    // verts in place; the bespoke port can't (immutable imagery) so we
    // bake the -0.01 shift into the uv_rect's V origin instead.
    const int32_t obj = 0;     // sub-object 0 — the single SymGlow quad
    const int32_t tex_slot = SparkleSubObjTextureSlot(loaded.img3d, obj);
    const int32_t slot     = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
    S3DTex tex = {};
    loaded.img3d->GetTexture(slot, &tex);
    sym->texture_ = tex.htexture;
    SparkleResolveSubObjUv(loaded.img3d, obj, sym->uv_rect_);
    // SetupObjects shift: tv -= 0.01 per vert.
    sym->uv_rect_[1] -= 0.01f;
    sym->size_wu_ = kSymGlowBespokeBaseSizeWu;
    if (sym->texture_ == kInvalidTexture)
    {
        log_error("[symglow] SpawnForTest: sub-object 0 texture unresolved");
        delete sym;
        return nullptr;
    }

    // SetupObjects (effect_old.cpp:4590-4592): timer=0, zscale=2.0, dz=0.1.
    sym->timer_  = 0;
    sym->zscale_ = kSymGlowBespokeZMin;
    sym->dz_     = kSymGlowBespokeDzInit;
    sym->u_offset_ = 0.0f;

    log_info("[symglow] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "texture=%u uv=[%.3f,%.3f %.3fx%.3f]",
             kSymGlowBespokeImageryPath, sym->GetMapIndex(),
             origin.x, origin.y, origin.z,
             sym->texture_,
             sym->uv_rect_[0], sym->uv_rect_[1],
             sym->uv_rect_[2], sym->uv_rect_[3]);
    return sym;
}

void TSymGlowEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TSymGlowAnimator::Animate (effect_old.cpp:4615-4633).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kSymGlowBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kSymGlowBespokeSimTickMs);

        // effect_old.cpp:4619: timer++.
        timer_++;

        // effect_old.cpp:4621-4622: every 40 ticks flip dz sign.
        if (!(timer_ % kSymGlowBespokeFlipTicks))
            dz_ *= -1.0f;

        // effect_old.cpp:4624: zscale += dz.
        zscale_ += dz_;

        // effect_old.cpp:4626-4630: clamp zscale to [2, 5].
        if (zscale_ < kSymGlowBespokeZMin)
            zscale_ = kSymGlowBespokeZMin;
        else if (zscale_ > kSymGlowBespokeZMax)
            zscale_ = kSymGlowBespokeZMax;

        // effect_old.cpp:4632: u = random(2,8)/100. The original advances
        // the per-vertex tu accumulator by `u` each Render. We accumulate
        // it here per sim-tick (the Render and Animate run once each per
        // sim-tick in the original) so the scroll cadence matches.
        const float u_step = float(random(kSymGlowBespokeUScrollMin,
                                          kSymGlowBespokeUScrollMax)) / 100.0f;
        u_offset_ += u_step;
        if (u_offset_ > 4096.0f)
            u_offset_ -= 4096.0f;     // keep bounded over long sessions
    }

    // --- Render: port of TSymGlowAnimator::Render (effect_old.cpp:4642-4665).
    // SetBlendState = Alpha. Single sub-object drawn once with:
    //   scl = (1.4, 1.4, zscale)  — zscale 2..5 stretches the quad
    //                                vertically (a 2D billboard, so this
    //                                reads as an aspect-stretched glow)
    //   per-vert tu += u           — V-scroll (the field is named tu but
    //                                in effect.cpp:4602 the SetupObjects
    //                                shifts tv; the Animate scrolls tu).
    const S3DPoint& base = Pos();
    SBillboardDrawItem item = {};
    item.world_pos[0] = float(base.x);
    item.world_pos[1] = float(base.y);
    item.world_pos[2] = float(base.z);
    // scl: (1.4, 1.4, zscale). On a screen-aligned 2D billboard, the
    // X = horizontal scale, Y = vertical scale — Z is unused in our
    // submission shape, so we fold the zscale onto the vertical size
    // (the snapshot's scl.z effectively stretches the quad's height
    // because the SymGlow imagery is a vertical sigil).
    item.size_wu[0] = size_wu_ * kSymGlowBespokeScaleXY;
    item.size_wu[1] = size_wu_ * zscale_;
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    // UV-scroll: shift the U origin by the accumulated u_offset (the
    // snapshot's per-vertex tu += u). The renderer wraps tex coords so
    // accumulated offsets are fine.
    item.uv_rect[0] = uv_rect_[0] + u_offset_;
    item.uv_rect[1] = uv_rect_[1];
    item.uv_rect[2] = uv_rect_[2];
    item.uv_rect[3] = uv_rect_[3];
    item.key.texture     = texture_;
    item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = debug_mode;
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    Renderer->SubmitFxBillboard(item);

    // --- LS coupling: soft warm point light at the symbol origin, gently
    // modulated by zscale so the light pulses with the breathing scale.
    const float t = (zscale_ - kSymGlowBespokeZMin)
                  / (kSymGlowBespokeZMax - kSymGlowBespokeZMin);
    const float intensity = kSymGlowBespokeLightIntensity * (0.6f + 0.4f * t);
    Renderer->AddPointLight(float(base.x), float(base.y), float(base.z) + 32.0f,
                            kSymGlowBespokeLightRadiusWu,
                            1.0f, 0.78f, 0.32f,
                            intensity);
}

// ----- M07 TPhotonEffect_Bespoke ------------------------------------------

void TPhotonEffect_Bespoke::ResetLaunch_()
{
    // --- Port of TPhotonAnimator::Initialize (missileeffect.cpp:187-234).
    // Initialize central flare (slot 0).
    p_[0].X = p_[0].Y = p_[0].Z = 0.0f;
    v_[0].X = v_[0].Y = v_[0].Z = 0.0f;
    scale_[0]    = 1.0f;
    framenum_[0] = 0;

    // Initialize the starting position of each spark, its scale and a
    // delay before it starts (a neg. framenum). NUM_PHOTON_SPARKS=16.
    for (int32_t n = 1; n < kPhotonBespokeNumSparks; n++)
    {
        p_[n].X = float(random(-kPhotonBespokeLaunchRadius, kPhotonBespokeLaunchRadius));
        p_[n].Y = float(random(-kPhotonBespokeLaunchRadius, kPhotonBespokeLaunchRadius));
        p_[n].Z = float(random(-kPhotonBespokeLaunchRadius, kPhotonBespokeLaunchRadius));

        // Set a vector for most of the sparks to move them to the center
        // of the effect (missileeffect.cpp:213-219).
        if (n > kPhotonBespokeNumSparks / 3)
        {
            v_[n].X = -p_[n].X / float(kPhotonBespokeLaunchDuration * 3 / 4);
            v_[n].Y = -p_[n].Y / float(kPhotonBespokeLaunchDuration * 3 / 4);
            v_[n].Z = -p_[n].Z / float(kPhotonBespokeLaunchDuration * 3 / 4);
        }
        else
        {
            v_[n].X = v_[n].Y = v_[n].Z = 0.0f;
        }

        scale_[n] = 0.0f;

        framenum_[n] = random(-kPhotonBespokeLaunchDuration / 2, 0);
    }

    // Initialize the framenums for the trailing photon sparks
    // (missileeffect.cpp:229-230).
    for (int32_t n = kPhotonBespokeNumSparks; n < kPhotonBespokeSlots; n++)
        framenum_[n] = -1;

    // Set how many photon sparks are active at the beginning
    // (missileeffect.cpp:233).
    activesparks_ = kPhotonBespokeNumSparks - 1;
}

void TPhotonEffect_Bespoke::EnterFly_()
{
    // --- Port of TPhotonAnimator::Animate FLY first-time block
    // (missileeffect.cpp:302-322). Harness has no live velocity so we
    // use a fixed back-projection direction (Y- = "behind" in iso).
    // dir.x / dir.y / dir.z are in ROLLOVER units (1 wu = 65536); the
    // snapshot scales each trailing photon by `t / ROLLOVER` along
    // -dir, so we use a tiny fake vel that produces visible spacing.
    constexpr float kFakeVelMag = 8.0f;   // per-step wu (matches MISSILE_SPEED feel)
    const float dirx = 0.0f;
    const float diry = kFakeVelMag;       // sim "moving south"
    const float dirz = 0.0f;

    // Initialize the variables for the leading photon (missileeffect.cpp:308-310).
    p_[0].X = p_[0].Y = p_[0].Z = 0.0f;
    scale_[0]    = 3.5f;
    framenum_[0] = 0;

    // Initialize the variables for the trailing photons
    // (missileeffect.cpp:312-321).
    int32_t t = 1;
    for (int32_t n = kPhotonBespokeNumSparks; n < kPhotonBespokeSlots; n++, t++)
    {
        framenum_[n] = -t;
        framenum_[t] = -1;
        // Back-projection: -dir * t (skipping the ROLLOVER division —
        // dir is already in our local wu/step scale).
        p_[n].X = -dirx * float(t);
        p_[n].Y = -diry * float(t);
        p_[n].Z = -dirz * float(t);
        scale_[n] = 3.5f - float(t) * kPhotonBespokeScaleStep;
    }
}

void TPhotonEffect_Bespoke::EnterExplode_()
{
    // --- Port of TPhotonAnimator::Animate EXPLODE first-time block
    // (missileeffect.cpp:335-353).
    for (int32_t n = 0; n < kPhotonBespokeNumSparks; n++)
    {
        p_[n].X = p_[n].Y = p_[n].Z = 0.0f;
        scale_[n] = 3.0f;

        v_[n].X = float(random(-3, 3)) * 3.0f / 2.0f;
        v_[n].Y = float(random(-3, 3)) * 3.0f / 2.0f;
        v_[n].Z = float(random(-3, 3)) * 3.0f / 2.0f;

        framenum_[n] = 0;
    }
    activesparks_ = kPhotonBespokeNumSparks;
}

TPhotonEffect_Bespoke* TPhotonEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    SLoadedImagery loaded = SparkleLoadImagery(kPhotonBespokeImageryPath, "photon");
    if (!loaded.img3d)
        return nullptr;

    auto* photon = new TPhotonEffect_Bespoke(loaded.base);
    photon->ForcePos(origin);
    photon->SetMapIndex(MapPane.MakeIndex());
    photon->ActivateComponents();

    const int32_t num_obj = loaded.img3d->NumObjects();
    const int32_t num_tex = loaded.img3d->NumTextures();
    if (num_obj < 1 || num_tex <= 0)
    {
        log_error("[photon] SpawnForTest: imagery underspec'd (objects=%d, textures=%d)",
                  num_obj, num_tex);
        delete photon;
        return nullptr;
    }

    // Get how many frames are in the texture (missileeffect.cpp:194-195).
    S3DTex tex = {};
    const int32_t tex_slot = SparkleSubObjTextureSlot(loaded.img3d, 0);
    const int32_t slot     = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
    loaded.img3d->GetTexture(slot, &tex);
    photon->texture_      = tex.htexture;
    photon->numtexframes_ = tex.numframes > 0 ? tex.numframes : 1;
    SparkleResolveSubObjUv(loaded.img3d, 0, photon->uv_rect_);
    photon->size_wu_ = kPhotonBespokeBaseSizeWu;
    if (photon->texture_ == kInvalidTexture)
    {
        log_error("[photon] SpawnForTest: sub-object 0 texture unresolved");
        delete photon;
        return nullptr;
    }

    // Set the beginning state of the effect (missileeffect.cpp:198).
    photon->state_       = 0;     // MISSILE_LAUNCH
    photon->oldstate_    = -1;    // force first-time on entry
    photon->state_ticks_ = 0;
    photon->ResetLaunch_();

    log_info("[photon] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "texture=%u numtexframes=%d uv=[%.3f,%.3f %.3fx%.3f]",
             kPhotonBespokeImageryPath, photon->GetMapIndex(),
             origin.x, origin.y, origin.z,
             photon->texture_, photon->numtexframes_,
             photon->uv_rect_[0], photon->uv_rect_[1],
             photon->uv_rect_[2], photon->uv_rect_[3]);
    return photon;
}

void TPhotonEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TPhotonAnimator::Animate (missileeffect.cpp:243-399).
    // Framerate-independent via the 24Hz sim-tick accumulator.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kPhotonBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kPhotonBespokeSimTickMs);
        state_ticks_++;

        // Harness state machine: cycle LAUNCH (until activesparks<NUM/3)
        // -> FLY (kPhotonBespokeFlyTicks) -> EXPLODE (until activesparks==0)
        // -> IDLE (gap) -> LAUNCH again. In-game this would be driven by
        // TMissileEffect::Pulse range/hit transitions, but the harness has
        // no live missile machinery so we drive the cycle on a fixed cadence.

        switch (state_)
        {
            case 0:     // MISSILE_LAUNCH
            {
                // missileeffect.cpp:256-259: adjust central flare.
                framenum_[0]++;
                if ((framenum_[0] > kPhotonBespokeLaunchDuration / 2)
                    && (scale_[0] < 5.5f))
                {
                    scale_[0] += kPhotonBespokeScaleStep;
                }

                // missileeffect.cpp:262-295: adjust the other sparks.
                for (int32_t n = 1; n < kPhotonBespokeNumSparks; n++)
                {
                    framenum_[n]++;

                    if (framenum_[n] >= 0)
                    {
                        // Move the sparks (missileeffect.cpp:269-271).
                        p_[n].X += v_[n].X;
                        p_[n].Y += v_[n].Y;
                        p_[n].Z += v_[n].Z;

                        if (framenum_[n] > kPhotonBespokeLaunchDuration / 2)
                        {
                            // missileeffect.cpp:275-290: scale-down + retire.
                            if (scale_[n] != 0.0f)
                            {
                                scale_[n] -= kPhotonBespokeScaleStep;
                                if (scale_[n] <= 0.0f)
                                {
                                    activesparks_--;
                                    // missileeffect.cpp:284-286: when 2/3 of
                                    // sparks are gone, set missile status
                                    // (= "ready to fly"). Harness analog:
                                    // transition to FLY when the snapshot
                                    // would set status.
                                    scale_[n] = 0.0f;
                                }
                            }
                        }
                        else
                        {
                            // missileeffect.cpp:292-293: scale-up.
                            scale_[n] += kPhotonBespokeScaleStep;
                        }
                    }
                }

                // State transition: in-game this happens when TMissileEffect
                // sees the status flag (missileeffect.cpp:286). Harness
                // mirrors: when activesparks_ < NUM/3.
                if (activesparks_ < kPhotonBespokeNumSparks / 3)
                {
                    state_       = 1;     // MISSILE_FLY
                    state_ticks_ = 0;
                }
                break;
            }

            case 1:     // MISSILE_FLY
            {
                if (state_ != oldstate_)
                {
                    EnterFly_();
                }
                else
                {
                    // missileeffect.cpp:325-327: just update the frame counter
                    // for the trailing photons.
                    for (int32_t n = kPhotonBespokeNumSparks; n < kPhotonBespokeSlots; n++)
                        framenum_[n]++;
                }

                if (state_ticks_ >= kPhotonBespokeFlyTicks)
                {
                    state_       = 2;     // MISSILE_EXPLODE
                    state_ticks_ = 0;
                }
                break;
            }

            case 2:     // MISSILE_EXPLODE
            {
                if (state_ != oldstate_)
                {
                    EnterExplode_();
                }
                else
                {
                    // missileeffect.cpp:357-393: per-spark integration +
                    // scale fall + trailing-photon drain.
                    for (int32_t n = 0; n < kPhotonBespokeNumSparks; n++)
                    {
                        const int32_t t = n + kPhotonBespokeNumSparks;
                        framenum_[n]++;

                        // Move the sparks (missileeffect.cpp:363-365).
                        p_[n].X += v_[n].X;
                        p_[n].Y += v_[n].Y;
                        p_[n].Z += v_[n].Z;

                        if (scale_[n] > 0.0f)
                        {
                            scale_[n] -= kPhotonBespokeScaleStep / 2.0f;
                            if (scale_[n] <= 0.0f)
                            {
                                scale_[n] = 0.0f;
                                activesparks_--;
                            }
                        }

                        // Trailing photon drain (missileeffect.cpp:383-389).
                        if (scale_[t] != 0.0f)
                        {
                            framenum_[t]++;
                            scale_[t] -= kPhotonBespokeScaleStep / 2.0f;
                            if (scale_[t] <= 0.0f)
                                scale_[t] = 0.0f;
                        }
                    }
                }

                if (activesparks_ <= 0 || state_ticks_ >= kPhotonBespokeExplodeTicks)
                {
                    state_       = 3;     // IDLE (harness gap)
                    state_ticks_ = 0;
                }
                break;
            }

            default:    // 3 = IDLE (harness-only gap before next LAUNCH)
            {
                if (state_ticks_ >= kPhotonBespokeIdleTicks)
                {
                    state_       = 0;     // back to LAUNCH
                    state_ticks_ = 0;
                    ResetLaunch_();
                }
                break;
            }
        }

        // Keep track of the state we're in (missileeffect.cpp:398).
        oldstate_ = state_;
    }

    // --- Render: port of TPhotonAnimator::Render (missileeffect.cpp:408-441).
    // SetBlendState = Alpha. Single sub-object drawn up to 32 times with
    // per-spark scale and position.
    const S3DPoint& base = Pos();
    SParticleDrawItem item = {};
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    item.key.texture     = texture_;
    item.rotation_rad    = 0.0f;
    // UV: snapshot picks per-spark frame = framenum % numtexframes
    // (missileeffect.cpp:429). The decoded photon.i3d texture's frames
    // are laid out along U (each frame occupies 1/numtexframes_ of the
    // U range). We compute the per-frame UV sub-rect at submission time.

    for (int32_t n = 0; n < kPhotonBespokeSlots; n++)
    {
        // missileeffect.cpp:418: only render the spark if framenum is positive.
        if (framenum_[n] < 0)
            continue;
        if (scale_[n] <= 0.0f)
            continue;

        // missileeffect.cpp:424-426: scale + position.
        const float scl = scale_[n];
        item.size_wu[0] = size_wu_ * scl;
        item.size_wu[1] = size_wu_ * scl;
        item.world_pos[0] = float(base.x) + p_[n].X;
        item.world_pos[1] = float(base.y) + p_[n].Y;
        item.world_pos[2] = float(base.z) + p_[n].Z;

        // Per-spark frame UV. The snapshot's `frame = framenum % numtexframes`
        // (missileeffect.cpp:429) cycles the bound texture frame; we slice
        // the authored U range into numtexframes_ cells and pick the
        // (framenum % numtexframes_)-th.
        const int32_t fnum = framenum_[n] % numtexframes_;
        if (numtexframes_ > 1)
        {
            const float cell_w = uv_rect_[2] / float(numtexframes_);
            item.uv_rect[0] = uv_rect_[0] + cell_w * float(fnum);
            item.uv_rect[1] = uv_rect_[1];
            item.uv_rect[2] = cell_w;
            item.uv_rect[3] = uv_rect_[3];
        }
        else
        {
            item.uv_rect[0] = uv_rect_[0];
            item.uv_rect[1] = uv_rect_[1];
            item.uv_rect[2] = uv_rect_[2];
            item.uv_rect[3] = uv_rect_[3];
        }

        Renderer->SubmitFxParticle(item);
    }

    // --- LS coupling: re-add one bright blue-white point light at the
    // effect origin per frame. Brightest at EXPLODE (per the bolt
    // discharge); fades with the leading-spark scale during LAUNCH/FLY.
    float light_t = 0.5f;
    if (state_ == 2)    // EXPLODE — brightest
        light_t = 1.0f;
    else if (scale_[0] > 0.0f)
        light_t = std::fmin(1.0f, scale_[0] / 5.5f);
    Renderer->AddPointLight(float(base.x), float(base.y), float(base.z) + 24.0f,
                            kPhotonBespokeLightRadiusWu,
                            0.7f, 0.85f, 1.0f,
                            kPhotonBespokeLightIntensity * light_t);
}

// ----- M08 TPixieEffect_Bespoke -------------------------------------------

TPixieEffect_Bespoke* TPixieEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    SLoadedImagery loaded = SparkleLoadImagery(kPixieBespokeImageryPath, "pixie");
    if (!loaded.img3d)
        return nullptr;

    auto* pixie = new TPixieEffect_Bespoke(loaded.base);
    pixie->ForcePos(origin);
    pixie->SetMapIndex(MapPane.MakeIndex());
    pixie->ActivateComponents();

    const int32_t num_obj = loaded.img3d->NumObjects();
    const int32_t num_tex = loaded.img3d->NumTextures();
    if (num_obj < 1 || num_tex <= 0)
    {
        log_error("[pixie] SpawnForTest: imagery underspec'd (objects=%d, textures=%d)",
                  num_obj, num_tex);
        delete pixie;
        return nullptr;
    }

    // Resolve the 2 sub-objects (pix[i].time = 0 or 1 selects between
    // them; effect_old.cpp:12237 GetObject(pix[i].time)). Fall back to
    // sub-object 0 if only one exists.
    for (int32_t i = 0; i < 2; ++i)
    {
        const int32_t obj = (num_obj > i) ? i : 0;
        const int32_t tex_slot = SparkleSubObjTextureSlot(loaded.img3d, obj);
        const int32_t slot     = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
        S3DTex tex = {};
        loaded.img3d->GetTexture(slot, &tex);
        pixie->textures_[i] = tex.htexture;
        SparkleResolveSubObjUv(loaded.img3d, obj, pixie->uv_rect_[i]);
    }
    pixie->size_wu_ = kPixieBespokeBaseSizeWu;
    if (pixie->textures_[0] == kInvalidTexture)
    {
        log_error("[pixie] SpawnForTest: sub-object 0 texture unresolved");
        delete pixie;
        return nullptr;
    }
    if (pixie->textures_[1] == kInvalidTexture)
        pixie->textures_[1] = pixie->textures_[0];     // fall back to first

    // --- Port of TPixieAnimator::Initialize (effect_old.cpp:12109-12127).
    for (int32_t i = 0; i < kPixieBespokeNumParts; i++)
    {
        // scale.x/y/z = random(MINSCALE*1000, MAXSCALE*1000) / 1000.
        const float s = float(random(int32_t(kPixieBespokeMinScale * 1000.0f),
                                     int32_t(kPixieBespokeMaxScale * 1000.0f))) / 1000.0f;
        pixie->pix_[i].scale.X = pixie->pix_[i].scale.Y = pixie->pix_[i].scale.Z = s;
        pixie->pix_[i].pos.X = float(random(-32, 32));
        pixie->pix_[i].pos.Y = float(random(-32, 32));
        pixie->pix_[i].pos.Z = float(random(-32, 32));
        pixie->pix_[i].vel.X = float(random(-32, 32)) / 25.0f;
        pixie->pix_[i].vel.Y = float(random(-32, 32)) / 25.0f;
        pixie->pix_[i].vel.Z = float(random(-32, 32)) / 25.0f;
        pixie->pix_[i].time  = random(0, 1);
    }
    pixie->charnear_     = 100;
    pixie->sim_accum_ms_ = 0.0;

    log_info("[pixie] SpawnForTest: '%s' map_index=%d origin=(%d,%d,%d) "
             "textures={%u,%u} num_obj=%d",
             kPixieBespokeImageryPath, pixie->GetMapIndex(),
             origin.x, origin.y, origin.z,
             pixie->textures_[0], pixie->textures_[1], num_obj);
    return pixie;
}

void TPixieEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Update: port of TPixieAnimator::Animate (effect_old.cpp:12129-12227).
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kPixieBespokeSimTickMs))
    {
        sim_accum_ms_ -= double(kPixieBespokeSimTickMs);

        // Per-particle integration (effect_old.cpp:12134-12163).
        for (int32_t i = 0; i < kPixieBespokeNumParts; i++)
        {
            // scale.x += random(-35, 35) / 1000.
            pix_[i].scale.X += float(random(-35, 35)) / 1000.0f;
            // The snapshot has scale.y = scale.z = scale.x commented out
            // (effect_old.cpp:12137 — "//pix[i].scale.y = pix[i].scale.z =
            // pix[i].scale.x;"). We DO NOT re-instate it (preserve dead-
            // code form as written). Only scale.x is mutated.
            if (pix_[i].scale.X > 0.07f)
                pix_[i].scale.X = 0.07f;

            // pos += vel.
            pix_[i].pos.X += pix_[i].vel.X;
            pix_[i].pos.Y += pix_[i].vel.Y;
            pix_[i].pos.Z += pix_[i].vel.Z;

            // Centring restorative force (effect_old.cpp:12143-12154).
            if (pix_[i].pos.X > 0.0f)
                pix_[i].vel.X -= kPixieBespokeAcc;
            if (pix_[i].pos.X < 0.0f)
                pix_[i].vel.X += kPixieBespokeAcc;
            if (pix_[i].pos.Y > 0.0f)
                pix_[i].vel.Y -= kPixieBespokeAcc;
            if (pix_[i].pos.Y < 0.0f)
                pix_[i].vel.Y += kPixieBespokeAcc;
            if (pix_[i].pos.Z > 0.0f)
                pix_[i].vel.Z -= kPixieBespokeAcc;
            if (pix_[i].pos.Z < 0.0f)
                pix_[i].vel.Z += kPixieBespokeAcc;

            // Time-flip on a 1/49 chance (effect_old.cpp:12155-12156).
            if (random(0, 48) == 17)
                pix_[i].time = 1 - pix_[i].time;

            // Floor clamp (effect_old.cpp:12157-12158).
            if (pix_[i].pos.Z < -float(kPixieBespokeHeight))
                pix_[i].pos.Z = -float(kPixieBespokeHeight);

            // Scale clamp (effect_old.cpp:12159-12162).
            if (pix_[i].scale.X < kPixieBespokeMinScale)
                pix_[i].scale.X = kPixieBespokeMinScale;
            if (pix_[i].scale.X > kPixieBespokeMaxScale)
                pix_[i].scale.X = kPixieBespokeMaxScale;
        }

        // effect_old.cpp:12164-12226: FindObjectsInRange character-flee
        // logic. Harness has no live character iteration in --test=vfx,
        // so we skip this block entirely. charnear_ stays at 100 (= 1.0x
        // scale on per-particle pos in Render). The "resume to origpos"
        // branch is also a no-op because the harness pins the effect at
        // a fixed origin via ForcePos.
    }

    // --- Render: port of TPixieAnimator::Render (effect_old.cpp:12229-12264).
    // SetBlendState = Alpha. Per-particle: pick sub-object (pix[i].time),
    // scale uniform by scale.x/(1+time*2), rotate Z by -face*TORADIAN
    // (face is 0 for a sector-less harness effect — so rotation is 0),
    // translate to (pos.x*charnear/100, pos.y*charnear/100, PIX_HEIGHT+pos.z).
    const S3DPoint& base = Pos();
    SParticleDrawItem item = {};
    item.color_rgba[0] = 1.0f;
    item.color_rgba[1] = 1.0f;
    item.color_rgba[2] = 1.0f;
    item.color_rgba[3] = 1.0f;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    item.rotation_rad    = 0.0f;     // face = 0 in harness (no in-game facing)

    for (int32_t i = 0; i < kPixieBespokeNumParts; i++)
    {
        // Pick sub-object (effect_old.cpp:12237: obj = GetObject(pix[i].time)).
        const int32_t time_idx = pix_[i].time != 0 ? 1 : 0;
        item.key.texture = textures_[time_idx];
        item.uv_rect[0] = uv_rect_[time_idx][0];
        item.uv_rect[1] = uv_rect_[time_idx][1];
        item.uv_rect[2] = uv_rect_[time_idx][2];
        item.uv_rect[3] = uv_rect_[time_idx][3];

        // Per-particle scale (effect_old.cpp:12243).
        // obj->scl = scale.x / (1 + (time * 2.0f)).
        const float scl = pix_[i].scale.X / (1.0f + float(pix_[i].time) * 2.0f);
        item.size_wu[0] = size_wu_ * scl;
        item.size_wu[1] = size_wu_ * scl;

        // Position (effect_old.cpp:12252-12254).
        const float wx = float(base.x) + (pix_[i].pos.X * float(charnear_)) / 100.0f;
        const float wy = float(base.y) + (pix_[i].pos.Y * float(charnear_)) / 100.0f;
        const float wz = float(base.z) + float(kPixieBespokeHeight) + pix_[i].pos.Z;
        item.world_pos[0] = wx;
        item.world_pos[1] = wy;
        item.world_pos[2] = wz;

        Renderer->SubmitFxParticle(item);
    }

    // --- LS coupling: soft blue-green point light at the swarm centre.
    // Modulated by the mean scale of the swarm so the light gently pulses.
    float mean_scale = 0.0f;
    for (int32_t i = 0; i < kPixieBespokeNumParts; i++)
        mean_scale += pix_[i].scale.X;
    mean_scale /= float(kPixieBespokeNumParts);
    const float t = (mean_scale - kPixieBespokeMinScale)
                  / (kPixieBespokeMaxScale - kPixieBespokeMinScale);
    const float intensity = kPixieBespokeLightIntensity * (0.6f + 0.4f * std::fmax(0.0f, std::fmin(1.0f, t)));
    Renderer->AddPointLight(float(base.x), float(base.y),
                            float(base.z) + float(kPixieBespokeHeight),
                            kPixieBespokeLightRadiusWu,
                            0.55f, 0.95f, 0.85f,
                            intensity);
}

// *************************************************************************
// * Wave 2 batch wave-bespoke-06-strip-ribbon: faithful direct ports of   *
// * pre-release snapshot animator bodies for Strip / ribbon family.       *
// * Translation rules: identical math, identical variable names where     *
// * legal in C++, identical per-tick step and render-pass order. Only the *
// * render API changes (RenderObject -> SubmitFxBillboard / SubmitFxStrip).
// *************************************************************************

namespace {

// Shared procedural-glow texture for the strip/ribbon family — used as a
// first-pass stand-in for the real Magic\Lightning Spark.I3D /
// Magic\NewLightStrip.I3D / Magic\ribbon.I3D assets which aren't bound
// yet. White cubic-falloff core, transparent edges, pairs with
// AdditiveStraight (premultiplied). 1 x 64 RGBA8 strip.
TTextureHandle StripFamilyGlowTexture()
{
    if (!Renderer) return kInvalidTexture;
    constexpr uint64_t kKey = 0x46584C4753545246ull;   // "FXLGSTRF"
    constexpr int32_t  kH   = 64;
    uint8_t rgba[kH * 4];
    for (int32_t i = 0; i < kH; ++i)
    {
        const float v       = float(i) / float(kH - 1);
        const float center  = std::abs(v - 0.5f) * 2.0f;
        const float falloff = (std::max)(0.0f, 1.0f - center);
        const float a       = falloff * falloff * falloff;
        const uint8_t byte  = uint8_t(a * 255.0f);
        rgba[i * 4 + 0] = byte;
        rgba[i * 4 + 1] = byte;
        rgba[i * 4 + 2] = byte;
        rgba[i * 4 + 3] = byte;
    }
    return Renderer->RegisterTextureAsset(kKey, rgba, sizeof(rgba),
                                          1, kH,
                                          ERendererTextureFormat::RGBA8,
                                          uint64_t(sizeof(rgba)));
}

// Round soft-edge billboard texture for the spark/streamer/ribbon-spark
// placeholders (no Magic\<x>.I3D bound yet). 32x32 white disc with
// quadratic-falloff alpha.
TTextureHandle StripFamilySparkTexture()
{
    if (!Renderer) return kInvalidTexture;
    constexpr uint64_t kKey = 0x46584C475350524Bull;   // "FXLGSPRK"
    constexpr int32_t  kS   = 32;
    uint8_t rgba[kS * kS * 4];
    for (int32_t y = 0; y < kS; ++y)
    {
        for (int32_t x = 0; x < kS; ++x)
        {
            const float dx = (float(x) + 0.5f) / float(kS) * 2.0f - 1.0f;
            const float dy = (float(y) + 0.5f) / float(kS) * 2.0f - 1.0f;
            const float r  = std::sqrt(dx * dx + dy * dy);
            const float a  = (std::max)(0.0f, 1.0f - r);
            const float aa = a * a;
            const uint8_t b = uint8_t(aa * 255.0f);
            const int32_t off = (y * kS + x) * 4;
            rgba[off + 0] = b;
            rgba[off + 1] = b;
            rgba[off + 2] = b;
            rgba[off + 3] = b;
        }
    }
    return Renderer->RegisterTextureAsset(kKey, rgba, sizeof(rgba),
                                          kS, kS,
                                          ERendererTextureFormat::RGBA8,
                                          uint64_t(sizeof(rgba)));
}

// 24 Hz sim tick (~41.7 ms) — matches the snapshot's tick rate.
constexpr int32_t kStripFamilySimTickMs = 1000 / 24;

}  // namespace

// =========================================================================
// S04 TLightningAnimator_Bespoke
// =========================================================================
// Snapshot source: src/stripeffect.cpp `#if 0` Initialize/SetupObjects/
// Animate/Render (:472-915) + TStripEffect Pulse (:406-457). Constants
// from src/stripeffect.h (STRIP_*, LIGHTNING_SCALE_*, ADD_FACTOR=2,
// SMOOTH_SIZE=4). The retail animator owns three sub-objects:
//   obj 0 = the LightStrip strip geometry (TStripAnimator)
//   obj 1 = the glow halo
//   obj 2 = the spark sub-emitter image
// First-pass port covers the strip + a tiny glow stand-in; sparks are
// stubbed (the SPARKS_TSparkAnimator forensics doc owns that sub-emitter,
// porting it lives in a follow-up slice).

TLightningAnimator_Bespoke* TLightningAnimator_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    // No imagery bound — first-pass uses the procedural strip glow
    // texture (no Magic\NewLightStrip.I3D loader yet). Same approach as
    // the existing TStripEffect::SpawnForTest path.
    auto* eff = new TLightningAnimator_Bespoke(static_cast<TObjectImagery*>(nullptr));
    eff->ForcePos(origin);
    eff->SetMapIndex(MapPane.MakeIndex());
    eff->ActivateComponents();

    // Caster facing — randomized yaw so successive Combat-cadence re-fires
    // point in different directions (same intuition as TStripEffect).
    const float yaw =
        (float(std::rand()) / float(RAND_MAX)) * 6.28318530718f;
    eff->forward_dir_[0] = std::cos(yaw);
    eff->forward_dir_[1] = std::sin(yaw);
    eff->forward_dir_[2] = 0.0f;

    // Retail SetupObjects walks a raycast up to MAX_MAXPOINTS=100 to find
    // the target range; in the harness we pick a plausible random length
    // in segments (10..40), matching ~100..400 wu (STRIP_SEG_LEN=10).
    eff->maxpoints_ = random(10, 40) * STRIP_SMOOTH_SIZE;
    if (eff->maxpoints_ > 100) eff->maxpoints_ = 100;

    // Snapshot SetupObjects seeds the strip with `numpoints = 2` initial
    // anchors at the local origin offset down-Y (the strip is authored in
    // a local frame with p.y = -10 * i).
    eff->numpoints_ = 2;
    for (int32_t i = 0; i < 2; ++i)
    {
        SBoltAnchor& a = eff->anchors_[i];
        a.pos[0] = float(origin.x) + eff->forward_dir_[0] * float(i * STRIP_SEG_LEN);
        a.pos[1] = float(origin.y) + eff->forward_dir_[1] * float(i * STRIP_SEG_LEN);
        a.pos[2] = float(origin.z) + 50.0f;   // matches Initialize() z+=50
    }

    eff->state_     = kLaunch;
    eff->duration_  = STRIP_FLY_DURATION;
    eff->glow_scale_ = 3.4f;       // retail initial
    eff->alive_     = true;

    log_info("[S04 lightning_bespoke] SpawnForTest: origin=(%d,%d,%d) "
             "maxpoints=%d yaw=%.2frad",
             origin.x, origin.y, origin.z,
             eff->maxpoints_, double(yaw));
    return eff;
}

void TLightningAnimator_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer || !alive_)
        return;

    // --- Sim-tick gate: original animator runs at 24 Hz.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kStripFamilySimTickMs))
    {
        sim_accum_ms_ -= double(kStripFamilySimTickMs);

        // --- Pulse() state machine (stripeffect.cpp :406-457):
        switch (state_)
        {
            case kLaunch:
                state_ = kFly;
                break;
            case kFly:
                --duration_;
                if (duration_ <= 0)
                {
                    state_ = kExplode;
                    duration_ = STRIP_EXPLODE_TICKS;
                }
                break;
            case kExplode:
                if (duration_ > 0) --duration_;
                break;
        }

        // --- Animate() body (stripeffect.cpp :664-820):
        // EXPLODE: shrink the tail at ADD_FACTOR=2 / tick, glow *= 0.8.
        if (state_ == kExplode)
        {
            for (int32_t i = 0; i < 2 && numpoints_ > 0; ++i)
                --numpoints_;
            glow_scale_ *= 0.8f;
        }
        // FLY: grow at ADD_FACTOR=2 anchors / tick until numpoints == maxpoints.
        else if (numpoints_ < maxpoints_)
        {
            for (int32_t i = 0; i < 2 && numpoints_ < maxpoints_; ++i)
            {
                // Snapshot stores p in local frame with p.y = -10 * numpoints,
                // p.x = p.z = 0; we project that to world space along
                // forward_dir_, anchored at the spawn origin.
                SBoltAnchor& a = anchors_[numpoints_];
                a.pos[0] = float(Pos().x) +
                           forward_dir_[0] * float(numpoints_ * STRIP_SEG_LEN);
                a.pos[1] = float(Pos().y) +
                           forward_dir_[1] * float(numpoints_ * STRIP_SEG_LEN);
                a.pos[2] = float(Pos().z) + 50.0f;
                ++numpoints_;
            }
        }

        // Smoothing calculations (stripeffect.cpp :741-792).
        // Allocate the px/pz arrays of size numpoints+SMOOTH_SIZE; fill
        // every SMOOTH_SIZE-th index with random(-13, 13); interpolate
        // intermediate indices with the snapshot's 0.5/2.0/3.5 weights.
        const int32_t n = numpoints_;
        if (n >= 2)
        {
            float px[100 + STRIP_SMOOTH_SIZE] = {0};
            float pz[100 + STRIP_SMOOTH_SIZE] = {0};
            for (int32_t i = 1; i < n; ++i)
            {
                if (!(i % STRIP_SMOOTH_SIZE))
                {
                    px[i] = float(random(-STRIP_JITTER_MAG, STRIP_JITTER_MAG));
                    pz[i] = float(random(-STRIP_JITTER_MAG, STRIP_JITTER_MAG));
                }
            }
            int32_t last_point = 0;
            int32_t next_point = STRIP_SMOOTH_SIZE;
            for (int32_t i = 1; i < n; ++i)
            {
                if (i == next_point)
                {
                    last_point = next_point;
                    next_point += STRIP_SMOOTH_SIZE;
                }
                const int32_t mod = i % STRIP_SMOOTH_SIZE;
                if (mod)
                {
                    if (mod == 1)
                    {
                        px[i] = ((px[next_point] - px[last_point]) * 0.5f) /
                                float(STRIP_SMOOTH_SIZE);
                        pz[i] = ((pz[next_point] - pz[last_point]) * 0.5f) /
                                float(STRIP_SMOOTH_SIZE);
                    }
                    else if (mod == 2)
                    {
                        px[i] = ((px[next_point] - px[last_point]) * 2.0f) /
                                float(STRIP_SMOOTH_SIZE);
                        pz[i] = ((pz[next_point] - pz[last_point]) * 2.0f) /
                                float(STRIP_SMOOTH_SIZE);
                    }
                    else if (mod == 3)
                    {
                        px[i] = ((px[next_point] - px[last_point]) * 3.5f) /
                                float(STRIP_SMOOTH_SIZE);
                        pz[i] = ((pz[next_point] - pz[last_point]) * 3.5f) /
                                float(STRIP_SMOOTH_SIZE);
                    }
                    px[i] += px[last_point];
                    pz[i] += pz[last_point];
                }
            }
            // Apply jitter to every anchor's xy + z, anchor 0 stays clean.
            // Snapshot also has a "down ? p->z = -(i-1)*20/(maxpoints-1)" droop
            // which we omit for first-pass (looks subtle and adds nothing for
            // the boot-render smoke test); keep px/pz only.
            for (int32_t i = 1; i < n; ++i)
            {
                anchors_[i].jitter[0] = px[i];
                anchors_[i].jitter[1] = pz[i];
            }
        }

        // ScrollTexture(-0.1) (stripeffect.cpp :817).
        u_scroll_ += -0.1f;
        if (u_scroll_ < -1.0f) u_scroll_ += 1.0f;
        if (u_scroll_ >  1.0f) u_scroll_ -= 1.0f;

        // Rotating glow degrees (stripeffect.cpp :814-815).
        rotdegree_    = std::fmod(rotdegree_ + 12.0f, 360.0f);
        morrotdegree_ = std::fmod(morrotdegree_ + 16.0f, 360.0f);

        // Self-destruct when EXPLODE drains all points + the glow is dim
        // (snapshot equivalent: `if (my_state == EXPLODE && !spark.GetCount()
        // && !impact_spark.GetCount()) KillThisEffect()`). We approximate
        // by waiting until numpoints == 0 AND glow_scale_ < 0.05.
        if (state_ == kExplode && numpoints_ <= 1 && glow_scale_ < 0.05f)
        {
            alive_ = false;
            break;
        }
    }

    if (numpoints_ < 2)
        return;

    // --- Render() — port of stripeffect.cpp :822-915. Strip half is the
    // SR pipeline submission (AdditiveStraight per snapshot SetAddBlendState
    // -- ALPHABLENDENABLE is on in the snapshot's Render :701 enable). The
    // glow uses two AdditiveStraight billboards as a stand-in for the
    // RenderObject(obj 1) RotateZ()+RotateZ() pair.

    // Perpendicular axes for applying jitter in world XY (matches
    // src/stripeffect.cpp's perp_x/perp_y).
    const float perp_x = -forward_dir_[1];
    const float perp_y =  forward_dir_[0];

    // Build SStripSegment[] for the strip body.
    static thread_local std::vector<SStripSegment> seg_scratch;
    seg_scratch.clear();
    const int32_t n_seg = numpoints_ - 1;
    seg_scratch.reserve(size_t(n_seg));
    for (int32_t i = 0; i < n_seg; ++i)
    {
        const SBoltAnchor& a = anchors_[i];
        const SBoltAnchor& b = anchors_[i + 1];
        const float ta = float(i)     / float(n_seg);
        const float tb = float(i + 1) / float(n_seg);
        SStripSegment seg = {};
        seg.world_a[0] = a.pos[0] + perp_x * a.jitter[0];
        seg.world_a[1] = a.pos[1] + perp_y * a.jitter[0];
        seg.world_a[2] = a.pos[2] + a.jitter[1];
        seg.world_b[0] = b.pos[0] + perp_x * b.jitter[0];
        seg.world_b[1] = b.pos[1] + perp_y * b.jitter[0];
        seg.world_b[2] = b.pos[2] + b.jitter[1];
        // Width hilt -> tip, LIGHTNING_SCALE constants -> STRIP_WIDTH_*.
        seg.width_a_wu = STRIP_WIDTH_HILT * (1.0f - ta) + STRIP_WIDTH_TIP * ta;
        seg.width_b_wu = STRIP_WIDTH_HILT * (1.0f - tb) + STRIP_WIDTH_TIP * tb;
        // Snapshot sets every vertex color to D3DRGBA(1,1,1,1) (line :848),
        // so all four corners are full white.
        for (int32_t k = 0; k < 4; ++k)
        {
            seg.color_a[k] = 1.0f;
            seg.color_b[k] = 1.0f;
        }
        seg.u_a = ta + u_scroll_;
        seg.u_b = tb + u_scroll_;
        seg_scratch.push_back(seg);
    }

    SStripDrawItem strip_item = {};
    strip_item.segments     = seg_scratch.data();
    strip_item.num_segments = int32_t(seg_scratch.size());
    strip_item.key.texture     = StripFamilyGlowTexture();
    strip_item.key.pipeline_id = uint16_t(EFxPipeline::Strip);
    // Snapshot: SetBlendState() enables ALPHABLENDENABLE with default
    // SRC_ALPHA/INV_SRC_ALPHA — but Render() in the snapshot is one
    // monolithic block with no SetAddBlendState mid-way for the strip
    // itself. Stripeffect.cpp's live port already migrated this to
    // AdditiveStraight (see kStripColor + EFxBlend::AdditiveStraight
    // comment). Preserve that here so the procedural-glow texture
    // (premultiplied) reads correctly.
    strip_item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    strip_item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    strip_item.debug_mode      = debug_mode;
    Renderer->SubmitFxStrip(strip_item);

    // Glow halo (snapshot stripeffect.cpp :861-902) — TWO billboard draws
    // counter-rotating at the caster hand. We stand them in with two
    // AdditiveStraight billboards offset slightly so a "rotating glow disc"
    // reads through the strip start.
    if (glow_scale_ > 0.005f)
    {
        const TTextureHandle spark_tex = StripFamilySparkTexture();
        if (spark_tex != kInvalidTexture)
        {
            // Snapshot: scale = glow_scale + random(0, 4f/10.0) (i.e. 0..0.4
            // jitter); we re-roll once per render. Placement is the bolt's
            // origin anchor.
            const float scl = glow_scale_ +
                              (float(random(0, 4)) / 10.0f);
            const float wx = anchors_[0].pos[0];
            const float wy = anchors_[0].pos[1];
            const float wz = anchors_[0].pos[2];
            for (int32_t pass = 0; pass < 2; ++pass)
            {
                SBillboardDrawItem item = {};
                item.world_pos[0] = wx;
                item.world_pos[1] = wy;
                item.world_pos[2] = wz;
                // Snapshot scales the i3d object by `scl` (uniform on x/y/z);
                // we pick a wu size that reads as a glow disc.
                const float size_wu = 24.0f * scl;
                item.size_wu[0] = size_wu;
                item.size_wu[1] = size_wu;
                item.color_rgba[0] = 1.0f;
                item.color_rgba[1] = 1.0f;
                item.color_rgba[2] = 1.0f;
                item.color_rgba[3] = 1.0f;
                item.uv_rect[0] = 0.0f;
                item.uv_rect[1] = 0.0f;
                item.uv_rect[2] = 1.0f;
                item.uv_rect[3] = 1.0f;
                item.key.texture     = spark_tex;
                item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
                item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
                item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
                item.light_mode      = EFxLightMode::Unlit;
                item.orientation     = EFxBillboardOrientation::ScreenAligned;
                item.debug_mode      = debug_mode;
                Renderer->SubmitFxBillboard(item);
            }
        }
    }

    // NOTE: spark + impact_spark sub-emitter passes are NOT ported
    // (forensics SPARKS_TSparkAnimator.md owns that piece). First-pass
    // sticks to the strip + glow.
}

// =========================================================================
// S05 TShockAnimator_Bespoke
// =========================================================================
// Snapshot source: src/effectcomp.cpp :609-770. The retail TShockAnimator
// is embedded in I21 TIceBoltAnimator at the target/caster end. Animator
// owns one ring TStripAnimator-shaped object with N vertices; per-tick
// scale_factor *= component-wise; when any axis crosses max_size, either
// flip to SHRINK or set done. Per-frame Render: rotate/scale/translate
// the object's matrix, optionally fade alpha by
// (max - scale)/(max - init), submit. We approximate the ring with N
// billboards arrayed in a circle on the world XY plane.

TShockAnimator_Bespoke* TShockAnimator_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    auto* eff = new TShockAnimator_Bespoke(static_cast<TObjectImagery*>(nullptr));
    eff->ForcePos(origin);
    eff->SetMapIndex(MapPane.MakeIndex());
    eff->ActivateComponents();

    // Snapshot Set() copies the incoming SShockParam and sets init_scale.
    // For the harness we use defaults consistent with the I21 ring usage:
    // start small, grow to ~50 wu over ~30 ticks (scale_factor 1.10/tick),
    // then shrink back via shrink_factor 0.92/tick. Pos lives at origin.
    // hmm_vec3 in this codebase uses uppercase .X/.Y/.Z (snapshot used lower).
    eff->pos_.X = float(origin.x);
    eff->pos_.Y = float(origin.y);
    eff->pos_.Z = float(origin.z) + 4.0f;
    eff->rot_   = {0.0f, 0.0f, 0.0f};
    eff->scale_         = {2.0f, 2.0f, 2.0f};         // start radius ~2 wu
    eff->scale_factor_  = {1.10f, 1.10f, 1.10f};
    eff->shrink_factor_ = {0.92f, 0.92f, 0.92f};
    eff->max_size_      = {50.0f, 50.0f, 50.0f};
    eff->min_size_      = {0.5f, 0.5f, 0.5f};
    eff->init_scale_    = eff->scale_;
    eff->flags_         = kFlagShrink | kFlagFade;
    eff->ring_color_[0] = 0.5f;
    eff->ring_color_[1] = 0.7f;
    eff->ring_color_[2] = 1.0f;
    eff->ring_color_[3] = 1.0f;
    eff->done_  = false;
    eff->grow_  = kGrow;

    log_info("[S05 shock_bespoke] SpawnForTest: origin=(%d,%d,%d) flags=%d "
             "init_scale=(%.2f,%.2f,%.2f) max=(%.2f,%.2f,%.2f)",
             origin.x, origin.y, origin.z, eff->flags_,
             double(eff->init_scale_.X), double(eff->init_scale_.Y), double(eff->init_scale_.Z),
             double(eff->max_size_.X), double(eff->max_size_.Y), double(eff->max_size_.Z));
    return eff;
}

void TShockAnimator_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate() (effectcomp.cpp :636-663):
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kStripFamilySimTickMs))
    {
        sim_accum_ms_ -= double(kStripFamilySimTickMs);
        if (done_)
            break;

        // scaling
        scale_.X *= scale_factor_.X;
        scale_.Y *= scale_factor_.Y;
        scale_.Z *= scale_factor_.Z;

        // check growth, and shrinking
        if ((scale_.X > max_size_.X || scale_.Y > max_size_.Y || scale_.Z > max_size_.Z)
            && grow_ == kGrow)
        {
            if (flags_ & kFlagShrink)
            {
                grow_ = kShrink;
                scale_factor_.X = shrink_factor_.X;
                scale_factor_.Y = shrink_factor_.Y;
                scale_factor_.Z = shrink_factor_.Z;
            }
            else
            {
                done_ = true;
            }
        }
        else if ((scale_.X < min_size_.X || scale_.Y < min_size_.Y || scale_.Z < min_size_.Z)
                 && grow_ == kShrink)
        {
            done_ = true;
        }
    }

    if (done_)
        return;

    // --- Render() (effectcomp.cpp :689-770):
    // Snapshot computes alpha_blend_factor = (max - scale) / (max - init) when
    // SHOCKWAVE_FLAG_FADE set, and multiplies the alpha by it. (RGB
    // commented out in snapshot.) We do the same.
    float alpha_blend_factor = 1.0f;
    if (flags_ & kFlagFade)
    {
        alpha_blend_factor = (max_size_.X - scale_.X) /
                             (max_size_.X - init_scale_.X);
        if (alpha_blend_factor < 0.0f) alpha_blend_factor = 0.0f;
        if (alpha_blend_factor > 1.0f) alpha_blend_factor = 1.0f;
    }

    // Snapshot draws ONE object with `ring_count * vertex_count` lverts.
    // We collapse to one ring of kRingVertices billboards on the world XY
    // plane, radius = scale.x * 10wu (the snapshot mesh shape isn't
    // directly accessible — first-pass approximation).
    const TTextureHandle spark_tex = StripFamilySparkTexture();
    if (spark_tex == kInvalidTexture)
        return;

    const float r = scale_.X * 10.0f;
    const float a = ring_color_[3] * alpha_blend_factor;
    for (int32_t j = 0; j < kRingVertices; ++j)
    {
        const float th = float(j) / float(kRingVertices) * 6.28318530718f;
        const float wx = pos_.X + std::cos(th) * r;
        const float wy = pos_.Y + std::sin(th) * r;
        const float wz = pos_.Z;

        SBillboardDrawItem item = {};
        item.world_pos[0] = wx;
        item.world_pos[1] = wy;
        item.world_pos[2] = wz;
        // Per-vertex size = a bit of the radius so consecutive billboards
        // overlap into a continuous ring.
        const float ring_segment_wu = r * 0.35f + 4.0f;
        item.size_wu[0] = ring_segment_wu;
        item.size_wu[1] = ring_segment_wu;
        item.color_rgba[0] = ring_color_[0];
        item.color_rgba[1] = ring_color_[1];
        item.color_rgba[2] = ring_color_[2];
        item.color_rgba[3] = a;
        item.uv_rect[0] = 0.0f;
        item.uv_rect[1] = 0.0f;
        item.uv_rect[2] = 1.0f;
        item.uv_rect[3] = 1.0f;
        item.key.texture     = spark_tex;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        // Snapshot Render() :756-758 sets SRCBLEND=SRC_ALPHA /
        // DESTBLEND=INV_SRC_ALPHA -> EFxBlend::Alpha. (Preserved verbatim;
        // do not reinterpret as Additive.)
        item.key.blend       = uint8_t(EFxBlend::Alpha);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        // Ground-plane ring -> WorldXY so it foreshortens correctly.
        item.orientation     = EFxBillboardOrientation::WorldXY;
        item.debug_mode      = debug_mode;
        Renderer->SubmitFxBillboard(item);
    }
}

// =========================================================================
// S07 TStreamerEffect_Bespoke
// =========================================================================
// Snapshot source: src/effect_old.cpp :10248-10434 (TStreamerAnimator).
// 4 streams (j=0..3), each per tick spawns STREAMER_SKIP*(4-j) new
// particles via InitStreamer(j); particle pos is sphere-mapped from
// (th, h) into (cos*r, sin*r, vertical), with modif = 0.5*(1+cos(2h+PI))
// for a smooth ring growth. Lifetime gated by frameon > STREAMER_DURATION.
// Render uses SetAddBlendState (AdditiveStraight).

TStreamerEffect_Bespoke* TStreamerEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    auto* eff = new TStreamerEffect_Bespoke(static_cast<TObjectImagery*>(nullptr));
    eff->ForcePos(origin);
    eff->SetMapIndex(MapPane.MakeIndex());
    eff->ActivateComponents();

    // Snapshot Initialize() (effect_old.cpp :10297-10322):
    eff->frameon_ = 0;
    for (int32_t j = 0; j < kStreamerMaxStreams; ++j)
    {
        eff->scl_init_[j] = 0.15f * float(j + 1);
        eff->dscl_[j]     = eff->scl_init_[j] / float(kStreamerMaxParticles);
        eff->h_[j]        = 0.0f;
        eff->dh_[j]       = 0.02f / float(4 - j);
        eff->th_[j]       = 0.0f;
        eff->dth_[j]      = 0.15f;
        for (int32_t i = 0; i < kStreamerMaxParticles; ++i)
            eff->stream_[j][i].count = 0;
    }
    eff->alive_ = true;

    log_info("[S07 streamer_bespoke] SpawnForTest: origin=(%d,%d,%d)",
             origin.x, origin.y, origin.z);
    return eff;
}

// Snapshot InitStreamer (effect_old.cpp :10276-10295) — adds one particle
// to stream x at the current (th, h) parametric point, advances angles,
// short-circuits frameon if h crosses PI.
void TStreamerEffect_Bespoke::InitStreamerSnap(int32_t x)
{
    // hmm_vec3 in this codebase uses uppercase .X/.Y/.Z (snapshot used lower).
    hmm_vec3 pos = {0.0f, 0.0f, 0.0f};
    const float modif = float(0.5 * (1.0 + std::cos(2.0 * double(h_[x]) + M_PI)));
    pos.X = float(kStreamerModifier) * std::cos(th_[x]) * modif *
            float(x + 1) / float(kStreamerMaxStreams);
    pos.Y = float(kStreamerModifier) * std::sin(th_[x]) * modif *
            float(x + 1) / float(kStreamerMaxStreams);
    pos.Z = float(kStreamerModifierV) * (std::cos(h_[x]) + 1.0f);
    h_[x] += dh_[x];
    if (h_[x] > float(M_PI))
    {
        // h_[x] -= M_PI;  (commented out in snapshot)
        frameon_ = kStreamerDuration + 1;
    }
    if (h_[x] > float(2.0 * M_PI))
        h_[x] -= float(2.0 * M_PI);
    th_[x] += dth_[x];
    if (th_[x] > float(2.0 * M_PI))
        th_[x] -= float(2.0 * M_PI);
    AddStreamerSnap(x, pos, scl_init_[x] * modif);
}

// Snapshot AddStreamer (effect_old.cpp :10324-10347) — finds the LAST
// dead slot (snapshot does `if (count <= 0) p = i` in a forward sweep,
// so p ends as the last free index), populates it, then decrements all
// counts and scales for the whole stream.
void TStreamerEffect_Bespoke::AddStreamerSnap(int32_t num, const hmm_vec3& pos, float scl)
{
    int32_t p = -1;
    for (int32_t i = 0; i < kStreamerMaxParticles; ++i)
    {
        if (stream_[num][i].count <= 0)
            p = i;
    }
    if (p > -1)
    {
        stream_[num][p].pos.X = pos.X;
        stream_[num][p].pos.Y = pos.Y;
        stream_[num][p].pos.Z = pos.Z;
        stream_[num][p].scl = scl;
        stream_[num][p].count = kStreamerMaxParticles;
    }
    for (int32_t i = 0; i < kStreamerMaxParticles; ++i)
    {
        stream_[num][i].count--;
        stream_[num][i].scl -= dscl_[num];
    }
}

void TStreamerEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate() (effect_old.cpp :10356-10381):
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kStripFamilySimTickMs))
    {
        sim_accum_ms_ -= double(kStripFamilySimTickMs);
        if (!alive_)
            break;
        frameon_++;
        for (int32_t j = 0; j < kStreamerMaxStreams; ++j)
        {
            for (int32_t i = 0; i < kStreamerSkip * (4 - j); ++i)
                InitStreamerSnap(j);
        }
        if (frameon_ > kStreamerDuration)
        {
            alive_ = false;
            break;
        }
    }

    if (!alive_)
        return;

    // --- Render() (effect_old.cpp :10390-10434):
    const TTextureHandle spark_tex = StripFamilySparkTexture();
    if (spark_tex == kInvalidTexture)
        return;

    const float ox = float(Pos().x);
    const float oy = float(Pos().y);
    const float oz = float(Pos().z);
    for (int32_t j = 0; j < kStreamerMaxStreams; ++j)
    {
        for (int32_t i = 0; i < kStreamerMaxParticles; ++i)
        {
            const SStreamerParticleEx& sp = stream_[j][i];
            if (sp.count <= 0 || sp.scl <= 0.0f)
                continue;
            // Snapshot Render: ScaleMatrix(scl), RotateX(-PI/2)+RotateX(-PI/6)+
            // RotateZ(-PI/4)+RotateZ(-face*TORADIAN). The orientation rotations
            // are baked into "face the camera flat", which billboards already
            // do; we use ScreenAligned + the snapshot's pos.
            SBillboardDrawItem item = {};
            item.world_pos[0] = ox + sp.pos.X;
            item.world_pos[1] = oy + sp.pos.Y;
            item.world_pos[2] = oz + sp.pos.Z;
            const float size_wu = sp.scl * 12.0f;   // imagery scale -> wu
            item.size_wu[0] = size_wu;
            item.size_wu[1] = size_wu;
            item.color_rgba[0] = 1.0f;
            item.color_rgba[1] = 1.0f;
            item.color_rgba[2] = 1.0f;
            item.color_rgba[3] = 1.0f;
            item.uv_rect[0] = 0.0f;
            item.uv_rect[1] = 0.0f;
            item.uv_rect[2] = 1.0f;
            item.uv_rect[3] = 1.0f;
            item.key.texture     = spark_tex;
            item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
            // Snapshot Render() :10395-10396 SetAddBlendState -> AdditiveStraight.
            item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
            item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
            item.light_mode      = EFxLightMode::Unlit;
            item.orientation     = EFxBillboardOrientation::ScreenAligned;
            item.debug_mode      = debug_mode;
            Renderer->SubmitFxBillboard(item);
        }
    }
}

// =========================================================================
// X11 TRibbonAnimator_Bespoke
// =========================================================================
// Snapshot source: src/effect_old.cpp :4069-4321. The revive halo's
// composite: NUM_RIBBON_SPARKS=30 small sparks orbiting in a column above
// the ribbon position, plus a slow-growing center spark at the floor,
// plus NUM_RIBBONS=3 rotating ribbons. Pre-release reads
// (PTReviveEffect)inst->stage / ->target — neither exists in the harness,
// so we land in the "no spell" branch (effect_old.cpp :4180-4186, grow
// ribscale to RIBBON_MAXSCALE then hold).

TRibbonAnimator_Bespoke* TRibbonAnimator_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    auto* eff = new TRibbonAnimator_Bespoke(static_cast<TObjectImagery*>(nullptr));
    eff->ForcePos(origin);
    eff->SetMapIndex(MapPane.MakeIndex());
    eff->ActivateComponents();

    // Snapshot Initialize() (effect_old.cpp :4084-4134):
    // numtexframes from imagery (NOT bound; default 1).
    // hmm_vec3 in this codebase uses uppercase .X/.Y/.Z (snapshot used lower).
    eff->ribbontimer_ = 0;
    eff->ribpos_.X = float(origin.x);
    eff->ribpos_.Y = float(origin.y);
    eff->ribpos_.Z = float(origin.z);
    eff->ribscale_ = 0.05f;   // RIBBON_MINSCALE
    for (int32_t n = 0; n < NUM_RIBBONS; ++n)
        eff->rotation_[n] = float(n) * 2.0f;
    for (int32_t n = 0; n < NUM_RIBBON_SPARKS; ++n)
    {
        eff->p_[n].X = float(random(-RIBBON_RADIUS, RIBBON_RADIUS));
        eff->p_[n].Y = float(random(-RIBBON_RADIUS, RIBBON_RADIUS));
        eff->p_[n].Z = float(random(0, RIBBON_RADIUS / 2));
        eff->v_[n].X = -eff->p_[n].X / float(RIBBON_SPARK_DURATION) / 2.0f;
        eff->v_[n].Y = -eff->p_[n].Y / float(RIBBON_SPARK_DURATION) / 2.0f;
        eff->v_[n].Z = 0.5f;
        eff->scale_spark_[n] = 0.0f;
        eff->framenum_[n] = random(-NUM_RIBBON_SPARKS, 0);
    }
    eff->centertilt_     = 0.0f;
    eff->centertilt_dx_  = 0.001f;
    eff->alive_          = true;

    log_info("[X11 ribbon_bespoke] SpawnForTest: origin=(%d,%d,%d)",
             origin.x, origin.y, origin.z);
    return eff;
}

void TRibbonAnimator_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer)
        return;

    // --- Animate() (effect_old.cpp :4143-4235):
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kStripFamilySimTickMs))
    {
        sim_accum_ms_ -= double(kStripFamilySimTickMs);
        if (!alive_)
            break;
        ribbontimer_++;
        // Rotate ribbons (snapshot :4157).
        for (int32_t n = 0; n < NUM_RIBBONS; ++n)
            rotation_[n] += 0.2f;
        // No PTReviveEffect in harness — take the else-branch (snapshot
        // :4180-4186): grow ribscale to RIBBON_MAXSCALE.
        const float RIBBON_MAXSCALE = 4.5f;
        const float RIBBON_MINSCALE = 0.05f;
        const float RIBBON_SCALEINC =
            (RIBBON_MAXSCALE - RIBBON_MINSCALE) / float(kRibbonGrowFrames);
        if (ribscale_ <= RIBBON_MAXSCALE)
            ribscale_ += RIBBON_SCALEINC;

        // Update all ribbon sparks (snapshot :4191-4228).
        for (int32_t n = 0; n < NUM_RIBBON_SPARKS; ++n)
        {
            framenum_[n]++;
            if (framenum_[n] > 0)
            {
                if (framenum_[n] >= RIBBON_SPARK_DURATION)
                {
                    p_[n].X = float(random(-RIBBON_RADIUS, RIBBON_RADIUS));
                    p_[n].Y = float(random(-RIBBON_RADIUS, RIBBON_RADIUS));
                    p_[n].Z = float(random(0, RIBBON_RADIUS / 2));
                    v_[n].X = -p_[n].X / float(RIBBON_SPARK_DURATION) / 2.0f;
                    v_[n].Y = -p_[n].Y / float(RIBBON_SPARK_DURATION) / 2.0f;
                    v_[n].Z = 0.5f;
                    framenum_[n] = 0;
                    scale_spark_[n] = 0.0f;
                }
                else if (framenum_[n] > RIBBON_SPARK_DURATION / 2)
                    scale_spark_[n] -= float(RIBBON_SPARK_SCALE_STEP);
                else
                    scale_spark_[n] += float(RIBBON_SPARK_SCALE_STEP);
                p_[n].X += v_[n].X;
                p_[n].Y += v_[n].Y;
                p_[n].Z += v_[n].Z;
                v_[n].Z += 0.5f;
            }
        }
        centertilt_ += centertilt_dx_;
        if (centertilt_ > 0.25f || centertilt_ < -0.25f)
            centertilt_dx_ = -centertilt_dx_;

        // Harness self-kill — snapshot relies on PTReviveEffect to KillThisEffect;
        // we use a fixed one-shot duration so the Combat-cadence harness can
        // re-trigger cleanly.
        if (ribbontimer_ >= kRibbonLifeTicks)
        {
            alive_ = false;
            break;
        }
    }

    if (!alive_)
        return;

    // --- Render() (effect_old.cpp :4244-4321):
    const TTextureHandle spark_tex = StripFamilySparkTexture();
    if (spark_tex == kInvalidTexture)
        return;

    const float ox = ribpos_.X;
    const float oy = ribpos_.Y;
    const float oz = ribpos_.Z;

    // 1) Ribbon sparks (snapshot :4257-4277). For each spark with framenum>0
    // and scale>0, draw the obj at p_[n] with scale_spark_[n].
    for (int32_t n = 0; n < NUM_RIBBON_SPARKS; ++n)
    {
        if (framenum_[n] > 0 && scale_spark_[n] > 0.0f)
        {
            SBillboardDrawItem item = {};
            item.world_pos[0] = ox + p_[n].X;
            item.world_pos[1] = oy + p_[n].Y;
            item.world_pos[2] = oz + p_[n].Z;
            const float size_wu = scale_spark_[n] * 16.0f;
            item.size_wu[0] = size_wu;
            item.size_wu[1] = size_wu;
            item.color_rgba[0] = 1.0f;
            item.color_rgba[1] = 1.0f;
            item.color_rgba[2] = 1.0f;
            item.color_rgba[3] = 1.0f;
            item.uv_rect[0] = 0.0f;
            item.uv_rect[1] = 0.0f;
            item.uv_rect[2] = 1.0f;
            item.uv_rect[3] = 1.0f;
            item.key.texture     = spark_tex;
            item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
            // Snapshot Render() :4248-4249 SetBlendState -> Alpha.
            item.key.blend       = uint8_t(EFxBlend::Alpha);
            item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
            item.light_mode      = EFxLightMode::Unlit;
            item.orientation     = EFxBillboardOrientation::ScreenAligned;
            item.debug_mode      = debug_mode;
            Renderer->SubmitFxBillboard(item);
        }
    }

    // 2) Center floor spark (snapshot :4283-4294).
    {
        const float scl = ribscale_ * 2.5f + (float(random(0, 10)) / 20.0f);
        SBillboardDrawItem item = {};
        item.world_pos[0] = ox;
        item.world_pos[1] = oy;
        item.world_pos[2] = oz;
        const float size_wu = scl * 12.0f;
        item.size_wu[0] = size_wu;
        item.size_wu[1] = size_wu;
        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 1.0f;
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = 0.0f;
        item.uv_rect[1] = 0.0f;
        item.uv_rect[2] = 1.0f;
        item.uv_rect[3] = 1.0f;
        item.key.texture     = spark_tex;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(EFxBlend::Alpha);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        item.orientation     = EFxBillboardOrientation::WorldXY;
        item.debug_mode      = debug_mode;
        Renderer->SubmitFxBillboard(item);
    }

    // 3) Ribbons (snapshot :4301-4316). Three rotating ribbons sitting at
    // ribpos. Each ribbon's z is scaled 2x x/y (snapshot :4308-4309); we
    // stand them in as three world-XY-tipped billboards offset upward,
    // rotating around ribpos via rotation_[n].
    for (int32_t n = 0; n < NUM_RIBBONS; ++n)
    {
        const float ang = rotation_[n];
        const float offs = ribscale_ * 8.0f;
        SBillboardDrawItem item = {};
        item.world_pos[0] = ox + std::cos(ang) * offs;
        item.world_pos[1] = oy + std::sin(ang) * offs;
        item.world_pos[2] = oz + ribscale_ * 10.0f;
        const float ribbon_size_wu = ribscale_ * 16.0f;
        item.size_wu[0] = ribbon_size_wu;
        item.size_wu[1] = ribbon_size_wu * 2.0f;       // snapshot scl.z = 2x scl.x/y
        item.color_rgba[0] = 0.9f;
        item.color_rgba[1] = 0.85f;
        item.color_rgba[2] = 0.55f;                    // pale gold
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = 0.0f;
        item.uv_rect[1] = 0.0f;
        item.uv_rect[2] = 1.0f;
        item.uv_rect[3] = 1.0f;
        item.key.texture     = spark_tex;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(EFxBlend::Alpha);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::Unlit;
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = debug_mode;
        Renderer->SubmitFxBillboard(item);
    }
}

// *************************************************************************
// * TWaterFallEffect_Bespoke (H02) + TWaterEffect_Bespoke (H01)            *
// *   first-pass faithful direct ports of the snapshot TWaterFallAnimator  *
// *   and TWaterAnimator render/integration bodies.                        *
// *                                                                        *
// * Source of truth:                                                       *
// *   - TWaterFallAnimator: src/effect_old.cpp:11685-11873                 *
// *   - TWaterAnimator:     src/effect_old.cpp:11895-12073                 *
// *                                                                        *
// * Render path note: the snapshot Render() bodies for BOTH effects        *
// * write D3DRENDERSTATE_SRCBLEND=ONE / DESTBLEND=ONE (additive accumulate)*
// * with the SetAddBlendState() call commented out. We honour the WRITTEN  *
// * factor pair verbatim — EFxBlend::AdditiveStraight (ONE/ONE). Per       *
// * orchestrator brief: "PRESERVE the snapshot's blend mode AS WRITTEN.    *
// * Don't reinterpret."                                                    *
// *                                                                        *
// * Asset: both classes share `misc\Water.i3d` (imagery.rvi confirms;      *
// * H01 entry maps "Water" -> misc\Water.i3d, H02 entry "Waterfall" ->     *
// * misc\Water.i3d). Single sub-object, single texture — billboard the    *
// * drop sprite per particle. Snapshot rendered with full D3D matrices    *
// * (RotateZ -π/4 + per-instance face rotation); we collapse to a         *
// * screen-aligned billboard for first-pass since the rotation knob isn't *
// * on SBillboardDrawItem.                                                 *
// *************************************************************************

namespace {

constexpr const char* kWaterImageryPath = "misc\\Water.i3d";   // shared H01/H02

// Snapshot constants — direct from effect_old.cpp:11693-11699.
constexpr int32_t kWaterFallMaxDrops  = 100;
constexpr int32_t kWaterFallLength    = 64;
constexpr int32_t kWaterFallHeight    = 64;
constexpr float   kWaterFallScale     = 0.1f;
constexpr float   kWaterFallGravity   = 0.25f;
constexpr float   kWaterFallTallScale = 2.0f;

// Snapshot constants — direct from effect_old.cpp:11903-11909.
constexpr int32_t kWaterMaxDrops      = 25;
constexpr int32_t kWaterLength        = 64;
constexpr int32_t kWaterWidth         = 64;
constexpr float   kWaterScale         = 0.2f;
constexpr float   kWaterTallScale     = 0.1f;
constexpr float   kWaterSpeed         = 2.0f;

// 24 Hz sim-tick gate — every animator on the project ports to this.
constexpr int32_t kWaterSimTickMs     = 1000 / 24;

// First-pass billboard size in world units (drop billboard, ~16 wu base
// scaled per-particle by drops[i].scale.x for visual variance). The
// snapshot drove this through D3DMATRIXScale + Render with vertices in
// a unit box mesh; we collapse to a single billboard scaled by an
// average of (scale.x, scale.y) so the per-drop fall-time stretch is
// approximated rather than dropped.
constexpr float   kWaterBaseSizeWu    = 16.0f;

// Resolve the .I3D's first nonzero texture slot via the shared helper.
// (SubObjTextureSlot / ResolveSubObjUv are file-static in the blood
// namespace above; re-declared as forward-only refs here.)
//
// Forward refs into the blood namespace above. Re-implement small
// wrappers in this anonymous namespace to keep coupling explicit and
// avoid yanking the blood namespace open here.
int32_t WaterSubObjTextureSlot(T3DImagery* img3d, int32_t objnum)
{
    if (!img3d || objnum < 0 || objnum >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(objnum);
    if (nfaces <= 0) return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[8 + 1] = {};
    int32_t numtexfaces[8 + 1] = {};
    img3d->GetObjFaces(objnum, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

void WaterResolveSubObjUv(T3DImagery* img3d, int32_t objnum, float out[4])
{
    out[0] = 0.0f; out[1] = 0.0f; out[2] = 1.0f; out[3] = 1.0f;
    if (!img3d) return;
    const int32_t nverts = img3d->NumObjVerts(objnum);
    if (nverts <= 0) return;
    std::vector<S3DVertex> vbuf(static_cast<size_t>(nverts), S3DVertex{});
    img3d->GetObjVerts(objnum, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
    float minu = vbuf[0].tu, maxu = vbuf[0].tu;
    float minv = vbuf[0].tv, maxv = vbuf[0].tv;
    for (int32_t i = 1; i < nverts; ++i)
    {
        if (vbuf[i].tu < minu) minu = vbuf[i].tu;
        if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
        if (vbuf[i].tv < minv) minv = vbuf[i].tv;
        if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
    }
    out[0] = minu;
    out[1] = minv;
    out[2] = maxu - minu;
    out[3] = maxv - minv;
}

// Load Misc\Water.i3d, resolve sub-object 0's texture/UV. Shared by
// both effects (imagery.rvi entries "Water" and "Waterfall" both point
// at misc\Water.i3d).
bool LoadWaterImagery(TObjectImagery*& out_base,
                      T3DImagery*&    out_img3d,
                      TTextureHandle& out_texture,
                      float           out_uv_rect[4])
{
    out_base = nullptr;
    out_img3d = nullptr;
    out_texture = kInvalidTexture;
    out_uv_rect[0] = 0.0f; out_uv_rect[1] = 0.0f;
    out_uv_rect[2] = 1.0f; out_uv_rect[3] = 1.0f;

    const int32_t img_id = TObjectImagery::FindImagery(kWaterImageryPath);
    if (img_id < 0)
    {
        log_error("[water] LoadWaterImagery: FindImagery('%s') failed",
                  kWaterImageryPath);
        return false;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[water] LoadWaterImagery: LoadImagery(id=%d '%s') failed",
                  img_id, kWaterImageryPath);
        return false;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[water] LoadWaterImagery: imagery '%s' is not T3DImagery",
                  kWaterImageryPath);
        TObjectImagery::FreeImagery(base);
        return false;
    }
    // Lazy mesh-load poke pattern (B01/F01/H04/X22).
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();
    if (num_obj <= 0 || num_tex <= 0)
    {
        log_error("[water] LoadWaterImagery: imagery underspec'd "
                  "(objects=%d, textures=%d)", num_obj, num_tex);
        return false;
    }
    const int32_t tex_slot = WaterSubObjTextureSlot(img3d, 0);
    const int32_t slot = (tex_slot >= 0 && tex_slot < num_tex) ? tex_slot : 0;
    S3DTex tex = {};
    img3d->GetTexture(slot, &tex);
    out_base = base;
    out_img3d = img3d;
    out_texture = tex.htexture;
    WaterResolveSubObjUv(img3d, 0, out_uv_rect);
    return true;
}

}   // namespace

// =========================================================================
// * H02 — TWaterFallEffect_Bespoke                                         *
// =========================================================================

TWaterFallEffect_Bespoke::~TWaterFallEffect_Bespoke()
{
    delete[] drops_;
    drops_ = nullptr;
}

void TWaterFallEffect_Bespoke::InitParticle(int32_t i)
{
    // VERBATIM port of TWaterFallAnimator::InitParticle
    // (src/effect_old.cpp:11701-11715).
    float grav = (float)(kWaterFallGravity * ((i % 2) + 1));
    drops_[i].pos.X = (float)(random(-kWaterFallLength, kWaterFallLength) / 2.0);
    drops_[i].pos.Y = (float)0.0f;
    drops_[i].pos.Z = (float)kWaterFallHeight;
    drops_[i].vel.X = (float)0.0f;
    drops_[i].vel.Y = (float)0.0f;
    drops_[i].vel.Z = (float)0.0f;
    drops_[i].scale.X = (float)(random(2, 4) * kWaterFallScale);
    drops_[i].scale.Y = (float)(kWaterFallTallScale * grav
        * std::sqrt(2 * (kWaterFallHeight - drops_[i].pos.Z) / grav));
    drops_[i].scale.Z = (float)1.0f;
    drops_[i].time = i;
}

void TWaterFallEffect_Bespoke::UpdateStuff()
{
    // VERBATIM port of TWaterFallAnimator::UpdateStuff
    // (src/effect_old.cpp:11717-11745).
    int32_t i;
    for (i = 0; i < numdrops_; i++)
    {
        if (drops_[i].time > 0)
        {
            drops_[i].time--;
            continue;
        }
        if (drops_[i].time == -1)
        {
            InitParticle(i);
            drops_[i].pos.Z = (float)kWaterFallHeight;
            drops_[i].vel.Z = 0.0f;
            drops_[i].time = 0;
        }
        drops_[i].pos.X += drops_[i].vel.X;
        drops_[i].pos.Y += drops_[i].vel.Y;
        drops_[i].pos.Z += drops_[i].vel.Z;
        float grav = (float)(kWaterFallGravity * ((i % 2) + 1));
        drops_[i].vel.Z -= grav;
        drops_[i].scale.Y = (float)(kWaterFallTallScale * grav
            * std::sqrt(2 * (kWaterFallHeight - drops_[i].pos.Z) / grav));
        if (drops_[i].pos.Z <= 0)
        {
            drops_[i].time = -1;
        }
    }
}

TWaterFallEffect_Bespoke*
TWaterFallEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    TObjectImagery* base = nullptr;
    T3DImagery*     img3d = nullptr;
    TTextureHandle  texture = kInvalidTexture;
    float           uv_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    if (!LoadWaterImagery(base, img3d, texture, uv_rect))
        return nullptr;

    auto* wf = new TWaterFallEffect_Bespoke(base);
    wf->ForcePos(origin);
    wf->SetMapIndex(MapPane.MakeIndex());
    wf->ActivateComponents();

    wf->texture_ = texture;
    wf->uv_rect_[0] = uv_rect[0];
    wf->uv_rect_[1] = uv_rect[1];
    wf->uv_rect_[2] = uv_rect[2];
    wf->uv_rect_[3] = uv_rect[3];
    wf->size_wu_ = kWaterBaseSizeWu;

    // VERBATIM port of TWaterFallAnimator::Initialize
    // (src/effect_old.cpp:11747-11767).
    //   inst->GetPos(eff) -> the effect base position; here Pos() suffices.
    //   numdrops = WATERFALL_MAXDROPS;
    //   drops = new SWaterParticle[numdrops];
    //   for (i = 0..numdrops) InitParticle(i);
    //   for (k = 0..numdrops) UpdateStuff();        // warmup pass
    wf->numdrops_ = kWaterFallMaxDrops;
    wf->drops_ = new SWaterParticle[wf->numdrops_]();
    for (int32_t i = 0; i < wf->numdrops_; i++)
        wf->InitParticle(i);
    // Warm-up: snapshot runs UpdateStuff `numdrops` times to seed a
    // staggered fall pattern (the per-particle `time` index dictates
    // when each drop "starts" falling). Preserve verbatim.
    for (int32_t k = 0; k < wf->numdrops_; k++)
        wf->UpdateStuff();

    log_info("[waterfall] SpawnForTest_BESPOKE: '%s' map_index=%d "
             "origin=(%d,%d,%d) drops=%d texture=%u uv=(%.3f,%.3f,%.3f,%.3f)",
             kWaterImageryPath, wf->GetMapIndex(),
             origin.x, origin.y, origin.z,
             wf->numdrops_, wf->texture_,
             wf->uv_rect_[0], wf->uv_rect_[1],
             wf->uv_rect_[2], wf->uv_rect_[3]);
    return wf;
}

void TWaterFallEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer || !drops_)
        return;

    // 24 Hz sim-tick gate — snapshot UpdateStuff was once per Animate
    // (framerate-locked); we run the per-tick integration once per sim
    // tick of accumulated wall-clock time so motion is framerate-indep.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kWaterSimTickMs))
    {
        sim_accum_ms_ -= double(kWaterSimTickMs);
        UpdateStuff();
    }

    // VERBATIM port of TWaterFallAnimator::Render
    // (src/effect_old.cpp:11813-11860). Snapshot sets:
    //   D3DRENDERSTATE_SRCBLEND  = D3DBLEND_ONE
    //   D3DRENDERSTATE_DESTBLEND = D3DBLEND_ONE
    // -> AdditiveStraight (ONE/ONE) verbatim. ZWRITE=false ZENABLE=true
    // -> EFxDepthMode::TestNoWrite.
    static const bool s_logged_first_submit = []{
        log_info("[waterfall] first submit (TickAndSubmit running)");
        return true;
    }();
    (void)s_logged_first_submit;

    if (texture_ == kInvalidTexture)
        return;

    const S3DPoint& base = Pos();
    for (int32_t i = 0; i < numdrops_; i++)
    {
        if (drops_[i].time != 0)
            continue;
        // Snapshot composes the matrix as:
        //   Scale(scale.x, scale.y, scale.z)
        //   RotateZ(-π/4)
        //   RotateZ(-(inst.face * 360 / 256) * TORADIAN)
        //   Translate(pos)
        // For first-pass we collapse to a screen-aligned billboard.
        // Per-drop size = base_size_wu * average(scale.x, scale.y) so
        // the per-particle tall-y stretch is approximated. The −π/4
        // and inst-face rotations are not on SBillboardDrawItem yet —
        // tracked under "drift adaptation" (no rotation knob in API).
        const float wx = float(base.x) + drops_[i].pos.X;
        const float wy = float(base.y) + drops_[i].pos.Y;
        const float wz = float(base.z) + drops_[i].pos.Z;

        SBillboardDrawItem item = {};
        // Stretch the billboard along Y (vertical) by scale.y, X by scale.x.
        item.size_wu[0] = size_wu_ * drops_[i].scale.X;
        item.size_wu[1] = size_wu_ * drops_[i].scale.Y;
        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 1.0f;
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = uv_rect_[0];
        item.uv_rect[1] = uv_rect_[1];
        item.uv_rect[2] = uv_rect_[2];
        item.uv_rect[3] = uv_rect_[3];
        item.key.texture     = texture_;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        // Snapshot SRCBLEND/DESTBLEND = ONE/ONE — AdditiveStraight verbatim.
        item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::LitFlat;  // snapshot DoLighting
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = debug_mode;
        item.world_pos[0]    = wx;
        item.world_pos[1]    = wy;
        item.world_pos[2]    = wz;
        Renderer->SubmitFxBillboard(item);
    }
}

// =========================================================================
// * H01 — TWaterEffect_Bespoke                                             *
// =========================================================================

TWaterEffect_Bespoke::~TWaterEffect_Bespoke()
{
    delete[] drops_;
    drops_ = nullptr;
}

void TWaterEffect_Bespoke::InitParticle(int32_t i)
{
    // VERBATIM port of TWaterAnimator::InitParticle
    // (src/effect_old.cpp:11911-11923).
    drops_[i].pos.X = (float)(-kWaterLength / 2);
    drops_[i].pos.Y = (float)(random(-kWaterWidth, kWaterWidth) / 2.0);
    drops_[i].pos.Z = (float)0.0f;
    drops_[i].vel.X = (float)(kWaterSpeed * ((i % 2) + 1));
    drops_[i].vel.Y = (float)0.0f;
    drops_[i].vel.Z = (float)0.0f;
    drops_[i].scale.X = (float)(random(17, 30) * kWaterTallScale);
    drops_[i].scale.Y = (float)(random(2, 4) * kWaterScale);
    drops_[i].scale.Z = (float)1.0f;
    drops_[i].time = i;
}

void TWaterEffect_Bespoke::UpdateStuff()
{
    // VERBATIM port of TWaterAnimator::UpdateStuff
    // (src/effect_old.cpp:11925-11948).
    int32_t i;
    for (i = 0; i < numdrops_; i++)
    {
        if (drops_[i].time > 0)
        {
            drops_[i].time--;
            continue;
        }
        if (drops_[i].time == -1)
        {
            InitParticle(i);
            drops_[i].time = 0;
        }
        drops_[i].pos.X += drops_[i].vel.X;
        drops_[i].pos.Y += drops_[i].vel.Y;
        drops_[i].pos.Z += drops_[i].vel.Z;
        if (drops_[i].pos.X >= kWaterLength / 2)
        {
            drops_[i].time = -1;
        }
    }
}

TWaterEffect_Bespoke*
TWaterEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    TObjectImagery* base = nullptr;
    T3DImagery*     img3d = nullptr;
    TTextureHandle  texture = kInvalidTexture;
    float           uv_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    if (!LoadWaterImagery(base, img3d, texture, uv_rect))
        return nullptr;

    auto* w = new TWaterEffect_Bespoke(base);
    w->ForcePos(origin);
    w->SetMapIndex(MapPane.MakeIndex());
    w->ActivateComponents();

    w->texture_ = texture;
    w->uv_rect_[0] = uv_rect[0];
    w->uv_rect_[1] = uv_rect[1];
    w->uv_rect_[2] = uv_rect[2];
    w->uv_rect_[3] = uv_rect[3];
    w->size_wu_ = kWaterBaseSizeWu;

    // VERBATIM port of TWaterAnimator::Initialize
    // (src/effect_old.cpp:11950-11970). Same warmup pattern as H02.
    w->numdrops_ = kWaterMaxDrops;
    w->drops_ = new SWaterParticle[w->numdrops_]();
    for (int32_t i = 0; i < w->numdrops_; i++)
        w->InitParticle(i);
    for (int32_t k = 0; k < w->numdrops_; k++)
        w->UpdateStuff();

    log_info("[water] SpawnForTest_BESPOKE: '%s' map_index=%d "
             "origin=(%d,%d,%d) drops=%d texture=%u uv=(%.3f,%.3f,%.3f,%.3f)",
             kWaterImageryPath, w->GetMapIndex(),
             origin.x, origin.y, origin.z,
             w->numdrops_, w->texture_,
             w->uv_rect_[0], w->uv_rect_[1],
             w->uv_rect_[2], w->uv_rect_[3]);
    return w;
}

void TWaterEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
{
    if (!Renderer || !drops_)
        return;

    // 24 Hz sim-tick gate — same family pattern as H02 / B01 / M05.
    sim_accum_ms_ += TTime::DeltaTime() * 1000.0;
    while (sim_accum_ms_ >= double(kWaterSimTickMs))
    {
        sim_accum_ms_ -= double(kWaterSimTickMs);
        UpdateStuff();
    }

    // VERBATIM port of TWaterAnimator::Render
    // (src/effect_old.cpp:12016-12062). Snapshot sets:
    //   D3DRENDERSTATE_SRCBLEND  = D3DBLEND_ONE
    //   D3DRENDERSTATE_DESTBLEND = D3DBLEND_ONE
    // -> AdditiveStraight (ONE/ONE) verbatim. ZWRITE=false ZENABLE=true
    // -> EFxDepthMode::TestNoWrite. The Render() body for TWaterAnimator
    // commented out ALL three rotation lines (RotateX, RotateZ -π/4,
    // RotateZ inst-face) — so the snapshot already collapses to axis-
    // aligned which maps directly to ScreenAligned billboard.
    static const bool s_logged_first_submit = []{
        log_info("[water] first submit (TickAndSubmit running)");
        return true;
    }();
    (void)s_logged_first_submit;

    if (texture_ == kInvalidTexture)
        return;

    const S3DPoint& base = Pos();
    for (int32_t i = 0; i < numdrops_; i++)
    {
        if (drops_[i].time != 0)
            continue;
        const float wx = float(base.x) + drops_[i].pos.X;
        const float wy = float(base.y) + drops_[i].pos.Y;
        const float wz = float(base.z) + drops_[i].pos.Z;

        SBillboardDrawItem item = {};
        // Snapshot: scale.x tall (long stream), scale.y narrow (thin spray).
        item.size_wu[0] = size_wu_ * drops_[i].scale.X;
        item.size_wu[1] = size_wu_ * drops_[i].scale.Y;
        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 1.0f;
        item.color_rgba[3] = 1.0f;
        item.uv_rect[0] = uv_rect_[0];
        item.uv_rect[1] = uv_rect_[1];
        item.uv_rect[2] = uv_rect_[2];
        item.uv_rect[3] = uv_rect_[3];
        item.key.texture     = texture_;
        item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
        item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
        item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
        item.light_mode      = EFxLightMode::LitFlat;
        item.orientation     = EFxBillboardOrientation::ScreenAligned;
        item.debug_mode      = debug_mode;
        item.world_pos[0]    = wx;
        item.world_pos[1]    = wy;
        item.world_pos[2]    = wz;
        Renderer->SubmitFxBillboard(item);
    }
}
