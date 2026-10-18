#!/usr/bin/env python3
"""Compiled actual IrisFlare controller/matrices versus whole original caller."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from irisflare_native_probe import NativeIris,ROOT,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include "irisflarecore.h"
#include <cstdio>
int main(){irisflare_core::State s;for(int tick=1;tick<=205;++tick){s.Advance();int draw=0;s.Render([&](int obj,unsigned flags,const hmm_mat4&m){printf("D %d %d %d %u",tick,draw++,obj,flags);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});printf("S %d %.9g",tick,s.rotation);for(auto&p:s.positions)printf(" %.9g %.9g %.9g",p.X,p.Y,p.Z);printf(" %d %d %.9g %.9g %.9g %.9g %.9g\n",s.iterations,s.ticks,s.cylinder[0],s.cylinder[1],s.growth[0],s.growth[1],s.flare_angle);}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'core.cpp';driver.write_text(MAIN);binary=output/'core';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;states={};draws={}
 for l in q.stdout.splitlines():
  p=l.split();tick=int(p[1])
  if p[0]=='S':states[tick]=list(map(float,p[2:]))
  else:draws.setdefault(tick,[]).append(dict(object=int(p[3]),flags=hex(int(p[4])),matrix=list(map(float,p[5:]))))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/irisflare.i3d')
 f=NativeIris(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);f.raster_enabled=False;errors=[];fields=matrix_fields=0;max_state=max_matrix=0;rows=[]
 for tick in range(1,206):
  f.advance();_,r=f.render();s=r['state'];native=[s['rotation'],*s['positions'],s['iterations'],s['ticks'],*s['cylsizes'],*s['growth'],s['flare_angle']];delta=max(abs(a-b)for a,b in zip(native,states[tick]));max_state=max(max_state,delta);fields+=len(native)
  if delta>3e-5:errors.append(dict(tick=tick,kind='state',error=delta,native=native,source=states[tick]))
  if len(r['draws'])!=len(draws[tick]):errors.append(dict(tick=tick,kind='count',native=len(r['draws']),source=len(draws[tick])))
  for i,(n,p)in enumerate(zip(r['draws'],draws[tick])):
   if(n['object'],n['flags'])!=(p['object'],p['flags']):errors.append(dict(tick=tick,draw=i,kind='object_flags'))
   delta=max(abs(a-b)for a,b in zip(n['matrix'],p['matrix']));max_matrix=max(max_matrix,delta);matrix_fields+=16
   if delta>2e-4:errors.append(dict(tick=tick,draw=i,kind='matrix',error=delta,native=n['matrix'],source=p['matrix']))
  rows.append(r)
 report=dict(status='pass'if not errors else'fail',accepted=False,asset_sha256=ASSET_SHA,state_fields=fields,matrix_fields=matrix_fields,max_state_error=max_state,max_matrix_error=max_matrix,errors=errors,sanitizers='ASan/UBSan',source_sha256=sha((ROOT/'src/game/effects/irisflarecore.h').read_bytes()),native_constructor=f.constructor,rows=rows,scope='205 complete original custom Animate/RenderObject/device-handoff calls against independently compiled actual production controller and matrix code. All23 state/cache fields per tick, all emitted matrices/order/flags, original reset201 blank3degenerateflaredraws and uninterrupted rotation. No native pose/state inputs to source. Null owner command/resource/COM/legacy initial-cache boundaries remain explicit; pixels/actual GPU and natural context separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,matrix_fields,max_state,max_matrix,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
