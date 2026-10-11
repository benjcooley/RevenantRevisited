"""Original moving three-state LightStrip versus retained production controls."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT

class LightStripTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=90)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_original_three_state_pose_and_admission(self):
  with tempfile.TemporaryDirectory(prefix='lightstrip-poses-')as tmp:
   out=Path(tmp);self.invoke('lightstrip_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],8190);self.assertEqual(r['matrix_fields'],12656)
   self.assertIn('default_unchanged=910',r['controls']);self.assertIn('profile_mutations=1307',r['controls'])
 def test_full_native_pool_rng_and_skipped_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='lightstrip-state-')as tmp:
   for stride in (1,3):
    out=Path(tmp)/str(stride);native=out/'native';compare=out/'comparison'
    self.invoke('lightstrip_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--render-stride',stride,'--state-only')
    trace=json.loads((native/'trace.json').read_text());r=trace['records']
    self.assertEqual([(i,r[i]['before']['state'],r[i]['before']['frame'],r[i]['before']['position'][0])for i in(9,27,28,57)],[(9,1,0,-180),(27,1,18,180),(28,2,0,200),(57,2,28,200)])
    self.assertTrue(r[57]['after']['flags']&0x1000)
    self.invoke('lightstrip_state_probe.py',native/'trace.json','--output',compare)
    proof=json.loads((compare/'manifest.json').read_text());self.assertEqual(proof['status'],'pass',proof)
    self.assertGreater(proof['checks'],330000);self.assertEqual(proof['error_count'],0)

if __name__=='__main__':unittest.main()
