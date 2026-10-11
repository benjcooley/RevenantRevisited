import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from nav import LevelMaps, farthest_clear, plan_path  # noqa: E402
from walk import WalkGrid  # noqa: E402
from world import Exit, Place, World, parse_exit_list  # noqa: E402

DATA = Path(__file__).resolve().parents[3] / 'data'


def grid(rows: list[str]) -> WalkGrid:
    """'.' floor at 10, '#' no floor, '^' a ledge at 100."""
    heights = [{'.': 10, '#': 0, '^': 100}[c] for row in rows for c in row]
    return WalkGrid({'origin': [0, 0], 'cell': 16, 'w': len(rows[0]), 'h': len(rows), 'heights': heights})


ROOM = [
    '...........',
    '...........',
    '...........',
    '#####...###',
    '...........',
    '...........',
    '...........',
]


class Paths(unittest.TestCase):
    def test_around_a_wall_through_its_gap(self):
        g = grid(ROOM)
        path = plan_path(g, (8, 8), (8, 104))                      # cell (0,0) to cell (0,6)
        self.assertEqual(path[0], (8, 8))
        self.assertEqual(path[-1], (8, 104))
        self.assertIn((104, 56), path)                              # through the gap's middle, (6, 3), clear of its sides
        self.assertNotIn((88, 56), path)                            # (5, 3) touches the wall

    def test_the_next_click_is_the_farthest_point_in_sight(self):
        g = grid(ROOM)
        path = plan_path(g, (8, 8), (8, 104))
        point = farthest_clear(g, (8, 8), path)
        self.assertTrue(g.open_line((8, 8), point))
        self.assertGreater(path.index(point), 1)
        self.assertLess(path.index(point), len(path) - 1)           # the wall hides the end

    def test_a_ledge_blocks_and_an_unreachable_target_gets_the_nearest(self):
        g = grid(['.........', '....^....', '.........', '....^....', '.........'])
        path = plan_path(g, (8, 40), (136, 40))
        self.assertTrue(path[-1][0] < 72 or path[-1] == (136, 40))   # around or short of the ledge, never over it
        self.assertNotIn((72, 24), path)
        self.assertEqual(plan_path(g, (72, 8), (8, 8)), [(72, 8)])  # on the ledge's top: every step down is too far
        self.assertEqual(plan_path(grid(['#.']), (8, 8), (24, 8)), [])  # standing on no floor the grid knows

    def test_target_outside_the_grid(self):
        g = grid(ROOM)
        path = plan_path(g, (24, 24), (2000, 24))
        self.assertEqual(path[-1][1], 24)
        self.assertGreaterEqual(path[-1][0], 136)                   # toward the edge nearest the target


class Maps(unittest.TestCase):
    def test_merging_keeps_floor_and_grows(self):
        maps = LevelMaps()
        maps.merge(2, WalkGrid({'origin': [0, 0], 'cell': 16, 'w': 3, 'h': 1, 'heights': [10, 10, 10]}))
        maps.merge(2, WalkGrid({'origin': [2, 0], 'cell': 16, 'w': 3, 'h': 1, 'heights': [0, 10, 10]}))
        self.assertEqual(maps.heights[2], {(0, 0): 10, (1, 0): 10, (2, 0): 10, (3, 0): 10, (4, 0): 10})

    def test_a_dead_end_turns_the_search_elsewhere(self):
        # two pockets near the target: the nearer one is where Locke got stuck
        rows = ['............',
                '............',
                '............',
                '............']
        maps = LevelMaps()
        maps.merge(0, grid(rows))
        target = (600, 40)                                          # beyond the grid's right edge
        first = maps.plan(0, (24, 24), target)
        self.assertGreaterEqual(first[-1][0], 152)
        maps.dead_end(0, first[-1])
        second = maps.plan(0, (24, 24), target)
        self.assertGreaterEqual(abs(second[-1][0] - first[-1][0]) + abs(second[-1][1] - first[-1][1]), 100)


class Routes(unittest.TestCase):
    def test_exit_list_lines(self):
        text = ('//Keep\n\njailenter (6280, 4338, 33) LEVEL 2 MAPINDEX 0x036a03f6 ambient 10 (255, 255, 255)\n'
                'LabGateBin (9952, 9928, 576) level 53 mapindex 0x35249020 ambient 30 (255, 0, 0)\n')
        self.assertEqual(parse_exit_list(text), {'jailenter': (2, (6280, 4338, 33)), 'labgatebin': (53, (9952, 9928, 576))})

    def test_route_through_two_exits(self):
        world = World.__new__(World)
        world.exits = lambda level: {
            2: (Exit('ressexit', 2, (1190, 1120, 6), 2, (9992, 8966, 576)),
                Exit('jailenter', 2, (12793, 11345, 160), 2, (6280, 4338, 33)),
                Exit('jailexit', 2, (6258, 4434, 32), 2, (12810, 11392, 161)))}.get(level, ())
        rand = Place(2, (5760, 4528, 44), 'RandK', 'Rand', 'CHARACTER')
        self.assertEqual([e.name for e in world.route(2, (1342, 578, 17), rand)], ['ressexit', 'jailenter'])
        self.assertEqual(world.route(2, (6000, 4400, 33), rand), [])              # already there: walk
        self.assertIsNone(world.route(2, (0, 0, 0), Place(9, (0, 0, 0), 'X', 'X', 'ITEM')))

    @unittest.skipUnless((DATA / 'Modules' / 'Ahkuilon.rvm').exists(), 'no game data')
    def test_the_keep_route_to_rand(self):
        world = World(DATA)
        rand = world.find('RandK')[0]
        self.assertEqual(rand.level, 2)
        self.assertEqual([e.name for e in world.route(2, (1342, 578, 17), rand)], ['ressexit', 'jailenter'])


if __name__ == '__main__':
    unittest.main()
