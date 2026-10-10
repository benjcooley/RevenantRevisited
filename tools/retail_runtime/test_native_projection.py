"""Executed owner/view and a wrong-domain negative control, not sampled math."""
from pathlib import Path
import tempfile
import unittest
from native_projection_contract import run, ROOT

class NativeProjectionTests(unittest.TestCase):
    def test_identity_moving_and_tilted_owners(self):
        executable=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        if not executable.is_file():self.skipTest('Pinned private retail executable required')
        with tempfile.TemporaryDirectory() as temporary:
            report=run(executable,Path(temporary)/'cases',selected_cases=(0,17,24))
        self.assertEqual(report['status'],'pass')
        self.assertEqual(len(report['cases']),3)
        for case in report['cases']:
            self.assertEqual(case['differing_pixels'],0)
            self.assertGreater(case['old_common_owner_differing_pixels'],0)
