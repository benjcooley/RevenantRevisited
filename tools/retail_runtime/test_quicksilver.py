"""Exact Quicksilver admission and native particle geometry/raster regression."""
from pathlib import Path
import tempfile
import unittest
from quicksilver_probe import ROOT, run as render
from quicksilver_profile_contract import run as profile


class QuicksilverTests(unittest.TestCase):
    def test_strict_profile_and_independent_render_packets(self):
        executable=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        archive=ROOT/'data/imagery.rvi'
        with tempfile.TemporaryDirectory(prefix='quicksilver-native-') as tmp:
            admission=profile(executable,archive,Path(tmp)/'profile')
            result=render(executable,archive,Path(tmp)/'render')
        self.assertEqual(admission['status'],'pass')
        self.assertEqual(admission['profile_positive_cases'],4)
        self.assertEqual(admission['profile_rejection_cases'],258)
        self.assertEqual(admission['channel_comparisons'],810)
        self.assertEqual(admission['maximum_absolute_error'],0)
        self.assertEqual(result['status'],'pass')
        self.assertEqual(result['type_id'],'0xad92bd35')
        self.assertEqual(result['full_state_errors'],0)
        self.assertEqual(result['full_state_comparisons'],85614)
        self.assertEqual(result['sampled_live_packets'],2221)
        self.assertEqual(result['actual_native_draw_packets'],2109)
        self.assertEqual(len(result['native_zero_scale_rejections']),112)
        self.assertEqual(result['raster_replays_per_case'],2)
        self.assertEqual(len(result['pairs']),5)
        for pair in result['pairs']:
            self.assertGreater(pair['visible_native_pixels'],0)
            self.assertEqual(pair['differing_rgb565_pixels'],0)
            self.assertTrue(pair['depth_equal'])
            self.assertEqual(pair['image_sha256'][0],pair['image_sha256'][1])
        self.assertEqual(result['max_native_FIX_centre_error'],0)
        self.assertFalse(result['metal_backend_compared'])
        self.assertFalse(result['accepted'])


if __name__=='__main__':
    unittest.main()
