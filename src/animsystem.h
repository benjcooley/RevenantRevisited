// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *                 animsystem.h - Animation Channel System                *
// *************************************************************************

#pragma once

#include "revtypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

typedef uint32_t AnimTrackId;

constexpr uint16_t ANIM_TARGET_ROOT = 0;

inline constexpr AnimTrackId MakeAnimTrackId(uint16_t target, uint16_t channel)
{
    return (uint32_t(target) << 16) | uint32_t(channel);
}

inline constexpr uint16_t AnimTrackTarget(AnimTrackId id)
{
    return uint16_t(id >> 16);
}

inline constexpr uint16_t AnimTrackChannel(AnimTrackId id)
{
    return uint16_t(id & 0xFFFFu);
}

// Built-in channel ids. Rotation channels store quaternion components.
enum class EAnimChannel : uint16_t
{
    PosX = 1,
    PosY,
    PosZ,

    RotX,
    RotY,
    RotZ,
    RotW,

    ScaleX,
    ScaleY,
    ScaleZ,

    Alpha = 100,
    TintR,
    TintG,
    TintB,

    TextureFrame = 200,
    UOffset,
    VOffset,
};

enum class EAnimTrackKind : uint8_t
{
    Constant,
    Step,
    Linear,
};

enum class EAnimBlendMode : uint8_t
{
    Override,
    Additive,
};

_STRUCTDEF(SAnimKey)
struct SAnimKey
{
    float time = 0.0f;
    float value = 0.0f;
};

_STRUCTDEF(SAnimQuat)
struct SAnimQuat
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

_STRUCTDEF(SAnimTrack)
struct SAnimTrack
{
    AnimTrackId id = 0;
    EAnimTrackKind kind = EAnimTrackKind::Linear;
    std::vector<SAnimKey> keys;

    float Sample(float time, float duration = 0.0f, bool loop = false) const;
};

_STRUCTDEF(SAnimPose)
struct SAnimPose
{
    std::unordered_map<AnimTrackId, float> values;

    void Clear();
    bool Has(AnimTrackId id) const;
    float Get(AnimTrackId id, float fallback = 0.0f) const;
    void Set(AnimTrackId id, float value);

    SAnimQuat GetQuat(uint16_t target) const;
    void SetQuat(uint16_t target, const SAnimQuat& q);
};

_STRUCTDEF(SAnimClip)
struct SAnimClip
{
    std::string name;
    float duration = 0.0f;
    bool loop = true;
    std::vector<SAnimTrack> tracks;

    SAnimPose Sample(float time) const;
};

_STRUCTDEF(SAnimLayer)
struct SAnimLayer
{
    const SAnimClip* clip = nullptr;
    float time = 0.0f;
    float weight = 1.0f;
    EAnimBlendMode mode = EAnimBlendMode::Override;
};

SAnimQuat NormalizeQuat(SAnimQuat q);
SAnimQuat NlerpQuat(SAnimQuat a, SAnimQuat b, float t);
SAnimPose EvaluateAnimLayers(const std::vector<SAnimLayer>& layers);

inline constexpr AnimTrackId AnimTrack(uint16_t target, EAnimChannel channel)
{
    return MakeAnimTrackId(target, uint16_t(channel));
}

