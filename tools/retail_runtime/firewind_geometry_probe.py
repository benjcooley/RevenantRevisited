"""Compiled FireWind ordered authored matrices against actual original RenderObject."""
import argparse,json,subprocess
from pathlib import Path
from runtime import Runtime
from fireflash_probe import function
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
v=Runtime(a.executable);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000);tables=a.output/'tables.bin';tables.write_bytes(bytes(v.uc.mem_read(0x634d44,1024)));native=json.loads(a.trace.read_text())
cpp=r'''
#include <cstdio>
#include <fstream>
#include "firewindcore.h"
#include "retailsoftwaretransform.h"
struct S3DPoint{int x,y,z;};short DistX[256],DistY[256];void ConvertToVector(int,int,S3DPoint&,int=0);
uint32_t seed=1;int random_range(int lo,int hi){if(lo==hi)return lo;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
VECTOR
int main(int argc,char**argv){std::ifstream t(argv[1],std::ios::binary);t.read((char*)DistX,512);t.read((char*)DistY,512);firewind_core::State s;s.Initialize(random_range);hmm_mat4 owner;BuildRetailSoftwareOwner(owner,0,0,78,0);
 for(int tick=1;tick<=TICKS;++tick){s.Pulse(random_range,[](int a,int d){S3DPoint p;ConvertToVector(a,d,p);return hmm_vec3{float(p.x),float(p.y),float(p.z)};});int count=0;std::printf("[");
 if(tick<=140&&tick%STRIDE==0)s.Render([&](int object,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,&local,&owner);std::printf("%s{\"object\":%d,\"matrix\":[",count++?",":"",object);for(int i=0;i<16;++i)std::printf("%s%.9e",i?",":"",world.Elements[i/4][i%4]);std::printf("]}");});std::puts("]");}}
'''.replace('VECTOR',function((ROOT/'src/object.cpp').read_text(),'void ConvertToVector(')).replace('TICKS',str(len(native['records']))).replace('STRIDE',str(native['render_stride']))
source=a.output/'geometry.cpp';source.write_text(cpp);binary=a.output/'geometry';subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True)
port=[json.loads(line)for line in subprocess.check_output([str(binary),str(tables)],text=True).splitlines()];checks=0;packets=0;maximum=0;errors=[]
for row,draws in zip(native['records'],port):
 if not row['draws']:continue
 assert len(row['draws'])==len(draws),(row['tick'],len(row['draws']),len(draws))
 for slot,(n,c)in enumerate(zip(row['draws'],draws)):
  assert n['object']==c['object'];packets+=1
  for component,(x,y)in enumerate(zip(n['matrix'],c['matrix'])):
   checks+=1;delta=abs(x-y);maximum=max(maximum,delta)
   if delta>5e-5:errors.append(dict(tick=row['tick'],slot=slot,component=component,native=x,port=y,delta=delta))
r=dict(status='pass'if not errors else'differences',checks=checks,packets=packets,max_matrix_error=maximum,errors=errors[:100],trace=dict(path=str(a.trace.resolve()),sha256=sha(a.trace.read_bytes())),source_sha256={f:sha((ROOT/f).read_bytes())for f in('src/firewindcore.h','src/math3d.cpp','src/retailsoftwaretransform.h')},scope='Original ordered RenderObject matrices with literal asset geometry and native owner0,0,78 face0. No screen-space fit.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,packets,maximum,'errors',errors[:8]);assert not errors
