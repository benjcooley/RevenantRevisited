import os
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import combat  # noqa: E402
from combat import Tactic, Tactician, action_phase  # noqa: E402
from inputs import Bindings, Plan, chord  # noqa: E402
from jev import Jev  # noqa: E402
from knowledge import Attack, Character, Knowledge, Spell  # noqa: E402

KEYS_8 = ('RIGHT', 'PGUP', 'UP', 'HOME', 'LEFT', 'END', 'DOWN', 'PGDN')
KEYS = {'CombatSwing': 'A', 'CombatThrust': 'S', 'CombatChop': 'D', 'CombatCombo7': 'Q',
        'CombatBlock': 'F', 'Invoke1': 'F1', 'BeltUse2': 'F6', 'CombatCombo1': 'CTRL-A',
        **dict(zip(combat.DIRECTIONS, KEYS_8)),
        **{'Dodge' + name: 'CTRL-' + key for name, key in zip(combat.DIRECTIONS, KEYS_8)}}


def attack(name, button, band=(19, 80), impact=4, block=3, skill=0, flags=0, mask=0x1f):
    return Attack(name, flags, button, block, impact, 10, band[0], band[1], 45, 0, 10, skill, mask, 0)


def kb() -> Knowledge:
    # Bands as char.def has them: close, normal, lunge, and a catch-all.
    locke = Character('Locke', attacks=[
        attack('elbow', 1, (0, 20), impact=3), attack('thrust', 1), attack('swing', 2), attack('chop', 3, impact=5),
        attack('chargethrust', 1, (79, 300), impact=14), attack('spinswing', 2, (79, 300), impact=13),
        attack('anychop', 3, (0, 10000)), attack('comboa1', 10, (0, 0), skill=9), attack('comboa2', 10, (0, 0), skill=9),
        attack('finisher', 4, (0, 40), flags=0x1),
        attack('aikidothrower', 4, (0, 10000), flags=0x2200081, mask=0x1f)])
    ogrok = Character('Ogrok', damagemods={'burn': 0, 'freeze': 25, 'cut': -10},
                      attacks=[attack('atkchop', 3, (0, 90), impact=10, block=4), attack('throw', 4, (0, 10000), flags=0x1)])
    return Knowledge({'Locke': locke, 'Ogrok': ogrok},
                     {'Ice Bolt': Spell('Ice Bolt', 'freeze', ('Ocean', 'Sky'), 30, (40, 50), 1, '')})


def observation(enemy_action='combat', enemy_tick=0, d=60, health=(80, 100), mana=(50, 60), skill=10):
    return {'t': 100, 'screen': 'play', 'control': True,
            'player': {'pos': [1000, 1000, 0], 'screen': [320, 240], 'health': list(health), 'mana': list(mana),
                       'fatigue': [70, 80], 'mode': 'combat', 'action_name': 'combat', 'action_tick': 5,
                       'target': 7, 'attacklevel': skill, 'skills': {'sword': [3, 0, 100]},
                       'weapon': {'name': 'Sword', 'mask': 4, 'skill': 'sword'},
                       'quick_spells': ['Ice Bolt', None, None, None],
                       'belt': [None, 'Healing Potion', None, None, None]},
            'objects': [{'id': 7, 'name': 'Ogrok1', 'type': 'Ogrok', 'class': 'character', 'enemy': True,
                         'pos': [1000 + d, 1000, 0], 'screen': [380, 240], 'health': [40, 50],
                         'action_name': enemy_action, 'action_tick': enemy_tick}]}


class Answers:
    """A Decisions endpoint stub: the intent and a pick per intent."""

    def __init__(self, intent, confidence=0.8, picks=None):
        self.intent, self.confidence, self.picks, self.bodies = intent, confidence, picks or {}, []

    def __call__(self, url, body, headers):
        self.bodies.append(body)
        answers = {}
        for name, question in body['questions'].items():
            options = list(question['criteria'])
            choice = self.intent if name == 'intent' else self.picks.get(name, options[0])
            answers[name] = {'type': 'choice', 'choice': choice, 'confidence': self.confidence,
                             'probabilities': {o: (self.confidence if o == choice else 0.0) for o in options}}
        return {'answers': answers, 'usage': {'cost': 0}}


def tactician(post=None) -> Tactician:
    os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')
    return Tactician(Jev(post=post or Answers('attack')), kb(), Bindings({k: chord(v) for k, v in KEYS.items()}))


class Phases(unittest.TestCase):
    def test_attack_timing(self):
        k = kb()
        self.assertEqual(action_phase(k, 'Ogrok', 'atkchop', 2).word, 'winding up an attack')
        about = action_phase(k, 'Ogrok', 'atkchop', 5)
        self.assertEqual((about.word, about.ticks_to_impact), ('about to strike', 5))
        self.assertEqual(action_phase(k, 'Ogrok', 'atkchop', 11).word, 'striking')
        self.assertEqual(action_phase(k, 'Ogrok', 'atkchop', 20).word, 'recovering after an attack')
        self.assertEqual(action_phase(k, 'Ogrok', 'combat block', 0).word, 'blocking')
        self.assertEqual(action_phase(k, 'Ogrok', None, None).word, 'unknown')


class Situations(unittest.TestCase):
    def test_reach_words_follow_the_bands(self):
        t = tactician()
        for d, reach, theirs in ((10, 'close', True), (60, 'close', True), (150, 'a lunge away', False),
                                 (85, 'a lunge away', True), (600, 'far', False)):
            with self.subTest(d=d):
                s = t.assess(observation(d=d))
                self.assertEqual((s.reach, s.in_its_reach), (reach, theirs))

    def test_the_variant_at_a_distance(self):
        t = tactician()
        self.assertEqual(t.assess(observation(d=10)).variant(1).name, 'elbow')
        self.assertEqual(t.assess(observation(d=60)).variant(1).name, 'thrust')
        self.assertEqual(t.assess(observation(d=200)).variant(1).name, 'chargethrust')
        self.assertEqual(t.assess(observation(d=60)).variant(10).name, 'comboa1')

    def test_options_follow_reach_skill_mana_and_belt(self):
        t = tactician()
        close = t.options(observation(), t.assess(observation()))
        self.assertEqual(set(close['attack']), {'attack:1', 'attack:2', 'attack:3', 'attack:10'})
        self.assertIn('2 blows', close['attack']['attack:10'])
        self.assertIn('S: a medium blow', close['attack']['attack:2'])
        self.assertEqual(set(close['magic']), {'cast:1'})
        self.assertEqual(set(close['heal']), {'potion:2'})
        lunge = t.options(observation(d=200), t.assess(observation(d=200)))
        self.assertEqual(set(lunge['attack']), {'attack:1', 'attack:2', 'attack:3'})   # no combos at range
        self.assertIn('charging lunge', lunge['attack']['attack:1'])
        far = observation(d=600, mana=(10, 60), skill=0)
        self.assertEqual(set(t.options(far, t.assess(far))), {'defend', 'reposition', 'heal'})

    def test_state_is_words(self):
        t = tactician()
        obs = observation(enemy_action='atkchop', enemy_tick=6)
        state = t.state(obs, t.assess(obs))
        self.assertEqual(state['target']['doing'], 'about to strike')
        self.assertEqual(state['target']['distance'], 'close: both can strike')
        self.assertEqual(state['target']['takes_extra_damage_from'], ['cut'])    # cut -10: 110%
        self.assertEqual(state['target']['resists'], ['freeze'])                 # freeze 25: 75%
        self.assertEqual(state['locke']['health'], 'healthy')


class Retail_rules(unittest.TestCase):
    def test_a_button_pressed_twice_running_is_not_offered_a_third_time(self):
        t = tactician()
        obs = observation()
        plan = Plan()
        for _ in range(2):
            t.plan(Tactic('attack:2', 'test'), obs, plan)
        options = t.options(obs, t.assess(obs))
        self.assertNotIn('attack:2', options['attack'])
        self.assertIn('attack:1', options['attack'])
        t.plan(Tactic('attack:1', 'test'), obs, plan)                       # another button breaks the run
        self.assertIn('attack:2', t.options(obs, t.assess(obs))['attack'])

    def test_a_grapple_refused_against_a_kind_is_not_offered_again(self):
        t = tactician()
        obs = observation(d=30)
        self.assertIn('grapple:4', t.options(obs, t.assess(obs))['attack'])
        t.plan(Tactic('grapple:4', 'test'), obs, Plan())
        t.learn(observation(d=30))                                           # Locke didn't go into a throw
        self.assertNotIn('grapple:4', t.options(obs, t.assess(obs))['attack'])

    def test_a_dead_target_is_left_for_the_nearest_living_enemy(self):
        t = tactician()
        obs = observation()
        obs['objects'][0]['health'] = [0, 50]
        obs['objects'].append(dict(obs['objects'][0], id=8, health=[30, 50], pos=[1100, 1000, 0]))
        self.assertEqual(t.target_of(obs)['id'], 8)

    def test_a_tired_locke_isnt_offered_blows_he_cant_pay_for(self):
        t = tactician()
        obs = observation()
        obs['player']['fatigue'] = [5, 80]                                 # the fixture's blows cost 10
        s = t.assess(obs)
        self.assertNotIn('attack', t.options(obs, s))
        self.assertEqual(t.rule(obs, s, t.options(obs, s)).name, 'back_off')

    def test_a_grapple_needs_contact(self):
        t = tactician()
        obs = observation(d=60)
        self.assertNotIn('grapple:4', t.options(obs, t.assess(obs))['attack'])


class Deciding(unittest.TestCase):
    def test_fan_out_takes_the_chosen_intents_pick(self):
        post = Answers('defend', picks={'defend': 'dodge_back', 'attack': 'attack:3'})
        t = tactician(post)
        tactic = t.decide(observation())
        self.assertEqual((tactic.name, tactic.reason), ('dodge_back', 'jev'))
        self.assertEqual(set(post.bodies[0]['questions']), {'intent', 'attack', 'defend', 'reposition'})
        self.assertEqual(t.history, ['dodge_back'])

    def test_unsure_falls_back_to_the_rule(self):
        tactic = tactician(Answers('attack', confidence=0.1)).decide(observation('atkchop', 7))
        self.assertEqual(tactic.name, 'block')
        self.assertTrue(tactic.reason.startswith('rule'))
        hurt = tactician(Answers('attack', confidence=0.1)).decide(observation(health=(15, 100)))
        self.assertEqual(hurt.name, 'potion:2')
        far = tactician(Answers('attack', confidence=0.1)).decide(observation(d=600))
        self.assertEqual((far.name, far.reason), ('close_in', 'rule: too far for either to strike'))


class Doing(unittest.TestCase):
    def test_directions_are_screen_relative(self):
        t = tactician()
        self.assertEqual(t.direction((1, 0)), 'Right')
        self.assertEqual(t.direction((0, -1)), 'Up')
        self.assertEqual(t.direction((-1, 1)), 'DownLeft')
        t.steps = {'Up': (0.7, -0.7), 'Right': (0.7, 0.7)}            # measured world steps, rotated
        self.assertEqual(t.direction((1, -1)), 'Up')

    def test_calibrated_steering_uses_world_positions(self):
        t = tactician()
        t.steps = {'Up': (0, -9), 'Down': (0, 9), 'Left': (-9, 0), 'Right': (9, 0)}
        obs = observation()
        obs['objects'][0]['pos'] = [1000, 1300, 0]                   # below Locke in the world...
        obs['objects'][0]['screen'] = None                            # ...and off the screen
        plan = Plan()
        t.plan(Tactic('close_in', 'test'), obs, plan)
        self.assertEqual(plan.events()[0]['key'], 'DOWN')

    def test_plans(self):
        t = tactician()
        obs = observation()                                          # the target is to Locke's screen right
        for name, expected, ticks in (
                ('attack:2', [('S', True, 0), ('S', False, 2)], 6),
                ('attack:3', [('D', True, 0), ('D', False, 2)], 7),
                ('block', [('F', True, 0), ('F', False, 8)], 9),
                ('dodge_back', [('CONTROL', True, 0), ('LEFT', True, 0), ('LEFT', False, 2), ('CONTROL', False, 2)], 8),
                ('orbit_left', [('UP', True, 0), ('UP', False, 6)], 7),
                ('close_in', [('RIGHT', True, 0), ('RIGHT', False, 4)], 5),
                ('cast:1', [('F1', True, 0), ('F1', False, 2)], 12)):
            with self.subTest(name):
                plan = Plan()
                run = t.plan(Tactic(name, 'test'), obs, plan)
                self.assertEqual([(e['key'], e['down'], e['t']) for e in plan.events()], expected)
                self.assertEqual(run, ticks)


if __name__ == '__main__':
    unittest.main()
