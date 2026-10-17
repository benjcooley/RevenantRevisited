#!/usr/bin/env python3
"""The observation against the running game: each field checked by doing
what the director does with it, or against retail's own reads.

From the port's `opening` waypoint (Locke in the resurrection chamber, a
Short Sword in his pack and nothing in hand; tools/playtest/waypoints.py)
and its `rahul defeated` (the sword and the cloth worn, Rahul killed): each
class boots the software machine if it isn't cached (about 2 minutes),
starts its slot once (seconds) and keeps an image each case loads (about a
second).

    "$PY" -m unittest discover -s tools/retail_runtime/slots/play -p 'test_observe.py'
"""
import json
import math
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
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_ESP  # noqa: E402
from build_contract import BASELINE  # noqa: E402
from playworld import PlayWorld  # noqa: E402
import observe  # noqa: E402

SETTLE = 24                  # ticks after the start: the screen faded in, the scene quiet
RADIUS = 800                 # observe()'s object radius
PLAY_BRANCH = 0x0044f84f     # TMapPane::MouseClick past the side tabs and the editor; button at [esp + 0xe8]
WALK_RADIUS_CALL = 0x004c3a51   # FindClearPath's GetWalkHeightRadius call: esi the character, [esp + 8] the radius
FIND_SECTOR = 0x00499e10     # cdecl (level, sector x, sector y)
ON_OBJECT_PRESSED = 0xf8     # TMapPane: onobject, the map index the left press found or -1 (CW §1.1)
EQUIP_PRESS_SLOT = 0x005364c5   # TEquipPane::MouseClick, a left press on a cell: esi the slot it found
BELT_HOVER_SLOT = 0x0052ce07    # TBarInvPane::Animate: eax the belt slot id under the pointer (0x10a: none)
GET_OBJ_STAT = 0xdc          # the player's GetObjStat 0x0051ae30 (vtable +0xdc), what every stat read asks
LEFT, RIGHT = 1, 2
VK_UP, VK_ESCAPE = 0x26, 0x1b
WAYPOINTS = Path.home() / 'RevenantRevisited-testsaves' / 'waypoints'   # tools/playtest/waypoints.py ROOT
# weapon.def:4-9 and armor.def's RED CLOTH SHIRT / BROWN CLOTH PANTS (BASICMODS): what retail binds over
# class.def's rows at load (GRIND.md §5.1; class.def says Value 100, and 130 / 125, Protection 0, 16 / 12)
SHORT_SWORD = dict(type=2, damage=13, minstrength=16, value=195)
CLOTH = dict(protection=1, minstrength=14, minconstitution=10, value=10)


def waypoint_slot(name):
    """The newest port waypoint `name` that isn't retired: its manifests
    read directly, so the case doesn't hang on the catalogue code's version
    (tools/playtest/waypoints.py)."""
    found = []
    for manifest in WAYPOINTS.glob('port/*/waypoint.json'):
        data = json.loads(manifest.read_text())
        if data.get('name') == name and not data.get('retired'):
            found.append((data['created'], manifest.parent / 'slot' / data['slot']))
    return max(found)[1] if found else None


class WaypointCase(unittest.TestCase):
    """Cases on one waypoint: the slot started once, settled, and saved as
    an image each case loads."""
    WAYPOINT = None

    @classmethod
    def setUpClass(cls):
        slot = waypoint_slot(cls.WAYPOINT)
        if slot is None:
            raise unittest.SkipTest(f'no port waypoint "{cls.WAYPOINT}" (tools/playtest/waypoints.py)')
        cls.temp = tempfile.TemporaryDirectory()
        cls.image = Path(cls.temp.name) / 'waypoint.image'
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

    def drag(self, start, end):
        """The left button pressed at `start`, carried to `end` and let go
        there, a tick each step, as a player drags an item."""
        x, y = start
        path = [(start, 0), (start, LEFT), ((x - 5, y), LEFT), (((x + end[0]) // 2, (y + end[1]) // 2), LEFT),
                (end, LEFT), (end, 0)]
        for point, buttons in path:
            self.world.step([{'mouse': list(point), 'buttons': buttons}])
        for _ in range(4):
            self.world.step()

    def by_name(self, obs, name):
        return next(o for o in obs['objects'] if o['name'] == name)

    def stat(self, obj, stat):
        """A stat as retail's own GetObjStat answers it (the player: his
        modified copy)."""
        return observe.signed(self.world.peek(self.observer.m.u32(self.observer.m.u32(obj) + GET_OBJ_STAT),
                                              (stat,), this=obj))


class ObservationTests(WaypointCase):
    WAYPOINT = 'opening'

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

    # ---- objects --------------------------------------------------------

    def test_objects_leave_out_what_the_player_cannot_see(self):
        """keep.s's MUDOKON, the script holder that turns Sardok and Tendrick
        toward Locke while Rahul fights, stands in reach but invisible:
        retail's own flag lookup says so, and it isn't listed. Sardok and
        Tendrick, in view, are."""
        observer, m = self.observer, self.observer.m

        def flag_name(bit):          # retail's name for a flag bit, as a guest string
            return m.u32(observe.FLAG_NAMES + 4 * (bit.bit_length() - 1))
        self.assertEqual(m.text(flag_name(observe.OF_INVISIBLE)), 'INVISIBLE')
        self.assertEqual(m.text(flag_name(observe.OF_EDITOR)), 'EDITOR')
        player = observer.player()
        mudokon = next(o for o in observer.objects_on_level(m.u16(player + observe.OBJ_LEVEL))
                       if m.text(m.u32(o + observe.OBJ_NAME)) == 'MUDOKON')
        self.assertTrue(self.world.peek(observe.FLAG_NAMED, (flag_name(observe.OF_INVISIBLE),), this=mudokon))
        self.assertLess(math.dist(observer.pos(player)[:2], observer.pos(mudokon)[:2]), RADIUS)
        names = [o['name'] for o in observer.observe(objects=RADIUS)['objects']]
        self.assertNotIn('MUDOKON', names)
        self.assertLessEqual({'SardokR', 'TendrickR'}, set(names))

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

    def test_cursor_names_what_the_pointer_is_over(self):
        obs = self.observer.observe()
        tendrick = self.by_name(obs, 'TendrickR')
        for point, expected in ((tendrick['screen'], tendrick['id']), (obs['player']['screen'], 0),
                                ((300, 300), None), ((500, 100), None)):
            self.world.step([{'mouse': list(point), 'buttons': 0}])
            self.assertEqual(self.observer.observe()['cursor'], expected, point)

    def test_a_press_acts_on_the_cursor(self):
        obs = self.observer.observe()
        for point in (self.by_name(obs, 'TendrickR')['screen'], (300, 300)):
            world = PlayWorld.load(BASELINE, self.image)
            observer = observe.Observer(world)
            world.step([{'mouse': list(point), 'buttons': 0}])
            cursor = observer.observe()['cursor']
            world.step([{'mouse': list(point), 'buttons': LEFT}])
            pressed = world.vm.u32(observe.MAP_PANE + ON_OBJECT_PRESSED)     # the press's OnObject, by map index
            self.assertEqual(observe.signed(pressed), -1 if cursor is None else cursor, point)


    # ---- the numbers a grind needs ---------------------------------------

    def test_attributes_and_skills_are_what_retails_stat_reads_answer(self):
        player = self.observer.observe()['player']
        locke = self.observer.player()
        self.assertEqual(player['attacklevel'], self.stat(locke, 20))
        self.assertEqual(player['stats'], {name: self.stat(locke, 34 + i) for i, name in
                                           enumerate(('strn', 'cons', 'agil', 'rflx', 'mind', 'luck'))})
        self.assertEqual(list(player['skills']), list(observe.SKILL_NAMES))
        for i, name in enumerate(observe.SKILL_NAMES):
            self.assertEqual(player['skills'][name], [self.stat(locke, 40 + i), self.stat(locke, 51 + i),
                                                      self.stat(locke, 62 + i)], name)
        # the opening's `playerlevel 1`: every skill 0, 0 experience, 300 to the next (GRIND §2.1);
        # newgame.sav's level-up markers Strn, Cons and the cursor at 1 (GRIND §1.4)
        self.assertEqual(player['skills']['hands'], [0, 0, 300])
        self.assertEqual(player['levelup'], [0, 1, 1])
        self.assertIsNone(player['effect'])

    def test_items_are_every_item_with_the_numbers_retail_bound(self):
        player = self.observer.observe()['player']
        items = {item['slot']: item for item in player['items']}
        for cell in player['pack']:
            self.assertEqual((items[cell['slot']]['name'], items[cell['slot']]['eqslot']),
                             (cell['name'], cell['eqslot']))
        sword = next(item for item in player['items'] if item['name'] == 'Short Sword')
        self.assertEqual((sword['class'], sword['amount'], sword['stats']), ('weapon', 1, SHORT_SWORD))
        shirt = next(item for item in player['items'] if item['name'] == 'Red Cloth Shirt')
        self.assertEqual((shirt['eqslot'], shirt['stats']), (2, CLOTH))
        self.assertEqual(next(item for item in player['items'] if item['class'] == 'talisman')['stats'], {})

    def test_what_is_worn_sets_retails_damage_armour_and_skill(self):
        """Barehanded: Locke's WEAPONDAMAGE 12 (char.def:120) and the Hands
        skill. Equipped by right clicks on the reported cells: the sword's
        Damage is what WeaponDamage 0x00520830 answers, the skill is Sword,
        and the cloth's Protection sums to ArmorValue 0x00519850."""
        player = self.observer.observe()['player']
        self.assertEqual((player['weapon']['skill'], player['combat']['damage'], player['combat']['armor']),
                         ('hands', 12, 0))
        self.assertEqual(player['combat']['offense'], 10 * player['xp'][0])
        worn = ('Short Sword', 'Red Cloth Shirt', 'Brown Cloth Pants')
        for name in worn:
            cell = next(c for c in self.observer.observe()['player']['pack'] if c['name'] == name)
            self.click(cell['screen'], RIGHT)
            for _ in range(4):
                self.world.step()
        player = self.observer.observe()['player']
        self.assertEqual(sorted(player['equipped'].values()), sorted(worn))
        items = {item['name']: item for item in player['items']}
        self.assertEqual(player['weapon']['skill'], 'sword')
        self.assertEqual(player['combat']['damage'], items['Short Sword']['stats']['damage'])
        self.assertEqual(player['combat']['armor'], sum(items[name]['stats']['protection'] for name in worn[1:]))
        self.assertEqual(items['Short Sword']['slot'], 0x100 + 4)

    def test_equip_slots_are_the_cells_retail_hit_tests(self):
        found = []
        self.world.vm.uc.hook_add(UC_HOOK_CODE, lambda uc, *_: found.append(uc.reg_read(UC_X86_REG_ESI)),
                                  begin=EQUIP_PRESS_SLOT, end=EQUIP_PRESS_SLOT)
        cells = self.observer.observe()['equip_slots']
        self.assertEqual(len(cells), 11)
        for slot, cell in enumerate(cells):
            found.clear()
            self.click(cell, LEFT)
            self.assertEqual(found, [slot], cell)

    def test_a_drag_from_an_equip_slot_to_the_pack_unequips(self):
        sword = next(c for c in self.observer.observe()['player']['pack'] if c['name'] == 'Short Sword')
        self.click(sword['screen'], RIGHT)
        for _ in range(4):
            self.world.step()
        obs = self.observer.observe()
        self.assertEqual(obs['player']['equipped'].get('4'), 'Short Sword')
        empty = next(slot for slot in range(observe.INV_COLUMNS * observe.INV_ROWS)
                     if slot not in {c['slot'] for c in obs['player']['pack']})
        column, row = divmod(empty, observe.INV_ROWS)
        m, pane = self.observer.m, observe.INVENTORY_PANE
        cell = (m.i32(pane + observe.PANE_X) + observe.INV_LEFT + column * observe.INV_DX + observe.INV_CELL // 2,
                m.i32(pane + observe.PANE_Y) + observe.INV_TOP + row * observe.INV_DY + observe.INV_CELL // 2)
        self.drag(obs['equip_slots'][4], cell)
        player = self.observer.observe()['player']
        self.assertNotIn('4', player['equipped'])
        self.assertEqual(next(c for c in player['pack'] if c['name'] == 'Short Sword')['screen'], list(cell))

    def test_belt_boxes_are_the_boxes_retail_hit_tests(self):
        seen = []
        self.world.vm.uc.hook_add(UC_HOOK_CODE, lambda uc, *_: seen.append(uc.reg_read(UC_X86_REG_EAX)),
                                  begin=BELT_HOVER_SLOT, end=BELT_HOVER_SLOT)
        boxes = self.observer.observe()['belt_boxes']
        self.assertEqual(len(boxes), observe.BELT_SLOTS)
        for n, box in enumerate(boxes):
            self.world.step([{'mouse': list(box), 'buttons': 0}])
            seen.clear()
            self.world.step()
            self.assertEqual(set(seen), {observe.BELT_FIRST + n}, box)

    def test_a_drop_on_a_belt_box_puts_the_item_on_the_belt(self):
        obs = self.observer.observe()
        sword = next(c for c in obs['player']['pack'] if c['name'] == 'Short Sword')
        self.drag(sword['screen'], obs['belt_boxes'][0])
        player = self.observer.observe()['player']
        self.assertEqual(player['belt'][0], 'Short Sword')
        self.assertEqual(next(i for i in player['items'] if i['name'] == 'Short Sword')['slot'], observe.BELT_FIRST)

    def test_an_item_dropped_on_the_map_is_listed_with_its_numbers(self):
        obs = self.observer.observe()
        sword = next(c for c in obs['player']['pack'] if c['name'] == 'Short Sword')
        self.drag(sword['screen'], (250, 250))
        obs = self.observer.observe()
        self.assertIn('Short Sword dropped.', obs['messages'])
        self.assertNotIn('Short Sword', [i['name'] for i in obs['player']['items']])
        ground = self.by_name(obs, 'Short Sword')
        self.assertEqual((ground['class'], ground['item']), ('weapon', dict(amount=1, eqslot=4, stats=SHORT_SWORD)))

    def test_characters_carry_their_class_value(self):
        """class.def's CHARACTER TYPES: Rahul {16, 1}, Sardok and Tendrick
        {16, -1} (class.def:3079, 3087, 3099)."""
        obs = self.observer.observe()
        self.assertEqual({o['type']: o['value'] for o in obs['objects'] if o['class'] == 'character'},
                         {'Rahul': 1, 'Sardok': -1, 'Tendrick': -1})
        for o in obs['objects']:
            if o['class'] == 'character':
                self.assertEqual(o['dead'], o['health'][0] < 1)


class AfterTheFirstKillTests(WaypointCase):
    """Rahul killed with the Short Sword, the cloth worn."""
    WAYPOINT = 'rahul defeated'

    def test_the_kill_fed_exp_and_the_sword_skill_alike(self):
        """A level-1 Locke's kill of a Value-1 monster is worth 10, to Exp
        and to the skill of the weapon in hand (GRIND §1.2, §2.1)."""
        player = self.observer.observe()['player']
        self.assertEqual(player['weapon'], dict(name='Short Sword', mask=1 << 2, skill='sword'))
        self.assertEqual(player['xp'], [1, 10, 300])
        self.assertEqual(player['skills']['sword'], [0, 10, 300])
        self.assertEqual({name: skill[1] for name, skill in player['skills'].items() if skill[1]}, {'sword': 10})

    def test_the_worn_cloth_is_retails_armour(self):
        player = self.observer.observe()['player']
        worn = [item for item in player['items'] if 0x100 <= item['slot'] < 0x100 + 11]
        self.assertEqual({item['slot'] - 0x100: item['name'] for item in worn},
                         {int(slot): name for slot, name in player['equipped'].items()})
        self.assertEqual(player['combat']['armor'], sum(item['stats'].get('protection', 0) for item in worn))
        self.assertEqual(player['combat']['armor'], 2)


if __name__ == '__main__':
    unittest.main()
