// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowcoverage.h - the script lines a run executes, and when      *
// *************************************************************************
//
// docs/shadow/ARCHITECTURE.md §9.4 and §9.5. Every script line the game
// runs is recorded by its source file, its OBJECT block and its line in the
// block (0 is the OBJECT line: a prototype's text starts there), with the
// last tick it ran; and every story variable's change, with its tick.
// tools/shadow/shadow.py matches them against tools/shadow/storymap.py's
// writers and readers (file:line, and each one's OBJECT line): which
// writers fired, and whether a town reader of a changed variable ran
// after the change -- the town only sees a new state once Locke is back.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class TScript;

class TShadowCoverage
{
  public:
    // A script line about to run (TScript::IStepObserver's BeforeLine).
    void Line(TScript& script, int32_t offset, int64_t tick);
    // A story variable changed between two ticks.
    void StateChanged(const std::string& name, int32_t was, int32_t is, int64_t tick);

    // All of it as JSON: {"blocks": [{"file", "object", "lines": {line: tick}}], "changes": [...]}.
    [[nodiscard]] std::string Json() const;

  private:
    struct SChange
    {
        std::string name;
        int32_t     was  = 0;
        int32_t     is   = 0;
        int64_t     tick = 0;
    };

    // By file, then OBJECT name: each line run, and the last tick it ran.
    std::map<std::string, std::map<std::string, std::map<int32_t, int64_t>>> lines;
    std::vector<SChange> changes;
};
