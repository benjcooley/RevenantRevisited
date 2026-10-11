import os
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(HERE))
from host import Host  # noqa: E402
from inputs import Bindings, InputState, Plan, chord  # noqa: E402
from jev import Jev  # noqa: E402
from walk import Memory, WalkGrid, Walker, screen_direction  # noqa: E402

BINDINGS = Bindings({'Run': chord('R')})


class Picks:
    """A Decisions stub that picks a given option, else the first."""

    def __init__(self, prefer=None):
        self.prefer, self.bodies = prefer, []

    def __call__(self, url, body, headers):
        self.bodies.append(body)
        options = list(body['questions']['where']['criteria'])
        choice = next((o for o in options if self.prefer and o.startswith(self.prefer)), options[0])
        return {'answers': {'where': {'type': 'choice', 'choice': choice, 'confidence': 0.9,
                                      'probabilities': {o: float(o == choice) for o in options}}}}


def walker(prefer=None) -> Walker:
    os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')
    return Walker(Jev(post=Picks(prefer)), BINDINGS)


class Pieces(unittest.TestCase):
    def test_screen_directions(self):
        self.assertEqual(screen_direction(1, 0), 'right')
        self.assertEqual(screen_direction(0, -5), 'up')
        self.assertEqual(screen_direction(-3, 3), 'down and left')

    def test_walk_grid_lines(self):
        heights = [10] * 16
        heights[1 * 4 + 2] = 0                       # a hole at cell (2, 1)
        heights[3 * 4 + 1] = 60                      # a ledge at cell (1, 3)
        grid = WalkGrid({'origin': [0, 0], 'cell': 16, 'w': 4, 'h': 4, 'heights': heights})
        self.assertTrue(grid.open_line((8, 8), (8, 40)))
        self.assertFalse(grid.open_line((8, 24), (60, 24)))     # through the hole
        self.assertFalse(grid.open_line((8, 56), (40, 56)))     # up the ledge
        self.assertFalse(grid.open_line((8, 8), (200, 8)))      # off the grid: no floor

    def test_memory(self):
        memory = Memory()
        memory.note({'player': {'level': 2, 'pos': [600, 300, 0]}})
        self.assertTrue(memory.seen(2, (700, 400)))
        self.assertFalse(memory.seen(2, (900, 300)))
        self.assertFalse(memory.seen(1, (600, 300)))


class WithTheReferenceHost(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.host = Host([sys.executable, HERE / 'fakehost.py'], Path(self.tmp.name), timeout=30)
        self.host.hello()
        self.host.start('newgame', seed=1)                     # player at (1001, 2000)

    def tearDown(self):
        self.host.close()
        self.tmp.cleanup()

    def to_the_wall(self) -> InputState:
        plan = Plan()
        plan.hold(0, chord('RIGHT'))
        plan.release(140, chord('RIGHT'))
        self.host.step(141, plan.events())                    # x = 1141, the wall at 1150
        return plan.state

    def test_destinations_include_people_and_open_ground_only(self):
        w = walker()
        obs = self.host.observe(walkgrid=320)
        found = {d.key: d for d in w.destinations(self.host, obs, WalkGrid(obs['walkgrid']))}
        self.assertIn('obj:5', found)
        self.assertIn("person 'Sardok'", found['obj:5'].words)
        ground = [k for k in found if k.startswith('ground:')]
        self.assertEqual(len(ground), 15)
        self.assertNotIn('ground:0:far', ground)                # 160 right: past the wall at 1150
        self.assertEqual(self.host.pick(*found['obj:5'].screen)['object'], 5)
        state = self.to_the_wall()
        obs = self.host.observe(walkgrid=320)
        found = {d.key for d in w.destinations(self.host, obs, WalkGrid(obs['walkgrid']))}
        self.assertNotIn('ground:0:near', found)                # right, beyond the wall
        self.assertIn('ground:4:near', found)                   # left, open
        self.assertIsInstance(state, InputState)

    def test_choose_and_walk_to_ground(self):
        w = walker(prefer='ground:2:far')
        obs = self.host.observe(walkgrid=320)
        dest = w.choose('explore', obs, w.destinations(self.host, obs, WalkGrid(obs['walkgrid'])))
        self.assertEqual(dest.key, 'ground:2:far')
        result, obs, _ = w.walk(self.host, InputState(), dest)
        self.assertEqual(result, 'arrived')
        x, y = obs['player']['pos'][:2]
        self.assertLessEqual(abs(x - 1001) + abs(y - 1840), 8)    # within Goto's snap

    def test_walk_into_the_wall_stalls(self):
        state = self.to_the_wall()
        w = walker()
        obs = self.host.observe()
        result, obs, _ = w.walk(self.host, state, self._ground(obs, 520, 240))   # 200 units right, past the wall
        self.assertEqual(result, 'stalled')
        self.assertEqual(obs['player']['pos'][0], 1149)

    def test_walk_to_a_person_stops_within_use_reach(self):
        w = walker()
        obs = self.host.observe()
        sardok = next(d for d in w.destinations(self.host, obs, None) if d.key == 'obj:5')
        result, obs, _ = w.walk(self.host, InputState(), sardok)
        self.assertEqual(result, 'arrived')

    def _ground(self, obs, x, y):
        from walk import Destination
        picked = self.host.pick(x, y)
        return Destination('ground:test', 'test', (x, y), tuple(picked['pos']))


if __name__ == '__main__':
    unittest.main()
