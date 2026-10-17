"""Causal relative-translation regression and real native normal illumination."""
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import firecone_owner_probe as probe
import firecone_render_probe as render
from lit_software_probe import LitSoftwareFixture

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'


class FireConeOwnerContract(unittest.TestCase):
    def test_native_owner_lit_pixels_and_natural_drain(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'owner', facings=(0, 128), ticks=(1, 20, 93))
        self.assertEqual(report['status'], 'pass')
        self.assertGreaterEqual(report['nonempty_pairs'], 4)
        self.assertEqual(report['exact_state_fields'], 73806)
        self.assertTrue(report['original_illumination_executed'])
        self.assertFalse(report['full_effect_accepted'])
        self.assertTrue(all(c['native_packets'] == c['production_packets'] == 0
                            for c in report['cases'] if c['tick'] == 93))

    def test_old_translation_stretch_cancellation_changes_pixels(self):
        original = render.function

        def old_translation(source, signature):
            body = original(source, signature)
            if signature.startswith('void TFireConeEffect_Bespoke::SubmitPool('):
                body = body.replace('MtxTranslate(&local, &particle.pos);',
                    'const hmm_vec3 old_pos={particle.pos.X,particle.pos.Y,'
                    'REV_FIX_Z_VALUE(FIX_Z_VALUE(particle.pos.Z))/WORLD3D_Z_SCALE};'
                    'MtxTranslate(&local,&old_pos);')
            return body

        with tempfile.TemporaryDirectory() as directory, patch.object(render, 'function', side_effect=old_translation):
            report = probe.run(EXE, Path(directory) / 'old', facings=(0,), ticks=(1, 20))
        self.assertEqual(report['status'], 'differences_found')
        self.assertTrue(all(c['differing_pixels'] > 0 for c in report['cases']))
        self.assertGreater(max(c['paired_matrix_max_error'] for c in report['cases']), 1)

    def test_native_authored_normals_control_illumination(self):
        software = LitSoftwareFixture(EXE, 64, 64)
        software.lighting((38, 38, 38), (1, 1, 1))
        identity = [float(i % 5 == 0) for i in range(16)]
        lit = software.draw_authored([(0, 0, 0, 0, 0, 1, 0, 0)], [], identity, raster=False)[2][0]
        away = software.draw_authored([(0, 0, 0, 0, 0, -1, 0, 0)], [], identity, raster=False)[2][0]
        self.assertEqual(lit[3:7], (23, 23, 23, 31))
        self.assertEqual(away[3:7], (4, 4, 4, 31))


if __name__ == '__main__':
    unittest.main()
