"""Strict admission must preserve exact Speed keys and unrelated key policy."""
from pathlib import Path
import tempfile
import unittest
from speed_profile_contract import ROOT, run


class SpeedProfileContractTests(unittest.TestCase):
    def test_profile_mutations_and_original_integer_decoder(self):
        with tempfile.TemporaryDirectory(prefix='speed-profile-native-') as tmp:
            report=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',
                       ROOT/'data/imagery.rvi',Path(tmp))
        self.assertEqual(report['status'],'pass')
        self.assertEqual(report['error_count'],0)
        self.assertEqual(report['type_id'],'0xad92bd36')
        self.assertEqual(report['profile_positive_cases'],4)
        self.assertEqual(report['profile_rejection_cases'],258)
        categories=report['mutation_categories']
        self.assertEqual(categories['reject:key_word'],53)
        self.assertEqual(categories['reject:material_float'],51)
        self.assertEqual(categories['reject:quad_float'],64)
        self.assertEqual(categories['reject:face_index'],12)
        self.assertEqual(report['boundary_policy_cases'],['default_half_open','admitted_inclusive'])
        self.assertEqual(report['pose_samples'],90)
        self.assertEqual(report['channel_comparisons'],810)
        self.assertEqual(report['maximum_absolute_error'],0)
        self.assertEqual(report['new_rendered_ab_cases'],0)
        self.assertFalse(report['accepted'])


if __name__=='__main__':
    unittest.main()
