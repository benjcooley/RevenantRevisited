from pathlib import Path
import tempfile
import unittest
from speed_composite_probe import ROOT, run


class SpeedComposite(unittest.TestCase):
    def test_complete_isolated_components_blend_order_and_lighting(self):
        with tempfile.TemporaryDirectory() as directory:
            report = run(ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe', Path(directory) / 'composites')
        self.assertEqual(report['status'], 'pass')
        self.assertEqual(len(report['cases']), 6)
        for case in report['cases']:
            self.assertGreater(case['particles'], 0)
            self.assertGreater(case['pixels_changed_by_base'], 0)
            self.assertEqual(case['native_base_colors'], [[21, 21, 21, 31]] * 4)
            self.assertEqual(case['production_base_colors'], case['native_base_colors'])
            self.assertEqual(case['differing_pixels'], 0)
            self.assertTrue(case['depth_equal'])
            self.assertTrue(case['native_warm_replay_equal'])
        self.assertFalse(report['actual_map_compared'])
        self.assertFalse(report['full_effect_accepted'])


if __name__ == '__main__':
    unittest.main()
