#!/usr/bin/env python3
"""Full native speed controller versus compiled literal production simulation.

Uses exact native-decoded authored emitter poses as common-domain inputs. This
isolates controller state and render sampling; it is not a source pose/render
submission or pixels acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]
FIELDS = ('tick','slot','rng','alive','age','x','y','z','vx','vy','vz','credit',
          'lifespan','scale','red','green','blue','alpha','friction','gravity',
          'render_x','render_y','render_z','rotation_x','rotation_y','rotation_z',
          'packed_red','packed_green','packed_blue','packed_alpha','render_scale')

DRIVER = r'''
#include "authoredpartsys.h"
#include <cstdio>
#include <fstream>
#include <vector>
using namespace authored_partsys;
struct Pose{float position[3],scale[3],matrix[16];};
int main(int argc,char**argv){
 Definition d;std::string diag;if(argc!=3||!ParseDefinition(argv[1],d,diag))return 2;
 std::ifstream f(argv[2],std::ios::binary);int ticks=0,emitters=0,quality=0;
 f.read((char*)&ticks,4);f.read((char*)&emitters,4);f.read((char*)&quality,4);if(ticks<1||emitters<1)return 3;
 TickInputs in;in.owner_position={0,0,0};in.has_object_zero_origin=true;
 f.read((char*)in.object_zero_origin.data(),12);in.emitters.resize(emitters);
 std::vector<int>frames(ticks);std::vector<Pose>poses(ticks*emitters);
 for(int t=0;t<ticks;++t){f.read((char*)&frames[t],4);f.read((char*)&poses[t*emitters],sizeof(Pose)*emitters);}if(!f)return 3;
 in.ground_height=[](int,int,int){return 0;};
 auto set=[&](int tick){in.animation_frame=frames[tick];for(int index=0;index<emitters;++index){
 const auto&p=poses[tick*emitters+index];auto&e=in.emitters[index];
 for(int i=0;i<3;++i){e.position[i]=p.position[i];e.scale[i]=p.scale[i];e.origin[i]=p.matrix[12+i];}
 for(int i=0;i<16;++i)e.matrix[i]=p.matrix[i];}};
 set(0);State state;if(!state.Initialize(d,in,quality,diag))return 4;
 unsigned rng=1;RandomRange random=[&](int a,int b){rng=rng*214013u+2531011u;return a+((rng>>16)&32767u)%(b-a+1);};
 for(int tick=0;tick<ticks;++tick){set(tick);if(tick==60)in.owner_position={16,-8,4};
 state.Advance(in,random);std::vector<RenderSample>samples(state.Capacity());
 for(size_t i=0;i<state.Capacity();++i)if(state.Particles()[i].alive)samples[i]=state.SampleRender(i,random);
 for(size_t i=0;i<state.Capacity();++i){const auto&p=state.Particles()[i];
 printf("%d %zu %u %d %d",tick,i,rng,p.alive,p.age);
 for(float v:p.position)printf(" %.9g",v);for(float v:p.velocity)printf(" %.9g",v);
 printf(" %.9g %d %.9g",state.EmissionCredit(),p.lifespan,p.scale);
 for(float v:p.color)printf(" %.9g",v);printf(" %.9g %.9g %.9g",p.alpha,p.friction,p.gravity);
 if(p.alive){const auto&s=samples[i];for(float v:s.position)printf(" %.9g",v);
 for(float v:s.rotation)printf(" %.9g",v);for(float v:s.color)printf(" %.9g",v);
 printf(" %.9g %.9g",s.alpha,s.scale);}else for(int j=0;j<11;++j)printf(" 0");puts("");}
 }
}
'''


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def compare(preflight, output):
    output.mkdir(parents=True, exist_ok=True)
    native = json.loads((preflight/'retail-state-trace.json').read_text())
    asset = json.loads((preflight/'asset-triage.json').read_text())[0]
    tag = next(t['parameters'] for t in asset['tags'] if t['name']=='partsys')
    source = output/'state_driver.cpp'
    source.write_text(DRIVER)
    binary = output/'state_driver'
    command = ['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',
        '-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),
        str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)]
    built = subprocess.run(command,capture_output=True,text=True)
    (output/'compile.log').write_text(built.stdout+built.stderr)
    if built.returncode:
        raise RuntimeError('Actual production state compile failed; see compile.log')
    poses = output/'poses.bin'
    emitter_count = len(native['poses'][0].get('emitters',[native['poses'][0]]))
    pose_data = struct.pack('<3i3f',len(native['poses']),emitter_count,native.get('quality',0),*native.get('object_zero_initial_origin',[0,0,-10]))
    for p in native['poses']:
        pose_data += struct.pack('<i',p['frame'])
        for e in p.get('emitters',[p]):
            pose_data += struct.pack('<22f',*e['position'],*e['scale'],*e['matrix'])
    poses.write_bytes(pose_data)
    run = subprocess.run([str(binary),tag,str(poses)],capture_output=True,text=True)
    (output/'state_driver.log').write_text(run.stderr)
    if run.returncode:
        raise RuntimeError('Actual production state execution failed; see state_driver.log')
    (output/'production-state.csv').write_text(run.stdout)
    production = [[float(x) for x in line.split()]for line in run.stdout.splitlines()]
    if len(production)!=len(native['rows']):
        raise AssertionError('Full pool row count differs')
    errors=[];ignored=[];checks=0;max_error=0;born=set()
    for row,(a,b) in enumerate(zip(native['rows'],production)):
        if len(a)!=len(FIELDS) or len(a)!=len(b):
            raise AssertionError('Full pool field layout differs')
        if a[3]:born.add(int(a[1]))
        for field,(x,y) in enumerate(zip(a,b)):
            # Native Initialize leaves never-born scale/alpha0; production
            # Particle defaults1. Never exclude a previously-born/live slot.
            if int(a[1]) not in born and field in(13,17):
                if x!=y:ignored.append(dict(tick=int(a[0]),slot=int(a[1]),field=FIELDS[field],native=x,production=y))
                continue
            checks+=1;diff=abs(x-y);max_error=max(max_error,diff)
            tolerance=0 if field in(0,1,2,3,4,12,26,27,28)else .001
            if diff>tolerance:
                errors.append(dict(tick=int(a[0]),slot=int(a[1]),field=FIELDS[field],native=x,production=y,error=diff))
    report=dict(status='pass'if not errors else'fail',comparisons=checks,rows=len(production),
        native_capacity=native['capacity'],quality=native.get('quality',0),error_count=len(errors),errors=errors[:100],
        maximum_absolute_error=max_error,ignored_never_born_defaults=ignored,
        native_full_parser_initialize=True,original_emitter_decode_matrix=True,
        production_sources={p:sha(ROOT/p)for p in('src/authoredpartsys.cpp','src/partsysdefinition.cpp','src/authoredpartsys.h')},
        generated_source_sha256=sha(source),binary_sha256=sha(binary),probe_sha256=sha(__file__),
        sanitizers='ASan/UBSan',source_edits=False,new_rendered_ab_cases=0,accepted=False,
        scope='Whole native parser/Initialize/Pulse+live RenderSample against actual compiled production parser/State; '
        'common exact native-authored emitter poses. Native sample stops4028e2 before owner/device mesh. '
        'Original object construction/punctuation/rendercarrier, independent compiled pose decoder, full geometry/raster '
        'or real map/Metal/spell acceptance remain open.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--preflight',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();report=compare(args.preflight,args.output)
    print(json.dumps({k:v for k,v in report.items()if k not in('errors','ignored_never_born_defaults')},indent=2))
    if report['error_count']:raise SystemExit(1)
