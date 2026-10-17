"""Exact PowerUp keyed mesh profile and original owner lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class PowerUpTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_all_keyed_poses_and_negative_profiles(self):
  with tempfile.TemporaryDirectory(prefix='powerup-poses-')as tmp:
   out=Path(tmp);self.invoke('powerup_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],12285);self.assertEqual(r['matrix_fields'],7696)
   self.assertIn('profile_mutations=1650',r['controls']);self.assertIn('default_unchanged=1365',r['controls']);self.assertIn('inclusive_changed=494',r['controls'])
 def test_original_owner_and_normal_lit_render(self):
  with tempfile.TemporaryDirectory(prefix='powerup-owner-')as tmp:
   out=Path(tmp);self.invoke('powerup_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());r=trace['records']
   self.assertFalse(r[88]['after']['flags']&0x1000);self.assertTrue(r[89]['after']['flags']&0x1000);self.assertEqual(r[89]['after']['frame'],89)
   self.assertTrue(r[29]['draws']);self.assertTrue(all(x['arguments'][1]==0x112 for row in r for x in row['draws']))
   self.assertTrue(all(not(row['draws'])for row in r[89:]))
   self.assertEqual(r[-1]['after']['rng_draws'],0)
if __name__=='__main__':unittest.main()
