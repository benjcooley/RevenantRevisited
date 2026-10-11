import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from nav import LevelMaps, farthest_clear, footprint, plan_path, search  # noqa: E402
from walk import UNKNOWN, WalkGrid, cell_of  # noqa: E402
from world import Exit, Place, World, parse_exit_list  # noqa: E402

DATA = Path(__file__).resolve().parents[3] / 'data'


def grid(rows: list[str]) -> WalkGrid:
    """'.' floor at 10, '#' no floor, '^' a ledge at 100, '?' unknown; cell
    (x, y) is centred on (16x, 16y), as the game's walk cells are."""
    heights = [{'.': 10, '#': 0, '^': 100, '?': UNKNOWN}[c] for row in rows for c in row]
    return WalkGrid({'origin': [0, 0], 'cell': 16, 'w': len(rows[0]), 'h': len(rows), 'heights': heights})


ROOM = [
    '###############',
    '#.............#',
    '#.............#',
    '#.............#',
    '#.............#',
    '#######...#####',
    '#.............#',
    '#.............#',
    '#.............#',
    '#.............#',
    '###############',
]


class Cells(unittest.TestCase):
    def test_cells_are_centred_like_the_game(self):
        self.assertEqual([cell_of(x) for x in (-8, 7, 8, 23, 24)], [0, 0, 1, 1, 2])

    def test_footprint_mirrors_get_walk_height_radius(self):
        g = grid(ROOM)
        height = lambda c: g.cell_height(*c)
        under, step, hole = footprint(height, 7 * 16, 2 * 16, 24)        # mid-room
        self.assertEqual((under, step, hole), (10, 0, False))
        under, step, hole = footprint(height, 1 * 16, 1 * 16, 24)        # in a corner: walls within reach
        self.assertTrue(hole)


class Paths(unittest.TestCase):
    def test_through_the_doorway_with_room_to_spare(self):
        g = grid(ROOM)
        path = plan_path(g, (3 * 16, 2 * 16), (3 * 16, 8 * 16), radius=12)
        self.assertEqual(path[0], (48, 32))
        self.assertEqual(path[-1], (48, 128))
        self.assertIn((128, 80), path)                                   # the doorway's middle cell (8, 5)

    def test_a_radius_too_wide_for_the_doorway(self):
        g = grid(ROOM)
        path = plan_path(g, (3 * 16, 2 * 16), (3 * 16, 8 * 16), radius=40)
        self.assertLess(path[-1][1], 80)                                 # stays on the near side

    def test_the_next_click_is_the_farthest_point_in_sight(self):
        g = grid(ROOM)
        path = plan_path(g, (3 * 16, 2 * 16), (3 * 16, 8 * 16), radius=12)
        point = farthest_clear(g, (48, 32), path)
        self.assertTrue(g.open_line((48, 32), point))
        self.assertLess(path.index(point), len(path) - 1)                # the wall hides the end

    def test_a_ledge_blocks(self):
        g = grid(['..........', '..........', '....^.....', '....^.....', '....^.....',
                  '....^.....', '..........', '..........'])
        path = plan_path(g, (16, 64), (144, 64), radius=8)
        self.assertNotIn((64, 64), path)
        self.assertEqual(path[-1], (144, 64))                            # around it


class Exploring(unittest.TestCase):
    def test_an_unreachable_target_draws_the_path_to_the_frontier(self):
        g = grid(['?????????',
                  '?.......?',
                  '?.......?',
                  '?...#...?',
                  '?.......?',
                  '#########'])
        path = search(lambda c: g.cell_height(*c), (2 * 16, 2 * 16), (40 * 16, 2 * 16), radius=4)
        end = (path[-1][0] // 16, path[-1][1] // 16)
        self.assertEqual(end[0], 7)                                      # the edge beside the unknown, toward the target

    def test_merging_keeps_known_cells_and_skips_unknown(self):
        maps = LevelMaps()
        maps.merge(2, WalkGrid({'origin': [0, 0], 'cell': 16, 'w': 3, 'h': 1, 'heights': [10, 0, UNKNOWN]}))
        maps.merge(2, WalkGrid({'origin': [2, 0], 'cell': 16, 'w': 2, 'h': 1, 'heights': [10, UNKNOWN]}))
        self.assertEqual(maps.heights[2], {(0, 0): 10, (1, 0): 0, (2, 0): 10})

    def test_a_dead_end_turns_the_search_elsewhere(self):
        rows = ['????????????????',
                '?..............?',
                '?..............?',
                '?..............?',
                '?..............?',
                '????????????????']
        maps = LevelMaps()
        maps.merge(0, grid(rows))
        first = maps.plan(0, (2 * 16, 2 * 16), (40 * 16, 3 * 16), radius=4)
        maps.dead_end(0, first[-1])
        second = maps.plan(0, (2 * 16, 2 * 16), (40 * 16, 3 * 16), radius=4)
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
        self.assertEqual(world.route(2, (6000, 4400, 33), rand), [])
        self.assertIsNone(world.route(2, (0, 0, 0), Place(9, (0, 0, 0), 'X', 'X', 'ITEM')))

    @unittest.skipUnless((DATA / 'Modules' / 'Ahkuilon.rvm').exists(), 'no game data')
    def test_the_keep_route_to_rand(self):
        world = World(DATA)
        rand = world.find('RandK')[0]
        self.assertEqual(rand.level, 2)
        self.assertEqual([e.name for e in world.route(2, (1342, 578, 17), rand)], ['ressexit', 'jailenter'])


if __name__ == '__main__':
    unittest.main()
