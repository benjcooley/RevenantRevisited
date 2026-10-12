"""Independent exact ARGB4444 blend and declared raster-center controls."""
import struct,unittest
from argb4444_device_raster import rgba4444,source_alpha,emissive_saturation,raster
MATERIAL=struct.pack('<I17f2I',80,*([1.]*16),0.,0,16)
class FireDeviceTests(unittest.TestCase):
 def test_all65536_words_source_alpha(self):
  backgrounds=((0,0,0),(31,127,241),(255,255,255),(1,1,1))
  checks=0
  for word in range(65536):
   alpha=word>>12;channels=[(word>>shift)&15 for shift in(8,4,0)]
   for destination in backgrounds:
    got=source_alpha(rgba4444(word),destination)
    want=tuple((2*(n*17*alpha+d*(15-alpha))+15)//30 for n,d in zip(channels,destination))
    self.assertEqual(got,want,(hex(word),destination));checks+=3
  self.assertEqual(checks,786432)
 def test_original_material_saturation_and_rejections(self):
  self.assertEqual(emissive_saturation(MATERIAL),(1.,)*4)
  for i in range(16):
   changed=bytearray(MATERIAL);struct.pack_into('<f',changed,4+i*4,.5)
   with self.assertRaises(AssertionError):emissive_saturation(changed)
 def packet(self,word=0x8fff):
  return dict(projected=[[0,0,1],[4,0,1],[4,4,1],[0,4,1]],vertices_hex=b''.join(struct.pack('<8f',0,0,0,0,0,1,.5,.5)for _ in range(4)).hex(),indices=[0,1,2,0,2,3],texture_width=2,texture_height=2,texture_hex=struct.pack('<4H',*[word]*4).hex(),material_hex=MATERIAL.hex())
 def test_top_left_alpha_zero_and_integer_half_centers(self):
  for center in(0.,.5):
   for mode in('point','linear'):
    raw=raster([self.packet()],mode,8,8,pixel_center=center)
    self.assertEqual(max(raw),136);self.assertEqual(sum(v!=0 for v in raw),48)
    self.assertEqual(set(raster([self.packet(0x0fff)],mode,8,8,pixel_center=center)),{0})
  p=self.packet();p['projected']=[[x+.25,y+.25,z]for x,y,z in p['projected']]
  integer=raster([p],'point',8,8,pixel_center=0.);half=raster([p],'point',8,8,pixel_center=.5)
  self.assertEqual(tuple(integer[:3]),(0,0,0));self.assertEqual(tuple(half[:3]),(136,136,136));self.assertNotEqual(integer,half)
 def test_api_center_translation_identity(self):
  p=self.packet(0x7731);direct=raster([p],'point',8,8,pixel_center=0.)
  translated=self.packet(0x7731);translated['projected']=[[x+.5,y+.5,z]for x,y,z in translated['projected']]
  self.assertEqual(direct,raster([translated],'point',8,8,pixel_center=.5))
if __name__=='__main__':unittest.main()
