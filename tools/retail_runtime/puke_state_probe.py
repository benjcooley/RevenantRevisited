#!/usr/bin/env python3
"""Compile actual Puke core and compare every lifecycle field/RNG to original."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from puke_visual_reference import PukeScene,ROOT,sha
DRIVER=r'''
#include "pukecore.h"
#include <cstdio>
#include <cstdint>
int main(){retail_puke::State s;uint32_t seed=1;for(int t=0;t<=32;++t){if(t)s.Advance([&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);});printf("%d %d %d %.9g %u",t,s.alive,s.life,s.scale,seed);for(int i=0;i<25;++i){printf(" %d",s.starts[i]);for(auto v:s.position[i])printf(" %.9g",v);for(auto v:s.velocity[i])printf(" %.9g",v);}puts("");}}
'''
def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False);driver=out/'core.cpp';driver.write_text(DRIVER);binary=out/'core'
 subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],check=True);q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);(out/'source.trace').write_text(q.stdout);source=[l.split()for l in q.stdout.splitlines()]
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/puke.i3d'))
 f=PukeScene(exe,data);rows=[];fields=0;maximum=0;errors=[]
 for tick,port in enumerate(source):
  if tick:f.advance()
  n=f.state();flat=[tick,int(n['alive']),n['life'],n['scale'],n['seed']]
  for i in range(25):flat.extend([n['starts'][i],*n['positions'][i],*n['velocities'][i]])
  assert len(flat)==len(port)
  for k,(expected,actual)in enumerate(zip(flat,map(float,port))):
   delta=abs(expected-actual);maximum=max(maximum,delta);fields+=1
   if delta>2e-6:errors.append(dict(tick=tick,field=k,native=expected,port=actual))
  before=f.state();f.raster_enabled=False;f.render();assert before==f.state();rows.append(n)
 report=dict(status='fail'if errors else'pass',fields=fields,max_float_error=maximum,errors=errors,native=rows,source_sha256=sha((ROOT/'src/pukecore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),driver_sha256=sha(driver.read_bytes()),first_dead=next(t for t,r in enumerate(rows)if not r['alive']),controls='All 33 states and RNG; full original Render on every live tick leaves state/RNG unchanged; post-kill updates preserve state; source ASan/UBSan.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,errors[:3]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
