"""Exact YLose profile, original fade Render cache and owner lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class YLoseTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_authored_keys_and_profile_controls(self):
  with tempfile.TemporaryDirectory(prefix='ylose-pose-')as tmp:
   out=Path(tmp);self.invoke('ylose_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],2142);self.assertEqual(r['matrix_fields'],3808)
   self.assertIn('profile_mutations=591',r['controls']);self.assertIn('default_unchanged=238',r['controls']);self.assertIn('inclusive_changed=220',r['controls'])
 def test_original_render_cache_across_skipped_renders(self):
  with tempfile.TemporaryDirectory(prefix='ylose-fade-')as tmp:
   out=Path(tmp);self.invoke('ylose_fade_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(r['fields'],4200)
   self.assertEqual([c['stride']for c in r['cases']],[1,3,5,13,0])
 def test_owner_and_complete_normal_and_faded_draws(self):
  with tempfile.TemporaryDirectory(prefix='ylose-owner-')as tmp:
   out=Path(tmp);self.invoke('ylose_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   r=json.loads((out/'trace.json').read_text())['records'];self.assertFalse(r[12]['after']['flags']&0x1000);self.assertTrue(r[13]['after']['flags']&0x1000)
   self.assertEqual(len(r[6]['draws']),17)
   for draw in r[6]['draws']:
    self.assertEqual(draw['arguments'][1],0x112 if draw['object']in(0,1,12)else 0x1e2)
    self.assertEqual(draw['states']['22'],1);self.assertEqual(draw['states']['7'],0);self.assertEqual(draw['states']['14'],0)
   self.assertTrue(all(not(row['draws'])for row in r[13:]));self.assertEqual(r[-1]['after']['rng_draws'],0)
 def test_own_original_oversized_primitive_control(self):
  with tempfile.TemporaryDirectory(prefix='ylose-bounds-')as tmp:
   out=Path(tmp);self.invoke('ylose_software_bounds_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'proof.json').read_text());self.assertEqual(r['capture'],12);self.assertGreater(r['controls'][1]['pixels'],r['controls'][0]['pixels'])
if __name__=='__main__':unittest.main()
