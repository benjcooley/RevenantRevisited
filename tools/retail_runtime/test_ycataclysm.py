"""Own Y registry/pool/render proof; base height conversion is an explicit negative control."""
import json,subprocess,sys,tempfile,unittest,zipfile
from fireball_visual_reference import read_fireball
from pathlib import Path
from speed_profile_contract import ROOT
class YCataclysmTests(unittest.TestCase):
 def invoke(self,tool,*args,success=True):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=180)
  self.assertEqual(q.returncode==0,success,q.stdout+q.stderr)
 def test_own_asset_shared_geometry(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
   base=read_fireball(z.read('Imagery/Magic/cataclysm.i3d'));own=read_fireball(z.read('Imagery/Magic/YCataclysm.I3D'))
  self.assertNotEqual(base['sha256'],own['sha256']);self.assertEqual((base['version'],own['version']),(2,3))
  self.assertEqual(base['face_hex'],own['face_hex'])
  for a,b in zip(base['objects'],own['objects']):
   for key in ('name','material','vertex_count','vertex_hex','texture_runs'):self.assertEqual(a[key],b[key])
 def test_own_registry_complete_pool_and_raw_height(self):
  with tempfile.TemporaryDirectory(prefix='ycataclysm-')as tmp:
   out=Path(tmp);self.invoke('ycataclysm_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());rows=trace['records']
   self.assertEqual(trace['dispatch']['lookup']['YCataclysm'],'0x66d018')
   self.assertEqual(trace['asset_sha256'],'efc43b4c048c899e86e2c5adac697f537380bf143cea756fefa47c016526b1d8')
   self.assertEqual(rows[-1]['state']['life'],80);self.assertTrue(rows[80]['state']['flags']&0x1000);self.assertFalse(rows[79]['state']['flags']&0x1000)
   self.invoke('cataclysm_state_probe.py','--trace',out/'trace.json','--output',out/'source')
   self.invoke('cataclysm_geometry_probe.py','--trace',out/'trace.json','--output',out/'geometry','--raw-local-z')
   self.invoke('cataclysm_geometry_probe.py','--trace',out/'trace.json','--output',out/'wrong-base-height',success=False)
   wrong=json.loads((out/'wrong-base-height/manifest.json').read_text())
   self.assertGreater(wrong['max_matrix_error'],99)
   self.assertEqual({i for e in wrong['errors'] for i,(a,b) in enumerate(zip(e['native'],e['port'])) if abs(a-b)>5e-5},{14})
if __name__=='__main__':unittest.main()
