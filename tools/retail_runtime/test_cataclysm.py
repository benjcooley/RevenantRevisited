"""Original Cataclysm complete custom pool, owner cadence and render contract."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class CataclysmTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=180)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_complete_pool_owner_and_render_cadence(self):
  with tempfile.TemporaryDirectory(prefix='cataclysm-')as tmp:
   out=Path(tmp);traces=[]
   for stride in(1,3):
    native=out/str(stride);self.invoke('cataclysm_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--state-only','--render-stride',stride)
    trace=json.loads((native/'trace.json').read_text());traces.append(trace);r=trace['records']
    self.assertEqual(trace['initial']['rng_draws'],673);self.assertEqual(r[-1]['state']['rng_draws'],826)
    self.assertEqual(r[19]['state']['phase'],2);self.assertEqual(r[20]['state']['phase'],3)
    self.assertFalse(r[79]['state']['flags']&0x1000);self.assertTrue(r[80]['state']['flags']&0x1000);self.assertEqual(r[80]['state']['life'],80)
    self.invoke('cataclysm_state_probe.py','--trace',native/'trace.json','--output',native/'source')
    proof=json.loads((native/'source/manifest.json').read_text());self.assertEqual(proof['checks'],110565);self.assertEqual(proof['checks'],proof['exact'])
    if stride==1:
     self.invoke('cataclysm_geometry_probe.py','--trace',native/'trace.json','--output',native/'geometry')
     self.assertEqual(len(r[0]['draws']),5)
     self.assertEqual(r[0]['draws'][-1]['states']['19'],2)
     self.assertEqual(r[0]['draws'][-1]['states']['22'],1)
     self.assertEqual(r[20]['draws'][0]['object'],2)
     self.assertEqual(r[20]['draws'][0]['arguments'][1],0x1e2)
   self.assertEqual(traces[0]['initial'],traces[1]['initial'])
   self.assertEqual([r['state']for r in traces[0]['records']],[r['state']for r in traces[1]['records']])
 def test_original_allocator_and_existing_fixture_formats(self):
  with tempfile.TemporaryDirectory(prefix='cataclysm-fixture-')as tmp:
   self.invoke('cataclysm_fixture_controls.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',tmp)
 def test_native_textured_prelit_shader_arithmetic(self):
  with tempfile.TemporaryDirectory(prefix='cataclysm-shader-')as tmp:
   self.invoke('cataclysm_blend_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',tmp)
if __name__=='__main__':unittest.main()
