"""Three strict typed profiles and independent native particle image regressions."""
from pathlib import Path
import tempfile,unittest
from static_particles_profile_contract import PROFILES,ROOT,run as profiles
from static_particles_probe import run as render


class StaticParticleBatchTests(unittest.TestCase):
    def test_strict_profiles(self):
        with tempfile.TemporaryDirectory(prefix='static-particle-profiles-')as tmp:
            report=profiles(ROOT/'data/imagery.rvi',Path(tmp))
        self.assertEqual(report['status'],'pass')
        self.assertEqual([c['positive_cases']for c in report['cases']],[4,4,4])
        self.assertEqual([c['rejection_cases']for c in report['cases']],[732,699,141])
        for case in report['cases']:
            self.assertEqual(case['categories']['C reject typed_id'],4)
            self.assertEqual(case['categories']['C reject vertex_float'],32)
            self.assertEqual(case['categories']['C reject face_index'],6)
        self.assertFalse(report['accepted'])

    def test_all_three_native_frontends(self):
        counts={'Regeneration':(102596,3013,2828,185),'SwiftStrike':(102596,3013,2828,185),'fspray':(218072,4438,4232,206)}
        with tempfile.TemporaryDirectory(prefix='static-particle-render-')as tmp:
            for profile in PROFILES:
                with self.subTest(name=profile['name']):
                    report=render(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/profile['name'],profile)
                    self.assertEqual(report['status'],'pass')
                    self.assertEqual(report['full_state_errors'],0)
                    checks,live,draws,zero=counts[profile['name']]
                    self.assertEqual(report['full_state_comparisons'],checks)
                    self.assertEqual(report['sampled_live_packets'],live)
                    self.assertEqual(report['actual_native_draw_packets'],draws)
                    self.assertEqual(len(report['native_zero_scale_rejections']),zero)
                    self.assertEqual(report['native_dispatch']['registry_count'],93)
                    self.assertEqual(report['native_dispatch']['actual_factory'],'0x40dc00')
                    self.assertEqual(report['fresh_native_state_replays'],2)
                    self.assertEqual(report['native_row_pose_sha256'][0],report['native_row_pose_sha256'][1])
                    self.assertEqual(report['raster_replays_per_case'],2)
                    self.assertEqual(len(report['pairs']),5)
                    for pair in report['pairs']:
                        self.assertEqual(pair['differing_rgb565_pixels'],0)
                        self.assertGreater(pair['visible_native_pixels'],0)
                        self.assertTrue(pair['depth_equal'])
                    self.assertFalse(report['accepted'])


if __name__=='__main__':unittest.main()
