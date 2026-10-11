"""Walking toward a point beyond the next few steps.

    maps = LevelMaps()
    maps.merge(level, walkgrid)                      # every walk grid observed
    path = maps.plan(level, locke_pos, target_pos)   # world points, cell centres
    point = farthest_clear(grid, locke_pos, path)    # the next click

The host's walk grid covers only the sectors loaded round Locke, so each one
observed is merged into a per-level height map that grows as he goes, and
paths are planned over all of it. A* by the game's blocking rule
(TCharacter::Blocked: no floor, or a step of more than 32 units, blocks;
docs/MOVEMENT.md), eight neighbours without cutting corners. The game tests
a step over Locke's radius, not a point, so a path keeps a cell of
clearance: a cell next to a wall or a ledge is not walked through (the start
and the target excepted: Locke may stand there, an exit sits in its
doorway).

When the target can't be reached on the map seen so far, the path goes to
the reachable cell nearest it, outside the dead zones: places where Locke
got stuck, which `dead_end` adds. A dead end therefore turns the search
elsewhere, and walking elsewhere loads new ground into the map (stairs, the
other side of a wall): goal-biased exploration rather than retrying a corner.

Locke walks by a click on the farthest point of the path he has a straight,
open line to, so a straight run costs one click.
"""
from __future__ import annotations

import heapq
import math
from dataclasses import dataclass, field
from typing import Callable, Sequence

from walk import CELL, STEP_LIMIT, WalkGrid

NEIGHBOURS = ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1))
MAX_EXPANSIONS = 120_000             # a search's bound: about a 350x350-cell region
DEAD_ZONE = 160                      # world units round a dead end the planner won't aim for

Height = Callable[[tuple[int, int]], int]


def _clear(height: Height, c: tuple[int, int]) -> bool:
    """Floor all round, within a step of this cell's height."""
    h = height(c)
    return h != 0 and all(height((c[0] + dx, c[1] + dy)) != 0
                          and abs(height((c[0] + dx, c[1] + dy)) - h) <= STEP_LIMIT for dx, dy in NEIGHBOURS)


def _can_step(height: Height, a: tuple[int, int], b: tuple[int, int], goal: tuple[int, int]) -> bool:
    ha, hb = height(a), height(b)
    if hb == 0 or abs(hb - ha) > STEP_LIMIT:
        return False
    if b != goal and not _clear(height, b):
        return False
    if a[0] != b[0] and a[1] != b[1]:                    # a diagonal: both sides open too
        for side in ((b[0], a[1]), (a[0], b[1])):
            hs = height(side)
            if hs == 0 or abs(hs - ha) > STEP_LIMIT:
                return False
    return True


def _centre(c: tuple[int, int], cell: int = CELL) -> tuple[int, int]:
    return c[0] * cell + cell // 2, c[1] * cell + cell // 2


def search(height: Height, start: Sequence[float], target: Sequence[float], cell: int = CELL,
           avoid: Callable[[tuple[int, int]], bool] = lambda c: False) -> list[tuple[int, int]]:
    """World points (cell centres) from `start` toward `target`: to it when
    reachable, else to the reachable cell nearest it that `avoid` doesn't
    rule out. [] when `start` has no floor."""
    begin = (int(start[0] // cell), int(start[1] // cell))
    if height(begin) == 0:
        return []
    goal = (int(target[0] // cell), int(target[1] // cell))
    tx, ty = target[0], target[1]

    def h(c):
        x, y = _centre(c, cell)
        return math.hypot(x - tx, y - ty)

    frontier = [(h(begin) / cell, 0.0, begin)]
    came: dict[tuple[int, int], tuple[int, int] | None] = {begin: None}
    cost = {begin: 0.0}
    nearest, expanded = begin, 0
    while frontier and expanded < MAX_EXPANSIONS:
        _, g, c = heapq.heappop(frontier)
        if g > cost.get(c, math.inf):
            continue
        expanded += 1
        if c == goal:
            nearest = c
            break
        if not avoid(c) and (avoid(nearest) or h(c) < h(nearest)):
            nearest = c
        for dx, dy in NEIGHBOURS:
            n = (c[0] + dx, c[1] + dy)
            if not _can_step(height, c, n, goal):
                continue
            step = g + (1.4142 if dx and dy else 1.0)
            if step < cost.get(n, math.inf):
                cost[n] = step
                came[n] = c
                heapq.heappush(frontier, (step + h(n) / cell, step, n))
    path = []
    c = nearest
    while c is not None:
        path.append(_centre(c, cell))
        c = came[c]
    return path[::-1]


def plan_path(grid: WalkGrid, start: Sequence[float], target: Sequence[float]) -> list[tuple[int, int]]:
    """A path over one observed walk grid alone."""
    def height(c):
        x, y = c[0] - grid.ox, c[1] - grid.oy
        return grid.heights[y * grid.w + x] if 0 <= x < grid.w and 0 <= y < grid.h else 0
    return search(height, start, target, grid.cell)


@dataclass
class LevelMaps:
    """Walk heights seen so far, per level, by absolute walk cell; and the
    dead zones found on each."""
    heights: dict[int, dict[tuple[int, int], int]] = field(default_factory=dict)
    dead: dict[int, list[tuple[float, float]]] = field(default_factory=dict)

    def merge(self, level: int, grid: WalkGrid) -> None:
        """A grid's cells into the level's map. A 0 only ever means 'no
        floor here as far as this grid knows' (an unloaded sector reads 0
        too), so it never erases floor seen before."""
        cells = self.heights.setdefault(level, {})
        for y in range(grid.h):
            row = y * grid.w
            for x in range(grid.w):
                h = grid.heights[row + x]
                key = (grid.ox + x, grid.oy + y)
                if h:
                    cells[key] = h
                elif key not in cells:
                    cells[key] = 0

    def dead_end(self, level: int, pos: Sequence[float]) -> None:
        self.dead.setdefault(level, []).append((pos[0], pos[1]))

    def plan(self, level: int, start: Sequence[float], target: Sequence[float]) -> list[tuple[int, int]]:
        cells = self.heights.get(level, {})
        dead = self.dead.get(level, [])

        def avoid(c):
            x, y = _centre(c)
            return any(math.hypot(x - dx, y - dy) < DEAD_ZONE for dx, dy in dead)
        return search(lambda c: cells.get(c, 0), start, target, avoid=avoid)


def farthest_clear(grid: WalkGrid, start: Sequence[float], path: Sequence[tuple[int, int]],
                   on_screen=lambda p: True) -> tuple[int, int] | None:
    """The farthest point of `path` with a straight, open line from Locke
    that `on_screen` accepts (a click must land on the play field)."""
    for point in reversed(path[1:]):
        if grid.open_line(start, point) and on_screen(point):
            return point
    return None
