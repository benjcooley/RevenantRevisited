"""Original Funnel exact profile, lifecycle, cached emitter and two-controller proof."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class FunnelTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=100)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_original_three_state_pose_and_admission(self):
  with tempfile.TemporaryDirectory(prefix='funnel-poses-')as tmp:
   out=Path(tmp);self.invoke('funnel_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],3600);self.assertEqual(r['matrix_fields'],6304);self.assertIn('default_unchanged=400',r['controls'])
 def test_original_base_winding_and_scene_policy(self):
  with tempfile.TemporaryDirectory(prefix='funnel-winding-')as tmp:
   out=Path(tmp);self.invoke('funnel_draw_policy_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   self.assertEqual(json.loads((out/'manifest.json').read_text())['status'],'pass')
 def test_original_owner_and_both_pools_at_full_and_skipped_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='funnel-state-')as tmp:
   for stride in(1,3):
    out=Path(tmp)/str(stride);native=out/'native';self.invoke('funnel_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--frames',145,'--state-only','--render-stride',stride)
    d=json.loads((native/'trace.json').read_text());r=d['records'];self.assertEqual(d['registry_lookup'],'0x66cf70')
    self.assertEqual([(i,r[i]['after']['state'],r[i]['after']['frame'],r[i]['after']['position'][0])for i in(9,99,100,129,139)],[(9,1,0,-180),(99,1,0,180),(100,1,1,184),(129,2,0,296),(139,2,9,296)])
    self.assertEqual(r[100]['after']['damage_events'],[101]);self.assertTrue(r[139]['after']['flags']&4096)
    for tool,name in[('funnel_state_probe.py','pools'),('funnel_owner_probe.py','owner')]:
     self.invoke(tool,native/'trace.json','--output',out/name);report=json.loads((out/name/'manifest.json').read_text());self.assertEqual(report['status'],'pass',report)
    self.assertGreater(json.loads((out/'pools/manifest.json').read_text())['checks'],108000)
if __name__=='__main__':unittest.main()
