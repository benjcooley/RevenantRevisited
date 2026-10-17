#!/usr/bin/env python3
"""Compiled actual Shriek core versus every original slot/counter/RNG field."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from shriek_visual_reference import ShriekScene,ROOT,sha
DRIVER=r'''
#include "shriekcore.h"
#include <cstdio>
#include <cstdint>
int main(){retail_shriek::State s;uint32_t seed=1;s.Initialize([&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);});for(int t=0;t<=50;++t){if(t)s.Advance();printf("%d %d %d %u",t,s.alive,s.counter,seed);for(int i=0;i<8;++i)printf(" %d %.9g %.9g %.9g",s.starts[i],s.scale[i],s.position[i],s.fade[i]);puts("");}}
'''
def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False);driver=out/'core.cpp';driver.write_text(DRIVER);binary=out/'core';subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],check=True);q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);(out/'source.trace').write_text(q.stdout);source=[l.split()for l in q.stdout.splitlines()]
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/shriek.i3d'))
 f=ShriekScene(exe,data);rows=[];fields=0;maximum=0;errors=[]
 for tick,port in enumerate(source):
  if tick:f.advance()
  n=f.state();flat=[tick,int(n['alive']),n['counter'],n['seed']]
  for i in range(8):flat.extend([n['starts'][i],n['scales'][i],n['positions'][i],n['fades'][i]])
  assert len(flat)==len(port)
  for k,(expected,actual)in enumerate(zip(flat,map(float,port))):
   delta=abs(expected-actual);maximum=max(maximum,delta);fields+=1
   if delta>2e-6:errors.append(dict(tick=tick,field=k,native=expected,port=actual))
  before=f.state();f.raster_enabled=False;f.render();assert before==f.state();rows.append(n)
 report=dict(status='fail'if errors else'pass',fields=fields,max_float_error=maximum,errors=errors,native=rows,source_sha256=sha((ROOT/'src/shriekcore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),driver_sha256=sha(driver.read_bytes()),first_dead=next(t for t,r in enumerate(rows)if not r['alive']),controls='All51 states/RNG; full original live Render does not alter state/RNG; post-kill no-op; ASan/UBSan source.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,errors[:3]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
