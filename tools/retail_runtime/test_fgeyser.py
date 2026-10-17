"""Original simultaneous fgeyser controllers versus retained production controls."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT

class FGeyserTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=90)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_original_single_state_pose_and_admission(self):
  with tempfile.TemporaryDirectory(prefix='fgeyser-poses-')as tmp:
   out=Path(tmp);self.invoke('fgeyser_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],4050);self.assertEqual(r['matrix_fields'],7200)
   self.assertIn('default_unchanged=450',r['controls'])
 def test_full_native_pool_rng_and_skipped_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='fgeyser-state-')as tmp:
   for stride in (1,3):
    out=Path(tmp)/str(stride);native=out/'native';compare=out/'comparison'
    self.invoke('fgeyser_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--render-stride',stride,'--state-only')
    trace=json.loads((native/'trace.json').read_text());r=trace['records']
    self.assertEqual([(c['prototype'],c['capacity'])for c in trace['initial']['controllers']],[(0,145),(2,104)])
    self.assertFalse(r[88]['after']['flags']&0x1000)
    self.assertTrue(r[89]['after']['flags']&0x1000)
    self.assertEqual(r[89]['after']['frame'],89)
    self.assertTrue(all(len(row['before']['pulse_inputs'])==2 for row in r[:89]))
    self.invoke('fgeyser_state_probe.py',native/'trace.json','--output',compare)
    proof=json.loads((compare/'manifest.json').read_text());self.assertEqual(proof['status'],'pass',proof)
    self.assertGreater(proof['checks'],500000);self.assertEqual(proof['error_count'],0)

if __name__=='__main__':unittest.main()
