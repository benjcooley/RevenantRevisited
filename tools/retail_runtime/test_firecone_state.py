"""FireCone complete null-spell pool oracle and arithmetic regression control."""
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import firecone_state_probe as probe

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'


class FireConeNativeStateComparison(unittest.TestCase):
    def test_all_pool_slots_defined_fields_rng_and_call_order_through_drain(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'proof', 100)
        self.assertEqual(report['status'], 'pass')
        self.assertEqual(report['state_errors'], [])
        self.assertEqual(report['float_difference_count'], 0)
        self.assertEqual(report['pool_slots'], 260)
        self.assertEqual(report['float_fields'], 73806)
        self.assertEqual(report['native_random_calls'], 1243)
        self.assertEqual(report['owner_position'], [10000, 10000, 116])
        self.assertEqual(report['initialization_calls'], [('0x50c170', 'fire'), ('0x50c170', 'burst'), ('0x50c170', 'smoke')])
        summary = report['native_summary']
        self.assertEqual(summary[6]['state'], 2)
        self.assertEqual(summary[15]['state'], 3)
        self.assertEqual(summary[33]['done'], 1)
        self.assertTrue(summary[92]['alive'])
        self.assertFalse(summary[93]['alive'])
        self.assertEqual(summary[100]['active'], dict(fire=0, smoke=0, burst=0))

    def test_float_scale_intermediate_is_detected_before_first_transition(self):
        original_function = probe.function

        def premature_scale_store(source, signature):
            body = original_function(source, signature)
            if signature.startswith('void TFireConeEffect_Bespoke::SimulateTick('):
                body = body.replace(
                    'const long double equ = (125.0L - static_cast<long double>(flame->pos.Z)) *\n'
                    '                                static_cast<long double>(.01f);',
                    'const float equ = (125.0f - flame->pos.Z) * .01f;')
            return body

        with tempfile.TemporaryDirectory() as directory, patch.object(probe, 'function', side_effect=premature_scale_store):
            report = probe.run(EXE, Path(directory) / 'regression', 2)
        self.assertEqual(report['state_errors'], [])
        self.assertGreater(report['float_difference_count'], 0)
        first = report['first_float_differences'][0]
        self.assertEqual((first['tick'], first['pool'], first['slot'], first['field']), (2, 'fire', 0, 3))
        self.assertEqual(first['original'], 0.30263999104499817)
        self.assertEqual(first['port'], 0.3026399612426758)


if __name__ == '__main__':
    unittest.main()
