"""Independent actual JhagaAttack controller pools versus the production particle core."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from fireball_visual_reference import read_fireball
DRIVER=r'''
#include "authoredpartsys.h"
#include <cstdio>
#include <fstream>
using namespace authored_partsys;
int main(int argc,char**argv){if(argc!=5)return 2;Definition defs[3];std::string diag;
for(int i=0;i<3;++i)if(!ParseDefinition(argv[i+2],defs[i],diag)){fprintf(stderr,"%s",diag.c_str());return 3;}
std::ifstream f(argv[1],std::ios::binary);int count;f.read((char*)&count,4);State states[2];int previous=-1;unsigned seed=1;
RandomRange rng=[&](int lo,int hi){seed=seed*214013u+2531011u;return lo+((seed>>16)&32767u)%(hi-lo+1);};
for(int tick=0;tick<count;++tick){int st,n,render;f.read((char*)&st,4);f.read((char*)&n,4);f.read((char*)&render,4);TickInputs inputs[2];
for(int j=0;j<n;++j){auto&in=inputs[j];f.read((char*)&in.animation_frame,4);in.has_object_zero_origin=true;in.emitters.resize(1);f.read((char*)in.owner_position.data(),12);f.read((char*)in.object_zero_origin.data(),12);auto&e=in.emitters[0];f.read((char*)e.position.data(),12);f.read((char*)e.scale.data(),12);f.read((char*)e.matrix.data(),64);for(int k=0;k<3;++k)e.origin[k]=e.matrix[12+k];in.ground_height=[](int x,int,int){return 1;};
if(st!=previous && !states[j].Initialize(defs[st==1?0:1+j],in,0,diag))return 4;states[j].Advance(in,rng);}
previous=st;
if(render)for(int j=0;j<n;++j)for(size_t i=0;i<states[j].Capacity();++i)if(states[j].Particles()[i].alive)states[j].SampleRender(i,rng);
printf("R %d %u\n",tick,seed);
for(int j=0;j<n;++j)for(size_t i=0;i<states[j].Capacity();++i){const auto&p=states[j].Particles()[i];printf("P %d %d %zu %d %d",tick,j,i,p.alive,p.age);for(float v:p.position)printf(" %.9g",v);for(float v:p.velocity)printf(" %.9g",v);printf(" %.9g %d %.9g",states[j].EmissionCredit(),p.lifespan,p.scale);for(float v:p.color)printf(" %.9g",v);printf(" %.9g %.9g %.9g\n",p.alpha,p.friction,p.gravity);}}
return f?0:5;}
'''
def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);data=json.loads(trace.read_text());records=data['records'];records=records[:next((i+1 for i,r in enumerate(records)if r['after']['flags']&4096),len(records))];source=output/'state.cpp';source.write_text(DRIVER);binary=output/'state'
 q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Compile failed')
 raw=struct.pack('<i',len(records));rows=[];seeds=[]
 for r in records:
  b=r['before'];a=r['after'];cs=a['controllers'];raw+=struct.pack('<3i',b['state'],len(cs),int(r['rendered'] and not(a['flags']&0x1000)));seeds.append(a['seed'])
  for j,c in enumerate(cs):
   p=b['pulse_inputs'][str(c['address'])];e=p['emitters'][0];raw+=struct.pack('<i28f',p['frame'],*p['position'],*[p['origin'][k]-p['position'][k] for k in range(3)],*e['position'],*e['scale'],*e['matrix'])
   pool=bytes.fromhex(c['pool_hex']);rf=lambda o:struct.unpack_from('<f',pool,o)[0];ri=lambda o:struct.unpack_from('<i',pool,o)[0]
   for i in range(c['capacity']):
    o=i*252;rows.append([r['tick'],j,i,ri(o+0xe4),ri(o+0xd0),*[rf(o+0xb8+k*4)for k in range(3)],*[rf(o+0xc4+k*4)for k in range(3)],c['credit'],ri(o+0xd4),rf(o+0x64),*[rf(o+0x50+k*4)for k in range(3)],rf(o+0x60),rf(o+0x4c),rf(o)])
 inp=output/'inputs.bin';inp.write_bytes(raw)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_fireball(z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/jattack.i3d')))
 tags=[t['parameters']for t in asset['tags']if t['name']=='partsys'];q=subprocess.run([str(binary),str(inp),*tags],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'controls.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Source run failed')
 parsed=[l.split()for l in q.stdout.splitlines()];port=[[float(v)for v in p[1:]]for p in parsed if p[0]=='P'];rng=[int(p[2])for p in parsed if p[0]=='R'];assert len(rows)==len(port)
 errors=[];maximum=0;checks=0;born=set();last_state=-1
 for a,b in zip(rows,port):
  st=records[int(a[0])]['after']['state']
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
