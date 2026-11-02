#!/usr/bin/env python3
"""Compiled actual strip vertices and halo matrices vs original full Render."""
import argparse,json,subprocess,struct,zipfile
from pathlib import Path
from windstrip_visual_reference import WindStripScene,ROOT,sha
DRIVER=r'''
#include "windstripmatrix.h"
#include <cstdio>
int main(){retail_windstrip::State s;unsigned seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);for(int t=1;t<=251;++t){s.Animate();printf("%d",t);for(auto& strip:s.strips)for(int i=0;i<20;++i){auto v=retail_windstrip::StripVertex(strip,i);printf(" %.9g %.9g %.9g %.9g %d",v.x,v.y,v.z,float(double(i/2)*double(.10000000149011612f)),i&1);}s.RenderDrops(rng,[](int,const auto&){});auto m=retail_windstrip::BillboardMatrix(s.halo,{0,0,s.halo});for(int r=0;r<4;++r)for(int c=0;c<4;++c)printf(" %.9g",m.Elements[r][c]);puts("");}}
'''
def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False);p=out/'geometry.cpp';p.write_text(DRIVER);binary=out/'geometry';subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(p),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
 q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);(out/'source.trace').write_text(q.stdout)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/wind.i3d'))
 f=WindStripScene(exe,data);f.raster_enabled=False;maximum=0;fields=0;errors=[];stack_values=set()
 from unicorn import UC_HOOK_CODE
 from unicorn.x86_const import UC_X86_REG_ESP
 def width(uc,*_):stack_values.add(f.vm.u32(uc.reg_read(UC_X86_REG_ESP)+0x1c))
 f.vm.uc.hook_add(UC_HOOK_CODE,width,begin=0x50be30,end=0x50be30)
 for tick,line in enumerate(q.stdout.splitlines(),1):
  f.advance();f.render();port=list(map(float,line.split()))[1:];native=[]
  for packet in f.draws[:20]:
   assert packet['arguments'][1]==482 and packet['arguments'][3]==20
   for v in struct.iter_unpack('<8f',bytes.fromhex(packet['vertices_hex'])):native.extend(v[:3]+v[6:])
  if f.state()['halo']>0:native.extend(f.draws[-1]['matrix'])
  for k,(a,b)in enumerate(zip(native,port)):
   delta=abs(a-b);maximum=max(maximum,delta);fields+=1
   if delta>3e-5 and len(errors)<50:errors.append(dict(tick=tick,field=k,native=a,source=b))
  if tick%50==0:print(tick,maximum,errors[:2],flush=True)
 report=dict(status='fail'if errors else'pass',fields=fields,max_float_error=maximum,errors=errors,incoming_width_step_bits=list(map(hex,sorted(stack_values))),source_sha256=sha((ROOT/'src/game/effects/windstripmatrix.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Full original GenerateStrip and RenderObject draw packets; independent compiled20x20 generated vertices/UVs and originalhalo matrix over251 activeframes. Tiny uninitialized equal-width step residues truncate tozero, retained as incoming caller-stack limitation.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
