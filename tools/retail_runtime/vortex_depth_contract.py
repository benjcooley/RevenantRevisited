#!/usr/bin/env python3
"""Original cull/depth packets versus compiled actual Vortex triangle submission."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from vortex_native_preflight import NativeVortex,ROOT,ASSET_SHA
from speed_profile_contract import function,sha
MAIN=r'''
#include "math3d.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <vector>
struct Draw{int index_offset=0,index_count=0;bool retail_flat_depth=false;float retail_flat_depth_offset=0;};
struct Capture{int object=0;void SubmitHelperMesh(const Draw&d){printf("F %d %d %.9g\n",object,d.index_offset/3,d.retail_flat_depth_offset);}}capture,*Renderer=&capture;
int main(int argc,char**argv){std::ifstream f(argv[1],std::ios::binary);for(int object=0;object<3;++object){capture.object=object;uint32_t n,faces;f.read(reinterpret_cast<char*>(&n),4);f.read(reinterpret_cast<char*>(&faces),4);std::vector<hmm_vec3>points(n);f.read(reinterpret_cast<char*>(points.data()),n*12);struct{std::vector<uint16_t>indices;}part;part.indices.resize(faces*3);f.read(reinterpret_cast<char*>(part.indices.data()),faces*6);bool StartupVfxNativeDomain=true;Draw mesh;TRIANGLE_LOOP}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/vortex.i3d'))
 f=NativeVortex(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;packets=[];return_address=None
 def finished(uc,a,n,u):
  d=f.draws[-1];count=d['args'][3];indices=list(struct.unpack('<'+'H'*d['args'][5],uc.mem_read(d['args'][4],d['args'][5]*2)));rows=[list(x[:3])for x in struct.iter_unpack('<3f4I2f',uc.mem_read(f.software.vertices,count*36))];packets.append(dict(indices=indices,vertices=rows))
 def entered(uc,a,n,u):
  nonlocal return_address
  ret=v.u32(uc.reg_read(UC_X86_REG_ESP))
  if return_address is None:return_address=ret;v.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
  else:assert return_address==ret
 v.uc.hook_add(UC_HOOK_CODE,entered,begin=0x56eb30,end=0x56eb30);f.render_default();assert len(packets)==3
 fixture=bytearray();expected={};vectors=v.allocate(36)
 for i,(o,packet)in enumerate(zip(f.asset['objects'],packets)):
  assert v.call(0x409950,(i,0,0,vectors,vectors+12,vectors+24),this=f.imagery)==1;p,r,s=struct.unpack('<3f',v.uc.mem_read(vectors,12)),struct.unpack('<3f',v.uc.mem_read(vectors+12,12)),struct.unpack('<3f',v.uc.mem_read(vectors+24,12));assert r==(0.,0.,0.)and s==(1.,1.,1.);raw=list(struct.iter_unpack('<8f',bytes.fromhex(o['vertex_hex'])));points=[[row[c]+p[c]for c in range(3)]for row in raw];indices=packet['indices'];assert max(indices)<len(points);fixture.extend(struct.pack('<2I',len(points),len(indices)//3));fixture.extend(b''.join(struct.pack('<3f',*row)for row in points));fixture.extend(struct.pack('<'+'H'*len(indices),*indices))
  for face in range(len(indices)//3):
   xyz=[packet['vertices'][j]for j in indices[face*3:face*3+3]];a,b,c=xyz;area=(b[0]-a[0])*(-c[1]+a[1])-(-b[1]+a[1])*(c[0]-a[0]);
   if area>0:expected[(i,face)]=max(row[2]for row in xyz)-2750.
 path=output/'worldpoints.fixture';path.write_bytes(fixture);source=(ROOT/'src/effects/vortexpreview.cpp').read_text();loop=function(source,'for(size_t i=0;i<part.indices.size();i+=3)');driver=output/'depth.cpp';driver.write_text(MAIN.replace('TRIANGLE_LOOP',loop));binary=output/'depth';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;actual={(int(p[1]),int(p[2])):float(p[3])for line in q.stdout.splitlines()if(p:=line.split())[0]=='F'};errors=[dict(face=key,kind='cull')for key in expected.keys()^actual.keys()];maximum=max(abs(actual[k]-expected[k])for k in actual.keys()&expected.keys());floor_diffs=sum(int(actual[k]+2750)!=int(expected[k]+2750)for k in actual.keys()&expected.keys());report=dict(status='pass'if not errors and maximum<.001 and not floor_diffs else'fail',asset_sha256=ASSET_SHA,triangles=304,positive_faces=len(expected),source_faces=len(actual),cull_errors=errors,max_depth_error=maximum,integer_depth_differences=floor_diffs,actual_source_loop_sha256=sha(loop.encode()),fixture_sha256=sha(fixture),accepted=False,scope='Executed original complete RenderObject FVF vertices and actual indices vs compiled actual source triangle cull/max-depth loop, all304authoredtriangles/205vertices. Native27posechannels independently exact; this depth-only contract uses declared source world geometry with zero rotations/unit scale and no fittedcamera. Integerdepth/equality retained as measured; GPU sampling/naturaldevice separate.');(output/'native-packets.json').write_text(json.dumps(packets,indent=2)+'\n');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
