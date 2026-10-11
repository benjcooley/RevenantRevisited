"""Exact GVortex profile, complete default renderer and real owner lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class GVortexTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_authored_keys_and_profile_controls(self):
  with tempfile.TemporaryDirectory(prefix='gvortex-pose-')as tmp:
   out=Path(tmp);self.invoke('gvortex_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],25920);self.assertEqual(r['matrix_fields'],22560)
   self.assertIn('profile_mutations=5680',r['controls']);self.assertIn('default_unchanged=2880',r['controls']);self.assertIn('inclusive_changed=1407',r['controls'])
 def test_owner_and_complete_normal_lit_draws(self):
  with tempfile.TemporaryDirectory(prefix='gvortex-owner-')as tmp:
   out=Path(tmp);self.invoke('gvortex_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());r=trace['records'];self.assertFalse(r[238]['after']['flags']&0x1000);self.assertTrue(r[239]['after']['flags']&0x1000)
   self.assertEqual(len(r[19]['draws']),5);self.assertEqual(trace['initial']['controllers'],[dict(vtable='0x5a357c',state=0,tag_frame=2,blend=16)])
   for draw in r[19]['draws']:
    self.assertEqual(draw['arguments'][1],0x112)
    self.assertEqual(draw['states']['22'],1);self.assertEqual(draw['states']['7'],0 if draw['object']>=8 else 1);self.assertEqual(draw['states']['14'],0)
   self.assertTrue(all(not(row['draws'])for row in r[239:]));self.assertEqual(r[-1]['after']['rng_draws'],0)
if __name__=='__main__':unittest.main()
