#!/usr/bin/env python3
"""Independent WindStrip20-strip/400-drop source vs complete original lifecycle."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from windstrip_visual_reference import WindStripScene,ROOT,sha
DRIVER=r'''
#include "windstripcore.h"
#include <cstdio>
#include <cstdlib>
int main(int argc,char**argv){int stride=atoi(argv[1]);retail_windstrip::State s;unsigned seed=1;int calls=0;auto rng=[&](int a,int b){++calls;seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);for(int t=0;t<=255;++t){if(t)s.Animate();if(t%stride==0)s.RenderDrops(rng,[](int,const auto&){});printf("%d %d %d %.9g %u %d",t,s.alive,s.frame,s.halo,seed,calls);for(const auto&x:s.strips){for(float v:{x.r,x.theta,x.h,x.amplitude,x.dr,x.dtheta,x.dh,x.da})printf(" %.9g",v);for(auto p:x.points)printf(" %.9g %.9g %.9g",p.x,p.y,p.z);}for(auto d:s.drops){bool used=s.alive&&d.used;printf(" %d",used);for(float v:{d.position.x,d.position.y,d.position.z,d.velocity.x,d.velocity.y,d.velocity.z,d.scale})printf(" %.9g",used?v:0.f);}puts("");}}
'''
def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False);driver=out/'core.cpp';driver.write_text(DRIVER);binary=out/'core';subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],check=True)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/wind.i3d'))
 results=[]
 for stride in(1,3):
  q=subprocess.run([str(binary),str(stride)],capture_output=True,text=True,check=True);(out/f'source-{stride}.trace').write_text(q.stdout);f=WindStripScene(exe,data);f.raster_enabled=False;fields=0;maximum=0;errors=[];rows=[]
  for tick,line in enumerate(q.stdout.splitlines()):
   if tick:f.advance()
   if tick%stride==0 and f.state()['alive']:f.render()
   n=f.full_state();flat=[tick,int(n['alive']),n['frame'],n['halo'],n['seed'],n['rng_draws']]
   for group in(n['strips'],n['drops']):
    for row in group:flat.extend(row)
   port=list(map(float,line.split()));assert len(flat)==len(port),(len(flat),len(port))
   for k,(expected,actual)in enumerate(zip(flat,port)):
    delta=abs(expected-actual);maximum=max(maximum,delta);fields+=1
    if delta>3e-5 and len(errors)<100:errors.append(dict(tick=tick,field=k,native=expected,port=actual))
   rows.append(n)
   if tick%25==0:print(stride,tick,maximum,errors[:2],flush=True)
  results.append(dict(stride=stride,fields=fields,max_float_error=maximum,errors=errors,native=rows))
 report=dict(status='fail'if any(r['errors']for r in results)else'pass',cases=results,source_sha256=sha((ROOT/'src/game/effects/windstripcore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),controls='Actual owner Pulse then NextFrame then custom Animate; original full Render each stride1/3.20 independent strip histories and400 drop slots, live-only payload, exact RNG; natural null spell. ASan/UBSan compiled source.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
