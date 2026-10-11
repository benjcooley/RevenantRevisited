"""Exact YPowerUp keyed mesh profile and original owner lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class YPowerUpTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_all_keyed_poses_and_negative_profiles(self):
  with tempfile.TemporaryDirectory(prefix='ypowerup-poses-')as tmp:
   out=Path(tmp);self.invoke('ypowerup_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],36135);self.assertEqual(r['matrix_fields'],64240)
   self.assertIn('profile_mutations=3944',r['controls']);self.assertIn('default_unchanged=4015',r['controls']);self.assertIn('inclusive_changed=2005',r['controls'])
 def test_original_owner_and_normal_lit_render(self):
  with tempfile.TemporaryDirectory(prefix='ypowerup-owner-')as tmp:
   out=Path(tmp);self.invoke('ypowerup_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());r=trace['records']
   self.assertFalse(r[71]['after']['flags']&0x1000);self.assertTrue(r[72]['after']['flags']&0x1000);self.assertEqual(r[72]['after']['frame'],72)
   self.assertTrue(r[29]['draws']);self.assertTrue(all(x['arguments'][1]==0x112 for row in r for x in row['draws']))
   self.assertTrue(all(not(row['draws'])for row in r[72:]))
   self.assertEqual(r[-1]['after']['rng_draws'],0)
   self.assertEqual(len(r[24]['draws']),55)
   for row in r:
    for draw in row['draws']:
     self.assertEqual(draw['states']['22'],1) # caller requests CULL_NONE; kernel winding is separate
     self.assertEqual(draw['states']['7'],0) # depth test disabled
     self.assertEqual(draw['states']['14'],0) # depth writes disabled
 def test_original_winding_independence(self):
  with tempfile.TemporaryDirectory(prefix='ypowerup-winding-')as tmp:
   out=Path(tmp);self.invoke('ypowerup_winding_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(len(r['cases']),2)
 def test_original_late_ray_guard_negative_control(self):
  with tempfile.TemporaryDirectory(prefix='ypowerup-bounds-')as tmp:
   out=Path(tmp);self.invoke('ypowerup_software_bounds_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'proof.json').read_text());self.assertEqual(r['status'],'diagnostic_negative_control');self.assertEqual(r['capture'],71)
   self.assertGreater(r['controls'][1]['pixels'],r['controls'][0]['pixels'])
if __name__=='__main__':unittest.main()
