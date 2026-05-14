// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *                animsystem.cpp - Animation Channel System              *
// *************************************************************************

#include "animsystem.h"

#include <algorithm>
#include <cmath>

namespace {

bool IsQuatChannel(uint16_t channel)
{
    return channel == uint16_t(EAnimChannel::RotX) ||
           channel == uint16_t(EAnimChannel::RotY) ||
           channel == uint16_t(EAnimChannel::RotZ) ||
           channel == uint16_t(EAnimChannel::RotW);
}

float DefaultChannelValue(uint16_t channel)
{
    if (channel == uint16_t(EAnimChannel::RotW))
        return 1.0f;
    if (channel == uint16_t(EAnimChannel::ScaleX) ||
        channel == uint16_t(EAnimChannel::ScaleY) ||
        channel == uint16_t(EAnimChannel::ScaleZ) ||
        channel == uint16_t(EAnimChannel::Alpha))
        return 1.0f;
    return 0.0f;
}

float WrapTime(float time, float duration, bool loop)
{
    if (duration <= 0.0f)
        return time;
    if (!loop)
        return (std::max)(0.0f, (std::min)(time, duration));
    float t = std::fmod(time, duration);
    if (t < 0.0f)
        t += duration;
    return t;
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float Dot(SAnimQuat a, SAnimQuat b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

struct SScalarAccum
{
    float value = 0.0f;
    float weight = 0.0f;
    bool additive = false;
};

struct SQuatAccum
{
    SAnimQuat q = {};
    float weight = 0.0f;
    bool initialized = false;
};

} // namespace

void SAnimPoseLayout::ConfigureBoneLocalTRS(int32_t bones)
{
    bone_count = (std::max)(0, bones);
    channel_count = bone_count * ANIM_BONE_STRIDE;
    channel_flags.assign(size_t(channel_count), uint8_t(ANIM_POSE_CHANNEL_NONE));
    quat_spans.clear();
    quat_spans.reserve(size_t(bone_count));
    for (int32_t bone = 0; bone < bone_count; ++bone)
    {
        const int32_t offset = bone * ANIM_BONE_STRIDE + ANIM_BONE_ROT_X;
        quat_spans.push_back({ offset });
        SetChannelFlags(offset, 4, uint8_t(ANIM_POSE_CHANNEL_QUAT));
    }
}

void SAnimPoseLayout::SetChannelFlags(int32_t offset, int32_t count, uint8_t flags)
{
    if (offset < 0 || count <= 0)
        return;
    const int32_t end = (std::min)(offset + count, channel_count);
    for (int32_t i = offset; i < end; ++i)
        channel_flags[size_t(i)] |= flags;
}

int32_t SAnimPoseLayout::BoneOffset(int32_t bone_index) const
{
    if (bone_index < 0 || bone_index >= bone_count)
        return -1;
    return bone_index * ANIM_BONE_STRIDE;
}

uint8_t SAnimPoseLayout::ChannelFlags(int32_t channel_index) const
{
    if (channel_index < 0 || channel_index >= int32_t(channel_flags.size()))
        return uint8_t(ANIM_POSE_CHANNEL_NONE);
    return channel_flags[size_t(channel_index)];
}

void SAnimPoseBuffer::Configure(const SAnimPoseLayout& new_layout)
{
    layout = &new_layout;
    values.resize(size_t((std::max)(0, new_layout.channel_count)));
    ResetToIdentity();
}

void SAnimPoseBuffer::ResetToIdentity()
{
    if (!layout)
    {
        values.clear();
        return;
    }
    AnimPoseResetToIdentity(*layout, Data(), Count());
}

float SAnimTrack::Sample(float time, float duration, bool loop) const
{
    if (keys.empty())
        return DefaultChannelValue(AnimTrackChannel(id));
    if (keys.size() == 1 || kind == EAnimTrackKind::Constant)
        return keys.front().value;

    const float t = WrapTime(time, duration, loop);
    if (t <= keys.front().time)
        return keys.front().value;
    if (t >= keys.back().time)
    {
        if (!loop || kind == EAnimTrackKind::Step)
            return keys.back().value;

        // In a looping clip, the final key is the start of the final frame
        // interval, not the clip end. The clip duration names the end of that
        // interval, so linear tracks continue from the final key toward the
        // first key until the wrapped time reaches 0 again.
        const float first_time = keys.front().time;
        const float final_span = duration - keys.back().time + first_time;
        if (final_span <= 0.0f)
            return keys.back().value;
        const float u = (std::min)(1.0f, (t - keys.back().time) / final_span);
        return Lerp(keys.back().value, keys.front().value, u);
    }

    auto it = std::upper_bound(keys.begin(), keys.end(), t,
        [](float value, const SAnimKey& key) { return value < key.time; });
    if (it == keys.begin())
        return it->value;

    const SAnimKey& b = *it;
    const SAnimKey& a = *(it - 1);
    if (kind == EAnimTrackKind::Step || b.time <= a.time)
        return a.value;

    const float u = (t - a.time) / (b.time - a.time);
    return Lerp(a.value, b.value, u);
}

void SAnimPose::Clear()
{
    values.clear();
}

bool SAnimPose::Has(AnimTrackId id) const
{
    return values.find(id) != values.end();
}

float SAnimPose::Get(AnimTrackId id, float fallback) const
{
    auto it = values.find(id);
    return it == values.end() ? fallback : it->second;
}

void SAnimPose::Set(AnimTrackId id, float value)
{
    values[id] = value;
}

SAnimQuat SAnimPose::GetQuat(uint16_t target) const
{
    SAnimQuat q;
    q.x = Get(AnimTrack(target, EAnimChannel::RotX), 0.0f);
    q.y = Get(AnimTrack(target, EAnimChannel::RotY), 0.0f);
    q.z = Get(AnimTrack(target, EAnimChannel::RotZ), 0.0f);
    q.w = Get(AnimTrack(target, EAnimChannel::RotW), 1.0f);
    return NormalizeQuat(q);
}

void SAnimPose::SetQuat(uint16_t target, const SAnimQuat& q)
{
    const SAnimQuat n = NormalizeQuat(q);
    Set(AnimTrack(target, EAnimChannel::RotX), n.x);
    Set(AnimTrack(target, EAnimChannel::RotY), n.y);
    Set(AnimTrack(target, EAnimChannel::RotZ), n.z);
    Set(AnimTrack(target, EAnimChannel::RotW), n.w);
}

SAnimPose SAnimClip::Sample(float time) const
{
    SAnimPose pose;
    for (const SAnimTrack& track : tracks)
        pose.Set(track.id, track.Sample(time, duration, loop));

    std::vector<uint16_t> targets;
    targets.reserve(pose.values.size());
    for (const auto& kv : pose.values)
        if (IsQuatChannel(AnimTrackChannel(kv.first)))
            targets.push_back(AnimTrackTarget(kv.first));

    std::sort(targets.begin(), targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());

    for (uint16_t target : targets)
        pose.SetQuat(target, pose.GetQuat(target));
    return pose;
}

SAnimQuat NormalizeQuat(SAnimQuat q)
{
    const float len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (len2 <= 1e-12f)
        return {};
    const float inv = 1.0f / std::sqrt(len2);
    q.x *= inv;
    q.y *= inv;
    q.z *= inv;
    q.w *= inv;
    return q;
}

SAnimQuat NlerpQuat(SAnimQuat a, SAnimQuat b, float t)
{
    a = NormalizeQuat(a);
    b = NormalizeQuat(b);
    if (Dot(a, b) < 0.0f)
    {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
    }
    return NormalizeQuat({
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t),
        Lerp(a.z, b.z, t),
        Lerp(a.w, b.w, t),
    });
}

void AnimPoseResetToIdentity(const SAnimPoseLayout& layout, float* out, int32_t out_count)
{
    if (!out || out_count <= 0)
        return;

    const int32_t count = (std::min)(out_count, layout.channel_count);
    std::fill(out, out + count, 0.0f);

    for (int32_t bone = 0; bone < layout.bone_count; ++bone)
    {
        const int32_t base = layout.BoneOffset(bone);
        if (base < 0 || base + ANIM_BONE_STRIDE > count)
            continue;
        out[base + ANIM_BONE_ROT_W] = 1.0f;
        out[base + ANIM_BONE_SCALE_X] = 1.0f;
        out[base + ANIM_BONE_SCALE_Y] = 1.0f;
        out[base + ANIM_BONE_SCALE_Z] = 1.0f;
    }
}

void AnimPoseNormalizeQuats(const SAnimPoseLayout& layout, float* pose, int32_t pose_count)
{
    if (!pose || pose_count <= 0)
        return;

    for (const SAnimQuatSpan& span : layout.quat_spans)
    {
        if (span.offset < 0 || span.offset + 3 >= pose_count)
            continue;

        SAnimQuat q = {
            pose[span.offset + 0],
            pose[span.offset + 1],
            pose[span.offset + 2],
            pose[span.offset + 3],
        };
        q = NormalizeQuat(q);
        pose[span.offset + 0] = q.x;
        pose[span.offset + 1] = q.y;
        pose[span.offset + 2] = q.z;
        pose[span.offset + 3] = q.w;
    }
}

void AnimPoseAlignQuatSigns(const SAnimPoseLayout& layout, const float* reference,
                            float* pose, int32_t pose_count)
{
    if (!reference || !pose || pose_count <= 0)
        return;

    for (const SAnimQuatSpan& span : layout.quat_spans)
    {
        if (span.offset < 0 || span.offset + 3 >= pose_count)
            continue;

        const float dot =
            reference[span.offset + 0] * pose[span.offset + 0] +
            reference[span.offset + 1] * pose[span.offset + 1] +
            reference[span.offset + 2] * pose[span.offset + 2] +
            reference[span.offset + 3] * pose[span.offset + 3];
        if (dot >= 0.0f)
            continue;

        pose[span.offset + 0] = -pose[span.offset + 0];
        pose[span.offset + 1] = -pose[span.offset + 1];
        pose[span.offset + 2] = -pose[span.offset + 2];
        pose[span.offset + 3] = -pose[span.offset + 3];
    }
}

void AnimPoseBlendLayers(const SAnimPoseLayout& layout, const SAnimBlendLayer* layers,
                         int32_t layer_count, float* out, int32_t out_count)
{
    if (!out || out_count <= 0)
        return;

    const int32_t count = (std::min)(out_count, layout.channel_count);
    if (!layers || layer_count <= 0 || count <= 0)
    {
        AnimPoseResetToIdentity(layout, out, out_count);
        return;
    }

    std::fill(out, out + count, 0.0f);
    bool any = false;
    for (int32_t layer_index = 0; layer_index < layer_count; ++layer_index)
    {
        const SAnimBlendLayer& layer = layers[layer_index];
        if (!layer.values || layer.weight == 0.0f)
            continue;

        any = true;
        if (layer.mask)
        {
            for (int32_t i = 0; i < count; ++i)
                out[i] += layer.values[i] * layer.weight * layer.mask[i];
        }
        else
        {
            for (int32_t i = 0; i < count; ++i)
                out[i] += layer.values[i] * layer.weight;
        }
    }

    // q and -q represent the same rotation. Re-accumulate only the quaternion
    // spans with a shared hemisphere before normalization; every other channel
    // remains the plain weighted array sum above.
    for (const SAnimQuatSpan& span : layout.quat_spans)
    {
        if (span.offset < 0 || span.offset + 3 >= count)
            continue;

        SAnimQuat reference = {};
        bool have_reference = false;
        out[span.offset + 0] = 0.0f;
        out[span.offset + 1] = 0.0f;
        out[span.offset + 2] = 0.0f;
        out[span.offset + 3] = 0.0f;

        for (int32_t layer_index = 0; layer_index < layer_count; ++layer_index)
        {
            const SAnimBlendLayer& layer = layers[layer_index];
            if (!layer.values || layer.weight == 0.0f)
                continue;

            SAnimQuat q = {
                layer.values[span.offset + 0],
                layer.values[span.offset + 1],
                layer.values[span.offset + 2],
                layer.values[span.offset + 3],
            };
            if (!have_reference)
            {
                reference = q;
                have_reference = true;
            }
            else if (Dot(reference, q) < 0.0f)
            {
                q.x = -q.x;
                q.y = -q.y;
                q.z = -q.z;
                q.w = -q.w;
            }

            const float mx = layer.mask ? layer.mask[span.offset + 0] : 1.0f;
            const float my = layer.mask ? layer.mask[span.offset + 1] : 1.0f;
            const float mz = layer.mask ? layer.mask[span.offset + 2] : 1.0f;
            const float mw = layer.mask ? layer.mask[span.offset + 3] : 1.0f;
            out[span.offset + 0] += q.x * layer.weight * mx;
            out[span.offset + 1] += q.y * layer.weight * my;
            out[span.offset + 2] += q.z * layer.weight * mz;
            out[span.offset + 3] += q.w * layer.weight * mw;
        }
    }

    AnimPoseNormalizeQuats(layout, out, count);

    for (int32_t i = 0; i < count; ++i)
    {
        if (layout.ChannelFlags(i) & uint8_t(ANIM_POSE_CHANNEL_BOOL))
            out[i] = out[i] >= 0.5f ? 1.0f : 0.0f;
    }

    if (!any)
    {
        AnimPoseResetToIdentity(layout, out, out_count);
        return;
    }
}

void AnimPoseBlendTwo(const SAnimPoseLayout& layout, const float* a, const float* b,
                      float alpha, float* out, int32_t out_count)
{
    if (!a && !b)
    {
        AnimPoseResetToIdentity(layout, out, out_count);
        return;
    }

    alpha = (std::max)(0.0f, (std::min)(alpha, 1.0f));
    const SAnimBlendLayer layers[2] = {
        { a ? a : b, 1.0f - alpha, nullptr },
        { b ? b : a, alpha,        nullptr },
    };
    AnimPoseBlendLayers(layout, layers, 2, out, out_count);
}

SAnimPose EvaluateAnimLayers(const std::vector<SAnimLayer>& layers)
{
    SAnimPose out;
    std::unordered_map<AnimTrackId, SScalarAccum> scalars;
    std::unordered_map<uint16_t, SQuatAccum> quats;

    for (const SAnimLayer& layer : layers)
    {
        if (!layer.clip || layer.weight <= 0.0f)
            continue;

        SAnimPose sample = layer.clip->Sample(layer.time);
        std::vector<uint16_t> quat_targets;
        for (const auto& kv : sample.values)
        {
            const AnimTrackId id = kv.first;
            const uint16_t target = AnimTrackTarget(id);
            const uint16_t channel = AnimTrackChannel(id);
            if (IsQuatChannel(channel))
            {
                quat_targets.push_back(target);
                continue;
            }

            SScalarAccum& acc = scalars[id];
            if (layer.mode == EAnimBlendMode::Additive)
            {
                acc.value += kv.second * layer.weight;
                acc.additive = true;
            }
            else
            {
                acc.value += kv.second * layer.weight;
                acc.weight += layer.weight;
            }
        }

        std::sort(quat_targets.begin(), quat_targets.end());
        quat_targets.erase(std::unique(quat_targets.begin(), quat_targets.end()), quat_targets.end());
        for (uint16_t target : quat_targets)
        {
            SAnimQuat q = sample.GetQuat(target);
            SQuatAccum& acc = quats[target];
            if (!acc.initialized)
            {
                acc.q = { q.x * layer.weight, q.y * layer.weight, q.z * layer.weight, q.w * layer.weight };
                acc.weight = layer.weight;
                acc.initialized = true;
            }
            else
            {
                if (Dot(acc.q, q) < 0.0f)
                {
                    q.x = -q.x; q.y = -q.y; q.z = -q.z; q.w = -q.w;
                }
                acc.q.x += q.x * layer.weight;
                acc.q.y += q.y * layer.weight;
                acc.q.z += q.z * layer.weight;
                acc.q.w += q.w * layer.weight;
                acc.weight += layer.weight;
            }
        }
    }

    for (const auto& kv : scalars)
    {
        const SScalarAccum& acc = kv.second;
        if (acc.additive || acc.weight <= 0.0f)
            out.Set(kv.first, acc.value);
        else
            out.Set(kv.first, acc.value / acc.weight);
    }

    for (const auto& kv : quats)
    {
        SAnimQuat q = kv.second.q;
        if (kv.second.weight > 0.0f)
        {
            const float inv = 1.0f / kv.second.weight;
            q.x *= inv; q.y *= inv; q.z *= inv; q.w *= inv;
        }
        out.SetQuat(kv.first, q);
    }

    return out;
}
