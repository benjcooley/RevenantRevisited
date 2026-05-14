// *************************************************************************
// *                         Cinematix Revenant                            *
// *              i3danimpose.cpp - I3D-to-pose animation bridge            *
// *************************************************************************

#include "i3danimpose.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "3dimage.h"

namespace {

constexpr int32_t kLegacyTransitionBlendFrames = 5;

SAnimQuat QuatFromEulerXYZ(float x, float y, float z)
{
    const float cx = std::cos(x * 0.5f), sx = std::sin(x * 0.5f);
    const float cy = std::cos(y * 0.5f), sy = std::sin(y * 0.5f);
    const float cz = std::cos(z * 0.5f), sz = std::sin(z * 0.5f);

    SAnimQuat qx = { sx, 0.0f, 0.0f, cx };
    SAnimQuat qy = { 0.0f, sy, 0.0f, cy };
    SAnimQuat qz = { 0.0f, 0.0f, sz, cz };
    auto mul = [](const SAnimQuat& a, const SAnimQuat& b) -> SAnimQuat {
        return {
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        };
    };
    return NormalizeQuat(mul(mul(qz, qy), qx));
}

SAnimQuat CanonicalQuat(SAnimQuat q)
{
    q = NormalizeQuat(q);
    // q and -q encode the same rotation. Keep a stable hemisphere at the
    // adapter boundary so frame-to-frame blending has fewer sign flips to fix.
    if (q.w < 0.0f)
    {
        q.x = -q.x;
        q.y = -q.y;
        q.z = -q.z;
        q.w = -q.w;
    }
    return q;
}

bool IsLoopingSameStateTransition(T3DImagery* img, int32_t state, int32_t prevstate)
{
    if (!img || state < 0 || prevstate != state)
        return false;
    if (state >= img->NumStates())
        return false;
    return (img->GetAniFlags(state) & AF_LOOPING) != 0;
}

bool HasMatchingEndBegTransition(T3DImagery* img,
                                 int32_t state,
                                 int32_t prevstate,
                                 int32_t prevframe)
{
    if (!img || state < 0 || prevstate < 0)
        return false;
    if (state >= img->NumStates() || prevstate >= img->NumStates())
        return false;

    const int32_t prev_frames = img->GetAniLength(prevstate);
    if (prev_frames <= 0 || prevframe < prev_frames - 1)
        return false;

    char* begstate = img->FindTag((char*)"beg", state);
    char* endstate = img->FindTag((char*)"end", prevstate);
    return begstate && endstate && stricmp(endstate, begstate) == 0;
}

} // namespace

void ConfigureI3DLegacyPoseLayout(T3DImagery* img, SAnimPoseLayout& layout)
{
    layout.ConfigureBoneLocalTRS(img ? img->NumObjects() : 0);
}

bool SampleI3DLegacyFrameToPose(T3DImagery* img,
                                const SAnimPoseLayout& layout,
                                int32_t state,
                                int32_t frame,
                                int32_t prevstate,
                                int32_t prevframe,
                                float* out,
                                int32_t out_count)
{
    if (!out || out_count < layout.channel_count)
        return false;

    AnimPoseResetToIdentity(layout, out, out_count);
    if (!img || state < 0 || state >= img->NumStates())
        return false;

    img->SetPrevState(prevstate, prevframe);
    const int32_t bones = (std::min)(layout.bone_count, img->NumObjects());
    for (int32_t objnum = 0; objnum < bones; ++objnum)
    {
        if (img->IsHidden(objnum, state))
            continue;

        hmm_vec3 pos = {};
        hmm_vec3 rot = {};
        hmm_vec3 scl = {1.0f, 1.0f, 1.0f};
        if (!img->GetAniKey(objnum, state, frame, pos, rot, scl))
            continue;

        const int32_t base = layout.BoneOffset(objnum);
        if (base < 0 || base + ANIM_BONE_STRIDE > out_count)
            continue;

        const SAnimQuat q = CanonicalQuat(QuatFromEulerXYZ(rot.X, rot.Y, rot.Z));
        out[base + ANIM_BONE_POS_X] = pos.X;
        out[base + ANIM_BONE_POS_Y] = pos.Y;
        out[base + ANIM_BONE_POS_Z] = pos.Z;
        out[base + ANIM_BONE_ROT_X] = q.x;
        out[base + ANIM_BONE_ROT_Y] = q.y;
        out[base + ANIM_BONE_ROT_Z] = q.z;
        out[base + ANIM_BONE_ROT_W] = q.w;
        out[base + ANIM_BONE_SCALE_X] = scl.X;
        out[base + ANIM_BONE_SCALE_Y] = scl.Y;
        out[base + ANIM_BONE_SCALE_Z] = scl.Z;
    }

    return true;
}

bool SampleI3DLegacyFrameBlendToPose(T3DImagery* img,
                                     const SAnimPoseLayout& layout,
                                     int32_t state,
                                     int32_t frame,
                                     int32_t nextstate,
                                     int32_t nextframe,
                                     float framefrac,
                                     int32_t prevstate,
                                     int32_t prevframe,
                                     float* current_out,
                                     float* next_out,
                                     float* blended_out,
                                     int32_t out_count,
                                     const float** out_pose)
{
    if (out_pose)
        *out_pose = nullptr;
    if (!current_out || !next_out || !blended_out || out_count < layout.channel_count)
        return false;

    // Keep two concerns separate:
    //   1. key interpolation within the current clip, including last->first
    //      loop interpolation;
    //   2. optional A/B transition blend from the previous state into the
    //      current state.
    // Both operations blend fixed pose buffers. The old T3DImagery::prevstate
    // path is deliberately bypassed in the live bridge so it cannot leak a
    // state transition into ordinary loop playback.
    if (!SampleI3DLegacyFrameToPose(img, layout, state, frame,
            -1, 0, current_out, out_count))
        return false;

    const float* pose = current_out;
    if ((nextstate != state || nextframe != frame) && framefrac > 0.0f)
    {
        const float alpha = (std::min)(framefrac, 1.0f);
        if (SampleI3DLegacyFrameToPose(img, layout, nextstate, nextframe,
                -1, 0, next_out, out_count))
        {
            AnimPoseBlendTwo(layout, current_out, next_out, alpha,
                blended_out, out_count);
            pose = blended_out;
        }
    }

    const bool has_transition =
        !IsLoopingSameStateTransition(img, state, prevstate) &&
        !HasMatchingEndBegTransition(img, state, prevstate, prevframe) &&
        img &&
        prevstate >= 0 && prevstate < img->NumStates() &&
        frame >= 0 && frame < kLegacyTransitionBlendFrames;
    if (has_transition &&
        SampleI3DLegacyFrameToPose(img, layout, prevstate, prevframe,
            -1, 0, next_out, out_count))
    {
        const float alpha = (std::min)(1.0f,
            float(frame + 1) / float(kLegacyTransitionBlendFrames));
        if (pose == blended_out)
        {
            AnimPoseBlendTwo(layout, next_out, blended_out, alpha,
                current_out, out_count);
            pose = current_out;
        }
        else
        {
            AnimPoseBlendTwo(layout, next_out, current_out, alpha,
                blended_out, out_count);
            pose = blended_out;
        }
    }

    if (out_pose)
        *out_pose = pose;
    return true;
}

SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame)
{
    return SampleI3DAnimPose(img, state, frame, -1, 0);
}

SAnimPose SampleI3DAnimPose(T3DImagery* img, int32_t state, int32_t frame,
                            int32_t prevstate, int32_t prevframe)
{
    SAnimPose pose;
    if (!img || state < 0 || state >= img->NumStates())
        return pose;

    img->SetPrevState(prevstate, prevframe);
    for (int32_t objnum = 0; objnum < img->NumObjects(); ++objnum)
    {
        if (img->IsHidden(objnum, state))
            continue;

        hmm_vec3 pos = {};
        hmm_vec3 rot = {};
        hmm_vec3 scl = {1.0f, 1.0f, 1.0f};
        if (!img->GetAniKey(objnum, state, frame, pos, rot, scl))
            continue;

        const uint16_t target = uint16_t(objnum);
        pose.Set(AnimTrack(target, EAnimChannel::PosX), pos.X);
        pose.Set(AnimTrack(target, EAnimChannel::PosY), pos.Y);
        pose.Set(AnimTrack(target, EAnimChannel::PosZ), pos.Z);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleX), scl.X);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleY), scl.Y);
        pose.Set(AnimTrack(target, EAnimChannel::ScaleZ), scl.Z);
        pose.SetQuat(target, CanonicalQuat(QuatFromEulerXYZ(rot.X, rot.Y, rot.Z)));
    }
    return pose;
}

SAnimPose SampleI3DAnimPoseInterpolated(T3DImagery* img, int32_t state,
                                        int32_t frame, int32_t nextframe,
                                        float framefrac,
                                        int32_t prevstate,
                                        int32_t prevframe)
{
    const bool suppress_loop_transition =
        IsLoopingSameStateTransition(img, state, prevstate);
    const int32_t sample_prevstate = suppress_loop_transition ? -1 : prevstate;
    const int32_t sample_prevframe = suppress_loop_transition ? 0 : prevframe;

    SAnimPose pose = SampleI3DAnimPose(img, state, frame,
        sample_prevstate, sample_prevframe);
    if (!img || nextframe < 0 || nextframe == frame || framefrac <= 0.0f)
        return pose;

    if (framefrac > 1.0f)
        framefrac = 1.0f;

    SAnimPose next = SampleI3DAnimPose(img, state, nextframe,
        sample_prevstate, sample_prevframe);
    return BlendI3DAnimPoses(pose, next, framefrac);
}

SAnimPose BlendI3DAnimPoses(const SAnimPose& a, const SAnimPose& b, float t)
{
    SAnimPose out = a;
    std::vector<uint16_t> quat_targets;
    for (const auto& kv : b.values)
    {
        const AnimTrackId id = kv.first;
        const uint16_t channel = AnimTrackChannel(id);
        const uint16_t target = AnimTrackTarget(id);
        if (channel == uint16_t(EAnimChannel::RotX) ||
            channel == uint16_t(EAnimChannel::RotY) ||
            channel == uint16_t(EAnimChannel::RotZ) ||
            channel == uint16_t(EAnimChannel::RotW))
        {
            quat_targets.push_back(target);
            continue;
        }
        const float av = a.Get(id, kv.second);
        out.Set(id, av + (kv.second - av) * t);
    }
    std::sort(quat_targets.begin(), quat_targets.end());
    quat_targets.erase(std::unique(quat_targets.begin(), quat_targets.end()), quat_targets.end());
    for (uint16_t target : quat_targets)
        out.SetQuat(target, NlerpQuat(a.GetQuat(target), b.GetQuat(target), t));
    return out;
}
