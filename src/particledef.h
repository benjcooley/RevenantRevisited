// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   particledef.h - the data of an engine particle effect (effects.def) *
// *************************************************************************
//
// Annotated records read straight from DEF text by defio (docs/vfx/
// PREFAB_SYSTEM.md §2.5). Member names are the DEF keys. Fields that the
// runtime wants in a cooked form (the legacy `blend` toggle, the 0..255
// chroma key) are derived in PostLoad. The engine architecture these
// describe is docs/PARTICLE_EFFECTS.md.
//
//     effect Blood
//     {
//         debug = normal
//         bucket blood_fly { ... }            -> SParticleEffectDef::bucket[]
//         emitter billboard                   -> SParticleEffectDef::emitter[]
//         {
//             local_pos = [0, 0, 32]
//             output flame_billboard { life = -1 }   -> SParticleEmitterEffectDef::output[]
//         }
//     }
//
// *************************************************************************

#pragma once

#include "refl.h"

#include <HandmadeMath.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// `output <bucket> { life }` -- which bucket an emitter writes to.
struct SParticleEmitterOutputDef
{
    PROP(1)  std::string name;                          // Target bucket
    PROP(2)  float       life = -1.0f;                  // Particle life in seconds; -1 = forever
};

struct SParticleEmitterEffectDef
{
    PROP(1)  std::string name;
    PROP(2)  hmm_vec3    local_pos = {{0, 0, 0}};       // Offset from the owner, world units
    PROP(3)  std::vector<SParticleEmitterOutputDef> output;
};

struct SParticleBucketEffectDef
{
    PROP(1)  std::string name;
    PROP(2)  int32_t     texture_slot = 0;
    PROP(3)  int32_t     atlas_cols = 1;
    PROP(4)  int32_t     atlas_rows = 1;
    PROP(5)  int32_t     atlas_frames = 1;
    PROP(6)  bool        random_start_frame = false;    // Pick a random atlas cell per particle at spawn
    PROP(7)  float       width = 1.0f;
    PROP(8)  float       height = 1.0f;
    PROP(9)  float       scale = 1.0f;
    PROP(10) std::string blend = "additive";            // Legacy toggle: additive | alpha (blend_mode overrides)
    PROP(11) bool        flip_v = false;
    PROP(12) hmm_vec3    chroma_key = {{-1, -1, -1}};   // Key colour, 0..255 per channel; negative = none
    PROP(13) std::string frame_expr;
    PROP(14) std::string uv_rect_expr;
    PROP(15) std::string imagery_path;                  // Imagery to take the texture from; empty = the owner's
    PROP(16) std::string blend_mode;                    // alpha | additive | additive_straight | premul_alpha
    PROP(17) std::string light_mode;                    // unlit | lit_flat
    PROP(18) std::string depth_mode;                    // test_no_write | test_write | none
    PROP(19) std::string orientation;                   // screen_aligned | world_xy
    PROP(20) std::string spawn_expr;                    // Statements run once when a particle spawns
    PROP(21) std::string tick_expr;                     // Statements run every sim tick
    PROP(22) std::string kill_expr;                     // True = kill (respawn for spawn_count buckets)
    PROP(23) int32_t     spawn_burst = 0;               // One-shot burst of N at attach
    PROP(24) int32_t     spawn_count = 0;               // Steady-state population of N
    PROP(25) int32_t     tick_hz = 0;                   // Sim rate for tick_expr; 0 = per frame
    PROP(26) float       default_life = -1.0f;          // Seconds; <= 0 = no auto-kill
    PROP(27) std::string chain_bucket;                  // Bucket a dying particle spawns into

    // Derived in PostLoad.
    bool  additive = true;                   // from `blend` (legacy)
    bool  chroma_key_enabled = false;        // from `chroma_key`
    float chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};

    void PostLoad();
};

struct SParticleEffectDef
{
    PROP(1)  std::string name;
    PROP(2)  std::string debug = "normal";              // normal | solid_green
    PROP(3)  int32_t     inactive_ttl_pulses = 1;       // Draw pulses an idle runtime survives
    PROP(4)  std::vector<SParticleBucketEffectDef>  bucket;
    PROP(5)  std::vector<SParticleEmitterEffectDef> emitter;

    bool debug_solid = false;                // derived in PostLoad

    void PostLoad();
    [[nodiscard]] const SParticleBucketEffectDef* FindBucket(std::string_view bucket_name) const;
    // The structural checks a loader runs after reading (names, atlas,
    // spawn mode, chain / output targets). First problem into `error`.
    [[nodiscard]] bool Validate(std::string* error) const;
};

// A whole effects.def: `effect <name> { }` repeated.
struct SParticleEffectsFile
{
    PROP(1)
    std::vector<SParticleEffectDef> effect;

    [[nodiscard]] const SParticleEffectDef* Find(std::string_view effect_name) const;
};
