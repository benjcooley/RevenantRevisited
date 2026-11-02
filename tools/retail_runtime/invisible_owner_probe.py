#!/usr/bin/env python3
"""Actual Invisible submission/whole geometry, including original translated quirk."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from invisible_native_preflight import NativeInvisible,ROOT,ASSET_SHA
from speed_controller_preflight import read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "invisiblecore.h"
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>
struct SHelperMeshSubmit{int mesh=0;float world[16]{};int retail_lighting=0;bool retail_source_alpha=false,retail_software_projection=false,additive_blend=false,retail_additive_cull_none=false;};
struct Capture{SHelperMeshSubmit draw;void SubmitRetailSourceAlphaAfterFx(const SHelperMeshSubmit&d){draw=d;}void SubmitGoldFlareAfterFx(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;
bool StartupVfxNativeDomain=true;
void ProductionSubmit(int object,const hmm_mat4&local,const hmm_mat4&root){struct{int meshes[2]={1,2};}holder;auto*s=&holder;COMPOSITION}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);float raw[128];f.read(reinterpret_cast<char*>(raw),512);assert(f.gcount()==512);int checks=0;
for(int owner=0;owner<2;++owner){int x=owner?100:0,y=owner?200:0,z=owner?30:0;invisible_core::State s;hmm_mat4 root;BuildRetailSoftwareOwner(root,x,y,z,0);for(int tick=1;tick<=16;++tick){s.Advance();int draw=0;s.Render([&](int obj,const hmm_mat4&local){ProductionSubmit(obj,local,root);hmm_mat4 world;for(int r=0;r<4;++r)for(int c=0;c<4;++c)world.Elements[c][r]=capture.draw.world[r*4+c];printf("D %d %d %d %d",owner,tick,draw++,obj);for(int i=0;i<16;++i)printf(" %.9g",(&world.Elements[0][0])[i]);puts("");for(int i=obj?12:0;i<(obj?16:12);++i){hmm_vec3 p={raw[8*i],raw[8*i+1],raw[8*i+2]},a,b,result;MtxTransform(&local,&p,&a);if(obj)b=a;else MtxTransform(&root,&a,&b);MtxTransform(&world,&p,&result);for(int c=0;c<3;++c)assert(fabs(result.Elements[c]-b.Elements[c])<.0001f);++checks;}assert(capture.draw.retail_lighting==(obj?1:3)&&capture.draw.retail_software_projection);if(obj)assert(capture.draw.additive_blend&&capture.draw.retail_additive_cull_none);else assert(capture.draw.retail_source_alpha);},float(x),float(y),float(z));}}printf("CHECKS %d\n",checks);}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/invisiblepreview.cpp').read_text();a=source.index('hmm_mat4 world=local;');b=source.index('++count;',a);branch=source[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('COMPOSITION',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/invisible.i3d'))
 a=read_asset(data);fixture=output/'vertices.fixture';fixture.write_bytes(b''.join(bytes.fromhex(o['vertex_hex'])for o in a['objects']));q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;draws={};checks=0
 for l in q.stdout.splitlines():
  p=l.split()
  if p[0]=='CHECKS':checks=int(p[1]);continue
  owner,tick,index,obj=map(int,p[1:5]);draws.setdefault((owner,tick),[]).append((obj,list(map(float,p[5:]))))
 errors=[];fields=0;maximum=0;rows=[]
 for owner,pos in enumerate(((0,0,0),(100,200,30))):
  f=NativeInvisible(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);f.vm.write(f.owner+0x10,struct.pack('<3i',*pos));f.initialize();f.raster_enabled=False
  for tick in range(1,17):
   f.step();_,r=f.render_whole();p=draws[(owner,tick)];n=r['draws'];assert len(n)==len(p)
   for i,(d,(obj,m))in enumerate(zip(n,p)):
    delta=max(abs(a-b)for a,b in zip(d['world'],m));maximum=max(maximum,delta);fields+=16
    if d['object']!=obj or delta>2e-4:errors.append(dict(owner=pos,tick=tick,draw=i,error=delta))
   rows.append(dict(owner_position=pos,**r))
 report=dict(status='pass'if not errors else'fail',accepted=False,geometry_checks=checks,world_matrix_fields=fields,max_matrix_error=maximum,errors=errors,authored_vertices=16,asset_sha256=ASSET_SHA,actual_submit_span_sha256=sha(branch.encode()),sanitizers='ASan/UBSan',rows=rows,scope='Actual extracted source submit composition/material/lighting flags across all16 real authored vertices and all16 early ticks at origin and translated100,200,30. Original complete Init/Animate/Render verifies every composed worldmatrix; rings genuinely include owner twice while ABSPOS sparks bypass root. This exact null-invoker placement is preserved; actor-bound natural scene and GPU equality separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,fields,maximum,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
