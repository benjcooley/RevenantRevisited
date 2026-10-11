"""Walking toward a point beyond the next few steps.

    maps = LevelMaps()
    maps.merge(level, walkgrid)                              # every walk grid observed
    path = maps.plan(level, locke_pos, target_pos, radius)   # world points, cell centres
    point = farthest_clear(grid, locke_pos, path)            # the next click

The host's walk grid covers only the sectors loaded round Locke, so each one
observed is merged into a per-level map that grows as he goes; cells no
loaded sector held stay unknown. A* over it, stepping cell to cell by the
game's own rule (TCharacter::Blocked, FindClearPath `0x004c39d0`, with
TMapPane::GetWalkHeightRadius `0x004530a0`): at the new spot, every walk
cell within Locke's radius must have floor, no two side by side may differ
by more than 32, and the floor there may not be more than 32 above or below
where he stands.

When the target can't be reached on the map so far, the path goes to the
frontier: the reachable cell next to unknown ground that lies nearest the
target, outside the dead zones `dead_end` adds where Locke got stuck.
Walking there loads new sectors, so the next plan sees further: goal-biased
exploration, which finds stairs and doors out of view. With no frontier left
toward the target, the path goes to the nearest reachable cell.

Locke walks by a click on the farthest point of the path he has a straight,
open line to, so a straight run costs one click.
"""
from __future__ import annotations

import heapq
import math
from dataclasses import dataclass, field
from typing import Callable, Sequence

from walk import CELL, STEP_LIMIT, UNKNOWN, WalkGrid, cell_of

NEIGHBOURS = ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1))
MAX_EXPANSIONS = 120_000             # a search's bound: about a 350x350-cell region
DEAD_ZONE = 160                      # world units round a dead end the planner won't aim for
DEFAULT_RADIUS = 24
RADIUS_CUT = 0x6f                    # GetWalkHeightRadius: a radius whose cells reach 0x80 is cut to this

Cell = tuple[int, int]
Height = Callable[[Cell], int]       # a cell's floor height; UNKNOWN when unseen


def centre(c: Cell) -> tuple[int, int]:
    return c[0] * CELL, c[1] * CELL


def footprint(height: Height, x: int, y: int, radius: int) -> tuple[int, int, bool]:
    """GetWalkHeightRadius at (x, y): the floor under it, the largest step
    between side-by-side cells whose nearest point lies within `radius`,
    and whether any of those has no floor (or is unknown)."""
    under = max(0, height((cell_of(x), cell_of(y))))
    if radius <= 0 or radius >= 0xf0:
        return under, 0, False
    if ((radius + CELL - 1) & ~(CELL - 1)) >= 0x80:
        radius = RADIUS_CUT
    x0, y0 = (x - radius) & ~(CELL - 1), (y - radius) & ~(CELL - 1)
    x1, y1 = (x + radius + CELL) & ~(CELL - 1), (y + radius + CELL) & ~(CELL - 1)
    maxdelta, hole = 0, False
    rows: list[list[int | None]] = []
    for cy in range(y0, y1 + 1, CELL):
        row: list[int | None] = []
        for cx in range(x0, x1 + 1, CELL):
            dx = abs(min(max(x, cx - CELL // 2), cx + CELL // 2 - 1) - x)
            dy = abs(min(max(y, cy - CELL // 2), cy + CELL // 2 - 1) - y)
            if dx * dx + dy * dy > radius * radius:
                row.append(None)
                continue
            h = max(0, height((cell_of(cx), cell_of(cy))))
            hole |= h == 0
            if row and row[-1] is not None:
                maxdelta = max(maxdelta, abs(h - row[-1]))
            if rows and rows[-1][len(row)] is not None:
                maxdelta = max(maxdelta, abs(h - rows[-1][len(row)]))
            row.append(h)
        rows.append(row)
    return under, maxdelta, hole


class Walkable:
    """Whether Locke can stand at each cell centre, by the game's rule,
    cached per cell: (floor height, or None when he can't)."""

    def __init__(self, height: Height, radius: int):
        self.height, self.radius = height, radius
        self.cache: dict[Cell, int | None] = {}

    def stand(self, c: Cell) -> int | None:
        if c not in self.cache:
            under, maxdelta, hole = footprint(self.height, *centre(c), self.radius)
            self.cache[c] = None if (under == 0 or hole or maxdelta > STEP_LIMIT) else under
        return self.cache[c]


def search(height: Height, start: Sequence[float], target: Sequence[float], radius: int = DEFAULT_RADIUS,
           avoid: Callable[[Cell], bool] = lambda c: False) -> list[tuple[int, int]]:
    """World points (cell centres) from `start` toward `target`: to it when
    reachable; else to the frontier nearest it (a reachable cell beside
    unknown ground) that `avoid` doesn't rule out; else to the nearest
    reachable cell. [] when `start` has no floor."""
    walk = Walkable(height, radius)
    begin = (cell_of(start[0]), cell_of(start[1]))
    if max(0, height(begin)) == 0:
        return []
    goal = (cell_of(target[0]), cell_of(target[1]))
    tx, ty = target[0], target[1]

    def h(c):
        x, y = centre(c)
        return math.hypot(x - tx, y - ty)

    def frontier(c):
        return any(height((c[0] + dx, c[1] + dy)) == UNKNOWN for dx, dy in NEIGHBOURS[:4])

    heap = [(h(begin) / CELL, 0.0, begin)]
    came: dict[Cell, Cell | None] = {begin: None}
    cost = {begin: 0.0}
    nearest = edge = None
    expanded = 0
    while heap and expanded < MAX_EXPANSIONS:
        _, g, c = heapq.heappop(heap)
        if g > cost.get(c, math.inf):
            continue
        expanded += 1
        if c == goal:
            nearest, edge = c, None
            break
        if c != begin and not avoid(c):
            if nearest is None or h(c) < h(nearest):
                nearest = c
            if frontier(c) and (edge is None or h(c) < h(edge)):
                edge = c
        here = walk.stand(c) or max(0, height(c))
        for dx, dy in NEIGHBOURS:
            n = (c[0] + dx, c[1] + dy)
            there = walk.stand(n) if n != goal else max(0, height(n)) or None
            if there is None or abs(there - here) > STEP_LIMIT:
                continue
            step = g + (1.4142 if dx and dy else 1.0)
            if step < cost.get(n, math.inf):
                cost[n] = step
                came[n] = c
                heapq.heappush(heap, (step + h(n) / CELL, step, n))
    end = goal if goal in came and nearest == goal else (edge or nearest or begin)
    path = []
    c = end
    while c is not None:
        path.append(centre(c))
        c = came[c]
    return path[::-1]


def plan_path(grid: WalkGrid, start: Sequence[float], target: Sequence[float],
              radius: int = DEFAULT_RADIUS) -> list[tuple[int, int]]:
    """A path over one observed walk grid alone."""
    return search(lambda c: grid.cell_height(*c), start, target, radius)


@dataclass
class LevelMaps:
    """Walk heights seen so far, per level, by walk cell; and the dead zones
    found on each. A cell never seen is UNKNOWN."""
    heights: dict[int, dict[Cell, int]] = field(default_factory=dict)
    dead: dict[int, list[tuple[float, float]]] = field(default_factory=dict)

    def merge(self, level: int, grid: WalkGrid) -> None:
        cells = self.heights.setdefault(level, {})
        for y in range(grid.h):
            row = y * grid.w
            for x in range(grid.w):
                h = grid.heights[row + x]
                if h != UNKNOWN:
                    cells[(grid.ox + x, grid.oy + y)] = h

    def dead_end(self, level: int, pos: Sequence[float]) -> None:
        self.dead.setdefault(level, []).append((pos[0], pos[1]))

    def plan(self, level: int, start: Sequence[float], target: Sequence[float],
             radius: int = DEFAULT_RADIUS) -> list[tuple[int, int]]:
        cells = self.heights.get(level, {})
        dead = self.dead.get(level, [])

        def avoid(c):
            x, y = centre(c)
            return any(math.hypot(x - dx, y - dy) < DEAD_ZONE for dx, dy in dead)
        return search(lambda c: cells.get(c, UNKNOWN), start, target, radius, avoid)


def farthest_clear(grid: WalkGrid, start: Sequence[float], path: Sequence[tuple[int, int]],
                   on_screen=lambda p: True) -> tuple[int, int] | None:
    """The farthest point of `path` with a straight, open line from Locke
    that `on_screen` accepts (a click must land on the play field)."""
    for point in reversed(path[1:]):
        if grid.open_line(start, point) and on_screen(point):
            return point
    return None
