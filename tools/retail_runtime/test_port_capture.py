"""Output files do not turn a crashing or stalled capture process into a pass."""
import json
from pathlib import Path
import sys
import tempfile
import unittest
from port_capture import capture


class CaptureExitTests(unittest.TestCase):
    def exercise(self,exit_code=0,stall=False,visible=True,rejected=False,corrupt=False,flush_delay=0,depth=None):
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
print('trace='+os.environ.get('REV_CAPTURE_FIXTURE_TRACE',''),flush=True)
'''.replace('COLOR',repr((255,255,255) if visible else (0,0,0)))+
            ("print('[authored-static] missing objects or unsupported controller tags',flush=True)\n" if rejected else '')+
            ("Path(prefix+'001-test.png').write_bytes(b'invalid PNG')\n" if corrupt else '')+
            (f'print({depth!r},flush=True)\n' if depth else '')+
            (f'time.sleep({flush_delay})\n')+
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

    def test_scenario_trace_and_lighting_policy_are_self_contained(self):
        scenario,output,assets=self.exercise()
        scenario['environment']={'REV_CAPTURE_FIXTURE_TRACE':'1','REVENANT_ASSETS_PATH':'ignored-explicit-root'}
        scenario['lighting_mode']='modern'
        report=capture(scenario,output)
        self.assertIn('--vfx-lighting-mode=modern',report['command'])
        self.assertIn('trace=1',(output/'run.log').read_text())
        self.assertIn('assets='+str(assets),(output/'run.log').read_text())

    def test_invalid_scenario_environment_and_lighting_reject_before_launch(self):
        for key,value in [('environment',[]),('environment',{'REV_TRACE':1}),('environment',{'BAD=KEY':'1'}),('lighting_mode','unknown')]:
            scenario,output,_=self.exercise()
            scenario[key]=value
            with self.assertRaises(ValueError):capture(scenario,output)
            self.assertFalse((output/'run.log').exists())

    def test_explicit_lossless_png_compression_reaches_command(self):
        scenario,output,_=self.exercise()
        scenario['png_compression']=5
        report=capture(scenario,output)
        self.assertIn('--snapcompression=5',report['command'])

    def test_invalid_png_compression_rejects_before_launch(self):
        for level in [4,10,True,5.5,'5']:
            scenario,output,_=self.exercise()
            scenario['png_compression']=level
            with self.assertRaisesRegex(ValueError,'PNG compression'):
                capture(scenario,output)
            self.assertFalse((output/'run.log').exists())

    def test_sparse_output_preserves_simulation_step(self):
        scenario,output,_=self.exercise()
        scenario['capture_stride']=8
        report=capture(scenario,output)
        self.assertIn('--filmstrip=1,0.3333333333333333',report['command'])
        self.assertIn('--snapstep=0.0416666666666667',report['command'])
        self.assertEqual(report['capture_stride'],8)
        self.assertEqual(report['sample_simulation_ticks'],[2])

    def test_nonpositive_capture_stride_rejects_before_launch(self):
        scenario,output,_=self.exercise()
        scenario['capture_stride']=0
        with self.assertRaisesRegex(ValueError,'capture stride'):
            capture(scenario,output)
        self.assertFalse(output.exists())

    def test_filmstrip_flush_can_use_remaining_declared_budget(self):
        scenario,output,_=self.exercise(flush_delay=3)
        report=capture(scenario,output)
        self.assertEqual(report['status'],'pass')
        self.assertFalse(report['forced_owned_shutdown'])
        self.assertLess(report['elapsed_seconds'],scenario['timeout_seconds'])

    def test_explicit_viewport_resolution_reaches_capture_command(self):
        scenario,output,_=self.exercise()
        scenario['resolution']=[1024,768]
        report=capture(scenario,output)
        self.assertIn('--resolution=1024x768',report['command'])

    def test_explicit_depth_range_is_recorded(self):
        scenario,output,_=self.exercise(depth='[vfx-source-depth] native=0 near=2238 far=3262')
        scenario['depth_range']=[2238,3262]
        report=capture(scenario,output)
        self.assertEqual(report['depth_range'],[2238,3262])
        self.assertFalse(report['depth_range_native_domain'])

    def test_explicit_depth_range_requires_matching_runtime_marker(self):
        for marker in (None,'[vfx-source-depth] native=0 near=2238 far=3262'):
            with self.subTest(marker=marker):
                scenario,output,_=self.exercise(depth=marker)
                scenario['depth_range']=[0,65535]
                with self.assertRaises(RuntimeError):capture(scenario,output)
                report=json.loads((output/'manifest.json').read_text())
                self.assertIn('viewer depth range',report['error'])

    def test_signed_actual_camera_is_recorded(self):
        scenario,output,_=self.exercise(depth='[vfx-camera] explicit=1 actual=(1280,-220) viewport=(1920,1080)')
        scenario.update(camera=[1280,-220],resolution=[1920,1080])
        report=capture(scenario,output)
        self.assertEqual(report['actual_camera'],[1280,-220])
        self.assertEqual(report['actual_viewport'],[1920,1080])

    def test_camera_fallback_and_wrong_resolution_reject(self):
        for marker in ('[vfx-camera] explicit=1 actual=(1280,540) viewport=(1920,1080)',
                       '[vfx-camera] explicit=0 actual=(1280,-220) viewport=(1920,1080)',
                       '[vfx-camera] explicit=1 actual=(1280,-220) viewport=(640,480)'):
            with self.subTest(marker=marker):
                scenario,output,_=self.exercise(depth=marker)
                scenario.update(camera=[1280,-220],resolution=[1920,1080])
                with self.assertRaises(RuntimeError):capture(scenario,output)
                self.assertIn('Actual viewer',json.loads((output/'manifest.json').read_text())['error'])

    def test_nonpositive_resolution_rejects_before_process_launch(self):
        scenario,output,_=self.exercise()
        scenario['resolution']=[1024,0]
        with self.assertRaisesRegex(ValueError,'Positive resolution'):
            capture(scenario,output)

    def test_native_domain_requires_adapter_confirmation(self):
        scenario,output,_=self.exercise()
        scenario['native_domain']=True
        with self.assertRaises(RuntimeError):capture(scenario,output)
        report=json.loads((output/'manifest.json').read_text())
        self.assertEqual(report['status'],'fail')
        self.assertIn('coordinate policy',report['error'])

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
