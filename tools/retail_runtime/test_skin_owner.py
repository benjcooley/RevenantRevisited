"""Each skin's original owner loop and no-loop negative control."""
import struct,unittest,zipfile
from skin_visual_reference import NativeSkin,PROFILES,ROOT
class SkinOwnerTests(unittest.TestCase):
 def test_original_loop_and_no_loop_control(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
   for name,profile in PROFILES.items():
    data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+profile['asset']+'.i3d'))
    for looping in (True,False):
     f=NativeSkin(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name);f.initialize_scene();v=f.vm
     if not looping:
      method=v.u32(v.u32(f.imagery)+0x8c);v.write_code(method,b'\xb8'+struct.pack('<I',0x2000)+b'\xc2\x04\x00')
     for tick in range(1,31):
      f.advance();self.assertEqual(bool(v.u32(f.owner+8)&0x1000),not looping and tick==30)
     self.assertEqual(struct.unpack('<h',v.uc.mem_read(f.owner+0x5c,2))[0],0 if looping else 29)
if __name__=='__main__':unittest.main()
