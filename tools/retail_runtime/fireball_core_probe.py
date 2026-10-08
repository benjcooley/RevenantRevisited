"""Generated FireBall head/history state and pixels vs original Animate prefix.

Sparks are isolated on both sides: this tests the shared head/trail stage with
an explicitly selected MSVC stream, not the complete animator's random order.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
import zipfile
from fireball_trail_probe import TrailFixture, position_tolerance
from fireball_head_probe import ROOT, ASSET_SHA, port_driver
from software_probe import save_rgb565_png


def compile_core(output):
    effect=(ROOT/'src/effect.cpp').read_text()
    stage=effect[effect.index('void TFireBallEffect::StepAnimate()'):effect.index('void TFireBallEffect::',effect.index('void TFireBallEffect::StepAnimate()')+5)]
    if 'fireball_core::Advance(fireball_, trail_'not in stage:raise AssertionError('Actual runtime no longer invokes tested helper')
    header=(ROOT/'src/effect.h').read_text()
    data=header[header.index('struct SFireBallData'):header.index('// One spark',header.index('struct SFireBallData'))]
    source=output/'port-core.cpp'
    source.write_text(r'''
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "math3d.h"
#include "fireballcore.h"
'''+data+r'''
uint32_t seed;
int draw(int low,int high){seed=214013u*seed+2531011u;return low+int((seed>>16)&32767u)%(high-low+1);}
void card(const SFireBallData&p){std::printf("{\"position\":[%.9g,%.9g,%.9g],\"scale\":%.9g,\"rotation\":%d,\"frame\":%.9g,\"glow\":%.9g}",p.pos.X,p.pos.Y,p.pos.Z,p.scale,int(p.rotation),p.frame,p.glow);}
int main(int argc,char**argv){
 if(argc!=5)return 2;seed=uint32_t(std::strtoul(argv[2],nullptr,10));
 SFireBallData head;head.scale=.6f;head.glow=1.5f;head.frame=std::atoi(argv[3]);head.rotation=std::atoi(argv[4]);
 SFireBallData trail[10];for(auto&p:trail)p.glow=0;
 FILE*f=std::fopen(argv[1],"r");if(!f)return 3;int x,y,z,tick=0;std::printf("[");
 while(std::fscanf(f,"%d %d %d",&x,&y,&z)==3){
  fireball_core::Advance(head,trail,float(x),float(y),float(z),16,3,.85f,draw);
  if(tick++)std::printf(",");std::printf("{\"seed\":%u,\"head\":",seed);card(head);std::printf(",\"trail\":[");
  for(int i=0;i<10;++i){if(i)std::printf(",");card(trail[i]);}std::printf("]}");
 }std::fclose(f);std::puts("]");
}
''')
    binary=output/'port-core'
    command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),'-o',str(binary)]
    compiled=subprocess.run(command,text=True,capture_output=True)
    (output/'core-compile.log').write_text(compiled.stdout+compiled.stderr)
    if compiled.returncode:raise RuntimeError('Core driver compilation failed')
    return binary,dict(command=command,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest())


def original_head(fixture):
    raw=fixture.vm.uc.mem_read(fixture.animator+0x4ac,28)
    x,y,z,scale,rotation,frame,glow=struct.unpack('<4fiff',raw)
    return dict(position=[x,y,z],scale=scale,rotation=rotation,frame=frame,glow=glow)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True)
    args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/newfireball.i3d')
    if hashlib.sha256(asset).hexdigest()!=ASSET_SHA:raise ValueError('Wrong authored asset')
    files=['src/fireballcore.h','src/effect.cpp','src/effect.h','src/fireballquad.h','src/fireballquad.cpp','src/math3d.cpp']
    hashes={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in files}
    core,compiled=compile_core(args.output);render,verts,render_compiled=port_driver(args.output,asset)
    fixture=TrailFixture(args.executable,asset)
    cases=[];errors=[];checks=0;pixel_differences=0;pixel_pairs=0;started=time.perf_counter()
    specs=[('range_history',1,0,0,[1000,0,128],200),('spin_wrap',42,15,358,[0,1000,128],200),
           ('frame_skip',0xffffffff,2,0,[-1000,-1000,128],72)]
    selected={1,2,5,10,20,37,50,60,61,62,65,69,70,72}
    for name,seed,frame,rotation,target,ticks in specs:
        fixture.surface.restore();fixture.missile.configure([0,0,128],target,target_hp=0,animator_present=True)
        fixture.vm.rng_seed=seed
        fixture.vm.write(fixture.animator+0x4b8,struct.pack('<fif',.6,rotation,float(frame)))
        original=[];motion=[]
        for tick in range(1,ticks+1):
            motion.append(fixture.missile.step(1)[-1]);fixture.advance_original_prefix()
            original.append(dict(seed=fixture.vm.rng_seed,head=original_head(fixture),trail=fixture.trail_state()))
        poses=args.output/(name+'-positions.txt');poses.write_text(''.join(' '.join(map(str,m['position']))+'\n'for m in motion))
        port=json.loads(subprocess.check_output([str(core),str(poses),str(seed),str(frame),str(rotation)],text=True))
        if len(port)!=ticks:raise AssertionError('Port did not produce each tick')
        max_error=0;exact_float_fields=0
        for tick,(a,b)in enumerate(zip(original,port),1):
            checks+=1
            if a['seed']!=b['seed']:errors.append(dict(case=name,tick=tick,field='seed',original=a['seed'],port=b['seed']))
            for slot,(ca,cb)in enumerate(zip([a['head']]+a['trail'],[b['head']]+b['trail'])):
                for field in ('position','scale','rotation','frame','glow'):
                    av=ca[field]if field=='position'else[ca[field]];bv=cb[field]if field=='position'else[cb[field]]
                    for component,(x,y)in enumerate(zip(av,bv)):
                        checks+=1;delta=abs(x-y);max_error=max(max_error,delta)
                        # JSON decimal roundtrip is normalized to binary32.
                        exact=struct.pack('<f',x)==struct.pack('<f',y)
                        if exact:exact_float_fields+=1
                        if not exact:errors.append(dict(case=name,tick=tick,slot=slot,field=field,component=component,original=x,port=y))
        replay=[]
        fixture.surface.restore();fixture.missile.configure([0,0,128],target,target_hp=0,animator_present=True)
        fixture.vm.rng_seed=seed;fixture.vm.write(fixture.animator+0x4b8,struct.pack('<fif',.6,rotation,float(frame)))
        pixel_reports=[]
        for tick in range(1,ticks+1):
            state=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
            replay_state=dict(seed=fixture.vm.rng_seed,head=original_head(fixture),trail=fixture.trail_state())
            if replay_state!=original[tick-1]or state!=motion[tick-1]:raise AssertionError('Original state replay changed')
            if name!='range_history'or tick not in selected:continue
            cards=fixture.trail_packets();cards=[c for c in cards if c['scale']!=0]
            candidate=port[tick-1];trail_file=args.output/'generated-trail.bin'
            trail_file.write_bytes(b''.join(struct.pack('<4fiff',*s['position'],s['scale'],s['rotation'],s['frame'],s['glow'])for s in candidate['trail']))
            head=candidate['head'];packet=json.loads(subprocess.check_output([str(render),str(verts),*map(str,state['position']),
                '0',str(state['angle']),str(head['frame']),str(head['rotation']),str(head['scale']),str(head['glow']),str(trail_file)],text=True))
            head_visible=state['state']in(0,1)
            if head_visible:
                native=original[tick-1]['head']
                glow=fixture.packet(True,int(native['frame']),native['rotation'],native['scale'],native['glow'],0)
                head_card=fixture.packet(False,int(native['frame']),native['rotation'],native['scale'],native['glow'],0)
                cards=[glow]+cards+[head_card]
            generated=packet['quads']if head_visible else packet['quads'][1:-1]
            generated=[c for c in generated if any(v!=c['positions'][0]for v in c['positions'][1:])]
            geometry_errors=0
            if len(cards)!=len(generated):geometry_errors+=1
            for a,b in zip(cards,generated):
                for field in ('positions','uvs'):
                    for x,y in zip(a[field],b[field]):
                        for u,v in zip(x,y):
                            if abs(u-v)>(position_tolerance(u)if field=='positions'else 1e-7):geometry_errors+=1
            if geometry_errors:errors.append(dict(case=name,tick=tick,field='generated_geometry',errors=geometry_errors))
            ref=fixture.pixels(cards,instruction_limit=5000000);out=fixture.pixels(generated,instruction_limit=5000000);pixels=fixture.surface.width*fixture.surface.height
            diff=sum(x!=y for x,y in zip(struct.unpack('<'+'H'*pixels,ref),struct.unpack('<'+'H'*pixels,out)))
            pixel_differences+=diff;pixel_pairs+=1
            save_rgb565_png(args.output/f'retail-{tick:03d}.png',ref,960,540);save_rgb565_png(args.output/f'port-{tick:03d}.png',out,960,540)
            pixel_reports.append(dict(tick=tick,head_glow_included=head_visible,different_pixels=diff,retail_sha256=hashlib.sha256(ref).hexdigest(),port_sha256=hashlib.sha256(out).hexdigest()))
        cases.append(dict(name=name,seed=seed,initial_frame=frame,initial_spin=rotation,source=[0,0,128],destination=target,
            ticks=ticks,original=original,port=port,motion=motion,max_float_error=max_error,exact_binary32_fields=exact_float_fields,
            exact_original_replay_ticks=ticks,pixels=pixel_reports))
    if hashes!={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in files}:raise AssertionError('Source changed during comparison')
    report=dict(status='pass'if not errors and not pixel_differences else'differences_found',cases=cases,errors=errors,
        field_checks=checks,exact_binary32_fields=sum(c['exact_binary32_fields']for c in cases),pixel_pairs=pixel_pairs,pixel_differences=pixel_differences,source_sha256=hashes,
        compiled_core=compiled,compiled_render=render_compiled,elapsed_seconds=time.perf_counter()-started,
        retail_binary_sha256=hashlib.sha256(args.executable.read_bytes()).hexdigest(),asset_sha256=ASSET_SHA,
        rng_contract='Explicit MSVC CRT stream, with spark callbacks excluded on both sides; production global native CRT stream and complete spark RNG ordering not certified.',
        original_functions=['0x510220','0x470920','0x510e40..0x511179','0x511530..0x51153b','0x511950'],
        full_animator=False,scope='Production StepAnimate shared head/history helper generates all candidate state. Original Animate prefix generates oracle state; actual original missile Pulse/Move supplies owner trajectory. Three MSVC seeds, frame skip/wrap, integer spin wrap, ten-slot decay and range-impact history drain. Exact RNG seed/frame/spin and binary32 equality for all state fields (9-digit JSON normalized to binary32); generated head/glow/trail geometry/pixels compared in 14 moving/drain captures (head/glow excluded after impact as original dispatch requires). Spark callbacks isolated/no RNG, steady .6 head scale, state-specific burst/ring/damage/reaper excluded. Shared original software raster is not Metal parity.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in ('cases','errors')},indent=2));print('errors',len(errors))
    if errors:print(json.dumps(errors[:20],indent=2))
    if errors or pixel_differences:raise SystemExit(1)


if __name__=='__main__':main()
