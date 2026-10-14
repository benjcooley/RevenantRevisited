// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadownav.h - Shadow Smoke's proof that a walk exists          *
// *************************************************************************
//
// Shadow Smoke teleports Locke, but only along a walk the game would let
// him take (docs/shadow/ARCHITECTURE.md §5). TShadowWalkField is a level's
// ground as a character of one radius walks it: which 16-unit walk cells he
// can stand on by the game's own rule, which of them connect, and the paths
// between them.
//
// A cell is standable where TCharacter::Blocked would let him stand: the
// real TMapPane::GetWalkHeightRadius at his radius finds no hole, no step
// over 0x20 between the cells in reach, and ground under him. Two
// neighbouring standable cells connect when their heights differ by 0x20
// or less, and a diagonal step also needs both cells beside it standable,
// so a path never cuts a corner the game would stop him at. Connected cells
// form regions; a walk exists between two points of the same region.
//
// The field is built for the level the map pane shows (Locke's) and goes
// stale when any walkmap changes (TSector::WalkmapVersion): a door that
// opens, a barrier that drops.

#pragma once

#include "../revtypes.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

class TShadowWalkField
{
  public:
    // A walk: the cells' centres on the ground, start to end, and its length.
    struct SPath
    {
        std::vector<S3DPoint> points;
        int32_t               length = 0;      // in world units
    };

    // Builds the field of the pane's level for a character of `radius`.
    // False when the level isn't loaded.
    bool Build(int32_t radius);
    // Built for this level and radius, and no walkmap has changed since.
    [[nodiscard]] bool Current(int32_t level, int32_t radius) const;

    [[nodiscard]] int32_t Level() const { return level; }
    [[nodiscard]] int32_t Regions() const { return regions; }
    // The region of the cell under `p`; 0 where Locke can't stand.
    [[nodiscard]] int32_t RegionAt(const S3DPoint& p) const;
    // The standable cell of `region` (any region, for 0) nearest `p` within
    // `range` units, on its ground; none if there is no such cell.
    [[nodiscard]] std::optional<S3DPoint> NearestStandable(const S3DPoint& p, int32_t range,
                                                           int32_t region = 0) const;
    // What stepping onto a cell costs beyond its distance (A* units: 10 a
    // straight step), for steering a walk around places.
    using TStepCost = std::function<int32_t(const S3DPoint& cell)>;
    // A walk from `from` to `to` over connected cells; none when the two
    // aren't in one region.
    [[nodiscard]] std::optional<SPath> FindPath(const S3DPoint& from, const S3DPoint& to,
                                                const TStepCost& extra = {}) const;

  private:
    struct SCell
    {
        int32_t x = 0;
        int32_t y = 0;
    };

    [[nodiscard]] bool Inside(int32_t cx, int32_t cy) const;
    [[nodiscard]] size_t Index(int32_t cx, int32_t cy) const;
    [[nodiscard]] SCell CellOf(const S3DPoint& p) const;
    [[nodiscard]] S3DPoint PointOf(int32_t cx, int32_t cy) const;
    [[nodiscard]] bool Standable(int32_t cx, int32_t cy) const;
    // A step from one cell to its neighbour (dx, dy in -1..1).
    [[nodiscard]] bool Connects(int32_t cx, int32_t cy, int32_t dx, int32_t dy) const;
    void LabelRegions();

    int32_t  level   = -1;
    int32_t  radius  = 0;
    uint64_t version = 0;               // TSector::WalkmapVersion when built
    int32_t  x0      = 0;               // the field's first cell
    int32_t  y0      = 0;
    int32_t  width   = 0;               // in cells
    int32_t  height  = 0;
    int32_t  regions = 0;
    std::vector<uint16_t> ground;       // each cell's walk height (0: none)
    std::vector<uint8_t>  standable;
    std::vector<int32_t>  region;       // 0: not standable
};
