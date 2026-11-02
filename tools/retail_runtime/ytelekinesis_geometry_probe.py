"""Independent core draw-order, packed-color and matrix comparison with full native Render."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser();p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);native=json.loads(a.trace.read_text())
cpp=r'''
#include <cstdio>
#include "ytelekinesismatrix.h"
uint32_t seed=1;int random_range(int lo,int hi){seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
int main(){ytelekinesis_core::State s;s.Initialize(random_range);for(int tick=0;tick<TICKS;++tick){s.Pulse();if(!s.alive)continue;for(int i=0;i<s.count;++i){const auto&r=s.rocks[i];hmm_mat4 m;ytelekinesis_core::Matrix(m,r);printf("%d %d %d %d %u",tick,i,r.mesh,1,0xffffffffu);for(int j=0;j<16;++j)printf(" %.9g",(&m.Elements[0][0])[j]);puts("");}}}
'''.replace('TICKS',str(len(native['records'])))
source=a.output/'draw.cpp';source.write_text(cpp);binary=a.output/'draw';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
raw=subprocess.check_output([str(binary)],text=True);(a.output/'source.trace').write_text(raw);port={}
for line in raw.splitlines():
 t,i,o,mode,color,*m=line.split();port.setdefault(int(t),[]).append(dict(object=int(o),mode=int(mode),color=int(color),matrix=[float(x)for x in m]))
checks=0;maximum=0;errors=[]
for r in native['records']:
 if not r['draws']:continue
 actual=port[r['tick']];assert len(actual)==len(r['draws']),(r['tick'],len(actual),len(r['draws']))
 for i,(s,n)in enumerate(zip(actual,r['draws'])):
  assert s['object']==n['object'],(r['tick'],i)

  delta=max(abs(x-y)for x,y in zip(s['matrix'],n['matrix']));maximum=max(maximum,delta);checks+=16
  if delta>5e-5:errors.append(dict(tick=r['tick'],draw=i,error=delta,native=n['matrix'],port=s['matrix']))
report=dict(status='pass'if not errors else'fail',matrix_fields=checks,max_matrix_error=maximum,errors=errors,trace_sha256=sha(a.trace.read_bytes()),source_sha256={p:sha((ROOT/p).read_bytes())for p in('src/game/effects/ytelekinesiscore.h','src/game/effects/ytelekinesismatrix.h','src/engine/core/math3d.cpp','src/engine/render/retailsoftwaretransform.h')});(a.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,maximum,errors[:2]);assert not errors
