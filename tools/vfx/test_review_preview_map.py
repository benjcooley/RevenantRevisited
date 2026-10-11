"""Admission tests: source implementation audit is independent of ledger status."""
import unittest
from generate_review_preview_map import PARTIAL, SOURCE_MAPPINGS, admitted_candidates, candidate_metadata


class ReviewPreviewAdmission(unittest.TestCase):
    def test_placeholders_and_invalid_aliases_fail_closed(self):
        candidates = '''TBlastEffect_BESPOKE
TTornadoEffect_BESPOKE
TMeteorStormEffect_BESPOKE
TPulpEffect_BESPOKE TDustEffect_BESPOKE TMagicShieldEffect_BESPOKE
TLabyrinthEffect_BESPOKE TJhagaAttackEffect_BESPOKE TJTeleEffect_BESPOKE TCEyesEffect_BESPOKE
TCataclysmEffect_BESPOKE TFunnelEffect_BESPOKE TMaelstromEffect_BESPOKE TNakrnothEffect_BESPOKE
TBuffEffect_Bespoke__Might_BESPOKE TBuffEffect_Bespoke__Stoneskin_BESPOKE
TInvisibleEffect_Bespoke_BESPOKE
TBuffEffect_Bespoke__speed_BESPOKE TGoldEffect_BESPOKE TPunchAndJudyEffect_BESPOKE
TCombatFlashEffect_BESPOKE TStrikeEffect_BESPOKE
TFlareAnimator_BESPOKE TFlareAnimator_BESPOKE__Teleporter
TFireSwarmEffect_BESPOKE__dragonfire TFireConeEffect_BESPOKE__dragonfire
TFireSwarmEffect_BESPOKE__dragonattack TFireSwarmEffect_BESPOKE__headfireball
TIcedEffect_BESPOKE__Icedsparks
TWaterFallEffect_BESPOKE__WaterFlft TWaterFallEffect_BESPOKE__RiverFall
TNewUnreviewedEffect_BESPOKE'''.split()
        self.assertEqual(admitted_candidates({'preview_candidates': candidates}), [])

    def test_blood_exact_controller_precedes_drawable_fallback(self):
        row={'retail_type_id':'0xddc4042e','preview_candidates':['TBloodEffect_BESPOKE']}
        self.assertEqual(admitted_candidates(row),['Blood_AUTHORED','TBloodEffect_BESPOKE'])
        partial,description=candidate_metadata(row,'Blood_AUTHORED')
        self.assertFalse(partial)
        self.assertIn('live global RNG without reseed',description)
        self.assertIn('SetParams(40,224,3,5,5,1)',description)
        self.assertEqual(admitted_candidates({'retail_type_id':'0xffffffff','preview_candidates':['Blood_AUTHORED']}),[])
    def test_teleporter_is_exact_null_spell_not_irisflare_alias(self):
        row={'retail_type_id':'0x369a5d2e','preview_candidates':['IrisFlare_AUTHORED','TFlareAnimator_BESPOKE__Teleporter']}
        self.assertEqual(admitted_candidates(row),['Teleporter_ORIGINAL_NULL'])
        self.assertIn('real kill51',candidate_metadata(row,'Teleporter_ORIGINAL_NULL')[1])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xffffffff','preview_candidates':['Teleporter_ORIGINAL_NULL']}),[])

    def test_quicksand_configured_target(self):
        row={'retail_type_id':'0xbabeface','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['Quicksand_ORIGINAL_TARGET'])
        self.assertIn('stationary target offset(96,0,0)',candidate_metadata(row,'Quicksand_ORIGINAL_TARGET')[1])

    def test_drip_exact_continuous_scope_rejects_aliases(self):
        row={'retail_type_id':'0x5975abde','preview_candidates':['TDripEffect','TDripEffect_LINKED_DRAIN','TDripEffect.OnHand','TBloodEffect_BESPOKE']}
        self.assertEqual(admitted_candidates(row),['TDripEffect_LINKED_NO_SPLASH'])
        self.assertIn('SetParams(20,128,1)',candidate_metadata(row,'TDripEffect_LINKED_NO_SPLASH')[1])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xffffffff','preview_candidates':['TDripEffect_LINKED_NO_SPLASH']}),[])

    def test_ytelekinesis_uses_exact_moving_chunk_controller(self):
        row={"retail_type_id":"0xaeaeeb28","preview_candidates":[]}
        self.assertEqual(admitted_candidates(row),["YTelekinesis_ORIGINAL"])
        self.assertFalse(candidate_metadata(row,"YTelekinesis_ORIGINAL")[0])

    def test_essencedrain_uses_all_root_default_controller(self):
        row={"retail_type_id":"0xdefeca7e","preview_candidates":[]}
        self.assertEqual(admitted_candidates(row),["EssenceDrain_AUTHORED"])

    def test_barriers_and_jhaga_exact_controllers(self):
        for type_id,name in [('0x224a5adf','BarrierEW_ORIGINAL'),('0x224b5adc','BarrierNS_ORIGINAL'),('0xaeaeeb32','JhagaAttack_ORIGINAL')]:
            self.assertEqual(admitted_candidates({'retail_type_id':type_id,'preview_candidates':[]}),[name])

    def test_tornado_keeps_distinct_owner_and_caller_scope(self):
        row={'retail_type_id':'0x2784abde','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['TTornado_CONFIGURED_TARGET'])
        partial,description=candidate_metadata(row,'TTornado_CONFIGURED_TARGET')
        self.assertFalse(partial)
        self.assertIn('natural variant selects Funnel',description)

    def test_ymanadrain_distinct_configured_target(self):
        row={'retail_type_id':'0xae92bc14','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['TYManaDrain_CONFIGURED_TARGET'])
        partial,description=candidate_metadata(row,'TYManaDrain_CONFIGURED_TARGET')
        self.assertFalse(partial)
        self.assertIn('uncapped mana transfer',description)
        self.assertEqual(admitted_candidates({'retail_type_id':'0xad92bc13','preview_candidates':[]}),['TManaDrain_CONFIGURED_TARGET'])

    def test_manadrain_exact_configured_target(self):
        row={'retail_type_id':'0xad92bc13','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['TManaDrain_CONFIGURED_TARGET'])
        partial,description=candidate_metadata(row,'TManaDrain_CONFIGURED_TARGET')
        self.assertFalse(partial)
        self.assertIn('actual mana transfer',description)
        self.assertNotIn('TManaDrain_CONFIGURED_TARGET',admitted_candidates({'retail_type_id':'0xad92bc12','preview_candidates':[]}))

    def test_paralize_named_no_target_is_explicitly_partial(self):
        row = {'retail_type_id': '0xad92bc14', 'preview_candidates': []}
        self.assertEqual(admitted_candidates(row), ['TParalize1_CONFIGURED_TARGET'])
        configured, description=candidate_metadata(row,'TParalize1_CONFIGURED_TARGET')
        self.assertFalse(configured)
        self.assertIn('offset(96,0,0), radius16',description)
        row['preview_candidates']=['TParalize1_NAMED_NO_TARGET']
        self.assertEqual(admitted_candidates(row),['TParalize1_CONFIGURED_TARGET','TParalize1_NAMED_NO_TARGET'])
        partial, reason = candidate_metadata(row, 'TParalize1_NAMED_NO_TARGET')
        self.assertTrue(partial)
        self.assertIn('enormous full trajectory', reason)
        self.assertNotIn('TParalize1_NAMED_NO_TARGET', admitted_candidates({
            'retail_type_id': '0xad92bc15', 'preview_candidates': []}))

    def test_dragonattack_uses_exact_declared_caller(self):
        row={'retail_type_id':'0xba09fead','preview_candidates':['TFireSwarmEffect_BESPOKE__dragonattack']}
        self.assertEqual(admitted_candidates(row),['Dragonattack_ORIGINAL'])
        partial,description=candidate_metadata(row,'Dragonattack_ORIGINAL')
        self.assertFalse(partial)
        self.assertIn('target offset(280,0), ground0',description)
        self.assertIn('arbitrary endpoints',description)
        self.assertNotIn('Dragonattack_ORIGINAL',admitted_candidates({'retail_type_id':'0xad92bc1a','preview_candidates':[]}))

    def test_headfireball_uses_exact_self_moving_trap(self):
        row={'retail_type_id':'0xd000f005','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['HeadFireBall_ORIGINAL'])
        self.assertFalse(candidate_metadata(row,'HeadFireBall_ORIGINAL')[0])

    def test_invisible_uses_exact_null_invoker_controller(self):
        row={'retail_type_id':'0xad92bc16','preview_candidates':[]}
        self.assertEqual(admitted_candidates(row),['Invisible_AUTHORED'])

    def test_smoke_exact_quality_controller(self):
        row={"retail_type_id":"0x224a5ccc","preview_candidates":[]}
        self.assertEqual(admitted_candidates(row),["Smoke_ORIGINAL_QUALITY"])
        self.assertFalse(candidate_metadata(row,"Smoke_ORIGINAL_QUALITY")[0])

    def test_icedsparks_uses_its_verified_default_factory(self):
        row={'retail_type_id':'0xacabfada','preview_candidates':['TIcedEffect_BESPOKE__Icedsparks']}
        self.assertEqual(admitted_candidates(row),['TIcedsparks_AUTHORED_STILL'])
        self.assertFalse(candidate_metadata(row,'TIcedsparks_AUTHORED_STILL')[0])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xffffffff',
            'preview_candidates':['TIcedEffect_BESPOKE__Icedsparks']}),[])

    def test_replacement_bodies_survive_stale_stub_banners(self):
        candidates = '''TBloodEffect_BESPOKE TSparksEffect_BESPOKE
TFireConeEffect_BESPOKE TFireFlashEffect_BESPOKE TFireWindEffect_BESPOKE TFairyEffect_BESPOKE
TGlobeEffect_BESPOKE TFogEffect_Bespoke__MistFog_BESPOKE'''.split()
        self.assertEqual(set(admitted_candidates({'preview_candidates': candidates})), set(candidates))

    def test_new_exact_source_registration_overrides_stale_ledger(self):
        self.assertEqual(admitted_candidates({'retail_type_id':'0x452dade0',
            'preview_candidates':[]}), ['TVortexEffect_BESPOKE'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xffffffff',
            'preview_candidates':[]}), [])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xf32bcfac',
            'preview_candidates':['TMeteorStormEffect_BESPOKE']}),
            ['MeteorStorm_AUTHORED'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xad92bd36',
            'preview_candidates':['TBuffEffect_Bespoke__speed_BESPOKE']}),
            ['TSpeed_AUTHORED_TAGS'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0xad92bd35',
            'preview_candidates':[]}), ['TQuicksilver_AUTHORED_TAGS'])
        self.assertEqual(admitted_candidates({'retail_type_id':'0x0ea34fa3',
            'preview_candidates':['TBlastEffect_BESPOKE']}), ['TBlastEffect_BESPOKE'])

    def test_maelstrom_original_precedes_retained_partial(self):
        row = {'retail_type_id': '0xaeaeeb26',
               'preview_candidates': ['TMaelstromEffect_BESPOKE']}
        self.assertEqual(admitted_candidates(row),
                         ['Maelstrom_ORIGINAL', 'TMaelstromEffect_BESPOKE'])
        self.assertFalse(candidate_metadata(row, 'Maelstrom_ORIGINAL')[0])
        self.assertTrue(candidate_metadata(row, 'TMaelstromEffect_BESPOKE')[0])
        self.assertEqual(admitted_candidates({'retail_type_id': '0xae5eeb26',
            'preview_candidates': []}), ['YMaelstrom_ORIGINAL'])

    def test_windstrip_exact_retail_replacement_precedes_three_strip_preview(self):
        names = admitted_candidates({'retail_type_id':'0xded3dbed',
            'preview_candidates':['TWindStripAnimator_BESPOKE']})
        self.assertEqual(names[0], 'TWindStrip_BESPOKE_NULLSPELL')
        self.assertNotIn('TWindStrip_BESPOKE_NULLSPELL', admitted_candidates({
            'retail_type_id':'0xdedbdbed', 'preview_candidates':[]}))

    def test_partial_requires_exact_identity_and_explicit_warning(self):
        self.assertTrue(PARTIAL)
        self.assertNotIn('0x98974eab', PARTIAL)
        for retail_id, (name, reason) in PARTIAL.items():
            row = {'retail_type_id': retail_id, 'preview_candidates': [name]}
            # Exact authored replacements precede retained partial previews.
            exact = [SOURCE_MAPPINGS[retail_id]] if retail_id in SOURCE_MAPPINGS else []
            self.assertEqual(admitted_candidates(row), list(dict.fromkeys(exact + [name])))
            partial, description = candidate_metadata(row, name)
            self.assertTrue(partial)
            self.assertIn(reason, description)
            self.assertIn('not complete original implementation', description)
            self.assertEqual(admitted_candidates({'retail_type_id': '0xffffffff',
                                                 'preview_candidates': [name]}), [])
            self.assertEqual(admitted_candidates({'retail_type_id': retail_id,
                                                 'preview_candidates': []}), exact)
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
