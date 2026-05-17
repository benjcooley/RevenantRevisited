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
#include "revutils.h"
#include "logging.h"
#include "time.h"

#include <cstring>
#include <cmath>
#include <cstdlib>
#include <utility>

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
        bucket.additive = bucket_node->get_string("blend", "additive") == "additive";
        bucket.flip_v = bucket_node->get_bool("flip_v", false);
        ParseOptionalColorRgb01(*bucket_node, "chroma_key", bucket.chroma_key, bucket.chroma_key_rgb);
        bucket.frame_expr = bucket_node->get_string("frame_expr");
        bucket.uv_rect_expr = bucket_node->get_string("uv_rect_expr");
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
    if (def.emitters.empty())
        ParticleFatal("[particle] effect " + def.name + " has no emitters");
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
        if (bucket.frame_expr.empty())
            ParticleFatal("[particle] bucket " + bucket.name + " missing frame_expr");
        if (bucket.uv_rect_expr.empty())
            ParticleFatal("[particle] bucket " + bucket.name + " missing uv_rect_expr");
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
    ParticleLayoutAddVar(layout, EParticleVar::EmitPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
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

SParticleBucketDesc BuildRuntimeBucketDesc(const SParticleEffectDef& effect_def,
                                           const SParticleBucketEffectDef& bucket_def,
                                           TObjectInstance* owner)
{
    T3DImagery* img3d = owner ? dynamic_cast<T3DImagery*>(owner->GetImagery()) : nullptr;
    if (!img3d || img3d->NumTextures() <= 0)
        FatalError("[particle] effect requires 3D imagery with at least one texture");

    S3DTex tex = {};
    img3d->GetTexture(bucket_def.texture_slot, &tex);
    if (tex.htexture == kInvalidTexture)
        FatalError("[particle] effect bucket texture handle is invalid");

    const int32_t texture_width = int32_t(tex.desc.width);
    const int32_t texture_height = int32_t(tex.desc.height);
    if (texture_width <= 0 || texture_height <= 0)
    {
        char err[256];
        snprintf(err, sizeof(err),
                 "[particle] effect bucket texture dimensions invalid desc=%ux%u",
                 tex.desc.width, tex.desc.height);
        FatalError(err);
    }

    SParticleBucketDesc desc = {};
    desc.name = bucket_def.name;
    desc.blend = bucket_def.additive ? EParticleBlendMode::Additive : EParticleBlendMode::Alpha;
    desc.sort = EParticleSortMode::None;
    desc.texture = tex.htexture;
    desc.texture_width = texture_width;
    desc.texture_height = texture_height;
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
        TParticleExpression expr;
        std::string error;
        if (!expr.Compile(bucket_def.uv_rect_expr.c_str(), &error))
            ParticleFatal("[particle] bucket " + bucket_def.name + " uv_rect expression compile failed: " + error);
        SParticleEvalContext ctx = {};
        ctx.time_frame = float(TTime::Time());
        expr.Eval(ctx, uv_rect, 4);
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

void TParticleEffectManager::StartRuntime(SRuntime& runtime, TObjectInstance* owner)
{
    if (!runtime.effect_def || !owner)
        return;

    runtime.buckets.clear();
    const SParticleBufferLayout layout = ParticleEffectBucketLayout();
    for (const SParticleBucketEffectDef& bucket_def : runtime.effect_def->buckets)
    {
        SParticleBucketDesc desc = BuildRuntimeBucketDesc(*runtime.effect_def, bucket_def, owner);
        runtime.buckets.push_back(ParticleManager().GetOrCreateGlobalBucket(desc, layout));
    }

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

void TBloodEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
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

void TMistEffect::TickAndSubmitForTest(EFxDebugMode debug_mode)
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
