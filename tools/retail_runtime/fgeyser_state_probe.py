#!/usr/bin/env python3
"""Two simultaneous original fgeyser controllers versus production State."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from speed_controller_preflight import read_asset
DRIVER=r'''
#include "authoredpartsys.h"
#include <cstdio>
#include <fstream>
using namespace authored_partsys;
int main(int argc,char**argv){if(argc!=4)return 2;Definition definitions[2];std::string diag;
for(int i=0;i<2;++i)if(!ParseDefinition(argv[i+2],definitions[i],diag))return 3;
std::ifstream f(argv[1],std::ios::binary);int count;f.read((char*)&count,4);State states[2];bool initialized=false;unsigned seed=1;
RandomRange rng=[&](int lo,int hi){seed=seed*214013u+2531011u;return lo+((seed>>16)&32767u)%(hi-lo+1);};
for(int tick=0;tick<count;++tick){int advance,render;f.read((char*)&advance,4);f.read((char*)&render,4);
for(int controller=0;controller<2;++controller){int frame;f.read((char*)&frame,4);TickInputs in;in.animation_frame=frame;in.has_object_zero_origin=true;in.emitters.resize(1);f.read((char*)in.owner_position.data(),12);f.read((char*)in.object_zero_origin.data(),12);
auto&e=in.emitters[0];f.read((char*)e.position.data(),12);f.read((char*)e.scale.data(),12);f.read((char*)e.matrix.data(),64);for(int i=0;i<3;++i)e.origin[i]=e.matrix[12+i];in.ground_height=[](int,int,int){return 1;};
if(!initialized&&!states[controller].Initialize(definitions[controller],in,0,diag))return 4;
if(advance)states[controller].Advance(in,rng);}
initialized=true;
if(render)for(auto& state:states)for(size_t i=0;i<state.Capacity();++i)if(state.Particles()[i].alive)state.SampleRender(i,rng);
for(int controller=0;controller<2;++controller){auto&state=states[controller];for(size_t i=0;i<state.Capacity();++i){const auto&p=state.Particles()[i];printf("%d %d %zu %u %d %d",tick,controller,i,seed,p.alive,p.age);for(float v:p.position)printf(" %.9g",v);for(float v:p.velocity)printf(" %.9g",v);printf(" %.9g %d %.9g",state.EmissionCredit(),p.lifespan,p.scale);for(float v:p.color)printf(" %.9g",v);printf(" %.9g %.9g %.9g\n",p.alpha,p.friction,p.gravity);}}}
return f?0:5;}
'''
def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);data=json.loads(trace.read_text());records=data['records'];source=output/'state.cpp';source.write_text(DRIVER);binary=output/'state'
 cmd=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/engine/fx/authoredpartsys.cpp'),str(ROOT/'src/engine/fx/partsysdefinition.cpp'),'-o',str(binary)]
 q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Compile failed')
 raw=struct.pack('<i',len(records));rows=[];alive_before=True
 for record in records:
  b=record['before'];a=record['after'];raw+=struct.pack('<2i',int(alive_before),int(record['rendered']and not(a['flags']&0x1000)))
  for cidx,c in enumerate(a['controllers']):
   matches=[p for p in b['pulse_inputs']if p['controller']==c['address']];assert len(matches)==1,(record['tick'],cidx,len(matches));p=matches[0];e=p['emitters'][0]
   raw+=struct.pack('<i28f',p['frame'],*p['position'],*p['origin'],*e['position'],*e['scale'],*e['matrix'])
   pool=bytes.fromhex(c['pool_hex']);rf=lambda offset:struct.unpack_from('<f',pool,offset)[0];ri=lambda offset:struct.unpack_from('<i',pool,offset)[0]
   for i in range(c['capacity']):
    off=i*252;rows.append([record['tick'],cidx,i,a['seed'],ri(off+0xe4),ri(off+0xd0),*[rf(off+0xb8+j*4)for j in range(3)],*[rf(off+0xc4+j*4)for j in range(3)],c['credit'],ri(off+0xd4),rf(off+0x64),*[rf(off+0x50+j*4)for j in range(3)],rf(off+0x60),rf(off+0x4c),rf(off)])
  alive_before=not(a['flags']&0x1000)
 inp=output/'inputs.bin';inp.write_bytes(raw)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_asset(z.read('Imagery/Cave/cavfgeyser.i3d'))
 tags=[t['parameters']for t in asset['tags']if t['name']=='partsys'];q=subprocess.run([str(binary),str(inp),*tags],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'controls.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Execution failed')
 port=[[float(v)for v in l.split()]for l in q.stdout.splitlines()];assert len(rows)==len(port)
 errors=[];maximum=0;checks=0;born=set()
 for a,b in zip(rows,port):
  key=(a[1],a[2])
  if a[4]:born.add(key)
  for i,(x,y)in enumerate(zip(a,b)):
   if key not in born and i in(14,18):continue
   d=abs(x-y);maximum=max(maximum,d);checks+=1
   if d>(0 if i in(0,1,2,3,4,5,13)else .001):errors.append(dict(tick=a[0],controller=a[1],slot=a[2],field=i,native=x,source=y,error=d))
 report=dict(status='fail'if errors else'pass',checks=checks,rows=len(rows),maximum_error=maximum,errors=errors[:100],error_count=len(errors),trace_sha256=sha(trace.read_bytes()),source_sha256=sha(source.read_bytes()),production_sha256={p:sha((ROOT/p).read_bytes())for p in('src/engine/fx/authoredpartsys.cpp','src/engine/fx/partsysdefinition.cpp','src/engine/fx/authoredpartsys.h','src/engine/fx/partsysdefinition.h')},inputs_sha256=sha(raw),render_stride=data['render_stride'],sanitizers='ASan/UBSan',accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
