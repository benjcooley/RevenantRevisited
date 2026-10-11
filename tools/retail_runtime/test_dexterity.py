"""Dexterity's full owner clock and explicit loop-control negative case."""
import struct,unittest,zipfile
from dexterity_visual_reference import NativeDexterity,ROOT
class DexterityTests(unittest.TestCase):
 def test_original_owner_completion_and_loop_control(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/dexterity.i3d'))
  for looping in (True,False):
   f=NativeDexterity(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);r=f.initialize_scene();self.assertEqual(r['controllers'],['0x5a3544']);v=f.vm
   if not looping:
    method=v.u32(v.u32(f.imagery)+0x8c);v.write_code(method,b'\xb8'+struct.pack('<I',0x2000)+b'\xc2\x04\x00')
   for tick in range(1,61):
    f.advance();self.assertEqual(bool(v.u32(f.owner+8)&0x1000),not looping and tick==60)
   frame=struct.unpack('<h',v.uc.mem_read(f.owner+0x5c,2))[0];self.assertEqual(frame,0 if looping else 59)
if __name__=='__main__':unittest.main()
