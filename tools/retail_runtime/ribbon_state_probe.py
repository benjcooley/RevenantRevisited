#!/usr/bin/env python3
"""Actual Ribbon state/sampling production header against full native caller."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from ribbon_native_preflight import NativeRibbon,ROOT,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include "effects/ribbonstate.h"
#include <cstdio>
int main(){ribbon_state::State s;uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);for(int tick=1;tick<=90;++tick){if(tick)s.Advance(rng);float floor=s.FloorScale(rng);printf("F %d %u %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",tick,seed,s.scale,s.tilt,s.tilt_delta,s.rotations[0],s.rotations[1],s.rotations[2],floor);for(int i=0;i<30;++i){auto&p=s.sparks[i];printf("P %d %d %d %.9g",tick,i,p.frame,p.scale);for(float f:p.p)printf(" %.9g",f);for(float f:p.v)printf(" %.9g",f);puts("");}}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'state.cpp';driver.write_text(MAIN);binary=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('compile failed')
 q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'source.log').write_text(q.stderr);assert q.returncode==0 and not q.stderr;frames={};particles={}
 for line in q.stdout.splitlines():
  p=line.split()
  if p[0]=='F':frames[int(p[1])]=(int(p[2]),list(map(float,p[3:])))
  else:particles[(int(p[1]),int(p[2]))]=list(map(float,p[3:]))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/ribbon.i3d'))
 f=NativeRibbon(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;f.raster_enabled=False;errors=[];maximum=0;fields=0;rows=[]
 rf=lambda p:struct.unpack('<f',v.uc.mem_read(p,4))[0]
 for tick in range(1,91):
  if tick:f.advance()
  _,record=f.render();values=[rf(f.animator+0x4dc),rf(f.animator+0x4e0),rf(f.animator+0x4e4),*[rf(f.animator+0x4c8+i*4)for i in range(3)],rf(f.animobjs[0]+0x40)];delta=max(abs(a-b)for a,b in zip(values,frames[tick][1]));maximum=max(maximum,delta);fields+=7
  if v.rng_seed!=frames[tick][0]or delta>3e-5:errors.append(dict(tick=tick,kind='core',native=values,source=frames[tick],rng=v.rng_seed,delta=delta))
  for slot in range(30):
   values=[struct.unpack('<i',v.uc.mem_read(f.animator+0x444+slot*4,4))[0],rf(f.animator+0x3cc+slot*4),*[rf(f.animator+0xfc+slot*12+i*4)for i in range(3)],*[rf(f.animator+0x264+slot*12+i*4)for i in range(3)]];source=particles[tick,slot];delta=max(abs(a-b)for a,b in zip(values,source));maximum=max(maximum,delta);fields+=8;rows.append([tick,slot,*values])
   if delta>3e-5:errors.append(dict(tick=tick,slot=slot,delta=delta,native=values,source=source))
 report=dict(status='pass'if not errors else'fail',fields=fields,ticks=90,first_render_tick=1,max_error=maximum,errors=errors,asset_sha256=ASSET_SHA,source_header_sha256=sha((ROOT/'src/effects/ribbonstate.h').read_bytes()),driver_sha256=sha(driver.read_bytes()),accepted=False,scope='Independently compiled actual production Ribbon state/Initialize/20tickgrow/30spark reset/integration/rotations/floor-sample RNG versus complete original constructors/Initialize/Animate/Initialize→Animate1→Render1 then all90ticks through declared device56eb30handoff. Exact full native visual separately retained. Nullspell/target state only; no fakeactor or fittedphysics.');(output/'native-state.json').write_text(json.dumps(rows));(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
