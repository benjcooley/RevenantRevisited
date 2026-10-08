"""Original shared FireBall spark pool/RNG before generated head/history.

Runs real Set/Animate/Create virtuals from retail, with synthetic empty backing
storage matching recovered constructor capacity40. State-specific burst/ring
remain excluded. Candidate compiles the actual StepAnimate prefix from source.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from fireball_trail_probe import TrailFixture,position_tolerance
from fireball_head_probe import ROOT,ASSET_SHA,function,port_driver
from software_probe import save_rgb565_png
from fireball_core_probe import original_head


class SparkFixture(TrailFixture):
    def __init__(self,executable,asset,**asset_offsets):
        super().__init__(executable,asset,**asset_offsets)
        self.pool=self.vm.allocate(40*72)
        self.vm.put_u32(self.animator+0x248,0x5b4308)
        self.vm.put_u32(self.animator+0x2c8,self.pool)
        self.vm.put_u32(self.animator+0x2cc,40)
        self.vm.put_u32(self.animator+0x2d0,self.animator)
        self.vm.put_u32(self.animator+0x2d4,self.obj)
        self.rng_calls=[]
        self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=0x483300,end=0x483300)
        self.surface.checkpoint()

    def observe_random(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        self.rng_calls.append(list(struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8))))

    def advance_original_prefix(self):
        self.rng_calls=[]
        super().advance_original_prefix()

    def state(self):
        records=[]
        for i in range(40):
            raw=self.vm.uc.mem_read(self.pool+i*72,72)
            records.append(dict(used=bool(struct.unpack_from('<I',raw)[0]),
                position=list(struct.unpack_from('<3f',raw,4)),velocity=list(struct.unpack_from('<3f',raw,16)),
                scale=struct.unpack_from('<f',raw,28)[0],life=struct.unpack_from('<i',raw,56)[0],
                flicker_status=bool(struct.unpack_from('<I',raw,64)[0])))
            scales=struct.unpack_from('<3f',raw,28)
            if not scales[0]==scales[1]==scales[2]:raise AssertionError('FireBall uniform-scale scope violated')
        return dict(seed=self.vm.rng_seed,rng_calls=list(self.rng_calls),head=original_head(self),trail=self.trail_state(),sparks=records,
            explode=struct.unpack('<i',self.vm.uc.mem_read(self.animator+0x360,4))[0])


def compile_prefix(output):
    effect=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    body=function(effect,'void TFireBallEffect::StepAnimate()')
    body=body[:body.index('    // --- state-specific actions')]+ '\n}\n'
    count=function(effect,'int32_t TFireBallEffect::LiveSparkCount() const')
    constants='\n'.join(line for line in header.splitlines()if line.startswith('inline constexpr')and'kFireBall'in line)
    data=header[header.index('struct SFireBallData'):header.index('_CLASSDEF(TFireBallEffect)',header.index('struct SFireBallData'))]
    source=output/'port-spark.cpp'
    source.write_text(r'''
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <vector>
#include <utility>
#include "math3d.h"
#include "fireballcore.h"
#include "fireballspark.h"
uint32_t seed;std::vector<std::pair<int,int>> rng_calls;
int random(int low,int high){rng_calls.emplace_back(low,high);if(low==high)return low;if(low>high){int t=low;low=high;high=t;}
 seed=214013u*seed+2531011u;return low+int((seed>>16)&32767u)%(high-low+1);}
struct S3DPoint {int32_t x{},y{},z{};};
'''+constants+'\n'+data+r'''
struct TFireBallEffect {
 S3DPoint position;int state_{},old_state_{},firsttime_{},explode_{},frame_count_{16},glow_frame_{3};bool status_{};
 SFireBallData fireball_,trail_[kFireBallTrailSize];SFireBallSpark sparks_[kFireBallMaxSpark];
 const S3DPoint& Pos()const{return position;}
 int32_t LiveSparkCount()const;void StepAnimate();
};
'''+count+'\n'+body+r'''
void card(const SFireBallData&p){std::printf("{\"position\":[%.9g,%.9g,%.9g],\"scale\":%.9g,\"rotation\":%d,\"frame\":%.9g,\"glow\":%.9g}",p.pos.X,p.pos.Y,p.pos.Z,p.scale,int(p.rotation),p.frame,p.glow);}
int main(int argc,char**argv){
 if(argc!=3)return 2;seed=uint32_t(std::strtoul(argv[2],nullptr,10));TFireBallEffect effect;
 effect.fireball_.scale=.6f;effect.fireball_.glow=1.5f;for(auto&p:effect.trail_)p.glow=0;
 FILE*f=std::fopen(argv[1],"r");if(!f)return 3;int tick=0;std::printf("[");
 while(std::fscanf(f,"%d %d %d %d",&effect.state_,&effect.position.x,&effect.position.y,&effect.position.z)==4){
  rng_calls.clear();effect.StepAnimate();effect.old_state_=effect.state_;
  if(tick++)std::printf(",");std::printf("{\"seed\":%u,\"explode\":%d,\"head\":",seed,effect.explode_);card(effect.fireball_);std::printf(",\"trail\":[");
  for(int i=0;i<10;++i){if(i)std::printf(",");card(effect.trail_[i]);}std::printf("],\"sparks\":[");
  for(int i=0;i<40;++i){const auto&p=effect.sparks_[i];if(i)std::printf(",");
   std::printf("{\"used\":%s,\"position\":[%.9g,%.9g,%.9g],\"velocity\":[%.9g,%.9g,%.9g],\"scale\":%.9g,\"life\":%d,\"flicker_status\":%s}",
    p.used?"true":"false",p.pos.X,p.pos.Y,p.pos.Z,p.vel.X,p.vel.Y,p.vel.Z,p.scale,p.life,
#ifdef FIREBALL_SPARK_FLICKER_STATE
    p.flicker_status?"true":"false"
#else
    (p.life&1)==0?"true":"false"
#endif
   );}
  std::printf("],\"rng_calls\":[");for(size_t i=0;i<rng_calls.size();++i){if(i)std::printf(",");std::printf("[%d,%d]",rng_calls[i].first,rng_calls[i].second);}std::printf("]}");
 }std::fclose(f);std::puts("]");
}
''')
    binary=output/'port-spark';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),'-o',str(binary)]
    if 'bool     flicker_status'in header:command.insert(1,'-DFIREBALL_SPARK_FLICKER_STATE')
    result=subprocess.run(command,text=True,capture_output=True);(output/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Actual Spark prefix compilation failed')
    return binary,dict(command=command,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest())


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--pixels',action='store_true');parser.add_argument('--producer',type=Path);parser.add_argument('--producer-manifest',type=Path)
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/newfireball.i3d')
    if hashlib.sha256(asset).hexdigest()!=ASSET_SHA:raise ValueError('Wrong authored asset')
    files=['src/effect.cpp','src/effect.h','src/fireballcore.h','src/fireballspark.h'];sources={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in files}
    if args.producer:
        if not args.producer_manifest:parser.error('--producer requires retained manifest')
        prior=json.loads(args.producer_manifest.read_text());compiled=dict(prior['compiled_port']);binary=args.producer.resolve()
        if hashlib.sha256(binary.read_bytes()).hexdigest()!=compiled['binary_sha256']:raise ValueError('Retained producer changed')
        compiled['retained_manifest']=str(args.producer_manifest.resolve());compiled['retained_source_sha256']=prior['source_sha256']
    else:binary,compiled=compile_prefix(args.output)
    fixture=SparkFixture(args.executable,asset)
    cases=[];checks=0;errors=[];started=time.perf_counter();pixel_reports=[];pixel_errors=0
    if args.pixels:render,vertices,render_compiled=port_driver(args.output,asset)
    else:render_compiled=None
    for name,seed,target,launch_delay in [('flight_range',1,[1000,0,128],0),('launch_wait',42,[0,1000,128],3),('diagonal',0xffffffff,[-1000,-1000,128],0)]:
        fixture.surface.restore();fixture.missile.configure([0,0,128],target,target_hp=0,animator_present=True,launch_ready=not launch_delay)
        fixture.vm.rng_seed=seed;original=[];motion=[]
        for tick in range(1,97):
            if tick==launch_delay+1:fixture.vm.put_u32(fixture.missile.obj+0x18c,1)
            motion.append(fixture.missile.step(1)[-1]);fixture.advance_original_prefix();original.append(fixture.state())
        file=args.output/(name+'-motion.txt');file.write_text(''.join(' '.join(map(str,[m['state'],*m['position']]))+'\n'for m in motion))
        candidate=json.loads(subprocess.check_output([str(binary),str(file),str(seed)],text=True));case_errors=0
        for tick,(a,b)in enumerate(zip(original,candidate),1):
            for field in ('seed','explode','rng_calls'):
                checks+=1
                if a[field]!=b[field]:errors.append(dict(case=name,tick=tick,field=field,original=a[field],port=b[field]));case_errors+=1
            for group in ('head','trail','sparks'):
                aa=[a[group]]if group=='head'else a[group];bb=[b[group]]if group=='head'else b[group]
                for slot,(u,v)in enumerate(zip(aa,bb)):
                    for field in u:
                        x=u[field]if isinstance(u[field],list)else[u[field]];y=v[field]if isinstance(v[field],list)else[v[field]]
                        for component,(rx,px)in enumerate(zip(x,y)):
                            checks+=1;exact=(rx==px)if field in('used','life','flicker_status','rotation','frame')else struct.pack('<f',rx)==struct.pack('<f',px)
                            if not exact:
                                errors.append(dict(case=name,tick=tick,group=group,slot=slot,field=field,component=component,original=rx,port=px));case_errors+=1
        fixture.surface.restore();fixture.missile.configure([0,0,128],target,target_hp=0,animator_present=True,launch_ready=not launch_delay)
        fixture.vm.rng_seed=seed
        for tick in range(1,97):
            if tick==launch_delay+1:fixture.vm.put_u32(fixture.missile.obj+0x18c,1)
            row=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
            if row!=motion[tick-1]or fixture.state()!=original[tick-1]:raise AssertionError('Actual Spark pool/RNG warm replay failed')
            if not args.pixels or name!='flight_range' or tick not in {1,2,5,10,20,37,50,60,61,62,65,69,70,72}:continue
            native=original[tick-1];ported=candidate[tick-1];cards=[c for c in fixture.trail_packets()if c['scale']!=0]
            head_visible=row['state']in(0,1)
            if head_visible:
                h=native['head'];cards=[fixture.packet(True,int(h['frame']),h['rotation'],h['scale'],h['glow'],0)]+cards+[fixture.packet(False,int(h['frame']),h['rotation'],h['scale'],h['glow'],0)]
            trail_file=args.output/'generated-trail.bin'
            trail_file.write_bytes(b''.join(struct.pack('<4fiff',*p['position'],p['scale'],p['rotation'],p['frame'],p['glow'])for p in ported['trail']))
            h=ported['head'];packet=json.loads(subprocess.check_output([str(render),str(vertices),*map(str,row['position']),
                '0',str(row['angle']),str(h['frame']),str(h['rotation']),str(h['scale']),str(h['glow']),str(trail_file)],text=True))
            generated=packet['quads']if head_visible else packet['quads'][1:-1]
            generated=[c for c in generated if any(v!=c['positions'][0]for v in c['positions'][1:])]
            if len(cards)!=len(generated):errors.append(dict(case=name,tick=tick,field='generated_card_count'))
            for a,b in zip(cards,generated):
                for field in('positions','uvs'):
                    for x,y in zip(a[field],b[field]):
                        for u,v in zip(x,y):
                            if abs(u-v)>(position_tolerance(u)if field=='positions'else 1e-7):errors.append(dict(case=name,tick=tick,field='generated_'+field,original=u,port=v))
            ref=fixture.pixels(cards,instruction_limit=5000000);out=fixture.pixels(generated,instruction_limit=5000000);pixels=960*540
            diff=sum(x!=y for x,y in zip(struct.unpack('<'+'H'*pixels,ref),struct.unpack('<'+'H'*pixels,out)));pixel_errors+=diff
            save_rgb565_png(args.output/f'retail-{tick:03d}.png',ref,960,540);save_rgb565_png(args.output/f'port-{tick:03d}.png',out,960,540)
            pixel_reports.append(dict(tick=tick,different_pixels=diff,head_glow_included=head_visible,retail_sha256=hashlib.sha256(ref).hexdigest(),port_sha256=hashlib.sha256(out).hexdigest()))
        cases.append(dict(name=name,seed=seed,launch_readiness_delay=launch_delay,source=[0,0,128],destination=target,ticks=96,
            original=original,port=candidate,motion=motion,errors=case_errors,exact_original_warm_replay_ticks=96,
            original_live_counts=[sum(p['used']for p in s['sparks'])for s in original],port_live_counts=[sum(p['used']for p in s['sparks'])for s in candidate]))
    if sources!={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in files}:raise AssertionError('Production changed during comparison')
    report=dict(status='pass'if not errors and not pixel_errors else'differences_found',field_checks=checks,error_count=len(errors),errors=errors,cases=cases,
        compiled_port=compiled,compiled_render=render_compiled,pixel_pairs=pixel_reports,pixel_differences=pixel_errors,
        original_random_range_calls=sum(len(s['rng_calls'])for c in cases for s in c['original']),source_sha256=sources,retail_binary_sha256=hashlib.sha256(args.executable.read_bytes()).hexdigest(),asset_sha256=ASSET_SHA,
        elapsed_seconds=time.perf_counter()-started,original_functions=['0x510220','0x470920','0x510e40..0x511179','0x50ad50','0x50adb0','0x50af40','0x483300'],
        full_animator=False,scope='Actual original shared spark Set/Animate/Create runs before original head/history stage. Synthetic empty pool capacity40 matches native constructor; three explicit MSVC streams, original missile owner trajectories, delayed launch readiness, range impact/explosion transition and spark pool lifetime drain. Candidate compiles actual production StepAnimate prefix. All slots/used/life/flicker/RNG/head/history compared strictly; no state-specific growth/burst/ring/damage/reaper, Optional rendered head/glow/trail subset uses candidate-generated state including actual shared spark RNG consumption; spark particles themselves are not rendered. No complete effect or Metal acceptance.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in('cases','errors')},indent=2));print(json.dumps(errors[:10],indent=2))
    if errors or pixel_errors:raise SystemExit(1)


if __name__=='__main__':main()
