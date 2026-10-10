"""Native empty-selection fallback and strict compiled production grammar."""
import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest
import zipfile

import scrolltex_water_probe as probe
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_EIP


class NativeMultipleObjects(probe.ScrollFixture):
    def __init__(self, image):
        super().__init__(image)
        v = self.vm
        self.drawobjects = [self.drawobj, v.allocate(0x200), v.allocate(0x200)]
        self.multi_verts = [self.drawverts, v.allocate(4*32), v.allocate(4*32)]
        image_rows = v.allocate(12)
        draw_rows = v.allocate(12)
        v.put_u32(self.context+0x74, image_rows)
        v.put_u32(self.context+0x64, 3)
        v.put_u32(self.animator+0x44, 3)
        v.put_u32(self.animator+0x54, draw_rows)
        for i, name in enumerate(('first', 'second', 'third')):
            image_obj = v.allocate(0x48)
            v.write(image_obj, name.encode()+b'\0')
            v.put_u32(image_rows+4*i, image_obj)
            v.put_u32(draw_rows+4*i, self.drawobjects[i])
            v.put_u32(self.drawobjects[i]+0xa0, 4)
            v.put_u32(self.drawobjects[i]+0xa4, self.multi_verts[i])
            for vertex in range(4):
                v.write(self.multi_verts[i]+vertex*32,
                    struct.pack('<8f', 0,0,0,0,0,1,.125*vertex,.25*i))

    def boundary(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if address == 0x40eef0:
            index = self.vm.u32(sp+4)
            if index not in (0,1,2):
                raise AssertionError('Unexpected fixture object')
            uc.reg_write(UC_X86_REG_EAX, self.drawobjects[index])
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp+4+(4 if address == 0x40eef0 else 8))


class ScrollTexFallback(unittest.TestCase):
    def test_native_unmatched_selector_falls_back_to_every_object(self):
        image = Path(os.environ.get('RETAIL_RUNTIME_EXE',
            str(probe.ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')))
        f = NativeMultipleObjects(image)
        v = f.vm
        text = b'obj=water,du=0,dv=0.01\0'
        pointer = v.allocate(len(text))
        v.write(pointer, text)
        controller = v.call(0x4059e0, (0,0,f.animator,f.context,f.owner))
        self.assertEqual(v.call(0x401820,(pointer,),this=controller),1)
        self.assertEqual(f.pre_fallback,0)
        self.assertEqual(v.u32(controller+0x18),3)
        v.put_u32(0x65cb38,24)
        v.call(0x401990,this=controller)
        for i, vertices in enumerate(f.multi_verts):
            for vertex in range(4):
                u, value = struct.unpack('<2f',v.uc.mem_read(vertices+vertex*32+24,8))
                self.assertEqual(u,.125*vertex)
                self.assertAlmostEqual(value,.25*i+.01*24,places=6)

    def test_compiled_initializer_keeps_strict_grammar_and_per_object_overlap(self):
        profiles = json.loads(Path(probe.__file__).with_name('scrolltex_water_profiles.json').read_text())['profiles']
        with zipfile.ZipFile(probe.ROOT/'data/imagery.rvi') as z:
            names = {n.lower():n for n in z.namelist()}
            row = profiles[0]
            metadata = {row['name']:probe.parse_asset(z.read(names[('Imagery/'+row['asset']).lower()]),row)}
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            probe.build_controller(output,metadata)
            prefix = (output/'controller-current.cpp').read_text().split('int main(){')[0]
            checks = r'''
#include <cassert>
int main(){T3DImagery image;image.objects.rows={{"first"},{"second"},{"third"}};
 S3DImageryBody body;body.tags={{0,0,{"scrolltex"},{"obj=legacy_name,du=0,dv=.01"}}};
 image.InitializeScrollTexTracks(&body);assert(image.scrolltex_tracks.size()==3);
 for(int i=0;i<3;++i){float a[2],b[2];assert(image.scrolltex_tracks[i].object==i);
  assert(image.ScrollTexOffset(i,0,0,24,a));assert(image.ScrollTexOffset(i,0,0,24,b));
  assert(!memcmp(a,b,sizeof a));assert(a[0]==0&&a[1]==.01f*24);}
 body.tags={{0,0,{"scrolltex"},{"obj=second,du=0,dv=.02"}}};
 image.InitializeScrollTexTracks(&body);assert(image.scrolltex_tracks.size()==1&&image.scrolltex_tracks[0].object==1);
 for(const char*bad:{"obj=(second),du=0,dv=.01","obj=second,du=0","obj=second,du=nan,dv=0",
  "obj=second,du=1e999,dv=0","obj=second,du=0,dv=.01,other=1","obj=second,du=0,dv=.01,du=1",
  "obj=second,du=0,dv=.01,","obj=second*,du=0,dv=.01"}){
  body.tags={{0,0,{"scrolltex"},{bad}}};image.InitializeScrollTexTracks(&body);assert(image.scrolltex_tracks.empty());}
 body.numtags=2;body.tags={{0,0,{"scrolltex"},{"obj=legacy_name,du=0,dv=.01"}},
  {0,0,{"scrolltex"},{"obj=second,du=0,dv=.02"}}};image.InitializeScrollTexTracks(&body);
 assert(image.scrolltex_tracks.size()==3);for(auto&t:image.scrolltex_tracks)assert(t.dv==.01f);
 body.numtags=1;body.tags={{0,5,{"scrolltex"},{"obj=second,du=0,dv=.01"}}};image.InitializeScrollTexTracks(&body);
 float uv[2];assert(!image.ScrollTexOffset(1,0,4,24,uv)&&uv[0]==0&&uv[1]==0);
 assert(image.ScrollTexOffset(1,0,5,24,uv));assert(!image.ScrollTexOffset(1,1,5,24,uv));
 body.tags={{0,0,{"animtex"},{"obj=second,u=4,d=4"}}};image.InitializeScrollTexTracks(&body);assert(image.scrolltex_tracks.empty());}
'''
            source = output/'regression.cpp'
            source.write_text(prefix+checks)
            binary = output/'regression'
            subprocess.run(['clang++','-std=c++17',str(source),'-o',str(binary)],check=True,capture_output=True)
            subprocess.run([str(binary)],check=True,capture_output=True)


if __name__=='__main__':unittest.main()
