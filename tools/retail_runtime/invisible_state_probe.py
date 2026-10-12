#!/usr/bin/env python3
"""Independent compiled production Invisible state/matrix contract."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "invisiblecore.h"
#include <cstdio>
void state(const invisible_core::State&s,int t){printf("S %d %u %d %d %.9g",t,s.rng,s.state,s.life,s.rotation);for(int i=0;i<6;++i)printf(" %.9g %.9g %d",s.z[i],s.scale[i],s.delay[i]);for(auto&p:s.particles)printf(" %d %d %d %d %.9g %d",p.x,p.y,p.z,p.velocity,p.scale,p.delay);puts("");}
int main(){invisible_core::State s;state(s,0);for(int t=1;t<=501;++t){s.Advance();state(s,t);int draw=0;s.Render([&](int o,const hmm_mat4&m){printf("D %d %d %d",t,draw++,o);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});}}
'''
def run(output,native):
 output.mkdir(parents=True,exist_ok=False);driver=output/'core.cpp';driver.write_text(MAIN);binary=output/'core';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;states={};draws={}
 for l in q.stdout.splitlines():
  p=l.split();t=int(p[1]);
  if p[0]=='S':states[t]=list(map(float,p[2:]))
  else:draws.setdefault(t,[]).append((int(p[3]),list(map(float,p[4:]))))
 r=json.loads(native.read_text());rows=[dict(tick=0,**r['initialization'])]+r['rows'];errors=[];fields=matrix_fields=0;maximum=matrixmax=0
 for row in rows:
  t=row['tick'];b=bytes.fromhex(row['state_hex']);get=lambda fmt,off:struct.unpack_from(fmt,b,off-0xfc)
  v=[row.get('rng_after'),get('<i',0x174)[0],row['life'],get('<f',0x17c)[0]]
  for i in range(6):v.extend([get('<f',0x104+i*12)[0],get('<f',0x144+i*4)[0],get('<i',0x15c+i*4)[0]])
  for i in range(70):v.extend([*get('<3i',0x180+i*12),get('<i',0x4c8+i*4)[0],get('<f',0x5e0+i*4)[0],get('<i',0x6f8+i*4)[0]])
  delta=max(abs(a-b)for a,b in zip(v,states[t]));maximum=max(maximum,delta);fields+=len(v)
  if delta>1e-5:errors.append(dict(tick=t,kind='state',error=delta,fields=[(i,a,b)for i,(a,b)in enumerate(zip(v,states[t]))if abs(a-b)>1e-5]))
  if t==0:continue
  n=row['draws'];p=draws.get(t,[])
  if len(n)!=len(p):errors.append(dict(tick=t,kind='count',native=len(n),source=len(p)))
  for i,(a,(o,m))in enumerate(zip(n,p)):
   delta=max(abs(x-y)for x,y in zip(a['matrix'],m));matrixmax=max(matrixmax,delta);matrix_fields+=16
   if a['object']!=o or delta>2e-4:errors.append(dict(tick=t,draw=i,kind='matrix',error=delta))
 report=dict(status='pass'if not errors else'fail',accepted=False,state_fields=fields,matrix_fields=matrix_fields,max_state_error=maximum,max_matrix_error=matrixmax,errors=errors,source_sha256=sha((ROOT/'src/invisiblecore.h').read_bytes()),native_manifest_sha256=sha(native.read_bytes()),scope='Compiled actual production70-particle/6-ring controller, original500-tick finite owner, full RNG/state/cache and all emitted matrices across501 original Pulse/Animate/Render rows. Native input states not fed into source; ASan/UBSan. Upstream null generic owner, audio/world and original device handoff boundaries explicit; actualGPU/naturalcharacter separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,matrix_fields,maximum,matrixmax,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);a=p.parse_args();run(a.output,a.native)
