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
        return loop ? keys.front().value : keys.back().value;

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

