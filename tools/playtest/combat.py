"""Combat: Jev picks a tactic, code turns it into tick-exact input.

Revenant's fighting is tactical rather than reflexive (docs/playtest/PLAN.md
§11): in combat mode Locke steps in 8 directions while facing one target,
attacks need no aiming, and what matters is the choice of attack, combo,
block, dodge or spell, and moving around the enemy. So at each decision
point the tactician asks Jev, in one request, for an intent (attack, defend,
reposition, magic, heal) and, speculatively, for the best option within
each intent; code takes the option of the chosen intent (TypeSafe's
"speculative fan-out"). A low-confidence intent falls back to a rule.

Everything numeric stays here. The distance becomes words from char.def's
attack bands: each attack button has variants chosen by distance (a close
blow, a normal blow, a charging lunge), so "close", "a lunge away" and
"far" follow from which of Locke's and the enemy's ordinary attacks cover
it. The enemy's action becomes a phase from its attack's timings (winding
up, about to strike, striking, recovering). Directions become keys through
each direction control's step measured in the world at the first fight
(the director's calibration), so an enemy off the screen is steered toward
as surely as one on it.

Two of retail's rules shape the options (docs/playtest/GRIND.md §2.4-2.5):
the third press in a row of the same attack button on one target gets the
worst hit roll and risks an instant counter (ButtonAttack 0x4d2480), so a
button pressed twice running isn't offered a third time; and barehanded
Locke has grapples (throws on Ctrl+A, Ctrl+S) that only some monsters can
be thrown with, so a grapple the game refuses against a kind of monster
isn't offered against that kind again.
"""
from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Sequence

from inputs import Bindings, Plan
from jev import Choice, Jev
from knowledge import WM_HAND, Attack, Knowledge

# Screen directions, counter-clockwise from screen right: the controls that
# step that way, and with "Dodge" in front, leap (Revenant.ini [Controls]).
DIRECTIONS = ('Right', 'UpRight', 'Up', 'UpLeft', 'Left', 'DownLeft', 'Down', 'DownRight')
LUNGE = 70                           # an attack chosen only beyond this is a charge
SAME_BUTTON_LIMIT = 2                # presses in a row of one button before the third is penalised
GRAPPLE_REACH = 40                   # a grapple is offered within this (the finishers' band, GRIND §2.5)

INTENTS = {
    'attack': 'Strike the target now.',
    'defend': 'Protect Locke from a blow: block it or leap clear.',
    'reposition': 'Move: close the distance, back away, or circle the target.',
    'magic': 'Cast one of the quick spells at the target.',
    'heal': 'Drink a healing potion from the belt.',
}
DEFENCES = {
    'block': 'Raise the guard: blows cost fatigue instead of health. Best against a strike about to land.',
    'dodge_back': 'Leap backward, out of reach.',
    'dodge_left': "Leap sideways to Locke's left.",
    'dodge_right': "Leap sideways to Locke's right.",
}
MOVES = {
    'orbit_left': 'Circle the target to the left, keeping the distance.',
    'orbit_right': 'Circle the target to the right, keeping the distance.',
    'close_in': 'Step toward the target.',
    'back_off': 'Step away from the target.',
}


# ---- words ---------------------------------------------------------------------

def fraction_word(pair: Sequence[int] | None, words: Sequence[str]) -> str:
    """[current, maximum] as one of four words, from best to worst."""
    if not pair or not pair[1]:
        return 'unknown'
    f = pair[0] / pair[1]
    return words[0] if f > 0.75 else words[1] if f > 0.45 else words[2] if f > 0.2 else words[3]


def health_word(pair) -> str:
    return fraction_word(pair, ('healthy', 'wounded', 'badly hurt', 'near death'))


def fatigue_word(pair) -> str:
    return fraction_word(pair, ('fresh', 'winded', 'tired', 'exhausted'))


def mana_word(pair) -> str:
    return fraction_word(pair, ('full', 'half', 'low', 'nearly empty'))


def distance(a: Sequence[int], b: Sequence[int]) -> float:
    return math.hypot(a[0] - b[0], a[1] - b[1])


def describe(attack: Attack, key: str) -> str:
    """An attack variant in words, from its data."""
    speed = 'fast' if attack.impacttime <= 3 else 'medium' if attack.impacttime <= 6 else 'slow'
    kind = 'charging lunge' if attack.mindist >= LUNGE else 'blow'
    return (f'{key}: a {speed} {kind} (lands after {attack.impacttime} ticks), '
            f'damage {attack.damagemod:+d}%, fatigue {attack.fatigue}.')


@dataclass(frozen=True)
class Phase:
    """What a character's current action means to an opponent."""
    word: str                        # 'about to strike', 'striking', 'recovering after an attack', ...
    attack: Attack | None = None
    ticks_to_impact: int | None = None


def action_phase(kb: Knowledge, kind: str, name: str | None, tick: int | None) -> Phase:
    """The phase of `kind`'s current action, from char.def's timings."""
    if not name:
        return Phase('unknown')
    character = kb.character(kind)
    attack = next((a for a in character.attacks if a.name == name), None) if character else None
    if attack and tick is not None:
        if tick < attack.impacttime:
            left = attack.impacttime - tick
            word = 'about to strike' if left <= attack.blocktime + 2 else 'winding up an attack'
            return Phase(word, attack, left)
        if tick <= attack.impacttime + 2:
            return Phase('striking', attack, 0)
        return Phase('recovering after an attack', attack)
    lowered = name.lower()
    for key, word in (('block', 'blocking'), ('stun', 'stunned'), ('down', 'knocked down'),
                      ('dead', 'dead'), ('death', 'dying'), ('invoke', 'casting a spell'),
                      ('imp', 'reeling from a hit'), ('walk', 'moving'), ('run', 'moving'),
                      ('step', 'moving'), ('combat', 'ready'), ('dodge', 'dodging')):
        if key in lowered:
            return Phase(word)
    return Phase('busy')


# ---- the situation -------------------------------------------------------------

@dataclass(frozen=True)
class Situation:
    """One decision's facts, computed once."""
    target: dict
    d: float
    attacks: tuple[Attack, ...]      # Locke's usable ordinary attacks, every variant
    reach: str                       # 'close', 'a lunge away', 'far'
    in_its_reach: bool
    phase: Phase                     # the target's
    mine: Phase                      # Locke's

    def variant(self, button: int) -> Attack | None:
        """The button's attack at this distance: its first variant whose
        band covers it, else (combos) its first."""
        same = [a for a in self.attacks if a.button == button]
        return next((a for a in same if a.covers(self.d)), same[0] if same else None)

    @property
    def distance_words(self) -> str:
        return {('close', True): 'close: both can strike',
                ('close', False): 'close: Locke can strike, it cannot reach him',
                ('a lunge away', True): 'a lunge away: Locke can charge in, and it can reach him',
                ('a lunge away', False): 'a lunge away: Locke can charge in',
                ('far', True): "out of Locke's reach but within its reach",
                ('far', False): 'too far for either to strike'}[self.reach, self.in_its_reach]


@dataclass(frozen=True)
class Tactic:
    name: str                        # 'attack:2', 'block', 'orbit_left', 'cast:1', 'potion:3', ...
    reason: str                      # 'jev', or the rule that chose it
    confidence: float = 0.0


@dataclass
class Tactician:
    jev: Jev
    kb: Knowledge
    bindings: Bindings
    steps: dict[str, tuple[float, float]] = field(default_factory=dict)  # control -> world step, measured
    confidence_floor: float = 0.25
    history: list[str] = field(default_factory=list)
    messages: list[str] = field(default_factory=list)        # the game's text-bar lines, the director's
    presses: list[tuple[int, int]] = field(default_factory=list)   # (target id, attack button), in order
    refused: set[tuple[str, int]] = field(default_factory=set)     # (monster kind, grapple button) refused
    pending: tuple[str, int, frozenset[str]] | None = None         # a grapple pressed: kind, button, its moves

    def target_of(self, obs: dict) -> dict | None:
        """The enemy to fight: Locke's own target while it lives, else the
        nearest living enemy (the game keeps a dead target until he turns
        to another)."""
        player = obs['player']
        enemies = [o for o in obs.get('objects', []) if o.get('class') == 'character' and o.get('enemy')
                   and (o.get('health') or [1])[0] > 0]
        chosen = next((o for o in enemies if o['id'] == player.get('target')), None)
        return chosen or (min(enemies, key=lambda o: distance(o['pos'], player['pos'])) if enemies else None)

    def assess(self, obs: dict) -> Situation | None:
        target = self.target_of(obs)
        if not target:
            return None
        player = obs['player']
        skills = player.get('skills', {})
        attacks = tuple(self.kb.locke_attacks(player.get('weapon', {}).get('mask', 0) or WM_HAND,
                                              skills.get('attack', 0), skills.get('weapon', 0)))
        d = distance(player['pos'], target['pos'])
        covering = [a for a in attacks if a.covers(d)]
        reach = ('far' if not covering else
                 'a lunge away' if all(a.mindist >= LUNGE for a in covering) else 'close')
        kind = target.get('type') or target.get('name', '')
        return Situation(target, d, attacks, reach,
                         any(a.covers(d) for a in self.kb.attacks(kind)),
                         action_phase(self.kb, kind, target.get('action_name'), target.get('action_tick')),
                         action_phase(self.kb, 'Locke', player.get('action_name'), player.get('action_tick')))

    # -- what Jev is asked --
    def worn_out(self, target: dict) -> int | None:
        """The button pressed SAME_BUTTON_LIMIT times running on `target`:
        a third press would get the worst roll."""
        run = [b for t, b in self.presses[-SAME_BUTTON_LIMIT:] if t == target['id']]
        return run[0] if len(run) == SAME_BUTTON_LIMIT and len(set(run)) == 1 else None

    def grapples(self, obs: dict, s: Situation) -> dict[str, str]:
        """The grapples Locke can try on the target now: close in, fatigue
        enough, and not refused against its kind before."""
        player = obs['player']
        if s.d > GRAPPLE_REACH or s.mine.word not in ('ready', 'moving'):    # a busy Locke drops the press
            return {}
        fatigue = (player.get('fatigue') or [0, 0])[0]
        kind = s.target.get('type') or s.target.get('name', '')
        mask = (player.get('weapon') or {}).get('mask', 0) or WM_HAND
        options = {}
        for button, moves in self.kb.locke_grapples(mask).items():
            control = moves[0].control
            if (kind, button) in self.refused or control not in self.bindings or fatigue < min(m.fatigue for m in moves):
                continue
            options[f'grapple:{button}'] = (f'{self.bindings[control]}: grab and throw it ({", ".join(m.name for m in moves)}); '
                                            f'damage {moves[0].damagemod:+d}%, fatigue {moves[0].fatigue}. '
                                            f'Only some monsters can be thrown.')
        return options

    def options(self, obs: dict, s: Situation) -> dict[str, dict[str, str]]:
        """Each intent's available options, by name, with what they mean."""
        player = obs['player']
        attacks = {}
        worn = self.worn_out(s.target)
        if s.reach != 'far':
            for button in sorted({a.button for a in s.attacks}):
                v = s.variant(button)
                if not v or v.control not in self.bindings or (button > 3 and s.reach != 'close') or button == worn:
                    continue
                key = str(self.bindings[v.control])
                if button > 3:
                    blows = len([a for a in s.attacks if a.button == button])
                    attacks[f'attack:{button}'] = f'{key}: a learned combination of {blows} blows.'
                else:
                    attacks[f'attack:{button}'] = describe(v, key)
            attacks.update(self.grapples(obs, s))
        mana = (player.get('mana') or [0, 0])[0]
        spells = {}
        for slot, name in enumerate(player.get('quick_spells') or [], start=1):
            spell = self.kb.spells.get(name) if name else None
            if spell and spell.mana <= mana and f'Invoke{slot}' in self.bindings:
                spells[f'cast:{slot}'] = (f'Cast {spell.name}: {spell.damagetype} damage '
                                          f'{spell.damage[0]}-{spell.damage[1]}, costs {spell.mana} mana.')
        potions = {f'potion:{slot}': f'Drink {name}.'
                   for slot, name in enumerate(player.get('belt') or [], start=1)
                   if name and 'heal' in name.lower() and f'BeltUse{slot}' in self.bindings}
        options = {'attack': attacks, 'defend': dict(DEFENCES), 'reposition': dict(MOVES),
                   'magic': spells, 'heal': potions}
        return {intent: opts for intent, opts in options.items() if opts}

    def state(self, obs: dict, s: Situation) -> dict:
        player = obs['player']
        brief = self.kb.monster_brief(s.target.get('type') or s.target.get('name', ''))
        others = [o for o in obs.get('objects', []) if o.get('class') == 'character' and o.get('enemy')
                  and o['id'] != s.target['id'] and distance(o['pos'], player['pos']) < 300]
        return {
            'locke': {'health': health_word(player.get('health')), 'fatigue': fatigue_word(player.get('fatigue')),
                      'mana': mana_word(player.get('mana')), 'doing': s.mine.word,
                      'weapon': (player.get('weapon') or {}).get('name') or 'bare hands'},
            'target': {'kind': brief['kind'], 'distance': s.distance_words, 'doing': s.phase.word,
                       'health': health_word(s.target.get('health')),
                       'takes_extra_damage_from': brief.get('takes_extra_damage_from'),
                       'resists': brief.get('resists')},
            'other_enemies_near': len(others),
            'last_tactics': self.history[-3:],
            'recent_blows': [m for m in self.messages if ' vs. ' in m][-4:],
        }

    # -- deciding --
    def decide(self, obs: dict) -> Tactic | None:
        self.learn(obs)
        s = self.assess(obs)
        if not s:
            return None
        if s.reach == 'far' and not s.in_its_reach:
            return self._chose(Tactic('close_in', 'rule: too far for either to strike'))
        options = self.options(obs, s)
        questions = {'intent': Choice('What should Locke do next in this fight against `target`?',
                                      {i: INTENTS[i] for i in options})}
        for intent, opts in options.items():
            if len(opts) > 1:
                questions[intent] = Choice(f'If Locke chooses to {intent}, which option is best right now '
                                           f'against `target`?', opts)
        answers = self.jev.ask(self.state(obs, s), tag='combat', **questions)
        intent = answers['intent']
        if intent.confidence < self.confidence_floor and intent.margin < 0.15:
            return self._chose(self.rule(obs, s, options))
        pick = answers[intent.choice].choice if intent.choice in answers else next(iter(options[intent.choice]))
        return self._chose(Tactic(pick, 'jev', intent.confidence))

    def rule(self, obs: dict, s: Situation, options: dict[str, dict[str, str]]) -> Tactic:
        """When Jev is unsure: the textbook answer."""
        if 'heal' in options and health_word(obs['player'].get('health')) in ('badly hurt', 'near death'):
            return Tactic(next(iter(options['heal'])), 'rule: heal when badly hurt')
        if s.in_its_reach and s.phase.word in ('about to strike', 'striking'):
            return Tactic('block', 'rule: block a blow about to land')
        if 'attack' in options:
            name = 'attack:2' if 'attack:2' in options['attack'] else next(iter(options['attack']))
            return Tactic(name, 'rule: strike when in reach')
        return Tactic('close_in', 'rule: close the distance')

    def learn(self, obs: dict) -> None:
        """After a grapple press: did Locke go into one of its moves? If
        not, the game refused it against that kind of monster."""
        if self.pending:
            kind, button, moves = self.pending
            if (obs['player'].get('action_name') or '').lower() not in moves:
                self.refused.add((kind, button))
            self.pending = None

    def _chose(self, tactic: Tactic) -> Tactic:
        self.history.append(tactic.name)
        return tactic

    # -- doing --
    def direction(self, vector: tuple[float, float]) -> str:
        """The direction control whose step is closest to `vector`: a world
        vector against the measured world steps, or, uncalibrated, a screen
        vector (y down) against each control stepping the way its name says."""
        steps = self.steps or {name: (math.cos(math.radians(45 * i)), -math.sin(math.radians(45 * i)))
                               for i, name in enumerate(DIRECTIONS)}
        length = math.hypot(*vector) or 1.0
        unit = (vector[0] / length, vector[1] / length)
        return max(steps, key=lambda name: (steps[name][0] * unit[0] + steps[name][1] * unit[1])
                   / (math.hypot(*steps[name]) or 1.0))

    def plan(self, tactic: Tactic, obs: dict, plan: Plan) -> int:
        """Add the tactic's input to `plan` from tick 0; returns how many
        ticks to run before deciding again."""
        s = self.assess(obs)
        if self.steps:                                       # world vectors against the measured steps
            here, there = obs['player']['pos'], (s.target['pos'] if s else obs['player']['pos'])
        else:                                                # screen vectors against the names
            here = obs['player'].get('screen') or [320, 240]
            there = (s.target.get('screen') if s else None) or here
        toward = (there[0] - here[0], there[1] - here[1])
        away = (-toward[0], -toward[1])
        left = (toward[1], -toward[0])                       # a quarter turn; circling either way
        right = (-left[0], -left[1])
        kind, _, arg = tactic.name.partition(':')
        if kind == 'attack':
            v = s.variant(int(arg))
            plan.press(0, self.bindings[v.control], hold=2)
            self.presses.append((s.target['id'], v.button))
            return max(6, v.impacttime + 2)
        if kind == 'grapple':
            mask = (obs['player'].get('weapon') or {}).get('mask', 0) or WM_HAND
            moves = self.kb.locke_grapples(mask)[int(arg)]
            plan.press(0, self.bindings[moves[0].control], hold=2)
            self.presses.append((s.target['id'], int(arg)))
            self.pending = (s.target.get('type') or s.target.get('name', ''), int(arg),
                            frozenset(m.name.lower() for m in moves))
            return 6
        if kind == 'cast':
            plan.press(0, self.bindings[f'Invoke{arg}'], hold=2)
            return 12
        if kind == 'potion':
            plan.press(0, self.bindings[f'BeltUse{arg}'], hold=2)
            return 6
        if kind == 'block':
            plan.hold(0, self.bindings['CombatBlock'])
            plan.release(8, self.bindings['CombatBlock'])
            return 9
        if kind.startswith('dodge_'):
            vector = {'dodge_back': away, 'dodge_left': left, 'dodge_right': right}[kind]
            plan.press(0, self.bindings['Dodge' + self.direction(vector)], hold=2)
            return 8
        stride = 12 if s and s.reach == 'far' else 4             # a long walk in while nothing can strike
        vector, hold = {'orbit_left': (left, 6), 'orbit_right': (right, 6),
                        'close_in': (toward, stride), 'back_off': (away, 4)}[kind]
        keys = self.bindings[self.direction(vector)]
        plan.hold(0, keys)
        plan.release(hold, keys)
        return hold + 1
