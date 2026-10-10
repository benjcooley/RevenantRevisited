"""Complete native Water25-drop state and authored geometry, including clock rates."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from water_probe import ROOT,f32


class LiteralWaterComparison(unittest.TestCase):
    def test_all_native_drops_pixels_replay_and_actual_production_clock(self):
        with tempfile.TemporaryDirectory()as temporary:
            output=Path(temporary)/'proof'
            result=subprocess.run([sys.executable,str(Path(__file__).with_name('water_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--output',str(output)],
                capture_output=True,text=True,timeout=40)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            report=json.loads((output/'manifest.json').read_text())
            self.assertEqual(report['status'],'pass');self.assertEqual(report['case_count'],97)
            self.assertEqual(report['native_pool_records'],25);self.assertEqual(report['native_warmup_ticks'],25)
            self.assertEqual(report['native_birth_rng_calls'],87)
            self.assertEqual(report['field_checks'],97*25*10)
            self.assertEqual(report['port_components']['geometry'],'authored_quad')
            self.assertTrue(report['port_components']['runtime_preview_equal'])
            self.assertEqual((output/'port-trace.txt').read_bytes(),(output/'runtime-port-trace.txt').read_bytes())
            for case in report['cases']:
                self.assertEqual(len(case['original_state']['particles']),25)
                self.assertEqual(len(case['port_state']['particles']),25)
                self.assertEqual(case['state_errors'],[])
                if case['image_hashes']:
                    self.assertEqual(case['image_hashes'][0],case['image_hashes'][1])
                    self.assertEqual(case['image_hashes'][2],case['image_hashes'][3])
            reference=report['cases'][96]['original_state']
            self.assertEqual(reference['random_count'],420)
            for fps in (6,24,30,60,144):
                trace=subprocess.check_output([str(output/'water-port-component'),str(output/'native-random-inputs.txt'),str(fps)],text=True)
                lines=trace.splitlines();self.assertEqual(lines[0],'G 0 87')
                self.assertEqual(lines[1],f'C {fps} 96 420')
                particles=[line.split()for line in lines[2:]];self.assertEqual(len(particles),25)
                for expected,values in zip(reference['particles'],particles):
                    self.assertEqual(int(values[3]),expected['time'])
                    self.assertEqual(b''.join(f32(float(v))for v in values[4:]),
                        b''.join(f32(v)for v in expected['values']))
            for fps in (6,60,144):
                trace=subprocess.check_output([str(output/'water-port-component'),str(output/'native-random-inputs.txt'),'fraction',str(fps)],text=True).split()
                self.assertEqual(trace[:2],['F',str(fps)])
                fraction,delta,expected=map(float,trace[2:5])
                self.assertGreater(fraction,0);self.assertLess(fraction,1)
                self.assertGreater(delta,0);self.assertAlmostEqual(delta,expected,delta=4e-6)
                self.assertEqual(trace[5],'1') # Complete25-record state and RNG unchanged by Submit.
            self.assertFalse(report['dosbox_used']);self.assertFalse(report['full_game_integration'])


if __name__=='__main__':unittest.main()
