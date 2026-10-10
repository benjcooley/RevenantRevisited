"""Original alpha object creation, RenderObject state selection and raster controls.

Decoded imagery is an explicit resource boundary. No whole-scene/lighting claim.
"""
import json
from pathlib import Path
import struct
import unittest
import zipfile

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from colored_ribbon_probe import parse_parts
from software_probe import SoftwareFixture

ROOT = Path(__file__).resolve().parents[2]


class NativeAlphaSceneState(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.f = SoftwareFixture(ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe',32,32)

    def test_original_ribbon_constructor_and_renderobject_select_mode4(self):
        v = self.f.vm
        def blob(data):
            p = v.allocate(len(data)); v.write(p,data); return p
        profiles = json.loads(Path(__file__).with_name('ribbon_profiles.json').read_text())['profiles']
        with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
            names = {n.lower():n for n in archive.namelist()}
            for profile in profiles:
                member = ('Imagery/'+profile['asset'].replace('\\','/')).lower()
                parts = parse_parts(archive.read(names[member]),profile)
                for alpha in (False,True):
                    imagery=v.allocate(0x100); objects=v.allocate(8)
                    materials=v.allocate(8); textures=v.allocate(8)
                    for off,value in ((12,1),(0x64,2),(0x74,objects),(0x34,2),
                                      (0x44,materials),(0x4c,2),(0x5c,textures)):
                        v.put_u32(imagery+off,value)
                    for i,part in enumerate(parts):
                        resource=v.allocate(0x48)
                        v.write(resource,part['object_name'].encode()+b'\0')
                        v.put_u32(resource+0x3c,part['material'])
                        counts=[0,0,0]; counts[i+1]=len(part['indices'])//3
                        v.put_u32(resource+0x38,blob(struct.pack('<3I',*counts)))
                        v.put_u32(objects+i*4,resource)
                        # Material values cannot affect this decision: both
                        # outcomes of 409e5e..409eaf join mode4 at409eb1.
                        v.put_u32(materials+i*4,v.allocate(0x58))
                        texture=v.allocate(0x98)
                        v.put_u32(texture+0x64,int(alpha))
                        v.put_u32(textures+i*4,texture)
                        obj=v.call(0x409ca0,(i,0),this=imagery)
                        self.assertEqual(v.u32(obj+0x348),4 if alpha else 0)
                        self.assertEqual(v.u32(obj)&0x1000000,0x1000000 if alpha else 0)
                        if not alpha: continue
                        for address,value in ((0x5d7a28,1),(0x66818c,0),
                            (0x5e8790,0),(0x5c61ac,1),(0x5e91c0,0)):
                            v.put_u32(address,value)
                        states={}
                        def observe(uc,address,size,user):
                            sp=uc.reg_read(UC_X86_REG_ESP)
                            states[v.u32(sp+4)]=v.u32(sp+8)
                        h=v.uc.hook_add(UC_HOOK_CODE,observe,begin=0x417060,end=0x417060)
                        try:
                            # Literal RenderObject local frame: constructed
                            # object pointer, stop immediately before DrawPrimitive.
                            v.put_u32(v.call_sp+0xc8,obj)
                            v.call(0x40adfc,stop_address=0x40aeba)
                        finally: v.uc.hook_del(h)
                        self.assertEqual(states,{14:0,7:1,22:1,19:5,20:6,21:2})

    def test_native_mode4_blends_reverse_faces_and_rear_layers(self):
        f=self.f; v=f.vm
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e8790,0),(0x5c61ac,1)):
            v.put_u32(address,value)
        v.call(0x417d60,(4,1),this=0x65a57c)
        f.set_texture(2,2,struct.pack('<4H',*[0x8fff]*4),format='ARGB4444')
        def quad(z): return [(x,y,z,31,31,31,31,0,0) for x,y in ((8,8),(24,8),(24,24),(8,24))]
        center=lambda output:struct.unpack_from('<H',output,2*(16*32+16))[0]
        front=[0,1,2,0,2,3]; back=[2,1,0,3,2,0]
        for winding in (front,back):
            f.clear()
            first,depth=f.draw(quad(100),winding,z_enabled=True,z_write=False)
            second,depth2=f.draw(quad(110),winding,z_enabled=True,z_write=False)
            self.assertEqual(center(first),0x8430)
            self.assertGreater(center(second),center(first))
            self.assertEqual(depth,depth2)
            self.assertEqual(center(depth),0xffff)
        f.clear()
        first,_=f.draw(quad(100),front,z_enabled=True,z_write=True)
        blocked,_=f.draw(quad(110),front,z_enabled=True,z_write=True)
        self.assertEqual(first,blocked)


if __name__ == '__main__': unittest.main()
