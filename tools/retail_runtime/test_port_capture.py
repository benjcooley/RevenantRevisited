"""Output files do not turn a crashing or stalled capture process into a pass."""
import json
from pathlib import Path
import sys
import tempfile
import unittest
from port_capture import capture


class CaptureExitTests(unittest.TestCase):
    def exercise(self,exit_code=0,stall=False):
        temporary=tempfile.TemporaryDirectory();self.addCleanup(temporary.cleanup)
        root=Path(temporary.name).resolve();assets=root/'assets';assets.mkdir()
        (assets/'effects.def').write_text('Capture test definitions\n')
        binary=root/'capture-process'
        # A deliberately tiny producer tests subprocess completion, not graphics.
        binary.write_text('#!'+sys.executable+'\n'+'''import os,sys,time
from pathlib import Path
prefix=next(a.split('=',1)[1] for a in sys.argv if a.startswith('--snapprefix='))
Path(prefix+'001-test.png').write_bytes(b'frame fixture')
Path(prefix+'filmstrip.png').write_bytes(b'filmstrip fixture')
Path(prefix+'timing.csv').write_text('frame,simulation_seconds,relative_seconds\\n1,0.083333333,0\\n')
print("--vfx='CaptureFixture' matched",flush=True)
print('assets='+os.environ['REVENANT_ASSETS_PATH'],flush=True)
'''+('time.sleep(60)\n' if stall else f'raise SystemExit({exit_code})\n'))
        binary.chmod(0o755)
        output=root/'output'
        scenario=dict(binary=str(binary),assets_root=str(assets),data_root=str(root),
            effect='CaptureFixture',frames=1,fps=24,timeout_seconds=5)
        return scenario,output,assets

    def test_normal_exit_preserves_explicit_assets_root_and_passes(self):
        scenario,output,assets=self.exercise()
        report=capture(scenario,output)
        self.assertEqual(report['status'],'pass');self.assertEqual(report['exit_code'],0)
        self.assertFalse(report['forced_owned_shutdown'])
        self.assertIn('assets='+str(assets),(output/'run.log').read_text())
        self.assertEqual((output/'effects.def').read_bytes(),(assets/'effects.def').read_bytes())

    def test_complete_outputs_do_not_hide_nonzero_exit(self):
        scenario,output,_=self.exercise(exit_code=7)
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertEqual(report['status'],'fail');self.assertEqual(report['exit_code'],7)
        self.assertIn('code 7',report['error']);self.assertEqual(len(report['frames']),1)

    def test_complete_outputs_do_not_hide_forced_shutdown(self):
        scenario,output,_=self.exercise(stall=True)
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertEqual(report['status'],'fail');self.assertTrue(report['forced_owned_shutdown'])
        self.assertIn('forced shutdown',report['error']);self.assertEqual(len(report['frames']),1)


if __name__=='__main__':unittest.main()
