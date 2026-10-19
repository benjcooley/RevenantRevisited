"""Compiled actual qualified Labyrinth controller vs executed original1..3 diagnostic."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "labyrinthqualifiedcore.h"
#include <cstdio>
int main(){uint32_t rng=1;auto random=[&](int a,int b){rng=rng*214013u+2531011u;return a+int(((rng>>16)&32767u)%uint32_t(b-a+1));};labyrinth_qualified_core::State s;s.Initialize(random);for(int t=1;t<=170;++t){s.Advance(random);int n=0;s.Render(random,[&](int obj,const hmm_mat4&m){printf("D %d %d %d",t,n++,obj);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});printf("H %d %u\n",t,rng);for(int i=0;i<150;++i){const auto&p=s.stars[i];printf("S %d %d %.9g %.9g %.9g %.9g %d %d %d %d\n",t,i,p.radius,p.angle,p.length,p.speed,p.selector,p.position[0],p.position[1],p.position[2]);}}}
'''
def run(out,native):
 out.mkdir(parents=True,exist_ok=False);d=out/'core.cpp';d.write_text(MAIN);binary=out/'core';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(d),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);assert q.returncode==0 and not q.stderr;(out/'source.trace').write_text(q.stdout);states={};draws={};rng={}
 for line in q.stdout.splitlines():
  p=line.split();t=int(p[1])
  if p[0]=='S':states[t,int(p[2])]=list(map(float,p[3:]))
  elif p[0]=='H':rng[t]=int(p[2])
  else:draws.setdefault(t,[]).append(dict(object=int(p[3]),matrix=list(map(float,p[4:]))))
 n=json.loads(native.read_text());assert n['patch']['address']=='0x50540b'and n['patched_original'];errors=[];fields=matfields=0;maximum=maxmatrix=0
 for r in n['rows']:
  t=r['tick'];b=bytes.fromhex(r['state_hex']);f=struct.unpack('<600f',b[:2400]);sel=struct.unpack('<150i',b[2400:3000]);pos=list(struct.iter_unpack('<3i',b[3000:4800]))
  if rng[t]!=r['rng']:errors.append(dict(tick=t,kind='rng',source=rng[t],native=r['rng']))
  for i in range(150):
   expected=[f[k*150+i]for k in range(4)]+[sel[i],*pos[i]];source=states[t,i];delta=max(abs(a-b)for a,b in zip(expected,source));fields+=8;maximum=max(maximum,delta)
   if delta>4e-5:errors.append(dict(tick=t,star=i,kind='state',error=delta,native=expected,source=source))
  if len(draws[t])!=len(r['draws']):errors.append(dict(tick=t,kind='drawcount',source=len(draws[t]),native=len(r['draws'])))
  for i,(a,b)in enumerate(zip(draws[t],r['draws'])):
   delta=max(abs(x-y)for x,y in zip(a['matrix'],b['matrix']));matfields+=16;maxmatrix=max(maxmatrix,delta)
   if a['object']!=b['object']or delta>3e-4:errors.append(dict(tick=t,draw=i,kind='matrix',error=delta,native=b,source=a))
 result=dict(status='pass'if not errors else'fail',accepted=False,qualified_modified_original=True,state_fields=fields,matrix_fields=matfields,maximum_state_error=maximum,maximum_matrix_error=maxmatrix,errors=errors,source_sha256=sha((ROOT/'src/game/effects/labyrinthqualifiedcore.h').read_bytes()),native_sha256=sha(native.read_bytes()),sanitizers='ASan/UBSan',scope='Actual production150stars/Init/Advance/fullRender matrices andRNG compared against whole original with one bound4→3 patch. Own selectors only1..3 andauthoredbackground0; no injected state or inventedmesh.');(out/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(result['status'],fields,matfields,maximum,maxmatrix,len(errors));print(errors[:2]);return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);a=p.parse_args();run(a.output,a.native)
