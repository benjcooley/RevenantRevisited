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
    if (float* df = bucket.VarPtr(particle_index, EParticleVar::DrawFrame))
        *df = 0.0f;
    if (float* uv = bucket.VarPtr(particle_index, EParticleVar::DrawUvRect))
    { uv[0] = 0.0f; uv[1] = 0.0f; uv[2] = 1.0f; uv[3] = 1.0f; }
    if (float* age = bucket.VarPtr(particle_index, EParticleVar::Age))
        *age = 0.0f;
    if (float* age01 = bucket.VarPtr(particle_index, EParticleVar::Age01))
        *age01 = 0.0f;
    // Seed: deterministic-per-slot mix of particle_index + bucket_def address.
    if (float* seed = bucket.VarPtr(particle_index, EParticleVar::Seed))
    {
        uint32_t s = uint32_t(particle_index) * 2654435761u
                   + uint32_t(uintptr_t(&bucket_def) & 0xffffffffu) * 16807u;
        if (s == 0) s = 1;
        *seed = float(s & 0x7fffffu) / float(0x7fffffu);
    }

    // Now run the user spawn expression (statement-form). Identifier
    // aliases pos/vel/color/scale etc. resolve against the bucket slots
    // we just pre-initialised.
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

// Per-bucket integration. dt_seconds is wall-clock for this render frame.
// If tick_hz > 0 the bucket runs the tick_expr at the fixed cadence
// (accumulating leftover ms across frames); otherwise the tick_expr runs
// once per render frame.
void TParticleEffectManager::IntegrateBucket(SBucketRuntime& brt, TObjectInstance* owner, float dt_seconds, float owner_particle_id)
{
    if (!brt.bucket || !brt.bucket_def) return;
    if (!brt.tick_compiled && !brt.kill_compiled && brt.bucket_def->default_life <= 0.0f
        && brt.bucket_def->spawn_count == 0)
    {
        // Nothing to do -- no per-tick dynamics, no life-based kill,
        // no spawn-topup restock target. The legacy emitter/output
        // path (TorchFlame) hits this branch.
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
                    // One-shot burst: tag the particle dead via owner_id
                    // mutation, then reap below.
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
        IntegrateBucket(brt, owner, dt_seconds, runtime->owner_particle_id);
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
// * TBloodEffect - PE-pipeline minimum-viable spawn for --test=vfx        *
// *************************************************************************
//
// Scope: Phase 2.2 pipeline validator. This is *not* a faithful port of
// retail TBloodSystem kinematics (gravity, splat-sticking, surface-decal
// blood splats). Those land in Phase 2.2.1 once TBloodSystem +
// TBloodAnimator are mapped against the retail Ghidra output
// (recon/classes/cls_0x5acaa8.cpp — the merged Blood+Mist+WaterFall file
// flagged in INVENTORY for split-before-port).
//
// What this *does* deliver:
//   1. A real `TBloodEffect` instance that owns its lifecycle.
//   2. A `TParticleBucket` allocated from `ParticleManager()` and submitted
//      every frame via `Renderer->SubmitFxParticleBucket()` — exercising
//      the PE pipeline end-to-end through the real effect class lineage.
//   3. The bucket's texture is sourced from the canonical bloodimagery
//      (`Misc\Blood.I3D`, the same asset playscreen.cpp:236 loads), so
//      this validates 3D-imagery → particle-texture wiring too.
//   4. Spawns red, downward + outward droplets with brief lifetime — the
//      "vaguely like a blood spray" bar the scope sets explicitly.
//
// What this does *not* deliver:
//   - Faithful retail TBloodSystem droplet/splat state machine.
//   - Surface intersection for decals.
//   - The TBloodAnimator <-> TParticleSystem dispatch glue.
//
// Per the time-box rule, all of the above are deferred to Phase 2.2.1
// rather than blocking this row on TBloodSystem reverse-engineering.
namespace {

constexpr const char* kBloodImageryPath  = "Misc\\Blood.I3D";
constexpr const char* kBloodBucketName   = "vfx.blood.droplets";

// Monotonic counter for per-instance owner_particle_id so multiple
// concurrent spawns don't share particles in the global bucket. The
// usual ownership stamp is GetMapIndex(), but particles allocated via
// SpawnForTest live in the *global* TParticleManager bucket where map
// indices aren't unique across the whole engine — a dedicated 1000+
// range avoids stomping any real map-index-derived ids.
float NextBloodOwnerId()
{
    static float next = 1000.0f;
    const float v = next;
    next += 1.0f;
    return v;
}

// Lazily allocate the shared droplet bucket against the blood imagery's
// texture slot 0. Returns nullptr if the texture handle isn't available
// yet (caller logs + bails).
TParticleBucket* AcquireBloodBucket(T3DImagery* img3d)
{
    if (TParticleBucket* existing = ParticleManager().FindGlobalBucket(kBloodBucketName))
        return existing;

    // Mirror TFlameEffect::AttachVisualComponent's lazy-mesh-init poke:
    // NumTextures() doesn't trigger init by itself, NumObjects() does.
    (void)img3d->NumObjects();
    if (img3d->NumTextures() <= 0)
    {
        log_error("[blood] AcquireBloodBucket: imagery has 0 textures after "
                  "lazy-init poke (objects=%d)", img3d->NumObjects());
        return nullptr;
    }

    S3DTex tex = {};
    img3d->GetTexture(0, &tex);
    if (tex.htexture == kInvalidTexture)
    {
        log_error("[blood] AcquireBloodBucket: texture slot 0 handle invalid");
        return nullptr;
    }

    SParticleBucketDesc desc = {};
    desc.name           = kBloodBucketName;
    desc.scope          = EParticleBucketScope::Global;
    // Blood is a textured droplet: standard alpha blending lets the
    // droplet outline read against the scene, scene lighting (LitFlat)
    // multiplies its dark-red base color by (ambient + sun_term) so it
    // doesn't render flat-dark like an unlit overlay would. Explicit
    // TestNoWrite (default) makes the depth-mode choice readable here.
    desc.light_mode     = EParticleLightMode::Unlit;
    desc.depth_mode     = EParticleDepthMode::TestNoWrite;
    // PremulAlpha works correctly with the chroma-key-converted
    // Blood.I3D texture (alpha=0 in the black bg, rgb already
    // premultiplied since opaque pixels have alpha=1). This removes
    // the dark-red fringe that straight-alpha bilinear interpolation
    // produced at the splat edges.
    desc.blend          = EParticleBlendMode::PremulAlpha;
    desc.sort           = EParticleSortMode::None;
    desc.texture        = tex.htexture;
    desc.texture_width  = int32_t(tex.desc.width  > 0 ? tex.desc.width  : 1);
    desc.texture_height = int32_t(tex.desc.height > 0 ? tex.desc.height : 1);
    // Blood.I3D verified by direct atlas dump: 2×2 grid of 64×64
    // splat variants (cluster / X / oval / dot) on a pure-black bg.
    // The second texture in the I3D is a stencil mask, not yet used.
    desc.frame_cols     = 2;
    desc.frame_rows     = 2;
    desc.default_width  = 32.0f;
    desc.default_height = 32.0f;

    SParticleBufferLayout layout = {};
    ParticleLayoutAddVar(layout, EParticleVar::OwnerId);
    ParticleLayoutAddVar(layout, EParticleVar::Life);
    ParticleLayoutAddVar(layout, EParticleVar::Age);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
    ParticleLayoutAddVar(layout, EParticleVar::DrawColor);
    ParticleLayoutAddVar(layout, EParticleVar::DrawFrame);
    // Per-particle rotation (radians). Retail blood used a random
    // birth orientation so successive splats didn't all face the
    // same way -- we mirror that.
    ParticleLayoutAddVar(layout, EParticleVar::DrawRot);
    // EmitVel stores per-particle velocity (world-units / sec) so the
    // tick step can integrate without re-randomizing every frame.
    ParticleLayoutAddVar(layout, EParticleVar::EmitVel);

    TParticleBucket* bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
    log_info("[blood] AcquireBloodBucket: bucket='%s' tex=%dx%d handle=%u",
             kBloodBucketName, desc.texture_width, desc.texture_height,
             tex.htexture);
    return bucket;
}

} // namespace

TBloodEffect::~TBloodEffect()
{
    // Evict our particles from the shared bucket. Bucket itself stays
    // alive on the manager — it's reused across spawn cycles in the
    // --test=vfx harness (Left/Right cycling destroys + respawns).
    if (bucket_ && owner_particle_id_ >= 0.0f)
        bucket_->KillParticlesByOwner(owner_particle_id_);
}

// Retail TBloodEffect::Initialize / Pulse are the in-game spawn-path
// hooks (decompiled body lives in recon/classes/cls_0x5acaa8.cpp, merged
// with Mist + WaterFall — see INVENTORY B01 notes). For Phase 2.2
// (PE-pipeline validator) we provide empty bodies so the vtable links;
// the `--test=vfx` path drives spawn + tick through SpawnForTest +
// TickAndSubmitForTest instead. Faithful retail kinematics + the
// SObjectDef-driven Pulse cycle land in Phase 2.2.1.
void TBloodEffect::Initialize()
{
    // Phase 2.2.1: port body from recon/classes/cls_0x5acaa8.cpp once the
    // Ghidra merge is split into TBloodEffect / TMistEffect / TWaterFallEffect.
}

void TBloodEffect::Pulse()
{
    TEffect::Pulse();
    // Phase 2.2.1: port body from recon. The PE-pipeline harness uses
    // TickAndSubmitForTest, which doesn't depend on Pulse() — but the
    // in-game spawn path will once Phase 2.2.1 lands.
}

// Cached parsed effect def (lazy on first use). Mirrors TorchFlameDef().
const SParticleEffectDef& BloodEffectDef()
{
    static const SParticleEffectDef def = ParseParticleEffectDef("Blood");
    return def;
}

// Engine-driven SpawnForTest. Loads Misc\Blood.I3D (still needed -- the
// effect instance refcount must own an imagery for the engine's component
// lifecycle to be valid), constructs the TBloodEffect, attaches a
// TParticleEffectComponent configured with the Blood def, and pulses it
// once so the engine StartRuntime path runs the spawn_burst.
//
// Per-particle dynamics (cone spawn, gravity arc, alpha fade) live in
// the effects.def `Blood` block; this method is now ~30 lines of
// component wiring instead of ~95 lines of bespoke spawn-init.
TBloodEffect* TBloodEffect::SpawnForTest(const S3DPoint& origin)
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

    auto* blood = new TBloodEffect(base);
    blood->ForcePos(origin);
    blood->SetMapIndex(MapPane.MakeIndex());
    blood->ActivateComponents();

    // Attach the engine-driven particle component. spawn_burst = 10
    // in effects.def -> StartRuntime fires the cone burst at this point.
    auto particle_effect = std::make_unique<TParticleEffectComponent>();
    particle_effect->Configure(&BloodEffectDef());
    blood->AddComponent(std::move(particle_effect));

    // Drive one DrawPulse so StartRuntime + EmitSpawnTopup fire now (the
    // harness's first TickAndSubmitForTest happens later in the frame and
    // would otherwise miss this frame's submit -- we want the burst alive
    // immediately on Spawn).
    if (auto* pe = blood->GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    // Cache the bucket pointer for direct submission in TickAndSubmit.
    blood->bucket_ = ParticleManager().FindGlobalBucket("blood_droplets");
    blood->owner_particle_id_ = float(blood->GetMapIndex());

    log_info("[blood] SpawnForTest (engine-driven): map_index=%d origin=(%d,%d,%d) "
             "bucket=%p def=Blood",
             blood->GetMapIndex(),
             origin.x, origin.y, origin.z,
             (void*)blood->bucket_);
    return blood;
}

void TBloodEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
{
    if (!Renderer) return;

    // Drive the engine's per-frame integration (tick_expr + kill_expr +
    // age update + reap). DrawPulse is the standard per-frame entry on
    // TParticleEffectComponent -- the harness reuses it here instead of
    // re-implementing the integration loop.
    if (auto* pe = GetComponent<TParticleEffectComponent>())
        pe->DrawPulse();

    // Submit the global bucket to the FX queue. The bucket holds particles
    // from every TBloodEffect instance currently live; the renderer doesn't
    // care which instance owns which particle for rendering -- only the
    // owner_id matters for kill/respawn bookkeeping (handled in the engine).
    if (bucket_)
        Renderer->SubmitFxParticleBucket(*bucket_, debug_mode);
}

#if 0
// REVISITED: replaced by effects.def Blood declaration, kept for reference
// until migration of all 5 bespoke effects (B01/M05/F03/H03/H04) lands.
// The original bespoke spawn/tick body integrated 10 droplets with polar
// cone init, kGravity = -480 wu/s^2 Euler, held-then-linear-fade alpha
// curve, age-based reap. All of that is now expressed in the effects.def
// Blood block's spawn_expr / tick_expr / kill_expr.
TBloodEffect* TBloodEffect::SpawnForTest_BESPOKE(const S3DPoint& origin)
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

void TBloodEffect::TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode)
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
