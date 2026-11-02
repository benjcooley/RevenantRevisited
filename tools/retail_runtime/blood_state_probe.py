#!/usr/bin/env python3
"""Compiled actual Blood core against executed original full state and matrices."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "bloodcore.h"
#include <cstdio>
void print(const blood_core::State&s,int t){printf("S %d %u %d ",t,s.rng,s.done);for(const auto&p:s.slots)for(int i=0;i<36;++i)printf("%02x",reinterpret_cast<const unsigned char*>(&p)[i]);puts("");}
int main(){blood_core::State s(2);print(s,0);printf("C %u\n",blood_core::State::PackedColor(52));for(int t=1;t<=100;++t){s.Advance();print(s,t);s.Render([&](int slot,int object,int pass,const hmm_mat4&m){printf("D %d %d %d %d",t,slot,object,pass);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});}}
'''
def run(output,native):
 output.mkdir(parents=True,exist_ok=False);driver=output/'core.cpp';driver.write_text(MAIN);binary=output/'core';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;states={};draws={};color=None
 for l in q.stdout.splitlines():
  p=l.split();
  if p[0]=='C':color=int(p[1]);continue
  t=int(p[1]);
  if p[0]=='S':states[t]=(int(p[2]),int(p[3]),bytes.fromhex(p[4]))
  else:draws.setdefault(t,[]).append(dict(slot=int(p[2]),object=int(p[3]),pass_index=int(p[4]),matrix=list(map(float,p[5:]))))
 r=json.loads(native.read_text());rows=[dict(tick=0,**r['initialization'])]+r['rows'];errors=[];fields=0;maximum=0;matrix_fields=0;matrixmax=0;color_fields=0;rng_fields=0
 for row in rows:
  t=row['tick'];
  if 'rng_after'in row:
   rng_fields+=1
   if row['rng_after']!=states[t][0]:errors.append(dict(tick=t,kind='rng',native=row['rng_after'],source=states[t][0]))
  if t:
   native_draws=row['draws'];source_draws=draws.get(t,[])
   if len(native_draws)!=len(source_draws):errors.append(dict(tick=t,kind='draw_count',native=len(native_draws),source=len(source_draws)))
   for i,(n,p)in enumerate(zip(native_draws,source_draws)):
    if 'object'in n:
     d=max(abs(a-b)for a,b in zip(n['matrix'],p['matrix']));matrix_fields+=16;matrixmax=max(matrixmax,d)
     if n['object']!=p['object']or d>2e-5:errors.append(dict(tick=t,draw=i,kind='object_matrix',native=n['object'],source=p['object'],error=d))
    verts=bytes.fromhex(n['vertex_hex'])
    for v in range(n['args'][3]):
     c=struct.unpack_from('<I',verts,32*v+16)[0];color_fields+=1
     if c!=color:errors.append(dict(tick=t,draw=i,kind='prelit_color',native=c,source=color))
  n=bytes.fromhex(row['state_hex']);s=states[t][2]
  for i in range(30):
   for k in range(7):
    a,b=struct.unpack_from('<f',n,36*i+4*k)[0],struct.unpack_from('<f',s,36*i+4*k)[0];d=abs(a-b);maximum=max(maximum,d);fields+=1
    if d>3e-5:errors.append(dict(tick=t,slot=i,channel=k,native=a,source=b,error=d))
   fields+=5
   if n[36*i+28:36*i+36]!=s[36*i+28:36*i+36]:errors.append(dict(tick=t,slot=i,kind='used_object_stage_age_delay',native=n[36*i+28:36*i+36].hex(),source=s[36*i+28:36*i+36].hex()))
  if t and row['done']!=states[t][1]:errors.append(dict(tick=t,kind='done'))
 report=dict(status='pass'if not errors else'fail',accepted=False,state_fields=fields,max_state_error=maximum,matrix_fields=matrix_fields,max_matrix_error=matrixmax,color_fields=color_fields,rng_fields=rng_fields,errors=errors,source_sha256=sha((ROOT/'src/game/effects/bloodcore.h').read_bytes()),native_manifest_sha256=sha(native.read_bytes()),sanitizers='ASan/UBSan',scope='Actual compiled independently initialized production30-slot core vs all full native slots; real constructor/SetParams/Init/Animate/wholeRender, fixedcommonInit seed2, detail4, declaredmap/world boundary. No native state inputs to source. Actual Metal/materials, matrices/RNG and naturalmap caller separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);a=p.parse_args();run(a.output,a.native)
