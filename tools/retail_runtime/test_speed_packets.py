#!/usr/bin/env python3
"""Original-native versus actual-production Speed packet regression."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]


class SpeedPacketTests(unittest.TestCase):
    def test_native_geometry_admission_uv_and_shared_software(self):
        with tempfile.TemporaryDirectory(prefix='speed-packets-')as temporary:
            output=Path(temporary)
            command=[sys.executable,str(Path(__file__).with_name('speed_packet_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--output',str(output)]
            p=subprocess.run(command,capture_output=True,text=True,timeout=60)
            self.assertEqual(p.returncode,0,p.stdout+p.stderr)
            manifest=json.loads((output/'manifest.json').read_text())
            self.assertEqual(manifest['status'],'pass',manifest)
            self.assertEqual(manifest['sampled_live_packets'],2221)
            self.assertEqual(manifest['actual_native_draw_packets'],2109)
            self.assertEqual(len(manifest['native_zero_scale_rejections']),112)
            self.assertTrue(all(r['native_matrix_return']==0 and r['source_draw_count']==0
                for r in manifest['native_zero_scale_rejections']))
            self.assertEqual(manifest['geometry_errors'],[])
            self.assertEqual(manifest['centre_errors'],[])
            self.assertEqual(manifest['independent_centre_checks'],4218)
            self.assertIn('keys_sha256',manifest['independent_production_emitter'])
            self.assertLess(manifest['max_raw_centre_error'],2e-5)
            self.assertLess(manifest['max_native_FIX_centre_error'],3e-6)
            self.assertEqual(manifest['pixel_errors'],[])
            self.assertLess(manifest['max_corner_error'],6e-5)
            self.assertLess(manifest['max_uv_error'],1e-7)
            self.assertEqual([p['tick']for p in manifest['pairs']],[5,15,29,60,89])
            self.assertTrue(all(p['differing_rgb565_pixels']==0 and p['depth_equal'] and
                p['visible_native_pixels']>0 for p in manifest['pairs']))
            self.assertFalse(manifest['full_game_integration'])
            self.assertFalse(manifest['metal_backend_compared'])


if __name__=='__main__':
    unittest.main()
