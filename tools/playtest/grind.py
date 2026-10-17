"""Where to grind: hunting sites from the game's own data, and which to visit.

    grounds = Grounds.survey(world, kb, level)          # the level's sites
    site = grounds.next(locke_pos, locke_level, style, tick)
    grounds.visited(site, tick)

A site is a knot of hostile monsters on a level: the placed ones (each lives
once, GRIND.md §3.1) and the MonsterGen helpers (each slot restocks one
monster every 70-91 s while the generator is off screen and under its cap).
Hostile means an unnamed character whose class.def type has Value 1 or more
and attacks on sight by default: named characters are the story's people,
bosses and dummies (GRIND.md §3.4), and townsfolk have Value -1.

A site is scored for Locke as GRIND.md §9 step 2 says: experience wants
monsters with Value at least his level (a kill below Level - 3 is worth 1),
survival wants Value no more than Level + 1 (each point of Value is 12
to-hit points both ways, each level of his 10), so a site with stronger
monsters loses worth for each. Barehanded, a monster's
`hand` resistance cuts what his fists do (DAMAGEMODS, §2.4). Visiting turns
over the sites: a site left for REVISIT seconds has had its generators'
clocks running, so it is worth a return.
"""
from __future__ import annotations

import math
from dataclasses import dataclass, field

from knowledge import Knowledge
from world import Generator, Place, World

SITE_RADIUS = 600                    # monsters within this of a site's first one join it
REVISIT = 24 * 120                   # ticks before a site is worth another visit: its generators' clocks
DANGER = 2.0                         # what one monster above Locke's class takes off a site's worth
STYLES = ('hands', 'weapon', 'mixed')


@dataclass(frozen=True)
class Site:
    level: int
    centre: tuple[int, int, int]
    monsters: tuple[tuple[str, int], ...]    # (type, how many): placed ones, plus a generator slot's cap
    generators: int

    @property
    def count(self) -> int:
        return sum(n for _, n in self.monsters)

    def words(self) -> str:
        kinds = ', '.join(f'{n} {t}' for t, n in self.monsters)
        return f"{kinds}{f', {self.generators} generators' if self.generators else ''}"


def hostile(kb: Knowledge, place: Place) -> bool:
    if place.objclass.upper() != 'CHARACTER' or place.name:
        return False
    kind = kb.char_type(place.typename)
    return bool(kind and kind.value >= 1 and kind.aggressive)


def cluster(points: list[tuple[tuple[int, int, int], str, int, bool]]) -> list[list]:
    """Greedy grouping: each point joins the first group whose first point
    is within SITE_RADIUS, else starts one."""
    groups: list[list] = []
    for point in points:
        for group in groups:
            if math.dist(group[0][0][:2], point[0][:2]) <= SITE_RADIUS:
                group.append(point)
                break
        else:
            groups.append([point])
    return groups


@dataclass
class Grounds:
    sites: list[Site]
    kb: Knowledge
    last_visit: dict[tuple, int] = field(default_factory=dict)   # site centre -> tick left

    @classmethod
    def survey(cls, world: World, kb: Knowledge, level: int) -> Grounds:
        points = [(p.pos, p.typename.strip(), 1, False) for p in world.objects(level) if hostile(kb, p)]
        for g in world.generators(level):
            for kind, _ in g.slots:
                if kb.char_type(kind):                      # a few slots name no type and never spawn
                    points.append((g.place.pos, kb.char_type(kind).name, max(1, g.cap), True))
        sites = []
        for group in cluster(points):
            counts: dict[str, int] = {}
            for _, kind, n, _ in group:
                counts[kind] = counts.get(kind, 0) + n
            xs, ys, zs = zip(*(p for p, *_ in group))
            centre = (round(sum(xs) / len(xs)), round(sum(ys) / len(ys)), round(sum(zs) / len(zs)))
            generators = len({p for p, _, _, gen in group if gen})
            sites.append(Site(level, centre, tuple(sorted(counts.items(), key=lambda kv: -kv[1])), generators))
        return cls(sites, kb)

    def fit(self, kind: str, locke_level: int, style: str) -> float:
        """How good one monster of `kind` is to grind on, 0 for one to
        leave alone: its Exp for Locke against how hard it hits back."""
        t = self.kb.char_type(kind)
        if not t or t.value > locke_level + 1:
            return 0.0                                          # out of his class: it hits, he glances
        worth = 1.0 if t.value >= locke_level else 0.5 if t.value >= locke_level - 3 else 0.05
        if style == 'hands':
            character = self.kb.character(kind)
            hand = character.damagemods.get('hand', 0) if character else 0
            worth *= (100 - hand) / 100                         # fists do (100 - hand)%
        return worth

    def score(self, site: Site, locke_level: int, style: str) -> float:
        """The site's worth: its monsters' fit, less a penalty for each one
        out of his class, which will come for him all the same."""
        too_strong = sum(n for kind, n in site.monsters
                         if (t := self.kb.char_type(kind)) and t.value > locke_level + 1)
        return sum(self.fit(kind, locke_level, style) * n for kind, n in site.monsters) - DANGER * too_strong

    def next(self, pos, locke_level: int, style: str, tick: int) -> Site | None:
        """The site to hunt next: the best scoring of those not visited in
        the last REVISIT ticks, nearer ones preferred among the good."""
        ready = [s for s in self.sites if tick - self.last_visit.get(s.centre, -REVISIT) >= REVISIT]
        worth = [(self.score(s, locke_level, style), s) for s in ready]
        worth = [(w, s) for w, s in worth if w > 0]
        if not worth:
            return None
        best = max(w for w, _ in worth)
        good = [s for w, s in worth if w >= best / 2]
        return min(good, key=lambda s: math.dist(s.centre[:2], pos[:2]))

    def visited(self, site: Site, tick: int) -> None:
        self.last_visit[site.centre] = tick
