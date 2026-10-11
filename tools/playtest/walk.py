"""Walking: where to go on screen, click to walk, arrival and stalls.

Both games walk Locke to the map point under a left click on open ground,
and to an object (then use or take it) under a left click on the object
(docs/playtest/PLAN.md §10). So walking is choosing a screen point:

- objects in view (people, doors and exits, containers, items) are
  destinations as they are;
- open ground is sampled in rings around Locke on screen, mapped to the
  world by the host's `pick` (the game's own pointer conversion), and kept
  when the walk grid says the way there is open by the game's rule
  (TCharacter::Blocked: no floor, or a step of more than 32 units, blocks);
- each is labelled with what lies that way and whether Locke has been
  there, and Jev picks one for the objective.

`walk` clicks and steps until Locke arrives, stops moving, or runs out of
time. Distances, directions and the walk grid stay in code.
"""
from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Sequence

from inputs import Bindings, Plan
from jev import Choice, Jev

STEP_LIMIT = 32                      # TCharacter::Blocked's MAXZMOVE: higher steps block
CELL = 16                            # walk grid cell, world units
EXPLORED_CELL = 256                  # memory cell for "been there", world units
SNAP = 8                             # Goto snaps onto its point within this
ARRIVE = 24                          # stopped this close to a ground point is arrived
USE_REACH = 100                      # a clicked object is used from within 96
COMPASS = ('right', 'up and right', 'up', 'up and left', 'left', 'down and left', 'down', 'down and right')
DESTINATION_CLASSES = {'character', 'exit', 'container', 'item', 'weapon', 'armor', 'food', 'potion',
                       'money', 'scroll', 'talisman', 'key', 'tool', 'lightsource'}


def screen_direction(dx: float, dy: float) -> str:
    """A screen vector (y down) as one of eight words."""
    sector = round(math.degrees(math.atan2(-dy, dx)) / 45) % 8
    return COMPASS[sector]


@dataclass(frozen=True)
class Destination:
    key: str                         # the Jev option name: 'obj:1712', 'ground:3:far'
    words: str                       # what it is, for Jev
    screen: tuple[int, int]
    world: tuple[int, int, int] | None
    object_id: int | None = None


@dataclass
class Memory:
    """Where Locke has been, per level, in coarse cells."""
    visited: dict[int, set[tuple[int, int]]] = field(default_factory=dict)

    def note(self, obs: dict) -> None:
        player = obs['player']
        self.visited.setdefault(player.get('level', 0), set()).add(self._cell(player['pos']))

    def seen(self, level: int, pos: Sequence[int]) -> bool:
        return self._cell(pos) in self.visited.get(level, set())

    @staticmethod
    def _cell(pos: Sequence[int]) -> tuple[int, int]:
        return (int(pos[0]) // EXPLORED_CELL, int(pos[1]) // EXPLORED_CELL)


class WalkGrid:
    """The observation's walk grid: heights by cell, 0 where there is no floor."""

    def __init__(self, grid: dict):
        self.ox, self.oy = grid['origin']
        self.cell = grid.get('cell', CELL)
        self.w, self.h = grid['w'], grid['h']
        self.heights = grid['heights']

    def height(self, x: float, y: float) -> int:
        gx, gy = int(x // self.cell) - self.ox, int(y // self.cell) - self.oy
        if 0 <= gx < self.w and 0 <= gy < self.h:
            return self.heights[gy * self.w + gx]
        return 0

    def open_line(self, a: Sequence[int], b: Sequence[int]) -> bool:
        """Every cell on the straight line from a to b has floor, and no step
        between neighbours exceeds STEP_LIMIT."""
        steps = max(1, int(math.hypot(b[0] - a[0], b[1] - a[1]) // (self.cell / 2)))
        previous = self.height(a[0], a[1])
        if previous == 0:
            return False
        for i in range(1, steps + 1):
            x = a[0] + (b[0] - a[0]) * i / steps
            y = a[1] + (b[1] - a[1]) * i / steps
            h = self.height(x, y)
            if h == 0 or abs(h - previous) > STEP_LIMIT:
                return False
            previous = h
        return True


def in_rect(point: Sequence[float], rect: Sequence[int]) -> bool:
    return rect[0] <= point[0] < rect[2] and rect[1] <= point[1] < rect[3]


@dataclass
class Walker:
    jev: Jev
    bindings: Bindings
    memory: Memory = field(default_factory=Memory)
    rings: tuple[int, ...] = (80, 160)
    playfield: tuple[int, int, int, int] = (0, 0, 640, 480)

    def destinations(self, host, obs: dict, grid: WalkGrid | None) -> list[Destination]:
        player = obs['player']
        here = player.get('screen') or [320, 240]
        level = player.get('level', 0)
        field_rect = obs.get('playfield', self.playfield)
        found: list[Destination] = []
        for o in obs.get('objects', []):
            screen = o.get('screen')
            if (o.get('class') not in DESTINATION_CLASSES or not screen or not in_rect(screen, field_rect)
                    or o.get('enemy')):
                continue
            way = screen_direction(screen[0] - here[0], screen[1] - here[1])
            kind = 'person' if o['class'] == 'character' else o['class']
            found.append(Destination(f"obj:{o['id']}", f"{kind} '{o.get('name') or o.get('type')}', {way}",
                                     tuple(screen), tuple(o['pos']), o['id']))
        for sector, word in enumerate(COMPASS):
            angle = math.radians(45 * sector)
            for ring, label in zip(self.rings, ('near', 'far')):
                point = (round(here[0] + ring * math.cos(angle)), round(here[1] - ring * math.sin(angle)))
                if not in_rect(point, field_rect):
                    continue
                picked = host.pick(*point)
                pos = picked.get('pos')
                if picked.get('object') is not None or not pos:
                    continue
                if grid and not grid.open_line(player['pos'], pos):
                    continue
                been = 'visited' if self.memory.seen(level, pos) else 'unexplored'
                found.append(Destination(f'ground:{sector}:{label}', f'open ground {word}, {label}, {been}',
                                         point, tuple(pos)))
        return found

    def choose(self, objective: str, obs: dict, candidates: Sequence[Destination]) -> Destination:
        if len(candidates) == 1:
            return candidates[0]
        state = {'objective': objective,
                 'locke': {'level': obs['player'].get('level'), 'health': obs['player'].get('health')},
                 'recent_messages': obs.get('messages', [])[-5:]}
        answer = self.jev.ask(state, tag='walk', where=Choice(
            'Where should Locke walk next to make progress on `objective`? Prefer people and places the '
            'objective names; otherwise prefer unexplored ground.',
            {c.key: c.words for c in candidates}))['where']
        return next(c for c in candidates if c.key == answer.choice)

    def walk(self, host, state, dest: Destination, run: bool = False, limit: int = 24 * 15,
             chunk: int = 6) -> tuple[str, dict, object]:
        """Click `dest` and step until Locke arrives ('arrived'), stops short
        ('stalled'), never sets off ('refused'), or `limit` ticks pass
        ('timeout'). Returns the result, the last observation and the input
        state to carry on with. An object counts as reached within the
        reach the game uses it from (MAP_INPUT.md §4: 96)."""
        arrive = USE_REACH if dest.object_id is not None else ARRIVE
        plan = Plan(state)
        if run:
            plan.hold(0, self.bindings['Run'])
        end = plan.click(0, *dest.screen)
        host.step(plan.ticks(end), plan.events())
        state = plan.state
        obs = host.observe()
        start = last = obs['player']['pos']
        still, ran = 0, end
        result = 'timeout'
        while ran < limit:
            host.step(chunk)
            ran += chunk
            obs = host.observe()
            pos = obs['player']['pos']
            self.memory.note(obs)
            moving = math.hypot(pos[0] - last[0], pos[1] - last[1]) >= 2
            last = pos
            if dest.world:
                gap = math.hypot(pos[0] - dest.world[0], pos[1] - dest.world[1])
                if gap <= SNAP or (gap <= arrive and not moving):
                    result = 'arrived'
                    break
            still = 0 if moving else still + chunk
            if still >= 2 * chunk:
                set_off = math.hypot(pos[0] - start[0], pos[1] - start[1]) >= 2
                result = 'stalled' if set_off else 'refused'
                break
        if run:
            plan = Plan(state)
            plan.release(0, self.bindings['Run'])
            host.step(1, plan.events())
            state = plan.state
        return result, obs, state
