// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       test_shadowplan.cpp - reading Shadow Smoke plans                 *
// *************************************************************************
//
// The plan format (docs/shadow/ARCHITECTURE.md §9.1): what parses, what it
// parses to, and that every mistake is refused with its line.

#include "../src/shadow/shadowplan.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace {

// A plan written to a file of its own, removed afterwards.
class PlanFile
{
  public:
    explicit PlanFile(const std::string& text)
    {
        static int count = 0;
        path = std::filesystem::temp_directory_path() /
               ("test_shadowplan_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()) + "_" +
                std::to_string(count++) + ".plan");
        std::ofstream(path, std::ios::binary) << text;
    }
    ~PlanFile() { std::filesystem::remove(path); }
    [[nodiscard]] std::string Path() const { return path.string(); }

  private:
    std::filesystem::path path;
};

// The error a plan is refused with, after its "<path>:" prefix; empty when
// it loads.
std::string LoadError(const std::string& text)
{
    const PlanFile file(text);
    TShadowPlan plan;
    std::string error;
    if (plan.Load(file.Path(), error))
        return {};
    return error.substr(file.Path().size() + 1);
}

}  // namespace

TEST(ShadowPlan, ReadsStagesAndSteps)
{
    const PlanFile file(
        "# a comment\r\n"
        "module Ahkuilon\r\n"
        "\r\n"
        "stage opening \"Locke wakes in the Keep\"\r\n"
        "    start   newgame\r\n"
        "    scene   choose 1 2 3   # every line\r\n"
        "    expect  RAHULSTATE = 1\r\n"
        "    save    \"opening\"\r\n"
        "stage s1 \"Gus's deal\"\r\n"
        "    talk    Gus1 choose 1 1 1\r\n"
        "    expect  level >= 2\r\n"
        "    expect  have \"Golden Sun Key\"\r\n"
        "    expect  have \"Diamond Ring\" = 0\r\n");
    TShadowPlan plan;
    std::string error;
    ASSERT_TRUE(plan.Load(file.Path(), error)) << error;

    EXPECT_EQ(plan.Module(), "Ahkuilon");
    ASSERT_EQ(plan.Stages().size(), 2u);
    EXPECT_EQ(plan.FindStage("S1"), 1);
    EXPECT_EQ(plan.FindStage("s9"), -1);

    const SShadowStage& opening = plan.Stages()[0];
    EXPECT_EQ(opening.id, "opening");
    EXPECT_EQ(opening.title, "Locke wakes in the Keep");
    EXPECT_EQ(opening.line, 4);
    ASSERT_EQ(opening.steps.size(), 4u);
    EXPECT_EQ(opening.steps[0].verb, EShadowVerb::Start);
    EXPECT_EQ(opening.steps[1].verb, EShadowVerb::Scene);
    EXPECT_EQ(opening.steps[1].choices, (std::vector<int32_t>{1, 2, 3}));
    EXPECT_EQ(opening.steps[1].line, 6);
    EXPECT_EQ(opening.steps[2].condition.subject, EShadowSubject::GameState);
    EXPECT_EQ(opening.steps[2].condition.name, "RAHULSTATE");
    EXPECT_EQ(opening.steps[2].condition.value, 1);
    EXPECT_EQ(opening.steps[3].verb, EShadowVerb::Save);
    EXPECT_EQ(opening.steps[3].name, "opening");

    const SShadowStage& s1 = plan.Stages()[1];
    EXPECT_EQ(s1.steps[0].Describe(), "talk Gus1 choose 1 1 1");
    EXPECT_EQ(s1.steps[1].condition.subject, EShadowSubject::Level);
    EXPECT_EQ(s1.steps[1].condition.compare, EShadowCompare::Ge);
    EXPECT_EQ(s1.steps[2].Describe(), "expect have \"Golden Sun Key\" >= 1");
    EXPECT_EQ(s1.steps[3].Describe(), "expect have \"Diamond Ring\" = 0");
}

TEST(ShadowPlan, ConditionsCompare)
{
    SShadowCondition c;
    c.compare = EShadowCompare::Lt;
    c.value = 3;
    EXPECT_TRUE(c.Holds(2));
    EXPECT_FALSE(c.Holds(3));
    c.compare = EShadowCompare::Ne;
    EXPECT_TRUE(c.Holds(2));
    EXPECT_FALSE(c.Holds(3));
}

TEST(ShadowPlan, RefusesMistakesByLine)
{
    EXPECT_EQ(LoadError("module A\nscene\n"), "2: 'scene' outside a stage");
    EXPECT_EQ(LoadError("module A\nstage a\n  dance\n"), "3: unknown word 'dance'");
    EXPECT_EQ(LoadError("module A\nstage a\n  scene choose 0\n"), "3: a choice is a number from 1 to 9, not '0'");
    EXPECT_EQ(LoadError("module A\nstage a\n  scene choose\n"), "3: 'choose' needs at least one answer");
    EXPECT_EQ(LoadError("module A\nstage a\n  scene 3\n"), "3: expected 'choose' or the end of the line");
    EXPECT_EQ(LoadError("module A\nstage a\n  save \"open\n"), "3: unterminated quote");
    EXPECT_EQ(LoadError("module A\nstage a\nstage a\n"), "3: a second stage 'a'");
    EXPECT_EQ(LoadError("module A\nstage a\n  expect MISTSTATE 2\n"),
              "3: expected a comparison: <op> <number>, with <op> one of = != < <= > >=");
    EXPECT_EQ(LoadError("module A\nstage a\n  start loadgame\n"), "3: 'start' takes 'newgame'");
    EXPECT_EQ(LoadError("stage a\n  scene\n"), "2: no 'module' line");
    EXPECT_EQ(LoadError("module A\n"), "1: no stages");
}
