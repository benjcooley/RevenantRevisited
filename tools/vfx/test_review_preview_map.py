"""Admission tests: source implementation audit is independent of ledger status."""
import unittest
from generate_review_preview_map import PARTIAL, admitted_candidates, candidate_metadata


class ReviewPreviewAdmission(unittest.TestCase):
    def test_placeholders_and_invalid_aliases_fail_closed(self):
        candidates = '''TBlastEffect_BESPOKE TFireWindEffect_BESPOKE
TFairyEffect_BESPOKE TTornadoEffect_BESPOKE TVortexEffect_BESPOKE
TPulpEffect_BESPOKE TDustEffect_BESPOKE TMagicShieldEffect_BESPOKE
TLabyrinthEffect_BESPOKE TJhagaAttackEffect_BESPOKE TJTeleEffect_BESPOKE TCEyesEffect_BESPOKE
TCataclysmEffect_BESPOKE TFunnelEffect_BESPOKE TMaelstromEffect_BESPOKE TNakrnothEffect_BESPOKE
TBuffEffect_Bespoke__Might_BESPOKE TBuffEffect_Bespoke__Stoneskin_BESPOKE
TInvisibleEffect_Bespoke_BESPOKE TBuffEffect_Bespoke__charm_BESPOKE
TBuffEffect_Bespoke__speed_BESPOKE TGoldEffect_BESPOKE TPunchAndJudyEffect_BESPOKE
TCombatFlashEffect_BESPOKE TStrikeEffect_BESPOKE
TFlareAnimator_BESPOKE TFlareAnimator_BESPOKE__Teleporter
TFireSwarmEffect_BESPOKE__dragonfire TFireConeEffect_BESPOKE__dragonfire
TFireSwarmEffect_BESPOKE__dragonattack TFireSwarmEffect_BESPOKE__headfireball
TIcedEffect_BESPOKE__Snow TIcedEffect_BESPOKE__Icedsparks
TWaterFallEffect_BESPOKE__WaterFlft TWaterFallEffect_BESPOKE__RiverFall
TNewUnreviewedEffect_BESPOKE'''.split()
        self.assertEqual(admitted_candidates({'preview_candidates': candidates}), [])

    def test_replacement_bodies_survive_stale_stub_banners(self):
        candidates = '''TBloodEffect_BESPOKE TSparksEffect_BESPOKE
TFireConeEffect_BESPOKE TFireFlashEffect_BESPOKE TMeteorStormEffect_BESPOKE
TGlobeEffect_BESPOKE TFogEffect_Bespoke__MistFog_BESPOKE'''.split()
        self.assertEqual(set(admitted_candidates({'preview_candidates': candidates})), set(candidates))

    def test_new_exact_source_registration_overrides_stale_ledger(self):
        self.assertEqual(admitted_candidates({'retail_type_id':'0xad92bd36',
            'preview_candidates':['TBuffEffect_Bespoke__speed_BESPOKE']}),
            ['TSpeed_AUTHORED_TAGS'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xad92bd35',
            'preview_candidates':[]}), ['TQuicksilver_AUTHORED_TAGS'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0x0ea34fa3',
            'preview_candidates':['TBlastEffect_BESPOKE']}), ['TBlastEffect_BESPOKE'])

    def test_partial_requires_exact_identity_and_explicit_warning(self):
        self.assertEqual(len(PARTIAL), 21)
        for retail_id, (name, reason) in PARTIAL.items():
            row = {'retail_type_id': retail_id, 'preview_candidates': [name]}
            self.assertEqual(admitted_candidates(row), [name])
            partial, description = candidate_metadata(row, name)
            self.assertTrue(partial)
            self.assertIn(reason, description)
            self.assertIn('not complete original implementation', description)
            self.assertEqual(admitted_candidates({'retail_type_id': '0xffffffff',
                                                 'preview_candidates': [name]}), [])
            self.assertEqual(admitted_candidates({'retail_type_id': retail_id,
                                                 'preview_candidates': []}), [])
        fog={'retail_type_id':'0xc24b2a85','preview_candidates':['TFogEffect_BESPOKE']}
        self.assertEqual(admitted_candidates(fog), ['TFogEffect_BESPOKE'])
        self.assertTrue(candidate_metadata(fog, 'TFogEffect_BESPOKE')[0])
        self.assertIn('post-blend RGB565', candidate_metadata(fog, 'TFogEffect_BESPOKE')[1])

    def test_implemented_precedes_partial_and_keeps_separate_metadata(self):
        row = {'retail_type_id': '0x0ea34fa3', 'preview_candidates': [
            'TBlastEffect_BESPOKE', 'TGoldEffect_AUTHORED_TAGS']}
        self.assertEqual(admitted_candidates(row), [
            'TGoldEffect_AUTHORED_TAGS', 'TBlastEffect_BESPOKE'])
        self.assertFalse(candidate_metadata(row, 'TGoldEffect_AUTHORED_TAGS')[0])

    def test_exact_authored_replacement_preferred_without_old_stub_fallback(self):
        self.assertEqual(admitted_candidates({'preview_candidates': [
            'TGoldEffect_BESPOKE', 'TGoldEffect_AUTHORED_TAGS', 'TGoldEffect_AUTHORED_TAGS']}),
            ['TGoldEffect_AUTHORED_TAGS'])


if __name__ == '__main__':
    unittest.main()
