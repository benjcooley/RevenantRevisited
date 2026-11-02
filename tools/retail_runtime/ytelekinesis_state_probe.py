"""Compare compiled production YTelekinesis core to independent original state."""
import argparse,json,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha

def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);native=json.loads(trace.read_text());cpp=r'''
#include <cstdio>
#include "ytelekinesiscore.h"
unsigned seed=1,draws=0;int random_range(int lo,int hi){++draws;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
void dump(int tick,const ytelekinesis_core::State&s){printf("S %d %d %d %u %u\n",tick,s.count,int(s.alive),draws,seed);for(int i=0;i<s.count;++i){const auto&r=s.rocks[i];printf("R %d %d",tick,i);for(auto v:{r.position,r.velocity,r.rotation,r.rotation_velocity,r.scale})printf(" %.9g %.9g %.9g",v.x,v.y,v.z);printf(" %d %d %d %d %d\n",r.dead,r.hit,r.mesh,r.angle,r.life);}}
int main(){ytelekinesis_core::State s;s.Initialize(random_range);dump(-1,s);for(int i=0;i<TICKS;++i){s.Pulse();dump(i,s);}}
'''.replace('TICKS',str(len(native['records'])))
 p=output/'core.cpp';p.write_text(cpp);exe=output/'core';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p),'-o',str(exe)],check=True)
 raw=subprocess.check_output([str(exe)],text=True);(output/'source.trace').write_text(raw);states={};rocks={}
 for line in raw.splitlines():
  kind,t,*values=line.split();t=int(t)
  if kind=='S':states[t]=list(map(int,values))
  else:rocks.setdefault(t,[]).append(list(map(float,values[1:])))
 errors=[];maximum=0;fields=0
 for tick,n in [(-1,native['initial'])]+[(r['tick'],r['state'])for r in native['records']]:
  assert states[tick]==[len(n['rocks']),int(not(n['flags']&4096)),n['rng_draws'],n['seed']],(tick,states[tick],n)
  for i,(s,r)in enumerate(zip(rocks[tick],n['rocks'])):
   expected=sum([r[k]for k in ['position','velocity','rotation','rotation_velocity','scale']],[])+[r[k]for k in ['dead','hit','mesh','angle','life']]
   for k,(a,b)in enumerate(zip(s,expected)):
    d=abs(a-b);maximum=max(maximum,d);fields+=1
    if d>max(5e-5,abs(b)*1e-7):errors.append(dict(tick=tick,rock=i,field=k,source=a,native=b,error=d))
 report=dict(status='pass'if not errors else'fail',fields=fields,max_error=maximum,errors=errors[:20],trace_sha256=sha(trace.read_bytes()),source_sha256=sha((ROOT/'src/game/effects/ytelekinesiscore.h').read_bytes()))
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);assert not errors
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
