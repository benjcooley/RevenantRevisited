// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowplan.h - a Shadow Smoke plan: stages and steps          *
// *************************************************************************
//
// The plan is the playthrough Shadow Smoke performs (docs/shadow/
// ARCHITECTURE.md §9): stages of the story, each a list of steps Locke
// takes in order. It is a text file in the spirit of the game's .s scripts,
// one step to a line:
//
//     module Ahkuilon
//     place  KeepGate level 0 1842 24388 451
//     stage keep "The Keep"
//         start   newgame
//         scene   choose 1 2 3 1 2 3
//         expect  RAHULSTATE = 1
//         travel  RandK
//         talk    RandK choose 1
//         save    "opening"
//
// `#` starts a comment. Words are case-insensitive; quoted text keeps its
// case. An unknown word is an error, so each new verb is added on purpose.

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class EShadowVerb : uint8_t
{
    Start,      // start newgame
    Scene,      // scene [choose n ...]: a scene plays; its menus get these answers
    Travel,     // travel <place|object> [evade]: a proven walk there
    Talk,       // talk <npc> [choose n ...]: Locke goes to the NPC and talks, then as scene
    Use,        // use <object> [with "<item>"]: Locke goes to it and uses it
    Take,       // take <container> [with "<key>"]: Locke goes to it and empties it
    Buy,        // buy "<item>" [<n>] from <merchant>: through the merchant's shop
    Sell,       // sell "<item>" [<n>] to <merchant>
    Equip,      // equip "<item>": into its equipment slot
    Unequip,    // unequip "<item>": back into the pack
    Train,      // train [<attribute> [<attribute>]]: the next level-up's two picks (none: the style's)
    Consume,    // consume "<item>": eat or drink it
    Fight,      // fight <target> [win]: Locke goes to it and fights it, and whoever joins
    Invent,     // invent: cast each combination he holds that makes a spell he doesn't know (§8.6)
    Grind,      // grind until level <n> | skill <name> <n> [restock <routine>]: fight around here until then
    Expect,     // expect <state> <op> <n> | expect level|maplevel <op> <n> | expect skill <name> <op> <n>
                //   | expect have "<item>" [<op> <n>]
    Save,       // save "<name>": a waypoint save
};

// How the run plays (ARCHITECTURE.md §8.4): the plan's `style` line, or
// --shadow-style.
enum class EShadowStyle : uint8_t { Melee, Hands, Bow, Spells, Mixed };
inline constexpr std::array<const char*, 5> kShadowStyleNames = {"melee", "hands", "bow", "spells", "mixed"};
[[nodiscard]] std::optional<EShadowStyle> ShadowParseStyle(const std::string& name);
// The level-up picks of a style: two attributes, 0 Strn .. 5 Luck.
[[nodiscard]] std::array<int32_t, 2> ShadowStylePicks(EShadowStyle style);

// A named spot the plan travels to: a point on a level, or an object found
// by name when the step runs.
struct SShadowPlace
{
    std::string name;
    int32_t     level  = 0;
    int32_t     x      = 0;
    int32_t     y      = 0;
    int32_t     z      = 0;
    std::string object;         // when set, the place is wherever this object is
};

enum class EShadowCompare : uint8_t { Eq, Ne, Lt, Le, Gt, Ge };

// What an `expect` reads.
enum class EShadowSubject : uint8_t
{
    GameState,  // a story variable (state.def), through ScriptManager.GameState
    Level,      // Locke's level
    MapLevel,   // the map level he is on
    Have,       // how many of an item Locke carries, bags included
    Skill,      // one of Locke's skills (kSkillNames: hands, sword, bows, ...)
};

struct SShadowCondition
{
    EShadowSubject subject = EShadowSubject::GameState;
    std::string    name;                            // the state, the item, or the skill
    EShadowCompare compare = EShadowCompare::Eq;
    int32_t        value   = 0;

    [[nodiscard]] bool Holds(int32_t actual) const;
    // "MISTSTATE = 2", "level >= 4", "have Golden Sun Key >= 1".
    [[nodiscard]] std::string Describe() const;
};

struct SShadowStep
{
    EShadowVerb          verb = EShadowVerb::Scene;
    int32_t              line = 0;      // in the plan file
    std::string          target;        // start: "newgame"; travel, talk, use: a place or an object
    std::string          item;          // use: what to use on the target; take: the key; buy, sell: what
    int32_t              count = 1;     // buy, sell: how many
    bool                 upto  = false; // buy: `count` is what he should end up holding
    SShadowCondition     goal;          // grind: what it goes on until (a level or a skill, >=)
    std::string          routine;       // grind: the routine that restocks him
    bool                 evade = false; // travel: run past the leg's monsters (§5.5)
    bool                 mustwin = false; // fight: the target must be dead after
    std::vector<int32_t> choices;       // scene, talk: the answer to each menu, in order
    SShadowCondition     condition;     // expect
    std::string          name;          // save: the waypoint's name
    std::vector<int32_t> attributes;    // train: the picks (0 Strn .. 5 Luck); none: the style's

    // "talk Gus1 choose 1 1 1", for the log.
    [[nodiscard]] std::string Describe() const;
};

struct SShadowStage
{
    std::string              id;
    std::string              title;
    int32_t                  line = 0;
    std::vector<SShadowStep> steps;
};

class TShadowPlan
{
  public:
    // Reads the plan at `path`. False with `error` ("<path>:<line>: what")
    // when it can't be read or doesn't parse.
    bool Load(const std::string& path, std::string& error);

    [[nodiscard]] const std::string& Path() const { return path; }
    [[nodiscard]] const std::string& Module() const { return module; }
    [[nodiscard]] const std::vector<SShadowStage>& Stages() const { return stages; }
    // The index of the stage with this id, -1 when there is none.
    [[nodiscard]] int32_t FindStage(const std::string& id) const;
    // The place with this name, nullptr when there is none.
    [[nodiscard]] const SShadowPlace* FindPlace(const std::string& name) const;
    // A routine: steps a stage's grind runs when it needs them (restocking),
    // written like a stage; nullptr when there is none.
    [[nodiscard]] const SShadowStage* FindRoutine(const std::string& name) const;
    // The play style: the plan's, until the command line sets another.
    [[nodiscard]] EShadowStyle Style() const { return style; }
    void SetStyle(EShadowStyle s) { style = s; }

  private:
    std::string               path;
    std::string               module;
    EShadowStyle              style = EShadowStyle::Melee;
    std::vector<SShadowPlace> places;
    std::vector<SShadowStage> stages;
    std::vector<SShadowStage> routines;
};
