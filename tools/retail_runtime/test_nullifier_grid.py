"""Compile actual three-backend integer-grid clauses and default controls."""
import subprocess,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
class NullifierGrid(unittest.TestCase):
 def test_three_backend_projection_and_default_controls(self):
  functions=[]
  for backend in ('metal','glsl','hlsl'):
   source=(ROOT/f'src/shaders/fx.{backend}.h').read_text();start=source.index('    // Native software RGB565 packets truncate XYZ');end=source.index(';',source.index('scene_z_n =',start))+1;body=source[start:end]
   for prefix in ('in.','i.',''):body=body.replace(prefix+'retail_metadata.y','metadata')
   body=body.replace('p.camz.x','near').replace('p.camz.y','span').replace('camz.x','near').replace('camz.y','span')
   functions.append(f'void {backend}(float&spx,float&spy,float&scene_z_n,float scene_z_wu,float near,float span,float metadata,bool prelit_rgba5,bool native_projection){{'+body+'}')
  code='#include <cassert>\n#include <cmath>\n#include <algorithm>\nusing std::trunc;float max(float a,double b){return std::max(a,float(b));}\n'+'\n'.join(functions)+r'''
int main(){for(float x:{-12.75f,-.25f,0.f,.25f,17.9f})for(float y:{-3.5f,0.f,9.75f})for(float z:{.25f,100.75f,65534.9f})for(int mode=0;mode<3;++mode)for(int prelit=0;prelit<2;++prelit){
 float a=x,b=y,c=z/65535.f,d=x,e=y,f=c,g=x,h=y,i=c;
 metal(a,b,c,z,0,65535,float(mode),bool(prelit),mode>0);glsl(d,e,f,z,0,65535,float(mode),bool(prelit),mode>0);hlsl(g,h,i,z,0,65535,float(mode),bool(prelit),mode>0);
 assert(a==d&&a==g&&b==e&&b==h&&c==f&&c==i);bool grid=mode==2||(prelit&&mode>0);assert(a==(grid?std::trunc(x)+.5f:x)&&b==(grid?std::trunc(y)+.5f:y));assert(c==(mode==2?std::trunc(z)/65535.f:z/65535.f));}}
'''
  with tempfile.TemporaryDirectory()as d:
   p=Path(d);(p/'grid.cpp').write_text(code);subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'grid.cpp'),'-o',str(p/'grid')],check=True);subprocess.run([str(p/'grid')],check=True)
 def test_actual_uv_clause_requires_native_and_software(self):
  from speed_profile_contract import function
  source=(ROOT/'src/3dimage.cpp').read_text();body=function(source,'                if (item.retail_software_pixel_grid && native_software &&')
  code=r'''#include <cassert>
#include <cmath>
struct Item{bool retail_software_pixel_grid;float uv[4][2];};
int main(){for(int enabled=0;enabled<2;++enabled)for(int native_software=0;native_software<2;++native_software)for(int sw=0;sw<2;++sw)for(float value:{-.124f,0.f,.509502411f,.994478703f,1.f}){
 struct{bool sw;bool UsesRetailSoftwareMeshLighting(){return sw;}}renderer{bool(sw)};Item item{};item.retail_software_pixel_grid=bool(enabled);int c=0;item.uv[c][0]=value;item.uv[c][1]=value;
 BODY
 float expected=enabled&&native_software&&sw?std::trunc(value*64.f)/64.f:value;assert(item.uv[c][0]==expected&&item.uv[c][1]==expected);}}
'''.replace('BODY',body).replace('#include <cmath>','#include <cmath>\n#include <initializer_list>')
  with tempfile.TemporaryDirectory()as d:
   p=Path(d);(p/'uv.cpp').write_text(code);subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'uv.cpp'),'-o',str(p/'uv')],check=True);subprocess.run([str(p/'uv')],check=True)
if __name__=='__main__':unittest.main()
