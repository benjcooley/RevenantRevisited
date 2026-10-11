import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import rec  # noqa: E402
from inputs import Bindings, InputState, Plan, chord  # noqa: E402

RETAIL_INI = Path('/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/revenant.ini')

CONTROLS = """[Controls]
CombatSwing = A,JOY1
CombatCombo1 = CTRL-A,JOY8-JOY1
DodgeUpLeft = CTRL-HOME,DJOYUPLF
UpRight = PGUP,JOYUPRT
QuickSave = CTRL-BS
Sneak =
OnlyJoy = JOY3
"""


class Chords(unittest.TestCase):
    def test_control_mapper_names(self):
        self.assertEqual(chord('CTRL-BS').keys, ('CONTROL', 'BACK'))
        self.assertEqual(chord('PGDN').keys, ('NEXT',))
        self.assertEqual(chord('ALT-S').keys, ('MENU', 'S'))
        self.assertEqual(chord('control+f5').keys, ('CONTROL', 'F5'))
        with self.assertRaises(rec.FormatError):
            chord('CTRL-NOPE')

    def test_bindings_from_an_ini(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'Revenant.ini'
            path.write_text(CONTROLS, encoding='cp1252')
            bindings = Bindings.from_ini(path)
        self.assertEqual(bindings['CombatCombo1'].keys, ('CONTROL', 'A'))
        self.assertEqual(bindings['DodgeUpLeft'].keys, ('CONTROL', 'HOME'))
        self.assertEqual(bindings['UpRight'].keys, ('PRIOR',))
        self.assertNotIn('Sneak', bindings)
        self.assertNotIn('OnlyJoy', bindings)
        with self.assertRaises(KeyError):
            bindings['Sneak']

    def test_an_ini_without_controls_plays_with_the_defaults(self):
        with tempfile.TemporaryDirectory() as tmp:
            shipped, mine = Path(tmp) / 'shipped.ini', Path(tmp) / 'mine.ini'
            shipped.write_text(CONTROLS, encoding='cp1252')
            mine.write_text('[Paths]\nSavePath = .\n[Controls]\nCombatSwing = Z\n', encoding='cp1252')
            self.assertEqual(Bindings.from_ini(mine, defaults=shipped)['CombatSwing'].keys, ('Z',))
            self.assertEqual(Bindings.from_ini(mine, defaults=shipped)['QuickSave'].keys, ('CONTROL', 'BACK'))
            mine.write_text('[Paths]\nSavePath = .\n', encoding='cp1252')
            self.assertEqual(Bindings.from_ini(mine, defaults=shipped)['CombatSwing'].keys, ('A',))
            self.assertEqual(Bindings.from_ini(mine).controls, {})

    @unittest.skipUnless(RETAIL_INI.exists(), 'the retail lab install is not here')
    def test_the_shipped_bindings_resolve(self):
        bindings = Bindings.from_ini(RETAIL_INI)
        for control, keys in {'CombatMode': ('C',), 'CombatChop': ('D',), 'Run': ('R',),
                              'QuickSave': ('CONTROL', 'BACK'), 'Invoke1': ('F1',),
                              'BeltUse5': ('F9',), 'DodgeDownRight': ('CONTROL', 'NEXT'),
                              'CombatCombo7': ('Q',)}.items():
            self.assertEqual(bindings[control].keys, keys, control)


class Plans(unittest.TestCase):
    def test_press_orders_modifiers_around_the_key(self):
        plan = Plan()
        after = plan.press(3, chord('CTRL-A'), hold=2)
        self.assertEqual(after, 6)
        self.assertEqual(plan.events(), [
            {'key': 'CONTROL', 'down': True, 't': 3}, {'key': 'A', 'down': True, 't': 3},
            {'key': 'A', 'down': False, 't': 5}, {'key': 'CONTROL', 'down': False, 't': 5}])
        self.assertEqual(plan.state.held, frozenset())
        self.assertEqual(plan.ticks(), 6)

    def test_click_moves_then_presses_then_releases(self):
        plan = Plan(InputState(x=10, y=10))
        end = plan.click(0, 412, 233, hold=2)
        self.assertEqual(end, 4)
        self.assertEqual(plan.events(), [
            {'mouse': [412, 233], 'buttons': 0, 't': 0},
            {'mouse': [412, 233], 'buttons': rec.BUTTON_LEFT, 't': 1},
            {'mouse': [412, 233], 'buttons': 0, 't': 3}])
        with self.assertRaises(ValueError):
            plan.move(0, 640, 0)

    def test_typing_sends_key_char_key(self):
        plan = Plan()
        end = plan.type(0, 'Ab 1', gap=1)
        self.assertEqual(end, 4)
        events = plan.events()
        self.assertEqual(events[:5], [
            {'key': 'SHIFT', 'down': True, 't': 0}, {'key': 'A', 'down': True, 't': 0},
            {'char': 65, 't': 0}, {'key': 'A', 'down': False, 't': 0},
            {'key': 'SHIFT', 'down': False, 't': 0}])
        self.assertIn({'char': 98, 't': 1}, events)
        self.assertIn({'key': 'SPACE', 'down': True, 't': 2}, events)
        self.assertIn({'char': ord('1'), 't': 3}, events)

    def test_state_carries_held_keys_and_buttons(self):
        plan = Plan()
        plan.hold(0, chord('R'))
        plan.button(1, rec.BUTTON_RIGHT, True)
        self.assertEqual(plan.state.held, frozenset({'R'}))
        follow = Plan(plan.state)
        follow.release_all(0)
        self.assertEqual(follow.events(), [{'key': 'R', 'down': False, 't': 0}])
        self.assertEqual(follow.state.buttons, rec.BUTTON_RIGHT)

    def test_every_event_parses_as_a_recording_line(self):
        plan = Plan()
        plan.press(0, chord('CTRL-BS'))
        plan.click(2, 100, 100)
        plan.type(6, 'Save 1')
        for event in plan.events():
            rec.parse_item(event)


if __name__ == '__main__':
    unittest.main()
