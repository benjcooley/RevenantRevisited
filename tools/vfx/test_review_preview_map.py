"""Admission tests: source implementation audit is independent of ledger status."""
import unittest
from generate_review_preview_map import admitted_candidates


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
        candidates = '''TBloodEffect_BESPOKE TAuraEffect_BESPOKE TSparksEffect_BESPOKE
TFireConeEffect_BESPOKE TFireFlashEffect_BESPOKE TMeteorStormEffect_BESPOKE
TGlobeEffect_BESPOKE TFogEffect_Bespoke__MistFog_BESPOKE'''.split()
        self.assertEqual(set(admitted_candidates({'preview_candidates': candidates})), set(candidates))

    def test_exact_authored_replacement_preferred_without_old_stub_fallback(self):
        self.assertEqual(admitted_candidates({'preview_candidates': [
            'TGoldEffect_BESPOKE', 'TGoldEffect_AUTHORED_TAGS', 'TGoldEffect_AUTHORED_TAGS']}),
            ['TGoldEffect_AUTHORED_TAGS'])


if __name__ == '__main__':
    unittest.main()
