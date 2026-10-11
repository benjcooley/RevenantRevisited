"""Guard real untextured extraction/raster and material-sensitive barrier proof."""
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from barrier_static_probe import ROOT


class BarrierNativeTests(unittest.TestCase):
    def test_authored_zero_texture_material_and_moved_owner(self):
        with tempfile.TemporaryDirectory(prefix='barrier-native-')as tmp:
            result=subprocess.run([sys.executable,str(Path(__file__).with_name('barrier_static_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--output',tmp],
                text=True,capture_output=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            report=json.loads((Path(tmp)/'manifest.json').read_text())
            self.assertEqual(report['status'],'pass')
            self.assertFalse(report['dummy_texture_used'])
            self.assertFalse(report['native_illumination_producer'])
            self.assertFalse(report['actual_map_runtime'])
            self.assertFalse(report['full_effect_accepted'])
            self.assertTrue(report['compiled_generic_submission']['untextured'])
            self.assertEqual(report['original_raster_branch_calls'],{'no_texture':20,'texture':0})
            self.assertEqual(report['case_count'],4)
            self.assertEqual(report['replays_per_case'],2)
            for case in report['cases']:
                self.assertGreater(case['nonzero_pixels'],0)
                self.assertEqual(case['authored_diffuse'],[1.,0.,0.,1.])
                self.assertEqual(case['compiled_diffuse'],case['authored_diffuse'])
                self.assertEqual(case['color_different_bytes'],0)
                self.assertTrue(case['depth_equal'])
                self.assertEqual(case['material_control_changed_pixels'],case['nonzero_pixels'])
                self.assertTrue(case['material_control_depth_equal'])


if __name__=='__main__':unittest.main()
