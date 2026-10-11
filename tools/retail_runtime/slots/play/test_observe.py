#!/usr/bin/env python3
"""The observation against the running game: each field checked by doing
what the director does with it, or against retail's own reads.

From the port's `opening` waypoint (Locke in the resurrection chamber, a
Short Sword in his pack and nothing in hand; tools/playtest/waypoints.py):
the first case boots the software machine if it isn't cached (about 2
minutes), starts the slot once (seconds) and keeps an image each case
loads (about a second).

    "$PY" -m unittest discover -s tools/retail_runtime/slots/play -p 'test_observe.py'
"""
import json
import random
import struct
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_ESI, UC_X86_REG_ESP  # noqa: E402
from build_contract import BASELINE  # noqa: E402
from playworld import PlayWorld  # noqa: E402
import observe  # noqa: E402

SETTLE = 24                  # ticks after the start: the screen faded in, the scene quiet
PLAY_BRANCH = 0x0044f84f     # TMapPane::MouseClick past the side tabs and the editor; button at [esp + 0xe8]
WALK_RADIUS_CALL = 0x004c3a51   # FindClearPath's GetWalkHeightRadius call: esi the character, [esp + 8] the radius
FIND_SECTOR = 0x00499e10     # cdecl (level, sector x, sector y)
LEFT, RIGHT = 1, 2
VK_UP, VK_ESCAPE = 0x26, 0x1b
WAYPOINTS = Path.home() / 'RevenantRevisited-testsaves' / 'waypoints'   # tools/playtest/waypoints.py ROOT


def opening_slot():
    """The newest port waypoint named "opening" that isn't retired: its
    manifests read directly, so the case doesn't hang on the catalogue
    code's version (tools/playtest/waypoints.py)."""
    found = []
    for manifest in WAYPOINTS.glob('port/*/waypoint.json'):
        data = json.loads(manifest.read_text())
        if data.get('name') == 'opening' and not data.get('retired'):
            found.append((data['created'], manifest.parent / 'slot' / data['slot']))
    return max(found)[1] if found else None


class ObservationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        slot = opening_slot()
        if slot is None:
            raise unittest.SkipTest('no port waypoint "opening" (tools/playtest/waypoints.py)')
        cls.temp = tempfile.TemporaryDirectory()
        cls.image = Path(cls.temp.name) / 'opening.image'
        world = PlayWorld.boot(BASELINE)
        world.overlay.place_slot(slot, slot.name)
        world.start(slot=slot.name, seed=1)
        for _ in range(SETTLE):
            world.step()
        world.save(cls.image)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def setUp(self):
        self.world = PlayWorld.load(BASELINE, self.image)
        self.observer = observe.Observer(self.world)

    def click(self, point, button):
        """A press and a release at `point`, a tick each, the pointer there first."""
        for buttons in (0, button, 0):
            self.world.step([{'mouse': list(point), 'buttons': buttons}])

    # ---- pack, equipped, radius -----------------------------------------

    def test_right_click_on_the_reported_cell_equips_the_sword(self):
        player = self.observer.observe()['player']
        sword = next(item for item in player['pack'] if item['name'] == 'Short Sword')
        self.assertEqual((sword['class'], sword['eqslot']), ('weapon', 4))
        self.assertIsNotNone(sword['screen'])
        self.assertNotIn('4', player['equipped'])
        self.assertIsNone(player['weapon']['name'])
        talisman = next(item for item in player['pack'] if item['class'] == 'talisman')
        self.assertEqual(talisman['eqslot'], -1)                  # no EqSlot stat
        self.click(sword['screen'], RIGHT)
        for _ in range(4):
            self.world.step()
        after = self.observer.observe()['player']
        self.assertEqual(after['equipped'].get('4'), 'Short Sword')
        self.assertEqual(after['weapon']['name'], 'Short Sword')
        self.assertNotIn('Short Sword', [item['name'] for item in after['pack']])

    def test_a_modal_takes_the_cells_and_the_world(self):
        self.world.step([{'key': VK_ESCAPE, 'down': True}])
        self.world.step([{'key': VK_ESCAPE, 'down': False}])
        for _ in range(SETTLE):
            self.world.step()
        obs = self.observer.observe()
        self.assertEqual(obs['screen'], 'menu')
        self.assertTrue(obs['player']['pack'])
        self.assertTrue(all(item['screen'] is None for item in obs['player']['pack']))
        route = self.observer.mouse_route()
        self.assertFalse(any(self.observer.world_click(x, y, route) for x, y in ((300, 200), (30, 30))))

    def test_radius_is_what_retail_tests_lockes_steps_over(self):
        radius = self.observer.observe()['player']['radius']
        player = self.observer.player()
        passed = []

        def walk_radius(uc, address, size, user):
            if uc.reg_read(UC_X86_REG_ESI) == player:
                passed.append(self.world.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 8))
        self.world.vm.uc.hook_add(UC_HOOK_CODE, walk_radius, begin=WALK_RADIUS_CALL, end=WALK_RADIUS_CALL)
        self.world.step([{'key': VK_UP, 'down': True}])
        for _ in range(8):
            self.world.step()
        self.world.step([{'key': VK_UP, 'down': False}])
        self.assertTrue(passed)
        self.assertEqual(set(passed), {radius})

    # ---- walk grid ------------------------------------------------------

    def test_walkgrid_reads_each_cell_as_get_walk_height(self):
        world, observer = self.world, self.observer
        obs = observer.observe(walkgrid=480)
        grid, (x, y, z) = obs['walkgrid'], obs['player']['pos']
        ox, oy, w = grid['origin'][0], grid['origin'][1], grid['w']
        self.assertEqual((grid['cell'], grid['h'], len(grid['heights'])), (16, w, w * w))

        def height(gx, gy):
            return grid['heights'][(gy - oy) * w + gx - ox]
        self.assertEqual(height((x + 8) >> 4, (y + 8) >> 4), z)              # floor under Locke
        east = [height(gx, (y + 8) >> 4) for gx in range((x + 8) >> 4, ox + w)]
        self.assertGreater(east.index(0), 0)                                  # the chamber's floor ends east
        self.assertNotIn(-1, east[:east.index(0)])                            # in a wall, not the unknown
        level = obs['player']['level']
        cells = [(ox + i % w, oy + i // w) for i in range(w * w)]
        rng = random.Random(1)
        sample = [c for c in cells if height(*c) > 0] + rng.sample(cells, 200)
        for gx, gy in sample:
            world.vm.write(world.scratch, struct.pack('<3i', gx * 16, gy * 16, 0))
            retail = world.peek(observe.WALK_HEIGHT, (world.scratch, level, 0))
            self.assertEqual(retail, max(height(gx, gy), 0), (gx, gy))
            known = world.peek(FIND_SECTOR, (level, gx >> 6, gy >> 6)) != 0
            self.assertEqual(known, height(gx, gy) != -1, (gx, gy))

    # ---- the mouse ------------------------------------------------------

    def test_world_is_where_retails_routing_takes_a_left_press(self):
        world, observer = self.world, self.observer
        pressed = []

        def play_branch(uc, address, size, user):
            pressed.append(world.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 0xe8))
        world.vm.uc.hook_add(UC_HOOK_CODE, play_branch, begin=PLAY_BRANCH, end=PLAY_BRANCH)
        # open floor, the status bar over the map, a side tab, the map's corner,
        # the side panel, the bottom bar, the bottom side tab
        points = [(300, 200), (30, 30), (425, 200), (451, 419), (500, 100), (100, 450), (420, 400)]
        expected = [True, True, False, True, False, False, False]
        for point, reaches in zip(points, expected):
            predicted = observer.world_click(*point, observer.mouse_route())
            world.step([{'mouse': list(point), 'buttons': 0}])
            pressed.clear()
            world.step([{'mouse': list(point), 'buttons': LEFT}])
            world.step([{'mouse': list(point), 'buttons': 0}])
            self.assertEqual((predicted, LEFT in pressed), (reaches, reaches), point)


if __name__ == '__main__':
    unittest.main()
