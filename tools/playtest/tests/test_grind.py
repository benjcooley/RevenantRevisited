import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from grind import REVISIT, Grounds  # noqa: E402
from knowledge import Character, CharType, Knowledge  # noqa: E402
from world import Generator, Place, World, read_generator  # noqa: E402

DATA = Path(__file__).resolve().parents[3] / 'data'


def generator_record(slots, cap) -> str:
    out = bytes([len(slots)])
    for name, freq in slots:
        out += name.encode() + b'\0' + struct.pack('<iii', 1, freq, 0)
    return (out + struct.pack('<i', cap)).hex()


class StubWorld:
    def __init__(self, places):
        self.places = places

    def objects(self, level):
        return tuple(p for p in self.places if p.level == level)

    def generators(self, level):
        return tuple(g for p in self.objects(level) if p.typename == 'MonsterGen' for g in [read_generator(p)] if g)


def kb() -> Knowledge:
    types = {t.name.lower(): t for t in (CharType('Araknid', 14, 1, True), CharType('Kantha', 16, 2, True),
                                         CharType('Thrush Druhg', 10, 3, True), CharType('Townsman', 16, -1, False))}
    return Knowledge({'Thrush Druhg': Character('Thrush Druhg', damagemods={'hand': 20})}, {}, types)


def monster(kind, x, y, name=''):
    return Place(0, (x, y, 0), name, kind, 'CHARACTER')


class Generators(unittest.TestCase):
    def test_a_generator_record_reads_like_monstergen_load(self):
        place = Place(0, (5, 5, 0), '', 'MonsterGen', 'HELPER', generator_record([('kantha', 7400), ('araknid ', 9100)], 3))
        self.assertEqual(read_generator(place), Generator(place, (('kantha', 7400), ('araknid', 9100)), 3))
        self.assertIsNone(read_generator(Place(0, (5, 5, 0), '', 'MonsterGen', 'HELPER', '02ff')))   # cut short

    @unittest.skipUnless((DATA / 'imagery.rvi').exists(), 'no game data')
    def test_level_0_has_the_generators_grind_md_counts(self):
        self.assertEqual(len(World(DATA).generators(0)), 109)


class Sites(unittest.TestCase):
    def grounds(self):
        return Grounds.survey(StubWorld([
            monster('Araknid', 1000, 1000), monster('Araknid', 1100, 1000),          # an easy knot
            Place(0, (1050, 1050, 0), '', 'MonsterGen', 'HELPER', generator_record([('kantha', 7400)], 2)),
            monster('Thrush Druhg', 5000, 5000), monster('Thrush Druhg', 5050, 5000),  # a hard one, far off
            monster('Araknid', 5100, 5000),
            monster('Townsman', 9000, 9000),                                          # not hostile
            monster('Araknid', 9000, 9100, name='pet1'),                              # a named one: the story's
        ]), kb(), 0)

    def test_sites_group_hostiles_and_generators(self):
        g = self.grounds()
        self.assertEqual(len(g.sites), 2)
        easy = next(s for s in g.sites if s.centre[0] < 2000)
        self.assertEqual(dict(easy.monsters), {'Araknid': 2, 'Kantha': 2})        # a slot counts its cap
        self.assertEqual(easy.generators, 1)

    def test_a_level_1_locke_hunts_the_easy_site_and_avoids_the_strong(self):
        g = self.grounds()
        site = g.next((0, 0, 0), 1, 'hands', 0)
        self.assertLess(site.centre[0], 2000)
        hard = next(s for s in g.sites if s.centre[0] > 4000)
        self.assertLess(g.score(hard, 1, 'hands'), 0)                              # two Thrush Druhgs above his class
        self.assertGreater(g.score(hard, 3, 'weapon'), g.score(hard, 3, 'hands'))  # fists do 80% to them

    def test_a_visited_site_rests_while_its_generators_restock(self):
        g = self.grounds()
        first = g.next((0, 0, 0), 2, 'weapon', 0)
        g.visited(first, 100)
        self.assertNotEqual(g.next((0, 0, 0), 2, 'weapon', 200), first)
        self.assertEqual(g.next((0, 0, 0), 2, 'weapon', 100 + REVISIT), first)


if __name__ == '__main__':
    unittest.main()
