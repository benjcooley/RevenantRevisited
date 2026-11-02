"""Compare all original barrier pool fields with the compiled source core."""
import argparse
import json
import struct
import subprocess
import zipfile
from pathlib import Path

from barrier_wall_native_preflight import NativeBarrierWall, PROFILES, ROOT
from speed_profile_contract import sha


def snapshot(fixture):
    v=fixture.vm
    a=fixture.animator
    vec=lambda p:list(struct.unpack('<3f',v.uc.mem_read(p,12)))
    return dict(seed=v.rng_seed,delay=v.u32(a+0x4bc),
                bounds=list(struct.unpack('<7f',v.uc.mem_read(a+0x4c0,28))),
                flares=[vec(a+0x264+i*12)+vec(a+0xfc+i*12)+
                        [v.u32(a+0x3cc+i*4),v.u32(a+0x444+i*4)]
                        for i in range(30)])


def run(output,ticks):
    output.mkdir(parents=True,exist_ok=False)
    cases=[]
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        for profile in PROFILES:
            name,_,_,_,_=profile
            data=archive.read(next(n for n in archive.namelist()
                                  if n.lower()=='imagery/misc/'+name.lower()+'.i3d'))
            f=NativeBarrierWall(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,profile)
            f.raster_enabled=False
            rows=[snapshot(f)]
            for tick in range(ticks):
                f.advance(); f.render(); rows.append(snapshot(f))
            folder=output/name;folder.mkdir()
            vertices=[x[:3] for x in struct.iter_unpack('<8f',bytes.fromhex(f.asset['objects'][0]['vertex_hex']))]
            cpp='''#include <cstdio>
#include "barriercore.h"
unsigned seed=1;
int random_range(int lo,int hi){if(lo==hi)return lo;seed=seed*214013u+2531011u;return lo+int((seed>>16)&32767)%(hi-lo+1);}
void dump(const retail_barrier::State&s){printf("%u %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g",seed,s.delay,s.middle_y,s.upper.x,s.upper.y,s.upper.z,s.lower.x,s.lower.y,s.lower.z);for(const auto&f:s.flares)printf(" %.9g %.9g %.9g %.9g %.9g %.9g %d %d",f.position.x,f.position.y,f.position.z,f.velocity.x,f.velocity.y,f.velocity.z,f.type,f.motion);puts("");}
int main(){retail_barrier::Vec3 vertices[]={VERTICES};retail_barrier::State s;s.Initialize(vertices,sizeof(vertices)/sizeof(vertices[0]));dump(s);for(int i=0;i<TICKS;++i){s.Advance(random_range);dump(s);}}
'''.replace('VERTICES',','.join('{'+','.join(float(x).hex()+'f'for x in v)+'}' for v in vertices)).replace('TICKS',str(ticks))
            source=folder/'core.cpp';source.write_text(cpp);exe=folder/'core'
            subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),'-o',str(exe)],check=True)
            text=subprocess.check_output([str(exe)],text=True);(folder/'source.trace').write_text(text)
            assert len(text.splitlines())==len(rows)==ticks+1
            errors=[];maximum=0;fields=0
            for tick,(line,native) in enumerate(zip(text.splitlines(),rows)):
                actual=list(map(float,line.split()))
                expected=[native['seed'],native['delay']]+native['bounds']+sum(native['flares'],[])
                assert len(actual)==len(expected)==249
                for field,(a,b) in enumerate(zip(actual,expected)):
                    residual=abs(a-b);maximum=max(maximum,residual);fields+=1
                    integral=field<2 or (field>=9 and (field-9)%8>=6)
                    if residual>(0 if integral else max(1e-6,abs(b)*1e-7)):
                        errors.append(dict(tick=tick,field=field,source=a,native=b))
            (folder/'native.json').write_text(json.dumps(rows,indent=2)+'\n')
            cases.append(dict(name=name,status='pass'if not errors else'fail',fields=fields,max_error=maximum,errors=errors[:20],native_sha256=sha((folder/'native.json').read_bytes())))
    report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,core_sha256=sha((ROOT/'src/game/effects/barriercore.h').read_bytes()),scope='All30 original pool slots, copied own-resource bounds, delay and exact MSVC stream through original Animate and every intervening Render. Equal random-range endpoints return without consuming CRT RNG, matching the original integer helper. No visual credit, source renderer or natural caller claim.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report));assert report['status']=='pass'


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=600)
    a=p.parse_args();run(a.output,a.ticks)
