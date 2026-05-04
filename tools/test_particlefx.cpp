// *************************************************************************
// *                         Cinematix Revenant                            *
// *              test_particlefx.cpp - Particle FX VM Tests                *
// *************************************************************************

#include "../src/defdoc.h"
#include "../src/particlefx.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace {

void expect_near(float a, float b)
{
    EXPECT_NEAR(a, b, 0.0001f);
}

void expect_vec_near(const float* values, std::initializer_list<float> expected)
{
    int i = 0;
    for (float e : expected)
        expect_near(values[i++], e);
}

defdoc::Document load_effect_defs()
{
    std::filesystem::path path =
        std::filesystem::current_path() / "data" / "Resources" / "effects.def";
    if (!std::filesystem::exists(path))
        path = std::filesystem::current_path() / ".." / "data" / "Resources" / "effects.def";
    std::ifstream in(path);
    if (!in)
    {
        ADD_FAILURE() << "failed to open " << path;
        return {};
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return defdoc::Document::parse(ss.str());
}

TEST(ParticleFxExpression, ScalarMath)
{
    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile("1 + 2 * 3", &error)) << error;
    expect_near(expr.Eval({}), 7.0f);
    EXPECT_GE(expr.MaxStackDepth(), 2);

    ASSERT_TRUE(expr.Compile("min(max(10, 2), 7)", &error)) << error;
    expect_near(expr.Eval({}), 7.0f);

    ASSERT_TRUE(expr.Compile("clamp(-2, 0, 1)", &error)) << error;
    expect_near(expr.Eval({}), 0.0f);

    ASSERT_TRUE(expr.Compile("lerp(10, 20, 0.25)", &error)) << error;
    expect_near(expr.Eval({}), 12.5f);
}

TEST(ParticleFxExpression, UnaryMathFunctions)
{
    TParticleExpression expr;
    std::string error;

    ASSERT_TRUE(expr.Compile("floor(2.9)", &error)) << error;
    expect_near(expr.Eval({}), 2.0f);

    ASSERT_TRUE(expr.Compile("sin(0)", &error)) << error;
    expect_near(expr.Eval({}), 0.0f);

    ASSERT_TRUE(expr.Compile("cos(0)", &error)) << error;
    expect_near(expr.Eval({}), 1.0f);

    ASSERT_TRUE(expr.Compile("-5 + 2", &error)) << error;
    expect_near(expr.Eval({}), -3.0f);
}

TEST(ParticleFxExpression, Variables)
{
    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile("time_frame + age + age01 + seed", &error)) << error;

    SParticleEvalContext ctx = {};
    ctx.time_frame = 10.0f;
    ctx.age = 2.0f;
    ctx.age01 = 0.5f;
    ctx.seed = 7.0f;
    expect_near(expr.Eval(ctx), 19.5f);
}

TEST(ParticleFxExpression, VectorConstructors)
{
    TParticleExpression expr;
    std::string error;

    ASSERT_TRUE(expr.Compile("vec2(1, 2)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 2);
    float v2[2] = {};
    expr.Eval({}, v2, 2);
    expect_vec_near(v2, {1.0f, 2.0f});

    ASSERT_TRUE(expr.Compile("vec3(1, 2, 3)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 3);
    float v3[3] = {};
    expr.Eval({}, v3, 3);
    expect_vec_near(v3, {1.0f, 2.0f, 3.0f});

    ASSERT_TRUE(expr.Compile("vec4(1, 2, 3, 4)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 4);
    float v4[4] = {};
    expr.Eval({}, v4, 4);
    expect_vec_near(v4, {1.0f, 2.0f, 3.0f, 4.0f});
}

TEST(ParticleFxExpression, VectorMathAndBroadcast)
{
    TParticleExpression expr;
    std::string error;

    ASSERT_TRUE(expr.Compile("vec3(1, 2, 3) + 10", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 3);
    float add[3] = {};
    expr.Eval({}, add, 3);
    expect_vec_near(add, {11.0f, 12.0f, 13.0f});

    ASSERT_TRUE(expr.Compile("vec2(2, 4) * vec2(3, 5)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 2);
    float mul[2] = {};
    expr.Eval({}, mul, 2);
    expect_vec_near(mul, {6.0f, 20.0f});

    ASSERT_TRUE(expr.Compile("clamp(vec3(-1, 0.5, 2), 0, 1)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 3);
    float clampv[3] = {};
    expr.Eval({}, clampv, 3);
    expect_vec_near(clampv, {0.0f, 0.5f, 1.0f});

    ASSERT_TRUE(expr.Compile("lerp(vec2(0, 10), vec2(10, 30), 0.5)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 2);
    float lerpv[2] = {};
    expr.Eval({}, lerpv, 2);
    expect_vec_near(lerpv, {5.0f, 20.0f});
}

TEST(ParticleFxExpression, Frame)
{
    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile("frame(time_frame, 0.458333333)", &error)) << error;

    SParticleEvalContext ctx = {};
    ctx.time_frame = 0.0f;
    expect_near(expr.Eval(ctx), 0.0f);
    ctx.time_frame = 3.0f;
    expect_near(expr.Eval(ctx), 1.0f);
    ctx.time_frame = 17.0f;
    expect_near(expr.Eval(ctx), 7.0f);
    ctx.time_frame = 18.0f;
    expect_near(expr.Eval(ctx), 8.0f);
}

TEST(ParticleFxExpression, Flipbook)
{
    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile("flipbook(frame(time_frame, 0.458333333), 4, 2)", &error)) << error;
    EXPECT_EQ(expr.ResultLanes(), 4);

    SParticleEvalContext ctx = {};
    ctx.time_frame = 17.0f;
    float uv[4] = {};
    expr.Eval(ctx, uv, 4);

    expect_near(uv[0], 0.75f);
    expect_near(uv[1], 0.5f);
    expect_near(uv[2], 0.25f);
    expect_near(uv[3], 0.5f);
}

TEST(ParticleFxExpression, CompileMetadata)
{
    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile("flipbook(frame(time_frame, 0.458333333), 4, 2)", &error)) << error;

    EXPECT_TRUE(expr.IsValid());
    EXPECT_EQ(expr.Source(), "flipbook(frame(time_frame, 0.458333333), 4, 2)");
    EXPECT_EQ(expr.ResultLanes(), 4);
    EXPECT_GT(expr.MaxStackDepth(), 0);
    EXPECT_FALSE(expr.Code().empty());
    EXPECT_FALSE(expr.Constants().empty());
}

TEST(ParticleFxExpression, InvalidExpressions)
{
    TParticleExpression expr;
    std::string error;
    EXPECT_FALSE(expr.Compile("1 +", &error));
    EXPECT_FALSE(error.empty());

    error.clear();
    EXPECT_FALSE(expr.Compile("unknown_value", &error));
    EXPECT_FALSE(error.empty());

    error.clear();
    EXPECT_FALSE(expr.Compile("flipbook(1, 2)", &error));
    EXPECT_FALSE(error.empty());
}

TEST(ParticleFxDefs, TorchFlame)
{
    defdoc::Document doc = load_effect_defs();
    auto effects = doc.root().blocks("effect");
    ASSERT_FALSE(effects.empty());

    const defdoc::Node* torch = nullptr;
    for (const defdoc::Node* e : effects)
    {
        if ((*e)["$name"].as_string() == "TorchFlame")
        {
            torch = e;
            break;
        }
    }
    ASSERT_NE(torch, nullptr);
    EXPECT_EQ(torch->get_string("debug"), "normal");
    EXPECT_EQ(torch->get_int("inactive_ttl_pulses"), 12);

    auto buckets = torch->blocks("bucket");
    ASSERT_EQ(buckets.size(), 1);
    const defdoc::Node* bucket = buckets[0];
    EXPECT_EQ((*bucket)["$name"].as_string(), "flame_billboard");
    EXPECT_EQ(bucket->get_int("atlas_cols"), 4);
    EXPECT_EQ(bucket->get_int("atlas_rows"), 2);
    ASSERT_EQ((*bucket)["anchor"].as_array().size(), 2);
    EXPECT_DOUBLE_EQ((*bucket)["anchor"].as_array()[0].as_double(), 0.5);
    EXPECT_DOUBLE_EQ((*bucket)["anchor"].as_array()[1].as_double(), 1.0);
    EXPECT_DOUBLE_EQ(bucket->get_double("scale"), 0.5);
    EXPECT_TRUE(bucket->get_bool("flip_v"));
    ASSERT_EQ((*bucket)["chroma_key"].as_array().size(), 3);
    EXPECT_EQ((*bucket)["chroma_key"].as_array()[0].as_int(), 255);
    EXPECT_EQ(bucket->get_string("frame_expr"), "frame(time_frame, 13.0)");
    EXPECT_EQ(bucket->get_string("uv_rect_expr"), "flipbook(frame(time_frame, 13.0), 4, 2)");

    auto emitters = torch->blocks("emitter");
    ASSERT_EQ(emitters.size(), 1);
    const defdoc::Node* emitter = emitters[0];
    EXPECT_EQ((*emitter)["$name"].as_string(), "billboard");
    ASSERT_EQ((*emitter)["local_pos"].as_array().size(), 3);
    EXPECT_DOUBLE_EQ((*emitter)["local_pos"].as_array()[0].as_double(), 0.0);
    EXPECT_DOUBLE_EQ((*emitter)["local_pos"].as_array()[2].as_double(), 32.0);
    auto outputs = emitter->blocks("output");
    ASSERT_EQ(outputs.size(), 1);
    EXPECT_EQ((*outputs[0])["$name"].as_string(), "flame_billboard");
    EXPECT_DOUBLE_EQ(outputs[0]->get_double("life"), -1.0);

    TParticleExpression expr;
    std::string error;
    ASSERT_TRUE(expr.Compile(bucket->get_string("uv_rect_expr").c_str(), &error)) << error;
    SParticleEvalContext ctx = {};
    ctx.time_frame = 0.70f;
    float uv[4] = {};
    expr.Eval(ctx, uv, 4);
    expect_near(uv[0], 0.25f);
    expect_near(uv[1], 0.0f);
}

TEST(ParticleFxBucket, LayoutAndOwnerCleanup)
{
    SParticleBufferLayout layout = {};
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::OwnerId));
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::Life));
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::DrawPos));
    ASSERT_EQ(layout.vars[int(EParticleVar::OwnerId)].offset, 0);
    ASSERT_EQ(layout.vars[int(EParticleVar::OwnerId)].lanes, 1);
    ASSERT_EQ(layout.vars[int(EParticleVar::Life)].offset, 1);
    ASSERT_EQ(layout.vars[int(EParticleVar::DrawPos)].lanes, 3);
    ASSERT_GT(layout.stride_floats, 0);

    SParticleBucketDesc desc = {};
    desc.name = "test_bucket";
    TParticleBucket bucket(desc, layout);
    EXPECT_FALSE(bucket.Active());

    const int p0 = bucket.AddParticle(10.0f, -1.0f);
    const int p1 = bucket.AddParticle(20.0f, 0.5f);
    const int p2 = bucket.AddParticle(10.0f, 1.0f);
    ASSERT_EQ(p0, 0);
    ASSERT_EQ(p1, 1);
    ASSERT_EQ(p2, 2);
    EXPECT_TRUE(bucket.Active());
    EXPECT_EQ(bucket.Count(), 3);

    float* p0_pos = bucket.VarPtr(p0, EParticleVar::DrawPos);
    ASSERT_NE(p0_pos, nullptr);
    p0_pos[0] = 1.0f;
    p0_pos[1] = 2.0f;
    p0_pos[2] = 3.0f;

    bucket.KillParticlesByOwner(10.0f);
    EXPECT_EQ(bucket.Count(), 1);
    const float* owner = bucket.VarPtr(0, EParticleVar::OwnerId);
    ASSERT_NE(owner, nullptr);
    expect_near(*owner, 20.0f);

    bucket.KillParticlesByOwner(20.0f);
    EXPECT_EQ(bucket.Count(), 0);
    EXPECT_FALSE(bucket.Active());
}

TEST(ParticleFxManager, GlobalBucketLookup)
{
    SParticleBufferLayout layout = {};
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::OwnerId));
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::Life));

    SParticleBucketDesc desc = {};
    desc.name = "flame_billboard";

    TParticleManager manager;
    TParticleBucket* a = manager.GetOrCreateGlobalBucket(desc, layout);
    TParticleBucket* b = manager.GetOrCreateGlobalBucket(desc, layout);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a, b);
    EXPECT_EQ(manager.FindGlobalBucket("flame_billboard"), a);
    EXPECT_EQ(manager.FindGlobalBucket("missing"), nullptr);

    manager.Clear();
    EXPECT_EQ(manager.FindGlobalBucket("flame_billboard"), nullptr);
}

TEST(ParticleFxManager, GlobalSingleton)
{
    ParticleManager().Clear();

    SParticleBufferLayout layout = {};
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::OwnerId));
    ASSERT_TRUE(ParticleLayoutAddVar(layout, EParticleVar::Life));

    SParticleBucketDesc desc = {};
    desc.name = "singleton_bucket";

    TParticleBucket* bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
    ASSERT_NE(bucket, nullptr);
    EXPECT_EQ(ParticleManager().FindGlobalBucket("singleton_bucket"), bucket);

    ParticleManager().Clear();
    EXPECT_EQ(ParticleManager().FindGlobalBucket("singleton_bucket"), nullptr);
}

TEST(ParticleFxManager, DrawPulsePassAdvances)
{
    ParticleManager().Clear();
    EXPECT_EQ(ParticleManager().DrawPulsePass(), 0u);

    ParticleManager().BeginDrawPulsePass();
    EXPECT_EQ(ParticleManager().DrawPulsePass(), 1u);

    ParticleManager().BeginDrawPulsePass();
    EXPECT_EQ(ParticleManager().DrawPulsePass(), 2u);

    ParticleManager().Clear();
    EXPECT_EQ(ParticleManager().DrawPulsePass(), 0u);
}

} // namespace
