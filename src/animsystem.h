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

// ---------------------------------------------------------------------------
// Runtime pose-buffer core
// ---------------------------------------------------------------------------
//
// Track data is authored/static clip data: target-property values keyed over
// time. Pose data is the sampled result at one time. This section is only the
// hot-path pose representation: a fixed float buffer plus small metadata that
// describes how to interpret spans of that buffer. Blending is weighted array
// math; metadata such as "this contiguous run is a quaternion" lives in the
// pose-buffer layout so the core can normalize quaternion spans after blending
// without doing channel-name/type discovery in the blend loop.
//
// Requirements:
//   * No allocation in ordinary per-frame sampling/blending. Configure pose
//     layouts and buffers at load/attach time, then reuse them.
//   * Runtime blend inputs are caller-owned float buffers. The core accepts N
//     layers now; current game code only needs A/B with weights 1-alpha/alpha.
//     Output buffers must not alias layer input buffers.
//   * Masks are future-ready: a layer may point at a precomputed per-channel
//     weight array. Passing nullptr means an unmasked layer. Building masks is
//     not a runtime blend concern.
//   * Legacy adapters may translate old state/frame data into these buffers,
//     but they should stay quarantined outside this core.

constexpr int32_t ANIM_BONE_POS_X   = 0;
constexpr int32_t ANIM_BONE_POS_Y   = 1;
constexpr int32_t ANIM_BONE_POS_Z   = 2;
constexpr int32_t ANIM_BONE_ROT_X   = 3;
constexpr int32_t ANIM_BONE_ROT_Y   = 4;
constexpr int32_t ANIM_BONE_ROT_Z   = 5;
constexpr int32_t ANIM_BONE_ROT_W   = 6;
constexpr int32_t ANIM_BONE_SCALE_X = 7;
constexpr int32_t ANIM_BONE_SCALE_Y = 8;
constexpr int32_t ANIM_BONE_SCALE_Z = 9;
constexpr int32_t ANIM_BONE_STRIDE  = 10;

enum EAnimPoseChannelFlag : uint8_t
{
    ANIM_POSE_CHANNEL_NONE = 0,
    ANIM_POSE_CHANNEL_QUAT = 1 << 0,
    ANIM_POSE_CHANNEL_BOOL = 1 << 1,
};

_STRUCTDEF(SAnimQuatSpan)
struct SAnimQuatSpan
{
    int32_t offset = 0; // Four contiguous floats: x, y, z, w.
};

_STRUCTDEF(SAnimPoseLayout)
struct SAnimPoseLayout
{
    int32_t bone_count = 0;
    int32_t channel_count = 0;
    // Parallel metadata for each float in a sampled pose buffer. The blend
    // system remains float-array based, while post passes use these flags to
    // handle non-scalar semantics such as bools or quaternion components.
    std::vector<uint8_t> channel_flags;
    std::vector<SAnimQuatSpan> quat_spans;

    void ConfigureBoneLocalTRS(int32_t bones);
    void SetChannelFlags(int32_t offset, int32_t count, uint8_t flags);
    [[nodiscard]] bool IsValid() const { return channel_count > 0; }
    [[nodiscard]] int32_t BoneOffset(int32_t bone_index) const;
    [[nodiscard]] uint8_t ChannelFlags(int32_t channel_index) const;
};

_STRUCTDEF(SAnimPoseBuffer)
struct SAnimPoseBuffer
{
    const SAnimPoseLayout* layout = nullptr;
    std::vector<float> values;

    void Configure(const SAnimPoseLayout& new_layout);
    void ResetToIdentity();
    [[nodiscard]] float* Data() { return values.empty() ? nullptr : values.data(); }
    [[nodiscard]] const float* Data() const { return values.empty() ? nullptr : values.data(); }
    [[nodiscard]] int32_t Count() const { return int32_t(values.size()); }
};

_STRUCTDEF(SAnimBlendLayer)
struct SAnimBlendLayer
{
    const float* values = nullptr;
    float weight = 0.0f;
    const float* mask = nullptr; // Optional per-channel multiplier, same layout.
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
    // Full clip duration in seconds. For legacy 24 Hz frame data with N
    // frames this is N / 24, not the final key time (N - 1) / 24. Frame/key
    // integers name whole time intervals; they are not end timestamps.
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
void AnimPoseResetToIdentity(const SAnimPoseLayout& layout, float* out, int32_t out_count);
void AnimPoseNormalizeQuats(const SAnimPoseLayout& layout, float* pose, int32_t pose_count);
void AnimPoseAlignQuatSigns(const SAnimPoseLayout& layout, const float* reference,
                            float* pose, int32_t pose_count);
// Blend already-sampled poses. Callers choose weights that make sense for their
// layer stack. The core keeps the operation pose-buffer based: scalar channels
// are weighted sums; quaternion spans are sign-aligned against the first active
// layer and normalized after accumulation. If masks are introduced for quat
// spans, all four components of a quaternion should use the same mask value.
void AnimPoseBlendLayers(const SAnimPoseLayout& layout, const SAnimBlendLayer* layers,
                         int32_t layer_count, float* out, int32_t out_count);
void AnimPoseBlendTwo(const SAnimPoseLayout& layout, const float* a, const float* b,
                      float alpha, float* out, int32_t out_count);
SAnimPose EvaluateAnimLayers(const std::vector<SAnimLayer>& layers);

inline constexpr AnimTrackId AnimTrack(uint16_t target, EAnimChannel channel)
{
    return MakeAnimTrackId(target, uint16_t(channel));
}
