// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadownav.cpp - Shadow Smoke's proof that a walk exists        *
// *************************************************************************

#include "shadownav.h"

#include "../gamemap.h"
#include "../mapmanager.h"
#include "../mappane.h"
#include "../revenant.h"
#include "../sector.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <unordered_map>

namespace {

constexpr int32_t kCellsPerSector = 1 << (SECTORWSHIFT - WALKMAPSHIFT);
constexpr int32_t kCellSize       = WALKMAPGRANULARITY;
constexpr int32_t kMaxStep        = 0x20;       // TCharacter::Blocked's step, up or down
// A* costs: a straight step, a diagonal one (10 and 14 for 1 and sqrt 2).
constexpr int32_t kStraight = 10;
constexpr int32_t kDiagonal = 14;

int32_t Octile(int32_t dx, int32_t dy)
{
    dx = std::abs(dx);
    dy = std::abs(dy);
    return kStraight * std::max(dx, dy) + (kDiagonal - kStraight) * std::min(dx, dy);
}

} // namespace

bool TShadowWalkField::Build(int32_t r)
{
    const TGameMap* map = MapManager.CurrentMap();
    if (!map || !map->IsLoaded() || map->Level() != MapPane.GetMapLevel() || map->Sectors().empty())
        return false;

    int32_t minsx = INT_MAX, minsy = INT_MAX, maxsx = INT_MIN, maxsy = INT_MIN;
    for (const TSector* sec : map->Sectors())
    {
        if (!sec)
            continue;
        minsx = std::min(minsx, sec->SectorX());
        minsy = std::min(minsy, sec->SectorY());
        maxsx = std::max(maxsx, sec->SectorX());
        maxsy = std::max(maxsy, sec->SectorY());
    }
    level = map->Level();
    radius = r;
    version = TSector::WalkmapVersion();
    x0 = minsx * kCellsPerSector;
    y0 = minsy * kCellsPerSector;
    width = (maxsx - minsx + 1) * kCellsPerSector;
    height = (maxsy - minsy + 1) * kCellsPerSector;
    const size_t cells = size_t(width) * size_t(height);
    ground.assign(cells, 0);
    standable.assign(cells, 0);
    region.assign(cells, 0);

    for (const TSector* sec : map->Sectors())
    {
        if (!sec)
            continue;
        const int32_t cx = sec->SectorX() * kCellsPerSector;
        const int32_t cy = sec->SectorY() * kCellsPerSector;
        for (int32_t wy = 0; wy < kCellsPerSector; ++wy)
            for (int32_t wx = 0; wx < kCellsPerSector; ++wx)
                ground[Index(cx + wx, cy + wy)] = (uint16_t)sec->ReturnWalkmap(wx, wy);
    }

    // The game's own test of a place to stand, cell by cell.
    for (int32_t cy = y0; cy < y0 + height; ++cy)
        for (int32_t cx = x0; cx < x0 + width; ++cx)
        {
            const size_t i = Index(cx, cy);
            if (!ground[i])
                continue;
            int32_t maxdelta = 0, h = 0;
            bool hole = false;
            MapPane.GetWalkHeightRadius(PointOf(cx, cy), radius, maxdelta, h, hole);
            standable[i] = !hole && h != 0 && maxdelta <= kMaxStep;
        }

    LabelRegions();
    return true;
}

bool TShadowWalkField::Current(int32_t lvl, int32_t r) const
{
    return level == lvl && radius == r && version == TSector::WalkmapVersion();
}

bool TShadowWalkField::Inside(int32_t cx, int32_t cy) const
{
    return cx >= x0 && cy >= y0 && cx < x0 + width && cy < y0 + height;
}

size_t TShadowWalkField::Index(int32_t cx, int32_t cy) const
{
    return size_t(cy - y0) * size_t(width) + size_t(cx - x0);
}

TShadowWalkField::SCell TShadowWalkField::CellOf(const S3DPoint& p) const
{
    // A cell's centre is its coordinate times 16 (TMapPane::GetWalkHeight).
    return {(p.x + kCellSize / 2) >> WALKMAPSHIFT, (p.y + kCellSize / 2) >> WALKMAPSHIFT};
}

S3DPoint TShadowWalkField::PointOf(int32_t cx, int32_t cy) const
{
    const int32_t z = Inside(cx, cy) ? ground[Index(cx, cy)] : 0;
    return S3DPoint(cx * kCellSize, cy * kCellSize, z);
}

bool TShadowWalkField::Standable(int32_t cx, int32_t cy) const
{
    return Inside(cx, cy) && standable[Index(cx, cy)];
}

bool TShadowWalkField::Connects(int32_t cx, int32_t cy, int32_t dx, int32_t dy) const
{
    const int32_t nx = cx + dx, ny = cy + dy;
    if (!Standable(nx, ny))
        return false;
    if (std::abs(int32_t(ground[Index(cx, cy)]) - int32_t(ground[Index(nx, ny)])) > kMaxStep)
        return false;
    // No cutting a corner: a diagonal needs both cells beside it.
    return dx == 0 || dy == 0 || (Standable(cx + dx, cy) && Standable(cx, cy + dy));
}

void TShadowWalkField::LabelRegions()
{
    regions = 0;
    std::vector<SCell> queue;
    for (int32_t cy = y0; cy < y0 + height; ++cy)
        for (int32_t cx = x0; cx < x0 + width; ++cx)
        {
            const size_t start = Index(cx, cy);
            if (!standable[start] || region[start])
                continue;
            const int32_t id = ++regions;
            region[start] = id;
            queue.assign(1, SCell{cx, cy});
            while (!queue.empty())
            {
                const SCell c = queue.back();
                queue.pop_back();
                for (int32_t dy = -1; dy <= 1; ++dy)
                    for (int32_t dx = -1; dx <= 1; ++dx)
                    {
                        if ((dx || dy) && Connects(c.x, c.y, dx, dy))
                        {
                            const size_t n = Index(c.x + dx, c.y + dy);
                            if (!region[n])
                            {
                                region[n] = id;
                                queue.push_back({c.x + dx, c.y + dy});
                            }
                        }
                    }
            }
        }
}

int32_t TShadowWalkField::RegionAt(const S3DPoint& p) const
{
    const SCell c = CellOf(p);
    return Inside(c.x, c.y) ? region[Index(c.x, c.y)] : 0;
}

std::optional<S3DPoint> TShadowWalkField::NearestStandable(const S3DPoint& p, int32_t range, int32_t wanted) const
{
    const SCell c = CellOf(p);
    const int32_t rings = range / kCellSize;
    std::optional<S3DPoint> best;
    int64_t bestdist = INT64_MAX;
    // Ring by ring outward; a ring's nearest cell can still lose to the next
    // ring's (the square's corners), so search one ring past the first hit.
    int32_t lastring = rings;
    for (int32_t ring = 0; ring <= lastring; ++ring)
    {
        for (int32_t dy = -ring; dy <= ring; ++dy)
            for (int32_t dx = -ring; dx <= ring; ++dx)
            {
                if (std::max(std::abs(dx), std::abs(dy)) != ring)
                    continue;
                const int32_t cx = c.x + dx, cy = c.y + dy;
                if (!Standable(cx, cy) || (wanted && region[Index(cx, cy)] != wanted))
                    continue;
                const S3DPoint at = PointOf(cx, cy);
                const int64_t d = int64_t(at.x - p.x) * (at.x - p.x) + int64_t(at.y - p.y) * (at.y - p.y);
                if (d > int64_t(range) * range || d >= bestdist)
                    continue;
                bestdist = d;
                best = at;
                lastring = std::min(lastring, ring + 1);
            }
    }
    return best;
}

std::optional<TShadowWalkField::SPath> TShadowWalkField::FindPath(const S3DPoint& from, const S3DPoint& to,
                                                                  const TStepCost& extra) const
{
    const SCell a = CellOf(from);
    const SCell b = CellOf(to);
    if (!Standable(a.x, a.y) || !Standable(b.x, b.y) || region[Index(a.x, a.y)] != region[Index(b.x, b.y)])
        return std::nullopt;

    // A* over the cells, octile distance; ties go to the newer node.
    struct SOpen
    {
        int32_t f = 0;
        size_t  cell = 0;
        bool operator>(const SOpen& o) const { return f > o.f; }
    };
    std::priority_queue<SOpen, std::vector<SOpen>, std::greater<>> open;
    std::unordered_map<size_t, int32_t> g;          // best cost found to a cell
    std::unordered_map<size_t, size_t>  parent;
    const size_t start = Index(a.x, a.y);
    const size_t goal = Index(b.x, b.y);
    g[start] = 0;
    open.push({Octile(b.x - a.x, b.y - a.y), start});

    while (!open.empty())
    {
        const SOpen top = open.top();
        open.pop();
        if (top.cell == goal)
            break;
        const int32_t cx = x0 + int32_t(top.cell % size_t(width));
        const int32_t cy = y0 + int32_t(top.cell / size_t(width));
        const int32_t cost = g[top.cell];
        if (top.f - Octile(b.x - cx, b.y - cy) > cost)
            continue;                               // a stale entry
        for (int32_t dy = -1; dy <= 1; ++dy)
            for (int32_t dx = -1; dx <= 1; ++dx)
            {
                if ((!dx && !dy) || !Connects(cx, cy, dx, dy))
                    continue;
                const size_t n = Index(cx + dx, cy + dy);
                int32_t next = cost + ((dx && dy) ? kDiagonal : kStraight);
                if (extra)
                    next += extra(PointOf(cx + dx, cy + dy));
                const auto known = g.find(n);
                if (known != g.end() && known->second <= next)
                    continue;
                g[n] = next;
                parent[n] = top.cell;
                open.push({next + Octile(b.x - (cx + dx), b.y - (cy + dy)), n});
            }
    }
    if (!g.count(goal))
        return std::nullopt;

    SPath path;
    for (size_t at = goal;; at = parent[at])
    {
        path.points.push_back(PointOf(x0 + int32_t(at % size_t(width)), y0 + int32_t(at / size_t(width))));
        if (at == start)
            break;
    }
    std::reverse(path.points.begin(), path.points.end());
    double length = 0.0;
    for (size_t i = 1; i < path.points.size(); ++i)
        length += std::hypot(double(path.points[i].x - path.points[i - 1].x),
                             double(path.points[i].y - path.points[i - 1].y));
    path.length = int32_t(std::lround(length));
    return path;
}
