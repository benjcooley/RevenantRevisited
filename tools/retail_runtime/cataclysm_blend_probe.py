"""Original RGB565 prelit source-alpha kernel versus all three production shader branches.

This is an arithmetic kernel proof with explicit RGBA5 inputs. Full Cataclysm
owner/normal/prelit reference films independently establish those input colors.
Destination RGB565 blending remains a separate GPU precision limitation.
"""
import argparse,json,struct,subprocess
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
f=LitSoftwareFixture(a.exe,256,128);v=f.vm;v.call(0x417d60,(2,1),this=0x65a57c);records=[]
for texel in [0,0xffff,0x8421,0x1234,0xabcd,0x37e7,0x7bef]:
 f.set_texture(2,2,struct.pack('<4H',*[texel]*4))
 cases=[(color,alpha)for color in [(22,15,3),(7,11,19)]+[(i,i,i)for i in range(32)]for alpha in range(32)]
 for offset in range(0,len(cases),512):
  batch=cases[offset:offset+512];vertices=[];indices=[]
  for i,(color,alpha)in enumerate(batch):
   x=(i%32)*8+1;y=(i//32)*8+1;base=len(vertices)
   vertices.extend((xx,yy,100,*color,alpha,0,0)for xx,yy in[(x,y),(x+6,y),(x+6,y+6),(x,y+6)])
   indices.extend(base+j for j in(0,1,2,0,2,3))
  f.clear();raw,_=f.draw(vertices,indices,instruction_limit=10000000)
  for i,(color,alpha)in enumerate(batch):
   pixel=struct.unpack_from('<H',raw,2*(((i//32)*8+3)*256+(i%32)*8+3))[0]
   records.append([texel,*color,alpha,(pixel>>11)&31,(pixel>>5)&63,pixel&31])
(a.output/'native-literals.json').write_text(json.dumps(records))
source=(ROOT/'src/renderer.cpp').read_text();parts=source.split('// Explicit RGB565 + constant prelit RGBA5 (actual FVF_LVERTEX).')[1:];assert len(parts)==3;functions=[]
for i,part in enumerate(parts):
 begin=part.index('float a5 =');alpha=part[begin:part.index('\n        ',part.index('discard',begin))];alpha=alpha.replace('hp.diffuse','carrier').replace('diffuse','carrier').replace('discard_fragment();','return float3{0,0,0};').replace('discard;','return float3{0,0,0};')
 begin=part.index('texture5 =');begin=part.rfind('\n',0,begin)+1;end=part.index('\n        ',part.index(' / (alpha *',begin));body=part[begin:end].replace('hp.diffuse','carrier').replace('diffuse','carrier').replace('vec3','float3').replace('float3(','make3(')
 functions.append(f'float3 shader{i}(float3 tex,float4 carrier){{'+alpha+body+'return source*alpha*make3(31,63,31);}')
pre=r'''
#include <cmath>
#include <cstdio>
#include <cassert>
#include <algorithm>
using float3=float __attribute__((ext_vector_type(3)));using float4=float __attribute__((ext_vector_type(4)));
float3 make3(float a,float b,float c){return{a,b,c};}
float3 floor(float3 x){return{std::floor(x.x),std::floor(x.y),std::floor(x.z)};}
float clamp(float x,float a,float b){return std::max(a,std::min(x,b));}float3 clamp(float3 x,float a,float b){return{clamp(x.x,a,b),clamp(x.y,a,b),clamp(x.z,a,b)};}
'''
main='int main(){int x[8];int cases=0;while(scanf("%d %d %d %d %d %d %d %d",x,x+1,x+2,x+3,x+4,x+5,x+6,x+7)==8){float3 tex={float(x[0]>>11)/31.f,float((x[0]>>5)&63)/63.f,float(x[0]&31)/31.f};float4 c={float(x[1]),float(x[2]),float(x[3]),float(x[4])};for(auto s:{shader0(tex,c),shader1(tex,c),shader2(tex,c)})for(int i=0;i<3;++i)if(std::abs(s[i]-x[5+i])>=.0001f){fprintf(stderr,"Mismatch tex=%x rgb=%d,%d,%d alpha=%d channel=%d actual=%d shader=%.9g\\n",x[0],x[1],x[2],x[3],x[4],i,x[5+i],s[i]);return 1;}++cases;}printf("PASS %d native literals, three shader branches\\n",cases);}'
cpp=a.output/'shader.cpp';cpp.write_text(pre+'\n'.join(functions)+main);binary=a.output/'shader';subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',str(cpp),'-o',str(binary)],check=True);run=subprocess.run([str(binary)],input='\n'.join(' '.join(map(str,r))for r in records),text=True,capture_output=True);print(run.stdout,run.stderr);assert run.returncode==0
r=dict(status='pass',native_cases=len(records),shader_channel_checks=len(records)*3*3,records=records,executable_sha256=sha(a.exe.read_bytes()),shader_source_sha256=sha(source.encode()),driver_sha256=sha(cpp.read_bytes()),scope=__doc__);(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n')
