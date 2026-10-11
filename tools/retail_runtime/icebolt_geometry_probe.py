"""Compiled original-domain IceBolt Render packets versus executed native RenderObject."""
import argparse,json,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha
DRIVER=r'''
#include <cstdio>
#include "iceboltcore.h"
#include "retailsoftwaretransform.h"
uint32_t seed=1;
int random_range(int lo,int hi){seed=seed*214013u+2531011u;return lo+int((seed>>16)&32767u)%(hi-lo+1);}
int main(){icebolt_core::State s;s.Initialize(random_range);hmm_mat4 owner;BuildRetailSoftwareOwner(owner,0,0,128,0);
for(int t=0;t<121;++t){s.Pulse(random_range);std::printf("[");int count=0;icebolt_core::Render(s,0,[&](const icebolt_core::Packet&p){hmm_mat4 w;MtxMultiply(&w,&p.local,&owner);std::printf("%s{\"object\":%d,\"matrix\":[",count++?",":"",p.object);for(int i=0;i<16;++i)std::printf("%s%.9e",i?",":"",w.Elements[i/4][i%4]);std::printf("]}");});std::puts("]");}}
'''
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);source=a.output/'geometry.cpp';source.write_text(DRIVER);binary=a.output/'geometry'
subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True)
native=json.loads(a.trace.read_text());port=[json.loads(x)for x in subprocess.check_output([str(binary)],text=True).splitlines()];errors=[];checks=0;maximum=0;count=0
for row,candidate in zip(native['records'],port):
 if not row['draws']:continue
 assert len(row['draws'])==len(candidate),(row['tick'],len(row['draws']),len(candidate))
 for slot,(n,c)in enumerate(zip(row['draws'],candidate)):
  assert n['object']==c['object'];count+=1
  for component,(x,y)in enumerate(zip(n['matrix'],c['matrix'])):
   checks+=1;delta=abs(x-y);maximum=max(maximum,delta)
   if delta>5e-5:errors.append(dict(tick=row['tick'],slot=slot,object=n['object'],component=component,native=x,port=y,delta=delta))
r=dict(status='pass'if not errors else'differences',checks=checks,packets=count,max_matrix_error=maximum,errors=errors,native_trace=dict(path=str(a.trace.resolve()),sha256=sha(a.trace.read_bytes())),source_sha256={x:sha((ROOT/x).read_bytes())for x in('src/iceboltcore.h','src/math3d.cpp','src/retailsoftwaretransform.h')})
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,count,maximum,'errors',len(errors));print(json.dumps(errors[:8],indent=2))
if errors:raise SystemExit(1)
