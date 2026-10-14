// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    shadowencounters.cpp - the monsters a walk would meet on the way     *
// *************************************************************************

#include "shadowencounters.h"

#include "shadowtravel.h"

#include "../character.h"
#include "../mappane.h"
#include "../player.h"
#include "../revenant.h"

#include <algorithm>
#include <climits>
#include <cmath>

namespace {

// The walk is looked at every this many units: a monster's reach is
// hundreds (COMBATRANGEMAX, 192 by default), so none slips between looks.
constexpr int32_t kLookSpacing = 64;

// Whether `monster`'s look would find Locke standing at `at` (see the header).
bool WouldFind(TCharacter& monster, const S3DPoint& at)
{
    const SCharData* data = monster.GetCharData();
    const S3DPoint from = monster.Pos();
    const int32_t centre = (int32_t)std::lround(std::hypot(double(at.x - from.x), double(at.y - from.y)));
    const int32_t edge = std::max(0, centre - monster.Radius() - Player->Radius());
    return edge <= data->combatrangemax && edge - monster.Radius() - 32 <= data->hearingrange;
}

// What evading still leaves to fight (§5.5): a quarter of them against
// monsters slower than his run, half against the as fast or faster.
constexpr double kSlowShare = 0.25;
constexpr double kFastShare = 0.5;

} // namespace

std::vector<SShadowEncounter> ShadowLegEncounters(const std::vector<S3DPoint>& path, int32_t level)
{
    std::vector<SShadowEncounter> met;
    if (path.empty() || !Player)
        return met;

    // The points to look from, and the box of the walk.
    std::vector<size_t> looks;
    SRect box{INT_MAX, INT_MAX, INT_MIN, INT_MIN};
    int32_t since = kLookSpacing;
    for (size_t i = 0; i < path.size(); ++i)
    {
        if (i > 0)
            since += (int32_t)std::lround(
                std::hypot(double(path[i].x - path[i - 1].x), double(path[i].y - path[i - 1].y)));
        if (since >= kLookSpacing || i + 1 == path.size())
        {
            looks.push_back(i);
            since = 0;
        }
        box.left = std::min(box.left, path[i].x);
        box.top = std::min(box.top, path[i].y);
        box.right = std::max(box.right, path[i].x);
        box.bottom = std::max(box.bottom, path[i].y);
    }

    // The monsters near enough to find him on some of it: no char.def gives
    // a combat range near this.
    constexpr int32_t kWidestReach = 1024;
    SRect around{box.left - kWidestReach, box.top - kWidestReach, box.right + kWidestReach,
                 box.bottom + kWidestReach};
    for (TMapIterator i(level, &around, CHECK_NOINVENT); i; i++)
    {
        if (i->ObjClass() != OBJCLASS_CHARACTER)
            continue;
        auto* monster = static_cast<TCharacter*>(i.Item());
        if (monster->Health() <= 0 || !monster->Aggressive() || !monster->IsEnemy(Player))
            continue;
        for (const size_t at : looks)
            if (WouldFind(*monster, path[at]))
            {
                met.push_back({monster, at});
                break;
            }
    }
    std::sort(met.begin(), met.end(), [](const SShadowEncounter& a, const SShadowEncounter& b) {
        return a.index < b.index;
    });
    return met;
}

size_t ShadowEvadeShare(const std::vector<SShadowEncounter>& encounters, double lockerun)
{
    if (encounters.empty())
        return 0;
    bool outrun = true;
    for (const SShadowEncounter& e : encounters)
        if (TCharacter* monster = e.monster.Get();
            monster && ShadowMoveSpeed(*monster, {"runf", "combatf", "walkf"}) >= lockerun)
            outrun = false;
    const double share = outrun ? kSlowShare : kFastShare;
    return std::max<size_t>(1, (size_t)std::ceil(double(encounters.size()) * share));
}
