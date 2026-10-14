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

TEST(ShadowPlan, ReadsPlacesAndLockesVerbs)
{
    const PlanFile file(
        "module Ahkuilon\n"
        "place KeepGate level 0 1842 24388 451\n"
        "place Tavern object Gus1\n"
        "style hands\n"
        "routine town \"Back for potions\"\n"
        "    travel  Tavern\n"
        "stage s \"Everything Locke does\"\n"
        "    travel  KeepGate\n"
        "    travel  barin evade\n"
        "    use     TOWERIN with \"Golden Sun Key\"\n"
        "    take    Chest@10532,11000\n"
        "    buy     \"Lesser Healing\" 2 from ELAHNI1\n"
        "    sell    \"Bone Sickle\" to GINA1\n"
        "    equip   \"Short Sword\"\n"
        "    unequip \"Short Sword\"\n"
        "    consume \"Apple\"\n"
        "    train   strength agil\n"
        "    train   cons\n"
        "    train\n"
        "    fight   Rahul win\n"
        "    expect  maplevel = 0\n"
        "    grind   until level 4 restock town\n"
        "    grind   until skill Hands 3\n"
        "    expect  skill hands >= 3\n"
        "    sell    junk to CRONUS1\n"
        "    invent\n");
    TShadowPlan plan;
    std::string error;
    ASSERT_TRUE(plan.Load(file.Path(), error)) << error;

    EXPECT_EQ(plan.Style(), EShadowStyle::Hands);
    EXPECT_EQ(ShadowStylePicks(EShadowStyle::Bow), (std::array<int32_t, 2>{2, 3}));
    const SShadowPlace* gate = plan.FindPlace("keepgate");
    ASSERT_NE(gate, nullptr);
    EXPECT_EQ(gate->level, 0);
    EXPECT_EQ(gate->x, 1842);
    EXPECT_EQ(gate->z, 451);
    ASSERT_NE(plan.FindPlace("Tavern"), nullptr);
    EXPECT_EQ(plan.FindPlace("Tavern")->object, "Gus1");

    const std::vector<SShadowStep>& steps = plan.Stages()[0].steps;
    ASSERT_EQ(steps.size(), 19u);
    EXPECT_EQ(steps[0].verb, EShadowVerb::Travel);
    EXPECT_FALSE(steps[0].evade);
    EXPECT_TRUE(steps[1].evade);
    EXPECT_EQ(steps[2].verb, EShadowVerb::Use);
    EXPECT_EQ(steps[2].item, "Golden Sun Key");
    EXPECT_EQ(steps[3].verb, EShadowVerb::Take);
    EXPECT_EQ(steps[3].target, "Chest@10532,11000");
    EXPECT_EQ(steps[4].verb, EShadowVerb::Buy);
    EXPECT_EQ(steps[4].count, 2);
    EXPECT_EQ(steps[4].target, "ELAHNI1");
    EXPECT_EQ(steps[5].verb, EShadowVerb::Sell);
    EXPECT_EQ(steps[5].count, 1);
    EXPECT_EQ(steps[6].verb, EShadowVerb::Equip);
    EXPECT_EQ(steps[7].verb, EShadowVerb::Unequip);
    EXPECT_EQ(steps[8].verb, EShadowVerb::Consume);
    EXPECT_EQ(steps[9].attributes, (std::vector<int32_t>{0, 2}));
    EXPECT_EQ(steps[10].attributes, (std::vector<int32_t>{1, 1}));
    EXPECT_TRUE(steps[11].attributes.empty());
    EXPECT_EQ(steps[11].Describe(), "train (the style's)");
    EXPECT_EQ(steps[12].verb, EShadowVerb::Fight);
    EXPECT_TRUE(steps[12].mustwin);
    EXPECT_EQ(steps[13].condition.subject, EShadowSubject::MapLevel);
    EXPECT_EQ(steps[14].Describe(), "grind until level 4 restock town");
    EXPECT_EQ(steps[14].goal.subject, EShadowSubject::Level);
    EXPECT_EQ(steps[14].routine, "town");
    EXPECT_EQ(steps[15].Describe(), "grind until skill hands 3");
    EXPECT_EQ(steps[15].goal.compare, EShadowCompare::Ge);
    EXPECT_EQ(steps[16].condition.subject, EShadowSubject::Skill);
    EXPECT_EQ(steps[16].Describe(), "expect skill hands >= 3");
    EXPECT_TRUE(steps[17].junk);
    EXPECT_EQ(steps[17].Describe(), "sell junk to CRONUS1");
    EXPECT_EQ(steps[18].verb, EShadowVerb::Invent);
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
    EXPECT_EQ(LoadError("module A\nplace p level 0 1\n"), "2: 'place <name> level' takes <level> <x> <y> [<z>]");
    EXPECT_EQ(LoadError("module A\nplace p here\n"),
              "2: 'place <name>' takes 'level <level> <x> <y> [<z>]' or 'object <name>'");
    EXPECT_EQ(LoadError("module A\nstage a\n  buy \"Apple\" from\n"),
              "3: 'buy' takes \"<item>\" [[upto] <count>] from <merchant>");
    EXPECT_EQ(LoadError("module A\nstage a\n  train speed\n"),
              "3: 'train' takes attributes (strn cons agil rflx mind luck), not 'speed'");
    EXPECT_EQ(LoadError("module A\nstage a\n  fight Rahul now\n"), "3: 'fight' takes a target, then 'win' or nothing");
    EXPECT_EQ(LoadError("module A\nstage a\n  grind until skill swimming 3\n"),
              "3: 'skill' takes a skill (attack defense invoke hands knife sword bludgeon axes bows stealth "
              "lockpick), not 'swimming'");
    EXPECT_EQ(LoadError("module A\nstyle sneaky\n"), "2: 'style' takes one of melee, hands, bow, spells, mixed");
    EXPECT_EQ(LoadError("module A\nstage a\n  train strn agil luck\n"), "3: 'train' takes up to two attributes");
    EXPECT_EQ(LoadError("module A\nstage a\n  grind until level\n"),
              "3: 'grind' takes 'until level <n>' or 'until skill <name> <n>', then 'restock <routine>' or nothing");
}
