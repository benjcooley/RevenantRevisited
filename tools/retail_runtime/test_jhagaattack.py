"""JhagaAttack exact binding, timed owner and original particle/controller proof."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class JhagaAttackTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_own_factory_and_all_state_keys(self):
  with tempfile.TemporaryDirectory(prefix='jhaga-profile-')as tmp:
   p=Path(tmp);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
   self.invoke('jhagaattack_native_preflight.py',exe,'--output',p/'factory');self.assertEqual(json.loads((p/'factory/manifest.json').read_text())['status'],'pass')
   self.invoke('jhagaattack_pose_probe.py',exe,'--output',p/'pose');r=json.loads((p/'pose/manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],4050);self.assertEqual(r['matrix_fields'],5568);self.assertIn('profile_mutations=170',r['controls']);self.assertIn('default_unchanged=450',r['controls'])
 def test_original_timed_motion_and_both_particle_pools(self):
  with tempfile.TemporaryDirectory(prefix='jhaga-state-')as tmp:
   p=Path(tmp);self.invoke('jhagaattack_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',p/'native','--frames',100,'--state-only')
   j=json.loads((p/'native/trace.json').read_text());rows=j['records'];self.assertEqual(j['dispatch']['lookup']['JhagaAttack'],'0x5e8508')
   self.assertEqual([(rows[t]['after']['phase'],rows[t]['after']['position'][1])for t in(18,19,32,33,87)],[(0,60),(1,52),(1,-52),(2,-60),(2,-60)])
   self.assertTrue(rows[87]['after']['flags']&4096);self.assertEqual(rows[-1]['after']['rng_draws'],796)
   for tool,name in [('jhagaattack_owner_probe.py','owner'),('jhagaattack_state_probe.py','particles')]:
    self.invoke(tool,p/'native/trace.json','--output',p/name);r=json.loads((p/name/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(json.loads((p/'particles/manifest.json').read_text())['checks'],847268)
if __name__=='__main__':unittest.main()
