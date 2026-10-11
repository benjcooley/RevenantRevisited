#!/usr/bin/env python3
"""Compiled independent EnergySpray state/RNG vs actual owner/leaf/render calls."""
import argparse,json,subprocess,struct,zipfile
from pathlib import Path
from energyspray_visual_reference import EnergySprayScene,ROOT,sha
DRIVER=r'''
#include "energyspraycore.h"
#include <cstdio>
#include <cstdlib>
int main(int argc,char**argv){int stride=atoi(argv[1]);retail_energyspray::State s;uint32_t seed=1;int calls=0;auto rng=[&](int a,int b){++calls;seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);for(int t=0;t<=140;++t){if(t){s.Pulse();s.Animate(rng);}float render=0;if(t%stride==0 && s.alive)render=s.RenderScale(rng);printf("%d %d %d %d %u %d %.9g",t,s.alive,s.phase,s.countdown,seed,calls,s.core_scale);for(auto&array:{s.angles_a,s.angles_b,s.lengths})for(auto x:array)printf(" %.9g",x);for(auto&array:s.colors)for(auto x:array)printf(" %d",x);for(auto&array:s.directions)for(auto x:array)printf(" %d",x);for(auto&array:{s.radii,s.radial_angles,s.velocities,s.scales})for(auto x:array)printf(" %.9g",x);for(auto x:s.status)printf(" %d",x);puts("");}}
'''
def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False);driver=out/'core.cpp';driver.write_text(DRIVER);binary=out/'core';subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],check=True)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 results=[]
 for stride in(1,3):
  q=subprocess.run([str(binary),str(stride)],capture_output=True,text=True,check=True);(out/f'source-{stride}.trace').write_text(q.stdout);source=[l.split()for l in q.stdout.splitlines()];f=EnergySprayScene(exe,data);f.raster_enabled=False;fields=0;maximum=0;errors=[];rows=[]
  for tick,port in enumerate(source):
   if tick:f.advance()
   before=f.state();
   if tick%stride==0 and before['alive']:f.render()
   n=f.state();assert n['rng_draws']-before['rng_draws']==int(tick%stride==0 and before['alive'])
   flat=[tick,int(n['alive']),n['phase'],struct.unpack('<i',struct.pack('<I',n['countdown']))[0],n['seed'],n['rng_draws'],n['core_scale']]
   for k in('angles_a','angles_b','lengths','colors','directions','radii','radial_angles','velocities','scales','status'):flat.extend(n[k])
   assert len(flat)==len(port),(len(flat),len(port))
   for k,(expected,actual)in enumerate(zip(flat,map(float,port))):
    delta=abs(expected-actual);maximum=max(maximum,delta);fields+=1
    if delta>2e-5:errors.append(dict(tick=tick,field=k,native=expected,port=actual))
   rows.append(n)
  results.append(dict(stride=stride,fields=fields,max_float_error=maximum,errors=errors,native=rows));print(stride,fields,maximum,errors[:3],flush=True)
 report=dict(status='fail'if any(r['errors']for r in results)else'pass',cases=results,source_sha256=sha((ROOT/'src/energyspraycore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),controls='Complete original owner/leaf calls; exact RNG everytick and one extra per actual Render; stride1/3 changes source stream together; post-kill no-op; ASan/UBSan compiled core.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
