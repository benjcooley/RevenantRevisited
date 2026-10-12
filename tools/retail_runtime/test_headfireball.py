"""Exact headfireball trap caller and independent Hfire controller regression."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class HeadFireBallTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=150)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_exact_asset_keys_and_profile_controls(self):
  with tempfile.TemporaryDirectory(prefix='headfireball-pose-')as tmp:
   p=Path(tmp);self.invoke('headfireball_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',p)
   j=json.loads((p/'manifest.json').read_text());self.assertEqual(j['status'],'pass',j);self.assertEqual(j['key_fields'],4050);self.assertIn('profile_mutations=170',j['controls']);self.assertIn('default_unchanged=450',j['controls'])
 def test_original_trap_spawn_flight_impact_and_kill(self):
  with tempfile.TemporaryDirectory(prefix='headfireball-state-')as tmp:
   p=Path(tmp);self.invoke('headfireball_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',p/'native','--frames',110,'--state-only')
   j=json.loads((p/'native/trace.json').read_text());rows=j['records'];self.assertEqual(j['dispatch']['lookup']['headfireball'],'0x5e8508');self.assertEqual(j['owner_constructor_vtable'],'0x5a85ac');self.assertEqual(len(j['spawn_definitions']),1)
   self.assertEqual([(rows[t]['after']['state'],rows[t]['after']['frame'],rows[t]['after']['position'][1])for t in(17,18,31,32,86)],[(0,18,60),(1,0,52),(1,13,-52),(2,0,-60),(2,54,-60)])
   self.assertTrue(rows[86]['after']['flags']&4096);self.assertEqual(rows[-1]['after']['rng_draws'],892)
   self.assertEqual([(c['function'],c['tick'])for c in j['damage']],[('0x4d3750',32),('0x4d3590',32)])
   for tool,name in [('headfireball_owner_probe.py','owner'),('headfireball_state_probe.py','particles')]:
    self.invoke(tool,p/'native/trace.json','--output',p/name);r=json.loads((p/name/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(json.loads((p/'owner/manifest.json').read_text())['integer_fields'],1100)
   self.assertEqual(json.loads((p/'particles/manifest.json').read_text())['checks'],517990)
if __name__=='__main__':unittest.main()
