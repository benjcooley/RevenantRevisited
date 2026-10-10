"""Original FireCone render boundary, lifecycle, and unresolved stage residual."""
from pathlib import Path
import tempfile
import struct
import unittest
import zipfile
from unittest.mock import patch
import firecone_render_probe as probe

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'


class FireConeNativeRenderDiagnostic(unittest.TestCase):
    def test_negative_uniform_scale_still_submits_and_flicker_flag_does_not_enlarge(self):
        with zipfile.ZipFile(probe.ROOT / 'data/imagery.rvi') as archive:
            parts = probe.parse_asset(archive.read('Imagery/Magic/firecone.i3d'))
        fixture = probe.FireConeRenderFixture(EXE)
        fixture.step()
        base = fixture.vm.u32(fixture.animator + 0x158 + 4)
        fixture.vm.write(base + 12, struct.pack('<3f', -1, -1, -1))
        fixture.vm.put_u32(base + 72, 1)
        state = fixture.state()
        native = fixture.render(parts)
        with tempfile.TemporaryDirectory() as directory:
            binary, assets, _ = probe.compile_submit(Path(directory), parts)
            port = probe.candidate_packets(binary, assets, Path(directory), state, 0, parts)
        self.assertEqual(native[0]['scale'], [-1, -1, -1])
        self.assertEqual((len(native), len(port)), (4, 4))
        self.assertEqual((port[0]['pool'], port[0]['slot']), ('fire', 0))
        self.assertLess(max(abs(a - b) for a, b in zip(native[0]['matrix'][0:2], port[0]['matrix'][0:2])), 2e-7)

    def test_native_full_used_pool_order_moving_states_drain_and_raw_stage_residual(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'render', facings=(0, 128))
        self.assertTrue(report['every_tick_state_replay_verified'])
        self.assertEqual(report['state_fields_exact'], 73806)
        self.assertFalse(report['software_pixels_executed'])
        self.assertFalse(report['full_acceptance'])
        front = {c['tick']: c for c in report['cases'] if c['face_byte'] == 0}
        self.assertEqual([front[t]['native_count'] for t in probe.SELECTED], [0, 4, 8, 24, 57, 77, 90, 50, 1, 0, 0])
        self.assertTrue(all(c['native_count'] == c['production_count'] for c in front.values()))
        self.assertFalse(front[93]['alive'])
        first = front[1]['native_packets'][0]
        self.assertEqual(first['matrix'][12:15], [21, 4, -4])
        self.assertEqual(first['flags'], 0x100)
        self.assertEqual(front[1]['native_render_calls'], [dict(pool=p, arguments=[0, 0]) for p in ('fire', 'smoke', 'burst')])
        self.assertLess(max(front[1]['raw_matrix_max_difference_by_output_column'][:2]), 2e-8)
        self.assertGreater(front[1]['raw_matrix_max_difference_by_output_column'][2], .1)
        self.assertGreater(front[1]['raw_authored_point_max_difference_xyz'][2], 7)
        self.assertTrue(any(c['native_flicker_flags_ignored'] for c in front.values()))
        back = next(c for c in report['cases'] if c['face_byte'] == 128 and c['tick'] == 1)
        self.assertEqual((back['native_count'], back['production_count'], back['paired_packets']), (4, 0, 0))

    def test_wrong_particle_degree_conversion_is_detected_before_bridge(self):
        original_function = probe.function

        def wrong_degrees(source, signature):
            body = original_function(source, signature)
            if signature.startswith('void TFireConeEffect_Bespoke::SubmitPool('):
                body = body.replace('float(particle.rot.X * TORADIAN)', 'particle.rot.X')
            return body

        with tempfile.TemporaryDirectory() as directory, patch.object(probe, 'function', side_effect=wrong_degrees):
            report = probe.run(EXE, Path(directory) / 'negative-control', facings=(0,))
        first = next(c for c in report['cases'] if c['tick'] == 1)
        self.assertGreater(max(first['raw_matrix_max_difference_by_output_column'][:2]), .05)


if __name__ == '__main__':
    unittest.main()
