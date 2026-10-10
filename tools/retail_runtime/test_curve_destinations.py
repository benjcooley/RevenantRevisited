#!/usr/bin/env python3
"""Bounded actual native/parser regression for curve scalar destinations."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import zipfile

from speed_controller_preflight import ROOT, read_asset, SPEED_SHA


class CurveDestinationTests(unittest.TestCase):
    def test_actual_native_vs_compiled_destinations(self):
        executable=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        with tempfile.TemporaryDirectory(prefix='curve-destinations-')as temporary:
            output=Path(temporary)
            with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
                member=next(n for n in archive.namelist()if n.lower()=='imagery/magic/speed.i3d')
                asset=read_asset(archive.read(member))
            self.assertEqual(asset['sha256'],SPEED_SHA)
            asset['name']='speed'
            metadata=output/'assets.json';metadata.write_text(json.dumps([asset]))
            command=[sys.executable,str(Path(__file__).with_name('curve_destination_probe.py')),
                str(executable),'--asset-triage',str(metadata),'--output',str(output/'probe')]
            process=subprocess.run(command,capture_output=True,text=True,timeout=45)
            self.assertEqual(process.returncode,0,process.stdout+process.stderr)
            manifest=json.loads((output/'probe/manifest.json').read_text())
            self.assertEqual(manifest['status'],'pass',manifest)
            self.assertEqual(manifest['errors'],[])
            self.assertEqual(manifest['comparisons'],28)
            rows={row['name']:row for row in manifest['cases']}
            expected={'absent':[1,1,100,100],'scale-literal':[1.5,1,100,100],
                'scale-curve':[0,1,100,100],'alpha-literal':[1,.75,100,100],
                'alpha-curve':[1,0,100,100],'bounce-literal':[1,1,25,75],
                'bounce-curve':[1,1,0,100]}
            self.assertEqual(set(rows),set(expected))
            for name,values in expected.items():
                self.assertEqual(rows[name]['native'],values,name)
                self.assertEqual(rows[name]['production'],values,name)
                self.assertTrue(rows[name]['equal'],name)
                self.assertGreater(rows[name]['original_call_counts'].get('0x40d750',0),0)
                self.assertGreater(rows[name]['original_call_counts'].get('0x403320',0),0)


if __name__=='__main__':
    unittest.main()
