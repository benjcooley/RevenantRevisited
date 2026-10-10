"""Exact FireFlash angles/fade/RNG and explicit remaining matrix-rounding scope."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]


class FireFlashNativeComparison(unittest.TestCase):
    def test_full_pool_exact_angles_scale_integer_rng_and_native_blend_mapping(self):
        with tempfile.TemporaryDirectory()as directory:
            output=Path(directory)/'proof'
            result=subprocess.run([sys.executable,str(Path(__file__).with_name('fireflash_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--state-only','--output',str(output)],
                capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            report=json.loads((output/'manifest.json').read_text())
            self.assertEqual(report['native_particle_count'],150)
            self.assertEqual(report['native_birth_rng'],225);self.assertEqual(report['native_final_rng'],759)
            self.assertEqual(len(report['cases']),101)
            self.assertTrue(report['native_blend_mapping']['source_additive_metadata_correct'])
            states=dict(report['native_blend_mapping']['states'])
            self.assertEqual((states[19],states[20]),(2,2))
            f32=lambda value:struct.pack('<f',value)
            for case in report['cases']:
                a,b=case['original_state'],case['port_state']
                self.assertEqual(a['frame'],b['frame']);self.assertEqual(a['random_count'],b['random_count'])
                self.assertEqual([f32(v)for v in a['values']],[f32(v)for v in b['values']])
                self.assertEqual(len(a['particles']),150);self.assertEqual(len(b['particles']),150)
                for native,modern in zip(a['particles'],b['particles']):
                    self.assertEqual(native['integers'],modern['integers'])
                    # Only the explicitly characterized shared matrix output
                    # may differ; angles, scale and all other fields stay exact.
                    self.assertEqual([f32(v)for v in native['values'][3:]],[f32(v)for v in modern['values'][3:]])
                    for x,y in zip(native['values'][:3],modern['values'][:3]):self.assertLessEqual(abs(x-y),5e-6)
                if case['original_draw_count']is not None:
                    self.assertEqual(case['original_draw_count'],len(b['draws']))
                    self.assertTrue(all(d['additive']for d in b['draws']))
            self.assertEqual(report['cases'][0]['float32_differences'],0)
            self.assertFalse(report['software_pixel_comparison_executed'])
            self.assertFalse(report['full_acceptance'])


if __name__=='__main__':unittest.main()
