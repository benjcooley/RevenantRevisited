"""Exact Maelstrom resource, native controller dispatch, hidden objects and lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class MaelstromTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=180)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_profile_and_native_pose(self):
  with tempfile.TemporaryDirectory(prefix='maelstrom-pose-')as tmp:
   out=Path(tmp);self.invoke('maelstrom_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],112455);self.assertEqual(r['matrix_fields'],77504)
   self.assertIn('profile_mutations=8179',r['controls']);self.assertIn('default_unchanged=12495',r['controls']);self.assertIn('inclusive_changed=3813',r['controls'])
 def test_actual_owner_controllers_visibility_and_kill(self):
  with tempfile.TemporaryDirectory(prefix='maelstrom-owner-')as tmp:
   out=Path(tmp);self.invoke('maelstrom_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   t=json.loads((out/'trace.json').read_text());r=t['records'];c=t['initial']['controllers']
   self.assertEqual([v['vtable']for v in c],['0x5a3480']*3+['0x5a357c'])
   self.assertEqual([v['objects']for v in c[:3]],[[4],[2],[3]]);self.assertEqual(c[3]['blend'],16)
   self.assertFalse(r[243]['after']['flags']&0x1000);self.assertTrue(r[244]['after']['flags']&0x1000)
   self.assertEqual(r[-1]['after']['clock'],260);self.assertEqual(r[-1]['after']['rng_draws'],0)
   self.assertTrue(all(not x['draws']for x in r[244:]))
   for x in r:
    for d in x['draws']:
     self.assertNotIn(d['object'],range(20,26));self.assertEqual(d['arguments'][1],0x112)
     self.assertEqual(d['states']['22'],1);self.assertEqual(d['states']['7'],0 if d['object']==0 else 1);self.assertEqual(d['states']['14'],0)
 def test_global_scroll_clock_matches_full_native_render(self):
  with tempfile.TemporaryDirectory(prefix='maelstrom-scroll-')as tmp:
   out=Path(tmp);self.invoke('maelstrom_scroll_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'proof.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(r['uv_fields'],2304)
   self.assertEqual(r['max_uv_error'],0);self.assertEqual({c['owner_frame']for c in r['cases']},{1})
if __name__=='__main__':unittest.main()
