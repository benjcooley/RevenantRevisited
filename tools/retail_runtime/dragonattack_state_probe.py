"""Dragon's actual base-before-owner controller scheduling versus shared core.

Controller state comes from the actual controller, not the owner: original
Effect::Pulse updates it before DragonAttack::Pulse changes owner state.
"""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from fireball_visual_reference import read_fireball
from headfireball_state_probe import DRIVER
def run(trace,output):
 ground_height=0
 if ground_height not in(0,1):raise ValueError('Only declared ground0/1 fixtures are supported')
 output.mkdir(parents=True,exist_ok=True);data=json.loads(trace.read_text());assert data['type_id']=='0xba09fead' and data['effect']=='dragonattack';records=data['records'];records=records[:next((i+1 for i,r in enumerate(records)if r['after']['flags']&4096),len(records))];source=output/'state.cpp';source.write_text(DRIVER.replace('in.ground_height=[](int x,int,int){return 1;};',f'in.ground_height=[](int x,int,int){{return {ground_height};}};'));binary=output/'state'
 q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Compile failed')
 raw=struct.pack('<i',len(records));rows=[];seeds=[]
 for r in records:
  b=r['before'];a=r['after'];cs=a['controllers'];raw+=struct.pack('<3i',cs[0]['state'] if cs else 0,len(cs),int(bool(cs) and cs[0]['state']==a['state'] and not(a['flags']&0x1000)));seeds.append(a['seed'])
  for j,c in enumerate(cs):
   p=b['pulse_inputs'][str(c['address'])];e=p['emitters'][0];raw+=struct.pack('<i28f',p['frame'],*p['position'],*[p['origin'][k]-p['position'][k] for k in range(3)],*e['position'],*e['scale'],*e['matrix'])
   pool=bytes.fromhex(c['pool_hex']);rf=lambda o:struct.unpack_from('<f',pool,o)[0];ri=lambda o:struct.unpack_from('<i',pool,o)[0]
   for i in range(c['capacity']):
    o=i*252;rows.append([r['tick'],j,i,ri(o+0xe4),ri(o+0xd0),*[rf(o+0xb8+k*4)for k in range(3)],*[rf(o+0xc4+k*4)for k in range(3)],c['credit'],ri(o+0xd4),rf(o+0x64),*[rf(o+0x50+k*4)for k in range(3)],rf(o+0x60),rf(o+0x4c),rf(o)])
 inp=output/'inputs.bin';inp.write_bytes(raw)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_fireball(z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/hfire.i3d')))
 tags=[t['parameters']for t in asset['tags']if t['name']=='partsys'];q=subprocess.run([str(binary),str(inp),*tags],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'controls.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Source run failed')
 parsed=[l.split()for l in q.stdout.splitlines()];port=[[float(v)for v in p[1:]]for p in parsed if p[0]=='P'];rng=[int(p[2])for p in parsed if p[0]=='R'];assert len(rows)==len(port)
 errors=[];maximum=0;checks=0;born=set();last_state=-1
 for a,b in zip(rows,port):
  st=records[int(a[0])]['after']['controllers'][int(a[1])]['state']
  if st!=last_state:born.clear();last_state=st
  slot=tuple(a[1:3])
  if a[3]:born.add(slot)
  for i,(x,y)in enumerate(zip(a,b)):
   if slot not in born and i in(13,17):continue
   d=abs(x-y);maximum=max(maximum,d);checks+=1
   if d>(0 if i in(0,1,2,3,4,12)else .001):errors.append(dict(tick=a[0],controller=a[1],slot=a[2],field=i,native=x,source=y,error=d))
 for t,(a,b)in enumerate(zip(seeds,rng)):
  if a!=b:errors.append(dict(tick=t,native_seed=a,source_seed=b))
 report=dict(status='fail'if errors else'pass',checks=checks,maximum_error=maximum,errors=errors[:80],error_count=len(errors),trace_sha256=sha(trace.read_bytes()),driver_sha256=sha(source.read_bytes()),production_sha256={p:sha((ROOT/p).read_bytes())for p in('src/authoredpartsys.cpp','src/partsysdefinition.cpp')},inputs_sha256=sha(raw),sanitizers='ASan/UBSan')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,len(errors));return report

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
