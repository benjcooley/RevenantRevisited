"""The world the director travels: where named objects stand, where exits lead.

    world = World(data_dir, slot_dir)        # the module's base map, or the slot's working set
    world.find('RandK')                      # [Place(level, pos, name, typename, objclass)]
    world.route(level, pos, goal)            # the exits to take, in order, to reach goal

Two sources, both the game's own data:

- the module's exit list (exit.def, docs/gameflow/forensics/EXITS.md §1.3):
  an exit's *instance name* to the point and level it takes the player to;
- the sector files: every object's name, type, class and position, by level
  (the slot's working set where it has one, else the base map; storytest.py
  decodes them, through revsave.py).

Routing assumes a level's ground is connected between any two points: the
cost of a leg is its straight-line distance. The walk itself finds the way
around walls (nav.py); a wrong guess shows up as a stalled leg, and the
director then sets that exit aside.
"""
from __future__ import annotations

import heapq
import math
import re
import sys
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'storytest'))
import storytest  # noqa: E402  (ModuleData, scan_level: the sector decoding)

EXIT_LINE = re.compile(r'^\s*(\S+)\s*\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\)\s*level\s+(\d+)', re.I)


@dataclass(frozen=True)
class Place:
    level: int
    pos: tuple[int, int, int]
    name: str                        # instance name, '' when it goes by its type's
    typename: str
    objclass: str


@dataclass(frozen=True)
class Exit:
    name: str                        # the instance name exit.def lists
    level: int
    pos: tuple[int, int, int]        # where the exit object stands
    to_level: int
    to_pos: tuple[int, int, int]     # where it takes the player


def parse_exit_list(text: str) -> dict[str, tuple[int, tuple[int, int, int]]]:
    """exit.def: name (lower case) -> (level, point)."""
    out = {}
    for raw in text.splitlines():
        m = EXIT_LINE.match(raw.split('//', 1)[0])
        if m:
            out[m.group(1).lower()] = (int(m.group(5)), (int(m.group(2)), int(m.group(3)), int(m.group(4))))
    return out


def flat(a, b) -> float:
    return math.hypot(a[0] - b[0], a[1] - b[1])


class World:
    def __init__(self, data: Path, slot: Path | None = None, module: str = 'Ahkuilon'):
        self.module = storytest.ModuleData(Path(data), module)
        self.slot = Path(slot) if slot else None
        self.exit_list = parse_exit_list(self.module.read('exit.def').decode(storytest.TEXT_ENCODING))
        self.levels = sorted({int(m.group(1)) for n in self.module.names
                              for m in [re.match(r'map/(\d+)_', n)] if m})

    @lru_cache(maxsize=None)
    def objects(self, level: int) -> tuple[Place, ...]:
        return tuple(Place(o.level, (o.x, o.y, o.z), o.name, o.typename, o.objclass)
                     for o in storytest.scan_level(self.module, level, self.slot))

    def find(self, name: str, levels: list[int] | None = None) -> list[Place]:
        """Objects answering to `name` (instance name, else type name; case
        ignored), on the given levels or all of them."""
        lowered = name.lower()
        return [p for level in (levels or self.levels) for p in self.objects(level)
                if (p.name or p.typename).lower() == lowered]

    @lru_cache(maxsize=None)
    def exits(self, level: int) -> tuple[Exit, ...]:
        """The level's exits that the exit list sends somewhere."""
        out = []
        for p in self.objects(level):
            target = self.exit_list.get(p.name.lower()) if p.name else None
            if target:
                out.append(Exit(p.name, level, p.pos, target[0], target[1]))
        return tuple(out)

    def route(self, level: int, pos, goal: Place, limit: int = 6) -> list[Exit] | None:
        """The exits to take, in order, from (level, pos) to `goal`, by the
        cheapest straight-line walking; [] to walk straight there; None when
        no chain of at most `limit` exits reaches the goal's level."""
        start = (level, tuple(pos))
        frontier: list[tuple[float, int, tuple, list[Exit]]] = [(0.0, 0, start, [])]
        best: dict[tuple, float] = {start: 0.0}
        counter = 0
        done = None
        while frontier:
            cost, _, (lv, at), taken = heapq.heappop(frontier)
            if done and cost >= done[0]:
                break
            if lv == goal.level:
                total = cost + flat(at, goal.pos)
                if not done or total < done[0]:
                    done = (total, taken)
            if len(taken) >= limit:
                continue
            for e in self.exits(lv):
                if e in taken:
                    continue
                arrive = (e.to_level, e.to_pos)
                step = cost + flat(at, e.pos)
                if step < best.get(arrive, math.inf):
                    best[arrive] = step
                    counter += 1
                    heapq.heappush(frontier, (step, counter, arrive, taken + [e]))
        return done[1] if done else None
