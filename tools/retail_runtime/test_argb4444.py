"""Executed Blue RGB565 lookup/alpha arithmetic; not full GPU parity."""
from pathlib import Path
import struct
import unittest
from software_probe import SoftwareFixture
from lit_software_probe import LitSoftwareFixture

IMAGE = Path(__file__).resolve().parents[2] / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
QUAD = [(x,y,100,31,31,31,31,0,0) for x,y in ((8,8),(24,8),(24,24),(8,24))]
INDICES = [0,1,2,0,2,3]


def source565(texel):
    return ((texel >> 8 & 15) << 12) | ((texel >> 4 & 15) << 7) | ((texel & 15) << 1)


def blend565(texel, destination):
    source = source565(texel)
    alpha = (texel >> 12) + 1
    result = 0
    for shift, mask in ((11,31),(5,63),(0,31)):
        src, dst = (source >> shift) & mask, (destination >> shift) & mask
        result |= (dst - dst * alpha // 16 + src * alpha // 16) << shift
    return result


class NativeARGB4444(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixture = SoftwareFixture(IMAGE,32,32)

    def test_every_original_lookup_entry(self):
        f = self.fixture
        table = struct.unpack('<65536H', f.vm.uc.mem_read(f.vm.u32(0x675ea0),131072))
        self.assertEqual(table, tuple(source565(i) for i in range(65536)))

    def test_all_alpha_nibbles_color_ramps_and_backgrounds(self):
        f = self.fixture
        for alpha in range(16):
            for color in (0,1,7,8,14,15):
                texel = alpha << 12 | color << 8 | (15-color) << 4 | color
                f.set_texture(2,2,struct.pack('<4H',*[texel]*4),format='ARGB4444')
                for background in (0,0x7bef,0xffff,0x081f):
                    f.clear(color=background)
                    output,_ = f.draw(QUAD,INDICES)
                    with self.subTest(alpha=alpha,color=color,background=background):
                        self.assertEqual(struct.unpack_from('<H',output,2*(16*32+16))[0],blend565(texel,background))
        # Normalized /15 RGBA would discard this pixel and expand white to65535.
        self.assertNotEqual(blend565(0x0fff,0),0)
        self.assertEqual(blend565(0xffff,0),0xf79e)

    def test_actual_scene_additive_rgb565_uses_initialized_camera_tables(self):
        f = LitSoftwareFixture(IMAGE,32,32)
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):
            f.vm.put_u32(address,value)
        f.vm.call(0x417d60,(8,1),this=0x65a57c)
        f.set_texture(2,2,struct.pack('<4H',*[0x8000]*4))
        f.clear(color=0x7bef)
        output,_ = f.draw(QUAD,INDICES,z_enabled=True,z_write=False)
        self.assertEqual(struct.unpack_from('<H',output,2*(16*32+16))[0],0xf3ce)

    def test_additive_rgb565_channel_ramps_and_destination_rounding(self):
        import random
        f = LitSoftwareFixture(IMAGE,32,32)
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):
            f.vm.put_u32(address,value)
        f.vm.call(0x417d60,(8,1),this=0x65a57c)
        rng = random.Random(1)
        for texel in [0xffff,0xf800,0x7e0,0x1f,*[rng.randrange(65536) for _ in range(12)]]:
            f.set_texture(2,2,struct.pack('<4H',*[texel]*4))
            for light in range(32):
                quad = [(x,y,100,light,light,light,31,0,0) for x,y in ((8,8),(24,8),(24,24),(8,24))]
                for background in (0,0x7bef,0xffff,0x1234):
                    f.clear(color=background)
                    output,_ = f.draw(quad,INDICES,z_enabled=True,z_write=False)
                    expected = 0
                    for shift,mask in ((11,31),(5,63),(0,31)):
                        # Green starts with its high five texture bits, then
                        # expands to six bits before the additive table masks.
                        source = ((texel >> (6 if shift == 5 else shift)) & 31) * light // 31
                        source = (source * (2 if shift == 5 else 1)) & ~1
                        destination = ((background >> shift) & mask) & ~1
                        expected |= min(mask,source+destination) << shift
                    with self.subTest(texel=texel,light=light,background=background):
                        self.assertEqual(struct.unpack_from('<H',output,2*(16*32+16))[0],expected)


if __name__ == '__main__': unittest.main()
