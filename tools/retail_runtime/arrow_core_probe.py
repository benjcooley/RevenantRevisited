"""Compile production QueenArrow motion/Render against independent native trace."""
import argparse,json,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha

def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);native=json.loads(trace.read_text())
 cpp=r'''
#include <cstdio>
#include "arrowmatrix.h"
uint32_t seed=1,draws=0;int random_range(int lo,int hi){++draws;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
int main(){arrow_core::State s;for(int tick=0;tick<TICKS;++tick){s.Pulse({-180,0,38},64,[](arrow_core::Point p){return p.x>=180;},random_range);int index=0;if(tick%STRIDE==0)s.Render(random_range,[&](const arrow_core::Draw& d){hmm_mat4 m;arrow_core::Matrix(m,d);printf("D %d %d %d",tick,index++,d.object);for(int j=0;j<16;++j)printf(" %.9g",(&m.Elements[0][0])[j]);puts("");});printf("S %d %d %d %d %.9g %.9g %d %d %d %d %u %u\n",tick,s.position.x,s.position.y,s.position.z,s.vx,s.vy,s.grow,s.color,s.countdown,int(s.alive),draws,seed);}}
'''.replace('TICKS',str(len(native['records']))).replace('STRIDE',str(native.get('render_stride',1)))
 p=output/'core.cpp';p.write_text(cpp);exe=output/'core';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-fsanitize=address,undefined','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(p),str(ROOT/'src/math3d.cpp'),'-o',str(exe)],check=True)
 raw=subprocess.check_output([str(exe)],text=True);(output/'source.trace').write_text(raw);states={};packets={}
 for line in raw.splitlines():
  kind,t,*values=line.split();t=int(t)
  if kind=='S':states[t]=list(map(float,values))
  else:packets.setdefault(t,[]).append(dict(object=int(values[1]),matrix=list(map(float,values[2:]))))
 errors=[];maximum=0;checks=0
 for r in native['records']:
  n=r['state'];s=states[r['tick']];expected=[*n['position'],*n['velocity'][:2],n['grow'],n['color'],n['countdown'],int(not(n['flags']&4096)),n['rng_draws'],n['seed']]
  assert s==expected,(r['tick'],s,expected)
  if not r['draws']:continue
  actual=packets[r['tick']];assert len(actual)==len(r['draws'])
  for i,(s,n)in enumerate(zip(actual,r['draws'])):
   assert s['object']==n['object'];d=max(abs(x-y)for x,y in zip(s['matrix'],n['matrix']));maximum=max(maximum,d);checks+=16
   if d>5e-5:errors.append(dict(tick=r['tick'],index=i,error=d,native=n['matrix'],source=s['matrix']))
 report=dict(status='pass'if not errors else'fail',ticks=len(states),state_fields=len(states)*11,matrix_fields=checks,max_matrix_error=maximum,errors=errors,trace_sha256=sha(trace.read_bytes()),source_sha256={p:sha((ROOT/p).read_bytes())for p in('src/arrowcore.h','src/arrowmatrix.h','src/math3d.cpp')})
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,maximum,errors[:1]);assert not errors;return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
