"""Original simultaneous Comet controllers versus retained production controls."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT

class MeteorStormTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=90)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_original_single_state_pose_and_admission(self):
  with tempfile.TemporaryDirectory(prefix='meteorstorm-poses-')as tmp:
   out=Path(tmp);self.invoke('meteorstorm_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],3240);self.assertEqual(r['matrix_fields'],4400)
   self.assertIn('default_unchanged=360',r['controls'])
 def test_original_primitive_span_boundaries(self):
  with tempfile.TemporaryDirectory(prefix='comet-bounds-')as tmp:
   out=Path(tmp);self.invoke('meteorstorm_software_bounds_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(r['checks'],20);self.assertEqual(r['authored_triangle_checks'],58)
 def test_full_native_pool_rng_and_skipped_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='meteorstorm-state-')as tmp:
   for stride in (1,3):
    out=Path(tmp)/str(stride);native=out/'native';compare=out/'comparison'
    self.invoke('meteorstorm_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--render-stride',stride,'--state-only')
    trace=json.loads((native/'trace.json').read_text());r=trace['records']
    self.assertEqual([(c['prototype'],c['capacity'])for c in trace['initial']['controllers']],[(0,125),(5,20),(2,112)])
    self.assertFalse(r[43]['after']['flags']&0x1000)
    self.assertTrue(r[44]['after']['flags']&0x1000)
    self.assertEqual(r[44]['after']['frame'],44)
    self.assertTrue(all(len(row['before']['pulse_inputs'])==3 for row in r[:44]))
    self.invoke('meteorstorm_state_probe.py',native/'trace.json','--output',compare)
    proof=json.loads((compare/'manifest.json').read_text());self.assertEqual(proof['status'],'pass',proof)
    self.assertGreater(proof['checks'],300000);self.assertEqual(proof['error_count'],0)

if __name__=='__main__':unittest.main()
