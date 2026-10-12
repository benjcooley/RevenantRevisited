#!/usr/bin/env python3
"""Compiled actual Halo controller/public API semantics against original execution."""
import argparse,json,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "halocore.h"
#include <cstdio>
int main(){for(int which=0;which<3;++which){halo_core::State s(which==0?60:which==1?20:0,which==0?.04f:which==1?.08f:0.f);for(int t=1;t<=70;++t){s.Advance();hmm_mat4 m;bool draw=s.Matrix(m);printf("%d %d %d %.9g %d %d",which,t,s.frame,s.scale,s.done,draw);if(draw)for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}}}
'''
def run(output,inputs):
 output.mkdir(parents=True,exist_ok=False);driver=output/'state.cpp';driver.write_text(MAIN);binary=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;source={}
 for line in q.stdout.splitlines():
  p=line.split();source[int(p[0]),int(p[1])]=dict(frame=int(p[2]),scale=float(p[3]),done=int(p[4]),draw=int(p[5]),matrix=list(map(float,p[6:])))
 errors=[];state_fields=0;matrix_fields=0;maximum=0.;scale_max=0.
 for case,path in enumerate(inputs):
  native=json.loads(path.read_text())
  for n in native['rows']:
   s=source[case,n['tick']];delta=abs(n['scale']-s['scale']);scale_max=max(scale_max,delta);state_fields+=4
   if n['frame']!=s['frame']or bool(int(n['flags'],16)&0x1000)!=bool(s['done'])or delta>1e-7 or len(n['draws'])!=s['draw']:errors.append(dict(case=case,tick=n['tick'],kind='state',native=n,source=s))
   for draw in n['draws']:
    d=max(abs(a-b)for a,b in zip(draw['matrix'],s['matrix']));maximum=max(maximum,d);matrix_fields+=16
    if d>2e-5:errors.append(dict(case=case,tick=n['tick'],kind='matrix',error=d))
 r=dict(status='pass'if not errors else'fail',accepted=False,state_fields=state_fields,matrix_fields=matrix_fields,max_scale_error=scale_max,max_matrix_error=maximum,errors=errors,native_manifests=[dict(path=str(p),sha256=sha(p.read_bytes()))for p in inputs],core_sha256=sha((ROOT/'src/halocore.h').read_bytes()),sanitizers='ASan/UBSan',scope='Declared public(60,.04)/(20,.08) and actualdefault0/0. Whole scale/frame/kill/draw and all16 localmatrix channels versus executed original. Original caller, actualMetal and naturalmap separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],state_fields,matrix_fields,maximum,len(errors));assert not errors;return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,nargs=3,required=True);a=p.parse_args();run(a.output,a.native)
