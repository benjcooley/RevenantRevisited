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

// Per-bucket quad orientation. Mirrors renderer's EFxBillboardOrientation
// (the bucket-particle path maps EParticleOrientation -> EFxBillboardOrientation
// at submission time). Default ScreenAligned matches the historical
// camera-aligned billboard expansion -- ground-projected buckets (AoE
// rings on the floor, ground-decal swarms) opt in to WorldXY.
enum class EParticleOrientation : uint8_t
{
    ScreenAligned = 0,
    WorldXY,
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

// Coordinate space the bucket's per-particle DrawPos is interpreted in.
// World (default): DrawPos is absolute world coordinates. Particles do not
// follow their owner after spawn -- "spawn-and-forget" semantics that match
// blood splats, debris, smoke trails that drift on their own.
// Local: DrawPos is relative to the bucket's `anchor` (set by the owning
// effect each frame via TParticleBucket::SetAnchor). The renderer adds
// anchor + DrawPos when materializing instances. Local lets particles
// move with their owner (torch wisps follow the torch's instance, an
// aura ring follows the character). Only meaningful for single-owner
// buckets; shared global buckets must stay World since they hold
// particles from multiple owners with different positions.
enum class EParticleBucketSpace : uint8_t
{
    World = 0,
    Local,
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
    // Per-bucket quad orientation. Default ScreenAligned keeps existing
    // bucket behavior; ground-aligned PE swarms (ground decal rings,
    // floor-projected glyph particles) set this to WorldXY.
    EParticleOrientation orientation = EParticleOrientation::ScreenAligned;
    EParticleSortMode sort = EParticleSortMode::None;
    int32_t sort_order = 0;
    // World/Local space for DrawPos. See EParticleBucketSpace comment.
    // Defaults to World so existing buckets keep current semantics.
    EParticleBucketSpace space = EParticleBucketSpace::World;
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

    // Per-particle PRNG state for Rand01/Rand ops. Treated as a 32-bit LCG
    // state (Park-Miller, multiplier 48271) that the eval loop advances on
    // each Rand01 call and writes back to the originating slot when the
    // eval is operating on a real bucket particle (EvalParticle path).
    // For the stateless Eval (frame_expr / uv_rect_expr) path the rng
    // state is ephemeral -- each Eval call seeds from `seed` and discards
    // changes.
    mutable uint32_t rng_state = 0;
};

// Forward decl -- the bucket-bound eval reads/writes particle slots.
class TParticleBucket;

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
    StoreVar,        // Pop value, write into named EParticleVar slot.
                     // Statement-form (no result lanes). Used by tick/spawn
                     // expressions to mutate particle state per-tick.
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
    // VM-extension ops (2026-05-17). See docs/vfx/VM_EXTENSION_FORENSICS.md
    // for the per-effect justification.
    Rand01,          // () -> U[0,1) per call; advances per-particle LCG seed.
    Rand,            // (lo, hi) -> lo + (hi-lo) * Rand01.
    Step,            // (edge, x) -> 0 if x<edge else 1 (per lane).
    Select,          // (cond, t, f) -> cond!=0 ? t : f (per lane). Branchless
                     // conditional for piecewise curves; lane count = max(t,f).
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

// Compile mode controls which identifier kinds the parser accepts.
//   Expression: legacy stateless expression (frame_expr / uv_rect_expr).
//               Identifiers: time_frame, age, age01, seed.
//   Statements: statement-form sequence ("var = expr;"). Identifiers
//               include particle-var aliases (pos, vel, color, scale,
//               frame, rot) which are read/write, and emit_pos/emit_vel
//               which are read-only. Used by tick_expr / spawn_expr.
enum class EParticleExprMode : uint8_t
{
    Expression,
    Statements,
};

class TParticleExpression
{
  public:
    bool Compile(const char* expr, std::string* error = nullptr,
                 EParticleExprMode mode = EParticleExprMode::Expression);
    float Eval(const SParticleEvalContext& ctx) const;
    void Eval(const SParticleEvalContext& ctx, float* out_values, int32_t out_lanes) const;
    // Bucket-bound eval. Reads particle-var identifiers from
    // bucket[particle_index] and writes StoreVar results back into the
    // same slots. Used by tick_expr (per-tick integration) and spawn_expr
    // (one-shot init). The eval context still supplies time_frame / age /
    // age01 / seed and the per-particle rng_state.
    void EvalParticle(const SParticleEvalContext& ctx,
                      TParticleBucket& bucket,
                      int32_t particle_index,
                      const float emit_pos[3] = nullptr,
                      const float emit_vel[3] = nullptr) const;
    // Bucket-bound predicate eval -- returns the boolean result of a
    // single-expression compile (mode=Expression). Treats nonzero scalar
    // result as true. Used by kill_expr.
    [[nodiscard]] bool EvalParticlePredicate(const SParticleEvalContext& ctx,
                                             TParticleBucket& bucket,
                                             int32_t particle_index,
                                             const float emit_pos[3] = nullptr,
                                             const float emit_vel[3] = nullptr) const;
    [[nodiscard]] bool IsValid() const { return valid; }
    [[nodiscard]] uint8_t ResultLanes() const { return result_lanes; }
    [[nodiscard]] const std::string& Source() const { return source; }
    [[nodiscard]] const std::vector<uint16_t>& Code() const { return code; }
    [[nodiscard]] const std::vector<float>& Constants() const { return constants; }
    [[nodiscard]] uint16_t MaxStackDepth() const { return max_stack_depth; }
    [[nodiscard]] EParticleExprMode Mode() const { return mode; }

  private:
    std::string source;
    std::vector<uint16_t> code;
    std::vector<float> constants;
    uint16_t max_stack_depth = 0;
    uint8_t result_lanes = 1;
    bool valid = false;
    EParticleExprMode mode = EParticleExprMode::Expression;
};

const SParticleOpDef* ParticleOpDefs(size_t* count = nullptr);
