import sys,subprocess,struct,json,zipfile,argparse
from pathlib import Path
sys.path.insert(0,'tools/retail_runtime')
from funnel_state_probe import DRIVER,ROOT,run as state_run
from speed_profile_contract import sha
from fireball_visual_reference import read_fireball
p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();out=a.output;out.mkdir(parents=True,exist_ok=True);trace=a.trace
assert state_run(trace,out/'pool')['status']=='pass'
s=DRIVER.replace('#include "authoredpartsys.h"','#include "authoredpartsys.h"\n#include "speedauthoredmatrix.h"')
s=s.replace('states[j].SampleRender(i,rng);',r'''{auto q=states[j].SampleRender(i,rng);if(q.scale>1e-5f){hmm_mat4 m;hmm_vec3 p{q.position[0],q.position[1],q.position[2]},r{q.rotation[0],q.rotation[1],q.rotation[2]},scale{q.scale,q.scale,q.scale};BuildRetailSpeedCameraMatrix(m,p,r,scale);printf("D %d %d %zu",tick,j,i);for(int k=0;k<16;++k)printf(" %.9g",(&m.Elements[0][0])[k]);for(float c:q.color)printf(" %d",int(c)&255);puts("");}}''')
p=out/'render.cpp';p.write_text(s);exe=out/'render';q=subprocess.run(['clang++','-std=c++17','-O1','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(p),str(ROOT/'src/engine/fx/authoredpartsys.cpp'),str(ROOT/'src/engine/fx/partsysdefinition.cpp'),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(exe)],capture_output=True,text=True);(out/'compile.log').write_text(q.stderr);assert q.returncode==0
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:a=read_fireball(z.read('Imagery/Magic/funnel.i3d'))
q=subprocess.run([str(exe),str(out/'pool/inputs.bin'),*[t['parameters']for t in a['tags']if t['name']=='partsys']],capture_output=True,text=True);(out/'source.trace').write_text(q.stdout);assert q.returncode==0
port={}
for l in q.stdout.splitlines():
 a=l.split()
 if a[0]=='D':port.setdefault(int(a[1]),[]).append((int(a[2]),list(map(float,a[4:20])),list(map(int,a[20:]))))
n=json.loads(trace.read_text());errors=[];checks=0;maximum=0
for r in n['records']:
 if not r['draws']:continue
 original=[d for d in r['draws']if d['object']in(4,6)];source=port.get(r['tick'],[])
 assert len(original)==len(source),(r['tick'],len(original),len(source))
 for o,(obj,m,c)in zip(original,source):
  assert o['object']==[4,6][obj]
  packed=struct.unpack_from('<I',bytes.fromhex(o['vertices_hex']),16)[0];native_color=[(packed>>16)&255,(packed>>8)&255,packed&255]
  if native_color!=c:errors.append(dict(tick=r['tick'],native_color=native_color,source_color=c))
  delta=max(abs(x-y)for x,y in zip(o['matrix'],m));maximum=max(maximum,delta);checks+=16
  if delta>.0003:errors.append(dict(tick=r['tick'],object=o['object'],native=o['matrix'],source=m,error=delta))
report=dict(status='fail'if errors else'pass',checks=checks,maximum=maximum,errors=errors[:8],error_count=len(errors),trace_sha256=sha(trace.read_bytes()),driver_sha256=sha(p.read_bytes()),scope='Actual full default Render FVF_LVERTEX matrices/packed RGB versus production SampleRender and shared camera matrix; no host colors or matrices injected into original.');(out/'manifest.json').write_text(json.dumps(report,indent=2));print(checks,maximum,len(errors))
