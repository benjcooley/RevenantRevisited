"""Compile the production inline PartSys against the genuine loaded steam pool.

Every Pulse and Render RNG call is retained. Native cached emitter inputs are
explicit; independent authored pose and actual GPU comparisons are separate.
"""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from speed_controller_preflight import read_asset
DRIVER=r'''
#include "authoredpartsys.h"
#include <cstdio>
#include <fstream>
using namespace authored_partsys;
int main(int argc,char**argv){Definition d;std::string diag;if(!ParseDefinition(argv[2],d,diag))return 3;std::ifstream f(argv[1],std::ios::binary);int count;f.read((char*)&count,4);State state;unsigned seed=1;
RandomRange rng=[&](int lo,int hi){seed=seed*214013u+2531011u;return lo+((seed>>16)&32767u)%(hi-lo+1);};
for(int tick=0;tick<count;++tick){int advance,render,frame;f.read((char*)&advance,4);f.read((char*)&render,4);f.read((char*)&frame,4);TickInputs in;in.animation_frame=frame;in.has_object_zero_origin=true;in.emitters.resize(1);f.read((char*)in.owner_position.data(),12);f.read((char*)in.object_zero_origin.data(),12);auto&e=in.emitters[0];f.read((char*)e.position.data(),12);f.read((char*)e.scale.data(),12);f.read((char*)e.matrix.data(),64);for(int i=0;i<3;++i)e.origin[i]=e.matrix[12+i];in.ground_height=[](int,int,int){return 0;};
if(tick==0&&!state.Initialize(d,in,0,diag))return 4;if(advance)state.Advance(in,rng);if(render)for(size_t i=0;i<state.Capacity();++i)if(state.Particles()[i].alive)state.SampleRender(i,rng);
for(size_t i=0;i<state.Capacity();++i){const auto&p=state.Particles()[i];printf("%d %zu %u %d %d",tick+1,i,seed,p.alive,p.age);for(float v:p.position)printf(" %.9g",v);for(float v:p.velocity)printf(" %.9g",v);printf(" %.9g %d %.9g",state.EmissionCredit(),p.lifespan,p.scale);for(float v:p.color)printf(" %.9g",v);printf(" %.9g %.9g %.9g\n",p.alpha,p.friction,p.gravity);}}
return f?0:5;}
'''
def run(trace,out):
 out.mkdir(parents=True,exist_ok=False);d=json.loads(trace.read_text());records=d['rows'];source=out/'probe.cpp';source.write_text(DRIVER);binary=out/'probe'
 q=subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
 raw=struct.pack('<i',len(records));rows=[];last=d['initial_inputs'][0];initial_origin=[x-y for x,y in zip(last['origin'],last['position'])]
 for r in records:
  assert len(r['pulse_inputs'])<=1
  if r['pulse_inputs']:last=r['pulse_inputs'][0]
  p=last;e=p['emitters'][0];raw+=struct.pack('<3i28f',bool(r['pulse_inputs']),not bool(r['flags']&0x1000),p['frame'],*p['position'],*initial_origin,*e['position'],*e['scale'],*e['matrix'])
  pool=bytes.fromhex(r['pool_hex']);rf=lambda off:struct.unpack_from('<f',pool,off)[0];ri=lambda off:struct.unpack_from('<i',pool,off)[0]
  for slot in range(104):
   off=slot*252;rows.append([r['tick'],slot,r['seed'],ri(off+0xe4),ri(off+0xd0),*[rf(off+0xb8+j*4)for j in range(3)],*[rf(off+0xc4+j*4)for j in range(3)],r['emission_credit'],ri(off+0xd4),rf(off+0x64),*[rf(off+0x50+j*4)for j in range(3)],rf(off+0x60),rf(off+0x4c),rf(off)])
 inp=out/'inputs.bin';inp.write_bytes(raw)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_asset(z.read('Imagery/Cave/CavSGeyser.I3D'))
 tag=asset['tags'][0]['parameters'];q=subprocess.run([str(binary),str(inp),tag],capture_output=True,text=True);(out/'source.trace').write_text(q.stdout);assert q.returncode==0,q.stderr
 port=[[float(v)for v in l.split()]for l in q.stdout.splitlines()];assert len(rows)==len(port);born=set();errors=[];checks=0;maximum=0
 for a,b in zip(rows,port):
  if a[3]:born.add(a[1])
  for i,(x,y)in enumerate(zip(a,b)):
   if a[1]not in born and i in(13,17):continue
   error=abs(x-y);maximum=max(maximum,error);checks+=1
   if error>(0 if i in(0,1,2,3,4,12)else .002):errors.append(dict(tick=a[0],slot=a[1],field=i,native=x,source=y))
 negatives={}
 for name,body in [('skipped_render_rng',DRIVER.replace('if(render)for','if(false)for')),('advanced_cached_frame',DRIVER.replace('in.animation_frame=frame;','in.animation_frame=frame+1;'))]:
  wrong=out/(name+'.cpp');wrong.write_text(body);exe=out/name
  subprocess.run(['clang++','-std=c++17','-O1','-iquote',str(ROOT/'src'),str(wrong),str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(exe)],check=True)
  bad=subprocess.check_output([str(exe),str(inp),tag],text=True);different=sum(a!=b for a,b in zip(bad.splitlines(),q.stdout.splitlines()));assert different>0;negatives[name]=dict(rejected=True,differing_rows=different)
 result=dict(negative_controls=negatives,status='fail'if errors else'pass',fields=checks,max_error=maximum,errors=errors[:80],error_count=len(errors),ground_queries=sum(len(r['ground'])for r in records),ground_results=sorted(set(q['result']for r in records for q in r['ground'])),initial_origin=initial_origin,native_sha256=sha(trace.read_bytes()),accepted=False,scope=__doc__);(out/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
