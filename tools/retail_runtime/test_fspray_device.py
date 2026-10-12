"""Narrow actual fspray submission policy plus semantic sample-lattice controls."""
import struct,subprocess,tempfile,unittest
from pathlib import Path
from dust_device_reference import raster
ROOT=Path(__file__).resolve().parents[2]
class FsprayDeviceTests(unittest.TestCase):
 def test_actual_exact_profile_policy_full_byte_domain(self):
  source=(ROOT/'src/3dimage.cpp').read_text();start=source.index('            const uint32_t color_type =');end=source.index('            // Actual SW LV',start);body=source[start:end]
  names='combat_native might immortalmight fmastery speed static_particles head jhaga lightstrip funnel comet fgeyser dust poison'.split()
  code='#include <cmath>\n#include <cstdint>\n#include <cstdio>\nint main(){int checks=0;for(int profile=-1;profile<19;++profile)for(int sw=0;sw<2;++sw)for(int value=0;value<256;++value){\n'
  code+='struct{bool sw;bool UsesRetailSoftwareMeshLighting()const{return sw;}}renderer{bool(sw)};struct Owner{uint32_t id;uint32_t ObjId()const{return id;}}owner{profile==14||profile==18?0x0c052638u:profile==15?0x10ac03deu:profile==16?0xe0a3bc43u:profile==17?0xad92bd38u:0xdeadu};auto*inst=&owner;\n'
  code+=''.join(f'bool {name}=profile=={i}'+(' || (profile>=14&&profile<=17)'if name=='static_particles'else'')+';\n'for i,name in enumerate(names))
  code+='struct{float color[3];}sample{{float(value),float(value),float(value)}};struct{float color_rgba[4];}item{};\n'+body
  code+='float expected=profile<0||profile==18||((dust||(profile>=14&&profile<=17))&&!sw)?float(value)/255.f:float(value>>3)/31.f;for(int c=0;c<3;++c){if(std::fabs(item.color_rgba[c]-expected)>1e-7f)return 1;++checks;}}std::printf("checks=%d\\n",checks);}'
  with tempfile.TemporaryDirectory()as tmp:
   cpp=Path(tmp)/'policy.cpp';exe=Path(tmp)/'policy';cpp.write_text(code);subprocess.run(['clang++','-std=c++17',str(cpp),'-o',str(exe)],check=True,capture_output=True)
   result=subprocess.run([str(exe)],check=True,capture_output=True,text=True);self.assertIn('checks=30720',result.stdout)
 def test_modern_lattice_is_explicit_and_default_preserved(self):
  vertices=b''.join(struct.pack('<3fIII2f',0,0,0,0,0x006a7e9b,0,.5,.5)for _ in range(4))
  p=dict(projected=[[.2,.2,1],[4.2,.2,1],[4.2,4.2,1],[.2,4.2,1]],vertices_hex=vertices.hex(),indices=[0,1,2,0,2,3],texture_hex=struct.pack('<4H',*[0xffff]*4).hex(),texture_width=2,texture_height=2)
  for mode in ('point','linear'):
   original=raster([p],mode,8,8);self.assertEqual(original,raster([p],mode,8,8,sample_center=0));modern=raster([p],mode,8,8,sample_center=.5);self.assertNotEqual(original,modern);self.assertEqual(tuple(modern[:3]),(106,126,155));self.assertEqual(sum(v!=0 for v in modern),16*3)
   old=raster([p],mode,8,8,legacy_color=True,sample_center=.5);self.assertEqual(tuple(old[:3]),(107,123,156));self.assertNotEqual(modern,old)
if __name__=='__main__':unittest.main()
