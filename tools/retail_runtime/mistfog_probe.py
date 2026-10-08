#!/usr/bin/env python3
"""Retail MistFog controller versus compiled production state/mesh on SW raster."""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='83022b78d13d39ea43bd7925a9e19966833e2e39af5fcad000cab4635230591c'
INDICES=(0,1,3,3,2,0)
WIDTH=384;HEIGHT=384;PIXELS=WIDTH*HEIGHT


def production_spans():
    s=(ROOT/'src/effect.cpp').read_text();h=(ROOT/'src/effect.h').read_text()
    return dict(reset=span(s,'void TFogEffect_Bespoke__MistFog::ResetBall_(','\nvoid TFogEffect_Bespoke__MistFog::Initialize'),
        advance_submit=span(s,'void TFogEffect_Bespoke__MistFog::Advance(','// =========================================================================\n// W3-C RockStorm'),
        types=span(h,'class TFogEffect_Bespoke__MistFog :','\n_CLASSDEF(TRockStormEffect_Bespoke)'),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset,inputs,ticks):
    pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
#undef RAND_MAX
#define RAND_MAX 32767
#define SMOKE_GRAV (-0.01f)
enum class EFxBlend:uint8_t {Alpha,AdditiveStraight};
enum class EFxDepthMode:uint8_t {TestNoWrite};
enum class EFxDebugMode:uint8_t {Normal,SolidColor,FullTexture};
enum class EFxLightMode:uint8_t {Lit,Unlit};
struct SQuadDrawItem {float world_pos[4][3]{};float uv[4][2]{};float color_rgba[4]{};
 struct {TTextureHandle texture{};uint8_t blend{},depth_mode{};} key;
 EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer {std::vector<SQuadDrawItem> draws;void SubmitFxQuad(const SQuadDrawItem& i){draws.push_back(i);}};
TRenderer renderer;TRenderer* Renderer=&renderer;
struct TObjectImagery {};struct SObjectDef {};
struct World {hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4& Matrix()const{return m;}};
struct TEffect {World world;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};
 virtual ~TEffect()=default;virtual void OffScreen(){};void SetCommandDone(bool){};
 const World& Transform()const{return world;}};
std::ifstream random_inputs;int random_count=0;bool in_range=false;int expected_low=0,expected_high=0;
extern "C" int rand() {int range,lo,hi,value;
 if(!(random_inputs>>range>>lo>>hi>>value)||bool(range)!=in_range||
    (in_range&&(lo!=expected_low||hi!=expected_high)))std::abort();
 ++random_count;return value;}
int random(int lo,int hi){in_range=true;expected_low=lo;expected_high=hi;
 int value=rand()%(hi-lo+1)+lo;in_range=false;return value;}
'''
    records=struct.unpack_from('<32f',asset,0xfc)
    def lit(v):
        s=format(v,'.9g');return s+'f' if '.' in s or 'e' in s else s+'.0f'
    vertices=','.join('{{'+','.join(lit(v) for v in records[i:i+3])+'},{'+','.join(lit(v) for v in records[i+3:i+6])+'},'+lit(records[i+6])+','+lit(records[i+7])+'}' for i in range(0,32,8))
    trailer=r'''
int main(int argc,char**argv){random_inputs.open(argv[1]);if(!random_inputs)return 2;
 TFogEffect_Bespoke__MistFog effect(nullptr);S3DVertex verts[]={VERTICES};
 effect.authored_vertices_.assign(verts,verts+4);effect.texture_=1;
 effect.centerx_=effect.centery_=0;for(int i=0;i<25;++i)effect.ResetBall_(i);effect.initialized_=true;
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
  std::printf("S %d %d %d %zu\n",tick,effect.ticks_,random_count,renderer.draws.size());
  for(int i=0;i<25;++i){const auto&p=effect.smoke_[i];
   std::printf("P %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",tick,i,p.life,p.x,p.y,p.z,p.rot,p.vx,p.vy,p.vz,p.size);}
  for(const auto&d:renderer.draws){std::printf("D %d",tick);
   for(int i=0;i<4;++i)std::printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);std::puts("");}
 }
 int a,b,c,d;if(random_inputs>>a>>b>>c>>d)throw std::runtime_error("Unconsumed native random inputs");
}
'''.replace('VERTICES',vertices).replace('TICKS',str(ticks))
    generated=output/'mistfog-port-component.cpp'
    generated.write_text(prelude+'\n'+types+'\n'+pieces['reset']+'\n'+pieces['advance_submit']+'\n'+trailer)
    binary=output/'mistfog-port-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),
        '-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    compile=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(compile.stdout+compile.stderr)
    if compile.returncode:raise RuntimeError('MistFog production-body compile failed; see port-compile.log')
    trace=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True)
    (output/'port-trace.txt').write_text(trace.stdout);(output/'port-run.log').write_text(trace.stderr)
    if trace.returncode:raise RuntimeError('MistFog port RNG or execution mismatch; see port-run.log')
    frames={}
    for line in trace.stdout.splitlines():
        fields=line.split();tag=fields[0];tick=int(fields[1])
        if tag=='S':frames[tick]=dict(ticks=int(fields[2]),random_count=int(fields[3]),particles=[],draws=[])
        elif tag=='P':frames[tick]['particles'].append(dict(index=int(fields[2]),life=int(fields[3]),values=[float(x) for x in fields[4:]]))
        elif tag=='D':frames[tick]['draws'].append(dict(positions=[[float(v) for v in fields[2+i*5:5+i*5]] for i in range(4)],uvs=[[float(v) for v in fields[5+i*5:7+i*5]] for i in range(4)]))
    return frames,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        compiled_production_bodies=['ResetBall_','Advance','Submit','src/math3d.cpp'],
        boundaries=['exact Misc asset vertices/UVs/texture','identity owner world','renderer records production SubmitFxQuad',
                    'strict native raw/range RNG input replay; RAND_MAX=32767 is explicit common Windows-input normalization'])


class MistFogFixture:
    def __init__(self,executable,asset):
        if sha(asset)!=ASSET_SHA:raise ValueError('Unsupported MistFog asset revision')
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT);self.vm=self.software.vm
        self.software.set_texture(64,64,asset[0x2d4:0x2d4+8192],format='RGB565')
        records=struct.unpack_from('<32f',asset,0xfc)
        self.points=[records[i:i+3] for i in range(0,32,8)];self.uvs=[records[i+6:i+8] for i in range(0,32,8)]
        self.animator=self.vm.allocate(0x600);self.owner=self.vm.allocate(0x400);vtable=self.vm.allocate(0x200)
        self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200);self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.vm.put_u32(self.owner,vtable);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vtable+0x158,done)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,
                         0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        self.random=[];self.random_pending=[];self.range_pending=[];self.packets=[]
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x48332c,0x58c582,0x58c5a3):
            self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('MistFog requested other authored mesh')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if (self.vm.u32(sp+4),self.vm.u32(sp+8))!=(8,1):raise AssertionError('MistFog blend request changed')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if obj!=self.obj or self.vm.u32(obj)!=0x48:raise AssertionError('MistFog SCL1/POS2 flags changed')
            self.packets.append(dict(flags=0x48,position=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x10,12))),
                                     scale=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x40,12)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def observe_random(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x483300:self.range_pending.append(struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8)))
        elif address==0x48332c:self.range_pending.pop()
        elif address==0x58c582:
            self.random_pending.append((1,*self.range_pending[-1]) if self.range_pending else (0,0,0))
        else:self.random.append((*self.random_pending.pop(),uc.reg_read(UC_X86_REG_EAX)))

    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[];self.range_pending=[];self.packets=[]
        self.vm.call(0x4f2840,this=self.animator)

    def animate(self):self.vm.call(0x4f2950,this=self.animator)

    def state(self):
        particles=[]
        for i in range(25):
            p=self.animator+0x110+i*40;values=struct.unpack('<9fI',self.vm.uc.mem_read(p,40))
            # Semantic order matches current production SMistPuff declaration.
            particles.append(dict(index=i,life=values[9],values=[values[j] for j in (0,1,2,8,4,5,6,3)]))
        return dict(ticks=self.vm.u32(self.animator+0xfc),random_count=len(self.random),particles=particles)

    def original_draws(self):
        self.packets=[];self.vm.call(0x4f2ae0,this=self.animator);draws=[]
        for packet in self.packets:
            self.vm.put_u32(self.obj,packet['flags']);self.vm.write(self.obj+0x10,struct.pack('<3f',*packet['position']))
            self.vm.write(self.obj+0x40,struct.pack('<3f',*packet['scale']))
            # Override SCL1/POS2 flags skip authored animation-key lookup. The
            # actual original CalcObjectMatrix performs scale and translation.
            self.vm.call(0x40a420,(self.obj,0,0,0,0),this=self.imagery)
            positions=[]
            for point in self.points:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result))
                positions.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=positions,uvs=self.uvs,**packet))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT) for p in projected):raise AssertionError('Fixture viewport clips MistFog corners')
            vertices=[(*p,31,31,31,31,*uv) for p,uv in zip(projected,draw['uvs'])]
            self.software.draw(vertices,INDICES,z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--ticks',type=int,default=240);parser.add_argument('--repeat',type=int,default=2)
    parser.add_argument('--state-only',action='store_true');args=parser.parse_args()
    if args.repeat<2 or args.ticks<1:parser.error('At least two replays and one tick are required')
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode()) for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive) as archive:asset=archive.read('Imagery/Misc/mistfog.i3d')
    fixture=MistFogFixture(args.executable,asset);fixture.reset()
    for tick in range(args.ticks):fixture.animate()
    random=fixture.random.copy();random_file=args.output/'native-random-inputs.txt'
    random_file.write_text(''.join(f'{kind} {lo} {hi} {value}\n' for kind,lo,hi,value in random))
    port,compiled=build_port(args.output,asset,random_file,args.ticks)
    samples=sorted({x for x in (0,1,2,24,48,100,150,200,args.ticks) if x<=args.ticks})
    cases=[];errors=[];timings=[];first={}
    for repeat in range(args.repeat):
        fixture.reset()
        for tick in range(args.ticks+1):
            if tick:fixture.animate()
            original=fixture.state();ported=port[tick];state_errors=[]
            if original['ticks']!=ported['ticks'] or original['random_count']!=ported['random_count']:state_errors.append('clock or random count')
            for p,q in zip(original['particles'],ported['particles']):
                if p['life']!=q['life']:state_errors.append(f'puff{p["index"]}.life')
                for field,(a,b) in enumerate(zip(p['values'],q['values'])):
                    if f32(a)!=f32(b):state_errors.append(f'puff{p["index"]}.float{field}')
            pair=None;position_error=uv_error=0;diff=None;native_draws=[]
            if tick in samples and not args.state_only:
                start=time.perf_counter();native_draws=fixture.original_draws();retail,z=fixture.pixels(native_draws)
                modern,mz=fixture.pixels(ported['draws']);timings.append((time.perf_counter()-start)*1000)
                pair=(sha(retail),sha(modern),sha(z),sha(mz))
                if repeat==0:first[tick]=pair
                elif first[tick]!=pair:raise AssertionError('MistFog warm pixels/depth changed')
                position_error=max(abs(a-b) for p,q in zip(native_draws,ported['draws']) for x,y in zip(p['positions'],q['positions']) for a,b in zip(x,y))
                uv_error=max(abs(a-b) for p,q in zip(native_draws,ported['draws']) for x,y in zip(p['uvs'],q['uvs']) for a,b in zip(x,y))
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(PIXELS)+'H',retail),struct.unpack('<'+str(PIXELS)+'H',modern)))
                if repeat==0:
                    save_rgb565_png(args.output/f'retail-{tick:03d}.png',retail,WIDTH,HEIGHT)
                    save_rgb565_png(args.output/f'port-{tick:03d}.png',modern,WIDTH,HEIGHT)
            if repeat==0:
                if state_errors or position_error>3e-5 or uv_error>1e-7 or diff:
                    errors.append(dict(tick=tick,state_errors=state_errors,max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,original_state=original,port_state=ported,state_errors=state_errors,
                    max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff,
                    image_hashes=pair,original_draws=native_draws))
        if fixture.random!=random:raise AssertionError('MistFog native RNG failed exact warm replay')
    after={k:sha(v.encode()) for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production MistFog source spans changed during comparison')
    report=dict(status='pass' if not errors else 'fail',failures=errors,retail_sha256=RETAIL_SHA,
        asset_member='Imagery/Misc/mistfog.i3d',asset_sha256=sha(asset),vertex_offset='0xfc',texture_offset='0x2d4',
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_component=compiled,
        registration='0x4f2820 -> builder0x5ad000 -> factory0x4f9850 -> vtable0x5ad004',
        original_initialize='0x4f2840',original_animate='0x4f2950',original_render='0x4f2ae0',
        original_matrix='0x40a420',original_random='0x58c582 with range wrapper0x483300',original_rng_calls=len(random),
        native_random_input_sha256=sha(random_file.read_bytes()),case_count=len(cases),sampled_pixel_ticks=[] if args.state_only else samples,
        replays_per_case=args.repeat,median_warm_pixel_pair_ms=statistics.median(timings) if timings else None,
        camera=dict(position=[0,0,0],zdist=1925,viewport=[WIDTH,HEIGHT],owner_world='identity'),
        lighting_policy='Explicit full-intensity white transformed-vertex color in both frontends; authored-normal scene illumination and hardware ONE/ONE blending remain unverified. Original RGB565 software modulation/coverage is retained, not changed to mimic GPU blending.',
        scope='Actual original MistFog Initialize/Animate/Render/CalcObjectMatrix, exact Misc RGB565 asset, strict native RNG replay, compiled production ResetBall/Advance/Submit; shared original projection/raster for selected frames. Owner/base-animator/imagery/extents interfaces are explicit fixtures. No map illumination, natural creation/deletion or modern GPU parity claim.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='cases'},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
