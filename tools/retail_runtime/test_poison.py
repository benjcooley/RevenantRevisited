"""Original two four-emitter poison controllers versus retained production controls."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT

class PoisonTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=90)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_original_single_state_pose_and_admission(self):
  with tempfile.TemporaryDirectory(prefix='poison-poses-')as tmp:
   out=Path(tmp);self.invoke('poison_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],3240);self.assertEqual(r['matrix_fields'],5760)
   self.assertIn('default_unchanged=360',r['controls'])
 def test_full_native_pool_rng_and_skipped_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='poison-state-')as tmp:
   for stride in (1,3):
    out=Path(tmp)/str(stride);native=out/'native';compare=out/'comparison'
    self.invoke('poison_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--render-stride',stride,'--state-only')
    trace=json.loads((native/'trace.json').read_text());r=trace['records']
    self.assertEqual([(c['prototype'],c['capacity'])for c in trace['initial']['controllers']],[(1,16),(0,31)])
    self.assertTrue(all(not(row['after']['flags']&0x1000) for row in r))
    self.assertEqual(r[59]['after']['frame'],0);self.assertEqual(r[119]['after']['frame'],0)
    self.assertTrue(all(len(row['before']['pulse_inputs'])==2 for row in r))
    self.invoke('poison_state_probe.py',native/'trace.json','--output',compare)
    proof=json.loads((compare/'manifest.json').read_text());self.assertEqual(proof['status'],'pass',proof)
    self.assertGreater(proof['checks'],80000);self.assertEqual(proof['error_count'],0)

if __name__=='__main__':unittest.main()
