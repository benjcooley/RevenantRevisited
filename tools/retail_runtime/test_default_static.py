"""All two selected asset pools: native dispatch, authored pose and software pixels."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]


class DefaultStaticComparison(unittest.TestCase):
    def test_complete_default_assets_match_compiled_production_and_replay(self):
        with tempfile.TemporaryDirectory()as temporary:
            output=Path(temporary)
            process=subprocess.run([sys.executable,str(Path(__file__).with_name('default_static_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--output',str(output)],
                capture_output=True,text=True,timeout=30)
            self.assertEqual(process.returncode,0,process.stdout+process.stderr)
            report=json.loads((output/'manifest.json').read_text())
            self.assertEqual(report['status'],'pass');self.assertEqual(report['case_count'],8)
            self.assertEqual(report['replays_per_case'],2)
            lookup=report['native_builder_dispatch']['lookup']
            self.assertEqual(lookup['CharUtility'],'0x5e8508');self.assertEqual(lookup['SewerWater'],'0x5e8508')
            self.assertEqual(lookup['EFFECT'],'0x5e8508');self.assertNotEqual(lookup['FLAME'],'0x5e8508')
            self.assertEqual(len(report['native_builder_dispatch']['registrations']),92)
            for case in report['cases']:
                self.assertEqual(case['differing_rgb565_pixels'],0);self.assertTrue(case['depth_equal'])
                self.assertGreater(case['nonzero_retail_pixels'],0)
                self.assertEqual(len(case['poses']),5 if case['name']=='CharUtility'else 1)
                self.assertEqual(sum(len(p['original_positions'])for p in case['poses']),50 if case['name']=='CharUtility'else 4)
                self.assertEqual(case['image_hashes'][0],case['image_hashes'][1])
                self.assertEqual(case['image_hashes'][2],case['image_hashes'][3])
            self.assertFalse(report['dosbox_used']);self.assertFalse(report['full_game_integration'])
            self.assertFalse(report['metal_backend_compared'])


if __name__=='__main__':unittest.main()
