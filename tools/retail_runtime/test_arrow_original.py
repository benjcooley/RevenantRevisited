"""Distinct arrow factories and QueenArrow moving pulse/Render cadence regression."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class ArrowTests(unittest.TestCase):
 def invoke(self,tool,*args):
  p=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=100)
  self.assertEqual(p.returncode,0,p.stdout+p.stderr)
 def test_distinct_complete_native_factories(self):
  with tempfile.TemporaryDirectory(prefix='arrow-factories-')as tmp:
   out=Path(tmp);self.invoke('arrow_native_preflight.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   d=json.loads((out/'manifest.json').read_text());self.assertEqual([r['color']for r in d['bindings']],[0,2]);self.assertEqual(d['base_calls'],2)
 def test_original_motion_collision_kill_and_render_rng_at_two_cadences(self):
  with tempfile.TemporaryDirectory(prefix='queen-arrow-')as tmp:
   for stride in(1,3):
    out=Path(tmp)/str(stride);native=out/'native';self.invoke('arrow_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--frames',24,'--state-only','--render-stride',stride,'--resolution',640,480,'--center',320,320)
    rows=json.loads((native/'trace.json').read_text())['records'];self.assertEqual(rows[0]['state']['position'],[-157,0,78]);self.assertEqual(rows[15]['state']['position'],[188,0,78]);self.assertEqual(rows[16]['state']['countdown'],3);self.assertEqual(rows[17]['state']['countdown'],1);self.assertTrue(rows[18]['state']['flags']&4096)
    self.invoke('arrow_core_probe.py',native/'trace.json','--output',out/'source');self.assertEqual(json.loads((out/'source/manifest.json').read_text())['status'],'pass')
if __name__=='__main__':unittest.main()
