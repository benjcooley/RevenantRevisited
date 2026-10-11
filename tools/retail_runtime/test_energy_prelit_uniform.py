"""Compile all actual packed prelit vertex decoders and compare old varyings."""
import subprocess,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
class PackedPrelit(unittest.TestCase):
 def test_all_packed_colors_all_three_vertex_shaders(self):
  source=(ROOT/'src/renderer.cpp').read_text();marker='// Exact packed20-bit prelit color;';parts=source.split(marker)[1:];self.assertEqual(len(parts),3);functions=[]
  for index,part in enumerate(parts):
   body=part[part.index('\n'):part.index('\n        }')].replace('p.retail_raster.z','packed').replace('retail_raster.z','packed').replace('in.uv.y','weight').replace('i.uv.y','weight').replace('uv.y','weight').replace('p.uv_shift.x','shift').replace('uv_shift.x','shift').replace('o.retail_color','out.rgb').replace('v_retail_color','out.rgb').replace('o.uv.x','out.alpha').replace('v_uv.x','out.alpha').replace('vec3','float3').replace('float3(','make3(')
   functions.append(f'Result decode{index}(float packed,float weight,float shift){{Result out;'+body+'return out;}')
  pre=r'''
#include <cassert>
#include <cstdint>
using uint=uint32_t;typedef float float3 __attribute__((ext_vector_type(3)));
float3 make3(float x,float y,float z){return{x,y,z};}struct Result{float3 rgb;float alpha;};
'''
  main=r'''
int main(){for(uint32_t p=0;p<1048576;++p)for(float weight:{0.f,1.f}){
 const auto a=decode0(float(p),weight,0),b=decode1(float(p),weight,0),c=decode2(float(p),weight,0);
 assert(a.alpha==float((p>>15)&31)*weight&&a.alpha==b.alpha&&b.alpha==c.alpha);
 for(int i=0;i<3;++i){float expected=float((p>>(10-i*5))&31)*weight/31.f;assert(a.rgb[i]==expected&&a.rgb[i]==b.rgb[i]&&b.rgb[i]==c.rgb[i]);}
 // Independent conversion back to the old ARGB8-bearing immutable mesh.
 uint32_t old=((p>>15)<<27)|(((p>>10)&31)<<19)|(((p>>5)&31)<<11)|((p&31)<<3);
 assert(((old>>27)<<15|((old>>19)&31)<<10|((old>>11)&31)<<5|((old>>3)&31))==p);
}}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text('#include <initializer_list>\n'+pre+'\n'.join(functions)+main);subprocess.run(['clang++','-std=c++17','-O2','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
