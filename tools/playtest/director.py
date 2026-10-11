"""The play loop: modes owned by code, narrow questions to Jev.

    director = Director(host, jev, router, kb, bindings, bundle, milestones, saves)
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

The milestone list gives the objective; a milestone is done when its check
holds, and a milestone marked for saving quick-saves and exports the slot
to the save library with its recording. Everything the director does goes
to the host as input, so the session's recording replays it.
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
from jev import Choice, Jev, Router, tool
from knowledge import Knowledge
from walk import Walker, WalkGrid

COMBAT_RADIUS = 400                  # an enemy this close and on Locke means a fight
WALKGRID_RADIUS = 480
DIGITS = '123456'
# Each direction straight after its opposite, so calibrating ends where it began.
CALIBRATION_ORDER = tuple(DIRECTIONS[i + offset] for i in range(4) for offset in (0, 4))


@dataclass
class Milestone:
    name: str
    objective: str                   # what Jev is told to make progress on
    done: Callable[[dict, 'Director'], bool]
    save: bool = False


@dataclass
class SaveLibrary:
    """`<root>/<game>/<milestone>/<slot>/` with a manifest beside the slot."""
    root: Path

    def keep(self, host: Host, game: str, milestone: str, obs: dict, bundle: Path) -> Path:
        target = self.root / game / milestone.replace('/', '-')
        if target.exists():
            shutil.rmtree(target)
        target.mkdir(parents=True)
        exported = host.export_slot('@last', target / 'slot')
        player = obs.get('player', {})
        (target / 'manifest.json').write_text(json.dumps({
            'game': game, 'build': host.info.get('build'), 'milestone': milestone, 'tick': host.tick,
            'slot': exported.get('slot'), 'level': player.get('level'), 'pos': player.get('pos'),
            'health': player.get('health'), 'xp': player.get('xp'), 'recording': str(bundle),
            'created': time.strftime('%Y-%m-%dT%H:%M:%S')}, indent=2))
        return target


@dataclass
class Director:
    host: Host
    jev: Jev
    router: Router
    kb: Knowledge
    bindings: Bindings
    bundle: Path
    milestones: list[Milestone]
    saves: SaveLibrary | None = None
    game: str = 'port'
    watch: Path | None = None        # where a watcher's page reads state.json (watch.py)
    state: InputState = field(default_factory=InputState)
    log: list[dict] = field(default_factory=list)
    messages: list[str] = field(default_factory=list)
    choices_taken: dict[str, set[str]] = field(default_factory=dict)

    def __post_init__(self) -> None:
        self.walker = Walker(self.jev, self.bindings)
        self.tactician = Tactician(self.jev, self.kb, self.bindings)
        self.current = 0
        self.shown = ('', '')                                   # the caption: mode, text
        if self.watch:
            self.watch.mkdir(parents=True, exist_ok=True)

    # -- the loop --
    @property
    def milestone(self) -> Milestone | None:
        return self.milestones[self.current] if self.current < len(self.milestones) else None

    def run(self, max_ticks: int) -> dict:
        while self.host.tick < max_ticks and self.milestone:
            obs = self.host.observe(objects=800)
            self.messages += obs.get('messages', [])
            self.walker.memory.note(obs)
            if self.milestone.done(obs, self):
                self._reached(obs)
                continue
            mode = self.mode(obs)
            self.note('mode', mode=mode)
            getattr(self, mode)(obs)
            self.publish()
            if mode == 'dead':
                break
        return {'tick': self.host.tick, 'reached': [m.name for m in self.milestones[:self.current]],
                'spend': self.jev.ledger.summary()}

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
        player = obs['player']
        return [o for o in obs.get('objects', [])
                if o.get('class') == 'character' and o.get('enemy') and (o.get('health') or [1])[0] > 0
                and distance(o['pos'], player['pos']) < COMBAT_RADIUS
                and (o.get('target') == 0 or player.get('mode') == 'combat')]

    # -- modes --
    def wait(self, obs: dict) -> None:
        self.caption('watching', 'the scene plays' if not obs.get('control', True) else 'loading')
        self.host.step(12)

    def dialog(self, obs: dict) -> None:
        dialog = obs['dialog']
        choices = dialog['choices'][:len(DIGITS)]
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
        if not self.tactician.directions and self.host.capable('project'):
            self.calibrate()
            return
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
        full = self.host.observe(objects=800, walkgrid=WALKGRID_RADIUS)
        grid = WalkGrid(full['walkgrid']) if full.get('walkgrid') else None
        candidates = self.walker.destinations(self.host, full, grid)
        if not candidates:
            self.unknown(full, why='no destination in view')
            return
        dest = self.walker.choose(self.milestone.objective, full, candidates)
        self.caption('explore', f'walking to {dest.words}')
        result, _, self.state = self.walker.walk(self.host, self.state, dest)
        self.note('walk', to=dest.key, words=dest.words, result=result)

    def dead(self, obs: dict) -> None:
        self.host.mark('death')
        self.note('dead')

    def unknown(self, obs: dict, why: str = 'a screen the director has no state for') -> None:
        self.caption('looking', f'{why}: asking the router')
        frame = self.bundle / 'frames' / f't{self.host.tick:07d}.png'
        frame.parent.mkdir(parents=True, exist_ok=True)
        self.host.observe(frame=frame)
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
        """Measure which way on screen each direction control steps Locke:
        step briefly with each, and project where he started and ended with
        the same camera (the camera follows him, so his own screen position
        hardly moves). Opposite steps follow each other, so he ends where he
        began."""
        steps: dict[str, tuple[float, float]] = {}
        for name in CALIBRATION_ORDER:
            before = self.host.observe(objects=0)['player']['pos']
            self.press(self.bindings[name], then=hold)
            after = self.host.observe(objects=0)['player']['pos']
            a, b = self.host.project([before, after])
            if a and b and (a[0], a[1]) != (b[0], b[1]):
                steps[name] = (b[0] - a[0], b[1] - a[1])
        self.tactician.directions = steps
        self.note('calibrated', steps={k: [round(v, 1) for v in s] for k, s in steps.items()})

    # -- milestones and saves --
    def _reached(self, obs: dict) -> None:
        milestone = self.milestone
        self.caption('milestone', milestone.name)
        self.host.mark('milestone', name=milestone.name)
        self.note('milestone', name=milestone.name)
        if milestone.save and self.saves:
            try:
                self.press(self.bindings['QuickSave'], then=48)
                kept = self.saves.keep(self.host, self.game, milestone.name, obs, self.bundle)
                self.host.mark('saved', milestone=milestone.name, path=str(kept))
                self.note('saved', path=str(kept))
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


def level_is(level: int) -> Callable[[dict, Director], bool]:
    return lambda obs, d: obs.get('player', {}).get('level') == level


def near(pos: tuple[int, int], within: int = 200) -> Callable[[dict, Director], bool]:
    return lambda obs, d: math.hypot(obs['player']['pos'][0] - pos[0], obs['player']['pos'][1] - pos[1]) <= within
