#!/usr/bin/env python3
"""Independent production MagicShield finite core against complete original traces."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "magicshieldcore.h"
#include <cstdio>
#include <cstdlib>
uint32_t rng=2;int Random(int a,int b){if(a==b)return a;rng=rng*214013u+2531011u;return a+int((rng>>16)&32767u)%(b-a+1);}
int main(int argc,char**argv){if(argc!=2)return 2;int minimum=atoi(argv[1]);magicshield_core::Owner o;magicshield_core::State s;s.Initialize(Random);float resistance=0;for(int t=1;t<=170;++t){if(!o.dead){o.Pulse(true,minimum,[&](float value){resistance=value;});if(!o.dead)s.Animate(o.phase,S3DPoint{0,0,16},Random);}printf("H %d %d %d %d %.9g %u\n",t,o.phase,o.countdown,o.dead,resistance,rng);printf("S %d %.9g %.9g %.9g %.9g %d %d %d",t,s.angle,s.scale.X,s.scale.Y,s.scale.Z,s.position.x,s.position.y,s.position.z);for(float v:s.heights)printf(" %.9g",v);for(float v:s.angles)printf(" %.9g",v);printf(" %d\n",s.tier);if(!o.dead)s.Render(o.phase,[&](int object,int slot,const hmm_mat4&m){printf("D %d %d %d",t,object,slot);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});}}
'''
def run(output,native,minimum):
 output.mkdir(parents=True,exist_ok=False);d=output/'state.cpp';d.write_text(MAIN);binary=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(d),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary),str(minimum)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;heads={};states={};draws={}
 for line in q.stdout.splitlines():
  p=line.split();t=int(p[1])
  if p[0]=='H':heads[t]=[int(x)for x in p[2:5]]+[float(p[5]),int(p[6])]
  elif p[0]=='S':states[t]=list(map(float,p[2:]))
  else:draws.setdefault(t,[]).append((int(p[2]),list(map(float,p[4:]))))
 n=json.loads(native.read_text());assert n['seed_after_actor_set']==2;errors=[];maximum=matrix_max=0;fields=matfields=0
 for row in n['rows']:
  t=row['tick'];h=heads[t];expected=[row['owner_phase'],struct.unpack('<i',struct.pack('<I',row['owner_countdown']))[0],int(bool(int(row['flags'],16)&0x1000)),row['invoker_factor'],row['rng_after']];fields+=5
  if h!=expected:errors.append(dict(tick=t,kind='header_rng_stat',source=h,native=expected))
  raw=bytes.fromhex(row['custom_state_hex']);native_state=list(struct.unpack_from('<4f3i40fi',raw,0));source=states[t];fields+=48;delta=max(abs(a-b)for a,b in zip(native_state,source));maximum=max(maximum,delta)
  if delta>3e-5:errors.append(dict(tick=t,kind='state',max_error=delta))
  nd=row['draws'];sd=draws.get(t,[])
  if len(nd)!=len(sd):errors.append(dict(tick=t,kind='draw_count',native=len(nd),source=len(sd)))
  for i,(a,b)in enumerate(zip(nd,sd)):
   delta=max(abs(x-y)for x,y in zip(a['matrix'],b[1]));matrix_max=max(matrix_max,delta);matfields+=16
   if a['object']!=b[0]or delta>3e-5:errors.append(dict(tick=t,kind='matrix',draw=i,max_error=delta))
 report=dict(status='pass'if not errors else'fail',accepted=False,state_fields=fields,matrix_fields=matfields,max_state_error=maximum,max_matrix_error=matrix_max,errors=errors,native_manifest_sha256=sha(native.read_bytes()),core_sha256=sha((ROOT/'src/game/effects/magicshieldcore.h').read_bytes()),sanitizers='ASan/UBSan',scope='Actual compiled production finite owner/state and all active whole draw matrices, originalMSVC RNG through170; exact declared existing real variant minimum, genuine caster position as upstreamAPI input. No saved state/pose injected into production. Native unknown-name constructor-storage0 selector,10flames; finite ownerkill151 before any remaining animator step. Actual source actor/spell APIs and Metal are separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,matfields,maximum,matrix_max,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--minimum',type=int,required=True);a=p.parse_args();run(a.output,a.native,a.minimum)
