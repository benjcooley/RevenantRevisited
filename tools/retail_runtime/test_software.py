"""Original retail machine-code raster contract and warm-reset checks."""
import hashlib
from pathlib import Path
import struct
import unittest
from software_probe import SoftwareFixture


class OriginalSoftwareRaster(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        image=Path(__file__).resolve().parents[2]/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        cls.fixture=SoftwareFixture(image)
        cls.quad=[(x,y,100,31,31,31,31,u,v) for x,y,u,v in
            [(8,8,0,0),(50,8,1,0),(50,50,1,1),(8,50,0,1)]]
        cls.indices=[0,1,2,0,2,3]

    def test_original_rgb565_texture_and_depth_policies_replay(self):
        f=self.fixture
        texels=([0xf800,0xf800,0x07e0,0x07e0]*2+[0x001f,0x001f,0xffff,0xffff]*2)
        f.set_texture(4,4,struct.pack('<16H',*texels));f.clear();f.checkpoint()
        for z_enabled,z_write,initial,visible,z_inside in [
            (False,False,65535,True,65535),(True,True,600,True,100),
            (True,False,600,True,600),(True,True,50,False,50)]:
            with self.subTest(z_enabled=z_enabled,z_write=z_write,initial=initial):
                first=None
                for _ in range(3):
                    f.restore();f.clear(depth=initial)
                    color,depth=f.draw(self.quad,self.indices,z_enabled,z_write)
                    values=set(struct.unpack('<4096H',color))
                    self.assertEqual(values,{0,0xf800,0x07e0,0x001f,0xffff} if visible else {0})
                    self.assertEqual(struct.unpack_from('<H',depth,2*(20*64+20))[0],z_inside)
                    self.assertEqual(f.com_trace,['Lock','Unlock'])
                    pair=(hashlib.sha256(color).digest(),hashlib.sha256(depth).digest())
                    if first is None:first=pair
                    else:self.assertEqual(pair,first)

    def test_original_argb4444_quantization_and_minimum_alpha(self):
        f=self.fixture
        for texel,expected in [(0x0f00,0x081e),(0x8f00,0x800e),(0xff00,0xf000)]:
            with self.subTest(texel=texel):
                f.set_texture(4,4,struct.pack('<16H',*[texel]*16),format='ARGB4444')
                f.clear(color=31);f.checkpoint()
                for _ in range(3):
                    f.restore();color,depth=f.draw(self.quad,self.indices)
                    self.assertEqual(struct.unpack_from('<H',color,2*(20*64+20))[0],expected)
                    self.assertEqual(set(struct.unpack('<4096H',depth)),{65535})

    def test_original_projection_basis(self):
        f=self.fixture;f.restore()
        points=f.project([(0,0,0),(20,0,0),(0,20,0),(0,0,20)])
        self.assertEqual(points[0],(32.,32.,2750.))
        self.assertAlmostEqual(points[1][0],52.20305252,places=5)
        self.assertAlmostEqual(points[2][0],11.79694748,places=5)
        self.assertAlmostEqual(points[3][1],7.25641632,places=5)


if __name__=='__main__':unittest.main()
