import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from knowledge import Knowledge, parse_characters, parse_spells  # noqa: E402

DATA = Path(__file__).resolve().parents[3] / 'data'

CHAR_DEF = r'''
#define CA_HAND   0x0010
#define CA_BLOOD  0x0200
#define WM_HAND   0x0001
CHARACTER "Brute"	// comment
BEGIN
  DAMAGEMODS 0, 10, -5, 0, 0, 0, 0, -100, 25, 0
  GROUPS "beast"
	ATTACK "smash", CA_HAND | CA_BLOOD, 2, "", "", "", "",\
		   3, 4, 12, 0, 19, 75,\
		   0, 45, 32, 16, 14, 7,\
		   WM_HAND, 2, 100, 0, 0
	IMPACT "imph", 0, "", 0, 0, 10000, 0, 0
END
'''

SPELL_DEF = r'''
#define DT_BURN 7
#define TP_BASIC 3
#define FALSE 0
SPELL "Spark"
BEGIN
	DAMAGETYPE DT_BURN
	VARIANT "Spark", TP_BASIC, "LI", "fireflash", 31, 50, 50, 55, 1, 0, FALSE, 50
	CONTROLDATA "Strike"
	BEGIN
		PATTERN "circle"
	END
	VARIANT "Enemy Spark", TP_BASIC, "MA", "fireflash", 186, 50, 56, 68, 0, 0, FALSE, 50
END
'''


class Parsing(unittest.TestCase):
    def test_character_block(self):
        brute = parse_characters(CHAR_DEF)['Brute']
        self.assertEqual(brute.damagemods['burn'], -100)
        self.assertEqual(brute.damagemods['freeze'], 25)
        self.assertEqual(brute.groups, 'beast')
        smash = brute.attacks[0]
        self.assertEqual((smash.name, smash.flags, smash.button, smash.impacttime), ('smash', 0x210, 2, 4))
        self.assertEqual((smash.maxdist, smash.fatigue, smash.attackskill, smash.weaponskill), (75, 14, 7, 2))
        self.assertEqual(smash.control, 'CombatThrust')

    def test_spells_keep_player_variants_only(self):
        spells = parse_spells(SPELL_DEF)
        self.assertEqual(list(spells), ['Spark'])
        spark = spells['Spark']
        self.assertEqual((spark.damagetype, spark.talismans, spark.mana, spark.damage, spark.skill, spark.pattern),
                         ('burn', ('Moon', 'Sky'), 31, (50, 55), 1, 'circle'))


@unittest.skipUnless((DATA / 'resources.rvr').exists(), 'no game data in this checkout')
class ShippedData(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.kb = Knowledge.load(DATA)

    def test_known_facts(self):
        self.assertIn('freeze', self.kb.monster_brief('White Dragon')['resists'])
        self.assertIn('burn', self.kb.monster_brief('White Dragon')['takes_extra_damage_from'])
        self.assertIn('burn', self.kb.monster_brief('Cinderate')['resists'])
        self.assertEqual(self.kb.spells['Quicksand'].talismans, ('Earth', 'Ocean'))
        self.assertFalse(self.kb.monster_brief('Nobody')['known'])

    def test_combos_need_skill(self):
        unskilled = {a.button for a in self.kb.locke_attacks(0x1e, attackskill=0, weaponskill=0)}
        skilled = {a.button for a in self.kb.locke_attacks(0x1e, attackskill=30, weaponskill=30)}
        self.assertTrue({1, 2, 3} <= unskilled)
        self.assertFalse({10, 11, 12} & unskilled)
        self.assertTrue({10, 11, 12} <= skilled)


if __name__ == '__main__':
    unittest.main()
