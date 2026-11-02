// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *     shadowzones.h - where walking sets a script off (CUBE, PROXIMITY)   *
// *************************************************************************
//
// A script's CUBE trigger fires when Locke stands in its box, and a
// PROXIMITY trigger named for him fires when he comes within its distance
// of the object it runs on (TScript::Triggered). Both test where he is each
// tick, not where he passed, so a teleport over one doesn't fire it and a
// walk through it would (docs/shadow/ARCHITECTURE.md §5.4). Travel routes
// around these zones when it can, and stops in one when it can't.
//
// The zones come from the scripts themselves: every script area.def gives
// the level, and master.s, parsed with the engine's own TScriptProto (as
// the script manager does, without loading them). Only the camera's area
// has its scripts loaded at any time, so reading the files is what sees the
// whole level. A zone counts only if an object it runs on stands on the
// level; a CUBE given a region name instead of corners never fires
// (retail's trigger test reads only the corners).

#pragma once

#include "engine/core/revtypes.h"

#include <cstdint>
#include <string>
#include <vector>

class TShadowLog;

struct SShadowZone
{
    enum class EKind : uint8_t { Cube, Proximity };

    EKind       kind = EKind::Cube;
    std::string owner;              // the prototype's name: what it runs on
    std::string file;               // the script file
    S3DPoint    low{};              // Cube: its corners (S3DRect's, which
    S3DPoint    high{};             // copies only from a non-const one)
    S3DPoint    centre{};           // Proximity: the object's position
    int32_t     dist = 0;           // Proximity: the trigger's distance

    // Whether Locke at `p` sets it off, as TScript::Triggered tests it.
    [[nodiscard]] bool Contains(const S3DPoint& p) const;
    // "cube Sardok4 (cave.s)".
    [[nodiscard]] std::string Describe() const;
};

class TShadowZones
{
  public:
    // The zones of `level` (logged).
    void Build(int32_t level, TShadowLog& log);
    [[nodiscard]] int32_t Level() const { return level; }
    [[nodiscard]] const std::vector<SShadowZone>& Zones() const { return zones; }
    // The first zone that `p` sets off, -1 for none.
    [[nodiscard]] int32_t ZoneAt(const S3DPoint& p) const;

  private:
    void Read(const std::string& file);

    int32_t                  level = -1;
    std::vector<SShadowZone> zones;
};
