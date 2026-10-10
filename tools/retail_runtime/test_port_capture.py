"""Output files do not turn a crashing or stalled capture process into a pass."""
import json
from pathlib import Path
import sys
import tempfile
import unittest
from port_capture import capture


class CaptureExitTests(unittest.TestCase):
    def exercise(self,exit_code=0,stall=False,visible=True,rejected=False,corrupt=False):
        temporary=tempfile.TemporaryDirectory();self.addCleanup(temporary.cleanup)
        root=Path(temporary.name).resolve();assets=root/'assets';assets.mkdir()
        (assets/'effects.def').write_text('Capture test definitions\n')
        binary=root/'capture-process'
        # A deliberately tiny producer tests subprocess completion, not graphics.
        binary.write_text('#!'+sys.executable+'\n'+'''import os,sys,time
from pathlib import Path
from PIL import Image
prefix=next(a.split('=',1)[1] for a in sys.argv if a.startswith('--snapprefix='))
Image.new('RGB',(1,1),COLOR).save(prefix+'001-test.png')
Image.new('RGB',(1,1),COLOR).save(prefix+'filmstrip.png')
Path(prefix+'timing.csv').write_text('frame,simulation_seconds,relative_seconds\\n1,0.083333333,0\\n')
print("--vfx='CaptureFixture' matched",flush=True)
print('assets='+os.environ['REVENANT_ASSETS_PATH'],flush=True)
'''.replace('COLOR',repr((255,255,255) if visible else (0,0,0)))+
            ("print('[authored-static] missing objects or unsupported controller tags',flush=True)\n" if rejected else '')+
            ("Path(prefix+'001-test.png').write_bytes(b'invalid PNG')\n" if corrupt else '')+
            ('time.sleep(60)\n' if stall else f'raise SystemExit({exit_code})\n'))
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
        self.assertEqual(report['pixel_summary']['nonblack_frames'],1)

    def test_empty_black_preview_cannot_pass_as_visible_effect(self):
        scenario,output,_=self.exercise(visible=False)
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertEqual(report['pixel_summary']['nonblack_frames'],0)
        self.assertIn('No visible effect',report['error'])

    def test_explicit_empty_control_capture_is_allowed(self):
        scenario,output,_=self.exercise(visible=False)
        scenario['expect_visible']=False
        self.assertEqual(capture(scenario,output)['status'],'pass')

    def test_asset_rejection_cannot_pass_even_with_visible_pixels(self):
        scenario,output,_=self.exercise(rejected=True)
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertIn('rejected its asset/controller',report['error'])

    def test_invalid_image_cannot_pass(self):
        scenario,output,_=self.exercise(corrupt=True)
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertIn('Invalid captured image',report['error'])

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
