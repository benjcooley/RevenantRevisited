#!/usr/bin/env python3
"""Original projected-triangle rejection versus the narrow Comet source gate.

The three raster branches share literal640/480 edge limits. Calls execute the
original comparisons, then stop immediately before pixel raster for accepted
triangles. This isolates primitive acceptance from texture/color coverage.
"""
import argparse,json,struct,subprocess
from pathlib import Path
from software_probe import SoftwareFixture
from speed_profile_contract import ROOT,sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS,UC_X86_REG_EIP
class NativeBounds:
 def __init__(self,exe):
  self.f=SoftwareFixture(exe);self.f.set_texture(2,2,b'\xff\xff'*4);self.calls=[];self.accepted=False
  self.raster={0x56e0b7:0x56e284,0x56e3e7:0x56e61f,0x56e76e:0x56ea9f}
  guards=(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768)
  for a in(*guards,*self.raster):self.f.vm.uc.hook_add(UC_HOOK_CODE,self.hook,begin=a,end=a)
 def hook(self,uc,a,*_):
  self.calls.append(dict(address=hex(a),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
  if a in self.raster:self.accepted=True;uc.reg_write(UC_X86_REG_EIP,self.raster[a])
 def test(self,points):
  self.calls=[];self.accepted=False
  self.f.draw([(x,y,1,31,31,31,31,0,0)for x,y in points],[0,1,2]);return self.accepted

def run(exe,output):
 output.mkdir(parents=True,exist_ok=True)
 cases=[[(0,0),(640,0),(0,480)],[(0,0),(640.00006103515625,0),(0,480)],[(0,0),(640,0),(0,480.0000305175781)],[(0,0),(-640,0),(0,-480)],[(0,0),(-640.00006103515625,0),(0,-480)],[(0,0),(0,-480.0000305175781),(640,0)],[(0,0),(0,100),(641,0)],[(0,0),(0,100),(0,481)],[(0,0),(-640,0),(640,0)],[(99,-55),(100,-50),(12,-50)]]
 # Both winding orders and translations retain the same extent comparisons.
 cases+= [[p[0],p[2],p[1]]for p in cases[:]]
 source=output/'bounds.cpp';source.write_text(r'''
#include "cometsoftwarebounds.h"
#include <cstdio>
int main(){hmm_vec3 p[3];while(scanf("%f %f %f %f %f %f",&p[0].X,&p[0].Y,&p[1].X,&p[1].Y,&p[2].X,&p[2].Y)==6)printf("%d\n",RetailCometProjectedTriangleAllowed(p));}
''');binary=output/'bounds';cmd=['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(source),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)]
 q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
 q=subprocess.run([str(binary)],input='\n'.join(' '.join(str(v)for p in c for v in p)for c in cases),capture_output=True,text=True);assert q.returncode==0,q.stderr;port=list(map(int,q.stdout.split()));f=NativeBounds(exe);rows=[]
 for points,current in zip(cases,port):
  original=f.test(points);rows.append(dict(points=points,native=original,source=bool(current),branches=f.calls));assert original==bool(current),rows[-1]
 # Actual authored base poses independently exercise the matrix wrapper.
 import zipfile
 from meteorstorm_visual_reference import MeteorStormScene
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/comet.I3D')
 scene=MeteorStormScene(exe,asset);v=scene.vm;native_cases=[];inputs=[];point=v.allocate(12);result=v.allocate(12)
 for obj in (4,7):
  raw=[p[:3]for p in struct.iter_unpack('<8f',bytes.fromhex(scene.asset['objects'][obj]['vertex_hex']))]
  o=scene.asset['objects'][obj];offset=o['texture_runs'][0];indices=list(struct.unpack('<6H',bytes.fromhex(scene.asset['face_hex'])[offset*6:(offset+2)*6]))
  for frame in range(45):
   if not v.call(0x40a420,(scene.animobjs[obj],0,frame,0,0),this=scene.imagery):continue
   matrix=bytes(v.uc.mem_read(scene.animobjs[obj]+0x58,64));world=[]
   for p in raw:
    v.write(point,struct.pack('<3f',*p));v.call(0x43ad80,(scene.animobjs[obj]+0x58,point,result));world.append(struct.unpack('<3f',v.uc.mem_read(result,12)))
   projected=scene.software.project(world,camera=(0,0,0))
   for face in range(2):
    ix=indices[3*face:3*face+3];points=[projected[i][:2]for i in ix];native=f.test(points)
    inputs.append(list(struct.unpack('<16f',matrix))+[p for i in ix for p in raw[i]])
    native_cases.append(dict(object=obj,frame=frame,face=face,native=native))
 world_source=output/'world.cpp';world_source.write_text(r'''
#include "cometsoftwarebounds.h"
#include <cstdio>
int main(){hmm_mat4 m;hmm_vec3 p[3];while(scanf("%f",&m.Elements[0][0])==1){for(int i=1;i<16;++i)scanf("%f",(&m.Elements[0][0])+i);for(auto&v:p)scanf("%f%f%f",&v.X,&v.Y,&v.Z);printf("%d\n",RetailCometTriangleAllowed(p,m));}}
''');world_binary=output/'world';world_cmd=list(cmd);world_cmd[world_cmd.index(str(source))]=str(world_source);world_cmd[-1]=str(world_binary)
 q=subprocess.run(world_cmd,capture_output=True,text=True);assert q.returncode==0,q.stderr
 q=subprocess.run([str(world_binary)],input='\n'.join(' '.join(map(str,row))for row in inputs),capture_output=True,text=True);assert q.returncode==0,q.stderr
 for row,current in zip(native_cases,map(int,q.stdout.split())):row['source']=bool(current);assert row['native']==row['source'],row
 report=dict(status='pass',checks=len(rows),cases=rows,authored_triangle_checks=len(native_cases),authored_triangles=native_cases,source_sha256=sha((ROOT/'src/game/effects/cometsoftwarebounds.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),executable_sha256=sha(exe.read_bytes()),scope='Primitive admission only; zero replacement geometry/color; guard enabled solely by native-domain and software-lighting conjunction in Comet adapter.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',len(rows));return report
def run_visual(exe,output):
 import zipfile
 from meteorstorm_visual_reference import MeteorStormScene
 from software_probe import save_rgb565_png
 output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/comet.I3D')
 f=MeteorStormScene(exe,data)
 for tick in range(15):
  f.step();f.animate();f.raster_enabled=False
  if tick<14:f.render()
 v=f.vm;v.checkpoint();count=f.rng_draws;guards=[]
 def observe(uc,address,*_):
  if f.current_object==7:guards.append(dict(address=hex(address),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
 for address in(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):
  v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 rows=[]
 for name,limits in [('original',(640,480)),('widened_negative_control',(6400,4800))]:
  v.restore();f.rng_draws=count;guards.clear();v.write(0x5b9cc0,struct.pack('<f',limits[0]));v.write(0x5b9cbc,struct.pack('<f',limits[1]));f.raster_enabled=True;pixels=f.render()
  image=output/(name+'.png');save_rgb565_png(image,pixels,1920,1080)
  rows.append(dict(name=name,limits=limits,guards=list(guards),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)),state=f.state(),draws=f.draws,image=str(image.resolve()),image_sha256=sha(image.read_bytes())))
 assert rows[0]['state']==rows[1]['state'] and rows[0]['draws']==rows[1]['draws']
 assert any(g['reject']for g in rows[0]['guards']) and not any(g['reject']for g in rows[1]['guards'])
 assert rows[1]['pixels']>rows[0]['pixels']
 report=dict(status='diagnostic_negative_control',capture=14,controls=rows,source_sha256=sha(Path(__file__).read_bytes()),original_sha256=sha(exe.read_bytes()),scope='Only literal raster guard constants changed in the second branch. Authored vertices, matrices, colors, controller state and RNG are identical. Widened image is not original visual acceptance.')
 (output/'proof.json').write_text(json.dumps(report,indent=2)+'\n');print('guard negative control',rows[0]['pixels'],rows[1]['pixels']);return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--visual-negative-control',action='store_true');a=p.parse_args();(run_visual if a.visual_negative_control else run)(a.exe,a.output)
