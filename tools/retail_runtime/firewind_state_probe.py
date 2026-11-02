"""Compile the FireWind core against original complete400-slot pool records."""
import argparse,json,struct,subprocess
from pathlib import Path
from runtime import Runtime
from fireflash_probe import function
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
v=Runtime(a.executable);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000);tables=a.output/'tables.bin';tables.write_bytes(bytes(v.uc.mem_read(0x634d44,1024)))
native=json.loads(a.trace.read_text());full='records'in native
rows=([native['initial']]+[r['state']for r in native['records']])if full else native['samples'];stride=native.get('render_stride',0)
cpp=r'''
#include <cstdio>
#include <fstream>
#include "firewindcore.h"
struct S3DPoint{int x,y,z;};short DistX[256],DistY[256];void ConvertToVector(int,int,S3DPoint&,int=0);
uint32_t seed=1,ranges=0,draws=0;int random_range(int lo,int hi){++ranges;if(lo==hi)return lo;++draws;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
VECTOR
int main(int argc,char**argv){std::ifstream t(argv[1],std::ios::binary);t.read((char*)DistX,512);t.read((char*)DistY,512);firewind_core::State s;s.Initialize(random_range);
 for(int tick=0;tick<TICKS;++tick){if(tick){s.Pulse(random_range,[](int a,int d){S3DPoint p;ConvertToVector(a,d,p);return hmm_vec3{float(p.x),float(p.y),float(p.z)};});if(STRIDE>0&&tick<=140&&tick%STRIDE==0)s.Render([](int,const hmm_mat4&){});}
 fwrite(&ranges,4,1,stdout);fwrite(&draws,4,1,stdout);fwrite(&seed,4,1,stdout);fwrite(s.particles.data(),92,400,stdout);}}
'''.replace('VECTOR',function((ROOT/'src/game/object/object.cpp').read_text(),'void ConvertToVector(')).replace('TICKS',str(len(rows))).replace('STRIDE',str(stride if stride else 1000000))
source=a.output/'core.cpp';source.write_text(cpp);binary=a.output/'core';subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
raw=subprocess.check_output([str(binary),str(tables)]);(a.output/'port.bin').write_bytes(raw);errors=[];checks=0;exact=0;position_differences=0;maximum=0;rng=[]
for tick,row in enumerate(rows):
 chunk=raw[tick*36812:(tick+1)*36812];ranges,draws,seed=struct.unpack_from('<3I',chunk);expected=row['rng_draws']if full else row['random_calls'];actual=draws if full else ranges
 rng.append(dict(tick=tick,expected=expected,actual=actual,match=actual==expected));assert actual==expected,(tick,actual,expected)
 if full:assert seed==row['seed']
 n=bytes.fromhex(row['pool_hex']);c=chunk[12:]
 for pos in range(0,36800,4):
  checks+=1
  if n[pos:pos+4]==c[pos:pos+4]:exact+=1;continue
  slot,field=pos//92,pos%92;x,y=struct.unpack_from('<f',n,pos)[0],struct.unpack_from('<f',c,pos)[0];delta=abs(x-y)
  if field in(0,4,8):position_differences+=1;maximum=max(maximum,delta)
  if field not in(0,4,8)or delta>2e-5:
   if len(errors)<100:errors.append(dict(tick=tick,slot=slot,field=field,native=x,port=y,delta=delta))
r=dict(status='pass'if not errors else'differences',checks=checks,exact_binary32_or_integer=exact,position_differences=position_differences,max_position_error=maximum,errors=errors,rng=rng,render_stride=stride,trace=dict(path=str(a.trace.resolve()),sha256=sha(a.trace.read_bytes())),executable_sha256=sha(a.executable.read_bytes()),source_sha256={f:sha((ROOT/f).read_bytes())for f in('src/game/effects/firewindcore.h','src/engine/core/math3d.cpp','src/game/object/object.cpp')},scope='All400 complete92-byte records after each original tick; state/life/RNG/angles/scales exact, generic matrix trig/position arithmetic tolerance2e-5 world units. Actual production ConvertToVector with original initialization table. Render scale mutations follow the declared stride; post-kill controller pulses are diagnostic only.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,'exact',exact,'position max',maximum,'errors',errors[:8]);assert not errors
