#!/usr/bin/env python3
"""Compile Burn leaf using independently verified real source actor-pose inputs."""
import argparse,json,re,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "burncore.h"
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cstdio>
#include <fstream>
uint32_t rng=2;int Random(int a,int b){if(a==b)return a;rng=rng*214013u+2531011u;return a+int((rng>>16)&32767u)%(b-a+1);}
int main(int argc,char**argv){assert(argc==2);hmm_mat4 bones[22];std::ifstream f(argv[1],std::ios::binary);f.read(reinterpret_cast<char*>(bones),sizeof(bones));assert(f.gcount()==sizeof(bones));hmm_mat4 root;BuildRetailSoftwareOwner(root,0,0,16,96);burn_core::State s;for(int t=1;t<=90;++t){if(!s.done){if(Random(0,34)<2 && t<50)Random(1,2);s.Advance(Random,[&](int i,hmm_mat4&m){m=bones[i];return true;},root,22);}printf("H %d %d %d %d %u\n",t,s.frame,s.to_add,s.done,rng);for(int pool=0;pool<2;++pool)for(int i=0;i<70;++i){const auto&p=(pool?s.smoke:s.fire)[i];printf("S %d %d %d %d",t,pool,i,p.used);if(p.used){for(int k=0;k<15;++k)printf(" %.9g",reinterpret_cast<const float*>(&p)[k]);printf(" %d %d",p.life,p.span);}puts("");}s.Render([&](int pool,int slot,int object,const hmm_mat4&m){printf("D %d %d %d %d",t,pool,slot,object);for(int k=0;k<16;++k)printf(" %.9g",(&m.Elements[0][0])[k]);puts("");});}}
'''
def run(output,native,source_log):
 output.mkdir(parents=True,exist_ok=False);log=source_log.read_text();poses={int(m[1]):list(map(float,m[2].split(',')))for m in re.finditer(r'\[burn-actor-pose\] object=(\d+) ok=1 matrix=([0-9.eE+,-]+)',log)};assert len(poses)==22;fixture=output/'source-actual-pose.fixture';fixture.write_bytes(b''.join(struct.pack('<16f',*poses[i])for i in range(22)));driver=output/'state.cpp';driver.write_text(MAIN);binary=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;states={};headers={};draws={}
 for line in q.stdout.splitlines():
  p=line.split();t=int(p[1])
  if p[0]=='H':headers[t]=list(map(int,p[2:]));continue
  if p[0]=='S':states[t,int(p[2]),int(p[3])]=(int(p[4]),list(map(float,p[5:20])),list(map(int,p[20:])));continue
  draws.setdefault(t,[]).append((int(p[4]),list(map(float,p[5:]))))
 n=json.loads(native.read_text());assert n['interpolate'] is False and n['seed_after_actor_set']==2;errors=[];statefields=matrixfields=0;state_max=matrix_max=0.;rngfields=0
 for row in n['rows']:
  t=row['tick'];h=headers[t];statefields+=3;rngfields+=1
  if h!=[row['animator_frame'],row['to_add'],int(bool(int(row['flags'],16)&0x1000)),row['rng_after']]:errors.append(dict(tick=t,kind='header_rng',native=[row['animator_frame'],row['to_add'],int(bool(int(row['flags'],16)&0x1000)),row['rng_after']],source=h))
  for pool in(0,1):
   raw=bytes.fromhex(row['smoke_hex']if pool else row['fire_hex'])
   for i in range(70):
    vals=struct.unpack_from('<18f4i',raw,i*88);s=states[t,pool,i];statefields+=1
    if bool(vals[21])!=bool(s[0]):errors.append(dict(tick=t,pool=pool,slot=i,kind='used'))
    if vals[21]and s[0]:
     d=max(abs(a-b)for a,b in zip(vals[:15],s[1]));state_max=max(state_max,d);statefields+=17
     if d>3e-5 or list(vals[19:21])!=s[2]:errors.append(dict(tick=t,pool=pool,slot=i,kind='slot',error=d))
  native_draws=row['draws'];source_draws=draws.get(t,[])
  if len(native_draws)!=len(source_draws):errors.append(dict(tick=t,kind='draw_count',native=len(native_draws),source=len(source_draws)))
  for k,(a,b)in enumerate(zip(native_draws,source_draws)):
   d=max(abs(x-y)for x,y in zip(a['matrix'],b[1]));matrix_max=max(matrix_max,d);matrixfields+=16
   if a['object']!=b[0]or d>3e-5:errors.append(dict(tick=t,draw=k,kind='matrix',error=d))
 report=dict(status='pass'if not errors else'fail',accepted=False,state_fields=statefields,matrix_fields=matrixfields,rng_fields=rngfields,max_state_error=state_max,max_matrix_error=matrix_max,errors=errors,native_manifest_sha256=sha(native.read_bytes()),actual_source_pose_log_sha256=sha(source_log.read_bytes()),core_sha256=sha((ROOT/'src/game/effects/burncore.h').read_bytes()),sanitizers='ASan/UBSan',scope='Actualcompiled Burn leaf/directowner-range sequence with readonly actual source22bone API outputs independently matched to original under interpolationoff. All140used flags, all15 meaningfulfloat fields andlife/span, wholelocaldrawmatrices/RNG/finitekill through90. Originalunused temp/flicker bytes are uninitialized andnotconsumed by Render(false). ActualactorDamage/stat calls remain full original/sourceMetal checks, not modeled/faked here; no nativebone output injected into production.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],statefields,matrixfields,rngfields,state_max,matrix_max,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--source-log',type=Path,required=True);a=p.parse_args();run(a.output,a.native,a.source_log)
