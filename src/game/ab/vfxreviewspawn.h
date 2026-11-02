#pragma once
#include <cstdint>

class TObjectInstance;
struct S3DPoint;

struct VfxReviewSpawnInfo
{
    // Known current null-spell constructor family and valid EFFECT metadata.
    // Directed projectile rows also require a real map endpoint API.
    // This is not asset availability, renderer fidelity, or natural-caller proof.
    bool safe_factory = false;
    bool projectile = false;
    bool supports_endpoints = false;
    // Current map drawing/controller path available; no fidelity claim.
    // Describe cannot inspect lazy assets; Configure refines this after spawn.
    bool renderer_supported = false;
    // A real effect builder/component/controller, rather than bare asset faces.
    bool uses_effect_runtime = false;
    int object_type = -1;
    int authored_sound_tags = 0;
    const char* builder_name = "";
    const char* description = "";
};

// Call after CLASS EFFECT metadata is loaded. Does not instantiate/load assets.
VfxReviewSpawnInfo DescribeVfxReviewSpawn(uint32_t type_id);

// Call on the real freshly created map object, before its first Pulse. Leaves
// ordinary effects at their creation anchor. FireBall's existing Pulse owns
// movement; destination supplies original launch aim, not a forced impact.
// Adds no spell/caster, preview components, velocity override, or sound.
VfxReviewSpawnInfo ConfigureVfxReviewSpawn(TObjectInstance& effect,
                                         const S3DPoint& source,
                                         const S3DPoint& destination);
