"""Compiled IceBolt Pulse state against original full native leaf trace."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha
DRIVER=r'''
#include <cstdio>
#include "iceboltcore.h"
uint32_t seed=1,draws=0;
int random_range(int lo,int hi){++draws;seed=seed*214013u+2531011u;return lo+int((seed>>16)&32767u)%(hi-lo+1);}
void emit(const icebolt_core::State&s){
 std::printf("{\"seed\":%u,\"rng_draws\":%u,\"frame\":%d,\"angle\":%d,\"length\":%.9e,\"scales\":[%.9e,%.9e,%.9e,%.9e,%.9e,%.9e,%.9e],\"frost\":[",seed,draws,s.frameon_,s.angle_,s.length_,s.spherescale_[0],s.spherescale_[1],s.cylscale_,s.ringout_,s.spiralang_,s.spiralscale_,s.cy_);
 for(int i=0;i<50;++i){auto&p=s.frost_[i];std::printf("%s{\"p\":[%.9e,%.9e,%.9e],\"v\":[%.9e,%.9e,%.9e],\"s\":%.9e,\"t\":%.9e}",i?",":"",p.pos.X,p.pos.Y,p.pos.Z,p.vel.X,p.vel.Y,p.vel.Z,p.s,p.t);}
 std::printf("],\"snow\":[");for(int i=0;i<50;++i){auto&p=s.snow_[i];std::printf("%s{\"h\":%.9e,\"th\":%.9e,\"r\":%.9e,\"rs\":%.9e,\"sz\":%.9e}",i?",":"",p.h,p.th,p.r,p.rs,p.sz);}std::puts("]}");
}
int main(){icebolt_core::State s;s.Initialize(random_range);emit(s);for(int i=0;i<121;++i){s.Pulse(random_range);emit(s);}}
'''
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);source=a.output/'core.cpp';source.write_text(DRIVER.replace("i<121", "i<"+str(len(json.loads(a.trace.read_text())["records"]))));binary=a.output/'core'
subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
t=json.loads(a.trace.read_text());original=[t['initial']]+[r['state']for r in t['records']];port=[json.loads(x)for x in subprocess.check_output([str(binary)],text=True).splitlines()];assert len(original)==len(port);errors=[];checks=0
for tick,(n,c)in enumerate(zip(original,port)):
 def check(x,y,path):
  global checks
  if isinstance(y,dict):
   for k in y:check(x[k],y[k],path+[k])
  elif isinstance(y,list):
   assert len(x)==len(y)
   for i,(xx,yy)in enumerate(zip(x,y)):check(xx,yy,path+[i])
  else:
   checks+=1;equal=x==y if path[0]in('seed','rng_draws','frame','angle')else struct.pack('<f',x)==struct.pack('<f',y)
   if not equal:errors.append(dict(tick=tick,path=path,native=x,port=y))
 check(n,c,[])
r=dict(status='pass'if not errors else'differences',checks=checks,errors=errors,original_trace=dict(path=str(a.trace.resolve()),sha256=sha(a.trace.read_bytes())),source_sha256=sha((ROOT/'src/iceboltcore.h').read_bytes()),port=port)
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,'errors',len(errors));print(json.dumps(errors[:15],indent=2))
if errors:raise SystemExit(1)
