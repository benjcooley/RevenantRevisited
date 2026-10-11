#!/usr/bin/env python3
"""Actual FireSwarm leaf/owner/Illuminate/raster and ARGB primitive-span proof."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS
from fireswarm_probe import FireSwarmFixture
from firecone_owner_probe import native_owner,native_world
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from meteorstorm_software_bounds_probe import NativeBounds
from speed_profile_contract import ROOT,sha
TICKS=(2,15,25,27,28,30,36,48,60,72,74,75,76,80)

def run(exe,out):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/fireswarm.i3d')
 f=FireSwarmFixture(exe,asset);f.objects=[f.obj];f.reset();s=LitSoftwareFixture(exe,1920,1080);v=s.vm;s.lighting((38,38,38),(1,1,1));s.set_texture(64,64,f.mesh['texture'],format='ARGB4444')
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c);guards=[]
 def hook(uc,a,*_):guards.append(dict(address=hex(a),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
 for a in(0x56e006,0x56e036,0x56e05c,0x56e082):v.uc.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
 cases=[];inputs=[];expected=[];states=[];negative=[]
 for tick in range(81):
  if tick:f.animate()
  states.append(f.state())
  if tick not in TICKS:continue
  owner=native_owner(f);f.original_draws();packets=[native_world(f,dict(object=0,matrix=m))for m in f.packets]
  def raster(limits=(640,480)):
   v.write(0x5b9cc0,struct.pack('<f',limits[0]));v.write(0x5b9cbc,struct.pack('<f',limits[1]));s.clear();guards.clear();vertices=[]
   for m in packets:
    _,_,p=s.draw_authored(f.mesh['vertices'],f.mesh['indices'],m,instruction_limit=80000000);vertices+=p
   return v.surface_bytes('screen'),v.surface_bytes('depth'),list(guards),vertices
  image,depth,trace,vertices=raster();assert raster()==(image,depth,trace,vertices)
  rejected=[];face=-1
  for row in trace:
   if row['address']=='0x56e006':face+=1;rejected.append(False)
   rejected[face]|=row['reject']
  if packets:
   assert len(packets)==1 and len(rejected)==192
   for face in range(192):
    indices=f.mesh['indices'][3*face:3*face+3];inputs.append(packets[0]+[c for i in indices for c in f.mesh['vertices'][i][:3]]);expected.append(not rejected[face])
  path=out/f'native-{tick:03d}.png';save_rgb565_png(path,image,1920,1080)
  cases.append(dict(tick=tick,state=f.state(),owner_matrix=owner,matrices=packets,path=str(path.resolve()),sha256=sha(path.read_bytes()),depth_sha256=sha(depth),guard_rejected_faces=[i for i,x in enumerate(rejected)if x],guard_accepted=len(rejected)-sum(rejected),projected_bounds=[min(p[0]for p in vertices),min(-p[1]for p in vertices),max(p[0]for p in vertices),max(-p[1]for p in vertices)]if vertices else None,nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',image))))
  if tick==15:
   wide,wdepth,wtrace,wvertices=raster((6400,4800));assert vertices==wvertices and not any(x['reject']for x in wtrace);assert image!=wide
   p=out/'widened-negative-015.png';save_rgb565_png(p,wide,1920,1080);negative=dict(tick=tick,state=f.state(),matrices=packets,unchanged_transformed_vertices_sha256=sha(repr(vertices).encode()),original_sha256=sha(image),widened_sha256=sha(wide),widened_image_sha256=sha(p.read_bytes()),scope='Only literal640/480 bounds widened10x. Same matrices/vertices/UV/colors/controller state; changed pixels are diagnostic only.')
  print('native',tick,cases[-1]['guard_accepted'],cases[-1]['nonzero_pixels'],flush=True)
 # Actual source helper compiled independently; native comparison remains x86.
 cpp=out/'guard.cpp';cpp.write_text('''#include "cometsoftwarebounds.h"
#include <cstdio>
int main(){hmm_mat4 m;hmm_vec3 p[3];while(scanf("%f",&m.Elements[0][0])==1){for(int i=1;i<16;++i)scanf("%f",(&m.Elements[0][0])+i);for(auto&v:p)scanf("%f%f%f",&v.X,&v.Y,&v.Z);printf("%d\\n",RetailCometTriangleAllowed(p,m));}}
''');binary=out/'guard';cmd=['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(cpp),str(ROOT/'src/math3d.cpp'),'-o',str(binary)];subprocess.run(cmd,check=True);q=subprocess.run([str(binary)],input='\n'.join(' '.join(map(str,row))for row in inputs),text=True,capture_output=True,check=True);actual=list(map(bool,map(int,q.stdout.split())));assert actual==expected
 # Boundary controls exercise the actual ARGB branch at exact/nextfloat limits.
 b=NativeBounds(exe);b.f.set_texture(2,2,b'\xff\xff'*4,format='ARGB4444');points=[[(0,0),(640,0),(0,480)],[(0,0),(640.00006103515625,0),(0,480)],[(0,0),(640,0),(0,480.0000305175781)],[(0,0),(-640,0),(0,-480)],[(0,0),(-640.00006103515625,0),(0,-480)],[(0,0),(0,-480.0000305175781),(640,0)],[(0,0),(0,100),(641,0)],[(0,0),(0,100),(0,481)],[(0,0),(-640,0),(640,0)],[(99,-55),(100,-50),(12,-50)]];points +=[[p[0],p[2],p[1]]for p in points[:]];bounds=[]
 for p in points:
  native=b.test(p);rule=all(abs(p[0][0]-p[i][0])<=640 and abs(p[0][1]-p[i][1])<=480 for i in(1,2));assert native==rule;bounds.append(dict(points=p,native=native,trace=b.calls))
 report=dict(status='reference_complete',type_id='0x582c1e78',asset_sha256=sha(asset),executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),source_guard_sha256=sha((ROOT/'src/cometsoftwarebounds.h').read_bytes()),cases=cases,states=states,bounds=bounds,compiled_authored_guard_checks=len(expected),negative_control=negative,configuration=dict(viewport=[1920,1080],origin=[0,0,0],facing=0,camera=[0,0,0],ambient=[38]*3,directional=[1]*3,scene=2,cull=3,z_test=True,z_write=False,fps=24,warmup=1),scope='Actual original FireSwarm Init/Animate/Render and original owner/RenderObject matrix carrier/Illuminate/projector/ARGBsoftware raster. Decoded resource, base animator traversal, GetObject and RenderObject dispatch are explicit boundaries. No GPU geometry/color input; no fit. All192triangles retained, actual source span gate/culling decides pixels. Early software rejection is not hardware appearance credit; naturalcaller/audio/device/exactpixels remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.exe,a.output)
