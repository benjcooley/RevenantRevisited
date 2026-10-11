"""Fairy full original caller/state and source default-stream controls."""
from pathlib import Path
import struct,tempfile,unittest,zipfile
from fairy_state_probe import run,ROOT
from fairy_visual_reference import NativeFairy
class FairyTests(unittest.TestCase):
 def test_full_cached_emitter_state_and_default_stream(self):
  with tempfile.TemporaryDirectory(prefix='fairy-state-')as tmp:r=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['fields'],50804);self.assertTrue(r['default_zero_identical']);self.assertLess(r['max_error'],1e-6)
 def test_end_state_removes_controllers_without_fake_drain(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fairy.i3d'))
  f=NativeFairy(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);f.initialize_scene();v=f.vm;f.render()
  for i in range(5):f.advance();f.render()
  v.write(f.owner+0xc,struct.pack('<h',1));v.put_u32(f.animator+0xc,1);f.state=1
  v.call(0x40df90,(1,),this=f.animator,instruction_limit=5000000)
  self.assertEqual(v.u32(f.animator+0x58),0);f.advance();raw,r=f.render();self.assertEqual(len(r['draws']),1);self.assertEqual(r['draws'][0]['object'],1)
if __name__=='__main__':unittest.main()
