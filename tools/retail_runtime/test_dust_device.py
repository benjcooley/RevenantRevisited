"""Qualified device arithmetic and actual compiled Dust submission policy."""
import math, struct, subprocess, tempfile, unittest
from pathlib import Path
from dust_device_reference import modulate, raster
ROOT=Path(__file__).resolve().parents[2]
class DustDeviceTests(unittest.TestCase):
 def test_semantic_modulation_full_byte_domain(self):
  for word in (0,0x20,0x841,0x861,0x1082,0x10a2,0x18c3,0x18e3,0x2104,0x2124,0x2945,0x2965,0x3186,0x31a6,0x39c7,0x39e7,0x4208,0xffff):
   for byte in range(256):
    got=modulate(word,(byte,byte,byte))
    expected=(((word>>11)&31)*byte/(31*255),((word>>5)&63)*byte/(63*255),(word&31)*byte/(31*255))
    for actual,want in zip(got,expected):self.assertAlmostEqual(actual,want,places=14)
  self.assertGreater(modulate(0x4208,(29,22,18))[0],(29>>3)/31*((0x4208>>11)/31))
 def test_top_left_no_duplicate_diagonal_and_zero(self):
  vertices=b''.join(struct.pack('<3fIII2f',0,0,0,0,0xff202020,0,.5,.5)for _ in range(4))
  p=dict(projected=[[0,0,1],[4,0,1],[4,4,1],[0,4,1]],vertices_hex=vertices.hex(),indices=[0,1,2,0,2,3],texture_hex=struct.pack('<4H',*[0xffff]*4).hex(),texture_width=2,texture_height=2)
  for mode in ('point','linear'):
   raw=raster([p],mode,8,8);self.assertEqual(max(raw),32);self.assertEqual(sum(v!=0 for v in raw),16*3)
   p['texture_hex']=bytes(8).hex();self.assertEqual(set(raster([p],mode,8,8)),{0});p['texture_hex']=struct.pack('<4H',*[0xffff]*4).hex()
 def test_actual_packet_color_and_additive_target_rounding(self):
  # Tick8 first original diffuse RGB29/22/18; zero alpha proves ONE ignores it.
  vertices=b''.join(struct.pack('<3fIII2f',0,0,0,0,0x001d1612,0,.5,.5)for _ in range(4))
  p=dict(projected=[[0,0,1],[4,0,1],[4,4,1],[0,4,1]],vertices_hex=vertices.hex(),indices=[0,1,2,0,2,3],texture_hex=struct.pack('<4H',*[0x4208]*4).hex(),texture_width=2,texture_height=2)
  for mode in ('point','linear'):
   raw=raster([p],mode,8,8);self.assertEqual(tuple(raw[:3]),(7,6,5))
   doubled=raster([p,p],mode,8,8);self.assertEqual(tuple(doubled[:3]),(14,12,10))
   old=raster([p],mode,8,8,legacy_color=True);self.assertEqual(tuple(old[:3]),(6,4,4));self.assertNotEqual(raw,old)
 def test_compiled_production_policy(self):
  source=(ROOT/'src/3dimage.cpp').read_text();start=source.index('            const bool retail_rgb5_color =');end=source.index('            // Actual SW LV',start);body=source[start:end]
  names='combat_native might immortalmight fmastery speed static_particles head jhaga lightstrip funnel comet fgeyser dust poison'.split()
  code='#include <cmath>\n#include <cstdio>\nint main(){ int checks=0; for(int profile=-1;profile<14;++profile)for(int sw=0;sw<2;++sw)for(int value=0;value<256;++value){\n'
  code+='struct {bool sw;bool UsesRetailSoftwareMeshLighting()const{return sw;}} renderer{bool(sw)};\n'
  code+=''.join(f'bool {name}=profile=={i};\n'for i,name in enumerate(names))
  code+='bool fspray_rgb8=false;\n'
  code+='struct{float color[3];}sample{{float(value),float(value),float(value)}};struct{float color_rgba[4];}item{};\n'+body
  code+='float expected=profile<0 || (dust&&!sw)?float(value)/255.f:float(value>>3)/31.f;for(int c=0;c<3;++c){if(std::fabs(item.color_rgba[c]-expected)>1e-7f)return 1;++checks;}}std::printf("checks=%d\\n",checks);}'
  with tempfile.TemporaryDirectory()as tmp:
   cpp=Path(tmp)/'policy.cpp';exe=Path(tmp)/'policy';cpp.write_text(code)
   subprocess.run(['clang++','-std=c++17',str(cpp),'-o',str(exe)],check=True,capture_output=True)
   result=subprocess.run([str(exe)],check=True,capture_output=True,text=True);self.assertIn('checks=23040',result.stdout)
if __name__=='__main__':unittest.main()
