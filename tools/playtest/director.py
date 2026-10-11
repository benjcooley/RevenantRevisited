"""The play loop: modes owned by code, narrow questions to Jev.

    director = Director(host, jev, router, kb, bindings, bundle, milestones,
                        waypoints=Catalogue(), story='keep', start=0, parent='newgame')
    director.run(max_ticks=24 * 60 * 20)

Each turn observes the game and picks a mode from the observation
(docs/playtest/PLAN.md §9):

- wait      loading, a scene, or control off: step time on
- dialog    choices are up: Jev picks one for the objective; a digit key
- combat    an enemy is on Locke: the tactician (combat.py)
- explore   control is Locke's: walk somewhere useful (walk.py), use what
            is there
- dead      the death screen: end the session (or reload, later)
- unknown   a screen with no state: the router looks at the frame

The milestone list (a story) gives the objective; a milestone is done when
its check holds, and a milestone marked for saving quick-saves and adds the
slot to the waypoint catalogue (waypoints.py) as the next point sessions
can play from. Everything the director does goes to the host as input, so
the session's recording replays it.

The bundle keeps the whole session for finding bugs (timeline.py renders
it): every observation (observations.jsonl), every decision and what it was
based on (director.jsonl, and Jev's states, questions and answers in
session.rvlog), every input and check line (session.rvrec), frames every
two seconds of game time and at every notable moment (frames/), and the
host's own logs (logs/, at the end).
"""
from __future__ import annotations

import json
import math
import shutil
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable

from combat import DIRECTIONS, Tactician, distance
from host import Host, HostError
from inputs import Bindings, InputState, Plan, chord
from rec import BUTTON_RIGHT
from jev import Choice, Jev, Router, tool
from knowledge import Knowledge
from nav import DEFAULT_RADIUS, LevelMaps, farthest_clear
from walk import CLICK_HEIGHT, USE_REACH, Destination, Walker, WalkGrid, in_rect
from waypoints import Catalogue, Waypoint, story_states, wrong_states
from world import Place, World

ENGAGE_RADIUS = 500                  # a living enemy this close means a fight
WALKGRID_RADIUS = 480
WIDE_WALKGRID_RADIUS = 2400          # once per stop, when the way to it isn't in the map yet
DIGITS = '123456'
# Each direction straight after its opposite, so calibrating ends where it began.
CALIBRATION_ORDER = tuple(DIRECTIONS[i + offset] for i in range(4) for offset in (0, 4))


@dataclass
class Milestone:
    name: str
    objective: str                   # what Jev is told to make progress on
    done: Callable[[dict, 'Director'], bool]
    save: bool = False
    goal: str | None = None          # an object to travel to (instance name): code finds the way
    expect: dict[str, int] = field(default_factory=dict)   # story variables its save must hold


@dataclass
class Director:
    host: Host
    jev: Jev
    router: Router
    kb: Knowledge
    bindings: Bindings
    bundle: Path
    milestones: list[Milestone]
    waypoints: Catalogue | None = None
    story: str = ''                  # the milestone list's name, for waypoints
    start: int = 0                   # milestones already done where the session starts
    parent: str = 'newgame'          # the waypoint the session starts from
    game: str = 'port'
    frame_every: int = 48            # ticks between kept frames
    world: World | None = None       # where things are and where exits lead (world.py)
    watch: Path | None = None        # where a watcher's page reads state.json (watch.py)
    state: InputState = field(default_factory=InputState)
    log: list[dict] = field(default_factory=list)
    messages: list[str] = field(default_factory=list)
    choices_taken: dict[str, set[str]] = field(default_factory=dict)
    talked: set[str] = field(default_factory=set)              # people Locke has talked to (names)
    equip_tried: set[int] = field(default_factory=set)         # pack slots right-clicked to equip
    exit_tries: dict[str, int] = field(default_factory=dict)   # uses of an exit that left Locke where he was
    blocked_exits: set[str] = field(default_factory=set)       # exits that wouldn't take him: explore instead
    stuck: int = 0                                             # stalls in a row at one spot while travelling
    stuck_at: list | None = None
    dead_ends: int = 0                                         # dead ends since Locke last got nearer
    wide_looks: set = field(default_factory=set)               # (level, stop) a wide walk grid was fetched for
    no_walkgrid: bool = False                                  # the host offers no walk grid (an older emulator host)
    best_gap: float = math.inf                                 # the nearest he has been to the next stop

    def __post_init__(self) -> None:
        self.walker = Walker(self.jev, self.bindings)
        self.maps = LevelMaps()                                 # walk heights seen so far, per level
        self.tactician = Tactician(self.jev, self.kb, self.bindings, messages=self.messages)
        self.current = self.start
        self.created: list[tuple[Waypoint, int]] = []           # waypoints made, and the tick of their mark
        self.last_frame = -self.frame_every
        self.shown = ('', '')                                   # the caption: mode, text
        if self.watch:
            self.watch.mkdir(parents=True, exist_ok=True)

    # -- the loop --
    @property
    def milestone(self) -> Milestone | None:
        return self.milestones[self.current] if self.current < len(self.milestones) else None

    def run(self, max_ticks: int) -> dict:
        while self.host.tick < max_ticks and self.milestone:
            obs = self.observe()
            self.jev.ledger.context['t'] = self.router.ledger.context['t'] = self.host.tick
            self.messages += obs.get('messages', [])
            self.walker.memory.note(obs)
            if self.milestone.done(obs, self):
                self._reached(obs)
                continue
            if self.equip(obs):
                continue
            mode = self.mode(obs)
            self.note('mode', mode=mode)
            getattr(self, mode)(obs)
            self.publish()
            if mode == 'dead':
                break
        return {'tick': self.host.tick, 'reached': [m.name for m in self.milestones[self.start:self.current]],
                'waypoints': [str(w.dir) for w, _ in self.created], 'spend': self.jev.ledger.summary()}

    def observe(self, frame: bool = False, walkgrid: int | None = None) -> dict:
        """An observation, logged (all of them: the gameplay log), with a
        frame when one is due. The walk grid, when asked for, is used but
        not logged: it is large and the map's, not the session's."""
        due = frame or self.host.tick - self.last_frame >= self.frame_every
        path = self.bundle / 'frames' / f't{self.host.tick:07d}.png' if due else None
        if path:
            path.parent.mkdir(parents=True, exist_ok=True)
            self.last_frame = self.host.tick
        if walkgrid is not None and self.no_walkgrid:
            walkgrid = None
        try:
            obs = self.host.observe(objects=800, frame=path, walkgrid=walkgrid)
        except HostError as error:
            if walkgrid is None or 'walkgrid' not in str(error):
                raise
            self.no_walkgrid = True                             # this host has none: walk without it
            self.note('no walkgrid', host=str(error))
            obs = self.host.observe(objects=800, frame=path)
        logged = {k: v for k, v in obs.items() if k != 'walkgrid'}
        with (self.bundle / 'observations.jsonl').open('a', encoding='utf-8') as f:
            f.write(json.dumps(logged, separators=(',', ':')) + '\n')
        return obs

    def mode(self, obs: dict) -> str:
        screen = obs.get('screen')
        if screen == 'death':
            return 'dead'
        if screen in ('loading', 'movie'):
            return 'wait'
        if (obs.get('dialog') or {}).get('choices'):
            return 'dialog'
        if screen != 'play':
            return 'unknown'
        if not obs.get('control', True):
            return 'wait'
        if self.threats(obs):
            return 'combat'
        return 'explore'

    def threats(self, obs: dict) -> list[dict]:
        """Living enemies near enough to fight. Locke squares up to them
        rather than waiting to be attacked: in the Keep, Rahul stands in his
        fighting stance and only strikes once Locke closes."""
        player = obs['player']
        return [o for o in obs.get('objects', [])
                if o.get('class') == 'character' and o.get('enemy') and (o.get('health') or [1])[0] > 0
                and distance(o['pos'], player['pos']) < ENGAGE_RADIUS]

    # -- equipment --
    def equip(self, obs: dict) -> bool:
        """Wear and wield what the pack holds for an empty equipment slot:
        right-click it, as a player does (PLAYER_INPUT.md §6: the item goes
        to its own slot). One item a turn, each slot tried once, and only
        while control is Locke's and no enemy is close enough to strike."""
        player = obs.get('player') or {}
        if not obs.get('control') or obs.get('screen') != 'play' or (obs.get('dialog') or {}).get('choices'):
            return False
        if any(distance(o['pos'], player['pos']) < 160 for o in self.threats(obs)):
            return False
        worn = {int(k) for k in (player.get('equipped') or {})}
        for item in player.get('pack') or []:
            if (item.get('eqslot', -1) >= 0 and item['eqslot'] not in worn and item.get('screen')
                    and item['slot'] not in self.equip_tried):
                self.equip_tried.add(item['slot'])
                self.caption('equip', f"putting on {item['name']}")
                plan = Plan(self.state)
                end = plan.click(0, *item['screen'], button=BUTTON_RIGHT)
                self.host.step(plan.ticks(end + 4), plan.events())
                self.state = plan.state
                after = self.observe()['player'].get('equipped') or {}
                self.note('equip', item=item['name'], eqslot=item['eqslot'],
                          worn=str(item['eqslot']) in after)
                return True
        return False

    # -- modes --
    def wait(self, obs: dict) -> None:
        self.caption('watching', 'the scene plays' if not obs.get('control', True) else 'loading')
        self.host.step(12)

    def dialog(self, obs: dict) -> None:
        dialog = obs['dialog']
        choices = dialog['choices'][:len(DIGITS)]
        if dialog.get('speaker'):
            self.talked.add(dialog['speaker'])
        asked = self.choices_taken.setdefault(dialog.get('speaker', '') + '|' + '|'.join(choices), set())
        options = {DIGITS[i]: text + (' (already chosen before)' if DIGITS[i] in asked else '')
                   for i, text in enumerate(choices)}
        state = {'objective': self.milestone.objective, 'speaker': dialog.get('speaker'),
                 'recent_lines': self.messages[-6:]}
        pick = self.jev.ask(state, tag='dialog', reply=Choice(
            'Which reply should Locke give to `speaker` to make progress on `objective`? A reply already '
            'chosen before leads back to what was heard.', options))['reply']
        asked.add(pick.choice)
        text = choices[int(pick.choice) - 1]
        self.caption('dialog', f'reply {pick.choice}: "{text}" ({pick.confidence:.2f})')
        self.note('dialog', speaker=dialog.get('speaker'), choice=pick.choice, text=text)
        self.press(chord(pick.choice), then=24)

    def combat(self, obs: dict) -> None:
        if obs['player'].get('mode') != 'combat':
            self.press(self.bindings['CombatMode'], then=6)
            return
        if not self.tactician.steps:
            self.calibrate()
            return
        if not self.tactician.history:
            self.observe(frame=True)                                 # the first contact of a fight
        tactic = self.tactician.decide(obs)
        if not tactic:
            self.host.step(6)
            return
        plan = Plan(self.state)
        ticks = self.tactician.plan(tactic, obs, plan)
        s = self.tactician.assess(obs)
        self.caption('combat', f"{tactic.name.replace(':', ' ')} against {s.target.get('type') or s.target.get('name')} "
                               f"({s.phase.word}, {s.reach}) - {tactic.reason} {tactic.confidence:.2f}")
        self.note('tactic', tactic=tactic.name, reason=tactic.reason, confidence=round(tactic.confidence, 2))
        self.host.step(plan.ticks(ticks), plan.events())
        self.state = plan.state

    def explore(self, obs: dict) -> None:
        if obs['player'].get('mode') == 'combat':                    # a click in combat attacks
            self.press(self.bindings['CombatMode'], then=6)
            return
        if self.milestone.goal and self.world and self.travel(obs, self.milestone.goal):
            return
        full = self.observe(walkgrid=WALKGRID_RADIUS)
        grid = WalkGrid(full['walkgrid']) if full.get('walkgrid') else None
        candidates = self.walker.destinations(self.host, full, grid)
        if not candidates:
            self.unknown(full, why='no destination in view')
            return
        dest = self.walker.choose(self.milestone.objective, full, candidates)
        self.caption('explore', f'walking to {dest.words}')
        result, _, self.state = self.walker.walk(self.host, self.state, dest, observe=self.observe)
        self.note('walk', to=dest.key, words=dest.words, result=result)
        if result == 'talked':
            self.talked.add(dest.name)
        if result != 'arrived':
            self.observe(frame=True)

    def travel(self, obs: dict, goal: str) -> bool:
        """One leg toward `goal`: find it (in view, else in the map data),
        route through the exits that get there, then use or talk to the
        next stop once in reach, or walk the path toward it. False when
        there is no way to tell where to go, so exploring takes over."""
        player = obs['player']
        level, pos = player.get('level'), player['pos']
        named = {(o.get('name') or '').lower(): o for o in obs.get('objects', [])}
        live = named.get(goal.lower())
        if live:
            place = Place(level, tuple(live['pos']), goal, live.get('type', ''), live.get('class', ''))
        else:
            places = self.world.find(goal)
            place = next((p for p in places if p.level == level), places[0] if places else None)
        if not place:
            return False
        route = self.world.route(level, pos, place)
        if route is None:
            return False
        stop_name, stop_pos = (route[0].name, route[0].pos) if route else (goal, place.pos)
        if route and route[0].name.lower() in self.blocked_exits:
            return False                                             # that exit wouldn't open: explore instead
        stop = named.get(stop_name.lower())
        words = f"'{stop_name}'" + (f", on the way to '{goal}'" if route else '')
        if stop and stop.get('screen') and distance(stop['pos'], pos) <= USE_REACH:
            person = stop.get('class') == 'character'
            spot = self.clickable(stop)
            if not spot:
                # nothing of it to click from here (Locke stands in front): step to its side
                return self.step_aside(obs, stop)
            self.caption('travel', ('talking to ' if person else 'using ') + words)
            dest = Destination(f"use:{stop['id']}", words, spot, None, stop['id'], use=True,
                               name=stop_name, person=person)
            result, after, self.state = self.walker.walk(self.host, self.state, dest, observe=self.observe)
            if person:
                result = 'talked' if self.conversation_follows() else 'no answer'
                if result == 'talked':
                    self.talked.add(stop_name)
            self.note('travel', to=stop_name, goal=goal, result=result)
            if person:
                return True
            if distance(after['player']['pos'], pos) < 2 and after['player'].get('level') == level:
                tries = self.exit_tries[stop_name.lower()] = self.exit_tries.get(stop_name.lower(), 0) + 1
                if tries >= 3:
                    self.blocked_exits.add(stop_name.lower())        # locked, or a script's turn first
            return True
        full = self.observe(walkgrid=WALKGRID_RADIUS)
        if not full.get('walkgrid'):
            return False
        grid = WalkGrid(full['walkgrid'])
        self.maps.merge(level, grid)
        radius = player.get('radius') or DEFAULT_RADIUS
        path = self.maps.plan(level, pos, stop_pos, radius)
        if path and distance(path[-1], stop_pos) > USE_REACH and (level, tuple(stop_pos)) not in self.wide_looks:
            # the way isn't in the map yet: look wider once (the host holds the whole level)
            self.wide_looks.add((level, tuple(stop_pos)))
            wide = self.observe(walkgrid=WIDE_WALKGRID_RADIUS)
            if wide.get('walkgrid'):
                self.maps.merge(level, WalkGrid(wide['walkgrid']))
                path = self.maps.plan(level, pos, stop_pos, radius)
        if len(path) < 2 or distance(path[-1], pos) < 32:
            # nowhere new to go on the map seen so far: mark it, and let exploring take a turn
            self.maps.dead_end(level, pos)
            self.note('dead end', at=list(pos[:2]), toward=stop_name, why='no way on the known map')
            return False
        # the game takes a click's point at Locke's height + 50, so aim there
        shown = dict(zip(path, self.host.project([[x, y, pos[2] + CLICK_HEIGHT] for x, y in path])))
        field_rect = full.get('playfield', self.walker.playfield)
        point = farthest_clear(grid, pos, path, lambda p: bool(shown[p][0]) and shown[p][1]
                               and in_rect(shown[p][0], field_rect))
        if not point:
            return False
        self.caption('travel', f'walking toward {words}')
        dest = Destination(f'nav:{point[0]}:{point[1]}', f'toward {words}', tuple(shown[point][0]), (*point, pos[2]))
        result, after, self.state = self.walker.walk(self.host, self.state, dest, observe=self.observe)
        self.note('travel', to=stop_name, goal=goal, result=result, point=list(point))
        here = after['player']['pos']
        gap = distance(here, stop_pos)
        if gap < self.best_gap - 100:
            self.best_gap, self.dead_ends = gap, 0               # getting closer: the dead ends did their job
        if result in ('stalled', 'refused'):
            self.stuck = (self.stuck + 1) if self.stuck_at and distance(here, self.stuck_at) < 24 else 1
            self.stuck_at = here
            if self.stuck >= 3:                                  # this spot is a dead end: aim elsewhere
                self.stuck = 0
                self.maps.dead_end(level, here)
                self.dead_ends += 1
                self.note('dead end', at=list(here[:2]), toward=stop_name)
                if self.dead_ends >= 5:                          # no nearer after five: a look at the screen
                    self.dead_ends = 0
                    self.unknown(after, why=f"Locke can't find the way toward {words}; pick a point on "
                                            f"open ground, stairs or a door to click that leads on")
        else:
            self.stuck = 0
        return result != 'refused'

    def clickable(self, obj: dict) -> tuple[int, int] | None:
        """A screen point that the game's own pick resolves to `obj` and
        that reaches the world: its feet, body and head first. The object's
        screen position is where its feet are; Locke, a wall or bars may
        cover that, and a click then lands on whatever is on top."""
        fx, fy = obj['screen']
        for dx, dy in ((0, -30), (0, -45), (0, -15), (-12, -30), (12, -30), (0, -60), (-20, -45), (20, -45), (0, 0)):
            x, y = fx + dx, fy + dy
            if not (0 <= x < 640 and 0 <= y < 480):
                continue
            picked = self.host.pick(x, y)
            if 'object' not in picked:
                return fx, fy - 30                  # a host that can't pick objects: the body, untested
            if picked.get('object') == obj['id'] and picked.get('world', True):
                return x, y
        return None

    def step_aside(self, obs: dict, obj: dict) -> bool:
        """Walk a few steps to the side of `obj`, so it isn't behind Locke."""
        pos, at = obs['player']['pos'], obj['pos']
        d = max(1.0, distance(pos, at))
        side = (at[0] + (at[1] - pos[1]) / d * 70, at[1] - (at[0] - pos[0]) / d * 70)
        (shown, reaches), = self.host.project([[side[0], side[1], pos[2] + CLICK_HEIGHT]])
        if not shown or not reaches:
            return False
        self.caption('travel', f"stepping aside to reach '{obj.get('name')}'")
        dest = Destination('aside', 'aside', tuple(shown), (round(side[0]), round(side[1]), pos[2]))
        result, _, self.state = self.walker.walk(self.host, self.state, dest, observe=self.observe)
        self.note('step aside', of=obj.get('name'), result=result)
        return result != 'refused'

    def conversation_follows(self, within: int = 72) -> bool:
        """After a click on a person: does a conversation start (a dialog up,
        a line of speech, or a scene that takes control) within `within` ticks?"""
        waited = 0
        while waited < within:
            obs = self.observe()
            self.messages += obs.get('messages', [])
            if obs.get('screen') == 'dialog' or (obs.get('dialog') or {}).get('choices') \
                    or (obs.get('dialog') or {}).get('text') or not obs.get('control', True):
                return True
            self.host.step(12)
            waited += 12
        return False

    def dead(self, obs: dict) -> None:
        self.observe(frame=True)
        self.host.mark('death')
        self.note('dead')

    def unknown(self, obs: dict, why: str = 'a screen the director has no state for') -> None:
        self.caption('looking', f'{why}: asking the router')
        self.observe(frame=True)
        frame = self.bundle / 'frames' / f't{self.host.tick:07d}.png'
        reply = self.router.ask(
            f'This is the 1999 PC game Revenant, 640x480. The player character is Locke. Objective: '
            f'{self.milestone.objective}. Situation: {why}. Choose one action with a tool.',
            images=[frame], tag='unknown', tool_choice='required',
            tools=[tool('click', 'Left-click a screen point', {'x': {'type': 'integer'}, 'y': {'type': 'integer'}},
                        ['x', 'y']),
                   tool('press', 'Press a key (a Windows key name such as RETURN, ESCAPE, C, 1)',
                        {'key': {'type': 'string'}}, ['key']),
                   tool('wait', 'Let the game run', {'seconds': {'type': 'number'}}, ['seconds'])])
        call = reply.tool_calls[0] if reply.tool_calls else None
        self.note('router', why=why, call=vars(call) if call else None, said=reply.content[:300])
        plan = Plan(self.state)
        if call and call.name == 'click':
            end = plan.click(0, int(call.arguments['x']) % 640, int(call.arguments['y']) % 480)
            self.host.step(plan.ticks(end + 12), plan.events())
        elif call and call.name == 'press':
            try:
                end = plan.press(0, chord(str(call.arguments['key'])))
            except ValueError:
                end = 0
            self.host.step(plan.ticks(end + 12), plan.events())
        else:
            seconds = float((call.arguments if call else {}).get('seconds', 2))
            self.host.step(max(1, min(240, round(seconds * 24))))
        self.state = plan.state

    def calibrate(self, hold: int = 3) -> None:
        """Measure where each direction control steps Locke in the world:
        a short step with each, opposites in turn, so he ends near where he
        began. The tactician then steers by world vectors, which works as
        well for an enemy off the screen. A control that didn't move him
        (the first press can go to turning him) is tried again at the end."""
        steps: dict[str, tuple[float, float]] = {}
        for name in [*CALIBRATION_ORDER, *CALIBRATION_ORDER]:
            if name in steps:
                continue
            before = self.observe()['player']['pos']
            self.press(self.bindings[name], then=hold)
            after = self.observe()['player']['pos']
            if (after[0], after[1]) != (before[0], before[1]):
                steps[name] = (after[0] - before[0], after[1] - before[1])
        self.tactician.steps = steps
        self.note('calibrated', steps={k: list(v) for k, v in steps.items()})

    # -- milestones and waypoints --
    def _reached(self, obs: dict) -> None:
        milestone = self.milestone
        self.caption('milestone', milestone.name)
        self.observe(frame=True)
        self.host.mark('milestone', name=milestone.name)
        self.note('milestone', name=milestone.name)
        if milestone.save and self.waypoints:
            try:
                self.press(self.bindings['QuickSave'], then=48)
                exported = self.host.export_slot('@last', self.bundle / 'exports' / str(self.host.tick))
                wrong = wrong_states(story_states(Path(exported['path'])), milestone.expect)
                if wrong:                                    # kept as evidence, never played from
                    self.note('save suspect', name=milestone.name, states=wrong)
                waypoint = self.waypoints.add(
                    self.game, milestone.name, self.story, self.current + 1, self.parent, exported['path'],
                    obs=obs, build=self.host.info.get('build'), tick=self.host.tick,
                    slot_name=exported['slot'], retired=f'story states {wrong}' if wrong else '')
                self.host.mark('saved', waypoint=waypoint.dir.name, slot=exported['slot'])
                self.created.append((waypoint, self.host.tick))
                self.parent = waypoint.name
                self.note('waypoint', name=milestone.name, path=str(waypoint.dir), slot=exported['slot'])
            except (HostError, KeyError) as error:
                self.note('save failed', error=str(error))
        self.current += 1

    # -- watching --
    def caption(self, mode: str, text: str) -> None:
        """What the director is doing, for a person watching: drawn by the
        host over its window, and in the watch page's state."""
        if (mode, text) == self.shown:
            return
        self.shown = (mode, text)
        if self.host.capable('caption'):
            self.host.request('caption', text=f'{mode}: {text}')
        self.publish()

    def publish(self) -> None:
        if not self.watch:
            return
        state = {'game': self.game, 'tick': self.host.tick, 'mode': self.shown[0], 'caption': self.shown[1],
                 'objective': self.milestone.objective if self.milestone else 'done',
                 'milestones': [m.name for m in self.milestones], 'current': self.current,
                 'spend': self.jev.ledger.summary()}
        temporary = self.watch / 'state.json.tmp'
        temporary.write_text(json.dumps(state))
        temporary.replace(self.watch / 'state.json')

    # -- helpers --
    def press(self, keys, then: int = 0) -> None:
        plan = Plan(self.state)
        end = plan.press(0, keys)
        self.host.step(plan.ticks(end + then), plan.events())
        self.state = plan.state

    def note(self, kind: str, **fields) -> None:
        entry = {'t': self.host.tick, 'kind': kind, **fields}
        self.log.append(entry)
        with (self.bundle / 'director.jsonl').open('a', encoding='utf-8') as f:
            f.write(json.dumps(entry) + '\n')


# ---- checks for milestones -----------------------------------------------------

def message_seen(text: str) -> Callable[[dict, Director], bool]:
    lowered = text.lower()
    return lambda obs, d: any(lowered in m.lower() for m in d.messages)


def control_on(obs: dict, d: Director) -> bool:
    return bool(obs.get('control')) and obs.get('screen') == 'play' and not (obs.get('dialog') or {}).get('choices')


def character_dead(name: str) -> Callable[[dict, Director], bool]:
    def check(obs: dict, d: Director) -> bool:
        o = next((o for o in obs.get('objects', []) if (o.get('name') or '').lower() == name.lower()), None)
        return o is not None and (o.get('health') or [1])[0] <= 0
    return check


def after_scene(check: Callable[[dict, Director], bool], settle: int = 48,
                give_up: int = 24 * 60) -> Callable[[dict, Director], bool]:
    """`check`, then the scene it sets off: done once `check` has held and
    a scene (control off) has played and given control back for `settle`
    ticks, or `give_up` ticks after `check` first held when no scene comes.
    A waypoint saved there is past the scene, which also keeps saves out of
    the half-settled moment just after a death (FINDINGS.md F1)."""
    seen = {'since': None, 'off': False, 'on_since': None}

    def done(obs: dict, d: Director) -> bool:
        if seen['since'] is None:
            if not check(obs, d):
                return False
            seen['since'] = d.host.tick
        if not obs.get('control'):
            seen['off'], seen['on_since'] = True, None
            return False
        if seen['on_since'] is None:
            seen['on_since'] = d.host.tick
        if seen['off'] and d.host.tick - seen['on_since'] >= settle:
            return True
        return d.host.tick - seen['since'] >= give_up
    return done


def talked_to(name: str) -> Callable[[dict, Director], bool]:
    """Locke has talked to someone whose name contains `name` (instance
    names carry suffixes: RandK, SardokR)."""
    lowered = name.lower()
    return lambda obs, d: any(lowered in who.lower() for who in d.talked)


def level_is(level: int) -> Callable[[dict, Director], bool]:
    return lambda obs, d: obs.get('player', {}).get('level') == level


def every(*checks: Callable[[dict, Director], bool]) -> Callable[[dict, Director], bool]:
    return lambda obs, d: all(check(obs, d) for check in checks)


def near(pos: tuple[int, int], within: int = 200) -> Callable[[dict, Director], bool]:
    return lambda obs, d: math.hypot(obs['player']['pos'][0] - pos[0], obs['player']['pos'][1] - pos[1]) <= within
