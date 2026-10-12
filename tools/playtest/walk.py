"""Walking: where to go on screen, click to walk, arrival and stalls.

Both games walk Locke to the map point under a left click on open ground,
and to an object (then use or take it) under a left click on the object
(docs/playtest/PLAN.md §10). So walking is choosing a screen point:

- objects in view (doors and exits, containers, items) are destinations as
  they are: a click walks to one and uses or takes it;
- a click on a person talks to them where they stand and never walks
  (PlayMouseRelease: characters aren't walked to), so a person farther than
  use reach is approached by the open ground beside them, projected to the
  screen by the host, and one within reach is a 'talk' destination;
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
from typing import Callable, Sequence

from inputs import Bindings, Plan
from jev import Choice, Jev

STEP_LIMIT = 32                      # TCharacter::Blocked's MAXZMOVE: higher steps block
CELL = 16                            # walk grid cell, world units; cell k is centred on k * CELL
CLICK_HEIGHT = 50                    # a click's map point is taken at Locke's height + 50 (MAP_INPUT.md §5)
UNKNOWN = -1                         # a walk grid cell no loaded sector holds
EXPLORED_CELL = 256                  # memory cell for "been there", world units
SNAP = 8                             # Goto snaps onto its point within this
ARRIVE = 24                          # stopped this close to a ground point is arrived
USE_REACH = 0x60                     # a clicked object is used within this game distance (0x0044fd74)
NUDGES = ((0, 0), (0, -8), (8, 0), (0, 8), (-8, 0), (8, -8), (-8, 8))   # pixels to try round a covered spot
BESIDE = 60                          # how far from a person to stand when walking up to them
SHUNNED = 6                          # choices a destination sits out after a refused or stalled walk
COMPASS = ('right', 'up and right', 'up', 'up and left', 'left', 'down and left', 'down', 'down and right')
DESTINATION_CLASSES = {'character', 'exit', 'container', 'item', 'weapon', 'armor', 'food', 'potion',
                       'money', 'scroll', 'talisman', 'key', 'tool', 'lightsource'}


def game_distance(a: Sequence[float], b: Sequence[float]) -> int:
    """The flat distance as the game measures it (::Distance,
    src/object.cpp; retail's DistTable): sqrt rounded, both components
    halved together until they fit the 256x256 table."""
    x, y, shifts = abs(int(b[0]) - int(a[0])), abs(int(b[1]) - int(a[1])), 0
    while x > 255 or y > 255:
        x, y, shifts = x >> 1, y >> 1, shifts + 1
    return min(255, math.floor(math.sqrt(x * x + y * y) + 0.5)) << shifts


def cell_of(x: float) -> int:
    """The walk cell holding world coordinate x: GetWalkHeight's
    (x + GRIDSIZE / 2) >> WALKMAPSHIFT, so cell k spans k*16-8 .. k*16+7."""
    return (int(math.floor(x)) + CELL // 2) >> 4


def screen_direction(dx: float, dy: float) -> str:
    """A screen vector (y down) as one of eight words."""
    sector = round(math.degrees(math.atan2(-dy, dx)) / 45) % 8
    return COMPASS[sector]


@dataclass(frozen=True)
class Destination:
    key: str                         # the Jev option name: 'obj:1712', 'near:1712', 'talk:1712', 'ground:3:far'
    words: str                       # what it is, for Jev
    screen: tuple[int, int]
    world: tuple[int, int, int] | None
    object_id: int | None = None
    use: bool = False                # a click on an object in reach: use it (talk to a person), no walk
    name: str = ''                   # the object's name, for objects
    person: bool = False


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
    """The observation's walk grid: heights by cell, 0 where there is no
    floor, UNKNOWN where no loaded sector holds the cell."""

    def __init__(self, grid: dict):
        self.ox, self.oy = grid['origin']
        self.cell = grid.get('cell', CELL)
        self.w, self.h = grid['w'], grid['h']
        self.heights = grid['heights']

    def cell_height(self, gx: int, gy: int) -> int:
        x, y = gx - self.ox, gy - self.oy
        if 0 <= x < self.w and 0 <= y < self.h:
            return self.heights[y * self.w + x]
        return UNKNOWN

    def height(self, x: float, y: float) -> int:
        """The floor height under a world point; 0 for none or unknown."""
        return max(0, self.cell_height(cell_of(x), cell_of(y)))

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


def playfield_clamp(point: Sequence[float], rect: Sequence[int]) -> tuple[int, int]:
    return (min(max(int(point[0]), rect[0]), rect[2] - 1), min(max(int(point[1]), rect[1]), rect[3] - 1))


@dataclass
class Walker:
    jev: Jev
    bindings: Bindings
    memory: Memory = field(default_factory=Memory)
    recent: list[str] = field(default_factory=list)          # destination keys reached lately, newest last
    shunned: dict[str, int] = field(default_factory=dict)    # key -> choices left to sit out
    rings: tuple[int, ...] = (80, 160)
    playfield: tuple[int, int, int, int] = (0, 0, 640, 480)
    under: Callable[[int, int], int | None] | None = None   # the object a click at a pixel acts on
    run: bool = False                                        # hold the run key on a walk (CLICKWALK.md)

    def destinations(self, host, obs: dict, grid: WalkGrid | None) -> list[Destination]:
        player = obs['player']
        here = player.get('screen') or [320, 240]
        level = player.get('level', 0)
        field_rect = obs.get('playfield', self.playfield)
        found: list[Destination] = []
        self.shunned = {k: n - 1 for k, n in self.shunned.items() if n > 1}
        for o in obs.get('objects', []):
            screen = o.get('screen')
            if (o.get('class') not in DESTINATION_CLASSES or not screen or not in_rect(screen, field_rect)
                    or o.get('enemy')):
                continue
            name = o.get('name') or o.get('type')
            gap = game_distance(o['pos'], player['pos'])
            way = screen_direction(screen[0] - here[0], screen[1] - here[1])
            if o['class'] == 'character':
                if gap <= USE_REACH:
                    key = f"talk:{o['id']}"
                    if key not in self.recent:
                        found.append(Destination(key, f"talk to '{name}', beside Locke", tuple(screen), None,
                                                 o['id'], use=True, name=name, person=True))
                    continue
                key = f"near:{o['id']}"
                f = BESIDE / gap
                spot = (round(o['pos'][0] + (player['pos'][0] - o['pos'][0]) * f),
                        round(o['pos'][1] + (player['pos'][1] - o['pos'][1]) * f), o['pos'][2])
                shown, reaches = host.project([(spot[0], spot[1], player['pos'][2] + CLICK_HEIGHT)])[0]
                if (key in self.shunned or not shown or not reaches or not in_rect(shown, field_rect)
                        or (grid and not grid.open_line(player['pos'], spot))):
                    continue
                found.append(Destination(key, f"walk up to '{name}', {way}", tuple(shown), spot, name=name))
                continue
            if gap <= USE_REACH and o['class'] not in ('item',) and f"use:{o['id']}" not in self.recent:
                # in reach: a click uses it (an exit takes Locke through, a chest opens)
                found.append(Destination(f"use:{o['id']}", f"use {o['class']} '{name}', beside Locke",
                                         tuple(screen), None, o['id'], use=True, name=name))
                continue
            key = f"obj:{o['id']}"
            if key in self.shunned or (gap <= USE_REACH and key in self.recent):
                continue                                         # just reached, and still here
            lately = ', went there just now' if key in self.recent else ''
            found.append(Destination(key, f"{o['class']} '{name}', {way}{lately}",
                                     tuple(screen), tuple(o['pos']), o['id'], name=name))
        for sector, word in enumerate(COMPASS):
            angle = math.radians(45 * sector)
            for ring, label in zip(self.rings, ('near', 'far')):
                point = (round(here[0] + ring * math.cos(angle)), round(here[1] - ring * math.sin(angle)))
                if not in_rect(point, field_rect):
                    continue
                picked = host.pick(*point)
                pos = picked.get('pos')
                if picked.get('object') is not None or not pos or not picked.get('world', True):
                    continue                                     # an object, nothing, or the HUD there
                if grid and not grid.open_line(player['pos'], pos):
                    continue
                key = f'ground:{sector}:{label}'
                if key in self.shunned:
                    continue
                been = 'visited' if self.memory.seen(level, pos) else 'unexplored'
                found.append(Destination(key, f'open ground {word}, {label}, {been}', point, tuple(pos)))
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

    def uncovered(self, screen: Sequence[int]) -> tuple[int, int] | None:
        """A pixel at or a few from `screen` where a click is a walk, not
        a use: no object under it (a ground click beside an exit would take
        Locke through it). Unchecked without `under`."""
        if not self.under:
            return tuple(screen)
        x0, y0 = playfield_clamp(screen, self.playfield)
        for dx, dy in NUDGES:
            x, y = playfield_clamp((x0 + dx, y0 + dy), self.playfield)
            if self.under(x, y) is None:
                return x, y
        return None

    def walk(self, host, state, dest: Destination, run: bool | None = None, limit: int = 24 * 15,
             chunk: int = 6, observe=None) -> tuple[str, dict, object]:
        """Click `dest` and step until Locke arrives ('arrived'), stops short
        ('stalled'), never sets off ('refused'), or `limit` ticks pass
        ('timeout'). Returns the result, the last observation and the input
        state to carry on with. An object counts as reached within the
        reach the game uses it from (MAP_INPUT.md §4: 96). `observe`, when
        given, is called in place of the host's (the director's logs)."""
        observe = observe or host.observe
        run = self.run if run is None else run
        if not dest.use and dest.object_id is None:
            spot = self.uncovered(dest.screen)
            if spot is None:                                 # every pixel there is on some object
                self.shunned[dest.key] = SHUNNED
                return 'refused', observe(), state
            dest = Destination(dest.key, dest.words, spot, dest.world, name=dest.name)
        if dest.use:
            plan = Plan(state)
            end = plan.click(0, *dest.screen)
            host.step(plan.ticks(end + 12), plan.events())
            self.recent = [k for k in self.recent if k != dest.key][-4:] + [dest.key]
            return ('talked' if dest.person else 'used'), observe(), plan.state
        arrive = USE_REACH if dest.object_id is not None else ARRIVE
        plan = Plan(state)
        if run:
            plan.hold(0, self.bindings['Run'])
        end = plan.click(0, *dest.screen)
        host.step(plan.ticks(end), plan.events())
        state = plan.state
        obs = observe()
        start = last = obs['player']['pos']
        still, ran = 0, end
        result = 'timeout'
        while ran < limit:
            host.step(chunk)
            ran += chunk
            obs = observe()
            pos = obs['player']['pos']
            self.memory.note(obs)
            moving = math.hypot(pos[0] - last[0], pos[1] - last[1]) >= 2
            last = pos
            if dest.world:
                gap = (game_distance(pos, dest.world) if dest.object_id is not None
                       else math.hypot(pos[0] - dest.world[0], pos[1] - dest.world[1]))
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
        if result == 'arrived':
            self.recent = [k for k in self.recent if k != dest.key][-4:] + [dest.key]
        elif result in ('refused', 'stalled'):
            self.shunned[dest.key] = SHUNNED
        return result, obs, state
