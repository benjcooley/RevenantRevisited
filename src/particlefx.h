// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  particlefx.h - particle expression VM                 *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "render3d_types.h"

#include <cstdint>
#include <string>
#include <vector>

enum class EParticleVar : uint16_t
{
    TimeFrame,
    Age,
    Age01,
    Seed,
    OwnerId,
    Life,
    EmitPos,
    EmitRot,
    EmitScl,
    EmitVel,
    EmitColor,
    EmitLife,
    EmitFrame,
    LifePos,
    LifeRot,
    LifeScl,
    LifeVel,
    LifeColor,
    LifeAlpha,
    LifeFrame,
    DrawPos,
    DrawRot,
    DrawScl,
    DrawColor,
    DrawFrame,
    DrawUvRect,
    Count,
};

struct SParticleVarSlot
{
    int32_t offset = -1;
    int32_t lanes = 0;
};

struct SParticleBufferLayout
{
    SParticleVarSlot vars[int(EParticleVar::Count)];
    int32_t stride_floats = 0;
};

enum class EParticleBucketScope : uint8_t
{
    Global,
    Local,
};

// Canonical per-bucket blend mode -- mirrored to renderer's EFxBlend at
// submission time. AdditiveStraight is the D3DBLEND_ONE/ONE "self-lit
// overlay" mode used by retail TBloodSystem; Additive (the legacy default)
// is the SRC_ALPHA / ONE alpha-weighted additive used by smoke / spark
// trails; PremulAlpha is the standard one/one_minus_src_alpha premultiplied
// path for assets already authored with baked-in coverage.
enum class EParticleBlendMode : uint8_t
{
    Alpha,            // SRC_ALPHA / ONE_MINUS_SRC_ALPHA  (default for textured droplets)
    Additive,         // SRC_ALPHA / ONE                  (alpha-weighted additive)
    AdditiveStraight, // ONE / ONE                        (retail-style "self-lit" additive)
    PremulAlpha,      // ONE / ONE_MINUS_SRC_ALPHA        (premultiplied alpha)
};

// Whether the engine should multiply particle color by scene lighting.
// Per the original author's note ("some particles are lit by the scene
// lighting, others are glowing or self lit"): blood, debris, lit smoke
// should be LitFlat; explosions, sparks, glows stay Unlit. Per-particle
// normal-mapped lighting is a future expansion -- LitFlat treats the
// particle's normal as world-up (matches the way pre-release dls
// reconstructed lighting for round splats).
enum class EParticleLightMode : uint8_t
{
    Unlit = 0,   // particle color is literal (current behavior; default for back-compat)
    LitFlat,     // multiply rgb by (ambient + max(0, Ldir.z) * sun_color)
    // Future: LitNormalMapped (per-particle normal), LitHemisphere, ...
};

// Standard transparency-vs-depth knobs. TestNoWrite is what every
// transparent particle wants; TestWrite lets alpha-tested / mostly-solid
// particles (decals, sliced impostors) interleave with the world depth.
// None is for always-on-top overlays (HUD-attached fx, locators).
enum class EParticleDepthMode : uint8_t
{
    TestNoWrite = 0, // depth-test enabled, no depth write (default)
    TestWrite,       // depth-test + depth write
    None,            // no depth test, no depth write
};

enum class EParticleSortMode : uint8_t
{
    None,
    Depth,
    Distance,
    Age,
    Custom,
};

struct SParticleBucketDesc
{
    std::string name;
    EParticleBucketScope scope = EParticleBucketScope::Global;
    EParticleBlendMode blend = EParticleBlendMode::Additive;
    // Defaults preserve existing behavior so untouched bucket descs
    // render identically after this change (Unlit + TestNoWrite).
    EParticleLightMode light_mode = EParticleLightMode::Unlit;
    EParticleDepthMode depth_mode = EParticleDepthMode::TestNoWrite;
    EParticleSortMode sort = EParticleSortMode::None;
    int32_t sort_order = 0;
    TTextureHandle texture = kInvalidTexture;
    int32_t texture_width = 1;
    int32_t texture_height = 1;
    // Sprite atlas grid. When frame_cols * frame_rows > 1, the renderer
    // computes per-particle uv_rect from the particle's DrawFrame value
    // (frame_index mod (cols*rows)). 1×1 = use whole texture (or explicit
    // DrawUvRect override if the bucket layout includes it).
    int32_t frame_cols = 1;
    int32_t frame_rows = 1;
    float default_width = 1.0f;
    float default_height = 1.0f;
    bool debug_solid = false;
    bool flip_v = false;
    bool chroma_key = false;
    float chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};
};

int32_t ParticleDefaultLanes(EParticleVar var);
bool ParticleLayoutAddVar(SParticleBufferLayout& layout, EParticleVar var, int32_t lanes = 0);

class TParticleBucket
{
  public:
    TParticleBucket() = default;
    TParticleBucket(SParticleBucketDesc desc, SParticleBufferLayout layout);

    [[nodiscard]] const SParticleBucketDesc& Desc() const { return desc; }
    [[nodiscard]] const SParticleBufferLayout& Layout() const { return layout; }
    [[nodiscard]] int32_t Count() const { return count; }
    [[nodiscard]] bool Active() const { return count > 0; }

    int32_t AddParticle(float owner_id, float life);
    void KillParticlesByOwner(float owner_id);
    float* VarPtr(int32_t particle_index, EParticleVar var);
    const float* VarPtr(int32_t particle_index, EParticleVar var) const;

  private:
    void RemoveAt(int32_t particle_index);

    SParticleBucketDesc desc;
    SParticleBufferLayout layout = {};
    std::vector<float> data;
    int32_t count = 0;
};

class TParticleManager
{
  public:
    TParticleBucket* GetOrCreateGlobalBucket(const SParticleBucketDesc& desc,
                                             const SParticleBufferLayout& layout);
    TParticleBucket* FindGlobalBucket(const char* name);
    [[nodiscard]] int32_t GlobalBucketCount() const { return int32_t(global_buckets.size()); }
    TParticleBucket* GlobalBucket(int32_t index);
    const TParticleBucket* GlobalBucket(int32_t index) const;
    void BeginDrawPulsePass();
    [[nodiscard]] uint32_t DrawPulsePass() const { return draw_pulse_pass; }
    void Clear();

  private:
    std::vector<TParticleBucket> global_buckets;
    uint32_t draw_pulse_pass = 0;
};

TParticleManager& ParticleManager();

struct SParticleEvalContext
{
    float time_frame = 0.0f;
    float age = 0.0f;
    float age01 = 0.0f;
    float seed = 0.0f;
};

enum class EParticleArgKind : uint8_t
{
    FloatOffset,
    ConstIndex,
    TextureIndex,
    CurveIndex,
    VarId,
    ImmediateU16,
};

enum class EParticleOp : uint16_t
{
    End,
    LoadConst,
    LoadVar,
    StoreVar,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Neg,
    Floor,
    Sin,
    Cos,
    Min,
    Max,
    Clamp,
    Lerp,
    Frame,
    Flipbook,
    Vec2,
    Vec3,
    Vec4,
};

struct SParticleOpArgDef
{
    const char* name = "";
    EParticleArgKind kind = EParticleArgKind::ImmediateU16;
};

struct SParticleOpDef
{
    const char* name = "";
    uint8_t arg_count = 0;
    SParticleOpArgDef args[6] = {};
};

class TParticleExpression
{
  public:
    bool Compile(const char* expr, std::string* error = nullptr);
    float Eval(const SParticleEvalContext& ctx) const;
    void Eval(const SParticleEvalContext& ctx, float* out_values, int32_t out_lanes) const;
    [[nodiscard]] bool IsValid() const { return valid; }
    [[nodiscard]] uint8_t ResultLanes() const { return result_lanes; }
    [[nodiscard]] const std::string& Source() const { return source; }
    [[nodiscard]] const std::vector<uint16_t>& Code() const { return code; }
    [[nodiscard]] const std::vector<float>& Constants() const { return constants; }
    [[nodiscard]] uint16_t MaxStackDepth() const { return max_stack_depth; }

  private:
    std::string source;
    std::vector<uint16_t> code;
    std::vector<float> constants;
    uint16_t max_stack_depth = 0;
    uint8_t result_lanes = 1;
    bool valid = false;
};

const SParticleOpDef* ParticleOpDefs(size_t* count = nullptr);
