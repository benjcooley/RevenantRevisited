"""Exact YAttack profile, complete default renderer and real owner lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class YAttackTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_authored_keys_and_profile_controls(self):
  with tempfile.TemporaryDirectory(prefix='yattack-pose-')as tmp:
   out=Path(tmp);self.invoke('yattack_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],15795);self.assertEqual(r['matrix_fields'],28080)
   self.assertIn('profile_mutations=3919',r['controls']);self.assertIn('default_unchanged=1755',r['controls']);self.assertIn('inclusive_changed=1716',r['controls'])
 def test_owner_and_complete_normal_lit_draws(self):
  with tempfile.TemporaryDirectory(prefix='yattack-owner-')as tmp:
   out=Path(tmp);self.invoke('yattack_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());r=trace['records'];self.assertFalse(r[43]['after']['flags']&0x1000);self.assertTrue(r[44]['after']['flags']&0x1000)
   self.assertEqual(len(r[14]['draws']),39);self.assertEqual(trace['initial']['controllers'],[dict(vtable='0x5a357c',state=0,tag_frame=11,blend=80)])
   for draw in r[14]['draws']:
    self.assertEqual(draw['arguments'][1],0x112)
    self.assertEqual(draw['states']['22'],1);self.assertEqual(draw['states']['7'],0);self.assertEqual(draw['states']['14'],0)
   self.assertTrue(all(not(row['draws'])for row in r[44:]));self.assertEqual(r[-1]['after']['rng_draws'],0)
 def test_own_original_oversized_primitive_control(self):
  with tempfile.TemporaryDirectory(prefix='yattack-bounds-')as tmp:
   out=Path(tmp);self.invoke('yattack_software_bounds_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'proof.json').read_text());self.assertEqual(r['capture'],14);self.assertGreater(r['controls'][1]['pixels'],r['controls'][0]['pixels'])
if __name__=='__main__':unittest.main()
