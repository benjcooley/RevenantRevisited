#!/usr/bin/env python3
"""Actual Smoke source50-puff state/RNG against complete original Init/Animate."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from smoke_visual_reference import NativeSmoke,ROOT,sha
DRIVER=r'''
#include "smokecore.h"
#include <cstdio>
#include <cstdint>
int main(){for(int quality:{0,1,3}){retail_smoke::State s;uint32_t seed=1;auto rng=[&](){seed=seed*214013u+2531011u;return int((seed>>16)&32767u);};s.Initialize(rng,quality);for(int t=0;t<=600;++t){if(t)s.Advance(rng);printf("%d %d %d %d %u %.9g %.9g %.9g %.9g",quality,t,s.alive,s.ticks,seed,s.centerx,s.centery,s.vx,s.vy);for(auto p:s.puffs){for(float f:{p.x,p.y,p.z,p.size,p.vx,p.vy,p.vz,p.vsize,p.rotation})printf(" %.9g",f);printf(" %d",p.life);}puts("");}}}
'''
def run(out):
 out.mkdir(parents=True,exist_ok=False);driver=out/'core.cpp';driver.write_text(DRIVER);binary=out/'core';subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],check=True);q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);(out/'source.trace').write_text(q.stdout);lines=q.stdout.splitlines()
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/smoke.i3d'))
 fields=0;maximum=0;errors=[];results=[]
 for group,quality in enumerate((0,1,3)):
  f=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,quality);f.raster_enabled=False
  for tick in range(601):
   if tick:f.advance()
   n=f.state();flat=[quality,tick,int(n['alive']),n['tick'],n['seed'],*n['center'],*[v for row in n['slots']for v in row]];port=list(map(float,lines[group*601+tick].split()));assert len(flat)==len(port)
   for k,(expected,actual)in enumerate(zip(flat,port)):
    delta=abs(expected-actual);maximum=max(maximum,delta);fields+=1
    if delta>2e-5 and len(errors)<20:errors.append(dict(quality=quality,tick=tick,field=k,native=expected,port=actual))
   if tick%100==0:before=f.state();f.render();assert before==f.state()
  results.append(dict(quality=quality,alive=n['alive'],ticks=n['tick'],seed=n['seed']))
 report=dict(status='fail'if errors else'pass',fields=fields,max_error=maximum,errors=errors,results=results,source_sha256=sha((ROOT/'src/game/effects/smokecore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='601ticks each at original quality0/1 immediatekill and quality3 live50-puff reset/centerbounce; allfields andMSVCstream, completeoriginalRender probes preservestate/RNG, compiled actualsource ASan/UBSan.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
