// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *                test_anim_system.cpp - Animation Tests                  *
// *************************************************************************

#include "../src/animsystem.h"

#include <cassert>
#include <cmath>
#include <iostream>

static bool near(float a, float b, float eps = 1e-4f)
{
    return std::fabs(a - b) <= eps;
}

static void test_track_id()
{
    AnimTrackId id = MakeAnimTrackId(42, uint16_t(EAnimChannel::ScaleY));
    assert(AnimTrackTarget(id) == 42);
    assert(AnimTrackChannel(id) == uint16_t(EAnimChannel::ScaleY));
}

static void test_linear_and_step_tracks()
{
    SAnimTrack linear;
    linear.id = AnimTrack(0, EAnimChannel::ScaleX);
    linear.kind = EAnimTrackKind::Linear;
    linear.keys = {{0.0f, 1.0f}, {1.0f, 3.0f}};
    assert(near(linear.Sample(0.5f), 2.0f));
    assert(near(linear.Sample(1.25f, 1.0f, true), 1.5f));

    SAnimTrack step;
    step.id = AnimTrack(0, EAnimChannel::TextureFrame);
    step.kind = EAnimTrackKind::Step;
    step.keys = {{0.0f, 0.0f}, {1.0f, 5.0f}};
    assert(near(step.Sample(0.75f), 0.0f));
    assert(near(step.Sample(1.0f), 5.0f));
}

static void test_clip_and_layer_blend()
{
    SAnimClip a;
    a.name = "a";
    a.duration = 1.0f;
    a.loop = false;
    a.tracks.push_back({AnimTrack(0, EAnimChannel::Alpha), EAnimTrackKind::Linear, {{0, 0}, {1, 1}}});

    SAnimClip b;
    b.name = "b";
    b.duration = 1.0f;
    b.loop = false;
    b.tracks.push_back({AnimTrack(0, EAnimChannel::Alpha), EAnimTrackKind::Linear, {{0, 1}, {1, 0}}});

    SAnimPose pose = EvaluateAnimLayers({
        {&a, 0.5f, 0.25f, EAnimBlendMode::Override},
        {&b, 0.5f, 0.75f, EAnimBlendMode::Override},
    });

    assert(near(pose.Get(AnimTrack(0, EAnimChannel::Alpha)), 0.5f));
}

static void test_quaternion_blend()
{
    SAnimClip identity;
    identity.duration = 1.0f;
    identity.loop = false;
    identity.tracks = {
        {AnimTrack(2, EAnimChannel::RotX), EAnimTrackKind::Constant, {{0, 0}}},
        {AnimTrack(2, EAnimChannel::RotY), EAnimTrackKind::Constant, {{0, 0}}},
        {AnimTrack(2, EAnimChannel::RotZ), EAnimTrackKind::Constant, {{0, 0}}},
        {AnimTrack(2, EAnimChannel::RotW), EAnimTrackKind::Constant, {{0, 1}}},
    };

    SAnimClip z180;
    z180.duration = 1.0f;
    z180.loop = false;
    z180.tracks = {
        {AnimTrack(2, EAnimChannel::RotX), EAnimTrackKind::Constant, {{0, 0}}},
        {AnimTrack(2, EAnimChannel::RotY), EAnimTrackKind::Constant, {{0, 0}}},
        {AnimTrack(2, EAnimChannel::RotZ), EAnimTrackKind::Constant, {{0, 1}}},
        {AnimTrack(2, EAnimChannel::RotW), EAnimTrackKind::Constant, {{0, 0}}},
    };

    SAnimPose pose = EvaluateAnimLayers({
        {&identity, 0.0f, 0.5f, EAnimBlendMode::Override},
        {&z180, 0.0f, 0.5f, EAnimBlendMode::Override},
    });
    SAnimQuat q = pose.GetQuat(2);
    const float inv_sqrt2 = 0.70710678f;
    assert(near(q.z, inv_sqrt2, 1e-3f));
    assert(near(q.w, inv_sqrt2, 1e-3f));
}

int main()
{
    test_track_id();
    test_linear_and_step_tracks();
    test_clip_and_layer_blend();
    test_quaternion_blend();
    std::cout << "anim system tests passed\n";
    return 0;
}

