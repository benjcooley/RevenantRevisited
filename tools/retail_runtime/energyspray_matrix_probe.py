#!/usr/bin/env python3
"""Actual leaf full Render matrices/colors vs independently compiled source matrices."""
import argparse,json,subprocess,struct,zipfile
from pathlib import Path
from energyspray_visual_reference import EnergySprayScene,ROOT,sha
DRIVER=r'''
#include "energyspraymatrix.h"
#include <cstdio>
int main(){retail_energyspray::State s;uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);for(int t=0;t<=140;++t){if(t){s.Pulse();s.Animate(rng);}if(!s.alive)continue;auto emit=[&](int n,const hmm_mat4&m,uint32_t color=0){printf("%d %d %u",t,n,color);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");};if(s.phase<3)for(int i=0;i<25;++i)emit(i,retail_energyspray::RadialMatrix(s,i));emit(25,retail_energyspray::FlareMatrix(s.RenderScale(rng)));if(s.phase<3)for(int i=0;i<20;++i)emit(26+i,retail_energyspray::TriangleMatrix(s,i),retail_energyspray::PackedColor(s,i));}}
'''
def run(exe,out):
 out.mkdir(parents=True,exist_ok=False);p=out/'matrix.cpp';p.write_text(DRIVER);binary=out/'matrix';subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(p),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True);q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);(out/'source.trace').write_text(q.stdout);source={(int(v[0]),int(v[1])):(int(v[2]),list(map(float,v[3:])))for l in q.stdout.splitlines()if(v:=l.split())}
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 f=EnergySprayScene(exe,data);f.raster_enabled=False;fields=0;maximum=0;errors=[];colors=0
 for tick in range(141):
  if tick:f.advance()
  f.render()
  for index,d in enumerate(f.draws):
   key=index if len(f.draws)>1 else 25;color,expected=source[tick,key]
   for j,(x,y)in enumerate(zip(d['matrix'],expected)):
    delta=abs(x-y);fields+=1;maximum=max(maximum,delta)
    if delta>5e-4:errors.append(dict(tick=tick,draw=index,field=j,native=x,port=y))
   if d['object']==2:assert struct.unpack_from('<I',bytes.fromhex(d['vertices_hex']),16)[0]==color;colors+=1
 report=dict(status='fail'if errors else'pass',fields=fields,max_float_error=maximum,packed_colors=colors,errors=errors,source_sha256=sha((ROOT/'src/energyspraymatrix.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='All original full RenderObject draw matrices and actual packed RGB32 vertex writes, every live phase; independently compiled original operation sequence, no sampled native pose supplied to source.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
