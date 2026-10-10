from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import buff_base_lighting_probe as probe

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'


class BuffBaseLighting(unittest.TestCase):
    def test_independent_matrices_and_original_lit_bases(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'bases')
        self.assertEqual(report['status'], 'pass')
        self.assertEqual(len(report['cases']), 90)
        self.assertEqual(report['independent_production_emitter_frames'], 60)
        self.assertTrue(all(c['original_warm_replay_equal'] for c in report['cases']))
        self.assertFalse(report['full_effect_accepted'])
        self.assertTrue(all(c['native_colors'] == [[21, 21, 21, 31]] * 4 for c in report['cases']))

    def test_wrong_nonuniform_scale_rotation_order_is_detected(self):
        original = probe.function

        def wrong_order(source, signature):
            body = original(source, signature)
            if signature.startswith('bool T3DAnimator::FmasteryBaseMeshWorldMatrix('):
                body = body.replace('MtxScale(&world,&scale);', '')
                body = body.replace('MtxRotateZ(&world,rotation.Z);',
                    'MtxRotateZ(&world,rotation.Z); MtxScale(&world,&scale);')
            return body

        with tempfile.TemporaryDirectory() as directory, patch.object(probe, 'function', side_effect=wrong_order):
            report = probe.run(EXE, Path(directory) / 'wrong-order', profiles=(probe.PROFILES[2],))
        self.assertEqual(report['status'], 'differences_found')
        self.assertGreater(max(c['matrix_max_error'] for c in report['cases']), .1)
        self.assertTrue(any(c['differing_pixels'] > 0 for c in report['cases']))


if __name__ == '__main__':
    unittest.main()
