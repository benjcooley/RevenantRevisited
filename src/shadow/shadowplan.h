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
//     stage keep "The Keep"
//         start   newgame
//         scene   choose 3 3 3 3 3 3 3 3 3
//         expect  RAHULSTATE = 1
//         save    "opening"
//
// `#` starts a comment. Words are case-insensitive; quoted text keeps its
// case. An unknown word is an error, so each new verb is added on purpose.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class EShadowVerb : uint8_t
{
    Start,      // start newgame
    Scene,      // scene [choose n ...]: a scene plays; its menus get these answers
    Talk,       // talk <npc> [choose n ...]: Locke talks to the NPC, then as scene
    Expect,     // expect <state> <op> <n> | expect level <op> <n> | expect have "<item>" [<op> <n>]
    Save,       // save "<name>": a waypoint save
};

enum class EShadowCompare : uint8_t { Eq, Ne, Lt, Le, Gt, Ge };

// What an `expect` reads.
enum class EShadowSubject : uint8_t
{
    GameState,  // a story variable (state.def), through ScriptManager.GameState
    Level,      // Locke's level
    Have,       // how many of an item Locke carries, bags included
};

struct SShadowCondition
{
    EShadowSubject subject = EShadowSubject::GameState;
    std::string    name;                            // the state, or the item
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
    std::string          target;        // start: "newgame"; talk: the NPC
    std::vector<int32_t> choices;       // scene, talk: the answer to each menu, in order
    SShadowCondition     condition;     // expect
    std::string          name;          // save: the waypoint's name

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

  private:
    std::string               path;
    std::string               module;
    std::vector<SShadowStage> stages;
};
